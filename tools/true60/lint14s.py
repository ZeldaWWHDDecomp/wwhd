#!/usr/bin/env python3
"""lint14s.py [--sites] [--depth N] [FILE ...] (default: every wwhd_src/d/actor unit with a shared-world-state rule (14s) gate): a shared-world-state rule (14s)
whole-execute body must not reach a dt-scaled call.

The 14s form (`if (!t60c::end()) return ...;` at the head of an execute or of a frame-logic function) runs the original
body once per 30 Hz frame on the tick30 step, but at the converted process's dt (1/N): it does NOT set dt 1 (the 14g form
t60w::Guard does, d/t60_whole.h begin). Any call that scales by t60::dt then moves / counts 1/N of the frame
(fopAcM_posMove moved the bird by speed * 1/2 once per frame). Such an actor
belongs in the 14g form.

Static check, over-approximating: every function body of wwhd_src (comments / literals blanked) is indexed by its short
name (and its glue address, `/* ADDR */` before the definition); a body is a dt reader when it reads `t60::dt()` other
than in a `== 1` / `!= 1` test, or calls a t60c step helper (step / damp / ratio / share / frameAdd / glide*). Calls are
`name(` tokens, resolved by short name (every definition of that name), by the unit's glue (`ext ADDR ... name(`) and by
include/fn_names.h (`name ... gabi::call<R>(0xADDR`) to the definition at that address. From each 14s gate's function
the reachable set (to --depth, default 6) is searched; a reached dt reader is reported with its call path. Name
collisions can report a path the code cannot take: read the path; `// lint14s: ok REASON` on the gate line accepts it.
Gates: `if (!t60c::end()) return ...;` (the rest of the body on that path: a gate inside an if branch does not reach
its else chain) and `if (!t60c::end()) { picture } else ...` (what
follows the block). --sites adds the positive per-frame sites `if (t60c::end() ...) X` (X). Exit 1 when any unaccepted
path is found.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
SRC = os.path.join(ROOT, "wwhd_src")
sys.path.insert(0, HERE)
from scopelint import strip, blocks  # noqa: E402

GATE = re.compile(r"\bif\s*\(\s*(!?)\s*t60c::end\(\)")


def close(code, i, op, cl):
    """the index after the bracket group opening at code[i] (op), or len(code)"""
    d = 0
    for j in range(i, len(code)):
        if code[j] == op:
            d += 1
        elif code[j] == cl:
            d -= 1
            if d == 0:
                return j + 1
    return len(code)


SITES = False  # --sites: also the positive `if (t60c::end() ...) X` per-frame sites (the per-step chance rule (8) / 11), not only 14s bodies


def opener(code, i):
    """the index of the innermost '{' still open at code[i], or None"""
    d = 0
    for j in range(i - 1, -1, -1):
        if code[j] == "}":
            d += 1
        elif code[j] == "{":
            if d == 0:
                return j
            d -= 1
    return None


def is_loop(code, o):
    """the block opening at code[o] is a loop body (for / while / do)"""
    h = code[:o].rstrip()
    if re.search(r"\bdo$", h):
        return True
    if not h.endswith(")"):
        return False
    d = 0
    for j in range(len(h) - 1, -1, -1):
        if h[j] == ")":
            d += 1
        elif h[j] == "(":
            d -= 1
            if d == 0:
                return bool(re.search(r"\b(for|while)\s*$", h[:j]))
    return False


def skip_stmt(code, i):
    """the index after the statement starting at code[i] (a block, an if / else chain, or up to its ';')"""
    rest = code[i:].lstrip()
    i += len(code[i:]) - len(rest)
    if rest.startswith("{"):
        return close(code, i, "{", "}")
    m = re.match(r"(?:if|for|while|switch)\s*\(", rest)
    if m:
        j = close(code, i + m.end() - 1, "(", ")")
        j = skip_stmt(code, j)
        if rest.startswith("if"):
            r = code[j:].lstrip()
            if re.match(r"else\b", r):
                j = skip_stmt(code, j + (len(code[j:]) - len(r)) + 4)
        return j
    k = code.find(";", i)
    return len(code) if k < 0 else k + 1


def leaves(code, i, e):
    """code[i:e] (the rest of a block) has an unconditional top-level `return` (a statement of its own, not the body of
    an unbraced if / else / loop): the path does not get past the block"""
    d = 0
    for m in re.finditer(r"[{}()]|\breturn\b", code[i:e]):
        t = m.group(0)
        if t in "{(":
            d += 1
        elif t in "})":
            d -= 1
        elif d == 0:
            k = i + m.start()
            prev = code[:k].rstrip()
            if prev[-1:] in (";", "{", "}") and not re.search(r"\belse$", prev):
                return True
    return False


def reach(body, i):
    """the text reached after body[i] on the path that got there: the rest of each enclosing block, a sibling else
    branch skipped (that path did not take it, tag_light's Detector branch), a whole loop body (its next
    round), up to the end of the function body"""
    out = []
    while True:
        o = opener(body, i)
        if o is None or o == 0:
            out.append(body[i:])
            return "".join(out)
        e = close(body, o, "{", "}")
        if is_loop(body, o):
            out.append(body[o:e])
        else:
            out.append(body[i:e])
            if leaves(body, i, e - 1):  # the block always returns on this path (hookshot procShot :574)
                return "".join(out)
        j = e
        while True:  # the if's else chain: not on this path
            r = body[j:].lstrip()
            if not re.match(r"else\b", r):
                break
            j = skip_stmt(body, j + (len(body[j:]) - len(r)) + 4)
        i = j


DT1 = re.compile(r"\bt60_set_dt\s*\(\s*1(?:\.0*f?)?\s*\)")
RESTORE = re.compile(r"\bt60_set_dt\s*\(")


def dt1_off(code):
    """the code with each `t60_set_dt(1.0f)` ... next `t60_set_dt(` stretch blanked (braces and line breaks kept): calls
    there run at dt 1, the 14g dt (pt_action b_att_ang / b_att_run)"""
    out = list(code)
    pos = 0
    while True:
        m = DT1.search(code, pos)
        if not m:
            return "".join(out)
        r = RESTORE.search(code, m.end())
        end = r.start() if r else len(code)
        for i in range(m.end(), end):
            if out[i] not in "{}\n":
                out[i] = " "
        pos = end if r else len(code)


NEGIF = re.compile(r"^\s*#\s*(ifdef|ifndef)\s+T60C_NEGCTRL\b")
PPIF = re.compile(r"^\s*#\s*if")
PPELSE = re.compile(r"^\s*#\s*(else|elif)\b")
PPEND = re.compile(r"^\s*#\s*endif\b")


def negctrl_off(code):
    """the code with the T60C_NEGCTRL (negative control build) branches blanked: the lint judges the production build
    (pt_t60_b_att_run's `1.0f - t60::dt()` is NEGCTRL-only)"""
    lines = code.split("\n")
    stack = []  # per open #if: [kind, blank now]; kind 'neg' toggles at #else
    for i, ln in enumerate(lines):
        outer = any(b for _, b in stack)
        m = NEGIF.match(ln)
        if m:
            stack.append(["neg", m.group(1) == "ifdef"])
            continue
        if PPIF.match(ln):
            stack.append(["other", False])
        elif PPELSE.match(ln) and stack:
            if stack[-1][0] == "neg":
                stack[-1][1] = not stack[-1][1]
                continue
        elif PPEND.match(ln) and stack:
            stack.pop()
            continue
        if outer:
            lines[i] = " " * len(ln)
    return "\n".join(lines)


def scopes(body):
    """[(offset, the tick30-only text)] of the body's t60c::end() gates: `if (!end()) return` -> the rest of the body on
    that path (reach: an enclosing branch's else chain excluded); `if (!end()) {..}` -> what follows that block (the
    else chain and what follows); `if (end() ...) X` -> X"""
    res = []
    for g in GATE.finditer(body):
        c = close(body, body.index("(", g.start()), "(", ")")
        rest = body[c:].lstrip()
        k = c + (len(body[c:]) - len(rest))
        if g.group(1):
            if rest.startswith("return"):
                res.append((g.start(), reach(body, k)))
            elif rest.startswith("{"):
                e = close(body, k, "{", "}")
                if re.search(r"\breturn\b", body[k:e]):  # the picture block leaves: what follows is tick30-only
                    res.append((g.start(), reach(body, e)))
                else:  # the block falls through: only its else chain is tick30-only (daPt_Execute :259)
                    j = e
                    r = body[j:].lstrip()
                    if re.match(r"else\b", r):
                        j = skip_stmt(body, j + (len(body[j:]) - len(r)) + 4)
                    res.append((g.start(), body[e:j]))
        elif SITES:
            if rest.startswith("{"):
                res.append((g.start(), body[k:close(body, k, "{", "}")]))
            else:
                res.append((g.start(), rest.split(";", 1)[0]))
    return res
DT = re.compile(r"t60::dt\(\)(?!\s*[!=]=\s*1(?:\.0f?)?\b)")
HELPERS = re.compile(r"\bt60c::(step|damp|ratio|share|frameAdd|glideBegin|glideStep)\s*\(")
CALL = re.compile(r"(->|\.)?\s*\b((?:[A-Za-z_]\w*::)*)([A-Za-z_]\w*)\s*(?:<[^<>;(){}]*>)?\s*\(")
GCALL = re.compile(r"gabi::call\w*\s*(?:<[^<>;(){}]*>)?\s*\(\s*0x([0-9A-Fa-f]{8})")
FORM_NS = {"t60c", "t60", "t60w", "t60objw", "t60wc", "gabi", "std"}
NOTCALL = {"if", "while", "for", "switch", "return", "sizeof", "case", "do", "else", "new", "delete", "catch",
           "static_cast", "reinterpret_cast", "const_cast", "dynamic_cast", "decltype", "alignof", "defined", "be",
           "u8", "s8", "u16", "s16", "u32", "s32", "f32", "f64", "BOOL", "bool", "int", "float", "void"}
# gate helpers themselves and test-only predicates: they read dt only to decide whether the step is converted
BENIGN = {"on", "end", "first", "dt", "tick30", "first_step", "frameTag", "t60_set_dt"}
MARK = re.compile(r"/\* ([0-9A-Fa-f]{8}) \*/[ \t]*$")
EXT = re.compile(r"^ext ([0-9A-Fa-f]{8}) .*?\b([A-Za-z_]\w*)\(", re.M)
FN = re.compile(r"\b([A-Za-z_]\w*)\s*\(A\.\.\. a\)\s*\{\s*return gabi::call<R>\(0x([0-9A-Fa-f]{8})")


def funcs(path):
    """[(short name, qualified name, address or None, body, gate line or None, accepted)] of the file's function bodies"""
    raw = open(path, encoding="utf-8", errors="replace").read()
    code = negctrl_off(strip(raw))
    rawlines = raw.split("\n")
    out = []
    bl = blocks(code)
    classes = [(o, c, n) for o, c, k, n in bl if k in ("class", "tclass")]
    for o, c, kind, _ in bl:
        if kind != "func":
            continue
        head = code[max(0, o - 600):o]
        seg = re.split(r"[;{}]", head)[-1]
        depth, k = 0, None
        for i, ch in enumerate(seg):  # the first '(' at depth 0: the identifier before it is the name
            if ch == "(":
                if depth == 0:
                    k = i
                    break
                depth += 1
        if k is None:
            continue
        m = re.search(r"([A-Za-z_~][\w:~]*)\s*(?:<[^<>]*>)?\s*$", seg[:k])
        if not m or m.group(1).split("::")[-1] in NOTCALL:
            continue
        qual = m.group(1)
        if "::" not in qual:  # an inline member defined in its class body: Class::name
            inner = [n for co, cc, n in classes if co < o < cc]
            if inner:
                qual = inner[-1] + "::" + qual
        line = code.count("\n", 0, o - len(head) + head.rfind(seg))
        addr = None
        brace = code.count("\n", 0, o)
        for j in range(brace, max(line - 1, brace - 8), -1):  # the marker line between the previous body and this one
            mm = MARK.search(rawlines[j]) if 0 <= j < len(rawlines) else None
            if mm:
                addr = mm.group(1).upper()
                break
        body = dt1_off(code[o:c + 1])
        gs = []
        for off, text in scopes(body):
            gline = code.count("\n", 0, o + off)
            gs.append((gline, "lint14s: ok" in rawlines[gline], text))
        out.append((qual.split("::")[-1], qual, addr, body, gs))
    return out, raw


def main():
    global SITES
    args = sys.argv[1:]
    depth = 6
    if args[:1] == ["--sites"]:
        SITES = True
        args = args[1:]
    if args[:1] == ["--depth"]:
        depth = int(args[1])
        args = args[2:]
    files = []
    for dp, _, fs in os.walk(SRC):
        for f in fs:
            if f.endswith((".cpp", ".h")):
                files.append(os.path.join(dp, f))
    byname, byaddr, reader, gates, glue = {}, {}, {}, [], {}
    for p in files:
        try:
            fl, raw = funcs(p)
        except Exception:
            continue
        rel = os.path.relpath(p, ROOT)
        for m in EXT.finditer(raw):
            glue.setdefault(rel, {})[m.group(2)] = m.group(1).upper()
        if p.endswith("fn_names.h"):
            for m in FN.finditer(raw):
                glue.setdefault("*", {})[m.group(1)] = m.group(2).upper()
        for short, qual, addr, body, gs in fl:
            key = (rel, qual, addr)
            calls = {(m, q, n) for m, q, n in CALL.findall(body)
                     if n not in NOTCALL and not (q and q.split("::")[0] in FORM_NS)}
            calls |= {("@", "", a.upper()) for a in GCALL.findall(body)}  # a direct guest call: its definition
            byname.setdefault(short, []).append((key, calls))
            if addr:
                byaddr.setdefault(addr, []).append((key, calls))
            if short not in BENIGN and (DT.search(body) or HELPERS.search(body)):
                reader[key] = "t60::dt()" if DT.search(body) else "t60c helper"
            if "/d/actor/" in "/" + rel:
                for gline, ok, text in gs:
                    gc = {(m, q, n) for m, q, n in CALL.findall(text)
                          if n not in NOTCALL and not (q and q.split("::")[0] in FORM_NS)}
                    gc |= {("@", "", a.upper()) for a in GCALL.findall(text)}
                    direct = bool(DT.search(text) or HELPERS.search(text))
                    gates.append((rel, qual, gline + 1, ok, key, gc, direct))
    if args:
        want = {os.path.relpath(os.path.abspath(a), ROOT) for a in args}
        gates = [g for g in gates if g[0] in want]

    def unit(rel):
        base = os.path.splitext(os.path.basename(rel))[0]
        return {rel, "wwhd_src/include/d/actor/%s.h" % base, "wwhd_src/include/d/actor/%s_t60.h" % base}

    def targets(rel, call):
        meth, qual, name = call
        if meth == "@":
            return list(byaddr.get(name, []))
        defs = byname.get(name, [])
        if qual:  # Class::name / ns::name: the definitions so qualified
            res = [d for d in defs if d[0][1].endswith(qual + name)] or [d for d in defs if d[0][0] in unit(rel)]
        else:  # unqualified or a member call: the unit's own definitions, else a name defined at most twice
            res = [d for d in defs if d[0][0] in unit(rel)]
            owners = {d[0][1].rsplit("::", 1)[0] if "::" in d[0][1] else "" for d in defs}
            if not res and (len(defs) <= 2 and not meth or meth and len(owners) == 1):
                res = list(defs)  # a free name defined at most twice, or a member name of one class only
        for scope in ((rel,) if meth else (rel, "*")):  # a member call (-> / .) is no free function of fn_names.h
            a = glue.get(scope, {}).get(name)  # a guest call through the unit's glue / fn_names.h
            if a:
                res += byaddr.get(a, [])
        return res

    bad = 0
    for rel, qual, line, ok, key0, calls0, direct in gates:
        seen = {key0: None}
        frontier = [(key0, calls0)]
        hits = [key0] if direct else []
        if direct:
            reader[key0] = "t60::dt() / t60c helper inside the gate"
        for _ in range(depth):
            nxt = []
            for key, calls in frontier:
                for n in sorted(calls):
                    for k2, c2 in targets(key[0], n):
                        if k2 in seen:
                            continue
                        seen[k2] = key
                        if k2 in reader:
                            hits.append(k2)
                        else:
                            nxt.append((k2, c2))
            frontier = nxt
        tag = "ok (accepted)" if ok and hits else ("CLEAN" if not hits else "REACHES dt")
        print("%s:%d %s: %s" % (rel, line, qual, tag))
        for h in hits:
            path, k = [], h
            while k is not None:
                path.append("%s%s" % (k[1], " @" + k[2] if k[2] else ""))
                k = seen[k]
            print("    %s  [%s in %s]" % (" <- ".join(path), reader[h], h[0]))
        if hits and not ok:
            bad += 1
    print("14s gates: %d, reaching a dt reader: %d" % (len(gates), bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())

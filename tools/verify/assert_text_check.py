#!/usr/bin/env python3
"""assert_text_check.py: string literals of glued sources against the original's strings.

usage: assert_text_check.py [SRC.cpp ...] [--all] [--tsv OUT] [--summary]

A glued source passes a host string literal to a guest call (JUT_ASSERT_fail, OSReport, resource names,
...); gabi::guest_str replaces it by the guest address of its `str` registration, looked up by the
literal's bytes (pinned `in FUNC` first, then the file's unpinned lines). The guest only ever sees the
image string at that address: the literal is a lookup key. autoglue checks a registration's text against
the image unless the line says `renamed`, so `renamed` lines (and literals no line registers) can carry
text the original does not have. This tool checks, per source:

  1. every literal inside a marked function (`/* ADDR */`): the registration guest_str would pick while
     ADDR runs, and the literal's bytes against the image string there, byte for byte:
       TEXT      bytes differ (`renamed` lines);
       NULLSPELL the source writes NULL where the image has (0) (autoglue tolerates it);
       ENC       same text, other encoding (UTF-8 Japanese / \\u00XX for Shift-JIS; autoglue tolerates it);
       AMBIG     several unpinned registrations with these bytes at different addresses (guest_str stops);
       DUPPIN    several `in FUNC` lines with these bytes at different addresses: every site gets the last one
                 (information: identical text, only the pointer may differ from the original's at a site);
       UNREG     no registration at all (gabi::guest_str stops the run if the literal reaches a guest call);
       ADDR      the registration's address is not an address the original function ADDR forms
                 (lis/addi), i.e. not the string the original loads at that site;
  2. every JUT_ASSERT_fail(file, line, msg) call in a marked function against the original's calls to
     JUTAssertion::showAssert (0273AA24) in that function: the call with the same msg address must exist
     and pass the same line (r4) and file address (r3) (LINE / FILE / NOCALL);
  3. every registration with source text (`str ADDR "..."`) against the image (TEXT/NULLSPELL/ENC), and
     registrations no literal of the file uses (UNUSED, information only).
Literals outside marked functions (file-scope tables, unmarked helpers) are resolved against all of the
file's registrations with the same bytes and reported only when none has the literal's exact text.

Output: one line per finding `CLASS file:line function "ours" | "original" @ADDR`, then a per-class count;
--tsv writes the same as a table, --summary prints the counts per unit only. Exit status 1 when a TEXT,
NULLSPELL, UNREG, ADDR, LINE, FILE or NOCALL finding exists (ENC, UNUSED and UNSURE are information;
UNSURE: only original calls whose arguments the propagation could not pin fit the source call).
Needs build/verify/image.bin and build/gen (function starts) of this tree. Read-only.
"""
import argparse
import os
import re
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import autoglue as ag  # noqa: E402

ASSERT_TARGET = 0x0273AA24  # JUTAssertion::showAssert (wwhd_src/include/bindings.h JUT_ASSERT_fail)
FAIL_CLASSES = ("AMBIG", "TEXT", "NULLSPELL", "UNREG", "ADDR", "LINE", "FILE", "NOCALL")

# ------------------------------------------------------------------ original side
_starts = None


def func_starts():
    global _starts
    if _starts is None:
        gen = os.path.join(ROOT, "build", "gen")
        out = subprocess.run("grep -ho '^void f_[0-9A-F]\\{8\\}(Cpu' code_*.c", shell=True, cwd=gen,
                             capture_output=True, text=True).stdout
        _starts = sorted({int(x, 16) for x in re.findall(r"f_([0-9A-F]{8})\(", out)})
        if not _starts:
            sys.exit("assert_text_check: no functions in build/gen")
    return _starts


def func_end(f):
    import bisect
    s = func_starts()
    i = bisect.bisect_right(s, f)
    return s[i] if i < len(s) else f + 0x4000


def s16(v):
    return v - 0x10000 if v & 0x8000 else v


_X31_DEST_A = {28, 60, 124, 284, 316, 412, 444, 476, 24, 536, 792, 824, 922, 954, 26}
_X31_NODEST = {0, 32, 4, 86, 54, 278, 246, 470, 1014, 982, 467, 144, 598, 854, 151, 183, 215, 247, 407, 439, 663,
               695, 727, 759, 983, 662, 918, 150, 725, 661}
_X31_UPD = {183, 247, 439, 695, 759, 55, 119, 311, 375, 567, 631}  # update forms: rA changes


def _step(w, pc, reg, f, end, formed, calls, his):
    """one instruction's effect on the known register values (reg: {r: value}); records formed addresses
    and calls"""
    op = w >> 26
    d, a, b = (w >> 21) & 31, (w >> 16) & 31, (w >> 11) & 31
    imm = w & 0xFFFF
    if op == 15:  # addis / lis
        base = 0 if a == 0 else reg.get(a)
        reg[d] = None if base is None else (base + (s16(imm) << 16)) & 0xFFFFFFFF
    elif op in (12, 13, 14):  # addic / addic. (GHS forms r0-based addresses with them) / addi / li
        base = (0 if op == 14 else reg.get(0)) if a == 0 else reg.get(a)
        reg[d] = None if base is None else (base + s16(imm)) & 0xFFFFFFFF
        if reg[d] is not None and (a != 0 or op != 14):
            formed.add(reg[d])
        elif base is None and len(his.get(a, ())) == 1:
            formed.add((next(iter(his[a])) + s16(imm)) & 0xFFFFFFFF)
    elif op == 24:  # ori rA, rS, UIMM
        reg[a] = None if reg.get(d) is None else reg[d] | imm
        if reg[a] is not None:
            formed.add(reg[a])
    elif op == 18:  # b / bl
        li = w & 0x03FFFFFC
        if li & 0x02000000:
            li -= 0x04000000
        tgt = (li if w & 2 else pc + li) & 0xFFFFFFFF
        if w & 1 or not (f <= tgt < end):
            calls.append((pc, tgt, {r: reg.get(r) for r in range(3, 11)}))
        if w & 1:
            for r in (0,) + tuple(range(3, 13)):
                reg.pop(r, None)
    elif op == 31:
        xo = (w >> 1) & 0x3FF
        if xo == 444 and d == b:  # mr rA, rS
            reg[a] = reg.get(d)
        elif xo in _X31_DEST_A:
            reg.pop(a, None)
        elif xo in _X31_NODEST:
            pass
        else:
            reg.pop(d, None)
        if xo in _X31_UPD:
            reg.pop(a, None)
    elif op in (7, 8) or 32 <= op <= 35 or 40 <= op <= 43:
        if op >= 32:  # a load: its address is formed too (an inlined copy reads the string word by word)
            base = reg.get(a) if a else 0
            if base is None and len(his.get(a, ())) == 1:
                base = next(iter(his[a]))
            if base is not None and a:
                formed.add((base + s16(imm)) & 0xFFFFFFFF)
        reg.pop(d, None)
        if op in (33, 35, 41, 43):
            reg.pop(a, None)
    elif op in (20, 21, 23, 25, 26, 27, 28, 29):
        reg.pop(a, None)
    elif op == 46:  # lmw
        for r in range(d, 32):
            reg.pop(r, None)
    elif op in (37, 39, 45, 47, 49, 51, 53, 55):
        reg.pop(a, None)
    elif op == 19 and ((w >> 1) & 0x3FF) in (16, 528) and w & 1:  # bclrl / bcctrl
        for r in (0,) + tuple(range(3, 13)):
            reg.pop(r, None)
    for k in [k for k, v in reg.items() if v is None]:
        del reg[k]


def _succ(w, pc, f, end):
    """successor addresses inside the function"""
    op = w >> 26
    nxt = [pc + 4] if pc + 4 < end else []
    if op == 18 and not w & 1:
        li = w & 0x03FFFFFC
        if li & 0x02000000:
            li -= 0x04000000
        tgt = (li if w & 2 else pc + li) & 0xFFFFFFFF
        return [tgt] if f <= tgt < end else []
    if op == 16:
        bd = w & 0xFFFC
        if bd & 0x8000:
            bd -= 0x10000
        tgt = (bd if w & 2 else pc + bd) & 0xFFFFFFFF
        bo = (w >> 21) & 31
        out = [tgt] if f <= tgt < end else []
        return out if (bo & 0x14) == 0x14 and not w & 1 else out + nxt
    if op == 19 and ((w >> 1) & 0x3FF) in (16, 528) and not w & 1:
        bo = (w >> 21) & 31
        return [] if (bo & 0x14) == 0x14 else nxt
    return nxt


def scan_original(f):
    """(addresses the function forms with lis/addi/ori, [(call addr, target, {reg: value})]): forward constant
    propagation over the function's control-flow graph (a value is known at a point when every path gives it)"""
    end = func_end(f)
    code = ag.img_read(f, end - f)
    if code is None:
        return set(), []
    words = struct.unpack(">%dI" % (len(code) // 4), code)
    # every lis value each register gets anywhere in the function (fallback for formed addresses)
    his = {}
    for w in words:
        if w >> 26 == 15 and (w >> 16) & 31 == 0:
            his.setdefault((w >> 21) & 31, set()).add((s16(w & 0xFFFF) << 16) & 0xFFFFFFFF)
    state_in = {}
    TOP = object()
    # roots: the entry, then (jump-table targets: bctr has no static successors) each address no path reached yet,
    # in address order, with nothing known
    for root in range(f, end, 4):
        if root in state_in:
            continue
        state_in[root] = {}
        work = [root]
        while work:
            pc = work.pop()
            reg = dict(state_in[pc])
            w = words[(pc - f) // 4]
            _step(w, pc, reg, f, end, set(), [], his)
            for s in _succ(w, pc, f, end):
                old = state_in.get(s, TOP)
                new = reg if old is TOP else {k: v for k, v in old.items() if reg.get(k) == v}
                if old is TOP or new != old:
                    state_in[s] = new
                    work.append(s)
    formed, calls = set(), []
    for pc in sorted(state_in):
        reg = dict(state_in[pc])
        _step(words[(pc - f) // 4], pc, reg, f, end, formed, calls, his)
    return formed, calls


def cstr_or_none(a):
    if a is None:
        return None
    try:
        return ag.img_cstr(a)
    except ag.GlueError:
        return None


# ------------------------------------------------------------------ source side
LIT = re.compile(r'"(x*)"')
STATIC_TABLE = re.compile(r"\bstatic\b[^;(){}]*\[[^;]*\]\s*=\s*\{[^;]*$")


def literals(text, code, lo, hi):
    """[(start, end, bytes)] of the string literals in code[lo:hi], adjacent literals joined"""
    out = []
    for m in LIT.finditer(code, lo, hi):
        s, e = m.start(), m.end()
        ls = code.rfind("\n", 0, s) + 1
        if code[ls:s].lstrip().startswith("#") or code[max(0, s - 7):s].rstrip().endswith("extern") \
                or re.search(r"\b(?:u8|u|U|L|R)$", code[max(0, s - 2):s]):
            continue
        if HOST_ARRAY.search(code[max(0, s - 120):s]) or any(b <= s < en for b, en in disabled_ranges(code)):
            continue  # a host char array (host-side compare), or code the preprocessor drops
        body = ag.c_str_bytes(text[s + 1:e - 1])
        if out and code[out[-1][1]:s].strip() == "":
            ps, _, pb = out[-1]
            out[-1] = (ps, e, pb + body)
        else:
            out.append((s, e, body))
    return out


HOST_ARRAY = re.compile(r"\bchar\s+\w+\s*\[\s*\w*\s*\]\s*=\s*$")
DROPPED_MACROS = ("NO_GETENV",)  # defined by the unit's headers (wwhd_src/include/libjpeg/jmemmgr.h): `#ifndef` blocks drop out
_dis_cache = {}


def disabled_ranges(code):
    if _dis_cache.get("code") is not code:
        out = []
        for m in re.finditer(r"^[ \t]*#\s*(?:ifndef\s+(%s)\b|if\s+0\b)" % "|".join(DROPPED_MACROS), code, re.M):
            depth, i = 0, m.start()
            for dm in re.finditer(r"^[ \t]*#\s*(if|ifdef|ifndef|endif|else)\b", code[m.start():], re.M):
                if dm.group(1).startswith("if"):
                    depth += 1
                elif depth == 1 and dm.group(1) in ("endif", "else"):
                    i = m.start() + dm.start()
                    break
                elif dm.group(1) == "endif":
                    depth -= 1
            out.append((m.start(), i))
        _dis_cache.update(code=code, out=out)
    return _dis_cache["out"]


def classify(host, content):
    if host == content:
        return None
    if ag.same_text(host, content):
        return "ENC"
    if ag.same_text(host.replace("NULL", "(0)"), content):
        return "NULLSPELL"
    return "TEXT"


def show(s):
    return '"%s"' % s.encode("latin1").decode("utf-8", "backslashreplace").replace("\n", "\\n")


def split_args(code, open_paren):
    close = ag.match_close(code, open_paren, "(", ")")
    parts, depth, start = [], 0, open_paren + 1
    for i in range(open_paren + 1, close):
        c = code[i]
        if c in "([{":
            depth += 1
        elif c in ")]}":
            depth -= 1
        elif c == "," and depth == 0:
            parts.append((start, i))
            start = i + 1
    parts.append((start, close))
    return parts


def int_lit(t):
    t = t.strip()
    m = re.fullmatch(r"(0[xX][0-9A-Fa-f]+|\d+)[uUlL]*", t)
    return int(m.group(1), 0) if m else None


def check_file(path, findings, stats):
    rel = os.path.relpath(path, ROOT)
    text = open(path, encoding="utf-8", errors="surrogateescape").read()
    try:
        g = ag.parse_glue(text)
    except ag.GlueError as e:
        findings.append(("GLUEERR", rel, 0, "-", str(e), "", None))
        return
    if g is None:
        return
    strs, _datas, _exts, glue_pos = g
    code = ag.mask(text[:glue_pos])
    lineof = lambda p: text.count("\n", 0, p) + 1  # noqa: E731
    used = set()
    funcs = []
    for m in ag.MARKER.finditer(text[:glue_pos]):
        fa = int(m.group(1), 16)
        par = code.find("(", m.end())
        try:
            close = ag.match_close(code, par, "(", ")")
            brace = code.find("{", close)
            end = ag.match_close(code, brace, "{", "}")
        except ag.GlueError:
            continue
        funcs.append((fa, brace, end))
    spans = sorted((b, e) for _, b, e in funcs)

    def emit(cls, pos, fn, ours, orig, addr):
        findings.append((cls, rel, lineof(pos), "%08X" % fn if fn else "-", ours, orig, addr))

    for fa, lo, hi in funcs:
        formed, calls = scan_original(fa)
        lits = literals(text, code, lo, hi)
        stats["literals"] += len(lits)
        resolved = {}
        for s, e, host in lits:
            if STATIC_TABLE.search(code, lo, s) and STATIC_TABLE.search(code[lo:s]):
                continue  # a function-local static table: a data view, its initialiser is autoglue's check
            pinned = [x for x in strs if x[2] == fa and x[1] == host]
            cands = pinned or [x for x in strs if x[2] == 0 and x[1] == host]
            if not cands:
                emit("UNREG", s, fa, show(host), "", None)
                continue
            # the registry list runs from the last registration to the first: guest_str takes the last pinned line
            addr = cands[-1][0] if pinned else cands[0][0]
            if len({x[0] for x in cands}) > 1:
                # unpinned: guest_str stops ("ambiguous string in one source file"); pinned: every site of the
                # function gets the last line's address, while the original may load another copy at some site
                emit("DUPPIN" if pinned else "AMBIG", s, fa, show(host), " ".join("@%08X" % x[0] for x in cands), addr)
            used.update((x[0], x[1], x[2]) for x in cands)
            resolved[s] = addr
            content = ag.img_cstr(addr)
            cls = classify(host, content)
            if cls:
                emit(cls, s, fa, show(host), show(content), addr)
            if addr not in formed and addr - 1 not in formed:  # addr-1: an inlined strcmp's lbzu loop
                emit("ADDR", s, fa, show(host), show(content), addr)
        # JUT_ASSERT_fail(file, line, msg) against the original's showAssert calls
        oc = [c for c in calls if c[1] == ASSERT_TARGET]
        for m in re.finditer(r"\bJUT_ASSERT_fail\s*\(", code[lo:hi]):
            p = lo + m.end() - 1
            args = split_args(code, p)
            if len(args) != 3:
                continue
            stats["asserts"] += 1
            fl = [r for r in resolved if args[0][0] <= r < args[0][1]]
            ml = [r for r in resolved if args[2][0] <= r < args[2][1]]
            line = int_lit(code[args[1][0]:args[1][1]])
            if not ml:
                continue
            maddr = resolved[ml[0]]
            # candidates: the original's calls with this msg address, plus calls whose r5 the propagation could
            # not pin (paths that merge into one shared call with different values)
            match = [c for c in oc if c[2].get(5) in (maddr, None)]
            fit = [c for c in match if c[2].get(4) in (line, None) or line is None]
            fit = [c for c in fit if not fl or c[2].get(3) in (resolved[fl[0]], None)]
            if fit:
                if not any(c[2].get(5) == maddr and c[2].get(4) == line and (not fl or c[2].get(3) == resolved[fl[0]])
                           for c in fit):
                    emit("UNSURE", lo + m.start(), fa, "msg @%08X line %s" % (maddr, "0x%X" % line if line is not None else "?"),
                         "only calls with unresolved arguments fit", maddr)
                continue
            if not match:
                emit("NOCALL", lo + m.start(), fa, "msg @%08X" % maddr, "no showAssert call with r5=%08X" % maddr, maddr)
                continue
            if line is not None and not any(c[2].get(4) in (line, None) for c in match):
                emit("LINE", lo + m.start(), fa, "line 0x%X" % line,
                     "line " + ",".join("0x%X" % c[2][4] if c[2].get(4) is not None else "?" for c in match), maddr)
            elif fl:
                emit("FILE", lo + m.start(), fa, "file @%08X" % resolved[fl[0]],
                     "file " + ",".join("@%08X" % c[2][3] if c[2].get(3) is not None else "?" for c in match), maddr)
    # literals outside marked functions
    for s, e, host in literals(text, code, 0, glue_pos):
        if any(b <= s < en for b, en in spans):
            continue
        cands = [x for x in strs if x[1] == host]
        if not cands:
            continue  # file-scope text (tables are checked by autoglue's initialiser check), not a guest_str key
        used.update((x[0], x[1], x[2]) for x in cands)
        if not any(ag.img_cstr(x[0]) == host for x in cands):
            content = ag.img_cstr(cands[0][0])
            cls = classify(host, content)
            if cls:
                emit(cls, s, 0, show(host), show(content), cands[0][0])
    # registrations
    gm = re.search(r"/\* WWHD glue", text)
    for addr, host, fn in strs:
        stats["registrations"] += 1
        if (addr, host, fn) not in used:
            ln = None
            for lm in re.finditer(r"^[ \t]*str\s+%08X\b.*$" % addr, text[gm.start():], re.M | re.I):
                ln = gm.start() + lm.start()
                break
            emit("UNUSED", ln or gm.start(), fn, show(host), show(ag.img_cstr(addr)), addr)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("srcs", nargs="*")
    ap.add_argument("--all", action="store_true", help="every glued source under wwhd_src")
    ap.add_argument("--tsv")
    ap.add_argument("--summary", action="store_true")
    ap.add_argument("--quiet-info", action="store_true", help="omit ENC and UNUSED lines (counts kept)")
    a = ap.parse_args()
    srcs = list(a.srcs)
    if a.all:
        out = subprocess.run(["git", "grep", "-l", "/\\* WWHD glue", "--", "wwhd_src"], cwd=ROOT,
                             capture_output=True, text=True).stdout.split()
        srcs += [os.path.join(ROOT, p) for p in out]
    if not srcs:
        ap.error("no sources (give SRC.cpp or --all)")
    findings, stats = [], {"literals": 0, "asserts": 0, "registrations": 0}
    for p in srcs:
        check_file(os.path.abspath(p), findings, stats)
    findings.sort(key=lambda x: (x[1], x[2], x[0]))
    counts = {}
    per_file = {}
    for f in findings:
        counts[f[0]] = counts.get(f[0], 0) + 1
        per_file.setdefault(f[1], {}).setdefault(f[0], 0)
        per_file[f[1]][f[0]] += 1
    if a.summary:
        for fn in sorted(per_file):
            c = per_file[fn]
            if any(k in FAIL_CLASSES for k in c):
                print("%s %s" % (fn, " ".join("%s=%d" % kv for kv in sorted(c.items()))))
    else:
        for cls, rel, line, fn, ours, orig, addr in findings:
            if a.quiet_info and cls in ("ENC", "UNUSED", "UNSURE", "DUPPIN"):
                continue
            print("%-9s %s:%d %s %s | %s%s" % (cls, rel, line, fn, ours, orig, " @%08X" % addr if addr else ""))
    if a.tsv:
        with open(a.tsv, "w") as o:
            o.write("class\tfile\tline\tfunction\tours\toriginal\taddress\n")
            for cls, rel, line, fn, ours, orig, addr in findings:
                o.write("\t".join([cls, rel, str(line), fn, ours, orig, "%08X" % addr if addr else ""]) + "\n")
    fails = sum(v for k, v in counts.items() if k in FAIL_CLASSES or k == "GLUEERR")
    print("assert_text_check: %d sources, %d literals in marked functions, %d JUT_ASSERT_fail calls, %d registrations; %s; "
          "%d failing findings in %d files" % (len(srcs), stats["literals"], stats["asserts"], stats["registrations"],
                                               " ".join("%s=%d" % kv for kv in sorted(counts.items())) or "no findings",
                                               fails, sum(1 for c in per_file.values() if any(k in FAIL_CLASSES for k in c))))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

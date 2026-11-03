#!/usr/bin/env python3
"""Audit a landed mutation ledger against its receipts (2026-11-02, after J3D-HDREW#1; replaces the identity part of the
campaign script integrate_receipts.py, which checked every receipt against ONE pinned source hash and logged and skipped the
rest: after a source change its proof tables could not match, and stale receipts kept their rows' classes unchecked).

    receipt_audit.py --ledger TSV --receipts DIR [--receipts DIR ...] [--root DIR] [--versions REPO:REF ...]
                     [--idmap DIRSUBSTR=TSV ...] [--exclude DIRNAME=REASON ...] [--exclude-spec SPEC=REASON ...]
                     [--ruled-basis PREFIX ...] [--object-pin [--tools-root DIR]] [--function-pin]
                     [--gate-min-n 1000] [--gate-seeds 1,7] [--out-receipts TSV] [--out-rows TSV]

Ledger: one row per mutant (global_id, function, helper_callers, source, span_start, span_end, original, replacement,
mutation, final_class, class_source). Spans refer to the source under --root (the landed tree).

Every `mutant*.json` under the receipt roots gets exactly one status (nothing is skipped silently; --out-receipts lists all):
  excluded      an --exclude-receipt file, a path component that is an --exclude directory, or a run of an --exclude-spec unit
                spec (the reason is listed)
  unreadable    not a JSON receipt
  unjoined      no ledger row has this function (or a helper caller) and this mutation text (the ` @file:line` token
                dropped); with an --idmap for its directory: the mapped row instead, and a mutation-text mismatch is an error
  no-companion  the receipt has no mutantN.cpp, so the text it compiled is unknown (never backs a row)
  current       its mutantN.cpp is the landed source with the row's span replaced by the row's replacement
  stale         its mutantN.cpp is an EARLIER version of the file (from --versions; the receipt names that version's blob
                and commit) with one edit `original` -> `replacement`: evidence about that version only
  error         joined, but its mutantN.cpp matches neither the landed source nor any earlier version, or it is the landed
                source with the edit at another offset than the ledger's span (a stale span in the ledger)
Credited rows (final_class D, EQ, RI, NAMED_GAP, SYNC) need a current receipt of their class that passes the gate (at
least --gate-min-n inputs, an input seed of --gate-seeds): D a `difference` with a clean witness (cascade.kind / row_kills, so -nulltrap and the zero-page rule apply),
the other classes `survived` or `equivalent-identical-code`; or a ruled basis named in the row's class_source (an
--ruled-basis prefix: a capture table, a written ruling; listed per row as `basis`). With --function-pin, a stale receipt of
the row's class whose version has the row's function text (mutate.bodies) identical to the landed one backs it as `pin`
(a report only: a header or struct change elsewhere can change the code). --object-pin backs such a row as `pin` only when
its function (and its helper callers) compile to identical object code in the receipt's version and the landed one
(ObjectPin: per function, raw bytes, relocations and the local code and data they reference). An EQ, NAMED_GAP or RI row with a current clean difference in the gate is CONTRADICTED, unless --ruled-difference names it
with a reason (then the difference is listed as `contradicted_ruled`, and the ruling backs the row as `ruled-difference`
when nothing else does; a named row without such a difference is an error, so
stale exceptions show).
An UNBACKED or CONTRADICTED credited row, an `error` receipt or an --idmap mismatch makes the exit status 1, with the rows
and receipts listed. Self-test:
receipt_audit_test.py."""
import argparse
import collections
import csv
import hashlib
import shutil
import tempfile
import json
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import cascade  # noqa: E402

_bodies_cache = {}


def function_text(text, func):
    """the code of FUNC in TEXT as mutate.py cuts it (the verified function with the helpers defined before it), or None"""
    key = (hash(text), len(text))
    if key not in _bodies_cache:
        import mutate
        try:
            _bodies_cache[key] = {"%08X" % a: text[s:e] for a, s, e in (mutate.bodies(text) or [])}
        except Exception:
            _bodies_cache[key] = {}
    return _bodies_cache[key].get(func.upper())

CREDITED = ("D", "EQ", "RI", "NAMED_GAP", "SYNC")
CONTRADICTABLE = ("EQ", "NAMED_GAP", "RI")  # a current clean difference contradicts these classes
SURVIVOR_KINDS = ("survived", "equivalent-identical-code")
SUFFIX = re.compile(r" @[^ ]+:\d+(?= |$)")  # the receipt's ` @file:line`, also before a ` (helper NAME)` tail
csv.field_size_limit(1 << 30)


def receipt_spec(j):
    """the unit spec a receipt's run used, from its command (`-spec PATH`: a unit file's name without .txt, or the build
    directory of an expanded spec), or "" """
    cmd = j.get("cmd", j.get("command", []))
    if isinstance(cmd, str):
        try:
            import ast
            cmd = ast.literal_eval(cmd)
        except (ValueError, SyntaxError):
            cmd = cmd.split()
    if not isinstance(cmd, list) or "-spec" not in cmd or cmd.index("-spec") + 1 >= len(cmd):
        return ""
    sp = str(cmd[cmd.index("-spec") + 1]).rstrip("/")
    if os.path.basename(sp) == "spec.expanded.txt":
        return os.path.basename(os.path.dirname(sp))
    b = os.path.basename(sp)
    return b[:-4] if b.endswith(".txt") else b


def norm_mutation(m):
    return SUFFIX.sub("", (m or "").strip())


def read_text(path):
    with open(path, "rb") as f:
        return f.read().decode("utf-8", errors="surrogateescape")


def common_prefix(a, b):
    lo, hi = 0, min(len(a), len(b))
    while lo < hi:
        mid = (lo + hi + 1) // 2
        if a[:mid] == b[:mid]:
            lo = mid
        else:
            hi = mid - 1
    return lo


def one_edit(base, variant, original, replacement, at=None):
    """the offsets k where variant == base[:k] + replacement + base[k + len(original):] (at: only that offset)"""
    lo, lr = len(original), len(replacement)
    if len(variant) - len(base) != lr - lo:
        return []
    ks = [at] if at is not None else range(max(0, common_prefix(base, variant) - max(lo, lr)), common_prefix(base, variant) + 1)
    out = []
    for k in ks:
        if base[k:k + lo] == original and variant[k:k + lr] == replacement and base[:k] == variant[:k] and \
                base[k + lo:] == variant[k + lr:]:
            out.append(k)
    return out


class ObjectPin:
    """`--object-pin`: does the row's function compile to identical object code in a receipt's version and in the landed
    version? Each version is compiled as verify.py compiles a unit source (the landed autoglue, verify.FP, -O2, the
    landed runtime/tools include directories) against that version's own wwhd_src/include (`git archive`), so a struct
    or header change shows. Per function: the disassembly with raw bytes and relocations of every symbol the VERIFY line
    names for the address (overloads all), and, recursively, of the local code and the bytes of the local data they
    reference (static helpers, string literals, constant tables; local names replaced by their contents' hash, so a
    renumbered literal still matches). A version that does not compile with the landed tools does not pin."""

    def __init__(self, tools_root, work):
        self.tools_root, self.work = tools_root, work
        self.objs, self.incs, self.notes = {}, {}, []
        import verify
        self.fp = verify.build_flags()

    def include_tree(self, repo, commit):
        if (repo, commit) not in self.incs:
            d = os.path.join(self.work, "inc-" + commit[:12])
            if not os.path.isdir(d):
                os.makedirs(d)
                arc = subprocess.run(["git", "-C", repo, "archive", commit, "wwhd_src/include"], capture_output=True)
                subprocess.run(["tar", "-x", "-C", d], input=arc.stdout, capture_output=True)
            self.incs[(repo, commit)] = os.path.join(d, "wwhd_src", "include")
        return self.incs[(repo, commit)]

    def compile(self, key, src, text, wwhd_inc):
        if key in self.objs:
            return self.objs[key]
        import autoglue
        d = os.path.join(self.work, "obj-%d" % len(self.objs))
        os.makedirs(d)
        try:
            glued = autoglue.transform(text, src) if "/* WWHD glue" in text else text
        except Exception as e:
            self.objs[key] = (None, "autoglue: %s" % e)
            return self.objs[key]
        cpp = os.path.join(d, os.path.basename(src))
        open(cpp, "w", encoding="utf-8", errors="surrogateescape").write(glued)
        obj = cpp + ".o"
        T = self.tools_root
        inc = ["-I" + os.path.join(T, "runtime/include"), "-I" + os.path.join(T, "tools/verify/include"),
               "-I" + os.path.join(T, "tools/verify/src"), "-I" + wwhd_inc, "-I" + os.path.join(T, "build/gen")]
        r = subprocess.run(["clang++", "-std=c++20", "-O2", "-w"] + self.fp + inc + ["-c", cpp, "-o", obj], capture_output=True, text=True)
        self.objs[key] = (obj, glued) if r.returncode == 0 else (None, "does not compile: " + (r.stderr.strip().splitlines() or ["?"])[-1][:160])
        return self.objs[key]

    @staticmethod
    def _syms(obj):
        out = subprocess.run(["objdump", "-t", obj], capture_output=True, text=True).stdout
        syms = []
        for l in out.splitlines():
            f = l.split()
            if len(f) >= 4 and re.match(r"[0-9a-f]{16}$", f[0]):
                syms.append((int(f[0], 16), f[1], f[-2], f[-1]))  # addr, scope (l/g), section, name
        return syms

    def fingerprint(self, obj, glued, func):
        """the function's code closure as text, or None when no symbol is found"""
        names = re.findall(r"VERIFY\(0x%s,\s*&?([^)]+?)\);" % func.upper(), glued) + \
            re.findall(r"HostFn wwhd_fn_%s\(\(const void\*\)&?([^,]+?),\s*0x" % func.upper(), glued)
        syms = self._syms(obj)
        dem = subprocess.run(["c++filt"], input="\n".join(s[3] for s in syms),  # Mach-O names (`__Z...`) as they are
                             capture_output=True, text=True).stdout.splitlines()
        pats = [re.compile(r"^(?:[\w:]+::)?" + re.escape(n.strip()) + r"\(") for n in names]  # its own namespaces may lead
        want = [s for s, d in zip(syms, dem) if any(pt.match(d) for pt in pats)]
        if not want:
            return None
        by_name = {s[3]: s for s in syms}
        sec_syms = collections.defaultdict(list)
        for s in syms:
            sec_syms[s[2]].append(s)
        sec_data = {}

        def data_bytes(sym):
            a, _scope, sec, name = sym
            if sec not in sec_data:
                seg, _, sname = sec.partition(",")
                dump = subprocess.run(["objdump", "-s", "--section=" + sec, obj], capture_output=True, text=True).stdout
                b = bytearray()
                for l in dump.splitlines():
                    m = re.match(r"^\s*[0-9a-f]+ ((?:[0-9a-f]{2,8} ?){1,4})", l)
                    if m:
                        b += bytes.fromhex(m.group(1).replace(" ", ""))
                base = min((x[0] for x in sec_syms[sec]), default=0)
                sec_data[sec] = (base, bytes(b))
            base, b = sec_data[sec]
            nxt = min((x[0] for x in sec_syms[sec] if x[0] > a), default=base + len(b))
            return b[a - base:nxt - base].hex()
        memo = {}

        def closure(name, depth=0):
            if name in memo:
                return memo[name]
            memo[name] = "<recursive %s>" % name
            sym = by_name.get(name)
            if sym is None:
                return name  # external: its name is the identity
            if "__text" not in sym[2]:
                memo[name] = "data:" + data_bytes(sym)
                return memo[name]
            dis = subprocess.run(["objdump", "-d", "-r", "--disassemble-symbols=" + name, obj], capture_output=True, text=True).stdout
            lines = []
            for l in dis.splitlines()[3:]:
                l = l.strip()
                if not l or l.startswith("Disassembly") or l.endswith(">:"):
                    continue
                l = re.sub(r"^[0-9a-f]+:\s*", "", l)
                refs = re.findall(r"\b(l_\.[\w.]+|L[\w.$]+|ltmp\d+|__Z\w+|_[A-Za-z_]\w*)", l)
                for ref in refs:
                    tgt = by_name.get(ref)
                    if tgt is not None and (tgt[1] == "l" or ref.startswith(("l_", "L", "ltmp"))) and ref != name and depth < 50:
                        h = hashlib.sha256(closure(ref, depth + 1).encode()).hexdigest()[:16]
                        l = l.replace(ref, "local:" + h)
                lines.append(l)
            memo[name] = "\n".join(lines)
            return memo[name]
        return "\n==\n".join(closure(s[3]) for s in sorted(want, key=lambda s: s[3]))

    def same(self, landed_root, src, landed, repo, commit, vtext, funcs):
        """(True/False, note): every function in FUNCS compiles alike in the version and the landed text"""
        lo, lg = self.compile(("landed", src), src, landed, os.path.join(landed_root, "wwhd_src", "include"))
        if lo is None:
            return False, "landed " + lg
        vo, vg = self.compile((repo, commit, src), src, vtext, self.include_tree(repo, commit))
        if vo is None:
            return False, "version " + vg
        for f in funcs:
            a, b = self.fingerprint(lo, lg, f), self.fingerprint(vo, vg, f)
            if a is None or b is None:
                return False, "no symbol for %s" % f
            if a != b:
                return False, "%s compiles differently" % f
        return True, "object identical"


class Versions:
    """earlier versions of the ledger's source files: distinct blobs reachable from REPO:REF in commit order"""

    def __init__(self, specs):
        self.specs = specs
        self.cache = {}

    def of(self, path):
        if path not in self.cache:
            seen, out = set(), []
            for spec in self.specs:
                repo, ref = spec.rsplit(":", 1)
                log = subprocess.run(["git", "-C", repo, "log", ref, "--format=%H", "--", path], capture_output=True, text=True)
                for c in log.stdout.split():
                    blob = subprocess.run(["git", "-C", repo, "rev-parse", "%s:%s" % (c, path)], capture_output=True, text=True).stdout.strip()
                    if not blob or blob in seen:
                        continue
                    seen.add(blob)
                    data = subprocess.run(["git", "-C", repo, "cat-file", "blob", blob], capture_output=True).stdout
                    out.append((blob, c, data.decode("utf-8", errors="surrogateescape")))
            self.cache[path] = out
        return self.cache[path]


def load_ledger(path):
    with open(path, newline="") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    need = {"global_id", "function", "source", "span_start", "span_end", "original", "replacement", "mutation", "final_class", "class_source"}
    miss = need - set(rows[0]) if rows else need
    if miss:
        raise SystemExit("receipt_audit: ledger lacks column(s): %s" % ", ".join(sorted(miss)))
    return rows


def load_idmaps(specs):
    maps = []
    for s in specs:
        sub, path = s.split("=", 1)
        with open(path, newline="") as f:
            m = {r["local_id"]: r["global_id"] for r in csv.DictReader(f, delimiter="\t")}
        maps.append((sub, m, path))
    return maps


def receipt_files(roots):
    for root in roots:
        for d, _dns, fns in os.walk(root):
            for fn in sorted(fns):
                if fn.startswith("mutant") and fn.endswith(".json"):
                    yield os.path.join(d, fn)


def audit(a):
    rows = load_ledger(a.ledger)
    by_id = {r["global_id"]: r for r in rows}
    by_key = collections.defaultdict(list)
    for r in rows:
        for fn in {r["function"]} | {x for x in (r.get("helper_callers") or "").split(",") if x}:
            by_key[(fn.upper(), norm_mutation(r["mutation"]))].append(r)
    landed = {}

    def landed_text(src):
        if src not in landed:
            p = os.path.join(a.root, src)
            landed[src] = read_text(p) if os.path.isfile(p) else None
        return landed[src]
    versions = Versions(a.versions)
    idmaps = load_idmaps(a.idmap)
    excludes = dict(x.split("=", 1) if "=" in x else (x, "excluded") for x in a.exclude)
    # an absolute PATH names one file; a relative one names every receipt whose path ends in /PATH (archive args files
    # name receipts relative to their receipt root, so they work on any machine)
    exclude_files = {(os.path.abspath(k) if os.path.isabs(k) else k): (r or "excluded")
                     for k, _, r in (x.partition("=") for x in a.exclude_receipt)}
    exclude_specs = dict(x.split("=", 1) if "=" in x else (x, "excluded spec") for x in a.exclude_spec)
    seeds = {int(x) for x in a.gate_seeds.split(",") if x}
    out, errors = [], []
    backing = collections.defaultdict(list)  # global_id -> current receipts (kind, clean witness, gated, path)
    pins = collections.defaultdict(list)  # global_id -> stale receipts whose function text equals the landed one
    stale_at = collections.defaultdict(list)  # global_id -> stale receipts with their version (for --object-pin)
    for p in receipt_files(a.receipts):
        parts = p.split(os.sep)
        rec = {"receipt": p, "status": "", "global_id": "", "kind": "", "version": "", "note": ""}
        ex = next((x for x in parts if x in excludes), None)
        exf = next((r for k, r in exclude_files.items() if os.path.abspath(p) == k or p.endswith(os.sep + k)), None)
        if exf is not None:
            rec.update(status="excluded", note=exf)
            out.append(rec)
            continue
        if ex:
            rec.update(status="excluded", note=excludes[ex])
            out.append(rec)
            continue
        try:
            j = json.loads(read_text(p))
            assert isinstance(j, dict)
        except Exception:
            rec["status"] = "unreadable"
            out.append(rec)
            continue
        sp = receipt_spec(j)
        if sp in exclude_specs:
            rec.update(status="excluded", note="spec %s: %s" % (sp, exclude_specs[sp]))
            out.append(rec)
            continue
        func, mut = str(j.get("func", "")).upper(), norm_mutation(j.get("mutation"))
        rec["kind"] = cascade.kind(j) if j.get("class") else ""
        mapped = next(((sub, m, mp) for sub, m, mp in idmaps if any(sub in x for x in parts[:-1])), None)
        if mapped:
            gid = mapped[1].get(str(j.get("id")))
            r = by_id.get(gid) if gid else None
            if r is None or norm_mutation(r["mutation"]) != mut or func not in {r["function"].upper()} | {
                    x.upper() for x in (r.get("helper_callers") or "").split(",") if x}:
                rec.update(status="error", global_id=gid or "", note="id map %s: local id %s -> %s does not match the receipt's function/mutation"
                           % (mapped[2], j.get("id"), gid))
                out.append(rec)
                errors.append(rec)
                continue
            cands = [r]
        else:
            cands = by_key.get((func, mut), [])
        if not cands:
            rec["status"] = "unjoined"
            out.append(rec)
            continue
        comp = p[:-5] + ".cpp"
        if not os.path.isfile(comp):
            rec.update(status="no-companion", global_id=",".join(r["global_id"] for r in cands))
            out.append(rec)
            continue
        ctext = read_text(comp)
        hits = []
        for r in cands:
            t = landed_text(r["source"])
            if t is not None and r["final_class"] != "REMOVED" and one_edit(t, ctext, r["original"], r["replacement"], at=int(r["span_start"])):
                hits.append(("current", r, ""))
        if not hits:  # the landed text, but not at the ledger's span: the ledger is wrong, not the receipt old
            off = [(r, k) for r in cands if landed_text(r["source"]) is not None
                   for k in one_edit(landed_text(r["source"]), ctext, r["original"], r["replacement"])]
            if off:
                rec.update(status="error", global_id=",".join(r["global_id"] for r, _ in off),
                           note="its mutantN.cpp is the landed text with the edit at offset %s, not at the ledger's span %s" % (
                               ",".join(str(k) for _, k in off), ",".join(r["span_start"] for r, _ in off)))
                out.append(rec)
                errors.append(rec)
                continue
        if not hits:
            for r in cands:
                t = landed_text(r["source"])
                for blob, commit, vt in versions.of(r["source"]):
                    if vt == t:
                        continue  # the landed version itself is checked above, at the span
                    if one_edit(vt, ctext, r["original"], r["replacement"]):
                        same = t is not None and function_text(vt, r["function"]) is not None and \
                            function_text(vt, r["function"]) == function_text(t, r["function"])
                        hits.append(("stale", r, "%s (commit %s)%s" % (blob[:12], commit[:12], ", function identical" if same else "")))
                        if same:
                            pins[r["global_id"]].append((rec["kind"], p, j))
                        stale_at[r["global_id"]].append((rec["kind"], p, j, blob, commit, vt))
                        break
        if not hits:
            rec.update(status="error", global_id=",".join(r["global_id"] for r in cands),
                       note="its mutantN.cpp matches neither the landed %s nor any earlier version (%d)" % (
                           cands[0]["source"], len(versions.of(cands[0]["source"]))))
            out.append(rec)
            errors.append(rec)
            continue
        st = hits[0][0]
        rec.update(status=st, global_id=",".join(h[1]["global_id"] for h in hits if h[0] == st),
                   version=";".join(sorted({h[2] for h in hits if h[0] == st})))
        out.append(rec)
        if st == "current":
            try:
                gated = int(j.get("n", 0)) >= a.gate_min_n and int(j.get("input_seed", -1)) in seeds
            except (TypeError, ValueError):
                gated = False
            if not gated:
                rec["note"] = "outside the gate (n %s, input seed %s)" % (j.get("n"), j.get("input_seed"))
            for h in hits:
                if h[0] == "current":
                    clean = rec["kind"] == "difference" and any(cascade.row_kills(x) for x in cascade.failing_rows(j))
                    backing[h[1]["global_id"]].append((rec["kind"], clean, gated, p))
    objpin = ObjectPin(a.tools_root, a.object_work) if a.object_pin else None

    def gated(j):
        try:
            return int(j.get("n", 0)) >= a.gate_min_n and int(j.get("input_seed", -1)) in seeds
        except (TypeError, ValueError):
            return False

    def class_ok(c, kind, j):
        if c == "D":
            return kind == "difference" and any(cascade.row_kills(x) for x in cascade.failing_rows(j))
        return kind in SURVIVOR_KINDS
    row_out, unbacked = [], []
    ruled_diff = dict(x.split("=", 1) if "=" in x else (x, "") for x in a.ruled_difference)
    ruled_seen = set()
    for r in rows:
        c = r["final_class"]
        if c not in CREDITED:
            continue
        rs = backing.get(r["global_id"], [])
        if c == "D":
            ok = [p for k, clean, g, p in rs if g and k == "difference" and clean]
        else:
            ok = [p for k, clean, g, p in rs if g and k in SURVIVOR_KINDS]
        pin, pin_note = [], ""
        if a.function_pin:
            pin = [p for k, p, j in pins.get(r["global_id"], []) if gated(j) and class_ok(c, k, j)]
        if objpin is not None and not ok:
            pin = []
            funcs = [r["function"]] + [x for x in (r.get("helper_callers") or "").split(",") if x]
            tried = set()
            for k, p, j, blob, commit, vt in stale_at.get(r["global_id"], []):
                if not (gated(j) and class_ok(c, k, j)) or blob in tried:
                    continue
                tried.add(blob)
                repo = next(v.rsplit(":", 1)[0] for v in a.versions if subprocess.run(
                    ["git", "-C", v.rsplit(":", 1)[0], "cat-file", "-e", commit], capture_output=True).returncode == 0)
                same, pin_note = objpin.same(a.root, r["source"], landed_text(r["source"]), repo, commit, vt, funcs)
                if same:
                    pin = [p for k2, p, j2, b2, _c, _v in stale_at[r["global_id"]] if b2 == blob and gated(j2) and class_ok(c, k2, j2)]
                    pin_note = "object identical to version %s" % blob[:12]
                    break
        basis = next((b for b in a.ruled_basis if r["class_source"].startswith(b)), None)
        killed = c in CONTRADICTABLE and [p for k, clean, g, p in rs if g and k == "difference" and clean]
        ruled = ruled_diff.get(r["global_id"])
        if ruled is not None:
            ruled_seen.add(r["global_id"])
        if killed and ruled is not None:  # a ruled exception: listed, not failing
            killed_ruled = killed
            killed = []
        else:
            killed_ruled = []
        how = "CONTRADICTED" if killed else "receipt" if ok else "pin" if pin else "basis" if basis else \
            "ruled-difference" if killed_ruled else "UNBACKED"  # an accepted ruled difference backs the row by its ruling
        if pin and not ok:
            ok = pin
        if killed:
            ok = killed
        row_out.append({"global_id": r["global_id"], "class": c, "function": r["function"], "class_source": r["class_source"],
                        "backing": how, "basis": basis or "", "receipts": len(ok), "first": ok[0] if ok else "",
                        "current_other": len(rs) - len(ok), "pin_note": pin_note,
                        "contradicted_ruled": ("%s (%s)" % (ruled, killed_ruled[0])) if killed_ruled else ""})
        if ruled is not None and not killed_ruled:
            errors.append({"receipt": "", "status": "error", "global_id": r["global_id"],
                           "note": "--ruled-difference %s names a row with no current clean difference (a stale exception)" % r["global_id"]})
        if how in ("UNBACKED", "CONTRADICTED"):
            unbacked.append(row_out[-1])
    for g in sorted(set(ruled_diff) - ruled_seen):
        errors.append({"receipt": "", "status": "error", "global_id": g,
                       "note": "--ruled-difference %s names no credited EQ/NAMED_GAP/RI row of the ledger" % g})
    return rows, out, errors, row_out, unbacked


def write_tsv(path, recs, cols):
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=cols, delimiter="\t", extrasaction="ignore")
        w.writeheader()
        w.writerows(recs)


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0], fromfile_prefix_chars="@")  # @FILE: one argument per line
    ap.add_argument("--ledger", required=True)
    ap.add_argument("--root", default=os.path.abspath(os.path.join(HERE, "..", "..")))
    ap.add_argument("--receipts", action="append", default=[], required=True)
    ap.add_argument("--versions", action="append", default=[], help="REPO:REF whose history holds earlier versions of the sources")
    ap.add_argument("--idmap", action="append", default=[], help="DIRSUBSTR=TSV with local_id/global_id columns")
    ap.add_argument("--exclude", action="append", default=[], help="DIRNAME=REASON: receipts below such a directory are listed as excluded")
    ap.add_argument("--exclude-receipt", action="append", default=[], help="PATH=REASON: one receipt file, excluded")
    ap.add_argument("--exclude-spec", action="append", default=[], help="SPEC=REASON: receipts whose run used that unit spec "
                    "(from the receipt's -spec: a unit file name without .txt, or an expanded spec's build directory) are excluded")
    ap.add_argument("--ruled-difference", action="append", default=[], help="ROW=REASON: an EQ, NAMED_GAP or RI row accepted "
                    "despite a current clean difference (listed as contradicted-ruled); a named row without one is an error")
    ap.add_argument("--ruled-basis", action="append", default=[], help="class_source prefix that names a ruled basis (no receipt needed)")
    ap.add_argument("--gate-min-n", type=int, default=1000, help="inputs a backing receipt ran at least (n 10000 is more evidence than 1000)")
    ap.add_argument("--gate-seeds", default="1,7")
    ap.add_argument("--object-pin", action="store_true", help="a stale receipt of the row's class backs it as `pin` when the "
                    "row's function compiles to identical object code in its version and the landed one (ObjectPin)")
    ap.add_argument("--tools-root", default=os.path.abspath(os.path.join(HERE, "..", "..")),
                    help="tree whose runtime/tools include directories and build/gen the object pin compiles with")
    ap.add_argument("--function-pin", action="store_true", help="a stale receipt whose version has the row's function text "
                    "unchanged backs the row as `pin` (a weaker basis than a receipt on the landed text; off by default)")
    ap.add_argument("--out-receipts")
    ap.add_argument("--out-rows")
    a = ap.parse_args(argv)
    if a.object_pin and a.function_pin:
        ap.error("--object-pin and --function-pin exclude each other (the text pin is a report only)")
    a.object_work = tempfile.mkdtemp(prefix="receipt_audit-objpin-") if a.object_pin else None
    try:
        rows, out, errors, row_out, unbacked = audit(a)
    finally:
        if a.object_work:
            shutil.rmtree(a.object_work, ignore_errors=True)  # this run's own compile directory
    if a.out_receipts:
        write_tsv(a.out_receipts, out, ["receipt", "status", "global_id", "kind", "version", "note"])
    if a.out_rows:
        write_tsv(a.out_rows, row_out, ["global_id", "class", "function", "class_source", "backing", "basis", "receipts", "first",
                                        "current_other", "pin_note", "contradicted_ruled"])
    st = collections.Counter(r["status"] for r in out)
    summary = {"ledger_rows": len(rows), "receipts": len(out), "status": dict(st),
               "stale_versions": dict(collections.Counter(r["version"].split(" ")[0] for r in out if r["status"] == "stale").most_common(20)),
               "credited": len(row_out), "backing": dict(collections.Counter(r["backing"] for r in row_out)),
               "basis": dict(collections.Counter(r["basis"] for r in row_out if r["backing"] == "basis")),
               "errors": len(errors), "unbacked": len(unbacked),
               "contradicted_ruled": sum(1 for r in row_out if r["contradicted_ruled"])}
    print(json.dumps(summary, indent=1, sort_keys=True))
    for e in errors[:50]:
        print("ERROR %s: %s" % (("receipt " + e["receipt"]) if e["receipt"] else "row " + e["global_id"], e["note"]), file=sys.stderr)
    for u in unbacked[:300]:
        if u["backing"] == "CONTRADICTED":
            print("CONTRADICTED credited row %s (%s, %s): a current receipt in the gate kills it: %s" % (u["global_id"], u["class"], u["class_source"], u["first"]), file=sys.stderr)
        else:
            print("UNBACKED credited row %s (%s, %s): no current %s receipt that passes the gate, no ruled basis" % (
                u["global_id"], u["class"], u["class_source"], "difference" if u["class"] == "D" else "survivor/identical-code"), file=sys.stderr)
    return 1 if errors or unbacked else 0


if __name__ == "__main__":
    sys.exit(main())

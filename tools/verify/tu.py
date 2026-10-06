#!/usr/bin/env python3
"""List the WWHD functions of one GameCube translation unit, for porting and progress tracking.

usage: tu.py d_a_bk [--all]        (file name without .cpp)
       tu.py --summary FILE...     one line per unit: functions, verified, GameCube stubs

The unit's range runs from its first to its last named function (build/names.tsv); unnamed
functions inside it belong to it (GHS links each unit contiguously). Unnamed functions right
outside it, up to the neighbouring unit's first named function, are listed with '?'.

Columns: address, size, verified (V = a VERIFY(0xADDR) exists in wwhd_src/), GameCube source
state (src = decompiled body, stub = "Nonmatching" placeholder, - = no GameCube function),
evidence, name, GameCube symbol.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
from funcdb import GenIndex, load_names  # noqa: E402


def verified_addrs():
    out = {}
    for dp, _, fs in os.walk(os.path.join(ROOT, "wwhd_src")):
        for f in fs:
            if f.endswith(".cpp"):
                p = os.path.join(dp, f)
                for m in re.finditer(r"^\s*VERIFY\(\s*0x([0-9A-Fa-f]{8})", open(p).read(), re.M):
                    out[int(m.group(1), 16)] = os.path.relpath(p, ROOT)
    return out


def gc_bodies(unit):
    """mangled symbol -> 'src' | 'stub' from tww/src/**/unit.cpp"""
    res = {}
    for dp, _, fs in os.walk(os.path.join(ROOT, "tww", "src")):
        if unit + ".cpp" in fs:
            text = open(os.path.join(dp, unit + ".cpp"), errors="replace").read()
            heads = list(re.finditer(r"/\* [0-9A-F]{8}-[0-9A-F]{8}\s+\.text\s+(\S+)\s*\*/", text))
            for i, m in enumerate(heads):
                end = heads[i + 1].start() if i + 1 < len(heads) else len(text)
                body = text[m.end():end]
                code = re.sub(r"/\*.*?\*/|//[^\n]*", "", body, flags=re.S)
                inner = code[code.find("{") + 1:code.rfind("}")] if "{" in code else ""
                res[m.group(1)] = "stub" if "Nonmatching" in body and not inner.strip() else "src"
            break
    return res


def unit_range(fname, names):
    """(lo, hi, outliers): the unit's main cluster of named functions. A cluster ends where a
    function named into another file sits between two of this file's functions; names outside
    the biggest cluster are probably matcher errors (or per-file inline copies placed elsewhere)."""
    allnamed = sorted(names)
    clusters, cur = [], []
    for a in allnamed:
        if names[a][1] == fname:
            cur.append(a)
        elif cur and names[a][1] not in ("", fname):
            clusters.append(cur)
            cur = []
    if cur:
        clusters.append(cur)
    if not clusters:
        return None, None, []
    # merge clusters separated by only a few foreign names (inline copies named by file of origin)
    best = max(clusters, key=len)
    outliers = [a for c in clusters if c is not best for a in c]
    return best[0], best[-1], outliers


def unit_rows(unit, gen, names, gc, ver, include_edges=True):
    fname = unit + ".cpp"
    lo, hi, outliers = unit_range(fname, names)
    if lo is None:
        return []
    addrs = sorted(set(a for a in gen.addrs if lo <= a <= hi) | set(a for a in names if lo <= a <= hi))
    rows = [(a, "") for a in addrs]
    if include_edges:
        i = gen.addrs.index(lo) if lo in gen.addrs else None
        if i is not None:
            j = i - 1
            while j >= 0 and gen.addrs[j] not in names:
                rows.insert(0, (gen.addrs[j], "?"))
                j -= 1
        k = gen.addrs.index(hi) + 1 if hi in gen.addrs else None
        while k is not None and k < len(gen.addrs) and gen.addrs[k] not in names:
            rows.append((gen.addrs[k], "?"))
            k += 1
    return rows


def main():
    args = sys.argv[1:]
    gen = GenIndex(os.path.join(ROOT, "build", "gen"))
    names, gc = load_names(os.path.join(ROOT, "build"))
    ver = verified_addrs()
    if args and args[0] == "--summary":
        print("%-24s %6s %6s %6s %6s" % ("unit", "funcs", "named", "verif", "stubs"))
        for unit in args[1:]:
            rows = unit_rows(unit, gen, names, gc, ver, include_edges=False)
            bodies = gc_bodies(unit)
            ns = sum(1 for a, _ in rows if a in names)
            nv = sum(1 for a, _ in rows if a in ver)
            stubs = sum(1 for a, _ in rows if bodies.get(gc.get(a, ("",))[0]) == "stub")
            print("%-24s %6d %6d %6d %6d" % (unit, len(rows), ns, nv, stubs))
        return
    unit = args[0]
    rows = unit_rows(unit, gen, names, gc, ver)
    bodies = gc_bodies(unit)
    _, _, outl = unit_range(unit + ".cpp", names)
    for a in outl:
        print("#outside the unit's range (matcher error?): %08X %s %s" % (a, names[a][0], names[a][2]))
    for a, mark in rows:
        n, f, e = names.get(a, ("", "", ""))
        sym = gc.get(a, ("", ""))[0]
        st = bodies.get(sym, "-") if sym else "-"
        other = "" if not f or f == unit + ".cpp" else " [%s]" % f
        print("%s%08X %6d %s %-4s %-10s %s%s  %s" % (mark or " ", a, gen.size(a), "V" if a in ver else ".", st, e, n, other, sym))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Self-test of the verify result-row parser (verify.RESULT_LINE), used by readable_check and verify_all.

The function column is 40 characters wide ("%-40.40s"). Overloaded members are displayed as a cast,
e.g. "(void (daNpc_Md_c::*)(int, int, int)) &", so a name can contain spaces; such rows must count
like any other (the old one-token pattern dropped them from function counts and coverage).
Exit 0 when all cases pass.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import readable_check  # noqa: E402
import verify  # noqa: E402
import verify_all  # noqa: E402


def row(addr, name, g, r, cv, first="ok"):
    # the exact layout of harness.cpp / verify.py's sharded merge
    return "%08X  %-40.40s %10s %10s %8s  %s" % (addr, name, g, r, cv, first)


CASES = [
    # (line, expected (addr, name, gp, gn, rp, rn, cov, nb, first)) ; None = not a result row
    (row(0x02286BC0, "(bool (daNpc_Md_c::*)(mDoExt_McaMorf2*, int)) &daNpc_Md_c::setAnm", "10000/10000", "0/0", "9/9"),
     ("02286BC0", "(bool (daNpc_Md_c::*)(mDoExt_McaMorf2*,", 10000, 10000, 0, 0, 9, 9, "ok")),
    (row(0x0228A524, "(void (daNpc_Md_c::*)(int, int, int)) &", "1000/1000", "3/3", "10/12"),
     ("0228A524", "(void (daNpc_Md_c::*)(int, int, int)) &", 1000, 1000, 3, 3, 10, 12, "ok")),
    (row(0x0228D208, "(void (daNpc_Md_c::*)(cXyz*, int, int))", "5/10", "0/0", "1/4", "gen #3: r3 0 != 1"),
     ("0228D208", "(void (daNpc_Md_c::*)(cXyz*, int, int))", 5, 10, 0, 0, 1, 4, "gen #3: r3 0 != 1")),
    # plain one-token names, short and exactly 40 characters
    (row(0x02000000, "daObj_Quake_c::execute", "10000/10000", "12/12", "18/18"),
     ("02000000", "daObj_Quake_c::execute", 10000, 10000, 12, 12, 18, 18, "ok")),
    (row(0x02000004, "x" * 40, "10000/10000", "12/12", "18/18"),
     ("02000004", "x" * 40, 10000, 10000, 12, 12, 18, 18, "ok")),
    # recorded column with rejected inputs "(-N)"
    ("%08X  %-40.40s %10s %10s %8s  %s" % (0x02000008, "f", "4/4", "2/3(-1)", "1/1", "rec #0: r3"),
     ("02000008", "f", 4, 4, 2, 3, 1, 1, "rec #0: r3")),
    # not result rows
    ("%-8s  %-40s %10s %10s %8s  %s" % ("addr", "function", "generated", "recorded", "coverage", "first difference"), None),
    ("%08X  %-40s  no original in unit" % (0x0200000C, "g"), None),
]


def main():
    fails = 0
    for line, want in CASES:
        m = verify.RESULT_LINE.match(line)
        got = None
        if m:
            g = m.groups()
            got = (g[0], g[1].strip()) + tuple(int(x) for x in g[2:8]) + (g[8].strip(),)
        if got != want:
            fails += 1
            print("FAIL %r\n  want %r\n  got  %r" % (line, want, got))
    # both consumers use the one pattern and see all rows
    text = "\n".join(line for line, _ in CASES)
    res = readable_check.parse(text)
    want_addrs = {w[0] for _, w in CASES if w}
    if set(res) != want_addrs or res["0228A524"]["name"] != "(void (daNpc_Md_c::*)(int, int, int)) &":
        fails += 1
        print("FAIL readable_check.parse: %r" % sorted(res))
    if verify_all.LINE is not verify.RESULT_LINE:
        fails += 1
        print("FAIL verify_all.LINE is not verify.RESULT_LINE")
    n = len(CASES) + 2
    print("verify_parse_test: %s %d/%d" % ("ok" if not fails else "FAIL", n - fails, n))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

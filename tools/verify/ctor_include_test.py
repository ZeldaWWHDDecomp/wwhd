#!/usr/bin/env python3
"""Self-test (2026-11-01, ): an expanded spec (a unit with `include`) loads
tools/verify/ctor_sizes.tsv like the unit it includes.

Before, the harness looked for ctor_sizes.tsv beside specfile.py's expanded copy in the build / verify-scratch
directory, found none, and ran included specs without the constructor facts (no undersized-Local constructor check,
no zero fill of output-only mocked-constructor storage). Checks:
1. With WWHD_REPORT_CTOR_SIZES=1, a temporary companion tools/verify/units/_ctor_include_test/comp.txt = `include
   d_a_ep.txt` reports ctor_sizes.tsv loaded with the same constructor count as d_a_ep itself, and its result rows
   (-n 200, seed 1, -nulltrap) are byte-identical to d_a_ep's.
2. Behaviour under `stackfill periodic` on daEp_Execute 0212DBFC (d_a_ep builds a dBgS_GndChk through the listed,
   mocked cBgS_GndChk constructor 02008E0C, sizeof 0x40, whose storage the constructor facts zero on both sides): the
   companion `include d_a_ep.txt` + `stackfill periodic` and a plain copy of d_a_ep.txt + `stackfill periodic` give
   byte-identical rows at -n 2000. On the base harness they differ (measured 2026-11-02: the include form passed
   2000/2000 without the zero fill, the plain copy 1887/2000 with it - d_a_ep is not stackfill-clean, see
   ctor_storage_test), so this fails there and passes with the fix.
3. A spec whose ctor_sizes.tsv cannot be found (a copy of d_a_ep.txt under a temporary .../units/ directory, run with
   the d_a_ep harness and -list) prints `ctor_sizes: PATH missing` on stderr without WWHD_REPORT_CTOR_SIZES.
The temporary units and scratch are removed.
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITDIR = os.path.join(HERE, "units", "_ctor_include_test")


def rows(out):
    return [l for l in out.splitlines() if len(l) > 10 and l[8:10] == "  " and all(c in "0123456789ABCDEF" for c in l[:8])]


def run(unit, scratch, args=("-n", "200", "-seed", "1", "-nulltrap", "-shards", "1")):
    env = dict(os.environ, WWHD_VERIFY_SCRATCH=scratch, WWHD_REPORT_CTOR_SIZES="1")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit] + list(args), cwd=ROOT,
                       capture_output=True, text=True, env=env)
    counts = set(re.findall(r"ctor_sizes: (\d+) constructors", r.stderr + r.stdout))
    missing = "missing" in re.findall(r"ctor_sizes: \S+ (loaded|missing)", r.stderr + r.stdout)
    return r.returncode, rows(r.stdout), counts, missing, (r.stderr or r.stdout)[-400:]


def main():
    fails = []
    base = os.environ.get("WWHD_VERIFY_SCRATCH") or os.path.join(ROOT, "build", "verify")
    scratch = tempfile.mkdtemp(prefix="ctor_include_test-", dir=base)
    os.makedirs(UNITDIR, exist_ok=True)
    try:
        open(os.path.join(UNITDIR, "comp.txt"), "w").write("include d_a_ep.txt\n")
        rc, a, ca, ma, ea = run("_ctor_include_test/comp", scratch)
        rc2, b, cb, mb, eb = run("d_a_ep", scratch)
        if rc or rc2:
            fails.append("exit %d / %d: %s / %s" % (rc, rc2, ea, eb))
        if ma or not ca:
            fails.append("companion: ctor_sizes.tsv not loaded (%s)" % (ca or "missing"))
        if mb or not cb:
            fails.append("d_a_ep: ctor_sizes.tsv not loaded (%s)" % (cb or "missing"))
        if ca and cb and ca != cb:
            fails.append("constructor counts differ: %s vs %s" % (ca, cb))
        if not fails and (not a or a != b):
            fails.append("rows differ (%d companion, %d direct)" % (len(a), len(b)))
        # 2. the zero fill of the constructor storage reaches the include form
        open(os.path.join(UNITDIR, "inc_fill.txt"), "w").write("# verify: -only 0212DBFC\ninclude d_a_ep.txt\nstackfill periodic\n")
        open(os.path.join(UNITDIR, "plain_fill.txt"), "w").write(
            "# verify: -only 0212DBFC\n" + open(os.path.join(HERE, "units", "d_a_ep.txt")).read() + "\nstackfill periodic\n")
        big = ("-n", "2000", "-seed", "1", "-shards", "4")
        rc, a2, _, _, e2 = run("_ctor_include_test/inc_fill", scratch, big)
        rc2, b2, _, _, f2 = run("_ctor_include_test/plain_fill", scratch, big)
        if not a2 or not b2:
            fails.append("stackfill runs gave no rows: %s / %s" % (e2, f2))
        elif a2 != b2:
            fails.append("stackfill: include form %s / plain copy %s" % (a2[0][:72], b2[0][:72]))
        # 3. a miss is always said
        os.environ["WWHD_VERIFY_SCRATCH"] = scratch
        sys.path.insert(0, HERE)
        import verify  # noqa: E402
        exe = verify.build("d_a_ep", jobs=4)
        tdir = tempfile.mkdtemp(prefix="ctor_missing-", dir=scratch)
        os.makedirs(os.path.join(tdir, "units"))
        sp = os.path.join(tdir, "units", "x.txt")
        shutil.copy(os.path.join(HERE, "units", "d_a_ep.txt"), sp)
        env = {k: v for k, v in os.environ.items() if k != "WWHD_REPORT_CTOR_SIZES"}
        r = subprocess.run([exe, "-spec", sp, "-list"], cwd=ROOT, capture_output=True, text=True, env=env)
        want = "ctor_sizes: %s/ctor_sizes.tsv missing" % tdir
        if want not in r.stderr:
            fails.append("missing ctor_sizes.tsv not reported (want %r): %s" % (want, r.stderr[-300:]))
    finally:
        shutil.rmtree(UNITDIR, ignore_errors=True)
        if not fails:
            shutil.rmtree(scratch, ignore_errors=True)
    print("ctor_include_test: %s" % ("FAIL: " + "; ".join(fails) if fails else "ok (include path loads ctor_sizes.tsv; rows equal to d_a_ep, also under stackfill periodic; a miss is reported)"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

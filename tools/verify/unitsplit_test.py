#!/usr/bin/env python3
"""Self-test of the unit split (verify.py -unitsplit N, 2026-11-02): unit.c compiled as N translation units.
Uses lib2_agl_19 (147 tested functions, 212 callees), -only 027D1D60,027D7EA8 -n 200 -seed 1 -nulltrap:
1. No split (default: the split is opt-in): no unit.parts manifest, unit.c.o linked.
2. -unitsplit 4: unit.parts names 4 parts, the link map takes unit.p0.c.o..unit.p3.c.o and not unit.c.o, unit.c is the
   same file as without a split, and stdout is byte-identical to run 1.
3. WWHD_UNIT_SPLIT_AUTO_MB=0 WWHD_UNIT_SPLIT=3: the environment request (3 parts), same stdout.
4. WWHD_UNIT_SPLIT_AUTO_MB=0 with no request: no split (and the stale manifest of run 3 is gone).
Exit 0 when all pass."""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402
UNIT = "lib2_agl_19"
ARGS = ["-only", "027D1D60,027D7EA8", "-n", "200", "-seed", "1", "-nulltrap", "--no-header-args", "--no-cache", "-shards", "1"]


def run(extra, env=None):
    e = dict(os.environ)
    e.pop("WWHD_UNIT_SPLIT", None)
    e.update(env or {})
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT] + ARGS + extra, cwd=ROOT, env=e,
                       capture_output=True, text=True)
    out = verify.unit_dir(UNIT)
    m = os.path.join(out, "unit.parts")
    parts = open(m).read().split() if os.path.exists(m) else None
    linked = set()
    mp = os.path.join(out, "verify.map")
    if os.path.exists(mp):
        for line in open(mp, errors="replace"):
            if line.startswith("[") and "/unit." in line:
                linked.add(os.path.basename(line.split("] ", 1)[1].strip()))
    unitc = open(os.path.join(out, "unit.c"), "rb").read() if os.path.exists(os.path.join(out, "unit.c")) else None
    return r, parts, linked, unitc


def main():
    fails = []
    r0, p0, l0, u0 = run([])
    rows0 = [l for l in r0.stdout.splitlines() if l[:8] in ("027D1D60", "027D7EA8")]
    if r0.returncode or len(rows0) != 2:
        fails.append("run 1 failed: %s" % (r0.stdout + r0.stderr)[-400:])
    if p0 is not None or l0 != {"unit.c.o"}:
        fails.append("run 1: split without a request (parts %s, linked %s)" % (p0, sorted(l0)))
    r1, p1, l1, u1 = run(["-unitsplit", "4"])
    if p1 != ["unit.p%d.c" % k for k in range(4)] or l1 != {"unit.p%d.c.o" % k for k in range(4)}:
        fails.append("run 2: parts %s, linked %s" % (p1, sorted(l1)))
    if u1 != u0:
        fails.append("run 2: unit.c differs from the unsplit unit.c")
    if r1.returncode or r1.stdout != r0.stdout:
        fails.append("run 2: stdout differs from the unsplit run")
    r2, p2, l2, _ = run([], {"WWHD_UNIT_SPLIT_AUTO_MB": "0", "WWHD_UNIT_SPLIT": "3"})
    if p2 != ["unit.p%d.c" % k for k in range(3)] or l2 != {"unit.p%d.c.o" % k for k in range(3)} or r2.stdout != r0.stdout:
        fails.append("run 3: parts %s, linked %s, stdout same %s" % (p2, sorted(l2), r2.stdout == r0.stdout))
    r3, p3, l3, _ = run([], {"WWHD_UNIT_SPLIT_AUTO_MB": "0"})
    if p3 is not None or l3 != {"unit.c.o"} or r3.stdout != r0.stdout:
        fails.append("run 4: parts %s, linked %s, stdout same %s" % (p3, sorted(l3), r3.stdout == r0.stdout))
    for f in fails:
        print("FAIL", f)
    print("unitsplit_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

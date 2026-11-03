#!/usr/bin/env python3
"""Self-test of `stopat FUNC TARGET N live REG...`.

`stopat` ends both sides at the N-th top-level call to TARGET and compares the calls and the net effect up to it, not the
exit registers, so a mutant that differs only in a register the original keeps live past the stop (AGL#3: f13, read by
the suffix at source 1558) was unobserved. With `live REG...` the named GPRs/FPRs are compared at the terminal call.

lib2_sead_15's constructWorkerCounter 02759928 with `arg! 02759928 r3 0` (so it calls allocateWorkerObject 0273AD10) and
`stopat 02759928 0273AD10 1`:
a. unmodified, without and with `live f13 r3`: ok;
b. a candidate that writes f13 = 1.0 just before the call: ok without `live` (unobserved, the old behaviour), and with
   `live f13`: `stopat: live f13 at the terminal call: original ... candidate 1/...`; with `live r3` only: ok;
c. `live x5` and `live` without a register are spec errors.
Writes a temporary unit directory (tools/verify/units/_stoplive_test) and a mutant source next to the original
(overwritten by the next run, never deleted). Builds go below WWHD_VERIFY_SCRATCH when set. Exit 0 when all pass.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_stoplive_test")
F = "02759928"
SRC = "wwhd_src/sead/WorkerRuntimeInstances.cpp"
MUT = "wwhd_src/sead/_stoplive_test_WorkerRuntimeInstances.cpp"
SITE = "    if(object==NULL) object=static_cast<CounterWord*>(allocateWorkerObject(sizeof(CounterWord)));\n"
F13 = "    if(object==NULL) { gabi::cpu->f[13].ps0 = 1.0; gabi::cpu->f[13].ps1 = 1.0; }\n" + SITE


def spec(name, src, live):
    os.makedirs(TMP, exist_ok=True)
    text = open(os.path.join(UNITS, "lib2_sead_15.txt")).read().replace("src " + SRC, "src " + src)
    text += "\narg! %s r3 0\nstopat %s 0273AD10 1%s\n" % (F, F, (" live " + live) if live is not None else "")
    open(os.path.join(TMP, name + ".txt"), "w").write(text)
    return "_stoplive_test/" + name


def run(unit):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit, "-only", F, "-n", "100", "-seed", "1",
                        "-buildj", "4", "-noprogress"], cwd=ROOT, capture_output=True, text=True)
    rows = [l for l in r.stdout.splitlines() if l.startswith(F)]
    return r.returncode, (rows[0] if rows else (r.stdout + r.stderr)[-300:])


def main():
    text = open(os.path.join(ROOT, SRC)).read()
    if text.count(SITE) != 1:
        raise SystemExit("site not found once in %s" % SRC)
    open(os.path.join(ROOT, MUT), "w").write(text.replace(SITE, F13))
    fails = []
    cases = [("base_nolive", SRC, None, True), ("base_live", SRC, "f13 r3", True), ("f13_nolive", MUT, None, True),
             ("f13_live", MUT, "f13", False), ("f13_liver3", MUT, "r3", True)]
    for name, src, live, ok in cases:
        rc, row = run(spec(name, src, live))
        print("%-12s live %-7s rc %d  %s" % (name, live or "-", rc, row))
        if ok and (rc or " ok" not in row):
            fails.append("%s: want ok, got %s" % (name, row))
        if not ok and (rc == 0 or "stopat: live f13 at the terminal call: original" not in row or "candidate 1/1" not in row):
            fails.append("%s: want the live f13 difference, got %s" % (name, row))
    for name, live in (("bad_reg", "x5"), ("bad_empty", "")):
        rc, row = run(spec(name, SRC, live))
        print("%-12s live %-7r rc %d  %s" % (name, live, rc, row[-120:]))
        if rc != 2 or "stopat" not in row:
            fails.append("%s: want a spec error, got rc %d %s" % (name, rc, row))
    for x in fails:
        print("FAIL", x)
    print("stoplive_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

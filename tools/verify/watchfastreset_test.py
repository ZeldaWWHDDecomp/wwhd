#!/usr/bin/env python3
"""Self-test of the -fastreset setup-state check under -watch (2026-10-29; AGL#3 02776E40 recipe).

With -fastreset (default) a setupsnapshot validation input's candidate side installs the original side's post-setup state;
under -watch, -trace or WWHD_VERIFY_FASTRESET_CHECK=1 it runs the setups for real and compares its state with the
original's (val_diff). That check failed with `fastreset check: ... (run counters)`, exit 3, whenever the tested function
loads a float-stored NaN word as an integer at depth 0 (vm_ld32 notes it in the 8-slot V.fcopy ring for the GHS
value-copy rule): reset() cleared only the ring's count (V.nfcopy), so the slots at or past the count kept the previous
run's values (the original side saw the previous input's candidate run, the candidate side the original's run of this
input), and val_diff compared all 8 slots. Those slots are never read (vm_st32 reads slots < nfcopy), so no verdict was
affected; only the check was wrong. reset() now clears the whole ring, and val_diff names the counter that differs.

Uses merged lib2_agl_12_ctor_027AB058_kind0 (setupsnapshot world) with -snapcheck 1 (every input a validation input) and
-n 5, on two functions whose original stores a float argument to the stack and reloads it as an integer
(027A8D08 DrawProgram, 027AA2EC VolumeDraw):
a. -watch: exit 0, no `fastreset check` line, the same rows as the run without -watch (before the fix: exit 3);
b. WWHD_VERIFY_FASTRESET_CHECK=1: the same (before the fix: exit 3);
c. -nofastreset: the same rows (control).
Builds go below WWHD_VERIFY_SCRATCH when set; nothing is written into the tree. Exit 0 when all pass.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNIT = "lib2_agl_12_ctor_027AB058_kind0"
FUNCS = ("027A8D08", "027AA2EC")


def run(func, extra=(), env=None):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-only", func, "-n", "5", "-snapcheck", "1",
                        "-buildj", "4", "-noprogress"] + list(extra), cwd=ROOT, capture_output=True, text=True,
                       env=dict(os.environ, **(env or {})))
    rows = [l for l in r.stdout.splitlines() if l[:8] == func]
    check = [l for l in r.stderr.splitlines() if l.startswith("fastreset check")]
    return r.returncode, rows, check


def main():
    fails = []
    for f in FUNCS:
        rc0, rows0, chk0 = run(f)
        print("%s plain: rc %d %s" % (f, rc0, rows0[0] if rows0 else "-"))
        if rc0 or not rows0 or chk0:
            fails.append("%s: the plain run does not pass: rc %d %s %s" % (f, rc0, rows0, chk0))
        for what, extra, env in (("-watch", ["-watch", "5D030000"], None),
                                 ("FASTRESET_CHECK=1", [], {"WWHD_VERIFY_FASTRESET_CHECK": "1"}),
                                 ("-nofastreset", ["-nofastreset"], None)):
            rc, rows, chk = run(f, extra, env)
            print("%s %s: rc %d %s%s" % (f, what, rc, rows[0] if rows else "-", (" | " + chk[0]) if chk else ""))
            if rc != rc0 or rows != rows0 or chk:
                fails.append("%s %s: want rc %d and the plain rows without a fastreset check line, got rc %d %s %s"
                             % (f, what, rc0, rc, rows, chk))
    for x in fails:
        print("FAIL", x)
    print("watchfastreset_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

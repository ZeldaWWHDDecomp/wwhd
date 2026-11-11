#!/usr/bin/env python3
"""Self-test of -p0exclude (harness.cpp) and its use on nulltrap_compare's side without -nulltrap
(2026-10-31: a difference on an input where the unmodified original touched page 0 is no evidence, zero-page rule).

Carrier: merged lib2_sead_15's TextBuffer::assureTerminationImpl 027598AC (`text=mText; capacity=mCapacity;
text[capacity-1]=0;`) with a deliberately wrong candidate that stores 1 instead of 0, so it differs on every input.
1. `arg! 027598AC r3 0`: every input makes the original read mText/mCapacity from page 0. On the -nulltrap side both sides
   take a DSI (aborted on both sides); on the plain side every difference comes after the original's page-0 access and is
   excluded. Verdict: no FAIL, and the `plain difference(s) after the original's page-0 access excluded` line appears.
2. Without the `arg!` line (valid object pointers): the wrong candidate is a FAIL as before (nulltrap_compare exit 1,
   verdict FAIL naming 027598AC).
3. The harness refuses `-p0exclude` together with a command-line `-nulltrap` (exit 2).
4. A unit with the `nulltrap` directive (every run traps page 0, so nulltrap_compare's plain side traps too): nulltrap_compare
   runs (no exit 2) and the plain side notes `p0exclude: off` (2026-10-31, lib2_agl_1 main spec).
Writes a temporary unit directory and a mutant source and removes them. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
FUNC = "027598AC"
ORIG_SRC = "wwhd_src/sead/WorkerRuntimeInstances.cpp"
MUT_SRC = "wwhd_src/sead/_p0exclude_test_WorkerRuntimeInstances.cpp"
UDIR = os.path.join(HERE, "units", "_p0exclude_test")


def write_unit(name, extra):
    lines = []
    for l in open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read().splitlines():
        lines.append("src " + MUT_SRC if l.strip() == "src " + ORIG_SRC else l)
    with open(os.path.join(UDIR, name + ".txt"), "w") as f:
        f.write("\n".join(lines + extra) + "\n")
    return "_p0exclude_test/" + name


def ntc(unit, extra=()):
    # without WWHD_VERIFY_SCRATCH: case 3 runs the built harness from build/verify/_p0exclude_test/valid, and the cleanup
    # below removes that directory (a scratch redirect would build it elsewhere and leave it there)
    env = {k: v for k, v in os.environ.items() if k != "WWHD_VERIFY_SCRATCH"}
    r = subprocess.run([sys.executable, os.path.join(HERE, "nulltrap_compare.py"), unit, "-n", "200", "-seed", "1",
                        "-only", FUNC] + list(extra), cwd=ROOT, capture_output=True, text=True, env=env)
    return r.returncode, r.stdout + r.stderr


def main():
    fails = []
    text = open(os.path.join(ROOT, ORIG_SRC)).read()
    old = "    text[capacity-1]=0;\n}\n/* 027598C4 */"
    if text.count(old) != 1:
        print("p0exclude_test: FAIL (carrier source changed: %s)" % ORIG_SRC)
        return 1
    os.makedirs(UDIR, exist_ok=True)
    try:
        open(os.path.join(ROOT, MUT_SRC), "w").write(text.replace(old, "    text[capacity-1]=1;\n}\n/* 027598C4 */"))
        rc, out = ntc(write_unit("null", ["arg! %s r3 0" % FUNC]))
        verdict = [l for l in out.splitlines() if l.startswith("nulltrap_compare: _p0exclude_test")]
        if "after the original's page-0 access excluded" not in out:
            fails.append("case 1: no exclusion line: %s" % out[-600:])
        if any("FAIL" in v for v in verdict) or any(l.startswith("FAIL seed") for l in out.splitlines()):
            fails.append("case 1: page-0 differences counted as FAIL: %s" % verdict)
        rc, out = ntc(write_unit("valid", []))
        verdict = [l for l in out.splitlines() if l.startswith("nulltrap_compare: _p0exclude_test")]
        if rc != 1 or not any("FAIL" in v and FUNC in v for v in verdict):
            fails.append("case 2: wrong candidate on valid inputs not a FAIL (rc %d): %s" % (rc, verdict or out[-400:]))
        logs = os.path.join(UDIR, "logs")
        rc, out = ntc(write_unit("unitnt", ["arg! %s r3 0" % FUNC, "nulltrap"]), ["--logdir", logs])
        perr = os.path.join(logs, "_p0exclude_test/unitnt.s1.plain.err")
        note = os.path.exists(perr) and "p0exclude: off" in open(perr).read()
        if rc not in (0, 1) or not note:
            fails.append("case 4: unit `nulltrap` directive: rc %d, plain-side note %s: %s" % (rc, note, out[-300:]))
        exe = os.path.join(ROOT, "build", "verify", "_p0exclude_test", "valid", "verify")
        if os.path.exists(exe):
            r = subprocess.run([exe, "-spec", os.path.join(UDIR, "valid.txt"), "-n", "1", "-nulltrap", "-p0exclude"],
                               cwd=ROOT, capture_output=True, text=True)
            if r.returncode != 2:
                fails.append("case 3: -p0exclude with -nulltrap not refused (rc %d)" % r.returncode)
        else:
            fails.append("case 3: harness executable not found at %s" % exe)
    finally:
        if os.path.exists(os.path.join(ROOT, MUT_SRC)):
            os.remove(os.path.join(ROOT, MUT_SRC))
        shutil.rmtree(UDIR, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_p0exclude_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("p0exclude_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

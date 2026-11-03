#!/usr/bin/env python3
"""Self-test of the unit-file `stopat FUNC TARGET N` directive (harness.cpp): a run of FUNC ends at its N-th top-level call to
the mocked TARGET; calls and net memory up to there are compared, the return value is not (non-returning functions such as
thread loops; JASTaskThread::run 02810B9C).

Uses the merged JKRDvdFile::close 027EACBC (lib_js_base): it calls JKRDvdCloseInfo 0284B780, then clears mIsAvailable and
unlinks the file. With `stopat 027EACBC 0284B780 1`:
1. the unmodified source passes every input;
2. a candidate that differs only after the terminal call (stores 1 instead of 0) passes, and fails without the directive;
3. a candidate that passes a different argument to the terminal call fails;
4. a candidate that returns before the terminal call fails (only one side stops);
5. malformed lines exit 2.
Writes a temporary unit file and a mutant source next to JKRDvdFile.cpp and removes them. Exit 0 when all cases pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_stopat_test")
FUNC, TARGET = "027EACBC", "0284B780"
SRC = "wwhd_src/JSystem/JKernel/JKRDvdFile.cpp"
MUT = "wwhd_src/JSystem/JKernel/_stopat_test_JKRDvdFile.cpp"
SPEC = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()
STOP = "stopat %s %s 1" % (FUNC, TARGET)


def run(name, lines, src=SRC, n="300"):
    body = [("src " + src) if l.strip() == "src " + SRC else l for l in SPEC]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_stopat_test/" + name, "-only", FUNC, "-n", n],
                       cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith(FUNC)]
    return r.returncode, (row[0] if row else r.stdout[-400:] + r.stderr[-400:])


def mutant(old, new):
    text = open(os.path.join(ROOT, SRC)).read()
    if text.count(old) != 1:
        raise SystemExit("mutation site not found once in %s: %r" % (SRC, old))
    open(os.path.join(ROOT, MUT), "w").write(text.replace(old, new))


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        rc, row = run("plain", [STOP])
        if rc or "300/300" not in row:
            fails.append("unmodified source does not pass with stopat: %s" % row)
        mutant("        mIsAvailable = 0;\n        JKRDvdRemove", "        mIsAvailable = 1;\n        JKRDvdRemove")
        rc, row = run("after", [STOP], src=MUT)
        if rc or "300/300" not in row:
            fails.append("a difference after the terminal call was compared: %s" % row)
        rc, row = run("after_nostop", [], src=MUT)
        if rc == 0:
            fails.append("the after-stop mutant passes without stopat (test lost its point): %s" % row)
        mutant("JKRDvdCloseInfo(mFileInfo) != 0", "JKRDvdCloseInfo(mFileInfo + 1) != 0")
        rc, row = run("argument", [STOP], src=MUT)
        if rc == 0:
            fails.append("a different argument at the terminal call passed: %s" % row)
        mutant("    if ((u8)mIsAvailable == 0) return;\n    if (JKRDvdCloseInfo", "    return;\n    if (JKRDvdCloseInfo")
        rc, row = run("early_return", [STOP], src=MUT)
        if rc == 0:
            fails.append("returning before the terminal call passed: %s" % row)
        for bad in ("stopat %s %s 0" % (FUNC, TARGET), "stopat %s %s" % (FUNC, TARGET), "stopat %s xyz 1" % FUNC,
                    "stopat * %s 1" % TARGET, "stopat %s %s 1 extra" % (FUNC, TARGET)):
            rc, _ = run("bad", [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_stopat_test"), ignore_errors=True)
        if os.path.exists(os.path.join(ROOT, MUT)):
            os.remove(os.path.join(ROOT, MUT))
    for f in fails:
        print("FAIL", f)
    print("stopat_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Test of single-value `ret`/`ret!` lines (harness.cpp, 2026-10-11): `ret ADDR V` means `ret ADDR V V` (before, the
value was silently ignored), and malformed lines exit 2. Uses the merged unit hd_text_unit (its ret pins on
025F4BC0/02878098); writes units/_ret_single_test/ and removes it. Exit 0 when all cases pass."""
import os, re, shutil, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_ret_single_test")
BASE = open(os.path.join(HERE, "units", "hd_text_unit.txt")).read()
PIN = re.compile(r"^(ret!?\s+[0-9A-Fa-f]{8})\s+([^\s,#]+)\s*$", re.M)


def run(name, text, n="1000"):
    open(os.path.join(TMP, name + ".txt"), "w").write(text)
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_ret_single_test/" + name, "-n", n],
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, "\n".join(l for l in r.stdout.splitlines() if not l.startswith("unit "))


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        if not PIN.search(BASE):
            fails.append("hd_text_unit has no single-value ret line any more; pick another unit")
        single = run("single", BASE)
        double = run("double", PIN.sub(r"\1 \2 \2", BASE))
        if single[0] or single != double:
            fails.append("`ret ADDR V` differs from `ret ADDR V V`")
        for bad in ("ret 025F4BC0", "ret! 025F4BC0", "ret 025F4BC0 1,2 3"):
            if run("bad", BASE + "\n" + bad + "\n", "1")[0] != 2:
                fails.append("not rejected: " + bad)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_ret_single_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("ret_single_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

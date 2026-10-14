#!/usr/bin/env python3
"""Parse test of the unit-file `mockretain TARGET rN SIZE` directive (harness.cpp): valid lines load and leave
an unrelated unit's results unchanged; malformed ones exit 2. (Behaviour was checked on MAP-dungeon_map#4
02666EA4, see README.) Uses the merged unit hd_txt_text_builder_input; writes units/_mockretain_test/ and
removes it. Exit 0 when all cases pass."""
import os, shutil, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_mockretain_test")
BASE = open(os.path.join(HERE, "units", "hd_txt_text_builder_input.txt")).read()


def run(name, extra):
    open(os.path.join(TMP, name + ".txt"), "w").write(BASE + "\n" + extra + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_mockretain_test/" + name, "-n", "300"],
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, "\n".join(l for l in r.stdout.splitlines() if not l.startswith("unit "))


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        a = run("plain", "")
        b = run("retain", "mockretain 01000300 r3 8")
        if a[0] or a != b:
            fails.append("a mockretain of an uncalled target changed the results")
        for bad in ("mockretain 01000300 r3 0", "mockretain 01000300 r11 8", "mockretain 01000300 r3", "mockretain 01000300 r3 8 9"):
            if run("bad", bad)[0] != 2:
                fails.append("not rejected: " + bad)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_mockretain_test"), ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mockretain_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

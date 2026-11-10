#!/usr/bin/env python3
"""Self-test (2026-11-01): a cascade in which no pass runs (every companion has nothing in scope) still writes
alive.json, so the seed-7 cascade that reads it with --from-alive works (before, item3 cascades of lib2_sead_14 and
hd_hud_place_name_rupy_4more failed there with FileNotFoundError). No mutate.py run: the receipt's source is in no spec."""
import json
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))


def main():
    W = tempfile.mkdtemp(prefix="cascade_empty_test-")
    fails = []
    rc = os.path.join(W, "rc")
    os.mkdir(rc)
    json.dump({"id": 7, "src": "wwhd_src/none/not_in_any_spec.cpp", "func": "02000000", "class": "survived",
               "mutation": "test"}, open(os.path.join(rc, "mutant7.json"), "w"))
    open(os.path.join(rc, "mutant7.cpp"), "w").write("// test\n")
    cas = [sys.executable, os.path.join(HERE, "cascade.py"), "--receipts", rc, "--load-max", "1000"]
    r1 = subprocess.run(cas + ["--out", os.path.join(W, "s1"), "--input-seed", "1", "lib2_sead_14"], cwd=ROOT,
                        capture_output=True, text=True)
    ap = os.path.join(W, "s1", "alive.json")
    if r1.returncode or not os.path.exists(ap):
        fails.append("s1: rc %d, alive.json %s: %s" % (r1.returncode, os.path.exists(ap), (r1.stdout + r1.stderr)[-300:]))
    else:
        a = json.load(open(ap))
        if list(a) != ["7"] or a["7"]["killed_by"]:
            fails.append("s1 alive.json: %s" % a)
        r2 = subprocess.run(cas + ["--from-alive", ap, "--out", os.path.join(W, "s7"), "--input-seed", "7", "lib2_sead_14"],
                            cwd=ROOT, capture_output=True, text=True)
        if r2.returncode or not os.path.exists(os.path.join(W, "s7", "alive.json")):
            fails.append("s7: rc %d: %s" % (r2.returncode, (r2.stdout + r2.stderr)[-300:]))
    print("cascade_empty_test: %s" % ("FAIL: " + "; ".join(fails) if fails else "ok"))
    if not fails:
        shutil.rmtree(W, ignore_errors=True)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

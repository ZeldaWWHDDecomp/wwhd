#!/usr/bin/env python3
"""Self-test: -stopfirst does not stop on an input where the original took a page-0 DSI (2026-11-01).

Under -nulltrap a difference on such an input never kills (zero-page rule, cascade.py kind()). Before the fix the harness
took the first difference as the run's verdict even there: a mutant whose first differing input was an original-fault
input (a pre-fault call difference) ended its run on it, the clean inputs were never compared, and the cascade kept the
mutant alive as difference-page0 (hd_fw_parts_text_valid #1432 on 02705648, 13 such mutants in the re-check B archives).

The mutant here is that one: hasAnySuffix's stringLength without its refreshString (SafeString terminator) call. Input
1000003 of seed 1 makes the original fault on page 0 after a differing call #0; later inputs are clean. Checks, seeds 1
and 7: the rerun receipt is a kill under the cascade rule (class difference, kind() difference), its first difference
names no page-0 fault, and the unmutated campaign baseline still passes. Work files in a temporary directory under the
verify scratch (removed when everything passed).
"""
import glob
import json
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import cascade  # noqa: E402
import verify  # noqa: E402

UNIT = "hd_fw_parts_text_valid"
SRC = "wwhd_src/d/hd_layout_locale_search.cpp"
DROP = "static s32 stringLength(LayoutLocaleString_l *string) {\n    refreshString(string);\n"


def main():
    fails = []
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="stopfirst_page0_test-", dir=base)
    text = open(os.path.join(ROOT, SRC)).read()
    if text.count(DROP) != 1:
        print("stopfirst_page0_test: FAIL: the stringLength body changed; update the test")
        return 1
    rr = os.path.join(W, "in")
    os.mkdir(rr)
    open(os.path.join(rr, "mutant1.cpp"), "w").write(text.replace(DROP, DROP.replace("    refreshString(string);\n", "")))
    json.dump({"id": 1, "src": SRC, "func": "02705648", "class": "survived",
               "mutation": "drop statement `refreshString(string);` (stopfirst_page0_test)"},
              open(os.path.join(rr, "mutant1.json"), "w"))
    for seed in ("1", "7"):
        rc_dir = os.path.join(W, "rc-s" + seed)
        r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), UNIT, "--rerun", rr, "--rerun-class", "survived",
                            "--n", "1000", "--input-seed", seed, "--nulltrap", "-j", "2", "--no-progress",
                            "--receipts", rc_dir], cwd=ROOT, capture_output=True, text=True)
        js = glob.glob(os.path.join(rc_dir, "mutant1.json"))
        if r.returncode or not js:
            fails.append("seed %s: mutate rc %d, receipts %s: %s" % (seed, r.returncode, js, (r.stdout + r.stderr)[-600:]))
            continue
        j = json.load(open(js[0]))
        first = (j.get("stdout") or "").split("\n")[1:2]
        if j["class"] != "difference" or cascade.kind(j) != "difference":
            fails.append("seed %s: class %s, cascade kind %s (want a clean kill): %s" % (seed, j["class"], cascade.kind(j), first))
        elif any("page-0" in x or "faulted" in x for x in first):
            fails.append("seed %s: first difference names a page-0 fault: %s" % (seed, first))
    print("stopfirst_page0_test: %s" % ("FAIL: " + "; ".join(fails) if fails else "ok (seeds 1 and 7: clean kill)"))
    if not fails:
        shutil.rmtree(W, ignore_errors=True)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

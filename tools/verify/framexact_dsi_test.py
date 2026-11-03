#!/usr/bin/env python3
"""Self-test: `framexact` exit registers are not compared on an input where both sides took the same page-0 DSI (2026-11-02).

Under -nulltrap an input where both sides fault on page 0 at the same access crashed the console: there is no exit, so the
nonvolatile registers at the fault (the original's body values, the candidate's not-yet-restored caller values) are not
observable, like the dying stack frames. Before the fix the framexact exit compare (r14..r31, f14..f31) still ran there:
hd_sys_02034B14 02034FFC failed on all 453 both-DSI inputs of seed 1, hd_ui_02005978 02005EA8 on 998.

Checks on HDGameFramework::procFrame (02034FFC, framexact), seed 1, -nulltrap:
  1. unmutated: every row compares (no framexact exit difference) and the original still faults on page 0 on 521
     inputs, so 479/1000 clean (both-DSI inputs are not clean inputs; 453 before 2026-11-02 whose
     page-0 calls fault on the original side too: 68 inputs call through a page-0 pointer);
  2. a mutant that drops the task null check faults on page 0 on the candidate side only: still a difference naming the
     one-sided fault;
  3. a mutant that changes r14 at the end of the body (not restored by the epilogue): still an `exit r14 ... (framexact)`
     difference on a clean input.
Work files in a temporary directory under the verify scratch (removed when everything passed).
"""
import glob
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

UNIT = "hd_sys_02034B14"
FUNC = "02034FFC"
SRC = "wwhd_src/hd/hd_sys_02034B14.cpp"
NULLCHECK = "    if (!task || !gabi::call_ptr<u32>(task->mpVtable[3], task, &l_frameTaskType)) task = nullptr;\n"
TAIL = "    HDFramework_rootEndFrame(ea(i_this->mpRootTask.get()));\n}\n\n/* 020350C4 */"

MUTANTS = {
    1: (NULLCHECK, NULLCHECK.replace("!task || ", ""), "by the candidate only"),
    2: (TAIL, TAIL.replace("\n}\n", "\n    cpu->r[14] ^= 1;\n}\n", 1), "exit r14: "),
}


def main():
    fails = []
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="framexact_dsi_test-", dir=base)
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-only", FUNC, "-n", "1000", "-seed", "1",
                        "-nulltrap", "-shards", "2"], cwd=ROOT, capture_output=True, text=True)
    out = r.stdout + r.stderr
    row = [l for l in out.splitlines() if l.startswith(FUNC)]
    if r.returncode or not row or "1000/1000" not in row[0] or "framexact" in out:
        fails.append("unmutated: rc %d, row %s: %s" % (r.returncode, row[:1], out[-600:]))
    else:
        dsi = sum(int(m) for m in re.findall(r"took a page-0 DSI on (\d+) input", out))
        if dsi != 521:  # both-DSI inputs are not clean inputs: 479/1000 clean
            fails.append("unmutated: %d page-0 inputs (want 521, 479/1000 clean): %s" % (dsi, out[-600:]))
    text = open(os.path.join(ROOT, SRC)).read()
    rr = os.path.join(W, "in")
    os.mkdir(rr)
    for mid, (old, new, _) in MUTANTS.items():
        if text.count(old) != 1:
            fails.append("mutant %d: the procFrame body changed; update the test" % mid)
            continue
        open(os.path.join(rr, "mutant%d.cpp" % mid), "w").write(text.replace(old, new))
        json.dump({"id": mid, "src": SRC, "func": FUNC, "class": "survived", "mutation": "framexact_dsi_test #%d" % mid},
                  open(os.path.join(rr, "mutant%d.json" % mid), "w"))
    if not fails:
        rc_dir = os.path.join(W, "rc")
        r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), UNIT, "--rerun", rr, "--rerun-class", "survived",
                            "--n", "1000", "--input-seed", "1", "--nulltrap", "-j", "2", "--no-progress", "--receipts", rc_dir],
                           cwd=ROOT, capture_output=True, text=True)
        for mid, (_, _, want) in MUTANTS.items():
            js = glob.glob(os.path.join(rc_dir, "mutant%d.json" % mid))
            if r.returncode or not js:
                fails.append("mutant %d: mutate rc %d, receipts %s: %s" % (mid, r.returncode, js, (r.stdout + r.stderr)[-600:]))
                continue
            j = json.load(open(js[0]))
            so = j.get("stdout") or ""
            if j["class"] != "difference" or want not in so:
                fails.append("mutant %d: class %s, want a difference naming %r: %s" % (mid, j["class"], want, so[:400]))
    print("framexact_dsi_test: %s" % ("FAIL: " + "; ".join(fails) if fails else
                                       "ok (both-DSI rows compare, one-sided DSI and clean-input exit r14 still differ)"))
    if not fails:
        shutil.rmtree(W, ignore_errors=True)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Self-test: mutate.py stops before any mutant when the unmutated baseline fails (2026-10-27, AGL#8: a campaign
reported 3,165/3,165 killed because 0279345C already failed unmutated), and the default receipts.

1. failing baseline: a temporary spec (lib2_sead_15 with a copy of WorkerRuntimeInstances.cpp whose constructWorkerCounter
   02759928 stores 1 instead of 0) and a campaign on another function (--func 02759964): exit 3 before any mutant, the
   message names the 02759928 row, no summary line, no receipt;
2. the same with --allow-failing-baseline: the campaign runs without 02759928 (stderr names it) and every receipt carries
   baseline_excluded ["02759928"];
3. passing baseline (the unchanged spec, --func 02759928 --max 6 --seed 3): exit 0, "unmutated baseline passes", receipts
   by default in WWHD_VERIFY_SCRATCH/_receipts/<unit>-..., the same classes and stdout as with an explicit --receipts DIR;
   --no-receipts writes none;
4. failing_rows: "ok (some inputs aborted on both sides)" is a passing row.
Temporary files are removed; builds go to a private WWHD_VERIFY_SCRATCH directory (below an existing one when set).
"""
import atexit
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
import mutate  # noqa: E402

UNITS = os.path.join(HERE, "units")
TMPU = os.path.join(UNITS, "_mutate_baseline_test")
SRC = "wwhd_src/sead/WorkerRuntimeInstances.cpp"
MUT = "wwhd_src/sead/_mutate_baseline_test_WorkerRuntimeInstances.cpp"


def mut(*args, env=None):
    e = dict(os.environ)
    e.update(env or {})
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + list(args), cwd=ROOT, capture_output=True,
                       text=True, env=e)
    return r.returncode, r.stdout, r.stderr


def receipts(d):
    out = {}
    for f in sorted(glob.glob(os.path.join(d, "mutant*.json"))):
        j = json.load(open(f))
        out[j["id"]] = j
    return out


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    scratch = tempfile.mkdtemp(prefix="wwhd-mutate_baseline_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = scratch
    os.environ.pop("WWHD_MUT_SCRATCH", None)
    atexit.register(shutil.rmtree, scratch, True)
    tmp = tempfile.mkdtemp(prefix="wwhd-mutate_baseline_test-rc-")
    atexit.register(shutil.rmtree, tmp, True)
    fails = []
    try:
        text = open(os.path.join(ROOT, SRC)).read()
        old = "if(object!=NULL) object->mValue=0;"
        if text.count(old) != 1:
            raise SystemExit("mutation site not found once in %s" % SRC)
        open(os.path.join(ROOT, MUT), "w").write(text.replace(old, "if(object!=NULL) object->mValue=1;"))
        os.makedirs(TMPU, exist_ok=True)
        open(os.path.join(TMPU, "bad.txt"), "w").write(
            open(os.path.join(UNITS, "lib2_sead_15.txt")).read().replace("src " + SRC, "src " + MUT))

        # 1. failing baseline
        rd = os.path.join(tmp, "bad")
        rc, out, err = mut("_mutate_baseline_test/bad", "--func", "02759964", "--n", "50", "--max", "3", "-j", "2",
                           "--receipts", rd)
        print("1. failing baseline: rc %d" % rc)
        for l in err.splitlines():
            if l.startswith("mutate.py: the unmutated") or l.startswith("  0275") or l.startswith("mutate.py: no mutant"):
                print("     " + l[:160])
        if rc != 3 or "02759928" not in err or "no mutant was run" not in err:
            fails.append("failing baseline: rc %d, stderr %s" % (rc, err[-600:]))
        if "mutants compiled" in out or receipts(rd):
            fails.append("failing baseline: mutants ran: %s" % out[-300:])

        # 2. --allow-failing-baseline
        rd = os.path.join(tmp, "allow")
        rc, out, err = mut("_mutate_baseline_test/bad", "--func", "02759964", "--n", "50", "--max", "3", "-j", "2",
                           "--receipts", rd, "--allow-failing-baseline")
        rs = receipts(rd)
        print("2. --allow-failing-baseline: rc %d, %d receipt(s), excluded %s; %s" % (
            rc, len(rs), sorted({tuple(j.get("baseline_excluded", [])) for j in rs.values()}),
            (out.splitlines() or ["-"])[-1][:120] if "mutants compiled" in out else "no summary"))
        if rc or not rs or any(j.get("baseline_excluded") != ["02759928"] for j in rs.values()) or "leaving out 02759928" not in err:
            fails.append("allow-failing-baseline: rc %d, receipts %s, stderr %s" % (rc, list(rs.values())[:1], err[-400:]))

        # 3. passing baseline: default receipts == explicit receipts
        args = ["lib2_sead_15", "--func", "02759928", "--n", "50", "--max", "6", "--seed", "3", "-j", "2"]
        rc1, out1, err1 = mut(*args)
        dflt = glob.glob(os.path.join(scratch, "*", "_receipts", "lib2_sead_15-*")) + \
            glob.glob(os.path.join(scratch, "_receipts", "lib2_sead_15-*"))
        rd = os.path.join(tmp, "explicit")
        rc2, out2, err2 = mut(*(args + ["--receipts", rd]))
        r1 = receipts(dflt[0]) if dflt else {}
        r2 = receipts(rd)
        same = {i: j["class"] for i, j in r1.items()} == {i: j["class"] for i, j in r2.items()}
        print("3. passing baseline: rc %d/%d, default receipts %s (%d), explicit (%d), classes %s, stdout %s" % (
            rc1, rc2, dflt[0] if dflt else "-", len(r1), len(r2), "same" if same else "DIFFERENT",
            "same" if out1 == out2 else "DIFFERENT"))
        if rc1 or rc2 or "unmutated baseline passes" not in err1 or len(dflt) != 1 or not r1 or not same or out1 != out2:
            fails.append("passing baseline: rc %d/%d, default %s, stderr %s" % (rc1, rc2, dflt, err1[-400:]))
        before = set(glob.glob(os.path.join(scratch, "*", "_receipts", "*")))
        rc3, out3, err3 = mut(*(args + ["--no-receipts"]))
        after = set(glob.glob(os.path.join(scratch, "*", "_receipts", "*")))
        if rc3 or after != before or out3 != out1:
            fails.append("--no-receipts: rc %d, new receipts %s" % (rc3, after - before))

        # 4. failing_rows
        row = "%08X  %-40.40s %10s %10s %8s  %s"
        ok_ab = row % (0x02759928, "constructWorkerCounter", "50/50", "0/0", "3/5", "ok (some inputs aborted on both sides)")
        diff = row % (0x02759964, "sead::worker_runtime::Registration::destroy", "49/50", "0/0", "3/5", "call #1 ...")
        if mutate.failing_rows(ok_ab + "\n" + diff + "\n") != [diff] or mutate.classify(1, ok_ab + "\n") != "invalid":
            fails.append("failing_rows: %r" % mutate.failing_rows(ok_ab + "\n" + diff + "\n"))
    finally:
        shutil.rmtree(TMPU, ignore_errors=True)
        if os.path.exists(os.path.join(ROOT, MUT)):
            os.remove(os.path.join(ROOT, MUT))
    for x in fails:
        print("FAIL", x)
    print("mutate_baseline_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

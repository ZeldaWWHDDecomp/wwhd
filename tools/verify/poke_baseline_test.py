#!/usr/bin/env python3
"""Self-test: the state after `setup`/`poke`/`postsetup` is the net-effect baseline (harness.cpp, 2026-10-26).

A poked byte that the function rewrites with the same value is not a change. Before the fix, poked bytes kept their pre-poke
initial value, so a candidate that drops a redundant store of the poked value showed a false difference (AGL#18 027CDE3C
IDs 40/58).

Uses merged lib2_sead_15's constructWorkerCounter 02759928 (`if (object != NULL) object->mValue = 0;`) with
`arg! r3 0x5E000000`:
1. the unchanged source passes with `poke 5E000000 u32 0` and with `poke 5E000000 u32 0x12345678`;
2. a copy without the store passes with `poke ... u32 0` (the original only rewrites the poked 0: no false difference);
3. the same copy FAILS with `poke ... u32 0x12345678` (the original changes the word: a real difference is still seen);
4. the copy also FAILS with a `setup` line plus `poke ... u32 0x12345678` (baseline taken after setup and poke).
Writes temporary files under tools/verify/units/_poke_baseline_test/ and wwhd_src/sead/_poke_baseline_tmp.cpp, removes them.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
TMP = os.path.join(HERE, "units", "_poke_baseline_test")
SRC = "wwhd_src/sead/WorkerRuntimeInstances.cpp"
MSRC = "wwhd_src/sead/_poke_baseline_tmp.cpp"
SPEC = open(os.path.join(HERE, "units", "lib2_sead_15.txt")).read().splitlines()
FUNC = "02759928"
STORE = "if(object!=NULL) object->mValue=0;"


def run(name, lines, n="200"):
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_poke_baseline_test/" + name, "-only", FUNC, "-n", n,
                        "-shards", "1"], cwd=ROOT, capture_output=True, text=True)
    out = r.stdout + r.stderr
    row = [l for l in out.splitlines() if l.startswith(FUNC)]
    return (row[0].endswith(" ok") if row else None), out


def spec(src, poke, extra=()):
    lines = [l for l in SPEC if l.strip() != "real 02759928"]
    if src != SRC:
        lines = [(l.replace(SRC, src) if l.startswith("src ") else l) for l in lines]
    return lines + ["arg! %s r3 0x5E000000" % FUNC, "poke %s 5E000000 u32 %s" % (FUNC, poke)] + list(extra)


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    text = open(os.path.join(ROOT, SRC)).read()
    try:
        if text.count(STORE) != 1:
            fails.append("store line not found once in %s" % SRC)
            raise SystemExit
        open(os.path.join(ROOT, MSRC), "w").write(text.replace(STORE, "if(object!=NULL) {}"))
        for poke in ("0", "0x12345678"):
            ok, out = run("orig_%s" % poke, spec(SRC, poke))
            if ok is not True:
                fails.append("unchanged source fails with poke %s: %s" % (poke, out[-300:]))
        ok, out = run("drop_same", spec(MSRC, "0"))
        if ok is not True:
            fails.append("false difference: dropped redundant store of the poked value fails: %s" % out[-400:])
        ok, out = run("drop_diff", spec(MSRC, "0x12345678"))
        if ok is not False:
            fails.append("real difference not seen (dropped store, poked 0x12345678): %s" % out[-300:])
        ok, out = run("drop_diff_setup", spec(MSRC, "0x12345678", ["setup %s 02759928 r3=0x5E000100" % FUNC, "real 02759928"]))
        if ok is not False:
            fails.append("real difference not seen with a setup line: %s" % out[-300:])
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        for d in ("_poke_baseline_test",):
            shutil.rmtree(os.path.join(ROOT, "build", "verify", d), ignore_errors=True)
        try:
            os.remove(os.path.join(ROOT, MSRC))
        except OSError:
            pass
    for f in fails:
        print("FAIL", f)
    print("poke_baseline_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

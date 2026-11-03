#!/usr/bin/env python3
"""Self-test of the baseline cache (mutate.py --baseline-cache / --baseline-only, tools/verify/basecache.py; 2026-10-30,
).

On lib2_sead_15 (--func 02759928 --max 6 --seed 3 --n 100, a 6-mutant campaign):
1. reuse gives the same campaign: the campaign without the cache, the one that records its baseline and the one that
   takes the recorded baseline (stderr "cached baseline ..., not run again") give the same mutate.py stdout and, per
   mutant, the same class and harness stdout; the cached one reads the recorded reference-side records (100% hits);
   --baseline-only records once (exit 0) and a later campaign hits; basecache.prefetch runs two baselines (one of them
   failing) at most 2 at a time;
2. a key change runs the baseline again: --n 101, --nulltrap, another WWHD_* variable each record a new entry; a
   changed record file in an entry is not used (the baseline runs); baseline_key changes with a spec line, the
   executable bytes, the command and the refcache key;
3. a failing baseline is never cached as passing: the spec with 02759928 failing unmutated exits 3 with the cache on,
   twice, and leaves no entry; with --allow-failing-baseline the campaign runs but records nothing; Cache.lookup refuses
   a hand-made entry whose stored run fails, and Cache.store refuses a nonzero exit;
4. TimeoutPolicy(first_load=..., remeasure=True): a cached wall time is used at its recorded load and measured again
   when the load moved by more than half (the default since 2026-10-31 never measures again: mutate_timeout_test.py).
Builds go to a private WWHD_VERIFY_SCRATCH directory (below an existing one when set); temporary files are removed.
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
import basecache  # noqa: E402
import mutate  # noqa: E402

UNITS = os.path.join(HERE, "units")
TMPU = os.path.join(UNITS, "_basecache_test")
SRC = "wwhd_src/sead/WorkerRuntimeInstances.cpp"
MUT = "wwhd_src/sead/_basecache_test_WorkerRuntimeInstances.cpp"
ARGS = ["lib2_sead_15", "--func", "02759928", "--max", "6", "--seed", "3", "--n", "100", "-j", "2"]


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


def entries(cdir):
    return sorted(e for e in os.listdir(cdir) if os.path.isfile(os.path.join(cdir, e, "receipt.json"))) \
        if os.path.isdir(cdir) else []


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    scratch = tempfile.mkdtemp(prefix="wwhd-basecache_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = scratch
    os.environ.pop("WWHD_MUT_SCRATCH", None)
    os.environ.pop("WWHD_BASELINE_CACHE", None)
    atexit.register(shutil.rmtree, scratch, True)
    tmp = os.path.join(scratch, "t")
    os.makedirs(tmp)
    fails = []
    try:
        # 1. the same campaign with and without the cache
        cdir = os.path.join(tmp, "bc")
        runs = {}
        for tag, extra in (("plain", []), ("store", ["--baseline-cache", cdir]), ("hit", ["--baseline-cache", cdir])):
            rd = os.path.join(tmp, "r-" + tag)
            runs[tag] = mut(*(ARGS + extra + ["--receipts", rd])) + (receipts(rd),)
        (rc0, out0, _, r0), (rc1, out1, err1, r1), (rc2, out2, err2, r2) = runs["plain"], runs["store"], runs["hit"]
        cls = [{i: j["class"] for i, j in r.items()} for r in (r0, r1, r2)]
        sto = [{i: j.get("stdout") for i, j in r.items()} for r in (r0, r1, r2)]
        print("1. plain / store / hit: rc %d/%d/%d, %d/%d/%d receipts, classes %s, harness stdout %s, mutate stdout %s; %s" % (
            rc0, rc1, rc2, len(r0), len(r1), len(r2), "same" if cls[0] == cls[1] == cls[2] else "DIFFERENT",
            "same" if sto[0] == sto[1] == sto[2] else "DIFFERENT", "same" if out0 == out1 == out2 else "DIFFERENT",
            sorted(set(cls[0].values()))))
        if rc0 or rc1 or rc2 or not r0 or not (cls[0] == cls[1] == cls[2]) or not (sto[0] == sto[1] == sto[2]) \
                or not (out0 == out1 == out2):
            fails.append("reuse: classes %s, stdout %s / %s" % (cls, out0[-300:], out2[-300:]))
        if "recorded in the baseline cache" not in err1 or "not run again" not in err2 or "not run again" in err1:
            fails.append("reuse: store/hit notes missing: %s || %s" % (err1[-400:], err2[-400:]))
        # every original the hit campaign ran live is a digest record's live fallback (a killed mutant's differing input;
        # the default digest records since 2026-10-31), none a missing record
        import re as _re
        mh = _re.search(r"reference-side cache: \d+ generated input\(s\) took the baseline's recorded original, (\d+) ran it live", out2)
        md = _re.search(r"(\d+) ran the original live after a digest difference", out2)
        if not mh or int(mh.group(1)) != (int(md.group(1)) if md else 0) or len(entries(cdir)) != 1:
            fails.append("reuse: refcache hits / one entry: %s, entries %s" % (out2[-400:], entries(cdir)))
        if {j.get("baseline_cache", {}).get("use") for j in r2.values()} != {"hit"} or \
                {j.get("baseline_cache", {}).get("use") for j in r1.values()} != {"stored"} or \
                any("baseline_cache" in j for j in r0.values()):
            fails.append("reuse: receipts' baseline_cache fields wrong")
        # --baseline-only records, the campaign hits
        cdir2 = os.path.join(tmp, "bc2")
        rcb, outb, errb = mut(*(ARGS + ["--baseline-cache", cdir2, "--baseline-only"]))
        rch, outh, errh = mut(*(ARGS + ["--receipts", os.path.join(tmp, "r-bo")]), env={"WWHD_BASELINE_CACHE": cdir2})
        rh = receipts(os.path.join(tmp, "r-bo"))
        print("   --baseline-only: exit %d, %d entry; then campaign (WWHD_BASELINE_CACHE): exit %d, %s, classes %s" % (
            rcb, len(entries(cdir2)), rch, "hit" if "not run again" in errh else "NO HIT",
            "same" if {i: j["class"] for i, j in rh.items()} == cls[0] else "DIFFERENT"))
        if rcb or "mutants compiled" in outb or len(entries(cdir2)) != 1 or rch or "not run again" not in errh or \
                {i: j["class"] for i, j in rh.items()} != cls[0] or outh != out0:
            fails.append("--baseline-only: %d %s %s" % (rcb, errb[-300:], errh[-300:]))

        # 3 (setup). a failing baseline: 02759928 stores 1 instead of 0
        text = open(os.path.join(ROOT, SRC)).read()
        old = "if(object!=NULL) object->mValue=0;"
        if text.count(old) != 1:
            raise SystemExit("mutation site not found once in %s" % SRC)
        open(os.path.join(ROOT, MUT), "w").write(text.replace(old, "if(object!=NULL) object->mValue=1;"))
        os.makedirs(TMPU, exist_ok=True)
        open(os.path.join(TMPU, "bad.txt"), "w").write(
            open(os.path.join(UNITS, "lib2_sead_15.txt")).read().replace("src " + SRC, "src " + MUT))

        # prefetch: two specs, one failing, at most 2 at a time
        cdir3 = os.path.join(tmp, "bc3")
        seen = []
        res = basecache.prefetch(["lib2_sead_15", "_basecache_test/bad"], ARGS[1:], cdir3, parallel=2,
                                 load_max=1e9, poll=0.2, load=lambda: seen.append(1) or 0.0)
        rcp, outp, errp = mut(*(ARGS + ["--baseline-cache", cdir3, "--receipts", os.path.join(tmp, "r-pf")]))
        print("   prefetch: %s, %d entry; campaign after it: exit %d, %s" % (res, len(entries(cdir3)), rcp,
                                                                         "hit" if "not run again" in errp else "NO HIT"))
        if res != {"lib2_sead_15": 0, "_basecache_test/bad": 3} or len(entries(cdir3)) != 1 or rcp or "not run again" not in errp:
            fails.append("prefetch: %s, entries %s, %s" % (res, entries(cdir3), errp[-300:]))

        # 2. key changes run the baseline again
        for tag, extra, env in (("--n 101", ["--n", "101"], None), ("--nulltrap", ["--nulltrap"], None),
                                ("WWHD_REFCACHE_MAX_MB=512", [], {"WWHD_REFCACHE_MAX_MB": "512"})):
            before = entries(cdir)
            rck, outk, errk = mut(*(ARGS + extra + ["--baseline-cache", cdir, "--no-receipts"]), env=env)
            new = len(entries(cdir)) - len(before)
            print("2. key change %-26s exit %d, baseline %s, %d new entry" % (
                tag + ":", rck, "cached" if "not run again" in errk else "ran", new))
            if rck or "not run again" in errk or new != 1:
                fails.append("key change %s: %s" % (tag, errk[-300:]))
        # a changed record file is not used
        key1 = json.load(open(os.path.join(cdir2, entries(cdir2)[0], "receipt.json")))["key"]
        recs = sorted(glob.glob(os.path.join(cdir2, key1, "refcache", "*")))
        with open(recs[0], "r+b") as f:
            b = f.read(1)
            f.seek(0)
            f.write(bytes([b[0] ^ 1]))
        rct, outt, errt = mut(*(ARGS + ["--baseline-cache", cdir2, "--no-receipts"]))
        print("   changed record file: exit %d, %s" % (rct, "NOT USED, baseline ran" if "record files differ" in errt and
                                                         "not run again" not in errt else "USED"))
        if rct or "record files differ" not in errt or "not run again" in errt or outt != out0:
            fails.append("tampered entry: %s" % errt[-300:])
        # baseline_key: spec line, executable bytes, command, refcache key
        import verify
        import specfile
        exe = verify.build("lib2_sead_15", jobs=2)
        spec = specfile.spec_path("lib2_sead_15", ROOT, verify.unit_dir("lib2_sead_15"))
        lines = list(specfile.expanded_lines(os.path.join(UNITS, "lib2_sead_15.txt")))
        cmd = [exe, "-spec", spec, "-n", "100", "-seed", "3"]
        k0 = basecache.baseline_key(cmd, "rk", lines)[0]
        exe2 = os.path.join(tmp, "verify-copy")
        shutil.copy(exe, exe2)
        with open(exe2, "ab") as f:
            f.write(b"\0")
        ks = {"same inputs": basecache.baseline_key(cmd, "rk", lines)[0],
              "spec line": basecache.baseline_key(cmd, "rk", lines + ["fill * 0\n"])[0],
              "executable bytes": basecache.baseline_key([exe2] + cmd[1:], "rk", lines)[0],
              "-seed": basecache.baseline_key(cmd[:-1] + ["4"], "rk", lines)[0],
              "refcache key": basecache.baseline_key(cmd, "rk2", lines)[0],
              "env": basecache.baseline_key(cmd, "rk", lines, environ=dict(os.environ, WWHD_VERIFY_FASTRESET="0"))[0],
              "neutral env": basecache.baseline_key(cmd, "rk", lines, environ=dict(os.environ, WWHD_VERIFY_PROGRESS="1"))[0]}
        print("   baseline_key: " + ", ".join("%s %s" % (t, "same" if k == k0 else "changes") for t, k in ks.items()))
        if ks["same inputs"] != k0 or ks["neutral env"] != k0 or \
                any(ks[t] == k0 for t in ("spec line", "executable bytes", "-seed", "refcache key", "env")):
            fails.append("baseline_key: %s" % ks)

        # 3. a failing baseline is never cached
        cdir4 = os.path.join(tmp, "bc4")
        bad = ["_basecache_test/bad", "--func", "02759964", "--n", "50", "--max", "3", "-j", "2", "--baseline-cache", cdir4]
        r1 = mut(*(bad + ["--no-receipts"]))
        r2_ = mut(*(bad + ["--no-receipts"]))
        ra = mut(*(bad + ["--allow-failing-baseline", "--receipts", os.path.join(tmp, "r-allow")]))
        rs = receipts(os.path.join(tmp, "r-allow"))
        print("3. failing baseline with the cache: exit %d, %d; entries %d; --allow-failing-baseline: exit %d, %d receipt(s), "
              "baseline_cache use %s, entries %d" % (r1[0], r2_[0], len(entries(cdir4)), ra[0], len(rs),
                                                     sorted({j.get("baseline_cache", {}).get("use") for j in rs.values()}),
                                                     len(entries(cdir4))))
        if r1[0] != 3 or r2_[0] != 3 or "02759928" not in r2_[2] or "not run again" in r2_[2] or entries(cdir4) or \
                ra[0] or not rs or {j.get("baseline_cache", {}).get("use") for j in rs.values()} != {"ran"}:
            fails.append("failing baseline: %s / %s / %s" % (r1[2][-300:], r2_[2][-300:], ra[2][-300:]))
        # hand-made entries whose stored run fails; store refuses a nonzero exit
        c = basecache.Cache(os.path.join(tmp, "bc5"))
        row = "%08X  %-40.40s %10s %10s %8s  %s"
        okrow = row % (0x02759928, "constructWorkerCounter", "50/50", "0/0", "3/5", "ok")
        badrow = row % (0x02759928, "constructWorkerCounter", "49/50", "0/0", "3/5", "call #1 ...")
        chk = lambda rc, so: mutate.check_baseline(rc, so, "t")  # noqa: E731
        refused = []
        for k, (verdict, rc, so) in {"a" * 64: ("fail", 1, badrow), "b" * 64: ("pass", 1, badrow),
                                     "c" * 64: ("pass", 0, badrow), "d" * 64: ("pass", 0, "")}.items():
            os.makedirs(c.entry(k))
            json.dump({"version": basecache.VERSION, "key": k, "verdict": verdict, "rc": rc, "stdout": so + "\n",
                       "refcache_key": None, "records": None, "wall": 1.0, "load": 1.0, "time": "-"},
                      open(os.path.join(c.entry(k), "receipt.json"), "w"))
            refused.append(c.lookup(k, None, chk) is None)
        k = "e" * 64
        os.makedirs(c.entry(k))
        json.dump({"version": basecache.VERSION, "key": k, "verdict": "pass", "rc": 0, "stdout": okrow + "\n",
                   "refcache_key": None, "records": None, "wall": 1.0, "load": 1.0, "time": "-"},
                  open(os.path.join(c.entry(k), "receipt.json"), "w"))
        good = c.lookup(k, None, chk) is not None
        try:
            c.store("f" * 64, [], [], 1, badrow, 1.0, 1.0, None, None)
            store_refused = False
        except ValueError:
            store_refused = True
        print("   Cache.lookup refuses stored failing runs: %s; accepts a passing one: %s; store(rc 1) refused: %s" % (
            refused, good, store_refused))
        if not all(refused) or not good or not store_refused or os.path.exists(c.entry("f" * 64)):
            fails.append("lookup/store of failing runs: %s %s %s" % (refused, good, store_refused))

        # 4. TimeoutPolicy with a cached wall time
        loads = [10.0]
        walls = [5.0, 7.0]
        p = mutate.TimeoutPolicy(None, lambda: walls.pop(0), loadavg=lambda: loads[0], first_load=12.0, remeasure=True)
        v1 = p.current()  # load 10 vs recorded 12: the cached wall time stays
        b1 = p.base
        loads[0] = 30.0
        v2 = p.current()  # load moved > 50%: measured again (synchronously, or in the background since, where
        if hasattr(p, "wait"):  # this call returns the last limit scaled by the load ratio: 5 s x 10 x 30/12 = 125 s)
            p.wait()
        v3 = p.current()  # after the re-measurement: 7 s x 10
        print("4. TimeoutPolicy(first_load=12): %.0f s at load 10 (cached wall %.0f s kept); at load 30 %.0f s, after the "
              "re-measurement %.0f s (wall %.0f s)" % (v1, b1, v2, v3, p.base))
        if v1 != 60.0 or b1 != 5.0 or v2 not in (70.0, 125.0) or p.base != 7.0 or v3 != 70.0 or walls:
            fails.append("TimeoutPolicy: %s %s %s %s %s" % (v1, b1, v2, v3, p.base))
    finally:
        shutil.rmtree(TMPU, ignore_errors=True)
        if os.path.exists(os.path.join(ROOT, MUT)):
            os.remove(os.path.join(ROOT, MUT))
    for x in fails:
        print("FAIL", x)
    print("basecache_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

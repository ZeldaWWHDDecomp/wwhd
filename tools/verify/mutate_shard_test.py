#!/usr/bin/env python3
"""Self-test of mutate.py's sharded serial phases (2026-10-31, fasttools2 change 3).

The baseline gate and the --schemata check run as -range parts in parallel (verify.run_range_shards), their rows merged
as verify.py -shards merges them; record parts (ADDR.ref.LO-HI) and validation-state parts (ADDR-TAG-sSEED.val.LO-HI)
are joined (refcache.join_shard_dir), the check's .reach parts concatenated.

1. Harness level, lib2_sead_15 plus `setup 02759564 02759928 r3=0x5E000000` + `setupsnapshot` (so snapshots and validation
   states are written), -n 600, seeds 1 and 7, -nulltrap: 4 parts with -refcache-write/-snapcache-write print exactly the
   unsharded run's stdout (rc too); after the join every ADDR.ref, .snap and world blob is byte-identical to the
   unsharded run's, except that a part's first input of the setupsnapshot function (a validation input only there) is
   unrecorded (readers run it live), and the joined .val holds every validation state the unsharded .val holds, byte for byte (plus the
   parts' own first inputs). Reading runs (-only, -refcache/-snapcache with -refcache-check/-snapcache-check) on the
   joined cache print the live rows and exit 0; so does a -runlist subset (37,38,500) whose first input validates only
   there (its original runs live: 2 records, 1 live), on either cache. A part that fails (a missing executable) gives rc 2, problems, no table.
2. Campaign level, lib2_sead_15 --max 40 --n 300 seed 1 (and a --rerun of its survivors), --shards 4 against --shards 1,
   plain and --schemata --fork --reach-skip: the same classes and harness stdout per mutant and the same reach records
   (WWHD_MUT_REACH_DUMP); the log names the -range parts of the baseline and of the schema check.
Work files in a new directory under the verify scratch (removed when everything passed). Exit 0 when all pass.
"""
import glob
import json
import os
import shutil
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402
import specfile  # noqa: E402
import refcache  # noqa: E402


def val_entries(data):
    """{input: entry bytes} of a .val file"""
    hl = struct.unpack_from("<Q", data, 0)[0]
    n = struct.unpack_from("<Q", data, 8 + hl)[0]
    p, out = 16 + hl, {}
    for _ in range(n):
        idx = struct.unpack_from("<I", data, p)[0]
        ln = struct.unpack_from("<Q", data, p + 12)[0]
        out[idx] = data[p:p + 20 + ln]
        p += 20 + ln
    return out


def ref_records(data):
    """{input: record bytes or None (run live)} of an ADDR.ref file (u64 header length, header, records, offset table)"""
    tab = struct.unpack_from("<Q", data, len(data) - 8)[0]
    n = (len(data) - 8 - tab) // 8
    offs = struct.unpack_from("<%dQ" % n, data, tab)
    return {i: (data[o:o + 4 + struct.unpack_from("<I", data, o)[0]] if o else None) for i, o in enumerate(offs)}


def mutate(*args, env=None):
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py")] + list(args) + ["-j", "4", "--no-progress"],
                       cwd=ROOT, capture_output=True, text=True, env=env)
    return r.returncode, r.stdout + r.stderr


def receipts(d):
    return {json.load(open(f))["id"]: json.load(open(f)) for f in glob.glob(os.path.join(d, "mutant*.json"))}


def main():
    fails = []
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="mutate_shard_test-", dir=base)
    print("mutate_shard_test: files in %s" % W)
    unit = "lib2_sead_15"
    exe = verify.build(unit, jobs=4)
    spec = specfile.spec_path(unit, ROOT, verify.unit_dir(unit))
    hdr = verify.header_args(unit)
    sp = os.path.join(W, "sead_snap.txt")
    open(sp, "w").write(open(spec).read() + "\nsetup 02759564 02759928 r3=0x5E000000\nsetupsnapshot\n")
    keys = ["-refcache-key", "k" * 32, "-snapcache-key", "s" * 32]

    # 1. harness level
    for seed in ("1", "7"):
        cmd = [exe, "-spec", sp] + hdr + ["-noprogress", "-nulltrap", "-n", "600", "-seed", seed]
        ud, sd = os.path.join(W, "u-s" + seed), os.path.join(W, "s-s" + seed)
        os.mkdir(ud)
        os.mkdir(sd)
        u = subprocess.run(cmd + ["-refcache-write", ud, "-snapcache-write", ud] + keys, cwd=ROOT, capture_output=True,
                           text=True)
        res = verify.run_range_shards(cmd + ["-refcache-write", sd, "-snapcache-write", sd] + keys, 600, 4)
        bad = refcache.join_shard_dir(sd, 600)
        if res["problems"] or bad or res["rc"] != u.returncode or res["stdout"] != u.stdout:
            fails.append("seed %s: sharded run differs: rc %s/%d problems %s join %s\n%s\n---\n%s" % (
                seed, res["rc"], u.returncode, res["problems"], bad, res["stdout"][-600:], u.stdout[-600:]))
        uf, sf = sorted(os.listdir(ud)), sorted(os.listdir(sd))
        if uf != sf:
            fails.append("seed %s: files %s vs %s" % (seed, uf, sf))
        nval = 0
        for f in uf:
            a, b = open(os.path.join(ud, f), "rb").read(), open(os.path.join(sd, f), "rb").read() if f in sf else b""
            if f.endswith(".val"):
                ea, eb = val_entries(a), val_entries(b)
                nval += len(eb) - len(ea)
                if any(eb.get(i) != v for i, v in ea.items()) or len(eb) <= len(ea):
                    fails.append("seed %s: %s: joined validation states %s, unsharded %s" % (seed, f, sorted(eb), sorted(ea)))
            elif f.endswith(".ref") and a != b:
                # the parts' first inputs validate the snapshot in their part only: left unrecorded (live), every other
                # record byte-identical
                ra, rb = ref_records(a), ref_records(b)
                odd = [i for i in ra if ra[i] != rb.get(i) and not (rb.get(i) is None and i in (150, 300, 450) and f == "02759564.ref")]
                if odd or len(ra) != len(rb):
                    fails.append("seed %s: %s: records of inputs %s differ" % (seed, f, odd[:10]))
            elif a != b:
                fails.append("seed %s: %s differs between the unsharded and the joined sharded cache" % (seed, f))
        rows = [l for l in u.stdout.splitlines() if l[:8].strip() and l[8:10] == "  " and l[:8] != "addr    "]
        for row in rows[:3] + ["02759564"]:
            f = row[:8]
            live = subprocess.run(cmd + ["-only", f], cwd=ROOT, capture_output=True, text=True)
            rd = subprocess.run(cmd + ["-only", f, "-refcache", sd, "-snapcache", sd] + keys + ["-refcache-check", "-snapcache-check"],
                                cwd=ROOT, capture_output=True, text=True)
            want = "597 input(s) took the recorded original, 3 ran it live" if f == "02759564" else \
                "600 input(s) took the recorded original, 0 ran it live"
            if rd.returncode != live.returncode or rd.stdout != live.stdout or want not in rd.stderr:
                fails.append("seed %s %s: reading the joined cache: rc %d/%d %s" % (seed, f, rd.returncode, live.returncode,
                                                                                  rd.stderr[-300:]))
        # a -runlist subset whose first input is not a validation input elsewhere runs that input's original live
        rl = os.path.join(W, "runlist-s%s.txt" % seed)
        open(rl, "w").write("02759564 37,38,500\n")
        for d in (ud, sd):
            live = subprocess.run(cmd + ["-only", "02759564", "-runlist", rl], cwd=ROOT, capture_output=True, text=True)
            rd = subprocess.run(cmd + ["-only", "02759564", "-runlist", rl, "-refcache", d, "-snapcache", d] + keys +
                                ["-refcache-check", "-snapcache-check"], cwd=ROOT, capture_output=True, text=True)
            if rd.returncode or rd.stdout != live.stdout or "refcache: 2 input(s) took the recorded original, 1 ran it live" not in rd.stderr:
                fails.append("seed %s runlist 37,38,500 on %s: rc %d %s" % (seed, os.path.basename(d), rd.returncode, rd.stderr[-300:]))
        print("  harness seed %s: %d rows, %d files identical, %d extra validation state(s) from the parts" % (
            seed, len(rows), len(uf), nval))
    res = verify.run_range_shards([os.path.join(W, "no-such-exe")], 200, 2)
    if res["rc"] != 2 or not res["problems"] or res["stdout"]:
        fails.append("a failing part: %s" % res)

    # 2a. the part count rule
    import importlib.util
    spec_ = importlib.util.spec_from_file_location("mutate_mod", os.path.join(HERE, "mutate.py"))
    mm = importlib.util.module_from_spec(spec_)
    os.environ.pop("WWHD_MUT_MIN_SHARD_INPUTS", None)
    spec_.loader.exec_module(mm)
    for n_, k_, want in ((10000, 8, 8), (1000, 8, 4), (300, 4, 1), (249, 4, 1), (500, 4, 2), (10000, 1, 1)):
        if mm.shard_parts(n_, k_) != want:
            fails.append("shard_parts(%d, %d) = %d, want %d" % (n_, k_, mm.shard_parts(n_, k_), want))
    if mm.MIN_SHARD_INPUTS != 250:
        fails.append("MIN_SHARD_INPUTS default %d, want 250" % mm.MIN_SHARD_INPUTS)

    # 2. campaign level (n 300 stays 4 parts with the self-test override of the 250-input minimum)
    main1 = os.path.join(W, "main-k1")
    for mode, extra in (("plain", []), ("fast", ["--schemata", "--fork", "--reach-skip"])):
        res = {}
        for k in ("1", "4"):
            rd = os.path.join(W, "%s-k%s" % (mode, k))
            env = dict(os.environ, WWHD_MUT_REACH_DUMP=rd + ".reach.json", WWHD_MUT_MIN_SHARD_INPUTS="50")
            rc, out = mutate(unit, "--max", "40", "--n", "300", "--seed", "1", "--nulltrap", "--shards", k, "--receipts", rd,
                             *extra, env=env)
            if rc:
                fails.append("%s --shards %s: rc %d %s" % (mode, k, rc, out[-400:]))
            if k == "4" and "baseline gate as 4 -range parts" not in out:
                fails.append("%s --shards 4: the baseline did not run sharded: %s" % (mode, out[-600:]))
            if k == "4" and mode == "fast" and "-range parts" not in out.split("baseline gate as")[-1]:
                fails.append("fast --shards 4: the schema check did not run sharded")
            if k == "1" and "-range parts" in out:
                fails.append("%s --shards 1: something ran sharded" % mode)
            res[k] = receipts(rd)
            if mode == "plain" and k == "1":
                shutil.copytree(rd, main1)
        a, b = res["1"], res["4"]
        diff = [i for i in sorted(set(a) | set(b)) if (a.get(i, {}).get("class"), a.get(i, {}).get("stdout")) !=
                (b.get(i, {}).get("class"), b.get(i, {}).get("stdout"))]
        if diff or not a:
            fails.append("%s: %d of %d mutants differ between 1 and 4 parts (first %s)" % (mode, len(diff), len(a), diff[:5]))
        if mode == "fast":
            ra, rb = (open(os.path.join(W, "fast-k%s.reach.json" % k)).read() for k in ("1", "4"))
            if ra != rb or len(ra) < 10:
                fails.append("fast: the reach records differ between 1 and 4 parts")
        cls = {}
        for j in a.values():
            cls[j["class"]] = cls.get(j["class"], 0) + 1
        print("  campaign %s: %d mutants identical with 1 and 4 parts, classes %s" % (mode, len(a) - len(diff), cls))
    # a --rerun of the survivors (scoped baseline, change 2) with and without parts
    rin = os.path.join(W, "in")
    os.mkdir(rin)
    for j in receipts(main1).values():
        if j["class"] in ("survived", "difference"):
            shutil.copy(os.path.join(main1, "mutant%d.json" % j["id"]), rin)
            shutil.copy(os.path.join(main1, "mutant%d.cpp" % j["id"]), rin)
    rr = {}
    for k in ("1", "4"):
        rd = os.path.join(W, "rerun-k%s" % k)
        rc, out = mutate(unit, "--rerun", rin, "--rerun-class", "survived,difference", "--n", "1000", "--seed", "7",
                         "--nulltrap", "--shards", k, "--receipts", rd)
        if rc:
            fails.append("rerun --shards %s: rc %d %s" % (k, rc, out[-400:]))
        rr[k] = {i: (j["class"], j.get("stdout")) for i, j in receipts(rd).items()}
    if rr["1"] != rr["4"] or not rr["1"]:
        fails.append("rerun: classes/stdout differ between 1 and 4 parts")
    print("  rerun: %d mutants identical with 1 and 4 parts" % len(rr["1"]))
    for x in fails:
        print("FAIL", x)
    if not fails:
        shutil.rmtree(W, ignore_errors=True)
    print("mutate_shard_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

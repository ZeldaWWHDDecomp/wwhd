#!/usr/bin/env python3
"""Self-test of the gate-record reuse (2026-10-30): the gate's raw logs are the coverage evidence, and survivor replays reuse the gate's
-nulltrap reference-side records.

1. refcache.join on synthetic parts: joined bytes equal the single file; gaps, overlaps, other headers and records
   outside a part's range are refused, and so are joined records over WWHD_REFCACHE_MAX_MB.
2. Coverage from the gate (lib2_sead_15 and lib2_agl_1, n 300, seed 1, -shards 2): `nulltrap_compare.py --logdir L
   --refcache-out R` and separate `verify.py -v -v` runs (-nulltrap and plain). The gate's raw -nulltrap log equals the
   separate -nulltrap -v -v stdout byte for byte (after the build line), and L/SPEC.s1.coverage.txt gives, per function
   and side, the same rows, coverage and uncovered blocks as the separate runs. The verdict output equals a run without
   --logdir/--refcache-out apart from the added record line.
3. The gate's records (sharded, written by the -dsiout run and joined) are byte-identical to the files an unsharded
   harness run in mutate.py's configuration writes with the same key, for every function of both specs.
4. Replays (lib2_sead_15, 0275A86C and 0275BC8C): a 30-mutant --nulltrap campaign at n 300 seed 1 (kills and survivors), then
   `--rerun` of its difference/survived/abort mutants with --refcache-from R and without: same class and same harness stdout for every mutant;
   the reusing run says so, runs no baseline original (every baseline input from the records) and its receipts name
   the directory; with WWHD_REFCACHE_CHECK=1 (live original compared with each record) the classes are the same again.
   A different n, a different input seed and a manifest with another key are refused (the baseline records its own).
   With --baseline-cache: on a miss the gate's records win and nothing is stored; a later hit wins over --refcache-from.
Temporary files in a private WWHD_VERIFY_SCRATCH directory, removed at exit. Exit 0 when all cases pass.
"""
import glob
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import refcache  # noqa: E402
import nulltrap_compare as nc  # noqa: E402

N, SEED = 300, 1
SPECS = ("lib2_sead_15", "lib2_agl_1")
MUT_UNIT, MUT_FUNC = "lib2_sead_15", "0275A86C,0275BC8C"


def run(cmd, env=None):
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, errors="replace",
                       env=dict(os.environ, WWHD_VERIFY_PROGRESS="0", **(env or {})))
    return r.returncode, r.stdout, r.stderr


def body(out):
    """verify output without the build line (`unit ...: N functions under test ... -> DIR`)"""
    return "\n".join(l for l in out.splitlines() if not l.startswith("unit ") or "functions under test" not in l)


def join_cases(fails):
    head = struct.pack("<Q", 5) + b"HEADR"
    recs = [b"aa", b"bbb", b"", b"c", b"dd", b"e"]  # input 2 runs live (offset 0)

    def mk(lo, hi, n=6, h=head, extra=()):
        b, off = bytearray(h), [0] * n
        for i in list(range(lo, hi)) + list(extra):
            if recs[i]:
                off[i] = len(b)
                b += struct.pack("<I", len(recs[i])) + recs[i]
        tab = len(b)
        return bytes(b) + struct.pack("<%dQ" % n, *off) + struct.pack("<Q", tab)
    whole = mk(0, 6)
    j, why = refcache.join([(3, 6, mk(3, 6)), (0, 3, mk(0, 3))], 6)
    if j != whole:
        fails.append("join: joined parts differ from the single file (%s)" % why)
    for parts, what in (([(0, 3, mk(0, 3)), (4, 6, mk(4, 6))], "a gap"), ([(0, 4, mk(0, 4)), (3, 6, mk(3, 6))], "an overlap"),
                        ([(0, 3, mk(0, 3)), (3, 6, mk(3, 6, h=struct.pack("<Q", 5) + b"OTHER"))], "another header"),
                        ([(0, 3, mk(0, 3, extra=(4,))), (3, 6, mk(3, 6))], "a record outside its range"),
                        ([(0, 3, mk(0, 3)[:-3]), (3, 6, mk(3, 6))], "a truncated part")):
        if refcache.join(parts, 6)[0] is not None:
            fails.append("join: %s was not refused" % what)
    os.environ["WWHD_REFCACHE_MAX_MB"] = str(20.0 / (1 << 20))  # 20 bytes: header 13 + records 4+2 + 4+3 fits, all do not
    try:
        if refcache.join([(0, 2, mk(0, 2)), (2, 6, mk(2, 6))], 6)[0] is not None:
            fails.append("join: records over WWHD_REFCACHE_MAX_MB were not refused")
    finally:
        del os.environ["WWHD_REFCACHE_MAX_MB"]


def parse_rows(out):
    """{addr: (gen, cov, uncovered)} of a -v -v table"""
    q = nc.parse(out, "")
    return {f: (r["gen"], r["cov"], sorted(r["uncovered"])) for f, r in q.items()}


def parse_coverage(text, side):
    out = {}
    for l in text.splitlines():
        m = re.match(r"^([0-9A-F]{8}) (\S+)\s+rows (\d+)/(\d+) coverage (\d+)/(\d+)(?: clean \d+/\d+)? uncovered: ([0-9 -]+?)(?: \[.*\])?$", l)
        if m and m.group(2) == side:
            unc = [] if m.group(7) == "-" else [int(x) for x in m.group(7).split()]
            out[m.group(1)] = ((int(m.group(3)), int(m.group(4))), (int(m.group(5)), int(m.group(6))), unc)
    return out


def gate_cases(fails, tmp):
    L, R = os.path.join(tmp, "logs"), os.path.join(tmp, "rc")
    ntc = [sys.executable, os.path.join(HERE, "nulltrap_compare.py")]
    common = ["-n", str(N), "-seed", str(SEED), "-shards", "2", "-buildj", "4", "--archive"]
    import verify
    import specfile
    for spec in SPECS:
        g = run(ntc + [spec] + common + ["--logdir", L, "--refcache-out", R])
        p = run(ntc + [spec] + common)
        if g[0] != p[0] or g[0] not in (0, 1):
            fails.append("%s: gate exit %d / %d: %s" % (spec, g[0], p[0], g[2][-400:]))
            continue
        extra = [l for l in g[1].splitlines() if l not in p[1].splitlines()]
        if [l for l in p[1].splitlines() if l not in g[1].splitlines()] or len(extra) != 1 or "reference-side records" not in extra[0]:
            fails.append("%s: the gate's verdict output changed with --logdir/--refcache-out: %s" % (spec, extra))
        vq = run([sys.executable, os.path.join(HERE, "verify.py"), spec, "-n", str(N), "-seed", str(SEED), "-shards", "2",
                  "--archive", "-v", "-v", "-nulltrap"])
        vp = run([sys.executable, os.path.join(HERE, "verify.py"), spec, "-n", str(N), "-seed", str(SEED), "-shards", "2",
                  "--archive", "-v", "-v"])
        raw = open(os.path.join(L, "%s.s%d.nulltrap.out" % (spec, SEED))).read()
        if body(raw) != body(vq[1]):
            fails.append("%s: the gate's raw -nulltrap log differs from a separate -nulltrap -v -v run" % spec)
        cov = open(os.path.join(L, "%s.s%d.coverage.txt" % (spec, SEED))).read()
        for side, v in (("-nulltrap", vq[1]), ("plain", vp[1])):
            want, have = parse_rows(v), parse_coverage(cov, side)
            if not want or want != have:
                bad = sorted(f for f in set(want) | set(have) if want.get(f) != have.get(f))
                fails.append("%s: %s coverage from the gate log differs from a separate -v -v run: %s" % (
                    spec, side, ["%s %s / %s" % (f, want.get(f), have.get(f)) for f in bad[:3]] or "no rows"))
        if not all(os.path.exists(os.path.join(L, "%s.s%d.%s" % (spec, SEED, x))) for x in ("nulltrap.err", "dsiout.txt")):
            fails.append("%s: raw stderr / -dsiout file not archived" % spec)
        # 3. the gate's joined records == an unsharded harness run in mutate.py's configuration with the same key
        d, m = refcache.gate_dir(R, SEED, spec)
        if d is None:
            fails.append("%s: %s" % (spec, m))
            continue
        exe = verify.build(spec, jobs=4)
        out = verify.unit_dir(spec)
        sp = specfile.spec_path(spec, ROOT, out)
        hdr = verify.header_args(spec)
        scope = None
        if "-only" in hdr:
            i = hdr.index("-only")
            scope = {int(x, 16) for x in hdr[i + 1].split(",")}
            hdr = hdr[:i] + hdr[i + 2:]
        args = refcache.harness_args(sp, N, SEED, hdr, True, False, scope, verify.unit_rec(spec))
        k = refcache.key(out, sp, args, verify.unit_rec(spec), ROOT, verify.support_objs())
        if k != m.get("key"):
            fails.append("%s: the manifest key is not mutate.py's key for this configuration" % spec)
        w = os.path.join(tmp, "direct-" + spec)
        os.makedirs(w)
        run([exe] + args + ["-refcache-write", w, "-refcache-key", k])
        mine = sorted(os.path.basename(f) for f in glob.glob(os.path.join(d, "*.ref")))
        ref = sorted(os.path.basename(f) for f in glob.glob(os.path.join(w, "*.ref")))
        diff = [f for f in ref if f not in mine or open(os.path.join(d, f), "rb").read() != open(os.path.join(w, f), "rb").read()]
        if not ref or mine != ref or diff:
            fails.append("%s: gate records differ from an unsharded mutate-configuration run (%d/%d files, %d differ: %s)" % (
                spec, len(mine), len(ref), len(diff), diff[:3]))
        if m.get("records") != [f[:8] for f in mine] or sorted(m.get("functions", [])) != [f[:8] for f in ref]:
            fails.append("%s: manifest records/functions do not list the files" % spec)
        if glob.glob(os.path.join(d, "*.ref.*")):
            fails.append("%s: shard parts left behind" % spec)
    return R


def mutate(*args, env=None):
    r = run([sys.executable, os.path.join(HERE, "mutate.py"), MUT_UNIT, "--n", str(N), "-j", "4", "--no-progress", "--nulltrap",
             "--no-tce"] + list(args), env=env)
    return r[0], r[1] + r[2]


def classes(d):
    out = {}
    for f in glob.glob(os.path.join(d, "mutant*.json")):
        j = json.load(open(f))
        out[j["id"]] = (j["class"], j.get("stdout"))
    return out


def replay_cases(fails, tmp, R):
    A = os.path.join(tmp, "A")
    rc, o = mutate("--func", MUT_FUNC, "--max", "30", "--seed", str(SEED), "--receipts", A)
    ca = classes(A)
    kinds = {c for c, _ in ca.values()}
    if rc not in (0, 1) or not ca or "difference" not in kinds or "survived" not in kinds:
        fails.append("campaign: rc %d, classes %s: %s" % (rc, sorted(kinds), o[-400:]))
        return
    kinds -= {"broken"}
    allc = ",".join(sorted(kinds))
    ca = {i: c for i, c in ca.items() if c[0] in kinds}
    base = ["--rerun", A, "--rerun-class", allc, "--input-seed", str(SEED)]
    B, C, D = (os.path.join(tmp, x) for x in "BCD")
    rb, ob = mutate(*base, "--receipts", B, "--refcache-from", R)
    rcx, oc = mutate(*base, "--receipts", C)
    rd, od = mutate(*base, "--receipts", D, "--refcache-from", R, env={"WWHD_REFCACHE_CHECK": "1"})
    cb, cc, cd = classes(B), classes(C), classes(D)
    if not cb or sorted(cb) != sorted(cc) or sorted(cb) != sorted(ca):
        fails.append("replay ids: %s / %s / %s (%s)" % (sorted(cb), sorted(cc), sorted(ca), (ob + oc)[-400:]))
    for i in sorted(set(cb) & set(cc)):
        if cb[i] != cc[i]:
            fails.append("mutant %d: class/stdout with the gate's records %s, without %s" % (i, cb[i][0], cc[i][0]))
        if cd.get(i, (None,))[0] != cc[i][0]:
            fails.append("mutant %d: WWHD_REFCACHE_CHECK=1 class %s, without reuse %s" % (i, cd.get(i, (None,))[0], cc[i][0]))
    if "reusing the gate's records" not in ob or not re.search(r"baseline against the gate's records: \d+ input\(s\) from the records, 0 run live", ob):
        fails.append("the reusing replay did not report the reuse: %s" % ob[-500:])
    if "reusing the gate's records" in oc:
        fails.append("the replay without --refcache-from reused records")
    if "CHECK FAILED" in od or rd not in (0, 1):
        fails.append("WWHD_REFCACHE_CHECK=1 replay: rc %d %s" % (rd, od[-300:]))
    j = json.load(open(glob.glob(os.path.join(B, "mutant*.json"))[0]))
    if not j.get("refcache_from"):
        fails.append("receipts of the reusing replay do not name the records")
    print("replay: %d mutants, classes %s; reuse == live for all" % (len(cb), {k: sum(1 for c, _ in cb.values() if c == k) for k in kinds}))
    # refusals: other n, other input seed, another key
    for extra, what in ((["--n", "200"], "key differs"), (["--input-seed", "7"], "no refcache.json")):  # -n is in the key
        a = [x for x in base if x not in ("--input-seed", str(SEED))] if "--input-seed" in extra else base
        r2, o2 = mutate(*(a + extra), "--receipts", os.path.join(tmp, "E" + extra[1]), "--refcache-from", R)
        if "not used" not in o2 or what not in o2 or "reusing" in o2:
            fails.append("--refcache-from with %s was not refused: %s" % (" ".join(extra), o2[-300:]))
    # with --baseline-cache: a miss + matching gate records -> the gate wins, nothing stored; a hit wins over the gate
    BC = os.path.join(tmp, "bc")
    r4, o4 = mutate(*base, "--receipts", os.path.join(tmp, "G1"), "--refcache-from", R, "--baseline-cache", BC)
    r5, o5 = mutate(*base, "--receipts", os.path.join(tmp, "G2"), "--baseline-cache", BC)
    r6, o6 = mutate(*base, "--receipts", os.path.join(tmp, "G3"), "--refcache-from", R, "--baseline-cache", BC)
    if "reusing the gate's records" not in o4 or "recorded in the baseline cache" in o4:
        fails.append("--baseline-cache miss + --refcache-from: %s" % o4[-400:])
    if "recorded in the baseline cache" not in o5:
        fails.append("--baseline-cache alone did not store: %s" % o5[-400:])
    if "cached baseline" not in o6 or "reusing the gate's records" in o6 or "not used: the baseline cache" not in o6:
        fails.append("--baseline-cache hit + --refcache-from: %s" % o6[-400:])
    for x in ("G1", "G2", "G3"):
        cx = classes(os.path.join(tmp, x))
        if {i: c[0] for i, c in cx.items()} != {i: c[0] for i, c in cc.items()}:
            fails.append("%s: classes differ from the live replay" % x)
    d, m = refcache.gate_dir(R, SEED, MUT_UNIT)
    bad = os.path.join(tmp, "badkey", MUT_UNIT, "s%d" % SEED)
    shutil.copytree(d, bad)
    m["key"] = "0" * 32
    refcache.write_manifest(bad, **m)
    r3, o3 = mutate(*base, "--receipts", os.path.join(tmp, "F"), "--refcache-from", os.path.join(tmp, "badkey"))
    if "key differs" not in o3 or "reusing" in o3:
        fails.append("a manifest with another key was not refused: %s" % o3[-300:])


def main():
    import atexit
    tmp = tempfile.mkdtemp(prefix="wwhd-refcache_reuse_test-", dir=os.environ.get("WWHD_VERIFY_SCRATCH") or None)
    os.environ["WWHD_VERIFY_SCRATCH"] = tmp
    atexit.register(shutil.rmtree, tmp, True)
    fails = []
    join_cases(fails)
    R = gate_cases(fails, tmp)
    replay_cases(fails, tmp, R)
    for f in fails:
        print("FAIL", f)
    print("refcache_reuse_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

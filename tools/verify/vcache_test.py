#!/usr/bin/env python3
"""Self-test of verify.py's result cache (vcache.py, 2026-10-27), on lib2_agl_6 (40 functions, 4 sources):
1. sharded (-shards 2) and unsharded -v -v runs: --no-cache, then cached twice; stdout byte-identical in all three,
   the second cached run takes every row from the cache (`40 of 40`), and the nulltrap notes on stderr are the same;
2. a temporary copy of the spec whose TimingHistory.cpp copy gets one source line changed (createTimingHistory
   0278BBA8: 95.0f -> 96.0f): exactly that function and its native caller createTimingSingleton 0278BC9C miss,
   the other 38 hit; restoring the line hits all 40 again;
3. a changed spec line (an added `stable` line) and a changed tool file (a temporary file under tools/verify) miss
   for every function;
4. --archive and WWHD_VERIFY_ARCHIVE=1 never print a cached row; WWHD_VERIFY_CACHE=0 / --no-cache run uncached;
5. LRU eviction: entries above WWHD_VERIFY_CACHE_MAX_MB are removed oldest first;
6. nulltrap_compare (both phases: -nulltrap -dsiout, then -runlist) prints the same report archived, filling, cached.
Builds and the cache go to a private WWHD_VERIFY_SCRATCH directory (below an existing one when set); temporary
files are removed.
"""
import atexit
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
UNITS = os.path.join(HERE, "units")
TMPU = os.path.join(UNITS, "_vcache_test")
SRC = "wwhd_src/agl/TimingHistory.cpp"
COPY = "wwhd_src/agl/_vcache_test_TimingHistory.cpp"
TOOLFILE = os.path.join(HERE, "_vcache_test_toolfile.txt")
EDIT = ("timing->mPreset = 95.0f;", "timing->mPreset = 96.0f;")
ARGS = ["-n", "20", "-seed", "1", "-nulltrap", "-noprogress", "-buildj", "4"]


def run(unit, *extra, env=None):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit] + ARGS + list(extra), cwd=ROOT,
                       capture_output=True, text=True, env=dict(os.environ, **(env or {})))
    if r.returncode not in (0, 1):
        raise SystemExit("verify.py %s failed (%d):\n%s" % (unit, r.returncode, r.stderr[-2000:]))
    m = re.search(r"vcache: (\d+) of (\d+) function", r.stderr)
    cached = set(re.findall(r"vcache: ([0-9A-F]{8}) cached", r.stderr))
    return r.stdout, r.stderr, (int(m.group(1)), int(m.group(2))) if m else None, cached


def rows(out):
    return [l for l in out.splitlines() if re.match(r"^[0-9A-F]{8}  ", l)]


def notes(err):
    return sorted(l for l in err.splitlines() if l.startswith("nulltrap: "))


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    scratch = tempfile.mkdtemp(prefix="wwhd-vcache_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = scratch
    for k in ("WWHD_VERIFY_ARCHIVE", "WWHD_VERIFY_CACHE", "WWHD_VERIFY_CACHE_MAX_MB"):
        os.environ.pop(k, None)
    atexit.register(shutil.rmtree, scratch, True)
    fails = []

    def check(cond, what):
        print("%s %s" % ("ok  " if cond else "FAIL", what), flush=True)
        if not cond:
            fails.append(what)

    try:
        # 1. identical rows, cached vs uncached, sharded and unsharded -v -v
        for mode in (["-shards", "2"], ["-v", "-v"], ["-v", "-v", "-shards", "2"]):
            o0, e0, s0, _ = run("lib2_agl_6", *mode, "--no-cache")
            o1, e1, s1, _ = run("lib2_agl_6", *mode)
            o2, e2, s2, c2 = run("lib2_agl_6", *mode)
            check(s0 is None and len(rows(o0)) == 40, "%s --no-cache: 40 rows, no cache line" % mode)
            check(s1 == (0, 40) and s2 == (40, 40) and len(c2) == 40, "%s: first run fills, second takes 40/40 (%s, %s)" % (mode, s1, s2))
            check(o0 == o1 == o2, "%s: stdout byte-identical (uncached, filling, cached)" % mode)
            check(notes(e0) == notes(e1) == notes(e2), "%s: nulltrap notes identical" % mode)
        # 2. a changed source line: the function and its native caller miss, the rest hit
        os.makedirs(TMPU, exist_ok=True)
        text = open(os.path.join(ROOT, SRC)).read()
        if text.count(EDIT[0]) != 1:
            raise SystemExit("edit site not found once in %s" % SRC)
        spec = open(os.path.join(UNITS, "lib2_agl_6.txt")).read().replace("src " + SRC, "src " + COPY)
        open(os.path.join(TMPU, "a.txt"), "w").write(spec)
        open(os.path.join(ROOT, COPY), "w").write(text)
        u = "_vcache_test/a"
        oa, _, sa, _ = run(u, "-shards", "2")
        ob, _, sb, _ = run(u, "-shards", "2")
        check(sa == (0, 40) and sb == (40, 40) and oa == ob, "copy spec: fills, then 40/40 identical")
        open(os.path.join(ROOT, COPY), "w").write(text.replace(*EDIT))
        oc, _, sc, cc = run(u, "-shards", "2")
        missed = {r[:8] for r in rows(oc)} - cc
        check(sc == (38, 40) and missed == {"0278BBA8", "0278BC9C"}, "edited createTimingHistory: misses %s (%s)" % (sorted(missed), sc))
        check([r for r in rows(oc) if not r.startswith(("0278BBA8", "0278BC9C"))] ==
              [r for r in rows(oa) if not r.startswith(("0278BBA8", "0278BC9C"))], "edited: the 38 cached rows are the old rows")
        open(os.path.join(ROOT, COPY), "w").write(text)
        od, _, sd, _ = run(u, "-shards", "2")
        check(sd == (40, 40) and od == oa, "restored line: 40/40 cached, identical rows")
        # 3. a changed spec line and a changed tool file miss everywhere
        open(os.path.join(TMPU, "a.txt"), "w").write(spec + "stable 7F000000 4\n")
        _, _, se, _ = run(u, "-shards", "2")
        check(se == (0, 40), "added spec line: 0/40 cached (%s)" % (se,))
        open(os.path.join(TMPU, "a.txt"), "w").write(spec)
        open(TOOLFILE, "w").write("vcache_test\n")
        _, _, sf, _ = run(u, "-shards", "2")
        check(sf == (0, 40), "changed tool file: 0/40 cached (%s)" % (sf,))
        os.remove(TOOLFILE)
        # 4. archive mode and the switches
        for extra, env, what in ((["--archive"], None, "--archive"), ([], {"WWHD_VERIFY_ARCHIVE": "1"}, "WWHD_VERIFY_ARCHIVE=1"),
                                 (["--no-cache"], None, "--no-cache"), ([], {"WWHD_VERIFY_CACHE": "0"}, "WWHD_VERIFY_CACHE=0")):
            o, e, s, c = run(u, "-shards", "2", *extra, env=env)
            check(s is None and not c and o == oa and ("archive mode" in e) == ("ARCHIVE" in what or "archive" in what),
                  "%s: no cached row, identical rows" % what)
        # 6. nulltrap_compare's two phases (-nulltrap with -dsiout, then -runlist): archive vs filling vs cached
        def ntc(*extra):
            r = subprocess.run([sys.executable, os.path.join(HERE, "nulltrap_compare.py"), "lib2_agl_6", "-n", "200", "-seed", "1",
                                "-seed", "7", "-shards", "2", "-buildj", "4"] + list(extra), cwd=ROOT, capture_output=True, text=True)
            return r.returncode, r.stdout
        a0, a1, a2 = ntc("--archive"), ntc(), ntc()
        check(a0 == a1 == a2 and "nulltrap_compare: lib2_agl_6:" in a0[1],
              "nulltrap_compare (two phases): archive, filling and cached runs print the same report")
        # 5. LRU eviction
        import vcache
        d = os.path.join(scratch, "vcache", "zz")
        os.makedirs(d, exist_ok=True)
        now = time.time()
        for i in range(4):
            p = os.path.join(d, "zz%d.json" % i)
            open(p, "w").write("x" * 400000)
            os.utime(p, (now - 1000 + i, now - 1000 + i))
        os.environ["WWHD_VERIFY_CACHE_MAX_MB"] = "1"
        s = vcache.Session.__new__(vcache.Session)
        s.cdir = os.path.join(scratch, "vcache")
        s.evict()
        left = sorted(f for f in os.listdir(d))
        check("zz0.json" not in left and "zz3.json" in left, "eviction removes the oldest entries first (%s)" % left)
    finally:
        for p in (os.path.join(ROOT, COPY), TOOLFILE):
            if os.path.exists(p):
                os.remove(p)
        shutil.rmtree(TMPU, ignore_errors=True)
    print("vcache_test: %s" % ("OK" if not fails else "FAIL (%d)" % len(fails)))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

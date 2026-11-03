#!/usr/bin/env python3
"""Self-test of verify.py's sharded merge against incomplete shards (2026-10-27, : under load a shard of
lib2_nw_snd_6 ended early and the merge printed its last 28 functions with 7500 of 10000 inputs, which nulltrap_compare
read as ok).
1. shard_problems() on fixed shard outputs: a complete run passes; a shard killed by a signal, one that exits 0 without
   its `@SHARDEND` marker, one whose marker count disagrees, and one missing a function the others report are each
   named; a shard stopped on purpose (-stopfirst, marker flag 1) is not asked for the other shards' functions.
2. A real run: verify.py lib2_sead_1 -n 4000 -shards 4, shard 2 killed (SIGKILL) while it runs: exit 2, an
   `INCOMPLETE sharded run: shard 2 (inputs 2000:3000) killed by SIGKILL` line, and no result row at all.
   The same run without the kill: exit 0, every row 4000/4000. Both without the result cache (WWHD_VERIFY_CACHE=0).
3. With the result cache (vcache, a fresh seed so no key exists yet): two functions cached by a first run (-only),
   then a full run with shard 2 killed: INCOMPLETE, exit 2, no row (the cached rows are not printed either), and it
   stores nothing (the next run reports `2 of N function(s) cached`); a further run takes every row from the cache,
   identical to the complete one.
Builds go to WWHD_VERIFY_SCRATCH when set (as verify.py). Exit 0 when all cases pass.
"""
import os
import re
import signal
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

fails = []
ROWS = ["@SHARD 0279D92C 10 10 0 0 0 0 -1 3 0,1|f|", "@SHARD 0279DB64 10 10 0 0 0 0 -1 2 0|g|"]
R = [(0, 10), (10, 20)]


def probs(outs, codes):
    return verify.shard_problems(outs, codes, R)


full = "\n".join(ROWS + ["@SHARDEND 2 0"]) + "\n"
if probs([full, full], [0, 0]):
    fails.append("complete run reported: %s" % probs([full, full], [0, 0]))
cut = ROWS[0] + "\n"  # killed after its first function: the second function's rows are missing
p = probs([full, cut], [0, -9])
if len(p) != 1 or "shard 1 (inputs 10:20) killed by SIGKILL after 1 function row(s) (last 0279D92C)" not in p[0]:
    fails.append("killed shard: %s" % p)
p = probs([full, cut], [0, 0])
if len(p) != 1 or "no end marker" not in p[0]:
    fails.append("cut shard with exit 0: %s" % p)
p = probs([full, ROWS[0] + "\n@SHARDEND 2 0\n"], [0, 0])
if len(p) != 1 or "end marker says 2 function row(s), 1 read" not in p[0]:
    fails.append("marker count: %s" % p)
p = probs([full, ROWS[0] + "\n@SHARDEND 1 0\n"], [0, 1])
if len(p) != 1 or "no row for 1 function(s)" not in p[0] or "0279DB64" not in p[0]:
    fails.append("missing function: %s" % p)
if probs([full, ROWS[0] + "\n@SHARDEND 1 1\n"], [0, 1]):
    fails.append("-stopfirst shard reported: %s" % probs([full, ROWS[0] + "\n@SHARDEND 1 1\n"], [0, 1]))
p = probs([full, None], [0, None])
if len(p) != 1 or "shard 1" not in p[0]:
    fails.append("shard without output (thread failed): %s" % p)

# 2. a real sharded run with one shard killed
UNIT, N = "lib2_sead_1", 4000
env = dict(os.environ, WWHD_VERIFY_PROGRESS="0", WWHD_VERIFY_CACHE="0")
subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-n", "1", "-buildj", "4"], cwd=ROOT, env=env,
               capture_output=True)  # build first: the timing below is the run only
row = re.compile(r"^[0-9A-F]{8}  ")


def run(kill, extra=(), e=env):
    p = subprocess.Popen([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-n", str(N), "-shards", "4"] + list(extra),
                         cwd=ROOT, env=e, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    killed = False
    if kill:
        t0 = time.time()
        while not killed and time.time() - t0 < 120 and p.poll() is None:
            kids = subprocess.run(["pgrep", "-P", str(p.pid)], capture_output=True, text=True).stdout.split()
            for pid in kids:
                args = subprocess.run(["ps", "-o", "args=", "-p", pid], capture_output=True, text=True).stdout
                if "-shardout" in args and "-range 2000:3000" in args:
                    time.sleep(1.5)
                    os.kill(int(pid), signal.SIGKILL)
                    killed = True
                    break
            time.sleep(0.2)
    out, err = p.communicate()
    return p.returncode, out, err, killed


rc, out, err, killed = run(True)
if not killed:
    fails.append("could not find shard 2 to kill (rc %s)" % rc)
elif rc != 2 or "INCOMPLETE sharded run: shard 2 (inputs 2000:3000) killed by SIGKILL" not in out or \
        any(row.match(l) for l in out.splitlines()):
    fails.append("killed shard: rc %s, out tail %r, err tail %r" % (rc, out[-400:], err[-400:]))
rc, out, err, _ = run(False)
rows = [l for l in out.splitlines() if row.match(l)]
if rc != 0 or not rows or any(" %d/%d " % (N, N) not in l for l in rows):
    fails.append("complete run: rc %s, %d rows, first bad %r" % (rc, len(rows), next((l for l in rows if " %d/%d " % (N, N) not in l), "")))
# 3. the cached path
if os.environ.get("WWHD_VERIFY_SCRATCH"):
    cenv = dict(env, WWHD_VERIFY_CACHE="1")
    seed = ["-seed", str(int(time.time()) % 1000000 + 1000)]  # fresh keys
    lst = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-list"], cwd=ROOT, env=env,
                         capture_output=True, text=True).stdout.split()
    lst = [a for a in lst if re.match(r"^[0-9A-F]{8}$", a)]
    nf = len(rows)
    two = ",".join(lst[:2])
    rc, out, err, _ = run(False, seed + ["-only", two], cenv)
    if rc != 0 or len([l for l in out.splitlines() if row.match(l)]) != 2:
        fails.append("3: filling two functions: rc %s\n%s" % (rc, err[-400:]))
    rc, out, err, killed = run(True, seed, cenv)
    if not killed or rc != 2 or "INCOMPLETE sharded run: shard 2 (inputs 2000:3000) killed by SIGKILL" not in out or \
            any(row.match(l) for l in out.splitlines()) or "vcache: 2 of %d function(s) cached" % nf not in err:
        fails.append("3: killed shard with cache hits: killed %s rc %s, out %r, err %r" % (killed, rc, out[-300:], err[-500:]))
    rc, out3, err, _ = run(False, seed, cenv)
    if rc != 0 or "vcache: 2 of %d function(s) cached" % nf not in err:
        fails.append("3: the killed run stored rows (or the rerun failed): rc %s, %r" % (rc, [l for l in err.splitlines() if "vcache" in l]))
    rc, out4, err, _ = run(False, seed, cenv)
    if rc != 0 or "vcache: %d of %d function(s) cached" % (nf, nf) not in err or out4 != out3:
        fails.append("3: fully cached run: rc %s, %r, identical %s" % (rc, [l for l in err.splitlines() if "vcache" in l], out4 == out3))
else:
    print("shard_merge_test: WWHD_VERIFY_SCRATCH unset: cached case skipped")
for f in fails:
    print("FAIL", f)
print("shard_merge_test:", "FAIL" if fails else "PASS")
sys.exit(1 if fails else 0)

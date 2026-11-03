#!/usr/bin/env python3
"""Self-test of verify.py --resume DIR and nulltrap_compare.py --resume DIR (2026-10-31: a killed gate
resumes per function instead of from input 0).

Unit lib2_sead_15 (53 functions), -n 300, seed 1, -v -v, --archive. Each run's stdout is compared with an uninterrupted
archive run (the reference):
 1. sharded (-shards 3): a run killed (whole process tree) once some functions have stored their parts, i.e. in the
    middle of a later function, then resumed with the same DIR: stdout byte-identical to the reference; stderr names
    the functions taken from DIR (`resume: ADDR from DIR`), and the function that was running at the kill is not among
    them (its parts were incomplete).
 2. unsharded: the same kill/resume, byte-identical; a second resume takes every function from DIR (no harness run of
    any function) and is byte-identical again.
 3. a changed key misses: the same DIR with WWHD_VERIFY_HANG_LIMIT=77 takes nothing from DIR; byte-identical.
 4. a damaged part (its stdout text altered, the stored hash left) and a truncated part of the default key are misses:
    those functions run again; byte-identical.
 5. archive mode without --resume never reads DIR (no `resume:` line).
 6. nulltrap_compare --resume DIR --logdir L, -n 300 seed 1: the second run prints the same rows and verdict, its
    `nulltrap_compare: resume:` lines name the functions per side, and L/SPEC.resume.json lists them; the page-0 side's
    part directory is keyed on the -dsiout content.
Files stay in a new directory under the verify scratch (printed; never removed). Exit 0 when all pass.
"""
import glob
import json
import os
import re
import signal
import subprocess
import sys
import tempfile
import time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

UNIT = "lib2_sead_15"
BASE = ["-n", "300", "-seed", "1", "-v", "-v", "-noprogress", "--archive", "-buildj", "8"]


def run(args, env=None):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT] + args, cwd=ROOT, capture_output=True,
                       text=True, errors="replace", env=env)
    return r.returncode, r.stdout, r.stderr


def parts(d):
    return glob.glob(os.path.join(d, "*", "*.json"))


def killed_run(args, d, want_parts):
    """start the run, kill its whole process group once DIR holds want_parts parts; returns the parts at the kill"""
    p = subprocess.Popen([sys.executable, os.path.join(HERE, "verify.py"), UNIT] + args, cwd=ROOT,
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, start_new_session=True)
    t = time.time()
    while p.poll() is None and len(parts(d)) < want_parts and time.time() - t < 600:
        time.sleep(0.05)
    finished = p.poll() is not None
    try:
        os.killpg(p.pid, signal.SIGKILL)
    except ProcessLookupError:
        pass
    p.wait()
    return finished, len(parts(d))


def from_dir(err):
    return set(re.findall(r"^resume: ([0-9A-F]{8}) from ", err, re.M))


def main():
    fails = []
    verify.build(UNIT, jobs=8)
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="resume_test-", dir=base)
    print("resume_test: files in %s (not removed)" % W)
    rc, ref, err = run(BASE)
    if rc not in (0, 1) or "resume:" in err:
        fails.append("reference run: rc %d" % rc)
    nfun = len([l for l in ref.splitlines() if re.match(r"^[0-9A-F]{8} ", l)])

    # 1: sharded kill + resume
    d1 = os.path.join(W, "sharded")
    fin, got = killed_run(BASE + ["-shards", "3", "--resume", d1], d1, 3 * 12)
    if fin:
        fails.append("1: the run finished before the kill (no mid-run state)")
    rc, out, err = run(BASE + ["-shards", "3", "--resume", d1])
    took = from_dir(err)
    if out != ref:
        fails.append("1: resumed sharded stdout differs from the reference")
    if not (0 < len(took) < nfun):
        fails.append("1: want some but not all functions from DIR, got %d of %d" % (len(took), nfun))
    print("1: killed with %d parts stored; resume took %d of %d functions from DIR" % (got, len(took), nfun))

    # 2: unsharded kill + resume, then a full resume
    d2 = os.path.join(W, "unsharded")
    fin, got = killed_run(BASE + ["-shards", "1", "--resume", d2], d2, 12)
    if fin:
        fails.append("2: the run finished before the kill")
    rc, out, err = run(BASE + ["-shards", "1", "--resume", d2])
    took = from_dir(err)
    if out != ref:
        fails.append("2: resumed unsharded stdout differs from the reference")
    if not (0 < len(took) < nfun):
        fails.append("2: want some but not all functions from DIR, got %d of %d" % (len(took), nfun))
    rc, out, err = run(BASE + ["-shards", "1", "--resume", d2])
    if out != ref or len(from_dir(err)) != nfun:
        fails.append("2: second resume: want all %d functions from DIR and the reference stdout, got %d" % (nfun, len(from_dir(err))))
    print("2: killed with %d parts; first resume took %d, second %d of %d" % (got, len(took), len(from_dir(err)), nfun))

    default_parts = set(parts(d2))  # complete for the default key (step 3 adds parts under another key)

    # 3: a changed key misses
    rc, out, err = run(BASE + ["-shards", "1", "--resume", d2], env=dict(os.environ, WWHD_VERIFY_HANG_LIMIT="77"))
    if from_dir(err) or out != ref:
        fails.append("3: changed key: %d functions taken from DIR (want 0), stdout %s" % (
            len(from_dir(err)), "same" if out == ref else "differs"))

    # 4: damaged and truncated parts are misses (DIR d2 is complete for the default key)
    ps = sorted(p for p in default_parts if ".p0of1." in p)
    ps = [p for p in ps if json.load(open(p)).get("key")]
    victims = []
    for p in ps:
        e = json.load(open(p))
        if e["shards"] == 1 and len(victims) < 2:
            victims.append((p, e["addr"]))
    if len(victims) == 2:
        (p1, a1), (p2, a2) = victims
        e = json.load(open(p1))
        e["stdout"] = e["stdout"].replace("ok", "OK", 1) if "ok" in e["stdout"] else e["stdout"] + " "
        json.dump(e, open(p1, "w"))
        txt = open(p2).read()
        open(p2, "w").write(txt[:len(txt) // 2])
        rc, out, err = run(BASE + ["-shards", "1", "--resume", d2])
        took = from_dir(err)
        if a1 in took or a2 in took or len(took) < nfun - 2 or out != ref:
            fails.append("4: damaged %s / truncated %s: taken %s %s, %d from DIR, stdout %s" % (
                a1, a2, a1 in took, a2 in took, len(took), "same" if out == ref else "differs"))
    else:
        fails.append("4: no parts to damage")

    # 5: archive mode without --resume never reads a resume DIR
    rc, out, err = run(BASE)
    if "resume:" in err or out != ref:
        fails.append("5: archive run without --resume read DIR or differs")

    # 6: nulltrap_compare --resume
    d6, L1, L2 = os.path.join(W, "ntc"), os.path.join(W, "log1"), os.path.join(W, "log2")
    cmd = [sys.executable, os.path.join(HERE, "nulltrap_compare.py"), UNIT, "-n", "300", "-seed", "1", "-buildj", "8",
           "--archive", "-noprogress", "--resume", d6]
    r1 = subprocess.run(cmd + ["--logdir", L1], cwd=ROOT, capture_output=True, text=True, errors="replace")
    r2 = subprocess.run(cmd + ["--logdir", L2], cwd=ROOT, capture_output=True, text=True, errors="replace")
    strip = lambda t: [l for l in t.splitlines() if not l.startswith("nulltrap_compare: resume:")
                       and "resume directory" not in l]
    if r1.returncode != r2.returncode or strip(r1.stdout) != strip(r2.stdout):
        fails.append("6: nulltrap_compare rows/verdict differ between the run and its resume")
    nt = re.search(r"^nulltrap_compare: resume: seed 1 -nulltrap side: (\d+) function", r2.stdout, re.M)
    if not nt or int(nt.group(1)) != nfun:
        fails.append("6: second run: want all %d -nulltrap functions from DIR: %s" % (
            nfun, [l for l in r2.stdout.splitlines() if "resume:" in l]))
    man = os.path.join(L2, "%s.resume.json" % UNIT)
    if not os.path.exists(man) or not json.load(open(man)).get("runs"):
        fails.append("6: no resume manifest %s" % man)
    plain_dirs = glob.glob(os.path.join(d6, UNIT, "s1.plain.*"))
    if not all(re.search(r"s1\.plain\.[0-9a-f]{16}$", x) for x in plain_dirs):
        fails.append("6: page-0 side part directory not keyed on the -dsiout content: %s" % plain_dirs)
    print("6: nulltrap_compare rc %d / %d; %s" % (r1.returncode, r2.returncode,
                                                  [l for l in r2.stdout.splitlines() if "resume:" in l]))

    for f in fails:
        print("FAIL", f)
    print("resume_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

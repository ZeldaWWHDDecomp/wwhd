#!/usr/bin/env python3
"""Self-test: a function's generated inputs, and so its reference-side records, do not depend on what ran before it in the
same harness process (2026-10-29, AGL#17 027C93A8).

The constant-dictionary pre-runs used to inherit the clobber switch of whatever the process ran last (the previous
function's last input, a setupsnapshot capture, or the process start), so a function's dictionary, and with it all its
generated inputs, differed between a whole-scope run (the campaign baseline that writes the -refcache records) and a fresh
mutant run of that function alone. The mutant's candidate then ran on other inputs than the recorded original: 49 false
kills in the AGL#17 campaign.

On the merged spec lib2_agl_4 (0277C578 and 0277EBF4 had such a dictionary before the fix):
1. for every function, the record file a whole-scope -refcache-write run writes equals the one a fresh -only run of that
   function alone writes (byte for byte; the header carries the dictionary);
2. a fresh -only run of each of the two functions reads the whole-scope records with -refcache-check (the original runs
   anyway and must equal the record) and prints the same row as the same run without the cache;
3. the same function after a predecessor whose last input index is even (it used to leave the switch on) and odd (off)
   writes the same record.
Writes temporary directories under build/verify/_mutate_refcache_order_test/ and removes them. Exit 0 when all pass.
"""
import filecmp
import glob
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
TMP = os.path.join(ROOT, "build", "verify", "_mutate_refcache_order_test")
UNIT, FUNCS = "lib2_agl_4", ("0277C578", "0277EBF4")
N = 50


def private_build_dir():
    import atexit
    import tempfile
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3])
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)


def main():
    private_build_dir()
    import verify
    import specfile
    shutil.rmtree(TMP, ignore_errors=True)
    os.makedirs(TMP)
    fails = []
    try:
        exe = verify.build(UNIT, jobs=4)
        spec = specfile.spec_path(UNIT, ROOT, verify.unit_dir(UNIT))
        hdr = [x for x in verify.header_args(UNIT)]

        def harness(*args, n=N):
            r = subprocess.run([exe, "-spec", spec, "-n", str(n), "-seed", "1", "-nulltrap"] + hdr + list(args), cwd=ROOT,
                               capture_output=True, text=True, env=dict(os.environ, WWHD_VERIFY_PROGRESS="0"))
            return r.returncode, r.stdout, r.stderr

        def write(d, *args, n=N):
            os.makedirs(d)
            return harness(*args, "-refcache-write", d, "-refcache-key", "K", n=n)

        full = os.path.join(TMP, "full")
        w = write(full)
        if w[0] not in (0, 1):
            fails.append("whole-scope run rc %d: %s" % (w[0], w[2][-300:]))
            raise RuntimeError
        funcs = sorted(os.path.basename(f)[:8] for f in glob.glob(os.path.join(full, "*.ref")))
        order = re.findall(r"^([0-9A-F]{8})  ", w[1], re.M)
        for f in FUNCS:
            if f not in funcs:
                fails.append("%s: no record in the whole-scope run" % f)
            elif order.index(f) == 0:
                fails.append("%s: first in the whole-scope run (the case needs a predecessor)" % f)
        # 1. every function: whole scope == alone
        diff = []
        for f in funcs:
            d = os.path.join(TMP, "one", f)
            harness_r = write(d, "-only", f)
            if harness_r[0] not in (0, 1) or not filecmp.cmp(os.path.join(full, f + ".ref"), os.path.join(d, f + ".ref"),
                                                             shallow=False):
                diff.append(f)
        if diff:
            fails.append("records differ between the whole-scope run and a run of the function alone: %s" % " ".join(diff))
        # 2. alone, reading the whole-scope records, with the live check
        for f in FUNCS:
            p = harness("-only", f)
            c = harness("-only", f, "-refcache", full, "-refcache-key", "K", "-refcache-check")
            if c[0] != p[0] or c[1] != p[1] or "CHECK FAILED" in c[2] or "does not match" in c[2]:
                fails.append("%s: -refcache-check of the whole-scope records rc %d: %s" % (f, c[0], c[2][-300:]))
            elif "%d input(s) took the recorded original, 0 ran it live" % N not in c[2]:
                fails.append("%s: the records were not used: %s" % (f, c[2][-200:]))
        # 3. predecessor ending on an even / odd input index
        for f in FUNCS:
            pred = order[order.index(f) - 1] if f in order and order.index(f) else None
            if not pred:
                continue
            got = []
            for last in (0, 1):
                rl = os.path.join(TMP, "rl-%s-%d.txt" % (f, last))
                with open(rl, "w") as fh:
                    fh.write("%s %d\n%s all\n" % (pred, last, f))
                # (a -runlist run writes no records: the live check compares this run's original with them instead)
                c = harness("-only", "%s,%s" % (pred, f), "-runlist", rl, "-refcache", full, "-refcache-key", "K",
                            "-refcache-check")
                got.append(c)
                if "CHECK FAILED" in c[2] or c[0] not in (0, 1):
                    fails.append("%s after %s ending at input %d: -refcache-check rc %d: %s" % (f, pred, last, c[0],
                                                                                           c[2][-300:]))
            rows = [[l for l in g[1].splitlines() if l.startswith(f + "  ")] for g in got]
            if rows[0] != rows[1] or not rows[0]:
                fails.append("%s: rows differ after a predecessor ending at an even / odd input" % f)
    except RuntimeError:
        pass
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_refcache_order_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

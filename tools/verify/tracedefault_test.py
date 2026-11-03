#!/usr/bin/env python3
"""Self-test of the `-trace` default (2026-11-02): tracing is off unless `-trace SEED` is given.

`trace_seed` used to default to 0, so a run with `-seed 0` and no `-trace` traced generated input 0 (seed 0*1000003+0) and
dictionary pre-run 0 (seed 0*7777+0), and the refcache writer (-refcache-write) and -snapcache treated the run as a traced
one and switched themselves off. Uses lib_js_base, JKRDvdFile::close 027EACBC:
1. `-seed 0` without -trace: no `trace` header and no access line (ld32/st32) on stdout;
2. `-seed 0 -trace 0` (asked for explicitly): input 0 and dictionary pre-run 0 are traced, as before;
3. the result row is the same in 1 and 2;
4. `-seed 0 -refcache-write DIR` without -trace writes the function's record DIR/027EACBC.ref (before the fix: none).
Writes a temporary unit directory and a temporary record directory (under $WWHD_VERIFY_SCRATCH when set) and removes
both. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_tracedefault_test")
CLOSE = "027EACBC"
BASE = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()


def run(extra):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_tracedefault_test/t", "-only", CLOSE, "-n", "20",
                        "-seed", "0", "--no-cache", "-noprogress", "-buildj", "4"] + list(extra), cwd=ROOT, capture_output=True,
                       text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith(CLOSE)]
    return r.stdout, (row[0] if row else r.stdout[-400:] + r.stderr[-400:])


def traced(out):
    """the trace headers and access lines of a run's stdout"""
    return [l for l in out.splitlines() if l.strip().startswith(("trace ", "ld32 ", "st32 "))]


def main():
    os.makedirs(TMP, exist_ok=True)
    scratch = os.environ.get("WWHD_VERIFY_SCRATCH")
    rdir = tempfile.mkdtemp(prefix="tracedefault-", dir=scratch if scratch and os.path.isdir(scratch) else None)
    fails = []
    try:
        with open(os.path.join(TMP, "t.txt"), "w") as f:
            f.write("\n".join(BASE) + "\n")
        out, plain = run([])
        t = traced(out)
        if t:
            fails.append("1: `-seed 0` without -trace printed %d trace line(s), first %r" % (len(t), t[0]))
        else:
            print("1 ok: no trace lines")
        out2, row2 = run(["-trace", "0"])
        heads = [l.strip() for l in out2.splitlines() if l.strip().startswith("trace ")]
        if not any(h.startswith("trace prerun") and "(dictionary pre-run 0)" in h for h in heads) or \
                not any(h.startswith("trace ") and not h.startswith("trace prerun") for h in heads) or \
                not any(l.strip().startswith(("ld32 ", "st32 ")) for l in out2.splitlines()):
            fails.append("2: `-trace 0` did not trace input 0 and pre-run 0: %r" % heads[:4])
        else:
            print("2 ok:", "; ".join(heads[:2]))
        if row2 != plain:
            fails.append("3: row differs with -trace 0: %r vs %r" % (row2, plain))
        else:
            print("3 ok:", plain)
        _, row4 = run(["-refcache-write", rdir, "-refcache-key", "K"])
        ref = os.path.join(rdir, CLOSE + ".ref")
        if not os.path.exists(ref):
            fails.append("4: no %s.ref after `-seed 0 -refcache-write` (files: %s)" % (CLOSE, os.listdir(rdir)))
        elif row4 != plain:
            fails.append("4: row differs with -refcache-write: %r vs %r" % (row4, plain))
        else:
            print("4 ok: %s.ref written (%d bytes)" % (CLOSE, os.path.getsize(ref)))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(rdir, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

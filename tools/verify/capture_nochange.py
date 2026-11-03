#!/usr/bin/env python3
"""capture_nochange.py BASE_TREE UNIT[:FUNC]... : the no-change proof of the capture route's harness part. Every unit (specs without `capture`)
runs on BASE_TREE (unpatched verifier) and on this tree with the same -n and seeds, without result cache; each per-function result line must be
byte-identical, and so must one -trace of the first input (net effects, calls, every load). Exit 0 when all are identical."""
import hashlib, os, re, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
def one(tree, unit, func, seed, trace):
    cmd = [sys.executable, "tools/verify/verify.py", unit, "-n", "1" if trace else "1000", "-seed", str(seed), "--no-cache"]
    if func: cmd += ["-only", func]
    if trace: cmd += ["-trace", str(seed * 1000003)]
    r = subprocess.run(cmd, cwd=tree, capture_output=True, text=True, env=dict(os.environ))
    out = r.stdout
    if trace:  # the addresses inside host paths are not guest behaviour; a trace has none, the header names the unit dir
        out = "\n".join(l for l in out.splitlines() if not l.startswith(("unit ", "verify:")))
    else:
        out = "\n".join(l for l in out.splitlines() if re.match(r"^[0-9A-F]{8}  ", l))
    return r.returncode, out
base = sys.argv[1]; bad = 0
for u in sys.argv[2:]:
    unit, _, func = u.partition(":")
    for seed in (1, 7):
        for trace in (False, True):
            a, b = one(base, unit, func, seed, trace), one(ROOT, unit, func, seed, trace)
            same = a == b
            print("%-44s s%d %-5s rc %d/%d lines %d sha %s %s" % (u, seed, "trace" if trace else "n1000", a[0], b[0], a[1].count("\n") + 1, hashlib.sha256(a[1].encode()).hexdigest()[:12], "same" if same else "DIFFERENT"), flush=True)
            bad += not same
print("capture_nochange:", "PASS" if not bad else "FAIL (%d)" % bad)
sys.exit(1 if bad else 0)

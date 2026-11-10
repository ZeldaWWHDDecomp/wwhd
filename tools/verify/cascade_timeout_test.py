#!/usr/bin/env python3
"""Self-test of cascade.py --timeout (2026-10-31, fasttools2 change 1).

cascade.py --timeout N passes `--timeout N` to every mutate.py pass (a fixed limit: mutate.py never measures and never
retries); without it no --timeout is passed (mutate.py derives its limit once from the pass's own baseline). A pass whose
receipts say `timeout` never kills: the mutant stays alive with a `timeout` history entry and is rerun on the next
companion.

cascade.py --sweep-stale-refcache (2026-11-10) is forwarded to every mutate.py pass; without it no pass gets it.

Runs cascade.py against a stub tree (--root): a fake tools/verify/mutate.py that records its command line and writes a
`timeout` receipt for every rerun mutant (no build, no harness), and two companion specs that name the mutants' source.
Temporary directories only; exit 0 when all pass.
"""
import json
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))

STUB = r'''
import json, os, sys
a = sys.argv[1:]
rerun, rc = a[a.index("--rerun") + 1], a[a.index("--receipts") + 1]
os.makedirs(rc, exist_ok=True)
with open(os.environ["STUB_ARGV"], "a") as fh:
    fh.write(json.dumps(a) + "\n")
for fn in os.listdir(rerun):
    if fn.endswith(".json"):
        j = json.load(open(os.path.join(rerun, fn)))
        j.update({"class": "timeout", "timeout": 7, "timeouts_tried": [7]})
        json.dump(j, open(os.path.join(rc, fn), "w"))
'''


def main():
    fails = []
    work = tempfile.mkdtemp(prefix="cascade_timeout_test-")
    try:
        root = os.path.join(work, "root")
        os.makedirs(os.path.join(root, "tools", "verify", "units"))
        open(os.path.join(root, "tools", "verify", "mutate.py"), "w").write(STUB)
        for c in ("comp_a", "comp_b"):
            open(os.path.join(root, "tools", "verify", "units", c + ".txt"), "w").write("src wwhd_src/x/A.cpp\n")
        main_dir = os.path.join(work, "main")
        os.makedirs(main_dir)
        for i in (3, 5):
            json.dump({"id": i, "src": "wwhd_src/x/A.cpp", "func": "02000000", "mutation": "m%d" % i, "class": "survived"},
                      open(os.path.join(main_dir, "mutant%d.json" % i), "w"))
            open(os.path.join(main_dir, "mutant%d.cpp" % i), "w").write("// mutant %d\n" % i)
        for tag, extra, want, sweep in (("fixed", ["--timeout", "7"], ["--timeout", "7"], False),
                                        ("derived", [], None, False),
                                        ("sweep", ["--sweep-stale-refcache"], None, True)):
            argv = os.path.join(work, tag + ".argv")
            out = os.path.join(work, tag)
            r = subprocess.run([sys.executable, os.path.join(HERE, "cascade.py"), "--receipts", main_dir, "--out", out,
                                "--input-seed", "1", "--root", root, "--load-max", "100000"] + extra + ["comp_a", "comp_b"],
                               capture_output=True, text=True, env=dict(os.environ, STUB_ARGV=argv))
            if r.returncode:
                fails.append("%s: cascade rc %d: %s" % (tag, r.returncode, (r.stdout + r.stderr)[-400:]))
                continue
            calls = [json.loads(x) for x in open(argv)]
            if len(calls) != 2:
                fails.append("%s: %d mutate.py passes, want 2 (one per companion)" % (tag, len(calls)))
            for c in calls:
                has = c[c.index("--timeout"):c.index("--timeout") + 2] if "--timeout" in c else None
                if has != want:
                    fails.append("%s: mutate.py got --timeout %s, want %s" % (tag, has, want))
                if ("--sweep-stale-refcache" in c) != sweep:
                    fails.append("%s: mutate.py got --sweep-stale-refcache %s, want %s" % (tag, not sweep, sweep))
            alive = json.load(open(os.path.join(out, "alive.json")))
            for i, v in sorted(alive.items()):
                kinds = [h[2] for h in v["hist"][1:]]
                if v["killed_by"] or kinds != ["timeout", "timeout"]:
                    fails.append("%s: mutant %s: a timeout killed it or it was not rerun on both companions: %s" % (tag, i, v))
    finally:
        shutil.rmtree(work, ignore_errors=True)
    for x in fails:
        print("FAIL", x)
    print("cascade_timeout_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

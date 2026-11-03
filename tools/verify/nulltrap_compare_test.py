#!/usr/bin/env python3
"""Self-test of nulltrap_compare.py.
1. Parser, comparisons and verdicts on fixed text (no build): LOST blocks, LOW clean, receipts, -dsiout merging,
   INCOMPLETE rows, and (2026-10-28) FAIL: a row with passes < total on either side gives verdict FAIL, exit 1, one
   `FAIL seed S ADDR SIDE rows ...: <first difference>` line per failing row, over STEER and INCOMPLETE (both listed).
2. A real run (builds; WWHD_VERIFY_SCRATCH as verify.py): lib_js_base -only 027EC058 (JKRFileLoaderList__dt) with a
   deliberately wrong candidate (`flags & 2` for `flags & 1`) gives FAIL naming 027EC058 on both sides, exit 1; the
   unmodified source gives OK, exit 0. Writes a temporary unit directory and a mutant source and removes them.
Exit 0 when all cases pass."""
import json, os, sys, tempfile
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import nulltrap_compare as nc

PLAIN = """addr      function                                  generated   recorded coverage  first difference
0279D92C  &MultiFilter::destroy                       200/200        0/0    19/21  ok
    uncovered blocks: 5 9
0279DB64  &MultiFilter::draw                          200/200        0/0    25/27  ok
    uncovered blocks: 3 23
"""
NT = PLAIN.replace("19/21  ok\n    uncovered blocks: 5 9", "18/21  ok\n    uncovered blocks: 5 9 10")
ERR = ("nulltrap: 0279D92C: the original took a page-0 DSI on 10 input(s); first: load 0000000B\n"
       "nulltrap: 0279D92C: the original took a page-0 DSI on 5 input(s); first: load 0000000B\n"
       "nulltrap: 0279DB64: the original took a page-0 DSI on 190 input(s); first: load 0000000B\n")
fails = []
p, q = nc.parse(PLAIN, ""), nc.parse(NT, ERR)
if q["0279D92C"]["dsi"] != 15 or q["0279DB64"]["dsi"] != 190:
    fails.append("dsi sums %s" % {k: v["dsi"] for k, v in q.items()})
rows, lost = nc.compare(p, q, 200)
txt = {f: t for f, t, _ in rows}
if "LOST blocks 10" not in txt["0279D92C"]:
    fails.append("lost block not reported: %s" % txt["0279D92C"])
if "LOW clean" not in txt["0279DB64"] or "clean 10/200" not in txt["0279DB64"]:
    fails.append("low clean not reported: %s" % txt["0279DB64"])
if sorted(lost) != ["0279D92C", "0279DB64"]:
    fails.append("lost %s" % lost)
rows, lost = nc.compare(p, nc.parse(PLAIN, ""), 200)
if lost:
    fails.append("identical runs reported lost %s" % lost)
with tempfile.TemporaryDirectory() as d:
    a, b = os.path.join(d, "a"), os.path.join(d, "b")
    os.makedirs(a); os.makedirs(b)
    for i, (x, y) in enumerate([("difference", "difference"), ("difference", "survived"), ("abort", "difference")]):
        json.dump({"class": x}, open(os.path.join(a, "mutant%d.json" % i), "w"))
        json.dump({"class": y}, open(os.path.join(b, "mutant%d.json" % i), "w"))
    lost, changed, only = nc.compare_receipts(a, b)
    if lost != [(1, "difference", "survived")] or changed != [(2, "abort", "difference")] or only:
        fails.append("receipts %s %s %s" % (lost, changed, only))
# skipped plain runs (2026-10-27): -dsiout parsing and the merged plain row
DSIOUT = ("0279D92C 3,5,250\n# 0279D92C pass 5,250\n# 0279D92C done\n# 0279DB64 done\n"
          "0279DC00 all\n# 0279DC00 prerun 2\n# 0279DC00 done\n")
dsi, done = nc.parse_dsiout(DSIOUT)
if dsi != {"0279D92C": ({3, 5, 250}, {5, 250}), "0279DC00": ("all", set())} or done != {"0279D92C", "0279DB64", "0279DC00"}:
    fails.append("parse_dsiout %s %s" % (dsi, done))
d2, _ = nc.parse_dsiout("0279D92C 3\n# 0279D92C done\n0279D92C 5,250\n# 0279D92C pass 5,250\n0279DC00 all\n0279DC00 7\n")
if d2 != {"0279D92C": ({3, 5, 250}, {5, 250}), "0279DC00": ("all", set())}:
    fails.append("parse_dsiout shards %s" % d2)
NT2 = ("0279D92C  &MultiFilter::destroy                       198/200        0/0    18/21  x\n    uncovered blocks: 5 9 10\n"
       "0279DB64  &MultiFilter::draw                          200/200        0/0    25/27  ok\n    uncovered blocks: 3 23\n"
       "0279DC00  &MultiFilter::other                         200/200        0/0    5/6  ok\n    uncovered blocks: 1\n")
PART = ("0279D92C  &MultiFilter::destroy                       1/1        0/0    12/21  x\n    uncovered blocks: 1 2 3 4 5 6 7 8 9\n"
        "0279DC00  &MultiFilter::other                         199/199        0/0    6/6  ok\n    uncovered blocks:\n")
pl, notes = nc.merge_plain(nc.parse(NT2, ""), nc.parse(PART, ""), dsi, 200)
# 0279D92C: 198 - 1 (nulltrap pass at 5; 250 is recorded) + 1 = 198 of 200 - 2 generated DSI inputs + 1 (one aborted on both)
if pl["0279D92C"]["gen"] != (198, 199) or pl["0279D92C"]["uncovered"] != {5, 9} or pl["0279D92C"]["cov"] != (19, 21):
    fails.append("merged partial %s" % pl["0279D92C"])
if pl["0279DB64"]["gen"] != (200, 200) or "all clean" not in notes["0279DB64"]:
    fails.append("merged clean %s %s" % (pl["0279DB64"], notes))
if pl["0279DC00"]["gen"] != (199, 199) or pl["0279DC00"]["uncovered"]:
    fails.append("merged whole %s" % pl["0279DC00"])
# a cut-short run: a total below n without inputs aborted on both sides is INCOMPLETE, never ok
CUT = PLAIN.replace("200/200        0/0    19/21  ok", "150/150        0/0    19/21  ok")
rows, lost = nc.compare(nc.parse(CUT, ""), nc.parse(PLAIN, ""), 200)
txt = {f: t for f, t, _ in rows}
if not txt["0279D92C"].startswith("INCOMPLETE") or "plain run cut short" not in txt["0279D92C"] or "0279D92C" not in lost:
    fails.append("cut run not INCOMPLETE: %s" % txt["0279D92C"])
AB = PLAIN.replace("200/200        0/0    19/21  ok", "150/150        0/0    19/21  ok (some inputs aborted on both sides)")
rows, lost = nc.compare(nc.parse(AB, ""), nc.parse(AB, ""), 200)
if any(t.startswith("INCOMPLETE") for _, t, _ in rows) or lost:
    fails.append("both-sides aborts read as INCOMPLETE: %s" % rows)
# failing rows (2026-10-28): a row with passes < total on either side is FAIL, named with side and first difference,
# even when coverage and clean counts give nothing to steer; rows aborted on both sides are not failures
FAILROW = PLAIN.replace("200/200        0/0    25/27  ok", "126/200        0/0    25/27  r3 0000002A vs 0000002B")
fl = nc.failures(nc.parse(PLAIN, ""), nc.parse(FAILROW, ""))
if fl != [("0279DB64", "-nulltrap", "126/200", "r3 0000002A vs 0000002B")]:
    fails.append("failing -nulltrap row: %s" % fl)
rows, lost = nc.compare(nc.parse(PLAIN, ""), nc.parse(FAILROW, ""), 200)
if lost:
    fails.append("failing row read as STEER: %s" % lost)
RECFAIL = PLAIN.replace("200/200        0/0    19/21  ok", "200/200     3/5(-1)    19/21  f1 0 vs 1 (some inputs aborted on both sides)")
fl = nc.failures(nc.parse(RECFAIL, ""), nc.parse(PLAIN, ""))
if fl != [("0279D92C", "plain", "200/200 recorded 3/5", "f1 0 vs 1")]:
    fails.append("failing recorded plain row: %s" % fl)
if nc.failures(nc.parse(AB, ""), nc.parse(AB, "")):
    fails.append("both-sides aborts read as FAIL")
# merged plain row: failures among the page-0 inputs' plain runs carry the plain first difference
PARTF = PART.replace("1/1        0/0    12/21  x", "0/1        0/0    12/21  plain diff")
pl, _ = nc.merge_plain(nc.parse(NT2, ""), nc.parse(PARTF, ""), dsi, 200)
if pl["0279D92C"]["gen"] != (197, 199) or pl["0279D92C"]["status"] != "plain diff":
    fails.append("merged failing row %s" % pl["0279D92C"])
# the verdict: FAIL (exit 1) over STEER and INCOMPLETE, each failing row named with seed, side and first difference
import contextlib, io
def fake_main(nt_out, nt_err, argv):
    old = nc.run_verify
    def fake(spec, args, seed, nulltrap, extra=()):
        if "-dsiout" in extra:
            with open(extra[extra.index("-dsiout") + 1], "w") as w:
                w.write("".join("# %s done\n" % f for f in nc.parse(nt_out, "")))
        return nt_out, nt_err
    nc.run_verify = fake
    buf = io.StringIO()
    try:
        with contextlib.redirect_stdout(buf):
            rc = nc.main(argv)
    finally:
        nc.run_verify = old
    return rc, buf.getvalue()
rc, out = fake_main(FAILROW, ERR, ["X", "-n", "200", "-seed", "3"])
if rc != 1 or "FAIL seed 3 0279DB64 plain rows 126/200: r3 0000002A vs 0000002B" not in out \
        or "FAIL seed 3 0279DB64 -nulltrap rows 126/200: r3 0000002A vs 0000002B" not in out \
        or "nulltrap_compare: X: FAIL: 0279DB64; STEER: 0279DB64\n" not in out:
    fails.append("FAIL verdict with STEER rc %d:\n%s" % (rc, out))
rc, out = fake_main(FAILROW, "", ["X", "-n", "200", "-seed", "3"])
if rc != 1 or not out.endswith("nulltrap_compare: X: FAIL: 0279DB64\n"):
    fails.append("FAIL verdict rc %d:\n%s" % (rc, out))
rc, out = fake_main(PLAIN, "", ["X", "-n", "200", "-seed", "3"])
if rc != 0 or not out.endswith("nulltrap_compare: X: OK\n") or "FAIL" in out:
    fails.append("OK verdict rc %d:\n%s" % (rc, out))
rc, out = fake_main(FAILROW.replace("19/21  ok", "19/21  ok").replace("200/200        0/0    19/21", "150/150        0/0    19/21"),
                    "", ["X", "-n", "200", "-seed", "3"])
if rc != 1 or "FAIL: 0279DB64; INCOMPLETE: 0279D92C" not in out:
    fails.append("FAIL over INCOMPLETE rc %d:\n%s" % (rc, out))
import subprocess
r = subprocess.run([sys.executable, os.path.join(os.path.dirname(os.path.abspath(__file__)), "nulltrap_compare.py"),
                    "/no/such/spec.txt", "-n", "10", "-seed", "1"], capture_output=True, text=True)
if r.returncode != 2 or "no function rows" not in r.stdout:
    fails.append("bad spec gave rc %d: %s" % (r.returncode, r.stdout[-300:]))
# 2. real runs: a deliberately wrong candidate is FAIL, the unmodified source OK
import shutil
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_ntcfail_test")
F = "027EC058"
SRC = "wwhd_src/JSystem/JKernel/JKRFileLoader.cpp"
MUT = "wwhd_src/JSystem/JKernel/_ntcfail_test_JKRFileLoader.cpp"
RIGHT, WRONG = "    if (flags & 1) operator_delete(list);\n", "    if (flags & 2) operator_delete(list);\n"
orig = open(os.path.join(ROOT, SRC)).read()
if orig.count(RIGHT) != 1:
    fails.append("real run: %s no longer has %r" % (SRC, RIGHT.strip()))
else:
    os.makedirs(TMP, exist_ok=True)
    try:
        with open(os.path.join(ROOT, MUT), "w") as w:
            w.write(orig.replace(RIGHT, WRONG))
        base = open(os.path.join(UNITS, "lib_js_base.txt")).read().splitlines()
        for name, src in (("right", SRC), ("wrong", MUT)):
            with open(os.path.join(TMP, name + ".txt"), "w") as w:
                w.write("\n".join(("src " + src) if l.strip() == "src " + SRC else l for l in base) + "\n")
        for name, want_rc, want in (("wrong", 1, "nulltrap_compare: _ntcfail_test/wrong: FAIL: %s\n" % F),
                                    ("right", 0, "nulltrap_compare: _ntcfail_test/right: OK\n")):
            r = subprocess.run([sys.executable, os.path.join(HERE, "nulltrap_compare.py"), "_ntcfail_test/" + name,
                                "-only", F, "-n", "400", "-seed", "1", "-shards", "2", "-buildj", "4"],
                               cwd=ROOT, capture_output=True, text=True)
            fl = [l for l in r.stdout.splitlines() if l.startswith("FAIL seed 1 %s" % F)]
            ok = r.returncode == want_rc and r.stdout.endswith(want)
            if name == "wrong":
                ok = ok and len(fl) == 2 and "plain" in fl[0] and "-nulltrap" in fl[1] and "rows 400/400" not in r.stdout
            if not ok:
                fails.append("real run %s: rc %d:\n%s%s" % (name, r.returncode, r.stdout[-1500:], r.stderr[-800:]))
            else:
                print("real run %s: %s" % (name, (fl[0] if fl else want.strip())))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        if os.path.exists(os.path.join(ROOT, MUT)):
            os.remove(os.path.join(ROOT, MUT))
for f in fails:
    print("FAIL", f)
print("nulltrap_compare_test:", "FAIL" if fails else "PASS")
sys.exit(1 if fails else 0)

#!/usr/bin/env python3
"""Self-test of the reference-side cache (harness -refcache-write/-refcache/-refcache-check, mutate.py default,
--no-refcache; 2026-10-28).

On the merged spec lib2_sead_15:
1. harness level, with and without -nulltrap: a run that writes the records prints exactly what a plain run prints;
   a run that reads them prints the same stdout and stderr (apart from its `refcache:` line), takes every generated
   input from the records, and -refcache-check (live original compared with the record and its decode) passes;
   a different -refcache-key, a truncated file and a missing directory make every function run its original live
   with the same output; a -runlist subset reads the records and prints what the plain run with that subset prints;
   a writing run with -runlist (not every input) writes no file.
2. mutate.py: a short campaign (12 mutants of 02759928) with the cache and with --no-refcache gives the same class
   and the same harness stdout for every mutant, and the cached campaign reports cache hits.
Writes temporary directories under build/verify/_mutate_refcache_test/ and removes them. Exit 0 when all cases pass.
"""
import glob
import json
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
TMP = os.path.join(ROOT, "build", "verify", "_mutate_refcache_test")
UNIT, FUNC = "lib2_sead_15", "02759928"


def private_build_dir():
    import atexit
    import tempfile
    d = tempfile.mkdtemp(prefix="wwhd-%s-" % os.path.basename(__file__)[:-3])
    os.environ["WWHD_VERIFY_SCRATCH"] = d
    atexit.register(shutil.rmtree, d, True)


def harness(exe, spec, *args):
    r = subprocess.run([exe, "-spec", spec, "-n", "150", "-seed", "1"] + list(args), cwd=ROOT, capture_output=True, text=True,
                       env=dict(os.environ, WWHD_VERIFY_PROGRESS="0"))
    return r.returncode, r.stdout, r.stderr


def strip_rc(err):
    return "\n".join(l for l in err.splitlines() if not l.startswith("refcache:"))


def harness_cases(fails):
    import verify
    import specfile
    exe = verify.build(UNIT, jobs=4)
    spec = specfile.spec_path(UNIT, ROOT, verify.unit_dir(UNIT))
    nf = None
    for extra in ([], ["-nulltrap"]):
        tag = " ".join(extra) or "plain"
        d = os.path.join(TMP, "c" + "".join(extra))
        os.makedirs(d)
        p = harness(exe, spec, *extra)
        if p[0] not in (0, 1) or not re.search(r"^[0-9A-F]{8}  ", p[1], re.M):
            fails.append("%s: plain run failed rc %d: %s" % (tag, p[0], p[2][-300:]))
            continue
        nf = len(re.findall(r"^[0-9A-F]{8}  ", p[1], re.M))
        w = harness(exe, spec, *extra, "-refcache-write", d, "-refcache-key", "K")
        if w[:2] != p[:2] or strip_rc(w[2]) != strip_rc(p[2]):
            fails.append("%s: the writing run's output differs from the plain run" % tag)
        files = glob.glob(os.path.join(d, "*.ref"))
        if len(files) != nf:
            fails.append("%s: %d record files for %d functions" % (tag, len(files), nf))
        r = harness(exe, spec, *extra, "-refcache", d, "-refcache-key", "K")
        if r[:2] != p[:2] or strip_rc(r[2]) != strip_rc(p[2]):
            fails.append("%s: the reading run's output differs from the plain run" % tag)
        m = re.search(r"refcache: (\d+) input\(s\) took the recorded original, (\d+) ran it live", r[2])
        if not m or int(m.group(1)) != 150 * nf or int(m.group(2)) != 0:
            fails.append("%s: cache hits %s (want %d, 0 live)" % (tag, m.groups() if m else None, 150 * nf))
        c = harness(exe, spec, *extra, "-refcache", d, "-refcache-key", "K", "-refcache-check")
        if c[0] != p[0] or "CHECK FAILED" in c[2] or c[1] != p[1]:
            fails.append("%s: -refcache-check rc %d: %s" % (tag, c[0], c[2][-300:]))
        k = harness(exe, spec, *extra, "-refcache", d, "-refcache-key", "OTHER")
        if k[:2] != p[:2] or k[2].count("does not match this run's configuration") != nf:
            fails.append("%s: a different key did not run every function live" % tag)
        with open(files[0], "r+b") as fh:  # truncate one record file: that function runs live
            fh.truncate(os.path.getsize(files[0]) - 9)
        t = harness(exe, spec, *extra, "-refcache", d, "-refcache-key", "K")
        if t[:2] != p[:2] or "does not match" not in t[2]:
            fails.append("%s: a truncated record file was not refused" % tag)
        g = harness(exe, spec, *extra, "-refcache", os.path.join(d, "missing"), "-refcache-key", "K")
        if g[:2] != p[:2]:
            fails.append("%s: a missing directory changed the output" % tag)
        first = re.search(r"^([0-9A-F]{8})  ", p[1], re.M).group(1)
        rl = os.path.join(TMP, "runlist.txt")
        with open(rl, "w") as fh:
            fh.write("%s 1,5,7,149\n" % first)
        sp = harness(exe, spec, *extra, "-runlist", rl)
        os.makedirs(os.path.join(d, "fresh"))
        harness(exe, spec, *extra, "-refcache-write", os.path.join(d, "fresh"), "-refcache-key", "K")
        sr = harness(exe, spec, *extra, "-runlist", rl, "-refcache", os.path.join(d, "fresh"), "-refcache-key", "K")
        if sr[:2] != sp[:2] or "4 input(s) took the recorded original" not in sr[2]:
            fails.append("%s: -runlist with the records differs (%s)" % (tag, sr[2][-200:]))
        os.makedirs(os.path.join(d, "sub"))
        harness(exe, spec, *extra, "-runlist", rl, "-refcache-write", os.path.join(d, "sub"), "-refcache-key", "K")
        if os.listdir(os.path.join(d, "sub")):
            fails.append("%s: a -runlist run wrote records" % tag)


def mutate(*args):
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), UNIT, "--func", FUNC, "--n", "100", "-j", "4",
                        "--no-progress", "--max", "12", "--no-tce"] + list(args), cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def mutate_cases(fails):
    A, B = os.path.join(TMP, "A"), os.path.join(TMP, "B")
    rc1, out1 = mutate("--receipts", A)
    rc2, out2 = mutate("--receipts", B, "--no-refcache")
    if rc1 or rc2:
        fails.append("campaigns rc %d/%d: %s" % (rc1, rc2, (out1 + out2)[-500:]))
        return
    m = re.search(r"reference-side cache: (\d+) generated input\(s\) took", out1)
    if not m or int(m.group(1)) == 0:
        fails.append("the cached campaign reports no cache hits: %s" % out1[-400:])
    if "reference-side cache" in out2:
        fails.append("--no-refcache still used the cache")
    ra = {json.load(open(f))["id"]: json.load(open(f)) for f in glob.glob(os.path.join(A, "mutant*.json"))}
    rb = {json.load(open(f))["id"]: json.load(open(f)) for f in glob.glob(os.path.join(B, "mutant*.json"))}
    if sorted(ra) != sorted(rb) or not ra:
        fails.append("mutant ids differ: %s / %s" % (sorted(ra), sorted(rb)))
    for i in set(ra) & set(rb):
        if ra[i]["class"] != rb[i]["class"] or ra[i].get("stdout") != rb[i].get("stdout"):
            fails.append("mutant %d: %s/%s" % (i, ra[i]["class"], rb[i]["class"]))
        if "-refcache" not in (ra[i].get("cmd") or ["-refcache"]) or "-refcache" in (rb[i].get("cmd") or []):
            fails.append("mutant %d: run commands do not show the cache use" % i)


def main():
    private_build_dir()
    shutil.rmtree(TMP, ignore_errors=True)
    os.makedirs(TMP)
    fails = []
    try:
        harness_cases(fails)
        mutate_cases(fails)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_refcache_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

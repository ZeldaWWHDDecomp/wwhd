#!/usr/bin/env python3
"""Self-test of the reference-side cache's digest records (harness -refcache-digest, mutate.py's default since
2026-10-31, fasttools2 change 4).

A digest record keeps per input only what the candidate's run and the input's outcome need exactly, plus a 128-bit hash
of the original's calls (under compare_full's argument masks), net memory effect and `final` bytes. A reading run takes
the original from the record when the candidate's part hashes the same, needs no comparison when a side aborted (no
mock-model rejection), and otherwise runs the original live and compares in full. The outcome must be the one of an
all-live run in every case.

A. Harness level, lib2_sead_15 + `setup`/`setupsnapshot` on 02759564 and the same with `arg! 02759928 r3 0` (page-0
   accesses under -nulltrap), seeds 1 and 7, -nulltrap, n 600: written digest records are smaller than full ones; a
   reading run, a reading run with -refcache-digest-miss (every comparison falls back to the live original) and one with
   -refcache-check all print the live run's stdout and its stderr apart from `refcache:` lines (so coverage and the
   semantic-effect statistics are not counted twice); the unmutated candidate matches every recorded digest.
B. Mock-model rejections and aborts (the modelreject_test fixtures, lib2_agl_5 0278635C): digest records written by the
   unmodified candidate and read by candidates that reject on their side only (b), pass another argument (c), reject on
   both sides (d, other fixture) and abort on both sides (e: `(some inputs aborted on both sides)`): every row equals the
   live row of the same executable, with and without -refcache-digest-miss, and -refcache-check passes.
D. A world spec (agl3_d3c_ownerdepth_alt, -n 60) whose candidate passes more stack argument words than the original
   records: the unmutated candidate matches every digest (the first min(original, candidate) words are hashed).
C. Campaign level, lib2_sead_15 --max 60 --n 300 seed 1 --nulltrap: digest records (default), --refcache-full and
   --no-refcache give the same class and harness stdout per mutant; WWHD_REFCACHE_CHECK=1 with digest records too.
Work files under the verify scratch (removed when everything passed); B writes the modelreject_test temporary unit
directory and variant sources (removed at the end). Exit 0 when all pass.
"""
import glob
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402
import specfile  # noqa: E402

KEY = ["-refcache-key", "d" * 32]


def run(exe, spec, args):
    r = subprocess.run([exe, "-spec", spec, "-noprogress"] + args, cwd=ROOT, capture_output=True, text=True, errors="replace")
    return r.returncode, r.stdout, r.stderr


def plain(err):
    return "\n".join(l for l in err.splitlines() if not l.startswith("refcache:"))


def dsize(d):
    return sum(os.path.getsize(os.path.join(d, f)) for f in os.listdir(d))


def main():
    fails = []
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="refcache_digest_test-", dir=base)
    print("refcache_digest_test: files in %s" % W)

    def same(tag, live, other):
        if live[0] != other[0] or live[1] != other[1] or plain(live[2]) != plain(other[2]):
            fails.append("%s: differs from the live run (rc %d/%d)\n%s\n---\n%s" % (tag, live[0], other[0], live[1][-500:],
                                                                                  other[1][-500:]))

    # A. harness level
    unit = "lib2_sead_15"
    exe = verify.build(unit, jobs=4)
    hdr = verify.header_args(unit)
    text = open(specfile.spec_path(unit, ROOT, verify.unit_dir(unit))).read() + "\nsetup 02759564 02759928 r3=0x5E000000\nsetupsnapshot\n"
    for name, extra in (("snap", ""), ("null", "\narg! 02759928 r3 0\n")):
        sp = os.path.join(W, name + ".txt")
        open(sp, "w").write(text + extra)
        for seed in ("1", "7"):
            a = hdr + ["-nulltrap", "-n", "600", "-seed", seed]
            tag = "%s seed %s" % (name, seed)
            live = run(exe, sp, a)
            fd, dd = os.path.join(W, "%s-s%s-full" % (name, seed)), os.path.join(W, "%s-s%s-dg" % (name, seed))
            os.mkdir(fd)
            os.mkdir(dd)
            same(tag + " full write", live, run(exe, sp, a + ["-refcache-write", fd] + KEY))
            w = run(exe, sp, a + ["-refcache-write", dd] + KEY + ["-refcache-digest"])
            same(tag + " digest write", live, w)
            m = re.search(r"wrote digest records of (\d+) input\(s\); the unmutated candidate matches (\d+)", w[2])
            if not m or m.group(1) != m.group(2) or dsize(dd) >= dsize(fd):
                fails.append("%s: digest write: %s, %d vs %d bytes" % (tag, m and m.groups(), dsize(dd), dsize(fd)))
            r = run(exe, sp, a + ["-refcache", dd] + KEY)
            same(tag + " digest read", live, r)
            if "ran the original live after a digest difference" not in r[2] or " 0 ran it live" not in r[2]:
                fails.append("%s: digest read stats: %s" % (tag, [l for l in r[2].splitlines() if l.startswith("refcache")]))
            same(tag + " digest miss", live, run(exe, sp, a + ["-refcache", dd] + KEY + ["-refcache-digest-miss"]))
            ck = run(exe, sp, a + ["-refcache", dd] + KEY + ["-refcache-check"])
            same(tag + " digest check", live, ck)
            print("  A %s: rows %d, digest %d KB vs full %d KB; %s" % (
                tag, live[1].count("\n") - 1, dsize(dd) >> 10, dsize(fd) >> 10,
                [l.split("refcache: ")[1] for l in r[2].splitlines() if l.startswith("refcache: digest")]))
            if "DSI" not in live[2] and "page-0" not in live[1] + live[2] and name == "null":
                print("    (note: no page-0 fault in %s)" % tag)

    # B. model rejections and aborts
    import modelreject_test as mr
    made = []
    try:
        groups = {"1 64": [("a", "base"), ("b", "bigwidth"), ("c", "shifted")], "16385": [("e", "base"), ("d", "shifted")]}
        for width, cases in groups.items():
            exes = {}
            for tag, variant in cases:
                u = mr.spec("dg_%s_%s" % (tag, variant), variant, width)
                exes[tag] = (verify.build(u, jobs=4), specfile.spec_path(u, ROOT, verify.unit_dir(u)), verify.header_args(u))
            d = os.path.join(W, "mr-%s" % width.replace(" ", "-"))
            os.mkdir(d)
            bt = cases[0][0]
            a = ["-only", mr.F, "-n", "200", "-seed", "1"]
            e0, s0, h0 = exes[bt]
            w = run(e0, s0, h0 + a + ["-refcache-write", d] + KEY + ["-refcache-digest"])
            if w[0] != 0 and bt == "a":
                fails.append("B %s: the base fixture does not pass: %s" % (width, w[1][-300:]))
            for tag, variant in cases:
                ex, sp, h = exes[tag]
                live = run(ex, sp, h + a)
                row = [l for l in live[1].splitlines() if l.startswith(mr.F)]
                for mode, extra in (("read", []), ("miss", ["-refcache-digest-miss"]), ("check", ["-refcache-check"])):
                    same("B %s %s %s" % (tag, variant, mode), live, run(ex, sp, h + a + ["-refcache", d] + KEY + extra))
                r = run(ex, sp, h + a + ["-refcache", d] + KEY)
                st = [l.split("refcache: digest records: ")[1] for l in r[2].splitlines() if l.startswith("refcache: digest")]
                print("  B %s %s: %s | %s" % (tag, variant, row[0][60:] if row else "?", st[0] if st else "no digest line"))
                if tag == "e" and (not row or "(some inputs aborted on both sides)" not in row[0]):
                    fails.append("B e: no both-sides abort row: %s" % row)
                if tag in ("b", "d") and (not st or st[0].startswith("0 input(s) matched") and " 0 ran the original live" in st[0]):
                    fails.append("B %s: the rejection did not run the original live: %s" % (tag, st))
    finally:
        made = glob.glob(os.path.join(ROOT, "wwhd_src", "agl", "_modelreject_test_*"))
        for f in made:
            os.remove(f)
        shutil.rmtree(mr.TMP, ignore_errors=True)

    # D. a world spec whose candidate passes more stack argument words than the original records (compare_full compares
    # the first min(original, candidate) words): the unmutated candidate must match every digest
    u = "agl3_d3c_ownerdepth_alt"
    ex, sp, h = verify.build(u, jobs=4), specfile.spec_path(u, ROOT, verify.unit_dir(u)), verify.header_args(u)
    d = os.path.join(W, "agl3")
    os.mkdir(d)
    a = h + ["-nulltrap", "-n", "60", "-seed", "1"]
    live = run(ex, sp, a)
    w = run(ex, sp, a + ["-refcache-write", d] + KEY + ["-refcache-digest"])
    same("D write", live, w)
    m = re.search(r"wrote digest records of (\d+) input\(s\); the unmutated candidate matches (\d+)", w[2])
    if not m or m.group(1) != m.group(2) or m.group(1) == "0":
        fails.append("D: the unmutated candidate does not match every digest: %s" % (m and m.groups(),))
    r = run(ex, sp, a + ["-refcache", d] + KEY)
    same("D read", live, r)
    print("  D %s: %s" % (u, m and "the unmutated candidate matches %s of %s digests" % (m.group(2), m.group(1))))

    # C. campaign level
    res = {}
    for tag, extra, env in (("digest", [], {}), ("full", ["--refcache-full"], {}), ("none", ["--no-refcache"], {}),
                            ("check", [], {"WWHD_REFCACHE_CHECK": "1"})):
        rd = os.path.join(W, "camp-" + tag)
        r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), unit, "--max", "60", "--n", "300", "--seed", "1",
                            "--nulltrap", "-j", "4", "--no-progress", "--receipts", rd] + extra, cwd=ROOT, capture_output=True,
                           text=True, env=dict(os.environ, **env))
        if r.returncode:
            fails.append("C %s: rc %d %s" % (tag, r.returncode, (r.stdout + r.stderr)[-400:]))
        res[tag] = {json.load(open(f))["id"]: json.load(open(f)) for f in glob.glob(os.path.join(rd, "mutant*.json"))}
        line = [l.strip() for l in r.stdout.splitlines() if "reference-side cache:" in l]
        print("  C %s: %d mutants; %s" % (tag, len(res[tag]), line[0][:220] if line else "no refcache line"))
    for tag in ("full", "none", "check"):
        diff = [i for i in res["digest"] if (res["digest"][i]["class"], res["digest"][i].get("stdout")) !=
                (res[tag].get(i, {}).get("class"), res[tag].get(i, {}).get("stdout"))]
        if diff or len(res[tag]) != len(res["digest"]):
            fails.append("C: digest vs %s: %d mutant(s) differ (first %s)" % (tag, len(diff), diff[:5]))
    cls = {}
    for j in res["digest"].values():
        cls[j["class"]] = cls.get(j["class"], 0) + 1
    print("  C classes %s" % cls)
    for x in fails:
        print("FAIL", x)
    if not fails:
        shutil.rmtree(W, ignore_errors=True)
    print("refcache_digest_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

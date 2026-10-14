#!/usr/bin/env python3
"""Test of the constructor-storage default (mkunit.py + harness.cpp, 2026-10-11): a callee listed in
ctor_sizes.tsv gets r3 = output-only storage of sizeof bytes (not compared before the call; the harness zeroes it
after a generated call) unless a unit or global r3 fact exists. Builds a copy of the d_snap unit without its
explicit `callee 0274F5F0 r3=out24` line and checks the generated callee row for 0274F5F0: pointee size 24,
output-only bit 3, no random-fill bit (the explicit line keeps its random fill). Also checks that sizes.py lists
the second constructor prologue form (0274FDBC, 0x74).
Negative test: d_a_ep daEp_Execute builds a dBgS_GndChk: the mocked cBgS_GndChk constructor (02008E0C, sizeof 0x40,
zeroed by the default), then the derived stores past sizeof (mPass bytes at +0x44..). Under `stackfill periodic`, a copy that
skips the zero store at +0x45 must fail on inputs the unchanged source passes (d_a_ep is not stackfill-clean
by itself: an untouched padding byte differs between the two frame layouts).
Writes units/_ctor_test*.txt and wwhd_src/d/actor/_ctor_neg_ep.cpp and removes them. Exit 0 on PASS."""
import os, re, shutil, subprocess, sys
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))


def row(unit, target):
    subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit, "-list"], cwd=ROOT, capture_output=True)
    for l in open(os.path.join(ROOT, "build", "verify", unit, "unit.c")):
        if l.strip().startswith("{0x%su" % target):
            return re.sub(r'"[^"]*"', '""', l.strip())
    return None


def main():
    fails = []
    spec = open(os.path.join(HERE, "units", "d_snap.txt")).read()
    if "callee 0274F5F0 r3=out24" not in spec:
        fails.append("d_snap no longer has the reference fact; pick another unit")
    tmp = os.path.join(HERE, "units", "_ctor_test.txt")
    try:
        open(tmp, "w").write(spec.replace("callee 0274F5F0 r3=out24\n", ""))
        want, got = row("d_snap", "0274F5F0"), row("_ctor_test", "0274F5F0")
        # fields: ..., {ptrsz}, {outp}, real, declared, retkind, retbits, retconst, nstack, outonly, {ext}, fill
        def fields(r):
            if not r:
                return None
            toks = re.findall(r"\{[^}]*\}|[^,{}\s][^,]*", r.strip().rstrip(",")[1:-1])
            toks = [x.strip() for x in toks]
            num = lambda x: int(x.rstrip("u"), 0)
            # addr, name, imask, fmask, defr, deff, {ptrsz}, {outp}, real, declared, rk, rb, rc, nstack, outonly, {ext}, fill
            return (toks[6].strip("{}").split(",")[3], num(toks[14]) >> 3 & 1, num(toks[16]) >> 3 & 1)
        w, g = fields(want), fields(got)
        if not w or w != ("24", 1, 1):
            fails.append("explicit row unexpected: %s" % want)
        if not g or g != ("24", 1, 0):
            fails.append("default constructor row: want size 24, output-only, no random fill; got %s" % got)
    finally:
        os.remove(tmp)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_ctor_test"), ignore_errors=True)
    # negative test: a skipped store just past the constructor's sizeof is still caught under stackfill
    ep = open(os.path.join(ROOT, "wwhd_src", "d", "actor", "d_a_ep.cpp")).read()
    call = "dBgS_GndChk_ct(gndChk.get(), EP_GNDCHK_VT, true);"
    # dBgS_GndChk_ct (d/d_bg_s.h) spelled out without the zero store to mPass[1] (+0x45)
    skip = ("{ u32 b = gabi::ea(gndChk.get()); gabi::call(0x02008E0C, gndChk.get()); "
            "for (int i = 0; i < 7; i++) if (i != 1) gabi::store<u8>(b + 0x44 + i, i == 0 ? 1 : 0); "
            "gabi::store<u32>(b + 0x50, 1); gabi::store<u32>(b + 0x4, b + 0x4C); gabi::store<u32>(b + 0x0, b + 0x40); "
            "gabi::store<u32>(b + 0x4C, EP_GNDCHK_VT.v4C); gabi::store<u32>(b + 0x40, EP_GNDCHK_VT.v40); "
            "gabi::store<u32>(b + 0x20, EP_GNDCHK_VT.v20); gabi::store<u32>(b + 0x10, EP_GNDCHK_VT.v10); }")
    espec = open(os.path.join(HERE, "units", "d_a_ep.txt")).read()
    neg_src = os.path.join(ROOT, "wwhd_src", "d", "actor", "_ctor_neg_ep.cpp")
    try:
        if ep.count(call) != 1:
            fails.append("d_a_ep no longer has the dBgS_GndChk construction; pick another unit")
        counts = {}
        for name, text in (("_ctor_test_pos", ep), ("_ctor_test_neg", ep.replace(call, skip))):
            open(neg_src, "w").write(text)
            open(os.path.join(HERE, "units", name + ".txt"), "w").write(
                espec.replace("src wwhd_src/d/actor/d_a_ep.cpp", "src wwhd_src/d/actor/_ctor_neg_ep.cpp") + "\nstackfill periodic\n")
            r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), name, "-n", "2000", "-only", "0212DBFC"],
                               cwd=ROOT, capture_output=True, text=True)
            line = next((l for l in r.stdout.splitlines() if l.startswith("0212DBFC")), "")
            m = re.search(r"\b(\d+)/2000\b", line)
            counts[name] = int(m.group(1)) if m else None
        # d_a_ep itself is not stackfill-clean (an untouched padding byte of the same object, +0x4B, differs between the
        # two frame layouts), so the check is relative: skipping the store must lose inputs the unchanged source passes
        if counts.get("_ctor_test_pos") is None or counts.get("_ctor_test_neg") is None or \
                not counts["_ctor_test_neg"] < counts["_ctor_test_pos"]:
            fails.append("skipped store past sizeof not caught under stackfill: passing inputs %s" % counts)
    finally:
        for p in (neg_src, os.path.join(HERE, "units", "_ctor_test_pos.txt"), os.path.join(HERE, "units", "_ctor_test_neg.txt")):
            if os.path.exists(p):
                os.remove(p)
        for d in ("_ctor_test_pos", "_ctor_test_neg"):
            shutil.rmtree(os.path.join(ROOT, "build", "verify", d), ignore_errors=True)
    sz = subprocess.run([sys.executable, os.path.join(HERE, "sizes.py")], cwd=ROOT, capture_output=True, text=True).stdout
    if not re.search(r"^0274FDBC\s+74\b", sz, re.M):
        fails.append("sizes.py misses the cmpwi-form constructor 0274FDBC (0x74)")
    for f in fails:
        print("FAIL", f)
    print("ctor_storage_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

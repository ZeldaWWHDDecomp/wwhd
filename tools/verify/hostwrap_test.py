#!/usr/bin/env python3
"""Self-test of verify.py -guardstrict (host-index wrap diagnostic) and of the direct-guest-access crash message
(2026-10-29).

-guardstrict builds the unit with GABI_GUEST_GUARD=2 (objects and executable in <unit dir>/guardstrict): every be<>/gptr<>
access whose host address is neither in the guest window nor NULL-based below 4 GB, i.e. reaches its guest address only
through ea()'s 32-bit wrap, prints a `guardstrict:` line (function, side, guest address, host pc and an atos line; once
per function and pc, and once more at a site's first access outside the guard's fast range) and a closing summary; rows are unchanged. A SIGSEGV/SIGBUS inside the guest window (or NULL-based
below 4 GB) names the guest address and the host pc: candidate code read guest memory without a be<> view (AGL#7: a raw
`u8` array member).

Uses lib2_sead_15's constructWorkerCounter 02759928 (WorkerRuntimeInstances.cpp; `object->mValue=0`) with variants:
a. unmodified: -guardstrict prints `guardstrict: no host-wrapped ...`; rows identical with and without -guardstrict;
b. u32 index wrap above the window: `(object + (i - 1))[1].mValue = 0` with u32 i = 0 (host +16 GB, ea() wraps back to
   the same EA): ok rows, a `guardstrict: 02759928 candidate side` line `above the guest window`; rows identical with and
   without -guardstrict, and the build without it prints no guardstrict line;
c. NULL-based index past 4 GB (a be<u64> read at host 20 GB + EA): `NULL-based past 4 GB; outside the guard's fast range`;
   rows identical with and without -guardstrict;
d. one site, fast first and slow later (a be<u64> read p[j] whose first access is a u32 index wrap above the window and
   every later one NULL-based past 4 GB, as in a curve table whose pointer is NULL on some inputs): the site is reported
   at its first access and again at its first slow access (`outside the guard's fast range`); rows identical with and
   without -guardstrict. Before d08357d30 only the first, fast occurrence was printed and the slow kind was never flagged;
e. a raw host write `((u8*)object)[0] = 0`: the run dies with `the fault address ... is guest XXXXXXXX: a direct host
   access to guest memory` and an atos line.
Writes a temporary unit directory (tools/verify/units/_hostwrap_test) and variant sources next to the original
(overwritten by the next run, never deleted). Builds go below WWHD_VERIFY_SCRATCH when set. Exit 0 when all pass.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_hostwrap_test")
F = "02759928"
SRC = "wwhd_src/sead/WorkerRuntimeInstances.cpp"
SITE = "    if(object!=NULL) object->mValue=0;\n"
VARIANTS = {
    "base": None,
    "above": "    if(object!=NULL) { u32 i = 0; (object + (i - 1))[1].mValue=0; }\n",
    "nullbased": ("    if(object!=NULL) { (void)u64(gabi::at<be<u64>>(0)[0xA0000000u + (gabi::ea(object) >> 3)]); "
                  "object->mValue=0; }\n"),
    "mixed": ("    if(object!=NULL) { static u32 calls; be<u64>* p = calls++ ? gabi::at<be<u64>>(0) : "
              "reinterpret_cast<be<u64>*>(object); u32 j = p ? 0x20000000u : 0xA0000000u + (gabi::ea(object) >> 3); "
              "(void)u64(p[j]); object->mValue=0; }\n"),
    "raw": "    if(object!=NULL) { reinterpret_cast<u8*>(object)[0]=0; object->mValue=0; }\n",
}
HDR = ["-strictnan", "-calleesp", "-footprint"]  # lib2_sead_15's `# verify:` header


def spec(name):
    os.makedirs(TMP, exist_ok=True)
    src = SRC
    if VARIANTS[name] is not None:
        text = open(os.path.join(ROOT, SRC)).read()
        if text.count(SITE) != 1:
            raise SystemExit("site not found once in %s" % SRC)
        src = "wwhd_src/sead/_hostwrap_test_%s_WorkerRuntimeInstances.cpp" % name
        open(os.path.join(ROOT, src), "w").write(text.replace(SITE, VARIANTS[name]))
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write(open(os.path.join(UNITS, "lib2_sead_15.txt")).read().replace("src " + SRC, "src " + src))
    return "_hostwrap_test/" + name


def run(unit, strict):
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), unit, "-only", F, "-n", "20", "-seed", "1",
                        "-buildj", "4", "-noprogress"] + HDR + (["-guardstrict"] if strict else []),
                       cwd=ROOT, capture_output=True, text=True)
    rows = [l for l in r.stdout.splitlines() if l.startswith(F)]
    gs = [l for l in r.stderr.splitlines() if l.startswith("guardstrict:")]
    return r.returncode, rows, gs, r.stderr


def main():
    fails = []
    for name in ("base", "above", "nullbased", "mixed"):
        u = spec(name)
        rc0, rows0, gs0, _ = run(u, False)
        rc1, rows1, gs1, err1 = run(u, True)
        print("%s: rc %d / %d  %s\n   %s" % (name, rc0, rc1, rows1[0] if rows1 else err1[-300:], "\n   ".join(gs1) or "-"))
        if (rc0, rows0) != (rc1, rows1) or not rows1:
            fails.append("%s: rows differ with -guardstrict: %s / %s" % (name, rows0, rows1))
        if gs0:
            fails.append("%s: guardstrict lines without -guardstrict: %s" % (name, gs0))
        sites = [l for l in gs1 if l.startswith("guardstrict: %s candidate side" % F)]
        if name == "base":
            if gs1 != ["guardstrict: no host-wrapped be<>/gptr<> access"]:
                fails.append("base: want no host-wrapped access, got %s" % gs1)
        elif name == "above":
            if rc1 or len(sites) != 1 or "index wrap above the guest window" not in sites[0] or "atos -o" not in sites[0] \
                    or "fast range" in sites[0]:
                fails.append("above: want ok rows and one fast-range `index wrap above the guest window` site, got rc %d %s" % (rc1, gs1))
        elif name == "mixed":
            if len(sites) != 2 or "index wrap above the guest window" not in sites[0] or "fast range" in sites[0] \
                    or "NULL-based past 4 GB" not in sites[1] or "outside the guard's fast range" not in sites[1] \
                    or sites[0].split("host pc ")[1].split()[0] != sites[1].split("host pc ")[1].split()[0]:
                fails.append("mixed: want the site reported fast first, then again at its first slow occurrence "
                             "(same host pc), got %s" % gs1)
        else:
            if len(sites) != 1 or "NULL-based past 4 GB" not in sites[0] or "outside the guard's fast range" not in sites[0]:
                fails.append("nullbased: want one `NULL-based past 4 GB` site outside the fast range, got %s" % gs1)
        if name != "base" and not any(l.startswith("guardstrict: %s: " % F) for l in gs1):
            fails.append("%s: no per-function summary line: %s" % (name, gs1))
    u = spec("raw")
    rc, rows, gs, err = run(u, False)
    msg = [l for l in err.splitlines() if "direct host access to guest memory" in l]
    print("raw: rc %d\n   %s" % (rc, msg[0] if msg else err[-300:]))
    if rc == 0 or not msg or "is guest " not in msg[0] or "atos -o" not in msg[0]:
        fails.append("raw: want the direct-host-access crash message with a guest address and an atos line, got rc %d %s"
                     % (rc, err[-400:]))
    for x in fails:
        print("FAIL", x)
    print("hostwrap_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

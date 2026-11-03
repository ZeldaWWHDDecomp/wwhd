#!/usr/bin/env python3
"""Self-test of the unit-file `argf64[!] FUNC|* fN v1[,v2,...]` directive (harness.cpp).

`arg FUNC fN ...` and the generated FPR inputs are single-precision values (double(float(v))), so a candidate
that narrows a double parameter to f32 passes. `argf64` places exact doubles: decimal, or 0x + 16 hex digits
of raw IEEE-754 bits (signalling NaNs included).

Uses the merged unit lib_js_jstudio_3: TFunctionValueAttribute_range::range_set (028489D8) stores f1/f2 with stfd.
1. The merged source passes with `argf64! 028489D8 f1 1.000000000931322574615478515625` (1 + 2^-30), with the
   raw form of the same value, and with a raw signalling NaN under -strictnan.
2. A candidate that narrows `begin` to f32 passes without the directive and with `arg! 028489D8 f1 ...` of the
   same value, and fails with argf64 (decimal, raw and the SNaN).
3. Malformed lines are rejected (exit 2).
Writes temporary unit files under tools/verify/units/_argf64_test/ and a temporary source next to
functionvalue.cpp, and removes them. Exit 0 when all cases pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_argf64_test")
FUNC = "028489D8"
SRC = "wwhd_src/JSystem/JStudio/functionvalue.cpp"
MUT = "wwhd_src/JSystem/JStudio/_argf64_test_functionvalue.cpp"
SPEC = open(os.path.join(UNITS, "lib_js_jstudio_3.txt")).read().splitlines()
EXACT = "1.000000000931322574615478515625"
RAW = "0x3FF0000004000000"
SNAN = "0x7FF0000000000001"


def run(name, lines, src=SRC, n="300", extra=()):
    body = [(("src " + src) if l.strip() == "src " + SRC else l) for l in SPEC]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_argf64_test/" + name, "-only", FUNC, "-n", n,
                        "--no-header-args"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith(FUNC)]
    ok = bool(row) and row[0].split()[-1] == "ok" and r.returncode == 0
    return r.returncode, ok, (row[0] if row else (r.stdout + r.stderr)[-400:])


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        if not any(l.strip() == "src " + SRC for l in SPEC):
            fails.append("lib_js_jstudio_3 no longer lists %s" % SRC)
        for name, lines, extra in (("base", [], ()),
                                   ("dec", ["argf64! %s f1 %s" % (FUNC, EXACT)], ()),
                                   ("raw", ["argf64! %s f1 %s" % (FUNC, RAW)], ()),
                                   ("star", ["argf64! * f1 %s,%s" % (EXACT, RAW)], ()),
                                   ("snan", ["argf64! %s f1 %s" % (FUNC, SNAN)], ("-strictnan",))):
            rc, ok, row = run(name, lines, extra=extra)
            if not ok:
                fails.append("merged source fails (%s): %s" % (name, row))
        text = open(os.path.join(ROOT, SRC)).read()
        old = "fBegin_=begin;"
        mt = text.replace(old, "fBegin_=static_cast<f64>(static_cast<f32>(begin));", 1)
        if mt == text:
            fails.append("mutation site %r not found in %s" % (old, SRC))
        else:
            open(os.path.join(ROOT, MUT), "w").write(mt)
            for name, lines in (("mut_none", []), ("mut_arg", ["arg! %s f1 %s" % (FUNC, EXACT)])):
                rc, ok, row = run(name, lines, src=MUT)
                if not ok:
                    fails.append("narrowing candidate fails without argf64 (%s; test lost its point): %s" % (name, row))
            for name, lines, extra in (("mut_dec", ["argf64! %s f1 %s" % (FUNC, EXACT)], ()),
                                       ("mut_raw", ["argf64! %s f1 %s" % (FUNC, RAW)], ()),
                                       ("mut_snan", ["argf64! %s f1 %s" % (FUNC, SNAN)], ("-strictnan",))):
                rc, ok, row = run(name, lines, src=MUT, extra=extra)
                if ok:
                    fails.append("narrowing candidate passes with %s: %s" % (lines[0], row))
        for bad in ("argf64 %s r3 1.5" % FUNC, "argf64 %s f1" % FUNC, "argf64 %s f1 0xZZ00000000000000" % FUNC,
                    "argf64 %s f1 0x3FF0" % FUNC, "argf64 %s f1 1.5 2.5" % FUNC, "argf64 %s f32 1.5" % FUNC,
                    "argf64 %s f1 1.5x" % FUNC):
            rc, _, _ = run("bad", [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_argf64_test"), ignore_errors=True)
        if os.path.exists(os.path.join(ROOT, MUT)):
            os.remove(os.path.join(ROOT, MUT))
    for f in fails:
        print("FAIL", f)
    print("argf64_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

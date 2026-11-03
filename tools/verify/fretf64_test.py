#!/usr/bin/env python3
"""Self-test of the unit-file `fretf64[!] ADDR v1[,v2,...]` directive (harness.cpp).

`fret ADDR LO HI` and the generated f1 results of mocked callees are single-precision values (double(float(v))), so
a candidate that narrows a double result to f32 passes. `fretf64` makes a mocked callee return exact doubles:
decimal, or 0x + 16 hex digits of raw IEEE-754 bits.

Uses the merged unit lib_js_jstudio_3: extrapolateParameter_repeat (0284A7E4) returns the double result of the
mocked FunctionValue_fmod (028F3AB8).
1. The merged source passes with `fretf64! 028F3AB8 1.000000000931322574615478515625` (1 + 2^-30), with the raw form
   of the same value and with a list of several exact values.
2. A candidate that narrows the fmod result to f32 passes without the directive and with `fret! 028F3AB8` of the
   same value, and fails with fretf64 (decimal and raw).
3. Malformed lines are rejected (exit 2).
Writes temporary unit files under tools/verify/units/_fretf64_test/ and a temporary source next to
functionvalue_inline.cpp, and removes them. Exit 0 when all cases pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_fretf64_test")
FUNC = "0284A7E4"
CALLEE = "028F3AB8"
SRC = "wwhd_src/JSystem/JStudio/functionvalue_inline.cpp"
MUT = "wwhd_src/JSystem/JStudio/_fretf64_test_functionvalue_inline.cpp"
SPEC = open(os.path.join(UNITS, "lib_js_jstudio_3.txt")).read().splitlines()
EXACT = "1.000000000931322574615478515625"
RAW = "0x3FF0000004000000"
LIST = "0.30000000000000004,1.000000000931322574615478515625,0x400921FB54442D18"


def run(name, lines, src=SRC, n="300"):
    body = [l.replace(SRC, src) if l.startswith("src ") else l for l in SPEC]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_fretf64_test/" + name, "-only", FUNC, "-n", n,
                        "--no-header-args"], cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith(FUNC)]
    ok = bool(row) and row[0].split()[-1] == "ok" and r.returncode == 0
    return r.returncode, ok, (row[0] if row else (r.stdout + r.stderr)[-400:])


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        if not any(l.startswith("src ") and SRC in l for l in SPEC):
            fails.append("lib_js_jstudio_3 no longer lists %s" % SRC)
        for name, lines in (("base", []), ("dec", ["fretf64! %s %s" % (CALLEE, EXACT)]),
                            ("raw", ["fretf64! %s %s" % (CALLEE, RAW)]), ("list", ["fretf64 %s %s" % (CALLEE, LIST)])):
            rc, ok, row = run(name, lines)
            if not ok:
                fails.append("merged source fails (%s): %s" % (name, row))
        text = open(os.path.join(ROOT, SRC)).read()
        old = "f64 result=FunctionValue_fmod(value,maximum);"
        mt = text.replace(old, "f64 result=static_cast<f64>(static_cast<f32>(FunctionValue_fmod(value,maximum)));", 1)
        if mt == text:
            fails.append("mutation site %r not found in %s" % (old, SRC))
        else:
            open(os.path.join(ROOT, MUT), "w").write(mt)
            for name, lines in (("mut_none", []), ("mut_fret", ["fret! %s %s %s" % (CALLEE, EXACT, EXACT)])):
                rc, ok, row = run(name, lines, src=MUT)
                if not ok:
                    fails.append("narrowing candidate fails without fretf64 (%s; test lost its point): %s" % (name, row))
            for name, lines in (("mut_dec", ["fretf64! %s %s" % (CALLEE, EXACT)]), ("mut_raw", ["fretf64! %s %s" % (CALLEE, RAW)])):
                rc, ok, row = run(name, lines, src=MUT)
                if ok:
                    fails.append("narrowing candidate passes with %s: %s" % (lines[0], row))
        for bad in ("fretf64 %s" % CALLEE, "fretf64 * 1.5", "fretf64 %s 0xZZ00000000000000" % CALLEE,
                    "fretf64 %s 0x3FF0" % CALLEE, "fretf64 %s 1.5 2.5" % CALLEE, "fretf64 %s 1.5x" % CALLEE,
                    "fretf64 0x%sQ 1.5" % CALLEE):
            rc, _, _ = run("bad", [bad], n="1")
            if rc != 2:
                fails.append("not rejected (rc %d): %s" % (rc, bad))
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_fretf64_test"), ignore_errors=True)
        if os.path.exists(os.path.join(ROOT, MUT)):
            os.remove(os.path.join(ROOT, MUT))
    for f in fails:
        print("FAIL", f)
    print("fretf64_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

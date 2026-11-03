#!/usr/bin/env python3
"""Self-test of the float-mark NaN tolerance of the net-effect comparison (harness.cpp same_net).

Without -strictnan, two NaNs with different payloads compare equal in a word that one side last wrote with a float store
(the Result's float marks, fwords/dwords); with -strictnan they differ, and a NaN against a non-NaN differs either way.
2026-10-26 (AGL#18 performance): the marks became ascending lists instead of std::sets (built per run from the page marks;
not built under -strictnan, where nothing reads them). This test pins the comparison they feed.

Uses the merged unit lib_js_jstudio_3: TFunctionValueAttribute_range::range_set (028489D8) stores f1 with stfd
(`argf64! 028489D8 f1 SNAN` makes every input a signalling NaN):
1. the merged source passes, with and without -strictnan;
2. a candidate that stores a different NaN payload (quiet NaN 0x7FF8000000000002) passes without -strictnan and FAILS with it;
3. a candidate that stores 0.0 for a NaN FAILS without -strictnan (the tolerance covers NaN against NaN only).
Writes temporary unit files under tools/verify/units/_nanmark_test/ and a temporary source next to functionvalue.cpp, and
removes them. Exit 0 when all cases pass.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
UNITS = os.path.join(HERE, "units")
TMP = os.path.join(UNITS, "_nanmark_test")
FUNC = "028489D8"
SRC = "wwhd_src/JSystem/JStudio/functionvalue.cpp"
MUT = "wwhd_src/JSystem/JStudio/_nanmark_test_functionvalue.cpp"
SPEC = open(os.path.join(UNITS, "lib_js_jstudio_3.txt")).read().splitlines()
SNAN = "0x7FF0000000000001"
SITE = "fBegin_=begin;"


def run(name, src, extra=()):
    body = [(("src " + src) if l.strip() == "src " + SRC else l) for l in SPEC]
    with open(os.path.join(TMP, name + ".txt"), "w") as f:
        f.write("\n".join(body + ["argf64! %s f1 %s" % (FUNC, SNAN)]) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_nanmark_test/" + name, "-only", FUNC, "-n", "100",
                        "--no-header-args"] + list(extra), cwd=ROOT, capture_output=True, text=True)
    row = [l for l in r.stdout.splitlines() if l.startswith(FUNC)]
    ok = bool(row) and row[0].split()[-1] == "ok" and r.returncode == 0
    return ok, (row[0] if row else (r.stdout + r.stderr)[-400:])


def main():
    os.makedirs(TMP, exist_ok=True)
    fails = []
    try:
        text = open(os.path.join(ROOT, SRC)).read()
        if not any(l.strip() == "src " + SRC for l in SPEC) or SITE not in text:
            fails.append("lib_js_jstudio_3 / %s no longer has the store %r" % (SRC, SITE))
            return finish(fails)
        for name, extra in (("base", ()), ("base_strict", ("-strictnan",))):
            ok, row = run(name, SRC, extra)
            if not ok:
                fails.append("merged source fails (%s): %s" % (name, row))
        open(os.path.join(ROOT, MUT), "w").write(
            text.replace(SITE, "fBegin_=begin!=begin?__builtin_nan(\"0x2\"):begin;", 1))
        ok, row = run("payload", MUT)
        if not ok:
            fails.append("other NaN payload in a float-stored word fails without -strictnan: %s" % row)
        ok, row = run("payload_strict", MUT, ("-strictnan",))
        if ok:
            fails.append("other NaN payload passes under -strictnan: %s" % row)
        open(os.path.join(ROOT, MUT), "w").write(text.replace(SITE, "fBegin_=begin!=begin?0.0:begin;", 1))
        ok, row = run("zero", MUT)
        if ok:
            fails.append("0.0 in place of a NaN passes without -strictnan: %s" % row)
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
        shutil.rmtree(os.path.join(ROOT, "build", "verify", "_nanmark_test"), ignore_errors=True)
        if os.path.exists(os.path.join(ROOT, MUT)):
            os.remove(os.path.join(ROOT, MUT))
    return finish(fails)


def finish(fails):
    for f in fails:
        print("FAIL", f)
    print("nanmark_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

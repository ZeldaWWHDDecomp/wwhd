#!/usr/bin/env python3
"""Self-test: spec value tokens must parse completely (2026-10-27). Before, `arg`/`field`/`ret`/`mockfield`/`fill`
operands were read with strtoll/strtod/strtoul without an end check, so a token such as `*0x5E100008` steered 0,
`r3+6C` used offset 6, `ret ADDR r3 V` drew 0..V, and a `field` line whose value did not start like a number (or that
lacked its FUNC) was skipped.

Builds lib2_sead_15 once, then runs its harness with the expanded spec plus one bad line per case: each exits 2 with
`FILE:LINE: invalid KW <what> "TOKEN" (not a complete number): LINE`. The unchanged spec, and the same spec with valid
forms of every directive (hex, decimal, negative, float, pick lists, rN+0xOFF), still pass. Builds go to a private
WWHD_VERIFY_SCRATCH directory (below an existing one when set), removed at exit.
"""
import atexit
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)

UNIT = "lib2_sead_15"
F = "02759928"  # constructWorkerCounter(CounterWord*)
BAD = [  # (line, token the message must name)
    ("arg %s r3 *0x5E100008" % F, "*0x5E100008"),
    ("arg! %s r3 0x54000000,*0x5E100008" % F, "*0x5E100008"),
    ("arg %s r3 s32 0 1" % F, "s32"),
    ("arg %s r3 ptr" % F, "ptr"),
    ("arg %s x3 0 1" % F, "x3"),
    ("arg 0275992G r3 0 1", "0275992G"),
    ("ret 028F5C88 r3 0x5C120000", "r3"),
    ("ret! 028F5C88 0,0.25,1", "0.25"),
    ("field %s r3+6C u32 0,1" % F, "6C"),
    ("field %s r4-4 u32 0,1" % F, "r4-4"),
    ("field * r3+0x3AC ptr nonnull", "nonnull"),
    ("field 586F5B2C u32 0x58500000", "missing FUNC"),  # FUNC left out: skipped before
    ("field * 0x54000000 u32 *0x5E100008", "*0x5E100008"),
    ("field * 0x54000000 u32 1,2x", "2x"),
    ("mockfield 028F5C88 r3+0 u32 0,abc", "abc"),
    ("fill %s 0x54000000 0x10 0xZZ" % F, "0xZZ"),
]
GOOD = ["arg %s r3 0x54000000,0x54000100" % F, "arg %s r3 0 -1" % F, "arg %s f1 -1.5 2.5e3" % F,
        "field * 0x54000000 u32 0,1,0xFFFFFFFF", "field %s r3+0x8 u32 7" % F, "field * 0x54000000 f32 -0.5,inf",
        "ret 028F5C88 0x54010000", "fill %s r3+0x10 16 0xFF" % F]


def main():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    scratch = tempfile.mkdtemp(prefix="wwhd-spec_tokens_test-", dir=base if base and os.path.isdir(base) else None)
    os.environ["WWHD_VERIFY_SCRATCH"] = scratch
    atexit.register(shutil.rmtree, scratch, True)
    import verify
    fails = []
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), UNIT, "-n", "1", "-buildj", "4", "-noprogress"],
                       cwd=ROOT, capture_output=True, text=True)
    out = verify.unit_dir(UNIT)
    import specfile
    exe, spec = os.path.join(out, "verify"), specfile.spec_path(UNIT, ROOT, out)
    if r.returncode or not os.path.exists(exe) or not os.path.exists(spec):
        print("FAIL build: rc %d %s" % (r.returncode, (r.stdout + r.stderr)[-600:]))
        return 1
    text = open(spec).read()
    nlines = text.count("\n") + (0 if text.endswith("\n") else 1)
    tmp = os.path.join(out, "spec_tokens_test.txt")

    def run(extra, n="20"):
        with open(tmp, "w") as fh:
            fh.write(text + ("" if text.endswith("\n") else "\n") + "\n".join(extra) + "\n")
        return subprocess.run([exe, "-spec", tmp, "-n", n, "-only", F, "-noprogress"], cwd=ROOT, capture_output=True,
                              text=True)

    r = run([])
    print("unchanged spec: rc %d" % r.returncode)
    if r.returncode:
        fails.append("unchanged spec: rc %d %s" % (r.returncode, r.stderr[-300:]))
    r = run(GOOD)
    print("valid forms (%d lines): rc %d" % (len(GOOD), r.returncode))
    if r.returncode == 2:
        fails.append("valid forms rejected: %s" % r.stderr[-400:])
    for k, (line, tok) in enumerate(BAD):
        r = run([line])
        msg = r.stderr.strip().splitlines()[-1] if r.stderr.strip() else ""
        ok = r.returncode == 2 and (('"%s"' % tok) in msg or (tok.startswith("missing") and tok in msg)) and ("%s:%d:" % (tmp, nlines + 1)) in msg and line in msg
        print("  %-48s rc %d  %s" % (line, r.returncode, msg.replace(tmp, "SPEC")[:150]))
        if not ok:
            fails.append("%r: rc %d, %r" % (line, r.returncode, msg))
    for x in fails:
        print("FAIL", x)
    print("spec_tokens_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

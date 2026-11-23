#!/usr/bin/env python3
"""Self-test of verify.py's T-02 static-initialiser warning (2026-11-18): a spec that runs
cSAngle::Radian 02006760 / cDegree::Radian 02006AA4 for real without `setup * 02007410` gets a warning naming
shared/c_angle_statics.txt; the same spec with the include (directly or through an included file) gets none; a spec
without those readers gets none. Temporary unit files under units/_t60static_test are removed."""
import contextlib
import io
import os
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import verify  # noqa: E402
import specfile  # noqa: E402

D = "_t60static_test"
DIR = os.path.join(specfile.UNITS, D)


def warn(name, text):
    with open(os.path.join(DIR, name + ".txt"), "w") as fh:
        fh.write(text)
    err = io.StringIO()
    with contextlib.redirect_stderr(err):
        verify.t60_static_warning(D + "/" + name)
    return err.getvalue()


def main():
    os.makedirs(DIR, exist_ok=True)
    bad = []
    try:
        w = warn("bare", "real 02006E50 02006760\n")
        if "02006760" not in w or "shared/c_angle_statics.txt" not in w:
            bad.append("no warning for a real cSAngle::Radian without the initialiser: %r" % w)
        w = warn("deg", "real 02006AA4\n")
        if "02006AA4" not in w:
            bad.append("no warning for a real cDegree::Radian: %r" % w)
        w = warn("inc", "real 02006760\ninclude shared/c_angle_statics.txt\n")
        if w:
            bad.append("warning despite the include: %r" % w)
        warn("mid", "include shared/c_angle_statics.txt\n")
        w = warn("nested", "include %s/mid.txt\nreal 02006760 02006AA4\n" % D)
        if w:
            bad.append("warning despite a nested include: %r" % w)
        w = warn("none", "real 02006E50 028F43F8\n")
        if w:
            bad.append("warning without a pi reader: %r" % w)
    finally:
        shutil.rmtree(DIR, ignore_errors=True)
    for b in bad:
        print("FAIL", b)
    print("t60_static_warning_test: %s" % ("FAIL" if bad else "PASS"))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())

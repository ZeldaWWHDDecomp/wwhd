#!/usr/bin/env python3
"""Self-test of mutate.py's object selection (replace_object): the mutant replaces exactly the object of the
mutated source, matched by whole basename. Regression: mutating vec.cpp also replaced mtxvec.cpp.o (suffix match,
duplicate symbols at link; MTX unit 2026-10-21). Exit 0 when all cases pass."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mutate  # noqa: E402

OUT = "build/verify/u"
OBJS = [OUT + "/unit.c.o", OUT + "/espresso_fp.c.o", OUT + "/harness.cpp.o",
        OUT + "/mtxvec.cpp.o", OUT + "/vec.cpp.o", OUT + "/xvec.cpp.o"]
fails = 0


def check(name, got, want):
    global fails
    if got != want:
        fails += 1
        print("FAIL %s: %s != %s" % (name, got, want))


# 1. suffix collision: only vec.cpp.o is replaced, mtxvec.cpp.o and xvec.cpp.o stay
check("vec", mutate.replace_object(OBJS, "wwhd_src/sdk/mtx/vec.cpp", "M.o"),
      [o if not o.endswith("/vec.cpp.o") else "M.o" for o in OBJS])
check("mtxvec", mutate.replace_object(OBJS, "wwhd_src/sdk/mtx/mtxvec.cpp", "M.o"),
      [o if not o.endswith("/mtxvec.cpp.o") else "M.o" for o in OBJS])
# 2. no object of that name, or two of them: refuse instead of linking the wrong set
for name, objs, src in (("missing", OBJS, "wwhd_src/sdk/mtx/mtx.cpp"),
                        ("duplicate", OBJS + ["other/vec.cpp.o"], "wwhd_src/sdk/mtx/vec.cpp")):
    try:
        mutate.replace_object(objs, src, "M.o")
        fails += 1
        print("FAIL %s: no error" % name)
    except SystemExit:
        pass
print("mutate_objsel_test: %s" % ("PASS" if not fails else "%d FAIL" % fails))
sys.exit(1 if fails else 0)

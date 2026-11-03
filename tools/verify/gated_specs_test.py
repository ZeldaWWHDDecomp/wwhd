#!/usr/bin/env python3
"""Self-test: the whole-tree gates leave out units/shared/ and top-level units/_NAME/, keep companion specs, and mutate.py refuses
units/_diag/. Runs on a temporary directory and on the real tree; builds nothing. Exit 0 when all pass."""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import readable_check  # noqa: E402
import specfile  # noqa: E402

FAILS = []


def check(ok, what):
    print(("PASS " if ok else "FAIL ") + what)
    if not ok:
        FAILS.append(what)


def main():
    with tempfile.TemporaryDirectory() as d:
        for p in ("a.txt", "a/comp.txt", "a/_keep.txt", "a/_sub/deep.txt", "shared/frag.txt", "_diag/x.txt", "_foo_test/y.txt", "notes.md"):
            os.makedirs(os.path.dirname(os.path.join(d, p)) or d, exist_ok=True)
            open(os.path.join(d, p), "w").close()
        got = specfile.gated_specs(d)
        check(got == ["a", "a/_keep", "a/_sub/deep", "a/comp"], "only shared/ and top-level _NAME/ go: %s" % got)
    real = specfile.gated_specs(os.path.join(HERE, "units"))
    check(len(real) > 100 and not [u for u in real if u.startswith(("_", "shared/"))], "the real tree: %d specs, none under _NAME/ or shared/" % len(real))
    check(readable_check.all_specs() == real, "readable_check uses the same list")
    src = open(os.path.join(HERE, "verify_all.py")).read()
    check("specfile.gated_specs(ud)" in src, "verify_all uses the same list")
    r = subprocess.run([sys.executable, os.path.join(HERE, "mutate.py"), "_diag/nosuch"], capture_output=True, text=True)
    check(r.returncode != 0 and "diagnosis only" in r.stderr, "mutate.py refuses _diag: %s" % r.stderr.strip()[:90])
    d = os.path.join(HERE, "units", "_diag")
    had = os.path.isdir(d)
    t = os.path.join(d, "_gated_specs_test_tmp.txt")
    os.makedirs(d, exist_ok=True)
    try:
        with open(t, "w") as f:
            f.write("src wwhd_src/gated_a.cpp wwhd_src/gated_b.cpp\n")
        import autoglue
        import objcmp
        autoglue._SPEC_SRCS = None
        check(autoglue._spec_siblings("wwhd_src/gated_a.cpp") == set(), "a _diag spec makes no source a sibling for autoglue")
        srcs, users = objcmp.unit_sources()
        check("gated_a.cpp" not in " ".join(srcs) and not [u for us in users.values() for u in us if u.startswith("_diag/")], "objcmp lists no _diag spec")
    finally:
        os.remove(t)
        if not had:
            os.rmdir(d)
    print("gated_specs_test: %s" % ("PASS" if not FAILS else "FAIL (%d)" % len(FAILS)))
    return 1 if FAILS else 0


if __name__ == "__main__":
    sys.exit(main())

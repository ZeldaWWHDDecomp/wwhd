#!/usr/bin/env python3
"""Self-test of mutate.py's host-symbol check (2026-10-26, AGL#13).

A mutant that drops `unsigned index = a * 3;` makes the later use bind to the host libc index() (strings.h): it
compiles on the host only. new_host_symbols(base_obj, mutant_obj, source) must name `index` for it; a libc call the
compiler makes on its own (a zeroing loop that becomes memset/bzero when the mutant lengthens it) is not spelled in the
source and must not count, nor may a member access spelled `p->index`. The companion host_function_values: a swap of
`short index = a;` with `if (index == 0) return 1;` binds index to libc index() and clang folds the compare away, so
the object has no new symbol - the "comparison of function 'index'" warning must still name it (2026-10-26 survey:
17 of 20 AGL cases); an unchanged source names nothing. Temporary files in build/verify/_mutate_hostsym_test/.
"""
import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import mutate  # noqa: E402

TMP = os.path.join(ROOT, "build", "verify", "_mutate_hostsym_test")
BASE = """#include <strings.h>
struct S { unsigned index; };
unsigned f(int a, S* p) {
    unsigned index = a * 3;
    return (unsigned)(unsigned long)index + p->index;
}
void g(char* q) { for (int i = 0; i < 3; i++) q[i] = 0; }
"""


def obj(name, text):
    path = os.path.join(TMP, name + ".cpp")
    open(path, "w").write(text)
    r = subprocess.run(["clang++", "-std=c++20", "-O1", "-w", "-c", path, "-o", path[:-4] + ".o"], capture_output=True, text=True)
    if r.returncode:
        raise SystemExit("compile %s: %s" % (name, r.stderr))
    return path[:-4] + ".o"


def main():
    fails = []
    shutil.rmtree(TMP, ignore_errors=True)
    os.makedirs(TMP)
    try:
        base = obj("base", BASE)
        dropped = BASE.replace("    unsigned index = a * 3;\n", "")
        got = mutate.new_host_symbols(base, obj("dropped", dropped), dropped)
        if got != {"index"}:
            fails.append("dropped declaration: %s, want {'index'}" % sorted(got))
        longer = BASE.replace("i < 3", "i < 3000")  # clang makes the loop a bzero call
        raw = mutate.new_host_symbols(base, obj("longer", longer))
        got = mutate.new_host_symbols(base, obj("longer", longer), longer)
        if not raw:
            fails.append("the lengthened loop made no new libc reference (the filter check proves nothing)")
        if got:
            fails.append("compiler-made libc call counted: %s (unfiltered %s)" % (sorted(got), sorted(raw)))
        if mutate.new_host_symbols(base, base, BASE):
            fails.append("the base against itself is not empty")
        sw_base = "#include <strings.h>\nint h(short a) {\n    short index = a;\n    if (index == 0) return 1;\n    return index;\n}\n"
        sw_mut = sw_base.replace("    short index = a;\n    if (index == 0) return 1;\n", "    if (index == 0) return 1;\n    short index = a;\n")
        bo, mo = obj("swbase", sw_base), obj("swmut", sw_mut)
        flags = ["-std=c++20", "-O1", "-w"]
        if mutate.new_host_symbols(bo, mo, sw_mut):
            fails.append("the folded swap shows a new symbol after all (the companion check is untested)")
        got = mutate.host_function_values(os.path.join(TMP, "swbase.cpp"), os.path.join(TMP, "swmut.cpp"), flags, sw_mut)
        if got != {"index"}:
            fails.append("swap of `short index = a;` and `if (index == 0)`: %s, want {'index'}" % sorted(got))
        if mutate.host_function_values(os.path.join(TMP, "swbase.cpp"), os.path.join(TMP, "swbase.cpp"), flags, sw_base):
            fails.append("the unchanged source names a function value")
    finally:
        shutil.rmtree(TMP, ignore_errors=True)
    for f in fails:
        print("FAIL", f)
    print("mutate_hostsym_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Self-test of autoglue's scoped struct lookup (find_struct), used by the static-table checks.

Two headers define a nested `ProcEntry` of different shapes (A::ProcEntry: two u32, B::ProcEntry: one
pointer to member), plus a file-scope ProcEntry (three bytes) in a third header. The lookup must
resolve a bare name by the enclosing class, then the source's own includes, a qualified name exactly,
and never by whichever header happens to come first. Runs on a temporary tree; exit 0 when all pass.
"""
import os
import shutil
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import autoglue  # noqa: E402

A_H = """#pragma once
struct A {
    struct ProcEntry { be<u32> x; be<u32> y; };
    void f();
};
"""
B_H = """#pragma once
struct B {
    struct ProcEntry { gabi::ptmf<int> proc; };
    void g();
};
"""
C_H = """#pragma once
struct ProcEntry { u8 a; u8 b; u8 c; };
"""


def main():
    tmp = tempfile.mkdtemp(prefix="gluescope-")
    old_root = autoglue.ROOT
    failures = []
    try:
        inc = os.path.join(tmp, "wwhd_src", "include", "d")
        os.makedirs(inc)
        # B and C sort before A in a directory walk: a by-name lookup would find them first
        for name, text in (("a_unit.h", A_H), ("0_b_unit.h", B_H), ("00_c.h", C_H)):
            open(os.path.join(inc, name), "w").write(text)
        autoglue.ROOT = tmp

        def kinds(name, src, scope=""):
            autoglue._layout_cache.clear()
            return [k for k, _, _, _ in autoglue.find_struct(name, src, "wwhd_src/d/x.cpp", scope)]

        cases = [
            ("bare name in a member function of A", "ProcEntry", '#include "d/a_unit.h"\n', "A", ["int", "int"]),
            ("bare name in a member function of B", "ProcEntry", '#include "d/0_b_unit.h"\n', "B", ["ptmf"]),
            ("qualified A::ProcEntry (B included first)", "A::ProcEntry", '#include "d/0_b_unit.h"\n#include "d/a_unit.h"\n', "", ["int", "int"]),
            ("qualified B::ProcEntry", "B::ProcEntry", '#include "d/a_unit.h"\n#include "d/0_b_unit.h"\n', "", ["ptmf"]),
            ("bare name at file scope: the file-scope struct", "ProcEntry", '#include "d/00_c.h"\n#include "d/a_unit.h"\n', "", ["int", "int", "int"]),
            ("struct of the source itself wins", "ProcEntry", 'struct ProcEntry { be<f32> v; };\n#include "d/a_unit.h"\n', "", ["float"]),
        ]
        for what, name, src, scope, want in cases:
            got = kinds(name, src, scope)
            ok = got == want
            print("%-50s %s (got %s)" % (what, "ok" if ok else "WRONG, want %s" % want, got))
            if not ok:
                failures.append(what)

        # an `ext` line for a function an included header already defines (same signature, same
        # address) emits no second definition; a different address is an error
        open(os.path.join(inc, "api.h"), "w").write(
            "#pragma once\ninline void JPAGetXYZRotateMtx(s16 x, s16 y, s16 z, Mtx34* out) { gabi::call(0x028245AC, x, y, z, out); }\n")
        unit = ('#include "d/api.h"\n/* 02000000 */\nvoid f() {}\n/* WWHD glue (tools/verify/autoglue.py)\n'
                'ext %s void JPAGetXYZRotateMtx(s16, s16, s16, Mtx34*)\n*/\n')
        out = autoglue.transform(unit % "028245AC", "wwhd_src/d/x.cpp")
        ok = "JPAGetXYZRotateMtx(s16 wwhd_p0" not in out and "other unit (ext)" not in out
        print("%-50s %s" % ("ext already defined by gc_api: skipped", "ok" if ok else "WRONG"))
        if not ok:
            failures.append("ext skip")
        try:
            autoglue.transform(unit % "028245B0", "wwhd_src/d/x.cpp")
            ok = False
        except autoglue.GlueError:
            ok = True
        print("%-50s %s" % ("ext with another address: error", "ok" if ok else "WRONG"))
        if not ok:
            failures.append("ext address check")
    finally:
        autoglue.ROOT = old_root
        autoglue._layout_cache.clear()
        shutil.rmtree(tmp, ignore_errors=True)
    print("autoglue_scope_test: %s" % ("FAIL: " + "; ".join(failures) if failures else "ok"))
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()

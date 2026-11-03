#!/usr/bin/env python3
"""Self-test of autoglue's namespace handling (2026-10-21, JS-JAudio#4 JASystem::Calc::bcopy/bzero):
1. a marked free function defined inside `namespace A { namespace B {` is registered by its qualified name A::B::f
   (VERIFY_NAMED keeps the bare display name, so existing objects stay identical) and gets no `this`;
2. an out-of-namespace definition `R A::B::f(...)` whose qualifier is a namespace (declared in the source or an
   included header) is a free function: no `this`, VERIFY with the qualified name;
3. a qualified definition `R C::f(...)` of a class method still passes `this`;
4. the glued source compiles (clang++ -fsyntax-only with the verify include paths), with a libc bcopy in scope.
Exit 0 when all cases pass."""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import autoglue  # noqa: E402

SRC = '''#include "bindings.h"
#include <strings.h>
namespace JASystem { namespace Calc {
/* 02800000 */
void bcopy(const void* src, void* dst, u32 n) { (void)src; (void)dst; (void)n; }
void bzero(void* dst, u32 n);
} }
/* 02800010 */
void JASystem::Calc::bzero(void* dst, u32 n) { (void)dst; (void)n; }
struct TCalcUser {
    be<u32> mValue;
    u32 get();
};
/* 02800020 */
u32 TCalcUser::get() { return mValue; }
/* WWHD glue
*/
'''


def main():
    fails = []
    out = autoglue.transform(SRC, "wwhd_src/JSystem/JAudio/_autoglue_ns_test.cpp")
    lines = out.splitlines()
    want = [
        ("VERIFY_NAMED(0x02800000, JASystem::Calc::bcopy, \"bcopy\");", "namespace-block function not registered by its qualified name"),
        ("(const void*)&JASystem::Calc::bcopy, 0x02800000", "namespace-block function's HostFn not qualified"),
        ("VERIFY(0x02800010, JASystem::Calc::bzero);", "qualified namespace definition not registered as a free function"),
        ("WWHD_GLUE_FUNC(0x02800000, void, src, dst, n);", "namespace-block function got `this`"),
        ("WWHD_GLUE_FUNC(0x02800010, void, dst, n);", "qualified namespace definition got `this`"),
        ("WWHD_GLUE_FUNC(0x02800020, u32, this);", "class method lost `this`"),
        ("VERIFY(0x02800020, &TCalcUser::get);", "class method not registered as a member pointer"),
    ]
    for text, why in want:
        if not any(text in l for l in lines):
            fails.append(why + ": " + text)
    with tempfile.NamedTemporaryFile("w", suffix=".cpp", delete=False) as f:
        f.write(out)
        tmp = f.name
    r = subprocess.run(["clang++", "-std=c++20", "-fsyntax-only", "-w", "-Iruntime/include", "-Itools/verify/include",
                        "-Itools/verify/src", "-Iwwhd_src/include", tmp], cwd=ROOT, capture_output=True, text=True)
    os.remove(tmp)
    if r.returncode:
        fails.append("glued source does not compile:\n" + r.stderr[-1500:])
    for f in fails:
        print("FAIL", f)
    print("autoglue_ns_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

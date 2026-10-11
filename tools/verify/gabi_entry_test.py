#!/usr/bin/env python3
"""Compile test of gabi.h's guest entries (VERIFY), with the flags verify.py uses (-fsyntax-only).

a function with reference parameters (`void Set(cXy&, cXy&)`, `f32 GetLen(const cXy&)`) can be
      registered: the guest passes the object's address (ArgFrom<T&>).
a const member function (`f32 GetLen(const cXy&) const`) can be registered without dropping const.
Exit 0 when the test source compiles.
"""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))

SRC = r"""
#include "wwhd.h"
#include <type_traits>
struct cXy { be<f32> x, y; cXy(const cXy&) = delete; };
struct cM2dGBox {
    cXy mP0, mP1;
    void Set(cXy& p0, cXy& p1);
    f32 GetLen(const cXy& p) const;
    int Count() const;
};
f32 Len(const cXy& p);
void cM2dGBox::Set(cXy& p0, cXy& p1) { WWHD_GLUE_FUNC(0x0201060C, void, this, &p0, &p1); }
f32 cM2dGBox::GetLen(const cXy& p) const { WWHD_GLUE_FUNC(0x02010630, f32, this, &p); return p.x; }
int cM2dGBox::Count() const { WWHD_GLUE_FUNC(0x02010700, int, this); return 0; }
f32 Len(const cXy& p) { WWHD_GLUE_FUNC(0x02010800, f32, &p); return p.y; }
VERIFY(0x0201060C, &cM2dGBox::Set);
VERIFY(0x02010630, &cM2dGBox::GetLen);
VERIFY(0x02010700, &cM2dGBox::Count);
VERIFY(0x02010800, Len);
static_assert(std::is_same_v<decltype(gabi::entry_of(&cM2dGBox::GetLen)), gabi::CMEntry<cM2dGBox, f32, const cXy&>>);
static_assert(gabi::ret_of(&cM2dGBox::Count) == gabi::RET_INT4);
"""


def main():
    cxx = os.environ.get("CXX", "clang++")
    with tempfile.TemporaryDirectory(prefix="gabientry-") as tmp:
        cpp = os.path.join(tmp, "t.cpp")
        open(cpp, "w").write(SRC)
        inc = ["-I" + os.path.join(ROOT, d) for d in ("runtime/include", "tools/verify/include", "tools/verify/src", "wwhd_src/include")]
        r = subprocess.run([cxx, "-std=c++20", "-fsyntax-only", "-w"] + inc + [cpp], capture_output=True, text=True)
    if r.returncode:
        print(r.stderr.strip()[:2000])
        print("gabi_entry_test: FAIL")
        return 1
    print("gabi_entry_test: ok (reference parameters, const member functions)")
    return 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""readable_lint self-tests.
framenoauto: a FrameLocal in a function without gabi::AutoFrame / WWHD_FUNC is counted (it would fall
back to a Local below r1), one with the automatic frame is not.
fieldnum: offset names with 3 to 5 hex digits (m2C4, m3522, m0FD7C) are counted; 6 digits, lower-case
hex and real names are not. field_XXX without 0x (2 to 5 hex digits) and unk_0xNN are
counted too, u8 padding arrays with those names (u8 field_48[8]) and real names (field_count) are not."""
import os
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import readable_lint  # noqa: E402

SRC = """/* 02000000 */
void bad() {
    gabi::NativeFrame<0x20> frame;
    gabi::FrameLocal<be<u32>> a(8);
    gabi::FrameLocal<be<u32>> b(0xC);  // comment mentioning AutoFrame does not count
}
/* 02000040 */
void good() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::FrameLocal<be<u32>> c(8);
}
/* 02000080 */
void raw() {
    WWHD_FUNC(0x02000080, void);
    FrameLocal<be<u32>> d(8);
}
"""


FIELD_SRC = """struct s {
    be<s16> m2C4;
    be<s16> m3522;
    cXyz m0FD7C;
    be<u32> m100A0B;
    be<u32> m0fd7c;
    be<u32> mFrameCounter;
};
"""

# offset names without 0x (shand's field_2BA..field_31C) and unk_0x60 forms
FIELD2_SRC = """struct t {
    /* 0x2BA */ be<s16> field_2BA;
    /* 0x31C */ be<f32> field_31C;
    /* 0x18 */ be<s16> field_18;
    /* 0x60 */ be<s16> unk_0x60;
    /* 0x48 */ u8 field_48[8];
    /* 0x06 */ u8 unk_0x6[0xC - 0x6];
    be<u32> field_count;
    be<u32> field_123456;
    be<u32> field_0x10;
};
void f(t* p) { p->field_2BA = 1; }
"""


def lint_text(text, column):
    with tempfile.NamedTemporaryFile("w", suffix=".cpp", delete=False) as f:
        f.write(text)
        path = f.name
    try:
        return readable_lint.lint(path)[column]
    finally:
        os.remove(path)


def main():
    n = lint_text(FIELD_SRC, "fieldnum")
    if n != 3:
        sys.exit("readable_lint_test: FAIL fieldnum = %d, expected 3 (m2C4, m3522, m0FD7C)" % n)
    print("readable_lint_test: PASS (fieldnum counts 3- to 5-digit offset names)")
    n = lint_text(FIELD2_SRC, "fieldnum")
    if n != 6:
        sys.exit("readable_lint_test: FAIL fieldnum = %d, expected 6 (field_2BA x2, field_31C, field_18, "
                 "unk_0x60, field_0x10)" % n)
    print("readable_lint_test: PASS (fieldnum counts field_XXX / unk_0xNN offset names, not u8 padding arrays)")
    with tempfile.NamedTemporaryFile("w", suffix=".cpp", delete=False) as f:
        f.write(SRC)
        path = f.name
    try:
        n = readable_lint.lint(path)["framenoauto"]
    finally:
        os.remove(path)
    if n != 2:
        sys.exit("readable_lint_test: FAIL framenoauto = %d, expected 2 (bad() only)" % n)
    print("readable_lint_test: PASS (framenoauto counts only FrameLocal without an automatic frame)")


if __name__ == "__main__":
    main()

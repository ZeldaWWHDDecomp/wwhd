#!/usr/bin/env python3
"""readable_lint framenoauto self-test: a FrameLocal in a function without gabi::AutoFrame / WWHD_FUNC
is counted (it would fall back to a Local below r1), one with the automatic frame is not."""
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


def main():
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

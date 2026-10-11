#!/usr/bin/env python3
"""Self-test of autoglue's string handling: the GuestStr registration literal must hold exactly the bytes
of the guest string (the source literal is looked up by its bytes, gabi::guest_str).

control bytes (a warning text with leading/trailing newlines) must not end the generated literal.
bytes >= 0x80 (Shift-JIS) must be written byte-exactly, glue text may hold Japanese text or escapes,
and a `str ADDR` line without text takes the bytes of the source's own spelling of the literal.
Each literal is compiled with the host compiler and its bytes compared. Exit 0 when all pass.
"""
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import autoglue  # noqa: E402

# (description, host string: one character per byte, as autoglue.img_cstr returns it)
LIT_CASES = [
    ("plain", "daObj_Quake_c"),
    ("quote and backslash", 'a"b\\c'),
    ("leading/trailing newline (d_kankyo_4 1004F1E0)", "\nPOINTLIGHT COUNT OVER!!!\n"),
    ("tab, CR, DEL, BEL", "a\tb\rc\x7fd\x07"),
    ("control byte before a digit", "\x01" + "234"),
    ("Shift-JIS bytes (d_a_obj_quake 1002EC08)", "\x90U\x93\xae\x8c\xf8\x89\xca"),
    ("UTF-8 bytes of a Japanese literal", "\u632f\u52d5\u52b9\u679c".encode("utf-8").decode("latin1")),
    ("octal escape before a digit", "\xff" + "7"),
]

SJIS = "\x90U\x93\xae\x8c\xf8\x89\xca"           # image bytes of "振動効果", one char per byte
UTF8 = "振動効果".encode("utf-8").decode("latin1")  # what clang makes of the literal "振動効果"

# (literal body as written in source/glue, expected bytes)
BYTES_CASES = [
    (r"\x90U\x93\xae\x8c\xf8\x89\xca", SJIS),
    (r"\220U\223\256\214\370\211\312", SJIS),
    ("振動効果", UTF8),
    (r"\u632f\u52d5\u52b9\u679c", UTF8),
    (r"\u0082f\u0081i", "\xc2\x82f\xc2\x81i"),
    (r"\nPOINTLIGHT\t\\\"\?", "\nPOINTLIGHT\t\\\"?"),
    (r"\1234", "\x534"),
]

# (host bytes, image bytes, same text?)
SAME_CASES = [
    (SJIS, SJIS, True),                                 # exact
    (UTF8, SJIS, True),                                 # Japanese literal in UTF-8 vs the Shift-JIS image
    ("\xc2\x82f\xc2\x81i", "\x82f\x81i", True),           # legacy \u00XX spelling of each image byte
    ("Vibration Effect", SJIS, False),                  # a translation stays `renamed`
    (UTF8, "\x90U\x93\xae\x8c\xf8\x89\xcb", False),      # one byte off
]

# (source text before the glue block, image bytes, expected host bytes)
SPELL_CASES = [
    ('f("\\x90U\\x93\\xae\\x8c\\xf8\\x89\\xca");', SJIS, SJIS),
    ('f("振動効果");', SJIS, UTF8),
    ('f("\\u0082f\\u0081i");', "\x82f\x81i", "\xc2\x82f\xc2\x81i"),
    ('f("other"); // "振動効果" in a comment', SJIS, SJIS),
    ('f("振動効果"); g("\\x90U\\x93\\xae\\x8c\\xf8\\x89\\xca");', SJIS, SJIS),  # exact spelling wins
]


def compile_check(cases):
    """compile `const char sN[] = <literal>;` for each case and print each one's bytes; [(ok, detail)]"""
    cxx = os.environ.get("CXX", "clang++")
    tmp = tempfile.mkdtemp(prefix="gluestr-")
    try:
        src = ["#include <cstdio>"]
        for i, (_, s) in enumerate(cases):
            src.append("static const char s%d[] = %s;" % (i, autoglue.cstr_lit(s)))
        src.append("int main() {")
        for i in range(len(cases)):
            src.append('    for (unsigned i = 0; i + 1 < sizeof s%d; i++) printf("%%02x", (unsigned char)s%d[i]); printf("\\n");' % (i, i))
        src.append("}")
        cpp = os.path.join(tmp, "t.cpp")
        exe = os.path.join(tmp, "t")
        open(cpp, "w", encoding="utf-8").write("\n".join(src) + "\n")
        r = subprocess.run([cxx, "-std=c++20", "-o", exe, cpp], capture_output=True, text=True)
        if r.returncode:
            return [(False, "compile failed: " + r.stderr.strip()[:400])] * len(cases)
        got = subprocess.run([exe], capture_output=True, text=True).stdout.split("\n")
        out = []
        for i, (_, s) in enumerate(cases):
            want = s.encode("latin1").hex()
            out.append((got[i] == want, "want %s got %s" % (want, got[i])))
        return out
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def main():
    fails, n = 0, 0
    for (desc, s), (ok, detail) in zip(LIT_CASES, compile_check(LIT_CASES)):
        n += 1
        if not ok:
            fails += 1
            print("FAIL literal %s: %s (%s)" % (desc, autoglue.cstr_lit(s), detail))
        if "\n" in autoglue.cstr_lit(s):
            n += 1
            fails += 1
            print("FAIL literal %s: raw newline in %r" % (desc, autoglue.cstr_lit(s)))
    for body, want in BYTES_CASES:
        n += 1
        got = autoglue.c_str_bytes(body)
        if got != want:
            fails += 1
            print("FAIL c_str_bytes %r: want %r got %r" % (body, want, got))
    for host, content, want in SAME_CASES:
        n += 1
        if autoglue.same_text(host, content) != want:
            fails += 1
            print("FAIL same_text %r %r: want %s" % (host, content, want))
    for src, content, want in SPELL_CASES:
        n += 1
        got = autoglue.source_spelling(src, len(src), content)
        if got != want:
            fails += 1
            print("FAIL source_spelling %r: want %r got %r" % (src, want, got))
    print("autoglue_str_test: %s %d/%d" % ("ok" if not fails else "FAIL", n - fails, n))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

#!/usr/bin/env python3
"""Leftover lint for readability rewrites: counts, never a failure (exit 0).

usage: readable_lint.py [--all] [--tsv] FILE...

For each source (and its unit header wwhd_src/include/<same path>.h when it exists) counts what the
style guide (docs/readability-style.md) wants gone or minimised. Comments and string literals are not
counted. Columns:

  goto      `goto` statements
  rawcall   guest calls by address in the source (gabi::call/call_ptr/tail_ptr(0xADDR...)); named
            bindings/gc_api wrappers instead
  rawmem    raw memory accesses: gabi::load/store/at<T>(...), ea(x) + 0xOFF, and the local helper
            families raw units define (load<u32>(a, off), put<..>, read<..>, ld8/st16/ldf...); struct
            fields instead
  fnwrap    fn::Name(...) calls (readable_names.py's mechanical step; the final unit calls through
            gc_api / bindings, whose signatures are checked)
  addr      guest data addresses written as numbers (0x10xxxxxx / 0x104xxxxx globals, STR(0x...)
            literals); named statics / gc_api objects instead
  fieldnum  unnamed fields: field_0x123, field_123 (2-5 hex digits), unk_123/unk123, unk_0x60,
            mXXX..mXXXXX hex names (m3522, m0FD7C), _0x.. members; field_/unk_0x u8 padding arrays
            (u8 field_48[8]) are not counted. Short offset names count too: mXX with two upper-case hex
            digits (m1C, m00, mBC), field_X / unk_X / unkX with one; not counted there are locals of
            that shape (f32 m00 = mtx->m[0][0], matrix elements) unless reached as a member (->m04,
            .m04) or declared in a struct body, enum values, and the hex-shaped real names in
            SHORT_REAL (mAA: GX2 surface anti-aliasing mode). One-digit mX (mA, m0) is never counted
  fnnum     functions named after addresses (fn_/sub_/func_/FUN_ + hex, or a name ending in an
            8-digit address)
  scaffold  harness scaffolding written by hand: WWHD_FUNC(...) and VERIFY(...) lines (a glue source
            has none: autoglue inserts them)
  local_l   local copies of shared declarations (identifiers ending in _l)
  framenoauto  gabi::FrameLocal slots in a function (from its `/* ADDR */` marker to the next one)
            without gabi::AutoFrame / WWHD_FUNC: without the automatic native frame FrameLocal falls
            back to a Local below r1 instead of the original's sp offset. Add the AutoFrame (or use
            NativeFrame + gabi::at); this column should be 0
  lines     non-empty code lines

--all lints every unit source (all `src` files of tools/verify/units); --tsv prints tab-separated.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))

PATTERNS = [
    ("goto", re.compile(r"\bgoto\b")),
    ("rawcall", re.compile(r"(?<![\w.>])(?:gabi::)?(?:call|call_ptr|tail_ptr|call_site)\s*(?:<[^;(]*?>)?\s*\(\s*0x[0-9A-Fa-f]{6,8}")),
    ("rawmem", re.compile(r"\bgabi::(?:load|store|at)\s*<|(?<!::)\b(?:load|store|put|read|write|get|set)\s*<\s*(?:u8|s8|u16|s16|u32|s32|f32|f64|float|int)\s*>\s*\(|\bea\s*\([^()]*(?:\([^()]*\))?[^()]*\)\s*\+\s*(?:0x[0-9A-Fa-f]+|\d+)"
                          r"|\b(?:ld|st)(?:8|16|32|s8|s16|f|d|u8|u16|u32)?\s*\(")),
    ("fnwrap", re.compile(r"\bfn::\w+")),
    ("addr", re.compile(r"\b0x10[0-9A-Fa-f]{6}\b|\bSTR\s*\(\s*0x")),
    ("fieldnum", re.compile(r"\bfield_0x[0-9A-Fa-f]+\b|\bunk_?[0-9A-Fa-f]{2,}\b|\bm[0-9A-F]{3,5}\b|(?<![0-9A-Za-z])_0x[0-9A-Fa-f]+\b"
                            # field_2BC (no 0x) and unk_0x60 offset names; not u8 padding arrays (u8 field_48[8])
                            r"|(?<!\bu8 )\bfield_[0-9A-Fa-f]{2,5}\b|(?<!\bu8 )\bunk_0x[0-9A-Fa-f]+\b")),
    ("fnnum", re.compile(r"\b(?:fn|sub|func|FUN)_[0-9A-Fa-f]{6,8}\w*\b|\b[A-Za-z]\w*_[0-9A-F]{8}\s*\(")),
    ("scaffold", re.compile(r"\bWWHD_FUNC\s*\(|^\s*VERIFY\s*\(", re.M)),
    ("local_l", re.compile(r"\b[A-Za-z]\w*_l\b")),
]
COLS = [n for n, _ in PATTERNS] + ["framenoauto", "lines"]
# short offset names (see the fieldnum doc): context decides, see short_offset_names()
SHORT = re.compile(r"\bm[0-9A-F]{2}\b|(?<!\bu8 )\bfield_[0-9A-Fa-f]\b|(?<!\bu8 )\bunk_?[0-9A-F]\b")
SHORT_REAL = {"mAA"}  # GX2SurfaceDesc aa (JPATextureDescriptor::mAA, daMant texture descriptor)
SCALAR = (r"(?:const\s+)?(?:unsigned\s+|signed\s+)?(?:f32|f64|float|double|s8|s16|s32|s64|u8|u16|u32|u64|int|long|"
          r"short|char|bool|BOOL|auto|cXyz|csXyz|Vec)\s*[*&]?\s*")
LOCALDECL = re.compile(SCALAR + r"(m[0-9A-F]{2}|field_[0-9A-Fa-f]|unk_?[0-9A-F])\s*(?=[=;,)])")
LOCALMORE = re.compile(r",\s*(m[0-9A-F]{2}|field_[0-9A-Fa-f]|unk_?[0-9A-F])\s*(?=[=;,])")
STRUCTHEAD = re.compile(r"\b(struct|class|union)\b[^(){};=]*$")
ENUMHEAD = re.compile(r"\benum\b[^(){};=]*$")


def brace_kinds(code):
    """per position: 's' inside a struct/class/union body, 'e' inside an enum body, 'c' anywhere else"""
    kinds = bytearray(b"c" * (len(code) + 1))
    stack, last, cur = [], 0, ord("c")
    for i, ch in enumerate(code):
        if ch == "{":
            head = code[last:i]
            stack.append(cur)
            cur = ord("s") if STRUCTHEAD.search(head) else ord("e") if ENUMHEAD.search(head) else ord("c")
            last = i + 1
        elif ch == "}":
            cur = stack.pop() if stack else ord("c")
            last = i + 1
        elif ch == ";":
            last = i + 1
        kinds[i] = cur
    return kinds


def short_offset_names(code):
    """count short offset names (mXX, field_X, unk_X): members and their uses; not locals, enum values, SHORT_REAL"""
    hits = [m for m in SHORT.finditer(code) if m.group(0) not in SHORT_REAL]
    if not hits:
        return 0
    kinds = brace_kinds(code)
    local = set()
    for m in LOCALDECL.finditer(code):
        if kinds[m.start(1)] != ord("c"):
            continue
        local.add(m.group(1))
        end = code.find(";", m.end())
        local.update(LOCALMORE.findall(code[m.end():end if end >= 0 else len(code)]))  # f32 m24 = .., m2C = ..
    n = 0
    for m in hits:
        k = kinds[m.start()]
        if k == ord("e"):
            continue
        member = re.search(r"(?:->|\.)\s*$", code[max(0, m.start() - 4):m.start()]) is not None
        if m.group(0) in local and k != ord("s") and not member:
            continue
        n += 1
    return n


FRAMELOCAL = re.compile(r"\bFrameLocal\s*<")
AUTOFRAME = re.compile(r"\bAutoFrame\b|\bWWHD_FUNC\s*\(")
MARKER = re.compile(r"(?=^/\* [0-9A-Fa-f]{8} \*/)", re.M)


def framenoauto(text):
    """FrameLocal uses in marker-delimited functions that construct no automatic frame"""
    n = 0
    for seg in MARKER.split(text):
        code = strip(seg)
        uses = len(FRAMELOCAL.findall(code))
        if uses and not AUTOFRAME.search(code):
            n += uses
    return n


def strip(text):
    """remove comments and string/char literals, keep line structure"""
    out, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("\n" * text.count("\n", i, j))
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == "\\" else 1
            out.append(c + c)
            i = j + 1
        else:
            out.append(c)
            i += 1
    return "".join(out)


def lint(path):
    code = strip(open(path, errors="replace").read())
    row = {name: len(p.findall(code)) for name, p in PATTERNS}
    row["fieldnum"] += short_offset_names(code)
    row["framenoauto"] = framenoauto(open(path, errors="replace").read())
    row["lines"] = sum(1 for l in code.splitlines() if l.strip())
    return row


def files_for(paths):
    out = []
    for p in paths:
        p = os.path.relpath(os.path.abspath(p), ROOT) if os.path.isabs(p) or os.path.exists(p) else p
        if p not in out:
            out.append(p)
        if p.startswith("wwhd_src/") and p.endswith(".cpp"):
            h = "wwhd_src/include/" + p[len("wwhd_src/"):-4] + ".h"
            if os.path.exists(os.path.join(ROOT, h)) and h not in out:
                out.append(h)
    return out


def main():
    args = sys.argv[1:]
    tsv = "--tsv" in args
    args = [a for a in args if a != "--tsv"]
    if "--all" in args:
        srcs = []
        for dp, _, fs in os.walk(os.path.join(HERE, "units")):
            for f in fs:
                if f.endswith(".txt"):
                    for line in open(os.path.join(dp, f)):
                        w = line.split("#")[0].split()
                        if w and w[0] == "src":
                            srcs += [s for s in w[1:] if s not in srcs]
        args = sorted(srcs)
    if not args:
        sys.exit(__doc__)
    files = files_for(args)
    tot = dict.fromkeys(COLS, 0)
    if tsv:
        print("\t".join(["file"] + COLS))
    else:
        print("readable_lint (counts, not failures; see docs/readability-style.md)")
        print("%-52s " % "file" + " ".join("%8s" % c for c in COLS))
    for f in files:
        full = os.path.join(ROOT, f)
        if not os.path.exists(full):
            continue
        row = lint(full)
        for c in COLS:
            tot[c] += row[c]
        if tsv:
            print("\t".join([f] + [str(row[c]) for c in COLS]))
        else:
            print("%-52.52s " % f + " ".join("%8d" % row[c] for c in COLS))
    if not tsv and len(files) > 1:
        print("%-52s " % "TOTAL" + " ".join("%8d" % tot[c] for c in COLS))


if __name__ == "__main__":
    main()

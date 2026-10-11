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
  fieldnum  unnamed fields: field_0x123, unk_123/unk123, mXXXX hex names (m3522), _0x.. members
  fnnum     functions named after addresses (fn_/sub_/func_/FUN_ + hex, or a name ending in an
            8-digit address)
  scaffold  harness scaffolding written by hand: WWHD_FUNC(...) and VERIFY(...) lines (a glue source
            has none: autoglue inserts them)
  local_l   local copies of shared declarations (identifiers ending in _l)
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
    ("fieldnum", re.compile(r"\bfield_0x[0-9A-Fa-f]+\b|\bunk_?[0-9A-Fa-f]{2,}\b|\bm[0-9A-F]{3,4}\b|(?<![0-9A-Za-z])_0x[0-9A-Fa-f]+\b")),
    ("fnnum", re.compile(r"\b(?:fn|sub|func|FUN)_[0-9A-Fa-f]{6,8}\w*\b|\b[A-Za-z]\w*_[0-9A-F]{8}\s*\(")),
    ("scaffold", re.compile(r"\bWWHD_FUNC\s*\(|^\s*VERIFY\s*\(", re.M)),
    ("local_l", re.compile(r"\b[A-Za-z]\w*_l\b")),
]
COLS = [n for n, _ in PATTERNS] + ["lines"]


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

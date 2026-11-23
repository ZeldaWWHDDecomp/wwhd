#!/usr/bin/env python3
"""scopelint.py [FILE|DIR ...] (default wwhd_src): scope objects whose constructor / destructor never run in the glued build.

The source glue (tools/verify/autoglue.py, the harness and the game's decomp build) rewrites every class-typed local of a
function body, `T x;` / `T x(args);` / `T x = e;` (T a plain, non-template name), into a guest stack slot
gabi::LocalOf<T>: its storage is initialised by gabi_construct(x, args...) and finalised by gabi_destroy(x), whose
defaults do nothing (gabi.h). T's own constructor and destructor never run. A RAII gate (`struct DtOne { DtOne() {
t60_set_dt(1); } ~DtOne() { ... } };` then `DtOne d;`) is silently a no-op (mbdoor head x2;
true60.h: "plain calls, no scope object"). Template locals (`Dt1<> d;`, `Form<A> f(a);`) are left as host objects by the
glue and do run.

Flags every such local of a glued body (the function after a `/* ADDR */` marker or an `/* inlined into */` line, as
autoglue.py) whose type (outside the gabi_construct overloads) declares a constructor or a destructor with a
body in the scanned sources. Exit 1 when any is found. `// scopelint: ok REASON` on the line accepts one.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
MARKER = re.compile(r"^/\* ([0-9A-Fa-f]{8}) \*/[ \t]*$", re.M)  # autoglue.py: a marked (glued) function follows
INLINED = re.compile(r"^/\* inlined into [^*\n]*\*/[ \t]*$", re.M)
# autoglue.py's class-typed local (statement line)
LOCAL = re.compile(r"^(\s*)(?:const\s+)?((?:\w+::)*[A-Za-z_]\w*)\s+([A-Za-z_]\w*)\s*(\(.*\)|=.*)?;\s*$")
KEYWORDS = {"return", "goto", "delete", "throw", "case", "else", "typedef", "using", "namespace", "friend", "break",
            "continue", "static_assert", "co_return", "public", "private", "protected", "struct", "class", "enum",
            "union", "new", "sizeof", "do", "extern", "static", "inline", "virtual", "template", "typename", "auto"}
SCALARS = {"int", "char", "short", "long", "float", "double", "bool", "void", "unsigned", "signed", "s8", "u8", "s16",
           "u16", "s32", "u32", "s64", "u64", "f32", "f64", "BOOL", "uint8_t", "uint16_t", "uint32_t", "uint64_t",
           "int8_t", "int16_t", "int32_t", "int64_t", "size_t", "uintptr_t", "intptr_t"}


def strip(src):
    """the source with comments and string / char literals blanked (positions and line breaks kept)"""
    out, i, n = [], 0, len(src)
    while i < n:
        c = src[i]
        if src.startswith("//", i):
            j = src.find("\n", i)
            j = n if j < 0 else j
            out.append(" " * (j - i)); i = j
        elif src.startswith("/*", i):
            j = src.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("".join(ch if ch == "\n" else " " for ch in src[i:j])); i = j
        elif c in "\"'":
            j = i + 1
            while j < n and src[j] != c:
                j += 2 if src[j] == "\\" else 1
            j = min(j + 1, n)
            out.append(c + " " * (j - i - 2) + (c if j - i >= 2 else "")); i = j
        else:
            out.append(c); i += 1
    return "".join(out)


def blocks(code):
    """[(open index, close index, kind, name)]: kind 'class' (struct / class / union NAME ... {, with 'template' just
    before it as 'tclass'), 'func' (a `{` after `)` [const|override|noexcept|final|-> T]*), else 'other'"""
    res, stack = [], []
    for m in re.finditer(r"[{}]", code):
        i = m.start()
        if code[i] == "{":
            head = code[max(0, i - 400):i]
            seg = re.split(r"[;{}]", head)[-1]
            cm = re.search(r"\b(struct|class|union)\s+(?:alignas\([^)]*\)\s+)?([A-Za-z_]\w*)[^()]*$", seg)
            if cm and not re.search(r"\benum\b", seg):
                kind = "tclass" if re.search(r"\btemplate\s*<", seg) else "class"
                stack.append([i, kind, cm.group(2)])
            elif re.search(r"\)\s*(?:const|override|noexcept|final|volatile|&|->\s*[\w:<>\s*&]+)*\s*$", seg) \
                    or re.search(r"\)\s*:\s*[^;]*$", seg):  # a constructor's initialiser list
                stack.append([i, "func", None])
            else:
                stack.append([i, "other", None])
        elif stack:
            o, kind, name = stack.pop()
            res.append((o, i, kind, name))
    return res


def scan(paths):
    files = []
    for p in paths:
        if os.path.isdir(p):
            for dp, _, fs in os.walk(p):
                files += [os.path.join(dp, f) for f in fs if f.endswith((".cpp", ".h", ".hpp", ".inc"))]
        else:
            files.append(p)
    srcs = {}
    for f in sorted(files):
        try:
            raw = open(f, errors="replace").read()
        except OSError:
            continue
        srcs[f] = (raw, strip(raw))
    # types with a constructor / destructor body (non-template classes)
    scoped = {}
    hooked = set()
    for f, (raw, code) in srcs.items():
        for m in re.finditer(r"gabi_construct\s*\(\s*(?:\w+::)*([A-Za-z_]\w*)\s*&", code):
            hooked.add(m.group(1))
        for o, c, kind, name in blocks(code):
            if kind != "class":
                continue
            body = code[o + 1:c]
            if re.search(r"(?:^|[;{}\s])(~?%s)\s*\([^;{}]*\)\s*(?::[^;{]*)?\{" % re.escape(name), body):
                scoped.setdefault(name, []).append("%s:%d" % (os.path.relpath(f, ROOT), code.count("\n", 0, o) + 1))
        # out-of-line X::X() { / X::~X() {
        for m in re.finditer(r"\b([A-Za-z_]\w*)::~?\1\s*\([^;{}]*\)\s*(?::[^;{]*)?\{", code):
            scoped.setdefault(m.group(1), []).append("%s:%d" % (os.path.relpath(f, ROOT), code.count("\n", 0, m.start()) + 1))
    for t in hooked:
        scoped.pop(t, None)
    hits = []
    for f, (raw, code) in srcs.items():
        bl = blocks(code)
        allf = sorted((o, c) for o, c, k, _ in bl if k == "func")
        # only the glued bodies: the function after a `/* ADDR */` marker line or an `/* inlined into ... */` line
        # (autoglue.py MARKER / INLINED); their class-typed locals become guest stack slots
        funcs = []
        for mk in list(MARKER.finditer(raw)) + list(INLINED.finditer(raw)):
            nxt = next(((o, c) for o, c in allf if o > mk.end()), None)
            if nxt:
                funcs.append(nxt)
        classes = [(o, c) for o, c, k, _ in bl if k in ("class", "tclass")]
        rlines, lines = raw.split("\n"), code.split("\n")
        pos = 0
        for ln_no, ln in enumerate(lines, 1):
            at = pos
            pos += len(ln) + 1
            m = LOCAL.match(ln)
            if not m:
                continue
            typ, name = m.group(2), m.group(3)
            base = typ.split("::")[-1]
            if typ in SCALARS or typ in KEYWORDS or name in KEYWORDS or base not in scoped:
                continue
            # inside a function body, and not inside a class body nested deeper than that function
            fn = [o for o, c in funcs if o < at < c]
            if not fn:
                continue
            inner = max(fn)
            if any(inner < o < at < c for o, c in classes):
                continue
            if "scopelint: ok" in rlines[ln_no - 1]:
                continue
            ns = typ.split("::")[0].replace("_t60", "") if "::" in typ else ""
            locs = scoped[base]
            loc = next((l for l in locs if ns and os.path.basename(l).startswith("d_a_%s_" % ns)), locs[0])
            hits.append("%s:%d: `%s %s` - %s declares a constructor / destructor (%s%s): the glue's guest stack slot never "
                        "runs it (use plain calls)" % (os.path.relpath(f, ROOT), ln_no, typ, name, base, loc,
                                                       ", %d definitions of the name" % len(locs) if len(locs) > 1 else ""))
    return hits


def main():
    paths = sys.argv[1:] or [os.path.join(ROOT, "wwhd_src")]
    hits = scan(paths)
    for h in hits:
        print(h)
    print("scopelint: %d scope object(s) whose constructor / destructor the glue drops" % len(hits))
    return 1 if hits else 0


if __name__ == "__main__":
    sys.exit(main())

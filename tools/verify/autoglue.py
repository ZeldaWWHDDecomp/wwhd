#!/usr/bin/env python3
"""autoglue: GameCube-style decompiled source -> harness source.

Opt-in per source file: a file that contains a comment block starting with `/* WWHD glue` is
written in GameCube decompilation style. The visible source only marks each function with its
WWHD address:

    /* 022A400C */
    void daNpc_Nz_c::checkOrder() {
        ...
    }

and the glue block (at the end of the file) lists the addresses of the translation unit's data:

    /* WWHD glue (tools/verify/autoglue.py)
    str  1001EC8C                        string literal (content read from the RPX image)
    str  1001EC3C "modelData != NULL"    ... written differently in source (JUT_ASSERT)
    str  1001EC3C in 022A2AD8            ... only while that function is tested (duplicates)
    str  10057E04 "a->mSize" renamed     ... spelled with other names on purpose (not compared)
    data mode_tbl 1001EE28               static table / object (initialiser checked against the image)
    data offset 10467D20 guard 10467D2C  function-local static with a GHS guard word
    */

Before compilation (verify.py, mutate.py) this tool rewrites the file:
  - after the `{` of each marked function: WWHD_GLUE_FUNC(addr, R, [this,] args) (the harness
    entry/nested-call glue), and VERIFY(addr, fn) at the end of the file; a marked constructor
    X::X() gets a wrapper `X* X__ct(X* self)` (GHS: allocates when self is NULL);
  - class-typed locals `T x(args);` / `T x = e;` -> gabi::Stack<T> (guest stack slot);
  - `static T name[] = {...};` / `static T name;` with a `data` line -> a view of the guest object at
    that address (still `static` at file scope unless some source or header declares the name `extern`:
    then it is shared with other translation units); the initialiser is compared with the RPX image at build time;
  - `static T name(init);` with `data ... guard ...` -> gabi::local_static;
  - the strings of `str` lines are registered (host literal -> guest address, gabi::guest_str), with the
    file's function addresses (gabi::GuestFile: unpinned strings of the running function's file are preferred);
  - `#include "assets/X.h"` (model data, tools/verify/assets.py) is replaced by the header generated
    from the game, on one line; its table is then handled like any other.
Line numbers are kept (one output line per input line), so compiler errors point at the source.

usage: autoglue.py SRC [OUT]       (prints the transformed source)
"""
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))

SCALARS = {"u8", "s8", "u16", "s16", "u32", "s32", "u64", "s64", "f32", "f64", "int", "bool", "BOOL", "char",
           "float", "double", "short", "long", "unsigned", "signed", "cPhs_State", "fpc_ProcID", "auto", "size_t",
           "uintptr_t", "uint8_t", "uint16_t", "uint32_t", "int8_t", "int16_t", "int32_t"}
SCALAR_SIZE = {"u8": 1, "s8": 1, "char": 1, "bool": 1, "u16": 2, "s16": 2, "u32": 4, "s32": 4, "int": 4, "BOOL": 4,
               "f32": 4, "float": 4, "cPhs_State": 4, "fpc_ProcID": 4}
KEYWORDS = {"return", "delete", "goto", "case", "else", "throw", "typedef", "using", "new", "do", "if", "while",
            "for", "switch", "break", "continue", "default", "co_return", "static_assert", "sizeof", "struct",
            "class", "enum", "union", "const", "static", "extern", "volatile", "inline", "template", "typename"}


class GlueError(Exception):
    pass


# ------------------------------------------------------------------ image
_image = None


def image():
    global _image
    if _image is None:
        path = os.path.join(ROOT, "build", "verify", "image.bin")
        d = open(path, "rb").read()
        regs, i = [], 0
        while i < len(d):
            a, n = struct.unpack_from("<II", d, i)
            i += 8
            regs.append((a, d[i:i + n]))
            i += n
        _image = regs
    return _image


def img_read(a, n):
    for b, data in image():
        if b <= a and a + n <= b + len(data):
            return data[a - b:a - b + n]
    return None


def img_cstr(a):
    out = b""
    while True:
        c = img_read(a, 1)
        if c is None:
            raise GlueError("string at %08X is outside the image" % a)
        if c == b"\0":
            return out.decode("latin1")
        out += c
        a += 1


def c_str_bytes(body):
    """the bytes a C++ compiler makes of a narrow string literal's body (between the quotes), with the
    UTF-8 execution character set of the host build: characters outside ASCII become their UTF-8 bytes,
    escapes (\\n, \\xHH.., \\ooo, ...) their byte, \\uXXXX its UTF-8 bytes. Returned one character per
    byte (latin1), like img_cstr."""
    simple = {"n": 10, "t": 9, "r": 13, "a": 7, "b": 8, "f": 12, "v": 11, "\\": 92, "'": 39, '"': 34, "?": 63}
    out, i = bytearray(), 0
    while i < len(body):
        c = body[i]
        i += 1
        if c != "\\":
            out += c.encode("utf-8")
            continue
        if i >= len(body):
            raise GlueError("string %r: ends in a backslash" % body)
        e = body[i]
        i += 1
        if e in simple:
            out.append(simple[e])
        elif e == "x":
            j = i
            while j < len(body) and body[j] in "0123456789abcdefABCDEF":
                j += 1
            if j == i or int(body[i:j], 16) > 0xFF:
                raise GlueError("string %r: bad \\x escape" % body)
            out.append(int(body[i:j], 16))
            i = j
        elif e in "uU":  # universal character name: its UTF-8 bytes
            k = 4 if e == "u" else 8
            h = body[i:i + k]
            if len(h) != k or any(ch not in "0123456789abcdefABCDEF" for ch in h):
                raise GlueError("string %r: bad \\%s escape" % (body, e))
            out += chr(int(h, 16)).encode("utf-8")
            i += k
        elif e in "01234567":
            j = i
            while j < len(body) and j < i + 2 and body[j] in "01234567":
                j += 1
            v = int(body[i - 1:j], 8)
            if v > 0xFF:
                raise GlueError("string %r: bad octal escape" % body)
            out.append(v)
            i = j
        else:
            raise GlueError("string %r: unknown escape \\%s" % (body, e))
    return out.decode("latin1")


def same_text(host, content):
    """host (source bytes) and content (image bytes) spell the same text: identical bytes; or the source
    writes each image byte as a character U+0000..U+00FF (`\\u0082f...`: UTF-8 in the host build, the form
    older units use); or it writes the image's Shift-JIS text in UTF-8 (a readable Japanese literal).
    Only the host side is spelled differently: the guest always gets the image's bytes at ADDR."""
    if host == content:
        return True
    try:
        text = host.encode("latin1").decode("utf-8")
    except UnicodeDecodeError:
        return False
    if text == content:
        return True
    try:
        return text == content.encode("latin1").decode("cp932")
    except UnicodeDecodeError:
        return False


def source_spelling(text, glue_pos, content):
    """the host bytes of the source's literal for an image string given without source text (`str ADDR`).
    The registration must hold the bytes the source literal compiles to (gabi::guest_str looks the literal
    up by its bytes). A literal with exactly the image bytes (`"\\x90\\x55..."`) wins; else one that
    spells the same text another way (same_text: `\\u0090U...` or UTF-8 Japanese); else the image bytes."""
    code = mask(text[:glue_pos])
    alt = None
    for lm in re.finditer(r'"(x*)"', code):
        try:
            b = c_str_bytes(text[lm.start() + 1:lm.end() - 1])
        except GlueError:
            continue
        if b == content:
            return content
        if alt is None and same_text(b, content):
            alt = b
    return alt if alt is not None else content


# ------------------------------------------------------------------ lexing helpers
def mask(text):
    """same length as text: comments -> spaces, string/char literal contents -> 'x' (quotes kept)"""
    out = list(text)
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            for k in range(i, j):
                out[k] = " "
            i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            for k in range(i, j):
                if out[k] != "\n":
                    out[k] = " "
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == "\\" else 1
            for k in range(i + 1, min(j, n)):
                out[k] = "x"
            i = j + 1
        else:
            i += 1
    return "".join(out)


def match_close(code, i, open_ch, close_ch):
    """index of the bracket closing the one at code[i]"""
    depth = 0
    for j in range(i, len(code)):
        if code[j] == open_ch:
            depth += 1
        elif code[j] == close_ch:
            depth -= 1
            if depth == 0:
                return j
    raise GlueError("unbalanced %s at offset %d" % (open_ch, i))


def split_top(s, sep=","):
    parts, depth, cur = [], 0, ""
    for ch in s:
        if ch in "([{<":
            depth += 1
        elif ch in ")]}>":
            depth -= 1
        if ch == sep and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip():
        parts.append(cur)
    return parts


def line_of(text, pos):
    return text.count("\n", 0, pos) + 1


# ------------------------------------------------------------------ glue block
def parse_glue(text):
    m = re.search(r"/\* WWHD glue(.*?)\*/", text, re.S)
    if not m:
        return None
    strs, datas, exts = [], {}, []
    for raw in m.group(1).split("\n")[1:]:
        ln = raw.strip()
        if not ln or ln.startswith("#"):
            continue
        sm = re.match(r'^str\s+([0-9A-Fa-f]{8})(?:\s+("(?:[^"\\]|\\.)*"))?(\s+renamed)?(?:\s+in\s+([0-9A-Fa-f]{8}))?\s*(?:#.*)?$', ln)
        dm = re.match(r"^data\s+(\w+)\s+([0-9A-Fa-f]{8})(?:\s+guard\s+([0-9A-Fa-f]{8}))?(?:\s+in\s+(\w+))?(\s+nocheck)?\s*(?:#.*)?$", ln)
        em = re.match(r"^ext\s+([0-9A-Fa-f]{8})\s+(.*?)\s*(?:#.*)?$", ln)
        if em:
            # `callback`: the source passes the function by name: register it as a HostFn
            cb = re.match(r"^(.*\))\s*(const\s*)?callback$", em.group(2))
            sig = (cb.group(1) + (" const" if cb.group(2) else "")) if cb else em.group(2)
            exts.append((int(em.group(1), 16), sig, bool(cb)))
            continue
        if sm:
            addr = int(sm.group(1), 16)
            content = img_cstr(addr)
            host = c_str_bytes(sm.group(2)[1:-1]) if sm.group(2) else source_spelling(text, m.start(), content)
            # `renamed`: the source spells the text with other (GameCube) names on purpose, e.g. an assert
            # condition whose HD member names differ from zeldaret/tww's; say which in a comment
            if sm.group(2) and not sm.group(3) and not same_text(host.replace("NULL", "(0)"), content):
                raise GlueError("str %08X: source text %r does not match the image %r (intended? add `renamed`)" % (addr, host, content))
            strs.append((addr, host, int(sm.group(4), 16) if sm.group(4) else 0))
        elif dm:
            datas[dm.group(1)] = (int(dm.group(2), 16), int(dm.group(3), 16) if dm.group(3) else None, bool(dm.group(5)))
        else:
            raise GlueError("glue line not understood: %s" % ln)
    return strs, datas, exts, m.start()


# ------------------------------------------------------------------ layouts for initialiser checks
_layout_cache = {}


def struct_defs(text):
    """every struct/class definition of a text: [(qualified name, match of `struct X ... {`)], the
    qualified name built from the enclosing struct/class/namespace definitions"""
    code = mask(text)
    out, stack = [], []  # stack: (name or None, close position)
    for m in re.finditer(r"\b(struct|class|namespace)\s+(\w+)\s*(?::[^{;()]*)?\{", code):
        while stack and stack[-1][1] < m.start():
            stack.pop()
        close = match_close(code, m.end() - 1, "{", "}")
        scope = "::".join(n for n, _ in stack)
        qual = (scope + "::" if scope else "") + m.group(2)
        if m.group(1) != "namespace":
            out.append((qual, m))
        stack.append((m.group(2), close))
    return out


def include_closure(srctext, srcpath):
    """the headers a source includes ("..." includes, transitively, breadth first) as [(path, text)]"""
    dirs = [os.path.join(ROOT, "wwhd_src", "include"), os.path.join(ROOT, "tools", "verify", "include")]
    if srcpath and srcpath != "<src>":
        dirs.insert(0, os.path.dirname(os.path.join(ROOT, srcpath)))
    out, seen, queue = [], set(), [srctext]
    while queue:
        t = queue.pop(0)
        for inc in re.findall(r'^\s*#\s*include\s*"([^"]+)"', t, re.M):
            for d in dirs:
                p = os.path.normpath(os.path.join(d, inc))
                if os.path.exists(p):
                    if p not in seen:
                        seen.add(p)
                        txt = open(p, errors="replace").read()
                        out.append((p, txt))
                        queue.append(txt)
                    break
    return out


MEMBER_DECL = re.compile(r"^(?:\s*(?:public|private|protected)\s*:)*\s*(static\s+)?(?:inline\s+|virtual\s+|constexpr\s+)*"
                         r"(?:[^=;(]*?[\s*&])?(~?\w+)\s*\(")


def class_members(cls, srctext, srcpath="<src>"):
    """the member functions class `cls` declares (in the source or a header it includes):
    {name: [declared static?, ...]}, one entry per declaration (overloads)"""
    key = ("members", cls, srcpath, hash(srctext))
    if key in _layout_cache:
        return _layout_cache[key]
    out = {}

    def decl(stmt):
        dm = MEMBER_DECL.match(stmt)
        if dm:
            out.setdefault(dm.group(2), []).append(bool(dm.group(1)))

    for _, t in [(srcpath, srctext)] + include_closure(srctext, srcpath):
        for q, m in struct_defs(t):
            if q != cls and not q.endswith("::" + cls):
                continue
            code = mask(t)
            end = match_close(code, m.end() - 1, "{", "}")
            depth, stmt = 0, ""
            for ch in code[m.end():end]:  # top-level declarations of the class body
                if ch == "{":
                    if depth == 0:
                        decl(stmt)  # an inline definition
                    depth += 1
                elif ch == "}":
                    depth -= 1
                    stmt = ""
                elif depth == 0:
                    stmt += ch
                    if ch == ";":
                        decl(stmt)
                        stmt = ""
                    elif ch == ":" and re.match(r"^\s*(?:public|private|protected)\s*:$", stmt):
                        stmt = ""
            break  # the first definition of the class (the source's own, else the nearest header)
        else:
            continue
        break
    _layout_cache[key] = out
    return out


def static_members(cls, srctext, srcpath="<src>"):
    """names of the member functions class `cls` declares `static`: an out-of-class definition
    `R C::f(...)` does not repeat the keyword, so the class definition decides whether f takes `this`"""
    return {n for n, st in class_members(cls, srctext, srcpath).items() if any(st)}


def is_lvalue_ref(ptype):
    """a parameter type `T&` / `const T&` (not `T&&`): the guest ABI passes its address"""
    t = ptype.strip()
    return t.endswith("&") and not t.endswith("&&")


def find_struct(name, srctext, srcpath="<src>", scope=""):
    """fields of a guest-layout struct: [(kind, size, count)] (kind: int/float/ptmf/ptr/pad).
    The name is resolved by scope, as C++ does for these tables: a qualified name (A::B) exactly; a
    bare name first in the enclosing class `scope` (the class of the member function the table is
    in), then at file scope, in the source and the headers it includes (breadth first); only then
    in any header (top level first). Two units may each have their own nested ProcEntry."""
    key = (name, scope, srcpath)
    if key in _layout_cache:
        return _layout_cache[key]
    near = [(srcpath, srctext)] + include_closure(srctext, srcpath)
    far = []
    for dp, _, fs in os.walk(os.path.join(ROOT, "wwhd_src", "include")):
        for f in sorted(fs):
            if f.endswith(".h"):
                far.append((os.path.join(dp, f), open(os.path.join(dp, f), errors="replace").read()))
    short = name.split("::")[-1]
    defs_near = [(t, q, m) for _, t in near for q, m in struct_defs(t)]
    wanted = []
    if "::" in name:
        wanted.append(lambda q: q == name or q.endswith("::" + name))
    else:
        if scope:
            parts = scope.split("::")
            for i in range(len(parts), 0, -1):
                cand = "::".join(parts[:i]) + "::" + name
                wanted.append(lambda q, cand=cand: q == cand or q.endswith("::" + cand))
        wanted.append(lambda q: q == name)
    found = None
    for w in wanted:
        for t, q, m in defs_near:
            if w(q):
                found = (t, m)
                break
        if found:
            break
    if not found:  # not reachable from the source: any header, top-level definitions first, then nested
        defs_far = [(t, q, m) for _, t in far for q, m in struct_defs(t)]
        for w in wanted + [lambda q: q.split("::")[-1] == short]:
            for t, q, m in defs_near + defs_far:
                if w(q):
                    found = (t, m)
                    break
            if found:
                break
    if found:
        t, m = found
        code = mask(t)
        end = match_close(code, m.end() - 1, "{", "}")
        body = code[m.end():end]
        fields = []
        depth = 0
        stmt = ""
        for ch in body:  # top-level statements only (skip method bodies)
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                stmt = ""
                continue
            if depth:
                continue
            if ch == ";":
                fields.append(stmt.strip())
                stmt = ""
            else:
                stmt += ch
        out = []
        for f in fields:
            f = re.sub(r"\s+", " ", f)
            if not f or "(" in f or f.startswith(("typedef", "using", "enum", "static", "public", "private", "friend")):
                continue
            fm = re.match(r"^(?:const )?(be<(\w+)>|gptr<.*>|gabi::ptmf<.*>|ptmf<.*>|u8|s8|char)\s+(.*)$", f)
            if not fm:
                raise GlueError("cannot read field `%s` of %s" % (f, name))
            for decl in split_top(fm.group(3)):
                decl = decl.strip()
                cnt = 1
                for d in re.findall(r"\[([^\]]+)\]", decl):
                    cnt *= int(eval(d, {}, {}))
                t0 = fm.group(1)
                if t0.startswith("be<"):
                    st = fm.group(2)
                    kind = "float" if st in ("f32", "float") else "int"
                    out.append((kind, SCALAR_SIZE[st], cnt, st))
                elif t0.startswith("gptr<"):
                    out.append(("ptr", 4, cnt, None))
                elif "ptmf<" in t0:
                    out.append(("ptmf", 8, cnt, None))
                else:
                    out.append(("pad" if decl.startswith("_") else "int", 1, cnt, "u8"))
        _layout_cache[key] = out
        return out
    raise GlueError("no layout found for %s (needed to check a table initialiser)" % name)


def leaves(init):
    """flatten a braced initialiser into leaf expressions (strings kept)"""
    s = init.strip()
    if s.startswith("{"):
        inner = s[1:s.rindex("}")]
        out = []
        for p in split_top(inner):
            out += leaves(p)
        return out
    return [s]


def enum_table(srctext, srcpath="<src>"):
    """enumerators of the enums the source and its included headers define:
    {qualified name: value}; a plain enum's constants are also listed in their enclosing scope (C++
    lookup), so `leaves_particle::ID_AK_SN_FALLENLEAFFAN00` and `daX_c::Mode::MODE_WAIT` resolve"""
    key = ("enums", srcpath, hash(srctext))
    if key in _layout_cache:
        return _layout_cache[key]
    out = {}
    for _, t in [(srcpath, srctext)] + include_closure(srctext, srcpath):
        code = mask(t)
        scopes = []  # (name, open brace, close brace)
        for m in re.finditer(r"\b(?:struct|class|namespace)\s+(\w+)\s*(?::[^{;()]*)?\{", code):
            scopes.append((m.group(1), m.end() - 1, match_close(code, m.end() - 1, "{", "}")))
        for m in re.finditer(r"\benum\s+(class\s+|struct\s+)?(\w+)?\s*(?::\s*[\w:\s]+?)?\s*\{", code):
            close = match_close(code, m.end() - 1, "{", "}")
            outer = "::".join(n for n, a, b in sorted(scopes, key=lambda x: x[1]) if a < m.start() < b)
            names = [outer + "::" + m.group(2) if outer else m.group(2)] if m.group(2) else []
            if not m.group(1):  # unscoped: also visible in the enclosing scope
                names.append(outer)
            local, cur = {}, -1
            for item in split_top(t[m.end():close]):
                item = re.sub(r"/\*.*?\*/|//[^\n]*", "", item, flags=re.S).strip()
                if not item:
                    continue
                em = re.match(r"^(\w+)\s*(?:=\s*(.+))?$", item, re.S)
                if not em:
                    break
                if em.group(2) is None:
                    cur += 1
                else:
                    try:
                        cur = int_expr(em.group(2), lambda n: local.get(n))
                    except (GlueError, SyntaxError, TypeError, ValueError, NameError, ZeroDivisionError):
                        break  # a value this checker cannot compute: the rest of this enum stays unknown
                local[em.group(1)] = cur
                for sc in names:
                    out.setdefault((sc + "::" if sc else "") + em.group(1), set()).add(cur)
    _layout_cache[key] = out
    return out


def int_expr(expr, resolve):
    """value of a C integer constant expression of literals, names (resolve(name) -> int or None) and
    + - * | & ^ ~ << >> ( ): table initialisers and enumerator values"""
    v = re.sub(r"/\*.*?\*/", " ", expr, flags=re.S).strip()
    v = re.sub(r"\((?:u|s)(?:8|16|32)\)|\((?:unsigned |signed )?(?:int|short|char|long)\)", " ", v)  # integer casts
    v = re.sub(r"\b(0[xX][0-9a-fA-F]+|\d+)[uUlL]+\b", r"\1", v)

    def name(m):
        val = resolve(m.group(0))
        if val is None:
            raise GlueError("`%s` is not a known constant" % m.group(0))
        return "(%d)" % val
    v = re.sub(r"(?<![\w])(?:::)?[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*", name, v)
    if not re.match(r"^[-+*|&^~<>()0-9xXa-fA-F ]+$", v):
        raise GlueError("`%s` is not an integer constant expression" % expr)
    return int(eval(v, {}, {}))


def enum_resolver(srctext, srcpath, scope=""):
    """resolve(name) for table initialisers: a qualified name by its trailing components, a bare name
    in the enclosing class `scope` first, then anywhere; ambiguous values are an error"""
    table = enum_table(srctext, srcpath)

    def resolve(n):
        n = n.lstrip(":")
        cands = []
        if scope and "::" not in n:
            cands = [v for k, v in table.items() if k == scope + "::" + n or k.endswith("::" + scope + "::" + n)]
        if not cands:
            cands = [v for k, v in table.items() if k == n or k.endswith("::" + n)]
        vals = set().union(*cands) if cands else set()
        if len(vals) > 1:
            raise GlueError("enum constant `%s` is ambiguous (%s)" % (n, ", ".join(hex(x) for x in sorted(vals))))
        return vals.pop() if vals else None
    return resolve


def encode_leaf(leaf, kind, size, st, methods, func_ctx, resolve=None):
    leaf = leaf.strip()
    if kind == "ptmf":
        m = re.match(r"^&?\s*([\w:]+)$", leaf)
        if not m or m.group(1) not in methods:
            raise GlueError("table entry `%s`: not a marked method" % leaf)
        return struct.pack(">hhI", 0, -1, methods[m.group(1)])
    if leaf.startswith('"'):
        return ("str", c_str_bytes(leaf[1:-1]))
    v = leaf.rstrip("fF") if re.match(r"^-?[\d.]+(?:[eE][-+]?\d+)?[fF]$", leaf) else leaf
    if kind == "float":
        return struct.pack(">f", float(eval(v, {}, {})))
    val = int(eval(v.replace("/*", "#"), {}, {})) if re.match(r"^[-+0-9xXa-fA-F() ]+$", v) else None
    if val is None and resolve is not None and re.search(r"[A-Za-z_]", v):
        try:  # named constants (enums of the source and its headers)
            val = int_expr(v, resolve)
        except (GlueError, SyntaxError, TypeError, ValueError, ZeroDivisionError) as e:
            raise GlueError("table entry `%s`: cannot evaluate (%s)" % (leaf, e))
    if val is None:
        raise GlueError("table entry `%s`: cannot evaluate" % leaf)
    fmt = {1: ">B", 2: ">H", 4: ">I"}[size]
    return struct.pack(fmt, val & ((1 << (8 * size)) - 1))


def enclosing_scope(code, pos):
    """the class of the member function whose body contains pos ("" at file scope or in a free function)"""
    best = ""
    for m in re.finditer(r"\b((?:\w+::)+)~?\w+\s*\([^;{}]*\)\s*(?:const\s*)?\{", code[:pos]):
        close = match_close(code, m.end() - 1, "{", "}")
        if close > pos:
            best = m.group(1)[:-2]
    return best


def check_table(name, addr, elem, dims, init, srctext, methods, where, srcpath="<src>", scope=""):
    """compare a static table's initialiser with the image"""
    if elem.endswith("*"):
        fields = [("ptr", 4, 1, None)]
    elif elem in SCALAR_SIZE:
        fields = [("float" if elem in ("f32", "float") else "int", SCALAR_SIZE[elem], 1, elem)]
    else:
        fields = find_struct(elem, srctext, srcpath, scope)
    flat = []
    for kind, size, cnt, st in fields:
        flat += [(kind, size, st)] * cnt
    lv = leaves(init)
    resolve = enum_resolver(srctext, srcpath, scope)
    esize = sum(f[1] for f in flat)
    nelem = 1
    for d in dims:
        nelem *= d
    # GHS pads each element to its alignment; our layouts are explicit
    if len(lv) != len([f for f in flat if f[0] != "pad"]) * nelem:
        raise GlueError("%s: %d initialisers for %d elements of %d fields" % (where, len(lv), nelem, len(flat)))
    off, li = addr, 0
    for e in range(nelem):
        for kind, size, st in flat:
            if kind == "pad":
                off += size
                continue
            enc = encode_leaf(lv[li], kind, size, st, methods, None, resolve)
            li += 1
            got = img_read(off, size)
            if isinstance(enc, tuple):  # pointer to a string
                p = struct.unpack(">I", got)[0]
                s = img_cstr(p)
                if not same_text(enc[1], s):
                    raise GlueError("%s[%d]: image string at %08X is %r, source %r" % (where, e, p, s, enc[1]))
            elif got != enc:
                raise GlueError("%s[%d]: image %s at %08X, source `%s` = %s" % (where, e, got.hex(), off, lv[li - 1], enc.hex()))
            off += size
    return esize, nelem


# ------------------------------------------------------------------ transform
MARKER = re.compile(r"^/\* ([0-9A-Fa-f]{8}) \*/[ \t]*$", re.M)
# an unmarked function that GHS inlined into a marked one (`/* inlined into daXxx_Execute */` on the line
# before it): its body runs as candidate code, so its class-typed locals become guest stack slots too
INLINED = re.compile(r"^/\* inlined into [^*\n]*\*/[ \t]*$", re.M)


def guest_view(elem):
    """C++ type of a table element seen in guest memory"""
    if elem.endswith("*"):
        return "gptr<%s>" % elem[:-1].strip()
    if elem in SCALARS:
        return "be<%s>" % elem
    return elem


_EXTERN_PAT = re.compile(r"^\s*extern\s+(?!\"C\")([^;(){}]*?)(?:\(\s*[&*]\s*(\w+)\s*\)|\b(\w+))\s*(?:\[[^\]]*\])*\s*;", re.M)
_SPEC_SRCS = None
_EXPORTED_CACHE = {}


def _spec_siblings(srcpath):
    """the other sources that share a verify unit with srcpath (tools/verify/units/**.txt `src` lines)"""
    global _SPEC_SRCS
    if _SPEC_SRCS is None:
        _SPEC_SRCS = []
        for dp, _, fs in os.walk(os.path.join(HERE, "units")):
            for f in fs:
                if f.endswith(".txt"):
                    srcs = set()
                    for line in open(os.path.join(dp, f), errors="replace"):
                        w = line.split("#")[0].split()
                        if w and w[0] == "src":
                            srcs |= {x if x.startswith("wwhd_src/") else "wwhd_src/" + x for x in w[1:]}
                    _SPEC_SRCS.append(srcs)
    rel = os.path.relpath(os.path.join(ROOT, srcpath), ROOT) if not srcpath.startswith("wwhd_src/") else srcpath
    out = set()
    for srcs in _SPEC_SRCS:
        if rel in srcs:
            out |= srcs - {rel}
    return out


def exported_names(srctext, srcpath):
    """TU-wide `data` views that other translation units use: names declared `extern` in a header this source
    includes (a merged source's statics exposed to companions) or in a source that shares a verify unit with it
    (or a header that one includes). Only these keep external linkage; every other TU-wide view stays `static`,
    as written (2026-10-12: before, all views lost `static`, and linking several units together, as the port's
    decomp build does, gave duplicate symbols such as l_hio in ji1)."""
    key = (srcpath, hash(srctext))
    if key in _EXPORTED_CACHE:
        return _EXPORTED_CACHE[key]
    names = set()
    texts = [t for _, t in include_closure(srctext, srcpath)]
    for sib in _spec_siblings(srcpath):
        p = os.path.join(ROOT, sib)
        if os.path.exists(p):
            st = open(p, errors="replace").read()
            texts.append(st)
            texts += [t for _, t in include_closure(st, sib)]
    for t in texts:
        names |= {m.group(2) or m.group(3) for m in _EXTERN_PAT.finditer(t)}
    _EXPORTED_CACHE[key] = names
    return names


def transform(text, path="<src>"):
    if "/* WWHD glue" in text:
        import assets  # model data generated from the game (tools/verify/assets.py), included in place
        try:
            text = assets.expand_includes(text)
        except assets.AssetError as e:
            raise GlueError("%s: %s" % (path, e))
    g = parse_glue(text)
    if g is None:
        return text
    strs, datas, exts, glue_pos = g
    code = mask(text)
    lines = text.split("\n")
    edits = {}      # line index -> new line text
    after = []      # text appended at the end
    funcs = []      # (addr, kind, qname, ret, (param types, const) or None, cls)
    methods = {}    # "Class::name" -> addr (for tables)
    bodies = []     # (start, end) offsets of marked function bodies
    used_data = set()

    def set_line(idx, new):
        if idx in edits:
            raise GlueError("%s:%d: two rewrites on one line" % (path, idx + 1))
        edits[idx] = new

    # ---- functions
    for m in MARKER.finditer(text):
        if m.start() >= glue_pos:
            break
        addr = int(m.group(1), 16)
        brace = code.find("{", m.end())
        # the header ends at the first `{` after the parameter list
        par = code.find("(", m.end())
        if par < 0 or par > brace:
            raise GlueError("%s:%d: marker not followed by a function" % (path, line_of(text, m.start())))
        close = match_close(code, par, "(", ")")
        brace = code.find("{", close)
        header = re.sub(r"\s+", " ", code[m.end():brace]).strip()
        end = match_close(code, brace, "{", "}")
        hm = re.match(r"^((?:static |inline )*)(.*?)\s*((?:\w+::)*~?\w+)\s*\((.*)\)\s*(const)?$", header)
        if not hm:
            raise GlueError("%s:%d: cannot parse `%s`" % (path, line_of(text, m.start()), header))
        ret, qname, params = hm.group(2).strip(), hm.group(3), hm.group(4)
        parts = qname.split("::")
        cls = "::".join(parts[:-1]) if len(parts) > 1 else None
        kind = "ctor" if cls and parts[-1] == parts[-2] else ("method" if cls else "free")
        if kind == "method" and parts[-1] in static_members(cls, text, path):
            kind = "static"  # a static member function: no `this`; registered like a free function
        # parameter names (unnamed ones get one)
        names = []
        ptypes = []     # parameter types (typed member-pointer casts for overloads)
        call_args = []  # as passed on to the guest by WWHD_GLUE_FUNC: a reference as its address
        ptext = text[par + 1:close]
        new_ptext = []
        for i, p in enumerate(split_top(ptext)):
            ps = re.sub(r"/\*.*?\*/", "", p).strip()
            ps = ps.split("=")[0].strip()
            if ps in ("", "void"):
                new_ptext.append(p)
                continue
            nm = re.match(r"^(.*[\s*&])([A-Za-z_]\w*)$", ps)
            if nm and nm.group(1).strip() and nm.group(2) not in SCALARS:
                names.append(nm.group(2))
                new_ptext.append(p)
                ptype = nm.group(1)
            else:
                nn = "wwhd_p%d" % i
                names.append(nn)
                new_ptext.append(" " + ps + " " + nn if i else ps + " " + nn)
                ptype = ps
            call_args.append(("&" if is_lvalue_ref(ptype) else "") + names[-1])
            ptypes.append(" ".join(ptype.split()))
        if any(n.startswith("wwhd_p") for n in names):
            hl = line_of(text, par) - 1
            if line_of(text, close) - 1 != hl:
                raise GlueError("%s:%d: unnamed parameters must be on one line" % (path, hl + 1))
            ls = text.rfind("\n", 0, par) + 1
            line = lines[hl]
            set_line(hl, line[:par + 1 - ls] + ",".join(new_ptext) + line[close - ls:])
        bl = line_of(text, brace) - 1
        if kind == "ctor":
            # the constructor stays a C++ constructor; a wrapper is the guest function
            nxt = text.find("\n", end)
            after_line = line_of(text, end) - 1
            wrapper = (" static %s* %s__ct(%s* self) { WWHD_GLUE_FUNC(0x%08X, %s*, self); "
                       "if (!self) self = (%s*)gabi::call<void*>(0x0273AD10, (u32)sizeof(%s)); "
                       "if (gabi::ea(self) != 0) new (self) %s(); return self; }") % (cls, cls, cls, addr, cls, cls, cls, cls)
            funcs.append((addr, "ctor", cls + "__ct", None, None, cls))
            edits.setdefault(("tail", after_line), []).append(wrapper)
        else:
            args = (["this"] if kind == "method" else []) + call_args
            glue = " WWHD_GLUE_FUNC(0x%08X, %s%s);" % (addr, ret, "".join(", " + a for a in args))
            edits.setdefault(("head", bl, brace - (text.rfind("\n", 0, brace) + 1)), []).append(glue)
            funcs.append((addr, kind, qname, ret, (ptypes, bool(hm.group(5))), cls))
            if kind == "method":
                methods[qname] = addr
        bodies.append((brace, end))

    for m in INLINED.finditer(text):
        if m.start() >= glue_pos:
            break
        par = code.find("(", m.end())
        brace = code.find("{", m.end())
        if par < 0 or brace < 0 or par > brace:
            raise GlueError("%s:%d: `inlined into` comment not followed by a function" % (path, line_of(text, m.start())))
        close = match_close(code, par, "(", ")")
        brace = code.find("{", close)
        bodies.append((brace, match_close(code, brace, "{", "}")))

    # ---- statics and class-typed locals (statement lines)
    def in_body(pos):
        return any(a < pos < b for a, b in bodies)

    pos = 0
    li = 0
    skip_until = -1
    while li < len(lines):
        ln = lines[li]
        mln = code[pos:pos + len(ln)]
        start = pos
        pos += len(ln) + 1
        li += 1
        if start >= glue_pos or start < skip_until:
            continue
        s = mln.strip()
        if not s.startswith("static ") and not in_body(start):
            continue
        # static table / object / function-local static
        sm = re.match(r"^(\s*)static\s+(const\s+)?([\w:]+(?:\s*\*)?)\s+(\w+)\s*((?:\[[^\]]*\])*)\s*(=|\(|;)", mln)
        if sm and sm.group(4) in datas:
            name = sm.group(4)
            addr, guard, nocheck = datas[name]
            used_data.add(name)
            indent, const, elem, dims_s, tok = sm.group(1), sm.group(2) or "", sm.group(3).replace(" ", ""), sm.group(5), sm.group(6)
            # extent of the declaration
            j = start + sm.end() - 1
            if tok == "=":
                k = j + 1
                while code[k] in " \t\n":
                    k += 1
                if code[k] == "{":
                    k = match_close(code, k, "{", "}") + 1
                    init = text[code.find("{", j):k]
                else:
                    k = code.find(";", j)
                    init = text[j + 1:k]
                semi = code.find(";", k)
            elif tok == "(":
                k = match_close(code, j, "(", ")")
                init = text[j + 1:k]
                semi = code.find(";", k)
            else:
                init, semi = None, j
            first, last = line_of(text, start) - 1, line_of(text, semi) - 1
            where = "%s:%d %s" % (path, first + 1, name)
            dims = [int(eval(d, {}, {})) if d.strip() else None for d in re.findall(r"\[([^\]]*)\]", dims_s)]
            gv = guest_view(elem)
            if guard is not None:
                if dims:
                    raise GlueError("%s: guarded static arrays are not supported" % where)
                ctor = ("gabi::construct_dispatch(o, %s);" % init) if init else ""
                new = "%s%s& %s = gabi::local_static<%s>(0x%08X, 0x%08X, [&](%s& o) { %s });" % (
                    indent, gv, name, gv, addr, guard, gv, ctor)
            elif dims:
                if init is None:
                    raise GlueError("%s: table without initialiser" % where)
                if dims[0] is None:
                    dims[0] = len(split_top(init.strip()[1:init.strip().rindex("}")]))
                if not nocheck:
                    inner = 1
                    for d in dims[1:]:
                        inner *= d
                    check_table(name, addr, elem, dims, init, text, methods, where, path, enclosing_scope(code, start))
                ext = "".join("[%d]" % d for d in dims)
                new = "%s%s%s (&%s)%s = gabi::object_at<%s%s%s>(0x%08X);" % (
                    indent, const, gv, name, ext, const, gv, ext, addr)
            else:
                if init is not None and tok == "=" and elem in SCALARS and not init.strip().startswith("{"):
                    init = "{" + init.strip() + "}"  # `static s32 x = 1;`: a scalar in .data, checked like a table
                if init is not None:  # a constant object (.rodata/.data): its initialiser is checked
                    if tok != "=" or not init.strip().startswith("{"):
                        raise GlueError("%s: initialised static object needs a guard (or `= {...}`)" % where)
                    if not nocheck:
                        check_table(name, addr, elem, [1], init, text, methods, where, path, enclosing_scope(code, start))
                new = "%s%s%s& %s = gabi::object_at<%s%s>(0x%08X);" % (indent, const, gv, name, const, gv, addr)
            if guard is None and indent == "" and not in_body(start) and name not in exported_names(text, path):
                new = new[:len(indent)] + "static " + new[len(indent):]  # file scope: keep the source's internal linkage
            set_line(first, new)
            for x in range(first + 1, last + 1):
                set_line(x, "")
            if last + 1 > li:
                pos = sum(len(l) + 1 for l in lines[:last + 1])
                li = last + 1
            continue
        if sm and in_body(start):
            raise GlueError("%s:%d: function-local static `%s` needs a `data` glue line" % (path, line_of(text, start), sm.group(4)))
        if not in_body(start):
            continue
        # class-typed local: `T x;`, `T x(args);`, `T x = e;`
        lm = re.match(r"^(\s*)(?:const\s+)?((?:\w+::)*[A-Za-z_]\w*)\s+([A-Za-z_]\w*)\s*(\(.*\)|=.*)?;\s*$", mln)
        if lm and lm.group(2) not in SCALARS and lm.group(2) not in KEYWORDS and lm.group(3) not in KEYWORDS:
            indent, typ, name, init = lm.group(1), lm.group(2), lm.group(3), lm.group(4)
            org = ln[len(indent):]
            semi = mln.rindex(";")
            if init is None:
                args = ""
            elif init.startswith("("):
                a0 = mln.index("(", len(indent) + len(typ) + 1)
                args = ln[a0 + 1:mln.rindex(")")]
            else:
                a0 = mln.index("=", len(indent))
                args = ln[a0 + 1:semi].strip()
            tail = ln[semi + 1:]
            ctor = "(%s)" % args if args.strip() else ""
            new = "%sgabi::LocalOf<%s> %s__s%s; [[maybe_unused]] %s& %s = %s__s.ref();%s" % (indent, typ, name, ctor, typ, name, name, tail)
            set_line(li - 1, new)

    unused = set(datas) - used_data
    if unused:
        raise GlueError("%s: glue data not found in source: %s" % (path, ", ".join(sorted(unused))))

    # ---- apply line edits
    out = []
    head_edits = {}
    tail_edits = {}
    for k, v in edits.items():
        if isinstance(k, tuple) and k[0] == "head":
            head_edits.setdefault(k[1], []).append((k[2], v))
        elif isinstance(k, tuple) and k[0] == "tail":
            tail_edits[k[1]] = v
    for i, ln in enumerate(lines):
        if i in edits:
            ln = edits[i]
        for col, ins in sorted(head_edits.get(i, []), reverse=True):
            if i in edits:  # parameters renamed on the header line: the body starts at its last `{`
                col = ln.rindex("{")
            ln = ln[:col + 1] + "".join(ins) + ln[col + 1:]
        if i in tail_edits:
            ln = ln + "".join(tail_edits[i])
        out.append(ln)

    # ---- registrations
    reg = ["", "/* ---- autoglue: registrations ---- */"]
    if strs:
        # the file's functions: gabi::guest_str prefers this file's unpinned strings while one of them runs
        # (the port's decomp build links several sources into one registry)
        faddrs = sorted({f[0] for f in funcs})
        reg.append("static const u32 wwhd_file_funcs_[] = {%s};" % (", ".join("0x%08X" % a for a in faddrs) or "0"))
        reg.append("static const gabi::GuestFile wwhd_file_{wwhd_file_funcs_, %d};" % len(faddrs))
    for n, (addr, host, func) in enumerate(strs):
        reg.append("static gabi::GuestStr wwhd_str_%d(0x%08X, %s, 0x%08X, &wwhd_file_);" % (n, addr, cstr_lit(host), func))
    declared = None

    def ext_hostfn(qname, ret, params, addr):
        """`ext ADDR sig callback`: the source passes this other unit's function by name
        (`fopAcM_entrySolidHeap(this, useHeapInit, n)`); gabi::guest_fn maps the binding to ADDR.
        Opt-in, so the objects of units without the flag stay the same."""
        sig = ", ".join(p.strip() for p in params if p.strip() not in ("", "void"))
        return "static gabi::HostFn wwhd_ext_fn_%08X((const void*)static_cast<%s (*)(%s)>(&%s), 0x%08X);" % (
            addr, ret, sig, qname, addr)

    for addr, sig, callback in exts:
        hm = re.match(r"^(.*?)\s*((?:\w+::)*~?\w+)\s*\((.*)\)\s*(const)?$", sig)
        if not hm:
            raise GlueError("ext %08X: cannot parse `%s`" % (addr, sig))
        ret, qname, params = hm.group(1).strip(), hm.group(2), hm.group(3)
        # an included header (gc_api) may already define this function with the same signature: then the
        # ext line only documents it (no second definition); it must call the same address
        if "::" not in qname:
            if declared is None:
                declared = header_functions(text, path)
            hit = declared.get((qname, sig_key(params)))
            if hit is not None:
                hret, hbody = hit
                if " ".join(hret.split()) != " ".join(ret.split()) or ("0x%08X" % addr).lower() not in hbody.lower():
                    raise GlueError("ext %08X %s: an included header defines %s(%s) differently (return type or address)"
                                    % (addr, sig, qname, params))
                if callback:
                    reg.append(ext_hostfn(qname, ret, split_top(params), addr))
                continue
        names, ps = [], []
        for i, p in enumerate(split_top(params)):
            p = p.strip()
            if p in ("", "void"):
                continue
            nm = re.match(r"^(.*[\s*&])([A-Za-z_]\w*)$", p)
            if nm and nm.group(1).strip() and nm.group(2) not in SCALARS:
                names.append(("&" if is_lvalue_ref(nm.group(1)) else "") + nm.group(2))
                ps.append(p)
            else:
                names.append(("&" if is_lvalue_ref(p) else "") + "wwhd_p%d" % i)
                ps.append(p + " wwhd_p%d" % i)
        member = "::" in qname and qname.split("::")[-1] not in static_members(qname.rsplit("::", 1)[0], text, path)
        args = (["this"] if member else []) + names
        reg.append("%s %s(%s)%s { return gabi::call<%s>(0x%08X%s); } /* other unit (ext) */" % (
            ret, qname, ", ".join(ps), " const" if hm.group(4) else "", ret, addr, "".join(", " + a for a in args)))
        if callback:
            if member:
                raise GlueError("ext %08X %s: `callback` needs a free or static function" % (addr, sig))
            reg.append(ext_hostfn(qname, ret, ps, addr))
    marked = {}
    for f in funcs:
        marked[f[2]] = marked.get(f[2], 0) + 1
    for addr, kind, qname, ret, sig, cls in funcs:
        # an overloaded name (declared more than once in its class, or marked twice here) is ambiguous
        # as `&C::f`: name the overload by a typed cast; the harness lists it as `&C::f(types)`
        overloaded = kind != "ctor" and (marked[qname] > 1 or (
            cls is not None and len(class_members(cls, text, path).get(qname.split("::")[-1], [])) > 1))
        if overloaded:
            ptypes, is_const = sig
            if kind == "method":
                fn = "static_cast<%s (%s::*)(%s)%s>(&%s)" % (ret, cls, ", ".join(ptypes), " const" if is_const else "", qname)
                shown = "&%s(%s)%s" % (qname, ", ".join(ptypes), " const" if is_const else "")
            else:
                fn = "static_cast<%s (*)(%s)>(&%s)" % (ret, ", ".join(ptypes), qname)
                shown = "%s(%s)" % (qname, ", ".join(ptypes))
            reg.append("VERIFY_NAMED(0x%08X, %s, %s);" % (addr, fn, cstr_lit(shown)))
            if kind != "method":
                reg.append("static gabi::HostFn wwhd_fn_%08X((const void*)%s, 0x%08X);" % (addr, fn, addr))
        elif kind == "method":
            reg.append("VERIFY(0x%08X, &%s);" % (addr, qname))
        else:
            reg.append("VERIFY(0x%08X, %s);" % (addr, qname))
            reg.append("static gabi::HostFn wwhd_fn_%08X((const void*)&%s, 0x%08X);" % (addr, qname, addr))
    return "#line 1 \"%s\"\n" % path + "\n".join(out) + "\n#line %d \"%s.glue\"\n" % (1, path) + "\n".join(reg) + "\n"


def sig_key(params):
    """parameter types without names, whitespace-normalised"""
    out = []
    for p in split_top(params):
        p = " ".join(p.split())
        if p in ("", "void"):
            continue
        p = re.sub(r"\s*=.*$", "", p)  # default arguments
        nm = re.match(r"^(.*[\s*&])([A-Za-z_]\w*)$", p)
        if nm and nm.group(1).strip() and nm.group(2) not in SCALARS:
            p = nm.group(1)
        out.append(re.sub(r"\s+", "", p))
    return tuple(out)


def header_functions(srctext, srcpath):
    """free inline functions the source's included headers define: {(name, param types): (return type, body)}"""
    found = {}
    for _, t in include_closure(srctext, srcpath):
        code = mask(t)
        for m in re.finditer(r"^[ \t]*(?:static\s+)?inline\s+([^;{}()]*?)\b(\w+)\s*\(([^;{}]*)\)\s*\{", code, re.M):
            close = match_close(code, m.end() - 1, "{", "}")
            found.setdefault((m.group(2), sig_key(t[m.start(3):m.end(3)])), (m.group(1).strip(), t[m.end():close]))
    return found


def cstr_lit(s):
    """a C++ string literal with exactly the bytes of `s` (one character per byte, as img_cstr returns):
    control bytes and bytes outside ASCII are written as 3-digit octal escapes (a raw newline would end
    the literal; a raw Latin-1 character would be written as two UTF-8 bytes)"""
    out = []
    for c in s:
        if c == "\\" or c == '"':
            out.append("\\" + c)
        elif ord(c) < 0x20 or ord(c) >= 0x7F:  # control bytes, DEL, bytes >= 0x80
            if ord(c) > 0xFF:
                raise GlueError("string %r: character %r is not one byte" % (s, c))
            out.append("\\%03o" % ord(c))
        else:
            out.append(c)
    return '"' + "".join(out) + '"'


def bodies(text):
    """(addr, start, end) of each marked function, for mutate.py (helpers between functions included)"""
    gm = re.search(r"/\* WWHD glue", text)
    if gm is None:
        return None
    glue_pos = gm.start()
    code = mask(text)
    out = []
    prev = 0
    for m in MARKER.finditer(text):
        if m.start() >= glue_pos:
            break
        par = code.find("(", m.end())
        close = match_close(code, par, "(", ")")
        brace = code.find("{", close)
        end = match_close(code, brace, "{", "}")
        out.append((int(m.group(1), 16), prev if prev else m.start(), end + 1))
        prev = end + 1
    return out


def main():
    src = sys.argv[1]
    text = open(src).read()
    try:
        res = transform(text, os.path.relpath(src, ROOT) if os.path.isabs(src) else src)
    except GlueError as e:
        sys.exit("autoglue: %s" % e)
    if len(sys.argv) > 2:
        open(sys.argv[2], "w").write(res)
    else:
        sys.stdout.write(res)


if __name__ == "__main__":
    main()

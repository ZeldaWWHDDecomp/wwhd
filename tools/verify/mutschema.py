#!/usr/bin/env python3
"""Mutant schemata for mutate.py --schemata (2026-10-26): all mutants of one unit source in one object file.

The old tool compiles and links every mutant on its own (mutantN.cpp -> mutantN.o -> mutantN). A schema is the unit
source (as compiled: autoglue output, or `#line 1` + text) with each schema-able mutant K rewritten into a runtime
choice between the mutant and the original (see src/mutschema.h); one -O1 compile and one link serve all of them,
and the harness picks the live mutant from WWHD_MUT_ACTIVE.

Exactness: a mutant goes into a schema only when
- the compiled text of the mutant (autoglue.transform of the mutated source, exactly what the old tool compiles)
  equals the compiled text of the original with that one site replaced (map_site), and
- its site is code (not inside a comment, string or preprocessor line) and its kind has a rewrite that keeps the
  parse of everything around it (see the rewrite functions), and
- the schema compiles with the site in it.
Everything else (and every site a compile error points at) falls back to the old per-mutant build. Sites that
overlap without nesting (adjacent swaps) go into separate layers: one schema object per layer.
"""
import os
import re

# ------------------------------------------------------------------ lexing
TOKEN = re.compile(r"""
    (?P<id>[A-Za-z_]\w*)
  | (?P<num>(?:\d+\.\d*|\.\d+|\d+)(?:[eE][+-]?\d+)?[A-Za-z_]*|0[xX][0-9A-Fa-f]+[A-Za-z_]*)
  | (?P<str>"x*"|'x*')
  | (?P<op>->\*|\.\.\.|<<=|>>=|<=>|::|->|\+\+|--|<<|>>|<=|>=|==|!=|&&|\|\||\+=|-=|\*=|/=|%=|&=|\|=|\^=|[-+*/%&|^~!<>=?:;,.()\[\]{}\#])
""", re.X)
ASSIGN = {"=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="}
OPEN, CLOSE = {"(": ")", "[": "]", "{": "}"}, {")": "(", "]": "[", "}": "{"}


def code_mask(text):
    """autoglue.mask (comments -> spaces, string contents -> x) with preprocessor lines blanked to '#' as well"""
    import autoglue
    m = list(autoglue.mask(text))
    pos = 0
    cont = False
    for line in text.split("\n"):
        if cont or line.lstrip().startswith("#"):
            for k in range(pos, pos + len(line)):
                m[k] = "#"
            cont = line.rstrip().endswith("\\")
        pos += len(line) + 1
    return "".join(m)


def tokens(masked):
    out = []
    for t in TOKEN.finditer(masked):
        out.append((t.start(), t.end(), t.group(0)))
    return out


# ------------------------------------------------------------------ mapping a mutant into the compiled text
def common_prefix(a, b):
    """length of the common prefix of a and b (binary search on slice equality: C-speed compares)"""
    lo, hi = 0, min(len(a), len(b))
    while lo < hi:
        mid = (lo + hi + 1) // 2
        if a[:mid] == b[:mid]:
            lo = mid
        else:
            hi = mid - 1
    return lo


def common_suffix(a, b, limit):
    lo, hi = 0, min(len(a), len(b), limit)
    while lo < hi:
        mid = (lo + hi + 1) // 2
        if a[len(a) - mid:] == b[len(b) - mid:]:
            lo = mid
        else:
            hi = mid - 1
    return lo


def map_site(t0, tk, old, new):
    """x such that tk == t0[:x] + new + t0[x+len(old):] and t0[x:x+len(old)] == old, or None"""
    if len(tk) - len(t0) != len(new) - len(old):
        return None
    p = common_prefix(t0, tk)
    q = common_suffix(t0, tk, min(len(t0), len(tk)) - p)
    # the change lies in t0[p : len(t0)-q]; the site may extend over equal characters on both sides
    lo = max(0, len(t0) - q - len(old))
    for x in range(min(p, len(t0) - len(old)), lo - 1, -1):
        if t0.startswith(old, x) and tk.startswith(new, x) and t0[x + len(old):] == tk[x + len(new):] and t0[:x] == tk[:x]:
            return x
    return None


# ------------------------------------------------------------------ sites
class Site:
    """one schema-able mutant: kind, mutant id k, span [s, e) in the compiled text, sub-spans whose text is rewritten
    recursively (nested sites), and render(subtexts) -> replacement text for the span"""

    def __init__(self, k, kind, s, e, subs, render):
        self.k, self.kind, self.s, self.e, self.subs, self.render = k, kind, s, e, subs, render

    def __repr__(self):
        return "Site(%d %s %d:%d)" % (self.k, self.kind, self.s, self.e)


def lit_type(lit):
    """the C++ type class of an integer/float literal mutate.py writes (None: not one we handle)"""
    if re.fullmatch(r"\d+\.\d+f", lit):
        return "float"
    if re.fullmatch(r"0[xX][0-9A-Fa-f]+", lit):
        v = int(lit, 16)
        return "int" if v <= 0x7FFFFFFF else "unsigned" if v <= 0xFFFFFFFF else "long" if v <= 0x7FFFFFFFFFFFFFFF else "ulong"
    if re.fullmatch(r"[1-9]\d*|0", lit):
        v = int(lit)
        return "int" if v <= 0x7FFFFFFF else "long" if v <= 0x7FFFFFFFFFFFFFFF else None
    return None


DECL = re.compile(r"^(?:[A-Za-z_][\w:]*(?:\s*<[^;=]*>)?[\s*&]+)+[A-Za-z_]\w*\s*(?:=|;|\[|\{|\()")


def statement_ok(code):
    """a one-line statement that can be wrapped in a block without changing what it means: balanced, no braces,
    no labels or case/default/else, not a declaration (its scope would end at the block)"""
    c = code.strip()
    if not c.endswith(";") or "{" in c or "}" in c or "#" in c:
        return False
    # a whole one-line if/for/while statement stays one statement inside the block (an `else` on the next line would
    # lose its `if` and fail to compile, never change meaning)
    if re.match(r"^(else|case|default|return|do|switch|goto|template|typedef|using|static|const|auto|"
                r"extern|struct|class|enum|union|namespace)\b", c):
        return False
    if re.match(r"^(if|for|while)\b", c):
        return True if _balanced(c) and not re.search(r"\belse\b", c) else False
    if re.match(r"^[A-Za-z_]\w*\s*:(?!:)", c):
        return False
    if DECL.match(c):
        return False
    return _balanced(c)


def _balanced(c):
    depth = 0
    for ch in c:
        if ch in "([":
            depth += 1
        elif ch in ")]":
            depth -= 1
            if depth < 0:
                return False
    return depth == 0


def split_comment(text, masked):
    """(code, trailing comment) of one line: the comment starts at the first // outside strings"""
    for i in range(len(text) - 1):
        if text[i:i + 2] == "//" and masked[i] == " ":
            return text[:i], text[i:]
    return text, ""


def prev_code_line(masked, pos):
    """the masked code of the closest non-blank line before offset pos"""
    i = masked.rfind("\n", 0, pos)
    while i > 0:
        j = masked.rfind("\n", 0, i)
        line = masked[j + 1:i].strip()
        if line and not line.startswith("#"):
            return line
        i = j
    return ""


LVALUE = re.compile(r"\s*([A-Za-z_]\w*)(?:\s*(?:\[[^\]]*\]|\.\s*[A-Za-z_]\w*))*\s*(?:[-+*/%&|^]|<<|>>)?=(?!=)")


def uninit_local(masked, pos, code):
    """the statement `code` (at pos) assigns to a local declared without an initializer earlier in the same function
    (`u32 target;` ... `target = f;`): dropping or moving it makes the mutant read an uninitialised variable, whose
    value depends on code generation, so only the mutant's own build can tell (gate finding 2026-10-26, lib2_sead_3
    #10/#20: killed on its own build, survived in the schema)"""
    m = LVALUE.match(code)
    if not m:
        return False
    name = m.group(1)
    start = masked.rfind("\n{", 0, pos)
    k = masked.rfind("\n", 0, pos)
    while k > 0:  # back to the function header: the closest earlier line that starts in column 0 and is code
        j = masked.rfind("\n", 0, k)
        line = masked[j + 1:k]
        if line[:1] not in ("", " ", "\t", "#") and line.strip():
            start = j
            break
        k = j
    body = masked[max(start, 0):pos]
    decl = re.compile(r"(?:[\w>\]*&]\s+|,\s*)%s\s*(?:\[[^\]]*\]\s*)*(?:;|,)" % re.escape(name))
    return decl.search(body) is not None


def braceless_body(masked, pos):
    """the line at pos may be the statement of a braceless if/for/while/else/do on the line before it (lib2_jsys_adapt
    #12: dropping it left `if (c) }` - the old mutant does not compile, or makes the next statement the body)"""
    p = prev_code_line(masked, pos)
    return p.endswith(")") or re.search(r"\b(else|do)$", p) is not None


class Parser:
    """token-level operand boundaries for &&, || and unary !"""

    def __init__(self, masked):
        self.toks = tokens(masked)
        self.at = {t[0]: i for i, t in enumerate(self.toks)}

    def idx(self, pos):
        return self.at.get(pos)

    def left_operand(self, i, stop_ops):
        toks = self.toks
        depth = 0
        j = i - 1
        while j >= 0:
            t = toks[j][2]
            if t in CLOSE:
                if t == "}" and depth == 0:
                    break
                depth += 1
            elif t in OPEN:
                if depth == 0:
                    break
                depth -= 1
            elif depth == 0 and (t in (";", ",", "?", ":", "#") or t in ASSIGN or t in stop_ops or
                                 t in ("return", "case", "throw", "else", "do")):
                break
            j -= 1
        if j + 1 >= i:
            return None
        return toks[j + 1][0], toks[i - 1][1]

    def right_operand(self, i, stop_ops):
        toks = self.toks
        depth = 0
        j = i + 1
        while j < len(toks):
            t = toks[j][2]
            if t in OPEN:
                depth += 1
            elif t in CLOSE:
                if depth == 0:
                    break
                depth -= 1
            elif depth == 0 and (t in (";", ",", "?", ":", "#") or t in ASSIGN or t in stop_ops):
                break
            j += 1
        if j <= i + 1 or depth:
            return None
        return toks[i + 1][0], toks[j - 1][1], j

    def template_bracket(self, i):
        """the < or > at token i looks like a template argument bracket (memberAt<u32>(...), gabi::at<be<u8>>(...)):
        the old mutant of it does not compile, and its schema rewrite breaks the parse around it"""
        toks, n = self.toks, len(self.toks)
        stop = (";", "{", "}", "&&", "||", "?", "#")
        if toks[i][2] == "<":
            if i == 0 or not re.match(r"[A-Za-z_]", toks[i - 1][2]):
                return False
            depth, k = 0, i
            while k < n and toks[k][2] not in stop:
                depth += {"<": 1, ">": -1, ">>": -2}.get(toks[k][2], 0)
                if toks[k][2] in ("(", ")") and depth == 1:
                    return False
                if depth <= 0:
                    return k + 1 < n and (toks[k + 1][2] in ("(", "::", "*", "&", ">", ",", ")", "{") or
                                          re.match(r"[A-Za-z_]", toks[k + 1][2]) is not None)
                k += 1
            return False
        depth, k = 0, i
        while k >= 0 and toks[k][2] not in stop:
            depth += {">": 1, ">>": 2, "<": -1}.get(toks[k][2], 0)
            if toks[k][2] in ("(", ")") and depth == 1:
                return False
            if depth <= 0:
                return k > 0 and re.match(r"[A-Za-z_]", toks[k - 1][2]) is not None
            k -= 1
        return False

    def group_end(self, j):
        """index of the token closing the bracket at token j"""
        depth = 0
        for k in range(j, len(self.toks)):
            t = self.toks[k][2]
            if t in OPEN:
                depth += 1
            elif t in CLOSE:
                depth -= 1
                if depth == 0:
                    return k
        return None

    def unary_operand(self, i):
        """(start, end, next token index) of the cast-expression after the unary ! at token i, or None"""
        toks = self.toks
        n = len(toks)
        j = i + 1
        while j < n and toks[j][2] in ("!", "~", "-", "+", "*", "&", "++", "--"):
            j += 1
        if j >= n:
            return None
        t = toks[j][2]
        if t == "(":
            k = self.group_end(j)
            if k is None:
                return None
            nxt = toks[k + 1][2] if k + 1 < n else ""
            if re.match(r"[A-Za-z_\d]", nxt) or nxt in ("(", "!", "~", "::"):  # a cast (T)x: its operand follows
                r = self.unary_operand(k)
                return None if r is None else (toks[i + 1][0], r[1], r[2])
            j = k + 1
        elif re.match(r"[A-Za-z_]", t) or t == "::":
            while True:  # qualified name, optional template arguments (only when ( or :: follows them)
                if j < n and toks[j][2] == "::":
                    j += 1
                if j >= n or not re.match(r"[A-Za-z_]", toks[j][2]):
                    return None
                j += 1
                if j < n and toks[j][2] == "<":
                    depth, k = 0, j
                    while k < n and toks[k][2] not in (";", "{", "}", "&&", "||"):
                        depth += {"<": 1, ">": -1, ">>": -2}.get(toks[k][2], 0)
                        if depth <= 0:
                            break
                        k += 1
                    if k < n and depth == 0 and k + 1 < n and toks[k + 1][2] in ("(", "::"):
                        j = k + 1
                if j < n and toks[j][2] == "::":
                    continue
                break
        elif re.match(r"[\d.]", t) or t[0] in "\"'":
            j += 1
        else:
            return None
        while j < n:  # postfix chain
            t = toks[j][2]
            if t in ("(", "["):
                k = self.group_end(j)
                if k is None:
                    return None
                j = k + 1
            elif t in (".", "->") and j + 1 < n and re.match(r"[A-Za-z_]", toks[j + 1][2]):
                j += 2
            elif t in ("++", "--"):
                j += 1
            else:
                break
        return toks[i + 1][0], toks[j - 1][1], j


CMP = {"==": "EQ", "!=": "NE", "<": "LT", "<=": "LE", ">": "GT", ">=": "GE"}
TERMINATORS = {")", ";", ",", "&&", "||", "?", ":", "]", "}"}


MARK_PREFIX = "marker only: "


def marker(k, cs, ce, masked, why):
    """a marker-only site for a statement the schema cannot rewrite (declarations, uninitialised locals): records
    when the statement runs (--reach-skip), never makes the mutant live - the mutant is built on its own. Placed as a
    statement of its own before the line; only where one can stand (not after a braceless if/else, not on case/label
    /else/} lines, no braces in the line; at namespace scope it does not compile and falls back)."""
    c = masked[cs:ce].strip()
    if not c.endswith(";") or "{" in c or "}" in c or re.match(r"^(else|case|default)\b|^[A-Za-z_]\w*\s*:(?!:)", c):
        return None, why
    return Site(k, "mark", cs, ce, [(cs, ce)], lambda sub, k=k: "(void)WWHD_MUT(%d); %s" % (k, sub[0])), MARK_PREFIX + why


def make_site(k, desc, x, old, new, t0, masked, parser):
    """the Site for mutant k (old -> new at offset x of the compiled text t0), or (None, reason)"""
    e = x + len(old)
    if masked[x:e] != t0[x:e] and not desc.startswith(("drop statement", "swap")):
        return None, "not code (comment, string or preprocessor line)"
    if "#" in masked[x:e]:
        return None, "preprocessor line"
    kind = desc.split(" @")[0]
    if kind.startswith("literal"):
        ta, tb = lit_type(old), lit_type(new)
        if ta is None or ta != tb:
            return None, "literal type changes (%s -> %s)" % (ta, tb)
        return Site(k, "literal", x, e, [], lambda sub, o=old, n=new, k=k: "(WWHD_MUT(%d) ? %s : %s)" % (k, n, o)), None
    if kind == "operator + -> -":
        return Site(k, "add", x, e, [], lambda sub, k=k: " + wwhd_mut::Op<%d, wwhd_mut::ADD>{} + " % k), None
    m = re.fullmatch(r"operator (==|!=|<=|>=|<|>)->(==|!=|<=|>=|<|>)", kind)
    if m:
        op = old
        i = parser.idx(x)
        if i is None or parser.toks[i][2] != op:
            return None, "operator token not found"
        if op in ("<", ">") and parser.template_bracket(i):
            return None, "template bracket"
        # a right operand spelled NULL / 0 / nullptr: keep the literal so pointer compares stay null-pointer compares
        tail = ""
        span_e = e
        toks = parser.toks
        if i + 2 < len(toks) and toks[i + 1][2] in ("NULL", "0", "nullptr") and toks[i + 2][2] in TERMINATORS:
            tail = {"NULL": "wwhd_mut::NullTok{}", "0": "wwhd_mut::ZeroTok{}", "nullptr": "wwhd_mut::NullptrTok{}"}[toks[i + 1][2]]
            span_e = toks[i + 1][1]
        if tail:
            return Site(k, "cmp", x, span_e, [], lambda sub, op=op, k=k, tail=tail, gap=t0[e:toks[i + 1][0]]:
                        "%s wwhd_mut::Op<%d, wwhd_mut::%s>{} %s%s%s" % (op, k, CMP[op], op, gap, tail)), None
        return Site(k, "cmp", x, e, [], lambda sub, op=op, k=k: "%s wwhd_mut::Op<%d, wwhd_mut::%s>{} %s" % (op, k, CMP[op], op)), None
    m = re.fullmatch(r"operator (&&|\|\|)->(&&|\|\|)", kind)
    if m:
        i = parser.idx(x)
        if i is None or parser.toks[i][2] != old:
            return None, "operator token not found"
        # The span is the logical-and-expression around the operator: from the closest `||` (or `?`, `=`, `,`, an
        # open bracket ...) on the left to the closest one on the right. && binds tighter than ||, so flipping one
        # regroups its neighbours: the textual mutant of the first && in `a && b && c` is `a || (b && c)`, of the
        # second || in `a || b || c` it is `a || (b && c)`. With L / R the text of the span left / right of the
        # operator, the original reads `... || (L) OLD (R) || ...` and the textual mutant `... || (L) NEW (R) || ...`
        # (&& and || are associative, with the same evaluation order and short-circuit), so the rewrite below runs
        # exactly what mutantN.cpp runs. Before 2026-10-28 the span was the old operator's operands (stopping at the next && for
        # &&, taking every || on the left for ||): the schema ran `(a || b) && c` (SEAD#16 #1039).
        lo = parser.left_operand(i, ("||",))
        ro = parser.right_operand(i, ("||",))
        if not lo or not ro:
            return None, "operand boundaries"
        ls, le = lo
        rs, re_, _ = ro
        return Site(k, "logic", ls, re_, [(ls, le), (rs, re_)],
                    lambda sub, k=k, o=old, n=new: "(WWHD_MUT(%d) ? ((%s) %s (%s)) : ((%s) %s (%s)))" % (
                        k, sub[0], n, sub[1], sub[0], o, sub[1])), None
    if kind == "drop !":
        i = parser.idx(x)
        if i is None or parser.toks[i][2] != "!":
            return None, "! token not found"
        r = parser.unary_operand(i)
        if not r:
            return None, "operand boundaries"
        os_, oe, nxt = r
        toks = parser.toks
        prev = toks[i - 1][2] if i else ""
        prev2 = toks[i - 2][2] if i > 1 else ""
        nt = toks[nxt][2] if nxt < len(toks) else ""
        boolean = (prev in ("&&", "||") and nt in TERMINATORS) or \
                  (prev == "(" and prev2 in ("if", "while") and nt in (")", "&&", "||")) or \
                  (nt in ("&&", "||") and prev in ("(", "&&", "||", "return", "=", ",", "?", ":"))
        if boolean:
            return Site(k, "not", x, oe, [(os_, oe)], lambda sub, k=k: "(WWHD_MUT(%d) ? !!(%s) : !(%s))" % (k, sub[0], sub[0])), None
        return Site(k, "not", x, oe, [(os_, oe)], lambda sub, k=k: "(WWHD_MUT(%d) ? (%s) : !(%s))" % (k, sub[0], sub[0])), None
    if kind.startswith("drop statement"):
        line = t0[x:e]
        if new != "" or "\n" in line:
            return None, "not one line"
        code, comment = split_comment(line, masked[x:e])
        ind = len(code) - len(code.lstrip())
        cs, ce = x + ind, x + len(code.rstrip())
        if braceless_body(masked, x):  # the old mutant leaves `if (c)` without its statement: another meaning or none
            return None, "braceless body"
        if not statement_ok(masked[x:x + len(code)]):
            return marker(k, cs, ce, masked, "statement shape")
        if uninit_local(masked, x + len(code) - len(code.lstrip()), code):
            return marker(k, cs, ce, masked, "assigns an uninitialised local")
        return Site(k, "drop", cs, ce, [(cs, ce)], lambda sub, k=k: "{ if (!WWHD_MUT(%d)) { %s } }" % (k, sub[0])), None
    if kind.startswith("swap"):
        # old = A + between + B, new = B + between + A (whole lines)
        lines = old.split("\n")
        if len(lines) < 2:
            return None, "swap shape"
        a_line, b_line = lines[0], lines[-1]
        between = old[len(a_line):len(old) - len(b_line)]
        if new != b_line + between + a_line or between.strip():
            return None, "swap shape"
        ma = masked[x:x + len(a_line)]
        mb = masked[e - len(b_line):e]
        ca, cma = split_comment(a_line, ma)
        cb, cmb = split_comment(b_line, mb)
        ia = len(ca) - len(ca.lstrip())
        ib = len(cb) - len(cb.lstrip())
        as_, ae = x + ia, x + len(ca.rstrip())
        if braceless_body(masked, x):  # A may be the body of a braceless if/for/while/else
            return None, "braceless body"
        if not statement_ok(ma[:len(ca)]) or not statement_ok(mb[:len(cb)]):
            return marker(k, as_, ae, masked, "statement shape")
        if uninit_local(masked, x, ca) or uninit_local(masked, e - len(b_line), cb):
            return marker(k, as_, ae, masked, "assigns an uninitialised local")
        bs, be_ = e - len(b_line) + ib, e - len(b_line) + len(cb.rstrip())
        # line 1: the whole construct (+ A's comment), then the original line break(s) and B's indentation; B's
        # comment stays where it is (after the span): same line count
        return Site(k, "swap", as_, be_, [(as_, ae), (bs, be_)],
                    lambda sub, k=k, cma=cma, between=between, ib=ib:
                    "{ if (WWHD_MUT(%d)) { %s %s } else { %s %s } }%s%s%s" % (
                        k, sub[1], sub[0], sub[0], sub[1], (" " + cma) if cma else "", between, " " * ib)), None
    return None, "kind %r has no schema rewrite" % kind


# ------------------------------------------------------------------ layering and rendering
def nests(inner, outer):
    """inner lies inside one of outer's rewritten sub-spans"""
    return any(a <= inner.s and inner.e <= b for a, b in outer.subs)


def compatible(a, b):
    if a.e <= b.s or b.e <= a.s:
        return True
    if a.s <= b.s and b.e <= a.e and (a.s, a.e) != (b.s, b.e):
        return nests(b, a)
    if b.s <= a.s and a.e <= b.e and (a.s, a.e) != (b.s, b.e):
        return nests(a, b)
    return False


def layers(sites):
    """greedy: each site into the first layer where it is compatible with every site already there"""
    out = []
    for st in sorted(sites, key=lambda s: (s.s, -s.e)):
        for L in out:
            if all(compatible(st, o) for o in L):
                L.append(st)
                break
        else:
            out.append([st])
    return out


def render(text, sites):
    """text with the (compatible) sites rewritten: (new text, {k: [(start, end), ...]} where each rendered copy of
    site k lies in the new text; a site inside a duplicated operand has two copies)"""
    sites = sorted(sites, key=lambda s: (s.s, -s.e))

    def rec(lo, hi, cand):
        """(text[lo:hi] with the sites of cand inside it rewritten, {k: [(s, e)]} relative to that result)"""
        cand = [s for s in cand if lo <= s.s and s.e <= hi]
        top = []
        for s in cand:  # outermost sites (sorted by start, longest first)
            if top and s.s < top[-1].e:
                continue
            top.append(s)
        out, spans, cur, pos = [], {}, 0, lo
        for s in top:
            out.append(text[pos:s.s])
            cur += s.s - pos
            inner = [c for c in cand if c is not s and s.s <= c.s and c.e <= s.e]
            subs = [rec(a, b, inner) for a, b in s.subs]
            r = s.render([st for st, _ in subs])
            spans.setdefault(s.k, []).append((cur, cur + len(r)))
            for st, sp in subs:  # locate each copy of the operand text in the rendering
                at = 0
                while st:
                    f = r.find(st, at)
                    if f < 0:
                        break
                    for k, lst in sp.items():
                        spans.setdefault(k, []).extend((cur + f + a, cur + f + b) for a, b in lst)
                    at = f + len(st)
            out.append(r)
            cur += len(r)
            pos = s.e
        out.append(text[pos:hi])
        return "".join(out), spans

    return rec(0, len(text), sites)


def blame(diag, path, newtext, spans):
    """the mutant ids whose rendered span is the innermost one containing a position that compiler output diag names
    in file path (errors, and notes such as "in instantiation ... requested here")"""
    starts = [0]
    for i, ch in enumerate(newtext):
        if ch == "\n":
            starts.append(i + 1)
    hit = set()
    base = os.path.basename(path)
    for m in re.finditer(r"([^\s:]+):(\d+):(\d+):", diag):
        if os.path.basename(m.group(1)) != base:
            continue
        ln, col = int(m.group(2)), int(m.group(3))
        if ln - 1 >= len(starts):
            continue
        pos = starts[ln - 1] + col - 1
        best, size = None, None
        for k, lst in spans.items():
            for a, b in lst:
                if a <= pos < b and (size is None or b - a < size):
                    best, size = k, b - a
        if best is None:  # a follow-on error after a site on the same line: the last site that starts before it
            line_start = starts[ln - 1]
            near = None
            for k, lst in spans.items():
                for a, b in lst:
                    if line_start <= a <= pos and (near is None or a > near):
                        best, near = k, a
        if best is not None:
            hit.add(best)
    return hit


# ------------------------------------------------------------------ building
def build(src, text, entries, transform, compile_obj, workdir, tag, jobs=1):
    """Schema objects for the mutants `entries` [(k, desc, s, e, new)] of source `src` (text = its current text).

    transform(text) -> the compiled text (autoglue or `#line 1` + text; may raise); compile_obj(cpp, obj) ->
    (returncode, stderr). Returns (objects, fallback): objects = [(obj path, [k, ...])], fallback = {k: reason} for
    the mutants left to the per-mutant build. A mutant whose reason starts with MARK_PREFIX is in an object as well,
    as a marker only (its reach can be recorded; it runs on its own build)."""
    import concurrent.futures
    fallback = {}
    t0 = transform(text)
    masked = code_mask(t0)
    parser = Parser(masked)
    sites = []
    for k, desc, s, e, new in entries:
        mt = text[:s] + new + text[e:]
        try:
            tk = transform(mt)
        except Exception as ex:  # the per-mutant build reports it (stage glue)
            fallback[k] = "transform: %s" % ex
            continue
        x = map_site(t0, tk, text[s:e], new)
        if x is None:
            fallback[k] = "compiled text differs from the original beyond the site"
            continue
        site, why = make_site(k, desc, x, text[s:e], new, t0, masked, parser)
        if site is None:
            fallback[k] = why
            continue
        if why:  # marker only: in the build for the reach recording, built on its own to run
            fallback[k] = why
        sites.append(site)
    queue = layers(sites)
    objects = []
    n = [0]

    def attempt(group):
        """compile one group; returns (obj, ks) or a list of groups to try instead (blamed sites removed / halves)"""
        n[0] += 1
        base = os.path.join(workdir, "%s-%d" % (tag, n[0]))
        newtext, spans = render(t0, group)
        with open(base + ".cpp", "w") as fh:
            fh.write(newtext)
        rc, err = compile_obj(base + ".cpp", base + ".o")
        if rc == 0:
            return base + ".o", [s.k for s in group]
        bad = blame(err, base + ".cpp", newtext, spans)
        if bad:
            for k in bad:
                fallback[k] = "schema compile error"  # also ends a marker-only site
            rest = [s for s in group if s.k not in bad]
            return [rest] if rest else []
        if len(group) == 1:
            fallback[group[0].k] = "schema compile error (unattributed)"  # also ends a marker-only site
            return []
        h = len(group) // 2
        return [group[:h], group[h:]]

    with concurrent.futures.ThreadPoolExecutor(max(1, jobs)) as ex:
        pending = [ex.submit(attempt, g) for g in queue if g]
        while pending:
            done, _ = concurrent.futures.wait(pending, return_when=concurrent.futures.FIRST_COMPLETED)
            pending = [p for p in pending if p not in done]
            for d in done:
                r = d.result()
                if isinstance(r, tuple):
                    objects.append(r)
                else:
                    pending += [ex.submit(attempt, g) for g in r if g]
    return objects, fallback

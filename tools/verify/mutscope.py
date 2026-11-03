#!/usr/bin/env python3
"""Per-mutant test scope for mutate.py (2026-10-27): which functions under test can see a mutant at all.

How a unit runs (tools/verify/README.md, "The original is the game's recompiled code"): the harness calls ONE candidate
function per row at top level. Every marked candidate function starts with the nested-call check (WWHD_FUNC /
WWHD_GLUE_FUNC: `if (gabi::Activation::nested()) return gabi::call<R>(addr, ...)`), so when candidate F calls candidate
G, G's body does not run: the call goes to the harness by address and is answered by a mock, a recording, or the
ORIGINAL's code (`real ADDR` links real_ADDR, generated from build/gen by mkunit.py; setups/postsetups run such
originals too). Inputs are independent (each starts from clear_all(), seeded by function and index; -shards/-runlist
rely on the same). So the mutated text of a marked function's body after that check runs only in its own row.

Code without that check is shared: file-local helpers (and any unmarked function), a glue constructor's body and
init list (autoglue keeps X::X() a C++ constructor and wraps it), code in front of a WWHD_FUNC statement, class bodies,
file-scope initialisers. Host (non-guest) statics outlive an input and a row.

affected(src, s, e) returns (set of addresses, reason) or (None, reason) = run the whole campaign scope:
  * span inside the checked body of marked F                              -> {F}
  * span inside a helper H (an unmarked function definition)              -> every marked function whose code names H,
                                                                             directly or through other helpers
  * F/H named in file-scope code (static initialisers, host tables,
    macros, class bodies; glue `data` views and VERIFY lines excluded)    -> whole unit (may run at start-up or anywhere)
  * the code reads/writes a host static (file-scope variable without a
    glue `data` line, not const)                                          -> + every function naming that static
  * a helper declared/used in an included header more than once,
    an operator/constructor/destructor helper, a constructor body,
    code before WWHD_FUNC, anything not inside a function definition      -> whole unit
  * a function of the set is listed `real` or is a setup/postsetup target
    in the spec (its ORIGINAL runs in other rows; candidate code does
    not, but kept conservative)                                           -> whole unit
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import autoglue  # noqa: E402

NOT_FUNCS = {"if", "for", "while", "switch", "catch", "return", "sizeof", "alignof", "decltype", "static_assert"}
WFUNC = re.compile(r"\bWWHD_FUNC\(\s*0x([0-9A-Fa-f]{8})")


def _prefix_len(a, b):
    """length of the common prefix of a and b (binary search on slice compares)"""
    lo, hi = 0, min(len(a), len(b))
    while lo < hi:
        mid = (lo + hi + 1) // 2
        if a[:mid] == b[:mid]:
            lo = mid
        else:
            hi = mid - 1
    return lo


def changed_span(base, mutant):
    """(s, e, e2): the smallest span base[s:e] that mutant replaces (by mutant[s:e2]); None when equal"""
    if base == mutant:
        return None
    s = _prefix_len(base, mutant)
    k = _prefix_len(base[s:][::-1], mutant[s:][::-1])
    return s, len(base) - k, len(mutant) - k


def _enclosing_open(code, pos):
    """index of the `{` that encloses pos (None at file scope)"""
    depth = 0
    for j in range(pos - 1, -1, -1):
        c = code[j]
        if c == "}":
            depth += 1
        elif c == "{":
            if depth == 0:
                return j
            depth -= 1
    return None


class SourceIndex:
    """top-level items of one candidate source: function definitions (marked or helper) and file-scope code"""

    def __init__(self, src, text):
        self.src = src
        self.text = text
        self.code = autoglue.mask(text)
        g = autoglue.parse_glue(text)
        self.glue = g is not None
        self.limit = g[3] if g else len(text)
        self.datas = set(g[1]) if g else set()
        self.funcs = []   # dicts: start, end (exclusive), name (simple, or None), qual, marked addr|None, guard (s, e)|None
        self._parse()
        self._marks()
        fs = list(self.code[:self.limit])
        for f in self.funcs:  # file-scope code: everything outside function definitions
            for k in range(f["start"], f["end"]):
                if fs[k] != "\n":
                    fs[k] = " "
        filescope = "".join(fs)
        filescope = re.sub(r"\bVERIFY(?:_NAMED)?\s*\([^;]*\);", lambda m: " " * len(m.group(0)), filescope)
        # preprocessor lines other than #define (a macro naming a helper is a use)
        filescope = re.sub(r"(?m)^[ \t]*#(?![ \t]*define\b).*$", lambda m: " " * len(m.group(0)), filescope)
        stmts = []
        for st in re.split(r"(?<=[;}])", filescope):
            m = re.match(r"\s*static\s+[^=;{(]*?\b(\w+)\s*(\[[^\]]*\]\s*)*(=|;|\{)", st)
            if m and m.group(1) in self.datas:
                continue  # a glue `data` view: its initialiser is compared with the image at build time, never run
            stmts.append(st)
        self.filescope = "".join(stmts)
        # host statics: namespace-scope variable definitions (not const/constexpr, not a glue data view, not extern);
        # members inside class bodies do not count
        self.host_statics = set()
        stack, cur = [], []
        for ch in self.filescope:
            if ch == "{":
                head = "".join(cur)
                stack.append("ns" if re.search(r'(\bnamespace\b[\w\s:]*|\bextern\s+"x*"\s*)$', head) else "other")
                cur = []
            elif ch == "}":
                if stack:
                    stack.pop()
                cur = []
            elif ch == ";":
                if all(k == "ns" for k in stack):
                    self._host_static("".join(cur) + ";")
                cur = []
            else:
                cur.append(ch)

    def _host_static(self, st):
        s = " ".join(st.split())
        if not s or re.match(r"^(using|typedef|namespace|template|extern|struct|class|union|enum|#|return|friend)\b", s):
            return
        if re.search(r"\b(const|constexpr|constinit)\b", s.split("=")[0]):
            return
        m = re.match(r"^(?:static\s+|inline\s+|thread_local\s+)*[A-Za-z_][\w:<>,\s*&]*?[\s*&]((?:\w+::)*\w+)\s*(\[[^\]]*\]\s*)*"
                     r"(=[^;]*)?;$", s)
        if m and "(" not in s.split("=")[0] and m.group(1).split("::")[-1] not in self.datas:
            self.host_statics.add(m.group(1).split("::")[-1])

    def _parse(self):
        limit = self.limit
        # directives are not part of a definition's header (a #define body continued with \ stays out as well)
        code = re.sub(r"(?m)^[ \t]*#(?:.*\\\n)*.*$", lambda m: re.sub(r"[^\n]", " ", m.group(0)), self.code)
        stack = []
        last = 0
        i = 0
        while i < limit:
            c = code[i]
            if c == ";":
                last = i + 1
            elif c == "}":
                if stack:
                    stack.pop()
                last = i + 1
            elif c == "{":
                header = code[last:i]
                if re.search(r"\bnamespace\b[\w\s:]*$", header) or re.search(r'\bextern\s+"x*"\s*$', header):
                    stack.append("ns")
                elif all(k == "ns" for k in stack) and "=" not in header.split("(")[0] and "(" in header:
                    close = autoglue.match_close(code, i, "{", "}")
                    hs = last + len(header) - len(header.lstrip())
                    lines = [ln for ln in header.split("\n") if ln.strip()]
                    name = qual = None
                    initlist = re.search(r"\)\s*:(?!:)", header) is not None
                    m = re.search(r"((?:~?\w+::)*~?[A-Za-z_]\w*)\s*\([^;{}]*\)\s*(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?$",
                                  header)
                    if m and not initlist and lines and not lines[0].lstrip().startswith("#"):
                        qual = m.group(1)
                        name = qual.split("::")[-1]
                        if name in NOT_FUNCS:
                            name = None
                    if initlist:  # X::X(...) : a(1), b{2} {: a constructor with an init list
                        mm = re.search(r"((?:\w+::)*~?\w+)\s*\(", header)
                        qual = mm.group(1) if mm else None
                    self.funcs.append({"start": hs, "end": close + 1, "brace": i, "name": name, "qual": qual,
                                       "initlist": initlist, "marked": None, "guard": None, "ctor": False})
                    i = close + 1
                    last = i
                    continue
                else:
                    stack.append("other")
                last = i + 1
            i += 1

    def _marks(self):
        code, text = self.code, self.text
        byb = {f["brace"]: f for f in self.funcs}
        if self.glue:
            for m in autoglue.MARKER.finditer(text):
                if m.start() >= self.limit:
                    break
                par = code.find("(", m.end())
                if par < 0:
                    continue
                close = autoglue.match_close(code, par, "(", ")")
                brace = code.find("{", close)
                f = byb.get(brace)
                if f is None:
                    continue
                f["marked"] = int(m.group(1), 16)
                parts = (f["qual"] or "").split("::")
                f["ctor"] = f["initlist"] or (len(parts) > 1 and parts[-1] == parts[-2])
                if not f["ctor"]:  # autoglue puts WWHD_GLUE_FUNC right after the `{`: the whole body is checked
                    f["guard"] = (brace + 1, f["end"] - 1)
        for m in WFUNC.finditer(code[:self.limit]):
            ob = _enclosing_open(code, m.start())
            f = next((f for f in self.funcs if f["start"] <= m.start() < f["end"]), None)
            if f is None or ob is None:
                continue
            f["marked"] = int(m.group(1), 16)
            parts = (f["qual"] or "").split("::")
            f["ctor"] = f["initlist"] or (len(parts) > 1 and parts[-1] == parts[-2])
            if ob == f["brace"] and not f["ctor"]:
                semi = code.find(";", m.end())
                f["guard"] = (semi + 1, f["end"] - 1)  # code in front of WWHD_FUNC(...); runs nested too

    def item_at(self, s, e):
        """the function definition holding [s, e) entirely, else None"""
        for f in self.funcs:
            if f["start"] <= s and e <= f["end"]:
                return f
        return None

    def body(self, f):
        return self.code[f["start"]:f["end"]]


class Scope:
    """affected sets over all candidate sources of a unit"""

    def __init__(self, srcs, spec_lines=(), root=ROOT):
        self.idx = {}
        for src in srcs:
            if src not in self.idx:
                self.idx[src] = SourceIndex(src, open(os.path.join(root, src)).read())
        self.fixed = set()  # addresses whose original runs in other rows (real / setup / postsetup targets)
        for line in spec_lines:
            f = line.split("#")[0].split()
            if not f:
                continue
            if f[0] == "real":
                self.fixed.update(int(x, 16) for x in f[1:] if re.match(r"^[0-9A-Fa-f]{8}$", x))
            elif f[0] in ("setup", "postsetup") and len(f) >= 3 and re.match(r"^[0-9A-Fa-f]{8}$", f[2]):
                self.fixed.add(int(f[2], 16))
        self.host_statics = set()
        for si in self.idx.values():
            self.host_statics |= si.host_statics
        self._headers = {}
        self._closures = {}

    def _mentions(self, name):
        pat = re.compile(r"\b%s\b" % re.escape(name))
        return [(si, f) for si in self.idx.values() for f in si.funcs if pat.search(si.body(f))], pat

    def _header_uses(self, src, name):
        """occurrences of name in the headers src includes (masked; once per source)"""
        if src not in self._headers:
            si = self.idx[src]
            self._headers[src] = "\n".join(autoglue.mask(t) for _, t in autoglue.include_closure(si.text, src))
        return len(re.findall(r"\b%s\b" % re.escape(name), self._headers[src]))

    def filescope_use(self, si, name):
        """name used in executable file-scope code of si (initialisers, inline bodies, macros, host tables); member and
        function declarations (`R name(...);`, `friend ...;`) do not count"""
        pat = re.compile(r"\b%s\b" % re.escape(name))
        fs = si.filescope
        for m in pat.finditer(fs):
            a = max(fs.rfind(";", 0, m.start()), fs.rfind("{", 0, m.start()), fs.rfind("}", 0, m.start())) + 1
            b = fs.find(";", m.end())
            st = " ".join(fs[a:b + 1 if b >= 0 else len(fs)].split())
            if re.match(r"^(?:(?:public|private|protected)\s*:\s*)*(?:friend\s+)?[^=]*\b%s\s*\([^;]*\)\s*(?:const|override|noexcept|final|\s)*"
                        r"(?:=\s*(?:0|default|delete)\s*)?;$" % re.escape(name), st):
                continue
            if re.match(r"^(?:static\s+|inline\s+|thread_local\s+)*[A-Za-z_][\w:<>,\s*&]*?[\s*&](?:\w+::)*%s\s*(?:\[[^\]]*\]\s*)*"
                        r"(?:=.*)?;$" % re.escape(name), st):
                continue  # the definition of a variable of that name (its own initialiser is not a use of it)
            return True
        return False

    def closure(self, names):
        key = tuple(sorted(names))
        if key not in self._closures:
            self._closures[key] = self._closure(names)
        out, problems = self._closures[key]
        return set(out), list(problems)

    def _closure(self, names):
        """(marked functions naming any of names, directly or through helpers; problems): problems name uses that
        cannot be bounded (file-scope code, headers, operators, constructors)"""
        out, problems, seen = set(), [], set()
        todo = list(names)
        while todo:
            name = todo.pop()
            if name in seen:
                continue
            seen.add(name)
            if name.startswith("operator") or name.startswith("~"):
                problems.append("operator/destructor %s (implicit calls)" % name)
            pat = re.compile(r"\b%s\b" % re.escape(name))
            for si in self.idx.values():
                if self.filescope_use(si, name):
                    problems.append("%s named in file-scope code of %s" % (name, os.path.basename(si.src)))
                for f in si.funcs:
                    if f["name"] == name and f["marked"] is None and not f["ctor"]:
                        if self._header_uses(si.src, name) > 1:
                            problems.append("%s used in an included header" % name)
                        continue  # its own definition
                    if not pat.search(si.body(f)):
                        continue
                    if f["marked"] is not None:
                        out.add(f["marked"])
                        if f["ctor"]:
                            problems.append("constructor %s names %s (constructors run without the nested check)" % (
                                f["qual"], name))
                    elif f["name"] is None or f["ctor"]:
                        problems.append("%s used in an unnamed/constructor definition in %s" % (name, os.path.basename(si.src)))
                    else:
                        todo.append(f["name"])
        return out, problems

    def users(self, start_items, start_names):
        out, problems = self.closure(start_names)
        if problems:
            return None, problems[0]
        return out, None

    def statics_of(self, items):
        names = set()
        for si, f in items:
            body = si.body(f)
            for h in self.host_statics:
                if re.search(r"\b%s\b" % re.escape(h), body):
                    names.add(h)
        return names

    def affected(self, src, s, e):
        """(set of addresses, None) or (None, reason): the functions under test whose rows can run base[s:e]"""
        si = self.idx.get(src)
        if si is None:
            return None, "source not indexed"
        if e < s:
            e = s
        f = si.item_at(s, max(e, s + 1) if e == s else e)
        if f is None:
            return None, "change outside any function definition"
        checked = False
        if f["marked"] is not None and f["guard"] and f["guard"][0] <= s and e <= f["guard"][1] + 1:
            checked = True
            items = [(si, f)]
            out = {f["marked"]}
            # F itself running outside a row: named in file-scope code or through helpers called there
            if f["name"]:
                _, problems = self.closure([f["name"]])
                start = [p for p in problems if "file-scope" in p]
                if start:
                    return None, start[0]
        elif f["marked"] is not None:
            return None, "%s of %08X runs without the nested check" % (
                "constructor" if f["ctor"] else "code before WWHD_FUNC", f["marked"])
        else:
            if not f["name"] or f["ctor"]:
                return None, "unnamed or constructor helper definition"
            items = [(si, f)]
            out, why = self.users([], [f["name"]])
            if out is None:
                return None, why
        st = self.statics_of(items)
        if st:  # host statics the changed code touches: every row naming them can read what it left
            more, why = self.users([], sorted(st))
            if more is None:
                return None, why
            out |= more
        # real/setup/postsetup targets run the ORIGINAL (real_ADDR) wherever they run outside their own row, and the
        # candidate's checked body runs only in its own row (the WWHD_FUNC nested-call check dispatches every other call),
        # so a change inside F's checked body touches F's row only, even when F is also listed real/setup (,
        # 2026-10-30; before: whole scope). Helpers, and checked bodies whose host statics widen the set to other
        # functions, keep the conservative fallback.
        hit = (out - {f["marked"]} if checked else out) & self.fixed
        if hit:
            return None, "%s listed real/setup in the spec" % " ".join("%08X" % x for x in sorted(hit))
        if not out:
            return None, "no marked user found"
        return out, None

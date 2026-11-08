#!/usr/bin/env python3
"""Exact declarations of guest calls between decompiled functions (lint, 2026-11-03).

A decompiled function calls another function through a guest call (an `ext` glue line's stub, a direct gabi::call,
a header wrapper): the call puts the declared arguments into r3../f1.. and leaves every other register as it was. A
call that declares fewer arguments than the callee reads still passes the harness when the missing register happens to
hold the right value (the caller's own entry value passed on, or an earlier call's result), but native calls in the
port's decomp build can only pass plain C++ arguments when every declaration is exact.

The lint compiles every unit source with -DGABI_DECL_AUDIT (gabi.h marks the guest calls deprecated, so clang reports
each call site with its argument types; nothing is built), compares each call with the callee's definition (its entry
glue as compiled) and with the original's live-in registers (funcdb Dataflow), and classifies a missing register by the
original caller's dataflow (see ERROR_CLASSES / TOLERATED below). Definitions whose original reads an argument register
they do not take are listed too (known live-in over-approximations: decl_exceptions.tsv).

usage: decl_lint.py [-j N] [--cache DIR] [--out TSV] [--handoff TSV]
       decl_lint.py --gen DIR --image FILE --rpx FILE --runtime DIR --tools DIR --out-dir DIR --handoff TSV  (port mode)
Needs build/gen (the recompiled original, see README step 4), build/verify/livein.tsv and the game image (autoglue).
Port mode (the port's decomp build makes the hand-off table at build time from the player's own translation): those
inputs from the given places (funcdb reads the port's register-local translation as the plain form), the live-in table
made from --gen into --out-dir, caches there too; nothing is written into this tree.
Exit 0: no under-declared call; 1: under-declared calls (listed); 2: a source did not compile.
"""
import argparse
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
# inputs outside the sources; the port's decomp build points them at its own build (--gen, --image, --rpx, --runtime,
# --tools, --out-dir), the defaults are this repository's build/ as before
PATHS = {"gen": os.path.join(ROOT, "build", "gen"), "livein": os.path.join(ROOT, "build", "verify", "livein.tsv"),
         "rpx": os.path.join(ROOT, "game", "code", "cking.rpx"), "runtime": os.path.join(ROOT, "runtime"),
         "tools": os.path.join(ROOT, "tools")}
sys.path.insert(0, HERE)
import autoglue  # noqa: E402
import specfile  # noqa: E402

EXT_LINE = re.compile(r"^ext\s+([0-9A-Fa-f]{8})\s+(.*?)\s*(?:#.*)?$")


def unit_sources():
    """gated unit specs -> {source: [unit names]}"""
    udir = os.path.join(HERE, "units")
    out = collections.defaultdict(list)
    for name in specfile.gated_specs(udir):
        for line in specfile.expanded_lines(os.path.join(udir, name + ".txt")):
            t = line.split("#")[0].split()
            if t and t[0] == "src":
                for s in t[1:]:
                    if s.startswith("wwhd_src/") and s.endswith(".cpp") and name not in out[s]:
                        out[s].append(name)
    return out


def reg_class(ptype):
    """'r' (integer register) or 'f' (floating-point register) for a parameter type"""
    t = re.sub(r"\b(const|volatile|struct|class|enum)\b", " ", ptype)
    if "*" in t or "&" in t or "[" in t:
        return "r"
    return "f" if t.split() and t.split()[-1] in autoglue.FLOAT_SCALARS else "r"


def param_types(params):
    out = []
    for p in autoglue.split_top(params):
        ps = re.sub(r"/\*.*?\*/", "", p).strip().split("=")[0].strip()
        if ps in ("", "void"):
            continue
        if ps == "...":
            out.append("...")
            continue
        nm = re.match(r"^(.*[\s*&])([A-Za-z_]\w*)$", ps)
        out.append(nm.group(1).strip() if nm and nm.group(1).strip() and nm.group(2) not in autoglue.SCALARS else ps)
    return out


def regs_of(this, ptypes):
    """(int registers, float registers) a declaration passes; None for a variadic one"""
    if "..." in ptypes:
        return None
    gi = 1 if this else 0
    fi = 0
    for p in ptypes:
        if reg_class(p) == "f":
            fi += 1
        else:
            gi += 1
    return gi, fi


def mask_of(regs):
    gi, fi = regs
    return frozenset([("r", 3 + i) for i in range(min(gi, 8))] + [("f", 1 + i) for i in range(min(fi, 8))])


Def = collections.namedtuple("Def", "addr file qname kind ret ptypes regs line end")
Ext = collections.namedtuple("Ext", "addr file qname member ret ptypes regs line text")


def parse_source(rel):
    """-> ([Def], [Ext]) of one source with a glue block"""
    with open(os.path.join(ROOT, rel), encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    m = re.search(r"/\* WWHD glue(.*?)\*/", text, re.S)
    if not m:
        return [], []
    glue_pos = m.start()
    code = autoglue.mask(text)
    defs, exts = [], []
    nss = None
    for mk in autoglue.MARKER.finditer(text):
        if mk.start() >= glue_pos:
            break
        addr = int(mk.group(1), 16)
        brace = code.find("{", mk.end())
        par = code.find("(", mk.end())
        if par < 0 or par > brace:
            continue
        close = autoglue.match_close(code, par, "(", ")")
        brace = code.find("{", close)
        header = re.sub(r"\s+", " ", code[mk.end():brace]).strip()
        hm = re.match(r"^((?:static |inline )*)(.*?)\s*((?:\w+::)*~?\w+)\s*\((.*)\)\s*(const)?$", header)
        if not hm:
            continue
        end = autoglue.line_of(text, autoglue.match_close(code, brace, "{", "}"))
        ret, qname = hm.group(2).strip(), hm.group(3)
        parts = qname.split("::")
        cls = "::".join(parts[:-1]) if len(parts) > 1 else None
        kind = "ctor" if cls and parts[-1] == parts[-2] else ("method" if cls else "free")
        if nss is None:
            nss = autoglue.namespace_names(text, rel)
        if kind == "method" and cls in nss:
            kind = "free"
        if kind == "method" and parts[-1] in autoglue.static_members(cls, text, rel):
            kind = "static"
        ptypes = param_types(text[par + 1:close])
        if kind == "ctor":  # the guest function is the wrapper `cls__ct(cls* self)`
            regs, ret, ptypes = (1, 0), cls + "*", []
        else:
            regs = regs_of(kind == "method", ptypes)
        defs.append(Def(addr, rel, qname, kind, ret, tuple(ptypes), regs, autoglue.line_of(text, mk.start()), end))
    gl = text[m.start():m.end()].split("\n")
    base = autoglue.line_of(text, m.start())
    for i, raw in enumerate(gl[1:], 1):
        em = EXT_LINE.match(raw.strip())
        if not em:
            continue
        sig = em.group(2)
        cb = re.match(r"^(.*\))\s*(const\s*)?callback$", sig)
        if cb:
            sig = cb.group(1) + (" const" if cb.group(2) else "")
        hm = re.match(r"^(.*?)\s*((?:\w+::)*~?\w+)\s*\((.*)\)\s*(const)?$", sig)
        if not hm:
            continue
        ret, qname, params = hm.group(1).strip(), hm.group(2), hm.group(3)
        owner = qname.rsplit("::", 1)[0] if "::" in qname else None
        if nss is None:
            nss = autoglue.namespace_names(text, rel)
        member = owner is not None and qname.split("::")[-1] not in autoglue.static_members(owner, text, rel) \
            and owner not in nss
        ptypes = param_types(params)
        exts.append(Ext(int(em.group(1), 16), rel, qname, member, ret, tuple(ptypes), regs_of(member, ptypes),
                        base + i, raw.strip()))
    return defs, exts


def read_livein(path):
    li = {}
    if os.path.isfile(path):
        for ln in open(path):
            a, im, fm = ln.split()
            im, fm = int(im, 16), int(fm, 16)
            li[int(a, 16)] = frozenset([("r", r) for r in range(3, 11) if im >> r & 1] +
                                       [("f", r) for r in range(1, 9) if fm >> r & 1])
    return li


def collect(sources):
    defs, exts = collections.defaultdict(list), []
    for s in sorted(sources):
        d, e = parse_source(s)
        for x in d:
            defs[x.addr].append(x)
        exts += e
    return defs, exts


def fmt_regs(regs):
    return "var" if regs is None else "%dr%df" % regs


class Flow:
    """the original's code (build/gen): which argument registers still hold the function's entry value at a call"""

    def __init__(self, gen_dir):
        from funcdb import GenIndex, Dataflow
        self.gen = GenIndex(gen_dir, cache=PATHS.get("index"))
        self.df = Dataflow(self.gen, unknown_reads_args=False)
        self.memo, self.ind_memo = {}, {}

    def sites(self, f):
        """{target: [(entry-held must set, entry-held may set)]} for every reachable direct call of f"""
        if f in self.memo:
            return self.memo[f]
        from funcdb import ARGS, VOLATILE
        out = collections.defaultdict(list)
        if f not in self.gen.loc:
            self.memo[f], self.ind_memo[f] = out, []
            return out
        g = self.df.cfg(f)
        n = len(g.insns)
        kill = []
        for k in range(n):
            d = set(g.defs[k])
            for t in g.calls[k]:
                d |= self.df.maydef(t)
            if g.indirect[k]:
                d |= VOLATILE
            kill.append(d)
        preds = [[] for _ in range(n)]
        for k in range(n):
            for j in g.succ[k]:
                preds[j].append(k)
        top = frozenset(ARGS)
        must = [top] * n
        may = [frozenset()] * n
        if n:
            may[0] = top
        changed = True
        while changed:
            changed = False
            for k in sorted(g.reach):
                if k == 0:
                    im, iy = top, top
                else:
                    ps = [p for p in preds[k] if p in g.reach]
                    im = frozenset.intersection(*[must[p] - kill[p] for p in ps]) if ps else frozenset()
                    iy = frozenset().union(*[may[p] - kill[p] for p in ps]) if ps else frozenset()
                    if k in (0,):
                        pass
                if im != must[k] or iy != may[k]:
                    must[k], may[k] = im, iy
                    changed = True
        for k in g.reach:
            for t in g.calls[k]:
                own = set(g.defs[k])
                out[t].append((must[k] - own, may[k] - own))
        ind = []
        for k in sorted(g.reach):
            if g.indirect[k] and not g.insns[k][1].startswith("switch"):  # a jump table is no call
                im = re.search(r"\b(imp_\w+)\(c\)", g.insns[k][1])
                name = im.group(1) if im else None
                if name is None:  # a call through a data import (MEMFreeToDefaultHeap): its loading instruction's reloc
                    di = data_imports()
                    for j in range(k - 1, max(-1, k - 10), -1):
                        a0 = g.insns[j][0]
                        if a0 in di:
                            name = di[a0]
                            break
                ind.append((g.insns[k][0], name, may[k] - set(g.defs[k])))
        self.ind_memo[f] = ind
        self.memo[f] = out
        return out

    def indirect_held(self, f):
        """[(address, import name or None, entry-held may set)] of f's indirect calls (bctr/bctrl, imports)"""
        if f not in self.ind_memo:
            self.sites(f)
        return self.ind_memo.get(f, [])


def regs_text(m):
    return ",".join("%s%d" % r for r in sorted(m)) or "-"


# ---------------------------------------------------------------- compile mode: every guest call's argument types

WARN = re.compile(r"^(.*?):(\d+):(\d+): warning: '(call|call_ptr|tail_ptr)<(.*)>' is deprecated: gabi-decl-audit")
NOTE_REQ = re.compile(r"^(.*?):(\d+):(\d+): note: in instantiation of .* requested here")
CFLAGS = ["-std=c++20", "-fsyntax-only", "-DGABI_DECL_AUDIT", "-Wno-everything", "-Wdeprecated-declarations",
          "-ferror-limit=0", "-fno-strict-aliasing", "-fwrapv", "-DGABI_GUEST_GUARD=1",
          "-Iruntime/include", "-Itools/verify/include", "-Itools/verify/src", "-Iwwhd_src/include", "-Ibuild/gen"]


def compile_flags():
    """CFLAGS with the port mode's runtime include and gen directories (identical to CFLAGS by default)"""
    f = list(CFLAGS)
    if PATHS["runtime"] != os.path.join(ROOT, "runtime"):
        f[f.index("-Iruntime/include")] = "-I" + os.path.join(PATHS["runtime"], "include")
    if PATHS["gen"] != os.path.join(ROOT, "build", "gen"):
        f[f.index("-Ibuild/gen")] = "-I" + PATHS["gen"]
    return f


def split_targs(s):
    out, depth, cur = [], 0, ""
    for ch in s:
        if ch in "<([":
            depth += 1
        elif ch in ">)]":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def type_class(t):
    t = re.sub(r"\b(const|volatile)\b", "", t).strip()
    return "f" if t in ("float", "double") else "r"


def glued_text(rel):
    """the source as the harness compiles it (autoglue), #line directives blanked (line numbers stay: glued line
    N+1 is source line N; the registrations follow the source)"""
    with open(os.path.join(ROOT, rel), encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    if "/* WWHD glue" in text:
        text = autoglue.transform(text, rel)
        text = re.sub(r"^#line .*$", "", text, flags=re.M)
    else:
        text = "\n" + text
    return text


def compile_sites(rel, scratch):
    """-> (sites, error text): sites [(file, line, col, kind, R, [arg types], (outer file, line) or None)]"""
    import hashlib
    import subprocess
    text = glued_text(rel)
    key = hashlib.sha1((rel + "\0" + text).encode()).hexdigest()[:20]
    path = os.path.join(scratch, key + "_" + os.path.basename(rel))
    with open(path, "w") as fh:
        fh.write(text)
    cmd = ["clang++"] + compile_flags() + ["-iquote", os.path.join(ROOT, os.path.dirname(rel)), path]
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    sites, last = [], None
    for ln in r.stderr.splitlines():
        m = WARN.match(ln)
        if m:
            f = rel if m.group(1) == path else os.path.relpath(m.group(1), ROOT) if os.path.isabs(m.group(1)) else m.group(1)
            ta = split_targs(m.group(5))
            last = [f, int(m.group(2)), int(m.group(3)), m.group(4), ta[0], ta[1:], None]
            sites.append(last)
            continue
        m = NOTE_REQ.match(ln)
        if m and last is not None and m.group(1) == path and last[6] is None:
            last[6] = (rel, int(m.group(2)))
    errs = "\n".join(ln for ln in r.stderr.splitlines() if ": error:" in ln)
    os.remove(path)
    lines = text.split("\n")
    htexts = {}
    for st in sites:
        f, ln, col = st[0], st[1], st[2]
        if f == rel:
            src_line = lines[ln - 1] if ln - 1 < len(lines) else ""
        else:
            if f not in htexts:
                try:
                    htexts[f] = open(os.path.join(ROOT, f), encoding="utf-8", errors="replace").read().split("\n")
                except OSError:
                    htexts[f] = []
            hl = htexts[f]
            src_line = hl[ln - 1] if ln - 1 < len(hl) else ""
        gm = re.match(r"(?:WWHD_GLUE_FUNC|WWHD_FUNC)\(\s*0x([0-9A-Fa-f]{8})", src_line[col - 1:])
        if gm:  # a definition's own entry glue: the guest call its nested activation makes, with its parameters
            st.append(int(gm.group(1), 16))
            st.append("glue")
        else:
            m = SITE_HEAD.match(src_line[col - 1:])
            a = m.group(2).strip() if m else ""
            st.append(int(a.rstrip("u"), 16) if re.match(r"^0x[0-9A-Fa-f]{1,8}u?$", a) else None)
            stub = STUB.search(src_line) if f == rel else None
            st.append("stub" if stub else ("body" if f == rel else "hdr"))
        if f == rel:
            st[1] = ln - 1  # the source's own line number
    return text, sites, errs


def _compile_worker(args):
    rel, scratch = args
    try:
        text, sites, errs = compile_sites(rel, scratch)
    except Exception as e:  # noqa: BLE001 (one source's failure is reported with the rest)
        return rel, [], "decl_lint: %s: %s" % (type(e).__name__, e)
    return rel, sites, errs


IMAGE_PATH = [None]  # port mode: --image (workers read it themselves)


def _assets_paths():
    """port mode: a source's model data headers (tools/verify/assets.py, which autoglue expands) come from --image and
    --rpx and are written into --out-dir, never into this tree (a decompilation without assets.py: nothing to do)"""
    if not PATHS.get("assets"):
        return
    try:
        import assets
    except ImportError:
        return
    assets.IMAGE, assets.RPX, assets.OUT = PATHS["image"], PATHS["rpx"], PATHS["assets"]


def _worker_paths(paths, image):
    """a compile worker takes the parent's paths (the port mode's; a spawned process starts with the defaults)"""
    PATHS.update(paths)
    _assets_paths()
    if PATHS["tools"] not in sys.path:
        sys.path.append(PATHS["tools"])  # autoglue's rpx.py
    if image:
        IMAGE_PATH[0] = image
        autoglue._image = read_image(image)


def compile_all(sources, jobs, scratch, cache=None):
    """{source: (sites, errors)}; cache: a JSON file of earlier results keyed by the glued text and headers"""
    import concurrent.futures
    import hashlib
    import json
    hdr = hashlib.sha1()
    for d in ("tools/verify/include", "wwhd_src/include", os.path.join(PATHS["runtime"], "include")):
        for dp, _, fs in sorted(os.walk(os.path.join(ROOT, d))):
            for f in sorted(fs):
                st = os.stat(os.path.join(dp, f))
                hdr.update(("%s %d %d\n" % (os.path.join(dp, f), st.st_size, st.st_mtime_ns)).encode())
    hk = hdr.hexdigest()
    old = {}
    if cache and os.path.isfile(cache):
        try:
            old = json.load(open(cache))
        except ValueError:
            old = {}
    if old.get("headers") != hk:
        old = {"headers": hk, "src": {}}
    res, todo = {}, []
    for s in sources:
        st = os.stat(os.path.join(ROOT, s))
        k = "%d %d" % (st.st_size, st.st_mtime_ns)
        e = old["src"].get(s)
        if e and e[0] == k:
            res[s] = (e[1], e[2])
        else:
            todo.append(s)
    os.makedirs(scratch, exist_ok=True)
    def save():
        if cache:
            tmp = cache + ".tmp%d" % os.getpid()
            with open(tmp, "w") as fh:
                json.dump(old, fh)
            os.replace(tmp, cache)
    with concurrent.futures.ProcessPoolExecutor(max_workers=jobs, initializer=_worker_paths,
                                                initargs=(dict(PATHS), IMAGE_PATH[0])) as ex:
        for i, (rel, sites, errs) in enumerate(ex.map(_compile_worker, [(s, scratch) for s in todo], chunksize=4)):
            res[rel] = (sites, errs)
            st = os.stat(os.path.join(ROOT, rel))
            if not errs.startswith("decl_lint: "):  # a crash of the tool itself is not cached
                old["src"][rel] = ["%d %d" % (st.st_size, st.st_mtime_ns), sites, errs]
            if i % 200 == 199:
                save()
    save()
    return res


def count_args(s):
    """top-level arguments of a call's masked argument text (brackets only: `a->b` and `x < y` are no nesting)"""
    if not s.strip():
        return 0
    depth, n = 0, 1
    for ch in s:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        elif ch == "," and depth == 0:
            n += 1
    return n


SITE_HEAD = re.compile(r"(call|call_ptr|tail_ptr)\s*(?:<.*?>)?\s*\(\s*([^,()]*)")
STUB = re.compile(r"\{ return gabi::call<.*?>\(0x([0-9A-F]{8})\b.*/\* other unit \(ext\) \*/")


def site_regs(types):
    gi = sum(1 for t in types if type_class(t) == "r")
    fi = sum(1 for t in types if type_class(t) == "f")
    return gi, fi


ORDER = ["c", "local", "mixed", "nosite", "hdr", "b", "callee-b", "a", "e?i", "d?", "e?", "d", "e"]


def _import_reads(runtime=None):
    """{imp_lib_name: argument registers the runtime's HLE implementation reads} (runtime/src HLE(lib, name) bodies)"""
    import glob
    out = {}
    for path in glob.glob(os.path.join(runtime or os.path.join(ROOT, "runtime"), "src", "**", "*.cpp"), recursive=True):
        t = open(path, encoding="utf-8", errors="replace").read()
        for m in re.finditer(r"HLE\((\w+),\s*(\w+)\)\s*\{", t):
            i, depth = m.end(), 1
            while depth and i < len(t):
                depth += {"{": 1, "}": -1}.get(t[i], 0)
                i += 1
            body = t[m.end():i]
            rs = {("r", 3 + int(x)) for x in re.findall(r"\barg\(c,\s*(\d+)\)", body)}
            rs |= {("r", int(x)) for x in re.findall(r"c->r\[(\d+)\]", body)}
            for x in re.findall(r"\barg64\(c,\s*(\d+)\)", body):
                rs |= {("r", int(x)), ("r", int(x) + 1)}
            rs |= {("f", int(x)) for x in re.findall(r"c->f\[(\d+)\]", body)}
            if re.search(r"\b(ppc_dispatch|va_|vararg)", body):  # forwards or walks its arguments: all
                rs |= set(ARG_REGS)
            out["imp_%s_%s" % (m.group(1), m.group(2))] = frozenset(rs)
    return out


ARG_REGS = [("r", i) for i in range(3, 11)] + [("f", i) for i in range(1, 9)]
IMPORT_READS = _import_reads()
# coreinit's data imports the runtime implements as host functions (runtime/src/main.cpp init_data_imports)
_HEAP_IMPORT_READS = {"imp_coreinit_MEMAllocFromDefaultHeap": frozenset([("r", 3)]),
                      "imp_coreinit_MEMAllocFromDefaultHeapEx": frozenset([("r", 3), ("r", 4)]),
                      "imp_coreinit_MEMFreeToDefaultHeap": frozenset([("r", 3)])}
IMPORT_READS.update(_HEAP_IMPORT_READS)
_DATA_IMPORTS = None


class IndirectTargets:
    """the possible targets of the original's bctr/bctrl calls (2026-11-04): `lwz rX, SLOT(rV)` of a vtable pointer
    `lwz rV, VOFF(rObj)` -> the function at SLOT of every vtable that some constructor stores at VOFF (`lis/addi` of the
    vtable then `stw` to VOFF of an object); `lwz rX, OFF(rK)` of a `lis` constant -> the function at that address.
    Over-approximate (every class with a vtable at VOFF); None when the call's target cannot be resolved."""

    def __init__(self, cache_dir=None):
        import pickle
        path = PATHS["rpx"]
        self.ok = os.path.isfile(path)
        if not self.ok:
            return
        st = os.stat(path)
        key = "%d-%d" % (st.st_size, st.st_mtime_ns)
        cp = os.path.join(cache_dir, "targets3-%s.pkl" % key) if cache_dir else None
        if cp and os.path.isfile(cp):
            self.ins, self.vt, self.words, self.data, self.vstores = pickle.load(open(cp, "rb"))
            return
        sys.path.insert(0, PATHS["tools"])
        import capstone
        from rpx import Rpx
        rpx = Rpx(path)
        t = rpx.by_name[".text"]
        md = capstone.Cs(capstone.CS_ARCH_PPC, capstone.CS_MODE_32 | capstone.CS_MODE_BIG_ENDIAN | capstone.CS_MODE_PS)
        md.skipdata = True
        ins = [(i.address, i.mnemonic, i.op_str) for i in md.disasm(t.data, t.addr)]
        vt, hi, vstores = collections.defaultdict(set), {}, []
        for k, (a, m, o) in enumerate(ins):
            ops = [x.strip() for x in o.split(",")]
            if m == "lis" and len(ops) == 2:
                hi[ops[0]] = (int(ops[1], 0) << 16, k)
                continue
            if m in ("addi", "addic") and len(ops) == 3 and ops[1] in hi and k - hi[ops[1]][1] < 12:
                v = (hi[ops[1]][0] + int(ops[2], 0)) & 0xFFFFFFFF
                for j in range(k + 1, min(len(ins), k + 8)):
                    m2, o2 = ins[j][1], ins[j][2]
                    mm = re.match(r"(r\d+), (-?0x[0-9a-f]+|-?\d+)\((r\d+)\)", o2)
                    if m2 == "stw" and mm and mm.group(1) == ops[0]:
                        vt[int(mm.group(2), 0)].add(v)
                        vstores.append((ins[j][0], v, int(mm.group(2), 0)))
                    if o2.split(",")[0].strip() == ops[0] and not m2.startswith(("st", "cmp")):
                        break
                continue
            if ops and ops[0] in hi and not m.startswith(("st", "cmp")):
                hi.pop(ops[0], None)
        self.ins = {a: (m, o) for a, m, o in ins}
        self.vt = {k: frozenset(v) for k, v in vt.items()}
        self.vstores = vstores
        self.words = {}
        self.data = [(sec.addr, bytes(sec.data)) for sec in rpx.sections if sec.data and sec.name in (".data", ".rodata")]
        for sec in rpx.sections:
            if sec.data and sec.name in (".data", ".rodata"):
                for off in range(0, len(sec.data) - 3, 4):
                    w = int.from_bytes(sec.data[off:off + 4], "big")
                    if t.addr <= w < t.addr + len(t.data):
                        self.words[sec.addr + off] = w
        if cp:
            os.makedirs(cache_dir, exist_ok=True)
            pickle.dump((self.ins, self.vt, self.words, self.data, self.vstores), open(cp + ".tmp", "wb"))
            os.replace(cp + ".tmp", cp)

    @staticmethod
    def _dest(m, o):
        if m.startswith(("st", "cmp", "fcmp", "b", "mt")):
            return None
        x = o.split(",")[0].strip()
        return x or None

    def resolve(self, site):
        """('slot', SLOT, VOFF) / ('ptr', ADDR) / None for the bctr/bctrl at site"""
        if not self.ok:
            return None
        reg, a = None, site
        for _ in range(20):
            a -= 4
            m, o = self.ins.get(a, ("?", ""))
            if m == "mtctr" and reg is None:
                reg = o.strip()
                continue
            if reg and self._dest(m, o) == reg:
                mm = re.match(r"\w+, (-?0x[0-9a-f]+|-?\d+)\((r\d+)\)", o) if m == "lwz" else None
                if not mm:
                    return None
                off, base, b = int(mm.group(1), 0), mm.group(2), a
                for _ in range(20):
                    b -= 4
                    m2, o2 = self.ins.get(b, ("?", ""))
                    if self._dest(m2, o2) == base:
                        if m2 == "lis":
                            return ("ptr", ((int(o2.split(",")[1], 0) << 16) + off) & 0xFFFFFFFF)
                        m3 = re.match(r"\w+, (-?0x[0-9a-f]+|-?\d+)\((r\d+)\)", o2) if m2 in ("lwz", "lwzu") else None
                        return ("slot", off, int(m3.group(1), 0)) if m3 else None
                return None
        return None

    def candidates(self, site, known, fn=None, size=0, family=None):
        """the functions the call at site may reach (decompiled or in build/gen: `known`), None if unresolved.
        fn/size/family (2026-11-04, step B): when the call's object is fn's own `this` (entry r3, kept in any register)
        and family (the vtables that hold a method of fn's class) has vtables at the call's vtable-pointer offset,
        only those vtables count"""
        r = self.resolve(site)
        if r is None:
            t = self.ptmf_table(site, known)
            return t
        if r[0] == "ptr":
            w = self.words.get(r[1])
            return frozenset([w]) if w in known else None
        bases = self.vt.get(r[2], frozenset())
        if family and fn is not None and self.object_is_this(fn, size, site):
            narrow = bases & family
            if narrow:
                bases = self.with_subclasses(narrow, bases)
        out = frozenset(w for w in (self.words.get(v + r[1]) for v in bases) if w in known)
        return out or None

    def half(self, a):
        for base, d in self.data:
            if base <= a < base + len(d) - 1:
                return int.from_bytes(d[a - base:a - base + 2], "big", signed=True)
        return None

    def word(self, a):
        for base, d in self.data:
            if base <= a < base + len(d) - 3:
                return int.from_bytes(d[a - base:a - base + 4], "big")
        return None

    def _def_of(self, a, reg, limit=24):
        """(address, mnemonic, operands) of the last write of reg before a (straight-line, None if not found)"""
        for _ in range(limit):
            a -= 4
            m, o = self.ins.get(a, ("?", ""))
            if m in ("blr", "bctr", "b"):
                # a block entered only by a branch: continue before the one conditional branch to it (if unique)
                start, src = a + 4, []
                for k in range(1, 64):
                    m2, o2 = self.ins.get(start - 4 * k, ("?", ""))
                    t = re.findall(r"0x[0-9a-f]+", o2)
                    if m2.startswith("b") and m2 not in ("b", "bl", "blr", "bctr", "bctrl") and t and \
                            int(t[-1], 16) == start:
                        src.append(start - 4 * k)
                if len(src) != 1:
                    return None
                a = src[0]
                continue
            if m in ("bl", "bctrl"):
                return None
            if self._dest(m, o) == reg:
                return a, m, [x.strip() for x in o.split(",")]
        return None

    def _const(self, a, reg):
        """the constant a lis/addi pair puts into reg before a, or None"""
        d = self._def_of(a, reg)
        if d and d[1] in ("addi", "addic") and len(d[2]) == 3:
            h = self._def_of(d[0], d[2][1])
            if h and h[1] == "lis":
                return ((int(h[2][1], 0) << 16) + int(d[2][2], 0)) & 0xFFFFFFFF
        return None

    def _table_entry(self, a, reg):
        """(table base, stride) when reg holds base + index * stride (an add of a lis/addi constant and a mulli/slwi)"""
        d = self._def_of(a, reg)
        if not d or d[1] != "add" or len(d[2]) != 3:
            return None
        for x, y in ((d[2][1], d[2][2]), (d[2][2], d[2][1])):
            b = self._const(d[0], x)
            if b is None:
                continue
            sd = self._def_of(d[0], y)
            if sd and sd[1] == "mulli":
                return b, int(sd[2][2], 0)
            if sd and sd[1] in ("slwi", "rlwinm") and sd[1] == "slwi":
                return b, 1 << int(sd[2][2], 0)
        return None

    def _entries(self, base, stride, off):
        """(idx, function) of the pointer-to-member triples {s16 adjust, s16 index, u32 function} at off of a table's
        entries (stops at the first entry that is no such triple)"""
        out = []
        for k in range(256):
            e = base + k * stride + off
            idx, fn = self.half(e + 2), self.word(e + 4)
            if idx is None or fn is None or not (-1 <= idx < 0x400):
                break
            if idx == -1 and not (fn == 0 or fn in self.code_starts):
                break
            out.append((idx, fn, self.half(e + 6)))
        return out

    def ptmf_table(self, site, known):
        """candidates of a pointer-to-member call through a constant table (2026-11-04, step C): `lwz rF, OFF(rT)`
        with rT = table + index * stride (non-virtual: every entry's function with index -1), or the virtual path
        `lwz rF, 4(rS)`, rS = vtable + (lha IDX(rT)) * 8 (every entry with an index >= 0: that slot of each vtable at
        the entry's vtable offset; no such entry: no target). None when the call is not of that shape"""
        if not hasattr(self, "code_starts"):
            self.code_starts = frozenset(known)
        a, reg = site, None
        for _ in range(20):
            a -= 4
            m, o = self.ins.get(a, ("?", ""))
            if m == "mtctr" and reg is None:
                reg = o.strip()
                break
        if reg is None:
            return None
        d = self._def_of(a, reg)
        if not d or d[1] != "lwz":
            return None
        mm = re.match(r"(-?0x[0-9a-f]+|-?\d+)\((r\d+)\)", d[2][1])
        if not mm:
            return None
        off, rt = int(mm.group(1), 0), mm.group(2)
        te = self._table_entry(d[0], rt)
        if te is not None:  # non-virtual: the triple's function word is at off
            ents = self._entries(te[0], te[1], off - 4)
            if not ents:
                return None
            return frozenset(fn for idx, fn, _ in ents if idx == -1 and fn in known)
        # virtual: rt = vtable + index * 8
        ad = self._def_of(d[0], rt)
        if not ad or ad[1] != "add" or off != 4:
            return None
        for x, y in ((ad[2][1], ad[2][2]), (ad[2][2], ad[2][1])):
            sd = self._def_of(ad[0], y)
            if not sd or sd[1] != "slwi" or sd[2][2] not in ("3", "0x3"):
                continue
            ld = self._def_of(sd[0], sd[2][1])
            if not ld or ld[1] != "lha":
                continue
            m2 = re.match(r"(-?0x[0-9a-f]+|-?\d+)\((r\d+)\)", ld[2][1])
            te = self._table_entry(ld[0], m2.group(2)) if m2 else None
            if te is None:
                continue
            ents = self._entries(te[0], te[1], int(m2.group(1), 0) - 2)
            if not ents:
                return None
            out = set()
            for idx, fn, voff in ents:
                if idx >= 0:
                    if voff is None:
                        return None
                    out |= {w for w in (self.words.get(v + idx * 8 + 4) for v in self.vt.get(voff, ())) if w in known}
                    if not self.vt.get(voff):
                        return None
            return frozenset(out)
        return None

    def entries(self, v):
        """{slot: function} of the vtable at v (up to the next vtable base)"""
        if not hasattr(self, "_entries_memo"):
            self._entries_memo, self._allv = {}, set().union(*self.vt.values()) if self.vt else set()
        if v not in self._entries_memo:
            e = {}
            for k in range(0, 0x400, 4):
                if k and v + k in self._allv:
                    break
                w = self.words.get(v + k)
                if w is not None:
                    e[k] = w
            self._entries_memo[v] = e
        return self._entries_memo[v]

    def children(self, starts):
        """{vtable: vtables of its direct subclasses} from the constructors (step C, 2026-11-04): a function that stores
        vtable W calls (bl, before that store) a function storing V, or stores V at the same offset earlier itself (an
        inlined base constructor): W's class derives from V's"""
        if hasattr(self, "_children"):
            return self._children
        import bisect
        starts = sorted(starts)

        def func_of(a):
            i = bisect.bisect_right(starts, a) - 1
            return starts[i] if i >= 0 else None
        by_func = collections.defaultdict(list)
        for a, v, voff in self.vstores:
            by_func[func_of(a)].append((a, v, voff))
        kids = collections.defaultdict(set)
        for f, stores in by_func.items():
            for a, w, voff in stores:
                for a2, v, voff2 in stores:
                    if a2 < a and voff2 == voff and v != w:
                        kids[v].add(w)
                x = f
                while x is not None and x < a:
                    m, o = self.ins.get(x, ("?", ""))
                    if m == "bl":
                        t = re.findall(r"0x[0-9a-f]+", o)
                        for _a, v, voff2 in by_func.get(int(t[-1], 16), ()) if t else ():
                            if voff2 == voff and v != w:
                                kids[v].add(w)
                    x += 4
        self._children = kids
        return kids

    def with_subclasses(self, narrow, bases):
        """narrow plus the vtables of every subclass (children(), transitively) that the call's offset admits: a subclass
        that overrides all the class's decompiled methods holds none of them (step C, 2026-11-04)"""
        kids = getattr(self, "_children", {})
        out, todo = set(narrow), list(narrow)
        while todo:
            v = todo.pop()
            for w in kids.get(v, ()):
                if w not in out:
                    out.add(w)
                    todo.append(w)
        return frozenset(out & bases) | frozenset(narrow)

    def vtables_holding(self, fns):
        """the vtables (constructor-stored bases) that hold one of the functions fns"""
        if not hasattr(self, "_holding"):
            self._holding = collections.defaultdict(set)
            allv = set().union(*self.vt.values()) if self.vt else set()
            for v in allv:
                for k in range(0, 0x400, 4):  # up to the next vtable base (over-inclusion only widens a family)
                    if k and v + k in allv:
                        break
                    w = self.words.get(v + k)
                    if w is not None:
                        self._holding[w].add(v)
        out = set()
        for f in fns:
            out |= self._holding.get(f, set())
        return frozenset(out)

    def object_is_this(self, fn, size, site):
        """the object whose vtable the call at site loads is fn's entry r3 on every path (must-analysis over fn's
        instructions: `mr`/`or rD,rS,rS` copies, calls clobber r0/r3-r12, any other write kills)"""
        r = self.resolve(site)
        if r is None or r[0] != "slot":
            return False
        # the instruction that loads the vtable pointer and its base register
        reg, a, load, base = None, site, None, None
        for _ in range(20):
            a -= 4
            m, o = self.ins.get(a, ("?", ""))
            if m == "mtctr" and reg is None:
                reg = o.strip()
                continue
            if reg and self._dest(m, o) == reg:
                b = a
                vbase = re.match(r"\w+, (?:-?0x[0-9a-f]+|-?\d+)\((r\d+)\)", o).group(1)
                for _ in range(20):
                    b -= 4
                    m2, o2 = self.ins.get(b, ("?", ""))
                    if self._dest(m2, o2) == vbase:
                        mb = re.match(r"\w+, (?:-?0x[0-9a-f]+|-?\d+)\((r\d+)\)", o2)
                        if mb is None or m2 == "lwzu":  # lwzu: the base is this + VOFF afterwards, not this
                            return False
                        load, base = b, mb.group(1)
                        break
                break
        if load is None:
            return False
        facts = self._this_facts(fn, size)
        return base in facts.get(load, frozenset())

    def _this_facts(self, fn, size):
        """{instruction address: registers that hold fn's entry r3 before it} (must)"""
        if not hasattr(self, "_facts"):
            self._facts = {}
        if fn in self._facts:
            return self._facts[fn]
        addrs = list(range(fn, fn + size, 4))
        succ = {}
        for a in addrs:
            m, o = self.ins.get(a, ("?", ""))
            t = re.findall(r"0x[0-9a-f]+", o)
            tgt = int(t[-1], 16) if t and m.startswith("b") and m not in ("bl", "bctrl", "blrl") else None
            nx = [] if m in ("b", "blr", "bctr") else [a + 4]
            if tgt is not None and fn <= tgt < fn + size:
                nx.append(tgt)
            succ[a] = [x for x in nx if fn <= x < fn + size]
        vol = {"r0"} | {"r%d" % i for i in range(3, 13)}
        top = None
        inn = {a: top for a in addrs}
        inn[fn] = frozenset(["r3"])
        work = [fn]
        while work:
            a = work.pop()
            cur = inn[a]
            m, o = self.ins.get(a, ("?", ""))
            ops = [x.strip() for x in o.split(",")]
            out = set(cur)
            if m in ("bl", "bctrl", "blrl"):
                out -= vol
            elif m == "mr" or (m in ("or", "or.") and len(ops) == 3 and ops[1] == ops[2]):
                out.discard(ops[0])
                if ops[1] in cur:
                    out.add(ops[0])
            elif m == "lmw":
                out -= {"r%d" % i for i in range(int(ops[0][1:]), 32)}
            else:
                d = self._dest(m, o)
                if d:
                    out.discard(d)
                if re.match(r"(l|st)\w*u", m):
                    mm = re.search(r"\((r\d+)\)", o)
                    if mm:
                        out.discard(mm.group(1))
            out = frozenset(out)
            for b in succ.get(a, []):
                nv = out if inn[b] is None else (inn[b] & out)
                if nv != inn[b]:
                    inn[b] = nv
                    work.append(b)
        res = {a: v for a, v in inn.items() if v is not None}
        self._facts[fn] = res
        return res


def data_imports():
    """{instruction address: imp_lib_name} of the instructions that load a data import's address (rpx relocations)"""
    global _DATA_IMPORTS
    if _DATA_IMPORTS is None:
        _DATA_IMPORTS = {}
        path = PATHS["rpx"]
        if os.path.isfile(path):
            sys.path.insert(0, PATHS["tools"])
            from rpx import Rpx
            for _sec, off, _typ, sym, _add in Rpx(path).relocs:
                if getattr(sym, "import_kind", None) == "d":
                    _DATA_IMPORTS[off & ~3] = "imp_%s_%s" % (sym.import_lib, sym.name)
    return _DATA_IMPORTS


class Audit:
    """every guest call of the decompiled code (compile mode) against what its target takes"""

    def __init__(self, defs, exts, compiled, li, flow, siblings=None):
        self.defs, self.exts, self.li, self.flow = defs, exts, li, flow
        self.siblings = siblings  # {source: other sources linked with it}; None: autoglue's spec siblings
        self.hdr_code, self.refs_memo, self.code_memo, self.wrap_memo = {}, {}, {}, {}
        self.by_file = collections.defaultdict(list)
        for ds in defs.values():
            for d in ds:
                self.by_file[d.file].append(d)
        self.ext_by_file = collections.defaultdict(list)
        for e in exts:
            self.ext_by_file[e.file].append(e)
        self.texts = {}
        self.D = {}       # addr -> (int, float) registers its definition takes, as compiled (entry glue)
        self.sites = []   # (where, file, line, kind, R, types, addr, caller def or None, outer)
        seen = set()
        for rel, (sites, _) in sorted(compiled.items()):
            for f, line, col, kind, ret, types, outer, addr, where in sites:
                if where == "glue":
                    self.D.setdefault(addr, site_regs(types))
                    continue
                if where == "hdr":
                    key = (f, line, col, ret, tuple(types))
                    if key in seen:
                        continue
                    seen.add(key)
                caller = None
                if where == "body":
                    caller = self.enclosing(rel, line)
                elif where == "hdr" and outer:
                    caller = self.enclosing(outer[0], outer[1] - 1)
                self.sites.append((where, f if where != "stub" else rel, line, kind, ret, types, addr, caller, outer))
        for a, ds in defs.items():
            if a not in self.D and ds[0].regs is not None:
                self.D[a] = ds[0].regs

    def enclosing(self, rel, line):
        for d in self.by_file.get(rel, []):
            if d.line <= line <= d.end:
                return d
        return None

    def varargs(self, a):
        return a in self.flow.gen.loc and self.flow.df.cfg(a).varargs

    def upper(self, a):
        if not hasattr(self, "_up"):
            from funcdb import Dataflow
            self._up = Dataflow(self.flow.gen, unknown_reads_args=True, cfgs=self.flow.df.cfgs)
        return frozenset(self._up.livein(a)) if a in self.flow.gen.loc else frozenset()

    def code_of(self, rel):
        """masked code of a source before its glue block (or of a header)"""
        if rel not in self.code_memo:
            if rel not in self.texts:
                with open(os.path.join(ROOT, rel), encoding="utf-8", errors="replace") as fh:
                    self.texts[rel] = autoglue.mask(fh.read()).split("\n")
            code = "\n".join(self.texts[rel])
            self.code_memo[rel] = code[:code.find("/* WWHD glue")] if "/* WWHD glue" in code else code
        return self.code_memo[rel]

    def siblings_of(self, rel):
        """the other sources linked with rel (they share a verify unit): they call its ext stubs through a header
        declaration (d_s_play_2.cpp -> d_s_play.cpp's stub; 2026-11-04)"""
        if self.siblings is not None:
            return sorted(self.siblings.get(rel, ()))
        return sorted(autoglue._spec_siblings(rel)) if not os.path.isabs(rel) else []

    def header_code(self, path):
        if path not in self.hdr_code:
            with open(path, encoding="utf-8", errors="replace") as fh:
                self.hdr_code[path] = autoglue.mask(fh.read())
        return self.hdr_code[path]

    def calls_in(self, code, name, nargs):
        """offsets of the uses of `name` in masked code that make calls: `name(...)` (no prototype), and the name
        passed on as a host function pointer (`beamCall(mDoAud_seStartHD, ...)`, `boInvoke(boStopBattleSound)`: the
        helper calls it; 2026-11-04). A use converted to a guest address (a gabi::HostFn binding, gabi::guest_fn) is
        no call: the guest calls the original's address with its full registers.
        nargs: the argument count an overloaded stub's calls must match (None: any)"""
        out = []
        for m in re.finditer(r"(?<![\w~])%s\b" % re.escape(name), code):
            ls = code.rfind("\n", 0, m.start()) + 1
            le = code.find("\n", m.end())
            line = code[ls:le if le >= 0 else len(code)]
            if DECL_LINE.match(line):
                continue
            call = re.match(r"\s*\(", code[m.end():])
            if call:
                if nargs is not None:
                    op = m.end() + call.end() - 1
                    close = autoglue.match_close(code, op, "(", ")")
                    if close < 0 or count_args(code[op + 1:close]) != nargs:
                        continue
            elif re.search(r"(?:\.|->)\s*$", code[ls:m.start()]) or self.in_guest_address(code, m.start()):
                continue  # a member of that name, or a guest address
            q = re.search(r"(\w+)\s*::\s*$", code[max(0, m.start() - 80):m.start()])
            out.append((m.start(), q.group(1) if q else None))
        return out

    @staticmethod
    def in_guest_address(code, pos):
        """pos lies inside the arguments of gabi::guest_fn(...) or a gabi::HostFn binding's constructor"""
        for g in re.finditer(r"\bguest_fn\s*\(|\bHostFn\s+\w+\s*\(", code[max(0, pos - 400):pos]):
            op = max(0, pos - 400) + g.end() - 1
            try:
                if autoglue.match_close(code, op, "(", ")") > pos:
                    return True
            except autoglue.GlueError:
                pass
        return False

    def header_wrappers(self, rel, name, nargs):
        """inline functions of the headers rel includes whose bodies call `name`: [(wrapper name, scope or None)]
        (m_Do_machine.h `namespace cMl { inline void init() { cMl_initFromHeapReturnRegister(); } }`, 2026-11-04)"""
        key = (rel, name, nargs)
        if key in self.wrap_memo:
            return self.wrap_memo[key]
        out = []
        with open(os.path.join(ROOT, rel), encoding="utf-8", errors="replace") as fh:
            src = fh.read()
        for path, _ in autoglue.include_closure(src, rel):
            code = self.header_code(path)
            if name not in code:
                continue
            for pos, _q in self.calls_in(code, name, nargs):
                best = None
                for fm in re.finditer(r"\b(~?\w+)\s*\([^;{}]*\)\s*(?:const\s*)?(?:noexcept\s*)?\{", code[:pos]):
                    if fm.group(1) in ("if", "for", "while", "switch", "return", "sizeof"):
                        continue
                    if autoglue.match_close(code, fm.end() - 1, "{", "}") > pos:
                        best = fm
                if best is None:
                    continue
                scope = None
                for sm in re.finditer(r"\b(?:struct|class|namespace)\s+(\w+)\s*(?::[^{;()]*)?\{", code[:best.start()]):
                    if autoglue.match_close(code, sm.end() - 1, "{", "}") > best.start():
                        scope = sm.group(1)
                if (best.group(1), scope) not in out:
                    out.append((best.group(1), scope))
        self.wrap_memo[key] = out
        return out

    def stub_refs(self, rel, a, regs):
        """[(file, enclosing definition or None)] of every call of the ext stub(s) of a that source rel declares with
        these registers (callback lines excluded): in rel itself, in the sources linked with it (unless they declare
        their own stub of that name), and through inline wrappers of the headers those include"""
        key = (rel, a, regs)
        if key in self.refs_memo:
            return self.refs_memo[key]
        es = [e for e in self.ext_by_file.get(rel, []) if e.addr == a and e.regs == regs
              and "callback" not in e.text.split("#")[0].split()]
        out = []
        for e in es:
            name = e.qname.split("::")[-1]
            same = [x for x in self.ext_by_file.get(rel, []) if x.qname.split("::")[-1] == name]
            nargs = len(e.ptypes) if len({len(x.ptypes) for x in same}) > 1 else None
            for f in [rel] + self.siblings_of(rel):
                if f != rel and any(x.qname.split("::")[-1] == name for x in self.ext_by_file.get(f, [])):
                    continue
                if f != rel and not os.path.isfile(os.path.join(ROOT, f)):
                    continue
                code = self.code_of(f)
                hits = [pos for pos, _q in self.calls_in(code, name, nargs)]
                for w, scope in self.header_wrappers(f, name, nargs):
                    if w == name:
                        continue
                    for pos, q in self.calls_in(code, w, None):
                        d = self.enclosing(f, code.count("\n", 0, pos) + 1)
                        if scope is None or q == scope or (q is None and d is not None and d.qname.startswith(scope + "::")):
                            hits.append(pos)
                for pos in hits:
                    d = self.enclosing(f, code.count("\n", 0, pos) + 1)
                    if (f, d) not in out:
                        out.append((f, d))
        self.refs_memo[key] = out
        return out

    def stub_result_used(self, rel, a, regs):
        """does any call of the ext stub(s) of a (as stub_refs finds them, directly in a source) use its result? A call
        that is a whole statement (`name(...);`, also after `if (...)`, `else`, a `(void)` cast) discards it"""
        es = [e for e in self.ext_by_file.get(rel, []) if e.addr == a and e.regs == regs]
        for e in es:
            name = e.qname.split("::")[-1]
            for f in [rel] + self.siblings_of(rel):
                if f != rel and any(x.qname.split("::")[-1] == name for x in self.ext_by_file.get(f, [])):
                    continue
                if f != rel and not os.path.isfile(os.path.join(ROOT, f)):
                    continue
                code = self.code_of(f)
                for pos, _q in self.calls_in(code, name, len(e.ptypes) if False else None):
                    m = re.match(r"%s\s*\(" % re.escape(name), code[pos:])
                    if not m:
                        return True  # passed on as a function pointer: unknown
                    close = autoglue.match_close(code, pos + m.end() - 1, "(", ")")
                    after = code[close + 1:].lstrip()
                    before = re.sub(r"(?:\w+\s*(?:::|\.|->)\s*)*$", "", code[:pos]).rstrip()
                    before = re.sub(r"\(\s*void\s*\)$", "", before).rstrip()
                    stmt = before.endswith((";", "{", "}")) or re.search(r"\belse$", before) is not None
                    if not stmt and before.endswith(")"):
                        o = before.rfind("(")
                        depth, i = 0, len(before) - 1
                        while i >= 0:
                            if before[i] == ")":
                                depth += 1
                            elif before[i] == "(":
                                depth -= 1
                                if depth == 0:
                                    break
                            i -= 1
                        stmt = re.search(r"\b(if|while|for)\s*$", before[:i]) is not None
                    if not (stmt and after.startswith(";")):
                        return True
        return False

    def stub_skip(self, rel, a, regs):
        """an ext stub that makes no call: every ext line behind it is a `callback` (the source takes the function's
        address) or nothing calls it (stub_refs: the source, the sources linked with it, header inline wrappers)"""
        if not any(e.addr == a and e.regs == regs for e in self.ext_by_file.get(rel, [])):
            return False
        return not self.stub_refs(rel, a, regs)

    def stub_users(self, rel, a, regs):
        """the definitions (of rel or a source linked with it) whose bodies call the ext stub(s) of a declared with
        these registers, directly or through a header inline wrapper"""
        out = []
        for _f, d in self.stub_refs(rel, a, regs):
            if d is not None and d not in out:
                out.append(d)
        return out

    def callers_of(self, where, rel, caller, a, regs=None):
        """[(caller def, entry-held must, may)] for the original's calls of a in this site's function(s)"""
        cands = [caller] if caller else (self.stub_users(rel, a, regs) if where == "stub" else [])
        out = []
        for f in cands:
            for must, may in self.flow.sites(f.addr).get(a, []):
                out.append((f, must, may))
        return out

    def classify(self, site):
        where, rel, line, kind, ret, types, a, caller, outer = site
        P = mask_of(site_regs(types))
        L = self.li.get(a, frozenset())
        dec = a in self.D
        if self.varargs(a):
            return "c", "variadic callee", dec
        need = mask_of(self.D[a]) if dec else L
        missing = need - P
        extra = (P - need) if dec else frozenset()
        if not missing and not extra:
            return None, "", dec
        U = self.upper(a)
        cls, notes = [], []
        if missing:
            real = missing & L
            if dec and missing - U:
                cls.append("d")
                notes.append("callee declares %s, the original never reads it" % regs_text(missing - U))
            if dec and (missing & U) - L:
                cls.append("d?")
                notes.append("callee declares %s, only an indirect call of the original may read it" % regs_text((missing & U) - L))
            if real:
                if where == "hdr" and not caller:
                    cls.append("hdr")
                    notes.append("missing %s in a header call" % regs_text(real))
                else:
                    sites = self.callers_of(where, rel, caller, a, site_regs(types))
                    if not sites:
                        cls.append("nosite")
                        notes.append("missing %s; the original of this function does not call it" % regs_text(real))
                    for f, must, may in sites:
                        own = mask_of(self.D.get(f.addr) or (0, 0))
                        for r in sorted(real):
                            k = ("a" if r in own else "b") if r in must else ("mixed" if r in may else "local")
                            cls.append(k)
                            notes.append("%s %s%d@%08X" % (k, r[0], r[1], f.addr))
        if extra:
            if extra & L:
                cls.append("callee-b")
                notes.append("passes %s: the callee does not declare it, the original reads it" % regs_text(extra & L))
            if (extra & U) - L:
                fwd, where_ = self.forwarded(a, (extra & U) - L)
                if fwd:
                    cls.append("e?i")
                    notes.append("passes %s: the callee's original passes it on unchanged to an indirect call (%s): "
                                 "take it and forward it" % (regs_text(fwd), where_))
                if ((extra & U) - L) - fwd:
                    cls.append("e?")
                    notes.append("passes %s: only an indirect call further down may read it" % regs_text(((extra & U) - L) - fwd))
            if extra - U:
                cls.append("e")
                notes.append("passes %s: nothing reads it" % regs_text(extra - U))
        return min(set(cls), key=ORDER.index), " | ".join(dict.fromkeys(notes)), dec

    def forwarded(self, a, regs):
        """(registers of regs that a's original still holds from its entry at one of its own indirect calls, where)
        (2026-11-04: hdLayoutResMgr_c::resume passes its r4 on to the thread's sendMessage). An import that is known
        (runtime HLE) not to read the register does not count."""
        hit, where_ = set(), []
        known = INDIRECT_EXCEPTIONS.get(a, frozenset())
        for addr, imp, held in self.flow.indirect_held(a):
            r = (set(held) & set(regs)) - known
            if imp is not None and imp in IMPORT_READS:
                r &= IMPORT_READS[imp]
            if r:
                hit |= r
                where_.append("%s@%08X" % (imp or "bctr", addr))
        return frozenset(hit), ", ".join(where_[:3])

    def def_under(self):
        """{addr: registers the original reads at entry that the definition does not take}"""
        out = {}
        for a, regs in self.D.items():
            if regs is None or a not in self.flow.gen.loc:
                continue
            u = self.li.get(a, frozenset()) - mask_of(regs)
            if u:
                out[a] = u
        return out


DECL_LINE = re.compile(r"^(?:extern\s+|static\s+|inline\s+)*(?!return\b)[A-Za-z_][\w:<>,]*(?:[\s\*&]+[A-Za-z_][\w:<>,]*)*[\s\*&]+~?\w+\s*\([^;{}]*\)\s*(?:const\s*)?;")  # an unindented prototype

# classes of a non-exact call (Audit.classify):
#   a        the missing register holds the caller's own parameter at the call (pass it)
#   b        it holds the caller's entry value, which the caller does not declare (make it a parameter)
#   local    it holds a value the caller computed (an earlier call's result or argument): pass it explicitly
#   mixed    entry value on some paths only
#   nosite   the site's function is not a decompiled function whose original calls the target (a helper)
#   hdr      a header call with an under-declared argument and no caller to classify it by
#   callee-b the site passes a register the callee's definition does not take but its original reads
#   c        variadic callee (cannot be declared exactly)
#   d / d?   the callee declares a parameter its original never reads (d? only an indirect call might)
#   e / e?   the site passes an argument nothing reads (e? only an indirect call further down might)
#   e?i      the site passes an argument the callee's definition does not take, and the callee's original passes it on
#            unchanged to one of its own indirect calls (bctr/bctrl or an import that may read it): the definition must
#            take it and forward it (2026-11-04, hdLayoutResMgr_c::resume -> the thread's sendMessage reads r4)
ERROR_CLASSES = ("a", "b", "local", "mixed", "nosite", "hdr", "callee-b", "e?i", "ret")
#   ret      (result check, result_audit) a call site declares a result register the callee's definition does not
#            produce, and the caller's original reads it after the call: the decompiled caller reads a stale register
#   ret?     the same, but the caller's original only returns the register on to its own callers (unclear: the
#            callee is `guest` in the hand-off)
#   ret-unread  the same declaration, but no caller reads the register (tolerated)
#   ind      (not a call site class) the original's own indirect call may read an entry register the decompiled body
#            does not pass (callptr_audit): the function is `guest` in the hand-off
# d? and e/e? stay exceptions (the native path passes everything for d? callees and drops
# trailing extra arguments by this table); c cannot be expressed; d is reported (unread parameters, harmless).
# 2026-11-04: an e? extra is not droppable when an indirect call further down reads it (resume): the callee's own
# indirect calls are e?i (an error, above). A callee with unresolved e? sites left is `guest`
# in the hand-off (the round trip keeps the register; a lost register is a silent wrong result)
TOLERATED = ("c", "d", "d?", "e", "e?", "ind", "ret?", "ret-unread")
EXCEPTIONS = os.path.join(HERE, "decl_exceptions.tsv")
INDIRECT_EXCEPTIONS = {}  # filled from EXCEPTIONS by run_compile_audit
RAW_READ = re.compile(r"gabi::cpu->(?:r\[(?:[3-9]|10)\]|f\[[1-8]\])")


def read_indirect_exceptions(path):
    """{callee addr: registers} from `indirect<TAB>ADDR<TAB>REGS<TAB>reason` lines: registers the callee's original holds
    at its own indirect calls that no target of those calls reads (checked against the binary: the vtables or tables
    the call can reach), or that cannot be passed as a value (`guest` in the reason: the round trip keeps them)"""
    out = {}
    if os.path.isfile(path):
        for ln in open(path, encoding="utf-8"):
            f = ln.rstrip("\n").split("\t")
            if len(f) >= 4 and f[0] == "indirect":
                out[int(f[1], 16)] = frozenset((r[0], int(r[1:])) for r in f[2].split(","))
    return out


def read_exceptions(path):
    """{addr: reason} of definitions whose original's live-in registers are a known over-approximation"""
    out = {}
    if os.path.isfile(path):
        for ln in open(path, encoding="utf-8"):
            ln = ln.split("#", 1)[0].rstrip() if not ln.lstrip().startswith("#") else ""
            f = ln.split("\t")
            if len(f) >= 3 and f[0] == "livein":
                out[int(f[1], 16)] = f[2]
    return out


def raw_reads(defs):
    """{addr: count} of definitions whose bodies read an argument register directly (gabi::cpu->rN / fN)"""
    out, texts = {}, {}
    for a, ds in defs.items():
        d = ds[0]
        if d.file not in texts:
            with open(os.path.join(ROOT, d.file), encoding="utf-8", errors="replace") as fh:
                texts[d.file] = autoglue.mask(fh.read()).split("\n")
        n = len(RAW_READ.findall("\n".join(texts[d.file][d.line - 1:d.end])))
        if n:
            out[a] = n
    return out


def run_compile_audit(jobs, cache, scratch, out=None):
    """-> dict with the audit's rows and counts (see main)"""
    srcs = unit_sources()
    defs, exts = collect(srcs)
    compiled = compile_all(sorted(srcs), jobs, scratch, cache)
    errs = {k: v[1] for k, v in compiled.items() if v[1]}
    li = read_livein(PATHS["livein"])
    INDIRECT_EXCEPTIONS.clear()
    INDIRECT_EXCEPTIONS.update(read_indirect_exceptions(EXCEPTIONS))
    flow = Flow(PATHS["gen"])
    au = Audit(defs, exts, compiled, li, flow)
    rows, counts = [], collections.Counter()
    by_target = collections.defaultdict(collections.Counter)  # decompiled callee -> classes of its sites (None = exact)
    n = collections.Counter()
    for st in au.sites:
        where, rel, line, kind, ret, types, a, caller, outer = st
        if kind == "call_ptr" or a is None:
            n["indirect"] += 1
            continue
        if where == "stub" and au.stub_skip(rel, a, site_regs(types)):
            n["unused"] += 1
            continue
        n["known"] += 1
        cls, note, dec = au.classify(st)
        n["dec"] += dec
        if dec:
            by_target[a][cls] += 1
        if cls is None:
            continue
        counts[cls] += 1
        rows.append((cls, "dec" if dec else "rec", where, a, fmt_regs(site_regs(types)),
                     fmt_regs(au.D.get(a)) if dec else regs_text(li.get(a, frozenset())), "%s:%d" % (rel, line),
                     "%08X" % caller.addr if caller else "-", note))
    for cls, a, site, users, regs, note in result_audit(au, defs):
        counts[cls] += 1
        by_target[a][cls] += 1
        rows.append((cls, "dec", "result", a, "-", fmt_regs(au.D.get(a)), site, users, note))
    held = callptr_audit(au, defs, li, IndirectTargets(os.path.dirname(cache) if cache else None))
    for a, hits in sorted(held.items()):
        for site, regs, how in hits:
            counts["ind"] += 1
            d = defs[a][0]
            rows.append(("ind", "dec", "body", a, "-", fmt_regs(au.D.get(a)), "%s:%d" % (d.file, d.line), "%08X" % a,
                         "the original's indirect call at %08X may read %s, still held from entry (%s); the decompiled "
                         "call does not pass it" % (site, regs_text(regs), how)))
    exc = read_exceptions(EXCEPTIONS)
    under = au.def_under()
    raw = raw_reads(defs)
    res = dict(audit=au, rows=rows, counts=counts, by_target=by_target, under=under, exceptions=exc, raw=raw,
               errors=errs, n=n, li=li, held=held)
    if out:
        with open(out, "w") as fh:
            fh.write("# class\ttarget\twhere\taddr\tpasses\ttakes\tsite\tcaller\tnote\n")
            for r in rows:
                fh.write("%s\t%s\t%s\t%08X\t%s\t%s\t%s\t%s\t%s\n" % r)
            for a in sorted(under):
                if not defs.get(a):
                    continue  # no parsed definition (see main)
                d = defs[a][0]
                fh.write("def-under\t%s\tdef\t%08X\t%s\t%s\t%s:%d\t-\toriginal reads %s%s\n" % (
                    "excepted" if a in exc else "error", a, fmt_regs(au.D[a]), regs_text(li.get(a, frozenset())), d.file,
                    d.line, regs_text(under[a]), (": " + exc[a]) if a in exc else ""))
    return res


WRAP_CALL = re.compile(r"(?<![\w&])(~?[A-Za-z_]\w*)\s*\(")
WRAP_FN = re.compile(r"(~?[A-Za-z_]\w*)\s*\(([^;{}]*)\)\s*(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?\{")
WRAP_KW = frozenset(("if", "for", "while", "switch", "return", "sizeof", "catch", "decltype", "static_assert", "alignof"))
_WRAP_TEXT = {}


def header_wrapper(rel, line):
    """the name of the inline function of header rel whose body holds the indirect call (call_ptr / tail_ptr) on line,
    None when it cannot be told (the innermost function whose brace is still open there)"""
    if rel not in _WRAP_TEXT:
        try:
            with open(os.path.join(ROOT, rel), encoding="utf-8", errors="replace") as fh:
                _WRAP_TEXT[rel] = autoglue.mask(fh.read()).split("\n")
        except OSError:
            _WRAP_TEXT[rel] = []
    lines = _WRAP_TEXT[rel]
    if not 0 < line <= len(lines):
        return None
    m = re.search(r"\b(?:call_ptr|tail_ptr)\w*\s*<|\b(?:call_ptr|tail_ptr)\w*\s*\(", lines[line - 1])
    if not m:
        return None
    pre = "\n".join(lines[max(0, line - 13):line - 1] + [lines[line - 1][:m.start()]])
    best = None
    for fm in WRAP_FN.finditer(pre):
        if fm.group(1) in WRAP_KW:
            continue
        depth = 1
        for ch in pre[fm.end():]:
            depth += 1 if ch == "{" else -1 if ch == "}" else 0
            if depth == 0:
                break
        if depth > 0:
            best = fm.group(1)
    return best


def callptr_audit(au, defs, li, targets):
    """{definition addr: [(site, registers, how)]} (2026-11-04, buildLayoutScreen): the original of a decompiled function
    still holds an entry argument register at one of its own indirect calls (bctr/bctrl, an import), a target of that
    call may read it, and the decompiled body does not demonstrably pass it there. The decompiled calls (call_ptr /
    tail_ptr) cannot be paired with the original's by address, so a register counts as passed only when every decompiled
    indirect call of the function passes it (an ambiguous pairing is unclear, i.e. guest).
    Targets: imports by the runtime HLE's reads, bctr/bctrl by IndirectTargets (every candidate's live-in; unresolved:
    may read anything). decl_exceptions.tsv `indirect` lines remove registers checked by hand."""
    passed = collections.defaultdict(list)
    wrappers = collections.defaultdict(list)
    for st in au.sites:
        where, rel, line, kind, ret, types, a, caller, outer = st
        if kind in ("call_ptr", "tail_ptr") and where in ("body", "hdr") and caller is not None:
            passed[caller.addr].append(mask_of(site_regs(types)))
        elif kind in ("call_ptr", "tail_ptr") and where == "hdr":
            name = header_wrapper(rel, line)
            if name:
                wrappers[name].append(mask_of(site_regs(types)))
    # (2026-11-06) an indirect call inside a header's inline function (c_bg_w.h DispatchGrpThrough, hd_sys_02006294.h
    # getStateId) is compiled once per header, not per caller: it counts for every decompiled function whose body calls
    # a function of that name (each of its sites; a name shared by other functions only adds sites to the intersection)
    if wrappers:
        texts = {}
        for a, ds in defs.items():
            d = ds[0]
            if d.file not in texts:
                with open(os.path.join(ROOT, d.file), encoding="utf-8", errors="replace") as fh:
                    texts[d.file] = autoglue.mask(fh.read()).split("\n")
            body = "\n".join(texts[d.file][d.line - 1:d.end])
            for n in set(WRAP_CALL.findall(body)):
                passed[a].extend(wrappers.get(n, ()))
    known = au.flow.gen.loc
    # step B: the class of each decompiled function (its own class, or a free function's first pointer parameter) and
    # the vtables that hold a method of that class
    methods = collections.defaultdict(set)
    for a, ds in defs.items():
        if ds[0].kind in ("method", "ctor", "static") and "::" in ds[0].qname:
            methods[ds[0].qname.rsplit("::", 1)[0]].add(a)

    def cls_of(d):
        if d.kind in ("method", "ctor") and "::" in d.qname:
            return d.qname.rsplit("::", 1)[0]
        if d.ptypes and "*" in d.ptypes[0]:
            t = re.sub(r"\b(const|volatile|struct|class)\b|\*", " ", d.ptypes[0]).split()
            return t[-1] if t else None
        return None
    family_memo = {}
    if targets is not None and hasattr(targets, "children") and getattr(targets, "ok", False):
        targets.children(known)
    out = {}
    for a in sorted(defs):
        if a not in known or au.D.get(a) is None:
            continue
        common = frozenset.intersection(*passed[a]) if passed.get(a) else frozenset()
        skip = INDIRECT_EXCEPTIONS.get(a, frozenset())
        hits = []
        for site, imp, held in au.flow.indirect_held(a):
            regs = (frozenset(held) & frozenset(ARG_REGS)) - skip
            if imp is not None:
                regs &= IMPORT_READS.get(imp, frozenset(ARG_REGS))
                how = imp
            else:
                regs -= common
                cls = cls_of(defs[a][0])
                if cls is not None and cls not in family_memo:
                    family_memo[cls] = targets.vtables_holding(methods.get(cls, ())) if targets is not None else None
                fam = family_memo.get(cls) if cls is not None else None
                size = au.flow.gen.size(a) if hasattr(au.flow.gen, "size") else 0
                cands = targets.candidates(site, known, a, size, fam) if targets is not None else None
                if cands is None:
                    how = "unresolved target"
                else:
                    reads = frozenset().union(*[li.get(c, frozenset()) for c in cands])
                    regs &= reads
                    how = "%d candidate%s" % (len(cands), "s" if len(cands) != 1 else "")
            if regs:
                hits.append((site, regs, how))
        if hits:
            out[a] = hits
    return out


RESULT_REGS = (("r", 3), ("r", 4), ("f", 1))


def result_regs(ret):
    """the result registers a declared return type produces (r3; r3/r4 for 64-bit; f1 for floating point)"""
    t = re.sub(r"\b(const|volatile|static|inline|virtual|struct|class|enum)\b", " ", ret or "").strip()
    if t in ("", "void"):
        return frozenset()
    if "*" in t or "&" in t:
        return frozenset([("r", 3)])
    b = t.split()[-1]
    if b in ("f32", "f64", "float", "double"):
        return frozenset([("f", 1)])
    if b in ("u64", "s64") or t.endswith("long long"):
        return frozenset([("r", 3), ("r", 4)])
    return frozenset([("r", 3)])


def def_result(d):
    return frozenset([("r", 3)]) if d.kind == "ctor" else result_regs(d.ret)


class ResultReads:
    """which result registers the original of a decompiled function reads directly after each of its calls (2026-11-04,
    dScnLogo_Delete): backward liveness of r3/r4/f1 over its instructions; a call kills what its callee may write;
    the function's own returns read the registers its declared result produces"""

    def __init__(self, flow, defs, exit_reads=None):
        self.flow, self.defs, self.memo = flow, defs, {}
        self.exit_reads = exit_reads  # {function: registers its callers read after calling it} (fixpoint), or None

    def exit_of(self, f):
        if self.exit_reads is not None and f in self.exit_reads:
            return self.exit_reads[f]
        return def_result(self.defs[f][0]) if f in self.defs else frozenset(RESULT_REGS)

    def after(self, f, inst_only=False):
        """{callee: [registers read after each call of it]}; inst_only: read by an instruction of f itself, not only
        returned to f's callers"""
        key = (f, inst_only)
        if key in self.memo:
            return self.memo[key]
        df, out = self.flow.df, collections.defaultdict(list)
        if f not in self.flow.gen.loc:
            self.memo[key] = out
            return out
        g = df.cfg(f)
        n = len(g.insns)
        res = set(RESULT_REGS)
        rr = frozenset() if inst_only else self.exit_of(f)
        use, dfn, tail = [], [], []
        for k in range(n):
            s = g.insns[k][1]
            u, d = set(g.use[k]) & res, set(g.defs[k]) & res
            for t in g.calls[k]:
                d |= df.maydef(t) & res
            if g.indirect[k]:
                d |= res
            if s.startswith("return;") or "return;" in s:
                u |= rr
            tail.append(s.startswith("MUSTTAIL return f_"))
            use.append(u)
            dfn.append(d)
        live = [set() for _ in range(n)]
        changed = True
        while changed:
            changed = False
            for k in range(n - 1, -1, -1):
                o = set(rr) if tail[k] else set()
                for j in g.succ[k]:
                    o |= live[j]
                new = use[k] | (o - dfn[k])
                if new != live[k]:
                    live[k] = new
                    changed = True
        for k in g.reach:
            if g.calls[k]:
                o = set(rr) if tail[k] else set()
                for j in g.succ[k]:
                    o |= live[j]
                for t in g.calls[k]:
                    out[t].append(frozenset(o))
        self.memo[key] = out
        return out


def exit_reads(flow, defs, rounds=8):
    """{function: result registers its callers read directly after calling it}: a fixpoint over every function of
    build/gen, starting from the functions whose callers are unknown (their address is taken, or no direct caller): a
    decompiled one keeps its declared result, any other r3/f1 (2026-11-04)"""
    gen, df = flow.gen, flow.df
    allf = list(gen.loc)
    direct = collections.defaultdict(set)
    for f in allf:
        g = df.cfg(f)
        for k in g.reach:
            for t in g.calls[k]:
                direct[t].add(f)
    taken = set()
    for f in allf:
        for m in re.finditer(r"0x(02[0-9A-F]{6})u", gen.text(f, keep=False)):
            a = int(m.group(1), 16)
            if a in gen.loc:
                taken.add(a)
    path = PATHS["rpx"]
    if os.path.isfile(path):
        sys.path.insert(0, PATHS["tools"])
        from rpx import Rpx
        for sec in Rpx(path).sections:
            if sec.data and sec.name in (".data", ".rodata"):
                for off in range(0, len(sec.data) - 3, 4):
                    w = int.from_bytes(sec.data[off:off + 4], "big")
                    if w in gen.loc:
                        taken.add(w)

    def base(f):
        if f in taken or not direct.get(f):
            return def_result(defs[f][0]) if f in defs else frozenset([("r", 3), ("f", 1)])
        return frozenset()
    reads = {f: base(f) for f in allf}
    for _ in range(rounds):
        rr = ResultReads(flow, defs, reads)
        new = collections.defaultdict(set)
        for f in allf:
            for t, lvs in rr.after(f).items():
                for lv in lvs:
                    new[t] |= lv
        changed = False
        for f in allf:
            v = frozenset(new.get(f, ())) | base(f)
            if v != reads[f]:
                reads[f] = v
                changed = True
        if not changed:
            break
    return reads


def result_audit(au, defs, rr=None):
    """[(class, target, site, caller, registers, note)] for calls of decompiled functions whose declared result (the ext
    stub / gabi::call<R> / header wrapper) is a register the callee's definition does not produce (void, or the other
    register class). ret: the caller's original reads that register after the call (the decompiled caller uses a stale
    register: with native calls nothing stores it); ret-unread: it does not (the declaration only, tolerated)"""
    rr = rr or ResultReads(au.flow, defs, exit_reads(au.flow, defs))
    out = []
    for st in au.sites:
        where, rel, line, kind, ret, types, a, caller, outer = st
        if a is None or a not in defs or kind == "call_ptr":
            continue
        site_res = result_regs(ret)
        miss = site_res - def_result(defs[a][0])
        if not miss:
            continue
        used = True
        if where == "stub":
            if au.stub_skip(rel, a, site_regs(types)):
                continue
            users = au.stub_users(rel, a, site_regs(types))
            used = au.stub_result_used(rel, a, site_regs(types))
        else:
            users = [caller] if caller else []
        read, inst = set(), set()
        for u in users:
            for lv in rr.after(u.addr).get(a, []):
                read |= lv & miss
            for lv in rr.after(u.addr, True).get(a, []):
                inst |= lv & miss
        preserved = read - au.flow.df.maydef(a)
        note = ("the definition of %08X returns %s; the caller's original reads %s after the call%s" % (
            a, regs_text(def_result(defs[a][0])), regs_text(read),
            " (left unchanged by the callee: the value the caller had)" if preserved else "")) if read else \
            "the definition of %08X returns %s; no caller reads %s after the call" % (a, regs_text(def_result(defs[a][0])), regs_text(miss))
        if read and not used:
            note += "; the decompiled callers discard the declared result (declare it void)"
        elif read and not inst:
            note += "; only returned on to the caller's own callers (unclear)"
        out.append(("ret" if read and used and inst else ("ret?" if read and used else "ret-unread"), a, "%s:%d" % (rel, line), ",".join("%08X" % u.addr for u in users) or "-",
                    frozenset(read or miss), note))
    return out


def handoff(res, path):
    """the native-calls table: per decompiled function, whether every call into it is exact and its body takes all its
    inputs as parameters"""
    au = res["audit"]
    lines = ["# decl_lint.py hand-off table: addr, status, registers its definition takes, file, name, details",
             "# exact: every decompiled call site passes exactly the registers the definition takes, the definition takes",
             "#   every argument register its original reads, and the body reads no argument register directly",
             "# shim: as exact, except call sites that pass extra arguments nothing reads (e: drop them) or leave out",
             "#   parameters the original never reads (d: pass anything)",
             "# guest: keep the register round trip (variadic, d? or e? callee, under-declared definition, raw register reads,",
             "#   an indirect call of the original that may read an entry register the decompiled call does not pass)"]
    st = collections.Counter()
    for a in sorted(au.D):
        ds = au.defs.get(a)
        if not ds:
            continue
        d = ds[0]
        why = []
        cl = res["by_target"].get(a, collections.Counter())
        if au.varargs(a) or cl.get("c"):
            why.append("variadic")
        if cl.get("d?"):
            why.append("d? callee (%d sites)" % cl["d?"])
        if cl.get("ret?"):
            why.append("%d call sites may read a result register the definition does not produce" % cl["ret?"])
        if cl.get("e?"):
            why.append("e? callee (%d sites: an indirect call further down may read an extra argument)" % cl["e?"])
        bad = sum(v for k, v in cl.items() if k in ERROR_CLASSES)
        if bad:
            why.append("%d under-declared sites" % bad)
        if a in res["under"]:
            why.append("definition under-declared (%s)%s" % (regs_text(res["under"][a]),
                                                            " [excepted: livein over-approximation]" if a in res["exceptions"] else ""))
        if a in res["raw"]:
            why.append("%d raw argument-register reads" % res["raw"][a])
        if res.get("held", {}).get(a):
            hs = res["held"][a]
            why.append("%d indirect calls may read entry registers it does not pass (%s)" % (
                len(hs), ",".join(sorted({regs_text(r) for _, r, _ in hs}))[:60]))
        shim = {k: v for k, v in cl.items() if k in ("d", "e")}
        status = "guest" if why else ("shim" if shim else "exact")
        st[status] += 1
        lines.append("%08X\t%s\t%s\t%s\t%s\t%s" % (a, status, fmt_regs(au.D[a]), d.file, d.qname,
                                                  "; ".join(why + ["%s %d" % kv for kv in sorted(shim.items())]) or "-"))
    with open(path, "w") as fh:
        fh.write("\n".join(lines) + "\n")
    return st


def read_image(path):
    """mkimage.py's image file -> [(address, bytes)] (autoglue.image's format)"""
    import struct
    d = open(path, "rb").read()
    regs, i = [], 0
    while i < len(d):
        addr, n = struct.unpack_from("<II", d, i)
        i += 8
        regs.append((addr, d[i:i + n]))
        i += n
    return regs


def write_livein(gen_dir, out, index_cache):
    """livein_all.py's table for GEN_DIR (port mode), with the index cached outside the gen directory"""
    from funcdb import GenIndex, Dataflow
    gen = GenIndex(gen_dir, cache=index_cache)
    df = Dataflow(gen, unknown_reads_args=False)
    rows = []
    for addr in sorted(gen.addrs):
        try:
            li = df.livein(addr)
        except Exception:
            continue
        im = sum(1 << r for k, r in li if k == "r" and 3 <= r <= 10)
        fm = sum(1 << r for k, r in li if k == "f" and 1 <= r <= 8)
        if im or fm:
            rows.append("%08X\t%X\t%X\n" % (addr, im, fm))
    tmp = out + ".tmp%d" % os.getpid()
    with open(tmp, "w") as f:
        f.writelines(rows)
    os.replace(tmp, out)


def default_cache():
    base = os.environ.get("WWHD_VERIFY_SCRATCH")
    if base and os.path.isdir(base):
        return os.path.join(base, "decl_lint-%s" % hashlib_short(ROOT))
    return os.path.join(ROOT, "build", "verify", "decl_lint")


def hashlib_short(s):
    import hashlib
    return hashlib.sha1(s.encode()).hexdigest()[:10]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("-j", "--jobs", type=int, default=4, help="parallel clang -fsyntax-only processes (default 4)")
    ap.add_argument("--cache", help="cache directory (default: $WWHD_VERIFY_SCRATCH/decl_lint-<tree> or build/verify/decl_lint)")
    ap.add_argument("--out", help="every non-exact call site and definition (TSV)")
    ap.add_argument("--handoff", help="the native-calls table (TSV)")
    # port mode (the port's decomp build makes the table from its own translation at build time): every input outside
    # the sources from the given places, every output (live-in table, caches) in --out-dir; nothing written into the tree
    ap.add_argument("--gen", help="the recompiled game code (code_*.c; default build/gen)")
    ap.add_argument("--image", help="the game image autoglue reads (mkimage.py output; default build/verify/image.bin)")
    ap.add_argument("--rpx", help="the game's cking.rpx (default game/code/cking.rpx)")
    ap.add_argument("--runtime", help="the runtime whose include/ the sources compile against and whose src/ HLE bodies "
                    "give the imports' argument registers (default runtime)")
    ap.add_argument("--tools", help="the directory with rpx.py (default tools)")
    ap.add_argument("--out-dir", help="port mode: live-in table (made from --gen when missing), caches and indexes")
    a = ap.parse_args()
    for k in ("gen", "rpx", "runtime", "tools"):
        if getattr(a, k):
            PATHS[k] = os.path.abspath(getattr(a, k))
    if a.tools and PATHS["tools"] not in sys.path:
        sys.path.append(PATHS["tools"])  # autoglue's rpx.py
    if a.runtime:
        IMPORT_READS.clear()
        IMPORT_READS.update(_import_reads(PATHS["runtime"]))
        IMPORT_READS.update(_HEAP_IMPORT_READS)
    if a.image:
        IMAGE_PATH[0] = PATHS["image"] = os.path.abspath(a.image)
        autoglue._image = read_image(a.image)
    if a.out_dir:
        os.makedirs(a.out_dir, exist_ok=True)
        PATHS["livein"] = os.path.join(a.out_dir, "livein.tsv")
        PATHS["index"] = os.path.join(a.out_dir, "genindex.pkl")
        if a.image:
            PATHS["assets"] = os.path.join(os.path.abspath(a.out_dir), "include", "assets")
            _assets_paths()
        if not os.path.isfile(PATHS["livein"]):
            write_livein(PATHS["gen"], PATHS["livein"], os.path.join(a.out_dir, "genindex.pkl"))
    cache = a.cache or (os.path.join(a.out_dir, "decl_lint") if a.out_dir else default_cache())
    os.makedirs(cache, exist_ok=True)
    res = run_compile_audit(a.jobs, os.path.join(cache, "sites.json"), os.path.join(cache, "tu"), a.out)
    n, counts = res["n"], res["counts"]
    print("decl_lint: %d guest calls with a static target (%d to decompiled functions), %d indirect, %d ext stubs that "
          "make no call" % (n["known"], n["dec"], n["indirect"], n["unused"]))
    for c in ERROR_CLASSES + TOLERATED:
        if counts.get(c):
            print("  %-8s %5d%s" % (c, counts[c], "  (tolerated)" if c in TOLERATED else ""))
    under = res["under"]
    exc = res["exceptions"]
    err_def = sorted(x for x in under if x not in exc)
    print("  definitions taking fewer argument registers than their original reads: %d (%d excepted in %s)" % (
        len(under), len(under) - len(err_def), os.path.relpath(EXCEPTIONS, ROOT)))
    if a.handoff:
        st = handoff(res, a.handoff)
        print("  hand-off: %s" % ", ".join("%s %d" % kv for kv in sorted(st.items())))
    bad = sum(counts.get(c, 0) for c in ERROR_CLASSES) + len(err_def)
    for r in res["rows"]:
        if r[0] in ERROR_CLASSES:
            print("decl_lint: %s %s %08X passes %s, takes %s (%s) %s" % (r[6], r[0], r[3], r[4], r[5], r[7], r[8]))
    for x in err_def:
        if not res["audit"].defs.get(x):  # no parsed definition (a translation the sources do not match): address only
            print("decl_lint: %08X takes %s, its original reads %s" % (x, fmt_regs(res["audit"].D.get(x)),
                                                                     regs_text(res["li"].get(x, frozenset()))))
            continue
        d = res["audit"].defs[x][0]
        print("decl_lint: %s:%d %s (%08X) takes %s, its original reads %s" % (
            d.file, d.line, d.qname, x, fmt_regs(res["audit"].D[x]), regs_text(res["li"].get(x, frozenset()))))
    if res["errors"]:
        for k, v in sorted(res["errors"].items()):
            print("decl_lint: %s: %s" % (k, v.splitlines()[0] if v else ""))
        return 2
    print("decl_lint: %s" % ("ok" if not bad else "%d under-declared" % bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())

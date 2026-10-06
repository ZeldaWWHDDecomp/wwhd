#!/usr/bin/env python3
"""Draft bindings for WWHD functions: one `inline` C++ wrapper per address, typed from the
GameCube signature (build/wwhd_to_gc.tsv) and checked against what the WWHD code reads.

usage: mkbind.py ADDR [ADDR...]          print drafts
       mkbind.py --callees ADDR...       drafts for every callee of these functions that has no
                                         binding yet (grep of wwhd_src/include)

A draft is a starting point. The harness checks it: a wrong argument register shows up as a
call mismatch. Lines are marked:
  // regs: r3 r4 f1      argument registers the WWHD code reads (live-in analysis)
  // !! ...              the GameCube signature and the WWHD registers disagree (HD signature
                         change, or a wrong matcher name)
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
from funcdb import GenIndex, load_names, signature, arg_regs, Dataflow, RetWidth, CALL_RE  # noqa: E402

CTYPE = {"bool": "bool", "char": "s8", "schar": "s8", "uchar": "u8", "short": "s16", "sshort": "s16",
         "ushort": "u16", "int": "s32", "sint": "s32", "uint": "u32", "long": "s32", "slong": "s32", "ulong": "u32",
         "float": "f32", "double": "f64", "longlong": "s64", "ulonglong": "u64", "wchar": "u16"}


def ctype(t):
    if t.kind in ("ptr", "ref"):
        i = t.inner
        if i is None:
            return "void*"
        if i.kind == "class":
            base = i.name.replace("::", "_")
            return ("const " if i.const else "") + base + "*"
        if i.kind in ("int", "float"):
            return ("const " if i.const else "") + "be<%s>*" % CTYPE.get(i.name, "u32")
        if i.kind == "array":
            return "Mtx34*" if i.name.startswith("float[4]") else "void*"
        if i.kind == "func":
            return "u32 /*fn*/"
        if i.kind == "ptr":
            return "u32* /*ptr to ptr*/"
        return "void*"
    if t.kind == "class":
        return t.name.replace("::", "_") + " /*by value!*/"
    if t.kind in ("int", "float"):
        return CTYPE.get(t.name, "u32")
    return "u32"


def cname(nm):
    nm = nm.split("<")[0]
    return re.sub(r"\W", "_", nm.replace("::", "_"))


def draft(a, gen, names, gc, df, rw):
    nm = names.get(a, ("", "", ""))
    sym = gc.get(a, ("", ""))[0]
    sig = signature(sym) if sym else None
    li = df.livein(a) if a in gen.loc else set()
    regs = sorted((k, r) for k, r in li)
    regtxt = " ".join("%s%d" % kr for kr in regs)
    w = rw.width(a) if a in gen.loc else None
    if w is None:
        ret = "void /*result type unknown: s32, pointer or float? check the callers*/"
    elif w[0] == "const":
        ret = "s32 /*constant %d*/" % w[1]
    else:
        ret = {("u", 1): "bool", ("u", 8): "u8", ("s", 8): "s8", ("u", 16): "u16", ("s", 16): "s16"}.get((w[0], w[1]), "s32")
    fn = cname(nm[0]) if nm[0] else "f_%08X" % a
    params, args = [], []
    note = ""
    if sig:
        method, ps = sig
        if method:
            cls = nm[0].rsplit("::", 1)[0] if "::" in nm[0] else "void"
            params.append("%s* self" % cname(cls))
            args.append("self")
        for i, t in enumerate(ps):
            if t.kind == "vararg":
                params.append("/*...*/")
                break
            params.append("%s a%d" % (ctype(t), i))
            args.append("a%d" % i)
        ints, flts, _ = arg_regs(method, ps)
        want = {("r", r) for r in ints} | {("f", f) for f in flts}
        extra = sorted(set(li) - want)
        if extra:
            note = " // !! WWHD also reads %s (HD signature?)" % " ".join("%s%d" % kr for kr in extra)
    else:
        n_r = [r for k, r in regs if k == "r"]
        n_f = [r for k, r in regs if k == "f"]
        for r in n_r:
            params.append("u32 r%d" % r)
            args.append("r%d" % r)
        for f in n_f:
            params.append("f32 f%d" % f)
            args.append("f%d" % f)
        note = " // !! no GameCube signature: types from registers"
    if "f1" in regtxt and not sig and w is None:
        pass
    call_r = ret.split(" ")[0]
    if ret.startswith("void /*"):
        note += " // result type unknown (GameCube mangling has none)"
    body = "gabi::call%s(0x%08X%s)" % ("" if call_r == "void" else "<%s>" % call_r, a, "".join(", " + x for x in args))
    if call_r != "void":
        body = "return " + body
    head = "/* %08X %s (%s, %s) regs: %s */" % (a, nm[0] or "?", nm[1] or "?", nm[2] or "-", regtxt or "-")
    return "%s\ninline %s %s(%s) { %s; }%s" % (head, call_r, fn, ", ".join(params), body, note)


def bound_addrs():
    out = set()
    for dp, _, fs in os.walk(os.path.join(ROOT, "wwhd_src", "include")):
        for f in fs:
            for m in re.finditer(r"0x([0-9A-Fa-f]{8})", open(os.path.join(dp, f)).read()):
                out.add(int(m.group(1), 16))
    return out


def main():
    args = sys.argv[1:]
    gen = GenIndex(os.path.join(ROOT, "build", "gen"))
    names, gc = load_names(os.path.join(ROOT, "build"))
    df = Dataflow(gen, unknown_reads_args=False)
    rw = RetWidth(Dataflow(gen))
    if args and args[0] == "--callees":
        have = bound_addrs()
        targets = set()
        for x in args[1:]:
            for m in CALL_RE.finditer(gen.text(int(x, 16))):
                targets.add(int(m.group(1), 16))
        addrs = sorted(t for t in targets if t not in have)
    else:
        addrs = [int(x, 16) for x in args]
    for a in addrs:
        print(draft(a, gen, names, gc, df, rw))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Build a verification unit: the recompiled originals of the functions a candidate source
implements, isolated from the rest of the game.

usage: mkunit.py UNIT [--root .] [--out build/verify/UNIT]

Reads tools/verify/units/UNIT.txt:
    src  wwhd_src/...cpp        candidate sources (VERIFY(0xADDR, fn) marks what they implement)
    real ADDR                   link this callee's real code instead of a mock (pure helpers)
    field ADDR|* rN+OFF TYPE LO [HI]   steer generated inputs (read by the harness, -spec)

Writes OUT/unit.c: each tested function's generated C (renamed orig_X, memory accesses through
the harness, basic-block coverage points), real callees (real_X), a stub for every other guest
function or import they reference (records the call: vm_call), and the callee table (which
argument registers to compare, which registers a mock may write, pointer argument sizes).
"""
import argparse
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from funcdb import (GenIndex, load_names, signature, arg_regs, stack_words, Dataflow, RetWidth, CALL_RE, INT_ARGS, FLT_ARGS)
from extent_lint import extent  # noqa: E402

IMP_RE = re.compile(r"\b(imp_\w+)\(c\)")
SITE_RE = re.compile(r"\b(site_[0-9A-F]{8})\(c\)")


def read_unit(path):
    u = {"src": [], "real": []}
    for line in open(path):
        line = line.split("#")[0].split()
        if not line:
            continue
        if line[0] == "src":
            u["src"] += line[1:]
        elif line[0] == "real":
            u["real"] += [int(x, 16) for x in line[1:]]
        elif line[0] == "noret":  # noret FUNC # reason: a proven false positive of ret_values.tsv
            u.setdefault("noret", set()).update(int(x, 16) for x in line[1:])
        elif line[0] == "retclass":  # retclass FUNC # reason: a proven exception to the return-register class lint
            u.setdefault("retclass", set()).update(int(x, 16) for x in line[1:])
        elif line[0] == "pointee":
            for kv in line[2:]:
                k, v = kv.split("=")
                u.setdefault("pointee", {})[(int(line[1], 16), k)] = int(v, 0)
        elif line[0] == "callee":
            for kv in line[2:]:
                k, v = kv.split("=")
                if v.startswith("inout"):  # rN=inout12: compared at the call, and generated mocks fill it afterwards
                    u.setdefault("fill", set()).add((int(line[1], 16), int(k[1:])))
                    v = v[5:].lstrip(":")
                elif v.startswith("out"):  # rN=out12: output-only storage of 12 bytes
                    u.setdefault("outonly", set()).add((int(line[1], 16), int(k[1:])))
                    v = v[3:].lstrip(":")
                u.setdefault("over", {})[(int(line[1], 16), k)] = int(v, 0)
    return u


def instrument(text, new_name, cov):
    """rename the function, add coverage points at block starts"""
    lines = text.split("\n")
    out = []
    nb = 0
    m = re.match(r"void (f_[0-9A-F]{8}(?:_orig)?)\(", lines[0])
    out.append("void %s(Cpu* __restrict c) {" % new_name)
    pending = cov
    if cov:
        out.append("    VM_COV(0, %d);" % nb)
        nb += 1
    for ln in lines[1:]:
        # a hooked neighbour (runtime hook or recording tap) is reached as f_X_orig: same function
        ln = re.sub(r"\bf_([0-9A-F]{8})_orig\(c\)", r"f_\1(c)", ln)
        out.append(ln)
        if not cov:
            continue
        s = ln.strip()
        if ln.startswith("L_"):
            out.append("    VM_COV(0, %d);" % nb)
            nb += 1
        elif s.startswith("if (") or s.startswith("c->ctr--; if ("):
            out.append("    VM_COV(0, %d);" % nb)
            nb += 1
    return "\n".join(out), nb


def pointee_size(t):
    if t is None or t.kind not in ("ptr", "ref") or t.inner is None:
        return 0
    i = t.inner
    if i.kind in ("int", "float"):
        return i.size
    if i.kind == "class":
        return i.size
    if i.kind == "array":  # Mtx: float[4]* (row pointer)
        return 48 if i.name.startswith("float[4]") else (i.size or 0)
    return 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit")
    ap.add_argument("--root", default=".")
    ap.add_argument("--out")
    a = ap.parse_args()
    root = os.path.abspath(a.root)
    build = os.path.join(root, "build")
    out = a.out or os.path.join(build, "verify", a.unit)
    os.makedirs(out, exist_ok=True)
    unit = read_unit(os.path.join(root, "tools", "verify", "units", a.unit + ".txt"))
    tested = []
    for s in unit["src"]:
        for m in re.finditer(r"^\s*VERIFY\(\s*0x([0-9A-Fa-f]{8})\s*,", open(os.path.join(root, s)).read(), re.M):
            tested.append(int(m.group(1), 16))
    tested = sorted(set(tested))
    gen = GenIndex(os.path.join(build, "gen"))
    names, gc = load_names(build)
    # known matcher errors (tools/verify/matcher_errors.tsv): their GameCube signature is not used
    # and HD signature changes (tools/verify/hd_signatures.tsv): compare only what the WWHD code reads
    for mepath in (os.path.join(root, "tools", "verify", "matcher_errors.tsv"), os.path.join(root, "tools", "verify", "hd_signatures.tsv")):
        if not os.path.exists(mepath):
            continue
        for line in open(mepath):
            f = line.split("\t")
            if re.match(r"[0-9A-F]{8}$", f[0]):
                gc.pop(int(f[0], 16), None)
    df = Dataflow(gen)
    df_lo = Dataflow(gen, unknown_reads_args=False)  # registers the code itself reads (no guesses for indirect calls)
    rw = RetWidth(df)
    imports = {}
    ij = os.path.join(build, "gen", "imports.json")
    for e in json.load(open(ij)):
        if e["kind"] == "f":
            ident = "imp_%s_%s" % (re.sub(r"[^A-Za-z0-9_]", "_", e["lib"].replace(".rpl", "")), re.sub(r"[^A-Za-z0-9_]", "_", e["name"]))
            imports[ident] = (e["slot"], e["name"])

    body = ['/* generated by tools/verify/mkunit.py: unit %s (recompiled originals; build output, do not commit) */' % a.unit,
            '#include "vm_gen.h"', '#include "unit.h"', ""]
    callees, imps, sites = set(), set(), set()
    funcs_c, origs = [], []
    for addr, prefix in [(x, "orig") for x in tested] + [(x, "real") for x in unit["real"]]:
        if addr not in gen.loc:
            sys.exit("no generated function at %08X" % addr)
        text = gen.text(addr)
        for m in CALL_RE.finditer(text):
            callees.add(int(m.group(1), 16))
        imps |= set(IMP_RE.findall(text))
        sites |= set(SITE_RE.findall(text))
        t, nb = instrument(text, "%s_%08X" % (prefix, addr), prefix == "orig")
        funcs_c.append(t)
        if prefix == "orig":
            origs.append((addr, nb))
    real = set(unit["real"])
    callees |= real
    overrides = unit.get("over", {})
    # Include known dynamic-only targets with explicit unit callee facts.
    callees.update(target for target, _ in overrides if target in gen.loc)
    # global callee facts (tools/verify/callee_facts.tsv, same syntax as unit `callee` lines):
    # defaults for every unit that calls the target; a unit's own line for the same key wins
    overrides = dict(overrides)
    cfpath = os.path.join(root, "tools", "verify", "callee_facts.tsv")
    if os.path.exists(cfpath):
        known = callees | {imports[x][0] for x in imps if x in imports}  # also import slots (e.g. C0006178 GX2DrawIndexedEx) take facts too
        for line in open(cfpath):
            f = line.split("#")[0].split()
            if len(f) < 2 or not re.match(r"[0-9A-Fa-f]{8}$", f[0]) or int(f[0], 16) not in known:
                continue
            for kv in f[1:]:
                k, v = kv.split("=")
                key = (int(f[0], 16), k)
                if v.startswith("inout"):  # rN=inoutN: compared, then filled by generated mocks (kept even under a unit line)
                    v = v[5:].lstrip(":")
                    unit.setdefault("fill", set()).add((key[0], int(k[1:])))
                elif v.startswith("out"):  # rN=outN: output-only storage, as in unit `callee` lines
                    v = v[3:].lstrip(":")
                    unit.setdefault("fill", set()).add((key[0], int(k[1:])))  # the fill also applies under a unit line
                    if key not in overrides:
                        unit.setdefault("outonly", set()).add((key[0], int(k[1:])))
                overrides.setdefault(key, int(v, 0))
        # guard (2026-10-05): a unit line may not declare a smaller object than the global fact for the
        # same callee/register: unit lines win, and too-small unit sizes hid undersized gabi::Locals
        # (m_Do_ext 0x1C vs 0x40, d_operate_wind out8 vs out12)
        glob = {}
        for line in open(cfpath):
            f = line.split("#")[0].split()
            if len(f) < 2 or not re.match(r"[0-9A-Fa-f]{8}$", f[0]):
                continue
            for kv in f[1:]:
                k, v = kv.split("=")
                if re.match(r"r\d+$", k):
                    glob[(int(f[0], 16), k)] = int(re.sub(r"^(inout|out):?", "", v), 0)
        bad = []
        for src in ("over", "pointee"):
            for key, size in unit.get(src, {}).items():
                g = glob.get(key)
                if g and g < 0xFFFE and size < g:
                    bad.append("%s %08X %s=%d (global callee_facts.tsv: %d)" % ("callee" if src == "over" else "pointee", key[0], key[1], size, g))
        if bad:
            sys.exit("mkunit: unit file declares a smaller object than the global fact (unit lines may only be equal or larger):\n  " + "\n  ".join(bad))
    decls = ["void f_%08X(Cpu* c);" % x for x in sorted(callees)]
    decls += ["void %s(Cpu* c);" % x for x in sorted(imps)]
    decls += ["void %s(Cpu* c);" % x for x in sorted(sites)]
    decls += ["void real_%08X(Cpu* c);" % x for x in sorted(real)]
    body += decls + [""] + funcs_c + [""]
    body.append("/* calls out of the unit: recorded by the harness */")
    for x in sorted(callees):
        body.append("void f_%08X(Cpu* c) { vm_call(c, 0x%08Xu, VM_CALL_DIRECT); }" % (x, x))
    for x in sorted(imps):
        slot = imports.get(x, (0, x))[0]
        body.append("void %s(Cpu* c) { vm_call(c, 0x%08Xu, VM_CALL_IMPORT); }" % (x, slot))
    for x in sorted(sites):
        body.append("void %s(Cpu* c) { (void)c; } /* runtime instruction hook: not part of the game's code */" % x)

    def mask(regs):
        m = 0
        for r in regs:
            m |= 1 << r
        return m

    rows = []
    import_args = {}
    iap = os.path.join(root, "tools", "verify", "import_args.tsv")
    if os.path.exists(iap):
        for line in open(iap):
            f = line.split("#")[0].split("\t")
            if len(f) >= 4 and re.match(r"[0-9A-F]{8}$", f[0]):
                import_args[int(f[0], 16)] = (int(f[2], 16), int(f[3], 16))
    allc = sorted(callees) + [imports[x][0] for x in sorted(imps) if x in imports]
    for x in allc:
        is_imp = x not in gen.loc
        nm = names.get(x, ("",))[0] if not is_imp else next(v[1] for k, v in imports.items() if v[0] == x)
        sym = gc.get(x, ("",))[0]
        sig = signature(sym) if sym else None
        ptrsz = [0] * 11
        outp = [0] * 11
        declared = 0
        nstack = 0
        rk, rb, rc = 0, 0, 0
        if is_imp:
            # the import's own argument registers (import_args.tsv: HLE implementation / mangled signature),
            # compared at every call whatever the candidate declares (game test: GX2CallDisplayList without r4)
            im, fm = import_args.get(x, (0, 0))
            declared = 1
            defr, deff = 0xFFFFFFFF, 0xFFFFFFFF
        else:
            w = rw.width(x)
            if w:
                rk, rb, rc = {"u": (1, w[1], 0), "s": (2, w[1], 0), "const": (3, 32, w[1])}[w[0]]
            li = df.livein(x)
            md = df.maydef(x)
            defr = mask(r for k, r in md if k == "r")
            deff = mask(r for k, r in md if k == "f")
            # compared registers: what the callee's code reads (sound unless it reads them only
            # through indirect calls) plus the GameCube signature's arguments (the matcher can be
            # wrong about the name, so the signature never narrows the set)
            if df.cfg(x).varargs:
                declared = 1  # variadic: also compare what the candidate passes
            lo = df_lo.livein(x)
            im = mask(r for k, r in lo if k == "r")
            fm = mask(r for k, r in lo if k == "f")
            if sig:
                method, params = sig
                ints, flts, types = arg_regs(method and ("r", 3) in li, params)
                im |= mask(ints)
                fm |= mask(flts)
                nstack = stack_words(method and ("r", 3) in li, params)
                for reg, t in types.items():
                    if reg[0] == "r":
                        n = int(reg[1:])
                        ptrsz[n] = pointee_size(t)
                        outp[n] = int(t.kind in ("ptr", "ref") and t.inner is not None and not t.inner.const)
        rl = "real_%08X" % x if x in real else "0"
        # a constructor initialises its object: what was in the storage before is not an input
        if sym.startswith("__ct__") or nm.endswith("::ct") or nm.endswith("_ct"):
            ptrsz[3] = 255
        nstack = max(nstack, overrides.get((x, "stack"), 0))
        if (x, "retu") in overrides:  # reviewed return fact: unsigned, N significant bits (e.g. retu=1 for a 0/1 bool)
            rk, rb, rc = 1, overrides[(x, "retu")], 0
        for reg in range(3, 11):
            if (x, "r%d" % reg) in overrides:
                ptrsz[reg] = overrides[(x, "r%d" % reg)] or 0xFFFE  # explicit rN=0: a scalar (0xFFFE = no pointee compare)
                im |= 1 << reg  # a fact about rN declares rN an argument (compared at the call), e.g. `r5=0`
        # Storage size is distinct from a claim that the callee consumes that register.
        for reg in range(3, 11):
            if (x, "r%d" % reg) in unit.get("pointee", {}):
                ptrsz[reg] = unit["pointee"][(x, "r%d" % reg)]
        outonly = 0
        for (fa, reg) in unit.get("outonly", set()):
            if fa == x:
                outonly |= 1 << reg
                outp[reg] = 1
        fill = outonly  # generated mocks fill outN and inoutN storage
        for (fa, reg) in unit.get("fill", set()):
            if fa == x:
                fill |= 1 << reg
                outp[reg] = 1
        # static extent (extent_lint.py): how far the callee's code reaches through each argument register;
        # the harness's undersized-Local check uses it like a size fact (game test 2026-10-05)
        ext = [0] * 11
        if not is_imp:
            for reg in range(3, 11):
                if im >> reg & 1:
                    ext[reg] = min(extent(x, "r%d" % reg)[0], 0xFFFD)
        rows.append('    {0x%08Xu, "%s", 0x%Xu, 0x%Xu, 0x%Xu, 0x%Xu, {%s}, {%s}, %s, %d, %d, %d, 0x%Xu, %d, 0x%Xu, {%s}, 0x%Xu},' % (
            x, nm.replace('"', ""), im, fm, defr, deff, ",".join(map(str, ptrsz)), ",".join(map(str, outp)), rl,
            declared, rk, rb, rc, nstack, outonly, ",".join(map(str, ext)), fill))
    body.append("\nconst CalleeInfo unit_callees[] = {\n%s\n    {0}\n};" % "\n".join(rows))
    body.append("const unsigned unit_ncallees = %d;" % len(rows))
    # return values (ret_values.tsv, from the original's callers): compared at return even when the
    # candidate is declared void, and a void declaration of such a function is rejected (game test:
    # daNpc_Bj1_c::demo 021F563C returns a BOOL its callers test; the void candidate dropped it)
    retvals = {}
    rvp = os.path.join(root, "tools", "verify", "ret_values.tsv")
    if os.path.exists(rvp):
        for line in open(rvp):
            f = line.split("#")[0].split("\t")
            if len(f) >= 2 and re.match(r"[0-9A-F]{8}$", f[0]):
                regs = f[1].strip().split(",")
                retvals[int(f[0], 16)] = (1 if "r3" in regs else 0) | (2 if "f1" in regs else 0) | (4 if "r4" in regs else 0)
    noret = unit.get("noret", set())
    retok = unit.get("retclass", set())
    voidbad = []

    def decl_class(t):  # register class of a declared return type
        t = re.sub(r"\s+", "", t).replace("gabi::", "")
        if t == "void":
            return "void"
        if t in ("f32", "f64", "float", "double"):
            return "f"
        if t in ("Pair32", "u64", "s64", "uint64_t", "int64_t", "unsignedlonglong", "longlong", "SxyzResult"):  # SxyzResult: RET_AGG6 (c_sxyz)
            return "p"
        return "i"

    def table_class(rr):
        return "p" if rr & 5 == 5 else "f" if rr & 2 else "i" if rr & 1 else None

    origset = dict(origs)
    for sp in unit["src"]:
        p = os.path.join(root, sp)
        if not os.path.exists(p):
            continue
        for m in re.finditer(r"WWHD_FUNC\(\s*0x([0-9A-Fa-f]{8})\s*,\s*([^,()]+(?:<[^>]*>)?[^,()]*?)\s*[,)]", open(p, errors="replace").read()):
            fa = int(m.group(1), 16)
            rr = retvals.get(fa)
            if not rr or fa in noret or fa not in origset:
                continue
            dc, tc = decl_class(m.group(2)), table_class(rr)
            if dc == "void":
                voidbad.append("%08X (%s, %s): declared void, original callers read %s" % (fa, names.get(fa, ("",))[0] or "?", sp, tc))
            elif dc != tc and not (dc == "p" and tc == "i") and fa not in retok:
                voidbad.append("%08X (%s, %s): declared %s (%s register), original callers read %s" % (
                    fa, names.get(fa, ("",))[0] or "?", sp, m.group(2).strip(), {"i": "r3", "f": "f1", "p": "r3:r4"}[dc],
                    {"i": "r3", "f": "f1", "p": "r3:r4"}[tc]))
    if voidbad:
        msg = ("mkunit: return type of a function whose result the original's callers use (tools/verify/ret_values.tsv) is void or"
               " in the wrong register class; declare the type the callers read, or add `noret FUNC # reason` /"
               " `retclass FUNC # reason` for a proven exception:\n  " + "\n  ".join(voidbad))
        if os.environ.get("WWHD_RETLINT") == "warn":
            print(msg.replace("mkunit:", "mkunit (warning):"), file=sys.stderr)
        else:
            sys.exit(msg)
    rows = []
    for addr, nb in origs:
        sym = gc.get(addr, ("",))[0]
        sig = signature(sym) if sym else None
        argt = ["x"] * 11
        nflt = 0
        li = df.livein(addr)
        if sig:
            method, params = sig
            ints, flts, types = arg_regs(method, params)
            for reg, t in types.items():
                if reg[0] == "r":
                    n = int(reg[1:])
                    if t.kind in ("ptr", "ref"):
                        argt[n] = "p"
                    elif t.name in ("short", "sshort"):
                        argt[n] = "h"
                    elif t.name == "ushort":
                        argt[n] = "H"
                    elif t.name in ("char", "schar"):
                        argt[n] = "c"
                    elif t.name in ("uchar", "bool"):
                        argt[n] = "b"
                    elif t.kind == "int":
                        argt[n] = "i"
            nflt = len(flts)
        rr = 0 if addr in noret else retvals.get(addr, 0)
        w = rw.width(addr) if rr & 1 else None
        rbits = w[1] if w and w[0] in ("u", "s") and w[1] in (8, 16) else 32
        rows.append('    {0x%08Xu, "%s", "%s", orig_%08X, %d, "%s", %d, 0x%Xu, 0x%Xu, %d, %d},' % (
            addr, names.get(addr, ("",))[0], sym, addr, nb, "".join(argt), nflt,
            mask(r for k, r in li if k == "r"), mask(r for k, r in li if k == "f"), rr, rbits))
    body.append("\nconst OrigFunc unit_funcs[] = {\n%s\n};" % "\n".join(rows))
    body.append("const unsigned unit_nfuncs = %d;" % len(rows))
    body.append('const char unit_name[] = "%s";' % a.unit)
    open(os.path.join(out, "unit.c"), "w").write("\n".join(body) + "\n")
    print("unit %s: %d functions under test, %d callees, %d imports -> %s" % (a.unit, len(origs), len(callees), len(imps), out))


if __name__ == "__main__":
    main()

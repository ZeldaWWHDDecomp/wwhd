#!/usr/bin/env python3
"""Build a verification unit: the recompiled originals of the functions a candidate source
implements, isolated from the rest of the game.

usage: mkunit.py UNIT [--root .] [--out build/verify/UNIT] [--split N] [--split-auto BYTES]

Reads tools/verify/units/UNIT.txt:
    src  wwhd_src/...cpp        candidate sources (VERIFY(0xADDR, fn) marks what they implement)
    real ADDR                   link this callee's real code instead of a mock (pure helpers)
    field ADDR|* rN+OFF TYPE LO [HI]   steer generated inputs (read by the harness, -spec)

Writes OUT/unit.c: each tested function's generated C (renamed orig_X, memory accesses through
the harness, basic-block coverage points), real callees (real_X), a stub for every other guest
function or import they reference (records the call: vm_call), and the callee table (which
argument registers to compare, which registers a mock may write, pointer argument sizes).

Split (2026-11-02, verify.py `unitsplit`): --split N (N > 1), or --split-auto BYTES when unit.c is larger than BYTES
(N = ceil(size / (BYTES/2))), also writes OUT/unit_shared.h (the includes and every declaration, plus the orig_X
prototypes the function table needs), OUT/unit.pK.c (K = 0..N-1: contiguous runs of the generated functions, balanced by
size; the call stubs and the tables in the last part, all of them extern as in unit.c) and the manifest OUT/unit.parts
(their names, one per line). verify.py then compiles the parts instead of unit.c (each TU a fraction of unit.c's compiler
memory). unit.c itself is always written, byte for byte as without a split (triage.py and others read it). Without a
split the part files and the manifest are removed, so a stale manifest is never used.
"""
import argparse
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import autoglue  # noqa: E402
import specfile  # noqa: E402
from funcdb import (GenIndex, load_names, signature, arg_regs, stack_words, Dataflow, RetWidth, CALL_RE, INT_ARGS, FLT_ARGS)
from extent_lint import extent  # noqa: E402

IMP_RE = re.compile(r"\b(imp_\w+)\(c\)")
SITE_RE = re.compile(r"\b(site_[0-9A-F]{8})\(c\)")


def read_unit(path):
    u = {"src": [], "real": []}
    for line in specfile.expanded_lines(path):
        line = line.split("#")[0].split()
        if not line:
            continue
        if line[0] == "src":
            u["src"] += line[1:]
        elif line[0] == "real":
            u["real"] += [int(x, 16) for x in line[1:]]
        elif line[0] == "importvoid":  # importvoid SLOT...: the port's HLE of this import is void (defr = deff = 0 below)
            u.setdefault("importvoid", set()).update(int(x, 16) for x in line[1:])
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
                if kv == "fixture":  # a mock-only target the game code does not contain (a fixture's callback slot)
                    u.setdefault("fixture", set()).add(int(line[1], 16))
                    continue
                k, v = kv.split("=")
                cm = re.match(r"cstr(?::(\w+))?$", v)
                if cm:  # rN=cstr[:MAX]: a NUL-terminated string, compared through its first NUL (scan at most MAX, default 4096)
                    u.setdefault("dyn", {})[(int(line[1], 16), int(k[1:]))] = (0, 0xFF)
                    u.setdefault("heapref", set()).add((int(line[1], 16), int(k[1:])))  # strings live on the stack, heap or in rodata
                    v = cm.group(1) or "4096"
                dm = re.match(r"dyn(\d+)\*r(\d+)(?::(\w+))?$", v)
                if dm:  # rN=dyn4*r4[:CAP]: compare 4*r4 pointee bytes at each call (at most CAP, default 4096)
                    u.setdefault("dyn", {})[(int(line[1], 16), int(k[1:]))] = (int(dm.group(1)), int(dm.group(2)))
                    v = dm.group(3) or "4096"
                if v.startswith("heap"):  # rN=heap48: also compare 48 pointee bytes when rN is a heap (non-stack) pointer
                    u.setdefault("heapref", set()).add((int(line[1], 16), int(k[1:])))
                    v = v[4:].lstrip(":")
                elif v.startswith("inout"):  # rN=inout12: compared at the call, and generated mocks fill it afterwards
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


_CTOR_SIZES = None


def ctor_sizes():
    """tools/verify/ctor_sizes.tsv: constructor address -> sizeof (bytes)"""
    global _CTOR_SIZES
    if _CTOR_SIZES is None:
        _CTOR_SIZES = {}
        p = os.path.join(os.path.dirname(os.path.abspath(__file__)), "ctor_sizes.tsv")
        if os.path.exists(p):
            for line in open(p):
                f = line.split()
                if len(f) >= 2 and not line.startswith("#"):
                    _CTOR_SIZES[int(f[0], 16)] = int(f[1], 16)
    return _CTOR_SIZES


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


HELPER_WORD = re.compile(r"/\* [0-9A-F]{8}: ([0-9A-F]{8}) \*/")
HELPER_TAIL = re.compile(r"MUSTTAIL return f_([0-9A-F]{8})\(c\);")


def ghs_helper_word(w):
    """an instruction of the GHS register save/restore helpers: r11-relative GPR/FPR/paired-single stores and loads,
    ps_merge10, mflr/mtlr r0 (or mflr r31), isync, mr r1,r11 / ori r1,r11,0, blr"""
    op, ra = w >> 26, (w >> 16) & 31
    if op in (32, 36, 48, 50, 52, 54, 56, 60) and ra == 11:  # lwz stw lfs lfd stfs stfd psq_l psq_st (r11)
        return True
    if op == 4 and (w & 0x7FF) == 0x4A0:  # ps_merge10
        return True
    return w in (0x7C0802A6, 0x7FE802A6, 0x7C0803A6, 0x4C00012C, 0x7D615B78, 0x61610000, 0x4E800020)


def ghs_helper_chains(gen, funcs):
    """addresses of the helper chains called by FUNCS (each entry plus the entries it falls through to)"""
    out = set()
    for f in funcs:
        if f not in gen.loc:
            continue
        for m in CALL_RE.finditer(gen.text(f)):
            a = int(m.group(1), 16)
            while a in gen.loc and a not in out:
                text = gen.text(a)
                words = [int(x, 16) for x in HELPER_WORD.findall(text)]
                if 0x4E800020 in words:  # the generated text runs on past the helper's blr (padding, next entries)
                    words = words[:words.index(0x4E800020) + 1]
                # a helper entry saves or loads at least one register through r11 (an empty `blr` function is not one)
                if not any((w >> 26) in (32, 36, 48, 50, 52, 54, 56, 60) and ((w >> 16) & 31) == 11 for w in words) \
                        or not all(ghs_helper_word(w) for w in words):
                    break
                out.add(a)
                t = HELPER_TAIL.search(text)
                if not t:
                    break
                a = int(t.group(1), 16)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit")
    ap.add_argument("--root", default=".")
    ap.add_argument("--out")
    ap.add_argument("--split", type=int, default=0, help="write unit.c also as N translation units (N > 1)")
    ap.add_argument("--split-auto", type=int, default=0, help="split when unit.c exceeds BYTES (0: never)")
    a = ap.parse_args()
    root = os.path.abspath(a.root)
    build = os.path.join(root, "build")
    out = a.out or os.path.join(build, "verify", a.unit)
    os.makedirs(out, exist_ok=True)
    unit = read_unit(os.path.join(root, "tools", "verify", "units", a.unit + ".txt"))
    tested = []
    for s in unit["src"]:
        text = open(os.path.join(root, s)).read()
        for m in re.finditer(r"^\s*VERIFY\(\s*0x([0-9A-Fa-f]{8})\s*,", text, re.M):
            tested.append(int(m.group(1), 16))
        glued = autoglue.bodies(text)  # GameCube-style source: functions marked /* ADDR */
        tested += [a for a, _, _ in glued or []]
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
    df_lo = Dataflow(gen, unknown_reads_args=False, cfgs=df.cfgs)  # registers the code itself reads (no guesses for indirect
    # calls); the Cfgs are the same for both, so they are built once
    rw = RetWidth(df)
    imports = {}
    ij = os.path.join(build, "gen", "imports.json")
    for e in json.load(open(ij)):
        if e["kind"] == "f":
            ident = "imp_%s_%s" % (re.sub(r"[^A-Za-z0-9_]", "_", e["lib"].replace(".rpl", "")), re.sub(r"[^A-Za-z0-9_]", "_", e["name"]))
            imports[ident] = (e["slot"], e["name"])
    # `importvoid SLOT` (opt-in): a function import slot only, and no line of the unit may state a value for it
    importvoid = unit.get("importvoid", set())
    bad = sorted(importvoid - {v[0] for v in imports.values()})
    if bad:
        sys.exit("mkunit: importvoid: not a function import slot: " + " ".join("%08X" % x for x in bad))
    if importvoid:
        for line in specfile.expanded_lines(os.path.join(root, "tools", "verify", "units", a.unit + ".txt")):
            f = line.split("#")[0].split()
            if len(f) >= 2 and f[0] in ("ret", "ret!", "retarg", "fret", "fret!", "fretf64", "fretf64!", "mockretfield", "nestedret") and \
                    re.match(r"(0[xX])?[0-9A-Fa-f]{1,8}$", f[1]) and int(f[1], 16) in importvoid:
                sys.exit("mkunit: importvoid %08X contradicts the unit line: %s" % (int(f[1], 16), line.strip()))

    body = ['/* generated by tools/verify/mkunit.py: unit %s (recompiled originals; build output, do not commit) */' % a.unit,
            '#include "vm_gen.h"', '#include "unit.h"', ""]
    callees, imps, sites = set(), set(), set()
    funcs_c, origs = [], []
    # GHS register save/restore helpers (_savegpr/_restgpr/_savefpr..., 2026-10-24): the recompiler splits each into
    # one-instruction functions that fall through to the next entry with a tail call. A tested or real function that
    # calls into one needs the whole chain run for real, or its registers and SP are not saved/restored (AGL#8:
    # SP 0x30 low and the callee's r30/r31 leaking into the caller). Add every such chain automatically.
    unit["real"] = sorted(set(unit["real"]) | ghs_helper_chains(gen, tested + list(unit["real"])))
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
    fixtures = unit.get("fixture", set())
    callees |= fixtures  # explicitly declared mock-only targets (no generated code; never executed)
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
    head = list(body)  # the includes (unit_shared.h of a split)
    body += decls + [""] + funcs_c + [""]
    tail_at = len(body)  # call stubs and tables from here on (the last part of a split)
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
        is_fix = x in fixtures and x not in gen.loc
        is_imp = x not in gen.loc and not is_fix
        nm = "fixture_%08X" % x if is_fix else names.get(x, ("",))[0] if not is_imp else next(v[1] for k, v in imports.items() if v[0] == x)
        sym = gc.get(x, ("",))[0]
        sig = signature(sym) if sym else None
        ptrsz = [0] * 11
        outp = [0] * 11
        declared = 0
        nstack = 0
        rk, rb, rc = 0, 0, 0
        if is_fix:
            # declared fixture target: exactly the registers its unit facts name are compared; it may change every volatile
            im = mask(int(k[1:]) for (t, k) in overrides if t == x and re.match(r"r(\d+)$", k) and 3 <= int(k[1:]) <= 10)
            fm = 0
            declared = 1
            defr, deff = 0xFFFFFFFF, 0xFFFFFFFF
        elif is_imp:
            # the import's own argument registers (import_args.tsv: HLE implementation / mangled signature),
            # compared at every call whatever the candidate declares (game test: GX2CallDisplayList without r4)
            im, fm = import_args.get(x, (0, 0))
            declared = 1
            defr, deff = (0, 0) if x in importvoid else (0xFFFFFFFF, 0xFFFFFFFF)
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
        heapref = 0  # rN=heapSIZE unit facts: pointee bytes compared for heap pointers too
        for (fa, reg) in unit.get("heapref", set()):
            if fa == x:
                heapref |= 1 << reg
        fill = outonly  # generated mocks fill outN and inoutN storage
        for (fa, reg) in unit.get("fill", set()):
            if fa == x:
                fill |= 1 << reg
                outp[reg] = 1
        # a constructor listed in ctor_sizes.tsv (sizes.py: GHS constructors allocate sizeof when this == NULL)
        # constructs its r3 object: the storage before the call is not an input, and a generated mock fills its
        # sizeof bytes with zeros before the mocked call (harness), so later calls that receive the object see the
        # same bytes on both sides and no layout-dependent stale bytes are left in reused frames (2026-10-11: INF-render#2#2, stale stack bytes under stackfill). Explicit unit/global r3
        # facts win.
        cs = ctor_sizes().get(x)
        if cs and (x, "r3") not in overrides and (x, "r3") not in unit.get("pointee", {}) and not is_imp:
            if cs <= 0xFFFD:
                ptrsz[3] = cs
                outonly |= 1 << 3  # no random fill: the harness zeroes a listed constructor's storage before the mocked call
                outp[3] = 1
            else:
                ptrsz[3] = 255
        # static extent (extent_lint.py): how far the callee's code reaches through each argument register;
        # the harness's undersized-Local check uses it like a size fact (game test 2026-10-05)
        ext = [0] * 11
        if not is_imp and not is_fix:
            for reg in range(3, 11):
                if im >> reg & 1:
                    ext[reg] = min(extent(x, "r%d" % reg)[0], 0xFFFD)
        dynsrc, dynmul = [0] * 11, [0] * 11  # rN=dynK*rM: pointee size K*rM at each call (ptrsz holds the cap)
        for (fa, reg), (mul, src) in unit.get("dyn", {}).items():
            if fa == x:
                dynsrc[reg], dynmul[reg] = src, mul
        rows.append('    {0x%08Xu, "%s", 0x%Xu, 0x%Xu, 0x%Xu, 0x%Xu, {%s}, {%s}, %s, %d, %d, %d, 0x%Xu, %d, 0x%Xu, {%s}, 0x%Xu, 0x%Xu, {%s}, {%s}},' % (
            x, nm.replace('"', ""), im, fm, defr, deff, ",".join(map(str, ptrsz)), ",".join(map(str, outp)), rl,
            declared, rk, rb, rc, nstack, outonly, ",".join(map(str, ext)), fill, heapref,
            ",".join(map(str, dynsrc)), ",".join(map(str, dynmul))))
    body.append("\nconst CalleeInfo unit_callees[] = {\n%s\n    {0}\n};" % "\n".join(rows))
    body.append("const unsigned unit_ncallees = %d;" % len(rows))
    # every function import slot of the game (imports.json kind "f", sorted): with -nulltrap an indirect call or branch to
    # an address that is neither code of the image nor one of these slots is an instruction-fetch fault (2026-10-29)
    islots = sorted({slot for slot, _ in imports.values()})
    body.append("const uint32_t unit_import_slots[] = {%s};" % (", ".join("0x%08Xu" % x for x in islots) or "0"))
    body.append("const unsigned unit_nimport_slots = %d;" % len(islots))
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
    # memory: the dataflow caches are not needed any more, and
    # unit.c is written line by line instead of joined into one string first (the same bytes as "\n".join(body) + "\n")
    del df, df_lo, rw
    gen._text.clear()
    size = 0
    with open(os.path.join(out, "unit.c"), "w") as f:
        for line in body:
            f.write(line)
            f.write("\n")
            size += len(line) + 1
    write_split(out, a, size, head, decls, funcs_c, origs, body[tail_at:])
    print("unit %s: %d functions under test, %d callees, %d imports -> %s" % (a.unit, len(origs), len(callees), len(imps), out))


def split_count(a, size, nitems):
    """number of translation units: --split N, else --split-auto BYTES above the threshold, else 1"""
    n = a.split if a.split > 1 else (-(-size // max(1, a.split_auto // 2)) if a.split_auto > 0 and size > a.split_auto else 1)
    return max(1, min(n, nitems))


def write_split(out, a, textsize, head, decls, funcs_c, origs, tail):
    """the split form of unit.c (module docstring): unit_shared.h, unit.p0.c .. unit.pN-1.c and the unit.parts manifest.
    textsize: the length of unit.c's text (characters)"""
    n = split_count(a, textsize, len(funcs_c) + 1)
    items = funcs_c + ["\n".join(tail)] if n > 1 else []  # the stubs and tables stay together, at the end (as in unit.c)
    manifest = os.path.join(out, "unit.parts")
    for f in os.listdir(out):  # stale parts of an earlier split (another N, or none now)
        if f == "unit.parts" or f == "unit_shared.h" or re.match(r"unit\.p\d+\.c$", f):
            os.remove(os.path.join(out, f))
    if n <= 1:
        return
    shared = ["#ifndef WWHD_UNIT_SHARED_H", "#define WWHD_UNIT_SHARED_H"] + head + decls + \
        ["void orig_%08X(Cpu* c);" % x for x, _ in origs] + ["#endif", ""]
    total = sum(len(t) + 1 for t in items)
    groups, cur, size, done = [], [], 0, 0
    for i, t in enumerate(items):  # contiguous runs, cut when the run reaches its share of what is left
        cur.append(t)
        size += len(t) + 1
        left = n - len(groups)
        if left > 1 and size >= (total - done) / left and len(items) - i - 1 >= left - 1:
            groups.append(cur)
            done += size
            cur, size = [], 0
    groups.append(cur)
    open(os.path.join(out, "unit_shared.h"), "w").write("\n".join(shared))
    names = []
    for k, g in enumerate(groups):
        names.append("unit.p%d.c" % k)
        open(os.path.join(out, names[-1]), "w").write(
            '/* generated by tools/verify/mkunit.py: unit %s, part %d of %d (build output, do not commit) */\n'
            '#include "unit_shared.h"\n\n%s\n' % (a.unit, k + 1, len(groups), "\n".join(g)))
    open(manifest, "w").write("\n".join(names) + "\n")
    print("mkunit: unit.c (%.1f MB) split into %d translation units (%s)" % (textsize / 1048576.0, len(groups),
          ", ".join("%.1f MB" % (sum(len(t) + 1 for t in g) / 1048576.0) for g in groups)), file=sys.stderr)


if __name__ == "__main__":
    main()

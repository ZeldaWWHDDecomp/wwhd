#!/usr/bin/env python3
"""Static lint for undersized gabi::Locals (2026-10-05, game test: Locals smaller than the object the
callee fills; the harness only sees sizes that a callee fact declares).

usage: extent_lint.py [--all] [--verbose] [FILE.cpp ...]     (default: every .cpp under wwhd_src)

For every call in the candidate source that passes a gabi::Local (`L.get()`, `L.a`, `ea(L.get())`,
`at<..>(L.a)`, optionally `+ CONST`, or a variable bound to one of these) to a guest callee
(`call(0xADDR, ...)`, `gabi::call<..>(0xADDR, ...)`), the lint computes how far the callee reaches
through that argument register: loads/stores relative to the register and its copies (`mr`, `addi`),
followed into direct calls up to depth 4. Accesses inside loops (between a backward branch and its
target) are not counted (their reach depends on a trip count, e.g. memset-style helpers); loops and
indexed accesses make the result a lower bound (marked `+idx`). A hit is printed when the reach exceeds the Local's size minus
the offset. Float arguments are not separated from integer ones (the argument position is taken as
r3 + index), so a hit needs a look; the value loaded from a Local is not its address and is skipped.

Exit status 1 when there are hits (for use in scripts)."""
import functools
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools"))
sys.path.insert(0, HERE)
from ppcdis import dis  # noqa: E402
from funcdb import GenIndex  # noqa: E402

gen = GenIndex(os.path.join(ROOT, "build", "gen"))

W = {'lwz': 4, 'stw': 4, 'lhz': 2, 'lha': 2, 'sth': 2, 'lbz': 1, 'stb': 1, 'lfs': 4, 'stfs': 4, 'lfd': 8, 'stfd': 8,
     'lwzu': 4, 'stwu': 4, 'lbzu': 1, 'stbu': 1, 'lhzu': 2, 'sthu': 2, 'lhau': 2, 'lfsu': 4, 'stfsu': 4, 'lfdu': 8,
     'stfdu': 8, 'psq_l': 8, 'psq_st': 8, 'psq_lu': 8, 'psq_stu': 8}
INDEXED = ('lwzx', 'stwx', 'lbzx', 'stbx', 'lhzx', 'sthx', 'lhax', 'lfsx', 'stfsx', 'lfdx', 'stfdx', 'lwzux', 'stwux')


@functools.lru_cache(None)
def insns(a):
    n = gen.size(a) // 4
    if not n:
        return ()
    out = []
    for line in dis(a, n).split("\n"):
        m = re.match(r"([0-9a-f]{8}): ([0-9a-f]{8})\s+(\S+)\s*(.*)", line)
        if m:
            out.append((int(m.group(1), 16), m.group(3), [x.strip() for x in m.group(4).split(",")] if m.group(4) else []))
    return tuple(out)


@functools.lru_cache(None)
def loop_ranges(a):
    """[target, branch] address ranges of backward branches: accesses there depend on a trip count"""
    out = []
    for ad, op, ops in insns(a):
        if op.startswith('b') and ops:
            t = re.match(r'0x([0-9a-f]+)$', ops[-1])
            if t and int(t.group(1), 16) <= ad and int(t.group(1), 16) >= a:
                out.append((int(t.group(1), 16), ad))
    return tuple(out)


@functools.lru_cache(None)
def extent(a, reg, depth=0):
    """(bytes reached through reg from its value at entry, lower-bound flag)"""
    tr = {reg: 0}
    mx, lower = 0, False
    loops = loop_ranges(a)
    for ad, op, ops in insns(a):
        if any(lo <= ad <= hi for lo, hi in loops):
            # inside a loop: what it reaches depends on a trip count (memset-style helpers); only
            # forget the registers it redefines
            lower = True
            if ops and ops[0] in tr and not op.startswith(('st', 'cmp', 'b', 'dcb', 'tw', 'psq_st', 'mt')):
                del tr[ops[0]]
            m = re.match(r'(-?0x[0-9a-f]+|-?\d+)\((r\d+)\)$', ops[-1]) if ops else None
            if m and op.endswith('u') and m.group(2) in tr:
                del tr[m.group(2)]
            continue
        if op in ('mr', 'mr.') and len(ops) == 2 and ops[1] in tr:
            tr[ops[0]] = tr[ops[1]]; continue
        if op in ('or', 'or.') and len(ops) == 3 and ops[1] == ops[2] and ops[1] in tr:
            tr[ops[0]] = tr[ops[1]]; continue
        if op in ('addi', 'addic', 'addic.') and len(ops) == 3 and ops[1] in tr and re.match(r'-?(0x)?[0-9a-f]+$', ops[2]):
            tr[ops[0]] = tr[ops[1]] + int(ops[2], 0); continue
        m = re.match(r'(-?0x[0-9a-f]+|-?\d+)\((r\d+)\)$', ops[-1]) if ops else None
        if op in W and m and m.group(2) in tr:
            off = tr[m.group(2)] + int(m.group(1), 0)
            mx = max(mx, off + W[op])
            if op.endswith('u'):
                tr[m.group(2)] = off; lower = True
            if op[0] in 'lp' and not op.startswith('psq_st') and ops[0] in tr:
                del tr[ops[0]]
            continue
        if op in ('stmw', 'lmw') and m and m.group(2) in tr:
            mx = max(mx, tr[m.group(2)] + int(m.group(1), 0) + 4 * (32 - int(ops[0][1:]))); continue
        if op in INDEXED and len(ops) == 3 and (ops[1] in tr or ops[2] in tr):
            lower = True
        if op in ('bl', 'b') and ops:
            t = re.match(r'0x([0-9a-f]+)$', ops[0])
            if t and depth < 4:
                ta = int(t.group(1), 16)
                for r in ('r3', 'r4', 'r5', 'r6', 'r7', 'r8', 'r9', 'r10'):
                    if r in tr:
                        e, lb = extent(ta, r, depth + 1)
                        if e:
                            mx = max(mx, tr[r] + e)
                        lower |= lb
            if op == 'bl':
                for r in ('r0', 'r3', 'r4', 'r5', 'r6', 'r7', 'r8', 'r9', 'r10', 'r11', 'r12'):
                    tr.pop(r, None)
            continue
        if ops and ops[0] in tr and not op.startswith(('st', 'cmp', 'b', 'dcb', 'tw', 'psq_st', 'mt')):
            del tr[ops[0]]
    return mx, lower


TSIZE = {'u8': 1, 's8': 1, 'char': 1, 'bool': 1, 'u16': 2, 's16': 2, 'u32': 4, 's32': 4, 'f32': 4, 'BOOL': 4,
         'f64': 8, 'u64': 8, 's64': 8}
SLOT = lambda n: (n + 15) & ~15


def type_size(t, structs):
    t = re.sub(r'\s+', '', t)
    t = re.sub(r'^(?:gabi::)?be<(\w+)>', r'\1', t)
    t = re.sub(r'^(?:gabi::)?gptr<[^>]*>', 'u32', t)
    m = re.match(r'(\w+)((?:\[(?:0x[0-9A-Fa-f]+|\d+)\])+)$', t)
    if m:
        base = TSIZE.get(m.group(1)) or structs.get(m.group(1))
        if not base:
            return None
        for d in re.findall(r'\[(0x[0-9A-Fa-f]+|\d+)\]', m.group(2)):
            base *= int(d, 0)
        return base
    return TSIZE.get(t) or structs.get(t)


def struct_sizes(text):
    """struct X { be<u32> a, b; u8 data[8]; ... }: sizes of simple flat structs (be<T>, gptr<T>, arrays)"""
    out = {}
    for m in re.finditer(r'struct\s+(\w+)\s*(?::[^{]*)?\{([^{}]*)\}', text):
        body, size, ok = m.group(2), 0, True
        for decl in body.split(';'):
            decl = re.sub(r'/\*.*?\*/|//.*', '', decl).strip()
            if not decl:
                continue
            dm = re.match(r'(?:be<(\w+)>|gptr<[^>]*>|(\w+))\s+(.*)$', decl)
            if not dm:
                ok = False; break
            el = 4 if dm.group(1) is None and dm.group(2) is None else TSIZE.get(dm.group(1) or dm.group(2) or '', None)
            if decl.startswith('gptr'):
                el = 4
            if el is None:
                ok = False; break
            for v in dm.group(3).split(','):
                dims = re.findall(r'\[(0x[0-9A-Fa-f]+|\d+)\]', v)
                n = 1
                for d in dims:
                    n *= int(d, 0)
                size += el * n
        if ok and size:
            out[m.group(1)] = size
    # declared sizes win (WWHD_SIZE(T, N), static_assert(sizeof(T) == N))
    for m in re.finditer(r'WWHD_SIZE\(\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+|\d+)\s*\)', text):
        out[m.group(1)] = int(m.group(2), 0)
    for m in re.finditer(r'sizeof\(\s*(\w+)\s*\)\s*==\s*(0x[0-9A-Fa-f]+|\d+)', text):
        out[m.group(1)] = int(m.group(2), 0)
    return out


def split_args(s):
    out, d, cur = [], 0, ''
    for ch in s:
        if ch in '([{':
            d += 1
        elif ch in ')]}':
            if d == 0:
                out.append(cur)
                return out
            d -= 1
        if ch == ',' and d == 0:
            out.append(cur); cur = ''; continue
        cur += ch
    out.append(cur)
    return out


CONST = r'(0x[0-9A-Fa-f]+|\d+)u?'


def lint_file(path, verbose=False):
    text = open(path, errors='replace').read()
    structs = struct_sizes(text)
    hits, checked = [], 0
    # function bodies: split at WWHD_FUNC so names/aliases do not leak between functions
    starts = [m.start() for m in re.finditer(r'WWHD_FUNC\(', text)] + [len(text)]
    for i in range(len(starts) - 1):
        body = text[starts[i]:starts[i + 1]]
        fm = re.match(r'WWHD_FUNC\(\s*0x([0-9A-Fa-f]{8})', body)
        func = fm.group(1).upper() if fm else '?'
        locs = {}
        for m in re.finditer(r'Local<\s*([^;]*?)\s*>\s+(\w+)\s*[;{(]', body):
            z = type_size(m.group(1), structs)
            locs[m.group(2)] = (z, m.group(1))
        if not locs:
            continue
        # aliases: x = L.a / ea(L.get()) [+ C];  y = x + C
        alias = {}
        ref = r'(?:ea\(\s*)?(?:gabi::)?(?:at<[^>]*>\(\s*)?\b(%s)\s*(?:\.a\b|\.get\(\)|->a\b)\s*\)?\)?'
        for n in locs:
            for m in re.finditer(r'(\w+)\s*=\s*' + ref % n + r'\s*(?:([+-])\s*' + CONST + r')?\s*[;,]', body):
                o = int(m.group(4), 0) if m.group(4) else 0
                alias[m.group(1)] = (n, -o if m.group(3) == '-' else o)
        for _ in range(2):
            for m in re.finditer(r'(\w+)\s*=\s*(\w+)\s*\+\s*' + CONST + r'\s*[;,]', body):
                if m.group(2) in alias and m.group(1) not in alias:
                    n, o = alias[m.group(2)]
                    alias[m.group(1)] = (n, o + int(m.group(3), 0))
        for m in re.finditer(r'\bcall\s*(?:<[^>()]*>)?\(\s*([^,()]*?0x[0-9A-Fa-f]{8}[^,()]*?)\s*(?:/\*.*?\*/)?\s*,', body):
          for target in [t.upper() for t in re.findall(r'0x([0-9A-Fa-f]{8})', m.group(1))]:
            args = split_args(body[m.end():m.end() + 800])
            for idx, a in enumerate(args):
                a = a.strip()
                if re.match(r'(gabi::)?(load|gmem_ld\d+|word|byte|half|get|ld\w*)\b', a):
                    continue  # the value loaded from a Local, not its address
                name, off = None, 0
                for n in locs:
                    mm = re.fullmatch(ref % n + r'\s*(?:\+\s*' + CONST + r')?', a)
                    if mm:
                        name, off = n, int(mm.group(2), 0) if mm.group(2) else 0
                        break
                if name is None:
                    mm = re.fullmatch(r'(?:gabi::)?(?:at<[^>]*>\(\s*)?(\w+)\s*(?:\+\s*' + CONST + r')?\s*\)?', a)
                    if mm and mm.group(1) in alias:
                        name, off = alias[mm.group(1)][0], alias[mm.group(1)][1] + (int(mm.group(2), 0) if mm.group(2) else 0)
                if name is None or idx > 7:
                    continue
                z, t = locs[name]
                checked += 1
                e, lower = extent(int(target, 16), 'r%d' % (3 + idx))
                if z is None:
                    if verbose and e:
                        print("unsized  %s %s %s r%d Local %s<%s>+%#x reach %#x" % (path, func, target, 3 + idx, name, t, off, e))
                    continue
                if e and off + e > z:
                    hits.append((path, func, target, 3 + idx, name, t, z, off, e, lower))
    return hits, checked


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    verbose = '--verbose' in sys.argv
    files = args or [os.path.join(dp, f) for dp, _, fs in os.walk(os.path.join(ROOT, 'wwhd_src')) for f in fs if f.endswith('.cpp')]
    total, allhits = 0, []
    for p in sorted(files):
        h, c = lint_file(p, verbose)
        total += c
        allhits += h
    for (p, func, target, r, name, t, z, off, e, lower) in allhits:
        print("HIT %s func %s -> %s r%d: Local %s<%s> size %#x, offset %#x, callee reaches %#x%s" % (
            os.path.relpath(p, ROOT), func, target, r, name, t, z, off, e, " (+idx: lower bound)" if lower else ""))
    print("%d Local-passing call arguments checked, %d hits" % (total, len(allhits)), file=sys.stderr)
    sys.exit(1 if allhits else 0)


if __name__ == '__main__':
    main()

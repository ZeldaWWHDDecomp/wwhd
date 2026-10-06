#!/usr/bin/env python3
"""Which WWHD functions return a value, from the ORIGINAL code's callers (2026-10-05, game test:
daNpc_Bj1_c::demo 021F563C returns a BOOL its callers test, the candidate was declared void, and a
void candidate never had r3 compared; the Ocrogh Korok talk never started).

usage: ret_values.py            writes tools/verify/ret_values.tsv (addr, r3/f1 flags, evidence)

Method (generated C of the whole program, build/gen; funcdb.Cfg/Dataflow):
  - per caller, backward liveness over its instructions; a direct call reads the callee's live-in
    registers and kills what the callee may define; an indirect call or import kills the volatile
    registers and reads nothing (only explicit reads count); at a `return;` the caller's own return registers are live (fixpoint over the table).
  - F returns r3 (f1) if, at some direct call `bl F`, r3 (f1) is live after the call AND F may write
    r3 (f1) (Dataflow.maydef). The second condition removes GHS's cross-call register allocation:
    a caller may keep its own value in r3 across a callee that never touches it.
  - tail calls: if F ends in `b G` (MUSTTAIL) and G returns r3 (f1), F returns it too.
  - global passes until nothing changes (callers' return registers feed back into liveness at their exits).
Limits: callers reached only through function pointers / vtables give no evidence (a virtual whose
every caller is indirect is missed); an indirect call is assumed to read all argument registers, so a
value passed through to a virtual call counts as used; a function whose result no caller in the
binary uses is not listed (dead result). r3:r4 pairs (64-bit results, HD small-struct returns such as csXyz) are
recorded only when the callee itself writes r4 and the caller reads r4 on the straight-line path right after the call
(liveness through joins over-approximates)."""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
from funcdb import GenIndex, Dataflow, ARGS, VOLATILE  # noqa: E402

R3, R4, F1 = ("r", 3), ("r", 4), ("f", 1)


def main():
    gen = GenIndex(os.path.join(ROOT, "build", "gen"))
    df = Dataflow(gen, unknown_reads_args=False)
    funcs = list(gen.addrs)
    ret = {}        # addr -> set of return registers
    evidence = {}   # (addr, reg) -> first caller address
    tail = {}       # addr -> set of tail-call targets
    tailre = re.compile(r"^MUSTTAIL return f_([0-9A-F]{8})\(c\);")

    def straight_use(g, k, reg):
        j = k
        for _ in range(64):
            nx = g.succ[j]
            if len(nx) != 1:
                return False
            j = nx[0]
            if reg in g.use[j]:
                return True
            if reg in g.defs[j] or g.calls[j] or g.indirect[j]:
                return False
        return False

    owndefs = {}

    def own_def(f, reg):
        if f not in owndefs:
            try:
                g = df.cfg(f)
                owndefs[f] = set().union(*[g.defs[k] for k in g.reach]) if g.reach else set()
            except Exception:
                owndefs[f] = set()
        return reg in owndefs[f]
    for p in range(10):
        changed = False
        for n_, c in enumerate(funcs):
            try:
                g = df.cfg(c)
            except Exception:
                continue
            n = len(g.insns)
            if not n:
                continue
            use, defs = [], []
            for k in range(n):
                u, d = set(g.use[k]), set(g.defs[k])
                s = g.insns[k][1]
                tm = tailre.match(s)
                if tm:
                    tail.setdefault(c, set()).add(int(tm.group(1), 16))
                if k in g.reach:
                    for t in g.calls[k]:
                        u |= df.livein(t) - d
                        if not tm:
                            d |= df.maydef(t)
                    if g.indirect[k]:
                        d |= VOLATILE  # its arguments are not known: no reads assumed (explicit reads only)
                    if s.startswith("return;"):
                        u |= ret.get(c, set())
                use.append(u)
                defs.append(d)
            live = [set() for _ in range(n)]
            ch = True
            while ch:
                ch = False
                for k in range(n - 1, -1, -1):
                    out = set()
                    for j in g.succ[k]:
                        out |= live[j]
                    new = use[k] | (out - defs[k])
                    if new != live[k]:
                        live[k] = new
                        ch = True
            for k in g.reach:
                s = g.insns[k][1]
                if tailre.match(s):
                    continue
                for t in g.calls[k]:
                    out = set()
                    for j in g.succ[k]:
                        out |= live[j]
                    md = df.maydef(t)
                    for reg in (R3, F1, R4):
                        if reg in out and reg in md and reg not in ret.get(t, set()):
                            if reg == R4 and (R3 not in out or not own_def(t, R4) or not straight_use(g, k, R4)):
                                continue  # r4 of a pair: the callee itself must write it, and the caller must read
                                          # it on the straight-line path right after the call (liveness through
                                          # joins over-approximates: infeasible paths, values kept across calls)
                            ret.setdefault(t, set()).add(reg)
                            evidence.setdefault((t, reg), g.insns[k][0])
                            changed = True
        # tail calls: F returns what its tail-call target returns
        tch = True
        while tch:
            tch = False
            for f, ts in tail.items():
                for t in ts:
                    for reg in list(ret.get(t, set())):
                        if reg not in ret.get(f, set()):
                            ret.setdefault(f, set()).add(reg)
                            evidence.setdefault((f, reg), "tail:%08X" % t)
                            tch = changed = True
        print("pass %d: %d functions return a value" % (p + 1, len(ret)), file=sys.stderr)
        if not changed:
            break
    out = os.path.join(HERE, "ret_values.tsv")
    with open(out, "w") as fo:
        fo.write("# Generated by ret_values.py: functions whose result some original caller uses.\n")
        fo.write("# addr\tregs\tevidence (first caller instruction reading it, or tail:TARGET)\n")
        for a in sorted(ret):
            regs = ",".join("%s%d" % r for r in sorted(ret[a], key=lambda r: (r[0] != "r", r[1])))
            ev = ",".join(("%08X" % evidence[(a, r)]) if isinstance(evidence[(a, r)], int) else evidence[(a, r)]
                          for r in sorted(ret[a], key=lambda r: (r[0] != "r", r[1])))
            fo.write("%08X\t%s\t%s\n" % (a, regs, ev))
    print("%d functions -> %s" % (len(ret), out))


if __name__ == "__main__":
    main()

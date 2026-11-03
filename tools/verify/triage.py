#!/usr/bin/env python3
"""Shared survivor triage (2026-10-30): mechanical first-pass checks for mutation survivors.

Generalises AGL#2's archived scripts: 77% of AGL#2's EQ/RI rows fell into four families that can be checked mechanically. Every check
works on the COMPILED candidate (the unmutated source and the mutant, both compiled exactly like mutate.py's
per-mutant build: clang++ -std=c++20 -O1 -w, verify.FP, autoglue), source lines are
context only. The output is a CANDIDATE reason with evidence, never a final class: helpers and the decide.

Checks (column `check`):
  swap      swap independence: the base and mutant objects make the same effects (guest loads/stores through
            gmem_*, Cpu-struct accesses, guest calls, host calls, conditional exits), and every pair of effects
            whose order the swap inverted commutes: two reads; or accesses to provably disjoint locations (same
            base value and non-overlapping offsets, different frame objects with disjoint frame spans, a frame
            object that never escapes vs any other pointer, guest vs Cpu struct); a guest call commutes only with
            accesses to frame objects whose address it does not read (argument mask of build/verify/livein.tsv).
            Anything else (two calls, a call vs a heap access, two different heap bases, a moved conditional exit)
            is left open with the pair named. Effects that are not a permutation (a value now computed
            differently) give `dependent`.
  frame     FrameLocal shift: a FrameLocal<T> object's frame offset differs between base and mutant (directly, or
            through a constant argument of an in-TU helper that constructs it). The shifted span is compared with
            frames.tsv (save slots, back chain, caller frame above the frame size) and with the other frame objects
            of the candidate; then byte liveness (§1.4 item 12 and its 21:0x clarification) on the mutant's
            control-flow graph for every out-of-span byte: RI when a foreign read (another object's access, a guest
            call that reads a pointer to another object, the epilogue restore of a save slot, the caller's frame)
            can see a byte the shifted object wrote, or when the shifted object reads a byte before writing it
            itself (stale frame data / foreign data); EQ candidate when neither path exists.
  unreadarg unread argument registers: the only effects that differ are Cpu-struct writes of argument registers
            r3..r10 / f1..f8 whose next consumer on every path is a guest call whose live-in mask (livein.tsv)
            does not read them; or writes into a frame object whose address only ever reaches unread argument
            registers (a SafeText passed as r5/r6 to a callee that reads r3/r4 only).
  cr6       CR bit changes (gabi::cpu->cr[k] stores) before a guest call: EQ candidate when the callee does not read
            that CR bit at entry (non-variadic), or it is variadic (`bne/beq cr1` over the f1-f8 saves) and every
            constant format string among its argument registers has no floating-point conversion.

Grouping: function x original block (VM_COV of the generated unit.c, located through the guest call nearest the
mutation in the candidate) x mutation kind x reach (receipt: `not reached`, reached through a -runlist, unknown).

usage:
  triage.py UNIT --receipts DIR [--receipts DIR2 ...] [--class survived[,timeout]] [--ids N,M,...]
            [--ledger LEDGER.tsv [--ledger-class EQ,RI]] [--root CLONE] [--tree SRC_TREE] [--frames TSV]
            [--livein TSV] [--unitc unit.c] [--image image.bin] [--wdis-root DIR] [--out OUT.tsv] [--scratch DIR]
            [-j N]
  --root: the unit's clone (default: this checkout). Read only: compiles and evidence go to --scratch.
  --tree: where the sources the receipts were made from live (default --root); e.g. a `git archive` of the
          campaign commit when the clone has moved on.
  --ledger: take the survivor list from a ledger TSV (id/func/src/mutation/class columns, as AGL#2's), e.g. to
            compare the candidates with final classes (columns ledger_class, agree).
Output: OUT.tsv (one row per survivor: id, func, src, kind, block, reach, group, check, candidate, reason, evidence
file) and OUT.summary.txt (per family: counts per candidate; per group). Exit 0.
Self-test: triage_test.py.
"""
import argparse
import collections
import concurrent.futures
import difflib
import json
import os
import re
import struct
import threading
import subprocess
import sys

sys.dont_write_bytecode = True  # never write __pycache__ into a read-only checkout

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
M64 = (1 << 64) - 1
HOSTMAP = 0x200000000000  # PPC_MEM_BASE (runtime/include/ppc.h)

# Cpu struct (runtime/include/ppc.h): r[32] @0, lr @0x80, ctr @0x84, cr[32] bytes @0x88, xer 4 bytes @0xA8,
# f[32] {ps0, ps1} doubles @0xB0
CPU_R, CPU_CR, CPU_F = 0x0, 0x88, 0xB0


def cpu_name(off):
    if off < 0x80:
        return "r%d" % (off // 4) + ("" if off % 4 == 0 else "+%d" % (off % 4))
    if off < 0x88:
        return "lr" if off < 0x84 else "ctr"
    if off < 0xA8:
        return "cr[%d]" % (off - CPU_CR)
    if off < CPU_F:
        return "xer+%d" % (off - 0xA8)
    if off < CPU_F + 32 * 16:
        n, r = divmod(off - CPU_F, 16)
        return "f%d.%s" % (n, "ps0" if r < 8 else "ps1")
    return "cpu+0x%X" % off


# ---------------------------------------------------------------------------------------------- objdump parsing
class Ins:
    __slots__ = ("addr", "mn", "ops", "reloc", "text", "target")

    def __init__(self, addr, mn, ops, text):
        self.addr, self.mn, self.ops, self.text = addr, mn, ops, text
        self.reloc = None
        self.target = None

    def norm(self):
        """text without addresses (branch targets by symbol only), for base/mutant alignment"""
        t = re.sub(r"0x[0-9a-f]+ <([^>+]*)(\+0x[0-9a-f]+)?>", r"<\1>", self.mn + " " + ", ".join(self.ops))
        return t + (" @" + self.reloc if self.reloc else "")


def split_ops(s):
    out, depth, cur = [], 0, ""
    for ch in s:
        if ch in "[{":
            depth += 1
        elif ch in "]}":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip())
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def parse_objdump(text):
    """{symbol: [Ins]} of `objdump -d -r --no-show-raw-insn` output (Mach-O arm64)"""
    funcs, cur, byaddr = {}, None, {}
    for line in text.split("\n"):
        m = re.match(r"^([0-9a-f]+) <(.*)>:$", line)
        if m:
            cur = funcs.setdefault(m.group(2), [])
            continue
        if cur is None:
            continue
        m = re.match(r"^\s+([0-9a-f]+):\s+(\S+)\s*(.*)$", line)
        if m and not line.startswith("\t\t"):
            ops = m.group(3).split(";")[0].strip()
            ins = Ins(int(m.group(1), 16), m.group(2), split_ops(ops), line)
            t = re.search(r"\b0x([0-9a-f]+) <", ops)
            if t:
                ins.target = int(t.group(1), 16)
            cur.append(ins)
            byaddr[ins.addr] = ins
            continue
        m = re.match(r"^\s+([0-9a-f]+):\s+ARM64_RELOC_(\w+)\s+(\S+)", line)
        if m:
            ins = byaddr.get(int(m.group(1), 16))
            if ins is not None and ins.reloc is None:
                ins.reloc = m.group(3)
                if m.group(2).startswith("TLVP"):
                    ins.reloc = "TLV:" + m.group(3)
                elif m.group(2) in ("PAGE21", "PAGEOFF12", "GOT_LOAD_PAGE21", "GOT_LOAD_PAGEOFF12"):
                    ins.reloc = "ADDR:" + m.group(3)
    return funcs


def func_of_addr(funcs, addr):
    """the symbol of marked function ADDR: its WWHD_FUNC check materialises the address (mov/movk) near the top"""
    a = int(addr, 16)
    lo, hi = a & 0xFFFF, a >> 16
    best = None
    for name, ins in funcs.items():
        head = ins[:60]
        for i, x in enumerate(head[:-1]):
            if x.mn == "mov" and len(x.ops) == 2 and x.ops[1].startswith("#") and _imm(x.ops[1]) == lo:
                y = head[i + 1]
                if y.mn == "movk" and y.ops[0] == x.ops[0] and _imm(y.ops[1]) == hi and "lsl #16" in ",".join(y.ops):
                    if best is None or len(ins) > len(funcs[best]):
                        best = name
                    break
    return best


def _imm(s):
    s = s.strip().lstrip("#")
    try:
        return int(s, 0)
    except ValueError:
        try:
            return int(float(s) * 0)  # floats are not used as integers
        except ValueError:
            return None


# ------------------------------------------------------------------------------------------------ abstract values
# A value is (base, off): base None = the constant off; otherwise an opaque hashable base plus a constant offset.
def C(n):
    return (None, n & M64)


def is_const(v):
    return v is not None and v[0] is None


def addk(v, k):
    if v is None:
        return None
    return (v[0], (v[1] + k) & M64)


def opv(*parts):
    return (("op",) + tuple(parts), 0)


REGRE = re.compile(r"^([xwsdqvhb])(\d+)$")


def reg(name):
    """canonical register key, or None for an immediate/other operand"""
    name = name.strip()
    if name in ("sp", "wsp"):
        return "sp"
    if name in ("xzr", "wzr"):
        return "zr"
    if name in ("x29", "fp", "w29"):
        return "x29"
    if name in ("x30", "lr", "w30"):
        return "x30"
    m = REGRE.match(name.split(".")[0])
    if not m:
        return None
    return ("x" if m.group(1) in "xw" else "v") + m.group(2)


def regsize(name):
    c = name.strip()[0]
    return {"x": 8, "w": 4, "s": 4, "d": 8, "q": 16, "h": 2, "b": 1}.get(c, 8)


CALLER_SAVED = ["x%d" % i for i in range(19)] + ["x30"] + ["v%d" % i for i in range(8)] + ["v%d" % i for i in range(16, 32)]

GMEM = re.compile(r"^_gmem_(ld|st|lf|stf|lfd|stfd)(8|16|32|64|s|d)?$")
PURE_CALLS = re.compile(r"^(__ZTWN4gabi3cpuE|_gmem_host_object|__ZN4gabi9CandidateC[12]E.*|_gmem_frame_info|"
                        r"__ZN4gabi10FrameLocal.*D[12]Ev|__ZN4gabi5Local.*D[12]Ev|_gmem_stack_fresh|"
                        r"__ZL\d+ppc_stfs_bits_slow.*|__ZL\d+ppc_lfs.*|_gmem_frame_fallback|___clang_call_terminate|"
                        r"__ZSt9terminatev|__Unwind_Resume|___cxa_begin_catch|___cxa_end_catch)$")
NORETURN = re.compile(r"^(_ppc_trap|_gmem_trap|___clang_call_terminate|__ZSt9terminatev|__Unwind_Resume|_abort|"
                      r"___assert_rtn|__ZN4gabi.*(fail|fatal).*)$")
DO_CALL = re.compile(r"^__ZN4gabi7do_callEP3CpuRNS_7ArgPackEji$")
FL_CTOR = re.compile(r"^__ZN4gabi10FrameLocalI(.*)EC[12]Ej$")
LOCAL_CTOR = re.compile(r"^__ZN4gabi5LocalI(.*)EC[12]Ev$")
AUTOFRAME = re.compile(r"^__ZN4gabi9AutoFrame[CD][12]E")


class Event:
    __slots__ = ("pos", "node", "kind", "space", "addr", "width", "val", "target", "args", "sym", "ins", "chain",
                 "extra")

    def __init__(self, kind, ins, node, chain, **kw):
        self.kind, self.ins, self.node, self.chain = kind, ins, node, chain
        self.pos = (node,) + tuple(c for c in chain[1:]) + (ins.addr,)
        self.space = kw.get("space")
        self.addr = kw.get("addr")
        self.width = kw.get("width", 0)
        self.val = kw.get("val")
        self.target = kw.get("target")
        self.args = kw.get("args")
        self.sym = kw.get("sym")
        self.extra = kw.get("extra")

    def is_read(self):
        return self.kind in ("gr", "cr", "hr")

    def is_write(self):
        return self.kind in ("gw", "cw", "hw")

    def is_mem(self):
        return self.is_read() or self.is_write()

    def where(self):
        return "+".join("%x" % c for c in self.chain[1:]) + ("@" if len(self.chain) > 1 else "") + "%x" % self.ins.addr


class Obj:
    """a frame object (gabi::FrameLocal<T>) or gabi::Local<T>"""
    __slots__ = ("key", "kind", "type", "off", "size", "ins", "chain", "node")

    def __init__(self, key, kind, typ, off, size, ins, chain, node):
        self.key, self.kind, self.type, self.off, self.size = key, kind, typ, off, size
        self.ins, self.chain, self.node = ins, chain, node


def demangle_type(m):
    """a readable name for a mangled template argument (best effort)"""
    out, i = [], 0
    while i < len(m):
        d = re.match(r"N?(\d+)", m[i:])
        if not d:
            i += 1
            continue
        n = int(d.group(1))
        s = i + len(d.group(0))
        out.append(m[s:s + n])
        i = s + n
    return "::".join(out) or m


class Analysis:
    """value numbering + effect collection over one function of an object (in-object callees inlined into the
    effect list, depth <= 3)"""

    def __init__(self, funcs, name, livein=None, depth=0, chain=(0,), entry=None, canon_prefix=(), glue=None):
        self.funcs, self.name, self.livein = funcs, name, livein or {}
        self.glue = glue or {}  # glue `data NAME ADDR` lines: host static -> guest address
        self.ins = funcs[name]
        self.depth, self.chain, self.canon_prefix = depth, chain, canon_prefix
        self.entry = entry
        self.idx = {x.addr: i for i, x in enumerate(self.ins)}
        self.events, self.objs = [], {}
        self.calls = []  # (node, ins, kind, target value, args)
        self._cfg()
        self._canon_calls()

    # -------- CFG
    def _cfg(self):
        ins = self.ins
        leaders = {0}
        for i, x in enumerate(ins):
            if x.mn in ("b", "br", "ret") or x.mn.startswith("b.") or x.mn in ("cbz", "cbnz", "tbz", "tbnz"):
                if i + 1 < len(ins):
                    leaders.add(i + 1)
                if x.target is not None and x.target in self.idx and x.mn != "bl":
                    leaders.add(self.idx[x.target])
            elif x.mn == "bl" and x.reloc and NORETURN.match(x.reloc) and i + 1 < len(ins):
                leaders.add(i + 1)
        self.starts = sorted(leaders)
        self.block_of = {}
        self.blocks = []
        for k, s in enumerate(self.starts):
            e = self.starts[k + 1] if k + 1 < len(self.starts) else len(ins)
            self.blocks.append((s, e))
            for i in range(s, e):
                self.block_of[i] = k
        self.succ = collections.defaultdict(list)
        self.pred = collections.defaultdict(list)
        for k, (s, e) in enumerate(self.blocks):
            last = ins[e - 1]
            fall = k + 1 < len(self.blocks)
            tgt = self.block_of.get(self.idx.get(last.target)) if last.target is not None else None
            if last.mn == "b" and last.reloc is None:
                outs = [tgt] if tgt is not None else []
            elif last.mn == "b":  # tail call
                outs = []
            elif last.mn in ("ret", "br"):
                outs = []
            elif last.mn.startswith("b.") or last.mn in ("cbz", "cbnz", "tbz", "tbnz"):
                outs = ([tgt] if tgt is not None else []) + ([k + 1] if fall else [])
            elif last.mn == "bl" and last.reloc and NORETURN.match(last.reloc):
                outs = []
            else:
                outs = [k + 1] if fall else []
            for o in outs:
                self.succ[k].append(o)
                self.pred[o].append(k)
        # blocks reachable from the entry (landing pads have no explicit edges: left out)
        seen, todo = {0}, [0]
        while todo:
            k = todo.pop()
            for o in self.succ[k]:
                if o not in seen:
                    seen.add(o)
                    todo.append(o)
        self.reach = seen
        # guard blocks: a diagnostic gmem_host_object call and a branch back
        self.guard_block = set()
        for k, (s, e) in enumerate(self.blocks):
            body = self.ins[s:e]
            if any(x.mn == "bl" and x.reloc == "_gmem_host_object" for x in body) and len(body) <= 6:
                self.guard_block.add(k)

    def _canon_calls(self):
        """canonical id of every bl: (symbol, k-th bl to that symbol in the function), plus the inline chain"""
        cnt = collections.Counter()
        self.canon = {}
        for x in self.ins:
            if x.mn in ("bl", "blr") or (x.mn == "b" and x.reloc):
                s = x.reloc or x.mn
                self.canon[x.addr] = self.canon_prefix + ((s, cnt[s]),)
                cnt[s] += 1

    # -------- state
    def initial(self):
        st = {"r": {}, "hs": {}, "cpu": {}, "ver": {"guest": ("v0",), "cpu": ("v0",), "host": ("v0",)},
              "flags": None}
        sp = (("sp", self.depth), 0)
        st["r"]["sp"] = sp
        if self.entry:
            for k, v in self.entry["r"].items():
                st["r"][k] = v
            st["cpu"] = dict(self.entry.get("cpu", {}))
            st["r"]["sp"] = sp
        else:
            for i in range(8):
                st["r"]["x%d" % i] = (("arg", i), 0)
                st["r"]["v%d" % i] = (("farg", i), 0)
        return st

    def get(self, st, r):
        if r == "zr":
            return C(0)
        v = st["r"].get(r)
        if v is None:
            v = (("init", r), 0)
        return v

    def operand(self, st, o):
        o = o.strip()
        if o.startswith("#"):
            n = _imm(o)
            return C(n) if n is not None else opv("fimm", o)
        r = reg(o)
        if r:
            return self.get(st, r)
        return opv("operand", o)

    def meet(self, k, states, phis):
        """meet of predecessor out-states at block k (phi'd keys stay phi'd: monotone)"""
        if not states:
            return None
        out = {"r": {}, "hs": {}, "cpu": {}, "ver": {}, "flags": None}
        for part in ("r", "hs", "cpu", "ver"):
            keys = set()
            for s in states:
                keys |= set(s[part])
            for key in keys:
                vals = [s[part].get(key) for s in states]
                pk = (part, key)
                if pk not in phis and all(v == vals[0] for v in vals) and vals[0] is not None:
                    out[part][key] = vals[0]
                else:
                    if all(v is None for v in vals):
                        continue
                    phis.add(pk)
                    if part == "ver":
                        out[part][key] = ("phi", k)
                    elif part == "hs":
                        ws = {v[1] for v in vals if v is not None}
                        out[part][key] = ((("phi", self.canon_prefix, k, part, key), 0), min(ws))
                    else:
                        out[part][key] = (("phi", self.canon_prefix, k, part, key), 0)
        return out

    def run(self):
        """fixpoint over the CFG, then one recording pass"""
        nb = len(self.blocks)
        ins_state = {0: self.initial()}
        out_state = {}
        phis = collections.defaultdict(set)
        todo = collections.deque([0])
        inq = {0}
        steps = 0
        while todo and steps < 20000:
            steps += 1
            k = todo.popleft()
            inq.discard(k)
            st = ins_state.get(k)
            if st is None:
                continue
            st = _copy(st)
            self.exec_block(k, st, record=False)
            if out_state.get(k) == st:
                continue
            out_state[k] = st
            for o in self.live_succ(k, st):
                preds = [out_state[p] for p in self.pred[o] if p in out_state and o in self.live_succ(p, out_state[p])]
                if o == 0:
                    preds.append(self.initial())
                new = self.meet(o, preds, phis[o])
                if new != ins_state.get(o):
                    ins_state[o] = new
                    if o not in inq:
                        todo.append(o)
                        inq.add(o)
        self.in_state = ins_state
        # later analyses (byte liveness, consumers, reachability) see only the edges a constant condition leaves
        live = collections.defaultdict(list)
        lpred = collections.defaultdict(list)
        for k2, st2 in out_state.items():
            for o in self.live_succ(k2, st2):
                live[k2].append(o)
                lpred[o].append(k2)
        self.succ, self.pred = live, lpred
        self.reach = set(out_state) | {0}
        self.phi_in = getattr(self, "phi_in", {})
        for o, pk in phis.items():
            for part, key in pk:
                if part == "ver":
                    continue
                v = ins_state.get(o, {}).get(part, {}).get(key)
                if part == "hs" and v is not None:
                    v = v[0]
                if v is None or not (isinstance(v[0], tuple) and v[0][0] == "phi"):
                    continue
                inc = []
                for p in lpred.get(o, []) + ([None] if o == 0 else []):
                    src = out_state.get(p) if p is not None else self.initial()
                    w = src.get(part, {}).get(key) if src else None
                    if part == "hs" and w is not None:
                        w = w[0]
                    if w is not None:
                        inc.append(w)
                self.phi_in[v[0]] = inc
        for k in range(nb):
            if k in ins_state and k in self.reach:
                self.exec_block(k, _copy(ins_state[k]), record=True)
        return self

    def live_succ(self, k, st):
        d = st.get("_br")
        s, e = self.blocks[k]
        last = self.ins[e - 1]
        if d is None or not (last.mn.startswith("b.") or last.mn in ("cbz", "cbnz", "tbz", "tbnz")):
            return self.succ[k]
        tgt = self.block_of.get(self.idx.get(last.target))
        return [tgt] if d and tgt is not None else ([k + 1] if not d and k + 1 < len(self.blocks) else [])

    # -------- one block
    def exec_block(self, k, st, record):
        st["_br"] = None
        s, e = self.blocks[k]
        for i in range(s, e):
            self.step(st, self.ins[i], k, record)

    def emit(self, record, kind, ins, k, **kw):
        if record:
            self.events.append(Event(kind, ins, k, self.chain, **kw))

    def mem_addr(self, st, opnd):
        """[xN, #imm] / [xN] / [xN, #imm]! / [xN, xM] -> (base reg, value, writeback)"""
        m = re.match(r"\[([^\],]+)(?:,\s*([^\]]+))?\](!?)", opnd)
        if not m:
            return None, None, None
        b = reg(m.group(1))
        bv = self.get(st, b)
        if m.group(2):
            o = m.group(2).strip()
            if o.startswith("#"):
                n = _imm(o)
                v = addk(bv, n) if n is not None else opv("addr", bv, o)
            else:
                parts = [p.strip() for p in o.split(",")]
                iv = self.operand(st, parts[0])
                sh = 0
                if len(parts) > 1:
                    mm = re.search(r"#(\d+)", parts[1])
                    sh = int(mm.group(1)) if mm else 0
                if is_const(iv):
                    n = iv[1]
                    if len(parts) > 1 and parts[1].startswith("sxtw") and n & 0x80000000:
                        n -= 1 << 32
                    v = addk(bv, n << sh)
                elif bv[0] == ("tlsval", "__ZN4gabi3cpuE"):
                    v = (("cpuidx",), -1)  # the Cpu struct at an unknown offset
                else:
                    v = opv("addrx", bv, iv, o)
        else:
            v = bv
        return b, v, (m.group(3) == "!")

    def space_of(self, v):
        b = v[0]
        if b is None:
            return "host", None
        if b == ("tlsval", "__ZN4gabi3cpuE") or b == ("cpuidx",):
            return "cpu", v[1]
        if isinstance(b, tuple) and b[0] == "sp":
            return "hstack", v[1]
        if isinstance(b, tuple) and b[0] in ("tlv", "tlsaddr"):
            return "tls", None
        if isinstance(b, tuple) and b[0] == "sym":
            return "host", None  # a host static (glue table, string)
        # any other pointer the candidate dereferences directly is a host view of guest memory (gptr/be<> fast
        # path: guest address | PPC_MEM_BASE): the guest memory, so it may alias every gmem_* access
        return "guest", None

    def load(self, st, space, addr, width):
        if space == "hstack":
            return hs_load(st["hs"], addr, width, st["ver"]["host"])
        if space == "cpu":
            v = st["cpu"].get(addr[1]) if addr[0] != ("cpuidx",) else None
            if v is not None and v[0] != "stale":
                return v
            return (("ld", "cpu", addr[1], width, st["ver"]["cpu"]), 0)
        if space == "tls":
            return (("tlsval", addr[0][1] if isinstance(addr[0], tuple) and len(addr[0]) > 1 else "?"), 0)
        if space == "host" and isinstance(addr[0], tuple) and addr[0][0] == "sym" and addr[1] == 0:
            m = re.match(r"^(?:__ZL\d+|_)(\w+)$", addr[0][1])
            if m and m.group(1) in self.glue:
                ga = self.glue[m.group(1)]
                return C(ga) if width == 4 else C(ga + HOSTMAP)  # a glue `data` symbol: its guest address
        return (("ld", space, addr, width, st["ver"]["host"]), 0)

    def step(self, st, x, k, record):
        mn, ops = x.mn, x.ops
        R = st["r"]
        base = mn.split(".")[0]
        # ---------------- calls
        if mn == "bl" or (mn == "b" and x.reloc):
            self.call(st, x, k, record)
            if mn == "b":
                self.emit(record, "ret", x, k)
            return
        if mn == "blr":
            r0 = self.get(st, reg(ops[0]))
            if r0[0] and isinstance(r0[0], tuple) and r0[0][0] == "tlvfn":
                R["x0"] = (("tlsaddr", r0[0][1]), 0)  # thread-local variable access thunk
                return
            self.emit(record, "hcall", x, k, sym="indirect", args={i: self.get(st, "x%d" % i) for i in range(8)})
            self.clobber(st, x)
            return
        if mn == "ret":
            self.emit(record, "ret", x, k, val=(self.get(st, "x0"), self.get(st, "v0")))
            return
        if mn in ("b", "br"):
            if mn == "br":
                self.emit(record, "hcall", x, k, sym="br")
            return
        if mn.startswith("b.") or mn in ("cbz", "cbnz", "tbz", "tbnz"):
            self.branch(st, x, k, record)
            return
        # ---------------- memory
        if base in ("ldr", "ldur", "ldrb", "ldrh", "ldrsb", "ldrsh", "ldrsw", "ldurb", "ldurh", "ldursb",
                    "ldursh", "ldursw", "ldp", "ldar", "ldarb", "ldarh", "ldxr", "ldaxr", "ldpsw", "ld1"):
            self.mem(st, x, k, record, load=True)
            return
        if base in ("str", "stur", "strb", "strh", "sturb", "sturh", "stp", "stlr", "stlrb", "stlrh", "stxr",
                    "stlxr", "st1"):
            self.mem(st, x, k, record, load=False)
            return
        if base in ("casal", "cas", "casa", "casl", "ldadd", "swp"):
            self.emit(record, "hcall", x, k, sym="atomic")
            for o in ops[:2]:
                r = reg(o)
                if r:
                    R[r] = opv("atomic", x.addr)
            return
        # ---------------- flags
        if mn in ("cmp", "cmn", "tst", "fcmp", "fcmpe", "ccmp", "ccmn", "fccmp"):
            st["flags"] = opv(mn, *[self.operand(st, o) for o in ops], ops[0].strip()[:1])
            return
        if not ops:
            return
        d = reg(ops[0])
        if d is None or d == "zr":
            return
        src = [self.operand(st, o) for o in ops[1:]]
        v = None
        if x.reloc and x.reloc.startswith("TLV:"):
            sym = x.reloc[4:]
            if mn == "adrp" or base in ("ldr",):
                v = (("tlv", sym), 0)
        elif x.reloc and x.reloc.startswith("ADDR:"):
            v = (("sym", x.reloc[5:]), 0)
        elif mn == "adrp":
            v = opv("adrp", ops[1])
        if v is None:
            v = self.alu(mn, ops, src, st)
        if d.startswith("x") and ops[0].strip().startswith("w") and is_const(v):
            v = C(v[1] & 0xFFFFFFFF)
        R[d] = v
        if mn in ("adds", "subs", "ands", "bics", "negs"):
            st["flags"] = opv("flags", mn, v)

    def alu(self, mn, ops, src, st):
        if mn in ("mov", "fmov") and len(src) == 1:
            if mn == "fmov" and ops[1].strip().startswith("#"):
                return opv("fconst", ops[1])
            return src[0]
        if mn in ("movz",):
            return C(_shift_imm(ops))
        if mn == "movk" and len(src) >= 1:
            old = self.get(st, reg(ops[0]))
            sh = _lsl(ops)
            if is_const(old) and is_const(src[0]):
                return C((old[1] & ~(0xFFFF << sh)) | ((src[0][1] & 0xFFFF) << sh))
            return opv("movk", old, src[0], sh)
        if mn == "movn":
            return C(~_shift_imm(ops))
        if mn in ("add", "sub") and len(src) >= 2:
            sh = _lsl(ops)
            a, b = src[0], src[1]
            if is_const(b) and len(ops) <= 4:
                n = b[1] << sh if ops[2].strip().startswith("#") else b[1]
                if len(ops) == 4 and not ops[2].strip().startswith("#"):
                    ext = ops[3].strip()
                    em = re.match(r"(sxtw|uxtw|sxth|uxth|sxtb|uxtb|lsl|sxtx|uxtx)(?:\s+#(\d+))?$", ext)
                    if not em:
                        return opv(mn, a, b, ext)
                    n = b[1]
                    if em.group(1) == "sxtw":
                        n &= 0xFFFFFFFF
                        n = n - (1 << 32) if n >> 31 else n
                    elif em.group(1) == "uxtw":
                        n &= 0xFFFFFFFF
                    n <<= int(em.group(2) or 0)
                return addk(a, n if mn == "add" else -n)
            if is_const(a) and mn == "add" and len(ops) == 3:
                return addk(b, a[1])
            if len(ops) == 3 and mn == "add" and a[1] == 0 and b[1] == 0:
                return opv("add", *sorted([a, b], key=repr))
            return opv(mn, a, b, ",".join(ops[3:]))
        if mn == "orr" and len(src) == 2:
            a, b = src
            if is_const(b) and b[1] == HOSTMAP:
                return addk(a, HOSTMAP)  # guest address -> host pointer (guest addresses are below 2^32)
            if is_const(a) and a[1] == 0:
                return b
            if is_const(a) and is_const(b):
                return C(a[1] | b[1])
        if mn == "csel" and len(src) == 3:
            a, b = src[0], src[1]
            # null-propagating host pointer: csel xD, xzr, xP, eq (P = guest address | HOSTMAP)
            if is_const(a) and a[1] == 0 and not is_const(b):
                return b
            if is_const(b) and b[1] == 0 and not is_const(a):
                return a
            if a == b:
                return a
        if mn in ("lsl", "lsr", "asr", "ror") and len(src) == 2 and is_const(src[0]) and is_const(src[1]):
            n, s = src[0][1], src[1][1]
            if mn == "lsl":
                return C(n << s)
            if mn == "lsr":
                return C(n >> s)
        if mn in ("sxtw", "uxtw", "sxth", "uxth", "sxtb", "uxtb") and len(src) == 1 and not is_const(src[0]):
            if mn == "uxtw":
                return src[0]
        return opv(mn, *src, *[o for o in ops[1:] if o.strip().startswith(("lsl", "lsr", "asr", "sxtw", "uxtw"))])

    def clobber(self, st, x):
        R = st["r"]
        cid = self.canon.get(x.addr, (("?", x.addr),))
        for r in CALLER_SAVED:
            R[r] = (("clob", cid, r), 0)
        for key in ("guest", "cpu", "host"):
            st["ver"][key] = ("call", cid)
        # registers a callee may leave untouched: kept, marked stale (argument-register flow across calls)
        st["cpu"] = {o: (v if v[0] == "stale" else ("stale", v)) for o, v in st["cpu"].items()}
        st["flags"] = None

    def mem(self, st, x, k, record, load):
        ops = x.ops
        R = st["r"]
        pair = x.mn in ("ldp", "stp", "ldpsw")
        nregs = 2 if pair else 1
        mops = [o for o in ops if o.startswith("[")]
        if not mops:
            if load:  # pc-relative literal
                for r in ops[:nregs]:
                    rk = reg(r)
                    if rk and rk != "zr":
                        R[rk] = opv("literal", x.addr, ops[-1])
            return
        mi = ops.index(mops[0])
        b, addr, wb = self.mem_addr(st, mops[0])
        post = None
        if mi + 1 < len(ops) and ops[mi + 1].startswith("#"):
            post = _imm(ops[mi + 1])  # post-index: access at base, then base += imm
        regs = ops[:nregs]
        w = regsize(regs[0])
        if x.mn.endswith("b") or x.mn.endswith("sb"):
            w = 1
        elif x.mn.endswith("h") or x.mn.endswith("sh"):
            w = 2
        elif x.mn in ("ldrsw", "ldursw", "ldpsw"):
            w = 4
        space, off = self.space_of(addr)
        for j, r in enumerate(regs):
            a = addk(addr, j * w)
            rk = reg(r)
            if load:
                if x.reloc and x.reloc.startswith("TLV:"):
                    v = (("tlv", x.reloc[4:]), 0)
                elif space == "tls":
                    b0 = addr[0]
                    if b0[0] == "tlv":
                        v = (("tlvfn", b0[1]), 0)
                    else:
                        v = (("tlsval", b0[1]), 0)
                else:
                    if space == "guest":
                        a = _guest(a)
                        v = (("ld", "guest", a, w, st["ver"]["guest"]), 0)
                        self.emit(record, "gr", x, k, space="guest", addr=a, width=w)
                    else:
                        v = self.load(st, space, a, w)
                    if space == "cpu":
                        self.emit(record, "cr", x, k, space="cpu", addr=a, width=w)
                    elif space == "host":
                        self.emit(record, "hr", x, k, space="host", addr=a, width=w)
                if rk and rk != "zr":
                    R[rk] = v
            else:
                v = self.get(st, rk) if rk else C(0)
                if space == "hstack":
                    hs_store(st["hs"], a, w, v)
                elif space == "cpu":
                    if a[0] == ("cpuidx",):
                        st["cpu"] = {}
                        w = 0x300
                    else:
                        st["cpu"][a[1]] = v
                    st["ver"]["cpu"] = ("st", x.addr, j)
                    self.emit(record, "cw", x, k, space="cpu", addr=a, width=w, val=v)
                elif space == "tls":
                    pass  # gabi::Activation depth/current bookkeeping
                elif space == "guest":
                    a = _guest(a)
                    st["ver"]["guest"] = ("st", self.canon_prefix, x.addr, j)
                    self.emit(record, "gw", x, k, space="guest", addr=a, width=w, val=v)
                else:
                    st["ver"]["host"] = ("st", x.addr, j)
                    self.emit(record, "hw", x, k, space="host", addr=a, width=w, val=v)
        if wb or post is not None:
            R[b] = addk(R.get(b, self.get(st, b)), post if post is not None else 0) if post is not None else addr

    def decide(self, st, x):
        """True (taken) / False (falls through) when the condition is a constant, else None"""
        mn = x.mn
        if mn in ("cbz", "cbnz"):
            v = self.get(st, reg(x.ops[0]))
            if is_const(v):
                z = (v[1] & (0xFFFFFFFF if x.ops[0].strip().startswith("w") else M64)) == 0
                return z if mn == "cbz" else not z
            return None
        if mn in ("tbz", "tbnz"):
            v = self.get(st, reg(x.ops[0]))
            b = _imm(x.ops[1])
            if is_const(v) and b is not None:
                z = not (v[1] >> b & 1)
                return z if mn == "tbz" else not z
            return None
        f = st.get("flags")
        if not (f and isinstance(f[0], tuple) and f[0][0] == "op" and f[0][1] in ("cmp", "cmn") and len(f[0]) == 5):
            return None
        a, b, w = f[0][2], f[0][3], f[0][4]
        if not (is_const(a) and is_const(b)):
            return None
        bits = 32 if w == "w" else 64
        mask = (1 << bits) - 1
        ua, ub = a[1] & mask, (b[1] if f[0][1] == "cmp" else -b[1]) & mask
        sa = ua - (1 << bits) if ua >> (bits - 1) else ua
        sb = ub - (1 << bits) if ub >> (bits - 1) else ub
        c = mn.split(".")[1] if "." in mn else ""
        table = {"eq": ua == ub, "ne": ua != ub, "gt": sa > sb, "ge": sa >= sb, "lt": sa < sb, "le": sa <= sb,
                 "hi": ua > ub, "hs": ua >= ub, "cs": ua >= ub, "lo": ua < ub, "cc": ua < ub, "ls": ua <= ub}
        return table.get(c)

    def branch(self, st, x, k, record):
        d = self.decide(st, x)
        st["_br"] = d
        if d is not None:
            return  # a constant condition (gabi::ArgPack's register count): no branch in the program
        mn = x.mn
        if mn in ("cbz", "cbnz", "tbz", "tbnz"):
            cv = self.get(st, reg(x.ops[0]))
        else:
            cv = st["flags"]
        tb = self.block_of.get(self.idx.get(x.target)) if x.target is not None else None
        guard = tb in self.guard_block or _is_guardval(cv)
        if guard:
            return
        self.emit(record, "br", x, k, val=cv, extra=mn.split(" ")[0])

    def call(self, st, x, k, record):
        sym = x.reloc or "?"
        R = st["r"]
        cid = self.canon.get(x.addr)
        if sym == "__ZTWN4gabi3cpuE":
            R["x0"] = (("tlsaddr", "__ZN4gabi3cpuE"), 0)
            return
        m = GMEM.match(sym)
        if m:
            a = self.get(st, "x0")
            w = {"8": 1, "16": 2, "32": 4, "64": 8, "s": 4, "d": 8}.get(m.group(2) or "", 4)
            if m.group(1) in ("lfd", "stfd"):
                w = 8
            ga = _guest(a)
            if m.group(1).startswith("st"):
                val = self.get(st, "x1") if m.group(1) == "st" else self.get(st, "v0") if False else self.get(st, "x1")
                self.emit(record, "gw", x, k, space="guest", addr=ga, width=w, val=val)
                self.clobber_regs(st, x)
                st["ver"]["guest"] = ("st", self.canon_prefix, x.addr)
            else:
                self.emit(record, "gr", x, k, space="guest", addr=ga, width=w)
                v = (("ld", "guest", ga, w, st["ver"]["guest"]), 0)
                self.clobber_regs(st, x)
                R["x0"] = v
            return
        if PURE_CALLS.match(sym):
            if sym == "_gmem_stack_fresh":
                pass
            self.clobber_regs(st, x)
            return
        fm = FL_CTOR.match(sym) or LOCAL_CTOR.match(sym)
        if fm:
            slot = self.get(st, "x0")
            off = self.get(st, "x1") if FL_CTOR.match(sym) else None
            key = ("fl" if FL_CTOR.match(sym) else "loc",) + (cid,)
            size = ctor_size(self.funcs, sym)
            o = Obj(key, key[0], demangle_type(fm.group(1)), off[1] if off and is_const(off) else None, size, x,
                    self.chain, k)
            if record:
                self.objs[key] = o
            self.objs_all = getattr(self, "objs_all", {})
            self.objs_all[key] = o
            sp_, so = self.space_of(slot)
            self.clobber_regs(st, x)
            if sp_ == "hstack":
                # FrameLocal<T>/Local<T> {u32 a; u32 prev; bool in_frame}: a = the object's guest address; the
                # automatic native frame is assumed active (in_frame = 1, the harness's normal case), so the
                # destructor's r1 restore is not a branch of the program
                hs_store(st["hs"], slot, 4, (key, 0))
                hs_store(st["hs"], addk(slot, 4), 4, (("prev_r1", key), 0))
                hs_store(st["hs"], addk(slot, 8), 1, C(1))
            self.emit(record, "obj", x, k, sym=sym, extra=key, val=off)
            return
        if AUTOFRAME.match(sym):
            self.emit(record, "frame", x, k, sym=sym)
            self.clobber(st, x)
            return
        if DO_CALL.match(sym):
            tgt = self.get(st, "x2")
            args = {}
            for n in range(3, 11):
                v = st["cpu"].get(CPU_R + 4 * n)
                if v is not None:
                    args["r%d" % n] = v
            for n in range(1, 9):
                v = st["cpu"].get(CPU_F + 16 * n)
                if v is not None:
                    args["f%d" % n] = v
            for b in range(32):
                v = st["cpu"].get(CPU_CR + b)
                if v is not None:
                    args["cr%d" % b] = v
            self.emit(record, "gcall", x, k, target=tgt, args=args, sym=sym)
            self.calls.append((k, x, tgt, args))
            self.clobber(st, x)
            R["x0"] = (("ret", cid), 0)
            return
        if NORETURN.match(sym):
            self.emit(record, "trap", x, k, sym=sym)
            return
        if sym in self.funcs and self.depth < 3 and sym not in self.chain_syms():
            if record:
                entry = {"r": {r: v for r, v in R.items() if r.startswith("x") and int(r[1:]) < 8 or
                               r.startswith("v") and int(r[1:]) < 8}, "cpu": st["cpu"]}
                sub = Analysis(self.funcs, sym, self.livein, self.depth + 1, self.chain + (x.addr,), entry,
                               cid, self.glue).run()
                sub.inl_sym = sym
                for e in sub.events:
                    e.node = k
                    e.pos = (k,) + tuple(e.chain[1:]) + (e.ins.addr,)
                    if e.kind == "ret":
                        continue
                    self.events.append(e)
                for key, o in sub.objs.items():
                    o.node = k
                    self.objs[key] = o
                self.phi_in = getattr(self, "phi_in", {})
                self.phi_in.update(sub.phi_in)
                self.calls.extend((k, cx, t, a) for (_, cx, t, a) in sub.calls)
            self.clobber(st, x)
            R["x0"] = (("ret", cid), 0)
            return
        self.emit(record, "hcall", x, k, sym=sym, args={i: self.get(st, "x%d" % i) for i in range(8)})
        self.clobber(st, x)
        R["x0"] = (("ret", cid), 0)

    def chain_syms(self):
        return {self.name}

    def clobber_regs(self, st, x):
        """a gmem_* / pure helper: caller-saved registers only (no memory version change)"""
        cid = self.canon.get(x.addr, (("?", x.addr),))
        for r in CALLER_SAVED:
            st["r"][r] = (("clob", cid, r), 0)


def hs_store(hs, a, w, v):
    """host stack slot write: (base, off) -> (value, width); overlapped older slots are dropped"""
    for key in [k for k in hs if k[0] == a[0] and k[1] < a[1] + w and a[1] < k[1] + hs[k][1]]:
        del hs[key]
    hs[(a[0], a[1])] = (v, w)


def hs_load(hs, a, w, ver):
    for (b, o), (v, sw) in hs.items():
        if b != a[0] or not (o <= a[1] and a[1] + w <= o + sw):
            continue
        if o == a[1] and sw == w:
            return v
        if is_const(v):
            sh = (a[1] - o) * 8
            return C((v[1] >> sh) & ((1 << (8 * w)) - 1))
        if o == a[1] and w >= 4 and sw <= 8:
            return v  # a narrower load of a pointer/value: its low part (little endian)
        break
    return (("ld", "hstack", a, w, ver), 0)


def _guest(a):
    """guest effective address of a host pointer argument (strip the PPC_MEM_BASE offset)"""
    if a is None:
        return a
    off = a[1]
    if off >= HOSTMAP and off < HOSTMAP + (1 << 33):
        off -= HOSTMAP
    return (a[0], off & 0xFFFFFFFF if a[0] is None else off)


def _is_guardval(v):
    """GABI_GUEST_GUARD host-window check: cbz/cbnz on (pointer + K) >> 41"""
    if v is None or not isinstance(v[0], tuple):
        return False
    b = v[0]
    return len(b) >= 4 and b[0] == "op" and b[1] == "lsr" and b[3] == C(41)


def _lsl(ops):
    for o in ops:
        m = re.match(r"lsl #(\d+)", o.strip())
        if m:
            return int(m.group(1))
    return 0


def _shift_imm(ops):
    return (_imm(ops[1]) or 0) << _lsl(ops)


def _copy(st):
    return {"r": dict(st["r"]), "hs": dict(st["hs"]), "cpu": dict(st["cpu"]), "ver": dict(st["ver"]),
            "flags": st["flags"], "_br": st.get("_br")}


_CTOR_SIZE = {}


def ctor_size(funcs, sym):
    """sizeof(T) of FrameLocal<T>/Local<T>: the third argument of its gmem_stack_fresh(a, kSize, sizeof(T)) call"""
    if (id(funcs), sym) in _CTOR_SIZE:
        return _CTOR_SIZE[(id(funcs), sym)]
    size = None
    body = funcs.get(sym, [])
    for i, x in enumerate(body):
        if x.mn == "bl" and x.reloc == "_gmem_stack_fresh":
            for y in reversed(body[max(0, i - 6):i]):
                if y.mn == "mov" and y.ops[0] in ("w2", "x2") and y.ops[1].startswith("#"):
                    size = _imm(y.ops[1])
                    break
            break
    _CTOR_SIZE[(id(funcs), sym)] = size
    return size


# ------------------------------------------------------------------------------------------------- descriptors
_PHI = {}  # phi value -> incoming values, of the analysis whose events are being described (set by describe())
_MEMO = {}  # value -> description, per describe()/cdesc() call (a phi under evaluation maps to ("loop",))


def canon(v, depth=0, seen=None):
    """a version-free structural description of a value (for matching base and mutant effects); a phi is
    described by the set of its incoming values (no register names, no block numbers), and a phi whose inputs all
    describe the same value is that value"""
    if v is None or depth > 64:
        return None
    if isinstance(v, tuple) and len(v) == 2 and v[0] == "stale":
        return ("stale", canon(v[1], depth + 1))
    if not (isinstance(v, tuple) and len(v) == 2):
        return v
    try:
        hit = _MEMO.get(v)
    except TypeError:
        hit = None
    if hit is not None:
        return hit
    b, off = v
    if b is None:
        return (None, off)
    if isinstance(b, tuple) and b and b[0] == "phi" and b in _PHI:
        key = (b, 0)
        LOOP = (("phi-cycle",), 0)  # a phi reached again while it is being described (a loop-carried value)
        if key in _MEMO:
            r = _MEMO[key]
        else:
            _MEMO[key] = LOOP
            inc = {canon(x, depth + 1) for x in _PHI[b]}
            inc.discard(LOOP)  # only the phi itself, unchanged around the loop
            if len(inc) == 1:
                r = next(iter(inc))
            elif not inc:
                r = LOOP
            else:
                r = (("phi", frozenset(inc)), 0)
            _MEMO[key] = r
        if r is None or not (isinstance(r, tuple) and len(r) == 2):
            return r
        if not isinstance(r[1], int):  # e.g. ("stale", value): keep it whole
            return (("wrapped", r), off)
        out = (r[0], (r[1] + off) & M64)
        if r == LOOP:
            return out  # not memoised: its meaning depends on the phi under evaluation
    else:
        out = (canon_base(b, depth), off)
    _MEMO[v] = out
    return out


def canon_base(b, depth, seen=None):
    if not isinstance(b, tuple):
        return b
    if b[0] == "ld":
        return ("ld", b[1], canon(b[2], depth + 1) if isinstance(b[2], tuple) and len(b[2]) == 2 else b[2], b[3])
    if b[0] == "op":
        return ("op",) + tuple(canon(p, depth + 1) if isinstance(p, tuple) and len(p) == 2 and
                               (p[0] is None or isinstance(p[0], tuple)) else p for p in b[1:])
    if b[0] == "phi":
        return ("phi", b[3])  # no incoming facts (not recorded)
    return b


def cdesc(v, an):
    global _PHI, _MEMO
    _PHI, _MEMO = an.phi_in, {}
    try:
        return canon(v)
    finally:
        _PHI, _MEMO = {}, {}


def describe(events, an):
    """descriptors of an analysis's events"""
    global _PHI, _MEMO
    _PHI, _MEMO = an.phi_in, {}
    try:
        return [ev_desc(e) for e in events]
    finally:
        _PHI, _MEMO = {}, {}


def ev_desc(e):
    if e.is_mem():
        return (e.kind, e.space, canon(e.addr), e.width, canon(e.val) if e.is_write() else None)
    if e.kind == "gcall":
        return ("gcall", canon(e.target), tuple(sorted((k, canon(v)) for k, v in e.args.items())))
    if e.kind == "hcall":
        return ("hcall", e.sym, tuple(sorted((k, canon(v)) for k, v in (e.args or {}).items())))
    if e.kind == "br":
        return ("br", e.extra, canon(e.val))
    if e.kind == "obj":
        return ("obj", e.sym, e.extra, canon(e.val))
    if e.kind == "frame":
        return ("frame", e.sym)
    if e.kind == "ret":
        return ("ret", canon(e.val[0]) if e.val else None, canon(e.val[1]) if e.val else None)
    return (e.kind, e.sym)


def fmt_val(v):
    if v is None:
        return "?"
    if isinstance(v, tuple) and len(v) == 2 and v[0] == "stale":
        return "stale(" + fmt_val(v[1]) + ")"
    b, off = v
    if b is None:
        return "0x%X" % off
    return "%s%s" % (fmt_base(b), ("+0x%X" % off) if off and off < 1 << 40 else ("-0x%X" % ((-off) & M64)) if off else "")


def fmt_base(b):
    if not isinstance(b, tuple):
        return str(b)
    if b[0] == "arg":
        return "x%d@entry" % b[1]
    if b[0] == "farg":
        return "v%d@entry" % b[1]
    if b[0] == "tlsval":
        return "cpu" if b[1] == "__ZN4gabi3cpuE" else "tls:" + b[1]
    if b[0] == "ld":
        a = b[2]
        if b[1] == "cpu":
            return "cpu.%s" % cpu_name(a if isinstance(a, int) else 0)
        return "ld%d[%s]" % (b[3] * 8, fmt_val(a) if isinstance(a, tuple) else a)
    if b[0] in ("fl", "loc"):
        return "%s#%s" % ("FrameLocal" if b[0] == "fl" else "Local", _cid(b[1]))
    if b[0] == "clob":
        return "after(%s).%s" % (_cid(b[1]), b[2])
    if b[0] == "ret":
        return "ret(%s)" % _cid(b[1])
    if b[0] == "op":
        return "%s(%s)" % (b[1], ",".join(fmt_val(p) if isinstance(p, tuple) and len(p) == 2 and
                                           (p[0] is None or isinstance(p[0], tuple)) else str(p) for p in b[2:]))
    if b[0] == "phi":
        return "phi(%s)" % (b[4],)
    if b[0] == "sym":
        return b[1]
    if b[0] == "sp":
        return "hostsp%d" % b[1]
    return repr(b)


def _cid(c):
    if not c:
        return "?"
    return "/".join("%s#%d" % (_short(s), n) for s, n in c)


def _short(s):
    s = str(s)
    m = re.match(r"__Z\w*?\d+(\w+?)E", s)
    if s.startswith("__ZN4gabi7do_call"):
        return "do_call"
    return (m.group(1) if m else s)[:40]


def fmt_event(e, objs=None, layout=None):
    w = e.where()
    if e.is_mem():
        s = "%s %s %d bytes at %s" % ({"gr": "guest load", "gw": "guest store", "cr": "cpu load", "cw": "cpu store",
                                         "hr": "host load", "hw": "host store"}[e.kind], w, e.width, fmt_addr(e, layout))
        if e.is_write():
            s += " <- %s" % fmt_val(e.val)
        return s
    if e.kind == "gcall":
        return "guest call %s to %s (%s)" % (w, fmt_val(e.target), ", ".join(
            "%s=%s" % (k, fmt_val(v)) for k, v in sorted(e.args.items(), key=lambda kv: _argkey(kv[0]))))
    if e.kind == "hcall":
        return "host call %s %s" % (w, e.sym)
    if e.kind == "br":
        return "conditional branch %s (%s on %s)" % (w, e.extra, fmt_val(e.val))
    if e.kind == "ret":
        return "return %s (x0=%s, v0=%s)" % (w, fmt_val(e.val[0]), fmt_val(e.val[1])) if e.val else "return %s" % w
    if e.kind == "obj":
        return "frame object %s at %s (offset %s)" % (_cid(e.extra[1]), w, fmt_val(e.val))
    return "%s %s %s" % (e.kind, w, e.sym or "")


def _argkey(k):
    m = re.match(r"([a-z]+)(\d+)", k)
    return (m.group(1), int(m.group(2))) if m else (k, 0)


def fmt_addr(e, layout=None):
    if e.space == "cpu":
        return "cpu->%s" % cpu_name(e.addr[1])
    a = e.addr
    if layout and a and isinstance(a[0], tuple) and a[0] in layout:
        o = layout[a[0]]
        if o.off is not None:
            return "%s (%s, frame sp+0x%X)" % (fmt_val(a), o.type, o.off + a[1])
    return fmt_val(a)


# ------------------------------------------------------------------------------------------------ external facts
def load_livein(path):
    out = {}
    if path and os.path.exists(path):
        for line in open(path):
            p = line.split()
            if len(p) >= 3:
                out[int(p[0], 16)] = (int(p[1], 16), int(p[2], 16))
    return out


def load_frames(path):
    """frames.tsv: ADDR SIZE ITEMS -> {addr: (size, [(off, n, what)])} (items: b back chain, g:REG:OFF GPR save,
    d/s0/s1/p FPR saves, l LR (caller frame), o:OFF other)"""
    out = {}
    if not path or not os.path.exists(path):
        return out
    for line in open(path):
        p = line.rstrip("\n").split("\t")
        if len(p) < 2 or not re.match(r"^[0-9A-Fa-f]{8}$", p[0]):
            continue
        try:
            size = int(p[1])
        except ValueError:
            continue
        slots = []
        for it in (p[2].split() if len(p) > 2 else []):
            if it == "b":
                slots.append((0, 4, "back chain"))
                continue
            k, *rest = it.split(":")
            if k in ("l", "o") or len(rest) < 2:
                continue
            n = {"g": 4, "d": 8, "s0": 4, "s1": 4, "p": 8}.get(k, 4)
            slots.append((int(rest[1], 16), n, {"g": "r", "d": "f", "s0": "f", "s1": "ps1 f", "p": "psq f"}.get(k, k) + rest[0]))
        out[int(p[0], 16)] = (size, slots)
    return out


class Image:
    """build/verify/image.bin (mkimage.py): (addr, len, data) sections"""

    def __init__(self, path):
        self.secs = []
        if path and os.path.exists(path):
            data = open(path, "rb").read()
            i = 0
            while i + 8 <= len(data):
                a, n = struct.unpack("<II", data[i:i + 8])
                self.secs.append((a, data[i + 8:i + 8 + n]))
                i += 8 + n

    def cstr(self, a, limit=512):
        for base, d in self.secs:
            if base <= a < base + len(d):
                o = a - base
                e = d.find(b"\0", o, o + limit)
                if e < 0:
                    return None
                try:
                    return d[o:e].decode("latin-1")
                except Exception:
                    return None
        return None


_WDIS = {}


def wdis(root, addr):
    """the original's disassembly (tools/verify/wdis.py ADDR of `root`)"""
    if addr in _WDIS:
        return _WDIS[addr]
    t = ""
    try:
        r = subprocess.run([sys.executable, "-B", os.path.join(root, "tools", "verify", "wdis.py"), "%08X" % addr],
                           cwd=root, capture_output=True, text=True, timeout=120)
        t = r.stdout
    except Exception:
        pass
    _WDIS[addr] = t
    return t


def callee_cr_reads(root, addr, limit=40):
    """CR bits the callee reads before writing their field, from its entry (straight-line scan of the first
    `limit` instructions): {bit: 'bne cr1 at ADDR'}; variadic = bit 6 read before an FPR save sequence"""
    t = wdis(root, addr)
    reads, written = {}, set()
    fprsaves = 0
    for line in t.split("\n")[1:limit + 1]:
        m = re.match(r"\s*([0-9a-f]{8})\s+(\S+)\s*(.*)$", line)
        if not m:
            continue
        a, mn, ops = m.group(1), m.group(2), m.group(3)
        if mn.startswith("stfd") and re.match(r"f[1-8],", ops.replace(" ", "")):
            fprsaves += 1
        bm = re.match(r"b(eq|ne|lt|ge|gt|le|so|ns)(lr|ctr)?[-+]?\s+cr(\d)", mn + " " + ops)
        if bm:
            fld = int(bm.group(3))
            bit = 4 * fld + {"lt": 0, "ge": 0, "gt": 1, "le": 1, "eq": 2, "ne": 2, "so": 3, "ns": 3}[bm.group(1)]
            if fld not in written:
                reads.setdefault(bit, "%s %s at %s" % (mn, ops.strip(), a))
            continue
        bc = re.match(r"bc[l]?[a]?\s+(\d+),\s*(\d+)", mn + " " + ops)
        if bc:
            bit = int(bc.group(2))
            if bit // 4 not in written:
                reads.setdefault(bit, "%s %s at %s" % (mn, ops.strip(), a))
            continue
        cm = re.match(r"(cmp\w*|fcmp\w*)\s+cr(\d)", mn + " " + ops)
        if cm:
            written.add(int(cm.group(2)))
        if mn.endswith(".") or (mn.startswith("cmp") and "cr" not in ops):
            written.add(0)
        cr = re.match(r"cr(clr|set|xor|or|and|nor|eqv|nand|andc|orc)\s+(\d+)", mn + " " + ops)
        if cr:
            pass  # bit writes: a later read of the same bit sees the new value
        if mn in ("mtcrf", "mtcr", "mcrf"):
            break
        if mn in ("blr", "b") or mn.startswith("bl"):
            break
    return reads, fprsaves


FPCONV = re.compile(r"%[-+ #0']*(\d+|\*)?(\.(\d+|\*))?(hh|h|ll|l|L|z|j|t|q)?[eEfFgGaA]")


# --------------------------------------------------------------------------------------------- compile / disasm
class Builder:
    """compile a source text exactly like mutate.py's per-mutant build (autoglue, -O1, verify.FP), disassemble"""

    def __init__(self, root, tree, scratch):
        self.root, self.tree, self.scratch = root, tree, scratch
        tv = os.path.join(tree, "tools", "verify")
        if tv not in sys.path:
            sys.path.insert(0, tv)
        import autoglue  # noqa: E402  (the tree's own version)
        import verify  # noqa: E402
        autoglue.ROOT = tree
        self.autoglue, self.FP = autoglue, verify.FP
        self.inc = ["-Iruntime/include", "-Itools/verify/include", "-Itools/verify/src", "-Iwwhd_src/include"]
        self.cache = {}

    def build_funcs(self, src, text, tag, keep):
        """compile + disassemble and parse: ({symbol: [Ins]}, error). Without `keep` the files go to one slot per
        worker thread, overwritten by the next mutant (scratch stays small); the base objects are always kept."""
        if not keep:
            tag = "slot-%s" % threading.current_thread().name.replace("/", "_")
        dis, err = self.build(src, text, tag, cache=keep)
        if not dis:
            return None, err
        return (load_dis(dis) if keep else parse_objdump(open(dis).read())), None

    def build(self, src, text, tag, cache=True):
        d = os.path.join(self.scratch, "obj", tag)
        os.makedirs(d, exist_ok=True)
        p = os.path.join(d, os.path.basename(src))
        dis = p + ".dis"
        if cache and os.path.exists(dis) and os.path.exists(p) and open(p).read() == self._compiled(src, text):
            return dis, None
        body = self._compiled(src, text)
        with open(p, "w") as fh:
            fh.write(body)
        r = subprocess.run(["clang++", "-std=c++20", "-O1", "-w"] + self.FP + self.inc +
                           ["-I" + os.path.dirname(os.path.join(self.tree, src)), "-c", p, "-o", p + ".o"],
                           cwd=self.tree, capture_output=True, text=True)
        if r.returncode:
            return None, r.stderr[-800:]
        with open(dis, "w") as fh:
            subprocess.run(["objdump", "-d", "-r", "--no-show-raw-insn", p + ".o"], stdout=fh, text=True)
        return dis, None

    def glue(self, src, text):
        try:
            g = self.autoglue.parse_glue(text)
        except Exception:
            return {}
        if not g:
            return {}
        return {name: v[0] for name, v in g[1].items()}

    def _compiled(self, src, text):
        key = (src, hash(text))
        if key not in self.cache:
            self.cache[key] = self.autoglue.transform(text, src) if "/* WWHD glue" in text else \
                '#line 1 "%s"\n' % src + text
        return self.cache[key]


_DIS = {}


def load_dis(path):
    if path not in _DIS:
        _DIS[path] = parse_objdump(open(path).read())
    return _DIS[path]


# ------------------------------------------------------------------------------------------------ the checks
class Ctx:
    def __init__(self, a):
        self.a = a
        self.livein = load_livein(a.livein)
        self.frames = load_frames(a.frames)
        self.image = Image(a.image)
        self.wdis_root = a.wdis_root
        self.keep_objects = getattr(a, "keep_objects", False)
        self.specs = load_specs(getattr(a, "tree", None) or getattr(a, "root", ROOT), getattr(a, "unit", None))

    def footprint_specs(self, func):
        """specs that compare the frame a function leaves (-footprint) and test `func`"""
        return [n for n, only in self.specs["footprint"] if only is None or func.upper() in only]

    def compared(self, target, regname):
        """pointee bytes the harness compares at a call (spec `callee` lines / callee_facts.tsv), or 0"""
        if not is_const(target):
            return 0
        return self.specs["compare"].get(((target[1] & 0xFFFFFFFF), regname), 0)


def load_specs(root, unit):
    """observation points of the unit's specs: -footprint headers (with their -only scope) and pointee compares
    (`callee ADDR rN=SIZE|outN|inoutN|heapN`, also tools/verify/callee_facts.tsv)"""
    out = {"footprint": [], "compare": {}, "files": []}
    files = []
    base = os.path.join(root, "tools", "verify", "units")
    if unit:
        if os.path.exists(os.path.join(base, unit + ".txt")):
            files.append(os.path.join(base, unit + ".txt"))
        d = os.path.join(base, unit)
        if os.path.isdir(d):
            files += sorted(os.path.join(d, f) for f in os.listdir(d) if f.endswith(".txt"))
    facts = os.path.join(root, "tools", "verify", "callee_facts.tsv")
    for path in files + ([facts] if os.path.exists(facts) else []):
        name = os.path.relpath(path, base) if path != facts else "callee_facts.tsv"
        out["files"].append(name)
        for line in open(path, errors="replace"):
            h = re.match(r"#\s*verify:\s*(.*)$", line)
            if h:
                flags = h.group(1).split()
                if "-footprint" in flags:
                    only = None
                    if "-only" in flags and flags.index("-only") + 1 < len(flags):
                        only = set(x.upper() for x in flags[flags.index("-only") + 1].split(","))
                    out["footprint"].append((name, only))
                continue
            line = line.split("#")[0].strip()
            m = re.match(r"^(?:callee\s+)?([0-9A-Fa-f]{8})\s+(.*)$", line) if (path == facts or line.startswith("callee ")) else None
            if not m:
                continue
            for kv in m.group(2).split():
                km = re.match(r"^([rf]\d+)=(?:inout|out|heap:?|dyn\d+\*r\d+:?)?(\d+|0x[0-9A-Fa-f]+)$", kv)
                if km:
                    key = (int(m.group(1), 16), km.group(1))
                    out["compare"][key] = max(out["compare"].get(key, 0), int(km.group(2), 0))
    return out


def reads_reg(ctx, target, regname):
    """does guest function `target` (value) read argument register regname at entry? (livein.tsv); None = unknown"""
    if not is_const(target):
        return None
    li = ctx.livein.get(target[1] & 0xFFFFFFFF)
    if li is None:
        return None
    m = re.match(r"([rf])(\d+)$", regname)
    if not m:
        return None
    n = int(m.group(2))
    return bool((li[0] if m.group(1) == "r" else li[1]) >> n & 1)


def obj_base(v):
    """the frame object a pointer value points into (its key), or None"""
    if v is None:
        return None
    if v[0] == "stale":
        return obj_base(v[1])
    b = v[0]
    if isinstance(b, tuple) and b and b[0] in ("fl", "loc"):
        return b
    return None


def escapes(ctx, an, persistent_only=False):
    """frame objects whose address escapes: stored anywhere (except argument registers), passed to an unknown host
    call, or in an argument register a guest call reads (livein; unknown callee = reads). -> {key: [reasons]}; plus
    {key: [(call event, reg)]} of the unread argument registers that carry it"""
    esc = collections.defaultdict(list)
    unread = collections.defaultdict(list)
    for e in an.events:
        if e.is_write() and obj_base(e.val):
            if e.space == "cpu" and e.addr[1] < 0x80 and 3 <= e.addr[1] // 4 <= 10:
                continue  # an argument register: judged at the call
            esc[obj_base(e.val)].append(("stored by %s" % fmt_event(e), e))
        elif e.kind == "hcall":
            for i, v in (e.args or {}).items():
                if obj_base(v):
                    esc[obj_base(v)].append(("passed to host call %s (%s)" % (e.sym, e.where()), e))
        elif e.kind == "gcall":
            for r, v in e.args.items():
                ob = obj_base(v)
                if ob and r[0] in "rf":
                    rd = reads_reg(ctx, e.target, r)
                    if rd is False:
                        unread[ob].append((e, r))
                    elif persistent_only:
                        continue  # passed for the duration of that call (callees do not keep stack pointers)
                    else:
                        esc[ob].append(("%s of guest call %s to %s (%s)" % (
                            r, e.where(), fmt_val(e.target), "reads it" if rd else "callee facts unknown"), e))
    return esc, unread


def align(fb, fm):
    """base instruction address -> mutant instruction address for instructions in equal runs of the two bodies"""
    nb = [x.norm() for x in fb]
    nm = [x.norm() for x in fm]
    amap = {}
    for tag, a1, a2, b1, b2 in difflib.SequenceMatcher(None, nb, nm, autojunk=False).get_opcodes():
        if tag == "equal":
            for k in range(a2 - a1):
                amap[fb[a1 + k].addr] = fm[b1 + k].addr
    return amap


def match_events(eb, em, amap=None, A=None, B=None):
    """pair base and mutant events: first the same instruction (alignment of identical code, equal descriptors),
    then equal descriptors (in order).
    -> (pairs [(ib, im)], base-only, mutant-only)"""
    db = describe(eb, A) if A is not None else [ev_desc(e) for e in eb]
    dm = describe(em, B) if B is not None else [ev_desc(e) for e in em]
    pairs, used_b, used_m = [], set(), set()
    if amap:
        where_m = {}
        for j, e in enumerate(em):
            where_m.setdefault(tuple(e.chain[1:]) + (e.ins.addr,), []).append(j)
        for i, e in enumerate(eb):
            key = tuple(e.chain[1:]) + (e.ins.addr,)
            top = amap.get(key[0])
            if top is None:
                continue
            for j in where_m.get((top,) + key[1:], []):
                if j not in used_m and dm[j] == db[i]:
                    pairs.append((i, j))
                    used_b.add(i)
                    used_m.add(j)
                    break
    for keyf in (lambda d: d,):
        pool = collections.defaultdict(list)
        for j, d in enumerate(dm):
            if j not in used_m:
                pool[keyf(d)].append(j)
        for i, d in enumerate(db):
            if i in used_b:
                continue
            k = keyf(d)
            if pool[k]:
                j = pool[k].pop(0)
                pairs.append((i, j))
                used_b.add(i)
                used_m.add(j)
    bonly = [i for i in range(len(db)) if i not in used_b]
    monly = [j for j in range(len(dm)) if j not in used_m]
    pairs.sort()
    return pairs, bonly, monly


def relevant(e):
    """effects compared between base and mutant (the marked function's own return value included: the entry wrapper
    stores it to r3/f1)"""
    if e.kind == "ret":
        return len(e.chain) == 1
    return e.kind not in ("obj", "trap")


def commute(ctx, x, y, an, esc, layout):
    """(True, why) when effects x and y (base events) commute, else (False, why)"""
    if x.kind == "br" or y.kind == "br":
        o = y if x.kind == "br" else x
        if o.kind == "br":
            return False, "two conditional branches change order"
        if o.is_read() and o.space in ("cpu", "host"):
            return True, "a host/cpu load moved across a branch"
        if o.is_read():
            return False, "a guest load moved across a conditional branch (it may fault on inputs the branch skips)"
        return False, "%s moved across a conditional branch" % o.kind
    if x.kind == "ret" or y.kind == "ret":
        return False, "an effect moved across the function's return"
    if x.kind == "frame" or y.kind == "frame":
        o = y if x.kind == "frame" else x
        if o.is_mem() and o.space == "cpu" and o.addr[1] != 4:
            return True, "frame model vs cpu access (not r1)"
        if o.is_mem() and o.space == "guest":
            ob = obj_base(o.addr)
            if ob is None and not any(esc.get(k) for k in layout):
                return True, "frame model vs a guest access outside the frame (no frame object escapes)"
        if o.is_mem() and o.space == "host":
            return True, "frame model vs host access"
        return False, "frame model vs %s" % o.kind
    if x.is_mem() and y.is_mem():
        if x.is_read() and y.is_read():
            return True, "two loads"
        return disjoint(x, y, esc, layout)
    if x.kind in ("gcall", "hcall") and y.kind in ("gcall", "hcall"):
        return False, "two calls change order"
    c, m = (x, y) if x.kind in ("gcall", "hcall") else (y, x)
    if c.kind == "hcall":
        return False, "a host call %s and %s change order" % (c.sym, m.kind)
    if not m.is_mem():
        return False, "call vs %s" % m.kind
    if m.space == "cpu":
        off = m.addr[1]
        if off < 0x80 and off // 4 in (1, 2, 13) or 14 <= off // 4 <= 31 and off < 0x80:
            return True, "cpu %s (preserved/unused by the callee) vs guest call" % cpu_name(off)
        if off < 0x80 and 3 <= off // 4 <= 10 and m.is_write():
            rd = reads_reg(ctx, c.target, "r%d" % (off // 4))
            if rd is False:
                return True, "cpu store to %s, which callee %s does not read (live-in mask)" % (cpu_name(off), fmt_val(c.target))
        return False, "cpu %s vs guest call %s" % (cpu_name(off), fmt_val(c.target))
    if m.space == "host":
        return True, "host static (%s) vs guest call: guest code cannot reach host memory" % fmt_val(m.addr)
    ob = obj_base(m.addr)
    if ob is None:
        return False, "guest %s at %s vs guest call %s" % ("store" if m.is_write() else "load", fmt_val(m.addr),
                                                           fmt_val(c.target))
    if esc.get(ob):
        return False, "frame object %s escapes (%s)" % (_cid(ob[1]), esc[ob][0][0])
    unread = []
    for r, v in c.args.items():
        if obj_base(v) == ob:
            rd = reads_reg(ctx, c.target, r)
            if rd is not False:
                return False, "frame object %s is passed to the call in %s (%s)" % (
                    _cid(ob[1]), r, "read by the callee" if rd else "no live-in row")
            unread.append(r)
    if unread:
        return True, "frame object %s reaches guest call %s only in %s, which it does not read (live-in 0x%X)" % (
            _cid(ob[1]), fmt_val(c.target), "/".join(unread), ctx.livein.get(c.target[1] & 0xFFFFFFFF, (0, 0))[0])
    return True, "frame object %s never escapes and is not passed to guest call %s" % (_cid(ob[1]), fmt_val(c.target))


def disjoint(x, y, esc, layout):
    if x.space != y.space:
        return True, "different memories (%s vs %s)" % (x.space, y.space)
    if x.space == "cpu":
        if ("cpuidx",) in (x.addr[0], y.addr[0]):
            return False, "a Cpu-struct access at an unknown offset"
        a, b = x.addr[1], y.addr[1]
        if a + x.width <= b or b + y.width <= a:
            return True, "cpu %s vs %s" % (cpu_name(a), cpu_name(b))
        return False, "same cpu field %s" % cpu_name(a)
    ax, ay = x.addr, y.addr
    if ax[0] == ay[0]:
        if ax[1] + x.width <= ay[1] or ay[1] + y.width <= ax[1]:
            return True, "same base %s, offsets +0x%X/%d and +0x%X/%d" % (fmt_base(ax[0]) if ax[0] else "const",
                                                                         ax[1] & 0xFFFFFFFF, x.width, ay[1] & 0xFFFFFFFF, y.width)
        return False, "overlapping accesses at %s and %s" % (fmt_val(ax), fmt_val(ay))
    ox, oy = obj_base(ax), obj_base(ay)
    if ox and oy:
        lx, ly = layout.get(ox), layout.get(oy)
        if lx and ly and lx.off is not None and ly.off is not None and lx.kind == ly.kind == "fl":
            fx, fy = lx.off + ax[1], ly.off + ay[1]
            if fx + x.width <= fy or fy + y.width <= fx:
                return True, "frame bytes sp+0x%X/%d vs sp+0x%X/%d" % (fx, x.width, fy, y.width)
            return False, "frame bytes overlap: sp+0x%X/%d vs sp+0x%X/%d" % (fx, x.width, fy, y.width)
        return False, "two stack objects without known frame offsets"
    if ox or oy:
        o = ox or oy
        if not esc.get(o):
            return True, "frame object %s never escapes; the other access is through %s" % (
                _cid(o[1]), fmt_val(ay if ox else ax))
        return False, "frame object %s escapes (%s)" % (_cid(o[1]), esc[o][0][0])
    if ax[0] is None and ay[0] is None:
        a, b = ax[1], ay[1]
        if a + x.width <= b or b + y.width <= a:
            return True, "constant addresses 0x%X/%d vs 0x%X/%d" % (a, x.width, b, y.width)
        return False, "same constant address"
    return False, "different bases may alias: %s vs %s" % (fmt_val(ax), fmt_val(ay))


def check_swap(ctx, A, B, ev_b, ev_m, amap=None):
    pairs, bonly, monly = match_events(ev_b, ev_m, amap, A, B)
    lines = []
    if bonly or monly:
        lines.append("effects that differ (not a permutation):")
        lines += ["  base only:   " + fmt_event(ev_b[i], layout=A.objs) for i in bonly[:12]]
        lines += ["  mutant only: " + fmt_event(ev_m[j], layout=B.objs) for j in monly[:12]]
        return "dependent", "the swap changes what is computed: %d base-only / %d mutant-only effect(s), first: %s" % (
            len(bonly), len(monly), fmt_event((ev_b[bonly[0]] if bonly else ev_m[monly[0]]))), lines
    esc, _ = escapes(ctx, A, persistent_only=True)
    # inverted pairs (linear order in base vs in mutant)
    order_m = {i: j for i, j in pairs}
    inv = []
    idx = sorted(order_m)
    pos_m = [order_m[i] for i in idx]
    for a in range(len(idx)):
        for b in range(a + 1, len(idx)):
            if pos_m[a] > pos_m[b]:
                inv.append((idx[a], idx[b]))
    if not inv:
        return "EQ", "base and mutant make the same effects in the same order (the swapped statements compile to " \
                     "the same effect sequence)", ["no inverted effect pair"]
    bad = []
    lines.append("%d inverted effect pair(s):" % len(inv))
    for i, j in inv:
        ok, why = commute(ctx, ev_b[i], ev_b[j], A, esc, A.objs)
        lines.append("  %s | %s  ->  %s" % (fmt_event(ev_b[i], layout=A.objs), fmt_event(ev_b[j], layout=A.objs),
                                          ("commutes: " if ok else "OPEN: ") + why))
        if not ok:
            bad.append((i, j, why))
    if bad:
        i, j, why = bad[0]
        return "open", "%d of %d inverted pair(s) not shown to commute; first: %s / %s: %s" % (
            len(bad), len(inv), fmt_event(ev_b[i], layout=A.objs), fmt_event(ev_b[j], layout=A.objs), why), lines
    moved = sorted({ev_b[i].where() for i, j in inv} | {ev_b[j].where() for i, j in inv})
    kinds = collections.Counter()
    for i, j in inv:
        kinds[commute(ctx, ev_b[i], ev_b[j], A, esc, A.objs)[1].split(":")[0].split(",")[0]] += 1
    return "EQ", "independent on the compiled candidate: %d inverted effect pair(s) all commute (%s)" % (
        len(inv), "; ".join("%d x %s" % (n, k) for k, n in kinds.most_common(4))), lines


# ---------------------------------------------------------------------------- frame shift: byte liveness on the CFG
def frame_events(ctx, an, layout):
    """per CFG node (block index): ordered list of byte events (pos, owner, 'r'|'w'|'mw', frame byte offsets) in the
    candidate's frame, from direct guest accesses to frame objects and guest calls that read a pointer to one"""
    esc, unread = escapes(ctx, an, persistent_only=True)
    ev = collections.defaultdict(list)
    for e in an.events:
        if e.is_mem() and e.space == "guest":
            ob = obj_base(e.addr)
            if ob and ob in layout and layout[ob].off is not None and layout[ob].kind == "fl":
                lo = layout[ob].off + e.addr[1]
                ev[e.node].append((e.pos, ob, "r" if e.is_read() else ("w" if not e.chain[1:] else "w"),
                                   set(range(lo, lo + e.width)), e))
        elif e.kind == "gcall":
            for r, v in e.args.items():
                ob = obj_base(v)
                if ob and ob in layout and layout[ob].off is not None and r[0] in "rf":
                    if reads_reg(ctx, e.target, r) is False:
                        continue
                    o = layout[ob]
                    sz = o.size or 4
                    span = set(range(o.off, o.off + sz))
                    cmp_ = ctx.compared(e.target, r)
                    if cmp_:
                        # the harness compares cmp_ pointee bytes at this call (spec `callee` line): a direct read,
                        # stale bytes included
                        lo = o.off + ptr_off(v)
                        ev[e.node].append((e.pos, ob, "r", set(range(lo, lo + cmp_)), e))
                    ev[e.node].append((e.pos, ob, "cr", span, e))
                    ev[e.node].append((e.pos + (1,), ob, "mw", span, e))
            # objects escaped earlier (on a path to this call): any guest call may access them
            for ob, why in esc.items():
                if not any(reaches(an, w[1], e) for w in why):
                    continue
                if ob in layout and layout[ob].off is not None and not any(obj_base(v) == ob for v in e.args.values()):
                    o = layout[ob]
                    span = set(range(o.off, o.off + (o.size or 4)))
                    ev[e.node].append((e.pos, ob, "cr", span, e))
                    ev[e.node].append((e.pos + (1,), ob, "mw", span, e))
        elif e.kind == "hcall":
            for i, v in (e.args or {}).items():
                ob = obj_base(v)
                if ob and ob in layout and layout[ob].off is not None:
                    o = layout[ob]
                    span = set(range(o.off, o.off + (o.size or 4)))
                    ev[e.node].append((e.pos, ob, "r", span, e))
                    ev[e.node].append((e.pos + (1,), ob, "mw", span, e))
    for k in ev:
        ev[k].sort(key=lambda t: t[0])
    return ev


def ptr_off(v):
    """offset of a pointer argument within its object (guest addresses are 32-bit; host views carry PPC_MEM_BASE)"""
    o = v[1] & 0xFFFFFFFF
    return o - (1 << 32) if o >> 31 else o


def reaches(an, a, b):
    """can execution go from event a to event b (CFG, a strictly before b)?"""
    if a.node == b.node and a.pos < b.pos:
        return True
    rs = getattr(an, "_reach_cache", None)
    if rs is None:
        rs = an._reach_cache = {}
    if a.node not in rs:
        seen, todo = set(), list(an.succ[a.node])
        while todo:
            k = todo.pop()
            if k in seen:
                continue
            seen.add(k)
            todo.extend(an.succ[k])
        rs[a.node] = seen
    return b.node in rs[a.node]


def byte_flow(an, ev, byte, S):
    """dataflow on byte `byte` of the mutant's frame: who wrote it last (owner set) on reaching each event.
    Returns the list of problems: (kind, event, writers)"""
    nb = len(an.blocks)
    # state: set of possible last writers ('entry' = stale frame data)
    IN = {0: frozenset(["entry"])}
    OUT = {}
    todo = collections.deque([0])
    probs = []
    ret_blocks = {k for k in range(nb) if an.blocks[k][1] > an.blocks[k][0] and
                  an.ins[an.blocks[k][1] - 1].mn in ("ret",) or (an.ins[an.blocks[k][1] - 1].mn == "b" and
                                                               an.ins[an.blocks[k][1] - 1].reloc)}

    def transfer(k, s, record):
        for pos, owner, rw, bytes_, e in ev.get(k, []):
            if byte not in bytes_:
                continue
            if rw == "w":
                s = frozenset([owner])
            elif rw == "mw":
                s = s | {owner}
            else:
                if record:
                    # a callee reading through a pointer (rw == "cr") may only fill an output: stale bytes there
                    # are not counted, foreign data is
                    other = s - {S} if rw == "r" else s - {S, "entry"}
                    if owner == S and other:
                        probs.append(("S-reads-foreign", e, s, owner, rw))
                    elif owner != S and S in s:
                        probs.append(("foreign-reads-S", e, s, owner, rw))
        return s

    steps = 0
    while todo and steps < 100000:
        steps += 1
        k = todo.popleft()
        s = transfer(k, IN[k], False)
        if OUT.get(k) == s:
            continue
        OUT[k] = s
        for o in an.succ[k]:
            new = IN.get(o, frozenset()) | s
            if new != IN.get(o):
                IN[o] = new
                todo.append(o)
    for k in range(nb):
        if k in IN:
            s = transfer(k, IN[k], True)
            if k in ret_blocks:
                probs.append(("at-exit", None, s))
    return probs


def check_frame(ctx, func, A, B, src_lines=None):
    """FrameLocal shift: objects whose frame offset differs between base A and mutant B"""
    shifted = []
    for key, ob in B.objs.items():
        oa = A.objs.get(key)
        if oa and oa.off is not None and ob.off is not None and oa.off != ob.off:
            shifted.append((key, oa, ob))
    if not shifted:
        return None
    lines = []
    fr = ctx.frames.get(int(func, 16))
    fsize, slots = fr if fr else (None, [])
    verdict, reasons = "EQ", []
    for key, oa, ob in shifted:
        size = ob.size
        if size is None:
            return "open", "FrameLocal %s shifted 0x%X -> 0x%X but its size is unknown" % (ob.type, oa.off, ob.off), lines
        old, new = set(range(oa.off, oa.off + size)), set(range(ob.off, ob.off + size))
        outb = sorted(new - old)
        lines.append("shifted object %s %s (%d bytes, ctor %s): sp+0x%X..0x%X -> sp+0x%X..0x%X; out-of-span bytes %s" % (
            ob.type, _cid(key[1]), size, ob.ins.addr, oa.off, oa.off + size - 1, ob.off, ob.off + size - 1,
            ",".join("0x%X" % b for b in outb)))
        if fsize is None:
            lines.append("  no frames.tsv row for %s" % func)
            return "open", "FrameLocal shift without a frames.tsv row for %s" % func, lines
        hits = []
        for b in outb:
            for so, n, what in slots:
                if so <= b < so + n:
                    hits.append((b, so, n, what))
            if b >= fsize:
                hits.append((b, fsize, 8, "caller frame (back chain / LR save above sp+0x%X)" % fsize))
        if hits:
            b, so, n, what = hits[0]
            lines.append("  frames.tsv %s: frame 0x%X; byte sp+0x%X is in the %s slot sp+0x%X..0x%X" % (
                func, fsize, b, what, so, so + n - 1))
            return "RI", "FrameLocal shift: %s %s (0x%X bytes) moves from sp+0x%X..0x%X to sp+0x%X..0x%X in the 0x%X-byte " \
                         "frame of %s; byte sp+0x%X lands on the %s slot sp+0x%X..0x%X, which the epilogue restores " \
                         "(frames.tsv)" % (ob.type, _cid(key[1]), size, oa.off, oa.off + size - 1, ob.off,
                                           ob.off + size - 1, fsize, func, b, what, so, so + n - 1), lines
        # other frame objects overlapping the new span (context)
        others = [o for k2, o in B.objs.items() if k2 != key and o.kind == "fl" and o.off is not None and
                  o.size and set(range(o.off, o.off + o.size)) & set(outb)]
        for o in others:
            lines.append("  overlaps %s %s at sp+0x%X..0x%X (ctor %x)" % (o.type, _cid(o.key[1]), o.off,
                                                                         o.off + o.size - 1, o.ins.addr))
        ev = frame_events(ctx, B, B.objs)
        probs_all = []
        for b in outb:
            for p in byte_flow(B, ev, b, key):
                if p[0] == "at-exit":
                    continue
                probs_all.append((b, p))
        # read-before-own-write over the whole span (stale data)
        for b in sorted(new):
            for p in byte_flow(B, ev, b, key):
                if p[0] == "S-reads-foreign" and "entry" in p[2] and (b, p) not in probs_all:
                    probs_all.append((b, p))
        direct = [(b, p) for b, p in probs_all if p[4] != "cr"]
        if probs_all and not direct:
            b, p = probs_all[0]
            for bb, pp in probs_all[:8]:
                lines.append("  byte 0x%X: %s through a guest call at %s, last writers %s" % (
                    bb, pp[0], pp[1].where(), ",".join(_owner(o) for o in pp[2])))
            return "open", "FrameLocal shift: %s %s sp+0x%X -> sp+0x%X in %s: byte sp+0x%X is reached only through " \
                           "guest call %s, which receives a pointer to %s; whether the callee reads it before writing " \
                           "it (an output buffer) decides EQ vs RI" % (
                               ob.type, _cid(key[1]), oa.off, ob.off, func, b, fmt_val(p[1].target),
                               "the shifted object" if p[0] == "S-reads-foreign" else "the overlapped object"), lines
        if direct and all(p[1] is not None and p[1].kind == "gcall" for b, p in direct):
            b, p = direct[0]
            for bb, pp in direct[:8]:
                lines.append("  byte 0x%X: %s at the pointee compare of %s, last writers %s" % (
                    bb, pp[0], pp[1].where(), ",".join(_owner(o) for o in pp[2])))
            return "open", "FrameLocal shift %s sp+0x%X -> sp+0x%X in %s: the harness compares %d pointee bytes at " \
                           "guest call %s (spec callee line), which can then see byte sp+0x%X from %s: EQ only with a " \
                           "reason why the compared bytes stay equal" % (
                                    ob.type, oa.off, ob.off, func, ctx.compared(p[1].target, _arg_of(p[1], key) or "r3"),
                                    fmt_val(p[1].target), b, ",".join(_owner(o) for o in p[2] if o != key) or "the shifted object"), lines
        if direct:
            probs_all = direct
        if probs_all:
            b, p = probs_all[0]
            what = p[0]
            e = p[1]
            if what == "foreign-reads-S":
                why = "byte sp+0x%X: %s reads it after the shifted object's write (%s)" % (
                    b, fmt_event(e, layout=B.objs), "possible writers " + ",".join(_owner(o) for o in p[2]))
            else:
                why = "byte sp+0x%X: the shifted object reads it (%s) while it may hold %s" % (
                    b, fmt_event(e, layout=B.objs), ",".join(_owner(o) for o in p[2] if o != key))
            for bb, pp in probs_all[:8]:
                lines.append("  byte 0x%X: %s at %s, last writers %s" % (bb, pp[0], pp[1].where() if pp[1] else "-",
                                                                         ",".join(_owner(o) for o in pp[2])))
            return "RI", "FrameLocal shift: %s %s (0x%X bytes) sp+0x%X -> sp+0x%X in %s: %s" % (
                ob.type, _cid(key[1]), size, oa.off, ob.off, func, why), lines
        fps = ctx.footprint_specs(func)
        if fps:
            final = []
            for b in sorted(new | old):
                for an_ in (B, A):
                    evx = ev if an_ is B else frame_events(ctx, A, A.objs)
                    for p in byte_flow(an_, evx, b, key):
                        if p[0] == "at-exit" and key in p[2]:
                            final.append(b)
            if final:
                lines.append("  -footprint spec(s) %s compare the frame after return; the shifted object is a possible "
                             "final writer of sp+%s" % (", ".join(fps), ",".join("0x%X" % b for b in sorted(set(final))[:8])))
                return "open", "FrameLocal shift %s sp+0x%X -> sp+0x%X in %s can change the frame the function leaves " \
                               "(possible final writer of sp+0x%X), which -footprint spec %s compares: EQ only with a reason " \
                               "why no input of that spec sees a different final byte" % (
                                        ob.type, oa.off, ob.off, func, min(final), fps[0]), lines
        reasons.append("%s %s (0x%X bytes) moves from sp+0x%X..0x%X to sp+0x%X..0x%X in the 0x%X-byte frame of %s; "
                       "the out-of-span byte(s) %s overlap no save slot (frames.tsv) and on every path of the mutant's "
                       "compiled code no other object reads a byte the shifted object wrote, and the shifted object "
                       "reads only bytes it wrote itself%s" % (
                           ob.type, _cid(key[1]), size, oa.off, oa.off + size - 1, ob.off, ob.off + size - 1, fsize,
                           func, ",".join("sp+0x%X" % b for b in outb),
                           ("; overlapped objects: " + ", ".join("%s@sp+0x%X" % (o.type, o.off) for o in others))
                           if others else ""))
    return "EQ", "FrameLocal shift: " + "; ".join(reasons), lines


def _arg_of(e, key):
    for r, v in (e.args or {}).items():
        if obj_base(v) == key:
            return r
    return None


def _owner(o):
    if o == "entry":
        return "stale"
    if isinstance(o, tuple) and o[0] in ("fl", "loc"):
        return "obj " + _cid(o[1])
    return str(o)


# ------------------------------------------------------------------------- unread argument registers / CR bits
def next_consumers(ctx, an, ev_list, start_ev, off):
    """from cpu store start_ev of cpu field `off`, follow the CFG: the first consumers on every path:
    [('call', e, reads?) | ('read', e) | ('exit', blk)]; overwrite (a cpu store to off) ends a path"""
    order = collections.defaultdict(list)
    for e in ev_list:
        order[e.node].append(e)
    for k in order:
        order[k].sort(key=lambda e: e.pos)
    out = []
    seen = set()

    def scan(k, after):
        for e in order.get(k, []):
            if after is not None and e.pos <= after:
                continue
            if e.kind == "cw" and e.addr[1] == off:
                return "killed"
            if e.kind == "cr" and (e.addr[1] <= off < e.addr[1] + e.width or e.addr[0] == ("cpuidx",)):
                out.append(("read", e))
                return "done"
            if e.kind == "gcall":
                out.append(("call", e))
                return "call"
            if e.kind in ("hcall",) and e.sym not in ("indirect",):
                out.append(("hcall", e))
                return "done"
        return None

    todo = [(start_ev.node, start_ev.pos)]
    while todo:
        k, after = todo.pop()
        if (k, after) in seen:
            continue
        seen.add((k, after))
        r = scan(k, after)
        if r == "call":
            # a callee that does not read the register may leave it in place: keep following
            c = out[-1][1]
            todo.append((k, c.pos))
            continue
        if r in ("killed", "done"):
            continue
        if not an.succ[k]:
            out.append(("exit", k))
        for o in an.succ[k]:
            todo.append((o, None))
    return out


def check_regs(ctx, func, A, B, ev_b, ev_m, bonly, monly):
    """differing effects are only cpu stores (argument registers, CR bits) or stores into unread frame objects"""
    lines = []
    diff = [("base", ev_b[i], A) for i in bonly] + [("mutant", ev_m[j], B) for j in monly]
    if not diff:
        return None
    crs, regs, objw, other = [], [], [], []
    for side, e, an in diff:
        if e.kind == "hr":
            continue  # a host static read (glue table / data pointer): no effect
        if e.kind == "cw" and CPU_CR <= e.addr[1] < CPU_CR + 32:
            crs.append((side, e, an))
        elif e.kind == "cw" and (e.addr[1] < 0x80 and 3 <= e.addr[1] // 4 <= 10 or
                                 CPU_F + 16 <= e.addr[1] < CPU_F + 9 * 16):
            regs.append((side, e, an))
        elif e.kind == "gw" and obj_base(e.addr):
            objw.append((side, e, an))
        elif e.kind == "gcall":
            other.append((side, e, an))
        else:
            other.append((side, e, an))
    # a guest call whose arguments differ only in unread registers shows up as a gcall difference: pair them
    gb = [e for s, e, an in other if s == "base" and e.kind == "gcall"]
    gm = [e for s, e, an in other if s == "mutant" and e.kind == "gcall"]
    if gb or gm:
        if len(gb) != len(gm):
            return None
        for x, y in zip(sorted(gb, key=lambda e: e.pos), sorted(gm, key=lambda e: e.pos)):
            if cdesc(x.target, A) != cdesc(y.target, B):
                return None
            ks = set(x.args) | set(y.args)
            for r in ks:
                if cdesc(x.args.get(r), A) != cdesc(y.args.get(r), B):
                    if r.startswith("cr"):
                        continue
                    rd = reads_reg(ctx, x.target, r)
                    if rd is not False:
                        return None
        other = [t for t in other if t[1].kind != "gcall"]
    if other:
        return None
    if not (crs or regs or objw):
        return None
    reasons = []
    family = "cr6" if crs and not regs and not objw else "unreadarg"
    for side, e, an in regs:
        off = e.addr[1]
        rn = cpu_name(off).split(".")[0]
        cons = next_consumers(ctx, an, an.events, e, off)
        for c in cons:
            if c[0] == "call":
                rd = reads_reg(ctx, c[1].target, rn)
                lines.append("  %s %s: next consumer %s -> %s" % (side, fmt_event(e), fmt_event(c[1]),
                                                                  "reads %s" % rn if rd else
                                                                  ("does not read %s (live-in mask 0x%X)" % (
                                                                      rn, ctx.livein.get(c[1].target[1] & 0xFFFFFFFF, (0, 0))[0 if rn[0] == "r" else 1])
                                                                   if rd is False else "unknown callee")))
                if rd is not False:
                    return "open", "the changed %s reaches guest call %s, which reads it (or has no live-in row)" % (
                        rn, fmt_val(c[1].target)), lines
            elif c[0] == "read":
                lines.append("  %s %s: read back by %s" % (side, fmt_event(e), fmt_event(c[1])))
                return "open", "the changed %s is read back by the candidate (%s)" % (rn, c[1].where()), lines
            elif c[0] == "hcall":
                return "open", "the changed %s reaches host call %s" % (rn, c[1].sym), lines
            elif c[0] == "exit" and rn in ("r3", "f1"):
                return "open", "the changed %s may reach the function's return (return register)" % rn, lines
        tg = sorted({fmt_val(c[1].target) for c in cons if c[0] == "call"})
        reasons.append("%s %s changes only %s, which %s never read%s" % (
            side, "store" if side else "", rn, "/".join(tg) or "no callee",
            " (live-in masks " + ", ".join("%s 0x%X" % (t, ctx.livein.get(int(t, 16) & 0xFFFFFFFF, (0, 0))[0])
                                         for t in tg if re.match(r"0x[0-9A-F]+$", t)) + ")" if tg else ""))
    for side, e, an in objw:
        ob = obj_base(e.addr)
        esc, unread = escapes(ctx, an)
        reads = [x for x in an.events if x.kind == "gr" and obj_base(x.addr) == ob]
        if esc.get(ob):
            return "open", "the changed store goes to frame object %s, which escapes (%s)" % (_cid(ob[1]), esc[ob][0][0]), lines
        if reads:
            return "open", "the changed store goes to frame object %s, which the candidate reads back (%s)" % (
                _cid(ob[1]), reads[0].where()), lines
        o = an.objs.get(ob)
        # bytes of this object read by another object later (stale reuse)
        if o and o.off is not None and o.size:
            ev = frame_events(ctx, an, an.objs)
            for b in range(o.off, o.off + o.size):
                pr = [p for p in byte_flow(an, ev, b, ob) if p[0] == "foreign-reads-S"]
                if pr:
                    return "open", "byte sp+0x%X of frame object %s is read by %s afterwards" % (
                        b, _cid(ob[1]), fmt_event(pr[0][1], layout=an.objs)), lines
        via = unread.get(ob, [])
        lines.append("  %s %s: object %s %s reaches only %s" % (
            side, fmt_event(e, layout=an.objs), o.type if o else "?", _cid(ob[1]),
            ", ".join("%s of %s (live-in 0x%X)" % (r, fmt_val(c.target), ctx.livein.get(c.target[1] & 0xFFFFFFFF,
                                                                                            (0, 0))[0])
                      for c, r in via) or "nothing"))
        reasons.append("the %s store into %s %s is unobservable: the object is never read by the candidate, never "
                       "escapes, and reaches only %s" % (
                           side, o.type if o else "frame object", _cid(ob[1]),
                           ", ".join(sorted({"%s of %s" % (r, fmt_val(c.target)) for c, r in via})) or "no call"))
    for side, e, an in crs:
        off = e.addr[1]
        bit = off - CPU_CR
        cons = next_consumers(ctx, an, an.events, e, off)
        for c in cons:
            if c[0] == "read":
                return "open", "cr[%d] is read back by the candidate (%s)" % (bit, c[1].where()), lines
            if c[0] == "hcall":
                return "open", "cr[%d] reaches host call %s" % (bit, c[1].sym), lines
            if c[0] != "call":
                continue
            t = c[1].target
            if not is_const(t):
                return "open", "cr[%d] reaches an indirect guest call" % bit, lines
            rd, saves = callee_cr_reads(ctx.wdis_root, t[1] & 0xFFFFFFFF)
            touched = {bit, 6} if bit != 6 else {6}
            readbits = {b: w for b, w in rd.items() if b in touched}
            lines.append("  %s %s: next guest call %s; callee reads CR bits %s at entry; %d f1-f8 saves" % (
                side, fmt_event(e), fmt_val(t), ",".join("%d (%s)" % kv for kv in sorted(rd.items())) or "none", saves))
            if not readbits:
                reasons.append("cr[%d] change before guest call %s, which does not read CR bit%s %s at entry "
                               "(non-variadic)" % (bit, fmt_val(t), "s" if len(touched) > 1 else "",
                                                   "/".join(map(str, sorted(touched)))))
                continue
            if 6 in readbits and saves:
                fmts = []
                for r, v in sorted(c[1].args.items(), key=lambda kv: _argkey(kv[0])):
                    if r.startswith("r") and is_const(v):
                        s = ctx.image.cstr(v[1] & 0xFFFFFFFF)
                        if s and "%" in s:
                            fmts.append((r, v[1] & 0xFFFFFFFF, s))
                lines.append("  format strings among the arguments: %s" % (
                    "; ".join("%s=0x%08X %r" % f for f in fmts) or "none constant"))
                if not fmts:
                    return "open", "cr[%d] change before variadic %s; no constant format string among its argument " \
                                   "registers (format passed through a helper or a variable)" % (bit, fmt_val(t)), lines
                fp = [f for f in fmts if FPCONV.search(f[2])]
                if fp:
                    return "open", "cr[%d] change before variadic %s whose format %r has a floating-point conversion" % (
                        bit, fmt_val(t), fp[0][2]), lines
                reasons.append("cr[%d] change before the variadic call %s (%s: CR6 only selects whether f1-f8 are saved); "
                               "its format %s has no floating-point conversion, so no saved FPR is read" % (
                                   bit, fmt_val(t), readbits.get(6, ""), ", ".join(repr(f[2]) for f in fmts)))
                continue
            return "open", "cr[%d] change before %s, which reads CR bit(s) %s at entry" % (
                bit, fmt_val(t), ",".join(map(str, readbits))), lines
    return family, "; ".join(reasons), lines


# ------------------------------------------------------------------------------------------------ block mapping
def orig_blocks(unitc_text, func):
    """[(block id, first ppc addr, [callee addrs])] of orig_FUNC in the generated unit.c"""
    m = re.search(r"^void orig_%s\(Cpu\* __restrict c\) \{\n(.*?)^\}" % func, unitc_text, re.S | re.M)
    if not m:
        return []
    out = []
    cur = None
    for line in m.group(1).split("\n"):
        v = re.match(r"\s*VM_COV\(0, (\d+)\);", line)
        if v:
            cur = [int(v.group(1)), None, []]
            out.append(cur)
            continue
        if cur is None:
            continue
        a = re.search(r"/\* ([0-9A-F]{8}): ", line)
        if a and cur[1] is None:
            cur[1] = a.group(1)
        c = re.search(r"\b(?:f|real|imp)_([0-9A-Fa-f]{8}|\w+)\(c\)", line)
        if c and "MUSTTAIL return f_" not in line:
            cur[2].append(c.group(1).upper() if re.match(r"[0-9A-Fa-f]{8}$", c.group(1)) else c.group(1))
    return out


def map_block(blocks, an, events, first_pos):
    """the original block of the mutation: the next guest call at or after the first differing effect in the
    candidate (k-th call to that target) -> the block holding the k-th call to it in the original"""
    calls = [e for e in sorted(events, key=lambda e: e.pos) if e.kind == "gcall" and is_const(e.target)]
    if not blocks:
        return "?"
    nxt = [e for e in calls if first_pos is None or e.pos >= first_pos]
    prv = [e for e in calls if first_pos is not None and e.pos < first_pos]
    for pick, rel in ((nxt[:1], ""), (prv[-1:], ">")):
        if not pick:
            continue
        e = pick[0]
        t = "%08X" % (e.target[1] & 0xFFFFFFFF)
        k = sum(1 for x in calls if x.pos < e.pos and is_const(x.target) and (x.target[1] & 0xFFFFFFFF) == int(t, 16))
        seen = 0
        for bid, a, cs in blocks:
            for c in cs:
                if c == t:
                    if seen == k:
                        return "%sB%d" % (rel, bid)
                    seen += 1
    return "B?" if len(blocks) > 1 else "B0"


# ------------------------------------------------------------------------------------------------ one survivor
def triage_one(ctx, builder, item):
    """item: dict id, func, src, mutation, cpp (mutant source path), json (receipt) -> row dict"""
    i, func, src, mut = item["id"], item["func"], item["src"], item["mutation"]
    kind = mut.split(" ")[0]
    row = dict(id=i, func=func, src=src, kind=kind, mutation=mut, check="-", candidate="MANUAL", reason="",
               block="?", reach=item.get("reach", "unknown"), evidence="")
    ev_lines = ["survivor #%s %s %s" % (i, func, mut)]
    try:
        base_text = open(os.path.join(builder.tree, src)).read()
        mt = open(item["cpp"]).read()
    except OSError as e:
        row["reason"] = "no source: %s" % e
        return row
    # the receipt must come from this source: the mutant differs from it only in the mutation's hunk
    hunks = [op for op in difflib.SequenceMatcher(None, base_text.split("\n"), mt.split("\n"), autojunk=False)
             .get_opcodes() if op[0] != "equal"]
    span = sum(max(op[2] - op[1], op[4] - op[3]) for op in hunks)
    if len(hunks) > 2 or span > 6:
        row.update(check="stale", reason="the mutant differs from --tree's %s in %d hunk(s) / %d line(s): the receipt "
                                        "was made from another version of the source (pass --tree at the campaign's "
                                        "commit)" % (src, len(hunks), span))
        return finish(row, ev_lines, ctx)
    db, err = builder.build(src, base_text, "base/" + src.replace("/", "_"))
    fm, err2 = builder.build_funcs(src, mt, "m%s" % i, ctx.keep_objects)
    if not db or not fm:
        row["reason"] = "compile failed: %s" % (err or err2 or "")[:200]
        return row
    dm = os.path.join(ctx.a.scratch, "obj", "m%s" % i, os.path.basename(src) + ".dis") if ctx.keep_objects else \
        "(not kept: rerun with --keep-objects --ids %s)" % i
    fb = load_dis(db)
    sym = func_of_addr(fb, func)
    # every function whose code differs
    changed = []
    for name in fb:
        if name in fm and [x.norm() for x in fb[name]] != [x.norm() for x in fm[name]]:
            changed.append(name)
    ev_lines.append("objects: %s / %s; marked function symbol %s; functions whose code differs: %s" % (
        db[:-4] + ".o", dm, sym, ", ".join(changed) or "none"))
    if not changed:
        row.update(check="identical", candidate="EQ", reason="the function bodies compile identically (only "
                   "symbol/data differences, or none)")
        return finish(row, ev_lines, ctx)
    target = sym if sym in changed else None
    if target is None:
        # a helper changed: analyse the marked function (helpers are inlined into its effect list) when it calls
        # the helper, else the helper itself
        target = sym if sym and any(x.reloc in changed for x in fb[sym]) else changed[0]
    A = Analysis(fb, target, ctx.livein, glue=builder.glue(src, base_text)).run()
    B = Analysis(fm, target, ctx.livein, glue=builder.glue(src, mt)).run()
    ev_b = [e for e in A.events if relevant(e)]
    ev_m = [e for e in B.events if relevant(e)]
    ev_b.sort(key=lambda e: e.pos)
    ev_m.sort(key=lambda e: e.pos)
    amap = align(fb[target], fm[target])
    pairs, bonly, monly = match_events(ev_b, ev_m, amap, A, B)
    first = None
    if bonly or monly:
        first = min([ev_b[i2].pos for i2 in bonly] + [ev_m[j].pos for j in monly])
    else:
        om = {a: b for a, b in pairs}
        inv = [a for a in sorted(om) if any(om[a] > om[b2] for b2 in om if b2 > a)]
        if inv:
            first = ev_b[inv[0]].pos
    if ctx.unitc:
        row["block"] = map_block(orig_blocks(ctx.unitc, func), A, ev_b, first)
    # dispatch
    res = None
    fr = check_frame(ctx, func, A, B)
    if fr:
        row["check"] = "frame"
        res = fr
    elif kind == "swap" and not (bonly or monly):
        row["check"] = "swap"
        res = check_swap(ctx, A, B, ev_b, ev_m, amap)
    elif bonly or monly:
        r = check_regs(ctx, func, A, B, ev_b, ev_m, bonly, monly)
        if r:
            fam, why, lines = r
            if fam in ("unreadarg", "cr6"):
                row["check"] = fam
                res = ("EQ", why, lines)
            else:
                row["check"] = "cr6" if "cr[" in why else "unreadarg"
                res = (fam, why, lines)
        elif kind == "swap":
            row["check"] = "swap"
            res = check_swap(ctx, A, B, ev_b, ev_m, amap)
    else:
        # same effects in the same order but different code (register allocation, scheduling)
        row["check"] = "swap" if kind == "swap" else "same-effects"
        res = ("EQ", "base and mutant make the same effects in the same order (code differs only in registers, "
                     "scheduling or pure computation)", [])
    if res:
        verdict, why, lines = res
        row["candidate"] = {"EQ": "EQ?", "RI": "RI?", "dependent": "DEP", "open": "MANUAL"}.get(verdict, verdict)
        row["reason"] = why
        ev_lines += lines
    else:
        diff = [fmt_event(ev_b[x], layout=A.objs) for x in bonly[:6]] + [fmt_event(ev_m[x], layout=B.objs) for x in monly[:6]]
        row["reason"] = "no family: differing effects " + "; ".join(diff[:3])
        ev_lines.append("differing effects:")
        ev_lines += ["  base:   " + fmt_event(ev_b[x], layout=A.objs) for x in bonly[:12]]
        ev_lines += ["  mutant: " + fmt_event(ev_m[x], layout=B.objs) for x in monly[:12]]
    # the source-level diff and the instruction diff as context
    ev_lines.append("source diff (context only):")
    ev_lines += ["  " + l for l in list(difflib.unified_diff(base_text.split("\n"), mt.split("\n"), lineterm="",
                                                             n=1))[2:14]]
    ev_lines.append("instruction diff of %s:" % target)
    nb = [x.norm() for x in fb[target]]
    nm = [x.norm() for x in fm[target]]
    ev_lines += ["  " + l for l in list(difflib.unified_diff(nb, nm, lineterm="", n=2))[2:60]]
    return finish(row, ev_lines, ctx)


def finish(row, ev_lines, ctx):
    d = os.path.join(ctx.a.scratch, "evidence")
    os.makedirs(d, exist_ok=True)
    p = os.path.join(d, "m%s.txt" % row["id"])
    with open(p, "w") as fh:
        fh.write("\n".join(ev_lines) + "\n")
    row["evidence"] = p
    return row


# --------------------------------------------------------------------------------------------------- inputs
def receipts_items(dirs, classes, ids):
    items = {}
    for d in dirs:
        for f in os.listdir(d):
            m = re.match(r"mutant(\d+)\.json$", f)
            if not m:
                continue
            i = int(m.group(1))
            if ids and i not in ids:
                continue
            j = json.load(open(os.path.join(d, f)))
            cls = j.get("class", "")
            if classes and cls not in classes and not (ids and i in ids):
                continue
            cpp = os.path.join(d, "mutant%d.cpp" % i)
            if not os.path.exists(cpp):
                continue
            reach = "unknown"
            if "not reached" in str(j.get("reach", "")):
                reach = "not-reached"
            elif any("-runlist" == c for c in j.get("cmd", []) or []) or j.get("reach") == "reached":
                reach = "reached"
            if i not in items:
                items[i] = dict(id=i, func=j["func"], src=j["src"], mutation=j["mutation"], cpp=cpp, json=j,
                                reach=reach, cls=cls)
    return items


def ledger_rows(path, classes):
    rows = {}
    head = None
    for line in open(path):
        p = line.rstrip("\n").split("\t")
        if head is None:
            head = p
            continue
        r = dict(zip(head, p))
        if classes and r.get("class") not in classes:
            continue
        rows[int(r["id"])] = r
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("unit")
    ap.add_argument("--receipts", action="append", default=[])
    ap.add_argument("--class", dest="classes", default="survived")
    ap.add_argument("--ids", default="")
    ap.add_argument("--ledger")
    ap.add_argument("--ledger-class", default="EQ,RI")
    ap.add_argument("--skip-identical", action="store_true", help="ledger rows closed as identical code are left out")
    ap.add_argument("--root", default=ROOT)
    ap.add_argument("--tree")
    ap.add_argument("--frames")
    ap.add_argument("--livein")
    ap.add_argument("--unitc")
    ap.add_argument("--image")
    ap.add_argument("--wdis-root", help="checkout whose tools/verify/wdis.py and build/gen disassemble originals "
                                        "(default --root)")
    ap.add_argument("--out")
    ap.add_argument("--scratch")
    ap.add_argument("-j", type=int, default=4)
    ap.add_argument("--keep-objects", action="store_true",
                    help="keep every mutant's object and disassembly under SCRATCH/obj/mN (default: one reused slot "
                         "per worker; the evidence file keeps the instruction diff)")
    a = ap.parse_args()
    a.root = os.path.abspath(a.root)
    a.tree = os.path.abspath(a.tree or a.root)
    a.wdis_root = os.path.abspath(a.wdis_root or a.root)
    bv = os.path.join(a.root, "build", "verify")
    a.frames = a.frames or os.path.join(bv, "frames.tsv")
    a.livein = a.livein or os.path.join(bv, "livein.tsv")
    a.image = a.image or os.path.join(bv, "image.bin")
    if not a.unitc:
        c = os.path.join(bv, a.unit, "unit.c")
        a.unitc = c if os.path.exists(c) else None
    a.scratch = os.path.abspath(a.scratch or os.path.join(bv, "_triage", a.unit))
    a.out = a.out or os.path.join(a.scratch, "triage.tsv")
    os.makedirs(a.scratch, exist_ok=True)
    ids = {int(x) for x in a.ids.split(",") if x.strip()} if a.ids else set()
    items = receipts_items(a.receipts, set(a.classes.split(",")) if a.classes else set(), ids)
    led = None
    if a.ledger:
        led = ledger_rows(a.ledger, set(a.ledger_class.split(",")) if a.ledger_class else set())
        if a.skip_identical:
            led = {k: v for k, v in led.items() if "identical" not in v.get("reason", "") and
                   "identical" not in v.get("main_s1", "")}
        if ids:
            led = {k: v for k, v in led.items() if k in ids}
        allitems = receipts_items(a.receipts, set(), set(led))
        items = {i: allitems[i] for i in led if i in allitems}
        missing = [i for i in led if i not in allitems]
        if missing:
            print("triage: %d ledger row(s) without a receipt: %s" % (len(missing), missing[:20]), file=sys.stderr)
    ctx = Ctx(a)
    ctx.unitc = open(a.unitc).read() if a.unitc and os.path.exists(a.unitc) else None
    builder = Builder(a.root, a.tree, a.scratch)
    rows = []
    work = sorted(items.values(), key=lambda it: it["id"])
    # build the base objects first (once per source), then the mutants in parallel
    for src in sorted({it["src"] for it in work}):
        builder.build(src, open(os.path.join(a.tree, src)).read(), "base/" + src.replace("/", "_"))

    def one(it):
        try:
            return triage_one(ctx, builder, it)
        except Exception as ex:  # a tool failure on one survivor is reported, never fatal
            import traceback
            return dict(id=it["id"], func=it["func"], src=it["src"], kind=it["mutation"].split(" ")[0],
                        mutation=it["mutation"], check="error", candidate="MANUAL", block="?",
                        reach=it.get("reach", "unknown"),
                        reason="triage error: %s" % traceback.format_exc().strip().split("\n")[-1], evidence="")

    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, a.j)) as ex:
        for r in ex.map(one, work):
            rows.append(r)
            print("triage: #%s %s %s -> %s %s" % (r["id"], r["func"], r["check"], r["candidate"], r["reason"][:120]),
                  file=sys.stderr)
    write_out(a, rows, led)
    return 0


FAMILY_OF_CHECK = {"swap": "swap", "frame": "frame", "unreadarg": "unreadarg", "cr6": "cr6"}


def write_out(a, rows, led):
    cols = ["id", "func", "src", "kind", "block", "reach", "group", "check", "candidate", "reason", "evidence",
            "mutation"]
    if led is not None:
        cols += ["ledger_class", "agree"]
    with open(a.out, "w") as fh:
        fh.write("\t".join(cols) + "\n")
        for r in sorted(rows, key=lambda r: int(r["id"])):
            r["group"] = "%s|%s|%s|%s" % (r["func"], r["block"], r["kind"], r["reach"])
            if led is not None:
                lc = led.get(int(r["id"]), {}).get("class", "")
                r["ledger_class"] = lc
                c = r["candidate"].rstrip("?")
                r["agree"] = "yes" if c == lc else ("abstain" if c in ("MANUAL", "DEP") else "NO")
            fh.write("\t".join(str(r.get(c, "")).replace("\t", " ").replace("\n", " ") for c in cols) + "\n")
    summ = []
    fam = collections.defaultdict(collections.Counter)
    for r in rows:
        fam[r["check"]][r["candidate"]] += 1
    summ.append("survivors: %d" % len(rows))
    summ.append("per check (candidate counts):")
    for k in sorted(fam):
        summ.append("  %-12s %4d  %s" % (k, sum(fam[k].values()), ", ".join("%s %d" % kv for kv in fam[k].most_common())))
    groups = collections.Counter(r["group"] for r in rows)
    summ.append("groups (function|block|kind|reach): %d" % len(groups))
    for g, n in groups.most_common(40):
        cand = collections.Counter(r["candidate"] for r in rows if r["group"] == g)
        summ.append("  %-48s %4d  %s" % (g, n, ", ".join("%s %d" % kv for kv in cand.most_common())))
    if led is not None:
        ag = collections.Counter(r["agree"] for r in rows)
        summ.append("ledger comparison: %s" % ", ".join("%s %d" % kv for kv in ag.most_common()))
    text = "\n".join(summ) + "\n"
    with open(a.out[:-4] + ".summary.txt" if a.out.endswith(".tsv") else a.out + ".summary.txt", "w") as fh:
        fh.write(text)
    print(text)


if __name__ == "__main__":
    sys.exit(main())

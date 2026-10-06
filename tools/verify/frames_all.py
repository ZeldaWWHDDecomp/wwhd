#!/usr/bin/env python3
"""Native stack frames of every function in the image (build/verify/frames.tsv).

The original's prologue, up to its first branch: the frame size (stwu r1,-N(r1)), the back chain,
where it saves LR and the callee-saved registers it saves on entry (stmw / stw rN / stfd fN /
stfs or psq_st of FPR halves) with the values they hold on entry. gabi's automatic native frame
(gabi.h, WWHD_FUNC) replays exactly these stores and runs the candidate's callees at the original's
sp, so a decompiled function leaves the guest stack as the original does (game test 2026-10-05:
stale stack bytes reach the empty slots of cking.sav through initdata_to_card).

usage: frames_all.py [OUT]     (verify.py regenerates it when build/gen is newer)
Output, one line per function with a frame:
  ADDR<TAB>SIZE<TAB>items<TAB>flags
items (space separated, offsets hex, relative to the original's sp after stwu):
  l:OFF  LR saved at sp+OFF           g:R:OFF  GPR rR (entry value) at sp+OFF
  d:F:OFF  stfd fF (entry ps0, double)  s0:F:OFF / s1:F:OFF  stfs of fF's ps0 / ps1 (single)
  p:F:OFF  psq_st fF (ps0, ps1 as singles)
  o:OFF  the frame offset of the first stack object whose address the function takes (code order)
flags: empty when the prologue was parsed completely. A function whose frame cannot be described
(stwux / r12 frames, a store of a register modified before it is saved, ...) gets SIZE 0 and a
flag: the candidate keeps today's behaviour (no automatic frame); frames_all.py --list prints them.
Functions without stwu (leaves, tail branches) are omitted (nothing to replay).
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools"))
sys.path.insert(0, HERE)
from funcdb import GenIndex  # noqa: E402
from ppcdis import dis  # noqa: E402

NOWRITE = ("st", "cmp", "b", "mt", "dcb", "icb", "sync", "isync", "tw", "eieio", "psq_st", "crxor", "crclr",
           "crset", "cror", "creqv", "crnot", "mcrf", "nop")
BRANCH = re.compile(r"^b")


def num(s):
    return int(s, 0)


def analyse(addr, size):
    n = min(size // 4, 64)
    lines = dis(addr, n).split("\n")
    sp_delta = None          # frame size once stwu ran
    written = set()          # registers changed since entry ("r5", "f31", ...)
    swapped = set()          # FPRs whose halves were exchanged by ps_merge10 fN,fN,fN
    lr_reg = None            # register holding LR (mflr)
    items = []
    flags = []
    pending = []             # stores before stwu: (kind, reg, off_from_entry)
    for line in lines:
        m = re.match(r"([0-9a-f]{8}): [0-9a-f]{8}  (\S+)\s*(.*)", line)
        if not m:
            break
        mn, ops = m.group(2), m.group(3)
        if BRANCH.match(mn):
            break
        o = [x.strip() for x in ops.split(",")] if ops else []

        def mem(x):
            mm = re.match(r"(-?(?:0x)?[0-9a-f]+)\((r\d+)\)$", x)
            return (num(mm.group(1)), mm.group(2)) if mm else (None, None)

        if mn == "stwu" and o[0] == "r1":
            d, base = mem(o[1])
            if base != "r1" or sp_delta is not None:
                flags.append("stwu")
                return 0, [], flags
            sp_delta = -d
            items.insert(0, "b")
            for kind, reg, off in pending:
                items.append(fmt(kind, reg, off + sp_delta))
            continue
        if mn == "stwux":
            flags.append("stwux")
            return 0, [], flags
        if mn == "mflr":
            lr_reg = o[0]
            written.add(o[0])
            continue
        if mn in ("stw", "stmw", "stfd", "stfs", "psq_st"):
            d, base = mem(o[1])
            if base != "r1":
                continue
            reg = o[0]
            if mn == "stw" and reg == lr_reg:
                rec = ("l", None, d)
            elif mn == "stmw":
                r0 = int(reg[1:])
                if any(("r%d" % r) in written for r in range(r0, 32)):
                    continue          # not a save of entry values: body content
                for r in range(r0, 32):
                    add(items, pending, sp_delta, ("g", r, d + 4 * (r - r0)))
                continue
            elif mn == "stw":
                r = int(reg[1:])
                if r < 14:
                    continue          # an argument spill: body content, not part of the frame replay
                if reg in written:
                    continue          # a modified register: body content, not a save
                rec = ("g", r, d)
            else:
                f = int(reg[1:])
                if f < 14:
                    continue
                if reg in written:
                    continue
                if mn == "stfd":
                    rec = ("d", f, d)
                elif mn == "stfs":
                    rec = ("s1" if reg in swapped else "s0", f, d)
                else:
                    rec = ("p", f, d)
            add(items, pending, sp_delta, rec)
            continue
        if mn == "ps_merge10" and len(o) == 3 and o[0] == o[1] == o[2]:
            swapped.symmetric_difference_update({o[0]})
            continue
        if any(mn.startswith(p) for p in NOWRITE):
            continue
        if o:
            written.add(o[0])
            if o[0] == lr_reg:
                lr_reg = None         # the register no longer holds LR (e.g. lis r0 after mflr r0)
            if o[0] == "r1":
                if sp_delta is not None:
                    break             # the epilogue (addi r1,r1,N) of a function without a call
                flags.append("r1")
                return 0, [], flags
    if sp_delta is None:
        return frameless(addr, size)
    lo = objects_lo(addr, size, sp_delta)
    if lo is not None:
        items.append("o:%x" % lo)
    return sp_delta, items, flags


def objects_lo(addr, size, frame):
    """the frame offset of the first stack object whose address the function takes, in code order
    (the first addi rD, r1, OFF with 8 <= OFF < frame): GHS's objects follow it upward in the order
    they are used (d_a_st anm_init: 0x10, 0x18, and 0x8 only in a later branch). gabi places Locals
    from there upward."""
    for line in dis(addr, size // 4).split("\n"):
        m = re.match(r"[0-9a-f]{8}: [0-9a-f]{8}  addi r\d+, r1, (-?(?:0x)?[0-9a-f]+)$", line)
        if m:
            off = int(m.group(1), 0)
            if 8 <= off < frame:
                return off
    return None


def frameless(addr, size):
    """no stwu: a leaf, or a function that only tail-branches (b / bctr to another function, LR
    untouched). The latter gets SIZE 0 with flag `tail`: its candidate's calls are tail calls and
    must run at the caller's sp. A frameless function with a linking branch is left out."""
    lines = dis(addr, size // 4).split("\n")
    tail = False
    for line in lines:
        m = re.match(r"([0-9a-f]{8}): [0-9a-f]{8}  (\S+)\s*(.*)", line)
        if not m:
            continue
        mn, ops = m.group(2), m.group(3)
        if mn.startswith("b") and (mn.endswith("l") or mn.endswith("la") or mn == "bctrl" or mn == "blrl"):
            if mn not in ("bl", "bctrl", "blrl", "bla") and not re.match(r"b\w*l$", mn):
                continue
            return None, [], []
        if mn in ("bctr", "bgectr", "bltctr", "bnectr", "beqctr"):
            tail = True
        elif mn == "b" or (mn.startswith("b") and not mn.startswith("blr") and re.search(r"0x([0-9a-f]+)$", ops)):
            t = re.search(r"0x([0-9a-f]+)$", ops)
            if t and not (addr <= int(t.group(1), 16) < addr + size):
                tail = True
    return (0, [], ["tail"]) if tail else (None, [], [])


def fmt(kind, reg, off):
    if kind == "l":
        return "l:%x" % off
    return "%s:%d:%x" % (kind, reg, off)


def add(items, pending, sp_delta, rec):
    kind, reg, d = rec
    if sp_delta is None:
        pending.append((kind, reg, d))
    else:
        items.append(fmt(kind, reg, d))


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    out = args[0] if args else os.path.join(ROOT, "build", "verify", "frames.tsv")
    gen = GenIndex(os.path.join(ROOT, "build", "gen"))
    rows, odd = [], []
    for a in sorted(gen.addrs):
        size, items, flags = analyse(a, gen.size(a))
        if size is None:
            continue
        rows.append("%08X\t%d\t%s\t%s\n" % (a, size, " ".join(items), ",".join(flags)))
        if flags and flags != ["tail"]:
            odd.append((a, flags))
    tmp = out + ".tmp%d" % os.getpid()  # per-process: concurrent writers never share a temp file
    with open(tmp, "w") as f:
        f.writelines(rows)
    os.replace(tmp, out)
    print("%s: %d functions with a frame, %d not described (fallback)" % (out, len(rows), len(odd)), file=sys.stderr)
    if "--list" in sys.argv:
        for a, fl in odd:
            print("%08X %s" % (a, ",".join(fl)))


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Self-test of the unit-file `osmem` directive (harness.cpp namespace osm; 2026-11-01, 
-11-01 09:40): the shipping port's HLE OS/MEM backend.

Carrier: sead::ResourceState::allocate 02756D18 (PoolHeap.cpp: MEMGetBaseHeapHandle(1), MEMGetAllocatableSizeForExpHeapEx,
then a call through the loader data-import word MEMAllocFromDefaultHeapEx C1001000) tested with r4 = 0x3B7780C4 (the
sead arena request of the startup), after original setups: libc 028F588C (the 0x210-byte FILE buffer, default heap,
align 0x40), 0274B3C4 twice (MEMGetBaseHeapHandle(0) + MEMAllocFromFrmHeapEx, head align 0x100 and tail align -0x40),
027B6E2C (GX2CalcDepthBufferHiZInfo through r4/r5), 0258839C (OSGetSystemInfo), 028EF7DC (__ghs_mtx_init) and 028F26F4
(__ghs_flock_ptr; the zeroed block's lock word reads 0).
 1. HLE numbers (trace): FILE buffer 104DB040, arena 104DB250 (first fit, align 4, > 16 MB), base handles 1 -> 104DB000
    and 0 -> F4000000, allocatable 3FB24DB0 (port largest(4|4) after the FILE buffer), MEM1 head F4000100 and tail
    F5FFFF00, sysinfo 6FFE0000, mtx_init block 6FF00000 stored, flock_ptr 6FF00010; nested (setups) and top level (the
    tested function's indirect call through C1001000 reaches the host function C2000008).
 (plus 02034820: OSSetExceptionCallback x3 -> 0; 0274FEB0: the GX2 state setters keep r3 as the HLE does.)
 2. HiZ outputs: -watch shows the big-endian 0x1000 / 0x100 stores at r4 / r5.
 3. Rows ok at n 300 seeds 1 and 7, with -nulltrap too (C2000008 is a code target).
 4. setupsnapshot (the setups that read no generated memory) captured, rows equal; -nofastreset equal; -snapcache-write / read / check rc 0 and equal rows;
    -refcache-write / read / check rc 0 and equal rows.
 5. Off by default: without the directive no osmem line is printed and MEMGetBaseHeapHandle gets the generic answer.
 6. Parts: `osmem heaps` alone leaves GX2/OSGetSystemInfo/ghs imports to the generic mock.
 7. A spec naming a modelled target in another mock directive exits 2; malformed lines exit 2.
 8. The heap model equals the port's code (osmem_equiv.py: random operation sequences against coreinit_mem.cpp).
 9. The default data_end and the .bss start equal the RPX's under the port's load_rpx rule.
Unit and spec files stay in tools/verify/units/_osmem_test/ and a new directory under the verify scratch (paths printed;
never removed). Exit 0 when all pass."""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402

UNIT = "_osmem_test/osmem"
F = "02756D18"
SRC = ["src wwhd_src/sead/PoolHeap.cpp", "src wwhd_src/sead/PoolHeapStatics.cpp", "src wwhd_src/sead/HeapRegistryStatics.cpp"]
REAL = ["real 028F588C 0274B3C4 027B6E2C 0258839C 028EF7DC 028F26F4 02034820 0274FEB0"]
SETUPS = ["arg! 02756D18 r3 0x5A000000",
          "arg! 02756D18 r4 0x3B7780C4",
          "setup 02756D18 028F588C r3=0x210",
          "setup 02756D18 0274B3C4 r3=0x100 r4=0x100",
          "setup 02756D18 0274B3C4 r3=0x100 r4=-0x40",
          "setup 02756D18 027B6E2C r3=0x5C000000",
          "field * 101F84DC u32 0x5C002000",
          "field * 5C002038 u32 1",
          "setup 02756D18 0258839C r3=0 r4=0",
          "setup 02756D18 028EF7DC r3=0x5C001000",
          "setup 02756D18 028F26F4",
          "setup 02756D18 02034820 r3=0x5C003000",
          "setup 02756D18 0274FEB0 r3=0x5C004000"]
TRACE = "1000003"


def rows(out):
    return [l for l in out.splitlines() if l.startswith(F)]


def main():
    fails = []
    udir = os.path.join(HERE, "units", "_osmem_test")
    os.makedirs(udir, exist_ok=True)
    with open(os.path.join(udir, "osmem.txt"), "w") as f:
        f.write("\n".join(["# verify: -strictnan"] + SRC + ["osmem hle"] + REAL + SETUPS) + "\n")
    exe = verify.build(UNIT, jobs=8)
    base = verify.verify_scratch_base() or os.path.join(ROOT, "build", "verify")
    W = tempfile.mkdtemp(prefix="osmem_test-", dir=base)
    print("osmem_test: files in %s and %s (not removed)" % (W, udir))

    def spec(name, lines):
        p = os.path.join(W, name + ".txt")
        with open(p, "w") as f:
            f.write("\n".join(lines) + "\n")
        return p

    def run(sp, args, n="300", seed="1"):
        r = subprocess.run([exe, "-spec", sp, "-noprogress", "-strictnan", "-n", n, "-seed", seed, "-only", F] + args, cwd=ROOT,
                           capture_output=True, text=True, errors="replace")
        return r.returncode, r.stdout, r.stderr

    def ok(rs):
        return len(rs) == 1 and rs[0].rstrip().endswith(" ok")

    full = spec("full", SRC + ["osmem hle"] + REAL + SETUPS)
    # 1: the HLE numbers
    rc, out, err = run(full, ["-trace", TRACE], n="1")
    want = ["nested osmem C2000008 r3=00000210 r4=00000040",  # FILE buffer through the data-import word (nested)
            "-> 104DB040",
            "nested osmem C0009790 r3=00000000", "-> F4000000",
            "nested osmem C00096A0 r3=F4000000 r4=00000100 r5=00000100 -> F4000100",
            "nested osmem C00096A0 r3=F4000000 r4=00000100 r5=FFFFFFC0 -> F5FFFF00",
            "nested osmem C00060B0 r3=5C0000A8 r4=5C00009C r5=5C0000A0 (void)",
            "nested osmem C0009C80", "-> 6FFE0000",
            "nested osmem C000A7D0 r3=5C001000",
            "nested osmem C000A7B0", "-> 6FF00010",
            "\n        osmem C0009790 r3=00000001", "-> 104DB000",
            "\n        osmem C0009778 r3=104DB000 r4=00000004", "-> 3FB24DB0",
            "\n        osmem C2000008 r3=3B7780C4 r4=00000004", "-> 104DB250"]
    for w in want:
        if w not in out:
            fails.append("1: trace lacks %r" % w)
    exc = [l for l in out.splitlines() if "nested osmem C000A018" in l]
    if not exc or len(exc) % 3 or any(not l.endswith("-> 00000000") for l in exc):
        fails.append("1: OSSetExceptionCallback x3 -> 0 missing")
    for slot in ("C00067B0", "C00067A0", "C0006778"):  # GX2SetBlendControl / BlendConstantColor / AlphaTest: void, r3 kept
        if "nested osmem %s" % slot not in out:
            fails.append("1: GX2 setter %s not modelled" % slot)
    # the setters keep r3, so what reaches GX2SetBlendConstantColor does not depend on the input (the generic mock's r3 does)
    def a0(sp, seed, tr):
        o = run(sp, ["-trace", tr], n="1", seed=seed)[1]
        return [l.split()[3] for l in o.splitlines() if l.strip().startswith("nested call C00067A0")]
    v1, v2 = a0(full, "1", TRACE), a0(full, "2", "2000006")
    if not v1 or v1 != v2:
        fails.append("1: GX2SetBlendConstantColor's r3 depends on the input (a setter did not keep r3)")
    if "setup 028F26F4 -> r3 00000000" not in out:
        fails.append("1: __ghs_flock_ptr block not zeroed (028F26F4 did not skip the unlock)")
    # 2: HiZ outputs
    rc, out, err = run(full, ["-watch", "5C00009C:8"], n="1")
    hiz = [l.split()[2:4] for l in err.splitlines() if l.startswith("watch P")]
    want2 = [["5C00009C", "00"], ["5C00009D", "00"], ["5C00009E", "10"], ["5C00009F", "00"],
             ["5C0000A0", "00"], ["5C0000A1", "00"], ["5C0000A2", "01"], ["5C0000A3", "00"]]
    if hiz[:8] != want2:
        fails.append("2: HiZ outputs: %s" % hiz[:8])
    # 3: rows
    live = {}
    for seed in ("1", "7"):
        for extra in ([], ["-nulltrap"]):
            rs = rows(run(full, extra, seed=seed)[1])
            if not ok(rs):
                fails.append("3: row not ok (seed %s %s): %s" % (seed, extra, rs))
            live[(seed, bool(extra))] = rs
    # 4: snapshot, fastreset, snapcache, refcache
    # the setups that read only fixture state (027B6E2C copies generated words, 0258839C reads the generated clock)
    det = [l for l in SETUPS if not any(k in l for k in ("027B6E2C", "0258839C", "5C002", "101F84DC", "0274FEB0"))]
    snap = spec("snap", SRC + ["osmem hle"] + REAL + det + ["setupsnapshot"])
    nosnap = spec("nosnap", SRC + ["osmem hle"] + REAL + det)
    rc, out, err = run(snap, ["-v"], n="1200")
    if "setupsnapshot: %s captured" % F not in err or "refused" in err:
        fails.append("4: snapshot not captured: %s" % [l for l in err.splitlines() if "setupsnapshot" in l][:3])
    snap_rows = rows(out)
    if snap_rows != rows(run(nosnap, [], n="1200")[1]) or not ok(snap_rows):
        fails.append("4: setupsnapshot rows differ from the live setups: %s" % snap_rows)
    if rows(run(snap, ["-nofastreset"], n="1200")[1]) != snap_rows:
        fails.append("4: -nofastreset rows differ")
    cache = os.path.join(W, "snapcache")
    os.makedirs(cache, exist_ok=True)
    for tag, args in (("write", ["-snapcache-write", cache]), ("read", ["-snapcache", cache]),
                      ("check", ["-snapcache", cache, "-snapcache-check"])):
        rc, out, err = run(snap, ["-snapcache-key", "osmem"] + args, n="1200")
        if rc != 0 or rows(out) != snap_rows:
            fails.append("4: snapcache %s: rc %d rows %s %s" % (tag, rc, rows(out), err[-300:]))
    rcache = os.path.join(W, "refcache")
    os.makedirs(rcache, exist_ok=True)
    ref_live = live[("1", False)]
    for tag, args in (("write", ["-refcache-write", rcache]), ("read", ["-refcache", rcache]),
                      ("check", ["-refcache", rcache, "-refcache-check"])):
        rc, out, err = run(full, ["-refcache-key", "o" * 32] + args)
        if rc != 0 or rows(out) != ref_live:
            fails.append("4: refcache %s: rc %d rows %s" % (tag, rc, rows(out)))
    # 5: off by default
    off = spec("off", SRC + REAL + SETUPS)
    rc, out, err = run(off, ["-trace", TRACE], n="1")
    if "osmem" in out:
        fails.append("5: osmem lines without the directive")
    if "nested call C0009790 r3=00000000" not in out or "-> F4000000" in out:
        fails.append("5: without the directive MEMGetBaseHeapHandle should get the generic answer")
    # 6: parts
    heaps = spec("heaps", SRC + ["osmem heaps"] + REAL + SETUPS)
    rc, out, err = run(heaps, ["-trace", TRACE], n="1")
    if "osmem C0009790" not in out or "osmem C00060B0" in out or "osmem C0009C80" in out or "osmem C000A7B0" in out:
        fails.append("6: `osmem heaps` models the wrong set")
    # 7: conflicts and malformed lines
    for extra in (["ret! C0009790 0x104DB000"], ["alloc C00096A0 r4 0xF4000040 align=r5"], ["nestedret C2000008 1"]):
        rc, out, err = run(spec("conflict", SRC + ["osmem hle"] + REAL + SETUPS + extra), [], n="1")
        if rc != 2 or "osmem models" not in err:
            fails.append("7: conflict not rejected (rc %d): %s" % (rc, extra))
    for bad in (["osmem"], ["osmem foo"], ["osmem hle dataend=xyz"], ["osmem hle dataend=10000000"], ["osmem hle sysinfo=6FFE0004"],
                ["osmem hle", "osmem heaps"]):
        rc, out, err = run(spec("bad", SRC + REAL + SETUPS + bad), [], n="1")
        if rc != 2 or "invalid osmem directive" not in err:
            fails.append("7: not rejected (rc %d): %s" % (rc, bad))
    # 9: data_end and the .bss start against the RPX (the port's load_rpx rule: allocated sections at 02000000..C0000000;
    # NOBITS: addr + size; others at or above 0x10000000: addr + data size)
    import re
    sys.path.insert(0, os.path.join(ROOT, "tools"))
    from rpx import Rpx
    rp = Rpx(os.path.join(ROOT, "game", "code", "cking.rpx"))
    de, bss = 0, None
    for sct in rp.sections:
        if (sct.flags & 2) and 0x02000000 <= sct.addr < 0xC0000000:
            if sct.type == 8:
                de = max(de, sct.addr + sct.size)
                bss = sct.addr if bss is None else min(bss, sct.addr)
            elif sct.addr >= 0x10000000:
                de = max(de, sct.addr + len(sct.data or b""))
    hs = open(os.path.join(HERE, "src", "harness.cpp")).read()
    hde = int(re.search(r"static uint32_t data_end = 0x([0-9A-F]+)u", hs).group(1), 16)
    hbss = int(re.search(r"bss_start = 0x([0-9A-F]+)u", hs).group(1), 16)
    if (de, bss) != (hde, hbss):
        fails.append("9: RPX data_end/.bss %08X/%s vs harness %08X/%08X" % (de, bss and "%08X" % bss, hde, hbss))
    # 8: the heap model against the port's code
    r = subprocess.run([sys.executable, os.path.join(HERE, "osmem_equiv.py")], capture_output=True, text=True)
    if r.returncode != 0:
        fails.append("8: osmem_equiv: %s" % r.stdout.strip()[-300:])

    for f in fails:
        print("FAIL", f)
    print("osmem_test: %s" % ("FAIL" if fails else "PASS"))
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())

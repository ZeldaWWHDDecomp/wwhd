#!/usr/bin/env python3
"""Self-test of the unit-file `mockcopysurface TARGET` directive (harness.cpp, src/surface_calc.cpp).

The mock of GX2CopySurface (C0006120) re-tiles the source level into the destination level in guest memory with the
shipping port's CPU path (runtime/src/gfx/metal_surfaces.mm copy_surface_impl).

Model part (a host program linked with the harness support objects, no guest code):
1. The bytes-per-block / compressed table in surface_calc.cpp equals the colour branch of the port's format_info
   (runtime/src/gfx/formats.mm), read from that file.
2. Linear-aligned to linear-aligned, R8G8B8A8 64x16: element (x, y) goes from and to (y * pitch + x) * 4, pitch the
   address library's.
3. Round trips: linear -> tiled -> linear gives every source byte back at its own offset, and the tiled offsets of a
   level are distinct (a bijection: no element overwrites another), for R8G8B8A8 (2D tiled thin1 with pipe and bank
   swizzle bits set), BC1 and BC3 (4x4 blocks of 8 / 16 bytes), R16, R32G32B32A32 at level 0, level 1 (mipPtr) and
   level 2 (mipPtr + mipOffset[1]), and slice 1 of a 2D array surface.
4. The domain: a level above 13, a level base address of 0, and a descriptor outside the mocksurfacesize domain
   (width 16385) are refused (return 1).
Harness part (verify.py runs; slot-friendly sizes):
5. Opt-in: lib2_agl_3_mip_blur_level0_uniform_observed (02776524 calls C0006120; its kill input 1000003) with `mockcopysurface C0006120` passes
   -n 100 -seed 1 (row ok, all 100 generated inputs), and one -trace input (1000003) prints `mockcopysurface C0006120` byte lines whose values are those its source
   bytes had (each source address read in the trace answers the same value); without the directive no such line.
6. A malformed line (`mockcopysurface`, no target) exits 2.
Temporary files go under WWHD_VERIFY_SCRATCH (or build/verify/_copysurface_test) and the temporary unit directory
tools/verify/units/_copysurface_test/, both removed at the end. Exit 0 when all pass.
"""
import os
import re
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import verify  # noqa: E402  (support_objs: the harness support objects, built once)

UNITS = os.path.join(HERE, "units")
TMPU = os.path.join(UNITS, "_copysurface_test")
SCR = os.path.join(os.environ.get("WWHD_VERIFY_SCRATCH") or os.path.join(ROOT, "build", "verify"), "_copysurface_test")

HOST = r'''
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
extern "C" int verify_copy_bpb(uint32_t fmt, uint32_t* compressed);
extern "C" int verify_copy_surface(const uint8_t*, uint32_t, uint32_t, const uint8_t*, uint32_t, uint32_t, uint32_t*, uint32_t*,
                                   uint32_t*, void (*)(void*, uint32_t, uint32_t), void*);
static void be(uint8_t* d, uint32_t o, uint32_t v) { d[o] = v >> 24; d[o + 1] = v >> 16; d[o + 2] = v >> 8; d[o + 3] = v; }
struct Desc { uint8_t b[0x74]; };
static Desc desc(uint32_t dim, uint32_t w, uint32_t h, uint32_t depth, uint32_t fmt, uint32_t tile, uint32_t swz,
                 uint32_t img, uint32_t mip, uint32_t off1) {
    Desc d; memset(d.b, 0, sizeof d.b);
    be(d.b, 0, dim); be(d.b, 4, w); be(d.b, 8, h); be(d.b, 0xC, depth); be(d.b, 0x10, 13); be(d.b, 0x14, fmt);
    be(d.b, 0x24, img); be(d.b, 0x2C, mip); be(d.b, 0x30, tile); be(d.b, 0x34, swz); be(d.b, 0x44, off1);
    return d;
}
typedef std::vector<std::pair<uint32_t, uint32_t>> Pairs;
static void emit(void* c, uint32_t s, uint32_t d) { ((Pairs*)c)->emplace_back(s, d); }
static int copy(const Desc& s, uint32_t sm, uint32_t ss, const Desc& d, uint32_t dm, uint32_t ds, Pairs& p, uint32_t& sb, uint32_t& db, uint32_t& bpb) {
    p.clear(); return verify_copy_surface(s.b, sm, ss, d.b, dm, ds, &sb, &db, &bpb, emit, &p);
}
int main() {
    int bad = 0;
    for (uint32_t f = 0; f < 0x40; f++) { uint32_t c; int n = verify_copy_bpb(f, &c); printf("bpb %X %d %u\n", f, n, c); }
    { /* 2: linear aligned R8G8B8A8 64x16 -> linear aligned */
        Desc a = desc(1, 64, 16, 1, 0x1A, 1, 0, 0x10000000, 0x10100000, 0), b = desc(1, 64, 16, 1, 0x1A, 1, 0, 0x20000000, 0x20100000, 0);
        Pairs p; uint32_t sb, db, bpb;
        if (copy(a, 0, 0, b, 0, 0, p, sb, db, bpb) || p.size() != 64 * 16 || bpb != 4 || sb != 0x10000000 || db != 0x20000000) { printf("FAIL linear setup\n"); bad++; }
        uint32_t pitch = p.size() > 64 ? p[64].first / 4 : 0;
        for (uint32_t i = 0; i < p.size(); i++) {
            uint32_t x = i % 64, y = i / 64, want = (y * pitch + x) * 4;
            if (p[i].first != want || p[i].second != want) { printf("FAIL linear %u,%u: %X %X want %X\n", x, y, p[i].first, p[i].second, want); bad++; break; }
        }
        printf("linear pitch %u\n", pitch);
    }
    struct Case { const char* name; uint32_t dim, w, h, depth, fmt, tile, swz, level, slice; };
    const Case cases[] = {
        {"rgba8 2D tiled thin1 swizzled", 1, 256, 128, 1, 0x1A, 4, (1u << 8) | (2u << 9), 0, 0},
        {"rgba8 level 1", 1, 256, 128, 1, 0x1A, 4, 0, 1, 0},
        {"rgba8 level 2", 1, 256, 128, 1, 0x1A, 4, 0, 2, 0},
        {"bc1 2D tiled", 1, 256, 256, 1, 0x31, 4, 0, 0, 0},
        {"bc3 level 1", 1, 128, 128, 1, 0x33, 4, (1u << 8), 1, 0},
        {"r16 micro tiled", 1, 64, 64, 1, 0x05, 2, 0, 0, 0},
        {"rgba32 2D tiled", 1, 64, 64, 1, 0x23, 4, 0, 0, 0},
        {"rgba8 2D array slice 1", 5, 128, 128, 4, 0x1A, 4, 0, 0, 1},
    };
    for (const Case& k : cases) {
        /* linear surface L (tile 1 = linear aligned) and tiled T of the same shape; mipOffset[1] for level 2 */
        Desc L = desc(k.dim, k.w, k.h, k.depth, k.fmt, 1, 0, 0x10000000, 0x14000000, 0x40000);
        Desc T = desc(k.dim, k.w, k.h, k.depth, k.fmt, k.tile, k.swz, 0x20000000, 0x24000000, 0x40000);
        Pairs p1, p2; uint32_t sb1, db1, sb2, db2, bpb;
        if (copy(L, k.level, k.slice, T, k.level, k.slice, p1, sb1, db1, bpb) || copy(T, k.level, k.slice, L, k.level, k.slice, p2, sb2, db2, bpb)) {
            printf("FAIL %s: refused\n", k.name); bad++; continue;
        }
        uint32_t wantBase = k.level == 0 ? 0x20000000 : k.level == 1 ? 0x24000000 : 0x24040000;
        std::map<uint32_t, uint32_t> fwd;  /* linear offset -> tiled offset */
        std::set<uint32_t> tiled;
        bool ok = db1 == wantBase && sb2 == wantBase && p1.size() == p2.size() && !p1.empty();
        for (auto& [s, d] : p1) { fwd[s] = d; ok = ok && tiled.insert(d).second; }
        for (auto& [s, d] : p2) { auto it = fwd.find(d); ok = ok && it != fwd.end() && it->second == s; }
        /* distinct byte ranges: offsets bpb apart at least */
        uint32_t prev = 0; bool first = true;
        for (uint32_t t : tiled) { ok = ok && (first || t - prev >= bpb); prev = t; first = false; }
        printf("%s %s: %zu elements, %u bytes each, base %08X\n", ok ? "ok" : "FAIL", k.name, p1.size(), bpb, db1);
        bad += !ok;
    }
    { /* 4: the domain */
        Desc a = desc(1, 64, 64, 1, 0x1A, 4, 0, 0x10000000, 0x14000000, 0), z = desc(1, 64, 64, 1, 0x1A, 4, 0, 0x10000000, 0, 0);
        Desc w = desc(1, 16385, 64, 1, 0x1A, 4, 0, 0x10000000, 0x14000000, 0);
        Pairs p; uint32_t sb, db, bpb;
        int r1 = copy(a, 14, 0, a, 0, 0, p, sb, db, bpb), r2 = copy(z, 1, 0, a, 1, 0, p, sb, db, bpb), r3 = copy(w, 0, 0, a, 0, 0, p, sb, db, bpb),
            r4 = copy(a, 0, 0, a, 14, 0, p, sb, db, bpb);
        bool ok = r1 == 1 && r2 == 1 && r3 == 1 && r4 == 1;
        printf("%s domain: level 14 %d, level base 0 %d, width 16385 %d, dst level 14 %d\n", ok ? "ok" : "FAIL", r1, r2, r3, r4);
        bad += !ok;
    }
    printf("host bad %d\n", bad);
    return bad != 0;
}
'''


def port_table():
    """(hw format -> (bytesPerBlock, compressed)) from the colour branch of the port's format_info"""
    src = open(os.path.join(ROOT, "runtime", "src", "gfx", "formats.mm")).read()
    body = src[src.index("FormatInfo format_info(uint32_t fmt"):]
    body = body[body.index("    FormatInfo f;\n    switch (hw)"):body.index("    return f;\n}")]
    out = {}
    for line in body.splitlines():
        cases = [int(x, 16) for x in re.findall(r"case (0x[0-9A-Fa-f]+):", line)]
        m = re.search(r"f = make\(.*?, (\d+)(?:, [^)]*)?\)", line) or re.search(r", (\d+)(?:, [^,()]+)*\);", line)
        if "default:" in line:
            out["default"] = (int(re.search(r", (\d+)\)", line).group(1)), 0)
            continue
        if not cases:
            continue
        if not m:  # multi-line case (0x1A): the size follows on the next lines
            m = re.search(r"\n\s+(\d+), kind\);", body[body.index(line):])
        n = int(m.group(1))
        for c in cases:
            out[c] = (n, 1 if "compressed = true" in line else 0)
    return out


def host_part():
    os.makedirs(SCR, exist_ok=True)
    cpp, exe = os.path.join(SCR, "host.cpp"), os.path.join(SCR, "host")
    with open(cpp, "w") as f:
        f.write(HOST)
    os.chdir(ROOT)
    objs = verify.support_objs()
    subprocess.run(["clang++", "-std=c++20", "-O1", cpp] + objs + ["-o", exe], check=True)
    out = subprocess.run([exe], capture_output=True, text=True).stdout
    fails = [l for l in out.splitlines() if l.startswith("FAIL")]
    for l in out.splitlines():
        if not l.startswith("bpb "):
            print("  " + l)
    model = {}
    for l in out.splitlines():
        if l.startswith("bpb "):
            _, f, n, c = l.split()
            model[int(f, 16)] = (int(n), int(c))
    port = port_table()
    dflt = port.pop("default")
    tdiff = [f for f in range(0x40) if model[f] != port.get(f, dflt)]
    print("  %s format table vs formats.mm: %d formats listed by the port, %d differ %s" % (
        "ok" if not tdiff else "FAIL", len(port), len(tdiff), ["%X" % f for f in tdiff]))
    return len(fails) + (1 if tdiff else 0) + (0 if "host bad 0" in out else 1)


def verify_run(name, lines, extra):
    with open(os.path.join(TMPU, name + ".txt"), "w") as f:
        f.write("\n".join(lines) + "\n")
    r = subprocess.run([sys.executable, os.path.join(HERE, "verify.py"), "_copysurface_test/" + name] + list(extra),
                       cwd=ROOT, capture_output=True, text=True)
    return r.returncode, r.stdout + r.stderr


def harness_part():
    bad = 0
    os.makedirs(TMPU, exist_ok=True)
    # the shipped spec carries `mockcopysurface C0006120` itself: strip it, so "off" is off and "on" adds exactly one line
    base = [l for l in open(os.path.join(UNITS, "lib2_agl_3_mip_blur_level0_uniform_observed.txt")).read().splitlines()
            if not l.split("#", 1)[0].strip().startswith("mockcopysurface")]
    rc, out = verify_run("on", base + ["mockcopysurface C0006120"], ["-n", "100", "-seed", "1", "-only", "02776524"])
    ok = rc == 0 and re.search(r"^02776524 .* 100/100 .* ok$", out, re.M) is not None
    print("  %s directive on: rc %d" % ("ok" if ok else "FAIL", rc))
    bad += not ok
    if not ok:
        print(out[-3000:])
    # one traced input: every byte line's value equals what its source address held when read by the copy (the model
    # reads before the epoch; a later byte line may read a destination an earlier one wrote, as the port's loop does)
    rc, out = verify_run("on", base + ["mockcopysurface C0006120"], ["-n", "4", "-seed", "1", "-only", "02776524", "-trace", "1000003"])
    calls, lines, mem, consistent = 0, 0, {}, True
    for l in out.splitlines():
        if re.search(r"mockcopysurface C0006120 \d+ bytes", l):
            calls += 1
            mem = {}  # per call: guest code may change memory between calls
        m = re.search(r"mockcopysurface byte ([0-9A-F]{8}) -> ([0-9A-F]{8}) = ([0-9A-F]{2})", l)
        if m:
            s, d, v = m.groups()
            lines += 1
            if s in mem and mem[s] != v:
                consistent = False
            mem.setdefault(s, v)
            mem[d] = v
    ok = rc in (0, 1) and calls and consistent
    print("  %s traced input: %d copy call(s), %d byte lines, values consistent %s" % ("ok" if ok else "FAIL", calls, lines, consistent))
    bad += not ok
    rc, out = verify_run("off", base, ["-n", "4", "-seed", "1", "-only", "02776524", "-trace", "1000003"])
    ok = "mockcopysurface" not in out
    print("  %s directive off: no model lines" % ("ok" if ok else "FAIL"))
    bad += not ok
    rc, out = verify_run("malformed", base + ["mockcopysurface"], ["-n", "2", "-seed", "1", "-only", "02776524"])
    ok = rc == 2 and "invalid mockcopysurface directive" in out
    print("  %s malformed line: rc %d" % ("ok" if ok else "FAIL", rc))
    bad += not ok
    return bad


def main():
    bad = 0
    try:
        print("model (host):")
        bad += host_part()
        if "--host-only" not in sys.argv:
            print("harness:")
            bad += harness_part()
    finally:
        shutil.rmtree(TMPU, ignore_errors=True)
        shutil.rmtree(SCR, ignore_errors=True)
    print("copysurface_test: %s" % ("OK" if not bad else "%d FAILED" % bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())

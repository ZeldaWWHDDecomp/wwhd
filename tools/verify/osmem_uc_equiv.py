#!/usr/bin/env python3
"""osmem_uc_equiv.py: the `osmem uc` model (harness.cpp namespace osm) against the shipping port's own UC code.

Extracts HLE(coreinit, UCOpen / UCClose / UCReadSysConfig) from the port's runtime/src/hle/coreinit_misc.cpp (read only;
--port-src, default $WWHD_PORT_SRC or runtime/src (the port checkout the harness runs in)), compiles them with shim headers
(console_language() = 1: no WWHD_LANGUAGE and no saved setting) next to the whole osm namespace of
tools/verify/src/harness.cpp, and runs seeded random UCReadSysConfig item arrays on both (names: the six the port knows,
an unknown one, an empty one; dataSize 0/1/2/3/4/8/64; dataPtr 0 or a buffer; counts 0..12; stale error words), plus
UCOpen/UCClose. r3 and every byte of the item array and the data buffers must be equal. Exit 0 on PASS."""
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))


def block(src, pat):
    m = re.search(pat, src, re.M)
    if not m:
        return None
    i = src.index("{", m.start())
    depth = 0
    for j in range(i, len(src)):
        if src[j] == "{":
            depth += 1
        elif src[j] == "}":
            depth -= 1
            if depth == 0:
                return src[m.start():j + 1]
    return None


PORT_SHIM = r"""
namespace port {
struct Cpu { uint32_t r[32]; };
static uint8_t* g_mem;
namespace mem { inline uint8_t* ptr(uint32_t ea) { return g_mem + ea; }
inline std::string read_cstr(uint32_t ea) { if (!ea) return {}; return std::string((const char*)ptr(ea)); } }
inline uint32_t ld32(uint32_t ea) { uint32_t v; memcpy(&v, mem::ptr(ea), 4); return __builtin_bswap32(v); }
inline void st32(uint32_t ea, uint32_t v) { v = __builtin_bswap32(v); memcpy(mem::ptr(ea), &v, 4); }
inline void st16(uint32_t ea, uint16_t v) { v = __builtin_bswap16(v); memcpy(mem::ptr(ea), &v, 2); }
inline void st8(uint32_t ea, uint8_t v) { *mem::ptr(ea) = v; }
inline uint32_t arg(Cpu* c, int i) { return c->r[3 + i]; }
inline void ret(Cpu* c, uint32_t v) { c->r[3] = v; }
static uint32_t console_language() { return 1; }
#define HLE(lib, name) void imp_##lib##_##name(Cpu* c)
#define TRACE(...) do {} while (0)
%s
%s
%s
}
"""

HARNESS_SHIM = r"""
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <sys/mman.h>
struct HCpu { uint32_t r[32]; };
#define Cpu HCpu
static struct { int depth = 0; } V;
static std::unordered_map<uint32_t, uint8_t> g_hmem;
static void patch8(uint32_t ea, uint8_t v) { g_hmem[ea] = v; }
static uint8_t peek8(uint32_t ea) { auto it = g_hmem.find(ea); return it == g_hmem.end() ? 0 : it->second; }
static void tick() {}
[[noreturn]] static void model_reject(uint32_t, const char* why) { throw std::runtime_error(why); }
static std::vector<std::pair<uint32_t, uint32_t>> g_stable, g_zero_init;
static int g_trace = 0;
static bool dl_recording() { return false; }
"""

DRIVER = r"""
#undef Cpu
static uint64_t rs = 1;
static uint32_t rnd() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (uint32_t)rs; }
static void set8(uint32_t a, uint8_t v) { port::g_mem[a] = v; g_hmem[a] = v; }
static void set32(uint32_t a, uint32_t v) { for (int j = 0; j < 4; j++) set8(a + j, (uint8_t)(v >> (24 - 8 * j))); }
int main() {
    port::g_mem = (uint8_t*)mmap(nullptr, 0x1000000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    osm::parts = osm::P_UC;
    static const char* names[] = {"cafe.language", "cafe.cntry_reg", "cafe.eula_agree", "cafe.initial_launch", "parent.enable",
                                  "p_acct1.network_launcher", "cafe.version", ""};
    static const uint32_t sizes[] = {0, 1, 2, 3, 4, 8, 64};
    const uint32_t ITEMS = 0x100000, DATA = 0x200000;
    long ops = 0, diffs = 0;
    for (int seed = 1; seed <= 3000 && !diffs; seed++, ops++) {
        rs = 0x9E3779B97F4A7C15ull * seed;
        uint32_t count = rnd() % 13;
        for (uint32_t j = 0; j < 13 * 0x54; j++) set8(ITEMS + j, (uint8_t)rnd());
        for (uint32_t j = 0; j < 13 * 0x80; j++) set8(DATA + j, (uint8_t)rnd());
        for (uint32_t i = 0; i < count; i++) {
            uint32_t e = ITEMS + i * 0x54;
            const char* n = names[rnd() % 8];
            for (size_t k = 0; k <= strlen(n); k++) set8(e + (uint32_t)k, (uint8_t)n[k]);
            set32(e + 0x4C, sizes[rnd() % 7]);
            set32(e + 0x50, rnd() % 5 == 0 ? 0 : DATA + i * 0x80);
        }
        uint32_t target = seed % 10 == 0 ? 0xC000A430u : seed % 10 == 1 ? 0xC000A408u : 0xC000A448u;
        port::Cpu pc{}; HCpu hc{};
        pc.r[3] = hc.r[3] = rnd(); pc.r[4] = hc.r[4] = count; pc.r[5] = hc.r[5] = ITEMS;
        if (target == 0xC000A430u) port::imp_coreinit_UCOpen(&pc);
        else if (target == 0xC000A408u) port::imp_coreinit_UCClose(&pc);
        else port::imp_coreinit_UCReadSysConfig(&pc);
        uint32_t a[11]; for (int i = 3; i <= 10; i++) a[i] = hc.r[i];
        try { osm::call(&hc, target, a); } catch (std::exception& ex) { printf("seed %d: model rejected: %s\n", seed, ex.what()); diffs++; break; }
        if (pc.r[3] != hc.r[3]) { printf("seed %d target %08X: r3 port %08X osm %08X\n", seed, target, pc.r[3], hc.r[3]); diffs++; break; }
        for (uint32_t base : {ITEMS, DATA})
            for (uint32_t j = 0; j < 13 * 0x80 && !diffs; j++)
                if (port::g_mem[base + j] != peek8(base + j)) { printf("seed %d: byte %08X port %02X osm %02X\n", seed, base + j, port::g_mem[base + j], peek8(base + j)); diffs++; }
    }
    printf("osmem_uc_equiv: %ld calls, %ld differences\n", ops, diffs);
    return diffs ? 1 : 0;
}
"""


def main():
    args = sys.argv[1:]
    port = args[args.index("--port-src") + 1] if "--port-src" in args else None
    port = port or os.environ.get("WWHD_PORT_SRC") or "runtime/src"
    psrc = open(os.path.join(port, "hle", "coreinit_misc.cpp")).read()
    pieces = [block(psrc, r"^HLE\(coreinit, UCOpen\)"), block(psrc, r"^HLE\(coreinit, UCClose\)"), block(psrc, r"^HLE\(coreinit, UCReadSysConfig\)")]
    if not all(pieces):
        print("osmem_uc_equiv: cannot find the port's UC functions")
        return 2
    hsrc = open(os.path.join(HERE, "src", "harness.cpp")).read()
    osm = hsrc[hsrc.index("namespace osm {"):hsrc.index("}  // namespace osm") + len("}  // namespace osm")]
    d = tempfile.mkdtemp(prefix="osmem_uc_equiv-", dir=os.environ.get("TMPDIR"))
    cpp, exe = os.path.join(d, "eq.cpp"), os.path.join(d, "eq")
    open(cpp, "w").write(HARNESS_SHIM + osm + "\n#undef Cpu\n" + PORT_SHIM % tuple(pieces) + DRIVER)
    r = subprocess.run(["clang++", "-std=c++17", "-O1", "-w", "-o", exe, cpp], capture_output=True, text=True)
    if r.returncode:
        print(r.stderr[-3000:])
        print("osmem_uc_equiv: FAIL (compile)")
        return 2
    r = subprocess.run([exe], capture_output=True, text=True)
    print(r.stdout.strip()[-1500:])
    print("osmem_uc_equiv: %s" % ("PASS" if r.returncode == 0 else "FAIL"))
    return r.returncode


if __name__ == "__main__":
    sys.exit(main())

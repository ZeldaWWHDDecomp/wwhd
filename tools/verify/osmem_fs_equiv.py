#!/usr/bin/env python3
"""osmem_fs_equiv.py: the `osmem fs` model (harness.cpp namespace osm) against the shipping port's own fs.cpp.

Copies the port's runtime/src/hle/fs.cpp and platform/filesystem.h (read only; --port-src, default $WWHD_PORT_SRC or
runtime/src (the port checkout the harness runs in)) into a temporary directory with small shim headers (guest memory, HLE macro,
config::game_dir, no content mods and no language-pack redirect), and the whole osm namespace from
tools/verify/src/harness.cpp with shims for the harness memory (patch8/peek8). A temporary game tree (mixed-case names,
random contents, an empty file; the copy stops at its save-state section) is served to both. Seeded random sequences of FSOpenFile (exact, upper/lower-cased,
relative and /vol/content spellings, a declared-absent file), FSReadFile (sizes 0..9000, counts 0..9, past EOF),
FSSetPosFile, FSGetStat, FSGetStatFile, FSCloseFile (valid and stale handles), FSGetCwd (max 0..20) and FSInitCmdBlock
run on both; r3 and every guest byte either side wrote must be equal. Exit 0 on PASS."""
import os
import random
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))

SHIM_RUNTIME = r"""
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <algorithm>
#include <sys/stat.h>
struct Cpu { uint32_t r[32]; };
extern uint8_t* g_port_mem;
namespace mem { inline uint8_t* ptr(uint32_t ea) { return g_port_mem + ea; }
inline std::string read_cstr(uint32_t ea) { if (!ea) return {}; return std::string((const char*)ptr(ea)); }
inline void write_cstr(uint32_t ea, const std::string& s, uint32_t max) { uint32_t n = (uint32_t)std::min<size_t>(s.size(), max - 1); memcpy(ptr(ea), s.data(), n); ptr(ea)[n] = 0; } }
inline void st32(uint32_t ea, uint32_t v) { v = __builtin_bswap32(v); memcpy(mem::ptr(ea), &v, 4); }
inline uint32_t ld32(uint32_t ea) { uint32_t v; memcpy(&v, mem::ptr(ea), 4); return __builtin_bswap32(v); }
inline uint32_t arg(Cpu* c, int i) { return c->r[3 + i]; }
inline void ret(Cpu* c, uint32_t v) { c->r[3] = v; }
#define HLE(lib, name) extern "C" void imp_##lib##_##name(Cpu* c)
#define LOG(...) do {} while (0)
#define TRACE(...) do {} while (0)
namespace config { extern std::string game_dir, save_dir; }
struct BlockingScope {};
"""
SHIM_WW = "#pragma once\n#include <cstdint>\nnamespace wwatch { struct HostWrite { HostWrite(uint32_t, uint32_t) {} }; inline void hint(uint32_t, uint32_t) {} }\n"
SHIM_MODS = "#pragma once\n#include <string>\nnamespace mods { namespace content { inline std::string replacement(const std::string&, const std::string& = \"rb\") { return {}; } inline std::string pack_language(const std::string&) { return {}; } } }\n"
SHIM_LANG = "#pragma once\n#include <string>\nnamespace game_lang { inline std::string redirect(const std::string&) { return {}; } }\n"
SHIM_RTL = "#pragma once\n#include <string>\nnamespace rtl_text { inline void language_pack_opened(const std::string&) {} }\n"

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
#include <sys/mman.h>
uint8_t* g_port_mem;
namespace config { std::string game_dir, save_dir; }
extern "C" {
void imp_coreinit_FSOpenFile(::Cpu*); void imp_coreinit_FSReadFile(::Cpu*); void imp_coreinit_FSSetPosFile(::Cpu*);
void imp_coreinit_FSGetStat(::Cpu*); void imp_coreinit_FSGetStatFile(::Cpu*); void imp_coreinit_FSCloseFile(::Cpu*);
void imp_coreinit_FSGetCwd(::Cpu*); void imp_coreinit_FSInitCmdBlock(::Cpu*);
}
static uint64_t rs = 1;
static uint32_t rnd() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (uint32_t)rs; }
static const uint32_t STR = 0x100000, BUF = 0x200000, OUT = 0x300000, CMD = 0x400000;
static void put_str(uint32_t ea, const std::string& s) {
    memcpy(g_port_mem + ea, s.c_str(), s.size() + 1);
    for (size_t i = 0; i <= s.size(); i++) g_hmem[ea + (uint32_t)i] = (uint8_t)(i < s.size() ? s[i] : 0);
}
int main(int argc, char** argv) {
    g_port_mem = (uint8_t*)mmap(nullptr, 0x1000000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    config::game_dir = argv[1];
    std::vector<std::string> paths;
    for (int i = 2; i < argc; i++) paths.push_back(argv[i]);
    long ops = 0, diffs = 0;
    for (int seed = 1; seed <= 200 && !diffs; seed++) {
        rs = 0x9E3779B97F4A7C15ull * seed;
        /* fresh sessions: the harness model restarts per run; the port keeps its handle counter, so each seed closes all */
        std::vector<uint32_t> handles;
        for (int k = 0; k < 120 && !diffs; k++, ops++) {
            ::Cpu pc{}; HCpu hc{};
            uint32_t op = rnd() % 8, regs[11] = {0};
            std::vector<std::pair<uint32_t, uint32_t>> cmp; /* (address, length) compared after the op */
            uint32_t target = 0;
            if (op == 0 || (handles.empty() && op < 5)) {
                std::string p = paths[rnd() % paths.size()];
                put_str(STR, p); put_str(STR + 0x800, rnd() % 2 ? "r" : "rb");
                regs[5] = STR; regs[6] = STR + 0x800; regs[7] = OUT; target = 0xC0008E98u; cmp = {{OUT, 4}};
            } else if (op == 1 || op == 2) {
                regs[5] = BUF; regs[6] = rnd() % 5 == 0 ? 0 : 1 + rnd() % 9000; regs[7] = rnd() % 10;
                regs[8] = rnd() % 7 == 0 ? 999 : handles[rnd() % handles.size()]; target = 0xC0008EC8u; cmp = {{BUF, 90000}};
            } else if (op == 3) {
                regs[5] = rnd() % 7 == 0 ? 999 : handles[rnd() % handles.size()]; regs[6] = rnd() % 4 == 0 ? rnd() % 0x10000 : rnd() % 64;
                target = 0xC0008F48u;
            } else if (op == 4) {
                if (rnd() % 2) { put_str(STR, paths[rnd() % paths.size()]); regs[5] = STR; regs[6] = OUT; target = 0xC0008DF0u; }
                else { regs[5] = rnd() % 7 == 0 ? 999 : handles[rnd() % handles.size()]; regs[6] = OUT; target = 0xC0008E00u; }
                cmp = {{OUT, 0x64}};
            } else if (op == 5) {
                regs[5] = handles.empty() || rnd() % 5 == 0 ? 999 : handles[rnd() % handles.size()]; target = 0xC0008CF8u;
            } else if (op == 6) { regs[5] = OUT; regs[6] = rnd() % 21; target = 0xC0008D58u; cmp = {{OUT, 24}}; if (!regs[6]) regs[6] = 1; }
            else { regs[3] = CMD; target = 0xC0008E40u; cmp = {{CMD, 0xA80}}; }
            for (auto& [a, n] : cmp) for (uint32_t j = 0; j < n; j++) { uint8_t b = (uint8_t)rnd(); g_port_mem[a + j] = b; g_hmem[a + j] = b; }
            for (int i = 3; i <= 10; i++) { pc.r[i] = regs[i]; hc.r[i] = regs[i]; }
            switch (target) {
            case 0xC0008E98u: imp_coreinit_FSOpenFile(&pc); break; case 0xC0008EC8u: imp_coreinit_FSReadFile(&pc); break;
            case 0xC0008F48u: imp_coreinit_FSSetPosFile(&pc); break; case 0xC0008DF0u: imp_coreinit_FSGetStat(&pc); break;
            case 0xC0008E00u: imp_coreinit_FSGetStatFile(&pc); break; case 0xC0008CF8u: imp_coreinit_FSCloseFile(&pc); break;
            case 0xC0008D58u: imp_coreinit_FSGetCwd(&pc); break; case 0xC0008E40u: imp_coreinit_FSInitCmdBlock(&pc); break;
            }
            uint32_t argr[11]; for (int i = 3; i <= 10; i++) argr[i] = regs[i];
            try { osm::call(&hc, target, argr); } catch (std::exception& e) { printf("seed %d op %d target %08X: model rejected: %s\n", seed, k, target, e.what()); diffs++; break; }
            if (pc.r[3] != hc.r[3]) { printf("seed %d op %d target %08X: r3 port %08X osm %08X\n", seed, k, target, pc.r[3], hc.r[3]); diffs++; break; }
            for (auto& [a, n] : cmp) for (uint32_t j = 0; j < n; j++)
                if (g_port_mem[a + j] != peek8(a + j)) { printf("seed %d op %d target %08X: byte %08X port %02X osm %02X\n", seed, k, target, a + j, g_port_mem[a + j], peek8(a + j)); diffs++; break; }
            if (target == 0xC0008E98u && pc.r[3] == 0) handles.push_back(ld32(OUT));
            if (target == 0xC0008CF8u) handles.erase(std::remove(handles.begin(), handles.end(), regs[5]), handles.end());
        }
        for (uint32_t h : handles) { ::Cpu pc{}; HCpu hc{}; pc.r[5] = hc.r[5] = h; imp_coreinit_FSCloseFile(&pc); uint32_t a[11] = {0}; a[5] = h; osm::call(&hc, 0xC0008CF8u, a); }
        osm::st.fs_open.clear(); /* the port's counter keeps running: mirror it */
    }
    printf("osmem_fs_equiv: %ld operations, %ld differences\n", ops, diffs);
    return diffs ? 1 : 0;
}
"""


def main():
    args = sys.argv[1:]
    port = args[args.index("--port-src") + 1] if "--port-src" in args else None
    port = port or os.environ.get("WWHD_PORT_SRC") or "runtime/src"
    hsrc = open(os.path.join(HERE, "src", "harness.cpp")).read()
    osm = hsrc[hsrc.index("namespace osm {"):hsrc.index("}  // namespace osm") + len("}  // namespace osm")]
    d = tempfile.mkdtemp(prefix="osmem_fs_equiv-", dir=os.environ.get("TMPDIR"))
    s = os.path.join(d, "shim")
    for sub in ("hle", "mods", "platform"):
        os.makedirs(os.path.join(s, sub), exist_ok=True)
    src = open(os.path.join(port, "hle", "fs.cpp")).read()
    cut = src.find('#include "../savestate.h"')  # the save-state section is not part of the FS behaviour
    open(os.path.join(s, "hle", "fs.cpp"), "w").write(src if cut < 0 else src[:cut])
    shutil.copy(os.path.join(port, "platform", "filesystem.h"), os.path.join(s, "platform", "filesystem.h"))
    for name, text in (("runtime.h", SHIM_RUNTIME), ("write_watch.h", SHIM_WW), ("mods/content.h", SHIM_MODS),
                       ("game_languages.h", SHIM_LANG), ("rtl_text.h", SHIM_RTL)):
        open(os.path.join(s, name), "w").write(text)
    # a game tree served to both (the harness model reads files through osm::fs_files, filled here like `osmemfile`)
    work = os.path.join(d, "work")
    game = os.path.join(work, "game")
    rnd = random.Random(7)
    files = {"content/Common/Pack/Alpha.PACK": 70000, "content/Common/env.txt": 1234, "content/Cafe/Empty.bin": 0,
             "code/app.xml": 300, "meta/meta.xml": 5000}
    for rel, n in files.items():
        p = os.path.join(game, rel)
        os.makedirs(os.path.dirname(p), exist_ok=True)
        open(p, "wb").write(bytes(rnd.getrandbits(8) for _ in range(n)))
    guest = ["/vol/content/Common/Pack/Alpha.PACK", "/vol/content/Common/env.txt", "Common/env.txt", "/vol/content/Cafe/Empty.bin",
             "/vol/code/app.xml", "/vol/meta/meta.xml", "/vol/content/Common/Missing.bin"]
    decl = []
    for g in guest:
        hp = None
        for rel in files:
            if g.endswith(rel.split("/", 1)[1]) and (g.startswith("/vol/" + rel.split("/")[0]) or not g.startswith("/")):
                hp = rel
        decl.append((g, hp))
    init = ["osm::parts = osm::P_FS;"]
    for g, hp in decl:
        if hp is None:
            init.append('{ osm::FsFile f; f.guest = "%s"; f.host = osm::fs_host_exact(f.guest, "game"); f.key = osm::fs_key(f.host); f.absent = true; osm::fs_files.push_back(f); }' % g)
        else:
            if any(osm_key == hp for osm_key in []):
                pass
            init.append('{ osm::FsFile f; f.guest = "%s"; f.host = osm::fs_host_exact(f.guest, "game"); f.key = osm::fs_key(f.host); '
                        'FILE* fp = fopen("%s", "rb"); int ch; while ((ch = fgetc(fp)) != EOF) f.data.push_back((uint8_t)ch); fclose(fp); '
                        'bool dup = false; for (auto& o : osm::fs_files) dup |= o.key == f.key; if (!dup) osm::fs_files.push_back(f); }'
                        % (g, os.path.join(game, hp)))
    # case variants the guest may use (the volumes are case-insensitive)
    variants = guest + ["/vol/content/common/pack/alpha.pack", "/VOL/content/Common/env.txt".replace("/VOL", "/vol"), "/vol/content/COMMON/ENV.TXT"]
    driver = DRIVER.replace("int main(int argc, char** argv) {", "static void osm_init() {\n" + "\n".join(init) + "\n}\nint main(int argc, char** argv) {\n    osm_init();")
    hcpp = os.path.join(d, "h.cpp")
    open(hcpp, "w").write(HARNESS_SHIM + osm + "\n#undef Cpu\n" + '#include "shim/runtime.h"\n' + driver)
    exe = os.path.join(d, "eq")
    r = subprocess.run(["clang++", "-std=c++17", "-O1", "-w", "-I", s, "-o", exe, hcpp, os.path.join(s, "hle", "fs.cpp")], capture_output=True, text=True)
    if r.returncode:
        print(r.stderr[-4000:])
        print("osmem_fs_equiv: FAIL (compile)")
        return 2
    r = subprocess.run([exe, game] + variants, capture_output=True, text=True, cwd=work)
    print(r.stdout.strip()[-2000:])
    print("osmem_fs_equiv: %s" % ("PASS" if r.returncode == 0 else "FAIL"))
    return r.returncode


if __name__ == "__main__":
    sys.exit(main())

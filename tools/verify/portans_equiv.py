#!/usr/bin/env python3
"""portans_equiv.py: the `osmem ... portconst` table (tools/verify/src/portans_model.h) against the shipping port's own HLE at
commit f5554821 (--port-rev): each table row's HLE body is cut from the file its comment cites and compiled with shims (GX2DrawDone:
BlockingScope, emit_host, render_sync do nothing; the host wait takes no guest time), and called with random registers: r3 must equal
the table and no other register may change. A row whose import has no HLE in the port must be absent from every port source, and its
answer is core.cpp hle_unimplemented's, compiled from the port's text. The stub deltas are re-measured from build/gen (every `bl` to
an import), as strictmocks_test.py 6 does.
The `portmem` imports likewise, on two copies of a random guest window: the port's lines with shims (timebase::guest_now = the
test's clock; open_file = no file on the host, with fs.cpp's fopen and FS_NOT_FOUND lines checked in the text; write modes never
reach the port side and must be a model rejection), offsets taken from the port's struct headers.
--inject: each single-row bug in a copy of the table must make the comparison FAIL. Exit 0 on PASS."""
import argparse
import glob
import json
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)

# table name -> (lib as in the HLE macro, HLE name, port file; None = no HLE: hle_unimplemented)
ROWS = {
    "gx2 GX2DrawDone": ("gx2", "GX2DrawDone", "gx2/gx2_core.cpp"),
    "nn_ac Connect": ("nn_ac", "Connect__Q2_2nn2acFv", "hle/system_stubs.cpp"),
    "nn_ac Initialize": ("nn_ac", "Initialize__Q2_2nn2acFv", "hle/system_stubs.cpp"),
    "nn_olv Initialize": ("nn_olv", "Initialize__Q2_2nn3olvFPCQ3_2nn3olv15InitializeParam", "hle/system_stubs.cpp"),
    "nlibcurl curl_global_init_mem": ("nlibcurl", "curl_global_init_mem", "hle/system_stubs.cpp"),
    "nsysnet NSSLInit": ("nsysnet", "NSSLInit", "hle/system_stubs.cpp"),
    "nsysnet socket_lib_init": ("nsysnet", "socket_lib_init", "hle/system_stubs.cpp"),
    "coreinit IMDisableDim": ("coreinit", "IMDisableDim", "hle/coreinit_misc.cpp"),
    "coreinit OSEnableHomeButtonMenu": ("coreinit", "OSEnableHomeButtonMenu", "hle/coreinit_misc.cpp"),
    "coreinit OSGetSharedData": ("coreinit", "OSGetSharedData", "hle/coreinit_misc.cpp"),
    "nn_olv SetReportTypes": ("nn_olv.rpl", "SetReportTypes__Q3_2nn3olv6ReportFUi", None),
    "nn_olv InitializeParam::SetWork": ("nn_olv.rpl", "SetWork__Q3_2nn3olv15InitializeParamFPUcUi", None),
    "nn_olv InitializeParam ctor": ("nn_olv.rpl", "__ct__Q3_2nn3olv15InitializeParamFv", None),
}
PRELUDE = r"""
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
struct Cpu { uint32_t r[32]; uint32_t lr; };
inline uint32_t arg(Cpu* c, int i) { return c->r[3 + i]; }
inline void ret(Cpu* c, uint32_t v) { c->r[3] = v; }
static inline void log_msg(const char*, ...) {}
struct BlockingScope {};
enum { OP_DRAW_DONE = 1, kSyncDrawDone = 2 };
static inline void emit_host(int, std::vector<uint32_t>) {}
static inline void render_sync(int) {}
static constexpr uint32_t kResultOk = 0, kResultFail = 0xA0000000;
#define HLE(lib, name) void imp_##lib##_##name(Cpu* c)
"""
MAIN = r"""
#include "portans_model.h"
static uint64_t rs = 88172645463325252ull;
static uint32_t rnd32() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (uint32_t)(rs >> 11); }
int main() {
    long calls = 0;
    for (auto& e : FNS) {
        const portans::Constant* k = nullptr;
        for (auto& x : portans::kConstants) if (std::string(x.name) == e.name) k = &x;
        if (!k) { printf("seed row %s: not in the table\n", e.name); return 1; }
        if (k != portans::constant(k->slot) || k != portans::constant(k->slot - k->stub_delta)) { printf("seed row %s: lookup\n", e.name); return 1; }
        for (int n = 0; n < 2000; n++) {
            Cpu c = {};
            for (int i = 0; i < 32; i++) c.r[i] = rnd32();
            c.lr = rnd32();
            Cpu b = c;
            e.fn(&c);
            calls++;
            if (c.r[3] != k->r3) { printf("seed row %s: r3 port %08X table %08X\n", e.name, c.r[3], k->r3); return 1; }
            for (int i = 0; i < 32; i++) if (i != 3 && c.r[i] != b.r[i]) { printf("seed row %s: the port wrote r%d\n", e.name, i); return 1; }
        }
    }
    if (sizeof(portans::kConstants) / sizeof(portans::kConstants[0]) != sizeof(FNS) / sizeof(FNS[0])) { printf("seed the table has other rows\n"); return 1; }
    printf("portans_equiv: %zu rows, %ld calls: r3 and every other register as the port; 0 differences\n", sizeof(FNS) / sizeof(FNS[0]), calls);
    return 0;
}
"""
# the `portmem` program: the port's lines against mem_call on two copies of a random guest window
PRELUDE2 = r"""
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
typedef uint32_t uint32;
struct Fpr { double ps0; };
struct Cpu { uint32_t r[32]; uint32_t lr; Fpr f[32]; };
static const uint32_t kBase = 0x10000000u, kSize = 0x20000u;
static uint8_t* pm; static uint8_t* mm;
static uint8_t* at(uint8_t* m, uint32_t ea, uint32_t n) {
    if (ea - kBase >= kSize || ea - kBase + n > kSize) { printf("seed access %08X+%u outside the window\n", ea, n); exit(1); }
    return m + (ea - kBase);
}
inline uint32_t arg(Cpu* c, int i) { return c->r[3 + i]; }
inline uint64_t arg64(Cpu* c, int reg) { return ((uint64_t)c->r[reg] << 32) | c->r[reg + 1]; }
inline void ret(Cpu* c, uint32_t v) { c->r[3] = v; }
inline void ret64(Cpu* c, uint64_t v) { c->r[3] = (uint32_t)(v >> 32); c->r[4] = (uint32_t)v; }
static void st32(uint32_t a, uint32_t v) { uint8_t* p = at(pm, a, 4); p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; }
static void st64(uint32_t a, uint64_t v) { st32(a, (uint32_t)(v >> 32)); st32(a + 4, (uint32_t)v); }
static uint32_t ld32p(uint32_t a) { uint8_t* p = at(pm, a, 4); return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
/* cemu betype: the guest word through the struct's field */
struct Be32 { uint32_t raw; operator uint32_t() const { return __builtin_bswap32(raw); } Be32& operator=(uint32_t v) { raw = __builtin_bswap32(v); return *this; } };
struct BeF { uint32_t raw; BeF& operator=(float f) { uint32_t b; memcpy(&b, &f, 4); raw = __builtin_bswap32(b); return *this; } };
struct GX2Surface { uint8_t pad[SWIZZLE_OFF]; Be32 swizzle; };
namespace GX2 { struct GX2DepthBuffer { uint8_t pad[CLEARDEPTH_OFF]; BeF clearDepth; Be32 clearStencil; }; }
template <class T> static T* gp(uint32_t a) { return (T*)at(pm, a, 4); }
namespace mem { static uint8_t* ptr(uint32_t a) { return at(pm, a, 4); }
static std::string read_cstr(uint32_t ea) { if (!ea) return {}; std::string s; for (uint32_t a = ea;; a++) { uint8_t b = *at(pm, a, 1); if (!b) break; s.push_back((char)b); } return s; } }
static uint64_t g_now;
namespace timebase { static uint64_t guest_now() { return g_now; } }
static float farg(Cpu* c, int i) { return (float)c->f[1 + i].ps0; }
static const int32_t FS_NOT_FOUND = FSNOTFOUND;
/* open_file with no file on the host: read modes fail at fopen; the test never passes a write mode to the port side */
static int32_t open_file(const std::string&, const std::string&, uint32_t) { return FS_NOT_FOUND; }
#define HLE(lib, name) static void imp_##lib##_##name(Cpu* c)
"""
MAIN2 = r"""
#include "portans_model.h"
#include <stdexcept>
static uint64_t rs = 0x9E3779B97F4A7C15ull;
static uint32_t rnd32() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (uint32_t)(rs >> 11); }
struct Rej : std::runtime_error { using std::runtime_error::runtime_error; };
struct MEnv {
    uint8_t ld8(uint32_t a) { return *at(mm, a, 1); }
    uint32_t ld32(uint32_t a) { uint8_t* p = at(mm, a, 4); return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
    void st32(uint32_t a, uint32_t v) { uint8_t* p = at(mm, a, 4); p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; }
    void st64(uint32_t a, uint64_t v) { st32(a, (uint32_t)(v >> 32)); st32(a + 4, (uint32_t)v); }
    uint64_t now() { return g_now; }
    [[noreturn]] void reject(const char* w) { throw Rej(w); }
};
struct Row { uint32_t slot; void (*fn)(Cpu*); };
static const Row kRows[] = { {portans::GX2GPUTimeToCPUTime, imp_gx2_GX2GPUTimeToCPUTime}, {portans::GX2SampleTopGPUCycle, imp_gx2_GX2SampleTopGPUCycle},
    {portans::GX2SampleBottomGPUCycle, imp_gx2_GX2SampleBottomGPUCycle}, {portans::IMIsDimEnabled, imp_coreinit_IMIsDimEnabled},
    {portans::GX2SetSurfaceSwizzle, imp_gx2_GX2SetSurfaceSwizzle}, {portans::GX2SetClearDepthStencil, imp_gx2_GX2SetClearDepthStencil},
    {portans::GX2InitDepthBufferHiZEnable, imp_gx2_GX2InitDepthBufferHiZEnable}, {portans::SAVEOpenFile, imp_nn_save_SAVEOpenFile} };
int main() {
    pm = (uint8_t*)malloc(kSize); mm = (uint8_t*)malloc(kSize);
    for (uint32_t i = 0; i < kSize; i++) pm[i] = (uint8_t)rnd32();
    static const char* modes[] = {"r", "rb", "r+", "w", "wb", "a", ""};
    uint32_t mode_at[7];
    uint32_t p = kBase + 0x10;
    for (int i = 0; i < 7; i++) { mode_at[i] = p; strcpy((char*)at(pm, p, 4), modes[i]); p += 8; }
    memcpy(mm, pm, kSize);
    long calls = 0, rejects = 0;
    static const double fl[] = {0.0, 1.0, -1.0, 0.5, 1e300, 1e-300, 3.14159265358979};
    for (int n = 0; n < 200000; n++) {
        const Row& w = kRows[rnd32() % (sizeof kRows / sizeof kRows[0])];
        Cpu c = {};
        for (int i = 0; i < 32; i++) c.r[i] = rnd32();
        c.f[1].ps0 = rnd32() % 4 ? fl[rnd32() % 7] : (double)rnd32();
        uint32_t ptr = kBase + 0x100 + rnd32() % (kSize - 0x300);
        ptr &= rnd32() % 4 ? ~3u : ~0u;
        c.r[3] = rnd32() % 8 ? ptr : 0;
        if ((w.slot == portans::GX2SetSurfaceSwizzle || w.slot == portans::GX2SetClearDepthStencil) && !c.r[3]) c.r[3] = ptr;
        int md = rnd32() % 7;
        if (w.slot == portans::SAVEOpenFile) { c.r[7] = rnd32() % 10 ? mode_at[md] : 0; c.r[6] = 0; }
        g_now = ((uint64_t)rnd32() << 20) ^ rnd32();
        Cpu b = c;
        uint32_t a[11];
        for (int i = 0; i < 11; i++) a[i] = c.r[i];
        MEnv e;
        uint32_t r = 0;
        int k;
        const bool write_mode = w.slot == portans::SAVEOpenFile && c.r[7] && strpbrk(modes[md], "wa+");
        try { k = portans::mem_call(e, portans::mem_slot(w.slot), a, c.f[1].ps0, &r); }
        catch (const Rej&) {
            if (!write_mode) { printf("seed slot %08X: unexpected rejection\n", w.slot); return 1; }
            rejects++;
            continue;
        }
        if (write_mode) { printf("seed slot %08X mode %s: the model answered where it must reject\n", w.slot, modes[md]); return 1; }
        w.fn(&c);
        calls++;
        uint32_t mr3 = k ? r : a[3];
        if (mr3 != c.r[3]) { printf("seed slot %08X: r3 port %08X model %08X\n", w.slot, c.r[3], mr3); return 1; }
        for (int i = 0; i < 32; i++) if (i != 3 && c.r[i] != b.r[i]) { printf("seed slot %08X: the port wrote r%d\n", w.slot, i); return 1; }
        if (memcmp(pm, mm, kSize)) { uint32_t i = 0; while (pm[i] == mm[i]) i++; printf("seed slot %08X (r3 %08X r4 %08X): guest byte %08X port %02X model %02X\n", w.slot, b.r[3], b.r[4], kBase + i, pm[i], mm[i]); return 1; }
    }
    printf("portans_equiv: portmem: %ld calls, %ld expected write-mode rejections: r3, every other register and every guest byte as the port; 0 differences\n", calls, rejects);
    return 0;
}
"""
INJECT = [
    ("sample stores nothing", "if (a[3]) e.st64(a[3], e.now()); return 0;", "return 0;"),
    ("sample null check", "if (a[3]) e.st64(a[3], e.now()); return 0;", "if (a[3] & 4) e.st64(a[3], e.now()); return 0;"),
    ("IMIsDimEnabled stores 1", "if (a[3]) e.st32(a[3], 0); *r = 0; return 1;", "if (a[3]) e.st32(a[3], 1); *r = 0; return 1;"),
    ("IMIsDimEnabled answer", "if (a[3]) e.st32(a[3], 0); *r = 0; return 1;", "if (a[3]) e.st32(a[3], 0); *r = 1; return 1;"),
    ("swizzle offset", "e.st32(a[3] + 0x34, (e.ld32(a[3] + 0x34)", "e.st32(a[3] + 0x38, (e.ld32(a[3] + 0x38)"),
    ("swizzle mask", "& 0xFFFF00FFu) | ((a[4] & 0xFFu) << 8)", "& 0xFFFF0000u) | ((a[4] & 0xFFu) << 8)"),
    ("swizzle shift", "| ((a[4] & 0xFFu) << 8)", "| ((a[4] & 0xFFu) << 0)"),
    ("depth offset", "e.st32(a[3] + 0x88, bits);", "e.st32(a[3] + 0x84, bits);"),
    ("stencil mask", "e.st32(a[3] + 0x8C, a[4] & 0xFFu);", "e.st32(a[3] + 0x8C, a[4]);"),
    ("depth double", "const float d = (float)f1;", "const float d = (float)(f1 + 1.0);"),
    ("save answer", "*r = (uint32_t)kFsNotFound;", "*r = 0;"),
    ("save mode write accepted", "if (ch == 'w' || ch == 'a' || ch == '+')", "if (ch == 'a' || ch == '+')"),
    ("save mode + accepted", "if (ch == 'w' || ch == 'a' || ch == '+')", "if (ch == 'w' || ch == 'a')"),
    ("time answer changed", "default: return 0; /* GX2GPUTimeToCPUTime", "case GX2GPUTimeToCPUTime: *r = 0; return 1;\n    default: return 0; /*"),
    ("GX2DrawDone answer", '0xBD70BC88u, 1u, "gx2 GX2DrawDone"', '0xBD70BC88u, 0u, "gx2 GX2DrawDone"'),
    ("Connect answer", '0xA0000000u, "nn_ac Connect"', '0u, "nn_ac Connect"'),
    ("ac Initialize answer", '0u, "nn_ac Initialize"', '0xA0000000u, "nn_ac Initialize"'),
    ("olv Initialize answer", '0xA0000000u, "nn_olv Initialize"', '0u, "nn_olv Initialize"'),
    ("curl answer", '0u, "nlibcurl curl_global_init_mem"', '1u, "nlibcurl curl_global_init_mem"'),
    ("NSSL answer", '0u, "nsysnet NSSLInit"', '1u, "nsysnet NSSLInit"'),
    ("socket answer", '0u, "nsysnet socket_lib_init"', '1u, "nsysnet socket_lib_init"'),
    ("IMDisableDim answer", '0u, "coreinit IMDisableDim"', '1u, "coreinit IMDisableDim"'),
    ("HomeButton answer", '1u, "coreinit OSEnableHomeButtonMenu"', '0u, "coreinit OSEnableHomeButtonMenu"'),
    ("SharedData answer", '0u, "coreinit OSGetSharedData"', '1u, "coreinit OSGetSharedData"'),
    ("SetReportTypes answer", '0u, "nn_olv SetReportTypes"', '1u, "nn_olv SetReportTypes"'),
    ("SetWork answer", '0u, "nn_olv InitializeParam::SetWork"', '1u, "nn_olv InitializeParam::SetWork"'),
    ("ctor answer", '0u, "nn_olv InitializeParam ctor"', '1u, "nn_olv InitializeParam ctor"'),
    ("wrong gx2 delta", '0xBD70BC88u, 1u, "gx2 GX2DrawDone"', '0xBD70BD88u, 1u, "gx2 GX2DrawDone"'),
    ("wrong nn_ac delta", '0xBD70BD28u, 0u, "nn_ac Initialize"', '0xBD70BD88u, 0u, "nn_ac Initialize"'),
    ("wrong slot", '{0xC0009010u, 0xBD70BD88u, 0u, "coreinit IMDisableDim"}', '{0xC0009018u, 0xBD70BD88u, 0u, "coreinit IMDisableDim"}'),
    ("row dropped", '    {0xC0004E10u, 0xBD70BBF0u, 0u, "nn_olv SetReportTypes"},', '    /* dropped */'),
]


def measured_deltas():
    gen = os.path.join(ROOT, "build", "gen")
    imps = json.load(open(os.path.join(gen, "imports.json")))
    ident = {"imp_%s_%s" % (x["lib"].replace(".rpl", ""), x["name"]): x for x in imps}
    pat = re.compile(r"(imp_\w+)\(c\); /\* ([0-9A-F]{8}): ([0-9A-F]{8}) \*/")
    deltas = {}
    for f in glob.glob(os.path.join(gen, "code_*.c")):
        for m in pat.finditer(open(f).read()):
            x = ident.get(m.group(1))
            ins = int(m.group(3), 16)
            if not x or ins >> 26 != 18:
                continue
            off = ins & 0x03FFFFFC
            off -= 0x04000000 if off & 0x02000000 else 0
            deltas.setdefault(x["lib"], set()).add((x["slot"] - (int(m.group(2), 16) + off)) & 0xFFFFFFFF)
    return imps, deltas


def check_table(model, imps, deltas, rows):
    """python side of the table: slots and deltas against build/gen"""
    bad = []
    table = re.findall(r'\{0x([0-9A-F]{8})u, 0x([0-9A-F]{8})u, \w+, "([^"]+)"\}', model)
    if len(table) != len(rows):
        bad.append("table has %d rows, want %d" % (len(table), len(rows)))
    for slot, delta, name in table:
        lib, sym, _ = rows[name]
        hit = [x for x in imps if x["slot"] == int(slot, 16) and x["name"] == sym]
        if not hit:
            bad.append("%s: slot %s is not %s" % (name, slot, sym))
            continue
        d = deltas.get(hit[0]["lib"])
        if d and d != {int(delta, 16)}:
            bad.append("%s: stub delta %s, measured %s" % (name, delta, sorted("%08X" % v for v in d)))
    return bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port-src", default="runtime/src")
    ap.add_argument("--port-rev", default="f5554821")
    ap.add_argument("--inject", action="store_true")
    a = ap.parse_args()
    repo = os.path.dirname(os.path.dirname(os.path.abspath(a.port_src)))

    def port(name):
        return subprocess.run(["git", "-C", repo, "show", "%s:runtime/src/%s" % (a.port_rev, name)], capture_output=True, text=True, check=True).stdout
    alltext = subprocess.run(["git", "-C", repo, "grep", "-n", "HLE(", a.port_rev, "--", "runtime/src"], capture_output=True, text=True).stdout
    body, table = [], []
    for name, (lib, sym, f) in ROWS.items():
        ident = re.sub(r"\W", "_", name)
        if f is None:
            if sym in alltext:
                sys.exit("portans_equiv: %s has an HLE in the port now" % sym)
            continue
        m = re.search(r"^HLE\(%s, %s\) \{.*?^\}|^HLE\(%s, %s\) \{.*\}[^\n]*$" % (lib, sym, lib, sym), port(f), re.S | re.M)
        one = re.search(r"^HLE\(%s, %s\) \{[^\n]*\}[^\n]*$" % (lib, sym), port(f), re.M)
        text = one.group(0) if one else (m.group(0) if m else None)
        if not text:
            sys.exit("portans_equiv: HLE(%s, %s) not found in %s" % (lib, sym, f))
        body.append(text)
        table.append('{"%s", imp_%s_%s},' % (name, lib, sym))
    core = port("core.cpp")
    m = re.search(r'extern "C" void hle_unimplemented\(Cpu\* c, const char\* lib, const char\* name\) \{.*?\n\}', core, re.S)
    if not m or "c->r[3] = 0;" not in m.group(0):
        sys.exit("portans_equiv: core.cpp hle_unimplemented not found")
    body.append(m.group(0))
    for name, (lib, sym, f) in ROWS.items():
        if f is None:
            fn = "unimpl_" + re.sub(r"\W", "_", sym)
            body.append('static void %s(Cpu* c) { hle_unimplemented(c, "%s", "%s"); }' % (fn, lib, sym))
            table.append('{"%s", %s},' % (name, fn))
    src = PRELUDE + "\n".join(body) + "\nstruct Fn { const char* name; void (*fn)(Cpu*); };\nstatic const Fn FNS[] = {\n" + "\n".join(table) + "\n};\n" + MAIN
    import verify  # noqa: E402
    d = tempfile.mkdtemp(prefix="portans_equiv-", dir=verify.verify_scratch_base() or None)
    cpp = os.path.join(d, "equiv.cpp")
    open(cpp, "w").write(src)
    # the `portmem` program from the port's text; offsets from the port's struct headers
    core_gx2 = port("gx2/gx2_core.cpp")
    resources = port("gx2/gx2_resources.cpp")
    misc = port("hle/coreinit_misc.cpp")
    fs = port("hle/fs.cpp")
    surf = subprocess.run(["git", "-C", repo, "show", a.port_rev + ":runtime/third_party/cemu/Cafe/OS/libs/gx2/GX2_Surface.h"], capture_output=True, text=True, check=True).stdout
    regs = subprocess.run(["git", "-C", repo, "show", a.port_rev + ":runtime/third_party/cemu/gx2_texture_regs.h"], capture_output=True, text=True, check=True).stdout
    sw_off = re.search(r"\+0x([0-9A-Fa-f]+) \*/ uint32be swizzle;", surf).group(1)
    cd_off = re.search(r"\+0x([0-9A-Fa-f]+) \*/ float32be clearDepth;", regs).group(1)
    cs_off = re.search(r"\+0x([0-9A-Fa-f]+) \*/ uint32be clearStencil;", regs).group(1)
    if int(cs_off, 16) != int(cd_off, 16) + 4:
        sys.exit("portans_equiv: clearStencil no longer follows clearDepth")
    fsnf = re.search(r"FS_NOT_FOUND\s*=\s*(-?\d+)", fs)
    if not fsnf or "if (!f) return FS_NOT_FOUND;" not in fs or "FILE* f = fopen(hp.c_str(), m.c_str());" not in fs:
        sys.exit("portans_equiv: fs.cpp open_file no longer answers a missing file with FS_NOT_FOUND")
    save = re.search(r"^static std::string save_path\(.*?^\}", fs, re.S | re.M).group(0)

    def cut(text, lib, sym):
        m1 = re.search(r"^HLE\(%s, %s\) \{[^\n]*\}[^\n]*$" % (lib, sym), text, re.M) or re.search(r"^HLE\(%s, %s\) \{.*?^\}" % (lib, sym), text, re.S | re.M)
        if not m1:
            sys.exit("portans_equiv: HLE(%s, %s) not found" % (lib, sym))
        return m1.group(0)
    lines = [cut(core_gx2, "gx2", "GX2GPUTimeToCPUTime"), cut(core_gx2, "gx2", "GX2SampleTopGPUCycle"), cut(core_gx2, "gx2", "GX2SampleBottomGPUCycle"),
             cut(core_gx2, "gx2", "GX2SetClearDepthStencil"), cut(resources, "gx2", "GX2SetSurfaceSwizzle"), cut(resources, "gx2", "GX2InitDepthBufferHiZEnable"),
             cut(misc, "coreinit", "IMIsDimEnabled"), save, cut(fs, "nn_save", "SAVEOpenFile")]
    src2 = PRELUDE2.replace("SWIZZLE_OFF", "0x" + sw_off).replace("CLEARDEPTH_OFF", "0x" + cd_off).replace("FSNOTFOUND", fsnf.group(1)) + "\n".join(lines) + "\n" + MAIN2
    cpp2 = os.path.join(d, "equiv2.cpp")
    open(cpp2, "w").write(src2)
    imps, deltas = measured_deltas()
    model = open(os.path.join(HERE, "src", "portans_model.h")).read()

    def run(mdir, text):
        bad = check_table(text, imps, deltas, ROWS)
        exe = os.path.join(d, os.path.basename(mdir) + "_equiv")
        r = subprocess.run(["clang++", "-std=c++20", "-O1", "-w", "-I" + mdir, cpp, "-o", exe], capture_output=True, text=True, errors="replace")
        if r.returncode:
            return bad + ["build: " + r.stderr[-1500:]], ""
        r = subprocess.run([exe], capture_output=True, text=True, errors="replace")
        if r.returncode:
            bad.append(r.stdout.strip()[-300:] or "exit %d" % r.returncode)
        out = r.stdout.strip()
        exe2 = exe + "2"
        r = subprocess.run(["clang++", "-std=c++20", "-O1", "-w", "-I" + mdir, cpp2, "-o", exe2], capture_output=True, text=True, errors="replace")
        if r.returncode:
            return bad + ["build2: " + r.stderr[-1500:]], out
        r = subprocess.run([exe2], capture_output=True, text=True, errors="replace")
        if r.returncode:
            bad.append(r.stdout.strip()[-300:] or "exit %d" % r.returncode)
        return bad, out + "\n" + r.stdout.strip()
    bad, out = run(os.path.join(HERE, "src"), model)
    print(out)
    print("portans_equiv: %s (port at %s; programs in %s)%s" % ("PASS" if not bad else "FAIL", a.port_rev, d, "".join("\n  " + b for b in bad)))
    if not a.inject:
        return 1 if bad else 0
    missed = 0
    for i, (what, old, new) in enumerate(INJECT):
        if model.count(old) != 1:
            print("portans_equiv: inject %d (%s): text occurs %d times" % (i, what, model.count(old)))
            missed += 1
            continue
        md = os.path.join(d, "inject%02d" % i)
        os.makedirs(md, exist_ok=True)
        t = model.replace(old, new)
        open(os.path.join(md, "portans_model.h"), "w").write(t)
        b, _ = run(md, t)
        print("portans_equiv: inject %2d %-24s %s" % (i, what, "detected: " + b[0][:110] if b else "NOT DETECTED"))
        missed += 0 if b else 1
    print("portans_equiv: injected bugs: %d of %d detected" % (len(INJECT) - missed, len(INJECT)))
    return 1 if bad or missed else 0


if __name__ == "__main__":
    sys.exit(main())

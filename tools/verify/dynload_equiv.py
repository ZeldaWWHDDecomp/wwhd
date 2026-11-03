#!/usr/bin/env python3
"""dynload_equiv.py: the `osmem ... act` / `osmem ... dynload` model (tools/verify/src/dynload_model.h) against the
shipping port's own HLE, read at port commit f5554821 (--port-rev; '' = the working tree):
  - runtime/src/hle/system_stubs.cpp, the "nn_act (accounts)" section (the five nn::act imports);
  - runtime/src/hle/coreinit_misc.cpp, the "OSDynLoad" section (Acquire, Release, SetAllocator, FindExport with its
    logging stub for unimplemented exports);
  - runtime/src/hle/erreula.cpp, whole;
  - runtime/src/hle/system_stubs.cpp, the ProcUIProcessMessages line, for `procui`: r3 and every other register per sequence;
  - runtime/src/hle/swkbd.cpp, whole, in its own translation unit (shims: text_entry, input::prompt_text, guest_call and
    BlockingScope: no export the model answers reaches them), for the `swkbd` word: the exports the J3D route reaches
 must answer like the port and every other one must be a model rejection at
    the call; without the word a function export of module swkbd is a rejection at FindExport, as before;
  - runtime/src/core.cpp hle_find (compiled from the port's text over a registry the HLE macro fills, as runtime.h's
    HLE / HleReg do), and runtime.h kHleFuncBase.
Shims stand for what the model replaces: dispatch::register_host = core.cpp's (the next 8-byte address from
kHleFuncBase, after the boot's three MEMAllocFromDefaultHeap/Ex / MEMFreeToDefaultHeap registrations) with a lookup
table for the calls; mem::runtime_alloc = a zeroed bump allocator from 0x6FF00000, align 16 (the verifier's fixture
region; the model's runtime_alloc is the same rule); std::chrono::steady_clock in erreula.cpp = a clock with a period
of 1/30 s that the test advances by one tick right before each ErrEulaCalc call (one Calc per 30 Hz frame: the model's
time unit); LOG / TRACE / log_msg print nothing (their arguments are not evaluated, so ErrEulaAppearError's log reads
are left out on both sides).

Each sequence (its own process: the HLE's statics start fresh) runs a random mix on both sides, each side on its own
copy of the same random guest memory (0x6FE00000..0x70000000): nn::act calls, OSDynLoad_Acquire (names from a pool
with and without ".rpl", "" and null), FindExport (known handles, unknown ones; erreula's 20 symbols, the two the game
does not use, unknown symbols, ""; functions and data), Release / SetAllocator, and calls through every handed-out
address (erreula exports and logging stubs) with Calc frequent enough to reach every state and the confirm. After
every call r3 (unchanged for void functions), r0..r31 (no other register written on either side) and every byte of
the window must be equal, and so must erreula.cpp's statics (state, decided, HOME sign; read through a probe compiled
into the port's translation unit) and the model's. A function export of a named module other than erreula must be a model rejection (the port
answers it from its own registry); the sequence ends there, as the verifier's input does.

--inject: the self-check: each of a list of single-line bugs injected into a copy of dynload_model.h must make the
comparison FAIL (exit 0 only when every injected bug is detected and the clean model passes).
Exit 0 on PASS."""
import argparse
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

PRELUDE = r"""
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
struct Cpu { uint32_t r[32]; uint32_t lr; };
typedef void (*PpcFunc)(Cpu*);
namespace eq {
constexpr uint32_t kBase = 0x6FE00000u, kSize = 0x200000u;
extern uint8_t* port_mem;
inline uint8_t* at(uint8_t* m, uint32_t ea, uint32_t n) {
    if (ea - kBase >= kSize || ea - kBase + n > kSize) { fprintf(stderr, "access %08X+%u outside the window\n", ea, n); exit(3); }
    return m + (ea - kBase);
}
extern int64_t frames;
}
inline uint8_t ld8(uint32_t a) { return *eq::at(eq::port_mem, a, 1); }
inline uint16_t ld16(uint32_t a) { uint8_t* p = eq::at(eq::port_mem, a, 2); return (uint16_t)(p[0] << 8 | p[1]); }
inline uint32_t ld32(uint32_t a) { uint8_t* p = eq::at(eq::port_mem, a, 4); return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
inline void st8(uint32_t a, uint8_t v) { *eq::at(eq::port_mem, a, 1) = v; }
inline void st32(uint32_t a, uint32_t v) { uint8_t* p = eq::at(eq::port_mem, a, 4); p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; }
inline uint32_t arg(Cpu* c, int i) { return c->r[3 + i]; }
inline void ret(Cpu* c, uint32_t v) { c->r[3] = v; }
#define LOG(...) do {} while (0)
#define TRACE(...) do {} while (0)
static inline void log_msg(const char*, ...) {}
namespace mem {
inline std::string read_cstr(uint32_t ea) { /* core.cpp: null -> "", else the bytes up to the terminator */
    if (!ea) return {};
    std::string s;
    for (uint32_t a = ea;; a++) { uint8_t b = ld8(a); if (!b) break; s.push_back((char)b); }
    return s;
}
uint32_t runtime_alloc(uint32_t size, uint32_t align = 16);
}
namespace dispatch { uint32_t register_host(PpcFunc fn, const char* name); PpcFunc lookup(uint32_t a); }
/* runtime.h HLE registration */
struct HleReg { const char* lib; const char* name; PpcFunc fn; HleReg(const char* l, const char* n, PpcFunc f); };
std::vector<HleReg*>& hle_registry();
PpcFunc hle_find(const char* lib, const char* name);
#define HLE(lib, name)                                                          \
    static void hle_impl_##lib##_##name(Cpu* c);                                 \
    extern "C" void imp_##lib##_##name(Cpu* c) { hle_impl_##lib##_##name(c); }  \
    static HleReg hle_reg_##lib##_##name(#lib, #name, imp_##lib##_##name);       \
    static void hle_impl_##lib##_##name(Cpu* c)
/* erreula.cpp's steady_clock: 1/30 s ticks, advanced by the test before each ErrEulaCalc */
struct eq_clock {
    using rep = int64_t;
    using period = std::ratio<1, 30>;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<eq_clock>;
    static constexpr bool is_steady = true;
    static time_point now() { return time_point(duration(eq::frames)); }
};
"""

# the port's swkbd.cpp translation unit: what it needs beyond PRELUDE, and a probe of its statics after the file
SWKBD_PRELUDE = r"""
#include <algorithm>
#include <atomic>
#include <initializer_list>
#include <thread>
inline void st16(uint32_t a, uint16_t v) { uint8_t* p = eq::at(eq::port_mem, a, 2); p[0] = v >> 8; p[1] = (uint8_t)v; }
namespace mem { inline uint8_t* ptr(uint32_t ea) { return eq::at(eq::port_mem, ea, 1); } }
struct BlockingScope {};
[[noreturn]] static inline void eq_unreached(const char* w) { fprintf(stderr, "%s reached\n", w); exit(3); }
inline uint32_t guest_call(Cpu*, uint32_t, std::initializer_list<uint32_t>) { eq_unreached("guest_call"); }
namespace input { template <class F> void prompt_text(const std::u16string&, int, F) { eq_unreached("input::prompt_text"); } }
namespace text_entry {
struct Request { std::u16string initial, hint; int max_len = 0, mode = 0, language = 0; void (*changed)(const std::u16string&) = nullptr; };
template <class F> bool start(const Request&, F) { eq_unreached("text_entry::start"); }
inline void dismiss() {}
}
"""
SWKBD_PROBE = r"""
extern "C" uint32_t eq_swkbd_active() { return S.active; }
extern "C" uint32_t eq_swkbd_form_buf() { return S.form_buf; }
"""

SUPPORT = r"""
// ---- shims: the registry, register_host (core.cpp), runtime_alloc
uint8_t* eq::port_mem;
int64_t eq::frames = 0;
std::vector<HleReg*>& hle_registry() { static std::vector<HleReg*> r; return r; }
HleReg::HleReg(const char* l, const char* n, PpcFunc f) : lib(l), name(n), fn(f) { hle_registry().push_back(this); }
static uint32_t g_next_host = KHLEFUNCBASE;
static std::map<uint32_t, PpcFunc> g_host;
static void boot_host(Cpu*) {}
uint32_t dispatch::register_host(PpcFunc fn, const char*) { uint32_t a = g_next_host; g_next_host += 8; g_host[a] = fn; return a; }
PpcFunc dispatch::lookup(uint32_t a) { auto it = g_host.find(a); return it == g_host.end() ? nullptr : it->second; }
static uint32_t g_bump = 0x6FF00000u;
uint32_t mem::runtime_alloc(uint32_t size, uint32_t align) {
    uint32_t p = (g_bump + align - 1) & ~(align - 1);
    g_bump = p + size;
    memset(eq::at(eq::port_mem, p, size), 0, size);
    return p;
}
void eq_boot() { /* coreinit_mem.cpp mem_init_data_imports: the boot's three default-heap host functions come first */
    for (int i = 0; i < 3; i++) dispatch::register_host(boot_host, "boot");
}
"""

# after erreula.cpp in the same translation unit: its statics (state, decided, HOME sign) for the per-call state check
PROBE = r"""
extern "C" uint32_t eq_erreula_state() { return (uint32_t)g_state | (uint32_t)g_decided << 8 | (uint32_t)g_home_nix << 9; }
"""

PROG = r"""
#include "dynload_model.h"
#include <sys/wait.h>
#include <unistd.h>
#include <stdexcept>

void eq_boot();
extern "C" uint32_t eq_erreula_state();
extern "C" uint32_t eq_swkbd_active();
extern "C" uint32_t eq_swkbd_form_buf();
/* the exports the route reaches (by need): the test's own list; a call to any other must be a model rejection */
static const int kSwkbdAnswered[] = {dlm::S_Create};
static bool sw_answered(int i) { for (int k : kSwkbdAnswered) if (k == i) return true; return false; }
static uint8_t* model_mem;
static uint64_t rs;
static uint32_t rnd32() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (uint32_t)(rs >> 11); }
static uint32_t rnd(uint32_t n) { return n ? rnd32() % n : 0; }

struct Reject : std::runtime_error { using std::runtime_error::runtime_error; };
static uint32_t m_bump = 0x6FF00000u;
struct MEnv {
    uint8_t ld8(uint32_t a) { return *eq::at(model_mem, a, 1); }
    void st8(uint32_t a, uint32_t v) { *eq::at(model_mem, a, 1) = (uint8_t)v; }
    void st32(uint32_t a, uint32_t v) { uint8_t* p = eq::at(model_mem, a, 4); p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; }
    uint32_t runtime_alloc(uint32_t size) {
        uint32_t p = (m_bump + 15) & ~15u;
        m_bump = p + size;
        memset(eq::at(model_mem, p, size), 0, size);
        return p;
    }
    [[noreturn]] void reject(const char* why) { throw Reject(why); }
};

#define PORT_IMPORTS(X) X(nn_act, Initialize__Q2_2nn3actFv, dlm::ActInitialize) X(nn_act, Finalize__Q2_2nn3actFv, dlm::ActFinalize) \
    X(nn_act, GetSlotNo__Q2_2nn3actFv, dlm::ActGetSlotNo) X(nn_act, GetPrincipalId__Q2_2nn3actFv, dlm::ActGetPrincipalId) \
    X(nn_act, GetParentalControlSlotNoEx__Q2_2nn3actFPUcUc, dlm::ActGetParentalControlSlotNoEx) \
    X(coreinit, OSDynLoad_Acquire, dlm::DynAcquire) X(coreinit, OSDynLoad_Release, dlm::DynRelease) \
    X(coreinit, OSDynLoad_SetAllocator, dlm::DynSetAllocator) X(coreinit, OSDynLoad_FindExport, dlm::DynFindExport)
#define DECL(l, n, s) extern "C" void imp_##l##_##n(Cpu* c);
PORT_IMPORTS(DECL)
static bool port_call(uint32_t t, Cpu* c) {
#define CASE(l, n, s) if (t == s) { imp_##l##_##n(c); return true; }
    PORT_IMPORTS(CASE)
    if (PpcFunc f = dispatch::lookup(t)) { f(c); return true; }
    return false;
}

static const char* kModules[] = {"erreula.rpl", "erreula", "swkbd.rpl", "swkbd", ".rpl", "x.rpl", "nn_act.rpl", "coreinit.rpl", "", "erreula.rp", "rpl"};
static std::vector<std::string> sym_pool() {
    std::vector<std::string> v;
    for (int i = 0; i < dlm::E_COUNT; i++) v.push_back(dlm::kErreulaNames[i]);
    for (int i = 0; i < dlm::S_COUNT; i++) v.push_back(dlm::kSwkbdNames[i]);
    v.push_back("SwkbdUnknownThing__3RplFv");
    v.push_back("ErrEulaUnknownThing__3RplFv");
    v.push_back("");
    v.push_back("OSDynLoad_Acquire");
    v.push_back("Initialize__Q2_2nn3actFv");
    return v;
}

struct Stats { long calls = 0, acquires = 0, finds = 0, data = 0, stubs = 0, exp_calls = 0, calcs = 0, visible = 0, decided = 0, rejects = 0, home = 0, sw_creates = 0, sw_rejects = 0; } st;

extern "C" void imp_proc_ui_ProcUIProcessMessages(Cpu* c);
static int run_seq(uint64_t seed, int len) {
    rs = seed * 0x9E3779B97F4A7C15ull + 1;
    for (int i = 0; i < 8; i++) rnd32();
    /* `procui`: the port's answer whatever the registers, r3 only */
    for (int i = 0; i < 20; i++) {
        Cpu c = {};
        for (int j = 0; j < 32; j++) c.r[j] = rnd32();
        Cpu before = c;
        imp_proc_ui_ProcUIProcessMessages(&c);
        for (int j = 0; j < 32; j++)
            if (j != 3 && c.r[j] != before.r[j]) { printf("seed %llu: ProcUIProcessMessages: the port wrote r%d\n", (unsigned long long)seed, j); return 1; }
        if (c.r[3] != dlm::kProcUIForeground) { printf("seed %llu: ProcUIProcessMessages: r3 port %08X model %08X\n", (unsigned long long)seed, c.r[3], dlm::kProcUIForeground); return 1; }
    }
    eq::port_mem = (uint8_t*)malloc(eq::kSize);
    model_mem = (uint8_t*)malloc(eq::kSize);
    for (uint32_t i = 0; i < eq::kSize; i++) eq::port_mem[i] = (uint8_t)rnd32();
    /* the strings: module and symbol names in a string area */
    uint32_t str_at = eq::kBase + 0x1000;
    auto put_str = [&](const std::string& s) {
        uint32_t a = str_at;
        memcpy(eq::at(eq::port_mem, a, (uint32_t)s.size() + 1), s.c_str(), s.size() + 1);
        str_at += (uint32_t)s.size() + 1 + rnd(4);
        return a;
    };
    std::vector<uint32_t> mods, syms;
    for (const char* m : kModules) mods.push_back(put_str(m));
    std::vector<std::string> sp = sym_pool();
    for (auto& s : sp) syms.push_back(put_str(s));
    memcpy(model_mem, eq::port_mem, eq::kSize);
    eq_boot();
    dlm::State S;
    MEnv e;
    std::vector<uint32_t> handles, addrs;
    auto out_ptr = [&]() { uint32_t a = eq::kBase + 0x10000 + rnd(0xE0000 - 0x10000 - 8); return rnd(8) ? a & ~3u : a; };
    /* most sequences start like the game: Acquire("erreula.rpl"), then FindExport of its functions */
    const bool swk = rnd(4) != 0; /* the `swkbd` word */
    std::vector<std::pair<int, uint32_t>> script;
    int start = rnd(4);
    if (start == 0 || start == 1) {
        script.push_back({0, mods[0]});
        for (int i = 0; i < dlm::E_COUNT; i++) if (rnd(8)) script.push_back({1, syms[i]});
    }
    if (!swk && start == 3) { /* without the word the loader's first name is a rejection, as before */
        script.push_back({0, mods[2]});
        script.push_back({1, syms[dlm::E_COUNT]});
    }
    if (swk && (start == 1 || start == 2)) { /* the game's loader 0286CD7C: every name */
        script.push_back({0, mods[2]});
        for (int i = 0; i < dlm::S_COUNT; i++) script.push_back({1, syms[dlm::E_COUNT + i]});
    }
    size_t sk = 0;
    for (int op = 0; op < len; op++) {
        uint32_t a[11];
        for (int i = 0; i < 11; i++) a[i] = rnd32();
        uint32_t t;
        int kind = sk < script.size() ? script[sk].first : -1;
        if (kind == 0) { t = dlm::DynAcquire; a[3] = script[sk].second; a[4] = out_ptr(); }
        else if (kind == 1) { t = dlm::DynFindExport; a[3] = handles.back(); a[4] = 0; a[5] = script[sk].second; a[6] = out_ptr(); }
        else {
            uint32_t k = rnd(100);
            if (k < 5) { static const uint32_t act[] = {dlm::ActInitialize, dlm::ActFinalize, dlm::ActGetSlotNo, dlm::ActGetPrincipalId, dlm::ActGetParentalControlSlotNoEx};
                         t = act[rnd(5)]; if (t == dlm::ActGetParentalControlSlotNoEx) a[3] = out_ptr() + rnd(4); }
            else if (k < 9) { t = dlm::DynAcquire; a[3] = rnd(12) ? mods[rnd(mods.size())] : 0; a[4] = out_ptr(); }
            else if (k < 18) {
                t = dlm::DynFindExport;
                a[3] = handles.size() && rnd(6) ? handles[rnd(handles.size())] : rnd32();
                a[4] = rnd(6) ? 0 : rnd(3) ? 1 : rnd32();
                a[5] = rnd(15) ? syms[rnd(syms.size())] : 0;
                a[6] = out_ptr();
                /* a function of another named module is a model rejection that ends the sequence: rarely */
                auto m = S.modules.find(a[3]);
                if (!a[4] && m != S.modules.end() && m->second != "erreula" && !(swk && m->second == "swkbd") && rnd(50)) a[4] = 1;
            } else if (k < 20) t = rnd(2) ? dlm::DynRelease : dlm::DynSetAllocator;
            else if (k < 22) t = dlm::kHleFuncBase + 8 * (dlm::kFirstHost + rnd(40)); /* maybe not handed out yet */
            else if (addrs.empty()) t = dlm::DynSetAllocator;
            else {
                /* an export call: Calc often, so the fades and the confirm are reached */
                int calc = -1;
                for (size_t i = 0; i < addrs.size(); i++) if (S.host_index(addrs[i]) >= 0 && S.hosts[S.host_index(addrs[i])] == dlm::E_Calc) calc = (int)i;
                t = calc >= 0 && rnd(100) < 70 ? addrs[calc] : addrs[rnd(addrs.size())];
                /* a swkbd export outside the route ends the sequence (rejection): rarely */
                int h = S.host_index(t);
                if (h >= 0 && S.hosts[h] >= dlm::kSwkbdBase && S.hosts[h] != dlm::kStub && !sw_answered(S.hosts[h] - dlm::kSwkbdBase) && rnd(100))
                    for (uint32_t x : addrs) { int g = S.host_index(x); if (g >= 0 && S.hosts[g] >= dlm::kSwkbdBase && S.hosts[g] != dlm::kStub && sw_answered(S.hosts[g] - dlm::kSwkbdBase) && rnd(3) == 0) { t = x; break; } }
                h = S.host_index(t);
                if (h >= 0 && S.hosts[h] >= dlm::kSwkbdBase && S.hosts[h] != dlm::kStub && !sw_answered(S.hosts[h] - dlm::kSwkbdBase) && rnd(100)) t = dlm::DynSetAllocator;
            }
        }
        if (kind >= 0) sk++;
        Cpu c = {};
        for (int i = 0; i < 11; i++) c.r[i] = a[i];
        for (int i = 11; i < 32; i++) c.r[i] = rnd32();
        c.lr = rnd32();
        Cpu before = c;
        int hk = S.host_index(t);
        bool is_calc = hk >= 0 && S.hosts[hk] == dlm::E_Calc;
        if (is_calc) eq::frames++; /* one frame passes before each Calc */
        uint32_t r = 0;
        int mk;
        size_t nh = S.hosts.size();
        /* where the model must fail closed: a function export of another named module (swkbd counts as other without the
         * word) that is new, and a call to a swkbd export outside the route */
        auto findm = S.modules.find(a[3]);
        std::string fsym = t == dlm::DynFindExport ? dlm::read_cstr(e, a[5]) : std::string();
        const bool must_find = t == dlm::DynFindExport && findm != S.modules.end() && findm->second != "erreula" &&
                               !(swk && findm->second == "swkbd") && !a[4] && !S.exports.count(findm->second + "." + fsym);
        const bool must_call = hk >= 0 && S.hosts[hk] >= dlm::kSwkbdBase && S.hosts[hk] < dlm::kSwkbdBase + dlm::S_COUNT && !sw_answered(S.hosts[hk] - dlm::kSwkbdBase);
        try {
            mk = dlm::call(e, S, true, true, swk, t, a, &r);
        } catch (const Reject& x) {
            if (must_find) { st.rejects++; break; }
            if (must_call) { st.sw_rejects++; break; }
            printf("seed %llu op %d %s: unexpected model rejection: %s\n", (unsigned long long)seed, op, dlm::import_name(t), x.what());
            return 1;
        }
        if (must_find || must_call) {
            printf("seed %llu op %d %08X (word %d): the model answered where it must reject (%s)\n", (unsigned long long)seed, op, t, (int)swk,
                   must_find ? "FindExport" : dlm::kSwkbdNames[S.hosts[hk] - dlm::kSwkbdBase]);
            return 1;
        }
        bool pk = port_call(t, &c);
        st.calls++;
        if (pk != (mk >= 0)) {
            printf("seed %llu op %d target %08X: port %s, model %s\n", (unsigned long long)seed, op, t, pk ? "answers" : "has no function",
                   mk >= 0 ? "answers" : "does not model it");
            return 1;
        }
        if (!pk) continue;
        uint32_t mr3 = mk == 1 ? r : a[3];
        if (mr3 != c.r[3]) {
            printf("seed %llu op %d %s %08X (%s): r3 port %08X model %08X (args %08X %08X %08X %08X)\n", (unsigned long long)seed, op,
                   dlm::import_name(t), t, hk >= 0 && S.hosts[hk] < dlm::E_COUNT ? dlm::kErreulaNames[S.hosts[hk]] : hk >= 0 && S.hosts[hk] >= dlm::kSwkbdBase && S.hosts[hk] != dlm::kStub ? dlm::kSwkbdNames[S.hosts[hk] - dlm::kSwkbdBase] : "", c.r[3], mr3, a[3], a[4], a[5], a[6]);
            return 1;
        }
        for (int i = 0; i < 32; i++)
            if (i != 3 && c.r[i] != before.r[i]) { printf("seed %llu op %d %08X: the port wrote r%d\n", (unsigned long long)seed, op, t, i); return 1; }
        if (memcmp(eq::port_mem, model_mem, eq::kSize)) {
            uint32_t i = 0;
            while (eq::port_mem[i] == model_mem[i]) i++;
            printf("seed %llu op %d %s %08X (args %08X %08X %08X %08X): guest byte %08X port %02X model %02X\n", (unsigned long long)seed, op,
                   dlm::import_name(t), t, a[3], a[4], a[5], a[6], eq::kBase + i, eq::port_mem[i], model_mem[i]);
            return 1;
        }
        /* the erreula statics after every call (the state machine, not only what the game happens to read back) */
        uint32_t ms = S.err_state | (uint32_t)S.err_decided << 8 | (uint32_t)S.err_home_nix << 9;
        if (ms != eq_erreula_state()) {
            printf("seed %llu op %d %08X: erreula state port %03X model %03X (state | decided << 8 | home << 9)\n", (unsigned long long)seed, op, t,
                   eq_erreula_state(), ms);
            return 1;
        }
        if (eq_swkbd_active() != 0 || eq_swkbd_form_buf() != S.sw_form_buf) {
            printf("seed %llu op %d %08X: swkbd statics: active port %u, form_buf port %08X model %08X\n", (unsigned long long)seed, op, t,
                   eq_swkbd_active(), eq_swkbd_form_buf(), S.sw_form_buf);
            return 1;
        }
        if (hk >= 0 && S.hosts[hk] == dlm::kSwkbdBase + dlm::S_Create) st.sw_creates++;
        if (t == dlm::DynAcquire) { st.acquires++; handles.push_back(ld32(a[4])); }
        if (t == dlm::DynFindExport) {
            st.finds++;
            uint32_t v = ld32(a[6]);
            if (a[4]) st.data++;
            else if (S.host_index(v) >= 0) {
                addrs.push_back(v);
                if (S.hosts.size() != nh && S.hosts.back() == dlm::kStub) st.stubs++;
            }
        }
        if (hk >= 0) {
            st.exp_calls++;
            if (is_calc) st.calcs++;
            if (S.hosts[hk] == dlm::E_GetStateErrorViewer && mr3 == dlm::kVisible) st.visible++;
            if ((S.hosts[hk] == dlm::E_IsDecideSelectButtonError || S.hosts[hk] == dlm::E_GetResultType) && mr3) st.decided++;
            if (S.hosts[hk] == dlm::E_IsAppearHomeNixSign && mr3) st.home++;
        }
    }
    printf("STATS %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld\n", st.calls, st.acquires, st.finds, st.data, st.stubs, st.exp_calls, st.calcs,
           st.visible, st.decided, st.rejects, st.home, st.sw_creates, st.sw_rejects);
    return 0;
}

int main(int argc, char** argv) {
    int seqs = argc > 1 ? atoi(argv[1]) : 200, len = argc > 2 ? atoi(argv[2]) : 400;
    uint64_t base = argc > 3 ? strtoull(argv[3], nullptr, 0) : 1;
    int fails = 0;
    long tot[13] = {};
    for (int q = 0; q < seqs; q++) {
        int fd[2];
        if (pipe(fd)) return 3;
        pid_t pid = fork();
        if (pid == 0) {
            dup2(fd[1], 1);
            close(fd[0]);
            int rc = run_seq(base + q, len);
            fflush(stdout);
            _exit(rc);
        }
        close(fd[1]);
        std::string out;
        char b[4096];
        ssize_t n;
        while ((n = read(fd[0], b, sizeof b)) > 0) out.append(b, n);
        close(fd[0]);
        int ws = 0;
        waitpid(pid, &ws, 0);
        long v[13];
        if (WIFEXITED(ws) && WEXITSTATUS(ws) == 0 &&
            sscanf(out.c_str(), "STATS %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], &v[9], &v[10], &v[11], &v[12]) == 13) {
            for (int i = 0; i < 13; i++) tot[i] += v[i];
        } else {
            fails++;
            printf("%s", out.c_str());
            if (!WIFEXITED(ws) || WEXITSTATUS(ws) != 1) printf("seed %llu: the sequence ended abnormally (status %d)\n", (unsigned long long)(base + q), ws);
            if (fails >= 3) break;
        }
    }
    printf("dynload_equiv: %d sequences, %ld calls: %ld Acquire, %ld FindExport (%ld data, %ld logging stubs), %ld export calls (%ld Calc), "
           "%ld visible states read, %ld confirmed reads, %ld HOME-sign reads, %ld expected FindExport rejections, %ld SwkbdCreate, "
           "%ld expected swkbd call rejections; %d difference(s)\n",
           seqs, tot[0], tot[1], tot[2], tot[3], tot[4], tot[5], tot[6], tot[7], tot[8], tot[10], tot[9], tot[11], tot[12], fails);
    return fails ? 1 : 0;
}
"""

# single-line bugs for --inject: (description, old text in dynload_model.h, new text); each must be detected
INJECT = [
    ("GetSlotNo answer", "case ActGetSlotNo: *r = 1; return 1;", "case ActGetSlotNo: *r = 0; return 1;"),
    ("Initialize answer", "case ActInitialize: case ActFinalize: case ActGetPrincipalId: *r = 0; return 1;",
     "case ActInitialize: case ActFinalize: case ActGetPrincipalId: *r = 1; return 1;"),
    ("parental slot byte", "case ActGetParentalControlSlotNoEx: e.st8(a[3], 1); *r = 0; return 1;",
     "case ActGetParentalControlSlotNoEx: e.st8(a[3], 0); *r = 0; return 1;"),
    (".rpl strip length", "if (name.size() > 4 && name.substr(name.size() - 4) == \".rpl\")", "if (name.size() >= 4 && name.substr(name.size() - 4) == \".rpl\")"),
    ("handle mask", "(std::hash<std::string>()(name) & 0x0FFFFFFF)", "(std::hash<std::string>()(name) & 0x07FFFFFF)"),
    ("handle tag", "uint32_t h = 0x70000000u |", "uint32_t h = 0x60000000u |"),
    ("Acquire result", "        e.st32(a[4], h);\n        *r = 0;", "        e.st32(a[4], h);\n        *r = h;"),
    ("first host address", "static const uint32_t kFirstHost = 3;", "static const uint32_t kFirstHost = 2;"),
    ("host step", "addr = kHleFuncBase + 8 * (kFirstHost + (uint32_t)S.hosts.size());", "addr = kHleFuncBase + 8 * (kFirstHost + 1 + (uint32_t)S.hosts.size());"),
    ("data export size", "if (is_data) addr = e.runtime_alloc(0x100);", "if (is_data) addr = e.runtime_alloc(0xF0);"),
    ("unknown handle module", "std::string lib = m != S.modules.end() ? m->second : \"?\";", "std::string lib = m != S.modules.end() ? m->second : \"\";"),
    ("exports not cached", "        if (it == S.exports.end()) {\n            uint32_t addr;", "        if (true) {\n            uint32_t addr;"),
    ("cache key without module", "std::string key = lib + \".\" + sym;", "std::string key = sym;"),
    ("stub answer", "if (S.hosts[k] == kStub) { *r = 0; return 1; }", "if (S.hosts[k] == kStub) { return 0; }"),
    ("SetAllocator result", "case DynSetAllocator: *r = 0; return 1;", "case DynSetAllocator: return 0;"),
    ("Release result", "case DynRelease: return 0;", "case DynRelease: *r = 0; return 1;"),
    ("Create result", "case E_Create: set_state(S, kHidden); *r = 1; return 1;", "case E_Create: set_state(S, kHidden); *r = 0; return 1;"),
    ("Create keeps the state", "case E_Create: set_state(S, kHidden); *r = 1; return 1;", "case E_Create: *r = 1; return 1;"),
    ("fade one frame early", "static const uint32_t kFadeCalcs = 6, kDecideCalcs = 30;", "static const uint32_t kFadeCalcs = 5, kDecideCalcs = 30;"),
    ("fade one frame late", "static const uint32_t kFadeCalcs = 6, kDecideCalcs = 30;", "static const uint32_t kFadeCalcs = 7, kDecideCalcs = 30;"),
    ("confirm one frame early", "static const uint32_t kFadeCalcs = 6, kDecideCalcs = 30;", "static const uint32_t kFadeCalcs = 6, kDecideCalcs = 29;"),
    ("confirm one frame late", "static const uint32_t kFadeCalcs = 6, kDecideCalcs = 30;", "static const uint32_t kFadeCalcs = 6, kDecideCalcs = 31;"),
    ("timer not reset", "inline void set_state(State& S, uint32_t s) { S.err_state = s; S.err_calcs = 0; }",
     "inline void set_state(State& S, uint32_t s) { S.err_state = s; }"),
    ("AppearError keeps decided", "case E_AppearError: S.err_decided = false; set_state(S, kAppearing); return 0;",
     "case E_AppearError: set_state(S, kAppearing); return 0;"),
    ("DisappearError from any state", "case E_DisappearError: if (S.err_state == kVisible) set_state(S, kDisappearing); return 0;",
     "case E_DisappearError: if (S.err_state != kHidden) set_state(S, kDisappearing); return 0;"),
    ("disappearing ends visible", "else if (S.err_state == kDisappearing && S.err_calcs > kFadeCalcs) set_state(S, kHidden);",
     "else if (S.err_state == kDisappearing && S.err_calcs > kFadeCalcs) set_state(S, kVisible);"),
    ("right button decided", "case E_IsDecideSelectRightButtonError: case E_GetSelectButtonNumError: case E_GetResultCode: *r = 0; return 1;",
     "case E_GetSelectButtonNumError: case E_GetResultCode: *r = 0; return 1;\n    case E_IsDecideSelectRightButtonError: *r = S.err_decided; return 1;"),
    ("result type", "case E_GetResultType: *r = S.err_decided ? 1 : 0; return 1;", "case E_GetResultType: *r = S.err_decided ? 2 : 0; return 1;"),
    ("HOME sign kept", "case E_DisappearHomeNixSign: S.err_home_nix = false; return 0;", "case E_DisappearHomeNixSign: return 0;"),
    ("cursor active", "case E_IsSelectCursorActive: *r = 0; return 1;", "case E_IsSelectCursorActive: *r = 1; return 1;"),
    ("SwkbdCreate result", "if (fn == S_Create) { if (!S.sw_form_buf) S.sw_form_buf = e.runtime_alloc(kFormBufBytes); *r = 1; return 1; }",
     "if (fn == S_Create) { if (!S.sw_form_buf) S.sw_form_buf = e.runtime_alloc(kFormBufBytes); *r = 0; return 1; }"),
    ("SwkbdCreate allocates each time", "if (fn == S_Create) { if (!S.sw_form_buf) S.sw_form_buf", "if (fn == S_Create) { S.sw_form_buf"),
    ("SwkbdCreate allocates nothing", "if (fn == S_Create) { if (!S.sw_form_buf) S.sw_form_buf = e.runtime_alloc(kFormBufBytes); *r = 1; return 1; }", "if (fn == S_Create) { *r = 1; return 1; }"),
    ("form buffer size", "static const uint32_t kFormBufBytes = 0x2000;", "static const uint32_t kFormBufBytes = 0x1000;"),
    ("ProcUIProcessMessages answer", "kProcUIForeground = 0;", "kProcUIForeground = 2;"),
    ("swkbd word ignored", "else if (swk && lib == \"swkbd\")", "else if (lib == \"swkbd\")"),
    ("unmodelled swkbd export answered", "    static char why[200];\n", "    if (fn == S_GetStateKeyboard) { *r = 0; return 1; }\n    static char why[200];\n"),
    ("swkbd exports as stubs", "{ fn = swkbd_index(sym); if (fn >= 0) fn += kSwkbdBase; }", "{ fn = -1; }"),
    ("unknown swkbd symbol not a stub", "{ fn = swkbd_index(sym); if (fn >= 0) fn += kSwkbdBase; }", "{ fn = swkbd_index(sym); fn += kSwkbdBase; }"),
    ("unknown erreula symbol not a stub", "if (lib == \"erreula\") fn = erreula_index(sym);", "if (lib == \"erreula\") fn = erreula_index(sym) & 0x7F;"),
]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port-src", default=os.environ.get("WWHD_PORT_SRC", "runtime/src"))
    ap.add_argument("--port-rev", default="f5554821", help="read the port sources at this commit (git show); '' = the working tree")
    ap.add_argument("--seqs", default="400")
    ap.add_argument("--len", default="600")
    ap.add_argument("--seed", default="1")
    ap.add_argument("--inject", action="store_true", help="also check that each injected model bug is detected")
    a = ap.parse_args()

    def port_file(name):
        if a.port_rev:
            repo = os.path.dirname(os.path.dirname(os.path.abspath(a.port_src)))
            return subprocess.run(["git", "-C", repo, "show", "%s:runtime/src/%s" % (a.port_rev, name)], capture_output=True, text=True,
                                  check=True).stdout
        return open(os.path.join(a.port_src, name)).read()
    import verify  # noqa: E402
    d = tempfile.mkdtemp(prefix="dynload_equiv-", dir=verify.verify_scratch_base() or None)
    stubs = port_file("hle/system_stubs.cpp")
    m = re.search(r"^// ---- nn_act \(accounts\)$(.*?)^// ---- nn_ac ", stubs, re.S | re.M)
    if not m:
        sys.exit("dynload_equiv: the nn_act section not found in system_stubs.cpp")
    act = m.group(1)
    misc = port_file("hle/coreinit_misc.cpp")
    m = re.search(r"^// -+ OSDynLoad$(.*?)^// -+ system info$", misc, re.S | re.M)
    if not m:
        sys.exit("dynload_equiv: the OSDynLoad section not found in coreinit_misc.cpp")
    dyn = m.group(1)
    for fn in ("HLE(coreinit, OSDynLoad_Acquire)", "HLE(coreinit, OSDynLoad_Release)", "HLE(coreinit, OSDynLoad_SetAllocator)",
               "HLE(coreinit, OSDynLoad_FindExport)", "hle_find(lib.c_str(), sym.c_str())", "dispatch::register_host", "mem::runtime_alloc(0x100)"):
        if fn not in dyn:
            sys.exit("dynload_equiv: %s not in the OSDynLoad section" % fn)
    err = port_file("hle/erreula.cpp")
    if err.count("std::chrono::steady_clock") != 3:
        sys.exit("dynload_equiv: erreula.cpp: expected 3 uses of std::chrono::steady_clock, found %d" % err.count("std::chrono::steady_clock"))
    err = err.replace("std::chrono::steady_clock", "eq_clock")
    err = "\n".join(l for l in err.split("\n") if not l.lstrip().startswith("#include"))
    sw = port_file("hle/swkbd.cpp")
    sw = "\n".join(l for l in sw.split("\n") if not l.lstrip().startswith("#include"))
    swkbd_cpp = os.path.join(d, "port_swkbd.cpp")
    open(swkbd_cpp, "w").write(PRELUDE + SWKBD_PRELUDE + "\n// ---- swkbd.cpp\n" + sw + SWKBD_PROBE)
    core = port_file("core.cpp")
    m = re.search(r"^PpcFunc hle_find\(const char\* lib, const char\* name\) \{.*?^\}", core, re.S | re.M)
    if not m:
        sys.exit("dynload_equiv: core.cpp hle_find not found")
    hle_find = m.group(0)
    rt = port_file("runtime.h")
    m = re.search(r"constexpr uint32_t kHleFuncBase = (0x[0-9A-Fa-f]+);", rt)
    if not m:
        sys.exit("dynload_equiv: runtime.h kHleFuncBase not found")
    if int(m.group(1), 16) != 0xC2000000:
        sys.exit("dynload_equiv: runtime.h kHleFuncBase is %s; dynload_model.h has C2000000" % m.group(1))
    base = m.group(1)
    if core.count("register_host(") < 1 or not re.search(r"uint32_t a = g_next_host\.fetch_add\(8\);", core):
        sys.exit("dynload_equiv: core.cpp register_host is no longer an 8-byte step from g_next_host")
    memc = port_file("hle/coreinit_mem.cpp")
    regs = re.findall(r"dispatch::register_host\(([a-z_]+),", memc)
    if regs != ["default_alloc", "default_alloc_ex", "default_free"]:
        sys.exit("dynload_equiv: coreinit_mem.cpp's boot registrations changed: %r" % regs)
    m = re.search(r"^HLE\(proc_ui, ProcUIProcessMessages\).*$", stubs, re.M)
    if not m:
        sys.exit("dynload_equiv: HLE(proc_ui, ProcUIProcessMessages) not in system_stubs.cpp")
    act += "\n" + m.group(0) + "\n"
    port_cpp = os.path.join(d, "port.cpp")
    open(port_cpp, "w").write(PRELUDE + SUPPORT.replace("KHLEFUNCBASE", base + "u") + "\n// ---- core.cpp hle_find\n" + hle_find +
                              "\n// ---- system_stubs.cpp nn_act section\nstatic constexpr uint32_t kResultOk = 0;\n" + act +
                              "\n// ---- coreinit_misc.cpp OSDynLoad section\n" + dyn + "\n// ---- erreula.cpp\n" + err + PROBE)
    prog_cpp = os.path.join(d, "equiv.cpp")
    open(prog_cpp, "w").write(PRELUDE + PROG)
    flags = ["clang++", "-std=c++20", "-O1", "-w", "-I" + d]

    def build(model_dir, exe):
        objs = []
        for src in (port_cpp, swkbd_cpp):
            o = os.path.join(d, os.path.basename(src) + ".o")
            if not os.path.exists(o):
                r = subprocess.run(flags + ["-c", src, "-o", o], capture_output=True, text=True, errors="replace")
                if r.returncode:
                    print(r.stderr[-5000:])
                    return False
            objs.append(o)
        r = subprocess.run(flags + ["-I" + model_dir, prog_cpp] + objs + ["-o", exe], capture_output=True, text=True, errors="replace")
        if r.returncode:
            print(r.stderr[-5000:])
        return r.returncode == 0

    model_src = open(os.path.join(HERE, "src", "dynload_model.h")).read()
    exe = os.path.join(d, "equiv")
    if not build(os.path.join(HERE, "src"), exe):
        print("dynload_equiv: build failed (%s)" % d)
        return 2
    r = subprocess.run([exe, a.seqs, a.len, a.seed], capture_output=True, text=True, errors="replace")
    print(r.stdout.strip()[-3000:])
    ok = r.returncode == 0
    print("dynload_equiv: %s (port %s at %s; programs in %s)" % ("PASS" if ok else "FAIL", a.port_src, a.port_rev or "the working tree", d))
    if not a.inject:
        return 0 if ok else 1
    missed = []
    for i, (what, old, new) in enumerate(INJECT):
        if model_src.count(old) != 1:
            print("dynload_equiv: inject %d (%s): the text occurs %d times in dynload_model.h" % (i, what, model_src.count(old)))
            missed.append(what)
            continue
        md = os.path.join(d, "inject%02d" % i)
        os.makedirs(md, exist_ok=True)
        open(os.path.join(md, "dynload_model.h"), "w").write(model_src.replace(old, new))
        ie = os.path.join(md, "equiv")
        if not build(md, ie):
            missed.append(what + " (build failed)")
            continue
        r = subprocess.run([ie, "100", a.len, a.seed], capture_output=True, text=True, errors="replace")
        first = next((l for l in r.stdout.splitlines() if l.startswith("seed ")), "")
        det = r.returncode == 1 and first
        print("dynload_equiv: inject %2d %-40s %s" % (i, what, ("detected: " + first[:150]) if det else "NOT DETECTED (rc %d)" % r.returncode))
        if not det:
            missed.append(what)
    print("dynload_equiv: injected bugs: %d of %d detected" % (len(INJECT) - len(missed), len(INJECT)))
    return 0 if ok and not missed else 1


if __name__ == "__main__":
    sys.exit(main())

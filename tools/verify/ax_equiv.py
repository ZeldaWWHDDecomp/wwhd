#!/usr/bin/env python3
"""ax_equiv.py: the `osmem ... ax` model (tools/verify/src/ax_model.h) against the shipping port's own snd_core code,
runtime/src/hle/ax.cpp, read at port commit f5554821 (--port-rev; '' = the working tree) with its headers
audio_output_mode.h, audio_channels.h, dsp_overload_guard.h, game_clock.h and compiled in the same program as the model,
with shim headers for runtime.h, platform/host.h, guest_addr.h (GD = identity, the USA build), audio_out.h (a stereo
device, GamePad sounds off) and savestate.h. Both sides use the port's compile flags (-ffp-contract=off -fwrapv).

Each side has its own flat guest memory (16 MB chunks on demand, the same random contents). A seeded random sequence of
snd_core calls (AXInit first; voices acquired, freed, configured through every AXSetVoice* import with random parameter
blocks over random ADPCM / PCM16 / PCM8 sample data, started and stopped; aux, app-frame and final-mix callbacks
registered and removed; device upsample stages and aux returns; the game's fader chain at 1018EC64 made valid or not,
TV play / Off-TV play / fades) is run on both, interleaved with audio frames: on the port side the body of its
frame_thread loop (extracted from the source: process_voices, update_output_mode, run_aux_callbacks, the app frame
callbacks, store_aux_input, output_frame, the aux frame toggle; keep_dsp_channels and the host pacing left out as in the
model), on the model side axm::frame. A guest callback is simulated identically on both sides: it is logged (function,
arguments, a hash of all guest memory) and then writes seeded data through the buffers it gets (aux: the channel
buffers; final mix: the sample data), so later frames read what a game callback would leave. After every call and
every frame the results (r3 / void), the callback logs, every guest byte and every voice's state must be equal.
Exit 0 on PASS."""
import argparse
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

RUNTIME_H = r"""
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>
#include <map>
#include <memory>
struct Cpu { uint32_t r[32]; uint32_t lr; struct { double ps0, ps1; } f[32]; };
namespace eqmem {
struct Mem {
    std::map<uint32_t, std::unique_ptr<uint8_t[]>> chunks;
    uint8_t* ptr(uint32_t ea) {
        auto& c = chunks[ea >> 24];
        if (!c) { c.reset(new uint8_t[1u << 24]); fill(ea >> 24, c.get()); }
        return c.get() + (ea & 0xFFFFFF);
    }
    static void fill(uint32_t k, uint8_t* p) { uint64_t s = 0x1234567 + k; for (uint32_t i = 0; i < (1u << 24); i++) { s = s * 6364136223846793005ull + 1; p[i] = (uint8_t)(s >> 56); } }
};
extern Mem port_mem;
}
namespace mem {
constexpr uint32_t kMem2Start = 0x10000000, kMem2End = 0x50000000;
inline uint8_t* ptr(uint32_t ea) { return eqmem::port_mem.ptr(ea); }
uint32_t runtime_alloc(uint32_t size, uint32_t align = 16);
uint32_t host_alloc(uint32_t size, uint32_t align = 16);
}
inline uint16_t ld16(uint32_t a) { uint8_t* p = mem::ptr(a); return (uint16_t)(p[0] << 8 | p[1]); }
inline uint32_t ld32(uint32_t a) { uint8_t* p = mem::ptr(a); return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
inline void st16(uint32_t a, uint16_t v) { uint8_t* p = mem::ptr(a); p[0] = v >> 8; p[1] = (uint8_t)v; }
inline void st32(uint32_t a, uint32_t v) { uint8_t* p = mem::ptr(a); p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; }
inline uint32_t arg(Cpu* c, int i) { return c->r[3 + i]; }
inline void ret(Cpu* c, uint32_t v) { c->r[3] = v; }
#define HLE(lib, name) void imp_##lib##_##name(Cpu* c)
#define LOG(...) do {} while (0)
void guest_call(Cpu* c, uint32_t fn, std::initializer_list<uint32_t> args = {});
namespace timebase { constexpr uint64_t kTicksPerSec = 62156250ull; inline uint64_t now() { return 0; } }
namespace threads {
inline Cpu* make_service_cpu(const char*, uint32_t) { static Cpu c; return &c; }
inline void set_service_core(uint32_t) {}
inline void service_begin() {}
inline void service_end() {}
inline bool ensure_core() { return false; }
inline void release_core() {}
}
namespace interp { inline const char* phase_name() { return "-"; } }
"""
HOST_H = r"""
#pragma once
namespace host { inline void set_thread_name(const char*) {} }
"""
GUEST_ADDR_H = r"""
#pragma once
#define GD(a) ((uint32_t)(a))
"""
AUDIO_OUT_H = r"""
#pragma once
#include <cstdint>
namespace audio {
constexpr int kRate = 48000;
inline void init() {}
inline int channels() { return 2; }
inline bool gamepad_sounds() { return false; }
inline float gamepad_sounds_volume() { return 0.f; }
inline void push(const int16_t*, int) {}
inline int buffered_frames() { return 0; }
inline int target_frames() { return 1; }
inline void stats(uint64_t& a, uint64_t& b) { a = b = 0; }
inline void flush() {}
}
"""
SAVESTATE_H = r"""
#pragma once
#include <string>
namespace ss {
struct Writer { template <class T> void pod(const T&) {} void u8(uint32_t) {} void u32(uint32_t) {} void bytes(const void*, size_t) {} };
struct Reader { bool ok = true; template <class T> T pod() { return T(); } uint8_t u8() { return 0; } uint32_t u32() { return 0; }
                void bytes(void*, size_t) {} bool at_end() const { return true; } };
}
"""
GAME_CLOCK_SHIM = "namespace game_clock { unsigned rate() { return 1; } }\n"

PROG = r"""
#include "runtime.h"
#include "hle/ax.cpp"
#include "ax_model.h"
#include <cstdlib>

namespace eqmem { Mem port_mem, model_mem; }
@@GAME_CLOCK@@
static uint32_t g_port_rt = 0x00800000, g_port_host = 0x00C00000, g_model_rt = 0x00800000, g_model_host = 0x00C00000;
static uint32_t bump(uint32_t& top, uint32_t size, uint32_t align, eqmem::Mem& m) {
    uint32_t p = (top + align - 1) & ~(align - 1);
    top = p + size;
    memset(m.ptr(p), 0, size);
    return p;
}
uint32_t mem::runtime_alloc(uint32_t size, uint32_t align) { return bump(g_port_rt, size, align, eqmem::port_mem); }
uint32_t mem::host_alloc(uint32_t size, uint32_t align) { return bump(g_port_host, size, align, eqmem::port_mem); }

/* a simulated guest callback, identical on both sides: logged, then it writes seeded data through its buffers */
struct CallRec { uint32_t fn, nargs, a[3]; uint64_t hash; };
static std::vector<CallRec> g_log[2];
/* the guest memory the AX code and the callbacks write: the runtime and host_alloc regions, the parameter blocks, the faders */
static const uint32_t kWin[][2] = {{0x00800000, 0x10000}, {0x00C00000, 0x10000}, {0x00300000, 0x1100}, {0x10100000, 0x400}, {0x1018EC00, 0x100}};
static uint64_t mem_hash(eqmem::Mem& m) {
    uint64_t h = 1469598103934665603ull;
    for (auto& w : kWin) {
        const uint8_t* p = m.ptr(w[0]);
        for (uint32_t i = 0; i < w[1]; i += 8) { uint64_t x; memcpy(&x, p + i, 8); h = (h ^ x) * 1099511628211ull; }
    }
    return h;
}
static uint64_t g_cb_seed = 0;
static void callback(int side, eqmem::Mem& m, uint32_t fn, int n, const uint32_t* a) {
    CallRec r{fn, (uint32_t)n, {n > 0 ? a[0] : 0, n > 1 ? a[1] : 0, n > 2 ? a[2] : 0}, mem_hash(m)};
    g_log[side].push_back(r);
    uint64_t s = g_cb_seed ^ (uint64_t)fn * 0x9E3779B97F4A7C15ull ^ g_log[side].size();
    auto rnd = [&]() { s = s * 6364136223846793005ull + 1442695040888963407ull; return (uint32_t)(s >> 33); };
    auto w32 = [&](uint32_t ea, uint32_t v) { uint8_t* p = m.ptr(ea); p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; };
    auto r32 = [&](uint32_t ea) { uint8_t* p = m.ptr(ea); return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; };
    if (n == 3) { /* aux: (ptrs, user, info{channels, samples}) */
        uint32_t chs = r32(a[2]), smp = r32(a[2] + 4);
        if (chs > 8 || smp > 200) return;
        for (uint32_t c = 0; c < chs; c++) {
            uint32_t buf = r32(a[0] + 4 * c);
            for (uint32_t i = 0; i < smp; i += 7) w32(buf + 4 * i, (rnd() % 20001) - 10000);
        }
    } else if (n == 1) { /* final mix: (param{ptrs, channels u16, samples u16, devices u16}) */
        uint32_t ptrs = r32(a[0]), chs = r32(a[0] + 4) >> 16, smp = r32(a[0] + 4) & 0xFFFF, devs = r32(a[0] + 8) >> 16;
        if (chs * devs > 16 || smp > 200) return;
        for (uint32_t c = 0; c < chs * devs; c++) {
            uint32_t buf = r32(ptrs + 4 * c);
            for (uint32_t i = 0; i < smp; i += 11) w32(buf + 4 * i, (rnd() % 60001) - 30000);
        }
    }
}
void guest_call(Cpu*, uint32_t fn, std::initializer_list<uint32_t> args) {
    uint32_t a[3] = {};
    int n = 0;
    for (uint32_t v : args) a[n++] = v;
    callback(0, eqmem::port_mem, fn, n, a);
}
struct FlatEnv {
    eqmem::Mem& m;
    uint8_t ld8(uint32_t a) { return *m.ptr(a); }
    uint16_t ld16(uint32_t a) { uint8_t* p = m.ptr(a); return (uint16_t)(p[0] << 8 | p[1]); }
    uint32_t ld32(uint32_t a) { uint8_t* p = m.ptr(a); return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
    void st16(uint32_t a, uint32_t v) { uint8_t* p = m.ptr(a); p[0] = (uint8_t)(v >> 8); p[1] = (uint8_t)v; }
    void st32(uint32_t a, uint32_t v) { uint8_t* p = m.ptr(a); p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; }
    void zero(uint32_t a, uint32_t n) { memset(m.ptr(a), 0, n); }
    uint32_t runtime_alloc(uint32_t size, uint32_t align) { return bump(g_model_rt, size, align, m); }
    uint32_t host_alloc(uint32_t size, uint32_t align) { return bump(g_model_host, size, align, m); }
    void call(uint32_t fn, int n, const uint32_t* a) { callback(1, m, fn, n, a); }
};
static axm::State* M;

/* the port's frame (frame_thread's loop body, extracted from the source) */
static void port_frame(Cpu* c) {
@@FRAME@@
}

static uint64_t rs;
static uint32_t rnd(uint32_t n) { rs = rs * 6364136223846793005ull + 1442695040888963407ull; return n ? (uint32_t)((rs >> 33) % n) : 0; }
static int fails = 0;
static long nops = 0, nframes = 0, ncb = 0;
static const char* what = "";

static bool same_mem_win() {
    for (auto& w : kWin) {
        const uint8_t *p = eqmem::port_mem.ptr(w[0]), *q = eqmem::model_mem.ptr(w[0]);
        if (memcmp(p, q, w[1]))
            for (uint32_t i = 0; i < w[1]; i++)
                if (p[i] != q[i]) { printf("DIFF %s: guest byte %08X port %02X model %02X\n", what, w[0] + i, p[i], q[i]); return false; }
    }
    return true;
}
static bool same_mem() {
    for (auto& kv : eqmem::port_mem.chunks) eqmem::model_mem.ptr(kv.first << 24);
    for (auto& kv : eqmem::model_mem.chunks) eqmem::port_mem.ptr(kv.first << 24);
    for (auto& kv : eqmem::port_mem.chunks) {
        const uint8_t *p = kv.second.get(), *q = eqmem::model_mem.chunks[kv.first].get();
        if (memcmp(p, q, 1u << 24)) {
            for (uint32_t i = 0; i < (1u << 24); i++)
                if (p[i] != q[i]) { printf("DIFF %s: guest byte %08X port %02X model %02X\n", what, (kv.first << 24) | i, p[i], q[i]); break; }
            return false;
        }
    }
    return true;
}
static bool same_voices() {
    for (int i = 0; i < axm::kMaxVoices; i++) {
        const Voice& p = g_voices[i];
        const axm::Voice& q = M->voices[i];
        if (p.acquired != q.acquired || p.vpb != q.vpb || p.state != q.state || p.type != q.type || p.format != q.format || p.loop != q.loop ||
            p.loop_abs != q.loop_abs || p.end_abs != q.end_abs || p.cur_abs != q.cur_abs || p.samples != q.samples ||
            p.loop_count != q.loop_count || p.scale != q.scale || p.yn1 != q.yn1 || p.yn2 != q.yn2 || p.filter != q.filter ||
            p.ratio != q.ratio || p.frac != q.frac || p.prev != q.prev || p.cur != q.cur || p.ve_vol != q.ve_vol || p.ve_delta != q.ve_delta ||
            p.lpf_yn1 != q.lpf_yn1 || memcmp(&p.bq_xn1, &q.bq_xn1, 16) || memcmp(p.tv, q.tv, sizeof p.tv) ||
            memcmp(g_drc.mix[i], M->drc.mix[i], sizeof g_drc.mix[i])) {
            printf("DIFF %s: voice %d state\n", what, i);
            return false;
        }
    }
    if (g_aux_frame != M->aux_frame || g_output.play_drc != M->play_drc || memcmp(g_drc_aux_idle, M->drc_aux_idle, sizeof g_drc_aux_idle) ||
        memcmp(g_up_hist, M->up_hist, sizeof g_up_hist) || memcmp(g_drc_up_hist, M->drc_up_hist, sizeof g_drc_up_hist)) {
        printf("DIFF %s: frame state (aux frame %d/%d, play_drc %d/%d)\n", what, g_aux_frame, M->aux_frame, g_output.play_drc, M->play_drc);
        return false;
    }
    return true;
}
static bool same_logs() {
    if (g_log[0].size() != g_log[1].size()) { printf("DIFF %s: callbacks port %zu model %zu\n", what, g_log[0].size(), g_log[1].size()); return false; }
    for (size_t i = 0; i < g_log[0].size(); i++) {
        const CallRec &p = g_log[0][i], &q = g_log[1][i];
        if (p.fn != q.fn || p.nargs != q.nargs || memcmp(p.a, q.a, sizeof p.a) || p.hash != q.hash) {
            printf("DIFF %s: callback #%zu port %08X(%08X,%08X,%08X) model %08X(%08X,%08X,%08X)%s\n", what, i, p.fn, p.a[0], p.a[1], p.a[2],
                   q.fn, q.a[0], q.a[1], q.a[2], p.hash != q.hash ? " (memory at the call differs)" : "");
            return false;
        }
    }
    ncb += g_log[0].size();
    g_log[0].clear();
    g_log[1].clear();
    return true;
}
static void check() {
    bool ok = same_logs() && same_voices() && same_mem_win();
    if (!ok && ++fails >= 5) { printf("ax_equiv: stopping after %d differences\n", fails); exit(1); }
}

/* both sides: one import call with the same registers */
typedef void (*Hle)(Cpu*);
static void both(Hle h, uint32_t imp, const char* name, const uint32_t* regs, double f1) {
    Cpu c{};
    for (int i = 3; i <= 10; i++) c.r[i] = regs[i];
    c.f[1].ps0 = c.f[1].ps1 = f1;
    uint32_t r3_in = c.r[3];
    h(&c);
    uint32_t a[11] = {};
    for (int i = 3; i <= 10; i++) a[i] = regs[i];
    FlatEnv e{eqmem::model_mem};
    uint32_t r = 0;
    bool started = false;
    int k = axm::call(e, *M, imp, a, f1, &r, &started);
    uint32_t mr = k == 1 ? r : r3_in;
    what = name;
    nops++;
    if (k < 0) { printf("DIFF %s: the model does not answer %08X\n", name, imp); fails++; return; }
    if (c.r[3] != mr) { printf("DIFF %s: r3 port %08X model %08X\n", name, c.r[3], mr); fails++; }
    check();
}
static long st_playing = 0, st_audible = 0, st_drc = 0, st_auxcb = 0;
static void frame() {
    Cpu c{};
    port_frame(&c);
    FlatEnv e{eqmem::model_mem};
    axm::frame(e, *M);
    for (auto& v : M->voices) st_playing += v.acquired && v.state == 1;
    bool any = false;
    for (int ch = 0; ch < axm::kTvChannels && !any; ch++) for (int i = 0; i < axm::kSamples; i++) if (M->tv_bus[0][ch][i] != 0) { any = true; break; }
    st_audible += any;
    st_drc += M->play_drc;
    st_auxcb += M->aux_cb[0] || M->aux_cb[1] || M->aux_cb[2];
    what = "frame";
    nframes++;
    check();
}
static void wr(uint32_t a, uint32_t v, int size) { /* the same guest write on both sides (the game setting up a parameter block) */
    for (int j = 0; j < size; j++) {
        *eqmem::port_mem.ptr(a + j) = (uint8_t)(v >> (8 * (size - 1 - j)));
        *eqmem::model_mem.ptr(a + j) = (uint8_t)(v >> (8 * (size - 1 - j)));
    }
}
static uint32_t rd32(uint32_t a) { uint8_t* p = eqmem::port_mem.ptr(a); return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }

#define B(name) imp_snd_core_##name, axm::name, #name
int main(int argc, char** argv) {
    int nseq = argc > 1 ? atoi(argv[1]) : 60, len = argc > 2 ? atoi(argv[2]) : 400;
    M = new axm::State();
    const uint32_t PARAM = 0x00300000, SAMPLES = 0x00200000, FADE = 0x10100000;
    for (int s = 1; s <= nseq; s++) {
        /* fresh state on both sides (the port's globals reset as a new session would have them) */
        eqmem::port_mem.chunks.clear(); eqmem::model_mem.chunks.clear();
        for (auto& v : g_voices) v = Voice{};
        memset(g_app_frame_cb, 0, sizeof g_app_frame_cb); memset(g_final_mix_cb, 0, sizeof g_final_mix_cb);
        memset(g_aux_cb, 0, sizeof g_aux_cb); memset(g_aux_user, 0, sizeof g_aux_user);
        for (auto& x : g_aux_return) x = 0x8000;
        memset(g_upsample_stage, 0, sizeof g_upsample_stage);
        g_drc = DrcState{{}, {}, {}, {0x8000, 0x8000, 0x8000}};
        G = GuestBuffers{}; g_aux_frame = 0; g_vpb_base = 0; g_running = false;
        memset(g_up_hist, 0, sizeof g_up_hist); memset(g_drc_up_hist, 0, sizeof g_drc_up_hist);
        memset(g_drc_aux, 0, sizeof g_drc_aux); memset(g_drc_aux_idle, 0, sizeof g_drc_aux_idle);
        g_output = audio::OutputSelect();
        *M = axm::State();
        g_port_rt = g_model_rt = 0x00800000; g_port_host = g_model_host = 0x00C00000;
        rs = (uint64_t)s * 0x9E3779B97F4A7C15ull;
        g_cb_seed = rs;
        uint32_t R[11] = {};
        /* the fader chain: 1018EC64 -> mgr (+0x10) -> x (+4) -> player; faders at player+0x194 / +0x1AC */
        if (rnd(3)) {
            wr(0x1018EC64, FADE, 4); wr(FADE + 0x10, FADE + 0x100, 4); wr(FADE + 0x104, FADE + 0x200, 4);
        }
        both(B(AXInit), R, 0);
        /* AXInit's frame thread runs at once (port: the thread's first loop pass) */
        frame();
        uint32_t vpbs[8] = {};
        int nv = 0;
        for (int i = 0; i < len; i++) {
            int k = rnd(40);
            for (int j = 3; j <= 10; j++) R[j] = rnd(4) ? rnd(0x100) : rnd(0xFFFFFFFF);
            uint32_t v = nv ? vpbs[rnd(nv)] : 0;
            if (rnd(20) == 0) v = rnd(0xFFFFFFFF); /* an unknown voice */
            uint32_t p = PARAM + 0x100 * rnd(16);
            for (int j = 0; j < 0x80; j += 4) wr(p + j, rnd(4) ? rnd(0x10000) : rnd(0xFFFFFFFF), 4);
            switch (k) {
            case 0: case 1: case 2: { R[3] = rnd(32); R[4] = rnd(0x1000); R[5] = rnd(0x1000); both(B(AXAcquireVoiceEx), R, 0);
                uint32_t got = rd32(0) == 0 ? 0 : 0; (void)got; break; }
            case 3: R[3] = v; both(B(AXFreeVoice), R, 0); break;
            case 4: case 5: R[3] = v; R[4] = rnd(3) ? 1 : rnd(3); both(B(AXSetVoiceState), R, 0); break;
            case 6: R[3] = v; R[4] = rnd(2); both(B(AXSetVoiceType), R, 0); break;
            case 7: case 8: { /* AXPBOFFSET over the sample data */
                uint16_t fmts[4] = {0, 0x0A, 0x19, (uint16_t)rnd(0x100)};
                uint16_t fmt = fmts[rnd(4)];
                uint32_t base = (SAMPLES + rnd(0x1000)) >> (fmt == 0 ? 1 : fmt == 0x0A ? 0 : 0); (void)base;
                uint32_t sp = SAMPLES / (fmt == 0x0A ? 2 : 1) + rnd(0x100);
                if (fmt == 0) sp = SAMPLES / 2 / 8 * 8 + rnd(0x100) * 8;
                uint32_t len2 = 16 + rnd(4000);
                wr(p, fmt, 2); wr(p + 2, rnd(2), 2); wr(p + 4, rnd(len2), 4); wr(p + 8, len2, 4); wr(p + 0xC, rnd(4) ? 0 : rnd(len2), 4);
                wr(p + 0x10, sp, 4);
                R[3] = v; R[4] = p; both(B(AXSetVoiceOffsets), R, 0); break;
            }
            case 9: R[3] = v; R[4] = p; both(B(AXGetVoiceOffsets), R, 0); break;
            case 10: R[3] = v; R[4] = rnd(2); both(B(AXSetVoiceLoop), R, 0); break;
            case 11: R[3] = v; R[4] = rnd(4000); R[5] = SAMPLES / 2 + rnd(0x100); both(B(AXSetVoiceEndOffsetEx), R, 0); break;
            case 12: R[3] = v; R[4] = rnd(4000); R[5] = SAMPLES / 2 + rnd(0x100); both(B(AXSetVoiceLoopOffsetEx), R, 0); break;
            case 13: R[3] = v; R[4] = SAMPLES / 2 + rnd(0x100); both(B(AXGetVoiceCurrentOffsetEx), R, 0); break;
            case 14: R[3] = v; both(B(AXGetVoiceLoopCount), R, 0); break;
            case 15: { R[3] = v; double f = rnd(4) ? rnd(40000) / 10000.0 : (rnd(2) ? -1.0 : 9.0); both(B(AXSetVoiceSrcRatio), R, f); break; }
            case 16: wr(p, rnd(9), 2); R[3] = v; R[4] = p; both(B(AXSetVoiceSrc), R, 0); break;
            case 17: R[3] = v; R[4] = rnd(3); both(B(AXSetVoiceSrcType), R, 0); break;
            case 18: R[3] = v; R[4] = p; both(B(AXSetVoiceVe), R, 0); break;
            case 19: case 20: R[3] = v; R[4] = rnd(3); R[5] = rnd(3) ? 0 : 1; R[6] = rnd(10) ? p : 0; both(B(AXSetVoiceDeviceMix), R, 0); break;
            case 21: R[3] = v; R[4] = p; both(B(AXSetVoiceAdpcm), R, 0); break;
            case 22: R[3] = v; R[4] = p; both(B(AXSetVoiceAdpcmLoop), R, 0); break;
            case 23: wr(p, rnd(2), 2); R[3] = v; R[4] = p; both(B(AXSetVoiceLpf), R, 0); break;
            case 24: R[3] = v; R[4] = rnd(0x10000); R[5] = rnd(0x10000); both(B(AXSetVoiceLpfCoefs), R, 0); break;
            case 25: wr(p, rnd(2), 2); R[3] = v; R[4] = p; both(B(AXSetVoiceBiquad), R, 0); break;
            case 26: R[3] = v; both(B(AXSetVoiceBiquadCoefs), R, 0); break;
            case 27: R[3] = rnd(16000); R[4] = p; R[5] = p + 2; both(B(AXComputeLpfCoefs), R, 0); break;
            case 28: R[3] = 0x02000000 + 4 * rnd(8); both(B(AXRegisterAppFrameCallback), R, 0); break;
            case 29: R[3] = 0x02000000 + 4 * rnd(8); both(B(AXDeregisterAppFrameCallback), R, 0); break;
            case 30: R[3] = rnd(4); R[4] = rnd(4) ? 0x02100000 + 4 * rnd(4) : 0; both(B(AXRegisterDeviceFinalMixCallback), R, 0); break;
            case 31: R[3] = rnd(4); R[4] = rnd(2) ? p : 0; both(B(AXGetDeviceFinalMixCallback), R, 0); break;
            case 32: R[3] = rnd(3); R[4] = rnd(2); R[5] = rnd(4); R[6] = rnd(4) ? 0x02200000 + 4 * rnd(4) : 0; both(B(AXRegisterAuxCallback), R, 0); break;
            case 33: R[3] = rnd(3); R[4] = rnd(2); R[5] = rnd(4); R[6] = rnd(0x10000); both(B(AXSetAuxReturnVolume), R, 0); break;
            case 34: R[3] = rnd(4); R[4] = rnd(2); both(B(AXSetDeviceUpsampleStage), R, 0); break;
            case 35: R[3] = rnd(3); R[4] = rnd(2) ? p : 0; both(B(AXGetDeviceMode), R, 0); break;
            case 36: { /* the game moves its faders (TV play, Off-TV play, fades) */
                static const float vals[] = {0.0f, 1.0f, 0.5f, 0.25f};
                float tv = vals[rnd(4)], dv = vals[rnd(4)], tt = vals[rnd(4)], dt = vals[rnd(4)];
                uint32_t u;
                memcpy(&u, &tv, 4); wr(FADE + 0x200 + 0x194, u, 4); memcpy(&u, &tt, 4); wr(FADE + 0x200 + 0x198, u, 4); wr(FADE + 0x200 + 0x1A0, rnd(3), 4);
                memcpy(&u, &dv, 4); wr(FADE + 0x200 + 0x1AC, u, 4); memcpy(&u, &dt, 4); wr(FADE + 0x200 + 0x1B0, u, 4); wr(FADE + 0x200 + 0x1B8, rnd(3), 4);
                break;
            }
            default: { /* audio frames */
                int nf = 1 + rnd(4);
                for (int f = 0; f < nf; f++) frame();
                break;
            }
            }
            if (k <= 2) { /* remember an acquired voice (the last VPB the model handed out) */
                for (int q = 0; q < axm::kMaxVoices && nv < 8; q++)
                    if (M->voices[q].acquired) { bool have = false; for (int z = 0; z < nv; z++) have |= vpbs[z] == M->voices[q].vpb; if (!have) vpbs[nv++] = M->voices[q].vpb; }
            }
            if (fails) break;
        }
        for (int f = 0; f < 5 && !fails; f++) frame();
        what = "end of sequence (all guest memory)";
        if (!fails && !same_mem()) fails++;
        if (fails) break;
    }
    printf("ax_equiv: coverage: %ld voice-frames playing, %ld frames with a non-silent TV main bus, %ld frames in Off-TV play (GamePad aux "
           "callbacks on), %ld frames with a TV aux callback\n", st_playing, st_audible, st_drc, st_auxcb);
    printf("ax_equiv: %ld calls, %ld frames, %ld callbacks, %d difference(s)\n", nops, nframes, ncb, fails);
    return fails ? 1 : 0;
}
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port-src", default=os.environ.get("WWHD_PORT_SRC", "runtime/src"))
    ap.add_argument("--port-rev", default="f5554821", help="read the port sources at this commit (git show); '' = the working tree")
    ap.add_argument("--seqs", default="60")
    ap.add_argument("--len", default="400")
    a = ap.parse_args()

    def port_file(name):
        if a.port_rev:
            repo = os.path.dirname(os.path.dirname(os.path.abspath(a.port_src)))
            return subprocess.run(["git", "-C", repo, "show", "%s:runtime/src/%s" % (a.port_rev, name)], capture_output=True, text=True,
                                  check=True).stdout
        return open(os.path.join(a.port_src, name)).read()
    import verify  # noqa: E402
    d = tempfile.mkdtemp(prefix="ax_equiv-", dir=verify.verify_scratch_base() or None)
    os.makedirs(os.path.join(d, "hle"))
    os.makedirs(os.path.join(d, "platform"))
    ax = port_file("hle/ax.cpp")
    if ax.count("std::thread(frame_thread).detach();") != 1:
        sys.exit("ax_equiv: AXInit's frame thread start not found in ax.cpp")
    ax = ax.replace("std::thread(frame_thread).detach();", "/* (ax_equiv: no host thread; the test runs the frames) */")
    open(os.path.join(d, "hle", "ax.cpp"), "w").write(ax)
    for name in ("audio_output_mode.h", "audio_channels.h", "dsp_overload_guard.h", "game_clock.h"):
        open(os.path.join(d, name), "w").write(port_file(name))
    for name, text in (("runtime.h", RUNTIME_H), ("platform/host.h", HOST_H), ("guest_addr.h", GUEST_ADDR_H), ("audio_out.h", AUDIO_OUT_H),
                       ("savestate.h", SAVESTATE_H)):
        open(os.path.join(d, name), "w").write(text)
    # frame_thread's loop body: from service_begin() to service_end(), without keep_dsp_channels() (left out of the model,
    # ) and the locks
    m = re.search(r"threads::service_begin\(\);[^\n]*\n(.*?)\n\s*threads::service_end\(\);", ax, re.S)
    if not m:
        sys.exit("ax_equiv: frame_thread's loop body not found in ax.cpp")
    body = m.group(1)
    body = re.sub(r"^\s*keep_dsp_channels\(\);\s*$", "", body, flags=re.M)
    body = body.replace("std::lock_guard<std::mutex> lk(g_ax_mutex);", "")
    prog = PROG.replace("@@FRAME@@", body).replace("@@GAME_CLOCK@@", GAME_CLOCK_SHIM)
    cpp = os.path.join(d, "equiv.cpp")
    open(cpp, "w").write(prog)
    exe = os.path.join(d, "equiv")
    r = subprocess.run(["clang++", "-std=c++20", "-O2", "-w", "-ffp-contract=off", "-fno-strict-aliasing", "-fwrapv", "-I" + d,
                        "-I" + os.path.join(HERE, "src"), cpp, "-o", exe], capture_output=True, text=True)
    if r.returncode:
        print(r.stderr[-5000:])
        print("ax_equiv: build failed (%s)" % cpp)
        return 2
    r = subprocess.run([exe, a.seqs, a.len], capture_output=True, text=True)
    print(r.stdout.strip()[-3000:])
    print("ax_equiv: %s (port %s at %s; frame body extracted from frame_thread; program %s)" % (
        "PASS" if r.returncode == 0 else "FAIL", a.port_src, a.port_rev or "the working tree", cpp))
    return r.returncode


if __name__ == "__main__":
    sys.exit(main())

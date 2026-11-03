#!/usr/bin/env python3
"""pad_equiv.py: the `osmem ... pad` model (tools/verify/src/pad_model.h) against the shipping port's own pad HLE:
runtime/src/hle/system_stubs.cpp (the "vpad (GamePad)" section: VPADRead, tp_to_screen, VPADGetTPCalibratedPoint(Ex),
VPADControlMotor, VPADStopMotor, VPADBASEGetHeadphoneStatus) and runtime/src/hle/padscore.cpp (whole), read at port
commit f5554821 (--port-rev; '' = the working tree), with the port's own input.h, crashrec.h, rumble.h, motion/motion.h,
motion/fusion.h and motion/fusion.cpp (the virtual GamePad, compiled for real). Shims stand for what the model replaces
or leaves out (pad_model.h header): crashrec::read(pad) returns the next entry of a per-pad input list (the n-th call
for a pad gets entry n: the schedule), interp::repeat_input() false, input::pro_controller() the sequence's mode,
controller_reapply false, motion::vpad as motion.cpp with the gyro source off (VirtualPad::turn(step, {}) with a random
step, 0 at the first read, then VirtualPad::vpad()), every mods / story_* / game_settings_page / menu_sounds /
player_name / rumble hook an identity. The WWHD_*TRACE* switches are unset. WENCGetEncodeData has no HLE in the port:
core.cpp hle_unimplemented (r3 = 0) is compiled from the port's core.cpp text.

Each sequence (its own process: the HLE's statics start fresh) has a random dense input list per pad (runs of held
buttons, sticks at and around the 0.1 / 0.5 hysteresis edges, touches) and a random mode (GamePad / Pro Controller);
the model gets the same list as `padsched` ranges (runs, split at random, idle entries sometimes explicit). A random
sequence of all 19 imports with random arguments (channels 0..3 and garbage, null and unaligned buffers, counts 0, 1,
16, negative, error words inside the status buffer, touch points read back from the status buffer or random raw
words) runs on both, each side on its own copy of the same random guest memory. After every call r3 (unchanged for
void imports) and every byte of the guest window must be equal.

--inject: the self-check: each of a list of single-line bugs injected into a copy of pad_model.h must make the
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
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include "input.h"
#include "crashrec.h"
#include "rumble.h"
#include "motion/motion.h"
struct Cpu { uint32_t r[32]; uint32_t lr; };
namespace eq {
constexpr uint32_t kBase = 0x10000000u, kSize = 0x40000u;
extern uint8_t* port_mem;
inline uint8_t* at(uint8_t* m, uint32_t ea, uint32_t n) {
    if (ea - kBase >= kSize || ea - kBase + n > kSize) { fprintf(stderr, "access %08X+%u outside the window\n", ea, n); exit(3); }
    return m + (ea - kBase);
}
}
namespace mem { inline uint8_t* ptr(uint32_t ea) { return eq::at(eq::port_mem, ea, 1); } }
inline uint8_t ld8(uint32_t a) { return *eq::at(eq::port_mem, a, 1); }
inline uint16_t ld16(uint32_t a) { uint8_t* p = eq::at(eq::port_mem, a, 2); return (uint16_t)(p[0] << 8 | p[1]); }
inline uint32_t ld32(uint32_t a) { uint8_t* p = eq::at(eq::port_mem, a, 4); return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
inline float ldf32(uint32_t a) { uint32_t u = ld32(a); float f; memcpy(&f, &u, 4); return f; }
inline void st8(uint32_t a, uint8_t v) { *eq::at(eq::port_mem, a, 1) = v; }
inline void st16(uint32_t a, uint16_t v) { uint8_t* p = eq::at(eq::port_mem, a, 2); p[0] = v >> 8; p[1] = (uint8_t)v; }
inline void st32(uint32_t a, uint32_t v) { uint8_t* p = eq::at(eq::port_mem, a, 4); p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; }
inline void stf32(uint32_t a, float f) { uint32_t u; memcpy(&u, &f, 4); st32(a, u); }
inline uint32_t arg(Cpu* c, int i) { return c->r[3 + i]; }
inline void ret(Cpu* c, uint32_t v) { c->r[3] = v; }
#define HLE(lib, name) void imp_##lib##_##name(Cpu* c)
#define LOG(...) do {} while (0)
#define TRACE(...) do {} while (0)
static void log_msg(const char*, ...) {}

/* the replaced host input: crashrec::read(pad) = the pad's next list entry */
namespace eq {
std::vector<input::PadState> list[2];
size_t next[2];
bool pro = false;
std::vector<float> dts;
size_t dt_next = 0;
}
namespace crashrec {
input::PadState read(int pad) {
    size_t k = eq::next[pad]++;
    return k < eq::list[pad].size() ? eq::list[pad][k] : input::PadState{};
}
}
namespace input {
bool pro_controller() { return eq::pro; }
PadState read() { fprintf(stderr, "input::read on a repeated pass\n"); exit(3); }
}
namespace interp {
bool repeat_input() { return false; }
bool fresh_sticks() { return false; }
void trace_read(const char*) {}
uint64_t logic_steps() { return 0; }
}
namespace controller_reapply {
bool holds_pro_controller() { return false; }
bool pro_controller_live() { return false; }
bool holds_gamepad() { return false; }
}
namespace mods {
void one_screen_pad(input::PadState&) {}
void pad_play_rules(input::PadState&) {}
void item_ring_pad(input::PadState&, bool) {}
void context_touch_pad(input::PadState&, bool) {}
void touch_talk_pad(input::PadState&, bool) {}
void guest_pad_filter(Cpu*, input::PadState&, bool) {}
void moon_jump_input(const input::PadState&) {}
void move_speed_input(uint32_t) {}
void jump_input(uint32_t) {}
void fast_forward_input(uint32_t&) {}
}
namespace story_notebook { void pad(input::PadState&, Cpu*) {} void mute(input::PadState&) {} }
namespace story_scenes { void mute(input::PadState&) {} }
namespace story_bag { void pad(input::PadState&, Cpu*) {} void mute(input::PadState&) {} }
namespace game_settings_page { void pad(input::PadState&, Cpu*) {} void mute(input::PadState&) {} }
namespace menu_sounds { void service(Cpu*) {} }
namespace player_name { void service(Cpu*) {} }
namespace rumble {
void gamepad_pattern(uint32_t, const uint8_t*, uint32_t) {}
void gamepad_stop(uint32_t) {}
void pro_motor(uint32_t, bool) {}
}
/* motion.cpp vpad() with the gyro source off: no test turns, no mouse, no device; the step is the time since the last read */
namespace motion {
static VirtualPad g_pad;
static VpadMotion g_last;
VpadMotion vpad(bool repeat) {
    if (repeat) return g_last;
    float dt = eq::dt_next < eq::dts.size() ? eq::dts[eq::dt_next++] : 0.0f;
    Aim aim;
    float step = std::min(dt, 0.1f);
    g_pad.turn(step, aim);
    g_last = g_pad.vpad();
    return g_last;
}
void right_stick(float, float) {}
}
"""

PROG = r"""
#include "pad_model.h"
#include <sys/wait.h>
#include <unistd.h>

uint8_t* eq::port_mem;
static uint8_t* model_mem;
static uint64_t rs;
static uint32_t rnd32() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return (uint32_t)(rs >> 11); }
static uint32_t rnd(uint32_t n) { return n ? rnd32() % n : 0; }

struct MEnv {
    uint16_t ld16(uint32_t a) { uint8_t* p = eq::at(model_mem, a, 2); return (uint16_t)(p[0] << 8 | p[1]); }
    void st8(uint32_t a, uint32_t v) { *eq::at(model_mem, a, 1) = (uint8_t)v; }
    void st16(uint32_t a, uint32_t v) { uint8_t* p = eq::at(model_mem, a, 2); p[0] = (uint8_t)(v >> 8); p[1] = (uint8_t)v; }
    void st32(uint32_t a, uint32_t v) { uint8_t* p = eq::at(model_mem, a, 4); p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = (uint8_t)v; }
    void zero(uint32_t a, uint32_t n) { memset(eq::at(model_mem, a, n), 0, n); }
};

#define PORT_IMPORTS(X) X(vpad, VPADControlMotor) X(vpad, VPADGetTPCalibratedPoint) X(vpad, VPADRead) X(vpad, VPADStopMotor) \
    X(vpadbase, VPADBASEGetHeadphoneStatus) X(padscore, KPADGetMplsWorkSize) X(padscore, KPADInitEx) X(padscore, KPADReadEx) \
    X(padscore, KPADSetMplsWorkarea) X(padscore, WENCGetEncodeData) X(padscore, WPADCanSendStreamData) X(padscore, WPADControlMotor) \
    X(padscore, WPADControlSpeaker) X(padscore, WPADDisconnect) X(padscore, WPADEnableURCC) X(padscore, WPADEnableWiiRemote) \
    X(padscore, WPADGetBatteryLevel) X(padscore, WPADProbe) X(padscore, WPADSendStreamData)
#define DECL(l, n) void imp_##l##_##n(Cpu* c);
PORT_IMPORTS(DECL)
static void port_call(uint32_t t, Cpu* c) {
#define CASE(l, n) if (t == padm::n) { imp_##l##_##n(c); return; }
    PORT_IMPORTS(CASE)
    fprintf(stderr, "no port import %08X\n", t);
    exit(3);
}

static float stick() {
    static const float edge[] = {0.0f, 0.1f, -0.1f, 0.5f, -0.5f, 0.0999999f, -0.0999999f, 0.4999999f, -0.4999999f, 0.1000001f, 1.0f, -1.0f, 0.7f, -0.3f};
    if (rnd(3)) return edge[rnd(sizeof edge / sizeof edge[0])];
    return ((int)rnd(2000001) - 1000000) / 1000000.0f;
}
static input::PadState rand_in() {
    input::PadState p;
    static const uint32_t bits[] = {padm::kA, padm::kB, padm::kX, padm::kY, padm::kL, padm::kR, padm::kZL, padm::kZR, padm::kPlus,
        padm::kMinus, padm::kHome, padm::kUp, padm::kDown, padm::kLeft, padm::kRight, padm::kStickL, padm::kStickR};
    int nb = rnd(4);
    for (int i = 0; i < nb; i++) p.buttons |= bits[rnd(17)];
    if (rnd(2)) { p.lx = stick(); p.ly = stick(); }
    if (rnd(3) == 0) { p.rx = stick(); p.ry = stick(); }
    if (rnd(3) == 0) { p.touch = true; p.tx = rnd(1000001) / 1000000.0f; p.ty = rnd(1000001) / 1000000.0f; if (rnd(4) == 0) p.tx = (float)rnd(2); }
    return p;
}
static bool same(const input::PadState& a, const input::PadState& b) {
    return a.buttons == b.buttons && !memcmp(&a.lx, &b.lx, 4) && !memcmp(&a.ly, &b.ly, 4) && !memcmp(&a.rx, &b.rx, 4) &&
           !memcmp(&a.ry, &b.ry, 4) && a.touch == b.touch && !memcmp(&a.tx, &b.tx, 4) && !memcmp(&a.ty, &b.ty, 4);
}
static padm::PadIn to_model(const input::PadState& p) {
    padm::PadIn m;
    m.buttons = p.buttons; m.lx = p.lx; m.ly = p.ly; m.rx = p.rx; m.ry = p.ry; m.touch = p.touch; m.tx = p.tx; m.ty = p.ty;
    return m;
}
/* the dense list as padsched ranges: runs of equal entries, split at random; idle entries left out or explicit */
static bool to_sched(const std::vector<input::PadState>& L, padm::Sched& s) {
    size_t i = 0;
    while (i < L.size()) {
        size_t j = i + 1;
        while (j < L.size() && same(L[j], L[i]) && rnd(4)) j++;
        bool idle = same(L[i], input::PadState{});
        if (!idle || rnd(2))
            if (!s.add(i, j - 1, to_model(L[i]))) return false;
        i = j;
    }
    return true;
}

struct Stats { long calls = 0, reads = 0, kreads = 0, early = 0, held = 0, trig = 0, stickbits = 0, touch = 0, tp = 0; } st;

static int run_seq(uint64_t seed, int len) {
    rs = seed * 0x9E3779B97F4A7C15ull + 1;
    for (int i = 0; i < 8; i++) rnd32();
    eq::pro = rnd(3) == 0;
    padm::Sched sch;
    for (int pad = 0; pad < 2; pad++) {
        int n = len / 2 + rnd(len);
        eq::list[pad].clear();
        while ((int)eq::list[pad].size() < n) {
            input::PadState p = rnd(3) ? rand_in() : input::PadState{};
            int run = 1 + rnd(rnd(4) ? 3 : 20);
            for (int k = 0; k < run; k++) eq::list[pad].push_back(p);
        }
    }
    /* one schedule for both readers (the spec has one): the second pad's list is the first one's */
    eq::list[1] = eq::list[0];
    if (!to_sched(eq::list[0], sch)) { printf("schedule overlap (test bug)\n"); return 1; }
    for (int i = 0; i < len * 2; i++) eq::dts.push_back(i == 0 ? 0.0f : rnd(5) == 0 ? 0.0f : rnd(200000) / 1000000.0f);
    eq::port_mem = (uint8_t*)malloc(eq::kSize);
    model_mem = (uint8_t*)malloc(eq::kSize);
    for (uint32_t i = 0; i < eq::kSize; i++) eq::port_mem[i] = (uint8_t)rnd32();
    memcpy(model_mem, eq::port_mem, eq::kSize);
    padm::State S;
    MEnv e;
    uint32_t last_vpad_buf = 0;
    static const uint32_t imps[] = {padm::VPADRead, padm::VPADRead, padm::VPADRead, padm::VPADRead, padm::KPADReadEx, padm::KPADReadEx,
        padm::KPADReadEx, padm::VPADGetTPCalibratedPoint, padm::VPADGetTPCalibratedPoint, padm::WPADProbe, padm::WPADProbe,
        padm::VPADControlMotor, padm::VPADStopMotor, padm::VPADBASEGetHeadphoneStatus, padm::KPADGetMplsWorkSize, padm::KPADInitEx,
        padm::KPADSetMplsWorkarea, padm::WENCGetEncodeData, padm::WPADCanSendStreamData, padm::WPADControlMotor,
        padm::WPADControlSpeaker, padm::WPADDisconnect, padm::WPADEnableURCC, padm::WPADEnableWiiRemote, padm::WPADGetBatteryLevel,
        padm::WPADSendStreamData};
    auto buf = [&](uint32_t size) {
        uint32_t a = eq::kBase + 0x1000 + rnd(eq::kSize - 0x2000 - size);
        return rnd(8) ? a & ~3u : a;
    };
    auto chan = [&]() { int k = rnd(20); return k < 17 ? 0u : k < 19 ? 1 + rnd(3) : rnd32(); };
    auto count = [&]() {
        static const uint32_t c[] = {16, 16, 16, 1, 1, 2, 0, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu};
        return rnd(10) ? c[rnd(rnd(4) ? 6 : 10)] : rnd32();
    };
    for (int op = 0; op < len; op++) {
        uint32_t t = imps[rnd(sizeof imps / sizeof imps[0])];
        uint32_t a[11];
        for (int i = 0; i < 11; i++) a[i] = rnd32();
        if (t == padm::VPADRead || t == padm::KPADReadEx) {
            uint32_t size = t == padm::VPADRead ? 16 * 0xAC : 0xF08;
            a[3] = chan();
            a[4] = rnd(20) ? buf(size) : 0;
            a[5] = count();
            int k = rnd(10);
            a[6] = k < 2 ? 0 : k < 4 && a[4] ? a[4] + 4 * rnd(0x40) : buf(4);
            if (t == padm::VPADRead && a[4]) last_vpad_buf = a[4];
        } else if (t == padm::VPADGetTPCalibratedPoint) {
            a[3] = 0;
            a[4] = buf(8);
            a[5] = last_vpad_buf && rnd(3) ? last_vpad_buf + 0x52 + 8 * rnd(4) : buf(8);
            st.tp++;
        } else if (t == padm::WPADProbe) {
            a[3] = chan();
            a[4] = rnd(4) ? buf(4) : 0;
        } else if (t == padm::VPADControlMotor) {
            a[3] = rnd(2); a[4] = rnd(4) ? buf(16) : 0; a[5] = rnd(256);
        }
        Cpu c = {};
        for (int i = 0; i < 11; i++) c.r[i] = a[i];
        uint64_t vr = S.vpad_reads, kr = S.kpad_reads;
        port_call(t, &c);
        uint32_t r = 0;
        int k = padm::call(e, S, sch, eq::pro, t, a, &r);
        uint32_t mr3 = k == 1 ? r : a[3];
        st.calls++;
        if (S.vpad_reads != vr || S.kpad_reads != kr) {
            (S.vpad_reads != vr ? st.reads : st.kreads)++;
            uint32_t h = S.vpad_reads != vr ? S.vpad_last_hold : S.kpad_last;
            if (h) st.held++;
            if (S.vpad_reads != vr && (h & 0xFFF00000u)) st.stickbits++;
            uint32_t b = a[4] + (S.vpad_reads != vr ? 4 : 0x64);
            if (b - eq::kBase < eq::kSize - 4 && (model_mem[b - eq::kBase] | model_mem[b - eq::kBase + 1] | model_mem[b - eq::kBase + 2] | model_mem[b - eq::kBase + 3])) st.trig++;
            if (S.vpad_reads != vr && eq::list[0][std::min<size_t>(vr, eq::list[0].size() - 1)].touch && vr < eq::list[0].size()) st.touch++;
        } else if (t == padm::VPADRead || t == padm::KPADReadEx) st.early++;
        if (k < 0) { printf("seed %llu op %d: the model does not answer %08X\n", (unsigned long long)seed, op, t); return 1; }
        if (mr3 != c.r[3]) {
            printf("seed %llu op %d %s: r3 port %08X model %08X (args %08X %08X %08X %08X)\n", (unsigned long long)seed, op,
                   padm::import_name(t), c.r[3], mr3, a[3], a[4], a[5], a[6]);
            return 1;
        }
        if (memcmp(eq::port_mem, model_mem, eq::kSize)) {
            uint32_t i = 0;
            while (eq::port_mem[i] == model_mem[i]) i++;
            printf("seed %llu op %d %s (pro %d, args %08X %08X %08X %08X): guest byte %08X port %02X model %02X\n", (unsigned long long)seed,
                   op, padm::import_name(t), (int)eq::pro, a[3], a[4], a[5], a[6], eq::kBase + i, eq::port_mem[i], model_mem[i]);
            return 1;
        }
        if (S.vpad_reads != eq::next[0] || S.kpad_reads != eq::next[1]) {
            printf("seed %llu op %d %s: read ordinals port %zu/%zu model %llu/%llu\n", (unsigned long long)seed, op, padm::import_name(t),
                   eq::next[0], eq::next[1], (unsigned long long)S.vpad_reads, (unsigned long long)S.kpad_reads);
            return 1;
        }
    }
    printf("STATS %ld %ld %ld %ld %ld %ld %ld %ld %ld %d\n", st.calls, st.reads, st.kreads, st.early, st.held, st.trig, st.stickbits, st.touch,
           st.tp, (int)eq::pro);
    return 0;
}

int main(int argc, char** argv) {
    int seqs = argc > 1 ? atoi(argv[1]) : 200, len = argc > 2 ? atoi(argv[2]) : 400;
    uint64_t base = argc > 3 ? strtoull(argv[3], nullptr, 0) : 1;
    int fails = 0;
    long tot[10] = {};
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
        long v[10];
        if (WIFEXITED(ws) && WEXITSTATUS(ws) == 0 &&
            sscanf(out.c_str(), "STATS %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], &v[9]) == 10) {
            for (int i = 0; i < 10; i++) tot[i] += v[i];
        } else {
            fails++;
            printf("%s", out.c_str());
            if (!WIFEXITED(ws) || WEXITSTATUS(ws) != 1) printf("seed %llu: the sequence ended abnormally (status %d)\n", (unsigned long long)(base + q), ws);
            if (fails >= 3) break;
        }
    }
    printf("pad_equiv: %d sequences (%ld in Pro Controller mode), %ld calls: %ld VPADRead and %ld KPADReadEx reads of input, %ld early "
           "returns, %ld reads with buttons held, %ld with a trigger, %ld with stick-as-button bits, %ld touches, %ld "
           "VPADGetTPCalibratedPoint; %d difference(s)\n", seqs, tot[9], tot[0], tot[1], tot[2], tot[3], tot[4], tot[5], tot[6], tot[7], tot[8], fails);
    return fails ? 1 : 0;
}
"""

# single-line bugs for --inject: (description, old text in pad_model.h, new text); each must be detected
INJECT = [
    ("trig without the previous hold", "e.st32(st + 0x04, hold & ~last_hold); /* trig */", "e.st32(st + 0x04, hold); /* trig */"),
    ("release from the new hold", "e.st32(st + 0x08, last_hold & ~hold); /* release */", "e.st32(st + 0x08, ~last_hold & hold); /* release */"),
    ("stick hysteresis edge", "if (v >= 0.5f || (held && v >= 0.1f)) hold |= bit;", "if (v >= 0.5f || (held && v > 0.1f)) hold |= bit;"),
    ("stick press edge", "if (v >= 0.5f || (held && v >= 0.1f)) hold |= bit;", "if (v > 0.5f || (held && v >= 0.1f)) hold |= bit;"),
    ("right stick directions swapped", "stick_dirs(p.rx, p.ry, 0x01000000, 0x00800000, 0x04000000, 0x02000000);",
     "stick_dirs(p.rx, p.ry, 0x01000000, 0x00800000, 0x02000000, 0x04000000);"),
    ("VPADStatus clear too short", "e.zero(st, 0xAC);", "e.zero(st, 0xA8);"),
    ("battery level byte", "e.st8(st + 0xA1, 0xC0);", "e.st8(st + 0xA1, 0xC1);"),
    ("touch x offset", "S.last_tx = (uint16_t)(p.tx * 3883.0f + 92.0f);", "S.last_tx = (uint16_t)(p.tx * 3883.0f + 93.0f);"),
    ("touch validity", "e.st16(st + tp + 6, p.touch ? 0 : 3);", "e.st16(st + tp + 6, p.touch ? 0 : 2);"),
    ("touch kept without a touch", "if (p.touch) { /* touch panel", "if (true) { /* touch panel"),
    ("read ordinal off by one", "PadIn p = sch.at(S.vpad_reads++);", "PadIn p = sch.at(++S.vpad_reads);"),
    ("early return consumes a read", "/* VPAD_READ_INVALID_CONTROLLER */\n            *r = 0;", "/* VPAD_READ_INVALID_CONTROLLER */\n            *r = 0; S.vpad_reads++;"),
    ("VPAD error code", "if (err) e.st32(err, (uint32_t)-2); /* VPAD_READ_INVALID_CONTROLLER */", "if (err) e.st32(err, (uint32_t)-1);"),
    ("VPAD count check unsigned", "if (chan != 0 || !st || (int32_t)count <= 0) {\n            if (err) e.st32(err, (uint32_t)-2)",
     "if (chan != 0 || !st || count == 0) {\n            if (err) e.st32(err, (uint32_t)-2)"),
    ("GamePad buttons kept in Pro Controller mode", "            p.buttons = 0;\n", "            p.buttons &= ~0u;\n"),
    ("rest pose: dir z", "m.dir[2] = sw(q.rotate({0, 0, -1}));", "m.dir[2] = sw(q.rotate({0, 0, 1}));"),
    ("rest pose: acc sign", "m.acc = {-d.x, -d.y, d.z};", "m.acc = {d.x, -d.y, d.z};"),
    ("rest pose: acc_xy", "if (xy > 0.1f) { m.acc_xy[0] = -m.acc.y / xy; m.acc_xy[1] = m.acc.x / xy; }", "if (xy > 0.1f) { m.acc_xy[0] = m.acc.y / xy; m.acc_xy[1] = m.acc.x / xy; }"),
    ("tp_to_screen y scale", "e.st16(out + 2, (uint16_t)(y / 3694.0 * h));", "e.st16(out + 2, (uint16_t)(y / 3695.0 * h));"),
    ("tp_to_screen x offset", "int x = std::max<int>(e.ld16(raw) - 92, 0)", "int x = std::max<int>(e.ld16(raw) - 91, 0)"),
    ("WPADProbe any channel", "inline bool connected(bool pro, uint32_t chan) { return chan == 0 && pro; }",
     "inline bool connected(bool pro, uint32_t chan) { return pro; }"),
    ("WPADProbe no-controller type", "if (type) e.st32(type, on ? kDevURCC : kDevNone);", "if (type) e.st32(type, on ? kDevURCC : 0);"),
    ("KPAD devType", "e.st8(st + 0x5C, kDevURCC);    /* devType */", "e.st8(st + 0x5C, kDevNone);    /* devType */"),
    ("KPAD error when connected", "if (err) e.st32(err, (uint32_t)(con ? kKpadErrNone : kKpadErrNoController));",
     "if (err) e.st32(err, (uint32_t)kKpadErrNoController);"),
    ("KPAD button map", "{kA, 0x10},", "{kA, 0x20},"),
    ("KPAD clear too short", "e.zero(st, 0xF0);", "e.zero(st, 0xE0);"),
    ("KPAD stick order", "stf32(st + 0x6C, p.lx); stf32(st + 0x70, p.ly); /* lstick */", "stf32(st + 0x6C, p.ly); stf32(st + 0x70, p.lx); /* lstick */"),
    ("WPADSendStreamData result", "case WPADSendStreamData: case WPADControlSpeaker: *r = (uint32_t)kWpadErrNoController; return 1;",
     "case WPADSendStreamData: case WPADControlSpeaker: *r = 0; return 1;"),
    ("battery level answer", "case WPADGetBatteryLevel: *r = 4; return 1;", "case WPADGetBatteryLevel: *r = 3; return 1;"),
    ("headphone answer", "case VPADBASEGetHeadphoneStatus: *r = 0; return 1;", "case VPADBASEGetHeadphoneStatus: *r = 1; return 1;"),
    ("WENCGetEncodeData void", "case WENCGetEncodeData: *r = 0; return 1; /* hle_unimplemented */", "case WENCGetEncodeData: return 0;"),
    ("Mpls work size", "case KPADGetMplsWorkSize: *r = 0x5FE0; return 1;", "case KPADGetMplsWorkSize: *r = 0x5FE4; return 1;"),
]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port-src", default=os.environ.get("WWHD_PORT_SRC", "runtime/src"))
    ap.add_argument("--port-rev", default="f5554821", help="read the port sources at this commit (git show); '' = the working tree")
    ap.add_argument("--seqs", default="300")
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
    d = tempfile.mkdtemp(prefix="pad_equiv-", dir=verify.verify_scratch_base() or None)
    os.makedirs(os.path.join(d, "motion"))
    for name in ("input.h", "crashrec.h", "rumble.h"):
        open(os.path.join(d, name), "w").write(port_file(name))
    for name in ("motion/motion.h", "motion/fusion.h", "motion/fusion.cpp"):
        open(os.path.join(d, name), "w").write(port_file(name))
    stubs = port_file("hle/system_stubs.cpp")
    m = re.search(r"^// ---- vpad \(GamePad\).*?$(.*?)^// ---- padscore", stubs, re.S | re.M)
    if not m:
        sys.exit("pad_equiv: the vpad section not found in system_stubs.cpp")
    vpad = m.group(1)
    for fn in ("HLE(vpad, VPADRead)", "tp_to_screen", "HLE(vpad, VPADGetTPCalibratedPoint)", "HLE(vpad, VPADControlMotor)",
               "HLE(vpad, VPADStopMotor)", "HLE(vpadbase, VPADBASEGetHeadphoneStatus)"):
        if fn not in vpad:
            sys.exit("pad_equiv: %s not in the vpad section" % fn)
    pads = port_file("hle/padscore.cpp")
    pads = "\n".join(l for l in pads.split("\n") if not l.lstrip().startswith("#include"))
    core = port_file("core.cpp")
    m = re.search(r'extern "C" void hle_unimplemented\(Cpu\* c, const char\* lib, const char\* name\) \{.*?\n\}', core, re.S)
    if not m or "c->r[3] = 0;" not in m.group(0):
        sys.exit("pad_equiv: core.cpp hle_unimplemented (r3 = 0) not found")
    unimpl = "HLE(padscore, WENCGetEncodeData) { const char* lib = \"padscore\"; const char* name = \"WENCGetEncodeData\"; (void)lib; (void)name;\n" + \
        re.sub(r"^.*?\{", "", m.group(0), count=1, flags=re.S).replace("static std::mutex m;", "").replace(
            "std::lock_guard<std::mutex> lk(m);", "").replace("static std::unordered_map<std::string, int> seen;", "static std::map<std::string, int> seen;")
    port_cpp = os.path.join(d, "port.cpp")
    open(port_cpp, "w").write(PRELUDE + "\n// ---- system_stubs.cpp vpad section (f5554821)\n" + vpad + "\n// ---- padscore.cpp\n" + pads +
                              "\n// ---- core.cpp hle_unimplemented for WENCGetEncodeData\n" + unimpl + "\n")
    prog_cpp = os.path.join(d, "equiv.cpp")
    open(prog_cpp, "w").write(PRELUDE.replace("namespace eq {\nstd::vector", "namespace eq {\nextern std::vector").replace(
        "size_t next[2];", "extern size_t next[2];").replace("bool pro = false;", "extern bool pro;").replace(
        "std::vector<float> dts;", "extern std::vector<float> dts;").replace("size_t dt_next = 0;", "extern size_t dt_next;").split(
        "namespace crashrec {")[0] + PROG)
    flags = ["clang++", "-std=c++20", "-O2", "-w", "-ffp-contract=off", "-fno-strict-aliasing", "-fwrapv", "-I" + d]

    def build(model_dir, exe):
        objs = []
        for src in (port_cpp, os.path.join(d, "motion", "fusion.cpp")):
            o = os.path.join(d, os.path.basename(src) + ".o")
            if not os.path.exists(o):
                r = subprocess.run(flags + ["-c", src, "-o", o], capture_output=True, text=True)
                if r.returncode:
                    print(r.stderr[-5000:])
                    return False
            objs.append(o)
        r = subprocess.run(flags + ["-I" + model_dir, prog_cpp] + objs + ["-o", exe], capture_output=True, text=True)
        if r.returncode:
            print(r.stderr[-5000:])
        return r.returncode == 0

    env = {k: v for k, v in os.environ.items() if "TRACE" not in k}
    model_src = open(os.path.join(HERE, "src", "pad_model.h")).read()
    exe = os.path.join(d, "equiv")
    if not build(os.path.join(HERE, "src"), exe):
        print("pad_equiv: build failed (%s)" % d)
        return 2
    r = subprocess.run([exe, a.seqs, a.len, a.seed], capture_output=True, text=True, env=env)
    print(r.stdout.strip()[-3000:])
    ok = r.returncode == 0
    print("pad_equiv: %s (port %s at %s; programs in %s)" % ("PASS" if ok else "FAIL", a.port_src, a.port_rev or "the working tree", d))
    if not a.inject:
        return 0 if ok else 1
    missed = []
    for i, (what, old, new) in enumerate(INJECT):
        if model_src.count(old) != 1:
            print("pad_equiv: inject %d (%s): the text occurs %d times in pad_model.h" % (i, what, model_src.count(old)))
            missed.append(what)
            continue
        md = os.path.join(d, "inject%02d" % i)
        os.makedirs(md, exist_ok=True)
        open(os.path.join(md, "pad_model.h"), "w").write(model_src.replace(old, new))
        ie = os.path.join(md, "equiv")
        if not build(md, ie):
            missed.append(what + " (build failed)")
            continue
        r = subprocess.run([ie, "60", a.len, a.seed], capture_output=True, text=True, env=env)
        first = next((l for l in r.stdout.splitlines() if l.startswith("seed ")), "")
        det = r.returncode == 1 and first
        print("pad_equiv: inject %2d %-45s %s" % (i, what, ("detected: " + first[:150]) if det else "NOT DETECTED (rc %d)" % r.returncode))
        if not det:
            missed.append(what)
    print("pad_equiv: injected bugs: %d of %d detected" % (len(INJECT) - len(missed), len(INJECT)))
    return 0 if ok and not missed else 1


if __name__ == "__main__":
    sys.exit(main())

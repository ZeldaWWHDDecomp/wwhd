#!/usr/bin/env python3
"""guestthreads_equiv.py: the `osmem ... threads` object model (tools/verify/src/gthr_model.h, composed as the operations in
src/gthr_sched.inc compose it) against the shipping port's own code in runtime/src/threads.cpp (read only; --port-src,
default $WWHD_PORT_SRC or runtime/src (the port checkout the harness runs in)).

Read at port commit f5554821 (--port-rev; OSWaitRendezvous per core since aaa5fcde) and compiled with shim headers (no host threads): namespace osthread, core_from_affinity,
create_thread's guest stores, ObjTable, HMutex + mutex_lock/trylock/unlock, HEvent + event_wait, HQueue, and the HLE
bodies OSInitMutex/OSLockMutex/OSTryLockMutex/OSUnlockMutex, OSInitEvent/OSSignalEvent/OSResetEvent/OSWaitEvent,
OSInitMessageQueue/OSSendMessage/OSReceiveMessage, OSInitRendezvous/OSWaitRendezvous (rendezvous.h RendezvousCores), OSSetThreadSpecific/
OSGetThreadSpecific. The shim park_wait throws when its predicate is false (the caller would block), so the port's state
at that point (an event waiter already registered, a rendezvous arrival already counted, nothing else changed) is
compared with the model's at the point where the scheduler blocks the thread.

Seeded random operation sequences over 4 threads and a few objects of each kind; a thread whose operation would block is
parked and gets no further operations until its wait ends on both sides (a woken event waiter, a free mutex, a
non-empty/non-full queue, a rendezvous whose mask cores have all arrived; the four threads run on cores 1, 0, 2, 0), then the parked operation is completed as both implementations
complete it after their wait. After every operation: the result register, the would-block decision, every guest byte
written, the queue contents, mutex owner/count, event signaled/auto/waiter list/wake tokens and rendezvous counts must be
equal. Plus core_from_affinity for every mask and fallback, and create_thread's OSThread words for random arguments.
Exit 0 on PASS."""
import argparse
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))


def block(src, pat):
    m = re.search(pat, src, re.M)
    if not m:
        sys.exit("guestthreads_equiv: pattern not found in threads.cpp: " + pat)
    i = src.index("{", m.start())
    depth = 0
    for j in range(i, len(src)):
        if src[j] == "{":
            depth += 1
        elif src[j] == "}":
            depth -= 1
            if depth == 0:
                end = j + 1
                if src[end:end + 1] == ";":
                    end += 1
                return src[m.start():end]
    sys.exit("unbalanced " + pat)


PROG = r"""
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <array>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "gthr_model.h"

static uint8_t g_pmem[0x20000], g_mmem[0x20000]; /* port / model guest memory (small addresses) */
struct WouldBlock {};
namespace port {
struct Cpu { uint32_t r[32]; };
struct HostThread { bool woken = false; int id = 0; uint32_t core = 1; };
static HostThread g_ht[4];
static HostThread* t_self = nullptr;
static thread_local Cpu* t_cpu = nullptr;
static bool g_trace_msg = false;
inline uint32_t ld32(uint32_t ea) { uint32_t v; memcpy(&v, g_pmem + ea, 4); return __builtin_bswap32(v); }
inline void st32(uint32_t ea, uint32_t v) { v = __builtin_bswap32(v); memcpy(g_pmem + ea, &v, 4); }
inline void st16(uint32_t ea, uint16_t v) { v = __builtin_bswap16(v); memcpy(g_pmem + ea, &v, 2); }
inline void st8(uint32_t ea, uint8_t v) { g_pmem[ea] = v; }
namespace mem { inline uint8_t* ptr(uint32_t ea) { return g_pmem + ea; } inline uint32_t runtime_alloc(uint32_t, uint32_t = 0) { return 0; } }
static std::chrono::nanoseconds ticks_to_ns(uint64_t t) { return std::chrono::nanoseconds((int64_t)t); }
inline uint32_t arg(Cpu* c, int i) { return c->r[3 + i]; }
inline uint64_t arg64(Cpu* c, int r) { return (uint64_t)c->r[r] << 32 | c->r[r + 1]; }
inline void ret(Cpu* c, uint32_t v) { c->r[3] = v; }
namespace threads { inline uint32_t current_thread() { return t_self ? 0x10000u + 0x800u * (uint32_t)t_self->id : 0; } }
#define HLE(lib, name) void imp_##lib##_##name(Cpu* c)
#define LOG(...) do {} while (0)
#define TRACE(...) do {} while (0)
@@waitkind@@
static uint64_t g_now = 0; /* the test's clock (host ticks = guest ticks: no save-state offset) */
namespace timebase { static const uint64_t kTicksPerSec = 62156250ull; inline uint64_t simulation_now() { return g_now; } inline uint64_t to_host(uint64_t t) { return t; } }
template <class Pred>
static bool park_wait(std::unique_lock<std::mutex>&, std::condition_variable&, Pred pred, uint8_t, uint32_t,
                      const std::chrono::steady_clock::time_point* = nullptr, const uint64_t* = nullptr) {
    if (!pred()) throw WouldBlock{};
    return true;
}
@@rendezvous@@
@@osthread@@
@@core@@
static void create_words(uint32_t t, uint32_t entry, uint32_t stack_top, uint32_t stack_size, int prio, uint32_t attr, uint16_t& g_next_id) {
@@create@@
}
@@objtable@@
@@body@@
/* alarms (extracted below the `// --- alarms` banner: struct Alarm, the queue, arm_alarm, the alarm HLE bodies; the
 * alarm host thread is not started) and the os_time OSTime -> guest timebase conversion */
struct { void notify() {} } g_tick;
namespace os_time { inline int64_t bias() { return gthrm::kFixedOSTime; }
@@to_guest@@
}
@@alarms@@
static bool port_fire(uint64_t now, Alarm* out) { /* alarm_thread's dequeue: drop stale heads, fire a due one */
    for (;;) {
        if (g_alarm_queue.empty()) return false;
        auto it = g_alarm_queue.begin();
@@stale@@
        if (it->first > now) return false;
@@rearm@@
        *out = a;
        return true;
    }
}
}  // namespace port

/* the model side, composed as gthr_sched.inc's op_* functions compose gthr_model.h */
namespace model {
using namespace gthrm;
static std::map<uint32_t, Queue> q;
static std::map<uint32_t, Mutex> mx;
static std::map<uint32_t, Event> ev;
static std::map<uint32_t, Rendezvous> rdv;
static bool woken[4];
inline void put(uint32_t a, int size, uint32_t v) { for (int j = 0; j < size; j++) g_mmem[a + j] = (uint8_t)(v >> (8 * (size - 1 - j))); }
inline uint32_t w32(uint32_t a) { return (uint32_t)g_mmem[a] << 24 | (uint32_t)g_mmem[a + 1] << 16 | (uint32_t)g_mmem[a + 2] << 8 | g_mmem[a + 3]; }
}

static uint64_t rs;
static uint32_t rnd(uint32_t n) { rs = rs * 6364136223846793005ull + 1442695040888963407ull; return (uint32_t)((rs >> 33) % n); }
static const uint32_t QS[2] = {0x100, 0x140}, MS[2] = {0x200, 0x240}, ES[2] = {0x300, 0x340}, RS_[2] = {0x400, 0x440};
static const uint32_t BUF = 0x1000;
static const uint32_t kCore[4] = {1, 0, 2, 0}; /* the four test threads' cores (two on core 0: one arrival) */

enum Kind { K_NONE, K_LOCK, K_SEND, K_RECV, K_WAIT, K_RDV };
struct Park { Kind k = K_NONE; uint32_t obj = 0, a1 = 0, a2 = 0; };
static Park park[4];
static long nops = 0, nblock = 0, nresume = 0;
static int fails = 0;

static bool port_pred(int t) {
    const Park& p = park[t];
    switch (p.k) {
    case K_LOCK: { auto* m = port::g_mutexes.get(p.obj); return m->owner == nullptr || m->owner == &port::g_ht[t]; }
    case K_SEND: { auto* h = port::g_queues.get(p.obj); return h->msgs.size() < h->capacity; }
    case K_RECV: return !port::g_queues.get(p.obj)->msgs.empty();
    case K_WAIT: return port::g_ht[t].woken;
    case K_RDV: return port::g_rdv.get(p.obj)->cores.complete(p.a1);
    default: return true;
    }
}
static bool model_pred(int t) {
    using namespace gthrm;
    const Park& p = park[t];
    switch (p.k) {
    case K_LOCK: return m_can_lock(model::mx[p.obj], t);
    case K_SEND: return !q_full(model::q[p.obj]);
    case K_RECV: return !q_empty(model::q[p.obj]);
    case K_WAIT: return model::woken[t];
    case K_RDV: return rdv_complete(model::rdv[p.obj], p.a1);
    default: return true;
    }
}

static void compare(const char* what, uint64_t seed) {
    char why[256] = "";
    if (memcmp(g_pmem, g_mmem, sizeof g_pmem)) {
        for (size_t i = 0; i < sizeof g_pmem; i++)
            if (g_pmem[i] != g_mmem[i]) { snprintf(why, sizeof why, "guest byte %05zX: port %02X model %02X", i, g_pmem[i], g_mmem[i]); break; }
    }
    for (uint32_t a : QS) {
        auto* h = port::g_queues.get(a);
        auto& m = model::q[a];
        if (h->capacity != m.capacity || h->msgs.size() != m.msgs.size()) snprintf(why, sizeof why, "queue %X size/capacity", a);
        else for (size_t i = 0; i < m.msgs.size(); i++) if (h->msgs[i] != m.msgs[i]) snprintf(why, sizeof why, "queue %X message %zu", a, i);
    }
    for (uint32_t a : MS) {
        auto* h = port::g_mutexes.get(a);
        auto& m = model::mx[a];
        int po = h->owner ? (int)((const port::HostThread*)h->owner - port::g_ht) : -1;
        if (po != m.owner || (h->owner && h->count != m.count)) snprintf(why, sizeof why, "mutex %X owner %d/%d count %d/%d", a, po, m.owner, h->count, m.count);
    }
    for (uint32_t a : ES) {
        auto* h = port::g_events.get(a);
        auto& m = model::ev[a];
        if (h->signaled != m.signaled || h->auto_reset != m.auto_reset || h->waiters.size() != m.waiters.size()) snprintf(why, sizeof why, "event %X state", a);
        else for (size_t i = 0; i < m.waiters.size(); i++) if (h->waiters[i] - port::g_ht != m.waiters[i]) snprintf(why, sizeof why, "event %X waiter %zu", a, i);
    }
    for (int t = 0; t < 4; t++) if (park[t].k == K_WAIT && port::g_ht[t].woken != model::woken[t]) snprintf(why, sizeof why, "wake token of T%d", t);
    for (uint32_t a : RS_) if (port::g_rdv.get(a)->cores.arrived != model::rdv[a].arrived) snprintf(why, sizeof why, "rendezvous %X cores", a);
    if (*why) {
        if (fails++ < 10) printf("DIFF seed %llu op %ld (%s): %s\n", (unsigned long long)seed, nops, what, why);
    }
}

/* one operation by thread t on both sides; returns false when it blocks */
static void op(int t, uint64_t seed) {
    using namespace gthrm;
    port::Cpu c{};
    port::t_self = &port::g_ht[t];
    uint32_t pr = 0, mr = 0;
    bool pb = false, mb = false;
    char what[64];
    int k = rnd(17);
    uint32_t qa = QS[rnd(2)], ma = MS[rnd(2)], ea = ES[rnd(2)], ra = RS_[rnd(2)];
    auto P = [&](void (*f)(port::Cpu*)) { try { f(&c); } catch (WouldBlock&) { pb = true; } pr = c.r[3]; };
    switch (k) {
    case 0: { uint32_t n = rnd(4); snprintf(what, sizeof what, "T%d OSInitMessageQueue %X %u", t, qa, n);
        c.r[3] = qa; c.r[4] = BUF; c.r[5] = n; P(port::imp_coreinit_OSInitMessageQueue);
        q_init(model::q[qa], n); model::put(qa, 4, 0x6D536751u); mr = qa; break; }
    case 1: case 2: { uint32_t fl = rnd(4), v = rnd(1000); snprintf(what, sizeof what, "T%d OSSendMessage %X flags %u", t, qa, fl);
        uint32_t m = BUF + 0x40 * t; for (int i = 0; i < 4; i++) { port::st32(m + 4 * i, v + i); model::put(m + 4 * i, 4, v + i); }
        c.r[3] = qa; c.r[4] = m; c.r[5] = fl; P(port::imp_coreinit_OSSendMessage);
        Queue& Q = model::q[qa];
        if (q_full(Q)) { if (!(fl & 1)) mr = 0; else { mb = true; mr = qa; park[t] = {K_SEND, qa, m, fl}; } }
        else { q_put(Q, Msg{v, v + 1, v + 2, v + 3}, fl); mr = 1; }
        if (pb) park[t] = {K_SEND, qa, m, fl};
        break; }
    case 3: case 4: { uint32_t fl = rnd(2); snprintf(what, sizeof what, "T%d OSReceiveMessage %X flags %u", t, qa, fl);
        uint32_t m = BUF + 0x200 + 0x40 * t;
        c.r[3] = qa; c.r[4] = m; c.r[5] = fl; P(port::imp_coreinit_OSReceiveMessage);
        Queue& Q = model::q[qa];
        if (q_empty(Q)) { if (!(fl & 1)) mr = 0; else { mb = true; mr = qa; } }
        else { Msg x = q_take(Q); for (int i = 0; i < 4; i++) model::put(m + 4 * i, 4, x[i]); mr = 1; }
        if (pb || mb) park[t] = {K_RECV, qa, m, fl};
        break; }
    case 5: snprintf(what, sizeof what, "T%d OSInitMutex %X", t, ma);
        c.r[3] = ma; P(port::imp_coreinit_OSInitMutex); m_init(model::mx[ma]); model::put(ma, 4, 0x6D557458u); mr = ma; break;
    case 6: case 7: snprintf(what, sizeof what, "T%d OSLockMutex %X", t, ma);
        c.r[3] = ma; P(port::imp_coreinit_OSLockMutex); mr = ma;
        if (m_can_lock(model::mx[ma], t)) m_acquire(model::mx[ma], t); else mb = true;
        if (pb || mb) park[t] = {K_LOCK, ma};
        break;
    case 8: snprintf(what, sizeof what, "T%d OSTryLockMutex %X", t, ma);
        c.r[3] = ma; P(port::imp_coreinit_OSTryLockMutex); mr = m_trylock(model::mx[ma], t); break;
    case 9: case 10: { /* unlock only a mutex this thread holds (the port does not check; the game never does otherwise) */
        Mutex& M = model::mx[ma];
        if (M.owner != t) { snprintf(what, sizeof what, "T%d skip", t); pr = mr = 0; break; }
        snprintf(what, sizeof what, "T%d OSUnlockMutex %X", t, ma);
        c.r[3] = ma; P(port::imp_coreinit_OSUnlockMutex); m_unlock(M); mr = ma; break; }
    case 11: { uint32_t s = rnd(2), au = rnd(2); snprintf(what, sizeof what, "T%d OSInitEvent %X %u %u", t, ea, s, au);
        /* re-initialising an event with waiters is not done by the game; keep the waiter lists consistent */
        if (!model::ev[ea].waiters.empty()) { snprintf(what, sizeof what, "T%d skip", t); pr = mr = 0; break; }
        c.r[3] = ea; c.r[4] = s; c.r[5] = au; P(port::imp_coreinit_OSInitEvent); e_init(model::ev[ea], s, au); model::put(ea, 4, 0x65566E54u); mr = ea; break; }
    case 12: { snprintf(what, sizeof what, "T%d OSSignalEvent %X", t, ea);
        c.r[3] = ea; P(port::imp_coreinit_OSSignalEvent);
        std::vector<int> w; e_signal(model::ev[ea], w); for (int id : w) model::woken[id] = true; mr = ea; break; }
    case 13: snprintf(what, sizeof what, "T%d OSResetEvent %X", t, ea);
        c.r[3] = ea; P(port::imp_coreinit_OSResetEvent); e_reset(model::ev[ea]); mr = ea; break;
    case 14: { snprintf(what, sizeof what, "T%d OSWaitEvent %X", t, ea);
        c.r[3] = ea; P(port::imp_coreinit_OSWaitEvent); mr = ea;
        if (!e_try(model::ev[ea])) { model::woken[t] = false; model::ev[ea].waiters.push_back(t); mb = true; }
        if (pb || mb) park[t] = {K_WAIT, ea};
        break; }
    case 15: { uint32_t mask = 1 + rnd(7); snprintf(what, sizeof what, "T%d OSWaitRendezvous %X %u", t, ra, mask);
        c.r[3] = ra; c.r[4] = mask; P(port::imp_coreinit_OSWaitRendezvous);
        rdv_arrive(model::rdv[ra], kCore[t]); mr = 1;
        if (!rdv_complete(model::rdv[ra], mask)) mb = true;
        if (pb || mb) park[t] = {K_RDV, ra, mask};
        if (pb) pr = 1; /* (the port's r3 is set after its wait) */
        break; }
    default: { uint32_t key = rnd(16), v = rnd(100000); snprintf(what, sizeof what, "T%d OSSet/GetThreadSpecific %u", t, key);
        c.r[3] = key; c.r[4] = v; P(port::imp_coreinit_OSSetThreadSpecific);
        model::put(0x10000u + 0x800u * t + kSpecific + 4 * key, 4, v);
        c.r[3] = key; P(port::imp_coreinit_OSGetThreadSpecific); mr = model::w32(0x10000u + 0x800u * t + kSpecific + 4 * key); break; }
    }
    nops++;
    if (pb != mb) { if (fails++ < 10) printf("DIFF seed %llu op %ld (%s): blocks: port %d model %d\n", (unsigned long long)seed, nops, what, pb, mb); }
    else if (!pb && pr != mr) { if (fails++ < 10) printf("DIFF seed %llu op %ld (%s): r3 port %08X model %08X\n", (unsigned long long)seed, nops, what, pr, mr); }
    if (pb) nblock++; else park[t].k = K_NONE;
    compare(what, seed);
}

/* complete a parked operation on both sides once its wait is over */
static void resume(int t, uint64_t seed) {
    using namespace gthrm;
    Park p = park[t];
    port::t_self = &port::g_ht[t];
    port::Cpu c{};
    char what[64];
    snprintf(what, sizeof what, "T%d resume kind %d obj %X", t, (int)p.k, p.obj);
    switch (p.k) {
    case K_LOCK: c.r[3] = p.obj; port::imp_coreinit_OSLockMutex(&c); m_acquire(model::mx[p.obj], t); break;
    case K_SEND: { c.r[3] = p.obj; c.r[4] = p.a1; c.r[5] = p.a2; port::imp_coreinit_OSSendMessage(&c);
        Msg m{model::w32(p.a1), model::w32(p.a1 + 4), model::w32(p.a1 + 8), model::w32(p.a1 + 12)};
        q_put(model::q[p.obj], m, p.a2); break; }
    case K_RECV: { c.r[3] = p.obj; c.r[4] = p.a1; c.r[5] = p.a2; port::imp_coreinit_OSReceiveMessage(&c);
        Msg x = q_take(model::q[p.obj]); for (int i = 0; i < 4; i++) model::put(p.a1 + 4 * i, 4, x[i]); break; }
    case K_WAIT: /* the port's event_wait returns once the token is set; nothing more changes */ break;
    case K_RDV: break;
    default: break;
    }
    park[t].k = K_NONE;
    nresume++;
    compare(what, seed);
}

int main(int argc, char** argv) {
    int nseq = argc > 1 ? atoi(argv[1]) : 2000, len = argc > 2 ? atoi(argv[2]) : 200;
    /* core_from_affinity and create_thread's words */
    for (uint32_t m = 0; m < 16; m++)
        for (uint32_t f = 0; f < 3; f++)
            if (port::core_from_affinity(m, f) != gthrm::core_from_affinity(m, f)) { printf("DIFF core_from_affinity(%u, %u)\n", m, f); fails++; }
    rs = 99;
    for (int i = 0; i < 2000; i++) {
        memset(g_pmem, 0xAB, sizeof g_pmem);
        memset(g_mmem, 0xAB, sizeof g_mmem);
        uint32_t t = 0x8000 + 8 * rnd(256), entry = rnd(0xFFFFFFFF), top = rnd(0xFFFFFFFF), size = rnd(0x100000);
        int prio = (int)rnd(32) - 1;
        uint32_t attr = rnd(256);
        uint16_t id = (uint16_t)(1 + rnd(1000)), id2 = id;
        port::create_words(t, entry, top, size, prio, attr, id2);
        gthrm::create_words(t, entry, top, size, (uint32_t)prio, attr, id, [](uint32_t a, int sz, uint32_t v) { model::put(a, sz, v); });
        if (memcmp(g_pmem, g_mmem, sizeof g_pmem) || id2 != id + 1) { printf("DIFF create_thread words (case %d)\n", i); fails++; break; }
    }
    for (int s = 1; s <= nseq; s++) {
        memset(g_pmem, 0, sizeof g_pmem);
        memset(g_mmem, 0, sizeof g_mmem);
        for (uint32_t a : QS) { port::g_queues.reset(a); model::q[a] = {}; }
        for (uint32_t a : MS) { port::g_mutexes.reset(a); model::mx[a] = {}; }
        for (uint32_t a : ES) { port::g_events.reset(a); model::ev[a] = {}; }
        for (uint32_t a : RS_) { port::g_rdv.reset(a); model::rdv[a] = {}; }
        for (int t = 0; t < 4; t++) { port::g_ht[t] = port::HostThread(); port::g_ht[t].id = t; port::g_ht[t].core = kCore[t]; model::woken[t] = false; park[t] = Park(); }
        rs = (uint64_t)s * 0x9E3779B97F4A7C15ull;
        for (int i = 0; i < len; i++) {
            int t = rnd(4);
            if (park[t].k != K_NONE) {
                bool a = port_pred(t), b = model_pred(t);
                if (a != b) { if (fails++ < 10) printf("DIFF seed %d: wait of T%d over: port %d model %d\n", s, t, a, b); continue; }
                if (a) resume(t, s);
                continue;
            }
            op(t, s);
        }
    }
    /* alarms and the calendar clock: OSCreateAlarm / OSSetAlarm / OSSetPeriodicAlarm (OSTime starts above and below the
     * bias) / OSCancelAlarm / OSSet/GetAlarmUserData by threads on cores 0..2, interleaved with clock advances and firings;
     * the model composed as gthr_sched.inc composes it (when = clock + delay; os_to_guest; alarm_arm/cancel/pop_due) */
    long nalarm = 0, nfire = 0;
    for (int s = 1; s <= nseq / 10 + 1; s++) {
        memset(g_pmem, 0, sizeof g_pmem);
        memset(g_mmem, 0, sizeof g_mmem);
        port::g_alarm_queue.clear(); port::g_alarm_serial.clear(); port::g_next_serial = 1; port::g_alarm_thread_started = true;
        gthrm::Alarms mq;
        port::g_now = 1000;
        uint64_t mclock = 1000;
        rs = (uint64_t)s * 0x51ED27ull;
        for (int i = 0; i < len; i++) {
            int t = rnd(4);
            port::t_self = &port::g_ht[t];
            port::Cpu c{};
            uint32_t al = 0x2000 + 0x60 * rnd(6);
            char what[80];
            int k = rnd(9);
            uint32_t pr = 0, mr = 0;
            if (k == 0) { snprintf(what, sizeof what, "OSCreateAlarm %X", al); c.r[3] = al; port::imp_coreinit_OSCreateAlarm(&c);
                for (int j = 0; j < 0x58; j += 4) model::put(al + j, 4, 0); model::put(al, 4, 0x614C724Du); }
            else if (k == 1 || k == 2) { uint64_t delay = rnd(4) ? rnd(5000) : 0; uint32_t cb = 0x02000000 + 4 * rnd(16);
                snprintf(what, sizeof what, "OSSetAlarm %X +%llu", al, (unsigned long long)delay);
                c.r[3] = al; c.r[5] = (uint32_t)(delay >> 32); c.r[6] = (uint32_t)delay; c.r[7] = cb; port::imp_coreinit_OSSetAlarm(&c); pr = c.r[3];
                model::put(al + 0x0C, 4, cb); gthrm::alarm_arm(mq, al, mclock + delay, 0, cb, kCore[t]); mr = 1; }
            else if (k == 3) { uint64_t period = rnd(3) ? 1 + rnd(3000) : 0, start = mclock + (rnd(2) ? rnd(4000) : 0) - (rnd(2) ? rnd(800) : 0);
                if (rnd(2)) start += (uint64_t)gthrm::kFixedOSTime; /* an OSTime (OSGetTime() + ...) */
                uint32_t cb = 0x02000000 + 4 * rnd(16);
                snprintf(what, sizeof what, "OSSetPeriodicAlarm %X start %llu period %llu", al, (unsigned long long)start, (unsigned long long)period);
                c.r[3] = al; c.r[5] = (uint32_t)(start >> 32); c.r[6] = (uint32_t)start; c.r[7] = (uint32_t)(period >> 32); c.r[8] = (uint32_t)period;
                c.r[9] = cb; port::imp_coreinit_OSSetPeriodicAlarm(&c); pr = c.r[3];
                model::put(al + 0x0C, 4, cb);
                uint64_t st = gthrm::os_to_guest(start, gthrm::kFixedOSTime), when = st;
                if (when < mclock && period) when += ((mclock - st) / period + 1) * period;
                gthrm::alarm_arm(mq, al, when, period, cb, kCore[t]); mr = 1; }
            else if (k == 4) { snprintf(what, sizeof what, "OSCancelAlarm %X", al); c.r[3] = al; port::imp_coreinit_OSCancelAlarm(&c); pr = c.r[3];
                gthrm::alarm_cancel(mq, al); mr = 1; }
            else if (k == 5) { uint32_t v = rnd(1000); snprintf(what, sizeof what, "OSSet/GetAlarmUserData %X", al);
                c.r[3] = al; c.r[4] = v; port::imp_coreinit_OSSetAlarmUserData(&c); model::put(al + 0x38, 4, v);
                c.r[3] = al; port::imp_coreinit_OSGetAlarmUserData(&c); pr = c.r[3]; mr = model::w32(al + 0x38); }
            else { /* the clock moves on; every due alarm fires (in order), as the alarm thread and the service thread take them */
                uint64_t adv = rnd(3) ? rnd(2000) : rnd(20000);
                port::g_now += adv; mclock += adv;
                snprintf(what, sizeof what, "clock +%llu", (unsigned long long)adv);
                for (;;) {
                    port::Alarm pa; gthrm::Alarm ma;
                    bool pf = port::port_fire(port::g_now, &pa), mf = gthrm::alarm_pop_due(mq, mclock, &ma);
                    if (pf != mf) { if (fails++ < 10) printf("DIFF alarm seq %d op %d (%s): fires port %d model %d\n", s, i, what, pf, mf); break; }
                    if (!pf) break;
                    nfire++;
                    if (pa.guest != ma.guest || pa.callback != ma.callback || pa.core != ma.core) {
                        if (fails++ < 10) printf("DIFF alarm seq %d op %d (%s): firing port %X cb %X core %u model %X cb %X core %u\n", s, i, what,
                                                 pa.guest, pa.callback, pa.core, ma.guest, ma.callback, ma.core);
                        break;
                    }
                }
            }
            nalarm++;
            if (pr != mr) { if (fails++ < 10) printf("DIFF alarm seq %d op %d (%s): r3 port %08X model %08X\n", s, i, what, pr, mr); }
            if (memcmp(g_pmem, g_mmem, sizeof g_pmem)) { if (fails++ < 10) printf("DIFF alarm seq %d op %d (%s): guest memory\n", s, i, what); }
            /* the queues: live entries (due tick, alarm, callback, period, core) in order */
            std::vector<std::array<uint64_t, 5>> pq, mqv;
            for (auto& kv : port::g_alarm_queue) if (port::g_alarm_serial[kv.second.guest] == kv.second.serial) pq.push_back({kv.first, kv.second.guest, kv.second.callback, kv.second.period, kv.second.core});
            for (auto& kv : mq.queue) if (mq.serial[kv.second.guest] == kv.second.serial) mqv.push_back({kv.first, kv.second.guest, kv.second.callback, kv.second.period, kv.second.core});
            if (pq != mqv) { if (fails++ < 10) printf("DIFF alarm seq %d op %d (%s): live alarm queues differ (%zu / %zu)\n", s, i, what, pq.size(), mqv.size()); }
            if (fails) break;
        }
        if (fails) break;
    }
    printf("guestthreads_equiv: alarms: %ld operations, %ld firings\n", nalarm, nfire);
    printf("guestthreads_equiv: %ld operations, %ld blocked, %ld resumed, %d difference(s)\n", nops, nblock, nresume, fails);
    return fails ? 1 : 0;
}
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--port-src", default=os.environ.get("WWHD_PORT_SRC", "runtime/src"))
    ap.add_argument("--port-rev", default="f5554821", help="read the port sources at this commit of the port repository (git show); "
                    "'' = the working tree (default f5554821: OSWaitRendezvous per core, aaa5fcde)")
    ap.add_argument("--seqs", default="3000")
    ap.add_argument("--len", default="300")
    a = ap.parse_args()
    def port_file(name):
        if a.port_rev:
            repo = os.path.dirname(os.path.dirname(os.path.abspath(a.port_src)))
            return subprocess.run(["git", "-C", repo, "show", "%s:runtime/src/%s" % (a.port_rev, name)], capture_output=True, text=True,
                                  check=True).stdout
        return open(os.path.join(a.port_src, name)).read()
    src = port_file("threads.cpp")
    rdv_h = port_file("rendezvous.h")
    rdv_h = rdv_h[rdv_h.index("namespace threads {"):]
    waitkind = re.search(r"^enum WaitKind[^\n]*\n", src, re.M).group(0)
    osthread = block(src, r"^namespace osthread \{")
    core = block(src, r"^static uint32_t core_from_affinity\(")
    ct = block(src, r"^static HostThread\* create_thread\(")
    m1 = ct.index("memset(mem::ptr(t), 0, osthread::kSize);")
    m2 = ct.index("st32(t + osthread::kAffinity, attr & 7);")
    create = ct[m1:m2] + "st32(t + osthread::kAffinity, attr & 7);"
    objtable = block(src, r"^template <typename T>\nstruct ObjTable") if re.search(r"^template <typename T>\nstruct ObjTable", src, re.M) else None
    if objtable is None:
        i = src.index("struct ObjTable")
        j = src.rindex("template", 0, i)
        objtable = block(src[j:], r"^template <typename T>")
    start = src.index("// OSMutex: recursive")
    end = src.index("// OSThreadQueue / OSSleepThread")
    body = src[start:end]
    rstart = src.index("// OSRendezvous")
    rend = src.index("// ---------------------------------------------------------------- alarms")
    body += src[rstart:rend]
    body += "\n" + block(src, r"^HLE\(coreinit, OSSetThreadSpecific\)") + "\n" + block(src, r"^HLE\(coreinit, OSGetThreadSpecific\)") + "\n"
    # profiling hooks of the port (namespace mainprof, off unless enabled): shimmed as no-ops
    names = sorted(set(re.findall(r"mainprof::(g_\w+)", body)))
    mainprof = "namespace mainprof { inline bool on(Cpu*) { return false; } template <class... A> inline void bump(A...) {}\n" + \
        "".join("static uint64_t %s;\n" % n for n in names) + "}\n"
    body = mainprof + body
    a0 = src.index("// ---------------------------------------------------------------- alarms")
    a1 = src.index("HLE(coreinit, OSGetAlarmUserData)")
    a1 = src.index("\n", a1) + 1
    alarms = src[a0:a1]
    for pat in (r"^void timebase::set_clock_rate\(", r"^static void alarm_thread\(\) \{"):
        if re.search(pat, alarms, re.M):
            alarms = alarms.replace(block(alarms, pat), "")
    if alarms.count("std::thread(alarm_thread).detach();") != 1:
        sys.exit("guestthreads_equiv: arm_alarm's thread start not found")
    alarms = alarms.replace("std::thread(alarm_thread).detach();", "/* (equiv: no alarm thread) */")
    thread_fn = block(src, r"^static void alarm_thread\(\) \{")
    m = re.search(r"(if \(g_alarm_serial\[it->second\.guest\] != it->second\.serial\) \{.*?continue;\s*\})", thread_fn, re.S)
    if not m:
        sys.exit("guestthreads_equiv: alarm_thread's stale-entry drop not found")
    stale = m.group(1)
    m = re.search(r"(Alarm a = it->second;.*?)\n\s*lk\.unlock\(\);\s*\n\s*threads::set_service_core", thread_fn, re.S)
    if not m:
        sys.exit("guestthreads_equiv: alarm_thread's firing block not found")
    rearm = m.group(1).replace("threads::service_end();", "")
    to_guest = block(src, r"^uint64_t to_guest\(uint64_t os_ticks\)")
    prog = PROG
    for k, v in dict(alarms=alarms, stale=stale, rearm=rearm, to_guest=to_guest, rendezvous=rdv_h, waitkind=waitkind, osthread=osthread, core=core, create=create, objtable=objtable, body=body).items():
        prog = prog.replace("@@%s@@" % k, v)
    sys.path.insert(0, HERE)
    import verify  # noqa: E402
    d = tempfile.mkdtemp(prefix="guestthreads_equiv-", dir=verify.verify_scratch_base() or None)
    cpp = os.path.join(d, "equiv.cpp")
    exe = os.path.join(d, "equiv")
    open(cpp, "w").write(prog)
    r = subprocess.run(["clang++", "-std=c++20", "-O1", "-w", "-I" + os.path.join(HERE, "src"), cpp, "-o", exe], capture_output=True, text=True)
    if r.returncode:
        print(r.stderr[-4000:])
        print("guestthreads_equiv: build failed (%s)" % cpp)
        return 2
    r = subprocess.run([exe, a.seqs, a.len], capture_output=True, text=True)
    print(r.stdout.strip())
    print("guestthreads_equiv: %s (port source %s at %s, program %s)" % ("PASS" if r.returncode == 0 else "FAIL", a.port_src,
          a.port_rev or "the working tree", cpp))
    return r.returncode


if __name__ == "__main__":
    sys.exit(main())

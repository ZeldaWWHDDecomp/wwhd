/* Guest-thread object model of the `osmem ... threads` part (harness.cpp namespace gthr): the Cafe OS synchronisation
 * objects as the shipping port's runtime/src/threads.cpp implements them, without host threads. Pure data structures:
 * every operation either completes or says that the calling thread has to wait (the scheduler in harness.cpp then
 * blocks that guest thread and retries when it runs again, the way the port's park_wait re-checks its predicate).
 * Thread ids are the model's creation indices (0 = the main thread). Included by harness.cpp and by
 * guestthreads_equiv.py (compared there against the port's own HLE bodies). */
#pragma once
#include <stdint.h>
#include <array>
#include <deque>
#include <map>
#include <unordered_map>
#include <vector>

namespace gthrm {

/* OSThread offsets (threads.cpp namespace osthread; the OSThread layout is a public wut/Cemu fact) */
static const uint32_t kSize = 0x6A0, kState = 0x324, kAttr = 0x325, kId = 0x326, kSuspend = 0x328, kEffPrio = 0x32C,
                      kBasePrio = 0x330, kExitValue = 0x334, kStackBase = 0x394, kStackEnd = 0x398, kEntry = 0x39C,
                      kSpecific = 0x57C, kName = 0x5C0, kAffinity = 0x304;
enum : uint8_t { ST_NONE = 0, ST_READY = 1, ST_RUNNING = 2, ST_WAITING = 4, ST_MORIBUND = 8 };

/* threads.cpp core_from_affinity: bit0 = core 0, bit1 = core 1, bit2 = core 2; no bit or all three: the fallback
 * (the creating thread's core); else the lowest set bit */
inline uint32_t core_from_affinity(uint32_t mask, uint32_t fallback) {
    mask &= 7;
    if (!mask || mask == 7) return fallback;
    return (uint32_t)__builtin_ctz(mask);
}

/* create_thread's guest stores, in the port's order: W(addr, size, value) with size 1, 2 or 4; the whole 0x6A0-byte
 * block is zeroed first (memset) */
template <class W> inline void create_words(uint32_t t, uint32_t entry, uint32_t stack_top, uint32_t stack_size, uint32_t prio,
                                            uint32_t attr, uint16_t id, W&& w) {
    for (uint32_t i = 0; i < kSize; i += 4) w(t + i, 4, 0);
    w(t + 0, 4, 0x4F53436Fu); /* "OSContxt" */
    w(t + 4, 4, 0x6E747874u);
    w(t + kState, 1, ST_READY);
    w(t + kAttr, 1, attr & 0xFF);
    w(t + kId, 2, id);
    w(t + kSuspend, 4, 1);
    w(t + kEffPrio, 4, prio);
    w(t + kBasePrio, 4, prio);
    w(t + kStackBase, 4, stack_top);
    w(t + kStackEnd, 4, stack_top - stack_size);
    w(t + kEntry, 4, entry);
    w(t + kAffinity, 4, attr & 7);
}

/* OSMessageQueue: 16-byte messages; OSSendMessage flags bit0 = block, bit1 = high priority (front);
 * OSReceiveMessage flags bit0 = block */
typedef std::array<uint32_t, 4> Msg;
struct Queue {
    std::deque<Msg> msgs;
    uint32_t capacity = 0;
};
inline void q_init(Queue& q, uint32_t count) { q.msgs.clear(); q.capacity = count; }
inline bool q_full(const Queue& q) { return q.msgs.size() >= q.capacity; }
inline void q_put(Queue& q, const Msg& m, uint32_t flags) {
    if (flags & 2) q.msgs.push_front(m);
    else q.msgs.push_back(m);
}
inline bool q_empty(const Queue& q) { return q.msgs.empty(); }
inline Msg q_take(Queue& q) {
    Msg m = q.msgs.front();
    q.msgs.pop_front();
    return m;
}

/* OSMutex: recursive, owned by a thread (-1: free). Unlock does not check the owner (as the port) */
struct Mutex {
    int owner = -1;
    int count = 0;
};
inline void m_init(Mutex& m) { m.owner = -1; m.count = 0; }
inline bool m_can_lock(const Mutex& m, int self) { return m.owner == -1 || m.owner == self; }
inline void m_acquire(Mutex& m, int self) { /* after m_can_lock */
    if (m.owner == self) { m.count++; return; }
    m.owner = self;
    m.count = 1;
}
inline bool m_trylock(Mutex& m, int self) {
    if (!m_can_lock(m, self)) return false;
    m_acquire(m, self);
    return true;
}
inline void m_unlock(Mutex& m) {
    if (--m.count > 0) return;
    m.owner = -1;
    m.count = 0;
}

/* OSEvent: a signal wakes the threads waiting at that moment (manual reset: all of them, and the event becomes
 * signaled; auto reset: the first waiter only, without becoming signaled; with no waiter it becomes signaled). A woken
 * waiter returns even if the event is reset before it runs. */
struct Event {
    bool signaled = false;
    bool auto_reset = false;
    std::deque<int> waiters;
};
inline void e_init(Event& e, bool signaled, bool auto_reset) { e.signaled = signaled; e.auto_reset = auto_reset; }
/* the waiter tokens a signal sets go to `woken` */
inline void e_signal(Event& e, std::vector<int>& woken) {
    if (e.auto_reset && !e.waiters.empty()) {
        woken.push_back(e.waiters.front());
        e.waiters.pop_front();
    } else {
        e.signaled = true;
        for (int t : e.waiters) woken.push_back(t);
        e.waiters.clear();
    }
}
inline void e_reset(Event& e) { e.signaled = false; }
/* a wait that does not block: true (and an auto-reset event is consumed) */
inline bool e_try(Event& e) {
    if (!e.signaled) return false;
    if (e.auto_reset) e.signaled = false;
    return true;
}
inline void e_cancel(Event& e, int self) { /* a timed-out waiter leaves the list */
    for (auto it = e.waiters.begin(); it != e.waiters.end(); ++it)
        if (*it == self) { e.waiters.erase(it); break; }
}

/* OSRendezvous: one arrival flag per core, as on Cafe OS and in the port since f5554821 (aaa5fcde, runtime/src/rendezvous.h
 * RendezvousCores): a waiter marks the core it runs on (OSGetCoreId; cores 0..2) and waits until the flag of every core in
 * its mask (bits 0..2) is set. Two threads on one core are one arrival; a caller whose core is not in the mask still marks
 * it; only OSInitRendezvous clears the flags. */
struct Rendezvous {
    uint32_t arrived = 0; /* bit n: core n has arrived since OSInitRendezvous */
};
inline void rdv_arrive(Rendezvous& r, uint32_t core) {
    if (core <= 2) r.arrived |= 1u << core;
}
inline bool rdv_complete(const Rendezvous& r, uint32_t mask) { return (r.arrived & (mask & 7)) == (mask & 7); }

/* alarms (threads.cpp arm_alarm / alarm_thread at f5554821): a due-tick queue with serials; a cancelled or re-armed alarm's
 * old entries are stale (serial mismatch) and dropped when they reach the head (also before waiting on them); a periodic
 * alarm re-arms at when + period, or now + period when that is already past */
struct Alarm {
    uint32_t core, guest;
    uint64_t when, period;
    uint32_t callback;
    uint64_t serial;
};
struct Alarms {
    std::multimap<uint64_t, Alarm> queue;
    std::unordered_map<uint32_t, uint64_t> serial; /* alarm -> active serial (0 = cancelled) */
    uint64_t next_serial = 1;
};
inline void alarm_arm(Alarms& q, uint32_t alarm, uint64_t when, uint64_t period, uint32_t cb, uint32_t core) {
    uint64_t s = q.next_serial++;
    q.serial[alarm] = s;
    q.queue.emplace(when, Alarm{core, alarm, when, period, cb, s});
}
inline void alarm_cancel(Alarms& q, uint32_t alarm) { q.serial[alarm] = 0; }
/* the head after dropping stale entries (nullptr: none) */
inline const Alarm* alarm_head(Alarms& q) {
    while (!q.queue.empty()) {
        auto it = q.queue.begin();
        if (q.serial[it->second.guest] != it->second.serial) { q.queue.erase(it); continue; }
        return &it->second;
    }
    return nullptr;
}
/* fire the head if it is due at `now`: re-arm or retire it; true and *out = the firing (guest, callback, core) */
inline bool alarm_pop_due(Alarms& q, uint64_t now, Alarm* out) {
    const Alarm* h = alarm_head(q);
    if (!h || h->when > now) return false;
    auto it = q.queue.begin();
    Alarm a = it->second;
    q.queue.erase(it);
    if (a.period) {
        a.when += a.period;
        if (a.when < now) a.when = now + a.period;
        q.queue.emplace(a.when, a);
    } else {
        q.serial[a.guest] = 0;
    }
    *out = a;
    return true;
}
/* os_time (f5554821): OSGetTime = guest timebase + bias; the bias of a repeatable run is kFixedOSTime (2026-01-01 12:00,
 * WWHD_TEST_FIXED_SEED) or WWHD_OS_TIME=<seconds since 1970> read as local time; an OSTime at or above the bias goes back to
 * the guest timebase by subtracting it (alarm starts) */
constexpr int64_t kTicksPerSec = 62156250, kSecondsPerDay = 86400, kUnixTo2000 = 946684800;
constexpr int64_t kFixedOSTime = (9497 * kSecondsPerDay + 12 * 3600) * kTicksPerSec;
inline uint64_t os_to_guest(uint64_t os_ticks, int64_t bias) { return os_ticks >= (uint64_t)bias ? os_ticks - (uint64_t)bias : os_ticks; }

}  // namespace gthrm

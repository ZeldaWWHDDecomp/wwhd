/* `osmem ... threads` (opt-in, not in `hle`): a cooperative guest-thread model, so original code that creates threads and waits on them (JAudio, nw::snd
 * SoundThread/TaskThread, sead::Thread) runs for real. Included by harness.cpp (one translation unit) after the osm
 * namespace; the object semantics are in gthr_model.h.
 *
 * Execution. The recompiled code is native C: a guest call stack is a host call stack. Each guest thread therefore gets its
 * own host coroutine (a 64 MB mmap'd host stack, touched pages only, so it counts in the RSS that hostmembudget samples)
 * and its own Cpu; its guest stack is the one OSCreateThread names. The main thread (id 0) is the harness's ordinary run
 * context: the setups and the tested call. Exactly one context runs at a time; switches happen only inside modelled OS
 * calls (blocking, sleeping, yielding, joining, exiting, and the wake/perturbation rules below) and at the slice preemption
 * (a thread that runs `threadslice` guest accesses and modelled OS calls lets a runnable thread on ANOTHER core run: racy
 * polls such as `while (!flag);` are switch points). Per context the harness keeps V.depth, V.real_chain,
 * V.stack_internal, V.call_top and the candidate glue's gabi::cpu / frame_ctx / Activation state. Thread code runs at
 * depth >= 1 (its calls are nested calls: answered silently, real callees run for real); a unit (tested) function reached
 * from a thread runs the original on the original side and, from the tested call on, the candidate (or mutant) on the candidate
 * side (unit_fn: original on both sides during setups, postsetups and their drain).
 *
 * Schedule (deterministic). A thread runs until it blocks, sleeps, yields, joins or exits. Priorities and affinities as
 * created (core = threads.cpp core_from_affinity, fallback the creator's core; the main thread: core 1, priority 16). A
 * thread is eligible when it is runnable and no runnable thread on its core has a strictly higher priority (lower number).
 * A wake or resume that makes a same-core, strictly higher-priority thread runnable switches to it at once (Cafe
 * preemption). Otherwise the next thread is the first eligible one after the current in creation order (round robin).
 * Virtual clock: it advances with guest progress, `threadrate` ticks (default 64, see
 * kDefaultRate) after every modelled time read (OSGetTime, OSGetSystemTime, OSGetTick) and 1 tick after any other modelled OS
 * call of whichever thread runs, and when nothing is eligible it jumps to the earliest sleep or timeout as before. When progress carries the clock past a sleep, timeout or alarm, a thread that this wakes on
 * the running thread's core with a strictly higher priority runs at once (the timer interrupt's preemption), and an eligible
 * thread this wakes on another core runs at once too (its core is free on the console; at that tick, not at an access-counted
 * slice, so both sides wake it at the same point). An equal-priority thread on the running thread's core waits until the
 * runner blocks or yields (Cafe does not time-slice it). `threadrate=0` is the old clock (only the jumps). When nothing is
 * eligible and nothing waits on the clock either, the run aborts: `guest threads: deadlock (non-termination): ...` with
 * the thread table. `threadsched=N` (N != 0, or harness option -threadsched N) is a seeded perturbation: every sync point may switch to a seeded pick among the
 * eligible threads, the slice length is drawn per run (2^8..2^22 accesses) and the clock origin is offset per run.
 *
 * Drain: after the setups (before a snapshot capture and the baseline rebase) and after the tested call, the main
 * thread waits until no other thread is runnable (without a clock jump; the running threads' OS calls still advance it), so work that setups start is baseline and
 * work the tested call starts is part of its effect. `threads nodrain` switches that off. */

#include <cstdarg>
namespace gthr {
using namespace gthrm;

bool on = false;                  /* `osmem ... threads` */
uint32_t main_obj = 0x6FFE1000u; /* mainthread=HEX: the main thread's OSThread block */
uint64_t sched_seed = 0;   /* threadsched=N / -threadsched N (0: the default order) */
uint64_t clock_origin = 0; /* threadclock=HEX: OSGetTime at the start of a run (ticks) */
uint64_t budget = 1000000; /* threadbudget N: scheduler events per run */
uint32_t cap = 64;         /* threadcap N: threads per run (main included) */
uint64_t slice_len = 1ull << 20; /* threadslice N: guest accesses before a slice preemption */
/* threadrate=N: virtual-clock ticks per modelled time read (OSGetTime, OSGetSystemTime, OSGetTick); any other modelled OS call
 * (this namespace's call()) costs 1 tick; 0 = neither: the clock moves only when every thread waits. Paced by OS calls, not by
 * guest accesses: the original and the candidate make the same OS calls in the same order whenever they are equivalent (their
 * call logs are compared), but not the same number of guest accesses (different code generation), so an access-paced clock
 * would make OSGetTime/OSGetTick differ between equivalent sides. The timebase runs at the Espresso bus clock / 4 = 62.15625
 * MHz and the cores at 1.243125 GHz, so one tick is 20 core cycles: about one uncontended lock or queue call. A time read
 * stands for one iteration of a time-poll loop, the only loop that needs time to pass to end: 64 ticks, 1280 cycles, 1.03 us,
 * so one 60 Hz frame (1036973 ticks) is about 16200 iterations. Nominal, deterministic and documented, not measured.
 * Not 64 per call of every kind: compute between OS calls costs no clock, so a flag spin (027F9994: unlock, lock, read, 700000
 * ticks per slice) made time outrun the threads it waits for: AX frames arrived faster than core 0 could run them and their
 * audio work, and the lower-priority worker on core 0 that the spin waited for never ran (J3D start-up, 2026-11-02) */
static const uint64_t kDefaultRate = 64;
uint64_t clock_rate = kDefaultRate;
/* threadlog=N: entries of the per-thread call logs in one run, all threads together (52 bytes each; default 4,000,000,
 * about 200 MB); more aborts the run with a receipt naming the thread (a spin cannot eat the machine) */
uint64_t log_cap = 4000000;
bool drain_on = true;      /* `nodrain` */
int64_t os_bias = gthrm::kFixedOSTime; /* ostime=SECONDS: OSGetTime = clock + bias (os_time.h; default the repeatable run's) */
/* kMainCore, kMainPrio, kMainStackTop, kMainStackSize, kRpxEntry: declared with the forward declarations in harness.cpp */
static const size_t kHostStack = 64u << 20;
static const uint64_t timebase_ticks = 62156250ull; /* runtime.h kTicksPerSec (Espresso bus clock / 4) */
/* the fimport stubs of the RPX: stub = slot - delta, one delta PER LIBRARY (measured from every `bl` to an import in
 * build/gen, strictmocks_test.py 6: coreinit BD70BD88, gx2 BD70BC88, snd_core BD70BC90, padscore BD70BD60, vpad BD70BCC8,
 * ...). coreinit e.g. C0009988/028FDC00 OSBlockMove, C0009BB0/028FDE28 OSGetCoreId, C0009CC8/028FDF40 OSGetThreadSpecific,
 * C000A090/028FE308 OSSetThreadSpecific, C0009A48/028FDCC0 OSCreateThread. The thread imports are coreinit except
 * GX2WaitForVsync (gx2, C0006AE8/028FAE60). Before 2026-11-02 the coreinit delta was applied to it too:
 * 028FAD60, no import's stub, meant GX2WaitForVsync, and its real stub 028FAE60 was not recognized. */
static const uint32_t kStubDelta = 0xBD70BD88u;    /* coreinit */
static const uint32_t kStubDeltaGx2 = 0xBD70BC88u; /* gx2 (slots C0006090..C0006AE8) */
static uint32_t stub_of(uint32_t slot) { return slot - (slot >= 0xC0006090u && slot <= 0xC0006AE8u ? kStubDeltaGx2 : kStubDelta); }

enum Wait : uint8_t { W_NONE, W_MUTEX, W_EVENT, W_SEND, W_RECV, W_SLEEPQ, W_JOIN, W_RDV, W_SLEEP, W_DRAIN, W_EXITED, W_ALARM };
static const char* wait_name(int w) {
    static const char* n[] = {"running", "OSLockMutex", "OSWaitEvent", "OSSendMessage", "OSReceiveMessage", "OSSleepThread",
                              "OSJoinThread", "OSWaitRendezvous", "OSSleepTicks", "drain", "exited", "alarm service idle"};
    return n[w];
}

struct LogEv { uint32_t target; int kind, nint, nflt; uint32_t a[8]; uint32_t r3; };
struct Thr {
    int id = 0;
    uint32_t guest = 0, entry = 0, argc = 0, argv = 0, stack_top = 0, stack_size = 0, attr = 0, core = 1;
    int prio = 16;
    int suspend = 1;
    bool exited = false;
    uint32_t exit_value = 0;
    void (*host_entry)(Thr*) = nullptr; /* -gthrselftest: a host body instead of guest code */
    Cpu cpu{};
    void* host_sp = nullptr;
    void* stack_mem = nullptr;
    bool coroutine = false;
    Wait wait = W_NONE;
    uint32_t wait_obj = 0, wait_arg = 0;
    uint64_t deadline = 0;
    bool has_deadline = false, woken = false;
    int irq_off = 0;
    /* the harness context while switched out */
    int depth = 1, stack_internal = 0, act_depth = 0;
    std::vector<uint32_t> chain;
    uint32_t call_top = 0, act_cur = 0;
    Cpu* gcpu = nullptr;
    gabi::FrameCtx* fctx = nullptr;
    std::vector<LogEv> log;
    uint64_t slice = 0;
};
struct State {
    std::vector<Thr*> thr; /* by id; 0 = main */
    std::map<uint32_t, int> by_guest;
    std::map<uint32_t, Queue> q;
    std::map<uint32_t, Mutex> mx;
    std::map<uint32_t, Event> ev;
    std::map<uint32_t, Rendezvous> rdv;
    std::map<uint32_t, std::vector<int>> sleepq;
    /* alarms (gthr_model.h Alarms: threads.cpp arm_alarm / alarm_thread); the callbacks run on the alarm service thread
     * (created at the first alarm, priority -1, on the arming thread's core) with the SDA bases of the arming thread */
    Alarms alarms;
    int service = -1; /* the alarm service thread's id */
    int cur = 0;
    uint64_t clock = 0, events = 0, digest = 0x9E3779B97F4A7C15ull, nswitch = 0, npreempt = 0, slice_run = 0;
    uint64_t next_due = UINT64_MAX;   /* the earliest sleep/timeout/alarm after the clock (a progress tick reaching it fires) */
    uint64_t nlog = 0;                /* call-log entries of this run (threadlog) */
    uint16_t next_id = 2;
    bool touched = false; /* any model state beyond the boot state (snapshots refuse it) */
    int fault_thread = 0; /* the thread that ran when the run ended by a fault (-nulltrap) */
    bool deadlock = false;
    bool nomem = false;   /* -gthrselftest: no guest memory */
};
static State S;
/* `osmem ... ax` (see the AX section below) */
static axm::State* AX = nullptr;
static uint32_t ax_host_top = 0;
static int ax_thread = -1;
static uint64_t ax_t0 = 0;
int g_multi = 0; /* more than one thread exists in this run (the slice check in vm_ld/vm_st) */

/* ---------------------------------------------------------------- host coroutines (arm64 / x86-64) */
extern "C" void gthr_switch(void** save_sp, void* load_sp);
extern "C" void gthr_tramp();
#if defined(__aarch64__)
#ifdef __APPLE__
#define GTHR_SYM(x) "_" #x
#else
#define GTHR_SYM(x) #x
#endif
__asm__(".text\n.globl " GTHR_SYM(gthr_switch) "\n.p2align 2\n" GTHR_SYM(gthr_switch) ":\n"
        "  sub sp, sp, #160\n"
        "  stp x19, x20, [sp, #0]\n  stp x21, x22, [sp, #16]\n  stp x23, x24, [sp, #32]\n  stp x25, x26, [sp, #48]\n"
        "  stp x27, x28, [sp, #64]\n  stp x29, x30, [sp, #80]\n  stp d8, d9, [sp, #96]\n  stp d10, d11, [sp, #112]\n"
        "  stp d12, d13, [sp, #128]\n  stp d14, d15, [sp, #144]\n"
        "  mov x9, sp\n  str x9, [x0]\n  mov sp, x1\n"
        "  ldp x19, x20, [sp, #0]\n  ldp x21, x22, [sp, #16]\n  ldp x23, x24, [sp, #32]\n  ldp x25, x26, [sp, #48]\n"
        "  ldp x27, x28, [sp, #64]\n  ldp x29, x30, [sp, #80]\n  ldp d8, d9, [sp, #96]\n  ldp d10, d11, [sp, #112]\n"
        "  ldp d12, d13, [sp, #128]\n  ldp d14, d15, [sp, #144]\n"
        "  add sp, sp, #160\n  ret\n"
        ".globl " GTHR_SYM(gthr_tramp) "\n.p2align 2\n" GTHR_SYM(gthr_tramp) ":\n"
        "  mov x0, x19\n  blr x20\n  brk #1\n");
static void* initial_frame(void* top, void (*fn)(void*), void* arg) {
    uint64_t* sp = (uint64_t*)(((uintptr_t)top & ~(uintptr_t)15) - 160);
    memset(sp, 0, 160);
    sp[0] = (uint64_t)arg;              /* x19 */
    sp[1] = (uint64_t)fn;               /* x20 */
    sp[11] = (uint64_t)&gthr_tramp;     /* x30 */
    return sp;
}
#elif defined(__x86_64__)
#ifdef __APPLE__
#define GTHR_SYM(x) "_" #x
#else
#define GTHR_SYM(x) #x
#endif
__asm__(".text\n.globl " GTHR_SYM(gthr_switch) "\n" GTHR_SYM(gthr_switch) ":\n"
        "  pushq %rbp\n  pushq %rbx\n  pushq %r12\n  pushq %r13\n  pushq %r14\n  pushq %r15\n"
        "  movq %rsp, (%rdi)\n  movq %rsi, %rsp\n"
        "  popq %r15\n  popq %r14\n  popq %r13\n  popq %r12\n  popq %rbx\n  popq %rbp\n  ret\n"
        ".globl " GTHR_SYM(gthr_tramp) "\n" GTHR_SYM(gthr_tramp) ":\n"
        "  movq %r12, %rdi\n  callq *%r13\n  ud2\n");
static void* initial_frame(void* top, void (*fn)(void*), void* arg) {
    uint64_t* sp = (uint64_t*)(((uintptr_t)top & ~(uintptr_t)15) - 64);
    memset(sp, 0, 64);
    /* pop order r15 r14 r13 r12 rbx rbp, then ret */
    sp[2] = (uint64_t)fn;              /* r13 */
    sp[3] = (uint64_t)arg;             /* r12 */
    sp[6] = (uint64_t)&gthr_tramp;     /* return address */
    return sp;
}
#else
#error "osmem threads: no host context switch for this architecture"
#endif

/* ---------------------------------------------------------------- helpers */
static uint64_t mixh(uint64_t x) { x ^= x >> 31; x *= 0x7FB5D329728EA185ull; x ^= x >> 27; x *= 0x81DADEF4BC2DD44Dull; x ^= x >> 33; return x; }
static void dig(uint64_t a, uint64_t b, uint64_t c) { S.digest = mixh(S.digest ^ mixh(a * 0x100000001B3ull ^ mixh(b ^ (c << 17)))); }
static Thr* cur() { return S.thr[S.cur]; }
int current_id() { return on ? S.cur : 0; }
static uint32_t w32(uint32_t a) {
    V.depth++;
    uint32_t w = (uint32_t)peek8(a) << 24 | (uint32_t)peek8(a + 1) << 16 | (uint32_t)peek8(a + 2) << 8 | peek8(a + 3);
    V.depth--;
    return w;
}
static void put(uint32_t a, int size, uint32_t v) { /* an HLE store (st8/st16/st32): a callee effect, like osm::put32 */
    if (S.nomem) return;
    V.depth++;
    for (int j = 0; j < size; j++) patch8(a + j, (uint8_t)(v >> (8 * (size - 1 - j))));
    V.depth--;
}
static std::string table() {
    std::string s;
    char b[200];
    for (Thr* t : S.thr) {
        snprintf(b, sizeof b, "%sT%d %08X entry %08X core %u prio %d %s", s.empty() ? "" : "; ", t->id, t->guest, t->entry, t->core,
                 t->prio, t->exited ? "exited" : t->suspend > 0 ? "suspended" : wait_name(t->wait));
        s += b;
        if (!t->exited && t->wait != W_NONE && t->wait != W_SLEEP && t->wait != W_DRAIN) { snprintf(b, sizeof b, " %08X", t->wait_obj); s += b; }
        if (!t->exited && (t->wait == W_SLEEP || t->has_deadline)) { snprintf(b, sizeof b, " until %llu", (unsigned long long)t->deadline); s += b; }
    }
    return s;
}
static std::string hex8(uint32_t v) { char b[12]; snprintf(b, sizeof b, "%08X", v); return b; }
static void trace_ev(const char* what, int a, int b) {
    if (g_trace) printf("        thread %s T%d@c%u -> T%d@c%u (clock %llu, event %llu)\n", what, a, S.thr[a]->core, b, S.thr[b]->core,
                        (unsigned long long)S.clock, (unsigned long long)S.events);
}
[[noreturn]] static void abort_run(const std::string& why) {
    V.abort_reason = why;
    if (g_trace) printf("        thread abort: %s\n", why.c_str());
    if (!V.can_abort) { fprintf(stderr, "%s\n", why.c_str()); exit(2); }
    longjmp(V.abort_jmp, 1);
}
static void count_event() {
    if (++S.events > budget)
        abort_run("guest threads: scheduling budget exceeded (" + std::to_string(budget) + " scheduler events in one run; threadbudget): " + table());
}

/* ---------------------------------------------------------------- per-context harness state */
static void save_ctx(Thr* t) {
    t->depth = V.depth;
    t->chain = std::move(V.real_chain);
    V.real_chain.clear();
    t->stack_internal = V.stack_internal;
    t->call_top = V.call_top;
    t->gcpu = gabi::cpu;
    t->fctx = gabi::frame_ctx;
    t->act_depth = gabi::Activation::depth;
    t->act_cur = gabi::Activation::current;
}
static void load_ctx(Thr* t) {
    V.depth = t->depth;
    V.real_chain = std::move(t->chain);
    t->chain.clear();
    V.stack_internal = t->stack_internal;
    V.call_top = t->call_top;
    gabi::cpu = t->gcpu;
    gabi::frame_ctx = t->fctx;
    gabi::Activation::depth = t->act_depth;
    gabi::Activation::current = t->act_cur;
}

static void co_main(void* arg);
static void switch_to(Thr* n, const char* why) {
    Thr* o = cur();
    if (o == n) return;
    S.nswitch++;
    dig(1, (uint64_t)o->id << 32 | (uint32_t)n->id, S.clock);
    trace_ev(why, o->id, n->id);
    save_ctx(o);
    S.cur = n->id;
    load_ctx(n);
    n->slice = 0;
    if (!n->coroutine) {
        void* mem = mmap(nullptr, kHostStack, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (mem == MAP_FAILED) abort_run("guest threads: cannot map a host stack for a guest thread");
        mprotect(mem, 4096, PROT_NONE); /* guard page */
        n->stack_mem = mem;
        n->coroutine = true;
        n->host_sp = initial_frame((char*)mem + kHostStack, co_main, n);
    }
    gthr_switch(&o->host_sp, n->host_sp);
    /* back on o (its context was loaded by whoever switched here) */
}

/* ---------------------------------------------------------------- runnable / eligible */
static bool pred(Thr* t) {
    switch (t->wait) {
    case W_NONE: return true;
    case W_MUTEX: return m_can_lock(S.mx[t->wait_obj], t->id);
    case W_EVENT: case W_SLEEPQ: return t->woken;
    case W_SEND: return !q_full(S.q[t->wait_obj]);
    case W_RECV: return !q_empty(S.q[t->wait_obj]);
    case W_JOIN: return S.thr[t->wait_arg]->exited;
    case W_RDV: return rdv_complete(S.rdv[t->wait_obj], t->wait_arg);
    case W_SLEEP: return false;
    case W_ALARM: { const Alarm* h = alarm_head(S.alarms); return h && h->when <= S.clock; }
    default: return false;
    }
}
static bool runnable_plain(Thr* t) {
    if (t->exited || t->suspend > 0 || t->wait == W_DRAIN || t->wait == W_EXITED) return false;
    if (pred(t)) return true;
    return (t->wait == W_SLEEP || t->has_deadline) && S.clock >= t->deadline;
}
static std::vector<bool> runnable_all() {
    std::vector<bool> r(S.thr.size());
    bool other = false;
    for (Thr* t : S.thr) if ((r[t->id] = runnable_plain(t)) && t->id) other = true;
    if (S.thr[0]->wait == W_DRAIN) r[0] = !other; /* the draining main thread runs again once nobody else can */
    return r;
}
static std::vector<int> eligible(const std::vector<bool>& r) {
    std::vector<int> e;
    for (Thr* t : S.thr) {
        if (!r[t->id]) continue;
        bool shadow = false;
        for (Thr* u : S.thr)
            if (r[u->id] && u->core == t->core && u->prio < t->prio) { shadow = true; break; }
        if (!shadow) e.push_back(t->id);
    }
    return e;
}
/* the next thread: round robin after `from` (from itself last), or a seeded pick */
static int choose(const std::vector<int>& e, int from) {
    if (sched_seed) return e[mixh(sched_seed ^ mixh(S.events * 0x9E3779B97F4A7C15ull)) % e.size()];
    int best = -1, bd = 1 << 30, n = (int)S.thr.size();
    for (int id : e) {
        int d = ((id - from - 1) % n + n) % n; /* distance after `from`; from itself = n - 1 */
        if (d < bd) { bd = d; best = id; }
    }
    return best;
}
/* the earliest clock value after now at which a sleep, timeout or alarm becomes due (UINT64_MAX: none); called whenever a
 * deadline is set or the clock jumps. An early value is harmless (the progress tick that reaches it recomputes) */
static void refresh_due() {
    uint64_t n = UINT64_MAX;
    for (Thr* t : S.thr)
        if (!t->exited && (t->wait == W_SLEEP || t->has_deadline) && t->deadline > S.clock && t->deadline < n) n = t->deadline;
    for (auto& [when, al] : S.alarms.queue) /* the earliest entry after now (a stale one only makes it early) */
        if (when > S.clock) { if (when < n) n = when; break; }
    S.next_due = n;
}
/* reschedule: the current thread blocked, yielded or exited; returns when it runs again */
static void resched(const char* why) {
    count_event();
    for (;;) {
        std::vector<bool> r = runnable_all();
        std::vector<int> e = eligible(r);
        if (!e.empty()) {
            int n = choose(e, S.cur);
            if (n != S.cur) switch_to(S.thr[n], why);
            return;
        }
        uint64_t next = UINT64_MAX;
        for (Thr* t : S.thr)
            if (!t->exited && t->suspend <= 0 && (t->wait == W_SLEEP || t->has_deadline) && t->deadline < next) next = t->deadline;
        if (const Alarm* h = alarm_head(S.alarms); h && h->when < next) next = h->when; /* the next (live) alarm fires */
        if (next == UINT64_MAX) {
            S.deadlock = true;
            abort_run("guest threads: deadlock (non-termination): every thread waits and none waits on the clock: " + table());
        }
        dig(2, next, S.clock);
        if (g_trace) printf("        thread clock %llu -> %llu\n", (unsigned long long)S.clock, (unsigned long long)next);
        S.clock = next;
        refresh_due();
        count_event();
    }
}
/* after an operation that may have made threads runnable: a same-core, strictly higher-priority one runs at once; under
 * threadsched=N also a seeded switch among the eligible threads (the current one included) */
static void sync_point() {
    if (S.thr.size() < 2) return;
    Thr* me = cur();
    std::vector<bool> r = runnable_all();
    int best = -1;
    for (Thr* t : S.thr)
        if (r[t->id] && t != me && t->core == me->core && t->prio < me->prio && (best < 0 || t->prio < S.thr[best]->prio)) best = t->id;
    if (best >= 0) { count_event(); switch_to(S.thr[best], "preempt-wake"); return; }
    if (!sched_seed) return;
    uint64_t h = mixh(sched_seed ^ 0xA5A5 ^ mixh(S.events + 1));
    if (h & 1) return;
    std::vector<int> e = eligible(r);
    if (e.size() < 2) return;
    count_event();
    int n = e[(h >> 8) % e.size()];
    if (n != S.cur) switch_to(S.thr[n], "perturb");
}
/* block the current thread until its wait is over (the port's park_wait: the predicate is re-checked when it runs) */
static void block(Wait w, uint32_t obj, uint32_t arg = 0) {
    Thr* me = cur();
    me->wait = w;
    me->wait_obj = obj;
    me->wait_arg = arg;
    while (!runnable_plain(me)) resched(wait_name(w));
    me->wait = W_NONE;
}

/* a progress tick reached a sleep, timeout or alarm: the timer interrupt's preemption (a woken same-core thread with a
 * strictly higher priority runs at once; as sync_point without the perturbation), else a thread whose deadline just passed
 * and that is eligible on another core runs: on the console that core takes it at this tick */
static void timer_due() {
    refresh_due();
    if (!V.can_abort || cur()->irq_off || S.thr.size() < 2) return;
    Thr* me = cur();
    std::vector<bool> r = runnable_all();
    int best = -1;
    for (Thr* t : S.thr)
        if (r[t->id] && t != me && t->core == me->core && t->prio < me->prio && (best < 0 || t->prio < S.thr[best]->prio)) best = t->id;
    if (best >= 0) {
        count_event();
        dig(11, best, S.clock);
        switch_to(S.thr[best], "preempt-timer");
        return;
    }
    std::vector<int> o;
    for (int id : eligible(r)) {
        Thr* t = S.thr[id];
        if (t->core != me->core && (t->wait == W_SLEEP || t->wait == W_ALARM || t->has_deadline)) o.push_back(id);
    }
    if (o.empty()) return;
    count_event();
    int n = choose(o, S.cur);
    dig(12, n, S.clock);
    switch_to(S.thr[n], "wake-timer");
}
/* slice preemption (vm_ld/vm_st): a runnable thread on another core runs */
void slice_hit() {
    Thr* me = cur();
    if (++me->slice < S.slice_run || !V.can_abort) return;
    me->slice = 0;
    count_event(); /* a spin with nobody to switch to still ends at threadbudget */
    if (me->irq_off) return;
    std::vector<bool> r = runnable_all();
    std::vector<int> e = eligible(r), o;
    for (int id : e) if (S.thr[id]->core != me->core) o.push_back(id);
    if (o.empty()) return;
    count_event();
    S.npreempt++;
    dig(3, me->id, S.events);
    switch_to(S.thr[choose(o, S.cur)], "preempt-slice");
}

/* after every modelled OS call: the progress clock (threadrate ticks for a time read, else 1; reaching a deadline: timer_due),
 * then the slice */
static void os_progress(bool time_read = false) {
    if (clock_rate) {
        S.clock += time_read ? clock_rate : 1;
        if (S.clock >= S.next_due) timer_due();
    }
    if (g_multi) slice_hit();
}

/* ---------------------------------------------------------------- run lifecycle */
static void free_threads() {
    for (Thr* t : S.thr) {
        if (t->stack_mem) munmap(t->stack_mem, kHostStack);
        delete t;
    }
    S.thr.clear();
}
static Thr* new_main() {
    Thr* m = new Thr();
    m->id = 0;
    m->guest = main_obj;
    m->entry = kRpxEntry;
    m->core = kMainCore;
    m->prio = kMainPrio;
    m->suspend = 0;
    m->stack_top = kMainStackTop;
    m->stack_size = kMainStackSize;
    m->attr = 2;
    m->coroutine = true; /* the harness's own stack */
    return m;
}
/* every run starts from the boot state: the main thread only, no objects */
void reset() {
    if (!on) return;
    free_threads();
    bool nomem = S.nomem;
    S = State();
    S.nomem = nomem;
    S.thr.push_back(new_main());
    if (AX) *AX = axm::State();
    ax_host_top = kAxHostLo;
    ax_thread = -1;
    ax_t0 = 0;
    S.clock = clock_origin + (sched_seed ? (mixh(sched_seed ^ 0xC10C) % 0x100000000ull) << 8 : 0);
    S.slice_run = sched_seed ? 1ull << (8 + mixh(sched_seed ^ 0x511CE) % 15) : slice_len;
    g_multi = 0;
}
/* the header line of a run in a non-default mode (stderr, once per process) */
void announce() {
    if (!on) return;
    if (sched_seed)
        fprintf(stderr, "guestthreads: threadsched=%llu: a perturbed schedule (seeded picks, sync-point switches, slice length and clock "
                        "origin per run)\n", (unsigned long long)sched_seed);
}
/* why the post-setup state cannot be captured ("" = it is the boot state) */
std::string live_reason() {
    if (!on || !S.touched) return "";
    int n = 0;
    for (Thr* t : S.thr) n += t->id && !t->exited;
    return "guest threads: the setups left guest-thread state (" + std::to_string(S.thr.size() - 1) + " thread(s) created, " +
           std::to_string(n) + " alive; " + std::to_string(S.q.size() + S.mx.size() + S.ev.size() + S.rdv.size()) +
           " object(s)); thread contexts are not part of a snapshot";
}
/* the main thread waits until nobody else can run */
void drain() {
    if (!on || !drain_on || S.thr.size() < 2 || S.cur != 0) return;
    Thr* m = S.thr[0];
    m->wait = W_DRAIN;
    for (;;) {
        std::vector<bool> r = runnable_all();
        if (r[0]) break;
        resched("drain");
    }
    m->wait = W_NONE;
}
/* the guest stacks of the threads that ran: private, not part of the net effect */
bool in_thread_stack(uint32_t ea) {
    for (size_t i = 1; i < S.thr.size(); i++) {
        Thr* t = S.thr[i];
        if (t->coroutine && ea - (t->stack_top - t->stack_size) < t->stack_size) return true;
    }
    return false;
}

/* ---------------------------------------------------------------- unit functions reached from a thread */
static std::unordered_map<uint32_t, void (*)(Cpu*)> g_orig_fn, g_cand_fn;
static void unit_maps() {
    static bool done = false;
    if (done) return;
    done = true;
    for (unsigned i = 0; i < unit_nfuncs; i++) g_orig_fn[unit_funcs[i].addr] = unit_funcs[i].fn;
    for (gabi::Candidate* c = gabi::g_cands; c; c = c->next) g_cand_fn[c->addr] = c->fn;
}
/* in a guest thread: the unit's own function at TARGET for this side, or nullptr. Candidate code starts only at the tested
 * call (2026-11-02): during setups, postsetups and their drain both sides run the original, so a mutant cannot change a
 * setup on the candidate side alone. A function already on a thread's stack when the tested call starts stays original until
 * it returns; what it calls from then on is this side's code. */
bool g_tested = false; /* the tested call has started in this run (harness) */
static std::vector<uint32_t> g_on_stack; /* the unit functions on a thread's stack when it started (RunInfo::on_stack) */
void tested_reset() { g_tested = false; g_on_stack.clear(); }
void (*unit_fn(uint32_t target))(Cpu*) {
    if (!on || S.cur == 0) return nullptr;
    unit_maps();
    auto o = g_orig_fn.find(target);
    if (o == g_orig_fn.end()) return nullptr;
    if (V.is_candidate && g_tested) {
        auto c = g_cand_fn.find(target);
        return c != g_cand_fn.end() ? c->second : o->second;
    }
    return o->second;
}

/* the tested call starts: from here a guest thread runs this side's code, except what is already on its stack */
void tested_starts() {
    g_tested = true;
    if (!on) return;
    unit_maps();
    for (size_t i = 1; i < S.thr.size(); i++)
        for (uint32_t a : S.thr[i]->chain)
            if (g_orig_fn.count(a)) g_on_stack.push_back(a);
}

/* ---------------------------------------------------------------- thread bodies */
static void thread_exit(uint32_t value) {
    Thr* me = cur();
    me->exited = true;
    me->exit_value = value;
    me->wait = W_EXITED;
    put(me->guest + kExitValue, 4, value);
    put(me->guest + kState, 1, ST_MORIBUND);
    dig(4, me->id, value);
    if (g_trace) printf("        thread T%d@c%u exited with %08X\n", me->id, me->core, value);
    resched("exit");
    abort_run("guest threads: an exited thread was scheduled"); /* not reached */
}
static void run_entry(Thr* t, uint32_t target, Cpu* c);
static void co_main(void* arg) {
    Thr* t = (Thr*)arg;
    if (t->host_entry) {
        t->host_entry(t);
        thread_exit(t->cpu.r[3]);
    }
    Cpu* c = &t->cpu;
    c->r[3] = t->argc;
    c->r[4] = t->argv;
    c->lr = 0;
    run_entry(t, t->entry, c);
    thread_exit(c->r[3]);
}
/* run guest code at TARGET in thread t's context (a thread entry or an alarm callback): `real` or a unit function */
static void run_entry(Thr*, uint32_t target, Cpu* c) {
    c->lr = 0;
    void (*fn)(Cpu*) = unit_fn(target);
    if (!fn) {
        const CalleeInfo* ci = callee_info(target);
        if (ci && ci->real) fn = ci->real;
    }
    if (!fn) {
        char b[160];
        snprintf(b, sizeof b, "osmem threads: thread entry or alarm callback %08X is neither `real` nor a unit function (list it `real`)", target);
        model_reject(target, b);
    }
    V.real_chain.push_back(target);
    fn(c);
    V.real_chain.pop_back();
}

/* ---------------------------------------------------------------- operations (host values; the imports wrap them) */
Thr* op_create(const Cpu* creator, uint32_t t, uint32_t entry, uint32_t argc, uint32_t argv, uint32_t stack_top, uint32_t stack_size, int prio, uint32_t attr) {
    if (S.thr.size() >= cap)
        abort_run("guest threads: more than " + std::to_string(cap) + " threads in one run (threadcap): " + table());
    S.touched = true;
    Thr* n = new Thr();
    n->id = (int)S.thr.size();
    n->guest = t;
    n->entry = entry;
    n->argc = argc;
    n->argv = argv;
    n->stack_top = stack_top;
    n->stack_size = stack_size;
    n->prio = prio;
    n->attr = attr;
    n->core = core_from_affinity(attr, cur()->core);
    /* init_cpu: r1 below the stack top, the SDA bases of the creator, coreinit's GQR/FPSCR defaults */
    n->cpu.r[1] = (stack_top - 0x20) & ~0xFu;
    n->cpu.r[2] = creator ? creator->r[2] : 0;
    n->cpu.r[13] = creator ? creator->r[13] : 0;
    n->cpu.gqr[2] = 0x40004; n->cpu.gqr[3] = 0x50005; n->cpu.gqr[4] = 0x60006; n->cpu.gqr[5] = 0x70007;
    n->cpu.fpscr = 4;
    n->cpu.core = n->core;
    uint16_t id = S.next_id++;
    if (!S.nomem) create_words(t, entry, stack_top, stack_size, (uint32_t)prio, attr, id, [](uint32_t a, int sz, uint32_t v) { put(a, sz, v); });
    /* a second OSCreateThread on the same block replaces the object (g_threads[t] = ht) */
    S.by_guest[t] = n->id;
    S.thr.push_back(n);
    g_multi = 1;
    dig(5, n->id, entry);
    if (g_trace) printf("        thread T%d created: OSThread %08X entry %08X stack %08X+%X prio %d attr %X core %u\n", n->id, t, entry,
                        stack_top - stack_size, stack_size, prio, attr, n->core);
    return n;
}
static Thr* by_guest(uint32_t t) {
    if (t == main_obj) return S.thr[0];
    auto it = S.by_guest.find(t);
    return it == S.by_guest.end() ? nullptr : S.thr[it->second];
}
static int op_resume(uint32_t t, bool* known) {
    Thr* h = by_guest(t);
    *known = h != nullptr;
    if (!h) return 0;
    int prev = h->suspend;
    if (h->suspend > 0) h->suspend--;
    put(h->guest + kSuspend, 4, (uint32_t)h->suspend);
    S.touched = true;
    sync_point();
    return prev;
}
static bool op_join(uint32_t t, uint32_t* value) {
    Thr* h = by_guest(t);
    if (!h) return false;
    if (!h->exited) block(W_JOIN, t, (uint32_t)h->id);
    *value = h->exit_value;
    return true;
}
static void op_yield() {
    count_event();
    std::vector<bool> r = runnable_all();
    std::vector<int> e = eligible(r);
    if (e.empty()) return;
    int n = choose(e, S.cur);
    if (n != S.cur) switch_to(S.thr[n], "yield");
}
static void op_sleep(uint64_t ticks, const char* why = "OSSleepTicks") {
    Thr* me = cur();
    S.touched = true;
    me->deadline = S.clock + ticks;
    me->wait = W_SLEEP;
    refresh_due();
    while (S.clock < me->deadline) resched(why);
    me->wait = W_NONE;
}
static bool op_send(uint32_t qa, const Msg& m, uint32_t flags) {
    Queue& q = S.q[qa];
    S.touched = true;
    if (q_full(q)) {
        if (!(flags & 1)) return false;
        block(W_SEND, qa);
    }
    q_put(S.q[qa], m, flags);
    sync_point();
    return true;
}
static bool op_recv(uint32_t qa, uint32_t flags, Msg* m) {
    S.touched = true;
    if (q_empty(S.q[qa])) {
        if (!(flags & 1)) return false;
        block(W_RECV, qa);
    }
    *m = q_take(S.q[qa]);
    sync_point();
    return true;
}
static void op_lock(uint32_t a) {
    S.touched = true;
    if (!m_can_lock(S.mx[a], S.cur)) block(W_MUTEX, a);
    m_acquire(S.mx[a], S.cur);
}
static bool op_trylock(uint32_t a) { S.touched = true; return m_trylock(S.mx[a], S.cur); }
static void op_unlock(uint32_t a) {
    S.touched = true;
    m_unlock(S.mx[a]);
    sync_point();
}
static void op_signal(uint32_t a) {
    S.touched = true;
    std::vector<int> w;
    e_signal(S.ev[a], w);
    for (int id : w) S.thr[id]->woken = true;
    sync_point();
}
static bool op_wait_event(uint32_t a, bool timed, uint64_t ticks) {
    S.touched = true;
    if (e_try(S.ev[a])) return true;
    Thr* me = cur();
    me->woken = false;
    S.ev[a].waiters.push_back(me->id);
    if (timed) { me->has_deadline = true; me->deadline = S.clock + ticks; }
    me->wait = W_EVENT;
    me->wait_obj = a;
    if (timed) refresh_due();
    while (!runnable_plain(me)) resched("OSWaitEvent");
    me->wait = W_NONE;
    me->has_deadline = false;
    if (me->woken) return true;
    e_cancel(S.ev[a], me->id); /* timed out */
    return false;
}
static void op_rdv_wait(uint32_t a, uint32_t mask) {
    S.touched = true;
    rdv_arrive(S.rdv[a], cur()->core);
    sync_point();
    if (!rdv_complete(S.rdv[a], mask)) block(W_RDV, a, mask);
}
static void op_sleepq(uint32_t a) {
    S.touched = true;
    Thr* me = cur();
    me->woken = false;
    S.sleepq[a].push_back(me->id);
    block(W_SLEEPQ, a);
}

/* ---------------------------------------------------------------- alarms */
uint32_t service_obj = 0x6FFE1800u; /* the alarm service thread's OSThread block (stack below 0x6FFFF000) */
static const uint32_t kServiceStackTop = 0x6FFFF000u, kServiceStackSize = 0x1C000u;
static void run_entry(Thr* t, uint32_t target, Cpu* c);
static void (*g_host_alarm_cb)(uint32_t) = nullptr;
/* a service thread's Cpu as threads.cpp make_service_cpu -> init_cpu leaves it: r1 below the stack top, the module's SDA
 * bases (taken from the guest thread that started the service), coreinit's GQR/FPSCR defaults; kept across callbacks */
static void service_cpu(Thr* t, const Cpu* from) {
    memset(&t->cpu, 0, sizeof t->cpu);
    t->cpu.r[1] = (t->stack_top - 0x20) & ~0xFu;
    t->cpu.r[2] = from ? from->r[2] : 0;
    t->cpu.r[13] = from ? from->r[13] : 0;
    t->cpu.gqr[2] = 0x40004; t->cpu.gqr[3] = 0x50005; t->cpu.gqr[4] = 0x60006; t->cpu.gqr[5] = 0x70007;
    t->cpu.fpscr = 4;
    t->cpu.core = t->core;
}
/* core.cpp guest_call on a service thread's persistent Cpu: a 0x40-byte frame with its back chain below the current r1, the
 * arguments in r3.., lr 0; r1, lr and ctr restored afterwards (the other registers keep what the callback left) */
static void service_call(Thr* t, uint32_t fn, int n, const uint32_t* args) {
    Cpu* c = &t->cpu;
    uint32_t save_lr = c->lr, save_ctr = c->ctr, save_sp = c->r[1];
    c->r[1] -= 0x40;
    put(c->r[1], 4, save_sp);
    for (int i = 0; i < n; i++) c->r[3 + i] = args[i];
    c->lr = 0;
    c->pc = fn;
    dig(9, fn, S.clock);
    run_entry(t, fn, c);
    c->r[1] = save_sp;
    c->lr = save_lr;
    c->ctr = save_ctr;
}
static void service_body(Thr* t) {
    for (;;) {
        t->wait = W_ALARM;
        while (!runnable_plain(t)) resched("alarm service idle");
        t->wait = W_NONE;
        Alarm al;
        if (!alarm_pop_due(S.alarms, S.clock, &al)) continue;
        refresh_due(); /* a periodic alarm re-armed */
        t->core = al.core; /* threads::set_service_core(a.core) */
        dig(6, al.guest, S.clock);
        if (g_trace) printf("        thread alarm %08X fires on T%d@c%u (callback %08X, clock %llu)\n", al.guest, t->id, t->core, al.callback,
                            (unsigned long long)S.clock);
        if (g_host_alarm_cb) { g_host_alarm_cb(al.guest); continue; } /* -gthrselftest */
        uint32_t args[2] = {al.guest, 0};
        service_call(t, al.callback, 2, args); /* guest_call(c, a.callback, {a.guest, 0}) */
    }
}
static void arm_alarm(const Cpu* c, uint32_t alarm, uint64_t when, uint64_t period, uint32_t cb) {
    S.touched = true;
    if (S.service < 0) { /* threads::make_service_cpu("alarm"): priority -1 (preempts guest threads) */
        if (S.thr.size() >= cap) abort_run("guest threads: more than " + std::to_string(cap) + " threads in one run (threadcap): " + table());
        Thr* n = new Thr();
        n->id = (int)S.thr.size();
        n->guest = service_obj;
        n->entry = 0;
        n->prio = -1;
        n->suspend = 0;
        n->stack_top = kServiceStackTop;
        n->stack_size = kServiceStackSize;
        n->core = cur()->core;
        n->host_entry = service_body;
        n->wait = W_ALARM;
        /* make_service_cpu -> create_thread(t, 0, 0, 0, top, size, 0, 0x7): the block's words (the name string is host-only) */
        if (!S.nomem) create_words(service_obj, 0, kServiceStackTop, kServiceStackSize, 0, 7, S.next_id++,
                                   [](uint32_t a, int sz, uint32_t v) { put(a, sz, v); });
        service_cpu(n, c);
        S.thr.push_back(n);
        S.service = n->id;
        g_multi = 1;
        if (g_trace) printf("        thread T%d: alarm service thread (priority -1)\n", n->id);
    }
    alarm_arm(S.alarms, alarm, when, period, cb, cur()->core);
    refresh_due();
    dig(7, alarm, when);
}

/* ---------------------------------------------------------------- `osmem ... ax`: snd_core and the AX frame thread
 * (ax_model.h; the port's runtime/src/hle/ax.cpp at f5554821). AXInit starts the "AX frame" service thread as the port does
 * (make_service_cpu: priority -1, set_service_core(0)); it runs one frame at once and then every 3 ms of virtual time
 * (frame k at the AXInit tick + floor(k * 3 * 62156250 / 1000)), until AXQuit. The frame's guest stores are HLE effects
 * like the other osmem models'; its callbacks (aux, app frame, final mix) run as guest code on that thread. The port's
 * host_alloc buffers (the GamePad aux buffers) and the thread's own block and stack sit in the zeroed region
 * kAxHostLo..kAxHostHi (6FE00000..6FE40000): GamePad aux buffers from 6FE00000, the OSThread block at 6FE10000, the guest
 * stack 6FE20000..6FE40000. */
static const uint32_t kAxThreadObj = 0x6FE10000u, kAxStackTop = 0x6FE40000u, kAxStackSize = 0x20000u;
struct AxEnv {
    uint32_t target = 0;
    uint8_t ld8(uint32_t a) { return peek8(a); }
    uint16_t ld16(uint32_t a) { return (uint16_t)(peek8(a) << 8 | peek8(a + 1)); }
    uint32_t ld32(uint32_t a) { return w32(a); }
    void st16(uint32_t a, uint32_t v) { put(a, 2, v); }
    void st32(uint32_t a, uint32_t v) { put(a, 4, v); }
    void zero(uint32_t a, uint32_t n) {
        if (S.nomem) return;
        V.depth++;
        for (uint32_t j = 0; j < n; j++) patch8(a + j, 0);
        V.depth--;
    }
    uint32_t runtime_alloc(uint32_t size, uint32_t align) { return osm::bump_alloc(target, size, align); }
    uint32_t host_alloc(uint32_t size, uint32_t align) {
        uint32_t p = (ax_host_top + align - 1) & ~(align - 1);
        if ((uint64_t)p + size > kAxThreadObj) model_reject(target, "osmem ax: the host_alloc region 6FE00000..6FE10000 is exhausted");
        ax_host_top = p + size;
        zero(p, size); /* (zeroed: the region's init words are zero; as the port's bump() memset) */
        return p;
    }
    void call(uint32_t fn, int n, const uint32_t* args) { /* guest_call on the AX frame thread */
        Thr* t = cur();
        if (g_trace) printf("        thread ax callback %08X (r3=%08X) on T%d@c%u\n", fn, n ? args[0] : 0, t->id, t->core);
        service_call(t, fn, n, args);
    }
};
static uint64_t ax_frame_tick(uint64_t k) { return ax_t0 + k * 3ull * timebase_ticks / 1000ull; }
static void ax_body(Thr* t) {
    for (uint64_t k = 0;; k++) {
        t->deadline = ax_frame_tick(k);
        t->wait = W_SLEEP;
        refresh_due();
        while (S.clock < t->deadline) resched("ax frame idle");
        t->wait = W_NONE;
        if (!AX->running) thread_exit(0); /* while (g_running) */
        dig(8, k, S.clock);
        if (g_trace) printf("        thread ax frame %llu on T%d@c%u (clock %llu)\n", (unsigned long long)k, t->id, t->core,
                            (unsigned long long)S.clock);
        AxEnv e;
        e.target = axm::AXInit;
        axm::frame(e, *AX);
    }
}
bool ax_call(Cpu* c, uint32_t target, const uint32_t* a) {
    if (!axm::is_import(target)) return false;
    if (!AX) AX = new axm::State();
    S.touched = true;
    AxEnv e;
    e.target = target;
    uint32_t r = 0;
    bool started = false;
    int k = axm::call(e, *AX, target, a, c->f[1].ps0, &r, &started);
    if (k < 0) return false;
    c->r[3] = k ? r : a[3];
    if (g_trace) printf("        %sax T%d@c%u %08X r3=%08X r4=%08X r5=%08X -> %08X\n", V.depth ? "nested " : "", S.cur, cur()->core, target,
                        a[3], a[4], a[5], c->r[3]);
    if (started) { /* std::thread(frame_thread): make_service_cpu("AX frame", 0x20000), set_service_core(0) */
        if (S.thr.size() >= cap) abort_run("guest threads: more than " + std::to_string(cap) + " threads in one run (threadcap): " + table());
        Thr* n = new Thr();
        n->id = (int)S.thr.size();
        n->guest = kAxThreadObj;
        n->prio = -1;
        n->suspend = 0;
        n->stack_top = kAxStackTop;
        n->stack_size = kAxStackSize;
        n->core = 0;
        n->host_entry = ax_body;
        if (!S.nomem) create_words(kAxThreadObj, 0, kAxStackTop, kAxStackSize, 0, 7, S.next_id++,
                                   [](uint32_t aa, int sz, uint32_t v) { put(aa, sz, v); });
        service_cpu(n, c);
        S.thr.push_back(n);
        ax_thread = n->id;
        ax_t0 = S.clock;
        g_multi = 1;
        dig(10, n->id, S.clock);
        if (g_trace) printf("        thread T%d: AX frame thread (priority -1, core 0), first frame at clock %llu\n", n->id,
                            (unsigned long long)S.clock);
        sync_point();
    }
    return true;
}

/* ---------------------------------------------------------------- the imports */
enum : uint32_t {
    I_CREATE = 0xC0009A48u, I_RESUME = 0xC0009F38u, I_EXIT = 0xC0009B30u, I_JOIN = 0xC0009E00u, I_CURRENT = 0xC0009BC8u,
    I_COREID = 0xC0009BB0u, I_YIELD = 0xC000A248u, I_SLEEPTICKS = 0xC000A0D0u, I_SETNAME = 0xC000A078u, I_SETAFF = 0xC000A058u,
    I_GETPRIO = 0xC0009CC0u, I_SETSPEC = 0xC000A090u, I_GETSPEC = 0xC0009CC8u, I_BLOCKEXIT = 0xC0009998u, I_DISINT = 0xC0009A60u,
    I_RESTINT = 0xC0009F30u, I_BARRIER = 0xC0009E90u, I_INITMUTEX = 0xC0009D68u, I_LOCK = 0xC0009E40u, I_TRYLOCK = 0xC000A198u,
    I_UNLOCK = 0xC000A1C8u, I_GHSLOCK = 0xC000A778u, I_GHSUNLOCK = 0xC000A780u, I_MTXLOCK = 0xC000A7D8u, I_MTXUNLOCK = 0xC000A7E0u,
    I_FLOCK = 0xC000A7A8u, I_FUNLOCK = 0xC000A7C0u, I_INITEVENT = 0xC0009D48u, I_SIGNAL = 0xC000A0B0u, I_RESETEV = 0xC0009F18u,
    I_WAITEV = 0xC000A1F0u, I_WAITEVTO = 0xC000A1F8u, I_INITQ = 0xC0009D58u, I_SEND = 0xC0009FC8u, I_RECV = 0xC0009ED0u,
    I_INITTQ = 0xC0009DA0u, I_SLEEPTHREAD = 0xC000A0C8u, I_INITRDV = 0xC0009D78u, I_WAITRDV = 0xC000A208u, I_GETTIME = 0xC0009CD8u,
    I_SYSTIME = 0xC0009C98u, I_GETTICK = 0xC0009CD0u, I_CREATEALARM = 0xC0009A38u, I_SETALARM = 0xC0009FD8u,
    I_SETPERIODIC = 0xC000A038u, I_CANCELALARM = 0xC00099A8u, I_SETALARMDATA = 0xC0009FF0u, I_GETALARMDATA = 0xC0009B70u,
    I_VSYNC = 0xC0006AE8u, /* GX2WaitForVsync (gx2) */
};
static const uint32_t kImports[] = {I_CREATE, I_RESUME, I_EXIT, I_JOIN, I_CURRENT, I_COREID, I_YIELD, I_SLEEPTICKS, I_SETNAME, I_SETAFF,
    I_GETPRIO, I_SETSPEC, I_GETSPEC, I_BLOCKEXIT, I_DISINT, I_RESTINT, I_BARRIER, I_INITMUTEX, I_LOCK, I_TRYLOCK, I_UNLOCK, I_GHSLOCK,
    I_GHSUNLOCK, I_MTXLOCK, I_MTXUNLOCK, I_FLOCK, I_FUNLOCK, I_INITEVENT, I_SIGNAL, I_RESETEV, I_WAITEV, I_WAITEVTO, I_INITQ, I_SEND,
    I_RECV, I_INITTQ, I_SLEEPTHREAD, I_INITRDV, I_WAITRDV, I_GETTIME, I_SYSTIME, I_GETTICK, I_CREATEALARM, I_SETALARM, I_SETPERIODIC,
    I_CANCELALARM, I_SETALARMDATA, I_GETALARMDATA, I_VSYNC};
/* the import slot a target means (a slot, or its stub), 0 when not a thread import */
static uint32_t slot_of(uint32_t t) {
    for (uint32_t s : kImports) if (t == s || t == stub_of(s)) return s;
    return 0;
}
bool models(uint32_t t) { return on && slot_of(t) != 0; }

uint64_t clock_now() { return S.clock; }

/* GX2WaitForVsync (2026-11-02, J3D start-up: the main thread never blocked, so a lower-priority worker on its core never ran
 * again): as the port's gx2_core.cpp (f5554821 and later) without the debug/benchmark switches, the caller sleeps until the
 * next vsync. The port's vsync k is at k * 16683333 ns (59.94 Hz) after its epoch; here vsync k is at model tick
 * floor(k * 16683333 * 62156250 / 10^9) after clock 0. Only GX2WaitForVsync blocks: the port's GX2DrawDone waits for the
 * host GPU (no guest time; the verifier has none) and it has no GX2WaitForFlip. Frame interpolation (120/240 fps virtual
 * vsyncs) is not modelled: 60 Hz, the console's rate. */
static uint64_t vsync_tick(uint64_t k) { return (uint64_t)((unsigned __int128)k * 16683333u * timebase_ticks / 1000000000u); }
static uint64_t next_vsync(uint64_t now) {
    uint64_t k = (uint64_t)((unsigned __int128)now * 1000000000u / ((unsigned __int128)16683333u * timebase_ticks));
    while (vsync_tick(k) <= now) k++;
    return vsync_tick(k);
}

/* one modelled call; registers as the port's HLE leaves them (r3, and r4 for the 64-bit time results) */
bool call(Cpu* c, uint32_t target, const uint32_t* a) {
    uint32_t s = slot_of(target);
    if (!on || !s) return false;
    uint32_t r = c->r[3];
    bool ret = true;
    auto arg64 = [&](int n) { return (uint64_t)a[n] << 32 | a[n + 1]; };
    switch (s) {
    case I_CREATE: { /* (thread, entry, argc, argv, stack, stack_size, prio, attr) */
        op_create(c, a[3], a[4], a[5], a[6], a[7], a[8], (int)a[9], a[10]);
        r = 1;
        sync_point();
        break;
    }
    case I_RESUME: { bool known; r = (uint32_t)op_resume(a[3], &known); break; }
    case I_EXIT:
        if (S.cur == 0) model_reject(target, "osmem threads: OSExitThread on the main thread");
        thread_exit(a[3]);
        break;
    case I_JOIN: {
        uint32_t v = 0;
        if (!op_join(a[3], &v)) { r = 0; break; }
        if (a[4]) put(a[4], 4, v);
        r = 1;
        break;
    }
    case I_CURRENT: r = cur()->guest; break;
    case I_COREID: r = cur()->core; break;
    case I_YIELD: op_yield(); ret = false; break;
    case I_SLEEPTICKS: op_sleep(arg64(3)); ret = false; break;
    case I_SETNAME: put(a[3] + kName, 4, a[4]); ret = false; break;
    case I_SETAFF: {
        put(a[3] + kAffinity, 4, a[4]);
        if (Thr* h = by_guest(a[3])) h->core = core_from_affinity(a[4], h->core);
        r = 1;
        break;
    }
    case I_GETPRIO: r = w32(a[3] + kBasePrio); break;
    case I_SETSPEC: put(cur()->guest + kSpecific + 4 * a[3], 4, a[4]); ret = false; break;
    case I_GETSPEC: r = w32(cur()->guest + kSpecific + 4 * a[3]); break;
    case I_BLOCKEXIT: case I_BARRIER: ret = false; break;
    case I_DISINT: r = cur()->irq_off ? 0 : 1; cur()->irq_off = 1; break;
    case I_RESTINT: r = cur()->irq_off ? 0 : 1; if (a[3]) cur()->irq_off = 0; break;
    case I_INITMUTEX: S.touched = true; m_init(S.mx[a[3]]); put(a[3], 4, 0x6D557458u); ret = false; break;
    case I_LOCK: op_lock(a[3]); ret = false; break;
    case I_TRYLOCK: r = op_trylock(a[3]); break;
    case I_UNLOCK: op_unlock(a[3]); ret = false; break;
    case I_GHSLOCK: op_lock(0xC0FFEE00u); ret = false; break;
    case I_GHSUNLOCK: op_unlock(0xC0FFEE00u); ret = false; break;
    case I_MTXLOCK: op_lock(w32(a[3])); ret = false; break;
    case I_MTXUNLOCK: op_unlock(w32(a[3])); ret = false; break;
    case I_FLOCK: op_lock(0xC0FFEE10u); ret = false; break;
    case I_FUNLOCK: op_unlock(0xC0FFEE10u); ret = false; break;
    case I_INITEVENT: S.touched = true; e_init(S.ev[a[3]], a[4] != 0, a[5] != 0); put(a[3], 4, 0x65566E54u); ret = false; break;
    case I_SIGNAL: op_signal(a[3]); ret = false; break;
    case I_RESETEV: S.touched = true; e_reset(S.ev[a[3]]); ret = false; break;
    case I_WAITEV: op_wait_event(a[3], false, 0); ret = false; break;
    case I_WAITEVTO: r = op_wait_event(a[3], true, arg64(5)); break;
    case I_INITQ: S.touched = true; q_init(S.q[a[3]], a[5]); put(a[3], 4, 0x6D536751u); ret = false; break;
    case I_SEND: {
        Msg m{w32(a[4]), w32(a[4] + 4), w32(a[4] + 8), w32(a[4] + 12)};
        r = op_send(a[3], m, a[5]);
        break;
    }
    case I_RECV: {
        Msg m;
        if (!op_recv(a[3], a[5], &m)) { r = 0; break; }
        for (int i = 0; i < 4; i++) put(a[4] + 4 * i, 4, m[i]);
        r = 1;
        break;
    }
    case I_INITTQ: ret = false; break;
    case I_SLEEPTHREAD: op_sleepq(a[3]); ret = false; break;
    case I_INITRDV: S.touched = true; S.rdv[a[3]] = Rendezvous(); ret = false; break;
    case I_WAITRDV: op_rdv_wait(a[3], a[4]); r = 1; break;
    case I_GETTIME: { /* os_time::now(): the calendar clock */
        uint64_t tt = S.clock + (uint64_t)os_bias;
        c->r[3] = (uint32_t)(tt >> 32); c->r[4] = (uint32_t)tt; goto done;
    }
    case I_SYSTIME: c->r[3] = (uint32_t)(S.clock >> 32); c->r[4] = (uint32_t)S.clock; goto done;
    case I_GETTICK: r = (uint32_t)S.clock; break;
    /* alarms: OSAlarm layout (threads.cpp, after Cemu): +0x0C callback, +0x38 userData; the 0x58-byte block zeroed with "aLrM" */
    case I_CREATEALARM: for (uint32_t i = 0; i < 0x58; i += 4) put(a[3] + i, 4, 0); put(a[3], 4, 0x614C724Du); ret = false; break;
    case I_SETALARM: /* (alarm, delay r5:r6, callback r7) */
        put(a[3] + 0x0C, 4, a[7]);
        arm_alarm(c, a[3], S.clock + arg64(5), 0, a[7]);
        r = 1;
        break;
    case I_SETPERIODIC: { /* (alarm, start r5:r6 an OSTime, period r7:r8, callback r9) */
        put(a[3] + 0x0C, 4, a[9]);
        uint64_t start = os_to_guest(arg64(5), os_bias), period = arg64(7), when = start;
        if (when < S.clock && period) when += ((S.clock - start) / period + 1) * period;
        arm_alarm(c, a[3], when, period, a[9]);
        r = 1;
        break;
    }
    case I_CANCELALARM: S.touched = true; alarm_cancel(S.alarms, a[3]); r = 1; break;
    case I_SETALARMDATA: put(a[3] + 0x38, 4, a[4]); ret = false; break;
    case I_GETALARMDATA: r = w32(a[3] + 0x38); break;
    case I_VSYNC: op_sleep(next_vsync(S.clock) - S.clock, "GX2WaitForVsync"); ret = false; break;
    default: return false;
    }
    c->r[3] = ret ? r : a[3];
done:
    if (g_trace) printf("        %sthreads T%d@c%u %08X r3=%08X r4=%08X r5=%08X -> %08X\n", V.depth ? "nested " : "", S.cur, cur()->core, target, a[3],
                        a[4], a[5], c->r[3]);
    /* a modelled OS call is progress of the running thread too (2026-11-02, J3D start-up): the slice also expires at the
     * return of an OS call, so a spin whose only guest accesses lie inside a lock (`unlock; lock; flag = *p;`) can hand
     * over right after its unlock, when a waiter on another core can take the mutex, as that core would on the console */
    os_progress(s == I_GETTIME || s == I_SYSTIME || s == I_GETTICK);
    return true;
}

/* the per-thread call log (threads other than main), from the tested call on (2026-11-02: setups, postsetups and their drain run
 * the same originals on both sides, so their calls compare nothing, and a busy thread filled threadlogcap with them; the schedule
 * digest still covers them): every call that is not run for real, with r3..r10 (a value inside
 * the calling thread's own stack reads as 5A5A0000 when it lies in that stack) and r3 after the call. compare() checks
 * the argument registers by the main comparison's rule (harness call_mask: the callee's argument mask, plus the
 * candidate's declared argument count for a declared or unlisted callee; 2026-11-02, the mask alone missed the
 * arguments of declared imports) */
int log_begin(Cpu* c, uint32_t target, int kind, int nint, int nflt) {
    if (!on || S.cur == 0 || !g_tested) return -1; /* from the tested call on: before it both sides run the same originals */
    Thr* me = cur();
    LogEv e{};
    e.target = target;
    e.kind = kind;
    e.nint = nint;
    e.nflt = nflt;
    if (++S.nlog > log_cap)
        abort_run("guest threads: the per-thread call logs exceed " + std::to_string(log_cap) + " entries in one run (threadlog): T" +
                  std::to_string(me->id) + " (entry " + hex8(me->entry) + ") has " + std::to_string(me->log.size()) +
                  " of them; last call " + hex8(target) + ": " + table());
    for (int i = 3; i <= 10; i++) {
        uint32_t v = c->r[i];
        if (v - (me->stack_top - me->stack_size) < me->stack_size) v = 0x5A5A0000u; /* a pointer into its own stack */
        e.a[i - 3] = v;
    }
    me->log.push_back(e);
    return (int)me->log.size() - 1;
}
void log_end(int i, Cpu* c) {
    if (i < 0) return;
    cur()->log[i].r3 = c->r[3];
}

/* ---------------------------------------------------------------- results */
struct RunInfo {
    bool used = false, deadlock = false;
    int fault_thread = 0;
    uint64_t digest = 0;
    uint32_t nthreads = 0;
    uint64_t nswitch = 0, npreempt = 0, events = 0;
    std::vector<std::vector<LogEv>> logs; /* by thread id (1..) */
    std::vector<uint32_t> entries;
    std::vector<uint32_t> on_stack; /* unit functions on a thread's stack at the tested call */
};
void take(RunInfo* ri) {
    if (!on) return;
    ri->used = S.touched;
    ri->on_stack = g_on_stack;
    ri->deadlock = S.deadlock;
    ri->digest = S.digest;
    ri->nthreads = (uint32_t)S.thr.size();
    ri->nswitch = S.nswitch;
    ri->npreempt = S.npreempt;
    ri->events = S.events;
    ri->logs.clear();
    ri->entries.clear();
    for (size_t i = 1; i < S.thr.size(); i++) { ri->logs.push_back(S.thr[i]->log); ri->entries.push_back(S.thr[i]->entry); }
}
/* the first difference between the two sides' thread logs ("" when equal) */
std::string compare(const RunInfo& a, const RunInfo& b) {
    char buf[400];
    if (a.logs.size() != b.logs.size()) {
        snprintf(buf, sizeof buf, "guest threads: the original created %zu thread(s), the candidate %zu", a.logs.size(), b.logs.size());
        return buf;
    }
    for (size_t t = 0; t < a.logs.size(); t++) {
        const auto &x = a.logs[t], &y = b.logs[t];
        for (size_t k = 0; k < std::min(x.size(), y.size()); k++) {
            const LogEv &p = x[k], &q = y[k];
            if (p.target != q.target) {
                snprintf(buf, sizeof buf, "guest thread T%zu (entry %08X) call #%zu: original calls %08X, candidate %08X", t + 1,
                         a.entries[t], k, p.target, q.target);
                return buf;
            }
            CallEv ox{}, cy{};
            ox.target = p.target; ox.kind = p.kind;
            cy.target = q.target; cy.kind = q.kind; cy.nint = q.nint; cy.nflt = q.nflt;
            uint16_t m = refc::call_mask(ox, &cy);
            for (int i = 0; i < 8; i++)
                if ((m >> i & 1) && p.a[i] != q.a[i]) {
                    snprintf(buf, sizeof buf, "guest thread T%zu (entry %08X) call #%zu %08X: r%d original %08X candidate %08X", t + 1,
                             a.entries[t], k, p.target, i + 3, p.a[i], q.a[i]);
                    return buf;
                }
            if (p.r3 != q.r3) {
                snprintf(buf, sizeof buf, "guest thread T%zu (entry %08X) call #%zu %08X: result r3 original %08X candidate %08X", t + 1,
                         a.entries[t], k, p.target, p.r3, q.r3);
                return buf;
            }
        }
        if (x.size() != y.size()) {
            snprintf(buf, sizeof buf, "guest thread T%zu (entry %08X): the original makes %zu call(s), the candidate %zu", t + 1,
                     a.entries[t], x.size(), y.size());
            return buf;
        }
    }
    return "";
}

/* ---------------------------------------------------------------- -gthrselftest NAME (guestthreads_test.py)
 * The scheduler and objects driven by host bodies instead of guest code (no guest memory): each scenario prints its
 * event lines, then `digest D switches N preempts P clock C` or `ABORT reason`. */
static std::vector<std::string> st_log;
static void L(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
static void L(const char* fmt, ...) {
    char b[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(b, sizeof b, fmt, ap);
    va_end(ap);
    st_log.push_back(std::string("T") + std::to_string(S.cur) + " " + b);
}
static const uint32_t QA = 0x1000, QB = 0x1010, MA = 0x2000, EA = 0x3000, EB = 0x3010, RA = 0x4000;
static volatile int st_flag;
static Thr* st_spawn(uint32_t guest, uint32_t attr, int prio, void (*body)(Thr*), uint32_t arg = 0) {
    Thr* t = op_create(nullptr, guest, 0, arg, 0, 0x10000000u + 0x100000u * (uint32_t)S.thr.size(), 0x10000, prio, attr);
    t->host_entry = body;
    return t;
}
static void st_resume(Thr* t) { bool k; op_resume(t->guest, &k); }
static Msg mk(uint32_t v) { return Msg{v, 0, 0, 0}; }
/* order */
static void o_A(Thr*) { L("start"); op_send(QA, mk(1), 1); L("sent"); op_sleep(100); L("woke clock %llu", (unsigned long long)S.clock); cur()->cpu.r[3] = 0xA; }
static void o_B(Thr*) { L("start"); op_yield(); L("after yield"); }
static void sc_order() {
    q_init(S.q[QA], 4);
    Thr* a = st_spawn(0x100, 1, 16, o_A);
    Thr* b = st_spawn(0x200, 4, 16, o_B);
    st_resume(a); st_resume(b);
    L("receive");
    Msg m; op_recv(QA, 1, &m);
    L("got %u", m[0]);
    uint32_t v; op_join(a->guest, &v);
    L("joined A value %X", v);
}
/* preempt: a same-core, higher-priority thread runs at once on resume and on the wake by a send */
static void p_C(Thr*) { L("start"); Msg m; op_recv(QA, 1, &m); L("got %u", m[0]); }
static void sc_preempt() {
    q_init(S.q[QA], 4);
    Thr* c = st_spawn(0x100, 2, 10, p_C);
    L("resume C"); st_resume(c);
    L("send"); op_send(QA, mk(7), 1);
    L("after send");
}
/* the same with C on another core: no preemption */
static void sc_nopreempt() {
    q_init(S.q[QA], 4);
    Thr* c = st_spawn(0x100, 1, 10, p_C);
    L("resume C"); st_resume(c);
    L("send"); op_send(QA, mk(7), 1);
    L("after send");
}
static void sc_deadlock() { q_init(S.q[QA], 4); L("receive"); Msg m; op_recv(QA, 1, &m); L("not reached"); }
static void b_D(Thr*) { for (;;) op_yield(); }
static void sc_budget() { budget = 1000; Thr* d = st_spawn(0x100, 1, 16, b_D); st_resume(d); uint32_t v; op_join(d->guest, &v); L("not reached"); }
static void s_E(Thr*) { L("spin"); while (!st_flag) slice_hit(); L("flag seen"); }
static void s_F(Thr*) { L("set flag"); st_flag = 1; }
static void sc_slice() { /* F is resumed only after main's join: E spins with nobody on another core: threadbudget ends it */
    budget = 5000;
    slice_len = 64;
    S.slice_run = sched_seed ? S.slice_run : 64;
    st_flag = 0;
    Thr* e = st_spawn(0x100, 1, 16, s_E);
    Thr* f = st_spawn(0x200, 4, 20, s_F);
    st_resume(e);
    uint32_t v; op_join(e->guest, &v);
    st_resume(f); /* F is only resumed while E spins: E's slice preemption lets it run */
    L("joined E");
}
static void sc_slice2() { /* F resumed before E spins; E waits by spinning, F on another core runs at E's slice */
    st_flag = 0;
    S.slice_run = 64;
    Thr* e = st_spawn(0x100, 1, 16, s_E);
    Thr* f = st_spawn(0x200, 4, 20, s_F);
    f->suspend = 1;
    st_resume(e);
    L("resume F later");
    st_resume(f);
    uint32_t v; op_join(e->guest, &v);
    L("joined E");
}
/* slicecalls: main spins as JASTaskThread-style waits do (027F9994): `unlock; lock; read the flag` with the flag read (its only
 * guest access) inside the lock; the worker on another core needs the mutex to set the flag. The slice must also expire at
 * the OS calls, or every expiry finds the mutex held and the worker blocked */
static void sl_call(uint32_t slot) { Cpu cc{}; uint32_t a[11] = {}; a[3] = MA; cc.r[3] = MA; call(&cc, slot, a); }
static void sl_W(Thr*) { L("lock"); sl_call(I_LOCK); L("set flag"); st_flag = 1; sl_call(I_UNLOCK); }
static void sc_slicecalls() {
    budget = 5000;
    S.slice_run = sched_seed ? S.slice_run : 64;
    st_flag = 0;
    m_init(S.mx[MA]);
    sl_call(I_LOCK);
    Thr* w = st_spawn(0x100, 1, 16, sl_W); /* core 0 */
    g_multi = 1;
    st_resume(w);
    L("spin");
    for (;;) {
        sl_call(I_UNLOCK);
        sl_call(I_LOCK);
        slice_hit(); /* the flag read */
        if (st_flag) break;
    }
    sl_call(I_UNLOCK);
    L("flag seen");
    uint32_t v; op_join(w->guest, &v);
    L("joined");
}
/* vsync: the main thread (core 1, prio 16) waits for vsync each frame; a same-core worker at a lower priority (17) runs while
 * main sleeps in GX2WaitForVsync (as on the console and in the port), then main wakes at the next vsync tick */
static void vs_W(Thr*) { for (int i = 0; i < 3; i++) { L("piece %d clock %llu", i, (unsigned long long)S.clock); op_yield(); } }
static void sc_vsync() {
    Thr* w = st_spawn(0x100, 2, 17, vs_W); /* core 1 */
    st_resume(w);
    for (int f = 0; f < 3; f++) {
        L("frame %d clock %llu", f, (unsigned long long)S.clock);
        Cpu cc{}; uint32_t a[11] = {}; call(&cc, I_VSYNC, a);
    }
    L("after 3 vsyncs clock %llu", (unsigned long long)S.clock);
    uint32_t v; op_join(w->guest, &v);
    L("joined");
}
/* clockpoll (2026-11-02, J3D start-up T22 0203DEEC/0203DDC0): a same-core worker at a lower priority (17) never blocks: it
 * reads the time (OSGetSystemTime), re-posts a message to its own queue and takes it back until 1500000 ticks have passed,
 * while main (prio 16) sleeps in GX2WaitForVsync. The progress clock (threadrate) moves while the worker spins: main wakes at
 * its vsync tick (1036973) and preempts the worker at once; then main joins and the worker's interval ends. With
 * threadrate=0 (`clockpoll0`, the earlier clock) the clock never moves: the worker spins until threadbudget.
 * `clockpollx`: the worker on core 0 (main wakes at the same tick: wake-timer). `clockpolleq`: the worker on main's core at
 * main's priority: main runs only after the worker's interval ends (no time-slicing of equal priorities) */
static uint32_t cp_attr = 2;
static int cp_prio = 17;
static uint64_t cp_now() { Cpu cc{}; uint32_t a[11] = {}; call(&cc, I_SYSTIME, a); return (uint64_t)cc.r[3] << 32 | cc.r[4]; }
static void cp_W(Thr*) {
    uint64_t t0 = cp_now();
    L("poll from clock %llu", (unsigned long long)t0);
    uint64_t n = 0;
    while (cp_now() - t0 < 1500000) {
        op_send(QA, mk(5), 0); /* message 5 to its own queue; after each OS call the progress clock, as in call() */
        os_progress();
        Msg m;
        op_recv(QA, 0, &m);
        os_progress();
        for (int i = 0; i < 6; i++) slice_hit(); /* the loop's own loads and stores */
        n++;
    }
    L("interval passed clock %llu after %llu polls", (unsigned long long)S.clock, (unsigned long long)n);
}
static void sc_clockpoll() {
    budget = 2000;
    S.slice_run = 1ull << 16;
    q_init(S.q[QA], 4);
    Thr* w = st_spawn(0x100, cp_attr, cp_prio, cp_W); /* default: core 1, below main */
    st_resume(w);
    L("vsync wait clock %llu", (unsigned long long)S.clock);
    Cpu cc{}; uint32_t a[11] = {}; call(&cc, I_VSYNC, a);
    L("woke clock %llu", (unsigned long long)S.clock);
    uint32_t v; op_join(w->guest, &v);
    L("joined clock %llu", (unsigned long long)S.clock);
}
static void sc_clockpoll0() { clock_rate = 0; sc_clockpoll(); }
static void sc_clockpollx() { cp_attr = 1; sc_clockpoll(); }
static void sc_clockpolleq() { cp_prio = 16; sc_clockpoll(); }
/* logcap: a thread's call log beyond threadlog entries aborts the run with a receipt naming the thread */
static void lc_W(Thr*) { Cpu cc{}; for (int i = 0; i < 1000; i++) log_begin(&cc, I_GETTIME, 0, 0, 0); L("not reached"); }
static void sc_logcap() {
    log_cap = 100;
    tested_starts(); /* the log records from the tested call on */
    Thr* w = st_spawn(0x100, 1, 16, lc_W);
    w->entry = 0x0203DEECu;
    st_resume(w);
    uint32_t v; op_join(w->guest, &v);
    L("not reached");
}
/* stubs: the stub -> slot mapping of the thread imports (per-library deltas) */
static void sc_stubs() {
    for (uint32_t t : {0x028FDE28u, 0x028FDC00u, 0x028FAE60u, 0x028FAD60u}) L("target %08X -> slot %08X", t, slot_of(t));
}
/* logstart: the thread call log starts at the tested call: calls before it are not logged, calls after it are */
static void ls_T(Thr*) {
    Cpu cc{};
    for (int i = 0; i < 3; i++) log_begin(&cc, I_GETTICK, 0, 0, 0);
    L("before the tested call: %zu logged", cur()->log.size());
    Msg m; op_recv(QA, 1, &m);
    for (int i = 0; i < 2; i++) log_begin(&cc, I_GETTICK, 0, 0, 0);
    L("after it: %zu logged", cur()->log.size());
}
static void sc_logstart() {
    q_init(S.q[QA], 4);
    tested_reset();
    Thr* t = st_spawn(0x100, 1, 16, ls_T);
    st_resume(t);
    op_yield();
    tested_starts();
    op_send(QA, mk(1), 1);
    uint32_t v; op_join(t->guest, &v);
    L("joined");
}
static void c_child(Thr*) { L("core %u", cur()->core); }
static void c_parent(Thr*) {
    L("core %u", cur()->core);
    for (uint32_t m : {0u, 3u, 7u}) { Thr* t = st_spawn(0x800 + m * 16, m, 16, c_child); L("mask %u -> core %u", m, t->core); }
}
static void sc_coreid() {
    for (uint32_t m : {0u, 1u, 2u, 4u, 3u, 5u, 6u, 7u}) { Thr* t = st_spawn(0x100 + m * 16, m, 16, c_child); L("mask %u -> core %u", m, t->core); }
    Thr* p = st_spawn(0x700, 4, 16, c_parent);
    st_resume(p);
    uint32_t v; op_join(p->guest, &v);
    L("main core %u", cur()->core);
}
static void m_G(Thr*) { L("lock"); op_lock(MA); L("locked owner %d count %d", S.mx[MA].owner, S.mx[MA].count); op_unlock(MA); }
static void sc_mutex() {
    m_init(S.mx[MA]);
    op_lock(MA); op_lock(MA);
    L("count %d", S.mx[MA].count);
    Thr* g = st_spawn(0x100, 1, 16, m_G);
    st_resume(g);
    L("trylock by me %d", (int)op_trylock(MA));
    op_unlock(MA);
    op_yield();
    L("after yield owner %d count %d", S.mx[MA].owner, S.mx[MA].count);
    op_unlock(MA); op_unlock(MA);
    L("unlocked");
    uint32_t v; op_join(g->guest, &v);
    L("joined");
}
static void e_W(Thr* t) { L("wait %X", t->argc); bool ok = op_wait_event(t->argc, false, 0); L("woken %d", (int)ok); }
static void e_T(Thr*) { L("timed wait"); bool ok = op_wait_event(EB, true, 50); L("timed result %d clock %llu", (int)ok, (unsigned long long)S.clock); }
static void sc_event() {
    e_init(S.ev[EA], false, true);  /* auto reset */
    e_init(S.ev[EB], false, false); /* manual */
    Thr* w1 = st_spawn(0x100, 1, 16, e_W, EA);
    Thr* w2 = st_spawn(0x200, 4, 16, e_W, EA);
    Thr* tt = st_spawn(0x300, 1, 16, e_T);
    st_resume(w1); st_resume(w2); st_resume(tt);
    op_yield();
    L("signal auto");
    op_signal(EA);
    op_yield();
    L("signaled %d", (int)S.ev[EA].signaled);
    op_signal(EA);
    op_sleep(10);
    L("manual signal");
    op_signal(EB);
    L("signaled %d", (int)S.ev[EB].signaled);
    for (Thr* t : {w1, w2, tt}) { uint32_t v; op_join(t->guest, &v); }
    L("joined all clock %llu", (unsigned long long)S.clock);
    e_reset(S.ev[EB]);
    Thr* t2 = st_spawn(0x400, 1, 16, e_T);
    st_resume(t2);
    uint32_t v; op_join(t2->guest, &v);
    L("timeout path joined clock %llu", (unsigned long long)S.clock);
}
static void r_H(Thr*) { L("arrive"); op_rdv_wait(RA, 3); L("passed"); }
static void sc_rendezvous() {
    S.rdv[RA] = Rendezvous();
    Thr* h = st_spawn(0x100, 1, 16, r_H);
    st_resume(h);
    L("arrive"); op_rdv_wait(RA, 3); L("passed");
    uint32_t v; op_join(h->guest, &v);
}
static void r_K(Thr*) { L("worker arrives"); op_rdv_wait(RA, 1); L("worker passed"); }
static void sc_rdvcore() { /* main (core 1) waits with mask 1 (core 0): it passes only after the core-0 thread arrived */
    S.rdv[RA] = Rendezvous();
    Thr* k = st_spawn(0x100, 1, 16, r_K);
    st_resume(k);
    L("arrive mask 1"); op_rdv_wait(RA, 1); L("passed");
    uint32_t v; op_join(k->guest, &v);
}
static void r_S(Thr*) { L("arrives"); op_rdv_wait(RA, 3); L("passed"); }
static void sc_rdvsame() { /* two core-0 threads are one arrival: mask 3 completes only when core 1 (main, mask 1) arrives */
    S.rdv[RA] = Rendezvous();
    Thr* a = st_spawn(0x100, 1, 16, r_S);
    Thr* b = st_spawn(0x200, 1, 16, r_S);
    st_resume(a); st_resume(b);
    op_yield();
    L("arrive mask 1"); op_rdv_wait(RA, 1); L("passed");
    uint32_t v; op_join(a->guest, &v); op_join(b->guest, &v);
    L("joined");
}
static void n_N(Thr*) { Msg m; L("try receive %d", (int)op_recv(QB, 0, &m)); L("try send %d", (int)op_send(QB, mk(1), 0)); L("try send %d", (int)op_send(QB, mk(2), 0));
                        L("high send blocks"); op_send(QB, mk(3), 3); L("high sent"); }
static void sc_queue() {
    q_init(S.q[QB], 1);
    Thr* n = st_spawn(0x100, 1, 16, n_N);
    st_resume(n);
    Msg m;
    op_yield();
    L("receive %d", (int)op_recv(QB, 1, &m)); L("value %u", m[0]);
    L("receive %d", (int)op_recv(QB, 1, &m)); L("value %u", m[0]);
    uint32_t v; op_join(n->guest, &v);
    L("joined");
}
static int st_fires = 0;
static void st_alarm(uint32_t alarm) {
    L("alarm %X fires clock %llu", alarm, (unsigned long long)S.clock);
    if (alarm == 0x5000 && ++st_fires == 3) op_signal(EA);
}
static void a_W(Thr*) { L("wait"); op_wait_event(EA, false, 0); L("woken clock %llu", (unsigned long long)S.clock); }
static void sc_alarm() { /* a periodic alarm signals an event on its third firing; a one-shot alarm; a cancelled one */
    g_host_alarm_cb = st_alarm;
    e_init(S.ev[EA], false, true);
    Thr* w = st_spawn(0x100, 1, 16, a_W);
    st_resume(w);
    arm_alarm(nullptr, 0x5000, 100, 50, 1);
    arm_alarm(nullptr, 0x5100, 120, 0, 1);
    arm_alarm(nullptr, 0x5200, 130, 0, 1);
    alarm_cancel(S.alarms, 0x5200); /* OSCancelAlarm */
    uint32_t v; op_join(w->guest, &v);
    L("joined clock %llu", (unsigned long long)S.clock);
}
static void x_X(Thr*) { L("exit 0x55"); thread_exit(0x55); }
static void sc_exit() { Thr* x = st_spawn(0x100, 1, 16, x_X); st_resume(x); uint32_t v; op_join(x->guest, &v); L("value %X", v); }
static void sc_cap() { cap = 4; for (int i = 0; i < 5; i++) st_spawn(0x100 + 16 * i, 1, 16, c_child); L("not reached"); }
static void d_K(Thr*) { L("worker"); op_sleep(5); L("worker done"); }
static void sc_drain() { Thr* k = st_spawn(0x100, 1, 16, d_K); st_resume(k); L("drain"); drain(); L("drained clock %llu", (unsigned long long)S.clock); }

int selftest(const char* name) {
    static const std::pair<const char*, void (*)()> sc[] = {{"order", sc_order}, {"preempt", sc_preempt}, {"nopreempt", sc_nopreempt},
        {"deadlock", sc_deadlock}, {"budget", sc_budget}, {"slice", sc_slice2}, {"coreid", sc_coreid}, {"mutex", sc_mutex},
        {"event", sc_event}, {"rendezvous", sc_rendezvous}, {"queue", sc_queue}, {"exit", sc_exit}, {"cap", sc_cap}, {"drain", sc_drain},
        {"slicejoin", sc_slice}, {"rdvcore", sc_rdvcore}, {"rdvsame", sc_rdvsame}, {"alarm", sc_alarm}, {"logstart", sc_logstart}, {"stubs", sc_stubs}, {"slicecalls", sc_slicecalls}, {"vsync", sc_vsync},
        {"clockpoll", sc_clockpoll}, {"clockpoll0", sc_clockpoll0}, {"clockpollx", sc_clockpollx}, {"clockpolleq", sc_clockpolleq}, {"logcap", sc_logcap}};
    void (*fn)() = nullptr;
    std::string nm = name; /* NAME[:SEED]: SEED = threadsched */
    if (size_t k = nm.find(':'); k != std::string::npos) { sched_seed = strtoull(nm.c_str() + k + 1, nullptr, 0); nm.resize(k); }
    for (auto& [n, f] : sc) if (nm == n) fn = f;
    if (!fn) { fprintf(stderr, "-gthrselftest: unknown scenario %s\n", name); return 2; }
    on = true;
    S.nomem = true;
    reset();
    S.nomem = true;
    V.can_abort = true;
    if (setjmp(V.abort_jmp) == 0) {
        fn();
        V.can_abort = false;
        for (auto& l : st_log) printf("%s\n", l.c_str());
        printf("digest %016llX switches %llu preempts %llu clock %llu\n", (unsigned long long)S.digest, (unsigned long long)S.nswitch,
               (unsigned long long)S.npreempt, (unsigned long long)S.clock);
    } else {
        V.can_abort = false;
        for (auto& l : st_log) printf("%s\n", l.c_str());
        printf("ABORT %s\n", V.abort_reason.c_str());
    }
    return 0;
}
}  // namespace gthr

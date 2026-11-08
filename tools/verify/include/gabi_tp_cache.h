/* gabi_tp_cache.h: per-thread block pointer without a thread-local access (the port's WWHD_DECOMP_TP_CACHE, gabi.h
 * GABI_TP_CACHE; level 1 step 1a). On Android the game library is always dlopened, so each access to its
 * thread-local block goes through Bionic's tlsdesc_resolver_dynamic (initial-exec TLS is refused by dlopen). This
 * table maps a thread's key (its thread pointer) to its block: the fast path is a register read, two loads and a
 * compare, no call.
 *
 * Rules:
 *  - No eviction. A thread claims only an empty slot (compare-and-swap on the key, 0 -> BUSY); a thread whose slot
 *    is taken keeps using the normal thread-local path. A live slot is never overwritten.
 *  - Publication order: the claimer writes the block pointer while the key is BUSY, then stores its key with
 *    release order. A reader loads the key with acquire order and, only when it equals its own key, loads the
 *    block. A reader of another thread's slot never sees its own key there (keys are unique among live threads),
 *    so it can never use a pair that is being written; a signal handler that interrupts its own thread's claim
 *    sees BUSY (or 0) and takes the normal path.
 *  - Exit order: a thread_local guard clears the slot (key -> 0, release) when the thread exits. Thread-local
 *    destructors run before the thread's TLS memory is freed (Bionic pthread_exit: __cxa_thread_finalize before
 *    __free_dynamic_tls; Darwin: tlv finalizers before the TLV storage goes), so a later thread that reuses the
 *    thread pointer finds an empty slot. After the guard ran, the thread never claims again (`dead`).
 *  - Live blocks only: the caller claims only for a block in use (gabi: ThreadState::cpu set, i.e. inside a guest
 *    entry). A signal handler that runs during thread exit after the TLS memory was freed gets a fresh block from
 *    the platform (Darwin re-creates TLV storage; Bionic would re-allocate dynamic TLS) whose destructors never run;
 *    that block is zero (cpu null), so it never claims. Found by ds/tpc's churn test on the Mac (signals during
 *    exit, 4 of 15 runs stale before this rule, 0 after).
 *  - A thread cannot end without its destructors while the process lives (no per-thread kill on Linux, Android or
 *    macOS: SIGKILL ends the process; pthread_cancel / pthread_exit run the thread-local destructors). Threads that
 *    never exit keep their slot, which stays valid as long as they live.
 *  - fork: the child clears every slot except its own thread's (pthread_atfork), so a new child thread cannot
 *    match a parent thread's key.
 * The key: TPIDR_EL0 on Linux/Android aarch64 (the thread pointer), pthread_self() elsewhere (Darwin: the
 * thread's pthread_t; the Mac runs only correctness tests). */
#pragma once
#include <atomic>
#include <cstdint>
#include <pthread.h>

struct GabiTpSlot {
    std::atomic<uintptr_t> key;
    void* block;
};
inline constexpr unsigned GABI_TP_SLOTS = 64;
inline constexpr uintptr_t GABI_TP_BUSY = 1; /* a slot being claimed (never a real key: keys are aligned) */

[[gnu::always_inline]] inline uintptr_t gabi_tp_key() {
#if defined(__aarch64__) && !defined(__APPLE__)
    return (uintptr_t)__builtin_thread_pointer();
#else
    return (uintptr_t)pthread_self();
#endif
}
[[gnu::always_inline]] inline unsigned gabi_tp_index(uintptr_t k) { return (unsigned)((k >> 12) ^ (k >> 18)) & (GABI_TP_SLOTS - 1); }

#ifdef GABI_FAST
/* fast mode: up to GABI_TP_WAYS consecutive slots per thread (a direct-mapped slot taken by another thread left that
 * thread on the miss path for its whole life: the phone profile's sead worker spent 44% in the miss path) */
inline constexpr unsigned GABI_TP_WAYS = 4;
[[gnu::noinline]] inline void* gabi_tp_find_more(GabiTpSlot* table, uintptr_t k) {
    const unsigned i = gabi_tp_index(k);
    for (unsigned w = 1; w < GABI_TP_WAYS; w++) {
        GabiTpSlot& s = table[(i + w) & (GABI_TP_SLOTS - 1)];
        if (s.key.load(std::memory_order_acquire) == k) return s.block;
    }
    return nullptr;
}
#endif
/* the block cached for this thread, or null */
[[gnu::always_inline]] inline void* gabi_tp_find(GabiTpSlot* table, uintptr_t k) {
    GabiTpSlot& s = table[gabi_tp_index(k)];
    if (__builtin_expect(s.key.load(std::memory_order_acquire) == k, 1)) return s.block;
#ifdef GABI_FAST
    return gabi_tp_find_more(table, k);
#else
    return nullptr;
#endif
}

/* The miss path's claim, for the unit that owns TABLE (one guard per thread and table). */
struct GabiTpGuard {
    GabiTpSlot* slot = nullptr;
    bool* dead = nullptr;
    ~GabiTpGuard() {
        if (dead) *dead = true;
        if (slot) slot->key.store(0, std::memory_order_release);
    }
};
inline void gabi_tp_claim(GabiTpSlot* table, uintptr_t k, void* block, GabiTpGuard& guard, bool& dead) {
    if (dead || guard.slot) return; /* exiting, or this thread already holds a slot (a collision moved it) */
#ifdef GABI_FAST
    GabiTpSlot* sp = nullptr;
    for (unsigned w = 0; w < GABI_TP_WAYS && !sp; w++) {
        GabiTpSlot& c = table[(gabi_tp_index(k) + w) & (GABI_TP_SLOTS - 1)];
        if (c.key.load(std::memory_order_relaxed) != 0) continue; /* taken: no compare-and-swap on it */
        uintptr_t expect = 0;
        if (c.key.compare_exchange_strong(expect, GABI_TP_BUSY, std::memory_order_acquire, std::memory_order_relaxed)) sp = &c;
    }
    if (!sp) return;
    GabiTpSlot& s = *sp;
#else
    GabiTpSlot& s = table[gabi_tp_index(k)];
    if (s.key.load(std::memory_order_relaxed) != 0) return; /* taken: no compare-and-swap on every miss */
    uintptr_t expect = 0;
    if (!s.key.compare_exchange_strong(expect, GABI_TP_BUSY, std::memory_order_acquire, std::memory_order_relaxed)) return;
#endif
    guard.dead = &dead; /* arms the destructor (registered on the first use of the thread_local guard) */
    guard.slot = &s;
    s.block = block;
    s.key.store(k, std::memory_order_release);
}
/* pthread_atfork child handler: only the forking thread survives */
inline void gabi_tp_fork_child(GabiTpSlot* table) {
    const uintptr_t k = gabi_tp_key();
    for (unsigned i = 0; i < GABI_TP_SLOTS; i++)
        if (table[i].key.load(std::memory_order_relaxed) != k) table[i].key.store(0, std::memory_order_relaxed);
}

/* gabi: what decompiled WWHD source is written against.
 *
 * - Guest structures are described with their WWHD layout. Fields are `be<T>` (big-endian,
 *   accessed through the guest-memory functions below), pointers are `gptr<T>` (32-bit guest
 *   addresses). A `T*` in source is a token for a guest address (PPC_MEM_BASE + EA); it is never
 *   dereferenced directly, only through be<>/gptr<> members, so every access is visible to the
 *   harness.
 * - Calls to other guest functions go through `gabi::call<R>(addr, args...)` (PowerPC EABI:
 *   integers/pointers in r3..r10, floats in f1..f8, result in r3 or f1). Bindings with real
 *   names wrap these (see wwhd_src/include).
 * - Floating point follows the console as the recompiler models it: f32 arithmetic is IEEE
 *   single (exactly what fadds/fmuls/fdivs produce), and where GHS contracted a*b+c into
 *   fmadds the source must say so with gabi::fmadds() & co. (compile with -ffp-contract=off).
 *
 * The memory functions gmem_* are provided by the environment: the verification harness
 * (tools/verify/src/vm.cpp) or a native build.
 */
#pragma once
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <new>
#include <tuple>
#include <type_traits>

#include "ppc.h"
#include "native_frames.h"
#if defined(GABI_GAME_BUILD) && defined(GABI_TP_CACHE)
#include "gabi_tp_cache.h"
#endif

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
/* Opt-in two complete integer-register outputs, e.g. checksum sum/complement. */
struct Pair32 { u32 r3, r4; };
typedef int64_t s64;
typedef float f32;
typedef double f64;
typedef int BOOL;
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* GABI_GAME_BUILD: defined by the port's decomp build (the game, cmake/Decomp.cmake there), never by the
 * verification harness or its tools. It only changes how gabi's own state and the guest memory functions are
 * reached (inline, one thread-local block), not what a candidate does; every build here compiles exactly as
 * without it. */
extern "C" {
#ifdef GABI_GAME_BUILD
/* the game build: guest memory as the recompiled code accesses it (ppc.h), inline */
inline uint8_t gmem_ld8(uint32_t ea) { return ld8(ea); }
inline uint16_t gmem_ld16(uint32_t ea) { return ld16(ea); }
inline uint32_t gmem_ld32(uint32_t ea) { return ld32(ea); }
inline uint64_t gmem_ld64(uint32_t ea) { return ld64(ea); }
inline void gmem_st8(uint32_t ea, uint8_t v) { st8(ea, v); }
inline void gmem_st16(uint32_t ea, uint16_t v) { st16(ea, v); }
inline void gmem_st32(uint32_t ea, uint32_t v) { st32(ea, v); }
inline void gmem_st64(uint32_t ea, uint64_t v) { st64(ea, v); }
inline void gmem_stf64(uint32_t ea, uint64_t v) { st64(ea, v); }
inline void gmem_stf32(uint32_t ea, uint32_t v) { st32(ea, v); }
#else
uint8_t gmem_ld8(uint32_t ea);
uint16_t gmem_ld16(uint32_t ea);
uint32_t gmem_ld32(uint32_t ea);
uint64_t gmem_ld64(uint32_t ea);
void gmem_st8(uint32_t ea, uint8_t v);
void gmem_st16(uint32_t ea, uint16_t v);
void gmem_st32(uint32_t ea, uint32_t v);
void gmem_st64(uint32_t ea, uint64_t v);
void gmem_stf64(uint32_t ea, uint64_t v); /* explicit double store */
void gmem_stf32(uint32_t ea, uint32_t v); /* float store */
#endif
void gmem_stack_fresh(uint32_t ea, uint32_t n, uint32_t object_size); /* a new stack temporary (n = aligned slot, object_size = sizeof(T)): initial scratch-stack contents */
/* stack guard (2026-10-05): the outgoing argument frame do_call builds (on = 1 while it stores
 * the back chain and stack arguments), and the caller's SP before that frame while a guest call
 * runs (top = 0 after it): real callees may write below it (their frames, the linkage words),
 * not above it except inside live gabi::Locals */
void gmem_stack_args(int on);
/* a native-layout frame (gabi::NativeFrame): reserved stack the candidate does NOT initialise, so
 * the stale bytes the original's uninitialised frame slots carry stay as they are (game test
 * 2026-10-05: initdata_to_card copies uninitialised padding of its stack objects into the save).
 * Weak no-op default (game build); the harness registers the area for its stack guard. */
#ifdef GABI_HARNESS
void gmem_stack_reserve(uint32_t ea, uint32_t n);
#else
__attribute__((weak)) void gmem_stack_reserve(uint32_t ea, uint32_t n) { (void)ea; (void)n; }
#endif
/* the original's frame of a function (native_frames.h): weak null default = no automatic frames;
 * the harness and native/game-test builds link tools/verify/src/native_frames.cpp */
#ifndef GABI_HARNESS
__attribute__((weak)) const GabiFrameInfo* gmem_frame_info(uint32_t addr) { (void)addr; return nullptr; }
/* a gabi::Local that does not fit into the original's frame (automatic native frame fallback):
 * reported by the harness (stderr + build/verify/frame_fallbacks.tsv), ignored elsewhere */
__attribute__((weak)) void gmem_frame_fallback(uint32_t func, uint32_t size, uint32_t used, uint32_t space, uint32_t frame) {
    (void)func; (void)size; (void)used; (void)space; (void)frame;
}
#else
void gmem_frame_fallback(uint32_t func, uint32_t size, uint32_t used, uint32_t space, uint32_t frame);
#endif
/* a trap instruction (tw/twi) the original takes, e.g. the debugger break in nw::ut halt (0286DB70): the harness
 * counts it like the original's trap and compares the counts; the weak game-build default hands it to the
 * runtime's ppc_trap (the port owns what a native debugger break does) */
#ifdef GABI_HARNESS
void gmem_trap(Cpu* c, uint32_t addr);
#else
__attribute__((weak)) void gmem_trap(Cpu* c, uint32_t addr) { ppc_trap(c, addr); }
#endif
/* lwarx / stwcx. (a reservation pair) and sync for spin locks and atomic updates in guest memory: the game build
 * uses the runtime's native atomics (ppc.h ppc_lwarx/ppc_stwcx, a full fence); the harness routes them through
 * its observed memory (same reservation rule as the reference's vm_lwarx/vm_stwcx) */
#ifdef GABI_HARNESS
uint32_t gmem_lwarx(Cpu* c, uint32_t ea);
int gmem_stwcx(Cpu* c, uint32_t ea, uint32_t v);
void gmem_sync(void);
#else
__attribute__((weak)) uint32_t gmem_lwarx(Cpu* c, uint32_t ea) { return ppc_lwarx(c, ea); }
__attribute__((weak)) int gmem_stwcx(Cpu* c, uint32_t ea, uint32_t v) { ppc_stwcx(c, ea, v); return c->cr[2]; }
__attribute__((weak)) void gmem_sync(void) { __atomic_thread_fence(__ATOMIC_SEQ_CST); }
#endif
void gmem_call_top(uint32_t top);
/* GABI_GUEST_GUARD builds (the harness): a be<>/gptr<> whose `this` is outside guest_ref's ranges (gabi.h,
 * be<>::addr): a host object (host stack, heap block or image data) prints the diagnostic and ends the run (exit
 * status 86; its return address is the offending access); NULL-based guest address arithmetic returns. */
[[gnu::cold, gnu::noinline]] void gmem_host_object(const void* self, unsigned size);
/* GABI_GUEST_GUARD=2 builds (verify.py -guardstrict, a diagnostic): a be<>/gptr<> whose `this` is neither in the guest
 * window [PPC_MEM_BASE, +4 GB) nor a NULL-based address below 4 GB, i.e. host address arithmetic that only reaches its
 * guest address through ea()'s 32-bit wrap (fast: inside guest_ref's ranges) */
[[gnu::cold, gnu::noinline]] void gmem_host_wrap(const void* self, unsigned size, int fast);
/* kind: 0 direct, 1 through a function pointer; nint/nflt: argument registers set */
void gmem_call(Cpu* c, uint32_t target, int kind, int nint, int nflt);
}

namespace gabi {

#ifdef GABI_GAME_BUILD
/* The game build: gabi's per-thread state (the running candidate's Cpu, Activation depth and function, the
 * automatic frame) is one constant-initialised thread-local block, so a function reaches it with one
 * thread-local address per body (Mach-O: one _tlv_get_addr call, no wrapper function). Each game unit has
 * its own block (hidden, made local by the port's `ld -r`), as it had its own variables. The names below
 * (gabi::cpu, frame_ctx, Activation::depth/current) stay usable as before: they are views of the block. */
struct FrameCtx;
struct ThreadState {
    Cpu* cpu;
    int depth;
    u32 current;
    FrameCtx* frame_ctx;
    u32 caller_ok; /* native calls (WWHD_GLUE_FUNC below): the running function's own calls may go native */
    /* site-exact LR (the port's tools/decomp_build/site_lr.py): a call site of a decompiled body marks the return
     * address the original's bl leaves for its target (NativeScope::site, written through the body's own pointer to
     * this block, no thread-local access); the target's guest or native entry takes it (site_take). Key: the caller's
     * slot + 1 << 32 | target, 0: empty. Indexed by the target's word address, so marks for different targets
     * evaluated in one expression stay apart. */
#ifdef GABI_SITE_LR
    struct SiteMark { uint64_t key; u32 ret; };
    SiteMark site[8];
#endif
#ifdef GABI_FAST
    u32 probe; /* fast mode: the address gabi::call is entering natively (a body whose own glue makes the guest call
                * to its address again, e.g. d_a_andsw2's WWHD_GLUE_FUNC, gets the guest entry the second time) */
#endif
#ifdef GABI_NOCHECK
    u32 nocheck_off; /* GABI_NOCHECK: g_decomp_nocheck_off, copied into each thread's block by the port's gabi_tp_miss (the
                      * first lookup of every thread misses), so a body tests it in the block it already holds */
#endif
};
#ifdef GABI_SHARED_TLS
/* GABI_SHARED_TLS (the port's decomp build with real native calls between units): one block for every game unit,
 * defined once by the port (GABI_TLS_OWNER, default visibility, so `ld -r` keeps it global). A native call into
 * another unit then finds the caller's Cpu, depth and site marks already in place: no thread-local access of its
 * own. A guest entry (EntryScope) saves and clears what a unit's own block held for it (depth, frame_ctx, ...). */
extern thread_local constinit ThreadState tls __attribute__((visibility("default")));
#else
#ifdef GABI_SITE_LR
inline thread_local constinit ThreadState tls{nullptr, 0, 0, nullptr, 0, {}};
#else
inline thread_local constinit ThreadState tls{nullptr, 0, 0, nullptr, 0};
#endif
#endif
#ifdef GABI_TP_CACHE
/* GABI_TP_CACHE (the port's WWHD_DECOMP_TP_CACHE, level 1 step 1a): the block through gabi_tp_cache.h's
 * thread-pointer table instead of a thread-local access (Android: dlopened library = tlsdesc_resolver_dynamic per
 * access). A miss (first use on a thread, a slot taken by another thread, an exiting thread) takes the port's
 * gabi_tp_miss: the thread-local block itself, and the slot claimed when it is free. Same block, same values. */
#if !defined(GABI_SHARED_TLS) || !defined(GABI_SLIM_SCOPE)
#error "GABI_TP_CACHE needs GABI_SHARED_TLS (one block for all units) and GABI_SLIM_SCOPE"
#endif
extern "C" __attribute__((visibility("protected"))) GabiTpSlot g_gabi_tp_slots[GABI_TP_SLOTS];
extern "C" __attribute__((visibility("default"))) ThreadState* gabi_tp_miss();
[[gnu::always_inline]] inline ThreadState& tls_fast() {
    if (void* b = gabi_tp_find(g_gabi_tp_slots, gabi_tp_key())) {
#ifdef GABI_TP_CACHE_CHECK
        if (b != &tls) __builtin_trap(); /* debug: every hit must be this thread's own block */
#endif
        return *(ThreadState*)b;
    }
    return *gabi_tp_miss();
}
#ifdef GABI_PASS_TS
/* GABI_PASS_TS (the port's WWHD_DECOMP_FAST_PASS_TS): a body passes the block it looked up on to everything that runs
 * in it, instead of looking it up again: every use of the block (the body's own scope, Activation, AutoFrame, Locals,
 * its gabi::call / lcall / call_ptr, and the bodies and ext stubs the compiler inlines into it) takes it from
 * gabi_ts_pass, which the compiler may evaluate once per function and reuse (const: the same answer every time on a
 * thread). gabi_ts_pass is tls_fast above, unchanged (the same table, the same acquire load, the same miss path), out of
 * line in unit 0 (the port's glue.cpp); it always returns the calling thread's own block (&tls), and that block is
 * the thread's for its life, so one answer stands for every later lookup on that thread. A function that looks the
 * block up once still does exactly that one lookup. */
#if !defined(GABI_FAST) || !defined(GABI_GAME_BUILD)
#error "GABI_PASS_TS (the thread block passed on in a body) is a fast-mode game-build form: it needs GABI_FAST"
#endif
extern "C" [[gnu::const]] __attribute__((visibility("default"))) ThreadState* gabi_ts_pass();
#define GABI_TLS_ (*::gabi::gabi_ts_pass())
#define GABI_BODY_TS_ (*::gabi::gabi_ts_pass())
#else
#define GABI_TLS_ (::gabi::tls_fast())
#define GABI_BODY_TS_ ::gabi::tls_fast()
#endif
#else
#ifdef GABI_PASS_TS
#error "GABI_PASS_TS (the thread block passed on in a body) is a fast-mode form: it needs GABI_FAST and GABI_TP_CACHE"
#endif
#define GABI_TLS_ tls
#endif
/* GABI_FAST (the port's WWHD_DECOMP_FAST, the decomp build's fast mode): speed over verifier isolation, save-state
 * compatibility and guest-mod hooking. Decompiled-to-decompiled calls are native wherever a
 * body can run as a C++ call (function pointers and vtables included), AutoFrame keeps the frame's place without the
 * prologue stores, and the per-call debug bookkeeping (park guard depth, poison, per-slot mod switch) is left out.
 * Correctness bar: the game plays as the normal build does (A/B traces), not register- or frame-exact. */
#ifdef GABI_FAST
#if !defined(GABI_SHARED_TLS) || !defined(GABI_SLIM_SCOPE) || !defined(GABI_LEAN_ENTRY) || !defined(GABI_NATIVE_ENTRIES) || defined(GABI_SITE_LR)
#error "GABI_FAST needs GABI_SHARED_TLS, GABI_SLIM_SCOPE, GABI_LEAN_ENTRY and GABI_NATIVE_ENTRIES, and no GABI_SITE_LR"
#endif
#endif
#if defined(GABI_COLD_FALLBACK) && !defined(GABI_FAST)
#error "GABI_COLD_FALLBACK (guest call paths out of line) is a fast-mode form: it needs GABI_FAST"
#endif
#if defined(GABI_PTR_IC2) && !defined(GABI_PTR_IC)
#error "GABI_PTR_IC2 (two cached targets per site) needs GABI_PTR_IC"
#endif
#if defined(GABI_PTR_IC) && !defined(GABI_FAST)
#error "GABI_PTR_IC (per-site cache of call_ptr / tail_ptr targets) is a fast-mode form: it needs GABI_FAST"
#endif
#if defined(GABI_NX_LEAN) && (!defined(GABI_FAST) || !defined(GABI_GAME_BUILD))
#error "GABI_NX_LEAN (non-register-exact native entries without LR / pc) is a fast-mode game-build form: it needs GABI_FAST"
#endif
#if defined(GABI_AF_INLINE) && (!defined(GABI_FAST) || !defined(GABI_GAME_BUILD))
#error "GABI_AF_INLINE (AutoFrame's place-only frame inline) is a fast-mode game-build form: it needs GABI_FAST"
#endif
#if defined(GABI_THUNK_INL) && (!defined(GABI_FAST) || !defined(GABI_GAME_BUILD) || !defined(GABI_LEAF))
#error "GABI_THUNK_INL (leaf bodies inlined into their native-entry thunks) is a fast-mode game-build form: it needs GABI_FAST and GABI_LEAF"
#endif
#if defined(GABI_LEAF_BARE) && (!defined(GABI_FAST) || !defined(GABI_GAME_BUILD) || !defined(GABI_LEAF) || !defined(GABI_PASS_TS) || !defined(GABI_NOCHECK) || defined(GABI_LEAF_CHECK) || defined(GABI_CHECKCOUNT))
#error "GABI_LEAF_BARE (bare leaf bodies) is a fast-mode game-build form: it needs GABI_FAST, GABI_LEAF, GABI_PASS_TS (a const block lookup the compiler drops when unused) and GABI_NOCHECK, and no GABI_LEAF_CHECK / GABI_CHECKCOUNT"
#endif
#if defined(GABI_BARE_ENTRY) && !defined(GABI_LEAF_BARE)
#error "GABI_BARE_ENTRY (bare leaves with pointer parameters entered only through their native entry) needs GABI_LEAF_BARE"
#endif
#if defined(GABI_FAST_ACCESSOR) && (!defined(GABI_FAST) || !defined(GABI_GAME_BUILD) || !defined(GABI_NATIVE_ENTRIES) || !defined(GABI_FAST_NE))
#error "GABI_FAST_ACCESSOR (lazy accessors' native entries) is a fast-mode game-build form: it needs GABI_FAST, GABI_NATIVE_ENTRIES and GABI_FAST_NE"
#endif
#if defined(GABI_STACKARG) && (!defined(GABI_FAST) || !defined(GABI_GAME_BUILD) || !defined(GABI_LEAN_ENTRY) || !defined(GABI_TP_CACHE) || !defined(GABI_SHARED_TLS))
#error "GABI_STACKARG (stack-argument calls made native) is a fast-mode game-build form: it needs GABI_FAST, GABI_LEAN_ENTRY (a register-exact entry keeps the r1 it is given), GABI_TP_CACHE and GABI_SHARED_TLS"
#endif
#define GABI_TLS_BLOCK 1 /* the port's glue does not define gabi::cpu */
template <class T, T ThreadState::*M> struct TlsView {
    operator T() const { return GABI_TLS_.*M; }
    const TlsView& operator=(T v) const { GABI_TLS_.*M = v; return *this; }
    T operator->() const { return GABI_TLS_.*M; }
    T operator++(int) const { return (GABI_TLS_.*M)++; }
    T operator--(int) const { return (GABI_TLS_.*M)--; }
    const TlsView& operator++() const { ++(GABI_TLS_.*M); return *this; }
    const TlsView& operator--() const { --(GABI_TLS_.*M); return *this; }
};
inline constexpr TlsView<Cpu*, &ThreadState::cpu> cpu{};
#else
extern thread_local Cpu* cpu;
#endif

#ifdef GABI_GAME_BUILD
/* native calls (WWHD_GLUE_FUNC): the unit's table, written by the port (pre-included before this header as
 * GABI_NATIVE_TABLE: gabi_native::kTab, sorted by address); without it nothing goes native */
struct NativeInfo {
    u32 addr;
    u32 slot;  /* the runtime's slot index (decomp/functions.tsv order) */
    u32 flags; /* 1: may be called natively, 2: its own calls may go native, 4: a decompiled function (listed),
                * 8: register-exact (the port's decl table: exact or shim, every decompiled call passes exactly what the
                * definition reads), bits 8-11 / 12-15: the integer / float argument registers the definition takes
                * (`this` included), bits 16-17: its result (0 none, 1 r3, 2 r3+r4, 3 f1), 18: its native entry places
                * r1 (GABI_NATIVE_PLACE), 19: caller=ok only by the per-function A3 rule (GABI_A3_FN), 20: a leaf
                * (GABI_LEAF), 21: its source may pass unbased pointers (GABI_NOCHECK), 22: a leaf whose unit has no
                * caller=guest and no unbased function (GABI_LEAF_BARE), 23: such a leaf that no source of its unit calls
                * by name, so every call enters it through its native entry or from guest code (GABI_BARE_ENTRY), 24: a
                * reader of its stack arguments made native for do_call_sa only (GABI_STACKARG) */
};
/* the native entry signature of a register-exact function (NativeEntry::sig): its argument registers and result as
 * the port's table states them, bit 15 set (0: no native entry) */
constexpr u32 native_sig(u32 flags) { return (flags >> 8 & 0x3FFu) | 0x8000u; }
constexpr NativeInfo native_info(u32 a) {
#ifdef GABI_NATIVE_TABLE
    u32 lo = 0, hi = sizeof(gabi_native::kTab) / sizeof(gabi_native::kTab[0]);
    while (lo < hi) {
        u32 m = (lo + hi) / 2;
        if (gabi_native::kTab[m][0] < a) lo = m + 1;
        else hi = m;
    }
    if (lo < sizeof(gabi_native::kTab) / sizeof(gabi_native::kTab[0]) && gabi_native::kTab[lo][0] == a)
        return NativeInfo{a, gabi_native::kTab[lo][1], gabi_native::kTab[lo][2]};
#endif
    return NativeInfo{a, 0, 0};
}
}  /* namespace gabi */
/* the runtime (the port's runtime/src/decomp): per slot whether it may still be entered natively (null: never). The
 * running decompiled function (slot + 1, 0: none) for the LR of its guest calls is the port's Cpu::decomp_running. */
extern "C" __attribute__((visibility("default"))) uint8_t* g_decomp_native_ok; /* 1: form N, 2: form L */
/* the runtime: pc and the LR a guest call from the running decompiled function to TARGET gets (gmem_call) */
extern "C" __attribute__((visibility("default"))) u32 decomp_native_lr(Cpu* c, u32 target, uint64_t* lr_cache);
/* real native calls (register-exact functions, flags 8): per slot the function's native entry (gabi.h NEntry: C++
 * arguments in host registers, one u32 per integer register and one double per float register, converted as ArgPack /
 * ArgFrom convert them; its result as store_ret / result convert it) and its signature (native_sig; 0: not entered this
 * way, e.g. hooked by a mod since). Null: none. Registered by each unit (VERIFY), enabled by the runtime. */
struct GabiNativeEntry {
    void* fn;
    u32 sig;
    u32 reserved;
};
extern "C" __attribute__((visibility("default"))) GabiNativeEntry* g_decomp_native_entry;
#ifdef GABI_FAST
/* fast mode: the enabled native entries by guest address, for calls through function pointers and vtables (call_ptr):
 * open addressing, linear probe, addr 0 = empty, g_decomp_ptr_mask + 1 entries (a power of two). Built once by the
 * runtime (direct_init) before any guest thread runs; read-only afterwards. Null: none. */
struct GabiPtrEntry {
    u32 addr;
    u32 sig; /* NEntryT::sig */
    void* fn;
};
extern "C" __attribute__((visibility("default"))) const GabiPtrEntry* g_decomp_ptr_tab;
extern "C" __attribute__((visibility("default"))) u32 g_decomp_ptr_mask;
/* the same table for gabi::call by address (the runtime may leave entries out: WWHD_DECOMP_FAST_CALL bisecting) */
extern "C" __attribute__((visibility("default"))) const GabiPtrEntry* g_decomp_call_tab;
[[gnu::always_inline]] inline u32 gabi_ptr_hash(u32 a) { return (a >> 2) * 0x9E3779B1u >> 7; }
template <bool CALL = false> [[gnu::always_inline]] inline const GabiPtrEntry* gabi_ptr_find(u32 a) {
    const GabiPtrEntry* t = CALL ? g_decomp_call_tab : g_decomp_ptr_tab;
    if (__builtin_expect(!t, 0)) return nullptr;
    const u32 m = g_decomp_ptr_mask;
    for (u32 h = gabi_ptr_hash(a) & m;; h = (h + 1) & m) {
        const u32 x = t[h].addr;
        if (x == a) return t + h;
        if (!x) return nullptr;
    }
}
#endif
/* bit 0: register-exact native entries pass no guest registers (the port's WWHD_DECOMP_NATIVE_EXACT, default on);
 * bits 1 / 2: debug, poison the argument registers on such an entry / the result registers on its exit
 * (WWHD_DECOMP_POISON=1 both, =args only the arguments: a body that still reads a register the call no longer sets) */
extern "C" __attribute__((visibility("default"))) uint8_t g_decomp_exact;
#ifdef GABI_LEAF
#if !defined(GABI_FAST) || !defined(GABI_SLIM_SCOPE) || !defined(GABI_TP_CACHE)
#error "GABI_LEAF (leaf bodies keep no call state) is a fast-mode form: it needs GABI_FAST, GABI_SLIM_SCOPE and GABI_TP_CACHE"
#endif
/* the port's WWHD_DECOMP_FAST_LEAF: 1 = leaf bodies take the full native-call scope (WWHD_DECOMP_FAST_LEAF=0); set by the
 * runtime before any guest thread runs */
extern "C" __attribute__((visibility("default"))) uint8_t g_decomp_leaf_off;
#endif
#ifdef GABI_NSTATS
/* the port's WWHD_DECOMP_FAST_NSTATS (diagnostics builds only, never shipped): per slot the native entries into the body
 * (NativeScope's native path); null unless WWHD_DECOMP_FAST_NSTATS=PATH */
extern "C" __attribute__((visibility("default"))) uint64_t* g_decomp_nstats;
#ifdef GABI_FAST_ACCESSOR
/* with GABI_FAST_ACCESSOR: a lazy accessor's native entries by the running function at the call (Cpu::decomp_running,
 * the caller's slot + 1; 0: none) */
extern "C" __attribute__((visibility("default"))) uint64_t* g_decomp_acc_callers;
#endif
#endif
#ifdef GABI_A3_FN
/* the port's WWHD_DECOMP_FAST_A3_FN: 1 = bodies with NativeInfo bit 19 keep their calls guest calls (WWHD_DECOMP_FAST_A3_FN=0,
 * bisecting); set by the runtime before any guest thread runs */
extern "C" __attribute__((visibility("default"))) uint8_t g_decomp_a3_off;
#endif
#ifdef GABI_CHECKCOUNT
#if !defined(GABI_FAST) || !defined(GABI_TP_CACHE)
#error "GABI_CHECKCOUNT (counted native_enter_ok checks) is a fast-mode diagnostics form: it needs GABI_FAST and GABI_TP_CACHE"
#endif
/* the port's WWHD_DECOMP_FAST_CHECKCOUNT (diagnostics builds only, never shipped): per slot, how often a body's entry ran
 * each of native_enter_ok's checks and how often each refused (the body then made its guest call): GABI_CC_KINDS run
 * counters, then GABI_CC_KINDS refusal counters; null unless WWHD_DECOMP_FAST_CHECKCOUNT=PATH. Every refusal also goes to
 * decomp_checkcount_fail (callee slot, check, the running caller's address, the argument's index and value) */
enum : u32 {
    GABI_CC_GUEST,  /* entries with depth 0 (from guest code: no check runs) */
    GABI_CC_NESTED, /* entries with depth > 0 (a native call from another body: the checks below run) */
    GABI_CC_STATIC, /* the body's result or argument types / unrewritten returns refuse at compile time */
    GABI_CC_RULE,   /* NativeInfo bit 0 (native_rules.py: may be called natively), a constant */
    GABI_CC_CALLER, /* ThreadState::caller_ok (the running caller's calls may go native) */
    GABI_CC_NBYTE,  /* g_decomp_native_ok[slot] bit 0 (the runtime: still decompiled, unhooked, in the direct table) */
    GABI_CC_ARG,    /* one per pointer argument: null or in the guest window (native_arg_ok) */
    GABI_CC_KINDS
};
extern "C" __attribute__((visibility("default"))) uint64_t* g_decomp_checkcount;
extern "C" __attribute__((visibility("default"))) void decomp_checkcount_fail(u32 slot, u32 kind, u32 caller, u32 arg, uint64_t value);
#endif
#ifdef GABI_NOCHECK
/* the port's WWHD_DECOMP_FAST_NOCHECK: 1 = nested calls run every native_enter_ok check again (WWHD_DECOMP_FAST_NOCHECK=0,
 * debugging); set by the runtime before any guest thread runs */
extern "C" __attribute__((visibility("default"))) uint8_t g_decomp_nocheck_off;
#endif
/* registers a unit's native entry (the port's glue.cpp; MISMATCH: its C++ signature differs from the table's) */
extern "C" void decomp_register_native(const char* unit, u32 addr, void* fn, u32 sig, u32 table_sig);
namespace gabi {
template <class T> constexpr bool native_type_ok() {
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr (std::is_integral_v<U> || std::is_enum_v<U>) return sizeof(U) <= 4;
    else return std::is_floating_point_v<U> || std::is_pointer_v<U> || std::is_same_v<U, std::nullptr_t> || std::is_same_v<U, Pair32>;
}
template <class A> inline bool native_arg_ok(const A& a) {
    using U = std::remove_cv_t<std::remove_reference_t<A>>;
    if constexpr (std::is_pointer_v<U>) {
        /* as the guest ABI passes it: at(ea(p)) == p (null or the guest window), and never a host string literal or
         * a function passed by name (ArgPack turns those into their guest copies) */
        const uintptr_t v = (uintptr_t)a;
        return !v || v - (uintptr_t)PPC_MEM_BASE < 0x100000000ull;
    } else return true;
}
/* RET_OK: the body's return statements hand their value to NativeScope::ret (the port's gen.py rewrites them and
 * writes WWHD_GLUE_FUNC_R; a void function needs none) */
#ifdef GABI_TP_CACHE
/* the same check with the body's block (GABI_TP_CACHE: one block lookup per body) */
template <class R, bool RET_OK, class... A> [[gnu::always_inline]] inline bool native_enter_ok_t(const ThreadState& t, const NativeInfo& ni, const A&... a) {
    if constexpr (!RET_OK || std::is_reference_v<R> || !(native_type_ok<R>() || std::is_void_v<R>) || !(native_type_ok<A>() && ...)) return false;
    else {
        if (!(ni.flags & 1)) return false;
        if (!t.caller_ok) return false;
        const uint8_t* ok = g_decomp_native_ok;
        if (!ok || !(ok[ni.slot] & 1)) return false;
        return (native_arg_ok(a) && ...);
    }
}
#endif
#ifdef GABI_CHECKCOUNT
/* GABI_CHECKCOUNT: the body's entry decision (depth > 0 && !native_enter_ok_t) with the same checks in the same order and
 * the same result, each counted (true: the body runs natively or was entered from guest code) */
[[gnu::cold, gnu::noinline]] inline void cc_fail(u32 slot, u32 kind, const ThreadState& t, u32 arg, uint64_t value) {
    __atomic_fetch_add(&g_decomp_checkcount[(size_t)slot * (2 * GABI_CC_KINDS) + GABI_CC_KINDS + kind], 1, __ATOMIC_RELAXED);
    decomp_checkcount_fail(slot, kind, t.current, arg, value);
}
[[gnu::always_inline]] inline void cc_run(uint64_t* cc, u32 slot, u32 kind) {
    if (cc) __atomic_fetch_add(&cc[(size_t)slot * (2 * GABI_CC_KINDS) + kind], 1, __ATOMIC_RELAXED);
}
template <class A> [[gnu::always_inline]] inline bool native_arg_cc(uint64_t* cc, const ThreadState& t, u32 slot, u32 i, const A& a) {
    using U = std::remove_cv_t<std::remove_reference_t<A>>;
    if constexpr (std::is_pointer_v<U>) {
        cc_run(cc, slot, GABI_CC_ARG);
        if (native_arg_ok(a)) return true;
        if (cc) cc_fail(slot, GABI_CC_ARG, t, i, (uint64_t)(uintptr_t)a);
        return false;
    } else return true;
}
template <class R, bool RET_OK, class... A> [[gnu::always_inline]] inline bool native_enter_cc(const ThreadState& t, const NativeInfo& ni, const A&... a) {
    uint64_t* cc = g_decomp_checkcount;
    const u32 s = ni.slot;
    if (!(t.depth > 0)) { cc_run(cc, s, GABI_CC_GUEST); return true; }
    cc_run(cc, s, GABI_CC_NESTED);
    if constexpr (!RET_OK || std::is_reference_v<R> || !(native_type_ok<R>() || std::is_void_v<R>) || !(native_type_ok<A>() && ...)) {
        cc_run(cc, s, GABI_CC_STATIC);
        if (cc) cc_fail(s, GABI_CC_STATIC, t, 0, 0);
        return false;
    } else {
        cc_run(cc, s, GABI_CC_RULE);
        if (!(ni.flags & 1)) { if (cc) cc_fail(s, GABI_CC_RULE, t, 0, ni.flags); return false; }
        cc_run(cc, s, GABI_CC_CALLER);
        if (!t.caller_ok) { if (cc) cc_fail(s, GABI_CC_CALLER, t, 0, 0); return false; }
        cc_run(cc, s, GABI_CC_NBYTE);
        const uint8_t* ok = g_decomp_native_ok;
        if (!ok || !(ok[s] & 1)) { if (cc) cc_fail(s, GABI_CC_NBYTE, t, 0, ok ? ok[s] : 0x100u); return false; }
        u32 i = 0;
        bool r = true;
        ((r = r && native_arg_cc(cc, t, s, i++, a)), ...);
        return r;
    }
}
#endif
#ifdef GABI_NOCHECK
#if !defined(GABI_FAST) || !defined(GABI_TP_CACHE) || defined(GABI_CHECKCOUNT)
#error "GABI_NOCHECK (fewer native-entry checks) is a fast-mode form: it needs GABI_FAST and GABI_TP_CACHE, and not GABI_CHECKCOUNT"
#endif
/* GABI_NOCHECK (the port's WWHD_DECOMP_FAST_NOCHECK): a nested call's checks without the two that never refused in the
 * 2026-11-09 check counts (every route of the fast build, hours of play): the callee's native byte
 * (g_decomp_native_ok: the runtime's per-function switch, cleared for guest-mod hooks and the bisecting variables
 * WWHD_DECOMP_NATIVE_FUNCS / _EXCLUDE) and the per-argument guest-window check, except
 *  - for calls from a caller that marked them (caller_ok bit 1: NativeInfo bit 21, a source with unbased casts, the
 *    only callers whose arguments the counts saw refused),
 *  - for char-pointer and function-pointer arguments (a host string literal or a function passed by name: ArgPack
 *    turns them into their guest copies on the guest path), which keep it always.
 * caller_ok (refused by design for callers the rules keep guest, A1-A3) and the compile-time refusals stay.
 * ThreadState::nocheck_off (g_decomp_nocheck_off, WWHD_DECOMP_FAST_NOCHECK=0, debugging): every check as native_enter_ok_t */
template <class R, bool RET_OK, class... A> [[gnu::cold, gnu::noinline]] bool native_enter_full(const ThreadState& t, const NativeInfo& ni, A... a) {
    return native_enter_ok_t<R, RET_OK>(t, ni, a...);
}
template <class A> constexpr bool nc_always_checked() {
    using U = std::remove_cv_t<std::remove_reference_t<A>>;
    if constexpr (std::is_pointer_v<U>) {
        using P = std::remove_cv_t<std::remove_pointer_t<U>>;
        return std::is_same_v<P, char> || std::is_same_v<P, signed char> || std::is_same_v<P, unsigned char> ||
               std::is_function_v<P> || std::is_void_v<P>;
    } else return false;
}
template <class A> [[gnu::always_inline]] inline bool native_arg_nc(u32 co, const A& a) {
    using U = std::remove_cv_t<std::remove_reference_t<A>>;
    if constexpr (!std::is_pointer_v<U>) return true;
    else if constexpr (nc_always_checked<A>()) return native_arg_ok(a);
    else return !(co & 2) || native_arg_ok(a);
}
template <class R, bool RET_OK, class... A> [[gnu::always_inline]] inline bool native_enter_nc(const ThreadState& t, const NativeInfo& ni, const A&... a) {
    if constexpr (!RET_OK || std::is_reference_v<R> || !(native_type_ok<R>() || std::is_void_v<R>) || !(native_type_ok<A>() && ...)) return false;
    else {
        if (!(ni.flags & 1)) return false;
        if (__builtin_expect(t.nocheck_off, 0)) return native_enter_full<R, RET_OK>(t, ni, a...);
        const u32 co = t.caller_ok;
        if (!co) return false;
        return (native_arg_nc(co, a) && ...);
    }
}
#endif
template <class R, bool RET_OK, class... A> inline bool native_enter_ok(const NativeInfo& ni, const A&... a) {
    if constexpr (!RET_OK || std::is_reference_v<R> || !(native_type_ok<R>() || std::is_void_v<R>) || !(native_type_ok<A>() && ...)) return false;
    else {
        if (!(ni.flags & 1)) return false;
        if (!GABI_TLS_.caller_ok) return false;
        const uint8_t* ok = g_decomp_native_ok;
        if (!ok || !(ok[ni.slot] & 1)) return false;
        return (native_arg_ok(a) && ...);
    }
}
#endif

/* the original's trap instruction at guest address ADDR (tw/twi taken): compared as a trap event */
inline void trap(u32 addr) { gmem_trap(cpu, addr); }
/* lwarx: load the word at A and reserve it; stwcx.: store V at A only if the reservation still holds, returns
 * whether it stored (cr0.eq, also set in cpu->cr like the instruction); sync: a full memory barrier. Write the
 * original's loops with them, e.g. `while (gabi::load_reserved(a) != 0) {} ... while (!gabi::store_conditional(a, 1))`.
 * dcbst/dcbf/icbi have no effect the comparison sees and need no call. */
inline u32 load_reserved(u32 a) { return gmem_lwarx(cpu, a); }
inline bool store_conditional(u32 a, u32 v) { return gmem_stwcx(cpu, a, v) != 0; }
inline void sync() { gmem_sync(); }

/* ---- addresses ---- */
inline u32 ea(const void* p) { return p ? (u32)((const uint8_t*)p - PPC_MEM_BASE) : 0u; }
template <class T> inline T* at(u32 a) { return a ? (T*)(PPC_MEM_BASE + a) : nullptr; }

/* f32 values move like lfs / stfs on the console: bit for bit, a signalling NaN stays signalling
 * (ppc.h ppc_lfs_value / ppc_stfs_bits, PowerPC Book I 4.6.2 / 4.6.3). The recompiled reference
 * (vm_gen.h) does the same, so a typed f32 copy compares equal to the original's lfs/stfs pair. */
inline f32 f32_from_bits(u32 v) {
    f32 r;
    memcpy(&r, &v, 4);
    return r;
}
inline u32 f32_bits(f32 f) {
    u32 v;
    memcpy(&v, &f, 4);
    return v;
}
/* f32 <-> FPR (argument and result registers): an f32 in an FPR is the lfs image of its bits; an
 * FPR read as f32 keeps a NaN's bits as stfs would, other values round like frsp (unchanged). */
inline f64 fpr_from_f32(f32 f) { return ppc_lfs_value(f32_bits(f)); }
inline f32 f32_from_fpr(f64 d) { return d != d ? f32_from_bits(ppc_stfs_bits(d)) : (f32)d; }
template <class T> inline f64 fpr_from(T a) {
    if constexpr (std::is_same_v<T, f32>) return fpr_from_f32(a);
    else return (f64)a;
}
template <class T> inline T from_fpr(f64 d) {
    if constexpr (std::is_same_v<T, f32>) return f32_from_fpr(d);
    else return (T)d;
}

template <class T> inline T load(u32 a) {
    static_assert(!std::is_pointer_v<T>, "guest pointers are 32-bit: load<u32> and at<T>()");
    if constexpr (std::is_same_v<T, f32>) return f32_from_bits(gmem_ld32(a));
    else if constexpr (sizeof(T) == 1) { u8 v = gmem_ld8(a); T r; memcpy(&r, &v, 1); return r; }
    else if constexpr (sizeof(T) == 2) { u16 v = gmem_ld16(a); T r; memcpy(&r, &v, 2); return r; }
    else if constexpr (sizeof(T) == 4) { u32 v = gmem_ld32(a); T r; memcpy(&r, &v, 4); return r; }
    else { static_assert(sizeof(T) == 8); u64 v = gmem_ld64(a); T r; memcpy(&r, &v, 8); return r; }
}
template <class T> inline void store(u32 a, T x) {
    static_assert(!std::is_pointer_v<T>, "guest pointers are 32-bit: store<u32>(a, ea(p))");
    if constexpr (std::is_same_v<T, f64>) { u64 v; memcpy(&v, &x, 8); gmem_stf64(a, v); }
    else if constexpr (std::is_same_v<T, f32>) { u32 v; memcpy(&v, &x, 4); gmem_stf32(a, v); }
    else if constexpr (sizeof(T) == 1) { u8 v; memcpy(&v, &x, 1); gmem_st8(a, v); }
    else if constexpr (sizeof(T) == 2) { u16 v; memcpy(&v, &x, 2); gmem_st16(a, v); }
    else if constexpr (sizeof(T) == 4) { u32 v; memcpy(&v, &x, 4); gmem_st32(a, v); }
    else { static_assert(sizeof(T) == 8); u64 v; memcpy(&v, &x, 8); gmem_st64(a, v); }
}

/* ---- automatic native frame (2026-10-05) ----
 * Every decompiled function entered from guest code gets the original's frame (WWHD_FUNC ->
 * AutoFrame): the original's prologue stores are replayed at the original's sp (back chain, LR at
 * its save slot, the callee-saved GPR/FPR values the original saves, from their entry values), and
 * every callee starts at the original's sp. r1 itself stays at the entry sp during the body
 * (stack arguments at r1+8, hand-built frames); do_call places callees at the original's sp while
 * r1 == entry. gabi::Local objects are placed inside that frame (below its register save area,
 * 8-aligned); only if one does not fit does r1 go lower as before (then callees run lower). Stack
 * arguments are written to the frame's parameter area (sp+8...), as GHS does.
 * NativeFrame<SIZE> of the same size becomes a view of the automatic frame. With no table entry
 * (no frame in the original, or WWHD_NATIVE_FRAMES=0) nothing changes. */
struct FrameCtx {
    bool active = false;
    bool tail = false;         /* the original has no frame and only tail-branches: calls at the entry sp */
    u32 entry = 0, sp = 0, size = 0;
    u32 func = 0;
    u32 lo = 0, cur = 0, hi = 0; /* Locals inside the frame grow upward from lo (the original's first
                                  * stack object) to hi (its register save area); [cur, hi) is free */
    FrameCtx* prev = nullptr;
    const struct GabiFrameInfo* fi = nullptr; /* the original's prologue (active frames) */
    bool reload = false; /* gabi::call_site was used: the epilogue reloads the saved registers */
#ifdef GABI_FAST
    bool nostore = false; /* fast mode: the frame's place only, the prologue's stores left out (no reload) */
#endif
};
#ifdef GABI_GAME_BUILD
inline constexpr TlsView<FrameCtx*, &ThreadState::frame_ctx> frame_ctx{};
#else
inline thread_local FrameCtx* frame_ctx = nullptr;
#endif
/* WWHD_NATIVE_LOCALS=1: place gabi::Locals inside the original's frame (upward from its first
 * address-taken object). Off by default (2026-10-05): candidates are written against the
 * established placement (entry - size) and several emulate the original's layout around it, so a
 * moved Local changed what they pass (d_a_obj_mknjd, d_a_npc_md_event, d_a_st, d_a_beam, ...). */
inline bool native_locals() {
    static const bool on = [] { const char* e = getenv("WWHD_NATIVE_LOCALS"); return e && *e == '1'; }();
    return on;
}

/* ---- guest-memory guard (2026-10-27, AGL#2) ----
 * A be<T> (and so a gptr<T>, gfn, ptmf) is a view of guest memory: its accesses go to ea(this). A be<>/gptr<>
 * built as a HOST object (a local `gptr<T> x; x.v = a;`, a host static, a host heap object) has no guest
 * address: ea() maps the host address to an arbitrary guest location (host stack - PPC_MEM_BASE, wrapped to
 * 32 bits), so its "local" store lands somewhere in guest memory and is caught only when that location happens to
 * be compared (AGL#2: candidate-only writes at 6B3308xx that moved per build). Verification builds compile with
 * GABI_GUEST_GUARD=1 (tools/verify/verify.py FP, also mutate.py; objcmp.py leaves it out): every be<>::addr() checks
 * that `this` is a guest reference: fast path guest_ref (within 1 TB of PPC_MEM_BASE, which covers the guest window and
 * every address a candidate derives from a guest pointer by 32-bit arithmetic that ea() wraps like the console, e.g.
 * table[i] with a huge or negative index; or a NULL guest object's member, host addresses in [-64 GB, 4 GB)). Anything
 * else goes to the harness's gmem_host_object (cold): inside this thread's host stack, a malloc block or the verify
 * executable's segments (a host local / heap object / static) it ends the run with the `gabi: host-local be<>/gptr<>` diagnostic
 * (inside a candidate run the input ends as a candidate-only abort, a FAIL row; elsewhere exit status 86); otherwise it is NULL-based arithmetic with a large index (at<T>(0)[i] = host address i*sizeof(T),
 * which ea() wraps like the console) and the access proceeds. Legitimate be<> objects are all guest references: views
 * through at<T>(), object_at, local_static, gabi::Local/Stack/FrameLocal/NativeFrame slots (guest stack), autoglue's
 * table views and guest statics. Other builds (GABI_GUEST_GUARD undefined or 0) compile exactly as before.
 * Fix for a tripping candidate: keep host temporaries as plain values (u32 a; at<T>(a)), or put an object whose
 * address the original takes on the guest stack (gabi::Local / FrameLocal / a GameCube-style class local). */
#ifndef GABI_GUEST_GUARD
#define GABI_GUEST_GUARD 0
#endif
#ifndef GABI_HOST_BE_PROBE
#define GABI_HOST_BE_PROBE 0
#endif
/* a host address ea() may legitimately see as a be<>'s `this` (see above): two unsigned compares */
inline bool guest_ref(const void* p) {
    const uintptr_t a = (uintptr_t)p;
    return a - ((uintptr_t)PPC_MEM_BASE - (1ull << 40)) < (2ull << 40) || a + (1ull << 36) < (1ull << 36) + (1ull << 32);
}
/* the guest window or a NULL-based address below 4 GB: ea() reaches the guest address without wrapping (-guardstrict) */
inline bool guest_window(const void* p) {
    const uintptr_t a = (uintptr_t)p;
    return a - (uintptr_t)PPC_MEM_BASE < (1ull << 32) || a < (1ull << 32);
}
#if GABI_GUEST_GUARD >= 2
/* -guardstrict: also report every access whose host address wraps (a
 * u32 index on a >= 2-byte element such as keys[count - 1] with count 0, or a NULL-based index past 4 GB); behaviour
 * is otherwise that of GABI_GUEST_GUARD=1 (same guard decision, same results) */
#define GABI_GUEST_CHECK(self, size) \
    (__builtin_expect(!::gabi::guest_window(self), 0) \
         ? (gmem_host_wrap(self, size, ::gabi::guest_ref(self)), \
            ::gabi::guest_ref(self) ? (void)0 : gmem_host_object(self, size)) \
         : (void)0)
#elif GABI_GUEST_GUARD
#define GABI_GUEST_CHECK(self, size) \
    (__builtin_expect(!::gabi::guest_ref(self), 0) ? gmem_host_object(self, size) : (void)0)
#else
#define GABI_GUEST_CHECK(self, size) ((void)0)
#endif

/* ---- big-endian guest field ---- */
template <class T> struct be {
    uint8_t _raw[sizeof(T)];
    u32 addr() const { GABI_GUEST_CHECK(this, sizeof(T)); return ea(this); }
    T get() const { return load<T>(addr()); }
    void set(T v) { store<T>(addr(), v); }
    operator T() const { return get(); }
    be& operator=(T v) { set(v); return *this; }
    be& operator=(const be& o) { set(o.get()); return *this; }
#if GABI_HOST_BE_PROBE
    /* static probe (-DGABI_HOST_BE_PROBE=1 -fsyntax-only, 2026-10-27): no be<> (nor a gptr<> or a struct with be<>
     * fields) is ever constructed on the host; views of guest memory are never constructed, so every error is a host
     * object (or a construction in guest memory such as placement new, to be judged by hand) */
    be() = delete;
#else
    be() = default;
#endif
    be(const be&) = delete; /* guest fields live in guest memory: never copied on the host */
    template <class U> be& operator+=(const U& v) { set((T)(get() + v)); return *this; }
    template <class U> be& operator-=(const U& v) { set((T)(get() - v)); return *this; }
    template <class U> be& operator*=(const U& v) { set((T)(get() * v)); return *this; }
    template <class U> be& operator/=(const U& v) { set((T)(get() / v)); return *this; }
    template <class U> be& operator|=(const U& v) { set((T)(get() | v)); return *this; }
    template <class U> be& operator&=(const U& v) { set((T)(get() & v)); return *this; }
    template <class U> be& operator^=(const U& v) { set((T)(get() ^ v)); return *this; }
    template <class U> be& operator<<=(const U& v) { set((T)(get() << v)); return *this; }
    template <class U> be& operator>>=(const U& v) { set((T)(get() >> v)); return *this; }
    T operator++(int) { T o = get(); set((T)(o + 1)); return o; }
    T operator--(int) { T o = get(); set((T)(o - 1)); return o; }
    be& operator++() { set((T)(get() + 1)); return *this; }
    be& operator--() { set((T)(get() - 1)); return *this; }
};

/* ---- guest pointer field ---- */
template <class T> struct gptr {
    be<u32> v;
    operator T*() const { return at<T>(v.get()); }
    T* operator->() const { return at<T>(v.get()); }
    T* get() const { return at<T>(v.get()); }
    gptr& operator=(T* p) { v.set(ea(p)); return *this; }
    gptr& operator=(const gptr& o) { v.set(o.v.get()); return *this; }
};

/* function pointer stored in guest memory (an address of guest code) */
struct gfn {
    be<u32> v;
    u32 get() const { return v.get(); }
};

/* ---- host objects that stand for guest ones (GameCube-style source, tools/verify/autoglue.py) ----
 * A string literal or a decompiled function's name written in source is a host object. When it is
 * passed to a guest call it is replaced by its guest address: string literals by content (the
 * translation unit's strings, registered by autoglue from the RPX image), functions through the
 * VERIFY registry. Older units pass guest addresses (STR(addr), u32 callbacks): unchanged. */
/* A guest address on the host, as ea() sees it: the guest window (plus 1 MB: a member array of a guest
 * object near the top of the 32-bit space lies just past 4 GB) and member addresses of a NULL guest
 * object (host addresses below 4 GB: __PAGEZERO on macOS arm64, never host data). ea() wraps both
 * (d_a_npc_pf1 _delete with this == NULL reads mArcName at 0x7E6).
 * Host string literals live in the host image, above both. */
inline bool is_guest(const void* p) {
    return (uintptr_t)p < 0x100000000ull || (uintptr_t)p - (uintptr_t)PPC_MEM_BASE < 0x100100000ull;
}
/* The functions of one glued source file (autoglue registers one per file with `str` lines): it lets
 * guest_str prefer the strings of the file whose function is running. Several sources can form one
 * unit of the port's decomp build (WWHD_DECOMP), and their unpinned strings then share one registry. */
struct GuestFile {
    const u32* funcs; /* sorted ascending */
    u32 n;
    bool has(u32 f) const {
        u32 lo = 0, hi = n;
        while (lo < hi) {
            u32 mid = (lo + hi) / 2;
            if (funcs[mid] < f) lo = mid + 1;
            else hi = mid;
        }
        return lo < n && funcs[lo] == f;
    }
};
struct GuestStr {
    u32 addr;
    const char* text;      /* content as written in source (host) */
    u32 func;              /* 0: any function of the unit; else only while this function is tested */
    const GuestFile* file; /* the registering source file (nullptr: unknown) */
    GuestStr* next;
    static inline GuestStr* head = nullptr;
    static inline u32 generation = 0; /* bumped by every registration: invalidates guest_str's cache */
    GuestStr(u32 a, const char* t, u32 f = 0, const GuestFile* fl = nullptr)
        : addr(a), text(t), func(f), file(fl), next(head) {
        head = this;
        generation++;
    }
};
struct HostFn {
    const void* fn;
    u32 addr;
    HostFn* next;
    static inline HostFn* head = nullptr;
    HostFn(const void* f, u32 a) : fn(f), addr(a), next(head) { head = this; }
};
[[noreturn]] inline void glue_fatal(const char* what, const char* detail) {
    /* no <cstdio>: it would declare ::remove etc., names some units use */
    __builtin_printf("gabi: %s: %s (tools/verify/autoglue.py registry)\n", what, detail);
    __builtin_trap();
}
inline u32 current_function(); /* address of the function under test (Activation) */
/* The guest copy of a host string literal. In order: (a) the registration pinned to the running function
 * (`in FUNC`), wherever it is in the list; (b) else the unpinned registrations of the running function's
 * own source file; (c) else every unpinned registration. A choice between different addresses within
 * (b), or within (c) when (b) is empty, is a real ambiguity: fatal, add `in FUNC` to the glue line. */
inline const GuestStr* guest_str_scan(const char* s, u32 cur) {
    const GuestStr *pinned = nullptr, *same = nullptr, *any = nullptr;
    bool same_amb = false, any_amb = false;
    for (const GuestStr* g = GuestStr::head; g; g = g->next) {
        if (strcmp(g->text, s)) continue;
        if (g->func) {
            if (g->func == cur && !pinned) pinned = g; /* the first, as before */
            continue;
        }
        if (cur && g->file && g->file->has(cur)) {
            if (same && same->addr != g->addr) same_amb = true;
            same = g;
        }
        if (any && any->addr != g->addr) any_amb = true;
        any = g;
    }
    if (pinned) return pinned;
    if (same) {
        if (same_amb) glue_fatal("ambiguous string in one source file (add `in FUNC` to its glue line)", s);
        return same;
    }
    if (!any) glue_fatal("string literal without a guest copy", s);
    if (any_amb) glue_fatal("ambiguous string (add `in FUNC` to its glue line)", s);
    return any;
}
/* The scan reads the whole list, so results are cached per (literal, running function): a small
 * direct-mapped table per thread, checked against the found registration's text (the pointer alone could
 * be a reused host buffer) and the registry generation. */
struct GuestStrCacheEntry {
    const char* s;
    const char* text;
    u32 cur, gen, addr;
};
[[gnu::noinline]] inline u32 guest_str(const char* s) { /* one call per use, not a copy per call site */
    static thread_local GuestStrCacheEntry cache[256];
    u32 cur = current_function();
    uintptr_t h = ((uintptr_t)s >> 3) ^ ((uintptr_t)s >> 11) ^ (cur * 0x9E3779B1u);
    GuestStrCacheEntry& e = cache[(h ^ (h >> 8)) & 255];
    if (e.s == s && e.cur == cur && e.gen == GuestStr::generation && e.text && !strcmp(e.text, s)) return e.addr;
    const GuestStr* g = guest_str_scan(s, cur);
    e = GuestStrCacheEntry{s, g->text, cur, GuestStr::generation, g->addr};
    return g->addr;
}
inline u32 guest_addr(const char* s) { return is_guest(s) ? ea(s) : guest_str(s); }
#ifdef GABI_FAST_STR
#if !defined(GABI_FAST) || !defined(GABI_GAME_BUILD)
#error "GABI_FAST_STR (the port's WWHD_DECOMP_FAST_STR) is a fast-mode game-build form: it needs GABI_FAST"
#endif
}  /* namespace gabi */
/* the runtime: 1 when the source's guest_str / guest_addr calls use their per-site cache (set before any guest thread
 * runs; 0: the WWHD_DECOMP_FAST_STR=0 off switch) */
extern "C" __attribute__((visibility("default"))) uint8_t g_decomp_fast_str;
#ifndef GABI_FCOUNT
#ifdef GABI_FAST_COUNT
/* GABI_FAST_COUNT (the port's WWHD_DECOMP_FAST_COUNT, Mac statistics only): Local/FrameLocal fast, general; guest_str
 * site hits, misses */
extern "C" __attribute__((visibility("default"))) uint64_t g_decomp_fast_count[4];
#define GABI_FCOUNT(i) __atomic_fetch_add(&g_decomp_fast_count[i], 1, __ATOMIC_RELAXED)
#else
#define GABI_FCOUNT(i) ((void)0)
#endif
#endif
namespace gabi {
/* fast mode: guest_str with one cache entry per call site in the source (each guest_str(...) / guest_addr(...) the
 * decompiled code writes, through the macros below), no thread-local access. An entry holds the last (pointer, running
 * function, registry generation) of its site and what guest_str answered for it, with the registration's text; it
 * answers only when the string at that pointer still equals the text (the same pointer, or strcmp), so the answer is
 * guest_str's: guest_str_scan depends only on the text, the running function and the registry. Shared by the threads:
 * a sequence word (odd while a thread fills the entry; a filler that does not get it just leaves the entry). */
struct StrSite {
    u32 seq;
    u32 cur, gen, addr;
    const char* s;
    const char* text;
};
/* guest_str's answer and its registration's text (the lookup guest_str makes, own cache) */
[[gnu::noinline]] inline u32 guest_str_text(const char* s, u32 cur, const char** text) {
    static thread_local GuestStrCacheEntry cache[256];
    uintptr_t h = ((uintptr_t)s >> 3) ^ ((uintptr_t)s >> 11) ^ (cur * 0x9E3779B1u);
    GuestStrCacheEntry& e = cache[(h ^ (h >> 8)) & 255];
    if (e.s == s && e.cur == cur && e.gen == GuestStr::generation && e.text && !strcmp(e.text, s)) {
        *text = e.text;
        return e.addr;
    }
    const GuestStr* g = guest_str_scan(s, cur);
    e = GuestStrCacheEntry{s, g->text, cur, GuestStr::generation, g->addr};
    *text = g->text;
    return g->addr;
}
[[gnu::noinline, gnu::cold]] inline u32 guest_str_site_miss(StrSite& c, const char* s, u32 cur, u32 gen) {
    if (!__atomic_load_n(&g_decomp_fast_str, __ATOMIC_RELAXED)) return (guest_str)(s);
    const char* text;
    const u32 addr = guest_str_text(s, cur, &text);
    u32 q = __atomic_load_n(&c.seq, __ATOMIC_RELAXED);
    if (!(q & 1) && __atomic_compare_exchange_n(&c.seq, &q, q + 1, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
        __atomic_store_n(&c.cur, cur, __ATOMIC_RELAXED);
        __atomic_store_n(&c.gen, gen, __ATOMIC_RELAXED);
        __atomic_store_n(&c.addr, addr, __ATOMIC_RELAXED);
        __atomic_store_n(&c.s, s, __ATOMIC_RELAXED);
        __atomic_store_n(&c.text, text, __ATOMIC_RELAXED);
        __atomic_store_n(&c.seq, q + 2, __ATOMIC_RELEASE);
    }
    return addr;
}
template <class Site> [[gnu::always_inline]] inline u32 guest_str_site(Site, const char* s) {
    static constinit StrSite c{}; /* one per call site: Site is the site's own lambda type */
    const u32 cur = current_function();
    const u32 gen = GuestStr::generation;
    const u32 q = __atomic_load_n(&c.seq, __ATOMIC_ACQUIRE);
    const char* cs = __atomic_load_n(&c.s, __ATOMIC_RELAXED);
    const char* ct = __atomic_load_n(&c.text, __ATOMIC_RELAXED);
    const u32 ccur = __atomic_load_n(&c.cur, __ATOMIC_RELAXED);
    const u32 cgen = __atomic_load_n(&c.gen, __ATOMIC_RELAXED);
    const u32 ca = __atomic_load_n(&c.addr, __ATOMIC_RELAXED);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
    if (__builtin_expect(!(q & 1) && __atomic_load_n(&c.seq, __ATOMIC_RELAXED) == q && cs == s && ccur == cur && cgen == gen &&
                             ct && (ct == s || !strcmp(ct, s)), 1))
        return GABI_FCOUNT(2), ca; /* (an empty entry has text null) */
    GABI_FCOUNT(3);
    return guest_str_site_miss(c, s, cur, gen);
}
template <class Site> [[gnu::always_inline]] inline u32 guest_addr_site(Site site, const char* s) {
    return is_guest(s) ? ea(s) : guest_str_site(site, s);
}
/* (the source's calls take guest_str_site through the macros at the end of this header, after gabi.h's own uses) */
#endif
inline u32 guest_fn(const void* f) {
    for (const HostFn* h = HostFn::head; h; h = h->next)
        if (h->fn == f) return h->addr;
    glue_fatal("function passed by name has no WWHD address", "add its /* ADDR */ marker");
}

/* ---- guest calls ---- */
struct ArgPack {
    Cpu* c;
    int gi = 3, fi = 1;
    int ns = 0;
    u32 stk[8];
    void put_int(u32 v) {
        if (gi <= 10) c->r[gi] = v;
        else stk[ns++] = v; /* beyond r10: parameter area of the caller's frame */
        gi++;
    }
    template <class A> void put(A a) {
        if constexpr (std::is_floating_point_v<A>) {
            c->f[fi].ps0 = c->f[fi].ps1 = fpr_from(a);
            fi++;
        } else if constexpr (std::is_pointer_v<A> && std::is_function_v<std::remove_pointer_t<A>>) {
#ifdef GABI_GAME_BUILD
            /* the game build: a function pointer a native entry took out of a guest register points into the guest
             * window (ArgFrom); it passes on as that address (the harness never sees one) */
            put_int(is_guest((const void*)a) ? ea((const void*)a) : guest_fn((const void*)a));
#else
            put_int(guest_fn((const void*)a)); /* a decompiled callback passed by name (autoglue registry) */
#endif
        } else if constexpr (std::is_pointer_v<A> && std::is_same_v<std::remove_cv_t<std::remove_pointer_t<A>>, char>) {
            put_int(is_guest(a) ? ea(a) : guest_str(a)); /* a host string literal: its guest copy (autoglue registry) */
        } else if constexpr (std::is_pointer_v<A>) {
            put_int(ea(a));
        } else if constexpr (std::is_same_v<A, std::nullptr_t>) {
            put_int(0);
        } else if constexpr (std::is_enum_v<A>) {
            put_int((u32)(s32)a);
        } else if constexpr (std::is_same_v<A, bool>) {
            put_int(a ? 1u : 0u);
        } else if constexpr (std::is_integral_v<A> && std::is_signed_v<A>) {
            put_int((u32)(s32)a);
        } else if constexpr (std::is_integral_v<A>) {
            put_int((u32)a);
        } else {
            static_assert(sizeof(A) == 0, "unsupported argument type");
        }
    }
};

template <class R> inline R result(Cpu* c) {
    if constexpr (std::is_same_v<R, Pair32>) return {c->r[3], c->r[4]};
    else if constexpr (std::is_void_v<R>) return;
    else if constexpr (std::is_same_v<R, bool>) return c->r[3] != 0; /* GHS tests bool results with cmpwi */
    else if constexpr (std::is_floating_point_v<R>) return from_fpr<R>(c->f[1].ps0);
    else if constexpr (std::is_pointer_v<R>) return at<std::remove_pointer_t<R>>(c->r[3]);
    else return (R)c->r[3];
}

/* stack arguments go to a temporary outgoing-argument frame (back chain at +0, args from +8) */
inline void do_call(Cpu* c, ArgPack& p, u32 addr, int kind) {
    FrameCtx* fc = frame_ctx;
    if (p.ns && fc && fc->active && c->r[1] == fc->entry && fc->sp + 8 + 4 * (u32)p.ns <= (fc->cur > fc->lo ? fc->lo : fc->hi)) {
        /* in the original's frame: stack arguments at sp+8..., the callee at the original's sp */
        u32 sp = c->r[1];
        gmem_stack_args(1);
        for (int i = 0; i < p.ns; i++) store<u32>(fc->sp + 8 + 4 * i, p.stk[i]);
        gmem_stack_args(0);
        c->r[1] = fc->sp;
        gmem_call_top(fc->sp + 8);
        gmem_call(c, addr, kind, p.gi - 3, p.fi - 1);
        gmem_call_top(0);
        c->r[1] = sp;
        return;
    }
    if (!p.ns && fc && fc->active && c->r[1] == fc->entry) {
        /* the callee starts at the original's sp (its linkage area: sp+0 back chain, sp+4 LR) */
        u32 sp = c->r[1];
        c->r[1] = fc->sp;
        gmem_call_top(fc->sp + 8);
        gmem_call(c, addr, kind, p.gi - 3, p.fi - 1);
        gmem_call_top(0);
        c->r[1] = sp;
        return;
    }
    if (!p.ns && fc && fc->tail && c->r[1] == fc->entry) {
        /* the original branches (b / bctr) without a frame: the target runs at the caller's sp */
        u32 sp = c->r[1];
        gmem_call_top(sp + 8);
        gmem_call(c, addr, kind, p.gi - 3, p.fi - 1);
        gmem_call_top(0);
        c->r[1] = sp;
        return;
    }
    if (p.ns) {
        u32 size = (8 + 4 * p.ns + 15) & ~15u;
        u32 sp = c->r[1];
        u32 top = sp;
        if (fc && fc->active && sp == fc->entry) top = (fc->cur > fc->lo ? fc->lo : fc->hi) & ~15u; /* below the frame's Locals and save area */
        c->r[1] = top - size;
        gmem_stack_args(1);
        store<u32>(c->r[1], sp);
        for (int i = 0; i < p.ns; i++) store<u32>(c->r[1] + 8 + 4 * i, p.stk[i]);
        gmem_stack_args(0);
        gmem_call_top(top);
        gmem_call(c, addr, kind, p.gi - 3, p.fi - 1);
        gmem_call_top(0);
        c->r[1] = sp;
    } else {
        /* no stack arguments: still give the callee an outgoing linkage area (back chain + LR save
         * word), as GHS does: a real callee stores LR at caller SP+4, which must not hit live
         * gabi::Local payloads */
        u32 sp = c->r[1];
        c->r[1] = sp - 16; /* reserved only: no back-chain store, so memory the original never writes stays untouched */
        gmem_call_top(sp);
        gmem_call(c, addr, kind, p.gi - 3, p.fi - 1);
        gmem_call_top(0);
        c->r[1] = sp;
    }
}

/* GABI_DECL_AUDIT (tools/verify/decl_lint.py --compile only, never a build): every guest call reports its
 * argument types at its call site (a deprecation warning names call<R, A...>), so the lint sees what each call
 * passes in r3../f1.. */
#ifdef GABI_DECL_AUDIT
#define GABI_DECL_AUDIT_MARK [[deprecated("gabi-decl-audit")]]
#else
#define GABI_DECL_AUDIT_MARK
#endif

#ifdef GABI_GAME_BUILD
/* the game build: do_call with the entry INV and the caller's automatic frame FC passed in (one thread-local
 * access per call site instead of one more in do_call; on Android every thread-local access of the dlopen'ed
 * libmain.so is a TLS descriptor call). The same r1 and stack-argument handling as do_call. */
extern "C" __attribute__((visibility("default"))) void decomp_lcall(Cpu* c, u32 slot, u32 addr, uint64_t* lr_cache);
#ifdef GABI_EDGE_LCALL
/* level 1 step 3 (the port's WWHD_DECOMP_EDGE_LCALL): an edge, a call to a constant address that is no decompiled
 * function (recompiled code, an import stub): gmem_call's guest state for it (pc, LR = call_lr's answer for the
 * running caller, through the call site's one-word cache instead of the caller's site table) and the dispatch table */
extern "C" __attribute__((visibility("default"))) void decomp_edge_call(Cpu* c, u32 addr, uint64_t* lr_cache);
#endif
/* site-exact LR: the return address a mark of the running caller left for a call to TARGET (taken: the mark is
 * cleared), or 0 */
#ifdef GABI_SITE_LR /* the port's WWHD_DECOMP_SITE_LR: without it no source carries a mark, and no call looks for one */
inline u32 site_take(ThreadState& t, const Cpu* c, u32 target) {
    ThreadState::SiteMark& m = t.site[(target >> 2) & 7];
    if (m.key != ((uint64_t)c->decomp_running << 32 | target)) return 0;
    m.key = 0;
    return m.ret;
}
#else
inline constexpr u32 site_take(ThreadState&, const Cpu*, u32) { return 0; }
#endif
[[gnu::noinline]] inline void do_call_g(Cpu* c, ArgPack& p, FrameCtx* fc, u32 addr, int kind, u32 slot, uint64_t* lr_cache) {
    /* KIND 0 / 1: gmem_call (bl / bctrl); 2: the runtime's lean entry of SLOT (lcall below); | 4: c->lr already holds
     * the return address of the call site (site-exact LR: no call_lr answer) */
    auto inv = [&] {
#ifdef GABI_SITE_LR
        if ((kind & 3) == 2) decomp_lcall(c, slot, addr, (kind & 4) ? nullptr : lr_cache);
#else
        if (kind == 2) decomp_lcall(c, slot, addr, lr_cache);
#endif
#ifdef GABI_EDGE_LCALL
        else if (kind == 8) decomp_edge_call(c, addr, lr_cache);
#endif
#ifdef GABI_FAST
        else if (kind == 16) ppc_host_call(c, (PpcFunc)(void*)lr_cache); /* an HLE import's host function (call below) */
#endif
        else gmem_call(c, addr, kind, p.gi - 3, p.fi - 1);
    };
    if (p.ns && fc && fc->active && c->r[1] == fc->entry && fc->sp + 8 + 4 * (u32)p.ns <= (fc->cur > fc->lo ? fc->lo : fc->hi)) {
        u32 sp = c->r[1];
        for (int i = 0; i < p.ns; i++) store<u32>(fc->sp + 8 + 4 * i, p.stk[i]);
        c->r[1] = fc->sp;
        inv();
        c->r[1] = sp;
        return;
    }
    if (!p.ns && fc && fc->active && c->r[1] == fc->entry) {
        u32 sp = c->r[1];
        c->r[1] = fc->sp;
        inv();
        c->r[1] = sp;
        return;
    }
    if (!p.ns && fc && fc->tail && c->r[1] == fc->entry) {
        u32 sp = c->r[1];
        inv();
        c->r[1] = sp;
        return;
    }
    if (p.ns) {
        u32 size = (8 + 4 * p.ns + 15) & ~15u;
        u32 sp = c->r[1];
        u32 top = sp;
        if (fc && fc->active && sp == fc->entry) top = (fc->cur > fc->lo ? fc->lo : fc->hi) & ~15u;
        c->r[1] = top - size;
        store<u32>(c->r[1], sp);
        for (int i = 0; i < p.ns; i++) store<u32>(c->r[1] + 8 + 4 * i, p.stk[i]);
        inv();
        c->r[1] = sp;
    } else {
        u32 sp = c->r[1];
        c->r[1] = sp - 16;
        inv();
        c->r[1] = sp;
    }
}
#ifdef GABI_FAST
/* fast mode: a call through a function pointer or vtable to a decompiled function with an enabled native entry whose
 * shape matches this call's (defined with lcall below); false: the guest call */
template <class R, bool CALL = false, class... A> bool ptr_native_call(ThreadState& t, u32 fn, R* out, A... a);
template <bool CALL = false, class... A> bool ptr_native_call_v(ThreadState& t, u32 fn, A... a);
#endif
#ifdef GABI_FAST
/* fast mode: the host functions of the HLE import stubs 0xC0000000..0xC0040000 (4-byte steps), by (addr - base) >> 2;
 * built by the runtime once the dispatch table is sealed (null: none, WWHD_DECOMP_FAST_HLE=0). A guest call to one goes
 * straight to ppc_host_call (the import's own exception boundary) instead of gmem_call -> ppc_dispatch -> lookup, with
 * the same registers, r1 and pc. */
extern "C" __attribute__((visibility("default"))) const PpcFunc* g_decomp_hle_tab;
#endif
#ifdef GABI_FAST_MUTEX
#ifndef GABI_FAST
#error "GABI_FAST_MUTEX (direct host entries of the OS mutex imports) is a fast-mode form: it needs GABI_FAST"
#endif
/* GABI_FAST_MUTEX (the port's WWHD_DECOMP_FAST_MUTEX): a few HLE imports that take one integer register and return at
 * most one (OSLockMutex, OSUnlockMutex, OSTryLockMutex: sead::CriticalSection, the game's busiest imports) have a direct
 * host entry, by the same index as g_decomp_hle_tab: it gets the Cpu and the argument as ArgPack puts it into r3, puts it
 * there, runs the import's own work in the import's exception boundary and returns r3. A call by address to such an
 * import (gabi::call's HLE branch) takes it instead of ArgPack / do_call_g / ppc_host_call: the same r3, pc and result;
 * r1 is only lowered for the duration of the guest call there and nothing reads it in between. Built by the runtime
 * with g_decomp_hle_tab, only for imports the dispatch table still routes to their own host function (null: none,
 * WWHD_DECOMP_FAST_MUTEX=0). */
using GabiHleDirect = u32 (*)(Cpu* c, u32 r3);
extern "C" __attribute__((visibility("default"))) const GabiHleDirect* g_decomp_hle_direct;
template <class R, class A> constexpr bool hle_direct_shape1() {
    using U = std::remove_cv_t<std::remove_reference_t<A>>;
    constexpr bool arg_ok = (std::is_integral_v<U> && sizeof(U) <= 4) || std::is_enum_v<U> ||
                            (std::is_pointer_v<U> && !std::is_function_v<std::remove_pointer_t<U>> &&
                             !std::is_same_v<std::remove_cv_t<std::remove_pointer_t<U>>, char>);
    if constexpr (std::is_void_v<R>) return arg_ok;
    else return arg_ok && !std::is_reference_v<R> && ((std::is_integral_v<R> && sizeof(R) <= 4) || std::is_enum_v<R>);
}
template <class R, class... A> constexpr bool hle_direct_shape() {
    if constexpr (sizeof...(A) != 1) return false;
    else return hle_direct_shape1<R, A...>();
}
/* the value ArgPack::put gives r3 for these argument types */
template <class A> [[gnu::always_inline]] inline u32 hle_direct_arg(A a) {
    if constexpr (std::is_pointer_v<A>) return ea((const void*)a);
    else if constexpr (std::is_enum_v<A>) return (u32)(s32)a;
    else if constexpr (std::is_same_v<A, bool>) return a ? 1u : 0u;
    else if constexpr (std::is_signed_v<A>) return (u32)(s32)a;
    else return (u32)a;
}
#endif
#ifdef GABI_COLD_FALLBACK
/* GABI_COLD_FALLBACK (the port's WWHD_DECOMP_FAST_COLD_FALLBACK): the guest halves of gabi::call, lcall and
 * WWHD_GLUE_FUNC's guest call out of line, one function per target / shape, so a body that inlines one keeps no
 * ArgPack on its stack (no stack-protector canary for it) and the hot code stays small. Each does what the inline form
 * did, in the same order, with the caller's thread block: the same registers, r1, LR, pc and guest call. */
template <class R, class... A> [[gnu::noinline]] R call_hle_g(ThreadState& t, u32 addr, PpcFunc fn, A... a) {
    Cpu* c = t.cpu;
    ArgPack p{c};
    (p.put(a), ...);
    c->pc = addr;
    do_call_g(c, p, t.frame_ctx, addr, 16, 0, (uint64_t*)(void*)fn);
    return result<R>(c);
}
template <class R, class... A> [[gnu::noinline]] R call_g(ThreadState& t, u32 addr, A... a) {
    Cpu* c = t.cpu;
    ArgPack p{c};
    (p.put(a), ...);
    do_call_g(c, p, t.frame_ctx, addr, 0, 0, nullptr);
    return result<R>(c);
}
#endif
#ifdef GABI_HOSTRT_DIRECT
/* GABI_HOSTRT_DIRECT (the port's WWHD_DECOMP_FAST_HOSTRT_DIRECT): a guest call by address (gabi::call) to a host runtime
 * root or GHS libm function that the runtime enters straight away (hostrt_fast.cpp hostrt_fast_direct: their f_X, or
 * a bit-exact port such as sqrtf's) when no native entry of this call's shape took it: the guest state exactly as
 * do_call_g and gmem_call leave it for such a target (the argument registers as ArgPack puts them, r1 where do_call_g
 * starts a callee without stack arguments, pc = the target), then that function on the thread's Cpu, r1 back, the result
 * from the registers. It skips call_g, do_call_g, gmem_call's checks and hostrt_fast_direct's switch. The runtime's
 * table: g_decomp_hostrt_tab[(addr - 0x028EA000) >> 2] over hostrt_fast_direct's range, null: none (WWHD_HOSTRT_FAST=0,
 * WWHD_DECOMP_FAST_HOSTRT_DIRECT=0, or not built). */
extern "C" __attribute__((visibility("default"))) const PpcFunc* g_decomp_hostrt_tab;
template <class R, class... A> constexpr bool hostrt_direct_ok() {
    constexpr int nf = (0 + ... + (std::is_floating_point_v<A> ? 1 : 0)), ni = (int)sizeof...(A) - nf;
    return ni <= 8 && nf <= 13; /* no stack arguments (ArgPack: r3..r10, f1..f13) */
}
template <class R, class... A> [[gnu::noinline]] R call_hostrt_d(ThreadState& t, u32 addr, PpcFunc f, A... a) {
    Cpu* c = t.cpu;
    ArgPack p{c};
    (p.put(a), ...);
    const FrameCtx* fc = t.frame_ctx;
    const u32 sp = c->r[1];
    if (fc && fc->active && sp == fc->entry) c->r[1] = fc->sp;
    else if (!(fc && fc->tail && sp == fc->entry)) c->r[1] = sp - 16;
    c->pc = addr;
    f(c);
    c->r[1] = sp;
    return result<R>(c);
}
#endif
template <class R = void, class... A> inline R call(u32 addr, A... a) {
    ThreadState& t = GABI_TLS_;
#ifdef GABI_FAST
    if (addr - 0xC0000000u < 0x40000u) {
        const PpcFunc* ht = g_decomp_hle_tab;
        if (ht && !(addr & 3)) {
            if (const PpcFunc fn = ht[(addr - 0xC0000000u) >> 2]) {
#ifdef GABI_FAST_MUTEX
                if constexpr (hle_direct_shape<R, A...>()) {
                    const GabiHleDirect* hd = g_decomp_hle_direct;
                    if (const GabiHleDirect d = hd ? hd[(addr - 0xC0000000u) >> 2] : nullptr) {
                        Cpu* c = t.cpu;
                        c->pc = addr;
                        d(c, hle_direct_arg(a...));
                        return result<R>(c);
                    }
                }
#endif
#ifdef GABI_COLD_FALLBACK
                return call_hle_g<R, A...>(t, addr, fn, a...);
#else
                Cpu* c = t.cpu;
                ArgPack p{c};
                (p.put(a), ...);
                c->pc = addr;
                do_call_g(c, p, t.frame_ctx, addr, 16, 0, (uint64_t*)(void*)fn);
                return result<R>(c);
#endif
            }
        }
    }
    /* fast mode: a guest call by address (the headers' gabi::call<R>(ADDR, ...) forms) to a decompiled function with a
     * native entry of this call's shape takes it, as call_ptr does; code addresses only (HLE imports skip the probe) */
    if (addr < 0x10000000u && t.probe != addr) {
        const u32 saved_probe = t.probe;
        t.probe = addr;
        if constexpr (std::is_void_v<R>) {
            const bool done = ptr_native_call_v<true, A...>(t, addr, a...);
            t.probe = saved_probe;
            if (done) return;
        } else if constexpr (!std::is_reference_v<R> && native_type_ok<R>()) {
            R r;
            const bool done = ptr_native_call<R, true, A...>(t, addr, &r, a...);
            t.probe = saved_probe;
            if (done) return r;
        } else {
            t.probe = saved_probe;
        }
    }
#endif
#ifdef GABI_HOSTRT_DIRECT
    if constexpr (hostrt_direct_ok<R, A...>()) {
        if (addr - 0x028EA000u < 0xC000u) {
            const PpcFunc* ht = g_decomp_hostrt_tab;
            if (ht && !(addr & 3)) {
                if (const PpcFunc f = ht[(addr - 0x028EA000u) >> 2]) return call_hostrt_d<R, A...>(t, addr, f, a...);
            }
        }
    }
#endif
#ifdef GABI_COLD_FALLBACK
    return call_g<R, A...>(t, addr, a...);
#else
    Cpu* c = t.cpu;
    ArgPack p{c};
    (p.put(a), ...);
#ifdef GABI_SITE_LR
    int kind = 0;
    if (const u32 lr = site_take(t, c, addr)) { c->lr = lr; kind = 4; }
    do_call_g(c, p, t.frame_ctx, addr, kind, 0, nullptr);
#else
    do_call_g(c, p, t.frame_ctx, addr, 0, 0, nullptr);
#endif
    return result<R>(c);
#endif
}
#ifdef GABI_COLD_FALLBACK
template <class R, class... A> [[gnu::noinline]] R call_ptr_t_g(ThreadState& t, u32 fn, A... a) {
    Cpu* c = t.cpu;
    ArgPack p{c};
    (p.put(a), ...);
    c->pc = fn;
    do_call_g(c, p, t.frame_ctx, fn, 1, 0, nullptr);
    return result<R>(c);
}
#endif
template <class R = void, class... A> inline R call_ptr(u32 fn, A... a) {
    ThreadState& t = GABI_TLS_;
#ifdef GABI_FAST
    if constexpr (std::is_void_v<R>) {
        if (ptr_native_call_v<false, A...>(t, fn, a...)) return;
    } else if constexpr (!std::is_reference_v<R> && native_type_ok<R>()) {
        R r;
        if (ptr_native_call<R, false, A...>(t, fn, &r, a...)) return r;
    }
#endif
#ifdef GABI_COLD_FALLBACK
    return call_ptr_t_g<R, A...>(t, fn, a...);
#else
    Cpu* c = t.cpu;
    ArgPack p{c};
    (p.put(a), ...);
    c->pc = fn;
    do_call_g(c, p, t.frame_ctx, fn, 1, 0, nullptr);
    return result<R>(c);
#endif
}
#else
template <class R = void, class... A> GABI_DECL_AUDIT_MARK inline R call(u32 addr, A... a) {
    Cpu* c = cpu;
    ArgPack p{c};
    (p.put(a), ...);
    do_call(c, p, addr, 0);
    return result<R>(c);
}
template <class R = void, class... A> GABI_DECL_AUDIT_MARK inline R call_ptr(u32 fn, A... a) {
    Cpu* c = cpu;
    ArgPack p{c};
    (p.put(a), ...);
    c->pc = fn;
    do_call(c, p, fn, 1);
    return result<R>(c);
}
#endif

#ifdef GABI_GAME_BUILD
/* ---- lean calls (the port's native-calls design, form L; game build only) ----
 * A call to a decompiled function of another game unit (autoglue's `ext` stubs; the port's gen.py writes them as
 * lcall<ADDR, R>(args)). The guest state is built exactly as for gabi::call (ArgPack, do_call's r1 and stack
 * arguments, do_call_g); only the entry differs: instead of gmem_call (call_lr, the direct table's hash probe, the unit's
 * decomp_run) the runtime's decomp_lcall enters the callee's slot by its index, with the same LR, pc and running
 * function, or the dispatch table where gmem_call would take it (a frame-exact caller). g_decomp_native_ok[slot]
 * bit 2: the slot is in the direct table and not hooked since (else the guest call as before). */

/* ---- real native calls (register-exact functions, NativeInfo flags 8; game build only) ----
 * The native entry of a register-exact function takes one u32 per integer argument register and one double per
 * float argument register its definition takes (`this` first), in host registers, and returns its result the same
 * way (u32 r3, u64 r3:r4, double f1, or nothing). The values are exactly what ArgPack / store_ret would have put into
 * the guest registers and what ArgFrom / result take out of them, so the callee sees what the guest path gives it;
 * only the guest registers stay as they were (the decl table says no correct body reads them). */
template <size_t> using native_u32_t = u32;
template <size_t> using native_f64_t = double;
template <class T> constexpr bool native_fcls() { return std::is_floating_point_v<std::remove_cv_t<std::remove_reference_t<T>>>; }
template <u32 RK> struct NativeRet { using type = u32; };
template <> struct NativeRet<0> { using type = void; };
template <> struct NativeRet<2> { using type = uint64_t; };
template <> struct NativeRet<3> { using type = double; };
template <class R> constexpr u32 native_rk() {
    if constexpr (std::is_void_v<R>) return 0;
    else if constexpr (std::is_same_v<R, Pair32>) return 2;
    else if constexpr (std::is_floating_point_v<R>) return 3;
    else return 1;
}
/* a caller's arguments as ArgPack::put converts them, into host values */
struct NativePack {
    u32 r[8] = {};
    double f[8] = {};
    int gi = 0, fi = 0;
    template <class A> void put(A a) {
        if constexpr (std::is_floating_point_v<A>) f[fi++] = fpr_from(a);
        else if constexpr (std::is_pointer_v<A> && std::is_function_v<std::remove_pointer_t<A>>)
            r[gi++] = is_guest((const void*)a) ? ea((const void*)a) : guest_fn((const void*)a); /* as ArgPack (game build) */
        else if constexpr (std::is_pointer_v<A> && std::is_same_v<std::remove_cv_t<std::remove_pointer_t<A>>, char>) r[gi++] = is_guest(a) ? ea(a) : guest_str(a);
        else if constexpr (std::is_pointer_v<A>) r[gi++] = ea(a);
        else if constexpr (std::is_same_v<A, std::nullptr_t>) r[gi++] = 0;
        else if constexpr (std::is_enum_v<A>) r[gi++] = (u32)(s32)a;
        else if constexpr (std::is_same_v<A, bool>) r[gi++] = a ? 1u : 0u;
        else if constexpr (std::is_integral_v<A> && std::is_signed_v<A>) r[gi++] = (u32)(s32)a;
        else r[gi++] = (u32)a;
    }
};
/* the result as gabi::result converts the guest registers */
template <class R, class CR> inline R native_result(CR v) {
    if constexpr (std::is_same_v<R, Pair32>) return {(u32)(v >> 32), (u32)v};
    else if constexpr (std::is_same_v<R, bool>) return (u32)v != 0;
    else if constexpr (std::is_floating_point_v<R>) return from_fpr<R>(v);
    else if constexpr (std::is_pointer_v<R>) return at<std::remove_pointer_t<R>>((u32)v);
    else return (R)(u32)v;
}
/* may an lcall to a function with table FLAGS, declared R(A...) at the caller, use its native entry */
template <u32 FLAGS, class R, class... A> constexpr bool lcall_native_ok() {
    if ((FLAGS & 13u) != 13u) return false; /* native callee, listed, register-exact */
    constexpr int nf = (0 + ... + (native_fcls<A>() ? 1 : 0));
    constexpr int ni = (int)sizeof...(A) - nf;
    if (ni > 8 || nf > 8) return false;
    if (!(native_type_ok<A>() && ...) || std::is_reference_v<R>) return false;
    if constexpr (std::is_void_v<R>) return true;
    else return native_type_ok<R>() && native_rk<R>() == ((FLAGS >> 16) & 3u);
}
template <u32 FLAGS, class R, size_t... I, size_t... J>
inline R native_invoke(void* fn, const NativePack& p, std::index_sequence<I...>, std::index_sequence<J...>) {
    using CR = typename NativeRet<(FLAGS >> 16) & 3u>::type;
    using Fn = CR (*)(native_u32_t<I>..., native_f64_t<J>...);
    if constexpr (std::is_void_v<R>) {
        ((Fn)fn)(p.r[I]..., p.f[J]...);
    } else {
        return native_result<R>(((Fn)fn)(p.r[I]..., p.f[J]...));
    }
}
#ifdef GABI_STACKARG
/* GABI_STACKARG (the port's WWHD_DECOMP_FAST_STACKARG): a guest call with stack arguments to a function that reads
 * them from its entry SP (the port's native_rules.py STACK_ARG_READERS) made a native call. native_rules.py --stackarg
 * makes such a callee native for this call only (NativeInfo bit 24): its unit exports no wwhd_ne_ for it and registers
 * no entry with the runtime (NativeReg), so no other call can enter it natively without its stack arguments in place;
 * its own unit has wwhd_sa_ADDR (gabi_native::Sa<ADDR>), and the port's gen.py --stackarg writes its constant-address
 * do_call sites as do_call_sa (lcall's guest path below takes it too).
 *
 * sa_place: the callee's r1 and the stores do_call makes for P's stack arguments, the same in every case (in the
 * original's frame; else a new outgoing-argument frame below r1, or below the automatic frame's Locals and save area,
 * with its back chain): the callee reads and writes its stack arguments at the addresses the guest call gives them,
 * and the stack bytes stay what the guest call leaves. */
inline u32 sa_place(Cpu* c, const ArgPack& p) {
    FrameCtx* fc = frame_ctx;
    if (fc && fc->active && c->r[1] == fc->entry && fc->sp + 8 + 4 * (u32)p.ns <= (fc->cur > fc->lo ? fc->lo : fc->hi)) {
        for (int i = 0; i < p.ns; i++) store<u32>(fc->sp + 8 + 4 * i, p.stk[i]);
        return fc->sp;
    }
    u32 size = (8 + 4 * p.ns + 15) & ~15u;
    u32 sp = c->r[1];
    u32 top = sp;
    if (fc && fc->active && sp == fc->entry) top = (fc->cur > fc->lo ? fc->lo : fc->hi) & ~15u;
    const u32 nsp = top - size;
    store<u32>(nsp, sp);
    for (int i = 0; i < p.ns; i++) store<u32>(nsp + 8 + 4 * i, p.stk[i]);
    return nsp;
}
/* the native call: r1 placed (sa_place) while the callee runs, its register arguments as ArgPack put them into the
 * guest registers (the values the guest entry would take out of them), its result in r3 as the guest entry stores it;
 * as every register-exact native entry under GABI_LEAN_ENTRY it sets no LR / pc. Its WWHD_GLUE_FUNC sees a nested call
 * from a caller=ok body and runs natively (bit 24 sets callee=native); a register-exact entry keeps the r1 it is
 * given (GABI_LEAN_ENTRY), so the body's reads of cpu->r[1] find its stack arguments. */
template <u32 ADDR> [[gnu::always_inline]] inline void sa_native(Cpu* c, const ArgPack& p) {
    static constexpr NativeInfo ni = native_info(ADDR);
    constexpr u32 rk = (ni.flags >> 16) & 3u;
    static_assert(rk <= 1, "do_call_sa: a result in r3 or none");
    static_assert(((ni.flags >> 24) & 1u) && (ni.flags & 9u) == 9u, "do_call_sa: a register-exact callee native by bit 24");
    const u32 sp = c->r[1];
    c->r[1] = sa_place(c, p);
    NativePack np;
    for (int i = 0; i < 8; i++) {
        np.r[i] = c->r[3 + i];
        np.f[i] = c->f[1 + i].ps0;
    }
    if constexpr (rk == 0) {
        native_invoke<ni.flags, void>((void*)gabi_native::Sa<ADDR>::fn, np, std::make_index_sequence<(ni.flags >> 8) & 15u>{},
                                      std::make_index_sequence<(ni.flags >> 12) & 15u>{});
    } else {
        const u32 r = native_invoke<ni.flags, u32>((void*)gabi_native::Sa<ADDR>::fn, np, std::make_index_sequence<(ni.flags >> 8) & 15u>{},
                                                   std::make_index_sequence<(ni.flags >> 12) & 15u>{});
        c->r[3] = r;
    }
    c->r[1] = sp;
}
/* a do_call(C, P, ADDR, KIND) site (gen.py --stackarg): the native call when ADDR's unit made it one and the running
 * body may call natively (caller_ok, as lcall), else do_call as before */
template <u32 ADDR> inline void do_call_sa(Cpu* c, ArgPack& p, int kind) {
    if constexpr (gabi_native::Sa<ADDR>::ok) {
        if (__builtin_expect(GABI_TLS_.caller_ok && p.ns > 0, 1)) {
            sa_native<ADDR>(c, p);
            return;
        }
    }
    do_call(c, p, ADDR, kind);
}
#endif
#ifdef GABI_FAST
/* fast mode, calls through function pointers and vtables (call_ptr): the target's native entry when the runtime
 * enabled one (g_decomp_ptr_tab: a decompiled, register-exact function the direct table still routes to its body) and
 * this call passes exactly the registers it takes (same integer / float counts; the same result kind, or none for a
 * void call); otherwise the guest call. Arguments and result convert as for lcall's native entries. */
template <class... A> constexpr int ptr_nf() { return (0 + ... + (native_fcls<A>() ? 1 : 0)); }
template <class R, class... A> constexpr u32 ptr_call_sig() {
    return (u32)((int)sizeof...(A) - ptr_nf<A...>()) | (u32)ptr_nf<A...>() << 4 | native_rk<R>() << 8 | 0x8000u;
}
template <class R, class... A> constexpr bool ptr_native_types_ok() {
    constexpr int nf = ptr_nf<A...>(), ni = (int)sizeof...(A) - nf;
    if (ni > 8 || nf > 8 || std::is_reference_v<R>) return false;
    if (!(native_type_ok<A>() && ...)) return false;
    if constexpr (std::is_void_v<R>) return true;
    else return native_type_ok<R>();
}
template <class R, size_t... I, size_t... J>
[[gnu::always_inline]] inline R ptr_invoke(void* fn, const NativePack& p, std::index_sequence<I...>, std::index_sequence<J...>) {
    if constexpr (std::is_void_v<R>) {
        ((void (*)(native_u32_t<I>..., native_f64_t<J>...))fn)(p.r[I]..., p.f[J]...);
    } else {
        using CR = typename NativeRet<native_rk<R>()>::type;
        return native_result<R>(((CR(*)(native_u32_t<I>..., native_f64_t<J>...))fn)(p.r[I]..., p.f[J]...));
    }
}
/* may a call of shape R(A...) through a pointer take the native entry with signature SIG: the same registers (and the
 * same result kind, or a void call); with GABI_PTR_SUB also an entry that takes fewer integer or float registers */
#ifdef GABI_PTR_SUB
/* GABI_PTR_SUB (the port's WWHD_DECOMP_FAST_PTR_SUB): a call that passes MORE registers than the target's definition
 * takes (executeJob's tail_ptr(mExecute, job, context) to RenderDelegateEntries::dispatch(node)), as lcall_sub_shape
 * allows for lcall. The entry takes the leading integer and float registers (separate register files in both host
 * ABIs). A register-exact target reads nothing else (the decl table: its original reads only the registers its
 * definition takes). A target that is not (bit 14) may read the others through the guest registers (an indirect call
 * of its own that reads an entry register it does not pass, decl_lint's "held" case): ptr_sub_regs puts every argument
 * into the guest registers first, as ArgPack does on the guest call, so they hold what the guest call gives them.
 * g_decomp_ptr_sub: 1 = on (the runtime; WWHD_DECOMP_FAST_PTR_SUB=0: 0, equal shapes only, as before) */
extern "C" __attribute__((visibility("default"))) uint8_t g_decomp_ptr_sub;
template <class R, class... A> [[gnu::always_inline]] inline bool ptr_sig_ok(u32 sig) {
    constexpr u32 mask = std::is_void_v<R> ? ~0x4300u : ~0x4000u;
    if ((sig & mask) == ptr_call_sig<R, A...>()) return true;
    constexpr u32 nf = (u32)ptr_nf<A...>(), nI = (u32)sizeof...(A) - nf;
    if (!g_decomp_ptr_sub || !(sig & 0x8000u) || (sig & 15u) > nI || ((sig >> 4) & 15u) > nf) return false;
    if constexpr (std::is_void_v<R>) return true;
    else return ((sig >> 8) & 3u) == native_rk<R>();
}
/* a sub-shape call of a target that is not register-exact: the guest registers as the guest call puts them */
template <class R, class... A> [[gnu::always_inline]] inline void ptr_sub_regs(const ThreadState& t, u32 sig, A... a) {
    if (__builtin_expect((sig & 0x4000u) && (sig & 0xFFu) != (ptr_call_sig<R, A...>() & 0xFFu), 0)) {
        ArgPack p{t.cpu};
        (p.put(a), ...);
    }
}
#else
template <class R, class... A> [[gnu::always_inline]] inline bool ptr_sig_ok(u32 sig) {
    constexpr u32 mask = std::is_void_v<R> ? ~0x4300u : ~0x4000u;
    return (sig & mask) == ptr_call_sig<R, A...>();
}
#endif
#if defined(GABI_PTR_FRONT) || defined(GABI_CALL_FRONT)
/* GABI_PTR_FRONT (the port's WWHD_DECOMP_FAST_PTR_FRONT): a call_ptr_ic site's word (ptr_ic_v) holds the native entry
 * itself instead of its index in g_decomp_ptr_tab, so a hit reads no table, and a miss checks a small direct-mapped front
 * cache per call shape (ptr_front_v) before g_decomp_ptr_tab (~4 MB: a cold line per lookup at a site whose target
 * changes on most calls, e.g. fpcMtd_Method for every actor's method). GABI_CALL_FRONT (the port's
 * WWHD_DECOMP_FAST_CALL_FRONT): gabi::call by address and call_ptr outside the site caches (ptr_native_call) check the
 * front cache before the table as well (they probed the table on every call).
 * Word: low half the guest address; high half 0 = no native entry of this shape (the guest call), else the entry's host
 * address as a 4-aligned offset from &g_decomp_ptr_tab (the same image), bit 0 = PTR_SUB's guest registers needed
 * (ptr_sub_regs' condition, decided once per target and shape); the value 2 = an entry whose offset does not fit, looked
 * up in the table on each call. Every slot is one 64-bit word, written whole: no torn pairs between threads, and a word
 * only ever names what the read-only table gives for that address and shape. Same entries, shape rule and guest calls as
 * the table lookups they replace. CALL: g_decomp_call_tab (gabi::call by address) instead of g_decomp_ptr_tab. */
#if defined(GABI_PTR_FRONT) && (!defined(GABI_PTR_IC) || defined(GABI_PTR_IC2))
#error "GABI_PTR_FRONT needs GABI_PTR_IC (one word per site) and not GABI_PTR_IC2"
#endif
#ifndef GABI_PTR_FRONT_BITS
#define GABI_PTR_FRONT_BITS 10
#endif
/* the runtime (WWHD_DECOMP_FAST_PTR_FRONT=0 / WWHD_DECOMP_FAST_CALL_FRONT=0, bisecting): bit 0 / 1 = the front cache of
 * call_ptr / gabi::call is not used (every miss and every call_ptr / gabi::call probe takes the table, as before) */
extern "C" __attribute__((visibility("default"))) uint8_t g_decomp_front_off;
template <bool CALL, u32 CS> inline constinit uint64_t ptr_front_v[1u << GABI_PTR_FRONT_BITS] = {};
[[gnu::always_inline]] inline intptr_t ptr_front_base() { return (intptr_t)&g_decomp_ptr_tab; }
/* the table's entry for FN if a call of shape R(A...) may take it (ptr_native_call's rule), else null */
template <bool CALL, class R, class... A> [[gnu::always_inline]] inline const GabiPtrEntry* ptr_front_entry(u32 fn) {
    const GabiPtrEntry* e = gabi_ptr_find<CALL>(fn);
#ifdef GABI_PTR_SUB
    if (e && !ptr_sig_ok<R, A...>(e->sig)) e = nullptr;
#else
    constexpr u32 mask = std::is_void_v<R> ? ~0x4300u : ~0x4000u;
    if (e && (e->sig & mask) != ptr_call_sig<R, A...>()) e = nullptr;
#endif
    return e;
}
/* the word for FN from the front cache, filled from the table on a miss; IC (a site's word) set to it unless null */
template <bool CALL, class R, class... A> [[gnu::noinline]] u32 ptr_front_miss(uint64_t* ic, u32 fn) {
    constexpr u32 CS = ptr_call_sig<R, A...>();
    uint64_t* const slot = &ptr_front_v<CALL, CS>[(fn >> 2) * 0x9E3779B1u >> (32 - GABI_PTR_FRONT_BITS)];
    uint64_t w = __atomic_load_n(slot, __ATOMIC_RELAXED);
    const bool off = (g_decomp_front_off >> (CALL ? 1 : 0)) & 1;
    if ((u32)w != fn || off) {
        u32 hi = 0;
        if (const GabiPtrEntry* e = ptr_front_entry<CALL, R, A...>(fn)) {
            const intptr_t off = (intptr_t)e->fn - ptr_front_base();
            if (off && off == (intptr_t)(int32_t)off && !(off & 3)) {
                hi = (u32)off;
#ifdef GABI_PTR_SUB
                if ((e->sig & 0x4000u) && (e->sig & 0xFFu) != (CS & 0xFFu)) hi |= 1u;
#endif
            } else {
                hi = 2u;
            }
        }
        w = (uint64_t)hi << 32 | fn;
        if (!off) __atomic_store_n(slot, w, __ATOMIC_RELAXED);
    }
    if (ic) __atomic_store_n(ic, w, __ATOMIC_RELAXED);
    return (u32)(w >> 32);
}
/* the front cache's word for FN without a site word (gabi::call by address, call_ptr): a hit inline */
template <bool CALL, class R, class... A> [[gnu::always_inline]] inline u32 ptr_front_lookup(u32 fn) {
    constexpr u32 CS = ptr_call_sig<R, A...>();
    const uint64_t w = __atomic_load_n(&ptr_front_v<CALL, CS>[(fn >> 2) * 0x9E3779B1u >> (32 - GABI_PTR_FRONT_BITS)], __ATOMIC_RELAXED);
    if (__builtin_expect((u32)w == fn, 1)) return (u32)(w >> 32); /* (never filled while switched off) */
    return ptr_front_miss<CALL, R, A...>(nullptr, fn);
}
/* the native entry a nonzero word names; PTR_SUB's guest registers first where the word says so */
template <bool CALL, class R, class... A> [[gnu::noinline]] void* ptr_front_slow(const ThreadState& t, u32 fn, A... a) {
    const GabiPtrEntry* e = ptr_front_entry<CALL, R, A...>(fn);
#ifdef GABI_PTR_SUB
    ptr_sub_regs<R, A...>(t, e->sig, a...);
#else
    (void)t, ((void)a, ...);
#endif
    return e->fn;
}
template <bool CALL, class R, class... A> [[gnu::always_inline]] inline void* ptr_front_fn(const ThreadState& t, u32 hi, u32 fn, A... a) {
    if (__builtin_expect(hi == 2u, 0)) return ptr_front_slow<CALL, R, A...>(t, fn, a...);
#ifdef GABI_PTR_SUB
    if (__builtin_expect(hi & 1u, 0)) {
        ArgPack p{t.cpu};
        (p.put(a), ...);
    }
#endif
    return (void*)(ptr_front_base() + (intptr_t)(int32_t)(hi & ~3u));
}
#endif
template <class R, bool CALL, class... A> inline bool ptr_native_call(ThreadState& t, u32 fn, R* out, A... a) {
    if constexpr (!ptr_native_types_ok<R, A...>()) {
        return false;
    } else {
#ifdef GABI_CALL_FRONT
        const u32 hi = ptr_front_lookup<CALL, R, A...>(fn);
        if (!hi || !t.caller_ok) return false;
        void* const nfn = ptr_front_fn<CALL, R, A...>(t, hi, fn, a...);
        NativePack np;
        (np.put(a), ...);
        constexpr int nf = ptr_nf<A...>(), ni = (int)sizeof...(A) - nf;
        *out = ptr_invoke<R>(nfn, np, std::make_index_sequence<ni>{}, std::make_index_sequence<nf>{});
        return true;
#else
        const GabiPtrEntry* e = gabi_ptr_find<CALL>(fn);
#ifdef GABI_PTR_SUB
        if (!e || !ptr_sig_ok<R, A...>(e->sig) || !t.caller_ok) return false;
        ptr_sub_regs<R, A...>(t, e->sig, a...);
#else
        if (!e || (e->sig & ~0x4000u) != ptr_call_sig<R, A...>() || !t.caller_ok) return false;
#endif
        NativePack np;
        (np.put(a), ...);
        constexpr int nf = ptr_nf<A...>(), ni = (int)sizeof...(A) - nf;
        *out = ptr_invoke<R>(e->fn, np, std::make_index_sequence<ni>{}, std::make_index_sequence<nf>{});
        return true;
#endif
    }
}
template <bool CALL, class... A> inline bool ptr_native_call_v(ThreadState& t, u32 fn, A... a) {
    if constexpr (!ptr_native_types_ok<void, A...>()) {
        return false;
    } else {
#ifdef GABI_CALL_FRONT
        const u32 hi = ptr_front_lookup<CALL, void, A...>(fn);
        if (!hi || !t.caller_ok) return false;
        void* const nfn = ptr_front_fn<CALL, void, A...>(t, hi, fn, a...);
        NativePack np;
        (np.put(a), ...);
        constexpr int nf = ptr_nf<A...>(), ni = (int)sizeof...(A) - nf;
        ptr_invoke<void>(nfn, np, std::make_index_sequence<ni>{}, std::make_index_sequence<nf>{});
        return true;
#else
        const GabiPtrEntry* e = gabi_ptr_find<CALL>(fn);
#ifdef GABI_PTR_SUB
        if (!e || !ptr_sig_ok<void, A...>(e->sig) || !t.caller_ok) return false;
        ptr_sub_regs<void, A...>(t, e->sig, a...);
#else
        if (!e || (e->sig & ~0x4300u) != ptr_call_sig<void, A...>() || !t.caller_ok) return false;
#endif
        NativePack np;
        (np.put(a), ...);
        constexpr int nf = ptr_nf<A...>(), ni = (int)sizeof...(A) - nf;
        ptr_invoke<void>(e->fn, np, std::make_index_sequence<ni>{}, std::make_index_sequence<nf>{});
        return true;
#endif
    }
}
#endif
#ifdef GABI_PTR_IC
/* fast mode (the port's WWHD_DECOMP_FAST_PTR_IC): calls through function pointers and vtables with a cache per call
 * site. The port's gen.py writes call_ptr<R>(fn, ...) / tail_ptr<R>(fn, ...) as call_ptr_ic<decltype([]{}), R>(...)
 * (a type of its own per site, so ptr_ic_v is one word per site): the last target's address and, when it has a native
 * entry of this call's shape, that entry's index in g_decomp_ptr_tab + 1 (0: the guest call), read and written as one
 * 64-bit word (no torn pairs between threads). A hit skips gabi_ptr_find; a miss probes and records. The table is
 * read-only once guest threads run, so a recorded index stays valid. Same entries and guest calls as call_ptr. */
template <class Tag> inline constinit uint64_t ptr_ic_v = 0;
template <class R, class... A> [[gnu::noinline]] const GabiPtrEntry* ptr_ic_miss(uint64_t* ic, u32 fn) {
    const GabiPtrEntry* e = gabi_ptr_find<false>(fn);
    constexpr u32 mask = std::is_void_v<R> ? ~0x4300u : ~0x4000u;
#ifdef GABI_PTR_SUB
    (void)mask;
    if (e && !ptr_sig_ok<R, A...>(e->sig)) e = nullptr;
#else
    if (e && (e->sig & mask) != ptr_call_sig<R, A...>()) e = nullptr;
#endif
    __atomic_store_n(ic, (uint64_t)(e ? (u32)(e - g_decomp_ptr_tab) + 1u : 0u) << 32 | fn, __ATOMIC_RELAXED);
    return e;
}
#ifdef GABI_PTR_IC2
/* GABI_PTR_IC2 (the port's WWHD_DECOMP_FAST_PTR_IC2): two words per site (the last two targets), for sites whose target
 * alternates (a vtable call on objects of two classes); a miss records the new target in word 0 and moves the old word
 * 0 to word 1 (each word written whole, as above) */
template <class Tag> inline constinit uint64_t ptr_ic_v2[2] = {0, 0};
template <class R, class... A> [[gnu::noinline]] const GabiPtrEntry* ptr_ic_miss2(uint64_t* ic, u32 fn) {
    const uint64_t old = __atomic_load_n(&ic[0], __ATOMIC_RELAXED);
    const GabiPtrEntry* e = ptr_ic_miss<R, A...>(&ic[0], fn);
    if ((u32)old != fn) __atomic_store_n(&ic[1], old, __ATOMIC_RELAXED);
    return e;
}
template <class R, class... A> [[gnu::always_inline]] inline const GabiPtrEntry* ptr_ic_find2(uint64_t* ic, u32 fn) {
    for (int k = 0; k < 2; k++) {
        const uint64_t v = __atomic_load_n(&ic[k], __ATOMIC_RELAXED);
        if (__builtin_expect((u32)v == fn && fn != 0, 1)) {
            const u32 i = (u32)(v >> 32);
            return i ? g_decomp_ptr_tab + (i - 1) : nullptr;
        }
    }
    return ptr_ic_miss2<R, A...>(ic, fn);
}
#endif
template <class R, class... A> [[gnu::always_inline]] inline const GabiPtrEntry* ptr_ic_find(uint64_t* ic, u32 fn) {
    const uint64_t v = __atomic_load_n(ic, __ATOMIC_RELAXED);
    if (__builtin_expect((u32)v == fn && fn != 0, 1)) {
        const u32 i = (u32)(v >> 32);
        return i ? g_decomp_ptr_tab + (i - 1) : nullptr;
    }
    return ptr_ic_miss<R, A...>(ic, fn);
}
#ifdef GABI_PTR_FRONT
/* a call_ptr_ic site's word: a hit inline, else the front cache / table (GABI_PTR_FRONT above) */
template <class R, class... A> [[gnu::always_inline]] inline u32 ptr_front_find(uint64_t* ic, u32 fn) {
    const uint64_t v = __atomic_load_n(ic, __ATOMIC_RELAXED);
    if (__builtin_expect((u32)v == fn && fn != 0, 1)) return (u32)(v >> 32);
    return ptr_front_miss<false, R, A...>(ic, fn);
}
#endif
/* the guest halves of call_ptr / tail_ptr (shared per shape, out of line) */
template <class R, class... A> [[gnu::noinline]] R call_ptr_g(u32 fn, A... a) {
    ThreadState& t = GABI_TLS_;
    Cpu* c = t.cpu;
    ArgPack p{c};
    (p.put(a), ...);
    c->pc = fn;
    do_call_g(c, p, t.frame_ctx, fn, 1, 0, nullptr);
    return result<R>(c);
}
template <class R, class... A> [[gnu::noinline]] R tail_ptr_g(u32 fn, A... a);
template <class Tag, bool TAIL, class R, class... A> [[gnu::always_inline]] inline R ptr_ic_call(u32 fn, A... a) {
    if constexpr (ptr_native_types_ok<R, A...>()) {
#ifdef GABI_PTR_FRONT
        if (const u32 hi = ptr_front_find<R, A...>(&ptr_ic_v<Tag>, fn)) {
            ThreadState& t = GABI_TLS_;
            if (__builtin_expect(t.caller_ok, 1)) {
                void* const nfn = ptr_front_fn<false, R, A...>(t, hi, fn, a...);
                NativePack np;
                (np.put(a), ...);
                constexpr int nf = ptr_nf<A...>(), ni = (int)sizeof...(A) - nf;
                return ptr_invoke<R>(nfn, np, std::make_index_sequence<ni>{}, std::make_index_sequence<nf>{});
            }
        }
#else
#ifdef GABI_PTR_IC2
        const GabiPtrEntry* e = ptr_ic_find2<R, A...>(ptr_ic_v2<Tag>, fn);
#else
        const GabiPtrEntry* e = ptr_ic_find<R, A...>(&ptr_ic_v<Tag>, fn);
#endif
        if (e) {  /* (one brace in the source for both forms: autoglue reads this header too) */
            ThreadState& t = GABI_TLS_;
            if (__builtin_expect(t.caller_ok, 1)) {
#ifdef GABI_PTR_SUB
                ptr_sub_regs<R, A...>(t, e->sig, a...);
#endif
                NativePack np;
                (np.put(a), ...);
                constexpr int nf = ptr_nf<A...>(), ni = (int)sizeof...(A) - nf;
                return ptr_invoke<R>(e->fn, np, std::make_index_sequence<ni>{}, std::make_index_sequence<nf>{});
            }
        }
#endif
    }
    if constexpr (TAIL) return tail_ptr_g<R, A...>(fn, a...);
    else return call_ptr_g<R, A...>(fn, a...);
}
template <class Tag, class R = void, class... A> [[gnu::always_inline]] inline R call_ptr_ic(u32 fn, A... a) {
    return ptr_ic_call<Tag, false, R, A...>(fn, a...);
}
template <class Tag, class R = void, class... A> [[gnu::always_inline]] inline R tail_ptr_ic(u32 fn, A... a) {
    return ptr_ic_call<Tag, true, R, A...>(fn, a...);
}
#endif

#ifdef GABI_FAST
/* fast mode, lcall: a callee whose definition takes FEWER registers than the call passes (an ext line that declares
 * more parameters than the decompiled definition reads, e.g. bindTextureSampler: defined (sampler, locations), declared
 * (sampler, locations, s32, u32) by its callers). Its bit-14 entry is called with the caller's arguments: the callee
 * reads its own leading integer and float registers (separate register files in both host ABIs), as the original reads
 * only r3/r4 there; the result kind must match unless the call is void. */
template <class R, class... A> [[gnu::always_inline]] inline bool lcall_sub_shape(u32 sg) {
    constexpr u32 nf = (u32)ptr_nf<A...>(), nI = (u32)sizeof...(A) - nf;
    if ((sg & 0xC000u) != 0xC000u || (sg & 15u) > nI || ((sg >> 4) & 15u) > nf) return false;
    if constexpr (std::is_void_v<R>) return true;
    else return ((sg >> 8) & 3u) == native_rk<R>();
}
#endif
#ifdef GABI_EDGE_LCALL
/* an edge (see decomp_edge_call): the guest state as gabi::call builds it, then the runtime's edge entry */
template <u32 ADDR, class R, class... A> inline R edge_lcall(A... a) {
    ThreadState& t = GABI_TLS_;
    Cpu* c = t.cpu;
    ArgPack p{c};
    (p.put(a), ...);
    static constinit uint64_t lr_cache = 0; /* call_lr's answer for the last caller of this edge */
    do_call_g(c, p, t.frame_ctx, ADDR, 8, 0, &lr_cache);
    return result<R>(c);
}
#endif
#ifdef GABI_COLD_FALLBACK
/* GABI_COLD_FALLBACK: the guest half of lcall (TAG 0) and guest_lcall (TAG 1), out of line; each keeps its own LR cache
 * (one per target and form, as the inline forms' statics were) */
template <u32 ADDR, int TAG, class R, class... A> [[gnu::noinline]] R lcall_g(ThreadState& t, A... a) {
    static constexpr NativeInfo ni = native_info(ADDR);
    Cpu* c = t.cpu;
    ArgPack p{c};
    (p.put(a), ...);
    const uint8_t* ok = g_decomp_native_ok;
    static constinit uint64_t lr_cache = 0; /* the runtime's call_lr answer for the last caller (decomp_lcall) */
    do_call_g(c, p, t.frame_ctx, ADDR, __builtin_expect(ok && (ok[ni.slot] & 2), 1) ? 2 : 0, ni.slot, &lr_cache);
    return result<R>(c);
}
#endif
#ifdef GABI_GUEST_LCALL
/* level 1 step 3b (the port's WWHD_DECOMP_GUEST_LCALL): WWHD_GLUE_FUNC's guest call of a callee of the same unit
 * (it may not run natively) through the runtime's lean entry by slot, as lcall's guest path does for another unit
 * (decomp_lcall: the same LR, pc, running function and slot gmem_call would take; dispatch for a frame-exact caller,
 * gmem_call when the slot is hooked or not in the direct table) */
template <u32 ADDR, class R, class... A> inline R guest_lcall(A... a) {
    static constexpr NativeInfo ni = native_info(ADDR);
    if constexpr (!(ni.flags & 4)) {
        return call<R>(ADDR, a...);
    } else {
        ThreadState& t = GABI_TLS_;
#ifdef GABI_COLD_FALLBACK
        return lcall_g<ADDR, 1, R, A...>(t, a...);
#else
        Cpu* c = t.cpu;
        ArgPack p{c};
        (p.put(a), ...);
        const uint8_t* ok = g_decomp_native_ok;
        static constinit uint64_t lr_cache = 0;
        do_call_g(c, p, t.frame_ctx, ADDR, __builtin_expect(ok && (ok[ni.slot] & 2), 1) ? 2 : 0, ni.slot, &lr_cache);
        return result<R>(c);
#endif
    }
}
#endif
template <u32 ADDR, class R = void, class... A> inline R lcall(A... a) {
    static constexpr NativeInfo ni = native_info(ADDR);
    if constexpr (!(ni.flags & 4)) {
#ifdef GABI_EDGE_LCALL
        return edge_lcall<ADDR, R>(a...);
#else
        return call<R>(ADDR, a...);
#endif
    } else {
        ThreadState& t = GABI_TLS_;
        Cpu* c = t.cpu;
#ifdef GABI_STACKARG
        /* GABI_STACKARG: a stack-argument reader its unit made native for do_call_sa (bit 24, gabi_native::Sa): the
         * arguments as the guest path puts them (ArgPack), then the native call (sa_native) */
        if constexpr (((ni.flags >> 24) & 1u) && gabi_native::Sa<ADDR>::ok) {
            if (__builtin_expect(t.caller_ok, 1)) {
                ArgPack p{c};
                (p.put(a), ...);
                if (__builtin_expect(p.ns > 0, 1)) {
                    sa_native<ADDR>(c, p);
                    return result<R>(c);
                }
            }
        }
#endif
#ifdef GABI_NE_TABLE
        /* fast mode: the callee's exported native entry, bound by the linker (the port's native/K.h gabi_native::Ne) */
        if constexpr (lcall_native_ok<ni.flags, R, A...>() && gabi_native::Ne<ADDR>::ok) {
            if (__builtin_expect(t.caller_ok, 1)) {
                NativePack np;
                (np.put(a), ...);
                return native_invoke<ni.flags, R>((void*)gabi_native::Ne<ADDR>::fn, np, std::make_index_sequence<(ni.flags >> 8) & 15u>{},
                                                  std::make_index_sequence<(ni.flags >> 12) & 15u>{});
            }
        } else
#endif
#ifdef GABI_NATIVE_ENTRIES /* the port's WWHD_DECOMP_NATIVE_ENTRIES: native entries between units (NativeReg) */
        if constexpr (lcall_native_ok<ni.flags, R, A...>()) {
            /* a real native call: the callee's entry by its slot when the runtime enabled it with the signature the
             * table gives (shared thread-local block: the callee's NativeScope finds this caller's Cpu, depth and
             * site mark in place; the callee's own WWHD_GLUE_FUNC checks native_enter_ok again and else makes the
             * guest call itself) */
            const GabiNativeEntry* ne = g_decomp_native_entry;
            if (__builtin_expect(ne && ne[ni.slot].sig == native_sig(ni.flags) && t.caller_ok, 1)) {
                NativePack np;
                (np.put(a), ...);
                return native_invoke<ni.flags, R>(ne[ni.slot].fn, np, std::make_index_sequence<(ni.flags >> 8) & 15u>{},
                                                  std::make_index_sequence<(ni.flags >> 12) & 15u>{});
            }
        }
#endif
#ifdef GABI_FAST
        /* fast mode: a callee that is not register-exact, through its bit-14 entry when this call passes its C++ shape */
        if constexpr (!lcall_native_ok<ni.flags, R, A...>() && ptr_native_types_ok<R, A...>()) {
            const GabiNativeEntry* ne = g_decomp_native_entry;
            if (ne && t.caller_ok) {
                const u32 sg = ne[ni.slot].sig;
                constexpr u32 want = ptr_call_sig<R, A...>() | 0x4000u;
                if (sg == want || (std::is_void_v<R> && (sg & ~0x300u) == want) || lcall_sub_shape<R, A...>(sg)) {
                    NativePack np;
                    (np.put(a), ...);
                    constexpr int nf = ptr_nf<A...>(), nI = (int)sizeof...(A) - nf;
                    return ptr_invoke<R>(ne[ni.slot].fn, np, std::make_index_sequence<nI>{}, std::make_index_sequence<nf>{});
                }
            }
        }
#endif
#ifdef GABI_COLD_FALLBACK
        (void)c;
        return lcall_g<ADDR, 0, R, A...>(t, a...);
#else
        ArgPack p{c};
        (p.put(a), ...);
        const uint8_t* ok = g_decomp_native_ok;
        static constinit uint64_t lr_cache = 0; /* the runtime's call_lr answer for the last caller (decomp_lcall) */
#ifdef GABI_SITE_LR
        int kind = __builtin_expect(ok && (ok[ni.slot] & 2), 1) ? 2 : 0;
        if (const u32 lr = site_take(t, c, ADDR)) { c->lr = lr; kind |= 4; }
        do_call_g(c, p, t.frame_ctx, ADDR, kind, ni.slot, &lr_cache);
#else
        do_call_g(c, p, t.frame_ctx, ADDR, __builtin_expect(ok && (ok[ni.slot] & 2), 1) ? 2 : 0, ni.slot, &lr_cache);
#endif
        return result<R>(c);
#endif
    }
}
#ifdef GABI_FAST
/* fast mode, a call by constant address outside the ext stubs (the headers' gabi::call<R>(0xADDR, ...) wrappers, which
 * the port's gen.py writes as hcall<0xADDR, R>(...) only with WWHD_DECOMP_FAST_HCALL): a register-exact callee of
 * another unit (gabi_native::Ne) whose definition takes exactly the registers this call passes is called through its
 * exported native entry, bound by the linker; this is the call gabi::call's native-table probe takes for it (same
 * shape rule: integer and float counts equal, the result kind equal or a void call). Anything else is gabi::call. */
template <u32 FLAGS, class R, class... A> constexpr bool hcall_native_ok() {
    if constexpr (!lcall_native_ok<FLAGS, R, A...>()) {
        return false;
    } else {
        constexpr u32 nf = (u32)ptr_nf<A...>(), nI = (u32)sizeof...(A) - nf;
        return nI == ((FLAGS >> 8) & 15u) && nf == ((FLAGS >> 12) & 15u);
    }
}
template <u32 ADDR, class R = void, class... A> inline R hcall(A... a) {
#ifdef GABI_NE_TABLE
    static constexpr NativeInfo ni = native_info(ADDR);
    if constexpr (gabi_native::Ne<ADDR>::ok && hcall_native_ok<ni.flags, R, A...>()) {
        ThreadState& t = GABI_TLS_;
        if (__builtin_expect(t.caller_ok, 1)) {
            NativePack np;
            (np.put(a), ...);
            return native_invoke<ni.flags, R>((void*)gabi_native::Ne<ADDR>::fn, np, std::make_index_sequence<(ni.flags >> 8) & 15u>{},
                                              std::make_index_sequence<(ni.flags >> 12) & 15u>{});
        }
    }
#endif
    return call<R>(ADDR, a...);
}
#endif
#endif

/* ---- frame-exact chain calls (cross-build save states, 2026-10-06) ----
 * A call that is on the guest stack when a save state is taken at the frame boundary (the main
 * loop: hd_main -> Framework_initialize -> ... -> Framework_procFrame -> ... -> GameTask_calc) must
 * look exactly like the original's, so a state made by either build loads in the other:
 *  - LR = RET, the original's return address after its bl/bctrl (the callee stores it in this
 *    frame; the loader compares every LR slot of the chain);
 *  - the callee-saved registers hold what the original keeps in them at that call (deeper frames
 *    save them; a state made here and loaded into the original resumes the original with them).
 * The enclosing automatic frame then reloads its saved registers on return (AutoFrame::reload),
 * like the original's epilogue. Without an automatic frame the registers are put back after the
 * call instead. Values the candidate needs after the call should be taken from the registers
 * (e.g. procFrame's self from r30): after a state load they are the snapshot's. */
struct GReg { int r; u32 v; };
struct FReg { int f; f64 ps0, ps1; };
struct SiteFpr { f64 ps0, ps1; };
inline void site_prepare(Cpu* c, u32 ret, std::initializer_list<GReg> g, std::initializer_list<FReg> f, u32* gs, SiteFpr* fs) {
    for (auto& x : g) { gs[x.r] = c->r[x.r]; c->r[x.r] = x.v; }
    for (auto& x : f) { fs[x.f] = {c->f[x.f].ps0, c->f[x.f].ps1}; c->f[x.f].ps0 = x.ps0; c->f[x.f].ps1 = x.ps1; }
    c->lr = ret;
}
inline bool site_reload_on_exit() {
    FrameCtx* fc = frame_ctx;
#ifdef GABI_FAST
    if (fc && fc->nostore) return false; /* no saved registers to reload: site_restore puts them back */
#endif
    if (fc && fc->active && fc->fi) { fc->reload = true; return true; }
    return false;
}
inline void site_restore(Cpu* c, bool reload, std::initializer_list<GReg> g, std::initializer_list<FReg> f, const u32* gs, const SiteFpr* fs) {
    if (reload) return;
    for (auto& x : g) c->r[x.r] = gs[x.r];
    for (auto& x : f) { c->f[x.f].ps0 = fs[x.f].ps0; c->f[x.f].ps1 = fs[x.f].ps1; }
}
template <class R = void, class... A> inline R call_site(u32 ret, std::initializer_list<GReg> g, std::initializer_list<FReg> f, u32 addr, A... a) {
    Cpu* c = cpu;
    ArgPack p{c};
    (p.put(a), ...);
    u32 gs[32]; SiteFpr fs[32];
    site_prepare(c, ret, g, f, gs, fs);
    bool rl = site_reload_on_exit();
    do_call(c, p, addr, 0);
    site_restore(c, rl, g, f, gs, fs);
    return result<R>(c);
}
template <class R = void, class... A> inline R call_ptr_site(u32 ret, std::initializer_list<GReg> g, std::initializer_list<FReg> f, u32 fn, A... a) {
    Cpu* c = cpu;
    ArgPack p{c};
    (p.put(a), ...);
    u32 gs[32]; SiteFpr fs[32];
    site_prepare(c, ret, g, f, gs, fs);
    bool rl = site_reload_on_exit();
    c->pc = fn;
    do_call(c, p, fn, 1);
    site_restore(c, rl, g, f, gs, fs);
    return result<R>(c);
}

/* tail call (the original branches with b / bctr and keeps its caller's frame): the target runs
 * with the candidate's own r1, not 16 bytes lower, and stores LR into the caller's linkage area
 * like the original's target (game test 2026-10-05: d_file_select pointer-to-member dispatchers
 * shifted every frame below them, which changed stale stack bytes later copied into the save).
 * No stack arguments. */
template <class R = void, class... A> GABI_DECL_AUDIT_MARK inline R tail_ptr(u32 fn, A... a) {
#ifdef GABI_FAST
    { /* fast mode: the target's native entry as call_ptr takes it (the callee keeps r1 under a tail frame) */
        ThreadState& t = GABI_TLS_;
        if constexpr (std::is_void_v<R>) {
            if (ptr_native_call_v<false, A...>(t, fn, a...)) return;
        } else if constexpr (!std::is_reference_v<R> && native_type_ok<R>()) {
            R r;
            if (ptr_native_call<R, false, A...>(t, fn, &r, a...)) return r;
        }
    }
#endif
    Cpu* c = cpu;
    ArgPack p{c};
    (p.put(a), ...);
    c->pc = fn;
    u32 sp = c->r[1];
    gmem_call_top(c->r[1] + 8);
    gmem_call(c, fn, 1, p.gi - 3, p.fi - 1);
    gmem_call_top(0);
    c->r[1] = sp;
    return result<R>(c);
}
#ifdef GABI_PTR_IC
template <class R, class... A> [[gnu::noinline]] R tail_ptr_g(u32 fn, A... a) {
    Cpu* c = cpu;
    ArgPack p{c};
    (p.put(a), ...);
    c->pc = fn;
    u32 sp = c->r[1];
    gmem_call_top(c->r[1] + 8);
    gmem_call(c, fn, 1, p.gi - 3, p.fi - 1);
    gmem_call_top(0);
    c->r[1] = sp;
    return result<R>(c);
}
#endif

/* ---- guest stack temporaries (for locals whose address is passed to guest code) ---- */
#ifdef GABI_FAST_LOCAL
#if !defined(GABI_FAST) || !defined(GABI_GAME_BUILD)
#error "GABI_FAST_LOCAL (the port's WWHD_DECOMP_FAST_LOCAL) is a fast-mode game-build form: it needs GABI_FAST"
#endif
}  /* namespace gabi */
/* the runtime: 1 when gabi::Local / FrameLocal take their fast form (set before any guest thread runs; 0: the
 * WWHD_DECOMP_FAST_LOCAL=0 off switch, or WWHD_NATIVE_LOCALS=1, whose in-frame placement keeps the general form) */
extern "C" __attribute__((visibility("default"))) uint8_t g_decomp_fast_local;
#ifndef GABI_FCOUNT
#ifdef GABI_FAST_COUNT
/* GABI_FAST_COUNT (the port's WWHD_DECOMP_FAST_COUNT, Mac statistics only): Local/FrameLocal fast, general; guest_str
 * site hits, misses */
extern "C" __attribute__((visibility("default"))) uint64_t g_decomp_fast_count[4];
#define GABI_FCOUNT(i) __atomic_fetch_add(&g_decomp_fast_count[i], 1, __ATOMIC_RELAXED)
#else
#define GABI_FCOUNT(i) ((void)0)
#endif
#endif
namespace gabi {
/* fast mode, the established placement (native_locals() off): the block looked up once, r1 read once, and the slot
 * zeroed inline (gmem_stack_fresh of the game build is this memset); the same values and stores as the general form */
[[gnu::always_inline]] inline Cpu* local_fast_place(u32 size, u32* a, u32* prev) {
    ThreadState& t = GABI_TLS_;
    FrameCtx* fc = t.frame_ctx;
    Cpu* c = t.cpu;
    const u32 r1 = c->r[1];
    *prev = r1;
    u32 top = r1;
    if (fc && fc->active && fc->cur > fc->lo) top = fc->lo & ~15u; /* below in-frame Locals */
    const u32 na = top - size;
    c->r[1] = na;
    *a = na;
    return c;
}
#endif
template <class T> struct Local {
    static constexpr u32 kSize = (sizeof(T) + 15) & ~15u;
    u32 a;
    u32 prev_bump = 0;
    bool in_frame = false;
#ifdef GABI_FAST_LOCAL
    Cpu* fast_c = nullptr; /* the fast form's Cpu (null: the general form); the dtor restores r1 through it */
#endif
    /* a fresh stack slot: zeroed, as the harness's scratch stack starts out (GHS gives each
     * temporary its own frame slot; reused host-side slots would otherwise carry stale bytes
     * into the uninitialised parts of objects passed to callees) */
    Local() {
#ifdef GABI_FAST_LOCAL
        if (__builtin_expect(g_decomp_fast_local, 1)) {
            GABI_FCOUNT(0);
            fast_c = local_fast_place(kSize, &a, &prev_bump);
            memset(PPC_MEM_BASE + a, 0, kSize);
            return;
        }
        GABI_FCOUNT(1);
#endif
        FrameCtx* fc = frame_ctx;
        /* GHS packs its stack objects 4-aligned (word structs; d_a_beam's SafeStrings at sp+0xC, +0x14,
         * ...) from the lowest address-taken frame offset upward */
        constexpr u32 kAlign = 4;
        constexpr u32 kFrameSize = (sizeof(T) + kAlign - 1) & ~(kAlign - 1);
        if (fc && fc->active && cpu->r[1] == fc->entry && native_locals()) {
            u32 at = (fc->cur + kAlign - 1) & ~(kAlign - 1);
            if (at + kFrameSize <= fc->hi) {
                prev_bump = fc->cur;
                a = at;
                fc->cur = at + kFrameSize;
                in_frame = true;
                /* the slot the harness checks callers against: the 16-byte slot a Local always had,
                 * as far as the frame space above it allows */
                u32 slot = fc->hi - a < kSize ? fc->hi - a : kSize;
                gmem_stack_fresh(a, slot, sizeof(T));
                return;
            }
        }
        if (fc && fc->active && cpu->r[1] == fc->entry && native_locals())
            gmem_frame_fallback(fc->func, (u32)sizeof(T), fc->cur - fc->lo, fc->hi - fc->lo, fc->size);
        {
            /* the established placement (entry - size, r1 lowered): many candidates are written
             * against it (hand-emulated linkage words, one big Local as the frame, strcpy overflows
             * into the caller's linkage); while such a Local lives, callees run below it as before */
            prev_bump = cpu->r[1];
            u32 top = cpu->r[1];
            if (fc && fc->active && fc->cur > fc->lo) top = fc->lo & ~15u; /* below in-frame Locals */
            cpu->r[1] = top - kSize;
            a = cpu->r[1];
        }
        gmem_stack_fresh(a, kSize, sizeof(T));
    }
    ~Local() {
#ifdef GABI_FAST_LOCAL
        if (fast_c) { fast_c->r[1] = prev_bump; return; }
#endif
        if (in_frame) frame_ctx->cur = prev_bump;
        else cpu->r[1] = prev_bump;
    }
    Local(const Local&) = delete;
    T* get() const { return at<T>(a); }
    T* operator->() const { return get(); }
    T& operator*() const { return *get(); }
    operator T*() const { return get(); }
};

/* the original's sp of the running candidate's automatic native frame, or 0 (no frame) */
inline u32 native_sp() { FrameCtx* fc = frame_ctx; return fc && fc->active ? fc->sp : 0; }

/* FrameLocal<T> obj(OFF): a stack object at the original's frame offset sp+OFF (from the
 * disassembly, e.g. `addi r5, r1, 8`) inside the automatic native frame: r1 is not moved, so
 * callees keep starting at the original's sp, and the object's bytes are not initialised (like the
 * original's: what is not written keeps the stale stack contents). Without an automatic frame (no
 * table entry, WWHD_NATIVE_FRAMES=0, or r1 already lowered) it behaves like gabi::Local.
 * Game test 2026-10-06: f_pc_create_iter method/judge and hd_main held a Local while calling, so
 * their callees ran lower and left the empty save slots' stale bytes 0x10 lower. */
template <class T> struct FrameLocal {
    static constexpr u32 kSize = (sizeof(T) + 15) & ~15u;
    u32 a;
    u32 prev_r1 = 0;
    bool in_frame = false;
#ifdef GABI_FAST_LOCAL
    Cpu* fast_c = nullptr; /* the fast form's Cpu when r1 was lowered (null: in the frame, or the general form) */
#endif
    explicit FrameLocal(u32 off) {
#ifdef GABI_FAST_LOCAL
        if (__builtin_expect(g_decomp_fast_local, 1)) { /* the general form below with one block lookup and one r1 read */
            GABI_FCOUNT(0);
            ThreadState& t = GABI_TLS_;
            FrameCtx* fc = t.frame_ctx;
            Cpu* c = t.cpu;
            const u32 r1 = c->r[1];
            if (fc && fc->active && r1 == fc->entry && off >= 8 && off + sizeof(T) <= fc->size) {
                a = fc->sp + off;
                in_frame = true;
                return;
            }
            prev_r1 = r1;
            c->r[1] = r1 - kSize;
            a = r1 - kSize;
            fast_c = c;
            memset(PPC_MEM_BASE + a, 0, kSize);
            return;
        }
        GABI_FCOUNT(1);
#endif
        FrameCtx* fc = frame_ctx;
        if (fc && fc->active && cpu->r[1] == fc->entry && off >= 8 && off + sizeof(T) <= fc->size) {
            a = fc->sp + off;
            in_frame = true;
            return; /* the automatic frame is already reserved for the stack guard */
        }
        prev_r1 = cpu->r[1];
        cpu->r[1] -= kSize;
        a = cpu->r[1];
        gmem_stack_fresh(a, kSize, sizeof(T));
    }
    ~FrameLocal() {
#ifdef GABI_FAST_LOCAL
        if (fast_c) { fast_c->r[1] = prev_r1; return; }
#endif
        if (!in_frame) cpu->r[1] = prev_r1;
    }
    FrameLocal(const FrameLocal&) = delete;
    T* get() const { return at<T>(a); }
    T* operator->() const { return get(); }
    T& operator*() const { return *get(); }
    operator T*() const { return get(); }
};

/* A function's own stack frame laid out like the original (game test 2026-10-05, empty save slots):
 * the original's frame (stwu r1,-SIZE) holds objects at fixed sp offsets and leaves their
 * uninitialised bytes stale; callees run with r1 = that sp. NativeFrame<SIZE> lowers r1 by SIZE-16
 * (do_call adds the 16-byte linkage area, so callees see the original's sp), does not touch the
 * bytes, and sp() is the original's r1, so objects are addressed as in the disassembly (sp() + off).
 * entry() is the caller's sp (the original's back chain / LR save at entry() + 4). */
template <u32 SIZE> struct NativeFrame {
    static_assert(SIZE % 8 == 0 && SIZE >= 16, "PPC frames are 8-byte aligned (GHS)");
    u32 a, e;
    bool view = false;
    NativeFrame() {
        FrameCtx* fc = frame_ctx;
        if (fc && fc->active && fc->size == SIZE && cpu->r[1] == fc->entry) {
            /* the automatic native frame already is this frame */
            view = true;
            e = fc->entry;
            a = fc->sp + 16;
            return;
        }
        e = cpu->r[1];
        cpu->r[1] -= SIZE - 16;
        a = cpu->r[1];
        if (SIZE > 16) gmem_stack_reserve(a, SIZE - 16);
    }
    ~NativeFrame() { if (!view) cpu->r[1] += SIZE - 16; }
    NativeFrame(const NativeFrame&) = delete;
    u32 sp() const { return e - SIZE; }
    u32 entry() const { return e; }
    u32 operator+(u32 off) const { return sp() + off; }
};

/* ---- floating point as the console computes it (recompiler semantics, ppc2c.py) ---- */
/* PowerPC NaN propagation, the rule of the reference gen (recomp.py --ppc-nan-propagation, build/gen/ppc_nan.h;
 * the verification build only): with a NaN operand the result is the first NaN in the order frA, frB, frC
 * (fadds/fsubs: A, B; fmuls: A, C; the fmadd family FRT = FRA*FRC +/- FRB: A, B, C, not the formula's order),
 * quieted, with its sign kept (also by the negating and subtracting forms); an invalid operation without NaN
 * operands gives the default QNaN 7FC00000 (single) / 7FF8000000000000 (double). Host compilers may swap
 * commutative operands, so write the operands in the original's register order and use these helpers where
 * NaN payloads reach memory. */
inline f32 ppc_qnan(f32 x) { u32 b; memcpy(&b, &x, 4); b |= 0x00400000u; memcpy(&x, &b, 4); return x; }
inline f64 ppc_qnan(f64 x) { u64 b; memcpy(&b, &x, 8); b |= 0x0008000000000000ull; memcpy(&x, &b, 8); return x; }
inline f32 ppc_dnan32() { u32 b = 0x7FC00000u; f32 x; memcpy(&x, &b, 4); return x; }
inline f64 ppc_dnan64() { u64 b = 0x7FF8000000000000ull; f64 x; memcpy(&x, &b, 8); return x; }
template <class T> inline T ppc_nan2(T x, T y, T r, T dflt) {
    if (x != x) return ppc_qnan(x); if (y != y) return ppc_qnan(y); return r != r ? dflt : r; }
template <class T> inline T ppc_nan3(T a, T b, T c, T r, T dflt) {
    if (a != a) return ppc_qnan(a); if (b != b) return ppc_qnan(b); if (c != c) return ppc_qnan(c); return r != r ? dflt : r; }
inline f32 fadds_ppc(f32 a, f32 b) { return ppc_nan2(a, b, (f32)((f64)a + (f64)b), ppc_dnan32()); }
inline f32 fsubs_ppc(f32 a, f32 b) { return ppc_nan2(a, b, (f32)((f64)a - (f64)b), ppc_dnan32()); }
inline f32 fmuls_ppc(f32 a, f32 c) { return ppc_nan2(a, c, (f32)((f64)a * (f64)c), ppc_dnan32()); }
inline f32 fdivs_ppc(f32 a, f32 b) { return ppc_nan2(a, b, (f32)((f64)a / (f64)b), ppc_dnan32()); }
/* single-precision arithmetic on FPR (double) operands, as the original computes it when an operand is not a single
 * (e.g. an incoming f1 kept as a double): the reference's to_single(ppc_nan2(a, b, a OP b)), the multiply rounding
 * frC to 25 bits first (round25). Separate names (f*_fpr), so a call with single operands never changes meaning: use them with operands declared f64. */
inline f32 fadds_fpr(f64 a, f64 b) { return (f32)ppc_nan2(a, b, a + b, ppc_dnan64()); }
inline f32 fsubs_fpr(f64 a, f64 b) { return (f32)ppc_nan2(a, b, a - b, ppc_dnan64()); }
inline f32 fmuls_fpr(f64 a, f64 c) { return (f32)ppc_nan2(a, c, a * round25(c), ppc_dnan64()); }
inline f32 fdivs_fpr(f64 a, f64 b) { return (f32)ppc_nan2(a, b, a / b, ppc_dnan64()); }
inline f64 fadd_ppc(f64 a, f64 b) { return ppc_nan2(a, b, a + b, ppc_dnan64()); }
inline f64 fsub_ppc(f64 a, f64 b) { return ppc_nan2(a, b, a - b, ppc_dnan64()); }
inline f64 fmul_ppc(f64 a, f64 c) { return ppc_nan2(a, c, a * c, ppc_dnan64()); }
inline f64 fdiv_ppc(f64 a, f64 b) { return ppc_nan2(a, b, a / b, ppc_dnan64()); }
/* paired singles: each slot follows the same rule (ps_add/ps_sub/ps_mul/ps_div/ps_madd...: apply the helper per slot,
 * operands in the original's frA/frB/frC order) */
inline f32 fmadds(f32 a, f32 c, f32 b) { return ppc_nan3(a, b, c, (f32)((f64)a * (f64)c + (f64)b), ppc_dnan32()); }     /* a*c+b */
inline f32 fmsubs(f32 a, f32 c, f32 b) { return ppc_nan3(a, b, c, (f32)((f64)a * (f64)c - (f64)b), ppc_dnan32()); }     /* a*c-b */
inline f32 fnmadds(f32 a, f32 c, f32 b) { return ppc_nan3(a, b, c, (f32)(-((f64)a * (f64)c + (f64)b)), ppc_dnan32()); }
inline f32 fnmsubs(f32 a, f32 c, f32 b) { return ppc_nan3(a, b, c, (f32)(-((f64)a * (f64)c - (f64)b)), ppc_dnan32()); } /* b-a*c */
inline f64 fmadd(f64 a, f64 c, f64 b) { return ppc_nan3(a, b, c, std::fma(a, c, b), ppc_dnan64()); }
inline f64 fmsub(f64 a, f64 c, f64 b) { return ppc_nan3(a, b, c, std::fma(a, c, -b), ppc_dnan64()); }
inline f64 fnmsub(f64 a, f64 c, f64 b) { return ppc_nan3(a, b, c, -std::fma(a, c, -b), ppc_dnan64()); }
/* float -> s32 conversion (fctiwz: truncating, saturating, NaN -> INT_MIN) */
inline s32 ftoi(f64 d) { return (s32)(u32)ppc_fctiwz(d); }
/* paired-single and sqrt estimates */
inline f64 fres(f64 x) { return ppc_fres(x); }
inline f64 frsqrte(f64 x) { return ppc_frsqrte(x); }

/* ---- candidate registration (harness entry adapters) ---- */
typedef void (*EntryFn)(Cpu*);
enum RetKind { RET_VOID, RET_INT1, RET_INT2, RET_INT4, RET_FLOAT, RET_AGG6, RET_PAIR32 };
struct Candidate {
    u32 addr;
    const char* name;
    EntryFn fn;
    RetKind ret;
    Candidate* next;
    Candidate(u32 a, const char* n, EntryFn f, RetKind r);
};

/* the next integer-class argument at entry: r3..r10, then the words of the caller's parameter area at
 * entry SP+8, SP+12, ... (EABI; the same order ArgPack::put_int uses for outgoing calls). Before
 * 2026-10-11 a 9th integer argument was read from r11 (FW-parts_buttons report, 026F5C08). */
inline u32 int_arg(Cpu* c, int& gi) {
    int i = gi++;
    if (i <= 10) return c->r[i];
    return (u32)*at<be<u32>>(c->r[1] + 8 + 4 * (u32)(i - 11));
}
inline f64 float_arg(Cpu* c, int& fi) {
    if (fi > 8) glue_fatal("more than 8 floating-point arguments", "stack-passed floats are not modelled");
    return c->f[fi++].ps0;
}
template <class T> struct ArgFrom {
    static T get(Cpu* c, int& gi, int& fi) {
        if constexpr (std::is_floating_point_v<T>) return from_fpr<T>(float_arg(c, fi));
        else if constexpr (std::is_pointer_v<T>) return at<std::remove_pointer_t<T>>(int_arg(c, gi));
        else if constexpr (std::is_same_v<T, bool>) return (int_arg(c, gi) & 0xFF) != 0;
        else return (T)int_arg(c, gi);
    }
};

/* a reference parameter (`const cXy& p`): the guest passes the object's address */
template <class T> struct ArgFrom<T&> {
    static T& get(Cpu* c, int& gi, int&) { return *at<T>(int_arg(c, gi)); }
};

template <class R> constexpr RetKind ret_kind() {
    if constexpr (std::is_same_v<R, Pair32>) return RET_PAIR32;
    else if constexpr (std::is_void_v<R>) return RET_VOID;
    else if constexpr (std::is_floating_point_v<R>) return RET_FLOAT;
    else if constexpr (sizeof(R) == 1) return RET_INT1;
    else if constexpr (sizeof(R) == 2) return RET_INT2;
    else return RET_INT4;
}

/* Activation: a decompiled function runs its body only when entered from guest code. When
 * another decompiled function calls it directly (natural C++ call), WWHD_FUNC turns that call
 * into a guest call to its address, so every function is tested in isolation and a native
 * build can route the call wherever that address is implemented. */
struct Activation {
#ifdef GABI_GAME_BUILD
    static constexpr TlsView<int, &ThreadState::depth> depth{};
    static constexpr TlsView<u32, &ThreadState::current> current{}; /* the function under test (WWHD_GLUE_FUNC) */
#else
    static inline thread_local int depth = 0;
    static inline thread_local u32 current = 0; /* the function under test (WWHD_GLUE_FUNC) */
#endif
    Activation() { depth++; }
    explicit Activation(u32 addr) { depth++; current = addr; }
    ~Activation() { depth--; }
    static bool nested() { return depth > 0; }
};
inline u32 current_function() { return Activation::current; }
#ifdef GABI_TP_CACHE
/* WWHD_GLUE_FUNC's Activation with the body's block (GABI_TP_CACHE) */
struct ActivationT {
    ThreadState& t;
    [[gnu::always_inline]] ActivationT(ThreadState& t_, u32 addr) : t(t_) { t.depth++; t.current = addr; }
#ifdef GABI_LEAF
    /* a leaf (NativeScope::leaf): depth only, Activation::current stays the caller's (its scope does not restore it) */
    [[gnu::always_inline]] ActivationT(ThreadState& t_, u32 addr, bool leaf) : t(t_) { t.depth++; if (!leaf) t.current = addr; }
#endif
    [[gnu::always_inline]] ~ActivationT() { t.depth--; }
};
#ifdef GABI_LEAF_BARE
/* GABI_LEAF_BARE (the port's WWHD_DECOMP_FAST_LEAF_BARE): a bare leaf's scope and Activation, which keep nothing (see
 * WWHD_GLUE_FUNC_IMPL_ below). The same constructor arguments as NativeScope / ActivationT, no work */
struct BareScope {
    static constexpr bool leaf = true;
    template <class... A> [[gnu::always_inline]] BareScope(ThreadState&, const NativeInfo&, uint64_t*, bool, const A&...) {}
    template <class R> [[gnu::always_inline]] R ret(R r) { return r; }
    BareScope(const BareScope&) = delete;
};
struct BareActivation {
    [[gnu::always_inline]] BareActivation(ThreadState&, u32, bool) {}
};
/* a parameter whose native check the callee keeps for every caller (native_enter_nc: a char, void or function
 * pointer, which may be a host string literal or a function passed by name) */
template <class A> constexpr bool bare_checked_arg() {
    using U = std::remove_cv_t<std::remove_reference_t<A>>;
    if constexpr (std::is_pointer_v<U>) {
        using P = std::remove_cv_t<std::remove_pointer_t<U>>;
        return std::is_same_v<P, char> || std::is_same_v<P, signed char> || std::is_same_v<P, unsigned char> ||
               std::is_function_v<P> || std::is_void_v<P>;
    } else return false;
}
/* whether the body is bare: NativeInfo bit 22 (a leaf whose unit has no caller=guest and no unbased function:
 * native_rules.py --bare), the types every native entry needs (native_enter_nc's compile-time part: the result
 * rewritten, no reference, native types) and no parameter of a kind bare_checked_arg names. Then nothing of the glue
 * can matter (WWHD_GLUE_FUNC_IMPL_) */
template <u32 FLAGS, class R, bool RET_OK, class... A> constexpr auto glue_bare_of(const A&...) {
    constexpr bool types = RET_OK && !std::is_reference_v<R> && (native_type_ok<R>() || std::is_void_v<R>) &&
                           ((native_type_ok<A>() && !std::is_reference_v<A>) && ...);
#ifdef GABI_BARE_ENTRY
    /* GABI_BARE_ENTRY (the port's WWHD_DECOMP_FAST_BARE_ENTRY): NativeInfo bit 23 (native_rules.py --bare-entry: no
     * line of the leaf's unit, its headers included, calls it by name) lifts the parameter rule. Every call then reaches
     * the body through its native entry (lcall / hcall / call_ptr / gabi::call: each pointer argument rebuilt from a
     * guest address by the thunk, gabi::at) or from guest code (the guest entry: guest registers), so a char, void or
     * function pointer argument is null or inside the guest window and native_enter_nc's check of it always passes */
    constexpr bool args = !(bare_checked_arg<A>() || ...) || ((FLAGS >> 23) & 1);
#else
    constexpr bool args = !(bare_checked_arg<A>() || ...);
#endif
    return std::integral_constant<bool, ((FLAGS >> 22) & 1) && ((FLAGS >> 20) & 1) && types && args>{};
}
#endif
#endif

/* WWHD_FUNC's automatic native frame (see FrameCtx) */
#if defined(GABI_GAME_BUILD) && defined(WWHD_DECOMP_FRAME_SLOTS)
/* The game build with the port's per-slot frame table (its pre-included frames_vis.h: DecompFrameSlot,
 * g_decomp_frame_slots): the running function's frame comes from Cpu::decomp_running instead of a gmem_frame_info
 * lookup per entry, a frame of back chain, LR and GPR saves is replayed without the per-item switch, and the
 * harness's stack-guard calls (no-ops in the game build) are left out. The guest memory, registers and FrameCtx
 * are the same as AutoFrame below gives: the same stores at the same addresses with the same values (the table
 * comes from the analysis gmem_frame_info answers from; its switch-free form only for stores that do not overlap),
 * the same FrameCtx fields. Without the table (null: the port's WWHD_DECOMP_AUTOFRAME_FAST=0) the original code. */
struct AutoFrame {
    FrameCtx ctx;
    [[gnu::always_inline]] explicit AutoFrame(u32 addr) {
        ThreadState& t = GABI_TLS_;
        ctx.prev = t.frame_ctx;
        t.frame_ctx = &ctx;
        ctx.func = addr;
        Cpu* c = t.cpu;
        const DecompFrameSlot* tb = g_decomp_frame_slots;
        u32 s;
        if (__builtin_expect(!tb || !c || !(s = c->decomp_running), 0)) { by_lookup(addr); return; }
        const DecompFrameSlot* e = tb + (s - 1);
        if (__builtin_expect(e->addr != addr, 0)) e = decomp_frame_slot_find(addr);
        if (e->kind == DECOMP_FS_NONE) return;
        if (e->kind == DECOMP_FS_TAIL) { ctx.tail = true; ctx.entry = c->r[1]; return; }
#ifdef GABI_AF_INLINE
        /* GABI_AF_INLINE (the port's WWHD_DECOMP_FAST_AF_INLINE): a frame that is not frame-exact (the fast-mode
         * nostore frame, enter() below) placed inline: the same FrameCtx fields, no call */
        if (__builtin_expect(!(e->reserved & 1), 1)) {
            const u32 entry = c->r[1];
            const u32 sp = entry - e->size;
            ctx.active = true;
            ctx.fi = e->fi;
            ctx.entry = entry;
            ctx.size = e->size;
            ctx.sp = sp;
            ctx.nostore = true;
            ctx.hi = sp + e->hi_off;
            ctx.lo = sp + e->lo_off;
            ctx.cur = ctx.lo;
            return;
        }
#endif
        enter(c, e);
    }
    /* a frame (DECOMP_FS_FRAME / _ITEMS): the original's prologue stores */
    [[gnu::noinline]] void enter(Cpu* c, const DecompFrameSlot* e) {
        const GabiFrameInfo* fi = e->fi;
        const u32 entry = c->r[1];
        const u32 sp = entry - e->size;
        ctx.active = true;
        ctx.fi = fi;
        ctx.entry = entry;
        ctx.size = e->size;
        ctx.sp = sp;
#ifdef GABI_FAST
        /* fast mode: only frame-exact functions (DecompFrameSlot::reserved bit 0, the runtime's frames_init) replay
         * the original's prologue stores; every other frame keeps its place (sp, Locals, callees below it) */
        if (!(e->reserved & 1)) {
            ctx.nostore = true;
        } else
#endif
        if (e->kind == DECOMP_FS_FRAME) {
            if (e->flags & DECOMP_FS_BACKCHAIN) store<u32>(sp, entry);
            if (e->flags & DECOMP_FS_LR) store<u32>(sp + e->lr_off, c->lr);
            const GabiFrameItem* g = e->gpr;
            for (u32 i = 0, n = e->ngpr; i < n; i++) store<u32>(sp + g[i].off, c->r[g[i].reg]);
        } else {
            replay(c, fi, sp, entry);
        }
        ctx.hi = sp + e->hi_off;
        ctx.lo = sp + e->lo_off;
        ctx.cur = ctx.lo;
    }
    /* every item in its order (AutoFrame below) */
    static void replay(Cpu* c, const GabiFrameInfo* fi, u32 sp, u32 entry) {
        for (u32 i = 0; i < fi->n; i++) {
            const GabiFrameItem& it = fi->items[i];
            u32 ea = sp + it.off;
            switch (it.kind) {
            case GABI_FI_BACKCHAIN: store<u32>(sp, entry); continue;
            case GABI_FI_LR: store<u32>(ea, c->lr); break;
            case GABI_FI_GPR: store<u32>(ea, c->r[it.reg]); break;
            case GABI_FI_STFD: { u64 v; memcpy(&v, &c->f[it.reg].ps0, 8); gmem_st64(ea, v); break; }
            case GABI_FI_STFS0: gmem_st32(ea, ppc_stfs_bits(c->f[it.reg].ps0)); break;
            case GABI_FI_STFS1: gmem_st32(ea, ppc_stfs_bits(c->f[it.reg].ps1)); break;
            case GABI_FI_PSQ: {
                gmem_st32(ea, ppc_stfs_bits(c->f[it.reg].ps0)); gmem_st32(ea + 4, ppc_stfs_bits(c->f[it.reg].ps1)); break;
            }
            }
        }
    }
    /* without the table: AutoFrame below, word for word */
    [[gnu::noinline]] void by_lookup(u32 addr) {
        const GabiFrameInfo* fi = gmem_frame_info(addr);
        if (fi && !fi->size && (fi->flags & GABI_FF_TAIL)) { ctx.tail = true; ctx.entry = cpu->r[1]; return; }
        if (!fi || !fi->size) return;
        Cpu* c = cpu;
        ctx.active = true;
        ctx.fi = fi;
        ctx.entry = c->r[1];
        ctx.size = fi->size;
        ctx.sp = ctx.entry - fi->size;
        u32 low = fi->size;
        gmem_stack_args(1);
        replay(c, fi, ctx.sp, ctx.entry);
        for (u32 i = 0; i < fi->n; i++) {
            const GabiFrameItem& it = fi->items[i];
            if (it.kind != GABI_FI_BACKCHAIN && it.off >= 8 && it.off < low) low = it.off;
        }
        gmem_stack_args(0);
        ctx.hi = ctx.sp + (low & ~3u);
        ctx.lo = ctx.sp + (fi->objects_lo >= 8 && fi->objects_lo < low ? fi->objects_lo : 8);
        if (ctx.hi < ctx.lo) ctx.hi = ctx.lo;
        ctx.cur = ctx.lo;
        gmem_stack_reserve(ctx.sp + 8, fi->size - 8);
    }
    /* the epilogue's reloads (AutoFrame below) */
    [[gnu::noinline]] void reload_saved() {
        Cpu* c = cpu;
        const GabiFrameInfo* fi = ctx.fi;
        for (u32 i = 0; i < fi->n; i++) {
            const GabiFrameItem& it = fi->items[i];
            u32 ea = ctx.sp + it.off;
            if (it.kind == GABI_FI_STFS0 || it.kind == GABI_FI_STFS1) c->f[it.reg].ps0 = c->f[it.reg].ps1 = ppc_lfs_value(gmem_ld32(ea));
            else if (it.kind == GABI_FI_PSQ) { c->f[it.reg].ps0 = ppc_lfs_value(gmem_ld32(ea)); c->f[it.reg].ps1 = ppc_lfs_value(gmem_ld32(ea + 4)); }
        }
        for (u32 i = 0; i < fi->n; i++) {
            const GabiFrameItem& it = fi->items[i];
            u32 ea = ctx.sp + it.off;
            if (it.kind == GABI_FI_GPR) c->r[it.reg] = gmem_ld32(ea);
            else if (it.kind == GABI_FI_STFD) { u64 v = gmem_ld64(ea); memcpy(&c->f[it.reg].ps0, &v, 8); }
        }
    }
    [[gnu::always_inline]] ~AutoFrame() {
        if (__builtin_expect(ctx.reload, 0) && ctx.active && ctx.fi) reload_saved();
        GABI_TLS_.frame_ctx = ctx.prev;
    }
    AutoFrame(const AutoFrame&) = delete;
};
#else
struct AutoFrame {
    FrameCtx ctx;
    explicit AutoFrame(u32 addr) {
        ctx.prev = frame_ctx;
        frame_ctx = &ctx;
        ctx.func = addr;
        const GabiFrameInfo* fi = gmem_frame_info(addr);
        if (fi && !fi->size && (fi->flags & GABI_FF_TAIL)) { ctx.tail = true; ctx.entry = cpu->r[1]; return; }
        if (!fi || !fi->size) return;
        Cpu* c = cpu;
        ctx.active = true;
        ctx.fi = fi;
        ctx.entry = c->r[1];
        ctx.size = fi->size;
        ctx.sp = ctx.entry - fi->size;
        u32 low = fi->size; /* lowest offset of the register save area */
        gmem_stack_args(1); /* the replayed prologue stores are frame stores, not candidate overflows */
        for (u32 i = 0; i < fi->n; i++) {
            const GabiFrameItem& it = fi->items[i];
            u32 ea = ctx.sp + it.off;
            switch (it.kind) {
            case GABI_FI_BACKCHAIN: store<u32>(ctx.sp, ctx.entry); continue;
            case GABI_FI_LR: store<u32>(ea, c->lr); break;
            case GABI_FI_GPR: store<u32>(ea, c->r[it.reg]); break;
            case GABI_FI_STFD: { u64 v; memcpy(&v, &c->f[it.reg].ps0, 8); gmem_st64(ea, v); break; }
            case GABI_FI_STFS0: gmem_st32(ea, ppc_stfs_bits(c->f[it.reg].ps0)); break; /* stfs: bit-exact */
            case GABI_FI_STFS1: gmem_st32(ea, ppc_stfs_bits(c->f[it.reg].ps1)); break;
            case GABI_FI_PSQ: {
                gmem_st32(ea, ppc_stfs_bits(c->f[it.reg].ps0)); gmem_st32(ea + 4, ppc_stfs_bits(c->f[it.reg].ps1)); break;
            }
            }
            if (it.off >= 8 && it.off < low) low = it.off;
        }
        gmem_stack_args(0);
        ctx.hi = ctx.sp + (low & ~3u);
        ctx.lo = ctx.sp + (fi->objects_lo >= 8 && fi->objects_lo < low ? fi->objects_lo : 8);
        if (ctx.hi < ctx.lo) ctx.hi = ctx.lo;
        ctx.cur = ctx.lo;
        gmem_stack_reserve(ctx.sp + 8, fi->size - 8);
        /* r1 stays at the entry sp during the body (candidates read stack arguments at r1+8 and
         * build frames by hand from it); do_call starts callees at ctx.sp */
    }
    /* the original's epilogue for a function that set callee-saved registers for a chain call
     * (gabi::call_site): reload them from the frame's save slots (lmw / lwz rN, lfs+lfd fN), so the
     * caller gets back what the slots hold, as with the original (after a save state load: the
     * snapshot's values) */
    ~AutoFrame() {
        if (ctx.active && ctx.reload && ctx.fi) {
            Cpu* c = cpu;
            const GabiFrameInfo* fi = ctx.fi;
            for (u32 i = 0; i < fi->n; i++) { /* singles first: lfs sets ps0 and ps1 */
                const GabiFrameItem& it = fi->items[i];
                u32 ea = ctx.sp + it.off;
                if (it.kind == GABI_FI_STFS0 || it.kind == GABI_FI_STFS1) c->f[it.reg].ps0 = c->f[it.reg].ps1 = ppc_lfs_value(gmem_ld32(ea));
                else if (it.kind == GABI_FI_PSQ) { c->f[it.reg].ps0 = ppc_lfs_value(gmem_ld32(ea)); c->f[it.reg].ps1 = ppc_lfs_value(gmem_ld32(ea + 4)); }
            }
            for (u32 i = 0; i < fi->n; i++) {
                const GabiFrameItem& it = fi->items[i];
                u32 ea = ctx.sp + it.off;
                if (it.kind == GABI_FI_GPR) c->r[it.reg] = gmem_ld32(ea);
                else if (it.kind == GABI_FI_STFD) { u64 v = gmem_ld64(ea); memcpy(&c->f[it.reg].ps0, &v, 8); } /* lfd: ps0 only */
            }
        }
        frame_ctx = ctx.prev;
    }
    AutoFrame(const AutoFrame&) = delete;
};
#endif

template <class R> inline void store_ret(Cpu* c, R r) {
    if constexpr (std::is_same_v<R, Pair32>) { c->r[3] = r.r3; c->r[4] = r.r4; }
    else if constexpr (std::is_floating_point_v<R>) c->f[1].ps0 = c->f[1].ps1 = fpr_from(r);
    else if constexpr (std::is_pointer_v<R>) c->r[3] = ea(r);
    else if constexpr (std::is_same_v<R, bool>) c->r[3] = r ? 1 : 0;
    else if constexpr (std::is_signed_v<R>) c->r[3] = (u32)(s32)r;
    else c->r[3] = (u32)r;
}

/* An entry from guest code may be re-entrant (candidate A -> guest code -> candidate B): save and
 * restore the caller's cpu, activation depth and LR so A's later natural C++ calls still go through
 * the guest (game-test request, 2026-10-04). Results are unchanged for a single, non-nested entry. */
#if defined(GABI_TP_CACHE) && defined(GABI_TP_ENTRY)
/* GABI_TP_ENTRY (the port's WWHD_DECOMP_TP_ENTRY, level 1 step 1a-fix): the guest entry looks the block up once
 * (the TlsView accesses below would each take their own table lookup under GABI_TP_CACHE) and keeps it; the same
 * fields saved, set and restored in the same order as the EntryScope below */
struct EntryScope {
    ThreadState& t;
    Cpu* saved_cpu;
    int saved_depth;
    u32 saved_lr;
    u32 saved_current;
    Cpu* c;
    u32 saved_ok;
    FrameCtx* saved_fc;
    [[gnu::always_inline]] explicit EntryScope(Cpu* c_)
        : t(GABI_TLS_), saved_cpu(t.cpu), saved_depth(t.depth), saved_lr(c_->lr), saved_current(t.current), c(c_) {
        t.cpu = c_;
        t.depth = 0;
        saved_ok = t.caller_ok;
        t.caller_ok = 0;
        saved_fc = t.frame_ctx;
        t.frame_ctx = nullptr;
    }
    [[gnu::always_inline]] ~EntryScope() {
        t.frame_ctx = saved_fc;
        t.caller_ok = saved_ok;
        c->lr = saved_lr;
        t.depth = saved_depth;
        t.current = saved_current;
        t.cpu = saved_cpu;
    }
};
#else
struct EntryScope {
    Cpu* saved_cpu;
    int saved_depth;
    u32 saved_lr;
    u32 saved_current; /* the caller's function under test: its string pins (guest_str) apply again */
    Cpu* c;
    explicit EntryScope(Cpu* c_)
        : saved_cpu(cpu), saved_depth(Activation::depth), saved_lr(c_->lr), saved_current(Activation::current), c(c_) {
        cpu = c_;
        Activation::depth = 0;
#ifdef GABI_GAME_BUILD
        saved_ok = GABI_TLS_.caller_ok;
        GABI_TLS_.caller_ok = 0; /* a guest entry: only a WWHD_GLUE_FUNC body (NativeScope) lets its calls go native */
#endif
#ifdef GABI_SHARED_TLS
        saved_fc = GABI_TLS_.frame_ctx;
        GABI_TLS_.frame_ctx = nullptr; /* one block for all units: the caller's automatic frame is not the callee's */
#endif
    }
#ifdef GABI_GAME_BUILD
    u32 saved_ok;
#endif
#ifdef GABI_SHARED_TLS
    FrameCtx* saved_fc;
#endif
    ~EntryScope() {
#ifdef GABI_SHARED_TLS
        GABI_TLS_.frame_ctx = saved_fc;
#endif
#ifdef GABI_GAME_BUILD
        GABI_TLS_.caller_ok = saved_ok;
#endif
        c->lr = saved_lr;
        Activation::depth = saved_depth;
        Activation::current = saved_current;
        cpu = saved_cpu;
    }
};
#endif

template <class R, class... A> struct Entry {
    template <R (*F)(A...)> static void run(Cpu* c) {
        EntryScope scope(c);
        int gi = 3, fi = 1;
        /* braced initialisation evaluates the arguments in order */
        std::tuple<A...> args{ArgFrom<A>::get(c, gi, fi)...};
        if constexpr (std::is_void_v<R>) {
            std::apply(F, args);
        } else {
            R r = std::apply(F, args);
            store_ret<R>(c, r);
        }
    }
};
template <class C, class R, class... A> struct MEntry {
    template <R (C::*F)(A...)> static void run(Cpu* c) {
        EntryScope scope(c);
        int gi = 4, fi = 1;
        C* self = at<C>(c->r[3]);
        std::tuple<A...> args{ArgFrom<A>::get(c, gi, fi)...};
        auto inv = [self](A... a) { return (self->*F)(a...); };
        if constexpr (std::is_void_v<R>) {
            std::apply(inv, args);
        } else {
            R r = std::apply(inv, args);
            store_ret<R>(c, r);
        }
    }
};
/* a const member function (tww `f32 cM2dGBox::GetLen(const cXy&) const`): `this` is a const
 * object; otherwise the same entry as MEntry */
template <class C, class R, class... A> struct CMEntry {
    template <R (C::*F)(A...) const> static void run(Cpu* c) {
        EntryScope scope(c);
        int gi = 4, fi = 1;
        const C* self = at<const C>(c->r[3]);
        std::tuple<A...> args{ArgFrom<A>::get(c, gi, fi)...};
        auto inv = [self](A... a) { return (self->*F)(a...); };
        if constexpr (std::is_void_v<R>) {
            std::apply(inv, args);
        } else {
            R r = std::apply(inv, args);
            store_ret<R>(c, r);
        }
    }
};
template <class R, class... A> constexpr auto entry_of(R (*)(A...)) { return Entry<R, A...>{}; }
template <class C, class R, class... A> constexpr auto entry_of(R (C::*)(A...)) { return MEntry<C, R, A...>{}; }
template <class C, class R, class... A> constexpr auto entry_of(R (C::*)(A...) const) { return CMEntry<C, R, A...>{}; }
template <class R, class... A> constexpr RetKind ret_of(R (*)(A...)) { return ret_kind<R>(); }
template <class C, class R, class... A> constexpr RetKind ret_of(R (C::*)(A...)) { return ret_kind<R>(); }
template <class C, class R, class... A> constexpr RetKind ret_of(R (C::*)(A...) const) { return ret_kind<R>(); }

#ifdef GABI_GAME_BUILD
/* ---- native entries of register-exact functions (game build; see lcall's real native calls) ----
 * NEntry<...>::Thunk::run<F>(u32 r3.., double f1..): the arguments taken out of host values as ArgFrom takes them out
 * of the guest registers, F called as a plain C++ call (its WWHD_GLUE_FUNC sees a nested call: NativeScope's native
 * path with no guest registers), the result handed back as store_ret would put it. The parameter list is all integer
 * registers, then all float registers, so the caller needs nothing but the counts (NativeInfo flags). */
template <int BASE, class... A> struct NativeShape {
    static constexpr bool fl[sizeof...(A) + 1] = {native_fcls<A>()..., false};
    static constexpr int count(bool f) {
        int n = 0;
        for (size_t i = 0; i < sizeof...(A); i++) n += fl[i] == f;
        return n;
    }
    static constexpr int NI = count(false) + BASE, NF = count(true);
    static constexpr int idx(size_t k) { /* argument k's index among the registers of its class */
        int n = 0;
        for (size_t i = 0; i < k; i++) n += fl[i] == fl[k];
        return fl[k] ? n : n + BASE;
    }
};
template <class T> inline T native_arg(const u32* r, const double* f, int i) {
    if constexpr (std::is_lvalue_reference_v<T>) return *at<std::remove_reference_t<T>>(r[i]);
    else if constexpr (std::is_floating_point_v<T>) return from_fpr<T>(f[i]);
    else if constexpr (std::is_pointer_v<T>) return at<std::remove_pointer_t<T>>(r[i]);
    else if constexpr (std::is_same_v<T, bool>) return (r[i] & 0xFF) != 0;
    else return (T)r[i];
}
template <class R> inline typename NativeRet<native_rk<R>()>::type native_ret_value(R r) { /* store_ret's values */
    if constexpr (std::is_same_v<R, Pair32>) return (uint64_t)r.r3 << 32 | r.r4;
    else if constexpr (std::is_floating_point_v<R>) return fpr_from(r);
    else if constexpr (std::is_pointer_v<R>) return ea(r);
    else if constexpr (std::is_same_v<R, bool>) return r ? 1u : 0u;
    else if constexpr (std::is_signed_v<R>) return (u32)(s32)r;
    else return (u32)r;
}
template <class R, class... A> constexpr bool native_entry_types_ok() {
    /* as native_enter_ok: a reference parameter would make the body take the guest call anyway */
    return !std::is_reference_v<R> && (std::is_void_v<R> || native_type_ok<R>()) &&
           ((native_type_ok<A>() && !std::is_reference_v<A>) && ...);
}
template <class C, bool CONST, class R, class... A> struct NEntryT {
    using S = NativeShape<std::is_void_v<C> ? 0 : 1, A...>;
    using CR = typename NativeRet<native_rk<R>()>::type;
    static constexpr bool ok = native_entry_types_ok<R, A...>() && S::NI <= 8 && S::NF <= 8;
    static constexpr u32 sig = (u32)S::NI | (u32)S::NF << 4 | native_rk<R>() << 8 | 0x8000u;
    template <auto F, size_t... K> static CR invoke(const u32* r, const double* f, std::index_sequence<K...>) {
        if constexpr (std::is_void_v<C>) {
            if constexpr (std::is_void_v<R>) F(native_arg<A>(r, f, S::idx(K))...);
            else return native_ret_value<R>(F(native_arg<A>(r, f, S::idx(K))...));
        } else {
            using CC = std::conditional_t<CONST, const C, C>;
            CC* self = at<CC>(r[0]);
            if constexpr (std::is_void_v<R>) (self->*F)(native_arg<A>(r, f, S::idx(K))...);
            else return native_ret_value<R>((self->*F)(native_arg<A>(r, f, S::idx(K))...));
        }
    }
#ifdef GABI_THUNK_INL
    /* GABI_THUNK_INL (the port's WWHD_DECOMP_FAST_THUNK_INL): invoke with F's body inlined into the thunk (a leaf,
     * NativeInfo bit 20: small, no calls), so a native entry through the thunk is one function, not thunk + body. The
     * same conversions and call; only the call layer goes (F keeps its own out-of-line copy for its other callers) */
    template <auto F, size_t... K> [[gnu::always_inline]] static CR invoke_inl(const u32* r, const double* f, std::index_sequence<K...>) {
        if constexpr (std::is_void_v<C>) {
            if constexpr (std::is_void_v<R>) { [[clang::always_inline]] F(native_arg<A>(r, f, S::idx(K))...); }
            else { [[clang::always_inline]] return native_ret_value<R>(F(native_arg<A>(r, f, S::idx(K))...)); }
        } else {
            using CC = std::conditional_t<CONST, const C, C>;
            CC* self = at<CC>(r[0]);
            if constexpr (std::is_void_v<R>) { [[clang::always_inline]] (self->*F)(native_arg<A>(r, f, S::idx(K))...); }
            else { [[clang::always_inline]] return native_ret_value<R>((self->*F)(native_arg<A>(r, f, S::idx(K))...)); }
        }
    }
#endif
    template <class IS, class JS> struct Fn;
    template <size_t... I, size_t... J> struct Fn<std::index_sequence<I...>, std::index_sequence<J...>> {
        template <auto F> static CR run(native_u32_t<I>... ri, native_f64_t<J>... fj) {
            const u32 r[] = {ri..., 0u};
            const double f[] = {fj..., 0.0};
            return invoke<F>(r, f, std::index_sequence_for<A...>{});
        }
#ifdef GABI_THUNK_INL
        template <auto F> static CR run_inl(native_u32_t<I>... ri, native_f64_t<J>... fj) {
            const u32 r[] = {ri..., 0u};
            const double f[] = {fj..., 0.0};
            return invoke_inl<F>(r, f, std::index_sequence_for<A...>{});
        }
#endif
    };
    using Thunk = Fn<std::make_index_sequence<ok ? S::NI : 0>, std::make_index_sequence<ok ? S::NF : 0>>;
};
template <class R, class... A> constexpr auto nentry_of(R (*)(A...)) { return NEntryT<void, false, R, A...>{}; }
template <class C, class R, class... A> constexpr auto nentry_of(R (C::*)(A...)) { return NEntryT<C, false, R, A...>{}; }
template <class C, class R, class... A> constexpr auto nentry_of(R (C::*)(A...) const) { return NEntryT<C, true, R, A...>{}; }
/* the unit's glue (the port's glue.cpp): hands a native entry to the runtime (decomp_register_native) */
void native_register(u32 addr, void* fn, u32 sig, u32 table_sig);
/* VERIFY's second half in the game build: a register-exact function (the unit's table, flags 1 | 8) registers its
 * native entry; any other one has nothing to register (no code, no static initialisation) */
#ifdef GABI_FAST_ACCESSOR
/* GABI_FAST_ACCESSOR (the port's WWHD_DECOMP_FAST_ACCESSOR): the native entry of a lazy accessor (native_rules.py
 * --accessor, gabi_native::Acc<ADDR> in its own unit's table: kankyo_getEnvlight). The port checked at configure time
 * that the body is exactly `if (GUARD.value == 0) { GUARD.value = 1; ... } return &VALUE;` with a be<u32> guard word:
 * while that word is not 0 the body returns VALUE's address and does nothing else, so the entry returns that guest
 * address itself (what the thunk returns for it) without entering the body and its scope. While it is 0 the thunk runs
 * the body as before (the first call constructs). Every native call of the function goes through this entry (NativeReg
 * registers it, ne_body exports it); the guest entry runs the body as before. */
template <u32 ADDR, auto F> struct AccEntry {
    using E = decltype(nentry_of(F));
    static_assert(E::ok && E::sig == native_sig(native_info(ADDR).flags) && E::sig == (0x8000u | 1u << 8),
                  "AccEntry: a register-exact accessor without parameters and with an r3 result");
    static u32 run() {
#ifdef GABI_NSTATS
        if (uint64_t* n = g_decomp_acc_callers) __atomic_fetch_add(&n[GABI_TLS_.cpu->decomp_running], 1, __ATOMIC_RELAXED);
#endif
        if (__builtin_expect(load<u32>(gabi_native::Acc<ADDR>::guard) != 0, 1)) return gabi_native::Acc<ADDR>::value;
        return E::Thunk::template run<F>();
    }
};
#endif
template <u32 ADDR, auto F> struct NativeReg {
    NativeReg() {
        constexpr NativeInfo ni = native_info(ADDR);
#ifdef GABI_STACKARG
        /* GABI_STACKARG: a stack-argument reader made native for do_call_sa (bit 24) registers no entry: only
         * do_call_sa / lcall (sa_native) enter it natively, with its stack arguments in place */
        if constexpr ((ni.flags >> 24) & 1u) {
        } else
#endif
        if constexpr ((ni.flags & 9u) == 9u) {
            using E = decltype(nentry_of(F));
#ifdef GABI_FAST_ACCESSOR
            if constexpr (gabi_native::Acc<ADDR>::ok) native_register(ADDR, (void*)&AccEntry<ADDR, F>::run, E::sig, native_sig(ni.flags));
            else
#endif
#ifdef GABI_THUNK_INL
            if constexpr (E::ok && ((ni.flags >> 20) & 1u)) native_register(ADDR, (void*)&E::Thunk::template run_inl<F>, E::sig, native_sig(ni.flags));
            else
#endif
            if constexpr (E::ok) native_register(ADDR, (void*)&E::Thunk::template run<F>, E::sig, native_sig(ni.flags));
            else native_register(ADDR, nullptr, 0, native_sig(ni.flags)); /* argument or result types: no entry */
        }
#ifdef GABI_FAST
        /* fast mode: a native callee that is not register-exact gets an entry too, its C++ shape marked with bit 14 (its
         * NativeScope then puts the arguments into the guest registers, r1 / LR, and the result, as on the in-unit native
         * path); only a call that passes exactly that shape takes it (call_ptr, gabi::call, lcall below) */
        else if constexpr ((ni.flags & 5u) == 5u) {
            using E = decltype(nentry_of(F));
            if constexpr (E::ok) native_register(ADDR, (void*)&E::Thunk::template run<F>, E::sig | 0x4000u, E::sig | 0x4000u);
        }
#endif
    }
};
#ifdef GABI_FAST_NE
/* the body of a unit's exported native entry wwhd_ne_ADDR (the port's native/K.h): NativeReg's thunk, whose shape the
 * table states (native_rules emits it only for register-exact functions with an entry) */
template <u32 ADDR, auto F, class CR, class... X> [[gnu::always_inline]] inline CR ne_body(X... x) {
    using E = decltype(nentry_of(F));
    if constexpr (E::ok) {
        static_assert(E::sig == native_sig(native_info(ADDR).flags), "wwhd_ne_: the definition's shape differs from the table's");
#ifdef GABI_FAST_ACCESSOR
        if constexpr (gabi_native::Acc<ADDR>::ok) return AccEntry<ADDR, F>::run(x...);
        else
#endif
#ifdef GABI_THUNK_INL
        if constexpr ((native_info(ADDR).flags >> 20) & 1u) return E::Thunk::template run_inl<F>(x...);
        else
#endif
        return E::Thunk::template run<F>(x...);
    } else if constexpr (std::is_same_v<CR, uint64_t>) {
        /* C++ argument or result types without a native entry (reference parameters; NativeReg registers none): the
         * guest call by address, the values as the guest registers carry them */
        const Pair32 r = call<Pair32>(ADDR, x...);
        return (uint64_t)r.r3 << 32 | r.r4;
    } else if constexpr (std::is_same_v<CR, double>) {
        call<void>(ADDR, x...);
        return cpu->f[1].ps0;
    } else {
        return call<CR>(ADDR, x...);
    }
}
#endif
#endif

}  // namespace gabi

#define GABI_CAT2(a, b) a##b
#define GABI_CAT(a, b) GABI_CAT2(a, b)

/* First statement of every decompiled function: its WWHD address, return type and arguments
 * (with `this` first for methods). */
#define WWHD_FUNC(addr, R, ...)                                                  \
    if (gabi::Activation::nested()) return gabi::call<R>(addr __VA_OPT__(, ) __VA_ARGS__); \
    gabi::Activation wwhd_activation_;                                           \
    gabi::AutoFrame wwhd_native_frame_(addr)

/* VERIFY(0x021E01B8, daMtoge_actionUp) / VERIFY(0x021DFF30, &daMtoge_c::calcMtx): this source
 * function implements the WWHD function at that address (registers it with the harness). */
#if defined(GABI_GAME_BUILD) && defined(GABI_NATIVE_ENTRIES)
#define GABI_NATIVE_REG_(addr, fn) ; static gabi::NativeReg<addr, fn> GABI_CAT(wwhd_native_reg_, __LINE__)
#else
#define GABI_NATIVE_REG_(addr, fn)
#endif
/* fast mode (GABI_FAST_NE, the port's WWHD_DECOMP_FAST): each register-exact function is also exported under its
 * address, wwhd_ne_XXXXXXXX, for direct calls from other units (lcall); the port's native/K.h defines GABI_NE_<addr>
 * per VERIFY address (empty for a function that has no such entry) */
#if defined(GABI_GAME_BUILD) && defined(GABI_FAST_NE)
#define GABI_FAST_NE_(addr, fn) ; GABI_CAT(GABI_NE_, addr)(fn)
#else
#define GABI_FAST_NE_(addr, fn)
#endif
#define VERIFY(addr, fn)                                                                        \
    static gabi::Candidate GABI_CAT(wwhd_verify_, __LINE__)(addr, #fn,                         \
        &decltype(gabi::entry_of(fn))::template run<fn>, gabi::ret_of(fn)) GABI_NATIVE_REG_(addr, fn) GABI_FAST_NE_(addr, fn)

/* VERIFY with the name the harness shows: autoglue names an overloaded method by a typed cast,
 * VERIFY_NAMED(0x..., static_cast<void (C::*)(cXyz*)>(&C::set_mtx), "&C::set_mtx(cXyz*)") */
#define VERIFY_NAMED(addr, fn, name)                                                            \
    static gabi::Candidate GABI_CAT(wwhd_verify_, __LINE__)(addr, name,                        \
        &decltype(gabi::entry_of(fn))::template run<fn>, gabi::ret_of(fn)) GABI_NATIVE_REG_(addr, fn) GABI_FAST_NE_(addr, fn)

/* =====================================================================================
 * GameCube-style source (opt-in; tools/verify/autoglue.py). The visible source has only a
 * `/ * ADDR * /` comment above each function; autoglue inserts WWHD_GLUE_FUNC and VERIFY, turns
 * class-typed locals into gabi::Stack, and static tables / function-local statics into views of
 * their guest copies. Units written with WWHD_FUNC directly are not affected.
 * ===================================================================================== */
#ifdef GABI_GAME_BUILD
/* a WWHD_GLUE_FUNC body's scope. On a native entry (a nested call that native_enter_ok allowed) it makes the guest
 * state the body and its guest calls see the same as on the guest path (do_call + Entry::run):
 *  - the arguments in r3... / f1... as ArgPack puts them (decompiled bodies pass registers on implicitly: a call
 *    declared with fewer arguments than its callee takes hands over what the caller's entry registers still hold,
 *    which the verifier compared; JPABaseEmitter::initGpuState passes its heap in r4 this way);
 *  - r1 where do_call starts the callee (the caller's frame [sp, entry) stays intact);
 *  - on exit the result in r3 / r4 / f1 as store_ret puts it (the gen.py-rewritten returns hand it to ret()),
 *    after the body's locals are gone, as Entry::run does;
 *  - LR and pc as gmem_call sets them (decomp_native_lr), left so on exit as EntryScope does;
 * and keeps what the guest entry keeps for the caller (Activation::current, the automatic frame, the runtime's
 * running function). It counts Cpu::native_depth (the runtime's park guard). */
namespace gabi {
[[gnu::cold, gnu::noinline]] inline void native_ret_missing(u32 addr) {
    static bool once = false; /* no <cstdio> (see glue_fatal) */
    if (!once) { once = true; __builtin_printf("[decomp] native call %08X returned without its result (a return not rewritten)\n", addr); }
}
/* WWHD_DECOMP_POISON (g_decomp_exact bit 1): a register-exact native entry sets no guest registers; this makes the
 * argument registers (on entry) and the result registers (on exit) values no correct body reads, so a body or guest
 * callee that still reads one diverges visibly (test aid for the decl table's "only an indirect call may read it") */
[[gnu::cold, gnu::noinline]] inline void native_poison(Cpu* c, bool entry) {
    for (int i = 3; i <= (entry ? 10 : 4); i++) c->r[i] = 0xDEAD0000u | (u32)i;
    for (int i = 1; i <= (entry ? 13 : 1); i++) {
        const uint64_t b = 0x7FF8DEAD00000000ull | (uint64_t)i;
        memcpy(&c->f[i].ps0, &b, 8);
        memcpy(&c->f[i].ps1, &b, 8);
    }
}
#ifndef GABI_SLIM_SCOPE
struct NativeScope {
    u32 saved_ok;
    bool native;
    bool exact = false; /* a register-exact native entry: C++ arguments and result only (no ArgPack, no store_ret) */
    bool needs_ret;
    u8 ret_kind = 0; /* 0 none, 1 r3, 2 r3+r4, 3 f1 */
    u32 saved_current, entry_lr, saved_r1;
    u16 saved_running;
    u8 saved_depth;
    FrameCtx* saved_fc;
    Cpu* c;
    u32 ret_r3, ret_r4;
    double ret_f;
    u32 addr;
#ifdef GABI_SITE_LR
    ThreadState* ts;    /* this thread's block (the site marks below write through it) */
    uint64_t site_key;  /* this function's slot + 1 << 32 (site_take's caller check) */
#endif
    template <class... A> NativeScope(const NativeInfo& ni, uint64_t* lr_cache, bool needs_ret_, const A&... a) : needs_ret(needs_ret_) {
        ThreadState& t = GABI_TLS_;
#ifdef GABI_SITE_LR
        ts = &t;
        site_key = (uint64_t)(ni.slot + 1) << 32;
#endif
        saved_ok = t.caller_ok;
        t.caller_ok = (ni.flags >> 1) & 1;
        native = t.depth > 0;
        if (!native) return;
        c = t.cpu;
        addr = ni.addr;
        const u8 ex = (ni.flags & 8) ? g_decomp_exact : 0;
        exact = ex & 1;
        if (exact) {
            if (__builtin_expect(ex & 2, 0)) native_poison(c, true);
        } else if constexpr ((native_type_ok<A>() && ...)) {
            ArgPack p{c};
            (p.put(a), ...);
        }
        saved_current = t.current;
        saved_fc = t.frame_ctx;
        t.frame_ctx = nullptr; /* the callee's guest calls take do_call's plain path (no automatic frame of its own) */
#ifdef GABI_LEAN_ENTRY
        /* level 1, lean entry (the port's WWHD_DECOMP_LEAN_ENTRY): a register-exact native entry keeps r1 and LR as
         * they are; its guest calls (the edges) take do_call's plain path below r1 and their LR from gmem_call's
         * call-site answer for this function (decomp_running) */
        if (!exact) {
#endif
        saved_r1 = c->r[1];
        FrameCtx* fc = saved_fc;
        if (fc && fc->active && saved_r1 == fc->entry) c->r[1] = fc->sp;
        else if (!(fc && fc->tail && saved_r1 == fc->entry)) c->r[1] = saved_r1 - 16;
        /* LR and pc as gmem_call sets them for the guest call (before the running function changes); the exit
         * leaves this LR, as EntryScope does */
#ifdef GABI_SITE_LR
        if (const u32 lr = site_take(t, c, ni.addr)) { /* the caller's site mark: the bl's return address */
            c->pc = ni.addr;
            entry_lr = c->lr = lr;
        } else {
            entry_lr = c->lr = decomp_native_lr(c, ni.addr, lr_cache);
        }
#else
        entry_lr = c->lr = decomp_native_lr(c, ni.addr, lr_cache);
#endif
#ifdef GABI_LEAN_ENTRY
        }
#endif
        saved_running = c->decomp_running;
        c->decomp_running = (u16)(ni.slot + 1); /* its guest calls get its own call_lr answers */
        saved_depth = c->native_depth;          /* the runtime's park guard (saturating) */
        c->native_depth = (u8)(saved_depth + (saved_depth != 255));
    }
    /* site-exact LR: the port's tools/decomp_build/site_lr.py wraps a call of this body as `(GABI_SITE(T, RET), CALL)`:
     * the guest or native entry of T that CALL reaches takes RET as its LR (site_take; another target or caller does
     * not match, and a mark that is not taken is overwritten by the next one) */
#ifdef GABI_SITE_LR
    template <u32 T, u32 RET> void site() {
        ThreadState::SiteMark& m = ts->site[(T >> 2) & 7];
        m.key = site_key | T;
        m.ret = RET;
    }
#endif
    /* `return EXPR;` of a WWHD_GLUE_FUNC_R body (gen.py, only for scalar and pointer results): the parameter is
     * copy-initialised from EXPR exactly as the return value would be; keeps it for the exit and returns it */
    template <class R> R ret(R r) {
        static_assert(native_type_ok<R>() && !std::is_reference_v<R>, "gen.py rewrote the returns of a non-scalar result");
        if (__builtin_expect(native && !exact, 0)) {
            if constexpr (std::is_same_v<R, Pair32>) { ret_r3 = r.r3; ret_r4 = r.r4; ret_kind = 2; }
            else if constexpr (std::is_floating_point_v<R>) { ret_f = fpr_from(r); ret_kind = 3; }
            else if constexpr (std::is_pointer_v<R> || std::is_same_v<R, std::nullptr_t>) { ret_r3 = ea(r); ret_kind = 1; }
            else if constexpr (std::is_same_v<R, bool>) { ret_r3 = r ? 1 : 0; ret_kind = 1; }
            else if constexpr (std::is_signed_v<R>) { ret_r3 = (u32)(s32)r; ret_kind = 1; } /* as store_ret */
            else { ret_r3 = (u32)r; ret_kind = 1; }
        }
        return r;
    }
    ~NativeScope() {
        ThreadState& t = GABI_TLS_;
        if (native) {
            if (exact) {
                if (__builtin_expect(g_decomp_exact & 4, 0)) native_poison(c, false);
            } else {
                switch (ret_kind) { /* store_ret */
                case 1: c->r[3] = ret_r3; break;
                case 2: c->r[3] = ret_r3; c->r[4] = ret_r4; break;
                case 3: c->f[1].ps0 = c->f[1].ps1 = ret_f; break;
                default: if (needs_ret) native_ret_missing(addr);
                }
            }
            c->native_depth = saved_depth;
            c->decomp_running = saved_running;
#ifdef GABI_LEAN_ENTRY
            if (!exact) {
#endif
            c->lr = entry_lr;
            c->r[1] = saved_r1;
#ifdef GABI_LEAN_ENTRY
            }
#endif
            t.frame_ctx = saved_fc;
            t.current = saved_current;
        }
        t.caller_ok = saved_ok;
    }
    NativeScope(const NativeScope&) = delete;
};
#else
/* GABI_SLIM_SCOPE (the port's WWHD_DECOMP_SLIM_SCOPE, level 1 step 1b): the same scope with its fast path forced inline
 * into the body (one thread-local access per body: clang reuses the body's own block address; the block pointer is
 * kept for the exit) and the rare parts out of line: poisoning, a non-register-exact entry's ArgPack and r1 / LR, and
 * its result bookkeeping. Guest state, order of effects and results are those of the scope above. */
#if defined(GABI_NATIVE_PLACE) && !defined(GABI_LEAN_ENTRY)
#error "GABI_NATIVE_PLACE (level 1 step 2) builds on GABI_LEAN_ENTRY"
#endif
template <class... A> [[gnu::cold, gnu::noinline]] void native_args_slow(Cpu* c, const A&... a) {
    ArgPack p{c};
    (p.put(a), ...);
}
#ifdef GABI_NX_LEAN
/* GABI_NX_LEAN (the port's WWHD_DECOMP_FAST_NX_LEAN): a non-register-exact native entry's arguments put into the guest
 * registers inline, as native_args_slow's ArgPack puts them (the same values in r3..r10 / f1..; an argument beyond r10
 * is dropped as there, a native shape has none), without an ArgPack on the body's stack */
struct NxArgs {
    Cpu* c;
    int gi = 3, fi = 1;
    [[gnu::always_inline]] void put_int(u32 v) {
        if (gi <= 10) c->r[gi] = v;
        gi++;
    }
    template <class A> [[gnu::always_inline]] void put(const A& a) {
        if constexpr (std::is_floating_point_v<A>) {
            c->f[fi].ps0 = c->f[fi].ps1 = fpr_from(a);
            fi++;
        } else if constexpr (std::is_pointer_v<A> && std::is_function_v<std::remove_pointer_t<A>>) {
            put_int(is_guest((const void*)a) ? ea((const void*)a) : guest_fn((const void*)a));
        } else if constexpr (std::is_pointer_v<A> && std::is_same_v<std::remove_cv_t<std::remove_pointer_t<A>>, char>) {
            put_int(is_guest(a) ? ea(a) : guest_str(a));
        } else if constexpr (std::is_pointer_v<A>) {
            put_int(ea(a));
        } else if constexpr (std::is_same_v<A, std::nullptr_t>) {
            put_int(0);
        } else if constexpr (std::is_enum_v<A>) {
            put_int((u32)(s32)a);
        } else if constexpr (std::is_same_v<A, bool>) {
            put_int(a ? 1u : 0u);
        } else if constexpr (std::is_integral_v<A> && std::is_signed_v<A>) {
            put_int((u32)(s32)a);
        } else if constexpr (std::is_integral_v<A>) {
            put_int((u32)a);
        } else {
            static_assert(sizeof(A) == 0, "unsupported argument type");
        }
    }
};
#endif
struct NativeScope {
    u32 saved_ok;
    bool native;
    bool exact = false;
    bool needs_ret;
    u8 ret_kind = 0;
    u32 saved_current, entry_lr, saved_r1;
    u16 saved_running;
    u8 saved_depth;
    FrameCtx* saved_fc;
    Cpu* c;
    u32 ret_r3, ret_r4;
    double ret_f;
    u32 addr;
    ThreadState* ts; /* this thread's block (the exit and the site marks write through it) */
#ifdef GABI_LEAF
    bool leaf = false; /* a leaf body (bit 20): nothing kept, nothing restored but depth (ActivationT) */
#ifdef GABI_LEAF_CHECK
    [[gnu::noinline]] void leaf_poison(ThreadState& t) {
        Cpu* pc = t.cpu;
        saved_ok = t.caller_ok;
        saved_fc = t.frame_ctx;
        saved_current = t.current;
        saved_running = pc ? pc->decomp_running : 0;
        t.caller_ok = 0;                     /* a native call would go guest ... */
        t.frame_ctx = (FrameCtx*)(uintptr_t)1; /* ... where do_call_g / AutoFrame / Local / NativeScope trap */
        t.current = 0xFFFFFFFEu;
        if (pc) pc->decomp_running = 0xFFFF; /* the runtime traps on it (gmem_call, decomp_lcall, decomp_native_lr) */
    }
    [[gnu::noinline]] void leaf_unpoison(ThreadState& t) {
        if ((uintptr_t)t.frame_ctx != 1 || t.current != 0xFFFFFFFEu || t.caller_ok) __builtin_trap();
        if (t.cpu && t.cpu->decomp_running != 0xFFFF) __builtin_trap();
        t.caller_ok = saved_ok;
        t.frame_ctx = saved_fc;
        t.current = saved_current;
        if (t.cpu) t.cpu->decomp_running = saved_running;
    }
#endif
#endif
#ifdef GABI_NATIVE_PLACE
    bool placed = false; /* step 2: r1 placed as do_call places it (restored on exit) */
#endif
#ifdef GABI_SITE_LR
    uint64_t site_key;
#endif
    /* a non-register-exact entry: r1 where do_call starts the callee, LR and pc as gmem_call sets them */
    [[gnu::cold, gnu::noinline]] void enter_regs(const NativeInfo& ni, uint64_t* lr_cache) {
        saved_r1 = c->r[1];
        FrameCtx* fc = saved_fc;
        if (fc && fc->active && saved_r1 == fc->entry) c->r[1] = fc->sp;
        else if (!(fc && fc->tail && saved_r1 == fc->entry)) c->r[1] = saved_r1 - 16;
#ifdef GABI_SITE_LR
        if (const u32 lr = site_take(*ts, c, ni.addr)) {
            c->pc = ni.addr;
            entry_lr = c->lr = lr;
        } else {
            entry_lr = c->lr = decomp_native_lr(c, ni.addr, lr_cache);
        }
#else
        entry_lr = c->lr = decomp_native_lr(c, ni.addr, lr_cache);
#endif
    }
    /* a non-register-exact exit: store_ret, then LR and r1 back */
    [[gnu::cold, gnu::noinline]] void exit_regs() {
        switch (ret_kind) {
        case 1: c->r[3] = ret_r3; break;
        case 2: c->r[3] = ret_r3; c->r[4] = ret_r4; break;
        case 3: c->f[1].ps0 = c->f[1].ps1 = ret_f; break;
        default: if (needs_ret) native_ret_missing(addr);
        }
        c->lr = entry_lr;
        c->r[1] = saved_r1;
    }
#ifdef GABI_NX_LEAN
    /* GABI_NX_LEAN: exit_regs without LR (the entry set none) */
    [[gnu::always_inline]] void exit_regs_lean() {
        switch (ret_kind) {
        case 1: c->r[3] = ret_r3; break;
        case 2: c->r[3] = ret_r3; c->r[4] = ret_r4; break;
        case 3: c->f[1].ps0 = c->f[1].ps1 = ret_f; break;
        default: if (needs_ret) native_ret_missing(addr);
        }
        c->r[1] = saved_r1;
    }
#endif
    template <class... A>
#ifdef GABI_TP_CACHE
    [[gnu::always_inline]] NativeScope(ThreadState& t, const NativeInfo& ni, uint64_t* lr_cache, bool needs_ret_, const A&... a) : needs_ret(needs_ret_)
#else
    [[gnu::always_inline]] NativeScope(const NativeInfo& ni, uint64_t* lr_cache, bool needs_ret_, const A&... a) : needs_ret(needs_ret_)
#endif
    { /* (the brace outside the #if: autoglue balances braces in the text) */
#ifndef GABI_TP_CACHE
        ThreadState& t = GABI_TLS_;
#endif
        ts = &t;
#ifdef GABI_SITE_LR
        site_key = (uint64_t)(ni.slot + 1) << 32;
#endif
#ifdef GABI_LEAF
        /* the port's WWHD_DECOMP_FAST_LEAF: a leaf (NativeInfo bit 20, native_rules.py --leaf: register-exact, its original
         * makes no call of any kind, its body no guest call, no native call of a non-leaf, no Local / AutoFrame / Cpu
         * access / string literal) keeps none of the state only calls, Locals, frames and string literals read: caller_ok,
         * the running function (decomp_running), the automatic frame (frame_ctx) and Activation::current stay the
         * caller's; ActivationT still counts depth. g_decomp_leaf_off (WWHD_DECOMP_FAST_LEAF=0): the full scope.
         * GABI_LEAF_CHECK (diagnostics): instead of keeping them, they are poisoned for the body (leaf_poison) and any
         * call, frame or Local use traps */
        if ((ni.flags >> 20) & 1 && !g_decomp_leaf_off) {
            leaf = true;
            native = false;
#ifdef GABI_LEAF_CHECK
            leaf_poison(t);
#endif
            return;
        }
#endif
#ifdef GABI_LEAF_CHECK
        if ((uintptr_t)t.frame_ctx == 1) __builtin_trap(); /* a body entered from a leaf */
#endif
        saved_ok = t.caller_ok;
#ifdef GABI_A3_FN
        /* the port's WWHD_DECOMP_FAST_A3_FN: NativeInfo bit 19, caller=ok only by the per-function A3 rule (native_rules.py
         * --a3-fn); g_decomp_a3_off (WWHD_DECOMP_FAST_A3_FN=0) makes such a body's calls guest calls again. Constant for
         * every other body. */
        t.caller_ok = ((ni.flags >> 1) & 1) & ~(u32)((ni.flags >> 19) & g_decomp_a3_off);
#else
        t.caller_ok = (ni.flags >> 1) & 1;
#endif
#ifdef GABI_NOCHECK
        /* GABI_NOCHECK: a caller that may pass pointers outside the guest window (NativeInfo bit 21, native_rules.py
         * --unbased) marks its calls (bit 1) so their callees keep the per-argument window check (native_enter_nc) */
        t.caller_ok |= t.caller_ok & ((ni.flags >> 20) & 2);
#endif
        native = t.depth > 0;
        if (!native) return;
        c = t.cpu;
        addr = ni.addr;
#ifdef GABI_FAST
        exact = (ni.flags & 8) != 0; /* fast mode: register-exact always, no poison test aid */
        if (exact) {
        } else
#else
        const u8 ex = (ni.flags & 8) ? g_decomp_exact : 0;
        exact = ex & 1;
        if (exact) {
            if (__builtin_expect(ex & 2, 0)) native_poison(c, true);
        } else
#endif
                if constexpr ((native_type_ok<A>() && ...)) {
#ifdef GABI_NX_LEAN
            NxArgs p{c};
            (p.put(a), ...);
#else
            native_args_slow(c, a...);
#endif
        }
        saved_current = t.current;
        saved_fc = t.frame_ctx;
        t.frame_ctx = nullptr;
#ifdef GABI_NX_LEAN
        /* GABI_NX_LEAN: a non-register-exact entry keeps the argument registers (above), r1 where do_call starts the
         * callee and the result registers (exit_regs_lean), but sets no LR / pc (decomp_native_lr), as a
         * register-exact entry does under GABI_LEAN_ENTRY; inline (the entry kind is the body's constant) */
        if (!exact) {
            saved_r1 = c->r[1];
            const FrameCtx* fc = saved_fc;
            if (fc && fc->active && saved_r1 == fc->entry) c->r[1] = fc->sp;
            else if (!(fc && fc->tail && saved_r1 == fc->entry)) c->r[1] = saved_r1 - 16;
        }
#else
#ifdef GABI_LEAN_ENTRY
        if (!exact)
#endif
            enter_regs(ni, lr_cache);
#endif
#ifdef GABI_NATIVE_PLACE
        /* level 1 step 2 (the port's WWHD_DECOMP_NATIVE_AUTOFRAME / _LOCAL / _TAILPTR): a register-exact entry places r1
         * as do_call does when the callee keeps guest stack objects (NativeInfo bit 18: AutoFrame, Local, tail_ptr) or
         * the caller's automatic frame is active (its frame stays below the callee's frames, Locals and guest calls) */
        else if (const FrameCtx* fc = saved_fc; (ni.flags & (1u << 18)) || (fc && fc->active && c->r[1] == fc->entry)) {
            placed = true;
            saved_r1 = c->r[1];
            if (fc && fc->active && saved_r1 == fc->entry) c->r[1] = fc->sp;
            else if (!(fc && fc->tail && saved_r1 == fc->entry)) c->r[1] = saved_r1 - 16;
        }
#endif
        saved_running = c->decomp_running;
        c->decomp_running = (u16)(ni.slot + 1);
#ifdef GABI_NSTATS
        if (g_decomp_nstats) __atomic_fetch_add(&g_decomp_nstats[ni.slot], 1, __ATOMIC_RELAXED); /* diagnostics (below) */
#endif
#ifndef GABI_FAST /* fast mode: no park guard */
        saved_depth = c->native_depth;
        c->native_depth = (u8)(saved_depth + (saved_depth != 255));
#endif
    }
#ifdef GABI_SITE_LR
    template <u32 T, u32 RET> void site() {
        ThreadState::SiteMark& m = ts->site[(T >> 2) & 7];
        m.key = site_key | T;
        m.ret = RET;
    }
#endif
    template <class R> [[gnu::always_inline]] R ret(R r) {
        static_assert(native_type_ok<R>() && !std::is_reference_v<R>, "gen.py rewrote the returns of a non-scalar result");
        if (__builtin_expect(native && !exact, 0)) {
            if constexpr (std::is_same_v<R, Pair32>) { ret_r3 = r.r3; ret_r4 = r.r4; ret_kind = 2; }
            else if constexpr (std::is_floating_point_v<R>) { ret_f = fpr_from(r); ret_kind = 3; }
            else if constexpr (std::is_pointer_v<R> || std::is_same_v<R, std::nullptr_t>) { ret_r3 = ea(r); ret_kind = 1; }
            else if constexpr (std::is_same_v<R, bool>) { ret_r3 = r ? 1 : 0; ret_kind = 1; }
            else if constexpr (std::is_signed_v<R>) { ret_r3 = (u32)(s32)r; ret_kind = 1; }
            else { ret_r3 = (u32)r; ret_kind = 1; }
        }
        return r;
    }
    [[gnu::always_inline]] ~NativeScope() {
        ThreadState& t = *ts;
#ifdef GABI_LEAF
        if (leaf) {
#ifdef GABI_LEAF_CHECK
            leaf_unpoison(t);
#endif
            return;
        }
#endif
        if (native) {
            if (exact) {
#ifndef GABI_FAST
                if (__builtin_expect(g_decomp_exact & 4, 0)) native_poison(c, false);
#endif
#ifndef GABI_LEAN_ENTRY
                c->lr = entry_lr;
                c->r[1] = saved_r1;
#endif
#ifdef GABI_NATIVE_PLACE
                if (placed) c->r[1] = saved_r1;
#endif
            } else {
#ifdef GABI_NX_LEAN
                exit_regs_lean();
#else
                exit_regs();
#endif
            }
#ifndef GABI_FAST
            c->native_depth = saved_depth;
#endif
            c->decomp_running = saved_running;
            t.frame_ctx = saved_fc;
            t.current = saved_current;
        }
        t.caller_ok = saved_ok;
    }
    NativeScope(const NativeScope&) = delete;
};
#endif
}  /* namespace gabi */
#endif

#ifndef GABI_GAME_BUILD
#define WWHD_GLUE_FUNC(addr, R, ...)                                                       \
    if (gabi::Activation::nested()) return gabi::call<R>(addr __VA_OPT__(, ) __VA_ARGS__); \
    gabi::Activation wwhd_activation_(addr)
#else
/* The game build: native calls inside a unit (the port's native-calls design, form N). A nested call (a plain C++
 * call from another decompiled function of this unit) runs the body as an ordinary C++ call instead of a guest call
 * when all of these hold: the port's rules allow this callee and the running caller (decomp/native.txt, compiled
 * into the unit as gabi::native_info, see the port's tools/decomp_build/native_rules.py), the runtime still runs
 * the callee decompiled and unhooked (g_decomp_native_ok[slot]), and the arguments and result round-trip exactly
 * through the guest ABI (native_args_ok: guest-window pointers, no host string literals or functions passed by
 * name, no 64-bit integers). Otherwise the guest call as before. NativeScope makes the guest registers and r1 what
 * the guest path gives (arguments in, result out) and keeps what the guest entry would keep for the caller. */
#ifdef GABI_GUEST_LCALL
#define GABI_GLUE_GUEST_CALL_(addr, R, ...) ::gabi::guest_lcall<addr, R>(__VA_ARGS__)
#else
#define GABI_GLUE_GUEST_CALL_(addr, R, ...) gabi::call<R>(addr __VA_OPT__(, ) __VA_ARGS__)
#endif
#ifdef GABI_COLD_FALLBACK
/* GABI_COLD_FALLBACK (the port's WWHD_DECOMP_FAST_COLD_FALLBACK): a body's guest call (a nested call native_enter_ok
 * refused) in a cold function of its own, one per function: the body keeps only the check and a call. The arguments
 * pass by value as the guest call takes them (call / guest_lcall deduce the same types from them). */
namespace gabi {
template <u32 ADDR, class R, class... A> [[gnu::cold, gnu::noinline]] R glue_guest_call(A... a) {
    return GABI_GLUE_GUEST_CALL_(ADDR, R, a...);
}
}  /* namespace gabi */
#define GABI_GLUE_FALLBACK_(addr, R, ...) ::gabi::glue_guest_call<addr, R>(__VA_ARGS__)
#else
#define GABI_GLUE_FALLBACK_(addr, R, ...) GABI_GLUE_GUEST_CALL_(addr, R __VA_OPT__(, ) __VA_ARGS__)
#endif
#ifdef GABI_TP_CACHE
#ifdef GABI_LEAF_BARE
#define GABI_ACTIVATION_T_(addr) \
    std::conditional_t<wwhd_bare_, ::gabi::BareActivation, ::gabi::ActivationT> wwhd_activation_(wwhd_ts_, addr, wwhd_native_scope_.leaf)
#elif defined(GABI_LEAF)
#define GABI_ACTIVATION_T_(addr) ::gabi::ActivationT wwhd_activation_(wwhd_ts_, addr, wwhd_native_scope_.leaf)
#else
#define GABI_ACTIVATION_T_(addr) ::gabi::ActivationT wwhd_activation_(wwhd_ts_, addr)
#endif
/* a nested call (depth > 0) the checks refuse: the body makes the guest call instead (GABI_CHECKCOUNT: counted) */
#ifdef GABI_CHECKCOUNT
#define GABI_GLUE_REFUSED_(R, RET_OK, ...) \
    !::gabi::native_enter_cc<R, RET_OK>(wwhd_ts_, wwhd_native_info_ __VA_OPT__(, ) __VA_ARGS__)
#elif defined(GABI_NOCHECK)
#define GABI_GLUE_REFUSED_(R, RET_OK, ...) \
    wwhd_ts_.depth > 0 && !::gabi::native_enter_nc<R, RET_OK>(wwhd_ts_, wwhd_native_info_ __VA_OPT__(, ) __VA_ARGS__)
#else
#define GABI_GLUE_REFUSED_(R, RET_OK, ...) \
    wwhd_ts_.depth > 0 && !::gabi::native_enter_ok_t<R, RET_OK>(wwhd_ts_, wwhd_native_info_ __VA_OPT__(, ) __VA_ARGS__)
#endif
/* GABI_TP_CACHE: the body looks its block up once (wwhd_ts_) and hands it to the check, the scope and Activation */
#ifdef GABI_LEAF_BARE
/* GABI_LEAF_BARE (the port's WWHD_DECOMP_FAST_LEAF_BARE): a bare leaf (glue_bare_of) keeps none of the glue: no block
 * lookup, no refusal check, no scope and no depth count. Nothing of it can matter for such a body:
 *  - a leaf keeps no call state anyway (GABI_LEAF: its scope saves and restores nothing, Activation::current stays the
 *    caller's) and makes no call that could read the depth or a scope (its calls reach leaves only: native_rules.py
 *    --leaf; a leaf of another unit is entered by lcall, which checks caller_ok itself, and its own check passes then);
 *  - the refusal check (native_enter_nc) can only refuse a nested call from a caller=guest or unbased caller, or with a
 *    char / void / function pointer argument outside the guest window. Its unit has no caller=guest and no unbased
 *    function (bit 22), so no caller in its unit is refused; a caller in another unit enters it through its native
 *    entry (lcall / hcall / call_ptr / gabi::call after their own caller_ok check), whose pointer arguments are rebuilt
 *    from guest addresses (always inside the window); a body with a char / void / function pointer parameter keeps
 *    the glue.
 * The diagnostics run-time switches WWHD_DECOMP_FAST_LEAF=0 and WWHD_DECOMP_FAST_NOCHECK=0 do not reach a bare leaf.
 * Every other body: the glue below as before (its constant wwhd_bare_ is false) */
#define GABI_GLUE_BARE_(addr, R, RET_OK, ...) \
    constexpr bool wwhd_bare_ = decltype(::gabi::glue_bare_of<::gabi::native_info(addr).flags, R, RET_OK>(__VA_ARGS__))::value;
#define GABI_GLUE_SCOPE_T_ std::conditional_t<wwhd_bare_, ::gabi::BareScope, ::gabi::NativeScope>
#define GABI_GLUE_IF_NOT_BARE_ !wwhd_bare_ &&
#else
#define GABI_GLUE_BARE_(addr, R, RET_OK, ...)
#define GABI_GLUE_SCOPE_T_ ::gabi::NativeScope
#define GABI_GLUE_IF_NOT_BARE_
#endif
#define WWHD_GLUE_FUNC_IMPL_(addr, R, RET_OK, ...)                                                       \
    using wwhd_ret_t_ [[maybe_unused]] = R;                                                              \
    static constexpr ::gabi::NativeInfo wwhd_native_info_ = ::gabi::native_info(addr);                   \
    GABI_GLUE_BARE_(addr, R, RET_OK __VA_OPT__(, ) __VA_ARGS__)                                          \
    ::gabi::ThreadState& wwhd_ts_ = GABI_BODY_TS_;                                                       \
    if (GABI_GLUE_IF_NOT_BARE_ GABI_GLUE_REFUSED_(R, RET_OK __VA_OPT__(, ) __VA_ARGS__))                 \
        return GABI_GLUE_FALLBACK_(addr, R __VA_OPT__(, ) __VA_ARGS__);                                 \
    static constinit uint64_t wwhd_lr_cache_ = 0;                                                        \
    GABI_GLUE_SCOPE_T_ wwhd_native_scope_(wwhd_ts_, wwhd_native_info_, &wwhd_lr_cache_, !std::is_void_v<R> __VA_OPT__(, ) __VA_ARGS__); \
    GABI_ACTIVATION_T_(addr)
#else
#define WWHD_GLUE_FUNC_IMPL_(addr, R, RET_OK, ...)                                                       \
    using wwhd_ret_t_ [[maybe_unused]] = R;                                                              \
    static constexpr ::gabi::NativeInfo wwhd_native_info_ = ::gabi::native_info(addr);                   \
    if (::gabi::Activation::nested() &&                                                                  \
        !::gabi::native_enter_ok<R, RET_OK>(wwhd_native_info_ __VA_OPT__(, ) __VA_ARGS__))               \
        return GABI_GLUE_FALLBACK_(addr, R __VA_OPT__(, ) __VA_ARGS__);                                 \
    static constinit uint64_t wwhd_lr_cache_ = 0;                                                        \
    ::gabi::NativeScope wwhd_native_scope_(wwhd_native_info_, &wwhd_lr_cache_, !std::is_void_v<R> __VA_OPT__(, ) __VA_ARGS__); \
    gabi::Activation wwhd_activation_(addr)
#endif
#define GABI_SITE(T, RET) wwhd_native_scope_.site<T, RET>()
/* WWHD_GLUE_FUNC: a body whose returns were not rewritten (only a void one may go native); WWHD_GLUE_FUNC_R: gen.py
 * rewrote every `return EXPR;` of the body to `return wwhd_native_scope_.ret<wwhd_ret_t_>(EXPR);` */
#define WWHD_GLUE_FUNC(addr, R, ...) WWHD_GLUE_FUNC_IMPL_(addr, R, std::is_void_v<R> __VA_OPT__(, ) __VA_ARGS__)
#define WWHD_GLUE_FUNC_R(addr, R, ...) WWHD_GLUE_FUNC_IMPL_(addr, R, true __VA_OPT__(, ) __VA_ARGS__)
#endif

namespace gabi {

/* Construction hooks for guest objects (found by ADL): a class-typed local `T x(args);` becomes
 * a guest stack slot initialised by gabi_construct(x, args...) and finalised by gabi_destroy(x)
 * at the end of its scope (GHS: the inline constructor/destructor). Defaults: no arguments =
 * uninitialised storage (the scratch stack's bytes), one argument of the same type = copy. */
template <class T> inline void gabi_construct(T&) {}
template <class T> inline void gabi_destroy(T&) {}
/* a guarded function-local static of scalar type, `static f32 s(0.7f);` (autoglue local_static) */
template <class T, class V, class = std::enable_if_t<std::is_arithmetic_v<V>>> inline void gabi_construct(be<T>& o, V v) { o = (T)v; }
template <class T, class... A> inline void construct_dispatch(T& o, A&&... a) { gabi_construct(o, static_cast<A&&>(a)...); }
template <class T> inline void destroy_dispatch(T& o) { gabi_destroy(o); }

/* A value returned through a hidden result pointer (GHS: `T f(...)` is `void f(T* result, ...)`):
 * `T x = f(...)` / `x = f(...)` construct straight into x, as GHS passes x's address. */
template <class T, class F> struct Into {
    F f;
};
template <class T, class F> inline Into<T, F> into(F f) { return Into<T, F>{f}; }
template <class T, class F> inline void gabi_construct(T& o, const Into<T, F>& p) { p.f(&o); }

template <class T> struct Stack : Local<T> {
    template <class... A> explicit Stack(A&&... a) { construct_dispatch(*this->get(), static_cast<A&&>(a)...); }
    ~Stack() { destroy_dispatch(*this->get()); }
};

/* A local of a GameCube-style declaration: class types live on the guest stack, other types
 * (enums) on the host. */
template <class T, bool = std::is_class_v<T>> struct LocalOf : Stack<T> {
    using Stack<T>::Stack;
    T& ref() { return *this->get(); }
};
template <class T> struct LocalOf<T, false> {
    T v;
    LocalOf() = default;
    template <class A> explicit LocalOf(A&& a) : v(static_cast<A&&>(a)) {}
    T& ref() { return v; }
};

/* A guest object at a fixed address seen as a C++ reference (statics, tables). */
template <class T> inline T& object_at(u32 addr) { return *at<T>(addr); }

/* Function-local static with a GHS guard word: initialised on first use. */
template <class T, class F> inline T& local_static(u32 addr, u32 guard, F init) {
    if (!load<u32>(guard)) {
        init(*at<T>(addr));
        store<u32>(guard, 1);
    }
    return *at<T>(addr);
}

/* GHS pointer to member function in guest memory (8 bytes): this adjustment, virtual index
 * (< 0: not virtual), then the function's address, or (virtual) the vtable pointer's offset in
 * the low half. `(obj->*tbl[i].fn)(args...)` dispatches like the GHS runtime. */
template <class F> struct ptmf {
    /* 0x0 */ be<s16> d;
    /* 0x2 */ be<s16> i;
    /* 0x4 */ be<u32> f;
};
template <class C, class R, class... A> struct BoundPtmf {
    C* self;
    const ptmf<R (C::*)(A...)>* m;
    R operator()(A... a) const {
        u32 e = ea(m);
        u32 adjusted = ea(self) + (s32)load<s16>(e);
        s16 index = load<s16>(e + 2);
        u32 target;
        if (index < 0) {
            target = load<u32>(e + 4);
        } else {
            u32 vt = load<u32>(adjusted + (s32)load<s16>(e + 6));
            target = load<u32>(vt + (s32)index * 8 + 4);
        }
        return call_ptr<R>(target, at<C>(adjusted), a...);
    }
};
template <class C, class D, class R, class... A>
inline BoundPtmf<C, R, A...> operator->*(D* self, const ptmf<R (C::*)(A...)>& m) {
    return {static_cast<C*>(self), &m};
}

}  // namespace gabi

#ifdef GABI_FAST_STR
/* fast mode: the source's guest_str(...) / guest_addr(...) calls, each with its own cache entry (gabi::guest_str_site) */
#define guest_str(s) guest_str_site([] {}, (s))
#define guest_addr(s) guest_addr_site([] {}, (s))
#endif

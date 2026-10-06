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
#include <tuple>
#include <type_traits>

#include "ppc.h"
#include "native_frames.h"

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

extern "C" {
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
void gmem_call_top(uint32_t top);
/* kind: 0 direct, 1 through a function pointer; nint/nflt: argument registers set */
void gmem_call(Cpu* c, uint32_t target, int kind, int nint, int nflt);
}

namespace gabi {

extern thread_local Cpu* cpu;

/* ---- addresses ---- */
inline u32 ea(const void* p) { return p ? (u32)((const uint8_t*)p - PPC_MEM_BASE) : 0u; }
template <class T> inline T* at(u32 a) { return a ? (T*)(PPC_MEM_BASE + a) : nullptr; }

/* A float loaded into an FPR (lfs) is converted to double; on the host that conversion quiets a
 * signalling NaN, which the recompiled code then stores back. Model it for every f32 load. */
inline f32 f32_from_bits(u32 v) {
    if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x007FFFFFu) && !(v & 0x00400000u)) v |= 0x00400000u;
    f32 r;
    memcpy(&r, &v, 4);
    return r;
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
};
inline thread_local FrameCtx* frame_ctx = nullptr;
/* WWHD_NATIVE_LOCALS=1: place gabi::Locals inside the original's frame (upward from its first
 * address-taken object). Off by default (2026-10-05): candidates are written against the
 * established placement (entry - size) and several emulate the original's layout around it, so a
 * moved Local changed what they pass (d_a_obj_mknjd, d_a_npc_md_event, d_a_st, d_a_beam, ...). */
inline bool native_locals() {
    static const bool on = [] { const char* e = getenv("WWHD_NATIVE_LOCALS"); return e && *e == '1'; }();
    return on;
}

/* ---- big-endian guest field ---- */
template <class T> struct be {
    uint8_t _raw[sizeof(T)];
    u32 addr() const { return ea(this); }
    T get() const { return load<T>(addr()); }
    void set(T v) { store<T>(addr(), v); }
    operator T() const { return get(); }
    be& operator=(T v) { set(v); return *this; }
    be& operator=(const be& o) { set(o.get()); return *this; }
    be() = default;
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
            c->f[fi].ps0 = c->f[fi].ps1 = (double)a;
            fi++;
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
    else if constexpr (std::is_floating_point_v<R>) return (R)c->f[1].ps0;
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

template <class R = void, class... A> inline R call(u32 addr, A... a) {
    Cpu* c = cpu;
    ArgPack p{c};
    (p.put(a), ...);
    do_call(c, p, addr, 0);
    return result<R>(c);
}
template <class R = void, class... A> inline R call_ptr(u32 fn, A... a) {
    Cpu* c = cpu;
    ArgPack p{c};
    (p.put(a), ...);
    c->pc = fn;
    do_call(c, p, fn, 1);
    return result<R>(c);
}

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
template <class R = void, class... A> inline R tail_ptr(u32 fn, A... a) {
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

/* ---- guest stack temporaries (for locals whose address is passed to guest code) ---- */
template <class T> struct Local {
    static constexpr u32 kSize = (sizeof(T) + 15) & ~15u;
    u32 a;
    u32 prev_bump = 0;
    bool in_frame = false;
    /* a fresh stack slot: zeroed, as the harness's scratch stack starts out (GHS gives each
     * temporary its own frame slot; reused host-side slots would otherwise carry stale bytes
     * into the uninitialised parts of objects passed to callees) */
    Local() {
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
    explicit FrameLocal(u32 off) {
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
    ~FrameLocal() { if (!in_frame) cpu->r[1] = prev_r1; }
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
/* PowerPC NaN propagation for single-precision add/sub/mul: when an
 * operand is a NaN the result is the first NaN operand in the order frA, frB (fadds/fsubs) or frA, frC
 * (fmuls), quieted — host compilers may swap commutative operands, so write the operands in the
 * original's frA/frB order and use these where NaN payloads reach memory */
inline f32 ppc_qnan(f32 x) { u32 b; memcpy(&b, &x, 4); b |= 0x00400000u; memcpy(&x, &b, 4); return x; }
inline f32 fadds_ppc(f32 a, f32 b) { if (a != a) return ppc_qnan(a); if (b != b) return ppc_qnan(b); return (f32)((f64)a + (f64)b); }
inline f32 fsubs_ppc(f32 a, f32 b) { if (a != a) return ppc_qnan(a); if (b != b) return ppc_qnan(b); return (f32)((f64)a - (f64)b); }
inline f32 fmuls_ppc(f32 a, f32 c) { if (a != a) return ppc_qnan(a); if (c != c) return ppc_qnan(c); return (f32)((f64)a * (f64)c); }
inline f32 fmadds(f32 a, f32 c, f32 b) { return (f32)((f64)a * (f64)c + (f64)b); }   /* a*c+b */
inline f32 fmsubs(f32 a, f32 c, f32 b) { return (f32)((f64)a * (f64)c - (f64)b); }   /* a*c-b */
inline f32 fnmadds(f32 a, f32 c, f32 b) { return (f32)(-((f64)a * (f64)c + (f64)b)); }
inline f32 fnmsubs(f32 a, f32 c, f32 b) { return (f32)(-((f64)a * (f64)c - (f64)b)); } /* b-a*c */
inline f64 fmadd(f64 a, f64 c, f64 b) { return std::fma(a, c, b); }
inline f64 fmsub(f64 a, f64 c, f64 b) { return std::fma(a, c, -b); }
inline f64 fnmsub(f64 a, f64 c, f64 b) { return -std::fma(a, c, -b); }
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

template <class T> struct ArgFrom {
    static T get(Cpu* c, int& gi, int& fi) {
        if constexpr (std::is_floating_point_v<T>) return (T)c->f[fi++].ps0;
        else if constexpr (std::is_pointer_v<T>) return at<std::remove_pointer_t<T>>(c->r[gi++]);
        else if constexpr (std::is_same_v<T, bool>) return (c->r[gi++] & 0xFF) != 0;
        else return (T)c->r[gi++];
    }
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
    static inline thread_local int depth = 0;
    Activation() { depth++; }
    ~Activation() { depth--; }
    static bool nested() { return depth > 0; }
};

/* WWHD_FUNC's automatic native frame (see FrameCtx) */
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
            case GABI_FI_STFS0: { f32 x = (f32)c->f[it.reg].ps0; u32 v; memcpy(&v, &x, 4); gmem_st32(ea, v); break; }
            case GABI_FI_STFS1: { f32 x = (f32)c->f[it.reg].ps1; u32 v; memcpy(&v, &x, 4); gmem_st32(ea, v); break; }
            case GABI_FI_PSQ: {
                f32 x = (f32)c->f[it.reg].ps0, y = (f32)c->f[it.reg].ps1; u32 v, w;
                memcpy(&v, &x, 4); memcpy(&w, &y, 4); gmem_st32(ea, v); gmem_st32(ea + 4, w); break;
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
                if (it.kind == GABI_FI_STFS0 || it.kind == GABI_FI_STFS1) c->f[it.reg].ps0 = c->f[it.reg].ps1 = (f64)f32_from_bits(gmem_ld32(ea));
                else if (it.kind == GABI_FI_PSQ) { c->f[it.reg].ps0 = (f64)f32_from_bits(gmem_ld32(ea)); c->f[it.reg].ps1 = (f64)f32_from_bits(gmem_ld32(ea + 4)); }
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

template <class R> inline void store_ret(Cpu* c, R r) {
    if constexpr (std::is_same_v<R, Pair32>) { c->r[3] = r.r3; c->r[4] = r.r4; }
    else if constexpr (std::is_floating_point_v<R>) c->f[1].ps0 = c->f[1].ps1 = (f64)r;
    else if constexpr (std::is_pointer_v<R>) c->r[3] = ea(r);
    else if constexpr (std::is_same_v<R, bool>) c->r[3] = r ? 1 : 0;
    else if constexpr (std::is_signed_v<R>) c->r[3] = (u32)(s32)r;
    else c->r[3] = (u32)r;
}

/* An entry from guest code may be re-entrant (candidate A -> guest code -> candidate B): save and
 * restore the caller's cpu, activation depth and LR so A's later natural C++ calls still go through
 * the guest (game-test request, 2026-10-04). Results are unchanged for a single, non-nested entry. */
struct EntryScope {
    Cpu* saved_cpu;
    int saved_depth;
    u32 saved_lr;
    Cpu* c;
    explicit EntryScope(Cpu* c_) : saved_cpu(cpu), saved_depth(Activation::depth), saved_lr(c_->lr), c(c_) {
        cpu = c_;
        Activation::depth = 0;
    }
    ~EntryScope() {
        c->lr = saved_lr;
        Activation::depth = saved_depth;
        cpu = saved_cpu;
    }
};

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
template <class R, class... A> constexpr auto entry_of(R (*)(A...)) { return Entry<R, A...>{}; }
template <class C, class R, class... A> constexpr auto entry_of(R (C::*)(A...)) { return MEntry<C, R, A...>{}; }
template <class R, class... A> constexpr RetKind ret_of(R (*)(A...)) { return ret_kind<R>(); }
template <class C, class R, class... A> constexpr RetKind ret_of(R (C::*)(A...)) { return ret_kind<R>(); }

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
#define VERIFY(addr, fn)                                                                        \
    static gabi::Candidate GABI_CAT(wwhd_verify_, __LINE__)(addr, #fn,                         \
        &decltype(gabi::entry_of(fn))::template run<fn>, gabi::ret_of(fn))

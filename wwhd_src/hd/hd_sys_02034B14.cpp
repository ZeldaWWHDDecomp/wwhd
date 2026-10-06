/* hd_sys_02034B14: the HD ProcUI game framework (no GameCube source).
 *
 * TU 02034B14..02035443 (__sinit 02035398 + companions): a sead::GameFramework-derived framework
 * (0x390, base 0274B1F4, vtable 0x10004E88 at +0x24): ProcUI state +0x380 (ProcUIProcessMessages:
 * 2 = background, 3 = exit), "TV/DRC re-enabled" flag +0x38C, run mode +0x28; its createControllers
 * (offset list of controller objects in the controller manager 101F8AE8), the ProcUI init with the
 * save callback, the frame loop (wait on OSGetTime-based ticks through 02760C3C) and the shutdown
 * (_Exit). Also a sead Delegate invoker and a heap destructor (same shape as in 0203400C).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_02034B14 {

static constexpr u32 OSSavesDone_ReadyToRelease = 0xC0009F68;
static constexpr u32 ProcUIInit = 0xC0007868, ProcUIDrawDoneRelease = 0xC0007850;
static constexpr u32 ProcUIProcessMessages = 0xC0007880, ProcUIShutdown = 0xC00078B8;
static constexpr u32 GX2SetTVEnable = 0xC00069D0, GX2SetDRCEnable = 0xC0006808, GX2SetContextState = 0xC00067E0;
static constexpr u32 bossIsInitialized = 0xC00053C0, bossFinalize = 0xC0005108, actFinalize = 0xC00044E8;
static constexpr u32 OSBlockThreadsOnExit = 0xC0009998, _Exit = 0xC000A468;

/* 02034B14: sead Delegate invoke (tail call with the adjusted object) */
static u32 Delegate_invoke(u32 self, u32 a) {
    WWHD_FUNC(0x02034B14, u32, self, a);
    u32 obj = ld(self + 4);
    if (obj == 0) return self;
    s32 vi = (s16)lhz(self + 0xA);
    if (vi == 0) return self;
    u32 adj = obj + (u32)(s32)(s16)lhz(self + 8);
    if (vi < 0) return gabi::call_ptr<u32>(ld(self + 0xC), adj, a);
    u32 vt = ld(adj + (u32)(s32)(s16)lhz(self + 0xE));
    return gabi::call_ptr<u32>(ld(vt + (u32)vi * 8 + 4), adj, a);
}
VERIFY(0x02034B14, Delegate_invoke);

/* 02034B68: heap destructor */
static void Heap_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02034B68, void, self, flags);
    if (self == 0) return;
    u8 b = lbz(self + 0x38);
    st(self + 0xC, 0x10004CB0);
    if (b != 0) {
        u32 p = ld(self + 0x1C);
        if (p != 0) {
            st(self + 0x1C, 0);
            gabi::call(0x0273B310, self + 0x10);
            st(p + 8, ld(p + 8) - 1);
        }
        stb(self + 0x38, 0);
    }
    gabi::call(0x02752BEC, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02034B68, Heap_dt);

/* 02034C10: framework constructor */
static u32 Framework_ct(u32 self, u32 a) {
    WWHD_FUNC(0x02034C10, u32, self, a);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x390);
        if (t == 0) return 0;
    }
    gabi::call(0x0274B1F4, t, a);
    st(t + 0x380, 0);
    st(t + 0x24, 0x10004E88);
    u32 p = t + 0x384;
    bool ok = true;
    if (p == 0) {
        p = op_new(8);
        if (p == 0) ok = false;
    }
    if (ok) {
        st(p + 4, 0x10004DFC);
        st(p + 0, 0x10004DF8);
    }
    stb(t + 0x38C, 0);
    return t;
}
VERIFY(0x02034C10, Framework_ct);

/* append V to the controller manager's object array {count +0xDC, capacity +0xE0, data +0xE4} */
static inline void push_obj(u32 m, u32 v) {
    s32 n = (s32)ld(m + 0xDC);
    if (n < (s32)ld(m + 0xE0)) {
        st(ld(m + 0xE4) + (u32)n * 4, v);
        st(m + 0xDC, ld(m + 0xDC) + 1);
    }
}
/* link node NODE into the offset list at L (count +8) */
static inline void link_tail(u32 l, u32 node) {
    gabi::call(0x0273B2F0, l, node + ld(l + 0xC));
    st(l + 8, ld(l + 8) + 1);
}

/* 02034CC8: createControllers: the task-creation argument (a stack copy) and the controllers */
static void Framework_createControllers(u32 self, u32 a) {
    WWHD_FUNC(0x02034CC8, void, self, a);
    u32 entry = gabi::cpu->r[1];
    u32 f = entry - 0x128; /* the original's frame */
    gabi::cpu->r[1] = f;
    st(f + 8, 2);
    st(f + 0xC, 0x020354BC);
    gabi::call(0x02748CFC, f + 0x10, f + 8);
    u32 p = op_new(0xC);
    if (p != 0) {
        st(p + 8, 0);
        st(p + 0, 0x10004E54);
    }
    st(p + 4, 7);
    st(f + 0x6C, a);
    st(f + 0x70, p);
    for (u32 i = 0; i < 0x80; i += 4) st(f + 0x90 + i, ld(f + 0x10 + i));
    st(f + 0x10C, 0x0273C714);
    u32 r = gabi::call<u32>(0x02749ECC, ld(self + 0x18), f + 0x90);
    if (ld(0x101FD598) == 0) {
        st(0x101FD598, 1);
        st(0x101FD97C, 0x10004E34);
    }
    if (r != 0) gabi::call_ptr(vfn(r, 0x70, 0xC), r, 0x101FD97C);
    u32 m = ld(0x101F8AE8);
    push_obj(m, gabi::call<u32>(0x0273D6A4, 0, m, 0));
    link_tail(m + 0xCC, gabi::call<u32>(0x0273D820, 0, m, 0));
    for (u32 i = 0; i < 4; i++) {
        u32 o = gabi::call<u32>(0x0273D9E4, 0, m, i);
        link_tail(o + 0x138, gabi::call<u32>(0x0273E2A0, 0, o));
        push_obj(m, o);
    }
    link_tail(m + 0xCC, gabi::call<u32>(0x0273E380, 0, m));
    u32 o = gabi::call<u32>(0x0273E4B4, 0, m);
    link_tail(o + 0x138, gabi::call<u32>(0x0273EAC8, 0, o));
    push_obj(m, o);
    push_obj(m, gabi::call<u32>(0x0273D2D0, 0, m, 2, 0));
    gabi::cpu->r[1] = entry;
}
VERIFY(0x02034CC8, Framework_createControllers);

/* 02034F88: the ProcUI save callback */
static u32 saveCallback() {
    WWHD_FUNC(0x02034F88, u32);
    return gabi::call<u32>(OSSavesDone_ReadyToRelease);
}
VERIFY(0x02034F88, saveCallback);

/* 02034F8C: initialize: ProcUI, then the base framework's initialize */
static void Framework_initialize(u32 a, u32 b, u32 c, u32 d) {
    WWHD_FUNC(0x02034F8C, void, a, b, c, d);
    gabi::call(ProcUIInit, 0x02034F88);
    /* on the main loop's stack when a save state is taken (frame-exact chain call): LR 02034FD8,
     * r28..r31 as the original keeps them (cross-build save states) */
    gabi::call_site(0x02034FD8, {{28, b}, {29, c}, {30, d}, {31, a}}, {}, 0x027476D8, a, b, c, d);
}
VERIFY(0x02034F8C, Framework_initialize);

static u32 thunk_0274AF44(u32 self) {
    WWHD_FUNC(0x02034FF8, u32, self);
    return gabi::call<u32>(0x0274AF44, self);
}
VERIFY(0x02034FF8, thunk_0274AF44);

/* 02034FFC: per-frame procedure (draw / calc of the root task) */
static void Framework_procFrame(u32 self) {
    WWHD_FUNC(0x02034FFC, void, self);
    gabi::call(0x0274A414, ld(self + 0x18));
    u32 g = ld(0x101F8DC0);
    if (g != 0) gabi::call(0x02799C70, g);
    u32 g0 = ld(0x101FD718);
    u32 o = ld(self + 0x1C);
    if (g0 == 0) {
        st(0x101FD718, 1);
        st(0x101FD990, 0x10004E6C);
    }
    if (o == 0 || gabi::call_ptr<u32>(vfn(o, 0x3C, 0xC), o, 0x101FD990) == 0) o = 0;
    /* on the main loop's stack when a save state is taken (frame-exact chain call): LR 02035090,
     * r30 = self and r31 = o as the original keeps them; self is taken back from r30 (after a save
     * state load it is the snapshot's) */
    gabi::call_site(0x02035090, {{30, self}, {31, o}}, {}, 0x02746790, o);
    self = gabi::cpu->r[30];
    gabi::call(0x0272A8C4, ld(0x101F86E8));
    gabi::call(0x0272AD80, ld(0x101F86E8));
    gabi::call(0x0274A5EC, ld(self + 0x18));
}
VERIFY(0x02034FFC, Framework_procFrame);

/* 020350C4: ProcUI message processing (re-enable the outputs, background, exit) */
static void Framework_procUI(u32 self) {
    WWHD_FUNC(0x020350C4, void, self);
    if (lbz(self + 0x38C) == 0 && ld(self + 0x28) == 2) {
        stb(self + 0x38C, 1);
        st(self + 0x28, 1);
        gabi::call(GX2SetTVEnable, 1);
        gabi::call(GX2SetDRCEnable, 1);
    }
    gabi::call(0x0274C8C4, self);
    u32 old = ld(self + 0x380);
    if (old == 2) {
        if (gabi::call<u32>(0x02617AE4, ld(0x101F5088)) != 0) gabi::call(0x02618720, ld(0x101F5088), 0);
        gabi::call(ProcUIDrawDoneRelease);
        gabi::call(0x0274FBF8, ld(0x101F8B18));
        old = ld(self + 0x380);
    }
    u32 s = gabi::call<u32>(ProcUIProcessMessages, 1);
    st(self + 0x380, s);
    if (s == 3) {
        gabi::call(0x02032AD8, gabi::call<u32>(0x02032C6C), 1);
        gabi::call(0x020330E8, ld(0x1018F2BC));
        gabi::call(ProcUIShutdown);
        u32 m = ld(0x101F8BA8);
        u32 r12 = m; /* the register the code after _Exit tests (see below) */
        if (m != 0) {
            u32 o = ld(m + 0x14);
            r12 = ld(0x101FD648);
            if (r12 == 0) {
                st(0x101FD648, 1);
                st(0x101FD958, 0x10004E44);
            }
            if (o != 0 && gabi::call_ptr<u32>(vfn(o, 0, 0xC), o, 0x101FD958) != 0 && o != 0) gabi::call(0x02762F34, o);
        }
        u32 v = ld(0x101F4FF0);
        if (v != 0) gabi::call(0x02614FBC, v);
        v = ld(0x101F50D8);
        if (v != 0) gabi::call(0x02619EEC, v);
        v = ld(0x1018F504);
        if (v != 0) gabi::call(0x0203BCF4, v);
        if (gabi::call<u32>(bossIsInitialized) != 0) gabi::call(bossFinalize);
        gabi::call(actFinalize);
        gabi::call(OSBlockThreadsOnExit);
        gabi::call(_Exit, -1);
        /* _Exit does not return; the code after it (shared with the s != 3 path) tests r12, which
         * then still holds the last value loaded into it above. Modelled as the binary falls through. */
        s = r12;
    }
    if (old != 2) return;
    if (s == 0) {
        gabi::call(GX2SetContextState, ld(ld(0x101F8B18) + 0x138));
        gabi::call(GX2SetTVEnable, 1);
        gabi::call(GX2SetDRCEnable, 1);
    }
    gabi::call(0x0274FCCC, ld(0x101F8B18));
}
VERIFY(0x020350C4, Framework_procUI);

/* 020352AC: main loop: run frames, waiting (02760C3C) while the root task is idle */
static void Framework_mainLoop(u32 self) {
    WWHD_FUNC(0x020352AC, void, self);
    u32 lo = ld(0x104A11CC), hi = ld(0x104A11C8);
    u64 t = (((u64)hi << 32) | lo) * 10;
    Pair32 q = gabi::call<Pair32>(0x028F5B8C, (u32)(t >> 32), (u32)t, 0, 1000);
    u32 entry = gabi::cpu->r[1];
    u32 f = entry - 0x28;
    gabi::cpu->r[1] = f;
    for (;;) {
        gabi::call(0x0274FBF8, ld(0x101F8B18));
        gabi::call(0x0274A414, ld(self + 0x18));
        gabi::call(0x0274A5EC, ld(self + 0x18));
        gabi::call(0x0274FCCC, ld(0x101F8B18));
        if (ld(ld(self + 0x18) + 0xB4) != 0 || ld(self + 0x28) != 0) break;
        st(f + 0xC, q.r4);
        st(f + 0x14, q.r4);
        st(f + 8, q.r3);
        st(f + 0x10, q.r3);
        gabi::call(0x02760C3C, f + 0x10);
    }
    u32 o = ld(self + 0x1C);
    gabi::call_ptr(vfn(o, 0x3C, 0x34), o, 0);
    gabi::cpu->r[1] = entry;
}
VERIFY(0x020352AC, Framework_mainLoop);

static void sinit_02035398() {
    WWHD_FUNC(0x02035398, void);
    header_sinit(0x102008E4, 0x1018F2F0, 0x10004E7C);
}
VERIFY(0x02035398, sinit_02035398);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x0203542C, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x0203542C, Comp_dt1);

static void Comp_empty() {
    WWHD_FUNC(0x02035440, void);
}
VERIFY(0x02035440, Comp_empty);

}  // namespace hd_sys_02034B14

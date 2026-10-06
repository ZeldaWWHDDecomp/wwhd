/* hd_sys_020355A4: the HD GameTask (no GameCube source).
 *
 * TU 020355A4..02035B87 (__sinit 02035AE0 + companions): cking GameTask (0xCC, a sead::Task 027454E0
 * "GameTask", registered as the current game task 1018F370 through a flag object at +0xC8), its
 * prepare (graphics / input / UI managers: EventMgr through 02753004), the per-frame calc (HOME /
 * system button hold counters 1018F338..1018F348, 30 frames each, and the frame's subsystem
 * updates) and the root task creation (02748B98 with factory 02035C74).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_020355A4 {

/* 020355A4: destructor of the "registered as current game task" flag object */
static void Reg_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x020355A4, void, self, flags);
    if (self == 0) return;
    if (lbz(self) != 0) st(0x1018F370, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x020355A4, Reg_dt);

static void GameTask_dtThunk(u32 self, s32 flags) {
    WWHD_FUNC(0x020355D0, void, self, flags);
    gabi::call(0x02035B88, self - 0x14, flags);
}
VERIFY(0x020355D0, GameTask_dtThunk);

/* 020355D8: register as the current game task (once) */
static void GameTask_register(u32 self) {
    WWHD_FUNC(0x020355D8, void, self);
    if (ld(0x1018F370) != 0) return;
    st(0x1018F370, self);
    stb(self + 0xC8, 1);
}
VERIFY(0x020355D8, GameTask_register);

/* 020355F8: constructor */
static u32 GameTask_ct(u32 self, u32 param) {
    WWHD_FUNC(0x020355F8, u32, self, param);
    u32 t = self;
    if (t == 0) {
        t = op_new(0xCC);
        if (t == 0) return 0;
    }
    gabi::call(0x027454E0, t, param, 0x10004FCC); /* "GameTask" */
    st(t + 0x70, 0x10005020);
    st(t + 0x20, 0x100050D8);
    stb(t + 0xC8, 0);
    return t;
}
VERIFY(0x020355F8, GameTask_ct);

/* create a manager singleton: 0275320C(4) heap, the name SafeString {NAME, 0x10004F8C} on the stack */
static inline u32 create_mgr(u32 f, u32 off, u32 heapoff, u32 name, u32 factory) {
    u32 r = gabi::call<u32>(0x0275320C, 4);
    st(f + off + 4, 0x10004F8C);
    st(f + off, name);
    u32 x = gabi::call<u32>(factory);
    return gabi::call<u32>(0x02753004, r + heapoff, f + off, x, 1, 0);
}

/* 02035678: prepare */
static void GameTask_prepare(u32 self) {
    WWHD_FUNC(0x02035678, void, self);
    u32 entry = gabi::cpu->r[1];
    u32 f = entry - 0x28;
    gabi::cpu->r[1] = f;
    gabi::call(0x0272879C, 0);
    gabi::call(0x027289DC, ld(0x101F86B4), 0);
    gabi::call(0x02708910, 0);
    gabi::call(0x02708634, ld(0x101F8294));
    gabi::call(0x027488E8, self);
    u32 v = ld(0x101F4FF0);
    if (v == 0) {
        u32 o = create_mgr(f, 8, 0xE4, 0x10004FD8, 0x0203E8E8);
        gabi::call(0x02614DAC);
        gabi::call_ptr(vfn(o, 0xC, 0x2C), o);
        v = ld(0x101F4FF0);
    }
    if (v != 0) gabi::call(0x02614EA8, v);
    if (ld(0x1018F4AC) == 0) {
        u32 o = create_mgr(f, 0x10, 0x4FDC, 0x10004FF4, 0x0203E8C4);
        gabi::call(0x020393E0);
        gabi::call(0x02039480, ld(0x1018F4AC));
        gabi::call_ptr(vfn(o, 0xC, 0x2C), o);
    }
    gabi::cpu->r[1] = entry;
}
VERIFY(0x02035678, GameTask_prepare);

/* 020357CC: button hold counters (30 frames) */
static void GameTask_holdCounters() {
    WWHD_FUNC(0x020357CC, void);
    u32 c = gabi::call<u32>(0x027201DC, ld(0x101F84DC) + 0x12C0);
    s32 n = (s32)ld(0x1018F338) + 1;
    st(0x1018F338, (u32)n);
    if (n >= 0x1E) {
        gabi::call(0x02726D3C, c, 1);
        st(0x1018F338, 0);
    }
    if (gabi::call<u32>(0x026184AC, ld(0x101F5088)) != 0) {
        n = (s32)ld(0x1018F33C) + 1;
        st(0x1018F33C, (u32)n);
        if (n >= 0x1E) {
            gabi::call(0x02726D5C, c, 1);
            st(0x1018F33C, 0);
        }
    } else {
        st(0x1018F33C, 0);
    }
    if (gabi::call<u32>(0x02617AE4, ld(0x101F5088)) != 0) {
        n = (s32)ld(0x1018F340) + 1;
        st(0x1018F340, (u32)n);
        if (n >= 0x1E) {
            gabi::call(0x02726D7C, c, 1);
            st(0x1018F340, 0);
        }
    } else {
        st(0x1018F340, 0);
    }
    u32 r = gabi::call<u32>(0x0271FC5C, gabi::call<u32>(0x027200D0, ld(0x101F84DC) + 0x12C0));
    if (r != 0) {
        n = (s32)ld(0x1018F344) + 1;
        st(0x1018F348, 0);
        st(0x1018F344, (u32)n);
        if (n >= 0x1E) {
            gabi::call(0x02726EEC, c, 1);
            st(0x1018F344, 0);
        }
    } else {
        n = (s32)ld(0x1018F348) + 1;
        st(0x1018F344, 0);
        st(0x1018F348, (u32)n);
        if (n >= 0x1E) {
            gabi::call(0x02726ECC, c, 1);
            st(0x1018F348, 0);
        }
    }
}
VERIFY(0x020357CC, GameTask_holdCounters);

/* 0203593C: calc (skipped while the error viewer shows) */
static void GameTask_calc() {
    WWHD_FUNC(0x0203593C, void);
    if (gabi::call<u32>(0x02032FE8, ld(0x1018F2BC)) != 0) return;
    gabi::call(0x020357CC);
    u32 r = gabi::call<u32>(0x025EE048);
    u32 w = 1;
    if (r != 0) w = gabi::call<u32>(0x02756170, ld(0x101F8B4C), r);
    gabi::call(0x02617AF4, ld(0x101F5088));
    u32 v = ld(0x101F4FF0);
    if (v != 0) gabi::call(0x02614F74, v);
    v = ld(0x101F8A20);
    if (v != 0) gabi::call(0x0273841C, v);
    gabi::call(0x025F172C);
    v = ld(0x101F8294);
    if (v != 0) gabi::call(0x0270870C, v);
    gabi::call(0x0255E854); /* dKy_setLight */
    v = ld(0x101F8A20);
    if (v != 0) gabi::call(0x02738438, v);
    u32 x = ld(ld(ld(0x101F95D0) + 0x1024));
    if (x != 0 && x + 0x15D4 != 0) gabi::call(0x0278FEBC, x + 0x15D4);
    gabi::call(0x027199C0, ld(0x101F8378));
    gabi::call(0x02618940, ld(0x101F5088));
    gabi::call(0x02728A74, ld(0x101F86B4));
    gabi::call(0x02039834, ld(0x1018F4AC));
    if (w != 1) gabi::call(0x02756170, ld(0x101F8B4C), w);
}
VERIFY(0x0203593C, GameTask_calc);

/* 02035A78: create the root task (factory 02035C74) */
static void GameTask_createRoot(u32 self) {
    WWHD_FUNC(0x02035A78, void, self);
    u32 entry = gabi::cpu->r[1];
    u32 f = entry - 0x98;
    gabi::cpu->r[1] = f;
    u32 v = ld(0x101F4FF0);
    if (v != 0) gabi::call(0x02616ED0, v);
    st(f + 8, 2);
    st(f + 0xC, 0x02035C74);
    gabi::call(0x02748B98, f + 0x10, f + 8);
    gabi::call(0x027487D4, self, f + 0x10);
    gabi::cpu->r[1] = entry;
}
VERIFY(0x02035A78, GameTask_createRoot);

static void sinit_02035AE0() {
    WWHD_FUNC(0x02035AE0, void);
    header_sinit(0x1020091C, 0x1018F34C, 0x10005014);
}
VERIFY(0x02035AE0, sinit_02035AE0);

static void Comp_dt1(u32 self, s32 flags) {
    WWHD_FUNC(0x02035B74, void, self, flags);
    if (self == 0) return;
    if ((flags & 1) == 0) return;
    op_delete(self);
}
VERIFY(0x02035B74, Comp_dt1);

}  // namespace hd_sys_020355A4

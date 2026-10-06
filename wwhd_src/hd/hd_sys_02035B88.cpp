/* hd_sys_02035B88: the HD HOME button menu / ProcUI callbacks (no GameCube source).
 *
 * TU 02035B88..02035FD7 (__sinit 02035F44 + companions): the GameTask destructor, the RTTI check and
 * root-task factory (0x1120 bytes, constructor 0260B7C4) of the root scene task, and the HOME button
 * menu controller (0x40: enabled +0, "menu allowed" +1, mutex +4; function-local static 10200938,
 * guard 10200988, published in 1018F3A4) with its ProcUI callbacks (HOME button denied, acquire /
 * release foreground: screen-dimming state saved in 1018F3A8).
 */
#include "hd_blk.h"
using namespace hdblk;

namespace hd_sys_02035B88 {

static constexpr u32 ProcUIRegisterCallback = 0xC0007890, OSEnableHomeButtonMenu = 0xC0009B18;

/* 02035B88: GameTask destructor */
static void GameTask_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02035B88, void, self, flags);
    if (self == 0) return;
    st(self + 0x20, 0x100050D8);
    gabi::call(0x020355A4, self + 0xC8, 2);
    gabi::call(0x027452C8, self, 0);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02035B88, GameTask_dt);

static void empty_02035BF4() {
    WWHD_FUNC(0x02035BF4, void);
}
VERIFY(0x02035BF4, empty_02035BF4);

/* 02035BF8: checkDerivedRuntimeTypeInfo: this class or its parent */
static u32 isDerived(u32 self, u32 ti) {
    WWHD_FUNC(0x02035BF8, u32, self, ti);
    if (ld(0x101FD598) == 0) {
        st(0x101FD598, 1);
        st(0x101FD97C, 0x10004FB4);
    }
    if (ti == 0x101FD97C) return 1;
    if (ld(0x101FD594) == 0) {
        st(0x101FD594, 1);
        st(0x101FD980, 0x10004FA4);
    }
    return ti == 0x101FD980 ? 1 : 0;
}
VERIFY(0x02035BF8, isDerived);

static void empty_02035C70() {
    WWHD_FUNC(0x02035C70, void);
}
VERIFY(0x02035C70, empty_02035C70);

/* 02035C74: root task factory */
static u32 factory(u32 arg) {
    WWHD_FUNC(0x02035C74, u32, arg);
    u32 c = ld(arg);
    u32 heap = ld(c + ld(c + 0x14) * 4);
    u32 p = gabi::call<u32>(0x0273B050, 0x1120, heap, 4);
    if (p == 0) return 0;
    return gabi::call<u32>(0x0260B7C4, p, arg);
}
VERIFY(0x02035C74, factory);

/* 02035CC8: HOME button menu controller constructor */
static u32 Home_ct(u32 self) {
    WWHD_FUNC(0x02035CC8, u32, self);
    u32 t = self;
    if (t == 0) {
        t = op_new(0x40);
        if (t == 0) return 0;
    }
    stb(t, 1);
    stb(t + 1, 1);
    gabi::call(0x02760084, t + 4);
    return t;
}
VERIFY(0x02035CC8, Home_ct);

static void Home_dt(u32 self, s32 flags) {
    WWHD_FUNC(0x02035D1C, void, self, flags);
    if (self == 0) return;
    gabi::call(0x02760168, self + 4, 2);
    if (flags & 1) op_delete(self);
}
VERIFY(0x02035D1C, Home_dt);

static u32 Home_isAllowed(u32 self) {
    WWHD_FUNC(0x02035D70, u32, self);
    return lbz(self + 1);
}
VERIFY(0x02035D70, Home_isAllowed);

static u32 Home_instance() {
    WWHD_FUNC(0x02035D78, u32);
    return ld(0x1018F3A4);
}
VERIFY(0x02035D78, Home_instance);

/* 02035D84: ProcUI callback 5 (HOME button denied): show the error viewer's notice when allowed */
static u32 cbHomeDenied() {
    WWHD_FUNC(0x02035D84, u32);
    u32 m = gabi::call<u32>(0x02035D78);
    if (gabi::call<u32>(0x02035D70, m) != 0) gabi::call(0x020330BC, ld(0x1018F2BC));
    return 0;
}
VERIFY(0x02035D84, cbHomeDenied);

/* 02035DC0: ProcUI callback 1 (release foreground): save the dimming state, enable it */
static u32 cbRelease() {
    WWHD_FUNC(0x02035DC0, u32);
    u8 v = (u8)gabi::call<u32>(0x02032BD0, gabi::call<u32>(0x02032C6C));
    stb(0x1018F3A8, v);
    gabi::call(0x02032AD8, gabi::call<u32>(0x02032C6C), 1);
    return 0;
}
VERIFY(0x02035DC0, cbRelease);

/* 02035DFC: ProcUI callback 0 (acquire foreground): restore the dimming state */
static u32 cbAcquire() {
    WWHD_FUNC(0x02035DFC, u32);
    u32 d = gabi::call<u32>(0x02032C6C);
    gabi::call(0x02032AD8, d, lbz(0x1018F3A8));
    return 0;
}
VERIFY(0x02035DFC, cbAcquire);

/* 02035E2C: create the controller (function-local static) and register the ProcUI callbacks */
static void Home_init() {
    WWHD_FUNC(0x02035E2C, void);
    if (ld(0x10200988) == 0) {
        st(0x10200988, 1);
        gabi::call(0x02035CC8, 0x10200938);
        gabi::call(0x028F026C, 0x1018F374);
    }
    st(0x1018F3A4, 0x10200938);
    gabi::call(ProcUIRegisterCallback, 5, 0x02035D84, 0, 1);
    gabi::call(ProcUIRegisterCallback, 1, 0x02035DC0, 0, 1);
    gabi::call(ProcUIRegisterCallback, 0, 0x02035DFC, 0, 1);
}
VERIFY(0x02035E2C, Home_init);

/* 02035ED4: enable / disable the HOME button menu */
static void Home_setEnabled(u32 self, u32 en) {
    WWHD_FUNC(0x02035ED4, void, self, en);
    gabi::call(0x027601BC, self + 4);
    stb(self, (u8)en);
    gabi::call(OSEnableHomeButtonMenu, en != 0 ? 1 : 0);
    gabi::call(0x027601F0, self + 4);
}
VERIFY(0x02035ED4, Home_setEnabled);

static void Home_setAllowed(u32 self, u32 v) {
    WWHD_FUNC(0x02035F3C, void, self, v);
    stb(self + 1, (u8)v);
}
VERIFY(0x02035F3C, Home_setAllowed);

static void sinit_02035F44() {
    WWHD_FUNC(0x02035F44, void);
    header_sinit_at(0x10200978, 0x1018F380, 0x100050E8, 0x1020092C, 0x10200934);
}
VERIFY(0x02035F44, sinit_02035F44);

}  // namespace hd_sys_02035B88

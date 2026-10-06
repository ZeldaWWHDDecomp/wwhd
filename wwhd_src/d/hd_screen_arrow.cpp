/* hd_screen_arrow: HD arrow-type layout screen (bow arrow icon with element particles; states
 * Wait / Disp / Change), WWHD. HD-only code (no GameCube
 * source): written from the WWHD code. Range 0261AC84..0261BC97; static initialiser 0261B83C.
 * 0261BC08..0261BC80 (controller flag adapters) were verified with d_auction_screen.
 * Not verified here (sead library template instances): 0261BAE8/0261BAFC (SafeString), 0261BB08
 * (StateID getter), 0261BB1C/0261BB5C/0261BB9C (state delegate invokers), 0261BBDC (StateID).
 *
 * Screen (0x70): base 02700ADC (cking 2D screen: +0x18 state machine, +0x20 current state, +0x44
 * layout with its animation at +0xD4), +4 vtable 100E2D28, +0x2C/+0x30 vtables, +0x50[2] panes
 * "G_Arrow_00/01" (names 1048DF50), +0x58[2] element particles, +0x60 particle position,
 * +0x6C u8 arrow type (02526C9C), +0x6D u8, +0x6E u8 shown. States 1048DF60 (Wait),
 * 1048DF90 (Disp), 1048DFC0 (Change).
 */
#include "gabi.h"
using namespace gabi;

namespace hd_screen_arrow {

static const u32 kWait = 0x1048DF60, kDisp = 0x1048DF90, kChange = 0x1048DFC0;

static u32 anim(u32 self) { return load<u32>(load<u32>(self + 0x44) + 0xD4); }
static void changeState(u32 self, u32 st) { call<void>(0x020063C0, self + 0x18, st); }

/* stops an emitter: kill, keep alive -1 frames, detach */
static void stopEmitter(u32 slot) {
    u32 e = load<u32>(slot);
    store<u32>(e + 0x254, load<u32>(e + 0x254) | 4);
    e = load<u32>(slot);
    store<u32>(e + 0x5C, 0xFFFFFFFF);
    store<u32>(e + 0x254, load<u32>(e + 0x254) | 1);
    e = load<u32>(slot);
    store<u32>(e + 0x254, load<u32>(e + 0x254) & ~0x40u);
}

/* 0261AC84: deleting destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x0261AC84, void, p, flags);
    if (!p) return;
    store<u32>(p + 0x2C, 0x100E2E38);
    store<u32>(p + 0x30, 0x100E2E48);
    call<void>(0x02700A70, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x0261AC84, dtor);

/* 0261ACF0: removes both element particles */
void killParticles(u32 self) {
    WWHD_FUNC(0x0261ACF0, void, self);
    for (u32 i = 0; i < 2; i++) {
        const u32 slot = self + 0x58 + 4 * i;
        if (!load<u32>(slot)) continue;
        stopEmitter(slot);
        store<u32>(slot, 0);
    }
    store<u8>(self + 0x6E, 0);
}
VERIFY(0x0261ACF0, killParticles);

/* 0261AE00: hides (state Disp) when shown */
void hide(u32 self) {
    WWHD_FUNC(0x0261AE00, void, self);
    if (!load<u8>(self + 0x6E)) return;
    store<u8>(self + 0x6D, 0);
    changeState(self, kDisp);
}
VERIFY(0x0261AE00, hide);

static void spawn(u32 self, u32 type, u32 pos, u32 slot) {
    u32 idx = u8(type - 1);
    if (idx >= 3) idx = 0;
    const u32 id = load<u16>(0x100E2BB4 + 2 * idx);
    const u32 env = call<u32>(0x025200D4);
    store<u32>(slot, call<u32>(0x025A847C, load<u32>(env + 0x5AB0), 7u, id, pos, 0u, 0u, 255u, 0u, 0xFFFFFFFFu, 0u, 0u, 0u));
}

/* 0261AE28: shows the current arrow type at once */
void setType(u32 self) {
    WWHD_FUNC(0x0261AE28, void, self);
    const u32 t = call<u32>(0x02526C9C);
    store<u8>(self + 0x6C, u8(t));
    call<void>(0x0200552C, anim(self), 0u, 1u, f32(f64(t)));
    call<void>(0x0200552C, anim(self), 1u, 2u, f32(f64(load<u8>(self + 0x6C))));
    const u32 s = self + 0x58;
    if (load<u32>(s)) stopEmitter(s);
    store<u32>(s, load<u32>(s + 4));
    store<u32>(s + 4, 0);
    const u32 type = load<u8>(self + 0x6C);
    if (type && !load<u32>(s)) spawn(self, type, self + 0x60, s);
}
VERIFY(0x0261AE28, setType);

/* 0261AFA4: state Wait: changes to Wait when hidden, to Change when the arrow type changed */
void waitMove(u32 self) {
    WWHD_FUNC(0x0261AFA4, void, self);
    if (!load<u8>(self + 0x6E)) changeState(self, kWait);
    else if (const u32 t = call<u32>(0x02526C9C); load<u8>(self + 0x6C) != t) changeState(self, kChange);
    call_ptr<void>(load<u32>(load<u32>(self + 4) + 0x8C), self);
}
VERIFY(0x0261AFA4, waitMove);

/* 0261B020: state Change init: plays the change animation and swaps the particle */
void changeInit(u32 self) {
    WWHD_FUNC(0x0261B020, void, self);
    call<void>(0x020053E4, anim(self), 2u, 0u, load<f32>(0x100E2C70));
    call<void>(0x0200552C, anim(self), 0u, 1u, f32(f64(load<u8>(self + 0x6C))));
    const u32 t = call<u32>(0x02526C9C);
    store<u8>(self + 0x6C, u8(t));
    call<void>(0x0200552C, anim(self), 1u, 2u, f32(f64(t)));
    if (load<u32>(self + 0x5C)) {
        store<u32>(self + 0x58, load<u32>(self + 0x5C));
        store<u32>(self + 0x5C, 0);
        const u32 e = load<u32>(self + 0x58);
        const f32 x = load<f32>(self + 0x60);
        f32 y = load<f32>(self + 0x64);
        const f32 z = load<f32>(self + 0x68);
        if (load<u8>(e + 0x262) >= 7) y = -y;
        store<f32>(e + 0x22C, x);
        store<f32>(e + 0x230, y);
        store<f32>(e + 0x234, z);
    }
    if (!load<u8>(self + 0x6C)) return;
    Local<f32[8]> F;
    call<void>(0x02704D2C, F.a + 0x10, load<u32>(self + 0x54));
    const u32 type = load<u8>(self + 0x6C);
    store<f32>(F.a + 8, load<f32>(F.a + 0x18));
    store<f32>(F.a + 4, load<f32>(F.a + 0x14));
    store<f32>(F.a, load<f32>(F.a + 0x10));
    spawn(self, type, F.a, self + 0x5C);
}
VERIFY(0x0261B020, changeInit);

/* 0261B1C8: state Change: particles follow the panes; back to Wait/Disp when done */
void changeMove(u32 self) {
    WWHD_FUNC(0x0261B1C8, void, self);
    for (u32 i = 0; i < 2; i++) {
        if (!load<u32>(self + 0x58 + 4 * i)) continue;
        Local<f32[4]> pos;
        call<void>(0x02704D2C, pos.a, load<u32>(self + 0x50 + 4 * i));
        const u32 e = load<u32>(self + 0x58 + 4 * i);
        const f32 x = load<f32>(pos.a);
        f32 y = load<f32>(pos.a + 4);
        const f32 z = load<f32>(pos.a + 8);
        if (load<u8>(e + 0x262) >= 7) y = -y;
        store<f32>(e + 0x230, y);
        store<f32>(e + 0x234, z);
        store<f32>(e + 0x22C, x);
        const u32 e2 = load<u32>(self + 0x58 + 4 * i);
        store<u8>(e2 + 0x247, load<u8>(load<u32>(self + 0x50 + 4 * i) + 0x46));
    }
    if (!load<u8>(self + 0x6E)) changeState(self, kWait);
    else if (const u32 t = call<u32>(0x02526C9C); load<u8>(self + 0x6C) != t) changeState(self, kChange);
    else if (call<u32>(0x02005840, anim(self), 2u)) changeState(self, kDisp);
    call_ptr<void>(load<u32>(load<u32>(self + 4) + 0x8C), self);
}
VERIFY(0x0261B1C8, changeMove);

/* 0261B370: removes the current particle */
void killCurrent(u32 self) {
    WWHD_FUNC(0x0261B370, void, self);
    const u32 s = self + 0x58;
    if (!load<u32>(s)) return;
    stopEmitter(s);
    store<u32>(s, 0);
}
VERIFY(0x0261B370, killCurrent);

/* 0261B3C4: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x0261B3C4, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x70u);
        if (!p) return 0;
    }
    call<void>(0x02700ADC, p);
    store<u32>(p + 4, 0x100E2D28);
    store<u32>(p + 0x30, 0x100E2E48);
    store<u32>(p + 0x2C, 0x100E2E38);
    if (!(p + 0x50)) call<u32>(0x0273AD10, 8u);
    if (!(p + 0x58)) call<u32>(0x0273AD10, 8u);
    const f32 z = load<f32>(0x100E2C80);
    store<u8>(p + 0x6E, 0);
    store<f32>(p + 0x68, z);
    store<u8>(p + 0x6D, 0);
    store<f32>(p + 0x60, z);
    store<u8>(p + 0x6C, 0);
    store<f32>(p + 0x64, z);
    for (u32 i = 0; i < 2; i++) {
        store<u32>(p + 0x50 + 4 * i, 0);
        store<u32>(p + 0x58 + 4 * i, 0);
    }
    return p;
}
VERIFY(0x0261B3C4, ctor);

/* 0261B4E4: screen setup: the two arrow panes, the particle position and the Wait state */
u32 setup(u32 self, u32 a, u32 b, u32 c) {
    WWHD_FUNC(0x0261B4E4, u32, self, a, b, c);
    if (!call<u32>(0x02700B48, self, 0x1048DFF0u, a, b, c)) return 0;
    const u32 root = load<u32>(load<u32>(load<u32>(self + 0x44) + 4) + 0xC);
    for (u32 i = 0; i < 2; i++) {
        const u32 str = 0x1048DF50 + 8 * i;
        const u32 fn = load<u32>(load<u32>(str + 4) + 0x14);
        const u32 vt = load<u32>(root + 8);
        call_ptr<void>(fn, str);
        store<u32>(self + 0x50 + 4 * i, call_ptr<u32>(load<u32>(vt + 0x5C), root, load<u32>(str), 1u));
    }
    Local<f32[4]> pos;
    call<void>(0x02704D2C, pos.a, load<u32>(self + 0x50));
    const u32 st = load<u32>(self + 0x18);
    store<f32>(self + 0x60, load<f32>(pos.a));
    store<f32>(self + 0x68, load<f32>(pos.a + 8));
    store<f32>(self + 0x64, load<f32>(pos.a + 4));
    const u32 action = call_ptr<u32>(load<u32>(load<u32>(st) + 0x14), st, kWait);
    store<u32>(self + 0x20, action);
    call_ptr<void>(load<u32>(load<u32>(action) + 0x1C), action);
    return 1;
}
VERIFY(0x0261B4E4, setup);

/* 0261B628: hides the particles (screen hide) */
void hideParticles(u32 self) {
    WWHD_FUNC(0x0261B628, void, self);
    call<void>(0x026F8B00, self);
    for (u32 i = 0; i < 2; i++) {
        const u32 slot = self + 0x58 + 4 * i;
        if (!load<u32>(slot)) continue;
        u32 e = load<u32>(slot);
        store<u32>(e + 0x5C, 0xFFFFFFFF);
        store<u32>(e + 0x254, load<u32>(e + 0x254) | 1);
        e = load<u32>(slot);
        store<u32>(e + 0x254, load<u32>(e + 0x254) & ~0x40u);
    }
}
VERIFY(0x0261B628, hideParticles);

void stateExecute(u32 self) { WWHD_FUNC(0x0261B70C, void, self); call<void>(0x02006364, self + 0x18); }
VERIFY(0x0261B70C, stateExecute);
void toWait(u32 self) { WWHD_FUNC(0x0261B714, void, self); changeState(self, kWait); }
VERIFY(0x0261B714, toWait);

/* 0261B724: loads the layout and binds its three animations */
u32 loadScreen(u32 self, u32 a, u32 b, u32 c, u32 d, u32 e) {
    WWHD_FUNC(0x0261B724, u32, self, a, b, c, d, e);
    u32 scr = call<u32>(0x0273B050, 0x110u, load<u32>(load<u32>(0x1018C404) + 0x10), 4u);
    if (scr) scr = call<u32>(0x026FADB0, scr);
    store<u32>(self + 0x44, scr);
    if (!scr) return 0;
    if (!call_ptr<u32>(load<u32>(load<u32>(scr + 0xE0) + 0x14), scr, a, b, c, d, 3u, 3u, e)) return 0;
    for (u32 i = 0; i < 3; i++)
        call<void>(0x02004E04, anim(self), i, 0x1048DF38 + 8 * i, 0x1048DF20 + 8 * load<u32>(0x100E2BD4 + 4 * i));
    return 1;
}
VERIFY(0x0261B724, loadScreen);

/* 0261B83C: static initialiser: shared objects, pane/animation names and the three states */
void staticInit() {
    WWHD_FUNC(0x0261B83C, void);
    const u32 b = 0x1048DF10;
    store<u32>(b + 0xC, 0);
    store<u32>(b + 8, 0);
    store<u32>(b + 4, 0);
    store<u32>(b, 0);
    call<void>(0x028F026C, 0x101F5128u);
    const f32 lo = load<f32>(0x100E2C84), hi = load<f32>(0x100E2C88);
    store<f32>(0x1048DF04, lo);
    store<f32>(0x1048DF08, hi);
    call<void>(0x028ED6F8, 0x1048DF0Cu);
    call<void>(0x028F026C, 0x101F5134u);
    call<void>(0x028EAB2C, 0x1048DF0Du);
    call<void>(0x028F026C, 0x101F5140u);
    const u32 vt = 0x100E2BBC;
    const u32 an = 0x1048DF38, ar = 0x1048DF20, pane = 0x1048DF50, ps = 0x1048DFF0;
    store<u32>(an + 4, vt);
    store<u32>(ar + 4, vt);
    store<u32>(ar, 0x100E2C98);
    store<u32>(ar + 0xC, vt);
    store<u32>(ar + 8, 0x100E2CB4);
    store<u32>(ar + 0x14, vt);
    store<u32>(ps + 4, vt);
    store<u32>(ps, 0x100E2CE4);
    store<u32>(an, 0x100E2CA8);
    store<u32>(an + 0xC, vt);
    store<u32>(an + 8, 0x100E2CA8);
    store<u32>(an + 0x14, vt);
    const u32 guard = load<u32>(0x101FD8E4);
    store<u32>(pane + 4, vt);
    store<u32>(an + 0x10, 0x100E2C8C);
    store<u32>(ar + 0x10, 0x100E2CC0);
    store<u32>(pane, 0x100E2CCC);
    store<u32>(pane + 0xC, vt);
    store<u32>(pane + 8, 0x100E2CD8);
    u32 id;
    if (guard) id = load<u32>(0x101FDD50);
    else {
        id = 0;
        store<u32>(0x101FD8E4, 1);
    }
    const u32 w = kWait, d = kDisp, c = kChange;
    store<u16>(w + 0xC, 0);
    store<u16>(w + 0xE, 0x19);
    store<u16>(w + 0x14, 0);
    store<u32>(w, id + 1);
    store<u16>(w + 0x16, 0x1A);
    store<u32>(d, id + 2);
    store<u16>(d + 0xC, 0);
    store<u16>(w + 0x1C, 0);
    store<u16>(w + 0x1E, 0x1B);
    store<u32>(w + 4, 0x100E2CF0);
    store<u32>(w + 0x10, 4);
    store<u32>(d + 4, 0x100E2D00);
    store<u16>(c + 0xC, 0);
    store<u16>(c + 0xE, 0x1F);
    store<u32>(d + 0x10, 4);
    store<u32>(d + 0x18, 4);
    store<u32>(w + 0x18, 4);
    store<u32>(d + 0x20, 4);
    store<u16>(d + 0xE, 0x1C);
    store<u32>(d + 0x28, 0);
    store<u16>(d + 0x14, 0);
    store<u16>(d + 0x16, 0x1D);
    store<u16>(d + 0x1C, 0);
    store<u32>(w + 0x20, 4);
    store<u32>(w + 0x28, 0);
    store<u16>(d + 0x1E, 0x1E);
    store<u16>(w + 0x24, 0);
    store<u16>(c + 0x14, 0);
    store<u32>(c, id + 3);
    store<u32>(c + 4, 0x100E2D10);
    store<u16>(d + 0x24, 0);
    store<u16>(w + 0x26, 0);
    store<u32>(w + 8, 0x100E2C28);
    store<u32>(d + 8, 0x100E2C28);
    store<u16>(d + 0x26, 0);
    store<u32>(d + 0x2C, 0x101FF32C);
    store<u32>(w + 0x2C, 0x101FF32C);
    store<u32>(c + 0x10, 4);
    store<u32>(c + 0x18, 4);
    store<u32>(c + 0x20, 4);
    store<u32>(c + 0x28, 0);
    store<u32>(c + 8, 0x100E2C28);
    store<u32>(c + 0x2C, 0x101FF32C);
    store<u16>(c + 0x16, 0x20);
    store<u16>(c + 0x1C, 0);
    store<u16>(c + 0x1E, 0x21);
    store<u16>(c + 0x24, 0);
    store<u32>(0x101FDD50, id + 3);
    store<u16>(c + 0x26, 0);
}
VERIFY(0x0261B83C, staticInit);

/* 0261BB00 / 0261BB04 / 0261BB10 / 0261BB14 / 0261BB18: empty state enter/exit functions */
void nop1() { WWHD_FUNC(0x0261BB00, void); }
VERIFY(0x0261BB00, nop1);
void nop2() { WWHD_FUNC(0x0261BB04, void); }
VERIFY(0x0261BB04, nop2);
void nop3() { WWHD_FUNC(0x0261BB10, void); }
VERIFY(0x0261BB10, nop3);
void nop4() { WWHD_FUNC(0x0261BB14, void); }
VERIFY(0x0261BB14, nop4);
void nop5() { WWHD_FUNC(0x0261BB18, void); }
VERIFY(0x0261BB18, nop5);

} // namespace hd_screen_arrow

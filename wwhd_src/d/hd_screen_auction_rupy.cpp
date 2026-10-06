/* hd_screen_auction_rupy: HD auction rupee screen (rupee meter G_Meter_00 and a three-digit
 * counter G_AuctionRupy_00 that counts towards the current bid), WWHD. HD-only code (no GameCube source): written from the WWHD code.
 * Range 0261BC98..0261D087; static initialiser 0261CB94. Not verified here (sead library template
 * instances): 0261CF68/0261CF7C (SafeString), 0261CF88 (StateID getter), 0261CF9C/0261CFDC/0261D01C
 * (state delegate invokers), 0261D05C (StateID).
 *
 * Screen (0x6C): base 026F89E8 (cking screen with child screens: +0x18 state machine, +0x20
 * current state, +0x44 layout with its animation at +0xD4, +0x48/+0x4C child array), +4 vtable
 * 100E3050, +0x2C/+0x30 vtables, +0x50 meter scale (layout frame count), +0x54 rupees shown on the
 * meter, +0x58 counter delay, +0x5C s16 counter value, +0x5E[3] s16 digits, +0x64 u8 open,
 * +0x65..+0x69 counter animation flags, +0x6A/+0x6B meter flags. The rupee amounts come from
 * the game info (+0x5BA6 bid, +0x5BA8 meter value). Child screens are hd_screen_auction_number
 * (created by 0261D088).
 */
#include "gabi.h"
using namespace gabi;

namespace hd_screen_auction_rupy {

static const u32 kState = 0x1049DD60;

static u32 anim(u32 self) { return load<u32>(load<u32>(self + 0x44) + 0xD4); }
static void vcall(u32 self, u32 off) { call_ptr<void>(load<u32>(load<u32>(self + 4) + off), self); }
static u32 child(u32 self, u32 i) {
    u32 p = load<u32>(self + 0x4C);
    if (i < load<u32>(self + 0x48)) p += 4 * i;
    return load<u32>(p);
}

/* 0261BC98: deleting destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x0261BC98, void, p, flags);
    if (!p) return;
    store<u32>(p + 0x2C, 0x100E3148);
    store<u32>(p + 0x30, 0x100E3158);
    call<void>(0x026F88F0, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x0261BC98, dtor);

/* 0261BD04: resets the meter to the current value */
void resetMeter(u32 self) {
    WWHD_FUNC(0x0261BD04, void, self);
    call<void>(0x02005674, anim(self), 3u, 2u);
    call<void>(0x02005674, anim(self), 4u, 1u);
    const u32 g = call<u32>(0x025200D4);
    const s16 v = s16(load<u16>(g + 0x5BA8));
    store<u8>(self + 0x6A, 0);
    store<u8>(self + 0x6B, 0);
    store<f32>(self + 0x54, f32(f64(s32(v))));
}
VERIFY(0x0261BD04, resetMeter);

/* 0261BD90: resets the counter */
void resetCounter(u32 self) {
    WWHD_FUNC(0x0261BD90, void, self);
    call<void>(0x02005674, anim(self), 0u, 0u);
    store<u8>(self + 0x67, 0);
    store<u8>(self + 0x69, 1);
    store<u8>(self + 0x65, 0);
    store<u32>(self + 0x58, 0);
    store<u8>(self + 0x68, 0);
    store<u8>(self + 0x66, 0);
    store<u16>(self + 0x5C, 0);
}
VERIFY(0x0261BD90, resetCounter);

/* 0261BDF0: opens the screen */
void open(u32 self) {
    WWHD_FUNC(0x0261BDF0, void, self);
    call<void>(0x02676900, load<u32>(load<u32>(0x101F8344) + 0x178));
    call<void>(0x0261BD04, self);
    call<void>(0x0261BD90, self);
    vcall(self, 0x8C);
    call<void>(0x025916EC);
    store<u8>(self + 0x64, 1);
}
VERIFY(0x0261BDF0, open);

/* 0261BE58: meter: shows/hides it and follows the value (at most 100) */
void meterMove(u32 self) {
    WWHD_FUNC(0x0261BE58, void, self);
    const f32 one = load<f32>(0x100E2F30);
    const u32 on = load<u8>(self + 0x6A), shown = load<u8>(self + 0x6B);
    if (!on) {
        if (!shown) return;
        store<u8>(self + 0x6B, 0);
        call<void>(0x020053E4, anim(self), 5u, 2u, one);
        return;
    }
    if (!shown) {
        store<u8>(self + 0x6B, 1);
        call<void>(0x020053E4, anim(self), 3u, 2u, one);
    }
    const u32 g = call<u32>(0x025200D4);
    f32 v = f32(f64(s32(s16(load<u16>(g + 0x5BA8)))));
    const f32 mx = load<f32>(0x100E2F40);
    const f32 cur = load<f32>(self + 0x54);
    if (!(v <= mx)) v = mx;
    if (cur == v) return;
    const f32 r = v / mx;
    store<f32>(self + 0x54, v);
    call<void>(0x0200552C, anim(self), 4u, 1u, fmuls_ppc(r, load<f32>(self + 0x50)));
}
VERIFY(0x0261BE58, meterMove);

/* 0261BF64: sets the three digits from the counter value */
void setDigits(u32 self) {
    WWHD_FUNC(0x0261BF64, void, self);
    s16 div = 10, mul = 1;
    for (u32 i = 0; i < 3; i++) {
        const s32 v = s16(load<u16>(self + 0x5C));
        const s16 d = s16((v - (v / div) * div) / mul);
        div = s16(div * 10);
        mul = s16(mul * 10);
        call<void>(0x0261D454, child(self, i), u32(s32(d)));
        store<u16>(self + 0x5E + 2 * i, u16(d));
    }
}
VERIFY(0x0261BF64, setDigits);

/* 0261BFFC: counts one step towards the bid (1, 10 or 100 by distance) and rolls the digits */
void countStep(u32 self, u32 up) {
    WWHD_FUNC(0x0261BFFC, void, self, up);
    const u32 g = call<u32>(0x025200D4);
    const s32 cur = s16(load<u16>(self + 0x5C));
    const s32 tgt = s16(load<u16>(g + 0x5BA6));
    const s32 dist = up ? s16(tgt - cur) : s16(cur - tgt);
    s32 step;
    if (dist < 200) step = dist < 50 ? 1 : 10;
    else step = 100;
    s32 v = up ? s16(cur + step) : s16(cur - step);
    store<u16>(self + 0x5C, u16(v));
    s16 div = 10, mul = 1;
    for (u32 i = 0; i < 3; i++) {
        if (i) v = s16(load<u16>(self + 0x5C));
        const s16 d = s16((v - (v / div) * div) / mul);
        div = s16(div * 10);
        mul = s16(mul * 10);
        const u32 c = child(self, i);
        const s32 old = s16(load<u16>(self + 0x5E + 2 * i));
        call<void>(0x0261D4A8, c, u32(old), u32(s32(d)), up);
        store<u16>(self + 0x5E + 2 * i, u16(d));
    }
}
VERIFY(0x0261BFFC, countStep);

/* 0261C134: counter: shows it, counts towards the bid with a delay and plays the sounds */
void counterMove(u32 self) {
    WWHD_FUNC(0x0261C134, void, self);
    const f32 one = load<f32>(0x100E2F30);
    const u32 on = load<u8>(self + 0x67), shown = load<u8>(self + 0x68);
    if (!on) {
        if (!shown) return;
        store<u8>(self + 0x68, 0);
        call<void>(0x020053E4, anim(self), 2u, 0u, one);
        return;
    }
    if (!shown) {
        store<u8>(self + 0x68, 1);
        call<void>(0x020053E4, anim(self), 0u, 0u, one);
        if (!load<u8>(self + 0x66)) {
            store<u8>(self + 0x69, 1);
            store<u8>(self + 0x66, 1);
            store<u8>(self + 0x65, 1);
            call<void>(0x02005674, anim(self), 1u, 0u);
        }
    }
    u32 g = call<u32>(0x025200D4);
    const s32 cur = s16(load<u16>(self + 0x5C));
    if (cur < s32(s16(load<u16>(g + 0x5BA6)))) {
        if (cur == 0) {
            g = call<u32>(0x025200D4);
            store<u16>(self + 0x5C, load<u16>(g + 0x5BA6));
            call<void>(0x0261BF64, self);
        } else if (const u32 w = load<u32>(self + 0x58)) {
            store<u32>(self + 0x58, w - 1);
        } else {
            call<void>(0x0261BFFC, self, 1u);
            g = call<u32>(0x025200D4);
            if (s16(load<u16>(self + 0x5C)) == s16(load<u16>(g + 0x5BA6))) call<void>(0x025E1988, 0x8E0u);
            else {
                call<void>(0x025E1988, 0x8DFu);
                store<u32>(self + 0x58, 3);
            }
        }
    } else {
        g = call<u32>(0x025200D4);
        if (s16(load<u16>(self + 0x5C)) > s16(load<u16>(g + 0x5BA6))) {
            if (const u32 w = load<u32>(self + 0x58)) store<u32>(self + 0x58, w - 1);
            else {
                call<void>(0x0261BFFC, self, 0u);
                g = call<u32>(0x025200D4);
                if (s16(load<u16>(self + 0x5C)) != s16(load<u16>(g + 0x5BA6))) store<u32>(self + 0x58, 3);
            }
        }
    }
    const u32 counting = load<u8>(self + 0x65), idle = load<u8>(self + 0x69);
    if (counting) {
        if (idle) return;
        store<u8>(self + 0x69, 1);
        call<void>(0x02005708, anim(self), 1u, 0u);
        call<void>(0x02005488, anim(self), 1u, 0u, load<f32>(0x100E2F44));
    } else {
        if (!idle) return;
        store<u8>(self + 0x69, 0);
        call<void>(0x020053E4, anim(self), 1u, 0u, one);
    }
}
VERIFY(0x0261C134, counterMove);

/* 0261C3DC: state Move: meter and counter, then the screen update */
void move(u32 self) {
    WWHD_FUNC(0x0261C3DC, void, self);
    call<void>(0x0261BE58, self);
    call<void>(0x0261C134, self);
    vcall(self, 0xB4);
    vcall(self, 0x8C);
}
VERIFY(0x0261C3DC, move);

/* 0261C438: closes the screen */
void close(u32 self) {
    WWHD_FUNC(0x0261C438, void, self);
    call<void>(0x026768E4, load<u32>(load<u32>(0x101F8344) + 0x178));
    call<void>(0x025916FC);
    store<u8>(self + 0x64, 0);
}
VERIFY(0x0261C438, close);

void toMove(u32 self) { WWHD_FUNC(0x0261C47C, void, self); call<void>(0x020063C0, self + 0x18, kState); }
VERIFY(0x0261C47C, toMove);

/* 0261C48C: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x0261C48C, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x6Cu);
        if (!p) return 0;
    }
    call<void>(0x026F89E8, p);
    const f32 z = load<f32>(0x100E2F48);
    store<u8>(p + 0x66, 0);
    store<u8>(p + 0x6A, 0);
    store<u8>(p + 0x65, 0);
    store<u8>(p + 0x6B, 0);
    store<f32>(p + 0x50, z);
    store<f32>(p + 0x54, z);
    store<u32>(p + 4, 0x100E3050);
    store<u16>(p + 0x5C, 0);
    store<u8>(p + 0x68, 0);
    store<u8>(p + 0x64, 0);
    store<u32>(p + 0x30, 0x100E3158);
    store<u32>(p + 0x2C, 0x100E3148);
    store<u8>(p + 0x67, 0);
    store<u32>(p + 0x58, 0);
    store<u8>(p + 0x69, 0);
    for (u32 i = 0; i < 3; i++) store<u16>(p + 0x5E + 2 * i, 0);
    return p;
}
VERIFY(0x0261C48C, ctor);

/* 0261C54C: screen setup (layout, child screens, meter scale, first state) */
u32 setup(u32 self, u32 a, u32 b, u32 c) {
    WWHD_FUNC(0x0261C54C, u32, self, a, b, c);
    call<void>(0x026F90D8, self, 0x1048E09Cu, 2u);
    if (!call_ptr<u32>(load<u32>(load<u32>(self + 4) + 0x7C), self, 0x1048E10Cu, load<u32>(self + 0x34), a, b, c))
        return 0;
    if (!call_ptr<u32>(load<u32>(load<u32>(self + 4) + 0xA4), self)) return 0;
    call<void>(0x02005674, anim(self), 0u, 0u);
    call<void>(0x02005674, anim(self), 3u, 2u);
    const u32 an = anim(self);
    u32 fr = load<u32>(an + 8);
    if (load<u32>(an + 4) > 4) fr += 0x10;
    const u32 n = call<u32>(0x028713B8, load<u32>(load<u32>(fr) + 0x34));
    store<f32>(self + 0x50, f32(f64(n)));
    const u32 st = load<u32>(self + 0x18);
    const u32 action = call_ptr<u32>(load<u32>(load<u32>(st) + 0x14), st, kState);
    store<u32>(self + 0x20, action);
    call_ptr<void>(load<u32>(load<u32>(action) + 0x1C), action);
    vcall(self, 0xB4);
    vcall(self, 0x8C);
    return 1;
}
VERIFY(0x0261C54C, setup);

/* 0261C6EC: screen removal */
void remove(u32 self) {
    WWHD_FUNC(0x0261C6EC, void, self);
    vcall(self, 0xAC);
    vcall(self, 0x84);
    call<void>(0x026F91F8, self);
}
VERIFY(0x0261C6EC, remove);

/* 0261C740: runs the state machine unless the Move state is current */
void execute(u32 self) {
    WWHD_FUNC(0x0261C740, void, self);
    const u32 st = call<u32>(0x02006478, self + 0x18);
    const u32 fn = load<u32>(load<u32>(st + 8) + 0x14);
    const u32 vt = load<u32>(kState + 8);
    const u32 cur = call_ptr<u32>(fn, st);
    if (cur == call_ptr<u32>(load<u32>(vt + 0x14), kState)) return;
    call<void>(0x02006364, self + 0x18);
}
VERIFY(0x0261C740, execute);

/* 0261C7C0: draw when open */
void draw(u32 self, u32 a) {
    WWHD_FUNC(0x0261C7C0, void, self, a);
    if (!load<u8>(self + 0x64)) return;
    call_ptr<void>(load<u32>(load<u32>(self + 4) + 0x9C), self, a);
    call_ptr<void>(load<u32>(load<u32>(self + 4) + 0xC4), self, a);
}
VERIFY(0x0261C7C0, draw);

/* string equality as inlined (pointer check on the earlier loads, then at most 0x40001
 * characters compared through the string fields read again) */
static bool strEq(u32 a0, u32 b0, u32 pa, u32 pb) {
    if (a0 == b0) return true;
    const u32 a = load<u32>(pa), b = load<u32>(pb);
    for (u32 k = 0; k < 0x40001; k++) {
        const u8 x = load<u8>(a + k), y = load<u8>(b + k);
        if (x != y) return false;
        if (!x) return true;
    }
    return false;
}
static u32 cstr(u32 s) {
    call_ptr<void>(load<u32>(load<u32>(s + 4) + 0x14), s);
    return load<u32>(s);
}

/* 0261C830: creates the child screen for a layout part: digit panes become number screens */
u32 createChild(u32 self, u32 info) {
    WWHD_FUNC(0x0261C830, u32, self, info);
    const u32 S = 0x1048E170;
    const u32 factory = 0x0261D088;
    const u32 type = info + 8, name = info + 0x14;
    const u32 s = cstr(S);
    for (u32 i = 0; i < 3; i++) {
        call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
        call_ptr<void>(load<u32>(load<u32>(name + 4) + 0x14), name);
        const u32 n = load<u32>(name);
        const u32 ref = cstr(0x1048E06C + 8 * i);
        if (!strEq(n, ref, name, 0x1048E06C + 8 * i)) continue;
        Local<u32[2]> tmp;
        store<u32>(tmp.a, s);
        store<u32>(tmp.a + 4, 0x100E2E6C);
        call_ptr<void>(load<u32>(load<u32>(type + 4) + 0x14), type);
        call_ptr<void>(load<u32>(load<u32>(type + 4) + 0x14), type);
        const u32 t = load<u32>(type);
        call_ptr<void>(load<u32>(load<u32>(tmp.a + 4) + 0x14), tmp.a);
        if (!strEq(t, load<u32>(tmp.a), type, tmp.a)) return call<u32>(0x026F92C0, self, i, info);
        u32 slot = load<u32>(self + 0x4C);
        if (i < load<u32>(self + 0x48)) slot += 4 * i;
        store<u32>(slot, call_ptr<u32>(factory, 0u, 0u, info));
        return child(self, i) ? 1 : 0;
    }
    return 1;
}
VERIFY(0x0261C830, createChild);

/* 0261CA68: loads the layout and binds its six animations */
u32 loadScreen(u32 self, u32 a, u32 b, u32 c, u32 d, u32 e) {
    WWHD_FUNC(0x0261CA68, u32, self, a, b, c, d, e);
    u32 scr = call<u32>(0x0273B050, 0x110u, load<u32>(load<u32>(0x1018C404) + 0x10), 4u);
    if (scr) scr = call<u32>(0x026FADB0, scr);
    store<u32>(self + 0x44, scr);
    if (!scr) return 0;
    if (!call_ptr<u32>(load<u32>(load<u32>(scr + 0xE0) + 0x14), scr, a, b, c, d, 6u, 3u, e)) return 0;
    for (u32 i = 0; i < 6; i++)
        call<void>(0x02004E04, anim(self), i, 0x1048E03C + 8 * i, 0x1048E024 + 8 * load<u32>(0x100E2E84 + 4 * i));
    vcall(self, 0x8C);
    return 1;
}
VERIFY(0x0261CA68, loadScreen);

/* 0261CB94: static initialiser: shared objects, layout/animation/pane names and two states */
void staticInit() {
    WWHD_FUNC(0x0261CB94, void);
    const u32 b = 0x1048E014;
    store<u32>(b + 0xC, 0);
    store<u32>(b + 8, 0);
    store<u32>(b + 4, 0);
    store<u32>(b, 0);
    call<void>(0x028F026C, 0x101F514Cu);
    const f32 lo = load<f32>(0x100E2F58), hi = load<f32>(0x100E2F5C);
    store<f32>(0x1048DFF8, lo);
    store<f32>(0x1048DFFC, hi);
    call<void>(0x028ED6F8, 0x1048E010u);
    call<void>(0x028F026C, 0x101F5158u);
    call<void>(0x028EAB2C, 0x1048E011u);
    call<void>(0x028F026C, 0x101F5164u);
    const f32 big = load<f32>(0x100E2F60), small = load<f32>(0x100E2F64);
    const u32 vt = 0x100E2E6C;
    const u32 A = 0x1048E024, B = 0x1048E03C, C = 0x1048E084, D = 0x1048E06C, L = 0x1048E09C, P = 0x1048E10C;
    store<f32>(0x1048E000, big);
    store<f32>(0x1048E008, small);
    store<u32>(A + 4, vt);
    store<u32>(A, 0x100E2FE0);
    store<u32>(A + 0xC, vt);
    store<u32>(A + 8, 0x100E2FF4);
    store<u32>(B + 4, vt);
    store<u32>(B, 0x100E2FC0);
    store<u32>(B + 0xC, vt);
    store<u32>(B + 8, 0x100E2F88);
    store<u32>(B + 0x14, vt);
    store<f32>(0x1048E004, big);
    store<u32>(B + 0x10, 0x100E3000);
    store<u32>(B + 0x1C, vt);
    store<u32>(B + 0x18, 0x100E2F68);
    store<u32>(B + 0x24, vt);
    store<u32>(B + 0x20, 0x100E2F70);
    store<u32>(B + 0x2C, vt);
    store<u32>(B + 0x28, 0x100E3010);
    store<u32>(C + 0x14, vt);
    store<u32>(D + 4, vt);
    store<u32>(D, 0x100E2F9C);
    store<u32>(D + 0xC, vt);
    store<u32>(D + 8, 0x100E2FA8);
    store<u32>(D + 0x14, vt);
    store<u32>(D + 0x10, 0x100E2FB4);
    store<f32>(0x1048E00C, small);
    store<u32>(A + 0x14, vt);
    store<u32>(C + 0xC, vt);
    store<u32>(A + 0x10, 0x100E2F78);
    store<u32>(L + 4, vt);
    store<u32>(P + 4, vt);
    const u32 guard = load<u32>(0x101FD8E4);
    store<u32>(C + 4, vt);
    store<u32>(C, 0x100E301C);
    store<u32>(C + 0x10, 0x100E301C);
    store<u32>(C + 8, 0x100E301C);
    store<u32>(L, 0x100E3030);
    store<u32>(L + 0xC, vt);
    store<u32>(P, 0x100E3030);
    store<u32>(L + 8, 0x100E301C);
    u32 id;
    if (guard) id = load<u32>(0x101FDD50);
    else {
        id = 0;
        store<u32>(0x101FD8E4, 1);
    }
    const u32 W = 0x1048E0AC, E = 0x1048E0DC;
    store<u16>(W + 0x24, 0);
    store<u32>(W + 0x28, 0);
    store<u16>(W + 0x1E, 0x1B);
    store<u16>(W + 0xE, 0x19);
    store<u16>(W + 0xC, 0);
    store<u32>(W + 8, 0x100E2EE4);
    store<u32>(W, id + 1);
    store<u32>(W + 0x10, 4);
    store<u16>(W + 0x1C, 0);
    store<u32>(W + 4, 0x100E303C);
    store<u16>(E + 0xC, 0);
    store<u32>(W + 0x2C, 0x101FF32C);
    store<u16>(E + 0xE, 0x1C);
    store<u32>(E + 4, 0x100E2FD0);
    store<u16>(W + 0x16, 0x1A);
    store<u16>(W + 0x26, 0);
    store<u32>(E + 0x2C, 0x101FF32C);
    store<u32>(E + 0x28, 0);
    store<u32>(E + 8, 0x100E2EE4);
    store<u32>(E + 0x20, 4);
    store<u32>(E + 0x18, 4);
    store<u32>(E + 0x10, 4);
    store<u32>(W + 0x20, 4);
    store<u16>(W + 0x14, 0);
    store<u32>(W + 0x18, 4);
    store<u32>(E, id + 2);
    store<u16>(E + 0x16, 0x1D);
    store<u16>(E + 0x14, 0);
    store<u16>(E + 0x26, 0);
    store<u16>(E + 0x1E, 0x1E);
    store<u16>(E + 0x24, 0);
    store<u32>(0x101FDD50, id + 2);
    store<u16>(E + 0x1C, 0);
}
VERIFY(0x0261CB94, staticInit);

/* empty virtual functions / state enter-exit functions */
void nop1() { WWHD_FUNC(0x0261CF60, void); }
VERIFY(0x0261CF60, nop1);
void nop2() { WWHD_FUNC(0x0261CF64, void); }
VERIFY(0x0261CF64, nop2);
void nop3() { WWHD_FUNC(0x0261CF80, void); }
VERIFY(0x0261CF80, nop3);
void nop4() { WWHD_FUNC(0x0261CF84, void); }
VERIFY(0x0261CF84, nop4);
void nop5() { WWHD_FUNC(0x0261CF90, void); }
VERIFY(0x0261CF90, nop5);
void nop6() { WWHD_FUNC(0x0261CF94, void); }
VERIFY(0x0261CF94, nop6);
void nop7() { WWHD_FUNC(0x0261CF98, void); }
VERIFY(0x0261CF98, nop7);

} // namespace hd_screen_auction_rupy

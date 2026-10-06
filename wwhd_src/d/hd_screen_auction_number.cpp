/* hd_screen_auction_number: HD auction counter digit screen (one rolling digit "AuctionNumber_00"
 * of the hd_screen_auction_rupy counter), WWHD. 
 * HD-only code (no GameCube source): written from the WWHD code. Range 0261D088..0261D8CF; static
 * initialiser 0261D5BC. Not verified here (sead library template instances): 0261D7B4/0261D7C8
 * (SafeString), 0261D7D0 (StateID getter), 0261D7E4/0261D824/0261D864 (state delegate invokers),
 * 0261D8A4 (StateID).
 *
 * Screen (0x5C): base 02700ADC (+0x18 state machine, +0x20 current state, +0x44 layout with its
 * animation at +0xD4), +4 vtable 100E3270, +0x2C/+0x30 vtables, +0x50 roll frame, +0x54 s16 shown
 * digit, +0x56 s16 target digit, +0x58 u8 rolling up. State 1048E140 (Roll); 1049DD60 is the
 * shared idle state.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_screen_auction_number {

static const u32 kIdle = 0x1049DD60, kRoll = 0x1048E140;

static u32 anim(u32 self) { return load<u32>(load<u32>(self + 0x44) + 0xD4); }

/* 0261D088: creates a digit screen for a layout part */
u32 create(u32 a, u32 b, u32 c) {
    WWHD_FUNC(0x0261D088, u32, a, b, c);
    u32 p = call<u32>(0x0273B050, 0x5Cu, load<u32>(load<u32>(0x1018C404) + 0x10), 4u);
    if (!p) return 0;
    p = call<u32>(0x0261D340, p);
    if (!p) return 0;
    call<u32>(0x0261D3C0, p, a, b, c);
    return p;
}
VERIFY(0x0261D088, create);

/* 0261D138: deleting destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x0261D138, void, p, flags);
    if (!p) return;
    store<u32>(p + 0x2C, 0x100E3350);
    store<u32>(p + 0x30, 0x100E3360);
    call<void>(0x02700A70, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x0261D138, dtor);

/* 0261D1A4: state Roll init */
void rollInit(u32 self) {
    WWHD_FUNC(0x0261D1A4, void, self);
    store<u32>(self + 0x50, 0);
}
VERIFY(0x0261D1A4, rollInit);

/* 0261D1B0: state Roll: turns the digit over three frames, then steps it (wrapping 0..9) */
void rollMove(u32 self) {
    WWHD_FUNC(0x0261D1B0, void, self);
    const u32 n = load<u32>(self + 0x50) + 1;
    s32 d = s16(load<u16>(self + 0x54));
    const f32 three = load<f32>(0x100E321C);
    store<u32>(self + 0x50, n);
    if (load<u8>(self + 0x58)) {
        const f32 t = f32(f64(n)) / three;
        call<void>(0x0200552C, anim(self), 0u, 0u, fadds_ppc(f32(f64(d)), t));
    } else {
        if (d == 0) d = 10;
        const f32 t = f32(f64(n)) / three;
        call<void>(0x0200552C, anim(self), 0u, 0u, fsubs_ppc(f32(f64(d)), t));
    }
    if (load<u32>(self + 0x50) < 3) return;
    const u32 up = load<u8>(self + 0x58);
    const s32 tgt = s16(load<u16>(self + 0x56));
    if (up) {
        d = s16(d + 1);
        if (d >= 10) {
            d = 0;
            store<u16>(self + 0x54, 0);
            store<u32>(self + 0x50, 0);
            if (u32(d) != u32(tgt)) return;
            call<void>(0x020063C0, self + 0x18, kIdle);
            return;
        }
    } else {
        d = s16(d - 1);
    }
    store<u16>(self + 0x54, u16(d));
    store<u32>(self + 0x50, 0);
    if (u32(d) != u32(tgt)) return;
    call<void>(0x020063C0, self + 0x18, kIdle);
}
VERIFY(0x0261D1B0, rollMove);

/* 0261D340: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x0261D340, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x5Cu);
        if (!p) return 0;
    }
    call<void>(0x02700ADC, p);
    store<u32>(p + 0x2C, 0x100E3350);
    store<u32>(p + 4, 0x100E3270);
    store<u32>(p + 0x30, 0x100E3360);
    store<u16>(p + 0x56, 0);
    store<u8>(p + 0x58, 0);
    store<u32>(p + 0x50, 0);
    store<u16>(p + 0x54, 0);
    return p;
}
VERIFY(0x0261D340, ctor);

/* 0261D3C0: screen setup and the idle state */
u32 setup(u32 self, u32 a, u32 b, u32 c) {
    WWHD_FUNC(0x0261D3C0, u32, self, a, b, c);
    if (!call<u32>(0x02700B48, self, 0x1048E170u, a, b, c)) return 0;
    const u32 st = load<u32>(self + 0x18);
    const u32 action = call_ptr<u32>(load<u32>(load<u32>(st) + 0x14), st, kIdle);
    store<u32>(self + 0x20, action);
    call_ptr<void>(load<u32>(load<u32>(action) + 0x1C), action);
    return 1;
}
VERIFY(0x0261D3C0, setup);

/* 0261D454: shows a digit at once */
void setDigit(u32 self, u32 d) {
    WWHD_FUNC(0x0261D454, void, self, d);
    call<void>(0x0200552C, anim(self), 0u, 0u, f32(f64(s32(d))));
}
VERIFY(0x0261D454, setDigit);

/* 0261D4A8: starts rolling from the old to the new digit */
void roll(u32 self, u32 from, u32 to, u32 up) {
    WWHD_FUNC(0x0261D4A8, void, self, from, to, up);
    if (from == to) return;
    store<u16>(self + 0x54, u16(from));
    store<u16>(self + 0x56, u16(to));
    store<u8>(self + 0x58, u8(up));
    call<void>(0x020063C0, self + 0x18, kRoll);
}
VERIFY(0x0261D4A8, roll);

/* 0261D4D0: loads the layout and binds its animation */
u32 loadScreen(u32 self, u32 a, u32 b, u32 c, u32 d, u32 e) {
    WWHD_FUNC(0x0261D4D0, u32, self, a, b, c, d, e);
    u32 scr = call<u32>(0x0273B050, 0x110u, load<u32>(load<u32>(0x1018C404) + 0x10), 4u);
    if (scr) scr = call<u32>(0x026FADB0, scr);
    store<u32>(self + 0x44, scr);
    if (!scr) return 0;
    if (!call_ptr<u32>(load<u32>(load<u32>(scr + 0xE0) + 0x14), scr, a, b, c, d, 1u, 1u, e)) return 0;
    call<void>(0x02004E04, anim(self), 0u, 0x1048E11Cu, 0x1048E114 + 8 * load<u32>(0x100E316C));
    return 1;
}
VERIFY(0x0261D4D0, loadScreen);

/* 0261D5BC: static initialiser: shared objects, names and the Roll state */
void staticInit() {
    WWHD_FUNC(0x0261D5BC, void);
    const u32 b = 0x1048E130;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F5170u);
    const f32 hi = load<f32>(0x100E3234), lo = load<f32>(0x100E3230);
    store<f32>(0x1048E128, hi);
    store<f32>(0x1048E124, lo);
    call<void>(0x028ED6F8, 0x1048E12Cu);
    call<void>(0x028F026C, 0x101F517Cu);
    call<void>(0x028EAB2C, 0x1048E12Du);
    call<void>(0x028F026C, 0x101F5188u);
    const u32 guard = load<u32>(0x101FD8E4);
    const u32 vt = 0x100E3170, X = 0x1048E11C, Y = 0x1048E114, Z = 0x1048E170;
    store<u32>(X + 4, vt);
    store<u32>(Y + 4, vt);
    store<u32>(Y, 0x100E3250);
    store<u32>(X, 0x100E3238);
    store<u32>(Z + 4, vt);
    store<u32>(Z, 0x100E325C);
    const u32 S = kRoll;
    u32 id;
    if (guard) id = load<u32>(0x101FDD50) + 1;
    else {
        id = 1;
        store<u32>(0x101FD8E4, 1);
    }
    store<u16>(S + 0xC, 0);
    store<u16>(S + 0xE, 0x19);
    store<u16>(S + 0x14, 0);
    store<u16>(S + 0x16, 0x1A);
    store<u16>(S + 0x1C, 0);
    store<u32>(S, id);
    store<u32>(S + 4, 0x100E3240);
    store<u32>(S + 0x10, 4);
    store<u32>(S + 0x18, 4);
    store<u32>(S + 0x20, 4);
    store<u32>(0x101FDD50, id);
    store<u32>(S + 0x28, 0);
    store<u32>(S + 8, 0x100E31D0);
    store<u16>(S + 0x1E, 0x1B);
    store<u16>(S + 0x24, 0);
    store<u16>(S + 0x26, 0);
    store<u32>(S + 0x2C, 0x101FF32C);
}
VERIFY(0x0261D5BC, staticInit);

/* empty state enter/exit functions */
void nop1() { WWHD_FUNC(0x0261D7CC, void); }
VERIFY(0x0261D7CC, nop1);
void nop2() { WWHD_FUNC(0x0261D7D8, void); }
VERIFY(0x0261D7D8, nop2);
void nop3() { WWHD_FUNC(0x0261D7DC, void); }
VERIFY(0x0261D7DC, nop3);
void nop4() { WWHD_FUNC(0x0261D7E0, void); }
VERIFY(0x0261D7E0, nop4);

} // namespace hd_screen_auction_number

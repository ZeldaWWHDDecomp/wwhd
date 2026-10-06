/* hd_input_misc: HD UI input receiver (pane hit tests and distances), two initialiser-only TUs and
 * the GamePad gyro/orientation reader, WWHD. HD-only code
 * (no GameCube source): written from the WWHD code. Range 02616ED4..0261758F.
 *
 * Receiver (0x60): base 02001A00 (event receiver), +0x14 vtable 100E28F8 (+0x14 hit test (x, y)),
 * +0x18 pane (its global matrix at +0x48), +0x58..+0x5A u8 flags, +0x5C user value.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_input_misc {

/* 02616ED4: receiver constructor */
u32 ctor(u32 p, u32 owner, u32 value) {
    WWHD_FUNC(0x02616ED4, u32, p, owner, value);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x60u);
        if (!p) return 0;
    }
    call<void>(0x02001A00, p, owner);
    store<u32>(p + 0x5C, value);
    store<u32>(p + 0x14, 0x100E28F8);
    store<u8>(p + 0x59, 0);
    store<u8>(p + 0x5A, 0);
    store<u8>(p + 0x58, 0);
    return p;
}
VERIFY(0x02616ED4, ctor);

/* 02616F58: receiver destructor */
void dtor(u32 p, u32 flags) {
    WWHD_FUNC(0x02616F58, void, p, flags);
    if (!p) return;
    call<void>(0x02001ABC, p, 0u);
    if (flags & 1) call<void>(0x0273AF40, p);
}
VERIFY(0x02616F58, dtor);

/* copies a pane's global matrix (0x30 bytes) to the stack */
static void copyMtx(u32 dst, u32 pane) {
    const u32 m = load<u32>(pane + 0x18) + 0x48;
    for (u32 i = 0; i < 12; i++) store<u32>(dst + 4 * i, load<u32>(m + 4 * i));
}

/* 02616FAC: does this receiver's hit test contain a corner of the item's pane rectangle? */
u8 hitsCorner(u32 self, u32 item) {
    WWHD_FUNC(0x02616FAC, u8, self, item);
    Local<u8[0x40]> F;
    const u32 rect = F.a, mtx = F.a + 0x10;
    copyMtx(mtx, item);
    call<void>(0x02876368, load<u32>(item + 0x18), rect);
    const f32 tx = load<f32>(mtx + 0xC), ty = load<f32>(mtx + 0x1C);
    const f32 l = fadds_ppc(tx, load<f32>(rect));
    const f32 r = fadds_ppc(tx, load<f32>(rect + 8));
    const f32 b = fadds_ppc(ty, load<f32>(rect + 0xC));
    const f32 t = fadds_ppc(ty, load<f32>(rect + 4));
    const f32 xs[4] = {l, r, l, r}, ys[4] = {t, t, b, b};
    for (u32 i = 0; i < 4; i++)
        if (call_ptr<u32>(load<u32>(load<u32>(self + 0x14) + 0x14), self, xs[i], ys[i])) return 1;
    return 0;
}
VERIFY(0x02616FAC, hitsCorner);

/* 02617174: distance between the two panes' translations */
f32 paneDistance(u32 a, u32 b) {
    WWHD_FUNC(0x02617174, f32, a, b);
    Local<u8[0x60]> F;
    copyMtx(F.a, a);
    copyMtx(F.a + 0x30, b);
    const f32 dy = fsubs_ppc(load<f32>(F.a + 0x1C), load<f32>(F.a + 0x4C));
    const f32 dy2 = fmuls_ppc(dy, dy);
    const f32 dx = fsubs_ppc(load<f32>(F.a + 0xC), load<f32>(F.a + 0x3C));
    const f32 d2 = fmadds(dx, dx, dy2);
    f32 inv = load<f32>(0x100E28E0);
    if (d2 > inv) {
        const f64 e = frsqrte(d2); /* the estimate stays double in the FPR */
        const f32 e2 = f32(e * round25(e)); /* fmuls rounds frC to 25 bits */
        const f32 eh = f32(e * f64(load<f32>(0x100E28E4)));
        inv = fmuls_ppc(fnmsubs(e2, d2, load<f32>(0x100E28E8)), eh);
    }
    return fmuls_ppc(inv, d2);
}
VERIFY(0x02617174, paneDistance);

u8 flag58(u32 p) { WWHD_FUNC(0x02617270, u8, p); return load<u8>(p + 0x58); }
VERIFY(0x02617270, flag58);
u8 flag59(u32 p) { WWHD_FUNC(0x02617278, u8, p); return load<u8>(p + 0x59); }
VERIFY(0x02617278, flag59);
u8 flag5A(u32 p) { WWHD_FUNC(0x02617280, u8, p); return load<u8>(p + 0x5A); }
VERIFY(0x02617280, flag5A);

static void stdInit(u32 b, u32 r1, u32 lo, u32 hi, u32 f, u32 r2, u32 r3) {
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, r1);
    const f32 a = load<f32>(lo), c = load<f32>(hi);
    store<f32>(f, a);
    store<f32>(f + 4, c);
    call<void>(0x028ED6F8, f + 8);
    call<void>(0x028F026C, r2);
    call<void>(0x028EAB2C, f + 9);
    call<void>(0x028F026C, r3);
}
void staticInit() {
    WWHD_FUNC(0x02617288, void);
    stdInit(0x1048DE20, 0x101F4FF8, 0x100E28EC, 0x100E28F0, 0x1048DE14, 0x101F5004, 0x101F5010);
}
VERIFY(0x02617288, staticInit);
void staticInit2() {
    WWHD_FUNC(0x0261731C, void);
    stdInit(0x1048DE3C, 0x101F501C, 0x100E2950, 0x100E2954, 0x1048DE30, 0x101F5028, 0x101F5034);
}
VERIFY(0x0261731C, staticInit2);

/* 026173B0: GamePad orientation: the three axis vectors of controller 8 (DRC) and their matrix */
void padOrientation(u32 out) {
    WWHD_FUNC(0x026173B0, void, out);
    const u32 ctl = call<u32>(0x0273CB8C, load<u32>(0x101F8AE8), 8u);
    if (!load<u32>(0x101FD694)) {
        store<u32>(0x101FD694, 1);
        store<u32>(0x101FD968, 0x100E2968);
    }
    if (!ctl) return;
    if (!call_ptr<u32>(load<u32>(load<u32>(ctl + 0x10) + 0xC), ctl, 0x101FD968u)) return;
    if (!ctl) return;
    if (s32(load<u32>(ctl + 0xAD4)) <= 0) return;
    const u32 m = ctl + 0x14;
    Local<f32[9]> V;
    const u32 v = V.a;
    const u32 so[9] = {0x6C, 0x70, 0x74, 0x78, 0x7C, 0x80, 0x84, 0x88, 0x8C};
    for (u32 i = 0; i < 9; i++) store<f32>(v + 4 * i, load<f32>(m + so[i]));
    store<f32>(out + 4, load<f32>(v + 4));
    store<f32>(out + 8, load<f32>(v + 8));
    store<f32>(out + 0x10, load<f32>(v + 0x10));
    store<f32>(out, load<f32>(v));
    store<f32>(out + 0x14, load<f32>(v + 0x14));
    store<f32>(out + 0xC, load<f32>(v + 0xC));
    store<f32>(out + 0x18, load<f32>(v + 0x18));
    store<f32>(out + 0x20, load<f32>(v + 0x20));
    store<f32>(out + 0x1C, load<f32>(v + 0x1C));
    call<void>(0x026175B4, out + 0x24, v, v + 0xC, v + 0x18);
}
VERIFY(0x026173B0, padOrientation);

void staticInit3() {
    WWHD_FUNC(0x026174FC, void);
    stdInit(0x1048DE58, 0x101F5040, 0x100E2978, 0x100E297C, 0x1048DE4C, 0x101F504C, 0x101F5058);
}
VERIFY(0x026174FC, staticInit3);

} // namespace hd_input_misc

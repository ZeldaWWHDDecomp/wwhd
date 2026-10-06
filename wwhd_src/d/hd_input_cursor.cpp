/* hd_input_cursor: HD swipe/stick cursor direction detector (5-sample touch history, inertia,
 * four-way direction), WWHD. HD-only code (no GameCube
 * source): written from the WWHD code. Range 0261A198..0261AC83 (sinit 0261A9BC with the
 * "Icon=CONTROLLER" names; 0261ABF0 is the initialiser of a TU without code). Not verified here
 * (sead library): 0261AA88/0261AA9C (SafeString vtable members), 0261AAA0 (Vector2 normalize),
 * 0261AB28 (Vector2 chase).
 *
 * Detector (0x5C): +0 vtable 100E2B54, +4 direction (0 none, 1..4), +8 history of 5 Vec2, +0x30 u8
 * active, +0x34 cursor Vec2, +0x3C inertia factor, +0x40 inertia frames, +0x44 frames left,
 * +0x48/+0x4C scale, +0x50 return rate, +0x54 unused 0.001, +0x58 direction threshold.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_input_cursor {

/* 0261A198: constructor */
u32 ctor(u32 p) {
    WWHD_FUNC(0x0261A198, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x5Cu);
        if (!p) return 0;
    }
    store<u32>(p + 4, 0);
    store<u32>(p, 0x100E2B54);
    if (!(p + 8)) call<u32>(0x0273AD10, 0x28u);
    store<u8>(p + 0x30, 0);
    const f32 z = load<f32>(0x100E2B08);
    u32 v = p + 0x34;
    if (!v) v = call<u32>(0x0273AD10, 8u);
    if (v) {
        store<f32>(v + 4, z);
        store<f32>(v, z);
    }
    const f32 one = load<f32>(0x100E2B14), half = load<f32>(0x100E2B0C);
    store<f32>(p + 0x3C, one);
    store<f32>(p + 0x54, load<f32>(0x100E2B68));
    store<f32>(p + 0x48, one);
    store<u32>(p + 0x40, 0xA);
    store<f32>(p + 0x58, half);
    store<u32>(p + 0x44, 0);
    store<f32>(p + 0x50, load<f32>(0x100E2B64));
    store<f32>(p + 0x4C, one);
    for (u32 i = 0; i < 5; i++) {
        store<f32>(p + 8 + 8 * i, z);
        store<f32>(p + 0xC + 8 * i, z);
    }
    return p;
}
VERIFY(0x0261A198, ctor);

/* sqrt of a squared length as GHS inlines it (one Newton step on frsqrte) */
static f32 approxLen(f32 l2, f32 z, f32 half, f32 three) {
    if (!(l2 > z)) return fmuls_ppc(z, l2);
    const f64 e = frsqrte(l2);
    const f32 e2 = f32(e * round25(e));
    const f32 eh = f32(e * round25(half));
    const f32 t = fnmsubs(e2, l2, three);
    return fmuls_ppc(fmuls_ppc(t, eh), l2);
}

/* 0261A2C0: update with the current touch/stick input (0,0 = released) */
void update(u32 self, u32 in) {
    WWHD_FUNC(0x0261A2C0, void, self, in);
    const f32 z = load<f32>(0x100E2B08), three = load<f32>(0x100E2B10), half = load<f32>(0x100E2B0C);
    const u32 h = self + 8;
    const f32 ix = load<f32>(in);
    if (!(ix == z) || !(load<f32>(in + 4) == z)) {
        const f32 lx = load<f32>(self + 0x28);
        store<u8>(self + 0x30, 1);
        if (lx == z && load<f32>(self + 0x2C) == z) store<u32>(self + 0x44, load<u32>(self + 0x40));
        for (u32 i = 0; i < 4; i++) {
            store<f32>(h + 8 * i, load<f32>(h + 8 * (i + 1)));
            store<f32>(h + 8 * i + 4, load<f32>(h + 8 * (i + 1) + 4));
        }
        Local<u32[2]> w;
        store<u32>(w.a, load<u32>(in));
        const f32 nx = load<f32>(w.a) / load<f32>(0x100E2B6C);
        store<u32>(w.a + 4, load<u32>(in + 4));
        const f32 ny = load<f32>(w.a + 4) / load<f32>(0x100E2B70);
        store<f32>(self + 0x28, nx);
        store<f32>(self + 0x2C, ny);
        f32 ax = z, ay = z;
        for (u32 i = 0; i < 4; i++) {
            const u32 c = h + 8 * i, n = h + 8 * (i + 1);
            if (load<f32>(c) == z && load<f32>(c + 4) == z) continue;
            const f32 dx = fsubs_ppc(load<f32>(n), load<f32>(c));
            const f32 dy = fsubs_ppc(load<f32>(n + 4), load<f32>(c + 4));
            ax = fadds_ppc(ax, dx);
            ay = fadds_ppc(ay, dy);
        }
        f32 vx = fmuls_ppc(ax, load<f32>(self + 0x48));
        const s32 left = s32(load<u32>(self + 0x44));
        const f32 cy0 = load<f32>(self + 0x38), cx0 = load<f32>(self + 0x34);
        f32 vy = fmuls_ppc(ay, load<f32>(self + 0x4C));
        if (left > 0) {
            const f32 d = load<f32>(self + 0x3C);
            vx = fmuls_ppc(vx, d);
            store<u32>(self + 0x44, u32(left - 1));
            vy = fmuls_ppc(vy, d);
        }
        f32 cx = fadds_ppc(cx0, vx);
        const f32 one = load<f32>(0x100E2B14);
        f32 cy = fadds_ppc(cy0, vy);
        store<f32>(self + 0x34, cx);
        store<f32>(self + 0x38, cy);
        if (cx > one) cx = one;
        store<f32>(self + 0x34, cx);
        if (cy > one) cy = one;
        const f32 l2 = fmadds(cx, cx, fmuls_ppc(cy, cy));
        store<f32>(self + 0x38, cy);
        if (approxLen(l2, z, half, three) > one) call<void>(0x0261AAA0, self + 0x34);
    } else {
        store<u8>(self + 0x30, 0);
        for (u32 i = 0; i < 5; i++) {
            store<f32>(h + 8 * i, z);
            store<f32>(h + 8 * i + 4, z);
        }
        const f32 rate = load<f32>(self + 0x50);
        Local<f32[2]> t;
        store<f32>(t.a, z);
        store<f32>(t.a + 4, z);
        store<u32>(self + 4, 0);
        call<u32>(0x0261AB28, self + 0x34, t.a, rate);
    }
    const f32 y2 = load<f32>(self + 0x38), x2 = load<f32>(self + 0x34);
    const f32 l2 = fmadds(x2, x2, fmuls_ppc(y2, y2));
    if (!(approxLen(l2, z, half, three) > load<f32>(self + 0x58))) {
        store<u32>(self + 4, 0);
        return;
    }
    const f32 x = load<f32>(self + 0x34), y = load<f32>(self + 0x38);
    if (x == z && y == z) return;
    u32 a;
    if (!(x < z)) {
        if (!(y < z)) {
            if (!(x < y)) a = call<u32>(0x02756D88, y / x);
            else a = 0x40000000u - call<u32>(0x02756D88, x / y);
        } else {
            if (!(x < -y)) a = 0u - call<u32>(0x02756D88, -(y / x));
            else a = call<u32>(0x02756D88, -(x / y)) - 0x40000000u;
        }
    } else {
        if (!(y < z)) {
            if (!(-x < y)) a = 0x80000000u - call<u32>(0x02756D88, -(y / x));
            else a = call<u32>(0x02756D88, -(x / y)) + 0x40000000u;
        } else {
            if (x > y) a = 0u - call<u32>(0x02756D88, x / y) - 0x40000000u;
            else a = call<u32>(0x02756D88, y / x) - 0x80000000u;
        }
    }
    u32 dir;
    if (a <= 0x2AAAAAAAu) {
        if (!a) return;
        dir = 2;
    } else if (a <= 0x55555554u) dir = 1;
    else if (a <= 0xAAAAAAA8u) dir = 4;
    else if (a <= 0xD5555552u) dir = 3;
    else dir = 2;
    store<u32>(self + 4, dir);
}
VERIFY(0x0261A2C0, update);

/* 0261A9B8: forwards to update */
void updateThunk(u32 self, u32 in) {
    WWHD_FUNC(0x0261A9B8, void, self, in);
    update(self, in);
}
VERIFY(0x0261A9B8, updateThunk);

/* 0261A9BC: static initialiser (shared objects and the two icon names) */
void staticInit() {
    WWHD_FUNC(0x0261A9BC, void);
    const u32 b = 0x1048DED8;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F50E0u);
    const f32 lo = load<f32>(0x100E2B78), hi = load<f32>(0x100E2B7C);
    store<f32>(0x1048DECC, lo);
    store<f32>(0x1048DED0, hi);
    call<void>(0x028ED6F8, 0x1048DED4u);
    call<void>(0x028F026C, 0x101F50ECu);
    call<void>(0x028EAB2C, 0x1048DED5u);
    call<void>(0x028F026C, 0x101F50F8u);
    store<u32>(0x1048DEC8, 0x100E2B1C);
    store<u32>(0x1048DEC4, 0x100E2B80);
    store<u32>(0x1048DEC0, 0x100E2B1C);
    store<u32>(0x1048DEBC, 0x100E2B90);
}
VERIFY(0x0261A9BC, staticInit);

/* 0261ABF0: initialiser of a TU without code */
void staticInit2() {
    WWHD_FUNC(0x0261ABF0, void);
    const u32 b = 0x1048DEF4;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F5104u);
    const f32 lo = load<f32>(0x100E2BA8), hi = load<f32>(0x100E2BAC);
    store<f32>(0x1048DEE8, lo);
    store<f32>(0x1048DEEC, hi);
    call<void>(0x028ED6F8, 0x1048DEF0u);
    call<void>(0x028F026C, 0x101F5110u);
    call<void>(0x028EAB2C, 0x1048DEF1u);
    call<void>(0x028F026C, 0x101F511Cu);
}
VERIFY(0x0261ABF0, staticInit2);

} // namespace hd_input_cursor

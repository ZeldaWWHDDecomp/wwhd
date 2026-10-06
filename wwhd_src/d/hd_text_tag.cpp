/* hd_text_tag: HD message tag processor base (font, scale and pictograph tags), WWHD.
 * HD-only code (no GameCube source): written from the WWHD
 * code. Range 025FA0F0..025FA5BF (static initialiser 025FA52C). hd_text_ruby (02601B60..) derives
 * from it.
 *
 * TagState (0x10): +0 u8 scale saved, +4/+8 f32 saved writer scale x/y, +0xC saved font.
 * Tag record: +2 u16 group, +4 u16 tag, +5 u8 (group 3: pictograph code), +6 u16 parameter size,
 * +8.. parameters (font id or scale percent, scale x/y percent at +0xA/+0xC; group 1 tag 0xC:
 * +0x10/+0x12 scale percent). Writer (nw4f TextWriter-like): +8 cursor x, +0xC/+0x10 scale,
 * +0x28 font. Font manager 101F4A50 (hd_font_mgr): font 4 is the pictograph font.
 */
#include "gabi.h"
using namespace gabi;

namespace hd_text_tag {

static f32 pct(u32 v) { return f32(f64(v)) / load<f32>(0x100E0F68); } /* u16 percent / 100 */
static f32 s32f(u32 v) { return f32(f64(s32(v))); }

/* 025FA0F0: TagState constructor */
u32 stateCtor(u32 p) {
    WWHD_FUNC(0x025FA0F0, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x10u);
        if (!p) return 0;
    }
    const f32 one = load<f32>(0x100E0F60);
    store<u8>(p, 0);
    store<f32>(p + 8, one);
    store<u32>(p + 0xC, 0);
    store<f32>(p + 4, one);
    return p;
}
VERIFY(0x025FA0F0, stateCtor);

/* 025FA140: font tag: switches the writer's font (0xFFFF: back to the saved one) and sets the scale
 * that gives the requested size in percent of the font's cell */
u32 fontTag(u32 st, u32 writer, u32 tag) {
    WWHD_FUNC(0x025FA140, u32, st, writer, tag);
    const u32 id = load<u16>(tag + 8);
    u32 f;
    if (id == 0xFFFF) {
        f = load<u32>(st + 0xC);
        if (!f) {
            f = load<u32>(writer + 0x28);
            if (!f) return 0;
        }
    } else {
        f = call<u32>(0x025F36B8, load<u32>(0x101F4A50), id);
        store<u32>(st + 0xC, load<u32>(writer + 0x28));
        if (!f) return 0;
    }
    store<u32>(writer + 0x28, f);
    if (load<u16>(tag + 6) < 6) return 0;
    const u32 vt = load<u32>(f + 4);
    f32 sx = pct(load<u16>(tag + 0xA));
    const f32 sy = pct(load<u16>(tag + 0xC));
    sx = sx / s32f(call_ptr<u32>(load<u32>(vt + 0x1C), f));
    const f32 h = s32f(call_ptr<u32>(load<u32>(load<u32>(f + 4) + 0x24), f));
    const f32 y = sy / h;
    store<f32>(writer + 0xC, sx);
    store<f32>(writer + 0x10, y);
    return 0;
}
VERIFY(0x025FA140, fontTag);

/* 025FA2CC: scale tag: scales the saved writer scale by a percentage */
u32 scaleTag(u32 st, u32 writer, u32 tag) {
    WWHD_FUNC(0x025FA2CC, u32, st, writer, tag);
    if (!load<u8>(st)) {
        const f32 x = load<f32>(writer + 0xC);
        store<f32>(st + 4, x);
        const f32 y = load<f32>(writer + 0x10);
        store<u8>(st, 1);
        store<f32>(st + 8, y);
    }
    const f32 s = pct(load<u16>(tag + 8));
    const f32 y = load<f32>(st + 8), x = load<f32>(st + 4);
    store<f32>(writer + 0xC, fmuls_ppc(x, s));
    store<f32>(writer + 0x10, fmuls_ppc(y, s));
    return 1;
}
VERIFY(0x025FA2CC, scaleTag);

/* 025FA348: tag group 0 (1 font, 2 scale) */
u32 group0(u32 st, u32 writer, u32 tag) {
    WWHD_FUNC(0x025FA348, u32, st, writer, tag);
    const u32 t = load<u16>(tag + 4);
    if (t == 1) return fontTag(st, writer, tag);
    if (t == 2) return scaleTag(st, writer, tag);
    return 1;
}
VERIFY(0x025FA348, group0);

/* 025FA36C: absolute scale (percent x/y) */
u32 setScale(u32 st, u32 writer, u32 tag) {
    WWHD_FUNC(0x025FA36C, u32, st, writer, tag);
    const f32 x = pct(load<u16>(tag + 0x10));
    const f32 y = pct(load<u16>(tag + 0x12));
    store<f32>(writer + 0xC, x);
    store<f32>(writer + 0x10, y);
    return 1;
}
VERIFY(0x025FA36C, setScale);

/* 025FA3CC: tag group 1 (0xC: absolute scale) */
u32 group1(u32 st, u32 writer, u32 tag) {
    WWHD_FUNC(0x025FA3CC, u32, st, writer, tag);
    if (load<u16>(tag + 4) == 0xC) setScale(st, writer, tag);
    return 1;
}
VERIFY(0x025FA3CC, group1);

/* 025FA3FC: pictograph tag: advances the line by the pictograph glyph's width */
u32 pictTag(u32 st, u32 writer, u32 ctx, u32 tag) {
    WWHD_FUNC(0x025FA3FC, u32, st, writer, ctx, tag);
    const u32 f = call<u32>(0x025F36B8, load<u32>(0x101F4A50), 4u);
    const u32 code = call<u32>(0x025F90C8, load<u32>(0x101F4A50) + 0x2C, u32(load<u8>(tag + 5)));
    Local<u8[0x18]> G;
    call<void>(0x0286EBEC, G.a);
    call_ptr<void>(load<u32>(load<u32>(f + 4) + 0x8C), f, G.a, code);
    const f32 w = f32(f64(load<u8>(G.a + 6)));
    const f32 x = fmadds(w, load<f32>(ctx + 0xC), load<f32>(ctx + 0x14));
    store<f32>(ctx + 0x14, x);
    store<f32>(writer + 8, x);
    return 1;
}
VERIFY(0x025FA3FC, pictTag);

/* 025FA4CC: tag dispatch by group (0, 1, 3) */
u32 dispatch(u32 st, u32 writer, u32 ctxp, u32 tag) {
    WWHD_FUNC(0x025FA4CC, u32, st, writer, ctxp, tag);
    const u32 g = load<u16>(tag + 2);
    const u32 ctx = load<u32>(ctxp);
    if (g < 1) return group0(st, ctx, tag);
    if (g == 1) return group1(st, ctx, tag);
    if (g == 3) return pictTag(st, writer, ctx, tag);
    return 1;
}
VERIFY(0x025FA4CC, dispatch);

/* 025FA50C: TagState reset */
void stateReset(u32 p) {
    WWHD_FUNC(0x025FA50C, void, p);
    const f32 one = load<f32>(0x100E0F60);
    store<u8>(p, 0);
    store<f32>(p + 4, one);
    store<u32>(p + 0xC, 0);
    store<f32>(p + 8, one);
}
VERIFY(0x025FA50C, stateReset);

void staticInit() {
    WWHD_FUNC(0x025FA52C, void);
    const u32 b = 0x1048D870;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4BE0u);
    const f32 lo = load<f32>(0x100E0F80), hi = load<f32>(0x100E0F84);
    store<f32>(0x1048D864, lo);
    store<f32>(0x1048D868, hi);
    call<void>(0x028ED6F8, 0x1048D86Cu);
    call<void>(0x028F026C, 0x101F4BECu);
    call<void>(0x028EAB2C, 0x1048D86Du);
    call<void>(0x028F026C, 0x101F4BF8u);
}
VERIFY(0x025FA52C, staticInit);

} // namespace hd_text_tag

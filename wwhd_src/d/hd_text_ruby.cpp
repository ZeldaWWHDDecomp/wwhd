/* hd_text_ruby: HD message tag processor (ruby/furigana, font, scale, colour tags), WWHD.
 * HD-only code (no GameCube source): written from the
 * WWHD code. Range 02601B60..026025CB; static initialiser 02602518. The two trailing sead
 * SafeString companions 026025AC/026025B0 are library (not verified here).
 *
 * Layouts (from the code):
 *   TagState (0x10): +0 u8 scaleSaved, +4/+8 f32 saved writer scale x/y, +0xC saved font
 *   tag record: +2 u16 group, +4 u16 tag, +5 u8 (group 3 value), +6 u16 param size, +8.. params
 *   writer (nw4f TextWriter-like, 0x4C copied): +0/+4 colours, +0xC/+0x10 scale, +0x14/+0x18
 *   cursor, +0x28 font, +0x48 copied into the ruby writer
 *   sead WSafeString temporaries {top, vtable}; WFixedSafeString<32> {top, vtable, 0x20, buf[32]}
 */
#include "gabi.h"
using namespace gabi;

namespace hd_text_ruby {

static const u32 kWSafeStringVt = 0x100E17A8;
static const u32 kWFixedSafeStringVt = 0x100E17D8;
static const u32 kFontMgr = 0x101F4A50;

/* 02601B60: TagState constructor */
u32 tagStateCtor(u32 p) {
    WWHD_FUNC(0x02601B60, u32, p);
    if (!p) {
        p = call<u32>(0x0273AD10, 0x10u);
        if (!p) return 0;
    }
    const f32 one = load<f32>(0x100E17A0);
    store<u8>(p, 0);
    store<f32>(p + 8, one);
    store<u32>(p + 0xC, 0);
    store<f32>(p + 4, one);
    return p;
}
VERIFY(0x02601B60, tagStateCtor);

/* 02601BB0: does the text contain a ruby control character (0x0E/0x0F)? */
u32 hasControlChar(u32 self, u32 text, s32 len) {
    WWHD_FUNC(0x02601BB0, u32, self, text, len);
    if (len <= 0) return 0;
    for (s32 i = 0; i < len; i++) {
        const u32 c = load<u16>(text + 2 * i);
        if (c == 0xE || c == 0xF) return 1;
    }
    return 0;
}
VERIFY(0x02601BB0, hasControlChar);

/* strlen of a wide SafeString, capped as sead does (0x40000) */
static s32 wideLength(u32 top) {
    s32 n = 0;
    if (!load<u16>(top)) return 0;
    for (;;) {
        n++;
        if (n > 0x40000) return 0;
        top += 2;
        if (!load<u16>(top)) return n;
    }
}

/* copies len (clamped to the buffer) wide chars of the SafeString at frame+8 into a fixed string */
static void copyInto(u32 frame, u32 fixedOff, s32 len) {
    const u32 buf = load<u32>(frame + fixedOff);
    s32 n = len;
    if (len < 0) {
        call<void>(0x026025AC, frame + 8);
        n = wideLength(load<u32>(frame + 8));
    }
    const s32 size = s32(load<u32>(frame + fixedOff + 8));
    if (n >= size) n = size - 1;
    call_ptr<void>(load<u32>(load<u32>(frame + 0xC) + 0x14), frame + 8);
    call<void>(0xC0009988, buf, load<u32>(frame + 8), u32(n * 2), 0u);
    store<u16>(buf + 2 * n, 0);
}

/* 02601BE8: draws the ruby text above the base text */
void drawRuby(u32 self, u32 writer, u32 tag, u32 text) {
    WWHD_FUNC(0x02601BE8, void, self, writer, tag, text);
    const s32 baseLen = s32(load<u16>(tag + 8) >> 1);
    const s32 rubyLen = s32(load<u16>(tag + 0xA) >> 1);
    const u32 cfg = load<u32>(0x101F4B5C);
    if (u32(baseLen - 1) >= 0x1F || u32(rubyLen - 1) >= 0x1F) return;
    Local<u8[0x100]> F;
    const u32 f = F.a;
    /* ruby string: WFixedSafeString<32> at +0x68, source SafeString at +8 */
    store<u32>(f + 0x68, f + 0x74);
    store<u32>(f + 0x70, 0x20);
    store<u32>(f + 0x6C, kWFixedSafeStringVt);
    store<u16>(f + 0xB2, 0);
    store<u16>(f + 0x74, 0);
    store<u32>(f + 0xC, kWSafeStringVt);
    store<u32>(f + 8, tag + 0xC);
    copyInto(f, 0x68, rubyLen);
    /* base string: WFixedSafeString<32> at +0xB4 */
    store<u32>(f + 8, text);
    store<u16>(f + 0xFE, 0);
    store<u16>(f + 0xC0, 0);
    store<u32>(f + 0xB8, kWFixedSafeStringVt);
    store<u32>(f + 0xBC, 0x20);
    store<u32>(f + 0xC, kWSafeStringVt);
    store<u32>(f + 0xB4, f + 0xC0);
    copyInto(f, 0xB4, baseLen);

    call<void>(0x0286E3C4, writer);
    call<void>(0x0286E42C, writer);
    const u32 font = load<u32>(writer + 0x28);
    const s32 lineHeight = call_ptr<s32>(load<u32>(load<u32>(font + 4) + 0x2C), font);
    /* the ruby writer: a copy of the base writer with the ruby font, smaller scale and spacing */
    const u32 rw = f + 0x1C;
    for (u32 i = 0; i < 0x4C; i += 4) store<u32>(rw + i, load<u32>(writer + i));
    store<u32>(rw + 0x28, call<u32>(0x025F36B8, load<u32>(kFontMgr), 2u));
    const f32 k = load<f32>(cfg + 0x974);
    f32 sx = fmuls_ppc(load<f32>(rw + 0xC), k);
    const f32 maxScale = fmuls_ppc(k, load<f32>(0x100E17F0));
    f32 sy = fmuls_ppc(load<f32>(rw + 0x10), k);
    if (sx > maxScale) sx = maxScale; /* ble: branch when not greater */
    if (sy > maxScale) sy = maxScale;
    store<f32>(rw + 0x10, sy);
    store<f32>(rw + 0xC, sx);
    const f32 spacing = load<f32>(cfg + 0x97C);
    const f32 zero = load<f32>(0x100E17F4);
    store<f32>(rw + 0x38, spacing);
    store<f32>(rw + 0x3C, zero);
    store<u32>(rw + 0x44, 0x300);
    store<u32>(rw + 0x48, load<u32>(writer + 0x48));
    call_ptr<void>(load<u32>(load<u32>(f + 0xB8) + 0x14), f + 0xB4);
    const f32 baseWidth = call<f32>(0x028706A0, writer, load<u32>(f + 0xB4), baseLen);
    call_ptr<void>(load<u32>(load<u32>(f + 0x6C) + 0x14), f + 0x68);
    const f32 rubyWidth = call<f32>(0x028706A0, rw, load<u32>(f + 0x68), rubyLen);
    const f32 diff = fsubs_ppc(baseWidth, rubyWidth);
    if (diff > zero) {
        const f32 d = diff / f32(rubyLen + 1);
        store<f32>(rw + 0x14, fadds_ppc(load<f32>(rw + 0x14), d));
        store<f32>(rw + 0x38, fadds_ppc(d, spacing));
    } else {
        store<f32>(rw + 0x14, fmadds(diff, load<f32>(0x100E1800), load<f32>(rw + 0x14)));
    }
    const f32 h = call<f32>(0x0286E494, writer) / f32(lineHeight);
    const f32 rise = load<f32>(cfg + 0x978);
    const f32 h2 = call<f32>(0x0286E494, writer);
    store<f32>(rw + 0x18, fadds_ppc(load<f32>(rw + 0x18), fmsubs(rise, h, h2)));
    const u32 col = tag + 2 * rubyLen;
    const u32 c = (u32(load<u8>(col + 0xD)) << 24) | (u32(load<u8>(col + 0xC)) << 16) |
                  (u32(load<u8>(col + 0xF)) << 8) | load<u8>(col + 0xE);
    store<u32>(rw, c);
    store<u32>(rw + 4, c);
    call_ptr<void>(load<u32>(load<u32>(f + 0x6C) + 0x14), f + 0x68);
    call<void>(0x028710D0, rw, load<u32>(f + 0x68), rubyLen);
    call<void>(0x0286FED0, rw, 2u);
}
VERIFY(0x02601BE8, drawRuby);

/* 02602038: ruby tag: draws the ruby unless the base text has control characters */
u32 tagRuby(u32 self, u32 writer, u32 tag, u32 text) {
    WWHD_FUNC(0x02602038, u32, self, writer, tag, text);
    if (!hasControlChar(self, text, s32(load<u16>(tag + 8) >> 1))) drawRuby(self, writer, tag, text);
    return 1;
}
VERIFY(0x02602038, tagRuby);

/* 0260208C: font tag (0xFFFF restores the saved font); optional size parameters */
u32 tagFont(u32 self, u32 writer, u32 tag) {
    WWHD_FUNC(0x0260208C, u32, self, writer, tag);
    const u32 id = load<u16>(tag + 8);
    u32 font;
    if (id == 0xFFFF) {
        font = load<u32>(self + 0xC);
        store<u32>(self + 0xC, 0);
    } else {
        font = call<u32>(0x025F36B8, load<u32>(kFontMgr), id);
        if (!load<u32>(self + 0xC)) store<u32>(self + 0xC, load<u32>(writer + 0x28));
    }
    store<u32>(writer + 0x28, font);
    if (load<u16>(tag + 6) >= 6) {
        const f32 h = f32(load<u16>(tag + 0xA)), w = f32(load<u16>(tag + 0xC));
        const f32 pc = load<f32>(0x100E1804);
        const f32 y = w / pc;
        const f32 x = h / pc;
        call<void>(0x0286E2E4, writer, x, y);
    }
    return 0;
}
VERIFY(0x0260208C, tagFont);

/* 0260216C: scale tag (percent of the scale saved at the first scale tag) */
u32 tagScale(u32 self, u32 writer, u32 tag) {
    WWHD_FUNC(0x0260216C, u32, self, writer, tag);
    if (!load<u8>(self)) {
        store<f32>(self + 4, load<f32>(writer + 0xC));
        const f32 y = load<f32>(writer + 0x10);
        store<u8>(self, 1);
        store<f32>(self + 8, y);
    }
    const f32 s = f32(load<u16>(tag + 8)) / load<f32>(0x100E1804);
    const f32 y = load<f32>(self + 8), x = load<f32>(self + 4);
    store<f32>(writer + 0xC, fmuls_ppc(x, s));
    store<f32>(writer + 0x10, fmuls_ppc(y, s));
    return 1;
}
VERIFY(0x0260216C, tagScale);

static u32 tagColour(u32 b) {
    return (u32(load<u8>(b + 1)) << 24) | (u32(load<u8>(b)) << 16) | (u32(load<u8>(b + 3)) << 8) | load<u8>(b + 2);
}

/* 026021E8: colour tag (both writer colours) */
u32 tagColor(u32 self, u32 writer, u32 tag) {
    WWHD_FUNC(0x026021E8, u32, self, writer, tag);
    const u32 c = tagColour(tag + 8);
    store<u32>(writer, c);
    store<u32>(writer + 4, c);
    return 1;
}
VERIFY(0x026021E8, tagColor);

/* 0260223C: system tag group dispatch */
u32 tagSystem(u32 self, u32 writer, u32 tag, u32 text) {
    WWHD_FUNC(0x0260223C, u32, self, writer, tag, text);
    switch (load<u16>(tag + 4)) {
    case 0: return tagRuby(self, writer, tag, text);
    case 1: return tagFont(self, writer, tag);
    case 2: return tagScale(self, writer, tag);
    case 3: return tagColor(self, writer, tag);
    }
    return 1;
}
VERIFY(0x0260223C, tagSystem);

/* 02602270: two colours and scale */
u32 tagColorScale(u32 self, u32 writer, u32 tag) {
    WWHD_FUNC(0x02602270, u32, self, writer, tag);
    store<u32>(writer, tagColour(tag + 8));
    store<u32>(writer + 4, tagColour(tag + 0xC));
    const f32 pc = load<f32>(0x100E1804);
    const f32 x = f32(load<u16>(tag + 0x10)), y = f32(load<u16>(tag + 0x12));
    store<f32>(writer + 0xC, x / pc);
    store<f32>(writer + 0x10, y / pc);
    return 1;
}
VERIFY(0x02602270, tagColorScale);

/* 02602334: group 1 dispatch */
u32 tagGroup1(u32 self, u32 writer, u32 tag) {
    WWHD_FUNC(0x02602334, u32, self, writer, tag);
    const u32 t = load<u16>(tag + 4);
    if (t == 0xC) return tagColorScale(self, writer, tag);
    if (t == 0xD) return 4;
    return 1;
}
VERIFY(0x02602334, tagGroup1);

/* 0260237C: group 3: prints a string from the message manager with font 4, raised by its ascent */
u32 tagGroup3(u32 self, u32 writer, u32 tag) {
    WWHD_FUNC(0x0260237C, u32, self, writer, tag);
    const u32 oldFont = load<u32>(writer + 0x28);
    const u32 font = call<u32>(0x025F36B8, load<u32>(kFontMgr), 4u);
    const u32 str = call<u32>(0x025F90C8, load<u32>(kFontMgr) + 0x2C, u32(load<u8>(tag + 5)));
    const u32 c0 = load<u32>(writer), c1 = load<u32>(writer + 4);
    store<u32>(writer + 0x28, font);
    const s32 asc = call_ptr<s32>(load<u32>(load<u32>(font + 4) + 0xAC), font);
    const f32 dy = fmuls_ppc(f32(-asc), load<f32>(writer + 0x10));
    store<f32>(writer + 0x18, fadds_ppc(load<f32>(writer + 0x18), dy));
    store<u32>(writer, 0xFFFFFFFF);
    store<u32>(writer + 4, 0xFFFFFFFF);
    call<void>(0x0286E9F0, writer, str);
    const f32 y = load<f32>(writer + 0x18);
    store<u32>(writer + 0x28, oldFont);
    store<f32>(writer + 0x18, fsubs_ppc(y, dy));
    store<u32>(writer, c0);
    store<u32>(writer + 4, c1);
    return 1;
}
VERIFY(0x0260237C, tagGroup3);

/* 026024C8: tag dispatch (context, tag, text following the tag) */
u32 process(u32 self, u32 ctx, u32 tag, u32 text) {
    WWHD_FUNC(0x026024C8, u32, self, ctx, tag, text);
    const u32 g = load<u16>(tag + 2);
    const u32 writer = load<u32>(ctx);
    if (g == 0) return tagSystem(self, writer, tag, text);
    if (g == 1) return tagGroup1(self, writer, tag);
    if (g == 3) return tagGroup3(self, writer, tag);
    return 1;
}
VERIFY(0x026024C8, process);

/* 026024F8: TagState reset */
void tagStateReset(u32 p) {
    WWHD_FUNC(0x026024F8, void, p);
    const f32 one = load<f32>(0x100E17A0);
    store<u8>(p, 0);
    store<f32>(p + 4, one);
    store<u32>(p + 0xC, 0);
    store<f32>(p + 8, one);
}
VERIFY(0x026024F8, tagStateReset);

/* 02602518: static initialiser */
void staticInit() {
    WWHD_FUNC(0x02602518, void);
    const u32 b = 0x1048DCCC;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4E98u);
    const f32 lo = load<f32>(0x100E1810), hi = load<f32>(0x100E1814);
    store<f32>(0x1048DCC0, lo);
    store<f32>(0x1048DCC4, hi);
    call<void>(0x028ED6F8, 0x1048DCC8u);
    call<void>(0x028F026C, 0x101F4EA4u);
    call<void>(0x028EAB2C, 0x1048DCC9u);
    call<void>(0x028F026C, 0x101F4EB0u);
}
VERIFY(0x02602518, staticInit);

} // namespace hd_text_ruby

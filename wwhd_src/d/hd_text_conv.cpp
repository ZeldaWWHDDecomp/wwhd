/* hd_text_conv: HD message control-character conversion (button/icon glyph codes), WWHD.
 * HD-only code (no GameCube source): written from the WWHD
 * code. Range 025F90C4..025F936F (static initialiser 025F92DC).
 *
 * Codes below 0x100E0D20's table map to private-use glyphs (E0xx). Codes 0x0D..0x12 and 0x1C..0x21
 * depend on the message manager (101F4B5C): its +0x998 / +0x99C values divided by the step at
 * 100E0CB0 (20) select between glyph variants (pairs in the tables at 100E0CE0..100E0D1C).
 */
#include "gabi.h"
using namespace gabi;

namespace hd_text_conv {

/* 025F90C4: empty function */
void empty() { WWHD_FUNC(0x025F90C4, void); }
VERIFY(0x025F90C4, empty);

static u16 variant(u32 mgr, u32 field, u32 table) {
    const s32 step = s32(load<u32>(0x100E0CB0));
    const s32 v = s32(load<u32>(mgr + field));
    const s32 i = v / step;
    return load<u16>(table + (u32(i) << 1));
}

/* 025F90C8: glyph code of a message control character */
u16 convert(u32 self, u32 c) {
    WWHD_FUNC(0x025F90C8, u16, self, c);
    const u16 r = load<u16>(0x100E0D20 + (c << 1));
    const u32 mgr = load<u32>(0x101F4B5C);
    if (!mgr) return r;
    const u32 k = c - 0xD;
    if (k > 0x14) return r;
    switch (k) {
    case 0: return variant(mgr, 0x998, 0x100E0D00);
    case 1: return variant(mgr, 0x998, 0x100E0D04);
    case 2: return variant(mgr, 0x998, 0x100E0D08);
    case 3: return variant(mgr, 0x998, 0x100E0D0C);
    case 4: return variant(mgr, 0x99C, 0x100E0CE0);
    case 5: return variant(mgr, 0x99C, 0x100E0CE8);
    case 15: return variant(mgr, 0x998, 0x100E0D10);
    case 16: return variant(mgr, 0x998, 0x100E0D14);
    case 17: return variant(mgr, 0x998, 0x100E0D18);
    case 18: return variant(mgr, 0x998, 0x100E0D1C);
    case 19: return variant(mgr, 0x99C, 0x100E0CF0);
    case 20: return variant(mgr, 0x99C, 0x100E0CF8);
    default: return r;
    }
}
VERIFY(0x025F90C8, convert);

void staticInit() {
    WWHD_FUNC(0x025F92DC, void);
    const u32 b = 0x1048D78C;
    store<u32>(b + 8, 0);
    store<u32>(b, 0);
    store<u32>(b + 0xC, 0);
    store<u32>(b + 4, 0);
    call<void>(0x028F026C, 0x101F4B64u);
    const f32 lo = load<f32>(0x100E0D74), hi = load<f32>(0x100E0D78);
    store<f32>(0x1048D780, lo);
    store<f32>(0x1048D784, hi);
    call<void>(0x028ED6F8, 0x1048D788u);
    call<void>(0x028F026C, 0x101F4B70u);
    call<void>(0x028EAB2C, 0x1048D789u);
    call<void>(0x028F026C, 0x101F4B7Cu);
}
VERIFY(0x025F92DC, staticInit);

} // namespace hd_text_conv

/** HD message control-character glyph selection; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdTextGlyphSelectors {
    u8 _padding00[0x998];
    be<s32> mFirstSelector;
    be<s32> mSecondSelector;
};
WWHD_OFFSET(hdTextGlyphSelectors, mFirstSelector, 0x998);
WWHD_OFFSET(hdTextGlyphSelectors, mSecondSelector, 0x99C);
struct hdTextGlyphTable {
    be<u16> mGlyphs[2];
    u16 get(s32 index) const {
        // HD: signed selector indices retain 32-bit guest-address wrapping.
        return *gabi::at<be<u16>>(gabi::ea(this) + (u32(index) << 1));
    }
};
WWHD_SIZE(hdTextGlyphTable, 4);
struct hdTextDefaultGlyphTable {
    be<u16> mGlyphs[66];
    u16 get(u32 code) const {
        // HD: the original lookup wraps the doubled control code before addressing.
        return *gabi::at<be<u16>>(gabi::ea(this) + (code << 1));
    }
};

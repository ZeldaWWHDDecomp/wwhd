/* HD message tag processing (hd_text_tag; hd_text_ruby derives from it): WWHD layouts. HD-only code, no
 * GameCube counterpart: names are descriptive; NW4F member names where the role is clear, marked
 * "probably" where the slot/offset is not confirmed by a verified callee. */
#pragma once
#include "bindings.h"
#include "gc_api/SSystem/c_malloc.h"

namespace nw {
namespace font {
/* nw::font::Font: C++ vtable at +4 (GHS: 8-byte slots) */
struct Font {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ be<u32> __vtbl;
    s32 getWidth() { return gabi::call_ptr<s32>(gabi::load<u32>(__vtbl + 0x1C), this); }  /* probably GetWidth / cell width */
    s32 getHeight() { return gabi::call_ptr<s32>(gabi::load<u32>(__vtbl + 0x24), this); } /* probably GetHeight / cell height */
};
/* nw::font::Glyph (0x18), constructor 0286EBEC */
struct Glyph {
    /* 0x00 */ u8 _00[6];
    /* 0x06 */ be<u8> mCharWidth; /* probably CharWidths::charWidth */
    /* 0x07 */ u8 _07[0x18 - 7];
};
WWHD_SIZE(Glyph, 0x18);
inline void gabi_construct(Glyph& g) { gabi::call(0x0286EBEC, &g); }
}  // namespace font
}  // namespace nw

/* the virtual GetGlyph(Glyph*, code) at vtable +0x8C (called with an explicit vtable load) */
inline void Font_getGlyph(nw::font::Font* font, nw::font::Glyph* glyph, u32 code) {
    gabi::call_ptr(gabi::load<u32>(font->__vtbl + 0x8C), font, glyph, code);
}

/* hd_font_mgr: the font manager singleton (*0x101F4A50) */
enum hdFontNo_e {
    HD_FONT_MAIN_e = 0,
    HD_FONT_MSG_e = 1,
    HD_FONT_RUBY_e = 2,
    HD_FONT_ZELDA_e = 3,
    HD_FONT_PICT_e = 4, /* pictographs */
    HD_FONT_CAFE_STD_e = 5,
    HD_FONT_MAIN_L_e = 6,
};
struct hdTextConv_c {
    /* 025F90C8 (hd_text_conv): glyph code of a message control character (a u16; callers pass r3 on
     * as it is, so the full register is kept here) */
    u32 convert(u32 c) { return gabi::call<u32>(0x025F90C8, this, c); }
};
struct hdFontMgr_c {
    /* 0x00 */ u8 _00[0x2C];
    /* 0x2C */ hdTextConv_c mTextConv;
    static hdFontMgr_c* get() { return gabi::at<hdFontMgr_c>(gabi::load<u32>(0x101F4A50)); }
    /* 025F36B8: font i (NULL when out of range) */
    nw::font::Font* getFont(s32 i) { return gabi::call<nw::font::Font*>(0x025F36B8, this, i); }
};

/* the text writer the tags change (NW4F TextWriter-like) */
struct hdTextWriter_c {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ be<f32> mLineX;    /* probably the line's cursor x */
    /* 0x0C */ be<f32> mScaleX;
    /* 0x10 */ be<f32> mScaleY;
    /* 0x14 */ be<f32> mCursorX;
    /* 0x18 */ u8 _18[0x28 - 0x18];
    /* 0x28 */ gptr<nw::font::Font> mpFont;
};
WWHD_OFFSET(hdTextWriter_c, mCursorX, 0x14);
WWHD_OFFSET(hdTextWriter_c, mpFont, 0x28);

/* print context: its writer first */
struct hdTextPrintContext_c {
    /* 0x00 */ gptr<hdTextWriter_c> mpWriter;
};

/* a tag record of the message text */
struct hdTextTag_c {
    enum Group_e { GROUP_FONT_e = 0, GROUP_LAYOUT_e = 1, GROUP_PICT_e = 3 };
    enum Tag_e { TAG_FONT_e = 1, TAG_SCALE_e = 2, TAG_ABS_SCALE_e = 0xC };
    /* 0x00 */ u8 _00[2];
    /* 0x02 */ be<u16> mGroup;
    union {
        /* 0x04 */ be<u16> mTag;
        struct {
            /* 0x04 */ u8 _04;
            /* 0x05 */ be<u8> mPictNo; /* group 3: the pictograph */
        };
    };
    /* 0x06 */ be<u16> mParamSize;
    /* 0x08 */ be<u16> mParam;       /* font tag: font no. (0xFFFF: back to the saved font); scale tag: percent */
    /* 0x0A */ be<u16> mFontScaleX;  /* font tag: size in percent of the font */
    /* 0x0C */ be<u16> mFontScaleY;
    /* 0x0E */ u8 _0E[2];
    /* 0x10 */ be<u16> mScaleX;      /* TAG_ABS_SCALE_e: percent */
    /* 0x12 */ be<u16> mScaleY;
};
WWHD_OFFSET(hdTextTag_c, mParamSize, 0x6);
WWHD_OFFSET(hdTextTag_c, mScaleX, 0x10);

/* the tag processor's saved state (0x10) */
struct hdTextTagState_c {
    /* 0x00 */ be<u8> mScaleSaved;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<f32> mSavedScaleX;
    /* 0x08 */ be<f32> mSavedScaleY;
    /* 0x0C */ gptr<nw::font::Font> mpSavedFont;
};
WWHD_SIZE(hdTextTagState_c, 0x10);

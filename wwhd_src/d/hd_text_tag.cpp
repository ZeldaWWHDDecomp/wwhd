/**
 * hd_text_tag.cpp
 * HD - message tag processing: font, scale and pictograph tags (hd_text_ruby derives from it)
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), written in the style of the GameCube
 * decompilation and verified function by function against the recompiled original (tools/verify,
 * unit hd_text_tag). Each function is marked with its WWHD address; the glue block at the end lists
 * this translation unit's data (tools/verify/autoglue.py).
 */

#include "d/hd_text_tag.h"

/* 025FA0F0 */
// hdTextTagState_c::hdTextTagState_c() (GHS: allocates when called with NULL)
static hdTextTagState_c* hdTextTagState_c__ct(hdTextTagState_c* i_this) {
    if (i_this == NULL) {
        i_this = (hdTextTagState_c*)operator_new(sizeof(hdTextTagState_c));
        if (i_this == NULL) {
            return NULL;
        }
    }
    i_this->mScaleSaved = false;
    i_this->mSavedScaleY = 1.0f;
    i_this->mpSavedFont = NULL;
    i_this->mSavedScaleX = 1.0f;
    return i_this;
}

/* 025FA140 */
// font tag: switch the writer's font (0xFFFF: back to the saved one) and scale it to the requested size
static BOOL hdTextTag_setFont(hdTextTagState_c* i_state, hdTextWriter_c* i_writer, hdTextTag_c* i_tag) {
    u16 fontNo = i_tag->mParam;
    nw::font::Font* font;
    if (fontNo == 0xFFFF) {
        font = i_state->mpSavedFont;
        if (font == NULL) {
            font = i_writer->mpFont;
            if (font == NULL) {
                return FALSE;
            }
        }
    } else {
        font = hdFontMgr_c::get()->getFont(fontNo);
        i_state->mpSavedFont = i_writer->mpFont;
        if (font == NULL) {
            return FALSE;
        }
    }
    i_writer->mpFont = font;

    if (i_tag->mParamSize < 6) {
        return FALSE;
    }
    f32 scaleX = i_tag->mFontScaleX / 100.0f;
    f32 scaleY = i_tag->mFontScaleY / 100.0f;
    scaleX = scaleX / font->getWidth();
    f32 height = font->getHeight();
    i_writer->mScaleX = scaleX;
    i_writer->mScaleY = scaleY / height;
    return FALSE;
}

/* 025FA2CC */
// scale tag: scale the writer relative to its scale before the first scale tag
static BOOL hdTextTag_scale(hdTextTagState_c* i_state, hdTextWriter_c* i_writer, hdTextTag_c* i_tag) {
    if (!i_state->mScaleSaved) {
        i_state->mSavedScaleX = i_writer->mScaleX;
        f32 y = i_writer->mScaleY;
        i_state->mScaleSaved = true;
        i_state->mSavedScaleY = y;
    }
    f32 scale = i_tag->mParam / 100.0f;
    f32 savedY = i_state->mSavedScaleY;
    f32 savedX = i_state->mSavedScaleX;
    i_writer->mScaleX = gabi::fmuls_ppc(savedX, scale);
    i_writer->mScaleY = gabi::fmuls_ppc(savedY, scale);
    return TRUE;
}

/* 025FA348 */
static BOOL hdTextTag_procFontGroup(hdTextTagState_c* i_state, hdTextWriter_c* i_writer, hdTextTag_c* i_tag) {
    switch (i_tag->mTag) {
    case hdTextTag_c::TAG_FONT_e:
        return hdTextTag_setFont(i_state, i_writer, i_tag);
    case hdTextTag_c::TAG_SCALE_e:
        return hdTextTag_scale(i_state, i_writer, i_tag);
    default:
        return TRUE;
    }
}

/* 025FA36C */
static BOOL hdTextTag_setScale(hdTextTagState_c* i_state, hdTextWriter_c* i_writer, hdTextTag_c* i_tag) {
    f32 scaleX = i_tag->mScaleX / 100.0f;
    f32 scaleY = i_tag->mScaleY / 100.0f;
    i_writer->mScaleX = scaleX;
    i_writer->mScaleY = scaleY;
    return TRUE;
}

/* 025FA3CC */
static BOOL hdTextTag_procLayoutGroup(hdTextTagState_c* i_state, hdTextWriter_c* i_writer, hdTextTag_c* i_tag) {
    if (i_tag->mTag == hdTextTag_c::TAG_ABS_SCALE_e) {
        hdTextTag_setScale(i_state, i_writer, i_tag);
    }
    return TRUE;
}

/* 025FA3FC */
// pictograph tag: advance the line by the pictograph glyph's width
static BOOL hdTextTag_putPict(hdTextTagState_c* i_state, hdTextWriter_c* i_writer, hdTextWriter_c* i_ctxWriter, hdTextTag_c* i_tag) {
    nw::font::Font* font = hdFontMgr_c::get()->getFont(HD_FONT_PICT_e);
    u32 code = hdFontMgr_c::get()->mTextConv.convert(i_tag->mPictNo);
    nw::font::Glyph glyph;
    Font_getGlyph(font, &glyph, code);
    f32 width = glyph.mCharWidth;
    f32 x = fmadds(width, i_ctxWriter->mScaleX, i_ctxWriter->mCursorX);
    i_ctxWriter->mCursorX = x;
    i_writer->mLineX = x;
    return TRUE;
}

/* 025FA4CC */
// dispatch by tag group
static BOOL hdTextTag_process(hdTextTagState_c* i_state, hdTextWriter_c* i_writer, hdTextPrintContext_c* i_ctx, hdTextTag_c* i_tag) {
    u16 group = i_tag->mGroup;
    hdTextWriter_c* ctxWriter = i_ctx->mpWriter;
    if (group < hdTextTag_c::GROUP_LAYOUT_e) {
        return hdTextTag_procFontGroup(i_state, ctxWriter, i_tag);
    } else if (group == hdTextTag_c::GROUP_LAYOUT_e) {
        return hdTextTag_procLayoutGroup(i_state, ctxWriter, i_tag);
    } else if (group == hdTextTag_c::GROUP_PICT_e) {
        return hdTextTag_putPict(i_state, i_writer, ctxWriter, i_tag);
    }
    return TRUE;
}

/* 025FA50C */
static void hdTextTagState_c__reset(hdTextTagState_c* i_this) {
    i_this->mScaleSaved = false;
    i_this->mSavedScaleX = 1.0f;
    i_this->mpSavedFont = NULL;
    i_this->mSavedScaleY = 1.0f;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 025FA52C */
// __sinit_hd_text_tag_cpp: the common header statics (the pi pair copied from .rodata)
static void __sinit_hd_text_tag_cpp() {
    sinit_header_statics_rodata(0x1048D864, 0x101F4BE0, 0x1048D870, 0x100E0F80);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
*/

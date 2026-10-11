/* HD message tags: ruby/furigana, font, scale and colour controls.
 * GameCube-style source verified as hd_text_ruby; no GameCube counterpart. */
#include "d/hd_text_ruby.h"

static const u32 wideStringVtable = 0x100E17A8;
static const u32 fixedStringVtable = 0x100E17D8;
static hdRuby_fontManagerPointer_c sFontManager;
static hdRuby_configPointer_c sConfig;
static f32 sOne;
static f32 sMaxScale;
static f32 sZero;
static f32 sHalf;
static f32 sPercent;

/* 02601B60 */
static hdRuby_tagState_c* hdRuby_constructState(hdRuby_tagState_c* state) {
    if (!state) {
        state = (hdRuby_tagState_c*)operator_new(sizeof(hdRuby_tagState_c));
        if (!state) return nullptr;
    }
    f32 one = sOne;
    state->mScaleSaved = 0;
    state->mSavedScaleY = one;
    state->mpSavedFont = nullptr;
    state->mSavedScaleX = one;
    return state;
}

/* 02601BB0 */
u32 hdRuby_tagState_c::hasControlChar(be<u16>* text, s32 length) {
    if (length <= 0) return 0;
    for (s32 i = 0; i < length; ++i) {
        u32 character = text[i];
        if (character == RUBY_TEXT_CONTROL_BEGIN || character == RUBY_TEXT_CONTROL_END) return 1;
    }
    return 0;
}

/* inlined into hdRuby_copyInto */
static s32 hdRuby_wideLength(be<u16>* text) {
    s32 length = 0;
    if (!*text) return 0;
    for (;;) {
        ++length;
        // HD: sead's maximum length guard returns zero for an unterminated string.
        if (length > 0x40000) return 0;
        ++text;
        if (!*text) return length;
    }
}

/* inlined into drawRuby */
static void hdRuby_copyInto(hdRuby_drawFrame_c* frame, hdRuby_fixedString_c* fixed, s32 length) {
    be<u16>* buffer = fixed->mString.mStringTop;
    s32 count = length;
    if (length < 0) {
        hdRuby_assureWideString(&frame->mSource);
        count = hdRuby_wideLength(frame->mSource.mStringTop);
    }
    s32 capacity = fixed->mCapacity;
    if (count >= capacity) count = capacity - 1;
    frame->mSource.assureTermination();
    hdRuby_copyText(buffer, frame->mSource.mStringTop, u32(count * 2), 0);
    buffer[count] = 0;
}

/* 02601BE8 */
void hdRuby_tagState_c::drawRuby(hdRuby_writer_c* writer, hdRuby_tag_c* tag, be<u16>* text) {
    s32 baseLength = u16(tag->mData.mParams[0]) >> 1;
    s32 rubyLength = u16(tag->mData.mParams[1]) >> 1;
    hdRuby_config_c* config = sConfig.mpConfig;
    if (u32(baseLength - 1) >= 31 || u32(rubyLength - 1) >= 31) return;
    gabi::Local<hdRuby_drawFrame_c> frame;
    frame->mRuby.mString.mStringTop = frame->mRuby.mBuffer;
    frame->mRuby.mCapacity = 32;
    frame->mRuby.mString.setVtable(fixedStringVtable);
    frame->mRuby.mBuffer[31] = 0;
    frame->mRuby.mBuffer[0] = 0;
    frame->mSource.setVtable(wideStringVtable);
    frame->mSource.mStringTop = &tag->mData.mParams[2];
    hdRuby_copyInto(frame.get(), &frame->mRuby, rubyLength);
    frame->mSource.mStringTop = text;
    frame->mBase.mBuffer[31] = 0;
    frame->mBase.mBuffer[0] = 0;
    frame->mBase.mString.setVtable(fixedStringVtable);
    frame->mBase.mCapacity = 32;
    frame->mSource.setVtable(wideStringVtable);
    frame->mBase.mString.mStringTop = frame->mBase.mBuffer;
    hdRuby_copyInto(frame.get(), &frame->mBase, baseLength);
    hdRuby_writerPrepareFirst(writer);
    hdRuby_writerPrepareSecond(writer);
    hdRuby_font_c* font = writer->mpFont;
    s32 lineHeight = gabi::call_ptr<s32>(font->mVtable->mGetLineHeight, font);
    hdRuby_writer_c* rubyWriter = &frame->mWriter;
    // HD: copying all writer words preserves unknown fields and NaN payloads.
    be<u32>* destination = reinterpret_cast<be<u32>*>(rubyWriter);
    be<u32>* source = reinterpret_cast<be<u32>*>(writer);
    for (u32 i = 0; i < sizeof(hdRuby_writer_c) / 4; ++i) destination[i] = u32(source[i]);
    rubyWriter->mpFont = hdRuby_getFont(sFontManager.mpManager, RUBY_FONT_RUBY);
    f32 scale = config->mScale;
    f32 scaleX = gabi::fmuls_ppc(rubyWriter->mScaleX, scale);
    f32 maximum = gabi::fmuls_ppc(scale, sMaxScale);
    f32 scaleY = gabi::fmuls_ppc(rubyWriter->mScaleY, scale);
    if (scaleX > maximum) scaleX = maximum;
    if (scaleY > maximum) scaleY = maximum;
    rubyWriter->mScaleY = scaleY;
    rubyWriter->mScaleX = scaleX;
    f32 spacing = config->mSpacing;
    f32 zero = sZero;
    rubyWriter->mCharSpacing = spacing;
    rubyWriter->mLineSpacing = zero;
    rubyWriter->mFlags = 0x300;
    rubyWriter->mUnknown = u32(writer->mUnknown);
    frame->mBase.mString.assureTermination();
    f32 baseWidth = hdRuby_textWidth(writer, frame->mBase.mString.mStringTop, baseLength);
    frame->mRuby.mString.assureTermination();
    f32 rubyWidth = hdRuby_textWidth(rubyWriter, frame->mRuby.mString.mStringTop, rubyLength);
    f32 difference = gabi::fsubs_ppc(baseWidth, rubyWidth);
    if (difference > zero) {
        f32 interval = difference / f32(rubyLength + 1);
        rubyWriter->mCursorX = gabi::fadds_ppc(rubyWriter->mCursorX, interval);
        rubyWriter->mCharSpacing = gabi::fadds_ppc(interval, spacing);
    } else {
        rubyWriter->mCursorX = gabi::fmadds(difference, sHalf, rubyWriter->mCursorX);
    }
    f32 heightRatio = hdRuby_fontHeight(writer) / f32(lineHeight);
    f32 rise = config->mRise;
    f32 height = hdRuby_fontHeight(writer);
    rubyWriter->mCursorY = gabi::fadds_ppc(rubyWriter->mCursorY, gabi::fmsubs(rise, heightRatio, height));
    // HD: colour bytes follow the variable-length ruby text, outside the fixed tag prefix.
    be<u8>* colour = gabi::at<be<u8>>(gabi::ea(tag) + 2u * u32(rubyLength) + 0xC);
    u32 color = (u32(colour[1]) << 24) | (u32(colour[0]) << 16) | (u32(colour[3]) << 8) | u32(colour[2]);
    rubyWriter->mColorTop = color;
    rubyWriter->mColorBottom = color;
    frame->mRuby.mString.assureTermination();
    hdRuby_print(rubyWriter, frame->mRuby.mString.mStringTop, rubyLength);
    hdRuby_destroyWriter(rubyWriter, 2);
}

/* 02602038 */
u32 hdRuby_tagState_c::tagRuby(hdRuby_writer_c* writer, hdRuby_tag_c* tag, be<u16>* text) {
    if (!hasControlChar(text, s32(u16(tag->mData.mParams[0]) >> 1))) drawRuby(writer, tag, text);
    return 1;
}

/* 0260208C */
u32 hdRuby_tagState_c::tagFont(hdRuby_writer_c* writer, hdRuby_tag_c* tag) {
    u32 index = tag->mData.mParams[0];
    hdRuby_font_c* font;
    if (index == RUBY_FONT_RESTORE) {
        font = mpSavedFont;
        mpSavedFont = nullptr;
    } else {
        font = hdRuby_getFont(sFontManager.mpManager, index);
        if (!mpSavedFont) mpSavedFont = writer->mpFont;
    }
    writer->mpFont = font;
    if (tag->mParamSize >= 6) {
        f32 height = u16(tag->mData.mParams[1]);
        f32 width = u16(tag->mData.mParams[2]);
        f32 percent = sPercent;
        f32 y = width / percent;
        f32 x = height / percent;
        hdRuby_setFontSize(writer, x, y);
    }
    return 0;
}

/* 0260216C */
u32 hdRuby_tagState_c::tagScale(hdRuby_writer_c* writer, hdRuby_tag_c* tag) {
    if (!mScaleSaved) {
        mSavedScaleX = f32(writer->mScaleX);
        f32 y = writer->mScaleY;
        mScaleSaved = 1;
        mSavedScaleY = y;
    }
    f32 scale = f32(u16(tag->mData.mParams[0])) / sPercent;
    f32 y = mSavedScaleY;
    f32 x = mSavedScaleX;
    writer->mScaleX = gabi::fmuls_ppc(x, scale);
    writer->mScaleY = gabi::fmuls_ppc(y, scale);
    return 1;
}

/* inlined into tagColor and tagColorScale */
static u32 hdRuby_tagColour(be<u8>* bytes) {
    return (u32(bytes[1]) << 24) | (u32(bytes[0]) << 16) | (u32(bytes[3]) << 8) | u32(bytes[2]);
}

/* 026021E8 */
u32 hdRuby_tagState_c::tagColor(hdRuby_writer_c* writer, hdRuby_tag_c* tag) {
    u32 color = hdRuby_tagColour(tag->mData.mParamBytes);
    writer->mColorTop = color;
    writer->mColorBottom = color;
    return 1;
}

/* 0260223C */
u32 hdRuby_tagState_c::tagSystem(hdRuby_writer_c* writer, hdRuby_tag_c* tag, be<u16>* text) {
    switch (u16(tag->mKind.mTag)) {
    case RUBY_TAG_RUBY: return tagRuby(writer, tag, text);
    case RUBY_TAG_FONT: return tagFont(writer, tag);
    case RUBY_TAG_SCALE: return tagScale(writer, tag);
    case RUBY_TAG_COLOR: return tagColor(writer, tag);
    }
    return 1;
}

/* 02602270 */
u32 hdRuby_tagState_c::tagColorScale(hdRuby_writer_c* writer, hdRuby_tag_c* tag) {
    writer->mColorTop = hdRuby_tagColour(tag->mData.mParamBytes);
    writer->mColorBottom = hdRuby_tagColour(&tag->mData.mParamBytes[4]);
    f32 percent = sPercent;
    f32 x = u16(tag->mData.mParams[4]);
    f32 y = u16(tag->mData.mParams[5]);
    writer->mScaleX = x / percent;
    writer->mScaleY = y / percent;
    return 1;
}

/* 02602334 */
u32 hdRuby_tagState_c::tagGroup1(hdRuby_writer_c* writer, hdRuby_tag_c* tag) {
    u32 kind = tag->mKind.mTag;
    if (kind == RUBY_TAG_COLOR_SCALE) return tagColorScale(writer, tag);
    if (kind == RUBY_TAG_FORMAT_BREAK) return 4;
    return 1;
}

/* 0260237C */
u32 hdRuby_tagState_c::tagGroup3(hdRuby_writer_c* writer, hdRuby_tag_c* tag) {
    hdRuby_font_c* oldFont = writer->mpFont;
    hdRuby_font_c* font = hdRuby_getFont(sFontManager.mpManager, RUBY_FONT_PICTURE);
    // HD: the picture-string manager occupies an embedded slot in the font manager.
    void* pictureManager = gabi::at<void>(gabi::ea((void*)sFontManager.mpManager) + 0x2C);
    be<u16>* text = hdRuby_pictureText(pictureManager, u32(tag->mKind.mPicture.mValue));
    u32 top = writer->mColorTop;
    u32 bottom = writer->mColorBottom;
    writer->mpFont = font;
    s32 ascent = gabi::call_ptr<s32>(font->mVtable->mGetAscent, font);
    f32 offsetY = gabi::fmuls_ppc(f32(-ascent), writer->mScaleY);
    writer->mCursorY = gabi::fadds_ppc(writer->mCursorY, offsetY);
    writer->mColorTop = 0xFFFFFFFF;
    writer->mColorBottom = 0xFFFFFFFF;
    hdRuby_printString(writer, text);
    f32 y = writer->mCursorY;
    writer->mpFont = oldFont;
    writer->mCursorY = gabi::fsubs_ppc(y, offsetY);
    writer->mColorTop = top;
    writer->mColorBottom = bottom;
    return 1;
}

/* 026024C8 */
u32 hdRuby_tagState_c::process(hdRuby_context_c* context, hdRuby_tag_c* tag, be<u16>* text) {
    u32 group = tag->mGroup;
    hdRuby_writer_c* writer = context->mpWriter;
    if (group == RUBY_GROUP_SYSTEM) return tagSystem(writer, tag, text);
    if (group == RUBY_GROUP_FORMAT) return tagGroup1(writer, tag);
    if (group == RUBY_GROUP_PICTURE) return tagGroup3(writer, tag);
    return 1;
}

/* 026024F8 */
void hdRuby_tagState_c::reset() {
    f32 one = sOne;
    mScaleSaved = 0;
    mSavedScaleX = one;
    mpSavedFont = nullptr;
    mSavedScaleY = one;
}

/* ---------------- HD: compiler-generated and template code ---------------- */

/* 02602518 */
static void __sinit_hd_text_ruby_cpp() {
    sinit_header_statics_rodata(0x1048DCC0, 0x101F4E98, 0x1048DCCC, 0x100E1810);
}

/* WWHD glue
data sFontManager 101F4A50 nocheck
data sConfig 101F4B5C nocheck
data sOne 100E17A0 nocheck
data sMaxScale 100E17F0 nocheck
data sZero 100E17F4 nocheck
data sHalf 100E1800 nocheck
data sPercent 100E1804 nocheck
ext 026025AC void hdRuby_assureWideString(hdRuby_wideString_c* string)
ext C0009988 void hdRuby_copyText(be<u16>* destination, be<u16>* source, u32 bytes, u32 flags)
ext 0286E3C4 void hdRuby_writerPrepareFirst(hdRuby_writer_c* writer)
ext 0286E42C void hdRuby_writerPrepareSecond(hdRuby_writer_c* writer)
ext 025F36B8 hdRuby_font_c* hdRuby_getFont(void* manager, s32 index)
ext 028706A0 f32 hdRuby_textWidth(hdRuby_writer_c* writer, be<u16>* text, s32 length)
ext 0286E494 f32 hdRuby_fontHeight(hdRuby_writer_c* writer)
ext 028710D0 void hdRuby_print(hdRuby_writer_c* writer, be<u16>* text, s32 length)
ext 0286FED0 void hdRuby_destroyWriter(hdRuby_writer_c* writer, u32 flags)
ext 0286E2E4 void hdRuby_setFontSize(hdRuby_writer_c* writer, f32 x, f32 y)
ext 025F90C8 be<u16>* hdRuby_pictureText(void* manager, u32 value)
ext 0286E9F0 void hdRuby_printString(hdRuby_writer_c* writer, be<u16>* text)
*/

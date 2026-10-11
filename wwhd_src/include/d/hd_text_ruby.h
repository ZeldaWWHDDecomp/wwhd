/* HD: wide-text ruby, font, scale and colour tags; no GameCube counterpart.
 * The writer copy and fixed wide strings retain the exact HD stack layout. */
#pragma once
#include "bindings.h"

enum hdRubyTagGroup_e { RUBY_GROUP_SYSTEM = 0, RUBY_GROUP_FORMAT = 1, RUBY_GROUP_PICTURE = 3 };
enum hdRubySystemTag_e { RUBY_TAG_RUBY, RUBY_TAG_FONT, RUBY_TAG_SCALE, RUBY_TAG_COLOR };
enum hdRubyFormatTag_e { RUBY_TAG_COLOR_SCALE = 12, RUBY_TAG_FORMAT_BREAK = 13 };
enum hdRubyFont_e { RUBY_FONT_RUBY = 2, RUBY_FONT_PICTURE = 4, RUBY_FONT_RESTORE = 0xFFFF };
enum hdRubyTextCode_e { RUBY_TEXT_CONTROL_BEGIN = 0xE, RUBY_TEXT_CONTROL_END = 0xF };
struct hdRuby_fontVtable_c { u8 _00[0x2C]; be<u32> mGetLineHeight; u8 _30[0xAC-0x30]; be<u32> mGetAscent; };
struct hdRuby_font_c { u8 _00[4]; gptr<hdRuby_fontVtable_c> mVtable; };
WWHD_OFFSET(hdRuby_font_c, mVtable, 4);
WWHD_SIZE(hdRuby_font_c, 8);
WWHD_OFFSET(hdRuby_fontVtable_c, mGetLineHeight, 0x2C);
WWHD_OFFSET(hdRuby_fontVtable_c, mGetAscent, 0xAC);
WWHD_SIZE(hdRuby_fontVtable_c, 0xB0);
struct hdRuby_writer_c {
    /* 0x00 */ be<u32> mColorTop, mColorBottom;
    /* 0x08 */ u8 _08[4];
    /* 0x0C */ be<f32> mScaleX, mScaleY;
    /* 0x14 */ be<f32> mCursorX, mCursorY;
    /* 0x1C */ u8 _1C[0xC];
    /* 0x28 */ gptr<hdRuby_font_c> mpFont;
    /* 0x2C */ u8 _2C[0xC];
    /* 0x38 */ be<f32> mCharSpacing, mLineSpacing;
    /* 0x40 */ u8 _40[4];
    /* 0x44 */ be<u32> mFlags;
    /* 0x48 */ be<u32> mUnknown;
};
WWHD_OFFSET(hdRuby_writer_c, mScaleX, 0xC);
WWHD_OFFSET(hdRuby_writer_c, mScaleY, 0x10);
WWHD_OFFSET(hdRuby_writer_c, mCursorX, 0x14);
WWHD_OFFSET(hdRuby_writer_c, mCursorY, 0x18);
WWHD_OFFSET(hdRuby_writer_c, mpFont, 0x28);
WWHD_OFFSET(hdRuby_writer_c, mCharSpacing, 0x38);
WWHD_OFFSET(hdRuby_writer_c, mLineSpacing, 0x3C);
WWHD_OFFSET(hdRuby_writer_c, mFlags, 0x44);
WWHD_OFFSET(hdRuby_writer_c, mUnknown, 0x48);
WWHD_SIZE(hdRuby_writer_c, 0x4C);
struct hdRuby_tag_c {
    be<u16> mUnknown, mGroup;
    union { be<u16> mTag; struct { be<u8> mHigh, mValue; } mPicture; } mKind;
    be<u16> mParamSize;
    union { be<u16> mParams[6]; be<u8> mParamBytes[12]; } mData;
};
WWHD_OFFSET(hdRuby_tag_c, mGroup, 2);
WWHD_OFFSET(hdRuby_tag_c, mKind, 4);
WWHD_OFFSET(hdRuby_tag_c, mParamSize, 6);
WWHD_OFFSET(hdRuby_tag_c, mData, 8);
WWHD_SIZE(hdRuby_tag_c, 0x14); // HD: prefix of a variable-length tag.
struct hdRuby_context_c { gptr<hdRuby_writer_c> mpWriter; };
WWHD_SIZE(hdRuby_context_c, 4);
struct hdRuby_stringVtable_c { u8 _00[0x14]; be<u32> mAssureTermination; };
struct hdRuby_wideString_c {
    gptr<be<u16>> mStringTop;
    gptr<hdRuby_stringVtable_c> mVtable;
    void setVtable(u32 address) { mVtable.v = address; }
    void assureTermination() { gabi::call_ptr<void>(mVtable->mAssureTermination, this); }
};
WWHD_SIZE(hdRuby_stringVtable_c, 0x18);
WWHD_OFFSET(hdRuby_stringVtable_c, mAssureTermination, 0x14);
WWHD_SIZE(hdRuby_wideString_c, 8);
WWHD_OFFSET(hdRuby_wideString_c, mVtable, 4);
struct hdRuby_fixedString_c { hdRuby_wideString_c mString; be<s32> mCapacity; be<u16> mBuffer[32]; };
WWHD_SIZE(hdRuby_fixedString_c, 0x4C);
WWHD_OFFSET(hdRuby_fixedString_c, mCapacity, 8);
WWHD_OFFSET(hdRuby_fixedString_c, mBuffer, 0xC);
struct hdRuby_drawFrame_c {
    u8 _00[8];
    hdRuby_wideString_c mSource;
    u8 _10[0xC];
    hdRuby_writer_c mWriter;
    hdRuby_fixedString_c mRuby;
    hdRuby_fixedString_c mBase;
};
WWHD_OFFSET(hdRuby_drawFrame_c, mSource, 8);
WWHD_OFFSET(hdRuby_drawFrame_c, mWriter, 0x1C);
WWHD_OFFSET(hdRuby_drawFrame_c, mRuby, 0x68);
WWHD_OFFSET(hdRuby_drawFrame_c, mBase, 0xB4);
WWHD_SIZE(hdRuby_drawFrame_c, 0x100);
struct hdRuby_config_c { u8 _00[0x974]; be<f32> mScale, mRise, mSpacing; };
WWHD_OFFSET(hdRuby_config_c, mScale, 0x974);
WWHD_OFFSET(hdRuby_config_c, mRise, 0x978);
WWHD_OFFSET(hdRuby_config_c, mSpacing, 0x97C);
WWHD_SIZE(hdRuby_config_c, 0x980);
struct hdRuby_configPointer_c { gptr<hdRuby_config_c> mpConfig; };
struct hdRuby_fontManagerPointer_c { gptr<void> mpManager; };
WWHD_SIZE(hdRuby_configPointer_c, 4);
WWHD_SIZE(hdRuby_fontManagerPointer_c, 4);
struct hdRuby_tagState_c {
    /* 0x00 */ be<u8> mScaleSaved;
    u8 _01[3];
    /* 0x04 */ be<f32> mSavedScaleX, mSavedScaleY;
    /* 0x0C */ gptr<hdRuby_font_c> mpSavedFont;
    u32 hasControlChar(be<u16>* text, s32 length);
    void drawRuby(hdRuby_writer_c* writer, hdRuby_tag_c* tag, be<u16>* text);
    u32 tagRuby(hdRuby_writer_c* writer, hdRuby_tag_c* tag, be<u16>* text);
    u32 tagFont(hdRuby_writer_c* writer, hdRuby_tag_c* tag);
    u32 tagScale(hdRuby_writer_c* writer, hdRuby_tag_c* tag);
    u32 tagColor(hdRuby_writer_c* writer, hdRuby_tag_c* tag);
    u32 tagSystem(hdRuby_writer_c* writer, hdRuby_tag_c* tag, be<u16>* text);
    u32 tagColorScale(hdRuby_writer_c* writer, hdRuby_tag_c* tag);
    u32 tagGroup1(hdRuby_writer_c* writer, hdRuby_tag_c* tag);
    u32 tagGroup3(hdRuby_writer_c* writer, hdRuby_tag_c* tag);
    u32 process(hdRuby_context_c* context, hdRuby_tag_c* tag, be<u16>* text);
    void reset();
};
WWHD_OFFSET(hdRuby_tagState_c, mSavedScaleX, 4);
WWHD_OFFSET(hdRuby_tagState_c, mSavedScaleY, 8);
WWHD_OFFSET(hdRuby_tagState_c, mpSavedFont, 0xC);
WWHD_SIZE(hdRuby_tagState_c, 0x10);
void hdRuby_assureWideString(hdRuby_wideString_c* string);
void hdRuby_copyText(be<u16>* destination, be<u16>* source, u32 bytes, u32 flags);
void hdRuby_writerPrepareFirst(hdRuby_writer_c* writer);
void hdRuby_writerPrepareSecond(hdRuby_writer_c* writer);
hdRuby_font_c* hdRuby_getFont(void* manager, s32 index);
f32 hdRuby_textWidth(hdRuby_writer_c* writer, be<u16>* text, s32 length);
f32 hdRuby_fontHeight(hdRuby_writer_c* writer);
void hdRuby_print(hdRuby_writer_c* writer, be<u16>* text, s32 length);
void hdRuby_destroyWriter(hdRuby_writer_c* writer, u32 flags);
void hdRuby_setFontSize(hdRuby_writer_c* writer, f32 x, f32 y);
be<u16>* hdRuby_pictureText(void* manager, u32 value);
void hdRuby_printString(hdRuby_writer_c* writer, be<u16>* text);

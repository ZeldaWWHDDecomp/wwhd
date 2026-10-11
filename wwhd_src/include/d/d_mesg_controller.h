/* Message controller subset. GameCube field names; HD retains their offsets.
 */
#pragma once
#include "gc_api/gc_api.h"

namespace JMessage {
struct TControl {
    // HD: base work is initialized by the original JMessage constructor.
    /* 0x00 */ u8 _000[0x38];
    /* 0x38 */ be<u32> __vtbl;
};
}
struct JUTFont;
struct dMesg_tControl : JMessage::TControl {
    enum : u32 { __vt = 0x100E0CB8 };
    void setMesgStatus(u32 status);

    /* 0x3C */ gptr<JUTFont> mMainFont;
    /* 0x40 */ gptr<JUTFont> mRubyFont;
    /* 0x44 */ be<f32> mLineLength[4];
    /* 0x54 */ be<s32> mLineCount;
    /* 0x58 */ be<s32> mLineStart;
    /* 0x5C */ be<s32> mInitFontSize;
    /* 0x60 */ be<s32> mNowFontSize;
    /* 0x64 */ be<s32> mCharSpace;
    /* 0x68 */ be<s32> mCharCode;
    /* 0x6C */ be<s32> mTextBoxWidth;
    /* 0x70 */ be<u8> mbHeader;
    /* 0x71 */ be<u8> mCode16Fg;
    /* 0x72 */ u8 _072[2];
};
WWHD_OFFSET(dMesg_tControl, __vtbl, 0x38);
WWHD_OFFSET(dMesg_tControl, mMainFont, 0x3C);
WWHD_OFFSET(dMesg_tControl, mLineLength, 0x44);
WWHD_OFFSET(dMesg_tControl, mInitFontSize, 0x5C);
WWHD_OFFSET(dMesg_tControl, mbHeader, 0x70);
WWHD_SIZE(dMesg_tControl, 0x74);

// HD: only the play object's message status is touched by this subset.
struct dMesg_play_view {
    /* 0x0000 */ u8 _0000[0x5BB2];
    /* 0x5BB2 */ be<u8> mMesgStatus;
};
WWHD_OFFSET(dMesg_play_view, mMesgStatus, 0x5BB2);
void JMessage_TControl_ct(JMessage::TControl* self);
void JMessage_TControl_dt(JMessage::TControl* self, u32 flags);
dMesg_tControl* dMesg_tControl_ct(dMesg_tControl* self);
void dMesg_tControl_dt(dMesg_tControl* self, u32 flags);

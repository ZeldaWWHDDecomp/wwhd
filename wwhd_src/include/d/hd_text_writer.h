/** HD message tag processor and pane helpers. No GameCube counterpart. */
#pragma once
#include "d/hd_text_tag.h"

struct hdMessageBuilder_c { gptr<void> mpMessage; };
struct hdMessageTagProcessor_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ hdMessageBuilder_c mBuilder;
    /* 0x08 */ hdTextTagState_c mRubyState;
    /* 0x18 */ hdTextTagState_c mTagState;
    /* 0x28 */ be<u32> mMessageArgument; // Role beyond the setup call is unconfirmed.
};
struct hdMessageContext_c {
    /* 0x00 */ gptr<hdTextWriter_c> mpWriter;
    /* 0x04 */ gptr<be<u16>> mpText;
};
struct hdMessageTag_c {
    enum Code_e { NEW_LINE = 0xA, BEGIN_TAG = 0xE, END_TAG = 0xF };
    /* 0x00 */ be<u16> mCode;
    /* 0x02 */ be<u16> mGroup;
    /* 0x04 */ be<u16> mTag;
    /* 0x06 */ be<u16> mParamSize;
};
struct hdMessagePaneVtable_c {
    u8 padding[0xCC];
    be<u32> mCopyString;
};
struct hdMessagePane_c {
    enum Flag_e { VISIBLE = 1 };
    u8 padding0[8];
    /* 0x08 */ gptr<hdMessagePaneVtable_c> mpVtable;
    u8 paddingC[0x44 - 0xC];
    /* 0x44 */ be<u8> mFlags;
    void copyString(u32 text, u32 offset, u32 length) {
        gabi::call_ptr<void>(mpVtable->mCopyString, this, text, offset, length);
    }
};
struct hdMessagePaneList_c {
    /* 0x00 */ be<u32> mCount;
    u8 padding4[4];
    /* 0x08 */ gptr<gptr<hdMessagePane_c>> mpPanes;
};
// HD: Keep all text and iterator arithmetic in 32-bit guest addresses, including wrap at zero.
inline be<u16>& hdMessage_character(u32 address) { return *gabi::at<be<u16>>(address); }
inline hdMessageTag_c* hdMessage_tag(u32 address) { return gabi::at<hdMessageTag_c>(address); }
inline hdMessagePane_c* hdMessage_pane(u32 address) { return gabi::at<gptr<hdMessagePane_c>>(address)->get(); }
WWHD_SIZE(hdMessageBuilder_c, 4);
WWHD_OFFSET(hdMessageTagProcessor_c, mRubyState, 8);
WWHD_OFFSET(hdMessageTagProcessor_c, mTagState, 0x18);
WWHD_SIZE(hdMessageTagProcessor_c, 0x2C);
WWHD_SIZE(hdMessageContext_c, 8);
WWHD_SIZE(hdMessageTag_c, 8);
WWHD_OFFSET(hdMessagePane_c, mpVtable, 8);
WWHD_OFFSET(hdMessagePane_c, mFlags, 0x44);
WWHD_OFFSET(hdMessagePaneVtable_c, mCopyString, 0xCC);
WWHD_SIZE(hdMessagePaneList_c, 12);

struct hdMessageHeaderStatics_c {
    be<f32> mFloatPair[2];
    be<u8> mFlag, mByte;
    u8 padding[2];
    be<u32> mZeroWords[4];
};
struct hdMessageStaticRecord_c { u8 opaque[12]; };
struct hdMessageStaticRecords_c { hdMessageStaticRecord_c mFirst, mSecond, mThird; };
struct hdMessageData_c { u8 unused; };
WWHD_SIZE(hdMessageHeaderStatics_c, 0x1C);
WWHD_SIZE(hdMessageStaticRecords_c, 36);

void* hdMessage_alloc(u32);
void hdMessage_baseConstruct(hdMessageTagProcessor_c*);
void hdMessage_builderConstruct(hdMessageBuilder_c*);
void hdMessage_rubyConstruct(hdTextTagState_c*);
void hdMessage_tagConstruct(hdTextTagState_c*);
u32 hdMessage_baseProcess(hdMessageTagProcessor_c*, u32, hdMessageContext_c*);
u32 hdMessage_baseCalcRect(hdMessageTagProcessor_c*, u32, u32, hdMessageContext_c*);
u32 hdMessage_rubyProcess(hdTextTagState_c*, hdMessageContext_c*, u32, u32);
u32 hdMessage_tagCalcRect(hdTextTagState_c*, u32, hdMessageContext_c*, u32, u32);
void hdMessage_rubyReset(hdTextTagState_c*);
void hdMessage_tagReset(hdTextTagState_c*);
u32 hdMessage_setup(hdMessageBuilder_c*, u32, u32, u32, u32);
void hdMessage_registerStatic(void*);
void hdMessage_initFlag(void*);
void hdMessage_initByte(void*);

/* HD message-only and main window state views. Private reconstruction. */
#pragma once
#include "wwhd.h"
struct HdMessageState { u8 mOpaque; };
struct HdMessageStateController { u8 mOpaque[0x2C]; };
struct HdMessageText { u8 mPad[2]; be<u8> mWait; };
struct HdMessageData {
 be<u32> mStatus; u8 mPad4[0x228]; gptr<HdMessageText> mText; u8 mPad230[0x428];
 be<u8> mBusy; u8 mPad659[0x3E]; be<u8> mWait; be<u8> mWaitInput; u8 mPad699[0x13];
 be<s32> mTypingPause; u8 mPad6B0[4]; be<s32> mWaitTimer; u8 mPad6B8[0x265]; be<u32> mColor;
};
struct HdMessageCore { u8 mPad[0x928]; be<u8> mSkip; u8 mPad929[0x57]; be<f32> mTypingSpeed; };
struct HdMessageCorePointer { gptr<HdMessageCore> mCore; };
struct HdMessageManager { u8 mPad[0x38]; be<u32> mWindowCount; };
struct HdMessageManagerPointer { gptr<HdMessageManager> mManager; };
struct HdMessagePane { u8 mPad[0xAC]; be<u32> mColor1; be<u32> mColor2; };
struct HdMessageOnly {
 u8 mPad0[4]; gptr<gfn> mVTable; u8 mPad8[0x10]; HdMessageStateController mState;
 u8 mPad44[0xC]; be<u32> mFlags; be<u32> mInputFlags; u8 mPad58[0x10];
 gptr<HdMessagePane> mPane; u8 mPad6C[0x18]; be<u32> mAdvanceCount; u8 mPad88[8];
 be<s32> mShown; be<f32> mTypingFraction; u8 mPad98[0x28]; be<s32> mStops[10];
 be<u32> mStopIndex; be<u8> mPadEC; be<u8> mStopFlag; u8 mPadEE[0x2A];
 gptr<HdMessageData> mMessage; u8 mPad11C[0x10]; gptr<HdMessagePane> mWaitPane;
};
struct HdMessageMain {
 u8 mPad0[0x18]; HdMessageStateController mState; u8 mPad44[0x10]; be<u32> mInputFlags;
 u8 mPad58[0xC]; gptr<HdMessagePane> mPane; u8 mPad68[0x10]; be<u32> mAdvanceCount;
 be<s32> mStops[10]; be<u32> mStopIndex; u8 mPadA8[8]; gptr<HdMessageData> mMessage;
};
struct HdMessagePlay {
 u8 mPad[0x5BBA]; be<u8> mKind; u8 mPad5BBB[0xA]; be<u8> mWindowActive; be<u8> mWindowSlot;
 u8 mPad5BC7[0xB]; be<u8> mAdvanceCount; u8 mPad5BD3[0x4D]; be<u8> mInputBlocked;
 u8 mPad5C21[0xF]; be<u32> mMessageId; be<u32> mMessageFlags;
};
enum HdMessageStatus { HD_MESSAGE_CONTINUE=7, HD_MESSAGE_WAIT=10, HD_MESSAGE_ADVANCE=14 };
enum HdMessageFlags { HD_MESSAGE_FAST=1, HD_MESSAGE_SHOW_ALL=2 };
enum HdMessageVSlot { HD_MESSAGE_STEP_TEXT=0x434/4, HD_MESSAGE_CLOSE_FIRST=0x444/4, HD_MESSAGE_CLOSE_SECOND=0x4A4/4, HD_MESSAGE_ADVANCE_TEXT=0x4B4/4, HD_MESSAGE_NEXT_STOP=0x4D4/4 };
enum HdMessageLimits { HD_MESSAGE_ALL_SHOWN=0x14F };
WWHD_OFFSET(HdMessageOnly,mStops,0xC0);
WWHD_OFFSET(HdMessageOnly,mMessage,0x118);
WWHD_OFFSET(HdMessageOnly,mWaitPane,0x12C);
WWHD_OFFSET(HdMessageMain,mStops,0x7C);
WWHD_OFFSET(HdMessageMain,mMessage,0xB0);
WWHD_OFFSET(HdMessageData,mTypingPause,0x6AC);
WWHD_OFFSET(HdMessageData,mWaitTimer,0x6B4);
WWHD_OFFSET(HdMessageData,mColor,0x91D);
WWHD_OFFSET(HdMessagePlay,mMessageId,0x5C30);
void hdMessageChangeState(HdMessageStateController*,HdMessageState*);
void hdMessageRefresh(HdMessagePane*,HdMessageData*);
void hdMessageSetMode(HdMessageCore*,u32);
HdMessagePlay* hdMessageGetPlay();
void hdMessageSound(u32);

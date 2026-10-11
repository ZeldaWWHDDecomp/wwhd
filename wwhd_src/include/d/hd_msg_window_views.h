/* HD keyboard and message window views. Private reconstruction. */
#pragma once
#include "wwhd.h"
// HD: scroll panes and sequence save/load phases have no GameCube counterpart.
enum HdMsgMode { HD_MSG_CONTINUE = 7 };
enum HdMsgIoPhase { HD_MSG_IO_START, HD_MSG_IO_WAIT };
enum HdMsgIoStatus { HD_MSG_IO_BUSY = 2, HD_MSG_IO_PENDING = 3 };
struct HdMsgState { u8 mOpaque; };
struct HdMsgController { u8 mOpaque[0x2C]; };
struct HdMsgPane { u8 mPad[0x40]; be<f32> mHeight; be<u8> mFlags; be<u8> mAlpha; };
struct HdMsgAnimation { u8 mPad[0xD4]; gptr<void> mPlayer; };
struct HdMsgMessage { be<u32> mMode; u8 mPad[0x648]; gptr<const char> mNextText; };
struct HdMsgPlay { u8 mPad[0x62F1]; be<u8> mWindowAlpha; };
struct HdMsgScroll {
 u8 mPad0[0x18]; HdMsgController mState; gptr<HdMsgAnimation> mAnimation;
 u8 mPad48[0x24]; gptr<HdMsgPane> mPanes[3]; be<u32> mPaneIndices[3];
 u8 mPad84[0x1C]; be<u32> mTimer; u8 mPadA4[0x10];
 be<f32> mUpperLimit; be<f32> mLowerLimit; be<f32> mLineSpacing; be<f32> mTarget; be<f32> mPosition;
 union { gptr<const char> mLines[20]; struct { u8 mPadC8[4]; be<u32> mOpeningMode; }; };
 be<u32> mLineIndex; be<u32> mLineCount; u8 mPad120[0x18]; gptr<HdMsgMessage> mMessage;
};
struct HdMsgSequence {
 u8 mPad0[0x18]; HdMsgController mState; u8 mPad44[0x48]; be<u32> mTextOffset;
 u8 mPad90[4]; gptr<HdMsgState> mNextState; u8 mPad98[0x1C]; be<u32> mTimer; be<u32> mPhase;
 u8 mPadBC[0x1B]; be<u8> mWaitComplete;
};
struct HdMsgKeyboard { u8 mPad[0x58]; gptr<HdMsgPane> mPane; };
struct HdMsgPosition { be<f32> x; be<f32> y; be<f32> z; };
struct HdMsgSaveManager { u8 mPad[0x1C]; be<u32> mStatus; };
struct HdMsgManagerPointer { gptr<HdMsgSaveManager> mManager; };
struct HdMsgControlPointer { gptr<void> mManager; };
WWHD_OFFSET(HdMsgScroll,mState,0x18);
WWHD_OFFSET(HdMsgScroll,mPanes,0x6C);
WWHD_OFFSET(HdMsgScroll,mOpeningMode,0xCC);
WWHD_OFFSET(HdMsgScroll,mLineIndex,0x118);
WWHD_OFFSET(HdMsgScroll,mMessage,0x138);
WWHD_OFFSET(HdMsgSequence,mTimer,0xB4);
WWHD_OFFSET(HdMsgSequence,mWaitComplete,0xD7);
WWHD_SIZE(HdMsgPosition,12);
void hdMsgChangeState(HdMsgController*,HdMsgState*);

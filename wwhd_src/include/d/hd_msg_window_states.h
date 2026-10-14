#pragma once
#include "d/hd_msg_window_base.h"
#include "d/hd_message_parts.h"
#include "d/hd_input_ctrl.h"
// Additional message-window states; shared base fields retain the merged layout.
struct hdMsgWindowStates_c : WindowBase {
    void enterStop();
    void enterWaitEnd();
    void enterWaitInputEnd();
    void enterAutoWait();
    void enterAutoWaitEnd();
    void enterHandStop();
    void enterClose();
    void enterCloseBlocked();
    void updateWaitInputEnd();
    void updateSelectionConfirm();
    void updateAccept();
    void updateClose();
    void updateCloseBlocked();
    void updateFinishClose();
    void updateStopEnd();
    void updateAutoStopEnd();
    void blockOpen();
    void leaveNumberSelect();
    void enterSelectTwo();
    void enterSelectThree();
    void resetNumberSelection();
    void resetCloseRequest();
    void clearCurrentNextMessage();
    void updateCameraToOpen();
    void updateStopInput();
    void enterAccept();
    void enterFinishAccept();
    void enterAdvanceClose();
    void enterFinishAdvance();
    void enterNextMessage();
    void updateNextMessage();
    void updateFinishAccept();
    void updateAdvanceStop();
    void updateNumberMode();
    void enterSelectionConfirm();
    void hideSelectionBackground();
    void enterSelectionClose();
    void updateSelectionClose();
    void updateSelectionLayout();
    void positionTwoChoices();
    void positionThreeChoices();
    void updateAutoStopInput();
    void updateHandMode();
    void updateHandStopEnd();
    void update();
    void startAnimation(u32 group,u32 animation);
    void setAnimationFrame(u32 group,u32 animation,f32 frame);
    u32 animationFinished(u32 group);
    static hdMsgWindowStates_c* construct(hdMsgWindowStates_c* window);
    void updateWhileActive(u32 value);
    void readController();
    void enterChoiceLayout();
    void updateChoiceLayout();
    void advanceText();
    void finishText();
    void updateNumberInput(be<u32>* count);
    void positionNumberPane();
    void reset();
    void prepareMessage();
};

struct MsgWindowPanes_l {
 u8 mReserved[0x44]; gptr<hdPartsLayout_c> mpLayout; u8 mReserved48[0x10]; be<u32> mHeldInput;
 be<u32> mMessageId; be<u32> mWindowMode; gptr<WindowPane> mpSelectionBackground;
 gptr<WindowPane> mpTextPane; gptr<WindowPane> mpTwoChoices; gptr<WindowPane> mpThreeChoices;
 gptr<WindowPane> mpTwoChoiceCursor; gptr<WindowPane> mpThreeChoiceCursor;
};
WWHD_OFFSET(MsgWindowPanes_l, mpSelectionBackground,0x64);
WWHD_OFFSET(MsgWindowPanes_l, mpTwoChoices,0x6C);
WWHD_OFFSET(MsgWindowPanes_l, mpThreeChoiceCursor,0x78);

struct ChoiceTextPane_l {
 u8 mReserved[0x40]; be<f32> mHeight; u8 mReserved44[4]; be<u32> mMatrix[12];
 u8 mReserved78[0x40]; be<f32> mFontWidth; be<f32> mFontHeight; be<f32> mLineSpace;
};
WWHD_OFFSET(ChoiceTextPane_l,mMatrix,0x48);
WWHD_OFFSET(ChoiceTextPane_l,mFontWidth,0xB8);
struct ChoicePositions_l { be<f32> mPositions[9]; };
struct ChoiceFont_l { u8 mReserved[4]; gptr<gfn> mpVTable; };
WWHD_OFFSET(ChoiceFont_l,mpVTable,4);

struct MsgPlayExtension_l { u8 mReserved[0x5BB2]; be<u8> mMessageActive; };
WWHD_OFFSET(MsgPlayExtension_l,mMessageActive,0x5BB2);

struct MsgInitialFields_l {
 u8 mReserved[0x2C]; be<u32> mReceiverVTable; be<u32> mStateVTable; u8 mReserved34[0x24];
 be<u32> mHeldInput; be<u32> mMessageId; be<u32> mWindowMode;
 gptr<WindowPane> mpBackground; gptr<WindowPane> mpText; gptr<WindowPane> mpTwo; gptr<WindowPane> mpThree;
 gptr<WindowPane> mpTwoCursor; gptr<WindowPane> mpThreeCursor; gptr<WindowPane> mpNumber; be<u32> mNumberValue;
 u8 mReserved84[0x68]; be<u8> mReady; be<u8> mStopReady; u8 mReservedEE[2]; be<u32> mWaitKind;
 be<u32> mSelection; be<u32> mPosition[3]; be<f32> mWidth; be<f32> mHeight; be<f32> mFrame;
 be<u8> mOpen; be<u8> mBlocked; be<u32> mColor; u8 mReserved116[2]; gptr<WindowMessage> mpMessage;
};
WWHD_OFFSET(MsgInitialFields_l,mHeldInput,0x58);
WWHD_OFFSET(MsgInitialFields_l,mReady,0xEC);
WWHD_OFFSET(MsgInitialFields_l,mColor,0x112);
WWHD_SIZE(MsgInitialFields_l,0x11C);
struct MsgIdleState_l { u8 mReserved[8]; gptr<gfn> mpNameVTable; };

struct MsgChoicePane_l {
 u8 mReserved[0x3C]; be<u32> mHeight; u8 mReserved40[4]; be<u8> mFlags;
 u8 mReserved45[0x1F7]; u8 mMessage[0x688]; be<u32> mMessageHeight;
 u8 mReserved8C8[0x25C]; be<u32> mMessageSpeed;
};
WWHD_OFFSET(MsgChoicePane_l,mMessage,0x23C);
WWHD_OFFSET(MsgChoicePane_l,mMessageHeight,0x8C4);
WWHD_OFFSET(MsgChoicePane_l,mMessageSpeed,0xB24);
struct MsgChoiceMessage_l {
 u8 mReserved[0x644]; be<s32> mLineCount; u8 mReserved648[4]; be<s32> mOffset; be<s32> mCount; be<s32> mShown;
 u8 mReserved658[0x68]; be<u32> mChoicePositions[9]; u8 mReserved6E4[0x204]; be<u32> mSpeed; u8 mReserved8EC[4]; be<f32> mNumberOffset; be<s32> mNumberDigits;
};
WWHD_OFFSET(MsgChoiceMessage_l,mSpeed,0x8E8);

struct MsgNumberPlay_l {
 u8 mReserved[0x5BA2]; be<s16> mNumber; u8 mReserved5BA4[0x21]; be<u8> mNumberMode; be<u8> mNumberIndex;
 u8 mReserved5BC7[0x6D]; be<u32> mNumberControl;
};
WWHD_OFFSET(MsgNumberPlay_l,mNumberMode,0x5BC5);
WWHD_OFFSET(MsgNumberPlay_l,mNumberControl,0x5C34);
struct MsgNumberLimits_l { be<u8> mMaximum; be<u8> mSelected; };
struct MsgNumberFactors_l { be<f32> mValues[25]; };
struct MsgNumberPane_l { u8 mReserved[0x1C]; be<f32> mX; be<f32> mY; be<f32> mZ; u8 mReserved28[0x1C]; be<u8> mFlags; };
WWHD_OFFSET(MsgNumberPane_l,mZ,0x24);
WWHD_OFFSET(MsgNumberPane_l,mFlags,0x44);
struct MsgMetricPane_l {
 u8 mReserved[0x1C]; be<f32> mX; be<f32> mY; u8 mReserved24[0x18]; be<f32> mHeight; u8 mReserved40[0x78];
 be<f32> mFontWidth; u8 mReservedBC[4]; be<f32> mLineSpace; be<f32> mCharacterSpace;
};
WWHD_OFFSET(MsgMetricPane_l,mFontWidth,0xB8);
struct MsgBuffer_l { gptr<void> mpBuffer; be<u32> mCount; };
struct MsgPreparation_l {
 u8 mReserved[8]; gptr<MsgBuffer_l> mpBuffer; u8 mReservedC[0x10C]; be<u32> mBufferIndex;
 u8 mReserved11C[0x7FD]; be<u32> mInitialColor; be<u32> mTextColor; be<u8> mInitialized;
};
WWHD_OFFSET(MsgPreparation_l,mBufferIndex,0x118);
WWHD_OFFSET(MsgPreparation_l,mInitialColor,0x919);
WWHD_OFFSET(MsgPreparation_l,mInitialized,0x921);
struct MsgTag_l { be<u16> mType; be<u16> mKind; be<u16> mValue; };

struct MsgConfiguration_l { u8 mReserved[0xC]; u8 mTextBuffer[0x114]; u8 mPositionData[1]; };
WWHD_OFFSET(MsgConfiguration_l,mPositionData,0x120);

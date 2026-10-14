/* HD event-window position/pane selection. This local game view names fields
 * absent from the merged event header; the framework base stays canonical. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_screen_msg_window_event.h"
struct hdMsgEventVisiblePane_l : hdPartsPaneSRT_c {
    void show() { mFlags = (u8(mFlags) & ~1u) | 1u; }
    void hide() { mFlags = u8(mFlags) & ~1u; }
};
WWHD_OFFSET(hdMsgEventVisiblePane_l, mFlags, 0x44);
WWHD_SIZE(hdMsgEventVisiblePane_l, 0x46);
struct hdMsgEventPositionMessage_l {
    u8 _00[0x11C]; be<u32> mMessageId;
};
WWHD_OFFSET(hdMsgEventPositionMessage_l, mMessageId, 0x11C);
WWHD_SIZE(hdMsgEventPositionMessage_l, 0x120);
struct hdMsgEventChildDesc_l;
struct hdMsgEventPosition_c : hdParts_c {
    u8 _050[0xC]; be<u32> mMessageId;
    gptr<hdMsgEventVisiblePane_l> mpAllPane;
    u8 _064[4];
    gptr<void> mpActiveTextPane;
    u8 _06C[0x14];
    gptr<hdMsgEventVisiblePane_l> mpPositionPane;
    u8 _084[0xF8-0x84]; cXyz mWorldPosition;
    u8 _104[0xC]; be<u8> mMessageChanged; u8 _111[1];
    be<u32> mSmallTextColor;
    u8 _116[2];
    gptr<hdMsgEventPositionMessage_l> mpMessage;
    be<u32> mWindowType;
    gptr<hdMsgEventVisiblePane_l> mWindowPanes[8];
    gptr<hdMsgEventVisiblePane_l> mTextPanes[3];
    be<u32> mTextPaneIndex;
    gptr<hdMsgEventVisiblePane_l> mPositionPanes[16];
    u32 createEventChildren(hdPartsDesc_c* desc);
    u32 loadEvent(u32 kind, u32 flags, void* description);
    void executeEvent();
    u32 setEventMessage(u32 type, u32 unused, u32 id);
    u32 createEventLayout(void* name, u32 kind, u32 option, u32 flags, void* description);
    void selectPositionAnimation();
    void finalizeEvent();
    void resetTextPosition();
    void bindWindowPanes();
    void bindPositionPanes();
    void bindTextPanes();
    void selectSongTextPane();
    void selectLargeTextPane();
    void selectMessageTextPane();
    void hideTextPanes();
    void hidePosition();
    void showPosition();
    void showPositionOnce();
    void showPositionLoop();
    u32 positionComplete();
    u32 positionProgress();
    u32 positionActive();
    hdParts_c* positionPart() { return *mChildren.safeSlot(15); }
};
WWHD_OFFSET(hdMsgEventPosition_c, mpAllPane, 0x60);
WWHD_OFFSET(hdMsgEventPosition_c, mpActiveTextPane, 0x68);
WWHD_OFFSET(hdMsgEventPosition_c, mpMessage, 0x118);
WWHD_OFFSET(hdMsgEventPosition_c, mWindowPanes, 0x120);
WWHD_OFFSET(hdMsgEventPosition_c, mTextPanes, 0x140);
WWHD_OFFSET(hdMsgEventPosition_c, mTextPaneIndex, 0x14C);
WWHD_OFFSET(hdMsgEventPosition_c, mPositionPanes, 0x150);
WWHD_SIZE(hdMsgEventPosition_c, 0x190);

struct hdMsgEventTextPane_l : hdMsgEventVisiblePane_l {
    u8 _46[2]; be<u32> mMatrixWords[12];
    u8 _78[0xAC-0x78]; be<u32> mColorTop, mColorBottom;
};
WWHD_OFFSET(hdMsgEventTextPane_l, mColorTop, 0xAC);
WWHD_OFFSET(hdMsgEventTextPane_l, mColorBottom, 0xB0);
WWHD_SIZE(hdMsgEventTextPane_l, 0xB4);
WWHD_OFFSET(hdMsgEventPosition_c, mSmallTextColor, 0x112);
struct hdMsgEventTextNames_l { hdUiSafeString mNames[3]; };
WWHD_SIZE(hdMsgEventTextNames_l, 0x18);


struct hdMsgEventWindowScalar_l { be<f32> mValue; };
WWHD_OFFSET(hdMsgEventWindowScalar_l, mValue, 0);
WWHD_SIZE(hdMsgEventWindowScalar_l, 4);

void* hdMsgEvent_allocateChild(u32, void*, s32);

WWHD_OFFSET(hdMsgEventPosition_c, mpPositionPane, 0x80);

u32 hdMsgEvent_currentBgm();

WWHD_OFFSET(hdMsgEventPosition_c, mMessageId, 0x5C);
WWHD_OFFSET(hdMsgEventPosition_c, mWorldPosition, 0xF8);
WWHD_OFFSET(hdMsgEventPosition_c, mMessageChanged, 0x110);
WWHD_OFFSET(hdMsgEventPosition_c, mWindowType, 0x11C);
WWHD_OFFSET(hdMsgEventTextPane_l, mMatrixWords, 0x48);

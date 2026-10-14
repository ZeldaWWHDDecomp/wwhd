/* HD-only event-message state handlers. Canonical merged window layout is reused. */
#pragma once
#include "d/hd_screen_msg_window_event.h"
#include "d/hd_parts.h"
struct hdMsgEventHandlers_c : hdMsgEvent_window_c {
    static hdMsgEventHandlers_c* construct(hdMsgEventHandlers_c* self);
    void enterSongMessage();
    void enterTextMessage();
    void configureSongPanes();
    void loopVisibleWindow();
    void prepareMessage();
    void updateOpeningMessage();
    void updateSequenceText();
    void updateActiveText();
    void beginTyping();
    void refreshText();
    u32 songLengthFor(s32 index);
    void initializeInput();
    void requestCancel();
    void executeSpecial();
    void beginClose();
    void updateClose();
    void enterMessage();
    void enterTimedMessage();
    void enterBaseWait();
    void enterTextWait();
    void enterCloseWait();
    void exitBaseWait();
    void startCloseAnimation();
    void updateOpening();
    void updateClosing();
    void updateClosingText();
    void captureAdvanceRequest();
};
void hdMsgEvent_prepareMessage(hdMsgEvent_window_c* window);
void hdMsgEvent_baseEnterTextWait(hdMsgEvent_window_c* window);
void hdMsgEvent_baseExitWait(hdMsgEvent_window_c* window);

struct hdMsgEventCallbacks_l : hdPartsVtbl_c {
    u8 _0C8[0x41C-0xC8]; gfn mClear;
    u8 _420[0xC]; gfn mUpdate;
    u8 _430[0x14]; gfn mOnCreate;
    u8 _448[4]; gfn mOnMessage; u8 _450[0x54]; gfn mPrepareText;
    u8 _4A8[0xC]; gfn mBeginClose;
    u8 _4B8[0x1C]; gfn mIsCloseComplete;
};
WWHD_OFFSET(hdMsgEventCallbacks_l, mClear, 0x41C);
WWHD_OFFSET(hdMsgEventCallbacks_l, mBeginClose, 0x4B4);
WWHD_OFFSET(hdMsgEventCallbacks_l, mIsCloseComplete, 0x4D4);
WWHD_SIZE(hdMsgEventCallbacks_l, 0x4D8);
u32 hdMsgEvent_getCoreStatus(void* core);


WWHD_OFFSET(hdMsgEventCallbacks_l, mUpdate, 0x42C);
void hdMsgEvent_initializeMessage(hdMsgEvent_message_c*, u32, u32);

extern hdMsgEvent_state_c& sEventOpeningSpecial;

void* hdMsgEvent_allocateWindow(u32);


WWHD_OFFSET(hdMsgEventCallbacks_l, mOnCreate, 0x444);
WWHD_OFFSET(hdMsgEventCallbacks_l, mPrepareText, 0x4A4);
void hdMsgEvent_setMessageId(hdMsgEvent_message_c*, u32, u32);
void hdMsgEvent_startWindowAnimation(hdMsgEvent_window_c*);

struct hdMsgEventMessageFlags_l {
    u8 _000[0x90C]; be<u8> mCancelRequested;
};
WWHD_OFFSET(hdMsgEventMessageFlags_l, mCancelRequested, 0x90C);
WWHD_SIZE(hdMsgEventMessageFlags_l, 0x90D);
struct hdMsgEventCancelFlag_l { u8 _000[0x1E9]; be<u8> mCancelRequested; };
WWHD_OFFSET(hdMsgEventCancelFlag_l, mCancelRequested, 0x1E9);
WWHD_SIZE(hdMsgEventCancelFlag_l, 0x1EA);
WWHD_OFFSET(hdMsgEventCallbacks_l, mOnMessage, 0x44C);

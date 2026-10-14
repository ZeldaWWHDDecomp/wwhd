/* HD: the second message-only screen "MsgOnly_00" (hd_screen_msg_only2.cpp): a message without a window
 * frame, shown and advanced through twelve states (in, out now, the close waits, the NPC answer, page
 * changes, stop, continue, close). Five of its state functions are hd_screen_msg_window_main.cpp's
 * (HdMessageMain). No GameCube counterpart. Layout, pane, animation and state names are HD strings; class,
 * member and method names are ours ("probably" marks a role read from its use only). Local game-derived
 * */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_screen_msg_window_main2.h"
#include "d/hd_input_ctrl.h"

/* the message text work (HdMsgText / WindowMessage) and the message system, by address */
inline void hdMsgText_restartPlay_l(void* message, u32 stop, u32 limit) { gabi::call(0x025F6FD8, message, stop, limit); }
inline void hdMsgText_setNumberIdle_l(void* message, u32 messageId) { gabi::call(0x025F6C54, message, messageId); }
inline void hdMsgControl_setStatus_l(void* control, u32 status) { gabi::call(0x025F74D0, control, status); }
inline void hdMsgResMgr_getMessage_l(void* manager, void* message, hdUiSafeString* label, void* out) { gabi::call(0x025F50E8, manager, message, label, out); }
/* the message sounds (m_Do_audio) */
inline void hdMsgOnly2_talkIn_l() { gabi::call(0x025E1C58); }
inline void hdMsgOnly2_talkOut_l() { gabi::call(0x025E1C64); }
inline void hdMsgOnly2_seStart_l(u32 sound) { gabi::call(0x025E1988, sound); }
inline void hdMsgOnly2_messageSe_l(u32 voice, u32 arg, s32 reverb) { gabi::call(0x025E1C3C, voice, arg, reverb); }
inline s32 hdMsgOnly2_getReverb_l(s32 roomNo) { return gabi::call<s32>(0x02520540, roomNo); }
/* 0200579C: hd_ui_02004A3C's setAnimationSpeed (here: the frame to restart from) */
inline void hdAnimSet_setSpeed_l(hdAnimSet* set, u32 entry, u32 slot, f32 speed) { gabi::call(0x0200579C, set, entry, slot, speed); }
/* 026B9820: an empty function the label copy calls */
inline void hdMsgOnly2_noop_l(void* string) { gabi::call(0x026B9820, string); }
/* OSBlockMove (coreinit import; 028FDC00 is its stub): copy `count` characters */
inline void hdMsgOnly2_copyChars_l(void* dst, void* src, u32 count, u32 arg) { gabi::call(0xC0009988, dst, src, count, arg); }

/* the message text work fields used here (our view) */
struct hdMsgOnly2Message_l {
    /* 0x000 */ be<u32> mStatus;       /* 7: continue, 10: wait, 14: answer; its low byte is the play's message state */
    /* 0x004 */ u8 _004[0x11C];
    /* 0x120 */ u8 mOut[0x10C];        /* the message lookup's output */
    /* 0x22C */ gptr<hdMsgMain2Input_l> mpInput;
    /* 0x230 */ u8 _230[0x41C];
    /* 0x64C */ be<s32> mNextStop;
    /* 0x650 */ u8 _650[0x38];
    /* 0x688 */ be<f32> mWidth;
    /* 0x68C */ u8 _68C[0xB];
    /* 0x697 */ be<u8> mAutoClose;
    /* 0x698 */ be<u8> mHandClose;
    /* 0x699 */ u8 _699[0x280];
    /* 0x919 */ u8 mColors[8];         /* unaligned */
};
WWHD_OFFSET(hdMsgOnly2Message_l, mpInput, 0x22C);
WWHD_OFFSET(hdMsgOnly2Message_l, mNextStop, 0x64C);
WWHD_OFFSET(hdMsgOnly2Message_l, mWidth, 0x688);
WWHD_OFFSET(hdMsgOnly2Message_l, mColors, 0x919);
enum {
    MSG_ONLY2_STATUS_CONTINUE = 7,
    MSG_ONLY2_STATUS_WAIT = 10,
    MSG_ONLY2_STATUS_ANSWER = 14,
};
/* the play state's message fields (dComIfGp_get(); our view) */
struct hdMsgOnly2Play_l {
    /* 0x0000 */ u8 _0000[0x5BB2];
    /* 0x5BB2 */ be<u8> mAnswer;       /* 13: probably a yes / no question */
    /* 0x5BB3 */ be<u8> mMessageState;  /* 6 out, 15 continue, 16 close, 17 closing, 18 all out */
    /* 0x5BB4 */ u8 _5BB4[5];
    /* 0x5BB9 */ be<u8> mAnswerResult;
    /* 0x5BBA */ be<u8> mKind;          /* 39: probably a sign */
    /* 0x5BBB */ u8 _5BBB[0x14];
    /* 0x5BCF */ be<u8> mDemoMode;
    /* 0x5BD0 */ u8 _5BD0[2];
    /* 0x5BD2 */ be<u8> mAdvanceCount;
    /* 0x5BD3 */ be<u8> mHandInput;
    /* 0x5BD4 */ u8 _5BD4[0x4D];
    /* 0x5C21 */ be<u8> mZoomActive;
};
WWHD_OFFSET(hdMsgOnly2Play_l, mMessageState, 0x5BB3);
WWHD_OFFSET(hdMsgOnly2Play_l, mKind, 0x5BBA);
WWHD_OFFSET(hdMsgOnly2Play_l, mAdvanceCount, 0x5BD2);
WWHD_OFFSET(hdMsgOnly2Play_l, mZoomActive, 0x5C21);
/* the controller words this screen reads (*(hdInputManager*); our view) */
struct hdMsgOnly2Pad_l {
    /* 0x000 */ be<u32> mTrigger;
    /* 0x004 */ u8 _004[4];
    /* 0x008 */ be<u32> mRepeat;
    /* 0x00C */ u8 _00C[0x100];
    /* 0x10C */ be<u32> mHold;
};
/* a sead FixedSafeString<6> (the label "dummy") */
struct hdMsgOnly2Label_l {
    /* 0x00 */ gptr<be<u8>> mText;
    /* 0x04 */ gptr<void> mVtable;
    /* 0x08 */ be<s32> mCapacity;
    /* 0x0C */ be<u8> mBuffer[6];
};
enum {
    MSG_ONLY2_ANIM_IN_ALL = 0,
    MSG_ONLY2_ANIM_OUT_MSG = 1,
    MSG_ONLY2_ANIM_IN_MSG = 2,
    MSG_ONLY2_ANIM_OUT_ALL = 3,
};

struct hdScreenMsgOnly2_c : hdParts_c {
    /* 0x50 */ be<u32> mHold;
    /* 0x54 */ be<u32> mTrigger;          /* bit 0: decide, bit 1: cancel */
    /* 0x58 */ be<u32> mPressed;
    /* 0x5C */ be<u32> mMessageId;
    /* 0x60 */ gptr<hdMsgMain2Pane_l> mpAllPane;              /* "N_All_00" */
    /* 0x64 */ gptr<hdMsgMain2TextBox_l> mpTextPanes[4];      /* T_Msg_00 / T_MsgLine1_00 / T_ChoiseL_00 / T_ChoiseR_00 */
    /* 0x74 */ be<u32> m74;
    /* 0x78 */ be<u32> mAdvanceCount;
    /* 0x7C */ be<s32> mStops[10];
    /* 0xA4 */ be<u32> mStopIndex;
    /* 0xA8 */ be<u32> mA8;
    /* 0xAC */ u8 mTextColor[4];                               /* unaligned */
    /* 0xB0 */ gptr<hdMsgOnly2Message_l> mpMessage;

    static void destroy(hdScreenMsgOnly2_c* i_this, s32 flags);
    void updateText();
    void playAnimation(u32 animation);
    void inAllEnter();
    u32 isAnimationEnd(u32 animation);
    void inAllExecute();
    void outNowEnter();
    void outNowExecute();
    void npcAnswerWaitExecute();
    void outMsgEnter();
    void outMsgExecute();
    void inMsgExecute();
    void outAllEnter();
    void outAllExecute();
    u32 hasDecide();
    void continueExecute();
    void setAnimationFrame(u32 animation, f32 frame);
    void reset();
    void closeEnter();
    void closeExecute();
    static hdScreenMsgOnly2_c* construct(hdScreenMsgOnly2_c* i_this);
    void findAllPane();
    void findTextPanes();
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void executeState();
    void draw(void* info);
    void closeFinish();
    u8 open(u32 arg, u32 messageId);
    u32 createChild(hdPartsDesc_c* desc);
    u32 loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);

    void startMessage(bool first);
    hdMsgOnly2Play_l* play() { return (hdMsgOnly2Play_l*)dComIfGp_get(); }
};
WWHD_OFFSET(hdScreenMsgOnly2_c, mMessageId, 0x5C);
WWHD_OFFSET(hdScreenMsgOnly2_c, mpTextPanes, 0x64);
WWHD_OFFSET(hdScreenMsgOnly2_c, mStops, 0x7C);
WWHD_OFFSET(hdScreenMsgOnly2_c, mTextColor, 0xAC);
WWHD_OFFSET(hdScreenMsgOnly2_c, mpMessage, 0xB0);
WWHD_SIZE(hdScreenMsgOnly2_c, 0xB4);

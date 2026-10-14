/* HD: the second copy of the main message window "MsgWindowMain_00" (hd_screen_msg_window_main2.cpp): a
 * message window of FW-parts_msg_base's message-window class with two- and three-choice windows, a number
 * selection, a key icon part and a tail that points at the speaker ("Position" animation). No GameCube
 * counterpart. Layout, pane, animation and state names are HD strings; class, member and method names are
 * ours ("probably" marks a role read from its use only).
 *
 * The base is FW-parts_msg_base's message-window class (hdMsgWindowStates_c on WindowBase,
 * d/hd_msg_window_states.h): its constructor and methods are called through windowStates(). The merged
 * WindowBase is not an hdParts_c and leaves most of the fields used here reserved, so hdMsgWindowBase_l
 * stays a view of the same object on hdParts_c; static_asserts tie its shared fields to WindowBase. */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_key_icon.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_msg_window_base.h"
#include "d/hd_msg_window_states.h"
#include "d/hd_layout_mgr.h"
#include "d/hd_screen_cursor.h"
#include "d/d_com_inf_game.h"

/* 026FC4D8 destroyLayoutWaitScreen (d/hd_layout_mgr.h) is the message-window base's deleting destructor */

/* the message manager (*0x101F4B5C): which message window is active */
struct hdMsgManager_l {
    /* 0x000 */ u8 _000[0x14];
    /* 0x014 */ u8 mMessage[0x938];   /* the message the windows show (WindowMessage) */
    /* 0x94C */ be<u32> mMode;        /* 1: probably a message is running */
    /* 0x950 */ u8 _950[4];
    /* 0x954 */ gptr<void> mpMainWindow2; /* this window, while loaded */
    /* 0x958 */ gptr<void> mpMsgOnly2;    /* the message-only screen (hdScreenMsgOnly2_c), while loaded */
};
WWHD_OFFSET(hdMsgManager_l, mMessage, 0x14);
WWHD_OFFSET(hdMsgManager_l, mMode, 0x94C);
WWHD_OFFSET(hdMsgManager_l, mpMainWindow2, 0x954);
WWHD_OFFSET(hdMsgManager_l, mpMsgOnly2, 0x958);
/* 025F85C0: place the message window at a screen position (0: top, others by the speaker; our reading) */
inline u32 hdMsgManager_setWindowPosition_l(hdMsgManager_l* manager, u32 visible, u32 position, void* pane, u32 arg) {
    return gabi::call<u32>(0x025F85C0, manager, visible, position, pane, arg);
}

/* the message's input description (WindowMessage::mpInput) */
struct hdMsgMain2Input_l {
    /* 0x00 */ u8 _00[3];
    /* 0x03 */ be<u8> mTailKind;      /* 0: towards the speaker, 1: up, 3: down, others: centre */
    /* 0x04 */ u8 _04[6];
    /* 0x0A */ be<u8> mVoice;         /* the voice to play (0: none) */
    /* 0x0B */ be<u8> mSpeaker;       /* the speaking actor's number (compared with the play's) */
};
/* the play state fields used here (dComIfGp_get(); our view) */
struct hdMsgMain2Play_l {
    /* 0x0000 */ u8 _0000[0x5B2C];
    /* 0x5B2C */ gptr<u8> mpPlayer;   /* +0x37C: the player's head position, probably */
    /* 0x5B30 */ u8 _5B30[0xA2];
    /* 0x5BD2 */ be<u8> mAdvanceCount;
    /* 0x5BD3 */ be<u8> mHandInput;
    /* 0x5BD4 */ u8 _5BD4[0x5C];
    /* 0x5C30 */ be<u32> mSpeaker;
};
WWHD_OFFSET(hdMsgMain2Play_l, mpPlayer, 0x5B2C);
WWHD_OFFSET(hdMsgMain2Play_l, mAdvanceCount, 0x5BD2);
WWHD_OFFSET(hdMsgMain2Play_l, mSpeaker, 0x5C30);

/* the text box fields the window changes (nw::lyt::TextBox; our view) */
struct hdMsgMain2TextBox_l {
    /* 0x00 */ u8 _00[0x3C];
    /* 0x3C */ be<f32> mWidth;
    /* 0x40 */ u8 _40[4];
    /* 0x44 */ be<u8> mFlags;
    /* 0x45 */ u8 _45[3];
    /* 0x48 */ be<f32> mMatrix[12];   /* the global matrix: [3] x, [7] y */
    /* 0x78 */ u8 _78[0x34];
    /* 0xAC */ be<u32> mColor;
    /* 0xB0 */ u8 _B0[0x20];
    /* 0xD0 */ be<u8> mTextAlign;     /* bits 0-1 horizontal, 2-3 vertical */
    /* 0xD1 */ u8 _D1[0x23];
    /* 0xF4 */ be<u8> mTextFlags;     /* bit 5: the text needs a new layout */
    /* 0xF5 */ u8 _F5[0x2B];
    /* 0x120 */ be<u8> mShrink;
};
WWHD_OFFSET(hdMsgMain2TextBox_l, mMatrix, 0x48);
WWHD_OFFSET(hdMsgMain2TextBox_l, mColor, 0xAC);
WWHD_OFFSET(hdMsgMain2TextBox_l, mTextAlign, 0xD0);
WWHD_OFFSET(hdMsgMain2TextBox_l, mTextFlags, 0xF4);
WWHD_OFFSET(hdMsgMain2TextBox_l, mShrink, 0x120);

/* the panes of the window (nw::lyt::Pane fields used here) */
struct hdMsgMain2Pane_l {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ gptr<hdCursorPaneVtbl_c> mVtable;
    /* 0x0C */ u8 _0C[0x38];
    /* 0x44 */ be<u8> mFlags;
};
enum {
    MSG_MAIN2_PANE_VISIBLE = 0x01,
    MSG_MAIN2_TEXT_CAPACITY = 335,
};

/* animation entries of "MsgWindowMain_00" */
enum hdMsgMain2Anim_e {
    MSG_MAIN2_ANIM_IN_ALL = 0,
    MSG_MAIN2_ANIM_LOOP_SEL_NUMBER = 1,
    MSG_MAIN2_ANIM_IN_SUB_WINDOW = 2,
    MSG_MAIN2_ANIM_OUT_SUB_WINDOW = 3,
    MSG_MAIN2_ANIM_LOOP_ALL = 4,
    MSG_MAIN2_ANIM_OUT_ALL = 5,
    MSG_MAIN2_ANIM_POSITION = 6,     /* its frame: the tail (0 down, 1 centre, 2 up) */
};

/* FW-parts_msg_base's message-window base (0x118 bytes): the fields this window uses */
struct hdMsgWindowBase_l : hdParts_c {
    /* the merged message-window base (the same object) */
    hdMsgWindowStates_c* windowStates() { return (hdMsgWindowStates_c*)(void*)this; }

    /* 0x050 */ u8 _050[0xC];
    /* 0x05C */ be<u32> mMessageId;
    /* 0x060 */ gptr<hdMsgMain2Pane_l> mpAllPane;        /* "N_All_00" */
    /* 0x064 */ gptr<hdMsgMain2Pane_l> mpSubWindowPane;  /* "N_SubWindow_00" */
    /* 0x068 */ gptr<hdMsgMain2TextBox_l> mpTextPane;    /* "T_Msg_00" */
    /* 0x06C */ gptr<hdMsgMain2TextBox_l> mpChoiceTexts[2]; /* "T_Choise2_00", "T_Choise3_00" */
    /* 0x074 */ gptr<hdMsgMain2Pane_l> mpChoiceWindows[2]; /* "W_Choise2_00", "W_Choise3_00" */
    /* 0x07C */ gptr<hdMsgMain2Pane_l> mpSelNumberPane;  /* "P_SelNumber_00" */
    /* 0x080 */ gptr<hdMsgMain2Pane_l> mpKeyIconPane;    /* "L_KeyIcon_00" */
    /* 0x084 */ be<u32> mAdvanceCount;
    /* 0x088 */ u8 _088[0x10];
    /* 0x098 */ be<s32> mStopOffsets[10];
    /* 0x0C0 */ be<s32> mStopCounts[10];
    /* 0x0E8 */ be<u32> mStopIndex;
    /* 0x0EC */ be<u8> mPlaced;           /* the window has a screen position */
    /* 0x0ED */ be<u8> mPlaceRequest;     /* a new position is wanted */
    /* 0x0EE */ u8 _0EE[2];
    /* 0x0F0 */ be<u32> mPosition;
    /* 0x0F4 */ be<u32> mRequestedPosition;
    /* 0x0F8 */ cXyz mSpeakerPos;          /* the world position the tail points at */
    /* 0x104 */ be<f32> mTextX;            /* the text pane's original position */
    /* 0x108 */ be<f32> mTextY;
    /* 0x10C */ u8 _10C[4];
    /* 0x110 */ be<u8> mOpenRequested;
    /* 0x111 */ be<u8> mCloseRequested;
    /* 0x112 */ u8 mTextColor[4];          /* the text pane's colour (unaligned) */
    /* 0x116 */ u8 _116[2];
    /* 0x118 */ gptr<WindowMessage> mpMessage;
};
WWHD_OFFSET(hdMsgWindowBase_l, mMessageId, 0x5C);
static_assert(offsetof(hdMsgWindowBase_l, mMessageId) == offsetof(WindowBase, mMessageId));
static_assert(offsetof(hdMsgWindowBase_l, mpTextPane) == offsetof(WindowBase, mpTextPane));
static_assert(offsetof(hdMsgWindowBase_l, mpSelNumberPane) == offsetof(WindowBase, mpNumberPane));
static_assert(offsetof(hdMsgWindowBase_l, mAdvanceCount) == offsetof(WindowBase, mAdvanceCount));
static_assert(offsetof(hdMsgWindowBase_l, mStopOffsets) == offsetof(WindowBase, mStopOffsets));
static_assert(offsetof(hdMsgWindowBase_l, mStopCounts) == offsetof(WindowBase, mStopCounts));
static_assert(offsetof(hdMsgWindowBase_l, mStopIndex) == offsetof(WindowBase, mStopIndex));
static_assert(offsetof(hdMsgWindowBase_l, mpMessage) == offsetof(WindowBase, mpMessage));
WWHD_OFFSET(hdMsgWindowBase_l, mpKeyIconPane, 0x80);
WWHD_OFFSET(hdMsgWindowBase_l, mStopOffsets, 0x98);
WWHD_OFFSET(hdMsgWindowBase_l, mStopIndex, 0xE8);
WWHD_OFFSET(hdMsgWindowBase_l, mSpeakerPos, 0xF8);
WWHD_OFFSET(hdMsgWindowBase_l, mTextColor, 0x112);
WWHD_OFFSET(hdMsgWindowBase_l, mpMessage, 0x118);
WWHD_SIZE(hdMsgWindowBase_l, 0x11C);

/* the key icon part of the keyboard screen (MSG-keyboard_text_area, merged), by address */
inline hdParts_c* hdKeyboardKeyIcon_construct_l(void* parts) { return gabi::call<hdParts_c*>(0x02674164, parts); }
inline u32 hdKeyboardKeyIcon_create_l(hdParts_c* icon, u32 arg1, u32 arg2, hdPartsDesc_c* desc) { return gabi::call<u32>(0x026741D0, icon, arg1, arg2, desc); }
enum { MSG_MAIN2_KEY_ICON_SIZE = 0x58 };

struct hdScreenMsgWindowMain2_c : hdMsgWindowBase_l {
    /* 0x11C */ gptr<hdMsgMain2Pane_l> mpPanes[5];          /* N_Position_00 / N_All_00 / N_Window_00 / N_Msg_00 / N_SubWindow_00 */
    /* 0x130 */ gptr<hdMsgMain2TextBox_l> mpTextPanes[3];   /* T_Msg_00 / T_Choise2_00 / T_Choise3_00 */
    /* 0x13C */ gptr<hdMsgMain2Pane_l> mpChoiceWindowPanes[2];
    /* 0x144 */ gptr<hdMsgMain2Pane_l> mpKeyIcon;           /* "L_KeyIcon_00" */
    /* 0x148 */ gptr<hdMsgMain2Pane_l> mpSelNumber;         /* "P_SelNumber_00" */

    static void destroy(hdScreenMsgWindowMain2_c* i_this, s32 flags);
    static hdScreenMsgWindowMain2_c* construct(hdScreenMsgWindowMain2_c* i_this);
    void findPanes();
    void findTextPanes();
    void findChoiceWindows();
    void findKeyIcon();
    void findSelNumber();
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void updatePlacement();
    void executeState();
    void setMessage(u32 arg, u32 messageId, cXyz* speakerPos);
    u8 open(u32 arg, u32 messageId, cXyz* speakerPos);
    u32 createChild(hdPartsDesc_c* desc);
    u32 loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void updateTail();
    void reset();
    void hideTextPanes();
    void hideChoiceWindows();
    void playInAll();
    u32 isInAllEnd();
    void playOutAll();
    u32 isOutAllEnd();
    void playLoopAll();
    void playInSubWindow();
    u32 isInSubWindowEnd();
    void playOutSubWindow();
    u32 isOutSubWindowEnd();
    void hideKeyIcon();
    void keyIconIn();
    void keyIconDecide();
    void keyIconOut();
    u32 isKeyIconWaiting();
    u32 isKeyIconHidden();
    u8 isKeyIconNotDeciding();

    hdPartsKeyIcon_c* keyIcon() { return (hdPartsKeyIcon_c*)mChildren.slot(0)->get(); }
    u32 vCreateChildren() { return gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this); }
};
WWHD_OFFSET(hdScreenMsgWindowMain2_c, mpPanes, 0x11C);
WWHD_OFFSET(hdScreenMsgWindowMain2_c, mpTextPanes, 0x130);
WWHD_OFFSET(hdScreenMsgWindowMain2_c, mpKeyIcon, 0x144);
WWHD_SIZE(hdScreenMsgWindowMain2_c, 0x14C);
void mDoLib_projectScreen(cXyz* src, cXyz* dst);

/* HD: the GamePad system window (hd_screen_system_window.cpp, companion of hd_screen_shortcut.cpp): the
 * "SequenceWindow_00" layout with the save / continue / warp / Miiverse / second-screen / hard-mode dialogs.
 * A dialog is a script (table at 0x101F6AE4) of 0x28-byte commands; the merged hd_screen_shortcut.cpp
 * holds its command handlers (it calls this class HdHudShortcut). States ::StateID_InAll ... _ResetEnd.
 * No GameCube counterpart. Layout, pane, text, sound and state names are HD strings; class, member and
 * method names are ours ("probably" marks a role read from its use only). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_sequence_window.h"
#include "gc_api/d/d_com_inf_game.h"

/* the window's class is hdSequenceWindow_c (d/hd_sequence_window.h); this file's code knew it by this name */
typedef hdSequenceWindow_c hdScreenSystemWindow_c;

/* one script entry is a text record (0x28 bytes, hdSequenceTextRecord_c: animation, button mode, labels...) */
typedef hdSequenceTextRecord_c hdSysWinCommand_c;
/* a dialog script: its commands and the index of the first one */
struct hdSysWinScript_c {
    /* 0x00 */ gptr<hdSequenceTextRecord_c> mpCommands;
    /* 0x04 */ u8 _04[7];
    /* 0x0B */ be<u8> mStart;
};
WWHD_OFFSET(hdSysWinScript_c, mStart, 0xB);
struct hdSysWinScripts_c {
    gptr<hdSysWinScript_c> mList[0x20];
};
enum hdSysWinMode_e {
    SYSWIN_MODE_SECOND_SCREEN = 2,
    SYSWIN_MODE_5 = 5,               /* probably a dialog the player has to answer */
    SYSWIN_MODE_8 = 8,
    SYSWIN_MODE_MESSAGE = 11,        /* a message by index (openMessage) */
    SYSWIN_MODE_MIIVERSE_INFO = 12,  /* the Miiverse info label (openMiiverseInfo) */
    SYSWIN_MODE_HARD_MODE = 0x11,    /* probably */
};

/* the message work area at the message manager (+0x14) is the window's HdMsgText (mpMessage) */
/* the message manager (pointer at 0x101F4B5C): this window registered at +0x968, the work area at +0x14 */
struct hdSysWinMsgMgr_c {
    /* 0x000 */ u8 _000[0x14];
    /* 0x014 */ HdMsgText mWork;
    /* ..... */ u8 _pad[0x968 - 0x14 - sizeof(HdMsgText)];
    /* 0x968 */ gptr<void> mpSystemWindow;
};
WWHD_OFFSET(hdSysWinMsgMgr_c, mWork, 0x14);
WWHD_OFFSET(hdSysWinMsgMgr_c, mpSystemWindow, 0x968);
/* the object the window watches each frame (pointer to it at 0x101F5088): its first word and +0x10C */
struct hdSysWinWatched_c {
    /* 0x000 */ be<u32> mFirst;
    /* 0x004 */ u8 _004[0x10C - 4];
    /* 0x10C */ be<u32> mStatus;
};
WWHD_OFFSET(hdSysWinWatched_c, mStatus, 0x10C);
struct hdSysWinWatchedHolder_c {
    gptr<hdSysWinWatched_c> mpObject;
};
/* HD option / menu state block at 0x1047B03C (two bytes used here) */
struct hdSysWinOptions_c {
    /* 0x00 */ u8 _00[0x40];
    /* 0x40 */ be<u8> mSaved;    /* probably */
    /* 0x41 */ u8 _41[0x5A - 0x41];
    /* 0x5A */ be<u8> mWindowOpen; /* probably */
};
WWHD_OFFSET(hdSysWinOptions_c, mSaved, 0x40);
WWHD_OFFSET(hdSysWinOptions_c, mWindowOpen, 0x5A);
/* the input controller singleton (0x101F8378): the cursor drives the menus, probably */
struct hdSysWinController_c {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ be<u8> mCursorActive;
};
WWHD_OFFSET(hdSysWinController_c, mCursorActive, 0x14);
/* the play object's three status bytes the window backs up (play+0x5C28 -> +0x5C2B) */
struct hdSysWinPlay_l {
    /* 0x0000 */ u8 _0000[0x5C28];
    /* 0x5C28 */ be<u8> mStatus[3];
    /* 0x5C2B */ be<u8> mStatusBackup[3];
};
WWHD_OFFSET(hdSysWinPlay_l, mStatus, 0x5C28);
WWHD_OFFSET(hdSysWinPlay_l, mStatusBackup, 0x5C2B);
/* panes: hdSequencePane_c (flags at +0x44, find-by-name in its vtable) */
/* a button part: its pane (+0x50) and a "selectable" word (+0x78), probably */
struct hdSysWinButton_c {
    /* 0x00 */ u8 _00[0x50];
    /* 0x50 */ gptr<void> mpPane;
    /* 0x54 */ u8 _54[0x78 - 0x54];
    /* 0x78 */ be<u32> mSelectable;
};
WWHD_OFFSET(hdSysWinButton_c, mpPane, 0x50);
WWHD_OFFSET(hdSysWinButton_c, mSelectable, 0x78);
/* the parts descriptor as this screen reads it: pane name at +0x14 */
struct hdSysWinDesc_l {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ hdUiSafeString mName;
};
WWHD_OFFSET(hdSysWinDesc_l, mName, 0x14);

/* callees: functions of this TU in other units, other parts, messages (by address; names ours) */
void hdSysWin_linkButton(hdParts_c* button, u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, u32 g, u32 h);
void hdSysWin_setMessageId(HdMsgText* msg, u32 id, u32 option);
void hdSysWin_setMessageLabel(HdMsgText* msg, hdUiSafeString* data, hdUiSafeString* label);
void hdSysWin_buildMessage(void* builder, HdMsgText* msg, void* text, void* text2);
void hdSysWin_setTextBoxMessage(void* textBox, u32 id);
void hdSysWin_deactivateLoop(hdParts_c* part);      /* hdPartsInOutLoop_c::deactivate */
void hdSysWin_setLoopSound(hdParts_c* part, hdUiSafeString* label); /* ::setLoopSound */
void hdSysWin_hideKeyIcon(hdParts_c* part);        /* hdPartsKeyIcon_c::hide */
void hdSysWin_bindGroup(hdAnimSet* set, u32 entry, hdUiSafeString* name, hdUiSafeString* group);

/* the play object (dComIfGp_get) and the menu flag (d_meter: dMenu_flag / dMenu_setFlag) */
hdSysWinPlay_l* hdSysWin_getPlay();
u8 hdSysWin_getMenuFlag();
void hdSysWin_setMenuFlag(u8 flag);
/* a SafeString that starts out empty (the runtime's shared empty string) */
inline void hdSysWin_setEmptySafeString(hdUiSafeString* string, hdUiStringTable* vtable, hdPartsChar_t* empty) {
    string->mVtable = vtable;
    string->mText = empty;
}
/* the command `index` commands further on (32-bit guest address arithmetic) */
inline hdSysWinCommand_c* hdSysWin_commandAt(hdSysWinCommand_c* commands, u32 index) {
    return hdPartsView<hdSysWinCommand_c>(gabi::ea(commands) + index * sizeof(hdSysWinCommand_c));
}
enum { SYSWIN_TEXT_MIIVERSE_INFO = 35 };

/* animation entries ("InAll", "FormDefault", "FormDRC", "FormProCon", "FormFrameCenter", "OnOffBG", "OutAll") */
enum hdSysWinAnim_e {
    SYSWIN_ANIM_IN_ALL = 0,
    SYSWIN_ANIM_ON_OFF_BG = 5,
};
/* child parts (the parts panes "L_BtnSeqenceL/R/C_00", "L_LoadingIcon_00", "L_KeyIcon_00") */
enum hdSysWinChild_e {
    SYSWIN_CHILD_BUTTON_L = 0,
    SYSWIN_CHILD_BUTTON_R = 1,
    SYSWIN_CHILD_BUTTON_C = 2,
    SYSWIN_CHILD_LOADING = 3,
    SYSWIN_CHILD_KEY_ICON = 4,
};
/* message ids */
enum {
    SYSWIN_MSG_FIRST = 0x31A6,
    SYSWIN_MSG_WARP_HERE = 0x31C6,          /* probably */
    SYSWIN_MSG_WARP_HERE_LATER = 0x31D7,    /* after event 0x2D80 */
    SYSWIN_MSG_INDEX_BASE = 0x32E3,
    SYSWIN_MSG_INDEX_COUNT = 0x31,
};


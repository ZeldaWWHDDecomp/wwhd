/* HD: the GamePad's item shortcut "Shortcut_00" (hd_screen_item_shortcut.cpp): shows the item behind the
 * shortcut button (Wind Waker; on the boat the salvage arm or the cannon with its bomb count) and the
 * unread-Miiverse marker. States ::StateID_Wait / _In / _Disp / _Out / _Change. No GameCube counterpart.
 * Layout, pane, group, animation and state names are HD strings; class, member and method names are ours
 * ("probably" marks a role read from its use only).
 *
 * The class derives from the "NoControl" parts class of FW-parts_msg_base (NoControlPart_l, 0x50). */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_no_control.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/d_item_data.h"

/* the unread-Miiverse marker part (0x58; another unit), by address */
hdParts_c* hdPartsUnreadMiiverse_construct(hdParts_c* parts);
u32 hdPartsUnreadMiiverse_init(hdParts_c* parts, u32 arg1, u32 arg2, hdPartsDesc_c* desc);
void hdPartsUnreadMiiverse_reset(hdParts_c* parts);
/* 02004E04: bind animation `entry` to its pane group (hd_ui_02004A3C) */
void hdItemShortcut_bindGroup(hdAnimSet* set, u32 entry, hdUiSafeString* name, hdUiSafeString* group);
/* FW-parts_text_valid: a text box shows a wide string; the runtime's wide-string format */
void hdItemShortcut_setText(void* textBox, void* string, u32 option);
void hdItemShortcut_format(void* string, const void* format, u32 value);

/* what the shortcut button shows (mKind) */
enum hdItemShortcutKind_e {
    ITEM_SHORTCUT_NONE = 0,
    ITEM_SHORTCUT_WIND_WAKER = 1,
    ITEM_SHORTCUT_SALVAGE = 2, /* the grappling hook, on the boat */
    ITEM_SHORTCUT_CANNON = 3,  /* bombs, on the boat */
    ITEM_SHORTCUT_UNSET = 4,
};
/* inventory slots it checks */
enum hdItemShortcutSlot_e {
    ITEM_SHORTCUT_SLOT_WIND_WAKER = 2,
    ITEM_SHORTCUT_SLOT_GRAPPLING_HOOK = 3,
    ITEM_SHORTCUT_SLOT_BOMBS = 13,
};
/* animation entries ("InAll", "InSalvageCannon" x2, "OutSalvageCannon" x2, "State", "MiiversePos", "Num",
 * "OutAll"); NONE: no animation */
enum hdItemShortcutAnim_e {
    ITEM_SHORTCUT_ANIM_IN_ALL = 0,
    ITEM_SHORTCUT_ANIM_IN_SALVAGE = 1,
    ITEM_SHORTCUT_ANIM_IN_CANNON = 2,
    ITEM_SHORTCUT_ANIM_OUT_SALVAGE = 3,
    ITEM_SHORTCUT_ANIM_OUT_CANNON = 4,
    ITEM_SHORTCUT_ANIM_STATE = 5,
    ITEM_SHORTCUT_ANIM_MIIVERSE_POS = 6,
    ITEM_SHORTCUT_ANIM_NUM = 7,
    ITEM_SHORTCUT_ANIM_OUT_ALL = 8,
    ITEM_SHORTCUT_ANIM_NONE = 9,
};

/* the parts descriptor as this screen reads it: type string at +0x08, pane name at +0x14 */
struct hdItemShortcutDesc_l {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ hdUiSafeString mType;
    /* 0x10 */ u8 _10[4];
    /* 0x14 */ hdUiSafeString mName;
};
WWHD_OFFSET(hdItemShortcutDesc_l, mType, 8);
WWHD_OFFSET(hdItemShortcutDesc_l, mName, 0x14);

/* a wide FixedSafeString<3> on the stack (the bomb count text) */
struct hdItemShortcutNumText_c {
    /* 0x00 */ gptr<be<u16>> mText;
    /* 0x04 */ gptr<void> mVtable;
    /* 0x08 */ be<s32> mCapacity;
    /* 0x0C */ be<u16> mBuffer[3];
    /* 0x12 */ u8 _12[2];
};
WWHD_OFFSET(hdItemShortcutNumText_c, mBuffer, 0xC);
WWHD_SIZE(hdItemShortcutNumText_c, 0x14);

/* a pane found by name (text box "T_Num_00") */
struct hdItemShortcutPaneVtbl_c {
    /* 0x00 */ u8 _00[0x5C];
    /* 0x5C */ gfn mFindPaneByName;
};
struct hdItemShortcutPane_c {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ gptr<hdItemShortcutPaneVtbl_c> mVtable;
};

struct hdScreenItemShortcut_c : NoControlPart_l {
    /* 0x50 */ gptr<void> mpNumPane;    /* "T_Num_00" */
    /* 0x54 */ be<u32> mKind;           /* hdItemShortcutKind_e */
    /* 0x58 */ be<u32> mPrevKind;
    /* 0x5C */ be<u32> mChangeAnimIn;   /* hdItemShortcutAnim_e */
    /* 0x60 */ be<u32> mChangeAnimOut;
    /* 0x64 */ be<u32> mMiiverseFrame;  /* 0: unread posts, 1: none, 2: not set */
    /* 0x68 */ be<u8> mBombNum;         /* the count shown (0xFF: none yet) */
    /* 0x69 */ u8 _69[3];

    static void destroy(hdScreenItemShortcut_c* i_this, s32 flags);
    void setAnimFrame(u32 anim, u32 frame);
    void waitEnter();
    void updateKind();
    void waitExecute();
    void startAnim(u32 anim);
    void updateMiiverse();
    void inEnter();
    void inExecute();
    void dispExecute();
    void outEnter();
    void outExecute();
    void changeEnter();
    void changeExecute();
    static hdScreenItemShortcut_c* construct(hdScreenItemShortcut_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
    void updateBombNum();
    void executeState();
    void reset();
    u32 visit(hdItemShortcutDesc_l* desc);
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
};
WWHD_OFFSET(hdScreenItemShortcut_c, mpNumPane, 0x50);
WWHD_OFFSET(hdScreenItemShortcut_c, mKind, 0x54);
WWHD_OFFSET(hdScreenItemShortcut_c, mChangeAnimIn, 0x5C);
WWHD_OFFSET(hdScreenItemShortcut_c, mMiiverseFrame, 0x64);
WWHD_OFFSET(hdScreenItemShortcut_c, mBombNum, 0x68);
WWHD_SIZE(hdScreenItemShortcut_c, 0x6C);

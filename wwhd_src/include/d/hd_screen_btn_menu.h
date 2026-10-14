/* HD: the pause menu's buttons (layout "BtnMenu_00", hd_screen_btn_menu.cpp): BasicButton parts with a
 * "new" mark that counts unread Miiverse comments; the arrow buttons ("BtnMenuArrow_00",
 * hd_screen_btn_menu_arrow.cpp) and the heart-piece button ("BtnMenuHeart_00", hd_screen_btn_menu_heart.cpp).
 * No GameCube counterpart. Layout, pane, animation, sound and state names are HD strings; class, member
 * and method names are ours ("probably" marks a role read from its use only). Local game-derived source;
 * */
#pragma once
#include "d/hd_parts_basic_button.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_screen_cursor.h"

/* the BasicButton and button-part helpers of FW-parts_buttons that its sources keep file-local, by address */
hdBasicButton_c* hdBasicButton_construct_l(hdBasicButton_c* button);   /* 026F1C3C */
BOOL hdButton_inputEnabled_l();                                        /* 026F67A4 */
BOOL hdButton_canDispatch_l(u32 event);                                /* 026F621C */
void hdButton_markTouchRelease_l();                                    /* 026F69A4 */
hdButtonEvent_l* hdButtonEvent_copy_l(hdButtonEvent_l* dst, hdButtonEvent_l* src);   /* 020013C0 */
/* 02615140: the receiver that holds the input (hd_input_eventmgr) */
u32 hdButtonEventManager_getActiveReceiver_l(hdButtonEventManager_l* manager);
/* 026150C0: hdUiReceiverList_pushBack (hd_input_eventmgr) */
void hdButtonEventManager_pushReceiver_l(hdButtonEventManager_l* manager, hdButtonReceiver_l* receiver);

/* the BasicButton virtual functions this code calls (our view of the vtable) */
struct hdBtnMenuVtbl_l {
    /* 0x000 */ u8 _000[0xDC];
    /* 0x0DC */ gfn mDispatch;
    /* 0x0E0 */ u8 _0E0[4];
    /* 0x0E4 */ gfn mIsDecide;
    /* 0x0E8 */ u8 _0E8[4];
    /* 0x0EC */ gfn mIsCancel;
    /* 0x0F0 */ u8 _0F0[4];
    /* 0x0F4 */ gfn mIsTouch;
    /* 0x0F8 */ u8 _0F8[0x14];
    /* 0x10C */ gfn mPlayTouchSound;
    /* 0x110 */ u8 _110[0x12C];
    /* 0x23C */ gfn mCancelCursor;
    /* 0x240 */ u8 _240[0x14];
    /* 0x254 */ gfn mPlaySound;
};
/* the Miiverse comment manager (hd_olv_02038E24) */
u32 hdBtnMenu_notifyCount_l(void* comments);   /* 020398B8 */
u32 hdBtnMenu_hasNotify_l(void* comments);     /* 02039970 */
/* 02005708: hd_ui_02004A3C's animationOperationD4 */
void hdBtnMenu_animOperationD4(hdAnimSet* set, u32 entry, u32 slot);
/* 02030B38: play a named sound effect */
void hdBtnMenu_playSound(u32 name);
inline void hdBtnMenu_playSoundName(const char* name) { hdBtnMenu_playSound(gabi::guest_addr(name)); }
/* the UI controller's input mode (+0x224; 2: probably touch only) */
struct hdBtnMenuController_l {
    /* 0x000 */ u8 _000[0x224];
    /* 0x224 */ be<u32> mMode;
};

/* button states, as GHS numbers them */
enum hdBtnMenuSelect_e {
    BTN_MENU_SELECT_NONE = 0,
    BTN_MENU_SELECT_CURSOR = 1,
    BTN_MENU_SELECT_TOUCH = 2,
};
enum hdBtnMenuAnim_e {
    BTN_MENU_ANIM_DECIDE = 2,
    BTN_MENU_ANIM_NEW_MARK = 7,
    BTN_MENU_ANIM_NEW_MARK_IN = 8,
    BTN_MENU_ANIM_NEW_MARK_OUT = 9,
};
enum hdBtnMenuNewMark_e {
    BTN_MENU_NEW_MARK_OFF = 0,
    BTN_MENU_NEW_MARK_IN = 1,
    BTN_MENU_NEW_MARK_SHOWN = 2,
    BTN_MENU_NEW_MARK_OUT = 3,
};

struct hdScreenBtnMenu_c : hdBasicButton_c {
    /* 0x8C */ be<u32> mKind;          /* 2: the Miiverse button, which shows the new mark */
    /* 0x90 */ be<u32> mNewMarkPhase;
    /* 0x94 */ gptr<hdCursorPane_c> mpTextPanes[2]; /* "T_Btn_00", "T_NewNum_00" */

    static void destroy(hdScreenBtnMenu_c* i_this, s32 flags);
    void decideEnter();
    void decideMove();
    void cancelEnter();
    void cancelMove();
    static hdScreenBtnMenu_c* construct(hdScreenBtnMenu_c* i_this);
    void findTextPanes();
    BOOL load(void* archive, void* heap, void* par);
    void updateNewCount();
    void update();
    void registerReceiver();
    void decide();
    static BOOL isTouchTap(hdButtonEvent_l* event);
    BOOL loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void playSound();

    hdButtonPart_c* part() { return &mButton; }
    hdBtnMenuVtbl_l* vtbl() { return (hdBtnMenuVtbl_l*)(void*)mButton.mVtable.get(); }
};
WWHD_OFFSET(hdScreenBtnMenu_c, mKind, 0x8C);
WWHD_OFFSET(hdScreenBtnMenu_c, mpTextPanes, 0x94);
WWHD_SIZE(hdScreenBtnMenu_c, 0x9C);

/* the input manager's held buttons (+0x124; our view of hdInputManager, hd_input_ctrl) */
#include "d/hd_input_ctrl.h"
struct hdBtnMenuInput_l {
    /* 0x000 */ u8 _000[0x124];
    /* 0x124 */ be<u32> mHeld;
};
enum {
    BTN_MENU_REPEAT_BUTTONS = 0x8001, /* the buttons whose repeat timing the arrows change */
    BTN_MENU_HELD_DECIDE = 0x1,       /* probably the A button */
};

/* the pause menu's page arrows "BtnMenuArrow_00": while held (or while their direction key is held) the
 * input repeat runs at the arrow's own rate */
struct hdScreenBtnMenuArrow_c : hdBasicButton_c {
    /* 0x8C */ be<u32> mDirection;      /* 0..3: the direction key (4: none) */
    /* 0x90 */ be<u8> mDirectionKeys;   /* the direction keys also press the arrow */
    /* 0x91 */ be<u8> mRepeatDelay;     /* 30 */
    /* 0x92 */ be<u8> mRepeatRate;      /* 1 */
    /* 0x93 */ be<u8> mRepeating;

    static void destroy(hdScreenBtnMenuArrow_c* i_this, s32 flags);
    void selectTouchEnter();
    void selectTouchMove();
    void selectCursorEnter();
    void selectCursorMove();
    void decideEnter();
    void decideMove();
    static hdScreenBtnMenuArrow_c* construct(hdScreenBtnMenuArrow_c* i_this);
    BOOL load(void* archive, void* heap, void* par);
    void restoreRepeat();
    void unload();
    u32 getDirectionMask(u32 direction);
    BOOL isHeld();
    void setRepeat();
    void updateRepeat();
    void update();
    BOOL isDecide(u32 event);
    BOOL isCancel(u32 event);
    u32 getDirectionButton(u32 direction);
    BOOL isTouch(hdButtonEvent_l* event);

    hdButtonPart_c* part() { return &mButton; }
    hdBtnMenuVtbl_l* vtbl() { return (hdBtnMenuVtbl_l*)(void*)mButton.mVtable.get(); }
};
WWHD_OFFSET(hdScreenBtnMenuArrow_c, mDirection, 0x8C);
WWHD_OFFSET(hdScreenBtnMenuArrow_c, mRepeating, 0x93);
WWHD_SIZE(hdScreenBtnMenuArrow_c, 0x94);
/* 026F61EC: hd_parts_button's static hdButton_isMenu */
BOOL hdButton_isMenu_l(u32 event);   /* 026F61EC */

/* the heart-piece button "BtnMenuHeart_00": shows how many heart pieces of the next heart are collected.
 * Its base is the "NoDecideButton" parts class (NoDecideButton_l, FW-parts_msg_base), itself a button part */
#include "d/d_com_inf_game.h"
#include "d/hd_no_decide_button.h"
enum {
    BTN_MENU_HEART_ANIM_HEART_PIECE = 5, /* its frame: the heart pieces (0..3) */
};
struct hdScreenBtnMenuHeart_c : hdButtonPart_c {
    /* 0x74 */ u8 _74[4];             /* the "NoDecideButton" base's member */
    /* 0x78 */ be<u32> mHeartPieces;   /* shown (4: not yet) */

    /* the "NoDecideButton" base (the merged header models it on hdParts_c) */
    NoDecideButton_l* noDecideButton() { return (NoDecideButton_l*)(hdParts_c*)this; }

    static void destroy(hdScreenBtnMenuHeart_c* i_this, s32 flags);
    void heartPieceEnter();
    void heartPieceExecute();
    static hdScreenBtnMenuHeart_c* construct(hdScreenBtnMenuHeart_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
    void updateHeartPieces();
    BOOL loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
};
WWHD_OFFSET(hdScreenBtnMenuHeart_c, mHeartPieces, 0x78);
WWHD_SIZE(hdScreenBtnMenuHeart_c, 0x7C);

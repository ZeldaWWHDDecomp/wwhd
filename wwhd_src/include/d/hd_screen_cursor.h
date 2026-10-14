/* HD: the menu cursor screens (layouts "Cursor_00" / "Cursor_01" / "Cursor_02", window pane
 * "W_Cursor_00"): an hdParts_c screen that follows the focused UI element (Cursor_00, hd_screen_cursor.cpp).
 * No GameCube counterpart. Layout, pane, animation and state names are HD strings; class, member and
 * method names are ours ("probably" marks a role read from its use only). Local game-derived source;
 * */
#pragma once
#include "d/hd_parts.h"

typedef be<u8> hdCursorChar_t; /* a guest string character */

/* the layout panes the cursor moves and resizes (nw::lyt::Pane fields used here) */
struct hdCursorPaneVtbl_c {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ gfn mGetRuntimeTypeInfo;
    /* 0x18 */ u8 _18[0x44];
    /* 0x5C */ gfn mFindPaneByName;
};
WWHD_OFFSET(hdCursorPaneVtbl_c, mGetRuntimeTypeInfo, 0x14);
WWHD_OFFSET(hdCursorPaneVtbl_c, mFindPaneByName, 0x5C);
enum hdCursorPaneFlag_e {
    CURSOR_PANE_VISIBLE = 0x01,
    CURSOR_PANE_DIRTY = 0x10, /* probably: recompute the matrix */
};
struct hdCursorPane_c {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ gptr<hdCursorPaneVtbl_c> mVtable;
    /* 0x0C */ u8 _0C[0x10];
    /* 0x1C */ cXyz mTranslate;
    /* 0x28 */ cXyz mRotate;
    /* 0x34 */ u8 _34[8];
    /* 0x3C */ be<f32> mWidth;
    /* 0x40 */ be<f32> mHeight;
    /* 0x44 */ be<u8> mFlags;
    /* 0x45 */ u8 _45[2];
    /* 0x47 */ be<u8> mBasePosition; /* bits 0-1 horizontal, 2-3 vertical: 1 = left/top, 2 = right/bottom */

    hdPartsTypeInfo_c* getRuntimeTypeInfo() { return gabi::call_ptr<hdPartsTypeInfo_c*>(mVtable->mGetRuntimeTypeInfo.get(), this); }
};
WWHD_OFFSET(hdCursorPane_c, mVtable, 8);
WWHD_OFFSET(hdCursorPane_c, mTranslate, 0x1C);
WWHD_OFFSET(hdCursorPane_c, mRotate, 0x28);
WWHD_OFFSET(hdCursorPane_c, mWidth, 0x3C);
WWHD_OFFSET(hdCursorPane_c, mHeight, 0x40);
WWHD_OFFSET(hdCursorPane_c, mFlags, 0x44);
WWHD_OFFSET(hdCursorPane_c, mBasePosition, 0x47);
WWHD_SIZE(hdCursorPane_c, 0x48);

/* the focused UI element the event manager reports: its pane, and a pane the cursor uses instead */
struct hdCursorTarget_c {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ gptr<void> mpEventMgr;
    /* 0x10 */ u8 _10[8];
    /* 0x18 */ gptr<hdCursorPane_c> mpPane;
    /* 0x1C */ u8 _1C[0x40];
    /* 0x5C */ gptr<hdCursorPane_c> mpCursorPane;
};
WWHD_OFFSET(hdCursorTarget_c, mpEventMgr, 0xC);
WWHD_OFFSET(hdCursorTarget_c, mpPane, 0x18);
WWHD_OFFSET(hdCursorTarget_c, mpCursorPane, 0x5C);

/* the input controller singleton: whether the cursor (not touch) drives the menus, probably */
struct hdCursorController_c {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ be<u8> mCursorActive;
};
WWHD_OFFSET(hdCursorController_c, mCursorActive, 0x14);

struct hdCursorSize_c {
    /* 0x00 */ be<f32> x;
    /* 0x04 */ be<f32> y;
};
WWHD_SIZE(hdCursorSize_c, 8);

/* 02615140: the element the UI event manager has focused (NULL: none) */
hdCursorTarget_c* hdCursor_getFocus(void* eventMgr);
/* 02704D2C: a pane's position in layout space */
void hdCursor_getPanePos(cXyz* out, hdCursorPane_c* pane);
/* 02004E04: bind animation `entry` to its pane group (hd_ui_02004A3C) */
inline void hdAnimSet_bindGroup(hdAnimSet* set, u32 entry, hdUiSafeString* name, hdUiSafeString* group) {
    gabi::call(0x02004E04, set, entry, name, group);
}
/* 028F0164: the runtime's array destructor (__destroy_arr) for a static's exit-time destruction */
inline void hdCursor_destroyArray(void* array, u32 count, u32 size, u32 destructor) { gabi::call(0x028F0164, array, count, size, destructor, 0u, 0u); }

/* HD: a float copied without arithmetic (lfs/stfs) keeps a signalling NaN bit for bit */
inline void hdCursor_copyFloat(be<f32>* dst, be<f32>* src) { gabi::store<u32>(gabi::ea(dst), gabi::load<u32>(gabi::ea(src))); }

/* animation entries of the cursor layouts ("InAll", "LoopAll", "OutAll") */
enum hdCursorAnim_e {
    CURSOR_ANIM_IN_ALL = 0,
    CURSOR_ANIM_LOOP_ALL = 1,
    CURSOR_ANIM_OUT_ALL = 2,
};

struct hdScreenCursor_c : hdParts_c {
    /* 0x50 */ gptr<hdCursorPane_c> mpWindowPane; /* "W_Cursor_00" */

    static void destroy(hdScreenCursor_c* i_this, s32 flags);
    void blankExecute();
    void inAllEnter();
    void inAllExecute();
    void loopAllEnter();
    void loopAllExecute();
    void outAllEnter();
    void outAllExecute();
    static hdScreenCursor_c* construct(hdScreenCursor_c* i_this);
    void findWindowPane();
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void executeState();
    void draw(void* info);
    static void setFrameScale(hdCursorSize_c* scale);
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    static void getAnchorPos(cXyz* out, hdScreenCursor_c* i_this, hdCursorPane_c* pane);
    void moveTo(hdCursorTarget_c* target);
    void calcLayout();
};
WWHD_OFFSET(hdScreenCursor_c, mpWindowPane, 0x50);
WWHD_SIZE(hdScreenCursor_c, 0x54);

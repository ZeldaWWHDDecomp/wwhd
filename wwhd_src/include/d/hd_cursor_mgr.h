/* HD: the menu cursor manager (hd_cursor_mgr.cpp): a heap-disposed singleton that owns the focus cursor
 * ("Cursor_00", hdScreenCursor_c) and the fixed cursor ("Cursor_02", hdScreenFixedCursor_c), runs them and
 * draws the active one. No GameCube counterpart. All names are ours. */
#pragma once
#include "d/hd_screen_fixed_cursor.h"

/* the cursor screens it owns */
enum hdCursorMgrCursor_e {
    CURSOR_MGR_FOCUS = 0, /* hdScreenCursor_c */
    CURSOR_MGR_FIXED = 1, /* hdScreenFixedCursor_c */
};

/* a sead heap disposer at the start of the singleton (0x10, vtable at +0xC) */
struct hdCursorMgrDisposerVtbl_c {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ gfn mDelete;
};
struct hdCursorMgrDisposer_c {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ gptr<hdCursorMgrDisposerVtbl_c> mVtable;
    void destroy(s32 flags) { gabi::call_ptr(mVtable->mDelete.get(), this, flags); }
};
WWHD_OFFSET(hdCursorMgrDisposer_c, mVtable, 0xC);
WWHD_SIZE(hdCursorMgrDisposer_c, 0x10);

struct hdCursorMgrVtbl_c {
    /* 0x00 */ u8 _00[0xC];
    /* 0x0C */ gfn mDelete;
};
struct hdCursorMgr_c : hdCursorMgrDisposer_c {
    /* 0x10 */ gptr<hdParts_c> mpCursors[2];
    /* 0x18 */ be<s32> mActive;   /* the cursor drawn; the fixed one also runs while it is active */
    /* 0x1C */ gptr<hdCursorMgrVtbl_c> mMgrVtable;

    static hdCursorMgr_c* construct(hdCursorMgr_c* i_this);
    static void createInstance(void* heap);
    static void deleteInstance();
    u32 create(u32 arg1, u32 arg2);
    void destroyCursors();
    void execute();
    void draw(void* info);
    void setFrameScale(hdCursorSize_c* scale);
    void setFixedPosition(hdCursorSize_c* position);
    void setFixedSize(hdCursorSize_c* size);
    static void destroyDisposer(hdCursorMgr_c* i_this, s32 flags);

    void destroy(s32 flags) { gabi::call_ptr(mMgrVtable->mDelete.get(), this, flags); }
    /* HD: SafeArray access: an index out of range uses element 0 */
    gptr<hdParts_c>* cursorSlot(u32 index) { return &mpCursors[index < 2 ? index : 0]; }
};
WWHD_OFFSET(hdCursorMgr_c, mpCursors, 0x10);
WWHD_OFFSET(hdCursorMgr_c, mActive, 0x18);
WWHD_OFFSET(hdCursorMgr_c, mMgrVtable, 0x1C);
WWHD_SIZE(hdCursorMgr_c, 0x20);

/* sead heap allocation and the disposer base (library, by address) */
void* hdCursorMgr_new(u32 size, void* heap, u32 alignment);
void hdCursorMgr_constructDisposer(hdCursorMgrDisposer_c* disposer, void* heap, u32 option);
void hdCursorMgr_destroyDisposerBase(hdCursorMgrDisposer_c* disposer, s32 flags);

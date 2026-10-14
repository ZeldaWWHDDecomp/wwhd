/* HD: the "Cursor_02" screen (hd_screen_fixed_cursor.cpp): a cursor frame ("W_Cursor_00") placed at a
 * position and size set from outside (statics), shown while an element has focus. States
 * ::StateID_Blank / _InAll / _LoopAll / _OutAll. No GameCube counterpart. Layout, pane, animation and
 * state names are HD strings; class, member and method names are ours. */
#pragma once
#include "d/hd_screen_cursor.h"

struct hdScreenFixedCursor_c : hdParts_c {
    /* 0x50 */ gptr<hdCursorPane_c> mpWindowPane; /* "W_Cursor_00" */

    static void destroy(hdScreenFixedCursor_c* i_this, s32 flags);
    void blankExecute();
    void inAllEnter();
    void inAllExecute();
    void loopAllEnter();
    void loopAllExecute();
    void outAllEnter();
    void outAllExecute();
    static hdScreenFixedCursor_c* construct(hdScreenFixedCursor_c* i_this);
    void findWindowPane();
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void executeState();
    void draw(void* info);
    static void setPosition(hdCursorSize_c* position);
    static void setSize(hdCursorSize_c* size);
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
    void calcLayout();
};
WWHD_OFFSET(hdScreenFixedCursor_c, mpWindowPane, 0x50);
WWHD_SIZE(hdScreenFixedCursor_c, 0x54);

/* HD: the "Cursor_01" screen (hd_screen_msg_cursor.cpp): a cursor frame for message choices. It registers
 * with the message manager, measures the choice lines of a message with the font to size its window
 * ("W_Cursor_00"), and moves its root group ("N_All_00") to the chosen line. States ::StateID_InAll /
 * _LoopAll / _OutAll, idle in hdParts_c's ::StateID_Blank. No GameCube counterpart. Layout, pane,
 * animation and state names are HD strings; class, member and method names are ours ("probably" marks a
 * role read from its use only). */
#pragma once
#include "d/hd_screen_cursor.h"

/* the message manager (pointer at 0x101F4B5C): the choice cursor it draws with, probably */
struct hdMsgCursorMsgMgr_c {
    /* 0x000 */ u8 _000[0x964];
    /* 0x964 */ gptr<void> mpChoiceCursor;
};
WWHD_OFFSET(hdMsgCursorMsgMgr_c, mpChoiceCursor, 0x964);

/* a font: its vtable at +4; slot +0x7C is the width of one character */
struct hdMsgCursorFontVtbl_c {
    /* 0x00 */ u8 _00[0x7C];
    /* 0x7C */ gfn mGetCharWidth;
};
WWHD_OFFSET(hdMsgCursorFontVtbl_c, mGetCharWidth, 0x7C);
struct hdMsgCursorFont_c {
    /* 0x00 */ u8 _00[4];
    /* 0x04 */ gptr<hdMsgCursorFontVtbl_c> mVtable;
    s32 getCharWidth(u32 c) { return gabi::call_ptr<s32>(mVtable->mGetCharWidth.get(), this, c); }
};
WWHD_OFFSET(hdMsgCursorFont_c, mVtable, 4);

/* message text: UTF-16 with control sequences (0x0E: tag, length in the u16 at +6; 0x0F: 6-byte code) */
typedef be<u16> hdMsgCursorChar_t;
enum hdMsgCursorCode_e {
    MSG_CURSOR_CODE_NEWLINE = 0x0A,
    MSG_CURSOR_CODE_TAG = 0x0E,
    MSG_CURSOR_CODE_SHORT_TAG = 0x0F,
};
/* the character `bytes` further on (32-bit guest address arithmetic) */
inline hdMsgCursorChar_t* hdMsgCursor_advance(hdMsgCursorChar_t* text, u32 bytes) { return hdPartsView<hdMsgCursorChar_t>(gabi::ea(text) + bytes); }

struct hdMsgCursorTag_c {
    /* 0x00 */ hdMsgCursorChar_t mCode;
    /* 0x02 */ u8 _02[4];
    /* 0x06 */ be<u16> mParamSize;
};
WWHD_OFFSET(hdMsgCursorTag_c, mParamSize, 6);

/* the text cursors of up to three choice lines (setLines2's stack object) */
struct hdMsgCursorLines_c {
    gptr<hdMsgCursorChar_t> mText[3];
};
WWHD_SIZE(hdMsgCursorLines_c, 0xC);

struct hdScreenMsgCursor_c : hdParts_c {
    /* 0x50 */ gptr<hdCursorPane_c> mpAllPane;    /* "N_All_00" */
    /* 0x54 */ gptr<hdCursorPane_c> mpWindowPane; /* "W_Cursor_00" */
    /* 0x58 */ cXyz mLinePositions[3];
    /* 0x7C */ be<f32> mBaseWidth;                /* the window's width in the layout */
    /* 0x80 */ be<u8> mDrawFront;                 /* drawn by draw() (else by drawBack()) */
    /* 0x81 */ u8 _81[3];

    static void destroy(hdScreenMsgCursor_c* i_this, s32 flags);
    void startAnim(u32 anim, f32 rate);
    void inAllEnter();
    u32 isAnimEnd(u32 anim);
    void inAllExecute();
    void loopAllEnter();
    void outAllEnter();
    void hide();
    void outAllExecute();
    static hdScreenMsgCursor_c* construct(hdScreenMsgCursor_c* i_this);
    void findAllPane();
    void findWindowPane();
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void executeState();
    void draw(void* info);
    void drawBack(void* info);
    s32 measureLine(gptr<hdMsgCursorChar_t>* text, f32 scale, hdMsgCursorFont_c* font);
    void setWidth(s32 textWidth);
    void setLines(cXyz* positions, hdUiSafeString* message, hdMsgCursorFont_c* font, f32 scale);
    void setLines2(cXyz* positions, hdUiSafeString* first, hdUiSafeString* second, hdMsgCursorFont_c* font, f32 scale);
    u32 select(BOOL open, u32 line, hdCursorPane_c* rotationSource, u8 drawFront);
    u32 isLooping();
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);
};
WWHD_OFFSET(hdScreenMsgCursor_c, mpAllPane, 0x50);
WWHD_OFFSET(hdScreenMsgCursor_c, mpWindowPane, 0x54);
WWHD_OFFSET(hdScreenMsgCursor_c, mLinePositions, 0x58);
WWHD_OFFSET(hdScreenMsgCursor_c, mBaseWidth, 0x7C);
WWHD_OFFSET(hdScreenMsgCursor_c, mDrawFront, 0x80);
WWHD_SIZE(hdScreenMsgCursor_c, 0x84);

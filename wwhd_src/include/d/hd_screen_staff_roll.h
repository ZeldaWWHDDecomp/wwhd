/* HD: the staff roll "StaffRoll_00" (hd_screen_staff_roll.cpp): fourteen text lines scroll up through the
 * screen, fading in and out near the edges, and are refilled from the 187 staff roll labels
 * "T_StaffRoll_000"... as they leave the top; then the "T_AllRights_00" line scrolls up alone. States
 * ::StateID_All / _CopyRight. No GameCube counterpart. Layout, pane, label and state names are HD strings;
 * class, member and method names are ours ("probably" marks a role read from its use only). Local
 * */
#pragma once
#include "d/hd_parts.h"
#include "d/hd_parts_text_pane.h"

/* a text pane of the layout (nw::lyt::TextBox, game text pane class; FW-parts_text_valid) */
struct hdStaffRollLine_c {
    hdTextPane_c* textPane() { return (hdTextPane_c*)(void*)this; }
    /* 0x000 */ u8 _000[8];
    /* 0x008 */ gptr<void> mVtable;
    /* 0x00C */ u8 _00C[0x10];
    /* 0x01C */ be<f32> mX;
    /* 0x020 */ be<f32> mY;
    /* 0x024 */ be<f32> mZ;
    /* 0x028 */ u8 _028[0x18];
    /* 0x040 */ be<f32> mHeight;
    /* 0x044 */ be<u8> mFlags;
    /* 0x045 */ be<u8> mAlpha;
    /* 0x046 */ u8 _046[0x23C - 0x46];
    /* 0x23C */ u8 mMessage[0x923];    /* the pane's message work area */
    /* 0xB5F */ be<u8> mIsHeading;     /* probably: the line is a job title */
};
WWHD_OFFSET(hdStaffRollLine_c, mX, 0x1C);
WWHD_OFFSET(hdStaffRollLine_c, mY, 0x20);
WWHD_OFFSET(hdStaffRollLine_c, mHeight, 0x40);
WWHD_OFFSET(hdStaffRollLine_c, mFlags, 0x44);
WWHD_OFFSET(hdStaffRollLine_c, mAlpha, 0x45);
WWHD_OFFSET(hdStaffRollLine_c, mMessage, 0x23C);
WWHD_OFFSET(hdStaffRollLine_c, mIsHeading, 0xB5F);
enum hdStaffRollLineFlag_e {
    STAFF_ROLL_LINE_VISIBLE = 0x01,
    STAFF_ROLL_LINE_DIRTY = 0x10,
};
/* a text pane's two text colours (top, bottom) */
struct hdStaffRollColors_c {
    be<u32> mTop;
    be<u32> mBottom;
};
/* the colours a line keeps (job title / name) */
struct hdStaffRollColorPair_c {
    /* 0x0 */ be<u32> mTop;
    /* 0x4 */ be<u32> mBottom;
};
/* a message work area on the stack (0x924; constructor 025F65F4): +8 is non-NULL when the label exists */
struct hdStaffRollMsg_c {
    /* 0x000 */ u8 _000[8];
    /* 0x008 */ gptr<void> mpEntry;
    /* 0x00C */ u8 mText[0x120 - 0xC];
    /* 0x120 */ u8 mText2[0x924 - 0x120];
};
WWHD_OFFSET(hdStaffRollMsg_c, mpEntry, 8);
WWHD_OFFSET(hdStaffRollMsg_c, mText, 0xC);
WWHD_OFFSET(hdStaffRollMsg_c, mText2, 0x120);
WWHD_SIZE(hdStaffRollMsg_c, 0x924);
/* a pane found by name */
struct hdStaffRollPaneVtbl_c {
    /* 0x00 */ u8 _00[0x14];
    /* 0x14 */ gfn mGetRuntimeTypeInfo;
    /* 0x18 */ u8 _18[0x44];
    /* 0x5C */ gfn mFindPaneByName;
};
struct hdStaffRollPane_c {
    /* 0x00 */ u8 _00[8];
    /* 0x08 */ gptr<hdStaffRollPaneVtbl_c> mVtable;
    hdPartsTypeInfo_c* getRuntimeTypeInfo() { return gabi::call_ptr<hdPartsTypeInfo_c*>(mVtable->mGetRuntimeTypeInfo.get(), this); }
};

/* callees (by address; names ours) */
hdStaffRollColors_c* hdStaffRoll_getTextColors(void* pane);   /* 02876428 */
void hdStaffRoll_constructMessage(hdStaffRollMsg_c* msg);     /* 025F65F4 */
void hdStaffRoll_setMessageLabel(hdStaffRollMsg_c* msg, hdUiSafeString* data, hdUiSafeString* label);   /* 025F6D70 */
void hdStaffRoll_buildMessage(void* builder, hdStaffRollMsg_c* msg, void* text, void* text2);   /* 025F50E8 */
inline void hdStaffRoll_destroyArray(void* array, u32 count, u32 size, u32 destructor) { gabi::call(0x028F0164, array, count, size, destructor, 0u, 0u); }

enum {
    STAFF_ROLL_LINE_COUNT = 16,   /* line 0: the job title, 1..14: names, 15: all rights reserved */
    STAFF_ROLL_FIRST_NAME = 1,
    STAFF_ROLL_LAST_NAME = 14,
    STAFF_ROLL_ALL_RIGHTS = 15,
    STAFF_ROLL_LABEL_COUNT = 187,
};

/* the 16 line pane names (initFromNames' stack array) */
struct hdStaffRollNames_c {
    hdUiSafeString mNames[STAFF_ROLL_LINE_COUNT];
};
WWHD_SIZE(hdStaffRollNames_c, 0x80);

struct hdScreenStaffRoll_c : hdParts_c {
    /* 0x50 */ gptr<hdStaffRollLine_c> mpLines[STAFF_ROLL_LINE_COUNT];
    /* 0x90 */ be<f32> mLineHeight;
    /* 0x94 */ be<s32> mTop;            /* ring index of the top line (lines 1..14) */
    /* 0x98 */ be<s32> mLineNum;        /* 14 */
    /* 0x9C */ be<f32> mX;
    /* 0xA0 */ be<s32> mNextLabel;
    /* 0xA4 */ be<f32> mSpeed;
    /* 0xA8 */ hdStaffRollColorPair_c mHeadingColors;
    /* 0xB0 */ hdStaffRollColorPair_c mNameColors;

    static void destroy(hdScreenStaffRoll_c* i_this, s32 flags);
    f32 getAlpha(f32 y);
    void scrollLines();
    s32 getBottomLine();
    s32 getTopLine();
    u32 isLabelEmpty(u32 index);
    void refillTop();
    void allExecute();
    void scrollAllRights();
    void copyRightExecute();
    static hdScreenStaffRoll_c* construct(hdScreenStaffRoll_c* i_this);
    u32 init(u32 arg1, u32 arg2, void* desc);
    void finalize();
    void executeState();
    u32 load(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc);

    hdStaffRollLine_c* line(u32 index) { return mpLines[index < STAFF_ROLL_LINE_COUNT ? index : 0]; }
};
WWHD_OFFSET(hdScreenStaffRoll_c, mpLines, 0x50);
WWHD_OFFSET(hdScreenStaffRoll_c, mLineHeight, 0x90);
WWHD_OFFSET(hdScreenStaffRoll_c, mNextLabel, 0xA0);
WWHD_OFFSET(hdScreenStaffRoll_c, mSpeed, 0xA4);
WWHD_OFFSET(hdScreenStaffRoll_c, mHeadingColors, 0xA8);
WWHD_OFFSET(hdScreenStaffRoll_c, mNameColors, 0xB0);
WWHD_SIZE(hdScreenStaffRoll_c, 0xB8);

/* the per-TU header statics when the pi pair and the two runtime objects are apart (StaffRoll TUs) */
inline void hdStaffRoll_initHeaderStatics(u32 zeroed, u32 desc, u32 piPair, u32 piRodata, u32 objects) {
    for (int i = 0; i < 4; i++) gabi::store<u32>(zeroed + 4 * i, 0);
    __register_global_object(desc);
    f32 lo = gabi::load<f32>(piRodata), hi = gabi::load<f32>(piRodata + 4);
    gabi::store<f32>(piPair, lo);
    gabi::store<f32>(piPair + 4, hi);
    gabi::call(0x028ED6F8, objects);
    __register_global_object(desc + 0xC);
    gabi::call(0x028EAB2C, objects + 1);
    __register_global_object(desc + 0x18);
}

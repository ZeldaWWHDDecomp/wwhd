/* d_a_bg (room background models), WWHD layout. 
 * HD: each BgModel has an extra flag (+0x10, "draw in the special list"), the ROOM heap path of
 * daBg_c::create is gone, sead::SafeString compares of the stage name select per-stage special
 * cases in createHeap/draw/execute. */
#pragma once
#include "bindings.h"

#define DABG_SAFESTRING_VTBL 0x1000886Cu /* this TU's sead::SafeString vtable */
#define DABG_VTBL 0x100089F8u            /* daBg_c vtable (HD virtual destructor) */

struct daBg_btkAnm_c {
    void play();
    void entry(J3DModelData* modelData);

    /* 0x00 */ gptr<mDoExt_btkAnm> anm;
    /* 0x04 */ be<u8> special;
    /* 0x05 */ u8 _05[3];
};
WWHD_SIZE(daBg_btkAnm_c, 8);

struct daBg_brkAnm_c {
    u32 play();
    void entry(J3DModelData* modelData);

    /* 0x00 */ gptr<mDoExt_btkAnm> anm; /* mDoExt_brkAnm (HD 0x78), frame at +4 as well */
    /* 0x04 */ be<u8> special;
    /* 0x05 */ u8 _05[3];
};
WWHD_SIZE(daBg_brkAnm_c, 8);

struct daBg_BgModel {
    /* 0x00 */ gptr<J3DModel> model;
    /* 0x04 */ gptr<daBg_btkAnm_c> btk;
    /* 0x08 */ gptr<daBg_brkAnm_c> brk;
    /* 0x0C */ gptr<dKy_tevstr_c> mpTevStr;
    /* 0x10 */ be<u8> mFlag; /* HD */
    /* 0x11 */ u8 _11[3];
};
WWHD_SIZE(daBg_BgModel, 0x14);

struct daBg_c : fopAc_ac_c {
    BOOL createHeap();
    BOOL draw();
    BOOL execute();

    /* 0x3AC */ request_of_phase_process_class mPhs; /* GameCube 0x290 */
    /* 0x3B4 */ daBg_BgModel bg[4];
    /* 0x404 */ gptr<dBgW> bgw;
    /* 0x408 */ be<u8> mUnloadTimer;
};
WWHD_OFFSET(daBg_c, bg, 0x3B4);
WWHD_OFFSET(daBg_c, bgw, 0x404);
WWHD_OFFSET(daBg_c, mUnloadTimer, 0x408);

/* sead::SafeString {const char*; vtable} with this TU's vtable; assureTermination is virtual (+0x14) */
struct daBg_SafeString {
    be<u32> mStr;
    be<u32> __vtbl;
};
static inline void daBg_ss_assure(daBg_SafeString* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
/* inline sead::SafeString::operator==(lit, stage name): a = {lit}, b = {play + 0x5134 (start stage name)} */
static inline bool daBg_ss_cmp(daBg_SafeString* a, daBg_SafeString* b) {
    daBg_ss_assure(a);
    daBg_ss_assure(a);
    u32 pa = a->mStr;
    daBg_ss_assure(b);
    u32 pb = b->mStr;
    if (pa == pb)
        return true;
    pa = a->mStr;
    pb = b->mStr;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(pa + n);
        u8 cb = gabi::load<u8>(pb + n);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
static inline bool daBg_isStage(u32 lit) {
    gabi::Local<daBg_SafeString> a;
    a->__vtbl = DABG_SAFESTRING_VTBL;
    a->mStr = lit;
    u32 play = dComIfGp_ea();
    gabi::Local<daBg_SafeString> b;
    b->__vtbl = DABG_SAFESTRING_VTBL;
    b->mStr = play + 0x5134;
    return daBg_ss_cmp(a.get(), b.get());
}
/* dStage_roomControl_c::mStatus[roomNo] (0x22C bytes each) at 0x1047E6CC: tevStr at +0x54, flags at +0x21C,
 * bgW at +0x228 */
static inline u32 daBg_roomStatus(s32 roomNo) { return 0x1047E6CCu + roomNo * 0x22C; }

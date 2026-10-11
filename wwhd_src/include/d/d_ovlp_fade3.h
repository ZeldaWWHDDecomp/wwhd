#pragma once
#include "gc_api/gc_api.h"
#include "f_op/f_op_overlap.h"

struct dDlst_base_c { be<u32> __vtbl; };
struct dDlst_snapShot_c : dDlst_base_c {};
// HD: The two textured draw packets expanded from 0x44 to 0x70 bytes.
struct dDlst_2DT2_c : dDlst_base_c { u8 padding[0x6C]; };
WWHD_SIZE(dDlst_base_c, 4);
WWHD_SIZE(dDlst_snapShot_c, 4);
WWHD_SIZE(dDlst_2DT2_c, 0x70);

struct dOvlpFd3_c : overlap_task_class {
    using executeFn = BOOL (dOvlpFd3_c::*)();
    void setExecute(BOOL (*function)(u32)) {
        mExecuteFn.d = 0;
        mExecuteFn.i = -1;
        mExecuteFn.f = gabi::guest_fn((const void*)function);
    }
    /* 0xD0 */ gabi::ptmf<executeFn> mExecuteFn;
    /* 0xD8 */ dDlst_snapShot_c mSnapshot_c;
    /* 0xDC */ dDlst_2DT2_c field_0xdc;
    /* 0x14C */ dDlst_2DT2_c field_0x120;
    /* 0x1BC */ be<u8> field_0x164;
    u8 padding[3];
};
WWHD_OFFSET(dOvlpFd3_c, mExecuteFn, 0xD0);
WWHD_OFFSET(dOvlpFd3_c, mSnapshot_c, 0xD8);
WWHD_OFFSET(dOvlpFd3_c, field_0xdc, 0xDC);
WWHD_OFFSET(dOvlpFd3_c, field_0x120, 0x14C);
WWHD_OFFSET(dOvlpFd3_c, field_0x164, 0x1BC);
WWHD_SIZE(dOvlpFd3_c, 0x1C0);

// HD: Native rendering replaces the GameCube snapshot/2D draw-list submissions.
struct dOvlpFadeRenderer_c {
    void capture(s32 channel, u32 context);
    void draw(s32 channel, u32 context, s32 enable);
    void release(s32 channel);
};
struct dOvlpDrawRegistry_c {
    void add(dOvlpFd3_c* task);
    void clear();
};
struct dOvlp_play_c {
    u8 padding[0x5AC9];
    be<u8> mWindowNum;
};
WWHD_OFFSET(dOvlp_play_c, mWindowNum, 0x5AC9);
WWHD_SIZE(dOvlp_play_c, 0x5ACA);
inline dOvlp_play_c* dOvlp_getPlay() { return gabi::at<dOvlp_play_c>(dComIfGp_ea()); }
inline void dComIfGp_setWindowNum(u8 value) { dOvlp_getPlay()->mWindowNum = value; }

void dDlst_base_construct(dDlst_base_c*);
void dDlst_base_destroy(dDlst_base_c*, s32);
void fpcBs_construct(base_process_class*);
void fpcBs_destruct(base_process_class*, s32);
void fopOvlpM_Done(dOvlpFd3_c*);
u32 fopOvlpM_IsOutReq(dOvlpFd3_c*);
void fopOvlpM_SceneIsStart();
void fopOvlpM_SceneIsStop();
void mDoGph_fadeOut(f32);
struct mDoGph_gInf_c { static void fadeOut(f32 speed) { mDoGph_fadeOut(speed); } };

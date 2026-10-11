/* Overlap fade: tww names, WWHD task base adds four bytes before the counters. */
#pragma once
#include "bindings.h"
#include "f_op/f_op_overlap.h"

struct overlap1_class : overlap_task_class {
    /* 0xD0 */ be<u32> mFadeOutTime; // HD: unsigned wrapping counters.
    /* 0xD4 */ be<u32> mFadeInTime;
};
WWHD_OFFSET(overlap1_class, mFadeOutTime, 0xD0);
WWHD_OFFSET(overlap1_class, mFadeInTime, 0xD4);
WWHD_SIZE(overlap1_class, 0xD8);
enum : s16 { fpcNm_OVERLAP0_e=0, fpcNm_OVERLAP6_e=2, fpcNm_OVERLAP7_e=3 };

struct dOvlpFd_SafeStringVtable {
    u8 _00[0x14];
    /* 0x14 */ gfn assureTerminationImpl;
};
WWHD_OFFSET(dOvlpFd_SafeStringVtable, assureTerminationImpl, 0x14);
WWHD_SIZE(dOvlpFd_SafeStringVtable, 0x18);

u32 dOvlpFd_startFadeInHD(void*, u32, u32);
void dOvlpFd_finishFadeHD(void*);
void mDoGph_startFadeOutHD(u32); // HD/glue: free adapter for the external static graphics method.
void fopOvlpM_SceneIsStart();
u32 fopOvlpM_IsOutReq(overlap1_class*);
void fopOvlpM_Done(overlap1_class*);

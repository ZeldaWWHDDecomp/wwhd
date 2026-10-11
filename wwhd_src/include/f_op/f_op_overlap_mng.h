/* Overlap process manager, WWHD. */
#pragma once
#include "gc_api/gc_api.h"
#include "SSystem/SComponent/c_request.h"
#include "f_pc/f_pc_base.h"
#include "f_op/f_op_scene_pause.h"
struct overlap_method_class;
struct overlap_task_class {
    leafdraw_class base;
    // HD: virtual destructor slot moves these GameCube fields by four bytes.
    gptr<overlap_method_class> sub_method;
    request_base_class mRq;
    u8 padding[3];
    be<u32> mScenePId;
};
WWHD_OFFSET(overlap_task_class, mRq, 0xC8);
WWHD_OFFSET(overlap_task_class, mScenePId, 0xCC);
WWHD_SIZE(overlap_task_class, 0xD0);
struct overlap_request_class {
    request_base_class base;
    u8 mDelayData[3];
    /* 0x04 */ be<u16> mRequestState;
    be<u16> mPeektime;
    /* 0x08 */ be<u32> mIsPeek;
    u8 mProcessAndPhase[0x14];
    /* 0x20 */ gptr<overlap_task_class> mpTask;
    gptr<layer_class> pCurrentLayer;
};
WWHD_SIZE(overlap_request_class, 0x28);
WWHD_OFFSET(overlap_request_class, mpTask, 0x20);
using fopOvlpM_RequestPtr = gptr<overlap_request_class>;
scene_class* fpcEx_SearchByID(u32);
BOOL fopOvlpReq_OverlapClr(overlap_request_class*);
overlap_request_class* fopOvlpReq_Request(overlap_request_class*, u32 procName, u32 peekTime, u32 flags);
u32 fopOvlpReq_Handler(overlap_request_class*);
BOOL fopOvlpReq_Cancel(overlap_request_class*);
struct OverlapMngHeaderStatics {
    // HD: initializer copies the pi words without float conversion.
    be<u32> negativePiBits;
    be<u32> positivePiBits;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct OverlapMngPiPair { be<u32> negativePiBits; be<u32> positivePiBits; };
WWHD_OFFSET(OverlapMngHeaderStatics, zero, 0xC);
WWHD_SIZE(OverlapMngHeaderStatics, 0x1C);
void OverlapMngFirstHeaderObject_ct(u8*);
void OverlapMngSecondHeaderObject_ct(u8*);

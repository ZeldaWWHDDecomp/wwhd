/* Overlap process requests. HD adds a mode byte to the GameCube request. */
#pragma once
#include "gc_api/gc_api.h"
#include "SSystem/SComponent/c_request.h"
#include "SSystem/SComponent/c_phase.h"
#include "gc_api/f_pc/f_pc.h"

// The overlap task owns the remaining process fields.
struct overlap_task_class {
    u8 mProcess[0xC8];
    /* 0xC8 */ request_base_class mRq;
    u8 mPadding[7];
};
WWHD_OFFSET(overlap_task_class, mRq, 0xC8);
WWHD_SIZE(overlap_task_class, 0xD0);
struct overlap_request_class {
    /* 0x00 */ request_base_class base;
    u8 mPadding;
    /* 0x02 */ be<s16> mDelay;
    /* 0x04 */ be<u16> mRequestState;
    /* 0x06 */ be<u16> mPeektime;
    /* 0x08 */ be<u32> mIsPeek;
    /* 0x0C */ be<u32> mOverlapState;
    /* 0x10 */ be<s16> mProcName;
    u8 mNamePadding[2];
    /* 0x14 */ be<u32> mPId;
    /* 0x18 */ request_of_phase_process_class mPhs;
    /* 0x20 */ gptr<overlap_task_class> mpTask;
    /* 0x24 */ gptr<layer_class> pCurrentLayer;
    /* 0x28 */ be<u8> mMode;
    u8 mModePadding[3];
};
WWHD_OFFSET(overlap_request_class, mDelay, 2);
WWHD_OFFSET(overlap_request_class, mRequestState, 4);
WWHD_OFFSET(overlap_request_class, mPeektime, 6);
WWHD_OFFSET(overlap_request_class, mIsPeek, 8);
WWHD_OFFSET(overlap_request_class, mOverlapState, 0xC);
WWHD_OFFSET(overlap_request_class, mProcName, 0x10);
WWHD_OFFSET(overlap_request_class, mPId, 0x14);
WWHD_OFFSET(overlap_request_class, mPhs, 0x18);
WWHD_OFFSET(overlap_request_class, mpTask, 0x20);
WWHD_OFFSET(overlap_request_class, pCurrentLayer, 0x24);
WWHD_OFFSET(overlap_request_class, mMode, 0x28);
WWHD_SIZE(overlap_request_class, 0x2C);
struct OverlapReqPhaseTable { cPhs__Handler entries[8]; };
struct OverlapReqGameInfo {
    u8 mGameState[0x444];
    be<u8> mOverlapMode;
};
WWHD_OFFSET(OverlapReqGameInfo, mOverlapMode, 0x444);
WWHD_SIZE(OverlapReqGameInfo, 0x445);
using OverlapReqGameInfoPtr = gptr<OverlapReqGameInfo>;

enum OverlapReqCommand { OverlapReq_START = 1, OverlapReq_CLEAR = 2 };
BOOL fpcM_Delete(overlap_task_class*);
BOOL fpcM_IsCreating(u32);
overlap_task_class* fpcEx_SearchByID(u32);
layer_class* fpcLy_RootLayer();
u32 fpcSCtRq_Request(layer_class*, s32, u32, u32, u32);

struct OverlapReqHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 mPadding[2];
    be<u32> zero[4];
};
WWHD_OFFSET(OverlapReqHeaderStatics, zero, 0xC);
WWHD_SIZE(OverlapReqHeaderStatics, 0x1C);
void OverlapReqFirstHeaderObject_ct(u8*);
void OverlapReqSecondHeaderObject_ct(u8*);

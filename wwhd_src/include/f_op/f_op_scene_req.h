/* Scene process requests, WWHD. */
#pragma once
#include "gc_api/gc_api.h"
#include "SSystem/SComponent/c_phase.h"
#include "f_op/f_op_scene_pause.h"

// Only fields exercised by scene requests are named; node creation owns the rest.
struct node_create_request {
    u8 mCreateState[0x40];
    /* 0x40 */ be<s32> mParameter;
    /* 0x44 */ be<u32> mRequestId;
    u8 mNodeState[0xC];
    /* 0x54 */ be<u32> mCreatingID;
    u8 mCreateData[0xC];
};
WWHD_SIZE(node_create_request, 0x64);
WWHD_OFFSET(node_create_request, mCreatingID, 0x54);
struct scene_request_class {
    node_create_request mCrtReq;
    /* 0x64 */ gptr<void> mFadeRequest;
    /* 0x68 */ request_of_phase_process_class mReqPhsProcCls;
    u8 mUnused[4];
};
WWHD_SIZE(scene_request_class, 0x74);
WWHD_OFFSET(scene_request_class, mFadeRequest, 0x64);
WWHD_OFFSET(scene_request_class, mReqPhsProcCls, 0x68);
struct node_create_request_method_class {
    be<u32> mpExecuteFunc;
    be<u32> mpCancelFunc;
    be<u32> mpUnkFunc;
    be<u32> mpPostMethodFunc;
};
WWHD_SIZE(node_create_request_method_class, 0x10);
enum { SceneReq_DELETE = 1, SceneReq_NO_FADE = 0x7FFF };
struct SceneReqPhaseTable { cPhs__Handler entries[8]; };

struct SceneReqHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct SceneReqPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(SceneReqHeaderStatics, zero, 0xC);
WWHD_SIZE(SceneReqHeaderStatics, 0x1C);

BOOL fopOvlpM_ClearOfReq();
BOOL fopOvlpM_IsDoingReq();
BOOL fopOvlpM_IsDone();
void fopOvlpM_ToldAboutID(u32);
// HD: cancellation forwards the current scene request in r3.
BOOL fopOvlpM_Cancel(scene_request_class*);
void* fopOvlpM_Request(u32 procName, u32 peekTime, u32 flags);
cPhs_State fpcNdRq_Execute(node_create_request*);
void fpcNdRq_Delete(node_create_request*);
scene_request_class* fpcNdRq_Request(u32 size, u32 reqType, scene_class*, u32 procName, u32 user, node_create_request_method_class*);
u32 fpcNdRq_ReRequest(u32 requestID, u32 procName, u32 user);
s32 fpcNdRq_Handler();
scene_class* fpcEx_SearchByID(u32);
void SceneReqFirstHeaderObject_ct(u8*);
void SceneReqSecondHeaderObject_ct(u8*);

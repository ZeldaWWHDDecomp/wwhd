/** Overlap Process Request. Verified WWHD behaviour. */
#include "f_op/f_op_overlap_req.h"

static OverlapReqPhaseTable phaseMethod;
static OverlapReqGameInfoPtr l_gameInfo;
static OverlapReqHeaderStatics l_headerStatics;

/* 025DC050 */
static cPhs_State fopOvlpReq_phase_Done(overlap_request_class* i_this) {
    if (fpcM_Delete(i_this->mpTask) != TRUE) {
        return cPhs_INIT_e;
    }
    i_this->mpTask = nullptr;
    i_this->mPeektime = 0;
    i_this->mOverlapState = 0;
    i_this->mIsPeek = 0;
    i_this->mRequestState = 0;
    return cPhs_NEXT_e;
}

/* 025DC0BC */
static cPhs_State fopOvlpReq_phase_IsDone(overlap_request_class* i_this) {
    cReq_Done(&i_this->base);
    s32 delay = i_this->mDelay;
    i_this->mDelay = static_cast<u16>(delay - 1);
    return delay <= 0 ? cPhs_NEXT_e : cPhs_INIT_e;
}

/* 025DC104 */
static cPhs_State fopOvlpReq_phase_IsWaitOfFadeout(overlap_request_class* i_this) {
    if (!cReq_Is_Done(&i_this->mpTask->mRq)) {
        return cPhs_INIT_e;
    }
    i_this->mIsPeek = 0;
    return cPhs_NEXT_e;
}

/* 025DC164 */
static cPhs_State fopOvlpReq_phase_WaitOfFadeout(overlap_request_class* i_this) {
    u16 peekTime = i_this->mPeektime;
    u8 command = i_this->base.mFlags;
    if (peekTime != 0) {
        peekTime = static_cast<u16>(peekTime - 1);
        i_this->mPeektime = peekTime;
    }
    if ((command & cReq_flag2Mask) == OverlapReq_CLEAR && peekTime == 0) {
        cReq_Command(&i_this->mpTask->mRq, OverlapReq_CLEAR);
        return cPhs_NEXT_e;
    }
    i_this->mIsPeek = 1;
    return cPhs_INIT_e;
}

/* 025DC1E0 */
static cPhs_State fopOvlpReq_phase_IsComplete(overlap_request_class* i_this) {
    if (!cReq_Is_Done(&i_this->mpTask->mRq)) {
        return cPhs_INIT_e;
    }
    cReq_Done(&i_this->base);
    return cPhs_NEXT_e;
}

/* 025DC240 */
static cPhs_State fopOvlpReq_phase_IsCreated(overlap_request_class* i_this) {
    if (fpcM_IsCreating(i_this->mPId) != 0) {
        return cPhs_INIT_e;
    }
    overlap_task_class* pBaseProc = fpcEx_SearchByID(i_this->mPId);
    if (!pBaseProc) {
        return cPhs_ERROR_e;
    }
    i_this->mpTask = pBaseProc;
    // HD: pass the request's mode to the game state after creation.
    u8 mode = i_this->mMode;
    l_gameInfo->mOverlapMode = mode;
    return cPhs_NEXT_e;
}

/* 025DC2D8 */
static cPhs_State fopOvlpReq_phase_Create(overlap_request_class* i_this) {
    fpcLy_SetCurrentLayer(i_this->pCurrentLayer);
    s32 procName = i_this->mProcName;
    layer_class* layer = fpcLy_CurrentLayer();
    i_this->mPId = fpcSCtRq_Request(layer, procName, 0, 0, 0);
    return cPhs_NEXT_e;
}

/* 025DC334 */
void fopOvlpReq_SetPeektime(overlap_request_class* i_this, u32 peekTime) {
    // HD: test the full argument register before narrowing to the stored halfword.
    if (peekTime > 0x7FFF) {
        return;
    }
    i_this->mPeektime = static_cast<u16>(peekTime);
}

/* 025DC344 */
overlap_request_class* fopOvlpReq_Request(overlap_request_class* i_this, u32 procName, u32 peekTime, u32 mode) {
    // HD: full argument registers and an additional mode byte.
    if (i_this->mRequestState == 1) {
        return nullptr;
    }
    cReq_Command(&i_this->base, OverlapReq_START);
    i_this->mProcName = static_cast<u16>(procName);
    cPhs_Set(&i_this->mPhs, phaseMethod.entries);
    fopOvlpReq_SetPeektime(i_this, peekTime);
    i_this->mRequestState = 1;
    i_this->mIsPeek = 0;
    i_this->mDelay = 1;
    i_this->mOverlapState = 0;
    i_this->mpTask = nullptr;
    i_this->pCurrentLayer = fpcLy_RootLayer();
    i_this->mMode = static_cast<u8>(mode);
    return i_this;
}

/* 025DC418 */
cPhs_State fopOvlpReq_Handler(overlap_request_class* i_this) {
    // HD: iterate NEXT phases without recursive stack growth.
    for (;;) {
        u32 phase_state = cPhs_Do(&i_this->mPhs, i_this);
        if (phase_state < cPhs_NEXT_e) {
            return cPhs_INIT_e;
        }
        if (phase_state == cPhs_NEXT_e) {
            continue;
        }
        return phase_state == cPhs_COMPLEATE_e ? cPhs_COMPLEATE_e : cPhs_ERROR_e;
    }
}

/* 025DC488 */
BOOL fopOvlpReq_Cancel(overlap_request_class* i_this) {
    return fopOvlpReq_phase_Done(i_this) == cPhs_NEXT_e;
}

/* 025DC4B4 */
BOOL fopOvlpReq_Is_PeektimeLimit(overlap_request_class* i_this) {
    return i_this->mPeektime == 0;
}

/* 025DC4C4 */
BOOL fopOvlpReq_OverlapClr(overlap_request_class* i_this) {
    if ((i_this->base.mFlags & cReq_flag0) != 0 || !fopOvlpReq_Is_PeektimeLimit(i_this)) {
        return FALSE;
    }
    cReq_Create(&i_this->base, OverlapReq_CLEAR);
    return TRUE;
}

// HD: compiler-generated and template code of this translation unit.
/* 025DC524 */
static void __sinit_f_op_overlap_req_cpp() {
    for (int i = 0; i < 4; ++i) {
        l_headerStatics.zero[i] = 0;
    }
    __register_global_object(0x101F3714);
    l_headerStatics.negativePi = -3.1415927f;
    l_headerStatics.positivePi = 3.1415927f;
    OverlapReqFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3720);
    OverlapReqSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F372C);
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data and other-unit calls
 data phaseMethod 101F36F4
 data l_gameInfo 101F86B4
 data l_headerStatics 1048A588
 ext 025DF944 BOOL fpcM_Delete(overlap_task_class*)
 ext 0201A330 void cReq_Done(request_base_class*)
 ext 0201A308 BOOL cReq_Is_Done(request_base_class*)
 ext 0201A34C void cReq_Command(request_base_class*, u32)
 ext 025DD868 BOOL fpcM_IsCreating(u32)
 ext 025DE50C overlap_task_class* fpcEx_SearchByID(u32)
 ext 025E14A8 u32 fpcSCtRq_Request(layer_class*, s32, u32, u32, u32)
 ext 02019F0C void cPhs_Set(request_of_phase_process_class*, cPhs__Handler*)
 ext 025DEAA8 layer_class* fpcLy_RootLayer()
 ext 02019F74 cPhs_State cPhs_Do(request_of_phase_process_class*, void*)
 ext 0201A33C void cReq_Create(request_base_class*, u32)
 ext 028ED6F8 void OverlapReqFirstHeaderObject_ct(u8*)
 ext 028EAB2C void OverlapReqSecondHeaderObject_ct(u8*)
*/

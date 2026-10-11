/** Framework - Scene Process Request. WWHD. */
#include "f_op/f_op_scene_req.h"

static s32 l_fopScnRq_IsUsingOfOverlap;
static node_create_request_method_class l_submethod;
static SceneReqPhaseTable l_noFadeFase;
static SceneReqPhaseTable l_fadeFase;
static SceneReqHeaderStatics l_headerStatics;
static const SceneReqPiPair l_piPair;

/* 025DCC28 */
static cPhs_State fopScnRq_phase_ClearOverlap() {
    return fopOvlpM_ClearOfReq() == TRUE ? cPhs_NEXT_e : cPhs_INIT_e;
}

/* 025DCC64 */
static cPhs_State fopScnRq_phase_Execute(scene_request_class* i_sceneReq) {
    return fpcNdRq_Execute(&i_sceneReq->mCrtReq);
}

/* 025DCC68 */
static cPhs_State fopScnRq_phase_IsDoingOverlap() {
    return fopOvlpM_IsDoingReq() == TRUE ? cPhs_NEXT_e : cPhs_INIT_e;
}

/* 025DCCA4 */
static cPhs_State fopScnRq_phase_IsDoneOverlap() {
    return fopOvlpM_IsDone() == TRUE ? cPhs_NEXT_e : cPhs_INIT_e;
}

/* 025DCCE0 */
static cPhs_State fopScnRq_phase_Done(scene_request_class* i_sceneReq) {
    if (i_sceneReq->mCrtReq.mParameter != SceneReq_DELETE) {
        fopScnPause_Disable(fpcEx_SearchByID(i_sceneReq->mCrtReq.mCreatingID));
    }
    l_fopScnRq_IsUsingOfOverlap = 0;
    return cPhs_NEXT_e;
}

/* 025DCD24 */
static cPhs_State fopScnRq_Execute(scene_request_class* i_sceneReq) {
    // HD: return the final phase result and iterate rather than recurse.
    cPhs_State phaseState;
    do {
        phaseState = cPhs_Do(&i_sceneReq->mReqPhsProcCls, i_sceneReq);
    } while (phaseState == cPhs_NEXT_e);
    return phaseState;
}

/* 025DCD68 */
static int fopScnRq_PostMethod(scene_class* i_scene, scene_request_class* i_sceneReq) {
    fopScnPause_Enable(i_scene);
    if (i_sceneReq->mFadeRequest) {
        fopOvlpM_ToldAboutID(i_scene->mBsPcId);
    }
    return 1;
}

/* 025DCDB8 */
static int fopScnRq_Cancel(scene_request_class* i_sceneReq) {
    if (i_sceneReq->mFadeRequest && !fopOvlpM_Cancel(i_sceneReq)) {
        return 0;
    }
    return 1;
}

/* 025DCE04 */
u32 fopScnRq_Request(u32 reqType, scene_class* i_scene, u32 procName, u32 user, u32 fadeProcName, u32 fadePeekTime, u32 flags) {
    // HD: full register widths and an extra flags argument for the fade request.
    scene_request_class* pScnReq = fpcNdRq_Request(sizeof(scene_request_class), reqType, i_scene, procName, user, &l_submethod);
    if (!pScnReq) {
        return -1;
    }
    if (fadeProcName != SceneReq_NO_FADE) {
        void* fade = nullptr;
        if (l_fopScnRq_IsUsingOfOverlap == 0) {
            fade = fopOvlpM_Request(fadeProcName, fadePeekTime, flags);
        }
        if (!fade) {
            fpcNdRq_Delete(&pScnReq->mCrtReq);
            return -1;
        }
        l_fopScnRq_IsUsingOfOverlap = 1;
        pScnReq->mFadeRequest = fade;
        cPhs_Set(&pScnReq->mReqPhsProcCls, l_fadeFase.entries);
    } else {
        pScnReq->mFadeRequest = nullptr;
        cPhs_Set(&pScnReq->mReqPhsProcCls, l_noFadeFase.entries);
    }
    return pScnReq->mCrtReq.mRequestId;
}

/* 025DCF08 */
u32 fopScnRq_ReRequest(u32 i_requestID, u32 i_procName, u32 i_data) {
    return fpcNdRq_ReRequest(i_requestID, i_procName, i_data);
}

/* 025DCF0C */
s32 fopScnRq_Handler() {
    return fpcNdRq_Handler();
}

/* 025DCF10 */
static void __sinit_f_op_scene_req_cpp() {
    // HD: common-header initialization retains the verified store/read/call order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3834);
    f32 lo = l_piPair.negativePi;
    f32 hi = l_piPair.positivePi;
    l_headerStatics.negativePi = lo;
    l_headerStatics.positivePi = hi;
    SceneReqFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3840);
    SceneReqSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F384C);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and other-unit calls
 data l_fopScnRq_IsUsingOfOverlap 101F37E0
 data l_submethod 101F37E4
 data l_noFadeFase 101F37F4
 data l_fadeFase 101F3814
 data l_headerStatics 1048A614
 data l_piPair 10057DA4
 ext 025DBE64 BOOL fopOvlpM_ClearOfReq()
 ext 025E04D0 cPhs_State fpcNdRq_Execute(node_create_request*)
 ext 025DBE38 BOOL fopOvlpM_IsDoingReq()
 ext 025DBE1C BOOL fopOvlpM_IsDone()
 ext 025DE50C scene_class* fpcEx_SearchByID(u32)
 ext 025DCAC4 int fopScnPause_Disable(scene_class*)
 ext 02019F74 cPhs_State cPhs_Do(request_of_phase_process_class*, void*)
 ext 025DCA64 int fopScnPause_Enable(scene_class*)
 ext 025DBDE4 void fopOvlpM_ToldAboutID(u32)
 ext 025DBF3C BOOL fopOvlpM_Cancel(scene_request_class*)
 ext 025E0840 scene_request_class* fpcNdRq_Request(u32 size, u32 reqType, scene_class*, u32 procName, u32 user, node_create_request_method_class*)
 ext 025DBE80 void* fopOvlpM_Request(u32 procName, u32 peekTime, u32 flags)
 ext 025E0538 void fpcNdRq_Delete(node_create_request*)
 ext 02019F0C void cPhs_Set(request_of_phase_process_class*, cPhs__Handler*)
 ext 025E0A20 u32 fpcNdRq_ReRequest(u32 requestID, u32 procName, u32 user)
 ext 025E062C s32 fpcNdRq_Handler()
 ext 028ED6F8 void SceneReqFirstHeaderObject_ct(u8*)
 ext 028EAB2C void SceneReqSecondHeaderObject_ct(u8*)
*/

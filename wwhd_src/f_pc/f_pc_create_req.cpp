/** Process creation requests. */
#include "f_pc/f_pc_create_req.h"

static CreateRequestHeaderStatics l_headerStatics;

/* 025DD84C */
BOOL fpcCtRq_isCreatingByID(create_tag* i_createTag, be<u32>* i_procID) {
    create_request* pReq = gabi::at<create_request>(i_createTag->base.mpTagData);
    return pReq->mBsPcId == *i_procID;
}

/* 025DD868 */
BOOL fpcCtRq_IsCreatingByID(u32 i_id) {
    // HD: preserve the original guest-stack key and frame seen by the iterator callback.
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::Local<be<u32>> key;
    *key.get() = i_id;
    return fpcCtIt_Judge(gabi::guest_fn((const void*)fpcCtRq_isCreatingByID), key.get()) != NULL;
}

/* 025DD8A0 */
BOOL fpcCtRq_Delete(create_request* i_request) {
    fpcCtTg_CreateQTo(&i_request->base);
    fpcLy_CreatedMesg(i_request->mpLayer);
    fpcLy_CancelQTo(&i_request->mMtdTg);
    create_request_method_class* methods = i_request->mpCtRqMtd;
    if (methods && !fpcMtd_Method(methods->mpDelete.get(), i_request)) {
        return FALSE;
    }
    base_process_class* proc = i_request->mpRes;
    if (proc) {
        proc->mpCtRq = 0;
    }
    cMl::free(i_request);
    return TRUE;
}

/* 025DD934 */
BOOL fpcCtRq_Cancel(create_request* i_request) {
    if (!i_request || i_request->mbIsCancelling) {
        return TRUE;
    }
    // HD: capture the process before setting the cancelling byte, as in the verified entry.
    base_process_class* proc = i_request->mpRes;
    i_request->mbIsCancelling = TRUE;
    if (proc && !fpcDt_Delete(proc)) {
        return FALSE;
    }
    create_request_method_class* methods = i_request->mpCtRqMtd;
    if (methods && !fpcMtd_Method(methods->mpCancel.get(), i_request)) {
        return FALSE;
    }
    return fpcCtRq_Delete(i_request);
}

/* 025DD9E4 */
BOOL fpcCtRq_IsDoing(create_request* i_request) {
    return i_request ? i_request->mbIsCreating : FALSE;
}

/* 025DD9F8 */
BOOL fpcCtRq_Do(create_request* i_request) {
    u32 phase = cPhs_COMPLEATE_e;
    create_request_method_class* methods = i_request->mpCtRqMtd;
    if (methods) {
        u32 handler = methods->mpHandler.get();
        if (handler) {
            i_request->mbIsCreating = TRUE;
            phase = fpcCtRq_callHandler(handler, i_request);
            i_request->mbIsCreating = FALSE;
        }
    }
    switch (phase) {
    case cPhs_COMPLEATE_e:
        if (fpcEx_ToExecuteQ(i_request->mpRes)) {
            return fpcCtRq_Delete(i_request);
        }
        return fpcCtRq_Cancel(i_request);
    case cPhs_STOP_e:
    case cPhs_ERROR_e:
        return fpcCtRq_Cancel(i_request);
    default:
        return TRUE;
    }
}

/* 025DDAC4 */
BOOL fpcCtRq_Handler() {
    return fpcCtIt_Method(gabi::guest_fn((const void*)fpcCtRq_Do), NULL);
}

/* 025DDAD4 */
create_request* fpcCtRq_Create(layer_class* i_layer, u32 i_size,
                              create_request_method_class* i_CtRqMethods) {
    create_request* pReq = (create_request*)cMl::memalignB(-4, i_size);
    if (pReq) {
        fpcCtTg_Init(&pReq->base, pReq);
        fpcMtdTg_Init(&pReq->mMtdTg, gabi::guest_fn((const void*)fpcCtRq_Cancel), pReq);
        pReq->mpLayer = i_layer;
        pReq->mpCtRqMtd = i_CtRqMethods;
        // HD: only this byte of the GameCube mpUnk1 slot is initialized here.
        pReq->hdFastCreateState = 0;
        pReq->mBsPcId = fpcBs_MakeOfId();
        pReq->mbIsCancelling = FALSE;
        pReq->mpRes = NULL;
        pReq->mbIsCreating = FALSE;
        fpcLy_CreatingMesg(pReq->mpLayer);
        fpcLy_ToCancelQ(pReq->mpLayer, &pReq->mMtdTg);
        fpcCtTg_ToCreateQ(&pReq->base);
    }
    return pReq;
}

/* 025DDB84 */
static void __sinit_f_pc_create_req_cpp() {
    // HD: retain this unit's header-object initialization and registration order.
    for (int i = 0; i < 4; ++i) {
        l_headerStatics.zero[i] = 0;
    }
    __register_global_object(0x101F395C);
    l_headerStatics.negativePi = -3.1415927f;
    l_headerStatics.positivePi = 3.1415927f;
    CreateRequestFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3968);
    CreateRequestSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3974);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_headerStatics 1048A6D8
ext 025DD714 void* fpcCtIt_Judge(u32, void*)
ext 025DD6D8 BOOL fpcCtIt_Method(u32, void*)
ext 025DDC28 void fpcCtTg_CreateQTo(create_tag*)
ext 025DDC18 void fpcCtTg_ToCreateQ(create_tag*)
ext 025DDC2C void fpcCtTg_Init(create_tag*, void*)
ext 025DEA90 void fpcLy_CreatedMesg(layer_class*)
ext 025DEA80 void fpcLy_CreatingMesg(layer_class*)
ext 025DE9E0 void fpcLy_CancelQTo(process_method_tag_class*)
ext 025DE9E4 void fpcLy_ToCancelQ(layer_class*, process_method_tag_class*)
ext 025DFE2C void fpcMtdTg_Init(process_method_tag_class*, u32, void*)
ext 025DFCAC BOOL fpcMtd_Method(u32, void*)
ext 025DE1B8 BOOL fpcDt_Delete(base_process_class*)
ext 025DE720 BOOL fpcEx_ToExecuteQ(base_process_class*)
ext 025DD290 u32 fpcBs_MakeOfId()
ext 028ED6F8 void CreateRequestFirstHeaderObject_ct(u8*)
ext 028EAB2C void CreateRequestSecondHeaderObject_ct(u8*)
*/

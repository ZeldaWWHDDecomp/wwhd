/** Standard process creation requests, GameCube structure verified against WWHD. Local only.
 * HD: no module-load phase and no module-free call on process creation failure.
 */
#include "f_pc/f_pc_stdcreate_req.h"

static create_request_method_class submethod;
// HD: the phase table starts at CreateProcess, without the GameCube Load phase.
static StandardCreatePhaseTable method;
static s32 g_fpcNd_type;

/* 025E12E0 */
cPhs_State fpcSCtRq_phase_CreateProcess(standard_create_request_class* i_SCtReq) {
    fpcLy_SetCurrentLayer(i_SCtReq->base.mpLayer);
    s16 name = i_SCtReq->mProcName;
    void* data = i_SCtReq->mpUserData;
    u32 id = i_SCtReq->base.mBsPcId;
    base_process_class* process = fpcBs_Create(name, id, data);
    i_SCtReq->base.mpRes = process;
    if (process == nullptr) {
        return cPhs_ERROR_e;
    } else {
        process->mpCtRq = gabi::ea(i_SCtReq);
        return cPhs_NEXT_e;
    }
}

/* 025E134C */
cPhs_State fpcSCtRq_phase_SubCreateProcess(standard_create_request_class* i_SCtReq) {
    fpcLy_SetCurrentLayer(i_SCtReq->base.mpLayer);
    return fpcBs_SubCreate(i_SCtReq->base.mpRes);
}

/* 025E1384 */
cPhs_State fpcSCtRq_phase_IsComplete(standard_create_request_class* i_SCtReq) {
    StandardCreateNodePrefix* procNode = reinterpret_cast<StandardCreateNodePrefix*>(i_SCtReq->base.mpRes.get());
    if (fpcBs_Is_JustOfType(g_fpcNd_type, procNode->mSubType) == TRUE) {
        if (fpcLy_IsCreatingMesg(procNode->mLayer) == TRUE) {
            return cPhs_INIT_e;
        }
    }
    return cPhs_NEXT_e;
}

/* 025E13F0 */
cPhs_State fpcSCtRq_phase_PostMethod(standard_create_request_class* i_SCtReq) {
    // HD: the callback is stored as a guest code address.
    u32 callback = i_SCtReq->mpCallBack.get();
    if (callback && gabi::call_ptr<s32>(callback, i_SCtReq->base.mpRes.get(),
                                      i_SCtReq->mpCallBackUserData.get()) == 0) {
        return cPhs_INIT_e;
    } else {
        return cPhs_NEXT_e;
    }
}

/* 025E144C */
cPhs_State fpcSCtRq_phase_Done(standard_create_request_class* i_SCtReq) {
    return cPhs_NEXT_e;
}

/* 025E1454 */
cPhs_State fpcSCtRq_Handler(standard_create_request_class* i_SCtReq) {
    // HD: GHS replaces the GameCube recursive NEXT step with a loop.
    cPhs_State phase_state;
    do {
        phase_state = cPhs_Do(&i_SCtReq->mPhase, i_SCtReq);
    } while (phase_state == cPhs_NEXT_e);
    return phase_state;
}

/* 025E1498 */
BOOL fpcSCtRq_Delete(standard_create_request_class* i_SCtReq) {
    return TRUE;
}

/* 025E14A0 */
BOOL fpcSCtRq_Cancel(standard_create_request_class* i_SCtReq) {
    return TRUE;
}

/* 025E14A8 */
fpc_ProcID fpcSCtRq_Request(layer_class* i_layer, s32 i_procName, u32 i_createFunc,
                          void* param_4, void* i_append) {
    // HD: compare the full-width process name before narrowing it into the request.
    if (i_procName >= 0x7FFF) {
        return fpcM_ERROR_PROCESS_ID_e;
    }
    standard_create_request_class* request = reinterpret_cast<standard_create_request_class*>(
        fpcCtRq_Create(i_layer, sizeof(standard_create_request_class), &submethod));
    if (request == nullptr) {
        return fpcM_ERROR_PROCESS_ID_e;
    }
    cPhs_Set(&request->mPhase, method.mHandlers);
    request->mProcName = static_cast<s16>(i_procName);
    request->mpUserData = i_append;
    u32 id = request->base.mBsPcId;
    request->mpCallBackUserData = param_4;
    request->mpCallBack.v = i_createFunc;
    return id;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */
/* 025E1538 */
static void __sinit_f_pc_stdcreate_req_cpp() {
    sinit_header_statics_rodata(0x1048ABA0, 0x101F46BC, 0x1048ABAC, 0x100584A4);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and external operations
data submethod 101F46B0
data method 101F4698
data g_fpcNd_type 101F3D60
ext 025DD414 base_process_class* fpcBs_Create(s32, u32, void*)
ext 025DD538 cPhs_State fpcBs_SubCreate(base_process_class*)
ext 025DD258 BOOL fpcBs_Is_JustOfType(int, int)
ext 025DEA6C BOOL fpcLy_IsCreatingMesg(void*)
ext 025DDAD4 create_request* fpcCtRq_Create(layer_class*, u32, create_request_method_class*)
ext 02019F0C void cPhs_Set(request_of_phase_process_class*, cPhs__Handler*)
ext 02019F74 cPhs_State cPhs_Do(request_of_phase_process_class*, void*)
*/

/** First-creation process requests. */
#include "f_pc/f_pc_fstcreate_req.h"

static create_request_method_class submethod;
static FastCreateHeaderStatics l_headerStatics;
static const FastCreatePiPair l_piPair;

/* 025DE820 */
cPhs_State fpcFCtRq_Do(fast_create_request* i_createReq) {
    if (i_createReq->mpFastCreateFunc.get() &&
        fpcFCtRq_callCreate(i_createReq->mpFastCreateFunc.get(),
                           i_createReq->base.mpRes, i_createReq->mpFastCreateData) == 0) {
        return cPhs_STOP_e;
    }
    return cPhs_COMPLEATE_e;
}

/* 025DE87C */
BOOL fpcFCtRq_Delete(fast_create_request* i_createReq) {
    return TRUE;
}

/* 025DE884 */
base_process_class* fpcFCtRq_Request(layer_class* i_layer, s32 i_procTypeID,
                                   void* i_createFunc, void* i_createData, void* pData) {
    // HD: no fpcLd_Use gate; retain the full-width profile argument passed to fpcBs_Create.
    fast_create_request* request = (fast_create_request*)fpcCtRq_Create(
        i_layer, sizeof(fast_create_request), &submethod);
    if (request) {
        fpcLy_SetCurrentLayer(i_layer);
        u32 id = fpcBs_MakeOfId();
        base_process_class* proc = fpcBs_Create(i_procTypeID, id, pData);
        if (proc) {
            // HD: mark creation before linking the new process back to its request.
            request->base.mbIsCreating = TRUE;
            proc->mpCtRq = gabi::ea(request);
            request->base.mpRes = proc;
            request->base.mBsPcId = proc->mBsPcId;
            if (fpcBs_SubCreate(proc) == cPhs_NEXT_e) {
                // HD: callbacks are stored as guest code addresses.
                request->mpFastCreateFunc.v = gabi::ea(i_createFunc);
                request->mpFastCreateData = i_createData;
                return proc;
            }
        }
        fpcCtRq_Cancel(&request->base);
    }
    return NULL;
}

/* 025DE94C */
static void __sinit_f_pc_fstcreate_req_cpp() {
    // HD: preserve the header objects' zero-store and registration order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3AC4);
    l_headerStatics.negativePi = l_piPair.negativePi;
    l_headerStatics.positivePi = l_piPair.positivePi;
    FastCreateFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3AD0);
    FastCreateSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3ADC);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data submethod 101F3AB8
data l_headerStatics 1048A7D4
data l_piPair 100582E8
ext 025DDAD4 create_request* fpcCtRq_Create(layer_class*, u32, create_request_method_class*)
ext 025DD934 BOOL fpcCtRq_Cancel(create_request*)
ext 025DD290 u32 fpcBs_MakeOfId()
ext 025DD414 base_process_class* fpcBs_Create(s32, u32, void*)
ext 025DD538 cPhs_State fpcBs_SubCreate(base_process_class*)
ext 028ED6F8 void FastCreateFirstHeaderObject_ct(u8*)
ext 028EAB2C void FastCreateSecondHeaderObject_ct(u8*)
*/

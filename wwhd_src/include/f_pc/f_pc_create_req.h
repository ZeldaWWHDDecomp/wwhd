/* Creation request layout and GameCube names, verified WWHD fields. */
#pragma once
#include "bindings.h"
#include "gc_api/f_pc/f_pc.h"
#include "gc_api/SSystem/c_malloc.h"
struct create_tag { create_tag_class base; };
struct process_method_tag_class {
    create_tag_class mCreateTag;
    gfn mpFunc;
    gptr<void> mpMthdData;
};
struct create_request_method_class { gfn mpHandler; gfn mpCancel; gfn mpDelete; };
struct create_request {
    create_tag base;
    /* 0x14 */ be<u8> mbIsCreating; // HD: read as an unsigned byte.
    /* 0x15 */ be<u8> mbIsCancelling;
    u8 _16[2];
    /* 0x18 */ process_method_tag_class mMtdTg;
    /* 0x34 */ gptr<create_request_method_class> mpCtRqMtd;
    /* 0x38 */ be<u8> hdFastCreateState; // HD: first-creation state byte, GC mpUnk1 slot.
    u8 _39[3];
    /* 0x3C */ be<u32> mBsPcId;
    /* 0x40 */ gptr<base_process_class> mpRes;
    /* 0x44 */ gptr<layer_class> mpLayer;
};
WWHD_SIZE(create_request, 0x48);
WWHD_SIZE(process_method_tag_class, 0x1C);
WWHD_OFFSET(create_request, mbIsCreating, 0x14);
WWHD_OFFSET(create_request, mMtdTg, 0x18);
WWHD_OFFSET(create_request, mpCtRqMtd, 0x34);
WWHD_OFFSET(create_request, mBsPcId, 0x3C);
BOOL fpcCtRq_isCreatingByID(create_tag*, be<u32>*);
BOOL fpcCtRq_IsCreatingByID(u32);
BOOL fpcCtRq_Delete(create_request*);
BOOL fpcCtRq_Cancel(create_request*);
BOOL fpcCtRq_IsDoing(create_request*);
BOOL fpcCtRq_Do(create_request*);
BOOL fpcCtRq_Handler();
create_request* fpcCtRq_Create(layer_class*, u32, create_request_method_class*);
void* fpcCtIt_Judge(u32, void*);
BOOL fpcCtIt_Method(u32, void*);
void fpcCtTg_CreateQTo(create_tag*);
void fpcCtTg_ToCreateQ(create_tag*);
void fpcCtTg_Init(create_tag*, void*);
void fpcLy_CreatedMesg(layer_class*);
void fpcLy_CreatingMesg(layer_class*);
void fpcLy_CancelQTo(process_method_tag_class*);
void fpcLy_ToCancelQ(layer_class*, process_method_tag_class*);
void fpcMtdTg_Init(process_method_tag_class*, u32, void*);
BOOL fpcMtd_Method(u32, void*);
BOOL fpcDt_Delete(base_process_class*);
BOOL fpcEx_ToExecuteQ(base_process_class*);
u32 fpcBs_MakeOfId();
inline u32 fpcCtRq_callHandler(u32 handler, create_request* request) {
    return gabi::call_ptr<u32>(handler, request);
}
struct CreateRequestHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
WWHD_SIZE(CreateRequestHeaderStatics, 0x1C);
void CreateRequestFirstHeaderObject_ct(u8*);
void CreateRequestSecondHeaderObject_ct(u8*);

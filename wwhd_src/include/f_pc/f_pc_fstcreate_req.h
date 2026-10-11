/* First creation requests, GameCube names with verified WWHD layout. */
#pragma once
#include "bindings.h"
#include "gc_api/f_pc/f_pc.h"

struct create_request_method_class { gfn mpHandler; gfn mpCancel; gfn mpDelete; };
struct create_request {
    u8 _00[0x38];
    /* 0x38 */ be<s8> mbIsCreating;
    u8 _39[3];
    /* 0x3C */ be<u32> mBsPcId;
    /* 0x40 */ gptr<base_process_class> mpRes;
    /* 0x44 */ gptr<layer_class> mpLayer;
};
struct fast_create_request {
    create_request base;
    /* 0x48 */ gfn mpFastCreateFunc;
    /* 0x4C */ gptr<void> mpFastCreateData;
};
WWHD_SIZE(create_request, 0x48);
WWHD_SIZE(fast_create_request, 0x50);
WWHD_OFFSET(create_request, mbIsCreating, 0x38);
WWHD_OFFSET(fast_create_request, mpFastCreateFunc, 0x48);
inline s32 fpcFCtRq_callCreate(u32 callback, base_process_class* process, void* data) {
    return gabi::call_ptr<s32>(callback, process, data);
}
create_request* fpcCtRq_Create(layer_class*, u32, create_request_method_class*);
BOOL fpcCtRq_Cancel(create_request*);
u32 fpcBs_MakeOfId();
base_process_class* fpcBs_Create(s32, u32, void*);
cPhs_State fpcBs_SubCreate(base_process_class*);

/* HD: ordered static header initialization, including two one-byte objects. */
struct FastCreateHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct FastCreatePiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_SIZE(FastCreateHeaderStatics, 0x1C);
WWHD_OFFSET(FastCreateHeaderStatics, zero, 0xC);
void FastCreateFirstHeaderObject_ct(u8*);
void FastCreateSecondHeaderObject_ct(u8*);

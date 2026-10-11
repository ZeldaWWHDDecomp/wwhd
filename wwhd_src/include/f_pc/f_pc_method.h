/* Process method dispatch; GameCube names with guest code addresses. Local only. */
#pragma once
#include "bindings.h"

struct process_method_class {
    /* 0x00 */ gfn mpCreateFunc;
    /* 0x04 */ gfn mpDeleteFunc;
    /* 0x08 */ gfn mpExecuteFunc;
    /* 0x0C */ gfn mpIsDeleteFunc;
};
WWHD_OFFSET(process_method_class, mpExecuteFunc, 8);
WWHD_SIZE(process_method_class, 0x10);
// HD: callbacks are guest code addresses, not host function pointers.
s32 fpcMtd_Method(u32 i_methodFunc, void* i_data);

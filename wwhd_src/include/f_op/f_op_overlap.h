/** Overlap process framework. */
#pragma once
#include "f_pc/f_pc_base.h"
#include "SSystem/SComponent/c_request.h" /* request_base_class, cReq_*: one declaration */

struct leafdraw_method_class;
struct overlap_method_class;

struct overlap_task_class {
    leafdraw_class base;
    // HD: base process vtable shifts GameCube's overlap fields by four bytes.
    gptr<overlap_method_class> sub_method;
    request_base_class mRq;
    u8 _paddingC9[3];
    be<fpc_ProcID> mScenePId;
};
WWHD_OFFSET(overlap_task_class, sub_method, 0xC4);
WWHD_OFFSET(overlap_task_class, mRq, 0xC8);
WWHD_OFFSET(overlap_task_class, mScenePId, 0xCC);
WWHD_SIZE(overlap_task_class, 0xD0);

struct overlap_process_profile_definition : leaf_process_profile_definition {
    gptr<overlap_method_class> sub_method;
};
WWHD_OFFSET(overlap_process_profile_definition, sub_method, 0x24);
WWHD_SIZE(overlap_process_profile_definition, 0x28);

s32 fpcLf_DrawMethod(leafdraw_method_class* method, void* task);
/* cReq_Create: SSystem/SComponent/c_request.h */

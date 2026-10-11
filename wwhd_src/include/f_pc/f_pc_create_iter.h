/* Process creation iterators, WWHD layout (same accessed offsets as GameCube).
 * Only the create-request fields used here are described. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_base.h"

struct node_list_class {
    gptr<void> mpHead;
    gptr<void> mpTail;
    be<s32> mSize;
};
struct method_filter {
    gfn mpMethodFunc;
    gptr<void> mpUserData;
};
struct node_judge_data {
    gfn mFunc;
    gptr<void> mpUserData;
};
struct fpcCtIt_jilprm_c {
    be<u32> mLayerID;
    gfn mFunc;
    gptr<void> mpUserData;
};
struct layer_class {
    /* 0x00 */ u8 padding[0xC];
    /* 0x0C */ be<u32> mLayerID;
};
struct create_request {
    /* 0x00 */ u8 padding[0x40];
    /* 0x40 */ gptr<base_process_class> mpRes;
    /* 0x44 */ gptr<layer_class> mpLayer;
};
struct create_tag {
    create_tag_class base;
};
WWHD_SIZE(node_list_class, 12);
WWHD_SIZE(method_filter, 8);
WWHD_SIZE(node_judge_data, 8);
WWHD_SIZE(fpcCtIt_jilprm_c, 12);
WWHD_OFFSET(layer_class, mLayerID, 0xC);
WWHD_OFFSET(create_request, mpRes, 0x40);
WWHD_OFFSET(create_request, mpLayer, 0x44);
WWHD_SIZE(create_request, 0x48);
WWHD_SIZE(create_tag, 0x14);

/* External callback entry points; HD stores guest function addresses in its filters. */
enum : u32 {
    cTgIt_MethodCall = 0x0201A9F4,
    cTgIt_JudgeFilter = 0x0201AA08,
};
BOOL cLsIt_Method(node_list_class*, u32, method_filter*);
void* cLsIt_Judge(node_list_class*, u32, node_judge_data*);
inline void* fpcCtIt_callJudge(u32 func, void* process, void* data) {
    return gabi::call_ptr<void*>(func, process, data);
}

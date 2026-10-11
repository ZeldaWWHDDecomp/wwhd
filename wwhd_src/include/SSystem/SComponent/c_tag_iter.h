/* Tagged-node iteration. WWHD layout. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_base.h"
#include "SSystem/SComponent/c_node_iter.h"

struct method_filter {
    gfn mpMethodFunc;
    gptr<void> mpUserData;
};
struct judge_filter {
    gfn mpJudgeFunc;
    gptr<void> mpUserData;
};
WWHD_OFFSET(method_filter, mpMethodFunc, 0);
WWHD_OFFSET(method_filter, mpUserData, 4);
WWHD_OFFSET(judge_filter, mpJudgeFunc, 0);
WWHD_OFFSET(judge_filter, mpUserData, 4);
WWHD_SIZE(method_filter, 8);
WWHD_SIZE(judge_filter, 8);
s32 cTgIt_MethodCall(create_tag_class*, method_filter*);
void* cTgIt_JudgeFilter(create_tag_class*, judge_filter*);

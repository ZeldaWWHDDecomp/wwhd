/**
 * Actor process iterator (fopAcIt), WWHD.
 * Ported from zeldaret/tww and verified against cking.rpx, unit f_op_actor_iter.
 */
#include "f_op/f_op_actor_iter.h"

static node_list_class g_fopAcTg_Queue;


/* 025D51DC */
static s32 fopAcIt_Executor(u32 i_execFunc, u32 i_data) {
    method_filter filter;
    filter.mpMethodFunc = i_execFunc;
    filter.mpUserData = i_data;
    return cLsIt_Method(&g_fopAcTg_Queue, cTgIt_MethodCall, &filter);
}

// HD: retain the guest u32 data register; the shared typed API uses void* here.
/* 025D5218 */
static u32 fopAcIt_JudgeGuest(u32 i_judgeFunc, u32 i_data) {
    judge_filter filter;
    filter.mpJudgeFunc = i_judgeFunc;
    filter.mpUserData = i_data;
    return cLsIt_Judge(&g_fopAcTg_Queue, cTgIt_JudgeFilter, &filter);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 025D5254 */
static void __sinit_f_op_actor_iter_cpp() {
    sinit_header_statics(0x10487440, 0x101F30A8);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data g_fopAcTg_Queue 101F3328
ext 020100A0 s32 cLsIt_Method(node_list_class* list, u32 callback, method_filter* data)
ext 020100BC u32 cLsIt_Judge(node_list_class* list, u32 callback, judge_filter* data)
*/

/** Process-creation queue iterators.
 * Verified against WWHD (f_pc_create_iter). */
#include "f_pc/f_pc_create_iter.h"

static node_list_class g_fpcCtTg_Queue;

/* 025DD6D8 */
BOOL fpcCtIt_Method(u32 i_judgeFunc, void* i_data) {
    // HD: the filter stays at sp+8 in the original 0x18-byte frame.
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<method_filter> iter(8);
    iter->mpMethodFunc.v = i_judgeFunc;
    iter->mpUserData = i_data;
    return cLsIt_Method(&g_fpcCtTg_Queue, cTgIt_MethodCall, iter);
}

/* 025DD714 */
void* fpcCtIt_Judge(u32 i_judgeFunc, void* i_data) {
    // HD: callees observe the original stack pointer and filter placement.
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<node_judge_data> iter(8);
    iter->mFunc.v = i_judgeFunc;
    iter->mpUserData = i_data;
    return cLsIt_Judge(&g_fpcCtTg_Queue, cTgIt_JudgeFilter, iter);
}

/* 025DD750 */
void* fpcCtIt_filter_JudgeInLayer(create_tag* i_createTag, fpcCtIt_jilprm_c* i_iterData) {
    create_request* create_req = gabi::at<create_request>(i_createTag->base.mpTagData);
    layer_class* layer = create_req->mpLayer;
    u32 expected = i_iterData->mLayerID;
    if (layer->mLayerID == expected) {
        u32 func = i_iterData->mFunc.get();
        void* user = i_iterData->mpUserData;
        return fpcCtIt_callJudge(func, create_req->mpRes, user);
    }
    return NULL;
}

/* 025DD780 */
void* fpcCtIt_JudgeInLayer(u32 i_layerID, u32 i_judgeFunc, void* i_data) {
    gabi::Local<fpcCtIt_jilprm_c> data;
    data.get()->mLayerID = i_layerID;
    data.get()->mFunc.v = i_judgeFunc;
    data.get()->mpUserData = i_data;
    return fpcCtIt_Judge(gabi::guest_fn((const void*)fpcCtIt_filter_JudgeInLayer), data.get());
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 025DD7B8 */
static void __sinit_f_pc_create_iter_cpp() {
    sinit_header_statics_rodata(0x1048A6BC, 0x101F3938, 0x1048A6C8, 0x10057E74);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data g_fpcCtTg_Queue 101F39A4
ext 020100A0 BOOL cLsIt_Method(node_list_class*, u32, method_filter*)
ext 020100BC void* cLsIt_Judge(node_list_class*, u32, node_judge_data*)
*/

/** Layer-local iteration and process lookup, in tww style.
 * Verified WWHD unit f_pc_layer_iter. */
#include "f_pc/f_pc_layer_iter.h"

/* 025DED70 */
BOOL fpcLyIt_OnlyHere(layer_class* i_layer, u32 i_func, void* i_data) {
    gabi::Local<method_filter> filter;
    filter->mpMethodFunc.v = i_func;
    filter->mpUserData = i_data;
    return cTrIt_Method(&i_layer->mNodeListTree, cTgIt_MethodCall, filter.get());
}

/* 025DEDA8 */
BOOL fpcLyIt_OnlyHereLY(layer_class* i_layer, u32 i_func, void* i_data) {
    layer_class* currentLayer = fpcLy_CurrentLayer();
    fpcLy_SetCurrentLayer(i_layer);
    BOOL result = fpcLyIt_OnlyHere(i_layer, i_func, i_data);
    fpcLy_SetCurrentLayer(currentLayer);
    return result;
}

/* 025DEE20 */
void* fpcLyIt_Judge(layer_class* i_layer, u32 i_func, void* i_data) {
    gabi::Local<judge_filter> filter;
    filter->mpJudgeFunc.v = i_func;
    filter->mpUserData = i_data;
    return cTrIt_Judge(&i_layer->mNodeListTree, cTgIt_JudgeFilter, filter.get());
}

/* 025DEE58 */
void* fpcLyIt_AllJudge(u32 i_func, void* i_data) {
    gabi::Local<judge_filter> filter;
    filter->mpJudgeFunc.v = i_func;
    filter->mpUserData = i_data;
    layer_class* current = fpcLy_RootLayer();
    while (current != NULL) {
        void* result = cTrIt_Judge(&current->mNodeListTree, cTgIt_JudgeFilter, filter.get());
        if (result != NULL) return result;
        current = reinterpret_cast<layer_class*>(current->mNode.mpNextNode.get());
    }
    return NULL;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */
/* 025DEED4 */
static void __sinit_f_pc_layer_iter_cpp() {
    sinit_header_statics_rodata(0x1048A80C, 0x101F3B50, 0x1048A818, 0x10058308);
}

/* WWHD glue (tools/verify/autoglue.py): external iteration and current-layer accessors
ext 0201ABF0 s32 cTrIt_Method(node_lists_tree_class*, u32, method_filter*)
ext 0201AC60 void* cTrIt_Judge(node_lists_tree_class*, u32, judge_filter*)
ext 025DEAA8 layer_class* fpcLy_RootLayer()
ext 025DED64 layer_class* fpcLy_CurrentLayer()
ext 025DEAB4 void fpcLy_SetCurrentLayer(layer_class*)
*/

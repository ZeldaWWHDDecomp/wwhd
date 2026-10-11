/** Process-line iteration with current-layer preservation. Private decomp source. */
#include "bindings.h"
#include "f_pc/f_pc_line_iter.h"
#include "gc_api/f_pc/f_pc.h"

static node_lists_tree_class g_fpcLn_Queue;

/* 025DF548 */
static s32 fpcLnIt_MethodCall(create_tag_class* i_createTag, method_filter* i_filter) {
    // HD glue: the shared tag layout retains mpTagData as a 32-bit guest address.
    base_process_class* process = gabi::at<base_process_class>(i_createTag->mpTagData);
    layer_class* pLayer = process->mLyTg.mpLayer;
    layer_class* pCurLayer = fpcLy_CurrentLayer();
    fpcLy_SetCurrentLayer(pLayer);
    s32 ret = cTgIt_MethodCall(i_createTag, i_filter);
    fpcLy_SetCurrentLayer(pCurLayer);
    return ret;
}

/* 025DF5C0 */
void fpcLnIt_Queue(u32 i_queueFunc) {
    method_filter filter;
    // HD: the public callback is already a guest code address.
    filter.mpMethodFunc.v = i_queueFunc;
    filter.mpUserData = NULL;
    cTrIt_Method(&g_fpcLn_Queue, fpcLnIt_MethodCall, &filter);
}

/* 025DF600 */
static void __sinit_f_pc_line_iter_cpp() {
    sinit_header_statics_rodata(0x1048A93C, 0x101F3C1C, 0x1048A948, 0x10058354);
}

/* WWHD glue (tools/verify/autoglue.py)
 data g_fpcLn_Queue 101F3C14
 ext 0201A9F4 s32 cTgIt_MethodCall(create_tag_class*, method_filter*)
 ext 0201ABF0 s32 cTrIt_Method(node_lists_tree_class*, fpcLnIt_MethodFunc, method_filter*)
*/

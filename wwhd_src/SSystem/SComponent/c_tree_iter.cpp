/** Node-list tree iteration. WWHD, verified unit c_tree_iter. */
#include "SSystem/SComponent/c_tree_iter.h"

/* 0201ABF0 */
s32 cTrIt_Method(node_lists_tree_class* tree, u32 method, void* data) {
    s32 count = tree->mNumLists;
    node_list_class* pList = tree->mpLists;
    s32 ret = TRUE;
    for (s32 remaining = count; remaining > 0; --remaining) {
        s32 sub = cLsIt_Method(pList++, method, data);
        if (sub == FALSE) {
            ret = FALSE;
        }
    }
    return ret;
}

/* 0201AC60 */
void* cTrIt_Judge(node_lists_tree_class* tree, u32 judge, void* data) {
    s32 count = tree->mNumLists;
    node_list_class* pList = tree->mpLists;
    for (s32 remaining = count; remaining > 0; --remaining) {
        void* pJudgeRet = cLsIt_Judge(pList++, judge, data);
        if (pJudgeRet != NULL) {
            return pJudgeRet;
        }
    }
    return NULL;
}

/* 0201ACE4 */
// HD: header statics and destructor registrations.
static void __sinit_c_tree_iter_cpp() {
    sinit_header_statics_rodata(0x101FFB70, 0x1018D43C, 0x101FFB7C, 0x100036E0);
}

/* WWHD glue (tools/verify/autoglue.py): external list iteration
ext 020100A0 s32 cLsIt_Method(node_list_class*, u32, void*)
ext 020100BC void* cLsIt_Judge(node_list_class*, u32, void*)
*/

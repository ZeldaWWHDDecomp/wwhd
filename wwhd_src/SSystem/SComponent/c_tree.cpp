/** Node-list trees. WWHD, verified unit c_tree. */
#include "SSystem/SComponent/c_tree.h"

/* 0201AAB0 */
s32 cTr_SingleCut(node_class* pNode) {
    return cLs_SingleCut(pNode);
}

/* 0201AAB4 */
s32 cTr_Addition(node_lists_tree_class* pTree, s32 listIdx, node_class* pNode) {
    if (listIdx >= pTree->mNumLists) {
        return 0;
    }
    return cLs_Addition(&pTree->mpLists.get()[listIdx], pNode);
}

/* 0201AADC */
s32 cTr_Insert(node_lists_tree_class* pTree, s32 listIdx, node_class* pNode, s32 idx) {
    if (listIdx >= pTree->mNumLists) {
        return 0;
    }
    return cLs_Insert(&pTree->mpLists.get()[listIdx], idx, pNode);
}

/* 0201AB04 */
void cTr_Create(node_lists_tree_class* pTree, node_list_class* pLists, s32 numLists) {
    // HD: count is stored before the list pointer, preserving overlapping arguments.
    pTree->mNumLists = numLists;
    pTree->mpLists = pLists;
    for (s32 i = 0; i < numLists; ++i) {
        cLs_Create(&pLists[i]);
    }
}

/* 0201AB5C */
// HD: header statics and destructor registrations.
static void __sinit_c_tree_cpp() {
    sinit_header_statics_rodata(0x101FFB54, 0x1018D418, 0x101FFB60, 0x100036D8);
}

/* WWHD glue (tools/verify/autoglue.py): external list operations
ext 0200FDF4 s32 cLs_SingleCut(node_class*)
ext 0200FE78 s32 cLs_Addition(node_list_class*, node_class*)
ext 0200FF10 s32 cLs_Insert(node_list_class*, s32, node_class*)
ext 02010008 void cLs_Create(node_list_class*)
*/

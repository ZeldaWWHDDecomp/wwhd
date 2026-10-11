/**
 * c_tag.cpp - tagged nodes for lists and trees.
 * WWHD (cking.rpx), verified against the recompiled original (unit c_tag).
 */
#include "SSystem/SComponent/c_tag.h"

/* 0201A71C */
u32 cTg_IsUse(create_tag_class* pTag) {
    return static_cast<u8>(pTag->mbIsUse);
}

/* 0201A724 */
s32 cTg_SingleCutFromTree(create_tag_class* pTag) {
    if (pTag->mbIsUse != true) {
        return 0;
    }
    pTag->mbIsUse = false;
    cTr_SingleCut(cTg_GetNode(pTag));
    return 1;
}

/* 0201A770 */
s32 cTg_AdditionToTree(node_lists_tree_class* pTree, s32 listIdx, create_tag_class* pTag) {
    if (pTag->mbIsUse) {
        return 0;
    }
    s32 ret = cTr_Addition(pTree, listIdx, cTg_GetNode(pTag));
    if (ret) {
        pTag->mbIsUse = true;
    }
    return ret;
}

/* 0201A7D4 */
s32 cTg_InsertToTree(node_lists_tree_class* pTree, s32 listIdx, create_tag_class* pTag, s32 idx) {
    if (pTag->mbIsUse) {
        return 0;
    }
    s32 ret = cTr_Insert(pTree, listIdx, cTg_GetNode(pTag), idx);
    if (ret) {
        pTag->mbIsUse = true;
    }
    return ret;
}

/* 0201A838 */
node_class* cTg_GetFirst(node_list_class* pList) {
    node_class* pNode = cLs_GetFirst(pList);
    if (pNode != NULL) {
        create_tag_class* pTag = reinterpret_cast<create_tag_class*>(pNode);
        pTag->mbIsUse = false;
    }
    return pNode;
}

/* 0201A868 */
s32 cTg_SingleCut(create_tag_class* pTag) {
    if (pTag->mbIsUse != true) {
        return 0;
    }
    pTag->mbIsUse = false;
    cLs_SingleCut(cTg_GetNode(pTag));
    return 1;
}

/* 0201A8B4 */
s32 cTg_Addition(node_list_class* pList, create_tag_class* pTag) {
    if (pTag->mbIsUse) {
        return 0;
    }
    s32 ret = cLs_Addition(pList, cTg_GetNode(pTag));
    if (ret) {
        pTag->mbIsUse = true;
    }
    return ret;
}

/* 0201A918 */
void cTg_Create(create_tag_class* pTag, void* pData) {
    cNd_Create(cTg_GetNode(pTag), NULL);
    pTag->mpTagData = gabi::ea(pData);
    pTag->mbIsUse = false;
}

/* 0201A960 */
// HD: initialize math/header globals and register their destructors.
static void __sinit_c_tag_cpp() {
    sinit_header_statics_rodata(0x101FFB1C, 0x1018D3D0, 0x101FFB28, 0x100036C8);
}

/* WWHD glue (tools/verify/autoglue.py): external list, tree and node operations
ext 0201AAB0 s32 cTr_SingleCut(node_class*)
ext 0201AAB4 s32 cTr_Addition(node_lists_tree_class*, s32, node_class*)
ext 0201AADC s32 cTr_Insert(node_lists_tree_class*, s32, node_class*, s32)
ext 0200FDF4 s32 cLs_SingleCut(node_class*)
ext 0200FE78 s32 cLs_Addition(node_list_class*, node_class*)
ext 0200FFB0 node_class* cLs_GetFirst(node_list_class*)
ext 02019CA8 void cNd_Create(node_class*, void*)
*/

/**
 * c_list.cpp - node lists: insertion and removal.
 * WWHD (cking.rpx), verified against the recompiled original (unit c_list).
 */
#include "SSystem/SComponent/c_list.h"

/* 0200FDE0 */
void cLs_Init(node_list_class* pList) {
    pList->mpHead = NULL;
    pList->mpTail = NULL;
    pList->mSize = 0;
}

/* 0200FDF4 */
s32 cLs_SingleCut(node_class* pNode) {
    node_list_class* pList = static_cast<node_list_class*>(pNode->mpData.get());
    // HD: both endpoints are read before either endpoint can be changed.
    node_class* head = pList->mpHead;
    node_class* tail = pList->mpTail;
    if (pNode == head) {
        pList->mpHead = pNode->mpNextNode;
    }
    if (pNode == tail) {
        pList->mpTail = pNode->mpPrevNode;
    }
    cNd_SingleCut(pNode);
    cNd_ClearObject(pNode);
    s32 newSize = (s32)((u32)pList->mSize - 1u);
    pList->mSize = newSize;
    return newSize > 0;
}

/* 0200FE78 */
s32 cLs_Addition(node_list_class* pList, node_class* pNode) {
    node_class* tail = pList->mpTail;
    if (tail == NULL) {
        pList->mpHead = pNode;
    } else {
        cNd_Addition(tail, pNode);
    }
    pList->mpTail = cNd_Last(pNode);
    cNd_SetObject(pNode, pList);
    s32 size = cNd_LengthOf(pList->mpHead);
    pList->mSize = size;
    return size;
}

/* 0200FF10 */
s32 cLs_Insert(node_list_class* pList, s32 idx, node_class* pNode) {
    node_class* pExisting = cNd_Order(pList->mpHead, idx);
    if (pExisting == NULL) {
        return cLs_Addition(pList, pNode);
    }
    cNd_SetObject(pNode, pList);
    cNd_Insert(pExisting, pNode);
    node_class* head = cNd_First(pNode);
    pList->mpHead = head;
    s32 size = cNd_LengthOf(head);
    pList->mSize = size;
    return size;
}

/* 0200FFB0 */
node_class* cLs_GetFirst(node_list_class* pList) {
    if (pList->mSize != 0) {
        node_class* pHead = pList->mpHead;
        cLs_SingleCut(pHead);
        return pHead;
    }
    return NULL;
}

/* 02010008 */
void cLs_Create(node_list_class* pList) {
    cLs_Init(pList);
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 0201000C */
static void __sinit_c_list_cpp() {
    sinit_header_statics_rodata(0x101FF7D0, 0x1018C7B4, 0x101FF7DC, 0x10001ED8);
}

/* WWHD glue (tools/verify/autoglue.py): external node operations
ext 02019B48 void cNd_SingleCut(node_class*)
ext 02019C8C void cNd_ClearObject(node_class*)
ext 02019B94 void cNd_Addition(node_class*, node_class*)
ext 02019AD8 node_class* cNd_Last(node_class*)
ext 02019C6C void cNd_SetObject(node_class*, void*)
ext 02019A90 s32 cNd_LengthOf(node_class*)
ext 02019AFC node_class* cNd_Order(node_class*, s32)
ext 02019C0C void cNd_Insert(node_class*, node_class*)
ext 02019AB4 node_class* cNd_First(node_class*)
*/

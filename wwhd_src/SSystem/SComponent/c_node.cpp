/**
 * c_node.cpp
 * System component - linked node manipulation
 *
 * WWHD, written in the style of zeldaret/tww and verified function by function against the
 * recompiled original (tools/verify, unit c_node).
 */

#include "SSystem/SComponent/c_node.h"

/* inlined into cNd_Addition */
static void cNd_Join(node_class* pA, node_class* pB) {
    pA->mpNextNode = pB;
    pB->mpPrevNode = pA;
}

/* 02019A90 */
int cNd_LengthOf(node_class* pNode) {
    u32 count = 0; // HD: word arithmetic preserves wraparound.
    while (pNode) {
        pNode = pNode->mpNextNode;
        count++;
    }
    return (int)count;
}

/* 02019AB4 */
node_class* cNd_First(node_class* pNode) {
    node_class* pRet = NULL;
    while (pNode) {
        pRet = pNode;
        pNode = pNode->mpPrevNode;
    }
    return pRet;
}

/* 02019AD8 */
node_class* cNd_Last(node_class* pNode) {
    node_class* pRet = NULL;
    while (pNode) {
        pRet = pNode;
        pNode = pNode->mpNextNode;
    }
    return pRet;
}

/* 02019AFC */
node_class* cNd_Order(node_class* pNode, int idx) {
    node_class* pRet = NULL;
    int i = 0;
    while (i < idx && pNode) {
        pRet = pNode;
        i++;
        pNode = pNode->mpNextNode;
    }
    if (i < idx) {
        return pRet;
    }
    return NULL;
}

/* 02019B48 */
void cNd_SingleCut(node_class* pNode) {
    node_class* pPrev = pNode->mpPrevNode;
    node_class* pNext = pNode->mpNextNode;
    // HD: both links are cached before either neighbouring node is changed.
    if (pPrev) {
        pPrev->mpNextNode = pNext;
    }
    if (pNext) {
        pNext->mpPrevNode = pPrev;
    }
    pNode->mpPrevNode = NULL;
    pNode->mpNextNode = NULL;
}

/* 02019B78 */
void cNd_Cut(node_class* pNode) {
    node_class* pPrev = pNode->mpPrevNode;
    if (pPrev) {
        pPrev->mpNextNode = NULL;
    }
    pNode->mpPrevNode = NULL;
}

/* 02019B94 */
void cNd_Addition(node_class* pA, node_class* pB) {
    JUT_ASSERT(219, pA != NULL && pB != NULL); // HD: reject missing lists.
    if (pA && pB) {
        node_class* pLast = cNd_Last(pA);
        cNd_Join(pLast, pB);
    }
}

/* 02019C0C */
void cNd_Insert(node_class* pA, node_class* pB) {
    node_class* pPrev = pA->mpPrevNode;
    if (pPrev) {
        cNd_Cut(pA);
        cNd_Addition(pPrev, pB);
    }
    cNd_Addition(pB, pA);
}

/* 02019C6C */
void cNd_SetObject(node_class* pNode, void* pData) {
    while (pNode) {
        pNode->mpData = pData;
        pNode = pNode->mpNextNode;
    }
}

/* 02019C8C */
void cNd_ClearObject(node_class* pNode) {
    cNd_SetObject(pNode, NULL);
}

/* 02019C94 */
void cNd_ForcedClear(node_class* pNode) {
    pNode->mpPrevNode = NULL;
    pNode->mpNextNode = NULL;
    pNode->mpData = NULL;
}

/* 02019CA8 */
void cNd_Create(node_class* pNode, void* pData) {
    // HD: write the data before clearing the links.
    pNode->mpData = pData;
    pNode->mpPrevNode = NULL;
    pNode->mpNextNode = NULL;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */

/* 02019CBC */
static void __sinit_c_node_cpp() {
    sinit_header_statics_rodata(0x101FFA18, 0x1018D28C, 0x101FFA24, 0x10003640);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
str 10003608
str 10003614 "pA != NULL && pB != NULL" renamed # HD: "!((i_Dst) == (0)) && !((i_Src) == (0))"
*/

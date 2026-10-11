/* Linked nodes, WWHD layout (unchanged from GameCube). */
#pragma once
#include "gc_api/JSystem/JUTAssert.h"

struct node_class {
    /* 0x00 */ gptr<node_class> mpPrevNode;
    /* 0x04 */ gptr<void> mpData;
    /* 0x08 */ gptr<node_class> mpNextNode;
};
WWHD_OFFSET(node_class, mpPrevNode, 0x00);
WWHD_OFFSET(node_class, mpData, 0x04);
WWHD_OFFSET(node_class, mpNextNode, 0x08);
WWHD_SIZE(node_class, 0x0C);

int cNd_LengthOf(node_class* pNode);
node_class* cNd_First(node_class* pNode);
node_class* cNd_Last(node_class* pNode);
node_class* cNd_Order(node_class* pNode, int idx);
void cNd_SingleCut(node_class* pNode);
void cNd_Cut(node_class* pNode);
void cNd_Addition(node_class* pA, node_class* pB);
void cNd_Insert(node_class* pA, node_class* pB);
void cNd_SetObject(node_class* pNode, void* pData);
void cNd_ClearObject(node_class* pNode);
void cNd_ForcedClear(node_class* pNode);
void cNd_Create(node_class* pNode, void* pData);

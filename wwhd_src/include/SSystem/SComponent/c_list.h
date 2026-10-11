/* c_list.h, WWHD node lists. GameCube layout unchanged (0xC bytes). */
#pragma once
#include "SSystem/SComponent/c_node_iter.h"

struct node_list_class {
    /* 0x00 */ gptr<node_class> mpHead;
    /* 0x04 */ gptr<node_class> mpTail;
    /* 0x08 */ be<s32> mSize;
};
WWHD_OFFSET(node_list_class, mpHead, 0);
WWHD_OFFSET(node_list_class, mpTail, 4);
WWHD_OFFSET(node_list_class, mSize, 8);
WWHD_SIZE(node_list_class, 0xC);

void cLs_Init(node_list_class*);
s32 cLs_SingleCut(node_class*);
s32 cLs_Addition(node_list_class*, node_class*);
s32 cLs_Insert(node_list_class*, s32, node_class*);
node_class* cLs_GetFirst(node_list_class*);
void cLs_Create(node_list_class*);

void cNd_SingleCut(node_class*);
void cNd_ClearObject(node_class*);
void cNd_Addition(node_class*, node_class*);
node_class* cNd_Last(node_class*);
void cNd_SetObject(node_class*, void*);
s32 cNd_LengthOf(node_class*);
node_class* cNd_Order(node_class*, s32);
void cNd_Insert(node_class*, node_class*);
node_class* cNd_First(node_class*);

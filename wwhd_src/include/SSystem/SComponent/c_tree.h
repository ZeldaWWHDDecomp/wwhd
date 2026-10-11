/* Node-list trees. WWHD layout unchanged. */
#pragma once
#include "SSystem/SComponent/c_list.h"

struct node_lists_tree_class {
    /* 0x00 */ gptr<node_list_class> mpLists;
    /* 0x04 */ be<s32> mNumLists;
};
WWHD_OFFSET(node_lists_tree_class, mpLists, 0);
WWHD_OFFSET(node_lists_tree_class, mNumLists, 4);
WWHD_SIZE(node_lists_tree_class, 8);
s32 cTr_SingleCut(node_class*);
s32 cTr_Addition(node_lists_tree_class*, s32, node_class*);
s32 cTr_Insert(node_lists_tree_class*, s32, node_class*, s32);
void cTr_Create(node_lists_tree_class*, node_list_class*, s32);

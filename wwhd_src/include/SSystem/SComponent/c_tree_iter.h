/* Node-list tree iteration. */
#pragma once
#include "SSystem/SComponent/c_tree.h"
// HD: callbacks remain guest code addresses at the public boundary.
s32 cTrIt_Method(node_lists_tree_class*, u32, void*);
void* cTrIt_Judge(node_lists_tree_class*, u32, void*);
s32 cLsIt_Method(node_list_class*, u32, void*);
void* cLsIt_Judge(node_list_class*, u32, void*);

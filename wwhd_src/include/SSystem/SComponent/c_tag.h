/* c_tag.h - tagged nodes. */
#pragma once
#include "SSystem/SComponent/c_node_iter.h"
#include "SSystem/SComponent/c_tree.h" /* cTr_* and (through c_list.h) cLs_*: one declaration each */
#include "f_pc/f_pc_base.h"

struct node_list_class;
struct node_lists_tree_class;

// The shared tag layout stores the GameCube mpNode member as bytes.
inline node_class* cTg_GetNode(create_tag_class* pTag) {
    return reinterpret_cast<node_class*>(pTag->mNode);
}

u32 cTg_IsUse(create_tag_class*);
s32 cTg_SingleCutFromTree(create_tag_class*);
s32 cTg_AdditionToTree(node_lists_tree_class*, s32, create_tag_class*);
s32 cTg_InsertToTree(node_lists_tree_class*, s32, create_tag_class*, s32);
node_class* cTg_GetFirst(node_list_class*);
s32 cTg_SingleCut(create_tag_class*);
s32 cTg_Addition(node_list_class*, create_tag_class*);
void cTg_Create(create_tag_class*, void*);

void cNd_Create(node_class*, void*);

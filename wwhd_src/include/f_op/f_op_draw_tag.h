/* Draw queue tags. */
#pragma once
#include "bindings.h"
#include "SSystem/SComponent/c_tree.h"
#include "f_pc/f_pc_base.h"

// HD: this entry point forwards the insertion result, although GameCube declares void.
u32 fopDwTg_ToDrawQ(create_tag_class*, s32);
void fopDwTg_DrawQTo(create_tag_class*);
bool fopDwTg_Init(create_tag_class*, void*);
void fopDwTg_CreateQueue();
u32 cTg_AdditionToTree(node_lists_tree_class*, s32, create_tag_class*);
void cTg_SingleCutFromTree(create_tag_class*);
void cTg_Create(create_tag_class*, void*);

struct draw_list_table {
    node_list_class lists[1000];
};
WWHD_SIZE(draw_list_table, 12000);

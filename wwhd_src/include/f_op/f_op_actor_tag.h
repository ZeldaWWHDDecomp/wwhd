/* Actor queue tags: tww names and unchanged tagged-node layout. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_base.h"
#include "SSystem/SComponent/c_list.h"

u32 fopAcTg_ToActorQ(create_tag_class*);
u32 fopAcTg_ActorQTo(create_tag_class*);
u32 fopAcTg_Init(create_tag_class*, void*);

s32 cTg_Addition(node_list_class*, create_tag_class*);
s32 cTg_SingleCutFromTree(create_tag_class*);
void cTg_Create(create_tag_class*, void*);

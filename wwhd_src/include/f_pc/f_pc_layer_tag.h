/* Layer tags: GameCube names, unchanged 0x1C-byte WWHD layout.
 * HD: list and priority inputs retain all 32 bits until stored as halfwords. */
#pragma once
#include "f_pc/f_pc_layer_iter.h"
enum : u32 { fpcLy_ROOT_e=0, fpcLy_CURRENT_e=0xFFFFFFFD, fpcLy_SPECIAL_e=0xFFFFFFFE, fpcLy_NONE_e=0xFFFFFFFF };
WWHD_OFFSET(layer_management_tag_class, mpLayer, 0x14);
WWHD_OFFSET(layer_management_tag_class, mNodeListID, 0x18);
WWHD_OFFSET(layer_management_tag_class, mNodeListIdx, 0x1A);
s32 fpcLyTg_ToQueue(layer_management_tag_class*, u32, u32, u32);
s32 fpcLyTg_QueueTo(layer_management_tag_class*);
s32 fpcLyTg_Move(layer_management_tag_class*, u32, u32, u32);
s32 fpcLyTg_Init(layer_management_tag_class*, u32, void*);
s32 fpcLy_ToQueue(layer_class*, u32, layer_management_tag_class*);
s32 fpcLy_IntoQueue(layer_class*, u32, layer_management_tag_class*, u32);
s32 fpcLy_QueueTo(layer_class*, layer_management_tag_class*);
layer_class* fpcLy_Layer(u32);
void cTg_Create(create_tag_class*, void*);

/** Process-line tags; private decomp source. */
#pragma once
#include "f_pc/f_pc_line.h"
#include "f_pc/f_pc_base.h"
struct line_tag {
    create_tag_class base;
    be<s32> mLineListID;
};
WWHD_OFFSET(line_tag, mLineListID, 0x14);
WWHD_SIZE(line_tag, 0x18);
void fpcLnTg_QueueTo(line_tag*);
s32 fpcLnTg_ToQueue(line_tag*, s32);
s32 fpcLnTg_Move(line_tag*, s32);
void fpcLnTg_Init(line_tag*, void*);
s32 cTg_SingleCutFromTree(create_tag_class*);
s32 cTg_AdditionToTree(node_lists_tree_class*, s32, create_tag_class*);
void cTg_Create(create_tag_class*, void*);

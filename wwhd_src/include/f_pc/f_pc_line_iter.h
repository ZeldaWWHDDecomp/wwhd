/** Process-line iteration; private decomp source. */
#pragma once
#include "f_pc/f_pc_line.h"
#include "SSystem/SComponent/c_tag_iter.h"
using fpcLnIt_MethodFunc = s32 (*)(create_tag_class*, method_filter*);
s32 cTrIt_Method(node_lists_tree_class*, fpcLnIt_MethodFunc, method_filter*);
void fpcLnIt_Queue(u32 queueFunc);

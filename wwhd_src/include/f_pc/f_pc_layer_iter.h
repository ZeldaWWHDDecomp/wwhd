/* Layer iteration: tww names, unchanged WWHD layer layout.
 * HD: callback arguments are guest code addresses at the public boundary. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_base.h"
#include "SSystem/SComponent/c_tree.h"

struct layer_class {
    /* 0x00 */ node_class mNode;
    /* 0x0C */ be<u32> mLayerID;
    /* 0x10 */ node_lists_tree_class mNodeListTree;
    /* 0x18 */ gptr<void> mpPcNode;
    /* 0x1C */ node_list_class mCancelList;
    struct { be<s16> mCreatingCount, mDeletingCount; } counts;
};
WWHD_OFFSET(layer_class, mLayerID, 0xC);
WWHD_OFFSET(layer_class, mNodeListTree, 0x10);
WWHD_OFFSET(layer_class, mpPcNode, 0x18);
WWHD_OFFSET(layer_class, mCancelList, 0x1C);
WWHD_OFFSET(layer_class, counts, 0x28);
WWHD_SIZE(layer_class, 0x2C);
struct method_filter { gfn mpMethodFunc; gptr<void> mpUserData; };
struct judge_filter { gfn mpJudgeFunc; gptr<void> mpUserData; };
WWHD_SIZE(method_filter, 8);
WWHD_SIZE(judge_filter, 8);
// External callback adapters belong to c_tag_iter, not this translation unit.
enum : u32 { cTgIt_MethodCall = 0x0201A9F4, cTgIt_JudgeFilter = 0x0201AA08 };
BOOL fpcLyIt_OnlyHere(layer_class*, u32, void*);
BOOL fpcLyIt_OnlyHereLY(layer_class*, u32, void*);
void* fpcLyIt_Judge(layer_class*, u32, void*);
void* fpcLyIt_AllJudge(u32, void*);
s32 cTrIt_Method(node_lists_tree_class*, u32, method_filter*);
void* cTrIt_Judge(node_lists_tree_class*, u32, judge_filter*);
layer_class* fpcLy_RootLayer();
layer_class* fpcLy_CurrentLayer();
void fpcLy_SetCurrentLayer(layer_class*);

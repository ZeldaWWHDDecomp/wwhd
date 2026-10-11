/* Process nodes. */
#pragma once
#include "f_pc/f_pc_leaf.h"
#include "SSystem/SComponent/c_tree.h"

struct process_node_class;
struct layer_class {
    /* 0x00 */ node_class mNode;
    /* 0x0C */ be<u32> mLayerID;
    /* 0x10 */ node_lists_tree_class mNodeListTree;
    /* 0x18 */ gptr<process_node_class> mpPcNode;
    /* 0x1C */ node_list_class mCancelList;
    struct {
        /* 0x28 */ be<s16> mCreatingCount;
        /* 0x2A */ be<s16> mDeletingCount;
    } counts;
};
WWHD_OFFSET(layer_class, mLayerID, 0xC);
WWHD_OFFSET(layer_class, mNodeListTree, 0x10);
WWHD_OFFSET(layer_class, mpPcNode, 0x18);
WWHD_OFFSET(layer_class, mCancelList, 0x1C);
WWHD_OFFSET(layer_class, counts, 0x28);
WWHD_SIZE(layer_class, 0x2C);

struct nodedraw_method_class {
    process_method_class base;
    gfn mpDrawFunc;
};
WWHD_OFFSET(nodedraw_method_class, mpDrawFunc, 0x10);
WWHD_SIZE(nodedraw_method_class, 0x14);

struct process_node_class {
    /* 0x000 */ base_process_class base;
    /* 0x0B8 */ be<u32> mSubType;
    /* 0x0BC */ gptr<nodedraw_method_class> mpNodeMtd;
    /* 0x0C0 */ layer_class mLayer;
    /* 0x0EC */ node_list_class mLayerNodeLists[16];
    /* 0x1AC */ be<u8> mUnk0;
    u8 padding[3];
};
// HD: the base-process vtable moves subtype and subsequent members four bytes later.
WWHD_OFFSET(process_node_class, mSubType, 0xB8);
WWHD_OFFSET(process_node_class, mpNodeMtd, 0xBC);
WWHD_OFFSET(process_node_class, mLayer, 0xC0);
WWHD_OFFSET(process_node_class, mLayerNodeLists, 0xEC);
WWHD_OFFSET(process_node_class, mUnk0, 0x1AC);
WWHD_SIZE(process_node_class, 0x1B0);

struct node_process_profile_definition {
    process_profile_definition base;
    gptr<nodedraw_method_class> sub_method;
};
WWHD_OFFSET(node_process_profile_definition, sub_method, 0x1C);
WWHD_SIZE(node_process_profile_definition, 0x20);

/* fpcBs_Is_JustOfType: f_pc/f_pc_base.h */
s32 fpcLy_IsCreatingMesg(layer_class*);
s32 fpcLy_IsDeletingMesg(layer_class*);
s32 fpcLy_Delete(layer_class*);
void fpcLy_Create(layer_class*, void*, node_list_class*, u32);
using fpcLyIt_JudgeFunc = void* (*)(void*);
void* fpcLyIt_Judge(layer_class*, fpcLyIt_JudgeFunc, void*);

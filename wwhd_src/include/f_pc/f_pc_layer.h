/* Layer management, WWHD layout (GameCube field names). */
#pragma once
#include "bindings.h"
#include "SSystem/SComponent/c_tree.h"
struct process_node_class;
struct process_method_tag_class;
struct layer_class {
    node_class mNode;
    be<u32> mLayerID;
    node_lists_tree_class mNodeListTree;
    gptr<process_node_class> mpPcNode;
    node_list_class mCancelList;
    struct { be<s16> mCreatingCount; be<s16> mDeletingCount; } counts;
};
WWHD_SIZE(layer_class, 0x2C);
WWHD_OFFSET(layer_class, mNodeListTree, 0x10);
WWHD_OFFSET(layer_class, mCancelList, 0x1C);
WWHD_OFFSET(layer_class, counts, 0x28);
enum : u32 { fpcLy_ROOT_e = 0, fpcLy_CURRENT_e = 0xFFFFFFFD };
/* HD: the verified template copy reads and writes one guest word at a time. */
struct LayerCopyWords { be<u32> words[11]; };
WWHD_SIZE(LayerCopyWords, sizeof(layer_class));
void fpcLy_CancelQTo(process_method_tag_class*);
s32 fpcLy_ToCancelQ(layer_class*, process_method_tag_class*);
bool fpcLy_CancelMethod(process_method_tag_class*);
s32 fpcLy_IntoQueue(layer_class*, s32, create_tag_class*, s32);
s32 fpcLy_ToQueue(layer_class*, s32, create_tag_class*);
s32 fpcLy_QueueTo(layer_class*, create_tag_class*);
s32 fpcLy_IsDeletingMesg(layer_class*);
void fpcLy_DeletingMesg(layer_class*);
void fpcLy_DeletedMesg(layer_class*);
s32 fpcLy_IsCreatingMesg(layer_class*);
void fpcLy_CreatingMesg(layer_class*);
void fpcLy_CreatedMesg(layer_class*);
layer_class* fpcLy_RootLayer();
layer_class* fpcLy_CurrentLayer();
void fpcLy_SetCurrentLayer(layer_class*);
layer_class* fpcLy_Layer(u32);
s32 fpcLy_Delete(layer_class*);
s32 fpcLy_Cancel(layer_class*);
void fpcLy_Create(layer_class*, void*, node_list_class*, u32);
void cNd_Create(node_class*, void*);
void fpcMtdTg_MethodQTo(process_method_tag_class*);
s32 fpcMtdTg_ToMethodQ(node_list_class*, process_method_tag_class*);
s32 fpcMtdTg_Do(process_method_tag_class*);
s32 fpcMtdIt_Method(node_list_class*, u32);
s32 cTg_InsertToTree(node_lists_tree_class*, s32, create_tag_class*, s32);
s32 cTg_AdditionToTree(node_lists_tree_class*, s32, create_tag_class*);
s32 cTg_SingleCutFromTree(create_tag_class*);
struct LayerHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct LayerPiPair { be<f32> negativePi; be<f32> positivePi; };
void LayerFirstHeaderObject_ct(u8*);
void LayerSecondHeaderObject_ct(u8*);

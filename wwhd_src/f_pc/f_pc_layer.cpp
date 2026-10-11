/** Process layers. */
#include "f_pc/f_pc_layer.h"
static const LayerCopyWords l_fpcLy_Crear;
static node_list_class l_fpcLy_LayerList;
static layer_class* l_fpcLy_CurrLayer_p;
static u32 IsInitOfLayerList = 1;
static u32 layer_id = 0;
static LayerHeaderStatics l_headerStatics;
static const LayerPiPair l_piPair;

static void fpcLy_copyTemplate(layer_class* layer) {
    // HD: retain forward word-copy order, including source/destination overlap semantics.
    LayerCopyWords* dst = reinterpret_cast<LayerCopyWords*>(layer);
    for (u32 i = 0; i < 11; ++i) {
        dst->words[i] = l_fpcLy_Crear.words[i];
    }
}

/* 025DE9E0 */
void fpcLy_CancelQTo(process_method_tag_class* i_methods) {
    fpcMtdTg_MethodQTo(i_methods);
}
/* 025DE9E4 */
s32 fpcLy_ToCancelQ(layer_class* i_layer, process_method_tag_class* i_methods) {
    return fpcMtdTg_ToMethodQ(&i_layer->mCancelList, i_methods);
}
/* 025DE9EC */
bool fpcLy_CancelMethod(process_method_tag_class* i_methods) {
    return fpcMtdTg_Do(i_methods) == TRUE;
}
/* 025DEA18 */
s32 fpcLy_IntoQueue(layer_class* i_layer, s32 i_treeListNo, create_tag_class* i_createTag, s32 i_no) {
    return cTg_InsertToTree(&i_layer->mNodeListTree, i_treeListNo, i_createTag, i_no);
}
/* 025DEA20 */
s32 fpcLy_ToQueue(layer_class* i_layer, s32 i_treeListNo, create_tag_class* i_createTag) {
    return cTg_AdditionToTree(&i_layer->mNodeListTree, i_treeListNo, i_createTag);
}
/* 025DEA28 */
s32 fpcLy_QueueTo(layer_class* i_layer, create_tag_class* i_createTag) {
    return cTg_SingleCutFromTree(i_createTag);
}
/* 025DEA30 */
s32 fpcLy_IsDeletingMesg(layer_class* i_layer) {
    return i_layer->counts.mDeletingCount > 0;
}
/* 025DEA44 */
void fpcLy_DeletingMesg(layer_class* i_layer) {
    // HD: increment wraps in the guest's 16-bit counter storage.
    i_layer->counts.mDeletingCount = (s16)u16(u16(i_layer->counts.mDeletingCount) + 1);
}
/* 025DEA54 */
void fpcLy_DeletedMesg(layer_class* i_layer) {
    s16 count = i_layer->counts.mDeletingCount;
    if (count > 0) {
        i_layer->counts.mDeletingCount = count - 1;
    }
}
/* 025DEA6C */
s32 fpcLy_IsCreatingMesg(layer_class* i_layer) {
    return i_layer->counts.mCreatingCount > 0;
}
/* 025DEA80 */
void fpcLy_CreatingMesg(layer_class* i_layer) {
    // HD: increment wraps in the guest's 16-bit counter storage.
    i_layer->counts.mCreatingCount = (s16)u16(u16(i_layer->counts.mCreatingCount) + 1);
}
/* 025DEA90 */
void fpcLy_CreatedMesg(layer_class* i_layer) {
    s16 count = i_layer->counts.mCreatingCount;
    if (count > 0) {
        i_layer->counts.mCreatingCount = count - 1;
    }
}
/* 025DEAA8 */
layer_class* fpcLy_RootLayer() {
    return (layer_class*)l_fpcLy_LayerList.mpHead.get();
}
/* 025DEAB4 */
void fpcLy_SetCurrentLayer(layer_class* i_layer) {
    l_fpcLy_CurrLayer_p = i_layer;
}
/* 025DEAC0 */
layer_class* fpcLy_Layer(u32 i_layerID) {
    if (i_layerID == fpcLy_ROOT_e) {
        return fpcLy_RootLayer();
    }
    layer_class* layer = fpcLy_RootLayer();
    if (layer->mLayerID == i_layerID) {
        return layer;
    }
    layer_class* current = l_fpcLy_CurrLayer_p;
    if (i_layerID == fpcLy_CURRENT_e || current->mLayerID == i_layerID) {
        return current;
    }
    while (layer) {
        if (layer->mLayerID == i_layerID) {
            break;
        }
        layer = (layer_class*)layer->mNode.mpNextNode.get();
    }
    return layer;
}
/* 025DEB58 */
s32 fpcLy_Delete(layer_class* i_layer) {
    node_list_class* lists = i_layer->mNodeListTree.mpLists;
    if (lists->mSize != 0 || i_layer->mCancelList.mSize != 0) {
        return FALSE;
    }
    cLs_SingleCut(&i_layer->mNode);
    fpcLy_copyTemplate(i_layer);
    return TRUE;
}
/* 025DEBE0 */
s32 fpcLy_Cancel(layer_class* i_layer) {
    return fpcMtdIt_Method(&i_layer->mCancelList, gabi::guest_fn((const void*)fpcLy_CancelMethod));
}
/* 025DEBF0 */
void fpcLy_Create(layer_class* i_layer, void* i_node, node_list_class* i_nodeList, u32 i_numLists) {
    fpcLy_copyTemplate(i_layer);
    cNd_Create(&i_layer->mNode, NULL);
    u32 id = layer_id;
    layer_id = id + 1;
    i_layer->mLayerID = id;
    i_layer->mpPcNode = (process_node_class*)i_node;
    if (IsInitOfLayerList == 1) {
        IsInitOfLayerList = 0;
        cLs_Create(&l_fpcLy_LayerList);
        fpcLy_SetCurrentLayer(i_layer);
    }
    i_layer->mNodeListTree.mpLists = i_nodeList;
    i_layer->mNodeListTree.mNumLists = (s32)i_numLists;
    cTr_Create(&i_layer->mNodeListTree, i_nodeList, (s32)i_numLists);
    cLs_Addition(&l_fpcLy_LayerList, &i_layer->mNode);
}
/* 025DECD0 */
static void __sinit_f_pc_layer_cpp() {
    // HD: preserve this translation unit's header objects and registration order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3B2C);
    l_headerStatics.negativePi = l_piPair.negativePi;
    l_headerStatics.positivePi = l_piPair.positivePi;
    LayerFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3B38);
    LayerSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3B44);
}
/* 025DED64 */
layer_class* fpcLy_CurrentLayer() {
    return l_fpcLy_CurrLayer_p;
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data l_fpcLy_Crear 101F3AEC
data l_fpcLy_LayerList 101F3B18
data l_fpcLy_CurrLayer_p 101F3AE8
data IsInitOfLayerList 101F3B24
data layer_id 101F3B28
data l_headerStatics 1048A7F0
data l_piPair 100582FC
ext 025DFE28 void fpcMtdTg_MethodQTo(process_method_tag_class*)
ext 025DFE24 s32 fpcMtdTg_ToMethodQ(node_list_class*, process_method_tag_class*)
ext 025DFE14 s32 fpcMtdTg_Do(process_method_tag_class*)
ext 025DFD78 s32 fpcMtdIt_Method(node_list_class*, u32)
ext 0201A7D4 s32 cTg_InsertToTree(node_lists_tree_class*, s32, create_tag_class*, s32)
ext 0201A770 s32 cTg_AdditionToTree(node_lists_tree_class*, s32, create_tag_class*)
ext 0201A724 s32 cTg_SingleCutFromTree(create_tag_class*)
ext 0200FDF4 s32 cLs_SingleCut(node_class*)
ext 02019CA8 void cNd_Create(node_class*, void*)
ext 02010008 void cLs_Create(node_list_class*)
ext 0201AB04 void cTr_Create(node_lists_tree_class*, node_list_class*, s32)
ext 0200FE78 s32 cLs_Addition(node_list_class*, node_class*)
ext 028ED6F8 void LayerFirstHeaderObject_ct(u8*)
ext 028EAB2C void LayerSecondHeaderObject_ct(u8*)
*/

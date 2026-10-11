/** Layer management tags and queue transitions, in tww style.
 * Verified WWHD unit f_pc_layer_tag. */
#include "f_pc/f_pc_layer_tag.h"

static const u32 crear[7] = {0, 0, 0, 0, 0, 0, 0xFFFFFFFF};

/* 025DEF68 */
s32 fpcLyTg_ToQueue(layer_management_tag_class* i_lyTag, u32 i_layerID, u32 i_listID, u32 i_listPriority) {
    layer_class* layer = i_lyTag->mpLayer;
    if (i_layerID == fpcLy_NONE_e && layer == NULL) return 0;
    if (i_layerID == fpcLy_NONE_e || i_layerID == fpcLy_CURRENT_e) {
        s32 idx = fpcLy_ToQueue(layer, i_listID, i_lyTag);
        if (idx == 0) return 0;
        i_lyTag->mNodeListID = (u16)i_listID;
        i_lyTag->mNodeListIdx = (u16)((u32)idx - 1);
        return 1;
    }
    // HD: a missing stored layer is resolved before reading its ID.
    if (layer == NULL || layer->mLayerID != i_layerID) {
        layer = fpcLy_Layer(i_layerID);
        i_lyTag->mpLayer = layer;
    }
    s32 result = fpcLy_IntoQueue(layer, i_listID, i_lyTag, i_listPriority);
    if (result == 0) return 0;
    i_lyTag->mNodeListIdx = (u16)i_listPriority;
    i_lyTag->mNodeListID = (u16)i_listID;
    return 1;
}

/* 025DF084 */
s32 fpcLyTg_QueueTo(layer_management_tag_class* i_lyTag) {
    layer_class* layer = i_lyTag->mpLayer;
    if (fpcLy_QueueTo(layer, i_lyTag) != 1) return 0;
    i_lyTag->mpLayer = NULL;
    i_lyTag->mNodeListIdx = 0xFFFF;
    i_lyTag->mNodeListID = 0xFFFF;
    return 1;
}

/* 025DF0F4 */
s32 fpcLyTg_Move(layer_management_tag_class* i_lyTag, u32 i_layerID, u32 i_listID, u32 i_listPriority) {
    layer_class* layer = fpcLy_Layer(i_layerID);
    if (layer == NULL) return 0;
    if (fpcLyTg_QueueTo(i_lyTag) != 1) return 0;
    i_lyTag->mpLayer = layer;
    return fpcLyTg_ToQueue(i_lyTag, i_layerID, i_listID, i_listPriority);
}

/* 025DF178 */
s32 fpcLyTg_Init(layer_management_tag_class* i_lyTag, u32 i_layerID, void* i_data) {
    // HD: seven ordered word copies preserve the template's byte layout and overlap behavior.
    for (u32 i = 0; i < 7; ++i) {
        *gabi::at<be<u32>>(gabi::ea(i_lyTag) + i * 4) = crear[i];
    }
    cTg_Create(&i_lyTag->mCreateTag, i_data);
    layer_class* layer = fpcLy_Layer(i_layerID);
    if (layer == NULL) return 0;
    i_lyTag->mpLayer = layer;
    return 1;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */
/* 025DF224 */
static void __sinit_f_pc_layer_tag_cpp() {
    sinit_header_statics_rodata(0x1048A828, 0x101F3B90, 0x1048A834, 0x10058310);
}

/* WWHD glue (tools/verify/autoglue.py): initial tag and layer operations
data crear 101F3B74
ext 025DEA20 s32 fpcLy_ToQueue(layer_class*, u32, layer_management_tag_class*)
ext 025DEA18 s32 fpcLy_IntoQueue(layer_class*, u32, layer_management_tag_class*, u32)
ext 025DEA28 s32 fpcLy_QueueTo(layer_class*, layer_management_tag_class*)
ext 025DEAC0 layer_class* fpcLy_Layer(u32)
ext 0201A918 void cTg_Create(create_tag_class*, void*)
*/

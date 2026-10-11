/** Process nodes. WWHD, verified unit f_pc_node. */
#include "f_pc/f_pc_node.h"

static s32 g_fpcNd_type;
static s32 g_fpcNd_IsCheckOfDeleteTiming;

/* 025DFFA8 */
static s32 fpcNd_DrawMethod(nodedraw_method_class* i_nodeMethods, void* i_data) {
    return fpcMtd_Method(i_nodeMethods->mpDrawFunc.get(), i_data);
}

/* 025DFFB0 */
static s32 fpcNd_Draw(process_node_class* i_procNode) {
    s32 ret = FALSE;
    if (i_procNode->mUnk0 == 0) {
        layer_class* curLayer_p = fpcLy_CurrentLayer();
        fpcLy_SetCurrentLayer(&i_procNode->mLayer);
        ret = fpcNd_DrawMethod(i_procNode->mpNodeMtd, i_procNode);
        fpcLy_SetCurrentLayer(curLayer_p);
    }
    return ret;
}

/* 025E0024 */
static s32 fpcNd_Execute(process_node_class* i_procNode) {
    layer_class* curLayer_p = fpcLy_CurrentLayer();
    fpcLy_SetCurrentLayer(&i_procNode->mLayer);
    s32 ret = fpcMtd_Execute(&i_procNode->mpNodeMtd->base, i_procNode);
    fpcLy_SetCurrentLayer(curLayer_p);
    return ret;
}

/* 025E0080 */
static void* fpcNd_IsCreatingFromUnder(void* i_procNode) {
    process_node_class* pProcNode = static_cast<process_node_class*>(i_procNode);
    if (pProcNode == NULL) {
        return NULL;
    }
    u32 subType = pProcNode->mSubType;
    if (fpcBs_Is_JustOfType(g_fpcNd_type, subType) == FALSE) {
        return NULL;
    }
    layer_class* layer = &pProcNode->mLayer;
    if (fpcLy_IsCreatingMesg(layer) == 0) {
        return fpcLyIt_Judge(layer, fpcNd_IsCreatingFromUnder, NULL);
    }
    return pProcNode;
}

/* 025E012C */
static s32 fpcNd_IsDeleteTiming(process_node_class* i_procNode) {
    if (g_fpcNd_IsCheckOfDeleteTiming == 1 && fpcNd_IsCreatingFromUnder(i_procNode) != NULL) {
        return FALSE;
    }
    return TRUE;
}

/* 025E017C */
static s32 fpcNd_IsDelete(process_node_class* i_procNode) {
    return fpcMtd_IsDelete(&i_procNode->mpNodeMtd->base, i_procNode);
}

/* 025E0188 */
static s32 fpcNd_Delete(process_node_class* i_procNode) {
    if (fpcLy_IsDeletingMesg(&i_procNode->mLayer) != 0) {
        return FALSE;
    }
    if (fpcMtd_Delete(&i_procNode->mpNodeMtd->base, i_procNode) != 1) {
        return FALSE;
    }
    i_procNode->mSubType = 0;
    return fpcLy_Delete(&i_procNode->mLayer);
}

/* 025E01FC */
static s32 fpcNd_Create(process_node_class* i_procNode) {
    process_node_class* pProcNode = i_procNode;
    if (pProcNode->base.mInitState == 0) {
        node_process_profile_definition* pProcProfileDef =
            reinterpret_cast<node_process_profile_definition*>(pProcNode->base.mpProf.get());
        pProcNode->mSubType = fpcBs_MakeOfType(reinterpret_cast<be<s32>*>(&g_fpcNd_type));
        pProcNode->mpNodeMtd = pProcProfileDef->sub_method;
        fpcLy_Create(&pProcNode->mLayer, pProcNode, pProcNode->mLayerNodeLists, 16);
        pProcNode->mUnk0 = 0;
    }
    layer_class* curLayer_p = fpcLy_CurrentLayer();
    fpcLy_SetCurrentLayer(&pProcNode->mLayer);
    s32 ret = fpcMtd_Create(&pProcNode->mpNodeMtd->base, pProcNode);
    fpcLy_SetCurrentLayer(curLayer_p);
    return ret;
}

/* 025E029C */
// HD: this translation unit's header statics and destructor registrations.
static void __sinit_f_pc_node_cpp() {
    sinit_header_statics(0x1048AAD0, 0x101F3D3C);
}

/* WWHD glue (tools/verify/autoglue.py): process-type state and external layer operations
data g_fpcNd_type 101F3D60
data g_fpcNd_IsCheckOfDeleteTiming 101F3D64
ext 025DFCAC BOOL fpcMtd_Method(u32, void*)
ext 025DD258 BOOL fpcBs_Is_JustOfType(int, int)
ext 025DD268 s32 fpcBs_MakeOfType(be<s32>*)
ext 025DEA6C s32 fpcLy_IsCreatingMesg(layer_class*)
ext 025DEA30 s32 fpcLy_IsDeletingMesg(layer_class*)
ext 025DEB58 s32 fpcLy_Delete(layer_class*)
ext 025DEBF0 void fpcLy_Create(layer_class*, void*, node_list_class*, u32)
ext 025DEE20 void* fpcLyIt_Judge(layer_class*, fpcLyIt_JudgeFunc, void*)
*/

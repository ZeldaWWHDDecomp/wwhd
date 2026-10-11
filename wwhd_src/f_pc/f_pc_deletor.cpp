/** Process deletion scheduler. */
#include "f_pc/f_pc_deletor.h"

static node_list_class g_fpcDtTg_Queue;
static s32 g_fpcNd_type;
static DeletorHeaderStatics l_headerStatics;

/* 025DDFA4 */
BOOL fpcDt_IsComplete() {
    return fpcDtTg_IsEmpty();
}

/* 025DDFA8 */
static s32 fpcDt_deleteMethod(base_process_class* i_proc) {
    layer_class* layer = fpcDt_getDeleteTag(i_proc)->mpLayer;
    fpcLy_SetCurrentLayer(layer);
    fpcLnTg_QueueTo(reinterpret_cast<create_tag_class*>(i_proc->mLnTg));
    if (fpcBs_Delete(i_proc) == 1) {
        fpcLy_DeletedMesg(layer);
        // HD: no GameCube profile-unload call follows deletion.
        return 1;
    }
    return 0;
}

/* 025DE024 */
BOOL fpcDt_Handler() {
    // HD: forward the iterator's result register.
    return cLsIt_Method(&g_fpcDtTg_Queue, fpcDtTg_Do_callback, fpcDt_deleteMethod);
}

/* 025DE040 */
BOOL fpcDt_ToDeleteQ(base_process_class* i_proc) {
    if (i_proc->mUnk0 == 1) {
        return FALSE;
    }
    if (cTg_IsUse(&fpcDt_getDeleteTag(i_proc)->base) != 0) {
        return TRUE;
    }
    process_node_class* procNode = reinterpret_cast<process_node_class*>(i_proc);
    if (fpcBs_Is_JustOfType(g_fpcNd_type, procNode->mSubType) != 0) {
        if (!fpcNd_IsDeleteTiming(procNode)) {
            return FALSE;
        }
        layer_class* layer = &procNode->mLayer;
        if (!fpcLy_Cancel(layer)) {
            JUTAssertion::showAssert("f_pc_deletor.cpp", 196, "0");
        }
        if (!fpcLyIt_OnlyHereLY(layer, fpcDt_ToDeleteQ, 0)) {
            return FALSE;
        }
    }
    // ToQueue was inlined here. Keep the second state check after cancellation calls.
    if (i_proc->mUnk0 == 1 || fpcBs_IsDelete(i_proc) != TRUE) {
        return FALSE;
    }
    if (fpcPi_IsInQueue(fpcDt_getPriority(i_proc)) == 1) {
        fpcPi_Delete(fpcDt_getPriority(i_proc));
    }
    fpcDt_getDeleteTag(i_proc)->mpLayer = i_proc->mLyTg.mpLayer;
    fpcDtTg_ToDeleteQ(fpcDt_getDeleteTag(i_proc));
    fpcLy_DeletingMesg(i_proc->mLyTg.mpLayer);
    if (fpcEx_IsExist(i_proc->mBsPcId) == TRUE) {
        if (!fpcEx_ExecuteQTo(i_proc)) {
            return FALSE;
        }
    } else if (!fpcCt_Abort(i_proc)) {
        return FALSE;
    }
    i_proc->mInitState = fpcDt_DELETE_STATE;
    return TRUE;
}

/* 025DE1B8 */
BOOL fpcDt_Delete(base_process_class* proc) {
    if (!proc) {
        return TRUE;
    }
    if (fpcCtRq_IsDoing(proc->mpCtRq) == TRUE || proc->mInitState == fpcDt_DELETE_STATE) {
        return FALSE;
    }
    return fpcDt_RequestDelete(proc);
}

// HD: compiler-generated and template code of this translation unit.
/* 025DE238 */
static void __sinit_f_pc_deletor_cpp() {
    for (int i = 0; i < 4; ++i) {
        l_headerStatics.zero[i] = 0;
    }
    __register_global_object(0x101F3A28);
    l_headerStatics.negativePi = -3.1415927f;
    l_headerStatics.positivePi = 3.1415927f;
    DeletorFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3A34);
    DeletorSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3A40);
}

/* WWHD glue (tools/verify/autoglue.py): translation-unit data and other-unit calls
 data l_headerStatics 1048A764
 data g_fpcDtTg_Queue 101F3A1C
 data g_fpcNd_type 101F3D60
 str 10058288 "f_pc_deletor.cpp"
 str 10058284 "0"
 ext 025DDE18 BOOL fpcDtTg_IsEmpty()
 ext 025DF694 void fpcLnTg_QueueTo(create_tag_class*)
 ext 025DD3B0 BOOL fpcBs_Delete(base_process_class*)
 ext 025DEA54 void fpcLy_DeletedMesg(layer_class*)
 ext 020100A0 s32 cLsIt_Method(node_list_class*, u32, DeletorMethod)
 ext 0201A71C s32 cTg_IsUse(create_tag_class*)
 ext 025DD258 BOOL fpcBs_Is_JustOfType(int, int)
 ext 025E012C BOOL fpcNd_IsDeleteTiming(process_node_class*)
 ext 025DEBE0 BOOL fpcLy_Cancel(layer_class*)
 ext 025DEDA8 BOOL fpcLyIt_OnlyHereLY(layer_class*, DeletorLayerMethod, u32)
 ext 025DD354 BOOL fpcBs_IsDelete(base_process_class*)
 ext 025E0CBC BOOL fpcPi_IsInQueue(process_priority_class*)
 ext 025E0CC0 void fpcPi_Delete(process_priority_class*)
 ext 025DDE2C void fpcDtTg_ToDeleteQ(delete_tag_class*)
 ext 025DEA44 void fpcLy_DeletingMesg(layer_class*)
 ext 025DE564 BOOL fpcEx_IsExist(u32)
 ext 025DE6C4 BOOL fpcEx_ExecuteQTo(base_process_class*)
 ext 025DDCE4 BOOL fpcCt_Abort(base_process_class*)
 ext 025DD9E4 BOOL fpcCtRq_IsDoing(u32)
 ext 025DE040 BOOL fpcDt_RequestDelete(base_process_class*)
 ext 028ED6F8 void DeletorFirstHeaderObject_ct(u8*)
 ext 028EAB2C void DeletorSecondHeaderObject_ct(u8*)
*/

/** Process execution queues. */
#include "f_pc/f_pc_executor.h"
static s32 g_fpcNd_type;
static ExecutorHeaderStatics l_headerStatics;
static const ExecutorPiPair l_piPair;

/* 025DE508 */
base_process_class* fpcEx_Search(u32 i_judgeFunc, void* i_data) {
    return (base_process_class*)fpcLyIt_AllJudge(i_judgeFunc, i_data);
}

/* 025DE50C */
base_process_class* fpcEx_SearchByID(u32 i_id) {
    // HD: retain the guest key write even for the two rejected process IDs.
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::Local<be<u32>> key;
    *key = i_id;
    if (i_id == 0xFFFFFFFE || i_id == fpcM_ERROR_PROCESS_ID_e) {
        return NULL;
    }
    return fpcEx_Search(fpcSch_JudgeByID, key.get());
}

/* 025DE564 */
BOOL fpcEx_IsExist(u32 i_id) {
    return fpcEx_SearchByID(i_id) != NULL;
}

/* 025DE58C */
BOOL fpcEx_Execute(base_process_class* i_proc) {
    if (i_proc->mInitState != 2 || fpcPause_IsEnable(i_proc, 1) == 1) {
        return FALSE;
    }
    return fpcBs_Execute(i_proc);
}

/* 025DE5F4 */
s32 fpcEx_ToLineQ(base_process_class* i_proc) {
    layer_class* pLayer = i_proc->mLyTg.mpLayer;
    process_node_class* pLayerPcNode = pLayer->mpPcNode;
    if (pLayer->mLayerID != 0 && cTg_IsUse(fpcEx_getLineTag(&pLayerPcNode->base)) != TRUE) {
        return 0;
    }
    u16 listID = fpcEx_getPriority(i_proc)->mInfoCurr.mListID;
    if (fpcLnTg_ToQueue(fpcEx_getLineTag(i_proc), listID) == 0) {
        fpcLyTg_QueueTo(&i_proc->mLyTg);
        return 0;
    }
    // HD: read subtype before changing initialization state.
    u32 subtype = ((process_node_class*)i_proc)->mSubType;
    i_proc->mInitState = 2;
    if (fpcBs_Is_JustOfType(g_fpcNd_type, subtype) != 0) {
        process_node_class* pNode = (process_node_class*)i_proc;
        fpcLyIt_OnlyHere(&pNode->mLayer, gabi::guest_fn((const void*)fpcEx_ToLineQ), i_proc);
    }
    return 1;
}

/* 025DE6C4 */
s32 fpcEx_ExecuteQTo(base_process_class* i_proc) {
    if (fpcLyTg_QueueTo(&i_proc->mLyTg) != TRUE) {
        return 0;
    }
    i_proc->mInitState = 3;
    return 1;
}

/* 025DE720 */
s32 fpcEx_ToExecuteQ(base_process_class* i_proc) {
    process_priority_queue_info* info = &fpcEx_getPriority(i_proc)->mInfoCurr;
    // HD: keep the verified list, priority, layer read order.
    u16 listID = info->mListID;
    u16 priority = info->mListPrio;
    u32 layer = info->mLayer;
    if (fpcLyTg_ToQueue(&i_proc->mLyTg, layer, listID, priority) != TRUE) {
        return 0;
    }
    fpcEx_ToLineQ(i_proc);
    return 1;
}

/* 025DE788 */
void fpcEx_Handler(u32 i_queueFunc) {
    fpcLnIt_Queue(i_queueFunc);
}

/* 025DE78C */
static void __sinit_f_pc_executor_cpp() {
    // HD: preserve header objects, constant copies and registration order.
    l_headerStatics.zero[2] = 0;
    l_headerStatics.zero[0] = 0;
    l_headerStatics.zero[3] = 0;
    l_headerStatics.zero[1] = 0;
    __register_global_object(0x101F3A94);
    l_headerStatics.negativePi = l_piPair.negativePi;
    l_headerStatics.positivePi = l_piPair.positivePi;
    ExecutorFirstHeaderObject_ct(&l_headerStatics.firstObject);
    __register_global_object(0x101F3AA0);
    ExecutorSecondHeaderObject_ct(&l_headerStatics.secondObject);
    __register_global_object(0x101F3AAC);
}

/* WWHD glue (tools/verify/autoglue.py): this translation unit's data
data g_fpcNd_type 101F3D60
data l_headerStatics 1048A7B8
data l_piPair 100582DC
ext 025DEE58 void* fpcLyIt_AllJudge(u32, void*)
ext 025E0B20 s32 fpcPause_IsEnable(base_process_class*, u32)
ext 025DD2B8 s32 fpcBs_Execute(base_process_class*)
ext 0201A71C s32 cTg_IsUse(create_tag_class*)
ext 025DF6C8 s32 fpcLnTg_ToQueue(create_tag_class*, u16)
ext 025DF084 s32 fpcLyTg_QueueTo(layer_management_tag_class*)
ext 025DEF68 s32 fpcLyTg_ToQueue(layer_management_tag_class*, u32, u16, u16)
ext 025DD258 BOOL fpcBs_Is_JustOfType(int, int)
ext 025DED70 s32 fpcLyIt_OnlyHere(layer_class*, u32, void*)
ext 025DF5C0 void fpcLnIt_Queue(u32)
ext 028ED6F8 void ExecutorFirstHeaderObject_ct(u8*)
ext 028EAB2C void ExecutorSecondHeaderObject_ct(u8*)
*/

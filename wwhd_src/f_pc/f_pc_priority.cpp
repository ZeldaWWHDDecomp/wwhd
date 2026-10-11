/**
 * Process priority changes and deferred queue handling, in tww style.
 * Verified against the original f_pc_priority specs.
 */
#include "f_pc/f_pc_priority.h"

static node_list_class l_fpcPi_Queue;
static const u32 crear[2] = {0xFFFFFFFF, 0xFFFFFFFF};

/* 025E0CBC */
s32 fpcPi_IsInQueue(process_priority_class* i_procPriority) {
    return cTg_IsUse(&i_procPriority->base);
}

/* 025E0CC0 */
s32 fpcPi_Delete(process_priority_class* i_procPriority) {
    cTg_SingleCut(&i_procPriority->base);
    fpcLy_CancelQTo(&i_procPriority->mMtdTag);
    // HD: deletion is folded into this entry; the clear words are loaded separately around stores.
    u32 layer = crear[0];
    i_procPriority->mInfoQ.mLayer = layer;
    u32 list = crear[1];
    i_procPriority->mInfoQ.mListWord = list;
    return 1;
}

/* 025E0D0C */
s32 fpcPi_IsNormal(u32 i_layer, u32 i_listID, u32 i_priority) {
    return i_layer < fpcLy_SPECIAL_e && i_listID < fpcPi_SPECIAL_e && i_priority < fpcPi_SPECIAL_e;
}

/* 025E0D38 */
s32 fpcPi_Change(process_priority_class* i_procPriority, u32 i_layer, u32 i_listID, u32 i_priority) {
    base_process_class* pProc = gabi::at<base_process_class>(i_procPriority->base.mpTagData);
    if (pProc->mInitState == 3) return 0;
    if (!fpcPi_IsNormal(i_layer, i_listID, i_priority)) return 0;
    u32 layer = i_procPriority->mInfoCurr.mLayer;
    u32 list = i_procPriority->mInfoCurr.mListWord;
    i_procPriority->mInfoQ.mLayer = layer;
    i_procPriority->mInfoQ.mListWord = list;
    bool changed = false;
    if (i_layer != fpcLy_CURRENT_e && i_procPriority->mInfoCurr.mLayer != i_layer) {
        changed = true;
        i_procPriority->mInfoQ.mLayer = i_layer;
    }
    if (i_listID != fpcPi_CURRENT_e && i_procPriority->mInfoCurr.mListID != i_listID) {
        changed = true;
        i_procPriority->mInfoQ.mListID = (u16)i_listID;
    }
    if (i_priority != fpcPi_CURRENT_e && i_procPriority->mInfoCurr.mListPrio != i_priority) {
        changed = true;
        i_procPriority->mInfoQ.mListPrio = (u16)i_priority;
    }
    u8 state = pProc->mInitState;
    if (state == 0 || state == 1) {
        layer = i_procPriority->mInfoQ.mLayer;
        list = i_procPriority->mInfoQ.mListWord;
        i_procPriority->mInfoCurr.mLayer = layer;
        i_procPriority->mInfoCurr.mListWord = list;
        return 1;
    }
    if (!changed) return 0;
    // HD: ToQueue is inlined here; preserve its layer load before the queue call.
    u32 queuedLayer = i_procPriority->mInfoQ.mLayer;
    if (!cTg_Addition(&l_fpcPi_Queue, &i_procPriority->base)) return 0;
    if (queuedLayer != fpcLy_CURRENT_e) {
        layer_class* pLayer = fpcLy_Layer(queuedLayer);
        if (!fpcLy_ToCancelQ(pLayer, &i_procPriority->mMtdTag)) {
            cTg_SingleCut(&i_procPriority->base);
            return 0;
        }
    }
    return 1;
}

/* 025E0EE4 */
s32 fpcPi_Handler() {
    for (;;) {
        create_tag_class* tag = cTg_GetFirst(&l_fpcPi_Queue);
        if (!tag) return 1;
        base_process_class* pProc = gabi::at<base_process_class>(tag->mpTagData);
        // HD: normalize the byte-array view to the wrapping guest address, including NULL.
        process_priority_class* i_procPriority = gabi::at<process_priority_class>(gabi::ea(pProc) + offsetof(base_process_class, mPi));
        fpcLy_CancelQTo(&i_procPriority->mMtdTag);
        if (!i_procPriority) return 1;
        base_process_class* owner = gabi::at<base_process_class>(i_procPriority->base.mpTagData);
        u32 layer = i_procPriority->mInfoQ.mLayer;
        void* pLineTag = owner->mLnTg;
        u16 priority = i_procPriority->mInfoQ.mListPrio;
        u16 list = i_procPriority->mInfoQ.mListID;
        if (fpcLyTg_Move(&owner->mLyTg, layer, list, priority) != 1) return 0;
        u16 oldList = i_procPriority->mInfoCurr.mListID;
        fpcLnTg_Move(pLineTag, oldList);
        u32 newLayer = i_procPriority->mInfoQ.mLayer;
        u32 newList = i_procPriority->mInfoQ.mListWord;
        i_procPriority->mInfoCurr.mLayer = newLayer;
        i_procPriority->mInfoCurr.mListWord = newList;
    }
}

/* 025E0FB0 */
s32 fpcPi_Init(process_priority_class* i_procPriority, void* pUserData, u32 i_layer, u32 i_listID, u32 i_priority) {
    if (!fpcPi_IsNormal(i_layer, i_listID, i_priority)) return 0;
    // HD: retain the interleaved halfword/word initialization order.
    i_procPriority->mInfoCurr.mListID = (u16)i_listID;
    i_procPriority->mInfoQ.mLayer = i_layer;
    i_procPriority->mInfoCurr.mLayer = i_layer;
    i_procPriority->mInfoCurr.mListPrio = (u16)i_priority;
    i_procPriority->mInfoQ.mListID = (u16)i_listID;
    i_procPriority->mInfoQ.mListPrio = (u16)i_priority;
    cTg_Create(&i_procPriority->base, pUserData);
    fpcMtdTg_Init(&i_procPriority->mMtdTag, fpcPi_Delete, i_procPriority);
    return 1;
}

/* ---------------- HD: compiler-generated code of this translation unit ---------------- */
/* 025E104C */
static void __sinit_f_pc_priority_cpp() {
    sinit_header_statics_rodata(0x1048AB24, 0x101F3E74, 0x1048AB30, 0x10058454);
}

/* WWHD glue (tools/verify/autoglue.py): queue, deletion sentinel and external methods
data l_fpcPi_Queue 1048AB40
data crear 101F3E6C
ext 0201A71C s32 cTg_IsUse(create_tag_class*)
ext 0201A868 void cTg_SingleCut(create_tag_class*)
ext 0201A8B4 s32 cTg_Addition(node_list_class*, create_tag_class*)
ext 0201A838 create_tag_class* cTg_GetFirst(node_list_class*)
ext 0201A918 void cTg_Create(create_tag_class*, void*)
ext 025DE9E0 void fpcLy_CancelQTo(process_method_tag_class*)
ext 025DEAC0 layer_class* fpcLy_Layer(u32)
ext 025DE9E4 s32 fpcLy_ToCancelQ(layer_class*, process_method_tag_class*)
ext 025DF0F4 s32 fpcLyTg_Move(layer_management_tag_class*, u32, u16, u16)
ext 025DF738 void fpcLnTg_Move(void*, u16)
ext 025DFE2C void fpcMtdTg_Init(process_method_tag_class*, process_method_tag_func, process_priority_class*)
*/

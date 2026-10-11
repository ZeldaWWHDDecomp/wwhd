/* Process executor, verified WWHD layout. */
#pragma once
#include "bindings.h"
#include "gc_api/f_pc/f_pc.h"
#include "f_pc/f_pc_node.h" /* layer_class, process_node_class: one definition */
struct process_priority_queue_info {
    be<u32> mLayer;
    be<u16> mListID;
    be<u16> mListPrio;
};
struct process_priority_class {
    u8 _00[0x38];
    process_priority_queue_info mInfoCurr;
};
WWHD_SIZE(process_priority_class, 0x40);
/* The shared base represents these embedded types as byte arrays. */
inline process_priority_class* fpcEx_getPriority(base_process_class* proc) {
    return reinterpret_cast<process_priority_class*>(proc->mPi);
}
inline create_tag_class* fpcEx_getLineTag(base_process_class* proc) {
    return reinterpret_cast<create_tag_class*>(proc->mLnTg);
}
base_process_class* fpcEx_Search(u32, void*);
base_process_class* fpcEx_SearchByID(u32);
BOOL fpcEx_IsExist(u32);
BOOL fpcEx_Execute(base_process_class*);
s32 fpcEx_ToLineQ(base_process_class*);
s32 fpcEx_ExecuteQTo(base_process_class*);
s32 fpcEx_ToExecuteQ(base_process_class*);
void fpcEx_Handler(u32);
void* fpcLyIt_AllJudge(u32, void*);
s32 fpcPause_IsEnable(base_process_class*, u32);
s32 fpcBs_Execute(base_process_class*);
s32 cTg_IsUse(create_tag_class*);
s32 fpcLnTg_ToQueue(create_tag_class*, u16);
s32 fpcLyTg_QueueTo(layer_management_tag_class*);
s32 fpcLyTg_ToQueue(layer_management_tag_class*, u32, u16, u16);
/* fpcBs_Is_JustOfType: f_pc/f_pc_base.h */
s32 fpcLyIt_OnlyHere(layer_class*, u32, void*);
void fpcLnIt_Queue(u32);
enum : u32 { fpcSch_JudgeByID = 0x025E1234 };
struct ExecutorHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 padding[2];
    be<u32> zero[4];
};
struct ExecutorPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_SIZE(ExecutorHeaderStatics, 0x1C);
void ExecutorFirstHeaderObject_ct(u8*);
void ExecutorSecondHeaderObject_ct(u8*);

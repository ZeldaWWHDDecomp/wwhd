/* Process deletion. HD uses the shared base and node layouts. */
#pragma once
#include "f_pc/f_pc_node.h"
#include "gc_api/JSystem/JUTAssert.h"
struct delete_tag_class {
    create_tag_class base;
    /* 0x14 */ gptr<layer_class> mpLayer;
    /* 0x18 */ be<s16> mTimer;
    u8 mPadding[2];
};
WWHD_OFFSET(delete_tag_class, mpLayer, 0x14);
WWHD_OFFSET(delete_tag_class, mTimer, 0x18);
WWHD_SIZE(delete_tag_class, 0x1C);
struct process_priority_class { u8 mPriorityData[0x40]; };
WWHD_SIZE(process_priority_class, 0x40);
inline delete_tag_class* fpcDt_getDeleteTag(base_process_class* i_proc) {
    return reinterpret_cast<delete_tag_class*>(i_proc->mDtTg);
}
inline process_priority_class* fpcDt_getPriority(base_process_class* i_proc) {
    return reinterpret_cast<process_priority_class*>(i_proc->mPi);
}
BOOL fpcDtTg_IsEmpty();
void fpcLnTg_QueueTo(create_tag_class*);
BOOL fpcBs_Delete(base_process_class*);
void fpcLy_DeletedMesg(layer_class*);
using DeletorMethod = s32 (*)(base_process_class*);
enum : u32 { fpcDtTg_Do_callback = 0x025DDE44 };
s32 cLsIt_Method(node_list_class*, u32, DeletorMethod);
s32 cTg_IsUse(create_tag_class*);
BOOL fpcNd_IsDeleteTiming(process_node_class*);
BOOL fpcLy_Cancel(layer_class*);
using DeletorLayerMethod = BOOL (*)(base_process_class*);
BOOL fpcLyIt_OnlyHereLY(layer_class*, DeletorLayerMethod, u32);
BOOL fpcBs_IsDelete(base_process_class*);
BOOL fpcPi_IsInQueue(process_priority_class*);
void fpcPi_Delete(process_priority_class*);
void fpcDtTg_ToDeleteQ(delete_tag_class*);
void fpcLy_DeletingMesg(layer_class*);
BOOL fpcEx_IsExist(u32);
BOOL fpcEx_ExecuteQTo(base_process_class*);
BOOL fpcCt_Abort(base_process_class*);
BOOL fpcCtRq_IsDoing(u32);
// The Delete entry branches to this address; keep that boundary observable to verification.
BOOL fpcDt_RequestDelete(base_process_class*);
enum { fpcDt_DELETE_STATE = 3 };
struct DeletorHeaderStatics {
    be<f32> negativePi;
    be<f32> positivePi;
    u8 firstObject;
    u8 secondObject;
    u8 mPadding[2];
    be<u32> zero[4];
};
struct DeletorPiPair { be<f32> negativePi; be<f32> positivePi; };
WWHD_OFFSET(DeletorHeaderStatics, zero, 0xC);
WWHD_SIZE(DeletorHeaderStatics, 0x1C);
WWHD_SIZE(DeletorPiPair, 8);
void DeletorFirstHeaderObject_ct(u8*);
void DeletorSecondHeaderObject_ct(u8*);

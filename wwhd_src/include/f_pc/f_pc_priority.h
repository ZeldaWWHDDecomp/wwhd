/* Process priorities: tww names and unchanged 0x40-byte WWHD layout.
 * HD: validator/entry arguments retain full 32-bit list and priority values. */
#pragma once
#include "bindings.h"
#include "f_pc/f_pc_base.h"
#include "SSystem/SComponent/c_list.h"

enum : u32 { fpcPi_CURRENT_e = 0xFFFD, fpcPi_SPECIAL_e = 0xFFFE };
enum : u32 { fpcLy_CURRENT_e = 0xFFFFFFFD, fpcLy_SPECIAL_e = 0xFFFFFFFE };
struct process_method_tag_class {
    /* 0x00 */ create_tag_class mCreateTag;
    /* 0x14 */ gfn mpFunc;
    /* 0x18 */ gptr<void> mpMthdData;
};
WWHD_OFFSET(process_method_tag_class, mpFunc, 0x14);
WWHD_SIZE(process_method_tag_class, 0x1C);
struct process_priority_queue_info {
    /* 0x00 */ be<u32> mLayer;
    // HD: packed access preserves the original word-copy order, including overlapping objects.
    union {
        /* 0x04 */ be<u32> mListWord;
        struct { be<u16> mListID, mListPrio; };
    };
};
WWHD_OFFSET(process_priority_queue_info, mListID, 4);
WWHD_OFFSET(process_priority_queue_info, mListPrio, 6);
WWHD_SIZE(process_priority_queue_info, 8);
struct process_priority_class {
    /* 0x00 */ create_tag_class base;
    /* 0x14 */ process_method_tag_class mMtdTag;
    /* 0x30 */ process_priority_queue_info mInfoQ;
    /* 0x38 */ process_priority_queue_info mInfoCurr;
};
WWHD_OFFSET(process_priority_class, mMtdTag, 0x14);
WWHD_OFFSET(process_priority_class, mInfoQ, 0x30);
WWHD_OFFSET(process_priority_class, mInfoCurr, 0x38);
WWHD_SIZE(process_priority_class, 0x40);

using process_method_tag_func = s32 (*)(process_priority_class*);

s32 fpcPi_Delete(process_priority_class*);
s32 fpcPi_IsNormal(u32, u32, u32);
s32 cTg_IsUse(create_tag_class*);
void cTg_SingleCut(create_tag_class*);
s32 cTg_Addition(node_list_class*, create_tag_class*);
create_tag_class* cTg_GetFirst(node_list_class*);
void cTg_Create(create_tag_class*, void*);
void fpcLy_CancelQTo(process_method_tag_class*);
layer_class* fpcLy_Layer(u32);
s32 fpcLy_ToCancelQ(layer_class*, process_method_tag_class*);
s32 fpcLyTg_Move(layer_management_tag_class*, u32, u16, u16);
void fpcLnTg_Move(void*, u16);
void fpcMtdTg_Init(process_method_tag_class*, process_method_tag_func, process_priority_class*);

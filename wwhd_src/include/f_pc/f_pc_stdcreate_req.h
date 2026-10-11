/* Standard process creation requests, GameCube names and verified HD layout. Local only. */
#pragma once
#include "f_pc/f_pc_fstcreate_req.h"
#include "SSystem/SComponent/c_phase.h"

struct standard_create_request_class {
    /* 0x00 */ create_request base;
    /* 0x48 */ request_of_phase_process_class mPhase;
    /* 0x50 */ be<s16> mProcName;
    /* 0x52 */ u8 _52[2];
    /* 0x54 */ gptr<void> mpUserData;
    /* 0x58 */ gfn mpCallBack;
    /* 0x5C */ gptr<void> mpCallBackUserData;
};
WWHD_OFFSET(standard_create_request_class, mPhase, 0x48);
WWHD_OFFSET(standard_create_request_class, mProcName, 0x50);
WWHD_OFFSET(standard_create_request_class, mpCallBackUserData, 0x5C);
WWHD_SIZE(standard_create_request_class, 0x60);

// HD: prefix view only; the node's subtype follows the base vtable.
struct StandardCreateNodePrefix {
    base_process_class base;
    /* 0xB8 */ be<s32> mSubType;
    /* 0xBC */ u8 _BC[4];
    /* 0xC0 */ u8 mLayer[1];
};
WWHD_OFFSET(StandardCreateNodePrefix, mSubType, 0xB8);
WWHD_OFFSET(StandardCreateNodePrefix, mLayer, 0xC0);
/* fpcBs_Is_JustOfType: f_pc/f_pc_base.h */
BOOL fpcLy_IsCreatingMesg(void*);

struct StandardCreatePhaseTable { cPhs__Handler mHandlers[6]; };
WWHD_SIZE(StandardCreatePhaseTable, 0x18);

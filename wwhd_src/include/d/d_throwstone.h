/* Cutscene stone actor. */
#pragma once
#include "bindings.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/SSystem/c_phase.h"

class daThrowstone_c : public fopAc_ac_c {
public:
    BOOL CreateHeap();
    cPhs_State _create();
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ Mtx34 mMtx;
    /* 0x3E8 */ be<u8> mDemoActive; // HD: latch the demo-data return value.
    u8 padding[3];
};
WWHD_OFFSET(daThrowstone_c, mPhs, 0x3AC);
WWHD_OFFSET(daThrowstone_c, mpModel, 0x3B4);
WWHD_OFFSET(daThrowstone_c, mMtx, 0x3B8);
WWHD_OFFSET(daThrowstone_c, mDemoActive, 0x3E8);
WWHD_SIZE(daThrowstone_c, 0x3EC);
enum ThrowstoneResources { dRes_INDEX_AISI_BDL_AISI_e = 3 };
enum ThrowstoneEvent { ThrowstoneEvent_Visible = 0x310 };
enum ThrowstoneDemoFlags {
    ThrowstoneDemo_Transform = 2,
    ThrowstoneDemo_Rotate = 8,
    ThrowstoneDemo_Animation = 0x20,
    ThrowstoneDemo_AnimationFrame = 0x40,
};
void throwstoneActorConstruct(daThrowstone_c*);
void throwstoneActorDestruct(daThrowstone_c*, u32);
void throwstoneDeleteStorage(void*);
u32 dDemo_setDemoData(fopAc_ac_c*, u32, u32, u32, u32, u32, u32, u32);
void throwstoneMatrixCopy(Mtx34*, Mtx34*);

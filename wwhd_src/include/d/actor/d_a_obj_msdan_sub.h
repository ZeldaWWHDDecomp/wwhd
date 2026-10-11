// Msdan staircase step, WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

namespace daObjMsdanSub {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c();
    BOOL Delete();
    enum Prm_e {
        PRM_SIZE_W = 0x1,
        PRM_SIZE_H = 0x10,
        PRM_SWSAVE_W = 0x8,
        PRM_SWSAVE_S = 0x0,
        PRM_OBJNO_W = 0x8,
        PRM_OBJNO_S = 0x8,
    };
    u32 prm_get_size();
    u32 prm_get_swSave();
    u32 prm_get_objNo();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(gptr<Mtx34>* pMtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mModel;
    /* 0x3EC */ be<s32> m2D4;
    /* 0x3F0 */ be<s32> mCurObjNo;
    /* 0x3F4 */ be<f32> m2DC;
    /* 0x3F8 */ be<f32> m2E0;
    /* 0x3FC */ be<u8> m2E4;
    u8 mPadding[3];
};
WWHD_SIZE(Act_c, 0x400);
WWHD_OFFSET(Act_c, mModel, 0x3E8);
WWHD_OFFSET(Act_c, m2E4, 0x3FC);
}  // namespace daObjMsdanSub

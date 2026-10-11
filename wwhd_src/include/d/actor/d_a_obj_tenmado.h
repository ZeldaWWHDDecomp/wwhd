// Two-leaf shutter window, WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

namespace daObjTenmado {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c();
    enum Mode_e { MODE_WAIT_e, MODE_START_e, MODE_OPENING_e, MODE_OPEN_e };
    enum Prm_e {
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 0,
        PRM_SWSAVE2_W = 8,
        PRM_SWSAVE2_S = 8,
    };
    u32 prm_get_swSave();
    u32 prm_get_swSave2();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(gptr<Mtx34>* mtx);
    BOOL Draw();

    /* 0x3E0 */ be<f32> m2C8;
    /* 0x3E4 */ be<s32> m2CC;
    /* 0x3E8 */ request_of_phase_process_class mPhase;
    /* 0x3F0 */ gptr<J3DModel> mModel1;
    /* 0x3F4 */ gptr<J3DModel> mModel2;
};
WWHD_SIZE(Act_c, 0x3F8);
WWHD_OFFSET(Act_c, m2CC, 0x3E4);
WWHD_OFFSET(Act_c, mModel2, 0x3F4);
}  // namespace daObjTenmado

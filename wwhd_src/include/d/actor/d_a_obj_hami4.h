#pragma once
#include "gc_api/gc_api.h"
struct daObjHami4_c : fopAc_ac_c {
    daObjHami4_c();
    enum : u32 { __vt = 0x10029E1C };
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 0 };
    enum State { CLOSE_STOP, DEMO_WAIT, OPEN_DEMO, OPEN_STOP };
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModels[4];
    gptr<dBgW> mdBgW[4];
    Mtx34 mBackgroundMtx[4];
    be<s32> mState;
    be<f32> mOpening;
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();
    void daObjHami4_close_stop();
    void daObjHami4_open_demo_wait();
    void daObjHami4_open_demo();
};
WWHD_OFFSET(daObjHami4_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjHami4_c, mpModels, 0x3B4);
WWHD_OFFSET(daObjHami4_c, mdBgW, 0x3C4);
WWHD_OFFSET(daObjHami4_c, mBackgroundMtx, 0x3D4);
WWHD_OFFSET(daObjHami4_c, mState, 0x494);
WWHD_SIZE(daObjHami4_c, 0x49C);
void hami4_actor_dt(void *, s32);
void hami4_orderOtherEvent(fopAc_ac_c *, const char *, u16, u16);
void hami4_seStart(u32);

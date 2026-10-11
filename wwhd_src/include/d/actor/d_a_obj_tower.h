#pragma once
#include "gc_api/gc_api.h"
struct daObjTower_c : fopAc_ac_c {
    daObjTower_c();
    enum : u32 { __vt = 0x1003165C };
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    gptr<dBgW> mpBgW;
    Mtx34 mMtx;
    be<u8> mBackgroundRegistered;
    u8 reserved[3];
};
WWHD_SIZE(daObjTower_c, 0x3F0);
WWHD_OFFSET(daObjTower_c, mMtx, 0x3BC);
WWHD_OFFSET(daObjTower_c, mBackgroundRegistered, 0x3EC);
// HD: demo translation is copied only when the translation enable bit is set.
struct tower_demo_actor_c {
    u8 padding[4];
    be<u16> mEnable;
    u8 paddingTrans[2];
    cXyz mTrans;
    enum { ENABLE_TRANS_e = 2 };
};
WWHD_OFFSET(tower_demo_actor_c, mTrans, 8);
tower_demo_actor_c *tower_getDemoActor(void *, u8);
void tower_actor_dt(void *, s32);
inline void *tower_demoManager(u32 address) {
    return gabi::at<void>(address);
}

#pragma once
#include "gc_api/gc_api.h"
struct daObjDoguuD_c : fopAc_ac_c {
    daObjDoguuD_c();
    enum : u32 { __vt = 0x100275EC };
    BOOL CreateHeap();
    void set_mtx();
    void CreateInit();
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    gptr<dBgW> mpBgW;
    Mtx34 mMtx;
    be<u8> mBgwRegistered, mUnusedParam;
    u8 padding[2];
    be<u32> mShape;
};
WWHD_OFFSET(daObjDoguuD_c, mMtx, 0x3BC);
WWHD_OFFSET(daObjDoguuD_c, mBgwRegistered, 0x3EC);
WWHD_OFFSET(daObjDoguuD_c, mShape, 0x3F0);
WWHD_SIZE(daObjDoguuD_c, 0x3F4);
struct DoguuHeaderZeros {
    be<u32> words[4];
};
struct DoguuDemoActor {
    u8 padding[4];
    be<u16> mEnable;
    u8 paddingShape[0x28 - 6];
    be<u32> mShapeId;
    enum { ENABLE_SHAPE_e = 16 };
};
WWHD_OFFSET(DoguuDemoActor, mShapeId, 0x28);
inline void *doguuDemoManager(u32 address) {
    return gabi::at<void>(address);
}
DoguuDemoActor *doguuGetDemoActor(void *, u8);
void doguuActorDt(void *, s32);
void doguuInitA(void *);
void doguuInitB(void *);

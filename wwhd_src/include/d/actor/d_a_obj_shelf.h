#pragma once
#include "bindings.h"
namespace daObjShelf {
struct Act_c : dBgS_MoveBgActor {
    request_of_phase_process_class mPhs; // 0x3E0
    gptr<J3DModel> mpModel;              // 0x3E8
    be<s32> mMode;
    be<f32> mRotSpeed;
    be<s16> mTargetAngle, mTimer, mVibY, mVibX, mVibZ;
    be<s8> mCurBounce;
    be<u8> mReturnToWait;
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void hold_event();
    void mode_wait_init();
    void mode_wait();
    void mode_vib_init();
    void mode_vib();
    void mode_rot_init();
    void mode_rot_init2();
    void mode_rot_init3();
    void mode_rot();
    void mode_fell_init();
    void mode_fell();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34**);
    BOOL Draw();
};
WWHD_SIZE(Act_c, 0x400);
WWHD_OFFSET(Act_c, mPhs, 0x3E0);
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mTargetAngle, 0x3F4);
WWHD_OFFSET(Act_c, mVibY, 0x3F8);
WWHD_OFFSET(Act_c, mVibX, 0x3FA);
WWHD_OFFSET(Act_c, mVibZ, 0x3FC);
WWHD_OFFSET(Act_c, mCurBounce, 0x3FE);
WWHD_OFFSET(Act_c, mMode, 0x3EC);
WWHD_OFFSET(Act_c, mRotSpeed, 0x3F0);
WWHD_OFFSET(Act_c, mTimer, 0x3F6);
WWHD_OFFSET(Act_c, mReturnToWait, 0x3FF);
}

#pragma once
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/m_Do/m_Do_mtx.h"
#include "gc_api/SSystem/c_math.h"
struct LeafEmitter;
struct LeafBgWorld;
struct daLlift_c : fopAc_ac_c {
    // HD: preserve full-word returns; GameCube declares several of these methods bool.
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    dCcD_Stts mStts;
    dCcD_Cyl mCyl;
    gptr<LeafBgWorld> mpBgW;
    Mtx34 mMtx;
    be<u8> m43C, m43D, m43E, m43F;
    be<s16> mTiltAngle;
    u8 m442[2];
    be<s16> mActorLifetimeFrameCount;
    u8 m446[2];
    Quaternion mCurrentRotation, mTargetRotation;
    be<u8> mbIsDescending, mbIsAscending;
    u8 m46A[2];
    Mtx34 mBGMtx;
    be<s32> mEmitterTimer;
    gptr<LeafEmitter> mEmitter1, mEmitter2, mEmitter3, mEmitter4;
    be<f32> mWaterY;
    s32 CreateHeap();
    void setMoveBGMtx();
    void set_mtx();
    void CreateInit();
    s32 _create();
    s32 _delete();
    s32 _draw();
    s32 MoveDownLift();
    void emitterCtrl();
    s32 _execute();
    s32 MoveUpLift();
    s32 checkEndDownLift();
};
WWHD_OFFSET(daLlift_c, mpModel, 0x3B4);
WWHD_OFFSET(daLlift_c, mpBgW, 0x524);
WWHD_OFFSET(daLlift_c, m43C, 0x558);
WWHD_OFFSET(daLlift_c, mCurrentRotation, 0x564);
WWHD_OFFSET(daLlift_c, mBGMtx, 0x588);
WWHD_OFFSET(daLlift_c, mWaterY, 0x5CC);
WWHD_SIZE(daLlift_c, 0x5D0);

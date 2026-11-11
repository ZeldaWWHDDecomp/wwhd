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
    be<u8> mbPlayerRiding, mbRiseWobble, mbDescendReady, mStartDelay;  /* was m43C: rideCallBack 021B3028 sets 1 when the rider is the player (fopAcM_GetName 0xA8); _execute 021B3D04 resets the tilt and lets the lift sink only while it is 0, then clears it every frame (tww the same); was m43D: MoveUpLift 021B4090 sets 1 while the lift is still below home.y + 560; rideCallBack 021B3028 then uses a random sine wobble (mActorLifetimeFrameCount << 11) as the tilt factor instead of 2, and clears it after each call (tww the same); was m43E: set to 1 by _execute 021B3D04 when mStartDelay runs out and by rideCallBack 021B3028 when the player rides; while set and not mbPlayerRiding, _execute resets the tilt and starts the descent (mbIsDescending) once the player is 270 away (tww the same); was m43F: CreateInit 021B35F4 sets 30; _execute 021B3D04 counts it down and sets mbDescendReady when it reaches 0 (tww the same) */
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
WWHD_OFFSET(daLlift_c, mbPlayerRiding, 0x558);
WWHD_OFFSET(daLlift_c, mCurrentRotation, 0x564);
WWHD_OFFSET(daLlift_c, mBGMtx, 0x588);
WWHD_OFFSET(daLlift_c, mWaterY, 0x5CC);
WWHD_SIZE(daLlift_c, 0x5D0);

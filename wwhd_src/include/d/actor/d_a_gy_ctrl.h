/* Gyorg controller, reconstructed WWHD layout. */
#pragma once
#include "f_op/f_op_actor.h"
struct daGyCtrl_c : fopAc_ac_c {
    cXyz mSpawnPositions[5];              // 3AC
    cXyz mCheckPositions[5];              // 3E8
    be<s16> mSpawnAngles[5];              // 424
    be<u8> mPathFree[5];                  // 42E
    u8 _433;
    be<f32> mDesiredRadius;               // 434
    be<s32> mSpawnIndex, mTarget, mMode;   // 438
    be<u8> mType, mCount;                 // 444
    u8 _446[2];
    be<f32> mMaxDistance;                 // 448
    be<u8> mSwitch, mLayerInitialized;    // 44C
    u8 _44E[2];
    be<s32> mRoom;                        // 450
    cXyz mCenter;                        // 454
    be<u8> mTargetInRange;                // 460
    u8 _461[3];
    be<f32> mRadius;                      // 464
    be<s16> mAngle;                       // 468
    u8 _46A[2];
    be<s32> mCreateTimer;                 // 46C
    u8 _470[4];
    be<s32> mCameraTimer, mCameraMode;     // 474
    be<u32> mChildIds[5];                 // 47C
    be<u8> mChildActive[5];               // 490
    u8 _495[3];
    u8 mLineCheck[0x6C];                  // 498
    be<u8> mPathClear;                   // 504
    u8 _505[3];
    cXyz mRadialPaths[16];                // 508
    be<u8> mAllChildrenDead;              // 5C8
    u8 _5C9[3];
    be<s32> mHideTimer;                   // 5CC
};
WWHD_OFFSET(daGyCtrl_c,mSpawnPositions,0x3AC);
WWHD_OFFSET(daGyCtrl_c,mLineCheck,0x498);
WWHD_OFFSET(daGyCtrl_c,mHideTimer,0x5CC);
WWHD_SIZE(daGyCtrl_c,0x5D0);

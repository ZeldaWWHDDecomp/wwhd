/* WWHD recovery fairy layout. */
#pragma once
#include "bindings.h"
struct Fa1MorfCallback {
    be<u32> vtable;
    be<s16> neckAngle;
    be<u16> neckJoint;
};
struct daNpc_Fa1_c : fopNpc_npc_c {
    u8 mPhase[8];                         // 7DC
    gptr<void> mpFairyMorf;               // 7E4
    Fa1MorfCallback mMorfCallback;        // 7E8
    u8 mSparkleCallback[0x14];            // 7F0
    gptr<void> mpEmitter;                // 804
    u8 mGroundCheck[0x54];                // 808
    cXyz mLightPosition;                 // 85C
    be<u16> mLightRed, mLightGreen, mLightBlue;
    u8 _86E[2];
    be<f32> mLightPower, mLightFluctuation;
    be<u32> mLightFlags;                  // 878
    be<f32> mLightMultiplier;            // 87C
    gptr<fopAc_ac_c> mpFlower;            // 880
    cXyz mLocalPosition, mMoveTarget;
    be<f32> mPlayerRadius, mGroundY;
    be<u16> mTimer;
    be<s16> mTurnAngle, mNeckAngle, mNeckStep;
    be<u8> mMode, mMoveTimer, mSubMode, mStatus;
    be<s8> mType;
    u8 _8B1[3];
    be<s16> mAreaAngle, mAreaTurn;
    be<f32> mAreaRadius, mAreaHeight;
};
WWHD_SIZE(Fa1MorfCallback, 8);
WWHD_OFFSET(daNpc_Fa1_c, mpFairyMorf, 0x7E4);
WWHD_OFFSET(daNpc_Fa1_c, mLightPosition, 0x85C);
WWHD_OFFSET(daNpc_Fa1_c, mTimer, 0x8A4);
WWHD_OFFSET(daNpc_Fa1_c, mMode, 0x8AC);
WWHD_SIZE(daNpc_Fa1_c, 0x8C0);

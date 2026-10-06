#pragma once
#include "bindings.h"

// Armos: the HD actor base adds 0x11C; actor-local subobjects keep their GC sizes.
struct am2_class : fopEn_enemy_c {
    request_of_phase_process_class mPhase;
    be<u32> mEyeJntHit, mpMorf, mReservedModel, mpBtkAnm, mpBrkAnm;
    be<u8> mType, mPrmAreaRadius, mStartsInactive, mSwitch;
    be<u8> mAction, mMode, mHitDirection, mbIsWeakBeingHit;
    be<u8> mbNotInHomeRoom, mbMadeWaterSplash, mInAbyssTimer;
    u8 _3EF[0x3FC-0x3EF];
    cXyz mEyeballPos, mNeedlePos, mWeakPos, m304, mSpawnPos, mLinChkCenter, mLinChkDest;
    be<s16> mCountDownTimers[5], mCountUpTimers[4], mTargetAngleY, mSpawnRotY;
    u8 _466[2];
    be<s32> mCurrBckIdx;
    be<f32> mCorrectionOffsetY, mAreaRadius, mPickedUpYPos, mAcchRadius;
    u8 mAcchCir[0x40], mAcch[0x1C4], mStts[0x3C];
    u8 mBodyCyl[0x130], mNeedleCyl[0x130], mEyeSph[0x12C], mWeakSph[0x12C];
    u8 mSmokeCb[0x20], mRippleCb[0x14], mEnemyIce[0x3B8];
};
WWHD_OFFSET(am2_class, mpMorf, 0x3D4);
WWHD_OFFSET(am2_class, mAction, 0x3E8);
WWHD_OFFSET(am2_class, mWeakPos, 0x414);
WWHD_OFFSET(am2_class, mCountDownTimers, 0x450);
WWHD_OFFSET(am2_class, mAcch, 0x4BC);
WWHD_OFFSET(am2_class, mBodyCyl, 0x6BC);
WWHD_OFFSET(am2_class, mRippleCb, 0xB94);
WWHD_OFFSET(am2_class, mEnemyIce, 0xBA8);
WWHD_SIZE(am2_class, 0xF60);

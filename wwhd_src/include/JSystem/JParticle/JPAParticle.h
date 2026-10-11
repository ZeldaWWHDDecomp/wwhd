/* JPABaseParticle (JSystem JParticle), WWHD layout with GameCube names.
 * HD = GameCube up to the status word (0x00..0xD0), as the verified JPA_particle unit reads it
 * (its own view: JPA_particle.h, JPAParticle_l); the HD GPU state after it is not described. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"
#include "wwhd.h"

struct JPABaseParticle {
    /* 0x00 */ u8 mLink[0x10];             /* JSULink<JPABaseParticle> */
    /* 0x10 */ cXyz mOffsetPosition;
    /* 0x1C */ cXyz mLocalPosition;
    /* 0x28 */ cXyz mGlobalPosition;
    /* 0x34 */ cXyz mVelocity;
    /* 0x40 */ cXyz mBaseVel;
    /* 0x4C */ cXyz mAccel;
    /* 0x58 */ cXyz mFieldVel;
    /* 0x64 */ cXyz mFieldAccel;
    /* 0x70 */ be<f32> mAirResist;
    /* 0x74 */ be<f32> mMoment;
    /* 0x78 */ be<f32> mCurFrame;
    /* 0x7C */ be<f32> mLifeTime;
    /* 0x80 */ be<f32> mCurNormTime;
    /* 0x84 */ be<f32> mFieldDrag;
    /* 0x88 */ be<f32> mDrag;
    /* 0x8C */ u8 mDrawParams[0xC8 - 0x8C];  /* JPADrawParams */
    /* 0xC8 */ be<u32> mpCallBack2;
    /* 0xCC */ be<u32> mStatus;
};
WWHD_OFFSET(JPABaseParticle, mVelocity, 0x34);
WWHD_OFFSET(JPABaseParticle, mCurFrame, 0x78);
WWHD_OFFSET(JPABaseParticle, mStatus, 0xCC);

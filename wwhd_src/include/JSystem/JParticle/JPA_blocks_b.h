/* JPADynamicsBlockArc archive views, WWHD. Layout and names from tww's
 * JPADynamicsBlock.h; the native vtable is represented explicitly. */
#pragma once
#include "gc_api/gc_api.h"

struct JPADynamicsBlockData {
    /* 0x00 */ be<u32> mFlags;
    /* 0x04 */ be<f32> mVolumeSweep;
    /* 0x08 */ be<f32> mVolumeMinRad;
    /* 0x0C */ be<u16> mVolumeSize;
    /* 0x0E */ be<u16> mDivNumber;
    /* 0x10 */ be<f32> mRate;
    /* 0x14 */ be<f32> mRateRndm;
    /* 0x18 */ be<u8> mRateStep;
    /* 0x19 */ u8 field_0x19;
    /* 0x1A */ be<s16> mMaxFrame;
    /* 0x1C */ be<s16> mStartFrame;
    /* 0x1E */ be<s16> mLifeTime;
    /* 0x20 */ be<f32> mLifeTimeRndm;
    /* 0x24 */ be<f32> mInitVelOmni;
    /* 0x28 */ be<f32> mInitVelAxis;
    /* 0x2C */ be<f32> mInitVelRndm;
    /* 0x30 */ be<f32> mInitVelDir;
    /* 0x34 */ be<f32> mInitVelRatio;
    /* 0x38 */ be<f32> mSpread;
    /* 0x3C */ be<f32> mAirResist;
    /* 0x40 */ be<f32> mAirResistRndm;
    /* 0x44 */ be<f32> mMoment;
    /* 0x48 */ be<f32> mMomentRndm;
    /* 0x4C */ be<f32> mAccel;
    /* 0x50 */ be<f32> mAccelRndm;
    /* 0x54 */ cXyz mEmitterScl;
    /* 0x60 */ cXyz mEmitterTrs;
    /* 0x6C */ cXyz mEmitterDir;
    /* 0x78 */ csXyz mEmitterRot;
    /* 0x7E */ u8 _7E[2];
};
WWHD_OFFSET(JPADynamicsBlockData, mVolumeSweep, 0x4);
WWHD_OFFSET(JPADynamicsBlockData, mVolumeMinRad, 0x8);
WWHD_OFFSET(JPADynamicsBlockData, mDivNumber, 0xE);
WWHD_OFFSET(JPADynamicsBlockData, mRate, 0x10);
WWHD_OFFSET(JPADynamicsBlockData, mRateRndm, 0x14);
WWHD_OFFSET(JPADynamicsBlockData, mRateStep, 0x18);
WWHD_OFFSET(JPADynamicsBlockData, mMaxFrame, 0x1A);
WWHD_OFFSET(JPADynamicsBlockData, mStartFrame, 0x1C);
WWHD_OFFSET(JPADynamicsBlockData, mLifeTime, 0x1E);
WWHD_OFFSET(JPADynamicsBlockData, mLifeTimeRndm, 0x20);
WWHD_OFFSET(JPADynamicsBlockData, mAccel, 0x4C);
WWHD_OFFSET(JPADynamicsBlockData, mAccelRndm, 0x50);
WWHD_OFFSET(JPADynamicsBlockData, mAirResist, 0x3C);
WWHD_OFFSET(JPADynamicsBlockData, mAirResistRndm, 0x40);
WWHD_OFFSET(JPADynamicsBlockData, mMoment, 0x44);
WWHD_OFFSET(JPADynamicsBlockData, mMomentRndm, 0x48);
WWHD_OFFSET(JPADynamicsBlockData, mEmitterScl, 0x54);
WWHD_OFFSET(JPADynamicsBlockData, mEmitterRot, 0x78);
WWHD_SIZE(JPADynamicsBlockData, 0x80);

class JPADynamicsBlockArc {
public:
    // HD: frame and lifetime getters return sign-extended halfwords in the full register.
    f32 getVolumeSweep();
    f32 getVolumeMinRad();
    u32 getDivNumber();
    f32 getRate();
    f32 getRateRndm();
    u32 getRateStep();
    s32 getMaxFrame();
    s32 getStartFrame();
    s32 getLifeTime();
    f32 getLifeTimeRndm();
    f32 getAccel();
    f32 getAccelRndm();
    f32 getAirResist();
    f32 getAirResistRndm();
    f32 getMoment();
    f32 getMomentRndm();
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ gptr<const JPADynamicsBlockData> mpData;
};
WWHD_OFFSET(JPADynamicsBlockArc, mpData, 0x04);
WWHD_SIZE(JPADynamicsBlockArc, 0x08);

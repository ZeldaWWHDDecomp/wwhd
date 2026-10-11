/**
 * JPA_blocks_b.cpp - Dynamics archive parameter accessors.
 * WWHD, verified against the original; names and layout from zeldaret/tww.
 */
#include "JSystem/JParticle/JPA_blocks_b.h"

/* 0284B588 */
f32 JPADynamicsBlockArc::getVolumeSweep() {
    return mpData->mVolumeSweep;
}

/* 0284B594 */
f32 JPADynamicsBlockArc::getVolumeMinRad() {
    return mpData->mVolumeMinRad;
}

/* 0284B5A0 */
u32 JPADynamicsBlockArc::getDivNumber() {
    return mpData->mDivNumber;
}

/* 0284B5AC */
f32 JPADynamicsBlockArc::getRate() {
    return mpData->mRate;
}

/* 0284B5B8 */
f32 JPADynamicsBlockArc::getRateRndm() {
    return mpData->mRateRndm;
}

/* 0284B5C4 */
u32 JPADynamicsBlockArc::getRateStep() {
    return mpData->mRateStep;
}

/* 0284B5D0 */
s32 JPADynamicsBlockArc::getMaxFrame() {
    return mpData->mMaxFrame;
}

/* 0284B5DC */
s32 JPADynamicsBlockArc::getStartFrame() {
    return mpData->mStartFrame;
}

/* 0284B5E8 */
s32 JPADynamicsBlockArc::getLifeTime() {
    return mpData->mLifeTime;
}

/* 0284B5F4 */
f32 JPADynamicsBlockArc::getLifeTimeRndm() {
    return mpData->mLifeTimeRndm;
}

/* 0284B648 */
f32 JPADynamicsBlockArc::getAccel() {
    return mpData->mAccel;
}

/* 0284B654 */
f32 JPADynamicsBlockArc::getAccelRndm() {
    return mpData->mAccelRndm;
}

/* 0284B660 */
f32 JPADynamicsBlockArc::getAirResist() {
    return mpData->mAirResist;
}

/* 0284B66C */
f32 JPADynamicsBlockArc::getAirResistRndm() {
    return mpData->mAirResistRndm;
}

/* 0284B678 */
f32 JPADynamicsBlockArc::getMoment() {
    return mpData->mMoment;
}

/* 0284B684 */
f32 JPADynamicsBlockArc::getMomentRndm() {
    return mpData->mMomentRndm;
}

/* WWHD glue (tools/verify/autoglue.py): no TU-local data */

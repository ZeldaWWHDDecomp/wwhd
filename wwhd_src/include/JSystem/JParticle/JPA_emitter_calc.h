#pragma once
#include "wwhd.h"
struct JPAEmitterCalc_l {
    u8 volumeFunc[8];
    be<f32> emitterScale[3];
    be<f32> emitterTranslation[3];
    be<s16> emitterRotation[3];
    be<u8> volumeType, rateStep;
    be<f32> emitterDirection[3];
    be<f32> rate, rateRandom, accel, accelRandom, airResistance, airRandom;
    be<f32> moment, momentRandom, lifetimeRandom, spread;
    be<s32> maxFrame;
    be<s16> lifetime, startFrame;
    be<u16> volumeSize, divisions;
    be<f32> velocityOmni, velocityAxis, velocityDirection, velocityRandom, velocityRatio;
    be<f32> volumeSweep, volumeMinRadius;
    be<u32> dataFlags, keyFlags;
    u8 pad08c[0x194 - 0x8c];
    be<f32> tick, time;
    u8 pad19c[0x1e0 - 0x19c];
    be<u32> dataLink, emitterCallback, particleCallback, randomSeed;
    be<f32> globalRotation[12], globalScale[3], globalTranslation[3];
    u8 pad238[0x24c - 0x238];
    be<f32> emitCount, rateStepTimer;
    be<u32> status;
    u8 pad258[0x3d4 - 0x258];
    be<u8> deleteCountdown, clearCountdown;
    u8 pad3d6[0x3fc - 0x3d6];
};
WWHD_OFFSET(JPAEmitterCalc_l, tick, 0x194);
WWHD_OFFSET(JPAEmitterCalc_l, randomSeed, 0x1ec);
WWHD_OFFSET(JPAEmitterCalc_l, status, 0x254);
WWHD_OFFSET(JPAEmitterCalc_l, deleteCountdown, 0x3d4);
static_assert(sizeof(JPAEmitterCalc_l) == 0x3fc);

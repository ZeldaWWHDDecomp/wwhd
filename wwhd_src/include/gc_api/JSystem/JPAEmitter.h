/* JPABaseEmitter, HD layout (members at GameCube offset - 4: the HD class has no vtable at +0). */
#pragma once
#include "wwhd.h"

struct JPABaseEmitter {
    /* 0x00 */ u8 _00[0x28];
    /* 0x28 */ be<f32> mEmitterDir[3];   /* GameCube 0x2C */
    /* 0x34 */ be<f32> mRate;            /* GameCube 0x38 [v nz] */
    /* 0x38 */ u8 _38[0x58 - 0x38];
    /* 0x58 */ be<f32> mSpread;          /* GameCube 0x5C [v nz] */
    /* 0x5C */ be<s32> mMaxFrame;        /* GameCube 0x60 [v nz] */
    /* 0x60 */ u8 _60[0x6C - 0x60];
    /* 0x6C */ be<f32> mInitialVelAxis;  /* GameCube 0x70 [v nz] */
    void setRate(f32 r) { mRate = r; }
    void setSpread(f32 s) { mSpread = s; }
    void setAwayFromAxisSpeed(f32 s) { mInitialVelAxis = s; }
    void setMaxFrame(s32 f) { mMaxFrame = f; }
    /* GameCube: setDirection(const JGeometry::TVec3<f32>&); HD inline with the three components (raw form
     * JPABaseEmitter_setDirection verified in lamp) [g] */
    void setDirection(f32 x, f32 y, f32 z) {
        mEmitterDir[0] = x;
        mEmitterDir[1] = y;
        mEmitterDir[2] = z;
    }
};

/* Scalar math (GameCube c_math.h); WWHD keeps float precision and wrapping random-state products.
 * HD initializer layouts below are the per-translation-unit common header statics. */
#pragma once
#include "wwhd.h"

s16 cM_rad2s(f32);
u16 U_GetAtanTable(f32, f32);
s16 cM_atan2s(f32, f32);
f32 cM_atan2f(f32, f32);
void cM_initRnd(s32, s32, s32);
f32 cM_rnd();
f32 cM_rndF(f32);
f32 cM_rndFX(f32);

/* HD: this runtime fmod entry is called with float arguments and returns a float. */
f32 cM_fmod(f32, f32);

struct cMathHeaderStatics {
    /* 0x00 */ be<f32> mNegativePi;
    /* 0x04 */ be<f32> mPositivePi;
    /* 0x08 */ u8 mFirstObject;
    /* 0x09 */ u8 mSecondObject;
    /* 0x0A */ u8 _0A[2];
    /* 0x0C */ be<u32> mZero[4];
};
WWHD_OFFSET(cMathHeaderStatics, mZero, 0xC);
WWHD_SIZE(cMathHeaderStatics, 0x1C);

struct cMathDtorRecord {
    /* 0x00 */ u8 mRecord[0xC];
};
WWHD_SIZE(cMathDtorRecord, 0xC);

struct cMathDtorRecords {
    /* 0x00 */ cMathDtorRecord mFirst;
    /* 0x0C */ cMathDtorRecord mSecond;
    /* 0x18 */ cMathDtorRecord mThird;
};
WWHD_OFFSET(cMathDtorRecords, mSecond, 0xC);
WWHD_OFFSET(cMathDtorRecords, mThird, 0x18);
WWHD_SIZE(cMathDtorRecords, 0x24);

struct cMathPiPair {
    /* 0x00 */ be<f32> mNegativePi;
    /* 0x04 */ be<f32> mPositivePi;
};
WWHD_SIZE(cMathPiPair, 8);

void __register_global_object(cMathDtorRecord*);
void cMathFirstHeaderObject_ct(u8*);
void cMathSecondHeaderObject_ct(u8*);

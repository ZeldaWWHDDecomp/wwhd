/* c_lib guest views. Vector and matrix layouts match GameCube. HD retains full-register
 * integer arguments in the entrypoints; constructor/runtime state is private to this TU.
 * External result-pointer adapters preserve GHS vector-return ABI. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"

struct cLib_vectorWordsHD {
    /* 0x00 */ be<u32> x;
    /* 0x04 */ be<u32> y;
    /* 0x08 */ be<u32> z;
};
WWHD_OFFSET(cLib_vectorWordsHD, x, 0);
WWHD_OFFSET(cLib_vectorWordsHD, y, 4);
WWHD_OFFSET(cLib_vectorWordsHD, z, 8);
WWHD_SIZE(cLib_vectorWordsHD, 0xC);
struct cLib_sinCosHD {
    /* 0x00 */ be<f32> sine;
    /* 0x04 */ be<f32> cosine;
};
WWHD_OFFSET(cLib_sinCosHD, sine, 0);
WWHD_OFFSET(cLib_sinCosHD, cosine, 4);
WWHD_SIZE(cLib_sinCosHD, 8);
struct cLib_runtimeObjectHD {
    /* 0x00 */ u8 opaque[12];
};
WWHD_SIZE(cLib_runtimeObjectHD, 12);
struct cLib_runtimeConstantsHD {
    /* 0x00 */ be<f32> lower;
    /* 0x04 */ be<f32> upper;
};
WWHD_OFFSET(cLib_runtimeConstantsHD, lower, 0);
WWHD_OFFSET(cLib_runtimeConstantsHD, upper, 4);
WWHD_SIZE(cLib_runtimeConstantsHD, 8);
struct cLib_runtimeStateHD {
    /* 0x00 */ be<f32> lower;
    /* 0x04 */ be<f32> upper;
    /* 0x08 */ be<u8> flagA;
    /* 0x09 */ be<u8> flagB;
    /* 0x0A */ u8 padding[2];
    /* 0x0C */ be<u32> zero[4];
};
WWHD_OFFSET(cLib_runtimeStateHD, lower, 0);
WWHD_OFFSET(cLib_runtimeStateHD, upper, 4);
WWHD_OFFSET(cLib_runtimeStateHD, flagA, 8);
WWHD_OFFSET(cLib_runtimeStateHD, flagB, 9);
WWHD_OFFSET(cLib_runtimeStateHD, zero, 0xC);
WWHD_SIZE(cLib_runtimeStateHD, 0x1C);

void cLib_memsetHD(void*, u32, u32);
f32 PSVECSquareMag(const cXyz*);
f32 cLib_sqrtfHD(f32);
f32 PSVECSquareDistance(const cXyz*, const cXyz*);
s32 cXyz_notEqualHD(const cXyz*, const cXyz*);
s32 cXyz_equalHD(const cXyz*, const cXyz*);
void cXyz_subtractHD(const cXyz*, cXyz*, const cXyz*);
void PSVECScale(const cXyz*, cXyz*, f32);
void PSVECSubtract(const cXyz*, const cXyz*, cXyz*);
void cXyz_scaleResultHD(const cXyz*, cXyz*, f32);
void cXyz_normalizeResultHD(const cXyz*, cXyz*);
s16 cM_atan2s(f32, f32);
void MTXTrans(Mtx34*, f32, f32, f32);
void MTXConcat(const Mtx34*, const Mtx34*, Mtx34*);
void MTXRotRad(Mtx34*, u32, f32);
void MTXScale(Mtx34*, f32, f32, f32);
void MTXMultVec(const Mtx34*, const cXyz*, cXyz*);
void MTXCopy(const Mtx34*, Mtx34*);
void cLib_runtimeObjectInitHD(cLib_runtimeObjectHD*);
void cLib_runtimeFlagAInitHD(be<u8>*);
void cLib_runtimeFlagBInitHD(be<u8>*);

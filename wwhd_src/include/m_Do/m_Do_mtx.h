/** Matrix helpers: GameCube matrix/vector layouts, HD full-width entry arguments.
 * Float evaluation order and aliasing are part of the verified contract. Private source. */
#pragma once
#include "SSystem/SComponent/c_xyz.h"

struct mDoMtx44 { be<f32> m[4][4]; };
struct mDoMtxSinCos { be<f32> mSin, mCos; };
struct mDoMtxStackPointers { gptr<Mtx34> next, end; };
struct mDoMtxBuffer { Mtx34 buffer[16]; };
struct mDoMtxQuatStack {
    gptr<Quaternion> field_0x0;
    Quaternion field_0x4;
    Quaternion field_0x14[16];
    gptr<Quaternion> field_0x114;
    gptr<gptr<Quaternion>> field_0x118;
};
struct mDoMtxHeaderStatics {
    be<f32> mNegativePi, mPi;
    be<u8> mFlag, mByte;
    u8 padding[2];
    be<u32> mZeroWords[4];
};
struct mDoMtxStaticRecord { u8 opaque[12]; };
struct mDoMtxStaticRecords { mDoMtxStaticRecord mFirst, mSecond, mThird; };
struct mDoMtxVectorWords { be<u32> x, y, z; };
WWHD_SIZE(mDoMtx44, 64);
WWHD_SIZE(mDoMtxSinCos, 8);
WWHD_SIZE(mDoMtxStackPointers, 8);
WWHD_SIZE(mDoMtxBuffer, 0x300);
WWHD_OFFSET(mDoMtxQuatStack, field_0x114, 0x114);
WWHD_OFFSET(mDoMtxQuatStack, field_0x118, 0x118);
WWHD_SIZE(mDoMtxQuatStack, 0x11C);
WWHD_SIZE(mDoMtxHeaderStatics, 0x1C);
WWHD_SIZE(mDoMtxStaticRecord, 12);
WWHD_SIZE(mDoMtxStaticRecords, 36);
WWHD_SIZE(mDoMtxVectorWords, 12);
inline Mtx34* mDoMtxAt(u32 address) { return gabi::at<Mtx34>(address); }
inline mDoMtx44* mDoMtx44At(u32 address) { return gabi::at<mDoMtx44>(address); }
inline cXyz* mDoMtxVectorAt(u32 address) { return gabi::at<cXyz>(address); }
inline Quaternion* mDoMtxQuatAt(u32 address) { return gabi::at<Quaternion>(address); }
inline csXyz* mDoMtxAnglesAt(u32 address) { return gabi::at<csXyz>(address); }
inline mDoMtxSinCos* mDoMtxSinCosAt(s32 angle) {
    return gabi::at<mDoMtxSinCos>(0x104A44F8u + (u32(u16(angle)) >> 3) * 8);
}
// GHS copies all words before writing any, including signalling NaN payloads.
inline void mDoMtx_copyVectorWords(u32 dest, u32 source) {
    auto* src = gabi::at<mDoMtxVectorWords>(source);
    auto* dst = gabi::at<mDoMtxVectorWords>(dest);
    u32 x = src->x, y = src->y, z = src->z;
    dst->x = x; dst->y = y; dst->z = z;
}
inline void mDoMtx_copyVectorFloats(u32 dest, u32 source) {
    auto* src = mDoMtxVectorAt(source);
    auto* dst = mDoMtxVectorAt(dest);
    f32 x = src->x, y = src->y, z = src->z;
    dst->x = x; dst->y = y; dst->z = z;
}
void mDoMtx_identity(u32);
void mDoMtx_concat(u32, u32, u32);
void mDoMtx_copy(u32, u32);
void mDoMtx_vectorSubtract(u32, u32, u32);
void mDoMtx_vectorNormalize(u32, u32);
void mDoMtx_vectorCross(u32, u32, u32);
s32 mDoMtx_vectorNormalizeRS(u32);
f32 mDoMtx_vectorDot(u32, u32);
void mDoMtx_sdkLookAt(u32, u32, u32, u32);
f32 mDoMtx_sqrt(f32);
s32 mDoMtx_atan2s(f32, f32);
void mDoMtx_assert(const char*, u32, const char*);
void mDoMtx_trans(u32, f32, f32, f32);
void mDoMtx_scale(u32, f32, f32, f32);
void mDoMtx_rotRad(u32, u32, f32);
void mDoMtx_quat(u32, u32);
void mDoMtx_registerStatic(u32);
void mDoMtx_initFlag(u32);
void mDoMtx_initByte(u32);

void mDoMtx_ZrotS(u32 p, s32 a);
void mDoMtx_YrotS(u32 p, s32 a);
void mDoMtx_XrotS(u32 p, s32 a);
void mDoMtx_XYZrotS(u32 p, s32 x, s32 y, s32 z);
void mDoMtx_XYZrotM(u32 p, s32 x, s32 y, s32 z);
void mDoMtx_ZXYrotS(u32 p, s32 x, s32 y, s32 z);
void mDoMtx_ZXYrotM(u32 p, s32 x, s32 y, s32 z);
void mDoMtx_XrotM(u32 p, s32 a);
void mDoMtx_YrotM(u32 p, s32 a);
void mDoMtx_ZrotM(u32 p, s32 a);
void mDoMtx_lookAt(u32 p, u32 eye, u32 target, s32 angle);
void mDoMtx_lookAtUp(u32 p, u32 eye, u32 target, u32 up, s32 angle);
void mDoMtx_concatProjView(u32 p, u32 v, u32 d);
s32 mDoMtx_inverseTranspose(u32 p, u32 d);
void mDoMtx_QuatConcat(u32 a, u32 b, u32 d);
void mDoMtx_MtxToRot(u32 p, u32 d);
s32 mDoMtx_push();
s32 mDoMtx_pop();
void mDoMtx_transM(f32 x, f32 y, f32 z);
void mDoMtx_scaleM(f32 x, f32 y, f32 z);
void mDoMtx_lYrotM(s32 a);
void mDoMtx_rYrotM(f32 a);
void mDoMtx_quatM(u32 q);

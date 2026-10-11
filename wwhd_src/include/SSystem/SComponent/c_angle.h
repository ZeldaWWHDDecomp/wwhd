/** Signed angles and spherical coordinates; GameCube field layouts are unchanged.
 * HD exposes allocating constructors and explicit result pointers. Private game-derived source. */
#pragma once
#include "bindings.h"

struct cSAngle { /* 0x00 */ be<s16> mAngle; };
struct cDegree { /* 0x00 */ be<f32> mDegree; };
struct cSPolar {
    /* 0x00 */ be<f32> mRadial;
    /* 0x04 */ cSAngle mAngle1;
    /* 0x06 */ cSAngle mAngle2;
};
struct cSGlobe {
    /* 0x00 */ be<f32> mRadius;
    /* 0x04 */ cSAngle mAzimuth;
    /* 0x06 */ cSAngle mInclination;
};
struct cAngleXyz { be<f32> x, y, z; };
struct cAngleHeaderStatics {
    be<f32> mNegativePi, mPi;
    be<u8> mFlag, mByte;
    u8 padding[2];
    be<u32> mZeroWords[4];
};
struct cAngleStaticRecord { u8 opaque[12]; };
struct cAngleStaticRecords { cAngleStaticRecord mFirst, mSecond, mThird; };
struct cAngleConstants { cSAngle mZero, mOne, mRightAngle, mHalfTurn, mThreeQuarterTurn; };
WWHD_SIZE(cSAngle, 2);
WWHD_SIZE(cDegree, 4);
WWHD_OFFSET(cSPolar, mAngle1, 4);
WWHD_OFFSET(cSPolar, mAngle2, 6);
WWHD_SIZE(cSPolar, 8);
WWHD_OFFSET(cSGlobe, mAzimuth, 4);
WWHD_OFFSET(cSGlobe, mInclination, 6);
WWHD_SIZE(cSGlobe, 8);
WWHD_SIZE(cAngleXyz, 12);
WWHD_SIZE(cAngleHeaderStatics, 0x1C);
WWHD_SIZE(cAngleStaticRecords, 36);
WWHD_SIZE(cAngleConstants, 10);

// HD: Keep the integer entry signatures, including the untruncated constructor inputs.
inline cSAngle* cSAngleAt(u32 address) { return gabi::at<cSAngle>(address); }
inline cDegree* cDegreeAt(u32 address) { return gabi::at<cDegree>(address); }
inline cSPolar* cSPolarAt(u32 address) { return gabi::at<cSPolar>(address); }
inline cSGlobe* cSGlobeAt(u32 address) { return gabi::at<cSGlobe>(address); }
u32 cAngle_alloc(u32 size);
f32 cAngle_sin(f32);
f32 cAngle_cos(f32);
f32 cAngle_tan(f32);
f32 cAngle_sqrt(f32);
s16 cAngle_atan2s(f32, f32);
void cAngle_registerStatic(void*);
void cAngle_initFlag(void*);
void cAngle_initByte(void*);

void cSAngle_Val(u32 p,s16 v);
u32 cSAngle_construct(u32 p,u32 v);
u32 cSAngle_scalarPlus(u32 out,s16 v,u32 a);
u32 cSAngle_scalarMinus(u32 out,s16 v,u32 a);
u32 cSAngle_constructDefault(u32 p);
void cSAngle_copyVal(u32 p,u32 a);
u32 cSAngle_constructCopy(u32 p,u32 a);
void cSAngle_degreeVal(u32 p,f32 v);
u32 cSAngle_constructDegree(u32 p,f32 v);
f32 cSAngle_Degree(u32 p);
f32 cSAngle_Radian(u32 p);
f32 cSAngle_Norm(u32 p);
s16 cSAngle_Abs(u32 p);
s16 cSAngle_Inv(u32 p);
f32 cSAngle_Sin(u32 p);
f32 cSAngle_Cos(u32 p);
f32 cSAngle_Tan(u32 p);
u32 cSAngle_negate(u32 p,u32 out);
u32 cSAngle_add(u32 p,u32 out,u32 q);
u32 cSAngle_subtract(u32 p,u32 out,u32 q);
void cSAngle_addAssign(u32 p,u32 q);
void cSAngle_subtractAssign(u32 p,u32 q);
u32 cSAngle_addScalar(u32 p,u32 out,s16 v);
u32 cSAngle_subtractScalar(u32 p,u32 out,s16 v);
void cSAngle_addScalarAssign(u32 p,s16 v);
void cSAngle_subtractScalarAssign(u32 p,s16 v);
u32 cSAngle_multiply(u32 p,u32 out,f32 v);
void cSAngle_multiplyAssign(u32 p,f32 v);
u32 cDegree_Formal(u32 p);
u32 cDegree_Val(u32 p,f32 v);
u32 cDegree_construct(u32 p,f32 v);
f32 cDegree_Radian(u32 p);
f32 cDegree_Sin(u32 p);
f32 cDegree_Cos(u32 p);
u32 cSPolar_constructDefault(u32 p);
u32 cSPolar_Formal(u32 p);
void cSPolar_Val(u32 p,u32 a,u32 b,f32 r);
void cSPolar_vectorVal(u32 p,u32 v);
u32 cSPolar_constructVector(u32 p,u32 v);
void cSPolar_Xyz(u32 p,u32 out);
u32 cSGlobe_Formal(u32 p);
void cSGlobe_Val(u32 p,u32 a,u32 b,f32 r);
void cSPolar_Globe(u32 p,u32 out);
u32 cSGlobe_construct(u32 p,u32 a,u32 b,f32 r);
u32 cSGlobe_constructDefault(u32 p);
u32 cSGlobe_copyVal(u32 p,u32 q);
u32 cSGlobe_constructCopy(u32 p,u32 q);
void cSGlobe_angleVal(u32 p,u32 a,u32 b,f32 r);
u32 cSGlobe_constructAngles(u32 p,u32 a,u32 b,f32 r);
void cSGlobe_vectorVal(u32 p,u32 v);
u32 cSGlobe_constructVector(u32 p,u32 v);
void cSGlobe_Polar(u32 p,u32 out);
void cSGlobe_Xyz(u32 p,u32 out);
u32 cSGlobe_Invert(u32 p);
f32 cAngle_Adjust(f32 v,f32 lo,f32 hi);

// HD: Copy the radius through a word view, preserving all float bits.
inline void cSGlobe_copyRadiusWord(cSGlobe* dest, const cSGlobe* source) {
    gabi::store<u32>(gabi::ea(&dest->mRadius), gabi::load<u32>(gabi::ea(&source->mRadius)));
}

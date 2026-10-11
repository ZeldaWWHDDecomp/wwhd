/** JPA fields: HD omits JPAFieldData's leading vtable, shifting its data by -4. */
#pragma once
#include "wwhd.h"
#include "JSystem/JParticle/JPAParticle.h"

// HD: each split field unit owns a private view while using the tww class and member names.
namespace jpa_field_a {
struct JPAFieldVectorWords { be<u32> x, y, z; };
union JPAFieldVector { cXyz mFloat; JPAFieldVectorWords mBits; };
class JPABaseField;
struct JPAFieldData {
    /* 0x00 */ gptr<JPABaseField> mpBaseField;
    /* 0x04 */ u8 mLink[0x10];
    /* 0x14 */ JPAFieldVector mVel;
    /* 0x20 */ JPAFieldVector mWork0;
    /* 0x2C */ JPAFieldVector mWork1;
    /* 0x38 */ JPAFieldVector mWork2;
    /* 0x44 */ be<f32> mMaxDistSq;
    /* 0x48 */ be<f32> mFadeOutRate;
    /* 0x4C */ be<f32> mFadeInRate;
    /* 0x50 */ JPAFieldVector mPos;
    /* 0x5C */ JPAFieldVector mDir;
    /* 0x68 */ be<f32> mMag;
    /* 0x6C */ be<f32> mMagRndm;
    /* 0x70 */ be<f32> mMaxDist;
    /* 0x74 */ be<f32> mVal1;
    /* 0x78 */ be<f32> mVal2;
    /* 0x7C */ be<f32> mVal3;
    /* 0x80 */ be<f32> mFadeIn;
    /* 0x84 */ be<f32> mFadeOut;
    /* 0x88 */ be<f32> mEnTime;
    /* 0x8C */ be<f32> mDisTime;
    /* 0x90 */ be<u16> mSttFlag;
    /* 0x92 */ be<u8> mType;
    /* 0x93 */ be<u8> mID;
    /* 0x94 */ be<u8> mVelType;
    /* 0x95 */ be<u8> mCycle;
};
class JPABaseField {
public:
    f32 calcFadeAffect(JPAFieldData* data, f32 time);
    void calcVel(JPAFieldData* data, JPABaseParticle* particle);
    void preCalc(JPAFieldData* data);
    bool isItinRange(JPAFieldData* data, f32 distanceSq);
    void init(JPAFieldData* data, JPABaseParticle* particle);
    gptr<void> mVtable;
};
class JPAGravityField : public JPABaseField {
public:
    void preCalc(JPAFieldData* data);
    void calc(JPAFieldData* data, JPABaseParticle* particle);
};
class JPAAirField : public JPABaseField {
public:
    void preCalc(JPAFieldData* data);
    void calc(JPAFieldData* data, JPABaseParticle* particle);
};
class JPAMagnetField : public JPABaseField {
public:
    void preCalc(JPAFieldData* data);
    void calc(JPAFieldData* data, JPABaseParticle* particle);
};
class JPANewtonField : public JPABaseField {
public:
    void preCalc(JPAFieldData* data);
    void calc(JPAFieldData* data, JPABaseParticle* particle);
};
struct JPAEmitterInfo {
    u8 _opaque00[0x68];
    /* 0x68 */ Mtx34 mGlobalRot;
    u8 _opaque98[0x3C];
    /* 0xD4 */ cXyz mEmitterTranslation;
};
struct JPAFieldSinCosPair { be<u32> mSine; be<u32> mCosine; };
struct JPAFieldSinCosTable { JPAFieldSinCosPair mEntries[8192]; };
enum JPAFieldFlags {
    JPA_FIELD_CONE = 0x1,
    JPA_FIELD_GLOBAL = 0x2,
    JPA_FIELD_LIMIT_SPEED = 0x4,
    JPA_FIELD_ENABLE_TIME = 0x8,
    JPA_FIELD_DISABLE_TIME = 0x10,
    JPA_FIELD_FADE_IN = 0x20,
    JPA_FIELD_FADE_OUT = 0x40,
    JPA_FIELD_CONSTANT_MAGNITUDE = 0x100,
};
enum JPAFieldVelocityType { JPA_FIELD_ACCELERATION = 0, JPA_BASE_VELOCITY = 1, JPA_FIELD_VELOCITY = 2 };
enum { JPAPtlcStts_Child = 4 };
WWHD_SIZE(JPAFieldVectorWords,0xC);
WWHD_SIZE(JPAFieldVector,0xC);
WWHD_SIZE(JPAFieldData,0x96);
WWHD_SIZE(JPABaseField,4);
WWHD_SIZE(JPAGravityField,4);
WWHD_SIZE(JPAAirField,4);
WWHD_SIZE(JPAMagnetField,4);
WWHD_SIZE(JPANewtonField,4);
WWHD_SIZE(JPAEmitterInfo,0xE0);
WWHD_OFFSET(JPAEmitterInfo,mGlobalRot,0x68);
WWHD_OFFSET(JPAEmitterInfo,mEmitterTranslation,0xD4);
WWHD_SIZE(JPAFieldSinCosPair,8);
WWHD_SIZE(JPAFieldSinCosTable,0x10000);
WWHD_OFFSET(JPAFieldData,mVel,0x14);
WWHD_OFFSET(JPAFieldData,mWork0,0x20);
WWHD_OFFSET(JPAFieldData,mWork1,0x2c);
WWHD_OFFSET(JPAFieldData,mWork2,0x38);
WWHD_OFFSET(JPAFieldData,mPos,0x50);
WWHD_OFFSET(JPAFieldData,mDir,0x5c);
WWHD_OFFSET(JPAFieldData,mMaxDistSq,0x44);
WWHD_OFFSET(JPAFieldData,mFadeOutRate,0x48);
WWHD_OFFSET(JPAFieldData,mFadeInRate,0x4c);
WWHD_OFFSET(JPAFieldData,mMag,0x68);
WWHD_OFFSET(JPAFieldData,mMagRndm,0x6c);
WWHD_OFFSET(JPAFieldData,mMaxDist,0x70);
WWHD_OFFSET(JPAFieldData,mVal1,0x74);
WWHD_OFFSET(JPAFieldData,mVal2,0x78);
WWHD_OFFSET(JPAFieldData,mVal3,0x7c);
WWHD_OFFSET(JPAFieldData,mFadeIn,0x80);
WWHD_OFFSET(JPAFieldData,mFadeOut,0x84);
WWHD_OFFSET(JPAFieldData,mEnTime,0x88);
WWHD_OFFSET(JPAFieldData,mDisTime,0x8c);
WWHD_OFFSET(JPAFieldData,mSttFlag,0x90);
WWHD_OFFSET(JPAFieldData,mVelType,0x94);
} // namespace jpa_field_a
f64 jpaFieldSqrt(f64 value);
void jpaFieldTransformVector(const Mtx34* matrix, const cXyz* source, cXyz* destination);

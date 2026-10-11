// Local WWHD reconstruction using the CC0 TWW JPA1 template.
#include "JSystem/JParticle/JPA_field_a.h"
using namespace jpa_field_a;

static JPAEmitterInfo* sFieldEmitterInfo;
static JPAFieldSinCosTable sFieldSinCosTable;

static f32 mul(f32 a, f32 b) { return gabi::fmuls_ppc(a, b); }
static f32 sub(f32 a, f32 b) { return gabi::fsubs_ppc(a, b); }
static f32 add(f32 a, f32 b) { return gabi::fadds_ppc(a, b); }
static f32 squared(f32 x, f32 y, f32 z) { return gabi::fmadds(z, z, gabi::fmadds(x, x, mul(y, y))); }
static f32 root(f32 v) { return (f32)jpaFieldSqrt((f64)v); }

/* 02822AC0 */
f32 jpa_field_a::JPABaseField::calcFadeAffect(JPAFieldData* data, f32 time) {
    u16 flags = data->mSttFlag;
    if (((flags & JPA_FIELD_ENABLE_TIME) && time < data->mEnTime) ||
        ((flags & JPA_FIELD_DISABLE_TIME) && !(time < data->mDisTime))) return 0.0f;
    if ((flags & JPA_FIELD_FADE_OUT) && !(time < data->mFadeOut))
        return mul(sub(data->mDisTime, time), data->mFadeOutRate);
    if ((flags & JPA_FIELD_FADE_IN) && time < data->mFadeIn)
        return mul(sub(time, data->mEnTime), data->mFadeInRate);
    return 1.0f;
}

/* 02822B54 */
void jpa_field_a::JPABaseField::calcVel(JPAFieldData* data, JPABaseParticle* particle) {
    u32 flags = particle->mStatus;
    f32 x = data->mVel.mFloat.x, z = data->mVel.mFloat.z, y = data->mVel.mFloat.y;
    if (!(flags & JPAPtlcStts_Child)) {
        f32 fade = calcFadeAffect(data, particle->mCurNormTime);
        y = mul(y, fade); x = mul(x, fade); z = mul(z, fade);
    }
    u8 type = data->mVelType;
    cXyz* velocity;
    switch (type) {
    case JPA_FIELD_ACCELERATION: velocity = &particle->mFieldAccel; break;
    case JPA_BASE_VELOCITY: velocity = &particle->mBaseVel; break;
    case JPA_FIELD_VELOCITY: velocity = &particle->mFieldVel; break;
    default: return;
    }
    f32 vx = velocity->x, vy = velocity->y;
    vx = add(vx, x);
    f32 vz = velocity->z;
    vy = add(vy, y); vz = add(vz, z);
    velocity->x = vx; velocity->y = vy; velocity->z = vz;
}

/* 02822C38 */
void jpa_field_a::JPABaseField::preCalc(JPAFieldData* data) {
    f32 distance = data->mMaxDist, fadeOut = data->mFadeOut;
    f32 distanceSq = mul(distance, distance);
    f32 outDelta = sub(data->mDisTime, fadeOut);
    data->mMaxDistSq = distanceSq;
    f32 enable = data->mEnTime;
    f32 inDelta = sub(data->mFadeIn, enable);
    data->mFadeOutRate = outDelta == 0.0f ? 1.0f : 1.0f / outDelta;
    data->mFadeInRate = inDelta == 0.0f ? 1.0f : 1.0f / inDelta;
}

/* 02822CA8 */
bool jpa_field_a::JPABaseField::isItinRange(JPAFieldData* data, f32 distanceSq) {
    return distanceSq < data->mMaxDistSq;
}

/* 02824504 */
void jpa_field_a::JPABaseField::init(JPAFieldData* data, JPABaseParticle* particle) {
}

/* 02822CBC */
void jpa_field_a::JPAGravityField::preCalc(JPAFieldData* data) {
    JPABaseField::preCalc(data);
    if (data->mSttFlag & JPA_FIELD_GLOBAL) {
        f32 y = data->mDir.mFloat.y, mag = data->mMag, x = data->mDir.mFloat.x;
        f32 vy = mul(y, mag), vx = mul(x, mag), z = data->mDir.mFloat.z;
        data->mVel.mFloat.y = vy;
        f32 vz = mul(z, mag);
        data->mVel.mFloat.x = vx; data->mVel.mFloat.z = vz;
    } else {
        // HD: retain the original fixed stack vector slot.
        gabi::FrameLocal<cXyz> direction(8);
        jpaFieldTransformVector(&sFieldEmitterInfo->mGlobalRot, &data->mDir.mFloat, direction.get());
        f32 y = direction->y, mag = data->mMag, x = direction->x;
        f32 vy = mul(y, mag), vx = mul(x, mag), z = direction->z;
        data->mVel.mFloat.y = vy;
        f32 vz = mul(z, mag);
        data->mVel.mFloat.x = vx; data->mVel.mFloat.z = vz;
    }
}

// HD: the gravity calculation tail-calls the shared velocity calculation.
/* 02822D60 */
void jpa_field_a::JPAGravityField::calc(JPAFieldData* data, JPABaseParticle* particle) {
    JPABaseField::calcVel(data, particle);
}

/* 02822D64 */
void jpa_field_a::JPAAirField::preCalc(JPAFieldData* data) {
    JPABaseField::preCalc(data);
    if (data->mSttFlag & JPA_FIELD_GLOBAL) {
        f32 mag = data->mMag, y = data->mDir.mFloat.y;
        u32 xBits = data->mDir.mBits.x;
        f32 vy = mul(y, mag);
        data->mWork2.mBits.x = xBits;
        u32 zBits = data->mDir.mBits.z;
        data->mWork2.mBits.y = data->mDir.mBits.y;
        data->mWork2.mBits.z = zBits;
        f32 vx = mul(data->mWork2.mFloat.x, mag), z = data->mWork2.mFloat.z;
        data->mVel.mFloat.x = vx;
        f32 vz = mul(z, mag);
        data->mVel.mFloat.y = vy; data->mVel.mFloat.z = vz;
    } else {
        jpaFieldTransformVector(&sFieldEmitterInfo->mGlobalRot, &data->mDir.mFloat, &data->mWork2.mFloat);
        f32 x = data->mWork2.mFloat.x, mag = data->mMag, y = data->mWork2.mFloat.y;
        f32 vx = mul(x, mag), z = data->mWork2.mFloat.z, vy = mul(y, mag);
        data->mVel.mFloat.x = vx;
        f32 vz = mul(z, mag);
        data->mVel.mFloat.y = vy; data->mVel.mFloat.z = vz;
    }
    if (data->mSttFlag & JPA_FIELD_CONE) {
        u16 angle = (u16)gabi::ftoi(mul(data->mVal1, 32768.0f));
        u32 cosine = sFieldSinCosTable.mEntries[(u32)angle >> 3].mCosine;
        data->mWork1.mBits.x = cosine;
        if (data->mSttFlag & JPA_FIELD_GLOBAL) {
            u32 x = data->mPos.mBits.x, y = data->mPos.mBits.y;
            data->mWork0.mBits.x = x;
            u32 z = data->mPos.mBits.z;
            data->mWork0.mBits.y = y; data->mWork0.mBits.z = z;
        } else {
            jpaFieldTransformVector(&sFieldEmitterInfo->mGlobalRot, &data->mWork0.mFloat, &data->mPos.mFloat);
        }
    }
}

/* 02822EC0 */
void jpa_field_a::JPAAirField::calc(JPAFieldData* data, JPABaseParticle* particle) {
    u16 flags = data->mSttFlag;
    // HD: unordered cone/fade comparisons follow the original branch conditions.
    bool apply = true;
    if (flags & JPA_FIELD_CONE) {
        cXyz* pos = (flags & JPA_FIELD_GLOBAL) ? &particle->mGlobalPosition : &particle->mLocalPosition;
        f32 ox = data->mWork0.mFloat.x, oy = data->mWork0.mFloat.y;
        f32 y = sub(pos->y, oy), x = sub(pos->x, ox);
        f32 z = sub(pos->z, data->mWork0.mFloat.z);
        f32 lengthSq = squared(x, y, z);
        if (lengthSq > 0.000003814697265625f) {
            f32 length = root(lengthSq), scale = 1.0f / length;
            x = mul(x, scale); z = mul(z, scale); y = mul(y, scale);
        }
        f32 dot = gabi::fmadds(data->mWork2.mFloat.x, x, mul(data->mWork2.mFloat.y, y));
        dot = gabi::fmadds(data->mWork2.mFloat.z, z, dot);
        apply = !(dot < data->mWork1.mFloat.x);
    }
    if (apply) JPABaseField::calcVel(data, particle);
    if (data->mSttFlag & JPA_FIELD_LIMIT_SPEED) {
        f32 y = particle->mBaseVel.y, x = particle->mBaseVel.x, z = particle->mBaseVel.z;
        f32 length = root(squared(x, y, z)), limit = data->mMagRndm;
        if (length > limit) {
            f32 scale = limit / length;
            x = particle->mBaseVel.x; y = particle->mBaseVel.y;
            x = mul(x, scale); z = particle->mBaseVel.z; y = mul(y, scale);
            particle->mBaseVel.x = x; z = mul(z, scale);
            particle->mBaseVel.y = y; particle->mBaseVel.z = z;
        }
    }
}

/* 0282308C */
void jpa_field_a::JPAMagnetField::preCalc(JPAFieldData* data) {
    JPABaseField::preCalc(data);
    u16 flags = data->mSttFlag;
    u32 xBits = data->mPos.mBits.x;
    if (flags & JPA_FIELD_GLOBAL) {
        u32 y = data->mPos.mBits.y, z = data->mPos.mBits.z;
        data->mWork0.mBits.y = y; data->mWork0.mBits.z = z; data->mWork0.mBits.x = xBits;
    } else {
        JPAEmitterInfo* info = sFieldEmitterInfo;
        data->mWork0.mFloat.x = sub(data->mPos.mFloat.x, info->mEmitterTranslation.x);
        data->mWork0.mFloat.y = sub(data->mPos.mFloat.y, info->mEmitterTranslation.y);
        data->mWork0.mFloat.z = sub(data->mPos.mFloat.z, info->mEmitterTranslation.z);
        jpaFieldTransformVector(&sFieldEmitterInfo->mGlobalRot, &data->mWork0.mFloat, &data->mWork0.mFloat);
    }
}

/* 0282312C */
void jpa_field_a::JPAMagnetField::calc(JPAFieldData* data, JPABaseParticle* particle) {
    cXyz* pos = (data->mSttFlag & JPA_FIELD_GLOBAL) ? &particle->mGlobalPosition : &particle->mLocalPosition;
    f32 x = sub(data->mWork0.mFloat.x, pos->x); data->mVel.mFloat.x = x;
    f32 y = sub(data->mWork0.mFloat.y, pos->y);
    f32 z0 = data->mWork0.mFloat.z; data->mVel.mFloat.y = y;
    f32 z = sub(z0, pos->z); data->mVel.mFloat.z = z;
    f32 lengthSq = squared(x, y, z), mag = data->mMag;
    if (lengthSq > 0.000003814697265625f) {
        f32 length = root(lengthSq), scale = mul(1.0f / length, mag);
        x = data->mVel.mFloat.x; y = data->mVel.mFloat.y;
        x = mul(x, scale); z = data->mVel.mFloat.z; y = mul(y, scale);
        data->mVel.mFloat.x = x; z = mul(z, scale);
        data->mVel.mFloat.y = y; data->mVel.mFloat.z = z;
    }
    JPABaseField::calcVel(data, particle);
}

/* 02823280 */
void jpa_field_a::JPANewtonField::preCalc(JPAFieldData* data) {
    JPABaseField::preCalc(data);
    u16 flags = data->mSttFlag;
    u32 x = data->mPos.mBits.x;
    if (flags & JPA_FIELD_GLOBAL) {
        data->mWork0.mBits.z = data->mPos.mBits.z;
        f32 value = data->mVal1;
        u32 y = data->mPos.mBits.y;
        f32 square = mul(value, value);
        data->mWork0.mBits.y = y; data->mWork0.mBits.x = x;
        data->mVal2 = square;
    } else {
        JPAEmitterInfo* info = sFieldEmitterInfo;
        data->mWork0.mFloat.x = sub(data->mPos.mFloat.x, info->mEmitterTranslation.x);
        data->mWork0.mFloat.y = sub(data->mPos.mFloat.y, info->mEmitterTranslation.y);
        data->mWork0.mFloat.z = sub(data->mPos.mFloat.z, info->mEmitterTranslation.z);
        jpaFieldTransformVector(&sFieldEmitterInfo->mGlobalRot, &data->mWork0.mFloat, &data->mWork0.mFloat);
        f32 value = data->mVal1; data->mVal2 = mul(value, value);
    }
}

/* 02823334 */
void jpa_field_a::JPANewtonField::calc(JPAFieldData* data, JPABaseParticle* particle) {
    bool global = data->mSttFlag & JPA_FIELD_GLOBAL;
    cXyz* pos = global ? &particle->mGlobalPosition : &particle->mLocalPosition;
    f32 x = sub(data->mWork0.mFloat.x, pos->x);
    f32 y0 = data->mWork0.mFloat.y; data->mVel.mFloat.x = x;
    f32 y = sub(y0, pos->y);
    u16 flags = data->mSttFlag;
    f32 z0 = data->mWork0.mFloat.z, mag = data->mMag; data->mVel.mFloat.y = y;
    f32 z = sub(z0, pos->z);
    f32 yy = global ? mul(y, y) : mul(data->mVel.mFloat.y, data->mVel.mFloat.y);
    data->mVel.mFloat.z = z;
    f32 lengthSq = gabi::fmadds(z, z, gabi::fmadds(x, x, yy));
    if (!(flags & JPA_FIELD_CONSTANT_MAGNITUDE)) {
        f32 threshold = data->mVal2;
        mag = mul(mag, 10.0f);
        if (lengthSq > threshold) mag = mul(mag, threshold) / lengthSq;
    }
    if (lengthSq > 0.000003814697265625f) {
        f32 length = root(lengthSq), scale = mul(1.0f / length, mag);
        x = data->mVel.mFloat.x; y = data->mVel.mFloat.y;
        x = mul(x, scale); z = data->mVel.mFloat.z; y = mul(y, scale);
        data->mVel.mFloat.x = x; z = mul(z, scale);
        data->mVel.mFloat.y = y; data->mVel.mFloat.z = z;
    }
    JPABaseField::calcVel(data, particle);
}

/* WWHD glue
 data sFieldEmitterInfo 104B5948
 data sFieldSinCosTable 104A44F8
 ext 028F37F0 f64 jpaFieldSqrt(f64)
 ext 028E8F64 void jpaFieldTransformVector(const Mtx34*, const cXyz*, cXyz*)
*/

/** Particle emitter calculation and volume sampling. Verified WWHD behavior, tww names. */
#include "JSystem/JParticle/JPA_emitter_calc.h"
#include <climits>
#include <cstring>
namespace jpa_emitter_calc {
static JPAEmitterInfo emtrInfo;
static char assertFile[1] = {};
static char assertMessage[1] = {};
// HD: typed ROM views use original bytes rather than invented initializers.
static SinCosEntry sincosTable[8192] = {};
u32 randomNext(JPABaseEmitter *e) {
    u32 s = (u32)e->mRandomSeed * 0x19660du + 0x3c6ef35fu;
    e->mRandomSeed = s;
    return s;
}
f32 randomFloat(JPABaseEmitter *e) {
    u32 bits = 0x3f800000u | (randomNext(e) >> 9);
    f32 f;
    std::memcpy(&f, &bits, 4);
    return f - 1.0f;
}
s16 randomShort(JPABaseEmitter *e) {
    return (s16)(randomNext(e) >> 16);
}
f32 sinAngle(u16 angle) {
    return sincosTable[angle >> 3].sin;
}
f32 cosAngle(u16 angle) {
    return sincosTable[angle >> 3].cos;
}
// HD: signed division edge cases retain the original quotient used by volume/key loops.
s32 divide(s32 a, s32 b) {
    return !b || (a == INT_MIN && b == -1) ? (a < 0 ? -1 : 0) : a / b;
}
void volumeResult(f32 x, f32 y, f32 z) {
    f32 sx = (f32)emtrInfo.mEmitterGlobalScale[0], sy = (f32)emtrInfo.mEmitterGlobalScale[1], sz = (f32)emtrInfo.mEmitterGlobalScale[2];
    emtrInfo.mVolumePos[0] = x;
    emtrInfo.mVolumePos[1] = y;
    emtrInfo.mVolumePos[2] = z;
    emtrInfo.mVelOmni[0] = gabi::fmuls_ppc(x, sx);
    emtrInfo.mVelOmni[1] = gabi::fmuls_ppc(y, sy);
    emtrInfo.mVelOmni[2] = gabi::fmuls_ppc(z, sz);
    emtrInfo.mVelAxis[0] = x;
    emtrInfo.mVelAxis[1] = 0.0f;
    emtrInfo.mVelAxis[2] = z;
}
// HD: capture the callback and guest target independently on each invocation.
void emitterCallback(JPABaseEmitter *e, u32 slot) {
    EmitterCallback* cb = e->mpEmitterCallBack;
    if (cb)
        gabi::call_ptr<void>(cb->__vtbl->slots[slot / 4], cb, e);
}
/* 028200C4 */
bool jpa_emitter_calc::JPABaseEmitter::doStartFrameProcess() {
    f32 time = mTime;
    if (!(time < (f32)(s16)mStartFrame))
        return true;
    if (!((u32)mFlags & JPAEmtrStts_StopCalc)) {
        f32 next = time + 1.0f;
        mTime = next < 0.0f ? 0.0f : next;
    }
    return false;
}
/* 0282014C */
bool jpa_emitter_calc::JPABaseEmitter::doTerminationProcess() {
    // HD: deletion waits two frames once the particle lists become empty.
    u8 delay = mDeleteCountdown;
    if (delay) {
        delay--;
        mDeleteCountdown = delay;
        return delay == 0;
    }
    s32 max = mMaxFrame;
    bool empty = false;
    if (max < 0) {
        mFlags = (u32)mFlags | JPAEmtrStts_EnableDeleteEmitter;
        empty = (u32)mActiveParticleCount + (u32)mChildParticleCount == 0;
    } else if (max > 0 && !((f32)mTick < (f32)max)) {
        u32 status = (u32)mFlags | JPAEmtrStts_EnableDeleteEmitter;
        mFlags = status;
        if (status & JPAEmtrStts_Immortal)
            return false;
        empty = (u32)mActiveParticleCount + (u32)mChildParticleCount == 0;
    }
    if (empty && !(u8)mDeleteCountdown) {
        mDeleteCountdown = 2;
        return false;
    }
    return empty;
}
/* 0281FE40 */
void jpa_emitter_calc::JPABaseEmitter::calc() {
    emtrInfo.mVolumeEmitCount = 0;
    if ((u32)mFlags & JPAEmtrStts_StopCalc) {
        emitterCallback(this, CallbackExecute);
        emitterCallback(this, CallbackExecuteAfter);
        return;
    }
    // HD: defer calculation while the teardown/clear countdowns are active.
    if ((u8)mDeleteCountdown)
        return;
    u8 clear = mClearCountdown;
    if (clear) {
        mClearCountdown = --clear;
        if (!clear)
            JPADeleteAllParticleHD(this, 0);
        return;
    }
    JPACalcKey(this);
    emitterCallback(this, CallbackExecute);
    JPACalcEmitterInfo(this);
    JPADrawCalc(mDraw);
    JPAFieldPreCalc(mFieldManager);
    if (!((u32)mFlags & JPAEmtrStts_EnableDeleteEmitter))
        JPACalcCreatePtcls(this);
    emitterCallback(this, CallbackExecuteAfter);
    JPACalcParticle(this);
    JPACalcChild(this);
    f32 tick = (f32)mTick + 1.0f;
    mTick = tick < 0.0f ? 0.0f : tick;
}
/* 0282167C */
void jpa_emitter_calc::JPAEmitterManager::calc(u8 group) {
    JPAEmitterManager* manager = this;
    if (group >= 16)
        JPAAssert(assertFile, 0x108, assertMessage);
    EmitterLink* link = manager->mEmtrGroup[group].first;
    while (link) {
        JPABaseEmitter* emitter = link->object;
        link = link->next;
        if (emitter->doStartFrameProcess()) {
            if (emitter->doTerminationProcess())
                JPADeleteEmitter(manager, emitter);
            else
                emitter->calc();
        }
    }
}

/* 0281F878 */
void jpa_emitter_calc::JPABaseEmitter::calcCreatePtcls() {
    if ((u32)mFlags & JPAEmtrStts_RateStepEmit) {
        s32 count = 0;
        if ((u32)mDataFlag & JPADynFlag_FixedInterval) {
            u32 div = mDivNumber;
            count = (u8)mVolumeType == 1 ? (s32)((div + 1) * (div - 1) * 4 + 6) : (s32)div;
            emtrInfo.mVolumeEmitIdx = 0;
        } else {
            f32 r = randomFloat(this);
            r = (r + r) - 1.0f;
            f32 increment =
                gabi::fmuls_ppc((f32)mRate, gabi::fmadds((f32)mRateRndm, r, 1.0f));
            f32 counter = gabi::fadds_ppc((f32)mEmitCount, increment);
            if (!(counter < 1.0f)) {
                count = gabi::ftoi(counter);
                mEmitCount = counter - (f32)count;
            } else {
                mEmitCount = counter;
                if (increment > 0.0f && ((u32)mFlags & JPAEmtrStts_FirstEmit))
                    count = 1;
            }
        }
        emtrInfo.mVolumeEmitCount = count;
        if ((u32)mFlags & JPAEmtrStts_StopEmit)
            count = 0;
        for (s32 i = 0; i < count; i++)
            if (!JPACreateParticle(this))
                break;
    }
    f32 limit = (f32)((u8)mRateStep + 1);
    f32 timer = (f32)mRateStepTimer + 1.0f;
    mRateStepTimer = timer;
    if (!(timer < limit)) {
        mRateStepTimer = timer - limit;
        mFlags = ((u32)mFlags | JPAEmtrStts_RateStepEmit) & ~u32(JPAEmtrStts_FirstEmit);
    } else
        mFlags = (u32)mFlags & ~u32(JPAEmtrStts_RateStepEmit | JPAEmtrStts_FirstEmit);
}
/* 0281F42C */
void jpa_emitter_calc::JPABaseEmitter::calcKey() {
    JPADataBlockLinkInfo* data = mpDataLinkInfo;
    for (u32 i = 0; i < (u8)data->keyNum; i++) {
        JPAKeyBlock* key = data->keyBlocks[i];
        u32 target = key->__vtbl->getKeyDataPtr;
        f32 tick = mTick;
        // HD: virtual scalar results retain their full register width.
        u32 ptr = gabi::call_ptr<u32>(target, key);
        u32 number = gabi::call_ptr<u32>(key->__vtbl->getNumber, key);
        if (gabi::call_ptr<u32>(key->__vtbl->isLoopEnable, key)) {
            s32 end = (s32)((u32)gabi::ftoi(gabi::at<KeyFrame>(ptr)[number - 1].time) + 1);
            u32 loops = (u32)divide(gabi::ftoi(tick), end);
            tick -= (f32)(s32)(loops * (u32)end);
        }
        f32 value = JPAGetKeyFrameValueHD(number, gabi::at<void>(ptr), tick);
        u32 id = gabi::call_ptr<u32>(key->__vtbl->getID, key);
        switch (id) {
        case 0:
            mRate = value;
            break;
        case 1:
            mVolumeSize = (u16)gabi::ftoi(value);
            break;
        case 2:
            mVolumeSweep = value;
            break;
        case 3:
            mVolumeMinRad = value;
            break;
        case 4:
            mLifeTime = (s16)gabi::ftoi(value);
            break;
        case 5:
            mMoment = value;
            break;
        case 6:
            mInitialVelOmni = value;
            break;
        case 7:
            mInitialVelAxis = value;
            break;
        case 8:
            mInitialVelDir = value;
            break;
        case 9:
            mSpread = value;
            break;
        case 10:
            mKeyScale = value;
            break;
        }
        data = mpDataLinkInfo;
    }
}
/* 0281DBEC */
void jpa_emitter_calc::JPABaseEmitter::calcEmitterGlobalPosition(void *dst) {
    gabi::Local<be<f32>[12]> matrix;
    MTXScale(matrix.get(), (f32)mGlobalDynamicsScale[0], (f32)mGlobalDynamicsScale[1],
                     (f32)mGlobalDynamicsScale[2]);
    MTXConcat(mGlobalRotation, matrix.get(), matrix.get());
    // HD: these are word copies; float assignment would quiet signaling NaNs.
    for (int i = 0; i < 3; i++)
        reinterpret_cast<be<u32>*>(matrix.get())[i * 4 + 3] = (u32)reinterpret_cast<be<u32>*>(mGlobalTranslation)[i];
    MTXMultVec(matrix.get(), mEmitterTranslation, dst);
}
/* 0281F688 */
void jpa_emitter_calc::JPABaseEmitter::calcEmitterInfo() {
    emtrInfo.mVolumeEmitXCount = 0;
    emtrInfo.mVolumeEmitAngleCount = 0;
    emtrInfo.mVolumeEmitAngleMax = 1;
    emtrInfo.mpCurEmitter = this;
    emtrInfo.mDivNumber = (u16)mDivNumber * 2 + 1;
    emtrInfo.mVolumeSize = (f32)(u16)mVolumeSize;
    gabi::Local<be<f32>[12]> scale, rotation, matrix;
    MTXScale(scale.get(), (f32)mEmitterScale[0], (f32)mEmitterScale[1],
                     (f32)mEmitterScale[2]);
    s16 x = ((s32)(s16)mEmitterRot[0] * 0x4000) / 90;
    s16 y = ((s32)(s16)mEmitterRot[1] * 0x4000) / 90;
    s16 z = ((s32)(s16)mEmitterRot[2] * 0x4000) / 90;
    JPAGetXYZRotateMtx(x, y, z, rotation.get());
    MTXScale(matrix.get(), (f32)mGlobalDynamicsScale[0], (f32)mGlobalDynamicsScale[1],
                     (f32)mGlobalDynamicsScale[2]);
    MTXConcat(mGlobalRotation, matrix.get(), matrix.get());
    // HD: these are word copies; float assignment would quiet signaling NaNs.
    for (int i = 0; i < 3; i++)
        reinterpret_cast<be<u32>*>(matrix.get())[i * 4 + 3] = (u32)reinterpret_cast<be<u32>*>(mGlobalTranslation)[i];
    MTXCopy(mGlobalRotation, &emtrInfo.mGlobalRot);
    MTXConcat(mGlobalRotation, rotation.get(), &emtrInfo.mEmitterGlobalRot);
    MTXConcat(&emtrInfo.mEmitterGlobalRot, scale.get(),
                     emtrInfo.mEmitterGlobalSR);
    JPAGetDirMtx(mEmitterDir, &emtrInfo.mEmitterDirMtx);
    for (int i = 0; i < 3; i++)
        emtrInfo.mEmitterGlobalScale[i] = gabi::fmuls_ppc((f32)mEmitterScale[i], (f32)mGlobalDynamicsScale[i]);
    // HD: these are word copies; float assignment would quiet signaling NaNs.
    for (int i = 0; i < 3; i++)
        reinterpret_cast<be<u32>*>(emtrInfo.mEmitterTranslation)[i] = (u32)reinterpret_cast<be<u32>*>(mEmitterTranslation)[i];
    // HD: these are word copies; float assignment would quiet signaling NaNs.
    for (int i = 0; i < 3; i++)
        reinterpret_cast<be<u32>*>(emtrInfo.mPublicScale)[i] = (u32)reinterpret_cast<be<u32>*>(mGlobalDynamicsScale)[i];
    MTXMultVec(matrix.get(), mEmitterTranslation, emtrInfo.mEmitterGlobalCenter);
}
/* 02820244 */
void jpa_emitter_calc::JPABaseEmitter::calcVolumePoint() {
    emtrInfo.mVolumePos[2] = 0.0f;
    emtrInfo.mVolumePos[1] = 0.0f;
    emtrInfo.mVolumePos[0] = 0.0f;
    f32 x = randomFloat(this) - 0.5f, y = randomFloat(this) - 0.5f, z = randomFloat(this) - 0.5f;
    emtrInfo.mVelOmni[0] = x;
    emtrInfo.mVelOmni[1] = y;
    emtrInfo.mVelOmni[2] = z;
    x = randomFloat(this) - 0.5f;
    z = randomFloat(this) - 0.5f;
    emtrInfo.mVelAxis[0] = x;
    emtrInfo.mVelAxis[1] = 0.0f;
    emtrInfo.mVelAxis[2] = z;
}
/* 02820354 */
void jpa_emitter_calc::JPABaseEmitter::calcVolumeLine() {
    f32 z;
    if ((u32)mDataFlag & JPADynFlag_FixedInterval) {
        u32 idx = (u32)emtrInfo.mVolumeEmitIdx;
        f32 count = (f32)(s32)emtrInfo.mVolumeEmitCount;
        f32 size = (f32)emtrInfo.mVolumeSize;
        emtrInfo.mVolumeEmitIdx = idx + 1;
        z = gabi::fmuls_ppc(size, ((f32)(s32)idx / (count - 1.0f)) - 0.5f);
    } else {
        f32 r = randomFloat(this) - 0.5f;
        z = gabi::fmuls_ppc(r, (f32)emtrInfo.mVolumeSize);
    }
    volumeResult(0.0f, 0.0f, z);
}
/* 02820688 */
void jpa_emitter_calc::JPABaseEmitter::calcVolumeCube() {
    f32 x = randomFloat(this) - 0.5f, y = randomFloat(this) - 0.5f, z = randomFloat(this) - 0.5f;
    f32 size = (f32)emtrInfo.mVolumeSize;
    volumeResult(gabi::fmuls_ppc(size, x), gabi::fmuls_ppc(size, y), gabi::fmuls_ppc(size, z));
}
/* 028204A0 */
void jpa_emitter_calc::JPABaseEmitter::calcVolumeCircle() {
    s16 angle;
    if ((u32)mDataFlag & JPADynFlag_FixedInterval) {
        u32 idx = (u32)emtrInfo.mVolumeEmitIdx;
        s16 step = divide((s32)(idx << 16), (s32)emtrInfo.mVolumeEmitCount);
        f32 sweep = mVolumeSweep;
        emtrInfo.mVolumeEmitIdx = idx + 1;
        angle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)step, sweep));
    } else {
        s16 r = randomShort(this);
        angle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)r, (f32)mVolumeSweep));
    }
    // HD: keep fused interpolation and the original double-precision density expression.
    f32 r = randomFloat(this), min = mVolumeMinRad;
    if ((u32)mDataFlag & JPADynFlag_FixedDensity)
        r = (f32)(1.0 - (double)r * r);
    f32 radius = gabi::fmuls_ppc(gabi::fmadds(r, 1.0f - min, min), (f32)emtrInfo.mVolumeSize);
    volumeResult(gabi::fmuls_ppc(sinAngle(angle), radius), 0.0f,
                 gabi::fmuls_ppc(cosAngle(angle), radius));
}
/* 02820778 */
void jpa_emitter_calc::JPABaseEmitter::calcVolumeSphere() {
    s16 x, angle;
    if ((u32)mDataFlag & JPADynFlag_FixedInterval) {
        u32 a = (u32)emtrInfo.mVolumeEmitAngleCount, maximum = (u32)emtrInfo.mVolumeEmitAngleMax;
        u32 latitude = (u32)emtrInfo.mVolumeEmitXCount, div = (u32)emtrInfo.mDivNumber;
        u16 step = divide((s32)(a << 16), (s32)maximum);
        x = (s16)((u32)divide((s32)(latitude << 15), (s32)(div - 1)) + 0x4000);
        angle = (s16)gabi::ftoi(gabi::fmadds((f32)step, (f32)mVolumeSweep, 32768.0f));
        if (++a == maximum) {
            latitude = (u32)emtrInfo.mVolumeEmitXCount + 1;
            div = (u32)emtrInfo.mDivNumber;
            emtrInfo.mVolumeEmitAngleCount = 0;
            emtrInfo.mVolumeEmitXCount = latitude;
            emtrInfo.mVolumeEmitAngleMax = (s32)(latitude * 2) < (s32)div
                                      ? maximum + (maximum == 1 ? 3 : 4)
                                      : (maximum == 4 ? 1 : maximum - 4);
        } else
            emtrInfo.mVolumeEmitAngleCount = a;
    } else {
        s16 r = randomShort(this);
        s16 r2 = randomShort(this);
        x = r >> 1;
        angle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)r2, (f32)mVolumeSweep));
    }
    // HD: keep fused interpolation and the original double-precision density expression.
    f32 r = randomFloat(this), min = mVolumeMinRad;
    if ((u32)mDataFlag & JPADynFlag_FixedDensity) {
        f32 square = r * r;
        r = (f32)(1.0 - (double)square * r);
    }
    f32 radius = gabi::fmuls_ppc(gabi::fmadds(r, 1.0f - min, min), (f32)emtrInfo.mVolumeSize);
    f32 side = gabi::fmuls_ppc(radius, cosAngle(x));
    volumeResult(gabi::fmuls_ppc(side, sinAngle(angle)), -gabi::fmuls_ppc(radius, sinAngle(x)),
                 gabi::fmuls_ppc(side, cosAngle(angle)));
}
/* 02820A84 */
void jpa_emitter_calc::JPABaseEmitter::calcVolumeCylinder() {
    s16 rangle = randomShort(this);
    s16 angle = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)rangle, (f32)mVolumeSweep));
    // HD: keep fused interpolation and the original double-precision density expression.
    f32 r = randomFloat(this), min = mVolumeMinRad;
    if ((u32)mDataFlag & JPADynFlag_FixedDensity)
        r = (f32)(1.0 - (double)r * r);
    f32 radius = gabi::fmuls_ppc(gabi::fmadds(r, 1.0f - min, min), (f32)emtrInfo.mVolumeSize);
    f32 y = randomFloat(this);
    y = (y + y) - 1.0f;
    volumeResult(gabi::fmuls_ppc(radius, sinAngle(angle)),
                 gabi::fmuls_ppc((f32)emtrInfo.mVolumeSize, y),
                 gabi::fmuls_ppc(radius, cosAngle(angle)));
}
/* 02820BE0 */
void jpa_emitter_calc::JPABaseEmitter::calcVolumeTorus() {
    s16 r = randomShort(this);
    s16 a = (s16)gabi::ftoi(gabi::fmuls_ppc((f32)r, (f32)mVolumeSweep));
    u16 b = randomShort(this);
    f32 size = (f32)emtrInfo.mVolumeSize;
    f32 radius = gabi::fmuls_ppc((f32)mVolumeMinRad, size);
    f32 sa = sinAngle(a), ca = cosAngle(a), sb = sinAngle(b), cb = cosAngle(b);
    f32 axisX = gabi::fmuls_ppc(gabi::fmuls_ppc(radius, sa), cb);
    f32 axisY = gabi::fmuls_ppc(radius, sb);
    f32 axisZ = gabi::fmuls_ppc(gabi::fmuls_ppc(radius, ca), cb);
    emtrInfo.mVelAxis[0] = axisX;
    emtrInfo.mVelAxis[1] = axisY;
    emtrInfo.mVelAxis[2] = axisZ;
    f32 x = gabi::fmadds(size, sa, axisX), z = gabi::fmadds(size, ca, axisZ);
    emtrInfo.mVolumePos[0] = x;
    emtrInfo.mVolumePos[1] = axisY;
    emtrInfo.mVolumePos[2] = z;
    emtrInfo.mVelOmni[0] = gabi::fmuls_ppc(x, (f32)emtrInfo.mEmitterGlobalScale[0]);
    emtrInfo.mVelOmni[1] = gabi::fmuls_ppc(axisY, (f32)emtrInfo.mEmitterGlobalScale[1]);
    emtrInfo.mVelOmni[2] = gabi::fmuls_ppc(z, (f32)emtrInfo.mEmitterGlobalScale[2]);
}

} // namespace jpa_emitter_calc
using namespace jpa_emitter_calc;

/* WWHD glue (tools/verify/autoglue.py)
data emtrInfo 104B5730
data sincosTable 104A44F8 nocheck
data assertFile 10170E84 nocheck
data assertMessage 10170E9C nocheck
ext 0281DE68 void JPADeleteAllParticleHD(jpa_emitter_calc::JPABaseEmitter*, u32)
ext 0281F42C void JPACalcKey(jpa_emitter_calc::JPABaseEmitter*)
ext 0281F688 void JPACalcEmitterInfo(jpa_emitter_calc::JPABaseEmitter*)
ext 0282DC48 void JPADrawCalc(void*)
ext 02824294 void JPAFieldPreCalc(void*)
ext 0281F878 void JPACalcCreatePtcls(jpa_emitter_calc::JPABaseEmitter*)
ext 0281FBD8 void JPACalcParticle(jpa_emitter_calc::JPABaseEmitter*)
ext 0281FD14 void JPACalcChild(jpa_emitter_calc::JPABaseEmitter*)
ext 028215F8 void JPADeleteEmitter(jpa_emitter_calc::JPAEmitterManager*, jpa_emitter_calc::JPABaseEmitter*)
ext 0281DCB8 void* JPACreateParticle(jpa_emitter_calc::JPABaseEmitter*)
ext 028249D8 f32 JPAGetKeyFrameValueHD(u32, void*, f32)
ext 028E945C void MTXScale(void*, f32, f32, f32)
ext 028E9108 void MTXConcat(void*, void*, void*)
ext 028E90D4 void MTXCopy(void*, void*)
ext 028E8F64 void MTXMultVec(void*, void*, void*)
ext 028245AC void JPAGetXYZRotateMtx(s16, s16, s16, void*)
ext 0282466C void JPAGetDirMtx(void*, void*)
ext 0273AA24 void JPAAssert(void*, s32, void*)
*/

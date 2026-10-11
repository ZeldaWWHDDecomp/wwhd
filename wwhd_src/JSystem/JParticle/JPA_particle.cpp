// Particle motion and emitter lifecycle; verified against the WWHD routines.
#include "JSystem/JParticle/JPA_particle.h"
#include <climits>
#include <cstring>

static JPAEmitterInfo emtrInfo;
static JPAFieldWorkspace fieldWork;

static s32 divide(s32 a, s32 b) {
    // HD: retain the PPC divide result for exceptional operands.
    return !b || (a == INT_MIN && b == -1) ? (a < 0 ? -1 : 0) : a / b;
}

// HD: the emitter loops share the deferred-deletion lifecycle.
static void calcParticles(JPABaseEmitter* emitter, bool children) {
    auto* list = children ? &emitter->mChildParticles : &emitter->mActiveParticles;
    auto* link = list->mpHead.get();
    while (link) {
        auto* p = link->mpObject.get();
        auto* next = link->mpNext.get();
        u8 delay = p->mDeletionDelay;
        if (delay) {
            p->mDeletionDelay = --delay;
            if (!delay)
                emitter->releaseParticle(p, list);
        } else {
            p->incFrame();
            u32 status = p->mStatus;
            if (status & JPAPtclStts_UNK_80)
                p->mStatus = status | JPAPtclStts_Delete;
            else {
                if (!children || gabi::ftoi(p->mCurFrame) != 0)
                    p->calcVelocity();
                auto* cb = p->mpCallBack2.get();
                if (cb)
                    gabi::call_ptr<void>(cb->mpVtable->execute.get(), cb, emitter, p);
                if (!((u32)p->mStatus & JPAPtclStts_Delete)) {
                    if (children)
                        JPADraw_calcChild(&emitter->mDraw, p);
                    else
                        JPADraw_calcParticle(&emitter->mDraw, p);
                    if (!children && emitter->mpDataLinkInfo->mpSweepShape.get() && p->checkCreateChild())
                        emitter->createChildren(p);
                    p->calcPosition();
                }
            }
            if ((u32)p->mStatus & JPAPtclStts_Delete)
                emitter->deleteParticle(p);
        }
        link = next;
    }
}

/* inlined into JPABaseParticle::initParticle and JPABaseParticle::initChild */
static u32 particleRandomNext(JPABaseEmitter* emtr) {
    u32 seed = (u32)emtr->mRandomSeed * 0x19660du + 0x3c6ef35fu;
    emtr->mRandomSeed = seed;
    return seed;
}

/* inlined into JPABaseParticle::initParticle and JPABaseParticle::initChild */
static f32 particleRandom(JPABaseEmitter* emtr) {
    u32 bits = 0x3f800000u | (particleRandomNext(emtr) >> 9);
    f32 value;
    std::memcpy(&value, &bits, 4);
    return value - 1.0f;
}

/* inlined into JPABaseParticle::initParticle and JPABaseParticle::initChild */
static f32 particleRandomRF(JPABaseEmitter* emtr) {
    f32 value = particleRandom(emtr);
    return gabi::fsubs_ppc(gabi::fadds_ppc(value, value), 1.0f);
}

/* inlined into JPABaseParticle::initParticle and JPABaseParticle::initChild */
static f32 lengthSquared(f32 x, f32 y, f32 z) {
    return gabi::fmadds(z, z, gabi::fmadds(x, x, gabi::fmuls_ppc(y, y)));
}

/* inlined into JPABaseParticle::initParticle and JPABaseParticle::initChild */
static f32 inverseLength(f32 squared) {
    return 1.0f / (f32)JPA_sqrt((f64)squared);
}

// HD: GPU setup and callbacks follow the GameCube particle initialization.
static void particleInitTail(JPABaseParticle* p, JPABaseEmitter* emtr, JPAShape* sweep, bool child) {
    p->mDeletionDelay = 0;
    p->mHidden = 0;
    auto* gpu = p->mpGpuState.get();
    gpu->mPendingCommands[0] = 0;
    gpu->mPendingCommands[1] = 0;
    bool useSlot = child;
    if (!child) {
        auto* shape = emtr->mpDataLinkInfo->mpSweepShape.get();
        useSlot = !shape || gabi::call_ptr<u32>(shape->mpVtable->isEnableDrawParent.get(), shape);
    }
    if (useSlot) {
        auto* shape = child ? sweep : emtr->mpDataLinkInfo->mpBaseShape.get();
        s32 type = gabi::call_ptr<s32>(shape->mpVtable->getType.get(), shape);
        bool allocate = type == 5;
        if (!allocate) {
            shape = child ? sweep : emtr->mpDataLinkInfo->mpBaseShape.get();
            allocate = gabi::call_ptr<s32>(shape->mpVtable->getType.get(), shape) == 6;
        }
        if (allocate) {
            JPA_allocateParticleSlot(emtr, (s32)child);
            p->mpSlot = emtr->mpSlots[child];
        }
    }
    JPA_particleInitGpu(p, emtr);
    auto* cb = p->mpCallBack2.get();
    if (cb)
        gabi::call_ptr<void>(cb->mpVtable->init.get(), cb, emtr, p);
    JPA_particleInitField(p, &fieldWork);
    JPA_particleInitFieldDraw(p, &fieldWork);
}

/* 028258B0 */
void JPABaseParticle::initParticle() {
    auto* emtr = emtrInfo.mpCurEmitter.get();
    mpEmitter = emtr;
    for (u32 i = 0; i < 3; ++i)
        mFieldAccel[i] = 0.0f;
    mFieldDrag = 1.0f;
    mDrag = 1.0f;
    mStatus = JPAPtclStts_FirstFrame;
    PSMTXMultVec(emtrInfo.mEmitterGlobalSR, emtrInfo.mVolumePos, mLocalPosition);
    if (emtr->mDataFlag & JPADynFlag_FollowEmtr)
        mStatus = (u32)mStatus | JPAPtclStts_UNK_20;
    for (u32 i = 0; i < 3; ++i)
        mOffsetPosition[i] = (f32)emtrInfo.mEmitterGlobalCenter[i];
    f32 velOmni[3]{}, velAxis[3]{}, velDir[3]{}, velRndm[3]{};
    for (u32 kind = 0; kind < 2; ++kind) {
        f32 speed = kind ? emtr->mInitialVelAxis : emtr->mInitialVelOmni;
        auto* vec = kind ? emtrInfo.mVelAxis : emtrInfo.mVelOmni;
        f32 squared = lengthSquared((f32)vec[0], (f32)vec[1], (f32)vec[2]);
        if (speed != 0.0f && squared > 0x1p-18f) {
            f32 scale = gabi::fmuls_ppc(inverseLength(squared), speed);
            for (u32 i = 0; i < 3; ++i)
                (kind ? velAxis : velOmni)[i] = gabi::fmuls_ppc((f32)vec[i], scale);
        }
    }
    if (emtr->mInitialVelDir != 0.0f) {
        f32 angle = gabi::fmuls_ppc(gabi::fmuls_ppc(32768.0f, particleRandomRF(emtr)), emtr->mSpread);
        s16 a = (s16)gabi::ftoi(angle), b = (s16)(particleRandomNext(emtr) >> 16);
        gabi::Local<be<f32>[12]> matrix;
        JPAGetYZRotateMtx(a, b, *matrix);
        PSMTXConcat(emtrInfo.mEmitterDirMtx, *matrix, *matrix);
        for (u32 i = 0; i < 3; ++i)
            velDir[i] = gabi::fmuls_ppc((*matrix)[i * 4 + 2], emtr->mInitialVelDir);
    }
    if (emtr->mInitialVelRndm != 0.0f) {
        f32 r[3];
        for (auto& v : r)
            v = particleRandom(emtr) - 0.5f;
        f32 speed = emtr->mInitialVelRndm;
        for (u32 i = 0; i < 3; ++i)
            velRndm[i] = gabi::fmuls_ppc(r[i], speed);
    }
    f32 ratioRandom = particleRandomRF(emtr);
    f32 ratio = gabi::fmadds(emtr->mInitialVelRatio, ratioRandom, 1.0f);
    for (u32 i = 0; i < 3; ++i) {
        f32 sum = gabi::fadds_ppc(gabi::fadds_ppc(gabi::fadds_ppc(velOmni[i], velAxis[i]), velDir[i]), velRndm[i]);
        mBaseVel[i] = gabi::fmuls_ppc(sum, ratio);
    }
    if (emtr->mDataFlag & JPADynFlag_InheritScale)
        for (u32 i = 0; i < 3; ++i)
            mBaseVel[i] = gabi::fmuls_ppc(mBaseVel[i], (f32)emtr->mEmitterScale[i]);
    PSMTXMultVec(emtrInfo.mEmitterGlobalRot, mBaseVel, mBaseVel);
    f32 accelRandom = particleRandomRF(emtr);
    f32 accel = gabi::fmuls_ppc(emtr->mAccel, gabi::fmadds(emtr->mAccelRndm, accelRandom, 1.0f));
    f32 squared = lengthSquared(mBaseVel[0], mBaseVel[1], mBaseVel[2]);
    if (squared > 0x1p-18f) {
        f32 scale = gabi::fmuls_ppc(inverseLength(squared), accel);
        for (u32 i = 0; i < 3; ++i)
            mAccel[i] = gabi::fmuls_ppc(mBaseVel[i], scale);
    } else
        for (u32 i = 0; i < 3; ++i)
            mAccel[i] = 0.0f;
    f32 airRandom = particleRandom(emtr) - 0.5f;
    f32 air = gabi::fmadds(emtr->mAirResistRndm, airRandom, emtr->mAirResist);
    mAirResist = air;
    if (air > 1.0f)
        mAirResist = 1.0f;
    f32 momentRandom = particleRandom(emtr);
    mMoment = gabi::fmuls_ppc(emtr->mMoment, gabi::fnmsubs(emtr->mMomentRndm, momentRandom, 1.0f));
    mCurFrame = -1.0f;
    f32 lifetimeRandom = particleRandom(emtr);
    mLifeTime = gabi::fmuls_ppc((f32)emtr->mLifeTime, gabi::fnmsubs(emtr->mLifeTimeRndm, lifetimeRandom, 1.0f));
    mCurNormTime = 0.0f;
    for (u32 i = 0; i < 3; ++i)
        mGlobalPosition[i] = gabi::fmadds(mLocalPosition[i], (f32)emtrInfo.mPublicScale[i], mOffsetPosition[i]);
    mpCallBack2 = emtr->mpParticleCallBack;
    JPAFieldManager_init(&emtr->mFieldManager, this);
    mChildState = 0;
    particleInitTail(this, emtr, nullptr, false);
}

/* 028260B4 */
void JPABaseParticle::initChild(JPABaseParticle* parent) {
    auto* emtr = emtrInfo.mpCurEmitter.get();
    auto* sweep = emtr->mpDataLinkInfo->mpSweepShape.get();
    mpEmitter = emtr;
    mStatus = JPAPtclStts_FirstFrame | JPAPtlcStts_Child;
    if (!gabi::call_ptr<u32>(sweep->mpVtable->isEnableField.get(), sweep)) {
        mFieldDrag = 1.0f;
        mDrag = 1.0f;
        mStatus = (u32)mStatus | JPAPtclStts_UNK_40;
    } else {
        mDrag = parent->mDrag;
        mFieldDrag = parent->mFieldDrag;
    }
    f32 randomRatio = particleRandomRF(emtr);
    auto* table = sweep->mpVtable.get();
    f32 baseVel = gabi::call_ptr<f32>(table->getBaseVel.get(), sweep);
    f32 baseRandom = gabi::call_ptr<f32>(table->getBaseVelRndm.get(), sweep);
    f32 speed = gabi::fmuls_ppc(baseVel, gabi::fmadds(baseRandom, randomRatio, 1.0f));
    f32 velRndm[3];
    for (auto& v : velRndm)
        v = particleRandomRF(emtr);
    f32 squared = lengthSquared(velRndm[0], velRndm[1], velRndm[2]);
    if (squared > 0x1p-18f) {
        f32 scale = gabi::fmuls_ppc(inverseLength(squared), speed);
        for (auto& v : velRndm)
            v = gabi::fmuls_ppc(v, scale);
    }
    f32 influence = gabi::call_ptr<f32>(sweep->mpVtable->getVelInfRate.get(), sweep);
    for (u32 i = 0; i < 3; ++i)
        mBaseVel[i] = gabi::fmadds(parent->mBaseVel[i], influence, velRndm[i]);
    influence = gabi::call_ptr<f32>(sweep->mpVtable->getVelInfRate.get(), sweep);
    for (u32 i = 0; i < 3; ++i)
        mFieldAccel[i] = gabi::fmuls_ppc(parent->mFieldVel[i], influence);
    f32 gravity = gabi::call_ptr<f32>(sweep->mpVtable->getGravity.get(), sweep);
    mAccel[0] = 0.0f;
    mAccel[1] = -gravity;
    mAccel[2] = 0.0f;
    mAirResist = parent->mAirResist;
    mMoment = parent->mMoment;
    mCurFrame = -1.0f;
    s32 life = gabi::call_ptr<s32>(sweep->mpVtable->getLife.get(), sweep);
    mLifeTime = (f32)life;
    mCurNormTime = 0.0f;
    f32 scale = gabi::fmuls_ppc(mMoment, mDrag);
    for (u32 i = 0; i < 3; ++i) {
        f32 field = mFieldAccel[i];
        mFieldVel[i] = field;
        mVelocity[i] = gabi::fmuls_ppc(gabi::fadds_ppc(mBaseVel[i], field), scale);
    }
    if (emtr->mDataFlag & JPADynFlag_FollowEmtrChld)
        mStatus = (u32)mStatus | JPAPtclStts_UNK_20;
    for (u32 i = 0; i < 3; ++i)
        mOffsetPosition[i] = parent->mOffsetPosition[i];
    for (u32 i = 0; i < 3; ++i)
        mLocalPosition[i] = parent->mLocalPosition[i];
    f32 positionRandom = gabi::call_ptr<f32>(sweep->mpVtable->getPosRndm.get(), sweep);
    if (positionRandom != 0.0f) {
        s16 a = (s16)(particleRandomNext(emtr) >> 16), b = (s16)(particleRandomNext(emtr) >> 16);
        gabi::Local<be<f32>[3]> unit;
        JPAGetUnitVec(a, b, *unit);
        f32 amount = gabi::fmuls_ppc(particleRandom(emtr), positionRandom);
        for (u32 i = 0; i < 3; ++i) {
            (*unit)[i] = gabi::fmuls_ppc((*unit)[i], amount);
            mLocalPosition[i] = gabi::fadds_ppc(mLocalPosition[i], (*unit)[i]);
        }
    }
    mpCallBack2 = emtr->mpParticleCallBack;
    mChildState = parent->mChildState;
    particleInitTail(this, emtr, sweep, true);
}

/* 028266C0 */
void JPABaseParticle::incFrame() {
    f32 age = gabi::fadds_ppc(mCurFrame, 1.0f);
    if (age < 0.0f)
        age = 0.0f;
    f32 lifetime = mLifeTime;
    mStatus = (u32)mStatus & ~JPAPtclStts_FirstFrame;
    mCurFrame = age;
    // HD: unordered ages also expire.
    if (!(age < lifetime)) {
        mCurNormTime = 1.0f;
        mStatus = (u32)mStatus | JPAPtclStts_Delete;
    } else
        mCurNormTime = age / lifetime;
}

/* 02826720 */
void JPABaseParticle::calcVelocity() {
    mFieldVel[0] = 0.0f;
    mFieldVel[1] = 0.0f;
    mFieldVel[2] = 0.0f;
    if ((u32)mStatus & JPAPtclStts_UNK_20)
        for (u32 i = 0; i < 3; ++i)
            mOffsetPosition[i] = emtrInfo.mEmitterGlobalCenter[i];
    for (u32 i = 0; i < 3; ++i)
        mBaseVel[i] = gabi::fadds_ppc(mBaseVel[i], mAccel[i]);
    if (!((u32)mStatus & JPAPtclStts_UNK_40))
        JPAFieldManager_calc(&emtrInfo.mpCurEmitter->mFieldManager, this);
    f32 air = mAirResist, scale = gabi::fmuls_ppc(mMoment, mDrag);
    for (u32 i = 0; i < 3; ++i) {
        f32 base = gabi::fmuls_ppc(mBaseVel[i], air), field = gabi::fadds_ppc(mFieldVel[i], mFieldAccel[i]);
        mBaseVel[i] = base;
        mFieldVel[i] = field;
        mVelocity[i] = gabi::fmuls_ppc(gabi::fadds_ppc(base, field), scale);
    }
}

/* 0282685C */
void JPABaseParticle::calcPosition() {
    for (u32 i = 0; i < 3; ++i) {
        f32 local = gabi::fadds_ppc(mLocalPosition[i], mVelocity[i]);
        mLocalPosition[i] = local;
        mGlobalPosition[i] = gabi::fmadds(local, emtrInfo.mPublicScale[i], mOffsetPosition[i]);
    }
}

/* 028268C8 */
bool JPABaseParticle::checkCreateChild() {
    auto* sweep = emtrInfo.mpCurEmitter->mpDataLinkInfo->mpSweepShape.get();
    f32 lifetime = mLifeTime, time = 1.0f;
    auto timingTarget = sweep->mpVtable->getTiming.get();
    if (lifetime > 1.0f)
        time = (f32)mCurFrame / (lifetime - 1.0f);
    f32 timing = gabi::call_ptr<f32>(timingTarget, sweep);
    if (time < timing)
        return false;
    auto stepTarget = sweep->mpVtable->getStep.get();
    s32 age = gabi::ftoi(mCurFrame);
    u32 step = gabi::call_ptr<u32>(stepTarget, sweep) + 1;
    return (u32)age - (u32)divide(age, (s32)step) * step == 0;
}

/* 0281FB08 */
void JPABaseEmitter::createChildren(JPABaseParticle* parent) {
    auto* sweep = mpDataLinkInfo->mpSweepShape.get();
    s32 count = gabi::call_ptr<s32>(sweep->mpVtable->getRate.get(), sweep);
    while (count > 0) {
        auto* particle = getPtclFromVacList();
        if (!particle)
            break;
        JSUPtrList_prepend(&mChildParticles, &particle->mLink);
        particle->initChild(parent);
        JPADraw_initChild(&mDraw, parent, particle);
        --count;
    }
}

/* 0281DCB8 */
JPABaseParticle* JPABaseEmitter::createParticle() {
    auto* particle = getPtclFromVacList();
    if (!particle)
        return nullptr;
    JSUPtrList_prepend(&mActiveParticles, &particle->mLink);
    (this->*mVolumeFunc)();
    particle->initParticle();
    JPADraw_initParticle(&mDraw, particle);
    return particle;
}

/* 0281FBD8 */
void JPABaseEmitter::calcParticle() {
    calcParticles(this, false);
}

/* 0281FD14 */
void JPABaseEmitter::calcChild() {
    calcParticles(this, true);
}

/* 0281FBAC */
void JPABaseEmitter::deleteParticle(JPABaseParticle* p) {
    // HD: the renderer keeps a hidden particle alive for two more frames.
    if ((u8)p->mDeletionDelay)
        return;
    u8 delay = p->mDeletionDelay;
    p->mHidden = 1;
    if (!delay)
        p->mDeletionDelay = 2;
}

/* 0281DE68 */
void JPABaseEmitter::deleteAllParticle(s32 deferred) {
    if (deferred) {
        JPA_deleteAllDeferred(this);
        return;
    }
    // HD: release both GPU-backed deferred batches before the linked lists.
    for (auto& batch : mDeferredParticles) {
        auto* data = batch.mpData.get();
        // HD: the vector end is computed with wrapping 32-bit byte arithmetic.
        u32 count = ((u32)batch.mSize * (u32)sizeof(*data)) / (u32)sizeof(*data);
        for (u32 index = 0; index < count; ++index) {
            auto* cursor = &data[index];
            auto* particle = cursor->get();
            for (u32 i = 1; i <= 14; ++i)
                JPA_rendererRelease(particle->mpRenderer.get(), i);
            JPA_particleDestroy(cursor->get());
        }
        JPA_clearParticleVector(&batch);
    }
    auto* link = mActiveParticles.mpHead.get();
    mDeletePending = 0;
    for (auto* list : {&mActiveParticles, &mChildParticles}) {
        if (list != &mActiveParticles)
            link = list->mpHead.get();
        while (link) {
            auto* particle = link->mpObject.get();
            for (u32 i = 1; i <= 14; ++i)
                JPA_rendererRelease(particle->mpRenderer.get(), i);
            JPA_particleDestroy(link->mpObject.get());
            auto* next = link->mpNext.get();
            JSUPtrList_remove(list, link);
            JSUPtrList_prepend(mpPtclVacList.get(), link);
            link = next;
        }
    }
    if ((u8)mDeleteEnabled) {
        if (!(u8)mDeleteRequested)
            mDeleteRequested = 1;
        mDeleteEnabled = 0;
    }
}

/* 0281DC6C */
JPABaseParticle* JPABaseEmitter::getPtclFromVacList() {
    auto* list = mpPtclVacList.get();
    if (!(u32)list->mCount)
        return nullptr;
    auto* particle = list->mpHead->mpObject.get();
    JSUPtrList_remove(list, &particle->mLink);
    return particle;
}

/* 0281FA98 */
void JPABaseEmitter::releaseParticle(JPABaseParticle* p, JPAParticleList* list) {
    for (u32 i = 1; i <= 14; ++i)
        JPA_rendererRelease(p->mpRenderer.get(), i);
    JPA_particleDestroy(p);
    JSUPtrList_remove(list, &p->mLink);
    JSUPtrList_prepend(mpPtclVacList.get(), &p->mLink);
}

/* WWHD glue
data emtrInfo 104B5730
data fieldWork 104A01EC
ext 028242E8 void JPAFieldManager_calc(JPAFieldManager*, JPABaseParticle*)
ext 02824230 void JPAFieldManager_init(JPAFieldManager*, JPABaseParticle*)
ext 027FE3D4 void JPA_rendererRelease(JPAParticleRenderer*, u32)
ext 02825868 void JPA_particleDestroy(JPABaseParticle*)
ext 027EC560 void JSUPtrList_remove(JPAParticleList*, JPAParticleLink*)
ext 027EC778 void JSUPtrList_prepend(JPAParticleList*, JPAParticleLink*)
ext 0282DED0 void JPADraw_initParticle(JPADraw*, JPABaseParticle*)
ext 0282E770 void JPADraw_initChild(JPADraw*, JPABaseParticle*, JPABaseParticle*)
ext 0282DCBC void JPADraw_calcParticle(JPADraw*, JPABaseParticle*)
ext 0282DD50 void JPADraw_calcChild(JPADraw*, JPABaseParticle*)
ext 0281DD6C void JPA_deleteAllDeferred(JPABaseEmitter*)
ext 0273B5DC void JPA_clearParticleVector(JPAParticleVector*)
ext 0281E8C0 void JPA_allocateParticleSlot(JPABaseEmitter*, s32)
ext 02825750 void JPA_particleInitGpu(JPABaseParticle*, JPABaseEmitter*)
ext 02825430 void JPA_particleInitField(JPABaseParticle*, JPAFieldWorkspace*)
ext 02825514 void JPA_particleInitFieldDraw(JPABaseParticle*, JPAFieldWorkspace*)
ext 028E8F64 void PSMTXMultVec(be<f32>*, be<f32>*, be<f32>*)
ext 028E9108 void PSMTXConcat(be<f32>*, be<f32>*, be<f32>*)
ext 02824520 void JPAGetYZRotateMtx(s16, s16, be<f32>*)
ext 02824AA0 void JPAGetUnitVec(s16, s16, be<f32>*)
ext 028F37F0 f64 JPA_sqrt(f64)
*/

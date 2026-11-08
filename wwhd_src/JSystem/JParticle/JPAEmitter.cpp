// Companion emitter initialization and Cafe deferred/GPU state, using tww JPA1 names.
#include "JSystem/JParticle/JPAEmitter.h"
#include "sead/ElementAt.h"
#include "JSystem/JParticle/JPAMath.h"
#include "d/d_com_inf_game.h"
void* JPAEmitterNew(u32);
void JPAEmitterLinkConstruct(void*,void*);
void JPAEmitterListInitiate(void*);
void JPAEmitterVertexBaseConstruct(void*);
void JPAEmitterClearScratch(void*,u32);
void* JPAEmitterGpuArrayConstruct(void*);
void JPAEmitterVertexStateInit(void*, void*);
void JPAEmitterGpuArrayInit(void*,u32,void*);
void JPAEmitterDeferredInit(JPAParticleVector*,u32,void*,s32);
void JPAEmitterIdentity(Mtx34*);
void JPAEmitterCopyMatrix(const Mtx34*,Mtx34*);
void JPAEmitterMakeRotation(s16,s16,s16,Mtx34*);
void JPAEmitterConcat(const Mtx34*,const Mtx34*,Mtx34*);
void JPAEmitterTransform(const Mtx34*,const cXyz*,cXyz*);
void JPAEmitterRegister(void*);
void JPAEmitterRandomInit(void*,u32);
void JPAEmitterConstructArray(void*,u32,u32,u32);
void* JPAEmitterInfoVectorConstruct(void*);
f64 JPAEmitterSqrt(f64);
void JPAEmitterInitFields(void*,jpa_emitter_calc::JPADataBlockLinkInfo*,jpa_emitter_calc::JPAEmitterInfo*);
void JPAEmitterVolumeCube(jpa_emitter_calc::JPABaseEmitter*);
void JPAEmitterVolumeSphere(jpa_emitter_calc::JPABaseEmitter*);
void JPAEmitterVolumeCylinder(jpa_emitter_calc::JPABaseEmitter*);
void JPAEmitterVolumeTorus(jpa_emitter_calc::JPABaseEmitter*);
void JPAEmitterVolumePoint(jpa_emitter_calc::JPABaseEmitter*);
void JPAEmitterVolumeCircle(jpa_emitter_calc::JPABaseEmitter*);
void JPAEmitterVolumeLine(jpa_emitter_calc::JPABaseEmitter*);
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegistration;
static const u32 sEmitterStringVtable;
struct DefaultColor { be<u32> bits[4]; };
static DefaultColor sDefaultColor;
static const u32 sVertexStateVtable;
}

static void constructEmitterList(void* list) {
    if (gabi::ea(list) == 0) list = JPAEmitterNew(12);
    if (list) JPAEmitterListInitiate(list);
}
static void constructDeferredVector(JPAParticleVector* vector) {
    if (gabi::ea(vector) == 0) vector = static_cast<JPAParticleVector*>(JPAEmitterNew(12));
    if (vector) {
        vector->mCapacity = 0;
        vector->mSize = 0;
        vector->mpData = nullptr;
    }
}

/* 0281D92C */
jpa_emitter_extension::JPABaseEmitter* JPABaseEmitterConstruct(jpa_emitter_extension::JPABaseEmitter* self) {
    if (!self) {
        self = static_cast<jpa_emitter_extension::JPABaseEmitter*>(JPAEmitterNew(sizeof(jpa_emitter_extension::JPABaseEmitter)));
        if (!self) return nullptr;
    }
    if (gabi::ea(self->mEmitterScale) == 0) JPAEmitterNew(12);
    if (gabi::ea(self->mEmitterTranslation) == 0) JPAEmitterNew(12);
    if (gabi::ea(self->mEmitterRot) == 0) JPAEmitterNew(6);
    if (gabi::ea(self->mEmitterDir) == 0) JPAEmitterNew(12);
    void* link = self->_08C;
    if (gabi::ea(link) == 0) link = JPAEmitterNew(16);
    if (link) JPAEmitterLinkConstruct(link, self);
    self->mTick = 0.0f;
    self->mTime = 0.0f;
    constructEmitterList(self->mFieldManager);
    constructEmitterList(&self->particles().mActive);
    constructEmitterList(&self->particles().mChild);
    constructDeferredVector(&self->deferredParticles()[0]);
    constructDeferredVector(&self->deferredParticles()[1]);
    void* random = &self->mRandomSeed;
    if (gabi::ea(random) == 0) random = JPAEmitterNew(4);
    if (random) JPAEmitterRandomInit(random, 0);
    if (gabi::ea(self->mGlobalRotation) == 0) JPAEmitterNew(48);
    if (gabi::ea(self->mGlobalDynamicsScale) == 0) JPAEmitterNew(12);
    if (gabi::ea(self->mGlobalTranslation) == 0) JPAEmitterNew(12);
    if (gabi::ea(self->scaleAndColors().mGlobalParticleScale) == 0) JPAEmitterNew(12);
    // HD: GHS places the SafeString vtable after its character pointer.
    auto name = reinterpret_cast<SafeString*>(&self->gpu().mResourceName);
    if (gabi::ea(name) == 0) name = static_cast<SafeString*>(JPAEmitterNew(8));
    if (name) {
        name->__vtbl = gabi::ea(&sEmitterStringVtable);
        name->mTextPtr = gabi::guest_addr("");
    }
    self->gpu().mRenderPass = 0;
    self->gpu().mRenderEnabled = 1;
    self->gpu().mColorScale[3] = 1.0f;
    self->gpu().mColorScale[1] = 1.0f;
    self->gpu().mColorScale[2] = 1.0f;
    self->gpu().mColorScale[0] = 1.0f;
    self->gpu().mDrawContextOverride = 0;
    self->gpu().mApplyColorScale = 0;
    for (u32 row = 0; row < 4; ++row)
        for (u32 column = 0; column < 4; ++column)
            self->gpu().mColorBits[row][column] = column == 3 ? 0x3F800000u : 0;
    if (gabi::ea(&self->gpu().mRenderMatrix) == 0) JPAEmitterNew(48);
    JPAEmitterVertexBaseConstruct(&self->gpu().mVertexState);
    self->gpu().mVertexState.__vtbl = &sVertexStateVtable;
    JPAEmitterClearScratch(self->gpu().mVertexState.mScratch, 0x34);
    if (gabi::ea(self->gpu().mVertexState.mScratch) == 0) JPAEmitterNew(48);
    JPAEmitterConstructArray(self->gpu().mDynamicBuffers, 2, 12,
        gabi::guest_fn(reinterpret_cast<const void*>(JPAEmitterGpuArrayConstruct)));
    return self;
}

static void appendDeferred(JPAParticleVector* vector, JPAParticleLink* link) {
    while (link) {
        u32 size = vector->mSize;
        u32 capacity = vector->mCapacity;
        JPAParticleLink* next = link->mpNext;
        ::JPABaseParticle* particle = link->mpObject;
        if (s32(size) < s32(capacity)) {
            sead::elementAt(vector->mpData.get(), size) = particle;
            vector->mSize = u32(vector->mSize) + 1;
        }
        link = next;
    }
}

/* 0281DD6C */
void jpa_emitter_extension::JPABaseEmitter::requestDelete() {
    if (u32(mClearCountdown)) return;
    mClearCountdown = 2;
    u32 count = mActiveParticleCount;
    if (count) {
        JPAEmitterDeferredInit(&deferredParticles()[0], count, gpu().mContext, 4);
        appendDeferred(&deferredParticles()[0], particles().mActive.mpHead);
    }
    count = mChildParticleCount;
    if (count) {
        JPAEmitterDeferredInit(&deferredParticles()[1], count, gpu().mContext, 4);
        appendDeferred(&deferredParticles()[1], particles().mChild.mpHead);
    }
}

/* 0281E0E0 */
void jpa_emitter_extension::JPABaseEmitter::initGpuState(void* context) {
    gpu().mParticlePools[0] = nullptr;
    gpu().mParticlePools[1] = nullptr;
    JPAEmitterVertexStateInit(&gpu().mVertexState, context);
    JPAEmitterGpuArrayInit(gpu().mDynamicBuffers[0], 1, context);
    JPAEmitterGpuArrayInit(gpu().mDynamicBuffers[1], 1, context);
    gpu().mLargePoolIndex = 0;
    gpu().mNoBloom = 0;
    gpu().mRenderEnabled = 1;
    gpu().mSmallPoolIndex = 0;
    gpu().mColorScale[3] = 1.0f;
    gpu().mColorScale[1] = 1.0f;
    gpu().mColorScale[0] = 1.0f;
    gpu().mColorScale[2] = 1.0f;
    gpu().mRenderPass = 0;
    gpu().mSmallPool = nullptr;
    gpu().mResourceName.v = gabi::guest_addr("");
    gpu().mParticlePoolIndices[1] = 0;
    gpu().mParticlePoolIndices[0] = 0;
    gpu().mDrawContextOverride = 0;
    gpu().mApplyColorScale = 0;
    gpu().mMediumPoolIndex = 0;
    gpu().mLargePool = nullptr;
    gpu().mMediumPool = nullptr;
    mClearCountdown = 0;
    gpu().mContext = context;
    mDeleteCountdown = 0;
}

/* 0281E1B8 */
void jpa_emitter_extension::JPABaseEmitter::setRenderMatrix(const Mtx34* source) {
    Mtx34 matrix;
    if (!source) JPAEmitterIdentity(&matrix);
    else {
        // HD: these are pure lfs/stfs copies, preserving signalling-NaN bits.
        auto src = reinterpret_cast<const JPAMathMatrixWords*>(source);
        auto dst = reinterpret_cast<JPAMathMatrixWords*>(&matrix);
        for (u32 row = 0; row < 3; ++row)
            for (u32 column = 0; column < 4; ++column) dst->m[row][column] = u32(src->m[row][column]);
    }
    JPAEmitterCopyMatrix(&matrix, &gpu().mRenderMatrix);
}

/* 0281E958 */
void jpa_emitter_extension::JPABaseEmitter::create(jpa_emitter_calc::JPADataBlockLinkInfo* info) {
    mpDataLinkInfo = info;
    auto blocks = info;
    auto dyn = static_cast<JPADynamicsView*>(blocks->mpDynamics.get());
    gabi::call_ptr<void>(dyn->__vtbl->getEmitterScl, dyn, mEmitterScale);
    gabi::call_ptr<void>(dyn->__vtbl->getEmitterTrs, dyn, mEmitterTranslation);
    gabi::call_ptr<void>(dyn->__vtbl->getEmitterRot, dyn, mEmitterRot);
    mVolumeType = gabi::call_ptr<u32>(dyn->__vtbl->getVolumeType, dyn);
    mRateStep = gabi::call_ptr<u32>(dyn->__vtbl->getRateStep, dyn);
    mDivNumber = gabi::call_ptr<u32>(dyn->__vtbl->getDivNumber, dyn);
    mRate = gabi::call_ptr<f32>(dyn->__vtbl->getRate, dyn);
    mRateRndm = gabi::call_ptr<f32>(dyn->__vtbl->getRateRndm, dyn);
    mMaxFrame = gabi::call_ptr<s32>(dyn->__vtbl->getMaxFrame, dyn);
    mStartFrame = gabi::call_ptr<u32>(dyn->__vtbl->getStartFrame, dyn);
    mVolumeSize = gabi::call_ptr<u32>(dyn->__vtbl->getVolumeSize, dyn);
    mVolumeSweep = gabi::call_ptr<f32>(dyn->__vtbl->getVolumeSweep, dyn);
    mVolumeMinRad = gabi::call_ptr<f32>(dyn->__vtbl->getVolumeMinRad, dyn);
    mLifeTime = gabi::call_ptr<u32>(dyn->__vtbl->getLifeTime, dyn);
    mLifeTimeRndm = gabi::call_ptr<f32>(dyn->__vtbl->getLifeTimeRndm, dyn);
    mMoment = gabi::call_ptr<f32>(dyn->__vtbl->getMoment, dyn);
    mMomentRndm = gabi::call_ptr<f32>(dyn->__vtbl->getMomentRndm, dyn);
    mInitialVelRatio = gabi::call_ptr<f32>(dyn->__vtbl->getInitVelRatio, dyn);
    mAccelRndm = gabi::call_ptr<f32>(dyn->__vtbl->getAccelRndm, dyn);
    mAirResist = gabi::call_ptr<f32>(dyn->__vtbl->getAirResist, dyn);
    mAirResistRndm = gabi::call_ptr<f32>(dyn->__vtbl->getAirResistRndm, dyn);
    mInitialVelOmni = gabi::call_ptr<f32>(dyn->__vtbl->getInitVelOmni, dyn);
    mInitialVelAxis = gabi::call_ptr<f32>(dyn->__vtbl->getInitVelAxis, dyn);
    mInitialVelRndm = gabi::call_ptr<f32>(dyn->__vtbl->getInitVelRndm, dyn);
    mInitialVelDir = gabi::call_ptr<f32>(dyn->__vtbl->getInitVelDir, dyn);
    mAccel = gabi::call_ptr<f32>(dyn->__vtbl->getAccel, dyn);
    gabi::call_ptr<void>(dyn->__vtbl->getEmitterDir, dyn, mEmitterDir);
    f32 y = mEmitterDir[1], x = mEmitterDir[0];
    f32 square = gabi::fmadds(x, x, gabi::fmuls_ppc(y, y));
    f32 z = mEmitterDir[2];
    square = gabi::fmadds(z, z, square);
    // HD: ble skips normalization whenever the greater-than bit is clear.
    if (square > 0.000003814697265625f) {
        f32 inverse = 1.0f / f32(JPAEmitterSqrt(square));
        y = mEmitterDir[1]; x = mEmitterDir[0]; z = mEmitterDir[2];
        mEmitterDir[0] = gabi::fmuls_ppc(x, inverse);
        mEmitterDir[1] = gabi::fmuls_ppc(y, inverse);
        mEmitterDir[2] = gabi::fmuls_ppc(z, inverse);
    }
    mSpread = gabi::call_ptr<f32>(dyn->__vtbl->getSpread, dyn);
    mDataFlag = gabi::call_ptr<u32>(dyn->__vtbl->getDataFlag, dyn);
    mUseKeyFlag = gabi::call_ptr<u32>(dyn->__vtbl->getUseKeyFlag, dyn);
    mFlags = 0x30;
    JPAEmitterIdentity(reinterpret_cast<Mtx34*>(mGlobalRotation));
    for (u32 i = 0; i < 3; ++i) {
        mGlobalDynamicsScale[i] = 1.0f;
        mGlobalTranslation[i] = 0.0f;
        scaleAndColors().mGlobalParticleScale[i] = 1.0f;
    }
    scaleAndColors().mGlobalPrmColor = 0xFFFFFFFF;
    scaleAndColors().mGlobalEnvColor = 0xFFFFFFFF;
    mEmitCount = 0.0f;
    mRateStepTimer = 0.0f;
    mTick = 0.0f;
    mTime = 0.0f;
    gpu().mRenderFlags = 0;
    u32 seed = u32(jpa_emitter_calc::emtrInfo.mRandom) * 0x19660D + 0x3C6EF35F;
    jpa_emitter_calc::emtrInfo.mRandom = seed;
    mRandomSeed = seed;
    JPAEmitterInitFields(mFieldManager, info, &jpa_emitter_calc::emtrInfo);
    auto volume = reinterpret_cast<gabi::ptmf<void (jpa_emitter_calc::JPABaseEmitter::*)()>*>(mVolumeFunc);
    volume->d = 0;
    u32 function;
    switch (u32(mVolumeType)) {
    case 0: function = gabi::guest_fn(reinterpret_cast<const void*>(JPAEmitterVolumeCube)); break;
    case 1: function = gabi::guest_fn(reinterpret_cast<const void*>(JPAEmitterVolumeSphere)); break;
    case 2: function = gabi::guest_fn(reinterpret_cast<const void*>(JPAEmitterVolumeCylinder)); break;
    case 3: function = gabi::guest_fn(reinterpret_cast<const void*>(JPAEmitterVolumeTorus)); break;
    case 4: function = gabi::guest_fn(reinterpret_cast<const void*>(JPAEmitterVolumePoint)); break;
    case 5: function = gabi::guest_fn(reinterpret_cast<const void*>(JPAEmitterVolumeCircle)); break;
    case 6: function = gabi::guest_fn(reinterpret_cast<const void*>(JPAEmitterVolumeLine)); break;
    default: function = 0; break;
    }
    volume->i = function ? -1 : 0;
    volume->f = function;
    // HD: all switch arms reset the render matrix and four cached color vectors.
    JPAEmitterIdentity(&gpu().mRenderMatrix);
    for (u32 row = 0; row < 4; ++row)
        for (u32 column = 0; column < 4; ++column)
            gpu().mColorBits[row][column] = u32(sDefaultColor.bits[column]);
    gpu().mColorScale[0] = 1.0f;
    gpu().mColorScale[1] = 1.0f;
    gpu().mColorScale[3] = 1.0f;
    gpu().mApplyColorScale = 0;
    gpu().mRenderEnabled = 1;
    gpu().mResourceName.v = gabi::guest_addr("");
    gpu().mDrawContextOverride = 0;
    blocks = mpDataLinkInfo.get();
    gpu().mRenderPass = 0;
    gpu().mColorScale[2] = 1.0f;
    JPABaseShapeToneView* base = static_cast<JPABaseShapeToneView*>(blocks->mpBaseShape.get());
    u32 variant = gabi::call_ptr<u32>(base->__vtbl->getTevMode, base) << 2;
    blocks = mpDataLinkInfo.get();
    if (blocks->mpExTexShape) {
        JPAExTexShapeView* shape = static_cast<JPAExTexShapeView*>(blocks->mpExTexShape.get());
        u32 mode = gabi::call_ptr<u32>(shape->__vtbl->getIndTexMode, shape);
        blocks = mpDataLinkInfo.get();
        shape = static_cast<JPAExTexShapeView*>(blocks->mpExTexShape.get());
        if (mode == 1) ++variant;
        if (gabi::call_ptr<u32>(shape->__vtbl->isEnableSecondTex, shape)) variant += 2;
    }
    variant <<= 1;
    u32 projection = gabi::call_ptr<u32>(base->__vtbl->isEnableProjection, base);
    u32 noBloom = gpu().mNoBloom;
    if (projection) ++variant;
    auto material = static_cast<JPAShaderMaterial*>(gpu().mMaterialBindings[noBloom ? 1 : 0].get());
    JPAShaderVariantTable* table = material->mVariants;
    JPAShaderMaterial* selected = material;
    if (table) {
        selected = nullptr;
        if (variant < u32(table->mCount)) selected = &table->mEntries.get()[variant];
    }
    gpu().mDeleteRequested = 0;
    gpu().mDrawContext = gabi::ea(selected);
    mDeleteCountdown = 0;
    mClearCountdown = 0;
    gpu().mTextureVariant = u8(variant);
}

/* 0281FFDC */
void jpa_emitter_extension::JPABaseEmitter::updateViewPosition(s32 view) {
    Mtx34 matrix;
    // HD: degree rotations become signed binary angles with truncating division.
    s16 x = s16((s32(mEmitterRot[0]) * 16384) / 90);
    s16 y = s16((s32(mEmitterRot[1]) * 16384) / 90);
    s16 z = s16((s32(mEmitterRot[2]) * 16384) / 90);
    JPAEmitterMakeRotation(x, y, z, &matrix);
    auto global = reinterpret_cast<Mtx34*>(mGlobalRotation);
    JPAEmitterConcat(global, &matrix, &matrix);
    u32 group = getViewGroup();
    if (view != -1) group = u32(view);
    JPAEmitterTransform(&matrix, reinterpret_cast<cXyz*>(mEmitterDir), reinterpret_cast<cXyz*>(jpa_emitter_calc::emtrInfo.mViewPosition[group]));
}

/* 02820CFC */
void __sinit_JPAEmitter_cpp() {
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[1] = 0;
    sStaticBlock.words[0] = 0;
    JPAEmitterRegister(&sStaticRegistration);
    JPAEmitterRandomInit(&jpa_emitter_calc::emtrInfo, 0);
    JPAEmitterConstructArray(jpa_emitter_calc::emtrInfo.mViewPosition, 3, 12,
        gabi::guest_fn(reinterpret_cast<const void*>(JPAEmitterInfoVectorConstruct)));
}

/* WWHD glue
str 10170D58 "" in 0281E958
data sDefaultColor 104A01EC
ext 028F37F0 f64 JPAEmitterSqrt(f64)
ext 028240DC void JPAEmitterInitFields(void*,jpa_emitter_calc::JPADataBlockLinkInfo*,jpa_emitter_calc::JPAEmitterInfo*)
ext 02820688 void JPAEmitterVolumeCube(jpa_emitter_calc::JPABaseEmitter*) callback
ext 02820778 void JPAEmitterVolumeSphere(jpa_emitter_calc::JPABaseEmitter*) callback
ext 02820A84 void JPAEmitterVolumeCylinder(jpa_emitter_calc::JPABaseEmitter*) callback
ext 02820BE0 void JPAEmitterVolumeTorus(jpa_emitter_calc::JPABaseEmitter*) callback
ext 02820244 void JPAEmitterVolumePoint(jpa_emitter_calc::JPABaseEmitter*) callback
ext 028204A0 void JPAEmitterVolumeCircle(jpa_emitter_calc::JPABaseEmitter*) callback
ext 02820354 void JPAEmitterVolumeLine(jpa_emitter_calc::JPABaseEmitter*) callback
str 10000160 "" in 0281D92C
data sEmitterStringVtable 10170CFC
data sVertexStateVtable 1016EF84
ext 0273AD10 void* JPAEmitterNew(u32)
ext 027EC50C void JPAEmitterLinkConstruct(void*,void*)
ext 027EC678 void JPAEmitterListInitiate(void*)
ext 027FB40C void JPAEmitterVertexBaseConstruct(void*)
ext 028F521C void JPAEmitterClearScratch(void*,u32)
ext 027FD6F4 void* JPAEmitterGpuArrayConstruct(void*) callback
str 10170D18 "" in 0281E0E0
data sStaticBlock 104B5720
data sStaticRegistration 101FA04C
ext 027FB5D4 void JPAEmitterVertexStateInit(void*, void*)
ext 027FD838 void JPAEmitterGpuArrayInit(void*,u32,void*)
ext 0273B584 void JPAEmitterDeferredInit(JPAParticleVector*,u32,void*,s32)
ext 028E9098 void JPAEmitterIdentity(Mtx34*)
ext 028E90D4 void JPAEmitterCopyMatrix(const Mtx34*,Mtx34*)
ext 028245AC void JPAEmitterMakeRotation(s16,s16,s16,Mtx34*)
ext 028E9108 void JPAEmitterConcat(const Mtx34*,const Mtx34*,Mtx34*)
ext 028E8F64 void JPAEmitterTransform(const Mtx34*,const cXyz*,cXyz*)
ext 028F026C void JPAEmitterRegister(void*)
ext 027ED6EC void JPAEmitterRandomInit(void*,u32)
ext 028EFFD0 void JPAEmitterConstructArray(void*,u32,u32,u32)
*/

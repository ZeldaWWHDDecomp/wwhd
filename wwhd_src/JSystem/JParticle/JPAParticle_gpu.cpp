// Particle construction and GPU-pool lifetime, adapted from tww JPAParticle.cpp.
#include "JSystem/JParticle/JPAParticle_gpu.h"
#include "sead/ElementAt.h"
#include "d/d_com_inf_game.h"
void* JPAParticleNew(u32);
void JPAParticleLinkConstruct(JPAParticleLink*,void*);
void* JPAParticleAcquirePool(void*,be<u32>*);
void JPAParticleReleasePool(void*,u32);
void JPAParticleRegister(void*);
void JPAParticleResetGpuState(JPAGpuState*);
void JPAParticleVertexSetData(JPAGpuVertexBuffer*,u32,void*);
void JPAParticleSetDrawContext(JPAParticleRenderer*,u32);
void JPAParticleShapeDraw(void*);
void JPAParticleDrawIndexed(u32,u32,u32,void*,u32,u32);
void JPAParticleSetPointSize(f32,f32);
void JPAParticleSetColor(JPAGpuState*,JPAFloatColor*,u32);
void JPAParticleSetMatrix(JPAGpuState*,const void*,u32);
void JPAParticleSetIndTexMatrix(JPAGpuState*,u32,JPAFloatColor*,JPAFloatColor*);
namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegistration;
static const Mtx34 sIdentityMatrix;
struct IndexedDrawState { be<u32> mIndexType, mPrimitive; gptr<void> mIndices; };
struct ShaderVtable { u8 mPrefix[0x2C]; be<u32> apply; };
struct ShaderState { u8 mPrefix[0xC]; gptr<ShaderVtable> __vtbl; };
static IndexedDrawState sParentDrawState;
static IndexedDrawState sChildDrawState;
}

void* JPAParticleHeapAllocate(u32,void*,s32);
void JPAParticleConstructArray(void*,u32,u32,u32);
JPAGpuBuffer* JPAGpuResourceBufferConstruct(JPAGpuBuffer*);
void* JPAResourceRecordConstruct(void*);
jpa_emitter_extension::JPAShaderMaterialSet* JPAParticleGetMaterialSet();
s32 JPAParticleFindMaterial(jpa_emitter_extension::JPAShaderProgram*,const SafeString*);
void JPAParticleInitializeMaterial(jpa_emitter_extension::JPAShaderMaterial*,jpa_emitter_extension::JPAShaderResource*,u32);
void* JPAParticleGetAllocator(void*);
void JPAParticleInitializeVertex(JPAGpuVertexBuffer*,void*,u32,be<u32>*);
void JPAParticleBindShape(jpa_emitter_extension::JPAShaderMaterial*,void*,JPAGpuVertexBuffer*,be<u32>*,u32);
void JPAParticleConstructShader(JPAGpuState*);
void JPAParticleClearMemory(void*,u32);
void JPAParticleInitializeShader(JPAGpuState*,void*);
JPAParticleRenderer* JPAParticleConstructRenderer(JPAParticleRenderer*);
void JPAParticleInitializeRenderer(JPAParticleRenderer*,u32,void*,void*);
namespace {
struct ParticleAllocatorVtable { u8 mPrefix[0x34]; be<u32> allocate; };
struct ParticleAllocator { u8 mPrefix[0xC]; gptr<ParticleAllocatorVtable> __vtbl; };
struct AllocatorSlot { gptr<void> mAllocator; };
static AllocatorSlot sAllocator;
struct ParticleGpuFormat { be<u32> mWords[4]; };
static ParticleGpuFormat sGpuFormat;
struct ParticleShaderVtable { be<u32> mWords[4]; };
static ParticleShaderVtable sShaderBaseVtable;
static ParticleShaderVtable sShaderVtable;
static ParticleShaderVtable sParticleStringVtable;
struct ShaderResourceRecord { be<u32> mWords[4]; };
struct ParticleShaderData {
    JPAFloatColor mColors[11];
    ShaderResourceRecord mRecords[3][2];
    Mtx34 mMatrices[8];
    JPAFloatColor mVectors[6];
};
struct ParticleShaderPrefix {
    u8 mPrefix[0xC]; gptr<ParticleShaderVtable> __vtbl;
    u8 mBeforeData[0x64]; ParticleShaderData mData; u8 mPadding[4];
};
WWHD_SIZE(ParticleShaderData,0x2F0);
WWHD_OFFSET(ParticleShaderData,mRecords,0xB0);
WWHD_OFFSET(ParticleShaderData,mMatrices,0x110);
WWHD_OFFSET(ParticleShaderData,mVectors,0x290);
WWHD_OFFSET(ParticleShaderPrefix,mData,0x74);
WWHD_SIZE(ParticleShaderPrefix,0x368);
void constructParticleShader(JPAGpuState* shader) {
    JPAParticleConstructShader(shader);
    auto prefix = reinterpret_cast<ParticleShaderPrefix*>(shader->mPrefix);
    prefix->__vtbl = &sShaderBaseVtable;
    JPAParticleClearMemory(&prefix->mData,sizeof(ParticleShaderData));
    for (u32 i = 0; i < 11; ++i) {
        prefix->mData.mColors[i].r = 0.0f;
        prefix->mData.mColors[i].g = 0.0f;
        prefix->mData.mColors[i].b = 0.0f;
        prefix->mData.mColors[i].a = 1.0f;
    }
    for (u32 i = 0; i < 3; ++i)
        JPAParticleConstructArray(prefix->mData.mRecords[i],2,sizeof(ShaderResourceRecord),
            gabi::guest_fn(reinterpret_cast<const void*>(JPAResourceRecordConstruct)));
    // HD: retain GHS null checks for the inlined empty matrix/vector constructors.
    for (u32 i = 0; i < 8; ++i)
        if (gabi::ea(&prefix->mData.mMatrices[i]) == 0) JPAParticleNew(sizeof(Mtx34));
    for (u32 i = 0; i < 6; ++i)
        if (gabi::ea(&prefix->mData.mVectors[i]) == 0) JPAParticleNew(sizeof(JPAFloatColor));
    prefix->__vtbl = &sShaderVtable;
    shader->mPendingCommands[1] = 0;
    shader->mPendingCommands[0] = 0;
}
}
/* 02824C04 */
void jpa_particle_extension::JPABaseParticle::initializeGpu(void* heap,void* context) {
    contextState().mInitialized = 0;
    auto pool = static_cast<JPAGpuPool<4>*>(JPAParticleHeapAllocate(sizeof(JPAGpuPool<4>),heap,4));
    if (pool) {
        JPAParticleConstructArray(pool,4,sizeof(JPAGpuBuffer),
            gabi::guest_fn(reinterpret_cast<const void*>(JPAGpuResourceBufferConstruct)));
        pool->mState.mShapeEnabled = 0;
        pool->mState.mSwapIndex = 0;
        pool->mState.mElementSize = 20;
        pool->mState.mInitialized = 0;
        pool->mState.mCursor = 0;
        for (u32 phase = 0; phase < 2; ++phase)
            for (u32 i = phase; i < 4; i += 2) pool->mBuffers[i].mInUse = 0;
    }
    contextState().mParentPool = pool;
    pool->mState.mCursor = 17;
    pool->mState.mFormat = &sGpuFormat;
    gabi::Local<SafeString> name;
    name->__vtbl = gabi::ea(&sParticleStringVtable);
    name->mTextPtr = gabi::guest_addr("jparticle_type_a");
    auto set = JPAParticleGetMaterialSet();
    s32 index = JPAParticleFindMaterial(set->mProgram,name.get());
    jpa_emitter_extension::JPAShaderMaterial* material = nullptr;
    if (index >= 0) {
        u32 count = set->mCount;
        auto materials = set->mMaterials.get();
        material = u32(index) < count ? &sead::elementAt(materials, u32(index)) : materials;
        if (!material->mReady) {
            auto program = set->mProgram.get();
            jpa_emitter_extension::JPAShaderResource* resource = nullptr;
            if (u32(index) < u32(program->mResourceCount)) resource = &sead::elementAt(program->mResources, u32(index));
            JPAParticleInitializeMaterial(material,resource,0);
            count = set->mCount;
            materials = set->mMaterials;
        }
        material = u32(index) < count ? &sead::elementAt(materials, u32(index)) : materials;
    }
    pool = contextState().mParentPool;
    for (u32 phase = 0; phase < 2; ++phase) {
        for (u32 i = phase; i < 4; i += 2) {
            JPAGpuBuffer* buffer = &pool->mBuffers[i];
            void* data = buffer->mBufferData;
            if (!data) {
                auto allocator = static_cast<ParticleAllocator*>(JPAParticleGetAllocator(sAllocator.mAllocator));
                void* allocation = gabi::call_ptr<void*>(allocator->__vtbl->allocate,allocator,80u,64u);
                if (allocation) { buffer->mAllocation = allocation; buffer->mCapacity = 4; }
                data = buffer->mAllocation;
                buffer->mBufferData = data;
            }
            JPAParticleInitializeVertex(&buffer->mVertexBuffer,data,4,&pool->mState.mCursor);
            if (material && gabi::ea(material) != u32(pool->mState.mShapeEnabled))
                JPAParticleBindShape(material,buffer->mShape,&buffer->mVertexBuffer,&pool->mState.mCursor,0);
        }
    }
    pool->mState.mMaterial = material;
    pool->mState.mInitialized = 1;
    gpuPool() = nullptr;
    mpSlot = nullptr;
    auto shader = static_cast<JPAGpuState*>(JPAParticleHeapAllocate(sizeof(JPAGpuState),heap,4));
    if (shader) constructParticleShader(shader);
    mpGpuState = shader;
    JPAParticleInitializeShader(shader,heap);
    auto renderer = static_cast<JPAParticleRenderer*>(JPAParticleHeapAllocate(0x4C,heap,4));
    if (renderer) renderer = JPAParticleConstructRenderer(renderer);
    mpRenderer = renderer;
    JPAParticleInitializeRenderer(renderer,0,context,heap);
    drawState().mHeap = heap;
    poolTail().mIndex = 0;
    drawState().mTextureIndices[0] = 65535;
    mpEmitter = nullptr;
    drawState().mTextureIndices[1] = 65535;
    mDrawSkipped = 0;
    mDeletionDelay = 0;
}

/* 02824B18 */
jpa_particle_extension::JPABaseParticle* JPABaseParticleConstruct(jpa_particle_extension::JPABaseParticle* self) {
    if (!self) {
        self = static_cast<jpa_particle_extension::JPABaseParticle*>(JPAParticleNew(sizeof(jpa_particle_extension::JPABaseParticle)));
        if (!self) return nullptr;
    }
    JPAParticleLink* link = &self->mLink;
    if (gabi::ea(link) == 0) link = static_cast<JPAParticleLink*>(JPAParticleNew(sizeof(JPAParticleLink)));
    if (link) JPAParticleLinkConstruct(link, self);
    // HD: GHS keeps the null-preserving inline vector constructor branches.
    if (gabi::ea(self->mOffsetPosition) == 0) JPAParticleNew(12);
    if (gabi::ea(self->mLocalPosition) == 0) JPAParticleNew(12);
    if (gabi::ea(self->mGlobalPosition) == 0) JPAParticleNew(12);
    if (gabi::ea(self->mVelocity) == 0) JPAParticleNew(12);
    if (gabi::ea(self->mBaseVel) == 0) JPAParticleNew(12);
    if (gabi::ea(self->mAccel) == 0) JPAParticleNew(12);
    if (gabi::ea(self->mFieldVel) == 0) JPAParticleNew(12);
    if (gabi::ea(self->mFieldAccel) == 0) JPAParticleNew(12);
    self->mCurFrame = 0.0f;
    return self;
}

/* 0282581C */
void jpa_particle_extension::JPABaseParticle::setDrawMode(u32 mode) { drawState().mDrawMode = u8(mode); }
/* 02825824 */
void jpa_particle_extension::JPABaseParticle::acquireGpuPool() {
    if (!gpuPool()) gpuPool() = JPAParticleAcquirePool(poolTail().mOwner, &poolTail().mIndex);
}
/* 02825868 */
void jpa_particle_extension::JPABaseParticle::releaseGpuPool() {
    if (gpuPool()) {
        JPAParticleReleasePool(poolTail().mOwner, poolTail().mIndex);
        gpuPool() = nullptr;
    }
}

static bool skipAnmTone(const jpa_particle_extension::JPABaseParticle* particle,
                        jpa_emitter_extension::JPABaseEmitter* emitter) {
    if (!u32(particle->mUseAnmTone)) return false;
    auto blocks = emitter->mpDataLinkInfo.get();
    auto shape = static_cast<jpa_emitter_extension::JPABaseShapeToneView*>(blocks->mpBaseShape.get());
    return gabi::call_ptr<u32>(shape->__vtbl->isEnableAnmTone, shape) != 0;
}

/* 02825158 */
void jpa_particle_extension::JPABaseParticle::finishGpuBuffers(u32 child) {
    bool tone = false;
    if (u32(mUseAnmTone)) {
        auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(mpEmitter.get());
        tone = skipAnmTone(this, emitter);
    }
    if (!tone) JPAParticleResetGpuState(mpGpuState);
    JPAGpuPool<4>* pool;
    if (!child) {
        if (!u32(mUseAnmTone)) return;
        pool = contextState().mParentPool;
    } else pool = reinterpret_cast<JPAGpuPool<4>*>(mpSlot.get());
    u32 active = pool->mState.mSwapIndex;
    for (u32 i = 0; i < 2; ++i) {
        JPAGpuVertexBuffer* buffer = &sead::elementAt(pool->mBuffers, active + i * 2).mVertexBuffer;
        void* data = buffer->mData;
        JPAParticleVertexSetData(buffer, 0, data);
    }
    pool->mState.mSwapIndex = u32(pool->mState.mSwapIndex) == 0 ? 1 : 0;
}

/* 02825284 */
void jpa_particle_extension::JPABaseParticle::drawGpu(u32 kind, u32 primitive, u32 count) {
    auto shader = reinterpret_cast<ShaderState*>(mpGpuState.get());
    u32 context = contextState().mDrawContext;
    gabi::call_ptr<void>(shader->__vtbl->apply, shader, context);
    JPAParticleSetDrawContext(mpRenderer, contextState().mDrawContext);
    if (kind == 0) {
        JPAGpuPool<4>* pool = contextState().mParentPool;
        u32 active = u32(pool->mState.mSwapIndex) == 0 ? 1 : 0;
        for (u32 i = 0; i < count; ++i) {
            JPAGpuBuffer* buffer = &pool->mBuffers[active + i * 2];
            if (u32(pool->mState.mShapeEnabled)) JPAParticleShapeDraw(buffer->mShape);
            JPAParticleDrawIndexed(sParentDrawState.mPrimitive, 4, sParentDrawState.mIndexType, sParentDrawState.mIndices, 0, 1);
        }
    } else if (kind == 1) {
        auto pool = reinterpret_cast<JPAGpuPool<4>*>(mpSlot.get());
        u32 active = u32(pool->mState.mSwapIndex) == 0 ? 1 : 0;
        for (u32 i = 0; i < count; ++i) {
            JPAGpuBuffer* buffer = &pool->mBuffers[active + i * 2];
            if (u32(pool->mState.mShapeEnabled)) JPAParticleShapeDraw(buffer->mShape);
            if (primitive) JPAParticleDrawIndexed(sChildDrawState.mPrimitive, primitive, sChildDrawState.mIndexType, sChildDrawState.mIndices, 0, 1);
        }
    } else {
        f32 size = f32(u32(drawState().mDrawMode));
        JPAParticleSetPointSize(size, size);
        JPAGpuPool<4>* pool = contextState().mParentPool;
        u32 active = u32(pool->mState.mSwapIndex) == 0 ? 1 : 0;
        JPAParticleShapeDraw(pool->mBuffers[active].mShape);
    }
}

/* 02825430 */
void jpa_particle_extension::JPABaseParticle::setPrmColor(const JPAFloatColor* input) {
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(mpEmitter.get());
    JPAFloatColor color;
    u32 b = input->bits[2], a = input->bits[3];
    color.bits[2] = b; color.bits[3] = a;
    u32 r = input->bits[0], g = input->bits[1];
    color.bits[0] = r; color.bits[1] = g;
    if (emitter) {
        if (skipAnmTone(this, emitter)) return;
        if (u32(emitter->gpu().mApplyColorScale)) {
            f32 alpha = emitter->gpu().mColorScale[3];
            f32 factor = gabi::fmuls_ppc(alpha, emitter->gpu().mColorScale[0]);
            color.r = gabi::fmuls_ppc(color.r, factor);
            factor = gabi::fmuls_ppc(alpha, emitter->gpu().mColorScale[1]);
            color.g = gabi::fmuls_ppc(color.g, factor);
            factor = gabi::fmuls_ppc(alpha, emitter->gpu().mColorScale[2]);
            color.b = gabi::fmuls_ppc(color.b, factor);
        }
    }
    JPAParticleSetColor(mpGpuState, &color, 0);
}

/* 02825514 */
void jpa_particle_extension::JPABaseParticle::setEnvColor(const JPAFloatColor* input) {
    auto emitter = reinterpret_cast<jpa_emitter_extension::JPABaseEmitter*>(mpEmitter.get());
    JPAFloatColor color;
    u32 b = input->bits[2], a = input->bits[3];
    color.bits[2] = b; color.bits[3] = a;
    u32 r = input->bits[0], g = input->bits[1];
    color.bits[0] = r; color.bits[1] = g;
    if (emitter) {
        if (skipAnmTone(this, emitter)) return;
        if (u32(emitter->gpu().mApplyColorScale)) {
            f32 alpha = emitter->gpu().mColorScale[3];
            f32 factor = gabi::fmuls_ppc(alpha, emitter->gpu().mColorScale[0]);
            color.r = gabi::fmuls_ppc(color.r, factor);
            factor = gabi::fmuls_ppc(alpha, emitter->gpu().mColorScale[1]);
            color.g = gabi::fmuls_ppc(color.g, factor);
            factor = gabi::fmuls_ppc(alpha, emitter->gpu().mColorScale[2]);
            color.b = gabi::fmuls_ppc(color.b, factor);
        }
    }
    JPAParticleSetColor(mpGpuState, &color, 1);
}

/* 028255F8 */
void jpa_particle_extension::JPABaseParticle::applyEmitterColors(jpa_emitter_extension::JPABaseEmitter* emitter) {
    if (skipAnmTone(this, emitter)) return;
    JPAEmitterColors colors;
    for (u32 i = 0; i < 4; ++i) {
        colors.mPrm.bits[i] = u32(emitter->gpu().mColorBits[0][i]);
        colors.mEnv.bits[i] = u32(emitter->gpu().mColorBits[1][i]);
    }
    setPrmColor(&colors.mPrm);
    setEnvColor(&colors.mEnv);
    JPAParticleSetMatrix(mpGpuState, &emitter->gpu().mRenderMatrix, 0);
}

/* 028256C0 */
void jpa_particle_extension::JPABaseParticle::setIndirectMatrix(u32 index, const JPAIndTexMatrix* matrix, s32 exponent) {
    JPAGpuState* state = mpGpuState;
    JPAEmitterColors rows;
    rows.mPrm.bits[0] = u32(matrix->elements[0][0]);
    rows.mPrm.bits[1] = u32(matrix->elements[0][1]);
    rows.mPrm.bits[2] = u32(matrix->elements[1][0]);
    rows.mPrm.bits[3] = u32(matrix->elements[1][1]);
    rows.mEnv.bits[0] = u32(matrix->elements[0][2]);
    rows.mEnv.bits[1] = u32(matrix->elements[1][2]);
    rows.mEnv.b = f32(exponent);
    rows.mEnv.a = 0.0f;
    JPAParticleSetIndTexMatrix(state, index, &rows.mPrm, &rows.mEnv);
}

/* 02825750 */
void jpa_particle_extension::JPABaseParticle::applyEmitterTexture(jpa_emitter_extension::JPABaseEmitter* emitter) {
    auto blocks = emitter->mpDataLinkInfo.get();
    if (blocks->mpExTexShape) {
        auto shape = static_cast<jpa_emitter_extension::JPAExTexShapeView*>(blocks->mpExTexShape.get());
        u32 mode = gabi::call_ptr<u32>(shape->__vtbl->getIndTexMode, shape);
        if (mode == 1) {
            blocks = emitter->mpDataLinkInfo.get();
            shape = static_cast<jpa_emitter_extension::JPAExTexShapeView*>(blocks->mpExTexShape.get());
            auto matrix = gabi::call_ptr<const JPAIndTexMatrix*>(shape->__vtbl->getIndTexMtx, shape);
            blocks = emitter->mpDataLinkInfo.get();
            shape = static_cast<jpa_emitter_extension::JPAExTexShapeView*>(blocks->mpExTexShape.get());
            s32 scale = gabi::call_ptr<s32>(shape->__vtbl->getExpScale, shape);
            setIndirectMatrix(0, matrix, scale);
            JPAParticleSetMatrix(mpGpuState, &sIdentityMatrix, 1);
        }
    }
    contextState().mDrawContext = u32(emitter->gpu().mDrawContext);
}

// HD: compiler-generated initialization of this translation unit.
/* 028269AC */
void __sinit_JPAParticle_cpp() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    JPAParticleRegister(&sStaticRegistration);
}

/* WWHD glue
data sAllocator 101F8B4C
data sGpuFormat 10172214
data sShaderBaseVtable 1016EFB4
data sShaderVtable 1016EFE4
data sParticleStringVtable 1017126C
str 10171284 "jparticle_type_a" in 02824C04
ext 0273B050 void* JPAParticleHeapAllocate(u32,void*,s32)
ext 028EFFD0 void JPAParticleConstructArray(void*,u32,u32,u32)
ext 027FFCBC jpa_emitter_extension::JPAShaderMaterialSet* JPAParticleGetMaterialSet()
ext 027B90AC s32 JPAParticleFindMaterial(jpa_emitter_extension::JPAShaderProgram*,const SafeString*)
ext 02800B0C void JPAParticleInitializeMaterial(jpa_emitter_extension::JPAShaderMaterial*,jpa_emitter_extension::JPAShaderResource*,u32)
ext 02756140 void* JPAParticleGetAllocator(void*)
ext 027FF478 void JPAParticleInitializeVertex(JPAGpuVertexBuffer*,void*,u32,be<u32>*)
ext 027FF530 void JPAParticleBindShape(jpa_emitter_extension::JPAShaderMaterial*,void*,JPAGpuVertexBuffer*,be<u32>*,u32)
ext 027FB40C void JPAParticleConstructShader(JPAGpuState*)
ext 028F521C void JPAParticleClearMemory(void*,u32)
ext 027FB5D4 void JPAParticleInitializeShader(JPAGpuState*,void*)
ext 027FE344 JPAParticleRenderer* JPAParticleConstructRenderer(JPAParticleRenderer*)
ext 027FE500 void JPAParticleInitializeRenderer(JPAParticleRenderer*,u32,void*,void*)
data sParentDrawState 104B4CFC
data sChildDrawState 104B4A84
ext 027FB678 void JPAParticleResetGpuState(JPAGpuState*)
ext 027B5E94 void JPAParticleVertexSetData(JPAGpuVertexBuffer*,u32,void*)
ext 027FE5B8 void JPAParticleSetDrawContext(JPAParticleRenderer*,u32)
ext 027BFE5C void JPAParticleShapeDraw(void*)
ext C0006178 void JPAParticleDrawIndexed(u32,u32,u32,void*,u32,u32)
ext C0006920 void JPAParticleSetPointSize(f32,f32)
data sIdentityMatrix 104A041C
ext 027FC500 void JPAParticleSetColor(JPAGpuState*,JPAFloatColor*,u32)
ext 027FC80C void JPAParticleSetMatrix(JPAGpuState*,const void*,u32)
ext 027FCAB0 void JPAParticleSetIndTexMatrix(JPAGpuState*,u32,JPAFloatColor*,JPAFloatColor*)
data sStaticBlock 104B5980
data sStaticRegistration 101FA088
ext 0273AD10 void* JPAParticleNew(u32)
ext 027EC50C void JPAParticleLinkConstruct(JPAParticleLink*,void*)
ext 0282FB1C void* JPAParticleAcquirePool(void*,be<u32>*)
ext 0282FBA8 void JPAParticleReleasePool(void*,u32)
ext 028F026C void JPAParticleRegister(void*)
*/

/** Boss lava waves, following tww dMagma_ballBoss_c.
 * HD: the balls own expanded mesh resources; retain their deleting-destructor copies and all
 * virtual cleanup. Verified against WWHD's recompiled functions. Local only.
 */
#include "d/d_boss_magma.h"

static btd_class* btd;
static dMagma_BallVtableHD l_ballVtable;
static Mtx34 l_viewMtx;
static u32 l_heapMgr;

/* 024F60E8 */
void dMagma_ballBoss_c::calc(f32 param_1, u8 param_2, s32 param_3) {
    btd_class* boss = btd;
    // HD: the negated comparison also sends a NaN boss height to the submerged case.
    if (boss && !(boss->m6E84 > -150.0f)) {
        mPos.y = -200.0f;
        return;
    }
    if (boss) {
        f32 z = mPos.z, x = mPos.x;
        f32 radius = std_sqrtf(gabi::fmadds(x, x, gabi::fmuls_ppc(z, z)));
        f32 diff = std::fabs(gabi::fsubs_ppc(boss->m6E7C, radius));
        if (diff < 300.0f) {
            f32 adjustment = REG0_F(5), base = mBaseY;
            f32 targetY =
                gabi::fadds_ppc(gabi::fadds_ppc(base, gabi::fadds_ppc(adjustment, 50.0f)), 30.0f);
            cLib_addCalc2(&mPos.y, gabi::fnmsubs(diff, 0.1f, targetY), 0.5f, 20.0f);
            cLib_addCalc2(&mScale, 1.5f, 0.2f, 0.2f);
            mWave = 0x4000;
            field_0x7C = 1000;
            return;
        }
    }
    s16 wave = mWave;
    u16 phase = (u16)wave;
    if (wave < 0) {
        // HD: preserve virtual dispatch and the verified integer/float argument registers.
        gabi::call_ptr<void>(mVtable->setup.get(), this, 0u, s32(-1), param_1);
        mWave = 0;
        phase = 0;
    }
    f32 adjustment = REG0_F(5);
    s16 nextWave = mWave;
    f32 base = mBaseY;
    s16 speed = field_0x7C;
    f32 sine = cM_ssin(phase);
    f32 y = gabi::fmadds(sine, gabi::fadds_ppc(adjustment, 50.0f), base);
    mWave = (s16)(nextWave + speed);
    mPos.y = y;
}

/* 024F62F8 */
void dMagma_ballBoss_c::update() {
    f32 y = mPos.y, z = mPos.z, x = mPos.x;
    mDoMtx_stack_c::transS(x, y, z);
    f32 scale = mScale;
    mDoMtx_stack_c::scaleM(scale, 1.0f, scale);
    PSMTXCopy(mDoMtx_stack_c::get(), &mTexProjMtx);
    PSMTXConcatHD(&l_viewMtx, &mTexProjMtx, &mPosMtx);
    // HD: update also prepares the mesh's rendering resources.
    dMagma_prepareBallHD(this);
}

/* 024F6098 */
static u32 b_a_sub(fopAc_ac_c* param_1, void* param_2) {
    // HD: IsActor runs before the explicit null test, and the result remains a full register.
    return fopAc_IsActor(param_1) && param_1 && param_1->mProcName == fpcNm_BTD_e
               ? gabi::ea(param_1)
               : 0;
}

/* 024F637C */
void dMagma_ballBoss_c::setup(f32 param_1, u8 param_2, s32 param_3) {
    f32 x = cM_rndFX(1300.0f);
    mPos.x = x;
    mPos.y = 0.0f;
    f32 max_z = std_sqrtf(gabi::fnmsubs(x, x, 1690000.0f));
    f32 z = cM_rndFX(max_z);
    mPos.z = z;
    f32 variation = cM_rndF(gabi::fadds_ppc(REG0_F(12), 0.5f));
    f32 scale = gabi::fadds_ppc(gabi::fadds_ppc(REG0_F(11), 0.5f), variation);
    mScale = scale;
    f32 base =
        gabi::fadds_ppc(gabi::fadds_ppc(gabi::fadds_ppc(REG0_F(6), -170.0f), param_1), 90.0f);
    mBaseY = base;
    btd_class* boss =
        gabi::at<btd_class>(dMagma_searchActorHD(gabi::guest_fn((const void*)b_a_sub), 0u));
    btd = boss;
    if (boss) {
        if (boss->m6E88 == 1) {
            f32 vz = mPos.z, vx = mPos.x;
            f32 dist = std_sqrtf(gabi::fmadds(vx, vx, gabi::fmuls_ppc(vz, vz)));
            f32 target_dist = gabi::fadds_ppc(REG0_F(16), 800.0f);
            f32 height = mBaseY;
            if (dist < target_dist) {
                f32 factor = gabi::fadds_ppc(REG0_F(17), 0.05f);
                height = gabi::fmadds(gabi::fsubs_ppc(target_dist, dist), factor, height);
                mBaseY = height;
            }
            boss = btd;
            mBaseY = gabi::fadds_ppc(height, boss->m6E84);
        } else {
            f32 delta = boss->m6E84, height = mBaseY;
            // HD: keep the boss operand first when both values carry NaN payloads.
            mBaseY = gabi::fadds_ppc(delta, height);
        }
    }
    f32 speed = cM_rndF(100.0f);
    speed = gabi::fadds_ppc(speed, 400.0f);
    field_0x7C = (s16)gabi::ftoi(speed);
    f32 wave = cM_rndF(8.0f);
    mWave = (s16)gabi::ftoi(gabi::fmuls_ppc(4096.0f, wave));
}

// HD: compiler-generated and template code of this translation unit.
/* 024F6570 */
static void __sinit_d_boss_magma_cpp() {
    sinit_header_statics_rodata(0x1046EE40, 0x101D53E8, 0x1046EE4C, 0x100446B0);
}

/* 024F6604 */
static void dMagma_MeshBufferHD__dt(dMagma_MeshBufferHD* self, u32 flags) {
    if (self) {
        dMagma_destroyMeshBindingHD(&self->mBinding, 2u);
        dMagma_destroyVertexBufferHD(&self->mVertexBuffer, 2u);
        if (flags & 1)
            dMagma_deleteHD(self);
    }
}

/* inlined into dMagma_ball_c__dt */
static void releaseField(be<u32>* address) {
    dMagma_HeapHD* heap = dMagma_findHeapHD(l_heapMgr, *address);
    u32 free = heap->mVtable->free.get();
    // The allocation field is reloaded after heap lookup, which may clobber it.
    gabi::call_ptr<void>(free, heap, u32(*address));
}

/* inlined into dMagma_ball_c__dt */
static void clearMeshPair(dMagma_MeshPairHD* p) {
    for (u32 i = 0; i < 2; ++i) {
        dMagma_MeshBufferHD* buffer = &p->mBuffers[i];
        dMagma_resetMeshBindingHD(&buffer->mBinding);
        u32 data = buffer->mpAllocation;
        buffer->mpData = 0;
        if (data) {
            releaseField(&buffer->mpAllocation);
            buffer->mAllocationCount = 0;
            buffer->mpAllocation = 0;
        }
    }
    p->mpManagedData = 0;
}

/* inlined into dMagma_ball_c__dt */
static void destroyMesh(dMagma_ball_c* self, u32 flags) {
    if (!self)
        return;
    self->mVtable = &l_ballVtable;
    clearMeshPair(&self->mMeshPair);
    // HD: the fallback uses the array base when the signed index fails the unsigned bound check.
    for (s32 i = 0; i < self->mResourceList.mCount; ++i) {
        u32 data = self->mResourceList.mpEntries;
        if ((u32)i < (u32)self->mResourceList.mCount)
            data += u32(i) * sizeof(dMagma_ResourceEntryHD);
        dMagma_ResourceEntryHD* entry = gabi::at<dMagma_ResourceEntryHD>(data);
        dMagma_destroyBindingHD(&entry->mBindings[0]);
        dMagma_destroyBindingHD(&entry->mBindings[1]);
    }
    for (u32 i = 0; i < 2; ++i)
        dMagma_destroyBindingHD(&self->mFirstBindings[i]);
    for (u32 i = 0; i < 2; ++i)
        dMagma_destroyBindingHD(&self->mSecondBindings[i]);
    dMagma_destroyShapeHD(&self->mSecondShape, 2u);
    dMagma_destroyShapeHD(&self->mFirstShape, 2u);
    dMagma_destroyMaterialHD(&self->mSecondMaterial, 0u);
    dMagma_destroyMaterialHD(&self->mFirstMaterial, 0u);
    dMagma_destroyResourceListHD(&self->mResourceList, 2u);
    dMagma_destroyVertexDescriptorHD(&self->mVertexDescriptor, 2u);
    dMagma_MeshPairHD* p = gabi::at<dMagma_MeshPairHD>(gabi::ea(&self->mMeshPair));
    if (p) {
        clearMeshPair(p);
        dMagma_destroyArrayHD(p, 2u, sizeof(dMagma_MeshBufferHD),
                              gabi::guest_fn((const void*)dMagma_MeshBufferHD__dt), 0u, 0u);
    }
    u32 data = self->mpOwnedResources;
    if (data) {
        for (s32 i = 0; i < self->mOwnedResourceCount; ++i) {
            dMagma_OwnedResourceHD* entry =
                gabi::at<dMagma_OwnedResourceHD>(data + u32(i) * sizeof(dMagma_OwnedResourceHD));
            if (entry) {
                entry->mpResource = 0;
                for (u32 a = 0; a < 2; ++a) {
                    dMagma_OwnedArrayHD* array = &entry->mArrays[a];
                    u32 arr = array->mpObjects;
                    if (arr) {
                        for (s32 j = 0; j < array->mCount; ++j) {
                            dMagma_OwnedObjectHD* object = gabi::at<dMagma_OwnedObjectHD>(
                                arr + u32(j) * sizeof(dMagma_OwnedObjectHD));
                            gabi::call_ptr<void>(object->mVtable->destroy.get(), object, 2u);
                            arr = array->mpObjects;
                        }
                        releaseField(&array->mpObjects);
                        array->mCount = 0;
                        array->mpObjects = 0;
                    }
                }
                data = self->mpOwnedResources;
            }
        }
        releaseField(&self->mpOwnedResources);
        self->mOwnedResourceCount = 0;
        self->mpOwnedResources = 0;
    }
    if (flags & 1)
        dMagma_deleteHD(self);
}

/* 024F6664 */
static void dMagma_ball_c__dt(dMagma_ball_c* self, u32 flags) { destroyMesh(self, flags); }
/* 024F6A28 */
static void dMagma_ballBoss_c__dt(dMagma_ballBoss_c* self, u32 flags) { destroyMesh(self, flags); }

/* WWHD glue (tools/verify/autoglue.py)
data btd 101D53E4
data l_ballVtable 1004462C
data l_viewMtx 104B45F8
data l_heapMgr 101F8B4C
ext 02589D30 void dMagma_prepareBallHD(dMagma_ball_c*)
ext 028E9108 void PSMTXConcatHD(Mtx34*, Mtx34*, Mtx34*)
ext 025DE508 u32 dMagma_searchActorHD(u32, u32)
ext 027BEBEC void dMagma_destroyBindingHD(void*)
ext 027BF7E8 void dMagma_resetMeshBindingHD(void*)
ext 027BF880 void dMagma_destroyMeshBindingHD(void*, u32)
ext 027B5CBC void dMagma_destroyVertexBufferHD(void*, u32)
ext 027BE2B0 void dMagma_destroyShapeHD(void*, u32)
ext 027FB528 void dMagma_destroyMaterialHD(void*, u32)
ext 027FD764 void dMagma_destroyResourceListHD(void*, u32)
ext 027B54A0 void dMagma_destroyVertexDescriptorHD(void*, u32)
ext 028F0164 void dMagma_destroyArrayHD(void*, u32, u32, u32, u32, u32)
ext 02755FEC dMagma_HeapHD* dMagma_findHeapHD(u32, u32)
ext 0273AF40 void dMagma_deleteHD(void*)
*/

// JPA manager construction helpers, adapted from tww and Cafe compiler output.
#include "JSystem/JParticle/JPAEmitterManager.h"
#include "sead/ElementAt.h"
#include "d/d_com_inf_game.h"
#include "JSystem/JParticle/JPAParticle_gpu.h"
using namespace jpa_emitter_extension;
void* JPAManagerNew(u32);
void JPAManagerDelete(void*);
void JPAManagerRegister(void*);
void JPAManagerDestroyArray(void*,u32,u32,u32,u32,u32);
void JPAFieldSafeStringDestroy(void*,u32);
void JPAManagerDeleteAllFields(void*);
void JPAManagerClearDeferred(JPAParticleVector*);
void JPAManagerRequestDelete(jpa_emitter_extension::JPABaseEmitter*);
void JPAManagerDeleteAllParticles(jpa_emitter_extension::JPABaseEmitter*,u32);
void JPAManagerListRemove(void*,void*);
void JPAManagerListPrepend(void*,void*);
void JPAManagerAssert(const char*,s32,const char*);
void JPAManagerUpdateViewPosition(jpa_emitter_extension::JPABaseEmitter*,s32);
void JPAManagerDrawIndirect(void*,void*,JPADrawParameters*);
void JPAManagerDrawFirstView(void*,void*,JPADrawParameters*);
void JPAManagerDrawSecondView(void*,void*,JPADrawParameters*);
void JPAManagerDraw(void*,void*,JPADrawParameters*);
// File scope, not in the anonymous namespace: the glue ext stubs JPAManagerFormatCounts/JPAManagerFormatEmitter take it,
// and a stub declared weak cannot have a parameter type with internal linkage.
struct ManagerPrintString { SafeString mString; be<u32> mSize; be<u8> mBuffer[512]; };
WWHD_SIZE(ManagerPrintString, 0x20C);
WWHD_OFFSET(ManagerPrintString, mBuffer, 0xC);
namespace {
struct GroupNameTable { SafeString names[16]; };
struct ManagerStaticBlock { be<u32> words[4]; };
static ManagerStaticBlock sStaticBlock;
static u32 sStaticRegistration;
static GroupNameTable sGroupNames;
static u32 sGroupNamesGuard;
static u32 sGroupNamesRegistration;
struct ManagerStringVtable { be<u32> mSlots[6]; };
static ManagerStringVtable sGroupStringVtable;
static ManagerStringVtable sPrintStringVtable;
static const char* const sGroupNameLiterals[16] = {
    "Normal",
    "Normal_P1",
    "Toon",
    "Toon_P1",
    "Projection",
    "ShipTail",
    "Wind",
    "2DFore",
    "2DBack",
    "2DMenu_Fore",
    "2DMenu_Back",
    "2DDrc_Fore",
    "2DDrc_Back",
    "UnDef",
    "UnDef",
    "UnDef",
};
}

/* 02820D7C */
void JPAEmitterInfoStorageDestroy(void* self, u32 deleting) {
    if (self && (deleting & 1)) JPAManagerDelete(self);
}
/* 02820D90 */
void* JPAEmitterInfoVectorConstruct(void* self) {
    if (!self) self = JPAManagerNew(0xC);
    return self;
}
/* 02820DBC */
void JPAEmitterInfoVectorDestroy(void*) {}
/* 02820DC0 */
void JPAEmitterGroupNamesDestroy() {
    JPAManagerDestroyArray(sGroupNames.names, 16, sizeof(SafeString),
        gabi::guest_fn(reinterpret_cast<const void*>(JPAFieldSafeStringDestroy)), 0, 0);
}

void JPAManagerListInitialize(void*);
void JPAManagerConstructArray(void*,u32,u32,u32);
void JPAManagerConstructDynamic(JPAManagerDynamicBuffer*);
void JPAManagerInitializeDynamic(JPAManagerDynamicBuffer*,void*,u32,u32);
void* JPAManagerCurrentHeap();
void JPAManagerInitializeGpuPools(JPAManagerGpuPools*,void*);
void* JPAManagerArrayAllocate(u32,void*,s32);
void* JPAManagerConstructAllocatedArray(void*,u32,u32,u32,u32);
s32 JPAManagerFindMaterial(JPAShaderProgram*,const SafeString*);
void JPAManagerInitializeMaterial(JPAShaderMaterial*,JPAShaderResource*,u32);
jpa_particle_extension::JPABaseParticle* JPABaseParticleConstruct(jpa_particle_extension::JPABaseParticle*);
jpa_emitter_extension::JPABaseEmitter* JPABaseEmitterConstruct(jpa_emitter_extension::JPABaseEmitter*);
namespace jpa_field_extension { struct JPAFieldDataObject; }
jpa_field_extension::JPAFieldDataObject* JPAFieldDataConstruct(jpa_field_extension::JPAFieldDataObject*);
namespace jpa_field_extension { struct JPAFieldDataObject; }
void* JPAFieldDataLinkAt(void*,u32);
void JPASetFieldVacantList(void*,void*);
namespace jpa_field_b { struct JPAFieldList; }
jpa_field_b::JPAFieldList* JPAFieldListConstruct(jpa_field_b::JPAFieldList*);
JPAGpuPool<4>* JPAGpuFieldPoolConstruct(JPAGpuPool<4>*);
JPAGpuPool<2>* JPAGpuParticlePoolConstruct(JPAGpuPool<2>*);
JPAGpuPool<10>* JPAGpuEmitterPoolConstruct(JPAGpuPool<10>*);
JPAGpuPool<300>* JPAGpuDrawPoolConstruct(JPAGpuPool<300>*);
namespace {
static const u32 sManagerArrayCookie;
struct ManagerDynamicSource { gptr<void> mData; };
static ManagerDynamicSource sDynamicSource;
template<class Pool,u32 N,class Constructor> void constructManagerPools(Pool (&pools)[N],Constructor constructor) {
    Pool* first = pools;
    if (gabi::ea(first) == 0) first = static_cast<Pool*>(JPAManagerNew(sizeof(pools)));
    if (first) JPAManagerConstructArray(first,N,sizeof(Pool),gabi::guest_fn(reinterpret_cast<const void*>(constructor)));
}
void* allocateManagerArray(u32 count,u32 stride,void* heap,u32 constructor,s32 line,const char* assertion) {
    void* allocation = JPAManagerArrayAllocate(count*stride+u32(sManagerArrayCookie),heap,4);
    if (!allocation) {
        JPAManagerAssert("JPAEmitterManager.cpp",line,assertion);
        return nullptr;
    }
    auto first = static_cast<u8*>(allocation)+u32(sManagerArrayCookie);
    void* objects = JPAManagerConstructAllocatedArray(first,count,stride,constructor,0);
    if (!objects) JPAManagerAssert("JPAEmitterManager.cpp",line,assertion);
    return objects;
}
}
/* 02820DE4 */
jpa_emitter_extension::JPAEmitterManager* JPAEmitterManagerConstruct(
    jpa_emitter_extension::JPAEmitterManager* self,JPAResourceManager* resource,
    u32 particleCount,u32 emitterCount,u32 fieldCount,void* heap,JPAShaderMaterialSet* materials) {
    if (!self) {
        self = static_cast<jpa_emitter_extension::JPAEmitterManager*>(JPAManagerNew(sizeof(jpa_emitter_extension::JPAEmitterManager)));
        if (!self) return nullptr;
    }
    jpa_emitter_calc::EmitterList* lists[3] = {&self->pools().mPtclPool,&self->pools().mEmtrPool,&self->pools().mFieldPool};
    for (u32 i = 0; i < 3; ++i) {
        void* list = lists[i];
        if (gabi::ea(list) == 0) list = JPAManagerNew(12);
        if (list) JPAManagerListInitialize(list);
    }
    JPAManagerConstructArray(self->mEmtrGroup,16,12,gabi::guest_fn(reinterpret_cast<const void*>(JPAFieldListConstruct)));
    JPAManagerConstructDynamic(&self->mParticleBuffer);
    JPAManagerConstructDynamic(&self->mEmitterBuffer);
    constructManagerPools(self->mGpuPools.mFieldPools,JPAGpuFieldPoolConstruct);
    constructManagerPools(self->mGpuPools.mParticlePools,JPAGpuParticlePoolConstruct);
    constructManagerPools(self->mGpuPools.mMediumPools,JPAGpuEmitterPoolConstruct);
    constructManagerPools(self->mGpuPools.mLargePools,JPAGpuDrawPoolConstruct);
    for (u32 i = 0; i < 4; ++i) {
        JPAManagerGpuDescriptor* descriptor = &self->mGpuPools.mDescriptors[i];
        if (gabi::ea(descriptor) == 0) descriptor = static_cast<JPAManagerGpuDescriptor*>(JPAManagerNew(16));
        if (descriptor) {
            descriptor->mWords[1] = 0;
            descriptor->mWords[2] = 0;
            descriptor->mWords[0] = 0;
            descriptor->mWords[3] = 0;
        }
    }
    if (!heap) heap = JPAManagerCurrentHeap();
    self->pools().mEmtrNum = emitterCount;
    self->pools().mFieldNum = fieldCount;
    self->pools().pResMgrArray[0] = resource;
    self->pools().mPtclNum = particleCount;
    self->mTextureRegistrationCount = 0;
    self->mMaterialSet = materials;
    gabi::Local<SafeString> name;
    name->mTextPtr = gabi::guest_addr("normal");
    name->__vtbl = gabi::ea(&sGroupStringVtable);
    JPAShaderMaterialSet* set = self->mMaterialSet;
    s32 materialIndex = JPAManagerFindMaterial(set->mProgram,name.get());
    JPAShaderMaterial* material = nullptr;
    if (materialIndex >= 0) {
        u32 count = set->mCount;
        JPAShaderMaterial* array = set->mMaterials;
        material = u32(materialIndex) < count ? &sead::elementAt(array, u32(materialIndex)) : array;
        if (!material->mReady) {
            JPAShaderProgram* program = set->mProgram;
            JPAShaderResource* shaderResource = nullptr;
            if (u32(materialIndex) < u32(program->mResourceCount)) shaderResource = &sead::elementAt(program->mResources, u32(materialIndex));
            JPAManagerInitializeMaterial(material,shaderResource,0);
            count = set->mCount;
            array = set->mMaterials;
        }
        material = u32(materialIndex) < count ? &sead::elementAt(array, u32(materialIndex)) : array;
    }
    self->mDefaultMaterial = material;
    JPATextureResource* textures = resource->pTexRes;
    void* defaultTexture = sead::elementAt(textures->mDescriptors, u32(textures->mDefaultTextureIndex));
    JPAManagerInitializeGpuPools(&self->mGpuPools,heap);
    self->mParticleBuffer.mFlags = 1;
    JPAManagerInitializeDynamic(&self->mEmitterBuffer,sDynamicSource.mData,9,10000);
    self->mEmitterBuffer.mFlags = 5;
    for (u32 i = 1; i < 8; ++i) self->pools().pResMgrArray[i] = nullptr;
    u32 count = self->pools().mPtclNum;
    auto particles = static_cast<jpa_particle_extension::JPABaseParticle*>(allocateManagerArray(count,0x128,heap,
        gabi::guest_fn(reinterpret_cast<const void*>(JPABaseParticleConstruct)),73,"pPtclArray"));
    for (u32 i = 0; i < u32(self->pools().mPtclNum); ++i) {
        auto particle = &particles[i];
        JPAManagerListPrepend(&self->pools().mPtclPool,particle);
        particle->initializeGpu(heap,defaultTexture);
        particle->drawState().mParticleBuffer = &self->mParticleBuffer;
        particle->poolTail().mOwner = &self->mGpuPools;
        particle->drawState().mEmitterBuffer = &self->mEmitterBuffer;
    }
    count = self->pools().mEmtrNum;
    auto emitters = static_cast<jpa_emitter_extension::JPABaseEmitter*>(allocateManagerArray(count,0x3FC,heap,
        gabi::guest_fn(reinterpret_cast<const void*>(JPABaseEmitterConstruct)),83,"pEmtrArray"));
    for (u32 i = 0; i < u32(self->pools().mEmtrNum); ++i) {
        auto emitter = &emitters[i];
        emitter->vacantParticlePool() = self;
        JPASetFieldVacantList(emitter->mFieldManager,&self->pools().mFieldPool);
        JPAManagerListPrepend(&self->pools().mEmtrPool,emitter->_08C);
        emitter->initGpuState(heap);
        emitter->gpu().mPoolOwner = &self->mGpuPools;
        emitter->initializeMaterialBindings(materials);
    }
    count = self->pools().mFieldNum;
    void* fields = allocateManagerArray(count,0x9C,heap,
        gabi::guest_fn(reinterpret_cast<const void*>(JPAFieldDataConstruct)),95,"pFieldArray");
    for (u32 i = 0; i < u32(self->pools().mFieldNum); ++i)
        JPAManagerListPrepend(&self->pools().mFieldPool,JPAFieldDataLinkAt(fields,i));
    return self;
}

void JPAManagerListAppend(void*,void*);
void JPAManagerInitializeDraw(void*,jpa_emitter_extension::JPABaseEmitter*,void*);
namespace {
struct ResourceEmitterHeader { gptr<jpa_emitter_calc::JPADataBlockLinkInfo> mpBlocks; };
struct ResourceEmitterData { gptr<ResourceEmitterHeader> mpHeader; };
}
/* 02821448 */
jpa_emitter_extension::JPABaseEmitter* jpa_emitter_extension::JPAEmitterManager::createSimpleEmitterID(
    const cXyz* position,u32 userID,u32 groupID,u32 resourceID,
    jpa_emitter_calc::EmitterCallback* emitterCallback,void* particleCallback) {
    if (groupID >= 16) JPAManagerAssert("JPAEmitterManager.cpp",121,"groupID < 16");
    if (resourceID >= 8) JPAManagerAssert("JPAEmitterManager.cpp",122,"rmID < 8");
    if (!pools().pResMgrArray[resourceID])
        JPAManagerAssert("JPAEmitterManager.cpp",123,"pResMgrArray[rmID] != (0)");
    JPAResourceManager* resources = static_cast<JPAResourceManager*>(pools().pResMgrArray[resourceID].get());
    JPAEmitterData* data = resources->pEmtrRes->getByUserIndex(userID);
    JPABaseEmitter* emitter = nullptr;
    auto freeList = reinterpret_cast<::JPAParticleList*>(&pools().mEmtrPool);
    if (data && u32(freeList->mCount) != 0) {
        emitter = reinterpret_cast<JPABaseEmitter*>(freeList->mpHead->mpObject.get());
        JPAManagerListRemove(&pools().mEmtrPool,emitter->_08C);
        JPAManagerListAppend(&mEmtrGroup[groupID],emitter->_08C);
        emitter->gpu().mNoBloom = 0;
        for (s32 i = 0; i < s32(u16(mTextureRegistrationCount)); ++i) {
            if (userID == u32(mTextureRegistrations[i].mID))
                emitter->gpu().mNoBloom = gabi::ea(mTextureRegistrations[i].mTexture.get());
        }
        emitter->gpu().mResMgrID = u8(resourceID);
        emitter->gpu().mResourceIndex = u16(userID);
        emitter->gpu().mGroupID = u8(groupID);
        auto resource = reinterpret_cast<ResourceEmitterData*>(data);
        emitter->create(resource->mpHeader->mpBlocks);
        resources = static_cast<JPAResourceManager*>(pools().pResMgrArray[resourceID].get());
        JPAManagerInitializeDraw(emitter->mDraw,emitter,resources->pTexRes);
        emitter->mpParticleCallBack = gabi::ea(particleCallback);
        emitter->mpEmitterCallBack = emitterCallback;
        emitter->mGlobalTranslation[0] = f32(position->x);
        emitter->mGlobalTranslation[1] = f32(position->y);
        emitter->mGlobalTranslation[2] = f32(position->z);
        if (u8(emitter->gpu().mGroupID) >= 7)
            emitter->mGlobalTranslation[1] = -f32(emitter->mGlobalTranslation[1]);
    }
    return emitter;
}
/* 028215F8 */
void jpa_emitter_extension::JPAEmitterManager::deleteEmitter(jpa_emitter_extension::JPABaseEmitter* emitter) {
    JPAManagerDeleteAllFields(emitter->mFieldManager);
    emitter->releaseGpuPools();
    emitter->mFlags = u32(emitter->mFlags) | 4;
    auto deferred = reinterpret_cast<JPAParticleVector*>(emitter->_1C4);
    JPAManagerClearDeferred(&deferred[0]);
    JPAManagerClearDeferred(&deferred[1]);
    u32 group = emitter->gpu().mGroupID;
    JPAManagerListRemove(&mEmtrGroup[group], emitter->_08C);
    JPAManagerListPrepend(&pools().mEmtrPool, emitter->_08C);
}

void JPAManagerFormatCounts(ManagerPrintString*,const char*,u32,u32);
void JPAManagerFormatEmitter(ManagerPrintString*,const char*,const char*,u32,const char*,u32,f64,f64,f64);
const char* JPAManagerEmitterName(u32);
/* 02821D28 */
void jpa_emitter_extension::JPAEmitterManager::printStatus() {
    if (sGroupNamesGuard == 0) {
        sGroupNamesGuard = 1;
        for (u32 i = 0; i < 16; ++i) {
            sGroupNames.names[i].__vtbl = gabi::ea(&sGroupStringVtable);
            sGroupNames.names[i].mTextPtr = gabi::guest_addr(sGroupNameLiterals[i]);
        }
        JPAManagerRegister(&sGroupNamesRegistration);
    }
    u32 freeParticles = reinterpret_cast<::JPAParticleList*>(&pools().mPtclPool)->mCount;
    u32 emitterCapacity = pools().mEmtrNum;
    u32 particleCapacity = pools().mPtclNum;
    u32 freeEmitters = reinterpret_cast<::JPAParticleList*>(&pools().mEmtrPool)->mCount;
    gabi::Local<ManagerPrintString> output;
    output->mSize = 512;
    output->mString.mTextPtr = gabi::ea(output->mBuffer);
    output->mString.__vtbl = gabi::ea(&sPrintStringVtable);
    output->mBuffer[511] = 0;
    output->mBuffer[0] = 0;
    gabi::cpu->cr[6] = 0; // Cafe variadic ABI: this call has no floating arguments.
    JPAManagerFormatCounts(output.get(),"Emitter Num[%d] Particle Num[%d]",emitterCapacity-freeEmitters,particleCapacity-freeParticles);
    for (u32 group = 0; group < 16; ++group) {
        auto node = mEmtrGroup[group].first.get();
        while (node) {
            auto emitter = static_cast<JPABaseEmitter*>(node->object.get());
            bool active = (u32(emitter->mFlags) & 4) == 0;
            node = node->next;
            if (!active) continue;
            f64 y = f32(emitter->mGlobalTranslation[1]);
            f64 z = f32(emitter->mGlobalTranslation[2]);
            f64 x = f32(emitter->mGlobalTranslation[0]);
            SafeString* groupName = &sGroupNames.names[group];
            auto table = gabi::at<ManagerStringVtable>(u32(groupName->__vtbl));
            gabi::call_ptr<void>(table->mSlots[5],groupName);
            u32 particles = u32(emitter->mActiveParticleCount) + u32(emitter->mChildParticleCount);
            u32 userID = emitter->gpu().mResourceIndex;
            const char* groupText = gabi::at<const char>(u32(groupName->mTextPtr));
            const char* name = JPAManagerEmitterName(userID);
            userID = emitter->gpu().mResourceIndex;
            gabi::cpu->cr[6] = 1; // Cafe variadic ABI: f1..f3 carry the translation.
            JPAManagerFormatEmitter(output.get(),"%s[ID=%d %s] (%.0f %.0f %.0f) %d",name,userID,groupText,particles,x,y,z);
        }
    }
}
/* 0282215C */
void jpa_emitter_extension::JPAEmitterManager::forceDeleteEmitter(jpa_emitter_extension::JPABaseEmitter* emitter, u32 immediate) {
    // HD: immediate deletion drains GPU-deferred particles; normal deletion requests a delay.
    if (immediate) {
        JPAManagerRequestDelete(emitter);
        JPAManagerDeleteAllParticles(emitter, 0);
        deleteEmitter(emitter);
    } else if (!u32(emitter->gpu().mDeleteRequested)) {
        emitter->gpu().mDeleteRequested = 1;
        JPAManagerRequestDelete(emitter);
    }
}

/* 028221EC */
void jpa_emitter_extension::JPAEmitterManager::clearResourceManager(u32 rmID) {
    if (rmID >= 8) JPAManagerAssert("JPAEmitterManager.cpp", 621, "rmID < 8");
    for (u32 group = 0; group < 16; ++group) {
        jpa_emitter_calc::EmitterLink* link = mEmtrGroup[group].first;
        while (link) {
            auto emitter = static_cast<jpa_emitter_extension::JPABaseEmitter*>(link->object.get());
            u32 resource = emitter->gpu().mResMgrID;
            link = link->next;
            if (resource == rmID) forceDeleteEmitter(emitter, 0);
        }
    }
    pools().pResMgrArray[rmID] = nullptr;
}

/* 0282228C */
void jpa_emitter_extension::JPAEmitterManager::forceDeleteAllEmitter() {
    for (u32 group = 0; group < 16; ++group) {
        jpa_emitter_calc::EmitterLink* link = mEmtrGroup[group].first;
        while (link) {
            jpa_emitter_calc::EmitterLink* next = link->next;
            auto emitter = static_cast<jpa_emitter_extension::JPABaseEmitter*>(link->object.get());
            forceDeleteEmitter(emitter, 1);
            link = next;
        }
    }
}

static void copyDrawParameters(JPADrawParameters* dst, const JPADrawParameters* src) {
    for (u32 i = 0; i < 16; ++i) dst->words[i] = u32(src->words[i]);
}

/* 02821734 */
void jpa_emitter_extension::JPAEmitterManager::drawView(jpa_emitter_extension::JPADrawInfo* info, u32 group) {
    if (group >= 16) JPAManagerAssert("JPAEmitterManager.cpp", 356, "groupID < 16");
    u32 view = JPAGroupToView(group);
    jpa_emitter_calc::emtrInfo.mCameraFovy[view] = gabi::fmuls_ppc(info->mFovy, 57.2957763671875f);
    // HD: the aspect is a pure word copy, including signalling-NaN payloads.
    *reinterpret_cast<be<u32>*>(&jpa_emitter_calc::emtrInfo.mCameraAspect[view]) = u32(*reinterpret_cast<be<u32>*>(&info->mAspect));
    jpa_emitter_calc::EmitterLink* link = mEmtrGroup[group].first;
    JPADrawParameters parameters;
    while (link) {
        auto emitter = static_cast<jpa_emitter_extension::JPABaseEmitter*>(link->object.get());
        if (!u32(emitter->mDeleteCountdown) && !u32(emitter->mClearCountdown) && !(u32(emitter->mFlags) & 4)) {
            JPAManagerUpdateViewPosition(emitter, -1);
            copyDrawParameters(&parameters, &info->mParameters);
            emitter->drawView(info->mCameraMtx, &parameters);
        }
        // HD: drawing may change the link; this load remains after the callback.
        link = link->next;
    }
}

/* 02821868 */
void jpa_emitter_extension::JPAEmitterManager::drawIndirect(jpa_emitter_extension::JPADrawInfo* info, u32 group) {
    if (group >= 16) JPAManagerAssert("JPAEmitterManager.cpp", 408, "groupID < 16");
    u32 view = JPAGroupToView(group);
    jpa_emitter_calc::emtrInfo.mCameraFovy[view] = gabi::fmuls_ppc(info->mFovy, 57.2957763671875f);
    // HD: the aspect is a pure word copy, including signalling-NaN payloads.
    *reinterpret_cast<be<u32>*>(&jpa_emitter_calc::emtrInfo.mCameraAspect[view]) = u32(*reinterpret_cast<be<u32>*>(&info->mAspect));
    jpa_emitter_calc::EmitterLink* link = mEmtrGroup[group].first;
    JPADrawParameters parameters;
    while (link) {
        auto emitter = static_cast<jpa_emitter_extension::JPABaseEmitter*>(link->object.get());
        if (!u32(emitter->mDeleteCountdown) && !u32(emitter->mClearCountdown) && !(u32(emitter->mFlags) & 4)) {
            JPAManagerUpdateViewPosition(emitter, -1);
            copyDrawParameters(&parameters, &info->mParameters);
            JPAManagerDrawIndirect(emitter->mDraw, info->mCameraMtx, &parameters);
        }
        // HD: drawing may change the link; this load remains after the callback.
        link = link->next;
    }
}

/* 028219A8 */
void jpa_emitter_extension::JPAEmitterManager::drawFirstView(jpa_emitter_extension::JPADrawInfo* info, u32 group) {
    if (group >= 16) JPAManagerAssert("JPAEmitterManager.cpp", 444, "groupID < 16");
    u32 view = JPAGroupToView(group);
    jpa_emitter_calc::emtrInfo.mCameraFovy[view] = gabi::fmuls_ppc(info->mFovy, 57.2957763671875f);
    // HD: the aspect is a pure word copy, including signalling-NaN payloads.
    *reinterpret_cast<be<u32>*>(&jpa_emitter_calc::emtrInfo.mCameraAspect[view]) = u32(*reinterpret_cast<be<u32>*>(&info->mAspect));
    jpa_emitter_calc::EmitterLink* link = mEmtrGroup[group].first;
    JPADrawParameters parameters;
    while (link) {
        auto emitter = static_cast<jpa_emitter_extension::JPABaseEmitter*>(link->object.get());
        if (!u32(emitter->mDeleteCountdown) && !u32(emitter->mClearCountdown) && !(u32(emitter->mFlags) & 4)) {
            JPAManagerUpdateViewPosition(emitter, 0);
            copyDrawParameters(&parameters, &info->mParameters);
            JPAManagerDrawFirstView(emitter->mDraw, info->mCameraMtx, &parameters);
        }
        // HD: drawing may change the link; this load remains after the callback.
        link = link->next;
    }
}

/* 02821AE8 */
void jpa_emitter_extension::JPAEmitterManager::drawSecondView(jpa_emitter_extension::JPADrawInfo* info, u32 group) {
    if (group >= 16) JPAManagerAssert("JPAEmitterManager.cpp", 479, "groupID < 16");
    u32 view = JPAGroupToView(group);
    jpa_emitter_calc::emtrInfo.mCameraFovy[view] = gabi::fmuls_ppc(info->mFovy, 57.2957763671875f);
    // HD: the aspect is a pure word copy, including signalling-NaN payloads.
    *reinterpret_cast<be<u32>*>(&jpa_emitter_calc::emtrInfo.mCameraAspect[view]) = u32(*reinterpret_cast<be<u32>*>(&info->mAspect));
    jpa_emitter_calc::EmitterLink* link = mEmtrGroup[group].first;
    JPADrawParameters parameters;
    while (link) {
        auto emitter = static_cast<jpa_emitter_extension::JPABaseEmitter*>(link->object.get());
        if (!u32(emitter->mDeleteCountdown) && !u32(emitter->mClearCountdown) && !(u32(emitter->mFlags) & 4)) {
            JPAManagerUpdateViewPosition(emitter, 1);
            copyDrawParameters(&parameters, &info->mParameters);
            JPAManagerDrawSecondView(emitter->mDraw, info->mCameraMtx, &parameters);
        }
        // HD: drawing may change the link; this load remains after the callback.
        link = link->next;
    }
}

/* 02821C28 */
void jpa_emitter_extension::JPAEmitterManager::draw(jpa_emitter_extension::JPADrawInfo* info, u32 group) {
    if (group >= 16) JPAManagerAssert("JPAEmitterManager.cpp", 315, "groupID < 16");
    jpa_emitter_calc::EmitterLink* link = mEmtrGroup[group].first;
    JPADrawParameters parameters;
    while (link) {
        auto emitter = static_cast<jpa_emitter_extension::JPABaseEmitter*>(link->object.get());
        if (!u32(emitter->mDeleteCountdown) && !u32(emitter->mClearCountdown) && !(u32(emitter->mFlags) & 4)) {
            emitter->gpu().mTextureContext = info->mTextureContext.get();
            copyDrawParameters(&parameters, &info->mParameters);
            JPAManagerDraw(emitter->mDraw, info->mCameraMtx, &parameters);
        }
        // HD: drawing may change the link; this load remains after the callback.
        link = link->next;
    }
}

/* 02822304 */
void jpa_emitter_extension::JPAEmitterManager::registerTexture(void* texture, u32 id) {
    if (!texture) return;
    u32 index = mTextureRegistrationCount;
    mTextureRegistrations[index].mTexture = texture;
    index = mTextureRegistrationCount;
    mTextureRegistrations[index].mID = u16(id);
    mTextureRegistrationCount = u16(u32(mTextureRegistrationCount) + 1);
}

// HD: compiler-generated initialization of this translation unit.
/* 02822340 */
void __sinit_JPAEmitterManager_cpp() {
    sStaticBlock.words[2] = 0;
    sStaticBlock.words[0] = 0;
    sStaticBlock.words[3] = 0;
    sStaticBlock.words[1] = 0;
    JPAManagerRegister(&sStaticRegistration);
}

/* WWHD glue
data sManagerArrayCookie 101FCD08
data sDynamicSource 104B4D2C
ext 027EC678 void JPAManagerListInitialize(void*)
ext 028EFFD0 void JPAManagerConstructArray(void*,u32,u32,u32)
ext 027B5430 void JPAManagerConstructDynamic(JPAManagerDynamicBuffer*)
ext 027B54E0 void JPAManagerInitializeDynamic(JPAManagerDynamicBuffer*,void*,u32,u32)
ext 027EC230 void* JPAManagerCurrentHeap()
ext 0282F050 void JPAManagerInitializeGpuPools(JPAManagerGpuPools*,void*)
ext 0273B0D4 void* JPAManagerArrayAllocate(u32,void*,s32)
ext 028EFF8C void* JPAManagerConstructAllocatedArray(void*,u32,u32,u32,u32)
ext 027B90AC s32 JPAManagerFindMaterial(JPAShaderProgram*,const SafeString*)
ext 02800B0C void JPAManagerInitializeMaterial(JPAShaderMaterial*,JPAShaderResource*,u32)
str 10170DF0 "normal" in 02820DE4
str 10170E04 "JPAEmitterManager.cpp" in 02820DE4
str 10170E1C "pPtclArray" in 02820DE4
str 10170E28 "pEmtrArray" in 02820DE4
str 10170DF8 "pFieldArray" in 02820DE4

data sGroupNamesGuard 104B5934
data sGroupNamesRegistration 101FA058
data sGroupStringVtable 10170D80
data sPrintStringVtable 10170DC8
ext 02759C28 void JPAManagerFormatCounts(ManagerPrintString*,const char*,u32,u32)
ext 02759C28 void JPAManagerFormatEmitter(ManagerPrintString*,const char*,const char*,u32,const char*,u32,f64,f64,f64)
ext 02843CAC const char* JPAManagerEmitterName(u32)
str 10170F88 "Normal" in 02821D28
str 10170FC8 "Normal_P1" in 02821D28
str 10170F90 "Toon" in 02821D28
str 10170F78 "Toon_P1" in 02821D28
str 10170FD4 "Projection" in 02821D28
str 10170FE0 "ShipTail" in 02821D28
str 10170F98 "Wind" in 02821D28
str 10170FA0 "2DFore" in 02821D28
str 10170FA8 "2DBack" in 02821D28
str 10170FB0 "2DMenu_Fore" in 02821D28
str 10170FBC "2DMenu_Back" in 02821D28
str 10170FEC "2DDrc_Fore" in 02821D28
str 10170FF8 "2DDrc_Back" in 02821D28
str 10170F80 "UnDef" in 02821D28
str 10171004 "Emitter Num[%d] Particle Num[%d]" in 02821D28
str 10171028 "%s[ID=%d %s] (%.0f %.0f %.0f) %d" in 02821D28

ext 027EC6F4 void JPAManagerListAppend(void*,void*)
ext 02829F10 void JPAManagerInitializeDraw(void*,jpa_emitter_extension::JPABaseEmitter*,void*)
str 10170E34 "JPAEmitterManager.cpp" in 02821448
str 10170E68 "groupID < 16" in 02821448
str 10170E78 "rmID < 8" in 02821448
str 10170E4C "pResMgrArray[rmID] != (0)" in 02821448
data sStaticBlock 104B58A4
data sStaticRegistration 101FA064
ext 028F026C void JPAManagerRegister(void*)
ext 0281FFDC void JPAManagerUpdateViewPosition(jpa_emitter_extension::JPABaseEmitter*,s32)
ext 0282C1EC void JPAManagerDrawIndirect(void*,void*,JPADrawParameters*)
ext 0282CB9C void JPAManagerDrawFirstView(void*,void*,JPADrawParameters*)
ext 0282CCAC void JPAManagerDrawSecondView(void*,void*,JPADrawParameters*)
ext 0282D9C0 void JPAManagerDraw(void*,void*,JPADrawParameters*)
str 10170EB0 "JPAEmitterManager.cpp" in 02821734
str 10170EC8 "groupID < 16" in 02821734
str 10170ED8 "JPAEmitterManager.cpp" in 02821868
str 10170EF0 "groupID < 16" in 02821868
str 10170F00 "JPAEmitterManager.cpp" in 028219A8
str 10170F18 "groupID < 16" in 028219A8
str 10170F28 "JPAEmitterManager.cpp" in 02821AE8
str 10170F40 "groupID < 16" in 02821AE8
str 10170F50 "JPAEmitterManager.cpp" in 02821C28
str 10170F68 "groupID < 16" in 02821C28
str 1017104C "JPAEmitterManager.cpp" in 028221EC
str 10171064 "rmID < 8" in 028221EC
ext 028243FC void JPAManagerDeleteAllFields(void*)
ext 0273B5DC void JPAManagerClearDeferred(JPAParticleVector*)
ext 0281DD6C void JPAManagerRequestDelete(jpa_emitter_extension::JPABaseEmitter*)
ext 0281DE68 void JPAManagerDeleteAllParticles(jpa_emitter_extension::JPABaseEmitter*,u32)
ext 027EC560 void JPAManagerListRemove(void*,void*)
ext 027EC778 void JPAManagerListPrepend(void*,void*)
ext 0273AA24 void JPAManagerAssert(const char*,s32,const char*)
data sGroupNames 104B58B4
ext 0273AD10 void* JPAManagerNew(u32)
ext 0273AF40 void JPAManagerDelete(void*)
ext 028F0164 void JPAManagerDestroyArray(void*,u32,u32,u32,u32,u32)
*/

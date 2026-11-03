// HD emitter GPU pool ownership and view grouping, written from the Cafe binary.
#include "JSystem/JParticle/JPAEmitter.h"
#include "sead/ElementAt.h"
#include "d/d_com_inf_game.h"
using namespace jpa_emitter_extension;
void* JPAEmitterAcquireMediumPool(void*, be<u32>*);
void* JPAEmitterAcquireLargePool(void*, be<u32>*);
void* JPAEmitterAcquireSmallPool(void*, be<u32>*);
void JPAEmitterReleaseMediumPool(void*, u32);
void JPAEmitterReleaseLargePool(void*, u32);
void JPAEmitterReleaseSmallPool(void*, u32);
void JPAEmitterDrawView(void*, void*, JPADrawParameters*);
namespace {
static const u8 sGroupToView[13] = {0,0,0,0,0,0,0,1,1,1,1,2,2};
}

/* 0281E048 */
u8 jpa_emitter_extension::JPABaseEmitter::getPivotX() {
    auto blocks = mpDataLinkInfo.get();
    if (!blocks->mpExtraShape) return 1;
    JPAExtraShapePivot* shape = static_cast<JPAExtraShapePivot*>(blocks->mpExtraShape.get());
    return u8(gabi::call_ptr<u32>(shape->__vtbl->getPivotX, shape));
}
/* 0281E094 */
u8 jpa_emitter_extension::JPABaseEmitter::getPivotY() {
    auto blocks = mpDataLinkInfo.get();
    if (!blocks->mpExtraShape) return 1;
    JPAExtraShapePivot* shape = static_cast<JPAExtraShapePivot*>(blocks->mpExtraShape.get());
    return u8(gabi::call_ptr<u32>(shape->__vtbl->getPivotY, shape));
}
s32 JPAEmitterFindMaterial(JPAShaderProgram*, const SafeString*);
void JPAEmitterInitializeMaterial(JPAShaderMaterial*, JPAShaderResource*, u32);
namespace {
struct MaterialStringVtable { be<u32> mSlots[6]; };
static MaterialStringVtable sMaterialStringVtable;
JPAShaderMaterial* findMaterial(JPAShaderMaterialSet* set, const SafeString* name) {
    s32 index = JPAEmitterFindMaterial(set->mProgram, name);
    if (index < 0) return nullptr;
    u32 count = set->mCount;
    JPAShaderMaterial* materials = set->mMaterials;
    JPAShaderMaterial* material = u32(index) < count ? &sead::elementAt(materials, u32(index)) : materials;
    if (!material->mReady) {
        JPAShaderProgram* program = set->mProgram;
        JPAShaderResource* resource = nullptr;
        if (u32(index) < u32(program->mResourceCount)) resource = &sead::elementAt(program->mResources, u32(index));
        JPAEmitterInitializeMaterial(material, resource, 0);
        count = set->mCount;
        materials = set->mMaterials;
    }
    return u32(index) < count ? &sead::elementAt(materials, u32(index)) : materials;
}
}
/* 0281E26C */
void jpa_emitter_extension::JPABaseEmitter::initializeMaterialBindings(JPAShaderMaterialSet* set) {
    gabi::Local<SafeString> name;
    name->mTextPtr = gabi::guest_addr("normal");
    name->__vtbl = gabi::ea(&sMaterialStringVtable);
    gpu().mMaterialBindings[0] = findMaterial(set, name.get());
    name->mTextPtr = gabi::guest_addr("no_bloom");
    name->__vtbl = gabi::ea(&sMaterialStringVtable);
    gpu().mMaterialBindings[1] = findMaterial(set, name.get());
}
JPAShaderMaterialSet* JPAEmitterGetMaterialSet();
u32 JPAEmitterColorEquals(const be<u32>*, const be<f32>*);
namespace { struct RenderColorWords { be<u32> mWords[4]; }; struct RenderColor { be<f32> mValues[4]; }; }
/* 0281E5A8 */
void jpa_emitter_extension::JPABaseEmitter::applyRenderInfo(JPAEmitterRenderInfo* info) {
    const be<u8>* name = gabi::at<const be<u8>>(u32(info->mName));
    if (u8(*name) != 0) {
        JPAShaderMaterialSet* set = JPAEmitterGetMaterialSet();
        gpu().mDrawContext = gabi::ea(findMaterial(set, reinterpret_cast<SafeString*>(info)));
        gpu().mDrawContextOverride = 1;
    }
    auto colors = reinterpret_cast<RenderColorWords*>(gpu().mColorScale);
    colors->mWords[0] = u32(info->mColorBits[0]);
    colors->mWords[1] = u32(info->mColorBits[1]);
    colors->mWords[2] = u32(info->mColorBits[2]);
    colors->mWords[3] = u32(info->mColorBits[3]);
    gabi::Local<RenderColor> white;
    for (u32 i = 0; i < 4; ++i) white->mValues[i] = 1.0f;
    if (u8(JPAEmitterColorEquals(info->mColorBits, white->mValues) ^ 1) != 0)
        gpu().mApplyColorScale = 1;
    gpu().mRenderEnabled = u8(info->mEnabled);
    gpu().mRenderPass = u8(info->mPass);
}
void JPAEmitterGetTextureName(void*, SafeString*, u32);
namespace {
void assureMaterialName(SafeString* name) {
    auto table = gabi::at<MaterialStringVtable>(u32(name->__vtbl));
    gabi::call_ptr<void>(table->mSlots[5], name);
}
}
/* 0281E440 */
void jpa_emitter_extension::JPABaseEmitter::selectTextureVariant(u32 selector) {
    auto blocks = mpDataLinkInfo.get();
    JPABaseShapeToneView* base = static_cast<JPABaseShapeToneView*>(blocks->mpBaseShape.get());
    u8 variant = u8((u32(gpu().mTextureVariant) & 0xFE) + selector);
    u32 texture = gabi::call_ptr<u32>(base->__vtbl->getTextureIndex, base);
    u32 mappedTexture = sead::elementAt(blocks->textureMap, texture);
    gabi::Local<SafeString> name;
    JPAEmitterGetTextureName(mDraw, name.get(), mappedTexture);
    gabi::Local<SafeString> excluded;
    excluded->mTextPtr = gabi::guest_addr("AK_kagerouSwap00");
    excluded->__vtbl = gabi::ea(&sMaterialStringVtable);
    assureMaterialName(name.get());
    assureMaterialName(name.get());
    u32 before = name->mTextPtr;
    assureMaterialName(excluded.get());
    u32 expected = excluded->mTextPtr;
    bool equal = before == expected;
    if (!equal) {
        const be<u8>* left = gabi::at<const be<u8>>(u32(name->mTextPtr));
        const be<u8>* right = gabi::at<const be<u8>>(expected);
        for (u32 i = 0; i < 0x40001; ++i) {
            u32 a = left[i], b = right[i];
            if (a != b) break;
            if (a == 0) { equal = true; break; }
        }
    }
    if (!equal) {
        auto material = static_cast<JPAShaderMaterial*>(gpu().mMaterialBindings[gpu().mNoBloom != 0].get());
        JPAShaderVariantTable* variants = material->mVariants;
        if (variants) material = variant < u32(variants->mCount) ? &variants->mEntries[variant] : nullptr;
        gpu().mDrawContext = gabi::ea(material);
    }
}
/* 0281E728 */
void jpa_emitter_extension::JPABaseEmitter::acquireMediumGpuPool() {
    if (!gpu().mMediumPool)
        gpu().mMediumPool = JPAEmitterAcquireMediumPool(gpu().mPoolOwner, &gpu().mMediumPoolIndex);
}
/* 0281E76C */
void jpa_emitter_extension::JPABaseEmitter::acquireLargeGpuPool() {
    if (!gpu().mLargePool)
        gpu().mLargePool = JPAEmitterAcquireLargePool(gpu().mPoolOwner, &gpu().mLargePoolIndex);
}
/* 0281E7B0 */
void jpa_emitter_extension::JPABaseEmitter::acquireSmallGpuPool() {
    if (!gpu().mSmallPool)
        gpu().mSmallPool = JPAEmitterAcquireSmallPool(gpu().mPoolOwner, &gpu().mSmallPoolIndex);
}
/* 0281E7F4 */
void jpa_emitter_extension::JPABaseEmitter::releaseGpuPools() {
    if (gpu().mMediumPool) {
        JPAEmitterReleaseMediumPool(gpu().mPoolOwner, gpu().mMediumPoolIndex);
        gpu().mMediumPoolIndex = 0;
        gpu().mMediumPool = nullptr;
    }
    if (gpu().mLargePool) {
        JPAEmitterReleaseLargePool(gpu().mPoolOwner, gpu().mLargePoolIndex);
        gpu().mLargePoolIndex = 0;
        gpu().mLargePool = nullptr;
    }
    if (gpu().mSmallPool) {
        JPAEmitterReleaseSmallPool(gpu().mPoolOwner, gpu().mSmallPoolIndex);
        gpu().mSmallPoolIndex = 0;
        gpu().mSmallPool = nullptr;
    }
    for (u32 i = 0; i < 2; ++i) {
        if (gpu().mParticlePools[i]) {
            JPAEmitterReleaseSmallPool(gpu().mPoolOwner, gpu().mParticlePoolIndices[i]);
            gpu().mParticlePools[i] = nullptr;
            gpu().mParticlePoolIndices[i] = 0;
        }
    }
}
/* 0281E8C0 */
bool jpa_emitter_extension::JPABaseEmitter::acquireParticleGpuPool(u32 index) {
    if (sead::elementAt(gpu().mParticlePools, index)) return false;
    sead::elementAt(gpu().mParticlePools, index) = JPAEmitterAcquireSmallPool(gpu().mPoolOwner, &sead::elementAt(gpu().mParticlePoolIndices, index));
    return true;
}
/* 0281E934 */
u32 JPAGroupToView(u32 group) {
    return group <= 12 ? sGroupToView[group] : 0;
}
/* 0281E950 */
u32 jpa_emitter_extension::JPABaseEmitter::getViewGroup() { return JPAGroupToView(gpu().mGroupID); }
/* 028200BC */
void jpa_emitter_extension::JPABaseEmitter::drawView(void* camera, JPADrawParameters* parameters) {
    JPAEmitterDrawView(mDraw, camera, parameters);
}

/* WWHD glue
str 10170D30 "AK_kagerouSwap00" in 0281E440
ext 02829EAC void JPAEmitterGetTextureName(void*,SafeString*,u32)
ext 027FFCBC JPAShaderMaterialSet* JPAEmitterGetMaterialSet()
ext 0274D4C0 u32 JPAEmitterColorEquals(const be<u32>*,const be<f32>*)
data sMaterialStringVtable 10170CFC
str 10170D1C "normal" in 0281E26C
str 10170D24 "no_bloom" in 0281E26C
ext 027B90AC s32 JPAEmitterFindMaterial(JPAShaderProgram*,const SafeString*)
ext 02800B0C void JPAEmitterInitializeMaterial(JPAShaderMaterial*,JPAShaderResource*,u32)
data sGroupToView 10170D44
ext 0282FBF0 void* JPAEmitterAcquireMediumPool(void*,be<u32>*)
ext 0282FCC4 void* JPAEmitterAcquireLargePool(void*,be<u32>*)
ext 0282FA58 void* JPAEmitterAcquireSmallPool(void*,be<u32>*)
ext 0282FC7C void JPAEmitterReleaseMediumPool(void*,u32)
ext 0282FD58 void JPAEmitterReleaseLargePool(void*,u32)
ext 0282FAD4 void JPAEmitterReleaseSmallPool(void*,u32)
ext 0282BEC8 void JPAEmitterDrawView(void*,void*,JPADrawParameters*)
*/

/* agl shadow map: construction, depth-texture allocation and release, and the per-layer finish (HiZ expand). */
// GX2 depth-buffer HiZ values: wut facts.
#include "agl/ShadowMap.h"
#include "agl/ShaderContext.h"
#include "agl/ParameterConstruction.h"
using namespace agl;
// The dynamic texture cache singleton (pointer word owned by agl/DynamicTextureCache.cpp).
static TextureCacheWord sTextureCache;
// Vtables of the map (its two bases) and of the embedded resource record, as this source stores them.
static u8 sShadowMapMethods;
static u8 sShadowMapInterfaceMethods;
static u8 sResourceRecordMethods;
static u8 sResourceRecordInterfaceMethods;
void freeShadowMap(void* storage);
/* 02796558 */
void agl::ShadowMap::releaseDepthTexture() {
    gabi::AutoFrame frame{0x02796558};
    TextureCacheEntry* texture = mDepthTexture;
    if (texture != NULL) {
        u32 shared = mUseSharedCache;
        DynamicTextureCache* cache = sTextureCache;
        if (shared != 0) cache->releaseShared(texture);
        else cache->release(texture);
        mDepthTexture = NULL;
    }
}
/* 027965BC */
void agl::ShadowMap::releaseTextures() {
    gabi::AutoFrame frame{0x027965BC};
    releaseDepthTexture();
    TextureCacheEntry* texture = mFilterTexture;
    if (texture != NULL) {
        u32 shared = mUseSharedCache;
        DynamicTextureCache* cache = sTextureCache;
        if (shared != 0) cache->releaseShared(texture);
        else cache->release(texture);
        mFilterTexture = NULL;
    }
}
/* 02796624 */
void agl::ShadowMap::destroy(ShadowMap* map, u32 flags) {
    gabi::AutoFrame frame{0x02796624};
    if (map != NULL) {
        map->mParameters.mVtable = (parameter_tree::ValueVtable*)&sShadowMapMethods;
        map->mInterfaceMethods = &sShadowMapInterfaceMethods;
        map->releaseTextures();
        // ~ResourceRecord, inlined
        map->mResource.detach();
        map->mResource.mVtable = (sead::DisposerVtable*)&sResourceRecordMethods;
        map->mResource.mInterfaceMethods = &sResourceRecordInterfaceMethods;
        map->mResource.detach();
        map->mResource.sead::RegisteredObject::destroy(0);
        TextureSampler::destroy(&map->mFilterInput, 2);
        map->mFrame.destroy(2);
        TextureSampler::destroy(&map->mFilterSampler, 2);
        TextureSampler::destroy(&map->mDepthSampler, 2);
        if ((flags & 1) != 0) freeShadowMap(map);
    }
}
// The shader context singleton, re-applied after the HiZ expand (pointer word owned by agl/ShaderContext.cpp).
static ShaderContextWord sShaderContext;
void GX2ExpandDepthBuffer(PlatformDepthBuffer_l* depth);
/* 02796EEC */
void agl::ShadowMap::finishLayer() {
    gabi::AutoFrame frame{0x02796EEC};
    mDepth.invalidate();
    if ((u32)mEnableHiZ.mValue != 0 && mDepth.mHiZBuffer.v != 0) {
        if ((u32)mDepth.mBase.mDirty != 0) {
            mDepth.initializeRegisters();
            mDepth.mBase.mDirty = 0;
        }
        GX2ExpandDepthBuffer(&mDepth.mDepth);
        sShaderContext->applyContext();
    }
    mDepthSampler.texture.invalidate();
    mCurrentTexture = &mDepthSampler;
}
// This source's SafeString vtable and its parameter vtables.
static sead::SafeTextVtable sShadowTextMethods;
static u8 sShadowIntegerMethods;
static u8 sShadowBooleanMethods;
static u8 sShadowFloatMethods;
// Meta texts of the parameters (each use has its own copy of the empty text).
static TextByte sShadowIntegerMeta;
static TextByte sShadowBooleanMeta;
static TextByte sShadowSizeMeta;
static TextByte sShadowMarginMeta;
void* allocateShadowMap(u32 size);
/* 02796034 */
agl::ShadowMap* agl::ShadowMap::create(ShadowMap* map) {
    gabi::AutoFrame frame{0x02796034};
    if (map == NULL) map = (ShadowMap*)allocateShadowMap(sizeof(ShadowMap));
    if (map != NULL) {
        ParameterObject::create(&map->mParameters);
        map->mParameters.mVtable = (parameter_tree::ValueVtable*)&sShadowMapMethods;
        map->mInterfaceMethods = &sShadowMapInterfaceMethods;
        u32 integerMethods = gabi::ea(&sShadowIntegerMethods);
        u32 booleanMethods = gabi::ea(&sShadowBooleanMethods);
        u32 floatMethods = gabi::ea(&sShadowFloatMethods);
        parameter_tree::Value* owner = &map->mParameters;
        auto* mapType = (shadow_parameters::IntegerParameter*)effect_construction::constructParameter(
            &map->mMapType,sizeof(shadow_parameters::IntegerParameter),
            integerMethods,&sShadowTextMethods,"shadow_map_type","Shadow Map Type",&sShadowIntegerMeta,owner,{8,0x10,0xC0});
        if (mapType != NULL) mapType->mValue = 0;
        auto* width = (shadow_parameters::IntegerParameter*)effect_construction::constructParameter(
            &map->mWidth,sizeof(shadow_parameters::IntegerParameter),
            integerMethods,&sShadowTextMethods,"size_w","幅",&sShadowSizeMeta,owner,{0x18,0x20,0x28},true);
        if (width != NULL) width->mValue = 512;
        auto* height = (shadow_parameters::IntegerParameter*)effect_construction::constructParameter(
            &map->mHeight,sizeof(shadow_parameters::IntegerParameter),
            integerMethods,&sShadowTextMethods,"size_h","高さ",&sShadowSizeMeta,owner,{0x30,0x38,0x40},true);
        if (height != NULL) height->mValue = 512;
        auto* hiZ = (shadow_parameters::BooleanParameter*)effect_construction::constructParameter(
            &map->mEnableHiZ,sizeof(shadow_parameters::BooleanParameter),
            booleanMethods,&sShadowTextMethods,"enable_hiz","HiZ有効",&sShadowBooleanMeta,owner,{0x48,0x50,0xC8});
        if (hiZ != NULL) hiZ->mValue = 0;
        auto* forceArray = (shadow_parameters::BooleanParameter*)effect_construction::constructParameter(
            &map->mForceArray,sizeof(shadow_parameters::BooleanParameter),
            booleanMethods,&sShadowTextMethods,"force_array","１枚でもArray",&sShadowBooleanMeta,owner,{0x58,0x60,0xD0});
        if (forceArray != NULL) forceArray->mValue = 0;
        auto* use16Unorm = (shadow_parameters::BooleanParameter*)effect_construction::constructParameter(
            &map->mUse16Unorm,sizeof(shadow_parameters::BooleanParameter),
            booleanMethods,&sShadowTextMethods,"use_16_unorm","16_UNorm",&sShadowBooleanMeta,owner,{0x68,0x70,0xD8});
        if (use16Unorm != NULL) use16Unorm->mValue = 0;
        auto* withoutContext = (shadow_parameters::BooleanParameter*)effect_construction::constructParameter(
            &map->mAllocWithoutContext,sizeof(shadow_parameters::BooleanParameter),
            booleanMethods,&sShadowTextMethods,"alloc_without_ctx","WithoutContext",&sShadowBooleanMeta,owner,{0x78,0x80,0xE0});
        if (withoutContext != NULL) withoutContext->mValue = 0;
        auto* fromMem1 = (shadow_parameters::BooleanParameter*)effect_construction::constructParameter(
            &map->mAllocFromMem1,sizeof(shadow_parameters::BooleanParameter),
            booleanMethods,&sShadowTextMethods,"alloc_from_mem1","Mem1",&sShadowBooleanMeta,owner,{0x88,0x90,0xE8});
        if (fromMem1 != NULL) fromMem1->mValue = 1;
        auto* scissor = (shadow_parameters::BooleanParameter*)effect_construction::constructParameter(
            &map->mScissor,sizeof(shadow_parameters::BooleanParameter),
            booleanMethods,&sShadowTextMethods,"scissor","Scissor",&sShadowBooleanMeta,owner,{0x98,0xA0,0xF0});
        if (scissor != NULL) scissor->mValue = 1;
        auto* margin = (shadow_parameters::FloatParameter*)effect_construction::constructParameter(
            &map->mScissorMargin,sizeof(shadow_parameters::FloatParameter),
            floatMethods,&sShadowTextMethods,"scissor_margin","Scissor Margin",&sShadowMarginMeta,owner,{0xA8,0xB0,0xB8},true);
        if (margin != NULL) margin->mValue = 1.0f;
        map->mCurrentTexture = NULL;
        map->mDepthTexture = NULL;
        TextureSampler::create(&map->mDepthSampler);
        map->mFilterTexture = NULL;
        TextureSampler::create(&map->mFilterSampler);
        map->mColor.construct();
        map->mDepth.construct();
        map->mFrame.construct();
        map->mFrameColor.construct();
        TextureSampler::create(&map->mFilterInput);
        ResourceRecord::create(&map->mResource);
        map->mNeedsRebuild = 0;
        map->mUseSharedCache = 0;
        map->mLayerCount = 0;
    }
    return map;
}

// Binds a depth surface to a cache texture (a d_snap-TU helper; the depth surface takes the texture's surface).
void bindDepthTexture(DepthSurface* depth, const TextureData* texture, u32 slice, u32 mip);
void GX2InitDepthBufferHiZEnable(PlatformDepthBuffer_l* depth, u32 enable);
/* 027967C4 */
void agl::ShadowMap::allocateDepthTexture(u32 layerCount) {
    gabi::AutoFrame frame{0x027967C4};
    u32 use16 = mUse16Unorm.mValue;
    mLayerCount = layerCount;
    gabi::FrameLocal<be<u32>> hiZBuffer(0x14);
    *hiZBuffer = 0;
    u32 format = use16 != 0 ? ShadowDepthFormat_D16 : ShadowDepthFormat_D24S8;
    u32 rangePool = (u32)mAllocFromMem1.mValue ^ 1;
    TextureCacheEntry* texture;
    if (layerCount != 1 || (u32)mForceArray.mValue != 0) {
        if ((u32)mAllocWithoutContext.mValue != 0) {
            u32 enableHiZ = mEnableHiZ.mValue;
            gabi::FrameLocal<sead::SafeText> name(0x18);
            name->mVtable = &sShadowTextMethods;
            name->mText.v = gabi::guest_str("shadow_map_depth");
            u32 width = mWidth.mValue;
            DynamicTextureCache* cache = sTextureCache;
            u32 height = mHeight.mValue;
            texture = cache->acquireShared2DArray(name.get(), format, width, height, layerCount, 1,
                                                  enableHiZ != 0 ? gabi::ea(hiZBuffer.get()) : 0, rangePool, 1, 0);
        } else {
            gabi::FrameLocal<sead::SafeText> name(0x20);
            name->mVtable = &sShadowTextMethods;
            u32 enableHiZ = mEnableHiZ.mValue;
            name->mText.v = gabi::guest_str("shadow_map_depth");
            u32 width = mWidth.mValue;
            DynamicTextureCache* cache = sTextureCache;
            u32 height = mHeight.mValue;
            texture = cache->acquire2DArray(name.get(), format, width, height, layerCount, 1,
                                            enableHiZ != 0 ? gabi::ea(hiZBuffer.get()) : 0, rangePool, 1, 0);
        }
    } else if ((u32)mAllocWithoutContext.mValue != 0) {
        gabi::FrameLocal<sead::SafeText> name(0x28);
        name->mVtable = &sShadowTextMethods;
        u32 enableHiZ = mEnableHiZ.mValue;
        name->mText.v = gabi::guest_str("shadow_map_depth");
        u32 width = mWidth.mValue;
        u32 height = mHeight.mValue;
        DynamicTextureCache* cache = sTextureCache;
        texture = cache->acquireShared2D(name.get(), format, width, height, 1,
                                         enableHiZ != 0 ? gabi::ea(hiZBuffer.get()) : 0, rangePool, 1, 0);
    } else {
        u32 enableHiZ = mEnableHiZ.mValue;
        gabi::FrameLocal<sead::SafeText> name(0x30);
        name->mText.v = gabi::guest_str("shadow_map_depth");
        name->mVtable = &sShadowTextMethods;
        u32 height = mHeight.mValue;
        u32 width = mWidth.mValue;
        DynamicTextureCache* cache = sTextureCache;
        texture = cache->acquire2D(name.get(), format, width, height, 1,
                                   enableHiZ != 0 ? gabi::ea(hiZBuffer.get()) : 0, rangePool, 1, 0);
    }
    u32 shared = mAllocWithoutContext.mValue;
    mDepthTexture = texture;
    mUseSharedCache = shared;
    bindDepthTexture(&mDepth, texture, 0, 0);
    u32 hiZ = (u32)mEnableHiZ.mValue != 0 ? (u32)*hiZBuffer : 0;
    mDepth.mHiZBuffer.v = hiZ;
    mDepth.mDepth.mHiZBuffer.v = hiZ;
    GX2InitDepthBufferHiZEnable(&mDepth.mDepth, hiZ != 0 ? 1 : 0);
    texture = mDepthTexture;
    if (!sameTextureLayout(mDepthSampler.texture, *texture)) {
        mDepthSampler.copyTexture(texture);
        return;
    }
    u32 image = texture->surface.mImage.v;
    u32 mipImage = texture->surface.mMipImage.v;
    mDepthSampler.texture.surface.mImage.v = image;
    mDepthSampler.texture.surface.mMipImage.v = mipImage;
    nativeSurface(&mDepthSampler)->mImage.v = image;
    nativeSurface(&mDepthSampler)->mMipImage.v = mipImage;
}
/* WWHD glue
str 10166C2C
str 10166C3C
str 10166C08
str 10166C10
str 10166C14
str 10166C1C
str 10166CA8
str 10166BF0
str 10166C6C
str 10166C84
str 10166CB4
str 10166CC4
str 10166C94
str 10166CD0
str 10166C5C
str 10166C24
str 10166BF8
str 10166C00
str 10166CE0
str 10166CF0
str 10166D0C
data sTextureCache 101F8DD4
data sShaderContext 101F8BD8
data sShadowMapMethods 10166D3C
data sShadowMapInterfaceMethods 10166D84
data sResourceRecordMethods 10166A64
data sResourceRecordInterfaceMethods 10166A74
data sShadowTextMethods 10166A2C
data sShadowIntegerMeta 10166A28
data sShadowBooleanMeta 10166A29
data sShadowSizeMeta 10166C4C
data sShadowMarginMeta 10166C78
data sShadowIntegerMethods 10166AFC
data sShadowBooleanMethods 10166A84
data sShadowFloatMethods 10166B74
ext 0273AD10 void* allocateShadowMap(u32)
ext 0273AF40 void freeShadowMap(void*)
ext C00061A0 void GX2ExpandDepthBuffer(agl::PlatformDepthBuffer_l*)
ext C00063E8 void GX2InitDepthBufferHiZEnable(agl::PlatformDepthBuffer_l*,u32)
ext 025C09D8 void bindDepthTexture(agl::DepthSurface*,const agl::TextureData*,u32,u32)
*/

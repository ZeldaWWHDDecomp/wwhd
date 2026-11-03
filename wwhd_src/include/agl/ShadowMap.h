/* agl shadow map: the depth texture a cascade shadow renders its layers into ("shadow_map_depth"), with its
 * parameters, the frame buffer used to clear and draw a layer, and the separable filter that turns the depth
 * layers into a filtered texture. Layout from its constructor (0xBE4 bytes); the members are the merged agl
 * texture, surface and render-buffer types (the constructor call is noted per member). AGL#9's CascadeShadow
 * holds one at +0x2C4. */
#pragma once
#include "wwhd.h"
#include "sead/Viewport.h"
#include "agl/ParameterObject.h"
#include "agl/TextureSampler.h"
#include "agl/RenderSurface.h"
#include "agl/RenderBuffer.h"
#include "agl/TextureResource.h"
#include "agl/DynamicTextureCache.h"
#include "agl/ScreenEffectParameters.h"
namespace agl {
// Library texture formats of the shadow textures.
enum : u32 {
    ShadowDepthFormat_D16 = 0x39,
    ShadowDepthFormat_D24S8 = 0x3A,
    ShadowFilterFormat = 0x26,
};
struct ShadowMapExtent { be<u32> mWidth, mHeight; };
// Pointer words of library singletons and text bytes, as the sources bind them with `data` lines.
namespace detail { struct ShaderHolder; }
struct ShaderContext;
struct PrimitiveShapes;
struct PrimitiveShapeInputs;
struct PrimitiveTexture;
using TextureCacheWord = gptr<DynamicTextureCache>;
using ShaderContextWord = gptr<ShaderContext>;
using ShaderHolderWord = gptr<detail::ShaderHolder>;
using PrimitiveShapesWord = gptr<PrimitiveShapes>;
using PrimitiveShapeInputsWord = gptr<PrimitiveShapeInputs>;
using PrimitiveTextureWord = gptr<PrimitiveTexture>;
using TextByte = be<u8>;
struct ShadowMap {
    /* 0x000 */ ParameterObject mParameters;          // 027A84B8 on the map
    /* 0x05C */ gptr<const void> mInterfaceMethods;   // the map's second vtable
    /* 0x060 */ shadow_parameters::IntegerParameter mMapType;
    /* 0x070 */ shadow_parameters::IntegerParameter mWidth;
    /* 0x080 */ shadow_parameters::IntegerParameter mHeight;
    /* 0x090 */ shadow_parameters::BooleanParameter mEnableHiZ;
    /* 0x0A0 */ shadow_parameters::BooleanParameter mForceArray;
    /* 0x0B0 */ shadow_parameters::BooleanParameter mUse16Unorm;
    /* 0x0C0 */ shadow_parameters::BooleanParameter mAllocWithoutContext;
    /* 0x0D0 */ shadow_parameters::BooleanParameter mAllocFromMem1;
    /* 0x0E0 */ shadow_parameters::BooleanParameter mScissor;
    /* 0x0F0 */ shadow_parameters::FloatParameter mScissorMargin;
    /* 0x100 */ gptr<TextureSampler> mCurrentTexture;  // the result: the depth sampler, or the filter sampler
    /* 0x104 */ gptr<TextureCacheEntry> mDepthTexture; // from the dynamic texture cache
    /* 0x108 */ TextureSampler mDepthSampler;          // 027BDF7C
    /* 0x2A0 */ gptr<TextureCacheEntry> mFilterTexture;
    /* 0x2A4 */ TextureSampler mFilterSampler;         // 027BDF7C
    /* 0x43C */ ColorSurface mColor;                   // 027B6CB8
    /* 0x584 */ DepthSurface mDepth;                   // 027B6E64
    /* 0x6D8 */ RenderBuffer mFrame;                   // 027B95B4
    /* 0x718 */ ColorSurface mFrameColor;              // 027B6CB8
    /* 0x860 */ TextureSampler mFilterInput;           // 027BDF7C
    /* 0x9F8 */ ResourceRecord mResource;              // 027A03A0
    /* 0xBDC */ be<u8> mUseSharedCache;                // textures come from the shared pool (alloc_without_ctx)
    /* 0xBDD */ be<u8> mNeedsRebuild;
    u8 mPadding[2];
    /* 0xBE0 */ be<s32> mLayerCount;
    static ShadowMap* create(ShadowMap* map);
    static void destroy(ShadowMap* map, u32 flags);
    void configure(u32 unused, void* context);
    void prepareLayer(u32 layer, const sead::RenderVector2* origin, const sead::RenderVector2* extent);
    void finishLayer();
    void allocateDepthTexture(u32 layerCount);
    u32 filterDepthTexture(u32 previousShaderMode);
    void setExtent(const ShadowMapExtent* extent);
    void releaseDepthTexture();
    void releaseTextures();
};
WWHD_OFFSET(ShadowMap, mInterfaceMethods, 0x5C);
WWHD_OFFSET(ShadowMap, mMapType, 0x60);
WWHD_OFFSET(ShadowMap, mWidth, 0x70);
WWHD_OFFSET(ShadowMap, mHeight, 0x80);
WWHD_OFFSET(ShadowMap, mEnableHiZ, 0x90);
WWHD_OFFSET(ShadowMap, mForceArray, 0xA0);
WWHD_OFFSET(ShadowMap, mUse16Unorm, 0xB0);
WWHD_OFFSET(ShadowMap, mAllocWithoutContext, 0xC0);
WWHD_OFFSET(ShadowMap, mAllocFromMem1, 0xD0);
WWHD_OFFSET(ShadowMap, mScissor, 0xE0);
WWHD_OFFSET(ShadowMap, mScissorMargin, 0xF0);
WWHD_OFFSET(ShadowMap, mCurrentTexture, 0x100);
WWHD_OFFSET(ShadowMap, mDepthTexture, 0x104);
WWHD_OFFSET(ShadowMap, mDepthSampler, 0x108);
WWHD_OFFSET(ShadowMap, mFilterTexture, 0x2A0);
WWHD_OFFSET(ShadowMap, mFilterSampler, 0x2A4);
WWHD_OFFSET(ShadowMap, mColor, 0x43C);
WWHD_OFFSET(ShadowMap, mDepth, 0x584);
WWHD_OFFSET(ShadowMap, mFrame, 0x6D8);
WWHD_OFFSET(ShadowMap, mFrameColor, 0x718);
WWHD_OFFSET(ShadowMap, mFilterInput, 0x860);
WWHD_OFFSET(ShadowMap, mResource, 0x9F8);
WWHD_OFFSET(ShadowMap, mUseSharedCache, 0xBDC);
WWHD_OFFSET(ShadowMap, mNeedsRebuild, 0xBDD);
WWHD_OFFSET(ShadowMap, mLayerCount, 0xBE0);
WWHD_SIZE(ShadowMap, 0xBE4);
WWHD_SIZE(ShadowMapExtent, 8);

// The description words of a cached texture that decide whether a sampler copies it again: the surface's
// dimension, width, height, depth, mip count and format, then swizzle, tile mode and sample count, in this order.
inline bool sameTextureLayout(const TextureData& cached, const TextureData& incoming) {
    const sead::GraphicsSurface& a = cached.surface;
    const sead::GraphicsSurface& b = incoming.surface;
    return (u32)a.mDimension == (u32)b.mDimension && (u32)a.mWidth == (u32)b.mWidth
        && (u32)a.mHeight == (u32)b.mHeight && (u32)a.mDepth == (u32)b.mDepth
        && (u32)a.mMipCount == (u32)b.mMipCount && (u32)a.mFormat == (u32)b.mFormat
        && (u32)a.mSwizzle == (u32)b.mSwizzle && (u32)a.mTileMode == (u32)b.mTileMode
        && (u32)a.mSamples == (u32)b.mSamples;
}
// TextureSampler::nativeTexture begins with the sampler's own GX2 surface.
inline sead::GraphicsSurface* nativeSurface(TextureSampler* sampler) {
    return reinterpret_cast<sead::GraphicsSurface*>(sampler->nativeTexture);
}
// A texture whose layout is unchanged only takes the new image addresses (sampler copy and native surface).
inline void refreshTextureImages(TextureSampler* sampler, const TextureData* texture) {
    u32 image = texture->surface.mImage.v;
    u32 mipImage = texture->surface.mMipImage.v;
    sampler->texture.surface.mImage.v = image;
    sampler->texture.surface.mMipImage.v = mipImage;
    nativeSurface(sampler)->mImage.v = image;
    nativeSurface(sampler)->mMipImage.v = mipImage;
}
}

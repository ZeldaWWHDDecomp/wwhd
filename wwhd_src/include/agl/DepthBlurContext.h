// Depth-blur draw context and its target set, derived from game loads, stores and stack sizes.
// Samplers and textures are AGL#15's agl::TextureSampler / TextureData, the mip pass output is an
// agl::RenderBuffer with an agl::ColorSurface attachment, and cache textures are agl::TextureCacheEntry.
#pragma once
#include "agl/DepthBlurValues.h"
#include "agl/TextureSampler.h"
#include "agl/RenderBuffer.h"
#include "agl/DynamicTextureCache.h"
#include "sead/Viewport.h"

struct ContextTargets {
    agl::TextureSampler mPrimary, mSecondary;
    agl::RenderBuffer mMipOutput;
    agl::ColorSurface mMipColor;
    gptr<agl::TextureCacheEntry> mBlurSource;
    agl::TextureSampler mBlurTarget;
    gptr<agl::TextureCacheEntry> mDepthSource;
    agl::TextureSampler mDepthTarget, mCombined;
};
// The draw argument of one depth-of-field pass (built by 0277F450 per view).
struct DepthBlurContext {
    enum Pass { cPass_ColorMipMap, cPass_DepthMipMap, cPass_Compose, cPass_Vignetting, cPass_Reduce };
    be<u32> mView;
    be<u32> mDrawMode;
    gptr<ContextTargets> mTargets;
    gptr<agl::depth_blur_value::Parameters> mParameters;
    gptr<agl::RenderBuffer> mOutput;
    be<f32> mNearDepth, mFarDepth;
    be<s32> mWidth, mHeight;
    be<u8> mFlags;
    u8 mPadding[3];
};
// Pointer words of the library singletons these passes read (bound per source by `data` lines).
using TextureCachePointer = gptr<agl::DynamicTextureCache>;
WWHD_SIZE(ContextTargets, 0x988);
WWHD_OFFSET(ContextTargets, mMipOutput, 0x330);
WWHD_OFFSET(ContextTargets, mMipColor, 0x370);
WWHD_OFFSET(ContextTargets, mBlurSource, 0x4B8);
WWHD_OFFSET(ContextTargets, mBlurTarget, 0x4BC);
WWHD_OFFSET(ContextTargets, mDepthTarget, 0x658);
WWHD_OFFSET(ContextTargets, mCombined, 0x7F0);
WWHD_SIZE(DepthBlurContext, 0x28);
WWHD_OFFSET(DepthBlurContext, mTargets, 8);
WWHD_OFFSET(DepthBlurContext, mOutput, 0x10);
WWHD_OFFSET(DepthBlurContext, mFlags, 0x24);

// Sampler settings slots these passes set (TextureSampler::updateFilters / updateLod forward them),
// and the TextureSampler::dirty bits set together with them.
enum : u32 { SamplerMode_MagnificationFilter = 0, SamplerMode_MinificationFilter = 1, SamplerMode_MipFilter = 2 };
enum : u32 { SamplerScalar_MinimumLod = 4, SamplerScalar_MaximumLod = 5, SamplerScalar_LodBias = 6 };
enum : u8 { SamplerDirty_Filters = 4, SamplerDirty_Lod = 8 };

// TextureSampler::nativeTexture starts with the sampler's own GX2 surface (as agl/MultiFilter.cpp reads it).
inline sead::GraphicsSurface* nativeSurface(agl::TextureSampler* sampler) {
    return reinterpret_cast<sead::GraphicsSurface*>(sampler->nativeTexture);
}
// A colour surface's SurfaceData_l prefix is AGL#15's TextureData layout (agl/RenderSurface.h: shared fold pending).
inline agl::TextureData* surfaceTexture(agl::ColorSurface* surface) {
    return reinterpret_cast<agl::TextureData*>(&surface->mBase);
}
// A render buffer's logical size and area are the six floats sead::Viewport reads as a RenderSurface.
inline sead::RenderSurface* frameSurface(agl::RenderBuffer* buffer) {
    return reinterpret_cast<sead::RenderSurface*>(&buffer->mBase);
}

inline bool contextSurfaceMatches(const agl::TextureSampler* first, const agl::TextureData* second) {
    const sead::GraphicsSurface& a = first->texture.surface;
    const sead::GraphicsSurface& b = second->surface;
    return a.mDimension == b.mDimension && a.mWidth == b.mWidth && a.mHeight == b.mHeight && a.mDepth == b.mDepth
        && a.mMipCount == b.mMipCount && a.mFormat == b.mFormat && a.mSwizzle == b.mSwizzle
        && a.mTileMode == b.mTileMode && a.mSamples == b.mSamples;
}
inline void refreshContextSurfaceImages(agl::TextureSampler* destination, const agl::TextureData* source) {
    void* image = source->surface.mImage;
    void* mipmaps = source->surface.mMipImage;
    destination->texture.surface.mImage = image;
    nativeSurface(destination)->mMipImage = mipmaps;
    nativeSurface(destination)->mImage = image;
    destination->texture.surface.mMipImage = mipmaps;
}

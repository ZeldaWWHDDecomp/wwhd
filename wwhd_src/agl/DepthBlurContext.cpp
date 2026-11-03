// Draw-context construction and surface synchronization, from this game's field accesses.
#include "bindings.h"
#include "agl/DepthBlurValues.h"

#include "agl/DepthBlurContext.h"

static void* allocateDepthBlurContext(u32 size);
static void assignContextSurface(agl::TextureSampler* destination, const agl::TextureData* source);
static void constructMipViewport(sead::Viewport* viewport, const sead::RenderSurface* surface);
static void applyMipViewport(sead::Viewport* viewport, const sead::RenderSurface* surface);
static void applyMipSurface(sead::RenderSurface* surface);
/* 0277F450 */
static DepthBlurContext* constructDepthBlurContext(DepthBlurContext* context, u32 view,
    ContextTargets* targets, agl::depth_blur_value::Parameters* parameters, agl::RenderBuffer* output,
    const agl::TextureData* secondary, u32 flags, f32 nearDepth, f32 farDepth) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!context) context = static_cast<DepthBlurContext*>(allocateDepthBlurContext(sizeof(DepthBlurContext)));
    if (!context) return nullptr;
    context->mParameters = parameters;
    context->mNearDepth = nearDepth;
    context->mFarDepth = farDepth;
    context->mTargets = targets;
    context->mOutput = output;
    context->mView = view;
    const agl::TextureData* surface = surfaceTexture(output->mColors[0]);
    s32 width = surface->minimumWidth;
    const s32 requestedWidth = surface->surface.mWidth;
    if (requestedWidth > width) width = requestedWidth;
    context->mWidth = width;
    surface = surfaceTexture(output->mColors[0]);
    const s32 minimumHeight = surface->minimumHeight;
    s32 height = surface->surface.mHeight;
    context->mFlags = u8(flags);
    if (minimumHeight > height) height = minimumHeight;
    context->mHeight = height;
    surface = surfaceTexture(output->mColors[0]);
    if (contextSurfaceMatches(&targets->mPrimary, surface))
        refreshContextSurfaceImages(&targets->mPrimary, surface);
    else
        assignContextSurface(&targets->mPrimary, surface);
    ContextTargets* currentTargets = context->mTargets;
    if (contextSurfaceMatches(&currentTargets->mSecondary, secondary))
        refreshContextSurfaceImages(&currentTargets->mSecondary, secondary);
    else
        assignContextSurface(&currentTargets->mSecondary, secondary);
    return context;
}

static u32 shiftSurfaceExtent(u32 value, u32 shift) {
    const u32 count = shift & 63;
    return count < 32 ? value >> count : 0;
}

/* 0277FBDC */
static void applyDepthBlurOutputMip(void*, agl::RenderBuffer* output, u32 mip, u32 secondaryOffset) {
    gabi::AutoFrame frame{gabi::current_function()};
    s32 width = -1, height = -1;
    if (output->mColors[0]) {
        agl::SurfaceView_l* surface = &output->mColors[0]->mBase;
        if (surface->mViewMip != mip) {
            surface->mViewMip = mip;
            surface->mDirty = 1;
        }
        surface = &output->mColors[0]->mBase;
        width = s32(shiftSurfaceExtent(surface->mSurface.mWidth, mip));
        const s32 minimumWidth = surface->mMinimumWidth;
        if (minimumWidth > width) width = minimumWidth;
        height = s32(shiftSurfaceExtent(surface->mSurface.mHeight, mip));
        const s32 minimumHeight = surface->mMinimumHeight;
        if (minimumHeight > height) height = minimumHeight;
    }
    if (output->mDepth) {
        agl::SurfaceView_l* surface = &output->mDepth->mBase;
        const u32 secondaryMip = mip + secondaryOffset;
        if (surface->mViewMip != secondaryMip) {
            surface->mViewMip = secondaryMip;
            surface->mDirty = 1;
            surface = &output->mDepth->mBase;
        }
        height = s32(shiftSurfaceExtent(surface->mSurface.mHeight, secondaryMip));
        width = s32(shiftSurfaceExtent(surface->mSurface.mWidth, secondaryMip));
        const s32 minimumWidth = surface->mMinimumWidth;
        const s32 minimumHeight = surface->mMinimumHeight;
        if (minimumWidth > width) width = minimumWidth;
        if (minimumHeight > height) height = minimumHeight;
    }
    struct ConversionBias { be<u32> exponent, mantissa; };
    gabi::FrameLocal<ConversionBias> conversion(0x18);
    conversion->mantissa = u32(width) ^ 0x80000000;
    conversion->exponent = 0x43300000;
    conversion->mantissa = u32(height) ^ 0x80000000;
    const f32 widthFloat = f32(width), heightFloat = f32(height);
    gabi::FrameLocal<agl::depth_blur_value::Vector4> bounds(8);
    if (0.0f < widthFloat) { bounds->z = widthFloat; bounds->x = 0.0f; }
    else { bounds->z = 0.0f; bounds->x = widthFloat; }
    sead::FrameBufferArea& area = output->mBase.mArea;
    if (0.0f < heightFloat) {
        area.mMinY = 0.0f;
        area.mMinX = bounds->x;
        area.mMaxX = bounds->z;
        bounds->w = heightFloat;
        area.mMaxY = heightFloat;
        bounds->y = 0.0f;
    } else {
        const f32 left = bounds->x;
        bounds->w = 0.0f;
        area.mMinX = left;
        const f32 right = bounds->z;
        area.mMinY = heightFloat;
        area.mMaxX = right;
        bounds->y = heightFloat;
        area.mMaxY = 0.0f;
    }
    output->mBase.mSize.mWidth = widthFloat;
    gabi::FrameLocal<sead::Viewport> viewport(0x20);
    output->mBase.mSize.mHeight = heightFloat;
    constructMipViewport(viewport, frameSurface(output));
    applyMipViewport(viewport, frameSurface(output));
    applyMipSurface(frameSurface(output));
}

WWHD_SIZE(DepthBlurContext, 0x28);
/* WWHD glue
 ext 0273AD10 void* allocateDepthBlurContext(u32 size)
 ext 027BDEB4 void assignContextSurface(agl::TextureSampler* destination, const agl::TextureData* source)
 ext 0274F5F0 void constructMipViewport(sead::Viewport* viewport, const sead::RenderSurface* surface)
 ext 0274F964 void applyMipViewport(sead::Viewport* viewport, const sead::RenderSurface* surface)
 ext 0274D690 void applyMipSurface(sead::RenderSurface* surface)
*/

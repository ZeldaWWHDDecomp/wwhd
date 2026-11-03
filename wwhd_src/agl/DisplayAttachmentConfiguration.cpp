// Render view attachments (RenderQueueManager::update 027C106C, after a resize or format change): the colour/depth (and,
// multisampled, the resolve) texture descriptions; then either own surfaces from the shared pool of the dynamic texture
// cache (flag 4) or the other view's surfaces (borrowed); finally the render target gets the colour and depth surfaces.
#include "agl/DisplayFrameState.h"
#include "agl/DynamicTextureCache.h"
using namespace agl::render;
static DisplayFloatConstant sUnsignedThreshold;
static sead::SafeTextVtable sAttachmentNameMethods;
void setSurfaceSwizzle(sead::GraphicsSurface*,s32);
void setDepthBufferHiZ(agl::PlatformDepthBuffer_l*,u32);
void attachDepthSurfaceTexture(agl::DepthSurface*,const agl::TextureData*);
// fctiwz of a float size, with the original's split above 2^31.
static u32 surfaceDimension(f32 value,f32 threshold,be<u32>* conversion) {
    if (value < threshold) {
        *conversion = (u32)gabi::ftoi(value);
        return *conversion;
    }
    *conversion = (u32)gabi::ftoi(gabi::fsubs_ppc(value,threshold));
    return (u32)*conversion + 0x80000000u;
}
/* 027D6658 */
void agl::render::DisplayFrameState::configureAttachments(u32 samples,DisplayFrameState* borrowed,u32 colorFormat,s32 colorSwizzle,u32 depthFormat,s32 depthSwizzle,u32 enableHiZ,u8 tileMode) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> conversion(0x10);
    f32 threshold = sUnsignedThreshold;
    u32 width = surfaceDimension(mSize.x,threshold,conversion.get());
    u32 height = surfaceDimension(mSize.y,threshold,conversion.get());
    if (samples == 0) {
        mTextureA.configure(GX2SurfaceDim_2D,colorFormat,width,height,1,1,0,0);
        mTextureB.configure(GX2SurfaceDim_2D,depthFormat,width,height,1,1,0,0);
    } else {
        mTextureA.configure(GX2SurfaceDim_2DMSAA,colorFormat,width,height,1,1,0,samples);
        mTextureB.configure(GX2SurfaceDim_2DMSAA,depthFormat,width,height,1,1,0,samples);
        mTextureC.configure(GX2SurfaceDim_2D,colorFormat,width,height,1,samples,0,0);
    }
    if (!borrowed) {
        agl::DynamicTextureCache* cache = sDynamicTextureCache;
        if (colorSwizzle != -1) {
            gabi::FrameLocal<gptr<void>> auxiliary(0x18);
            gabi::FrameLocal<sead::SafeText> name(0x20);
            *auxiliary.get() = NULL;
            name->mVtable = &sAttachmentNameMethods;
            name->mText.v = gabi::guest_str("agl::Renderer(color)");
            agl::TextureCacheEntry* color = cache->acquireSharedMultisample(name.get(),colorFormat,width,height,mTextureA.surface.mSamples,gabi::ea(auxiliary.get()),0,0,tileMode);
            mAttachment = color;
            mSurfaceA.mAuxBuffer = *auxiliary.get();
            mSurfaceA.mColor.mAuxBuffer = *auxiliary.get();
            setSurfaceSwizzle(&color->surface,colorSwizzle);
        }
        if (depthSwizzle != -1) {
            gabi::FrameLocal<gptr<void>> auxiliary(0x1C);
            gabi::FrameLocal<sead::SafeText> name(0x28);
            *auxiliary.get() = NULL;
            name->mVtable = &sAttachmentNameMethods;
            name->mText.v = gabi::guest_str("agl::Renderer(depth)");
            u32 hiZ = enableHiZ ? gabi::ea(auxiliary.get()) : 0;
            mSecondaryAttachment = cache->acquireSharedMultisample(name.get(),depthFormat,width,height,mTextureB.surface.mSamples,hiZ,0,0,tileMode);
            mSurfaceAuxiliary.mDepth.mHiZBuffer = *auxiliary.get();
            mSurfaceAuxiliary.mHiZBuffer = *auxiliary.get();
            setDepthBufferHiZ(&mSurfaceAuxiliary.mDepth,gabi::ea(*auxiliary.get()) != 0);
            setSurfaceSwizzle(&mSecondaryAttachment->surface,depthSwizzle);
        }
        mFlags = (u32)mFlags | 4u;
    } else {
        if (colorSwizzle != -1) {
            mAttachment = &mTextureA;
            mTextureA.surface.mImage = borrowed->mAttachment->surface.mImage;
            void* auxiliary = borrowed->mSurfaceA.mAuxBuffer;
            mSurfaceA.mAuxBuffer = auxiliary;
            mSurfaceA.mColor.mAuxBuffer = auxiliary;
            if (borrowed->mExtraAttachment) {
                mExtraAttachment = &mTextureC;
                mTextureC.surface.mImage = borrowed->mExtraAttachment->surface.mImage;
            } else mExtraAttachment = NULL;
        }
        if (depthSwizzle != -1) {
            mSecondaryAttachment = &mTextureB;
            mTextureB.surface.mImage = borrowed->mSecondaryAttachment->surface.mImage;
            void* auxiliary = borrowed->mSurfaceAuxiliary.mHiZBuffer;
            mSurfaceAuxiliary.mDepth.mHiZBuffer = auxiliary;
            mSurfaceAuxiliary.mHiZBuffer = auxiliary;
            setDepthBufferHiZ(&mSurfaceAuxiliary.mDepth,gabi::ea(auxiliary) != 0);
        }
        mFlags = (u32)mFlags & ~4u;
    }
    agl::RenderBuffer* target = mRenderTarget;
    target->clearColors();
    target->mDepth = NULL;
    agl::TextureData* attachment = mAttachment;
    target = mRenderTarget;
    if (attachment) {
        target->mColors[0] = &mSurfaceA;
        attachSurfaceTexture(&mSurfaceA,mAttachment);
        target = mRenderTarget;
    } else target->mColors[0] = NULL;
    if (mSecondaryAttachment) {
        target->mDepth = &mSurfaceAuxiliary;
        attachDepthSurfaceTexture(&mSurfaceAuxiliary,mSecondaryAttachment);
    } else target->mDepth = NULL;
    if (mExtraAttachment) attachSurfaceTexture(&mSurfaceC,mExtraAttachment);
}
/* WWHD glue
data sUnsignedThreshold 1016C2F8
data sAttachmentNameMethods 1016C180
str 1016C2FC "agl::Renderer(color)"
str 1016C314 "agl::Renderer(depth)"
ext 027BE930 void agl::TextureData::configure(u32,u32,u32,u32,u32,s32,u32,u32)
ext 0279FF24 agl::TextureCacheEntry* agl::DynamicTextureCache::acquireSharedMultisample(sead::SafeText*,u32,u32,u32,u32,u32,u32,u8,u8)
ext 027B954C void agl::RenderBuffer::clearColors()
ext 025C0AB8 void attachDepthSurfaceTexture(agl::DepthSurface*,const agl::TextureData*)
ext C00069B8 void setSurfaceSwizzle(sead::GraphicsSurface*,s32)
ext C00063E8 void setDepthBufferHiZ(agl::PlatformDepthBuffer_l*,u32)
*/

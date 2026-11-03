// Before the view's first pass: the begin listener, then the render target is bound with the scaled viewport and cleared
// (flag 0x80; clear mask 0 under flag 0x20, depth bit dropped without a depth surface), and a resized view refreshes the
// target's colour and depth textures. Afterwards the embedded buffer takes the target's frame and the colour attachment.
#include "agl/DisplayFrameState.h"
#include "agl/RenderDispatchContext.h"
using namespace agl::render;
// The render target's frame-buffer table: clear slot +0x28 (function word +0x2C).
struct RenderTargetMethods_l { u8 mPrefix[0x28]; sead::VtableSlot mClear; };
WWHD_OFFSET(RenderTargetMethods_l,mClear,0x28);
/* 027D6A74 */
void agl::render::DisplayFrameState::prepareAttachedSurface() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mBeginHook) {
        gabi::FrameLocal<RenderDispatchContext> context(8);
        u32 target = gabi::ea(mRenderTarget);
        u32 category = mCategory;
        createDirectDispatchContext(context.get(),category,target);
        category = mCategory;
        RenderFrameHook_l* hook = mBeginHook;
        context->mKind = category;
        hook->mListener.invoke(context.get());
    }
    agl::RenderBuffer* target = mRenderTarget;
    if (target) {
        u32 flags = mFlags;
        u32 mask = mClearMask;
        agl::DepthSurface* depth = target->mDepth;
        if (flags & 0x20u) mask = 0;
        if (!depth) mask &= ~2u;
        if (flags & 0x80u) {
            bindRenderBuffer(target);
            mScaledViewport.apply(targetSurface());
            target = mRenderTarget;
            auto* methods = static_cast<RenderTargetMethods_l*>(static_cast<void*>(target->mBase.mVtable));
            f32 clearDepth = mClearDepth;
            u32 stencil = mClearStencil;
            gabi::call_ptr<void>(methods->mClear.mFunction,target,mask,reinterpret_cast<sead::RenderQuaternion*>(mClearColor),stencil,clearDepth);
        } else {
            mScaledViewport.apply(reinterpret_cast<sead::RenderSurface*>(&target->mBase));
        }
        if ((u8)mSurfaceChanged) {
            target = mRenderTarget;
            agl::ColorSurface* color = target->mColors[0];
            if (color) {
                reinterpret_cast<agl::TextureData*>(color)->updateSurface();
                target = mRenderTarget;
            }
            agl::DepthSurface* depthSurface = target->mDepth;
            if (depthSurface) reinterpret_cast<agl::TextureData*>(depthSurface)->updateSurface();
            mSurfaceChanged = 0;
        }
    }
}
/* 027D740C */
void agl::render::DisplayFrameState::refreshAttachmentBounds() {
    gabi::AutoFrame frame{gabi::current_function()};
    agl::RenderBuffer* target = mRenderTarget;
    if (target && mAttachment) {
        sead::VirtualFrameBuffer* buffer = &mEmbeddedBuffer.mBase;
        buffer->mArea.mMinX = target->mBase.mArea.mMinX;
        buffer->mArea.mMinY = target->mBase.mArea.mMinY;
        f32 maxX = target->mBase.mArea.mMaxX;
        agl::RenderBuffer* current = mRenderTarget;
        buffer->mArea.mMaxX = maxX;
        buffer->mArea.mMaxY = target->mBase.mArea.mMaxY;
        buffer->mSize.mWidth = current->mBase.mSize.mWidth;
        f32 height = current->mBase.mSize.mHeight;
        agl::TextureData* attachment = mAttachment;
        buffer->mSize.mHeight = height;
        attachSurfaceTexture(&mSurfaceB,attachment);
        mFlags = (u32)mFlags | 0x400u;
    }
}
/* WWHD glue
ext 0274D690 void bindRenderBuffer(agl::RenderBuffer*)
ext 0274F964 void sead::Viewport::apply(const sead::RenderSurface*) const
ext 027BE794 void agl::TextureData::updateSurface()
ext 025C0928 void attachSurfaceTexture(agl::ColorSurface*,const agl::TextureData*)
*/

// Logical size of a render view: a changed size rebuilds the frame buffer area (sign-selected corners, NaN kept as the
// original's compares give it) and marks the view resized (0x100); the scaled viewport and the render target follow.
#include "agl/DisplayFrameState.h"
using namespace agl::render;
static DisplayFloatConstant sDisplayZero;
struct SurfaceBoundsScratch { be<f32> mLeft,mBottom,mRight,mTop; };
/* 027D5EE8 */
void agl::render::DisplayFrameState::setSurfaceExtent(const sead::RenderVector2* size) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 left = mFrameBuffer.mArea.mMinX;
    f32 right = mFrameBuffer.mArea.mMaxX;
    f32 width = gabi::fsubs_ppc(right,left);
    bool unchanged = (f32)size->x == width;
    if (unchanged) {
        f32 top = mFrameBuffer.mArea.mMaxY;
        f32 bottom = mFrameBuffer.mArea.mMinY;
        f32 height = gabi::fsubs_ppc(top,bottom);
        unchanged = (f32)size->y == height;
    }
    if (!unchanged) {
        u32 flags = mFlags;
        f32 zero = sDisplayZero;
        mFlags = flags | 0x100u;
        f32 requestedWidth = size->x;
        bool positiveWidth = zero < requestedWidth;
        f32 requestedHeight = size->y;
        bool positiveHeight = zero < requestedHeight;
        gabi::FrameLocal<SurfaceBoundsScratch> bounds(8);
        bounds->mRight = positiveWidth ? requestedWidth : zero;
        bounds->mLeft = positiveWidth ? zero : requestedWidth;
        bounds->mBottom = positiveHeight ? zero : requestedHeight;
        f32 boundLeft = bounds->mLeft;
        f32 boundBottom = bounds->mBottom;
        f32 boundRight = bounds->mRight;
        f32 boundTop = positiveHeight ? requestedHeight : zero;
        if (positiveHeight) {
            mFrameBuffer.mArea.mMinX = boundLeft;
            mFrameBuffer.mArea.mMinY = boundBottom;
            mFrameBuffer.mArea.mMaxX = boundRight;
            mFrameBuffer.mArea.mMaxY = boundTop;
        } else {
            mFrameBuffer.mArea.mMaxY = boundTop;
            mFrameBuffer.mArea.mMaxX = boundRight;
            mFrameBuffer.mArea.mMinY = boundBottom;
            mFrameBuffer.mArea.mMinX = boundLeft;
        }
        mSize.x = (f32)size->x;
        f32 newHeight = size->y;
        bounds->mTop = boundTop;
        mSize.y = newHeight;
    }
    mViewport.setSize(reinterpret_cast<const sead::RenderVector2*>(&mFrameBuffer.mSize));
    f32 top = mViewport.mTop;
    sead::RenderSurface* attached = targetSurface();
    mScaledViewport.mTop = top;
    u32 orientation = mViewport.mOrientation;
    f32 bottom = mViewport.mBottom;
    mScaledViewport.mOrientation = orientation;
    mScaledViewport.mBottom = bottom;
    f32 viewportLeft = mViewport.mLeft;
    f32 viewportRight = mViewport.mRight;
    mScaledViewport.mLeft = viewportLeft;
    mScaledViewport.mRight = viewportRight;
    if (attached) {
        attached->mWidth = (f32)mFrameBuffer.mSize.mWidth;
        attached->mHeight = (f32)mFrameBuffer.mSize.mHeight;
        attached = targetSurface();
        attached->mLeft = (f32)mFrameBuffer.mArea.mMinX;
        attached->mBottom = (f32)mFrameBuffer.mArea.mMinY;
        attached->mRight = (f32)mFrameBuffer.mArea.mMaxX;
        attached->mTop = (f32)mFrameBuffer.mArea.mMaxY;
        mSurfaceChanged = 1;
    }
}
/* 027D660C */
void agl::render::DisplayFrameState::refreshSurfaceExtent() {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u32)mFlags & 0x200u) {
        setSurfaceExtent(&mSize);
        mFlags = (u32)mFlags & ~0x200u;
    }
}
/* WWHD glue
data sDisplayZero 1016C290
ext 0274F53C void sead::Viewport::setSize(const sead::RenderVector2*)
*/

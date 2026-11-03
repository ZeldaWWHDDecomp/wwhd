/* Color/depth surface setup and cache invalidation at the opaque SDK boundary. */
// References: WWHD's own instructions and strings; GX2 SDK record layouts and import arguments are public wut/Cemu facts. No other reference.
#include "agl/RenderSurface.h"
static void* allocateObject(u32 bytes);
static void initializeSurfaceData(agl::SurfaceData_l* surface);
static void* setBytes(void* storage, u32 value, u32 bytes);
static void calculateColorAux(agl::PlatformColorBuffer_l* color, be<u32>* size, be<u32>* alignment);
static void calculateDepthHiZ(agl::PlatformDepthBuffer_l* depth, be<u32>* size, be<u32>* alignment);
static void initializeColorRegisters(agl::PlatformColorBuffer_l* color);
static void initializeDepthRegisters(agl::PlatformDepthBuffer_l* depth);
static void enableHiZ(agl::PlatformDepthBuffer_l* depth, u32 enabled);
static void invalidateRange(u32 mode, void* storage, u32 bytes);

static void copySurface(agl::PlatformSurface_l* destination, const agl::PlatformSurface_l* source);
/* 027B6C60 */
void agl::ColorSurface::calculateAuxiliary() {
    mHasAux = mBase.mSurface.mDimension == 6;
    copySurface(&mColor.mSurface, &mBase.mSurface);
    mColor.mSurface.mUsage = 2;
    if (mHasAux)
        gabi::tail_ptr<void>(gabi::guest_fn((const void*)calculateColorAux), &mColor, &mAuxSize, &mAuxAlignment);
}

// Unit-local helpers are included in the canonical mutation ranges.
static void copySurface(agl::PlatformSurface_l* destination, const agl::PlatformSurface_l* source) {
    destination->mDimension = (u32)source->mDimension;
    destination->mWidth = (u32)source->mWidth;
    destination->mHeight = (u32)source->mHeight;
    destination->mDepth = (u32)source->mDepth;
    destination->mMipLevels = (u32)source->mMipLevels;
    destination->mFormat = (u32)source->mFormat;
    destination->mAntiAlias = (u32)source->mAntiAlias;
    destination->mUsage = (u32)source->mUsage;
    destination->mImageSize = (u32)source->mImageSize;
    destination->mImage = source->mImage.get();
    destination->mMipmapSize = (u32)source->mMipmapSize;
    destination->mMipmaps = source->mMipmaps.get();
    destination->mTileMode = (u32)source->mTileMode;
    destination->mSwizzle = (u32)source->mSwizzle;
    destination->mAlignment = (u32)source->mAlignment;
    destination->mPitch = (u32)source->mPitch;
    for (u32 index = 0; index < 13; ++index)
        destination->mMipOffsets[index] = (u32)source->mMipOffsets[index];
}

/* 027B6CB8 */
agl::ColorSurface* agl::ColorSurface::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    auto result = this;
    if (gabi::ea(result) == 0) {
        result = (ColorSurface*)allocateObject(sizeof(ColorSurface));
        if (!result)
            return result;
    }
    initializeSurfaceData(&result->mBase);
    result->mBase.mViewFirstSlice = 0;
    result->mBase.mViewMip = 0;
    result->mBase.mDirty = 1;
    result->mHasAux = 0;
    result->mAuxSize = 0;
    result->mAuxBuffer = nullptr;
    result->mAuxAlignment = 0x800;
    setBytes(&result->mColor, 0, 0x9C);
    result->mColor.mAuxBuffer = nullptr;
    return result;
}
/* 027B6D64 */
void agl::ColorSurface::invalidate() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* image = mBase.mSurface.mImage;
    if (image)
        invalidateRange(0x10, image, mBase.mSurface.mImageSize);
    if (mBase.mSurface.mMipmaps != nullptr) {
        u32 size = mBase.mSurface.mMipmapSize;
        void* mipmaps = mBase.mSurface.mMipmaps;
        invalidateRange(0x10, mipmaps, size);
    }
    if (mAuxBuffer != nullptr) {
        u32 size = mAuxSize;
        void* auxiliary = mAuxBuffer;
        invalidateRange(0x10, auxiliary, size);
    }
}
/* 027B6DDC */
void agl::ColorSurface::initializeRegisters() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 firstSlice = mBase.mViewFirstSlice;
    u32 mip = mBase.mViewMip;
    mColor.mViewFirstSlice = firstSlice;
    mColor.mViewMip = mip;
    mColor.mViewNumSlices = 1;
    initializeColorRegisters(&mColor);
    mBase.mDirty = 0;
}
/* 027B6E2C */
void agl::DepthSurface::calculateHiZ() {
    copySurface(&mDepth.mSurface, &mBase.mSurface);
    mDepth.mSurface.mUsage = 4;
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)calculateDepthHiZ), &mDepth, &mHiZSize, &mHiZAlignment);
}
/* 027B6E64 */
agl::DepthSurface* agl::DepthSurface::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    auto result = this;
    if (gabi::ea(result) == 0) {
        result = (DepthSurface*)allocateObject(sizeof(DepthSurface));
        if (!result)
            return result;
    }
    initializeSurfaceData(&result->mBase);
    result->mBase.mViewFirstSlice = 0;
    result->mBase.mViewMip = 0;
    result->mBase.mDirty = 1;
    result->mHiZSize = 0;
    result->mHiZAlignment = 0x800;
    result->mHiZBuffer = nullptr;
    setBytes(&result->mDepth, 0, 0xAC);
    result->mDepth.mHiZBuffer = nullptr;
    enableHiZ(&result->mDepth, 0);
    return result;
}
/* 027B6F18 */
void agl::DepthSurface::invalidate() {
    gabi::AutoFrame frame{gabi::current_function()};
    void* image = mBase.mSurface.mImage;
    if (image)
        invalidateRange(0x20, image, mBase.mSurface.mImageSize);
    if (mBase.mSurface.mMipmaps != nullptr) {
        u32 size = mBase.mSurface.mMipmapSize;
        void* mipmaps = mBase.mSurface.mMipmaps;
        invalidateRange(0x20, mipmaps, size);
    }
    if (mHiZBuffer != nullptr) {
        u32 size = mHiZSize;
        void* hiz = mHiZBuffer;
        invalidateRange(0x20, hiz, size);
    }
}
/* 027B6F90 */
void agl::DepthSurface::initializeRegisters() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 firstSlice = mBase.mViewFirstSlice;
    u32 mip = mBase.mViewMip;
    mDepth.mViewFirstSlice = firstSlice;
    mDepth.mViewMip = mip;
    mDepth.mStencilClear = 0;
    mDepth.mDepthClear = 1.0f;
    mDepth.mViewNumSlices = 1;
    initializeDepthRegisters(&mDepth);
    mBase.mDirty = 0;
}
/* WWHD glue
ext 0273AD10 void* allocateObject(u32 bytes)
ext 027BE6B8 void initializeSurfaceData(agl::SurfaceData_l* surface)
ext C0009990 void* setBytes(void* storage, u32 value, u32 bytes)
ext C00060A0 void calculateColorAux(agl::PlatformColorBuffer_l* color, be<u32>* size, be<u32>* alignment) callback
ext C00060B0 void calculateDepthHiZ(agl::PlatformDepthBuffer_l* depth, be<u32>* size, be<u32>* alignment) callback
ext C00063D0 void initializeColorRegisters(agl::PlatformColorBuffer_l* color)
ext C00063F0 void initializeDepthRegisters(agl::PlatformDepthBuffer_l* depth)
ext C00063E8 void enableHiZ(agl::PlatformDepthBuffer_l* depth, u32 enabled)
ext C00064A8 void invalidateRange(u32 mode, void* storage, u32 bytes)
*/

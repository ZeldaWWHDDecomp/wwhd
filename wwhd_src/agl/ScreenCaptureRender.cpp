// Capture transfer and file output reconstructed from the assigned renderer body.
#include "bindings.h"
#include "agl/ScreenCapture.h"
#include "sead/RenderState.h"
void prepareCaptureTarget(void*);
void finishCaptureGpuWork();
void copyCaptureSurface(agl::CaptureCachedSurface*, agl::CaptureSurfaceDescriptor*);
void attachCaptureTarget(void*, void*);
void configureCaptureViewport(sead::Viewport*, const sead::RenderSurface*);
void applyCaptureViewport(sead::Viewport*, const sead::RenderSurface*);
void activateCaptureSurface(sead::RenderSurface*);
void createCaptureRenderState(sead::CompactRenderState*);
void applyCaptureRenderState(sead::CompactRenderState*);
void drawCaptureSurface(agl::CaptureCachedSurface*, u32);
void drawCaptureSurfaceWithGamma(agl::CaptureCachedSurface*, u32, f64);
void* acquireCaptureSlot(void*, const sead::SafeText*, u32, u32, u32, u32, u32, u32, u8, u8);
void releaseCaptureSlot(void*, void*);
struct CaptureEncodedImage { gptr<void> mData; be<u32> mSize; };
void encodeCaptureImage(CaptureEncodedImage*, u32, u32, void*, void*, void*);
void encodeCaptureImageAlternate(CaptureEncodedImage*, u32, u32, void*, void*);
void initializeCaptureFileHandle(sead::Disposer*);
void finishCaptureFileHandle(sead::Disposer*, u32);
struct CaptureFileProviderPrefix { u8 mBeforeDevice[16]; gptr<sead::FileDevice> mDevice; };
typedef gptr<void> CaptureAllocatorPointer;
typedef gptr<CaptureFileProviderPrefix> CaptureFileProviderPointer;
// Borrowed AGL#10 pointerword: no initializer or duplicate backing storage.
static CaptureAllocatorPointer sCaptureAllocatorOwner;
// Borrowed provider pointerword, interpreted only through its observed device+10.
static CaptureFileProviderPointer sCaptureFileProvider;
typedef be<u8> CaptureTextByte;
typedef be<f32> CaptureFloat;
static CaptureTextByte sCaptureSlotName;
static u32 sCaptureTextTable;
static CaptureTextByte sCaptureEmptyPath;
static u32 sCaptureHandleTable;
static CaptureFloat sCaptureZero;
static CaptureFloat sCaptureGamma;
/* 0279AF1C */
u32 agl::ScreenCapture::render(CaptureSourcePrefix* source, u32 options, sead::SafeText* path, void* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* outputStorage = storage;
    if (outputStorage == NULL) outputStorage = mOwnerHeap;
    auto* sourceSurfaceSlot = &source->mSurface;
    prepareCaptureTarget(*sourceSurfaceSlot);
    finishCaptureGpuWork();
    CaptureSurfaceDescriptor* sourceSurface = *sourceSurfaceSlot;
    sead::GraphicsSurface& sourceGraphics = sourceSurface->mSurface;
    s32 width = (u8)sourceSurface->mWidthFloor;
    u32 sourceWidth = sourceGraphics.mWidth;
    u32 sourceHeight = sourceGraphics.mHeight;
    if ((s32)sourceWidth > width) width = (s32)sourceWidth;
    s32 height = (u8)sourceSurface->mHeightFloor;
    if ((s32)sourceHeight > height) height = (s32)sourceHeight;
    gabi::FrameLocal<sead::SafeText> slotName(0x3C);
    slotName->mText = &sCaptureSlotName;
    slotName->mVtable = gabi::at<sead::SafeTextVtable>(gabi::ea(&sCaptureTextTable));
    gabi::FrameLocal<be<u32>> firstStackFlag(8), secondStackFlag(12);
    *firstStackFlag = 1; *secondStackFlag = 0;
    void* slot = acquireCaptureSlot(sCaptureAllocatorOwner, slotName.get(), 0x1D,
        (u32)width, (u32)height, 1, 0, 0, (u8)1, (u8)0);
    sead::GraphicsSurface& cached = mSurface.mDescriptor.mSurface;
    bool compatible = (u32)cached.mDimension == (u32)sourceGraphics.mDimension;
    if (compatible) compatible = (u32)cached.mWidth == (u32)sourceGraphics.mWidth;
    if (compatible) compatible = (u32)cached.mHeight == (u32)sourceGraphics.mHeight;
    if (compatible) compatible = (u32)cached.mDepth == (u32)sourceGraphics.mDepth;
    if (compatible) compatible = (u32)cached.mMipCount == (u32)sourceGraphics.mMipCount;
    if (compatible) compatible = (u32)cached.mFormat == (u32)sourceGraphics.mFormat;
    if (compatible) compatible = (u32)cached.mSwizzle == (u32)sourceGraphics.mSwizzle;
    if (compatible) compatible = (u32)cached.mTileMode == (u32)sourceGraphics.mTileMode;
    if (compatible) compatible = (u32)cached.mSamples == (u32)sourceGraphics.mSamples;
    if (!compatible) copyCaptureSurface(&mSurface, sourceSurface);
    else {
        void* image = sourceGraphics.mImage;
        void* mipImage = sourceGraphics.mMipImage;
        mSurface.mImageMirror = gabi::ea(image);
        cached.mImage = image;
        cached.mMipImage = mipImage;
        mSurface.mMipImageMirror = gabi::ea(mipImage);
    }
    attachCaptureTarget(mTarget, slot);
    // The original signed conversion uses this double-word scratch area twice.
    gabi::FrameLocal<be<u32>[2]> integerConversion(0x30);
    (*integerConversion)[0] = 0x43300000;
    (*integerConversion)[1] = (u32)width ^ 0x80000000;
    f32 widthFloat = (f32)width;
    (*integerConversion)[1] = (u32)height ^ 0x80000000;
    f32 heightFloat = (f32)height;
    f32 zero = sCaptureZero;
    gabi::FrameLocal<be<f32>[4]> viewportBounds(0x10);
    if (zero < widthFloat) { (*viewportBounds)[2] = widthFloat; (*viewportBounds)[0] = zero; }
    else { (*viewportBounds)[2] = zero; (*viewportBounds)[0] = widthFloat; }
    f32 left = (*viewportBounds)[0];
    f32 right = (*viewportBounds)[2];
    mViewportSurface.mLeft = left;
    if (zero < heightFloat) {
        (*viewportBounds)[3] = heightFloat;
        mViewportSurface.mBottom = zero;
        mViewportSurface.mTop = heightFloat;
        mViewportSurface.mRight = right;
        mViewportSurface.mWidth = widthFloat;
        (*viewportBounds)[1] = zero;
        mViewportSurface.mHeight = heightFloat;
    } else {
        (*viewportBounds)[3] = zero;
        mViewportSurface.mBottom = heightFloat;
        mViewportSurface.mTop = zero;
        mViewportSurface.mRight = right;
        mViewportSurface.mWidth = widthFloat;
        (*viewportBounds)[1] = heightFloat;
        mViewportSurface.mHeight = heightFloat;
    }
    gabi::FrameLocal<sead::Viewport> viewport(0xA0);
    configureCaptureViewport(viewport.get(), &mViewportSurface);
    applyCaptureViewport(viewport.get(), &mViewportSurface);
    activateCaptureSurface(&mViewportSurface);
    gabi::FrameLocal<sead::CompactRenderState> state(0xB8);
    createCaptureRenderState(state.get());
    state->mDepthWrite = 0; state->mBlendEnable = 0; state->mDepthTest = 0;
    applyCaptureRenderState(state.get());
    if ((u8)mTransferMode != 0) drawCaptureSurfaceWithGamma(&mSurface, 3, sCaptureGamma);
    else drawCaptureSurface(&mSurface, 3);
    prepareCaptureTarget(mTarget);
    finishCaptureGpuWork();
    gabi::FrameLocal<CaptureEncodedImage> encoded(0x28);
    if ((u8)mOutputKind != 0)
        encodeCaptureImageAlternate(encoded.get(), options, 1, slot, outputStorage);
    else encodeCaptureImage(encoded.get(), options, 1, slot, outputStorage, outputStorage);
    releaseCaptureSlot(sCaptureAllocatorOwner, slot);
    gabi::FrameLocal<sead::SafeText> emptyPath(0x20);
    emptyPath->mVtable = gabi::at<sead::SafeTextVtable>(gabi::ea(&sCaptureTextTable));
    emptyPath->mText = &sCaptureEmptyPath;
    path->assureTermination(); path->assureTermination();
    sead::SafeTextVtable* emptyTable = emptyPath->mVtable;
    be<u8>* originalPathText = path->mText;
    gabi::call_ptr<void>(emptyTable->mAssureTermination.mFunction, emptyPath.get());
    be<u8>* emptyText = emptyPath->mText;
    bool empty = gabi::ea(originalPathText) == gabi::ea(emptyText);
    if (!empty) {
        empty = false;
        be<u8>* input = path->mText;
        for (u32 index = 0; index < 0x40001; ++index) {
            u8 value = input[index]; u8 expected = emptyText[index];
            if (value != expected) { empty = false; break; }
            if (value == 0) { empty = true; break; }
        }
    }
    if (empty) { operator_delete(encoded->mData); return 0; }
    gabi::FrameLocal<sead::FileHandle> handle(0x64);
    initializeCaptureFileHandle(&handle->mDisposer);
    handle->mDevice = NULL; handle->mSourceDevice = NULL; handle->mReadChunkSize = 0;
    handle->mDisposer.mVtable = gabi::at<sead::DisposerVtable>(gabi::ea(&sCaptureHandleTable));
    sead::FileDevice* file = sCaptureFileProvider->mDevice->tryOpen(handle.get(), path, 1, 0);
    gabi::FrameLocal<be<u32>> written(0x38);
    *written = 0;
    file->tryWrite(written.get(), handle.get(), reinterpret_cast<const u8*>((void*)encoded->mData), encoded->mSize);
    file->tryClose(handle.get());
    operator_delete(encoded->mData);
    CaptureNoticeCallback* callback = mCallback;
    if (callback != NULL) {
        gabi::FrameLocal<CaptureNotice> notice(0x44);
        notice->mMode = mTransferMode;
        notice->mContext = mCallbackContext;
        notice->mWidth = (u32)width; notice->mHeight = (u32)height; notice->mOptions = options;
        notice->mPath.mText = path->mText;
        notice->mScale = mScale;
        notice->mPath.mVtable = gabi::at<sead::SafeTextVtable>(gabi::ea(&sCaptureTextTable));
        gabi::call_ptr<void>(callback->mTable->mInvoke, callback, notice.get());
    }
    sead::FileDevice* remainingDevice = handle->mSourceDevice;
    handle->mDisposer.mVtable = gabi::at<sead::DisposerVtable>(gabi::ea(&sCaptureHandleTable));
    if (remainingDevice != NULL) remainingDevice->tryClose(handle.get());
    finishCaptureFileHandle(&handle->mDisposer, 0);
    return 1;
}
/* WWHD glue
 data sCaptureAllocatorOwner 101F8DD4
 data sCaptureFileProvider 101F95F8
 data sCaptureSlotName 101676CC
 data sCaptureTextTable 10167508
 data sCaptureEmptyPath 101676C8
 data sCaptureHandleTable 101675B8
 data sCaptureZero 101676C0
 data sCaptureGamma 101676C4
 ext 027B6D64 void prepareCaptureTarget(void*)
 ext C0006168 void finishCaptureGpuWork()
 ext 027BDEB4 void copyCaptureSurface(agl::CaptureCachedSurface*, agl::CaptureSurfaceDescriptor*)
 ext 025C0928 void attachCaptureTarget(void*, void*)
 ext 0274F5F0 void configureCaptureViewport(sead::Viewport*, const sead::RenderSurface*)
 ext 0274F964 void applyCaptureViewport(sead::Viewport*, const sead::RenderSurface*)
 ext 0274D690 void activateCaptureSurface(sead::RenderSurface*)
 ext 0274FDBC void createCaptureRenderState(sead::CompactRenderState*)
 ext 0274FEB0 void applyCaptureRenderState(sead::CompactRenderState*)
 ext 027A9A24 void drawCaptureSurface(agl::CaptureCachedSurface*, u32)
 ext 027A9B0C void drawCaptureSurfaceWithGamma(agl::CaptureCachedSurface*, u32, f64)
 ext 0279FC6C void* acquireCaptureSlot(void*, const sead::SafeText*, u32, u32, u32, u32, u32, u32, u8, u8)
 ext 027A0040 void releaseCaptureSlot(void*, void*)
 ext 027C0574 void encodeCaptureImage(CaptureEncodedImage*, u32, u32, void*, void*, void*)
 ext 027C057C void encodeCaptureImageAlternate(CaptureEncodedImage*, u32, u32, void*, void*)
 ext 02752A84 void initializeCaptureFileHandle(sead::Disposer*)
 ext 02740B24 sead::FileDevice* sead::FileDevice::tryOpen(sead::FileHandle*, sead::SafeText*, u32, u32)
 ext 02740E08 u32 sead::FileDevice::tryWrite(be<u32>*, sead::FileHandle*, const u8*, u32)
 ext 02740BE0 u32 sead::FileDevice::tryClose(sead::FileHandle*)
 ext 02752BEC void finishCaptureFileHandle(sead::Disposer*, u32)
*/

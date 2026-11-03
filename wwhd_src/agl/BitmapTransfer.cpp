/* Bottom-up bitmap rows and surface-size preparation in the WWHD graphics library. */
// References: WWHD's own instructions and strings; GX2 SDK record layouts and import arguments are public wut/Cemu facts. No other reference.
#include "agl/BitmapTransfer.h"
static void* allocateObject(u32 bytes);
static void initializeSurfaceData(agl::SurfaceData_l* surface);
static void configureImageSurface(agl::SurfaceData_l* surface, u32 kind, u32 format, u32 width, u32 height,
                                  u32 depth, u32 mipLevels, u32 flags, u32 options);
static void applySurfaceLayout(agl::SurfaceData_l* surface, const agl::PlatformSurface_l* layout);
static void zeroRange(void* storage, u32 bytes);
struct BitmapTileModes { be<u8> mValues[3]; };
static const BitmapTileModes sBitmapTileModes;
static u32 sBitmapHalfwordConverter;
static agl::BitmapHeapSource_l* sBitmapHeapSource_l;
static u32 mapBitmapFormat(u32 format);
static void calculateSurface(agl::PlatformSurface_l* surface);
static void copySurface(const agl::PlatformSurface_l* source, u32 sourceLevel, u32 sourceSlice,
                         agl::PlatformSurface_l* destination, u32 destinationLevel, u32 destinationSlice);
static void installBitmapSurface(void* target, const agl::PlatformSurface_l* surface);

/* 027B63C0 */
agl::BitmapInfo* agl::BitmapInfo::construct(BitmapInfo* info, const BitmapHeader* header, u32 linearOutput) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(info) == 0) {
        info = (BitmapInfo*)allocateObject(sizeof(BitmapInfo));
        if (!info)
            return info;
    }
    u32 identifierSize = header->mIdentifierSize;
    info->mPixels = gabi::at<be<u8>>(gabi::ea(header) + identifierSize + 18);
    u32 converter = sBitmapHalfwordConverter;
    u32 width = gabi::call_ptr<u32>(converter, (u32)header->mWidth);
    info->mWidth = width;
    u32 height = gabi::call_ptr<u32>(converter, (u32)header->mHeight);
    info->mHeight = height;
    u32 sourceBytes = (u32)header->mPixelDepth >> 3;
    u32 currentWidth = info->mWidth;
    info->mDestinationPixelBytes = sourceBytes;
    info->mFormat = 0x1D;
    info->mSourcePixelBytes = sourceBytes;
    info->mInputSize = sourceBytes * currentWidth * height;
    if (sourceBytes == 1)
        info->mFormat = 1;
    else if (sourceBytes == 3)
        info->mDestinationPixelBytes = 4;
    gabi::FrameLocal<SurfaceData_l> surface(12);
    initializeSurfaceData(surface.get());
    u32 format = info->mFormat;
    u32 surfaceHeight = info->mHeight;
    u32 surfaceWidth = info->mWidth;
    configureImageSurface(surface.get(), 1, format, surfaceWidth, surfaceHeight, 1, 1, 0, 0);
    info->mOutputSize = (u32)surface->mSurface.mImageSize;
    if (linearOutput) {
        gabi::FrameLocal<PlatformSurface_l> linear(0x9C);
        // The original copies the SDK record as 29 consecutive words, not a floating-point copy.
        auto sourceWords = reinterpret_cast<const be<u32>*>(&surface->mSurface);
        auto destinationWords = reinterpret_cast<be<u32>*>(linear.get());
        for (u32 index = 0; index < 29; ++index)
            destinationWords[index] = (u32)sourceWords[index];
        linear->mTileMode = 1;
        applySurfaceLayout(surface.get(), linear.get());
        u32 size = info->mOutputSize;
        u32 linearSize = surface->mSurface.mImageSize;
        info->mOutputSize = linearSize > size ? linearSize : size;
    }
    return info;
}
/* 027B6614 */
void agl::copyBitmapRows(be<u8>* destination, const be<u8>* source, u32 components,
                         s32 width, s32 height, s32 stride) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (components != 1 && components != 3 && components != 4)
        return;
    u32 rowBytes = components == 1 ? (u32)width : (u32)width * 4;
    u32 rounded = (rowBytes + 31) & ~31u;
    u32 destinationStride = components == 1 ? (u32)stride : (u32)stride * 4;
    bool canZero = (gabi::ea(destination) & 31) == 0 && (s32)rounded <= (s32)destinationStride;
    if (height <= 0)
        return;
    u32 sourceStride = (u32)width * components;
    const be<u8>* sourceRow = gabi::at<const be<u8>>(gabi::ea(source) + ((u32)height - 1) * sourceStride);
    be<u8>* destinationRow = destination;
    for (u32 row = 0; row < (u32)height; ++row) {
        if (canZero)
            zeroRange(destinationRow, components == 1 ? destinationStride : rounded);
        if (width > 0) {
            for (u32 column = 0; column < (u32)width; ++column) {
                if (components == 1) {
                    u8 value = sourceRow[column];
                    destinationRow[column] = value;
                } else {
                    auto input = sourceRow + column * components;
                    auto output = destinationRow + column * 4;
                    u8 red = input[2];
                    output[0] = red;
                    u8 green = input[1];
                    output[1] = green;
                    u8 blue = input[0];
                    if (components == 3) {
                        output[3] = 255;
                        output[2] = blue;
                    } else {
                        output[2] = blue;
                        u8 alpha = input[3];
                        output[3] = alpha;
                    }
                }
            }
        }
        sourceRow = gabi::at<const be<u8>>(gabi::ea(sourceRow) - sourceStride);
        destinationRow = gabi::at<be<u8>>(gabi::ea(destinationRow) + destinationStride);
    }
}
/* 027B686C */
void agl::uploadBitmap(void* target, void* pixelStorage, const BitmapInfo* info, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!heap)
        heap = sBitmapHeapSource_l->mHeap;
    void* work = gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap,
                                      (u32)info->mOutputSize, 0x2000);
    u32 width = info->mWidth;
    u32 height = info->mHeight;
    auto source = info->mPixels.get();
    u32 components = info->mSourcePixelBytes;
    copyBitmapRows((be<u8>*)work, source, components, width, height, width);
    gabi::FrameLocal<PlatformSurface_l> destination(8);
    destination->mWidth = (u32)info->mWidth;
    destination->mHeight = (u32)info->mHeight;
    destination->mDepth = 1;
    destination->mDimension = 1;
    destination->mMipLevels = 1;
    destination->mFormat = mapBitmapFormat(info->mFormat);
    destination->mAntiAlias = 0;
    destination->mUsage = 1;
    destination->mSwizzle = 0;
    destination->mTileMode = 0;
    destination->mImage = pixelStorage;
    destination->mMipmaps = nullptr;
    calculateSurface(destination.get());
    gabi::FrameLocal<PlatformSurface_l> sourceSurface(0x7C);
    auto sourceWords = reinterpret_cast<const be<u32>*>(destination.get());
    auto destinationWords = reinterpret_cast<be<u32>*>(sourceSurface.get());
    for (u32 index = 0; index < 29; ++index)
        destinationWords[index] = (u32)sourceWords[index];
    sourceSurface->mImage = work;
    sourceSurface->mTileMode = 0x10;
    calculateSurface(sourceSurface.get());
    copySurface(sourceSurface.get(), 0, 0, destination.get(), 0, 0);
    installBitmapSurface(target, destination.get());
    gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, work);
}
/* 027B69A8 */
void agl::uploadBitmapInPlace(void* target, BitmapHeader* storage, u32 reserved, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<BitmapInfo> info(8);
    BitmapInfo::construct(info.get(), storage, 0);
    uploadBitmap(target, storage, info.get(), heap);
}
/* 027B6A08 */
u32 agl::mapBitmapTileMode(u32 mode) {
    return mode <= 2 ? (u32)sBitmapTileModes.mValues[mode] : 0;
}
/* WWHD glue
data sBitmapTileModes 10169950
ext 0273AD10 void* allocateObject(u32 bytes)
ext 027BE6B8 void initializeSurfaceData(agl::SurfaceData_l* surface)
ext 027BE930 void configureImageSurface(agl::SurfaceData_l* surface, u32 kind, u32 format, u32 width, u32 height, u32 depth, u32 mipLevels, u32 flags, u32 options)
ext 027BEA90 void applySurfaceLayout(agl::SurfaceData_l* surface, const agl::PlatformSurface_l* layout)
ext C00088C8 void zeroRange(void* storage, u32 bytes)
data sBitmapHalfwordConverter 1014761C
data sBitmapHeapSource_l 101F8C14
ext 02786348 u32 mapBitmapFormat(u32 format)
ext C00060D0 void calculateSurface(agl::PlatformSurface_l* surface)
ext C0006120 void copySurface(const agl::PlatformSurface_l* source, u32 sourceLevel, u32 sourceSlice, agl::PlatformSurface_l* destination, u32 destinationLevel, u32 destinationSlice)
ext 02786360 void installBitmapSurface(void* target, const agl::PlatformSurface_l* surface)
*/

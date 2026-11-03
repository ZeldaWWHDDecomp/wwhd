/* Bitmap header/pixel transfer views, reconstructed from the assigned WWHD readers. */
#pragma once
#include "agl/RenderSurface.h"
#include "sead/HeapRegistry.h"
namespace agl {
u32 mapBitmapTileMode(u32 mode);
void replaceBitmapMip(SurfaceData_l* object,const void* data,u32 level);
void prepareBitmapImage(void* target,void* pixels,u32 bytes,u32 format,u32 width,u32 height,sead::RegistryHeap* heap);
void convertBitmapLayout(void* outputImage,u32 unusedImageCapacity,void* outputMipmaps,u32 unusedMipmapCapacity,
                         u32 outputLayout,void* inputImage,void* inputMipmaps,u32 inputLayout,
                         u32 format,u32 width,u32 height,u32 mipLevels,void* workspace);
struct BitmapHeapSource_l { u8 mPrefix[16]; gptr<sead::RegistryHeap> mHeap; gptr<sead::RegistryHeap> mCompileHeap; };
struct BitmapHeader {
    be<u8> mIdentifierSize;
    u8 mPrefix[11];
    be<u16> mWidth;
    be<u16> mHeight;
    be<u8> mPixelDepth;
    be<u8> mAttributes;
};
struct BitmapInfo {
    be<u32> mWidth;
    be<u32> mHeight;
    gptr<be<u8>> mPixels;
    be<u32> mFormat;
    be<u32> mSourcePixelBytes;
    be<u32> mDestinationPixelBytes;
    be<u32> mInputSize;
    be<u32> mOutputSize;
    static BitmapInfo* construct(BitmapInfo* info, const BitmapHeader* header, u32 linearOutput);
};
void copyBitmapRows(be<u8>* destination, const be<u8>* source, u32 components,
                    s32 width, s32 height, s32 stride);
void uploadBitmap(void* target, void* pixelStorage, const BitmapInfo* info, sead::RegistryHeap* heap);
void uploadBitmapInPlace(void* target, BitmapHeader* storage, u32 reserved, sead::RegistryHeap* heap);
WWHD_OFFSET(BitmapHeapSource_l,mHeap,16);
WWHD_OFFSET(BitmapHeapSource_l,mCompileHeap,20);
WWHD_SIZE(BitmapHeapSource_l,24);
WWHD_OFFSET(BitmapHeader,mWidth,12);
WWHD_OFFSET(BitmapHeader,mHeight,14);
WWHD_OFFSET(BitmapHeader,mPixelDepth,16);
WWHD_SIZE(BitmapHeader,18);
WWHD_OFFSET(BitmapInfo,mPixels,8);
WWHD_OFFSET(BitmapInfo,mFormat,12);
WWHD_OFFSET(BitmapInfo,mSourcePixelBytes,16);
WWHD_OFFSET(BitmapInfo,mDestinationPixelBytes,20);
WWHD_OFFSET(BitmapInfo,mInputSize,24);
WWHD_OFFSET(BitmapInfo,mOutputSize,28);
WWHD_SIZE(BitmapInfo,32);
}

// Borrowed root pointer; one local data view until its owner is promoted.
extern gptr<agl::BitmapHeapSource_l>& sBitmapHeapSource_l;

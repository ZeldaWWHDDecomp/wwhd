// AGL capture encoder interfaces, reconstructed from WWHD's own calls and stores.
// format facts: Truevision TGA file format (header and footer layout)
#pragma once
#include "sead/HeapRegistry.h"
#include "sead/GraphicsSurface.h"
namespace agl {
sead::RegistryHeap* captureWorkHeap();
// Borrowed cache-slot prefix; the cache owner provides the full object and lifetime.
struct CaptureSurfacePrefix_l {
    be<u32> mFormat;
    sead::GraphicsSurface mSurface;
    be<u8> mMinimumWidth,mMinimumHeight;
};
struct CaptureImage {
    gptr<be<u8>> mData;
    be<u32> mBytes;
};
struct CaptureImageHeader {
    u8 mImageIdentification[2];
    be<u8> mImageType;
    u8 mColorAndOrigin[9];
    be<u16> mWidth,mHeight;
    be<u8> mPixelBits,mDescriptor;
};
struct CaptureImageFooter {
    be<u32> mExtensionOffset,mDeveloperOffset;
    be<u8> mSignature[18];
};
void encodeCaptureImage(CaptureImage*,u32 format,u32 flip,const CaptureSurfacePrefix_l*,sead::RegistryHeap* outputHeap,sead::RegistryHeap* workHeap,u32 allowFallback);
void encodeCaptureImageWithHeap(CaptureImage*,u32,u32,const CaptureSurfacePrefix_l*,sead::RegistryHeap*,sead::RegistryHeap*);
void encodeCaptureImageWithFallback(CaptureImage*,u32,u32,const CaptureSurfacePrefix_l*,sead::RegistryHeap*);
}
WWHD_OFFSET(agl::CaptureSurfacePrefix_l,mSurface,4);
WWHD_OFFSET(agl::CaptureSurfacePrefix_l,mMinimumWidth,0x78);
WWHD_OFFSET(agl::CaptureSurfacePrefix_l,mMinimumHeight,0x79);
WWHD_SIZE(agl::CaptureImage,8);
WWHD_OFFSET(agl::CaptureImage,mBytes,4);
WWHD_SIZE(agl::CaptureImageHeader,18);
WWHD_OFFSET(agl::CaptureImageHeader,mImageType,2);
WWHD_OFFSET(agl::CaptureImageHeader,mWidth,12);
WWHD_OFFSET(agl::CaptureImageHeader,mPixelBits,16);
WWHD_SIZE(agl::CaptureImageFooter,26);

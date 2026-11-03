// AGL capture image serialization, reconstructed from the supplied binary. No external code used.
// format facts: Truevision TGA file format (header fields, extension/developer offsets, footer signature)
#include "agl/CaptureImage.h"
namespace agl {}
using namespace agl;
struct CaptureOutputFormats { be<u8> mPixelBits[3],mImageTypes[3]; };
struct CaptureSignature { be<u8> mBytes[18]; };
struct CaptureHeapSelectionPrefix_l { u8 mOwnerState[16];gptr<sead::RegistryHeap> mWorkHeap; };
static const CaptureOutputFormats sCaptureFormats;
static const CaptureSignature sCaptureSignature;
typedef be<u32> CaptureConversionPointer;
static CaptureConversionPointer sCaptureHalfwordConversion;
static sead::SafeTextVtable sCaptureFallbackNameMethods;
typedef be<u8> CaptureNameByte;
static CaptureNameByte sCaptureFallbackNameFirst;
// Borrowed words owned by the unmerged AGL allocator/context owners. No backing initializer.
static CaptureHeapSelectionPrefix_l* sCaptureHeapSelection;
static void* sCaptureCacheOwner;
static void* sCaptureSyncOwner;
void* clearCaptureBytes(void*,u32,u32);
void* copyCaptureBytes(void*,const void*,u32,u32);
void prepareCaptureSurface(sead::GraphicsSurface*);
void invalidateCaptureMemory(void*,u32);
void copyCaptureSurface(const sead::GraphicsSurface*,u32,u32,const sead::GraphicsSurface*,u32,u32);
void synchronizeCaptureOwner(void*);
void invalidateCaptureGpu(u32,void*,u32);
CaptureSurfacePrefix_l* acquireCaptureCache(void*,sead::SafeText*,u32,u32,u32,u32,u32,u32,u32,u32);
void releaseCaptureCache(void*,CaptureSurfacePrefix_l*);
static void* allocateCaptureStorage(sead::RegistryHeap* heap,u32 size,u32 alignment) {
    u32 target=heap->dispatch()->mAllocate.mFunction;
    return gabi::call_ptr<void*>(target,heap,size,alignment);
}
sead::RegistryHeap* agl::captureWorkHeap() { return sCaptureHeapSelection->mWorkHeap; }
/* 027BFFB4 */
void agl::encodeCaptureImage(CaptureImage* output,u32 format,u32 flip,const CaptureSurfacePrefix_l* source,sead::RegistryHeap* outputHeap,sead::RegistryHeap* workHeap,u32 allowFallback) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!workHeap) workHeap=sCaptureHeapSelection->mWorkHeap;
    u32 pixelBits=32;
    s32 minimumWidth=source->mMinimumWidth;
    if(format<=2) pixelBits=sCaptureFormats.mPixelBits[format];
    s32 width=source->mSurface.mWidth;
    s32 sourceHeight=source->mSurface.mHeight;
    if(minimumWidth>width) width=minimumWidth;
    s32 height=source->mMinimumHeight;
    if(sourceHeight>height) height=sourceHeight;
    width=static_cast<u16>(width);height=static_cast<u16>(height);
    s32 bitCount=static_cast<s32>(static_cast<u32>(width)*static_cast<u32>(height)*pixelBits);
    s32 payloadBytes=bitCount/8;
    u32 totalBytes=static_cast<u32>(payloadBytes)+44;
    output->mBytes=totalBytes;
    void* allocation=allocateCaptureStorage(outputHeap,totalBytes,64);
    auto* header=gabi::at<CaptureImageHeader>(gabi::ea(allocation));
    output->mData=static_cast<be<u8>*>(allocation);
    clearCaptureBytes(allocation,0,18);
    if(format<=2) header->mImageType=sCaptureFormats.mImageTypes[format];
    u32 conversion=sCaptureHalfwordConversion;
    header->mWidth=gabi::call_ptr<u32>(conversion,static_cast<u32>(width));
    header->mHeight=gabi::call_ptr<u32>(conversion,static_cast<u32>(height));
    header->mPixelBits=pixelBits;header->mDescriptor=8;
    be<u8>* data=output->mData;
    auto* footer=gabi::at<CaptureImageFooter>(gabi::ea(data)+static_cast<u32>(payloadBytes)+18);
    footer->mExtensionOffset=0;footer->mDeveloperOffset=0;
    be<u8>* pixels=gabi::at<be<u8>>(gabi::ea(data)+18);
    copyCaptureBytes(footer->mSignature,&sCaptureSignature,18,0);
    gabi::FrameLocal<sead::GraphicsSurface> surface(0x18);
    surface->mSamples=0;surface->mHeight=height;surface->mFormat=0x1A;
    surface->mMipCount=1;surface->mSwizzle=0;surface->mDimension=1;
    surface->mTileMode=16;surface->mDepth=1;surface->mUsage=3;surface->mWidth=width;
    prepareCaptureSurface(surface);
    u32 allocationBytes=(static_cast<u32>(surface->mImageBytes)+31)&~31u;
    u32 alignment=(static_cast<u32>(surface->mAlignment)+31)&~31u;
    surface->mImage=allocateCaptureStorage(workHeap,allocationBytes,alignment);
    CaptureSurfacePrefix_l* cache=nullptr;
    if(!surface->mImage && allowFallback) {
        gabi::FrameLocal<sead::SafeText> name(0x10);
        name->mVtable=&sCaptureFallbackNameMethods;name->mText=&sCaptureFallbackNameFirst;
        cache=acquireCaptureCache(sCaptureCacheOwner,name,0x1D,(static_cast<u32>(width)+255)&~255u,height,1,0,2,1,0);
        surface->mImage=cache->mSurface.mImage;
    }
    void* temporary=surface->mImage;
    invalidateCaptureMemory(temporary,allocationBytes);
    copyCaptureSurface(&source->mSurface,0,0,surface,0,0);
    synchronizeCaptureOwner(sCaptureSyncOwner);
    invalidateCaptureGpu(16,surface->mImage,allocationBytes);
    const be<u8>* input=static_cast<be<u8>*>(static_cast<void*>(surface->mImage));
    u32 bytesPerPixel=pixelBits>>3;
    if(bytesPerPixel==1 || bytesPerPixel==3 || bytesPerPixel==4) {
        for(s32 row=0;row<height;++row) {
            u32 sourceRow=flip ? static_cast<u32>(height-1-row) : static_cast<u32>(row);
            for(s32 column=0;column<width;++column) {
                u32 sourceOffset=(sourceRow*static_cast<u32>(width)+static_cast<u32>(column))*4;
                const be<u8>* pixel=gabi::at<const be<u8>>(gabi::ea(input)+sourceOffset);
                if(bytesPerPixel==1) { *pixels=pixel[0];pixels=gabi::at<be<u8>>(gabi::ea(pixels)+1); }
                else {
                    pixels[0]=pixel[2];pixels[1]=pixel[1];pixels[2]=pixel[0];
                    if(bytesPerPixel==4) pixels[3]=pixel[3];
                    pixels=gabi::at<be<u8>>(gabi::ea(pixels)+bytesPerPixel);
                }
            }
        }
    }
    if(cache) releaseCaptureCache(sCaptureCacheOwner,cache);
    else {
        u32 release=workHeap->dispatch()->mRelease.mFunction;
        gabi::call_ptr<void>(release,workHeap,input);
    }
}
/* 027C0574 */
void agl::encodeCaptureImageWithHeap(CaptureImage* output,u32 format,u32 flip,const CaptureSurfacePrefix_l* source,sead::RegistryHeap* outputHeap,sead::RegistryHeap* workHeap) {
    gabi::AutoFrame frame{gabi::current_function()};
    encodeCaptureImage(output,format,flip,source,outputHeap,workHeap,0);
}
/* 027C057C */
void agl::encodeCaptureImageWithFallback(CaptureImage* output,u32 format,u32 flip,const CaptureSurfacePrefix_l* source,sead::RegistryHeap* outputHeap) {
    gabi::AutoFrame frame{gabi::current_function()};
    encodeCaptureImage(output,format,flip,source,outputHeap,nullptr,1);
}
/* WWHD glue
data sCaptureFormats 10169DE4
data sCaptureSignature 10169DEC
data sCaptureHalfwordConversion 1014761C
data sCaptureFallbackNameMethods 10169DCC
data sCaptureFallbackNameFirst 10169E00
data sCaptureHeapSelection 101F8C14
data sCaptureCacheOwner 101F8DD4
data sCaptureSyncOwner 101F8BD8
ext C0009990 void* clearCaptureBytes(void*,u32,u32)
ext C0009988 void* copyCaptureBytes(void*,const void*,u32,u32)
ext C00060D0 void prepareCaptureSurface(sead::GraphicsSurface*)
ext C00088A8 void invalidateCaptureMemory(void*,u32)
ext C0006120 void copyCaptureSurface(const sead::GraphicsSurface*,u32,u32,const sead::GraphicsSurface*,u32,u32)
ext 0276B114 void synchronizeCaptureOwner(void*)
ext C00064A8 void invalidateCaptureGpu(u32,void*,u32)
ext 0279FC6C agl::CaptureSurfacePrefix_l* acquireCaptureCache(void*,sead::SafeText*,u32,u32,u32,u32,u32,u32,u32,u32)
ext 027A0040 void releaseCaptureCache(void*,agl::CaptureSurfacePrefix_l*)
*/

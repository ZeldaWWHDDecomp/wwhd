/* Logical frame-buffer construction and attachment cleanup for WWHD. */
// References: WWHD's own instructions and strings; GX2 SDK record layouts and import arguments are public wut/Cemu facts. No other reference.
#include "agl/RenderBuffer.h"
static void* allocateObject(u32 bytes);
static void releaseObject(void* object);
static void destroyVirtualFrameBuffer(sead::VirtualFrameBuffer* buffer, u32 flags);
// Address-only views: the methods are not read through these markers here.
static u8 sLogicalBufferMethods;
static u8 sRenderBufferMethods;
// Borrowed registry prefix; the canonical storage owner is pending identification.
static const agl::RenderBufferRegistry_l sRenderBuffers;

/* 027B954C */
void agl::RenderBuffer::clearColors() {
    for (u32 index = 0; index < 8; ++index)
        mColors[index] = nullptr;
}
/* 027B958C */
void agl::RenderBuffer::clear() {
    gabi::AutoFrame frame{gabi::current_function()};
    clearColors();
    mDepth = nullptr;
}
/* 027B95B4 */
agl::RenderBuffer* agl::RenderBuffer::construct() {
    gabi::AutoFrame frame{gabi::current_function()};
    RenderBuffer* result = this;
    if (gabi::ea(result) == 0) {
        result = (RenderBuffer*)allocateObject(sizeof(RenderBuffer));
        if (!result)
            return result;
    }
    result->mBase.mVtable = &sLogicalBufferMethods;
    auto size = &result->mBase.mSize;
    if (gabi::ea(size) == 0)
        size = (sead::FrameBufferSize*)allocateObject(8);
    if (size) {
        size->mHeight = 1.0f;
        size->mWidth = 1.0f;
    }
    auto area = &result->mBase.mArea;
    if (gabi::ea(area) == 0)
        area = (sead::FrameBufferArea*)allocateObject(16);
    if (area) {
        area->mMaxX = 1.0f;
        area->mMinY = 0.0f;
        area->mMinX = 0.0f;
        area->mMaxY = 1.0f;
    }
    result->mBase.mVtable = &sRenderBufferMethods;
    if (gabi::ea(result->mColors) == 0)
        allocateObject(32);
    result->clear();
    return result;
}
/* 027B9694 */
void agl::RenderBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0)
        return;
    mBase.mVtable = &sRenderBufferMethods;
    s32 count = sRenderBuffers.mCount;
    if (count > 0) {
        for (u32 index = 0; index < (u32)count; ++index) {
            u32 currentCount = sRenderBuffers.mCount;
            auto entries = sRenderBuffers.mEntries.get();
            auto entry = entries + (index < currentCount ? index : 0);
            RenderBuffer* buffer = *entry;
            if (buffer == this)
                *entry = nullptr;
        }
    }
    destroyVirtualFrameBuffer(&mBase, 0);
    if (flags & 1)
        releaseObject(this);
}

/* WWHD color resolve, with context and SDK boundaries kept explicit. */
#include "agl/RenderBuffer.h"
#include "agl/GraphicsAccess.h"
#include "sead/RuntimeType.h"
#include "port_native_draw.h"
using namespace agl;
// Borrowed from sead/DisplayTargets.cpp.
struct ScanTargetMethods_l { u8 mPrefix[12]; be<u32> mMatches; };
static u32 sScanGuard_l;
static sead::RuntimeType sScanType_l;
static u8 sScanTypeTable_l;
static void invalidateResolveTexture(SurfaceData_l*);
static void setResolveViewport(f64,f64,f64,f64,f64,f64);
static void setResolveScissor(u32,u32,u32,u32);
static void calculateResolveSurface(PlatformSurface_l*);
static void initializeResolveColor(PlatformColorBuffer_l*);
static void resolveNativeColor(PlatformColorBuffer_l*,u32);
// Active AGL#1 owns ShaderContext (0x9C); declaration-only borrowed receiver here.
static void applyResolveShaderContext(void*);
WWHD_OFFSET(ScanTargetMethods_l,mMatches,12);
static void copyResolveSurface(PlatformSurface_l* to,const PlatformSurface_l* from) {
 auto* destination=reinterpret_cast<be<u32>*>(to);auto* source=reinterpret_cast<const be<u32>*>(from);
 for (u32 i=0;i<29;++i) destination[i]=static_cast<u32>(source[i]);
}
/* 027B975C */
void agl::RenderBuffer::resolveColor(sead::ScanTarget* target) {
 gabi::AutoFrame activation{gabi::current_function()};
 gabi::FrameLocal<PlatformColorBuffer_l> temporary(0x10);
 gabi::FrameLocal<PlatformSurface_l> corrected(0xAC);
 if (!static_cast<u32>(sScanGuard_l)) {sScanGuard_l=1;sScanType_l.mTable=&sScanTypeTable_l;}
 if (!target || !gabi::call_ptr<u32>(static_cast<ScanTargetMethods_l*>(target->mTable.get())->mMatches,target,&sScanType_l)) target=nullptr;
 ColorSurface* color=mColors[0];color->invalidate();
 color=mColors[0];invalidateResolveTexture(&color->mBase);
 // A matching ScanTarget is required; the original does not return on a failed type check.
 u32 mode=static_cast<u32>(target->mTarget)?4:1;
 color=mColors[0];
 u32 width=color->mColor.mSurface.mWidth,height=color->mColor.mSurface.mHeight;
 PlatformColorBuffer_l* original=&color->mColor;
 setResolveViewport(0,0,static_cast<f32>(static_cast<s32>(width)),static_cast<f32>(static_cast<s32>(height)),0,1);
 setResolveScissor(0,0,width,height);
 if (mode==1) {
  u32 format=original->mSurface.mFormat,antiAlias=original->mSurface.mAntiAlias;
  temporary->mSurface.mDimension=1;temporary->mSurface.mWidth=width;
  temporary->mViewNumSlices=1;temporary->mSurface.mFormat=format;
  temporary->mSurface.mUsage=0x80000003;temporary->mSurface.mDepth=1;
  temporary->mViewFirstSlice=0;temporary->mSurface.mMipLevels=1;
  temporary->mViewMip=0;temporary->mSurface.mAntiAlias=antiAlias;
  temporary->mSurface.mSwizzle=0;temporary->mSurface.mHeight=height;temporary->mSurface.mTileMode=0;
  calculateResolveSurface(&temporary->mSurface);initializeResolveColor(temporary.get());
  if (static_cast<u32>(temporary->mSurface.mAntiAlias)!=antiAlias) {
   copyResolveSurface(corrected.get(),&temporary->mSurface);
   corrected->mAntiAlias=antiAlias;corrected->mUsage=3;calculateResolveSurface(corrected.get());
   temporary->mSurface.mImageSize=static_cast<u32>(corrected->mImageSize);
   temporary->mSurface.mAlignment=static_cast<u32>(corrected->mAlignment);
  }
  temporary->mSurface.mImage=original->mSurface.mImage;
  color=mColors[0];temporary->mAuxBuffer=color->mColor.mAuxBuffer;
  temporary->mSurface.mTileMode=static_cast<u32>(original->mSurface.mTileMode);
#if PORT_NATIVE_DRAW
  wwhd_native_pass_copy(PORT_NATIVE_COPY_SCAN, mode == 1 ? 0 : 1, gabi::ea(temporary.get()), 0);  // (port_native_draw.h)
#endif
  resolveNativeColor(temporary.get(),mode);
 } else {
  color=mColors[0];
#if PORT_NATIVE_DRAW
  wwhd_native_pass_copy(PORT_NATIVE_COPY_SCAN, mode == 1 ? 0 : 1, gabi::ea(&color->mColor), 0);  // (port_native_draw.h)
#endif
  resolveNativeColor(&color->mColor,mode);
 }
 applyResolveShaderContext(sNativeResources_l);
}
/* WWHD glue
ext 0273AD10 void* allocateObject(u32 bytes)
ext 0273AF40 void releaseObject(void* object)
ext 0274D67C void destroyVirtualFrameBuffer(sead::VirtualFrameBuffer* buffer, u32 flags)
data sLogicalBufferMethods 10169B78
data sRenderBufferMethods 10169BD8
data sRenderBuffers 104A2090
data sScanGuard_l 101FD750
data sScanType_l 101FDDFC
data sScanTypeTable_l 10169BB8
ext 027BE794 void invalidateResolveTexture(SurfaceData_l*)
ext C0006A50 void setResolveViewport(f64,f64,f64,f64,f64,f64)
ext C0006970 void setResolveScissor(u32,u32,u32,u32)
ext C00060D0 void calculateResolveSurface(PlatformSurface_l*)
ext C00063D0 void initializeResolveColor(PlatformColorBuffer_l*)
ext C0006110 void resolveNativeColor(PlatformColorBuffer_l*,u32)
ext 0276B114 void applyResolveShaderContext(void*)
*/

/* WWHD bitmap surface preparation. Protected targets retain opaque interface bindings. */
// References: WWHD's own instructions and strings; GX2 SDK record layouts and import arguments are public wut/Cemu facts. No other reference.
#include "agl/BitmapTransfer.h"
#include "agl/GraphicsAccess.h"
using namespace agl;
static u32 mapBitmapFormat(u32);
static void calculateProtectedSurface(PlatformSurface_l*);
static void installBitmapSurface(void*,const PlatformSurface_l*);
static void calculateSdkSurface(PlatformSurface_l*);
static void copySdkSurface(const PlatformSurface_l*,u32,u32,PlatformSurface_l*,u32,u32);
static void applyBitmapShaderContext(void*);
static void* moveBitmapBytes(void*,const void*,u32,u32);
static void flushBitmapBytes(void*,u32);
static void releaseBitmapBytes(void*);
static void copySurfaceWords(PlatformSurface_l* destination,const PlatformSurface_l* source);
static u32 alignBitmapAddress(u32 address,u32 alignment);
/* 027B5EEC */
void agl::prepareBitmapImage(void* target,void* pixels,u32 bytes,u32 format,
                               u32 width,u32 height,sead::RegistryHeap* heap) {
 gabi::AutoFrame activation{gabi::current_function()};
 gabi::FrameLocal<PlatformSurface_l> destination(8);
 gabi::FrameLocal<PlatformSurface_l> source(0x7C);
 destination->mHeight=height;
 destination->mMipLevels=1;
 destination->mDepth=1;
 destination->mDimension=1;
 destination->mWidth=width;
 u32 nativeFormat=mapBitmapFormat(format);
 destination->mUsage=1;
 destination->mAntiAlias=0;
 destination->mFormat=nativeFormat;
 destination->mMipmaps=nullptr;
 destination->mSwizzle=0;
 destination->mTileMode=0;
 destination->mImage=nullptr;
 calculateSdkSurface(destination.get());
 if (!heap) heap=sBitmapHeapSource_l->mHeap;
 void* allocation=gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,
                                      static_cast<u32>(destination->mImageSize),static_cast<u32>(destination->mAlignment));
 destination->mImage=allocation;
 copySurfaceWords(source.get(),destination.get());
 source->mImage=pixels;
 source->mTileMode=0x10;
 calculateSdkSurface(source.get());
 copySdkSurface(source.get(),0,0,destination.get(),0,0);
 moveBitmapBytes(pixels,destination->mImage,bytes,0);
 flushBitmapBytes(pixels,bytes);
 gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,destination->mImage.get());
 destination->mImage=pixels;
 installBitmapSurface(target,destination.get());
}

// Unit-local helpers are included in the canonical mutation ranges.
static void copySurfaceWords(PlatformSurface_l* destination,const PlatformSurface_l* source) {
 const be<u32>* from=reinterpret_cast<const be<u32>*>(source);
 be<u32>* to=reinterpret_cast<be<u32>*>(destination);
 for (u32 i=0;i<29;++i) to[i]=static_cast<u32>(from[i]);
}

static u32 alignBitmapAddress(u32 address,u32 alignment) {
 u32 numerator=address+alignment-1;
 return (alignment?numerator/alignment:0)*alignment;
}

/* 027B6A24 */
void agl::convertBitmapLayout(void* outputImage,u32 unusedImageCapacity,void* outputMipmaps,u32 unusedMipmapCapacity,
                                u32 outputLayout,void* inputImage,void* inputMipmaps,u32 inputLayout,
                                u32 format,u32 width,u32 height,u32 mipLevels,void* workspace) {
 gabi::AutoFrame activation{gabi::current_function()};
 gabi::FrameLocal<PlatformSurface_l> destination(8);
 gabi::FrameLocal<PlatformSurface_l> source(0x7C);
 source->mHeight=height;
 source->mDepth=1;
 source->mWidth=width;
 source->mDimension=1;
 source->mMipLevels=mipLevels;
 u32 nativeFormat=mapBitmapFormat(format);
 source->mMipmaps=inputMipmaps;
 source->mFormat=nativeFormat;
 source->mImageSize=0;
 source->mAntiAlias=0;
 source->mMipmapSize=0;
 source->mUsage=1;
 source->mImage=inputImage;
 source->mTileMode=mapBitmapTileMode(inputLayout);
 source->mAlignment=0;
 source->mPitch=0;
 source->mSwizzle=0;
 calculateProtectedSurface(source.get());
 copySurfaceWords(destination.get(),source.get());
 destination->mTileMode=mapBitmapTileMode(outputLayout);
 calculateProtectedSurface(destination.get());
 if (workspace) {
  u32 alignment=destination->mAlignment;
  u32 image=alignBitmapAddress(gabi::ea(workspace),alignment);
  u32 imageSize=destination->mImageSize;
  u32 mipmapSize=destination->mMipmapSize;
  destination->mImage=gabi::at<void>(image);
  if (mipmapSize) destination->mMipmaps=gabi::at<void>(alignBitmapAddress(image+imageSize,alignment));
 } else {
  sead::RegistryHeap* heap=sBitmapHeapSource_l->mHeap;
  void* image=gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,
                                  static_cast<u32>(destination->mImageSize),static_cast<u32>(destination->mAlignment));
  u32 mipmapSize=destination->mMipmapSize;
  destination->mImage=image;
  if (mipmapSize) {
   heap=sBitmapHeapSource_l->mHeap;
   destination->mMipmaps=gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,mipmapSize,
                                             static_cast<u32>(destination->mAlignment));
  } else destination->mMipmaps=nullptr;
 }
 for (u32 level=0;level<mipLevels;++level) copySdkSurface(source.get(),level,0,destination.get(),level,0);
 if (outputLayout && inputLayout) applyBitmapShaderContext(sNativeResources_l);
 moveBitmapBytes(outputImage,destination->mImage,static_cast<u32>(destination->mImageSize),0);
 u32 mipmapSize=destination->mMipmapSize;
 if (mipmapSize) moveBitmapBytes(outputMipmaps,destination->mMipmaps,mipmapSize,0);
 if (!workspace) {
  releaseBitmapBytes(destination->mImage);
  void* mipmaps=destination->mMipmaps;
  if (mipmaps) releaseBitmapBytes(mipmaps);
 }
}

/* WWHD bitmap mip replacement; SDK and heap interfaces retain their bindings. */
#include "agl/BitmapTransfer.h"
using namespace agl;
static void calculateMipSurface(PlatformSurface_l*);
static void copyMipSurface(const PlatformSurface_l*,u32,u32,PlatformSurface_l*,u32,u32);
static void* moveMipBytes(void*,const void*,u32,u32);
static void flushMipBytes(void*,u32);
static void copyMipSurfaceWords(PlatformSurface_l* to,const PlatformSurface_l* from) {
 auto* output=reinterpret_cast<be<u32>*>(to);auto* input=reinterpret_cast<const be<u32>*>(from);
 for (u32 i=0;i<29;++i) output[i]=static_cast<u32>(input[i]);
}
static void* allocateMipBytes(u32 size,u32 alignment) {
 sead::RegistryHeap* heap=sBitmapHeapSource_l->mHeap;
 return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,size,alignment);
}
static void releaseMipBytes(void* memory) {
 sead::RegistryHeap* heap=sBitmapHeapSource_l->mHeap;
 gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction,heap,memory);
}
static void* addMipOffset(void* memory,u32 offset) {return gabi::at<void>(gabi::ea(memory)+offset);}
/* 027B6030 */
void agl::replaceBitmapMip(SurfaceData_l* object,const void* data,u32 level) {
 gabi::AutoFrame activation{gabi::current_function()};
 gabi::FrameLocal<PlatformSurface_l> linear(8),converted(0x7C);
 copyMipSurfaceWords(converted.get(),&object->mSurface);
 converted->mImage=nullptr;converted->mMipLevels=13;converted->mMipmaps=nullptr;
 calculateMipSurface(converted.get());
 converted->mImage=allocateMipBytes(converted->mImageSize,converted->mAlignment);
 converted->mMipmaps=allocateMipBytes(converted->mMipmapSize,converted->mAlignment);
 copyMipSurfaceWords(linear.get(),&object->mSurface);
 linear->mTileMode=0x10;linear->mImage=nullptr;linear->mMipmaps=nullptr;linear->mMipLevels=13;
 calculateMipSurface(linear.get());
 linear->mImage=allocateMipBytes(linear->mImageSize,0x2000);
 void* linearMipmaps=allocateMipBytes(linear->mMipmapSize,0x2000);
 linear->mMipmaps=linearMipmaps;
 u32 offset=0,convertedBytes;
 if (level==0) {
  u32 bytes=linear->mImageSize;convertedBytes=converted->mImageSize;
  moveMipBytes(linear->mImage,data,bytes,0);
 } else if (level==1) {
  u32 bytes=linear->mMipOffsets[1];convertedBytes=converted->mMipOffsets[1];
  moveMipBytes(linearMipmaps,data,bytes,0);
 } else {
  u32 levels=linear->mMipLevels,linearOffset=linear->mMipOffsets[level-1];
  offset=converted->mMipOffsets[level-1];
  void* destination=addMipOffset(linearMipmaps,linearOffset);
  u32 bytes;
  if (level==levels-1) {bytes=static_cast<u32>(linear->mMipmapSize)-linearOffset;convertedBytes=static_cast<u32>(converted->mMipmapSize)-offset;}
  else {bytes=static_cast<u32>(linear->mMipOffsets[level])-linearOffset;convertedBytes=static_cast<u32>(converted->mMipOffsets[level])-offset;}
  moveMipBytes(destination,data,bytes,0);
 }
 u32 slice=0;
 while (slice<static_cast<u32>(linear->mDepth)) {copyMipSurface(linear.get(),level,slice,converted.get(),level,slice);++slice;}
 if (level==0) {
  moveMipBytes(addMipOffset(object->mSurface.mImage,offset),addMipOffset(converted->mImage,offset),convertedBytes,0);
  flushMipBytes(object->mSurface.mImage,object->mSurface.mImageSize);
 } else {
  moveMipBytes(addMipOffset(object->mSurface.mMipmaps,offset),addMipOffset(converted->mMipmaps,offset),convertedBytes,0);
  flushMipBytes(object->mSurface.mMipmaps,object->mSurface.mMipmapSize);
 }
 releaseMipBytes(linear->mImage);releaseMipBytes(converted->mImage);
 releaseMipBytes(linear->mMipmaps);releaseMipBytes(converted->mMipmaps);
}
/* WWHD glue
ext 02786348 u32 mapBitmapFormat(u32)
ext 0278635C void calculateProtectedSurface(PlatformSurface_l*)
ext 02786360 void installBitmapSurface(void*,const PlatformSurface_l*)
ext C00060D0 void calculateSdkSurface(PlatformSurface_l*)
ext C0006120 void copySdkSurface(const PlatformSurface_l*,u32,u32,PlatformSurface_l*,u32,u32)
ext 0276B114 void applyBitmapShaderContext(void*)
ext C0009988 void* moveBitmapBytes(void*,const void*,u32,u32)
ext C00088A0 void flushBitmapBytes(void*,u32)
ext 0273AF40 void releaseBitmapBytes(void*)
ext C00060D0 void calculateMipSurface(PlatformSurface_l*)
ext C0006120 void copyMipSurface(const PlatformSurface_l*,u32,u32,PlatformSurface_l*,u32,u32)
ext C0009988 void* moveMipBytes(void*,const void*,u32,u32)
ext C00088A0 void flushMipBytes(void*,u32)
*/

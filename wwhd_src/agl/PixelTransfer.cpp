// AGL transfer allocation and packed-channel extraction, reconstructed from the supplied binary.
#include "agl/PixelTransfer.h"
#include "sead/DisplayTargets.h"
#include "sead/GraphicsRuntime.h"
#include "sead/RenderState.h"
namespace agl {}
using namespace agl;
static sead::DisposerVtable sPixelTransferMethods;
typedef be<u32> PixelWordConverter;
static PixelWordConverter sPixelWordConversion;
void initializePixelColorBuffer(TransferColorStorage_l*);
static u32 shiftPixelLeft(u32 value,u32 count) { count&=63;return count<32 ? value<<count : 0; }
static u32 shiftPixelRight(u32 value,u32 count) { count&=63;return count<32 ? value>>count : 0; }
// Temporary records: only the extents and fields used by this unit are named.
struct TransferSurfaceStorage_l { CaptureSurfacePrefix_l mPrefix;u8 mTail[0x90-sizeof(CaptureSurfacePrefix_l)]; };
struct TransferTargetStorage_l { sead::DisplayFrame mFrame;u8 mTail[0x1C]; };
struct TransferQuadStorage_l { u8 mBeforeEnable[0x91];be<u8> mEnabled;u8 mTail[0x198-0x92]; };
struct PixelFormatRemapping { be<u8> mFormats[10]; };
static const PixelFormatRemapping sPixelFormatRemapping;
static const sead::RenderVector2 sTransferQuadSize;
static const sead::RenderVector2 sTransferQuadOffset;
void createTransferSurface(TransferSurfaceStorage_l*);
void configureTransferSurface(TransferSurfaceStorage_l*,u32,u32,u32,u32,u32,u32,u32,u32);
void copyTransferSurfaceLayout(TransferSurfaceStorage_l*,const sead::GraphicsSurface*);
void assignTransferColor(TransferColorStorage_l*,const TransferSurfaceStorage_l*);
void createTransferTarget(TransferTargetStorage_l*);
void destroyTransferTarget(TransferTargetStorage_l*,u32);
void createTransferQuad(TransferQuadStorage_l*,const CaptureSurfacePrefix_l*);
void destroyTransferQuad(TransferQuadStorage_l*,u32);
void renderTransferQuad(TransferQuadStorage_l*,const sead::Viewport*,const sead::RenderVector2*,const sead::RenderVector2*,u32);
void finishTransferColor(TransferColorStorage_l*);
void invalidateCaptureMemory(void*,u32);
void activateTransferTarget(sead::DisplayFrame*);
/* 027BF144 */
agl::PixelBufferTransfer* agl::PixelBufferTransfer::create(PixelBufferTransfer* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!storage) storage=static_cast<PixelBufferTransfer*>(allocateRegistryStorage(sizeof(PixelBufferTransfer)));
    if(storage) {
        storage->mPixels=nullptr;storage->mRowStride=0;storage->mVtable=&sPixelTransferMethods;
        initializePixelColorBuffer(&storage->mColorBuffer);
    }
    return storage;
}
/* 027BF1A4 */
void agl::PixelBufferTransfer::release() {
    gabi::AutoFrame frame{gabi::current_function()};
    if(mPixels) { releaseRegistryStorage(mPixels);mPixels=nullptr; }
}
/* 027BF1E8 */
void agl::PixelBufferTransfer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(gabi::ea(this)) {
        mVtable=&sPixelTransferMethods;release();
        if(flags&1) releaseRegistryStorage(this);
    }
}
/* 027BF244 */
void agl::PixelBufferTransfer::readChannels(PixelChannels* output,u32 x,u32 y) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 bytes=vertexFormatBytes(mFormat);
    u32 stride=mRowStride;
    u32 source=gabi::ea(static_cast<be<u32>*>(mPixels))+(y*stride+x)*bytes;
    u32 bitOffset=0;
    for(s32 channel=0;channel<4;++channel) {
        s32 channels=vertexFormatChannels(mFormat);
        if(channels<=channel) { output->mValues[channel]=0;continue; }
        u32 mapped=vertexFormatChannelOrder(mFormat,channel);
        u32 width=vertexFormatChannelBits(mFormat,mapped);
        be<u32>* destination=&output->mValues[mapped<3 ? mapped : 3];
        u32 word=*gabi::at<be<u32>>(source);
        u32 conversion=sPixelWordConversion;
        word=gabi::call_ptr<u32>(conversion,word);
        u32 shifted=shiftPixelLeft(word,bitOffset);
        u32 right=32-width;
        bitOffset+=width;
        *destination=shiftPixelRight(shifted,right);
        if(bitOffset==32) { source+=4;bitOffset=0; }
    }
}
/* 027BF3FC */
void agl::PixelBufferTransfer::prepare(const CaptureSurfacePrefix_l* source,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    release();
    gabi::FrameLocal<TransferSurfaceStorage_l> temporary(0xC);
    createTransferSurface(temporary);
    u32 format=source->mFormat;
    if(format>=0x2F && format<=0x38) format=sPixelFormatRemapping.mFormats[format-0x2F];
    mFormat=format;
    s32 width=source->mSurface.mWidth,height=source->mSurface.mHeight;
    if(width<static_cast<s32>(source->mMinimumWidth)) width=source->mMinimumWidth;
    if(height<static_cast<s32>(source->mMinimumHeight)) height=source->mMinimumHeight;
    configureTransferSurface(temporary,1,format,width,height,1,1,0,0);
    gabi::FrameLocal<sead::GraphicsSurface> layout(0x9C);
    const auto* input=reinterpret_cast<const be<u32>*>(&temporary->mPrefix.mSurface);
    auto* output=reinterpret_cast<be<u32>*>(static_cast<sead::GraphicsSurface*>(layout));
    for(u32 i=0;i<29;++i) output[i]=input[i];
    layout->mTileMode=1;
    copyTransferSurfaceLayout(temporary,layout);
    mRowStride=temporary->mPrefix.mSurface.mPitch;
    if(!heap) heap=captureWorkHeap();
    u32 bytes=temporary->mPrefix.mSurface.mImageBytes;
    u32 allocate=heap->dispatch()->mAllocate.mFunction;
    void* pixels=gabi::call_ptr<void*>(allocate,heap,bytes,0x2000);
    mPixels=static_cast<be<u32>*>(pixels);
    invalidateCaptureMemory(pixels,bytes);
    temporary->mPrefix.mSurface.mImage=static_cast<void*>(mPixels);
    assignTransferColor(&mColorBuffer,temporary);
}
/* 027BF598 */
void agl::PixelBufferTransfer::copy(const CaptureSurfacePrefix_l* source) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<TransferTargetStorage_l> target(0x20);
    createTransferTarget(target);
    s32 width=source->mSurface.mWidth,height=source->mSurface.mHeight;
    if(width<static_cast<s32>(source->mMinimumWidth)) width=source->mMinimumWidth;
    if(height<static_cast<s32>(source->mMinimumHeight)) height=source->mMinimumHeight;
    f32 w=static_cast<f32>(static_cast<u32>(width)),h=static_cast<f32>(static_cast<u32>(height));
    auto* area=reinterpret_cast<sead::RenderSurface*>(&target->mFrame);
    area->mHeight=h;area->mWidth=w;
    f32 left=w>0.0f ? 0.0f:w,right=w>0.0f ? w:0.0f;
    f32 bottom=h>0.0f ? 0.0f:h,top=h>0.0f ? h:0.0f;
    area->mTop=top;area->mRight=right;area->mBottom=bottom;
    target->mFrame.mColor=reinterpret_cast<sead::ColorBuffer_l*>(&mColorBuffer);
    area->mLeft=left;
    activateTransferTarget(&target->mFrame);
    gabi::FrameLocal<sead::Viewport> viewport(0x60);
    sead::Viewport::createSurface(viewport,reinterpret_cast<const sead::RenderVector2*>(&target->mFrame));
    viewport->apply(area);
    gabi::FrameLocal<sead::CompactRenderState> state(0x78);
    sead::CompactRenderState::create(state);
    state->mDepthWrite=0;state->mBlendEnable=0;state->mDepthTest=0;state->apply();
    gabi::FrameLocal<TransferQuadStorage_l> quad(0xEC);
    createTransferQuad(quad,source);quad->mEnabled=1;
    renderTransferQuad(quad,viewport,&sTransferQuadSize,&sTransferQuadOffset,3);
    finishTransferColor(&mColorBuffer);
    destroyTransferQuad(quad,2);destroyTransferTarget(target,2);
}
/* 027BFEF4 */
void agl::PixelBufferTransfer::finish() {}
/* WWHD glue
data sPixelTransferMethods 10169DAC
data sPixelFormatRemapping 10169DA0
data sTransferQuadSize 104A0920
data sTransferQuadOffset 104A0908
ext 027BE6B8 void createTransferSurface(TransferSurfaceStorage_l*)
ext 027BE930 void configureTransferSurface(TransferSurfaceStorage_l*,u32,u32,u32,u32,u32,u32,u32,u32)
ext 027BEA90 void copyTransferSurfaceLayout(TransferSurfaceStorage_l*,const sead::GraphicsSurface*)
ext 025C0928 void assignTransferColor(agl::TransferColorStorage_l*,const TransferSurfaceStorage_l*)
ext 027B95B4 void createTransferTarget(TransferTargetStorage_l*)
ext 027B9694 void destroyTransferTarget(TransferTargetStorage_l*,u32)
ext 027BE114 void createTransferQuad(TransferQuadStorage_l*,const agl::CaptureSurfacePrefix_l*)
ext 027BE2B0 void destroyTransferQuad(TransferQuadStorage_l*,u32)
ext 027A90AC void renderTransferQuad(TransferQuadStorage_l*,const sead::Viewport*,const sead::RenderVector2*,const sead::RenderVector2*,u32)
ext 027B6D64 void finishTransferColor(agl::TransferColorStorage_l*)
ext 0274D690 void activateTransferTarget(sead::DisplayFrame*)
ext 0274F5F0 sead::Viewport* sead::Viewport::createSurface(sead::Viewport*,const sead::RenderVector2*)
ext 0274F964 void sead::Viewport::apply(const sead::RenderSurface*) const
ext 0274FDBC sead::CompactRenderState* sead::CompactRenderState::create(sead::CompactRenderState*)
ext 0274FEB0 void sead::CompactRenderState::apply()
data sPixelWordConversion 10147624
ext 027B6CB8 void initializePixelColorBuffer(agl::TransferColorStorage_l*)
*/

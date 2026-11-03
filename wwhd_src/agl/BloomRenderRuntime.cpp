// Bloom pass state, register payloads and bounded program-selection records.
#include "agl/BloomParameters.h"
#include "agl/EnvironmentLights.h"
#include "agl/ShaderCommandState.h"
#include "agl/ShaderHolder.h"
#include "agl/ShaderProgramActivation.h"
using namespace agl::environment;
// The bloom programs are agl::ShaderProgram (0x84 bytes) of the shader holder (sShaderHolder, defined once in
// agl/TimingStatics.cpp); a program's variant domain and shader mode are its ShaderProgramSharedData.
struct BloomRenderOwner {
    u8 mBeforeMask[0x10];be<u32> mMask;u8 mBeforeEnable[0x280];be<u8> mEnabled;u8 mBeforeKernel[0xF];
    be<f32> mKernelScale;u8 mBeforeColor[0x18];be<f32> mColor[3];
    be<u32> mRuntimeCount;gptr<void> mRuntimes;be<u32> mConfigurationCount;gptr<gptr<BloomColorWeights>> mConfigurations;
};
struct BloomSampleControls {agl::env::EnvironmentBooleanParameter mEnabled;agl::env::EnvironmentScalarParameter mNear,mFar,mGain,mLimit;};
struct BloomRectangle {be<f32> mWidth,mHeight,mMinX,mMinY,mMaxX,mMaxY;};
struct BloomRenderRuntime {
    u8 mBeforeWidth[0x338];be<s32> mWidth,mHeight;u8 mBeforeByteWidth[0x68];be<u8> mByteWidth,mByteHeight;
    u8 mBeforeRectangle[0x2B6];BloomRectangle mRectangle;u8 mBeforeWeight[0x5F4];be<f32> mWeight,mNear,mFar;
};
struct BloomGraphicsState {
    be<u8> mFirst,mSecond;u8 mBeforeFlags[0xA];be<u32> mFlags;u8 mBeforeColorMask[0xDC];
    be<u32> mColorMask;u8 mTail[0x2C];
};
struct BloomFloat4 {be<f32> mValues[4];};
struct BloomFloat2 {be<f32> x,y;};
struct BloomViewportState {u8 mBytes[0x18];};
void initializeBloomGraphics(BloomGraphicsState*);
void applyBloomGraphics(BloomGraphicsState*);
void prepareBloomProgram(agl::ShaderProgram*,u32);
void selectBloomShaderMode(u32);
void callBloomDisplayList(const void*,u32);
void activateBloomProgram(agl::ShaderProgram*);
void computeBloomThreshold(const void*,BloomFloat4*,f64);
void setBloomVertexRegisters(s32,u32,const void*);
void setBloomPixelRegisters(s32,u32,const void*);
void bindBloomPassTexture(void*,void*,s32,u32);
void prepareBloomRectangle(BloomRectangle*);
void initializeBloomViewport(BloomViewportState*,BloomRectangle*);
void applyBloomViewport(BloomViewportState*,BloomRectangle*);
void drawBloomSurface(void*,void*,void*);
void releaseBloomRenderTexture(void*);
template <class T> static T* bloomPointer(u32 address) {
    return gabi::at<T>(address);
}
// Bounded element lookups (an index past the table reads the first element). Built from the guest address, as
// the original does, because the table pointer may be 0.
static agl::ShaderVariantRadix* bloomRadix(agl::ShaderVariantDomain* domain,u32 index) {
    u32 address=gabi::ea(domain->radices.get());
    if((u32)domain->count>index) address+=index*sizeof(agl::ShaderVariantRadix);
    return bloomPointer<agl::ShaderVariantRadix>(address);
}
static void uploadBloomRegisters(const agl::ShaderProgram* program,u32 index,const BloomFloat4* payload) {
    const UniformLocations_l* uniform=programUniform(program,index);
    const s32 vertex=uniform->mVertexRegister;
    if(vertex!=-1) setBloomVertexRegisters(vertex,4,payload);
    const s32 pixel=uniform->mPixelRegister;
    if(pixel!=-1) setBloomPixelRegisters(pixel,4,payload);
}
static agl::ProgramResource16_l* bloomSampler(const agl::ShaderProgram* program,u32 index) {
    u32 address=gabi::ea(program->samplers.get());
    if((u32)program->samplerCount>index) address+=index*sizeof(agl::ProgramResource16_l);
    return bloomPointer<agl::ProgramResource16_l>(address);
}
static f32 bloomDimension(s32 size,u8 minimum) {
    return (f32)(u32)(size>(s32)minimum?size:(s32)minimum);
}
/* 02775C3C */
u32 renderBloomPass(BloomRenderOwner* owner,u32 index,u32 skipBase,u32 extra,u32 currentMode) {
    auto frame=gabi::AutoFrame(gabi::current_function());
    const u32 masked=index&(u32)owner->mMask;
    u32 configurationSlot=gabi::ea(owner->mConfigurations.get());
    if(masked<(u32)owner->mConfigurationCount) configurationSlot+=masked*4;
    auto* configuration=bloomPointer<gptr<BloomColorWeights>>(configurationSlot)->get();
    const u32 config=gabi::ea(configuration);
    auto* first=bloomPointer<BloomSampleControls>(config+0xB8);
    auto* second=bloomPointer<BloomSampleControls>(config+0x108);
    auto* third=bloomPointer<BloomSampleControls>(config+0x158);
    u32 runtimeAddress=gabi::ea(owner->mRuntimes.get());
    if(index<(u32)owner->mRuntimeCount) runtimeAddress+=index*0xC98;
    auto* runtime=bloomPointer<BloomRenderRuntime>(runtimeAddress);
    gabi::FrameLocal<be<u64>> conversion(8);
    *conversion.get()=0x4330000000000000ull|(u32)(runtime->mWidth>(s32)(u8)runtime->mByteWidth?(s32)runtime->mWidth:(u8)runtime->mByteWidth);
    const f32 inverseWidth=gabi::fdivs_ppc(1.0f,bloomDimension(runtime->mWidth,runtime->mByteWidth));
    *conversion.get()=0x4330000000000000ull|(u32)(runtime->mHeight>(s32)(u8)runtime->mByteHeight?(s32)runtime->mHeight:(u8)runtime->mByteHeight);
    const f32 inverseHeight=gabi::fdivs_ppc(1.0f,bloomDimension(runtime->mHeight,runtime->mByteHeight));
    gabi::FrameLocal<BloomGraphicsState> graphics(0xB0);
    initializeBloomGraphics(graphics.get());graphics->mFirst=0;graphics->mColorMask=15;graphics->mSecond=0;graphics->mFlags=0;
    const u32 colorFlag=*bloomPointer<be<u32>>(config+0x244);
    graphics->mColorMask=colorFlag!=0?255:15;applyBloomGraphics(graphics.get());
    agl::ShaderProgram* program=sShaderHolder->program(agl::detail::ShaderHolder::cShader_bloom_mask);
    agl::ShaderVariantDomain* domain=&programSharedData(program)->variation;
    u32 variant=0;
    if(skipBase==0) variant=domain->radices->divisor;
    if((u8)*bloomPointer<be<u8>>(config+0x264)!=0) variant+=(u16)bloomRadix(domain,1)->divisor;
    if((u8)first->mEnabled.mValue!=0) variant+=(u16)bloomRadix(domain,2)->divisor;
    if((u8)second->mEnabled.mValue!=0) variant+=(u16)bloomRadix(domain,3)->divisor;
    if(extra!=0) variant+=(u16)bloomRadix(domain,4)->divisor;
    if((u32)*bloomPointer<be<u32>>(config+0x244)!=0) variant+=(u16)bloomRadix(domain,5)->divisor;
    if((u8)third->mEnabled.mValue!=0) variant+=(u16)bloomRadix(domain,6)->divisor;
    if((u8)owner->mEnabled!=0) variant+=(u16)bloomRadix(domain,7)->divisor;
    if((s32)variant<=0) program=domain->root;
    else {
        u32 address=gabi::ea(domain->programs.get());
        if(variant-1<(u32)domain->programCount) address+=(variant-1)*sizeof(agl::ShaderProgram);
        program=bloomPointer<agl::ShaderProgram>(address);
    }
    if(((u8)program->flags&ShaderProgramFlag_UpdatePending)!=0) {
        program->flags=(u8)program->flags&~(u32)ShaderProgramFlag_UpdatePending;prepareBloomProgram(program,0);
    }
    const u32 mode=programSharedData(program)->mode;
    if(currentMode!=mode) selectBloomShaderMode(mode);
    const agl::DisplayListStorage* displayList=programDisplayList(program);
    const u32 commandBytes=displayList->mUsed;
    if(commandBytes!=0) callBloomDisplayList(displayList->mData.get(),commandBytes);
    else activateBloomProgram(program);
    currentMode=mode;
    gabi::FrameLocal<BloomFloat4> threshold(0x88),color(0x28),kernel(0x38),ranges(0x48),gains(0x58),depth(0x68),thirdRange(0x78);
    computeBloomThreshold(configuration,threshold.get(),gabi::fpr_from_f32(runtime->mWeight));uploadBloomRegisters(program,1,threshold.get());
    const f32 thresholdScale=threshold->mValues[3];
    color->mValues[0]=gabi::fmuls_ppc(owner->mColor[0],thresholdScale);
    color->mValues[1]=gabi::fmuls_ppc(owner->mColor[1],thresholdScale);
    color->mValues[3]=(f32)threshold->mValues[0];color->mValues[2]=gabi::fmuls_ppc(owner->mColor[2],thresholdScale);
    uploadBloomRegisters(program,0,color.get());
    bindBloomPassTexture(bloomPointer<void>(runtimeAddress+0x4C8),bloomSampler(program,2),-1,0);
    kernel->mValues[1]=0.0f;kernel->mValues[0]=gabi::fadds_ppc(owner->mKernelScale,owner->mKernelScale);
    kernel->mValues[2]=0.0f;kernel->mValues[3]=0.0f;uploadBloomRegisters(program,7,kernel.get());
    computeBloomThreshold(bloomPointer<void>(config+0x5C),threshold.get(),gabi::fpr_from_f32(runtime->mWeight));uploadBloomRegisters(program,8,threshold.get());
    bindBloomPassTexture(bloomPointer<void>(runtimeAddress+0x198),bloomSampler(program,1),-1,0);
    const f32 distance=gabi::fsubs_ppc(runtime->mFar,runtime->mNear);
    const f32 firstSpan=gabi::fsubs_ppc(first->mFar.mValue,first->mNear.mValue);
    const f32 inverseDistance=gabi::fdivs_ppc(1.0f,distance);
    const f32 thirdSpan=gabi::fsubs_ppc(third->mFar.mValue,third->mNear.mValue);
    const f32 firstScale=gabi::fdivs_ppc(distance,firstSpan);
    const f32 thirdScale=gabi::fdivs_ppc(distance,thirdSpan);
    const f32 secondStart=second->mNear.mValue;
    const f32 secondSpan=gabi::fsubs_ppc(second->mFar.mValue,secondStart);
    const f32 secondScale=gabi::fdivs_ppc(distance,secondSpan);
    const f32 secondOffset=-gabi::fmuls_ppc(gabi::fmuls_ppc(secondStart,secondScale),inverseDistance);
    const f32 firstOffset=-gabi::fmuls_ppc(gabi::fmuls_ppc(first->mNear.mValue,firstScale),inverseDistance);
    ranges->mValues[0]=firstScale;ranges->mValues[2]=secondScale;ranges->mValues[3]=secondOffset;ranges->mValues[1]=firstOffset;
    gains->mValues[0]=(f32)first->mLimit.mValue;gains->mValues[1]=(f32)first->mGain.mValue;
    gains->mValues[2]=(f32)second->mGain.mValue;gains->mValues[3]=0.0f;
    const f32 nearDistance=runtime->mNear,farDistance=runtime->mFar;
    depth->mValues[3]=inverseHeight;depth->mValues[2]=inverseWidth;
    depth->mValues[0]=gabi::fsubs_ppc(1.0f,gabi::fdivs_ppc(nearDistance,farDistance));
    depth->mValues[1]=gabi::fmuls_ppc(nearDistance,inverseDistance);
    thirdRange->mValues[0]=thirdScale;thirdRange->mValues[3]=(f32)third->mGain.mValue;
    thirdRange->mValues[2]=(f32)third->mLimit.mValue;
    thirdRange->mValues[1]=-gabi::fmuls_ppc(gabi::fmuls_ppc(third->mNear.mValue,thirdScale),inverseDistance);
    uploadBloomRegisters(program,2,ranges.get());uploadBloomRegisters(program,3,gains.get());
    uploadBloomRegisters(program,4,depth.get());uploadBloomRegisters(program,9,thirdRange.get());
    const u32 outputAddress=runtimeAddress+0x83C;
    const s32 outputWidth=*bloomPointer<be<s32>>(outputAddress+8),outputHeight=*bloomPointer<be<s32>>(outputAddress+0xC);
    const u8 byteWidth=*bloomPointer<be<u8>>(outputAddress+0x78),byteHeight=*bloomPointer<be<u8>>(outputAddress+0x79);
    const f32 width=bloomDimension(outputWidth,byteWidth),height=bloomDimension(outputHeight,byteHeight);
    *conversion.get()=0x4330000000000000ull|(u32)(outputWidth>(s32)byteWidth?outputWidth:byteWidth);
    gabi::FrameLocal<BloomFloat2> dimensions(0x20);dimensions->x=width;
    *conversion.get()=0x4330000000000000ull|(u32)(outputHeight>(s32)byteHeight?outputHeight:byteHeight);dimensions->y=height;
    runtime->mRectangle.mWidth=width;runtime->mRectangle.mHeight=height;
    gabi::FrameLocal<BloomFloat4> bounds(0x10);
    bounds->mValues[0]=0.0f<width?0.0f:width;bounds->mValues[2]=0.0f<width?width:0.0f;
    bounds->mValues[1]=0.0f<height?0.0f:height;bounds->mValues[3]=0.0f<height?height:0.0f;
    runtime->mRectangle.mMinX=(f32)bounds->mValues[0];runtime->mRectangle.mMinY=(f32)bounds->mValues[1];
    runtime->mRectangle.mMaxX=(f32)bounds->mValues[2];runtime->mRectangle.mMaxY=(f32)bounds->mValues[3];
    prepareBloomRectangle(&runtime->mRectangle);
    gabi::FrameLocal<BloomViewportState> viewport(0x98);
    initializeBloomViewport(viewport.get(),&runtime->mRectangle);applyBloomViewport(viewport.get(),&runtime->mRectangle);
    drawBloomSurface(owner,program,bloomPointer<void>(runtimeAddress+0x330));
    for(u32 child=0;child<2;++child) releaseBloomRenderTexture(bloomPointer<void>(runtimeAddress+0x83C+child*0x2E4));
    return currentMode;
}
static_assert(sizeof(agl::ShaderProgram)==0x84);
static_assert(offsetof(agl::ShaderProgram,binary)==0x7C);
static_assert(offsetof(agl::ShaderProgramSharedData,mode)==0x28);
static_assert(sizeof(agl::ShaderVariantRadix)==0x1C);
static_assert(offsetof(BloomRenderOwner,mConfigurations)==0x2D8);
static_assert(offsetof(BloomRenderRuntime,mByteWidth)==0x3A8);
static_assert(offsetof(BloomRenderRuntime,mRectangle)==0x660);
static_assert(offsetof(BloomRenderRuntime,mWeight)==0xC6C);
static_assert(sizeof(BloomGraphicsState)==0x11C);
static_assert(sizeof(BloomSampleControls)==0x50);
/* WWHD glue
ext 02750250 void initializeBloomGraphics(BloomGraphicsState*)
ext 02750370 void applyBloomGraphics(BloomGraphicsState*)
ext 027BB9E0 void prepareBloomProgram(agl::ShaderProgram*,u32)
ext 027B9F68 void selectBloomShaderMode(u32)
ext C00060E0 void callBloomDisplayList(const void*,u32)
ext 027BB7CC void activateBloomProgram(agl::ShaderProgram*)
ext 02775930 void computeBloomThreshold(const void*,BloomFloat4*,f64)
ext C0006A40 void setBloomVertexRegisters(s32,u32,const void*)
ext C0006908 void setBloomPixelRegisters(s32,u32,const void*)
ext 027BE53C void bindBloomPassTexture(void*,void*,s32,u32)
ext 0274D690 void prepareBloomRectangle(BloomRectangle*)
ext 0274F5F0 void initializeBloomViewport(BloomViewportState*,BloomRectangle*)
ext 0274F964 void applyBloomViewport(BloomViewportState*,BloomRectangle*)
ext 02775BDC void drawBloomSurface(void*,void*,void*)
ext 027B6D64 void releaseBloomRenderTexture(void*)
*/

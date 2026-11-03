// Render-slot update caller; opaque producer contracts are intentionally not expanded here.
#include "JSystem/J3DGraph/PipelineRenderState.h"
#include <bit>
using namespace j3d::pipeline;
namespace j3d::pipeline {
struct SlotUpdateMethods_l { u8 mPrefix[0xC]; gfn mValid; };
struct SlotUpdateBuffer_l {
    Scalar mWidth,mHeight,mBaseWidth,mBaseHeight,mViewWidth,mViewHeight;
    gptr<SlotUpdateMethods_l> mMethods; gptr<void> mSurface;
    u8 mState[0x1C]; be<u32> mWord3C;
};
struct SlotUpdateOwner_l { u8 mPrefix[8]; gptr<SlotUpdateBuffer_l> mBuffer; u8 mState[0x10]; gptr<void> mBinding; };
struct SlotUpdateSource_l { u8 mStorage[0x18]; };
struct SlotUpdateSurface_l { u8 mPrefix[0x91]; be<u8> mFlag; u8 mTail[0x106]; }; /* 0x198: the original ctor 027BE114 allocates 0x198 (li r3,0x198 at 027BE138) and stores up to +0x194 */
struct SlotUpdateResult_l { be<u32> mValue,mMode; };
struct SlotUpdateHandle_l { u8 mPrefix[8]; be<s32> mWidth,mHeight; u8 mState[0x68]; be<u8> mMinimumWidth,mMinimumHeight; };
struct SlotUpdateRectangle_l { Scalar mBaseWidth,mBaseHeight,mWidth,mHeight; };
struct SlotUpdateConversion_l { be<u32> mHigh,mLow; };

WWHD_OFFSET(j3d::pipeline::SlotUpdateBuffer_l,mMethods,0x18);
WWHD_OFFSET(j3d::pipeline::SlotUpdateBuffer_l,mSurface,0x1C);
WWHD_OFFSET(j3d::pipeline::SlotUpdateBuffer_l,mWord3C,0x3C);
WWHD_SIZE(j3d::pipeline::SlotUpdateBuffer_l,0x40);
static_assert(sizeof(SlotUpdateBuffer_l)==sizeof(RenderBufferStorage_l));
WWHD_OFFSET(j3d::pipeline::SlotUpdateSurface_l,mFlag,0x91);
WWHD_SIZE(j3d::pipeline::SlotUpdateSurface_l,0x198);
}
using SlotUpdateFlagWord_l=be<u32>;
using SlotUpdateMethodPointer_l=gptr<void>;
using SlotUpdateDouble_l=be<f64>;
static SlotUpdateFlagWord_l sSlotUpdateInitialized;
static SlotUpdateMethodPointer_l sSlotUpdateInstalledMethods;
static u8 sSlotUpdateMethods;
static u8 sSlotUpdateGlobal;
static PipelineSlotDimensions_l sSlotUpdateBase;
static PipelineSlotDimensions_l sSlotUpdateScale;
static Scalar sSlotUpdateGamma;
static Scalar sSlotUpdateZero;
static SlotUpdateDouble_l sSlotUpdateIntegerBias;
void prepareSlotUpdateSurface(void*);
void synchronizeSlotUpdateSurface(void*);
void initializeSlotUpdateSource(SlotUpdateSource_l*,SlotUpdateBuffer_l*);
void bindSlotUpdateSource(void*,SlotUpdateBuffer_l*);
void applySlotUpdateState(sead::CompactRenderState*);
void initializeSlotUpdateSurface(SlotUpdateSurface_l*,void*);
void prepareSlotUpdateBuffer(SlotUpdateBuffer_l*);
void bindSlotUpdateHandle(void*,u32);
void activateSlotUpdateBuffer(void*);
SlotUpdateResult_l* currentSlotUpdateResult(void*);
u32 drawSlotUpdateGamma(SlotUpdateSurface_l*,u32,f64);
u32 drawSlotUpdateSurface(SlotUpdateSurface_l*,SlotUpdateSource_l*,const PipelineSlotDimensions_l*,const PipelineSlotDimensions_l*,u32);
void destroySlotUpdateSurface(SlotUpdateSurface_l*,u32);
namespace j3d::pipeline {
/* 027F84A4 */
u32 PipelineRenderState::updateSlot(u32 index,void* object) {
    gabi::AutoFrame frame{0x027F84A4u};
    gabi::FrameLocal<PipelineSlotDimensions_l> dimensions(8);
    gabi::FrameLocal<SlotUpdateSource_l> source(0x10);
    gabi::FrameLocal<SlotUpdateSurface_l> surface(0x28);
    SlotUpdateOwner_l* owner=static_cast<SlotUpdateOwner_l*>(object);
    SlotUpdateBuffer_l* input=owner->mBuffer;
    if(sSlotUpdateInitialized==0) {
        sSlotUpdateInitialized=1;
        sSlotUpdateInstalledMethods=&sSlotUpdateMethods;
    }
    if(input!=nullptr) {
        SlotUpdateMethods_l* methods=input->mMethods;
        if(gabi::call_ptr<u32>(methods->mValid.get(),input,&sSlotUpdateInstalledMethods)==0) input=nullptr;
    }
    dimensions->mWidth.bits=static_cast<u32>(input->mViewWidth.bits);
    dimensions->mHeight.bits=static_cast<u32>(input->mViewHeight.bits);
    PipelineSlotTable* slots=reinterpret_cast<PipelineSlotTable*>(this);
    u32 handle=slots->createWord(index,dimensions.get());
    prepareSlotUpdateSurface(input->mSurface.get());
    synchronizeSlotUpdateSurface(input->mSurface.get());
    PipelineSlot_l* slot=&mSlots[index<19?index:0];
    SlotUpdateBuffer_l* buffer=static_cast<SlotUpdateBuffer_l*>(slot->mFirstStorage.get());
    void* second=slot->mSecondStorage;
    initializeSlotUpdateSource(source.get(),buffer);
    bindSlotUpdateSource(source.get(),buffer);
    applySlotUpdateState(&mRenderState);
    initializeSlotUpdateSurface(surface.get(),input->mSurface.get());
    surface->mFlag=1;
    prepareSlotUpdateBuffer(buffer);
    buffer->mWord3C=0;
    bindSlotUpdateHandle(second,handle);
    buffer->mSurface=second;
    activateSlotUpdateBuffer(buffer);
    SlotUpdateResult_l* result=currentSlotUpdateResult(&sSlotUpdateGlobal);
    u32 previous=result->mValue;
    u32 value;
    if(index==17 || index==18) {
        f64 gamma=ppc_lfs_value(sSlotUpdateGamma.bits);
        value=drawSlotUpdateGamma(surface.get(),previous,gamma);
    } else value=drawSlotUpdateSurface(surface.get(),source.get(),&sSlotUpdateScale,&sSlotUpdateBase,previous);
    prepareSlotUpdateSurface(buffer->mSurface.get());
    synchronizeSlotUpdateSurface(buffer->mSurface.get());
    result=currentSlotUpdateResult(&sSlotUpdateGlobal);
    result->mValue=value;
    result->mMode=0;
    activateSlotUpdateBuffer(owner->mBuffer.get());
    bindSlotUpdateSource(owner->mBinding.get(),owner->mBuffer.get());
    destroySlotUpdateSurface(surface.get(),2);
    return handle;
}
/* 027F869C */
u32 PipelineRenderState::updateScaledSlot(u32 index,void* object) {
    gabi::AutoFrame frame{0x027F869Cu};
    gabi::FrameLocal<PipelineSlotDimensions_l> scale(8),dimensions(0x18);
    gabi::FrameLocal<SlotUpdateConversion_l> conversion(0x10);
    gabi::FrameLocal<SlotUpdateSource_l> source(0x20);
    gabi::FrameLocal<SlotUpdateSurface_l> surface(0x38);
    SlotUpdateOwner_l* owner=static_cast<SlotUpdateOwner_l*>(object);
    SlotUpdateBuffer_l* input=owner->mBuffer;
    if(sSlotUpdateInitialized==0) {
        sSlotUpdateInitialized=1;sSlotUpdateInstalledMethods=&sSlotUpdateMethods;
    }
    if(input!=nullptr) {
        SlotUpdateMethods_l* methods=input->mMethods;
        if(gabi::call_ptr<u32>(methods->mValid.get(),input,&sSlotUpdateInstalledMethods)==0) input=nullptr;
    }
    dimensions->mWidth.bits=static_cast<u32>(input->mViewWidth.bits);
    dimensions->mHeight.bits=static_cast<u32>(input->mViewHeight.bits);
    PipelineSlotTable* slots=reinterpret_cast<PipelineSlotTable*>(this);
    u32 handle=slots->word(index);
    prepareSlotUpdateSurface(input->mSurface.get());
    synchronizeSlotUpdateSurface(input->mSurface.get());
    PipelineSlot_l* slot=&mSlots[index<19?index:0];
    SlotUpdateBuffer_l* buffer=static_cast<SlotUpdateBuffer_l*>(slot->mFirstStorage.get());
    void* second=slot->mSecondStorage;
    initializeSlotUpdateSource(source.get(),buffer);
    bindSlotUpdateSource(source.get(),buffer);
    applySlotUpdateState(&mRenderState);
    initializeSlotUpdateSurface(surface.get(),input->mSurface.get());
    surface->mFlag=1;
    prepareSlotUpdateBuffer(buffer);buffer->mWord3C=0;
    bindSlotUpdateHandle(second,handle);buffer->mSurface=second;
    activateSlotUpdateBuffer(buffer);
    SlotUpdateHandle_l* image=gabi::at<SlotUpdateHandle_l>(handle);
    s32 width=image->mWidth;s32 minimumWidth=image->mMinimumWidth;
    if(width>minimumWidth) minimumWidth=width;
    f64 inputWidth=ppc_lfs_value(dimensions->mWidth.bits);
    conversion->mHigh=0x43300000;conversion->mLow=static_cast<u32>(minimumWidth);
    u64 encoded=(static_cast<u64>(conversion->mHigh)<<32)|static_cast<u32>(conversion->mLow);
    f64 bias=sSlotUpdateIntegerBias;
    f64 convertedWidth=static_cast<f32>(gabi::fsub_ppc(std::bit_cast<f64>(encoded),bias));
    s32 minimumHeight=image->mMinimumHeight;s32 height=image->mHeight;
    if(height>minimumHeight) minimumHeight=height;
    f64 ratioWidth=gabi::fdivs_fpr(convertedWidth,inputWidth);
    conversion->mLow=static_cast<u32>(minimumHeight);
    encoded=(static_cast<u64>(conversion->mHigh)<<32)|static_cast<u32>(conversion->mLow);
    f64 convertedHeight=static_cast<f32>(gabi::fsub_ppc(std::bit_cast<f64>(encoded),bias));
    f64 inputHeight=ppc_lfs_value(dimensions->mHeight.bits);
    f64 ratioHeight=gabi::fdivs_fpr(convertedHeight,inputHeight);
    scale->mWidth.bits=ppc_stfs_bits(ratioWidth);scale->mHeight.bits=ppc_stfs_bits(ratioHeight);
    SlotUpdateResult_l* result=currentSlotUpdateResult(&sSlotUpdateGlobal);
    u32 previous=result->mValue;
    u32 value=drawSlotUpdateSurface(surface.get(),source.get(),scale.get(),&sSlotUpdateBase,previous);
    prepareSlotUpdateSurface(buffer->mSurface.get());synchronizeSlotUpdateSurface(buffer->mSurface.get());
    result=currentSlotUpdateResult(&sSlotUpdateGlobal);result->mValue=value;result->mMode=0;
    activateSlotUpdateBuffer(owner->mBuffer.get());
    bindSlotUpdateSource(owner->mBinding.get(),owner->mBuffer.get());
    destroySlotUpdateSurface(surface.get(),2);
    return handle;
}
/* 027F8974 */
void PipelineRenderState::copySlot(u32 index,void* object,u32 applyState) {
    gabi::AutoFrame frame{0x027F8974u};
    gabi::FrameLocal<PipelineSlotDimensions_l> scale(8);
    gabi::FrameLocal<SlotUpdateConversion_l> conversion(0x10);
    gabi::FrameLocal<SlotUpdateSource_l> source(0x18);
    gabi::FrameLocal<SlotUpdateSurface_l> surface(0x30);
    PipelineSlotTable* slots=reinterpret_cast<PipelineSlotTable*>(this);
    u32 handle=slots->word(index);
    if(handle==0) return;
    SlotUpdateOwner_l* owner=static_cast<SlotUpdateOwner_l*>(object);
    SlotUpdateBuffer_l* input=owner->mBuffer;
    if(sSlotUpdateInitialized==0) {
        sSlotUpdateInitialized=1;sSlotUpdateInstalledMethods=&sSlotUpdateMethods;
    }
    if(input!=nullptr) {
        SlotUpdateMethods_l* methods=input->mMethods;
        if(gabi::call_ptr<u32>(methods->mValid.get(),input,&sSlotUpdateInstalledMethods)==0) input=nullptr;
    }
    prepareSlotUpdateSurface(input->mSurface.get());synchronizeSlotUpdateSurface(input->mSurface.get());
    initializeSlotUpdateSurface(surface.get(),gabi::at<void>(handle));surface->mFlag=1;
    initializeSlotUpdateSource(source.get(),input);bindSlotUpdateSource(source.get(),input);
    if(applyState!=0) applySlotUpdateState(&mRenderState);
    SlotUpdateHandle_l* image=gabi::at<SlotUpdateHandle_l>(handle);
    s32 width=image->mWidth;s32 minimumWidth=image->mMinimumWidth;
    f64 inputWidth=ppc_lfs_value(input->mWidth.bits);
    if(width>minimumWidth) minimumWidth=width;
    conversion->mLow=static_cast<u32>(minimumWidth);conversion->mHigh=0x43300000;
    u64 encoded=(static_cast<u64>(conversion->mHigh)<<32)|static_cast<u32>(conversion->mLow);
    f64 bias=sSlotUpdateIntegerBias;
    s32 height=image->mHeight;s32 minimumHeight=image->mMinimumHeight;
    f64 convertedWidth=static_cast<f32>(gabi::fsub_ppc(std::bit_cast<f64>(encoded),bias));
    if(minimumHeight>height) height=minimumHeight;
    conversion->mLow=static_cast<u32>(height);
    f64 ratioWidth=gabi::fdivs_fpr(inputWidth,convertedWidth);
    encoded=(static_cast<u64>(conversion->mHigh)<<32)|static_cast<u32>(conversion->mLow);
    f64 convertedHeight=static_cast<f32>(gabi::fsub_ppc(std::bit_cast<f64>(encoded),bias));
    f64 inputHeight=ppc_lfs_value(input->mHeight.bits);
    f64 ratioHeight=gabi::fdivs_fpr(inputHeight,convertedHeight);
    scale->mWidth.bits=ppc_stfs_bits(ratioWidth);scale->mHeight.bits=ppc_stfs_bits(ratioHeight);
    SlotUpdateResult_l* result=currentSlotUpdateResult(&sSlotUpdateGlobal);
    u32 previous=result->mValue;
    u32 value=drawSlotUpdateSurface(surface.get(),source.get(),scale.get(),&sSlotUpdateBase,previous);
    prepareSlotUpdateSurface(input->mSurface.get());synchronizeSlotUpdateSurface(input->mSurface.get());
    result=currentSlotUpdateResult(&sSlotUpdateGlobal);result->mValue=value;result->mMode=0;
    destroySlotUpdateSurface(surface.get(),2);
}

/* 027F8B20 */
void PipelineRenderState::copyBetweenSlots(u32 destination,u32 sourceIndex) {
    gabi::AutoFrame frame{0x027F8B20u};
    gabi::FrameLocal<SlotUpdateConversion_l> conversion(8);
    gabi::FrameLocal<SlotUpdateRectangle_l> rectangle(0x10);
    gabi::FrameLocal<PipelineSlotDimensions_l> dimensions(0x20),scale(0x28);
    gabi::FrameLocal<SlotUpdateSource_l> source(0x30);
    gabi::FrameLocal<SlotUpdateSurface_l> surface(0x48);
    PipelineSlotTable* slots=reinterpret_cast<PipelineSlotTable*>(this);
    u32 destinationHandle=slots->word(destination);
    u32 sourceHandle=slots->word(sourceIndex);
    if(destinationHandle==0 || sourceHandle==0) {
        __atomic_thread_fence(__ATOMIC_ACQUIRE);return;
    }
    SlotUpdateHandle_l* image=gabi::at<SlotUpdateHandle_l>(sourceHandle);
    s32 width=image->mWidth;s32 minimumWidth=image->mMinimumWidth;
    s32 height=image->mHeight;
    if(minimumWidth>width) width=minimumWidth;
    s32 minimumHeight=image->mMinimumHeight;
    if(height>minimumHeight) minimumHeight=height;
    conversion->mLow=static_cast<u32>(width);
    f64 bias=sSlotUpdateIntegerBias;conversion->mHigh=0x43300000;
    u64 encoded=(static_cast<u64>(conversion->mHigh)<<32)|static_cast<u32>(conversion->mLow);
    conversion->mLow=static_cast<u32>(minimumHeight);
    u64 encodedHeight=(static_cast<u64>(conversion->mHigh)<<32)|static_cast<u32>(conversion->mLow);
    f64 savedWidth=static_cast<f32>(gabi::fsub_ppc(std::bit_cast<f64>(encoded),bias));
    f64 savedHeight=static_cast<f32>(gabi::fsub_ppc(std::bit_cast<f64>(encodedHeight),bias));
    dimensions->mWidth.bits=ppc_stfs_bits(savedWidth);dimensions->mHeight.bits=ppc_stfs_bits(savedHeight);
    SlotUpdateBuffer_l* buffer=reinterpret_cast<SlotUpdateBuffer_l*>(&mBuffer);
    prepareSlotUpdateBuffer(buffer);
    buffer->mWord3C=0;buffer->mSurface=&mSurface;
    bindSlotUpdateHandle(&mSurface,sourceHandle);
    buffer->mWidth.bits=ppc_stfs_bits(savedWidth);buffer->mHeight.bits=ppc_stfs_bits(savedHeight);
    f64 requestedWidth=ppc_lfs_value(dimensions->mWidth.bits);
    f64 zero=ppc_lfs_value(sSlotUpdateZero.bits);
    f64 requestedHeight=ppc_lfs_value(dimensions->mHeight.bits);
    if(zero<requestedWidth) {
        rectangle->mWidth.bits=ppc_stfs_bits(requestedWidth);
        rectangle->mBaseWidth.bits=ppc_stfs_bits(zero);
    } else {
        rectangle->mWidth.bits=ppc_stfs_bits(zero);
        rectangle->mBaseWidth.bits=ppc_stfs_bits(requestedWidth);
    }
    if(zero<requestedHeight) {
        rectangle->mBaseHeight.bits=ppc_stfs_bits(zero);
        rectangle->mHeight.bits=ppc_stfs_bits(requestedHeight);
    } else {
        rectangle->mBaseHeight.bits=ppc_stfs_bits(requestedHeight);
        rectangle->mHeight.bits=ppc_stfs_bits(zero);
    }
    buffer->mBaseWidth.bits=static_cast<u32>(rectangle->mBaseWidth.bits);
    buffer->mBaseHeight.bits=static_cast<u32>(rectangle->mBaseHeight.bits);
    buffer->mViewWidth.bits=static_cast<u32>(rectangle->mWidth.bits);
    buffer->mViewHeight.bits=static_cast<u32>(rectangle->mHeight.bits);
    activateSlotUpdateBuffer(buffer);
    initializeSlotUpdateSource(source.get(),buffer);bindSlotUpdateSource(source.get(),buffer);
    initializeSlotUpdateSurface(surface.get(),gabi::at<void>(destinationHandle));
    bias=sSlotUpdateIntegerBias;surface->mFlag=1;
    image=gabi::at<SlotUpdateHandle_l>(destinationHandle);
    width=image->mWidth;minimumWidth=image->mMinimumWidth;
    if(width>minimumWidth) minimumWidth=width;
    conversion->mLow=static_cast<u32>(minimumWidth);conversion->mHigh=0x43300000;
    encoded=(static_cast<u64>(conversion->mHigh)<<32)|static_cast<u32>(conversion->mLow);
    minimumHeight=image->mMinimumHeight;
    f64 convertedWidth=static_cast<f32>(gabi::fsub_ppc(std::bit_cast<f64>(encoded),bias));
    f64 actualWidth=ppc_lfs_value(buffer->mWidth.bits);height=image->mHeight;
    if(minimumHeight>height) height=minimumHeight;
    f64 ratioWidth=gabi::fdivs_fpr(actualWidth,convertedWidth);
    conversion->mLow=static_cast<u32>(height);
    encoded=(static_cast<u64>(conversion->mHigh)<<32)|static_cast<u32>(conversion->mLow);
    f64 convertedHeight=static_cast<f32>(gabi::fsub_ppc(std::bit_cast<f64>(encoded),bias));
    f64 actualHeight=ppc_lfs_value(buffer->mHeight.bits);
    f64 ratioHeight=gabi::fdivs_fpr(actualHeight,convertedHeight);
    scale->mWidth.bits=ppc_stfs_bits(ratioWidth);scale->mHeight.bits=ppc_stfs_bits(ratioHeight);
    applySlotUpdateState(&mRenderState);
    SlotUpdateResult_l* result=currentSlotUpdateResult(&sSlotUpdateGlobal);
    u32 previous=result->mValue;
    u32 value=drawSlotUpdateSurface(surface.get(),source.get(),scale.get(),&sSlotUpdateBase,previous);
    prepareSlotUpdateSurface(buffer->mSurface.get());synchronizeSlotUpdateSurface(buffer->mSurface.get());
    result=currentSlotUpdateResult(&sSlotUpdateGlobal);result->mValue=value;result->mMode=0;
    destroySlotUpdateSurface(surface.get(),2);
    __atomic_thread_fence(__ATOMIC_ACQUIRE);
}

}
/* WWHD glue
data sSlotUpdateInitialized 101FD7BC
data sSlotUpdateInstalledMethods 101FDCF8
data sSlotUpdateMethods 1016E9E0
data sSlotUpdateGlobal 104B45C0
data sSlotUpdateBase 104A0908
data sSlotUpdateScale 104A0920
data sSlotUpdateGamma 1016EA08
data sSlotUpdateIntegerBias 1016EA10
data sSlotUpdateZero 1016EA18
ext 027B6D64 void prepareSlotUpdateSurface(void*)
ext 027BE794 void synchronizeSlotUpdateSurface(void*)
ext 0274F5F0 void initializeSlotUpdateSource(j3d::pipeline::SlotUpdateSource_l*,j3d::pipeline::SlotUpdateBuffer_l*)
ext 0274F964 void bindSlotUpdateSource(void*,j3d::pipeline::SlotUpdateBuffer_l*)
ext 0274FEB0 void applySlotUpdateState(sead::CompactRenderState*)
ext 027BE114 void initializeSlotUpdateSurface(j3d::pipeline::SlotUpdateSurface_l*,void*)
ext 027B954C void prepareSlotUpdateBuffer(j3d::pipeline::SlotUpdateBuffer_l*)
ext 025C0928 void bindSlotUpdateHandle(void*,u32)
ext 0274D690 void activateSlotUpdateBuffer(void*)
ext 027F29D4 j3d::pipeline::SlotUpdateResult_l* currentSlotUpdateResult(void*)
ext 027A9B0C u32 drawSlotUpdateGamma(j3d::pipeline::SlotUpdateSurface_l*,u32,f64)
ext 027A90AC u32 drawSlotUpdateSurface(j3d::pipeline::SlotUpdateSurface_l*,j3d::pipeline::SlotUpdateSource_l*,const j3d::pipeline::PipelineSlotDimensions_l*,const j3d::pipeline::PipelineSlotDimensions_l*,u32)
ext 027BE2B0 void destroySlotUpdateSurface(j3d::pipeline::SlotUpdateSurface_l*,u32)
*/

// Frame update orchestration; offsets and call order derive from own 027F55FC instructions.
#include "JSystem/J3DGraph/ModelLighting.h"
#include "JSystem/J3DGraph/PipelineFrameQueue.h"
#include "JSystem/J3DGraph/PipelineRecordBindings.h"
#include "bindings.h"
using namespace j3d::pipeline;
static MatrixRows sFrameDefaultMatrix;
static PipelineFrameQueue* sFrameQueue;
void selectFrameBuffer(void*,u32,const MatrixRows*,u32);
void updateFrameColors(void*,MaterialPacket_l*);
void finishFrameObject(void*);
namespace j3d::pipeline {
namespace {
struct FrameCount_l { u8 mPrefix[6]; be<u8> mCount; };
struct FrameAnimatorMethods_l { u8 mPrefix[0x3C]; gfn mUpdate; };
struct FrameAnimator_l { u8 mPrefix[0x34]; gptr<FrameAnimatorMethods_l> mMethods; };
struct FrameData_l {
    u8 mPrefix[0x38];
    gptr<FrameAnimator_l> mFirst,mSecond,mLast,mThird,mFourth;
};
struct FramePacketHeader_l { u8 mPrefix[0x20]; be<u32> mStateOffset; };
struct FrameModel_l {
    u8 mPrefix[0x2C]; gptr<FrameCount_l> mCounts;
    u8 mBeforePages[0x30]; LightingPageArray_l mPages;
    u8 mBeforeAnimationFlag[0x10]; be<u8> mAnimationDisabled;
    u8 mBeforeFinishObject[0xB]; gptr<void> mFinishObject;
    be<u8> mFinishDisabled; u8 mBeforeJob[3]; sead::JobItem mJob;
};
static_assert(offsetof(FrameModel_l,mPages)==0x60);
static_assert(offsetof(FrameModel_l,mAnimationDisabled)==0x78);
static_assert(offsetof(FrameModel_l,mFinishObject)==0x84);
static_assert(offsetof(FrameModel_l,mFinishDisabled)==0x88);
static_assert(offsetof(FrameModel_l,mJob)==0x8C);
static_assert(offsetof(FrameData_l,mFirst)==0x38);
static_assert(offsetof(FrameData_l,mLast)==0x40);
static_assert(offsetof(FrameAnimator_l,mMethods)==0x34);
static_assert(offsetof(FrameAnimatorMethods_l,mUpdate)==0x3C);
}
/* 027F55FC */
void ModelPipeline_l::advanceFrame() {
    gabi::AutoFrame frame{0x027F55FCu};
    FrameModel_l* state=reinterpret_cast<FrameModel_l*>(this);
    FrameCount_l* counts=state->mCounts;
    s32 divisor=counts->mCount;
    s32 next=static_cast<s32>(mBufferIndex)+1;
    s32 quotient=divisor!=0 ? next/divisor : 0;
    u32 buffer=static_cast<u32>(next-quotient*divisor);
    mBufferIndex=buffer;
    selectFrameBuffer(mBuffer,0,&sFrameDefaultMatrix,buffer);
    selectFrameBuffer(mBuffer,1,&sFrameDefaultMatrix,buffer);
    updatePacketValues();
    for(s32 index=0;index<static_cast<s32>(mPacketCount);++index) {
        ModelDataPipeline_l* data=mModelData;
        MaterialPacket_l* packets=mMaterialPackets;
        ModelMaterial_l* material=data->mMaterials;
        u16 materialIndex=static_cast<u16>(index);
        if(materialIndex<static_cast<u32>(data->mMaterialCount)) material=&material[materialIndex];
        MaterialPacket_l* packet=&packets[index];
        // The source begins with the material flag word at +0xA0.
        updateFrameColors(&material->mFlags,packet);
        PipelineRecordBindings* bindings=reinterpret_cast<PipelineRecordBindings*>(this);
        PacketWord_l* destination=nullptr;
        if(static_cast<u32>(index)<static_cast<u32>(bindings->mPacketCapacity)) {
            PacketWord_l* output=bindings->mPackets;
            destination=&output[index];
        }
        FramePacketHeader_l* header=reinterpret_cast<FramePacketHeader_l*>(packet->mHeader.get());
        u32 relative=header->mStateOffset;
        be<u32>* source=nullptr;
        if(relative!=0) source=gabi::at<be<u32>>(gabi::ea(&header->mStateOffset)+relative);
        // Preserve the original interleaved transfers, including overlapping spans.
        // The original copy starts at the PacketWord base, before its named payload field.
        be<u32>* output=reinterpret_cast<be<u32>*>(destination);
        for(u32 word=0;word!=12;++word) output[word]=static_cast<u32>(source[word]);
    }
    if(state->mAnimationDisabled==0) {
        // Reload model data after each callback; a callback may replace the owner.
        FrameData_l* data=reinterpret_cast<FrameData_l*>(mModelData.get());
        FrameAnimator_l* animator=data->mFirst;
        if(animator!=nullptr) {
            FrameAnimatorMethods_l* methods=animator->mMethods;
            gabi::call_ptr<void>(methods->mUpdate.get(),animator,mBuffer);
            data=reinterpret_cast<FrameData_l*>(mModelData.get());
        }
        animator=data->mSecond;
        if(animator!=nullptr) {
            FrameAnimatorMethods_l* methods=animator->mMethods;
            gabi::call_ptr<void>(methods->mUpdate.get(),animator,mBuffer);
            data=reinterpret_cast<FrameData_l*>(mModelData.get());
        }
        animator=data->mThird;
        if(animator!=nullptr) {
            FrameAnimatorMethods_l* methods=animator->mMethods;
            gabi::call_ptr<void>(methods->mUpdate.get(),animator,mBuffer);
            data=reinterpret_cast<FrameData_l*>(mModelData.get());
        }
        animator=data->mFourth;
        if(animator!=nullptr) {
            FrameAnimatorMethods_l* methods=animator->mMethods;
            gabi::call_ptr<void>(methods->mUpdate.get(),animator,mBuffer);
            data=reinterpret_cast<FrameData_l*>(mModelData.get());
        }
        animator=data->mLast;
        if(animator!=nullptr) {
            FrameAnimatorMethods_l* methods=animator->mMethods;
            gabi::call_ptr<void>(methods->mUpdate.get(),animator,mBuffer);
        }
    }
    updateLighting(&state->mPages,-1,nullptr);
    sFrameQueue->submitOrExecute(&state->mJob);
    if(state->mFinishDisabled==0) finishFrameObject(state->mFinishObject.get());
}
}
/* WWHD glue
data sFrameDefaultMatrix 104B45F8
data sFrameQueue 101F997C
ext 027DE8A0 void selectFrameBuffer(void*,u32,const j3d::pipeline::MatrixRows*,u32)
ext 027F9FB8 void updateFrameColors(void*,j3d::pipeline::MaterialPacket_l*)
ext 027FB678 void finishFrameObject(void*)
*/

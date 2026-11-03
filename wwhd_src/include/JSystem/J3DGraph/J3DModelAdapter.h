// Companion views for fields not yet exported by the merged J3DModel header.
#pragma once
#include "JSystem/J3DGraph.h"
#include "JSystem/J3DGraph/J3DPacketAdapter.h"
#include "JSystem/J3DGraph/J3DInlineConversions.h"
#include "agl/gx2/BufferRuntime.h"
namespace j3d {
struct ModelMethods_l { u8 mPrefix[0x24]; be<u32> mCalculate; };
struct JointCalculator_l { gptr<ModelMethods_l> mMethods; };
struct ModelMaterialLink_l;
struct PacketMaterialBinding_l;
struct ModelRenderState_l;
struct JointNode_l {
    gptr<void> mResource;
    u8 mReserved[4];
    be<u32> mCallback;
    u8 mPadding[4];
    gptr<ModelMaterialLink_l> mMaterials;
    gptr<JointCalculator_l> mCalculator;
    u8 mTrailing[4];
};
struct JointTree_l { u8 mPrefix[8]; be<u16> mCount; };
struct ModelMemberCall_l { be<s16> mAdjust; be<s16> mVirtualIndex; be<u32> mFunction; };
struct ModelNameCallback_l {
    gptr<sead::VtableSlot> mMethods;
    gptr<J3DModel> mOwner;
    ModelMemberCall_l mCall;
};
struct ModelName_l {
    sead::SafeText mValue;
    be<u8> mActive;
    u8 mPadding[3];
    gptr<sead::VtableSlot> mMethods;
    ModelNameCallback_l mCallback;
};
struct ModelMaterialState_l {
    u8 mPrefix[0x14];
    be<u16> mShapeCount;
    be<u16> mBindingCount;
    gptr<agl::gx2util::PaletteBuffer> mMatrices;
    gptr<J3DEntryMaterialLookup> mLookup;
    gptr<PacketMaterialBinding_l> mBindings;
    u8 mMiddle[0x1C];
    gptr<void> mStorage;
    gptr<void> mAuxiliary;
    be<u32> mAuxiliaryBytes;
};
struct ModelState_l {
    sead::Disposer mDisposer;
    be<u8> mVisible;
    u8 mVisiblePadding[3];
    ModelMaterialState_l mMaterialState;
    u8 mAnimation[0xC];
    be<u8> mRenderEnabled;
    u8 mRenderPadding[3];
    be<u32> mRenderMode;
    be<u32> mChannels;
    be<u8> mDrawType;
    u8 mDrawPadding[3];
    gptr<void> mDirectRoot;
    gptr<void> mSetupState;
    gptr<ModelRenderState_l> mRenderState;
    be<u8> mDirectPrograms;
    u8 mProgramPadding[3];
    ModelName_l mName;
    gptr<J3DModelData> mModelData;
    be<u32> mFlags;
    be<u32> mCalculateCallback;
    be<u32> mUserArea;
    be<f32> mScale[3];
    be<f32> mBaseMatrix[12];
    be<u32> mWorkMatrix[12];
    be<s32> mPacketCount;
    gptr<ShapePacket> mPackets;
    be<s32> mAuxiliaryCount;
    gptr<void> mAuxiliary;
    be<s32> mMaterialCount;
    gptr<void> mMaterials;
    agl::gx2util::PoseCallback mPoseCallback;
};
struct ModelAdapter {
    static J3DModel* create(J3DModel*);
    static void reset(J3DModel*);
    static void destroy(J3DModel*,s32);
    static void calculate(J3DModel*);
    static void advanceJoint(void*,struct JointCallbackIterator_l*);
    static u32 initializePackets(J3DModel*);
    static u32 configure(J3DModel*,J3DModelData*,u32,u32);
};
struct JointCallbackHeader_l { be<u32> mIdentifier; };
struct JointCallbackIterator_l { gptr<JointCallbackHeader_l> mHeader; be<s32> mNext; };
struct ModelCurrent { gptr<J3DModel> mModel; };
struct ModelScalar { be<f32> mValue; };
struct ModelPair { be<f32> mValues[2]; };
struct ModelMatrix { be<f32> mValues[12]; };
}
extern j3d::ModelCurrent& sCurrentModel;
WWHD_SIZE(j3d::JointNode_l,0x1C);
WWHD_OFFSET(j3d::JointNode_l,mCalculator,0x14);
WWHD_SIZE(j3d::ModelMemberCall_l,8);
WWHD_SIZE(j3d::ModelName_l,0x20);
WWHD_SIZE(j3d::ModelMaterialState_l,0x4C);
WWHD_OFFSET(j3d::ModelState_l,mMaterialState,0x14);
WWHD_OFFSET(j3d::ModelState_l,mAnimation,0x60);
WWHD_OFFSET(j3d::ModelState_l,mRenderEnabled,0x6C);
WWHD_OFFSET(j3d::ModelState_l,mRenderMode,0x70);
WWHD_OFFSET(j3d::ModelState_l,mName,0x8C);
WWHD_OFFSET(j3d::ModelState_l,mModelData,0xAC);
WWHD_OFFSET(j3d::ModelState_l,mCalculateCallback,0xB4);
WWHD_OFFSET(j3d::ModelState_l,mScale,0xBC);
WWHD_OFFSET(j3d::ModelState_l,mBaseMatrix,0xC8);
WWHD_OFFSET(j3d::ModelState_l,mWorkMatrix,0xF8);
WWHD_OFFSET(j3d::ModelState_l,mPacketCount,0x128);
WWHD_OFFSET(j3d::ModelState_l,mPoseCallback,0x140);
WWHD_SIZE(j3d::ModelState_l,sizeof(J3DModel));

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_OFFSET(j3d::ModelMethods_l,mCalculate,0x24);
WWHD_OFFSET(j3d::JointCalculator_l,mMethods,0);
WWHD_OFFSET(j3d::JointTree_l,mCount,8);
WWHD_OFFSET(j3d::ModelNameCallback_l,mCall,8);
WWHD_OFFSET(j3d::JointCallbackHeader_l,mIdentifier,0);
WWHD_OFFSET(j3d::JointCallbackIterator_l,mNext,4);
WWHD_SIZE(j3d::ModelCurrent,4);
WWHD_SIZE(j3d::ModelScalar,4);
WWHD_SIZE(j3d::ModelPair,8);
WWHD_SIZE(j3d::ModelMatrix,48);

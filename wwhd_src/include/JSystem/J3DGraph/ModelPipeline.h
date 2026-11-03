#pragma once
#include "wwhd.h"
#include "JSystem/J3DGraph/ModelDataPipeline.h"
namespace j3d::pipeline {
struct MaterialPacketHeader_l { u8 mPrefix[0x11]; be<u8> mValueCount; };
struct MaterialPacket_l { gptr<MaterialPacketHeader_l> mHeader; u8 mState[0x28]; gptr<be<u32>> mValues; u8 mTail[12]; };
struct MaterialApplyTable_l { u8 mPrefix[0x14]; gfn mApply; };
struct MaterialApplyObject_l { u8 mPrefix[0x68]; gptr<MaterialApplyTable_l> mMethods; };
struct PipelineLightProvider_l;
struct PipelineFogProvider_l;
struct LightingPageArray_l;
struct PipelineJointEntryCount_l { u8 mPrefix[0x2C]; be<u16> mCount; };
struct MaterialCountHeader_l { u8 mPrefix[0x24]; be<u16> mMaterialCount; };
// Companion view of the established model layout, pending the canonical header owner's fold.
struct ModelPipeline_l {
    u8 mPrefix[0x14]; u8 mBuffer[0x16]; be<u16> mPacketCount;
    gptr<PipelineJointEntryCount_l> mJointEntryCount; u8 mMatrixState[4]; gptr<MaterialPacket_l> mMaterialPackets;
    u8 mState[0x34]; be<u8> mBufferIndex; u8 mBufferPadding[3]; be<s32> mLightingCount; u8 mModeState[0x38];
    gptr<ModelDataPipeline_l> mModelData; u8 mTail[0x94];
    void advanceFrame();
    void updatePacketBindings();
    void updatePacketValues();
    void updateLighting(LightingPageArray_l*,s32,const MatrixRows*);
};
}
WWHD_SIZE(j3d::pipeline::MaterialPacket_l,0x3C);
WWHD_OFFSET(j3d::pipeline::MaterialPacket_l,mValues,0x2C);
WWHD_OFFSET(j3d::pipeline::MaterialApplyObject_l,mMethods,0x68);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mApplyObject,0x24);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mValueStart,0x28);
WWHD_SIZE(j3d::pipeline::ModelMaterial_l,0x39C);
WWHD_SIZE(j3d::pipeline::MaterialValue_l,0x24);
WWHD_OFFSET(j3d::pipeline::MaterialValue_l,mWord,0x20);
WWHD_OFFSET(j3d::pipeline::ModelDataPipeline_l,mMaterials,0x10);
WWHD_OFFSET(j3d::pipeline::ModelDataPipeline_l,mValueTable,0x30);
WWHD_OFFSET(j3d::pipeline::ModelPipeline_l,mPacketCount,0x2A);
WWHD_OFFSET(j3d::pipeline::ModelPipeline_l,mMaterialPackets,0x34);
WWHD_OFFSET(j3d::pipeline::ModelPipeline_l,mBufferIndex,0x6C);
WWHD_OFFSET(j3d::pipeline::ModelPipeline_l,mModelData,0xAC);
WWHD_SIZE(j3d::pipeline::ModelPipeline_l,0x144);
WWHD_SIZE(j3d::pipeline::MaterialPacketHeader_l,0x12);
WWHD_SIZE(j3d::pipeline::MaterialApplyTable_l,0x18);
WWHD_SIZE(j3d::pipeline::MaterialApplyObject_l,0x6C);
WWHD_SIZE(j3d::pipeline::MaterialValueTable_l,0x20);
WWHD_SIZE(j3d::pipeline::ModelDataPipeline_l,0x4C);
WWHD_SIZE(j3d::pipeline::MaterialCountHeader_l,0x26);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mLightProvider,0x10);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mFogProvider,0x20);
WWHD_OFFSET(j3d::pipeline::ModelPipeline_l,mLightingCount,0x70);

WWHD_OFFSET(j3d::pipeline::ModelPipeline_l,mJointEntryCount,0x2C);
WWHD_OFFSET(j3d::pipeline::PipelineJointEntryCount_l,mCount,0x2C);
WWHD_SIZE(j3d::pipeline::PipelineJointEntryCount_l,0x2E);

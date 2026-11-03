// Model-resource layouts derived from this unit's own instructions.
// The historical tww declaration cross-check is excluded from layout evidence.
#pragma once
#include "JSystem/J3DGraph/J3DFrameRecords.h"
#include "wwhd.h"
#include "JSystem/J3DGraph/PipelineConversions.h"
#include "JSystem/J3DGraph/PipelineInlineState.h"
#include "sead/RenderState.h"
#include "sead/RenderMath.h"
namespace sead { struct RegistryHeap; }
namespace j3d::pipeline {
struct ModelMaterial_l;
struct PipelineLightProvider_l;
struct PipelineFogProvider_l;
struct MaterialApplyObject_l;
struct RawTransformEntry_l { u8 mData[0x40]; };
struct RawLookupNode_l { u8 mPrefix[12]; be<u32> mDataOffset; };
struct RawLookupBlock_l { u8 mHeader[8]; RawLookupNode_l mRoot,mFirstEntry; };
struct RawModelInfo_l {
    u8 mPrefix[8]; be<u16> mEntryCount; u8 mState[10]; be<u32> mEntriesOffset;
};
struct RawModelResource_l {
    u8 mPrefix[12]; be<u32> mInfoOffset; u8 mState[4];
    be<u32> mShapeLookupOffset,mMaterialLookupOffset;
    u8 mCountState[6]; be<u16> mShapeCount,mMaterialCount;
};
struct RawShapeResource_l { u8 mPrefix[12]; be<u16> mLookupIndex,mMaterialIndex,mEntryIndex; };
struct RawMaterialResource_l {
    u8 mPrefix[16]; be<u8> mScalarCount,mValueCount; u8 mState[14];
    be<u32> mRenderStateOffset; u8 mRenderPadding[4];
    be<u32> mValuesOffset,mScalarsOffset;
};
struct RawValueReference_l { be<u32> mNameOffset,mAttributeOffset; };
struct RawValueAttribute_l { u8 mPrefix[8]; be<u32> mKind,mCount; };
struct RawScalarResource_l { u8 mData[0x18]; };
struct RawRenderState_l { be<u32> mFlags; u8 mState[0x2C]; };
WWHD_SIZE(j3d::pipeline::RawRenderState_l,0x30);
struct MaterialResourceEntry_l {
    gptr<RawTransformEntry_l> mResource; be<u32> mWord4; gfn mCallback; be<u32> mMethods;
    gptr<ModelMaterial_l> mFirstMaterial; be<u32> mState; be<u8> mFlags; u8 mPadding[3];
};
struct MaterialShapeBinding_l { gptr<RawShapeResource_l> mResource; be<u8> mEnabled; u8 mPadding[3]; };
struct MaterialIndexedMethods_l { u8 mPrefix[12]; gfn mSet; };
struct MaterialTextureMatrix_l;
struct MaterialIndexedProvider_l {
    gptr<MaterialIndexedMethods_l> mMethods;
    gptr<MaterialTextureMatrix_l> mMatrices[8];
    be<u32> mWord24;
    void setTextureMatrix(u32 index,MaterialTextureMatrix_l* matrix);
    MaterialTextureMatrix_l* textureMatrix(u32 index) const;
};
struct MaterialStateProvider_l { u8 mPrefix[4]; be<u32> mMethods; u8 mState[0x44]; };
struct MaterialVectorProvider_l { be<u32> mMethods; PipelineVectorPair mValues[3]; };
struct MaterialColorProvider_l { be<u32> mMethods; be<u32> mState; ByteColor mColor; be<u8> mEnabled; u8 mPadding[3]; };
struct MaterialFogSettings_l {
    be<u8> mKind,mAdjustmentEnabled; be<u16> mCenter;
    be<f32> mStart,mEnd,mNear,mFar; ByteColor mColor;
};
struct MaterialTextureInfo_l {
    be<u8> mProjection,mInfo; u8 mPadding[2];
    be<f32> mCenter[3],mScale[2]; be<s16> mRotation; u8 mRotationPadding[2];
    be<f32> mTranslation[2]; sead::RenderMatrix44 mEffect;
};
struct MaterialTextureMatrix_l { MaterialTextureInfo_l mInfo; MatrixRows mMatrix; u8 mState[0x60]; };
struct MaterialScalarValues_l { be<s32> mCount; gptr<be<s32>> mValues; };
struct ModelMaterial_l {
    gptr<RawMaterialResource_l> mResource;
    gptr<ModelMaterial_l> mNext;
    gptr<MaterialShapeBinding_l> mShapeBinding;
    gptr<MaterialResourceEntry_l> mEntry;
    gptr<PipelineLightProvider_l> mLightProvider;
    gptr<MaterialIndexedProvider_l> mIndexedProvider;
    gptr<MaterialStateProvider_l> mStateProvider;
    gptr<MaterialVectorProvider_l> mVectorProvider;
    gptr<PipelineFogProvider_l> mFogProvider;
    gptr<MaterialApplyObject_l> mApplyObject;
    be<u32> mValueStart;
    sead::CompactRenderState mRenderState;
    be<u32> mFlags;
    sead::ColorChannels mColors[11];
    PipelineStorageRecord mStorage[3][2];
    MatrixRows mMatrices[8];
    sead::ColorChannels mExtraColors[6];
    MaterialScalarValues_l mScalarValues;
};
struct MaterialValue_l {
    u8 mPrefix[2]; be<u16> mKind,mCount; u8 mHeaderPadding[2];
    be<u8> mFlag; u8 mFlagPadding[3];
    be<u32> mFirstRelative; u8 mPayload[12]; be<u32> mLastRelative,mWord;
};
struct MaterialValueWords_l { be<u32> mWords[9]; };
struct MaterialValueTable_l {
    be<u16> mCount,mCapacity; gptr<MaterialValue_l> mValues;
    be<u32> mReserved[5]; be<u32> mEnabled;
};
struct MaterialNameBindings_l { be<u32> mWords[2]; be<u16> mCount; u8 mPadding[2]; be<u32> mMethods; };
struct PipelineResourceVariant_l { u8 mStorage[0x1D8]; be<u32> mState; };
struct ModelDataPipeline_l {
    gptr<RawModelResource_l> mResource;
    be<u32> mEntryCount; gptr<MaterialResourceEntry_l> mEntries;
    be<u32> mMaterialCount; gptr<ModelMaterial_l> mMaterials;
    FrameRecords mBindings;
    be<u32> mVariantCount; gptr<PipelineResourceVariant_l> mVariants;
    be<u8> mFlag; u8 mFlagPadding[3]; gptr<MaterialValueTable_l> mValueTable;
    gptr<MaterialNameBindings_l> mNameBindings;
    be<u32> mWord38,mWord3C,mWord40,mWord44,mWord48;
    static ModelDataPipeline_l* create(RawModelResource_l*,void* bindingSource,u32 unused,sead::RegistryHeap*);
    void initializeResource(RawModelResource_l* resource,sead::RegistryHeap* heap);
    void applyMaterialSource(void* source,void* material);
    u32 bindMaterialSources(FrameRecords*,sead::RegistryHeap*);
};
}
WWHD_SIZE(j3d::pipeline::RawTransformEntry_l,0x40);
WWHD_SIZE(j3d::pipeline::RawLookupNode_l,16);
WWHD_OFFSET(j3d::pipeline::RawLookupBlock_l,mFirstEntry,0x18);
WWHD_OFFSET(j3d::pipeline::RawModelInfo_l,mEntryCount,8);
WWHD_OFFSET(j3d::pipeline::RawModelInfo_l,mEntriesOffset,0x14);
WWHD_SIZE(j3d::pipeline::RawModelInfo_l,0x18);
WWHD_OFFSET(j3d::pipeline::RawModelResource_l,mShapeLookupOffset,0x14);
WWHD_OFFSET(j3d::pipeline::RawModelResource_l,mMaterialLookupOffset,0x18);
WWHD_OFFSET(j3d::pipeline::RawModelResource_l,mShapeCount,0x22);
WWHD_OFFSET(j3d::pipeline::RawModelResource_l,mMaterialCount,0x24);
WWHD_SIZE(j3d::pipeline::RawModelResource_l,0x26);
WWHD_OFFSET(j3d::pipeline::RawShapeResource_l,mLookupIndex,0xC);
WWHD_OFFSET(j3d::pipeline::RawShapeResource_l,mMaterialIndex,0xE);
WWHD_OFFSET(j3d::pipeline::RawShapeResource_l,mEntryIndex,0x10);
WWHD_SIZE(j3d::pipeline::RawShapeResource_l,0x12);
WWHD_OFFSET(j3d::pipeline::RawMaterialResource_l,mScalarCount,0x10);
WWHD_OFFSET(j3d::pipeline::RawMaterialResource_l,mRenderStateOffset,0x20);
WWHD_OFFSET(j3d::pipeline::RawMaterialResource_l,mValuesOffset,0x28);
WWHD_OFFSET(j3d::pipeline::RawMaterialResource_l,mScalarsOffset,0x2C);
WWHD_SIZE(j3d::pipeline::RawMaterialResource_l,0x30);
WWHD_SIZE(j3d::pipeline::RawValueReference_l,8);
WWHD_SIZE(j3d::pipeline::RawValueAttribute_l,16);
WWHD_SIZE(j3d::pipeline::RawScalarResource_l,0x18);
WWHD_OFFSET(j3d::pipeline::MaterialResourceEntry_l,mWord4,4);
WWHD_OFFSET(j3d::pipeline::MaterialResourceEntry_l,mCallback,8);
WWHD_OFFSET(j3d::pipeline::MaterialResourceEntry_l,mMethods,0xC);
WWHD_OFFSET(j3d::pipeline::MaterialResourceEntry_l,mFirstMaterial,0x10);
WWHD_OFFSET(j3d::pipeline::MaterialResourceEntry_l,mFlags,0x18);
WWHD_SIZE(j3d::pipeline::MaterialResourceEntry_l,0x1C);
WWHD_SIZE(j3d::pipeline::MaterialShapeBinding_l,8);
WWHD_OFFSET(j3d::pipeline::MaterialIndexedProvider_l,mMatrices,4);
WWHD_SIZE(j3d::pipeline::MaterialIndexedProvider_l,0x28);
WWHD_SIZE(j3d::pipeline::MaterialStateProvider_l,0x4C);
WWHD_SIZE(j3d::pipeline::MaterialVectorProvider_l,0x58);
WWHD_SIZE(j3d::pipeline::MaterialColorProvider_l,0x10);
WWHD_SIZE(j3d::pipeline::MaterialFogSettings_l,0x18);
WWHD_OFFSET(j3d::pipeline::MaterialTextureInfo_l,mEffect,0x24);
WWHD_SIZE(j3d::pipeline::MaterialTextureInfo_l,0x64);
WWHD_OFFSET(j3d::pipeline::MaterialTextureMatrix_l,mMatrix,0x64);
WWHD_SIZE(j3d::pipeline::MaterialTextureMatrix_l,0xF4);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mNext,4);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mLightProvider,0x10);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mFogProvider,0x20);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mApplyObject,0x24);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mValueStart,0x28);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mRenderState,0x2C);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mFlags,0xA0);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mColors,0xA4);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mStorage,0x154);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mMatrices,0x1B4);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mExtraColors,0x334);
WWHD_OFFSET(j3d::pipeline::ModelMaterial_l,mScalarValues,0x394);
WWHD_SIZE(j3d::pipeline::ModelMaterial_l,0x39C);
WWHD_OFFSET(j3d::pipeline::MaterialValue_l,mFirstRelative,0xC);
WWHD_OFFSET(j3d::pipeline::MaterialValue_l,mLastRelative,0x1C);
WWHD_OFFSET(j3d::pipeline::MaterialValue_l,mWord,0x20);
WWHD_SIZE(j3d::pipeline::MaterialValue_l,0x24);
WWHD_SIZE(j3d::pipeline::MaterialValueWords_l,0x24);
WWHD_SIZE(j3d::pipeline::MaterialValueTable_l,0x20);
WWHD_SIZE(j3d::pipeline::MaterialNameBindings_l,0x10);
WWHD_OFFSET(j3d::pipeline::ModelDataPipeline_l,mMaterials,0x10);
WWHD_OFFSET(j3d::pipeline::ModelDataPipeline_l,mValueTable,0x30);
WWHD_OFFSET(j3d::pipeline::ModelDataPipeline_l,mNameBindings,0x34);
WWHD_SIZE(j3d::pipeline::ModelDataPipeline_l,0x4C);
WWHD_SIZE(j3d::pipeline::PipelineResourceVariant_l,0x1DC);
WWHD_OFFSET(j3d::pipeline::ModelDataPipeline_l,mBindings,0x14);
WWHD_OFFSET(j3d::pipeline::ModelDataPipeline_l,mVariantCount,0x24);
WWHD_OFFSET(j3d::pipeline::ModelDataPipeline_l,mVariants,0x28);
WWHD_OFFSET(j3d::pipeline::ModelDataPipeline_l,mFlag,0x2C);

// WWHD primitive backend: shader metadata and generated geometry ownership.
#pragma once
#include "sead/RenderMath.h"
#include "sead/Allocation.h"
#include "sead/HeapRegistry.h"
namespace sead {
struct ShaderUniform_l {
    gptr<const be<u8>> mName;
    be<u32> mType, mCount, mLocation, mBlock;
};
struct ShaderSampler_l { gptr<const be<u8>> mName; be<u32> mType, mLocation; };
struct ShaderAttribute_l { gptr<const be<u8>> mName; be<u32> mType, mCount, mLocation; };
struct VertexShader_l {
    u8 mRegisters[0xD0];
    be<u32> mProgramSize;
    gptr<void> mProgram;
    u8 mUniformBlocks[12];
    be<u32> mUniformCount;
    gptr<ShaderUniform_l> mUniforms;
    u8 mMetadata[0x18];
    be<u32> mAttributeCount;
    gptr<ShaderAttribute_l> mAttributes;
};
struct PixelShader_l {
    u8 mRegisters[0xA4];
    be<u32> mProgramSize;
    gptr<void> mProgram;
    u8 mUniformBlocks[12];
    be<u32> mUniformCount;
    gptr<ShaderUniform_l> mUniforms;
    u8 mMetadata[0x10];
    be<u32> mSamplerCount;
    gptr<ShaderSampler_l> mSamplers;
};
struct VertexStream_l {
    be<u32> mLocation, mBuffer, mOffset, mFormat, mIndexType, mDivisor, mComponentMask, mEndian;
};
struct StreamTemplate_l { be<u32> mWords[20]; };
struct PrimitiveGeometry_l { gptr<void> mVertices; gptr<void> mIndices; };
struct PrimitiveVertex_l { be<f32> x, y, z, u, v, red, green, blue, alpha; };
// Typed views of only the canonical renderer's opaque byte regions.
struct PrimitiveShaderResources_l {
    be<u32> mPositionAttribute, mTexCoordAttribute, mColorAttribute;
    VertexStream_l mStreams[3];
    gptr<void> mFetchProgram;
};
struct PrimitiveSamplerStorage_l {
    u8 mSamplerBytes[12];
    PrimitiveGeometry_l mQuad;
    gptr<void> mLineIndices;
};
struct PrimitiveBasicMeshes_l { PrimitiveGeometry_l mBox, mCube, mCylinder; };
struct PrimitiveRoundMeshes_l {
    PrimitiveGeometry_l mSphereLarge, mDiskSmall, mDiskLarge;
    gptr<be<u16>> mRingSmallIndices, mRingLargeIndices;
};
// Method-only extension: canonical named pointers and matrix/register fields are reused.
struct PrimitiveBackend : PrimitiveRenderer {
    static PrimitiveBackend* create(PrimitiveBackend* object);
    void initialize(AllocationHeap* heap, const void* shaderArchive);
};
}
WWHD_OFFSET(sead::ShaderUniform_l,mLocation,12);
WWHD_SIZE(sead::ShaderUniform_l,20);
WWHD_OFFSET(sead::VertexShader_l,mProgramSize,0xD0);
WWHD_OFFSET(sead::VertexShader_l,mUniformCount,0xE4);
WWHD_OFFSET(sead::VertexShader_l,mUniforms,0xE8);
WWHD_OFFSET(sead::VertexShader_l,mAttributeCount,0x104);
WWHD_OFFSET(sead::VertexShader_l,mAttributes,0x108);
WWHD_OFFSET(sead::PixelShader_l,mUniformCount,0xB8);
WWHD_OFFSET(sead::PixelShader_l,mUniforms,0xBC);
WWHD_OFFSET(sead::PixelShader_l,mSamplerCount,0xD0);
WWHD_OFFSET(sead::PixelShader_l,mSamplers,0xD4);
WWHD_SIZE(sead::VertexStream_l,32);
WWHD_SIZE(sead::PrimitiveGeometry_l,8);
WWHD_OFFSET(sead::PrimitiveShaderResources_l,mStreams,0xC);
WWHD_OFFSET(sead::PrimitiveShaderResources_l,mFetchProgram,0x6C);
WWHD_SIZE(sead::PrimitiveShaderResources_l,0x70);
WWHD_OFFSET(sead::PrimitiveSamplerStorage_l,mQuad,0xC);
WWHD_OFFSET(sead::PrimitiveSamplerStorage_l,mLineIndices,0x14);
WWHD_SIZE(sead::PrimitiveSamplerStorage_l,0x18);
WWHD_SIZE(sead::PrimitiveBasicMeshes_l,0x18);
WWHD_OFFSET(sead::PrimitiveRoundMeshes_l,mRingSmallIndices,0x18);
WWHD_OFFSET(sead::PrimitiveRoundMeshes_l,mRingLargeIndices,0x1C);
WWHD_SIZE(sead::PrimitiveRoundMeshes_l,0x20);
WWHD_OFFSET(sead::PrimitiveRenderer,mShaderResources,0x94);
WWHD_OFFSET(sead::PrimitiveRenderer,mFilledMeshState,0x13C);
WWHD_OFFSET(sead::PrimitiveRenderer,mWireMeshState,0x15C);
WWHD_OFFSET(sead::PrimitiveRenderer,mTrailingStorage,0x184);
WWHD_OFFSET(sead::PrimitiveBackend,mModel,4);
WWHD_OFFSET(sead::PrimitiveBackend,mProjection,0x34);
WWHD_SIZE(sead::PrimitiveBackend,0x18C);

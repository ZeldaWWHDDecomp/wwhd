// Local views proved by the HD model configuration and shape submission entries.
#pragma once
#include "JSystem/J3DGraph/J3DModelPacketSetup.h"
#include "JSystem/J3DGraph/J3DFrameRecords.h"
#include "agl/gx2/ModelRuntime.h"
namespace j3d {
struct ShaderAttribute_l {
    be<u32> mNextOffset;
    be<u32> mReserved;
    be<u32> mPayloadOffset;
    be<u32> mPayloadExtra;
    u8 mPadding[8];
    be<u8> mName[1];
};
struct ShaderAttributes_l { be<u32> mReserved; be<u32> mCount; };
struct ShaderState_l {
    sead::SafeText mName;
    u8 mPrefix[0x20];
    gptr<void> mProgram;
    u8 mMiddle[0xC];
    gptr<ShaderAttributes_l> mAttributes;
};
struct ShaderProgram_l {
    be<u8> mFlags;
    u8 mPadding[3];
    gptr<void> mDisplayList;
    be<u32> mReserved;
    be<u32> mDisplayListBytes;
    u8 mMiddle[0x6C];
    gptr<ShaderState_l> mState;
};
struct ModelProgram_l { gptr<ShaderProgram_l> mShader; u8 mMiddle[0x1D4]; gptr<FrameRecord> mFrame; };
struct ModelProgramData_l {
    u8 mPrefix[0x24];
    be<u32> mProgramCount;
    gptr<ModelProgram_l> mPrograms;
    be<u8> mDirect;
};
struct ModelColorWords_l { be<u32> mWords[4]; };
struct ModelVector_l { be<f32> mComponents[4]; };
struct ModelRenderState_l {
    u8 mPrefix[0xC];
    gptr<sead::VtableSlot> mMethods;
    u8 mBase[0x64];
    ModelVector_l mColors[11];
    ModelVector_l mVectorPairs[3][2];
    Mtx34 mMatrices[8];
    ModelVector_l mVectors[6];
};
struct ModelMaterialDrawState_l { u8 mState[0x30]; be<u32> mPassMask; be<u8> mDirty; u8 mPadding[3]; };
}
WWHD_OFFSET(j3d::ShaderAttribute_l,mName,0x18);
WWHD_OFFSET(j3d::ShaderState_l,mAttributes,0x38);
WWHD_OFFSET(j3d::ShaderProgram_l,mState,0x7C);
WWHD_SIZE(j3d::ModelProgram_l,0x1DC);
WWHD_OFFSET(j3d::ModelRenderState_l,mColors,0x74);
WWHD_OFFSET(j3d::ModelRenderState_l,mVectorPairs,0x124);
WWHD_OFFSET(j3d::ModelRenderState_l,mMatrices,0x184);
WWHD_OFFSET(j3d::ModelRenderState_l,mVectors,0x304);
WWHD_SIZE(j3d::ModelRenderState_l,0x364);
WWHD_SIZE(j3d::ModelMaterialDrawState_l,0x38);
WWHD_OFFSET(j3d::ModelState_l,mMaterialState.mBindingCount,0x2A);
WWHD_OFFSET(j3d::ModelState_l,mMaterialState.mBindings,0x34);
WWHD_OFFSET(j3d::ModelState_l,mRenderState,0x84);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_OFFSET(j3d::ShaderAttributes_l,mCount,4);
WWHD_OFFSET(j3d::ModelProgramData_l,mDirect,0x2C);
WWHD_SIZE(j3d::ModelColorWords_l,16);
WWHD_SIZE(j3d::ModelVector_l,16);

WWHD_OFFSET(j3d::ShaderState_l,mName,0);
WWHD_OFFSET(j3d::ShaderState_l,mProgram,0x28);

// Views used by the two HD shape submission entries; all extents follow their accesses.
#pragma once
#include "JSystem/J3DGraph/J3DModelConfiguration.h"
#include "JSystem/J3DGraph/J3DRenderContext.h"
namespace j3d {
struct UniformSlots_l { be<s16> mVertex; be<s16> mPixel; be<s16> mGeometry; };
struct FrameUniform_l { u8 mPrefix[0xC]; UniformSlots_l mSlots; u8 mPadding[2]; };
struct FrameShader_l {
    gptr<ShaderProgram_l> mShader;
    u8 mPrefix[8];
    be<u32> mUniformCount;
    gptr<FrameUniform_l> mUniforms;
    u8 mTrailing[0x10];
};
struct UniformBuffer_l { u8 mPrefix[0x4C]; be<u32> mActive; };
struct UniformBufferSlot_l { u8 mPrefix[0x14]; gptr<void> mData; be<u32> mReserved; be<u32> mBytes; };
struct AnimationUniform_l { u8 mState[0x23C]; };
struct AnimationUniforms_l { be<u32> mCount; gptr<AnimationUniform_l> mEntries; be<u32> mReserved; };
struct ModelUniformBindings_l {
    u8 mPrefix[0x2C];
    gptr<void> mMatrix;
    u8 mMiddle[0x18];
    gptr<void> mFallbackPrograms;
};
struct ProgramUniforms_l { u8 mPrefix[0x10]; UniformSlots_l mSlots; };
struct ShapeBufferSource_l {
    gptr<ShapeResourceHeader_l> mHeader;
    u8 mPrefix[5];
    be<u8> mBufferCount;
    u8 mPadding[2];
    gptr<void> mBuffers;
};
struct DrawProjection_l { u8 mPrefix[0x6C]; be<u8> mFlags[3]; };
struct ShapeDrawInput_l {
    be<u32> mReserved;
    be<u32> mView;
    be<u32> mState;
    be<u32> mPass;
    gptr<DrawProjection_l> mProjection;
    gptr<AnimationUniforms_l> mViews;
    u8 mMiddle[0x18];
    gptr<UniformBuffer_l> mExtra;
};
struct DrawFixedState_l {
    u8 mPrefix[8];
    be<u32> mMode;
    u8 mState[0x60];
    be<u8> mFlags[3];
    // The constructor owns 0x74 bytes; the callers reserve SP+0x10 through SP+0x83.
    u8 mPadding[5];
};
}
WWHD_SIZE(j3d::FrameUniform_l,0x14);
WWHD_SIZE(j3d::FrameShader_l,0x24);
WWHD_OFFSET(j3d::ShapeDrawInput_l,mExtra,0x30);
WWHD_SIZE(j3d::DrawFixedState_l,0x74);
WWHD_OFFSET(j3d::ShapeBufferSource_l,mBufferCount,9);
WWHD_OFFSET(j3d::ModelUniformBindings_l,mFallbackPrograms,0x48);

WWHD_OFFSET(j3d::UniformBufferSlot_l,mData,0x14);
WWHD_OFFSET(j3d::UniformBufferSlot_l,mBytes,0x1C);
WWHD_OFFSET(j3d::UniformBuffer_l,mActive,0x4C);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_SIZE(j3d::UniformSlots_l,6);
WWHD_SIZE(j3d::AnimationUniform_l,0x23C);
WWHD_OFFSET(j3d::AnimationUniforms_l,mEntries,4);
WWHD_OFFSET(j3d::ProgramUniforms_l,mSlots,0x10);
WWHD_OFFSET(j3d::DrawProjection_l,mFlags,0x6C);

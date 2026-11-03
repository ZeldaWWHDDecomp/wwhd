// Narrow views of HD shader binding inputs; external input extents remain opaque.
#pragma once
#include "JSystem/J3DGraph.h"
namespace j3d {
struct MaterialBinding;
struct ShaderResource_l {
    u8 mPrefix[0x24];
    be<u32> mCodeOffset;
};
struct ShaderInput_l { u8 mPrefix[8]; gptr<ShaderResource_l> mResource; };
struct MaterialProgramState_l {
    u8 mPrefix[0x44];
    gptr<ShaderResource_l> mResource;
    u8 mTrailing[0x20];
};
struct MatrixBufferBinding_l {
    u8 mPrefix[0x2E];
    be<u16> mJoint;
    gptr<MaterialBinding> mBinding;
};
// An address anchor only, not a claim about the external reader's extent.
struct ModelMatrixAnchor_l { u8 mOpaque; };
struct ModelMatrixInput_l {
    u8 mPrefix[0x14];
    ModelMatrixAnchor_l mInput;
    u8 mPadding[0x17];
    gptr<MatrixBufferBinding_l> mMatrices;
};
struct MaterialBinding {
    be<u32> mReserved;
    MaterialProgramState_l mState;
    gptr<void> mCode;
    gptr<ShaderInput_l> mInput;
    gptr<void> mStorage;
    void setInput(ShaderInput_l*);
    void attachModel(J3DModel*);
    void allocateForResource(void*,ShaderInput_l*);
};
struct MaterialCountHeader_l { u8 mPrefix[8]; be<u16> mCount; };
struct MaterialNameHeader_l { u8 mPrefix[4]; be<u32> mNameOffset; };
struct MaterialBufferRequest_l {
    be<u32> mCount;
    be<u32> mElementCount;
    be<u32> mBytes;
    be<u8> mEnabled;
    be<u8> mAllocate;
    u8 mPadding[2];
    be<u32> mBase;
    be<u32> mOffset;
};
struct MaterialChainNode {
    gptr<void> mResource;
    gptr<MaterialChainNode> mNext;
};
struct MaterialChain {
    u8 mPrefix[0x10];
    gptr<MaterialChainNode> mFirst;
    u8 mPadding[4];
    be<u8> mFlags;
    void attach(void*,MaterialChainNode*);
};
}
WWHD_OFFSET(j3d::ShaderResource_l,mCodeOffset,0x24);
WWHD_OFFSET(j3d::ShaderInput_l,mResource,8);
WWHD_SIZE(j3d::MaterialProgramState_l,0x68);
WWHD_OFFSET(j3d::MaterialBinding,mState,4);
WWHD_OFFSET(j3d::MaterialBinding,mState.mResource,0x48);
WWHD_OFFSET(j3d::MaterialBinding,mCode,0x6C);
WWHD_OFFSET(j3d::MaterialBinding,mInput,0x70);
WWHD_OFFSET(j3d::MaterialBinding,mStorage,0x74);
WWHD_SIZE(j3d::MaterialBufferRequest_l,24);
WWHD_OFFSET(j3d::MatrixBufferBinding_l,mJoint,0x2E);
WWHD_OFFSET(j3d::MatrixBufferBinding_l,mBinding,0x30);
WWHD_OFFSET(j3d::ModelMatrixInput_l,mInput,0x14);
WWHD_OFFSET(j3d::ModelMatrixInput_l,mMatrices,0x2C);
WWHD_OFFSET(j3d::MaterialChain,mFirst,0x10);
WWHD_OFFSET(j3d::MaterialChain,mFlags,0x18);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_OFFSET(j3d::MaterialCountHeader_l,mCount,8);
WWHD_OFFSET(j3d::MaterialNameHeader_l,mNameOffset,4);
WWHD_OFFSET(j3d::MaterialChainNode,mNext,4);

// Resource and dispatch prefixes used by the HD shape packet.
#pragma once
#include "JSystem/J3DGraph/J3DPacketAdapter.h"
#include "JSystem/J3DGraph/J3DModelAdapter.h"
namespace j3d {
struct ShapeResourceHeader_l {
    u8 mPrefix[0xC];
    be<u16> mMaterialIndex;
    be<u16> mBindingIndex;
    be<u16> mJointIndex;
    u8 mPadding[0x12];
    be<u32> mPrimitiveOffset;
};
struct ShapeResource_l { gptr<ShapeResourceHeader_l> mHeader; };
struct PacketMaterialHeader_l {
    u8 mPrefix[4];
    be<u32> mNameOffset;
    u8 mMiddle[4];
    be<u16> mIdentifier;
    u8 mParameterPrefix[0xE];
    be<u32> mParametersOffset;
    be<u32> mStateOffset;
};
struct MaterialDualBuffer_l { u8 mPrefix[4]; be<u32> mBytes; u8 mTrailing[0x20]; };
struct PacketMaterialBinding_l {
    gptr<PacketMaterialHeader_l> mHeader;
    u8 mPrefix[0x10];
    MaterialDualBuffer_l mBuffer;
};
struct PacketModelRoot_l { u8 mPrefix[4]; be<u32> mNameOffset; };
struct PacketModelResources_l {
    u8 mPrefix[0x14];
    gptr<PacketModelRoot_l> mRoot;
    u8 mMiddle[0x1C];
    gptr<PacketMaterialBinding_l> mBindings;
};
struct PacketProperty_l { u8 mPrefix[8]; be<u32> mValue; };
struct PacketLegacyState_l { be<u32> mFlags; };
struct PacketPrimitive_l { u8 mPrefix[0xC]; be<u16> mIndex; };
struct PacketSubmissionMethods_l { u8 mPrefix[0x2C]; be<u32> mBindProgram; };
struct PacketSubmission_l { u8 mPrefix[0xC]; gptr<PacketSubmissionMethods_l> mMethods; };
struct PacketDrawBuffer_l {
    u8 mPrefix[0xC];
    be<u32> mMode;
    u8 mMiddle[0xC];
    gptr<Mtx34> mMatrix;
};
struct PacketDrawBuffers_l { gptr<PacketDrawBuffer_l> mBuffers[2]; };
struct PacketEntryMethods_l { ModelMemberCall_l mEntries[8]; };
}
WWHD_OFFSET(j3d::ShapeResourceHeader_l,mBindingIndex,0xE);
WWHD_OFFSET(j3d::ShapeResourceHeader_l,mJointIndex,0x10);
WWHD_OFFSET(j3d::ShapeResourceHeader_l,mPrimitiveOffset,0x24);
WWHD_OFFSET(j3d::PacketMaterialHeader_l,mParametersOffset,0x1C);
WWHD_OFFSET(j3d::PacketMaterialHeader_l,mStateOffset,0x20);
WWHD_SIZE(j3d::PacketMaterialBinding_l,0x3C);
WWHD_OFFSET(j3d::PacketModelResources_l,mRoot,0x14);
WWHD_OFFSET(j3d::PacketModelResources_l,mBindings,0x34);
WWHD_OFFSET(j3d::PacketDrawBuffer_l,mMatrix,0x1C);

WWHD_SIZE(j3d::PacketEntryMethods_l,0x40);

// Accessed prefix offsets and value strides; no unknown foreign extent is asserted.
WWHD_OFFSET(j3d::ShapeResource_l,mHeader,0);
WWHD_SIZE(j3d::MaterialDualBuffer_l,0x28);
WWHD_OFFSET(j3d::PacketModelRoot_l,mNameOffset,4);
WWHD_OFFSET(j3d::PacketProperty_l,mValue,8);
WWHD_SIZE(j3d::PacketLegacyState_l,4);
WWHD_OFFSET(j3d::PacketPrimitive_l,mIndex,0xC);
WWHD_OFFSET(j3d::PacketSubmissionMethods_l,mBindProgram,0x2C);
WWHD_OFFSET(j3d::PacketSubmission_l,mMethods,0xC);
WWHD_SIZE(j3d::PacketDrawBuffers_l,8);

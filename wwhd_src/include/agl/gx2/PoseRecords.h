/* Resource-bone and local-pose records shared by the graphics utilities. */
#pragma once
#include "agl/gx2/MatrixUtility.h"

namespace agl { namespace gx2util {
struct PoseBoneDescriptor {
    be<u32> mReserved; be<u16> mIndex; be<u16> mParent; u8 mReserved8[4];
    be<u16> mBillboardParent; u8 mReservedE[2]; be<u32> mFlags; be<f32> mScale[3];
    be<f32> mRotation[4]; be<f32> mTranslation[3]; u8 mTail[4];
};
struct PoseLocal { be<u32> mFlags; be<u32> mScale[3]; MatrixUtility mMatrix; };
WWHD_OFFSET(PoseBoneDescriptor, mIndex, 4);
WWHD_OFFSET(PoseBoneDescriptor, mParent, 6);
WWHD_OFFSET(PoseBoneDescriptor, mBillboardParent, 0xC);
WWHD_OFFSET(PoseBoneDescriptor, mFlags, 0x10);
WWHD_OFFSET(PoseBoneDescriptor, mScale, 0x14);
WWHD_OFFSET(PoseBoneDescriptor, mRotation, 0x20);
WWHD_OFFSET(PoseBoneDescriptor, mTranslation, 0x30);
WWHD_SIZE(PoseBoneDescriptor, 0x40);
WWHD_OFFSET(PoseLocal, mMatrix, 0x10);
WWHD_SIZE(PoseLocal, 0x40);
} }

#pragma once
#include "agl/gx2/GpuUtilitySupport.h"
namespace world_pose_support {
using agl_gx2_support::FloatWord;
using agl_gx2_support::TransformBone_l;
struct AffineMatrix_l { FloatWord mValues[12]; };
struct BoneTopology_l { u8 mPrefix[4]; be<u16> mIndex,mParent; u8 mTail[56]; };
struct WorldCursor_l { gptr<be<u32>> mIndex; be<u32> mNext; };
struct WorldUpdate_l { gptr<AffineMatrix_l> mWorld; be<u32> mFlags; gptr<TransformBone_l> mLocal; };
struct ScaledWorldUpdate_l { gptr<AffineMatrix_l> mWorld; gptr<be<u32>> mScale; gptr<TransformBone_l> mLocal; };
struct WorldCallback_l { gptr<be<u32>> mVtable; };
/* Qualified view of the unmerged PaletteBuffer prefix. Adopt its owner header
   when that unit is merged; callback semantics here follow the assigned body. */
struct WorldSkeleton_l {
    gptr<void> mResource; be<u16> mFlags; u8 mReserved[2];
    gptr<BoneTopology_l> mTopology; gptr<TransformBone_l> mLocal;
    gptr<AffineMatrix_l> mWorld; gptr<be<u32>> mCumulativeScale; u8 mUnexamined[0x14];
    be<u16> mBoneCount,mNextCallback; gptr<WorldCallback_l> mCallback;
    void updateWorld(const AffineMatrix_l*);
    void updateWorldScaled(const AffineMatrix_l*);
    void updateWorldCompensated(const AffineMatrix_l*);
    void updateWorldCumulativeScale(const AffineMatrix_l*);
};
WWHD_SIZE(AffineMatrix_l,48); WWHD_SIZE(BoneTopology_l,64);
WWHD_SIZE(WorldCursor_l,8); WWHD_SIZE(WorldUpdate_l,12);
WWHD_OFFSET(WorldSkeleton_l,mBoneCount,0x2C); WWHD_OFFSET(WorldSkeleton_l,mCallback,0x30);
}

WWHD_OFFSET(world_pose_support::BoneTopology_l,mIndex,4);
WWHD_OFFSET(world_pose_support::BoneTopology_l,mParent,6);
WWHD_OFFSET(world_pose_support::WorldSkeleton_l,mTopology,8);
WWHD_OFFSET(world_pose_support::WorldSkeleton_l,mLocal,0xC);
WWHD_OFFSET(world_pose_support::WorldSkeleton_l,mWorld,0x10);
WWHD_OFFSET(world_pose_support::WorldSkeleton_l,mCumulativeScale,0x14);
WWHD_OFFSET(world_pose_support::WorldCursor_l,mNext,4);
WWHD_OFFSET(world_pose_support::WorldUpdate_l,mLocal,8);
WWHD_OFFSET(world_pose_support::ScaledWorldUpdate_l,mScale,4);
WWHD_SIZE(world_pose_support::ScaledWorldUpdate_l,12);

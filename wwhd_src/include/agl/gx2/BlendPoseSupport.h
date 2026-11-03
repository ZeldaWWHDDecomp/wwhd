#pragma once
#include "agl/gx2/GpuUtilitySupport.h"
namespace blend_pose_support {
using agl_gx2_support::FloatWord;
using agl_gx2_support::SkeletonRuntime_l;
struct BlendPose_l {
    be<u32> mFlags; FloatWord mScale[3]; FloatWord mTranslation[3];
    be<f32> mWeight; FloatWord mOrientation[6];
};
struct PoseBlend_l {
    gptr<BlendPose_l> mData; be<u16> mTargetCount,mCapacity; be<u32> mState;
    gptr<void> mWork,mStorage;
    void accumulateEulerQuaternion(agl_gx2_support::TransformRuntime_l*,f32);
    void accumulateQuaternion(agl_gx2_support::TransformRuntime_l*,f32);
    void accumulateEulerAxes(agl_gx2_support::TransformRuntime_l*,f32);
    void accumulateQuaternionAxes(agl_gx2_support::TransformRuntime_l*,f32);
    void applyAxesNormalized(SkeletonRuntime_l*);
    void applyAxes(SkeletonRuntime_l*);
    void applyQuaternionNormalized(SkeletonRuntime_l*);
    void applyQuaternion(SkeletonRuntime_l*);
};
WWHD_OFFSET(BlendPose_l,mWeight,0x1C); WWHD_OFFSET(BlendPose_l,mOrientation,0x20); WWHD_SIZE(BlendPose_l,0x38);
WWHD_OFFSET(PoseBlend_l,mCapacity,6); WWHD_OFFSET(PoseBlend_l,mState,8); WWHD_SIZE(PoseBlend_l,0x14);
}

f64 blendAcos(f64);

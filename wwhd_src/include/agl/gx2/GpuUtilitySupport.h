#pragma once
#include "wwhd.h"
namespace agl_gx2_support {
f32 angleSine(u32,f32);
f32 angleCosine(u32,f32);
f32 singleMultiplyWide(f64,f64);
struct CurveBasisMatrix { be<f32> mValues[16]; };
WWHD_SIZE(CurveBasisMatrix,64);
struct CurveRegistrationState { be<u32> mWords[4]; };
WWHD_SIZE(CurveRegistrationState,16);
struct SampleCursor_l { be<f32> mStartFrame; be<f32> mEndFrame; be<u32> mIndex; };
struct AnimationCurve_l {
    be<u16> mFlags; be<u16> mKeyCount; be<u32> mTargetOffset;
    be<f32> mStartFrame; be<f32> mEndFrame; be<f32> mScale;
    be<u32> mOffsetBits; be<f32> mRepeatOffset;
    be<s32> mFramesRelative; be<s32> mValuesRelative;
    void locateFloatInterval(SampleCursor_l*,f32) const;
    void locateShortInterval(SampleCursor_l*,f32) const;
    void locateByteInterval(SampleCursor_l*,f32) const;
    f32 cubicFloatCached(SampleCursor_l*,f32);
    f32 cubicShortCached(SampleCursor_l*,f32);
    f32 cubicByteCached(SampleCursor_l*,f32);
    f32 linearFloatCached(SampleCursor_l*,f32);
    f32 linearShortCached(SampleCursor_l*,f32);
    f32 linearByteCached(SampleCursor_l*,f32);
    s32 readByteCached(SampleCursor_l*,f32);
    s32 readShortCached(SampleCursor_l*,f32);
    u32 readWordCached(SampleCursor_l*,f32);
    u32 readWord(f32) const;
    s32 readShort(f32) const;
    s32 readByte(f32) const;
    f32 linearFloat(f32) const;
    f32 linearShort(f32) const;
    f32 linearByte(f32) const;
};
WWHD_SIZE(SampleCursor_l,12); WWHD_OFFSET(SampleCursor_l,mIndex,8);
WWHD_OFFSET(AnimationCurve_l,mKeyCount,2); WWHD_OFFSET(AnimationCurve_l,mStartFrame,8);
WWHD_OFFSET(AnimationCurve_l,mFramesRelative,0x1C); WWHD_OFFSET(AnimationCurve_l,mValuesRelative,0x20); WWHD_SIZE(AnimationCurve_l,0x24);
union FloatWord { be<u32> mBits; be<f32> mValue; };
struct TransformResult_l {
    be<u32> mFlags; be<u32> mScale[3]; be<u32> mTranslation[3];
    be<u32> mReserved; FloatWord mRotation[4];
};
struct TransformTrack_l {
    be<u32> mFlags; u8 mHeader[6]; be<u8> mCurveCount; u8 mTail[13];
};
struct BindingTable_l {
    gptr<be<u32>> mEntries; be<u16> mFlags,mCapacity,mCount,mReserved;
};
struct TransformBone_l { be<u32> mFlags; be<u32> mScale[3]; FloatWord mMatrix[12]; };
struct SkeletonRuntime_l {
    gptr<void> mResource; be<u16> mFlags; u8 mHeader[6]; gptr<TransformBone_l> mBones;
};
WWHD_SIZE(FloatWord,4); WWHD_SIZE(TransformBone_l,64); WWHD_OFFSET(TransformBone_l,mMatrix,0x10);
WWHD_OFFSET(SkeletonRuntime_l,mFlags,4); WWHD_OFFSET(SkeletonRuntime_l,mBones,0xC); WWHD_SIZE(SkeletonRuntime_l,16);
enum AnimationStateBits { ResultsEvaluated = 8 };
enum BindingBits : u32 { SkipEvaluation = 0x40000000u, SkipApplication = 0x80000000u };
struct TransformRuntime_l {
    gptr<be<f32>> mFrame; u8 mFrameHeader[0x18];
    gptr<SampleCursor_l> mCursors; be<s32> mCursorCapacity,mCursorCount;
    be<u32> mLastFrameBits; gptr<TransformResult_l> mResults;
    gptr<void> mStorage; gptr<void> mDispatch; BindingTable_l mBindings;
    gptr<void> mResource; gptr<TransformTrack_l> mTracks;
    be<u32> mCapacity,mState; gptr<be<u16>> mTargets;
    void applyEulerTransforms(SkeletonRuntime_l*);
    void applyQuaternionTransforms(SkeletonRuntime_l*);
    void evaluateEulerTransforms();
    void evaluateQuaternionTransforms();
};
WWHD_SIZE(TransformResult_l,48); WWHD_OFFSET(TransformResult_l,mTranslation,0x10); WWHD_OFFSET(TransformResult_l,mRotation,0x20);
WWHD_SIZE(TransformTrack_l,24); WWHD_OFFSET(TransformTrack_l,mCurveCount,0xA);
WWHD_SIZE(BindingTable_l,12); WWHD_OFFSET(BindingTable_l,mCount,8);
WWHD_OFFSET(TransformRuntime_l,mCursors,0x1C); WWHD_OFFSET(TransformRuntime_l,mCursorCount,0x24);
WWHD_OFFSET(TransformRuntime_l,mResults,0x2C); WWHD_OFFSET(TransformRuntime_l,mBindings,0x38);
WWHD_OFFSET(TransformRuntime_l,mTracks,0x48); WWHD_OFFSET(TransformRuntime_l,mState,0x50); WWHD_SIZE(TransformRuntime_l,0x58);
union ChannelValue { be<u32> mInteger; be<f32> mFloat; };
struct ChannelBindingInfo_l {
    be<u16> mFirstCurve; be<u16> mFloatCount; be<u16> mIntegerCount;
    be<u16> mFirstDefault; be<u16> mDefaultCount; be<u16> mReserved; be<s32> mNameRelative;
};
struct ChannelTrack_l {
    be<u16> mTargetCount; be<u16> mFlags; be<u32> mReserved;
    be<u32> mResultOffset; be<u32> mFirstBinding; be<s32> mNameRelative;
    be<s32> mBindingsRelative; be<s32> mCurvesRelative; be<s32> mDefaultsRelative;
    void evaluateCached(ChannelValue*,const be<u16>*,SampleCursor_l*,f32);
    void evaluateUncached(ChannelValue*,const be<u16>*,f32);
};
WWHD_SIZE(ChannelValue,4); WWHD_SIZE(ChannelBindingInfo_l,16);
WWHD_OFFSET(ChannelBindingInfo_l,mFloatCount,2); WWHD_OFFSET(ChannelBindingInfo_l,mIntegerCount,4);
WWHD_OFFSET(ChannelTrack_l,mBindingsRelative,0x14); WWHD_OFFSET(ChannelTrack_l,mCurvesRelative,0x18); WWHD_SIZE(ChannelTrack_l,32);
struct UniformMember {
    be<u8> mEncoding; u8 mReserved; be<u16> mSourceOffset;
    be<u32> mDestinationOffset; be<u32> mConverter; u8 mTail[8];
    void copySwapped(be<u32>*,const be<u32>*) const;
    void copyNative(be<u32>*,const be<u32>*) const;
};
WWHD_OFFSET(UniformMember,mEncoding,0); WWHD_OFFSET(UniformMember,mSourceOffset,2);
WWHD_OFFSET(UniformMember,mDestinationOffset,4); WWHD_OFFSET(UniformMember,mConverter,8); WWHD_SIZE(UniformMember,20);
struct Vector3 { be<f32> x,y,z; };
struct Plane { Vector3 mNormal; be<f32> mDistance; };
struct Segment3 {
    Vector3 mBegin,mEnd;
    u32 intersectPlane(const Plane*,be<f32>*) const;
};
WWHD_SIZE(Vector3,12); WWHD_OFFSET(Plane,mDistance,12); WWHD_SIZE(Plane,16);
WWHD_OFFSET(Segment3,mEnd,12); WWHD_SIZE(Segment3,24);
struct CurveSamples {
    u8 mFormat[3];
    be<u8> mScalarCount;
    f32 linear(const be<f32>*, f32) const;
    f32 hermite(const be<f32>*, f32) const;
    f32 step(const be<f32>*, f32) const;
    f32 sine(const be<f32>*, f32) const;
    f32 cosine(const be<f32>*, f32) const;
    f32 squaredSine(const be<f32>*, f32) const;
    f32 linearKeys(const be<f32>*, f32) const;
    f32 hermiteKeys(const be<f32>*, f32) const;
    f32 stepKeys(const be<f32>*, f32) const;
    f32 zero(const be<f32>*, f32) const;
    f32 hermiteTimeKeys(const be<f32>*, f32) const;
};
WWHD_OFFSET(CurveSamples,mScalarCount,3);
WWHD_SIZE(CurveSamples,4);
}
f64 utilitySin(f64);
f64 utilityCos(f64);

void updateCurveCursor(agl_gx2_support::AnimationCurve_l*,agl_gx2_support::SampleCursor_l*,f32);
void utilityCopy(void*,const void*,u32);

u32 sampleChannelInteger(agl_gx2_support::AnimationCurve_l*,agl_gx2_support::SampleCursor_l*,f32);
f32 sampleChannelFloat(agl_gx2_support::AnimationCurve_l*,agl_gx2_support::SampleCursor_l*,f32);

void evaluateTransformCached(agl_gx2_support::TransformTrack_l*,agl_gx2_support::TransformResult_l*,agl_gx2_support::SampleCursor_l*,f32);
void evaluateTransformUncached(agl_gx2_support::TransformTrack_l*,agl_gx2_support::TransformResult_l*,f32);

void curveRegisterStatic(void*);

Pair32 transformAngleIndex(f64);

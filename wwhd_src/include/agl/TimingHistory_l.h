// agl timing history, query rings and fixed parameter storage (AGL#6), layouts from the WWHD constructors and accesses.
// Local views (namespace agl_history_l) until the shared agl header (AGL#1) is merged; the folds them in then.
#pragma once
#include "sead/FileDevice.h"
namespace agl_history_l {
struct OwnerDispatchPrefix {
    sead::VtableSlot mHeader[2];
    sead::VtableSlot mDestroy;
};
struct OwnerPrefix { gptr<OwnerDispatchPrefix> mVtable; };
struct QueryRing {
    be<u32> mCount;
    gptr<be<u64>> mData;
    be<u32> mFlags;
    void initialize(void* heap);
};
struct Sample {
    be<f32> mValue, mMeanThree, mMeanFive, mDelta, mAcceleration, mForecast;
};
struct HistoryStorage {
    gptr<Sample> mSamples;
    be<s32> mCapacity, mHead, mCount;
};
struct TimingHistory : OwnerPrefix {
    sead::Disposer mDisposer;
    be<u32> mFlags;
    gptr<QueryRing> mQueries[3];
    be<f32> mElapsed, mLoad, mPreset;
    HistoryStorage mHistory;
    be<f32> mPresetScalar;
    be<f32> mPresetPair[2];
    void initialize(void* heap);
    void update();
    void beginCapture();
    void endCapture();
};
struct ParameterDispatchPrefix {
    sead::VtableSlot mHeader[2];
    sead::VtableSlot mAssign;
    sead::VtableSlot mReserved[3];
    sead::VtableSlot mElementSize;
};
struct ParameterField {
    gptr<void> mVtable;
    be<u32> mValue;
    be<u8> mKind, mQualifier, mFlag, mReserved;
    static u32 size();
};
struct FixedParameter {
    gptr<void> mText;
    gptr<void> mTextVtable;
    gptr<ParameterDispatchPrefix> mVtable;
    ParameterField mFields[4];
    be<u8> mStorage[512];
    be<u32> mFieldCount;
    be<u8> mEnabled;
    u8 mPadding[3];
    be<u8>* storage();
    const be<u8>* storage() const;
    static u32 storageSize();
    u32 assignCompatible(FixedParameter* source);
};
// Singleton words created by this unit (each has its only store here). The two words before them are
// agl::detail::ShaderHolder's singleton and disposer (sShaderHolder, agl/ShaderHolder.h), also stored in
// agl/TimingStatics.cpp; the merged HD views of 101F8D74 (sLayer3DAngle, sGraphicsProfiler) switch to
// sTimingOwner at the merge.
extern gptr<TimingHistory>& sTimingOwner;
extern gptr<sead::Disposer>& sTimingDisposer;

// GHS register save area of a variadic function: r3..r10, then f1..f8 (stored only when CR1.eq is set).
struct VarArgSaveArea {
    be<u32> mGpr[8];
    be<f64> mFpr[8];
};
struct VarArgs {
    be<u8> mGpr, mFpr;
    u8 mPadding[2];
    gptr<void> mOverflow;
    gptr<VarArgSaveArea> mRegisters;
};
template <class T, class U> inline T* memberAt(U* object, u32 offset) {
    // Embedded GHS constructors use 32-bit addition even when it wraps to null.
    return gabi::at<T>(gabi::ea(object) + offset);
}
}
WWHD_OFFSET(agl_history_l::QueryRing,mData,4);
WWHD_OFFSET(agl_history_l::QueryRing,mFlags,8);
WWHD_SIZE(agl_history_l::QueryRing,12);
WWHD_SIZE(agl_history_l::Sample,24);
WWHD_SIZE(agl_history_l::HistoryStorage,16);
WWHD_OFFSET(agl_history_l::TimingHistory,mDisposer,4);
WWHD_OFFSET(agl_history_l::TimingHistory,mFlags,0x14);
WWHD_OFFSET(agl_history_l::TimingHistory,mQueries,0x18);
WWHD_OFFSET(agl_history_l::TimingHistory,mElapsed,0x24);
WWHD_OFFSET(agl_history_l::TimingHistory,mLoad,0x28);
WWHD_OFFSET(agl_history_l::TimingHistory,mPreset,0x2C);
WWHD_OFFSET(agl_history_l::TimingHistory,mHistory,0x30);
WWHD_OFFSET(agl_history_l::TimingHistory,mPresetScalar,0x40);
WWHD_SIZE(agl_history_l::TimingHistory,0x4C);
WWHD_SIZE(agl_history_l::ParameterField,12);
WWHD_OFFSET(agl_history_l::ParameterField,mKind,8);
WWHD_OFFSET(agl_history_l::FixedParameter,mVtable,8);
WWHD_OFFSET(agl_history_l::FixedParameter,mFields,12);
WWHD_OFFSET(agl_history_l::FixedParameter,mStorage,0x3C);
WWHD_OFFSET(agl_history_l::FixedParameter,mFieldCount,0x23C);
WWHD_OFFSET(agl_history_l::FixedParameter,mEnabled,0x240);
WWHD_SIZE(agl_history_l::FixedParameter,0x244);
WWHD_SIZE(agl_history_l::VarArgs,12);
WWHD_OFFSET(agl_history_l::VarArgSaveArea,mFpr,0x20);
WWHD_SIZE(agl_history_l::VarArgSaveArea,0x60);
WWHD_OFFSET(agl_history_l::OwnerPrefix,mVtable,0);
WWHD_OFFSET(agl_history_l::OwnerDispatchPrefix,mDestroy,0x10);
WWHD_SIZE(agl_history_l::OwnerPrefix,4);
WWHD_SIZE(agl_history_l::OwnerDispatchPrefix,24);
WWHD_OFFSET(agl_history_l::QueryRing,mCount,0);
WWHD_OFFSET(agl_history_l::Sample,mValue,0);
WWHD_OFFSET(agl_history_l::Sample,mMeanThree,4);
WWHD_OFFSET(agl_history_l::Sample,mMeanFive,8);
WWHD_OFFSET(agl_history_l::Sample,mDelta,12);
WWHD_OFFSET(agl_history_l::Sample,mAcceleration,16);
WWHD_OFFSET(agl_history_l::Sample,mForecast,20);
WWHD_OFFSET(agl_history_l::HistoryStorage,mSamples,0);
WWHD_OFFSET(agl_history_l::HistoryStorage,mCapacity,4);
WWHD_OFFSET(agl_history_l::HistoryStorage,mHead,8);
WWHD_OFFSET(agl_history_l::HistoryStorage,mCount,12);
WWHD_OFFSET(agl_history_l::TimingHistory,mPresetPair,0x44);
WWHD_OFFSET(agl_history_l::ParameterDispatchPrefix,mAssign,0x10);
WWHD_OFFSET(agl_history_l::ParameterDispatchPrefix,mElementSize,0x30);
WWHD_SIZE(agl_history_l::ParameterDispatchPrefix,0x38);
WWHD_OFFSET(agl_history_l::ParameterField,mVtable,0);
WWHD_OFFSET(agl_history_l::ParameterField,mValue,4);
WWHD_OFFSET(agl_history_l::ParameterField,mQualifier,9);
WWHD_OFFSET(agl_history_l::ParameterField,mFlag,10);
WWHD_OFFSET(agl_history_l::ParameterField,mReserved,11);
WWHD_OFFSET(agl_history_l::FixedParameter,mText,0);
WWHD_OFFSET(agl_history_l::FixedParameter,mTextVtable,4);
WWHD_OFFSET(agl_history_l::VarArgs,mGpr,0);
WWHD_OFFSET(agl_history_l::VarArgs,mFpr,1);
WWHD_OFFSET(agl_history_l::VarArgs,mOverflow,4);
WWHD_OFFSET(agl_history_l::VarArgs,mRegisters,8);

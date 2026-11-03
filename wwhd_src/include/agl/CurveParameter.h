/* WWHD two-curve parameter continuation. The inherited eight-byte prefix is opaque here. */
#pragma once
#include "sead/FileDevice.h"
#include "agl/gx2/GpuUtilitySupport.h"
namespace agl {
struct CurveEvaluatorTable_l { be<u32> mFunctions[11]; };
struct ParameterMethods_l {
    u8 mPrefix[0x14];
    be<u32> mCopy;
    u8 mBeforeKind[0x1C];
    be<u32> mKind;
    be<u32> mKindAdjustment;
    be<u32> mReadData;
    be<u32> mReadAdjustment;
    be<u32> mWriteData;
    u8 mBeforeSize[0x14];
    be<u32> mByteSize;
};
struct CurveDescriptor {
    be<u32> mMetadata;
    gptr<be<f32>> mValues;
    be<u8> mKind;
    be<u8> mReserved;
    be<u8> mCapacity;
    be<u8> mCount;
};
struct CurveValues {
    be<u32> mCount;
    be<u32> mKind;
    be<f32> mValues[30];
};
struct CurveParameter_l {
    u8 mInheritedPrefix[8];
    gptr<ParameterMethods_l> mMethods;
    CurveDescriptor mDescriptors[2];
    CurveValues mCurves[2];
    u32 kind();
    const void* readData();
    void* writeData();
    u32 byteSize();
    void initializeCurves();
    void setCurve(u32 index, u32 kind, const be<f32>* values, u32 count);
    void copyFrom(CurveParameter_l* source);
    u32 compareAndCopy(CurveParameter_l* source);
    void restoreDescriptors(const void* storage, u32 bytes);
    f32 evaluate(u32 index, f32 position) const;
};
struct CurveDefaults { be<f32> mValues[9]; };
WWHD_OFFSET(CurveDescriptor,mValues,4);
WWHD_SIZE(CurveEvaluatorTable_l,44);
WWHD_OFFSET(ParameterMethods_l,mCopy,0x14);
WWHD_OFFSET(ParameterMethods_l,mKind,0x34);
WWHD_OFFSET(ParameterMethods_l,mReadData,0x3C);
WWHD_OFFSET(ParameterMethods_l,mWriteData,0x44);
WWHD_OFFSET(ParameterMethods_l,mByteSize,0x5C);
WWHD_SIZE(ParameterMethods_l,0x60);
WWHD_OFFSET(CurveDescriptor,mKind,8);
WWHD_OFFSET(CurveDescriptor,mCapacity,10);
WWHD_OFFSET(CurveDescriptor,mCount,11);
WWHD_SIZE(CurveDescriptor,12);
WWHD_OFFSET(CurveValues,mKind,4);
WWHD_OFFSET(CurveValues,mValues,8);
WWHD_SIZE(CurveValues,128);
WWHD_OFFSET(CurveParameter_l,mMethods,8);
WWHD_OFFSET(CurveParameter_l,mDescriptors,12);
WWHD_OFFSET(CurveParameter_l,mCurves,36);
WWHD_SIZE(CurveParameter_l,292);
WWHD_SIZE(CurveDefaults,36);
}

// Image-initialized, read-only evaluator dispatch; AGL#14 is the data-only owner.
extern const agl::CurveEvaluatorTable_l& sCurveEvaluators_l;

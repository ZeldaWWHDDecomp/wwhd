// Value bank layout established by the assigned 0x20-byte constructor and buffer operations.
#pragma once
#include "agl/ParameterTree.h"
#include "sead/HeapRegistry.h"
namespace agl {
struct ParameterValueBuffer { be<u32> mCount; gptr<parameter_tree::Value> mData; };
struct ParameterValueBankDispatch { u8 mPrefix[12]; be<u32> mBeforeCopy; be<u32> mReserved; be<u32> mAfterCopy; };
struct ParameterValueBank {
    be<u32> mState;
    be<u32> mCount;
    be<s32> mActiveCount;
    be<u8> mSingleValue; u8 mPadding[3];
    be<u32> mCurrent;
    ParameterValueBuffer mValues;
    gptr<ParameterValueBankDispatch> mDispatch;
    static ParameterValueBank* create(ParameterValueBank* bank);
    void destroy(u32 flags);
    void initialize(u32 count, u32 singleValue, sead::RegistryHeap* heap);
    void copyValue(u32 destination, u32 source);
    void copyToOtherValues(u32 source);
};
}
WWHD_SIZE(agl::ParameterValueBuffer, 8);
WWHD_OFFSET(agl::ParameterValueBank, mActiveCount, 8);
WWHD_OFFSET(agl::ParameterValueBank, mSingleValue, 12);
WWHD_OFFSET(agl::ParameterValueBank, mCurrent, 16);
WWHD_OFFSET(agl::ParameterValueBank, mValues, 20);
WWHD_OFFSET(agl::ParameterValueBank, mDispatch, 28);
WWHD_SIZE(agl::ParameterValueBank, 32);
WWHD_OFFSET(agl::ParameterValueBankDispatch, mBeforeCopy, 12);
WWHD_OFFSET(agl::ParameterValueBankDispatch, mAfterCopy, 20);

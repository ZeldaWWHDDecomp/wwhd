// Packed-array parameter views established by the assigned copy/post-read bodies.
#pragma once
#include "wwhd.h"
namespace agl::packed_array {
struct QueryTable {
    u8 mPrefix[0x34];
    gfn mKind;
    u8 mKindPadding[4];
    gfn mReadData;
    u8 mReadPadding[4];
    gfn mWriteData;
    u8 mWritePadding[20];
    gfn mByteSize;
};
struct ArrayDescriptor {
    be<u32> mState;
    gptr<be<u32>> mSamples;
    be<u8> mCount, mReserved, mCapacity, mKind;
};
struct PackedArray {
    be<u32> mKind, mCount;
    be<u32> mSamples[30];
};
struct Parameter {
    be<u32> mNameHash;
    gptr<Parameter> mNext;
    gptr<QueryTable> mVtable;
    ArrayDescriptor mDescriptors[4];
    PackedArray mArrays[4];
    void copyFrom(const Parameter* source);
    void restoreViews(u32 unused, u32 serializedSize);
};
}
WWHD_OFFSET(agl::packed_array::QueryTable, mKind, 0x34);
WWHD_OFFSET(agl::packed_array::QueryTable, mReadData, 0x3C);
WWHD_OFFSET(agl::packed_array::QueryTable, mWriteData, 0x44);
WWHD_OFFSET(agl::packed_array::QueryTable, mByteSize, 0x5C);
WWHD_OFFSET(agl::packed_array::ArrayDescriptor, mSamples, 4);
WWHD_OFFSET(agl::packed_array::ArrayDescriptor, mCount, 8);
WWHD_OFFSET(agl::packed_array::ArrayDescriptor, mCapacity, 10);
WWHD_OFFSET(agl::packed_array::ArrayDescriptor, mKind, 11);
WWHD_SIZE(agl::packed_array::ArrayDescriptor, 12);
WWHD_OFFSET(agl::packed_array::PackedArray, mKind, 0);
WWHD_OFFSET(agl::packed_array::PackedArray, mCount, 4);
WWHD_OFFSET(agl::packed_array::PackedArray, mSamples, 8);
WWHD_SIZE(agl::packed_array::PackedArray, 128);
WWHD_OFFSET(agl::packed_array::Parameter, mVtable, 8);
WWHD_OFFSET(agl::packed_array::Parameter, mDescriptors, 12);
WWHD_OFFSET(agl::packed_array::Parameter, mArrays, 0x3C);
WWHD_SIZE(agl::packed_array::Parameter, 0x23C);
void movePackedParameterBytes(void*, const void*, u32, u32);

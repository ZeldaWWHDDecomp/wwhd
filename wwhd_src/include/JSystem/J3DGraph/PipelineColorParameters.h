#pragma once
#include "JSystem/J3DGraph/PipelineColorTable.h"
#include "JSystem/J3DGraph/PipelineConversions.h"
namespace j3d::pipeline {
struct PipelineParameterRecord_l {
    u8 mPrefix[2]; be<u16> mDataOffset; be<s32> mSignedWord4;
    u8 mState[4]; be<u16> mLinkedIndex; u8 mTail[6];
};
struct PipelineParameterResource_l { u8 mPrefix[0x34]; be<u32> mRecordsOffset,mLookupOffset; };
struct PipelineParameterSet_l {
    gptr<PipelineParameterResource_l> mResource;
    be<u16> mFlags; u8 mState[6]; gptr<be<u32>> mDirtyWords;
    u8 mValueState[0x18]; gptr<be<u8>> mValues;
    void* findTextureMatrix(s32 index,be<s32>* selectedIndex);
    void* editTexturePivot(s32 index);
    void* editTextureMatrix(s32 index);
    // Explicit hidden-result ABI: result is r3, parameters r4, index r5.
    static void readTevColor(SignedColor* result,PipelineParameterSet_l* parameters,s32 index);
    static void readKonstColor(ByteColor* result,PipelineParameterSet_l* parameters,s32 index);
};
namespace detail { void* editableParameterPayload(PipelineParameterSet_l*,s32,u32); }
}
WWHD_SIZE(j3d::pipeline::PipelineParameterRecord_l,0x14);
WWHD_OFFSET(j3d::pipeline::PipelineParameterRecord_l,mLinkedIndex,0xC);
WWHD_OFFSET(j3d::pipeline::PipelineParameterResource_l,mRecordsOffset,0x34);
WWHD_OFFSET(j3d::pipeline::PipelineParameterResource_l,mLookupOffset,0x38);
WWHD_SIZE(j3d::pipeline::PipelineParameterResource_l,0x3C);
WWHD_OFFSET(j3d::pipeline::PipelineParameterSet_l,mFlags,4);
WWHD_OFFSET(j3d::pipeline::PipelineParameterSet_l,mDirtyWords,0xC);
WWHD_OFFSET(j3d::pipeline::PipelineParameterSet_l,mValues,0x28);
WWHD_SIZE(j3d::pipeline::PipelineParameterSet_l,0x2C);

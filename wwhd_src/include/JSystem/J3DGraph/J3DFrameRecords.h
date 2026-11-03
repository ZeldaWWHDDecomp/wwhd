// HD frame records: three size/data buffer headers and two lifecycle bytes per element.
#pragma once
#include "sead/HeapRegistry.h"
namespace j3d {
struct ShaderProgram_l;
struct FrameBufferRange { be<u32> mCount; gptr<void> mData; };
struct FrameSourceRecord { u8 mPayload[0x84]; };
struct FrameSource {
    u8 mPrefix[0x1C];
    be<s32> mCount;
    gptr<FrameSourceRecord> mRecords;
};
struct FrameRecord {
    gptr<ShaderProgram_l> mShader;
    FrameBufferRange mRanges[3];
    be<u32> mState;
    be<u8> mActive;
    be<u8> mDirty;
    u8 mPadding[2];
};
struct FrameRecords {
    be<s32> mPublishedCount;
    gptr<FrameSource> mSource;
    be<s32> mCount;
    gptr<FrameRecord> mRecords;
    static void destroy(FrameRecords*,s32);
    void initialize(FrameSource*,sead::RegistryHeap*);
};
}
WWHD_SIZE(j3d::FrameBufferRange,8);
WWHD_SIZE(j3d::FrameSourceRecord,0x84);
WWHD_OFFSET(j3d::FrameSource,mCount,0x1C);
WWHD_OFFSET(j3d::FrameSource,mRecords,0x20);
WWHD_OFFSET(j3d::FrameRecord,mRanges,4);
WWHD_OFFSET(j3d::FrameRecord,mState,0x1C);
WWHD_OFFSET(j3d::FrameRecord,mActive,0x20);
WWHD_OFFSET(j3d::FrameRecord,mDirty,0x21);
WWHD_SIZE(j3d::FrameRecord,0x24);
WWHD_OFFSET(j3d::FrameRecords,mSource,4);
WWHD_OFFSET(j3d::FrameRecords,mCount,8);
WWHD_OFFSET(j3d::FrameRecords,mRecords,0xC);
WWHD_SIZE(j3d::FrameRecords,0x10);

#pragma once
#include "wwhd.h"
namespace j3d::pipeline {
struct PipelineFlagState {
    u8 mPrefix[8]; be<u8> mFlag; u8 mState[0x17]; be<u32> mWord;
    static PipelineFlagState* create(PipelineFlagState*);
};
struct PipelineVectorPair {
    union Scalar { be<f32> value; be<u32> bits; };
    Scalar mVectors[2][3]; be<u8> mFlag; u8 mPadding[3];
    static PipelineVectorPair* create(PipelineVectorPair*);
};
struct PipelineStorageRecord {
    u8 mStorage[16];
    static PipelineStorageRecord* create(PipelineStorageRecord*);
};
}
WWHD_SIZE(j3d::pipeline::PipelineFlagState,0x24);
WWHD_OFFSET(j3d::pipeline::PipelineFlagState,mFlag,8);
WWHD_OFFSET(j3d::pipeline::PipelineFlagState,mWord,0x20);
WWHD_SIZE(j3d::pipeline::PipelineVectorPair,0x1C);
WWHD_OFFSET(j3d::pipeline::PipelineVectorPair,mFlag,0x18);
WWHD_SIZE(j3d::pipeline::PipelineStorageRecord,16);

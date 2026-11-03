#pragma once
#include "wwhd.h"
namespace j3d::pipeline {
struct MaterialResourceEntry_l;
struct RecordWord_l;
// Typed signature over the existing four-byte guest code-address storage.
struct RecordInvoke_l : gfn {
    using Signature=void (*)(RecordWord_l*);
    void invoke(RecordWord_l* record) const;
};
struct RecordCallbackTable_l { u8 mPrefix[0x14]; RecordInvoke_l mInvoke; };
struct RecordWord_l { u8 mPrefix[0xC]; gptr<RecordCallbackTable_l> mMethods; u8 mState[0x90]; be<u32> mWord; u8 mTail[8]; };
struct PacketWord_l { u8 mPrefix[0x30]; be<u32> mWord; u8 mTail[4]; };
struct RecordKey_l { u8 mPrefix[12]; be<u16> mIndex; };
struct RecordKeyLink_l { gptr<RecordKey_l> mKey; };
struct RecordNode_l { u8 mPrefix[8]; gptr<RecordKeyLink_l> mLink; };
struct PipelineRecordBindings {
    u8 mPrefix[0x2A];
    be<u16> mPacketCount;
    u8 mState[0xFC];
    be<u32> mRecordCount;
    gptr<RecordWord_l> mRecords;
    be<u32> mIndexCount;
    gptr<be<u16>> mIndices;
    be<u32> mPacketCapacity;
    gptr<PacketWord_l> mPackets;
    void setRecordWords(u32);
    void invokeEntryRecords(const MaterialResourceEntry_l* entry);
    void setNodeRecordWord(const RecordNode_l*,u32);
    void setPacketWords(u32);
};
}
WWHD_SIZE(j3d::pipeline::RecordWord_l,0xAC);
WWHD_OFFSET(j3d::pipeline::RecordWord_l,mWord,0xA0);
WWHD_SIZE(j3d::pipeline::PacketWord_l,0x38);
WWHD_OFFSET(j3d::pipeline::PacketWord_l,mWord,0x30);
WWHD_OFFSET(j3d::pipeline::RecordKey_l,mIndex,0xC);
WWHD_OFFSET(j3d::pipeline::RecordNode_l,mLink,8);
WWHD_OFFSET(j3d::pipeline::PipelineRecordBindings,mPacketCount,0x2A);
WWHD_OFFSET(j3d::pipeline::PipelineRecordBindings,mRecordCount,0x128);
WWHD_OFFSET(j3d::pipeline::PipelineRecordBindings,mRecords,0x12C);
WWHD_OFFSET(j3d::pipeline::PipelineRecordBindings,mIndexCount,0x130);
WWHD_OFFSET(j3d::pipeline::PipelineRecordBindings,mIndices,0x134);
WWHD_OFFSET(j3d::pipeline::PipelineRecordBindings,mPacketCapacity,0x138);
WWHD_OFFSET(j3d::pipeline::PipelineRecordBindings,mPackets,0x13C);
WWHD_SIZE(j3d::pipeline::PipelineRecordBindings,0x140);

WWHD_OFFSET(j3d::pipeline::RecordWord_l,mMethods,0xC);
WWHD_OFFSET(j3d::pipeline::RecordCallbackTable_l,mInvoke,0x14);

WWHD_SIZE(j3d::pipeline::RecordInvoke_l,4);

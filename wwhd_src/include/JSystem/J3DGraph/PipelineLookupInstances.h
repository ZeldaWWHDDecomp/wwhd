#pragma once
#include "wwhd.h"
#include "sead/FileDevice.h"
#include "sead/HeapRegistry.h"
#include "JSystem/J3DGraph/PipelineConversions.h"
namespace j3d::pipeline {
struct OptionDescriptor_l { u8 mPrefix[12]; be<u32> mFlags; u8 mState[2]; be<s16> mSelection; };
struct PipelineOptionReference {
    be<f32> mScalar; be<u32> mMethods; gptr<OptionDescriptor_l> mDescriptor;
    u32 flagValue() const;
    s32 selection() const;
};
struct PipelineSlot_l {
    gptr<void> mFirstStorage,mSecondStorage; be<u32> mWord;
    u8 mState[8]; be<u32> mFormatWord,mModeWord; be<u32> mAlternateWord;
    be<u8> mStateEnabled,mAlternateEnabled,mReleaseMode; u8 mPadding;
};
struct PipelineSlotDimensions_l { Scalar mWidth,mHeight; };
struct PipelineSlotTable {
    static constexpr u32 kSlotCount=19;
    u8 mPrefix[20]; PipelineSlot_l mSlots[kSlotCount];
    u32 word(u32) const;
    u32 alternateWord(u32) const;
    void releaseWord(u32 index);
    void initializeSlots(sead::RegistryHeap* heap);
    u32 createWord(u32 index,const PipelineSlotDimensions_l* dimensions);
};
struct PipelineTextTermination { static void ensureLastByte(sead::SafeTextBuffer*); };
struct PipelineBufferTermination { static void ensureLastByte(sead::SafeTextBuffer*); };
}
WWHD_OFFSET(j3d::pipeline::OptionDescriptor_l,mFlags,12);
WWHD_OFFSET(j3d::pipeline::OptionDescriptor_l,mSelection,18);
WWHD_SIZE(j3d::pipeline::OptionDescriptor_l,20);
WWHD_OFFSET(j3d::pipeline::PipelineOptionReference,mDescriptor,8);
WWHD_SIZE(j3d::pipeline::PipelineOptionReference,12);
WWHD_OFFSET(j3d::pipeline::PipelineSlot_l,mWord,8);
WWHD_OFFSET(j3d::pipeline::PipelineSlot_l,mAlternateWord,0x1C);
WWHD_SIZE(j3d::pipeline::PipelineSlot_l,0x24);
WWHD_OFFSET(j3d::pipeline::PipelineSlotTable,mSlots,0x14);
WWHD_SIZE(j3d::pipeline::PipelineSlotTable,0x2C0);

WWHD_OFFSET(j3d::pipeline::PipelineSlot_l,mReleaseMode,0x22);

WWHD_OFFSET(j3d::pipeline::PipelineSlot_l,mFormatWord,0x14);
WWHD_OFFSET(j3d::pipeline::PipelineSlot_l,mModeWord,0x18);
WWHD_OFFSET(j3d::pipeline::PipelineSlot_l,mStateEnabled,0x20);
WWHD_OFFSET(j3d::pipeline::PipelineSlot_l,mAlternateEnabled,0x21);

// Collection prefix and record stride established by this unit's accessors and initializer.
#pragma once
#include "sead/HeapRegistry.h"
#include "sead/ElementAt.h"   // sead::elementPtr / nextElement: 32-bit guest pointer walks
#include "sead/RuntimeType.h"
#include "agl/UniformBuffer.h"

namespace agl {
struct RenderSelectionDispatch_l {
    u8 mPrefix[0xA0];
    sead::VtableSlot mIsRecordEnabled;
    u8 mMaskSetup[0x10];
    sead::VtableSlot mSetRecordEnabled;
    sead::VtableSlot mTestRecordMask;
    u8 mPreparation[0x10];
    sead::VtableSlot mPrepareHeap;
    sead::VtableSlot mInitializeRecord;
    sead::VtableSlot mInitializeBuffer;
    sead::VtableSlot mReleaseRecord;
    sead::VtableSlot mBeginUpdate;
    sead::VtableSlot mUpdateRecord;
    sead::VtableSlot mEvaluateRecord;
    sead::VtableSlot mBeginFinish;
    sead::VtableSlot mFinishRecord;
    sead::VtableSlot mBeginUpload;
    sead::VtableSlot mPrepareUpload;
    sead::VtableSlot mWriteUpload;
    sead::VtableSlot mBeginSubmit;
    sead::VtableSlot mSubmitRecord;
};
struct SelectionValue_l {
    be<u32> mFlags;
    UniformBuffer mBuffer;
    u8 mPrefix[0x10];
    be<f32> mColor[4];
    u8 mTail[0x1C];
};
struct ExtendedSelectionValue_l {
    be<u32> mFlags;
    u8 mPrefix[0x48];
    UniformBuffer mBuffer;
};
struct RenderSelectionRecord_l {
    be<u16> mFlags;
    u8 mReserved[2];
    be<u32> mSelectionMask;
    u8 mPrefix[0x10];
    be<f32> mPrimaryColor[4];
    be<f32> mSecondaryColor[4];
    u8 mTail[8];
    be<s32> mValueCount;
    gptr<SelectionValue_l> mValues;
};
struct RenderSelection_l {
    gptr<RenderSelectionDispatch_l> mMethods;
    be<u32> mState;
    be<u32> mRecordCount;
    gptr<RenderSelectionRecord_l> mRecords;
    be<u32> mActiveCount;
    be<u32> mSlotCount;
    gptr<be<u32>> mSlots;

    void setActiveCount(u32 count);
    u32 activeCount() const;
    u32 isRecordEnabled(u32 index) const;
    void setRecordMask(u32 index, u32 enabled);
    u32 testRecordMask(u32 index, u32 bit) const;
    void initialize(u32 state, s32 recordCount, s32 slotCount, sead::AllocationHeap* heap);
    u32 evaluateSlot(u32 slot, void* context, u32 outputIndex);
    void uploadSlot(u32 slot, void* context, u32 invalidate);
    void submitSlot(u32 slot, void* context, void* argument);
    void releaseRecords();
    void updateActiveRecords();
    void finishActiveRecords();
    // Empty virtual copies are distinguished only by their own ABI slots.
    void emptyHookA();
    void emptyHookB();
    void emptyHookC();
    void emptyHookD();
    void emptyHookE();
    void emptyHookF();
    void emptyHookG();
    u32 unavailableHook() const;
    u32 matchesRuntimeType(const sead::RuntimeType* type) const;
};
// A distinct record specialization established by the later initializer: stride 0x60.
struct ExtendedSelectionRecord_l {
    be<u8> mFlags;
    u8 mReserved[3];
    be<u32> mSelectionMask;
    u8 mPrefix[0x18];
    be<f32> mPrimaryColor[4];
    be<f32> mSecondaryColor[4];
    u8 mTail[0x18];
    be<s32> mValueCount;
    gptr<ExtendedSelectionValue_l> mValues;
};
struct ExtendedRenderSelection_l {
    gptr<RenderSelectionDispatch_l> mMethods;
    be<u32> mState;
    be<u32> mRecordCount;
    gptr<ExtendedSelectionRecord_l> mRecords;
    be<u32> mActiveCount;
    be<u32> mSlotCount;
    gptr<be<u32>> mSlots;
    void initialize(u32 state, s32 recordCount, s32 slotCount, sead::AllocationHeap* heap);
    u32 evaluateSlot(u32 slot, void* context, u32 outputIndex);
    void uploadSlot(u32 slot, void* context, u32 invalidate);
    void submitSlot(u32 slot, void* context, void* argument);
    void releaseRecords();
    void updateActiveRecords();
    void finishActiveRecords();
};
namespace collection_selection_text {
struct Text : sead::SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBufferA : sead::SafeTextBuffer { void destroy(u32 flags); void assureTerminationImpl(); };
struct TextBufferB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct TextBufferC : sead::SafeTextBuffer { void destroy(u32 flags); };
struct TextBufferD : sead::SafeTextBuffer { void destroy(u32 flags); };
struct OwnedText : sead::SafeText { void destroy(u32 flags); };
}
namespace render_selection_text {
// These are per-TU text interface copies; unknown fixed-buffer capacities are not inferred.
struct Text : sead::SafeText {
    void destroy(u32 flags);
    void assureTerminationImpl();
};
struct TextBufferA : sead::SafeTextBuffer {
    void assureTerminationImpl();
    void destroy(u32 flags);
};
struct TextBufferB : sead::SafeTextBuffer { void destroy(u32 flags); };
struct TextBufferC : sead::SafeTextBuffer { void destroy(u32 flags); };
}
}
WWHD_SIZE(agl::RenderSelectionRecord_l, 0x48);
WWHD_SIZE(agl::RenderSelectionDispatch_l, 0x148);
WWHD_OFFSET(agl::RenderSelectionDispatch_l, mIsRecordEnabled, 0xA0);
WWHD_OFFSET(agl::RenderSelectionDispatch_l, mBeginUpdate, 0xF8);
WWHD_OFFSET(agl::RenderSelectionDispatch_l, mUpdateRecord, 0x100);
WWHD_OFFSET(agl::RenderSelectionDispatch_l, mBeginFinish, 0x110);
WWHD_OFFSET(agl::RenderSelectionDispatch_l, mFinishRecord, 0x118);
WWHD_OFFSET(agl::RenderSelectionRecord_l, mFlags, 0);
WWHD_OFFSET(agl::RenderSelectionRecord_l, mSelectionMask, 4);
WWHD_SIZE(agl::RenderSelection_l, 0x1C);
WWHD_OFFSET(agl::RenderSelection_l, mMethods, 0);
WWHD_OFFSET(agl::RenderSelection_l, mRecordCount, 8);
WWHD_OFFSET(agl::RenderSelection_l, mRecords, 0xC);
WWHD_OFFSET(agl::RenderSelection_l, mActiveCount, 0x10);
WWHD_OFFSET(agl::RenderSelection_l, mSlotCount, 0x14);
WWHD_OFFSET(agl::RenderSelection_l, mSlots, 0x18);

WWHD_SIZE(agl::ExtendedSelectionRecord_l, 0x60);
WWHD_SIZE(agl::ExtendedRenderSelection_l, 0x1C);
WWHD_OFFSET(agl::ExtendedRenderSelection_l, mMethods, 0);
WWHD_OFFSET(agl::ExtendedRenderSelection_l, mRecordCount, 8);
WWHD_OFFSET(agl::ExtendedRenderSelection_l, mRecords, 0xC);
WWHD_OFFSET(agl::ExtendedRenderSelection_l, mActiveCount, 0x10);
WWHD_OFFSET(agl::ExtendedRenderSelection_l, mSlots, 0x18);

WWHD_SIZE(agl::SelectionValue_l, 0x5C);
WWHD_OFFSET(agl::SelectionValue_l, mBuffer, 4);
WWHD_SIZE(agl::ExtendedSelectionValue_l, 0x68);
WWHD_OFFSET(agl::ExtendedSelectionValue_l, mBuffer, 0x4C);
WWHD_OFFSET(agl::RenderSelectionRecord_l, mValueCount, 0x40);
WWHD_OFFSET(agl::RenderSelectionRecord_l, mValues, 0x44);
WWHD_OFFSET(agl::ExtendedSelectionRecord_l, mValueCount, 0x58);
WWHD_OFFSET(agl::ExtendedSelectionRecord_l, mValues, 0x5C);


WWHD_OFFSET(agl::RenderSelectionRecord_l, mPrimaryColor, 0x18);
WWHD_OFFSET(agl::RenderSelectionRecord_l, mSecondaryColor, 0x28);
WWHD_OFFSET(agl::ExtendedSelectionRecord_l, mPrimaryColor, 0x20);
WWHD_OFFSET(agl::ExtendedSelectionRecord_l, mSecondaryColor, 0x30);
WWHD_OFFSET(agl::SelectionValue_l, mColor, 0x30);

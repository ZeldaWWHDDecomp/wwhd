/* HD animation accumulation and solid-heap scope helpers. */
#pragma once
#include "bindings.h"

struct ExtBlendRecord {
    be<u32> mFlags;
    be<f32> mScale[3];
    be<f32> mTranslation[3];
    u8 padding[4];
    Quaternion mRotation;
};
struct ExtBlendResult {
    be<u32> mFlags;
    be<f32> mScale[3];
    be<f32> mTranslation[3];
    be<f32> mWeight;
    be<f32> mRotation[2][3];
};
struct ExtBlendAnimation {
    u8 padding[0x2C];
    gptr<ExtBlendRecord> mRecords;
    u8 padding2[8];
    gptr<be<u32>> mBindings;
    u8 padding3[4];
    be<u16> mCount;
    u8 padding4[0xE];
    be<u32> mFlags;
};
struct ExtBlendDestination {
    gptr<ExtBlendResult> mResults;
    u8 padding[4];
    be<u32> mFlags;
};
struct ExtHeap {
    u8 padding[0xC];
    gptr<gfn> mVtable;
    be<u32> mDebugName;
};
struct ExtSolidHeapScope {
    gptr<ExtHeap> mpHeap;
    void destroy();
};
WWHD_SIZE(ExtBlendRecord, 0x30);
WWHD_SIZE(ExtBlendResult, 0x38);
WWHD_OFFSET(ExtBlendAnimation, mBindings, 0x38);
WWHD_OFFSET(ExtBlendAnimation, mCount, 0x40);
WWHD_OFFSET(ExtBlendAnimation, mFlags, 0x50);
WWHD_SIZE(ExtSolidHeapScope, 4);
enum ExtBlendFlags : u32 {
    ExtBlend_Accumulated = 8,
    ExtBlend_BindingDisabled = 0x80000000,
    ExtBlend_BindingIndex = 0x7FFF,
};
enum ExtHeapConstants : u32 {
    ExtHeap_SolidKind = 0x534C4944,
    ExtHeap_FreeSlot = 15,
    ExtHeap_DefaultAlignment = 0x100,
    ExtHeap_DeleteStorage = 1,
};
ExtHeap* extGetParentHeap(ExtHeap*);
u32 extGetHeapKind(ExtHeap*);
void extDestroySolidHeap(ExtHeap*);
ExtSolidHeapScope* extAllocateScope(u32);
ExtHeap* extCreateSolidHeap(u32, ExtHeap*, u32);
void extRestoreCurrentHeap(ExtSolidHeapScope*);
void extAdjustSolidHeap(ExtHeap*);
void extDeleteScope(ExtSolidHeapScope*);

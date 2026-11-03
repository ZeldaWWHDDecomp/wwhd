// J3D HD renderer: resource binding layouts (resource payloads stay opaque).
// Names are our own; written from the WWHD binary.
#pragma once
#include "JSystem/J3DGraph/RenderGeometry.h"
namespace j3d {
struct RenderMaterialNameEntry_l {
    u8 mPrefix[8];
    be<u32> mCallback;
    u8 mGap[4];
    be<s32> mNameOffset;
};
struct RenderMaterialNames_l {
    u8 mPrefix[0x12];
    be<u16> mCount;
    u8 mPadding[0x20];
    be<s32> mEntriesOffset;
    void bindTextureMatrixCallbacks();
};
struct RenderResourceBinding;
struct RenderResourceChildren_l { be<s32> mCount; gptr<RenderResourceBinding> mRecords; };
struct RenderResourceBinding {
    gptr<void> mResource;
    RenderResourceSpan mFields[3];
    gptr<RenderResourceChildren_l> mChildren;
    be<u8> mActive, mBorrowedChildren;
    u8 mPadding[2];
    void reset();
    void activate(void* resource, sead::RegistryHeap* heap);
};
struct RenderGeometryBindings {
    gptr<RenderAttributeLocations_l> mLocations;
    RenderResourceSpan mSets[2];
    void prepare(RenderAttributeLocations_l* locations, RenderGeometryPool* geometry, sead::RegistryHeap* heap);
};
struct RenderResourceBindingGroups {
    be<u32> mCount;
    RenderResourceSpan mGroups;
    void prepare(RenderResourceBinding* resource, sead::RegistryHeap* heap);
};
}
WWHD_SIZE(j3d::RenderResourceBinding, 0x24);
WWHD_OFFSET(j3d::RenderResourceBinding, mChildren, 0x1C);
WWHD_OFFSET(j3d::RenderResourceBinding, mActive, 0x20);
WWHD_SIZE(j3d::RenderResourceChildren_l, 8);
WWHD_SIZE(j3d::RenderGeometryBindings, 20);
WWHD_SIZE(j3d::RenderResourceBindingGroups, 12);

WWHD_SIZE(j3d::RenderMaterialNameEntry_l, 20);
WWHD_OFFSET(j3d::RenderMaterialNames_l, mEntriesOffset, 0x34);

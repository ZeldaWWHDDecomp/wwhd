// Environment reference storage and its per-class ranges, from local constructors and array accesses.
#pragma once
#include "agl/EnvironmentClassRegistry.h"
namespace agl { namespace env {
struct EnvironmentReference { gptr<EnvironmentObject> mObject; };
struct EnvironmentReferenceRange { be<u16> mFirst; be<u16> mCount; };
struct EnvironmentReferenceCapacity {
    be<u32> mCounts[128];
    be<u32> mTotalCount;
    gptr<sead::VtableSlot> mMethods;
    static EnvironmentReferenceCapacity* construct(EnvironmentReferenceCapacity* object);
    void set(u32 typeId,u32 count);
};
struct EnvironmentRangeBuffer { be<s32> mCount; gptr<EnvironmentReferenceRange> mData; };
struct EnvironmentReferenceBuffer {
    be<s32> mCount;
    gptr<EnvironmentReference> mData;
    void sortRange(u32 compare,u32 first,u32 last);
};
struct EnvironmentReferenceList {
    gptr<EnvironmentObject> mOwner;
    be<u32> mReserved;
    EnvironmentRangeBuffer mRanges;
    EnvironmentReferenceBuffer mReferences;
    be<u32> mOtherState;
    gptr<sead::VtableSlot> mMethods;
    static EnvironmentReferenceList* construct(EnvironmentReferenceList* object);
    void initialize(const EnvironmentReferenceCapacity* capacity,sead::RegistryHeap* heap);
    void sortClassRange(u32 typeId);
    s32 classForReference(s32 referenceIndex) const;
    void clear();
    void destroy(u32 flags);
    static s32 compareReferences(const EnvironmentReference* left,const EnvironmentReference* right);
};
}}
WWHD_SIZE(agl::env::EnvironmentReference,4);
WWHD_SIZE(agl::env::EnvironmentReferenceRange,4);
WWHD_SIZE(agl::env::EnvironmentReferenceCapacity,520);
WWHD_SIZE(agl::env::EnvironmentRangeBuffer,8);
WWHD_SIZE(agl::env::EnvironmentReferenceBuffer,8);
WWHD_SIZE(agl::env::EnvironmentReferenceList,32);
WWHD_OFFSET(agl::env::EnvironmentReferenceCapacity,mTotalCount,0x200);
WWHD_OFFSET(agl::env::EnvironmentReferenceList,mRanges,8);
WWHD_OFFSET(agl::env::EnvironmentReferenceList,mReferences,16);
WWHD_OFFSET(agl::env::EnvironmentReferenceList,mMethods,28);

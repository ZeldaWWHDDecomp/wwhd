// Environment-set storage from its 0x190-byte constructor and 0xE4-byte record accesses.
#pragma once
#include "agl/EnvironmentReferences.h"
#include "agl/ParameterTree.h"
namespace agl { namespace env {
struct EnvironmentManager;
struct EnvironmentBlendListReference;
// The set's parameter lists: parameter-tree nodes with this class's own hooks.
struct EnvironmentSetParameterObject : parameter_tree::Node {
    u32 supportsParameters() const;
    void beforeResourceRead();
    u32 matchesClassResource(const EnvironmentBlendListReference* resource) const;
    void afterResourceRead();
    void onUpdateComplete();
    u32 resetSetRecordBindings();
    void refreshSetRecordBindings();
};
// The set's own parameter object: a parameter-tree value with this class's own hooks.
struct EnvironmentSetOwner : parameter_tree::Value {
    u32 supportsParameters() const;
    void beforeResourceRead();
    u32 acceptsResourceLists() const;
    void afterResourceRead();
    u32 matchesResource(const EnvironmentResourceReference* resource) const;
};
struct EnvironmentSetRecordIo_l {
    ParameterBinding mBinding;
    sead::SafeTextBuffer mName;
    u8 mOtherState[0x20];
    be<s32> mReferenceIndex;
    u8 mTail[8];
    gptr<void> mSetOwner;
    be<u32> mOtherOwnerState;
    be<u32> mTypeId;
    EnvironmentTextParameter mTypeName;
};
struct EnvironmentSetRecordOwnerView_l {
    gptr<sead::VtableSlot> mMethods;
    void recordChanged(EnvironmentSetRecordIo_l* record);
};
// The binary parameter-archive nodes link by byte offsets relative to the node itself.
// The arithmetic is on 32-bit guest addresses (it wraps like the console's), also from a NULL base.
template <class T, class U> inline T* relativeNode(const U* base, u32 offset) {
    u32 address = gabi::ea(base) + offset;
    return address != 0 ? reinterpret_cast<T*>(PPC_MEM_BASE + address) : nullptr;
}
struct EnvironmentSetResourceEntry {
    be<u32> mNextOffset;
    be<u32> mReserved;
    be<u32> mNameHash;
    EnvironmentSetResourceEntry* next() const { return relativeNode<EnvironmentSetResourceEntry>(this, u32(mNextOffset)); }
    be<u8>* text() const { return relativeNode<be<u8>>(this, sizeof(EnvironmentSetResourceEntry)); }
};
// An object record of a parameter archive: byte step to the next object, its parameter count and name hash; its
// parameter entries follow the record.
struct EnvironmentObjectRecord {
    be<u32> mNextOffset;
    be<u32> mParameterCount;
    be<u32> mNameHash;
    be<u32> mReserved;
    EnvironmentObjectRecord* next() const { return relativeNode<EnvironmentObjectRecord>(this, u32(mNextOffset)); }
    EnvironmentSetResourceEntry* firstParameter() const { return relativeNode<EnvironmentSetResourceEntry>(this, sizeof(EnvironmentObjectRecord)); }
};
struct EnvironmentSetResourceReference { gptr<EnvironmentObjectRecord> mObject; };
// One record per referenced object: a parameter-tree value (the record's parameter object) and its I/O binding.
struct EnvironmentSetRecord : parameter_tree::Value {
    EnvironmentSetRecordIo_l mIo;
    u32 matchesResource(const EnvironmentSetResourceReference* resource) const;
};
struct EnvironmentSetRecordBuffer { be<s32> mCount; gptr<EnvironmentSetRecord> mData; };
struct EnvironmentSet {
    EnvironmentReferenceList mReferences;
    gptr<sead::VtableSlot> mMethods;
    EnvironmentSetParameterObject mParameters;
    gptr<sead::VtableSlot> mIoMethods;
    EnvironmentTextParameter mName;
    EnvironmentSetParameterObject mSecondaryParameters;
    gptr<EnvironmentManager> mManager;
    EnvironmentSetOwner mOwner;
    EnvironmentSetRecordBuffer mRecords;
    static EnvironmentSet* construct(EnvironmentSet* object);
    void bindManager(EnvironmentManager* manager);
    void initialize(const EnvironmentReferenceCapacity* capacity,sead::RegistryHeap* heap);
    void destroy(u32 flags);
    u32 insertObject(EnvironmentObject* object);
    void recordChanged(EnvironmentSetRecordIo_l* record);
    u32 resetRecordBindings();
    void refreshRecordBindings();
};
}}
WWHD_SIZE(agl::env::EnvironmentSetParameterObject,0x64);
WWHD_SIZE(agl::env::EnvironmentSetOwner,0x5C);
WWHD_SIZE(agl::env::EnvironmentSetRecord,0xE4);
WWHD_SIZE(agl::env::EnvironmentSet,0x190);
WWHD_OFFSET(agl::env::EnvironmentSet,mParameters,0x24);
WWHD_OFFSET(agl::env::EnvironmentSet,mIoMethods,0x88);
WWHD_OFFSET(agl::env::EnvironmentSet,mName,0x8C);
WWHD_OFFSET(agl::env::EnvironmentSet,mSecondaryParameters,0xC4);
WWHD_OFFSET(agl::env::EnvironmentSet,mManager,0x128);
WWHD_OFFSET(agl::env::EnvironmentSet,mOwner,0x12C);
WWHD_OFFSET(agl::env::EnvironmentSet,mRecords,0x188);
WWHD_OFFSET(agl::env::EnvironmentSetRecord,mIo,0x5C);
WWHD_OFFSET(agl::env::EnvironmentSetRecordIo_l,mName,0xC);
WWHD_OFFSET(agl::env::EnvironmentSetRecordIo_l,mReferenceIndex,0x38);
WWHD_SIZE(agl::env::EnvironmentSetRecordIo_l,0x88);
WWHD_OFFSET(agl::env::EnvironmentSetRecordIo_l,mTypeName,0x50);
WWHD_SIZE(agl::env::EnvironmentSetResourceEntry,12);
WWHD_SIZE(agl::env::EnvironmentObjectRecord,16);
WWHD_OFFSET(agl::env::EnvironmentObjectRecord,mNameHash,8);
WWHD_OFFSET(agl::env::EnvironmentSetRecordIo_l,mSetOwner,0x44);
WWHD_OFFSET(agl::env::EnvironmentSetRecordIo_l,mTypeId,0x4C);
WWHD_SIZE(agl::env::EnvironmentSetRecordOwnerView_l,4);

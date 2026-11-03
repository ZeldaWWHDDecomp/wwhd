// Node storage and list traversal derived from assigned WWHD construction and accessors.
#pragma once
#include "sead/FileDevice.h"
#include "sead/HeapRegistry.h"
namespace agl {
sead::Disposer* createHierarchyDisposer(void* heap);
void destroyHierarchyDisposer(sead::Disposer* disposer, u32 flags);
void deleteHierarchyStorage(void* storage, u32 flags);
struct HierarchyItemDispatch { u8 mPrefix[0x14]; be<u32> mDestroy; };
struct HierarchyItem {
    gptr<HierarchyItemDispatch> mTable;
    be<u32> mSecondWord;
    be<u32> mFirstWord;
    sead::FixedSafeText<32> mName;
    sead::FixedSafeText<256> mDisplay;
    static HierarchyItem* create(HierarchyItem* item);
    void resetDisplay(u32 first, u32 second);
    void setName(sead::SafeText* name);
};
s32 compareHierarchyItemNames(HierarchyItem* first, HierarchyItem* second);
struct HierarchyItemBuffer { be<s32> mCount; gptr<HierarchyItem> mData; };
struct HierarchyEntryList { sead::LinkList mList; be<s32> mLinkOffset; };
// The owner and its leaf children have distinct dispatch interfaces.
struct HierarchyNodeDispatch {
    u8 mPrefix[0x1C]; be<u32> mNameAt;
    be<u32> mCountAdjustment; be<u32> mChildCount;
    be<u32> mRebuildAdjustment; be<u32> mRebuild;
    be<u32> mUpdateAdjustment; be<u32> mUpdate;
};
struct HierarchyLeafDispatch {
    u8 mPrefix[0x1C]; be<u32> mName;
    be<u32> mGroupAdjustment; be<u32> mGroup;
    be<u32> mPriorityAdjustment; be<u32> mPriority;
    be<u32> mUpdateAdjustment; be<u32> mUpdate;
};
// Only the leaf's dispatch word is established here; its complete derived size is not asserted.
struct HierarchyLeafPrefix { gptr<HierarchyLeafDispatch> mTable; };
s32 compareHierarchyLeaves(HierarchyLeafPrefix* first, HierarchyLeafPrefix* second);
struct HierarchyNode {
    gptr<HierarchyNodeDispatch> mTable;
    sead::PointerBuffer mChildren;
    HierarchyItemBuffer mItems;
    sead::PointerBuffer mEntries;
    HierarchyEntryList mLinkedEntries;
    be<s32> mSelection;
    be<u16> mFlags; u8 mPadding[2];
    static HierarchyNode* create(HierarchyNode* node);
    void destroy(u32 flags);
    void allocateEntries(s32 childCapacity, s32 itemCount, sead::RegistryHeap* heap);
    void rebuildEntries();
    void updateChildrenMatchingSelection(); void updateChildrenMatchingEntries();
    void appendChild(HierarchyLeafPrefix* child);
    sead::SafeText* childNameAt(u32 index); u32 childCount();
    void processLinkedEntries(); void clearFirstFlag();
};
}
WWHD_SIZE(agl::HierarchyEntryList, 16);
WWHD_OFFSET(agl::HierarchyNode, mChildren, 4);
WWHD_OFFSET(agl::HierarchyNode, mItems, 0x10);
WWHD_OFFSET(agl::HierarchyNode, mEntries, 0x18);
WWHD_OFFSET(agl::HierarchyNode, mLinkedEntries, 0x24);
WWHD_OFFSET(agl::HierarchyNode, mSelection, 0x34);
WWHD_OFFSET(agl::HierarchyNode, mFlags, 0x38);
WWHD_SIZE(agl::HierarchyNode, 0x3C);
WWHD_OFFSET(agl::HierarchyNodeDispatch, mNameAt, 0x1C);
WWHD_OFFSET(agl::HierarchyNodeDispatch, mChildCount, 0x24);
WWHD_OFFSET(agl::HierarchyNodeDispatch, mRebuild, 0x2C);

WWHD_OFFSET(agl::HierarchyItem, mName, 0xC);
WWHD_OFFSET(agl::HierarchyItem, mDisplay, 0x38);
WWHD_SIZE(agl::HierarchyItem, 0x144);
WWHD_SIZE(agl::HierarchyItemBuffer, 8);
WWHD_OFFSET(agl::HierarchyItemDispatch, mDestroy, 0x14);

WWHD_OFFSET(agl::HierarchyLeafDispatch, mName, 0x1C);
WWHD_OFFSET(agl::HierarchyLeafDispatch, mGroup, 0x24);
WWHD_OFFSET(agl::HierarchyLeafDispatch, mPriority, 0x2C);
WWHD_OFFSET(agl::HierarchyNodeDispatch, mUpdate, 0x34);

// AGL#9-owned pointerwords; HierarchyNode.cpp is their sole writer.
extern gptr<sead::Disposer>& sHierarchyDisposerSingleton;
extern gptr<sead::Disposer>& sHierarchyDisposerRegisteredSingleton;

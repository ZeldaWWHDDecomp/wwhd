// Hierarchy node operations reconstructed from this unit's WWHD instructions.
#include "bindings.h"
#include "agl/HierarchyNode.h"
#include "sead/HeapRegistry.h"
sead::RegistryHeap* findHierarchyArrayHeap(sead::RegistryManagerState*, void*);
sead::RegistryHeap* currentHierarchyArrayHeap(sead::RegistryManagerState*);
void* allocateHierarchyNodeStorage(u32);
void processHierarchyLinkedEntry(void*);
static u32 sHierarchyNodeTable;
static u32 sHierarchyDisposerTable;
// AGL#9-owned pointerwords, exported by reference in HierarchyNode.h.
typedef gptr<sead::Disposer> HierarchyDisposerPointer;
WWHD_SIZE(HierarchyDisposerPointer, 4);
static HierarchyDisposerPointer sHierarchyDisposerSingleton;
static HierarchyDisposerPointer sHierarchyDisposerRegisteredSingleton;
void* allocateHierarchyDisposerStorage(u32, void*, u32);
void initializeHierarchyDisposer(sead::Disposer*, void*, u32);
void finalizeHierarchyDisposer(sead::Disposer*, u32);
/* 0279B7B4 */
agl::HierarchyNode* agl::HierarchyNode::create(HierarchyNode* node) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (node == NULL) node = (HierarchyNode*)allocateHierarchyNodeStorage(sizeof(HierarchyNode));
    if (node == NULL) return node;
    node->mTable = reinterpret_cast<HierarchyNodeDispatch*>(&sHierarchyNodeTable);
    auto* children = gabi::at<sead::PointerBuffer>(gabi::ea(node) + offsetof(HierarchyNode, mChildren));
    if (children == NULL) children = (sead::PointerBuffer*)allocateHierarchyNodeStorage(sizeof(sead::PointerBuffer));
    if (children != NULL) {
        children->mCapacity = 0; children->mCount = 0; children->mStorage = NULL;
    }
    auto* items = gabi::at<HierarchyItemBuffer>(gabi::ea(node) + offsetof(HierarchyNode, mItems));
    if (items == NULL) items = (HierarchyItemBuffer*)allocateHierarchyNodeStorage(sizeof(HierarchyItemBuffer));
    if (items != NULL) { items->mData = NULL; items->mCount = 0; }
    auto* entries = gabi::at<sead::PointerBuffer>(gabi::ea(node) + offsetof(HierarchyNode, mEntries));
    if (entries == NULL) entries = (sead::PointerBuffer*)allocateHierarchyNodeStorage(sizeof(sead::PointerBuffer));
    if (entries != NULL) {
        entries->mCapacity = 0; entries->mCount = 0; entries->mStorage = NULL;
    }
    auto* linked = gabi::at<HierarchyEntryList>(gabi::ea(node) + offsetof(HierarchyNode, mLinkedEntries));
    if (linked == NULL) linked = (HierarchyEntryList*)allocateHierarchyNodeStorage(sizeof(HierarchyEntryList));
    if (linked != NULL) {
        linked->mList.mCount = 0;
        linked->mList.mPrevious = &linked->mList;
        linked->mLinkOffset = -1;
        linked->mList.mNext = &linked->mList;
    }
    node->mSelection = -1;
    auto* flags = gabi::at<be<u16>>(gabi::ea(node) + offsetof(HierarchyNode, mFlags));
    if (flags == NULL) flags = (be<u16>*)allocateHierarchyNodeStorage(2);
    if (flags != NULL) *flags = 1;
    node->mLinkedEntries.mLinkOffset = 0x3C;
    return node;
}
/* 0279B8D0 */
void agl::HierarchyNode::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mTable = reinterpret_cast<HierarchyNodeDispatch*>(&sHierarchyNodeTable);
    mChildren.release();
    HierarchyItem* data = mItems.mData;
    if (data != NULL) {
        s32 index = 0;
        while (index < (s32)mItems.mCount) {
            auto* item = gabi::at<HierarchyItem>(gabi::ea(data) + (u32)index * sizeof(HierarchyItem));
            gabi::call_ptr<void>(item->mTable->mDestroy, item, 2u);
            ++index;
            data = mItems.mData;
        }
        sead::RegistryHeap* heap = findHierarchyArrayHeap(sRegistryManager, data);
        void* storage = mItems.mData;
        gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, storage);
        mItems.mCount = 0; mItems.mData = NULL;
    }
    mEntries.release();
    mLinkedEntries.mList.clear();
    if ((flags & 1) != 0) operator_delete(this);
}
/* 0279BC70 */
void agl::HierarchyNode::allocateEntries(s32 childCapacity, s32 itemCount, sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    mChildren.allocate(childCapacity, heap, 4);
    if (itemCount > 0) {
        sead::RegistryHeap* allocationHeap = heap;
        if (allocationHeap == NULL) allocationHeap = currentHierarchyArrayHeap(sRegistryManager);
        auto* data = gabi::call_ptr<HierarchyItem*>(allocationHeap->dispatch()->mAllocate.mFunction,
            allocationHeap, (u32)itemCount * sizeof(HierarchyItem), 4u);
        u32 remaining = itemCount > 0 ? (u32)itemCount : 1;
        u32 offset = 0;
        do {
            auto* item = gabi::at<HierarchyItem>(gabi::ea(data) + offset);
            if (item != NULL) HierarchyItem::create(item);
            offset += sizeof(HierarchyItem);
        } while (--remaining != 0);
        if (data != NULL) { mItems.mData = data; mItems.mCount = itemCount; }
    }
    mEntries.allocate(itemCount, heap, 4);
    u32 count = (u32)(s32)mItems.mCount;
    u32 begin = gabi::ea((HierarchyItem*)mItems.mData);
    u32 end = begin + count * sizeof(HierarchyItem);
    for (u32 current = begin; current != end; current += sizeof(HierarchyItem)) {
        u32 index = (current - begin) / sizeof(HierarchyItem);
        gabi::at<HierarchyItem>(current)->resetDisplay(index, gabi::ea(this));
    }
}
/* 0279BD8C */
void agl::HierarchyNode::appendChild(HierarchyLeafPrefix* child) {
    s32 count = mChildren.mCount;
    s32 capacity = mChildren.mCapacity;
    if (count >= capacity) return;
    auto* storage = reinterpret_cast<gptr<HierarchyLeafPrefix>*>((void*)mChildren.mStorage);
    // Index formation uses the original word offset, including its unsigned wrapping.
    *gabi::at<gptr<HierarchyLeafPrefix>>(gabi::ea(storage) + ((u32)count << 2)) = child;
    mChildren.mCount = (u32)mChildren.mCount + 1;
}
/* 0279BDB8 */
void agl::HierarchyNode::processLinkedEntries() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 offset = (u32)(s32)mLinkedEntries.mLinkOffset;
    u32 sentinel = gabi::ea(&mLinkedEntries) - offset;
    u32 current = gabi::ea((sead::Link*)mLinkedEntries.mList.mNext) - offset;
    while (current != sentinel) {
        processHierarchyLinkedEntry(gabi::at<void>(current));
        auto* link = gabi::at<sead::Link>(current + offset);
        current = gabi::ea((sead::Link*)link->mNext) - offset;
    }
}
/* 0279BE24 */
void agl::HierarchyNode::clearFirstFlag() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (((u16)mFlags & 1) == 0) return;
    HierarchyNodeDispatch* table = mTable;
    gabi::call_ptr<void>(table->mRebuild, this);
    processLinkedEntries();
    mFlags = (u16)mFlags & 0xFFFE;
}
/* 0279C80C */
sead::SafeText* agl::HierarchyNode::childNameAt(u32 index) {
    gabi::AutoFrame frame{gabi::current_function()};
    HierarchyLeafPrefix* child = NULL;
    if (index < (u32)mChildren.mCount) {
        auto* slot = gabi::at<gptr<HierarchyLeafPrefix>>(gabi::ea((void*)mChildren.mStorage) + (index << 2));
        child = *slot;
    }
    HierarchyLeafDispatch* table = child->mTable;
    return gabi::call_ptr<sead::SafeText*>(table->mName, child);
}
/* 0279C83C */
u32 agl::HierarchyNode::childCount() { return (u32)mChildren.mCount; }
/* 0279BE80 */
s32 agl::compareHierarchyLeaves(HierarchyLeafPrefix* first, HierarchyLeafPrefix* second) {
    gabi::AutoFrame frame{gabi::current_function()};
    HierarchyLeafDispatch* firstTable = first->mTable;
    HierarchyLeafDispatch* secondTable = second->mTable;
    s32 firstPriority = gabi::call_ptr<s32>(firstTable->mPriority, first);
    s32 secondPriority = gabi::call_ptr<s32>(secondTable->mPriority, second);
    if (firstPriority < secondPriority) return -1;
    secondTable = second->mTable;
    firstTable = first->mTable;
    secondPriority = gabi::call_ptr<s32>(secondTable->mPriority, second);
    firstPriority = gabi::call_ptr<s32>(firstTable->mPriority, first);
    if (secondPriority < firstPriority) return 1;
    firstTable = first->mTable;
    secondTable = second->mTable;
    auto* firstName = gabi::call_ptr<sead::SafeText*>(firstTable->mName, first);
    auto* secondName = gabi::call_ptr<sead::SafeText*>(secondTable->mName, second);
    firstName->assureTermination();
    firstName->assureTermination();
    be<u8>* cached = firstName->mText;
    secondName->assureTermination();
    if (cached == (be<u8>*)secondName->mText) return 1;
    be<u8>* otherText = secondName->mText;
    for (u32 index = 0; index < (u32)sead::kMaxTextLength; ++index) {
        u8 otherByte = otherText[index];
        if (otherByte == 0) return 1;
        be<u8>* ownText = firstName->mText;
        u8 ownByte = ownText[index];
        if (ownByte == 0 || otherByte > ownByte) return -1;
        if (otherByte < ownByte) return 1;
    }
    return 1;
}
// Both callers compare bounded names after the same three termination dispatches.
static bool hierarchyItemMatchesText(agl::HierarchyItem* item, sead::SafeText* name) {
    item->mName.assureTermination();
    item->mName.assureTermination();
    be<u8>* cached = item->mName.mText;
    name->assureTermination();
    if (cached == (be<u8>*)name->mText) return true;
    be<u8>* first = item->mName.mText;
    be<u8>* second = name->mText;
    for (u32 index = 0; index <= (u32)sead::kMaxTextLength; ++index) {
        u8 left = first[index];
        u8 right = second[index];
        if (left != right) return false;
        if (left == 0) return true;
    }
    return false;
}
/* 0279C2AC */
void agl::HierarchyNode::rebuildEntries() {
    gabi::AutoFrame frame{gabi::current_function()};
    mChildren.heapSort(gabi::guest_fn(reinterpret_cast<const void*>(&compareHierarchyLeaves)));
    u32 childSlot = gabi::ea((void*)mChildren.mStorage);
    u32 childEnd = childSlot + ((u32)(s32)mChildren.mCount << 2);
    mEntries.mCount = 0;
    while (childSlot != childEnd) {
        u32 entryCount = (u32)(s32)mEntries.mCount;
        u32 entrySlot = gabi::ea((void*)mEntries.mStorage);
        u32 entryEnd = entrySlot + (entryCount << 2);
        bool found = false;
        while (entrySlot != entryEnd) {
            HierarchyLeafPrefix* child = *gabi::at<gptr<HierarchyLeafPrefix>>(childSlot);
            HierarchyLeafDispatch* table = child->mTable;
            HierarchyItem* item = *gabi::at<gptr<HierarchyItem>>(entrySlot);
            auto* name = gabi::call_ptr<sead::SafeText*>(table->mGroup, child);
            if (hierarchyItemMatchesText(item, name)) { found = true; break; }
            entryCount = (u32)(s32)mEntries.mCount;
            entrySlot += 4;
            entryEnd = gabi::ea((void*)mEntries.mStorage) + (entryCount << 2);
        }
        if (!found) {
            HierarchyItem* item = mItems.mData;
            if (entryCount < (u32)(s32)mItems.mCount)
                item = gabi::at<HierarchyItem>(gabi::ea(item) + entryCount * sizeof(HierarchyItem));
            HierarchyLeafPrefix* child = *gabi::at<gptr<HierarchyLeafPrefix>>(childSlot);
            auto* name = gabi::call_ptr<sead::SafeText*>(child->mTable->mGroup, child);
            item->setName(name);
            u32 count = (u32)(s32)mEntries.mCount;
            item = mItems.mData;
            if (count < (u32)(s32)mItems.mCount)
                item = gabi::at<HierarchyItem>(gabi::ea(item) + count * sizeof(HierarchyItem));
            if ((s32)count < (s32)mEntries.mCapacity) {
                u32 storage = gabi::ea((void*)mEntries.mStorage);
                *gabi::at<gptr<HierarchyItem>>(storage + (count << 2)) = item;
                mEntries.mCount = (u32)(s32)mEntries.mCount + 1;
            }
        }
        childSlot += 4;
    }
    mEntries.heapSort(gabi::guest_fn(reinterpret_cast<const void*>(&compareHierarchyItemNames)));
    // The original finishes with an unobserved finite cursor walk over the item buffer.
    u32 cursor = gabi::ea((HierarchyItem*)mItems.mData);
    u32 end = cursor + (u32)(s32)mItems.mCount * sizeof(HierarchyItem);
    while (cursor != end) cursor += sizeof(HierarchyItem);
}
/* 0279C494 */
void agl::HierarchyNode::updateChildrenMatchingSelection() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 current = gabi::ea((void*)mChildren.mStorage);
    u32 end = current + ((u32)(s32)mChildren.mCount << 2);
    while (current != end) {
        s32 selection = mSelection;
        HierarchyLeafPrefix* child = *gabi::at<gptr<HierarchyLeafPrefix>>(current);
        bool matches = true;
        if (selection != -1) {
            HierarchyLeafDispatch* table = child->mTable;
            HierarchyItem* item = mItems.mData;
            if ((u32)selection < (u32)(s32)mItems.mCount)
                item = gabi::at<HierarchyItem>(gabi::ea(item) + (u32)selection * sizeof(HierarchyItem));
            auto* name = gabi::call_ptr<sead::SafeText*>(table->mGroup, child);
            matches = hierarchyItemMatchesText(item, name);
            if (matches) child = *gabi::at<gptr<HierarchyLeafPrefix>>(current);
        }
        if (matches) {
            HierarchyLeafDispatch* table = child->mTable;
            // The update interface takes only the receiver; no incidental scratch register is forwarded.
            gabi::call_ptr<void>(table->mUpdate, child);
        }
        current += 4;
    }
}
/* 0279C5D0 */
void agl::HierarchyNode::updateChildrenMatchingEntries() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 current = gabi::ea((void*)mEntries.mStorage);
    u32 end = current + ((u32)(s32)mEntries.mCount << 2);
    u32 index = 0;
    while (current != end) {
        s32 selection = mSelection;
        if (selection == -1 || index == (u32)selection) {
            u32 childSlot = gabi::ea((void*)mChildren.mStorage);
            u32 childEnd = childSlot + ((u32)(s32)mChildren.mCount << 2);
            if (childSlot != childEnd) {
                do {
                    HierarchyLeafPrefix* child = *gabi::at<gptr<HierarchyLeafPrefix>>(childSlot);
                    HierarchyLeafDispatch* table = child->mTable;
                    HierarchyItem* item = *gabi::at<gptr<HierarchyItem>>(current);
                    auto* name = gabi::call_ptr<sead::SafeText*>(table->mGroup, child);
                    if (hierarchyItemMatchesText(item, name)) {
                        child = *gabi::at<gptr<HierarchyLeafPrefix>>(childSlot);
                        table = child->mTable;
                        gabi::call_ptr<void>(table->mUpdate, child);
                    }
                    childSlot += 4;
                } while (childSlot != childEnd);
                end = gabi::ea((void*)mEntries.mStorage) + ((u32)(s32)mEntries.mCount << 2);
            }
        }
        current += 4;
        ++index;
    }
}
/* 0279C728 */
void agl::deleteHierarchyStorage(void* storage, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (storage != NULL && (flags & 1) != 0) operator_delete(storage);
}
/* 0279C844 */
sead::Disposer* agl::createHierarchyDisposer(void* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::Disposer* disposer = sHierarchyDisposerSingleton;
    if (disposer != NULL) return disposer;
    disposer = (sead::Disposer*)allocateHierarchyDisposerStorage(sizeof(sead::Disposer), heap, 4);
    if (disposer != NULL) {
        initializeHierarchyDisposer(disposer, heap, 3);
        disposer->mVtable = reinterpret_cast<sead::DisposerVtable*>(&sHierarchyDisposerTable);
    }
    sHierarchyDisposerRegisteredSingleton = disposer;
    sHierarchyDisposerSingleton = disposer;
    return disposer;
}
/* 0279C8D0 */
void agl::destroyHierarchyDisposer(sead::Disposer* disposer, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (disposer == NULL) return;
    disposer->mVtable = reinterpret_cast<sead::DisposerVtable*>(&sHierarchyDisposerTable);
    if ((sead::Disposer*)sHierarchyDisposerRegisteredSingleton == disposer) {
        sHierarchyDisposerRegisteredSingleton = NULL;
        sHierarchyDisposerSingleton = NULL;
    }
    finalizeHierarchyDisposer(disposer, 0);
    if ((flags & 1) != 0) operator_delete(disposer);
}
/* WWHD glue
 ext 0273B7B4 void sead::PointerBuffer::heapSort(u32)
 ext 0273B584 void sead::PointerBuffer::allocate(s32, void*, s32)
 ext 02756140 sead::RegistryHeap* currentHierarchyArrayHeap(sead::RegistryManagerState*)
 ext 0273B5DC void sead::PointerBuffer::release()
 ext 0273B524 void sead::LinkList::clear()
 ext 02755FEC sead::RegistryHeap* findHierarchyArrayHeap(sead::RegistryManagerState*, void*)
 data sHierarchyNodeTable 10167800
 data sHierarchyDisposerTable 10167858
 data sHierarchyDisposerSingleton 101F8DCC
 data sHierarchyDisposerRegisteredSingleton 101F8DD0
 ext 0273B0D4 void* allocateHierarchyDisposerStorage(u32, void*, u32)
 ext 02752B0C void initializeHierarchyDisposer(sead::Disposer*, void*, u32)
 ext 02752BEC void finalizeHierarchyDisposer(sead::Disposer*, u32)
 ext 0273AD10 void* allocateHierarchyNodeStorage(u32)
 ext 027A80FC void processHierarchyLinkedEntry(void*)
*/

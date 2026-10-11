/** HD layout animation set: owned animation entries and active playback slots. No GameCube counterpart. */
#include "bindings.h"
#include "hd/hd_ui_02004A3C.h"
static u32 sAnimSetTypeInitialized;
static u32 sAnimSetParentInitialized;
static u32 sAnimSetRootInitialized;
static hdAnimSetType sAnimSetType;
static hdAnimSetType sAnimSetParent;
static hdAnimSetType sAnimSetRoot;
static hdAnimSetUiSystem* sAnimSetUiSystem;
static void* sAnimSetUiLock;
static void* sAnimSetHeapManager;

/* 02004A3C */
static hdAnimSetType* getAnimationSetType() {
    if (sAnimSetTypeInitialized != 0) return &sAnimSetType;
    u32 parentInitialized = sAnimSetParentInitialized;
    sAnimSetTypeInitialized = 1;
    if (parentInitialized != 0) {
        sAnimSetType.mParent = &sAnimSetParent;
        return &sAnimSetType;
    }
    u32 rootInitialized = sAnimSetRootInitialized;
    sAnimSetParentInitialized = 1;
    if (rootInitialized == 0) {
        sAnimSetRootInitialized = 1;
        sAnimSetRoot.mParent = NULL;
    }
    sAnimSetType.mParent = &sAnimSetParent;
    sAnimSetParent.mParent = &sAnimSetRoot;
    return &sAnimSetType;
}
/* 02004AC0 */
static hdAnimSet* constructAnimationSet(hdAnimSet* i_this) {
    if (i_this == NULL) {
        i_this = (hdAnimSet*)operator_new(sizeof(hdAnimSet));
        if (i_this == NULL) return NULL;
    }
    i_this->mLayout = NULL;
    i_this->mVtable = 0x10000970;
    hdAnimSetArray* entries = i_this->arrayForConstruction(offsetof(hdAnimSet, mEntries));
    if (entries == NULL) entries = (hdAnimSetArray*)operator_new(sizeof(hdAnimSetArray));
    if (entries != NULL) { entries->mData = NULL; entries->mCount = 0; }
    hdAnimSetArray* slots = i_this->arrayForConstruction(offsetof(hdAnimSet, mActiveSlots));
    if (slots == NULL) {
        slots = (hdAnimSetArray*)operator_new(sizeof(hdAnimSetArray));
        if (slots == NULL) return i_this;
    }
    slots->mData = NULL;
    slots->mCount = 0;
    return i_this;
}
/* 02004B5C */
static void destroyAnimationSet(hdAnimSet* i_this, s32 flags) {
    if (i_this == NULL) return;
    if (flags & 1) operator_delete(i_this);
}
static void* allocateAnimationSetArray(hdAnimSetHeap* heap, u32 size) {
    if (heap == NULL) heap = hdAnimSetGetCurrentHeap(sAnimSetHeapManager);
    return heap->allocate(size);
}
/* 02004B70 */
static u32 initializeAnimationSet(hdAnimSet* i_this, hdAnimSetLayout* layout, s32 entryCount, s32 slotCount) {
    i_this->mLayout = layout;
    hdAnimSetHeap* heap = sAnimSetUiSystem->mHeap;
    if (entryCount > 0) {
        void* data = allocateAnimationSetArray(heap, (u32)entryCount << 2);
        if (data != NULL) {
            i_this->mEntries.mData = (gptr<hdAnimSetEntry>*)data;
            i_this->mEntries.mCount = entryCount;
        }
    }
    s32 count = i_this->mEntries.mCount;
    for (s32 i = 0; i < count; ++i) *hdAnimSetArray::element(i_this->mEntries.mData, i) = NULL;
    if (slotCount > 0) {
        void* data = allocateAnimationSetArray(heap, (u32)slotCount << 2);
        if (data != NULL) {
            i_this->mActiveSlots.mData = (gptr<hdAnimSetEntry>*)data;
            i_this->mActiveSlots.mCount = slotCount;
        }
    }
    count = i_this->mActiveSlots.mCount;
    for (s32 i = 0; i < count; ++i) *hdAnimSetArray::element(i_this->mActiveSlots.mData, i) = NULL;
    return 1;
}
/* 02004CAC */
static void releaseAnimationSet(hdAnimSet* i_this) {
    s32 count = i_this->mEntries.mCount;
    gptr<hdAnimSetEntry>* data = i_this->mEntries.mData;
    for (s32 i = 0; i < count; ++i) {
        if (*hdAnimSetArray::at(data, count, i) == NULL) continue;
        hdAnimSetEntry* entry = *hdAnimSetArray::at(data, count, i);
        if (entry != NULL) {
            entry->destroy(3);
            count = i_this->mEntries.mCount;
            data = i_this->mEntries.mData;
        }
        *hdAnimSetArray::at(data, count, i) = NULL;
        count = i_this->mEntries.mCount;
        data = i_this->mEntries.mData;
    }
    if (data != NULL) {
        hdAnimSetHeap* heap = hdAnimSetFindHeap(sAnimSetHeapManager, data);
        heap->free(i_this->mEntries.mData);
        i_this->mEntries.mCount = 0;
        i_this->mEntries.mData = NULL;
    }
    gptr<hdAnimSetEntry>* slots = i_this->mActiveSlots.mData;
    if (slots != NULL) {
        hdAnimSetHeap* heap = hdAnimSetFindHeap(sAnimSetHeapManager, slots);
        heap->free(i_this->mActiveSlots.mData);
        i_this->mActiveSlots.mCount = 0;
        i_this->mActiveSlots.mData = NULL;
    }
}
static u32 makeAnimationSetEntry(hdAnimSet* i_this, u32 index, hdAnimSetString* name, hdAnimSetController* controller) {
    hdAnimSetHeap* heap = sAnimSetUiSystem->mHeap;
    gptr<hdAnimSetEntry>* slot = i_this->mEntries.at(index);
    hdAnimSetEntry* entry = (hdAnimSetEntry*)hdAnimSetAllocateEntry(sizeof(hdAnimSetEntry), heap, 4);
    if (entry != NULL) entry = hdAnimSetConstructEntry(entry, i_this->mLayout, name, controller);
    *slot = entry;
    return *i_this->mEntries.at(index) != NULL ? 1 : 0;
}
/* 02004E04 */
static u32 bindAnimationGroup(hdAnimSet* i_this, u32 index, hdAnimSetString* name, hdAnimSetString* group) {
    if (*i_this->mEntries.at(index) != NULL) return 0;
    hdAnimSetLock(sAnimSetUiLock);
    hdAnimSetStringTable* nameTable = name->mVtable;
    hdAnimSetLayout* layout = i_this->mLayout;
    gabi::call_ptr(nameTable->mAssureTermination.get(), name);
    hdAnimSetStringTable* groupTable = group->mVtable;
    const char* text = name->mText;
    gabi::call_ptr(groupTable->mAssureTermination.get(), group);
    void* paneGroup = hdAnimSetFindGroup(layout->mGroupContainer, group->mText);
    hdAnimSetController* controller = hdAnimSetBindGroup(layout, text, paneGroup, 0);
    hdAnimSetUnlock(sAnimSetUiLock);
    return makeAnimationSetEntry(i_this, index, name, controller);
}
/* 02004F50 */
static u32 bindAnimation(hdAnimSet* i_this, u32 index, hdAnimSetString* name) {
    if (*i_this->mEntries.at(index) != NULL) return 0;
    hdAnimSetLock(sAnimSetUiLock);
    name->assureTermination();
    hdAnimSetController* controller = hdAnimSetBindLayout(i_this->mLayout, name->mText, 0);
    hdAnimSetUnlock(sAnimSetUiLock);
    return makeAnimationSetEntry(i_this, index, name, controller);
}
/* 0200506C */
static u32 bindAnimationRange(hdAnimSet* i_this, u32 baseIndex, hdAnimSetString* name, u32 option, u32 count) {
    if (count == 0) return 1;
    for (u32 i = 0;; ++i) {
        if (*i_this->mEntries.at(baseIndex + i) != NULL) return 0;
        hdAnimSetLock(sAnimSetUiLock);
        hdAnimSetLayout* layout = i_this->mLayout;
        hdAnimSetController* controller = layout->bindRange(name, option, i);
        hdAnimSetUnlock(sAnimSetUiLock);
        if (makeAnimationSetEntry(i_this, baseIndex + i, name, controller) == 0) return 0;
        if (--count == 0) return 1;
    }
}
/* 020051C4 */
static void attachAnimation(hdAnimSet* i_this, u32 entryIndex, u32 slotIndex) {
    u32 entryCount = i_this->mEntries.mCount;
    gptr<hdAnimSetEntry>* entries = i_this->mEntries.mData;
    if (*hdAnimSetArray::at(entries, entryCount, entryIndex) == NULL) return;
    u32 slotCount = i_this->mActiveSlots.mCount;
    gptr<hdAnimSetEntry>* slots = i_this->mActiveSlots.mData;
    if (*hdAnimSetArray::at(slots, slotCount, slotIndex) != NULL &&
        (*hdAnimSetArray::at(slots, slotCount, slotIndex))->mController->mPlaying != 0) {
        hdAnimSetController* controller = (*hdAnimSetArray::at(slots, slotCount, slotIndex))->mController;
        controller->setPlaying(0);
        slotCount = i_this->mActiveSlots.mCount;
        entries = i_this->mEntries.mData;
        slots = i_this->mActiveSlots.mData;
        entryCount = i_this->mEntries.mCount;
    }
    *hdAnimSetArray::at(slots, slotCount, slotIndex) = *hdAnimSetArray::at(entries, entryCount, entryIndex);
    hdAnimSetEntry* entry = *i_this->mActiveSlots.at(slotIndex);
    hdAnimSetController* controller = entry->mController;
    controller->setPlaying(1);
}
/* 02005334 */
static void detachAnimation(hdAnimSet* i_this, u32 slotIndex) {
    u32 count = i_this->mActiveSlots.mCount;
    gptr<hdAnimSetEntry>* slots = i_this->mActiveSlots.mData;
    if (*hdAnimSetArray::at(slots, count, slotIndex) == NULL) return;
    hdAnimSetController* controller = (*hdAnimSetArray::at(slots, count, slotIndex))->mController;
    controller->setPlaying(0);
    *i_this->mActiveSlots.at(slotIndex) = NULL;
}
/* 020053E4 */
static void playAnimation(hdAnimSet* i_this, u32 entryIndex, u32 slotIndex, f32 rate) {
    if (*i_this->mEntries.at(entryIndex) == NULL) return;
    hdAnimSetAttach(i_this, entryIndex, slotIndex);
    hdAnimSetEntry* entry = *i_this->mActiveSlots.at(slotIndex);
    hdAnimSetPlayEntry(entry, rate);
}
/* 02005488 */
static void playAnimationAlternate(hdAnimSet* i_this, u32 entryIndex, u32 slotIndex, f32 rate) {
    if (*i_this->mEntries.at(entryIndex) == NULL) return;
    hdAnimSetAttach(i_this, entryIndex, slotIndex);
    hdAnimSetEntry* entry = *i_this->mActiveSlots.at(slotIndex);
    hdAnimSetPlayEntryAlternate(entry, rate);
}
/* 0200552C */
static void setAnimationFrame(hdAnimSet* i_this, u32 entryIndex, u32 slotIndex, f32 frame) {
    if (*i_this->mEntries.at(entryIndex) == NULL) return;
    hdAnimSetAttach(i_this, entryIndex, slotIndex);
    hdAnimSetEntry* entry = *i_this->mActiveSlots.at(slotIndex);
    entry->mController->setFrame(frame);
}
/* 020055E0 */
static void animationOperationC4(hdAnimSet* i_this, u32 entryIndex, u32 slotIndex) {
    if (*i_this->mEntries.at(entryIndex) == NULL) return;
    hdAnimSetAttach(i_this, entryIndex, slotIndex);
    hdAnimSetEntry* entry = *i_this->mActiveSlots.at(slotIndex);
    entry->mController->operationC4();
}
/* 02005674 */
static void animationOperationCC(hdAnimSet* i_this, u32 entryIndex, u32 slotIndex) {
    if (*i_this->mEntries.at(entryIndex) == NULL) return;
    hdAnimSetAttach(i_this, entryIndex, slotIndex);
    hdAnimSetEntry* entry = *i_this->mActiveSlots.at(slotIndex);
    entry->mController->operationCC();
}
/* 02005708 */
static void animationOperationD4(hdAnimSet* i_this, u32 entryIndex, u32 slotIndex) {
    if (*i_this->mEntries.at(entryIndex) == NULL) return;
    hdAnimSetAttach(i_this, entryIndex, slotIndex);
    hdAnimSetEntry* entry = *i_this->mActiveSlots.at(slotIndex);
    entry->mController->operationD4();
}
/* 0200579C */
static void setAnimationSpeed(hdAnimSet* i_this, u32 entryIndex, u32 slotIndex, f32 speed) {
    if (*i_this->mEntries.at(entryIndex) == NULL) return;
    hdAnimSetAttach(i_this, entryIndex, slotIndex);
    hdAnimSetEntry* entry = *i_this->mActiveSlots.at(slotIndex);
    entry->mController->mSpeed = speed;
}
/* 02005840 */
static u32 isAnimationEnd(hdAnimSet* i_this, u32 index) {
    hdAnimSetEntry* entry = *i_this->mEntries.at(index);
    if (entry == NULL) return 1;
    return hdAnimSetEntryIsEnd(entry);
}
/* 0200588C */
static u32 isAnimationStopped(hdAnimSet* i_this, u32 index) {
    hdAnimSetEntry* entry = *i_this->mEntries.at(index);
    if (entry == NULL) return 1;
    return (u32)entry->mController->mFlags & 1;
}
/* 020058E4 */
static void __sinit_hd_animation_set_cpp() {
    sinit_header_statics_rodata(0x101FF290, 0x1018C398, 0x101FF29C, 0x10000968);
}
/* WWHD glue (tools/verify/autoglue.py): HD-only animation set dependencies
 data sAnimSetTypeInitialized 101FD938
 data sAnimSetParentInitialized 101FD88C
 data sAnimSetRootInitialized 101FD888
 data sAnimSetType 101FD93C
 data sAnimSetParent 101FD940
 data sAnimSetRoot 101FD8FC
 data sAnimSetUiSystem 1018C404
 data sAnimSetUiLock 101F8B18
 data sAnimSetHeapManager 101F8B4C
 ext 02756140 hdAnimSetHeap* hdAnimSetGetCurrentHeap(void*)
 ext 02755FEC hdAnimSetHeap* hdAnimSetFindHeap(void*, void*)
 ext 0273B050 void* hdAnimSetAllocateEntry(u32, hdAnimSetHeap*, u32)
 ext 020034A0 hdAnimSetEntry* hdAnimSetConstructEntry(hdAnimSetEntry*, hdAnimSetLayout*, hdAnimSetString*, hdAnimSetController*)
 ext 0274FBF8 void hdAnimSetLock(void*)
 ext 0274FCCC void hdAnimSetUnlock(void*)
 ext 02874820 void* hdAnimSetFindGroup(void*, const char*)
 ext 0287F740 hdAnimSetController* hdAnimSetBindGroup(hdAnimSetLayout*, const char*, void*, u32)
 ext 0287F944 hdAnimSetController* hdAnimSetBindLayout(hdAnimSetLayout*, const char*, u32)
 ext 020051C4 void hdAnimSetAttach(hdAnimSet*, u32, u32)
 ext 0200361C void hdAnimSetPlayEntry(hdAnimSetEntry*, f32)
 ext 02003654 void hdAnimSetPlayEntryAlternate(hdAnimSetEntry*, f32)
 ext 0200368C u32 hdAnimSetEntryIsEnd(hdAnimSetEntry*)
*/

/** HD layout animation set; private views for its sead/NW4F objects. */
#pragma once
#include "wwhd.h"
struct hdAnimSetControllerTable {
    u8 _opaque00[0x24]; gfn mSetPlaying;
    u8 _opaque28[0x94]; gfn mSetFrame;
    u8 _opaqueC0[4]; gfn mOperationC4;
    u8 _opaqueC8[4]; gfn mOperationCC;
    u8 _opaqueD0[4]; gfn mOperationD4;
};
struct hdAnimSetController {
    u8 _opaque00[0xC]; be<f32> mSpeed; be<u8> mPlaying;
    u8 _opaque11[3]; gptr<hdAnimSetControllerTable> mVtable;
    u8 _opaque18[0x14]; be<u32> mFlags;
    void setPlaying(u32 playing) { gabi::call_ptr(mVtable->mSetPlaying.get(), this, playing); }
    void setFrame(f32 frame) { gabi::call_ptr(mVtable->mSetFrame.get(), this, frame); }
    // HD: these controller operations have no verified semantic names yet.
    void operationC4() { gabi::call_ptr(mVtable->mOperationC4.get(), this); }
    void operationCC() { gabi::call_ptr(mVtable->mOperationCC.get(), this); }
    void operationD4() { gabi::call_ptr(mVtable->mOperationD4.get(), this); }
};
struct hdAnimSetEntryTable { u8 _opaque00[0xC]; gfn mDestroy; };
struct hdAnimSetEntry {
    u8 _opaque00[0x34]; gptr<hdAnimSetController> mController; gptr<hdAnimSetEntryTable> mVtable;
    void destroy(u32 flags) { gabi::call_ptr(mVtable->mDestroy.get(), this, flags); }
};
WWHD_SIZE(hdAnimSetEntry, 0x3C);
struct hdAnimSetArray {
    be<u32> mCount; gptr<gptr<hdAnimSetEntry>> mData;
    static gptr<hdAnimSetEntry>* element(gptr<hdAnimSetEntry>* data, u32 index) {
        // HD: array element addresses wrap in the 32-bit guest space.
        return gabi::at<gptr<hdAnimSetEntry>>(gabi::ea(data) + index * sizeof(gptr<hdAnimSetEntry>));
    }
    static gptr<hdAnimSetEntry>* at(gptr<hdAnimSetEntry>* data, u32 count, u32 index) {
        // HD: sead SafeArray uses element zero for an out-of-range index.
        return element(data, index < count ? index : 0);
    }
    gptr<hdAnimSetEntry>* at(u32 index) { return at(mData, mCount, index); }
};
struct hdAnimSetLayoutTable { u8 _opaque00[0x94]; gfn mBindAnimationRange; };
struct hdAnimSetLayout {
    u8 _opaque00[0x10]; gptr<void> mGroupContainer;
    u8 _opaque14[0x1C]; gptr<hdAnimSetLayoutTable> mVtable;
    hdAnimSetController* bindRange(void* name, u32 option, u32 index) {
        return gabi::call_ptr<hdAnimSetController*>(mVtable->mBindAnimationRange.get(), this, name, option, index, 0);
    }
};
struct hdAnimSet {
    gptr<hdAnimSetLayout> mLayout; hdAnimSetArray mEntries; hdAnimSetArray mActiveSlots; be<u32> mVtable;
    hdAnimSetArray* arrayForConstruction(u32 offset) {
        // HD: self == -4/-12 makes one embedded SafeArray address wrap to NULL.
        return gabi::at<hdAnimSetArray>(gabi::ea(this) + offset);
    }
};
WWHD_SIZE(hdAnimSet, 0x18);
struct hdAnimSetStringTable { u8 _opaque00[0x14]; gfn mAssureTermination; };
struct hdAnimSetString {
    gptr<const char> mText; gptr<hdAnimSetStringTable> mVtable;
    void assureTermination() { gabi::call_ptr(mVtable->mAssureTermination.get(), this); }
};
struct hdAnimSetHeapTable { u8 _opaque00[0x34]; gfn mAllocate; u8 _opaque38[4]; gfn mFree; };
struct hdAnimSetHeap {
    u8 _opaque00[0xC]; gptr<hdAnimSetHeapTable> mVtable;
    void* allocate(u32 size) { return gabi::call_ptr<void*>(mVtable->mAllocate.get(), this, size, 4); }
    void free(void* memory) { gabi::call_ptr(mVtable->mFree.get(), this, memory); }
};
struct hdAnimSetUiSystem { u8 _opaque00[0x10]; gptr<hdAnimSetHeap> mHeap; };
struct hdAnimSetType { gptr<hdAnimSetType> mParent; };
hdAnimSetHeap* hdAnimSetGetCurrentHeap(void*);
hdAnimSetHeap* hdAnimSetFindHeap(void*, void*);
void* hdAnimSetAllocateEntry(u32, hdAnimSetHeap*, u32);
hdAnimSetEntry* hdAnimSetConstructEntry(hdAnimSetEntry*, hdAnimSetLayout*, hdAnimSetString*, hdAnimSetController*);
void hdAnimSetLock(void*);
void hdAnimSetUnlock(void*);
void* hdAnimSetFindGroup(void*, const char*);
hdAnimSetController* hdAnimSetBindGroup(hdAnimSetLayout*, const char*, void*, u32);
hdAnimSetController* hdAnimSetBindLayout(hdAnimSetLayout*, const char*, u32);
void hdAnimSetAttach(hdAnimSet*, u32, u32);
void hdAnimSetPlayEntry(hdAnimSetEntry*, f32);
void hdAnimSetPlayEntryAlternate(hdAnimSetEntry*, f32);
u32 hdAnimSetEntryIsEnd(hdAnimSetEntry*);
WWHD_SIZE(hdAnimSetControllerTable, 0xD8);
WWHD_OFFSET(hdAnimSetControllerTable, mSetPlaying, 0x24);
WWHD_OFFSET(hdAnimSetControllerTable, mSetFrame, 0xBC);
WWHD_SIZE(hdAnimSetController, 0x30);
WWHD_OFFSET(hdAnimSetController, mVtable, 0x14);
WWHD_OFFSET(hdAnimSetController, mFlags, 0x2C);
WWHD_SIZE(hdAnimSetEntryTable, 0x10);
WWHD_OFFSET(hdAnimSetEntry, mController, 0x34);
WWHD_SIZE(hdAnimSetArray, 8);
WWHD_SIZE(hdAnimSetLayoutTable, 0x98);
WWHD_SIZE(hdAnimSetLayout, 0x34);
WWHD_OFFSET(hdAnimSetLayout, mVtable, 0x30);
WWHD_OFFSET(hdAnimSet, mActiveSlots, 0xC);
WWHD_SIZE(hdAnimSetStringTable, 0x18);
WWHD_SIZE(hdAnimSetString, 8);
WWHD_SIZE(hdAnimSetHeapTable, 0x40);
WWHD_SIZE(hdAnimSetHeap, 0x10);
WWHD_SIZE(hdAnimSetUiSystem, 0x14);
WWHD_SIZE(hdAnimSetType, 4);

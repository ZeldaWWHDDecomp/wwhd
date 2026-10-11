/** HD layout screen and pane views. No GameCube counterpart; private decomp source. */
#pragma once
#include "wwhd.h"
struct hdScreenType { gptr<hdScreenType> mParent; };
struct hdScreenTextBox { u8 _opaque00[8]; be<u32> mVtable; u8 _opaque0C[0xF8]; };
struct hdScreenPicture { u8 _opaque00[8]; be<u32> mVtable; u8 _opaque0C[0xB4]; };
struct hdScreenDeleteTable { u8 _opaque00[0xC]; gfn mDestroy; };
struct hdScreenHolder {
    gptr<hdScreenDeleteTable> mVtable; gptr<void> mOwner; gptr<void> mLayout;
    void destroy(u32 flags) { gabi::call_ptr(mVtable->mDestroy.get(), this, flags); }
};
struct hdScreenPaneOwnerTable { u8 _opaque00[0xC]; gfn mGetRuntimeType; };
struct hdScreenPaneOwner {
    u8 _opaque00[0x30]; gptr<hdScreenPaneOwnerTable> mVtable; gptr<void> mTarget;
};
inline void* hdScreenReadTarget(hdScreenPaneOwner* owner) {
    // HD: failed DynamicCast still reads the target field through guest NULL.
    return gabi::at<gptr<void>>(gabi::ea(owner) + offsetof(hdScreenPaneOwner, mTarget))->get();
}
struct hdScreenResourceTable {
    u8 _opaque00[0x5C]; gfn mFindPane;
};
struct hdScreenResourceObject {
    u8 _opaque00[8]; gptr<hdScreenResourceTable> mVtable; be<u32> mBusy;
    void* findPane(const char* name) { return gabi::call_ptr<void*>(mVtable->mFindPane.get(), this, name, 1); }
};
struct hdScreenAccessorTable {
    u8 _opaque00[0x14]; gfn mDestroy;
    u8 _opaque18[0x34]; gfn mPrepareDraw; u8 _opaque50[4]; gfn mFinishDraw;
    u8 _opaque58[0xC]; gfn mDraw; u8 _opaque68[4]; gfn mDrawWithInfo;
};
struct hdScreenAccessor {
    u8 _opaque00[0xC]; gptr<hdScreenResourceObject> mObject;
    u8 _opaque10[0x20]; gptr<hdScreenAccessorTable> mVtable;
    void destroy(u32 flags) { gabi::call_ptr(mVtable->mDestroy.get(), this, flags); }
    void prepareDraw() { gabi::call_ptr(mVtable->mPrepareDraw.get(), this); }
    void finishDraw() { gabi::call_ptr(mVtable->mFinishDraw.get(), this); }
    void draw(void* object) { gabi::call_ptr(mVtable->mDraw.get(), this, object, 0); }
    void drawWithInfo(void* info) { gabi::call_ptr(mVtable->mDrawWithInfo.get(), this, info); }
};
inline u32 hdScreenAccessorBusy(hdScreenAccessor* accessor) {
    // HD: the busy read precedes the accessor NULL check in release/draw.
    hdScreenResourceObject* object = *gabi::at<gptr<hdScreenResourceObject>>(gabi::ea(accessor) + offsetof(hdScreenAccessor, mObject));
    return *gabi::at<be<u32>>(gabi::ea(object) + offsetof(hdScreenResourceObject, mBusy));
}
struct hdScreenAnimationSet {
    u8 _opaque00[0x14]; gptr<hdScreenDeleteTable> mVtable;
    void destroy(u32 flags) { gabi::call_ptr(mVtable->mDestroy.get(), this, flags); }
};
struct hdScreenLytObject {
    u8 _opaque00[0x40]; be<u32> mBlock[12];
    u8 _opaque70[0x38]; be<u32> mSystemWord; u8 _opaqueAC[0x1C];
};
struct hdScreenPaneArray {
    be<u32> mCount; gptr<gptr<void>> mData;
    static gptr<void>* element(gptr<void>* data, u32 index) {
        // HD: table indices and pointers retain 32-bit wrapping arithmetic.
        return gabi::at<gptr<void>>(gabi::ea(data) + index * sizeof(gptr<void>));
    }
};
struct hdScreenArchive { u8 _opaque00[0x10]; be<u8> mAvailable; };
struct hdScreenTable {
    u8 _opaque00[0x34]; gfn mCalc; u8 _opaque38[4]; gfn mInitialize;
    u8 _opaque40[4]; gfn mInitializeArchive; u8 _opaque48[4]; gfn mInitializeOptions;
    u8 _opaque50[4]; gfn mFinishArchiveSecond; u8 _opaque58[4]; gfn mFinishArchiveFirst;
};
struct hdLayoutScreen {
    be<u32> field_0x0; // HD: constructor clears this bookkeeping word; purpose unconfirmed.
    gptr<hdScreenAccessor> mAccessor; gptr<hdScreenHolder> mHolder; hdScreenLytObject mLytObject;
    gptr<hdScreenAnimationSet> mAnimationSet; hdScreenPaneArray mPanes; gptr<hdScreenTable> mVtable;
    hdScreenLytObject* lytObject() {
        return gabi::at<hdScreenLytObject>(gabi::ea(this) + offsetof(hdLayoutScreen, mLytObject));
    }
    hdScreenPaneArray* panesForConstruction() {
        // HD: this == -0xD8 makes the embedded SafeArray constructor receive NULL.
        return gabi::at<hdScreenPaneArray>(gabi::ea(this) + offsetof(hdLayoutScreen, mPanes));
    }
};
struct hdScreenStringTable { u8 _opaque00[0x14]; gfn mAssureTermination; };
struct hdScreenString {
    gptr<const char> mText; gptr<hdScreenStringTable> mVtable;
    void assureTermination() { gabi::call_ptr(mVtable->mAssureTermination.get(), this); }
};
struct hdScreenFixedString {
    gptr<be<u8>> mText; gptr<hdScreenStringTable> mVtable; be<u32> mCapacity; be<u8> mBuffer[64];
    void assureTermination() { gabi::call_ptr(mVtable->mAssureTermination.get(), this); }
    void terminate() {
        // HD: preserve capacity-zero and negative-capacity wrapping guest writes.
        *gabi::at<be<u8>>(gabi::ea(mText.get()) + (u32)mCapacity - 1) = 0;
    }
};
struct hdScreenHeapTable { u8 _opaque00[0x34]; gfn mAllocate; u8 _opaque38[4]; gfn mFree; };
struct hdScreenHeap {
    u8 _opaque00[0xC]; gptr<hdScreenHeapTable> mVtable;
    void* allocate(u32 size) { return gabi::call_ptr<void*>(mVtable->mAllocate.get(), this, size, 4); }
    void free(void* memory) { gabi::call_ptr(mVtable->mFree.get(), this, memory); }
};
struct hdScreenDrawInfo { be<u8> field_0x0, field_0x1; u8 _opaque02[6]; be<u32> mMode; };
struct hdScreenUiSystem { u8 _opaque00[0x10]; gptr<hdScreenHeap> mHeap; be<u32> mSystemWord; gptr<hdScreenDrawInfo> mDrawInfo; };
struct hdScreenNameTable { u8 _opaque00[4]; be<u16> mPaneCount; be<u16> mAnimationCount; };
struct hdScreenResource { u8 _opaque00[0x24]; hdScreenNameTable mNames; };
struct hdScreenPair { be<u32> mFirst; be<u32> mSecond; };
struct hdScreenPairSource { u8 _opaque00[0x1C]; be<u32> mSecond; be<u32> mFirst; };
void hdScreenConstructTextBoxBase(hdScreenTextBox*, u32, u32, u32);
void hdScreenConstructTextBoxResource(hdScreenTextBox*, u32);
void hdScreenDestroyTextBoxBase(hdScreenTextBox*, u32);
void hdScreenConstructPictureBase(hdScreenPicture*, u32, u32, u32);
void hdScreenDestroyPictureBase(hdScreenPicture*, u32);
void* hdScreenAllocate(u32, hdScreenHeap*, u32);
void* hdScreenConstructChild(void*, void*, hdScreenPaneOwner*, void*);
void hdScreenAddChild(void*, void*);
void hdScreenConstructLytObject(hdScreenLytObject*);
void hdScreenDestroyLytObject(hdScreenLytObject*, u32);
void* hdScreenConvertResource(void*);
void hdScreenSetLytResource(hdScreenLytObject*, void*);
void hdScreenSetResource(hdLayoutScreen*, void*);
void hdScreenSetBlock(hdLayoutScreen*, be<u32>*);
void hdScreenSetup(hdLayoutScreen*, void*, be<u32>*);
hdScreenHeap* hdScreenFindHeap(void*, void*);
hdScreenHeap* hdScreenGetCurrentHeap(void*);
void hdScreenReleaseAnimationSet(hdScreenAnimationSet*);
void hdScreenPrepareDrawInfo(hdScreenDrawInfo*);
hdScreenAccessor* hdScreenConstructAccessor(hdScreenAccessor*);
void hdScreenFormatName(hdScreenFixedString*, const char*, const char*);
hdScreenHolder* hdScreenConstructHolder(hdScreenHolder*, void*, hdScreenAccessor*);
u32 hdScreenLoadLayout(hdScreenAccessor*, const char*, u32, hdScreenHolder*, u32);
hdScreenAnimationSet* hdScreenConstructAnimationSet(hdScreenAnimationSet*);
u32 hdScreenInitializeAnimationSet(hdScreenAnimationSet*, hdScreenAccessor*, u32, u32);
const char* hdScreenPaneName(hdScreenNameTable*, u32);
const char* hdScreenAnimationName(hdScreenNameTable*, u32);
void hdScreenBindAnimation(hdScreenAnimationSet*, u32, hdScreenString*);
WWHD_SIZE(hdScreenType, 4);
WWHD_SIZE(hdScreenTextBox, 0x104);
WWHD_SIZE(hdScreenPicture, 0xC0);
WWHD_SIZE(hdScreenDeleteTable, 0x10);
WWHD_SIZE(hdScreenHolder, 0xC);
WWHD_SIZE(hdScreenPaneOwnerTable, 0x10);
WWHD_SIZE(hdScreenPaneOwner, 0x38);
WWHD_SIZE(hdScreenResourceTable, 0x60);
WWHD_SIZE(hdScreenResourceObject, 0x10);
WWHD_SIZE(hdScreenAccessorTable, 0x70);
WWHD_SIZE(hdScreenAccessor, 0x34);
WWHD_SIZE(hdScreenAnimationSet, 0x18);
WWHD_SIZE(hdScreenLytObject, 0xC8);
WWHD_SIZE(hdScreenPaneArray, 8);
WWHD_SIZE(hdScreenArchive, 0x11);
WWHD_SIZE(hdScreenTable, 0x60);
WWHD_SIZE(hdLayoutScreen, 0xE4);
WWHD_OFFSET(hdLayoutScreen, mAnimationSet, 0xD4);
WWHD_OFFSET(hdLayoutScreen, mPanes, 0xD8);
WWHD_OFFSET(hdScreenLytObject, mBlock, 0x40);
WWHD_OFFSET(hdScreenLytObject, mSystemWord, 0xA8);
WWHD_SIZE(hdScreenStringTable, 0x18);
WWHD_SIZE(hdScreenString, 8);
WWHD_SIZE(hdScreenFixedString, 0x4C);
WWHD_SIZE(hdScreenHeapTable, 0x40);
WWHD_SIZE(hdScreenHeap, 0x10);
WWHD_SIZE(hdScreenDrawInfo, 0xC);
WWHD_SIZE(hdScreenUiSystem, 0x1C);
WWHD_SIZE(hdScreenNameTable, 8);
WWHD_SIZE(hdScreenResource, 0x2C);
WWHD_SIZE(hdScreenPair, 8);
WWHD_SIZE(hdScreenPairSource, 0x24);

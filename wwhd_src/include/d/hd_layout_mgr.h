/* HD layout resource manager and framework companions. Private binary reconstruction. */
#pragma once
#include "bindings.h"
#include "hd/hd_ui_020021FC.h"
#include "d/hd_parts.h"
// Additional accessor entry used only by the layout-resource binding companion.
struct hdLayoutResourceBindingTable_c {
    u8 mPrefix[0x74];
    gfn mAttach;
};
/* hdPartsLayout_c (the parts' layout screen, 0x110) is defined in d/hd_parts.h */
struct hdLayoutLoop_c : hdParts_c {
    be<u32> mAnimationRequest;
    enum Request { Play, Stop, SetFrame, Reset, NoRequest };
    u32 initialize(void* resource, u32 first, u32 second, u32 third);
    u32 update();
    void applyRequest();
    u32 createLayout(void* resource, u32 first, u32 second, u32 third, u32 last);
};
struct hdLayoutCacheNode_c;
struct hdLayoutCachePool_c {
    gptr<hdLayoutCacheNode_c> mpRoot;
    gptr<hdLayoutCacheNode_c> mpFree;
    gptr<hdLayoutCacheNode_c> mpStorage;
    be<s32> mCount;
    be<s32> mCapacity;
};
struct hdLayoutCachedResource_c {
    gptr<hdScreenResource> mpArchive;
    hdScreenFixedString mName;
};
struct hdLayoutCacheNode_c {
    gptr<hdLayoutCacheNode_c> mpNext;
    be<u32> mUnused;
    be<u8> mRed;
    u8 mPadding[3];
    gptr<const char> mpName;
    gptr<void> mpStringTable;
    gptr<void> mpNodeTable;
    gptr<hdLayoutCachedResource_c> mpResource;
    gptr<hdLayoutCachePool_c> mpPool;
    be<u8> mNameBuffer[64];
    be<u8> mNameEnd;
    u8 mTail[3];
};
struct hdLayoutThreadTable_c {
    u8 mPrefix[0x1C];
    gfn mResume;
    u8 mGap[0xC];
    gfn mStart;
};
struct hdLayoutThread_c {
    u8 mPrefix[0xC];
    gptr<hdLayoutThreadTable_c> mpVtable;
};
struct hdLayoutHeapTable_c {
    u8 mPrefix[0x24];
    gfn mDestroy;
    u8 mGap[0x4C];
    gfn mBegin;
    u8 mPadding[4];
    gfn mEnd;
};
struct hdLayoutHeap_c {
    u8 mPrefix[0xC];
    gptr<hdLayoutHeapTable_c> mpVtable;
    void begin() {
        gabi::call_ptr(mpVtable->mBegin.get(), this);
    }
    void end() {
        gabi::call_ptr(mpVtable->mEnd.get(), this, 4u);
    }
};
struct hdLayoutResMgr_c {
    u8 mDisposer[0x10];
    gptr<hdLayoutHeap_c> mpHeap;
    gptr<hdLayoutHeap_c> mpResidentHeap;
    gptr<hdLayoutHeap_c> mpStaffRollHeap;
    gptr<hdLayoutThread_c> mpThread;
    be<u8> mBusy;
    u8 mPadding[3];
    hdLayoutCachePool_c mCache;
    hdLayoutCacheNode_c mNodes[1024];
    gptr<void> mpVtable;
    void init(hdLayoutHeap_c* heap);
    hdScreenResource* loadArchive(hdScreenString* name, hdLayoutHeap_c* heap);
    u32 cache(hdScreenString* name, hdLayoutHeap_c* heap);
    u32 unload(hdScreenString* name);
    void* find(hdScreenString* name);
    u8 isIdle();
    u32 resume();
    void createStaffRollHeap();
    void destroyStaffRollHeap();
};
struct hdLayoutTask_c {
    gptr<void> mpVtable;
    gptr<hdLayoutResMgr_c> mpOwner;
    be<s16> mAdjust;
    be<s16> mVirtualIndex;
    gfn mMethod;
};
struct hdLayoutUiSystem_c {
    u8 mPrefix[0x10];
    gptr<hdLayoutHeap_c> mpHeap;
};
struct hdLayoutUiManager_c {
    u8 mPrefix[0x2040];
    gptr<hdLayoutHeap_c> mpHeap;
};
struct hdLayoutDisposer_c {
    u8 mPrefix[0xC];
    gptr<void> mpVtable;
};
struct hdLayoutHeaderWords_c {
    be<u32> mWords[4];
};
struct hdLayoutHeaderRange_c {
    be<f32> mMin, mMax;
    be<u8> mSigned, mUnsigned;
    u8 mPadding[2];
};
struct hdLayoutRegistration_c {
    u8 mStorage[12];
};
WWHD_SIZE(hdLayoutCacheNode_c, 0x64);
WWHD_OFFSET(hdLayoutCacheNode_c, mpPool, 0x1C);
WWHD_OFFSET(hdLayoutCacheNode_c, mNameBuffer, 0x20);
WWHD_SIZE(hdLayoutCachePool_c, 0x14);
WWHD_SIZE(hdLayoutCachedResource_c, 0x50);
WWHD_OFFSET(hdLayoutResMgr_c, mCache, 0x24);
WWHD_OFFSET(hdLayoutResMgr_c, mpVtable, 0x19038);
WWHD_SIZE(hdLayoutResMgr_c, 0x1903C);
WWHD_SIZE(hdLayoutTask_c, 0x10);
WWHD_SIZE(hdLayoutDisposer_c, 0x10);
WWHD_SIZE(hdLayoutHeaderRange_c, 0xC);
WWHD_SIZE(hdLayoutHeaderWords_c, 0x10);
WWHD_SIZE(hdLayoutRegistration_c, 0xC);

struct hdLayoutPath_c {
    gptr<be<u8>> mText;
    gptr<hdScreenStringTable> mVtable;
    be<u32> mCapacity;
    be<u8> mBuffer[128];
};
WWHD_SIZE(hdLayoutPath_c, 0x8C);
struct hdLayoutWaitTable_c : hdPartsVtbl_c {
    u8 mBeforeWait[0x424 - sizeof(hdPartsVtbl_c)];
    gfn mWaitEnter;
    u8 mWaitPadding[4];
    gfn mWaitFinish;
    u8 mBeforeReset[0x44C - 0x430];
    gfn mReset;
    u8 mResetPadding[0xC];
    gfn mStop;
    u8 mBeforePrepare[0x4A4 - 0x460];
    gfn mPrepare;
};
struct hdLayoutArchiveData_c {
    u8 mPrefix[0x14];
    gptr<void> mpData;
};
struct hdLayoutWaitInput_c {
    u8 mPrefix[4];
    be<s16> mEventIndex;
    u8 mPadding[4];
    be<u8> mSound;
    be<u8> mActorSlot;
    u8 mGap[2];
    be<u8> mMessageMode;
};
struct hdLayoutWaitMessage_c {
    u8 mPrefix[0x120];
    u8 mResourceName[0x10C];
    gptr<hdLayoutWaitInput_c> mpInput;
};
struct hdLayoutWaitPane_c {
    u8 mPrefix[0x44];
    be<u8> mFlags;
};
struct hdLayoutWaitScreen_c : hdParts_c {
    u8 mBeforePane[0x10];
    gptr<hdLayoutWaitPane_c> mpPane;
    u8 mBeforeStatus[0x28];
    be<u32> mStatus;
    u8 mBeforeMessage[0x88];
    gptr<hdLayoutWaitMessage_c> mpMessage;
    void enterFirst();
    void enterSecond();
    void reset();
};
struct hdLayoutWaitPlay_c {
    u8 mBeforeEvent[0x51D0];
    u8 mEvent[0xCC];
    be<u32> mEventActor;
    u8 mBeforeIndex[0x5BA4 - 0x52A0];
    be<s16> mEventIndex;
    u8 mBeforeMode[0x1F];
    be<u8> mMessageMode;
    u8 mBeforeInput[0xC];
    be<u8> mAdvanceCount, mHandInput;
    u8 mBeforeActorSlots[0x5C34 - 0x5BD4];
    be<u32> mActorSlots[256];
};
WWHD_OFFSET(hdLayoutWaitScreen_c, mpPane, 0x60);
WWHD_OFFSET(hdLayoutWaitScreen_c, mStatus, 0x8C);
WWHD_OFFSET(hdLayoutWaitScreen_c, mpMessage, 0x118);
WWHD_OFFSET(hdLayoutWaitMessage_c, mpInput, 0x22C);
WWHD_OFFSET(hdLayoutWaitInput_c, mMessageMode, 0xE);
WWHD_OFFSET(hdLayoutWaitPlay_c, mMessageMode, 0x5BC5);
WWHD_OFFSET(hdLayoutWaitPlay_c, mAdvanceCount, 0x5BD2);
WWHD_OFFSET(hdLayoutWaitPlay_c, mActorSlots, 0x5C34);
struct hdLayoutDummyString_c {
    gptr<be<u8>> mText;
    gptr<hdScreenStringTable> mpVtable;
    be<s32> mCapacity;
    be<u8> mBuffer[6];
    u8 mPadding[2];
};
WWHD_SIZE(hdLayoutDummyString_c, 0x14);

WWHD_OFFSET(hdLayoutResMgr_c, mBusy, 0x20);
WWHD_OFFSET(hdLayoutResMgr_c, mNodes, 0x38);
WWHD_OFFSET(hdLayoutThreadTable_c, mStart, 0x2C);
WWHD_OFFSET(hdLayoutHeapTable_c, mBegin, 0x74);
WWHD_OFFSET(hdLayoutHeapTable_c, mEnd, 0x7C);

WWHD_OFFSET(hdLayoutLoop_c, mAnimationRequest, 0x50);
WWHD_SIZE(hdLayoutLoop_c, 0x54);
WWHD_OFFSET(hdLayoutResourceBindingTable_c, mAttach, 0x74);

WWHD_OFFSET(hdLayoutWaitTable_c, mWaitEnter, 0x424);
WWHD_OFFSET(hdLayoutWaitTable_c, mWaitFinish, 0x42C);
WWHD_OFFSET(hdLayoutWaitTable_c, mReset, 0x44C);
WWHD_OFFSET(hdLayoutWaitTable_c, mStop, 0x45C);
WWHD_OFFSET(hdLayoutWaitTable_c, mPrepare, 0x4A4);

/* 026FC4D8 (hd_layout_wait_screen.cpp): the deleting destructor of the wait screen / message-window base;
 * the message windows call it from their own destructors */
void destroyLayoutWaitScreen(hdParts_c* object, u32 flags);

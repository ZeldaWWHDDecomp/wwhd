// HD: Asynchronous resource task with no GameCube counterpart; verified Cafe layout.
// Fixed strings are 0x4C/0x8C bytes, requests 0xA0, tree nodes 0x20 and
// task is 0x1120 bytes.
#pragma once
#include "wwhd.h"

namespace hd_res_load_task {
struct MappedObject {
    be<u8> mByte;
};
struct Request;
struct ReferenceTree;
struct Heap;
struct SafeStringVtable {
    u8 mPrefix[0x14];
    be<u32> mAssureTermination;
};
WWHD_SIZE(SafeStringVtable, 0x18);
struct SafeString {
    be<u32> mStringTop;
    be<u32> mVtable;
};
WWHD_SIZE(SafeString, 8);
WWHD_OFFSET(SafeString, mVtable, 4);
struct BufferedString : SafeString {
    be<s32> mCapacity;
};
WWHD_SIZE(BufferedString, 0xC);
struct FixedString64 : BufferedString {
    be<u8> mBuffer[64];
};
WWHD_SIZE(FixedString64, 0x4C);
WWHD_OFFSET(FixedString64, mBuffer, 0xC);
struct FixedString128 : BufferedString {
    be<u8> mBuffer[128];
};
WWHD_SIZE(FixedString128, 0x8C);
WWHD_OFFSET(FixedString128, mBuffer, 0xC);
// HD: The verified stack allocation includes four bytes beyond FixedSafeString<128>.
struct FixedStringFrame {
    FixedString128 mString;
    be<u32> mTrailingWord;
};
WWHD_SIZE(FixedStringFrame, 0x90);
struct Request {
    /* 0x00 */ FixedString64 mArchive;
    /* 0x4C */ FixedString64 mResource;
    /* 0x98 */ be<u32> mID;
    /* 0x9C */ be<u8> mIsArchive;
    /* 0x9D */ be<u8> mIsStage;
    /* 0x9E */ u8 mPadding[2];
};
WWHD_SIZE(Request, 0xA0);
WWHD_OFFSET(Request, mResource, 0x4C);
WWHD_OFFSET(Request, mID, 0x98);
struct ReleaseResourceFrame {
    SafeString mArchive;
    Request mRequest;
};
WWHD_SIZE(ReleaseResourceFrame, 0xA8);
template <class T> struct Ring {
    be<u32> mEntries;
    be<s32> mCapacity;
    be<s32> mHead;
    be<s32> mCount;
};
using RequestRing = Ring<Request>;
using IDRing = Ring<be<u32>>;
WWHD_SIZE(RequestRing, 0x10);
WWHD_SIZE(IDRing, 0x10);
struct ReferenceNode {
    be<u32> mLeft;
    be<u32> mRight;
    be<u8> mIsRed;
    u8 mPadding[3];
    be<u32> mKey;
    be<u32> mVtable;
    be<u32> mID;
    be<u32> mReferences;
    be<u32> mTree;
};
WWHD_SIZE(ReferenceNode, 0x20);
WWHD_OFFSET(ReferenceNode, mReferences, 0x18);
struct ReferenceTree {
    be<u32> mRoot;
    be<u32> mFree;
    be<u32> mNodes;
    be<s32> mCount;
    be<s32> mCapacity;
    ReferenceNode mStorage[128];
};
WWHD_SIZE(ReferenceTree, 0x1014);
WWHD_OFFSET(ReferenceTree, mStorage, 0x14);
struct HeapVtable {
    u8 mPrefix[0x2C];
    be<u32> mAdjust;
    u8 mGap[4];
    be<u32> mAlloc;
    u8 mGap2[4];
    be<u32> mFree;
};
WWHD_SIZE(HeapVtable, 0x40);
struct ResourceManager {
    u8 mPrefix[0x11248];
    be<u32> mLoadState;
};
WWHD_SIZE(ResourceManager, 0x1124C);
struct ResourceControl {
    u8 mPrefix[0x2040];
    be<u32> mParentHeap;
    u8 mGap[0x2128 - 0x2044];
    be<u32> mPermanentHeap;
};
WWHD_SIZE(ResourceControl, 0x212C);
struct HeapEnvironment {
    u8 mPrefix[0x14];
    be<u32> mParentHeap;
};
WWHD_SIZE(HeapEnvironment, 0x18);
struct Heap {
    u8 mPrefix[0xC];
    be<u32> mVtable;
};
WWHD_SIZE(Heap, 0x10);
struct ResLoadTask {
    void setInstance();
    void prepare();
    void setIdle();
    u32 release(Request *i_entry);
    u32 unload(Request *i_entry);
    u32 peekRequest(Request *i_out);
    u32 finish(Request *i_entry);
    u32 next(Request *i_entry, u32 retry);
    void retryLoad(Request *i_entry, u32 retry);
    u32 startLoad(Request *i_entry, u32 retry);
    u32 waitLoad(Request *i_entry);
    void calc();
    void freeRings();
    u32 addRef(Request *i_entry);
    u32 requestArchive(SafeString *i_name, u32 front);
    u32 requestResource(SafeString *i_arc, SafeString *i_name, u32 front);
    u32 requestStage(SafeString *i_arc, SafeString *i_name);
    void releaseArchive(SafeString *i_name);
    void releaseResource(SafeString *i_arc, SafeString *i_name);
    void releaseStage(SafeString *i_arc, SafeString *i_name);
    u32 isLoadedId(u32 id);
    u32 isPendingId(u32 id);
    u32 isPendingArchive(SafeString *i_name);
    u32 isPendingResource(SafeString *i_arc, SafeString *i_name);
    void forgetId(u32 id);
    u32 isDerived(const be<u32> *i_info);

    /* 0x0000 */ u8 mTaskPrefix[0x20];
    /* 0x0020 */ be<u32> mTaskVtable;
    /* 0x0024 */ u8 mTaskBody[0x70 - 0x24];
    /* 0x0070 */ be<u32> mTaskSecondVtable;
    /* 0x0074 */ u8 mTaskTail[0xC8 - 0x74];
    /* 0x00C8 */ be<u8> mIsInstance;
    /* 0x00C9 */ u8 mPadding[3];
    /* 0x00CC */ RequestRing mRequests;
    /* 0x00DC */ be<u32> mState;
    /* 0x00E0 */ ReferenceTree mReferences;
    /* 0x10F4 */ IDRing mLoadedIDs;
    /* 0x1104 */ RequestRing mUnloads;
    /* 0x1114 */ be<u32> mCurrentHeap;
    /* 0x1118 */ be<u8> mFlushUnloads;
    /* 0x1119 */ u8 mPaddingTail[3];
    /* 0x111C */ be<s32> mWaitTimer;
};
WWHD_SIZE(ResLoadTask, 0x1120);
WWHD_OFFSET(ResLoadTask, mIsInstance, 0xC8);
WWHD_OFFSET(ResLoadTask, mRequests, 0xCC);
WWHD_OFFSET(ResLoadTask, mReferences, 0xE0);
WWHD_OFFSET(ResLoadTask, mLoadedIDs, 0x10F4);
WWHD_OFFSET(ResLoadTask, mUnloads, 0x1104);
WWHD_OFFSET(ResLoadTask, mCurrentHeap, 0x1114);
WWHD_OFFSET(ResLoadTask, mWaitTimer, 0x111C);
enum LoadState : u32 { STATE_IDLE, STATE_LOADING, STATE_WAITING };
} // namespace hd_res_load_task

void resLoadTask_destroyResourceRequestThunkTarget(u32, u32);

void resLoadTask_destroyArchiveRequestThunkTarget(u32, u32);

void resLoadTask_deleteObject(u32);

void resLoadTask_destroyTaskBase(u32, u32);

void resLoadTask_destroySingletonDisposer(u32, u32);

void resLoadTask_registerGlobalObject(u32);

void resLoadTask_constructHeaderObjectB(u32);

void resLoadTask_constructHeaderObjectA(u32);

u32 resLoadTask_eraseReference(u32, u32, u32);

u32 resLoadTask_findReference(u32, u32, u32);

void resLoadTask_formatString(u32, u32, u32, u32);

void resLoadTask_moveBytes(u32, u32, u32, u32);

u32 resLoadTask_hashCRC32(u32, u32);

u32 resLoadTask_insertReference(u32, u32, u32);

u32 resLoadTask_findContainingHeap(u32, u32);

u32 resLoadTask_loadArchive(u32, u32, u32);

u32 resLoadTask_canRetryArchive(u32, u32);

u32 resLoadTask_loadResource(u32, u32, u32, u32);

u32 resLoadTask_loadStage(u32, u32, u32, u32);

u32 resLoadTask_randomU32(u32);

u32 resLoadTask_createExpHeap(u32, u32, u32, u32, u32);

u32 resLoadTask_getResourceParentHeap(u32, u32, u32);

u32 resLoadTask_isArchiveLoaded(u32, u32);

u32 resLoadTask_retryArchiveLoad(u32, u32, u32);

u32 resLoadTask_startTaskRequest(u32, u32, u32);

u32 resLoadTask_peekTaskRequest(u32, u32);

void resLoadTask_registerLoadedResource(u32, u32, u32, u32);

void resLoadTask_registerLoadedArchive(u32, u32, u32);

u32 resLoadTask_unloadStage(u32, u32);

void resLoadTask_deleteResource(u32, u32, u32);

void resLoadTask_deleteArchive(u32, u32);

void resLoadTask_initializeLoadNotification();

void resLoadTask_initializeLoadSignals();

u32 resLoadTask_getTaskParentHeap();

void resLoadTask_registerArchive(u32, u32, u32, u32, u32, u32, u32);

u32 resLoadTask_getTextureParentHeap();

void resLoadTask_initializeJpegManager(u32);

void resLoadTask_attachMessageHeap(u32, u32);

void resLoadTask_createMessageManager(u32);

u32 resLoadTask_createFrameHeap(u32, u32, u32, u32, u32);

void resLoadTask_attachFontHeap(u32, u32);

void resLoadTask_createFontManager(u32);

void resLoadTask_loadPermanentPack(u32, u32, u32);

u32 resLoadTask_getPermanentPackName(u32);

void resLoadTask_loadPermanentArchive(u32, u32, u32);

void resLoadTask_prepareTaskBase(u32);

u32 resLoadTask_getCurrentHeap(u32);

u32 resLoadTask_allocateObject(u32);

void resLoadTask_constructTaskBase(u32, u32, u32);

void resLoadTask_destroyTask(u32, u32);

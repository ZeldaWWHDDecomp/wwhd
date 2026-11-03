// WWHD fixed-block heap and its translation-unit support, from local instructions.
#pragma once
#include "wwhd.h"
#include "sead/Allocation.h"
#include "sead/RuntimeType.h"
#include "sead/FoundationObjects.h"
#include "sead/HeapRegistry.h"
struct FixedAngleEntry { be<u32> mBase; be<f32> mStep; };
struct FixedAngleLookup { FixedAngleEntry mEntries[129]; };
using MessageAllocate = void* (*)(u32);
using MessageRelease = void (*)(void*);
namespace sead {
struct StringDispatch { u8 mBeforeAssureTermination[20]; be<u32> mAssureTermination; };
struct PoolHeap : RegistryHeap {
 be<u32> mBlockSize; gptr<be<u32> > mBlocks; be<u32> mBlockRegionSize;
 be<u32> mFreeSize; gptr<be<u32> > mFreeHead; gptr<be<u32> > mFreeTail;
 void rebuildFreeList();
 static PoolHeap* construct(PoolHeap* heap,const SafeText* name,RegistryHeap* parent,void* storage,u32 size,u32 block,u32 options);
 static PoolHeap* create(u32 block,u32 count,const SafeText* name,s32 alignment,RegistryHeap* parent,u32 options);
 void dispose();
 void* resizeFront(void* block,u32 size); void* resizeBack(void* block,u32 size);
 u32 supportsRelease() const; u32 supportsResize() const; u32 isType(const RuntimeType* type) const; void arrangeBlocks(u32 alignment, u32 reserveHeader);
 void* allocate(u32 size,s32 alignment); void release(void* block);
 u32 storageSize() const; void* storage() const; u32 totalSize() const;
 u32 freeSize() const; u32 largestBlock() const; u8 contains(void* pointer) const;
 void releaseAll(); void synchronize(); void destroy(u32 flags);
};
struct TerminatingBuffer : BufferedText { void terminatePool(); void terminateArena(); };
struct RootNameString { gptr<be<u8> > mText; be<u32> mTable; void destroy(u32 flags); void assureTermination() const; };
struct RootBufferedString { void destroy(u32 flags); };
struct PoolTreeTag { void destroy(u32 flags); };
struct PoolLockTag { void destroy(u32 flags); };
struct PoolNameString { gptr<be<u8> > mText; be<u32> mTable; void destroy(u32 flags); void assureTermination() const; };
struct ArenaTreeTag { void destroy(u32 flags); };
struct ArenaLockTag { void destroy(u32 flags); };
struct HeapMonitorTag { void destroy(u32 flags); };
struct HeapContextTag { void destroy(u32 flags); };
struct ForegroundNameString { gptr<be<u8> > mText; be<u32> mTable; void destroy(u32 flags); void assureTermination() const; };
struct MessageOwnerTag { void destroy(u32 flags); };
struct ResourceOwnerTag { void destroy(u32 flags); };
struct CoreIdentity { be<u32> mIndex; static CoreIdentity* construct(CoreIdentity* core); u32 affinity() const; };
struct CoreString { gptr<be<u8> > mText; be<u32> mTable; };
struct CoreWorkList {
 be<u32> mNameCount; gptr<CoreString> mNames;
 be<u32> mValueCount; gptr<be<u32> > mValues; be<u32> mFlags; be<u32> mFlagsTable;
 void initialize(u32 flags,RegistryHeap* heap); void clearValues(); void enterCore(); void leaveCore(); void finishCore(); void rotateColor(); void releaseCore();
};
struct JobDispatch {
 u8 mBeforeTake[28]; be<u32> mTake;
 be<u32> mReservedTake; be<u32> mTotal;
 be<u32> mReservedTotal; be<u32> mWait;
};
struct JobItemTable { u8 mBeforeExecute[12]; be<u32> mExecute; };
struct JobItem { u8 mBeforeTable[12]; gptr<JobItemTable> mTable; };
struct WorkerState { u8 mBeforeState[0x98]; be<u32> mState; };
struct JobQueue {
 gptr<JobDispatch> mTable; CoreString mName; be<u32> mWaitMode;
 CoreIdentity mCore; be<u32> mCoreMask;
 be<u32> mBatchSize[3]; be<u32> mActive[3]; be<u32> mCompleted[3];
 be<u32> mState; be<u32> mKind; CoreWorkList mWork;
 be<u32> mCapacity; gptr<be<u32> > mJobs; be<u32> mCount; be<u32> mNext;
 static JobQueue* constructBase(JobQueue* queue); static JobQueue* construct(JobQueue* queue);
 void initialize(s32 capacity,RegistryHeap* heap);
 void begin(const be<u32>* mask,const be<u32>* waitMode);
 void setBatchSize(const CoreIdentity* core,u32 count);
 u32 consume(be<u32>* consumed); u32 waitUntilCompleted();
 void synchronizeCompletion(); void finishCore(const CoreIdentity* core);
 u32 take(u32 limit,be<u32>* consumed,WorkerState* worker);
 u32 append(u32 job); void clear(); u32 count() const;
 void destroy(u32 flags); void beginImpl(); void endImpl();
 u32 emptyTake(u32 count,be<u32>* consumed,u32 option); u32 emptyTotal() const; void waitImpl();
};
struct DebugColor { be<f32> mRed; be<f32> mGreen; be<f32> mBlue; be<f32> mAlpha; };
struct MessageArchive { gptr<void> mReader; be<u32> mCount;
 u32 open(void* data,AllocationHeap* heap); void* entry(u32 index) const; u32 copyEntry(TerminatingBuffer* buffer,u32 index) const; };
struct FrameProvider { gptr<void> mInstance; void initialize(RegistryHeap* heap,void* settings,u32 size,u32 options); };
struct ProviderList { be<s32> mCount; be<s32> mCapacity; gptr<gptr<void> > mEntries; };
struct ProviderConstructor { be<u32> mTable; be<u32> mBaseTable; };
struct WorkerRuntime {
 u8 mBeforeTable[12]; gptr<JobItemTable> mTable; gptr<be<u8> > mName;
 u8 mBeforeEnabled[0x80]; be<u32> mEnabled; be<u32> mState;
 u8 mBeforeDelay[16]; be<s32> mDelay; be<u32> mReservedDelay; be<u32> mExtraDelay;
};
// +0x18 is each worker's job-queue capacity (default 0x20), passed to JobQueueWorker_l::create (0276A628).
struct WorkerManagerConfig { be<u32> mPriority[3]; be<u32> mStackSize[3]; be<s32> mQueueCapacity; gptr<void> mName; };
struct WorkerManager;
struct RegistryCallback { be<u32> mTable; gptr<WorkerManager> mOwner; be<u16> mKind; be<s16> mAdjustment; be<u32> mMethod; };
struct RegistryHost { u8 mBeforeWorkers[0xD8]; LinkList mWorkers; };
struct WorkerManager {
 be<u32> mTable; EmbeddedObject mObject; be<u32> mWorkerCount; gptr<gptr<WorkerRuntime> > mWorkers;
 be<u32> mQueueCapacity; gptr<gptr<JobQueue> > mQueues; be<u32> mQueueCount;
 be<u8> mSingleThread; u8 mPaddingMode[3]; be<u32> mReservedMode; be<u32> mSubmission;
 be<u32> mTimeHigh; be<u32> mTimeLow; be<u8> mSubmitted; u8 mTail[7];
 static WorkerManager* construct(WorkerManager* manager);
 void initialize(WorkerManagerConfig* config);
 void settle();
 void submit(u32 name,JobQueue* queue,const be<u32>* mask,const be<u32>* waitMode,const be<u32>* flags);
 void start();
};
struct MessageColor { be<u8> mRed; be<u8> mGreen; be<u8> mBlue; be<u8> mAlpha; };
struct MessageStyle { be<u32> mFont; be<u32> mSize; be<s32> mColor; be<s32> mShadow; };
struct MessagePair { be<u32> mGroup; be<u32> mValue; };
struct MessageStyles {
 gptr<void> mReader; be<u32> mColorCount; gptr<MessageColor> mColors;
 be<u32> mStyleCount; gptr<MessageStyle> mStyles;
 be<u32> mPairCount; gptr<MessagePair> mPairs; be<s32> mFallback;
 u32 open(void* data,RegistryHeap* heap);
};
struct ResourceState { be<u32> mResource; be<u32> mSize; be<u8> mOwned; u8 mPadding[3]; void allocate(u32 size); };
}
WWHD_OFFSET(sead::PoolHeap,mRegionStart,0x1C);
WWHD_OFFSET(sead::PoolHeap,mRegionSize,0x20);
WWHD_OFFSET(sead::PoolHeap,mParent,0x24);
WWHD_OFFSET(sead::PoolHeap,mLock,0x54);
WWHD_OFFSET(sead::PoolHeap,mFlags,0x90);
WWHD_OFFSET(sead::PoolHeap,mBlockSize,0x94);
WWHD_OFFSET(sead::PoolHeap,mFreeHead,0xA4);
WWHD_SIZE(sead::PoolHeap,0xAC);
WWHD_SIZE(sead::ResourceState,12);

WWHD_OFFSET(sead::JobQueue,mWaitMode,0xC);
WWHD_OFFSET(sead::JobQueue,mBatchSize,0x18);
WWHD_OFFSET(sead::JobQueue,mCompleted,0x30);
WWHD_OFFSET(sead::JobQueue,mWork,0x44);
WWHD_OFFSET(sead::JobQueue,mCount,0x64);
WWHD_SIZE(sead::JobQueue,0x6C);

using CoreConstruct = sead::CoreIdentity* (*)(sead::CoreIdentity*);

WWHD_OFFSET(sead::AllocationFailureEvent,mHeap,0);
WWHD_OFFSET(sead::AllocationFailureEvent,mWorkingSize,12);
WWHD_SIZE(sead::AllocationFailureEvent,20);

WWHD_OFFSET(sead::WorkerRuntime,mEnabled,0x94);
WWHD_OFFSET(sead::WorkerRuntime,mState,0x98);
WWHD_OFFSET(sead::WorkerRuntime,mDelay,0xAC);
WWHD_OFFSET(sead::WorkerManager,mWorkerCount,0x40);
WWHD_OFFSET(sead::WorkerManager,mSubmitted,0x68);
WWHD_SIZE(sead::WorkerManager,0x70);

WWHD_OFFSET(sead::WorkerManager,mObject,4);

WWHD_OFFSET(sead::StringDispatch,mAssureTermination,0x14);
WWHD_SIZE(sead::StringDispatch,0x18);
WWHD_OFFSET(sead::PoolHeap,mBlocks,0x98);
WWHD_OFFSET(sead::PoolHeap,mBlockRegionSize,0x9C);

// Core index -> worker slot (104A0BC0, 32 words), owned by sead/PoolHeapStatics.cpp.
extern be<u32> (&sCoreReverse)[32];

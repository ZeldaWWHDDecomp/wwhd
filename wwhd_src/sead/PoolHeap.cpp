// WWHD fixed-block allocation and translation-unit support, reconstructed from the binary.

#include "sead/PoolHeap.h"
static sead::RegistryMutex_l sPoolCreationLock = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
static u32 sCoreCount;
static u32 sCoreOrder[3] = {0,0,0};
static sead::DebugColor sJobColors[9] = {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
static u32 sJobColorGuard;
static u32 sArenaTypeGuard;
static sead::RuntimeType sArenaType;
static u32 sArenaTypeTable;
static u32 sHeapTypeTable;
static FixedAngleLookup sFixedAngleTable;
static u32 sResourceAllocate;
static sead::ProviderList sProviderList;

static u32 sProviderBaseTable;
static u8 sUnnamedCoreName;
static u8 sEmptyCoreName;
static u32 sCoreStringTable;
static u32 sJobBaseTable;
static u32 sJobQueueTable;
static u32 sJobKindTable;
static u32 sCoreWorkKind;
static u8 sDefaultJobName;
static u8 sDefaultWorkName;
static u32 sWorkerManagerTable;
static u32 sRegistryEntryTable;
static u32 sRegistryCallbackTable;
static sead::RegistryHost* sRegistryHost;
static u32 sWorkerNameBaseTable;
static u32 sWorkerNameTable;
static u32 sWorkerNameViewTable;
static u32 sPoolHeapTable;
static u32 sPoolTreeTable;
static u32 sStorageOwner;
static sead::AllocationHeap* sMessageHeap;
static sead::AllocationHeap* sResourceHeap;
extern void bindMessageAllocatorAddresses(u32 allocate,u32 release);
static void setMessageAllocators(MessageAllocate allocate,MessageRelease release) {
 u32 allocateAddress=allocate ? gabi::guest_fn(reinterpret_cast<const void*>(allocate)) : 0;
 u32 releaseAddress=release ? gabi::guest_fn(reinterpret_cast<const void*>(release)) : 0;
 bindMessageAllocatorAddresses(allocateAddress,releaseAddress);
}
extern void* openMessageReader(void*);
extern s32 messageEntryCount(void*);
extern void closeMessageReader(void*);
extern void* messageEntry(void*,u32);
extern void copyMessageEntry(void*,u32,void*);
extern void* baseSystemHeap(u32 index);
extern u32 allocatableSystemHeap(void* heap,u32 alignment);
extern void constructCoreArray(void* array,u32 count,u32 stride,CoreConstruct construct);
extern u32 frameSystemCapacity(void*,u32);
extern void* frameSystemAllocate(void*,u32,u32);
extern u32 providerObjectSize(void*);
extern void* constructProvider(sead::ProviderConstructor*,void*,u32,void*,u32,u32);
extern sead::WorkerRuntime* constructWorker(sead::WorkerRuntime*,sead::WorkerManager*,s32,u32,u32);
extern void formatWorkerName(sead::TerminatingBuffer*,const char*,void*,u32,const char*);
extern void initializeAllocatedString(void*,u32);
extern void copyWorkerName(void*,void*,u32,u32);
extern void setWorkerAffinity(sead::WorkerRuntime*,const be<u32>*);
extern void constructRegistryBase(sead::EmbeddedObject*);
extern void clearWorkerClock(void*);
extern void workerManagerCallback(sead::WorkerManager*);
extern void* openStyleReader(void*);
extern s32 styleColorCount(void*);
extern void styleColor(void*,u32,sead::MessageColor*);
extern s32 styleCount(void*);
extern u32 styleFont(void*,u32);
extern u32 styleSize(void*,u32);
extern s32 styleColorIndex(void*,u32);
extern s32 styleShadowIndex(void*,u32);
extern s32 stylePairCount(void*);
extern u32 stylePairGroup(void*,u32);
extern u32 stylePairValue(void*,u32);
extern s32 styleFallback(void*);
extern void assignWorkerJob(sead::WorkerRuntime*,u32,sead::JobQueue*,const be<u32>*);
extern void finishWorker(sead::WorkerRuntime*);
extern void wakeWorker(sead::WorkerRuntime*,u32);
extern Pair32 systemTime();
extern u32 currentCoreIndex();
extern void memoryBarrier();
extern void releasePoolObject(void* object);
extern void releaseArrayObject(void* object);
extern void* allocatePoolObject(u32 size);
extern void* allocateSupportArray(u32 size,sead::AllocationHeap* heap,s32 alignment);
extern void lockPool(void* lock);
extern void unlockPool(void* lock);
extern sead::RegistryHeap* constructHeapBase(sead::RegistryHeap*,const sead::SafeText*,sead::RegistryHeap*,u32,u32,s32,u32);

/* 02756254 */
void sead::RootNameString::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02756268 */
void sead::RootNameString::assureTermination() const {}
/* 0275626C */
void sead::RootBufferedString::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02756280 */
void sead::TerminatingBuffer::terminatePool() { mText[(u32)mCapacity - 1] = 0; }
/* 02756298 */
void sead::PoolTreeTag::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 027562AC */
void sead::PoolLockTag::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 027566D0 */
u32 sead::PoolHeap::storageSize() const { return mRegionSize; }
/* 02756974 */
void* sead::PoolHeap::storage() const { return gabi::at<void>(mRegionStart); }
/* 0275697C */
u32 sead::PoolHeap::totalSize() const { return mRegionSize; }
/* 02756984 */
u32 sead::PoolHeap::freeSize() const { return mFreeSize; }
/* 0275698C */
u32 sead::PoolHeap::largestBlock() const { return mFreeHead != NULL ? (u32)mBlockSize : 0; }
/* 027569A4 */
u8 sead::PoolHeap::contains(void* pointer) const {
 u32 start = gabi::ea(mBlocks.get());
 u32 end = start + (u32)mBlockRegionSize;
 return gabi::ea(pointer) >= start && gabi::ea(pointer) < end;
}
/* 027569D0 */
void sead::PoolHeap::synchronize() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if ((u32)mFlags & 1) { void* lock = mLock; lockPool(lock); if (lock != NULL) unlockPool(lock); }
}
/* 02756A1C */
void sead::PoolHeap::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (this != NULL) {
  mObject.mTable = gabi::ea(&sPoolHeapTable); mHeapInterface = gabi::ea(&sPoolTreeTable);
  RegistryHeap::detachAndDispose(); RegistryHeap::destroy(0);
  if (flags & 1) releasePoolObject(this);
 }
}
/* 02756A90 */
void sead::PoolNameString::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02756AA4 */
void sead::PoolNameString::assureTermination() const {}
/* 02756AA8 */
void sead::TerminatingBuffer::terminateArena() { mText[(u32)mCapacity - 1] = 0; }
/* 02756AC0 */
void sead::ArenaTreeTag::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02756B4C */
void* sead::PoolHeap::resizeFront(void* block,u32 size) { return 0; }
/* 02756B54 */
void* sead::PoolHeap::resizeBack(void* block,u32 size) { return 0; }
/* 02756B5C */
u32 sead::PoolHeap::supportsRelease() const { return 1; }
/* 02756B64 */
u32 sead::PoolHeap::supportsResize() const { return 0; }
/* 02756B6C */
void sead::HeapMonitorTag::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02756B80 */
void sead::HeapContextTag::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02756C98 */
static void initializeStorageOwner() { sStorageOwner = 0; }
/* 02756CA8 */
void sead::ForegroundNameString::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02756CBC */
void sead::ForegroundNameString::assureTermination() const {}
/* 02756CC0 */
static sead::ResourceState* constructResourceState(sead::ResourceState* state) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (state == NULL) { state = static_cast<sead::ResourceState*>(allocatePoolObject(12)); if (state == NULL) return state; }
 state->mResource = 0; state->mOwned = 0; state->mSize = 0; return state;
}
/* 02756D04 */
void sead::ResourceOwnerTag::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02757644 */
void sead::MessageOwnerTag::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02757658 */
static void* allocateMessageArray(u32 size) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); return allocateSupportArray(size,sMessageHeap,4); }
/* 02757668 */
static void releaseMessageArray(void* pointer) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); releaseArrayObject(pointer); }
/* 027579A8 */
static void* allocateResourceArray(u32 size) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); return allocateSupportArray(size,sResourceHeap,4); }
/* 027579B8 */
static void releaseResourceArray(void* pointer) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); releaseArrayObject(pointer); }

/* 027562C0 */
sead::PoolHeap* sead::PoolHeap::construct(PoolHeap* heap,const SafeText* name,RegistryHeap* parent,void* storage,u32 size,u32 block,u32 options) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (heap == NULL) { heap = static_cast<PoolHeap*>(allocatePoolObject(sizeof(PoolHeap))); if (heap == NULL) return heap; }
 constructHeapBase(heap,name,parent,gabi::ea(storage),size,1,options);
 heap->mBlockSize = block; heap->mBlockRegionSize = 0;
 heap->mObject.mTable = gabi::ea(&sPoolHeapTable);
 heap->mFreeHead = NULL; heap->mBlocks = NULL; heap->mFreeTail = NULL; heap->mFreeSize = 0;
 return heap;
}
/* 027564DC */
sead::PoolHeap* sead::PoolHeap::create(u32 block,u32 count,const SafeText* name,s32 alignment,RegistryHeap* parent,u32 options) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 lockPool(&sPoolCreationLock);
 u32 absolute = alignment < 0 ? 0u - u32(alignment) : u32(alignment);
 if (!count || !block) { unlockPool(&sPoolCreationLock); return NULL; }
 if (s32(absolute) < 4) absolute = 4;
 u32 magnitude = s32(absolute) < 0 ? 0u - absolute : absolute;
 if ((magnitude - 1) & magnitude) { unlockPool(&sPoolCreationLock); return NULL; }
 if (parent == NULL) {
  parent = registryCurrentHeap(sRegistryManager.get());
  if (parent == NULL) { unlockPool(&sPoolCreationLock); return NULL; }
 }
 u32 rounded = (block + absolute - 1) & ~(absolute - 1);
 u32 size = rounded * count + absolute + sizeof(PoolHeap);
 PoolHeap* heap = gabi::call_ptr<PoolHeap*>(parent->dispatch()->mAllocate.mFunction,parent,size,4);
 if (heap == NULL) { unlockPool(&sPoolCreationLock); return NULL; }
 heap = construct(heap,name,parent,heap,size,block,options);
 heap->arrangeBlocks(absolute,1);
 unlockPool(&sPoolCreationLock);
 return heap;
}
/* 02756628 */
void sead::PoolHeap::dispose() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 lockPool(&sPoolCreationLock);
 u32 destroyMethod = dispatch()->mDestroy.mFunction;
 void* storage = gabi::at<void>(mRegionStart);
 RegistryHeap* parent = mParent;
 gabi::call_ptr<void>(destroyMethod,this,2);
 if (parent != NULL && gabi::call_ptr<u32>(parent->dispatch()->mSupportsRelease.mFunction,parent))
  gabi::call_ptr<void>(parent->dispatch()->mRelease.mFunction,parent,storage);
 unlockPool(&sPoolCreationLock);
}
/* 027566D8 */
void* sead::PoolHeap::allocate(u32 size,s32 alignment) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 RegistryManagerState* manager = sRegistryManager;
 u32 block = mBlockSize;
 if (alignment < 0 || (block & (u32(alignment) - 1)) || size > block) {
  if (manager != NULL) {
   AllocationFailureReceiver* observer = manager->mFailureReceiver;
   if (observer != NULL) {
    gabi::FrameLocal<AllocationFailureEvent> failure(8);
    failure->mRequestedAlignment = alignment; failure->mWorkingSize = size;
    failure->mRequestedSize = size; failure->mWorkingAlignment = alignment; failure->mHeap = this;
    gabi::call_ptr<void>(observer->mTable->mReport.mFunction,observer,failure.get());
   }
  }
  return NULL;
 }
 void* lock = NULL;
 if ((u32)mFlags & 1) { lock = mLock; lockPool(lock); }
 be<u32>* blockPointer = mFreeHead;
 if (blockPointer == NULL) {
  if (manager != NULL) {
   AllocationFailureReceiver* observer = manager->mFailureReceiver;
   if (observer != NULL) {
    gabi::FrameLocal<AllocationFailureEvent> failure(0x1C);
    failure->mRequestedSize = size; failure->mWorkingSize = mBlockSize;
    failure->mRequestedAlignment = alignment; failure->mWorkingAlignment = alignment; failure->mHeap = this;
    gabi::call_ptr<void>(observer->mTable->mReport.mFunction,observer,failure.get());
   }
  }
  if (lock != NULL) unlockPool(lock);
  return NULL;
 }
 u32 actualBlockSize = mBlockSize;
 u32 next = *blockPointer;
 u32 free = mFreeSize;
 mFreeHead = gabi::at<be<u32> >(next);
 mFreeSize = free - actualBlockSize;
 if (lock != NULL) unlockPool(lock);
 return blockPointer;
}
/* 02756360 */
void sead::PoolHeap::rebuildFreeList() {
 u32 blockSize = mBlockSize;
 u32 count = blockSize ? (u32)mBlockRegionSize / blockSize : 0;
 u32 stride = u32(s32(blockSize) / 4) * 4;
 be<u32>* first = mBlocks;
 be<u32>* blocks = first;
 mFreeHead = blocks;
 for (s32 remaining = s32(count - 1); remaining > 0; --remaining) {
  *blocks = gabi::ea(blocks) + stride;
  blocks = gabi::at<be<u32> >(gabi::ea(blocks) + stride);
 }
 *gabi::at<be<u32> >(gabi::ea(first) + stride * (count - 1)) = 0;
 mFreeTail = first;
}
/* 027563BC */
void sead::PoolHeap::arrangeBlocks(u32 alignment,u32 reserveHeader) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 void* lock = NULL;
 if ((u32)mFlags & 1) { lock = mLock; lockPool(lock); }
 u32 headerEnd = gabi::ea(this) + sizeof(PoolHeap);
 u32 start = (headerEnd + alignment - 1) & ~(alignment - 1);
 if (reserveHeader && start == headerEnd) start += alignment;
 u32 block = ((u32)mBlockSize + alignment - 1) & ~(alignment - 1);
 u32 size = (u32)mRegionSize - (start - gabi::ea(this));
 mBlockSize = block;
 mBlocks = gabi::at<be<u32> >(start);
 mBlockRegionSize = size;
 mFreeSize = block * (block ? size / block : 0);
 rebuildFreeList();
 if (lock != NULL) unlockPool(lock);
}
/* 0275683C */
void sead::PoolHeap::release(void* block) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (block == NULL) return;
 if (!gabi::call_ptr<u32>(dispatch()->mContains.mFunction,this,block)) return;
 u32 flags = mFlags;
 if (flags & 2) return;
 void* lock = NULL;
 if (flags & 1) { lock = mLock; lockPool(lock); }
 *gabi::at<be<u32> >(gabi::ea(block)) = gabi::ea(mFreeHead.get());
 u32 size = mBlockSize;
 u32 free = mFreeSize;
 mFreeHead = static_cast<be<u32>*>(block);
 mFreeSize = free + size;
 if (lock != NULL) unlockPool(lock);
}
/* 027568EC */
void sead::PoolHeap::releaseAll() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 void* lock = NULL;
 if ((u32)mFlags & 1) { lock = mLock; lockPool(lock); }
 RegistryHeap::disposeRange(0,0);
 u32 block = mBlockSize;
 u32 region = mBlockRegionSize;
 mFreeSize = block * (block ? region / block : 0);
 rebuildFreeList();
 if (lock != NULL) unlockPool(lock);
}

/* 02757B48 */
static void initializeCoreMapping() {
 sCoreCount = 3; sCoreOrder[0] = 1; sCoreOrder[1] = 0; sCoreOrder[2] = 2;
 for (u32 core=0; core<3; ++core) sCoreReverse[(u32)sCoreOrder[core]] = core;
}
/* 02757B98 */
u32 sead::CoreIdentity::affinity() const { u32 index = sCoreOrder[(u32)mIndex]; return (index & 32) ? 0 : 1u << (index & 31); }
/* 02757DCC */
void sead::CoreWorkList::clearValues() {
 for (u32 index=0; s32(index) < s32(mValueCount); ++index) {
  be<u32>* values = mValues;
  if (index < (u32)mValueCount) values += index;
  *values = 0;
 }
}
/* 02758414 */
void sead::JobQueue::begin(const be<u32>* mask,const be<u32>* waitMode) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 u32 selected = *mask; mState = 6; mCoreMask = selected;
 for (u32 core=0; core<sCoreCount; ++core) {
  u32 index = core < 3 ? core : 0;
  u32 bit = (core & 32) ? 0 : 1u << (core & 31);
  mActive[index] = (selected & bit) ? 1 : 0;
  mCompleted[index] = 0;
 }
 mWaitMode = *waitMode;
}
/* 027584E0 */
void sead::JobQueue::setBatchSize(const CoreIdentity* core,u32 count) {
 u32 index = core->mIndex;
 if (!count) count = 1;
 mBatchSize[index < 3 ? index : 0] = count;
}
/* 0275850C */
u32 sead::JobQueue::consume(be<u32>* consumed) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 u32 total = gabi::call_ptr<u32>(mTable->mTotal,this);
 *consumed = 0;
 gabi::FrameLocal<be<u32> > count(8);
 u32 done;
 do {
  *count.get() = 0;
  done = gabi::call_ptr<u32>(mTable->mTake,this,total,count.get(),0);
  *consumed = (u32)*consumed + (u32)*count.get();
 } while (!done);
 return done;
}
/* 027585A8 */
u32 sead::JobQueue::waitUntilCompleted() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 for (;;) {
  u32 completed = 0;
  for (u32 core=0; core<3; ++core) completed += (u32)mCompleted[core];
  u32 total = gabi::call_ptr<u32>(mTable->mTotal,this);
  if (completed >= total) return 1;
 }
}
/* 02758634 */
void sead::JobQueue::synchronizeCompletion() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 memoryBarrier();
 u32 mode = mWaitMode;
 if (mode == 1 || mode == 2) gabi::call_ptr<void>(mTable->mWait,this);
}
/* 027586C4 */
void sead::JobQueue::finishCore(const CoreIdentity* core) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 memoryBarrier(); u32 index = core->mIndex; mActive[index < 3 ? index : 0] = 0;
 synchronizeCompletion();
}
/* 02758AAC */
u32 sead::JobQueue::append(u32 job) {
 u32 count = mCount; u32 capacity = mCapacity; mState = 3;
 if (count >= capacity) return 0;
 count = mCount; capacity = mCapacity; mCount = count + 1;
 be<u32>* jobs = mJobs; if (count < capacity) jobs += count;
 *jobs = job; return 1;
}
/* 02758B00 */
void sead::JobQueue::clear() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 mState = 5; mWork.clearValues(); mNext = 0; mCount = 0; mWaitMode = 0;
}
/* 02758B48 */
void sead::JobQueue::destroy(u32 flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); if (this != NULL && (flags & 1)) releasePoolObject(this); }
/* 02758B5C */
void sead::JobQueue::beginImpl() {}
/* 02758B60 */
void sead::JobQueue::endImpl() {}
/* 02758B64 */
u32 sead::JobQueue::emptyTake(u32 count,be<u32>* consumed,u32 option) { *consumed = 0; return 1; }
/* 02758B74 */
u32 sead::JobQueue::emptyTotal() const { return 0; }
/* 02758B7C */
void sead::JobQueue::waitImpl() {}
/* 02758B80 */
u32 sead::JobQueue::count() const { return mCount; }

// Typed views of contiguous scalar initialization regions; original aggregate spelling is unknown.
static f32 sMathSingleDefaults[82] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static f64 sMathDoubleDefaults[82] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
/* 02756E3C */
static void initializeMathDefaults() {
 sMathSingleDefaults[0] = 0.0f;
 sMathSingleDefaults[1] = 0.0f;
 sMathSingleDefaults[2] = 0.0f;
 sMathSingleDefaults[3] = 0.0f;
 sMathSingleDefaults[4] = 1.0f;
 sMathSingleDefaults[5] = 0.0f;
 sMathSingleDefaults[6] = 0.0f;
 sMathSingleDefaults[7] = 1.0f;
 sMathSingleDefaults[8] = 0.0f;
 sMathSingleDefaults[9] = 0.0f;
 sMathSingleDefaults[10] = 0.0f;
 sMathSingleDefaults[11] = 0.0f;
 sMathSingleDefaults[12] = 0.0f;
 sMathSingleDefaults[13] = 0.0f;
 sMathSingleDefaults[14] = 0.0f;
 sMathSingleDefaults[15] = 0.0f;
 sMathSingleDefaults[16] = 0.0f;
 sMathSingleDefaults[17] = 1.0f;
 sMathSingleDefaults[18] = 0.0f;
 sMathSingleDefaults[19] = 0.0f;
 sMathSingleDefaults[20] = 0.0f;
 sMathSingleDefaults[21] = 1.0f;
 sMathSingleDefaults[22] = 0.0f;
 sMathSingleDefaults[23] = 0.0f;
 sMathSingleDefaults[24] = 0.0f;
 sMathSingleDefaults[25] = 1.0f;
 sMathSingleDefaults[26] = 0.0f;
 sMathSingleDefaults[27] = 0.0f;
 sMathSingleDefaults[28] = 0.0f;
 sMathSingleDefaults[29] = 0.0f;
 sMathSingleDefaults[30] = 0.0f;
 sMathSingleDefaults[31] = 0.0f;
 sMathSingleDefaults[32] = 0.0f;
 sMathSingleDefaults[33] = 0.0f;
 sMathSingleDefaults[34] = 0.0f;
 sMathSingleDefaults[35] = 0.0f;
 sMathSingleDefaults[36] = 0.0f;
 sMathSingleDefaults[37] = 0.0f;
 sMathSingleDefaults[38] = 1.0f;
 sMathSingleDefaults[39] = 0.0f;
 sMathSingleDefaults[40] = 0.0f;
 sMathSingleDefaults[41] = 0.0f;
 sMathSingleDefaults[42] = 0.0f;
 sMathSingleDefaults[43] = 1.0f;
 sMathSingleDefaults[44] = 0.0f;
 sMathSingleDefaults[45] = 0.0f;
 sMathSingleDefaults[46] = 0.0f;
 sMathSingleDefaults[47] = 0.0f;
 sMathSingleDefaults[48] = 1.0f;
 sMathSingleDefaults[49] = 0.0f;
 sMathSingleDefaults[50] = 0.0f;
 sMathSingleDefaults[51] = 0.0f;
 sMathSingleDefaults[52] = 0.0f;
 sMathSingleDefaults[53] = 0.0f;
 sMathSingleDefaults[54] = 0.0f;
 sMathSingleDefaults[55] = 0.0f;
 sMathSingleDefaults[56] = 0.0f;
 sMathSingleDefaults[57] = 0.0f;
 sMathSingleDefaults[58] = 0.0f;
 sMathSingleDefaults[59] = 0.0f;
 sMathSingleDefaults[60] = 0.0f;
 sMathSingleDefaults[61] = 0.0f;
 sMathSingleDefaults[62] = 0.0f;
 sMathSingleDefaults[63] = 0.0f;
 sMathSingleDefaults[64] = 0.0f;
 sMathSingleDefaults[65] = 0.0f;
 sMathSingleDefaults[66] = 1.0f;
 sMathSingleDefaults[67] = 0.0f;
 sMathSingleDefaults[68] = 0.0f;
 sMathSingleDefaults[69] = 0.0f;
 sMathSingleDefaults[70] = 0.0f;
 sMathSingleDefaults[71] = 1.0f;
 sMathSingleDefaults[72] = 0.0f;
 sMathSingleDefaults[73] = 0.0f;
 sMathSingleDefaults[74] = 0.0f;
 sMathSingleDefaults[75] = 0.0f;
 sMathSingleDefaults[76] = 1.0f;
 sMathSingleDefaults[77] = 0.0f;
 sMathSingleDefaults[78] = 0.0f;
 sMathSingleDefaults[79] = 0.0f;
 sMathSingleDefaults[80] = 0.0f;
 sMathSingleDefaults[81] = 1.0f;
 sMathDoubleDefaults[0] = 0.0;
 sMathDoubleDefaults[1] = 0.0;
 sMathDoubleDefaults[2] = 0.0;
 sMathDoubleDefaults[3] = 0.0;
 sMathDoubleDefaults[4] = 1.0;
 sMathDoubleDefaults[5] = 0.0;
 sMathDoubleDefaults[6] = 0.0;
 sMathDoubleDefaults[7] = 1.0;
 sMathDoubleDefaults[8] = 0.0;
 sMathDoubleDefaults[9] = 0.0;
 sMathDoubleDefaults[10] = 0.0;
 sMathDoubleDefaults[11] = 0.0;
 sMathDoubleDefaults[12] = 0.0;
 sMathDoubleDefaults[13] = 0.0;
 sMathDoubleDefaults[14] = 0.0;
 sMathDoubleDefaults[15] = 0.0;
 sMathDoubleDefaults[16] = 0.0;
 sMathDoubleDefaults[17] = 1.0;
 sMathDoubleDefaults[18] = 0.0;
 sMathDoubleDefaults[19] = 0.0;
 sMathDoubleDefaults[20] = 0.0;
 sMathDoubleDefaults[21] = 1.0;
 sMathDoubleDefaults[22] = 0.0;
 sMathDoubleDefaults[23] = 0.0;
 sMathDoubleDefaults[24] = 0.0;
 sMathDoubleDefaults[25] = 1.0;
 sMathDoubleDefaults[26] = 0.0;
 sMathDoubleDefaults[27] = 0.0;
 sMathDoubleDefaults[28] = 0.0;
 sMathDoubleDefaults[29] = 0.0;
 sMathDoubleDefaults[30] = 0.0;
 sMathDoubleDefaults[31] = 0.0;
 sMathDoubleDefaults[32] = 0.0;
 sMathDoubleDefaults[33] = 0.0;
 sMathDoubleDefaults[34] = 0.0;
 sMathDoubleDefaults[35] = 0.0;
 sMathDoubleDefaults[36] = 0.0;
 sMathDoubleDefaults[37] = 0.0;
 sMathDoubleDefaults[38] = 1.0;
 sMathDoubleDefaults[39] = 0.0;
 sMathDoubleDefaults[40] = 0.0;
 sMathDoubleDefaults[41] = 0.0;
 sMathDoubleDefaults[42] = 0.0;
 sMathDoubleDefaults[43] = 1.0;
 sMathDoubleDefaults[44] = 0.0;
 sMathDoubleDefaults[45] = 0.0;
 sMathDoubleDefaults[46] = 0.0;
 sMathDoubleDefaults[47] = 0.0;
 sMathDoubleDefaults[48] = 1.0;
 sMathDoubleDefaults[49] = 0.0;
 sMathDoubleDefaults[50] = 0.0;
 sMathDoubleDefaults[51] = 0.0;
 sMathDoubleDefaults[52] = 0.0;
 sMathDoubleDefaults[53] = 0.0;
 sMathDoubleDefaults[54] = 0.0;
 sMathDoubleDefaults[55] = 0.0;
 sMathDoubleDefaults[56] = 0.0;
 sMathDoubleDefaults[57] = 0.0;
 sMathDoubleDefaults[58] = 0.0;
 sMathDoubleDefaults[59] = 0.0;
 sMathDoubleDefaults[60] = 0.0;
 sMathDoubleDefaults[61] = 0.0;
 sMathDoubleDefaults[62] = 0.0;
 sMathDoubleDefaults[63] = 0.0;
 sMathDoubleDefaults[64] = 0.0;
 sMathDoubleDefaults[65] = 0.0;
 sMathDoubleDefaults[66] = 1.0;
 sMathDoubleDefaults[67] = 0.0;
 sMathDoubleDefaults[68] = 0.0;
 sMathDoubleDefaults[69] = 0.0;
 sMathDoubleDefaults[70] = 0.0;
 sMathDoubleDefaults[71] = 1.0;
 sMathDoubleDefaults[72] = 0.0;
 sMathDoubleDefaults[73] = 0.0;
 sMathDoubleDefaults[74] = 0.0;
 sMathDoubleDefaults[75] = 0.0;
 sMathDoubleDefaults[76] = 1.0;
 sMathDoubleDefaults[77] = 0.0;
 sMathDoubleDefaults[78] = 0.0;
 sMathDoubleDefaults[79] = 0.0;
 sMathDoubleDefaults[80] = 0.0;
 sMathDoubleDefaults[81] = 1.0;
}

static f32 sQuaternionSingleDefaults[4] = {0,0,0,0};
static f64 sQuaternionDoubleDefaults[4] = {0,0,0,0};
/* 0275713C */
static void initializeQuaternionDefaults() {
 sQuaternionSingleDefaults[0] = 0.0f;
 sQuaternionSingleDefaults[1] = 0.0f;
 sQuaternionSingleDefaults[2] = 0.0f;
 sQuaternionSingleDefaults[3] = 1.0f;
 sQuaternionDoubleDefaults[0] = 0.0;
 sQuaternionDoubleDefaults[1] = 0.0;
 sQuaternionDoubleDefaults[2] = 0.0;
 sQuaternionDoubleDefaults[3] = 1.0;
}

// The initializer writes these consecutive typed scalar regions in the order below.
static u32 sIntegralIdentityDefaults[94] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static f32 sCompositeSingleDefaults[47] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static f64 sCompositeDoubleDefaults[47] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
/* 02757190 */
static void initializeIntegralMathDefaults() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 sIntegralIdentityDefaults[0] = 0;
 sIntegralIdentityDefaults[1] = 0;
 sIntegralIdentityDefaults[2] = 1;
 sIntegralIdentityDefaults[3] = 0;
 sIntegralIdentityDefaults[4] = 0;
 sIntegralIdentityDefaults[5] = 1;
 sIntegralIdentityDefaults[6] = 1;
 sIntegralIdentityDefaults[7] = 1;
 sIntegralIdentityDefaults[8] = 0;
 sIntegralIdentityDefaults[9] = 0;
 sIntegralIdentityDefaults[10] = 0;
 sIntegralIdentityDefaults[11] = 1;
 sIntegralIdentityDefaults[12] = 0;
 sIntegralIdentityDefaults[13] = 0;
 sIntegralIdentityDefaults[14] = 0;
 sIntegralIdentityDefaults[15] = 1;
 sIntegralIdentityDefaults[16] = 0;
 sIntegralIdentityDefaults[17] = 0;
 sIntegralIdentityDefaults[18] = 0;
 sIntegralIdentityDefaults[19] = 1;
 sIntegralIdentityDefaults[20] = 1;
 sIntegralIdentityDefaults[21] = 1;
 sIntegralIdentityDefaults[22] = 1;
 sIntegralIdentityDefaults[23] = 0;
 sIntegralIdentityDefaults[24] = 0;
 sIntegralIdentityDefaults[25] = 0;
 sIntegralIdentityDefaults[26] = 0;
 sIntegralIdentityDefaults[27] = 1;
 sIntegralIdentityDefaults[28] = 0;
 sIntegralIdentityDefaults[29] = 0;
 sIntegralIdentityDefaults[30] = 0;
 sIntegralIdentityDefaults[31] = 0;
 sIntegralIdentityDefaults[32] = 1;
 sIntegralIdentityDefaults[33] = 0;
 sIntegralIdentityDefaults[34] = 0;
 sIntegralIdentityDefaults[35] = 0;
 sIntegralIdentityDefaults[36] = 0;
 sIntegralIdentityDefaults[37] = 1;
 sIntegralIdentityDefaults[38] = 0;
 sIntegralIdentityDefaults[39] = 0;
 sIntegralIdentityDefaults[40] = 0;
 sIntegralIdentityDefaults[41] = 0;
 sIntegralIdentityDefaults[42] = 1;
 sIntegralIdentityDefaults[43] = 1;
 sIntegralIdentityDefaults[44] = 1;
 sIntegralIdentityDefaults[45] = 1;
 sIntegralIdentityDefaults[46] = 1;
 sIntegralIdentityDefaults[47] = 0;
 sIntegralIdentityDefaults[48] = 0;
 sIntegralIdentityDefaults[49] = 1;
 sIntegralIdentityDefaults[50] = 0;
 sIntegralIdentityDefaults[51] = 0;
 sIntegralIdentityDefaults[52] = 1;
 sIntegralIdentityDefaults[53] = 1;
 sIntegralIdentityDefaults[54] = 1;
 sIntegralIdentityDefaults[55] = 0;
 sIntegralIdentityDefaults[56] = 0;
 sIntegralIdentityDefaults[57] = 0;
 sIntegralIdentityDefaults[58] = 1;
 sIntegralIdentityDefaults[59] = 0;
 sIntegralIdentityDefaults[60] = 0;
 sIntegralIdentityDefaults[61] = 0;
 sIntegralIdentityDefaults[62] = 1;
 sIntegralIdentityDefaults[63] = 0;
 sIntegralIdentityDefaults[64] = 0;
 sIntegralIdentityDefaults[65] = 0;
 sIntegralIdentityDefaults[66] = 1;
 sIntegralIdentityDefaults[67] = 1;
 sIntegralIdentityDefaults[68] = 1;
 sIntegralIdentityDefaults[69] = 1;
 sIntegralIdentityDefaults[70] = 0;
 sIntegralIdentityDefaults[71] = 0;
 sIntegralIdentityDefaults[72] = 0;
 sIntegralIdentityDefaults[73] = 0;
 sIntegralIdentityDefaults[74] = 1;
 sIntegralIdentityDefaults[75] = 0;
 sIntegralIdentityDefaults[76] = 0;
 sIntegralIdentityDefaults[77] = 0;
 sIntegralIdentityDefaults[78] = 0;
 sIntegralIdentityDefaults[79] = 1;
 sIntegralIdentityDefaults[80] = 0;
 sIntegralIdentityDefaults[81] = 0;
 sIntegralIdentityDefaults[82] = 0;
 sIntegralIdentityDefaults[83] = 0;
 sIntegralIdentityDefaults[84] = 1;
 sIntegralIdentityDefaults[85] = 0;
 sIntegralIdentityDefaults[86] = 0;
 sIntegralIdentityDefaults[87] = 0;
 sIntegralIdentityDefaults[88] = 0;
 sIntegralIdentityDefaults[89] = 1;
 sIntegralIdentityDefaults[90] = 1;
 sIntegralIdentityDefaults[91] = 1;
 sIntegralIdentityDefaults[92] = 1;
 sIntegralIdentityDefaults[93] = 1;
 sCompositeSingleDefaults[0] = 0.0f;
 sCompositeSingleDefaults[1] = 0.0f;
 sCompositeSingleDefaults[2] = 1.0f;
 sCompositeSingleDefaults[3] = 0.0f;
 sCompositeSingleDefaults[4] = 0.0f;
 sCompositeSingleDefaults[5] = 1.0f;
 sCompositeSingleDefaults[6] = 1.0f;
 sCompositeSingleDefaults[7] = 1.0f;
 sCompositeSingleDefaults[8] = 0.0f;
 sCompositeSingleDefaults[9] = 0.0f;
 sCompositeSingleDefaults[10] = 0.0f;
 sCompositeSingleDefaults[11] = 1.0f;
 sCompositeSingleDefaults[12] = 0.0f;
 sCompositeSingleDefaults[13] = 0.0f;
 sCompositeSingleDefaults[14] = 0.0f;
 sCompositeSingleDefaults[15] = 1.0f;
 sCompositeSingleDefaults[16] = 0.0f;
 sCompositeSingleDefaults[17] = 0.0f;
 sCompositeSingleDefaults[18] = 0.0f;
 sCompositeSingleDefaults[19] = 1.0f;
 sCompositeSingleDefaults[20] = 1.0f;
 sCompositeSingleDefaults[21] = 1.0f;
 sCompositeSingleDefaults[22] = 1.0f;
 sCompositeSingleDefaults[23] = 0.0f;
 sCompositeSingleDefaults[24] = 0.0f;
 sCompositeSingleDefaults[25] = 0.0f;
 sCompositeSingleDefaults[26] = 0.0f;
 sCompositeSingleDefaults[27] = 1.0f;
 sCompositeSingleDefaults[28] = 0.0f;
 sCompositeSingleDefaults[29] = 0.0f;
 sCompositeSingleDefaults[30] = 0.0f;
 sCompositeSingleDefaults[31] = 0.0f;
 sCompositeSingleDefaults[32] = 1.0f;
 sCompositeSingleDefaults[33] = 0.0f;
 sCompositeSingleDefaults[34] = 0.0f;
 sCompositeSingleDefaults[35] = 0.0f;
 sCompositeSingleDefaults[36] = 0.0f;
 sCompositeSingleDefaults[37] = 1.0f;
 sCompositeSingleDefaults[38] = 0.0f;
 sCompositeSingleDefaults[39] = 0.0f;
 sCompositeSingleDefaults[40] = 0.0f;
 sCompositeSingleDefaults[41] = 0.0f;
 sCompositeSingleDefaults[42] = 1.0f;
 sCompositeSingleDefaults[43] = 1.0f;
 sCompositeSingleDefaults[44] = 1.0f;
 sCompositeSingleDefaults[45] = 1.0f;
 sCompositeSingleDefaults[46] = 1.0f;
 sCompositeDoubleDefaults[0] = 0.0;
 sCompositeDoubleDefaults[1] = 0.0;
 sCompositeDoubleDefaults[2] = 1.0;
 sCompositeDoubleDefaults[3] = 0.0;
 sCompositeDoubleDefaults[4] = 0.0;
 sCompositeDoubleDefaults[5] = 1.0;
 sCompositeDoubleDefaults[6] = 1.0;
 sCompositeDoubleDefaults[7] = 1.0;
 sCompositeDoubleDefaults[8] = 0.0;
 sCompositeDoubleDefaults[9] = 0.0;
 sCompositeDoubleDefaults[10] = 0.0;
 sCompositeDoubleDefaults[11] = 1.0;
 sCompositeDoubleDefaults[12] = 0.0;
 sCompositeDoubleDefaults[13] = 0.0;
 sCompositeDoubleDefaults[14] = 0.0;
 sCompositeDoubleDefaults[15] = 1.0;
 sCompositeDoubleDefaults[16] = 0.0;
 sCompositeDoubleDefaults[17] = 0.0;
 sCompositeDoubleDefaults[18] = 0.0;
 sCompositeDoubleDefaults[19] = 1.0;
 sCompositeDoubleDefaults[20] = 1.0;
 sCompositeDoubleDefaults[21] = 1.0;
 sCompositeDoubleDefaults[22] = 1.0;
 sCompositeDoubleDefaults[23] = 0.0;
 sCompositeDoubleDefaults[24] = 0.0;
 sCompositeDoubleDefaults[25] = 0.0;
 sCompositeDoubleDefaults[26] = 0.0;
 sCompositeDoubleDefaults[27] = 1.0;
 sCompositeDoubleDefaults[28] = 0.0;
 sCompositeDoubleDefaults[29] = 0.0;
 sCompositeDoubleDefaults[30] = 0.0;
 sCompositeDoubleDefaults[31] = 0.0;
 sCompositeDoubleDefaults[32] = 1.0;
 sCompositeDoubleDefaults[33] = 0.0;
 sCompositeDoubleDefaults[34] = 0.0;
 sCompositeDoubleDefaults[35] = 0.0;
 sCompositeDoubleDefaults[36] = 0.0;
 sCompositeDoubleDefaults[37] = 1.0;
 sCompositeDoubleDefaults[38] = 0.0;
 sCompositeDoubleDefaults[39] = 0.0;
 sCompositeDoubleDefaults[40] = 0.0;
 sCompositeDoubleDefaults[41] = 0.0;
 sCompositeDoubleDefaults[42] = 1.0;
 sCompositeDoubleDefaults[43] = 1.0;
 sCompositeDoubleDefaults[44] = 1.0;
 sCompositeDoubleDefaults[45] = 1.0;
 sCompositeDoubleDefaults[46] = 1.0;
}

/* 02757E0C */
void sead::CoreWorkList::enterCore() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 u32 first = currentCoreIndex(); u32 firstMapping = sCoreReverse[first];
 u32 second = currentCoreIndex(); u32 secondMapping = sCoreReverse[second];
}
/* 02757E50 */
void sead::CoreWorkList::leaveCore() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); u32 core = currentCoreIndex(); u32 mapping = sCoreReverse[core]; }
/* 02757E80 */
static sead::DebugColor* jobDebugColor(sead::CoreWorkList* list,u32 index) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (!sJobColorGuard) {
  const u8 components[9][4] = {
   {53,212,160,255},{0,170,114,255},{32,127,96,255},
   {191,134,48,255},{255,153,0,255},{255,178,64,255},
   {176,44,75,255},{234,0,55,255},{245,61,104,255}
  };
  for (u32 color=0;color<9;++color) {
   sJobColors[color].mRed = f32(components[color][0])/255.0f;
   sJobColors[color].mGreen = f32(components[color][1])/255.0f;
   sJobColors[color].mBlue = f32(components[color][2])/255.0f;
   sJobColors[color].mAlpha = 1.0f;
  }
  sJobColorGuard = 1;
 }
 return gabi::at<sead::DebugColor>(gabi::ea(sJobColors) + index * 16);
}
/* 0275812C */
void sead::CoreWorkList::rotateColor() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 u32 first = currentCoreIndex(); u32 ignored = sCoreReverse[first];
 u32 second = currentCoreIndex(); u32 core = sCoreReverse[second];
 be<u32>* values = mValues; if (core < (u32)mValueCount) values += core;
 jobDebugColor(this,*values);
 u32 next = (u32)*values + 1;
 *values = next % 9;
}
/* 027581C4 */
void sead::CoreWorkList::releaseCore() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); u32 core = currentCoreIndex(); u32 mapping = sCoreReverse[core]; }

/* 02756AD4 */
u32 sead::PoolHeap::isType(const RuntimeType* type) const {
 if (!sArenaTypeGuard) { sArenaTypeGuard = 1; sArenaType.mTable = &sArenaTypeTable; }
 if (type == &sArenaType) return 1;
 if (!sRegistryHeapTypeGuard) { sRegistryHeapTypeGuard = 1; sRegistryHeapType.mTable = &sHeapTypeTable; }
 return type == &sRegistryHeapType;
}
/* 027579BC */
u32 sead::MessageArchive::open(void* data,AllocationHeap* heap) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 sResourceHeap = heap;
 setMessageAllocators(allocateResourceArray,releaseResourceArray);
 mReader = openMessageReader(data);
 s32 count = messageEntryCount(mReader);
 if (count < 0) { closeMessageReader(mReader); mReader = NULL; mCount = 0; }
 else mCount = count;
 setMessageAllocators(NULL,NULL); sResourceHeap = NULL;
 return count >= 0;
}
/* 02757A80 */
void* sead::MessageArchive::entry(u32 index) const {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (index >= (u32)mCount) return NULL;
 return messageEntry(mReader,index);
}
/* 02757A9C */
u32 sead::MessageArchive::copyEntry(TerminatingBuffer* buffer,u32 index) const {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (s32(buffer->mCapacity) > 255 && index < (u32)mCount) {
  auto table = gabi::at<sead::StringDispatch>((u32)buffer->mVtable);
  gabi::call_ptr<void>(table->mAssureTermination,buffer);
  copyMessageEntry(mReader,index,buffer->mText);
  return 1;
 }
 buffer->mText[0] = 0; return 0;
}

/* 02756D18 */
void sead::ResourceState::allocate(u32 size) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 void* heap = baseSystemHeap(1);
 u32 available = allocatableSystemHeap(heap,4);
 if (size > available) size = available;
 mSize = size;
 mResource = gabi::ea(gabi::call_ptr<void*>(sResourceAllocate,size,4));
}
/* 02756D88 */
static u32 interpolateFixedAngle(f32 ratio) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 f32 scaled = gabi::fmuls_ppc(ratio,128.0f);
 s32 index = gabi::ftoi(scaled);
 f32 fraction = gabi::fsubs_ppc(scaled,f32(index));
 FixedAngleEntry* entry = gabi::at<FixedAngleEntry>(gabi::ea(&sFixedAngleTable) + u32(index) * 8);
 f32 step = entry->mStep;
 f32 delta = gabi::fmuls_ppc(step,fraction);
 u32 increment;
 if (delta < 2147483648.0f) increment = u32(gabi::ftoi(delta));
 else increment = u32(gabi::ftoi(gabi::fsubs_ppc(delta,2147483648.0f))) + 0x80000000;
 return (u32)entry->mBase + increment;
}
/* 02757BB8 */
static void initializeCoreStorage() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function()); constructCoreArray(sCoreReverse,32,4,sead::CoreIdentity::construct); }
/* 02757BD4 */
sead::CoreIdentity* sead::CoreIdentity::construct(CoreIdentity* core) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (core == NULL) { core = static_cast<CoreIdentity*>(allocatePoolObject(4)); if (core == NULL) return core; }
 core->mIndex = 0; return core;
}

/* 02756B94 */
void sead::FrameProvider::initialize(RegistryHeap* heap,void* settings,u32 size,u32 options) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (mInstance != NULL) return;
 void* base = baseSystemHeap(8);
 u32 available = frameSystemCapacity(base,4);
 if (size > available) size = available;
 void* storage = frameSystemAllocate(base,size,4);
 u32 objectSize = providerObjectSize(settings);
 void* object = gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,objectSize,4);
 gabi::FrameLocal<ProviderConstructor> constructor(8);
 constructor->mBaseTable = gabi::ea(&sProviderBaseTable);
 constructor->mTable = gabi::guest_str("sead::ForegroundBucket");
 void* instance = constructProvider(constructor.get(),object,objectSize,storage,size,options);
 mInstance = instance;
 s32 count = sProviderList.mCount;
 s32 capacity = sProviderList.mCapacity;
 if (count < capacity) {
  sProviderList.mEntries[u32(count)] = instance;
  sProviderList.mCount = u32(s32(sProviderList.mCount)) + 1;
 }
}

/* inlined into sead::CoreWorkList::initialize */
static sead::CoreString* initializeCoreString(sead::CoreString* string,void* text) {
 if (string == NULL) { string = static_cast<sead::CoreString*>(allocatePoolObject(8)); if (string == NULL) return string; }
 string->mTable = gabi::ea(&sCoreStringTable); string->mText = static_cast<be<u8>*>(text); return string;
}
/* 02757C10 */
void sead::CoreWorkList::initialize(u32 flags,RegistryHeap* heap) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 s32 count = sCoreCount;
 if (count > 0) {
  RegistryHeap* allocationHeap = heap;
  if (allocationHeap == NULL) allocationHeap = registryCurrentHeap(sRegistryManager.get());
  CoreString* strings = gabi::call_ptr<CoreString*>(allocationHeap->dispatch()->mAllocate.mFunction,allocationHeap,u32(count)*8,4);
  for (u32 index=0; index<u32(count); ++index) {
   CoreString* string = gabi::at<CoreString>(gabi::ea(strings)+index*8);
   if (string != NULL) initializeCoreString(string,&sUnnamedCoreName);
  }
  if (strings != NULL) { mNames = strings; mNameCount = count; }
  count = sCoreCount;
 }
 if (count > 0) {
  RegistryHeap* allocationHeap = heap;
  if (allocationHeap == NULL) allocationHeap = registryCurrentHeap(sRegistryManager.get());
  be<u32>* values = gabi::call_ptr<be<u32>*>(allocationHeap->dispatch()->mAllocate.mFunction,allocationHeap,u32(count)*4,4);
  if (values != NULL) { mValues = values; mValueCount = count; }
 }
 for (u32 index=0; s32(index)<s32(mValueCount); ++index) {
  be<u32>* values = mValues; if (index<(u32)mValueCount) values += index; *values = 0;
 }
 for (u32 index=0; s32(index)<s32(mNameCount); ++index) {
  CoreString* strings = mNames; if (index<(u32)mNameCount) strings = gabi::at<CoreString>(gabi::ea(strings)+index*8);
  strings->mText = reinterpret_cast<be<u8>*>(&sEmptyCoreName);
 }
 mFlags = flags;
}

/* 027581F4 */
sead::JobQueue* sead::JobQueue::constructBase(JobQueue* queue) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (queue == NULL) { queue = static_cast<JobQueue*>(allocatePoolObject(0x5C)); if (queue == NULL) return queue; }
 queue->mTable = reinterpret_cast<JobDispatch*>(&sJobBaseTable);
 initializeCoreString(gabi::at<CoreString>(gabi::ea(queue)+4),&sDefaultJobName);
 queue->mWaitMode = 0;
 queue->mTable = reinterpret_cast<JobDispatch*>(&sJobKindTable);
 CoreIdentity* core=gabi::at<CoreIdentity>(gabi::ea(queue)+0x10);
 if(core==NULL)core=static_cast<CoreIdentity*>(allocatePoolObject(4));
 if(core!=NULL)core->mIndex=0;
 queue->mCoreMask = 0;
 // The original emits null checks for embedded array constructors that do not store.
 if (gabi::ea(&queue->mBatchSize[0]) == 0) allocatePoolObject(12);
 if (gabi::ea(&queue->mActive[0]) == 0) allocatePoolObject(12);
 if (gabi::ea(&queue->mCompleted[0]) == 0) allocatePoolObject(12);
 queue->mState = 0; queue->mKind = gabi::ea(&sCoreWorkKind);
 struct ObjectArrayHeader { be<u32> mCount; be<u32> mData; };
 ObjectArrayHeader* names = gabi::at<ObjectArrayHeader>(gabi::ea(queue)+0x44);
 if (names == NULL) names = static_cast<ObjectArrayHeader*>(allocatePoolObject(8));
 if (names != NULL) { names->mData = 0; names->mCount = 0; }
 ObjectArrayHeader* values = gabi::at<ObjectArrayHeader>(gabi::ea(queue)+0x4C);
 if (values == NULL) values = static_cast<ObjectArrayHeader*>(allocatePoolObject(8));
 if (values != NULL) { values->mData = 0; values->mCount = 0; }
 CoreString* label = gabi::at<CoreString>(gabi::ea(queue)+0x54);
 if (label == NULL) { label = static_cast<CoreString*>(allocatePoolObject(8)); }
 if (label != NULL) initializeCoreString(label,&sDefaultWorkName);
 for (u32 core=0;core<3;++core) { queue->mActive[core]=0; queue->mCompleted[core]=0; }
 for (u32 core=0;core<3;++core) queue->mBatchSize[core]=8;
 return queue;
}
/* 02758724 */
sead::JobQueue* sead::JobQueue::construct(JobQueue* queue) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (queue == NULL) { queue = static_cast<JobQueue*>(allocatePoolObject(sizeof(JobQueue))); if (queue == NULL) return queue; }
 constructBase(queue); queue->mTable = reinterpret_cast<JobDispatch*>(&sJobQueueTable);
 struct JobArrayHeader { be<u32> mCount; be<u32> mData; };
 JobArrayHeader* jobs = gabi::at<JobArrayHeader>(gabi::ea(queue)+0x5C);
 if (jobs == NULL) jobs = static_cast<JobArrayHeader*>(allocatePoolObject(8));
 if (jobs != NULL) { jobs->mData=0; jobs->mCount=0; }
 queue->mState=0; queue->mNext=0; queue->mCount=0; return queue;
}
/* 027587B4 */
void sead::JobQueue::initialize(s32 capacity,RegistryHeap* heap) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 auto table = gabi::at<sead::StringDispatch>((u32)mName.mTable);
 gabi::call_ptr<void>(table->mAssureTermination,&mName);
 mWork.initialize(gabi::ea(mName.mText.get()),heap);
 u32 lockAddress = gabi::ea(&mCore.mIndex);
 while ((u32)mCore.mIndex == 1) {}
 do {
  while (gabi::load_reserved(lockAddress) != 0) {}
 } while (!gabi::store_conditional(lockAddress,1));
 if (capacity > 0) {
  RegistryHeap* allocationHeap = heap;
  if (allocationHeap == NULL) allocationHeap = registryCurrentHeap(sRegistryManager.get());
  be<u32>* jobs = gabi::call_ptr<be<u32>*>(allocationHeap->dispatch()->mAllocate.mFunction,allocationHeap,u32(capacity)*4,4);
  if (jobs != NULL) { mJobs=jobs; mCapacity=capacity; }
 }
 mCount=0; mNext=0; mState=1;
 memoryBarrier();
 do {
  while (gabi::load_reserved(lockAddress) != 1) {}
 } while (!gabi::store_conditional(lockAddress,0));
}

/* 027588DC */
u32 sead::JobQueue::take(u32 limit,be<u32>* consumed,WorkerState* worker) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 *consumed = 0; mWork.enterCore();
 u32 begin = 0; s32 end = -1; u32 count = 0; u32 done = 1;
 if ((u32)mCount != 0 && limit != 0) {
  if (worker != NULL) worker->mState = 3;
  u32 lockAddress = gabi::ea(&mCore.mIndex);
  while ((u32)mCore.mIndex == 1) {}
  do {
   while (gabi::load_reserved(lockAddress) != 0) {}
  } while (!gabi::store_conditional(lockAddress,1));
  if (worker != NULL) worker->mState = 4;
  begin = mNext; u32 total = mCount;
  count = total - begin; if (limit < count) count = limit;
  u32 next = begin + count; end = s32(next - 1); mNext = next;
  done = next >= total;
  memoryBarrier();
  do {
   while (gabi::load_reserved(lockAddress) != 1) {}
  } while (!gabi::store_conditional(lockAddress,0));
 }
 mWork.leaveCore(); mWork.rotateColor();
 if (worker != NULL) worker->mState = 5;
 for (u32 index=begin; s32(index)<=end; ++index) {
  be<u32>* jobs = mJobs; if (index < (u32)mCapacity) jobs += index;
  JobItem* job = gabi::at<JobItem>(*jobs);
  gabi::call_ptr<void>(job->mTable->mExecute,job);
 }
 if (worker != NULL) worker->mState = 6;
 mWork.releaseCore();
 if (worker != NULL) worker->mState = done ? 8 : 7;
 *consumed = count; return done;
}

/* 02759148 */
void sead::WorkerManager::settle() {
 u32 count = mQueueCount; u32 cores = sCoreCount;
 while (count--) { u32 remaining=cores; while (remaining--) {} }
 for (u32 core=0; core<cores; ++core) {
  gptr<WorkerRuntime>* workers = mWorkers; if (core<(u32)mWorkerCount) workers += core;
  WorkerRuntime* worker = *workers;
  s32 delay = worker->mDelay; while (delay > 0) --delay;
  if ((u32)worker->mExtraDelay) { u32 remaining=cores; while (remaining--) {} }
 }
}
/* 027591F4 */
void sead::WorkerManager::submit(u32 name,JobQueue* queue,const be<u32>* mask,const be<u32>* waitMode,const be<u32>* flags) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 gabi::FrameLocal<be<u32> > selected(0x10); *selected.get() = *mask;
 gabi::FrameLocal<be<u32> > mode(0x14); *mode.get() = *waitMode;
 gabi::FrameLocal<be<u32> > beginMask(0x18); *beginMask.get() = *selected.get();
 queue->begin(beginMask.get(),mode.get());
 auto table = reinterpret_cast<be<u32>*>(queue->mTable.get()); gabi::call_ptr<void>(table[5],queue);
 for (u32 core=0; s32(core)<s32(mWorkerCount); ++core) {
  u32 bit=(core&32)?0:1u<<(core&31);
  if ((u32)*selected.get() & bit) {
   gptr<WorkerRuntime>* workers = mWorkers; if (core<(u32)mWorkerCount) workers += core;
   WorkerRuntime* worker = *workers;
   if ((u32)worker->mState != 0) settle();
   gabi::FrameLocal<be<u32> > assignmentFlags(8); *assignmentFlags.get() = *flags;
   assignWorkerJob(worker,name,queue,assignmentFlags.get());
  }
 }
 u32 count=mQueueCount; gptr<JobQueue>* queues=mQueues;
 if (count<(u32)mQueueCapacity) queues+=count;
 *queues=queue; mQueueCount=(u32)mQueueCount+1;
}
/* 02759324 */
void sead::WorkerManager::start() {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 u8 singleThread=mSingleThread; mSubmitted=1;
 if (singleThread) {
  for (u32 index=0; index<(u32)mQueueCount; ++index) {
   gptr<JobQueue>* queues=mQueues; if (index<(u32)mQueueCapacity) queues+=index;
   JobQueue* queue=*queues; auto table=reinterpret_cast<be<u32>*>(queue->mTable.get()); gabi::call_ptr<void>(table[5],queue);
   gabi::FrameLocal<be<u32> > consumed(8); *consumed.get()=0;
   queues=mQueues; if (index<(u32)mQueueCapacity) queues+=index; queue=*queues;
   queue->consume(consumed.get());
   queues=mQueues; if (index<(u32)mQueueCapacity) queues+=index; queue=*queues;
   queue->mCompleted[0]=(u32)queue->mCompleted[0]+(u32)*consumed.get();
   queues=mQueues; if (index<(u32)mQueueCapacity) queues+=index;
   u32 core=currentCoreIndex(); gabi::FrameLocal<CoreIdentity> identity(0xC); identity->mIndex=sCoreReverse[core];
   queue=*queues; queue->finishCore(identity.get());
   for (u32 workerIndex=0;s32(workerIndex)<s32(mWorkerCount);++workerIndex) {
    gptr<WorkerRuntime>* workers=mWorkers; if (workerIndex<(u32)mWorkerCount) workers+=workerIndex;
    finishWorker(*workers);
   }
  }
 } else {
  mSubmission=(u32)mSubmission+1; Pair32 now=systemTime(); mTimeHigh=now.r3; mTimeLow=now.r4;
  for (u32 core=0;s32(core)<s32(mWorkerCount);++core) {
   gptr<WorkerRuntime>* workers=mWorkers; if (core<(u32)mWorkerCount) workers+=core;
   WorkerRuntime* worker=*workers; if ((u32)worker->mEnabled) wakeWorker(worker,1);
  }
 }
}

/* inlined into sead::MessageStyles::open */
static void* allocateStyleStorage(sead::RegistryHeap* heap,u32 size) {
 if (heap == NULL) heap = registryCurrentHeap(sRegistryManager.get());
 return gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction,heap,size,4);
}
/* 0275766C */
u32 sead::MessageStyles::open(void* data,RegistryHeap* heap) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 sMessageHeap = reinterpret_cast<AllocationHeap*>(heap);
 setMessageAllocators(allocateMessageArray,releaseMessageArray);
 mReader = openStyleReader(data);
 setMessageAllocators(NULL,NULL); sMessageHeap = NULL;
 s32 count = styleColorCount(mReader);
 if (count > 0) {
  MessageColor* colors = static_cast<MessageColor*>(allocateStyleStorage(heap,u32(count)*4));
  for (u32 index=0;index<u32(count);++index) {
   MessageColor* color = gabi::at<MessageColor>(gabi::ea(colors)+index*4);
   if (color != NULL) { color->mGreen=0; color->mBlue=0; color->mRed=0; color->mAlpha=255; }
  }
  MessageColor* end;
  if (colors == NULL) { colors=mColors; end=gabi::at<MessageColor>(gabi::ea(colors)+(u32)mColorCount*4); }
  else { mColorCount=count; end=gabi::at<MessageColor>(gabi::ea(colors)+u32(count)*4); mColors=colors; }
  MessageColor* begin=colors;
  gabi::FrameLocal<MessageColor> color(8);
  while (colors != end) {
   u32 index=(gabi::ea(colors)-gabi::ea(begin))>>2;
   styleColor(mReader,index,color.get());
   u8 green=color->mGreen,red=color->mRed,alpha=color->mAlpha,blue=color->mBlue;
   colors->mAlpha=alpha;colors->mGreen=green;colors->mRed=red;colors->mBlue=blue;
   colors=gabi::at<MessageColor>(gabi::ea(colors)+4);
  }
 }
 count=styleCount(mReader);
 if (count>0) {
  MessageStyle* styles=static_cast<MessageStyle*>(allocateStyleStorage(heap,u32(count)*16));
  MessageStyle* end;
  if (styles==NULL) { styles=mStyles;end=gabi::at<MessageStyle>(gabi::ea(styles)+(u32)mStyleCount*16); }
  else { mStyleCount=count;end=gabi::at<MessageStyle>(gabi::ea(styles)+u32(count)*16);mStyles=styles; }
  MessageStyle* begin=styles;
  while (styles!=end) {
   u32 index=(gabi::ea(styles)-gabi::ea(begin))>>4;
   styles->mFont=styleFont(mReader,index); styles->mSize=styleSize(mReader,index);
   s32 color=styleColorIndex(mReader,index); if (color<0) color=-1; styles->mColor=color;
   s32 shadow=styleShadowIndex(mReader,index);if (shadow<0) shadow=-1;styles->mShadow=shadow;
   styles=gabi::at<MessageStyle>(gabi::ea(styles)+16);
  }
 }
 count=stylePairCount(mReader);
 if (count>0) {
  MessagePair* pairs=static_cast<MessagePair*>(allocateStyleStorage(heap,u32(count)*8));
  MessagePair* end;
  if (pairs==NULL) { pairs=mPairs;end=gabi::at<MessagePair>(gabi::ea(pairs)+(u32)mPairCount*8); }
  else { mPairCount=count;end=gabi::at<MessagePair>(gabi::ea(pairs)+u32(count)*8);mPairs=pairs; }
  MessagePair* begin=pairs;
  while (pairs!=end) {
   u32 index=(gabi::ea(pairs)-gabi::ea(begin))>>3;
   pairs->mGroup=stylePairGroup(mReader,index);pairs->mValue=stylePairValue(mReader,index);
   pairs=gabi::at<MessagePair>(gabi::ea(pairs)+8);
  }
 }
 s32 fallback=styleFallback(mReader);if(fallback<0)fallback=0;mFallback=fallback;return 1;
}

/* 02758B88 */
sead::WorkerManager* sead::WorkerManager::construct(WorkerManager* manager) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 if (manager==NULL) { manager=static_cast<WorkerManager*>(allocatePoolObject(sizeof(WorkerManager)));if(manager==NULL)return manager; }
 manager->mTable=gabi::ea(&sWorkerManagerTable);
 EmbeddedObject* entry=gabi::at<EmbeddedObject>(gabi::ea(manager)+4);
 if(entry==NULL)entry=static_cast<EmbeddedObject*>(allocatePoolObject(sizeof(EmbeddedObject)));
 if(entry!=NULL) {
  constructRegistryBase(entry);entry->mRegistration.mPrevious=0;entry->mRegistered=0;entry->mOwner=NULL;
  entry->mHeader.mTable=gabi::ea(&sRegistryEntryTable);entry->mRegistrationData=gabi::ea(entry);entry->mRegistration.mNext=0;
  RegistryCallback* callback=gabi::at<RegistryCallback>(gabi::ea(entry)+0x24);
  if(callback!=NULL) {
   callback->mOwner=manager;callback->mKind=0;callback->mTable=gabi::ea(&sRegistryCallbackTable);
   callback->mMethod=gabi::guest_fn(reinterpret_cast<const void*>(workerManagerCallback));callback->mAdjustment=-1;
  }
  entry->mCallback=callback;
 }
 struct ObjectArrayHeader { be<u32> mCount; be<u32> mData; };
 ObjectArrayHeader* workers=gabi::at<ObjectArrayHeader>(gabi::ea(manager)+0x40);
 if(workers==NULL)workers=static_cast<ObjectArrayHeader*>(allocatePoolObject(8));
 if(workers!=NULL){workers->mData=0;workers->mCount=0;}
 ObjectArrayHeader* queues=gabi::at<ObjectArrayHeader>(gabi::ea(manager)+0x48);
 if(queues==NULL)queues=static_cast<ObjectArrayHeader*>(allocatePoolObject(8));
 if(queues!=NULL){queues->mData=0;queues->mCount=0;}
 manager->mReservedMode=0;manager->mSubmission=0;manager->mSingleThread=0;
 clearWorkerClock(&manager->mTimeHigh);manager->mSubmitted=0;return manager;
}

/* inlined into sead::WorkerManager::initialize */
static u32 workerNameLength(const be<u8>* text) {
 u32 length=0;
 if ((u8)*text != 0) {
  do { ++length; text=gabi::at<be<u8> >(gabi::ea(text)+1); if(s32(length)>262144)return 0; }
  while ((u8)*text != 0);
 }
 return length;
}
/* inlined into sead::WorkerManager::initialize */
static void assureWorkerName(sead::TerminatingBuffer* buffer) {
 auto table=gabi::at<sead::StringDispatch>((u32)buffer->mVtable);gabi::call_ptr<void>(table->mAssureTermination,buffer);
}
/* 02758CA8 */
void sead::WorkerManager::initialize(WorkerManagerConfig* config) {
 auto nativeFrame = gabi::AutoFrame(gabi::current_function());
 RegistryHost* host=sRegistryHost;
 if(host!=NULL) {
  LinkList* list=&host->mWorkers;
  if((u8)mObject.mRegistered) {
   LinkList* owner=mObject.mOwner;
   if(owner!=NULL){mObject.mOwner=NULL;mObject.mRegistration.detach();owner->mCount=(u32)owner->mCount-1;}
   mObject.mRegistered=0;
  }
  LinkList* owner=mObject.mOwner;
  if(owner!=NULL){mObject.mOwner=NULL;mObject.mRegistration.detach();owner->mCount=(u32)owner->mCount-1;}
  mObject.mOwner=list;list->insertBefore(&mObject.mRegistration);list->mCount=(u32)list->mCount+1;mObject.mRegistered=1;
 }
 s32 count=sCoreCount;
 RegistryHeap* heap=registryCurrentHeap(sRegistryManager.get());
 if(count>0) {
  RegistryHeap* allocationHeap=heap;
  if(allocationHeap==NULL)allocationHeap=registryCurrentHeap(sRegistryManager.get());
  gptr<WorkerRuntime>* workers=gabi::call_ptr<gptr<WorkerRuntime>*>(allocationHeap->dispatch()->mAllocate.mFunction,allocationHeap,u32(count)*4,4);
  if(workers!=NULL){mWorkers=workers;mWorkerCount=count;}
 }
 if(count!=0) {
  gabi::FrameLocal<be<u8>[128]> text(0x24);
  gabi::FrameLocal<TerminatingBuffer> buffer(0x18);
  for(u32 core=0,remaining=u32(count);remaining!=0;++core,--remaining) {
   u32 stackSize=config->mStackSize[core];s32 queueCapacity=config->mQueueCapacity;u32 priority=config->mPriority[core];
   WorkerRuntime* worker=constructWorker(NULL,this,queueCapacity,stackSize,priority);
   buffer->mText=reinterpret_cast<be<u8>*>(text.get());buffer->mCapacity=128;buffer->mVtable=gabi::ea(&sWorkerNameViewTable);
   (*text.get())[0]=0;(*text.get())[127]=0;gabi::cpu->cr[6]=0;
   formatWorkerName(buffer.get(),"%s/Worker%d(%s)",config->mName,core,"");
   TerminatingBuffer* name=static_cast<TerminatingBuffer*>(allocatePoolObject(12));
   if(name!=NULL) {
    assureWorkerName(buffer.get());u32 capacity=workerNameLength(buffer->mText)+1;
    be<u8>* nameText=static_cast<be<u8>*>(allocateSupportArray(capacity,reinterpret_cast<AllocationHeap*>(heap),4));
    if(nameText!=NULL)initializeAllocatedString(nameText,capacity);
    assureWorkerName(buffer.get());u32 length=workerNameLength(buffer->mText);
    TerminatingBuffer* constructed=name;
    if(constructed==NULL)constructed=static_cast<TerminatingBuffer*>(allocatePoolObject(12));
    if(constructed!=NULL){
     constructed->mText=nameText;constructed->mVtable=gabi::ea(&sWorkerNameBaseTable);constructed->mCapacity=length+1;
     if(s32(length+1)<=0){constructed->mCapacity=0;constructed->mText=NULL;}
     else constructed->mText[(u32)constructed->mCapacity-1]=0;
    }
    name->mVtable=gabi::ea(&sWorkerNameTable);nameText=name->mText;
    assureWorkerName(buffer.get());length=workerNameLength(buffer->mText);
    s32 nameCapacity=name->mCapacity;
    auto sourceTable=gabi::at<sead::StringDispatch>((u32)buffer->mVtable);u32 assure=sourceTable->mAssureTermination;
    if(s32(length)>=nameCapacity)length=u32(nameCapacity-1);
    gabi::call_ptr<void>(assure,buffer.get());
    copyWorkerName(nameText,buffer->mText,length,0);nameText[length]=0;
   }
   worker->mName=name->mText;
   gptr<WorkerRuntime>* workers=mWorkers;if(core<(u32)mWorkerCount)workers+=core;*workers=worker;
   worker->mEnabled=core;
   if(core!=0) {
    gabi::FrameLocal<be<u32> > affinity(0x10);*affinity.get()=(core&32)?0:1u<<(core&31);
    setWorkerAffinity(worker,affinity.get());
    auto table=reinterpret_cast<be<u32>*>(worker->mTable.get());gabi::call_ptr<void>(table[11],worker);
   }
  }
 }
 RegistryHeap* allocationHeap=registryCurrentHeap(sRegistryManager.get());
 gptr<JobQueue>* queues=gabi::call_ptr<gptr<JobQueue>*>(allocationHeap->dispatch()->mAllocate.mFunction,allocationHeap,256,4);
 if(queues!=NULL){mQueues=queues;mQueueCapacity=64;}mQueueCount=0;
}

/* WWHD glue
 ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
 data sIntegralIdentityDefaults 104A0790
 data sCompositeSingleDefaults 104A0908
 data sCompositeDoubleDefaults 104A09C8
 data sMathSingleDefaults 104A0384
 data sMathDoubleDefaults 104A04D0
 data sQuaternionSingleDefaults 104A0760
 data sQuaternionDoubleDefaults 104A0770
 data sPoolCreationLock 104A0328
 data sCoreCount 101F8B5C
 data sCoreOrder 104A0B40
 data sJobColors 104A0C40
 data sJobColorGuard 104A0CD0
 data sArenaTypeGuard 101FD770
 data sArenaType 101FDFA4
 data sArenaTypeTable 10145E68
 data sHeapTypeTable 10145E18
 data sFixedAngleTable 10146FB0
 data sResourceAllocate C1001000
 data sProviderList 104A0364
 str 10145F78 "sead::ForegroundBucket"
 data sProviderBaseTable 10145F60
 data sUnnamedCoreName 10147411
 data sEmptyCoreName 10147410
 data sCoreStringTable 10147414
 data sJobBaseTable 1014743C
 data sJobKindTable 101474B8
 data sJobQueueTable 101474E8
 data sCoreWorkKind 101474B0
 data sDefaultJobName 101474AC
 data sDefaultWorkName 10147412
 data sWorkerManagerTable 101475F0
 data sRegistryEntryTable 10147584
 data sRegistryCallbackTable 101475C4
 data sRegistryHost 101F8BD4
 data sWorkerNameBaseTable 10147534
 data sWorkerNameTable 1014754C
 data sWorkerNameViewTable 101475AC
 data sPoolHeapTable 10145EA8
 data sPoolTreeTable 10145E58
 data sStorageOwner 104A0380
 data sMessageHeap 101F8B54
 data sResourceHeap 101F8B58
 ext 027672B8 void bindMessageAllocatorAddresses(u32,u32)
 ext 0273A0C4 void* openMessageReader(void*)
 ext 0273A1A0 s32 messageEntryCount(void*)
 ext 0273A15C void closeMessageReader(void*)
 ext 0273A32C void* messageEntry(void*,u32)
 ext 0273A4B4 void copyMessageEntry(void*,u32,void*)
 ext C0009790 void* baseSystemHeap(u32)
 ext C0009778 u32 allocatableSystemHeap(void*,u32)
 ext 028EFFD0 void constructCoreArray(void*,u32,u32,CoreConstruct)
 ext C0009780 u32 frameSystemCapacity(void*,u32)
 ext C00096A0 void* frameSystemAllocate(void*,u32,u32)
 ext 02769A50 u32 providerObjectSize(void*)
 ext 02769A04 void* constructProvider(sead::ProviderConstructor*,void*,u32,void*,u32,u32)
 str 101475D4 "%s/Worker%d(%s)"
 str 10147518 "" in 02758CA8
 ext 0273B310 sead::Link* sead::Link::detach()
 ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
 ext 0276A628 sead::WorkerRuntime* constructWorker(sead::WorkerRuntime*,sead::WorkerManager*,s32,u32,u32)
 ext 02759C28 void formatWorkerName(sead::TerminatingBuffer*,const char*,void*,u32,const char*)
 ext 028F521C void initializeAllocatedString(void*,u32)
 ext C0009988 void copyWorkerName(void*,void*,u32,u32)
 ext 02760C4C void setWorkerAffinity(sead::WorkerRuntime*,const be<u32>*)
 ext 02752A84 void constructRegistryBase(sead::EmbeddedObject*)
 ext 02760DE8 void clearWorkerClock(void*)
 ext 02759810 void workerManagerCallback(sead::WorkerManager*) callback
 ext 0273A5C8 void* openStyleReader(void*)
 ext 0273A768 s32 styleColorCount(void*)
 ext 0273A790 void styleColor(void*,u32,sead::MessageColor*)
 ext 0273A8C0 s32 styleCount(void*)
 ext 0273A924 u32 styleFont(void*,u32)
 ext 0273A954 u32 styleSize(void*,u32)
 ext 0273A984 s32 styleColorIndex(void*,u32)
 ext 0273A9D4 s32 styleShadowIndex(void*,u32)
 ext 0273A898 s32 stylePairCount(void*)
 ext 0273A830 u32 stylePairGroup(void*,u32)
 ext 0273A868 u32 stylePairValue(void*,u32)
 ext 0273A700 s32 styleFallback(void*)
 ext 0276A780 void assignWorkerJob(sead::WorkerRuntime*,u32,sead::JobQueue*,const be<u32>*)
 ext 0276A8AC void finishWorker(sead::WorkerRuntime*)
 ext 0276A928 void wakeWorker(sead::WorkerRuntime*,u32)
 ext C0009C98 Pair32 systemTime()
 ext C0009BB0 u32 currentCoreIndex()
 ext C0009E90 void memoryBarrier()
 ext 0273AF40 void releasePoolObject(void*)
 ext 0273AFC8 void releaseArrayObject(void*)
 ext 0273AD10 void* allocatePoolObject(u32)
 ext 0273B0D4 void* allocateSupportArray(u32,sead::AllocationHeap*,s32)
 ext 027601BC void lockPool(void*)
 ext 027601F0 void unlockPool(void*)
 ext 0275589C sead::RegistryHeap* constructHeapBase(sead::RegistryHeap*,const sead::SafeText*,sead::RegistryHeap*,u32,u32,s32,u32)
 ext 02755C24 void sead::RegistryHeap::detachAndDispose()
 ext 02755AB4 void sead::RegistryHeap::disposeRange(u32,u32)
 ext 02755A48 void sead::RegistryHeap::destroy(u32)
*/

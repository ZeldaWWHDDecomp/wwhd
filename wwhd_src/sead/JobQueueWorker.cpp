// Worker lifecycle and message dispatch, from local WWHD instructions.
#include "sead/JobQueueWorker.h"
#include "sead/HeapRegistry.h"
using namespace sead;
static SafeTextVtable sJobWorkerTextTable;
static u32 sJobWorkerTable;
static u32 sJobWorkerEventTable;
void* allocateJobWorkerStorage(u32 size);
void releaseJobWorkerStorage(void* object);
void constructJobWorkerThread(JobQueueWorker_l* object,const SafeText* name,u32 zero,
    u32 priority,u32 anotherZero,u32 limit,u32 affinity,u32 mode);
void initializeJobWorkerClock(void* clock);
AllocationHeap* jobWorkerAllocationHeap(RegistryManagerState* manager);
void destroyJobWorkerEventBase(JobWorkerEvent_l* event,u32 flags);

/* 0276A614 */
void job_queue_text::Text::destroy(u32 flags) {
    if(this && (flags&1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseJobWorkerStorage),this);
}
/* 0276A628 */
JobQueueWorker_l* JobQueueWorker_l::create(JobQueueWorker_l* object,u32 owner,s32 capacity,u32 affinity,u32 priority) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!object) object=static_cast<JobQueueWorker_l*>(allocateJobWorkerStorage(sizeof(JobQueueWorker_l)));
    if(!object) return object;
    gabi::FrameLocal<SafeText> name(8);
    name->mVtable=&sJobWorkerTextTable;
    name->mText=const_cast<be<u8>*>(gabi::at<const be<u8>>(gabi::guest_str("Worker")));
    constructJobWorkerThread(object,name.get(),0,priority,0,0x7FFFFFFF,affinity,1);
    object->mState=0;
    object->mOwner=owner;
    object->mWorkerIndex=0;
    object->mObject.mTable=gabi::ea(&sJobWorkerTable);
    WorkerDeque_l* queue=gabi::at<WorkerDeque_l>(gabi::ea(object)+offsetof(JobQueueWorker_l,mQueue));
    if(!queue) queue=static_cast<WorkerDeque_l*>(allocateJobWorkerStorage(sizeof(WorkerDeque_l)));
    if(queue) {
        queue->mCapacity=0;
        queue->mHead=0;
        queue->mItems=nullptr;
        queue->mCount=0;
    }
    be<u32>* lock=gabi::at<be<u32>>(gabi::ea(object)+offsetof(JobQueueWorker_l,mQueueLock));
    if(!lock) lock=static_cast<be<u32>*>(allocateJobWorkerStorage(sizeof(be<u32>)));
    if(lock) *lock=0;
    object->mCurrentContext=0;
    object->mCycles=0;
    object->mCurrentJob=nullptr;
    initializeJobWorkerClock(&object->mCycleStartHigh);
    if(capacity>0) {
        AllocationHeap* heap=jobWorkerAllocationHeap(sRegistryManager);
        void* items=gabi::call_ptr<void*>(heap->mTable->mAllocate,heap,static_cast<u32>(capacity)*4,4);
        if(items) {
            object->mQueue.mCount=0;
            object->mQueue.mHead=0;
            object->mQueue.mItems=static_cast<gptr<QueuedJob_l>*>(items);
            object->mQueue.mCapacity=capacity;
        }
    }
    return object;
}
void publishJobWorkerMemory();
static void acquireJobWorkerQueue(be<u32>* lock) {
    u32 address=gabi::ea(lock);
    // The preliminary busy poll is folded into the reservation loop; acquisition still requires 0 -> 1.
    do { while(gabi::load_reserved(address)!=0) {} }
    while(!gabi::store_conditional(address,1));
}
static void releaseJobWorkerQueue(be<u32>* lock) {
    u32 address=gabi::ea(lock);
    do { while(gabi::load_reserved(address)!=1) {} }
    while(!gabi::store_conditional(address,0));
}
/* 0276A780 */
u32 JobQueueWorker_l::enqueue(u32 context,QueuedJob_l* job,const be<u32>* prepend) {
    gabi::AutoFrame frame{gabi::current_function()};
    acquireJobWorkerQueue(&mQueueLock);
    u32 result=0;
    s32 count=mQueue.mCount;
    if(count<static_cast<s32>(mQueue.mCapacity)) {
        s32 index;
        if(static_cast<u32>(*prepend)==0) {
            s32 head=mQueue.mHead;
            s32 capacity=mQueue.mCapacity;
            index=static_cast<s32>(static_cast<u32>(head)+static_cast<u32>(count));
            auto* items=mQueue.mItems.get();
            mQueue.mCount=static_cast<s32>(static_cast<u32>(count)+1);
            if(index>=capacity) index=static_cast<s32>(static_cast<u32>(index)-static_cast<u32>(capacity));
            items[static_cast<u32>(index)]=job;
        } else {
            s32 head=static_cast<s32>(static_cast<u32>(mQueue.mHead)-1);
            count=mQueue.mCount;
            mQueue.mHead=head;
            if(head<0) mQueue.mHead=static_cast<s32>(static_cast<u32>(mQueue.mCapacity)-1);
            s32 capacity=mQueue.mCapacity;
            index=mQueue.mHead;
            mQueue.mCount=static_cast<s32>(static_cast<u32>(count)+1);
            auto* items=mQueue.mItems.get();
            if(index>=capacity) index=static_cast<s32>(static_cast<u32>(index)-static_cast<u32>(capacity));
            items[static_cast<u32>(index)]=job;
        }
        result=1;
    }
    job->mContext=context;
    publishJobWorkerMemory();
    releaseJobWorkerQueue(&mQueueLock);
    return result;
}
/* 0276A8AC */
void JobQueueWorker_l::clear() {
    gabi::AutoFrame frame{gabi::current_function()};
    acquireJobWorkerQueue(&mQueueLock);
    mQueue.mCount=0; mQueue.mHead=0;
    publishJobWorkerMemory();
    releaseJobWorkerQueue(&mQueueLock);
}
/* 0276A95C */
QueuedJob_l* JobQueueWorker_l::dequeue() {
    gabi::AutoFrame frame{gabi::current_function()};
    acquireJobWorkerQueue(&mQueueLock);
    QueuedJob_l* result=nullptr;
    if(static_cast<s32>(mQueue.mCount)>0) {
        s32 head=mQueue.mHead,capacity=mQueue.mCapacity,index=head;
        auto* items=mQueue.mItems.get();
        if(index>=capacity) index=static_cast<s32>(static_cast<u32>(index)-static_cast<u32>(capacity));
        s32 count=mQueue.mCount;
        head=static_cast<s32>(static_cast<u32>(head)+1);
        result=items[static_cast<u32>(index)];
        mQueue.mHead=head;
        if(head>=capacity) mQueue.mHead=0;
        mQueue.mCount=static_cast<s32>(static_cast<u32>(count)-1);
    }
    publishJobWorkerMemory();
    releaseJobWorkerQueue(&mQueueLock);
    return result;
}
/* 0276A928 */
u32 JobQueueWorker_l::start(u32 message) {
    JobWorkerDispatch_l* table=dispatch();
    mState=1;
    u32 target=table->mWake.mFunction;
    u32 state=mState;
    (void)state;
    // HD: the worker thread's sendMessage(message, nonBlocking): the original leaves the state's 1 in r5.
    return gabi::tail_ptr<u32>(target,this,message,1u);
}
/* 0276A944 */
u32 JobQueueWorker_l::onMessage(u32 message) {
    if(message==1) return gabi::tail_ptr<u32>(dispatch()->mAwaitWake.mFunction,this);
    return gabi::ea(this);
}
// These bindings preserve the two-register timer return and existing queue boundary.
Pair32 readJobWorkerTime();
QueuedJob_l* dequeueJobWorker(JobQueueWorker_l* worker);
void finishQueuedJob(QueuedJob_l* job,const be<u32>* workerIndex);
/* 0276AA34 */
void JobQueueWorker_l::processJobs() {
    gabi::AutoFrame frame{gabi::current_function()};
    mCycles=static_cast<u32>(mCycles)+1;
    auto time=readJobWorkerTime();
    mCycleStartHigh=time.r3; mCycleStartLow=time.r4;
    mState=2;
    QueuedJob_l* job=dequeueJobWorker(this);
    mCurrentJob=job;
    // The binary publishes the first job's context only, even across later dequeues.
    if(job) mCurrentContext=job->mContext;
    gabi::FrameLocal<be<u32>> produced(8);
    gabi::FrameLocal<be<u32>> workerIndex(12);
    while(job) {
        u32 complete;
        do {
            u32 index=mWorkerIndex;
            *produced=0;
            u32 target=job->mDispatch->mExecute.mFunction;
            u32 input=job->mInput[index<3?index:0];
            complete=gabi::call_ptr<u32>(target,job,input,produced.get(),this);
            index=mWorkerIndex;
            auto* completed=&job->mCompleted[index<3?index:0];
            *completed=static_cast<u32>(*completed)+static_cast<u32>(*produced);
        } while(!complete);
        mState=9;
        u32 state=mState;
        *workerIndex=mWorkerIndex;
        finishQueuedJob(job,workerIndex.get());
        (void)state;
        mState=10;
        u32 completedState=mState;
        (void)completedState;
        mState=2;
        // The state reads are present in the original; dequeue does not consume them.
        job=dequeueJobWorker(this);
    }
    mState=0;
    u32 idleState=mState;
    (void)idleState;
}
/* 0276AB94 */
void job_event_text::Text::destroy(u32 flags) {
    if(this && (flags&1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseJobWorkerStorage),this);
}
/* 0276ABA8 */
void job_event_text::Text::assureTerminationImpl() {}
/* 0276ABAC */
u32 JobQueueWorker_l::endCycle() {
    return gabi::tail_ptr<u32>(dispatch()->mEndCycle.mFunction,this);
}
/* 0276ABBC */
void JobWorkerEvent_l::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this) {
        mTable=gabi::ea(&sJobWorkerEventTable);
        destroyJobWorkerEventBase(this,0);
        if(flags&1) releaseJobWorkerStorage(this);
    }
}
/* WWHD glue
 ext C0009E90 void publishJobWorkerMemory()
 ext C0009C98 Pair32 readJobWorkerTime()
 ext 0276A95C sead::QueuedJob_l* dequeueJobWorker(sead::JobQueueWorker_l*)
 ext 027586C4 void finishQueuedJob(sead::QueuedJob_l*,const be<u32>*)
 ext 0273AD10 void* allocateJobWorkerStorage(u32)
 ext 0273AF40 void releaseJobWorkerStorage(void*) callback
 ext 027607C0 void constructJobWorkerThread(sead::JobQueueWorker_l*,const sead::SafeText*,u32,u32,u32,u32,u32,u32)
 ext 02760DE8 void initializeJobWorkerClock(void*)
 ext 027609AC void destroyJobWorkerEventBase(sead::JobWorkerEvent_l*,u32)
 ext 02756140 sead::AllocationHeap* jobWorkerAllocationHeap(sead::RegistryManagerState*)
 data sJobWorkerTextTable 1015F8F4
 data sJobWorkerTable 1015F924
 data sJobWorkerEventTable 1015F90C
 str 1015F91C "Worker"
*/

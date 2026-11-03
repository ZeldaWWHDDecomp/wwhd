// WWHD worker/deque prefixes, reconstructed from the local instructions.
#pragma once
#include "sead/FoundationObjects.h"
#include "sead/FileDevice.h"
#include "sead/Allocation.h"
namespace sead {
struct QueuedJobDispatch_l {
    VtableSlot mHeader;
    VtableSlot mDestroy;
    VtableSlot mReserved;
    VtableSlot mExecute;
};
struct QueuedJob_l {
    gptr<QueuedJobDispatch_l> mDispatch;
    u8 mJobPrefix[0x14];
    be<u32> mInput[3];
    u8 mJobContext[12];
    be<u32> mCompleted[3];
    u8 mJobFlags[4];
    be<u32> mContext;
};
struct WorkerDeque_l {
    gptr<gptr<QueuedJob_l>> mItems;
    be<s32> mCapacity;
    be<s32> mHead;
    be<s32> mCount;
};
struct JobWorkerDispatch_l {
    VtableSlot mHeader;
    VtableSlot mDestroy;
    VtableSlot mThreadType;
    VtableSlot mWake;
    u8 mBeforeEndCycle[0x28];
    VtableSlot mEndCycle;
    u8 mBeforeAwaitWake[0x38];
    VtableSlot mAwaitWake;
};
struct JobQueueWorker_l {
    ObjectHeader mObject;
    u8 mThreadData[0x84];
    be<u32> mWorkerIndex;
    be<u32> mState;
    be<u32> mOwner;
    WorkerDeque_l mQueue;
    be<u32> mQueueLock;
    gptr<QueuedJob_l> mCurrentJob;
    be<u32> mCurrentContext;
    be<u32> mCycles;
    be<u32> mCycleStartHigh;
    be<u32> mCycleStartLow;
    static JobQueueWorker_l* create(JobQueueWorker_l* object,u32 owner,s32 capacity,u32 affinity,u32 priority);
    u32 enqueue(u32 context,QueuedJob_l* job,const be<u32>* prepend);
    void clear();
    u32 start();
    u32 onMessage(u32 message);
    QueuedJob_l* dequeue();
    void processJobs();
    u32 endCycle();
    JobWorkerDispatch_l* dispatch() const { return gabi::at<JobWorkerDispatch_l>(mObject.mTable); }
};
struct JobWorkerEvent_l {
    u8 mPrefix[0x18];
    be<u32> mTable;
    void destroy(u32 flags);
};
namespace job_queue_text { struct Text : SafeText { void destroy(u32 flags); }; }
namespace job_event_text { struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); }; }
}
WWHD_OFFSET(sead::QueuedJobDispatch_l,mExecute,0x18);
WWHD_OFFSET(sead::QueuedJob_l,mInput,0x18);
WWHD_OFFSET(sead::QueuedJob_l,mCompleted,0x30);
WWHD_OFFSET(sead::QueuedJob_l,mContext,0x40);
WWHD_SIZE(sead::QueuedJob_l,0x44);
WWHD_SIZE(sead::WorkerDeque_l,16);
WWHD_OFFSET(sead::JobWorkerDispatch_l,mWake,0x18);
WWHD_OFFSET(sead::JobWorkerDispatch_l,mEndCycle,0x48);
WWHD_OFFSET(sead::JobWorkerDispatch_l,mAwaitWake,0x88);
WWHD_OFFSET(sead::JobQueueWorker_l,mWorkerIndex,0x94);
WWHD_OFFSET(sead::JobQueueWorker_l,mState,0x98);
WWHD_OFFSET(sead::JobQueueWorker_l,mQueue,0xA0);
WWHD_OFFSET(sead::JobQueueWorker_l,mQueueLock,0xB0);
WWHD_OFFSET(sead::JobQueueWorker_l,mCurrentJob,0xB4);
WWHD_OFFSET(sead::JobQueueWorker_l,mCycleStartHigh,0xC0);
WWHD_SIZE(sead::JobQueueWorker_l,0xC8);

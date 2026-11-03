/* Worker queue completion helpers; canonical manager/worker/queue layouts belong to SEAD#14. */
#pragma once
#include "sead/WorkerRuntimeInstances.h"
#include "sead/PoolHeap.h"
namespace sead { namespace worker_runtime {
/* The worker runtime shares JobItemTable's prefix; this TU calls its later wait slot. */
struct WorkerWaitDispatch : JobItemTable {
    u8 mBeforeWait[0x7C];
    be<u32> mWait;
};
u32 allWorkersIdle(WorkerManager* manager);
void drainWorkerQueues(WorkerManager* manager);
WWHD_OFFSET(WorkerWaitDispatch,mWait,0x8C);
WWHD_OFFSET(WorkerRuntime,mTable,0xC);
WWHD_OFFSET(WorkerRuntime,mState,0x98);
WWHD_OFFSET(JobQueue,mCoreMask,0x14);
WWHD_OFFSET(JobQueue,mCompleted,0x30);
WWHD_OFFSET(WorkerManager,mReservedMode,0x58);
} }
sead::worker_runtime::CounterWord* constructWorkerCounter(sead::worker_runtime::CounterWord*);

/* Worker queue draining and idle observation, reconstructed from local instructions. */
#include "sead/WorkerQueueSupport.h"
using namespace sead;
using namespace sead::worker_runtime;
extern void constructWorkerCounters(void*,u32,u32,u32);
struct CounterSnapshot { CounterWord mTokens[256]; };
static bool allWorkersIdleNow(WorkerManager* manager) {
    s32 count=manager->mWorkerCount;
    if(count<=0) return true;
    auto workers=manager->mWorkers.get();
    for(u32 index=0;index<static_cast<u32>(count);++index) {
        WorkerRuntime* worker=workers[index<static_cast<u32>(count)?index:0];
        if(static_cast<u32>(worker->mState)!=0) return false;
    }
    return true;
}
static JobQueue* queuedWorkerJob(WorkerManager* manager,u32 index) {
    u32 capacity=manager->mQueueCapacity;
    auto jobs=manager->mQueues.get();
    return jobs[index<capacity?index:0];
}
/* 027594FC */
u32 sead::worker_runtime::allWorkersIdle(WorkerManager* manager) {
    gabi::AutoFrame frame{gabi::current_function()};
    return allWorkersIdleNow(manager);
}
/* 02759564 */
void sead::worker_runtime::drainWorkerQueues(WorkerManager* manager) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(static_cast<u32>(manager->mSingleThread)==0) {
        gabi::FrameLocal<CounterWord> progressIndex(8);
        for(u32 index=0;index<static_cast<u32>(manager->mQueueCount);++index) {
            // SEAD#14 names this offset58 word mReservedMode; here it holds the active queue.
            manager->mReservedMode=gabi::ea(queuedWorkerJob(manager,index));
            JobQueue* job=queuedWorkerJob(manager,index);
            progressIndex->mValue=0;
            u32 shift=progressIndex->mValue;
            u32 mask=shift<32?1u<<shift:0;
            if(static_cast<u32>(job->mCoreMask)&mask) {
                WorkerRuntime* worker=manager->mWorkers[0];
                auto table=static_cast<WorkerWaitDispatch*>(worker->mTable.get());
                u32 wait=table->mWait;
                gabi::call_ptr<void>(wait,worker);
            }
            for(;;) {
                job=queuedWorkerJob(manager,index);
                u32 total=0;
                for(u32 part=0;part<3;++part) total+=static_cast<u32>(job->mCompleted[part<3?part:0]);
                JobQueue* target=queuedWorkerJob(manager,index);
                u32 progress=gabi::call_ptr<u32>(target->mTable->mTotal,target);
                if(total>=progress) break;
            }
        }
        manager->mReservedMode=0;
    }
    // The original waits for the worker-owned tokens to become clear; no artificial loop cap.
    while(!allWorkersIdleNow(manager)) {}
    if(allWorkersIdle(manager)==0) {
        gabi::FrameLocal<CounterSnapshot> snapshot(0x10);
        constructWorkerCounters(snapshot.get(),256,4,gabi::guest_fn((const void*)&constructWorkerCounter));
        s32 count=manager->mWorkerCount;
        if(count>0) {
            auto workers=manager->mWorkers.get();
            gabi::FrameLocal<CounterWord> token(0xC);
            for(u32 index=0;index<static_cast<u32>(count);++index) {
                WorkerRuntime* worker=workers[index<static_cast<u32>(count)?index:0];
                token->mValue=worker->mState;
                snapshot->mTokens[index].mValue=token->mValue;
                (void)static_cast<u32>(snapshot->mTokens[index].mValue);
            }
            // The remaining count-down has no memory or call effects.
            for(s32 delay=0;delay<count;++delay) {}
        }
    }
    manager->mQueueCount=0;
    manager->mSubmitted=0;
}
/* WWHD glue
 ext 028EFFD0 void constructWorkerCounters(void*,u32,u32,u32)
*/

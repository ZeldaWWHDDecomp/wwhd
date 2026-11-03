// HD worker-set construction; original runtime array callbacks retain their own entry points.
#include "JSystem/J3DGraph/PipelineDelegates.h"
#include "bindings.h"
using namespace j3d::pipeline;
void* allocatePipelineStorage(u32);
void constructPipelineArray(void*,u32,u32,u32);
void executePipelinePending(FrameWorkerSet*);
void executePipelineJob(PipelineFrameQueue*,void*,sead::JobItem*);
void executePipelineCompletion(PipelineFrameQueue*);
static sead::SafeTextVtable sPipelineTaskTextMethods;
static u8 sPipelineWorkName;
static sead::JobItemTable sPipelineWorkTaskMethods;
static PipelineDelegateMethods_l sPipelineWorkDelegateMethods;
static PipelineDelegateMethods_l sPipelineJobDelegateMethods;
static PipelineDelegateMethods_l sPipelineCompletionDelegateMethods;
static sead::JobItemTable sPipelineCompletionTaskMethods;
static u8 sPipelineCompletionName;
namespace j3d::pipeline {
/* 027F90B0 */
FrameWorkerSet* FrameWorkerSet::create(FrameWorkerSet* workers) {
    gabi::AutoFrame frame{0x027F90B0u};
    if(workers==nullptr) workers=static_cast<FrameWorkerSet*>(allocatePipelineStorage(sizeof(FrameWorkerSet)));
    if(workers==nullptr) return workers;
    constructPipelineArray(workers,3,sizeof(JobPointerArray_l),gabi::guest_fn(reinterpret_cast<const void*>(&JobPointerArray_l::create)));
    // Retain the original modular embedded-address guard.
    QueueTask_l* task=gabi::at<QueueTask_l>(gabi::ea(workers)+offsetof(FrameWorkerSet,mTask));
    if(task==nullptr) task=static_cast<QueueTask_l*>(allocatePipelineStorage(sizeof(QueueTask_l)));
    if(task!=nullptr) {
        sead::SafeText* name=&task->mName;
        if(name==nullptr) name=static_cast<sead::SafeText*>(allocatePipelineStorage(sizeof(sead::SafeText)));
        if(name!=nullptr) {
            name->mVtable=&sPipelineTaskTextMethods;
            name->mText=reinterpret_cast<be<u8>*>(&sPipelineWorkName);
        }
        task->mFlag=0;
        task->mMethods=&sPipelineWorkTaskMethods;
        sead::instances::DelegateLayout* delegate=gabi::at<sead::instances::DelegateLayout>(gabi::ea(task)+offsetof(QueueTask_l,mDelegate));
        if(delegate!=nullptr) {
            delegate->mObject=workers;
            delegate->mCallback.mThisAdjustment=0;
            delegate->mCallback.mVirtualIndex=-1;
            delegate->mVtable=gabi::ea(&sPipelineWorkDelegateMethods);
            delegate->mCallback.mFunction=gabi::guest_fn(reinterpret_cast<const void*>(&executePipelinePending));
        }
    }
    constructPipelineArray(&workers->mQueues,3,sizeof(sead::JobQueue),gabi::guest_fn(reinterpret_cast<const void*>(&sead::JobQueue::construct)));
    sead::WorkerManager::construct(&workers->mWorkers);
    return workers;
}
/* 027F9560 */
PipelineFrameQueue* PipelineFrameQueue::create(PipelineFrameQueue* queue) {
    gabi::AutoFrame frame{0x027F9560u};
    if(queue==nullptr) queue=static_cast<PipelineFrameQueue*>(allocatePipelineStorage(sizeof(PipelineFrameQueue)));
    if(queue==nullptr) return queue;
    ModelPointerArray_l* models=gabi::at<ModelPointerArray_l>(gabi::ea(queue)+offsetof(PipelineFrameQueue,mModels));
    if(models==nullptr) models=static_cast<ModelPointerArray_l*>(allocatePipelineStorage(sizeof(ModelPointerArray_l)));
    if(models!=nullptr) {
        models->mCapacity=0;
        models->mCount=0;
        models->mItems=nullptr;
    }
    constructPipelineArray(&queue->mLocks,3,sizeof(sead::CriticalSection),gabi::guest_fn(reinterpret_cast<const void*>(&sead::CriticalSection::construct)));
    queue->mFlag=0;
    FrameWorkerSet::create(&queue->mWork);
    sead::instances::DelegateLayout* dispatch=gabi::at<sead::instances::DelegateLayout>(gabi::ea(queue)+offsetof(PipelineFrameQueue,mDelegate));
    if(dispatch==nullptr) dispatch=static_cast<sead::instances::DelegateLayout*>(allocatePipelineStorage(sizeof(sead::instances::DelegateLayout)));
    if(dispatch!=nullptr) {
        dispatch->mObject=queue;
        dispatch->mCallback.mThisAdjustment=0;
        dispatch->mVtable=gabi::ea(&sPipelineJobDelegateMethods);
        dispatch->mCallback.mFunction=gabi::guest_fn(reinterpret_cast<const void*>(&executePipelineJob));
        dispatch->mCallback.mVirtualIndex=-1;
    }
    sead::JobQueue::construct(&queue->mJobs);
    constructPipelineArray(&queue->mOutputs,2,sizeof(JobPointerArray_l),gabi::guest_fn(reinterpret_cast<const void*>(&JobPointerArray_l::create)));
    QueueTask_l* task=gabi::at<QueueTask_l>(gabi::ea(queue)+offsetof(PipelineFrameQueue,mCompletion));
    if(task==nullptr) task=static_cast<QueueTask_l*>(allocatePipelineStorage(sizeof(QueueTask_l)));
    if(task!=nullptr) {
        sead::SafeText* name=&task->mName;
        if(name==nullptr) name=static_cast<sead::SafeText*>(allocatePipelineStorage(sizeof(sead::SafeText)));
        if(name!=nullptr) {
            name->mVtable=&sPipelineTaskTextMethods;
            name->mText=reinterpret_cast<be<u8>*>(&sPipelineCompletionName);
        }
        task->mFlag=0;
        task->mMethods=&sPipelineCompletionTaskMethods;
        sead::instances::DelegateLayout* delegate=gabi::at<sead::instances::DelegateLayout>(gabi::ea(task)+offsetof(QueueTask_l,mDelegate));
        if(delegate!=nullptr) {
            delegate->mObject=queue;
            delegate->mCallback.mThisAdjustment=0;
            delegate->mCallback.mVirtualIndex=-1;
            delegate->mVtable=gabi::ea(&sPipelineCompletionDelegateMethods);
            delegate->mCallback.mFunction=gabi::guest_fn(reinterpret_cast<const void*>(&executePipelineCompletion));
        }
    }
    queue->mCompleted=0;
    return queue;
}
}
/* WWHD glue
data sPipelineTaskTextMethods 1016EB2C
data sPipelineWorkName 1016EB10
data sPipelineWorkTaskMethods 1016EB54
data sPipelineWorkDelegateMethods 1016EB84
data sPipelineJobDelegateMethods 1016EB64
data sPipelineCompletionDelegateMethods 1016EB9C
data sPipelineCompletionTaskMethods 1016EB74
data sPipelineCompletionName 1016EB11
ext 028EFFD0 void constructPipelineArray(void*,u32,u32,u32)
ext 027F91C8 void executePipelinePending(j3d::pipeline::FrameWorkerSet*) callback
ext 027F9BC0 void executePipelineJob(j3d::pipeline::PipelineFrameQueue*,void*,sead::JobItem*) callback
ext 027F9E10 void executePipelineCompletion(j3d::pipeline::PipelineFrameQueue*) callback
ext 02760084 sead::CriticalSection* sead::CriticalSection::construct(sead::CriticalSection*) callback
ext 02758724 sead::JobQueue* sead::JobQueue::construct(sead::JobQueue*) callback
ext 02758B88 sead::WorkerManager* sead::WorkerManager::construct(sead::WorkerManager*)
*/

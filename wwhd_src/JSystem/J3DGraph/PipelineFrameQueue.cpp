// HD pipeline model-work submission and completion; callback invocation contracts stay separate.
#include "JSystem/J3DGraph/PipelineFrameQueue.h"
#include "sead/ElementAt.h"
#include "bindings.h"
using namespace j3d::pipeline;
static PipelineCoreMap_l sPipelineCoreMap;
u32 pipelineCoreId();
void* allocatePipelineStorage(u32);
void* movePipelineJobs(void*,const void*,u32,u32);
// ABI view of the exact JobItem prefix proved by this TU's task constructors.
static sead::JobItem* jobView(QueueTask_l* task) {
    static_assert(offsetof(QueueTask_l,mMethods)==offsetof(sead::JobItem,mTable));
    static_assert(offsetof(QueueTask_l,mDelegate)==sizeof(sead::JobItem));
    return reinterpret_cast<sead::JobItem*>(task);
}
namespace j3d::pipeline {
/* 027F91C8 */
void FrameWorkerSet::executePending() {
    gabi::AutoFrame frame{0x027F91C8u};
    for(u32 core=0;core!=3;++core) {
        JobPointerArray_l* input=&mInputs[core];
        while(input->mCount>0) {
            s32 count=input->mCount;
            gptr<sead::JobItem>* items=input->mItems;
            sead::JobItem* item=items[0];
            u32 bytes=(static_cast<u32>(count)-1u)*sizeof(gptr<sead::JobItem>);
            movePipelineJobs(items,items+1,bytes,0);
            input->mCount=static_cast<s32>(input->mCount)-1;
            if(item==nullptr) break;
            sead::JobItemTable* methods=item->mTable;
            gabi::call_ptr<void>(methods->mExecute,item);
        }
    }
}
/* 027F92C0 */
void FrameWorkerSet::clearQueues() {
    gabi::AutoFrame frame{0x027F92C0u};
    for(u32 index=0;index!=3;++index) mQueues[index].clear();
}
/* 027F9470 */
void FrameWorkerSet::append(sead::JobItem* item) {
    gabi::AutoFrame frame{0x027F9470u};
    u32 core=pipelineCoreId();
    u32 slot=sPipelineCoreMap.mSlots[core];
    if(slot>2) return;
    JobPointerArray_l* input=&mInputs[slot];
    s32 count=input->mCount;
    s32 capacity=input->mCapacity;
    if(count<capacity) {
        gptr<sead::JobItem>* items=input->mItems;
        items[count]=item;
        count=input->mCount;
        input->mCount=count+1;
    }
}
/* 027F9994 */
void PipelineFrameQueue::finishFrame() {
    gabi::AutoFrame frame{0x027F9994u};
    sead::DelegateThread* dispatcher=mDispatcher;
    auto* methods=reinterpret_cast<PipelineQueueMethods_l*>(dispatcher->methods());
    gabi::call_ptr<u32>(methods->mSendMessage.get(),dispatcher,gabi::ea(jobView(&mCompletion)),0u);
    mLocks[0].lock();
    while(static_cast<u8>(mCompleted)==0) {
        mLocks[0].unlock();
        mLocks[0].lock();
    }
    mLocks[0].unlock();
}
/* 027F9A10 */
void PipelineFrameQueue::flushModels() {
    gabi::AutoFrame frame{0x027F9A10u};
    u32 count=mModels.mCount;
    gptr<ModelPipeline_l>* models=mModels.mItems;
    gptr<ModelPipeline_l>* end=sead::elementPtr(models,count); // 32-bit end and steps, as the original's `cmplw`
    while(models!=end) {
        ModelPipeline_l* model=*models;
        model->updatePacketBindings();
        models=sead::nextElement(models);
    }
    finishFrame();
}
/* 027F9A7C */
void PipelineFrameQueue::resetWork() {
    gabi::AutoFrame frame{0x027F9A7Cu};
    mModels.mCount=0;
    mWork.clearQueues();
    mCompleted=0;
}
/* 027F9AC0 */
u32 PipelineFrameQueue::submitWork() {
    gabi::AutoFrame frame{0x027F9AC0u};
    sead::DelegateThread* dispatcher=mDispatcher;
    auto* methods=reinterpret_cast<PipelineQueueMethods_l*>(dispatcher->methods());
    return gabi::tail_ptr<u32>(methods->mSendMessage.get(),dispatcher,gabi::ea(jobView(&mWork.mTask)),0u);
}
/* 027F9AE0 */
void PipelineFrameQueue::enqueue(sead::JobItem* job) {
    gabi::AutoFrame frame{0x027F9AE0u};
    if(static_cast<u8>(mFlag)!=0) {
        mWork.append(job);
        return;
    }
    sead::DelegateThread* dispatcher=mDispatcher;
    auto* methods=reinterpret_cast<PipelineQueueMethods_l*>(dispatcher->methods());
    u32 queued=gabi::call_ptr<u32>(methods->mSendMessage.get(),dispatcher,gabi::ea(job),1u);
    if(queued==0) {
        sead::JobItemTable* jobMethods=job->mTable;
        gabi::call_ptr<void>(jobMethods->mExecute,job);
    }
}
/* 027F9B64 */
void PipelineFrameQueue::submitOrExecute(sead::JobItem* job) {
    gabi::AutoFrame frame{0x027F9B64u};
    sead::DelegateThread* dispatcher=mDispatcher;
    auto* methods=reinterpret_cast<PipelineQueueMethods_l*>(dispatcher->methods());
    u32 queued=gabi::call_ptr<u32>(methods->mSendMessage.get(),dispatcher,gabi::ea(job),1u);
    if(queued==0) {
        sead::JobItemTable* jobMethods=job->mTable;
        gabi::call_ptr<void>(jobMethods->mExecute,job);
    }
}
/* 027F9CAC */
JobPointerArray_l* JobPointerArray_l::create(JobPointerArray_l* array) {
    gabi::AutoFrame frame{0x027F9CACu};
    if(array==nullptr) array=static_cast<JobPointerArray_l*>(allocatePipelineStorage(sizeof(JobPointerArray_l)));
    if(array!=nullptr) {
        array->mCount=0;
        array->mItems=nullptr;
        array->mCapacity=0;
    }
    return array;
}
/* 027F9E10 */
void PipelineFrameQueue::notifyCompleted() {
    gabi::AutoFrame frame{0x027F9E10u};
    mLocks[0].lock();
    mCompleted=1;
    mLocks[0].unlock();
}
}
/* WWHD glue
data sPipelineCoreMap 104A0BC0
ext C0009BB0 u32 pipelineCoreId()
ext C0009988 void* movePipelineJobs(void*,const void*,u32,u32)
ext 02758B00 void sead::JobQueue::clear()
ext 027601BC void sead::CriticalSection::lock()
ext 027601F0 void sead::CriticalSection::unlock()
*/

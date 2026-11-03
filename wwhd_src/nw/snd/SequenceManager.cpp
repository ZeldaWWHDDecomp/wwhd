/* Sequence asynchronous jobs and task pool lifecycle, reconstructed from WWHD. */
#include "nw/snd/SequenceManager.h"
using namespace nw::snd;
void* allocateSequenceManager(u32 size);
void releaseSequenceManager(void* storage);
void constructSequenceJob(SequenceAsyncJob_l* job);
void destroySequenceJob(SequenceAsyncJob_l* job,u32 flags);
void waitSequenceEvent(nw::ut::Event* event);
void initializeSequenceTaskPool(be<u32>* pool,void* storage,u32 size,u32 stride);
void finalizeSequenceTaskPool(be<u32>* pool);
void releaseSequenceTask(be<u32>* pool,SequenceAsyncTask_l* task);
void removeSequenceTaskNode(SequenceList* list,SequenceListNode* node);
void* fillSequenceManager(void* storage,u32 value,u32 size);
struct SequenceManagerAllocator_l { gptr<const be<u32>> vtable; };
/* 028AE2E8 */
SequenceManager* SequenceManager::construct(SequenceManager* manager) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (manager==nullptr) manager=static_cast<SequenceManager*>(allocateSequenceManager(sizeof(SequenceManager)));
    if (manager==nullptr) return manager;
    manager->mState=0;
    manager->mFreeSounds.count=0;
    manager->mPoolState[3]=0;
    manager->mPoolState[1]=0;
    manager->mPoolState[0]=0;
    manager->mPoolState[2]=0;
    constructSequenceJob(&manager->mJobs[0]);
    manager->mJobs[0].vtable=gabi::at<const be<u32>>(0x10188000);
    constructSequenceJob(&manager->mJobs[1]);
    manager->mJobs[1].vtable=gabi::at<const be<u32>>(0x10188030);
    SequenceList* tasks=gabi::at<SequenceList>(gabi::ea(manager)+0x390);
    if (tasks==nullptr) tasks=static_cast<SequenceList*>(allocateSequenceManager(sizeof(SequenceList)));
    if (tasks!=nullptr) {
        tasks->count=0;
        tasks->sentinel.previous=&tasks->sentinel;
        tasks->sentinel.next=&tasks->sentinel;
    }
    manager->mTaskPool[0]=0;
    manager->mTaskPool[1]=0;
    manager->mTaskPool[2]=0;
    manager->mAllocation=nullptr;
    initializeSequenceTaskPool(manager->mTaskPool,manager->mTaskStorage,0x1180,0x8C);
    fillSequenceManager(manager->mFlags,0,0x27F);
    return manager;
}
/* 028AE3D0 */
void SequenceManager::waitForTasks() {
    gabi::AutoFrame frame{gabi::current_function()};
    waitSequenceEvent(&mJobs[0].event);
    waitSequenceEvent(&mJobs[1].event);
    SequenceListNode* node=mTasks.sentinel.next;
    while (node!=&mTasks.sentinel) {
        SequenceListNode* current=node;
        node=node->next;
        SequenceAsyncTask_l* task=gabi::at<SequenceAsyncTask_l>(gabi::ea(current)-0x84);
        waitSequenceEvent(gabi::at<nw::ut::Event>(gabi::ea(current)-0x7C));
        removeSequenceTaskNode(&mTasks,current);
        if (task!=nullptr) {
            const be<u32>* table=task->job.vtable;
            gabi::call_ptr<void>(table[3],task,2u);
            releaseSequenceTask(mTaskPool,task);
        }
    }
}
/* 028AE46C */
void SequenceManager::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this==nullptr) return;
    waitForTasks();
    SequenceManagerAllocator_l* allocator=*gabi::at<gptr<SequenceManagerAllocator_l>>(0x101FCB70);
    if (allocator!=nullptr) {
        void* allocation=mAllocation;
        if (allocation!=nullptr) {
            const be<u32>* table=allocator->vtable;
            gabi::call_ptr<void>(table[7],allocator,allocation);
            mAllocation=nullptr;
        }
    }
    finalizeSequenceTaskPool(mTaskPool);
    destroySequenceJob(&mJobs[1],0);
    destroySequenceJob(&mJobs[0],0);
    if (flags&1) releaseSequenceManager(this);
}
/* WWHD glue
ext 0273AD10 void* allocateSequenceManager(u32)
ext 0273AF40 void releaseSequenceManager(void*)
ext 0289C050 void constructSequenceJob(nw::snd::SequenceAsyncJob_l*)
ext 0289C0C8 void destroySequenceJob(nw::snd::SequenceAsyncJob_l*,u32)
ext 028F6A2C void waitSequenceEvent(nw::ut::Event*)
ext 0288A184 void initializeSequenceTaskPool(be<u32>*,void*,u32,u32)
ext 0288A1D0 void finalizeSequenceTaskPool(be<u32>*)
ext 0288A26C void releaseSequenceTask(be<u32>*,nw::snd::SequenceAsyncTask_l*)
ext 0286DA9C void removeSequenceTaskNode(nw::snd::SequenceList*,nw::snd::SequenceListNode*)
ext C000A858 void* fillSequenceManager(void*,u32,u32)
*/

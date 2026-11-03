// HD pipeline queue lifetime and registration cleanup; thread hook remains an opaque call.
#include "JSystem/J3DGraph/PipelineQueueLifetime.h"
#include "bindings.h"
using namespace j3d::pipeline;
static PipelineFrameQueue* sPipelineFrameQueue;
static sead::Disposer* sPipelineFrameQueueDisposer;
static sead::DisposerVtable sPipelineFrameQueueDisposerMethods;
static sead::DisposerVtable sPipelineWorkerRegistrationMethods;
void* allocatePipelineRenderStorage(u32,sead::RegistryHeap*,s32);
void initializePipelineDisposer(sead::Disposer*,sead::RegistryHeap*,s32);
void finalizePipelineDisposer(sead::Disposer*,u32);
void releasePipelineStorage(void*);
void destroyPipelineArray(void*,u32,u32,u32,u32,u32);
void destroyPipelineCriticalSection(sead::CriticalSection*,u32);
namespace {
void unregisterPipelineWorker(sead::EmbeddedObject* object) {
    u8 registered=object->mRegistered;
    object->mHeader.mTable=gabi::ea(&sPipelineWorkerRegistrationMethods);
    if(registered!=0) {
        sead::LinkList* owner=object->mOwner;
        if(owner!=nullptr) {
            object->mOwner=nullptr;
            object->mRegistration.detach();
            owner->mCount=static_cast<s32>(owner->mCount)-1;
        }
        object->mRegistered=0;
    }
    static_assert(offsetof(sead::EmbeddedObject,mHeader)==0);
    static_assert(sizeof(sead::ObjectHeader)==sizeof(sead::RegisteredObject));
    reinterpret_cast<sead::RegisteredObject*>(object)->destroy(0);
}
}
namespace j3d::pipeline {
/* 027F96F0 */
PipelineFrameQueue* PipelineFrameQueue::instance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F96F0u};
    PipelineFrameQueue* queue=sPipelineFrameQueue;
    if(queue!=nullptr) return queue;
    queue=static_cast<PipelineFrameQueue*>(allocatePipelineRenderStorage(sizeof(PipelineFrameQueue),heap,4));
    if(queue!=nullptr) {
        initializePipelineDisposer(&queue->mDisposer,heap,3);
        queue->mDisposer.mVtable=&sPipelineFrameQueueDisposerMethods;
    }
    sPipelineFrameQueueDisposer=queue==nullptr?nullptr:&queue->mDisposer;
    if(queue!=nullptr) queue=PipelineFrameQueue::create(queue);
    sPipelineFrameQueue=queue;
    return queue;
}
/* 027F9784 */
void PipelineFrameQueue::destroy(PipelineFrameQueue* queue,u32 flags) {
    gabi::AutoFrame frame{0x027F9784u};
    if(queue==nullptr) return;
    sead::DelegateThread* dispatcher=queue->mDispatcher;
    auto* methods=reinterpret_cast<PipelineQueueMethods_l*>(dispatcher->methods());
    gabi::call_ptr<void>(methods->mHook.get(),dispatcher,0u);
    sead::EmbeddedObject* registration=&queue->mWork.mWorkers.mObject;
    if(registration!=nullptr) unregisterPipelineWorker(registration);
    destroyPipelineArray(&queue->mWork.mQueues,3,sizeof(sead::JobQueue),gabi::guest_fn(reinterpret_cast<const void*>(&PipelineQueueLifetime::releaseQueueStorage)),0,0);
    destroyPipelineArray(&queue->mLocks,3,sizeof(sead::CriticalSection),gabi::guest_fn(reinterpret_cast<const void*>(&destroyPipelineCriticalSection)),0,0);
    if((flags&1u)!=0) releasePipelineStorage(queue);
}
/* 027F9BD4 */
void PipelineQueueLifetime::destroyDisposer(sead::Disposer* disposer,u32 flags) {
    gabi::AutoFrame frame{0x027F9BD4u};
    if(disposer==nullptr) return;
    disposer->mVtable=&sPipelineFrameQueueDisposerMethods;
    if(disposer==sPipelineFrameQueueDisposer.get()) {
        PipelineFrameQueue* queue=sPipelineFrameQueue;
        sPipelineFrameQueueDisposer=nullptr;
        PipelineFrameQueue::destroy(queue,2);
        sPipelineFrameQueue=nullptr;
    }
    finalizePipelineDisposer(disposer,0);
    if((flags&1u)!=0) releasePipelineStorage(disposer);
}
/* 027F9C98 */
void PipelineQueueLifetime::releaseOpaqueStorage(void* object,u32 flags) {
    gabi::AutoFrame frame{0x027F9C98u};
    if(object!=nullptr && (flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F9D54 */
void PipelineQueueLifetime::destroyRegistration(sead::EmbeddedObject* object,u32 flags) {
    gabi::AutoFrame frame{0x027F9D54u};
    if(object==nullptr) return;
    unregisterPipelineWorker(object);
    if((flags&1u)!=0) releasePipelineStorage(object);
}
/* 027F9DFC */
void PipelineQueueLifetime::releaseQueueStorage(sead::JobQueue* queue,u32 flags) {
    gabi::AutoFrame frame{0x027F9DFCu};
    if(queue!=nullptr && (flags&1u)!=0) releasePipelineStorage(queue);
}
}
/* WWHD glue
data sPipelineFrameQueue 101F997C
data sPipelineFrameQueueDisposer 101F9980
data sPipelineFrameQueueDisposerMethods 1016EBD4
data sPipelineWorkerRegistrationMethods 1016EB44
ext 028F0164 void destroyPipelineArray(void*,u32,u32,u32,u32,u32)
ext 02760168 void destroyPipelineCriticalSection(sead::CriticalSection*,u32) callback
ext 0273B310 sead::Link* sead::Link::detach()
ext 02752BEC void sead::RegisteredObject::destroy(u32)
*/

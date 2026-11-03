// HD queue setup and task submission; local options view retains a pending shared field-role correction.
#include "JSystem/J3DGraph/PipelineQueueInitialization.h"
#include "bindings.h"
using namespace j3d::pipeline;
void constructPipelineWorkerSettings(PipelineWorkerSettings_l*);
void initializePipelineWorkers(sead::WorkerManager*,PipelineWorkerSettings_l*);
void submitPipelineQueues(sead::WorkerManager*,const char*,sead::JobQueue*,const be<u32>*,const be<u32>*,const be<u32>*);
void* allocatePipelineThread(u32,void*,s32);
void drainPipelineWorkers(sead::WorkerManager*);
struct PipelinePriority_l { be<s32> mValue; };
static PipelinePriority_l sPipelineDefaultPriority;
// Borrowed per-TU table view; no initializer or independent storage owner.
static sead::SafeTextVtable sPipelineInitTextMethods;
static sead::PointerBuffer* pointerBuffer(JobPointerArray_l* array) {
    static_assert(sizeof(JobPointerArray_l)==sizeof(sead::PointerBuffer));
    static_assert(offsetof(JobPointerArray_l,mCount)==offsetof(sead::PointerBuffer,mCount));
    static_assert(offsetof(JobPointerArray_l,mCapacity)==offsetof(sead::PointerBuffer,mCapacity));
    static_assert(offsetof(JobPointerArray_l,mItems)==offsetof(sead::PointerBuffer,mStorage));
    return reinterpret_cast<sead::PointerBuffer*>(array);
}
namespace j3d::pipeline {
/* 027F9308 */
void FrameWorkerSet::runQueues() {
    gabi::AutoFrame frame{0x027F9308u};
    gabi::FrameLocal<be<u32>> waitMode(8),flags(0xC),mask(0x10),firstCore(0x14),secondCore(0x18),thirdCore(0x1C);
    for(u32 index=0;index!=2;++index) {
        *firstCore=0;
        u32 first=*firstCore;
        *secondCore=1;
        u32 second=*secondCore;
        *flags=0;
        *thirdCore=2;
        u32 third=*thirdCore;
        *waitMode=0;
        *mask=(1u<<first)|(1u<<second)|(1u<<third);
        submitPipelineQueues(&mWorkers,"update_particle",&mQueues[index],mask.get(),waitMode.get(),flags.get());
    }
    mWorkers.start();
    drainPipelineWorkers(&mWorkers);
}
/* 027F93C4 */
void FrameWorkerSet::initialize(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F93C4u};
    gabi::FrameLocal<PipelineWorkerSettings_l> settings(8);
    for(u32 index=0;index!=3;++index) pointerBuffer(&mInputs[index])->allocate(4000,heap,4);
    for(u32 index=0;index!=3;++index) mQueues[index].initialize(5,heap);
    constructPipelineWorkerSettings(settings.get());
    settings->mWorkerArgument=3;
    initializePipelineWorkers(&mWorkers,settings.get());
}
/* 027F9890 */
u32 PipelineFrameQueue::initialize(s32 modelCapacity,sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{0x027F9890u};
    gabi::FrameLocal<sead::SafeText> name(0x18);
    gabi::FrameLocal<be<u32>> coreMask(0xC),firstCore(0x10),secondCore(0x14);
    static_assert(sizeof(ModelPointerArray_l)==sizeof(sead::PointerBuffer));
    static_assert(offsetof(ModelPointerArray_l,mCount)==offsetof(sead::PointerBuffer,mCount));
    static_assert(offsetof(ModelPointerArray_l,mCapacity)==offsetof(sead::PointerBuffer,mCapacity));
    static_assert(offsetof(ModelPointerArray_l,mItems)==offsetof(sead::PointerBuffer,mStorage));
    reinterpret_cast<sead::PointerBuffer*>(&mModels)->allocate(modelCapacity,heap,4);
    mWork.initialize(heap);
    auto* thread=static_cast<sead::DelegateThread*>(allocatePipelineThread(sizeof(sead::DelegateThread),heap,4));
    if(thread!=nullptr) {
        name->mText.v=gabi::guest_str("update_ubo");
        name->mVtable=&sPipelineInitTextMethods;
        s32 priority=sPipelineDefaultPriority.mValue;
        // Canonical ThreadDelegate is the exact four-byte interface prefix of this delegate.
        static_assert(offsetof(sead::instances::DelegateLayout,mVtable)==offsetof(sead::ThreadDelegate,mVtable));
        auto* delegate=reinterpret_cast<sead::ThreadDelegate*>(&mDelegate);
        thread=sead::DelegateThread::construct(thread,name.get(),delegate,heap,priority+2,0,0xFFFFFFFFu,0x9000,32);
        mDispatcher=thread;
    } else mDispatcher=nullptr;
    *firstCore=1;
    *coreMask=6;
    *secondCore=2;
    thread->setCoreMask(coreMask.get());
    sead::DelegateThread* dispatcher=mDispatcher;
    auto* methods=reinterpret_cast<PipelineQueueMethods_l*>(dispatcher->methods());
    return gabi::call_ptr<u32>(methods->mStart.get(),dispatcher);
}
}
/* WWHD glue
data sPipelineDefaultPriority 101F8B9C
data sPipelineInitTextMethods 1016EB2C
str 1016EBB4 "update_particle"
str 1016EBC4 "update_ubo"
ext 0273B584 void sead::PointerBuffer::allocate(s32,void*,s32)
ext 027587B4 void sead::JobQueue::initialize(s32,sead::RegistryHeap*)
ext 02759814 void constructPipelineWorkerSettings(j3d::pipeline::PipelineWorkerSettings_l*)
ext 02758CA8 void initializePipelineWorkers(sead::WorkerManager*,j3d::pipeline::PipelineWorkerSettings_l*)
ext 027591F4 void submitPipelineQueues(sead::WorkerManager*,const char*,sead::JobQueue*,const be<u32>*,const be<u32>*,const be<u32>*)
ext 02759324 void sead::WorkerManager::start()
ext 02759564 void drainPipelineWorkers(sead::WorkerManager*)
ext 0273B050 void* allocatePipelineThread(u32,void*,s32)
ext 0275FBD8 sead::DelegateThread* sead::DelegateThread::construct(sead::DelegateThread*,const sead::SafeText*,sead::ThreadDelegate*,sead::RegistryHeap*,s32,u32,u32,u32,s32)
ext 02760C4C void sead::Thread::setCoreMask(const be<u32>*)
*/

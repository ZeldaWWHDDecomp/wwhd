// HD pipeline delegate dispatch, with callback arity established per owning task family.
#include "JSystem/J3DGraph/PipelineDelegates.h"
#include "bindings.h"
using namespace j3d::pipeline;
namespace {
// Concrete family methods choose the argument list; no extra registers are inferred.
template <class... Args>
void tailDelegate(const sead::instances::DelegateLayout* delegate,Args... args) {
    void* object=delegate->mObject;
    if(object==nullptr) return;
    s16 index=delegate->mCallback.mVirtualIndex;
    if(index==0) return;
    s16 adjustment=delegate->mCallback.mThisAdjustment;
    void* adjusted=gabi::at<void>(gabi::ea(object)+static_cast<s32>(adjustment));
    u32 target;
    if(index<0) target=delegate->mCallback.mFunction;
    else {
        s16 tableOffset=delegate->mCallback.mVtableOffset;
        auto* tableWord=gabi::at<gptr<sead::instances::MethodEntry>>(gabi::ea(adjusted)+static_cast<s32>(tableOffset));
        sead::instances::MethodEntry* table=*tableWord;
        target=table[index].mFunction;
    }
    gabi::tail_ptr<void>(target,adjusted,args...);
}
void tailTaskDelegate(QueueTask_l* task) {
    sead::instances::DelegateLayout* delegate=&task->mDelegate;
    auto* methods=gabi::at<PipelineDelegateMethods_l>(delegate->mVtable);
    gabi::tail_ptr<void>(methods->mInvoke.get(),delegate);
}
}
namespace j3d::pipeline {
/* 027F7680 */
void QueueTask_l::invokeProviderCallback() {
    gabi::AutoFrame frame{0x027F7680u};
    tailTaskDelegate(this);
}
/* 027F7690 */
void PipelineDelegate_l::invokeProvider() {
    gabi::AutoFrame frame{0x027F7690u};
    tailDelegate(this);
}
/* 027F9CF0 */
void QueueTask_l::invokeWorkCallback() {
    gabi::AutoFrame frame{0x027F9CF0u};
    tailTaskDelegate(this);
}
/* 027F9D00 */
void PipelineDelegate_l::invokeWork() {
    gabi::AutoFrame frame{0x027F9D00u};
    tailDelegate(this);
}
/* 027F9EA4 */
void QueueTask_l::invokeCompletionCallback() {
    gabi::AutoFrame frame{0x027F9EA4u};
    tailTaskDelegate(this);
}
/* 027F9EB4 */
void PipelineDelegate_l::invokeCompletion() {
    gabi::AutoFrame frame{0x027F9EB4u};
    tailDelegate(this);
}
/* 027F9E50 */
void PipelineDelegate_l::invokeJob(void* context,sead::JobItem* job) {
    gabi::AutoFrame frame{0x027F9E50u};
    tailDelegate(this,context,job);
}
/* 027F9BC0 */
void PipelineFrameQueue::executeJob(void* context,sead::JobItem* job) {
    gabi::AutoFrame frame{0x027F9BC0u};
    sead::JobItemTable* methods=job->mTable;
    gabi::tail_ptr<void>(methods->mExecute,job,context);
}
}
/* WWHD glue
*/

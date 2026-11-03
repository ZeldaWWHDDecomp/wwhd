// Task transition helpers reconstructed from local WWHD instructions.
#include "sead/TaskTransitionCompanions.h"
using namespace sead;
struct TransitionTypeTable_l { be<u32> mWords[4]; };
static TransitionTypeTable_l sTransitionTypeTable_l;
static u32 sTransitionTypeGuard_l;
static RuntimeType sTransitionType_l;
static u32 sCreateTaskFromArgument_l;
static u32 sCreateTaskFromResource_l;
static TaskBaseVtable sNullFaderDrawTable_l;
static DisposerVtable sNullFaderTaskTable_l;
void* allocateTaskTransitionStorage(u32 size);
void releaseTaskTransitionStorage(void* storage);
void constructNullFaderBase(TaskTransition* object,const TaskConstructArg* context,const char* name);
void detachTransitionRender(MethodTreeNode* object);
void destroyTransitionDisposer(Disposer* object,u32 flags);
void destroyTransitionTaskBase(TaskTransition* object,u32 flags);
u32 prepareFadeTransition(TaskTransition* object);
void setFadeTransitionState(TaskTransition* object,u32 state);
void detachFadeTask(TaskTransition* object);
void lockTaskTransition(void* mutex);
void unlockTaskTransition(void* mutex);

static void destroyTransitionRegistration(CreateListener* object,u32 flags) {
    if(!object) return;
    Disposer* disposer=reinterpret_cast<Disposer*>(object);
    disposer->mVtable=reinterpret_cast<DisposerVtable*>(&sTransitionRegistrationTable);
    if(object->mListening) {
        LinkList* owner=object->mList;
        if(owner) {
            object->mList=nullptr;
            object->mLink.detach();
            owner->mCount=static_cast<s32>(owner->mCount)-1;
        }
        object->mListening=0;
    }
    destroyTransitionDisposer(gabi::at<Disposer>(gabi::ea(object)),0);
    if(flags&1) releaseTaskTransitionStorage(object);
}
static void destroyFadeTask(TaskTransition* object,u32 flags) {
    if(!object) return;
    object->mTask.mDisposer.mVtable=reinterpret_cast<DisposerVtable*>(&sTransitionTaskDisposerTable);
    object->mTransitionNode.mDisposer.mVtable=reinterpret_cast<DisposerVtable*>(&sTransitionNodeDisposerTable);
    object->mTransitionNode.mVtable=reinterpret_cast<MethodTreeNodeVtable*>(&sTransitionNodeTable);
    detachTransitionRender(&object->mTransitionNode);
    destroyTransitionDisposer(&object->mTransitionNode.mDisposer,0);
    object->mCalcEntry.mVtable=reinterpret_cast<MethodTreeNodeVtable*>(&sTransitionNodeTable);
    object->mCalcEntry.mDisposer.mVtable=reinterpret_cast<DisposerVtable*>(&sTransitionNodeDisposerTable);
    detachTransitionRender(&object->mCalcEntry);
    destroyTransitionDisposer(&object->mCalcEntry.mDisposer,0);
    destroyTransitionRegistration(gabi::at<CreateListener>(gabi::ea(object)+offsetof(TaskTransition,mRegistration)),0);
    destroyTransitionTaskBase(object,0);
    if(flags&1) releaseTaskTransitionStorage(object);
}

/* 02768400 */
u32 TaskTransition::request(const TaskCreateArg* request) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* mutex=&mTask.mTaskMgr->mCriticalSection;
    lockTaskTransition(mutex);
    if(!mState) {
        mOperation=0;
        be<u32>* destination=reinterpret_cast<be<u32>*>(&mParameters);
        const be<u32>* source=reinterpret_cast<const be<u32>*>(request);
        for(u32 i=0;i<32;++i) destination[i]=source[i];
        if(prepareFadeTransition(this)) {
            u32 context=mEnterFrames;
            mPreviousTask=0;
            mFrame=context;
            setFadeTransitionState(this,2);
            mReady=0;
            mBlend=1.0f;
            detachFadeTask(this);
            unlockTaskTransition(mutex);
            return 1;
        }
    }
    unlockTaskTransition(mutex);
    return 0;
}
/* 02768514 */
NullFaderTask* NullFaderTask::create(NullFaderTask* object,const TaskConstructArg* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(!object) object=static_cast<NullFaderTask*>(allocateTaskTransitionStorage(sizeof(NullFaderTask)));
    if(object) {
        constructNullFaderBase(object,context,"NullFaderTask");
        object->mTask.mVtable=&sNullFaderDrawTable_l;
        object->mTask.mDisposer.mVtable=&sNullFaderTaskTable_l;
        object->mEnterFrames=0;
        object->mExitEnd=0;
        object->mExitStart=0;
    }
    return object;
}
/* 0276859C */
void task_transition_text::Text::destroy(u32 flags) {
    if(this && (flags&1)) gabi::tail_ptr<void>(gabi::guest_fn((const void*)releaseTaskTransitionStorage),this);
}
/* 027685B0 */
void TransitionRenderObject::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if(this) {
        mVtable=reinterpret_cast<MethodTreeNodeVtable*>(&sTransitionNodeTable);
        mDisposer.mVtable=reinterpret_cast<DisposerVtable*>(&sTransitionNodeDisposerTable);
        detachTransitionRender(this);
        destroyTransitionDisposer(&mDisposer,0);
        if(flags&1) releaseTaskTransitionStorage(this);
    }
}
/* 02768624 */
void TransitionRegisteredObject::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    destroyTransitionRegistration(this,flags);
}
/* 027686CC */
void TaskTransition::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    destroyFadeTask(this,flags);
}
/* 027687C4 */
void NullFaderTask::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    destroyFadeTask(this,flags);
}

static u32 invokeTransitionDelegate(const MethodDelegate* delegate) {
    void* object=delegate->mObject;
    if(!object) return gabi::ea(delegate);
    s32 index=delegate->mMethod.mVirtualIndex;
    if(index==0) return gabi::ea(delegate);
    s32 adjustment=delegate->mMethod.mObjectAdjustment;
    void* receiver=gabi::at<void>(gabi::ea(object)+adjustment);
    u32 target;
    if(index<0) target=delegate->mMethod.mTarget;
    else {
        s32 offset=static_cast<s16>(static_cast<u32>(delegate->mMethod.mTarget));
        u32 table=*gabi::at<be<u32>>(gabi::ea(receiver)+offset);
        const VirtualEntry* methods=gabi::at<VirtualEntry>(table);
        target=methods[index].mTarget;
    }
    return gabi::tail_ptr<u32>(target,receiver);
}
/* 027688BC */
void TransitionHooks_l::idle() {}
/* 027688C0 */
u32 TransitionDelegate::invoke() { return invokeTransitionDelegate(this); }
/* 02768914 */
u32 TransitionHooks_l::matchesType(const RuntimeType* type) const {
    if(!sTransitionTypeGuard_l) {
        sTransitionTypeGuard_l=1;
        sTransitionType_l.mTable=&sTransitionTypeTable_l;
    }
    return type==&sTransitionType_l;
}
/* 02768950 */
void TransitionHooks_l::enter() {}
/* 02768954 */
void TransitionHooks_l::leave() {}
/* 02768958 */
void TransitionHooks_l::prepare() {}
/* 0276895C */
void TransitionHooks_l::finish() {}
/* 02768960 */
void TransitionHooks_l::update() {}
/* 02768964 */
u32 TransitionHooks_l::query() { return 0; }
/* 0276896C */
u32 TransitionEnterDelegate::invoke() { return invokeTransitionDelegate(this); }
/* 027689C0 */
u32 TransitionLeaveDelegate::invoke() { return invokeTransitionDelegate(this); }
/* 02768A14 */
u32 TaskCreationRequest::create(void* context) const {
    gabi::AutoFrame frame{gabi::current_function()};
    switch(static_cast<u32>(mType)) {
    case 1: {
        u32 target=sCreateTaskFromArgument_l;
        if(target) return gabi::call_ptr<u32>(target,static_cast<u32>(mValue));
        break;
    }
    case 2: return gabi::call_ptr<u32>(static_cast<u32>(mValue),context);
    case 3: {
        u32 target=sCreateTaskFromResource_l;
        if(target) return gabi::call_ptr<u32>(target,static_cast<u32>(mValue));
        break;
    }
    }
    return 0;
}
/* WWHD glue
 ext 0273AD10 void* allocateTaskTransitionStorage(u32)
 ext 0273AF40 void releaseTaskTransitionStorage(void*) callback
 ext 02767728 void constructNullFaderBase(sead::TaskTransition*,const sead::TaskConstructArg*,const char*)
 ext 02747A84 void detachTransitionRender(sead::MethodTreeNode*)
 ext 02752BEC void destroyTransitionDisposer(sead::Disposer*,u32)
 ext 027485F0 void destroyTransitionTaskBase(sead::TaskTransition*,u32)
 ext 02767C7C u32 prepareFadeTransition(sead::TaskTransition*)
 ext 02767D24 void setFadeTransitionState(sead::TaskTransition*,u32)
 ext 027486D8 void detachFadeTask(sead::TaskTransition*)
 ext 027601BC void lockTaskTransition(void*)
 ext 027601F0 void unlockTaskTransition(void*)
 str 1015F520 "NullFaderTask"
 data sNullFaderDrawTable_l 1015F608
 data sNullFaderTaskTable_l 1015F6D8
 data sTransitionTypeTable_l 1015F458
 data sTransitionTypeGuard_l 101FD594
 data sTransitionType_l 101FD980
 data sCreateTaskFromArgument_l 101F8BCC
 data sCreateTaskFromResource_l 101F8BD0
*/

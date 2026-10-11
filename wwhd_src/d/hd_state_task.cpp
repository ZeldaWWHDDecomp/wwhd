/* HD background worker task, driven by the 'updt' message. Private reconstruction. */
#include "d/hd_state_task.h"
using namespace gabi;
extern void* hdTaskAllocate(u32);
extern HdStateTask* hdTaskAllocateInstance(u32,void*,u32);
extern HdStateVTableObject* hdTaskAllocateWorker(u32,void*,u32);
extern void hdTaskDelete(void*);
extern void hdTaskStateConstruct(HdStateMachine*,HdStateTask*,void*);
extern void hdTaskChangeState(HdStateMachine*,HdStateId*);
extern void hdTaskStateExecute(HdStateMachine*);
extern void hdTaskQueueConstruct(HdStateQueue*);
extern void hdTaskThreadConstruct(HdStateThread*);
extern void hdTaskDisposerConstruct(HdStateDisposer*,void*,u32);
extern void hdTaskDisposerDestroy(HdStateDisposer*,u32);
extern void hdTaskThreadStep(HdStateThread*);
extern void hdTaskThreadJoin(HdStateThread*,HdStateVTableObject*);
extern void hdTaskThreadDestroy(HdStateThread*,u32);
extern void hdTaskQueueDestroy(HdStateQueue*,u32);
extern void hdTaskSharedData(u32,u32,be<u32>*,be<u32>*);
extern void hdTaskParamsConstruct(HdStateThreadParams*);
extern void hdTaskThreadStart(HdStateThread*,HdStateThreadParams*);
extern void hdTaskMessageConstruct(HdStateMessage*);
extern void hdTaskQueueSend(HdStateQueue*,HdStateMessage*);
extern HdStateVTableObject* hdTaskWorkerConstruct(HdStateVTableObject*,void*);
extern void hdTaskThreadRequest(HdStateThread*);
extern void hdTaskThreadWake(HdStateThread*);
extern void hdTaskRunObject(void*);
extern void hdTaskRegister(void*);
extern void hdTaskInitSigned(be<u8>*);
extern void hdTaskInitUnsigned(be<u8>*);
static HdStateSingleton l_singleton;
static HdStateDisposerHolder l_disposerHolder;
static u32 l_baseVTable;
static u32 l_objectVTable;
static u32 l_initialTaskVTable;
static u32 l_taskVTable;
static u32 l_delegateVTable;
static u32 l_disposerVTable;
static u32 l_stateIdVTable;
static u32 l_factory;
static u32 l_updateHandler;
static HdStateId l_waitState;
static HdStateId l_workState;
static HdStateId l_finishState;
static HdStateStatics l_statics;
static HdStateRange l_range;
static f32 l_lower;
static f32 l_upper;
static HdStateRegistration l_registration1;
static HdStateRegistration l_registration2;
static HdStateRegistration l_registration3;
static u32 l_stateIdInitialized;
static u32 l_lastStateId;

/* 025F9644 */
void hdStateTaskClearFlag(HdStateTask* self) {
    self->mFlag = 0;
}

/* 025F9650 */
u32 hdStateTaskIsIdle(HdStateTask* self) {
    return self->mProgress != HD_TASK_WORKING ? 1u : 0u;
}

/* 025F9664 */
void hdStateTaskUpdate(HdStateTask* self) {
    if (!hdStateTaskIsIdle(self)) return;
    self->mBusyCount = self->mBusyCount + 1;
    hdTaskThreadStep(&self->mThread);
    self->mBusyCount = self->mBusyCount - 1;
    hdTaskChangeState(&self->mState,&l_finishState);
}

/* 025F96E0 */
void hdStateTaskSetFlag(HdStateTask* self) {
    self->mFlag = 1;
}

/* 025F96EC */
HdStateTask* hdStateTaskConstruct(HdStateTask* self) {
    if (!self) {
        self = static_cast<HdStateTask*>(hdTaskAllocate(sizeof(HdStateTask)));
        if (!self) return NULL;
    }
    HdStateTask* base = self;
    if (!base) base = static_cast<HdStateTask*>(hdTaskAllocate(0x28));
    if (base) {
        base->mSelf = self;
        base->mReserved = 0;
        base->mObjectVTable = reinterpret_cast<gfn*>(&l_objectVTable);
        base->mVTable = reinterpret_cast<gfn*>(&l_baseVTable);
        base->mTaskVTable = reinterpret_cast<gfn*>(&l_initialTaskVTable);
        hdTaskStateConstruct(&base->mState,base,&l_factory);
    }
    self->mHeap = NULL;
    self->mTaskVTable = reinterpret_cast<gfn*>(&l_taskVTable);
    hdTaskQueueConstruct(&self->mQueue);
    hdTaskThreadConstruct(&self->mThread);
    self->mWorker = NULL;
    self->mSharedData = 0;
    // HD: preserve the embedded delegate's wrapping guest-address allocation check.
    HdStateDelegate* delegate = gabi::at<HdStateDelegate>(gabi::ea(self)+offsetof(HdStateTask,mUpdateDelegate));
    if (!delegate) delegate = static_cast<HdStateDelegate*>(hdTaskAllocate(sizeof(HdStateDelegate)));
    if (delegate) {
        delegate->mObject = self;
        delegate->mThisAdjustment = 0;
        delegate->mVTable = reinterpret_cast<gfn*>(&l_delegateVTable);
        delegate->mFunction = &l_updateHandler;
        delegate->mVirtualOffset = 0xFFFF;
    }
    self->mProgress = HD_TASK_IDLE;
    self->mFlag = 0;
    self->mObject = NULL;
    be<u32>* counter = gabi::at<be<u32>>(gabi::ea(self)+offsetof(HdStateTask,mBusyCount));
    if (!counter) {
        counter = static_cast<be<u32>*>(hdTaskAllocate(sizeof(be<u32>)));
        if (!counter) return self;
    }
    *counter = 0;
    return self;
}

/* 025F9818 */
void hdStateTaskCreateInstance(void* heap) {
    if (l_singleton.mInstance) return;
    HdStateTask* task = hdTaskAllocateInstance(sizeof(HdStateTask),heap,4);
    // HD: even a failed allocation passes the wrapped embedded disposer address onward.
    HdStateDisposer* disposer = gabi::at<HdStateDisposer>(gabi::ea(task)+offsetof(HdStateTask,mDisposer));
    if (disposer) {
        hdTaskDisposerConstruct(disposer,heap,3);
        disposer->mVTable = reinterpret_cast<gfn*>(&l_disposerVTable);
    }
    l_disposerHolder.mDisposer = disposer;
    l_singleton.mInstance = task ? hdStateTaskConstruct(task) : NULL;
}

/* 025F98B8 */
void hdStateTaskDestroy(HdStateTask* self,u32 flags) {
    if (!self) return;
    HdStateVTableObject* worker = self->mWorker;
    self->mTaskVTable = reinterpret_cast<gfn*>(&l_taskVTable);
    hdTaskThreadJoin(&self->mThread,worker);
    worker = self->mWorker;
    if (worker) {
        gabi::call_ptr<void>(worker->mVTable[5].get(),worker,3u);
        self->mWorker = NULL;
    }
    HdStateObject* object = self->mObject;
    if (object) {
        gabi::call_ptr<void>(object->mVTable[3].get(),object,3u);
        self->mObject = NULL;
    }
    hdTaskThreadDestroy(&self->mThread,2);
    hdTaskQueueDestroy(&self->mQueue,2);
    if (self) {
        HdStateVTableObject* factory = self->mState.mFactory;
        gabi::call_ptr<void>(factory->mVTable[7].get(),factory,&self->mState.mNext);
        factory = self->mState.mFactory;
        gabi::call_ptr<void>(factory->mVTable[7].get(),factory,&self->mState.mCurrent);
    }
    if (flags&1) hdTaskDelete(self);
}

/* 025F99BC */
void hdStateTaskStart(HdStateTask* self) {
    // HD: retain original frame allocation and the partially initialized parameter/message bytes.
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::Local<HdStateStartFrame> storage;
    HdStateStartFrame* frame = storage.get();
    hdTaskSharedData(2,0,&self->mSharedData,&frame->mSize);
    frame->mWord = 0;
    hdTaskParamsConstruct(&frame->mParams);
    HdStateVTableObject* worker = self->mWorker;
    frame->mParams.mMessageSize = 0x800;
    frame->mParams.mSharedData = &self->mSharedData;
    frame->mParams.mWord = &frame->mWord;
    frame->mParams.mStackSize = 0x400;
    frame->mParams.mPriority = 1;
    frame->mParams.mWorker = worker;
    frame->mParams.mSharedDataSize = &frame->mSize;
    frame->mParams.mAffinity = 0x1400;
    hdTaskThreadStart(&self->mThread,&frame->mParams);
    hdTaskMessageConstruct(&frame->mMessage);
    frame->mMessage.mBlocking = 1;
    frame->mMessage.mFlags = 0;
    frame->mMessage.mThread = &self->mThread;
    frame->mMessage.mKind = 0x19;
    hdTaskQueueSend(&self->mQueue,&frame->mMessage);
}

/* 025F9A90 */
void hdStateTaskInit(HdStateTask* self,void* heap) {
    self->mHeap = heap;
    HdStateVTableObject* worker = hdTaskAllocateWorker(8,heap,4);
    if (worker) worker = hdTaskWorkerConstruct(worker,self->mHeap);
    self->mWorker = worker;
    hdStateTaskStart(self);
    HdStateVTableObject* factory = self->mState.mFactory;
    HdStateVTableObject* state = gabi::call_ptr<HdStateVTableObject*>(factory->mVTable[5].get(),factory,&l_waitState);
    self->mState.mCurrent = state;
    gabi::call_ptr<void>(state->mVTable[7].get(),state);
}

/* 025F9B14 */
void hdStateTaskExecute(HdStateTask* self) {
    hdTaskStateExecute(&self->mState);
}

/* 025F9B1C */
u32 hdStateTaskRequest(HdStateTask* self) {
    if (!hdStateTaskIsIdle(self)) return 0;
    hdTaskThreadRequest(&self->mThread);
    return 1;
}

/* 025F9B68 */
u32 hdStateTaskRequestWhenNotBusy(HdStateTask* self) {
    if (self->mBusyCount) return 0;
    if (!hdStateTaskIsIdle(self)) return 0;
    hdTaskThreadWake(&self->mThread);
    return 1;
}

/* 025F9BC4 */
void hdStateTaskOnMessage(HdStateTask* self,void* object,u32 message) {
    if (message != HD_TASK_UPDATE) return;
    self->mProgress = HD_TASK_WORKING;
    hdTaskThreadWake(&self->mThread);
    hdTaskRunObject(object);
    self->mProgress = HD_TASK_DONE;
}

/* 025F9C28 */
void hdStateTaskDisposerDestroy(HdStateDisposer* disposer,u32 flags) {
    if (!disposer) return;
    disposer->mVTable = reinterpret_cast<gfn*>(&l_disposerVTable);
    if (disposer == l_disposerHolder.mDisposer) {
        HdStateTask* instance = l_singleton.mInstance;
        l_disposerHolder.mDisposer = NULL;
        gabi::call_ptr<void>(instance->mTaskVTable[3].get(),instance,2u);
        l_singleton.mInstance = NULL;
    }
    hdTaskDisposerDestroy(disposer,0);
    if (flags&1) hdTaskDelete(disposer);
}

static void setStateId(HdStateId* state,u32 id,const char* name,u16 enter,u16 execute,u16 exit) {
    state->mEnterAdjustment = 0;
    state->mId = id;
    state->mName = name;
    state->mEnterOffset = 0x24;
    state->mEnterIndex = enter;
    state->mExecuteAdjustment = 0;
    state->mExecuteIndex = execute;
    state->mExecuteOffset = 0x24;
    state->mExitOffset = 0x24;
    state->mReserved28 = 0;
    state->mExitAdjustment = 0;
    state->mExitIndex = exit;
    state->mVTable = reinterpret_cast<gfn*>(&l_stateIdVTable);
    state->mReserved24 = 0;
    state->mFactory = &l_factory;
    state->mReserved26 = 0;
}

/* 025F9CD0 */
void hdStateTaskStaticInit() {
    l_statics.mWords[2] = 0;
    l_statics.mWords[0] = 0;
    l_statics.mWords[3] = 0;
    l_statics.mWords[1] = 0;
    hdTaskRegister(&l_registration1);
    const f32 lower = l_lower;
    const f32 upper = l_upper;
    l_range.mLower = lower;
    l_range.mUpper = upper;
    hdTaskInitSigned(&l_range.mSigned);
    hdTaskRegister(&l_registration2);
    hdTaskInitUnsigned(&l_range.mUnsigned);
    hdTaskRegister(&l_registration3);
    u32 id = 0;
    if (l_stateIdInitialized) id = l_lastStateId;
    else l_stateIdInitialized = 1;
    setStateId(&l_waitState,id+1,gabi::at<const char>(gabi::guest_addr("Wait")),2,3,4);
    setStateId(&l_workState,id+2,gabi::at<const char>(gabi::guest_addr("InProgress")),5,6,7);
    l_lastStateId = id+3;
    setStateId(&l_finishState,id+3,gabi::at<const char>(gabi::guest_addr("Finish")),8,9,10);
}

/* WWHD glue (tools/verify/autoglue.py)
data l_singleton 101F4BD8
data l_disposerHolder 101F4BDC
data l_baseVTable 100E0E00
data l_objectVTable 100E0DC0
data l_initialTaskVTable 100E0DA0
data l_taskVTable 100E0F08
data l_delegateVTable 100E0DB0
data l_disposerVTable 100E0EF8
data l_stateIdVTable 100E0E68
data l_factory 101FF32C
data l_updateHandler 025F9BC4
data l_waitState 1048D7D4
data l_workState 1048D804
data l_finishState 1048D834
data l_statics 1048D7C4
data l_range 1048D7B8
data l_lower 100E0EB0
data l_upper 100E0EB4
data l_registration1 101F4BB4
data l_registration2 101F4BC0
data l_registration3 101F4BCC
data l_stateIdInitialized 101FD8E4
data l_lastStateId 101FDD50
str 100E0EB8 "Wait" renamed
str 100E0EC8 "InProgress" renamed
str 100E0EE0 "Finish" renamed
ext 0273AD10 void* hdTaskAllocate(u32)
ext 0273B0D4 HdStateTask* hdTaskAllocateInstance(u32,void*,u32)
ext 0273B050 HdStateVTableObject* hdTaskAllocateWorker(u32,void*,u32)
ext 0273AF40 void hdTaskDelete(void*)
ext 020062E8 void hdTaskStateConstruct(HdStateMachine*,HdStateTask*,void*)
ext 020063C0 void hdTaskChangeState(HdStateMachine*,HdStateId*)
ext 02006364 void hdTaskStateExecute(HdStateMachine*)
ext 028B2204 void hdTaskQueueConstruct(HdStateQueue*)
ext 028B28F8 void hdTaskThreadConstruct(HdStateThread*)
ext 02752B0C void hdTaskDisposerConstruct(HdStateDisposer*,void*,u32)
ext 02752BEC void hdTaskDisposerDestroy(HdStateDisposer*,u32)
ext 028B3E8C void hdTaskThreadStep(HdStateThread*)
ext 028B3124 void hdTaskThreadJoin(HdStateThread*,HdStateVTableObject*)
ext 028B2A40 void hdTaskThreadDestroy(HdStateThread*,u32)
ext 028B229C void hdTaskQueueDestroy(HdStateQueue*,u32)
ext C0009C58 void hdTaskSharedData(u32,u32,be<u32>*,be<u32>*)
ext 028B4230 void hdTaskParamsConstruct(HdStateThreadParams*)
ext 028B2B64 void hdTaskThreadStart(HdStateThread*,HdStateThreadParams*)
ext 028B289C void hdTaskMessageConstruct(HdStateMessage*)
ext 028B239C void hdTaskQueueSend(HdStateQueue*,HdStateMessage*)
ext 020039C0 HdStateVTableObject* hdTaskWorkerConstruct(HdStateVTableObject*,void*)
ext 028B3540 void hdTaskThreadRequest(HdStateThread*)
ext 028B3804 void hdTaskThreadWake(HdStateThread*)
ext 0275FFC8 void hdTaskRunObject(void*)
ext 028F026C void hdTaskRegister(void*)
ext 028ED6F8 void hdTaskInitSigned(be<u8>*)
ext 028EAB2C void hdTaskInitUnsigned(be<u8>*)
*/

/* WWHD sead task base: construction and destruction, attaching to the framework's method trees,
 * starting a task, shrinking its heaps to fit, the create-argument and heap-policy constructors
 * and the TU's empty virtual hooks and text copy. Reconstructed from this game's instructions. */

#include "sead/TaskBase.h"
#include "sead/TaskMgr.h"

using namespace sead;

void* allocateObject(u32 size);
void releaseObject(void* object);
void constructDisposer(sead::Disposer* disposer);
void destroyDisposer(sead::Disposer* disposer, u32 flags);
void constructArray(void* array, u32 count, u32 size, u32 constructor);

static constexpr u32 kDestroyRelease = 1;
static constexpr s32 kNoUserID = -1;
static constexpr u32 kHeapAlignment = 4;

static TaskBaseVtable sTaskBaseVtable;
static DisposerVtable sTaskBaseDisposerVtable;
static SafeTextVtable sTaskBaseTextVtable;
static u32 sTaskBaseTypeGuard;
static RuntimeType sTaskBaseType;
static u32 sTaskBaseTypeTable;
static PointerBuffer sRootHeaps;            // the heap manager's root heaps

/* the storage of a member object: its own address, or a new block when that address is NULL
 * (GHS member constructors allocate when their object pointer is NULL) */
template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

/* 027485F0 */
void sead::TaskBase::destroy(u32 flags) {
    if (this == NULL) return;
    TaskMgr* manager = mTaskMgr;
    mVtable = &sTaskBaseVtable;
    mDisposer.mVtable = &sTaskBaseDisposerVtable;
    if (manager != NULL) {
        TreeNode* child = mTreeNode.mChild;
        while (child != NULL) {
            manager->destroyTask(child->data<TaskBase>());
            child = mTreeNode.mChild;
            if (child != NULL) manager = mTaskMgr;
        }
    }
    mState = cDead;
    mTreeNode.detachAll();
    mListNode.erase();
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027486D0 */
static void destroyTaskBaseFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<TaskBase*>(reinterpret_cast<u8*>(disposer) - offsetof(TaskBase, mDisposer))->destroy(flags);
}

/* 027486D8 */
void sead::TaskBase::attachCalcDraw() {
    gabi::call_ptr<void>(mVtable->mAttachCalc.mFunction, this);
    gabi::call_ptr<void>(mVtable->mAttachDraw.mFunction, this);
}

/* 02748724 */
void sead::TaskBase::start() {
    attachCalcDraw();
    gabi::call_ptr<void>(mVtable->mPauseCalc.mFunction, this, 0);
    gabi::call_ptr<void>(mVtable->mPauseDraw.mFunction, this, 0);
    gabi::call_ptr<void>(mVtable->mEnter.mFunction, this);
}

/* 02748794 */
MethodTreeMgrBase* sead::TaskBase::getMethodTreeMgr() {
    // the framework's manager derives from MethodTreeMgrBase (sead/MethodTree.h holds its own view)
    return reinterpret_cast<MethodTreeMgrBase*>(mTaskMgr->mFramework->mMethodTreeMgr.get());
}

/* 027487A4 */
void sead::TaskBase::attachToMethodTree(u32 phase, MethodTreeNode* node) {
    getMethodTreeMgr()->attachMethod(phase, node);
}

/* 027487D4 */
u32 sead::TaskBase::requestCreateTask(const TaskCreateArg* arg) {
    return mTaskMgr->requestCreateTask(arg);
}

/* 027487DC */
void sead::TaskBase::adjustHeap(u32 index, u32 reserve) {
    RegistryHeap* heap = index < (u32)sRootHeaps.mCount ? mHeapArray.mHeaps[index] : mHeapArray.mHeaps[0];
    if (heap == NULL) return;
    if (mHeapArray.mAdjusted[index] != 0) return;
    mHeapArray.mAdjusted[index] = 1;
    if (reserve != 0) {
        // keep the reserve free: allocate it, shrink the heap, then release it again
        void* block = gabi::call_ptr<void*>(heap->dispatch()->mAllocate.mFunction, heap, reserve, kHeapAlignment);
        gabi::call_ptr<void>(heap->dispatch()->mTrimToParent.mFunction, heap);
        if (block != NULL) gabi::call_ptr<void>(heap->dispatch()->mRelease.mFunction, heap, block);
    } else {
        gabi::call_ptr<void>(heap->dispatch()->mTrimToParent.mFunction, heap);
    }
}

/* 027488E8 */
void sead::TaskBase::adjustHeapAll() {
    CriticalSection_l* section = &mTaskMgr->mCriticalSection;
    section->lock();
    for (s32 i = 0; i < sRootHeaps.mCount; i++) adjustHeap(i, 0);
    section->unlock();
}

/* 02748974 */
Framework* sead::TaskBase::getFramework() {
    return mTaskMgr->mFramework;
}

/* 02748980 */
TaskBase* sead::TaskBase::construct(TaskBase* task, const TaskConstructArg* arg, const be<u8>* name) {
    task = memberStorage(task);
    if (task == NULL) return NULL;
    TreeNode* treeNode = memberStorage(&task->mTreeNode);
    if (treeNode != NULL) {
        TreeNode::construct(treeNode);
        treeNode->mData = task;
    }
    constructDisposer(&task->mDisposer);
    SafeText* text = memberStorage(&task->mName);
    if (text != NULL) {
        text->mVtable = &sTaskBaseTextVtable;
        text->mText.v = gabi::guest_str("");
    }
    task->mVtable = &sTaskBaseVtable;
    task->mDisposer.mVtable = &sTaskBaseDisposerVtable;
    task->mParameter = arg->mParameter;
    be<u32>* flags = memberStorage(&task->mFlags);
    if (flags != NULL) *flags = 0;
    TaskListNode* listNode = memberStorage(&task->mListNode);
    if (listNode != NULL) {
        listNode->mNext = NULL;
        listNode->mTask = task;
        listNode->mPrevious = NULL;
        listNode->mList = NULL;
    }
    // the heap array is copied word by word
    const HeapArray* heaps = arg->mHeapArray;
    task->mHeapArray.mHeaps[0] = heaps->mHeaps[0];
    task->mHeapArray.mHeaps[1] = heaps->mHeaps[1];
    task->mHeapArray.mHeaps[2] = heaps->mHeaps[2];
    task->mHeapArray.mHeaps[3] = heaps->mHeaps[3];
    gabi::store<u32>(gabi::ea(task->mHeapArray.mAdjusted), gabi::load<u32>(gabi::ea(heaps->mAdjusted)));
    task->mHeapArray.mPrimaryIndex = heaps->mPrimaryIndex;
    TaskMgr* manager = arg->mTaskMgr;
    task->mTag = cApp;
    task->mTaskMgr = manager;
    task->mState = cCreated;
    task->mClassID.mType = TaskClassID::cTag_None;
    task->mFlags = 0;
    task->mClassID.mValue = 0;
    task->mName.mText = const_cast<be<u8>*>(name);
    return task;
}

/* 02748AFC */
TaskCreateArg* sead::TaskCreateArg::construct(TaskCreateArg* arg) {
    arg = memberStorage(arg);
    if (arg == NULL) return NULL;
    arg->mClassID.mType = TaskClassID::cTag_None;
    arg->mClassID.mValue = 0;
    constructArray(arg->mHeapPolicies, kMaxTaskHeaps, sizeof(HeapPolicy),
                   gabi::guest_fn(reinterpret_cast<const void*>(&HeapPolicy::construct)));
    arg->mListener = NULL;
    arg->mParent = NULL;
    arg->mTag = TaskBase::cApp;
    arg->mCreateCallback = 0;
    arg->mUserID = kNoUserID;
    arg->mFader = NULL;
    arg->mParameter = NULL;
    arg->mPrimaryHeapIndex = 0;
    arg->mName = NULL;
    arg->mCreatedTask = NULL;
    return arg;
}

/* 02748B98 */
TaskCreateArg* sead::TaskCreateArg::constructForClass(TaskCreateArg* arg, const TaskClassID* classID) {
    arg = memberStorage(arg);
    if (arg == NULL) return NULL;
    arg->mClassID.mType = classID->mType;
    arg->mClassID.mValue = classID->mValue;
    constructArray(arg->mHeapPolicies, kMaxTaskHeaps, sizeof(HeapPolicy),
                   gabi::guest_fn(reinterpret_cast<const void*>(&HeapPolicy::construct)));
    arg->mCreatedTask = NULL;
    arg->mCreateCallback = 0;
    arg->mTag = TaskBase::cApp;
    arg->mPrimaryHeapIndex = 0;
    arg->mUserID = kNoUserID;
    arg->mName = NULL;
    arg->mParameter = NULL;
    arg->mFader = NULL;
    arg->mParent = NULL;
    arg->mListener = NULL;
    return arg;
}

/* 02748C40 */
TaskCreateArg* sead::TaskCreateArg::constructPrimaryHeapOnly(TaskCreateArg* arg, const TaskClassID* classID) {
    arg = memberStorage(arg);
    if (arg == NULL) return NULL;
    constructForClass(arg, classID);
    // every heap is shrunk to fit, and only the primary heap is created
    s32 count = sRootHeaps.mCount;
    for (s32 i = 0; i < count; i++) arg->mHeapPolicies[i].mAdjust = 1;
    u32 primary = arg->mPrimaryHeapIndex;
    if (primary < (u32)sRootHeaps.mCount) {
        count = sRootHeaps.mCount;
        for (s32 i = 0; i < count; i++) arg->mHeapPolicies[i].mNoHeap = (u32)i != primary;
        arg->mPrimaryHeapIndex = primary;
    }
    return arg;
}

/* 02748CFC */
TaskCreateArg* sead::TaskCreateArg::constructSystem(TaskCreateArg* arg, const TaskClassID* classID) {
    arg = memberStorage(arg);
    if (arg != NULL) {
        constructPrimaryHeapOnly(arg, classID);
        arg->mTag = TaskBase::cSystem;
    }
    return arg;
}

/* 02748D5C */
void sead::task_base_instances::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02748D70 */
void sead::task_base_instances::Text::assureTerminationImpl() {}

/* 02748D74 */
HeapPolicy* sead::HeapPolicy::construct(HeapPolicy* policy) {
    policy = memberStorage(policy);
    if (policy != NULL) {
        policy->mFromEnd = 0;
        policy->mLeaveFree = 0;
        policy->mNoHeap = 0;
        policy->mAdjust = 0;
        policy->mSize = 0;
        policy->mParent = NULL;
        policy->mAdjustReserve = 0;
    }
    return policy;
}

/* 02748DC8 */
bool sead::TaskBase::isKindOf(const RuntimeType* type) const {
    if (sTaskBaseTypeGuard == 0) {
        sTaskBaseTypeGuard = 1;
        sTaskBaseType.mTable = &sTaskBaseTypeTable;
    }
    return type == &sTaskBaseType;
}

/* 02748E04 */
void sead::TaskBase::prepare() {}

/* 02748E08 */
void sead::TaskBase::enter() {}

/* 02748E0C */
void sead::TaskBase::exit() {}

/* 02748E10 */
void sead::TaskBase::onEvent(const TaskEvent* event) {}

/* WWHD glue
 ext 0273AD10 void* allocateObject(u32)
 ext 0273AF40 void releaseObject(void*)
 ext 02752A84 void constructDisposer(sead::Disposer*)
 ext 02752BEC void destroyDisposer(sead::Disposer*, u32)
 ext 028EFFD0 void constructArray(void*, u32, u32, u32)
 ext 02767360 void sead::TreeNode::construct(sead::TreeNode*)
 ext 0273B310 sead::Link* sead::Link::detach()
 data sTaskBaseVtable 10144BC8 nocheck
 data sTaskBaseDisposerVtable 10144C78 nocheck
 data sTaskBaseTextVtable 10144B9C nocheck
 data sTaskBaseTypeGuard 101FD594
 data sTaskBaseType 101FD980
 data sTaskBaseTypeTable 10144BB4
 data sRootHeaps 104A030C nocheck
 str 10144BC4 "" in 02748980
*/

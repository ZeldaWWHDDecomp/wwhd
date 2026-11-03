/* WWHD sead task with a calc and a draw method node: construction (both nodes bound to the
 * task's calc and draw), destruction, attaching the nodes to the system or app method trees (app
 * tasks below their parent task's nodes), detaching and pausing them; the TU's copies of the text,
 * method node and delegates, and the method tree manager base the TU constructs.
 * Reconstructed from this game's instructions. */

#include "sead/Task.h"
#include "sead/MethodTree.h"

using namespace sead;

void* allocateObject(u32 size);
void releaseObject(void* object);
void constructDisposer(sead::Disposer* disposer);
void destroyDisposer(sead::Disposer* disposer, u32 flags);
void constructCriticalSection(sead::CriticalSection_l* section);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kTaskVtableOffset = 0x70;  // where a task keeps its vtable
static constexpr s16 kCalcSlot = 0x16;          // virtual slots of calc and draw
static constexpr s16 kDrawSlot = 0x17;

static TaskBaseVtable sTaskVtable;
static DisposerVtable sTaskDisposerVtable;
static SafeTextVtable sTaskTextVtable;
static MethodTreeNodeVtable sTaskNodeVtable;
static DisposerVtable sTaskNodeDisposerVtable;
static DelegateVtable sTaskDelegateBaseVtable;
static DelegateVtable sTaskNodeDelegateVtable;
static DelegateVtable sTaskTaskDelegateVtable;
static u32 sTaskTypeGuard;
static RuntimeType sTaskType;
static u32 sTaskTypeTable;
static u32 sTaskBaseTypeGuard;
static RuntimeType sTaskBaseType;
static u32 sTaskTaskBaseTypeTable;
static MethodTreeMgrBaseVtable sMethodTreeMgrBaseVtable;
static u32 sMgrBaseTypeGuard;
static RuntimeType sMgrBaseType;
static u32 sMgrBaseTypeTable;

/* the storage of a member object: its own address, or a new block when that address is NULL
 * (GHS member constructors allocate when their object pointer is NULL) */
template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

/* inlined into the constructor: a node with an empty name, an unbound delegate, not paused */
static void constructNode(MethodTreeNode* node) {
    TreeNode* treeNode = memberStorage(&node->mTreeNode);
    if (treeNode != NULL) {
        TreeNode::construct(treeNode);
        treeNode->mData = node;
    }
    SafeText* name = memberStorage(&node->mName);
    if (name != NULL) {
        name->mVtable = &sTaskTextVtable;
        name->mText.v = gabi::guest_str("");
    }
    constructDisposer(&node->mDisposer);
    node->mCriticalSection = NULL;
    node->mVtable = &sTaskNodeVtable;
    node->mDisposer.mVtable = &sTaskNodeDisposerVtable;
    be<u32>* pauseFlags = memberStorage(&node->mPauseFlags);
    if (pauseFlags != NULL) *pauseFlags = MethodTreeNode::cPauseNone;
    node->mUserData = NULL;
    node->mPauseListener = NULL;
    MethodDelegate* delegate = &node->mDelegate;
    if (gabi::ea(delegate) != 0) {
        MethodDelegate* storage = memberStorage(delegate);
        if (storage != NULL) {
            storage->mMethod.mTarget = 0;
            storage->mVtable = &sTaskDelegateBaseVtable;
            storage->mMethod.mObjectAdjustment = 0;
            storage->mMethod.mVirtualIndex = 0;
            storage->mObject = NULL;
        }
        delegate->mVtable = &sTaskNodeDelegateVtable;
    }
    node->mPauseFlags = MethodTreeNode::cPauseNone;
}

/* inlined into the constructor: bind the node to a virtual function of the task */
static void bindTaskMethod(MethodTreeNode* node, Task* task, s16 slot, const be<u8>* name) {
    node->lock();
    MethodDelegate* delegate = &node->mDelegate;
    if (gabi::ea(delegate) != 0) {
        delegate->mVtable = &sTaskTaskDelegateVtable;
        delegate->mObject = reinterpret_cast<u8*>(task);
        delegate->mMethod.mVirtualIndex = slot;
        delegate->mMethod.mTarget = kTaskVtableOffset;
        delegate->mMethod.mObjectAdjustment = 0;
    }
    node->unlock();
    if (name != NULL) node->mName.mText = const_cast<be<u8>*>(name);
}

/* 02747CA8 */
MethodTreeMgrBase* sead::MethodTreeMgrBase::construct(MethodTreeMgrBase* manager) {
    manager = memberStorage(manager);
    if (manager != NULL) {
        manager->mVtable = &sMethodTreeMgrBaseVtable;
        constructCriticalSection(&manager->mCriticalSection);
    }
    return manager;
}

/* 02747CFC */
bool sead::MethodTreeMgrBase::isKindOf(const RuntimeType* type) const {
    if (sMgrBaseTypeGuard == 0) {
        sMgrBaseTypeGuard = 1;
        sMgrBaseType.mTable = &sMgrBaseTypeTable;
    }
    return type == &sMgrBaseType;
}

/* 02747D38 */
static void destroyTaskNodeFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<task_instances::MethodTreeNode*>(reinterpret_cast<u8*>(disposer) - offsetof(MethodTreeNode, mDisposer))
        ->destroy(flags);
}

/* 02747D40 */
void sead::Task::destroy(u32 flags) {
    if (this == NULL) return;
    mDrawEntry.mDisposer.mVtable = &sTaskNodeDisposerVtable;
    mTaskBase.mDisposer.mVtable = &sTaskDisposerVtable;
    mDrawEntry.mVtable = &sTaskNodeVtable;
    mDrawEntry.detachAll();
    destroyDisposer(&mDrawEntry.mDisposer, 0);
    mCalcEntry.mVtable = &sTaskNodeVtable;
    mCalcEntry.mDisposer.mVtable = &sTaskNodeDisposerVtable;
    mCalcEntry.detachAll();
    destroyDisposer(&mCalcEntry.mDisposer, 0);
    mTaskBase.destroy(0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02747DF8 */
static void destroyTaskFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<Task*>(reinterpret_cast<u8*>(disposer) - offsetof(TaskBase, mDisposer))->destroy(flags);
}

/* 02747E00 */
void sead::Task::attachCalc() {
    MethodTreeMgrBase* manager = mTaskBase.getMethodTreeMgr();
    manager->mCriticalSection.lock();
    TaskBase* parent = mTaskBase.parent();
    if (mTaskBase.mTag != TaskBase::cSystem) {
        if (parent != NULL) {
            parent->getMethodTreeNode(cCalcApp)->pushBackChild(&mCalcEntry);
        } else {
            mTaskBase.attachToMethodTree(cCalcApp, &mCalcEntry);
        }
    } else {
        mTaskBase.attachToMethodTree(cCalcSys, &mCalcEntry);
    }
    manager->mCriticalSection.unlock();
}

/* 02747EB8 */
void sead::Task::attachDraw() {
    MethodTreeMgrBase* manager = mTaskBase.getMethodTreeMgr();
    manager->mCriticalSection.lock();
    TaskBase* parent = mTaskBase.parent();
    if (mTaskBase.mTag != TaskBase::cSystem) {
        if (parent != NULL) {
            parent->getMethodTreeNode(cDrawApp)->pushFrontChild(&mDrawEntry);
        } else {
            mTaskBase.attachToMethodTree(cDrawApp, &mDrawEntry);
        }
    } else {
        mTaskBase.attachToMethodTree(cDrawSys, &mDrawEntry);
    }
    manager->mCriticalSection.unlock();
}

/* 02747F70 */
void sead::Task::detachCalc() { mCalcEntry.detachAll(); }

/* 02747F78 */
void sead::Task::detachDraw() { mDrawEntry.detachAll(); }

/* 02747F80 */
void sead::Task::pauseCalc(u32 pause) {
    mCalcEntry.setPauseFlags(pause ? MethodTreeNode::cPauseSelf : MethodTreeNode::cPauseNone);
}

/* 0274802C */
void sead::Task::pauseDraw(u32 pause) {
    mDrawEntry.setPauseFlags(pause ? MethodTreeNode::cPauseSelf : MethodTreeNode::cPauseNone);
}

/* 027480D8 */
MethodTreeNode* sead::Task::getMethodTreeNode(u32 phase) {
    if (phase <= cCalcApp) return &mCalcEntry;
    if (phase <= cDrawAppFinal) return &mDrawEntry;
    return NULL;
}

/* 02748100 */
Task* sead::Task::construct(Task* task, const TaskConstructArg* arg, const be<u8>* name) {
    task = memberStorage(task);
    if (task == NULL) return NULL;
    TaskBase::construct(&task->mTaskBase, arg, name);
    task->mTaskBase.mVtable = &sTaskVtable;
    task->mTaskBase.mDisposer.mVtable = &sTaskDisposerVtable;
    constructNode(&task->mCalcEntry);
    constructNode(&task->mDrawEntry);
    task->mCalcEntry.setPauseFlags(MethodTreeNode::cPauseBoth);
    task->mDrawEntry.setPauseFlags(MethodTreeNode::cPauseBoth);
    bindTaskMethod(&task->mCalcEntry, task, kCalcSlot, name);
    bindTaskMethod(&task->mDrawEntry, task, kDrawSlot, name);
    return task;
}

/* 0274843C */
void sead::task_instances::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02748450 */
void sead::task_instances::MethodTreeNode::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sTaskNodeVtable;
    mDisposer.mVtable = &sTaskNodeDisposerVtable;
    detachAll();
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027484C4 */
void sead::task_instances::Text::assureTerminationImpl() {}

/* 027484C8 */
void sead::task_instances::NodeDelegate::invoke() {
    u8* object;
    u32 target;
    if (resolve(object, target)) gabi::tail_ptr<void>(target, object);
}

/* 0274851C */
bool sead::Task::isKindOf(const RuntimeType* type) const {
    if (sTaskTypeGuard == 0) {
        sTaskTypeGuard = 1;
        sTaskType.mTable = &sTaskTypeTable;
    }
    if (type == &sTaskType) return true;
    if (sTaskBaseTypeGuard == 0) {
        sTaskBaseTypeGuard = 1;
        sTaskBaseType.mTable = &sTaskTaskBaseTypeTable;
    }
    return type == &sTaskBaseType;
}

/* 02748594 */
void sead::Task::calc() {}

/* 02748598 */
void sead::Task::draw() {}

/* 0274859C */
void sead::task_instances::TaskDelegate::invoke() {
    u8* object;
    u32 target;
    if (resolve(object, target)) gabi::tail_ptr<void>(target, object);
}

/* WWHD glue
 ext 02760084 void constructCriticalSection(sead::CriticalSection_l*)
 data sTaskVtable 10144ACC nocheck
 data sTaskDisposerVtable 10144B8C nocheck
 data sTaskTextVtable 10144A18 nocheck
 data sTaskNodeVtable 10144A58 nocheck
 data sTaskNodeDisposerVtable 10144A78 nocheck
 data sTaskDelegateBaseVtable 10144A30 nocheck
 data sTaskNodeDelegateVtable 10144A88 nocheck
 data sTaskTaskDelegateVtable 10144AB0 nocheck
 data sTaskTypeGuard 101FD6D0
 data sTaskType 101FD9DC
 data sTaskTypeTable 10144AA0
 data sTaskBaseTypeGuard 101FD594
 data sTaskBaseType 101FD980
 data sTaskTaskBaseTypeTable 10144A48
 data sMethodTreeMgrBaseVtable 101449D8 nocheck
 data sMgrBaseTypeGuard 101FD714
 data sMgrBaseType 101FD9A8
 data sMgrBaseTypeTable 101449C8
 str 10144AC8 "" in 02748100
*/

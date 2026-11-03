/* WWHD sead method trees: the calc-only task, the method tree manager, the calc+draw task and the
 * framework's run and initialization, with the per-TU copies of their texts, node destructors and
 * delegates (each task file and the manager file store their own copies).
 * Reconstructed from this game's instructions. */

#include "sead/MethodTree.h"
#include "sead/Random.h"

using namespace sead;

/* functions of other units (bound by the glue block) */
void* allocateObject(u32 size);
void releaseObject(void* object);
void constructDisposer(sead::Disposer* disposer);
void destroyDisposer(sead::Disposer* disposer, u32 flags);
void constructTreeNode(sead::TreeNode_l* node);
void constructTaskBase(sead::TaskBase_l* task, void* parameters, const be<u8>* name);
void destroyTaskBase(sead::TaskBase_l* task, u32 flags);
void constructMethodTreeMgrBase(sead::MethodTreeMgrBase_l* base);
void lockCriticalSection(void* section);
void unlockCriticalSection(void* section);
void destroyFrameworkMethodTreeNode(sead::MethodTreeNode_l* node, u32 flags);
void initHeapMgrFromRootHeap(void* heap);
void initHeapMgrFromArena(void* arena);
void createThreadMgr(void* heap);
void initThreadMgr(void* threadMgr, void* heap);
void finishFrameworkInitialize();
void* createTaskMgr(sead::TaskMgrInitArgs* arg);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kTaskBaseVtableOffset = 0x70;   // where a task keeps its vtable
static constexpr s16 kCalcSlot = 0x16;               // virtual slots of calc and the two draws
static constexpr s16 kDrawTopSlot = 0x17;
static constexpr s16 kDrawBottomSlot = 0x18;
static constexpr u32 kTaskMgrContextCount = 0x20;
static constexpr u32 kDefaultTaskMgrStackSize = 0x8000;

/* calc-only task file */
static TaskBaseVtable_l sCalcTaskVtable;
static DisposerVtable sCalcTaskDisposerVtable;
static SafeTextVtable sCalcTaskTextVtable;
static MethodTreeNodeVtable_l sCalcTaskNodeVtable;
static DisposerVtable sCalcTaskNodeDisposerVtable;
static DelegateVtable_l sCalcTaskDelegateBaseVtable;
static DelegateVtable_l sCalcTaskNodeDelegateVtable;
static DelegateVtable_l sCalcTaskTaskDelegateVtable;
static u32 sCalcTaskTypeGuard;
static RuntimeType sCalcTaskType;
static u32 sCalcTaskTypeTable;
static u32 sTaskBaseTypeGuard;
static RuntimeType sTaskBaseType;
static u32 sCalcTaskTaskBaseTypeTable;

/* method tree manager file */
static RuntimeTypedVtable_l sMethodTreeMgrVtable;
static SafeTextVtable sMgrTextVtable;
static MethodTreeNodeVtable_l sMgrNodeVtable;
static DisposerVtable sMgrNodeDisposerVtable;
static DelegateVtable_l sMgrDelegateBaseVtable;
static DelegateVtable_l sMgrNodeDelegateVtable;
static u32 sMgrMethodTreeMgrTypeTable;
static u32 sMgrBaseTypeGuard;
static RuntimeType sMgrBaseType;
static u32 sMgrMgrBaseTypeTable;

/* calc+draw task file */
static TaskBaseVtable_l sCalcDrawTaskVtable;
static DisposerVtable sCalcDrawTaskDisposerVtable;
static SafeTextVtable sCalcDrawTaskTextVtable;
static MethodTreeNodeVtable_l sCalcDrawTaskNodeVtable;
static DisposerVtable sCalcDrawTaskNodeDisposerVtable;
static DelegateVtable_l sCalcDrawTaskDelegateBaseVtable;
static DelegateVtable_l sCalcDrawTaskNodeDelegateVtable;
static DelegateVtable_l sCalcDrawTaskTaskDelegateVtable;
static u32 sCalcDrawTaskTypeGuard;
static RuntimeType sCalcDrawTaskType;
static u32 sCalcDrawTaskTypeTable;
static u32 sCalcDrawTaskTaskBaseTypeTable;
static u32 sCalcDrawTaskMethodTreeMgrTypeTable;

/* framework file */
static FrameworkVtable_l sFrameworkVtable;
static ObjectListVtable_l sObjectListVtable;
static SafeTextVtable sFrameworkTextVtable;
static PointerBuffer sRootHeaps;          // the heap manager's root heaps
static void* sThreadMgr;

/* ---- inline helpers ---- */

/* the storage of a member object: its own address, or a new block when that address is NULL
 * (GHS member constructors allocate when their object pointer is NULL) */
template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

/* inlined into the tasks' pause functions and the constructors */
static void setPauseFlags(MethodTreeNode_l* node, u32 flags) {
    node->lock();
    PauseListener_l* listener = node->mPauseListener;
    if (listener != NULL) gabi::call_ptr<void>(listener->mVtable->mOnPauseChange.mFunction, listener, node, flags);
    node->mPauseFlags = flags;
    node->unlock();
}

/* inlined into the constructors: a node with an empty name, an unbound delegate, not paused */
static void constructMethodTreeNode(MethodTreeNode_l* node, void* owner, const char* emptyText,
                                    SafeTextVtable* textVtable, MethodTreeNodeVtable_l* nodeVtable,
                                    DisposerVtable* disposerVtable, DelegateVtable_l* delegateBaseVtable,
                                    DelegateVtable_l* delegateVtable) {
    TreeNode_l* treeNode = memberStorage(&node->mTreeNode);
    if (treeNode != NULL) {
        constructTreeNode(treeNode);
        treeNode->mData = node;
    }
    SafeText* name = memberStorage(&node->mName);
    if (name != NULL) {
        name->mText.v = gabi::guest_str(emptyText);
        name->mVtable = textVtable;
    }
    constructDisposer(&node->mDisposer);
    node->mOwner = owner;
    node->mVtable = nodeVtable;
    node->mDisposer.mVtable = disposerVtable;
    be<u32>* pauseFlags = memberStorage(&node->mPauseFlags);
    if (pauseFlags != NULL) *pauseFlags = cPauseNone;
    node->mPauseListener = NULL;
    node->mReserved4C = 0;
    MethodDelegate_l* delegate = &node->mDelegate;
    if (gabi::ea(delegate) != 0) {
        MethodDelegate_l* storage = memberStorage(delegate);
        if (storage != NULL) {
            storage->mMethod.mTarget = 0;
            storage->mVtable = delegateBaseVtable;
            storage->mMethod.mObjectAdjustment = 0;
            storage->mMethod.mVirtualIndex = 0;
            storage->mObject = NULL;
        }
        delegate->mVtable = delegateVtable;
    }
    node->mPauseFlags = cPauseNone;
}

/* inlined into the task constructors: bind the node to a virtual function of the task */
static void bindTaskMethod(MethodTreeNode_l* node, void* task, DelegateVtable_l* delegateVtable, s16 slot,
                           const be<u8>* name) {
    node->lock();
    MethodDelegate_l* delegate = &node->mDelegate;
    if (gabi::ea(delegate) != 0) {
        delegate->mObject = static_cast<u8*>(task);
        delegate->mMethod.mObjectAdjustment = 0;
        delegate->mVtable = delegateVtable;
        delegate->mMethod.mTarget = kTaskBaseVtableOffset;
        delegate->mMethod.mVirtualIndex = slot;
    }
    node->unlock();
    if (name != NULL) node->mName.mText = const_cast<be<u8>*>(name);
}

/* ================================================================ calc-only task */

/* 027452C0 */
static void destroyCalcTaskNodeFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<calc_task_instances::MethodTreeNode*>(reinterpret_cast<u8*>(disposer) - 0x1C)->destroy(flags);
}

/* 027452C8 */
void sead::CalcTask::destroy(u32 flags) {
    if (this == NULL) return;
    mTaskBase.mDisposer.mVtable = &sCalcTaskDisposerVtable;
    mCalcEntry.mDisposer.mVtable = &sCalcTaskNodeDisposerVtable;
    mCalcEntry.mVtable = &sCalcTaskNodeVtable;
    mCalcEntry.detachAll();
    destroyDisposer(&mCalcEntry.mDisposer, 0);
    destroyTaskBase(&mTaskBase, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02745354 */
static void destroyCalcTaskFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<CalcTask*>(reinterpret_cast<u8*>(disposer) - 0x14)->destroy(flags);
}

/* 0274535C */
void sead::CalcTask::attachCalc() {
    MethodTreeMgr* manager = mTaskBase.getMethodTreeMgr();
    lockCriticalSection(manager);
    TreeNode_l* parentNode = mTaskBase.mTreeNode.mParent;
    u32 attachToParent = mTaskBase.mAttachToParent;
    TaskBase_l* parent = parentNode != NULL ? static_cast<TaskBase_l*>(parentNode->mData.get()) : NULL;
    if (attachToParent != 0) {
        if (parent != NULL) {
            parent->getMethodTreeNode(cCalcApp)->pushBackChild(&mCalcEntry);
        } else {
            mTaskBase.attachToMethodTree(cCalcApp, &mCalcEntry);
        }
    } else {
        mTaskBase.attachToMethodTree(cCalcSys, &mCalcEntry);
    }
    unlockCriticalSection(manager);
}

/* 02745414 */
void sead::CalcTask::detachCalc() { mCalcEntry.detachAll(); }

/* 0274541C */
void sead::CalcTask::pauseCalc(u32 pause) { setPauseFlags(&mCalcEntry, pause ? cPauseSelf : cPauseNone); }

/* 027454C8 */
MethodTreeNode_l* sead::CalcTask::getMethodTreeNode(u32 phase) { return phase <= cCalcApp ? &mCalcEntry : NULL; }

/* 027454E0 */
CalcTask* sead::CalcTask::construct(CalcTask* task, void* parameters, const be<u8>* name) {
    task = memberStorage(task);
    if (task == NULL) return NULL;
    constructTaskBase(&task->mTaskBase, parameters, name);
    task->mTaskBase.mVtable = &sCalcTaskVtable;
    task->mTaskBase.mDisposer.mVtable = &sCalcTaskDisposerVtable;
    constructMethodTreeNode(&task->mCalcEntry, NULL, "", &sCalcTaskTextVtable, &sCalcTaskNodeVtable,
                            &sCalcTaskNodeDisposerVtable, &sCalcTaskDelegateBaseVtable, &sCalcTaskNodeDelegateVtable);
    setPauseFlags(&task->mCalcEntry, cPauseSelfAndChild);
    bindTaskMethod(&task->mCalcEntry, task, &sCalcTaskTaskDelegateVtable, kCalcSlot, name);
    return task;
}

/* 027456D0 */
void sead::calc_task_instances::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027456E4 */
void sead::calc_task_instances::MethodTreeNode::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sCalcTaskNodeVtable;
    mDisposer.mVtable = &sCalcTaskNodeDisposerVtable;
    detachAll();
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02745758 */
void sead::calc_task_instances::Text::assureTerminationImpl() {}

/* 0274575C */
void sead::calc_task_instances::NodeDelegate::invoke() {
    u8* object = mObject;
    if (object == NULL) return;
    s16 index = mMethod.mVirtualIndex;
    if (index == 0) return;
    u8* adjusted = object + (s32)mMethod.mObjectAdjustment;
    u32 target;
    if (index < 0) {
        target = mMethod.mTarget;
    } else {
        s16 vtableOffset = (u32)mMethod.mTarget;
        VirtualEntry* vtable = *reinterpret_cast<gptr<VirtualEntry>*>(adjusted + vtableOffset);
        target = vtable[index].mTarget;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 027457B0 */
static u32 isKindOfCalcTask(CalcTask* task, const RuntimeType* type) {
    if (sCalcTaskTypeGuard == 0) {
        sCalcTaskTypeGuard = 1;
        sCalcTaskType.mTable = &sCalcTaskTypeTable;
    }
    if (type == &sCalcTaskType) return 1;
    if (sTaskBaseTypeGuard == 0) {
        sTaskBaseTypeGuard = 1;
        sTaskBaseType.mTable = &sCalcTaskTaskBaseTypeTable;
    }
    return type == &sTaskBaseType;
}

/* 02745828 */
void sead::CalcTask::calc() {}

/* 0274582C */
void sead::CalcTask::attachDraw() {}

/* 02745830 */
void sead::CalcTask::detachDraw() {}

/* 02745834 */
void sead::CalcTask::pauseDraw(u32 pause) {}

/* 02745838 */
void sead::calc_task_instances::TaskDelegate::invoke() {
    u8* object = mObject;
    if (object == NULL) return;
    s16 index = mMethod.mVirtualIndex;
    if (index == 0) return;
    u8* adjusted = object + (s32)mMethod.mObjectAdjustment;
    u32 target;
    if (index < 0) {
        target = mMethod.mTarget;
    } else {
        s16 vtableOffset = (u32)mMethod.mTarget;
        VirtualEntry* vtable = *reinterpret_cast<gptr<VirtualEntry>*>(adjusted + vtableOffset);
        target = vtable[index].mTarget;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* ================================================================ method tree manager */

/* 0274588C */
static void destroyMgrNodeFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<method_tree_mgr_instances::MethodTreeNode*>(reinterpret_cast<u8*>(disposer) - 0x1C)->destroy(flags);
}

/* 02745894 */
MethodTreeMgr* sead::MethodTreeMgr::construct(MethodTreeMgr* manager) {
    manager = memberStorage(manager);
    if (manager == NULL) return NULL;
    constructMethodTreeMgrBase(&manager->mBase);
    manager->mBase.mVtable = &sMethodTreeMgrVtable;
    MethodTreeNode_l* nodes[] = {
        &manager->mRootCalcNode, &manager->mSysCalcNode, &manager->mAppCalcNode,
        &manager->mTopRootDrawNode, &manager->mTopSysDrawNode, &manager->mTopAppDrawNode, &manager->mTopAppDrawFinalNode,
        &manager->mBtmRootDrawNode, &manager->mBtmSysDrawNode, &manager->mBtmAppDrawNode, &manager->mBtmAppDrawFinalNode,
    };
    for (MethodTreeNode_l* node : nodes) {
        constructMethodTreeNode(node, manager, "", &sMgrTextVtable, &sMgrNodeVtable, &sMgrNodeDisposerVtable,
                                &sMgrDelegateBaseVtable, &sMgrNodeDelegateVtable);
    }
    manager->mSystemScreen = cScreenBottom;
    manager->mAppScreen = cScreenTop;
    manager->mRootCalcNode.mName.mText.v = gabi::guest_str("sead::RootCalc");
    manager->mSysCalcNode.mName.mText.v = gabi::guest_str("sead::SysCalc");
    manager->mAppCalcNode.mName.mText.v = gabi::guest_str("sead::AppCalc");
    manager->mTopRootDrawNode.mName.mText.v = gabi::guest_str("sead::TopRootDraw");
    manager->mTopSysDrawNode.mName.mText.v = gabi::guest_str("sead::TopSysDraw");
    manager->mTopAppDrawNode.mName.mText.v = gabi::guest_str("sead::TopAppDraw");
    manager->mTopAppDrawFinalNode.mName.mText.v = gabi::guest_str("sead::TopAppDrawFinal");
    manager->mBtmRootDrawNode.mName.mText.v = gabi::guest_str("sead::BtmRootDraw");
    manager->mBtmSysDrawNode.mName.mText.v = gabi::guest_str("sead::BtmSysDraw");
    manager->mBtmAppDrawNode.mName.mText.v = gabi::guest_str("sead::BtmAppDraw");
    manager->mBtmAppDrawFinalNode.mName.mText.v = gabi::guest_str("sead::BtmAppDrawFinal");
    manager->mRootCalcNode.pushBackChild(&manager->mSysCalcNode);
    manager->mRootCalcNode.pushBackChild(&manager->mAppCalcNode);
    manager->mTopRootDrawNode.pushBackChild(&manager->mTopAppDrawNode);
    manager->mTopRootDrawNode.pushBackChild(&manager->mTopAppDrawFinalNode);
    manager->mTopRootDrawNode.pushBackChild(&manager->mTopSysDrawNode);
    manager->mBtmRootDrawNode.pushBackChild(&manager->mBtmAppDrawNode);
    manager->mBtmRootDrawNode.pushBackChild(&manager->mBtmAppDrawFinalNode);
    manager->mBtmRootDrawNode.pushBackChild(&manager->mBtmSysDrawNode);
    setPauseFlags(&manager->mSysCalcNode, cPauseNone);
    setPauseFlags(&manager->mAppCalcNode, cPauseNone);
    setPauseFlags(&manager->mTopSysDrawNode, cPauseNone);
    setPauseFlags(&manager->mTopAppDrawNode, cPauseNone);
    setPauseFlags(&manager->mTopAppDrawFinalNode, cPauseNone);
    setPauseFlags(&manager->mBtmSysDrawNode, cPauseNone);
    setPauseFlags(&manager->mBtmAppDrawNode, cPauseNone);
    setPauseFlags(&manager->mBtmAppDrawFinalNode, cPauseNone);
    setPauseFlags(&manager->mRootCalcNode, cPauseSelfAndChild);
    setPauseFlags(&manager->mTopRootDrawNode, cPauseSelfAndChild);
    setPauseFlags(&manager->mBtmRootDrawNode, cPauseSelfAndChild);
    return manager;
}

/* 027464F8 */
void sead::MethodTreeMgr::attachMethod(u32 phase, MethodTreeNode_l* node) {
    switch (phase) {
    case cCalcSys:
        mSysCalcNode.pushBackChild(node);
        return;
    case cCalcApp:
        mAppCalcNode.pushBackChild(node);
        return;
    case cDrawSys:
        if ((u32)mSystemScreen == cScreenTop) mTopSysDrawNode.pushFrontChild(node);
        else if ((u32)mSystemScreen == cScreenBottom) mBtmSysDrawNode.pushFrontChild(node);
        return;
    case cDrawApp:
        if ((u32)mAppScreen == cScreenTop) mTopAppDrawNode.pushFrontChild(node);
        else if ((u32)mAppScreen == cScreenBottom) mBtmAppDrawNode.pushFrontChild(node);
        return;
    case cDrawAppFinal:
        if ((u32)mAppScreen == cScreenTop) mTopAppDrawFinalNode.pushFrontChild(node);
        else if ((u32)mAppScreen == cScreenBottom) mBtmAppDrawFinalNode.pushFrontChild(node);
        return;
    case cDrawTopSys:
        mTopSysDrawNode.pushBackChild(node);
        return;
    case cDrawTopApp:
        mTopAppDrawNode.pushBackChild(node);
        return;
    case cDrawTopAppFinal:
        mTopAppDrawFinalNode.pushBackChild(node);
        return;
    case cDrawBtmSys:
        mBtmSysDrawNode.pushBackChild(node);
        return;
    case cDrawBtmApp:
        mBtmAppDrawNode.pushBackChild(node);
        return;
    case cDrawBtmAppFinal:
        mBtmAppDrawFinalNode.pushBackChild(node);
        return;
    }
}

/* 027465F8 */
void sead::MethodTreeMgr::pauseAll(u32 pause) {
    u32 flags = pause ? cPauseSelfAndChild : cPauseNone;
    setPauseFlags(&mRootCalcNode, flags);
    setPauseFlags(&mTopRootDrawNode, flags);
    setPauseFlags(&mBtmRootDrawNode, flags);
}

/* 02746790 */
void sead::MethodTreeMgr::calc() {
    // HD: the original tail-branches to call() without a frame: it runs at the caller's sp with the
    // caller's LR (frame-exact main-loop chain for cross-build save states).
    gabi::AutoFrame frame{gabi::current_function()};
    mRootCalcNode.call();
}

/* 02746798 */
void sead::MethodTreeMgr::drawTop() { mTopRootDrawNode.call(); }

/* 027467A0 */
void sead::MethodTreeMgr::drawBottom() { mBtmRootDrawNode.call(); }

/* 027467A8 */
void sead::method_tree_mgr_instances::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027467BC */
void sead::method_tree_mgr_instances::MethodTreeNode::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sMgrNodeVtable;
    mDisposer.mVtable = &sMgrNodeDisposerVtable;
    detachAll();
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02746830 */
void sead::method_tree_mgr_instances::Text::assureTerminationImpl() {}

/* 02746834 */
void sead::method_tree_mgr_instances::NodeDelegate::invoke() {
    u8* object = mObject;
    if (object == NULL) return;
    s16 index = mMethod.mVirtualIndex;
    if (index == 0) return;
    u8* adjusted = object + (s32)mMethod.mObjectAdjustment;
    u32 target;
    if (index < 0) {
        target = mMethod.mTarget;
    } else {
        s16 vtableOffset = (u32)mMethod.mTarget;
        VirtualEntry* vtable = *reinterpret_cast<gptr<VirtualEntry>*>(adjusted + vtableOffset);
        target = vtable[index].mTarget;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02746888 */
static u32 isKindOfMethodTreeMgr(MethodTreeMgr* manager, const RuntimeType* type) {
    if (sMethodTreeMgrTypeGuard == 0) {
        sMethodTreeMgrTypeGuard = 1;
        sMethodTreeMgrType.mTable = &sMgrMethodTreeMgrTypeTable;
    }
    if (type == &sMethodTreeMgrType) return 1;
    if (sMgrBaseTypeGuard == 0) {
        sMgrBaseTypeGuard = 1;
        sMgrBaseType.mTable = &sMgrMgrBaseTypeTable;
    }
    return type == &sMgrBaseType;
}

/* ================================================================ calc+draw task */

/* 02746900 */
static void destroyCalcDrawTaskNodeFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<calc_draw_task_instances::MethodTreeNode*>(reinterpret_cast<u8*>(disposer) - 0x1C)->destroy(flags);
}

/* 02746908 */
static void destroyCalcDrawTaskFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<CalcDrawTask*>(reinterpret_cast<u8*>(disposer) - 0x14)->destroy(flags);
}

/* 02746910 */
CalcDrawTask* sead::CalcDrawTask::construct(CalcDrawTask* task, void* parameters, const be<u8>* name) {
    task = memberStorage(task);
    if (task == NULL) return NULL;
    constructTaskBase(&task->mTaskBase, parameters, name);
    task->mTaskBase.mVtable = &sCalcDrawTaskVtable;
    task->mTaskBase.mDisposer.mVtable = &sCalcDrawTaskDisposerVtable;
    constructMethodTreeNode(&task->mCalcEntry, NULL, "", &sCalcDrawTaskTextVtable, &sCalcDrawTaskNodeVtable,
                            &sCalcDrawTaskNodeDisposerVtable, &sCalcDrawTaskDelegateBaseVtable,
                            &sCalcDrawTaskNodeDelegateVtable);
    constructMethodTreeNode(&task->mTopDrawEntry, NULL, "", &sCalcDrawTaskTextVtable, &sCalcDrawTaskNodeVtable,
                            &sCalcDrawTaskNodeDisposerVtable, &sCalcDrawTaskDelegateBaseVtable,
                            &sCalcDrawTaskNodeDelegateVtable);
    constructMethodTreeNode(&task->mBottomDrawEntry, NULL, "", &sCalcDrawTaskTextVtable, &sCalcDrawTaskNodeVtable,
                            &sCalcDrawTaskNodeDisposerVtable, &sCalcDrawTaskDelegateBaseVtable,
                            &sCalcDrawTaskNodeDelegateVtable);
    setPauseFlags(&task->mCalcEntry, cPauseSelfAndChild);
    setPauseFlags(&task->mTopDrawEntry, cPauseSelfAndChild);
    setPauseFlags(&task->mBottomDrawEntry, cPauseSelfAndChild);
    bindTaskMethod(&task->mCalcEntry, task, &sCalcDrawTaskTaskDelegateVtable, kCalcSlot, name);
    bindTaskMethod(&task->mTopDrawEntry, task, &sCalcDrawTaskTaskDelegateVtable, kDrawTopSlot, name);
    bindTaskMethod(&task->mBottomDrawEntry, task, &sCalcDrawTaskTaskDelegateVtable, kDrawBottomSlot, name);
    return task;
}

/* 02746D9C */
void sead::CalcDrawTask::pauseCalc(u32 pause) { setPauseFlags(&mCalcEntry, pause ? cPauseSelf : cPauseNone); }

/* 02746E48 */
void sead::CalcDrawTask::pauseDraw(u32 pause) {
    setPauseFlags(&mTopDrawEntry, pause ? cPauseSelf : cPauseNone);
    setPauseFlags(&mBottomDrawEntry, pause ? cPauseSelf : cPauseNone);
}

/* 02746F6C */
void sead::CalcDrawTask::attachCalc() {
    MethodTreeMgr* manager = mTaskBase.getMethodTreeMgr();
    lockCriticalSection(manager);
    TreeNode_l* parentNode = mTaskBase.mTreeNode.mParent;
    u32 attachToParent = mTaskBase.mAttachToParent;
    TaskBase_l* parent = parentNode != NULL ? static_cast<TaskBase_l*>(parentNode->mData.get()) : NULL;
    if (attachToParent != 0) {
        if (parent != NULL) {
            parent->getMethodTreeNode(cCalcApp)->pushBackChild(&mCalcEntry);
        } else {
            mTaskBase.attachToMethodTree(cCalcApp, &mCalcEntry);
        }
    } else {
        mTaskBase.attachToMethodTree(cCalcSys, &mCalcEntry);
    }
    unlockCriticalSection(manager);
}

/* 02747024 */
void sead::CalcDrawTask::attachDraw() {
    MethodTreeMgr* manager = mTaskBase.getMethodTreeMgr();
    lockCriticalSection(manager);
    TreeNode_l* parentNode = mTaskBase.mTreeNode.mParent;
    u32 attachToParent = mTaskBase.mAttachToParent;
    TaskBase_l* parent = parentNode != NULL ? static_cast<TaskBase_l*>(parentNode->mData.get()) : NULL;
    if (attachToParent != 0) {
        if (parent != NULL) {
            parent->getMethodTreeNode(cDrawTopApp)->pushFrontChild(&mTopDrawEntry);
            parent->getMethodTreeNode(cDrawBtmApp)->pushFrontChild(&mBottomDrawEntry);
        } else {
            mTaskBase.attachToMethodTree(cDrawTopApp, &mTopDrawEntry);
            mTaskBase.attachToMethodTree(cDrawBtmApp, &mBottomDrawEntry);
        }
    } else {
        mTaskBase.attachToMethodTree(cDrawTopSys, &mTopDrawEntry);
        mTaskBase.attachToMethodTree(cDrawBtmSys, &mBottomDrawEntry);
    }
    unlockCriticalSection(manager);
}

/* 02747128 */
void sead::CalcDrawTask::detachCalc() { mCalcEntry.detachAll(); }

/* 02747130 */
void sead::CalcDrawTask::detachDraw() {
    mTopDrawEntry.detachAll();
    mBottomDrawEntry.detachAll();
}

/* 02747168 */
MethodTreeNode_l* sead::CalcDrawTask::getMethodTreeNode(u32 phase) {
    if (phase <= cCalcApp) return &mCalcEntry;
    bool sys = phase == cDrawSys;
    if (sys || phase <= cDrawAppFinal) {
        // the screen the manager draws the system or the application on
        MethodTreeMgr* manager = mTaskBase.getMethodTreeMgr();
        if (sMethodTreeMgrTypeGuard == 0) {
            sMethodTreeMgrType.mTable = &sCalcDrawTaskMethodTreeMgrTypeTable;
            sMethodTreeMgrTypeGuard = 1;
        }
        if (manager != NULL) {
            if (!gabi::call_ptr<u32>(manager->mBase.mVtable->mIsKindOf.mFunction, manager, &sMethodTreeMgrType)) manager = NULL;
        }
        u32 screen = sys ? (u32)manager->mSystemScreen : (u32)manager->mAppScreen;
        return screen == cScreenTop ? &mTopDrawEntry : &mBottomDrawEntry;
    }
    if (phase <= cDrawTopAppFinal) return &mTopDrawEntry;
    if (phase <= cDrawBtmAppFinal) return &mBottomDrawEntry;
    return NULL;
}

/* 027472C4 */
void sead::calc_draw_task_instances::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 027472D8 */
void sead::calc_draw_task_instances::MethodTreeNode::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sCalcDrawTaskNodeVtable;
    mDisposer.mVtable = &sCalcDrawTaskNodeDisposerVtable;
    detachAll();
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274734C */
void sead::CalcDrawTask::destroy(u32 flags) {
    if (this == NULL) return;
    mTaskBase.mDisposer.mVtable = &sCalcDrawTaskDisposerVtable;
    mBottomDrawEntry.mDisposer.mVtable = &sCalcDrawTaskNodeDisposerVtable;
    mBottomDrawEntry.mVtable = &sCalcDrawTaskNodeVtable;
    mBottomDrawEntry.detachAll();
    destroyDisposer(&mBottomDrawEntry.mDisposer, 0);
    mTopDrawEntry.mVtable = &sCalcDrawTaskNodeVtable;
    mTopDrawEntry.mDisposer.mVtable = &sCalcDrawTaskNodeDisposerVtable;
    mTopDrawEntry.detachAll();
    destroyDisposer(&mTopDrawEntry.mDisposer, 0);
    mCalcEntry.mVtable = &sCalcDrawTaskNodeVtable;
    mCalcEntry.mDisposer.mVtable = &sCalcDrawTaskNodeDisposerVtable;
    mCalcEntry.detachAll();
    destroyDisposer(&mCalcEntry.mDisposer, 0);
    destroyTaskBase(&mTaskBase, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 02747420 */
void sead::calc_draw_task_instances::Text::assureTerminationImpl() {}

/* 02747424 */
void sead::calc_draw_task_instances::NodeDelegate::invoke() {
    u8* object = mObject;
    if (object == NULL) return;
    s16 index = mMethod.mVirtualIndex;
    if (index == 0) return;
    u8* adjusted = object + (s32)mMethod.mObjectAdjustment;
    u32 target;
    if (index < 0) {
        target = mMethod.mTarget;
    } else {
        s16 vtableOffset = (u32)mMethod.mTarget;
        VirtualEntry* vtable = *reinterpret_cast<gptr<VirtualEntry>*>(adjusted + vtableOffset);
        target = vtable[index].mTarget;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* 02747478 */
static u32 isKindOfCalcDrawTask(CalcDrawTask* task, const RuntimeType* type) {
    if (sCalcDrawTaskTypeGuard == 0) {
        sCalcDrawTaskTypeGuard = 1;
        sCalcDrawTaskType.mTable = &sCalcDrawTaskTypeTable;
    }
    if (type == &sCalcDrawTaskType) return 1;
    if (sTaskBaseTypeGuard == 0) {
        sTaskBaseTypeGuard = 1;
        sTaskBaseType.mTable = &sCalcDrawTaskTaskBaseTypeTable;
    }
    return type == &sTaskBaseType;
}

/* 027474F0 */
void sead::CalcDrawTask::calc() {}

/* 027474F4 */
void sead::CalcDrawTask::drawTop() {}

/* 027474F8 */
void sead::CalcDrawTask::drawBottom() {}

/* 027474FC */
void sead::calc_draw_task_instances::TaskDelegate::invoke() {
    u8* object = mObject;
    if (object == NULL) return;
    s16 index = mMethod.mVirtualIndex;
    if (index == 0) return;
    u8* adjusted = object + (s32)mMethod.mObjectAdjustment;
    u32 target;
    if (index < 0) {
        target = mMethod.mTarget;
    } else {
        s16 vtableOffset = (u32)mMethod.mTarget;
        VirtualEntry* vtable = *reinterpret_cast<gptr<VirtualEntry>*>(adjusted + vtableOffset);
        target = vtable[index].mTarget;
    }
    gabi::tail_ptr<void>(target, adjusted);
}

/* ================================================================ framework */

/* 02747550 */
static void destroyFrameworkNodeFromDisposer(Disposer* disposer, u32 flags) {
    destroyFrameworkMethodTreeNode(reinterpret_cast<MethodTreeNode_l*>(reinterpret_cast<u8*>(disposer) - 0x1C), flags);
}

/* 02747558 */
Framework* sead::Framework::construct(Framework* framework) {
    framework = memberStorage(framework);
    if (framework == NULL) return NULL;
    framework->mReady = 0;
    framework->mHeap = NULL;
    framework->mVtable = &sFrameworkVtable;
    ObjectList_l* list = memberStorage(&framework->mObjectList);
    if (list != NULL) {
        list->mCount = 0;
        list->mPrevious = list;
        list->mVtable = &sObjectListVtable;
        list->mNext = list;
    }
    framework->mMethodTreeMgr = NULL;
    framework->mTaskMgr = NULL;
    framework->mMethodTreeMgrHeap = NULL;
    return framework;
}

/* 027475F4 */
void sead::Framework::initialize(FrameworkInitArgs* arg) {
    void* rootHeap = arg->mRootHeap;
    if (rootHeap != NULL) initHeapMgrFromRootHeap(rootHeap);
    else initHeapMgrFromArena(arg->mArena);
    void* parent = NULL;
    if (sRootHeaps.mCount != 0) parent = *static_cast<gptr<void>*>(sRootHeaps.mStorage.get());
    gabi::Local<SafeText> name;
    name->mVtable = &sFrameworkTextVtable;
    name->mText.v = gabi::guest_str("sead::ThreadMgr");
    RegistryHeap* heap = FreeRegionHeap::create(0, name, static_cast<RegistryHeap*>(parent), 1, 0);
    createThreadMgr(heap);
    initThreadMgr(sThreadMgr, heap);
    gabi::call_ptr<void>(heap->dispatch()->mTrimToParent.mFunction, heap);
    GlobalRandom::createInstance(static_cast<RegistryHeap*>(parent));
    finishFrameworkInitialize();
}

/* 027476D8 */
void sead::Framework::run(void* heap, void* parameters, const FrameworkRunSettings* settings) {
    // HD: frame-exact for cross-build save states: the main thread runs the game below the run-loop
    // call, so the original's frame (r27..r31 saved, LR), its two stack objects at their offsets and,
    // at that call, its return address (one per branch) and r27..r31 are replayed.
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::call_ptr<void>(mVtable->mPrepareRun.mFunction, this, heap);
    gabi::FrameLocal<SafeText> name(0x20);
    name->mVtable = &sFrameworkTextVtable;
    name->mText.v = gabi::guest_str("sead::MethodTreeMgr");
    RegistryHeap* treeHeap = FreeRegionHeap::create(0, name, static_cast<RegistryHeap*>(heap), 1, 0);
    mMethodTreeMgr = gabi::call_ptr<MethodTreeMgr*>(mVtable->mCreateMethodTreeMgr.mFunction, this, treeHeap);
    gabi::call_ptr<void>(treeHeap->dispatch()->mTrimToParent.mFunction, treeHeap);
    mMethodTreeMgrHeap = treeHeap;
    u32 stackSize = settings->mStackSize;
    s32 priority = settings->mPriority;
    gabi::FrameLocal<TaskMgrInitArgs> taskMgrArg(8);
    taskMgrArg->mHeap = heap;
    taskMgrArg->mFramework = this;
    taskMgrArg->mContextCount = kTaskMgrContextCount;
    taskMgrArg->mParameters = parameters;
    taskMgrArg->mStackSize = stackSize != 0 ? stackSize : kDefaultTaskMgrStackSize;
    taskMgrArg->mPriority = priority;
    mTaskMgr = createTaskMgr(taskMgrArg);
    gabi::call_ptr_site(priority == -1 ? 0x02747804 : 0x027477D0,
                        {{27, gabi::ea(treeHeap)}, {28, gabi::ea(this)}, {29, gabi::ea(heap)}, {30, gabi::ea(parameters)},
                         {31, gabi::ea(settings)}},
                        {}, mVtable->mRunLoop.mFunction, this);
}

/* WWHD glue
 ext 0273AD10 void* allocateObject(u32)
 ext 0273AF40 void releaseObject(void*)
 ext 02752A84 void constructDisposer(sead::Disposer*)
 ext 02752BEC void destroyDisposer(sead::Disposer*, u32)
 ext 02767360 void constructTreeNode(sead::TreeNode_l*)
 ext 02748980 void constructTaskBase(sead::TaskBase_l*, void*, const be<u8>*)
 ext 027485F0 void destroyTaskBase(sead::TaskBase_l*, u32)
 ext 02747CA8 void constructMethodTreeMgrBase(sead::MethodTreeMgrBase_l*)
 ext 027601BC void lockCriticalSection(void*)
 ext 027601F0 void unlockCriticalSection(void*)
 ext 02747A64 void sead::MethodTreeNode_l::lock()
 ext 02747A74 void sead::MethodTreeNode_l::unlock()
 ext 02747A84 void sead::MethodTreeNode_l::detachAll()
 ext 02747AE4 void sead::MethodTreeNode_l::pushBackChild(sead::MethodTreeNode_l*)
 ext 02747B60 void sead::MethodTreeNode_l::pushFrontChild(sead::MethodTreeNode_l*)
 ext 02747C6C void sead::MethodTreeNode_l::call()
 ext 02748794 sead::MethodTreeMgr* sead::TaskBase_l::getMethodTreeMgr()
 ext 027487A4 void sead::TaskBase_l::attachToMethodTree(u32, sead::MethodTreeNode_l*)
 ext 02747960 void destroyFrameworkMethodTreeNode(sead::MethodTreeNode_l*, u32)
 ext 02755FE0 void initHeapMgrFromRootHeap(void*)
 ext 02755F80 void initHeapMgrFromArena(void*)
 ext 02753004 sead::RegistryHeap* sead::FreeRegionHeap::create(u32, const sead::SafeText*, sead::RegistryHeap*, s32, u32)
 ext 027604BC void createThreadMgr(void*)
 ext 0275FEF8 void initThreadMgr(void*, void*)
 ext 0275CD20 sead::GlobalRandom* sead::GlobalRandom::createInstance(sead::RegistryHeap*)
 ext 02757B48 void finishFrameworkInitialize()
 ext 027496EC void* createTaskMgr(sead::TaskMgrInitArgs*)
 data sCalcTaskVtable 101444A4 nocheck
 data sCalcTaskDisposerVtable 1014455C nocheck
 data sCalcTaskTextVtable 101443F0 nocheck
 data sCalcTaskNodeVtable 10144430 nocheck
 data sCalcTaskNodeDisposerVtable 10144450 nocheck
 data sCalcTaskDelegateBaseVtable 10144408 nocheck
 data sCalcTaskNodeDelegateVtable 10144460 nocheck
 data sCalcTaskTaskDelegateVtable 10144488 nocheck
 data sCalcTaskTypeGuard 101FD598
 data sCalcTaskType 101FD97C
 data sCalcTaskTypeTable 10144478
 data sTaskBaseTypeGuard 101FD594
 data sTaskBaseType 101FD980
 data sCalcTaskTaskBaseTypeTable 10144420
 data sMethodTreeMgrVtable 101446E0 nocheck
 data sMgrTextVtable 1014457C nocheck
 data sMgrNodeVtable 101445AC nocheck
 data sMgrNodeDisposerVtable 101445CC nocheck
 data sMgrDelegateBaseVtable 10144594 nocheck
 data sMgrNodeDelegateVtable 101445DC nocheck
 data sMgrMethodTreeMgrTypeTable 101445F4
 data sMgrBaseTypeGuard 101FD714
 data sMgrBaseType 101FD9A8
 data sMgrMgrBaseTypeTable 1014456C
 data sCalcDrawTaskVtable 101447E4 nocheck
 data sCalcDrawTaskDisposerVtable 101448AC nocheck
 data sCalcDrawTaskTextVtable 10144720 nocheck
 data sCalcDrawTaskNodeVtable 10144760 nocheck
 data sCalcDrawTaskNodeDisposerVtable 10144780 nocheck
 data sCalcDrawTaskDelegateBaseVtable 10144738 nocheck
 data sCalcDrawTaskNodeDelegateVtable 10144790 nocheck
 data sCalcDrawTaskTaskDelegateVtable 101447C8 nocheck
 data sCalcDrawTaskTypeGuard 101FD71C
 data sCalcDrawTaskType 101FD9D8
 data sCalcDrawTaskTypeTable 101447A8
 data sCalcDrawTaskTaskBaseTypeTable 10144750
 data sCalcDrawTaskMethodTreeMgrTypeTable 101447B8
 data sFrameworkVtable 10144958 nocheck
 data sObjectListVtable 10144924 nocheck
 data sFrameworkTextVtable 101448BC nocheck
 data sRootHeaps 104A030C nocheck
 data sThreadMgr 101F8B94 nocheck
 str 101444A0 "" in 027454E0
 str 10144604 "" in 02745894
 str 101447E0 "" in 02746910
 str 10144680 "sead::RootCalc"
 str 10144608 "sead::SysCalc"
 str 10144618 "sead::AppCalc"
 str 10144628 "sead::TopRootDraw"
 str 10144690 "sead::TopSysDraw"
 str 101446A4 "sead::TopAppDraw"
 str 1014463C "sead::TopAppDrawFinal"
 str 10144654 "sead::BtmRootDraw"
 str 101446B8 "sead::BtmSysDraw"
 str 101446CC "sead::BtmAppDraw"
 str 10144668 "sead::BtmAppDrawFinal"
 str 10144934 "sead::ThreadMgr"
 str 10144944 "sead::MethodTreeMgr"
*/

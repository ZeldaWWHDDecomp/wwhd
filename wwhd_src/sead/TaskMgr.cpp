/* WWHD sead task manager: construction (its heaps, the prepare thread, the null fader and the
 * create-request pool), queued and synchronous task creation, the task states and lists, the
 * per-frame create and destruction passes, recursive destruction and finalization; the TU's copies
 * of the text, method node and delegates. Reconstructed from this game's instructions. */

#include "sead/TaskMgr.h"
#include "sead/ResourceMgr.h"

using namespace sead;

void* allocateObject(u32 size);
void releaseObject(void* object);
void constructDisposer(sead::Disposer* disposer);
void destroyDisposer(sead::Disposer* disposer, u32 flags);
void constructCriticalSection(sead::CriticalSection_l* section);
void* allocateFromHeap(u32 size, sead::RegistryHeap* heap, s32 alignment);
void* allocateArrayFromHeap(u32 size, sead::RegistryHeap* heap, s32 alignment);
u32 setCurrentHeap(sead::RegistryManagerState* manager, sead::RegistryHeap* heap);
sead::DelegateThread_l* constructDelegateThread(sead::DelegateThread_l* thread, const sead::SafeText* name,
                                                sead::MethodDelegate* delegate, sead::RegistryHeap* heap,
                                                s32 priority, u32 messageQueueKind, u32 blockMessage,
                                                u32 stackSize, u32 messageQueueSize);
void yieldThread();
sead::TaskBase* constructNullFader(sead::TaskBase* fader, const sead::TaskConstructArg* arg);
u32 faderRequestCreateTask(sead::TaskBase* fader, const sead::TaskCreateArg* arg);
sead::TaskBase* createTaskByClass(const sead::TaskClassID* classID, const sead::TaskConstructArg* arg);

static constexpr u32 kDestroyRelease = 1;
static constexpr u32 kHeapAlignment = 4;
static constexpr u32 kNoHeapSet = 1;                // setCurrentHeap's "nothing switched"
static constexpr s32 kDefaultPrepareThreadPriority = 0x11;
static constexpr u32 kBlockMessage = 0x7FFFFFFF;
static constexpr u32 kPrepareThreadQueueSize = 0x20;
static constexpr u32 kPrepareMessage = 1;           // wake the prepare thread

static void* sTaskMgrVtable;
static SafeTextVtable sTaskMgrTextVtable;
static MethodTreeNodeVtable sTaskMgrNodeVtable;
static DisposerVtable sTaskMgrNodeDisposerVtable;
static DelegateVtable sTaskMgrDelegateBaseVtable;
static DelegateVtable sTaskMgrNodeDelegateVtable;
static DelegateVtable sTaskMgrThreadDelegateVtable;
static u32 sCreateListenerListVtable;
static PointerBuffer sRootHeaps;                    // the heap manager's root heaps
static ResourceMgr* sResourceMgr;                  // post-creates the loaded resources when a task starts running

/* the storage of a member object: its own address, or a new block when that address is NULL
 * (GHS member constructors allocate when their object pointer is NULL) */
template <class T>
static T* memberStorage(T* member) {
    if (gabi::ea(member) == 0) member = static_cast<T*>(allocateObject(sizeof(T)));
    return member;
}

/* ---- inlined helpers ---- */

static void constructList(LinkList* list) {
    list = memberStorage(list);
    if (list != NULL) {
        list->mNext = list;
        list->mCount = 0;
        list->mPrevious = list;
    }
}

/* unlink every task from the list (the tasks stay alive) */
static void clearTaskList(LinkList* list) {
    Link* node = list->mNext;
    while (node != list) {
        TaskListNode* entry = static_cast<TaskListNode*>(node);
        LinkList* owner = entry->mList;
        node = node->mNext;
        if (owner != NULL) {
            entry->mList = NULL;
            entry->detach();
            list->mCount = list->mCount - 1;
        }
    }
}

/* remove the listener from the list it listens on */
static void eraseListener(CreateListener* listener) {
    LinkList* list = listener->mList;
    if (list != NULL) {
        listener->mList = NULL;
        listener->mLink.detach();
        list->mCount = list->mCount - 1;
    }
}

static void attachListener(CreateListenerList* list, CreateListener* listener) {
    if (listener->mListening != 0) {
        eraseListener(listener);
        listener->mListening = 0;
    }
    eraseListener(listener);
    listener->mList = list;
    list->insertBefore(&listener->mLink);
    list->mCount = list->mCount + 1;
    listener->mListening = 1;
}

static void notifyListeners(CreateListenerList* list, TaskBase* task) {
    Link* node = list->mNext;
    while (node != list) {
        Link* next = node->mNext;
        CreateDelegate* delegate = static_cast<CreateListenerLink*>(node)->mListener->mDelegate;
        if (delegate != NULL) gabi::call_ptr<void>(delegate->mVtable->mInvoke.mFunction, delegate, task);
        node = next;
    }
}

/* the list's destructor: every listener stops listening */
static void releaseListeners(CreateListenerList* list) {
    if (list == NULL) return;
    Link* node = list->mNext;
    list->mVtable = &sCreateListenerListVtable;
    while (node != list) {
        CreateListener* listener = static_cast<CreateListenerLink*>(node)->mListener;
        node = node->mNext;
        if (listener->mListening != 0) {
            eraseListener(listener);
            listener->mListening = 0;
        }
    }
}

static void constructListenerList(CreateListenerList* list) {
    list = memberStorage(list);
    if (list != NULL) {
        list->mCount = 0;
        list->mPrevious = list;
        list->mVtable = &sCreateListenerListVtable;
        list->mNext = list;
    }
}

/* the create argument is copied word by word */
static void copyCreateArg(TaskCreateArg* destination, const TaskCreateArg* source) {
    const be<u32>* from = reinterpret_cast<const be<u32>*>(source);
    be<u32>* to = reinterpret_cast<be<u32>*>(destination);
    for (u32 i = 0; i < sizeof(TaskCreateArg) / 4; i++) to[i] = from[i];
}

/* the heaps of a new task carry its name */
static void nameTaskHeaps(TaskBase* task) {
    s32 count = sRootHeaps.mCount;
    for (s32 i = 0; i < count; i++) {
        RegistryHeap* heap = task->mHeapArray.mHeaps[i];
        if (heap != NULL) heap->mName = task->mName.mText;
    }
}

/* after a task's prepare: shrink the heaps its policies ask for */
static void adjustTaskHeaps(TaskBase* task, const HeapPolicy* policies) {
    for (s32 i = 0; i < sRootHeaps.mCount; i++) {
        const HeapPolicy* policy = (u32)i < (u32)sRootHeaps.mCount ? &policies[i] : &policies[0];
        if (policy->mAdjust != 0) task->adjustHeap(i, policy->mAdjustReserve);
    }
}

/* ================================================================ task manager */

/* 02748E14 */
static void destroyTaskMgrNodeFromDisposer(Disposer* disposer, u32 flags) {
    reinterpret_cast<task_mgr_instances::MethodTreeNode*>(reinterpret_cast<u8*>(disposer) - offsetof(MethodTreeNode, mDisposer))
        ->destroy(flags);
}

/* 02748E1C */
void sead::TaskMgr::setupWorkers() {
    gabi::AutoFrame frame{gabi::current_function()};
    RegistryHeap* heap = mHeapArray.primaryHeap();
    DelegateThread_l* thread = static_cast<DelegateThread_l*>(allocateFromHeap(sizeof(DelegateThread_l), heap, kHeapAlignment));
    if (thread != NULL) {
        gabi::FrameLocal<SafeText> name(0xC);
        name->mVtable = &sTaskMgrTextVtable;
        name->mText.v = gabi::guest_str("Prepare Thread");
        MethodDelegate* delegate = static_cast<MethodDelegate*>(allocateFromHeap(sizeof(MethodDelegate), heap, kHeapAlignment));
        if (delegate != NULL) {
            delegate->mObject = reinterpret_cast<u8*>(this);
            delegate->mMethod.mObjectAdjustment = 0;
            delegate->mMethod.mTarget = gabi::guest_fn(reinterpret_cast<const void*>(&TaskMgr::prepareThreadMain));
            delegate->mMethod.mVirtualIndex = -1;
            delegate->mVtable = &sTaskMgrThreadDelegateVtable;
        }
        s32 priority = mInitArgs.mPriority;
        if (priority == -1) priority = kDefaultPrepareThreadPriority;
        thread = constructDelegateThread(thread, name.get(), delegate, heap, priority, 0, kBlockMessage,
                                         mInitArgs.mStackSize, kPrepareThreadQueueSize);
        mPrepareThread = thread;
    } else {
        mPrepareThread = NULL;
    }
    // the thread is started even when it could not be allocated
    gabi::call_ptr<void>(thread->mVtable->mStart.mFunction, thread);

    // the null fader is a task constructed on the primary heap alone
    gabi::FrameLocal<TaskConstructArg> arg(0x14);
    gabi::FrameLocal<HeapArray> faderHeaps(0x20);
    faderHeaps->mPrimaryIndex = 0;
    for (u32 i = 0; i < kMaxTaskHeaps; i++) faderHeaps->mAdjusted[i] = 0;
    faderHeaps->mHeaps[0] = heap;
    arg->mHeapArray = faderHeaps.get();
    arg->mTaskMgr = this;
    arg->mParameter = NULL;
    TaskBase* fader = static_cast<TaskBase*>(allocateFromHeap(0x200, heap, kHeapAlignment));
    if (fader != NULL) fader = constructNullFader(fader, arg.get());
    mNullFader = fader;
    fader->mName.mText.v = gabi::guest_str("NullFader");
    mNullFader->mState = TaskBase::cRunning;

    CreateRequestPool* pool = static_cast<CreateRequestPool*>(allocateFromHeap(sizeof(CreateRequestPool), heap, kHeapAlignment));
    if (pool != NULL) {
        s32 capacity = mMaxCreateRequests;
        CreateRequestPool* storage = memberStorage(pool);
        if (storage != NULL) {
            storage->mQueue.mCount = 0;
            storage->mStorage = NULL;
            storage->mCapacity = 0;
            storage->mFreeList = NULL;
            storage->mQueue.mPrevious = &storage->mQueue;
            storage->mQueue.mNext = &storage->mQueue;
        }
        if (capacity > 0) {
            u32 bytes = capacity * sizeof(CreateRequest);
            CreateRequest* requests = static_cast<CreateRequest*>(allocateArrayFromHeap(bytes, heap, kHeapAlignment));
            if (requests != NULL) {
                pool->mFreeList = requests;
                for (s32 i = 0; i < capacity - 1; i++) requests[i].mNext = &requests[i + 1];
                // the last request, found with the original's 32-bit size arithmetic
                gabi::at<CreateRequest>(gabi::ea(requests) + bytes - sizeof(CreateRequest))->mNext = NULL;
                pool->mStorage = requests;
                pool->mCapacity = capacity;
            }
        }
    }
    mRequestPool = pool;
}

/* 0274905C */
bool sead::TaskMgr::enqueueCreateRequest(const TaskCreateArg* arg, CreateListener* listener) {
    mCriticalSection.lock();
    CreateRequestPool* pool = mRequestPool;
    if (pool->mQueue.mCount >= pool->mCapacity) {
        mCriticalSection.unlock();
        return false;
    }
    CreateRequest* request = pool->mFreeList;
    if (request != NULL) pool->mFreeList = static_cast<CreateRequest*>(request->mNext.get());
    if (request != NULL) {
        request->mNext = NULL;
        TaskCreateArg::construct(&request->mArg);
        constructListenerList(&request->mListeners);
        request->mQueueLink.mNext = NULL;
        request->mQueueLink.mPrevious = NULL;
    }
    // queued even without a request (the link of a NULL request)
    pool->mQueue.insertBefore(&request->mQueueLink);
    pool->mQueue.mCount = pool->mQueue.mCount + 1;
    if (request == NULL) {
        mCriticalSection.unlock();
        return false;
    }
    copyCreateArg(&request->mArg, arg);
    CreateListener* argListener = arg->mListener;
    if (listener != NULL) attachListener(&request->mListeners, listener);
    if (argListener != NULL) attachListener(&request->mListeners, argListener);
    mCriticalSection.unlock();
    return true;
}

/* 027492C0 */
u32 sead::TaskMgr::requestCreateTask(const TaskCreateArg* arg) {
    CriticalSection_l* section = &mCriticalSection;
    section->lock();
    TaskBase* fader = arg->mFader;
    u32 result;
    if (fader != NULL) {
        result = faderRequestCreateTask(fader, arg);
    } else {
        result = enqueueCreateRequest(arg, NULL);
    }
    section->unlock();
    return result;
}

/* 02749364 */
u32 sead::TaskMgr::requestCreateRootTask() {
    mRootTaskArgs.mFader = NULL;
    mRootTask = NULL;
    mRootTaskArgs.mTag = TaskBase::cApp;
    mRootTaskArgs.mCreatedTask = &mRootTask;
    return requestCreateTask(&mRootTaskArgs);
}

/* 02749388 */
TaskMgr* sead::TaskMgr::construct(TaskMgr* manager, const TaskMgrInitArgs* arg) {
    gabi::AutoFrame frame{gabi::current_function()};
    manager = memberStorage(manager);
    if (manager == NULL) return NULL;
    manager->mVtable = &sTaskMgrVtable;
    constructCriticalSection(&manager->mCriticalSection);
    // the framework is read from the argument before the thread pointer is cleared (027493CC)
    Framework* framework = arg->mFramework;
    manager->mPrepareThread = NULL;
    manager->mFramework = framework;
    manager->mNullFader = NULL;
    constructList(&manager->mQueuedList);
    constructList(&manager->mPreparedList);
    constructList(&manager->mActiveList);
    constructList(&manager->mSleepList);
    constructList(&manager->mDestroyList);
    constructList(&manager->mFinishedList);
    HeapArray* heaps = memberStorage(&manager->mHeapArray);
    if (heaps != NULL) {
        heaps->mPrimaryIndex = 0;
        for (u32 i = 0; i < kMaxTaskHeaps; i++) heaps->mAdjusted[i] = 0;
    }
    manager->mMaxCreateRequests = arg->mContextCount;
    TaskCreateArg::construct(&manager->mRootTaskArgs);
    const be<u32>* from = reinterpret_cast<const be<u32>*>(arg);
    be<u32>* to = reinterpret_cast<be<u32>*>(&manager->mInitArgs);
    for (u32 i = 0; i < sizeof(TaskMgrInitArgs) / 4; i++) to[i] = from[i];

    // the destruction node: an empty name, an unbound delegate, not paused
    MethodTreeNode* node = &manager->mDestructionNode;
    TreeNode* treeNode = memberStorage(&node->mTreeNode);
    if (treeNode != NULL) {
        TreeNode::construct(treeNode);
        treeNode->mData = node;
    }
    SafeText* nodeName = memberStorage(&node->mName);
    if (nodeName != NULL) {
        nodeName->mVtable = &sTaskMgrTextVtable;
        nodeName->mText.v = gabi::guest_str("");
    }
    constructDisposer(&node->mDisposer);
    node->mCriticalSection = NULL;
    node->mVtable = &sTaskMgrNodeVtable;
    node->mDisposer.mVtable = &sTaskMgrNodeDisposerVtable;
    be<u32>* pauseFlags = memberStorage(&node->mPauseFlags);
    if (pauseFlags != NULL) *pauseFlags = MethodTreeNode::cPauseNone;
    node->mPauseListener = NULL;
    node->mUserData = NULL;
    MethodDelegate* delegate = &node->mDelegate;
    if (gabi::ea(delegate) != 0) {
        MethodDelegate* storage = memberStorage(delegate);
        if (storage != NULL) {
            storage->mMethod.mTarget = 0;
            storage->mMethod.mObjectAdjustment = 0;
            storage->mVtable = &sTaskMgrDelegateBaseVtable;
            storage->mMethod.mVirtualIndex = 0;
            storage->mObject = NULL;
        }
        delegate->mVtable = &sTaskMgrNodeDelegateVtable;
    }
    node->mPauseFlags = MethodTreeNode::cPauseNone;
    manager->mRootTask = NULL;

    // one heap below every root heap
    s32 count = sRootHeaps.mCount;
    gabi::FrameLocal<SafeText> heapName(8);
    for (s32 i = 0; i < count; i++) {
        heapName->mVtable = &sTaskMgrTextVtable;
        heapName->mText.v = gabi::guest_str("sead::TaskMgr");
        RegistryHeap* root = NULL;
        if ((u32)i < (u32)sRootHeaps.mCount) root = static_cast<gptr<RegistryHeap>*>(sRootHeaps.mStorage.get())[i];
        manager->mHeapArray.mHeaps[i] = FreeRegionHeap::create(0, heapName.get(), root, 1, 0);
    }
    manager->setupWorkers();
    copyCreateArg(&manager->mRootTaskArgs, static_cast<const TaskCreateArg*>(arg->mParameters.get()));
    manager->requestCreateRootTask();
    return manager;
}

/* 027496EC */
TaskMgr* sead::TaskMgr::create(const TaskMgrInitArgs* arg) {
    TaskMgr* manager = static_cast<TaskMgr*>(allocateFromHeap(sizeof(TaskMgr), static_cast<RegistryHeap*>(arg->mHeap.get()), kHeapAlignment));
    if (manager != NULL) return construct(manager, arg);
    return NULL;
}

/* 02749734 */
void sead::TaskMgr::insertByTag(LinkList* list, TaskBase* task) {
    mCriticalSection.lock();
    task->mListNode.erase();
    Link* node = list->mNext;
    if (node != list) {
        s32 tag = task->mTag;
        do {
            TaskBase* other = static_cast<TaskListNode*>(node)->mTask;
            if (other->mTag < tag) {
                // before the first task with a lower tag
                LinkList* destination = other->mListNode.mList;
                task->mListNode.erase();
                task->mListNode.mList = destination;
                other->mListNode.insertBefore(&task->mListNode);
                destination->mCount = destination->mCount + 1;
                mCriticalSection.unlock();
                return;
            }
            node = node->mNext;
        } while (node != list);
    }
    task->mListNode.erase();
    task->mListNode.mList = list;
    list->insertBefore(&task->mListNode);
    list->mCount = list->mCount + 1;
    mCriticalSection.unlock();
}

/* 02749850 */
bool sead::TaskMgr::changeTaskState(TaskBase* task, u32 state) {
    mCriticalSection.lock();
    u32 previous = task->mState;
    bool changed = false;
    if (previous != state) {
        switch (state) {
        case TaskBase::cPrepare:
            if (previous == TaskBase::cCreated) {
                task->mState = TaskBase::cPrepare;
                insertByTag(&mQueuedList, task);
                DelegateThread_l* thread = mPrepareThread;
                changed = thread == NULL || thread->sendMessage(kPrepareMessage, 1);
            }
            break;
        case TaskBase::cPrepared:
            task->mState = TaskBase::cPrepared;
            task->mListNode.erase();
            changed = true;
            break;
        case TaskBase::cRunning: {
            task->mState = TaskBase::cRunning;
            task->mListNode.erase();
            insertByTag(&mActiveList, task);
            ResourceMgr* resourceMgr = sResourceMgr;
            if (resourceMgr != NULL) resourceMgr->postCreate();
            gabi::call_ptr<void>(task->mVtable->mStart.mFunction, task);
            changed = true;
            break;
        }
        case TaskBase::cPaused:
            task->mState = TaskBase::cPaused;
            changed = true;
            break;
        case TaskBase::cDying:
            if (previous == TaskBase::cRunning) {
                task->mState = TaskBase::cDying;
                gabi::call_ptr<void>(task->mVtable->mDetachCalc.mFunction, task);
                gabi::call_ptr<void>(task->mVtable->mDetachDraw.mFunction, task);
                insertByTag(&mFinishedList, task);
                changed = true;
            }
            break;
        case TaskBase::cDead:
            gabi::call_ptr<void>(task->mVtable->mExit.mFunction, task);
            task->mState = TaskBase::cDead;
            task->mListNode.erase();
            changed = true;
            break;
        }
    }
    mCriticalSection.unlock();
    return changed;
}

/* 02749AB4 */
void sead::TaskMgr::destroyTaskRecursive(TaskBase* task) {
    gabi::AutoFrame frame{gabi::current_function()};
    mCriticalSection.lock();
    TreeNode* child = task->mTreeNode.mChild;
    while (child != NULL) {
        destroyTaskRecursive(child->data<TaskBase>());
        child = task->mTreeNode.mChild;
    }
    if (changeTaskState(task, TaskBase::cDead)) {
        task->mTreeNode.detachAll();
        // the heaps are destroyed from a copy of the task's heap array
        s32 count = sRootHeaps.mCount;
        gabi::FrameLocal<HeapArray> heaps(8);
        heaps->mHeaps[0] = task->mHeapArray.mHeaps[0];
        heaps->mHeaps[3] = task->mHeapArray.mHeaps[3];
        heaps->mPrimaryIndex = task->mHeapArray.mPrimaryIndex;
        gabi::store<u32>(gabi::ea(heaps->mAdjusted), gabi::load<u32>(gabi::ea(task->mHeapArray.mAdjusted)));
        heaps->mHeaps[1] = task->mHeapArray.mHeaps[1];
        heaps->mHeaps[2] = task->mHeapArray.mHeaps[2];
        for (s32 i = 0; i < count; i++) {
            RegistryHeap* heap = heaps->mHeaps[i];
            if (heap != NULL) gabi::call_ptr<void>(heap->dispatch()->mDestroyHeap.mFunction, heap);
        }
    }
    mCriticalSection.unlock();
}

/* 02749BB0 */
void sead::TaskMgr::destroyTask(TaskBase* task) {
    if (methodTreeMgr()->mCriticalSection.tryLock() != 0) {
        destroyTaskRecursive(task);
        methodTreeMgr()->mCriticalSection.unlock();
    }
}

/* 02749C10 */
void sead::TaskMgr::createHeaps(HeapArray* heaps, const TaskCreateArg* arg) {
    gabi::AutoFrame frame{gabi::current_function()};
    heaps->mPrimaryIndex = arg->mPrimaryHeapIndex;
    s32 count = sRootHeaps.mCount;
    CriticalSection_l* section = &mCriticalSection;
    section->lock();
    gabi::FrameLocal<SafeText> sizedName(8);
    gabi::FrameLocal<SafeText> fillName(0x10);
    for (s32 i = 0; i < count; i++) {
        const HeapPolicy* policy = (u32)i < (u32)sRootHeaps.mCount ? &arg->mHeapPolicies[i] : &arg->mHeapPolicies[0];
        RegistryHeap* heap = NULL;
        if (policy->mNoHeap == 0) {
            RegistryHeap* parent = policy->mParent;
            if (parent == NULL) {
                TaskBase* parentTask = arg->mParent;
                if (parentTask == NULL) parent = mHeapArray.mHeaps[i];
                else parent = parentTask->mHeapArray.mHeaps[i];
            }
            if (parent != NULL) {
                s32 direction = policy->mFromEnd != 0 ? -1 : 1;
                u32 leaveFree = policy->mLeaveFree;
                u32 size = policy->mSize;
                if (leaveFree == 0 || size != 0) {
                    sizedName->mVtable = &sTaskMgrTextVtable;
                    sizedName->mText.v = gabi::guest_str("TaskHeap");
                    heap = FreeRegionHeap::create(size, sizedName.get(), parent, direction, 0);
                } else {
                    // all of the parent but what the policy leaves free
                    u32 available = gabi::call_ptr<u32>(parent->dispatch()->mMaxAllocation.mFunction, parent, kHeapAlignment);
                    if (available > policy->mLeaveFree) {
                        fillName->mVtable = &sTaskMgrTextVtable;
                        fillName->mText.v = gabi::guest_str("TaskHeap");
                        heap = FreeRegionHeap::create(available - policy->mLeaveFree, fillName.get(), parent, direction, 0);
                    }
                }
            }
        }
        heaps->mHeaps[i] = heap;
    }
    section->unlock();
}

/* 02749DEC */
TaskBase* sead::TaskMgr::createTaskObject(const TaskCreateArg* arg, HeapArray* heaps) {
    gabi::AutoFrame frame{gabi::current_function()};
    CriticalSection_l* section = &mCriticalSection;
    section->lock();
    gabi::FrameLocal<TaskClassID> classID(8);
    classID->mType = arg->mClassID.mType;
    classID->mValue = arg->mClassID.mValue;
    RegistryHeap* primary = heaps->primaryHeap();
    u32 previous = kNoHeapSet;
    s32 tag = arg->mTag;
    if (primary != NULL) previous = setCurrentHeap(sRegistryManager.get(), primary);
    gabi::FrameLocal<TaskConstructArg> constructArg(0x10);
    constructArg->mHeapArray = heaps;
    constructArg->mParameter = arg->mParameter;
    constructArg->mTaskMgr = this;
    TaskBase* task = createTaskByClass(classID.get(), constructArg.get());
    if (previous != kNoHeapSet) setCurrentHeap(sRegistryManager.get(), gabi::at<RegistryHeap>(previous));
    task->mClassID.mType = classID->mType;
    task->mTag = tag;
    task->mClassID.mValue = classID->mValue;
    TaskBase* parent = arg->mParent;
    if (parent != NULL) parent->mTreeNode.pushBackChild(&task->mTreeNode);
    section->unlock();
    return task;
}

/* 02749ECC */
TaskBase* sead::TaskMgr::createTaskSync(const TaskCreateArg* arg) {
    gabi::AutoFrame frame{gabi::current_function()};
    mCriticalSection.lock();
    gabi::FrameLocal<HeapArray> heaps(0x18);
    heaps->mPrimaryIndex = 0;
    for (u32 i = 0; i < kMaxTaskHeaps; i++) heaps->mAdjusted[i] = 0;
    createHeaps(heaps.get(), arg);
    TaskBase* task = createTaskObject(arg, heaps.get());
    nameTaskHeaps(task);
    u32 callback = arg->mCreateCallback;
    if (callback != 0) gabi::call_ptr<void>(callback, task);
    task->mState = TaskBase::cPrepare;
    RegistryHeap* primary = heaps->primaryHeap();
    u32 previous = kNoHeapSet;
    if (primary != NULL) previous = setCurrentHeap(sRegistryManager.get(), primary);
    gabi::call_ptr<void>(task->mVtable->mPrepare.mFunction, task);
    if (previous != kNoHeapSet) setCurrentHeap(sRegistryManager.get(), gabi::at<RegistryHeap>(previous));
    adjustTaskHeaps(task, arg->mHeapPolicies);
    changeTaskState(task, TaskBase::cPrepared);
    changeTaskState(task, TaskBase::cRunning);
    CreateListener* listener = arg->mListener;
    if (listener != NULL) {
        gabi::FrameLocal<CreateListenerList> listeners(8);
        listeners->mCount = 0;
        listeners->mNext = listeners.get();
        listeners->mVtable = &sCreateListenerListVtable;
        listeners->mPrevious = listeners.get();
        attachListener(listeners.get(), listener);
        notifyListeners(listeners.get(), task);
        releaseListeners(listeners.get());
    }
    gptr<TaskBase>* created = arg->mCreatedTask;
    if (created != NULL) *created = task;
    mCriticalSection.unlock();
    return task;
}

/* 0274A1E4 */
void sead::TaskMgr::processCreateQueue() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mCriticalSection.tryLock() == 0) return;
    CreateRequest* request = mRequestPool->front();
    if (request != NULL) {
        TaskBase* task = request->task();
        if (task == NULL) {
            gabi::FrameLocal<HeapArray> heaps(8);
            heaps->mPrimaryIndex = 0;
            for (u32 i = 0; i < kMaxTaskHeaps; i++) heaps->mAdjusted[i] = 0;
            createHeaps(heaps.get(), &request->mArg);
            task = createTaskObject(&request->mArg, heaps.get());
            nameTaskHeaps(task);
            request->mNext = task;
            changeTaskState(task, TaskBase::cPrepare);
        } else if (task->mState == TaskBase::cPrepared) {
            changeTaskState(task, TaskBase::cRunning);
            gptr<TaskBase>* created = request->mArg.mCreatedTask;
            if (created != NULL) *created = task;
            notifyListeners(&request->mListeners, task);
            CreateRequestPool* pool = mRequestPool;
            request->mQueueLink.detach();
            pool->mQueue.mCount = pool->mQueue.mCount - 1;
            releaseListeners(&request->mListeners);
            request->mNext = pool->mFreeList;
            pool->mFreeList = request;
        }
    }
    mCriticalSection.unlock();
}

/* 0274A414 */
void sead::TaskMgr::beginFrame() { processCreateQueue(); }

/* 0274A418 */
bool sead::TaskMgr::isDestroyable(TaskBase* task) {
    CriticalSection_l* section = &mCriticalSection;
    section->lock();
    u32 flags = task->mFlags;
    if ((flags & TaskBase::cRequestDestroy) != 0 && (flags & TaskBase::cDestroyable) != 0 &&
        task->mState == TaskBase::cRunning) {
        TreeNode* child = task->mTreeNode.mChild;
        while (child != NULL) {
            if (!isDestroyable(child->data<TaskBase>())) {
                section->unlock();
                return false;
            }
            child = task->mTreeNode.mChild;     // the original re-reads the first child
        }
        section->unlock();
        return true;
    }
    section->unlock();
    return false;
}

/* 0274A4E0 */
void sead::TaskMgr::processDestruction() {
    mDestructionNode.call();
    if (mCriticalSection.tryLock() == 0) return;
    Link* node = mActiveList.mNext;
    while (node != &mActiveList) {
        TaskBase* task = static_cast<TaskListNode*>(node)->mTask;
        u32 flags = task->mFlags;
        node = node->mNext;
        if ((flags & TaskBase::cRequestDestroy) != 0 && isDestroyable(task)) changeTaskState(task, TaskBase::cDying);
    }
    node = mFinishedList.mNext;
    while (node != &mFinishedList) {
        TaskBase* task = static_cast<TaskListNode*>(node)->mTask;
        gabi::call_ptr<void>(task->mVtable->mDetachCalc.mFunction, task);
        gabi::call_ptr<void>(task->mVtable->mDetachDraw.mFunction, task);
        task->mTreeNode.detachAll();
        destroyTaskRecursive(task);
        node = mFinishedList.mNext;
    }
    mCriticalSection.unlock();
}

/* 0274A5EC */
void sead::TaskMgr::endFrame() { processDestruction(); }

/* 0274A5F0 */
void sead::TaskMgr::finalize() {
    CriticalSection_l* section = &mCriticalSection;
    section->lock();
    TaskBase* root = mRootTask;
    if (root != NULL) destroyTask(root);
    clearTaskList(&mQueuedList);
    clearTaskList(&mActiveList);
    clearTaskList(&mDestroyList);
    clearTaskList(&mFinishedList);
    for (s32 i = 0; i < sRootHeaps.mCount; i++) {
        RegistryHeap* heap = mHeapArray.mHeaps[i];
        if (heap != NULL) gabi::call_ptr<void>(heap->dispatch()->mReleaseAll.mFunction, heap);
    }
    setupWorkers();
    requestCreateRootTask();
    section->unlock();
}

/* 0274A7A4 */
void sead::TaskMgr::prepareThreadMain(TaskMgr* manager) {
    TaskBase* task = NULL;
    manager->mCriticalSection.lock();
    Link* first = manager->mQueuedList.mNext;
    if (first != &manager->mQueuedList) task = static_cast<TaskListNode*>(first)->mTask;
    manager->mCriticalSection.unlock();
    if (task != NULL) {
        RegistryHeap* primary = task->mHeapArray.primaryHeap();
        u32 previous = kNoHeapSet;
        if (primary != NULL) previous = setCurrentHeap(sRegistryManager.get(), primary);
        gabi::call_ptr<void>(task->mVtable->mPrepare.mFunction, task);
        // the request that created the task
        CreateRequestPool* pool = manager->mRequestPool;
        CreateRequest* request = pool->front();
        while (request != NULL) {
            if (request->task() == task) break;
            Link* next = request->mQueueLink.mNext;
            request = next != &pool->mQueue ? CreateRequest::fromQueueLink(next) : NULL;
        }
        // read through a NULL request too, as the original does
        adjustTaskHeaps(task, gabi::at<HeapPolicy>(gabi::ea(request) + offsetof(CreateRequest, mArg.mHeapPolicies)));
        manager->changeTaskState(task, TaskBase::cPrepared);
        if (previous != kNoHeapSet) setCurrentHeap(sRegistryManager.get(), gabi::at<RegistryHeap>(previous));
    }
    yieldThread();
}

/* 0274A940 */
void sead::task_mgr_instances::Text::destroy(u32 flags) {
    if (this == NULL) return;
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274A954 */
void sead::task_mgr_instances::MethodTreeNode::destroy(u32 flags) {
    if (this == NULL) return;
    mVtable = &sTaskMgrNodeVtable;
    mDisposer.mVtable = &sTaskMgrNodeDisposerVtable;
    detachAll();
    destroyDisposer(&mDisposer, 0);
    if (flags & kDestroyRelease) releaseObject(this);
}

/* 0274A9C8 */
void sead::task_mgr_instances::Text::assureTerminationImpl() {}

/* 0274A9CC */
void sead::task_mgr_instances::NodeDelegate::invoke() {
    u8* object;
    u32 target;
    if (resolve(object, target)) gabi::tail_ptr<void>(target, object);
}

/* 0274AA20 */
void sead::task_mgr_instances::ThreadDelegate::invoke() {
    u8* object;
    u32 target;
    if (resolve(object, target)) gabi::tail_ptr<void>(target, object);
}

/* WWHD glue
 ext 0273B050 void* allocateFromHeap(u32, sead::RegistryHeap*, s32)
 ext 0273B0D4 void* allocateArrayFromHeap(u32, sead::RegistryHeap*, s32)
 ext 027601C4 u32 sead::CriticalSection_l::tryLock()
 ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
 ext 02753004 sead::RegistryHeap* sead::FreeRegionHeap::create(u32, const sead::SafeText*, sead::RegistryHeap*, s32, u32)
 ext 02756170 u32 setCurrentHeap(sead::RegistryManagerState*, sead::RegistryHeap*)
 ext 0275FBD8 sead::DelegateThread_l* constructDelegateThread(sead::DelegateThread_l*, const sead::SafeText*, sead::MethodDelegate*, sead::RegistryHeap*, s32, u32, u32, u32, u32)
 ext 02760C48 void yieldThread()
 ext 02768514 sead::TaskBase* constructNullFader(sead::TaskBase*, const sead::TaskConstructArg*)
 ext 02768400 u32 faderRequestCreateTask(sead::TaskBase*, const sead::TaskCreateArg*)
 ext 02768A14 sead::TaskBase* createTaskByClass(const sead::TaskClassID*, const sead::TaskConstructArg*)
 ext 0275DEEC void sead::ResourceMgr::postCreate()
 data sTaskMgrVtable 10144D48 nocheck
 data sTaskMgrTextVtable 10144CA0 nocheck
 data sTaskMgrNodeVtable 10144CE8 nocheck
 data sTaskMgrNodeDisposerVtable 10144D08 nocheck
 data sTaskMgrDelegateBaseVtable 10144C88 nocheck
 data sTaskMgrNodeDelegateVtable 10144D18 nocheck
 data sTaskMgrThreadDelegateVtable 10144D68 nocheck
 data sCreateListenerListVtable 10144D58 nocheck
 data sRootHeaps 104A030C nocheck
 data sResourceMgr 101F8B68
 str 10144D84 "Prepare Thread"
 str 10144D78 "NullFader"
 str 10144D98 "sead::TaskMgr"
 str 10144DA8 "TaskHeap" in 02749C10
 str 10144D94 "" in 02749388
*/

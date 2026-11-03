/* WWHD sead task manager: creates tasks from create arguments (synchronously, or queued and
 * prepared on its "Prepare Thread"), moves them through their states and lists, attaches them to
 * the framework's method trees and destroys them. Names from the binary's strings ("sead::TaskMgr",
 * "TaskHeap", "Prepare Thread", "NullFader") and the vtables; layouts from this game's constructor
 * (0x1A4 bytes) and instructions. */
#pragma once
#include "sead/TaskBase.h"
#include "sead/MethodTree.h"

namespace sead {

/* ---- local copy of a class owned by another unit (sead threads) ---- */

struct DelegateThreadVtable_l {
    VtableSlot mHeader;
    VtableSlot mDestroy;
    VtableSlot mReserved10;
    VtableSlot mSendMessage;                // 0x18 (message, block) -> bool
    VtableSlot mReserved20;
    VtableSlot mStart;                      // 0x28
};
/* a thread that runs a delegate (constructor 0275FBD8): its vtable is the disposer's */
struct DelegateThread_l {
    u8 mDisposerData[0xC];
    gptr<DelegateThreadVtable_l> mVtable;
    u8 mThreadData[0x88];

    bool sendMessage(u32 message, u32 block) {
        return gabi::call_ptr<u32>(mVtable->mSendMessage.mFunction, this, message, block) != 0;
    }
};

/* ---- create requests ---- */

/* the listeners of a create request */
struct CreateListenerList : LinkList {
    gptr<void> mVtable;
};
/* one queued create request; a free request links the next free one through mNext, a request
 * whose task was created holds the task there */
struct CreateRequest {
    gptr<void> mNext;
    TaskCreateArg mArg;
    CreateListenerList mListeners;
    Link mQueueLink;                        // in the pool's queue

    TaskBase* task() const { return static_cast<TaskBase*>(mNext.get()); }
    static CreateRequest* fromQueueLink(Link* link) {
        return gabi::at<CreateRequest>(gabi::ea(link) - offsetof(CreateRequest, mQueueLink));
    }
};
/* the fixed pool of create requests and the queue of the requests in use */
struct CreateRequestPool {
    LinkList mQueue;
    gptr<CreateRequest> mFreeList;
    gptr<CreateRequest> mStorage;
    be<s32> mCapacity;

    CreateRequest* front() {
        if (mQueue.mCount <= 0) return NULL;
        Link* first = mQueue.mNext;
        return first != NULL ? CreateRequest::fromQueueLink(first) : NULL;
    }
};

/* ---- task manager ---- */

struct TaskMgr {
    gptr<void> mVtable;                     // runtime type only
    CriticalSection_l mCriticalSection;
    gptr<Framework> mFramework;
    gptr<DelegateThread_l> mPrepareThread;
    gptr<TaskBase> mNullFader;
    LinkList mQueuedList;                   // waiting for the prepare thread
    LinkList mPreparedList;
    LinkList mActiveList;                   // running
    LinkList mSleepList;
    LinkList mDestroyList;
    LinkList mFinishedList;                 // dying: waiting to be destroyed
    HeapArray mHeapArray;                   // the manager's own heaps, parents of the tasks' heaps
    gptr<CreateRequestPool> mRequestPool;
    be<s32> mMaxCreateRequests;
    gptr<TaskBase> mRootTask;
    TaskCreateArg mRootTaskArgs;
    TaskMgrInitArgs mInitArgs;
    MethodTreeNode mDestructionNode;        // called at the start of every destruction pass

    static TaskMgr* construct(TaskMgr* manager, const TaskMgrInitArgs* arg);
    static TaskMgr* create(const TaskMgrInitArgs* arg);
    void setupWorkers();
    bool enqueueCreateRequest(const TaskCreateArg* arg, CreateListener* listener);
    u32 requestCreateTask(const TaskCreateArg* arg);    // the fader's or the queue's result
    u32 requestCreateRootTask();
    void insertByTag(LinkList* list, TaskBase* task);
    bool changeTaskState(TaskBase* task, u32 state);
    void destroyTaskRecursive(TaskBase* task);
    void destroyTask(TaskBase* task);
    void createHeaps(HeapArray* heaps, const TaskCreateArg* arg);
    TaskBase* createTaskObject(const TaskCreateArg* arg, HeapArray* heaps);
    TaskBase* createTaskSync(const TaskCreateArg* arg);
    void processCreateQueue();
    void beginFrame();
    bool isDestroyable(TaskBase* task);
    void processDestruction();
    void endFrame();
    void finalize();
    static void prepareThreadMain(TaskMgr* manager);   // the prepare thread's delegate target

    /* the framework's method tree manager, whose critical section guards task destruction */
    MethodTreeMgrBase* methodTreeMgr() const {
        return reinterpret_cast<MethodTreeMgrBase*>(mFramework->mMethodTreeMgr.get());
    }
};

/* per-TU copies of the task manager translation unit */
namespace task_mgr_instances {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct MethodTreeNode : sead::MethodTreeNode { void destroy(u32 flags); };
struct NodeDelegate : MethodDelegate { void invoke(); };
struct ThreadDelegate : MethodDelegate { void invoke(); };
}

}

WWHD_OFFSET(sead::DelegateThreadVtable_l, mSendMessage, 0x18);
WWHD_OFFSET(sead::DelegateThreadVtable_l, mStart, 0x28);
WWHD_OFFSET(sead::DelegateThread_l, mVtable, 0xC);
WWHD_SIZE(sead::DelegateThread_l, 0x98);
WWHD_OFFSET(sead::CreateListenerList, mVtable, 0xC);
WWHD_SIZE(sead::CreateListenerList, 0x10);
WWHD_OFFSET(sead::CreateRequest, mArg, 4);
WWHD_OFFSET(sead::CreateRequest, mListeners, 0x84);
WWHD_OFFSET(sead::CreateRequest, mQueueLink, 0x94);
WWHD_SIZE(sead::CreateRequest, 0x9C);
WWHD_OFFSET(sead::CreateRequestPool, mFreeList, 0xC);
WWHD_OFFSET(sead::CreateRequestPool, mStorage, 0x10);
WWHD_OFFSET(sead::CreateRequestPool, mCapacity, 0x14);
WWHD_SIZE(sead::CreateRequestPool, 0x18);
WWHD_OFFSET(sead::TaskMgr, mCriticalSection, 4);
WWHD_OFFSET(sead::TaskMgr, mFramework, 0x40);
WWHD_OFFSET(sead::TaskMgr, mPrepareThread, 0x44);
WWHD_OFFSET(sead::TaskMgr, mNullFader, 0x48);
WWHD_OFFSET(sead::TaskMgr, mQueuedList, 0x4C);
WWHD_OFFSET(sead::TaskMgr, mActiveList, 0x64);
WWHD_OFFSET(sead::TaskMgr, mDestroyList, 0x7C);
WWHD_OFFSET(sead::TaskMgr, mFinishedList, 0x88);
WWHD_OFFSET(sead::TaskMgr, mHeapArray, 0x94);
WWHD_OFFSET(sead::TaskMgr, mRequestPool, 0xAC);
WWHD_OFFSET(sead::TaskMgr, mMaxCreateRequests, 0xB0);
WWHD_OFFSET(sead::TaskMgr, mRootTask, 0xB4);
WWHD_OFFSET(sead::TaskMgr, mRootTaskArgs, 0xB8);
WWHD_OFFSET(sead::TaskMgr, mInitArgs, 0x138);
WWHD_OFFSET(sead::TaskMgr, mDestructionNode, 0x150);
WWHD_SIZE(sead::TaskMgr, 0x1A4);

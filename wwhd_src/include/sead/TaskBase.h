/* WWHD sead tasks: the task base every task derives from (tree links, disposer, name, the
 * heaps created for it, its state in the task manager and the virtual interface the manager
 * drives), the heap policies and create arguments a task is created from, and the per-TU copies
 * of the task-base translation unit. Layouts from this game's constructors (the task base
 * allocates 0x74 bytes, the create argument 0x80) and instructions. */
#pragma once
#include "sead/MethodTreeNode.h"
#include "sead/HeapRegistry.h"

namespace sead {
/* An event a task is notified of through TaskBase vtable slot 0x70 (word +0x74): an 8-byte record the sender builds
 * on its stack (SEAD#19 02767F70 sends kinds 0, 0, 1, 2, 4); the base implementation ignores it. */
struct TaskEvent {
    be<u32> mKind;
    be<u32> mData;   // the sender's table word
};


struct TaskBase;
struct TaskMgr;
struct Framework;

/* most heaps a task gets: one per root heap of the heap manager */
static constexpr s32 kMaxTaskHeaps = 4;

/* which class a task factory creates: (type, value) */
struct TaskClassID {
    enum Type : u32 {
        cTag_None = 0,
        cTag_Integer = 1,
        cTag_Factory = 2,               // mValue is the factory function
        cTag_Text = 3,
    };
    be<u32> mType;
    be<u32> mValue;
};

/* the heaps created for a task, one per root heap */
struct HeapArray {
    gptr<RegistryHeap> mHeaps[kMaxTaskHeaps];
    be<u8> mAdjusted[kMaxTaskHeaps];        // shrunk to fit once
    be<u32> mPrimaryIndex;                  // the heap the task is created in

    RegistryHeap* primaryHeap() const { return mHeaps[mPrimaryIndex]; }
};

/* how the heap of one root heap is created for a task */
struct HeapPolicy {
    gptr<RegistryHeap> mParent;             // NULL: the parent task's heap, or the manager's
    be<u32> mSize;                          // 0 with mLeaveFree: all but mLeaveFree of the parent
    be<u32> mLeaveFree;
    be<u32> mAdjustReserve;                 // kept free when the heap is shrunk to fit
    be<u8> mAdjust;                         // shrink to fit after the task's prepare
    be<u8> mFromEnd;                        // allocate the heap from the end of its parent
    be<u8> mNoHeap;                         // create no heap for this root heap
    u8 mPadding13;

    static HeapPolicy* construct(HeapPolicy* policy);
};

/* how a task is created */
struct CreateDelegateVtable {
    VtableSlot mHeader;
    VtableSlot mInvoke;                     // (task)
};
struct CreateDelegate {
    gptr<CreateDelegateVtable> mVtable;
};
struct CreateListener;
struct CreateListenerLink : Link {
    gptr<CreateListener> mListener;
};
struct CreateListener {
    u8 mOwnerData[0x10];
    CreateListenerLink mLink;
    gptr<LinkList> mList;
    gptr<CreateDelegate> mDelegate;         // told the created task
    u8 mState[0x14];
    be<u8> mListening;
    u8 mPadding3D[3];
};
struct TaskCreateArg {
    TaskClassID mClassID;
    HeapPolicy mHeapPolicies[kMaxTaskHeaps];
    be<u32> mPrimaryHeapIndex;
    gptr<TaskBase> mParent;
    gptr<void> mParameter;                  // handed to the task's constructor
    gptr<TaskBase> mFader;                  // set: the request goes through this fader task
    gptr<be<u8>> mName;
    gptr<gptr<TaskBase>> mCreatedTask;      // receives the created task
    gptr<CreateListener> mListener;         // told when the task is created
    be<s32> mUserID;                        // -1: none
    be<u32> mTag;                           // TaskBase::Tag
    be<u32> mCreateCallback;                // called with the new task before its prepare

    static TaskCreateArg* construct(TaskCreateArg* arg);
    static TaskCreateArg* constructForClass(TaskCreateArg* arg, const TaskClassID* classID);
    static TaskCreateArg* constructPrimaryHeapOnly(TaskCreateArg* arg, const TaskClassID* classID);
    static TaskCreateArg* constructSystem(TaskCreateArg* arg, const TaskClassID* classID);
};

/* what the task manager hands a task's constructor */
struct TaskConstructArg {
    gptr<HeapArray> mHeapArray;
    gptr<TaskMgr> mTaskMgr;
    gptr<void> mParameter;
};

/* a task in one of the task manager's lists */
struct TaskListNode : Link {
    gptr<TaskBase> mTask;
    gptr<LinkList> mList;

    /* inlined list removal: unlink from the list the node is in */
    void erase() {
        LinkList* list = mList;
        if (list != NULL) {
            mList = NULL;
            detach();
            list->mCount = list->mCount - 1;
        }
    }
};

struct TaskBaseVtable {
    VtableSlot mHeader;
    VtableSlot mIsKindOf;                   // 0x08 (type) -> bool
    VtableSlot mReserved10;
    VtableSlot mPauseCalc;                  // 0x18 (pause)
    VtableSlot mPauseDraw;                  // 0x20 (pause)
    VtableSlot mReserved28[4];
    VtableSlot mDestroy;                    // 0x48 (flags)
    VtableSlot mPrepare;                    // 0x50
    VtableSlot mStart;                      // 0x58
    VtableSlot mEnter;                      // 0x60
    VtableSlot mExit;                       // 0x68
    VtableSlot mOnEvent;                    // 0x70 (this, const TaskEvent*)
    VtableSlot mAttachCalc;                 // 0x78
    VtableSlot mAttachDraw;                 // 0x80
    VtableSlot mDetachCalc;                 // 0x88
    VtableSlot mDetachDraw;                 // 0x90
    VtableSlot mReserved98;
    VtableSlot mGetMethodTreeNode;          // 0xA0 (phase) -> node
};

/* the base of every task */
struct TaskBase {
    enum Tag : u32 {
        cSystem = 0,                        // attached to the system method trees
        cApp = 1,                           // attached to the app trees (or below the parent task)
    };
    enum State : u32 {
        cCreated = 0,
        cPrepare = 1,                       // queued for the prepare thread
        cPrepared = 2,
        cSleep = 3,
        cRunning = 4,
        cPaused = 5,
        cDying = 6,
        cDead = 7,
    };
    /* mFlags bits the destruction checks */
    enum Flag : u32 {
        cRequestDestroy = 2,
        cDestroyable = 4,
    };

    TreeNode mTreeNode;                     // mTreeNode.mData: the task itself
    Disposer mDisposer;
    SafeText mName;
    gptr<void> mParameter;
    be<u32> mFlags;
    TaskListNode mListNode;                 // in one of the task manager's lists
    HeapArray mHeapArray;
    gptr<TaskMgr> mTaskMgr;
    be<u32> mState;
    be<s32> mTag;                           // also the order in the manager's lists (higher first)
    TaskClassID mClassID;
    gptr<TaskBaseVtable> mVtable;

    static TaskBase* construct(TaskBase* task, const TaskConstructArg* arg, const be<u8>* name);
    void destroy(u32 flags);
    void attachCalcDraw();
    void start();
    MethodTreeMgrBase* getMethodTreeMgr();
    void attachToMethodTree(u32 phase, MethodTreeNode* node);
    u32 requestCreateTask(const TaskCreateArg* arg);
    void adjustHeap(u32 index, u32 reserve);
    void adjustHeapAll();
    Framework* getFramework();
    bool isKindOf(const RuntimeType* type) const;
    void prepare();
    void enter();
    void exit();
    void onEvent(const TaskEvent* event);   // base 02748E10: ignores the event

    TaskBase* parent() const {
        TreeNode* node = mTreeNode.mParent;
        return node != NULL ? node->data<TaskBase>() : NULL;
    }
    MethodTreeNode* getMethodTreeNode(u32 phase) {
        return gabi::call_ptr<MethodTreeNode*>(mVtable->mGetMethodTreeNode.mFunction, this, phase);
    }
};

/* per-TU copies of the task-base translation unit */
namespace task_base_instances {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
}

}

WWHD_SIZE(sead::TaskClassID, 8);
WWHD_OFFSET(sead::HeapArray, mAdjusted, 0x10);
WWHD_OFFSET(sead::HeapArray, mPrimaryIndex, 0x14);
WWHD_SIZE(sead::HeapArray, 0x18);
WWHD_OFFSET(sead::HeapPolicy, mLeaveFree, 8);
WWHD_OFFSET(sead::HeapPolicy, mAdjustReserve, 0xC);
WWHD_OFFSET(sead::HeapPolicy, mAdjust, 0x10);
WWHD_OFFSET(sead::HeapPolicy, mFromEnd, 0x11);
WWHD_OFFSET(sead::HeapPolicy, mNoHeap, 0x12);
WWHD_SIZE(sead::HeapPolicy, 0x14);
WWHD_OFFSET(sead::CreateDelegateVtable, mInvoke, 8);
WWHD_OFFSET(sead::CreateListenerLink, mListener, 8);
WWHD_OFFSET(sead::CreateListener, mLink, 0x10);
WWHD_OFFSET(sead::CreateListener, mList, 0x1C);
WWHD_OFFSET(sead::CreateListener, mDelegate, 0x20);
WWHD_OFFSET(sead::CreateListener, mListening, 0x38);
WWHD_SIZE(sead::CreateListener, 0x3C);
WWHD_OFFSET(sead::TaskCreateArg, mHeapPolicies, 8);
WWHD_OFFSET(sead::TaskCreateArg, mPrimaryHeapIndex, 0x58);
WWHD_OFFSET(sead::TaskCreateArg, mParent, 0x5C);
WWHD_OFFSET(sead::TaskCreateArg, mParameter, 0x60);
WWHD_OFFSET(sead::TaskCreateArg, mFader, 0x64);
WWHD_OFFSET(sead::TaskCreateArg, mName, 0x68);
WWHD_OFFSET(sead::TaskCreateArg, mCreatedTask, 0x6C);
WWHD_OFFSET(sead::TaskCreateArg, mListener, 0x70);
WWHD_OFFSET(sead::TaskCreateArg, mUserID, 0x74);
WWHD_OFFSET(sead::TaskCreateArg, mTag, 0x78);
WWHD_OFFSET(sead::TaskCreateArg, mCreateCallback, 0x7C);
WWHD_SIZE(sead::TaskCreateArg, 0x80);
WWHD_OFFSET(sead::TaskConstructArg, mParameter, 8);
WWHD_SIZE(sead::TaskConstructArg, 0xC);
WWHD_OFFSET(sead::TaskListNode, mTask, 8);
WWHD_OFFSET(sead::TaskListNode, mList, 0xC);
WWHD_SIZE(sead::TaskListNode, 0x10);
WWHD_OFFSET(sead::TaskBaseVtable, mPauseCalc, 0x18);
WWHD_OFFSET(sead::TaskBaseVtable, mDestroy, 0x48);
WWHD_OFFSET(sead::TaskBaseVtable, mPrepare, 0x50);
WWHD_OFFSET(sead::TaskBaseVtable, mExit, 0x68);
WWHD_OFFSET(sead::TaskBaseVtable, mAttachCalc, 0x78);
WWHD_OFFSET(sead::TaskBaseVtable, mDetachDraw, 0x90);
WWHD_OFFSET(sead::TaskBaseVtable, mGetMethodTreeNode, 0xA0);
WWHD_OFFSET(sead::TaskBase, mDisposer, 0x14);
WWHD_OFFSET(sead::TaskBase, mName, 0x24);
WWHD_OFFSET(sead::TaskBase, mParameter, 0x2C);
WWHD_OFFSET(sead::TaskBase, mFlags, 0x30);
WWHD_OFFSET(sead::TaskBase, mListNode, 0x34);
WWHD_OFFSET(sead::TaskBase, mHeapArray, 0x44);
WWHD_OFFSET(sead::TaskBase, mTaskMgr, 0x5C);
WWHD_OFFSET(sead::TaskBase, mState, 0x60);
WWHD_OFFSET(sead::TaskBase, mTag, 0x64);
WWHD_OFFSET(sead::TaskBase, mClassID, 0x68);
WWHD_OFFSET(sead::TaskBase, mVtable, 0x70);
WWHD_SIZE(sead::TaskBase, 0x74);
WWHD_SIZE(sead::TaskEvent, 8);

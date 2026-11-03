/* WWHD sead method trees: the calc-only and calc+draw tasks' method-tree members, the method
 * tree manager (calc tree and the top/bottom screen draw trees) and the framework's run and
 * initialization, with the per-TU copies of their texts, node destructors and delegates.
 * Names come from the binary's strings ("sead::RootCalc", "sead::TopAppDraw", "sead::ThreadMgr")
 * and the vtables; layouts from this game's constructors and instructions. */
#pragma once
#include "sead/FileDevice.h"
#include "sead/HeapRegistry.h"
#include "sead/MemberDispatch.h"
#include "sead/RuntimeType.h"

namespace sead {

/* ---- local copies of classes owned by other units (SEAD#10 task/tree code, SEAD#13 heaps) ---- */

/* tree links of a node; mData points back to the object that owns the node */
struct TreeNode_l {
    gptr<TreeNode_l> mParent;
    u8 mChildAndSiblings[0xC];
    gptr<void> mData;
};

struct DelegateVtable_l {
    VtableSlot mHeader;
    VtableSlot mInvoke;
};
/* a member-function delegate: object plus GHS pointer-to-member (vtable offset in mTarget's
 * low half when mVirtualIndex >= 0, else a direct function address) */
struct MethodDelegate_l {
    gptr<DelegateVtable_l> mVtable;
    gptr<u8> mObject;
    MemberSlot mMethod;
};

struct PauseListenerVtable_l {
    VtableSlot mHeader;
    VtableSlot mOnPauseChange;      // (node, pause flags)
};
struct PauseListener_l {
    gptr<PauseListenerVtable_l> mVtable;
};

struct MethodTreeNodeVtable_l {
    VtableSlot mHeader[3];
    VtableSlot mDestroy;
};
/* one node of a method tree: calls its delegate, then its children, unless paused */
struct MethodTreeNode_l {
    TreeNode_l mTreeNode;
    SafeText mName;
    Disposer mDisposer;
    MethodDelegate_l mDelegate;
    gptr<void> mOwner;              // the tree manager for its own nodes, else NULL
    be<u32> mReserved40;
    be<u32> mPauseFlags;
    gptr<PauseListener_l> mPauseListener;
    be<u32> mReserved4C;
    gptr<MethodTreeNodeVtable_l> mVtable;

    void lock();                    // SEAD#10
    void unlock();
    void detachAll();
    void pushBackChild(MethodTreeNode_l* child);
    void pushFrontChild(MethodTreeNode_l* child);
    void call();
};

/* pause flags of a method tree node */
enum MethodTreePause : u32 {
    cPauseNone = 0,
    cPauseSelf = 1,
    cPauseChild = 2,
    cPauseSelfAndChild = 3,
};

/* where a task attaches a method node in the tree manager */
enum MethodTreePhase : u32 {
    cCalcSys = 0,
    cCalcApp = 1,
    cDrawSys = 2,                   // top or bottom screen, as mSystemScreen selects
    cDrawApp = 3,                   // top or bottom screen, as mAppScreen selects
    cDrawAppFinal = 4,
    cDrawTopSys = 5,
    cDrawTopApp = 6,
    cDrawTopAppFinal = 7,
    cDrawBtmSys = 8,
    cDrawBtmApp = 9,
    cDrawBtmAppFinal = 10,
};
enum DrawScreen : u32 { cScreenTop = 0, cScreenBottom = 1 };

struct MethodTreeMgr;
struct TaskBaseVtable_l {
    VtableSlot mHeader[20];
    VtableSlot mGetMethodTreeNode;  // 0xA0 (phase) -> node
};
struct TaskBase_l {
    TreeNode_l mTreeNode;           // mTreeNode.mParent->mData: the parent task
    Disposer mDisposer;
    u8 mUnknown24[0x38];
    gptr<void> mTaskMgr;
    u8 mUnknown60[4];
    be<u32> mAttachToParent;        // attach below the parent task's node instead of the manager's
    u8 mUnknown68[8];
    gptr<TaskBaseVtable_l> mVtable;

    MethodTreeMgr* getMethodTreeMgr();                               // SEAD#10
    void attachToMethodTree(u32 phase, MethodTreeNode_l* node);
    MethodTreeNode_l* getMethodTreeNode(u32 phase) {
        return gabi::call_ptr<MethodTreeNode_l*>(mVtable->mGetMethodTreeNode.mFunction, this, phase);
    }
};

struct RuntimeTypedVtable_l {
    VtableSlot mHeader;
    VtableSlot mIsKindOf;           // (type) -> bool
};
/* the manager's base: a critical section and the runtime-typed vtable */
struct MethodTreeMgrBase_l {
    u8 mCriticalSection[0x3C];
    gptr<RuntimeTypedVtable_l> mVtable;
};

/* ---- this unit's classes ---- */

/* a task with one calc node */
struct CalcTask {
    TaskBase_l mTaskBase;
    MethodTreeNode_l mCalcEntry;

    static CalcTask* construct(CalcTask* task, void* parameters, const be<u8>* name);
    void destroy(u32 flags);
    void pauseCalc(u32 pause);
    void pauseDraw(u32 pause);
    void attachCalc();
    void attachDraw();
    void detachCalc();
    void detachDraw();
    MethodTreeNode_l* getMethodTreeNode(u32 phase);
    void calc();
};

/* a task with a calc node and draw nodes for the top and the bottom screen */
struct CalcDrawTask {
    TaskBase_l mTaskBase;
    MethodTreeNode_l mCalcEntry;
    MethodTreeNode_l mTopDrawEntry;
    MethodTreeNode_l mBottomDrawEntry;

    static CalcDrawTask* construct(CalcDrawTask* task, void* parameters, const be<u8>* name);
    void destroy(u32 flags);
    void pauseCalc(u32 pause);
    void pauseDraw(u32 pause);
    void attachCalc();
    void attachDraw();
    void detachCalc();
    void detachDraw();
    MethodTreeNode_l* getMethodTreeNode(u32 phase);
    void calc();
    void drawTop();
    void drawBottom();
};

/* the method trees: one calc tree and a draw tree per screen */
struct MethodTreeMgr {
    MethodTreeMgrBase_l mBase;
    MethodTreeNode_l mRootCalcNode;
    MethodTreeNode_l mSysCalcNode;
    MethodTreeNode_l mAppCalcNode;
    MethodTreeNode_l mTopRootDrawNode;
    MethodTreeNode_l mTopSysDrawNode;
    MethodTreeNode_l mTopAppDrawNode;
    MethodTreeNode_l mTopAppDrawFinalNode;
    MethodTreeNode_l mBtmRootDrawNode;
    MethodTreeNode_l mBtmSysDrawNode;
    MethodTreeNode_l mBtmAppDrawNode;
    MethodTreeNode_l mBtmAppDrawFinalNode;
    be<u32> mSystemScreen;
    be<u32> mAppScreen;

    static MethodTreeMgr* construct(MethodTreeMgr* manager);
    void attachMethod(u32 phase, MethodTreeNode_l* node);
    void pauseAll(u32 pause);
    void calc();
    void drawTop();
    void drawBottom();
};

struct FrameworkVtable_l {
    VtableSlot mHeader[10];
    VtableSlot mPrepareRun;         // 0x50 (heap)
    VtableSlot mRunLoop;            // 0x58
    VtableSlot mCreateMethodTreeMgr; // 0x60 (heap) -> manager
};
struct ObjectListVtable_l {
    VtableSlot mHeader;
    VtableSlot mUnused;
};
struct ObjectList_l : LinkList {
    gptr<ObjectListVtable_l> mVtable;
};

struct FrameworkRunSettings {
    be<u32> mStackSize;             // 0: 0x8000
    be<s32> mPriority;
};
struct FrameworkInitArgs {
    gptr<void> mArena;
    gptr<void> mRootHeap;           // used instead of the arena when set
};
struct TaskMgrInitArgs {
    be<u32> mContextCount;
    be<u32> mStackSize;
    be<s32> mPriority;
    gptr<void> mParameters;
    gptr<void> mHeap;
    gptr<struct Framework> mFramework;
};
struct Framework {
    be<u8> mReady;
    u8 mPadding01[3];
    gptr<void> mHeap;
    ObjectList_l mObjectList;
    gptr<void> mTaskMgr;
    gptr<MethodTreeMgr> mMethodTreeMgr;
    gptr<RegistryHeap> mMethodTreeMgrHeap;
    gptr<FrameworkVtable_l> mVtable;

    static Framework* construct(Framework* framework);
    static void initialize(FrameworkInitArgs* arg);
    void run(void* heap, void* parameters, const FrameworkRunSettings* settings);
};

/* ---- per-TU copies (one group per translation unit that stores the vtables) ---- */
namespace calc_task_instances {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct MethodTreeNode : MethodTreeNode_l { void destroy(u32 flags); };
struct NodeDelegate : MethodDelegate_l { void invoke(); };
struct TaskDelegate : MethodDelegate_l { void invoke(); };
}
namespace method_tree_mgr_instances {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct MethodTreeNode : MethodTreeNode_l { void destroy(u32 flags); };
struct NodeDelegate : MethodDelegate_l { void invoke(); };
}
namespace calc_draw_task_instances {
struct Text : SafeText { void destroy(u32 flags); void assureTerminationImpl(); };
struct MethodTreeNode : MethodTreeNode_l { void destroy(u32 flags); };
struct NodeDelegate : MethodDelegate_l { void invoke(); };
struct TaskDelegate : MethodDelegate_l { void invoke(); };
}

}

WWHD_OFFSET(sead::TreeNode_l, mData, 0x10);
WWHD_SIZE(sead::TreeNode_l, 0x14);
WWHD_OFFSET(sead::DelegateVtable_l, mInvoke, 8);
WWHD_OFFSET(sead::MethodDelegate_l, mObject, 4);
WWHD_OFFSET(sead::MethodDelegate_l, mMethod, 8);
WWHD_SIZE(sead::MethodDelegate_l, 0x10);
WWHD_OFFSET(sead::PauseListenerVtable_l, mOnPauseChange, 8);
WWHD_OFFSET(sead::MethodTreeNodeVtable_l, mDestroy, 0x18);
WWHD_OFFSET(sead::MethodTreeNode_l, mName, 0x14);
WWHD_OFFSET(sead::MethodTreeNode_l, mDisposer, 0x1C);
WWHD_OFFSET(sead::MethodTreeNode_l, mDelegate, 0x2C);
WWHD_OFFSET(sead::MethodTreeNode_l, mOwner, 0x3C);
WWHD_OFFSET(sead::MethodTreeNode_l, mPauseFlags, 0x44);
WWHD_OFFSET(sead::MethodTreeNode_l, mPauseListener, 0x48);
WWHD_OFFSET(sead::MethodTreeNode_l, mReserved4C, 0x4C);
WWHD_OFFSET(sead::MethodTreeNode_l, mVtable, 0x50);
WWHD_SIZE(sead::MethodTreeNode_l, 0x54);
WWHD_OFFSET(sead::TaskBaseVtable_l, mGetMethodTreeNode, 0xA0);
WWHD_OFFSET(sead::TaskBase_l, mDisposer, 0x14);
WWHD_OFFSET(sead::TaskBase_l, mTaskMgr, 0x5C);
WWHD_OFFSET(sead::TaskBase_l, mAttachToParent, 0x64);
WWHD_OFFSET(sead::TaskBase_l, mVtable, 0x70);
WWHD_SIZE(sead::TaskBase_l, 0x74);
WWHD_OFFSET(sead::RuntimeTypedVtable_l, mIsKindOf, 8);
WWHD_OFFSET(sead::MethodTreeMgrBase_l, mVtable, 0x3C);
WWHD_SIZE(sead::MethodTreeMgrBase_l, 0x40);
WWHD_OFFSET(sead::CalcTask, mCalcEntry, 0x74);
WWHD_SIZE(sead::CalcTask, 0xC8);
WWHD_OFFSET(sead::CalcDrawTask, mTopDrawEntry, 0xC8);
WWHD_OFFSET(sead::CalcDrawTask, mBottomDrawEntry, 0x11C);
WWHD_SIZE(sead::CalcDrawTask, 0x170);
WWHD_OFFSET(sead::MethodTreeMgr, mRootCalcNode, 0x40);
WWHD_OFFSET(sead::MethodTreeMgr, mSystemScreen, 0x3DC);
WWHD_OFFSET(sead::MethodTreeMgr, mAppScreen, 0x3E0);
WWHD_SIZE(sead::MethodTreeMgr, 0x3E4);
WWHD_OFFSET(sead::FrameworkVtable_l, mPrepareRun, 0x50);
WWHD_OFFSET(sead::FrameworkVtable_l, mRunLoop, 0x58);
WWHD_OFFSET(sead::FrameworkVtable_l, mCreateMethodTreeMgr, 0x60);
WWHD_OFFSET(sead::ObjectList_l, mVtable, 0xC);
WWHD_SIZE(sead::ObjectList_l, 0x10);
WWHD_OFFSET(sead::FrameworkRunSettings, mPriority, 4);
WWHD_SIZE(sead::FrameworkRunSettings, 8);
WWHD_OFFSET(sead::FrameworkInitArgs, mRootHeap, 4);
WWHD_SIZE(sead::FrameworkInitArgs, 8);
WWHD_OFFSET(sead::TaskMgrInitArgs, mParameters, 0xC);
WWHD_OFFSET(sead::TaskMgrInitArgs, mFramework, 0x14);
WWHD_SIZE(sead::TaskMgrInitArgs, 0x18);
WWHD_OFFSET(sead::Framework, mHeap, 4);
WWHD_OFFSET(sead::Framework, mObjectList, 8);
WWHD_OFFSET(sead::Framework, mTaskMgr, 0x18);
WWHD_OFFSET(sead::Framework, mMethodTreeMgr, 0x1C);
WWHD_OFFSET(sead::Framework, mMethodTreeMgrHeap, 0x20);
WWHD_OFFSET(sead::Framework, mVtable, 0x24);
WWHD_SIZE(sead::Framework, 0x28);

/* WWHD sead method tree nodes: the tree links every task and method node is built on, the
 * member-function delegates a node calls, the pause listener and the method tree node itself
 * (critical-section propagation, attaching children, the recursive call). Also the abstract method
 * tree manager base (a critical section plus the runtime-typed vtable) that the tasks attach to.
 * Names come from the binary's strings and vtables; layouts from this game's constructors and
 * instructions (the tree node is 0x10 bytes of links plus the owner pointer; the method tree node
 * 0x54 bytes; the manager base 0x40 bytes). */
#pragma once
#include "sead/FileDevice.h"
#include "sead/MemberDispatch.h"
#include "sead/RuntimeType.h"

namespace sead {

/* ---- local copy of a class owned by another unit (sead threads) ---- */

/* critical section: a disposer plus the OS mutex (constructor 02760084) */
struct CriticalSection_l {
    Disposer mDisposer;
    u8 mMutex[0x2C];

    void lock();
    void unlock();
    u32 tryLock();                          // nonzero when locked
};

/* ---- tree links ---- */

/* the links of a tree node; mData points back to the object that owns the node */
struct TreeNode {
    gptr<TreeNode> mParent;
    gptr<TreeNode> mChild;
    gptr<TreeNode> mNext;
    gptr<TreeNode> mPrevious;
    gptr<void> mData;

    template <class T> T* data() const { return static_cast<T*>(mData.get()); }

    static void construct(TreeNode* node);  // the four links (0x10 bytes) only
    void detachSubTree();                   // unlink from parent and siblings, keep the children
    void detachAll();                       // also release the children
    void pushBackChild(TreeNode* child);
    void pushFrontChild(TreeNode* child);
};

/* ---- delegates ---- */

struct DelegateVtable {
    VtableSlot mHeader;
    VtableSlot mInvoke;
};
/* a delegate without arguments */
struct Delegate {
    gptr<DelegateVtable> mVtable;

    void invoke() { gabi::call_ptr<void>(mVtable->mInvoke.mFunction, this); }
};
/* a member-function delegate: the object plus a GHS pointer to member (a vtable offset in
 * mTarget's low half when mVirtualIndex > 0, else a function address) */
struct MethodDelegate : Delegate {
    gptr<u8> mObject;
    MemberSlot mMethod;

    /* the member function the delegate calls and the object it calls it on (the invoke bodies of
     * every translation unit's copy); false when unbound */
    bool resolve(u8*& object, u32& target) const {
        u8* receiver = mObject;
        if (receiver == NULL) return false;
        s16 index = mMethod.mVirtualIndex;
        if (index == 0) return false;
        object = receiver + (s32)mMethod.mObjectAdjustment;
        if (index < 0) {
            target = mMethod.mTarget;
        } else {
            s16 vtableOffset = (u32)mMethod.mTarget;
            VirtualEntry* vtable = *reinterpret_cast<gptr<VirtualEntry>*>(object + vtableOffset);
            target = vtable[index].mTarget;
        }
        return true;
    }
};

/* told when a node's pause flags change: (node, new flags) */
struct PauseListenerVtable {
    VtableSlot mHeader;
    VtableSlot mOnPauseChange;
};
struct PauseListener {
    gptr<PauseListenerVtable> mVtable;
};

/* ---- method tree node ---- */

struct MethodTreeNodeVtable {
    VtableSlot mHeader[3];
    VtableSlot mDestroy;
};

/* one node of a method tree: calls its delegate, then its children, unless paused */
struct MethodTreeNode {
    enum PauseFlag : u32 {
        cPauseNone = 0,
        cPauseSelf = 1,
        cPauseChildren = 2,
        cPauseBoth = 3,
    };

    TreeNode mTreeNode;
    SafeText mName;
    Disposer mDisposer;
    MethodDelegate mDelegate;
    gptr<CriticalSection_l> mCriticalSection;    // the tree's lock, shared by every node of the tree
    be<u32> mReserved40;
    be<u32> mPauseFlags;
    gptr<PauseListener> mPauseListener;
    gptr<void> mUserData;
    gptr<MethodTreeNodeVtable> mVtable;

    void attachCriticalSectionRecursive(CriticalSection_l* section);
    void lock();
    void unlock();
    void detachAll();
    void pushBackChild(MethodTreeNode* child);
    void pushFrontChild(MethodTreeNode* child);
    void callRecursive();
    void call();

    /* inlined into the tasks' pause functions and constructors */
    void setPauseFlags(u32 flags) {
        lock();
        PauseListener* listener = mPauseListener;
        if (listener != NULL)
            gabi::call_ptr<void>(listener->mVtable->mOnPauseChange.mFunction, listener, this, flags);
        mPauseFlags = flags;
        unlock();
    }
};

/* ---- the method tree manager's base ---- */

struct MethodTreeMgrBaseVtable {
    VtableSlot mHeader;
    VtableSlot mIsKindOf;                   // (type) -> bool
    VtableSlot mSlots[2];
    VtableSlot mAttachMethod;               // 0x20 (phase, node)
    VtableSlot mReserved28;
    VtableSlot mPauseAll;                   // 0x30 (pause)
};
/* the abstract method tree manager: the critical section of the trees and the runtime-typed
 * vtable (the dual-screen manager of sead/MethodTree.h derives from it) */
struct MethodTreeMgrBase {
    CriticalSection_l mCriticalSection;
    gptr<MethodTreeMgrBaseVtable> mVtable;

    static MethodTreeMgrBase* construct(MethodTreeMgrBase* manager);
    bool isKindOf(const RuntimeType* type) const;
    void attachMethod(u32 phase, MethodTreeNode* node) {
        gabi::call_ptr<void>(mVtable->mAttachMethod.mFunction, this, phase, node);
    }
};

}

WWHD_SIZE(sead::CriticalSection_l, 0x3C);
WWHD_OFFSET(sead::TreeNode, mChild, 4);
WWHD_OFFSET(sead::TreeNode, mNext, 8);
WWHD_OFFSET(sead::TreeNode, mPrevious, 0xC);
WWHD_OFFSET(sead::TreeNode, mData, 0x10);
WWHD_SIZE(sead::TreeNode, 0x14);
WWHD_OFFSET(sead::DelegateVtable, mInvoke, 8);
WWHD_OFFSET(sead::MethodDelegate, mObject, 4);
WWHD_OFFSET(sead::MethodDelegate, mMethod, 8);
WWHD_SIZE(sead::MethodDelegate, 0x10);
WWHD_OFFSET(sead::PauseListenerVtable, mOnPauseChange, 8);
WWHD_OFFSET(sead::MethodTreeNodeVtable, mDestroy, 0x18);
WWHD_OFFSET(sead::MethodTreeNode, mName, 0x14);
WWHD_OFFSET(sead::MethodTreeNode, mDisposer, 0x1C);
WWHD_OFFSET(sead::MethodTreeNode, mDelegate, 0x2C);
WWHD_OFFSET(sead::MethodTreeNode, mCriticalSection, 0x3C);
WWHD_OFFSET(sead::MethodTreeNode, mPauseFlags, 0x44);
WWHD_OFFSET(sead::MethodTreeNode, mPauseListener, 0x48);
WWHD_OFFSET(sead::MethodTreeNode, mUserData, 0x4C);
WWHD_OFFSET(sead::MethodTreeNode, mVtable, 0x50);
WWHD_SIZE(sead::MethodTreeNode, 0x54);
WWHD_OFFSET(sead::MethodTreeMgrBaseVtable, mIsKindOf, 8);
WWHD_OFFSET(sead::MethodTreeMgrBaseVtable, mAttachMethod, 0x20);
WWHD_OFFSET(sead::MethodTreeMgrBaseVtable, mPauseAll, 0x30);
WWHD_OFFSET(sead::MethodTreeMgrBase, mVtable, 0x3C);
WWHD_SIZE(sead::MethodTreeMgrBase, 0x40);

/* sead tree-map instances emitted in the hd_parts_window translation unit (HDTU-LIB): the window
 * configuration map's node erase, the two member-function delegates the map clears itself with
 * (the user's delete callback, then the map's own node clear) and the in-order tree walks that
 * call them.
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

using namespace sead::instances;

static u8 sDeleteDelegateVtable;
static u8 sClearDelegateVtable;

/* 026F579C */
void sead::instances::parts_window::WindowNode::erase() {
    TreeMapPool<PooledTreeNode>* pool = mPool;
    mLeft = pool->mFreeHead;
    s32 count = pool->mCount;
    pool->mFreeHead = this;
    pool->mCount = count - 1;
}

/* inlined into the tree walks: Delegate1::invoke(node) */
void sead::instances::parts_window::NodeDelegate::invoke(StrTreeNode* node) {
    void* object = mObject;
    if (object == NULL) return;
    s16 index = mCallback.mVirtualIndex;
    if (index == 0) return;
    void* adjusted = static_cast<u8*>(object) + (s32)mCallback.mThisAdjustment;
    u32 target;
    if (index < 0) {
        target = mCallback.mFunction;
    } else {
        auto tableSlot = reinterpret_cast<gptr<MethodEntry>*>(static_cast<u8*>(adjusted) + (s32)mCallback.mVtableOffset);
        MethodEntry* table = *tableSlot;
        target = table[index].mFunction;
    }
    gabi::call_ptr<void>(target, adjusted, node);
}

/* 026F57BC */
void sead::instances::parts_window::NodeWalk::forEachClear(StrTreeNode* node, ClearDelegate* delegate) {
    do {
        StrTreeNode* left = node->mLower;
        if (left != NULL) forEachClear(left, delegate);
        delegate->invoke(node);
        node = node->mUpper;
    } while (node != NULL);
}

/* 026F586C */
void sead::instances::parts_window::NodeWalk::forEachDelete(StrTreeNode* node, DeleteDelegate* delegate) {
    do {
        StrTreeNode* left = node->mLower;
        if (left != NULL) forEachDelete(left, delegate);
        delegate->invoke(node);
        node = node->mUpper;
    } while (node != NULL);
}

/* 026F591C */
parts_window::DeleteDelegate* sead::instances::parts_window::DeleteDelegate::construct(DeleteDelegate* delegate, void* object, const MemberFunction* method) {
    if (delegate == NULL) {
        delegate = static_cast<DeleteDelegate*>(operator_new(sizeof(DeleteDelegate)));
        if (delegate == NULL) return delegate;
    }
    const MemberFunctionWords* source = reinterpret_cast<const MemberFunctionWords*>(method);
    MemberFunctionWords* callback = reinterpret_cast<MemberFunctionWords*>(&delegate->mCallback);
    delegate->mObject = object;
    u32 word = source->mWords[0];
    callback->mWords[0] = word;
    word = source->mWords[1];
    delegate->mVtable = gabi::ea(&sDeleteDelegateVtable);
    callback->mWords[1] = word;
    return delegate;
}

/* 026F5988 */
parts_window::ClearDelegate* sead::instances::parts_window::ClearDelegate::construct(ClearDelegate* delegate, void* object, const MemberFunction* method) {
    if (delegate == NULL) {
        delegate = static_cast<ClearDelegate*>(operator_new(sizeof(ClearDelegate)));
        if (delegate == NULL) return delegate;
    }
    const MemberFunctionWords* source = reinterpret_cast<const MemberFunctionWords*>(method);
    MemberFunctionWords* callback = reinterpret_cast<MemberFunctionWords*>(&delegate->mCallback);
    delegate->mObject = object;
    u32 word = source->mWords[0];
    callback->mWords[0] = word;
    word = source->mWords[1];
    delegate->mVtable = gabi::ea(&sClearDelegateVtable);
    callback->mWords[1] = word;
    return delegate;
}

/* WWHD glue
data sDeleteDelegateVtable 10109A84 nocheck
data sClearDelegateVtable 10109A74 nocheck
*/

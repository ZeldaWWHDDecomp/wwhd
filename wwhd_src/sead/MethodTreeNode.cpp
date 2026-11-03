/* WWHD sead method tree node: propagating the tree's critical section, locking, attaching and
 * detaching children, and the recursive call that runs a tree (each node's delegate, then its
 * children, unless paused). Reconstructed from this game's instructions. */

#include "sead/MethodTreeNode.h"

using namespace sead;

/* 027479F4 */
void sead::MethodTreeNode::attachCriticalSectionRecursive(CriticalSection_l* section) {
    MethodTreeNode* node = this;
    while (true) {
        TreeNode* child = node->mTreeNode.mChild;
        node->mCriticalSection = section;
        if (child != NULL) {
            MethodTreeNode* childNode = child->data<MethodTreeNode>();
            if (childNode != NULL) childNode->attachCriticalSectionRecursive(section);
        }
        TreeNode* next = node->mTreeNode.mNext;
        if (next == NULL) return;
        node = next->data<MethodTreeNode>();
        if (node == NULL) return;
    }
}

/* 02747A64 */
void sead::MethodTreeNode::lock() {
    CriticalSection_l* section = mCriticalSection;
    if (section != NULL) section->lock();
}

/* 02747A74 */
void sead::MethodTreeNode::unlock() {
    CriticalSection_l* section = mCriticalSection;
    if (section != NULL) section->unlock();
}

/* 02747A84 */
void sead::MethodTreeNode::detachAll() {
    CriticalSection_l* section = mCriticalSection;
    attachCriticalSectionRecursive(NULL);
    mCriticalSection = section;
    lock();
    mTreeNode.detachAll();
    unlock();
    mCriticalSection = NULL;
}

/* 02747AE4 */
void sead::MethodTreeNode::pushBackChild(MethodTreeNode* child) {
    lock();
    child->mTreeNode.detachSubTree();
    TreeNode* grandchild = child->mTreeNode.mChild;
    child->mCriticalSection = mCriticalSection;
    if (grandchild != NULL) {
        MethodTreeNode* node = grandchild->data<MethodTreeNode>();
        if (node != NULL) node->attachCriticalSectionRecursive(mCriticalSection);
    }
    mTreeNode.pushBackChild(&child->mTreeNode);
    unlock();
}

/* 02747B60 */
void sead::MethodTreeNode::pushFrontChild(MethodTreeNode* child) {
    lock();
    child->mTreeNode.detachSubTree();
    TreeNode* grandchild = child->mTreeNode.mChild;
    child->mCriticalSection = mCriticalSection;
    if (grandchild != NULL) {
        MethodTreeNode* node = grandchild->data<MethodTreeNode>();
        if (node != NULL) node->attachCriticalSectionRecursive(mCriticalSection);
    }
    mTreeNode.pushFrontChild(&child->mTreeNode);
    unlock();
}

/* 02747BDC */
void sead::MethodTreeNode::callRecursive() {
    // HD: frame-exact for cross-build save states: the main thread is parked below the delegate call
    // when a state is taken, so the original's frame (r30/r31 saved, LR) is replayed, both chain
    // calls carry the original's return address and r30/r31, and the node and child are taken back
    // from r31/r30 after them (after a state load they are the snapshot's).
    gabi::AutoFrame frame{gabi::current_function()};
    MethodTreeNode* self = this;
    if ((self->mPauseFlags & cPauseSelf) == 0) {
        gabi::call_ptr_site(0x02747C1C, {{31, gabi::ea(self)}}, {}, self->mDelegate.mVtable->mInvoke.mFunction, &self->mDelegate);
        self = gabi::at<MethodTreeNode>(gabi::cpu->r[31]);
    }
    TreeNode* child = self->mTreeNode.mChild;
    if (child != NULL && (self->mPauseFlags & cPauseChildren) == 0) {
        do {
            gabi::call_site(0x02747C44, {{30, gabi::ea(child)}, {31, gabi::ea(self)}}, {}, 0x02747BDC,
                            child->data<MethodTreeNode>());
            child = gabi::at<TreeNode>(gabi::cpu->r[30]);
            child = child->data<MethodTreeNode>()->mTreeNode.mNext;
        } while (child != NULL);
    }
}

/* 02747C6C */
void sead::MethodTreeNode::call() {
    // HD: frame-exact (see callRecursive): the original's frame, the chain call's return address and
    // r31, and the node taken back from r31.
    gabi::AutoFrame frame{gabi::current_function()};
    lock();
    gabi::call_site(0x02747C8C, {{31, gabi::ea(this)}}, {}, 0x02747BDC, this);
    gabi::at<MethodTreeNode>(gabi::cpu->r[31])->unlock();
}

/* WWHD glue
 ext 027601BC void sead::CriticalSection_l::lock()
 ext 027601F0 void sead::CriticalSection_l::unlock()
 ext 02767398 void sead::TreeNode::detachSubTree()
 ext 02767464 void sead::TreeNode::detachAll()
 ext 027674F0 void sead::TreeNode::pushBackChild(sead::TreeNode*)
 ext 02767540 void sead::TreeNode::pushFrontChild(sead::TreeNode*)
*/

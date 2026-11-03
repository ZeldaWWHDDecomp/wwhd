/* sead tree-map node instance emitted in the hd_res_load_task translation unit (HDTU-LIB):
 * the reference map's node returns itself to the map's free list (TreeMapNode::erase_).
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

/* 0260F9B4 */
void sead::instances::res_load_task::ReferenceNode::erase() {
    TreeMapPool<PooledTreeNode>* pool = mPool;
    mLeft = pool->mFreeHead;
    s32 count = pool->mCount;
    pool->mFreeHead = this;
    pool->mCount = count - 1;
}

/* WWHD glue
*/

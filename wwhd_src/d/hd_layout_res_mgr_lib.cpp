/* sead tree-map node instance emitted in the hd_layout_res_mgr translation unit (HDTU-LIB):
 * the layout cache map's node returns itself to the map's free list (TreeMapNode::erase_).
 * Reconstruction from the game instructions; */
#include "bindings.h"
#include "sead/HdTuInstances.h"

/* 026FC120 */
void sead::instances::layout_res_mgr::CacheNode::erase() {
    TreeMapPool<PooledTreeNode>* pool = mPool;
    mLeft = pool->mFreeHead;
    s32 count = pool->mCount;
    pool->mFreeHead = this;
    pool->mCount = count - 1;
}

/* WWHD glue
*/

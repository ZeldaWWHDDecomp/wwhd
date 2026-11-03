/* The node pool at the start of every fixed-capacity sead tree map of this game (the SafeString- and
 * hash-keyed maps of the resource managers, the window part's config map, the archive/path maps of
 * the resource control): the tree root, the free list of unused nodes (released nodes link through
 * their left pointer), the node storage that rebuilds the free list, the live node count and the
 * capacity. A node's erase puts it back on the free-list head and decrements the count (02613EE0:
 * lwz 0x1C(node) -> pool, 4(pool) -> node+0, node -> 4(pool), 0xC(pool) - 1 -> 0xC(pool)). */
#pragma once
#include "wwhd.h"
namespace sead {
template <class Node> struct TreeMapPool {
    gptr<Node> mRoot;
    gptr<Node> mFreeHead;
    gptr<Node> mNodeStorage;
    be<s32> mCount;
    be<s32> mCapacity;
};
WWHD_OFFSET(TreeMapPool<void>, mFreeHead, 4);
WWHD_OFFSET(TreeMapPool<void>, mNodeStorage, 8);
WWHD_OFFSET(TreeMapPool<void>, mCount, 0xC);
WWHD_OFFSET(TreeMapPool<void>, mCapacity, 0x10);
WWHD_SIZE(TreeMapPool<void>, 0x14);
}

/** HD root heaps; no GameCube counterpart. Private decomp source. */
#pragma once
#include "wwhd.h"

struct hdRootHeapName {
    gptr<const char> mText;
    be<u32> mVtable;
};
WWHD_OFFSET(hdRootHeapName, mVtable, 4);
WWHD_SIZE(hdRootHeapName, 8);

void* hdCreateExpandedHeap(u32 size, hdRootHeapName* name, void* parent, s32 direction, s32 locking);
void hdDestroySystemTask(void* task, u32 flags);
void hdDestroySystemTaskBase(void* task, u32 flags);

struct hdRootHeapRegistry {
    gptr<void> mHeaps[13];
};
WWHD_SIZE(hdRootHeapRegistry, 52);
struct hdRootHeapSizes {
    be<u32> mSizes[13];
};
WWHD_SIZE(hdRootHeapSizes, 52);
using hdSeadRootHeapArray = gptr<gptr<void>>;

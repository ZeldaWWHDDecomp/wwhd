/** HD root-task factory argument views. */
#pragma once
#include "wwhd.h"

struct hdTaskHeapArray {
    gptr<void> mHeaps[5];
    be<u32> mCurrentHeapIndex;
};
WWHD_OFFSET(hdTaskHeapArray, mCurrentHeapIndex, 0x14);

struct hdTaskConstructArg {
    gptr<hdTaskHeapArray> mHeapArray;
};
WWHD_OFFSET(hdTaskConstructArg, mHeapArray, 0);

struct hdTaskRuntimeTypeInfo {
    be<u32> mTypeNameAddress;
};
WWHD_SIZE(hdTaskRuntimeTypeInfo, 4);

void* hdTaskAllocate(u32 size, void* heap, s32 alignment);
void* hdRootTaskConstruct(void* task, hdTaskConstructArg* argument);

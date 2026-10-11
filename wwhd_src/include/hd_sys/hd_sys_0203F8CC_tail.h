/** HD SystemTask tail views; private decomp source. */
#pragma once
#include "hd/hd_sys_02035444.h"

struct hdSystemTaskInstanceGuard {
    be<u8> mOwnsInstance;
};
struct hdSystemTaskTailView {
    u8 _padding00[0x20];
    be<u32> mTaskVtable;
    u8 _padding24[0xA4];
    hdSystemTaskInstanceGuard mInstanceGuard;
};
WWHD_OFFSET(hdSystemTaskTailView, mTaskVtable, 0x20);
WWHD_OFFSET(hdSystemTaskTailView, mInstanceGuard, 0xC8);

u32 hdForwardSystemCall(void* target);
void hdSystemTaskDestroyInstanceGuard(hdSystemTaskInstanceGuard* guard, u32 flags);
void hdSystemTaskDestroyBase(hdSystemTaskTailView* task, u32 flags);
void* hdErrorViewerTaskConstruct(void* task, hdTaskConstructArg* argument);
void* hdGameTaskConstruct(void* task, hdTaskConstructArg* argument);

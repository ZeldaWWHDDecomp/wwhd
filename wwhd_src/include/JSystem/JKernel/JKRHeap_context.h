#pragma once
#include "wwhd.h"

struct JKRHeap;
// HD: heap selection is thread-local rather than tww's single sCurrentHeap.
struct HeapThreadContext {
    u8 mPrefix[0x70];
    gptr<JKRHeap> mpCurrentHeap;
};
struct HeapModuleManager {
    u8 mPrefix[0x24];
    be<u32> mThreadSpecificKey;
};
struct HeapModuleManagerSlot {
    gptr<HeapModuleManager> mpManager;
};
struct HeapContextSlot {
    gptr<void> mpContext;
};
WWHD_OFFSET(HeapThreadContext, mpCurrentHeap, 0x70);
WWHD_OFFSET(HeapModuleManager, mThreadSpecificKey, 0x24);
HeapThreadContext *OSGetThreadSpecific(u32 key);
JKRHeap *HeapContext_getCurrentHeap(void *context);

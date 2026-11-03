#ifndef WWHD_JASTHREAD_H
#define WWHD_JASTHREAD_H
#include "wwhd.h"
struct JASThreadVTable { u8 prefix[0x14]; gfn run; };
struct JASMessageQueue { u8 storage[0x3C]; };
// HD: JAudio owns a Cafe thread abstraction with its vtable after thread storage.
class JASThread {
public:
    gptr<void> mHeap;
    gptr<void> mThread;
    JASMessageQueue mQueue;
    gptr<void> mMessages;
    be<u32> mMessageCount;
    gptr<u8> mStack;
    be<u32> mStackSize;
    gptr<JASThreadVTable> mVtable;
    void* run();
};
using JASThreadEntry = void* (*)(s32, JASThread*);
void* JASThread_entry(s32 argc, JASThread* thread);
JASThread* JASThread_construct(JASThread* thread, void* heap, u32 stackSize, u32 messageCount, s32 priority, u32 affinity);
WWHD_OFFSET(JASThread, mQueue, 8);
WWHD_OFFSET(JASThread, mMessages, 0x44);
WWHD_OFFSET(JASThread, mStack, 0x4C);
WWHD_OFFSET(JASThread, mStackSize, 0x50);
WWHD_OFFSET(JASThread, mVtable, 0x54);
WWHD_SIZE(JASThread, 0x58);
WWHD_SIZE(JASMessageQueue, 0x3C);
#endif

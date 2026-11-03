#ifndef WWHD_JASTASKTHREAD_H
#define WWHD_JASTASKTHREAD_H
#include "JSystem/JAudio/JASThread.h"
#include "JSystem/JAudio/JASSystemHeap.h"
struct JASThreadMonitor { u8 storage[0x3C]; };
struct JASThreadQueue { u8 storage[0x10]; };
struct JASTaskMessage { gfn mCallback; be<u32> mCopied; union { u8 mArguments[4]; gptr<void> mArgument; }; };
class JASTaskThread : public JASThread {
public:
    JASThreadQueue mWaitQueue;
    be<u8> mPaused;
    u8 padding[3];
    JASThreadMonitor mMonitor;
    JASTaskMessage* allocCallStack(u32 callback, void* argument, u32 size);
    void run();
    u32 sendCmdMsg(u32 callback, void* argument, u32 size);
};
JASTaskThread* JASTaskThread_construct(JASTaskThread*, s32 priority, u32 messageCount, u32 stackSize, u32 affinity);
WWHD_OFFSET(JASTaskThread, mWaitQueue, 0x58);
WWHD_OFFSET(JASTaskThread, mPaused, 0x68);
WWHD_OFFSET(JASTaskThread, mMonitor, 0x6C);
WWHD_SIZE(JASTaskThread, 0xA8);
WWHD_SIZE(JASystem::Kernel::MessageBlock, 0x40C);
WWHD_SIZE(JASystem::Kernel::MessagePool, 0x30);
#endif

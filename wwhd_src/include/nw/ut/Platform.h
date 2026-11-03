#pragma once
#include "wwhd.h"
namespace nw { namespace ut {
struct Event {
    u8 storage[0x24];
    static Event* create(Event*,u32,u32);
    void destroy(u32);
    void signal();
    void reset();
    void wait();
    u32 tryWait(u32 timeoutWord);
};
struct ThreadTargetVtable { u8 reserved[0x14]; be<u32> run; };
struct ThreadTarget { gptr<ThreadTargetVtable> vtable; };
struct ThreadConfig {
    be<u32> priority;
    be<u32> stack;
    be<u32> stackSize;
    be<u32> attributes;
    gptr<const be<u8>> name;
};
struct Thread {
    u8 storage[0x6A8];
    be<u32> handle;
    u8 waitQueue[0x10];
    ThreadConfig config;
    gptr<ThreadTarget> target;
    static Thread* create(Thread*,ThreadTarget*);
    static u32 entry(u32,Thread*);
    void destroy(u32);
    u32 start(const ThreadConfig*);
    u32 join();
    u32 resume();
};
struct QueueMessage { be<u32> payload; be<u32> unusedWords[3]; };
WWHD_SIZE(QueueMessage,0x10); WWHD_OFFSET(QueueMessage,unusedWords,4);
struct QueueSendFrame { u8 linkage[8]; QueueMessage message; };
struct QueueReceiveFrame { u8 linkage[8]; QueueMessage message; u8 savedRegisters[8]; };
WWHD_SIZE(QueueSendFrame,0x18); WWHD_OFFSET(QueueSendFrame,message,8);
WWHD_SIZE(QueueReceiveFrame,0x20); WWHD_OFFSET(QueueReceiveFrame,message,8);
struct MessageQueue {
    u8 storage[0x3C];
    static MessageQueue* create(MessageQueue*);
    void destroy(u32);
    void initialize(void*,s32);
    u32 send(u32,u32);
    u32 sendFirst(u32,u32);
    u32 receive(be<u32>*,u32);
};
WWHD_SIZE(Event,0x24); WWHD_SIZE(ThreadConfig,0x14); WWHD_OFFSET(Thread,handle,0x6A8); WWHD_OFFSET(Thread,config,0x6BC); WWHD_OFFSET(Thread,target,0x6D0); WWHD_SIZE(Thread,0x6D4); WWHD_SIZE(MessageQueue,0x3C);
} }
void* utAllocate(u32);
void utDelete(void*);
void utInitializeEvent(nw::ut::Event*,u32,u32);
void utSignalEvent(nw::ut::Event*);
void utResetEvent(nw::ut::Event*);
void utWaitEvent(nw::ut::Event*);
u32 utTimedWaitEvent(nw::ut::Event*,u32,u32,u32);
void utInitializeThreadQueue(void*);
u32 utCreateThread(u32,u32,u32,nw::ut::Thread*,u32,u32,u32,u32);
void utNameThread(u32,const be<u8>*);
u32 utJoinThread(u32,void*);
u32 utResumeThread(u32);
void utInitializeMessageQueue(nw::ut::MessageQueue*,void*,s32);
u32 utSendMessage(nw::ut::MessageQueue*,const nw::ut::QueueMessage*,u32);
u32 utReceiveMessage(nw::ut::MessageQueue*,nw::ut::QueueMessage*,u32);
WWHD_SIZE(nw::ut::ThreadTargetVtable,0x18); WWHD_OFFSET(nw::ut::ThreadTargetVtable,run,0x14);
WWHD_SIZE(nw::ut::ThreadTarget,4);

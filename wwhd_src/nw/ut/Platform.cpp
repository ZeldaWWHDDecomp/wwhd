/* Cafe event, thread and message-queue adapters, reconstructed from WWHD. */
// SDK boundary facts: wut confirms the 16-byte OSMessage extent.
#include "nw/ut/Platform.h"
using namespace nw::ut;
/* 028F69A8 */
Event* Event::create(Event* event,u32 autoReset,u32 signalled) {
    if(!event) event=(Event*)utAllocate(sizeof(Event));
    if(event) utInitializeEvent(event,signalled,autoReset^1);
    return event;
}
/* 028F6A10 */
void Event::destroy(u32 flags) { if(gabi::ea(this) && (flags&1)) utDelete(this); }
/* 028F6A24 */
void Event::signal() { utSignalEvent(this); }
/* 028F6A28 */
void Event::reset() { utResetEvent(this); }
/* 028F6A2C */
void Event::wait() { utWaitEvent(this); }
/* 028F6A30 */
u32 Event::tryWait(u32 timeoutWord) { return utTimedWaitEvent(this,timeoutWord,0,0)!=0; }
/* 028F6A60 */
Thread* Thread::create(Thread* thread,ThreadTarget* target) {
    if(!thread) thread=(Thread*)utAllocate(sizeof(Thread));
    if(thread) {
        thread->target=target;
        thread->config.stackSize=0;
        thread->config.stack=0;
        thread->config.priority=0;
        thread->handle=(gabi::ea(thread)+7)&~7u;
        thread->config.name=nullptr;
        thread->config.attributes=0;
        utInitializeThreadQueue(thread->waitQueue);
    }
    return thread;
}
/* 028F6ADC */
void Thread::destroy(u32 flags) { if(gabi::ea(this) && (flags&1)) utDelete(this); }
/* 028F6AF0 */
u32 Thread::entry(u32,Thread* thread) {
    // HD: frame-exact for cross-build save states (the sound threads wait below this call): the
    // original's frame (back chain, LR).
    gabi::AutoFrame frame{gabi::current_function()};
    ThreadTarget* target=thread->target;
    gabi::call_ptr_site(0x028F6B10,{},{},target->vtable->run,target);
    return 0;
}
/* 028F6B24 */
u32 Thread::start(const ThreadConfig* settings) {
    config.priority=settings->priority;
    config.stack=settings->stack;
    config.stackSize=settings->stackSize;
    config.attributes=settings->attributes;
    config.name=settings->name;
    u32 threadHandle=handle;
    u32 stack=settings->stack;
    u32 size=settings->stackSize;
    u32 attributes=settings->attributes;
    u32 priority=settings->priority;
    u32 result=utCreateThread(threadHandle,gabi::guest_fn((const void*)Thread::entry),0,this,stack+size,size,priority,attributes&0xFFFF);
    result=result!=0;
    if(result) {
        const be<u8>* name=settings->name;
        if(name) utNameThread(handle,name);
    }
    return result;
}
/* 028F6BD8 */
u32 Thread::join() { return utJoinThread(handle,nullptr); }
/* 028F6BE4 */
u32 Thread::resume() { return utResumeThread(handle); }
/* 028F6BEC */
MessageQueue* MessageQueue::create(MessageQueue* queue) {
    if(!queue) queue=(MessageQueue*)utAllocate(sizeof(MessageQueue));
    return queue;
}
/* 028F6C18 */
void MessageQueue::destroy(u32 flags) { if(gabi::ea(this) && (flags&1)) utDelete(this); }
/* 028F6C2C */
void MessageQueue::initialize(void* messages,s32 count) { utInitializeMessageQueue(this,messages,count); }
/* 028F6C30 */
u32 MessageQueue::send(u32 message,u32 flags) {
    gabi::NativeFrame<sizeof(QueueSendFrame)> frame;
    QueueSendFrame* saved=gabi::at<QueueSendFrame>(frame.sp());
    saved->message.payload=message;
    return utSendMessage(this,&saved->message,flags)!=0;
}
/* 028F6C60 */
u32 MessageQueue::sendFirst(u32 message,u32 flags) {
    gabi::NativeFrame<sizeof(QueueSendFrame)> frame;
    QueueSendFrame* saved=gabi::at<QueueSendFrame>(frame.sp());
    saved->message.payload=message;
    return utSendMessage(this,&saved->message,flags|2)!=0;
}
/* 028F6C94 */
u32 MessageQueue::receive(be<u32>* output,u32 flags) {
    // HD: frame-exact for cross-build save states (the sound threads wait in this call): the
    // original's frame (back chain, LR, r31 saved) and r31 = the output at the wait; the output is
    // taken back from r31.
    gabi::AutoFrame automatic{gabi::current_function()};
    gabi::NativeFrame<sizeof(QueueReceiveFrame)> frame;
    QueueReceiveFrame* saved=gabi::at<QueueReceiveFrame>(frame.sp());
    u32 result=gabi::call_site<u32>(0x028F6CB0,{{31,gabi::ea(output)}},{},0xC0009ED0,this,&saved->message,flags);
    output=gabi::at<be<u32>>(gabi::cpu->r[31]);
    *output=saved->message.payload;
    return result!=0;
}
/* 028F6CD4 */
static void initializePlatform() {}
/* WWHD glue
ext 0273AD10 void* utAllocate(u32)
ext 0273AF40 void utDelete(void*)
ext C0009D48 void utInitializeEvent(nw::ut::Event*,u32,u32)
ext C000A0B0 void utSignalEvent(nw::ut::Event*)
ext C0009F18 void utResetEvent(nw::ut::Event*)
ext C000A1F0 void utWaitEvent(nw::ut::Event*)
ext C000A1F8 u32 utTimedWaitEvent(nw::ut::Event*,u32,u32,u32)
ext C0009DA0 void utInitializeThreadQueue(void*)
ext C0009A48 u32 utCreateThread(u32,u32,u32,nw::ut::Thread*,u32,u32,u32,u32)
ext C000A078 void utNameThread(u32,const be<u8>*)
ext C0009E00 u32 utJoinThread(u32,void*)
ext C0009F38 u32 utResumeThread(u32)
ext C0009D58 void utInitializeMessageQueue(nw::ut::MessageQueue*,void*,s32)
ext C0009FC8 u32 utSendMessage(nw::ut::MessageQueue*,const nw::ut::QueueMessage*,u32)
ext C0009ED0 u32 utReceiveMessage(nw::ut::MessageQueue*,nw::ut::QueueMessage*,u32)
*/

// HD JAudio task thread and message-block allocator.
#include "JSystem/JAudio/JASTaskThread.h"
using namespace JASystem::Kernel;
using namespace gabi;
struct Message { gptr<JASTaskMessage> task; be<u32> rest[3]; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
static JASThreadVTable sTaskVtable;
extern void* allocateTaskThreadObject(u32);
extern void* allocateTaskMessageBlock(u32, void*, u32);
extern void constructTaskMonitor(JASThreadMonitor*);
extern void initializeTaskWaitQueue(JASThreadQueue*);
extern void clearTaskWaitQueue(void*, u32);
extern void lockTaskPool(HeapMutex*);
extern void unlockTaskPool(HeapMutex*);
extern void copyTaskArguments(const void*, void*, u32);
extern u32 sendTaskQueueMessage(JASMessageQueue*, Message*, u32);
extern void deleteTaskMessageBlock(void*);
extern void registerTaskGlobals(void*);
extern u32 receiveTaskQueueMessage(JASMessageQueue*, Message*, u32);
extern void lockTaskMonitor(JASThreadMonitor*);
extern void unlockTaskMonitor(JASThreadMonitor*);
extern void sleepTaskThread(JASThreadQueue*);
/* 028108A4 */
JASTaskThread* JASTaskThread_construct(JASTaskThread* thread, s32 priority, u32 messageCount, u32 stackSize, u32 affinity) {
    if (!thread) {
        thread = static_cast<JASTaskThread*>(allocateTaskThreadObject(sizeof(JASTaskThread)));
        if (!thread)
            return nullptr;
    }
    JASThread_construct(thread, JASDram, stackSize, messageCount, priority, affinity);
    thread->mVtable = &sTaskVtable;
    clearTaskWaitQueue(&thread->mWaitQueue, sizeof(thread->mWaitQueue));
    thread->mPaused = 0;
    constructTaskMonitor(&thread->mMonitor);
    initializeTaskWaitQueue(&thread->mWaitQueue);
    return thread;
}
/* 02810944 */
JASTaskMessage* JASTaskThread::allocCallStack(u32 callback, void* argument, u32 size) {
    u32 required = size + 8;
    auto pool = static_cast<MessagePool*>(getMessagePool());
    lockTaskPool(&pool->mMutex);
    MessageBlock* block = pool->mHead;
    if (0x400u - (u32)block->mUsed < required) {
        if (required > 0x400) {
            unlockTaskPool(&pool->mMutex);
            return nullptr;
        }
        if (block && block->mReferences == 0) {
            block->mUsed = 0;
            block = pool->mHead;
        } else {
            void* heap = getSystemHeap();
            auto next = static_cast<MessageBlock*>(allocateTaskMessageBlock(sizeof(MessageBlock), heap, 4));
            if (next) {
                next->mUsed = 0;
                next->mPrevious = block;
                next->mReferences = 0;
            }
            block = next;
            pool->mHead = block;
            if (!block) {
                unlockTaskPool(&pool->mMutex);
                return nullptr;
            }
        }
    }
    u32 offset = block->mUsed;
    u32 references = block->mReferences;
    block->mUsed = offset + required;
    auto message = gabi::at<JASTaskMessage>(gabi::ea(block) + u32(offsetof(MessageBlock, mData)) + offset);
    block->mReferences = references + 1;
    unlockTaskPool(&pool->mMutex);
    if (!gabi::ea(message))
        return nullptr;
    message->mCopied = 1;
    copyTaskArguments(argument, message->mArguments, size);
    message->mCallback.v = callback;
    return message;
}
/* 02810AA4 */
u32 JASTaskThread::sendCmdMsg(u32 callback, void* argument, u32 size) {
    JASTaskMessage* task = allocCallStack(callback, argument, size);
    if (!task)
        return 0;
    Message message;
    message.task = task;
    u32 result = sendTaskQueueMessage(&mQueue, &message, 0);
    if (result)
        return result;
    auto pool = static_cast<MessagePool*>(getMessagePool());
    lockTaskPool(&pool->mMutex);
    MessageBlock* block = pool->mHead;
    MessageBlock* previous = nullptr;
    while (block) {
        u32 address = gabi::ea(task);
        if (gabi::ea(block->mData) <= address && address < gabi::ea(block + 1)) {
            u32 references = (u32)block->mReferences - 1;
            block->mReferences = references;
            if (block != pool->mHead && references == 0) {
                MessageBlock* next = block->mPrevious;
                deleteTaskMessageBlock(block);
                previous->mPrevious = next;
            }
            break;
        }
        previous = block;
        block = block->mPrevious;
    }
    unlockTaskPool(&pool->mMutex);
    return result;
}
/* 02810B9C */
void JASTaskThread::run() {
    // HD: frame-exact for cross-build save states: the thread waits in the receive, so the original's
    // frame (r26..r31 saved, LR), its message at sp+8 and, at the wait, its return address and the
    // registers the original holds there (r30 = this, r31 = the monitor; from the second pass on
    // r26..r29 = the last task, the pool mutex, the scan's previous block and the pool or next block).
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<Message> message(8);
    bool first = true;
    u32 r26 = 0, r27 = 0, r28 = 0, r29 = 0;
    for (;;) {
        if (first)
            gabi::call_site(0x02810BF4, {{30, gabi::ea(this)}, {31, gabi::ea(&mMonitor)}}, {}, 0xC0009ED0, &mQueue, message.get(), 1u);
        else
            gabi::call_site(0x02810BF4, {{26, r26}, {27, r27}, {28, r28}, {29, r29}, {30, gabi::ea(this)}, {31, gabi::ea(&mMonitor)}}, {},
                            0xC0009ED0, &mQueue, message.get(), 1u);
        first = false;
        JASTaskMessage* task = message->task;
        r26 = gabi::ea(task);
        lockTaskMonitor(&mMonitor);
        if (mPaused)
            sleepTaskThread(&mWaitQueue);
        unlockTaskMonitor(&mMonitor);
        u32 callback = task->mCallback.get();
        // HD retains the copied flag in r4 at the indirect callback.
        u32 copied = task->mCopied;
        if (copied)
            gabi::call_ptr<void>(callback, task->mArguments, copied);
        else
            gabi::call_ptr<void>(callback, task->mArgument.get(), copied);
        auto pool = static_cast<MessagePool*>(getMessagePool());
        r29 = gabi::ea(pool);
        r27 = gabi::ea(&pool->mMutex);
        r28 = 0;
        lockTaskPool(&pool->mMutex);
        MessageBlock* block = pool->mHead;
        MessageBlock* previous = nullptr;
        while (block) {
            u32 address = gabi::ea(task);
            if (gabi::ea(block->mData) <= address && address < gabi::ea(block + 1)) {
                u32 references = (u32)block->mReferences - 1;
                block->mReferences = references;
                if (block != pool->mHead && references == 0) {
                    MessageBlock* next = block->mPrevious;
                    r29 = gabi::ea(next);
                    deleteTaskMessageBlock(block);
                    previous->mPrevious = next;
                }
                break;
            }
            previous = block;
            r28 = gabi::ea(previous);
            block = block->mPrevious;
        }
        unlockTaskPool(&pool->mMutex);
    }
}
/* 02810CF8 */
void __sinit_JASTaskThread_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerTaskGlobals(&sRegistration);
}
/* WWHD glue
ext C0009ED0 u32 receiveTaskQueueMessage(JASMessageQueue*, Message*, u32)
ext 027604AC void lockTaskMonitor(JASThreadMonitor*)
ext 027604B4 void unlockTaskMonitor(JASThreadMonitor*)
ext C000A0C8 void sleepTaskThread(JASThreadQueue*)
data sTaskVtable 1016FE7C
data sInitialization 104B5284
data sRegistration 101F9DB8
ext 0273AD10 void* allocateTaskThreadObject(u32)
ext 0273B050 void* allocateTaskMessageBlock(u32, void*, u32)
ext 027603F0 void constructTaskMonitor(JASThreadMonitor*)
ext C0009DA0 void initializeTaskWaitQueue(JASThreadQueue*)
ext 028F521C void clearTaskWaitQueue(void*, u32)
ext C0009E40 void lockTaskPool(HeapMutex*)
ext C000A1C8 void unlockTaskPool(HeapMutex*)
ext 02843D30 void copyTaskArguments(const void*, void*, u32)
ext C0009FC8 u32 sendTaskQueueMessage(JASMessageQueue*, Message*, u32)
ext 0273AF40 void deleteTaskMessageBlock(void*)
ext 028F026C void registerTaskGlobals(void*)
*/

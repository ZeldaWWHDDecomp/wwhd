/* sead threads on the Cafe platform: critical sections, thread mutexes, message queues, threads and their manager,
 * and tick time. Reconstructed from this game's instructions; the SDK retail core count was checked against wut
 * declarations (facts only). No external implementation or library layout used. */
#include "sead/Thread.h"
using namespace sead;

static void* platformAllocateObject(u32 size);
static void* platformAllocateArray(u32 size, RegistryHeap* heap, s32 alignment);
static void platformReleaseObject(void* object);
static void platformReleaseArray(void* object);
static RegisteredObject* platformConstructDisposer(RegisteredObject* object);
static RegisteredObject* platformConstructHeapDisposer(RegisteredObject* object, RegistryHeap* heap, u32 option);
static void platformClear(void* destination, u32 size);
static void platformInitMutex(PlatformMutexStorage* mutex);
static void platformLockMutex(PlatformMutexStorage* mutex);
static u32 platformTryLockMutex(PlatformMutexStorage* mutex);
static void platformUnlockMutex(PlatformMutexStorage* mutex);
static void platformInitMessageQueue(PlatformMessageQueueStorage* queue, PlatformMessage* messages, s32 count);
static u32 platformSendMessage(PlatformMessageQueueStorage* queue, PlatformMessage* message, u32 flags);
static u32 platformReceiveMessage(PlatformMessageQueueStorage* queue, PlatformMessage* message, u32 flags);

static DisposerVtable sCriticalSectionVtable;
static DisposerVtable sThreadMutexVtable;

static void* platformAllocateAlignedObject(u32 size, RegistryHeap* heap, s32 alignment);
static void platformSetThreadSpecific(u32 key, Thread* value);
static PlatformThread* platformGetCurrentThread();
static s32 platformGetThreadPriority(PlatformThread* thread);
static u32 platformJoinThread(PlatformThread* thread, void* result);
static u32 platformCreateThread(PlatformThread* thread, u32 entry, u32 argument, void* context,
                         u8* stackTop, u32 stackSize, s32 priority, u32 attributes);
static void platformSetThreadName(PlatformThread* thread, const be<u8>* name);
static u32 platformSetThreadAffinity(PlatformThread* thread, u32 mask);
static s32 platformResumeThread(PlatformThread* thread);
static void platformSleepTicks(u32 high, u32 low);
static void platformYieldThread();
static u32 platformCoreBit(const be<u32>* core);

static ThreadManagerVtable sThreadManagerVtable;
static ThreadVtable sThreadVtable;
static ThreadVtable sDelegateThreadVtable;
static ThreadVtable sMainThreadVtable;
static DisposerVtable sDelegateNameOwnerVtable;
static DisposerVtable sThreadNameOwnerVtable;
static DisposerVtable sThreadManagerDisposerVtable;
static SafeTextVtable sThreadNameVtable;
static ThreadManager* sThreadManager;
static ThreadManagerDisposer* sThreadManagerDisposer;
static u8 sPrimaryThreadKeyUsed;
static u8 sSecondaryThreadKeyUsed;
static u32 sPlatformCoreCount;

/* inlined into the thread constructors */
static void clearThreadNode(ThreadNode* node) {
    if (gabi::ea(node) == 0) node = static_cast<ThreadNode*>(platformAllocateObject(sizeof(ThreadNode)));
    if (gabi::ea(node) != 0) {
        node->mNext = nullptr;
        node->mThread = nullptr;
        node->mPrevious = nullptr;
        node->mList = nullptr;
    }
}

/* inlined into thread start and affinity changes */
static u32 platformMask(Thread* thread, be<u32>* core, be<u32>* selected) {
    u32 result = 0;
    for (u32 index = 0; index < sPlatformCoreCount; ++index) {
        *selected = index;
        u32 bit = index < 32 ? 1u << static_cast<u32>(*selected) : 0;
        if ((static_cast<u32>(thread->mCoreMask) & bit) != 0) {
            *core = index;
            result |= platformCoreBit(core);
        }
    }
    return result;
}

/* 02760084 */
CriticalSection* CriticalSection::construct(CriticalSection* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<CriticalSection*>(platformAllocateObject(sizeof(CriticalSection)));
    if (object != nullptr) {
        platformConstructDisposer(object);
        object->mVtable = &sCriticalSectionVtable;
        platformClear(&object->mPlatformMutex, sizeof(PlatformMutexStorage));
        platformInitMutex(&object->mPlatformMutex);
    }
    return object;
}

/* 027600EC */
CriticalSection* CriticalSection::constructWithHeap(CriticalSection* object, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<CriticalSection*>(platformAllocateObject(sizeof(CriticalSection)));
    if (object != nullptr) {
        platformConstructHeapDisposer(object, heap, 1);
        object->mVtable = &sCriticalSectionVtable;
        platformClear(&object->mPlatformMutex, sizeof(PlatformMutexStorage));
        platformInitMutex(&object->mPlatformMutex);
    }
    return object;
}

/* 02760168 */
void CriticalSection::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    RegisteredObject::destroy(0);
    if (flags & 1) platformReleaseObject(this);
}

/* 027601BC */
void CriticalSection::lock() {
    gabi::AutoFrame frame{gabi::current_function()}; platformLockMutex(&mPlatformMutex); }

/* 027601C4 */
u32 CriticalSection::tryLock() {
    gabi::AutoFrame frame{gabi::current_function()}; return platformTryLockMutex(&mPlatformMutex) != 0; }

/* 027601F0 */
void CriticalSection::unlock() {
    gabi::AutoFrame frame{gabi::current_function()}; platformUnlockMutex(&mPlatformMutex); }

/* 027601F8 */
MessageQueue* MessageQueue::construct(MessageQueue* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<MessageQueue*>(platformAllocateObject(sizeof(MessageQueue)));
    if (object != nullptr) {
        object->mMessages = nullptr;
        platformClear(&object->mPlatformQueue, sizeof(PlatformMessageQueueStorage));
    }
    return object;
}

/* 0276024C */
void MessageQueue::releaseBuffer() {
    gabi::AutoFrame frame{gabi::current_function()};
    PlatformMessage* messages = mMessages;
    if (messages != nullptr) {
        platformReleaseArray(messages);
        mMessages = nullptr;
    }
}

/* 0276028C */
void MessageQueue::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    releaseBuffer();
    if (flags & 1) platformReleaseObject(this);
}

/* 027602DC */
void MessageQueue::allocate(s32 count, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (count > 0) {
        PlatformMessage* messages = static_cast<PlatformMessage*>(platformAllocateArray(static_cast<u32>(count) * sizeof(PlatformMessage), heap, 4));
        mMessages = messages;
        platformInitMessageQueue(&mPlatformQueue, messages, count);
    }
}

/* 02760338 */
u32 MessageQueue::push(u32 message, u32 nonBlocking) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<PlatformMessage> payload(8);
    payload->mValue = message;
    return platformSendMessage(&mPlatformQueue, payload.get(), nonBlocking == 0) != 0;
}

/* 02760374 */
u32 MessageQueue::pop(u32 nonBlocking) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<PlatformMessage> payload(8);
    u32 received = platformReceiveMessage(&mPlatformQueue, payload.get(), nonBlocking == 0);
    if (received != 0) return payload->mValue;
    return received;
}

/* 027603B0 */
u32 MessageQueue::pushFront(u32 message, u32 nonBlocking) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<PlatformMessage> payload(8);
    payload->mValue = message;
    return platformSendMessage(&mPlatformQueue, payload.get(), (nonBlocking == 0) | 2) != 0;
}

/* 027603F0 */
ThreadMutex* ThreadMutex::construct(ThreadMutex* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<ThreadMutex*>(platformAllocateObject(sizeof(ThreadMutex)));
    if (object != nullptr) {
        platformConstructDisposer(object);
        object->mVtable = &sThreadMutexVtable;
        platformClear(&object->mPlatformMutex, sizeof(PlatformMutexStorage));
        platformInitMutex(&object->mPlatformMutex);
    }
    return object;
}

/* 02760458 */
void ThreadMutex::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    RegisteredObject::destroy(0);
    if (flags & 1) platformReleaseObject(this);
}

/* 027604AC */
void ThreadMutex::lock() {
    gabi::AutoFrame frame{gabi::current_function()}; platformLockMutex(&mPlatformMutex); }

/* 027604B4 */
void ThreadMutex::unlock() {
    gabi::AutoFrame frame{gabi::current_function()}; platformUnlockMutex(&mPlatformMutex); }

/* 0275FBD8 */
DelegateThread* DelegateThread::construct(DelegateThread* object, const SafeText* name,
                                         ThreadDelegate* delegate, RegistryHeap* heap, s32 priority,
                                         u32 messageMode, u32 quitMessage, u32 stackSize, s32 messageCount) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<DelegateThread*>(platformAllocateObject(sizeof(DelegateThread)));
    if (object != nullptr) {
        Thread::construct(object, name, heap, priority, messageMode, quitMessage, stackSize, messageCount);
        object->mDelegate = delegate;
        object->mVtable = reinterpret_cast<DisposerVtable*>(&sDelegateThreadVtable);
    }
    return object;
}

/* 0275FC6C */
void DelegateThread::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    mNameOwnerVtable = &sDelegateNameOwnerVtable;
    Thread::destroy(0);
    if (flags & 1) platformReleaseObject(this);
}

/* 0275FCCC */
void DelegateThread::invoke(u32 message) {
    gabi::AutoFrame frame{gabi::current_function()};
    ThreadDelegate* delegate = mDelegate;
    u32 method = delegate->mVtable->mInvoke.mFunction;
    gabi::call_ptr<void>(method, delegate, this, message);
}

/* 0275FCEC */
void DelegateThread::runVirtual() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::call_ptr<void>(methods()->mRun.mFunction, this);
}

/* 0275FCFC */
ThreadManager* ThreadManager::construct(ThreadManager* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<ThreadManager*>(platformAllocateObject(sizeof(ThreadManager)));
    if (object != nullptr) {
        object->mVtable = &sThreadManagerVtable;
        LinkList* list = &object->mThreads;
        if (gabi::ea(list) == 0) list = static_cast<LinkList*>(platformAllocateObject(sizeof(LinkList)));
        if (gabi::ea(list) != 0) {
            list->mNext = list;
            list->mCount = 0;
            list->mPrevious = list;
        }
        object->mMainThread = nullptr;
        object->mThreadSpecificKey = 0;
        u32 key = 0;
        if (sPrimaryThreadKeyUsed == 0) {
            object->mThreadSpecificKey = 0;
            sPrimaryThreadKeyUsed = 1;
            key = object->mThreadSpecificKey;
        } else if (sSecondaryThreadKeyUsed == 0) {
            object->mThreadSpecificKey = 1;
            sSecondaryThreadKeyUsed = 1;
            key = object->mThreadSpecificKey;
        }
        platformSetThreadSpecific(key, nullptr);
    }
    return object;
}

/* 0275FDD0 */
void ThreadManager::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    Link* sentinel = &mThreads;
    Link* node = mThreads.mNext;
    mVtable = &sThreadManagerVtable;
    if (gabi::ea(node) != gabi::ea(sentinel)) {
        do {
            Thread* thread = static_cast<ThreadNode*>(node)->mThread;
            gabi::call_ptr<void>(thread->methods()->mRequestQuit.mFunction, thread, 0);
            node = node->mNext;
        } while (gabi::ea(node) != gabi::ea(sentinel));
        node = mThreads.mNext;
    }
    for (;;) {
        u32 exited = 1;
        while (gabi::ea(node) != gabi::ea(sentinel)) {
            Thread* thread = static_cast<ThreadNode*>(node)->mThread;
            u32 state = thread->mState;
            node = node->mNext;
            exited &= state == Thread::Exited || state == Thread::Joined;
        }
        Thread::yield();
        node = mThreads.mNext;
        if (exited != 0) break;
    }
    while (gabi::ea(node) != gabi::ea(sentinel)) {
        Thread* thread = static_cast<ThreadNode*>(node)->mThread;
        gabi::call_ptr<void>(thread->methods()->mJoin.mFunction, thread);
        node = node->mNext;
    }
    sThreadManager = nullptr;
    if (mThreadSpecificKey == 0) sPrimaryThreadKeyUsed = 0;
    else sSecondaryThreadKeyUsed = 0;
    if (flags & 1) platformReleaseObject(this);
}

/* 0275FEF8 */
void ThreadManager::attachCurrentForward(RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    attachCurrent(heap);
}

/* 0275FEFC */
u32 Thread::sendMessage(u32 message, u32 nonBlocking) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (message == 0) return 0;
    u32 state = mState;
    if (state == Exited || state == Joined || message == mQuitMessage) return 0;
    return mQueue.push(message, nonBlocking);
}

/* 0275FF34 */
void Thread::requestQuit(u32 pushFront) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 state = mState;
    if (state == Exited || state == Joined) return;
    if (state == Running) {
        mState = Quitting;
        (void)static_cast<u32>(mState);
    }
    u32 message = mQuitMessage;
    if (pushFront != 0) mQueue.pushFront(message, 0);
    else mQueue.push(message, 0);
}

/* 0275FF7C */
void Thread::quitAndJoin(u32 pushFront) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::call_ptr<void>(methods()->mRequestQuit.mFunction, this, pushFront);
    gabi::call_ptr<void>(methods()->mJoin.mFunction, this);
}

/* 0275FFC8 */
void Thread::emptyHook() {}

/* 0275FFCC */
void Thread::run() {
    gabi::AutoFrame frame{gabi::current_function()};
    // HD: frame-exact for cross-build save states: a worker thread waits in one of the two pops, so
    // each carries its own return address and r31 (the thread is taken back from r31 after it).
    Thread* self = this;
    u32 message = gabi::call_site<u32>(0x0275FFEC, {{31, gabi::ea(self)}}, {}, 0x02760374, &self->mQueue, (u32)self->mMessageMode);
    self = gabi::at<Thread>(gabi::cpu->r[31]);
    while (message != self->mQuitMessage) {
        gabi::call_ptr<void>(self->methods()->mProcessMessage.mFunction, self, message);
        message = gabi::call_site<u32>(0x02760020, {{31, gabi::ea(self)}}, {}, 0x02760374, &self->mQueue, (u32)self->mMessageMode);
        self = gabi::at<Thread>(gabi::cpu->r[31]);
    }
}

/* 027604BC */
ThreadManager* ThreadManager::create(RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    ThreadManager* manager = sThreadManager;
    if (manager == nullptr) {
        ThreadManager* storage = static_cast<ThreadManager*>(platformAllocateArray(sizeof(ThreadManager), heap, 4));
        RegisteredObject* disposer = &storage->mDisposer;
        if (gabi::ea(disposer) != 0) {
            platformConstructHeapDisposer(disposer, heap, 3);
            disposer->mVtable = &sThreadManagerDisposerVtable;
        }
        sThreadManagerDisposer = reinterpret_cast<ThreadManagerDisposer*>(disposer);
        manager = storage;
        if (manager != nullptr) manager = construct(manager);
        sThreadManager = manager;
    }
    return manager;
}

/* 0276055C */
Thread* Thread::wrapCurrent(Thread* object, RegistryHeap* heap, PlatformThread* platformThread) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<Thread*>(platformAllocateObject(sizeof(Thread)));
    if (object != nullptr) {
        platformConstructDisposer(object);
        object->mName.mVtable = &sThreadNameVtable;
        object->mNameOwnerVtable = &sThreadNameOwnerVtable;
        object->mName.mText.v = gabi::guest_str("sead::MainThread");
        object->mVtable = reinterpret_cast<DisposerVtable*>(&sThreadVtable);
        MessageQueue::construct(&object->mQueue);
        object->mStackSize = 0;
        clearThreadNode(&object->mNode);
        object->mReservedState = 0;
        object->mActiveThread = platformThread;
        object->mQuitMessage = 0x7FFFFFFF;
        object->mMessageMode = 1;
        object->mState = Running;
        gabi::FrameLocal<be<u32>> unused(8);
        *unused = 0;
        (void)static_cast<u32>(*unused);
        object->mStack = nullptr;
        object->mPlatformThread = platformThread;
        object->mCoreMask = 1;
        object->mPriority = platformGetThreadPriority(platformThread);
        object->mNode.mThread = object;
        object->mQueue.allocate(0x20, heap);
    }
    return object;
}

/* 02760680 */
void ThreadManager::attachCurrent(RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    PlatformThread* current = platformGetCurrentThread();
    MainThread* thread = static_cast<MainThread*>(platformAllocateAlignedObject(sizeof(MainThread), heap, 4));
    if (thread != nullptr) {
        Thread::wrapCurrent(thread, heap, current);
        thread->mVtable = reinterpret_cast<DisposerVtable*>(&sMainThreadVtable);
    }
    mMainThread = thread;
    platformSetThreadSpecific(sThreadManager->mThreadSpecificKey, thread);
}

/* 02760718 */
void ThreadManagerDisposer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    mVtable = &sThreadManagerDisposerVtable;
    if (this == sThreadManagerDisposer) {
        ThreadManager* manager = sThreadManager;
        sThreadManagerDisposer = nullptr;
        gabi::call_ptr<void>(manager->mVtable->mDestroy.mFunction, manager, 2);
        sThreadManager = nullptr;
    }
    RegisteredObject::destroy(0);
    if (flags & 1) platformReleaseObject(this);
}

/* 027607C0 */
Thread* Thread::construct(Thread* object, const SafeText* name, RegistryHeap* heap, s32 priority,
                          u32 messageMode, u32 quitMessage, u32 stackSize, s32 messageCount) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<Thread*>(platformAllocateObject(sizeof(Thread)));
    if (object != nullptr) {
        platformConstructDisposer(object);
        object->mName.mVtable = &sThreadNameVtable;
        be<u8>* text = name->mText;
        object->mVtable = reinterpret_cast<DisposerVtable*>(&sThreadVtable);
        object->mName.mText = text;
        object->mNameOwnerVtable = &sThreadNameOwnerVtable;
        MessageQueue::construct(&object->mQueue);
        object->mStackSize = stackSize;
        clearThreadNode(&object->mNode);
        object->mActiveThread = nullptr;
        object->mQuitMessage = quitMessage;
        object->mReservedState = 0;
        object->mMessageMode = messageMode;
        object->mState = NotStarted;
        gabi::FrameLocal<be<u32>> unused(8);
        *unused = 0;
        (void)static_cast<u32>(*unused);
        object->mNode.mThread = object;
        object->mCoreMask = 1;
        object->mPriority = priority;
        object->mStack = nullptr;
        object->mQueue.allocate(messageCount, heap);
        object->mStack = static_cast<u8*>(platformAllocateArray(stackSize, heap, 4));
        PlatformThread* platformThread = static_cast<PlatformThread*>(platformAllocateAlignedObject(sizeof(PlatformThread), heap, 8));
        if (platformThread != nullptr) platformClear(platformThread, sizeof(PlatformThread));
        object->mActiveThread = platformThread;
        object->mPlatformThread = platformThread;
        ThreadManager* manager = sThreadManager;
        if (manager != nullptr) {
            LinkList* oldList = object->mNode.mList;
            if (oldList != nullptr) {
                object->mNode.mList = nullptr;
                object->mNode.detach();
                oldList->mCount = static_cast<u32>(oldList->mCount) - 1;
            }
            object->mNode.mList = &manager->mThreads;
            manager->mThreads.insertBefore(&object->mNode);
            manager->mThreads.mCount = static_cast<u32>(manager->mThreads.mCount) + 1;
        }
    }
    return object;
}

/* 02760958 */
void Thread::join() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 state = mState;
    if (state != NotStarted && state != Joined) {
        platformJoinThread(mPlatformThread, nullptr);
        mState = Joined;
        (void)static_cast<u32>(mState);
    }
}

/* 027609AC */
void Thread::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    mVtable = reinterpret_cast<DisposerVtable*>(&sThreadVtable);
    mNameOwnerVtable = &sThreadNameOwnerVtable;
    ThreadManager* manager = sThreadManager;
    if (manager != nullptr) {
        if (manager->mMainThread != this) {
            if (mNode.mList != nullptr) {
                mNode.mList = nullptr;
                mNode.detach();
                manager->mThreads.mCount = static_cast<u32>(manager->mThreads.mCount) - 1;
            }
            u32 state = mState;
            if (state == Running) quitAndJoin(0);
            else if (state == Quitting || state == Exited) join();
            platformReleaseObject(mPlatformThread);
            platformReleaseArray(mStack);
        }
        mQueue.releaseBuffer();
    }
    mQueue.destroy(2);
    RegisteredObject::destroy(0);
    if (flags & 1) platformReleaseObject(this);
}

/* 02760ACC */
u32 ThreadManager::threadEntry(u32 argument, Thread* thread) {
    gabi::AutoFrame frame{gabi::current_function()};
    platformSetThreadSpecific(sThreadManager->mThreadSpecificKey, thread);
    // HD: frame-exact for cross-build save states (the thread waits below run()): the original's
    // return address and r31; the thread is taken back from r31.
    gabi::call_site(0x02760AF8, {{31, gabi::ea(thread)}}, {}, 0x0275FFCC, thread);
    thread = gabi::at<Thread>(gabi::cpu->r[31]);
    thread->mState = Thread::Exited;
    (void)static_cast<u32>(thread->mState);
    return 0;
}

/* 02760B1C */
u32 Thread::start() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mState != NotStarted) return 0;
    u8* stack = mStack;
    u32 stackSize = mStackSize;
    s32 priority = mPriority;
    PlatformThread* thread = mPlatformThread;
    u32 created = platformCreateThread(thread, gabi::guest_fn(reinterpret_cast<const void*>(ThreadManager::threadEntry)), 0,
                                       this, stack + stackSize, stackSize, priority, 0);
    if (created == 0) return 0;
    mName.assureTermination();
    platformSetThreadName(mPlatformThread, mName.mText);
    gabi::FrameLocal<be<u32>> core(8);
    gabi::FrameLocal<be<u32>> selected(0xC);
    u32 mask = platformMask(this, core.get(), selected.get());
    platformSetThreadAffinity(mPlatformThread, static_cast<u16>(mask));
    PlatformThread* active = mPlatformThread;
    mActiveThread = active;
    mState = Running;
    (void)static_cast<u32>(mState);
    platformResumeThread(active);
    return 1;
}

/* 02760C3C */
void Thread::sleep(const be<u32>* ticks) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 low = ticks[1];
    u32 high = ticks[0];
    platformSleepTicks(high, low);
}

/* 02760C48 */
void Thread::yield() {
    gabi::AutoFrame frame{gabi::current_function()};
    platformYieldThread();
}

/* 02760C4C */
void Thread::setCoreMask(const be<u32>* coreMask) {
    gabi::AutoFrame frame{gabi::current_function()};
    mCoreMask = *coreMask;
    gabi::FrameLocal<be<u32>> core(8);
    gabi::FrameLocal<be<u32>> selected(0xC);
    u32 mask = platformMask(this, core.get(), selected.get());
    platformSetThreadAffinity(mPlatformThread, static_cast<u16>(mask));
}

/* 02760CF8 */
void Thread::runVirtual() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::call_ptr<void>(methods()->mRun.mFunction, this);
}

/* 02760D08 */
void MainThread::emptyEntryHook() {}
/* 02760D0C */
void MainThread::emptyExitHook() {}
/* 02760D10 */
void MainThread::emptyMessageHook() {}
/* 02760D14 */
void MainThread::emptyStateHook() {}

/* 02760D18 */
void MainThread::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (this == nullptr) return;
    mNameOwnerVtable = &sThreadNameOwnerVtable;
    mVtable = reinterpret_cast<DisposerVtable*>(&sMainThreadVtable);
    mState = Exited;
    (void)static_cast<u32>(mState);
    Thread::destroy(0);
    if (flags & 1) platformReleaseObject(this);
}

struct PlatformSystemInfo { be<u32> mBusClock; };
static PlatformSystemInfo* platformGetSystemInfo();
static Pair32 platformGetSystemTime();
static TickTime sTickFrequency;

/* 02760D90 */
Pair32 TickTime::queryFrequency() {
    gabi::AutoFrame frame{gabi::current_function()};
    PlatformSystemInfo* info = platformGetSystemInfo();
    return {0, static_cast<u32>(info->mBusClock) >> 2};
}

/* 02760DBC */
void TickTime::initializeFrequency() {
    gabi::AutoFrame frame{gabi::current_function()};
    auto frequency = queryFrequency();
    sTickFrequency.mHigh = frequency.r3;
    sTickFrequency.mLow = frequency.r4;
}

/* 02760DE8 */
TickTime* TickTime::construct(TickTime* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<TickTime*>(platformAllocateObject(sizeof(TickTime)));
    if (object != nullptr) {
        auto time = platformGetSystemTime();
        object->mHigh = time.r3;
        object->mLow = time.r4;
    }
    return object;
}

/* 02760E34 */
Pair32 TickTime::subtract(const TickTime* other) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 otherLow = other->mLow;
    u32 high = mHigh;
    u32 low = mLow;
    u32 otherHigh = other->mHigh;
    u64 difference = ((static_cast<u64>(high) << 32) | low) -
                     ((static_cast<u64>(otherHigh) << 32) | otherLow);
    return {static_cast<u32>(difference >> 32), static_cast<u32>(difference)};
}

/* 02760E58 */
Pair32 TickTime::elapsed() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 low = mLow;
    u32 high = mHigh;
    gabi::FrameLocal<TickTime> previous(0x10);
    previous->mLow = low;
    previous->mHigh = high;
    gabi::FrameLocal<TickTime> current(0x18);
    TickTime* now = construct(current.get());
    return now->subtract(previous.get());
}

/* 02760E94 */
void TickTime::add(const TickTime* other) {
    u32 otherLow = other->mLow;
    u32 low = mLow;
    u32 otherHigh = other->mHigh;
    u64 lowSum = static_cast<u64>(low) + otherLow;
    u32 high = mHigh;
    u32 highSum = high + otherHigh + static_cast<u32>(lowSum >> 32);
    mLow = static_cast<u32>(lowSum);
    mHigh = highSum;
}

/* 02760EB8 */
Pair32 TickTime::plus(const TickTime* other) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 high = mHigh;
    u32 low = mLow;
    gabi::FrameLocal<TickTime> left(0x10);
    left->mHigh = high;
    left->mLow = low;
    gabi::FrameLocal<TickTime> right(0x18);
    right->mHigh = other->mHigh;
    right->mLow = other->mLow;
    left->add(right.get());
    low = left->mLow;
    high = left->mHigh;
    return {high, low};
}

static void releaseInlineText(void* object, u32 flags) {
    if (object != nullptr && (flags & 1)) platformReleaseObject(object);
}

/* 0275FB1C */
void ThreadTextEntryPoints::destroyView(SafeText* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 0275FB30 */
void ThreadTextEntryPoints::assureView(SafeText* text) {}
/* 0275FB34 */
void ThreadTextEntryPoints::destroyBuffer(SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 0275FB48 */
void ThreadTextEntryPoints::terminateBuffer(SafeTextBuffer* text) {
    be<u8>* buffer = text->mText;
    u32 capacity = text->mCapacity;
    buffer[capacity - 1] = 0;
}
/* 0275FB60 */
void ThreadTextEntryPoints::destroyBufferStorage(SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 0275FB74 */
void ThreadTextEntryPoints::destroyBufferAdapter(SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 0275FB88 */
void ThreadTextEntryPoints::destroyBufferMember(SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 0275FB9C */
void ThreadTextEntryPoints::destroyTextStorage(SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 0275FBB0 */
void ThreadTextEntryPoints::destroyFixedStorage(SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 0275FBC4 */
void ThreadTextEntryPoints::destroyTextBuffer(SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 02760040 */
void ThreadTextEntryPoints::destroyThreadText(SafeText* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 02760054 */
void ThreadTextEntryPoints::assureThreadText(SafeText* text) {}
/* 02760058 */
void ThreadTextEntryPoints::destroyThreadBuffer(SafeTextBuffer* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 0276006C */
void ThreadTextEntryPoints::terminateThreadBuffer(SafeTextBuffer* text) {
    be<u8>* buffer = text->mText;
    u32 capacity = text->mCapacity;
    buffer[capacity - 1] = 0;
}
/* 02760CE0 */
void ThreadTextEntryPoints::destroyThreadName(SafeText* text, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    releaseInlineText(text, flags);
}
/* 02760CF4 */
void ThreadTextEntryPoints::assureThreadName(SafeText* text) {}

/* WWHD glue
data sCriticalSectionVtable 1015E618
data sThreadMutexVtable 1015E628
data sThreadManagerVtable 1015E600
data sThreadVtable 1015E70C
data sDelegateThreadVtable 1015E528
data sMainThreadVtable 1015E660
data sDelegateNameOwnerVtable 1015E518
data sThreadNameOwnerVtable 1015E650
data sThreadManagerDisposerVtable 1015E6FC
data sThreadNameVtable 1015E638
data sThreadManager 101F8B94
data sThreadManagerDisposer 101F8B98
data sPrimaryThreadKeyUsed 101F8BA0
data sSecondaryThreadKeyUsed 101F8BA1
data sPlatformCoreCount 101F8B5C
data sTickFrequency 104A11C8
str 1015E6E8
ext 0273AD10 void* platformAllocateObject(u32)
ext 0273B0D4 void* platformAllocateArray(u32,sead::RegistryHeap*,s32)
ext 0273B050 void* platformAllocateAlignedObject(u32,sead::RegistryHeap*,s32)
ext 0273AF40 void platformReleaseObject(void*)
ext 0273AFC8 void platformReleaseArray(void*)
ext 02752A84 sead::RegisteredObject* platformConstructDisposer(sead::RegisteredObject*)
ext 02752B0C sead::RegisteredObject* platformConstructHeapDisposer(sead::RegisteredObject*,sead::RegistryHeap*,u32)
ext 02752BEC void sead::RegisteredObject::destroy(u32)
ext 028F521C void platformClear(void*,u32)
ext 0273B2F0 sead::Link* sead::Link::insertBefore(sead::Link*)
ext 0273B310 sead::Link* sead::Link::detach()
ext 02757B98 u32 platformCoreBit(const be<u32>*)
ext C0009D68 void platformInitMutex(sead::PlatformMutexStorage*)
ext C0009E40 void platformLockMutex(sead::PlatformMutexStorage*)
ext C000A198 u32 platformTryLockMutex(sead::PlatformMutexStorage*)
ext C000A1C8 void platformUnlockMutex(sead::PlatformMutexStorage*)
ext C0009D58 void platformInitMessageQueue(sead::PlatformMessageQueueStorage*,sead::PlatformMessage*,s32)
ext C0009FC8 u32 platformSendMessage(sead::PlatformMessageQueueStorage*,sead::PlatformMessage*,u32)
ext C0009ED0 u32 platformReceiveMessage(sead::PlatformMessageQueueStorage*,sead::PlatformMessage*,u32)
ext C000A090 void platformSetThreadSpecific(u32,sead::Thread*)
ext C0009BC8 sead::PlatformThread* platformGetCurrentThread()
ext C0009CC0 s32 platformGetThreadPriority(sead::PlatformThread*)
ext C0009E00 u32 platformJoinThread(sead::PlatformThread*,void*)
ext C0009A48 u32 platformCreateThread(sead::PlatformThread*,u32,u32,void*,u8*,u32,s32,u32)
ext C000A078 void platformSetThreadName(sead::PlatformThread*,const be<u8>*)
ext C000A058 u32 platformSetThreadAffinity(sead::PlatformThread*,u32)
ext C0009F38 s32 platformResumeThread(sead::PlatformThread*)
ext C000A0D0 void platformSleepTicks(u32,u32)
ext C000A248 void platformYieldThread()
ext C0009C80 PlatformSystemInfo* platformGetSystemInfo()
ext C0009C98 Pair32 platformGetSystemTime()
*/

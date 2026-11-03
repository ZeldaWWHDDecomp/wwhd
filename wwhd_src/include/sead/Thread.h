/* sead threads on the Cafe platform: critical sections, thread mutexes, message queues, threads and their
 * manager, and tick time. Layouts reconstructed from this game's constructors and instructions; the SDK
 * objects (mutex, message queue, thread) stay opaque at their platform boundary. */
#pragma once
#include "sead/HeapRegistry.h"
#include "sead/MethodTree.h"

namespace sead {
struct PlatformMutexStorage { u8 mStorage[0x2C]; };
struct PlatformMessage { be<u32> mValue; be<u32> mArguments[3]; };
struct PlatformMessageQueueStorage { u8 mStorage[0x3C]; };
/* the Cafe OSThread; only its stack bounds are read (the stack grows down from mStackStart) */
struct PlatformThread {
    u8 mReserved[0x394];
    be<u32> mStackStart;
    be<u32> mStackEnd;
    u8 mReserved39C[0x6A0 - 0x39C];
};

struct CriticalSection : RegisteredObject {
    PlatformMutexStorage mPlatformMutex;
    static CriticalSection* construct(CriticalSection* object);
    static CriticalSection* constructWithHeap(CriticalSection* object, RegistryHeap* heap);
    void destroy(u32 flags);
    void lock();
    u32 tryLock();
    void unlock();
};
struct ThreadMutex : RegisteredObject {
    PlatformMutexStorage mPlatformMutex;
    static ThreadMutex* construct(ThreadMutex* object);
    void destroy(u32 flags);
    void lock();
    void unlock();
};
struct MessageQueue {
    gptr<PlatformMessage> mMessages;
    PlatformMessageQueueStorage mPlatformQueue;
    static MessageQueue* construct(MessageQueue* object);
    void releaseBuffer();
    void destroy(u32 flags);
    void allocate(s32 count, RegistryHeap* heap);
    u32 push(u32 message, u32 nonBlocking);
    u32 pop(u32 nonBlocking);
    u32 pushFront(u32 message, u32 nonBlocking);
};

struct Thread;
struct ThreadVtable {
    VtableSlot mHeader;
    VtableSlot mDestroy;
    VtableSlot mReservedBeforeTick[3];
    VtableSlot mTick;               // called once per audio frame on the audio system's auxiliary worker
    VtableSlot mRequestQuit;
    VtableSlot mJoin;
    VtableSlot mReservedBeforeRun;
    VtableSlot mRun;
    VtableSlot mReservedBeforeMessage[5];
    VtableSlot mProcessMessage;
};
struct ThreadNode : Link {
    gptr<Thread> mThread;
    gptr<LinkList> mList;
};
struct Thread : RegisteredObject {
    SafeText mName;
    gptr<DisposerVtable> mNameOwnerVtable;
    MessageQueue mQueue;
    be<u32> mStackSize;
    ThreadNode mNode;
    be<u32> mReservedState;
    be<u32> mMessageMode;
    be<u32> mQuitMessage;
    gptr<PlatformThread> mActiveThread;
    be<u32> mState;
    be<u32> mCoreMask;
    gptr<PlatformThread> mPlatformThread;
    gptr<u8> mStack;
    be<s32> mPriority;
    enum State { NotStarted, Running, Quitting, Exited, Joined };
    ThreadVtable* methods() { return reinterpret_cast<ThreadVtable*>(mVtable.get()); }
    static Thread* wrapCurrent(Thread* object, RegistryHeap* heap, PlatformThread* platformThread);
    static Thread* construct(Thread* object, const SafeText* name, RegistryHeap* heap, s32 priority,
                             u32 messageMode, u32 quitMessage, u32 stackSize, s32 messageCount);
    void destroy(u32 flags);
    void join();
    u32 start();
    u32 sendMessage(u32 message, u32 nonBlocking);
    void requestQuit(u32 pushFront);
    void quitAndJoin(u32 pushFront);
    void run();
    void runVirtual();
    void emptyHook();
    static void sleep(const be<u32>* ticks);
    static void yield();
    void setCoreMask(const be<u32>* coreMask);
};
struct ThreadDelegateVtable { VtableSlot mHeader; VtableSlot mInvoke; };
struct ThreadDelegate { gptr<ThreadDelegateVtable> mVtable; };
struct DelegateThread : Thread {
    gptr<ThreadDelegate> mDelegate;
    static DelegateThread* construct(DelegateThread* object, const SafeText* name,
                                    ThreadDelegate* delegate, RegistryHeap* heap, s32 priority,
                                    u32 messageMode, u32 quitMessage, u32 stackSize, s32 messageCount);
    void destroy(u32 flags);
    void invoke(u32 message);
    void runVirtual();
};
struct MainThread : Thread {
    void destroy(u32 flags);
    void emptyEntryHook();
    void emptyExitHook();
    void emptyMessageHook();
    void emptyStateHook();
};
struct ThreadManagerVtable { VtableSlot mHeader; VtableSlot mReserved; VtableSlot mDestroy; };
struct ThreadManager {
    gptr<ThreadManagerVtable> mVtable;
    RegisteredObject mDisposer;
    LinkList mThreads;
    gptr<Thread> mMainThread;
    be<u32> mThreadSpecificKey;
    static ThreadManager* construct(ThreadManager* object);
    static ThreadManager* create(RegistryHeap* heap);
    void destroy(u32 flags);
    void attachCurrent(RegistryHeap* heap);
    void attachCurrentForward(RegistryHeap* heap);
    static u32 threadEntry(u32 argument, Thread* thread);
};
struct ThreadManagerDisposer : RegisteredObject { void destroy(u32 flags); };

struct TickTime {
    be<u32> mHigh;
    be<u32> mLow;
    static Pair32 queryFrequency();
    static void initializeFrequency();
    static TickTime* construct(TickTime* object);
    Pair32 subtract(const TickTime* other);
    Pair32 elapsed();
    void add(const TickTime* other);
    Pair32 plus(const TickTime* other);
};

/* Compiler-kept out-of-line copies of the text-object inlines in the thread translation units. The names
 * tell the copies apart; they infer neither an inline buffer capacity nor a complete derived layout. */
struct ThreadTextEntryPoints {
    static void destroyView(SafeText* text, u32 flags);
    static void assureView(SafeText* text);
    static void destroyBuffer(SafeTextBuffer* text, u32 flags);
    static void terminateBuffer(SafeTextBuffer* text);
    static void destroyBufferStorage(SafeTextBuffer* text, u32 flags);
    static void destroyBufferAdapter(SafeTextBuffer* text, u32 flags);
    static void destroyBufferMember(SafeTextBuffer* text, u32 flags);
    static void destroyTextStorage(SafeTextBuffer* text, u32 flags);
    static void destroyFixedStorage(SafeTextBuffer* text, u32 flags);
    static void destroyTextBuffer(SafeTextBuffer* text, u32 flags);
    static void destroyThreadText(SafeText* text, u32 flags);
    static void assureThreadText(SafeText* text);
    static void destroyThreadBuffer(SafeTextBuffer* text, u32 flags);
    static void terminateThreadBuffer(SafeTextBuffer* text);
    static void destroyThreadName(SafeText* text, u32 flags);
    static void assureThreadName(SafeText* text);
};

WWHD_SIZE(PlatformMutexStorage, 0x2C);
WWHD_SIZE(PlatformMessage, 0x10);
WWHD_SIZE(PlatformMessageQueueStorage, 0x3C);
WWHD_SIZE(PlatformThread, 0x6A0);
WWHD_OFFSET(PlatformThread, mStackStart, 0x394);
WWHD_OFFSET(PlatformThread, mStackEnd, 0x398);
WWHD_OFFSET(CriticalSection, mPlatformMutex, 0x10);
WWHD_SIZE(CriticalSection, 0x3C);
WWHD_OFFSET(ThreadMutex, mPlatformMutex, 0x10);
WWHD_SIZE(ThreadMutex, 0x3C);
WWHD_OFFSET(MessageQueue, mMessages, 0);
WWHD_OFFSET(MessageQueue, mPlatformQueue, 4);
WWHD_SIZE(MessageQueue, 0x40);
WWHD_OFFSET(ThreadVtable, mTick, 0x28);
WWHD_OFFSET(ThreadVtable, mRequestQuit, 0x30);
WWHD_OFFSET(ThreadVtable, mJoin, 0x38);
WWHD_OFFSET(ThreadVtable, mRun, 0x48);
WWHD_OFFSET(ThreadVtable, mProcessMessage, 0x78);
WWHD_SIZE(ThreadNode, 0x10);
WWHD_OFFSET(Thread, mName, 0x10);
WWHD_OFFSET(Thread, mQueue, 0x1C);
WWHD_OFFSET(Thread, mStackSize, 0x5C);
WWHD_OFFSET(Thread, mNode, 0x60);
WWHD_OFFSET(Thread, mReservedState, 0x70);
WWHD_OFFSET(Thread, mMessageMode, 0x74);
WWHD_OFFSET(Thread, mQuitMessage, 0x78);
WWHD_OFFSET(Thread, mActiveThread, 0x7C);
WWHD_OFFSET(Thread, mState, 0x80);
WWHD_OFFSET(Thread, mCoreMask, 0x84);
WWHD_OFFSET(Thread, mPlatformThread, 0x88);
WWHD_OFFSET(Thread, mStack, 0x8C);
WWHD_OFFSET(Thread, mPriority, 0x90);
WWHD_SIZE(Thread, 0x94);
WWHD_OFFSET(DelegateThread, mDelegate, 0x94);
WWHD_SIZE(DelegateThread, 0x98);
WWHD_OFFSET(ThreadManager, mDisposer, 4);
WWHD_OFFSET(ThreadManager, mThreads, 0x14);
WWHD_OFFSET(ThreadManager, mMainThread, 0x20);
WWHD_OFFSET(ThreadManager, mThreadSpecificKey, 0x24);
WWHD_SIZE(ThreadManager, 0x28);
WWHD_OFFSET(TickTime, mHigh, 0);
WWHD_OFFSET(TickTime, mLow, 4);
WWHD_SIZE(TickTime, 8);
} // namespace sead

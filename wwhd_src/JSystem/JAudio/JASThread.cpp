// HD: Cafe JAudio thread interface, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASThread.h"
using namespace gabi;
struct AffinityFlags { be<u8> values[3]; };
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
using ThreadName = gptr<const char>;
static AffinityFlags sAffinityFlags;
static Initialization sInitialization;
static Registration sRegistration;
static ThreadName sThreadName;
static JASThreadVTable sVtable;
extern void* allocateThreadObject(u32);
extern void* allocateThreadArray(u32, void*, u32);
extern void* allocateThreadStorage(u32, void*, u32);
extern void initializeThreadQueue(JASMessageQueue*, void*, u32);
extern u32 createAudioThread(void*, JASThreadEntry, s32, JASThread*, u8*, u32, s32, u32);
extern void setAudioThreadName(void*, const char*);
extern void registerThreadGlobals(void*);
/* 02810D20 */
void* JASThread_entry(s32, JASThread* thread) {
    // HD: the original tail-branches (bctr) without a frame: run() starts at the thread's initial sp
    // with its LR (cross-build save states: the audio threads wait below it).
    return gabi::tail_ptr<void*>(thread->mVtable->run.get(), thread);
}
/* 02810D34 */
JASThread* JASThread_construct(JASThread* thread, void* heap, u32 stackSize, u32 messageCount, s32 priority, u32 affinity) {
    if (!thread) {
        thread = static_cast<JASThread*>(allocateThreadObject(sizeof(JASThread)));
        if (!thread)
            return nullptr;
    }
    thread->mHeap = heap;
    thread->mMessages = nullptr;
    stackSize &= ~31u;
    thread->mStackSize = stackSize;
    thread->mThread = nullptr;
    thread->mStack = nullptr;
    thread->mMessageCount = 0;
    thread->mVtable = &sVtable;
    thread->mStack = static_cast<u8*>(allocateThreadArray(stackSize, heap, 32));
    void* currentHeap = thread->mHeap;
    void* storage = allocateThreadStorage(0x6A0, currentHeap, 32);
    thread->mThread = storage;
    thread->mMessageCount = messageCount;
    void* messages = allocateThreadArray(messageCount * 16, heap, 32);
    u32 count = thread->mMessageCount;
    thread->mMessages = messages;
    initializeThreadQueue(&thread->mQueue, messages, count);
    u32 flags = affinity <= 2 ? (u8)sAffinityFlags.values[affinity] : 0;
    void* nativeThread = thread->mThread;
    u8* stack = thread->mStack;
    u32 bytes = thread->mStackSize;
    createAudioThread(nativeThread, JASThread_entry, 0, thread, stack + bytes, bytes, priority, (flags | 8) & 0xFFFF);
    nativeThread = thread->mThread;
    setAudioThreadName(nativeThread, sThreadName);
    return thread;
}
/* 02810E50 */
void __sinit_JASThread_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerThreadGlobals(&sRegistration);
}
/* 02810E78 */
void* JASThread::run() {
    return nullptr;
}
/* WWHD glue
data sAffinityFlags 1016FEA4
data sVtable 1016FEAC
data sThreadName 101F9DD0
data sInitialization 104B5294
data sRegistration 101F9DC4
ext 0273AD10 void* allocateThreadObject(u32)
ext 0273B0D4 void* allocateThreadArray(u32, void*, u32)
ext 0273B050 void* allocateThreadStorage(u32, void*, u32)
ext C0009D58 void initializeThreadQueue(JASMessageQueue*, void*, u32)
ext C0009A48 u32 createAudioThread(void*, JASThreadEntry, s32, JASThread*, u8*, u32, s32, u32)
ext C000A078 void setAudioThreadName(void*, const char*)
ext 028F026C void registerThreadGlobals(void*)
*/

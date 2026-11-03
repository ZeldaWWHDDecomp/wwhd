/* WWHD sead memory helper: an address is on a stack when it lies on the main thread's stack or on the stack
 * of a thread in the thread manager's list.
 * Reconstructed from this game's instructions. */
#include "sead/MemoryStack.h"

using namespace sead;

static ThreadManager* sThreadManager;

/* inlined: within [end, start) of the thread's platform stack */
static bool isOnThreadStack(Thread* thread, u32 address) {
    PlatformThread* platformThread = thread->mPlatformThread;
    if (address < platformThread->mStackEnd) return false;
    return address < platformThread->mStackStart;
}

/* 0275CC9C */
bool sead::memory::isStackAddress(const void* address) {
    ThreadManager* manager = sThreadManager;
    if (manager == NULL) return false;
    u32 where = gabi::ea(address);
    Thread* mainThread = manager->mMainThread;
    if (mainThread != NULL && isOnThreadStack(mainThread, where)) return true;
    for (Link* link = manager->mThreads.mNext; link != &manager->mThreads; link = link->mNext) {
        if (isOnThreadStack(static_cast<ThreadNode*>(link)->mThread, where)) return true;
    }
    return false;
}

/* WWHD glue
 data sThreadManager 101F8B94
*/

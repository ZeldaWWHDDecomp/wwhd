/* The background audio worker's thread loop, reconstructed from WWHD. */
#include "nw/snd/AudioWorkQueue.h"
#include "nw/snd/DriverCommandQueue.h"
using namespace nw::snd;
void OSLockMutex(void* mutex);
void OSUnlockMutex(void* mutex);

/* 0289C998 */
void AudioWorker::run() {
    gabi::AutoFrame frame{0x0289C998u};
    // HD: frame-exact for cross-build save states (the task thread waits below this call): r31 = the
    // worker and, from the second pass on, r30 = its mutex, as the original keeps them.
    bool first = true;
    while (!stopRequested) {
        if (first) gabi::call_site(0x0289C9C4, {{31, gabi::ea(this)}}, {}, 0x0289C61C, AudioWorkQueue::instance());
        else gabi::call_site(0x0289C9C4, {{30, gabi::ea(this) + 0x6D8}, {31, gabi::ea(this)}}, {}, 0x0289C61C, AudioWorkQueue::instance());
        first = false;
        if (stopRequested) break;
        OSLockMutex(mutex);
        AudioWorkQueue::instance()->executePending();
        OSUnlockMutex(mutex);
        DriverCommandManager::taskThreadInstance()->collect();
    }
}

/* WWHD glue
ext C0009E40 void OSLockMutex(void*)
ext C000A1C8 void OSUnlockMutex(void*)
ext 0289C1A4 nw::snd::AudioWorkQueue* nw::snd::AudioWorkQueue::instance()
ext 0289C61C void nw::snd::AudioWorkQueue::waitForWork()
ext 0289C3D8 void nw::snd::AudioWorkQueue::executePending()
ext 02885F68 nw::snd::DriverCommandManager* nw::snd::DriverCommandManager::taskThreadInstance()
ext 02887158 void nw::snd::DriverCommandManager::collect()
*/

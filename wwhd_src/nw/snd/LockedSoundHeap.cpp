/* NW-SND#6: mutex-protected sound heap operations.
 * Written from WWHD's instructions; verified by tools/verify (spec lib2_nw_snd_6). */
#include "nw/snd/LockedSoundHeap.h"
using namespace nw::snd;
static u32 sLockedHeapVtable[6] = {0,0,0,0x02896F04,0,0x028970C4};
/* 02896DBC */
LockedSoundHeap* LockedSoundHeap::create(LockedSoundHeap* storage) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (storage == NULL) storage = (LockedSoundHeap*)utAllocate(sizeof(LockedSoundHeap));
    if (storage != NULL) {
        storage->alive = 1;
        storage->vtable = gabi::ea(sLockedHeapVtable);
        OSInitMutex(storage->mutex);
        SoundHeap::create(&storage->heap);
    }
    return storage;
}
/* 02896F04 */
void LockedSoundHeap::destroy(LockedSoundHeap* storage, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (storage != NULL) {
        storage->vtable = gabi::ea(sLockedHeapVtable);
        storage->finalize();
        storage->heap.destroy(2);
        storage->alive = 0;
        if (flags & 1) utDelete(storage);
    }
}
/* 02896E20 */
void LockedSoundHeap::reset() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (heap.allocator == NULL) return;
    OSLockMutex(mutex);
    heap.reset();
    OSUnlockMutex(mutex);
    DriverCommandManager* manager = DriverCommandManager::instance();
    if (manager->initialized != 0) {
        u32 serial = manager->submit(1);
        manager->waitFor(serial);
    }
}
/* 02896EA4 */
void LockedSoundHeap::finalize() {
    gabi::AutoFrame frame{gabi::current_function()};
    OSLockMutex(mutex);
    if (heap.allocator != NULL) {
        reset();
        heap.finalize();
    }
    OSUnlockMutex(mutex);
}
/* 02896F74 */
u32 LockedSoundHeap::initialize(u32 buffer, u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    OSLockMutex(mutex);
    u32 result = heap.initialize(buffer, size);
    OSUnlockMutex(mutex);
    return result;
}
/* 02896FE8 */
void* LockedSoundHeap::allocate(u32 size, u32 callback, u32 argument) {
    gabi::AutoFrame frame{gabi::current_function()};
    OSLockMutex(mutex);
    void* result = heap.allocate(size, callback, argument);
    OSUnlockMutex(mutex);
    return result;
}
/* 0289704C */
static void enqueueHeapRelease(LockedSoundHeap* owner, void* data) {
    gabi::AutoFrame frame{gabi::current_function()};
    DriverCommandManager* manager = DriverCommandManager::instance();
    if (manager->initialized != 0) {
        HeapReleaseCommand* command = (HeapReleaseCommand*)manager->allocCommand(sizeof(HeapReleaseCommand));
        command->data = data;
        command->owner = owner;
        command->id = HeapReleaseCommand_Id;
        manager->pushCommand(command);
    }
}
/* 028970C4 */
void* LockedSoundHeap::allocateWithRelease(u32 size) {
    gabi::AutoFrame frame{gabi::current_function()};
    OSLockMutex(mutex);
    void* result = allocate(size, gabi::guest_fn((const void*)enqueueHeapRelease), 0);
    OSUnlockMutex(mutex);
    return result;
}
/* WWHD glue
 data sLockedHeapVtable 10182270
 ext C0009D68 void OSInitMutex(void*)
 ext 02887768 nw::snd::SoundHeap* nw::snd::SoundHeap::create(nw::snd::SoundHeap*)
 ext 02887AE4 void nw::snd::SoundHeap::destroy(u32)
 ext C0009E40 void OSLockMutex(void*)
 ext C000A1C8 void OSUnlockMutex(void*)
 ext 02887E10 void nw::snd::SoundHeap::reset()
 ext 02887A8C void nw::snd::SoundHeap::finalize()
 ext 02887C84 u32 nw::snd::SoundHeap::initialize(u32,u32)
 ext 02887D70 void* nw::snd::SoundHeap::allocate(u32,u32,u32)
*/

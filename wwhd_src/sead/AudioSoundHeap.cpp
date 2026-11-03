/* sead sound heap: a locked nw::snd heap in a block of a sead heap, which the archive loader allocates sound data
 * from. Reconstructed from this game's instructions; the embedded nw::snd::SoundHeap uses the canonical nw/snd
 * header, the parked nw::snd base is called by address; OS mutex argument facts from wut. No external
 * implementation used. */
#include "sead/AudioArchive.h"
using namespace sead;

static void* allocateObject(u32 size);
static void* allocateArray(u32 size, RegistryHeap* heap, s32 alignment);
static void freeArray(void* memory);
static void releaseObject(void* object);
static void osLockMutex(void* mutex);
static void osUnlockMutex(void* mutex);
/* the parked nw::snd base of the sound heap */
static void constructLockedHeap(SoundHeap* heap);
static void finalizeLockedHeap(SoundHeap* heap);
static void destroyLockedHeap(SoundHeap* heap, u32 flags);
static u32 initializeLockedHeap(SoundHeap* heap, void* buffer, u32 size);

static SoundHeapVtable sSoundHeapVtable;
static DisposerVtable sSoundHeapAllocatorVtable;
static SoundHeapText sSoundHeapName;
static SafeTextVtable sSoundHeapTextVtable;

/* the work block must not straddle one of the 512 MB regions of the address space */
static const u32 kMemoryRegionMask = 0xE0000000;
static const s32 kWorkAlignment = 64;

/* 02764DE8 */
u32 SoundHeap::crossesMemoryBoundary() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 work = gabi::ea((void*)mWork);
    if (work == 0) return 0;
    osLockMutex(mMutex);
    u32 size = mHeap.totalSize();
    osUnlockMutex(mMutex);
    return ((work ^ (work + size)) & kMemoryRegionMask) != 0;
}

/* 02764E80 */
u32 SoundHeap::allocateWork(u32 size, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (heap == nullptr) heap = registryCurrentHeap(sRegistryManager);
    if (size == 0)
        size = gabi::call_ptr<u32>(heap->dispatch()->mMaxAllocation.mFunction, heap, (u32)kWorkAlignment) - kWorkAlignment;
    void* work = allocateArray(size, heap, kWorkAlignment);
    mWork = work;
    initializeLockedHeap(this, work, size);
    return crossesMemoryBoundary();
}

/* 02764F24 */
SoundHeap* SoundHeap::construct(SoundHeap* object, u32 size, RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == nullptr) object = static_cast<SoundHeap*>(allocateObject(sizeof(SoundHeap)));
    if (object != nullptr) {
        constructLockedHeap(object);
        object->mAllocatorVtable = &sSoundHeapAllocatorVtable;
        object->mVtable = &sSoundHeapVtable;
        object->mArchive = nullptr;
        object->mWork = nullptr;
        object->mLoader = nullptr;
        object->allocateWork(size, heap);
    }
    return object;
}

/* 02764FBC */
void SoundHeap::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mVtable = &sSoundHeapVtable;
    mAllocatorVtable = &sSoundHeapAllocatorVtable;
    finalizeLockedHeap(this);
    void* work = mWork;
    if (work != nullptr) freeArray(work);
    destroyLockedHeap(this, 0);
    if (flags & 1) releaseObject(this);
}

/* 02765040 */
void SoundHeap::setLoader(AudioArchiveLoader* loader, nw::snd::SoundArchive* archive) {
    gabi::AutoFrame frame{gabi::current_function()};
    mLoader = loader;
    mArchive = archive;
}

/* 0276504C */
static void initializeSoundHeapName() {
    gabi::AutoFrame frame{gabi::current_function()};
    sSoundHeapName.mVtable = &sSoundHeapTextVtable;
    sSoundHeapName.mText.v = gabi::guest_str("SoundHeap");
}

/* HD: compiler-generated and template code of this translation unit */

/* 02765070 */
void SoundHeapText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 02765084 */
void SoundHeapText::assureTermination() {
    gabi::AutoFrame frame{gabi::current_function()};
}

/* 02765088 */
void SoundHeapTextBuffer::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) releaseObject(this);
}

/* 0276509C */
void SoundHeapTextBuffer::assureTermination() {
    gabi::AutoFrame frame{gabi::current_function()};
    /* the last byte of the buffer, by guest address arithmetic (capacity is not checked) */
    *gabi::at<be<u8>>(gabi::ea((be<u8>*)mText) + mCapacity - 1) = 0;
}

/* WWHD glue
data sSoundHeapVtable 1015F02C
data sSoundHeapAllocatorVtable 1015F044
data sSoundHeapName 104A11F8
data sSoundHeapTextVtable 1015EFD0
str 1015F020 "SoundHeap"
ext 0273AD10 void* allocateObject(u32)
ext 0273B0D4 void* allocateArray(u32,sead::RegistryHeap*,s32)
ext 0273AFC8 void freeArray(void*)
ext 0273AF40 void releaseObject(void*)
ext 02756140 sead::RegistryHeap* registryCurrentHeap(void*)
ext C0009E40 void osLockMutex(void*)
ext C000A1C8 void osUnlockMutex(void*)
ext 02896DBC void constructLockedHeap(sead::SoundHeap*)
ext 02896EA4 void finalizeLockedHeap(sead::SoundHeap*)
ext 02896F04 void destroyLockedHeap(sead::SoundHeap*,u32)
ext 02896F74 u32 initializeLockedHeap(sead::SoundHeap*,void*,u32)
ext 02887E50 u32 nw::snd::SoundHeap::totalSize()
*/

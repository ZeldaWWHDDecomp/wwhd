/* Singleton storage and lifetime of the AGL capture provider. */
#include "agl/CaptureProvider.h"
static agl::CaptureProvider* sCaptureProvider;
static agl::CaptureProvider* sAllocatedCaptureProvider;
/* Address-only views: constructors install these image tables, never alter them. */
static sead::DisposerVtable sCaptureDisposerTable;
static sead::DisposerVtable sCaptureInterfaceTable;
static sead::DisposerVtable sCaptureFileHandleTable;
static sead::SafeTextVtable sCaptureTextTable;
constexpr u32 kCaptureEntryCapacity = 0x1000u;
/* 027C5060 */
agl::CaptureProvider* agl::CaptureProvider::construct(CaptureProvider* object) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) {
        object = static_cast<CaptureProvider*>(allocateCaptureStorage(sizeof(CaptureProvider)));
        if (!object) return nullptr;
    }
    object->mHostDevice = nullptr;
    object->mInterfaceTable = &sCaptureInterfaceTable;
    /* A zero modular member address selects the original out-of-line fallback. */
    CaptureEntryBuffer* entries = &object->mEntries;
    if (gabi::ea(entries) == 0)
        entries = static_cast<CaptureEntryBuffer*>(allocateCaptureStorage(sizeof(CaptureEntryBuffer)));
    if (entries) {
        entries->mEntries = nullptr;
        entries->mCount = 0;
    }
    constructCaptureCriticalSection(&object->mCriticalSection);
    return object;
}
/* 027C50E4 */
agl::CaptureProvider* agl::CaptureProvider::instance(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    CaptureProvider* result = sCaptureProvider;
    if (!result) {
        CaptureProvider* allocated = static_cast<CaptureProvider*>(allocateCaptureStorageFromHeap(sizeof(CaptureProvider), heap, 4));
        if (allocated) {
            registerCaptureObject(allocated, heap, 3);
            allocated->mVtable = &sCaptureDisposerTable;
        }
        sAllocatedCaptureProvider = allocated;
        result = allocated ? construct(allocated) : nullptr;
        sCaptureProvider = result;
    }
    return result;
}
/* 027C5178 */
void agl::CaptureProvider::destroy(CaptureProvider* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object) {
        destroyCaptureCriticalSection(&object->mCriticalSection, 2);
        if (flags & 1) deleteCaptureStorage(object);
    }
}
/* 027C51CC */
void agl::CaptureProvider::initializeHostCapture(sead::RegistryHeap* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::RegistryHeap* activeHeap = heap;
    if (!activeHeap) activeHeap = captureCurrentHeap(sRegistryManager);
    CaptureEntry* entries = gabi::call_ptr<CaptureEntry*>(
        activeHeap->dispatch()->mAllocate.mFunction, activeHeap, 0x8000u, 4);
    u32 end;
    if (!entries) {
        u32 count = mEntries.mCount;
        entries = mEntries.mEntries;
        end = gabi::ea(entries) + count * sizeof(CaptureEntry);
    } else {
        end = gabi::ea(entries) + kCaptureEntryCapacity * sizeof(CaptureEntry);
        mEntries.mEntries = entries;
        mEntries.mCount = kCaptureEntryCapacity;
    }
    for (CaptureEntry* entry = entries; gabi::ea(entry) != end; ++entry) {
        entry->mFirst = 0;
        entry->mSecond = 0;
    }
    sead::CafeHostFileDevice* device = static_cast<sead::CafeHostFileDevice*>(
        allocateCaptureDevice(sizeof(sead::CafeHostFileDevice), heap, 4));
    if (device) device = sead::CafeHostFileDevice::construct(device);
    mHostDevice = device;
    sead::FileDeviceManager* manager = sFileDeviceManager;
    gabi::FrameLocal<sead::SafeText> driveName(8);
    driveName->mVtable = &sCaptureTextTable;
    driveName->mText.v = gabi::guest_str("agl::hostpc");
    manager->mount(device, &*driveName);
}
/* 027C5740 */
void agl::CaptureProvider::destroyRegisteredPart(CaptureProvider* object, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!object) return;
    object->mVtable = &sCaptureDisposerTable;
    if (object == sAllocatedCaptureProvider) {
        CaptureProvider* current = sCaptureProvider;
        sAllocatedCaptureProvider = nullptr;
        u32 destroy = current->mInterfaceTable->mDestroy.mFunction;
        gabi::call_ptr<void>(destroy, current, 2);
        sCaptureProvider = nullptr;
    }
    object->sead::RegisteredObject::destroy(0);
    if (flags & 1) deleteCaptureStorage(object);
}
/* 027C58DC */
static void destroyCaptureFileHandle(sead::FileHandle* handle, u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!handle) return;
    sead::FileDevice* device = handle->mSourceDevice;
    handle->mDisposer.mVtable = &sCaptureFileHandleTable;
    if (device) device->tryClose(handle);
    /* The canonical handle embeds the16-byte disposer-compatible registered prefix. */
    reinterpret_cast<sead::RegisteredObject*>(&handle->mDisposer)->destroy(0);
    if (flags & 1) deleteCaptureStorage(handle);
}
/* WWHD glue
str 1016A3F0 "agl::hostpc"
data sCaptureFileHandleTable 1016A398
data sCaptureTextTable 1016A368
data sCaptureDisposerTable 1016A470
data sCaptureInterfaceTable 1016A480
data sCaptureProvider 101F95F8
data sAllocatedCaptureProvider 101F95FC
ext 02752BEC void sead::RegisteredObject::destroy(u32)
ext 02740BE0 u32 sead::FileDevice::tryClose(sead::FileHandle*)
ext 02756140 sead::RegistryHeap* captureCurrentHeap(sead::RegistryManagerState*)
ext 0273B050 void* allocateCaptureDevice(u32, sead::RegistryHeap*, s32)
ext 02743DA0 sead::CafeHostFileDevice* sead::CafeHostFileDevice::construct(sead::CafeHostFileDevice*)
ext 02741770 void sead::FileDeviceManager::mount(sead::FileDevice*, sead::SafeText*)
ext 0273AD10 void* allocateCaptureStorage(u32)
ext 0273B0D4 void* allocateCaptureStorageFromHeap(u32, sead::RegistryHeap*, s32)
ext 02752B0C void registerCaptureObject(sead::RegisteredObject*, sead::RegistryHeap*, u32)
ext 02760084 sead::CriticalSection* constructCaptureCriticalSection(sead::CriticalSection*)
ext 02760168 void destroyCaptureCriticalSection(sead::CriticalSection*, u32)
ext 0273AF40 void deleteCaptureStorage(void*)
*/

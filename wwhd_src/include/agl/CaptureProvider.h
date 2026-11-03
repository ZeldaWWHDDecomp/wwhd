/* Capture-provider storage reconstructed from AGL#17 instructions only. */
#pragma once
#include "sead/Thread.h"
#include "sead/CafeFileDevices.h"
namespace agl {
struct CaptureFileOptions;
struct CaptureEntry { be<u32> mFirst; be<u32> mSecond; };
struct CaptureEntryBuffer { be<u32> mCount; gptr<CaptureEntry> mEntries; };
struct CaptureProvider : sead::RegisteredObject {
    gptr<sead::CafeHostFileDevice> mHostDevice;
    CaptureEntryBuffer mEntries;
    sead::CriticalSection mCriticalSection;
    gptr<sead::DisposerVtable> mInterfaceTable;
    static CaptureProvider* construct(CaptureProvider* object);
    static CaptureProvider* instance(sead::RegistryHeap* heap);
    static void destroy(CaptureProvider* object, u32 flags);
    static void destroyRegisteredPart(CaptureProvider* object, u32 flags);
    void initializeHostCapture(sead::RegistryHeap* heap);
    u32 writeCaptureFile(const void* data, u32 size, CaptureFileOptions* options);
};
WWHD_SIZE(CaptureEntry, 8);
WWHD_SIZE(CaptureEntryBuffer, 8);
WWHD_OFFSET(CaptureProvider, mHostDevice, 0x10);
WWHD_OFFSET(CaptureProvider, mEntries, 0x14);
WWHD_OFFSET(CaptureProvider, mCriticalSection, 0x1C);
WWHD_OFFSET(CaptureProvider, mInterfaceTable, 0x58);
WWHD_SIZE(CaptureProvider, 0x5C);
}
void* allocateCaptureStorage(u32 size);
void* allocateCaptureStorageFromHeap(u32 size, sead::RegistryHeap* heap, s32 alignment);
void registerCaptureObject(sead::RegisteredObject*, sead::RegistryHeap*, u32 flags);
sead::CriticalSection* constructCaptureCriticalSection(sead::CriticalSection*);
void destroyCaptureCriticalSection(sead::CriticalSection*, u32 flags);
void deleteCaptureStorage(void*);
sead::RegistryHeap* captureCurrentHeap(sead::RegistryManagerState*);
void* allocateCaptureDevice(u32, sead::RegistryHeap*, s32);

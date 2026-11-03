/* HD driver callback lists. The Cafe lists hold sixteen callback/data pairs and a mutex. */
#pragma once
#include "bindings.h"
struct DriverCallbackEntry {
    be<u32> mCallback;
    gptr<void> mData;
};
struct DriverCallbackMutex { u8 mStorage[0x3C]; };
class DriverCallbackManager {
public:
    enum { CALLBACK_COUNT = 16 };
    u32 add(u32 callback, void* data);
    void process();
    DriverCallbackEntry mEntries[CALLBACK_COUNT];
    DriverCallbackMutex mMutex;
};
WWHD_SIZE(DriverCallbackEntry,8);
WWHD_OFFSET(DriverCallbackEntry,mData,4);
WWHD_OFFSET(DriverCallbackManager,mMutex,0x80);
WWHD_SIZE(DriverCallbackManager,0xBC);
u32 registerDriverCallback(u32 callback, void* data);
void processDriverCallbacks();
void processDriverSubframeCallbacks();
void processDriverDspCallbacks();
DriverCallbackEntry* constructDriverCallback(DriverCallbackEntry* entry);
void destroyDriverCallbackManager(DriverCallbackManager* manager,u32 flags);
void* allocateDriverCallback(u32 size);
void freeDriverCallback(void* object);
void initializeDriverCallbackMutex(DriverCallbackMutex* mutex);
void destroyDriverCallbackMutex(DriverCallbackMutex* mutex,u32 flags);
void lockDriverCallbackMutex(DriverCallbackMutex* mutex);
void unlockDriverCallbackMutex(DriverCallbackMutex* mutex);
using DriverCallbackConstructor = DriverCallbackEntry*(*)(DriverCallbackEntry*);
void constructDriverCallbackArray(void* array,u32 count,u32 stride,DriverCallbackConstructor constructor);
struct DriverCallbackHeaderStatics { be<u32> mWords[4]; };
struct DriverCallbackDestructorRecord { be<u32> mWords[3]; };
void registerDriverCallbackStatics(DriverCallbackDestructorRecord* record);

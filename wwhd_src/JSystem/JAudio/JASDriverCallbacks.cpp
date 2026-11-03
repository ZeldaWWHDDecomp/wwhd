/* Cafe driver callback scheduling, reconstructed from the WWHD binary. */
#include "JSystem/JAudio/JASDriverCallbacks.h"
static DriverCallbackManager subframeCallbacks;
static DriverCallbackManager driverCallbacks;
static DriverCallbackManager dspCallbacks;
static DriverCallbackHeaderStatics headerStatics;
static DriverCallbackHeaderStatics callbackHeaderStatics;
static DriverCallbackDestructorRecord headerDestructorRecord;
static DriverCallbackDestructorRecord subframeDestructorRecord;
static DriverCallbackDestructorRecord driverDestructorRecord;
static DriverCallbackDestructorRecord dspDestructorRecord;
static DriverCallbackDestructorRecord callbackDestructorRecord;
/* 028155FC */
u32 registerDriverCallback(u32 callback,void* data) { return driverCallbacks.add(callback,data); }
/* 02815610 */
void processDriverCallbacks() { driverCallbacks.process(); }
/* 0281561C */
void processDriverSubframeCallbacks() { subframeCallbacks.process(); }
/* 02815628 */
void processDriverDspCallbacks() { dspCallbacks.process(); }
/* 02815634 */
void __sinit_JASDriverCallbacks_cpp() {
    headerStatics.mWords[3] = 0;
    headerStatics.mWords[2] = 0;
    headerStatics.mWords[1] = 0;
    headerStatics.mWords[0] = 0;
    registerDriverCallbackStatics(&headerDestructorRecord);
    constructDriverCallbackArray(&subframeCallbacks,16,8,constructDriverCallback);
    initializeDriverCallbackMutex(&subframeCallbacks.mMutex);
    registerDriverCallbackStatics(&subframeDestructorRecord);
    constructDriverCallbackArray(&driverCallbacks,16,8,constructDriverCallback);
    initializeDriverCallbackMutex(&driverCallbacks.mMutex);
    registerDriverCallbackStatics(&driverDestructorRecord);
    constructDriverCallbackArray(&dspCallbacks,16,8,constructDriverCallback);
    initializeDriverCallbackMutex(&dspCallbacks.mMutex);
    registerDriverCallbackStatics(&dspDestructorRecord);
}
/* 0281571C */
void destroyDriverCallbackManager(DriverCallbackManager* manager,u32 flags) {
    if (!manager) return;
    destroyDriverCallbackMutex(&manager->mMutex,2);
    if (flags & 1) freeDriverCallback(manager);
}
/* 02815770 */
DriverCallbackEntry* constructDriverCallback(DriverCallbackEntry* entry) {
    if (!entry) entry = (DriverCallbackEntry*)allocateDriverCallback(8);
    if (entry) {
        entry->mData = nullptr;
        entry->mCallback = 0;
    }
    return entry;
}
/* 02843F28 */
u32 DriverCallbackManager::add(u32 callback,void* data) {
    lockDriverCallbackMutex(&mMutex);
    for (u32 i = 0; i < CALLBACK_COUNT; ++i) {
        if (!mEntries[i].mCallback) {
            mEntries[i].mCallback = callback;
            mEntries[i].mData = data;
            unlockDriverCallbackMutex(&mMutex);
            return 1;
        }
    }
    unlockDriverCallbackMutex(&mMutex);
    return 0;
}
/* 02843FE0 */
void DriverCallbackManager::process() {
    lockDriverCallbackMutex(&mMutex);
    for (u32 i = 0; i < CALLBACK_COUNT; ++i) {
        u32 callback = mEntries[i].mCallback;
        if (callback && gabi::call_ptr<s32>(callback,(void*)mEntries[i].mData) < 0) {
            mEntries[i].mData = nullptr;
            mEntries[i].mCallback = 0;
        }
    }
    unlockDriverCallbackMutex(&mMutex);
}
/* 02844060 */
void __sinit_JASCallbackManager_cpp() {
    callbackHeaderStatics.mWords[2] = 0;
    callbackHeaderStatics.mWords[0] = 0;
    callbackHeaderStatics.mWords[3] = 0;
    callbackHeaderStatics.mWords[1] = 0;
    registerDriverCallbackStatics(&callbackDestructorRecord);
}
/* WWHD glue
data subframeCallbacks 104B5408
data driverCallbacks 104B54C4
data dspCallbacks 104B5580
data headerStatics 104B53F8
data callbackHeaderStatics 104B65B4
data headerDestructorRecord 101F9F28
data subframeDestructorRecord 101F9F34
data driverDestructorRecord 101F9F40
data dspDestructorRecord 101F9F4C
data callbackDestructorRecord 101FC208
ext 028F026C void registerDriverCallbackStatics(DriverCallbackDestructorRecord*)
ext 028EFFD0 void constructDriverCallbackArray(void*,u32,u32,DriverCallbackConstructor)
ext 027603F0 void initializeDriverCallbackMutex(DriverCallbackMutex*)
ext 02760458 void destroyDriverCallbackMutex(DriverCallbackMutex*,u32)
ext 027604AC void lockDriverCallbackMutex(DriverCallbackMutex*)
ext 027604B4 void unlockDriverCallbackMutex(DriverCallbackMutex*)
ext 0273AD10 void* allocateDriverCallback(u32)
ext 0273AF40 void freeDriverCallback(void*)
*/

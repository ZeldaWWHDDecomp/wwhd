// HD JAudio file manager teardown and record facade.
#include "JSystem/JAudio/JASDvdThread_extra.h"
using namespace gabi;
using ManagerPointer = gptr<AudioFileManager>;
using WorkerPointer = gptr<void>;
static ManagerPointer sFileManager;
static WorkerPointer sFileWorker;
static u32 sManagerVtable;
static u32 sFileVtable;
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sManagerInitialization;
static Initialization sFileInitialization;
static Registration sManagerRegistration;
static Registration sFileRegistration;
extern void destroyDvdRecordMonitor(DvdRecordMonitor*);
extern void destroyDvdWorker(void*, u32);
extern void destroyDvdManagerBase(void*, u32);
extern void deleteDvdFacadeObject(void*);
extern void* allocateDvdFacadeObject(u32);
extern void constructDvdFacadeMonitor(DvdRecordMonitor*);
extern void destroyDvdFacadeMonitor(DvdRecordMonitor*, u32);
extern void lockDvdFacade(DvdRecordMonitor*);
extern void unlockDvdFacade(DvdRecordMonitor*);
extern AudioFileRecord* allocateDvdRecord(void*);
extern void registerDvdFacadeGlobals(void*);
/* 0280F388 */
void AudioFileManager::freeRecord(AudioFileRecord* record) {
    destroyDvdRecordMonitor(&record->mMonitor);
    u32 count = mActiveCount;
    AudioFileRecord* head = mFreeRecords;
    mActiveCount = count - 1;
    record->mNext = head;
    mFreeRecords = record;
}
/* 0280F3DC */
void destroyDvdFileManager(AudioFileManager* manager, u32 flags) {
    if (!manager)
        return;
    manager->mVtable = &sManagerVtable;
    if (sFileManager == manager) {
        void* worker = sFileWorker;
        sFileManager = nullptr;
        destroyDvdWorker(worker, 2);
        sFileWorker = nullptr;
    }
    destroyDvdManagerBase(manager, 0);
    if (flags & 1)
        deleteDvdFacadeObject(manager);
}
/* 0280F478 */
void __sinit_JASDvdManager_extra_cpp() {
    sManagerInitialization.words[2] = 0;
    sManagerInitialization.words[0] = 0;
    sManagerInitialization.words[3] = 0;
    sManagerInitialization.words[1] = 0;
    registerDvdFacadeGlobals(&sManagerRegistration);
}
/* 0280F4A0 */
JASDvdFile* constructDvdFile(JASDvdFile* file) {
    if (!file) {
        file = static_cast<JASDvdFile*>(allocateDvdFacadeObject(sizeof(JASDvdFile)));
        if (!file)
            return nullptr;
    }
    file->mRequestState = 0;
    file->mReady = 0;
    file->mVtable = &sFileVtable;
    file->mRecord = nullptr;
    constructDvdFacadeMonitor(&file->mMonitor);
    return file;
}
/* 0280F504 */
void destroyDvdFile(JASDvdFile* file, u32 flags) {
    if (!file)
        return;
    destroyDvdFacadeMonitor(&file->mMonitor, 2);
    if (flags & 1)
        deleteDvdFacadeObject(file);
}
/* 0280F558 */
AudioFileRecord* JASDvdFile::allocateRecord() {
    lockDvdFacade(&mMonitor);
    void* worker = sFileWorker;
    AudioFileRecord* record = allocateDvdRecord(worker);
    unlockDvdFacade(&mMonitor);
    return record;
}
/* 0280F5AC */
void JASDvdFile::releaseRecord(AudioFileRecord* record) {
    lockDvdFacade(&mMonitor);
    auto manager = reinterpret_cast<AudioFileManager*>(sFileWorker.get());
    manager->freeRecord(record);
    unlockDvdFacade(&mMonitor);
}
/* 0280FA60 */
u32 JASDvdFile::isReady() const { return mReady; }
/* 0280FB30 */
u32 JASDvdFile::getFileSize() const {
    return mReady ? (u32)mFileSize : 0;
}
/* 0280FB4C */
void __sinit_JASDvdFile_extra_cpp() {
    sFileInitialization.words[2] = 0;
    sFileInitialization.words[0] = 0;
    sFileInitialization.words[3] = 0;
    sFileInitialization.words[1] = 0;
    registerDvdFacadeGlobals(&sFileRegistration);
}
/* WWHD glue
data sFileManager 101F9D70
data sFileWorker 101F9D6C
data sManagerVtable 1016FDA4
data sFileVtable 1016FE24
data sManagerInitialization 104B51E4
data sFileInitialization 104B51F4
data sManagerRegistration 101F9D60
data sFileRegistration 101F9D78
ext 0273B310 void destroyDvdRecordMonitor(DvdRecordMonitor*)
ext 0280F214 void destroyDvdWorker(void*, u32)
ext 02752BEC void destroyDvdManagerBase(void*, u32)
ext 0273AF40 void deleteDvdFacadeObject(void*)
ext 0273AD10 void* allocateDvdFacadeObject(u32)
ext 027603F0 void constructDvdFacadeMonitor(DvdRecordMonitor*)
ext 02760458 void destroyDvdFacadeMonitor(DvdRecordMonitor*, u32)
ext 027604AC void lockDvdFacade(DvdRecordMonitor*)
ext 027604B4 void unlockDvdFacade(DvdRecordMonitor*)
ext 0280F2EC AudioFileRecord* allocateDvdRecord(void*)
ext 028F026C void registerDvdFacadeGlobals(void*)
*/

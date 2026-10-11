/** HD save manager and platform storage views.
 * HD replaces memory-card IO with a threaded FileDevice path. GameCube save member names are retained.
 */
#pragma once
#include "d/d_picture_box.h"
#include "gc_api/d/d_com_inf_game.h"
#include <initializer_list>
using namespace gabi;
namespace hdSave {
struct SaveMgr {
    /* 0x00 */ be<u32> mVtable;
    u8 padding[0x10];
    /* 0x14 */ be<u32> mpHeap;
    /* 0x18 */ be<u32> mpThread;
    /* 0x1C */ be<u32> mCommand;
    /* 0x20 */ be<u32> mReadStatus;
    /* 0x24 */ be<u32> mWriteStatus;
    /* 0x28 */ u8 mCriticalSection[0x3C];
    /* 0x64 */ be<u32> mpSaveData;
    /* 0x68 */ be<u32> mpWorkBuffer;
    void init(u32 heap);
    void sync(u32 slot);
    void startup();
    void initialCard(u32 slot);
    void initialCardWrapper(u32 slot);
    void mode(u32 mode);
    void begin(u32 mode);
    void checksumSlot(u32 data, u32 slot);
    void syncSlot(u32 slot);
    void saveCard(u32 slot, u32 update);
    void end();
    Pair32 checksum(u32 data, u32 count);
    void refreshGame();
    void loadCard(u32 slot, u32 resetItems);
    void threadEvent(u32 arg, u32 event);
    u32 checkSlot(u32 data, u32 slot);
    void terminateSlotName(u32 data, u32 slot);
    u32 readSlots(u32 file, u32 count);
    u32 readPlayer(u32 file, u32 count);
    u32 readStatus(u32 file, u32 count);
    u32 readEvent(u32 file, u32 count);
    u32 readMap(u32 file, u32 count);
    u32 readName(u32 file, u32 count);
    u32 checkSerial(u32 file, u32 count);
    u32 checkVersion(u32 file, u32 count);
    u32 writeSlots(u32 file, u32 count);
    u32 writePlayer(u32 file, u32 count);
    u32 writeStatus(u32 file, u32 count);
    u32 writeEvent(u32 file, u32 count);
    u32 writeMap(u32 file, u32 count);
    u32 writeSerial(u32 file, u32 count);
    u32 writeVersion(u32 file, u32 count);
    u32 writeName(u32 file, u32 count);
    void selectSlot(u32 slot);
    void resetSlot(u32 slot);
    void clearPictureStore(u32 slot);
    void clearMiiverseData();
    void clearPictures();
    void clearAll();
    void queueCreate();
    void queueDelete();
    void createCard();
    void copySlot(u32 to, u32 from);
    u32 openForWrite();
    u32 eventBit(u32 bit);
    u32 eventCount();
    void describeSlot(u32 playLog);
    u32 progress(u32 alternate);
    void adjustPlayLog(u32 dest, u32 old, u32 fresh);
    void advanceSlot(u32 slot);
    u32 readPictureData(u32 file, u32 count, u32 slot, u32 room);
    u32 writePlayLog(u32 file, u32 count);
    void readMiiverseData();
    void readPlayLog();
    u32 readPictureHeaders(u32 file, u32 count, u32 slot);
    u32 writePictureData(u32 file, u32 count, u32 slot, u32 room);
    u32 writePictureHeaders(u32 file, u32 count, u32 slot);
    void workerLoad();
    void workerMiiverseData();
    void readPictureFiles(u32 slot);
    void writePictureFiles(u32 slot);
    void writePlayLogFile();
    void mergePlayLogs(u32 dest);
    void workerWrite(u32 allSlots);
    void recollectionSave();
};
WWHD_OFFSET(SaveMgr, mVtable, 0);
WWHD_OFFSET(SaveMgr, mpHeap, 0x14);
WWHD_OFFSET(SaveMgr, mpThread, 0x18);
WWHD_OFFSET(SaveMgr, mCommand, 0x1C);
WWHD_OFFSET(SaveMgr, mReadStatus, 0x20);
WWHD_OFFSET(SaveMgr, mWriteStatus, 0x24);
WWHD_OFFSET(SaveMgr, mCriticalSection, 0x28);
WWHD_OFFSET(SaveMgr, mpSaveData, 0x64);
WWHD_OFFSET(SaveMgr, mpWorkBuffer, 0x68);
WWHD_SIZE(SaveMgr, 0x6C);
struct SaveName {
    be<u32> name, owner;
};
WWHD_OFFSET(SaveName, name, 0);
WWHD_OFFSET(SaveName, owner, 4);
WWHD_SIZE(SaveName, 8);
struct FileHandle {
    u8 mDisposer[12];
    be<u32> mVtable, mHandle, mpHeap;
    u8 padding[0x38 - 0x18];
    be<u32> mState;
};
WWHD_OFFSET(FileHandle, mVtable, 12);
WWHD_OFFSET(FileHandle, mHandle, 16);
WWHD_OFFSET(FileHandle, mpHeap, 20);
WWHD_OFFSET(FileHandle, mState, 0x38);
WWHD_SIZE(FileHandle, 0x3C);
struct FormatString {
    be<u32> mpText, mVtable, mCapacity;
    char mBuffer[64];
};
WWHD_OFFSET(FormatString, mpText, 0);
WWHD_OFFSET(FormatString, mVtable, 4);
WWHD_OFFSET(FormatString, mCapacity, 8);
WWHD_OFFSET(FormatString, mBuffer, 12);
WWHD_SIZE(FormatString, 76);
struct StageComparisons {
    SaveName names[4], games[8];
};
WWHD_OFFSET(StageComparisons, games, 32);
WWHD_SIZE(StageComparisons, 96);
struct SlotData {
    u8 mData[0xA8C];
    be<u32> mChecksum, mChecksumComplement;
};
WWHD_OFFSET(SlotData, mChecksum, 0xA8C);
WWHD_OFFSET(SlotData, mChecksumComplement, 0xA90);
WWHD_SIZE(SlotData, 0xA94);
inline u32 readWord(u32 address, u32 offset = 0) { return load<u32>(address + offset); }

// HD: save data has a leading platform header and expanded card, photograph and miiverseData-data areas.
struct SaveGameState {
    u8 mPlatformHeader[0x20];
    dSv_info_c mSave;
    u8 mCardInfo[1];
    u8 paddingMiiverseData[0xF026C0 - 0x12C1];
    u8 mMiiverseData[0x6A4];
};
WWHD_OFFSET(SaveGameState, mSave, 0x20);
WWHD_OFFSET(SaveGameState, mCardInfo, 0x12C0);
WWHD_OFFSET(SaveGameState, mMiiverseData, 0xF026C0);
WWHD_SIZE(SaveGameState, 0xF02D64);
struct Worker {
    void destroy(u32 flags);
    u8 mDisposer[12];
    be<u32> mVtable, mState, mpHeap;
};
WWHD_OFFSET(Worker, mVtable, 12);
WWHD_OFFSET(Worker, mpHeap, 20);
WWHD_SIZE(Worker, 24);
struct SaveDisposer : Worker {
    void destroy(u32 flags);
};
WWHD_SIZE(SaveDisposer, 24);
struct WorkerVtable {
    u8 padding[0x1C];
    be<u32> postMessage;
    u8 paddingStart[0x2C - 0x20];
    be<u32> start;
};
WWHD_OFFSET(WorkerVtable, postMessage, 0x1C);
WWHD_OFFSET(WorkerVtable, start, 0x2C);
WWHD_SIZE(WorkerVtable, 0x30);
// HD: GHS represents member callbacks as signed adjustment/index halfwords plus a target word.
struct MemberCallback {
    be<s16> mThisAdjustment, mVirtualIndex;
    be<u32> mTarget;
};
WWHD_OFFSET(MemberCallback, mVirtualIndex, 2);
WWHD_OFFSET(MemberCallback, mTarget, 4);
WWHD_SIZE(MemberCallback, 8);
struct WorkerCallback {
    be<u32> mVtable, mpObject;
    MemberCallback mCallback;
};
WWHD_OFFSET(WorkerCallback, mpObject, 4);
WWHD_OFFSET(WorkerCallback, mCallback, 8);
WWHD_SIZE(WorkerCallback, 16);
struct SafeStringVtable {
    u8 padding[0x14];
    be<u32> assureTermination;
};
WWHD_OFFSET(SafeStringVtable, assureTermination, 0x14);
WWHD_SIZE(SafeStringVtable, 0x18);
struct StageVtable {
    u8 padding[0x15C];
    be<u32> getStageInfo;
};
WWHD_OFFSET(StageVtable, getStageInfo, 0x15C);
WWHD_SIZE(StageVtable, 0x160);
struct StageInfo {
    u8 padding[9];
    be<u8> mSaveStage;
};
WWHD_OFFSET(StageInfo, mSaveStage, 9);
WWHD_SIZE(StageInfo, 10);
struct MenuManager {
    u8 padding[0x218];
    be<u32> mpScreen;
};
WWHD_OFFSET(MenuManager, mpScreen, 0x218);
WWHD_SIZE(MenuManager, 0x21C);
enum {
    SaveSlotCount = 3,
    SaveHeapSize = 0xAF000,
    SaveBufferSize = 0x1FBC,
    WorkBufferSize = 0x50400,
    FileAlignment = 0x200,
    SaveItemCount = 21,
    NoItem = 255,
    MaxSaveCount = 9999,
    WorkerIdle = 0,
    WorkerLoad = 1,
    WorkerWrite = 2,
    WorkerCreate = 3,
    WorkerDelete = 4,
    ReadNoFile = -2,
    ReadInvalid = -3,
    WriteNoFile = -1,
    WriteFailed = -2,
    WriteUnavailable = -3,
    OperationPending = 255,
    PhotoDataSize = 0x50000,
    MiiverseDataSize = 0x6A4,
    PlayLogSize = 0x174
};

// HD: fixed TU vtable/callback identities are kept as guest addresses, not native function pointers.
enum {
    SaveMgrVtableAddress = 0x10140840,
    SafeStringVtableAddress = 0x101407B0,
    FormatStringVtableAddress = 0x10140818,
    WorkerCallbackVtableAddress = 0x10140850,
    SaveDisposerVtableAddress = 0x10140AA8,
    FileDisposerVtableAddress = 0x10140830,
    WorkerEventAddress = 0x02724580
};
} // namespace hdSave

// HD: file-local ABI adapters preserve the complete integer argument registers.
s32 saveCardApi_storeSaveInfo(u32, u32, u32);
u32 saveBootstrapApi_allocateSaveAligned(u32, u32, u32);
u32 saveBootstrapApi_allocateSaveHeap(u32, u32, u32);
u32 saveBootstrapApi_allocateSaveObject(u32);
u32 saveBootstrapApi_checkItemFlag(u32, u32, u32);
u32 saveBootstrapApi_constructSaveWorker(u32, u32, u32, u32, u32, u32, u32, u32, u32);
u32 saveBootstrapApi_createSaveHeap(u32, u32, u32, u32, u32);
u32 saveBootstrapApi_getCardEvents(u32, u32);
u32 saveBootstrapApi_getCardMap(u32, u32);
u32 saveBootstrapApi_getCardPictures(u32, u32);
u32 saveBootstrapApi_getCardPlayLog(u32, u32);
u32 saveBootstrapApi_getCardPlayer(u32, u32);
u32 saveBootstrapApi_getCardStatus(u32, u32);
u32 saveBootstrapApi_getCurrentCardEvents(u32);
u32 saveBootstrapApi_getCurrentCardMap(u32);
u32 saveBootstrapApi_getCurrentCardPictures(u32);
u32 saveBootstrapApi_getCurrentCardPlayLog(u32);
u32 saveBootstrapApi_getCurrentCardPlayer(u32);
u32 saveBootstrapApi_getCurrentCardStatus(u32);
u32 saveBootstrapApi_getGameInfoAddress();
u32 saveBootstrapApi_getItemForSaveIndex(u32);
u32 saveBootstrapApi_saveMgrAllocateCall(u32, u32, u32);
u32 saveBootstrapApi_saveMgrCtorCall(u32);
u32 saveCardApi_checkCollectFlag(u32, u32, u32);
u32 saveCardApi_getCardEvents(u32, u32);
u32 saveCardApi_getCardMap(u32, u32);
u32 saveCardApi_getCardPictures(u32, u32);
u32 saveCardApi_getCardPlayLog(u32, u32);
u32 saveCardApi_getCardPlayer(u32, u32);
u32 saveCardApi_getCardStatus(u32, u32);
u32 saveCardApi_getCurrentCardEvents(u32);
u32 saveCardApi_getCurrentCardMap(u32);
u32 saveCardApi_getCurrentCardPictures(u32);
u32 saveCardApi_getCurrentCardPlayLog(u32);
u32 saveCardApi_getCurrentCardPlayer(u32);
u32 saveCardApi_getCurrentCardStatus(u32);
u32 saveCardApi_getGameInfoAddress();
u32 saveCardApi_saveMgrChecksumCall(u32, u32, u32);
u32 saveStorageApi_allocateSaveAligned(u32, u32, u32);
u32 saveStorageApi_calculatePictureHeaderChecksum(u32);
u32 saveStorageApi_checkAcquiredItem(u32);
u32 saveStorageApi_checkCollectFlag(u32, u32, u32);
u32 saveStorageApi_checkCollectSymbol(u32, u32);
u32 saveStorageApi_checkItemFlag(u32, u32, u32);
u32 saveStorageApi_checkSaveEventBit(u32, u32);
u32 saveStorageApi_getCardDataVersion(u32);
u32 saveStorageApi_getCardEvents(u32, u32);
u32 saveStorageApi_getCardMap(u32, u32);
u32 saveStorageApi_getCardName(u32, u32);
u32 saveStorageApi_getCardPictures(u32, u32);
u32 saveStorageApi_getCardPlayLog(u32, u32);
u32 saveStorageApi_getCardPlayLogSummary(u32, u32);
u32 saveStorageApi_getCardPlayer(u32, u32);
u32 saveStorageApi_getCardPlayerCompletion(u32);
u32 saveStorageApi_getCardPlayerMode(u32);
u32 saveStorageApi_getCardPlayerSlot(u32);
u32 saveStorageApi_getCardPlayerSummary(u32, u32);
u32 saveStorageApi_getCardStatus(u32, u32);
u32 saveStorageApi_getCurrentCardPictures(u32);
u32 saveStorageApi_getCurrentCardPlayLog(u32);
u32 saveStorageApi_getGameInfoAddress();
u32 saveStorageApi_getMenuPlayLogState(u32);
u32 saveStorageApi_getPictureData(u32, u32);
u32 saveStorageApi_getPictureHeaderChecksum(u32);
u32 saveStorageApi_getPictureRecord(u32, u32);
u32 saveStorageApi_getPlayLogCounterA(u32);
u32 saveStorageApi_getPlayLogCounterB(u32);
u32 saveStorageApi_getPlayLogCounterC(u32);
u32 saveStorageApi_getPlayLogCounterD(u32);
u32 saveStorageApi_getPlayLogCounterE(u32);
u32 saveStorageApi_getPlayLogCounterF(u32);
u32 saveStorageApi_getPlayLogCounterG(u32);
u32 saveStorageApi_getPlayLogCounterH(u32);
u32 saveStorageApi_getPlayLogProgress(u32);
u32 saveStorageApi_getPlayLogPropertyA(u32);
u32 saveStorageApi_getPlayLogPropertyB(u32);
u32 saveStorageApi_getPlayLogPropertyC(u32);
u32 saveStorageApi_getPlayLogPropertyD(u32);
u32 saveStorageApi_getPlayLogPropertyE(u32);
u32 saveStorageApi_getPlayLogPropertyF(u32);
u32 saveStorageApi_getPlayLogPropertyG(u32);
u32 saveStorageApi_getPlayLogPropertyH(u32);
u32 saveStorageApi_getPlayLogPropertyI(u32);
u32 saveStorageApi_getPlayLogPropertyJ(u32);
u32 saveStorageApi_getPlayLogPropertyK(u32);
u32 saveStorageApi_getPlayLogPropertyL(u32);
u32 saveStorageApi_getSaveEventRegister(u32, u32);
u32 saveStorageApi_getSaveFileEntry(u32, u32);
u32 saveStorageApi_getSaveFileHandle(u32);
u32 saveStorageApi_getTriforceCount(u32);
u32 saveStorageApi_isPictureSlotValid(u32, u32);
u32 saveStorageApi_readSaveFile(u32, u32, u32, u32);
u32 saveStorageApi_removeSaveFileEntry(u32);
u32 saveStorageApi_saveMgrCheckSlotCall(u32, u32, u32);
u32 saveStorageApi_saveMgrChecksumCall(u32, u32, u32);
u32 saveStorageApi_saveMgrEventBitCall(u32, u32);
u32 saveStorageApi_saveMgrEventCountCall(u32);
u32 saveStorageApi_saveMgrProgressCall(u32, u32);
u32 saveStorageApi_saveMgrReadPictureDataCall(u32, u32, u32, u32, u32);
u32 saveStorageApi_saveMgrWritePictureDataCall(u32, u32, u32, u32, u32);
u32 saveStorageApi_writeSaveFile(u32, u32, u32, u32);
void saveBootstrapApi_constructSaveDisposer(u32, u32, u32);
void saveBootstrapApi_copyCardEvents(u32, u32);
void saveBootstrapApi_copyCardMap(u32, u32);
void saveBootstrapApi_copyCardPlayer(u32, u32);
void saveBootstrapApi_copyCardStatus(u32, u32);
void saveBootstrapApi_copyPictureStore(u32, u32);
void saveBootstrapApi_copySavePlayLog(u32, u32);
void saveBootstrapApi_fillSaveBytes(u32, u32, u32);
void saveBootstrapApi_initializeSaveCriticalSection(u32);
void saveBootstrapApi_initializeSaveFileDevice(u32);
void saveCardApi_copyCardEvents(u32, u32);
void saveCardApi_copyCardMap(u32, u32);
void saveCardApi_copyCardPlayer(u32, u32);
void saveCardApi_copyCardStatus(u32, u32);
void saveCardApi_copyPictureStore(u32, u32);
void saveCardApi_copySavePlayLog(u32, u32);
void saveCardApi_disablePictureSlots(u32);
void saveCardApi_getSaveSystemTime();
void saveCardApi_initializeCardSlot(u32, u32);
void saveCardApi_initializePlayState(u32);
void saveCardApi_initializeSaveInfo(u32);
void saveCardApi_initializeSaveSlot(u32, u32, u32);
void saveCardApi_loadSaveInfo(u32, u32, u32);
void saveCardApi_markOwnedChart(u32, u32);
void saveCardApi_markVisitedStage(u32, u32);
void saveCardApi_saveMgrChecksumSlotCall(u32, u32, u32);
void saveCardApi_saveMgrInitialCardCall(u32, u32);
void saveCardApi_saveMgrModeCall(u32, u32);
void saveCardApi_saveMgrRefreshEquipmentCall(u32);
void saveCardApi_saveMgrRefreshGameCall(u32);
void saveCardApi_saveMgrSessionCounterCall(u32);
void saveCardApi_saveMgrStampTimeCall();
void saveCardApi_saveMgrStartupCall(u32);
void saveCardApi_saveMgrSyncCall(u32, u32);
void saveCardApi_saveMgrSyncSlotCall(u32, u32);
void saveCardApi_setSelectedEquipment(u32, u32);
void saveCardApi_updateSaveGameState();
void saveCompilerApi_deleteSaveObject(u32);
void saveCompilerApi_destroySaveDisposer(u32, u32);
void saveCompilerApi_unlinkFileDisposer(u32, u32);
void saveStorageApi_addPlayLogCounterA(u32, u32);
void saveStorageApi_addPlayLogCounterB(u32, u32);
void saveStorageApi_addPlayLogCounterC(u32, u32);
void saveStorageApi_addPlayLogCounterD(u32, u32);
void saveStorageApi_addPlayLogCounterE(u32, u32);
void saveStorageApi_addPlayLogCounterF(u32, u32);
void saveStorageApi_addPlayLogCounterG(u32, u32);
void saveStorageApi_addPlayLogCounterH(u32, u32);
void saveStorageApi_clearMiiverseDataSaveData(u32);
void saveStorageApi_clearPictureStore(u32);
void saveStorageApi_clearSavePlayLog(u32);
void saveStorageApi_closeSaveFile(u32);
void saveStorageApi_constructFileDisposer(u32);
void saveStorageApi_copyCardName(u32, u32);
void saveStorageApi_copySaveBytes(u32, u32, u32, u32);
void saveStorageApi_copySavePlayLog(u32, u32);
void saveStorageApi_deleteSaveObject(u32);
void saveStorageApi_destroySaveCriticalSection(u32, u32);
void saveStorageApi_destroySaveDisposer(u32, u32);
void saveStorageApi_disablePictureSlot(u32, u32);
void saveStorageApi_enablePictureSlots(u32);
void saveStorageApi_formatSaveFileName(u32, u32);
void saveStorageApi_formatSaveFileName(u32, u32, u32);
void saveStorageApi_formatSaveFileName(u32, u32, u32, u32);
void saveStorageApi_freeSaveBuffer(u32);
void saveStorageApi_getCurrentCardPlayLogDiscard(u32);
void saveStorageApi_initializeCurrentCardData(u32);
void saveStorageApi_initializeSaveHeaderByte(u32);
void saveStorageApi_initializeSaveHeaderFlag(u32);
void saveStorageApi_mergeSavePlayLog(u32, u32);
void saveStorageApi_openSaveFile(u32, u32, u32, u32, u32);
void saveStorageApi_playSaveErrorSound(u32, u32, u32);
void saveStorageApi_prepareRecollectionItems();
void saveStorageApi_registerSaveHeaderObject(u32);
void saveStorageApi_reinitializeSaveInfo(u32);
void saveStorageApi_restoreRecollectionItems();
void saveStorageApi_saveMgrAdjustPlayLogCall(u32, u32, u32, u32);
void saveStorageApi_saveMgrChecksumSlotCall(u32, u32, u32);
void saveStorageApi_saveMgrClearAllCall(u32);
void saveStorageApi_saveMgrClearMiiverseDataCall(u32);
void saveStorageApi_saveMgrClearPictureStoreCall(u32, u32);
void saveStorageApi_saveMgrClearPicturesCall(u32);
void saveStorageApi_saveMgrDescribeSlotCall(u32, u32);
void saveStorageApi_saveMgrEndCall(u32);
void saveStorageApi_saveMgrInitialCardCall(u32, u32);
void saveStorageApi_saveMgrLoadCardCall(u32, u32, u32);
void saveStorageApi_saveMgrQueueCreateCall(u32);
void saveStorageApi_saveMgrReadMiiverseDataCall(u32);
void saveStorageApi_saveMgrReadPictureFilesCall(u32, u32);
void saveStorageApi_saveMgrReadPictureHeadersCall(u32, u32, u32, u32);
void saveStorageApi_saveMgrReadPlayLogCall(u32);
void saveStorageApi_saveMgrSaveCardCall(u32, u32, u32);
void saveStorageApi_saveMgrSelectSlotCall(u32, u32);
void saveStorageApi_saveMgrTerminateSlotNameCall(u32, u32, u32);
void saveStorageApi_saveMgrWorkerLoadCall(u32, u32, u32);
void saveStorageApi_saveMgrWorkerMiiverseDataCall(u32, u32, u32);
void saveStorageApi_saveMgrWorkerWriteCall(u32, u32, u32);
void saveStorageApi_saveMgrWritePictureFilesCall(u32, u32);
void saveStorageApi_saveMgrWritePictureHeadersCall(u32, u32, u32, u32);
void saveStorageApi_saveMgrWritePlayLogCall(u32, u32, u32);
void saveStorageApi_saveMgrWritePlayLogFileCall(u32);
void saveStorageApi_setCardName(u32, u32);
void saveStorageApi_setCardPlayerSlot(u32, u32);
void saveStorageApi_setPictureSelection(u32, u32);
void saveStorageApi_setPlayLogCounterA(u32, u32);
void saveStorageApi_setPlayLogCounterB(u32, u32);
void saveStorageApi_setPlayLogCounterC(u32, u32);
void saveStorageApi_setPlayLogCounterD(u32, u32);
void saveStorageApi_setPlayLogCounterE(u32, u32);
void saveStorageApi_setPlayLogCounterF(u32, u32);
void saveStorageApi_setPlayLogCounterG(u32, u32);
void saveStorageApi_setPlayLogCounterH(u32, u32);
void saveStorageApi_setPlayLogProgress(u32, u32);
void saveStorageApi_setPlayLogPropertyA(u32, u32);
void saveStorageApi_setPlayLogPropertyB(u32, u32);
void saveStorageApi_setPlayLogPropertyC(u32, u32);
void saveStorageApi_setPlayLogPropertyD(u32, u32);
void saveStorageApi_setPlayLogPropertyE(u32, u32);
void saveStorageApi_setPlayLogPropertyF(u32, u32);
void saveStorageApi_setPlayLogPropertyG(u32, u32);
void saveStorageApi_setPlayLogPropertyH(u32, u32);
void saveStorageApi_setPlayLogPropertyI(u32, u32);
void saveStorageApi_setPlayLogPropertyJ(u32, u32);
void saveStorageApi_setPlayLogPropertyK(u32, u32);
void saveStorageApi_setPlayLogPropertyL(u32, u32);
void saveStorageApi_unlinkFileDisposer(u32, u32);
void saveStorageApi_updateCardDataVersion(u32);
void saveStorageApi_updatePictureHeaderChecksum(u32);

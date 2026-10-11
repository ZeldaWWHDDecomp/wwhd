// HD: ErrorViewerTask and the Wii U error UI have no GameCube counterpart.
#pragma once
#include "bindings.h"
#include <cstddef>
enum ErrorViewerState {
    ERROR_IDLE = 0,
    ERROR_SHOWN = 1,
    ERROR_CHECK = 2,
    ERROR_CLOSING = 3,
    ERROR_RESULT = 4,
    ERROR_RECOVER = 5,
    ERROR_LAUNCHED = 6
};
enum ErrorViewerCodes {
    ERROR_DISC = 0x192DB5,
    ERROR_STORAGE = 0x170AD5,
    ERROR_SETTINGS = 0x1E5D7C
};
enum ErrorViewerLimits {
    ERROR_QUEUE_CAPACITY = 64,
    ERROR_DISC_DELAY = 3,
    ERROR_STORAGE_DELAY = 13,
    ERROR_RECOVER_DELAY = 3
};
struct ErrorViewerTask;
struct ErrorRegistration {
    be<u8> mRegistered;
};
struct ErrorSafeString {
    gptr<const be<u8>> mpData;
    be<u32> __vtbl;
};
struct ErrorDelegate {
    be<u32> __vtbl;
    gptr<ErrorViewerTask> mpObject;
    be<s16> mAdjustment, mIndex;
    be<u32> mFunction;
};
struct ErrorFSEvent {
    u8 prefix[12];
    be<u32> __vtbl;
    ErrorSafeString mName;
    be<u32> mType, mArgument;
    ErrorDelegate mDelegate;
    u8 tail[16];
};
struct ErrorMutex {
    u8 bytes[0x3C];
};
struct ErrorEntry {
    be<u32> mCode;
    union {
        be<u32> mFlags;
        struct {
            be<u8> mFlag;
            u8 mFlagTail[3];
        };
    };
};
struct ErrorQueue {
    gptr<ErrorEntry> mpEntries;
    be<s32> mCapacity, mHead, mCount;
    ErrorEntry mStorage[ERROR_QUEUE_CAPACITY];
};
struct ErrorHeapVtable {
    u8 prefix[0x24];
    gfn mDestroy;
};
struct ErrorHeap {
    u8 prefix[12];
    gptr<ErrorHeapVtable> mpVtable;
    void destroy() { gabi::call_ptr(mpVtable->mDestroy.get(), this); }
};
struct ErrorFSClient {
    u8 bytes[0x1700];
};
struct ErrorViewerTask {
    u8 prefix[0x20];
    be<u32> __vtbl;
    u8 gap24[0x70 - 0x24];
    be<u32> mTaskVtable;
    u8 gap74[0xC8 - 0x74];
    ErrorRegistration mRegistration;
    u8 padC9[3];
    gptr<ErrorHeap> mpHeap;
    ErrorFSEvent mFirstEvent, mSecondEvent;
    ErrorMutex mMutex;
    ErrorQueue mQueue;
    be<u32> mState;
    ErrorEntry mCurrent;
    gptr<void> mpWork;
    be<u32> mRecoverFrames;
    be<u8> mSavedDim;
    u8 pad3B1[3];
    be<u32> mDiscFrames, mStorageFrames;
    ErrorFSClient mClient;
    gptr<void> mpWatchedVolume;
    be<u32> mVolumeState;
    void registerViewer();
    u32 isShowing();
    void push(u32 code, u32 flag);
    void appear();
    void disappear();
    u32 workSize();
    void createViewer();
    void prepare();
    void feedInput();
    u32 volumeError(void* client);
    void enter();
    void show(u32 code);
    void setState(u32 state);
    void popEntry();
    void stateIdle();
    void stateShown();
    u32 discStill(ErrorEntry* entry);
    u32 storageStill(ErrorEntry* entry);
    u32 volumeStill(ErrorEntry* entry);
    u32 otherStill(ErrorEntry* entry);
    void close();
    void stateCheck();
    void stateClosing();
    u32 launch(s32 page);
    void restoreDim();
    void stateResult();
    void stateRecover();
    void calc();
    void exit();
};
struct ErrorLiteral {
    be<u8> mText[64];
};
struct ErrorSettings {
    u8 prefix[0x10];
    be<u32> mRegion, mLanguage;
};
struct ErrorLanguageTable {
    be<u8> mLanguages[11];
};
struct ErrorNamedString {
    ErrorSafeString mString;
    u8 tail[4];
};
struct ErrorAppearArgs {
    be<u32> mType;
    u8 tail[8];
};
struct ErrorCreateArgs {
    gptr<void> mpWork;
    be<u32> mRegion, mLanguage;
    gptr<ErrorFSClient> mpClient;
};
struct ErrorShowArgs {
    be<u32> mType, mController, mReserved08, mReserved0C, mCode, mRegion, mReserved18, mReserved1C,
        mReserved20, mReserved24;
    be<u8> mFlag;
    u8 tail[3];
};
struct ErrorLaunchArgs {
    be<u32> mFlags, mReserved, mPage;
};
struct ErrorInputPad {
    u8 prefix[0xF00];
    be<s32> mCount;
    be<u32> mStatus;
};
struct ErrorInputPads {
    u8 prefix[0x1118];
    ErrorInputPad mPads[4];
};
struct ErrorVPadStatus {
    union {
        be<u32> mWords[0x2B];
        struct {
            u8 prefix[0x52];
            u8 mTouch[12];
        };
    };
};
struct ErrorGamePad {
    u8 prefix[0x14];
    ErrorVPadStatus mStatus;
    u8 gap[0xAD4 - 0x14 - sizeof(ErrorVPadStatus)];
    be<s32> mCount;
    be<u32> mResult;
};
struct ErrorInputArgs {
    gptr<ErrorVPadStatus> mpGamePad;
    gptr<ErrorInputPad> mPads[4];
    ErrorVPadStatus mStatus;
};
struct ErrorCPULists {
    u8 prefix[0x1020];
    be<u32> mCount;
    gptr<gptr<void>> mpLists;
};
struct ErrorMenuState {
    u8 prefix[0x1D0];
    be<u32> mStorageReady;
};
struct ErrorFileSystem {
    u8 prefix[0x24];
    u8 mVolume;
};
struct ErrorGameInfo {
    u8 prefix[0x599C];
    u8 mVibration;
};
struct ErrorHeader {
    be<f32> mMinAngle, mMaxAngle;
    be<u8> mByte, mFlag;
    u8 pad[2];
    be<u32> mWords[4];
};
struct ErrorInitConstants {
    be<f32> mMinAngle, mMaxAngle;
};
struct ErrorDestructorRecords {
    u8 mRecords[3][12];
};

void errorDelete(void*);
void errorDestroyMutex(ErrorMutex*, u32);
void errorDestroyEvent(ErrorFSEvent*, u32);
void errorDestroyTask(ErrorViewerTask*, u32);
ErrorSafeString* errorAllocateString(u32);
ErrorViewerTask* errorAllocateTask(u32);
void errorConstructTask(ErrorViewerTask*, void*, const be<u8>*);
void errorConstructEvent(ErrorFSEvent*);
void errorConstructMutex(ErrorMutex*);
ErrorQueue* errorAllocateQueue(u32);
void errorZero(void*, u32);
void errorReportQueue(const be<u8>*, u32, u32);
void errorLock(ErrorMutex*);
void errorUnlock(ErrorMutex*);
void errorAppear(ErrorAppearArgs*);
u32 errorGetHandle(ErrorViewerTask*);
void errorDisappear(u32);
u32 errorGetWorkSize(ErrorViewerTask*);
void* errorGetParentHeap();
ErrorHeap* errorCreateHeap(u32, ErrorSafeString*, void*, u32, u32);
void* errorAllocateInHeap(u32, ErrorHeap*, u32);
void errorClear(void*, u32, u32);
void errorCreate(ErrorCreateArgs*);
void errorAttachEvent(void*, u32, ErrorFSEvent*);
void errorAddClient(ErrorFSClient*, u32);
ErrorInputPads* errorGetPads(void*);
ErrorGamePad* errorGetGamePad(void*);
void errorCalibrateTouch(u32, void*, void*);
void errorSetInput(ErrorInputArgs*);
s32 errorVolumeState(void*);
u32 errorLastVolumeError(void*);
void errorPauseSound();
ErrorGameInfo* errorGetGameInfo();
void errorStopVibration(void*);
void* errorGetDimming();
u32 errorGetDim(void*);
void errorSetDim(void*, u32);
void errorReportShow(const be<u8>*, u32);
void errorShow(ErrorShowArgs*);
u32 errorMenuDisc(ErrorMenuState*);
u32 errorMenuDiscCheck(ErrorMenuState*);
u32 errorDiscResolved(ErrorMenuState*);
u32 errorMenuStorage(ErrorMenuState*);
u32 errorStorageResolved(ErrorMenuState*);
s32 errorViewerStatus(ErrorViewerTask*);
u32 errorOtherResolved(ErrorViewerTask*, ErrorEntry*);
u32 errorReportClose(const be<u8>*);
void errorClose(u32);
u32 errorAccountSlot();
void errorLaunchAccount(ErrorLaunchArgs*);
void errorLaunchSettings(ErrorLaunchArgs*);
u32 errorResultCode(ErrorViewerTask*);
s32 errorResultType(u32);
ErrorViewerTask* errorSetStateResult(ErrorViewerTask*, u32);
void errorResumeSound();
void errorDestroyUI(ErrorViewerTask*);
void errorFreeWork(void*);
u32 errorExitResult(ErrorViewerTask*);
u32 errorFirstFSEvent(ErrorViewerTask*, u32, u32);
u32 errorSecondFSEvent(ErrorViewerTask*, u32, u32);
void errorRegisterGlobal(void*);
void errorInitByte(void*);
void errorInitFlag(void*);

WWHD_OFFSET(ErrorSafeString, mpData, 0x0);
WWHD_OFFSET(ErrorSafeString, __vtbl, 0x4);
WWHD_SIZE(ErrorSafeString, 0x8);
WWHD_OFFSET(ErrorDelegate, mpObject, 0x4);
WWHD_OFFSET(ErrorDelegate, mAdjustment, 0x8);
WWHD_OFFSET(ErrorDelegate, mIndex, 0xA);
WWHD_OFFSET(ErrorDelegate, mFunction, 0xC);
WWHD_SIZE(ErrorDelegate, 0x10);
WWHD_OFFSET(ErrorFSEvent, __vtbl, 0xC);
WWHD_OFFSET(ErrorFSEvent, mName, 0x10);
WWHD_OFFSET(ErrorFSEvent, mType, 0x18);
WWHD_OFFSET(ErrorFSEvent, mArgument, 0x1C);
WWHD_OFFSET(ErrorFSEvent, mDelegate, 0x20);
WWHD_SIZE(ErrorFSEvent, 0x40);
WWHD_SIZE(ErrorMutex, 0x3C);
WWHD_OFFSET(ErrorEntry, mFlags, 0x4);
WWHD_OFFSET(ErrorEntry, mFlag, 0x4);
WWHD_SIZE(ErrorEntry, 0x8);
WWHD_OFFSET(ErrorQueue, mpEntries, 0x0);
WWHD_OFFSET(ErrorQueue, mCapacity, 0x4);
WWHD_OFFSET(ErrorQueue, mHead, 0x8);
WWHD_OFFSET(ErrorQueue, mCount, 0xC);
WWHD_OFFSET(ErrorQueue, mStorage, 0x10);
WWHD_SIZE(ErrorQueue, 0x210);
WWHD_OFFSET(ErrorViewerTask, __vtbl, 0x20);
WWHD_OFFSET(ErrorViewerTask, mTaskVtable, 0x70);
WWHD_OFFSET(ErrorViewerTask, mRegistration, 0xC8);
WWHD_OFFSET(ErrorViewerTask, mpHeap, 0xCC);
WWHD_OFFSET(ErrorViewerTask, mFirstEvent, 0xD0);
WWHD_OFFSET(ErrorViewerTask, mSecondEvent, 0x110);
WWHD_OFFSET(ErrorViewerTask, mMutex, 0x150);
WWHD_OFFSET(ErrorViewerTask, mQueue, 0x18C);
WWHD_OFFSET(ErrorViewerTask, mState, 0x39C);
WWHD_OFFSET(ErrorViewerTask, mCurrent, 0x3A0);
WWHD_OFFSET(ErrorViewerTask, mpWork, 0x3A8);
WWHD_OFFSET(ErrorViewerTask, mRecoverFrames, 0x3AC);
WWHD_OFFSET(ErrorViewerTask, mSavedDim, 0x3B0);
WWHD_OFFSET(ErrorViewerTask, mDiscFrames, 0x3B4);
WWHD_OFFSET(ErrorViewerTask, mStorageFrames, 0x3B8);
WWHD_OFFSET(ErrorViewerTask, mClient, 0x3BC);
WWHD_OFFSET(ErrorViewerTask, mpWatchedVolume, 0x1ABC);
WWHD_OFFSET(ErrorViewerTask, mVolumeState, 0x1AC0);
WWHD_SIZE(ErrorViewerTask, 0x1AC4);
WWHD_OFFSET(ErrorCreateArgs, mpClient, 0xC);
WWHD_SIZE(ErrorCreateArgs, 0x10);
WWHD_OFFSET(ErrorShowArgs, mCode, 0x10);
WWHD_OFFSET(ErrorShowArgs, mRegion, 0x14);
WWHD_OFFSET(ErrorShowArgs, mFlag, 0x28);
WWHD_SIZE(ErrorShowArgs, 0x2C);
WWHD_SIZE(ErrorNamedString, 0xC);
WWHD_SIZE(ErrorAppearArgs, 0xC);
WWHD_OFFSET(ErrorLaunchArgs, mPage, 0x8);
WWHD_SIZE(ErrorLaunchArgs, 0xC);
WWHD_OFFSET(ErrorInputPad, mCount, 0xF00);
WWHD_OFFSET(ErrorInputPad, mStatus, 0xF04);
WWHD_SIZE(ErrorInputPad, 0xF08);
WWHD_OFFSET(ErrorInputPads, mPads, 0x1118);
WWHD_SIZE(ErrorInputPads, 0x4D38);
WWHD_OFFSET(ErrorVPadStatus, mTouch, 0x52);
WWHD_SIZE(ErrorVPadStatus, 0xAC);
WWHD_OFFSET(ErrorGamePad, mStatus, 0x14);
WWHD_OFFSET(ErrorGamePad, mCount, 0xAD4);
WWHD_OFFSET(ErrorGamePad, mResult, 0xAD8);
WWHD_SIZE(ErrorGamePad, 0xADC);
WWHD_OFFSET(ErrorInputArgs, mPads, 0x4);
WWHD_OFFSET(ErrorInputArgs, mStatus, 0x14);
WWHD_SIZE(ErrorInputArgs, 0xC0);
WWHD_OFFSET(ErrorCPULists, mCount, 0x1020);
WWHD_OFFSET(ErrorCPULists, mpLists, 0x1024);
WWHD_SIZE(ErrorCPULists, 0x1028);
WWHD_OFFSET(ErrorSettings, mRegion, 0x10);
WWHD_OFFSET(ErrorSettings, mLanguage, 0x14);
WWHD_SIZE(ErrorSettings, 0x18);
WWHD_OFFSET(ErrorHeader, mByte, 0x8);
WWHD_OFFSET(ErrorHeader, mFlag, 0x9);
WWHD_OFFSET(ErrorHeader, mWords, 0xC);
WWHD_SIZE(ErrorHeader, 0x1C);

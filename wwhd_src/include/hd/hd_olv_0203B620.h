// HD: Miiverse operation manager; this system has no GameCube counterpart.
// Native entry points retain their verified 32-bit register signatures. Views describe guest data.
#pragma once
#include "bindings.h"
#include "hd/hd_olv_02038E24.h"

namespace hd_olv_0203B620 {
union OperationDimensions {
    be<u32> mWord;
    struct {
        be<u16> mWidth, mHeight;
    };
};
WWHD_SIZE(OperationDimensions, 4);
struct OperationHeader {
    be<u32> mWords[4];
};
WWHD_SIZE(OperationHeader, 0x10);
struct OperationInitConstants {
    be<f32> mMinusPi, mPi;
};
WWHD_SIZE(OperationInitConstants, 8);
struct OperationHeaderValues {
    be<f32> mMinusPi, mPi;
    be<u8> mByte, mFlag;
    u8 mPadding[2];
};
WWHD_SIZE(OperationHeaderValues, 0xC);
struct OperationDestructorRecords {
    u8 mRecords[3][0xC];
};
WWHD_SIZE(OperationDestructorRecords, 0x24);
struct OperationAddressLabel {
    u8 mByte;
};
WWHD_SIZE(OperationAddressLabel, 1);
struct OperationDelegate {
    be<u32> mVtable, mObject;
    be<s16> mDelta, mVtableIndex;
    be<u32> mFunction;
};
WWHD_SIZE(OperationDelegate, 0x10);
struct OperationManagerVtable {
    u8 mPrefix[0xC];
    gfn mDestroy;
};
WWHD_SIZE(OperationManagerVtable, 0x10);
enum OperationState {
    STATE_STOPPED,
    STATE_IDLE,
    STATE_INITIALIZING,
    STATE_DOWNLOADING,
    STATE_EMPATHY,
    STATE_DEFERRED,
    STATE_CANCELLED
};
enum NativeArrayRoutine : u32 {
    BOTTLE_DESTRUCTOR = 0x02038FF0,
    POST_DATA_CONSTRUCTOR = 0xC0004F18
};
enum WorkerMessage {
    MESSAGE_INITIALIZE = 1,
    MESSAGE_DOWNLOAD = 3,
    MESSAGE_EMPATHY = 4,
    MESSAGE_UPDATE = 5
};
enum Limits { MAX_POSTS = 50, MAX_BOTTLES = 10, MAX_TEXTURES = 20, MAX_IMAGE_BYTES = 0x50000 };
struct OperationAppData {
    be<u8> mType, mFlag;
    u8 mPadding[2];
    be<u32> mID, mSize, mValue;
};
WWHD_SIZE(OperationAppData, 0x10);
struct OperationCriticalSection {
    u8 mStorage[0x3C];
};
WWHD_SIZE(OperationCriticalSection, 0x3C);
struct OperationTick {
    be<u32> mHigh, mLow;
};
WWHD_SIZE(OperationTick, 8);
// HD: sead disposer occupies the first sixteen bytes; the native vtable is near the tail.
struct OliveOperationMgr {
    /* 0x00 */ OliveDisposer mDisposer;
    /* 0x10 */ be<u8> mEmpathyQueued;
    /* 0x11 */ u8 mPadding[3];
    /* 0x14 */ be<u32> mWorkHeap;
    /* 0x18 */ be<u32> mWorkBuffer;
    /* 0x1C */ be<u32> mWorkSize;
    /* 0x20 */ be<u32> mCurlHeap;
    /* 0x24 */ OperationAppData mAppData;
    /* 0x34 */ be<u32> mThread;
    /* 0x38 */ be<u32> mState;
    /* 0x3C */ OperationCriticalSection mCriticalSection;
    /* 0x78 */ OperationTick mStartTick;
    /* 0x80 */ be<u32> mPostID;
    /* 0x84 */ be<u32> mErrorOverride;
    /* 0x88 */ be<u8> mCurlInitialized;
    /* 0x89 */ be<u8> mNsslInitialized;
    /* 0x8A */ be<u8> mSocketsInitialized;
    /* 0x8B */ be<u8> mAcConnected;
    /* 0x8C */ be<u8> mAcInitialized;
    /* 0x8D */ u8 mNetworkPadding[3];
    /* 0x90 */ be<u32> __vtbl;
    /* 0x94 */ u8 mTail[4];
};
WWHD_OFFSET(OliveOperationMgr, mEmpathyQueued, 0x10);
WWHD_OFFSET(OliveOperationMgr, mWorkHeap, 0x14);
WWHD_OFFSET(OliveOperationMgr, mWorkBuffer, 0x18);
WWHD_OFFSET(OliveOperationMgr, mWorkSize, 0x1C);
WWHD_OFFSET(OliveOperationMgr, mCurlHeap, 0x20);
WWHD_OFFSET(OliveOperationMgr, mPostID, 0x80);
WWHD_OFFSET(OliveOperationMgr, mAcInitialized, 0x8C);
WWHD_OFFSET(OliveOperationMgr, mAppData, 0x24);
WWHD_OFFSET(OliveOperationMgr, mThread, 0x34);
WWHD_OFFSET(OliveOperationMgr, mState, 0x38);
WWHD_OFFSET(OliveOperationMgr, mCriticalSection, 0x3C);
WWHD_OFFSET(OliveOperationMgr, mStartTick, 0x78);
WWHD_OFFSET(OliveOperationMgr, mErrorOverride, 0x84);
WWHD_OFFSET(OliveOperationMgr, mCurlInitialized, 0x88);
WWHD_OFFSET(OliveOperationMgr, __vtbl, 0x90);
WWHD_SIZE(OliveOperationMgr, 0x98);
struct OperationSingletonData {
    be<u32> mInstance, mCurlHeap, mDisposer;
    be<u8> mPostSucceeded, mPostDone, mPostPending;
};
WWHD_OFFSET(OperationSingletonData, mPostSucceeded, 0xC);
WWHD_SIZE(OperationSingletonData, 0xF);
struct OperationHeapVtable {
    u8 mPrefix[0x2C];
    gfn mAdjust;
    u8 mGapAllocate[4];
    gfn mAllocate;
    u8 mGapFree[4];
    gfn mFree;
    u8 mGapSize[0x7C - 0x40];
    gfn mFreeSize;
};
WWHD_OFFSET(OperationHeapVtable, mAllocate, 0x34);
WWHD_OFFSET(OperationHeapVtable, mFree, 0x3C);
WWHD_OFFSET(OperationHeapVtable, mFreeSize, 0x7C);
WWHD_SIZE(OperationHeapVtable, 0x80);
struct OperationHeap {
    u8 mPrefix[0xC];
    gptr<OperationHeapVtable> mpVtable;
    u32 adjust() {
        return gabi::call_ptr<u32>(mpVtable->mAdjust.get(), this);
    }
    u32 allocate(u32 size, u32 alignment) {
        return gabi::call_ptr<u32>(mpVtable->mAllocate.get(), this, size, alignment);
    }
    u32 free(u32 address) {
        return gabi::call_ptr<u32>(mpVtable->mFree.get(), this, address);
    }
    u32 freeSize(u32 alignment) {
        return gabi::call_ptr<u32>(mpVtable->mFreeSize.get(), this, alignment);
    }
};
WWHD_SIZE(OperationHeap, 0x10);
struct OperationThreadVtable {
    u8 mPrefix[0x1C];
    gfn mSend;
    u8 mGapStart[0x2C - 0x20];
    gfn mStart;
    u8 mGapCancel[4];
    gfn mCancel;
    u8 mGapJoin[4];
    gfn mJoin;
};
WWHD_OFFSET(OperationThreadVtable, mStart, 0x2C);
WWHD_OFFSET(OperationThreadVtable, mCancel, 0x34);
WWHD_OFFSET(OperationThreadVtable, mJoin, 0x3C);
WWHD_SIZE(OperationThreadVtable, 0x40);
struct OperationThread {
    u8 mPrefix[0xC];
    gptr<OperationThreadVtable> mpVtable;
    u8 mGap[0x80 - 0x10];
    be<u32> mState;
    u32 send(u32 message, u32 flags) {
        return gabi::call_ptr<u32>(mpVtable->mSend.get(), this, message, flags);
    }
    u32 start() {
        return gabi::call_ptr<u32>(mpVtable->mStart.get(), this);
    }
    u32 cancel(u32 flags) {
        return gabi::call_ptr<u32>(mpVtable->mCancel.get(), this, flags);
    }
    u32 join() {
        return gabi::call_ptr<u32>(mpVtable->mJoin.get(), this);
    }
};
WWHD_OFFSET(OperationThread, mState, 0x80);
WWHD_SIZE(OperationThread, 0x84);
struct OperationWorker {
    u8 mPrefix[0x1C];
    be<u32> __vtbl;
    u8 mTail[4];
};
WWHD_OFFSET(OperationWorker, __vtbl, 0x1C);
WWHD_SIZE(OperationWorker, 0x24);
struct OperationTuning {
    be<f32> mFirst, mSecond, mThird;
};
WWHD_SIZE(OperationTuning, 0xC);
struct OperationUploadParam {
    u8 mStorage[0x1400];
};
struct OperationPortalParam {
    u8 mStorage[0x1000];
};
struct OperationInitializeParam {
    u8 mStorage[0x44];
};
struct OperationEmpathyParam {
    u8 mStorage[0x48];
};
struct OperationTopicData {
    u8 mStorage[0x1000];
};
struct OperationListParam {
    u8 mStorage[0x1000];
};
struct OperationPostData {
    u8 mStorage[0xC208];
};
struct OperationPostArray {
    OperationPostData mPosts[MAX_POSTS];
};
WWHD_SIZE(OperationUploadParam, 0x1400);
WWHD_SIZE(OperationPortalParam, 0x1000);
WWHD_SIZE(OperationInitializeParam, 0x44);
WWHD_SIZE(OperationEmpathyParam, 0x48);
WWHD_SIZE(OperationTopicData, 0x1000);
WWHD_SIZE(OperationListParam, 0x1000);
WWHD_SIZE(OperationPostData, 0xC208);
WWHD_SIZE(OperationPostArray, MAX_POSTS * 0xC208);
struct OperationFixedString {
    be<u32> mBuffer, mVtable, mCapacity;
    be<u8> mCharacters[0x20];
};
WWHD_SIZE(OperationFixedString, 0x2C);
struct OperationFixedWideString {
    be<u32> mBuffer, mVtable, mCapacity;
    be<u16> mCharacters[0x34];
};
WWHD_SIZE(OperationFixedWideString, 0x74);
struct OperationSurface {
    be<u32> mDimension, mWidth, mHeight, mDepth, mMipLevels, mFormat, mAntiAlias, mUse, mImageSize,
        mImage;
    be<u32> mMipSize, mMipImage, mTileMode, mSwizzle, mAlignment, mPitch, mMipOffset[13];
};
WWHD_OFFSET(OperationSurface, mMipLevels, 0x10);
WWHD_SIZE(OperationSurface, 0x74);
struct OperationTexture {
    OperationSurface mSurface;
    be<u32> mFirstMip, mMipCount, mFirstSlice, mSliceCount, mComponents, mRegisters[5];
};
WWHD_OFFSET(OperationTexture, mFirstMip, 0x74);
WWHD_SIZE(OperationTexture, 0x9C);
struct OperationMemoTexture {
    be<u32> mPresent;
    be<u16> mWidth, mHeight;
    be<u8> mFormat;
    u8 mPadding[3];
    OperationTexture mTexture;
};
WWHD_OFFSET(OperationMemoTexture, mTexture, 0xC);
WWHD_SIZE(OperationMemoTexture, 0xA8);
struct OperationTgaImage {
    be<u32> mUnknown;
    OperationSurface mSurface;
    be<u8> mMinimumWidth, mMinimumHeight;
    u8 mPadding[0x8C - 0x7A];
    be<u32> __vtbl;
};
WWHD_OFFSET(OperationTgaImage, __vtbl, 0x8C);
WWHD_SIZE(OperationTgaImage, 0x90);
struct OperationJpegJob {
    u8 mPrefix[0x18];
    be<u8> mDone, mSucceeded;
};
WWHD_SIZE(OperationJpegJob, 0x1A);
struct OperationGlobalPointer {
    be<u32> mAddress;
};
WWHD_SIZE(OperationGlobalPointer, 4);
struct OperationSavedPosts {
    u8 mPrefix[0xF026C0];
    be<u8> mData;
};
WWHD_OFFSET(OperationSavedPosts, mData, 0xF026C0);
WWHD_SIZE(OperationSavedPosts, 0xF026C1);
struct OperationSavedAlbum {
    u8 mPrefix[0x12C0];
    be<u8> mData;
};
WWHD_OFFSET(OperationSavedAlbum, mData, 0x12C0);
WWHD_SIZE(OperationSavedAlbum, 0x12C1);
inline OliveOperationMgr* manager(u32 address) {
    return gabi::at<OliveOperationMgr>(address);
}
inline OperationHeap* heap(u32 address) {
    return gabi::at<OperationHeap>(address);
}
inline OperationThread* thread(u32 address) {
    return gabi::at<OperationThread>(address);
}
inline OliveBottle* bottle(u32 address) {
    return gabi::at<OliveBottle>(address);
}
inline OliveTextureSlot* textureSlot(u32 address) {
    return gabi::at<OliveTextureSlot>(address);
}
inline OliveCommentMgr* comments(u32 address) {
    return gabi::at<OliveCommentMgr>(address);
}
inline OperationMemoTexture* memo(OliveBottle* entry) {
    return reinterpret_cast<OperationMemoTexture*>(&entry->mMemo);
}
} // namespace hd_olv_0203B620

// HD: native SDK adapters keep the exact register widths of this translation unit.
void bottleOp_constructByte(u32);
void bottleOp_constructFlag(u32);

void bottleOp_OSReport(u32);
void bottleOp_OSReport(u32, u32);
void bottleOp_OSReport(u32, u32, u32);
void bottleOp_destroyWorkerBase(u32, u32);
u32 bottleOp_allocateInHeap(u32, u32, u32);
u32 bottleOp_constructWorker(u32);
void bottleOp_destroyArray(u32, u32, u32, u32, u32, u32);
void bottleOp_zeroMemory(u32, u32);
void bottleOp_constructCriticalSection(u32);
void bottleOp_constructTick(u32);
u32 bottleOp_allocateAlignedInHeap(u32, u32, u32);
void bottleOp_constructDisposer(u32, u32, u32);
void bottleOp_olv_Finalize(u32);
void bottleOp_destroyCriticalSection(u32, u32);
u32 bottleOp_getParentHeap();
u32 bottleOp_createHeap(u32, u32, u32, u32, u32);
u32 bottleOp_constructDelegateThread(u32, u32, u32, u32, u32, u32, u32, u32, u32);
Pair32 bottleOp_OSGetSystemTime();
u32 bottleOp_createWorkHeap(u32, u32, u32, u32, u32);
void bottleOp_olv_Cancel();
u32 bottleOp_olv_IsInitialized();
void bottleOp_olv_Finalize();
void bottleOp_curl_global_cleanup();
void bottleOp_NSSLFinish();
void bottleOp_socket_lib_finish();
void bottleOp_ac_Close();
void bottleOp_ac_Finalize();
u32 bottleOp_olv_GetErrorCode(u32);
void bottleOp_showError(u32, u32, u32);
u32 bottleOp_getSystemUI();
void bottleOp_setSystemUIReady(u32, u32);
void bottleOp_setSystemUIBusy(u32, u32);
void bottleOp_clearError(u32);
void bottleOp_olv_UploadPostParam_ct(u32);
u32 bottleOp_getReadAlbum(u32);
u32 bottleOp_getAlbumImage(u32, u32);
void bottleOp_olv_UploadParam_SetExternalImageData(u32, u32, u32);
u32 bottleOp_getAlbumData(u32, u32);
u32 bottleOp_getAlbumDataType(u32);
u32 bottleOp_getAlbumFlag(u32, u32);
u32 bottleOp_getAlbumID(u32, u32);
u32 bottleOp_getAlbumDataSize(u32);
u32 bottleOp_getAlbumDataValue(u32);
void bottleOp_olv_UploadParam_SetAppData(u32, u32, u32);
void bottleOp_assureWideTermination(u32);
void bottleOp_OSBlockMove_(u32, u32, u32, u32);
void bottleOp_getTopicTag(u32, u32);
void bottleOp_olv_UploadParam_SetTopicTag(u32, u32);
void bottleOp_olv_UploadParam_SetSearchKey(u32, u32, u32);
void bottleOp_olv_UploadPostParam_SetBodyTextMaxLength(u32, u32);
void bottleOp_olv_UploadPostParam_SetFlags(u32, u32);
u32 bottleOp_olv_UploadPostDataByPostApp(u32);
void bottleOp_olv_UploadParamBase_dt(u32, u32);
void bottleOp_olv_PortalParam_ct(u32);
void bottleOp_olv_PortalParam_SetPostId(u32, u32);
u32 bottleOp_olv_StartPortalApp(u32);
u32 bottleOp_act_GetPrincipalId();
void bottleOp_olv_PortalParam_SetUserPid(u32, u32);
u32 bottleOp_olv_GetResultByPostApp();
u32 bottleOp_getWriteAlbum(u32);
void bottleOp_setAlbumPosted(u32, u32);
u32 bottleOp_checkMenuEvent(u32);
void bottleOp_setMenuEvent(u32, u32);
u32 bottleOp_getAllocationSize(u32, u32);
void bottleOp_OSBlockSet_(u32, u32, u32);
u32 bottleOp_ac_Initialize();
void bottleOp_ac_GetLastErrorCode(u32);
u32 bottleOp_ac_Connect();
u32 bottleOp_socket_lib_init();
u32 bottleOp_NSSLInit();
u32 bottleOp_curl_global_init_mem(u32, u32, u32, u32, u32, u32);
void bottleOp_olv_InitializeParam_ct(u32);
u32 bottleOp_olv_InitializeParam_SetWork(u32, u32, u32);
u32 bottleOp_olv_Initialize(u32);
void bottleOp_olv_SetReportTypes(u32);
void bottleOp_ProcUIRegisterCallback(u32, u32, u32, u32);
void bottleOp_olv_EmpathyParam_ct(u32);
void bottleOp_olv_EmpathyParam_SetPostId(u32, u32);
u32 bottleOp_olv_UploadEmpathyToPostData(u32);
Pair32 bottleOp_getElapsedTicks(u32);
Pair32 bottleOp_divideTicks(u32, u32, u32, u32);
void bottleOp_enterCriticalSection(u32);
void bottleOp_leaveCriticalSection(u32);
void bottleOp_destroyDisposer(u32, u32);
void bottleOp_constructArray(u32, u32, u32, u32);
void bottleOp_registerGlobal(u32);
void bottleOp_olv_TopicData_ct(u32);
void bottleOp_olv_ListParam_ct(u32);
void bottleOp_olv_ListParam_SetPostDataMaxNum(u32, u32);
void bottleOp_olv_ListParam_SetFlags(u32, u32);
u32 bottleOp_olv_DownloadPostDataList(u32, u32, u32, u32, u32);
void bottleOp_getSavedPostID(u32, u32, u32);
u32 bottleOp_olv_Data_GetPostId(u32);
u32 bottleOp_getSavedCommentCount(u32, u32);
u32 bottleOp_olv_Post_GetCommentCount(u32);
void bottleOp_setSavedPostID(u32, u32, u32);
void bottleOp_setSavedCommentCount(u32, u32, u32);
void bottleOp_flushSave(u32);
u32 bottleOp_olv_Data_GetUserPid(u32);
u32 bottleOp_getImageHeap();
Pair32 bottleOp_olv_Data_GetPostDate(u32);
void bottleOp_OSTicksToCalendarTime(u32, u32, u32);
u32 bottleOp_olv_Data_GetMiiNickname(u32);
u32 bottleOp_olv_Data_GetTopicTag(u32);
void bottleOp_setBottleTopic(u32, u32);
u32 bottleOp_olv_Data_TestFlags(u32, u32);
u32 bottleOp_olv_Data_GetBodyText(u32, u32, u32);
u32 bottleOp_olv_Post_GetEmpathyCount(u32);
u32 bottleOp_olv_Data_GetBodyMemo(u32, u32, u32, u32);
u32 bottleOp_constructTgaImage(u32);
void bottleOp_decodeTgaImage(u32, u32, u32, u32);
void bottleOp_GX2InitTextureRegs(u32);
void bottleOp_getTextureFormat(u32, u32, u32);
u32 bottleOp_olv_Data_GetExternalImageDataSize(u32);
u32 bottleOp_olv_Data_DownloadExternalImageData(u32, u32, u32, u32);
u32 bottleOp_olv_Data_GetAppData(u32, u32, u32, u32);
u32 bottleOp_startJpegJob(u32, u32, u32, u32, u32);
u32 bottleOp_getBottleMode(u32);
void bottleOp_commitBottleDownload(u32);
u32 bottleOp_checkItem(u32, u32);
u32 bottleOp_countDownloadedBottles(u32);
void bottleOp_OSReport(u32, u32, u32, u32);
void bottleOp_olv_ListParam_SetSearchKey(u32, u32, u32);
u32 bottleOp_allocateObject(u32);
void bottleOp_deleteObject(u32);

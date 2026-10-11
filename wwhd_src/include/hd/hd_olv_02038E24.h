// HD: OliveCommentMgr has no GameCube counterpart. These views are private to its TU.
#pragma once
#include "bindings.h"
#include <cstddef>

enum OliveCommentLimits { SHOWN_BOTTLES = 10, BOTTLE_TEXTURES = 20, STAGE_TOPICS = 59 };
enum OliveTextureState {
    TEXTURE_EMPTY = 0,
    TEXTURE_READY = 1,
    TEXTURE_SELECTED = 2,
    TEXTURE_SHOWN = 3,
    TEXTURE_RELEASED = 4
};
struct OliveStringVtable {
    u8 prefix[0x14];
    gfn mAssureTermination;
};
struct OliveSafeString {
    gptr<const be<u8>> mpData;
    be<u32> __vtbl;
    void assureTermination() {
        auto* vt = gabi::at<OliveStringVtable>(__vtbl);
        gabi::call_ptr<u32>(vt->mAssureTermination.get(), this);
    }
};
struct OliveWideString {
    gptr<be<u16>> mpData;
    be<u32> __vtbl;
    be<s32> mCapacity;
    void assureTermination() {
        auto* vt = gabi::at<OliveStringVtable>(__vtbl);
        gabi::call_ptr<u32>(vt->mAssureTermination.get(), this);
    }
};
template <u32 N> struct OliveFixedWideString : OliveWideString {
    be<u16> mBuffer[N];
};
struct OliveTextureData {
    gptr<void> mpImage;
    be<u16> mWidth, mHeight;
    u8 gap[0x30 - 8];
    gptr<void> mpSurface;
    u8 tail[0xA8 - 0x34];
};
struct OliveBottle {
    OliveFixedWideString<11> mName;
    u8 padName[2];
    OliveFixedWideString<51> mTopic;
    u8 padTopic[2];
    OliveFixedWideString<211> mBody;
    u8 padBody[2];
    be<u8> mHasMemo, mSpoiler;
    u8 pad24E[2];
    be<u32> mEmpathyCount, mCommentCount;
    be<u8> mEmpathy, mHasScreenshot, mRead, mValid, mApplicationFlag;
    u8 pad25D[3];
    be<u32> mApplicationID;
    be<u8> mApplicationType;
    u8 pad265[3];
    be<u32> mApplicationValue, mApplicationSize;
    gptr<void> mpScreenshotBuffer;
    OliveTextureData mScreenshot, mMemo;
    be<u8> mPostID[0x20];
    be<s8> mTextureSlot;
    be<u8> mFlag;
    u8 pad3E6[2];
    be<u32> __vtbl;
    void copy(OliveBottle* source);
    void setTopic(OliveWideString* topic);
    void clear();
};
struct OliveTextureSlot {
    gptr<void> mpMemo, mpImage, mpScreenshot;
    be<u32> mState;
};
struct OliveDisposer {
    u8 prefix[12];
    be<u32> __vtbl;
};
struct OliveHeapVtable {
    u8 prefix[0x2C];
    gfn mAdjust;
};
struct OliveHeap {
    u8 prefix[12];
    gptr<OliveHeapVtable> mpVtable;
    void adjust() { gabi::call_ptr<u32>(mpVtable->mAdjust.get(), this); }
};
struct OliveManagerVtable {
    u8 prefix[0x14];
    gfn mDestructor;
};
struct OliveCommentMgr {
    be<u32> __vtbl;
    OliveDisposer mDisposer;
    OliveBottle mShown[SHOWN_BOTTLES], mDownloaded[SHOWN_BOTTLES];
    OliveTextureSlot mTextures[BOTTLE_TEXTURES];
    be<u32> mRotation;
    be<u8> mFirstRotation;
    u8 pad4FC9[3];
    be<u32> mDownloadCount;
    gptr<OliveHeap> mpHeap;
    be<u8> mDownloadReset, mNotification, mNotificationRead, mAccountSlot;
    be<u32> mRotationState;
    // HD: preserve guest 32-bit wraparound and the original out-of-range slot fallback.
    OliveTextureSlot* texture(s32 index) {
        return gabi::at<OliveTextureSlot>(
            gabi::ea(mTextures) +
            ((u32)index < BOTTLE_TEXTURES ? (u32)index * sizeof(OliveTextureSlot) : 0));
    }
    OliveBottle* shown(u32 index) {
        return gabi::at<OliveBottle>(gabi::ea(mShown) +
                                     (index < SHOWN_BOTTLES ? index * sizeof(OliveBottle) : 0));
    }
    void destroy(u32 flags) {
        auto* vt = gabi::at<OliveManagerVtable>(__vtbl);
        gabi::call_ptr<u32>(vt->mDestructor.get(), this, flags);
    }
    void init();
    u32 countUnread();
    u32 notifyCount();
    u32 hasNotify();
    u32 unreadCount();
    u32 hasUnread();
    u32 countShown();
    u32 countReady();
    s32 downloadedEnd();
    u32 countSelected();
    u32 promoteOne();
    bool downloadedReady(u32 index, u32 state);
    void syncTextures();
    void resetDownloaded();
    void reset();
    u32 slotOk();
    u32 slotFirst();
    void shiftDownloaded();
    void rotate();
    f32 interval();
    f32 rate();
    void topicTag(OliveWideString* out);
    void sendEntry(OliveBottle* entry);
    void publish();
};
struct OliveDelegate {
    be<u32> __vtbl;
    gptr<void> mpObject;
    be<s16> mAdjustment, mIndex;
    union {
        be<u32> mFunction;
        struct {
            be<u16> mReserved;
            be<s16> mVtableOffset;
        };
    };
};
struct OliveMenuManager {
    u8 prefix[0x218];
    gptr<struct OliveStageInfo> mpStage;
    u8 gap[8];
    be<u32> mKind;
};
struct OliveStageInfo {
    u8 prefix[0x3E];
    be<s8> mTopic;
};
struct OliveSave {
    u8 prefix[0xD4];
    u8 mCollect;
    u8 gap[0x12C0 - 0xD5];
    u8 mMode;
};
struct OliveStageDescriptor {
    u8 prefix[9];
    be<u8> mType;
};
struct OliveStageVtable {
    u8 prefix[0x15C];
    gfn mGetInfo;
};
struct OliveStage {
    gptr<OliveStageVtable> mpVtable;
    OliveStageDescriptor* info() {
        return gabi::call_ptr<OliveStageDescriptor*>(mpVtable->mGetInfo.get(), this);
    }
};
struct OliveGameInfo {
    u8 prefix[0x5134];
    be<u8> mStageName[8];
    u8 gap[0x5150 - 0x513C];
    OliveStage mStage;
};
struct OliveMessageText {
    gptr<void> mpEntries;
    be<u32> mCount;
};
struct OliveMessageData {
    u8 prefix[8];
    gptr<OliveMessageText> mpTexts;
};
struct OliveMessageInfo {
    u8 prefix[0x10C];
    be<u32> mTextIndex;
    u8 pad[4];
};
struct OliveMessageFrame {
    OliveMessageData mData;
    OliveMessageInfo mInfo;
    be<u32> mWork[0x24A - (0x120 / 4)];
};
struct OliveMessageManager {
    u8 prefix[0x10];
    u8 mMutex;
};
struct OliveSleepSpan {
    be<u32> mHigh, mLow;
};
struct OliveHeader {
    be<f32> mMinAngle, mMaxAngle, mNearMin, mFarMin, mNearMax, mFarMax;
    be<u8> mByte, mFlag;
    u8 pad[2];
    be<u32> mWords[4];
};
struct OliveInitConstants {
    be<f32> mMinAngle, mMaxAngle, mFar, mNear;
};
struct OliveStageNames {
    OliveSafeString mNames[STAGE_TOPICS];
};
struct OliveStageLiterals {
    be<u8> mNames[STAGE_TOPICS][20];
};
struct OliveDestructorRecords {
    u8 mRecords[3][12];
};
struct OliveCategoryTopics {
    be<s8> mTopics[16];
};

struct OliveSafeWideString {
    gptr<be<u16>> mpData;
    be<u32> __vtbl;
};
struct OliveLiteral {
    be<u8> mText[64];
};
OliveBottle* oliveAllocateBottle(u32);
OliveCommentMgr* oliveAllocateManager(u32);
void* oliveAllocateBytes(u32);
void oliveDelete(void*);
void oliveDestroyWorker(void*, u32);
void oliveConvertTopic(void*, OliveWideString*, be<u16>*);
void oliveConstructArray(OliveBottle*, u32, u32, u32);
OliveCommentMgr* oliveAllocateManagerInHeap(u32, void*, u32);
void oliveConstructDisposer(OliveDisposer*, void*, u32);
void* oliveGetParentHeap();
OliveHeap* oliveCreateHeap(u32, OliveSafeString*, void*, u32, u32);
void* oliveAllocateInHeap(u32, OliveHeap*, u32);
u32 oliveGetAccountSlot();
void oliveQueryAccount(void*, u32);
void oliveQueryAccountSlot(be<u8>*, u32);
u32 oliveMenuBusy(void*);
u32 oliveMenuEvent(void*);
be<u8>* oliveGetSaveMode(void*);
OliveGameInfo* oliveGetGameInfo();
u32 oliveCheckCollect(void*, u32, u32);
void oliveConstructMessage(OliveMessageData*);
void oliveSetMessage(OliveMessageData*, OliveSafeString*, OliveSafeString*);
void oliveLookupMessage(void*, OliveMessageData*, void*, void*);
be<u16>* oliveGetMessageText(void*, u32);
void oliveTerminateWideString(OliveSafeWideString*);
void oliveSendMessage(OliveMessageManager*, void*, u32, u32, u32, u32);
u32 oliveMessagesAccepted(OliveMessageManager*);
void oliveSleep(OliveSleepSpan*);
void oliveLockMessageMutex(void*, void*);
void oliveDestroyDisposer(OliveDisposer*, u32);
void oliveRegisterGlobal(void*);
void oliveInitByte(void*);
void oliveInitFlag(void*);
void oliveCopyMemory(void*, void*, u32, u32);
void oliveClearMemory(void*, u32, u32);

WWHD_OFFSET(OliveSafeString, mpData, 0x0);
WWHD_OFFSET(OliveSafeString, __vtbl, 0x4);
WWHD_SIZE(OliveSafeString, 0x8);
WWHD_OFFSET(OliveWideString, mpData, 0x0);
WWHD_OFFSET(OliveWideString, __vtbl, 0x4);
WWHD_OFFSET(OliveWideString, mCapacity, 0x8);
WWHD_SIZE(OliveWideString, 0xC);
WWHD_OFFSET(OliveTextureData, mWidth, 0x4);
WWHD_OFFSET(OliveTextureData, mHeight, 0x6);
WWHD_OFFSET(OliveTextureData, mpSurface, 0x30);
WWHD_SIZE(OliveTextureData, 0xA8);
WWHD_OFFSET(OliveBottle, mName, 0x0);
WWHD_OFFSET(OliveBottle, mTopic, 0x24);
WWHD_OFFSET(OliveBottle, mBody, 0x98);
WWHD_OFFSET(OliveBottle, mHasMemo, 0x24C);
WWHD_OFFSET(OliveBottle, mSpoiler, 0x24D);
WWHD_OFFSET(OliveBottle, mEmpathyCount, 0x250);
WWHD_OFFSET(OliveBottle, mCommentCount, 0x254);
WWHD_OFFSET(OliveBottle, mEmpathy, 0x258);
WWHD_OFFSET(OliveBottle, mHasScreenshot, 0x259);
WWHD_OFFSET(OliveBottle, mRead, 0x25A);
WWHD_OFFSET(OliveBottle, mValid, 0x25B);
WWHD_OFFSET(OliveBottle, mApplicationFlag, 0x25C);
WWHD_OFFSET(OliveBottle, mApplicationID, 0x260);
WWHD_OFFSET(OliveBottle, mApplicationType, 0x264);
WWHD_OFFSET(OliveBottle, mApplicationValue, 0x268);
WWHD_OFFSET(OliveBottle, mApplicationSize, 0x26C);
WWHD_OFFSET(OliveBottle, mpScreenshotBuffer, 0x270);
WWHD_OFFSET(OliveBottle, mScreenshot, 0x274);
WWHD_OFFSET(OliveBottle, mMemo, 0x31C);
WWHD_OFFSET(OliveBottle, mPostID, 0x3C4);
WWHD_OFFSET(OliveBottle, mTextureSlot, 0x3E4);
WWHD_OFFSET(OliveBottle, mFlag, 0x3E5);
WWHD_OFFSET(OliveBottle, __vtbl, 0x3E8);
WWHD_SIZE(OliveBottle, 0x3EC);
WWHD_OFFSET(OliveTextureSlot, mpMemo, 0x0);
WWHD_OFFSET(OliveTextureSlot, mpImage, 0x4);
WWHD_OFFSET(OliveTextureSlot, mpScreenshot, 0x8);
WWHD_OFFSET(OliveTextureSlot, mState, 0xC);
WWHD_SIZE(OliveTextureSlot, 0x10);
WWHD_OFFSET(OliveCommentMgr, mDisposer, 0x4);
WWHD_OFFSET(OliveCommentMgr, mShown, 0x14);
WWHD_OFFSET(OliveCommentMgr, mDownloaded, 0x274C);
WWHD_OFFSET(OliveCommentMgr, mTextures, 0x4E84);
WWHD_OFFSET(OliveCommentMgr, mRotation, 0x4FC4);
WWHD_OFFSET(OliveCommentMgr, mFirstRotation, 0x4FC8);
WWHD_OFFSET(OliveCommentMgr, mDownloadCount, 0x4FCC);
WWHD_OFFSET(OliveCommentMgr, mpHeap, 0x4FD0);
WWHD_OFFSET(OliveCommentMgr, mDownloadReset, 0x4FD4);
WWHD_OFFSET(OliveCommentMgr, mNotification, 0x4FD5);
WWHD_OFFSET(OliveCommentMgr, mNotificationRead, 0x4FD6);
WWHD_OFFSET(OliveCommentMgr, mAccountSlot, 0x4FD7);
WWHD_OFFSET(OliveCommentMgr, mRotationState, 0x4FD8);
WWHD_SIZE(OliveCommentMgr, 0x4FDC);
WWHD_OFFSET(OliveDelegate, mpObject, 0x4);
WWHD_OFFSET(OliveDelegate, mAdjustment, 0x8);
WWHD_OFFSET(OliveDelegate, mIndex, 0xA);
WWHD_OFFSET(OliveDelegate, mFunction, 0xC);
WWHD_OFFSET(OliveDelegate, mVtableOffset, 0xE);
WWHD_SIZE(OliveDelegate, 0x10);
WWHD_OFFSET(OliveMenuManager, mpStage, 0x218);
WWHD_OFFSET(OliveMenuManager, mKind, 0x224);
WWHD_SIZE(OliveMenuManager, 0x228);
WWHD_OFFSET(OliveStageInfo, mTopic, 0x3E);
WWHD_SIZE(OliveStageInfo, 0x3F);
WWHD_OFFSET(OliveSave, mCollect, 0xD4);
WWHD_OFFSET(OliveSave, mMode, 0x12C0);
WWHD_SIZE(OliveSave, 0x12C1);
WWHD_OFFSET(OliveGameInfo, mStageName, 0x5134);
WWHD_OFFSET(OliveGameInfo, mStage, 0x5150);
WWHD_SIZE(OliveGameInfo, 0x5154);
WWHD_OFFSET(OliveMessageData, mpTexts, 0x8);
WWHD_SIZE(OliveMessageData, 0xC);
WWHD_OFFSET(OliveMessageInfo, mTextIndex, 0x10C);
WWHD_SIZE(OliveMessageInfo, 0x114);
WWHD_OFFSET(OliveMessageFrame, mData, 0x0);
WWHD_OFFSET(OliveMessageFrame, mInfo, 0xC);
WWHD_OFFSET(OliveMessageFrame, mWork, 0x120);
WWHD_SIZE(OliveMessageFrame, 0x928);
WWHD_OFFSET(OliveHeader, mByte, 0x18);
WWHD_OFFSET(OliveHeader, mFlag, 0x19);
WWHD_OFFSET(OliveHeader, mWords, 0x1C);
WWHD_SIZE(OliveHeader, 0x2C);
WWHD_OFFSET(OliveSafeWideString, mpData, 0x0);
WWHD_OFFSET(OliveSafeWideString, __vtbl, 0x4);
WWHD_SIZE(OliveSafeWideString, 0x8);

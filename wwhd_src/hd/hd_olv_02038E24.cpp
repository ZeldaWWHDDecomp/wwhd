// HD: Miiverse comment storage, download rotation and message publishing have no GameCube
// counterpart.
#include "hd/hd_olv_02038E24.h"

static OliveLiteral sMessageMutexName;
static OliveLiteral sHeapName;
static OliveLiteral sSeaStage;
static OliveLiteral sHiddenStage;
static OliveLiteral sPublishHeader;
static OliveLiteral sGroupName;
static OliveCommentMgr* sInstance;
static OliveDisposer* sDisposer;
static void* sMessageConverter;
static void* sMessageLookup;
static OliveMessageManager* sMessageManager;
static void* sMenuState;
static OliveMenuManager* sMenuManager;
static OliveSave* sSave;
static u32 sDebugAccount;
static u32 sDebugSlotOk;
static u32 sDebugSlotFirst;
static f32 sIdleInterval;
static f32 sNotifyInterval;
static f32 sRateScale;
static OliveHeader sHeader;
static OliveInitConstants sInitConstants;
static OliveDestructorRecords sDestructorRecords;
static OliveStageNames sStageNames;
static OliveStageLiterals sStageLiterals;
static OliveSafeString sMessageGroup;
static OliveCategoryTopics sCategoryTopics;

static s32 wideLength(const be<u16>* text) {
    s32 length = 0;
    if (*text == 0)
        return 0;
    for (;;) {
        length++;
        text++;
        if (length > 0x40000)
            return 0;
        if (*text == 0)
            return length;
    }
}
static void copyWideString(OliveWideString* destination, OliveWideString* source) {
    auto* vtable = gabi::at<OliveStringVtable>(source->__vtbl);
    be<u16>* buffer = destination->mpData;
    gabi::call_ptr<u32>(vtable->mAssureTermination.get(), source);
    s32 length = wideLength(source->mpData.get());
    s32 capacity = destination->mCapacity;
    auto* secondVtable = gabi::at<OliveStringVtable>(source->__vtbl);
    if (!(length < capacity))
        length = capacity - 1;
    gabi::call_ptr<u32>(secondVtable->mAssureTermination.get(), source);
    oliveCopyMemory(buffer, source->mpData.get(), (u32)length * 2, 0);
    *gabi::at<be<u16>>(gabi::ea(buffer) + (u32)length * 2) = 0;
}
template <u32 N> static void constructWideString(OliveFixedWideString<N>* text, u32 vtable) {
    text->mBuffer[N - 1] = 0;
    text->mpData = text->mBuffer;
    text->mBuffer[0] = 0;
    text->__vtbl = vtable;
    text->mCapacity = N;
}
/* 02038E24 */
static u32 invokeOliveDelegate(OliveDelegate* delegate, u32 first, u32 second) {
    void* object = delegate->mpObject;
    if (!object)
        return gabi::ea(delegate);
    s32 index = delegate->mIndex;
    if (index == 0)
        return gabi::ea(delegate);
    void* adjusted = gabi::at<void>(gabi::ea(object) + (s32)delegate->mAdjustment);
    u32 function;
    if (index < 0)
        function = delegate->mFunction;
    else {
        auto* table = gabi::at<gptr<be<u32>>>(gabi::ea(adjusted) + (s32)delegate->mVtableOffset);
        function = (*table).get()[index * 2 + 1];
    }
    return gabi::call_ptr<u32>(function, adjusted, first, second);
}
/* 02038E78 */
static void destroyOliveWorkerFromBase(void* base, u32 flags) {
    oliveDestroyWorker(gabi::at<void>(gabi::ea(base) - 0x10), flags);
}
/* 02038E80 */
static OliveBottle* constructOliveBottle(OliveBottle* entry) {
    if (!entry) {
        entry = oliveAllocateBottle(sizeof(OliveBottle));
        if (!entry)
            return nullptr;
    }
    entry->__vtbl = 0x10005CC8;
    constructWideString(&entry->mName, 0x100056C0);
    constructWideString(&entry->mTopic, 0x100056F0);
    constructWideString(&entry->mBody, 0x10005720);
    entry->mHasMemo = 0;
    entry->mMemo.mpSurface = nullptr;
    entry->mTextureSlot = -1;
    entry->mValid = 0;
    entry->mMemo.mWidth = 0;
    entry->mApplicationID = 0;
    entry->mScreenshot.mpSurface = nullptr;
    entry->mRead = 0;
    entry->mMemo.mpImage = nullptr;
    entry->mApplicationType = 0;
    entry->mFlag = 0;
    entry->mHasScreenshot = 0;
    entry->mScreenshot.mHeight = 0;
    entry->mApplicationValue = 0;
    entry->mMemo.mHeight = 0;
    entry->mEmpathy = 0;
    entry->mScreenshot.mWidth = 0;
    entry->mApplicationSize = 0;
    entry->mSpoiler = 0;
    entry->mCommentCount = 0;
    entry->mScreenshot.mpImage = nullptr;
    entry->mpScreenshotBuffer = nullptr;
    entry->mEmpathyCount = 0;
    entry->mApplicationFlag = 0;
    return entry;
}
/* 02038FF0 */
static void destroyOliveBottleStorage(void* entry, u32 flags) {
    if (entry && (flags & 1))
        oliveDelete(entry);
}
/* 02039004 */
void OliveBottle::copy(OliveBottle* source) {
    copyWideString(&mName, &source->mName);
    source->mTopic.assureTermination();
    oliveConvertTopic(sMessageConverter.get(), &mTopic, source->mTopic.mpData.get());
    copyWideString(&mBody, &source->mBody);
    mHasMemo = (u8)source->mHasMemo;
    mSpoiler = (u8)source->mSpoiler;
    mEmpathyCount = (u32)source->mEmpathyCount;
    mCommentCount = (u32)source->mCommentCount;
    mEmpathy = (u8)source->mEmpathy;
    mHasScreenshot = (u8)source->mHasScreenshot;
    mRead = (u8)source->mRead;
    mValid = (u8)source->mValid;
    mApplicationFlag = (u8)source->mApplicationFlag;
    mApplicationID = (u32)source->mApplicationID;
    mApplicationType = (u8)source->mApplicationType;
    mApplicationValue = (u32)source->mApplicationValue;
    mApplicationSize = (u32)source->mApplicationSize;
    mTextureSlot = (s8)source->mTextureSlot;
    mFlag = (u8)source->mFlag;
    mpScreenshotBuffer = source->mpScreenshotBuffer.get();
    oliveCopyMemory(&mScreenshot, &source->mScreenshot, sizeof(OliveTextureData), 0);
    oliveCopyMemory(&mMemo, &source->mMemo, sizeof(OliveTextureData), 0);
    oliveCopyMemory(mPostID, source->mPostID, sizeof(mPostID), 0);
}
/* 02039240 */
void OliveBottle::setTopic(OliveWideString* topic) {
    topic->assureTermination();
    oliveConvertTopic(sMessageConverter.get(), &mTopic, topic->mpData.get());
    if ((s32)mTopic.mCapacity <= 50)
        mTopic.assureTermination();
    else
        mTopic.mpData.get()[50] = 0;
}
/* 02039300 */
static OliveCommentMgr* constructOliveCommentMgr(OliveCommentMgr* manager) {
    if (!manager) {
        manager = oliveAllocateManager(sizeof(OliveCommentMgr));
        if (!manager)
            return nullptr;
    }
    manager->__vtbl = 0x10005738;
    OliveBottle* shown =
        gabi::at<OliveBottle>(gabi::ea(manager) + offsetof(OliveCommentMgr, mShown));
    if (!shown)
        shown = oliveAllocateBottle(sizeof(manager->mShown));
    if (shown)
        oliveConstructArray(shown, SHOWN_BOTTLES, sizeof(OliveBottle),
                            gabi::guest_fn((const void*)constructOliveBottle));
    OliveBottle* downloaded =
        gabi::at<OliveBottle>(gabi::ea(manager) + offsetof(OliveCommentMgr, mDownloaded));
    if (!downloaded)
        downloaded = oliveAllocateBottle(sizeof(manager->mDownloaded));
    if (downloaded)
        oliveConstructArray(downloaded, SHOWN_BOTTLES, sizeof(OliveBottle),
                            gabi::guest_fn((const void*)constructOliveBottle));
    if ((u32)(gabi::ea(manager) + offsetof(OliveCommentMgr, mTextures)) == 0)
        oliveAllocateBytes(sizeof(manager->mTextures));
    manager->mRotationState = 0;
    manager->mFirstRotation = 1;
    manager->mNotification = 0;
    manager->mRotation = 0;
    manager->mpHeap = nullptr;
    manager->mAccountSlot = 0;
    manager->mNotificationRead = 0;
    manager->mDownloadCount = 0;
    manager->mDownloadReset = 0;
    return manager;
}
/* 020393E0 */
static OliveCommentMgr* createOliveCommentMgr(void* heap) {
    OliveCommentMgr* instance = sInstance;
    if (instance)
        return instance;
    OliveCommentMgr* allocation = oliveAllocateManagerInHeap(sizeof(OliveCommentMgr), heap, 4);
    // HD: keep the original guest +4 disposer address even when allocation returns zero.
    OliveDisposer* disposer =
        gabi::at<OliveDisposer>(gabi::ea(allocation) + offsetof(OliveCommentMgr, mDisposer));
    if (disposer) {
        oliveConstructDisposer(disposer, heap, 3);
        disposer->__vtbl = 0x10005CD8;
    }
    sDisposer = disposer;
    OliveCommentMgr* result = allocation;
    if (allocation)
        result = constructOliveCommentMgr(allocation);
    sInstance = result;
    return result;
}
void OliveBottle::clear() {
    *mName.mpData = 0;
    *mTopic.mpData = 0;
    *mBody.mpData = 0;
    mHasMemo = 0;
    mSpoiler = 0;
    mEmpathyCount = 0;
    mCommentCount = 0;
    mEmpathy = 0;
    mHasScreenshot = 0;
    mRead = 0;
    mValid = 0;
    mApplicationFlag = 0;
    mApplicationID = 0;
    mApplicationType = 0;
    mApplicationValue = 0;
    mApplicationSize = 0;
    mpScreenshotBuffer = nullptr;
    mScreenshot.mpSurface = nullptr;
    mMemo.mpSurface = nullptr;
    mTextureSlot = -1;
    oliveClearMemory(mPostID, 0, sizeof(mPostID));
}
static void clearBottles(OliveBottle* entries) {
    for (u32 i = 0; i < SHOWN_BOTTLES; i++)
        entries[i].clear();
}
/* 02039480 */
void OliveCommentMgr::init() {
    clearBottles(mShown);
    clearBottles(mDownloaded);
    gabi::Local<OliveSafeString> name;
    name->__vtbl = 0x100055F0;
    name->mpData = sHeapName.mText;
    void* parent = oliveGetParentHeap();
    OliveHeap* heap = oliveCreateHeap(0, name.get(), parent, 1, 0);
    mpHeap = heap;
    OliveHeap* currentHeap = heap;
    for (u32 i = 0; i < BOTTLE_TEXTURES; i++) {
        mTextures[i].mpMemo = oliveAllocateInHeap(0x8000, currentHeap, 0x800);
        mTextures[i].mpImage = oliveAllocateInHeap(0x28000, mpHeap.get(), 0x2000);
        mTextures[i].mpScreenshot = oliveAllocateInHeap(0x50000, mpHeap.get(), 0x40);
        mTextures[i].mState = TEXTURE_EMPTY;
        currentHeap = mpHeap;
    }
    currentHeap->adjust();
    mAccountSlot = 2;
    gabi::Local<be<u8>[4]> result;
    u32 slot = oliveGetAccountSlot();
    oliveQueryAccount(result.get(), slot);
    if ((*result)[0] == 0) {
        slot = oliveGetAccountSlot();
        oliveQueryAccountSlot(&mAccountSlot, slot);
    }
    mDownloadReset = 0;
}
/* 02039834 */
static void oliveAssureTermination() {}
/* 02039838 */
u32 OliveCommentMgr::countUnread() {
    u32 count = 0;
    for (u32 i = 0; i < SHOWN_BOTTLES; i++) {
        if (!mShown[i].mValid)
            continue;
        if (mShown[i].mRead != 0)
            continue;
        count++;
    }
    return count;
}
/* 020398B8 */
u32 OliveCommentMgr::notifyCount() {
    void* state = sMenuState;
    OliveMenuManager* menu = sMenuManager;
    bool busy = oliveMenuBusy(state) != 0 || oliveMenuEvent(state) != 0;
    if (busy && menu->mKind == 2)
        return 0;
    u32 count = countUnread();
    if (mNotification != 0 && mNotificationRead == 0)
        count++;
    return count;
}
/* 02039970 */
u32 OliveCommentMgr::hasNotify() {
    u32 count = notifyCount();
    return ((0u - count) & ~count) >> 31;
}
/* 0203999C */
u32 OliveCommentMgr::unreadCount() {
    u32 count = countUnread();
    if (mNotification != 0 && mNotificationRead == 0)
        count++;
    return count;
}
/* 020399DC */
u32 OliveCommentMgr::hasUnread() {
    u32 count = unreadCount();
    return ((0u - count) & ~count) >> 31;
}
/* 02039A08 */
u32 OliveCommentMgr::countShown() {
    u32 count = 0;
    for (u32 i = 0; i < SHOWN_BOTTLES; i++)
        if (mShown[i].mValid != 0)
            count++;
    return count;
}
bool OliveCommentMgr::downloadedReady(u32 index, u32 state) {
    OliveBottle* entry = &mDownloaded[index];
    s32 slot = entry->mTextureSlot;
    if (slot < 0)
        return false;
    if (texture(slot)->mState != state)
        return false;
    if (entry->mValid == 0)
        return false;
    return entry->mPostID[0] != 0;
}
static u32 oliveSaveMode() { return *oliveGetSaveMode(&sSave->mMode); }
/* 02039A58 */
u32 OliveCommentMgr::countReady() {
    if (oliveSaveMode() == 2)
        return 0;
    u32 count = 0;
    for (u32 i = 0; i < SHOWN_BOTTLES; i++)
        if (downloadedReady(i, TEXTURE_READY))
            count++;
    return count;
}
/* 02039B80 */
s32 OliveCommentMgr::downloadedEnd() {
    s32 last = -1;
    for (u32 i = 0; i < SHOWN_BOTTLES; i++) {
        auto* entry = &mDownloaded[i];
        if (entry->mTextureSlot < 0)
            continue;
        if (entry->mValid == 0)
            continue;
        if (entry->mPostID[0] == 0)
            continue;
        last = i;
    }
    s32 end = last + 1;
    if (end > SHOWN_BOTTLES)
        end = SHOWN_BOTTLES;
    return end;
}
/* 02039C34 */
u32 OliveCommentMgr::countSelected() {
    if (oliveSaveMode() == 2)
        return 0;
    u32 count = 0;
    for (u32 i = 0; i < SHOWN_BOTTLES; i++)
        if (downloadedReady(i, TEXTURE_SELECTED))
            count++;
    return count;
}
/* 02039D5C */
u32 OliveCommentMgr::promoteOne() {
    for (u32 i = 0; i < SHOWN_BOTTLES; i++) {
        if (!downloadedReady(i, TEXTURE_READY))
            continue;
        s32 slot = mDownloaded[i].mTextureSlot;
        texture(slot)->mState = TEXTURE_SELECTED;
        return 1;
    }
    return 0;
}
static bool usesTexture(OliveBottle* entry, u32 index) {
    if (index != (u32)(s32)entry->mTextureSlot)
        return false;
    if (entry->mValid == 0)
        return false;
    return entry->mPostID[0] != 0;
}
/* 02039E4C */
void OliveCommentMgr::syncTextures() {
    for (u32 i = 0; i < BOTTLE_TEXTURES; i++) {
        bool found = false;
        for (u32 k = 0; k < SHOWN_BOTTLES && !found; k++)
            if (usesTexture(&mDownloaded[k], i))
                found = true;
        if (found)
            continue;
        for (u32 k = 0; k < SHOWN_BOTTLES; k++)
            if (usesTexture(&mShown[k], i)) {
                found = true;
                break;
            }
        if (found)
            continue;
        if (mTextures[i].mState != TEXTURE_EMPTY)
            mTextures[i].mState = TEXTURE_RELEASED;
    }
    for (u32 i = 0; i < BOTTLE_TEXTURES; i++)
        if (mTextures[i].mState == TEXTURE_SELECTED)
            mTextures[i].mState = TEXTURE_READY;
    mDownloadReset = 0;
}
/* 0203A070 */
void OliveCommentMgr::resetDownloaded() {
    for (u32 i = 0; i < BOTTLE_TEXTURES; i++) {
        u32 state = mTextures[i].mState;
        if (state == TEXTURE_RELEASED || state == TEXTURE_SELECTED || state == TEXTURE_READY)
            mTextures[i].mState = TEXTURE_EMPTY;
    }
    clearBottles(mDownloaded);
    mDownloadCount = 0;
    mDownloadReset = 1;
}
/* 0203A24C */
void OliveCommentMgr::reset() {
    mRotationState = 0;
    mNotificationRead = 0;
    mFirstRotation = 1;
    mRotation = 0;
    mNotification = 0;
    clearBottles(mShown);
    for (u32 i = 0; i < BOTTLE_TEXTURES; i++)
        if (mTextures[i].mState == TEXTURE_SHOWN)
            mTextures[i].mState = TEXTURE_EMPTY;
    resetDownloaded();
}
/* 0203A3F0 */
u32 OliveCommentMgr::slotOk() {
    if (sDebugAccount != 0)
        return sDebugSlotOk != 0;
    u32 slot = mAccountSlot;
    return (slot == 0 || slot == 1) ? 1 : 0;
}
/* 0203A438 */
u32 OliveCommentMgr::slotFirst() {
    if (sDebugAccount != 0)
        return sDebugSlotFirst != 0;
    return mAccountSlot == 0;
}
/* 0203A46C */
static u32 oliveModeOk() {
    u32 mode = oliveSaveMode();
    return (mode == 0 || mode == 1) ? 1 : 0;
}
void OliveCommentMgr::shiftDownloaded() {
    for (u32 i = 1; i < SHOWN_BOTTLES; i++)
        mDownloaded[i - 1].copy(&mDownloaded[i]);
}
/* 0203A4B8 */
void OliveCommentMgr::rotate() {
    if (!oliveModeOk())
        return;
    if (!slotOk())
        return;
    if (mFirstRotation == 0) {
        u32 index = (u32)mRotation + 1;
        mRotation = index;
        if ((s32)index >= SHOWN_BOTTLES)
            mRotation = 0;
    } else {
        u32 index = mRotation;
        mFirstRotation = 0;
        if ((s32)index >= SHOWN_BOTTLES)
            mRotation = 0;
    }
    s32 count = countSelected();
    mDownloadCount = count;
    if (count <= 0)
        return;
    u32 index = mRotation;
    OliveBottle* entry = shown(index);
    s32 slot = entry->mTextureSlot;
    if (slot >= 0) {
        texture(slot)->mState = TEXTURE_EMPTY;
        index = mRotation;
    }
    shown(index)->clear();
    while (mDownloaded[0].mValid == 0 || mDownloaded[0].mPostID[0] == 0) {
        shiftDownloaded();
        mDownloaded[SHOWN_BOTTLES - 1].clear();
    }
    index = mRotation;
    shown(index)->copy(&mDownloaded[0]);
    index = mRotation;
    entry = shown(index);
    slot = entry->mTextureSlot;
    if (slot >= 0)
        texture(slot)->mState = TEXTURE_SHOWN;
    shiftDownloaded();
    mDownloadCount = (u32)mDownloadCount - 1;
    mDownloaded[SHOWN_BOTTLES - 1].clear();
}
/* 0203A910 */
f32 OliveCommentMgr::interval() {
    if ((s32)notifyCount() > 0)
        return sNotifyInterval;
    return sIdleInterval;
}
/* 0203A958 */
f32 OliveCommentMgr::rate() {
    f32 intervalValue = interval();
    f32 numerator = sRateScale;
    if (numerator != numerator)
        return gabi::ppc_qnan(numerator);
    if (intervalValue != intervalValue)
        return gabi::ppc_qnan(intervalValue);
    return (f32)((f64)numerator / intervalValue);
}
static bool isOliveStage(OliveSafeString* literal, OliveSafeString* stage, const be<u8>* name) {
    literal->__vtbl = 0x100055F0;
    literal->mpData = name;
    OliveGameInfo* game = oliveGetGameInfo();
    auto* vtable = gabi::at<OliveStringVtable>(literal->__vtbl);
    stage->__vtbl = 0x100055F0;
    stage->mpData = game->mStageName;
    gabi::call_ptr<u32>(vtable->mAssureTermination.get(), literal);
    literal->assureTermination();
    auto* stageVtable = gabi::at<OliveStringVtable>(stage->__vtbl);
    const be<u8>* first = literal->mpData;
    gabi::call_ptr<u32>(stageVtable->mAssureTermination.get(), stage);
    const be<u8>* second = stage->mpData;
    if (first == second)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u32 a = first[i], b = second[i];
        if (a != b)
            return false;
        if (a == 0)
            return true;
    }
    return false;
}
/* 0203A984 */
void OliveCommentMgr::topicTag(OliveWideString* out) {
    s32 topic = 0;
    OliveGameInfo* game = oliveGetGameInfo();
    OliveStage* stage = &game->mStage;
    bool viaStage = false;
    if (!stage->info())
        topic = 0;
    else {
        game = oliveGetGameInfo();
        stage = &game->mStage;
        OliveStageDescriptor* info = stage->info();
        u32 category = ((u8)info->mType >> 1) & 0x7F;
        if (category < 3) {
            if (category <= 1)
                viaStage = true;
            else
                topic = oliveCheckCollect(&sSave->mCollect, 3, 0) != 0 ? 50 : 58;
        } else if (category <= 9)
            topic = sCategoryTopics.mTopics[category];
        else if (category <= 13)
            viaStage = true;
        else if (category <= 15)
            topic = 49;
        else
            topic = 0;
    }
    if (viaStage) {
        topic = sMenuManager->mpStage->mTopic;
        if (topic == 43 || topic == 0) {
            gabi::Local<OliveSafeString> firstLiteral, secondLiteral, firstStage, secondStage;
            if (isOliveStage(firstLiteral.get(), firstStage.get(), sSeaStage.mText) ||
                isOliveStage(secondLiteral.get(), secondStage.get(), sHiddenStage.mText))
                topic = 58;
        }
    }
    // HD: one frame view keeps message scratch offsets and stack-fill bytes unchanged.
    gabi::Local<OliveMessageFrame> frame;
    OliveSafeString* name = &sStageNames.mNames[0];
    if (topic >= 0) {
        if (topic > 58)
            topic = 58;
        topic = (s8)topic;
        name = &sStageNames.mNames[topic];
    }
    oliveConstructMessage(&frame->mData);
    oliveSetMessage(&frame->mData, &sMessageGroup, name);
    oliveLookupMessage(sMessageLookup.get(), &frame->mData, &frame->mInfo, frame->mWork);
    *out->mpData = 0;
    OliveMessageText* texts = frame->mData.mpTexts;
    be<u16>* text = nullptr;
    u32 index = frame->mInfo.mTextIndex;
    if (index < texts->mCount)
        text = oliveGetMessageText(texts->mpEntries.get(), index);
    be<u16>* destination = out->mpData;
    gabi::Local<OliveSafeWideString> source;
    source->__vtbl = 0x10005650;
    source->mpData = text;
    oliveTerminateWideString(source.get());
    s32 length = wideLength(source->mpData.get());
    s32 capacity = out->mCapacity;
    auto* vtable = gabi::at<OliveStringVtable>(source->__vtbl);
    if (!(length < capacity))
        length = capacity - 1;
    gabi::call_ptr<u32>(vtable->mAssureTermination.get(), source.get());
    oliveCopyMemory(destination, source->mpData.get(), (u32)length * 2, 0);
    *gabi::at<be<u16>>(gabi::ea(destination) + (u32)length * 2) = 0;
}
static void sendOliveString(OliveWideString* text) {
    text->assureTermination();
    auto* vtable = gabi::at<OliveStringVtable>(text->__vtbl);
    be<u16>* buffer = text->mpData;
    gabi::call_ptr<u32>(vtable->mAssureTermination.get(), text);
    s32 length = wideLength(text->mpData.get());
    oliveSendMessage(sMessageManager.get(), buffer, (u32)length, 25, 0, 0xFFFFFFFF);
}
/* 0203AD94 */
void OliveCommentMgr::sendEntry(OliveBottle* entry) {
    sendOliveString(&entry->mName);
    sendOliveString(&entry->mTopic);
    sendOliveString(&entry->mBody);
}
/* 0203AF50 */
void OliveCommentMgr::publish() {
    oliveSendMessage(sMessageManager.get(), sPublishHeader.mText, 20, 25, 0, 0xFFFFFFFF);
    gabi::Local<be<u16>[2]> separator;
    (*separator)[0] = 0x2026;
    oliveSendMessage(sMessageManager.get(), separator.get(), 2, 25, 0, 0xFFFFFFFF);
    for (u32 i = 0; i < SHOWN_BOTTLES; i++)
        sendEntry(&mShown[i]);
    for (u32 i = 0; i < SHOWN_BOTTLES; i++)
        sendEntry(&mDownloaded[i]);
    gabi::Local<OliveSleepSpan> span;
    while (oliveMessagesAccepted(sMessageManager.get()) == 0) {
        span->mLow = 1;
        span->mHigh = 0;
        oliveSleep(span.get());
    }
    oliveLockMessageMutex(&sMessageManager->mMutex, &sMessageMutexName);
}
/* 0203B0B4 */
static void destroyOliveDisposer(OliveDisposer* disposer, u32 flags) {
    if (!disposer)
        return;
    disposer->__vtbl = 0x10005CD8;
    if (disposer == sDisposer.get()) {
        OliveCommentMgr* manager = sInstance;
        sDisposer = nullptr;
        manager->destroy(2);
        sInstance = nullptr;
    }
    oliveDestroyDisposer(disposer, 0);
    if (flags & 1)
        oliveDelete(disposer);
}
/* 0203B15C */
static void __sinit_olive_comment_manager() {
    sHeader.mWords[3] = 0;
    sHeader.mWords[2] = 0;
    sHeader.mWords[1] = 0;
    sHeader.mWords[0] = 0;
    oliveRegisterGlobal(sDestructorRecords.mRecords[0]);
    f32 minAngle = sInitConstants.mMinAngle, maxAngle = sInitConstants.mMaxAngle;
    sHeader.mMinAngle = minAngle;
    sHeader.mMaxAngle = maxAngle;
    oliveInitByte(&sHeader.mByte);
    oliveRegisterGlobal(sDestructorRecords.mRecords[1]);
    oliveInitFlag(&sHeader.mFlag);
    oliveRegisterGlobal(sDestructorRecords.mRecords[2]);
    f32 farDistance = sInitConstants.mFar, nearDistance = sInitConstants.mNear;
    sHeader.mFarMin = farDistance;
    sHeader.mNearMax = nearDistance;
    sHeader.mNearMin = farDistance;
    sHeader.mFarMax = nearDistance;
    for (u32 i = 0; i < STAGE_TOPICS; i++) {
        sStageNames.mNames[i].__vtbl = 0x100055F0;
        sStageNames.mNames[i].mpData = sStageLiterals.mNames[i];
    }
    sMessageGroup.__vtbl = 0x100055F0;
    sMessageGroup.mpData = sGroupName.mText;
}
/* 0203B5F8 */
static void destroyOliveTopicStorage(void* storage, u32 flags) {
    if (storage && (flags & 1))
        oliveDelete(storage);
}
/* 0203B60C */
static void destroyOliveBodyStorage(void* storage, u32 flags) {
    if (storage && (flags & 1))
        oliveDelete(storage);
}

// clang-format off
/* WWHD glue
data sInstance 1018F4AC
data sDisposer 1018F4B0
data sMessageConverter 101F4B5C
data sMessageLookup 101F4AE8
data sMessageManager 101F4BD8
data sMenuState 101F5088
data sMenuManager 101F8344
data sSave 101F84DC
data sDebugAccount 1018F4B4
data sDebugSlotOk 1018F4B8
data sDebugSlotFirst 1018F4BC
data sIdleInterval 1018F4C0
data sNotifyInterval 1018F4C4
data sRateScale 100057D0
data sHeader 10200A7C
data sInitConstants 10005804
data sDestructorRecords 1018F488
data sStageNames 10200AA8
data sStageLiterals 10005814
data sMessageGroup 10200A74
data sCategoryTopics 100057E1
data sMessageMutexName 1048D804
data sHeapName 100057B8
data sSeaStage 100057D4
data sHiddenStage 100057DC
data sPublishHeader 100057EC
data sGroupName 10005CB0
ext 0273AD10 OliveBottle* oliveAllocateBottle(u32)
ext 0273AD10 OliveCommentMgr* oliveAllocateManager(u32)
ext 0273AD10 void* oliveAllocateBytes(u32)
ext 0273AF40 void oliveDelete(void*)
ext 0203B620 void oliveDestroyWorker(void*,u32)
ext 025F8CDC void oliveConvertTopic(void*,OliveWideString*,be<u16>*)
ext 028EFFD0 void oliveConstructArray(OliveBottle*,u32,u32,u32)
ext 0273B0D4 OliveCommentMgr* oliveAllocateManagerInHeap(u32,void*,u32)
ext 02752B0C void oliveConstructDisposer(OliveDisposer*,void*,u32)
ext 0203E8C4 void* oliveGetParentHeap()
ext 02753004 OliveHeap* oliveCreateHeap(u32,OliveSafeString*,void*,u32,u32)
ext 0273B0D4 void* oliveAllocateInHeap(u32,OliveHeap*,u32)
ext C0004660 u32 oliveGetAccountSlot()
ext 0286D938 void oliveQueryAccount(void*,u32)
ext 0286D9E0 void oliveQueryAccountSlot(be<u8>*,u32)
ext 02618498 u32 oliveMenuBusy(void*)
ext 02617AE4 u32 oliveMenuEvent(void*)
ext 027200F4 be<u8>* oliveGetSaveMode(void*)
ext 025200D4 OliveGameInfo* oliveGetGameInfo()
ext 025B7A2C u32 oliveCheckCollect(void*,u32,u32)
ext 025F65F4 void oliveConstructMessage(OliveMessageData*)
ext 025F6D70 void oliveSetMessage(OliveMessageData*,OliveSafeString*,OliveSafeString*)
ext 025F50E8 void oliveLookupMessage(void*,OliveMessageData*,void*,void*)
ext 0273A2E8 be<u16>* oliveGetMessageText(void*,u32)
ext 0203B6B0 void oliveTerminateWideString(OliveSafeWideString*)
ext 025F9B1C void oliveSendMessage(OliveMessageManager*,void*,u32,u32,u32,u32)
ext 025F9B68 u32 oliveMessagesAccepted(OliveMessageManager*)
ext 02760C3C void oliveSleep(OliveSleepSpan*)
ext 020063C0 void oliveLockMessageMutex(void*,void*)
ext 02752BEC void oliveDestroyDisposer(OliveDisposer*,u32)
ext 028F026C void oliveRegisterGlobal(void*)
ext 028ED6F8 void oliveInitByte(void*)
ext 028EAB2C void oliveInitFlag(void*)
ext C0009988 void oliveCopyMemory(void*,void*,u32,u32)
ext C0009990 void oliveClearMemory(void*,u32,u32)
*/

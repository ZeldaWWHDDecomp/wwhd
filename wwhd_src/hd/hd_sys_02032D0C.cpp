// HD: the Wii U error-viewer task owns FS events, an error queue and nn::erreula UI state.
#include "hd/hd_sys_02032D0C.h"

static u32 firstErrorFSEvent(ErrorViewerTask*, u32, u32);
static u32 secondErrorFSEvent(ErrorViewerTask*, u32, u32);
static ErrorViewerTask* sCurrentViewer;
static ErrorSettings* sSettings;
static ErrorCPULists* sCPULists;
static void* sInputManager;
static ErrorMenuState* sMenu;
static ErrorFileSystem* sFileSystem;
static ErrorLiteral sEmptyName;
static ErrorLiteral sTaskName;
static ErrorLiteral sQueueReport;
static ErrorLiteral sHeapName;
static ErrorLiteral sEventName;
static ErrorLiteral sShowReport;
static ErrorLiteral sCloseReport;
static ErrorLanguageTable sLanguageTable;
static ErrorHeader sHeader;
static ErrorInitConstants sInitConstants;
static ErrorDestructorRecords sDestructorRecords;

/* 02032D0C */
static void destroyErrorRegistration(ErrorRegistration* registration, s32 flags) {
    if (!registration)
        return;
    if (registration->mRegistered != 0)
        sCurrentViewer = nullptr;
    if (flags & 1)
        errorDelete(registration);
}
/* 02032D38 */
static void destroyErrorViewer(ErrorViewerTask* task, s32 flags) {
    if (!task)
        return;
    task->__vtbl = 0x10004C80;
    errorDestroyMutex(&task->mMutex, 2);
    errorDestroyEvent(&task->mSecondEvent, 2);
    errorDestroyEvent(&task->mFirstEvent, 2);
    destroyErrorRegistration(&task->mRegistration, 2);
    errorDestroyTask(task, 0);
    if (flags & 1)
        errorDelete(task);
}
/* 02032DC8 */
static void destroyErrorViewerFromBase(void* base, s32 flags) {
    destroyErrorViewer(gabi::at<ErrorViewerTask>(gabi::ea(base) - 0x14), flags);
}
/* 02032DD0 */
void ErrorViewerTask::registerViewer() {
    if (sCurrentViewer != nullptr)
        return;
    sCurrentViewer = this;
    mRegistration.mRegistered = 1;
}
static void constructErrorString(ErrorSafeString* text) {
    if (!text) {
        text = errorAllocateString(sizeof(ErrorSafeString));
        if (!text)
            return;
    }
    text->mpData = sEmptyName.mText;
    text->__vtbl = 0x10004A7C;
}
static void constructErrorDelegate(ErrorDelegate* delegate) {
    if (!delegate)
        return;
    delegate->mFunction = 0;
    delegate->__vtbl = 0x10004A64;
    delegate->mAdjustment = 0;
    delegate->mIndex = 0;
    delegate->mpObject = nullptr;
    delegate->__vtbl = 0x10004AC4;
}
static ErrorFSEvent* wrappedEvent(ErrorViewerTask* task, u32 offset) {
    return gabi::at<ErrorFSEvent>(gabi::ea(task) + offset);
}
/* 02032DF0 */
static ErrorViewerTask* constructErrorViewer(ErrorViewerTask* task, void* parameter) {
    if (!task) {
        task = errorAllocateTask(sizeof(ErrorViewerTask));
        if (!task)
            return nullptr;
    }
    errorConstructTask(task, parameter, sTaskName.mText);
    task->mTaskVtable = 0x10004BC8;
    task->__vtbl = 0x10004C80;
    task->mRegistration.mRegistered = 0;
    task->mpHeap = nullptr;
    // HD: constructor tests intentionally cover members wrapping through guest address zero.
    ErrorFSEvent* first = wrappedEvent(task, offsetof(ErrorViewerTask, mFirstEvent));
    ErrorFSEvent* second = wrappedEvent(task, offsetof(ErrorViewerTask, mSecondEvent));
    errorConstructEvent(first);
    constructErrorString(
        gabi::at<ErrorSafeString>(gabi::ea(first) + offsetof(ErrorFSEvent, mName)));
    first->mType = 0;
    first->__vtbl = 0x10169E3C;
    first->mArgument = 0;
    constructErrorDelegate(
        gabi::at<ErrorDelegate>(gabi::ea(first) + offsetof(ErrorFSEvent, mDelegate)));
    errorConstructEvent(second);
    constructErrorString(
        gabi::at<ErrorSafeString>(gabi::ea(second) + offsetof(ErrorFSEvent, mName)));
    second->__vtbl = 0x10169E3C;
    second->mType = 0;
    second->mArgument = 0;
    constructErrorDelegate(
        gabi::at<ErrorDelegate>(gabi::ea(second) + offsetof(ErrorFSEvent, mDelegate)));
    errorConstructMutex(&task->mMutex);
    ErrorQueue* queue = gabi::at<ErrorQueue>(gabi::ea(task) + offsetof(ErrorViewerTask, mQueue));
    if (!queue)
        queue = errorAllocateQueue(sizeof(ErrorQueue));
    if (queue) {
        queue->mCount = 0;
        queue->mHead = 0;
        queue->mpEntries = queue->mStorage;
        queue->mCapacity = ERROR_QUEUE_CAPACITY;
    }
    task->mState = ERROR_IDLE;
    errorZero(&task->mCurrent, sizeof(ErrorEntry));
    task->mDiscFrames = 0;
    task->mpWork = nullptr;
    task->mSavedDim = 1;
    task->mStorageFrames = 0;
    task->mRecoverFrames = 0;
    errorZero(&task->mClient, sizeof(ErrorFSClient));
    task->mVolumeState = 1;
    task->mpWatchedVolume = nullptr;
    return task;
}
/* 02032FE8 */
u32 ErrorViewerTask::isShowing() {
    u32 state = mState;
    return (state != ERROR_IDLE && state != ERROR_RECOVER) ? 1 : 0;
}
/* 0203300C */
void ErrorViewerTask::push(u32 code, u32 flag) {
    errorReportQueue(sQueueReport.mText, code, flag);
    errorLock(&mMutex);
    s32 count = mQueue.mCount;
    if (count < (s32)mQueue.mCapacity) {
        s32 index = (s32)mQueue.mHead + count;
        s32 capacity = mQueue.mCapacity;
        ErrorEntry* buffer = mQueue.mpEntries;
        mQueue.mCount = count + 1;
        if (!(index < capacity))
            index -= capacity;
        ErrorEntry* entry =
            gabi::at<ErrorEntry>(gabi::ea(buffer) + (u32)index * sizeof(ErrorEntry));
        entry->mCode = code;
        // HD: queue insertion writes only the first byte of the flag word.
        entry->mFlag = (u8)flag;
    }
    errorUnlock(&mMutex);
}
/* 020330BC */
void ErrorViewerTask::appear() {
    gabi::Local<ErrorAppearArgs> args;
    args->mType = 2;
    errorAppear(args.get());
}
/* 020330E8 */
void ErrorViewerTask::disappear() {
    u32 handle = errorGetHandle(this);
    if (handle)
        errorDisappear(handle);
}
/* 02033114 */
u32 ErrorViewerTask::workSize() { return errorGetWorkSize(this); }
/* 02033118 */
static u32 errorViewerRegion() {
    u32 region = sSettings->mRegion;
    if (region == 1)
        return 0;
    if (region == 2)
        return 1;
    if (region == 4)
        return 2;
    return 1;
}
/* 0203315C */
static u32 errorViewerLanguage() {
    u32 language = sSettings->mLanguage;
    if (language <= 5)
        return sLanguageTable.mLanguages[language];
    if (language < 8)
        return 1;
    if (language <= 10)
        return sLanguageTable.mLanguages[language];
    return 1;
}
/* 020331A8 */
void ErrorViewerTask::createViewer() {
    void* parent = errorGetParentHeap();
    u32 size = workSize();
    gabi::Local<ErrorNamedString> name;
    name->mString.__vtbl = 0x10004A7C;
    name->mString.mpData = sHeapName.mText;
    ErrorHeap* heap = errorCreateHeap(size + 0x200, &name->mString, parent, 1, 1);
    mpHeap = heap;
    u32 workBytes = workSize();
    void* work = errorAllocateInHeap(workBytes, mpHeap.get(), 4);
    mpWork = work;
    u32 clearBytes = workSize();
    errorClear(work, 0, clearBytes);
    gabi::Local<ErrorCreateArgs> args;
    args->mLanguage = 0;
    args->mpClient = &mClient;
    args->mRegion = 0;
    args->mpWork = mpWork.get();
    args->mRegion = errorViewerRegion();
    args->mLanguage = errorViewerLanguage();
    errorCreate(args.get());
}
/* 02033284 */
void ErrorViewerTask::prepare() {
    ErrorDelegate* first =
        gabi::at<ErrorDelegate>(gabi::ea(this) + offsetof(ErrorViewerTask, mFirstEvent) +
                                offsetof(ErrorFSEvent, mDelegate));
    if (first) {
        first->__vtbl = 0x10004ADC;
        first->mpObject = this;
        first->mIndex = -1;
        first->mFunction = gabi::guest_fn((const void*)firstErrorFSEvent);
        first->mAdjustment = 0;
    }
    mFirstEvent.mName.mpData = sEventName.mText;
    mFirstEvent.mType = 2;
    {
        ErrorCPULists* lists = sCPULists;
        u32 count = lists->mCount;
        gptr<void>* array = lists->mpLists;
        if (count > 12)
            array += 12;
        errorAttachEvent(*array, 0, &mFirstEvent);
    }
    ErrorDelegate* second =
        gabi::at<ErrorDelegate>(gabi::ea(this) + offsetof(ErrorViewerTask, mSecondEvent) +
                                offsetof(ErrorFSEvent, mDelegate));
    if (second) {
        second->__vtbl = 0x10004ADC;
        second->mpObject = this;
        second->mIndex = -1;
        second->mFunction = gabi::guest_fn((const void*)secondErrorFSEvent);
        second->mAdjustment = 0;
    }
    mSecondEvent.mName.mpData = sEventName.mText;
    mSecondEvent.mType = 2;
    {
        ErrorCPULists* lists = sCPULists;
        u32 count = lists->mCount;
        gptr<void>* array = lists->mpLists;
        if (count > 13)
            array += 13;
        errorAttachEvent(*array, 0, &mSecondEvent);
    }
    errorAddClient(&mClient, 0);
    createViewer();
}
/* 020333B4 */
void ErrorViewerTask::feedInput() {
    gabi::Local<ErrorInputArgs> args;
    args->mpGamePad = nullptr;
    for (u32 i = 0; i < 4; i++)
        args->mPads[i] = nullptr;
    ErrorInputPads* input = errorGetPads(sInputManager.get());
    for (u32 i = 0; i < 4; i++) {
        auto* pad = &input->mPads[i];
        if ((s32)pad->mCount > 0 || pad->mStatus == 0xFFFFFFFF)
            args->mPads[i] = pad;
    }
    ErrorGamePad* gamePad = errorGetGamePad(sInputManager.get());
    if ((s32)gamePad->mCount > 0 || gamePad->mResult == 0xFFFFFFFF) {
        for (u32 i = 0; i < 0x2B; i++)
            args->mStatus.mWords[i] = (u32)gamePad->mStatus.mWords[i];
        // HD: VPAD touch coordinates are an embedded packed field, 0x52 bytes into its status.
        void* touch = args->mStatus.mTouch;
        errorCalibrateTouch(0, touch, touch);
        args->mpGamePad = &args->mStatus;
    }
    errorSetInput(args.get());
}
/* 020334C8 */
u32 ErrorViewerTask::volumeError(void* client) {
    if (errorVolumeState(client) == 1)
        return 0;
    return errorLastVolumeError(client);
}
/* 0203351C */
void ErrorViewerTask::enter() {
    mStorageFrames = 0;
    mDiscFrames = 0;
    errorPauseSound();
    ErrorGameInfo* game = errorGetGameInfo();
    errorStopVibration(&game->mVibration);
    void* dim = errorGetDimming();
    mSavedDim = (u8)errorGetDim(dim);
    void* nextDim = errorGetDimming();
    errorSetDim(nextDim, 1);
}
/* 02033578 */
void ErrorViewerTask::show(u32 code) {
    errorReportShow(sShowReport.mText, code);
    gabi::Local<ErrorShowArgs> args;
    args->mCode = code;
    args->mType = 0;
    args->mReserved24 = 0;
    args->mFlag = 1;
    args->mReserved18 = 0;
    args->mReserved08 = 0;
    args->mController = 2;
    args->mReserved20 = 0;
    args->mRegion = 2;
    args->mReserved1C = 0;
    args->mReserved0C = 0;
    errorShow(args.get());
}
/* 020335F0 */
void ErrorViewerTask::setState(u32 state) { mState = state; }
void ErrorViewerTask::popEntry() {
    s32 head = mQueue.mHead, capacity = mQueue.mCapacity;
    ErrorEntry* buffer = mQueue.mpEntries;
    s32 index = head < capacity ? head : head - capacity;
    ErrorEntry* entry = gabi::at<ErrorEntry>(gabi::ea(buffer) + (u32)index * sizeof(ErrorEntry));
    s32 nextHead = (s32)mQueue.mHead + 1;
    mCurrent.mCode = (u32)entry->mCode;
    mQueue.mHead = nextHead;
    mCurrent.mFlags = (u32)entry->mFlags;
    s32 count = mQueue.mCount;
    if (!(nextHead < (s32)mQueue.mCapacity))
        mQueue.mHead = 0;
    mQueue.mCount = count - 1;
}
/* 020335F8 */
void ErrorViewerTask::stateIdle() {
    bool resetDisc = false;
    if (errorMenuDisc(sMenu.get()) == 0 && errorMenuDiscCheck(sMenu.get()) == 0)
        resetDisc = true;
    else if (errorDiscResolved(sMenu.get()) != 0)
        resetDisc = true;
    bool checkStorage;
    if (resetDisc) {
        mDiscFrames = 0;
        checkStorage = errorMenuStorage(sMenu.get()) != 0;
    } else {
        u32 count = (u32)mDiscFrames + 1;
        mDiscFrames = count;
        if (count >= ERROR_DISC_DELAY)
            push(ERROR_DISC, 0);
        checkStorage = errorMenuStorage(sMenu.get()) != 0;
    }
    bool resetStorage = true;
    if (checkStorage && errorStorageResolved(sMenu.get()) == 0) {
        resetStorage = false;
        u32 count = (u32)mStorageFrames + 1;
        mStorageFrames = count;
        if (count >= ERROR_STORAGE_DELAY)
            push(ERROR_STORAGE, 0);
    }
    if (resetStorage)
        mStorageFrames = 0;
    void* volume = &sFileSystem->mVolume;
    u32 code = volumeError(volume);
    if (code) {
        mpWatchedVolume = volume;
        mVolumeState = errorVolumeState(volume);
        push(code, 1);
    }
    errorLock(&mMutex);
    if ((s32)mQueue.mCount > 0)
        popEntry();
    errorUnlock(&mMutex);
    if (mCurrent.mCode != 0) {
        enter();
        show(mCurrent.mCode);
        setState(ERROR_SHOWN);
    }
}
/* 02033804 */
void ErrorViewerTask::stateShown() {
    if (errorViewerStatus(this) == 2)
        setState(ERROR_CHECK);
}
/* 02033844 */
u32 ErrorViewerTask::discStill(ErrorEntry* entry) {
    if (entry->mCode != ERROR_DISC)
        return 1;
    return errorDiscResolved(sMenu.get()) != 0 ? 0 : 1;
}
/* 0203389C */
u32 ErrorViewerTask::storageStill(ErrorEntry* entry) {
    if (entry->mCode != ERROR_STORAGE)
        return 1;
    ErrorMenuState* menu = sMenu;
    if (menu->mStorageReady != 0)
        return 0;
    return errorStorageResolved(menu) != 0 ? 0 : 1;
}
/* 02033900 */
u32 ErrorViewerTask::volumeStill(ErrorEntry* entry) {
    (void)entry;
    u32 state = errorVolumeState(mpWatchedVolume.get());
    if (state == mVolumeState)
        return 1;
    mpWatchedVolume = nullptr;
    return 0;
}
/* 02033954 */
u32 ErrorViewerTask::otherStill(ErrorEntry* entry) {
    return errorOtherResolved(this, entry) == 0 ? 1 : 0;
}
/* 02033990 */
void ErrorViewerTask::close() {
    u32 reportResult = errorReportClose(sCloseReport.mText);
    errorClose(reportResult);
}
/* 020339C0 */
void ErrorViewerTask::stateCheck() {
    u32 code = mCurrent.mCode;
    gabi::Local<ErrorEntry> disc, storage, volume, other;
    u32 still;
    if (code - 0x192D50u < 0x270Fu) {
        disc->mCode = code;
        disc->mFlags = (u32)mCurrent.mFlags;
        still = discStill(disc.get());
    } else if (code == ERROR_STORAGE) {
        storage->mCode = code;
        storage->mFlags = (u32)mCurrent.mFlags;
        still = storageStill(storage.get());
    } else if (mpWatchedVolume != nullptr) {
        volume->mCode = code;
        volume->mFlags = (u32)mCurrent.mFlags;
        still = volumeStill(volume.get());
    } else {
        other->mCode = code;
        other->mFlags = (u32)mCurrent.mFlags;
        still = otherStill(other.get());
    }
    if (still)
        return;
    close();
    setState(ERROR_CLOSING);
}
/* 02033AAC */
void ErrorViewerTask::stateClosing() {
    if (errorViewerStatus(this) == 0)
        setState(ERROR_RESULT);
}
/* 02033AEC */
u32 ErrorViewerTask::launch(s32 page) {
    gabi::Local<ErrorLaunchArgs> account, settings;
    if (page == 0x14 || page == 0x15) {
        errorClear(account.get(), 0, sizeof(ErrorLaunchArgs));
        account->mPage = errorAccountSlot();
        errorLaunchAccount(account.get());
        return 0;
    }
    errorClear(settings.get(), 0, sizeof(ErrorLaunchArgs));
    if ((u32)page == 0xB)
        settings->mPage = 1;
    else if ((u32)page == 0xC)
        settings->mPage = 2;
    else if ((u32)page == 0x10)
        settings->mPage = 6;
    errorLaunchSettings(settings.get());
    return 0;
}
/* 02033BAC */
void ErrorViewerTask::restoreDim() {
    void* dim = errorGetDimming();
    errorSetDim(dim, (u8)mSavedDim);
}
/* 02033BE0 */
void ErrorViewerTask::stateResult() {
    if (mCurrent.mFlag != 0) {
        u32 result = errorResultCode(this);
        bool next;
        if (result == ERROR_SETTINGS)
            next = launch(0xC) != 0;
        else {
            s32 type = errorResultType(result);
            if (type == 2) {
                ErrorViewerTask* task = errorSetStateResult(this, ERROR_SHOWN);
                u32 code = errorResultCode(task);
                show(code);
                return;
            }
            if (type == 3) {
                u32 page = errorResultCode(gabi::at<ErrorViewerTask>((u32)type));
                next = launch(page) != 0;
            } else
                next = true;
        }
        if (!next) {
            setState(ERROR_LAUNCHED);
            return;
        }
    }
    errorLock(&mMutex);
    s32 count = mQueue.mCount;
    if (count == 0) {
        mCurrent.mFlag = 0;
        mCurrent.mCode = 0;
    } else if (count > 0)
        popEntry();
    errorUnlock(&mMutex);
    // HD: these call chains forward the callee's r3 result, rather than assuming it equals this.
    if (mCurrent.mCode == 0) {
        ErrorViewerTask* task = errorSetStateResult(this, ERROR_RECOVER);
        task->restoreDim();
        return;
    }
    ErrorViewerTask* task = errorSetStateResult(this, ERROR_SHOWN);
    task->show(mCurrent.mCode);
}
/* 02033DA4 */
void ErrorViewerTask::stateRecover() {
    u32 count = (u32)mRecoverFrames + 1;
    mRecoverFrames = count;
    if (count < ERROR_RECOVER_DELAY)
        return;
    errorResumeSound();
    setState(ERROR_IDLE);
    mRecoverFrames = 0;
}
/* 02033DF4 */
void ErrorViewerTask::calc() {
    feedInput();
    switch ((s32)mState) {
    case ERROR_IDLE:
        stateIdle();
        break;
    case ERROR_SHOWN:
        stateShown();
        break;
    case ERROR_CHECK:
        stateCheck();
        break;
    case ERROR_CLOSING:
        stateClosing();
        break;
    case ERROR_RESULT:
        stateResult();
        break;
    case ERROR_RECOVER:
        stateRecover();
        break;
    }
}
/* 02033EE8 */
void ErrorViewerTask::exit() {
    errorDestroyUI(this);
    void* work = mpWork;
    if (work) {
        errorFreeWork(work);
        mpWork = nullptr;
    }
    ErrorHeap* heap = mpHeap;
    if (heap) {
        heap->destroy();
        mpHeap = nullptr;
    }
}
/* 02033F54 */
static u32 exitErrorViewerThunk(ErrorViewerTask* task) { return errorExitResult(task); }
/* 02033F58 */
static u32 firstErrorFSEvent(ErrorViewerTask* task, u32 first, u32 second) {
    return errorFirstFSEvent(task, first, second);
}
/* 02033F5C */
static u32 secondErrorFSEvent(ErrorViewerTask* task, u32 first, u32 second) {
    return errorSecondFSEvent(task, first, second);
}
/* 02033F60 */
static void __sinit_error_viewer() {
    sHeader.mWords[2] = 0;
    sHeader.mWords[0] = 0;
    sHeader.mWords[3] = 0;
    sHeader.mWords[1] = 0;
    errorRegisterGlobal(sDestructorRecords.mRecords[0]);
    f32 min = sInitConstants.mMinAngle, max = sInitConstants.mMaxAngle;
    sHeader.mMinAngle = min;
    sHeader.mMaxAngle = max;
    errorInitByte(&sHeader.mByte);
    errorRegisterGlobal(sDestructorRecords.mRecords[1]);
    errorInitFlag(&sHeader.mFlag);
    errorRegisterGlobal(sDestructorRecords.mRecords[2]);
}
/* 02033FF4 */
static void destroyErrorCompanion(void* object, s32 flags) {
    if (object && (flags & 1))
        errorDelete(object);
}
/* 02034008 */
static void errorCompanionEmpty() {}

// clang-format off
/* WWHD glue
data sCurrentViewer 1018F2BC
data sSettings 101F4BAC
data sCPULists 101F95D0
data sInputManager 101F8AE8
data sMenu 101F5088
data sFileSystem 101F8B08
data sEmptyName 10004AF4
data sTaskName 10004AF8
data sQueueReport 10004B08
data sHeapName 10004B40
data sEventName 10004B50
data sShowReport 10004B70
data sCloseReport 10004B94
data sLanguageTable 10004B34
data sHeader 10200814
data sInitConstants 10004BC0
data sDestructorRecords 1018F298
ext 0273AF40 void errorDelete(void*)
ext 02760168 void errorDestroyMutex(ErrorMutex*,u32)
ext 027C0804 void errorDestroyEvent(ErrorFSEvent*,u32)
ext 027452C8 void errorDestroyTask(ErrorViewerTask*,u32)
ext 0273AD10 ErrorSafeString* errorAllocateString(u32)
ext 0273AD10 ErrorViewerTask* errorAllocateTask(u32)
ext 027454E0 void errorConstructTask(ErrorViewerTask*,void*,const be<u8>*)
ext 02752A84 void errorConstructEvent(ErrorFSEvent*)
ext 02760084 void errorConstructMutex(ErrorMutex*)
ext 0273AD10 ErrorQueue* errorAllocateQueue(u32)
ext 028F521C void errorZero(void*,u32)
ext C0009EE8 void errorReportQueue(const be<u8>*,u32,u32)
ext 027601BC void errorLock(ErrorMutex*)
ext 027601F0 void errorUnlock(ErrorMutex*)
ext 0286CB7C void errorAppear(ErrorAppearArgs*)
ext 0286CA40 u32 errorGetHandle(ErrorViewerTask*)
ext 0286CBB4 void errorDisappear(u32)
ext 0286CBC4 u32 errorGetWorkSize(ErrorViewerTask*)
ext 0203E918 void* errorGetParentHeap()
ext 02753004 ErrorHeap* errorCreateHeap(u32,ErrorSafeString*,void*,u32,u32)
ext 0273B0D4 void* errorAllocateInHeap(u32,ErrorHeap*,u32)
ext C0009990 void errorClear(void*,u32,u32)
ext 0286C5F8 void errorCreate(ErrorCreateArgs*)
ext 027C23F8 void errorAttachEvent(void*,u32,ErrorFSEvent*)
ext C0008C78 void errorAddClient(ErrorFSClient*,u32)
ext 02034130 ErrorInputPads* errorGetPads(void*)
ext 02034200 ErrorGamePad* errorGetGamePad(void*)
ext C00075D0 void errorCalibrateTouch(u32,void*,void*)
ext 0286CA50 void errorSetInput(ErrorInputArgs*)
ext C0008E30 s32 errorVolumeState(void*)
ext C0008DB8 u32 errorLastVolumeError(void*)
ext 02031138 void errorPauseSound()
ext 025200D4 ErrorGameInfo* errorGetGameInfo()
ext 025CB694 void errorStopVibration(void*)
ext 02032C6C void* errorGetDimming()
ext 02032BD0 u32 errorGetDim(void*)
ext 02032AD8 void errorSetDim(void*,u32)
ext C0009EE8 void errorReportShow(const be<u8>*,u32)
ext 0286CB2C void errorShow(ErrorShowArgs*)
ext 02618498 u32 errorMenuDisc(ErrorMenuState*)
ext 026184AC u32 errorMenuDiscCheck(ErrorMenuState*)
ext 02617F14 u32 errorDiscResolved(ErrorMenuState*)
ext 02617AE4 u32 errorMenuStorage(ErrorMenuState*)
ext 02617FCC u32 errorStorageResolved(ErrorMenuState*)
ext 0286CA30 s32 errorViewerStatus(ErrorViewerTask*)
ext 0286CB4C u32 errorOtherResolved(ErrorViewerTask*,ErrorEntry*)
ext C0009EE8 u32 errorReportClose(const be<u8>*)
ext 0286CB3C void errorClose(u32)
ext C0004660 u32 errorAccountSlot()
ext C000AA28 void errorLaunchAccount(ErrorLaunchArgs*)
ext C000AA40 void errorLaunchSettings(ErrorLaunchArgs*)
ext 0286CB6C u32 errorResultCode(ErrorViewerTask*)
ext 0286CB5C s32 errorResultType(u32)
ext 020335F0 ErrorViewerTask* errorSetStateResult(ErrorViewerTask*,u32)
ext 0203116C void errorResumeSound()
ext 0286C9CC void errorDestroyUI(ErrorViewerTask*)
ext 0273AFC8 void errorFreeWork(void*)
ext 02033EE8 u32 errorExitResult(ErrorViewerTask*)
ext 0286CAA4 u32 errorFirstFSEvent(ErrorViewerTask*,u32,u32)
ext 0286CAE8 u32 errorSecondFSEvent(ErrorViewerTask*,u32,u32)
ext 028F026C void errorRegisterGlobal(void*)
ext 028ED6F8 void errorInitByte(void*)
ext 028EAB2C void errorInitFlag(void*)
*/

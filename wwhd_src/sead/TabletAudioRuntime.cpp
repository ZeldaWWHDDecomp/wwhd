// sead tablet (DRC) audio: the software surround mixer's worker thread, the mixer object, its SDK surround
// settings and the core-hopping threads that apply them. Reconstructed from this game's instructions.
#include "bindings.h"
#include "sead/ElementAt.h"
#include "sead/TabletAudioRuntime.h"
#include "sead/PoolHeap.h"
#include "sead/RuntimeType.h"
using sead::tablet_audio::Worker;
static sead::tablet_audio::WorkerText sAudioWorkerText;
static sead::ThreadVtable sAudioWorkerVtable;
static sead::DisposerVtable sAudioWorkerNameOwnerVtable;
static sead::SafeTextVtable sAudioWorkerTextVtable;
static u32 sAudioWorkerName;

/* 0276590C */
Worker* Worker::construct(Worker* object, s32 priority, sead::RegistryHeap* heap, u32 stackSize, s32 messageCount) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (object == NULL) object = static_cast<Worker*>(allocateTabletAudioObject(sizeof(Worker)));
    if (object != NULL) {
        Thread::construct(object, &sAudioWorkerText, heap, priority, 0, 0x7FFFFFFF, stackSize, messageCount);
        object->mVtable = reinterpret_cast<sead::DisposerVtable*>(&sAudioWorkerVtable);
    }
    return object;
}
/* 02765994 */
void Worker::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    u32 state = mState;
    mVtable = reinterpret_cast<sead::DisposerVtable*>(&sAudioWorkerVtable);
    mNameOwnerVtable = &sAudioWorkerNameOwnerVtable;
    if (state != Exited && state != Joined) quitAndJoin(0);
    Thread::destroy(0);
    if (flags & 1) operator_delete(this);
}
/* 02765A20 */
void Worker::startOnCurrentCore() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> mask(8);
    gabi::FrameLocal<be<u32>> core(12);
    gabi::FrameLocal<be<u32>> savedCore(16);
    u32 current = tabletAudioCoreId();
    *savedCore = sCoreReverse[current];
    *core = *savedCore;
    u32 shift = *core;
    *mask = (shift & 0x20) ? 0 : 1u << (shift & 31);
    setCoreMask(mask.get());
    start();
}
/* 02765A88 */
u32 Worker::sendReference(Reference* reference) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 state = mState;
    if (state == Quitting || state == Exited) return 0;
    // the virtual message send (Thread::sendMessage unless overridden), non-blocking
    u32 sent = gabi::call_ptr<u32>(methods()->mReservedBeforeTick[1].mFunction, this, reference, 1u);
    if (sent != 0) {
        // atomic increment (lwarx/stwcx. loop)
        u32 address = gabi::ea(&reference->mReferences);
        u32 value;
        do { value = gabi::load_reserved(address); }
        while (!gabi::store_conditional(address, value + 1));
    }
    return sent;
}
/* 02765B10 */
void Worker::releaseReference(Reference* reference) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (reference == NULL) return;
    u32 state = mState;
    u32 release = reference->mTable->mRelease;
    gabi::call_ptr<void>(release, reference, (u32)(state == Quitting));
    // atomic decrement (lwarx/stwcx. loop)
    u32 address = gabi::ea(&reference->mReferences);
    u32 value;
    do { value = gabi::load_reserved(address); }
    while (!gabi::store_conditional(address, value - 1));
}
/* 02765B78 */
static void initializeTabletAudioWorkerText() {
    gabi::AutoFrame frame{gabi::current_function()};
    sAudioWorkerText.mVtable = &sAudioWorkerTextVtable;
    sAudioWorkerText.mText = reinterpret_cast<be<u8>*>(&sAudioWorkerName);
}
/* 02765B9C */
void sead::tablet_audio::WorkerText::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) != 0 && (flags & 1)) operator_delete(this);
}
/* 02765BB0 */
void sead::tablet_audio::WorkerText::reserved() {
    gabi::AutoFrame frame{gabi::current_function()};
}
/* 02765BB4 */
void Worker::dispatchRun() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::tail_ptr<void>(methods()->mRun.mFunction, this);
}
/* 02765BC4 */
u32 sead::tablet_audio::Runtime::headphones() {
    gabi::AutoFrame frame{gabi::current_function()};
    return tabletAudioHeadphoneStatus(0) == 1;
}
/* 02765BF4 */
void sead::tablet_audio::Runtime::setOutputMode(u32 enabled) {
    gabi::AutoFrame frame{gabi::current_function()};
    tabletAudioOutputMode(enabled);
}
/* 02765BFC */
void sead::tablet_audio::Runtime::updateHeadphones() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 enabled = headphones();
    if (enabled != (u8)mHeadphones) setOutputMode(enabled);
    mHeadphones = enabled;
}
/* 02766790 */
void sead::tablet_audio::Runtime::resetHeadphones() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 enabled = headphones();
    setOutputMode(enabled);
    mHeadphones = enabled;
}
struct AudioConversionScratch { be<u32> mUpper; be<u32> mLower; };
static sead::tablet_audio::Runtime* sTabletAudioRuntime;
static u32 sTabletAudioBaseTable;
static u32 sTabletAudioReferenceBaseTable;
static u32 sTabletAudioTable;
static u32 sTabletAudioReferenceTable;

/* 02765C50 */
void sead::tablet_audio::Runtime::limitSample(be<s32>* sample) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<AudioConversionScratch> scratch(8);
    s32 input = *sample;
    scratch->mUpper = 0x43300000;
    scratch->mLower = (u32)input ^ 0x80000000;
    f32 threshold = mThreshold;
    f32 value = gabi::fmuls_ppc((f32)input, mInputGain);
    f32 scaled;
    bool clampLow;
    if (value > -threshold) {
        if (!(value < threshold)) value = gabi::fmadds(gabi::fsubs_ppc(value, threshold), mSlope, threshold);
        scaled = gabi::fmuls_ppc(value, mNormalization);
        clampLow = scaled < -32768.0f;
    } else {
        value = gabi::fmsubs(gabi::fadds_ppc(value, threshold), mSlope, threshold);
        scaled = gabi::fmuls_ppc(value, mNormalization);
        clampLow = scaled < -32768.0f;
    }
    s32 result;
    if (clampLow) result = -32768;
    else {
        if (scaled > 32767.0f) scaled = 32767.0f;
        result = gabi::ftoi(scaled);
    }
    scratch->mUpper = result;
    *sample = (s32)scratch->mUpper;
}
/* 02765D30 */
void sead::tablet_audio::Runtime::limitChannels(const gptr<be<s32>>* channels) {
    gabi::AutoFrame frame{gabi::current_function()};
    for (u32 i = 0; i < 144; ++i) {
        limitSample(&channels[0].get()[i]);
        limitSample(&channels[1].get()[i]);
        limitSample(&channels[2].get()[i]);
        limitSample(&channels[3].get()[i]);
    }
}
/* 02765D94 */
void sead::tablet_audio::Runtime::process() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<gptr<be<s32>>[2]> stereo(8);
    gabi::FrameLocal<gptr<be<s32>>[4]> channels(16);
    u32 target = mWriteCursor;
    u32 cursor = mProcessCursor;
    updateHeadphones();
    while (cursor != target) {
        u32 offset = cursor * 4;
        be<s32>* right = gabi::at<be<s32>>(gabi::ea(mBuffers[1].get()) + offset);
        be<s32>* left = gabi::at<be<s32>>(gabi::ea(mBuffers[0].get()) + offset);
        be<s32>* rearLeft = gabi::at<be<s32>>(gabi::ea(mBuffers[2].get()) + offset);
        be<s32>* rearRight = gabi::at<be<s32>>(gabi::ea(mBuffers[3].get()) + offset);
        (*stereo)[1] = right;
        (*channels)[0] = left;
        (*channels)[3] = rearRight;
        (*channels)[2] = rearLeft;
        (*stereo)[0] = left;
        (*channels)[1] = right;
        if (mLimit) limitChannels(*channels);
        processTabletAudioChannels(*channels, *stereo, 144);
        if (mDownmixExtra) {
            be<u32>* extraLeft = gabi::at<be<u32>>(gabi::ea(mBuffers[4].get()) + offset);
            be<u32>* extraRight = gabi::at<be<u32>>(gabi::ea(mBuffers[5].get()) + offset);
            for (u32 i = 0; i < 144; ++i) {
                be<s32>* outputLeft = (*stereo)[0];
                outputLeft[i] = (u32)outputLeft[i] + (u32)extraLeft[i];
                be<s32>* outputRight = (*stereo)[1];
                outputRight[i] = (u32)outputRight[i] + (u32)extraRight[i];
            }
        }
        u32 count = mBlocks;
        cursor += 144;
        if (cursor >= count * 144) cursor = 0;
    }
    mProcessCursor = cursor;
}
/* 02765ED0 */
void sead::tablet_audio::Runtime::callback(u32 device) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (device == 0 && mActive) process();
}
/* 02765EE8 */
static void tabletAudioReferenceCallback(sead::tablet_audio::Reference* reference, u32 device) {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::tablet_audio::Runtime* runtime = gabi::at<sead::tablet_audio::Runtime>(gabi::ea(reference) - 4);
    runtime->callback(device);
}
/* 02765EF0 */
void sead::tablet_audio::Settings::initialize() {
    gabi::AutoFrame frame{gabi::current_function()};
    mSpeakerPosition[0] = 1;
    mMode = 1;
    mDownmix[0] = 1.0f;
    mDownmix[1] = 1.0f;
    mOutputGain[1] = 1.0f;
    mSpeakerPosition[1] = 1;
    mDepth[1] = 1.0f;
    mSurroundGain = 0;
    mOutputGain[0] = 1.0f;
    mDepth[0] = 1.0f;
}
/* 02765F2C */
void sead::tablet_audio::Runtime::setLimiter(f32 threshold, f32 slope, f32 inputGain) {
    gabi::AutoFrame frame{gabi::current_function()};
    f32 distance = gabi::fsubs_ppc(1.0f, threshold);
    f32 compressed = gabi::fmadds(distance, slope, threshold);
    f32 level = gabi::fmuls_ppc(threshold, 32767.0f);
    f32 scale = gabi::fmuls_ppc(compressed, inputGain);
    mThreshold = level;
    mInputGain = inputGain;
    f32 normalization = gabi::fdivs_ppc(1.0f, scale);
    mSlope = slope;
    mNormalization = normalization;
}
/* 02765F64 */
void sead::tablet_audio::Runtime::resetLimiter() {
    gabi::AutoFrame frame{gabi::current_function()};
    setLimiter(0.5f, 0.4000000059604645f, 1.0f);
}
/* 02765F80 */
static sead::tablet_audio::Runtime* createTabletAudioRuntime(sead::tablet_audio::Runtime* runtime) {
    gabi::AutoFrame frame{gabi::current_function()};
    using namespace sead::tablet_audio;
    if (runtime == NULL) runtime = (Runtime*)allocateTabletAudioObject(sizeof(Runtime));
    if (runtime != NULL) {
        runtime->mTable = gabi::ea(&sTabletAudioBaseTable);
        runtime->mCompletion.mTable = reinterpret_cast<ReferenceTable*>(&sTabletAudioReferenceBaseTable);
        ReferenceCount* reference = gabi::at<ReferenceCount>(gabi::ea(runtime) + 4);
        if (reference == NULL) reference = (ReferenceCount*)allocateTabletAudioObject(4);
        if (reference != NULL) reference->mReferences = 0;
        runtime->mTable = gabi::ea(&sTabletAudioTable);
        runtime->mCompletion.mTable = reinterpret_cast<ReferenceTable*>(&sTabletAudioReferenceTable);
        runtime->mSettings.initialize();
        runtime->mPrevious.initialize();
        runtime->mHeadphones = 0;
        runtime->mProcessCursor = 0;
        runtime->mStackSize = 0x1000;
        runtime->mLc = 0;
        runtime->mLimit = 0;
        runtime->mThread = NULL;
        runtime->mInputGain = 1.0f;
        runtime->mDownmixExtra = 0;
        runtime->mSlope = 1.0f;
        runtime->mDeviceMode = 0;
        runtime->mThreshold = 1.0f;
        runtime->mSoftwareMix = 0;
        runtime->mNormalization = 1.0f;
        runtime->mPriority = 16;
        runtime->mReadCursor = 0;
        runtime->mWriteCursor = 0;
        runtime->mStack = NULL;
        runtime->mActive = 0;
        runtime->mWorker = NULL;
        runtime->mModeDirty = 0;
        runtime->mQueuedBlocks = 0;
        runtime->mCore = 1;
        runtime->mBypassSoftware = 0;
        runtime->mPrebufferBlocks = 4;
        runtime->mBlocks = 8;
        sTabletAudioRuntime = runtime;
        for (u32 i = 0; i < 8; ++i) runtime->mBuffers[i] = NULL;
        runtime->resetLimiter();
    }
    return runtime;
}

static u32 sTabletAudioMixCallback;
static u8 sTabletAudioCoreAffinities[3] = {1,2,4};
static u8 sTabletAudioRendezvousMasks[3] = {1,2,4};
/* 027660D8 */
void sead::tablet_audio::Runtime::unregisterMixCallback() {
    gabi::AutoFrame frame{gabi::current_function()};
    registerTabletFinalMix(1, sTabletAudioMixCallback);
    sTabletAudioMixCallback = 0;
}
/* 02766114 */
u32 sead::tablet_audio::Runtime::coreAffinity() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 core = mCore;
    return core <= 2 ? sTabletAudioCoreAffinities[core] : 0;
}
/* 02766138 */
u32 sead::tablet_audio::Runtime::rendezvousMask() {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 core = mCore;
    return core <= 2 ? sTabletAudioRendezvousMasks[core] : 0;
}
/* 027664A4 */
void sead::tablet_audio::Runtime::applyMode() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mSoftwareMix) {
        setTabletSurroundMode(0);
        bool bypass = true;
        if ((u32)mSettings.mMode != 0 && (u32)mDeviceMode == 2) bypass = false;
        u32 mode = mSettings.mMode;
        mBypassSoftware = bypass;
        mDownmixExtra = mode == 2;
        mModeDirty = 0;
    } else {
        if ((u32)mDeviceMode == 2) setTabletSurroundMode(mSettings.mMode);
        else setTabletSurroundMode(0);
        mModeDirty = 0;
    }
}
/* 02766578 */
void sead::tablet_audio::Runtime::applyLc() {
    gabi::AutoFrame frame{gabi::current_function()};
    setTabletSurroundLc(mLc);
}
/* 02766580 */
void sead::tablet_audio::Runtime::applySpeaker(u32 channel) {
    gabi::AutoFrame frame{gabi::current_function()};
    setTabletSurroundSpeaker(channel, sead::elementAt(&mSettings.mSpeakerPosition[0], channel)); /* guest EA arithmetic: wraps at 32 bits as on the console */
}
/* 02766594 */
void sead::tablet_audio::Runtime::applyDepth(u32 channel) {
    gabi::AutoFrame frame{gabi::current_function()};
    setTabletSurroundDepth(channel, sead::elementAt(&mSettings.mDepth[0], channel));
}
/* 027665A8 */
void sead::tablet_audio::Runtime::applyDownmix(u32 channel) {
    gabi::AutoFrame frame{gabi::current_function()};
    setTabletSurroundDownmix(channel, sead::elementAt(&mSettings.mDownmix[0], channel));
}
/* 027665BC */
void sead::tablet_audio::Runtime::applySurroundGain() {
    gabi::AutoFrame frame{gabi::current_function()};
    setTabletSurroundLevel(mSettings.mSurroundGain);
}
/* 027665C4 */
void sead::tablet_audio::Runtime::applyOutputGain(u32 channel) {
    gabi::AutoFrame frame{gabi::current_function()};
    setTabletSurroundOutput(channel, sead::elementAt(&mSettings.mOutputGain[0], channel));
}
/* 027665D8 */
void sead::tablet_audio::Runtime::applyAll() {
    gabi::AutoFrame frame{gabi::current_function()};
    applyMode();
    applyLc();
    applySpeaker(0);
    applySpeaker(1);
    applyDepth(0);
    applyDepth(1);
    applyDownmix(0);
    applyDownmix(1);
    applySurroundGain();
    applyOutputGain(0);
    applyOutputGain(1);
}
/* 02766674 */
void sead::tablet_audio::Runtime::initializeSettings() {
    gabi::AutoFrame frame{gabi::current_function()};
    applyAll();
}
/* 02766CBC */
void sead::tablet_audio::Runtime::applyChanged() {
    gabi::AutoFrame frame{gabi::current_function()};
    if ((u32)mSettings.mMode != (u32)mPrevious.mMode || mModeDirty) applyMode();
    if ((u32)mSettings.mSpeakerPosition[0] != (u32)mPrevious.mSpeakerPosition[0]) applySpeaker(0);
    if ((u32)mSettings.mSpeakerPosition[1] != (u32)mPrevious.mSpeakerPosition[1]) applySpeaker(1);
    if ((f32)mSettings.mDepth[0] != (f32)mPrevious.mDepth[0]) applyDepth(0);
    if ((f32)mSettings.mDepth[1] != (f32)mPrevious.mDepth[1]) applyDepth(1);
    if ((f32)mSettings.mDownmix[0] != (f32)mPrevious.mDownmix[0]) applyDownmix(0);
    if ((f32)mSettings.mDownmix[1] != (f32)mPrevious.mDownmix[1]) applyDownmix(1);
    if ((u32)mSettings.mSurroundGain != (u32)mPrevious.mSurroundGain) applySurroundGain();
    if ((f32)mSettings.mOutputGain[0] != (f32)mPrevious.mOutputGain[0]) applyOutputGain(0);
    if ((f32)mSettings.mOutputGain[1] != (f32)mPrevious.mOutputGain[1]) applyOutputGain(1);
}
/* 02766E00 */
void sead::tablet_audio::Runtime::updateSettings() {
    gabi::AutoFrame frame{gabi::current_function()};
    applyChanged();
}
/* 02766F1C */
void sead::tablet_audio::Runtime::setSettings(const Settings* settings) {
    gabi::AutoFrame frame{gabi::current_function()};
    const be<u32>* currentWords = reinterpret_cast<const be<u32>*>(&mSettings);
    be<u32>* previousWords = reinterpret_cast<be<u32>*>(&mPrevious);
    for (u32 i = 0; i < 10; i += 2) {
        previousWords[i] = (u32)currentWords[i];
        previousWords[i+1] = (u32)currentWords[i+1];
    }
    const be<u32>* sourceWords = reinterpret_cast<const be<u32>*>(settings);
    be<u32>* destinationWords = reinterpret_cast<be<u32>*>(&mSettings);
    for (u32 i = 0; i < 10; i += 2) {
        destinationWords[i] = (u32)sourceWords[i];
        destinationWords[i+1] = (u32)sourceWords[i+1];
    }
    if (mActive) {
        u32 currentCore = tabletAudioCoreId();
        if ((u32)mCore != currentCore) dispatchTabletAudioSettings(this);
        else updateSettings();
    }
}
/* 02766FC8 */
void sead::tablet_audio::Runtime::setDeviceMode(u32 mode) {
    gabi::AutoFrame frame{gabi::current_function()};
    bool active = mActive != 0;
    mDeviceMode = mode;
    if (active) {
        mModeDirty = 1;
        setSettings(&mSettings);
    }
}

// SDK facts: the OSRendezvous record layout (three per-core arrival words and the SDK context word) from wut
// coreinit/rendezvous.h and Cemu's coreinit; layout only, no implementation used. The object lives at its
// original address in this translation unit's data.
struct TabletAudioRendezvous { be<u32> mCoreStates[3]; be<u32> mSdkContext; };
WWHD_OFFSET(TabletAudioRendezvous,mCoreStates,0);
WWHD_OFFSET(TabletAudioRendezvous,mSdkContext,0xC);
WWHD_SIZE(TabletAudioRendezvous,0x10);
static TabletAudioRendezvous sTabletAudioRendezvous = {{0,0,0},0};
static sead::tablet_audio::WorkerText sTabletAudioName;
static sead::tablet_audio::WorkerText sTabletAudioThreadName;
static sead::SafeTextVtable sTabletAudioNameVtable;
static u32 sTabletAudioNameText;
static u32 sTabletAudioThreadNameText;

/* 0276615C */
static u32 tabletAudioEmptyEntry(void*) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 mask = sTabletAudioRuntime->rendezvousMask();
    waitTabletAudioRendezvous(&sTabletAudioRendezvous, mask | 2);
    return 0;
}
static void startTabletAudioThread(sead::tablet_audio::Runtime* runtime, u32 (*entry)(void*)) {
    u32 affinity = runtime->coreAffinity();
    u32 mask = runtime->rendezvousMask();
    initializeTabletAudioRendezvous(&sTabletAudioRendezvous);
    be<u8>* stack = runtime->mStack;
    u32 bytes = runtime->mStackSize;
    sead::PlatformThread* thread = runtime->mThread;
    u32 priority = runtime->mPriority;
    u32 callback = gabi::guest_fn(reinterpret_cast<const void*>(entry));
    createTabletAudioThread(thread, callback, 0, runtime, gabi::at<void>(gabi::ea(stack) + bytes), bytes, priority, (affinity | 8) & 0xFFFF);
    sTabletAudioThreadName.assureTermination();
    nameTabletAudioThread(runtime->mThread, sTabletAudioThreadName.mText);
    resumeTabletAudioThread(runtime->mThread);
    waitTabletAudioRendezvous(&sTabletAudioRendezvous, mask);
}
/* 02766198 */
static void dispatchTabletAudioEmpty(sead::tablet_audio::Runtime* runtime) {
    gabi::AutoFrame frame{gabi::current_function()};
    startTabletAudioThread(runtime, tabletAudioEmptyEntry);
}
/* 02766264 */
void sead::tablet_audio::Runtime::stop() {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mActive) return;
    if (mWorker != NULL && (u32)mCompletion.mReferences != 0) {
        do {
            const be<u32>* info = tabletAudioSystemInfo();
            u32 frequency = *info;
            u64 product = (u64)(frequency >> 2) * 100;
            auto ticks = divideTabletAudioTicks(product >> 32, (u32)product, 0, 1000000);
            sleepTabletAudioTicks(ticks.r3, ticks.r4);
        } while ((u32)mCompletion.mReferences != 0);
    }
    if (mSoftwareMix) unregisterMixCallback();
    u32 core = tabletAudioCoreId();
    if ((u32)mCore != core) dispatchTabletAudioEmpty(this);
    mActive = 0;
}
/* 02766334 */
void sead::tablet_audio::Runtime::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (gabi::ea(this) == 0) return;
    mTable = gabi::ea(&sTabletAudioTable);
    mCompletion.mTable = reinterpret_cast<ReferenceTable*>(&sTabletAudioReferenceTable);
    stop();
    for (u32 i = 0; i < 8; ++i) {
        void* buffer = mBuffers[i];
        if (buffer != NULL) releaseFoundationArray(buffer);
        mBuffers[i] = NULL;
    }
    void* stack = mStack;
    if (stack != NULL) {
        releaseFoundationArray(stack);
        mStack = NULL;
    }
    sead::PlatformThread* thread = mThread;
    if (thread != NULL) {
        operator_delete(thread);
        mThread = NULL;
    }
    sTabletAudioRuntime = NULL;
    if (flags & 1) operator_delete(this);
}
static sead::DisposerVtable sAudioSystemTypeTable;
/* 027663F8 */
static Worker* findTabletAudioWorker() {
    gabi::AutoFrame frame{gabi::current_function()};
    sead::AudioServices* services = sAudioServices;
    if (services == NULL) return NULL;
    sead::AudioService* backend = services->mBackend;
    if (backend == NULL) return NULL;
    sead::initializeRuntimeType(sAudioSystemType, sAudioSystemTypeGuard, &sAudioSystemTypeTable);
    sead::RuntimeTypedObject* object = reinterpret_cast<sead::RuntimeTypedObject*>(backend);
    u32 matches = gabi::call_ptr<u32>(object->mVtable->mIsKindOf.mFunction, object, &sAudioSystemType);
    if (matches == 0 || backend == NULL) return NULL;
    sead::AudioSystem* system = reinterpret_cast<sead::AudioSystem*>(backend);
    return static_cast<Worker*>(system->mAuxiliaryWorker.get());
}
/* 02766678 */
static u32 tabletAudioInitializeEntry(void*) {
    gabi::AutoFrame frame{gabi::current_function()};
    sTabletAudioRuntime->initializeSettings();
    u32 mask = sTabletAudioRuntime->rendezvousMask();
    waitTabletAudioRendezvous(&sTabletAudioRendezvous, mask | 2);
    return 0;
}
/* 027666C4 */
static void dispatchTabletAudioInitialize(sead::tablet_audio::Runtime* runtime) {
    gabi::AutoFrame frame{gabi::current_function()};
    startTabletAudioThread(runtime, tabletAudioInitializeEntry);
}
/* 027667D8 */
static u32 queryTabletAudioDeviceMode() {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u32>> mode(8);
    getTabletAudioDeviceMode(1, mode.get());
    return *mode;
}
/* 02766E04 */
static u32 tabletAudioSettingsEntry(void*) {
    gabi::AutoFrame frame{gabi::current_function()};
    sTabletAudioRuntime->updateSettings();
    u32 mask = sTabletAudioRuntime->rendezvousMask();
    waitTabletAudioRendezvous(&sTabletAudioRendezvous, mask | 2);
    return 0;
}
/* 02766E50 */
void dispatchTabletAudioSettings(sead::tablet_audio::Runtime* runtime) {
    gabi::AutoFrame frame{gabi::current_function()};
    startTabletAudioThread(runtime, tabletAudioSettingsEntry);
}
/* 02766FE8 */
static void initializeTabletAudioNames() {
    gabi::AutoFrame frame{gabi::current_function()};
    sTabletAudioName.mVtable = &sTabletAudioNameVtable;
    sTabletAudioName.mText = reinterpret_cast<be<u8>*>(&sTabletAudioNameText);
    sTabletAudioThreadName.mVtable = &sTabletAudioNameVtable;
    sTabletAudioThreadName.mText = reinterpret_cast<be<u8>*>(&sTabletAudioThreadNameText);
}

/* 02766804 */
void sead::tablet_audio::Runtime::mixBlock(const be<u32>* left,const be<u32>* right,
                                        const be<u32>* rearLeft,const be<u32>* rearRight,
                                        const be<u32>* extraLeft,const be<u32>* extraRight,
                                        be<u32>* outputLeft,be<u32>* outputRight) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (queryTabletAudioDeviceMode() != 1) return;
    u32 cursor = mWriteCursor;
    u32 offset = cursor * 4;
    u32 capacity = (u32)mBlocks * 144;
    for (u32 channel = 0; channel < 4; ++channel)
        zeroTabletAudioRange(gabi::at<void>(gabi::ea(mBuffers[channel].get()) + offset), 0x240);
    if (mDownmixExtra) {
        zeroTabletAudioRange(gabi::at<void>(gabi::ea(mBuffers[4].get()) + offset), 0x240);
        zeroTabletAudioRange(gabi::at<void>(gabi::ea(mBuffers[5].get()) + offset), 0x240);
    }
    moveFoundationBytes(gabi::at<void>(gabi::ea(mBuffers[0].get()) + offset), left, 0x240, 0);
    moveFoundationBytes(gabi::at<void>(gabi::ea(mBuffers[1].get()) + offset), right, 0x240, 0);
    moveFoundationBytes(gabi::at<void>(gabi::ea(mBuffers[2].get()) + offset), rearLeft, 0x240, 0);
    moveFoundationBytes(gabi::at<void>(gabi::ea(mBuffers[3].get()) + offset), rearRight, 0x240, 0);
    if (mDownmixExtra) {
        if (extraLeft != NULL) moveFoundationBytes(gabi::at<void>(gabi::ea(mBuffers[4].get()) + offset), extraLeft, 0x240, 0);
        if (extraRight != NULL) moveFoundationBytes(gabi::at<void>(gabi::ea(mBuffers[5].get()) + offset), extraRight, 0x240, 0);
    }
    cursor += 144;
    u32 prebuffer = mPrebufferBlocks;
    u32 queued = mQueuedBlocks;
    if (cursor >= capacity) cursor = 0;
    mWriteCursor = cursor;
    if (queued < prebuffer) mQueuedBlocks = queued + 1;
    u32 processed = mProcessCursor;
    zeroTabletAudioRange(outputLeft, 0x240);
    zeroTabletAudioRange(outputRight, 0x240);
    u32 read = mReadCursor;
    if (processed != read && (u32)mQueuedBlocks >= (u32)mPrebufferBlocks) {
        moveFoundationBytes(outputLeft, gabi::at<const void>(gabi::ea(mBuffers[0].get()) + read * 4), 0x240, 0);
        read = mReadCursor;
        moveFoundationBytes(outputRight, gabi::at<const void>(gabi::ea(mBuffers[1].get()) + read * 4), 0x240, 0);
        u32 next = (u32)mReadCursor + 144;
        mReadCursor = next < capacity ? next : 0;
        mWorker->sendReference(&mCompletion);
    } else {
        setTabletAudioBytes(outputLeft, 0, 0x240);
        setTabletAudioBytes(outputRight, 0, 0x240);
        mWorker->sendReference(&mCompletion);
    }
}
/* 02766A88 */
void sead::tablet_audio::Runtime::mixCallback(const MixDescriptor* descriptor) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!mActive || mBypassSoftware || !mSoftwareMix || mWorker == NULL) return;
    const MixChannels* channels = descriptor->mChannels;
    be<u32>* left = channels->mChannels[0];
    be<u32>* right = channels->mChannels[1];
    be<u32>* rearLeft = channels->mChannels[2];
    be<u32>* rearRight = channels->mChannels[3];
    be<u32>* extraLeft = channels->mChannels[4];
    be<u32>* extraRight = channels->mChannels[5];
    mixBlock(left,right,rearLeft,rearRight,extraLeft,extraRight,left,right);
}
/* 02766AFC */
static void tabletAudioFinalMix(const sead::tablet_audio::MixDescriptor* descriptor) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 callback = sTabletAudioMixCallback;
    if (callback != 0) gabi::call_ptr<void>(callback, descriptor);
    sTabletAudioRuntime->mixCallback(descriptor);
}
/* 02766B50 */
void sead::tablet_audio::Runtime::registerMixCallback() {
    gabi::AutoFrame frame{gabi::current_function()};
    getTabletFinalMix(1, &sTabletAudioMixCallback);
    registerTabletFinalMix(1, gabi::guest_fn(reinterpret_cast<const void*>(tabletAudioFinalMix)));
}
/* 02766B8C */
void sead::tablet_audio::Runtime::initialize(u32 core,void* heap) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mActive) return;
    if (mSoftwareMix) {
        mWorker = findTabletAudioWorker();
        for (u32 i = 0; i < 8; ++i) {
            void* buffer = allocateFoundationArray((u32)mBlocks * 0x240, heap, 64);
            mBuffers[i] = static_cast<be<u32>*>(buffer);
            setTabletAudioBytes(buffer, 0, (u32)mBlocks * 0x240);
        }
        mReadCursor = 0;
        mCore = core;
        mProcessCursor = 0;
        mQueuedBlocks = 0;
        mWriteCursor = 0;
    } else {
        mWorker = NULL;
        mCore = core;
    }
    if (core != tabletAudioCoreId()) {
        mThread = allocateTabletAudioThread(0x6A0, heap, 8);
        mStack = static_cast<be<u8>*>(allocateFoundationArray(mStackSize, heap, 4));
        dispatchTabletAudioInitialize(this);
    } else initializeSettings();
    if (mSoftwareMix) {
        resetHeadphones();
        registerMixCallback();
    }
    mActive = 1;
}

/* WWHD glue
 data sTabletAudioRuntime 101F8BBC
 data sTabletAudioBaseTable 1015F29C
 data sTabletAudioReferenceBaseTable 1015F2BC
 data sTabletAudioTable 1015F310
 data sTabletAudioReferenceTable 1015F330
 data sTabletAudioMixCallback 101F8BC0
 data sTabletAudioCoreAffinities 1015F2DC
 data sTabletAudioRendezvousMasks 1015F2DF
 data sTabletAudioRendezvous 104A1228
 data sTabletAudioName 104A1218
 data sTabletAudioThreadName 104A1220
 data sTabletAudioNameVtable 1015F274
 data sTabletAudioNameText 1015F300
 data sTabletAudioThreadNameText 1015F2E4
 data sAudioSystemTypeTable 1015F2CC
 data sAudioWorkerText 104A1210
 data sAudioWorkerVtable 1015F1C0
 data sAudioWorkerNameOwnerVtable 1015F198
 data sAudioWorkerTextVtable 1015F180
 data sAudioWorkerName 1015F1A8
 ext 0273AD10 void* allocateTabletAudioObject(u32)
 ext 027607C0 sead::Thread* sead::Thread::construct(sead::Thread*, const sead::SafeText*, sead::RegistryHeap*, s32, u32, u32, u32, s32)
 ext 0275FF7C void sead::Thread::quitAndJoin(u32)
 ext 027609AC void sead::Thread::destroy(u32)
 ext 02760C4C void sead::Thread::setCoreMask(const be<u32>*)
 ext 02760B1C u32 sead::Thread::start()
 ext C0006ED8 void registerTabletFinalMix(u32,u32)
 ext C0006F50 void setTabletSurroundMode(u32)
 ext C0006F38 void setTabletSurroundLc(u32)
 ext C0006F60 void setTabletSurroundSpeaker(u32,u32)
 ext C0006F68 void setTabletSurroundDepth(u32,f32)
 ext C0006F30 void setTabletSurroundDownmix(u32,f32)
 ext C0006F70 void setTabletSurroundLevel(u32)
 ext C0006F58 void setTabletSurroundOutput(u32,f32)
 ext C0009D78 void initializeTabletAudioRendezvous(void*)
 ext C000A208 void waitTabletAudioRendezvous(void*,u32)
 ext C0009A48 u32 createTabletAudioThread(sead::PlatformThread*,u32,u32,void*,void*,u32,u32,u32)
 ext C000A078 void nameTabletAudioThread(sead::PlatformThread*,const be<u8>*)
 ext C0009F38 void resumeTabletAudioThread(sead::PlatformThread*)
 ext C0009C80 const be<u32>* tabletAudioSystemInfo()
 ext 028F5A1C Pair32 divideTabletAudioTicks(u32,u32,u32,u32)
 ext C000A0D0 void sleepTabletAudioTicks(u32,u32)
 ext C0006D48 void getTabletAudioDeviceMode(u32,be<u32>*)
 ext 0273AFC8 void releaseFoundationArray(void*)
 ext C00088C8 void zeroTabletAudioRange(void*,u32)
 ext C0009990 void setTabletAudioBytes(void*,u32,u32)
 ext C0006D38 void getTabletFinalMix(u32,be<u32>*)
 ext 0273B050 sead::PlatformThread* allocateTabletAudioThread(u32,void*,s32)
 ext C0009988 void moveFoundationBytes(void*,const void*,u32,u32)
 ext C0009BB0 u32 tabletAudioCoreId()
 ext C0007778 u32 tabletAudioHeadphoneStatus(u32)
 ext C00071B8 void tabletAudioOutputMode(u32)
 ext C0007178 void processTabletAudioChannels(const gptr<be<s32>>*, const gptr<be<s32>>*, u32)
*/

/* Based on CC0 tww JASTrack.cpp; Cafe track control. */
#include "JSystem/JAudio/JASTrack.h"
#include "JSystem/JAudio/JASChannel.h"
#include "JSystem/JAudio/JASCalc.h"
using namespace JASystem;
static TrackHeaderStatics headerStatics;
static TrackDestructorRecord destructorRecord;
static TrackParser sParser;
static TrackOscillatorWords defaultEnvelope;
static TrackOscillatorWords defaultVibrato;
static TrackOscillatorWords defaultTremolo;
using TrackPointer = gptr<TTrack>;
static TrackPointer freeListHead;
static TrackPointer freeListTail;
using TrackHeapPointer = gptr<void>;
static TrackHeapPointer trackAudioHeap;
static s16 sAdsTable[12] = {0,0,0x7FFF,0,0,0x7FFF,0,0,0,14,0,0};
/* HD: compiler-generated construction uses the track pool rather than operator new. */
/* 02816ED8 */
TTrack* constructTrack(TTrack* track) {
    if (!track) {
        track = freeListHead;
        if (!track) return nullptr;
        freeListHead = track->mPoolNext;
        if (!freeListHead) freeListTail = nullptr;
    }
    track->mChannelList.initiate();
    constructTrackVibrate(&track->mVibrate);
    constructTrackChannels(&track->mChannelUpdater);
    track->mConnectMode = 0;
    track->mChannelState.mFlags = 0;
    track->mEnabled = 0;
    track->mTempoScale = 0.0f;
    track->mStatus = 0;
    track->mTempoAccumulator = 0;
    track->mRoute = 0;
    track->mIsPaused = 0;
    track->mParameterMode = 0;
    track->mMute = 0;
    track->mUpdateFlags = 0;
    track->mParent = nullptr;
    track->mPauseStatus = 0;
    track->mTempo = 120;
    track->mVolumeMode = 0;
    track->mTimebase = 120;
    track->mTempoDirty = 0;
    track->mTempoFraction = 0.0f;
    track->mTranspose = 0;
    track->mAutoDelete = 0;
    initializeTrackChannels(&track->mChannelUpdater);
    for (u32 i = 0; i < 12; ++i) track->mFilterCoefficients[i] = sAdsTable[i];
    JASystem::Calc::bzero(&track->mTimedParam,0x120);
    return track;
}
/* 0281702C */
void destroyTrack(TTrack* track,u32 flags) {
    if (!track) return;
    JSUPtrLink* link = track->mChannelList.mHead;
    while (link) {
        TChannel* channel = (TChannel*)(void*)link->mObject;
        JSUPtrLink* next = channel->mLinkState.mNext;
        channel->stop(0);
        channel = (TChannel*)(void*)link->mObject;
        channel->clearCallback();
        channel = (TChannel*)(void*)link->mObject;
        track->mChannelList.remove((JSUPtrLink*)(void*)channel->mLinkState.mObject);
        link = next;
    }
    destroyTrackList(&track->mChannelList,0);
    if (flags & 1) {
        track->mPoolNext = nullptr;
        TTrack* tail = freeListTail;
        if (tail) { tail->mPoolNext = track; freeListTail = track; }
        else { freeListTail = track; freeListHead = track; }
    }
}
static s8 quantizeTrackDelay(f32 value) {
    // psq_st with an s8 GQR type: a NaN saturates by its sign bit (750CL, paired-single store)
    if (value != value) return (gabi::f32_bits(value) >> 31) ? -128 : 127;
    if (value < -128.0f) return -128;
    if (value > 127.0f) return 127;
    return (s8)gabi::ftoi(value);
}
namespace JASystem {
/* 028170F4 */
void TTrack::initTimed() {
    for (u32 i = 0; i < TIMED_PARAM_COUNT; ++i) {
        mTimedParam[i].mDuration = 0.0f;
        mTimedParam[i].mCurrent = 1.0f;
        mTimedParam[i].mTarget = 1.0f;
    }
    const u32 zeroParameters[] = {1, 2, 4, 5, 13, 14, 15, 17};
    for (u32 index : zeroParameters) {
        mTimedParam[index].mCurrent = 0.0f;
        mTimedParam[index].mTarget = 0.0f;
    }
    mTimedParam[3].mCurrent = mTimedParam[3].mTarget = 0.5f;
    mTimedParam[16].mCurrent = mTimedParam[16].mTarget = 0.5f;
}
/* 02817198 */
void TTrack::updateTempo() {
    if (!mParent) {
        f32 tempo = gabi::fmuls_ppc((f32)mTimebase, (f32)mTempo);
        mTempoFraction = tempo;
        f32 interval = trackUpdateInterval();
        tempo = tempo / interval;
        mTempoFraction = gabi::fmuls_ppc(tempo, 1.3333333730697632f);
        if (mOuterParam && mOuterParam->checkOuterSwitch(OUTERPARAM_Tempo)) {
            mTempoFraction = gabi::fmuls_ppc(mTempoFraction, mOuterParam->mTempo);
        }
    } else {
        mTempoFraction = mParent->mTempoFraction;
        mTimebase = mParent->mTimebase;
    }
    for (u32 i = 0; i < MAX_CHILDREN; ++i) {
        TTrack* child = mChildren[i];
        if (child && child->mStatus) child->updateTempo();
    }
}
/* 028172C0 */
void TTrack::init() {
    mSeqCtrl.init();
    mTrackPort.init();
    mIntrMgr.init();
    mBeforeTieMode = 0;
    mBaseTime = 0;
    mConnectCase = 0;
    mLastNote = 0;
    for (u32 i = 0; i < 8; ++i) mNoteChannels[i] = nullptr;
    initializeTrackChannels(&mChannelUpdater);
    mChannelState.mFlags = 0;
    initTimed();
    initializeTrackRegisters(&mRegisterParameters);
    for (u32 slot = 0; slot < 2; ++slot) {
        mOscRoute[slot] = 15;
        for (u32 word = 0; word < 6; ++word) mOscConfigWords[slot * 6 + word] = (u32)defaultEnvelope.mWords[word];
    }
    mParent = nullptr;
    for (u32 i = 0; i < MAX_CHILDREN; ++i) mChildren[i] = nullptr;
    if (mOuterParam) mOuterParam->initExtBuffer();
    mRoute = 0;
    mTempoScale = 0.0f;
    mTempoFraction = 1.0f;
    mVibrate.init();
    mTempoAccumulator = 0;
    mUpdateFlags = 0;
    mTempo = 120;
    mTimebase = 48;
    updateTempo();
    mStatus = 0;
    mVolumeMode = 0;
    mParameterMode = 0;
    mTranspose = 0;
    mPauseStatus = 10;
    mConnectMode = 0;
    for (u32 i = 0; i < 3; ++i) {
        mCalcTypes[i] = 0;
        mParentCalcTypes[i] = 0;
        mChannelState.mCalcTypes[i] = 13;
    }
    mIsPaused = 0;
    mMute = 0;
    mEnabled = 1;
    mTempoDirty = 0;
}
/* 02817494 */
void TTrack::inherit() {
    TTrack* parent = mParent;
    if (!parent) return;
    mTempo = parent->mTempo;
    mTempoDirty = 0;
    mTempoFraction = parent->mTempoFraction;
    mTimebase = parent->mTimebase;
    mEnabled = parent->mEnabled;
    mIsPaused = parent->mIsPaused;
    mPauseStatus = parent->mPauseStatus;
    mVolumeMode = parent->mVolumeMode;
    mMute = parent->mMute;
    if (mConnectMode & 2) return;
    inheritTrackRegisters(&mRegisterParameters,&mParent->mRegisterParameters);
    for (u32 i = 0; i < 3; ++i) {
        mCalcTypes[i] = mParent->mCalcTypes[i];
        mParentCalcTypes[i] = mParent->mParentCalcTypes[i];
        mChannelState.mCalcTypes[i] = mParent->mChannelState.mCalcTypes[i];
    }
}
/* 02817580 */
f32 TTrack::panCalc(f32 a,f32 b,f32 weight,u32 type) {
    switch (type) {
    case 0: return a;
    case 1: return b;
    case 2: return gabi::fmadds(a,gabi::fsubs_ppc(1.0f,weight),gabi::fmuls_ppc(b,weight));
    default: return 0.0f;
    }
}
/* 028175C0 */
void TTrack::updateTrackAll() {
    be<u16>* registers = (be<u16>*)mRegisterParameters;
    f32 outerWeight = (f32)(u16)registers[11] / 32767.0f;
    s8 delay = quantizeTrackDelay(gabi::fmuls_ppc(mTimedParam[17].mCurrent,128.0f));
    s8 leftDelay = 0;
    if (delay < 0) { leftDelay = (s8)-delay; delay = 0; }
    mChannelState.mMixerMode = 16;
    mChannelState.mDelay[0] = leftDelay;
    mChannelState.mDelay[1] = delay;
    f32 volume = mTimedParam[0].mCurrent;
    if (!mVolumeMode) volume = gabi::fmuls_ppc(volume,volume);
    if (mMute) volume = 0.0f;
    f32 pitch = vibratoPitchToCent(mTimedParam[1].mCurrent,(f32)(u16)registers[7]);
    f32 vibrato = mVibrate.getValue();
    pitch = gabi::fmuls_ppc(pitch,vibrato);
    f32 pan = mTimedParam[3].mCurrent;
    f32 fxmix = mTimedParam[2].mCurrent;
    f32 dolby = mTimedParam[4].mCurrent;
    TOuterParam* outer = mOuterParam;
    if (outer) {
        u32 enabled = outer->checkOuterSwitch(OUTERPARAM_Volume);
        outer = mOuterParam;
        if (enabled) volume = gabi::fmuls_ppc(volume,outer->mVolume);
        enabled = outer->checkOuterSwitch(OUTERPARAM_Pitch);
        outer = mOuterParam;
        if (enabled) pitch = gabi::fmuls_ppc(pitch,outer->mPitch);
        enabled = outer->checkOuterSwitch(OUTERPARAM_Fxmix);
        outer = mOuterParam;
        if (enabled) fxmix = panCalc(fxmix,outer->mFxmix,outerWeight,mCalcTypes[1]);
        enabled = outer->checkOuterSwitch(OUTERPARAM_Dolby);
        outer = mOuterParam;
        if (enabled) dolby = panCalc(dolby,outer->mDolby,outerWeight,mCalcTypes[2]);
        enabled = outer->checkOuterSwitch(OUTERPARAM_Pan);
        if (enabled) { outer = mOuterParam; pan = panCalc(pan,outer->mPan,outerWeight,mCalcTypes[0]); }
    }
    if (!mParent || (mConnectMode & 1)) {
        mChannelState.mVolume = volume;
        mChannelState.mPitch = pitch;
        mChannelState.mPan = pan;
        mChannelState.mFxMix = fxmix;
        mChannelState.mDolby = dolby;
        // HD: independent tracks return before refreshing the filters here.
        return;
    }
    f32 parentVolume = mParent->mChannelState.mVolume;
    f32 parentWeight = (f32)(u16)registers[12] / 32767.0f;
    mChannelState.mVolume = gabi::fmuls_ppc(parentVolume,volume);
    mChannelState.mPitch = gabi::fmuls_ppc(mParent->mChannelState.mPitch,pitch);
    mChannelState.mPan = panCalc(pan,mParent->mChannelState.mPan,parentWeight,mParentCalcTypes[0]);
    mChannelState.mFxMix = panCalc(fxmix,mParent->mChannelState.mFxMix,parentWeight,mParentCalcTypes[1]);
    mChannelState.mDolby = panCalc(dolby,mParent->mChannelState.mDolby,parentWeight,mParentCalcTypes[2]);
    if (mOuterParam && mOuterParam->checkOuterSwitch(OUTERPARAM_FIR8Filter)) {
        for (u32 i = 0; i < 8; ++i) mChannelState.mFir[i] = mOuterParam->getIntFirFilter(i);
        mChannelState.mFilterFlags = 8;
    }
    for (u32 i = 0; i < 4; ++i) mChannelState.mIir[i] = (s16)gabi::ftoi(gabi::fmuls_ppc(mTimedParam[12 + i].mCurrent,32767.0f));
    mChannelState.mFilterFlags = mChannelState.mFilterFlags | 32;
    mChannelState.mFilterMix = (s16)gabi::ftoi(gabi::fmuls_ppc(mTimedParam[5].mCurrent,32767.0f));
}
/* 02817C60 */
void TTrack::updateTrack(u32 flags) {
    enum { UPDATE_DELAY = 1 << 17, UPDATE_IIR = 0xF000, UPDATE_FILTER_MIX = 32 };
    be<u16>* registers = (be<u16>*)mRegisterParameters;
    f32 outerWeight = (f32)(u16)registers[11] / 32767.0f;
    if (flags & UPDATE_DELAY) {
        s8 delay = quantizeTrackDelay(gabi::fmuls_ppc(mTimedParam[17].mCurrent,128.0f));
        s8 leftDelay = 0;
        if (delay < 0) { leftDelay = (s8)-delay; delay = 0; }
        mChannelState.mDelay[0] = leftDelay;
        mChannelState.mDelay[1] = delay;
    }
    if ((flags & OUTERPARAM_Tempo) && !mParent) updateTempo();
    f32 volume = 0.0f;
    f32 pitch = 1.0f;
    f32 pan = 0.0f;
    f32 fxmix = 0.0f;
    f32 dolby = 0.0f;
    if (flags & OUTERPARAM_Volume) {
        volume = mTimedParam[0].mCurrent;
        if (!mVolumeMode) volume = gabi::fmuls_ppc(volume,volume);
        if (mMute) volume = 0.0f;
        if (mOuterParam && mOuterParam->checkOuterSwitch(OUTERPARAM_Volume)) volume = gabi::fmuls_ppc(volume,mOuterParam->mVolume);
        if (mIsPaused && (mPauseStatus & 1)) volume = gabi::fmuls_ppc(volume,mTimedParam[16].mCurrent);
    }
    if (flags & OUTERPARAM_Pitch) {
        pitch = vibratoPitchToCent(mTimedParam[1].mCurrent,(f32)(u16)registers[7]);
        f32 vibrato = mVibrate.getValue();
        pitch = gabi::fmuls_ppc(pitch,vibrato);
        if (mOuterParam && mOuterParam->checkOuterSwitch(OUTERPARAM_Pitch)) pitch = gabi::fmuls_ppc(pitch,mOuterParam->mPitch);
    }
    if (flags & OUTERPARAM_Pan) {
        pan = mTimedParam[3].mCurrent;
        if (mOuterParam && mOuterParam->checkOuterSwitch(OUTERPARAM_Pan)) pan = panCalc(pan,mOuterParam->mPan,outerWeight,mCalcTypes[0]);
    }
    if (flags & OUTERPARAM_Fxmix) {
        fxmix = mTimedParam[2].mCurrent;
        if (mOuterParam && mOuterParam->checkOuterSwitch(OUTERPARAM_Fxmix)) fxmix = panCalc(fxmix,mOuterParam->mFxmix,outerWeight,mCalcTypes[1]);
    }
    if (flags & OUTERPARAM_Dolby) {
        dolby = mTimedParam[4].mCurrent;
        if (mOuterParam && mOuterParam->checkOuterSwitch(OUTERPARAM_Dolby)) dolby = panCalc(dolby,mOuterParam->mDolby,outerWeight,mCalcTypes[2]);
    }
    if (flags & UPDATE_IIR) {
        for (u32 i = 0; i < 4; ++i) mChannelState.mIir[i] = (s16)gabi::ftoi(gabi::fmuls_ppc(mTimedParam[12 + i].mCurrent,32767.0f));
        mChannelState.mFilterFlags = mChannelState.mFilterFlags | 32;
    }
    if (mOuterParam && (flags & OUTERPARAM_FIR8Filter) && mOuterParam->checkOuterSwitch(OUTERPARAM_FIR8Filter)) {
        for (u32 i = 0; i < 8; ++i) mChannelState.mFir[i] = mOuterParam->getIntFirFilter(i);
        mChannelState.mFilterFlags = (mChannelState.mFilterFlags & 32) + 8;
    }
    if (flags & UPDATE_FILTER_MIX) mChannelState.mFilterMix = (s16)gabi::ftoi(gabi::fmuls_ppc(mTimedParam[5].mCurrent,32767.0f));
    if (!mParent || (mConnectMode & 1)) {
        if (flags & OUTERPARAM_Volume) mChannelState.mVolume = volume;
        if (flags & OUTERPARAM_Pitch) mChannelState.mPitch = pitch;
        if (flags & OUTERPARAM_Pan) mChannelState.mPan = pan;
        if (flags & OUTERPARAM_Fxmix) mChannelState.mFxMix = fxmix;
        if (flags & OUTERPARAM_Dolby) mChannelState.mDolby = dolby;
    } else {
        f32 weight = (f32)(u16)registers[12] / 32767.0f;
        if (flags & OUTERPARAM_Volume) mChannelState.mVolume = gabi::fmuls_ppc(mParent->mChannelState.mVolume,volume);
        if (flags & OUTERPARAM_Pitch) mChannelState.mPitch = gabi::fmuls_ppc(mParent->mChannelState.mPitch,pitch);
        if (flags & OUTERPARAM_Pan) mChannelState.mPan = panCalc(pan,mParent->mChannelState.mPan,weight,mParentCalcTypes[0]);
        if (flags & OUTERPARAM_Fxmix) mChannelState.mFxMix = panCalc(fxmix,mParent->mChannelState.mFxMix,weight,mParentCalcTypes[1]);
        if (flags & OUTERPARAM_Dolby) mChannelState.mDolby = panCalc(dolby,mParent->mChannelState.mDolby,weight,mParentCalcTypes[2]);
    }
}
/* 02817A34 */
u32 TTrack::setSeqData(void* data) {
    init();
    mConnectMode = 3;
    mSeqCtrl.start(data, 0);
    updateTrackAll();
    mStatus = 2;
    return 1;
}
/* 02817A98 */
u32 TTrack::tryInterrupt() {
    if (mSeqCtrl.mPreviousFilePtr) return 0;
    void* handler = mIntrMgr.checkIntr();
    if (!handler) return 0;
    return mSeqCtrl.callIntr(handler);
}
/* 02817B00 */
u32 TTrack::checkNoteStop(u32 index) {
    TChannel* channel = mNoteChannels[index];
    return !channel || channel->mStatus == 0;
}
/* 02817B30 */
void TTrack::updateOscParam(u32 index, f32 value) {
    switch (index) {
    case 6: mOscConfigs[0].mScale = value; break;
    case 7: mOscConfigs[0].mRate = value; break;
    case 8: mOscConfigs[0].mOffset = value; break;
    case 9: mOscConfigs[1].mScale = value; break;
    case 10: mOscConfigs[1].mRate = value; break;
    case 11: mOscConfigs[1].mOffset = value; break;
    }
}
/* 02817B98 */
void TTrack::updateTimedParam() {
    for (u32 i = 0; i < TIMED_PARAM_COUNT; ++i) {
        TimedParam& parameter = mTimedParam[i];
        if (parameter.mDuration > 0.0f) {
            parameter.mCurrent = gabi::fadds_ppc(parameter.mCurrent, parameter.mDelta);
            parameter.mDuration = gabi::fsubs_ppc(parameter.mDuration, 1.0f);
            if (i < 6 || i >= 11) mUpdateFlags = mUpdateFlags | (1u << i);
            else updateOscParam(i, parameter.mCurrent);
        }
    }
    mUpdateFlags = mUpdateFlags | 2;
}
/* 0281816C */
void TTrack::updateSeq(u32 flags,u32 force) {
    flags |= mUpdateFlags;
    if (mOuterParam) {
        flags |= mOuterParam->getOuterUpdate();
        mOuterParam->setOuterUpdate(0);
    }
    mVibrate.incCounter();
    mUpdateFlags = 0;
    if (flags) updateTrack(flags);
    for (u32 i = 0; i < MAX_CHILDREN; ++i) {
        TTrack* child = mChildren[i];
        if (!child || !child->mStatus) continue;
        if (force) child->updateSeq(flags,force);
        else child->mUpdateFlags = child->mUpdateFlags | flags;
    }
}
/* 02818254 */
u32 TTrack::noteOff(u32 index,u32 release) {
    TChannel* channel = mNoteChannels[index];
    if (!channel) return 0;
    channel->stop(release);
    mNoteChannels[index] = nullptr;
    return 1;
}
/* 028182D0 */
void TTrack::close() {
    if (!mStatus) return;
    u32 release = mParent ? 0 : 10;
    for (u32 i = 0; i < 8; ++i) noteOff(i,release);
    mStatus = 0;
    for (u32 i = 0; i < MAX_CHILDREN; ++i) {
        TTrack* child = mChildren[i];
        if (child) { child->close(); mChildren[i] = nullptr; }
    }
    mMute = 0;
    JSUPtrLink* link = mChannelList.mHead;
    while (link) {
        TChannel* channel = (TChannel*)(void*)link->mObject;
        JSUPtrLink* next = channel->mLinkState.mNext;
        channel->stop(0);
        channel = (TChannel*)(void*)link->mObject;
        channel->clearCallback();
        channel = (TChannel*)(void*)link->mObject;
        mChannelList.remove((JSUPtrLink*)(void*)channel->mLinkState.mObject);
        link = next;
    }
    if (mAutoDelete) destroyTrack(this,3);
}
/* 02818400 */
s32 TTrack::mainProc() {
    s32 result = 0;
    if (mTempoDirty && mParent) {
        f32 ratio = (f32)mTempo / (f32)mParent->mTempo;
        if (ratio > 1.0f) ratio = 1.0f;
        f32 phase = gabi::fadds_ppc(mTempoScale,ratio);
        if (phase < 1.0f) { mTempoScale = phase; return 0; }
        mTempoScale = gabi::fsubs_ppc(phase,1.0f);
    }
    mIntrMgr.request(7);
    mIntrMgr.timerProcess();
    tryInterrupt();
    if (!mIsPaused || !(mPauseStatus & 2)) {
        s32 wait = mSeqCtrl.mWaitTimer;
        bool parseReady = true;
        if (wait == -1) {
            if (!checkNoteStop(0)) parseReady = false;
            else { wait = 0; mSeqCtrl.mWaitTimer = 0; }
        }
        if (parseReady) {
            s32 savedWait = mSeqCtrl.mSavedWaitTimer;
            if (savedWait > 0) {
                wait = mSeqCtrl.mWaitTimer;
                mSeqCtrl.mSavedWaitTimer = savedWait - 1;
            }
            if (wait > 0) {
                if (!mSeqCtrl.waitCountDown()) parseReady = false;
                else if (mBaseTime != 0xFFFFFFFF && !mConnectCase) mNoteChannels[0] = nullptr;
            }
            if (parseReady) result = parseTrackSequence(&sParser,this);
        }
        updateTimedParam();
    }
    updateSeq(0,0);
    if (result < 0) return -1;
    for (u32 i = 0; i < MAX_CHILDREN; ++i) {
        TTrack* child = mChildren[i];
        if (child && child->mStatus && child->mainProc() == -1) {
            child->close();
            mChildren[i] = nullptr;
        }
    }
    return 0;
}
/* 02818604 */
void TTrack::stopSeqMain() {
    updateSeq(0, 1);
    close();
}
/* 02818640 */
s32 TTrack::rootCallback(TTrack* track) {
    if (!track || !track->mStatus) return -1;
    if (track->mStatus == 3) { track->stopSeqMain(); return -1; }
    f32 phase = gabi::fadds_ppc(track->mTempoScale,track->mTempoFraction);
    if (phase < 1.0f) {
        track->mTempoScale = phase;
        track->updateSeq(0,1);
        return 0;
    }
    do {
        track->mTempoScale = gabi::fsubs_ppc(phase,1.0f);
        if (track->mainProc() == -1) { track->stopSeqMain(); return -1; }
        phase = track->mTempoScale;
    } while (!(phase < 1.0f));
    return 0;
}
/* 0281874C */
u32 TTrack::startSeq() {
    u32 status = mStatus;
    if (status < 2 || status == 3) return 0;
    if (status == 2) mStatus = 1;
    return scheduleTrackRoot(rootCallback,this);
}
/* 02818784 */
u32 TTrack::stopSeq() {
    u32 status = mStatus;
    if (status == 2) {
        mStatus = 0;
        if (mAutoDelete) destroyTrack(this,3);
    } else if (status) {
        mStatus = 3;
    }
    return 1;
}
/* 028187E4 */
s32 TTrack::getTranspose() {
    TTrack* track = this;
    u32 transpose = 0;
    while (track->mParent) {
        transpose += (s32)track->mTranspose;
        track = track->mParent;
    }
    return (s32)(transpose + (s32)track->mTranspose);
}
/* 0281882C */
void TTrack::pause(u32 paused,u32 children) {
    mIsPaused = paused;
    if (paused) {
        if (mPauseStatus & 1) mUpdateFlags = mUpdateFlags | 1;
        if (mPauseStatus & 4) {
            for (u32 i = 0; i < 8; ++i) noteOff(i,10);
        }
        if (mPauseStatus & 8) {
            for (u32 i = 0; i < 8; ++i) {
                TChannel* channel = mNoteChannels[i];
                if (channel) channel->setPauseFlag(1);
            }
        }
    } else {
        mUpdateFlags = mUpdateFlags | 1;
        for (u32 i = 0; i < 8; ++i) {
            TChannel* channel = mNoteChannels[i];
            if (channel) channel->setPauseFlag(0);
        }
    }
    mIntrMgr.request(paused ^ 1);
    if (children) {
        for (u32 i = 0; i < MAX_CHILDREN; ++i) {
            TTrack* child = mChildren[i];
            if (child && child->mStatus) child->pause(paused,1);
        }
    }
}
/* 0281896C */
void TTrack::setTempo(u16 tempo) {
    mTempo = tempo;
    if (!mParent) updateTempo();
    else mTempoDirty = 1;
}
/* 0281898C */
void TTrack::setTimebase(u16 timebase) {
    mTimebase = timebase;
    if (!mParent) updateTempo();
}
/* 028189A0 */
void TTrack::muteTrack(u32 mute) {
    u32 flags = mUpdateFlags;
    mMute = mute;
    mUpdateFlags = flags | 1;
    if ((u8)mute && (mPauseStatus & 32)) {
        for (u32 i = 0; i < 8; ++i) noteOff(i,10);
    }
    for (u32 i = 0; i < MAX_CHILDREN; ++i) {
        TTrack* child = mChildren[i];
        if (child) child->muteTrack(mute);
    }
}
/* 02818A50 */
void TTrack::connectBus(u32 line,u32 config) { mChannelState.mMixConfigs[line] = config; }
/* 02818A60 */
TTrack* TTrack::openChild(u32 index,u32 mode) {
    TTrack* child = mChildren[index];
    if (child) { child->close(); mChildren[index] = nullptr; }
    child = constructTrack(nullptr);
    if (!child) return nullptr;
    child->init();
    child->mParent = this;
    child->mConnectMode = mode;
    child->mAutoDelete = 1;
    u32 route = mRoute;
    child->mRoute = (((route & 0xF0000000) + 0x10000000) | (((route << 4) | index) & 0x0FFFFFFF));
    mChildren[index] = child;
    child->inherit();
    return child;
}
/* 02818B0C */
u32 TTrack::start(void* data, u32 offset) {
    mSeqCtrl.start(data, offset);
    mStatus = 1;
    updateTrackAll();
    return 0;
}
/* 02818B50 */
s32 TTrack::writePortAppDirect(u32 port, u32 value) {
    mTrackPort.writeImport(port, value);
    if (port == 0 || port == 1) mIntrMgr.request(port == 0 ? 3 : 4);
    return 1;
}
/* 02818BBC */
s32 TTrack::readPortAppDirect(u32 port, be<u16>* output) {
    *output = mTrackPort.readExport(port);
    return 1;
}
/* 02818BF4 */
TTrack* TTrack::routeTrack(u32 route) {
    TTrack* track = this;
    u32 depth = route >> 28;
    for (u32 i = 0; i < depth; ++i) {
        track = track->mChildren[route & 15];
        if (!track) return nullptr;
        route >>= 4;
    }
    return track;
}
/* 02818C20 */
s32 TTrack::writePortApp(u32 route, u32 value) {
    TTrack* track = routeTrack(route);
    if (!track) return 0;
    return track->writePortAppDirect((route >> 16) & 0xFF, value);
}
/* 02818C68 */
s32 TTrack::readPortApp(u32 route, be<u16>* output) {
    TTrack* track = routeTrack(route);
    if (!track) return 0;
    return track->readPortAppDirect((route >> 16) & 0xFF, output);
}
/* 02818CB0 */
u16 TTrack::readSelfPort(s32 port) { return mTrackPort.readImport(port); }
/* 02818CB8 */
void TTrack::writeSelfPort(s32 port, u32 value) { mTrackPort.writeExport(port, value); }
/* 02818CC0 */
u32 TTrack::readReg16(u32 target) {
    switch (target) {
    case 0x20: return trackBankNumber(&mRegisterParameters);
    case 0x21: return trackProgramNumber(&mRegisterParameters);
    case 0x22: {
        u32 high = (readReg16(0) << 8) & 0xFF00;
        return high | readReg16(1);
    }
    case 0x2C: {
        u32 result = 0;
        for (s32 i = MAX_CHILDREN - 1; i >= 0; --i) {
            result = (result << 1) & 0xFFFF;
            TTrack* child = mChildren[i];
            if (child && child->mStatus) result |= 1;
        }
        return result;
    }
    case 0x2D: {
        u32 result = 0;
        for (s32 i = 7; i >= 0; --i) result = ((result << 1) & 0xFFFF) | checkNoteStop(i);
        return result;
    }
    case 0x30: {
        u32 index = mSeqCtrl.mLoopIndex;
        return index ? (u32)mSeqCtrl.mLoopTimers[index - 1] : 0;
    }
    default: return ((be<u16>*)mRegisterParameters)[target];
    }
}
/* 02818E28 */
u32 TTrack::readReg32(u32 target) {
    if (target >= 0x28 && target <= 0x2B) return ((be<u32>*)mRegisterParameters)[8 + target - 0x28];
    if (target == 0x23) {
        u32 high = readReg16(4) << 16;
        return high | readReg16(5);
    }
    return readReg16(target);
}
/* 02818EBC */
void TTrack::writeRegDirect(u32 target,u32 value) {
    be<u16>* registers = (be<u16>*)mRegisterParameters;
    if (target <= 2) {
        value &= 0xFF;
        u32 extended = trackExtend8(value);
        registers[target] = value;
        registers[3] = extended;
    } else if (target == 0x20 || target == 0x21) {
        return;
    } else if (target == 0x22) {
        writeRegDirect(0,(value >> 8) & 0xFFFF);
        registers[1] = value & 0xFF;
        registers[3] = value;
    } else {
        registers[target] = value;
        registers[3] = value;
    }
}
/* 02818F84 */
u32 TTrack::loadTbl(u32 offset,u32 index,u32 type) {
    switch (type) {
    case 4: return ((be<u8>*)(void*)mSeqCtrl.mRawFilePtr)[offset + index];
    case 5: return mSeqCtrl.get16(offset + index * 2);
    case 6: return mSeqCtrl.get24(offset + index * 3);
    case 7: return mSeqCtrl.get32(offset + index * 4);
    case 8: return mSeqCtrl.get32(offset + index);
    default: return gabi::ea(this);
    }
}
/* 02819030 */
void TTrack::writeRegParam(u32 command) {
    u32 sourceType;
    u32 operation;
    u32 tableType = 0;
    u32 tableOffset = 0;
    switch (command & 15) {
    case 11: sourceType = 0; operation = 11; break;
    case 10: {
        u32 format = mSeqCtrl.readByte();
        sourceType = format & 12;
        tableType = (format >> 4) + 4;
        operation = 10;
        break;
    }
    case 9: {
        u32 format = mSeqCtrl.readByte();
        sourceType = format & 12;
        operation = format & 240;
        if (sourceType == 8) sourceType = 16;
        break;
    }
    default: sourceType = command & 12; operation = command & 3; break;
    }
    u32 target = mSeqCtrl.readByte();
    if (operation == 10) tableOffset = readReg32(mSeqCtrl.readByte());
    s32 operand;
    switch (sourceType) {
    case 0: operand = (s16)readReg16(mSeqCtrl.readByte()); break;
    case 4: operand = mSeqCtrl.readByte(); break;
    case 8: {
        u32 value = mSeqCtrl.readByte();
        operand = (s16)((value << 8) | ((value & 128) ? 0 : value << 1));
        break;
    }
    case 12: operand = (s16)mSeqCtrl.read16(); break;
    case 16: operand = -1; break;
    default: operand = 0; break;
    }
    // GHS retains the last callee's r3 for flag/address stores in the special-register cases.
    u32 lastResult = readReg16(target);
    s32 current = (s16)lastResult;
    switch (operation) {
    case 0: break;
    case 1:
        if (sourceType == 4) { lastResult = trackExtend8((u8)operand); operand = (s32)lastResult; }
        operand = (s16)((u32)operand + (u32)current);
        break;
    case 2: {
        u32 product = (u32)current * (u32)operand;
        writeRegDirect(4,product >> 16);
        writeRegDirect(5,product & 0xFFFF);
        return;
    }
    case 3:
        ((be<u16>*)mRegisterParameters)[3] = (u32)current - (u32)operand;
        return;
    case 11: operand = (s16)((u32)current - (u32)operand); break;
    case 0x10:
    case 0x20: {
        if (sourceType == 4) { lastResult = trackExtend8((u8)operand); operand = (s32)lastResult; }
        if (operand < 0) {
            u32 shift = (0u - (u32)operand) & 63;
            s32 value = operation == 0x10 ? (u16)current : current;
            operand = shift >= 32 ? (value < 0 ? -1 : 0) : value >> shift;
            if (operation == 0x10) operand = (s16)operand;
        } else {
            u32 shift = (u32)operand & 63;
            operand = (s16)(shift >= 32 ? 0 : (u32)current << shift);
        }
        break;
    }
    case 0x30: operand = (s16)(operand & current); break;
    case 0x40: operand |= current; break;
    case 0x50: operand ^= current; break;
    case 0x60: operand = (s16)(0u - (u32)current); break;
    case 0x90: {
        lastResult = trackRandomS32();
        u32 divisor = (u16)operand;
        // Espresso's zero-divisor quotient is zero, leaving the dividend as the remainder.
        operand = (s16)(divisor ? lastResult % divisor : lastResult);
        break;
    }
    case 10:
        lastResult = loadTbl(tableOffset,(u32)operand,tableType);
        operand = (s16)lastResult;
        break;
    }
    be<u16>* registers = (be<u16>*)mRegisterParameters;
    if (target <= 2) {
        operand &= 0xFF;
        lastResult = trackExtend8((u8)operand);
        registers[target] = operand;
        registers[3] = lastResult;
    } else if (target == 0x21) {
        lastResult = trackBankNumber(&mRegisterParameters);
        registers[3] = lastResult;
        registers[6] = ((lastResult << 8) & 0xFFFFFF00) | ((u32)operand & 0xFF);
        target = 6;
    } else if (target == 0x20) {
        operand = (s32)((u32)operand << 8);
        lastResult = trackProgramNumber(&mRegisterParameters);
        registers[3] = lastResult;
        registers[6] = (u32)operand | lastResult;
        target = 6;
    } else if (target == 0x2E) {
        u32 priority = registers[13];
        registers[3] = lastResult;
        registers[13] = (priority & 0xFF00) | ((u32)operand & 0xFF);
        target = 13;
    } else if (target == 0x2F) {
        u32 priority = registers[13];
        registers[3] = lastResult;
        registers[13] = ((u32)operand << 8) | (priority & 0xFF);
        target = 13;
    } else if (target == 0x22) {
        writeRegDirect(0,((u32)operand >> 8) & 0xFFFF);
        registers[3] = (u32)operand & 0xFF;
        registers[1] = (u32)operand & 0xFF;
        target = 1;
    } else if (target >= 0x28 && target <= 0x2B) {
        ((be<u32>*)mRegisterParameters)[8 + target - 0x28] = lastResult;
        return;
    } else {
        registers[target] = operand;
        registers[3] = (u16)operand;
    }
    if (target == 6) { mOscRoute[0] = 15; mOscRoute[1] = 15; }
    else if (target == 7) mUpdateFlags = mUpdateFlags | 2;
}
/* 02819758 */
u32 TTrack::exchangeRegisterValue(u32 target) {
    if (target < 0x40) return readReg32(target);
    return mTrackPort.mValue[(target - 0x40) & 0xFF];
}
/* 02819778 */
void TTrack::setParam(u32 index, f32 value, s32 duration) {
    TimedParam& parameter = mTimedParam[index];
    parameter.mTarget = value;
    if (duration <= 0) {
        parameter.mCurrent = value;
        parameter.mDelta = 0.0f;
        parameter.mDuration = 1.0f;
    } else {
        f32 frames = (f32)duration;
        parameter.mDelta = gabi::fsubs_ppc(value, parameter.mCurrent) / frames;
        parameter.mDuration = frames;
    }
}
/* 028197F4 */
void TTrack::setInterrupt(u32 interrupt) { mIntrMgr.request(interrupt); }
/* 028197FC */
void TTrack::assignExtBuffer(TOuterParam* outer) { mOuterParam = outer; }
}
/* 02819804 */
void TTrack::registerSeqCallback(u32 callback) { sCallBackFunc = callback; }
/* 02819810 */
void TTrack::newMemPool(s32 count) {
    TTrack* current = (TTrack*)allocateTrackArray(sizeof(TTrack),trackAudioHeap,4);
    TOuterParam* outer = allocateTrackOuter(sizeof(TOuterParam),trackAudioHeap,4);
    if (outer) outer = constructTrackOuter(outer);
    current->assignExtBuffer(outer);
    freeListHead = current;
    if (count > 1) {
        for (s32 i = count - 1; i; --i) {
            current->mPoolNext = (TTrack*)allocateTrackArray(sizeof(TTrack),trackAudioHeap,4);
            outer = allocateTrackOuter(sizeof(TOuterParam),trackAudioHeap,4);
            if (outer) outer = constructTrackOuter(outer);
            current->mPoolNext->assignExtBuffer(outer);
            current = current->mPoolNext;
            freeListTail = current;
        }
    }
    current->mPoolNext = nullptr;
}
/* 02819904 */
void TTrack::overwriteOsc(TChannel* channel) {
    for (u32 slot = 0; slot < 2; ++slot) {
        u32 route = mOscRoute[slot];
        if (route == 15) continue;
        u32 index = route & 3;
        TOscillator::Osc_* config = &mOscConfigs[slot];
        if (route & 8) channel->copyOsc(index,config);
        else if (route & 4) {
            be<s16>* release = config->rel_table;
            channel->copyOsc(index,config);
            config->rel_table = release;
        }
        channel->overwriteOsc(index,config);
    }
}
/* 028199BC */
u32 TTrack::channelCallback(u32 event,TChannel* channel,void* buffer,TTrack* track) {
    switch (event) {
    case 0: return updateTrackChannelDSP(&track->mChannelUpdater,channel,buffer);
    case 1: return initializeTrackChannelDSP(&track->mChannelUpdater,channel,buffer);
    case 2:
        for (u32 i = 0; i < 8; ++i) {
            if ((TChannel*)track->mNoteChannels[i] == channel) { track->mNoteChannels[i] = nullptr; break; }
        }
        return track->mChannelList.remove(&channel->mLinkState);
    case 3: return stopTrackChannel(channel,0);
    default: return gabi::ea(track);
    }
}
/* 02819A44 */
s32 TTrack::noteOn(u32 index,s32 key,s32 velocity,u32 gateTime,u32 skipSamples) {
    if (mMute && (mPauseStatus & 64)) return -1;
    // HD: a per-note inhibit mask can reject allocation before releasing the old note.
    u32 noteBit = (index & 63) < 32 ? 1u << (index & 31) : 0;
    if (mParameterMode & noteBit) return -1;
    noteOff(index,0);
    u32 bank = (u8)mapTrackBank(trackBankNumber(&mRegisterParameters));
    u32 program = trackProgramNumber(&mRegisterParameters);
    u32 allocator = mChannelState.mFlags;
    u32 priority = ((be<u16>*)mRegisterParameters)[13];
    TChannel* channel;
    if (!allocator) channel = allocateTrackNote(bank,program,(u8)key,(u8)velocity,priority,channelCallback,this);
    else channel = gabi::call_ptr<TChannel*>(allocator,this,bank,program,(u8)key,(u8)velocity,priority);
    if (!channel) return -1;
    channel->mGateTime = gateTime;
    mChannelList.append(&channel->mLinkState);
    mNoteChannels[index] = channel;
    channel->mSkipSamples = skipSamples;
    be<u16>* registers = (be<u16>*)mRegisterParameters;
    f32 sound = (u16)registers[8];
    f32 effect = (u16)registers[9];
    f32 oscillator = (u16)registers[10];
    channel->setPanPower(sound,effect,oscillator);
    overwriteOsc(channel);
    u32 release = mTempoAccumulator;
    if (release) channel->directReleaseOsc(release);
    return 0;
}
/* 02819BEC */
s32 TTrack::gateOn(u32 index,s32 key,s32 velocity,u32 gateTime) {
    TChannel* channel = mNoteChannels[index];
    if (!channel) return -1;
    gateTrackNote(channel,(u8)key,(u8)velocity);
    channel->mGateTime = gateTime;
    return 0;
}
/* 02819C64 */
void TTrack::oscSetupFull(u32 flags,u32 attackOffset,u32 releaseOffset) {
    u32 slot = (flags >> 4) & 1;
    u32 target = flags & 15;
    if (flags & 128) {
        for (u32 word = 0; word < 6; ++word) mOscConfigWords[slot * 6 + word] = (u32)defaultEnvelope.mWords[word];
        mOscConfigWords[slot * 6] = target;
        if (target == 1) mOscConfigs[slot].mOffset = 1.0f;
    }
    if (flags & 64) mOscConfigs[slot].table = (be<s16>*)((be<u8>*)(void*)mSeqCtrl.mRawFilePtr + attackOffset);
    if (flags & 32) mOscConfigs[slot].rel_table = (be<s16>*)((be<u8>*)(void*)mSeqCtrl.mRawFilePtr + releaseOffset);
}
/* 02819D48 */
void TTrack::oscSetupSimpleEnv(u32 type,u32 offset) {
    if (type == 0) {
        for (u32 word = 0; word < 6; ++word) mOscConfigWords[word] = (u32)defaultEnvelope.mWords[word];
        mOscConfigs[0].table = (be<s16>*)((be<u8>*)(void*)mSeqCtrl.mRawFilePtr + offset);
    } else if (type == 1) {
        mOscConfigs[0].rel_table = (be<s16>*)((be<u8>*)(void*)mSeqCtrl.mRawFilePtr + offset);
    }
}
/* 02819DAC */
void TTrack::oscSetupSimple(u32 type) {
    u32 slot;
    const TrackOscillatorWords* config;
    if (type == 0) { slot = 1; config = &defaultVibrato; }
    else if (type == 1) { slot = 0; config = &defaultTremolo; }
    else if (type == 2) { slot = 1; config = &defaultTremolo; }
    else return;
    for (u32 word = 0; word < 6; ++word) mOscConfigWords[slot * 6 + word] = (u32)config->mWords[word];
}
/* 02819E6C */
s32 TTrack::seqTimeToDspTime(s32 time,u32 scale) {
    f32 duration = gabi::fmuls_ppc((f32)time,(f32)scale) / 100.0f;
    if (mEnabled) duration = duration / mTempoFraction;
    else {
        // HD: divide the nominal tempo by the timebase before multiplying.
        duration = gabi::fmuls_ppc(duration,120.0f / (f32)mTimebase);
        u32 subframes = trackSubframes();
        duration = gabi::fmuls_ppc(duration,(f32)subframes / 10.0f);
    }
    return gabi::ftoi(duration);
}
/* HD: compiler-generated initialization of this translation unit. */
/* 02819F94 */
void __sinit_JASTrack_cpp() {
    headerStatics.mWords[2] = 0;
    headerStatics.mWords[0] = 0;
    headerStatics.mWords[3] = 0;
    headerStatics.mWords[1] = 0;
    registerTrackStatics(&destructorRecord);
}
/* WWHD glue
data sParser 104B568C
data trackAudioHeap 101F9DA8
data freeListHead 101F9FC4
data freeListTail 101F9FC8
data sAdsTable 101FC290
data defaultEnvelope 1017A77C
data defaultVibrato 1017A794
data defaultTremolo 1017A7AC
data headerStatics 104B567C
data destructorRecord 101F9FB8
ext 028458D4 void JASystem::TSeqCtrl::init()
ext 028452E8 void initializeTrackChannels(void*)
ext 028457C0 void initializeTrackRegisters(void*)
ext 02845840 void inheritTrackRegisters(void*,const void*)
ext 02816C58 void TOuterParam::initExtBuffer()
ext 02845910 void JASystem::TSeqCtrl::start(void*, u32)
ext 02845A94 u32 JASystem::TSeqCtrl::callIntr(void*)
ext 02845A6C u32 JASystem::TSeqCtrl::waitCountDown()
ext 028463B4 s32 parseTrackSequence(TrackParser*,JASystem::TTrack*)
ext 02845920 u32 JASystem::TSeqCtrl::read16()
ext 02845758 u32 trackRandomS32()
ext 02845984 u32 JASystem::TSeqCtrl::get16(u32) const
ext 028459A0 u32 JASystem::TSeqCtrl::get24(u32) const
ext 028459CC u32 JASystem::TSeqCtrl::get32(u32) const
ext 02845828 u32 trackBankNumber(void*)
ext 02845834 u32 trackProgramNumber(void*)
ext 0284565C u32 trackExtend8(u32)
ext 028155FC u32 scheduleTrackRoot(TrackRootCallback,JASystem::TTrack*)
ext 02816C8C u32 TOuterParam::getOuterUpdate()
ext 02816C84 void TOuterParam::setOuterUpdate(u32)
ext 02845564 u32 updateTrackChannelDSP(void*,JASystem::TChannel*,void*)
ext 028453F8 u32 initializeTrackChannelDSP(void*,JASystem::TChannel*,void*)
ext 02815330 u32 stopTrackChannel(JASystem::TChannel*,u32)
ext 02810FF0 u32 mapTrackBank(u32)
ext 02811064 JASystem::TChannel* allocateTrackNote(u32,u32,u32,u32,u32,TrackChannelCallback,JASystem::TTrack*)
ext 02811438 void gateTrackNote(JASystem::TChannel*,u32,u32)
ext 027EC6F4 u32 JSUPtrList::append(JSUPtrLink*)
ext 02813AC8 u32 trackSubframes()
ext 027EC678 void JSUPtrList::initiate()
ext 027EC560 u32 JSUPtrList::remove(JSUPtrLink*)
ext 027EC68C void destroyTrackList(JSUPtrList*,u32)
ext 02816E00 void constructTrackVibrate(JASystem::TVibrate*)
ext 028453C0 void constructTrackChannels(void*)
ext 0273B0D4 void* allocateTrackArray(u32,void*,u32)
ext 0273B050 TOuterParam* allocateTrackOuter(u32,void*,u32)
ext 02816BE0 TOuterParam* constructTrackOuter(TOuterParam*)
ext 028F026C void registerTrackStatics(TrackDestructorRecord*)
ext 02813AD4 f32 trackUpdateInterval()
ext 02816C70 u32 TOuterParam::checkOuterSwitch(u32)
ext 02816C94 s32 TOuterParam::getIntFirFilter(u32)
*/

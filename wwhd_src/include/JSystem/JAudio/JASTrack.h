/* JAudio 1 track; Cafe layout checked against the track constructor and field accessors. */
#pragma once
#include "bindings.h"
#include "JSystem/JSupport/JSUList.h"
#include "JSystem/JAudio/JASTrackPort.h"
#include "JSystem/JAudio/JASTrackInterrupt.h"
#include "JSystem/JAudio/JASVibrate.h"
#include "JSystem/JAudio/JASOuterParam.h"
#include "JSystem/JAudio/JASOscillator.h"
#include "JSystem/JAudio/JASSeqCtrl.h"
namespace JASystem {
class TChannel;
namespace DSPInterface { class DSPBuffer; }
/* Cafe channel updater: 0x50 bytes allocated by 028453C0; the same layout is the first 0x50 bytes of
 * TTrack::ChannelState (which adds the enclosing track's mFlags at 0x50)., JS-JAudio#5. */
struct TChannelUpdater {
    be<f32> mVolume;
    be<f32> mPitch;
    be<f32> mPan;
    be<f32> mFxMix;
    be<f32> mDolby;
    be<s16> mFir[8];
    be<s16> mIir[4];
    u8 mReservedFilter[8];
    be<s16> mFilterMix;
    be<u16> mMixConfigs[6];
    be<s8> mDelay[6];
    be<u8> mMixerMode;
    be<u8> mFilterFlags;
    be<u8> mCalcTypes[3];
    u8 mReservedCalc[3];
    void init();
    void updateJcToDSPInit(TChannel* channel, DSPInterface::DSPBuffer* buffer);
    void updateJcToDSP(TChannel* channel, DSPInterface::DSPBuffer* buffer);
};
class TTrack {
public:
    enum { MAX_CHILDREN = 16, TIMED_PARAM_COUNT = 18 };
    struct TimedParam {
        be<f32> mCurrent;
        be<f32> mTarget;
        be<f32> mDuration;
        be<f32> mDelta;
    };
    void setInterrupt(u32 interrupt);
    static u32 channelCallback(u32 event,TChannel* channel,void* buffer,TTrack* track);
    s32 noteOn(u32 index,s32 key,s32 velocity,u32 gateTime,u32 skipSamples);
    s32 gateOn(u32 index,s32 key,s32 velocity,u32 gateTime);
    TTrack* openChild(u32 index,u32 mode);
    static void newMemPool(s32 count);
    void updateTrack(u32 flags);
    s32 mainProc();
    static s32 rootCallback(TTrack* track);
    u32 startSeq();
    void overwriteOsc(TChannel* channel);
    void oscSetupFull(u32 flags,u32 attackOffset,u32 releaseOffset);
    void oscSetupSimpleEnv(u32 type,u32 offset);
    void oscSetupSimple(u32 type);
    s32 seqTimeToDspTime(s32 time,u32 scale);
    static void registerSeqCallback(u32 callback);
    s32 getTranspose();
    void pause(u32 paused,u32 children);
    void muteTrack(u32 mute);
    void connectBus(u32 line,u32 config);
    u32 stopSeq();
    u32 noteOff(u32 index,u32 release);
    f32 panCalc(f32 a,f32 b,f32 weight,u32 type);
    void init();
    void inherit();
    void updateTrackAll();
    u32 setSeqData(void* data);
    u32 start(void* data, u32 offset);
    u32 tryInterrupt();
    u32 checkNoteStop(u32 index);
    s32 writePortAppDirect(u32 port, u32 value);
    s32 writePortApp(u32 route, u32 value);
    s32 readPortApp(u32 route, be<u16>* output);
    void initTimed();
    void updateTimedParam();
    void updateOscParam(u32 index, f32 value);
    void stopSeqMain();
    void updateSeq(u32 flags, u32 force);
    void close();
    void updateTempo();
    void setTempo(u16 tempo);
    void setTimebase(u16 timebase);
    s32 readPortAppDirect(u32 port, be<u16>* output);
    TTrack* routeTrack(u32 route);
    u16 readSelfPort(s32 port);
    void writeSelfPort(s32 port, u32 value);
    u32 readReg32(u32 target);
    u32 readReg16(u32 target);
    void writeRegDirect(u32 target,u32 value);
    void writeRegParam(u32 command);
    u32 loadTbl(u32 offset,u32 index,u32 type);
    u32 exchangeRegisterValue(u32 target);
    void assignExtBuffer(TOuterParam* outer);
    void setParam(u32 index, f32 value, s32 duration);
    union {
        u8 mSequenceState[0x54];
        struct { union { u8 mTrackList[0xC]; JSUPtrList mChannelList; gptr<TTrack> mPoolNext; }; TSeqCtrl mSeqCtrl; };
    };
    TTrackPort mTrackPort;
    TIntrMgr mIntrMgr;
    union {
        u8 mNoteState[0x28];
        struct {
            gptr<TChannel> mNoteChannels[8];
            union {
                u8 mNoteControl[8];
                struct {
                    be<u32> mBaseTime;
                    be<u8> mConnectCase;
                    be<u8> mLastNote;
                    be<u8> mBeforeTieMode;
                    u8 mNotePadding;
                };
            };
        };
    };
    TVibrate mVibrate;
    struct ChannelState : TChannelUpdater {
        be<u32> mFlags;  // enclosing track only
    };
    union { u8 mChannelUpdater[0x54]; ChannelState mChannelState; };
    TimedParam mTimedParam[TIMED_PARAM_COUNT];
    struct RegisterParam {
        be<u16> mGeneralRegisters[3];
        be<u16> mFlag;
        be<u16> mControlRegisters[4];
        be<u16> mPanPower[5];
        u8 mRemainingState[0x26];
    };
    union {
        u8 mRegisterState[0x90];
        struct {
            union { u8 mRegisterParameters[0x40]; RegisterParam mRegisterParam; };
            union { TOscillator::Osc_ mOscConfigs[2]; be<u32> mOscConfigWords[12]; };
            be<u32> mOscRoute[2];
            be<s16> mFilterCoefficients[12];
        };
    };
    gptr<TTrack> mParent;
    gptr<TTrack> mChildren[MAX_CHILDREN];
    gptr<TOuterParam> mOuterParam;
    be<f32> mTempoScale;
    be<f32> mTempoFraction;
    be<u32> mRoute;
    be<u32> mUpdateFlags;
    be<u16> mTempoAccumulator;
    be<u16> mTempo;
    be<u16> mTimebase;
    be<s8> mTranspose;
    be<u8> mConnectMode;
    be<u8> mPauseStatus;
    be<u8> mVolumeMode;
    be<u8> mParameterMode;
    be<u8> mStatus;
    be<u8> mCalcTypes[3];
    be<u8> mParentCalcTypes[3];
    be<u8> mIsPaused;
    be<u8> mMute;
    union { be<u8> mEnabled; be<u8> mTimeRelate; };
    be<u8> mTempoDirty;
    be<u8> mAutoDelete;
    u8 mTailPadding;
};
WWHD_OFFSET(TTrack,mSeqCtrl,0xC);
WWHD_OFFSET(TTrack,mTrackPort,0x54);
WWHD_OFFSET(TTrack,mIntrMgr,0x94);
WWHD_OFFSET(TTrack,mNoteChannels,0xC0);
WWHD_OFFSET(TTrack,mVibrate,0xE8);
WWHD_OFFSET(TTrack,mChannelUpdater,0xF4);
WWHD_OFFSET(TTrack,mTimedParam,0x148);
WWHD_OFFSET(TTrack,mOscConfigs,0x2A8);
WWHD_OFFSET(TTrack,mOscRoute,0x2D8);
WWHD_OFFSET(TTrack,mFilterCoefficients,0x2E0);
WWHD_OFFSET(TTrack,mParent,0x2F8);
WWHD_OFFSET(TTrack,mChildren,0x2FC);
WWHD_OFFSET(TTrack,mOuterParam,0x33C);
WWHD_OFFSET(TTrack,mStatus,0x35B);
WWHD_SIZE(TTrack,0x368);
WWHD_SIZE(TTrack::TimedParam,0x10);
WWHD_SIZE(TTrack::ChannelState,0x54);
WWHD_SIZE(TChannelUpdater,0x50);
WWHD_OFFSET(TChannelUpdater,mFilterMix,0x34);
WWHD_OFFSET(TChannelUpdater,mMixerMode,0x48);
WWHD_OFFSET(TChannelUpdater,mCalcTypes,0x4A);
WWHD_OFFSET(TTrack::ChannelState,mFir,0x14);
WWHD_OFFSET(TTrack::ChannelState,mIir,0x24);
WWHD_OFFSET(TTrack::ChannelState,mFilterMix,0x34);
WWHD_OFFSET(TTrack::ChannelState,mDelay,0x42);
WWHD_OFFSET(TTrack::ChannelState,mFilterFlags,0x49);
WWHD_OFFSET(TTrack::ChannelState,mCalcTypes,0x4A);
WWHD_OFFSET(TTrack::ChannelState,mFlags,0x50);
}

struct TrackHeaderStatics { be<u32> mWords[4]; };
struct TrackDestructorRecord { be<u32> mWords[3]; };
void registerTrackStatics(TrackDestructorRecord*);
f32 trackUpdateInterval();
struct TrackOscillatorWords { be<u32> mWords[6]; };
void initializeTrackChannels(void* channels);
void initializeTrackRegisters(void* registers);
void inheritTrackRegisters(void* registers,const void* parentRegisters);
void destroyTrack(JASystem::TTrack* track,u32 flags);
u32 trackSubframes();
u32 trackBankNumber(void* registers);
u32 trackProgramNumber(void* registers);
u32 trackExtend8(u32 value);
u32 trackRandomS32();
JASystem::TTrack* constructTrack(JASystem::TTrack* track);
void constructTrackVibrate(JASystem::TVibrate* vibrate);
void constructTrackChannels(void* channels);
void destroyTrackList(JSUPtrList* list,u32 flags);
using TrackRootCallback = s32(*)(JASystem::TTrack*);
u32 scheduleTrackRoot(TrackRootCallback callback,JASystem::TTrack* track);
void* allocateTrackArray(u32 size,void* heap,u32 alignment);
TOuterParam* allocateTrackOuter(u32 size,void* heap,u32 alignment);
TOuterParam* constructTrackOuter(TOuterParam* outer);
using TrackChannelCallback = u32(*)(u32,JASystem::TChannel*,void*,JASystem::TTrack*);
u32 updateTrackChannelDSP(void* channels,JASystem::TChannel* channel,void* buffer);
u32 initializeTrackChannelDSP(void* channels,JASystem::TChannel* channel,void* buffer);
u32 stopTrackChannel(JASystem::TChannel* channel,u32 release);
u32 mapTrackBank(u32 bank);
JASystem::TChannel* allocateTrackNote(u32 bank,u32 program,u32 key,u32 velocity,u32 priority,TrackChannelCallback callback,JASystem::TTrack* track);
void gateTrackNote(JASystem::TChannel* channel,u32 key,u32 velocity);
struct TrackParser { u8 mStorage; };
s32 parseTrackSequence(TrackParser* parser,JASystem::TTrack* track);
// Sequence sync callback (data 101F9FCC): a guest word holding the address of a TrackSeqCallback, or 0.
// Written by TTrack::registerSeqCallback; TSeqParser::cmdSyncCPU (02846EA8) calls it with (track, u16 command
// argument) and stores the u16 result at track+0x26E, or 0xFFFF when no callback is registered.
using TrackSeqCallback = u16(*)(JASystem::TTrack*, u16);
extern be<u32>& sCallBackFunc;

WWHD_OFFSET(JASystem::TTrack::RegisterParam, mFlag, 6);
WWHD_OFFSET(JASystem::TTrack::RegisterParam, mPanPower, 0x10);
WWHD_SIZE(JASystem::TTrack::RegisterParam, 0x40);
WWHD_OFFSET(JASystem::TTrack, mRegisterParam, 0x268);
WWHD_OFFSET(JASystem::TTrack, mTimeRelate, 0x364);

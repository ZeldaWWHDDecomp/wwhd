/* JAudio logical channel. HD embeds four oscillators and moves the link to the head. */
#pragma once
#include "bindings.h"
#include "JSystem/JSupport/JSUList.h"
#include "JSystem/JAudio/JASOscillator.h"
namespace JASystem {
namespace DSPInterface { class DSPBuffer; }
class TDSPChannel;
struct ChannelWaveKey { be<u8> mFormat; be<u8> mKey; };
WWHD_OFFSET(ChannelWaveKey, mKey, 1);
WWHD_SIZE(ChannelWaveKey, 2);
namespace Driver {
struct PanMatrix_ {
    be<f32> mSound;
    be<f32> mEffect;
    be<f32> mChannel;
};
}
struct ChannelVtable { u8 mPrefix[12]; gfn mDestructor; };
struct ChannelMixer { be<u16> mValues[6]; };
WWHD_SIZE(ChannelMixer,12);
struct ChannelEffects {
    be<f32> mVolume;
    be<f32> mPitch;
    be<f32> mPan;
    be<f32> mFxMix;
    be<f32> mDolby;
};
WWHD_OFFSET(ChannelEffects,mPitch,4);
WWHD_OFFSET(ChannelEffects,mPan,8);
WWHD_OFFSET(ChannelEffects,mFxMix,12);
WWHD_OFFSET(ChannelEffects,mDolby,16);
WWHD_SIZE(ChannelEffects,20);
class TChannel {
public:
    enum CalcType { CALC_None = 0, CALC_AddChannelOnly = 1, CALC_AddAll = 13, CALC_WeightAll = 26 };
    static f32 calcEffect(const Driver::PanMatrix_* values, const Driver::PanMatrix_* weights, u32 type);
    static f32 calcPan(const Driver::PanMatrix_* values, const Driver::PanMatrix_* weights, u32 type);
    u32 play();
    static s32 dspCallback(u32 reason,DSPInterface::DSPBuffer* buffer,TChannel* channel);
    s32 updateJcToDSP(DSPInterface::DSPBuffer* buffer);
    void playLogicalChannel(DSPInterface::DSPBuffer* buffer);
    void updateMixer(be<u16>* output,f32 volume,f32 pan,f32 fxMix,f32 dolby);
    void updateEffectorParam(void* buffer,be<u16>* output,ChannelEffects* effects);
    void extraUpdate();
    void updateAutoMixer(void* buffer,f32 volume,f32 pan,f32 fxMix,f32 dolby);
    void effectOsc(u32 index,ChannelEffects* effects);
    void clearCallback();
    void stop(u32 release);
    void directReleaseOsc(u32 release);
    void setPanPower(f32 sound, f32 effect, f32 channel);
    void setKeySweepTarget(s32 key, u32 target);
    void setPauseFlag(u8 pause);
    void setMixConfig(u32 index, u16 config);
    void setOscInit(u32 index, const TOscillator::Osc_* config);
    void copyOsc(u32 index, TOscillator::Osc_* config);
    void overwriteOsc(u32 index, const TOscillator::Osc_* config);
    union { u8 mLink[0x10]; JSUPtrLink mLinkState; };
    be<u32> mBaseVtable;
    be<u32> mStatus;
    be<u8> mPauseFlag;
    u8 mReservedPause[3];
    gptr<void> mDSPChannel;
    be<u32> mCallback;
    gptr<void> mCallbackData;
    be<u32> mGateTime;
    TOscillator mOscillators[4];
    be<u16> mMixConfigs[6];
    be<u16> mPriority;
    be<u16> mRelease;
    be<f32> mSweepTarget;
    be<u32> mSweepRemaining;
    be<u32> mSkipSamples;
    be<f32> mPanBase;
    be<f32> mPanEffect;
    be<f32> mFxMixBase;
    be<f32> mFxMixEffect;
    be<f32> mDolbyBase;
    be<f32> mDolbyEffect;
    be<u8> mWaveType;
    u8 mReservedWave[3];
    gptr<void> mWave;
    be<u32> mWaveStart;
    be<f32> mBasePitch;
    be<f32> mBaseVolume;
    be<f32> mPitch;
    be<f32> mVolume;
    be<f32> mVolumeScale;
    be<f32> mPitchScale;
    be<u8> mCalcTypes[3];
    u8 mReservedCalc;
    Driver::PanMatrix_ mPanPower;
    gptr<ChannelVtable> mVtable;
};
WWHD_OFFSET(TChannel,mOscillators,0x2C);
WWHD_OFFSET(TChannel,mMixConfigs,0xAC);
WWHD_OFFSET(TChannel,mPriority,0xB8);
WWHD_OFFSET(TChannel,mSweepTarget,0xBC);
WWHD_OFFSET(TChannel,mWaveType,0xE0);
WWHD_OFFSET(TChannel,mBasePitch,0xEC);
WWHD_OFFSET(TChannel,mPitch,0xF4);
WWHD_OFFSET(TChannel,mCalcTypes,0x104);
WWHD_OFFSET(TChannel,mPanPower,0x108);
WWHD_OFFSET(TChannel,mVtable,0x114);
WWHD_SIZE(TChannel,0x118);
}
f32 channelKeyToPitch(s32 key);
JASystem::TOscillator* constructChannelOscillator(JASystem::TOscillator* oscillator);
void* allocateChannelObject(u32 size);
struct ChannelHeaderStatics { be<u32> mWords[4]; };
struct ChannelDestructorRecord { be<u32> mWords[3]; };
void registerChannelStatics(ChannelDestructorRecord*);

void destroyChannelBase(void* object,u32 flags);
void channelBaseNoOp();
JASystem::TChannel* constructChannel(JASystem::TChannel* channel,u32 callback,void* data);
void destroyChannel(JASystem::TChannel* channel,u32 flags);
u32 disableChannelPoolInterrupts();
void restoreChannelPoolInterrupts(u32 state);
void* allocateChannelPool(u32 size,void* heap,u32 alignment);
void constructChannelPool(void* pool);
JASystem::TChannel* popChannelPool(void* pool,u32 size);
void pushChannelPool(void* pool,JASystem::TChannel* channel,u32 size);
void constructChannelLink(JSUPtrLink* link,JASystem::TChannel* channel);
void destroyChannelLink(JSUPtrLink* link,u32 flags);
void detachChannelDSP(void* channel);
using ChannelOscillatorConstructor = JASystem::TOscillator*(*)(JASystem::TOscillator*);
void constructChannelOscillators(void* array,u32 count,u32 stride,ChannelOscillatorConstructor constructor);
void freeChannelObject(void* object);


f64 channelMixerSin(f64 value);

void stopChannelDSP(void* channel);
void freeChannelDSP(void* channel);

using ChannelDSPCallback = s32(*)(u32,JASystem::DSPInterface::DSPBuffer*,JASystem::TChannel*);
JASystem::TDSPChannel* allocateChannelDSP(u32 priority,ChannelDSPCallback callback,JASystem::TChannel* channel);

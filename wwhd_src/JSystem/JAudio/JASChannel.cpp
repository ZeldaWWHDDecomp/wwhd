/* Based on CC0 tww JASChannel.cpp; Cafe embeds its oscillator array. */
#include "JSystem/JAudio/JASChannel.h"
#include "JSystem/JAudio/JASDSPChannel.h"
#include "JSystem/JAudio/JASDriverIF.h"
#include "JSystem/JAudio/JASDSPInterface.h"
using namespace JASystem;
static ChannelHeaderStatics headerStatics;
static ChannelDestructorRecord destructorRecord;
static f32 C5BASE_PITCHTABLE[128] = {
    0.031250f, 0.033108f, 0.035077f, 0.037162f,
    0.039372f, 0.041713f, 0.044194f, 0.046822f,
    0.049606f, 0.052556f, 0.055681f, 0.058992f,
    0.062500f, 0.066216f, 0.070154f, 0.074325f,
    0.078745f, 0.083427f, 0.088388f, 0.093644f,
    0.099213f, 0.105112f, 0.111362f, 0.117984f,
    0.125000f, 0.132433f, 0.140308f, 0.148651f,
    0.157490f, 0.166855f, 0.176777f, 0.187288f,
    0.198425f, 0.210224f, 0.222725f, 0.235969f,
    0.250000f, 0.264866f, 0.280616f, 0.297302f,
    0.314980f, 0.333710f, 0.353553f, 0.374577f,
    0.396850f, 0.420448f, 0.445449f, 0.471937f,
    0.500000f, 0.529732f, 0.561231f, 0.594604f,
    0.629961f, 0.667420f, 0.707107f, 0.749154f,
    0.793701f, 0.840897f, 0.890899f, 0.943875f,
    1.000000f, 1.059463f, 1.122462f, 1.189207f,
    1.259921f, 1.334840f, 1.414214f, 1.498307f,
    1.587401f, 1.681793f, 1.781798f, 1.887749f,
    2.000000f, 2.118926f, 2.244924f, 2.378414f,
    2.519842f, 2.669680f, 2.828428f, 2.996615f,
    3.174803f, 3.363586f, 3.563596f, 3.775498f,
    4.000000f, 4.237853f, 4.489849f, 4.756829f,
    5.039685f, 5.339360f, 5.656855f, 5.993229f,
    6.349606f, 6.727173f, 7.127192f, 7.550996f,
    8.000000f, 8.475705f, 8.979697f, 9.513658f,
    10.079370f, 10.678720f, 11.313710f, 11.986459f,
    12.699211f, 13.454346f, 14.254383f, 15.101993f,
    16.000000f, 16.951410f, 17.959394f, 19.027315f,
    20.158739f, 21.357441f, 22.627420f, 23.972918f,
    25.398422f, 26.908691f, 28.508766f, 30.203985f,
    32.000000f, 33.902821f, 35.918789f, 38.054630f,
    40.317478f, 42.714882f, 45.254840f, 47.945835f,
};
static ChannelVtable channelVtable;
static ChannelHeaderStatics channelBaseVtable;
using ChannelPoolPointer = gptr<void>;
static ChannelPoolPointer channelPool;
static ChannelPoolPointer channelHeap;
static void ensureChannelPool() {
    if (channelPool) return;
    u32 state = disableChannelPoolInterrupts();
    if (!channelPool) {
        void* pool = allocateChannelPool(12,channelHeap,4);
        if (pool) constructChannelPool(pool);
        channelPool = pool;
    }
    restoreChannelPoolInterrupts(state);
}
/* 02813F04 */
void destroyChannelBase(void* object,u32 flags) {
    if (object && (flags & 1)) freeChannelObject(object);
}
/* 02813F18 */
void channelBaseNoOp() {}
/* 02813F1C */
void destroyChannel(TChannel* channel,u32 flags) {
    if (!channel) return;
    channel->mVtable = &channelVtable;
    channel->mBaseVtable = gabi::ea(&channelBaseVtable);
    if (channel->mDSPChannel) detachChannelDSP(channel->mDSPChannel);
    u32 callback = channel->mCallback;
    if (callback) gabi::call_ptr<void>(callback,2,channel,nullptr,(void*)channel->mCallbackData);
    destroyChannelLink(&channel->mLinkState,0);
    if (flags & 1) {
        ensureChannelPool();
        pushChannelPool(channelPool,channel,0x118);
    }
}
/* 02814028 */
TChannel* constructChannel(TChannel* channel,u32 callback,void* data) {
    if (!channel) {
        ensureChannelPool();
        channel = popChannelPool(channelPool,0x118);
        if (!channel) return nullptr;
    }
    constructChannelLink(&channel->mLinkState,channel);
    channel->mCallback = callback;
    channel->mGateTime = 0;
    channel->mDSPChannel = nullptr;
    channel->mBaseVtable = gabi::ea(&channelBaseVtable);
    channel->mCallbackData = data;
    channel->mVtable = &channelVtable;
    channel->mStatus = 0;
    constructChannelOscillators(&channel->mOscillators,4,0x20,constructChannelOscillator);
    channel->mWave = nullptr;
    channel->mPitch = 1.0f;
    channel->mFxMixBase = 0.0f;
    channel->mDolbyEffect = 0.0f;
    channel->mDolbyBase = 0.0f;
    channel->mSweepRemaining = 0;
    channel->mWaveType = 0;
    channel->mSweepTarget = 1.0f;
    channel->mMixConfigs[5] = 0;
    channel->mMixConfigs[2] = 0x352;
    channel->mCalcTypes[1] = 1;
    channel->mPriority = 0x13F;
    channel->mCalcTypes[2] = 1;
    channel->mCalcTypes[0] = 26;
    channel->mPanEffect = 0.5f;
    channel->mMixConfigs[3] = 0x412;
    channel->mMixConfigs[4] = 0;
    channel->mPanBase = 0.5f;
    channel->mVolume = 1.0f;
    channel->mSkipSamples = 0;
    channel->mMixConfigs[0] = 0x150;
    channel->mMixConfigs[1] = 0x210;
    channel->mWaveStart = 0;
    channel->mFxMixEffect = 0.0f;
    for (u32 i = 0; i < 4; ++i) channel->mOscillators[i].init();
    channel->mPitchScale = 1.0f;
    channel->mVolumeScale = 1.0f;
    channel->mPauseFlag = 0;
    return channel;
}
static u8 calc_sw_table[27][3] = {
    
    { 0, 0, 0 },       
    { 0, 0, 1 },        
    { 0, 0, 1 },        
    { 0, 1, 0 },        
    { 0, 1, 1 },         
    { 0, 1, 2 },      
    { 0, 1, 0 },        
    { 0, 2, 1 },      
    { 0, 2, 2 },   
    { 1, 0, 0 },        
    { 1, 0, 1 },         
    { 1, 0, 2 },      
    { 1, 1, 0 },         
    { 1, 1, 1 },          
    { 1, 1, 2 },       
    { 1, 2, 0 },      
    { 1, 2, 1 },       
    { 1, 2, 2 },    
    { 1, 0, 0 },        
    { 2, 0, 1 },      
    { 2, 0, 2 },   
    { 2, 1, 0 },      
    { 2, 1, 1 },       
    { 2, 1, 2 },    
    { 2, 2, 0 },   
    { 2, 2, 1 },    
    { 2, 2, 2 }, 
};
namespace JASystem {
/* 02814204 */
void TChannel::clearCallback() { mCallback = 0; mCallbackData = nullptr; }
/* 02814214 */
void TChannel::effectOsc(u32 index,ChannelEffects* effects) {
    f32 value = mOscillators[index].getValue();
    const TOscillator::Osc_* config = mOscillators[index].mOsc;
    // HD: effector addition is left unclamped.
    switch ((u32)config->mTargetWord) {
    case 0: effects->mVolume = gabi::fmuls_ppc(effects->mVolume,value); break;
    case 1: effects->mPitch = gabi::fmuls_ppc(effects->mPitch,value); break;
    case 2: effects->mPan = gabi::fadds_ppc(effects->mPan,gabi::fsubs_ppc(value,0.5f)); break;
    case 3: effects->mFxMix = gabi::fadds_ppc(effects->mFxMix,value); break;
    case 4: effects->mDolby = gabi::fadds_ppc(effects->mDolby,value); break;
    }
}
/* 02814364 */
f32 TChannel::calcEffect(const Driver::PanMatrix_* values, const Driver::PanMatrix_* weights, u32 type) {
    f32 result = 0.0f;
    if (calc_sw_table[type][0] == 1) result = values->mSound;
    else if (calc_sw_table[type][0] == 2) result = gabi::fmuls_ppc(values->mSound, weights->mSound);
    if (calc_sw_table[type][1] == 1) result = gabi::fadds_ppc(result, values->mEffect);
    else if (calc_sw_table[type][1] == 2) result = gabi::fmadds(values->mEffect, weights->mEffect, result);
    if (calc_sw_table[type][2] == 1) result = gabi::fadds_ppc(result, values->mChannel);
    else if (calc_sw_table[type][2] == 2) result = gabi::fmadds(values->mChannel, weights->mChannel, result);
    return result;
}
/* 0281442C */
f32 TChannel::calcPan(const Driver::PanMatrix_* values, const Driver::PanMatrix_* weights, u32 type) {
    f32 result = 0.0f;
    if (calc_sw_table[type][0] == 1) result = gabi::fsubs_ppc(values->mSound, 0.5f);
    else if (calc_sw_table[type][0] == 2) result = gabi::fmuls_ppc(gabi::fsubs_ppc(values->mSound, 0.5f), weights->mSound);
    if (calc_sw_table[type][1] == 1) result = gabi::fadds_ppc(result, gabi::fsubs_ppc(values->mEffect, 0.5f));
    else if (calc_sw_table[type][1] == 2) result = gabi::fmadds(gabi::fsubs_ppc(values->mEffect, 0.5f), weights->mEffect, result);
    if (calc_sw_table[type][2] == 1) result = gabi::fadds_ppc(result, gabi::fsubs_ppc(values->mChannel, 0.5f));
    else if (calc_sw_table[type][2] == 2) result = gabi::fmadds(gabi::fsubs_ppc(values->mChannel, 0.5f), weights->mChannel, result);
    return gabi::fadds_ppc(result, 0.5f);
}
/* 02815328 */
void TChannel::directReleaseOsc(u32 release) {
    mOscillators[0].mDirectRelease = release;
}
static f32 clampChannelUnit(f32 value) {
    if (!(value > 0.0f)) return 0.0f;
    return value >= 1.0f ? 1.0f : value;
}
static f32 channelMixComponent(u32 mode,f32 pan,f32 fxMix,f32 dolby,f32 previous) {
    switch (mode) {
    case 1: return pan;
    case 2: return fxMix;
    case 3: return dolby;
    case 5: return gabi::fsubs_ppc(1.0f,pan);
    case 6: return gabi::fsubs_ppc(1.0f,fxMix);
    case 7: return gabi::fsubs_ppc(1.0f,dolby);
    default: return previous;
    }
}
/* 02814524 */
void TChannel::updateMixer(be<u16>* output,f32 volume,f32 pan,f32 fxMix,f32 dolby) {
    f32 component = 0.0f;
    for (u32 i = 0; i < 6; ++i) {
        u32 config = mMixConfigs[i];
        if (!(config >> 8)) { output[i] = 0; continue; }
        f32 level = volume;
        u32 mode = (config >> 4) & 15;
        if (mode) {
            component = channelMixComponent(mode,pan,fxMix,dolby,component);
            if (mode == 2 || mode == 6) level = gabi::fmuls_ppc(level,component);
            else level = (f32)((f64)level * channelMixerSin(gabi::fmuls_ppc(component,1.5707963705062866f)));
        }
        mode = config & 15;
        if (mode) {
            component = channelMixComponent(mode,pan,fxMix,dolby,component);
            if (mode == 2 || mode == 6) level = gabi::fmuls_ppc(level,component);
            else {
                f32 angle = component;
                if (mode == 3 || mode == 7) angle = gabi::fmadds(component,0.34775999188423157f,0.326119989156723f);
                level = (f32)((f64)level * channelMixerSin(gabi::fmuls_ppc(angle,1.5707963705062866f)));
            }
        }
        level = clampChannelUnit(level);
        u32 maximum = Driver::getChannelLevel();
        output[i] = (u16)gabi::ftoi(gabi::fmuls_ppc(level,(f32)maximum));
    }
}
/* 02814A80 */
void TChannel::updateEffectorParam(void* buffer,be<u16>* output,ChannelEffects* effects) {
    Driver::PanMatrix_ fxValues;
    Driver::PanMatrix_ panValues;
    Driver::PanMatrix_ dolbyValues;
    fxValues.mSound = (f32)mFxMixBase; fxValues.mEffect = (f32)effects->mFxMix; fxValues.mChannel = (f32)mFxMixEffect;
    panValues.mSound = (f32)mPanBase; panValues.mEffect = (f32)effects->mPan; panValues.mChannel = (f32)mPanEffect;
    dolbyValues.mSound = (f32)mDolbyBase; dolbyValues.mEffect = (f32)effects->mDolby; dolbyValues.mChannel = (f32)mDolbyEffect;
    f32 pan = 0.5f, fxMix = 0.0f, dolby = 0.0f;
    u32 mode = Driver::getOutputMode();
    if (mode == 0) fxMix = calcEffect(&fxValues,&mPanPower,mCalcTypes[1]);
    else if (mode == 1 || mode == 2) {
        if (mCalcTypes[0]) pan = calcPan(&panValues,&mPanPower,mCalcTypes[0]);
        fxMix = calcEffect(&fxValues,&mPanPower,mCalcTypes[1]);
        if (mode == 2) dolby = calcEffect(&dolbyValues,&mPanPower,mCalcTypes[2]);
    }
    f32 volume = gabi::fmuls_ppc(gabi::fmuls_ppc(mVolume,effects->mVolume),mVolumeScale);
    pan = clampChannelUnit(pan); fxMix = clampChannelUnit(fxMix); dolby = clampChannelUnit(dolby);
    if (mMixConfigs[0] == 0xFFFF) updateAutoMixer(buffer,volume,pan,fxMix,dolby);
    else updateMixer(output,volume,pan,fxMix,dolby);
}
/* 02814950 */
void TChannel::updateAutoMixer(void* buffer,f32 volume,f32 pan,f32 fxMix,f32 dolby) {
    f32 level = 0.0f;
    if (volume > 0.0f) level = volume >= 1.0f ? 1.0f : volume;
    u32 maximum = Driver::getAutoLevel();
    u32 scaledVolume = (u16)gabi::ftoi(gabi::fmuls_ppc(level,(f32)maximum));
    u32 scaledPan = (u8)gabi::ftoi(gabi::fmuls_ppc(pan,127.5f));
    u32 scaledDolby = (u8)gabi::ftoi(gabi::fmuls_ppc(dolby,127.5f));
    u32 scaledFxMix = (u8)gabi::ftoi(gabi::fmuls_ppc(fxMix,127.5f));
    [[maybe_unused]] u32 unusedDelay = (u8)(u16)mMixConfigs[1];
    ((DSPInterface::DSPBuffer*)buffer)->setAutoMixer(scaledVolume,scaledPan,scaledDolby,scaledFxMix);
}
/* 02814D6C */
void TChannel::extraUpdate() {
    u32 remaining = mSweepRemaining;
    if (!remaining) return;
    f32 pitch = mPitch;
    f32 step = gabi::fsubs_ppc(mSweepTarget,pitch) / (f32)remaining;
    mSweepRemaining = (u32)mSweepRemaining - 1;
    mPitch = gabi::fadds_ppc(pitch,step);
}
static void initializeChannelEffects(ChannelEffects* effects) {
    effects->mVolume = 1.0f; effects->mPitch = 1.0f; effects->mPan = 0.5f; effects->mFxMix = 0.0f; effects->mDolby = 0.0f;
}
static void destroyFinishedChannel(TChannel* channel) {
    freeChannelDSP(channel->mDSPChannel);
    channel->mDSPChannel = nullptr;
    gabi::call_ptr<void>(channel->mVtable->mDestructor.get(),channel,3);
}
static void updateChannelDSPPitch(TChannel* channel,DSPInterface::DSPBuffer* buffer,ChannelEffects* effects) {
    f32 pitch = gabi::fmuls_ppc(channel->mPitch,effects->mPitch);
    pitch = gabi::fmuls_ppc(pitch,channel->mPitchScale);
    buffer->setPitch((u16)gabi::ftoi(gabi::fmuls_ppc(pitch,4096.0f)));
    buffer->setPauseFlag(channel->mPauseFlag);
}
/* 02814DC8 */
s32 TChannel::updateJcToDSP(DSPInterface::DSPBuffer* buffer) {
    struct WaveLoadState { u8 mPrefix[0x24]; gptr<be<u32>> mState; };
    if (mWave && !*(be<u32>*)((WaveLoadState*)(void*)mWave)->mState) {
        stopChannelDSP(mDSPChannel);
        destroyFinishedChannel(this);
        return -1;
    }
    if (mCallback) gabi::call_ptr<void>(mCallback,0,this,buffer,(void*)mCallbackData);
    if (mGateTime) {
        mGateTime = (u32)mGateTime - 1;
        if (!mGateTime && mCallback) gabi::call_ptr<void>(mCallback,3,this,buffer,(void*)mCallbackData);
    }
    ChannelEffects effects;
    initializeChannelEffects(&effects);
    for (u32 i = 0; i < 4; ++i) {
        if (!mOscillators[i].mOsc) continue;
        mOscillators[i].getOffset();
        effectOsc(i,&effects);
        if (!i && !mOscillators[0].mState) { destroyFinishedChannel(this); return -1; }
    }
    ChannelMixer mixer;
    updateEffectorParam(buffer,mixer.mValues,&effects);
    extraUpdate();
    for (u32 i = 0; i < 6; ++i) buffer->setMixerVolumeOnly(i,(s16)(u16)mixer.mValues[i]);
    updateChannelDSPPitch(this,buffer,&effects);
    return 0;
}
/* 02815008 */
void TChannel::playLogicalChannel(DSPInterface::DSPBuffer* buffer) {
    if (mCallback) gabi::call_ptr<void>(mCallback,1,this,buffer,(void*)mCallbackData);
    if (mWaveType == 0) buffer->setWaveInfo((Driver::Wave_*)(void*)mWave,mWaveStart,mSkipSamples);
    else if (mWaveType == 2) buffer->setOscInfo(mWaveStart);
    for (u32 i = 0; i < 6; ++i) {
        u32 config = mMixConfigs[i];
        u32 mode = Driver::getOutputMode();
        u32 bus = config >> 8;
        if (mode == 0) { if (bus == 8) bus = 11; else if (bus == 9) bus = 2; }
        else if (mode == 1 && bus == 8) bus = 11;
        buffer->setBusConnect(i,bus);
    }
    ChannelEffects effects;
    initializeChannelEffects(&effects);
    for (u32 i = 0; i < 4; ++i) if (mOscillators[i].mOsc) effectOsc(i,&effects);
    ChannelMixer mixer;
    updateEffectorParam(buffer,mixer.mValues,&effects);
    for (u32 i = 0; i < 6; ++i) buffer->setMixerInitVolume(i,(s16)(u16)mixer.mValues[i]);
    updateChannelDSPPitch(this,buffer,&effects);
}
/* 02815208 */
s32 TChannel::dspCallback(u32 reason,DSPInterface::DSPBuffer* buffer,TChannel* channel) {
    if (!reason) return channel->updateJcToDSP(buffer);
    if (reason == 1) { channel->playLogicalChannel(buffer); return 0; }
    if (reason == 2 || reason == 3) { destroyFinishedChannel(channel); return -1; }
    return 0;
}
/* 028152A4 */
u32 TChannel::play() {
    TDSPChannel* dsp = allocateChannelDSP((u8)(u16)mPriority,&TChannel::dspCallback,this);
    if (!dsp) { gabi::call_ptr<void>(mVtable->mDestructor.get(),this,3); return 0; }
    mDSPChannel = dsp;
    dsp->play();
    mStatus = 1;
    return 1;
}
/* 02815330 */
void TChannel::stop(u32 release) {
    if (mStatus != 1) return;
    if (release) directReleaseOsc(release);
    for (u32 i = 0; i < 4; ++i) {
        if (mOscillators[i].mOsc) mOscillators[i].release();
    }
    ((TDSPChannel*)(void*)mDSPChannel)->setPriority((u32)mPriority >> 8);
    mStatus = 2;
}
/* 028153D0 */
void TChannel::setPanPower(f32 sound, f32 effect, f32 channel) {
    // HD: normalize directly, including a zero total.
    f32 total = gabi::fadds_ppc(gabi::fadds_ppc(sound, effect), channel);
    mPanPower.mSound = sound / total;
    mPanPower.mEffect = effect / total;
    mPanPower.mChannel = channel / total;
}
/* 028153F4 */
void TChannel::setKeySweepTarget(s32 key, u32 target) {
    s32 note = key;
    if (mWaveType != 2 && mWave) {
        note = (s32)((u32)key + 60 - ((ChannelWaveKey*)(void*)mWave)->mKey);
    }
    f32 pitch = channelKeyToPitch(note);
    pitch = gabi::fmuls_ppc(pitch, mBasePitch);
    if (target) mSweepTarget = pitch;
    mSweepRemaining = target;
    if (!target) mPitch = pitch;
}
/* 0281548C */
void TChannel::setPauseFlag(u8 pause) { mPauseFlag = pause; }
/* 02815494 */
void TChannel::setMixConfig(u32 index, u16 config) { mMixConfigs[index] = config; }
/* 028154A4 */
void TChannel::setOscInit(u32 index, const TOscillator::Osc_* config) { mOscillators[index].initStart(config); }
/* 028154B8 */
void TChannel::copyOsc(u32 index, TOscillator::Osc_* config) {
    const TOscillator::Osc_* source = mOscillators[index].mOsc;
    if (!source) return;
    // Preserve the six word transfers, including their order when the objects overlap.
    struct OscWords { be<u32> mWords[6]; };
    const OscWords* from = (const OscWords*)source;
    OscWords* to = (OscWords*)config;
    for (u32 i = 0; i < 6; ++i) to->mWords[i] = (u32)from->mWords[i];
}
/* 02815504 */
void TChannel::overwriteOsc(u32 index, const TOscillator::Osc_* config) { setOscInit(index, config); }
}
/* HD: compiler-generated and template code of this translation unit */
/* 02815508 */
void __sinit_JASChannel_cpp() {
    headerStatics.mWords[2] = 0;
    headerStatics.mWords[0] = 0;
    headerStatics.mWords[3] = 0;
    headerStatics.mWords[1] = 0;
    registerChannelStatics(&destructorRecord);
}
/* 02815530 */
TOscillator* constructChannelOscillator(TOscillator* oscillator) {
    if (!oscillator) oscillator = (TOscillator*)allocateChannelObject(0x20);
    if (oscillator) oscillator->init();
    return oscillator;
}
/* 02815578 */
f32 channelKeyToPitch(s32 key) {
    if (key < 0) key = 0;
    else if (key > 127) key = 127;
    return C5BASE_PITCHTABLE[key];
}
/* WWHD glue
ext 02816064 void JASystem::DSPInterface::DSPBuffer::setAutoMixer(u32,u32,u32,u32)
ext 0281594C JASystem::TDSPChannel* allocateChannelDSP(u32,ChannelDSPCallback,JASystem::TChannel*)
ext 028159D8 void JASystem::TDSPChannel::play()
ext 028159E8 void stopChannelDSP(void*)
ext 028159C0 void freeChannelDSP(void*)
ext 028168F8 void JASystem::TOscillator::getOffset()
ext 028160DC void JASystem::DSPInterface::DSPBuffer::setMixerVolumeOnly(u32,u32)
ext 028160A8 void JASystem::DSPInterface::DSPBuffer::setMixerInitVolume(u32,u32)
ext 0281608C void JASystem::DSPInterface::DSPBuffer::setPitch(u32)
ext 02816118 void JASystem::DSPInterface::DSPBuffer::setPauseFlag(u32)
ext 02815EEC void JASystem::DSPInterface::DSPBuffer::setWaveInfo(JASystem::Driver::Wave_*,u32,u32)
ext 02816020 void JASystem::DSPInterface::DSPBuffer::setOscInfo(u32)
ext 0281619C void JASystem::DSPInterface::DSPBuffer::setBusConnect(u32,u32)
ext 028F43F8 f64 channelMixerSin(f64)
data channelVtable 1017017C
data channelBaseVtable 1017018C
data channelPool 104B53E4
data channelHeap 101F9DA8
data calc_sw_table 10170128
data C5BASE_PITCHTABLE 101701A0
data headerStatics 104B53E8
data destructorRecord 101F9F14
ext 0273AF40 void freeChannelObject(void*)
ext C0009A60 u32 disableChannelPoolInterrupts()
ext C0009F30 void restoreChannelPoolInterrupts(u32)
ext 0273B050 void* allocateChannelPool(u32,void*,u32)
ext 028102C0 void constructChannelPool(void*)
ext 02810304 JASystem::TChannel* popChannelPool(void*,u32)
ext 02810344 void pushChannelPool(void*,JASystem::TChannel*,u32)
ext 027EC50C void constructChannelLink(JSUPtrLink*,JASystem::TChannel*)
ext 027EC62C void destroyChannelLink(JSUPtrLink*,u32)
ext 028158C8 void detachChannelDSP(void*)
ext 028EFFD0 void constructChannelOscillators(void*,u32,u32,ChannelOscillatorConstructor)
ext 028169E0 f32 JASystem::TOscillator::getValue()
ext 028159D0 void JASystem::TDSPChannel::setPriority(u32)
ext 02816A10 u32 JASystem::TOscillator::release()
ext 02816448 void JASystem::TOscillator::init()
ext 0273AD10 void* allocateChannelObject(u32)
ext 028F026C void registerChannelStatics(ChannelDestructorRecord*)
ext 02816980 void JASystem::TOscillator::initStart(const TOscillator::Osc_*)
*/

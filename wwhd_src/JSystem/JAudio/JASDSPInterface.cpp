/** DSP channel buffers and effects routing, with GC names and verified WWHD semantics. */
#include "JSystem/JAudio/JASDSPInterface.h"
#include "sead/ElementAt.h"
using namespace JASystem;
using namespace JASystem::DSPInterface;

struct ByteTable {
    be<u8> entry[8];
};
struct ConnectTable {
    be<u16> entry[12];
};
struct SendTable {
    be<u16> entry[12];
};
static ByteTable COMP_BLOCKSAMPLES;
static ByteTable COMP_BLOCKBYTES;
static ConnectTable connect_table;
static SendTable SEND_TABLE;
using ChannelPointer = gptr<DSPBuffer>;
static ChannelPointer CH_BUF;
using EffectsPointer = gptr<FXBuffer>;
static EffectsPointer FX_BUF;
static f32 sDSPVolume;
static u8 sBootPending;
using SystemPointer = gptr<void>;
static SystemPointer sDSPSystem;
static SystemPointer JASDram;
static f32 sMixerScale;
struct DspStaticPointers {
    gptr<void> entry[4];
};
static DspStaticPointers sStaticPointers;
struct DspStaticState {
    u8 bytes[1];
};
static DspStaticState sStaticState;

extern void dspInitFilterNative(DSPBuffer*);
extern void dspSetFilterTableNative(be<u16>*, be<u16>*, u32);
extern void dspBootNative(void*, u32);
extern void dspSyncNative(void*);
extern void dspMixerNative(void*, f64);
extern void* dspAllocateNative(u32, void*, u32);
extern void dspClearWordsNative(void*, u32);
extern u32 dspSetFXLineNative(u32, void*, FxlineConfig_*);
extern void dspSetupNative(void*, u32, DSPBuffer*, FXBuffer*);
extern void dspStaticInitNative(void*);

namespace JASystem {
namespace DSPInterface {

/* 02815DCC */
void DSPBuffer::initFilter() {
    // HD: eight IIR coefficients overlap the distance-filter word.
    for (u32 i = 0; i < 8; ++i)
        mFirFilterParams[i] = 0;
    mFirFilterParams[0] = (u16)(32767);
    for (u32 i = 0; i < 8; ++i)
        filters.coefficients[i] = 0;
    filters.coefficients[0] = (u16)(32767);
    filters.distance = (u16)(0);
}

/* 02815E18 */
void DSPBuffer::allocInit() {
    // HD: also clear wave-ready and sample offset before the filter call.
    mPauseFlag = 0;
    mWaveReady = 0;
    mIsActive = 0;
    mAutoMixerBeenSet = 0;
    mIsFinished = 0;
    mForcedStop = 0;
    mSamplePosition = 0;
    dspInitFilterNative(this);
}

/* 02815E3C */
void DSPBuffer::playStart() {
    field_0x60 = (u16)(0);
    field_0x66 = (u16)(0);
    field_0x8 = (u16)(1);
    field_0x10c = (u32)(0);
    for (u32 i = 0; i < 4; ++i) {
        field_0x78[i] = 0;
        field_0xa8[i] = 0;
    }
    for (u32 i = 0; i < 20; ++i)
        field_0x80[i] = 0;
    // HD: clear the additional decoder history.
    for (u32 i = 0; i < 16; ++i)
        field_0xd0[i] = 0;
    mIsActive = (u16)(1);
}

/* 02815EB4 */
void DSPBuffer::stop() {
    mIsActive = (u16)(0);
}

/* 02815EC0 */
void DSPBuffer::forceStop() {
    mForcedStop = (u16)(1);
}

/* 02815ECC */
void DSPBuffer::acknowledge() {
    mIsFinished = (u16)(0);
    mIsActive = (u16)(0);
}

/* 028161B8 */
void DSPBuffer::markWaveReady() {
    mWaveReady = (u16)(1);
}

/* 02815EDC */
u32 DSPBuffer::isFinished() {
    return (u16)mIsFinished != 0;
}

/* 02815EEC */
void DSPBuffer::setWaveInfo(Driver::Wave_* wave, u32 address, u32 offset) {
    mWaveAramAddress = (u32)(address);
    mSamplesPerBlock = (u16)((u8)COMP_BLOCKSAMPLES.entry[(u8)wave->mWaveFormat]);
    u32 bytes = (u8)COMP_BLOCKBYTES.entry[(u8)wave->mWaveFormat];
    mBytesPerBlock = (u16)(bytes);
    mSamplePosition = (u32)(0);
    if (bytes >= 4) {
        mSampleCount = (u32)((u32)wave->mSampleCount);
        u16 loop = (u16)wave->mLoopFlag;
        mLoopFlag = (u16)(loop);
        if (loop) {
            u32 start = (u32)wave->mLoopStartSample;
            if (offset == 1)
                offset = start;
            mLoopStartSample = (u32)(start);
            mEndSample = (u32)((u32)wave->mLoopEndSample);
            mpLast = (u16)((u16)wave->mpLast);
            mpPenult = (u16)((u16)wave->mpPenult);
        } else
            mEndSample = (u32)((u32)mSampleCount);
        if (offset && (u32)mEndSample > offset) {
            u8 format = (u8)wave->mWaveFormat;
            if (format <= 1) {
                u32 increment = (offset * (u16)mBytesPerBlock) >> 4;
                u32 ptr = (u32)mWaveAramAddress, end = (u32)mEndSample;
                mSamplePosition = (u32)(offset);
                u32 start = (u32)mLoopStartSample;
                mWaveAramAddress = (u32)(ptr + increment);
                mEndSample = (u32)(end - offset);
                mLoopStartSample = (u32)(start - offset);
            } else if (format <= 3)
                mSamplePosition = (u32)(offset);
        }
        for (u32 i = 0; i < 16; ++i)
            field_0xb0[i] = 0;
    }
    mWaveReady = (u16)(0);
}

/* 02816020 */
void DSPBuffer::setOscInfo(u32 value) {
    mBytesPerBlock = (u16)(value);
    mSamplesPerBlock = (u16)(16);
    mWaveAramAddress = 0;
}

/* 02816038 */
void DSPBuffer::initAutoMixer() {
    if ((u16)mAutoMixerBeenSet) {
        mAutoMixerInitVolume = (u16)((u16)mAutoMixerVolume);
        return;
    }
    mAutoMixerBeenSet = (u16)(1);
    mAutoMixerInitVolume = (u16)(0);
}

/* 02816064 */
// HD: split delay controls and four full-width arguments replace the GC byte arguments.
void DSPBuffer::setAutoMixer(u32 volume, u32 mixerHigh, u32 mixerLow, u32 delay) {
    mAutoMixerVolume = (u16)(volume);
    mAutoMixerPanDolby = (u16)((mixerHigh << 8) | (mixerLow & 255));
    mAutoMixerBeenSet = (u16)(1);
    // HD: both delay components are derived from the delay argument.
    mAutoMixerFxMix = (u16)((delay << 8) | (delay << 1));
}

/* 0281608C */
void DSPBuffer::setPitch(u32 value) {
    if (value >= 32767)
        value = 32767;
    mPitch = (u16)(value);
}

/* 028160A0 */
void DSPBuffer::setMixerInitDelayMax(u32 value) {
    mMixerInitDelayMax = (u16)(value);
}

/* 02816118 */
void DSPBuffer::setPauseFlag(u32 value) {
    mPauseFlag = (u16)(value);
}

/* 02816194 */
void DSPBuffer::setDistFilter(u32 value) {
    filters.distance = (u16)(value);
}

// Guest EA arithmetic: mOutputChannels + index * 8 + word * 2 wraps at 32 bits as on the console (a host index on a
// NULL or garbage buffer reached host addresses past 4 GB).
static inline be<u16>& outputWord(be<u16> (*channels)[4], u32 index, u32 word) {
    return sead::elementAt<be<u16>>(gabi::ea(channels), index * 4u + word);
}

/* 028160A8 */
// HD: delay is initialized separately, retaining the original store order.
void DSPBuffer::setMixerInitVolume(u32 index, u32 value) {
    outputWord(mOutputChannels, index, 1) = (u16)value;
    outputWord(mOutputChannels, index, 3) = 0;
    outputWord(mOutputChannels, index, 2) = (u16)value;
}

/* 028160C4 */
void DSPBuffer::setMixerInitDelay(u32 index, u32 value) {
    outputWord(mOutputChannels, index, 3) = (u16)((value << 8) | (value & 255));
}

/* 028160DC */
// HD: volume updates retain only the low delay byte.
void DSPBuffer::setMixerVolumeOnly(u32 index, u32 value) {
    if (mForcedStop)
        return;
    u8 delay = (u16)outputWord(mOutputChannels, index, 3) & 255;
    outputWord(mOutputChannels, index, 1) = (u16)value;
    outputWord(mOutputChannels, index, 3) = delay;
}

/* 02816100 */
void DSPBuffer::setMixerDelay(u32 index, u32 value) {
    outputWord(mOutputChannels, index, 3) = (u16)((value << 8) | ((u16)outputWord(mOutputChannels, index, 3) & 255));
}

/* 02816120 */
void DSPBuffer::setFilterMode(u32 value) {
    u32 flag = value & 32, n = value & 31, max = flag ? 20 : 24;
    if (n > max)
        n = max;
    mFilterMode = (u16)(flag + n);
}

/* 02816158 */
void setFilterTable(be<u16>* dest, be<u16>* src, u32 count) {
    for (u32 i = 0; i < count; ++i)
        dest[i] = src[i];
}

/* 0281617C */
void DSPBuffer::setIIRFilterParam(be<u16>* src) {
    dspSetFilterTableNative(filters.coefficients, src, 8);
}

/* 02816188 */
void DSPBuffer::setFIR8FilterParam(be<u16>* src) {
    dspSetFilterTableNative(mFirFilterParams, src, 8);
}

/* 0281619C */
void DSPBuffer::setBusConnect(u32 index, u32 bus) {
    outputWord(mOutputChannels, index, 0) = connect_table.entry[bus];
}

/* 028161C4 */
// HD: the renderer system replaces the GC DSP boot/synchronization path.
void boot(u32 callback) {
    if (sBootPending) {
        dspBootNative(sDSPSystem, callback);
        sBootPending = 0;
    }
}

/* 02816210 */
void sync() {
    dspSyncNative(sDSPSystem);
}

/* 0281621C */
// HD: keep the double input and scale before handing it to the renderer.
void setDSPMixerLevel(f64 value) {
    f64 scaled = fmuls_ppc(value, sMixerScale);
    void* system = sDSPSystem;
    sDSPVolume = (f32)value;
    dspMixerNative(system, scaled);
}

/* 02816240 */
u32 getDSPHandle(u32 index) {
    return CH_BUF.v.get() + index * 384u;
}

/* 02816254 */
u32 setFXLine(u32 index, void* buffer, FxlineConfig_* config) {
    FXBuffer* fx = at<FXBuffer>(FX_BUF.v.get() + index * 32u);
    fx->mActive = 0;
    if (config) {
        fx->mSend1Value = config->mSend1Value;
        fx->mSend1 = SEND_TABLE.entry[(u16)config->mSend1Idx];
        fx->mSend2Value = config->mSend2Value;
        fx->mSend2 = SEND_TABLE.entry[(u16)config->mSend2Idx];
        fx->mBufferBlocks = (u16)config->mBufferBlocks;
        dspSetFilterTableNative(fx->mFilterParams, config->mFilterParams, 8);
        if (buffer) {
            u32 size = (u32)config->mBufferBlocks * 160u;
            fx->mpBuffer = buffer;
            clearFXBuffer(buffer, size);
            buffer = fx->mpBuffer;
        } else
            buffer = fx->mpBuffer;
    } else
        fx->mpBuffer = buffer;
    fx->mActive = buffer ? (u8)config->mActive : 0;
    return 1;
}

/* 02816358 */
void initBuffer() {
    DSPBuffer* ch = (DSPBuffer*)dspAllocateNative(0x6000, JASDram, 32);
    CH_BUF = ch;
    FXBuffer* fx = (FXBuffer*)dspAllocateNative(0x80, JASDram, 32);
    FX_BUF = fx;
    dspClearWordsNative(CH_BUF, 0x1800);
    dspClearWordsNative(FX_BUF, 0x20);
    for (u32 i = 0; i < 4; ++i)
        dspSetFXLineNative(i, nullptr, nullptr);
    void* system = sDSPSystem;
    DSPBuffer* channels = CH_BUF;
    FXBuffer* effects = FX_BUF;
    dspSetupNative(system, 64, channels, effects);
}

/* 0281641C */
// HD: compiler-generated initialization of the renderer state.
static void __sinit_JASDSPInterface_cpp() {
    for (u32 i = 0; i < 4; ++i)
        sStaticPointers.entry[i] = nullptr;
    dspStaticInitNative(&sStaticState);
}

/* 02816444 */
void DSPBuffer::flushChannel() {} // HD: coherent memory needs no flush.

} // namespace DSPInterface
} // namespace JASystem

/* WWHD glue
data COMP_BLOCKSAMPLES 101703BC
data COMP_BLOCKBYTES 101703C4
data connect_table 101703CC
data CH_BUF 101F9F68
data FX_BUF 101F9F6C
data sDSPVolume 101F9F70
data SEND_TABLE 101F9F74
data sBootPending 101F9F8C
data sDSPSystem 101FA00C
data JASDram 101F9DA8
data sMixerScale 101703E8
data sStaticPointers 104B564C
data sStaticState 101F9F90
ext 02815DCC void dspInitFilterNative(DSPBuffer*)
ext 02816158 void dspSetFilterTableNative(be<u16>*, be<u16>*, u32)
ext 0281ACEC void dspBootNative(void*, u32)
ext 0281B0FC void dspSyncNative(void*)
ext 0281AD74 void dspMixerNative(void*, f64)
ext 0273B0D4 void* dspAllocateNative(u32, void*, u32)
ext 028F5914 void dspClearWordsNative(void*, u32)
ext 02816254 u32 dspSetFXLineNative(u32, void*, FxlineConfig_*)
ext 0281ACF4 void dspSetupNative(void*, u32, DSPBuffer*, FXBuffer*)
ext 028F026C void dspStaticInitNative(void*)
*/

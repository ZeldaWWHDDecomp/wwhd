#pragma once
#include "wwhd.h"
using namespace gabi;
namespace JASystem {
namespace Driver {
// HD: format is at zero; the GC loop word is packed into the low halfword.
struct Wave_ {
    be<u8> mWaveFormat;  /* was field_0x1: after TP JASWaveInfo::mWaveFormat (HD: at +0); setWaveInfo 02815EEC indexes COMP_BLOCKSAMPLES / COMP_BLOCKBYTES with it */
    u8 padding[0x11];
    be<u16> mLoopFlag;
    be<u32> mLoopStartSample;  /* was field_0x14: after TP JASWaveInfo::mLoopStartSample; setWaveInfo 02815EEC */
    be<u32> mLoopEndSample;  /* was field_0x18: after TP JASWaveInfo::mLoopEndSample; setWaveInfo 02815EEC */
    be<u32> mSampleCount;  /* was field_0x1c: after TP JASWaveInfo::mSampleCount; setWaveInfo 02815EEC */
    be<u16> mpLast;  /* was field_0x20: after TP JASWaveInfo::mpLast; setWaveInfo 02815EEC */
    be<u16> mpPenult;  /* was field_0x22: after TP JASWaveInfo::mpPenult; setWaveInfo 02815EEC */
};
} // namespace Driver
namespace DSPInterface {
struct FxlineConfig_ {
    be<u8> mActive;  /* was field_0x0: setFXLine 02816254 copies it to the FX buffer's mActive when a buffer is set */
    u8 pad1;
    be<u16> mSend1Idx;  /* was field_0x2: setFXLine 02816254: FXBuffer mSend1 = SEND_TABLE[it] (tww FXBuffer::setFXLine the same) */
    be<u16> mSend1Value;  /* was field_0x4: setFXLine 02816254 copies it into FXBuffer mSend1Value, next to mSend1 (tww FXBuffer::setFXLine the same) */
    be<u16> mSend2Idx;  /* was field_0x6: setFXLine 02816254: FXBuffer mSend2 = SEND_TABLE[it] (tww FXBuffer::setFXLine the same) */
    be<u16> mSend2Value;  /* was field_0x8: setFXLine 02816254 copies it into FXBuffer mSend2Value, next to mSend2 (tww FXBuffer::setFXLine the same) */
    u8 padA[2];
    be<u32> mBufferBlocks;  /* was field_0xc: setFXLine 02816254: buffer size = it * 160 bytes; copied to the FX buffer */
    be<u16> mFilterParams[8];  /* was field_0x10: setFXLine 02816254: 8 filter words copied with setFilterTable */
};
struct FXBuffer {
    be<u16> mActive;  /* was field_0x0: setFXLine 02816254: the line config's mActive when a buffer is set, else 0 */
    be<u16> mBufferBlocks;  /* was field_0x2: setFXLine 02816254: the config's mBufferBlocks (buffer bytes = blocks * 160) */
    gptr<void> mpBuffer;  /* was field_0x4: setFXLine 02816254: the FX line's sample buffer (kept when no new one is given) */
    be<u16> mSend1;  /* was field_0x8: setFXLine 02816254: SEND_TABLE[FxlineConfig_ mSend1Idx], the first send entry of the FX line (tww FXBuffer::setFXLine the same) */
    be<u16> mSend1Value;  /* was field_0xa: setFXLine 02816254: FxlineConfig_ mSend1Value, paired with mSend1 (tww FXBuffer::setFXLine the same) */
    be<u16> mSend2;  /* was field_0xc: setFXLine 02816254: SEND_TABLE[FxlineConfig_ mSend2Idx], the second send entry of the FX line (tww FXBuffer::setFXLine the same) */
    be<u16> mSend2Value;  /* was field_0xe: setFXLine 02816254: FxlineConfig_ mSend2Value, paired with mSend2 (tww FXBuffer::setFXLine the same) */
    be<u16> mFilterParams[8];  /* was field_0x10: setFXLine 02816254: setFilterTable of the config's 8 filter words */
};
// HD: IIR copies eight words, overlapping the distance-filter word.
union FilterState {
    be<u16> coefficients[8];
    struct {
        u8 padding[8];
        be<u16> distance;
    };
};
class DSPBuffer {
  public:
    void initFilter();
    void allocInit();
    void playStart();
    // HD: status controls retained as separate methods.
    void stop();
    void forceStop();
    void acknowledge();
    void markWaveReady();
    u32 isFinished();
    void setWaveInfo(Driver::Wave_* wave, u32 address, u32 offset);
    void setOscInfo(u32 value);
    void initAutoMixer();
    void setAutoMixer(u32 volume, u32 mixerHigh, u32 mixerLow, u32 delay);
    void setPitch(u32 value);
    void setMixerInitDelayMax(u32 value);
    void setPauseFlag(u32 value);
    void setDistFilter(u32 value);
    void setMixerInitVolume(u32 index, u32 value);
    void setMixerInitDelay(u32 index, u32 value);
    void setMixerVolumeOnly(u32 index, u32 value);
    void setMixerDelay(u32 index, u32 value);
    void setFilterMode(u32 value);
    void setIIRFilterParam(be<u16>* src);
    void setFIR8FilterParam(be<u16>* src);
    void setBusConnect(u32 index, u32 bus);
    void flushChannel();
    // HD: unsigned access preserves the verified DSP register words and full-width API arguments.
    /* 0x000 */ be<u16> mIsActive;  /* was field_0x0: after TP JASDsp::TChannel::mIsActive; playStart 02815E3C sets 1, stop 02815EB4 / acknowledge 0 */
    /* 0x002 */ be<u16> mIsFinished;  /* was field_0x2: after TP JASDsp::TChannel::mIsFinished; isFinished 02815EDC returns it, acknowledge 02815ECC clears */
    /* 0x004 */ be<u16> mPitch;  /* was field_0x4: after TP JASDsp::TChannel::mPitch; setPitch 0281608C */
    u8 padding_6[0x2];
    /* 0x008 */ be<u16> field_0x8;
    u8 padding_a[0x2];
    /* 0x00C */ be<u16> mPauseFlag;  /* was field_0xc: after TP JASDsp::TChannel::mPauseFlag; setPauseFlag 02816118 */
    /* 0x00E */ be<u16> mMixerInitDelayMax;  /* was field_0xe: setMixerInitDelayMax 028160A0 stores it (TP: unused) */
    /* 0x010 */ be<u16> mOutputChannels[8][4];  /* was field_0x10: after TP JASDsp::TChannel::mOutputChannels (per output: bus connect, target volume, current volume, delay); setMixerInitVolume 028160A8 / setMixerDelay 02816100 / setBusConnect 0281619C index it */
    /* 0x050 */ be<u16> mAutoMixerPanDolby;  /* was field_0x50: after TP JASDsp::TChannel::mAutoMixerPanDolby; setAutoMixer 02816064 (high << 8 | low) */
    /* 0x052 */ be<u16> mAutoMixerFxMix;  /* was field_0x52: after TP JASDsp::TChannel::mAutoMixerFxMix; setAutoMixer 02816064 */
    /* 0x054 */ be<u16> mAutoMixerInitVolume;  /* was field_0x54: after TP JASDsp::TChannel::mAutoMixerInitVolume; initAutoMixer 02816038 copies mAutoMixerVolume or clears it */
    /* 0x056 */ be<u16> mAutoMixerVolume;  /* was field_0x56: after TP JASDsp::TChannel::mAutoMixerVolume; setAutoMixer 02816064 */
    /* 0x058 */ be<u16> mAutoMixerBeenSet;  /* was field_0x58: after TP JASDsp::TChannel::mAutoMixerBeenSet; initAutoMixer 02816038 sets 1, allocInit clears */
    /* 0x05A */ be<u16> mWaveReady;  /* was field_0x5a: markWaveReady 028161B8 sets 1, setWaveInfo 02815EEC / allocInit clear */
    u8 padding_5c[0x4];
    /* 0x060 */ be<u16> field_0x60;
    u8 padding_62[0x2];
    /* 0x064 */ be<u16> mSamplesPerBlock;  /* was field_0x64: after TP JASDsp::TChannel::mSamplesPerBlock; setWaveInfo 02815EEC from COMP_BLOCKSAMPLES[format], setOscInfo 16 */
    /* 0x066 */ be<u16> field_0x66;
    /* 0x068 */ be<u32> mSamplePosition;  /* was field_0x68: after TP JASDsp::TChannel::mSamplePosition; setWaveInfo 02815EEC stores the start offset */
    u8 padding_6c[0xc];
    /* 0x078 */ be<u16> field_0x78[4];
    /* 0x080 */ be<u16> field_0x80[20];
    /* 0x0A8 */ be<u16> field_0xa8[4];
    /* 0x0B0 */ be<u16> field_0xb0[16];
    /* 0x0D0 */ be<u16> field_0xd0[16];
    u8 padding_f0[0x10];
    /* 0x100 */ be<u16> mBytesPerBlock;  /* was field_0x100: after TP JASDsp::TChannel::mBytesPerBlock; setWaveInfo 02815EEC from COMP_BLOCKBYTES[format] */
    /* 0x102 */ be<u16> mLoopFlag;  /* was field_0x102: setWaveInfo 02815EEC stores the wave's mLoopFlag */
    /* 0x104 */ be<u16> mpLast;  /* was field_0x104: after TP JASDsp::TChannel::mpLast; setWaveInfo 02815EEC from the wave's ADPCM history */
    /* 0x106 */ be<u16> mpPenult;  /* was field_0x106: after TP JASDsp::TChannel::mpPenult; setWaveInfo 02815EEC from the wave's ADPCM history */
    /* 0x108 */ be<u16> mFilterMode;  /* was field_0x108: after TP JASDsp::TChannel::mFilterMode; setFilterMode 02816120 */
    /* 0x10A */ be<u16> mForcedStop;  /* was field_0x10a: after TP JASDsp::TChannel::mForcedStop; forceStop 02815EC0 sets 1; setMixerVolumeOnly 028160DC tests it */
    /* 0x10C */ be<u32> field_0x10c;
    /* 0x110 */ be<u32> mLoopStartSample;  /* was field_0x110: after TP JASDsp::TChannel::mLoopStartSample; setWaveInfo 02815EEC */
    /* 0x114 */ be<u32> mEndSample;  /* was field_0x114: after TP JASDsp::TChannel::mEndSample; setWaveInfo 02815EEC (loop end or sample count) */
    /* 0x118 */ be<u32> mWaveAramAddress;  /* was field_0x118: after TP JASDsp::TChannel::mWaveAramAddress; setWaveInfo 02815EEC stores the address argument */
    /* 0x11C */ be<u32> mSampleCount;  /* was field_0x11c: after TP JASDsp::TChannel::mSampleCount; setWaveInfo 02815EEC */
    /* 0x120 */ be<u16> mFirFilterParams[8];  /* was field_0x120: after TP JASDsp::TChannel::fir_filter_params; initFilter 02815DCC / setFIR8FilterParam 02816188 */
    u8 padding_130[0x18];
    /* 0x148 */ FilterState filters;
    u8 padding_158[0x28];
};
// HD: keep verified full-width argument and return types.
void setFilterTable(be<u16>* dest, be<u16>* src, u32 count);
void boot(u32 callback);
void sync();
void setDSPMixerLevel(f64 value);
u32 getDSPHandle(u32 index);
u32 setFXLine(u32 index, void* buffer, FxlineConfig_* config);
void initBuffer();
} // namespace DSPInterface
} // namespace JASystem
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mIsActive, 0x0);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mIsFinished, 0x2);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mPitch, 0x4);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x8, 0x8);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mPauseFlag, 0xC);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mMixerInitDelayMax, 0xE);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mOutputChannels, 0x10);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mAutoMixerPanDolby, 0x50);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mAutoMixerFxMix, 0x52);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mAutoMixerInitVolume, 0x54);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mAutoMixerVolume, 0x56);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mAutoMixerBeenSet, 0x58);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mWaveReady, 0x5A);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x60, 0x60);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mSamplesPerBlock, 0x64);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x66, 0x66);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mSamplePosition, 0x68);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x78, 0x78);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x80, 0x80);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0xa8, 0xA8);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0xb0, 0xB0);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0xd0, 0xD0);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mBytesPerBlock, 0x100);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mLoopFlag, 0x102);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mpLast, 0x104);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mpPenult, 0x106);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mFilterMode, 0x108);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mForcedStop, 0x10A);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x10c, 0x10C);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mLoopStartSample, 0x110);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mEndSample, 0x114);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mWaveAramAddress, 0x118);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mSampleCount, 0x11C);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, mFirFilterParams, 0x120);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, filters, 0x148);
WWHD_SIZE(JASystem::DSPInterface::DSPBuffer, 0x180);
WWHD_SIZE(JASystem::DSPInterface::FXBuffer, 0x20);
WWHD_SIZE(JASystem::DSPInterface::FxlineConfig_, 0x20);
WWHD_OFFSET(JASystem::Driver::Wave_, mLoopFlag, 0x12);
WWHD_OFFSET(JASystem::Driver::Wave_, mSampleCount, 0x1C);
WWHD_OFFSET(JASystem::DSPInterface::FilterState, distance, 8);
// Imported memset has its own dispatch kind; preserve that native call boundary.
inline void clearFXBuffer(void* buffer, u32 size) {
    ArgPack pack{cpu};
    pack.put(buffer);
    pack.put(0u);
    pack.put(size);
    do_call(cpu, pack, 0xC000A858, 2);
}

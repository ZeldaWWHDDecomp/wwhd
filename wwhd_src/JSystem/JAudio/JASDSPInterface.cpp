/** DSP channel buffers and effects routing, with GC names and verified WWHD semantics. */
#include "JSystem/JAudio/JASDSPInterface.h"
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
        field_0x120[i] = 0;
    field_0x120[0] = (u16)(32767);
    for (u32 i = 0; i < 8; ++i)
        filters.coefficients[i] = 0;
    filters.coefficients[0] = (u16)(32767);
    filters.distance = (u16)(0);
}

/* 02815E18 */
void DSPBuffer::allocInit() {
    // HD: also clear wave-ready and sample offset before the filter call.
    field_0xc = 0;
    field_0x5a = 0;
    field_0x0 = 0;
    field_0x58 = 0;
    field_0x2 = 0;
    field_0x10a = 0;
    field_0x68 = 0;
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
    field_0x0 = (u16)(1);
}

/* 02815EB4 */
void DSPBuffer::stop() {
    field_0x0 = (u16)(0);
}

/* 02815EC0 */
void DSPBuffer::forceStop() {
    field_0x10a = (u16)(1);
}

/* 02815ECC */
void DSPBuffer::acknowledge() {
    field_0x2 = (u16)(0);
    field_0x0 = (u16)(0);
}

/* 028161B8 */
void DSPBuffer::markWaveReady() {
    field_0x5a = (u16)(1);
}

/* 02815EDC */
u32 DSPBuffer::isFinished() {
    return (u16)field_0x2 != 0;
}

/* 02815EEC */
void DSPBuffer::setWaveInfo(Driver::Wave_* wave, u32 address, u32 offset) {
    field_0x118 = (u32)(address);
    field_0x64 = (u16)((u8)COMP_BLOCKSAMPLES.entry[(u8)wave->field_0x1]);
    u32 bytes = (u8)COMP_BLOCKBYTES.entry[(u8)wave->field_0x1];
    field_0x100 = (u16)(bytes);
    field_0x68 = (u32)(0);
    if (bytes >= 4) {
        field_0x11c = (u32)((u32)wave->field_0x1c);
        u16 loop = (u16)wave->mLoopFlag;
        field_0x102 = (u16)(loop);
        if (loop) {
            u32 start = (u32)wave->field_0x14;
            if (offset == 1)
                offset = start;
            field_0x110 = (u32)(start);
            field_0x114 = (u32)((u32)wave->field_0x18);
            field_0x104 = (u16)((u16)wave->field_0x20);
            field_0x106 = (u16)((u16)wave->field_0x22);
        } else
            field_0x114 = (u32)((u32)field_0x11c);
        if (offset && (u32)field_0x114 > offset) {
            u8 format = (u8)wave->field_0x1;
            if (format <= 1) {
                u32 increment = (offset * (u16)field_0x100) >> 4;
                u32 ptr = (u32)field_0x118, end = (u32)field_0x114;
                field_0x68 = (u32)(offset);
                u32 start = (u32)field_0x110;
                field_0x118 = (u32)(ptr + increment);
                field_0x114 = (u32)(end - offset);
                field_0x110 = (u32)(start - offset);
            } else if (format <= 3)
                field_0x68 = (u32)(offset);
        }
        for (u32 i = 0; i < 16; ++i)
            field_0xb0[i] = 0;
    }
    field_0x5a = (u16)(0);
}

/* 02816020 */
void DSPBuffer::setOscInfo(u32 value) {
    field_0x100 = (u16)(value);
    field_0x64 = (u16)(16);
    field_0x118 = 0;
}

/* 02816038 */
void DSPBuffer::initAutoMixer() {
    if ((u16)field_0x58) {
        field_0x54 = (u16)((u16)field_0x56);
        return;
    }
    field_0x58 = (u16)(1);
    field_0x54 = (u16)(0);
}

/* 02816064 */
// HD: split delay controls and four full-width arguments replace the GC byte arguments.
void DSPBuffer::setAutoMixer(u32 volume, u32 mixerHigh, u32 mixerLow, u32 delay) {
    field_0x56 = (u16)(volume);
    field_0x50 = (u16)((mixerHigh << 8) | (mixerLow & 255));
    field_0x58 = (u16)(1);
    // HD: both delay components are derived from the delay argument.
    field_0x52 = (u16)((delay << 8) | (delay << 1));
}

/* 0281608C */
void DSPBuffer::setPitch(u32 value) {
    if (value >= 32767)
        value = 32767;
    field_0x4 = (u16)(value);
}

/* 028160A0 */
void DSPBuffer::setMixerInitDelayMax(u32 value) {
    field_0xe = (u16)(value);
}

/* 02816118 */
void DSPBuffer::setPauseFlag(u32 value) {
    field_0xc = (u16)(value);
}

/* 02816194 */
void DSPBuffer::setDistFilter(u32 value) {
    filters.distance = (u16)(value);
}

/* 028160A8 */
// HD: delay is initialized separately, retaining the original store order.
void DSPBuffer::setMixerInitVolume(u32 index, u32 value) {
    field_0x10[index][1] = (u16)value;
    field_0x10[index][3] = 0;
    field_0x10[index][2] = (u16)value;
}

/* 028160C4 */
void DSPBuffer::setMixerInitDelay(u32 index, u32 value) {
    field_0x10[index][3] = (u16)((value << 8) | (value & 255));
}

/* 028160DC */
// HD: volume updates retain only the low delay byte.
void DSPBuffer::setMixerVolumeOnly(u32 index, u32 value) {
    if (field_0x10a)
        return;
    u8 delay = (u16)field_0x10[index][3] & 255;
    field_0x10[index][1] = (u16)value;
    field_0x10[index][3] = delay;
}

/* 02816100 */
void DSPBuffer::setMixerDelay(u32 index, u32 value) {
    field_0x10[index][3] = (u16)((value << 8) | ((u16)field_0x10[index][3] & 255));
}

/* 02816120 */
void DSPBuffer::setFilterMode(u32 value) {
    u32 flag = value & 32, n = value & 31, max = flag ? 20 : 24;
    if (n > max)
        n = max;
    field_0x108 = (u16)(flag + n);
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
    dspSetFilterTableNative(field_0x120, src, 8);
}

/* 0281619C */
void DSPBuffer::setBusConnect(u32 index, u32 bus) {
    field_0x10[index][0] = connect_table.entry[bus];
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
    fx->field_0x0 = 0;
    if (config) {
        fx->field_0xa = config->field_0x4;
        fx->field_0x8 = SEND_TABLE.entry[(u16)config->field_0x2];
        fx->field_0xe = config->field_0x8;
        fx->field_0xc = SEND_TABLE.entry[(u16)config->field_0x6];
        fx->field_0x2 = (u16)config->field_0xc;
        dspSetFilterTableNative(fx->field_0x10, config->field_0x10, 8);
        if (buffer) {
            u32 size = (u32)config->field_0xc * 160u;
            fx->field_0x4 = buffer;
            clearFXBuffer(buffer, size);
            buffer = fx->field_0x4;
        } else
            buffer = fx->field_0x4;
    } else
        fx->field_0x4 = buffer;
    fx->field_0x0 = buffer ? (u8)config->field_0x0 : 0;
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

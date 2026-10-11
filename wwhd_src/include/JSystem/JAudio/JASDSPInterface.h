#pragma once
#include "wwhd.h"
using namespace gabi;
namespace JASystem {
namespace Driver {
// HD: format is at zero; the GC loop word is packed into the low halfword.
struct Wave_ {
    be<u8> field_0x1;
    u8 padding[0x11];
    be<u16> mLoopFlag;
    be<u32> field_0x14;
    be<u32> field_0x18;
    be<u32> field_0x1c;
    be<u16> field_0x20;
    be<u16> field_0x22;
};
} // namespace Driver
namespace DSPInterface {
struct FxlineConfig_ {
    be<u8> field_0x0;
    u8 pad1;
    be<u16> field_0x2;
    be<u16> field_0x4;
    be<u16> field_0x6;
    be<u16> field_0x8;
    u8 padA[2];
    be<u32> field_0xc;
    be<u16> field_0x10[8];
};
struct FXBuffer {
    be<u16> field_0x0;
    be<u16> field_0x2;
    gptr<void> field_0x4;
    be<u16> field_0x8;
    be<u16> field_0xa;
    be<u16> field_0xc;
    be<u16> field_0xe;
    be<u16> field_0x10[8];
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
    /* 0x000 */ be<u16> field_0x0;
    /* 0x002 */ be<u16> field_0x2;
    /* 0x004 */ be<u16> field_0x4;
    u8 padding_6[0x2];
    /* 0x008 */ be<u16> field_0x8;
    u8 padding_a[0x2];
    /* 0x00C */ be<u16> field_0xc;
    /* 0x00E */ be<u16> field_0xe;
    /* 0x010 */ be<u16> field_0x10[8][4];
    /* 0x050 */ be<u16> field_0x50;
    /* 0x052 */ be<u16> field_0x52;
    /* 0x054 */ be<u16> field_0x54;
    /* 0x056 */ be<u16> field_0x56;
    /* 0x058 */ be<u16> field_0x58;
    /* 0x05A */ be<u16> field_0x5a;
    u8 padding_5c[0x4];
    /* 0x060 */ be<u16> field_0x60;
    u8 padding_62[0x2];
    /* 0x064 */ be<u16> field_0x64;
    /* 0x066 */ be<u16> field_0x66;
    /* 0x068 */ be<u32> field_0x68;
    u8 padding_6c[0xc];
    /* 0x078 */ be<u16> field_0x78[4];
    /* 0x080 */ be<u16> field_0x80[20];
    /* 0x0A8 */ be<u16> field_0xa8[4];
    /* 0x0B0 */ be<u16> field_0xb0[16];
    /* 0x0D0 */ be<u16> field_0xd0[16];
    u8 padding_f0[0x10];
    /* 0x100 */ be<u16> field_0x100;
    /* 0x102 */ be<u16> field_0x102;
    /* 0x104 */ be<u16> field_0x104;
    /* 0x106 */ be<u16> field_0x106;
    /* 0x108 */ be<u16> field_0x108;
    /* 0x10A */ be<u16> field_0x10a;
    /* 0x10C */ be<u32> field_0x10c;
    /* 0x110 */ be<u32> field_0x110;
    /* 0x114 */ be<u32> field_0x114;
    /* 0x118 */ be<u32> field_0x118;
    /* 0x11C */ be<u32> field_0x11c;
    /* 0x120 */ be<u16> field_0x120[8];
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
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x0, 0x0);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x2, 0x2);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x4, 0x4);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x8, 0x8);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0xc, 0xC);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0xe, 0xE);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x10, 0x10);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x50, 0x50);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x52, 0x52);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x54, 0x54);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x56, 0x56);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x58, 0x58);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x5a, 0x5A);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x60, 0x60);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x64, 0x64);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x66, 0x66);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x68, 0x68);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x78, 0x78);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x80, 0x80);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0xa8, 0xA8);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0xb0, 0xB0);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0xd0, 0xD0);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x100, 0x100);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x102, 0x102);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x104, 0x104);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x106, 0x106);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x108, 0x108);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x10a, 0x10A);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x10c, 0x10C);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x110, 0x110);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x114, 0x114);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x118, 0x118);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x11c, 0x11C);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, field_0x120, 0x120);
WWHD_OFFSET(JASystem::DSPInterface::DSPBuffer, filters, 0x148);
WWHD_SIZE(JASystem::DSPInterface::DSPBuffer, 0x180);
WWHD_SIZE(JASystem::DSPInterface::FXBuffer, 0x20);
WWHD_SIZE(JASystem::DSPInterface::FxlineConfig_, 0x20);
WWHD_OFFSET(JASystem::Driver::Wave_, mLoopFlag, 0x12);
WWHD_OFFSET(JASystem::Driver::Wave_, field_0x1c, 0x1C);
WWHD_OFFSET(JASystem::DSPInterface::FilterState, distance, 8);
// Imported memset has its own dispatch kind; preserve that native call boundary.
inline void clearFXBuffer(void* buffer, u32 size) {
    ArgPack pack{cpu};
    pack.put(buffer);
    pack.put(0u);
    pack.put(size);
    do_call(cpu, pack, 0xC000A858, 2);
}

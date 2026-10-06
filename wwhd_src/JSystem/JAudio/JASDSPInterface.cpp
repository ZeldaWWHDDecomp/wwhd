/* Retained HD DSP interface02815DCC..02816444; prior DSPChannel and
 * following oscillator excluded. No reconstruction of absent GC-only APIs. */
#include "gabi.h"
using namespace gabi;
namespace dsp_interface_cpp {
void initFilter(void *self) {
  WWHD_FUNC(0x02815DCC, void, self);
  u32 p = ea(self);
  for (u32 i = 0; i < 8; ++i)
    store<u16>(p + 0x120 + i * 2, 0);
  store<u16>(p + 0x120, 32767);
  for (u32 i = 0; i < 8; ++i)
    store<u16>(p + 0x148 + i * 2, 0);
  store<u16>(p + 0x148, 32767);
  store<u16>(p + 0x150, 0);
}
VERIFY(0x02815DCC, initFilter);
void allocInit(void *self) {
  WWHD_FUNC(0x02815E18, void, self);
  u32 p = ea(self);
  for (u32 off : {0xCu, 0x5Au, 0u, 0x58u, 2u, 0x10Au})
    store<u16>(p + off, 0);
  store<u32>(p + 0x68, 0);
  call<void>(0x02815DCC, self);
}
VERIFY(0x02815E18, allocInit);
void playStart(void *self) {
  WWHD_FUNC(0x02815E3C, void, self);
  u32 p = ea(self);
  store<u16>(p + 0x60, 0);
  store<u16>(p + 0x66, 0);
  store<u16>(p + 8, 1);
  store<u32>(p + 0x10C, 0);
  for (u32 i = 0; i < 4; ++i) {
    store<u16>(p + 0x78 + i * 2, 0);
    store<u16>(p + 0xA8 + i * 2, 0);
  }
  for (u32 i = 0; i < 20; ++i)
    store<u16>(p + 0x80 + i * 2, 0);
  for (u32 i = 0; i < 16; ++i)
    store<u16>(p + 0xD0 + i * 2, 0);
  store<u16>(p, 1);
}
VERIFY(0x02815E3C, playStart);
void stop(void *self) {
  WWHD_FUNC(0x02815EB4, void, self);
  store<u16>(ea(self) + 0x0, 0);
}
VERIFY(0x02815EB4, stop);
void forceStop(void *self) {
  WWHD_FUNC(0x02815EC0, void, self);
  store<u16>(ea(self) + 0x10A, 1);
}
VERIFY(0x02815EC0, forceStop);
void acknowledge(void *self) {
  WWHD_FUNC(0x02815ECC, void, self);
  store<u16>(ea(self) + 0x2, 0);
  store<u16>(ea(self) + 0x0, 0);
}
VERIFY(0x02815ECC, acknowledge);
void markWaveReady(void *self) {
  WWHD_FUNC(0x028161B8, void, self);
  store<u16>(ea(self) + 0x5A, 1);
}
VERIFY(0x028161B8, markWaveReady);
u32 isFinished(void *self) {
  WWHD_FUNC(0x02815EDC, u32, self);
  return load<u16>(ea(self) + 2) != 0;
}
VERIFY(0x02815EDC, isFinished);
void setWaveInfo(void *self, void *wave, u32 address, u32 offset) {
  WWHD_FUNC(0x02815EEC, void, self, wave, address, offset);
  u32 p = ea(self), w = ea(wave);
  store<u32>(p + 0x118, address);
  store<u16>(p + 0x64, load<u8>(0x101703BC + load<u8>(w)));
  u32 bytes = load<u8>(0x101703C4 + load<u8>(w));
  store<u16>(p + 0x100, bytes);
  store<u32>(p + 0x68, 0);
  if (bytes >= 4) {
    store<u32>(p + 0x11C, load<u32>(w + 0x1C));
    u16 loop = load<u16>(w + 0x12);
    store<u16>(p + 0x102, loop);
    if (loop) {
      u32 start = load<u32>(w + 0x14);
      if (offset == 1)
        offset = start;
      store<u32>(p + 0x110, start);
      store<u32>(p + 0x114, load<u32>(w + 0x18));
      store<u16>(p + 0x104, load<u16>(w + 0x20));
      store<u16>(p + 0x106, load<u16>(w + 0x22));
    } else
      store<u32>(p + 0x114, load<u32>(p + 0x11C));
    if (offset && load<u32>(p + 0x114) > offset) {
      u8 format = load<u8>(w);
      if (format <= 1) {
        u32 increment = (offset * load<u16>(p + 0x100)) >> 4;
        u32 ptr = load<u32>(p + 0x118), end = load<u32>(p + 0x114);
        store<u32>(p + 0x68, offset);
        u32 start = load<u32>(p + 0x110);
        store<u32>(p + 0x118, ptr + increment);
        store<u32>(p + 0x114, end - offset);
        store<u32>(p + 0x110, start - offset);
      } else if (format <= 3)
        store<u32>(p + 0x68, offset);
    }
    for (u32 i = 0; i < 16; ++i)
      store<u16>(p + 0xB0 + i * 2, 0);
  }
  store<u16>(p + 0x5A, 0);
}
VERIFY(0x02815EEC, setWaveInfo);
void setOscInfo(void *self, u32 value) {
  WWHD_FUNC(0x02816020, void, self, value);
  store<u16>(ea(self) + 0x100, value);
  store<u16>(ea(self) + 0x64, 16);
  store<u32>(ea(self) + 0x118, 0);
}
VERIFY(0x02816020, setOscInfo);
void initAutoMixer(void *self) {
  WWHD_FUNC(0x02816038, void, self);
  u32 p = ea(self);
  if (load<u16>(p + 0x58)) {
    store<u16>(p + 0x54, load<u16>(p + 0x56));
    return;
  }
  store<u16>(p + 0x58, 1);
  store<u16>(p + 0x54, 0);
}
VERIFY(0x02816038, initAutoMixer);
void setAutoMixer(void *self, u32 volume, u32 a, u32 b, u32 c) {
  WWHD_FUNC(0x02816064, void, self, volume, a, b, c);
  u32 p = ea(self);
  store<u16>(p + 0x56, volume);
  store<u16>(p + 0x50, (a << 8) | (b & 255));
  store<u16>(p + 0x58, 1);
  store<u16>(p + 0x52, (c << 8) | (c << 1));
}
VERIFY(0x02816064, setAutoMixer);
void setPitch(void *self, u32 value) {
  WWHD_FUNC(0x0281608C, void, self, value);
  if (value >= 32767)
    value = 32767;
  store<u16>(ea(self) + 4, value);
}
VERIFY(0x0281608C, setPitch);
void setMixerInitDelayMax(void *self, u32 value) {
  WWHD_FUNC(0x028160A0, void, self, value);
  store<u16>(ea(self) + 0xE, value);
}
VERIFY(0x028160A0, setMixerInitDelayMax);
void setPauseFlag(void *self, u32 value) {
  WWHD_FUNC(0x02816118, void, self, value);
  store<u16>(ea(self) + 0xC, value);
}
VERIFY(0x02816118, setPauseFlag);
void setDistFilter(void *self, u32 value) {
  WWHD_FUNC(0x02816194, void, self, value);
  store<u16>(ea(self) + 0x150, value);
}
VERIFY(0x02816194, setDistFilter);
void setMixerInitVolume(void *self, u32 index, u32 value) {
  WWHD_FUNC(0x028160A8, void, self, index, value);
  u32 p = ea(self) + index * 8;
  store<u16>(p + 0x12, value);
  store<u16>(p + 0x16, 0);
  store<u16>(p + 0x14, value);
}
VERIFY(0x028160A8, setMixerInitVolume);
void setMixerInitDelay(void *self, u32 index, u32 value) {
  WWHD_FUNC(0x028160C4, void, self, index, value);
  store<u16>(ea(self) + index * 8 + 0x16, (value << 8) | (value & 255));
}
VERIFY(0x028160C4, setMixerInitDelay);
void setMixerVolumeOnly(void *self, u32 index, u32 value) {
  WWHD_FUNC(0x028160DC, void, self, index, value);
  u32 p = ea(self);
  if (load<u16>(p + 0x10A))
    return;
  p += index * 8;
  u8 delay = load<u8>(p + 0x17);
  store<u16>(p + 0x12, value);
  store<u16>(p + 0x16, delay);
}
VERIFY(0x028160DC, setMixerVolumeOnly);
void setMixerDelay(void *self, u32 index, u32 value) {
  WWHD_FUNC(0x02816100, void, self, index, value);
  u32 p = ea(self) + index * 8;
  store<u16>(p + 0x16, (value << 8) | (load<u16>(p + 0x16) & 255));
}
VERIFY(0x02816100, setMixerDelay);
void setFilterMode(void *self, u32 value) {
  WWHD_FUNC(0x02816120, void, self, value);
  u32 flag = value & 32, n = value & 31, max = flag ? 20 : 24;
  if (n > max)
    n = max;
  store<u16>(ea(self) + 0x108, flag + n);
}
VERIFY(0x02816120, setFilterMode);
void setFilterTable(void *dest, void *src, u32 count) {
  WWHD_FUNC(0x02816158, void, dest, src, count);
  for (u32 i = 0; i < count; ++i)
    store<u16>(ea(dest) + i * 2, load<u16>(ea(src) + i * 2));
}
VERIFY(0x02816158, setFilterTable);
void setIIRFilterParam(void *self, void *src) {
  WWHD_FUNC(0x0281617C, void, self, src);
  call<void>(0x02816158, at<void>(ea(self) + 0x148), src, 8);
}
VERIFY(0x0281617C, setIIRFilterParam);
void setFIR8FilterParam(void *self, void *src) {
  WWHD_FUNC(0x02816188, void, self, src);
  call<void>(0x02816158, at<void>(ea(self) + 0x120), src, 8);
}
VERIFY(0x02816188, setFIR8FilterParam);
void setBusConnect(void *self, u32 index, u32 bus) {
  WWHD_FUNC(0x0281619C, void, self, index, bus);
  store<u16>(ea(self) + index * 8 + 0x10, load<u16>(0x101703CC + bus * 2));
}
VERIFY(0x0281619C, setBusConnect);
void boot(u32 callback) {
  WWHD_FUNC(0x028161C4, void, callback);
  if (load<u8>(0x101F9F8C)) {
    call<void>(0x0281ACEC, at<void>(load<u32>(0x101FA00C)), callback);
    store<u8>(0x101F9F8C, 0);
  }
}
VERIFY(0x028161C4, boot);
void sync() {
  WWHD_FUNC(0x02816210, void);
  call<void>(0x0281B0FC, at<void>(load<u32>(0x101FA00C)));
}
VERIFY(0x02816210, sync);
void setDSPMixerLevel(f64 value) {
  WWHD_FUNC(0x0281621C, void, value);
  f64 scaled = fmuls_ppc(value, load<f32>(0x101703E8));
  u32 system = load<u32>(0x101FA00C);
  store<f32>(0x101F9F70, value);
  call<void>(0x0281AD74, at<void>(system), scaled);
}
VERIFY(0x0281621C, setDSPMixerLevel);
u32 getDSPHandle(u32 index) {
  WWHD_FUNC(0x02816240, u32, index);
  return load<u32>(0x101F9F68) + index * 384;
}
VERIFY(0x02816240, getDSPHandle);
u32 setFXLine(u32 index, void *buffer, void *config) {
  WWHD_FUNC(0x02816254, u32, index, buffer, config);
  u32 c = ea(config), b = ea(buffer), p = load<u32>(0x101F9F6C) + index * 32;
  store<u16>(p, 0);
  if (c) {
    store<u16>(p + 0xA, load<u16>(c + 4));
    store<u16>(p + 8, load<u16>(0x101F9F74 + load<u16>(c + 2) * 2));
    store<u16>(p + 0xE, load<u16>(c + 8));
    store<u16>(p + 0xC, load<u16>(0x101F9F74 + load<u16>(c + 6) * 2));
    store<u16>(p + 2, load<u32>(c + 0xC));
    call<void>(0x02816158, at<void>(p + 0x10), at<void>(c + 0x10), 8);
    if (b) {
      u32 size = load<u32>(c + 0xC) * 160;
      store<u32>(p + 4, b);
      ArgPack pack{cpu};
      pack.put(at<void>(b));
      pack.put(0u);
      pack.put(size);
      do_call(cpu, pack, 0xC000A858, 2);
      b = load<u32>(p + 4);
    } else
      b = load<u32>(p + 4);
  } else
    store<u32>(p + 4, b);
  store<u16>(p, b ? load<u8>(c) : 0);
  return 1;
}
VERIFY(0x02816254, setFXLine);
void initBuffer() {
  WWHD_FUNC(0x02816358, void);
  u32 ch = call<u32>(0x0273B0D4, 0x6000, at<void>(load<u32>(0x101F9DA8)), 32);
  store<u32>(0x101F9F68, ch);
  u32 fx = call<u32>(0x0273B0D4, 0x80, at<void>(load<u32>(0x101F9DA8)), 32);
  store<u32>(0x101F9F6C, fx);
  call<void>(0x028F5914, at<void>(load<u32>(0x101F9F68)), 0x1800);
  call<void>(0x028F5914, at<void>(load<u32>(0x101F9F6C)), 0x20);
  for (u32 i = 0; i < 4; ++i)
    call<void>(0x02816254, i, at<void>(0), at<void>(0));
  call<void>(0x0281ACF4, at<void>(load<u32>(0x101FA00C)), 64,
             at<void>(load<u32>(0x101F9F68)), at<void>(load<u32>(0x101F9F6C)));
}
VERIFY(0x02816358, initBuffer);
void staticInit() {
  WWHD_FUNC(0x0281641C, void);
  for (u32 i = 0; i < 4; ++i)
    store<u32>(0x104B564C + i * 4, 0);
  call<void>(0x028F026C, at<void>(0x101F9F90));
}
VERIFY(0x0281641C, staticInit);
void flushChannel(void *self) { WWHD_FUNC(0x02816444, void, self); }
VERIFY(0x02816444, flushChannel);
} // namespace dsp_interface_cpp

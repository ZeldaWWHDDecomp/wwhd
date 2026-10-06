/**
 * d_a_grid.cpp (WWHD)
 * Ship sail (daGrid_c, cloth simulation and its GX2 draw packet daHo_packet_c)
 *
 * Written from the WWHD code with the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_grid.cpp) as reference, verified against cking.rpx.
 */
#include "d/actor/d_a_grid.h"
namespace {
constexpr u32 HIO = 0x104643AC;
template <class T> T read(const void *p, u32 off) {
  return gabi::load<T>(gabi::ea(p) + off);
}
template <class T> void write(void *p, u32 off, T value) {
  gabi::store<T>(gabi::ea(p) + off, value);
}
void *member(void *p, u32 off) { return gabi::at<void>(gabi::ea(p) + off); }
} // namespace
static void setBackNrm(daHo_packet_c *packet) {
  WWHD_FUNC(0x021635F0, void, packet);
  for (u32 i = 0; i < 85; ++i) {
    void *back = member(packet, 0x8D8 + 12 * i);
    write<f32>(back, 0, 0.f);
    write<f32>(back, 4, 0.f);
    write<f32>(back, 8, 0.f);
    gabi::call(0x028E8DAC, back, member(packet, 0x4DC + 12 * i), back);
  }
}
VERIFY(0x021635F0, setBackNrm);
static bool gridDelete(daGrid_c *actor) {
  WWHD_FUNC(0x021644C8, bool, actor);
  gabi::call(0x025204C8, member(actor, 0x3AC), STR(0x100107BC));
  gabi::call(0x025204C8, member(actor, 0x3B4), STR(0x100107C4));
  s8 no = gabi::load<s8>(HIO);
  if (no >= 0) {
    gabi::call(0x025F0A18, no);
    gabi::store<s8>(HIO, -1);
  }
  return true;
}
VERIFY(0x021644C8, gridDelete);
static void hioDestroy(void *hio, s32 flags) {
  WWHD_FUNC(0x02166918, void, hio, flags);
  if (hio) {
    write<s8>(hio, 0, -1);
    write<u32>(hio, 0xA0, 0x10010718);
    if (flags & 1)
      gabi::call(0x0273AF40, hio);
  }
}
VERIFY(0x02166918, hioDestroy);
static s32 gridDrawEntry(daGrid_c *a) {
  WWHD_FUNC(0x02162F68, s32, a);
  return gabi::call<s32>(0x02162B04, a);
}
VERIFY(0x02162F68, gridDrawEntry);
static s32 setNormalMatrix(daHo_packet_c *p) {
  WWHD_FUNC(0x02162F6C, s32, p);
  s16 angle = read<s16>(p, 0xCD4);
  void *matrix = gabi::at<void>(gabi::load<u32>(0x1018C7B0));
  return gabi::call<s32>(0x025F1884, matrix, angle);
}
VERIFY(0x02162F6C, setNormalMatrix);
static bool gridIsDelete(daGrid_c *a) {
  WWHD_FUNC(0x021644C0, bool, a);
  return true;
}
VERIFY(0x021644C0, gridIsDelete);
static s32 gridExecuteEntry(daGrid_c *a) {
  WWHD_FUNC(0x021644BC, s32, a);
  return gabi::call<s32>(0x021642D4, a);
}
VERIFY(0x021644BC, gridExecuteEntry);

static void packetBaseDestroy(void *p, s32 flags) {
  WWHD_FUNC(0x02165FA8, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x02165FA8, packetBaseDestroy);
static void *materialConstruct(void *p) {
  WWHD_FUNC(0x02165FBC, void *, p);
  if (!p)
    p = gabi::call<void *>(0x0273AD10, 0x254);
  if (p) {
    gabi::call(0x027B5BD8, member(p, 4));
    gabi::call(0x027BF734, member(p, 0x158));
    write<u32>(p, 0x250, 0);
    write<u32>(p, 0x24C, 0);
  }
  return p;
}
VERIFY(0x02165FBC, materialConstruct);
static void *vertexDescriptorConstruct(void *p) {
  WWHD_FUNC(0x02166018, void *, p);
  if (!p)
    p = gabi::call<void *>(0x0273AD10, 0x10);
  return p;
}
VERIFY(0x02166018, vertexDescriptorConstruct);
static void materialDestroy(void *p, s32 flags) {
  WWHD_FUNC(0x02166044, void, p, flags);
  if (p) {
    gabi::call(0x027BF880, member(p, 0x158), 2);
    gabi::call(0x027B5CBC, member(p, 4), 2);
    if (flags & 1)
      gabi::call(0x0273AF40, p);
  }
}
VERIFY(0x02166044, materialDestroy);
static void packetVirtualNoop(void *p) { WWHD_FUNC(0x02166940, void, p); }
VERIFY(0x02166940, packetVirtualNoop);
// Reserve the outgoing linkage words below each vector, so a real nested callee
// saving LR at incoming SP+4 cannot overwrite the first vector coordinates.
struct VectorFrame {
  u8 linkage[8];
  cXyz vector;
  u8 tail[12];
};
static bool gridExecute(daGrid_c *actor) {
  WWHD_FUNC(0x021642D4, bool, actor);
  gabi::call<void *>(0x025200D4);
  s32 alpha = read<u8>(actor, 0x1096);
  void *play = gabi::call<void *>(0x025200D4);
  u8 target;
  if (!read<u8>(play, 0x5292)) {
    play = gabi::call<void *>(0x025200D4);
    void *camera = gabi::at<void>(read<u32>(play, 0x5AF8));
    gabi::Local<VectorFrame> eye, delta;
    gabi::call(0x0201AD78, member(camera, 0x264), &eye->vector,
               member(camera, 0x7C0));
    gabi::call(0x0201ADE0, member(actor, 0x314), &delta->vector, &eye->vector);
    f32 mag = gabi::call<f32>(0x028E8DD0, &delta->vector);
    f32 distance = gabi::call<f32>(0x028F4384, mag);
    f32 cutoff = gabi::load<f32>(HIO + 0x30);
    if (distance > cutoff)
      target = gabi::load<u8>(HIO + 0x2C);
    else {
      f32 rate = distance / gabi::load<f32>(HIO + 0x30);
      f32 low = gabi::load<u8>(HIO + 0x2D), high = gabi::load<u8>(HIO + 0x2C);
      target = (u8)gabi::ftoi(gabi::fmadds(high, rate, low * (1.f - rate)));
    }
  } else
    target = gabi::load<u8>(HIO + 0x2C);
  u8 result = target > alpha + 5   ? (u8)(alpha + 5)
              : target < alpha - 5 ? (u8)(alpha - 5)
                                   : target;
  write<u8>(actor, 0x1096, result);
  play = gabi::call<void *>(0x025200D4);
  if (read<u8>(play, 0x5D2C))
    gabi::store<u8>(0x10464398, 1);
  else {
    play = gabi::call<void *>(0x025200D4);
    if (!read<u8>(play, 0x5D2C))
      gabi::store<u8>(0x10464398, 0);
  }
  if (read<f32>(actor, 0x334) < 0.06f)
    write<f32>(actor, 0x2B80, 0.f);
  else
    gabi::call(0x02163678, actor);
  return true;
}
VERIFY(0x021642D4, gridExecute);

static s32 gridCreateWrapper(daGrid_c *actor) {
  WWHD_FUNC(0x021655E0, s32, actor);
  return gabi::call<s32>(0x021650A0, actor);
}
VERIFY(0x021655E0, gridCreateWrapper);
static void packetEmpty(void *packet) { WWHD_FUNC(0x021664D8, void, packet); }
VERIFY(0x021664D8, packetEmpty);
static void copyMatrix(void *dst, const void *src) {
  WWHD_FUNC(0x021623C4, void, dst, src);
  f32 values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = read<f32>(src, 4 * i);
  for (u32 i = 0; i < 12; ++i)
    write<f32>(dst, 4 * i, values[i]);
}
VERIFY(0x021623C4, copyMatrix);

static void colorS10ToFloat(void *out, const void *color) {
  WWHD_FUNC(0x02162464, void, out, color);
  s16 r = read<s16>(color, 0), g = read<s16>(color, 2), b = read<s16>(color, 4),
      a = read<s16>(color, 6);
  write<f32>(out, 0, r / 255.f);
  write<f32>(out, 4, g / 255.f);
  write<f32>(out, 8, b / 255.f);
  write<f32>(out, 12, a / 255.f);
}
VERIFY(0x02162464, colorS10ToFloat);
static void colorU8ToFloat(void *out, const void *color) {
  WWHD_FUNC(0x02162528, void, out, color);
  u8 r = read<u8>(color, 0), g = read<u8>(color, 1), b = read<u8>(color, 2),
     a = read<u8>(color, 3);
  write<f32>(out, 0, r / 255.f);
  write<f32>(out, 4, g / 255.f);
  write<f32>(out, 8, b / 255.f);
  write<f32>(out, 12, a / 255.f);
}
VERIFY(0x02162528, colorU8ToFloat);
static void vectorCopy(cXyz *dst, const cXyz *src) {
  u32 x = read<u32>(src, 0), y = read<u32>(src, 4), z = read<u32>(src, 8);
  write<u32>(dst, 0, x);
  write<u32>(dst, 4, y);
  write<u32>(dst, 8, z);
}
static void setTopNrmVtx(daHo_packet_c *packet, cXyz *out) {
  WWHD_FUNC(0x021634F0, void, packet, out);
  gabi::Local<VectorFrame> scratch, first, second, normal, transformed;
  gabi::call(0x0201ADE0, member(packet, 0x47C), &scratch->vector,
             member(packet, 0x4D0));
  vectorCopy(&first->vector, &scratch->vector);
  gabi::call(0x0201ADE0, member(packet, 0x4C4), &scratch->vector,
             member(packet, 0x4D0));
  vectorCopy(&second->vector, &scratch->vector);
  gabi::call(0x0201B080, &first->vector, &scratch->vector, &second->vector);
  vectorCopy(&normal->vector, &scratch->vector);
  gabi::call(0x0201B1E4, &normal->vector, &scratch->vector);
  vectorCopy(&normal->vector, &scratch->vector);
  gabi::call(0x0200FCD8, &normal->vector, &transformed->vector);
  gabi::call(0x0201B1E4, &transformed->vector, &scratch->vector);
  vectorCopy(out, &scratch->vector);
}
VERIFY(0x021634F0, setTopNrmVtx);

static void setNrmVtx(daHo_packet_c *packet, cXyz *out, s32 x, s32 y) {
  WWHD_FUNC(0x02162F7C, void, packet, out, x, y);
  gabi::Local<VectorFrame> position, total, xDiff, yDiff, normal, scratch;
  void *source = member(packet, 0xE0 + 12 * (x + y * 7));
  position->vector.x = read<f32>(source, 0);
  position->vector.y = read<f32>(source, 4);
  position->vector.z = read<f32>(source, 8);
  total->vector.x = 0.f;
  total->vector.y = 0.f;
  total->vector.z = 0.f;
  auto subtract = [&](s32 index, cXyz *dst) {
    gabi::call(0x0201ADE0, member(packet, 0xE0 + 12 * index), &scratch->vector,
               &position->vector);
    vectorCopy(dst, &scratch->vector);
  };
  auto accumulate = [&](bool reversed) {
    gabi::call(0x0201B080, reversed ? &yDiff->vector : &xDiff->vector,
               &scratch->vector, reversed ? &xDiff->vector : &yDiff->vector);
    vectorCopy(&normal->vector, &scratch->vector);
    gabi::call(0x0201B1E4, &normal->vector, &scratch->vector);
    vectorCopy(&normal->vector, &scratch->vector);
    gabi::call(0x028E8D88, &total->vector, &normal->vector, &total->vector);
  };
  if (x != 0) {
    subtract(y * 7 + x - 1, &xDiff->vector);
    if (y != 0 && y != 9) {
      subtract((y - 1) * 7 + x, &yDiff->vector);
      accumulate(false);
    }
    if (y == 11) {
      subtract(84, &yDiff->vector);
      accumulate(true);
    } else if (y != 8) {
      subtract((y + 1) * 7 + x, &yDiff->vector);
      accumulate(true);
    }
  }
  if (x != 6) {
    subtract(y * 7 + x + 1, &xDiff->vector);
    if (y != 0 && y != 9) {
      subtract((y - 1) * 7 + x, &yDiff->vector);
      accumulate(true);
    }
    if (y == 11) {
      subtract(84, &yDiff->vector);
      accumulate(false);
    } else if (y != 8) {
      subtract((y + 1) * 7 + x, &yDiff->vector);
      accumulate(false);
    }
  }
  if (y > 7)
    total->vector.y = 0.f;
  if (!gabi::call<s32>(0x0201B47C, &total->vector)) {
    total->vector.x = 1.f;
    total->vector.y = 0.f;
    total->vector.z = 0.f;
  }
  gabi::call(0x0200FCF0);
  u16 angle = (u16)((u32)(x + y) * -800u);
  f32 sinAngle = gabi::load<f32>(0x104A44F8 + (angle >> 3) * 8);
  s16 rotation = (s16)gabi::ftoi(900.f * sinAngle);
  void *matrix = gabi::at<void>(gabi::load<u32>(0x1018C7B0));
  gabi::call(0x025F1C28, matrix, rotation);
  gabi::call(0x0200FCD8, &total->vector, &normal->vector);
  gabi::call(0x0201B1E4, &normal->vector, &scratch->vector);
  vectorCopy(out, &scratch->vector);
  gabi::call(0x0200FD38);
}
VERIFY(0x02162F7C, setNrmVtx);

static void copySailLighting(daGrid_c *actor, const void *ship) {
  // HD keeps three lighting records: actor, ship sail, and the extra colour
  // block.
  auto floats = [&](u32 off, u32 size) {
    for (u32 i = 0; i < size; i += 4)
      write<f32>(actor, off + i, read<f32>(ship, off + i));
  };
  auto bytes = [&](u32 off, u32 size) {
    for (u32 i = 0; i < size; ++i)
      write<u8>(actor, off + i, read<u8>(ship, off + i));
  };
  auto halves = [&](u32 off, u32 size) {
    for (u32 i = 0; i < size; i += 2)
      write<u16>(actor, off + i, read<u16>(ship, off + i));
  };
  floats(0x110, 0x18);
  bytes(0x128, 4);
  halves(0x12C, 8);
  floats(0x134, 0x20);
  for (u32 off = 0x194; off < 0x1A0; off += 4)
    write<u32>(actor, off, read<u32>(ship, off));
  halves(0x1A0, 8);
  write<u32>(actor, 0x1A8, read<u32>(ship, 0x1A8));
  write<u32>(actor, 0x1AC, read<u32>(ship, 0x1AC));
  halves(0x1B0, 8);
  floats(0x1B8, 12);
  bytes(0x1C4, 9);
  floats(0x1D0, 0x18);
  bytes(0x1E8, 4);
  halves(0x1EC, 8);
  floats(0x1F4, 0x20);
  floats(0x254, 0x18);
  bytes(0x26C, 4);
  halves(0x270, 8);
  floats(0x278, 0x20);
}
static bool gridDraw(daGrid_c *actor) {
  WWHD_FUNC(0x02162B04, bool, actor);
  if (read<f32>(actor, 0x334) < 0.06f)
    return true;
  void *ship = gabi::at<void>(gabi::load<u32>(0x1046438C));
  copySailLighting(actor, ship);
  f32 x = read<f32>(actor, 0x314), y = read<f32>(actor, 0x318),
      z = read<f32>(actor, 0x31C);
  gabi::call(0x0200FAD8, x, y, z, 0);
  auto matrix = []() { return gabi::at<void>(gabi::load<u32>(0x1018C7B0)); };
  s16 angle = read<s16>(actor, 0x322);
  gabi::call(0x025F1C28, matrix(), angle);
  angle = read<s16>(actor, 0x320);
  gabi::call(0x025F1BF4, matrix(), angle);
  angle = read<s16>(actor, 0x324);
  gabi::call(0x025F1C5C, matrix(), angle);
  ship = gabi::at<void>(gabi::load<u32>(0x1046438C));
  angle = read<s16>(ship, 0x64C);
  gabi::call(0x025F1C28, matrix(), angle);
  y = read<f32>(actor, 0x334);
  gabi::call(0x0200FC74, 1.f, y, 1.f, 1);
  x = gabi::load<f32>(HIO + 0x20);
  y = gabi::load<f32>(HIO + 0x24);
  z = gabi::load<f32>(HIO + 0x28);
  gabi::call(0x0200FC74, x, y, z, 1);
  gabi::call(0x028E90D4, matrix(), member(actor, 0x468));
  write<u32>(actor, 0x498, gabi::ea(actor) + 0x110);
  void *buffer;
  if (gabi::load<u8>(0x101F4829)) {
    void *play = gabi::call<void *>(0x025200D4);
    buffer = gabi::at<void>(read<u32>(play, 0x5D60));
  } else {
    u8 alpha = read<u8>(actor, 0x1096);
    void *play = gabi::call<void *>(0x025200D4);
    buffer = gabi::at<void>(read<u32>(play, alpha == 255 ? 0x5D78 : 0x5D94));
  }
  write<u32>(buffer, 0x1C, gabi::load<u32>(0x1018C7B0));
  gabi::call(0x027F0E04, buffer, member(actor, 0x3BC), 0);
  gabi::call(0x021625DC, member(actor, 0x3BC));
  return true;
}
VERIFY(0x02162B04, gridDraw);

static s32 gridCreate(daGrid_c *actor) {
  WWHD_FUNC(0x021650A0, s32, actor);
  u32 flags = read<u32>(actor, 0x2E4);
  if (!(flags & 8)) {
    if (actor) {
      gabi::call(0x025D4ED0, actor);
      write<u32>(actor, 0xB4, 0x10010708);
      gabi::call(0x02164534, member(actor, 0x3BC));
      flags = read<u32>(actor, 0x2E4);
    }
    write<u32>(actor, 0x2E4, flags | 8);
  }
  gabi::call<void *>(0x025200D4);
  write<u8>(actor, 0x24C0, (u8)read<u32>(actor, 0xB0));
  s32 phase =
      gabi::call<s32>(0x02520460, member(actor, 0x3AC), STR(0x10010814));
  if (phase != 4)
    return phase;
  phase = gabi::call<s32>(0x02520460, member(actor, 0x3B4), STR(0x1001081C));
  if (phase != 4)
    return phase;
  if (gabi::load<s8>(HIO) < 0) {
    s32 no = gabi::call<s32>(0x025F0A10, STR(0x10010824), gabi::at<void>(HIO));
    gabi::store<u8>(HIO, (u8)no);
  }
  constexpr u32 positions = 0x101B5E58;
  for (s32 i = 0; i < 85; ++i) {
    f32 amplitude = i < 7                 ? 40.f
                    : i < 14              ? 70.f
                    : (i >= 42 && i < 56) ? 85.f
                                          : 80.f;
    s32 rowIndex = 6;
    f32 z0 = gabi::load<f32>(positions + 8),
        z6 = gabi::load<f32>(positions + 6 * 12 + 8),
        zi = gabi::load<f32>(positions + i * 12 + 8);
    f32 first = std::fabs(z0 - zi), second = std::fabs(z6 - zi),
        half = std::fabs(z0 - z6) * 0.5f;
    f32 frequency = (1.5707963705062866f / half) * 1.05f;
    if (first > second)
      first = second;
    f32 wave = gabi::call<f32>(0x028F43F8, first * frequency);
    f32 xAmplitude = wave * amplitude;
    for (s32 j = 0; j < 7; ++j) {
      for (s32 row = 0; row < 12; ++row)
        if (i == j + row * 7)
          rowIndex = j + 56;
    }
    f32 yi = gabi::load<f32>(positions + i * 12 + 4),
        yr = gabi::load<f32>(positions + rowIndex * 12 + 4);
    f32 yAmplitude;
    if (yi < yr) {
      f32 y0 = gabi::load<f32>(positions + 4);
      first = std::fabs(y0 - yi);
      second = std::fabs(yr - yi);
      half = std::fabs(yr - y0) * 0.5f;
      frequency = (1.5707963705062866f / half) * 1.05f;
      yAmplitude = rowIndex == 56 ? 35.f : rowIndex == 57 ? 70.f : 80.f;
    } else {
      f32 top = gabi::load<f32>(positions + 84 * 12 + 4);
      first = std::fabs(yr - yi);
      second = std::fabs(top - yi);
      half = std::fabs(yr - top) * 0.5f;
      frequency = (1.5707963705062866f / half) * 1.15f;
      yAmplitude = 20.f;
    }
    if (first > second)
      first = second;
    wave = gabi::call<f32>(0x028F43F8, first * frequency);
    f32 yWave = wave * yAmplitude;
    f32 weight = gabi::call<f32>(
        0x028F4384, gabi::fmadds(xAmplitude, xAmplitude, yWave * yWave));
    write<f32>(actor, 0x24CC + i * 4, weight);
  }
  gabi::store<u8>(0x10464398, 0);
  struct JudgeFrame {
    u8 linkage[8];
    be<s16> name;
    u8 padding[22];
  };
  gabi::Local<JudgeFrame> judge;
  judge->name = 0xA5;
  void *ship =
      gabi::call<void *>(0x025D5218, gabi::at<void>(0x025E121C), &judge->name);
  gabi::store<u32>(0x1046438C, gabi::ea(ship));
  gabi::call(0x02163678, actor);
  return 4;
}
VERIFY(0x021650A0, gridCreate);
static void gridStaticInit() {
  WWHD_FUNC(0x02165DA8, void);
  for (u32 off : {8u, 0u, 12u, 4u})
    gabi::store<u32>(0x1046439C + off, 0);
  gabi::call(0x028F026C, gabi::at<void>(0x101B6AD8));
  gabi::store<f32>(0x10464390, -3.1415927410125732f);
  gabi::store<f32>(0x10464394, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<void>(0x10464399));
  gabi::call(0x028F026C, gabi::at<void>(0x101B6AE4));
  gabi::call(0x028EAB2C, gabi::at<void>(0x1046439A));
  gabi::call(0x028F026C, gabi::at<void>(0x101B6AF0));
  gabi::store<s8>(HIO, -1);
  gabi::store<u8>(HIO + 1, 1);
  gabi::store<u8>(HIO + 3, 0);
  gabi::store<u8>(HIO + 2, 0);
  gabi::store<f32>(HIO + 0x10, 40.f);
  gabi::store<f32>(HIO + 0x14, 0.5f);
  gabi::store<f32>(HIO + 0x18, 0.1f);
  gabi::store<f32>(HIO + 8, 0.1f);
  gabi::store<f32>(HIO + 0x1C, 0.4f);
  gabi::store<f32>(HIO + 0x20, 1.f);
  gabi::store<f32>(HIO + 0x24, 1.f);
  gabi::store<f32>(HIO + 0x28, 1.f);
  gabi::store<u8>(HIO + 4, 0);
  gabi::store<f32>(HIO + 0x30, 900.f);
  constexpr f32 folds[] = {1.f,  0.425f, 0.45f, 0.4f,  0.2f, 0.4f, 0.45f,
                           0.4f, 0.2f,   0.5f,  0.75f, 1.f,  1.f};
  constexpr f32 rates[] = {0.05f, 0.125f, 0.175f, 0.15f,  0.0625f, 0.15f, 0.2f,
                           0.15f, 0.075f, 0.175f, 0.175f, 0.1f,    0.f};
  for (u32 i = 0; i < 13; ++i) {
    gabi::store<f32>(HIO + 0x6C + i * 4, folds[i]);
    gabi::store<f32>(HIO + 0x38 + i * 4, rates[i]);
  }
  gabi::store<u8>(HIO + 0x2C, 255);
  gabi::store<u8>(HIO + 0x2D, 50);
  gabi::store<u8>(HIO + 0x34, 0);
  gabi::store<u8>(HIO + 0x35, 0);
  gabi::store<u32>(HIO + 0xA0, 0x10010718);
  gabi::call(0x028F026C, gabi::at<void>(0x101B6AFC));
}
VERIFY(0x02165DA8, gridStaticInit);

static void heapFreeMember(void *object, u32 off) {
  void *heap = gabi::call<void *>(0x02755FEC,
                                  gabi::at<void>(gabi::load<u32>(0x101F8B4C)),
                                  gabi::at<void>(read<u32>(object, off)));
  u32 vt = read<u32>(heap, 12), target = gabi::load<u32>(vt + 0x3C);
  gabi::call(target, heap, gabi::at<void>(read<u32>(object, off)));
}
static void releaseMaterialPool(void *pool) {
  for (u32 i = 0; i < 4; ++i) {
    void *material = member(pool, i * 0x254);
    gabi::call(0x027BF7E8, member(material, 0x158));
    bool allocated = read<u32>(material, 0x250) != 0;
    write<u32>(material, 0, 0);
    if (allocated) {
      (void)read<u32>(material, 0x24C);
      heapFreeMember(material, 0x250);
      write<u32>(material, 0x24C, 0);
      write<u32>(material, 0x250, 0);
    }
  }
}
static void releaseShaderArrays(void *collection) {
  u32 array = read<u32>(collection, 8);
  write<u32>(collection, 0, 0);
  if (array) {
    for (s32 i = 0; i < read<s32>(collection, 4); ++i) {
      void *child = gabi::at<void>(array + i * 0xF4);
      u32 vt = read<u32>(child, 0xF0);
      gabi::call(gabi::load<u32>(vt + 12), child, 2);
      array = read<u32>(collection, 8);
    }
    heapFreeMember(collection, 8);
    write<u32>(collection, 4, 0);
    write<u32>(collection, 8, 0);
  }
  array = read<u32>(collection, 0x10);
  if (array) {
    for (s32 i = 0; i < read<s32>(collection, 0xC); ++i) {
      void *child = gabi::at<void>(array + i * 0xF4);
      u32 vt = read<u32>(child, 0xF0);
      gabi::call(gabi::load<u32>(vt + 12), child, 2);
      array = read<u32>(collection, 0x10);
    }
    heapFreeMember(collection, 0x10);
    write<u32>(collection, 0xC, 0);
    write<u32>(collection, 0x10, 0);
  }
}
static void releaseSailPacket(void *packet) {
  void *pool = member(packet, 0xCE8);
  write<u32>(packet, 12, 0x10010870);
  releaseMaterialPool(pool);
  write<u32>(pool, 0x960, 0);
  for (s32 i = 0; i < read<s32>(packet, 0x1650); ++i) {
    u32 base = read<u32>(packet, 0x1654);
    if ((u32)i < read<u32>(packet, 0x1650))
      base += i * 0x23C;
    gabi::call(0x027BEBEC, gabi::at<void>(base + 0x10));
    gabi::call(0x027BEBEC, gabi::at<void>(base + 0x2C));
  }
  for (u32 off : {0x166Cu, 0x1688u, 0x1714u, 0x1730u})
    gabi::call(0x027BEBEC, member(packet, off));
  for (u32 off : {0x1ED4u, 0x1CACu, 0x1A84u})
    gabi::call(0x027BE2B0, member(packet, off), 2);
  gabi::call(0x027B54A0, member(packet, 0x1A6C), 2);
  gabi::call(0x027FB528, member(packet, 0x1704), 0);
  gabi::call(0x027FB528, member(packet, 0x165C), 0);
  gabi::call(0x027FD764, member(packet, 0x1650), 2);
  if (pool) {
    releaseMaterialPool(pool);
    write<u32>(pool, 0x960, 0);
    gabi::call(0x028F0164, pool, 4, 0x254, gabi::at<void>(0x02166044), 0, 0);
  }
  u32 collections = read<u32>(packet, 0xCE4);
  if (collections) {
    for (s32 i = 0; i < read<s32>(packet, 0xCE0); ++i) {
      void *collection = gabi::at<void>(collections + i * 0x14);
      if (collection)
        releaseShaderArrays(collection);
      collections = read<u32>(packet, 0xCE4);
    }
    heapFreeMember(packet, 0xCE4);
    write<u32>(packet, 0xCE0, 0);
    write<u32>(packet, 0xCE4, 0);
  }
  gabi::call(0x027F13DC, packet, 0);
}
static void packetDestroy(void *packet, s32 flags) {
  WWHD_FUNC(0x021660A4, void, packet, flags);
  if (packet) {
    releaseSailPacket(packet);
    if (flags & 1)
      gabi::call(0x0273AF40, packet);
  }
}
VERIFY(0x021660A4, packetDestroy);
static void actorDestroy(daGrid_c *actor, s32 flags) {
  WWHD_FUNC(0x021664DC, void, actor, flags);
  if (actor) {
    releaseSailPacket(member(actor, 0x3BC));
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x021664DC, actorDestroy);

struct MaterialFrame {
  u8 linkage[8];
  be<u32> words[12];
  u8 padding[8];
};
static bool textureEqual(void *packet, u32 first, u32 second) {
  for (u32 off : {4u, 8u, 12u, 16u, 20u, 24u, 56u, 52u, 28u})
    if (read<u32>(packet, first + off) != read<u32>(packet, second + off))
      return false;
  return true;
}
static void updateMaterial(void *packet) {
  WWHD_FUNC(0x021625DC, void, packet);
  if (!read<u8>(packet, 0x20FC)) {
    gabi::call(0x0274FBF8, gabi::at<void>(gabi::load<u32>(0x101F8B18)));
    gabi::Local<MaterialFrame> name;
    name->words[0] = 0x1001072C;
    name->words[1] = 0x100106F0;
    void *resource = gabi::call<void *>(
        0x026066C4, gabi::at<void>(gabi::load<u32>(0x101F4F28)),
        &name->words[0], 3);
    gabi::call(0x02773798, member(packet, 0x206C),
               gabi::at<void>(read<u32>(resource, 0x20)));
    if (!textureEqual(packet, 0x1ED4, 0x206C))
      gabi::call(0x027BDEB4, member(packet, 0x1ED4), member(packet, 0x206C));
    else {
      u32 handle = read<u32>(packet, 0x2094),
          offset = read<u32>(packet, 0x209C);
      write<u32>(packet, 0x1FA8, handle);
      write<u32>(packet, 0x1FB0, offset);
      write<u32>(packet, 0x1EFC, handle);
      write<u32>(packet, 0x1F04, offset);
    }
    gabi::call(0x0274FCCC, gabi::at<void>(gabi::load<u32>(0x101F8B18)));
    write<u8>(packet, 0x20FC, 1);
  }
  void *pool = member(packet, 0xCE8);
  for (u32 side = 0; side < 2; ++side) {
    u32 index = read<u32>(pool, 0x950) + side * 2;
    void *vertices = gabi::at<void>(read<u32>(pool, index * 0x254));
    for (u32 i = 0; i < 172; ++i) {
      u32 pos = gabi::load<u8>(0x101B64FC + i * 3),
          normal = gabi::load<u8>(0x101B64FD + i * 3);
      void *source = member(packet, 0xE0 + pos * 12);
      f32 x = read<f32>(source, 0), y = read<f32>(source, 4),
          z = read<f32>(source, 8);
      write<f32>(vertices, i * 32, x);
      write<f32>(vertices, i * 32 + 4, y);
      write<f32>(vertices, i * 32 + 8, z);
      source = member(packet, (side ? 0x8D8 : 0x4DC) + normal * 12);
      x = read<f32>(source, 0);
      y = read<f32>(source, 4);
      z = read<f32>(source, 8);
      write<f32>(vertices, i * 32 + 12, x);
      write<f32>(vertices, i * 32 + 16, y);
      write<f32>(vertices, i * 32 + 20, z);
    }
  }
  u32 index = read<u32>(pool, 0x950);
  for (u32 side = 0; side < 2; ++side) {
    void *material = member(pool, index * 0x254 + 4 + side * 0x4A8);
    u32 length = read<u32>(material, 0x14C);
    gabi::call(0x027B5E94, material, 0, length);
  }
  write<u32>(pool, 0x950, read<u32>(pool, 0x950) == 0 ? 1 : 0);
  void *lighting = gabi::at<void>(read<u32>(packet, 0xDC));
  gabi::call(0x0255F8F4, lighting);
  gabi::Local<MaterialFrame> view, color, model;
  gabi::call(0x021623C4, &view->words[0], gabi::at<void>(0x104B45F8));
  u32 projection = gabi::load<u32>(0x104B4708);
  gabi::call(0x027FDA54, member(packet, 0x1650), 0, &view->words[0],
             gabi::at<void>(0x104B470C), gabi::at<void>(projection + 0x240));
  lighting = gabi::at<void>(read<u32>(packet, 0xDC));
  u32 materialBase = read<u32>(packet, 0x1654);
  gabi::call(0x02162464, &color->words[0], member(lighting, 0x90));
  lighting = gabi::at<void>(read<u32>(packet, 0xDC));
  f32 attenuation = read<f32>(lighting, 0x28);
  gabi::call(0x0274D458, gabi::at<void>(materialBase + 0x1C4), &color->words[0],
             attenuation);
  lighting = gabi::at<void>(read<u32>(packet, 0xDC));
  gabi::call(0x02162464, &color->words[0], member(lighting, 0x160));
  lighting = gabi::at<void>(read<u32>(packet, 0xDC));
  attenuation = read<f32>(lighting, 0x16C);
  gabi::call(0x0274D458, gabi::at<void>(materialBase + 0x1D4), &color->words[0],
             attenuation);
  write<u32>(packet, 0x1700, 0);
  for (u32 i = 0; i < 12; ++i)
    gabi::store<f32>(gabi::ea(&model->words[0]) + 4 * i,
                     read<f32>(packet, 0xAC + 4 * i));
  gabi::call(0x028E90D4, &model->words[0], member(packet, 0x16D0));
  u8 alpha = read<u8>(packet, 0xCDA);
  lighting = gabi::at<void>(read<u32>(packet, 0xDC));
  write<u16>(lighting, 0x96, alpha);
  lighting = gabi::at<void>(read<u32>(packet, 0xDC));
  s16 r = read<s16>(lighting, 0x90), g = read<s16>(lighting, 0x92),
      b = read<s16>(lighting, 0x94), a = read<s16>(lighting, 0x96);
  write<f32>(packet, 0x17B8, r / 255.f);
  write<f32>(packet, 0x17BC, g / 255.f);
  write<f32>(packet, 0x17C0, b / 255.f);
  write<f32>(packet, 0x17C4, a / 255.f);
  u8 red = read<u8>(lighting, 0x98), green = read<u8>(lighting, 0x99),
     blue = read<u8>(lighting, 0x9A), opacity = read<u8>(lighting, 0x9B);
  write<f32>(packet, 0x17C8, red / 255.f);
  write<f32>(packet, 0x17CC, green / 255.f);
  write<f32>(packet, 0x17D0, blue / 255.f);
  write<f32>(packet, 0x17D4, opacity / 255.f);
  attenuation = read<f32>(lighting, 0x24);
  gabi::call(0x0274D2AC, member(packet, 0x17C8), attenuation);
  if (read<u8>(lighting, 0x9F))
    gabi::call(0x02162528, member(packet, 0x17D8), member(lighting, 0x9C));
  else
    for (u32 i = 0; i < 4; ++i)
      write<f32>(packet, 0x17D8 + i * 4, 0.f);
  gabi::call(0x027FE0DC, member(packet, 0x1650), 0);
}
VERIFY(0x021625DC, updateMaterial);

static f32 sailSin(u32 angle) {
  return gabi::load<f32>(0x104A44F8 + ((angle & 65535u) >> 3) * 8);
}
static f32 sailCos(u32 angle) {
  return gabi::load<f32>(0x104A44FC + ((angle & 65535u) >> 3) * 8);
}
static void hoMove(daGrid_c *actor) {
  WWHD_FUNC(0x02163678, void, actor);
  if (gabi::load<u8>(HIO + 0x34))
    return;
  gabi::call<void *>(0x025200D4);
  void *wind = gabi::call<void *>(0x0257DAA8);
  f32 power = gabi::call<f32>(0x02578348);
  write<s16>(actor, 0x24C4, 7500);
  write<s16>(actor, 0x24C6, 7200);
  f32 windX = read<f32>(wind, 0), windZ = read<f32>(wind, 8);
  void *ship = gabi::at<void>(gabi::load<u32>(0x1046438C));
  s16 sail = read<s16>(ship, 0x64C);
  s32 direction = gabi::call<s32>(0x020195B0, windX, windZ);
  s32 relative =
      (s32)((u32)(s32)(s16)(read<s16>(actor, 0x322) + sail) - (u32)direction);
  s16 opposing = (s16)((u32)relative + 0x8000);
  if (opposing > 0) {
    if (sail > 0 && sail < 0x4000)
      opposing = 0;
  } else if (sail < 0 && sail > -0x4000)
    opposing = 0;
  s16 desired = (s16)gabi::ftoi((f32)opposing * 0.6f);
  if (!read<u8>(actor, 0x2B9C))
    gabi::call(0x0200F428, member(actor, 0x2B94), desired, 4, 0x1000);
  else {
    ship = gabi::at<void>(gabi::load<u32>(0x1046438C));
    s16 forced =
        (s16)(read<s16>(actor, 0x2B9A) - read<s16>(ship, 0x64C) + 0x8000);
    gabi::call(0x0200F428, member(actor, 0x2B94), forced, 2, 0x1400);
  }
  s16 angle = read<s16>(actor, 0x2B94);
  write<u8>(actor, 0x2B9C, 0);
  gabi::call(0x025F1884, gabi::at<void>(gabi::load<u32>(0x1018C7B0)), angle);
  gabi::Local<VectorFrame> input, output, light;
  input->vector.x = 0.f;
  input->vector.y = 0.f;
  input->vector.z = 0.064f;
  gabi::call(0x0200FCD8, &input->vector, &output->vector);
  f32 impulse = output->vector.x,
      flutter = std::fabs((f32)output->vector.z) + 0.02f;
  input->vector.x = 1.f;
  input->vector.z = 0.f;
  gabi::call(0x0200FCD8, &input->vector, &output->vector);
  f32 cross =
      std::fabs((f32)output->vector.z) * (1.f - read<f32>(actor, 0x2B78));
  u32 phase = read<u32>(actor, 0x24BC);
  s32 advance = gabi::ftoi(gabi::fmadds(0.8f * power, 9000.f, 2500.f));
  if (advance > 10000)
    advance = 10000;
  f32 coefficient = sailSin(phase) * cross;
  f32 debugRate = gabi::load<f32>(0x1047B9AC) + 0.01f;
  write<u32>(actor, 0x24BC, phase + (u32)advance);
  s16 twist = (s16)(read<s16>(actor, 0x2B96) +
                    (s16)gabi::ftoi(3000.f * sailCos((u32)relative)));
  f32 flutterScale = gabi::fmadds(coefficient, debugRate, 1.f);
  twist = (s16)(twist + (twist > 0 ? 300 : -300));
  write<s16>(actor, 0x2B96, twist);
  f32 sideways = 0.5f * sailSin((u32)relative);
  input->vector.x = 0.f;
  input->vector.z = 1.6f;
  s16 oscillationTimer = read<s16>(actor, 0x2B8C);
  if (oscillationTimer) {
    if (oscillationTimer == 15)
      write<f32>(actor, 0x2B88, impulse);
    u32 speed = (u32)(gabi::load<s16>(0x1047C0AC) + 7000);
    f32 kick = gabi::fmadds(sailSin((u32)oscillationTimer * speed), 0.35f, 1.f);
    write<f32>(actor, 0x2B84, kick);
    bool enabled = gabi::load<u8>(0x10464398) != 0;
    f32 sign = read<f32>(actor, 0x2B88);
    if (!enabled) {
      write<f32>(actor, 0x2B84, kick * 0.75f);
      if (read<s16>(actor, 0x2B8C) == 5)
        write<s16>(actor, 0x2B8C, 1);
    }
    if (sign < 0.f)
      write<f32>(actor, 0x2B84, -read<f32>(actor, 0x2B84));
    write<s16>(actor, 0x2B8C, (s16)(read<s16>(actor, 0x2B8C) - 1));
  } else
    gabi::call(0x0200EDC8, member(actor, 0x2B84), 0.2f, 0.05f);
  if (read<f32>(actor, 0x2B80) == 0.f) {
    write<s16>(actor, 0x24C2, 15);
    write<f32>(actor, 0x2B80, 1.f);
  } else if (read<s16>(actor, 0x24C2) > 0) {
    gabi::call(0x0200ED84, member(actor, 0x2B7C), 1.414f, 0.25f,
               gabi::load<f32>(HIO + 0x1C));
    write<s16>(actor, 0x24C2, (s16)(read<s16>(actor, 0x24C2) - 1));
  } else
    gabi::call(0x0200ED84, member(actor, 0x2B7C), 1.f, 0.1f, 0.05f);
  f32 maxX = gabi::fmadds(power, 0.75f, 0.25f);
  maxX = (maxX - 1.f) >= 0.f ? 1.f : maxX;
  f32 maxZ = gabi::fmadds(power, 0.25f, 0.5f);
  maxZ = (maxZ - 1.f) >= 0.f ? 1.f : maxZ;
  for (u32 i = 0; i < 85; ++i) {
    u32 column = i % 7, row = i / 7;
    f32 negativeRow = (f32)(s32)row - 2.f;
    f32 lateral = 3.f - (f32)column;
    f32 span = 1.f - read<f32>(actor, 0x2B78);
    s16 offset =
        (s16)gabi::ftoi(10922.f * gabi::load<f32>(0x101B6AA4 + row * 4));
    phase = read<u32>(actor, 0x24BC);
    f32 waveX = sailSin(phase + i * (u32)(s32)read<s16>(actor, 0x24C4));
    f32 waveZ =
        sailCos(phase + i * (u32)(s32)read<s16>(actor, 0x24C6)) * flutter;
    gabi::call(0x0200FCD8, &input->vector, &output->vector);
    f32 px = (f32)output->vector.x * maxX, pz = (f32)output->vector.z * maxZ;
    output->vector.x = px;
    output->vector.z = pz;
    f32 kick = read<f32>(actor, 0x2B84), gain = read<f32>(actor, 0x2B7C);
    f32 wx = gabi::fmadds(waveX, flutter, ((px * flutterScale) * span) * gain);
    f32 wz = gabi::fmadds(waveZ, 0.5f, ((pz * cross) * span) * gain);
    f32 impulseWave = gabi::fmadds(std::fabs(sailCos((u32)(s32)desired)) * kick,
                                   0.2f, kick * 0.8f);
    wx += impulseWave;
    f32 lift =
        gabi::call<f32>(0x028F4384, gabi::fmadds(wx, wx, wz * wz)) * 0.05f;
    f32 weight = read<f32>(actor, 0x24CC + i * 4);
    wx *= weight;
    wz *= weight;
    lift *= weight;
    negativeRow = negativeRow >= 0.f ? 0.f : negativeRow;
    f32 taper = (negativeRow * negativeRow) * 0.25f;
    f32 furl = read<f32>(actor, 0x2B78);
    f32 rate = gabi::load<f32>(0x101B6A3C + row * 4);
    f32 folding = gabi::load<f32>(0x101B6A70 + row * 4);
    f32 height = (float)(-(double)gabi::fmadds(gabi::fmadds(0.3f, taper, 0.67f),
                                               furl, -1.f));
    height *= gabi::fmadds(folding, furl, span);
    void *pos = member(actor, 0x49C + i * 12);
    u32 initial = 0x101B5E58 + i * 12, base = 0x101B5E58 + row * 84;
    write<f32>(pos, 0, gabi::load<f32>(initial));
    f32 baseY = gabi::load<f32>(base + 4) * (1.f - height);
    write<f32>(pos, 4,
               gabi::fmadds(gabi::load<f32>(initial + 4), height, baseY));
    write<f32>(pos, 8, gabi::load<f32>(initial + 8) * height);
    u32 bend = (u32)(s32)read<s16>(actor, 0x2B96) + (u32)(s32)offset * column;
    f32 bulge = (float)(-(double)gabi::fmadds(lateral, lateral, -9.f));
    f32 deflection =
        ((sailSin(bend) * furl) * bulge) / 9.f - (sideways * bulge) / 9.f;
    f32 xBend = (120.f * (rate * furl)) * deflection;
    f32 yBend =
        (((5.f * rate) * (f32)column) * furl) *
        (((sailCos(bend) * read<f32>(actor, 0x2B78)) * (f32)column) / 6.f);
    f32 decay = (float)(-(double)gabi::fmadds(0.5f, taper, -1.f));
    xBend *= decay;
    yBend *= decay;
    f32 zBend = -gabi::call<f32>(0x028F4384,
                                 gabi::fmadds(xBend, xBend, yBend * yBend)) *
                0.25f;
    if (column > 4) {
      f32 magnitude = std::fabs(sailSin((u32)(s32)read<s16>(actor, 0x2B96) +
                                        2u * (u32)(s32)offset * column));
      zBend = gabi::fmadds(4.25f, (f32)(column - 4) * magnitude, zBend);
    }
    f32 roll =
        (6.f * gabi::load<f32>(0x101B6A3C + row * 4)) * ((f32)column / 6.f);
    f32 attenuation = gabi::fmadds(0.65f, span, 0.35f);
    f32 xDelta =
        gabi::fmadds(wx * read<f32>(actor, 0x2B7C), attenuation, xBend);
    f32 yDelta = gabi::fmadds(lift, attenuation, yBend);
    write<f32>(pos, 0, read<f32>(pos, 0) + xDelta);
    write<f32>(pos, 4, read<f32>(pos, 4) + yDelta);
    f32 zDelta = gabi::fmadds(
        zBend, roll * read<f32>(actor, 0x2B78),
        gabi::fmadds(wz * read<f32>(actor, 0x2B7C), attenuation, -13.75f));
    write<f32>(pos, 8, read<f32>(pos, 8) + zDelta);
  }
  f32 width = std::fabs(read<f32>(actor, 0x49C + 59 * 12));
  f32 rates[5];
  constexpr f32 defaults[] = {0.0015f, 0.000700000033f, 0.000200000068f,
                              0.0015f, 0.001200000057f};
  for (u32 j = 0; j < 5; ++j)
    rates[j] = (f32)(-(double)gabi::fmadds(
        width, gabi::load<f32>(0x1047B990 + j * 4) + defaults[j], -1.f));
  for (u32 i = 1; i < 85; ++i) {
    s32 group = i >= 56 && i <= 62   ? 0
                : i >= 49 && i <= 55 ? 1
                : i >= 42 && i <= 48 ? 2
                : i >= 63 && i <= 69 ? 3
                : i >= 70 && i <= 77 ? 4
                                     : -1;
    if (group >= 0) {
      u32 off = 0x49C + i * 12 + 8;
      write<f32>(actor, off, read<f32>(actor, off) * rates[group]);
    }
  }
  gabi::call(0x0201ADE0, member(actor, 0x194), &light->vector,
             member(actor, 0x314));
  void *packet = member(actor, 0x3BC);
  gabi::call(0x02162F6C, packet, &light->vector);
  for (s32 row = 0; row < 12; ++row)
    for (s32 column = 0; column < 7; ++column)
      gabi::call(0x02162F7C, packet,
                 member(actor, 0x898 + (row * 7 + column) * 12), column, row);
  gabi::call(0x021634F0, packet, member(actor, 0x898 + 84 * 12));
  gabi::call(0x021635F0, packet);
  gabi::call(0xC00088B8, member(actor, 0x49C), 0x3FC);
  gabi::call(0xC00088B8, member(actor, 0x898), 0x3FC);
  gabi::call(0xC00088B8, member(actor, 0xC94), 0x3FC);
}
VERIFY(0x02163678, hoMove);

static void *allocateFromHeap(u32 size, u32 alignment) {
  void *heap = gabi::call<void *>(0x02756140,
                                  gabi::at<void>(gabi::load<u32>(0x101F8B4C)));
  u32 vt = read<u32>(heap, 12);
  return gabi::call<void *>(gabi::load<u32>(vt + 0x34), heap, size, alignment);
}
static void *shaderCollection(void *packet, u32 index) {
  u32 count = read<u32>(packet, 0xCE0), base = read<u32>(packet, 0xCE4);
  return gabi::at<void>(base + (index < count ? index * 20 : 0));
}
static void copyTextureHandle(void *packet, u32 dst, u32 src) {
  u32 handle = read<u32>(packet, src + 0x28),
      offset = read<u32>(packet, src + 0x30);
  write<u32>(packet, dst + 0xD4, handle);
  write<u32>(packet, dst + 0xDC, offset);
  write<u32>(packet, dst + 0x28, handle);
  write<u32>(packet, dst + 0x30, offset);
}
static void *packetConstruct(void *packet) {
  WWHD_FUNC(0x02164534, void *, packet);
  if (!packet)
    packet = gabi::call<void *>(0x0273AD10, 0x2100);
  if (!packet)
    return packet;
  gabi::call(0x027F1534, packet);
  write<u32>(packet, 12, 0x10010870);
  write<u32>(packet, 0xCDC, 0);
  void *list = member(packet, 0xCE0);
  if (!list)
    list = gabi::call<void *>(0x0273AD10, 8);
  if (list) {
    write<u32>(list, 4, 0);
    write<u32>(list, 0, 0);
  }
  void *pool = member(packet, 0xCE8), *poolStorage = pool;
  if (!poolStorage)
    poolStorage = gabi::call<void *>(0x0273AD10, 0x968);
  if (poolStorage) {
    gabi::call(0x028EFFD0, poolStorage, 4, 0x254, gabi::at<void>(0x02165FBC));
    write<u32>(poolStorage, 0x950, 0);
    write<u32>(poolStorage, 0x960, 0);
    write<u32>(poolStorage, 0x958, 32);
    write<u8>(poolStorage, 0x964, 0);
    write<u32>(poolStorage, 0x954, 0);
    for (u32 i = 0; i < 4; ++i)
      write<u32>(poolStorage, i * 0x254, 0);
  }
  gabi::call(0x027FD6F4, member(packet, 0x1650));
  gabi::call(0x027FB40C, member(packet, 0x165C));
  write<u32>(packet, 0x1668, 0x1016EF84);
  gabi::call(0x028F521C, member(packet, 0x16D0), 0x34);
  if (!member(packet, 0x16D0))
    gabi::call<void *>(0x0273AD10, 0x30);
  gabi::call(0x027FB40C, member(packet, 0x1704));
  write<u32>(packet, 0x1710, 0x1016EFB4);
  gabi::call(0x028F521C, member(packet, 0x1778), 0x2F0);
  // Three groups of identity matrices and default material colours.
  for (u32 i = 0; i < 44; ++i)
    write<f32>(packet, 0x1778 + i * 4, (i % 4 == 3) ? 1.f : 0.f);
  for (u32 off : {0x1828u, 0x1848u, 0x1868u})
    gabi::call(0x028EFFD0, member(packet, off), 2, 0x10,
               gabi::at<void>(0x02166018));
  for (u32 offset = 0x1888; offset <= 0x19D8; offset += 0x30)
    if (!member(packet, offset))
      gabi::call<void *>(0x0273AD10, 0x30);
  for (u32 offset = 0x1A08; offset <= 0x1A58; offset += 0x10)
    if (!member(packet, offset))
      gabi::call<void *>(0x0273AD10, 0x10);
  write<u8>(packet, 0x1A68, 0);
  gabi::call(0x027B5430, member(packet, 0x1A6C));
  for (u32 off : {0x1A84u, 0x1CACu, 0x1ED4u}) {
    gabi::call(0x027BDF7C, member(packet, off));
    gabi::call(0x027BE6B8, member(packet, off + 0x198));
  }
  write<s16>(packet, 0xCD4, 0);
  write<s16>(packet, 0xCD6, 0);
  write<s16>(packet, 0xCD8, 0);
  write<u8>(packet, 0xCDA, 255);
  gabi::Local<MaterialFrame> name;
  name->words[0] = 0x100107CC;
  name->words[1] = 0x100106F0;
  void *archives = gabi::call<void *>(0x027FFCBC);
  void *root = gabi::at<void>(read<u32>(archives, 4));
  s32 selection = gabi::call<s32>(0x027B90AC, root, &name->words[0]);
  void *selected = nullptr;
  if (selection >= 0) {
    u32 count = read<u32>(archives, 8), entries = read<u32>(archives, 12);
    void *entry = gabi::at<void>(
        entries + ((u32)selection < count ? (u32)selection * 0x24 : 0));
    if (!read<u8>(entry, 0x20)) {
      root = gabi::at<void>(read<u32>(archives, 4));
      u32 programCount = read<u32>(root, 0x1C);
      void *program =
          (u32)selection < programCount
              ? gabi::at<void>(read<u32>(root, 0x20) + (u32)selection * 0x84)
              : nullptr;
      gabi::call(0x02800B0C, entry, program, 0);
      count = read<u32>(archives, 8);
      entries = read<u32>(archives, 12);
    }
    selected = gabi::at<void>(
        entries + ((u32)selection < count ? (u32)selection * 0x24 : 0));
  }
  gabi::call(0x0280068C, member(packet, 0xCDC), selected, 0);
  write<u32>(pool, 0x954, 19);
  write<u32>(pool, 0x95C, 0x10010864);
  for (u32 buffer = 0; buffer < 2; ++buffer)
    for (u32 side = 0; side < 2; ++side) {
      void *material = member(pool, buffer * 0x254 + side * 0x4A8);
      if (!read<u32>(material, 0)) {
        void *vertices = allocateFromHeap(0x1580, 0x40);
        if (vertices) {
          write<u32>(material, 0x250, gabi::ea(vertices));
          write<u32>(material, 0x24C, 172);
        }
        write<u32>(material, 0, read<u32>(material, 0x250));
      }
      gabi::call(0x027FF478, member(material, 4),
                 gabi::at<void>(read<u32>(material, 0)), 172,
                 member(pool, 0x954));
    }
  write<u32>(pool, 0x960, 0);
  write<u8>(pool, 0x964, 1);
  for (u32 i = 0; i < read<u32>(packet, 0xCDC); ++i) {
    void *collection = shaderCollection(packet, i);
    u32 program = read<u32>(collection, 0);
    releaseShaderArrays(collection);
    write<u32>(collection, 0, program);
    for (u32 side = 0; side < 2; ++side) {
      void *shaders = allocateFromHeap(0x1E8, 4);
      for (u32 j = 0; j < 2; ++j) {
        void *shader = member(shaders, j * 0xF4);
        if (shader)
          gabi::call(0x027BF734, shader);
      }
      if (shaders) {
        write<u32>(collection, 8 + side * 8, gabi::ea(shaders));
        write<u32>(collection, 4 + side * 8, 2);
      }
    }
    for (u32 buffer = 0; buffer < 2; ++buffer)
      for (u32 side = 0; side < 2; ++side) {
        u32 count = read<u32>(collection, 4 + buffer * 8),
            array = read<u32>(collection, 8 + buffer * 8);
        void *shader = gabi::at<void>(array + (side < count ? side * 0xF4 : 0));
        gabi::call(0x027FF530, gabi::at<void>(program), shader,
                   member(pool, 4 + buffer * 0x254 + side * 0x4A8),
                   member(pool, 0x954), 0);
      }
  }
  gabi::call(0x027FE084, member(packet, 0x1650), 1, 0);
  gabi::call(0x027B54E0, member(packet, 0x1A6C), gabi::at<void>(0x101B6700), 4,
             gabi::load<u32>(0x101B5E34));
  write<u32>(packet, 0x1A70, 4);
  for (u32 side = 0; side < 2; ++side) {
    u32 current = read<u32>(pool, 0x950);
    void *vertices =
        gabi::at<void>(read<u32>(pool, (current + side * 2) * 0x254));
    for (u32 i = 0; i < gabi::load<u32>(0x101B5E30); ++i) {
      u32 uv = gabi::load<u8>(0x101B64FE + i * 3);
      write<f32>(vertices, i * 32 + 24, gabi::load<f32>(0x101B6254 + uv * 8));
      write<f32>(vertices, i * 32 + 28, gabi::load<f32>(0x101B6258 + uv * 8));
    }
  }
  u32 current = read<u32>(pool, 0x950), other = current ? 0 : 1;
  for (u32 side = 0; side < 2; ++side)
    for (u32 i = 0; i < 172; ++i)
      for (u32 j = 0; j < 8; ++j) {
        void *src = gabi::at<void>(
            read<u32>(pool, (read<u32>(pool, 0x950) + side * 2) * 0x254));
        void *dst = gabi::at<void>(read<u32>(pool, (other + side * 2) * 0x254));
        write<u32>(dst, i * 32 + j * 4, read<u32>(src, i * 32 + j * 4));
      }
  current = read<u32>(pool, 0x950);
  for (u32 side = 0; side < 2; ++side) {
    void *material = member(pool, 4 + current * 0x254 + side * 0x4A8);
    gabi::call(0x027B5E94, material, 0, read<u32>(material, 0x14C));
  }
  write<u32>(pool, 0x950, read<u32>(pool, 0x950) ? 0 : 1);
  gabi::call(0x0274FBF8, gabi::at<void>(gabi::load<u32>(0x101F8B18)));
  name->words[0] = 0x100107DC;
  name->words[1] = 0x100106F0;
  void *resource = gabi::call<void *>(
      0x026066C4, gabi::at<void>(gabi::load<u32>(0x101F4F28)), &name->words[0],
      23);
  gabi::call(0x02773798, member(packet, 0x1C1C),
             gabi::at<void>(read<u32>(resource, 0x20)));
  if (!textureEqual(packet, 0x1A84, 0x1C1C))
    gabi::call(0x027BDEB4, member(packet, 0x1A84), member(packet, 0x1C1C));
  else
    copyTextureHandle(packet, 0x1A84, 0x1C1C);
  name->words[0] = 0x100107DC;
  name->words[1] = 0x100106F0;
  name->words[2] = 0x100107E4;
  name->words[3] = 0x100106F0;
  resource = gabi::call<void *>(0x026124B0,
                                gabi::at<void>(gabi::load<u32>(0x101F4F7C)),
                                &name->words[0], &name->words[2], 0);
  if (resource)
    gabi::call(0x02773870, member(packet, 0x1E44), resource, STR(0x100107D4));
  if (!textureEqual(packet, 0x1CAC, 0x1E44))
    gabi::call(0x027BDEB4, member(packet, 0x1CAC), member(packet, 0x1E44));
  else
    copyTextureHandle(packet, 0x1CAC, 0x1E44);
  write<u8>(packet, 0x20FC, 0);
  gabi::call(0x0274FCCC, gabi::at<void>(gabi::load<u32>(0x101F8B18)));
  return packet;
}
VERIFY(0x02164534, packetConstruct);

struct RenderFrame {
  u8 linkage[8];
  u8 state[0x120];
};
static void virtualMaterialApply(void *packet, u32 off, void *shader) {
  void *material = member(packet, off);
  u32 vt = read<u32>(material, 12);
  gabi::call(gabi::load<u32>(vt + 0x2C), material, shader);
}
static void applyShaderLocations(void *shader, void *material) {
  u32 location = read<u32>(material, 0x4C);
  void *binding = member(material, 0x10 + location * 0x1C);
  void *info = read<u32>(shader, 0xC) ? gabi::at<void>(read<u32>(shader, 0x10))
                                      : nullptr;
  s16 pixel = read<s16>(info, 0xC), vertex = read<s16>(info, 0xE),
      geometry = read<s16>(info, 0x10);
  u32 size = read<u32>(binding, 4), data = read<u32>(binding, 12);
  if (vertex != -1)
    gabi::call(0xC0006900, vertex, data, size);
  if (pixel != -1)
    gabi::call(0xC0006A38, pixel, data, size);
  if (geometry != -1)
    gabi::call(0xC00068A8, geometry, data, size);
}
static void applyShaderTextures(void *packet, void *shader) {
  bool alternate = gabi::load<u8>(0x10464398) != 0;
  u32 count = read<u32>(shader, 0x14);
  u32 textures = count ? read<u32>(shader, 0x18) : 0;
  gabi::call(0x027BE53C, member(packet, alternate ? 0x1CAC : 0x1A84),
             gabi::at<void>(textures + 4), -1, 0);
  count = read<u32>(shader, 0x14);
  textures = count > 1 ? read<u32>(shader, 0x18) + 0x14 : 0;
  gabi::call(0x027BE53C, member(packet, 0x1ED4), gabi::at<void>(textures + 4),
             -1, 0);
}
static void renderPacket(void *packet, void *drawInfo) {
  WWHD_FUNC(0x021655E4, void, packet, drawInfo);
  gabi::Local<MaterialFrame> names;
  names->words[0] = 0x1001082C;
  names->words[1] = 0x100106F0;
  void *play = gabi::call<void *>(0x025200D4);
  names->words[2] = gabi::ea(play) + 0x5134;
  names->words[3] = 0x100106F0;
  auto stringAssure = [&](u32 off) {
    u32 vt = gabi::load<u32>(gabi::ea(&names->words[0]) + off + 4);
    gabi::call(gabi::load<u32>(vt + 0x14), member(&names->words[0], off));
  };
  stringAssure(0);
  stringAssure(0);
  u32 first = names->words[0];
  stringAssure(8);
  u32 second = names->words[2];
  bool equal = first == second;
  if (!equal) {
    equal = true;
    for (u32 i = 0; i < 0x40001; ++i) {
      u8 a = gabi::load<u8>(first + i), b = gabi::load<u8>(second + i);
      if (a != b) {
        equal = false;
        break;
      }
      if (!a)
        break;
    }
  }
  if (equal) {
    play = gabi::call<void *>(0x025200D4);
    void *player = gabi::at<void>(read<u32>(play, 0x5B34));
    if (read<s8>(player, 0x326) >= 7)
      return;
  }
  s32 kind = read<s32>(drawInfo, 12);
  void *shader = nullptr;
  if (kind < 4) {
    void *collection = shaderCollection(packet, (u32)kind);
    shader = gabi::at<void>(read<u32>(collection, 0));
  }
  void *system = gabi::at<void>(0x104B45C0);
  void *active = gabi::call<void *>(0x027F29D4, system);
  void *program = gabi::at<void>(read<u32>(shader, 0));
  if (gabi::ea(program) != read<u32>(active, 4)) {
    u8 flags = read<u8>(program, 0);
    u32 previous = read<u32>(active, 0);
    if (flags & 2) {
      write<u8>(program, 0, flags & ~2u);
      gabi::call(0x027BB9E0, program, 0);
    }
    void *resource = gabi::at<void>(read<u32>(program, 0x7C));
    u32 group = read<u32>(resource, 0x28);
    if (previous != group)
      gabi::call(0x027B9F68, gabi::at<void>(group));
    if (read<u32>(program, 12))
      gabi::call(0xC00060E0, gabi::at<void>(read<u32>(program, 4)), read<u32>(program, 12)); /* GX2CallDisplayList(list, size): r4 = size (game test 2026-10-05: KoRL sail colours) */
    else
      gabi::call(0x027BB7CC, program);
    write<u32>(active, 0, group);
    write<u32>(active, 4, gabi::ea(program));
  }
  gabi::Local<RenderFrame> frame;
  void *state = &frame->state[0];
  kind = read<s32>(drawInfo, 12);
  if (kind == 0) {
    virtualMaterialApply(packet, 0x1704, shader);
    virtualMaterialApply(packet, 0x165C, shader);
    void *extra = gabi::at<void>(read<u32>(drawInfo, 0x14));
    if (extra)
      applyShaderLocations(shader, gabi::at<void>(read<u32>(extra, 4)));
    applyShaderTextures(packet, shader);
    gabi::call(0x02750250, state);
  } else if (kind == 1 || kind == 2) {
    applyShaderLocations(shader, gabi::at<void>(read<u32>(packet, 0x1654)));
    virtualMaterialApply(packet, 0x1704, shader);
    virtualMaterialApply(packet, 0x165C, shader);
    if (kind == 2) {
      void *extra = gabi::at<void>(read<u32>(drawInfo, 0x30));
      if (extra) {
        u32 vt = read<u32>(extra, 12);
        gabi::call(gabi::load<u32>(vt + 0x2C), extra, shader);
      }
    }
    applyShaderTextures(packet, shader);
    if (kind == 2)
      gabi::call(0x027FFE54, drawInfo, shader);
    gabi::call(0x02750250, state);
  } else
    gabi::call(0x02750250, state);
  write<u32>(state, 8, 0);
  write<u32>(state, 12, 3);
  write<u8>(state, 0xE0, 0);
  u32 flags = read<u32>(state, 0xEC);
  u32 packed = (((flags & ~15u) + 7) & 0xFFFFFF0Fu) + 0x10;
  write<u32>(state, 0xEC, packed);
  kind = read<s32>(drawInfo, 12);
  gabi::call(0x0280037C, kind, state);
  if (read<u8>(packet, 0xCDA) != 255 && read<u32>(drawInfo, 12) == 1) {
    write<u8>(state, 1, 0);
    write<u8>(state, 0, 1);
  }
  write<u8>(state, 0xE0, 0);
  gabi::call(0x02750370, state);
  auto selectedArray = [&]() {
    void *collection = shaderCollection(packet, read<u32>(drawInfo, 12));
    u32 current = read<u32>(packet, 0x1638);
    return member(collection, current ? 0 : 8);
  };
  void *array = selectedArray();
  gabi::call(0x027BFE5C, gabi::at<void>(read<u32>(array, 8)));
  auto draw = [&]() {
    u32 count = read<u32>(packet, 0x1A78);
    if (count) {
      u32 type = read<u32>(packet, 0x1A6C), offset = read<u32>(packet, 0x1A74),
          indices = read<u32>(packet, 0x1A70);
      gabi::call(0xC0006178, indices, count, type, offset, 0, 1);
    }
  };
  draw();
  write<u32>(state, 8, 1);
  gabi::call(0x02750370, state);
  array = selectedArray();
  u32 count = read<u32>(array, 4), shaders = read<u32>(array, 8);
  if (count > 1)
    shaders += 0xF4;
  gabi::call(0x027BFE5C, gabi::at<void>(shaders));
  draw();
  gabi::call(0x02750370, member(system, 0x18C));
}
VERIFY(0x021655E4, renderPacket);

static s32 gridDeleteWrapper(daGrid_c *a) {
  WWHD_FUNC(0x02164530, s32, a);
  return gabi::call<s32>(0x021644C8, a);
}
VERIFY(0x02164530, gridDeleteWrapper);

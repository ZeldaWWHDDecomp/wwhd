/**
 * d_a_bgn3.cpp (WWHD)
 * Boss - Puppet Ganon (Phase 3: the worm)
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww d_a_bgn3.cpp) and the WWHD code.
 */
#include "gabi.h"
#include <bit>
#include <cmath>
namespace bgn3 {
using gabi::call;
using gabi::load;
using gabi::store;
u32 bossSearch(u32 a, u32 ignored) {
  WWHD_FUNC(0x0208FC14, u32, a, ignored);
  if (call<s32>(0x025D4604, a) && a && load<s16>(a + 8) == 0xF3)
    return a;
  return 0;
}
VERIFY(0x0208FC14, bossSearch);
u32 baitSearch(u32 a, u32 ignored) {
  WWHD_FUNC(0x020900A4, u32, a, ignored);
  if (call<s32>(0x025D4604, a) && a && load<s16>(a + 8) == 0xDD)
    return a;
  return 0;
}
VERIFY(0x020900A4, baitSearch);
u32 treeCount(u32 a, u32 ignored) {
  WWHD_FUNC(0x02090624, u32, a, ignored);
  if (call<s32>(0x025D4604, a) && a && load<s16>(a + 8) == 0xCD)
    store<u32>(0x1046225C, load<u32>(0x1046225C) + 1);
  return 0;
}
VERIFY(0x02090624, treeCount);
bool ground(u32 a) {
  WWHD_FUNC(0x02090340, bool, a);
  f32 floor = load<f32>(0x1047B634) + 150.0f;
  if (!(load<f32>(a + 0x318) > floor)) {
    store<f32>(a + 0x318, floor);
    store<f32>(a + 0x340, 0.0f);
    return true;
  }
  return false;
}
VERIFY(0x02090340, ground);
void position(u32 a) {
  WWHD_FUNC(0x02090384, void, a);
  gabi::Local<f32[3]> v, out;
  store<f32>(v.a, 0.0f);
  store<f32>(v.a + 4, 0.0f);
  store<f32>(v.a + 8, load<f32>(a + 0x370));
  call<void>(0x025F1884, load<u32>(0x1018C7B0), load<s16>(a + 0x322));
  call<void>(0x0200FCD8, v.a, out.a);
  store<f32>(a + 0x33C, load<f32>(out.a));
  store<f32>(a + 0x344, load<f32>(out.a + 8));
  call<void>(0x028E8D88, a + 0x314, a + 0x33C, a + 0x314);
  f32 delta = load<f32>(0x1047B61C) - 5.0f;
  store<f32>(a + 0x340, load<f32>(a + 0x340) + delta);
}
VERIFY(0x02090384, position);
bool isDelete(u32 a) {
  WWHD_FUNC(0x02090908, bool, a);
  return true;
}
VERIFY(0x02090908, isDelete);
bool remove(u32 a) {
  WWHD_FUNC(0x02090910, bool, a);
  call<s32>(0x025204C8, a + 0x3C8, 0x10009058u);
  if (load<u8>(a + 0x12388)) {
    s8 n = load<s8>(0x10462288);
    store<u8>(0x1019119C, 0);
    call<void>(0x025F0A18, n);
  }
  call<void>(0x025E1B34, a + 0x12130);
  return true;
}
VERIFY(0x02090910, remove);
f32 tableSin(u32 a, u32 angle) {
  WWHD_FUNC(0x02091704, f32, a, angle);
  return load<f32>(a + ((angle & 65535) >> 3) * 8);
}
VERIFY(0x02091704, tableSin);
f32 tableCos(u32 a, u32 angle) {
  WWHD_FUNC(0x02091718, f32, a, angle);
  return load<f32>(a + ((angle & 65535) >> 3) * 8 + 4);
}
VERIFY(0x02091718, tableCos);
void hioDtor(u32 a, u32 flags) {
  WWHD_FUNC(0x02094448, void, a, flags);
  if (a && (flags & 1))
    call<void>(0x0273AF40, a);
}
VERIFY(0x02094448, hioDtor);
void partDtor(u32 a, u32 flags) {
  WWHD_FUNC(0x0209445C, void, a, flags);
  if (a) {
    call<void>(0x025E99E0, a + 0x1890, 2);
    call<void>(0x02515AE8, a + 0x2AC, 2);
    call<void>(0x02082DDC, a + 4, 2);
    if (flags & 1)
      call<void>(0x0273AF40, a);
  }
}
VERIFY(0x0209445C, partDtor);
void actorDtor(u32 a, u32 flags) {
  WWHD_FUNC(0x020944C8, void, a, flags);
  if (a) {
    call<void>(0x02082DDC, a + 0x122CC, 2);
    call<void>(0x025EB8B8, a + 0x12140, 2);
    call<void>(0x02515AE8, a + 0x12004, 2);
    call<void>(0x02515AE8, a + 0x11ED8, 2);
    call<void>(0x02515860, a + 0x11E9C, 2);
    call<void>(0x028F0164, a + 0x1BE8, 10, 0x19D8, 0x0209445Cu, 0, 0);
    call<void>(0x02082DDC, a + 0x1B20, 2);
    call<void>(0x025E99E0, a + 0x19D8, 2);
    call<void>(0x02082DDC, a + 0x488, 2);
    call<void>(0x02082DDC, a + 0x3D4, 2);
    call<void>(0x025D50BC, a, 0);
    if (flags & 1)
      call<void>(0x0273AF40, a);
  }
}
VERIFY(0x020944C8, actorDtor);
void genMessage(u32 a, u32 context) { WWHD_FUNC(0x020945C8, void, a, context); }
VERIFY(0x020945C8, genMessage);

u32 hioCtor(u32 a) {
  WWHD_FUNC(0x020914F4, u32, a);
  if (!a)
    a = call<u32>(0x0273AD10, 0x58);
  if (!a)
    return 0;
  store<s16>(a + 0x18, 1200);
  store<f32>(a + 0xC, 60.0f);
  store<u8>(a + 1, 0);
  store<f32>(a + 4, 50.0f);
  store<u8>(a + 2, 0);
  store<s8>(a, -1);
  store<s16>(a + 0x12, 3);
  store<s16>(a + 0xA, 3);
  store<s16>(a + 0x10, 1000);
  store<s16>(a + 0x1A, 3);
  store<s16>(a + 8, 800);
  store<u32>(a + 0x54, 0x10008FF4);
  store<s16>(a + 0x1E, 40);
  store<s16>(a + 0x22, 10);
  store<s16>(a + 0x1C, 60);
  store<s16>(a + 0x26, 45);
  store<s16>(a + 0x28, 120);
  store<s16>(a + 0x24, 20);
  store<s16>(a + 0x20, 30);
  store<f32>(a + 0x14, 80.0f);
  store<u8>(a + 0x3A, 1);
  store<f32>(a + 0x30, 5.0f);
  store<s16>(a + 0x2C, 250);
  store<f32>(a + 0x48, -8.0f);
  store<f32>(a + 0x34, 0.5f);
  store<f32>(a + 0x40, 45.0f);
  store<s16>(a + 0x2A, 200);
  store<s16>(a + 0x4E, 152);
  store<f32>(a + 0x3C, 150.0f);
  store<s16>(a + 0x4C, 188);
  store<s16>(a + 0x38, 3);
  store<f32>(a + 0x44, 8.0f);
  store<s16>(a + 0x50, 66);
  return a;
}
VERIFY(0x020914F4, hioCtor);
void staticInit() {
  WWHD_FUNC(0x02091648, void);
  store<u32>(0x10462274, 0);
  store<u32>(0x1046226C, 0);
  store<u32>(0x10462278, 0);
  store<u32>(0x10462270, 0);
  call<void>(0x028F026C, 0x10191288u);
  f32 low = load<f32>(0x10009098), high = load<f32>(0x1000909C);
  store<f32>(0x10462260, low);
  store<f32>(0x10462264, high);
  call<void>(0x028ED6F8, 0x10462268u);
  call<void>(0x028F026C, 0x10191294u);
  call<void>(0x028EAB2C, 0x10462269u);
  call<void>(0x028F026C, 0x101912A0u);
  store<f32>(0x10462284, 0.0f);
  store<f32>(0x1046227C, 0.0f);
  store<f32>(0x10462280, 0.0f);
  hioCtor(0x10462288);
}
VERIFY(0x02091648, staticInit);
u32 actorCtor(u32 a) {
  WWHD_FUNC(0x02090F08, u32, a);
  if (!a)
    a = call<u32>(0x0273AD10, 0x1238C);
  if (!a)
    return 0;
  call<void>(0x025D4ED0, a);
  store<u32>(a + 0xB4, 0x10009004);
  call<void>(0x02080404, a + 0x3D4);
  call<void>(0x02080404, a + 0x488);
  call<void>(0x025E9960, a + 0x19D8);
  call<void>(0x02080404, a + 0x1B20);
  call<void>(0x028EFFD0, a + 0x1BE8, 10, 0x19D8, 0x02090D4Cu);
  call<void>(0x0200BD2C, a + 0x11E9C);
  call<void>(0x02515DA0, a + 0x11EB8);
  store<u32>(a + 0x11EB4, 0x1004AE88);
  store<u32>(a + 0x11EB8, 0x1004AEC0);
  call<void>(0x025166F0, a + 0x11ED8);
  call<void>(0x025166F0, a + 0x12004);
  call<void>(0x025EB82C, a + 0x12140);
  call<void>(0x02080404, a + 0x122CC);
  return a;
}
VERIFY(0x02090F08, actorCtor);

bool draw(u32 a) {
  WWHD_FUNC(0x0208FC64, bool, a);
  u32 boss = call<u32>(0x025DE508, 0x0208FC14u, a);
  store<u32>(0x10462250, boss);
  if (boss && load<s8>(boss + 0x3D5) == 2 && load<u8>(0x104622C2)) {
    store<u8>(0x10462258, 188);
    store<u8>(0x10462259, 152);
    store<u8>(0x1046225A, 66);
    store<u8>(0x1046225B, 255);
    call<void>(0x025EA548, a + 0x19D8, 5, 0x10462258u, 3, a + 0x110,
               load<f32>(0x104622CC));
    u32 play = call<u32>(0x025200D4);
    u32 vt = load<u32>(a + 0x1B08);
    u32 index = gabi::call_ptr<u32>(load<u32>(vt + 0x14), a + 0x19D8);
    call<void>(0x025EDD04, play + 0x5FB4 + index * 0x9C, a + 0x19D8);
    for (int i = 0; i < 6; ++i) {
      u32 part = a + 0x1BE8 + 0x19D8 * i;
      call<void>(0x025EA548, part + 0x1890, 5, 0x10462258u, 3, a + 0x110,
                 load<f32>(0x104622CC));
      play = call<u32>(0x025200D4);
      vt = load<u32>(part + 0x19C0);
      index = gabi::call_ptr<u32>(load<u32>(vt + 0x14), part + 0x1890);
      call<void>(0x025EDD04, play + 0x5FB4 + index * 0x9C, part + 0x1890);
    }
  }
  return true;
}
VERIFY(0x0208FC64, draw);
bool execute(u32 a) {
  WWHD_FUNC(0x02090680, bool, a);
  call<u32>(0x025200D4);
  u32 boss = call<u32>(0x025DE508, 0x0208FC14u, a);
  store<u32>(0x10462250, boss);
  if (!boss)
    return true;
  u8 dbg = load<u8>(0x1046228A);
  if (dbg)
    store<u8>(a + 0x3A1, dbg);
  u32 bait = call<u32>(0x025DE508, 0x020900A4u, a);
  boss = load<u32>(0x10462250);
  store<u32>(0x10462254, bait);
  if (load<s8>(boss + 0x3D5) != 2) {
    store<s16>(a + 0x11E66, 10);
    store<f32>(a + 0x31C, 0.0f);
    store<u32>(a + 0x39C, 0);
    store<f32>(a + 0x314, 0.0f);
    store<f32>(a + 0x318, 30000.0f);
    return true;
  }
  u32 state = a + 0x11E64;
  if (load<s16>(state + 2) == 10) {
    u32 z = load<u32>(a + 0x2F4), x = load<u32>(a + 0x2EC);
    store<u32>(a + 0x31C, z);
    u32 y = load<u32>(a + 0x2F0);
    store<u32>(a + 0x314, x);
    store<u32>(a + 0x318, y);
    store<s16>(state + 2, 0);
    store<s16>(state + 0x28, 180);
    store<s16>(state + 8, 180);
  }
  store<u32>(a + 0x39C, 4);
  call<void>(0x0200ED84, state + 0x2D8, 0.0f, 1.0f, 0.01f);
  s16 counter = load<s16>(state);
  store<s16>(state, counter + 1);
  store<u8>(a + 0x38A, 0x22);
  for (int i = 0; i < 5; ++i) {
    s16 n = load<s16>(state + 0x22 + 2 * i);
    if (n)
      store<s16>(state + 0x22 + 2 * i, n - 1);
  }
  s16 t = load<s16>(state + 0x2C);
  if (t)
    store<s16>(state + 0x2C, t - 1);
  t = load<s16>(state + 0x2E);
  s16 first = load<s16>(state + 0xC);
  if (t)
    store<s16>(state + 0x2E, t - 1);
  s16 second = load<s16>(state + 0x14);
  if (first)
    store<s16>(state + 0xC, first - 1);
  if (second)
    store<s16>(state + 0x14, second - 1);
  call<void>(0x02091730, a);
  u32 count = load<u32>(state + 0x520);
  if (count) {
    store<u32>(state + 0x520, count - 1);
    store<u32>(0x1046225C, 0);
    call<u32>(0x025DE508, 0x02090624u, a);
    if (load<s32>(0x1046225C) < 20) {
      gabi::Local<f32[3]> pos;
      store<f32>(pos.a, call<f32>(0x02019918, 2500.0f));
      store<f32>(pos.a + 4, call<f32>(0x020198D8, 500.0f) + 3500.0f);
      store<f32>(pos.a + 8, call<f32>(0x02019918, 2500.0f));
      call<u32>(0x025D5834, 0xCD, 3, pos.a, load<s8>(a + 0x326), 0, 0, -1, 0);
    }
  }
  return true;
}
VERIFY(0x02090680, execute);

bool grCheck(u32 a, u32 pos) {
  WWHD_FUNC(0x0208FD9C, bool, a, pos);
  gabi::Local<u8[108]> line;
  gabi::Local<f32[3]> start, end;
  call<void>(0x02008FEC, line.a);
  f32 x = load<f32>(pos), y = load<f32>(pos + 4), z = load<f32>(pos + 8);
  store<u32>(line.a + 0x68, 1);
  store<u8>(line.a + 0x5D, 0);
  store<u8>(line.a + 0x62, 0);
  store<u8>(line.a + 0x60, 0);
  store<u32>(line.a + 0x64, 0x10008FD4);
  store<u32>(line.a + 0x20, 0x10008FC4);
  store<u8>(line.a + 0x61, 0);
  store<f32>(start.a + 8, z);
  store<f32>(end.a + 4, y - 1000.0f);
  store<f32>(start.a, x);
  store<u32>(line.a, line.a + 0x58);
  store<f32>(end.a + 8, z);
  store<u32>(line.a + 0x10, 0x10008FB4);
  store<u8>(line.a + 0x5F, 0);
  store<u32>(line.a + 4, line.a + 0x64);
  store<f32>(end.a, x);
  store<u8>(line.a + 0x5C, 0);
  store<u8>(line.a + 0x5E, 0);
  store<u32>(line.a + 0x58, 0x10008FE4);
  store<f32>(start.a + 4, y + 200.0f);
  call<void>(0x024F1AFC, line.a, start.a, end.a, a);
  s32 hit = call<s32>(0x02008860, call<u32>(0x025200D4) + 0x12A0, line.a);
  bool water = false;
  if (hit) {
    u32 zc = load<u32>(line.a + 0x38), xc = load<u32>(line.a + 0x30),
        yc = load<u32>(line.a + 0x34);
    store<u32>(pos, xc);
    store<u32>(pos + 4, yc);
    store<u32>(pos + 8, zc);
    store<f32>(pos + 4, load<f32>(0x1047B630) - 2.0f);
    water = call<s32>(0x024EF0F4, call<u32>(0x025200D4) + 0x12A0,
                      line.a + 0x14) == 0x13;
  }
  store<u32>(line.a + 0x58, 0x10008FE4);
  store<u32>(line.a + 0x64, 0x10008FA4);
  store<u32>(line.a + 0x20, 0x10008F94);
  call<void>(0x02008B4C, line.a, 0);
  return !water;
}
VERIFY(0x0208FD9C, grCheck);
void dropEffect(u32 a) {
  WWHD_FUNC(0x0208FF44, void, a);
  gabi::Local<f32[3]> pos;
  f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318);
  store<f32>(pos.a, x);
  f32 z = load<f32>(a + 0x31C);
  store<f32>(pos.a + 4, y);
  store<f32>(pos.a + 8, z);
  if (grCheck(a, pos.a))
    return;
  for (int i = 0; i < 3; ++i)
    call<u32>(0x025A847C, load<u32>(call<u32>(0x025200D4) + 0x5AB0), 0,
              0x8414 + i, pos.a, 0, 0, 255, 0, -1, 0, 0, 0);
  call<void>(0x025A8D40, load<u32>(call<u32>(0x025200D4) + 0x5AB0), 0x840E,
             pos.a, 255, 0x101D5E98u, 0x101D5E98u, 0);
  call<void>(0x025A8D40, load<u32>(call<u32>(0x025200D4) + 0x5AB0), 0x8408,
             pos.a, 255, 0x101D5E98u, 0x101D5E98u, 0);
}
VERIFY(0x0208FF44, dropEffect);
bool wall(u32 a) {
  WWHD_FUNC(0x020900F4, bool, a);
  gabi::Local<f32[3]> delta, v, out;
  call<void>(0x0201ADE0, a + 0x314, delta.a, a + 0x300);
  u32 z = load<u32>(delta.a + 8), x = load<u32>(delta.a);
  store<u32>(v.a + 8, z);
  store<u32>(v.a, x);
  u32 y = load<u32>(delta.a + 4);
  store<u32>(v.a + 4, y);
  store<f32>(v.a + 4, 0.0f);
  f32 sq = call<f32>(0x028E8DD0, v.a);
  f32 len = call<f32>(0x028F4384, sq);
  if (!(len > 0.0f))
    return false;
  gabi::Local<u8[108]> line;
  call<void>(0x02008FEC, line.a);
  store<u8>(line.a + 0x61, 0);
  store<u8>(line.a + 0x5E, 0);
  store<u32>(line.a + 4, line.a + 0x64);
  store<u8>(line.a + 0x5D, 0);
  store<u32>(line.a, line.a + 0x58);
  store<u32>(line.a + 0x58, 0x10008FE4);
  store<u8>(line.a + 0x5C, 0);
  store<u32>(line.a + 0x20, 0x10008FC4);
  store<u32>(line.a + 0x10, 0x10008FB4);
  u32 mat = load<u32>(0x1018C7B0);
  store<u8>(line.a + 0x60, 0);
  store<u8>(line.a + 0x62, 0);
  store<u32>(line.a + 0x64, 0x10008FD4);
  store<u8>(line.a + 0x5F, 0);
  store<u32>(line.a + 0x68, 1);
  s16 angle = call<s16>(0x020195B0, load<f32>(v.a), load<f32>(v.a + 8));
  call<void>(0x025F1884, mat, angle);
  f32 forward = load<f32>(0x1047BBB8) + 300.0f,
      up = load<f32>(0x1047B620) + 150.0f;
  store<f32>(v.a, 0.0f);
  store<f32>(v.a + 4, forward);
  store<f32>(v.a + 8, up);
  call<void>(0x0200FCD8, v.a, out.a);
  call<void>(0x028E8D88, out.a, a + 0x314, out.a);
  f32 px = load<f32>(a + 0x314), height = load<f32>(0x1047BBB8) + 300.0f;
  store<f32>(v.a, px);
  f32 py = load<f32>(a + 0x318);
  store<f32>(v.a + 4, py);
  f32 pz = load<f32>(a + 0x31C);
  store<f32>(v.a + 8, pz);
  store<f32>(v.a + 4, py + height);
  call<void>(0x024F1AFC, line.a, v.a, out.a, a);
  bool hit = call<s32>(0x02008860, call<u32>(0x025200D4) + 0x12A0, line.a) != 0;
  if (hit) {
    f32 oldx = load<f32>(a + 0x300), oldz = load<f32>(a + 0x308);
    store<f32>(a + 0x314, oldx);
    store<f32>(a + 0x31C, oldz);
  }
  store<u32>(line.a + 0x58, 0x10008FE4);
  store<u32>(line.a + 0x64, 0x10008FA4);
  store<u32>(line.a + 0x20, 0x10008F94);
  call<void>(0x02008B4C, line.a, 0);
  return hit;
}
VERIFY(0x020900F4, wall);

// Preserve the original add's first-NaN payload when mocked vector output
// and the previous point both contain NaNs; ordinary C++ may commute operands.
static f32 singleAdd(f32 a, f32 b) {
  if (std::isnan(a))
    return std::bit_cast<f32>(std::bit_cast<u32>(a) | 0x00400000u);
  if (std::isnan(b))
    return std::bit_cast<f32>(std::bit_cast<u32>(b) | 0x00400000u);
  return (f32)((f64)a + (f64)b);
}
void hairControl(u32 mat, u32 hair, s32 index, f32 length) {
  WWHD_FUNC(0x02090420, void, mat, hair, index, length);
  if (!load<u8>(0x104622C2))
    return;
  gabi::Local<f32[3]> direction, out;
  store<f32>(direction.a, 0.0f);
  store<f32>(direction.a + 4, 0.0f);
  store<f32>(direction.a + 8, length);
  u32 point = hair;
  for (int j = 4; j > 0; --j) {
    f32 step = load<f32>(0x1047BBB4) + 0.1f;
    f32 curx = load<f32>(point + 12), vy = load<f32>(hair + 0x7C),
        curz = load<f32>(point + 20), cury = load<f32>(point + 16);
    f32 gain = (f32)j * step;
    f32 prevz = load<f32>(point + 8);
    f32 yz = gabi::fmadds(vy, gain, cury);
    f32 bias = load<f32>(0x104622D0);
    f32 vz = load<f32>(hair + 0x80);
    f32 yy = yz + bias;
    f32 prevx = load<f32>(point), prevy = load<f32>(point + 4);
    f32 dz = gabi::fmadds(vz, gain, curz - prevz);
    f32 dy = yy - prevy;
    f32 vx = load<f32>(hair + 0x78);
    f32 dx = gabi::fmadds(vx, gain, curx - prevx);
    s16 rx = (s16)(0u - call<u32>(0x020195B0, dy, dz));
    f32 horizontal = call<f32>(0x028F4384, gabi::fmadds(dy, dy, dz * dz));
    s16 ry = call<s16>(0x020195B0, dx, horizontal);
    call<void>(0x025F18EC, load<u32>(0x1018C7B0), rx);
    call<void>(0x025F1C28, load<u32>(0x1018C7B0), ry);
    call<void>(0x0200FCD8, direction.a, out.a);
    point += 12;
    f32 ox = load<f32>(out.a), px = load<f32>(point - 12),
        py = load<f32>(point - 8);
    store<f32>(point, singleAdd(px, ox));
    f32 oy = load<f32>(out.a + 4);
    f32 pz = load<f32>(point - 4);
    store<f32>(point + 4, singleAdd(py, oy));
    f32 oz = load<f32>(out.a + 8);
    store<f32>(point + 8, singleAdd(pz, oz));
  }
  u32 lines = load<u32>(mat + 0x144);
  u32 dest = load<u32>(lines + (u32)index * 16);
  for (int i = 0; i < 5; ++i) {
    u32 x = load<u32>(hair + 12 * i);
    store<u32>(dest + 12 * i, x);
    u32 y = load<u32>(hair + 12 * i + 4);
    store<u32>(dest + 12 * i + 4, y);
    u32 z = load<u32>(hair + 12 * i + 8);
    store<u32>(dest + 12 * i + 8, z);
  }
}
VERIFY(0x02090420, hairControl);

static u32 resource(s32 index) {
  gabi::Local<u32[2]> name;
  store<u32>(name.a, 0x1000905C);
  store<u32>(name.a + 4, 0x10008F7C);
  return call<u32>(0x026066C4, load<u32>(0x101F4F28), name.a, index);
}
bool heapInit(u32 a) {
  WWHD_FUNC(0x02090984, bool, a);
  u32 head = resource(21), anim = resource(6);
  u32 morf = call<u32>(0x025E4F64, 0, head, 0, 0, anim, 2, 0, -1, 1, 0, 0,
                       0x11020203u, 1.0f);
  store<u32>(a + 0x3D0, morf);
  if (!morf || !load<u32>(morf + 0x90))
    return false;
  call<void>(0x0207FD38, a + 0x3D4, 0);
  u32 partData = resource(20);
  u32 model = call<u32>(0x025E38E0, partData, 0, 0x11020203u);
  store<u32>(a + 0x484, model);
  if (!model)
    return false;
  call<void>(0x0207FD38, a + 0x488, 0);
  if (!call<s32>(0x025E9B80, a + 0x19D8, 40, 5, 0))
    return false;
  for (int i = 0; i < 9; ++i) {
    u32 part = a + 0x1BE8 + 0x19D8 * i;
    model = call<u32>(0x025E38E0, partData, 0, 0x11020203u);
    store<u32>(part, model);
    if (!model)
      return false;
    call<void>(0x0207FD38, part + 4, 0);
    if (i < 7 && !call<s32>(0x025E9B80, part + 0x1890,
                            load<u16>(0x101911E4 + 2 * i), 5, 0))
      return false;
  }
  model = call<u32>(0x025E38E0, resource(24), 0, 0x11020203u);
  store<u32>(a + 0x1BD8, model);
  if (!model)
    return false;
  u32 weakB = resource(27);
  model = call<u32>(0x025E38E0, weakB, 0, 0x11020203u);
  store<u32>(a + 0x1BD4, model);
  if (!model)
    return false;
  u32 brk = call<u32>(0x0273AD10, 0x78);
  if (brk)
    brk = call<u32>(0x025E80D0, brk);
  store<u32>(a + 0x1BE0, brk);
  if (!brk)
    return false;
  anim = resource(39);
  if (!call<s32>(0x025E8154, load<u32>(a + 0x1BE0), weakB, anim, 1, 2, 0, -1, 0,
                 0, 1.0f))
    return false;
  u32 weakA = resource(30);
  model = call<u32>(0x025E38E0, weakA, 0, 0x11020203u);
  store<u32>(a + 0x1BD0, model);
  if (!model)
    return false;
  brk = call<u32>(0x0273AD10, 0x78);
  if (brk)
    brk = call<u32>(0x025E80D0, brk);
  store<u32>(a + 0x1BDC, brk);
  if (!brk)
    return false;
  anim = resource(42);
  if (!call<s32>(0x025E8154, load<u32>(a + 0x1BDC), weakA, anim, 1, 2, 0, -1, 0,
                 0, 1.0f))
    return false;
  call<void>(0x0207FD38, a + 0x1B20, 0);
  u32 rope = resource(46);
  if (!call<s32>(0x025EBA58, a + 0x12140, 1, 60, rope, 1))
    return false;
  call<void>(0x0207FD38, a + 0x122CC, 0);
  return true;
}
VERIFY(0x02090984, heapInit);
u32 partCtor(u32 a) {
  WWHD_FUNC(0x02090D4C, u32, a);
  if (!a)
    a = call<u32>(0x0273AD10, 0x19D8);
  if (!a)
    return 0;
  call<void>(0x02080404, a + 4);
  // The constructor retains a shared default tev snapshot for three blocks.
  f32 tev00 = load<f32>(0x1016E414 + 0x0);
  store<f32>(a + 0xB4, tev00);
  f32 tev04 = load<f32>(0x1016E414 + 0x4);
  store<f32>(a + 0xB8, tev04);
  f32 tev08 = load<f32>(0x1016E414 + 0x8);
  store<f32>(a + 0xBC, tev08);
  f32 tev0C = load<f32>(0x1016E414 + 0xC);
  store<f32>(a + 0xC0, tev0C);
  f32 tev10 = load<f32>(0x1016E414 + 0x10);
  store<f32>(a + 0xC4, tev10);
  f32 tev14 = load<f32>(0x1016E414 + 0x14);
  store<f32>(a + 0xC8, tev14);
  u8 tev18 = load<u8>(0x1016E414 + 0x18);
  store<u8>(a + 0xCC, tev18);
  u8 tev19 = load<u8>(0x1016E414 + 0x19);
  store<u8>(a + 0xCD, tev19);
  u8 tev1A = load<u8>(0x1016E414 + 0x1A);
  store<u8>(a + 0xCE, tev1A);
  u8 tev1B = load<u8>(0x1016E414 + 0x1B);
  store<u8>(a + 0xCF, tev1B);
  s16 tev1C = load<s16>(0x1016E414 + 0x1C);
  store<s16>(a + 0xD0, tev1C);
  s16 tev1E = load<s16>(0x1016E414 + 0x1E);
  store<s16>(a + 0xD2, tev1E);
  s16 tev20 = load<s16>(0x1016E414 + 0x20);
  store<s16>(a + 0xD4, tev20);
  s16 tev22 = load<s16>(0x1016E414 + 0x22);
  store<s16>(a + 0xD6, tev22);
  f32 tev24 = load<f32>(0x1016E414 + 0x24);
  store<f32>(a + 0xD8, tev24);
  f32 tev28 = load<f32>(0x1016E414 + 0x28);
  store<f32>(a + 0xDC, tev28);
  f32 tev2C = load<f32>(0x1016E414 + 0x2C);
  store<f32>(a + 0xE0, tev2C);
  f32 tev30 = load<f32>(0x1016E414 + 0x30);
  store<f32>(a + 0xE4, tev30);
  f32 tev34 = load<f32>(0x1016E414 + 0x34);
  store<f32>(a + 0xE8, tev34);
  f32 tev38 = load<f32>(0x1016E414 + 0x38);
  store<f32>(a + 0xEC, tev38);
  f32 tev3C = load<f32>(0x1016E414 + 0x3C);
  store<f32>(a + 0xF0, tev3C);
  f32 tev40 = load<f32>(0x1016E454);
  store<u8>(a + 0x18C, tev18);
  store<f32>(a + 0x1B4, tev40);
  store<u8>(a + 0x18E, tev1A);
  store<u8>(a + 0x212, tev1A);
  store<f32>(a + 0x17C, tev08);
  store<f32>(a + 0x184, tev10);
  store<u8>(a + 0x18F, tev1B);
  store<f32>(a + 0x1A4, tev30);
  store<f32>(a + 0x198, tev24);
  store<f32>(a + 0x180, tev0C);
  store<f32>(a + 0x208, tev10);
  store<u8>(a + 0x18D, tev19);
  store<f32>(a + 0x1A8, tev34);
  store<f32>(a + 0x1FC, tev04);
  store<f32>(a + 0x204, tev0C);
  store<f32>(a + 0x19C, tev28);
  store<u8>(a + 0x211, tev19);
  store<f32>(a + 0x20C, tev14);
  store<f32>(a + 0x178, tev04);
  store<u8>(a + 0x213, tev1B);
  store<s16>(a + 0x190, tev1C);
  store<f32>(a + 0x200, tev08);
  store<f32>(a + 0x1F8, tev00);
  store<f32>(a + 0x1A0, tev2C);
  store<f32>(a + 0x188, tev14);
  store<f32>(a + 0x1B0, tev3C);
  store<f32>(a + 0x174, tev00);
  store<f32>(a + 0xF4, tev40);
  store<f32>(a + 0x1AC, tev38);
  store<s16>(a + 0x194, tev20);
  store<s16>(a + 0x192, tev1E);
  store<s16>(a + 0x214, tev1C);
  store<u8>(a + 0x210, tev18);
  store<s16>(a + 0x196, tev22);
  store<s16>(a + 0x216, tev1E);
  store<s16>(a + 0x218, tev20);
  store<s16>(a + 0x21A, tev22);
  store<f32>(a + 0x21C, tev24);
  store<f32>(a + 0x220, tev28);
  store<f32>(a + 0x224, tev2C);
  store<f32>(a + 0x228, tev30);
  store<f32>(a + 0x22C, tev34);
  store<f32>(a + 0x230, tev38);
  store<f32>(a + 0x234, tev3C);
  store<f32>(a + 0x238, tev40);
  call<void>(0x025166F0, a + 0x2AC);
  call<void>(0x025E9960, a + 0x1890);
  return a;
}
VERIFY(0x02090D4C, partCtor);

static void copyActorTev(u32 a, u32 dest) {
  u32 src = a + 0x110;
  for (int o = 0; o < 24; o += 4)
    store<f32>(dest + o, load<f32>(src + o));
  for (int o = 0x18; o < 0x1C; ++o)
    store<u8>(dest + o, load<u8>(src + o));
  for (int o = 0x1C; o < 0x24; o += 2)
    store<s16>(dest + o, load<s16>(src + o));
  for (int o = 0x24; o < 0x44; o += 4)
    store<f32>(dest + o, load<f32>(src + o));
  for (int o = 0x84; o < 0x90; o += 4)
    store<u32>(dest + o, load<u32>(src + o));
  for (int o = 0x90; o < 0x98; o += 2)
    store<u16>(dest + o, load<u16>(src + o));
  store<u32>(dest + 0x98, load<u32>(src + 0x98));
  store<u32>(dest + 0x9C, load<u32>(src + 0x9C));
  for (int o = 0xA0; o < 0xA8; o += 2)
    store<u16>(dest + o, load<u16>(src + o));
  for (int o = 0xA8; o < 0xB4; o += 4)
    store<f32>(dest + o, load<f32>(src + o));
  for (int o = 0xB4; o < 0xBD; ++o)
    store<u8>(dest + o, load<u8>(src + o));
  for (int o = 0xC0; o < 0xD8; o += 4)
    store<f32>(dest + o, load<f32>(src + o));
  for (int o = 0xD8; o < 0xDC; ++o)
    store<u8>(dest + o, load<u8>(src + o));
  for (int o = 0xDC; o < 0xE4; o += 2)
    store<s16>(dest + o, load<s16>(src + o));
  for (int o = 0xE4; o < 0x104; o += 4)
    store<f32>(dest + o, load<f32>(src + o));
  for (int o = 0x144; o < 0x15C; o += 4)
    store<f32>(dest + o, load<f32>(src + o));
  for (int o = 0x15C; o < 0x160; ++o)
    store<u8>(dest + o, load<u8>(src + o));
  for (int o = 0x160; o < 0x168; o += 2)
    store<s16>(dest + o, load<s16>(src + o));
  for (int o = 0x168; o < 0x188; o += 4)
    store<f32>(dest + o, load<f32>(src + o));
}
s32 create(u32 a) {
  WWHD_FUNC(0x02091008, s32, a);
  u32 flags = load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (a) {
      actorCtor(a);
      flags = load<u32>(a + 0x2E4);
    }
    store<u32>(a + 0x2E4, flags | 8);
  }
  s32 phase = call<s32>(0x02520460, a + 0x3C8, 0x10009060u);
  if (phase != 4)
    return phase;
  if (!call<s32>(0x025D63E8, a, 0x02090984u, 0x96000))
    return 5;
  if (!load<u8>(0x1019119C)) {
    store<u8>(a + 0x12388, 1);
    store<u8>(0x1019119C, 1);
    store<u8>(0x10462288, call<u32>(0x025F0A10, 0x10009064u, 0x10462288u));
  }
  u32 st = a + 0x11E9C;
  call<void>(0x02515F14, st, 255, 255, a);
  call<void>(0x0251677C, a + 0x12004, 0x10191208u);
  u32 bits = load<u32>(a + 0x12004);
  store<u32>(a + 0x12048, st);
  store<u32>(a + 0x12004, bits & ~1u);
  call<void>(0x0251677C, a + 0x11ED8, 0x10191208u);
  bits = load<u32>(a + 0x11ED8);
  store<u32>(a + 0x11F1C, st);
  store<u32>(a + 0x11ED8, bits & ~1u);
  for (int i = 0; i < 9; ++i) {
    u32 part = a + 0x1BE8 + i * 0x19D8;
    call<void>(0x0251677C, part + 0x2AC, i == 8 ? 0x10191248u : 0x10191208u);
    bits = load<u32>(part + 0x2AC);
    store<u32>(part + 0x2F0, st);
    store<u32>(part + 0x2AC, bits & ~1u);
  }
  store<u8>(a + 0x3A0, 3);
  store<u8>(a + 0x3A1, 3);
  for (int i = 0; i < 9; ++i)
    copyActorTev(a, a + 0x1BE8 + 0x19D8 * i + 0xB4);
  execute(a);
  return phase;
}
VERIFY(0x02091008, create);
// Inlined movement states in the HD movement routine.
void moveShock(u32 a, s32 strength) {
  u32 play = call<u32>(0x025200D4);
  strength += load<s16>(0x1047B68C);
  gabi::Local<f32[3]> direction;
  store<f32>(direction.a, 0.0f);
  store<f32>(direction.a + 4, 1.0f);
  store<f32>(direction.a + 8, 0.0f);
  call<void>(0x025CB374, play + 0x599C, strength, -33, direction.a);
}
s16 centerAngle(u32 a) {
  f32 x = -load<f32>(a + 0x314);
  f32 z = -load<f32>(a + 0x31C);
  return call<s16>(0x020195B0, x, z);
}
void moveSound(u32 a, u32 id, u32 value) {
  if (a + 0x37C) {
    s32 reverb = call<s32>(0x02520540, load<s8>(a + 0x326));
    call<void>(0x025E1A40, id, a + 0x37C, value, reverb);
  }
}
void monsterSound(u32 a, u32 id) {
  if (a + 0x37C) {
    u32 identity = a ? load<u32>(a + 4) : 0xFFFFFFFFu;
    s32 reverb = call<s32>(0x02520540, load<s8>(a + 0x326));
    call<void>(0x025E1AA4, id, a + 0x37C, identity, 0, reverb);
  }
}
u32 soundVolume(f32 value) {
  u32 integer;
  if (!(value < 2147483648.0f))
    integer = (u32)gabi::ftoi(value - 2147483648.0f) + 0x80000000u;
  else
    integer = (u32)gabi::ftoi(value);
  return integer > 100 ? 100 : integer;
}
void splash(u32 a, u32 headEffect, u32 partEffect, bool ripples) {
  u32 s = a + 0x11E58;
  gabi::Local<u32[3]> p;
  if (((load<s16>(s + 0xC) + 2) & 3) == 0) {
    store<u32>(p.a, load<u32>(a + 0x314));
    store<u32>(p.a + 4, load<u32>(a + 0x318));
    store<u32>(p.a + 8, load<u32>(a + 0x31C));
    if (!grCheck(a, p.a)) {
      u32 play = call<u32>(0x025200D4);
      call<void>(0x025A8D40, load<u32>(play + 0x5AB0), headEffect, p.a, 255,
                 0x101D5E98u, 0x101D5E98u, 0);
      if (ripples && load<s16>(s + 0x18) == 0) {
        f32 random = call<f32>(0x020198D8, 20.0f);
        store<s16>(s + 0x18, (s16)gabi::ftoi(singleAdd(random, 20.0f)));
        moveSound(a, 0x6A44, 0);
      }
    }
  }
  for (int i = 0; i < 9; ++i) {
    if (((load<s16>(s + 0xC) + i) & (ripples ? 3 : 7)) == 0) {
      u32 part = a + 0x1BE8 + i * 0x19D8;
      store<u32>(p.a, load<u32>(part + 0x288));
      store<u32>(p.a + 4, load<u32>(part + 0x28C));
      store<u32>(p.a + 8, load<u32>(part + 0x290));
      if (!grCheck(a, p.a)) {
        u32 play = call<u32>(0x025200D4);
        call<void>(0x025A8D40, load<u32>(play + 0x5AB0), partEffect, p.a, 255,
                   0x101D5E98u, 0x101D5E98u, 0);
        if (ripples && i == 8 && load<s16>(s + 0x18) == 0) {
          f32 random = call<f32>(0x020198D8, 20.0f);
          store<s16>(s + 0x18, (s16)gabi::ftoi(singleAdd(random, 20.0f)));
          s32 reverb = call<s32>(0x02520540, load<s8>(a + 0x326));
          call<void>(0x025E1A40, 0x6A45u, s + 0x2D8, 0, reverb);
        }
      }
    }
  }
}
void motionSound(u32 a) {
  gabi::Local<f32[3]> delta, output;
  call<void>(0x0201ADE0, a + 0x314, delta.a, a + 0x300);
  store<f32>(delta.a + 4, 0.0f);
  f32 square = call<f32>(0x028E8DD0, delta.a);
  f32 length = call<f32>(0x028F4384, square);
  u32 volume = soundVolume(length * 3.5f);
  if (a && a + 0x37C)
    moveSound(a, 0x705A, volume);
  call<void>(0x0201ADE0, a + 0x314, output.a, a + 0x300);
  store<u32>(delta.a + 4, load<u32>(output.a + 4));
  store<f32>(delta.a, 0.0f);
  store<f32>(delta.a + 8, 0.0f);
  square = call<f32>(0x028E8DD0, delta.a);
  length = call<f32>(0x028F4384, square);
  volume = soundVolume(singleAdd(length, length));
  if (a && a + 0x37C)
    moveSound(a, 0x705B, volume);
}
bool movingState(u32 a) {
  u32 s = a + 0x11E58;
  call<u32>(0x025200D4);
  u32 bait = load<u32>(0x10462254);
  s16 turn = 0;
  s16 center = 0;
  bool chase = false;
  if (bait) {
    turn = load<s16>(s + 0x12);
    store<s16>(s + 0x14, 1);
    center = load<s16>(s + 0x34);
    chase = true;
  } else {
    s16 timer = load<s16>(s + 0x14);
    if (timer) {
      turn = load<s16>(s + 0x12);
      store<s16>(s + 0x14, (s16)(timer - 1));
      center = load<s16>(s + 0x34);
      chase = true;
    } else {
      store<s16>(a + 0x322, (s16)(load<s16>(a + 0x322) + load<s16>(s + 0x12)));
    }
  }
  if (chase) {
    s16 amount = turn < 0 ? (s16)(0u - (u32)turn) : turn;
    if (center) {
      s16 heading = centerAngle(a);
      call<void>(0x0200F428, a + 0x322, heading, 4, amount);
    } else if ((bait = load<u32>(0x10462254))) {
      gabi::Local<f32[3]> delta;
      call<void>(0x0201ADE0, bait + 0x314, delta.a, a + 0x314);
      f32 x = load<f32>(delta.a);
      f32 z = load<f32>(delta.a + 8);
      s16 heading = call<s16>(0x020195B0, x, z);
      store<f32>(s + 0x40, singleAdd(load<f32>(0x1047BBCC), 30.0f));
      call<void>(0x0200F428, a + 0x322, heading, 4, (s16)(amount * 2));
    } else {
      u32 play = call<u32>(0x025200D4);
      s16 heading = call<s16>(0x025D6894, a, load<u32>(play + 0x5B2C));
      s32 step = load<s16>(0x1047B69A) + 0x900;
      s16 counter = load<s16>(s + 0xC);
      f32 amplitude = singleAdd(load<f32>(0x1047B658), 4000.0f);
      f32 sine =
          load<f32>(0x104A44F8 + (((u32)(counter * step) & 65535) >> 3) * 8);
      s16 wobble = (s16)gabi::ftoi(sine * amplitude);
      call<void>(0x0200F428, a + 0x322, (s16)(heading + wobble), 4, amount);
    }
  }
  s8 health = load<s8>(a + 0x3A1);
  f32 speed = load<f32>(0x10462288 + (health == 3 ? 4 : health == 2 ? 12 : 20));
  call<void>(0x0200ED84, a + 0x370, speed, 1.0f, 3.0f);
  position(a);
  ground(a);
  bool struckWall = false;
  if (wall(a) && load<s16>(s + 0x30) == 0) {
    store<s16>(s + 0x14, 0);
    s16 heading = centerAngle(a);
    store<s16>(a + 0x322, heading);
    f32 stiffness = singleAdd(load<f32>(0x1047B624), 0.9f);
    store<s16>(s + 0x30, 20);
    store<f32>(s + 0x3C, stiffness);
    struckWall = true;
    u32 play = call<u32>(0x025200D4);
    s32 strength = load<s16>(0x1047B68C) + 5;
    gabi::Local<f32[3]> direction;
    store<f32>(direction.a + 8, 0.0f);
    store<f32>(direction.a + 4, 1.0f);
    store<f32>(direction.a, 0.0f);
    call<void>(0x025CB374, play + 0x599C, strength, -33, direction.a);
    moveSound(a, 0x5981, 0);
    store<s32>(s + 0x52C, load<s16>(0x104622C0));
    health = load<s8>(a + 0x3A1);
    s16 threshold = load<s16>(0x10462288 + (health == 3   ? 10
                                            : health == 2 ? 18
                                                          : 26));
    s16 count = (s16)(load<s16>(s + 0x16) + 1);
    if (count >= threshold) {
      store<s16>(s + 0x16, 0);
      health = load<s8>(a + 0x3A1);
      store<s16>(s + 0x14, load<s16>(0x10462288 + (health == 3   ? 28
                                                   : health == 2 ? 30
                                                                 : 32)));
    } else {
      store<s16>(s + 0x16, count);
    }
  }
  bool newTurn = struckWall;
  if (load<s16>(s + 0x32) == 0) {
    f32 random = call<f32>(0x020198D8, 50.0f);
    store<s16>(s + 0x32, (s16)gabi::ftoi(singleAdd(random, 30.0f)));
    health = load<s8>(a + 0x3A1);
    newTurn = true;
  }
  if (newTurn) {
    store<s16>(s + 0x12, load<s16>(0x10462288 + (health == 3   ? 8
                                                 : health == 2 ? 16
                                                               : 24)));
    f32 random = call<f32>(0x020198D8, 1.0f);
    if (random < 0.5f)
      store<s16>(s + 0x12, (s16)(0u - (u32)load<s16>(s + 0x12)));
  }
  u32 second = load<u32>(s + 0x1AC);
  u32 first = load<u32>(s + 0x80);
  store<u32>(s + 0x1AC, second | 1);
  store<u32>(s + 0x80, first | 1);
  store<s16>(a + 0x320, 0);
  if (load<f32>(a + 0x370) > 10.0f) {
    splash(a, 0x840E, 0x840D, false);
    moveSound(a, 0x705D, 0);
    motionSound(a);
    return true;
  }
  return false;
}
void stunnedState(u32 a) {
  u32 s = a + 0x11E58;
  call<u32>(0x025200D4);
  u16 phase = load<u16>(s + 0x10);
  if (phase == 0) {
    store<s16>(s + 0x10, 1);
    phase = 1;
  }
  if (phase == 1) {
    call<void>(0x025E535C, load<u32>(a + 0x3D0), a + 0x314, 0, 0);
    if (load<s16>(s + 0x2E) == 0) {
      store<s16>(s + 0xE, 0);
      store<s16>(s + 0x10, 0);
    }
  }
  call<void>(0x0200EDC8, a + 0x370, 1.0f, 3.0f);
  position(a);
  store<s16>(a + 0x320, 0);
  ground(a);
  if (wall(a) && load<s16>(s + 0x30) == 0) {
    store<s16>(s + 0x14, 0);
    s16 heading = centerAngle(a);
    store<s16>(a + 0x322, heading);
    store<f32>(s + 0x3C, singleAdd(load<f32>(0x1047B624), 0.9f));
  }
}
void movementPitch(u32 a) {
  gabi::Local<f32[3]> difference;
  call<void>(0x0201ADE0, a + 0x314, difference.a, a + 0x300);
  f32 z = load<f32>(difference.a + 8);
  f32 x = load<f32>(difference.a);
  f32 y = load<f32>(difference.a + 4);
  f32 square = gabi::fmadds(x, x, z * z);
  f32 horizontal = call<f32>(0x028F4384, square);
  s16 angle = (s16)(0u - call<u32>(0x020195B0, y, horizontal));
  if (angle > 0x3000)
    angle = 0x3000;
  else if (angle < -0x3000)
    angle = -0x3000;
  store<s16>(a + 0x320, angle);
}
void damagedState(u32 a) {
  u32 s = a + 0x11E58;
  call<u32>(0x025200D4);
  u16 phase = load<u16>(s + 0x10);
  if (phase == 0) {
    store<f32>(a + 0x340, 150.0f);
    store<f32>(a + 0x370, 30.0f);
    store<f32>(s + 0x3C, 1.0f);
    store<s16>(s + 0x10, 1);
  } else if (phase == 1) {
    f32 target = singleAdd(load<f32>(0x1047B634), 350.0f);
    f32 step = singleAdd(load<f32>(0x1047B638), 50.0f);
    call<void>(0x0200ED84, s + 0x470, target, 1.0f, step);
    if (ground(a)) {
      store<f32>(a + 0x340, 50.0f);
      store<s16>(s + 0x10, 2);
      f32 random = call<f32>(0x020198D8, 65536.0f);
      store<s16>(a + 0x322, (s16)gabi::ftoi(random));
      store<f32>(s + 0x3C, 1.0f);
      moveShock(a, 5);
      dropEffect(a);
    }
  } else if (phase == 2 && ground(a)) {
    store<s16>(s + 0xE, 0);
    store<s16>(s + 0x10, 0);
  }
  position(a);
  movementPitch(a);
  ground(a);
  if (wall(a) && load<s16>(s + 0x30) == 0) {
    store<s16>(a + 0x322, centerAngle(a));
    store<s16>(s + 0x30, 20);
  }
  u32 first = load<u32>(s + 0x80);
  u32 second = load<u32>(s + 0x1AC);
  store<u32>(s + 0x80, first & ~1u);
  store<u32>(s + 0x1AC, second & ~1u);
}
void dyingState(u32 a) {
  u32 s = a + 0x11E58;
  u32 play = call<u32>(0x025200D4);
  u32 player = load<u32>(play + 0x5B2C);
  u16 phase = load<u16>(s + 0x10);
  if (phase == 0) {
    store<f32>(a + 0x340, 150.0f);
    store<f32>(a + 0x370, 0.0f);
    store<s16>(s + 0x10, 1);
    store<u8>(load<u32>(0x10462250) + 0x151CE, 20);
    store<f32>(s + 0x3C, 1.0f);
    if (player && player + 0x37C)
      monsterSound(player, 0x4830);
  } else if (phase >= 1 && phase <= 4) {
    f32 height = load<f32>(a + 0x318) - 150.0f;
    if (height > 350.0f)
      height = 350.0f;
    store<f32>(s + 0x470, height);
    if (ground(a)) {
      store<s16>(s + 0x10, (s16)(load<s16>(s + 0x10) + 1));
      f32 random = call<f32>(0x020198D8, 65536.0f);
      store<s16>(a + 0x322, (s16)gabi::ftoi(random));
      store<f32>(s + 0x3C, 1.0f);
      moveShock(a, 5);
      dropEffect(a);
      store<f32>(a + 0x340, singleAdd(load<f32>(0x1047B63C), 130.0f));
      store<f32>(a + 0x370, 0.0f);
      random = call<f32>(0x020198D8, 65536.0f);
      store<s16>(a + 0x322, (s16)gabi::ftoi(random));
      monsterSound(a, 0x496E);
    }
    if (load<s16>(s + 0x10) == 4 && !(load<f32>(a + 0x340) > -30.0f)) {
      store<s16>(s + 0x10, 5);
      store<s16>(s + 0x12, 0);
    }
  } else if (phase == 5) {
    f32 rise = singleAdd(load<f32>(0x1047B640), 5.5f);
    f32 speed = singleAdd(load<f32>(a + 0x340), rise);
    store<f32>(a + 0x340, speed);
    if (speed > 0.0f)
      call<void>(0x0200ED84, s + 0x2E4, 1.0f, 1.0f, 0.015f);
    store<s16>(a + 0x322, (s16)(load<s16>(a + 0x322) + load<s16>(s + 0x12)));
    store<s16>(a + 0x32A, (s16)(load<s16>(a + 0x32A) + load<s16>(s + 0x12)));
    call<void>(0x0200F428, s + 0x12, 5000, 4, 100);
    f32 target = singleAdd(load<f32>(0x1047B628), 50.0f);
    call<void>(0x0200ED84, s + 0x40, target, 1.0f, 1.0f);
    f32 step = singleAdd(load<f32>(0x1047B638), 50.0f);
    call<void>(0x0200EDC8, s + 0x470, 1.0f, step);
  }
  position(a);
  movementPitch(a);
  ground(a);
  u32 second = load<u32>(s + 0x1AC);
  u32 first = load<u32>(s + 0x80);
  store<u32>(s + 0x80, first & ~1u);
  store<u32>(s + 0x1AC, second & ~1u);
  call<void>(0x0200ED84, a + 0x314, -15.0f, 0.05f, 30.0f);
  call<void>(0x0200ED84, a + 0x31C, 375.17f, 0.05f, 30.0f);
}

u32 movementParticle(u32 position, u32 effect, u32 rotation = 0, u32 scale = 0,
                     u32 managerOffset = 0x5AB0) {
  u32 play = call<u32>(0x025200D4);
  return call<u32>(0x025A847C, load<u32>(play + managerOffset), 0, effect,
                   position, rotation, scale, 255, 0, -1, 0, 0, 0);
}
void emitterScale(u32 emitter, f32 size) {
  store<f32>(emitter + 0x220, size);
  store<f32>(emitter + 0x224, size);
  store<f32>(emitter + 0x228, size);
  store<f32>(emitter + 0x238, size);
  store<f32>(emitter + 0x23C, size);
  store<f32>(emitter + 0x240, size);
}
void emitterMove(u32 ownerSlot, u32 position, f32 scale) {
  u32 emitter = load<u32>(ownerSlot);
  u8 type = load<u8>(emitter + 0x262);
  f32 y = load<f32>(position + 4);
  f32 x = load<f32>(position);
  f32 z = load<f32>(position + 8);
  if (type >= 7)
    y = -y;
  store<f32>(emitter + 0x22C, x);
  store<f32>(emitter + 0x230, y);
  store<f32>(emitter + 0x234, z);
  emitterScale(load<u32>(ownerSlot), scale);
}
void retireEmitter(u32 ownerSlot) {
  u32 emitter = load<u32>(ownerSlot);
  if (emitter) {
    u32 flags = load<u32>(emitter + 0x254);
    store<s32>(emitter + 0x5C, -1);
    store<u32>(emitter + 0x254, flags | 1);
    store<u32>(ownerSlot, 0);
  }
}
void hitControl(u32 a) {
  u32 s = a + 0x11E58;
  if (load<s16>(s + 0x38))
    return;
  gabi::Local<u8[20]> hit;
  if (call<s32>(0x025162A4, s + 0x80)) {
    u32 object = call<u32>(0x02516300, s + 0x80);
    store<u32>(hit.a, object);
    u32 source = call<u32>(0x02518DB0, hit.a);
    store<u32>(hit.a + 4, source);
    if (a && a + 0x37C) {
      monsterSound(a, 0x4973);
      moveSound(a, 0x5982, 0);
    }
    u8 kind = load<u8>(hit.a + 10);
    s32 severity;
    s16 duration;
    if (kind == 9) {
      severity = 6;
      u32 play = call<u32>(0x025200D4);
      gabi::Local<f32[3]> direction;
      store<f32>(direction.a, 0.0f);
      store<f32>(direction.a + 4, 1.0f);
      store<f32>(direction.a + 8, 0.0f);
      call<void>(0x025CB374, play + 0x599C, load<s16>(0x1047B68C) + 3, -33,
                 direction.a);
      store<u8>(0x101EACB7, 5);
      store<s16>(s + 0x528, 30);
      duration = load<s16>(0x104622B4);
    } else if (kind == 2) {
      store<s16>(s + 0x528, 25);
      severity = 5;
      duration = load<s16>(0x104622B2);
    } else if (kind == 1) {
      u32 objectInfo = call<u32>(0x025157AC, load<u32>(hit.a));
      if (load<u8>(objectInfo + 0x6F) == 1) {
        store<u8>(0x101EACB7, 3);
        store<s16>(s + 0x528, 20);
        severity = 4;
        duration = load<s16>(0x104622B0);
      } else {
        store<u8>(0x101EACB7, 2);
        store<s16>(s + 0x528, 15);
        severity = 3;
        duration = load<s16>(0x104622AE);
      }
    } else if (load<u32>(hit.a + 4) &&
               load<s16>(load<u32>(hit.a + 4) + 8) == 0x1D8) {
      store<s16>(s + 0x528, 10);
      severity = 2;
      duration = load<s16>(0x104622AC);
    } else {
      store<s16>(s + 0x528, 7);
      severity = 1;
      duration = load<s16>(0x104622AA);
    }
    if (load<s16>(s + 0xE) != 1) {
      store<s16>(s + 0x2E, duration);
      store<s16>(s + 0xE, 1);
    }
    store<s16>(s + 0x10, 0);
    store<s16>(s + 0x38, 20);
    if (severity >= 4) {
      movementParticle(s + 0x14C, 0x10);
      gabi::Local<f32[3]> size;
      gabi::Local<s16[3]> angle;
      store<f32>(size.a, 2.0f);
      store<f32>(size.a + 8, 2.0f);
      store<f32>(size.a + 4, 2.0f);
      store<s16>(angle.a + 4, 0);
      store<s16>(angle.a, 0);
      u32 play = call<u32>(0x025200D4);
      s16 heading = call<s16>(0x025D6894, a, load<u32>(play + 0x5B2C));
      store<s16>(angle.a + 2, heading);
      movementParticle(s + 0x14C, 0xD, angle.a, size.a);
      gabi::Local<f32[3]> p;
      store<f32>(p.a, load<f32>(s + 0x14C));
      store<f32>(p.a + 4, load<f32>(s + 0x150));
      store<f32>(p.a + 8, load<f32>(s + 0x154));
      call<void>(0x0255F554, p.a, 1);
    }
    call<void>(0x02518CC8, a, load<u32>(hit.a), 0x40);
    return;
  }
  if (call<s32>(0x025162A4, a + 0xED54)) {
    store<s16>(s + 0x38, 20);
    store<s16>(s + 0x22, 1);
    store<s16>(s + 0xE, 2);
    store<s16>(s + 0x10, 0);
    s8 health = load<s8>(a + 0x3A1);
    if (health) {
      store<s8>(a + 0x3A1, (s8)(health - 1));
      s32 reverb = call<s32>(0x02520540, load<s8>(a + 0x326));
      call<void>(0x025E1A40, 0x2879u, 0, 0x35, reverb);
      f32 size = singleAdd(load<f32>(0x1047B624), 2.0f);
      health = load<s8>(a + 0x3A1);
      u32 play = call<u32>(0x025200D4);
      if (health == 0) {
        store<f32>(play + 0x5B44, 0.0f);
        call<void>(0x025E1904, 30);
        u32 emitter = movementParticle(s + 0x2D8, 0x8457);
        if (emitter)
          emitterScale(emitter, size);
        emitter = movementParticle(s + 0x2D8, 0x8458);
        if (emitter)
          emitterScale(emitter, size);
        store<s16>(s + 0xE, 3);
        store<s16>(s + 0x10, 0);
        store<s16>(s + 0x38, 10000);
        reverb = call<s32>(0x02520540, load<s8>(a + 0x326));
        call<void>(0x025E1A7C, 0x4970u, 0, 0, reverb);
        reverb = call<s32>(0x02520540, load<s8>(a + 0x326));
        call<void>(0x025E1A40, 0x5983u, 0, 0, reverb);
      } else {
        u32 emitter = call<u32>(0x025A847C, load<u32>(play + 0x5AB0), 0, 0x8459,
                                s + 0x2D8, 0, 0, 255, 0, -1, 0, 0, 0);
        if (emitter)
          emitterScale(emitter, size);
        reverb = call<s32>(0x02520540, load<s8>(a + 0x326));
        call<void>(0x025E1A7C, 0x496Eu, 0, 0, reverb);
      }
    }
  }
  bool hitAny = false;
  if (call<s32>(0x025162A4, s + 0x1AC)) {
    store<u32>(hit.a, call<u32>(0x02516300, s + 0x1AC));
    hitAny = true;
  }
  for (int i = 0; i < 8; ++i) {
    u32 sphere = a + 0x1BE8 + i * 0x19D8 + 0x2AC;
    if (call<s32>(0x025162A4, sphere)) {
      store<u32>(hit.a, call<u32>(0x02516300, sphere));
      hitAny = true;
    }
  }
  if (hitAny && load<s16>(s + 0x3A) == 0) {
    store<s16>(s + 0x3A, 10);
    call<void>(0x02518CC8, a, load<u32>(hit.a), 0x44);
  }
}

void copyModelMatrix(u32 model, bool alternateOrder = false) {
  u32 matrix = load<u32>(0x1018C7B0);
  f32 cells[12];
  for (int i = 0; i < 12; ++i)
    cells[i] = load<f32>(matrix + i * 4);
  if (alternateOrder) {
    for (int i : {5, 6, 7, 9, 10, 11, 0, 1, 2, 3, 4, 8})
      store<f32>(model + 0xC8 + i * 4, cells[i]);
  } else {
    for (int i : {0, 1, 2, 3, 4, 8, 5, 6, 7, 9, 10, 11})
      store<f32>(model + 0xC8 + i * 4, cells[i]);
  }
}
void bodyControl(u32 a) {
  u32 s = a + 0x11E58;
  call<void>(0x0200F428, a + 0x32A, load<s16>(a + 0x322), 4, 0x1000);
  call<void>(0x0200F428, a + 0x328, load<s16>(a + 0x320), 8, 0x400);
  call<void>(0x025F1884, load<u32>(0x1018C7B0), load<s16>(a + 0x32A));
  call<void>(0x025F1BF4, load<u32>(0x1018C7B0), load<s16>(a + 0x328));
  gabi::Local<f32[3]> direction, delta, output;
  store<f32>(direction.a, 0.0f);
  store<f32>(direction.a + 4, 0.0f);
  store<f32>(direction.a + 8, singleAdd(load<f32>(0x1047B61C), -300.0f));
  call<void>(0x0200FCD8, direction.a, delta.a);
  call<void>(0x0201AD78, a + 0x314, output.a, delta.a);
  store<u32>(s, load<u32>(output.a));
  store<u32>(s + 4, load<u32>(output.a + 4));
  store<u32>(s + 8, load<u32>(output.a + 8));
  for (int i = 0; i < 9; ++i) {
    f32 size = singleAdd(load<f32>(0x1047B63C), 1.7f);
    f32 factor = load<f32>(0x101911C0 + i * 4);
    store<f32>(a + 0x1BE8 + i * 0x19D8 + 0x2A8, factor * size);
  }
  f32 separation = singleAdd(load<f32>(0x1047B630), 150.0f);
  f32 stiffness = load<f32>(s + 0x3C);
  gabi::Local<u8[108]> line;
  call<void>(0x02008FEC, line.a);
  f32 gravity = singleAdd(load<f32>(0x1047B628), -5.0f);
  for (u32 i = 0; i < 8; ++i)
    store<u8>(line.a + 0x5C + i, 0);
  store<u32>(line.a, line.a + 0x58);
  store<u32>(line.a + 4, line.a + 0x64);
  store<u32>(line.a + 0x10, 0x10008FB4);
  store<u32>(line.a + 0x68, 1);
  store<u32>(line.a + 0x64, 0x10008FD4);
  store<u32>(line.a + 0x58, 0x10008FE4);
  store<u32>(line.a + 0x20, 0x10008FC4);
  u32 firstPart = a + 0x1BE8;
  store<u32>(firstPart + 0x288, load<u32>(s));
  store<u32>(firstPart + 0x28C, load<u32>(s + 4));
  store<u32>(firstPart + 0x290, load<u32>(s + 8));
  call<void>(0x025F1884, load<u32>(0x1018C7B0), load<s16>(a + 0x32A));
  call<void>(0x025F1BF4, load<u32>(0x1018C7B0), load<s16>(a + 0x328));
  store<f32>(direction.a, 0.0f);
  store<f32>(direction.a + 4, 0.0f);
  store<f32>(direction.a + 8, -load<f32>(s + 0x40));
  call<void>(0x0200FCD8, direction.a, delta.a);
  for (int i = 1; i < 10; ++i) {
    u32 part = firstPart + i * 0x19D8;
    u32 prev = part - 0x19D8;
    f32 floor =
        singleAdd(load<f32>(0x1047B65C), 70.0f) * load<f32>(prev + 0x2A8);
    f32 y = singleAdd(singleAdd(load<f32>(part + 0x28C), gravity),
                      load<f32>(part + 0x2A0));
    f32 x = load<f32>(part + 0x288);
    f32 prevZ = load<f32>(prev + 0x290);
    f32 prevX = load<f32>(prev + 0x288);
    if (!(y > floor))
      y = floor;
    f32 z = load<f32>(part + 0x290);
    f32 dx = singleAdd(x - prevX, load<f32>(part + 0x29C));
    f32 dz = singleAdd(z - prevZ, load<f32>(part + 0x2A4));
    dx = singleAdd(dx, load<f32>(delta.a));
    dz = singleAdd(dz, load<f32>(delta.a + 8));
    f32 dy = y - load<f32>(prev + 0x28C);
    s16 heading = call<s16>(0x020195B0, dx, dz);
    store<s16>(prev + 0x296, heading);
    f32 horizontal = call<f32>(0x028F4384, gabi::fmadds(dx, dx, dz * dz));
    u32 pitch = call<u32>(0x020195B0, dy, horizontal);
    store<s16>(prev + 0x294, (s16)(0u - pitch));
    call<void>(0x025F1884, load<u32>(0x1018C7B0), load<s16>(prev + 0x296));
    call<void>(0x025F1BF4, load<u32>(0x1018C7B0), load<s16>(prev + 0x294));
    store<f32>(direction.a + 4, 0.0f);
    store<f32>(direction.a, 0.0f);
    store<f32>(direction.a + 8, separation * load<f32>(prev + 0x2A8));
    gabi::Local<f32[3]> segment;
    call<void>(0x0200FCD8, direction.a, segment.a);
    u32 oldX = load<u32>(part + 0x288);
    u32 oldY = load<u32>(part + 0x28C);
    store<u32>(part + 0x29C, oldX);
    u32 oldZ = load<u32>(part + 0x290);
    store<u32>(part + 0x2A0, oldY);
    store<u32>(part + 0x2A4, oldZ);
    call<void>(0x0201AD78, prev + 0x288, output.a, segment.a);
    store<u32>(part + 0x288, load<u32>(output.a));
    f32 vx = load<f32>(part + 0x288) - load<f32>(part + 0x29C);
    store<u32>(part + 0x28C, load<u32>(output.a + 4));
    store<u32>(part + 0x290, load<u32>(output.a + 8));
    f32 vy = load<f32>(part + 0x28C) - load<f32>(part + 0x2A0);
    f32 vz = load<f32>(part + 0x290) - load<f32>(part + 0x2A4);
    store<f32>(part + 0x29C, vx * stiffness);
    store<f32>(part + 0x2A0, vy * stiffness);
    store<f32>(part + 0x2A4, vz * stiffness);
  }
  call<void>(0x0200ED84, s + 0x3C, load<f32>(0x104622BC), 1.0f, 0.002f);
  call<void>(0x0200ED84, s + 0x40, load<f32>(0x104622B8), 1.0f, 0.5f);
  store<u32>(line.a + 0x58, 0x10008FE4);
  store<u32>(line.a + 0x64, 0x10008FA4);
  store<u32>(line.a + 0x20, 0x10008F94);
  call<void>(0x02008B4C, line.a, 0);
}
void hairRotations(int i, bool reverse) {
  s32 x = i * (load<s16>(0x1047B690) + 1000);
  x &= load<s16>(0x1047B692) + 0x3FFF;
  x -= load<s16>(0x1047B694) + 0x2000;
  if (reverse)
    x = -x;
  call<void>(0x025F1BF4, load<u32>(0x1018C7B0), (s16)x);
  s32 z = i * (load<s16>(0x1047B696) + 5000);
  z &= load<s16>(0x1047B698) + 0xFFF;
  z -= load<s16>(0x1047B69A) + 0x800;
  if (reverse)
    z = -z;
  call<void>(0x025F1C5C, load<u32>(0x1018C7B0), (s16)z);
}

void animatedBody(u32 a, bool moving) {
  u32 s = a + 0x11E58;
  gabi::Local<f32[3]> vector, world;
  call<void>(0x025E535C, load<u32>(a + 0x3D0), a + 0x314, 0, 0);
  call<void>(0x025E742C, load<u32>(a + 0x1BDC));
  call<void>(0x025E742C, load<u32>(a + 0x1BE0));
  f32 y = load<f32>(a + 0x318);
  f32 x = load<f32>(a + 0x314);
  f32 z = load<f32>(a + 0x31C);
  call<void>(0x0200FAD8, 0, x, y, z);
  call<void>(0x025F1C28, load<u32>(0x1018C7B0), load<s16>(a + 0x32A));
  call<void>(0x025F1BF4, load<u32>(0x1018C7B0), load<s16>(a + 0x328));
  call<void>(0x025F1C5C, load<u32>(0x1018C7B0), load<s16>(a + 0x32C));
  call<void>(0x0200FCF0);
  f32 size = singleAdd(load<f32>(0x1047B638), 2.0f);
  u32 model = load<u32>(a + 0x484);
  call<void>(0x0200FC74, 1, size, size, size);
  copyModelMatrix(model);
  store<f32>(vector.a, 0.0f);
  store<f32>(vector.a + 4, 0.0f);
  store<f32>(vector.a + 8, 0.0f);
  call<void>(0x0200FCD8, vector.a, world.a);
  call<void>(0x02018D40, s + 0x2C4, world.a);
  call<void>(0x02018C8C, s + 0x2C4, singleAdd(load<f32>(0x1047B614), 200.0f));
  call<void>(0x0200E240, call<u32>(0x025200D4) + 0x26A4, s + 0x1AC);
  bool effect;
  s16 timer;
  if (load<s16>(s + 0x22) == 23) {
    store<s16>(s + 0x20, 50);
    store<s16>(s + 0x2C, 99);
    timer = 99;
    effect = true;
  } else {
    timer = load<s16>(s + 0x2C);
    effect = timer != 0;
    if (effect) {
      timer = (s16)(timer - 1);
      store<s16>(s + 0x2C, timer);
    }
  }
  if (effect) {
    f32 factor = singleAdd(load<f32>(0x1047BAB0), 0.001f);
    f32 scale = (100.0f * (f32)timer) * factor;
    if (load<u32>(s + 0x24) == 0)
      store<u32>(s + 0x24, movementParticle(world.a, 0x3ED));
    else
      emitterMove(s + 0x24, world.a, scale);
    if (load<u32>(s + 0x28) == 0) {
      store<f32>(vector.a + 4, scale);
      store<f32>(vector.a + 8, scale);
      store<f32>(vector.a, scale);
      store<u32>(s + 0x28, movementParticle(world.a, 0x3EE));
    } else {
      emitterMove(s + 0x28, world.a, scale);
    }
  } else {
    retireEmitter(s + 0x24);
    retireEmitter(s + 0x28);
  }
  call<void>(0x0200FD38);
  for (int i = 0; i < 40; ++i) {
    call<void>(0x0200FCF0);
    hairRotations(i, true);
    store<f32>(vector.a + 4, singleAdd(load<f32>(0x1047BD94), 100.0f));
    u32 hair = a + 0x538 + i * 0x84;
    call<void>(0x0200FCD8, vector.a, hair);
    store<f32>(vector.a + 4, load<f32>(0x104622C4));
    call<void>(0x0200FCD8, vector.a, hair + 0x78);
    call<void>(0x028E8DAC, hair + 0x78, world.a, hair + 0x78);
    f32 length = load<f32>(0x104622C8);
    hairControl(a + 0x19D8, hair, i, singleAdd(length, length));
    call<void>(0x0200FD38);
  }
  call<void>(0x0200FAD8, 1, 0.0f, 0.0f,
             singleAdd(load<f32>(0x1047B618), 250.0f));
  s16 shake = load<s16>(s + 0x528);
  if (shake) {
    shake = (s16)(shake - 1);
    store<s16>(s + 0x528, shake);
  }
  f32 magnitude = (f32)shake * singleAdd(load<f32>(0x1047B648), 500.0f);
  s16 count = load<s16>(s + 0xC);
  f32 sine = tableSin(0x104A44F8, (s16)(count * 0x2100));
  s16 yaw = (s16)gabi::ftoi(sine * magnitude);
  f32 cosine = tableCos(0x104A44F8, (s16)(count * 0x2300));
  s16 pitch = (s16)gabi::ftoi(cosine * magnitude);
  call<void>(0x025F1C28, load<u32>(0x1018C7B0), yaw);
  call<void>(0x025F1BF4, load<u32>(0x1018C7B0), pitch);
  copyModelMatrix(load<u32>(load<u32>(a + 0x3D0) + 0x90));
  store<f32>(vector.a, 0.0f);
  store<f32>(vector.a + 8, 0.0f);
  store<f32>(vector.a + 4, 0.0f);
  call<void>(0x0200FCD8, vector.a, world.a);
  call<void>(0x02018D40, s + 0x198, world.a);
  call<void>(0x02018C8C, s + 0x198, singleAdd(load<f32>(0x1047B610), 150.0f));
  call<void>(0x0200E240, call<u32>(0x025200D4) + 0x26A4, s + 0x80);
  u32 ex = load<u32>(world.a);
  store<u32>(a + 0x37C, ex);
  u32 ey = load<u32>(world.a + 4);
  store<u32>(a + 0x380, ey);
  u32 ez = load<u32>(world.a + 8);
  store<u32>(a + 0x384, ez);
  store<u32>(a + 0x390, ex);
  store<u32>(a + 0x394, ey);
  store<u32>(a + 0x398, ez);
  f32 eyeHeight = singleAdd(load<f32>(0x1047B62C), 100.0f);
  store<f32>(a + 0x394, singleAdd(load<f32>(a + 0x394), eyeHeight));
  call<void>(0x025E55A0, load<u32>(a + 0x3D0));
  s16 hit = load<s16>(s + 0x22);
  if (hit) {
    hit = (s16)(hit + 1);
    store<s16>(s + 0x22, hit > 100 ? 0 : hit);
  }
  for (int i = 0; i < 9; ++i) {
    u32 part = a + 0x1BE8 + i * 0x19D8;
    x = load<f32>(part + 0x288);
    z = load<f32>(part + 0x290);
    y = load<f32>(part + 0x28C);
    call<void>(0x0200FAD8, 0, x, y, z);
    call<void>(0x025F1C28, load<u32>(0x1018C7B0), load<s16>(part + 0x296));
    call<void>(0x025F1BF4, load<u32>(0x1018C7B0), load<s16>(part + 0x294));
    call<void>(0x0200FCF0);
    size = load<f32>(part + 0x2A8);
    call<void>(0x0200FC74, 1, size, size, size);
    store<f32>(vector.a, 0.0f);
    store<f32>(vector.a + 4, 0.0f);
    store<f32>(vector.a + 8, 0.0f);
    call<void>(0x0200FCD8, vector.a, world.a);
    if (i == 8) {
      s8 health = load<s8>(a + 0x3A1);
      model = health == 3   ? load<u32>(a + 0x1BD8)
              : health == 2 ? load<u32>(a + 0x1BD4)
              : health == 1 ? load<u32>(a + 0x1BD0)
                            : 0;
      store<u32>(part, model);
      store<u32>(s + 0x2D8, load<u32>(world.a));
      store<u32>(s + 0x2DC, load<u32>(world.a + 4));
      store<u32>(s + 0x2E0, load<u32>(world.a + 8));
    }
    model = load<u32>(part);
    if (model)
      copyModelMatrix(model, true);
    call<void>(0x02018D40, part + 0x3C4, world.a);
    f32 radius = singleAdd(load<f32>(0x1047B640), 100.0f);
    radius = load<f32>(part + 0x2A8) * radius;
    call<void>(0x02018C8C, part + 0x3C4, radius);
    u32 flags = load<u32>(part + 0x2AC);
    store<u32>(part + 0x2AC, moving ? flags | 1 : flags & ~1u);
    call<void>(0x0200E240, call<u32>(0x025200D4) + 0x26A4, part + 0x2AC);
    call<void>(0x0200FD38);
    if (i < 7 && load<u8>(0x104622C2)) {
      for (int j = 0; j < load<u16>(0x101911E4 + i * 2); ++j) {
        call<void>(0x0200FCF0);
        hairRotations(j, false);
        f32 unusedHeight = singleAdd(load<f32>(0x1047BD94), 100.0f);
        store<f32>(vector.a + 4, unusedHeight);
        f32 hairLength = singleAdd(load<f32>(0x1047BD90), 60.0f);
        store<f32>(vector.a + 4, load<f32>(part + 0x2A8) * hairLength);
        u32 hair = part + 0x3F0 + j * 0x84;
        call<void>(0x0200FCD8, vector.a, hair);
        store<f32>(vector.a + 4, load<f32>(0x104622C4));
        call<void>(0x0200FCD8, vector.a, hair + 0x78);
        call<void>(0x028E8DAC, hair + 0x78, world.a, hair + 0x78);
        hairControl(part + 0x1890, hair, j,
                    load<f32>(part + 0x2A8) * load<f32>(0x104622C8));
        call<void>(0x0200FD38);
      }
    }
    if (load<s16>(s + 0x22) == load<s16>(0x101911F4 + i * 2)) {
      store<s16>(part + 0x284, 50);
      store<s16>(part + 0x3EC, 100);
    }
    s16 flash = load<s16>(part + 0x284);
    s16 timer = load<s16>(part + 0x3EC);
    if (flash)
      store<s16>(part + 0x284, (s16)(flash - 1));
    if (timer) {
      timer = (s16)(timer - 1);
      store<s16>(part + 0x3EC, timer);
      f32 factor = singleAdd(load<f32>(0x1047BA90), 0.04f);
      // Both products keep the original operand order (first-NaN payload).
      f32 scale = gabi::fmuls_ppc(gabi::fmuls_ppc(load<f32>(part + 0x2A8), (f32)timer), factor);
      if (load<u32>(part + 0x3E4) == 0)
        store<u32>(part + 0x3E4, movementParticle(part + 0x288, 0x3ED));
      else
        emitterMove(part + 0x3E4, part + 0x288, scale);
      if (load<u32>(part + 0x3E8) == 0) {
        store<f32>(vector.a + 4, scale);
        store<f32>(vector.a + 8, scale);
        store<f32>(vector.a, scale);
        store<u32>(part + 0x3E8, movementParticle(part + 0x288, 0x3EE));
      } else {
        emitterMove(part + 0x3E8, part + 0x288, scale);
      }
    } else {
      retireEmitter(part + 0x3E4);
      retireEmitter(part + 0x3E8);
    }
  }
  u32 first = load<u32>(s + 0x80);
  u32 second = load<u32>(s + 0x1AC);
  store<u32>(s + 0x80, moving ? first | 1 : first & ~1u);
  store<u32>(s + 0x1AC, moving ? second | 1 : second & ~1u);
  model = load<u32>(a + 0x484);
  call<void>(0x028E90D4, model ? model + 0xC8 : 0, load<u32>(0x1018C7B0));
  store<f32>(vector.a, 0.0f);
  store<f32>(vector.a + 4, 0.0f);
  store<f32>(vector.a + 8, 0.0f);
  call<void>(0x0200FCD8, vector.a, world.a);
  call<void>(0x0200EDC8, s + 0x470, 1.0f, 25.0f);
  u32 packet = load<u32>(s + 0x46C);
  u32 points = load<u32>(packet);
  u32 sizes = load<u32>(packet + 4);
  for (int i = 0; i < 60; ++i) {
    u32 angle = call<u32>(0x02019510, (f32)i * 0.05324733629822731f) & 65535;
    f32 amplitude =
        load<f32>(0x104A44F8 + (angle >> 3) * 8) * load<f32>(s + 0x470);
    s16 frequency = load<s16>(0x1047B68E);
    s16 counter = load<s16>(s + 0xC);
    s16 stagger = load<s16>(0x1047B690);
    u32 xAngle = (u32)(counter * (frequency + 300) + i * (stagger + 2000));
    s16 zStagger = load<s16>(0x1047B694);
    s16 zFrequency = load<s16>(0x1047B692);
    u32 zAngle = (u32)(counter * (zFrequency + 250) + i * (zStagger + 2000));
    f32 attenuation = (f32)(59 - i) * 0.01666666939854622f;
    amplitude = amplitude * attenuation;
    f32 vx = load<f32>(0x104A44F8 + ((xAngle & 65535) >> 3) * 8) * amplitude;
    f32 vz = load<f32>(0x104A44F8 + ((zAngle & 65535) >> 3) * 8) * amplitude;
    store<f32>(vector.a + 4, 0.0f);
    store<f32>(vector.a, vx);
    store<f32>(vector.a + 8, vz);
    gabi::Local<u32[3]> result;
    call<void>(0x0201AD78, world.a, result.a, vector.a);
    store<u32>(points + i * 12, load<u32>(result.a));
    store<u32>(points + i * 12 + 4, load<u32>(result.a + 4));
    store<u32>(points + i * 12 + 8, load<u32>(result.a + 8));
    store<u8>(sizes + i, load<s16>(0x1047B68E) + 10);
    store<f32>(world.a + 4, singleAdd(load<f32>(world.a + 4), 80.0f));
  }
}
void movement(u32 a) {
  WWHD_FUNC(0x02091730, void, a);
  call<u32>(0x025200D4);
  bool moving = false;
  if (load<u8>(0x10462289) == 0) {
    switch (load<u16>(a + 0x11E66)) {
    case 0:
      moving = movingState(a);
      break;
    case 1:
      stunnedState(a);
      break;
    case 2:
      damagedState(a);
      break;
    case 3:
      dyingState(a);
      break;
    default:
      break;
    }
  }
  splash(a, 0x8443, 0x8407, true);
  call<u32>(0x025200D4);
  hitControl(a);
  bodyControl(a);
  animatedBody(a, moving);
}
VERIFY(0x02091730, movement);

} // namespace bgn3

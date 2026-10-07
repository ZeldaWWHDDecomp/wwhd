// Scoped HD SwimTime screen update functions, reconstructed from WWHD.
#include "gabi.h"
using namespace gabi;
namespace hd_screen_swim_time {
namespace {
u32 anim(u32 self) { return load<u32>(load<u32>(self + 0x44) + 0xd4); }
} // namespace
void blink(u32 self) {
  WWHD_FUNC(0x026E6774, void, self);
  s32 current = load<s32>(call<u32>(0x025200D4) + 0x5b4c);
  s32 maximum = load<s32>(call<u32>(0x025200D4) + 0x5b50);
  if (!maximum)
    maximum = current;
  s32 half = maximum / 2;
  if (current < half) {
    if (!load<u8>(self + 0xa7)) {
      call<void>(0x020053E4, anim(self), 3u, 2u, 1.0f);
      store<u8>(self + 0xa7, 1);
    }
    f32 ratio = (f32)(s32)((u32)half - (u32)current) / (f32)half;
    f32 range = fsubs_ppc(load<f32>(self + 0x88), 1.0f);
    call<void>(0x02005488, anim(self), 3u, 2u, fmadds(ratio, range, 1.0f));
  }
  u32 frame = load<u32>(self + 0x8c) + 1, period = load<u32>(self + 0x90);
  store<u32>(self + 0x8c, frame);
  if ((s32)frame < (s32)period || (s32)frame < (s32)(period << 1))
    return;
  store<u32>(self + 0x8c, 0);
  if (current <= 0) {
    store<u32>(self + 0x90, 0);
    return;
  }
  if (current >= half)
    return;
  f32 ratio =
      call<f32>(0x025DB6F4, half, (s32)((u32)half - (u32)current), (u8)1);
  u32 cooldown = load<u32>(self + 0x94);
  store<s32>(self + 0x90, ftoi(fmadds(-17.0f, ratio, 20.0f)));
  if ((s32)cooldown <= 0) {
    store<u32>(self + 0x94, 2);
    call<void>(0x025E1988, 0x840u);
  } else
    store<u32>(self + 0x94, cooldown - 1);
}
VERIFY(0x026E6774, blink);
void flash(u32 self) {
  WWHD_FUNC(0x026E6970, void, self);
  u32 delay = load<u32>(self + 0x98);
  if (delay)
    store<u32>(self + 0x98, delay - 1);
  else {
    u32 available[6], count = 0;
    for (u32 i = 0; i < 6; ++i)
      if (!load<u8>(self + 0xa0 + i))
        available[count++] = i;
    if (count) {
      f32 choice = call<f32>(0x020198D8, (f32)count);
      u32 selected =
          (choice < 2147483648.0f)
              ? (u32)ftoi(choice)
              : (u32)ftoi(fsubs_ppc(choice, 2147483648.0f)) + 0x80000000u;
      u32 index = available[selected];
      store<u8>(self + 0xa0 + index, 1);
      u32 slot = self + 0x50 + (index < 6 ? index * 4 : 0);
      if (load<u32>(slot)) {
        call<u32>(0x025200D4);
        call<u32>(0x025200D4);
        u32 pane = load<u32>(slot);
        u32 x = load<u32>(pane + 0x1c), z = load<u32>(pane + 0x24);
        f32 distance = load<f32>(self + 0x80), ratio = load<f32>(self + 0x84);
        f32 y = load<f32>(self + 0x68 + (index < 6 ? index * 4 : 0));
        f32 position = fnmsubs(distance, ratio, y);
        pane = load<u32>(slot);
        u8 flags = load<u8>(pane + 0x44);
        store<u32>(pane + 0x1c, x);
        store<f32>(pane + 0x20, position);
        store<u8>(pane + 0x44, flags | 0x10);
        store<u32>(pane + 0x24, z);
      }
      u32 animation = index + 4;
      call<void>(0x020053E4, anim(self), animation,
                 load<u32>(0x10107044 + animation * 4), 1.0f);
      store<s32>(self + 0x98, ftoi(call<f32>(0x020198D8, 10.0f)));
    }
  }
  for (u32 i = 0; i < 6; ++i) {
    if (load<u8>(self + 0xa0 + i) && call<u32>(0x02005840, anim(self), i + 4))
      store<u8>(self + 0xa0 + i, 0);
  }
}
VERIFY(0x026E6970, flash);
void update(u32 self) {
  WWHD_FUNC(0x026E6BF8, void, self);
  s32 current = load<s32>(call<u32>(0x025200D4) + 0x5b4c);
  s32 maximum = load<s32>(call<u32>(0x025200D4) + 0x5b50);
  store<f32>(self + 0x84,
             (f32)(s32)((u32)maximum - (u32)current) / (f32)maximum);
  if (current) {
    call<void>(0x026E6774, self);
    call<void>(0x026E6970, self);
    u32 play = call<u32>(0x025200D4);
    if (load<u8>(play + 0x5bb0))
      store<u32>(play + 0x5b4c, load<u32>(play + 0x5b4c) - 1);
    call<void>(0x0200552C, anim(self), 10u, 1u, load<f32>(self + 0x84));
    if (!load<u32>(call<u32>(0x025200D4) + 0x5b4c)) {
      play = call<u32>(0x025200D4);
      store<u8>(play + 0x5bb0, 0);
      call<void>(0x020063C0, self + 0x18, 0x1049C874u);
    }
    if (!call<u32>(0x026E6428, self))
      call<void>(0x020063C0, self + 0x18, 0x1049C844u);
  } else
    call<void>(0x020063C0, self + 0x18, 0x1049C844u);
  call<void>(load<u32>(load<u32>(self + 4) + 0x8c), self);
}
VERIFY(0x026E6BF8, update);
} // namespace hd_screen_swim_time

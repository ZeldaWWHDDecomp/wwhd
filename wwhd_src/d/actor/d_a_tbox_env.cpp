// HD texture replacement, verified-source decompilation.
#include "bindings.h"
#include "d/actor/d_a_tbox.h"
static u32 addr(const void *p) { return gabi::ea(p); }
struct SafeString_l {
  be<u32> text, vtable;
};
static u32 getModelInfo(daTbox_c *self) {
  return gabi::call<u32>(0x024B432C, self);
}
static u32 ppcSlw(u32 x, u32 count) {
  count &= 63;
  return count < 32 ? x << count : 0;
}
static u32 ppcSrw(u32 x, u32 count) {
  count &= 63;
  return count < 32 ? x >> count : 0;
}
static s32 envShapeSet(daTbox_c *self) {
  WWHD_FUNC(0x024B4858, s32, self);
  // ResTIMG's relative offsets retain the address of this temporary. Preserve
  // its original frame position because those addresses escape into the
  // texture.
  struct TextureFrame_l {
    u8 pad[8];
    u8 image[36];
    SafeString_l name;
    u8 tail[0x1C];
  };
  gabi::Local<TextureFrame_l> frame;
  u32 info = getModelInfo(self);
  s32 index = gabi::load<s16>(info);
  frame->name.vtable = 0x1003FDC4;
  u32 control = gabi::load<u32>(0x101F4F28);
  frame->name.text = 0x1003FEC4;
  u32 model = gabi::call<u32>(0x026066C4, control, &frame->name, index),
      texture = gabi::load<u32>(model + 0x30);
  if (!texture)
    return 5;
  u32 names = gabi::load<u32>(model + 0x34);
  if (!names)
    return 5;
  u16 i = 0;
  if (i >= gabi::load<u16>(texture))
    return 4;
  do {
    u32 name = gabi::call<u32>(0x027ED1F0, names, i);
    if (!name)
      gabi::call(0x0273AA24, 0x1003FEE0, 0x183, 0x1003FED4);
    else {
      u32 expected = 0x1003FECC;
      u8 a, b;
      do {
        a = gabi::load<u8>(name++);
        b = gabi::load<u8>(expected++);
      } while (a == b && a);
      if (a == b) {
        u32 graphics = gabi::load<u32>(0x101F9968),
            image = gabi::call<u32>(0x027F81A4, graphics, 6);
        u32 allocation = gabi::call<u32>(0x0273AD10, 192),
            object = gabi::call<u32>(0x02773680, allocation, image);
        u32 local = addr(frame.get()) + 8;
        gabi::store<u32>(local + 32, object);
        gabi::store<u16>(local + 2, gabi::load<u32>(object + 8));
        gabi::store<u16>(local + 4, gabi::load<u32>(object + 12));
        gabi::store<u8>(local + 8, 0);
        u32 dest = gabi::load<u32>(texture + 4) + i * 36;
        for (int j = 0; j < 3; j++) {
          u32 x = gabi::load<u32>(local + j * 12),
              y = gabi::load<u32>(local + j * 12 + 4),
              z = gabi::load<u32>(local + j * 12 + 8);
          gabi::store<u32>(dest + j * 12, x);
          gabi::store<u32>(dest + j * 12 + 4, y);
          gabi::store<u32>(dest + j * 12 + 8, z);
        }
        dest = gabi::load<u32>(texture + 4) + i * 36;
        gabi::store<u32>(dest + 28, gabi::load<u32>(dest + 28) + local - dest);
        dest = gabi::load<u32>(texture + 4) + i * 36;
        gabi::store<u32>(dest + 12, gabi::load<u32>(dest + 12) + local - dest);
        dest = gabi::load<u32>(texture + 4) + i * 36;
        gabi::store<u32>(dest + 32, gabi::load<u32>(local + 32));
        u32 hi = gabi::load<u32>(texture + 24),
            lo = gabi::load<u32>(texture + 28), n = i < 64 ? i : i - 64,
            offset = i < 64 ? 8 : 16;
        u32 a = gabi::load<u32>(texture + offset),
            b = gabi::load<u32>(texture + offset + 4);
        u32 upper = ppcSlw(hi, n) | ppcSlw(lo, n + 32) | ppcSrw(lo, 32 - n),
            lower = ppcSlw(lo, n);
        if (i < 64) {
          gabi::store<u32>(texture + offset, a | upper);
          gabi::store<u32>(texture + offset + 4, b | lower);
        } else {
          gabi::store<u32>(texture + offset + 4, b | lower);
          gabi::store<u32>(texture + offset, a | upper);
        }
      }
    }
    i++;
  } while (i < gabi::load<u16>(texture));
  return 4;
}
VERIFY(0x024B4858, envShapeSet);

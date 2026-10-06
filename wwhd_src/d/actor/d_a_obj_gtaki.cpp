// WWHD Ganon waterfall. Derived game code:
#include "bindings.h"
namespace {
using gabi::at;
using gabi::call;
using gabi::load;
using gabi::store;
u32 model(u32 a) { return load<u32>(a + 0x3b4); }
u32 play() { return call<u32>(0x025200D4); }
void trans(u32 a) {
  float x = load<f32>(a + 0x314), y = load<f32>(a + 0x318),
        z = load<f32>(a + 0x31c);
  call(0x028E93CC, at<void>(0x1048D0CC), x, y, z);
}
void matrix(u32 dst) {
  f32 m[12];
  for (int i = 0; i < 12; i++)
    m[i] = load<f32>(0x1048D0CC + i * 4);
  for (int i = 0; i < 12; i++)
    store<f32>(dst + i * 4, m[i]);
}
} // namespace
void gtaki_setMtx(void *actor) {
  WWHD_FUNC(0x0234D9C8, void, actor);
  u32 a = gabi::ea(actor);
  f32 x = load<f32>(a + 0x330), y = load<f32>(a + 0x334),
      z = load<f32>(a + 0x338);
  u32 m = model(a);
  store<f32>(m + 0xbc, x);
  store<f32>(m + 0xc0, y);
  store<f32>(m + 0xc4, z);
  trans(a);
  call(0x025F1C28, at<void>(0x1048D0CC), load<s16>(a + 0x322));
  matrix(model(a) + 0xc8);
}
VERIFY(0x0234D9C8, gtaki_setMtx);
s32 gtaki_create(void *actor) {
  WWHD_FUNC(0x0234DC28, s32, actor);
  u32 a = gabi::ea(actor), flags = load<u32>(a + 0x2e4);
  if (!(flags & 8)) {
    if (a) {
      call(0x0234D794, actor);
      flags = load<u32>(a + 0x2e4);
    }
    store<u32>(a + 0x2e4, flags | 8);
  }
  s32 phase = call<s32>(0x02520460, at<void>(a + 0x3ac), at<char>(0x10029aa4));
  if (phase == 4) {
    if (!call<s32>(0x025D63E8, actor, at<void>(0x0234d790), 0x3450))
      return 5;
    call(0x0234DAA0, actor);
  }
  return phase;
}
VERIFY(0x0234DC28, gtaki_create);
s32 gtaki_delete(void *actor) {
  WWHD_FUNC(0x0234DCE0, s32, actor);
  u32 a = gabi::ea(actor);
  if (load<u32>(a + 0xf4)) {
    u32 p = play();
    call(0x020087EC, at<void>(p + 0x12a0), at<void>(load<u32>(a + 0x598)));
  }
  call(0x025204C8, at<void>(a + 0x3ac), at<char>(0x10029b84));
  return 1;
}
VERIFY(0x0234DCE0, gtaki_delete);
s32 gtaki_draw(void *actor) {
  WWHD_FUNC(0x0234DD38, s32, actor);
  u32 a = gabi::ea(actor);
  u32 env = call<u32>(0x02555D0C);
  call(0x025626A4, at<void>(env), 4, at<void>(a + 0x314), at<void>(a + 0x110));
  env = call<u32>(0x02555D0C);
  call(0x02562F5C, at<void>(env), at<void>(model(a)), at<void>(a + 0x110));
  u32 p = play();
  store<u32>(0x104b4634, load<u32>(p + 0x5d8c));
  p = play();
  store<u32>(0x104b4638, load<u32>(p + 0x5d90));
  u32 data = load<u32>(model(a) + 0xac);
  f32 frame = load<f32>(a + 0x3bc);
  call(0x025E7FC4, at<void>(a + 0x3b8), at<void>(data), frame);
  call(0x025E2DE0, at<void>(model(a)), 0);
  store<u32>(load<u32>(model(a) + 0xac) + 0x44, 0);
  p = play();
  store<u32>(0x104b4634, load<u32>(p + 0x5d78));
  p = play();
  store<u32>(0x104b4638, load<u32>(p + 0x5d7c));
  return 1;
}
VERIFY(0x0234DD38, gtaki_draw);
s32 gtaki_execute(void *actor) {
  WWHD_FUNC(0x0234DDF4, s32, actor);
  u32 a = gabi::ea(actor);
  call(0x020182E0, at<void>(a + 0x580), at<void>(a + 0x314));
  u32 p = play();
  call(0x0200E240, at<void>(p + 0x26a4), at<void>(a + 0x468));
  call(0x025E742C, at<void>(a + 0x3b8));
  gtaki_setMtx(actor);
  return 1;
}
VERIFY(0x0234DDF4, gtaki_execute);
s32 gtaki_isDelete(void *actor) {
  WWHD_FUNC(0x0234DF00, s32, actor);
  return 1;
}
VERIFY(0x0234DF00, gtaki_isDelete);
void gtaki_empty(void *actor) { WWHD_FUNC(0x0234DF74, void, actor); }
VERIFY(0x0234DF74, gtaki_empty);
void gtaki_trivialDtor(void *obj, u32 flags) {
  WWHD_FUNC(0x0234DEEC, void, obj, flags);
  if (obj && (flags & 1))
    call(0x0273AF40, obj);
}
VERIFY(0x0234DEEC, gtaki_trivialDtor);
void gtaki_dtor(void *obj, u32 flags) {
  WWHD_FUNC(0x0234DF08, void, obj, flags);
  u32 a = gabi::ea(obj);
  if (a) {
    call(0x02515A70, at<void>(a + 0x468), 2);
    call(0x02515860, at<void>(a + 0x42c), 2);
    call(0x025D50BC, obj, 0);
    if (flags & 1)
      call(0x0273AF40, obj);
  }
}
VERIFY(0x0234DF08, gtaki_dtor);
void gtaki_sinit() {
  WWHD_FUNC(0x0234DE58, void);
  store<u32>(0x10469cf8, 0);
  store<u32>(0x10469cf0, 0);
  store<u32>(0x10469cfc, 0);
  store<u32>(0x10469cf4, 0);
  call(0x028F026C, at<void>(0x101c9b00));
  store<f32>(0x10469ce4, load<f32>(0x10029b90));
  store<f32>(0x10469ce8, load<f32>(0x10029b94));
  call(0x028ED6F8, at<void>(0x10469cec));
  call(0x028F026C, at<void>(0x101c9b0c));
  call(0x028EAB2C, at<void>(0x10469ced));
  call(0x028F026C, at<void>(0x101c9b18));
}
VERIFY(0x0234DE58, gtaki_sinit);
void *gtaki_ctor(void *obj) {
  WWHD_FUNC(0x0234D794, void *, obj);
  u32 a = gabi::ea(obj);
  if (!a) {
    a = call<u32>(0x0273AD10, 0x794);
    if (!a)
      return nullptr;
  }
  call(0x025D4ED0, at<void>(a));
  store<u32>(a + 0xb4, 0x10029ad4);
  call(0x025E7C6C, at<void>(a + 0x3b8));
  call(0x0200BD2C, at<void>(a + 0x42c));
  call(0x02515DA0, at<void>(a + 0x448));
  store<u32>(a + 0x444, 0x1004ae88);
  store<u32>(a + 0x448, 0x1004aec0);
  call(0x02515FB8, at<void>(a + 0x468));
  store<u32>(a + 0x57c, 0x100015a8);
  store<u32>(a + 0x578, 0x10029ac4);
  call(0x02018590, at<void>(a + 0x580));
  store<u32>(a + 0x4a4, 0x1004b108);
  store<u32>(a + 0x594, 0x1004b150);
  store<u32>(a + 0x57c, 0x1004b160);
  // Three embedded lighting structures are initialized from the same template.
  f32 f[14];
  for (int i = 0; i < 6; i++)
    f[i] = load<f32>(0x1016e414 + i * 4);
  u8 b[4];
  for (int i = 0; i < 4; i++)
    b[i] = load<u8>(0x1016e42c + i);
  s16 h[4];
  for (int i = 0; i < 4; i++)
    h[i] = load<s16>(0x1016e430 + i * 2);
  for (int i = 0; i < 8; i++)
    f[6 + i] = load<f32>(0x1016e438 + i * 4);
  for (u32 off : {0x5ccu, 0x68cu, 0x710u}) {
    for (int i = 0; i < 6; i++)
      store<f32>(a + off + i * 4, f[i]);
    for (int i = 0; i < 4; i++)
      store<u8>(a + off + 0x18 + i, b[i]);
    for (int i = 0; i < 4; i++)
      store<s16>(a + off + 0x1c + i * 2, h[i]);
    for (int i = 0; i < 8; i++)
      store<f32>(a + off + 0x24 + i * 4, f[6 + i]);
  }
  return at<void>(a);
}
VERIFY(0x0234D794, gtaki_ctor);
s32 gtaki_init(void *actor) {
  WWHD_FUNC(0x0234DAA0, s32, actor);
  u32 a = gabi::ea(actor), m = model(a);
  store<u32>(a + 0x348, m ? m + 0xc8 : 0);
  call(0x025D674C, actor, -600.f, -0.f, -600.f, 600.f, 10000.f, 600.f);
  store<f32>(a + 0x364, 1.f);
  call(0x02515F14, at<void>(a + 0x42c), 0xff, 0xff, actor);
  call(0x02516518, at<void>(a + 0x468), at<void>(0x101c9abc));
  call(0x020184DC, at<void>(a + 0x580), 70.f * load<f32>(a + 0x330));
  s8 room = load<s8>(a + 0x2fe);
  store<u32>(a + 0x4ac, a + 0x42c);
  call(0x0255FFF4, at<void>(a + 0x5cc), room, 0xff);
  u32 env = call<u32>(0x02555D0C);
  call(0x025626A4, at<void>(env), 2, at<void>(a + 0x314), at<void>(a + 0x5cc));
  u32 p = play(), control = load<u32>(p + 0x5ab0);
  u32 e = call<u32>(0x025A847C, at<void>(control), 1, 0x835b,
                    at<void>(a + 0x314), 0, 0, 0xff, 0, -1, 0, 0, 0);
  if (e) {
    f32 z = load<f32>(a + 0x338), x = load<f32>(a + 0x330),
        y = load<f32>(a + 0x334);
    store<f32>(e + 0x220, x);
    store<f32>(e + 0x228, z);
    store<f32>(e + 0x224, y);
    x = load<f32>(a + 0x330);
    store<f32>(e + 0x238, x);
    store<f32>(e + 0x23c, x);
    store<f32>(e + 0x240, x);
    u8 r = load<u8>(a + 0x65d), b = load<u8>(a + 0x661);
    store<u8>(e + 0x244, r);
    u8 g = load<u8>(a + 0x65f);
    store<u8>(e + 0x246, b);
    store<u8>(e + 0x245, g);
  }
  gtaki_setMtx(actor);
  p = play();
  return call<s32>(0x024EEA6C, at<void>(p + 0x12a0),
                   at<void>(load<u32>(a + 0x598)), actor);
}
VERIFY(0x0234DAA0, gtaki_init);
s32 gtaki_heap(void *actor) {
  WWHD_FUNC(0x0234D5EC, s32, actor);
  u32 a = gabi::ea(actor);
  struct Name {
    be<u32> text, vt;
  };
  gabi::Local<Name> name1;
  name1->text = 0x10029b34;
  name1->vt = 0x10029aac;
  u32 data =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name1.get(), 5);
  if (!data)
    call(0x0273AA24, at<char>(0x10029b3c), 0x123, at<char>(0x10029b50));
  u32 m = call<u32>(0x025E38E0, at<void>(data), 0, 0x11020203);
  store<u32>(a + 0x3b4, m);
  if (!m)
    return 0;
  gabi::Local<Name> name2;
  name2->text = 0x10029b34;
  name2->vt = 0x10029aac;
  u32 anim =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name2.get(), 8);
  if (!anim)
    call(0x0273AA24, at<char>(0x10029b3c), 0x12d, at<char>(0x10029b64));
  call(0x025E7CE0, at<void>(a + 0x3b8), at<void>(data), at<void>(anim), 1, 2,
       1.f, 0, -1, 0, 0);
  call(0x0234D3AC, actor);
  trans(a);
  call(0x025F1C28, at<void>(0x1048d0cc), load<s16>(a + 0x32a));
  f32 y = load<f32>(a + 0x334), x = load<f32>(a + 0x330),
      z = load<f32>(a + 0x338);
  call(0x025F2518, x, y, z);
  call(0x028E90D4, at<void>(0x1048d0cc), at<void>(a + 0x59c));
  u32 bg = call<u32>(0x024F23F4, at<void>(0));
  store<u32>(a + 0x598, bg);
  if (!bg)
    return 0;
  gabi::Local<Name> name3;
  name3->text = 0x10029b34;
  name3->vt = 0x10029aac;
  u32 mesh =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name3.get(), 11);
  return call<s32>(0x0200A030, at<void>(load<u32>(a + 0x598)), at<void>(mesh),
                   1, at<void>(a + 0x59c)) == 0;
}
VERIFY(0x0234D5EC, gtaki_heap);
s32 gtaki_heapCheck(void *actor) {
  WWHD_FUNC(0x0234D790, s32, actor);
  return gtaki_heap(actor);
}
VERIFY(0x0234D790, gtaki_heapCheck);
namespace {
u32 slw(u32 v, u32 n) {
  n &= 63;
  return n < 32 ? v << n : 0;
}
u32 srw(u32 v, u32 n) {
  n &= 63;
  return n < 32 ? v >> n : 0;
}
} // namespace
void gtaki_dummyTexture(void *actor) {
  WWHD_FUNC(0x0234D3AC, void, actor);
  u32 data = load<u32>(model(gabi::ea(actor)) + 0xac),
      texture = load<u32>(data + 0x30), names = load<u32>(data + 0x34);
  if (!texture)
    call(0x0273AA24, at<char>(0x10029af8), 0xb8, at<char>(0x10029b0c));
  if (!names)
    call(0x0273AA24, at<char>(0x10029af8), 0xb9, at<char>(0x10029b1c));
  // The texture offsets escape the stack, so preserve the original
  // frame-relative header address.
  struct ImageFrame {
    u8 bytes[80];
  };
  gabi::Local<ImageFrame> frameStorage;
  u32 imageAddress = frameStorage.a + 8;
  for (u16 i = 0; i < load<u16>(texture); i++) {
    u32 name = call<u32>(0x027ED1F0, at<void>(names), i);
    if (!name) {
      call(0x0273AA24, at<char>(0x10029af8), 0xc0, at<char>(0x10029aec));
      continue;
    }
    u32 s = name, t = 0x10029ae4;
    u8 c, d;
    do {
      c = load<u8>(s++);
      d = load<u8>(t++);
    } while (c == d && c);
    if (c != d)
      continue;
    u32 frame = call<u32>(0x027F81A4, at<void>(load<u32>(0x101f9968)), 6);
    u32 allocation = call<u32>(0x0273AD10, 0xc0);
    u32 buffer = call<u32>(0x02773680, at<void>(allocation), at<void>(frame));
    store<u32>(imageAddress + 32, buffer);
    store<u16>(imageAddress + 2, (u16)load<u32>(buffer + 8));
    store<u8>(imageAddress + 8, 0);
    store<u16>(imageAddress + 4, (u16)load<u32>(buffer + 12));
    u32 dest = load<u32>(texture + 4) + i * 36;
    for (int j = 0; j < 9; j++)
      store<u32>(dest + j * 4, load<u32>(imageAddress + j * 4));
    dest = load<u32>(texture + 4) + i * 36;
    store<u32>(dest + 28, load<u32>(dest + 28) + imageAddress - dest);
    dest = load<u32>(texture + 4) + i * 36;
    store<u32>(dest + 12, load<u32>(dest + 12) + imageAddress - dest);
    dest = load<u32>(texture + 4) + i * 36;
    store<u32>(dest + 32, load<u32>(imageAddress + 32));
    u32 hi = load<u32>(texture + 24), lo = load<u32>(texture + 28);
    if (i < 64) {
      u32 oldlo = load<u32>(texture + 12), oldhi = load<u32>(texture + 8);
      u32 h = slw(hi, i) | srw(lo, 32 - i) | slw(lo, i + 32);
      store<u32>(texture + 8, oldhi | h);
      store<u32>(texture + 12, oldlo | slw(lo, i));
    } else {
      u32 k = i - 64, oldlo = load<u32>(texture + 20),
          oldhi = load<u32>(texture + 16);
      u32 h = slw(hi, k) | srw(lo, 32 - k) | slw(lo, k + 32);
      store<u32>(texture + 20, oldlo | slw(lo, k));
      store<u32>(texture + 16, oldhi | h);
    }
  }
}
VERIFY(0x0234D3AC, gtaki_dummyTexture);

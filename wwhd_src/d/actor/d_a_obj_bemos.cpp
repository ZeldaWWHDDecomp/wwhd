// Beamos, actual WWHD layouts and behavior.
#include "bindings.h"
using gabi::at;
using gabi::call;
using gabi::load;
using gabi::store;
struct BemosName {
  be<u32> text, vt;
};
s32 bemos_heap1(void *actor) {
  WWHD_FUNC(0x0232409C, s32, actor);
  u32 a = gabi::ea(actor);
  gabi::Local<BemosName> name0;
  name0->text = 0x10026478;
  name0->vt = 0x10026254;
  u32 data0 =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name0.get(), 5);
  if (!data0)
    call(0x0273AA24, at<void>(0x1002632c), 422, at<void>(0x10026340));
  u32 model0 = call<u32>(0x025E38E0, at<void>(data0), 0, 0x11020203);
  store<u32>(a + 0x3d0, model0);
  if (!model0)
    return 0;
  gabi::Local<BemosName> name1;
  name1->text = 0x10026478;
  name1->vt = 0x10026254;
  u32 data1 =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name1.get(), 8);
  if (!data1)
    call(0x0273AA24, at<void>(0x1002632c), 431, at<void>(0x10026340));
  u32 model1 = call<u32>(0x025E38E0, at<void>(data1), 0, 0x11020203);
  store<u32>(a + 0x3d4, model1);
  if (!model1)
    return 0;
  return 1;
}
VERIFY(0x0232409C, bemos_heap1);
s32 bemos_heap2(void *actor) {
  WWHD_FUNC(0x0232419C, s32, actor);
  u32 a = gabi::ea(actor);
  gabi::Local<BemosName> name0;
  name0->text = 0x10026478;
  name0->vt = 0x10026254;
  u32 data0 =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name0.get(), 6);
  if (!data0)
    call(0x0273AA24, at<void>(0x10026358), 453, at<void>(0x1002636c));
  u32 model0 = call<u32>(0x025E38E0, at<void>(data0), 0, 0x11020203);
  store<u32>(a + 0x3d0, model0);
  if (!model0)
    return 0;
  gabi::Local<BemosName> name1;
  name1->text = 0x10026478;
  name1->vt = 0x10026254;
  u32 data1 =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name1.get(), 9);
  if (!data1)
    call(0x0273AA24, at<void>(0x10026358), 462, at<void>(0x1002636c));
  u32 model1 = call<u32>(0x025E38E0, at<void>(data1), 0, 0x11020203);
  store<u32>(a + 0x3d4, model1);
  if (!model1)
    return 0;
  gabi::Local<BemosName> name2;
  name2->text = 0x10026478;
  name2->vt = 0x10026254;
  u32 data2 =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name2.get(), 12);
  if (!data2)
    call(0x0273AA24, at<void>(0x10026358), 480, at<void>(0x1002636c));
  u32 model2 = call<u32>(0x025E38E0, at<void>(data2), 0, 0x11020203);
  u32 upper = load<u32>(a + 0x3d4);
  store<u32>(a + 0x3d8, model2);
  if (!upper)
    return 0;
  gabi::Local<BemosName> name3;
  name3->text = 0x10026478;
  name3->vt = 0x10026254;
  u32 data3 =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name3.get(), 15);
  store<u32>(a + 0x458, data3);
  if (!data3) {
    call(0x0273AA24, at<void>(0x10026358), 488, at<void>(0x10026380));
    data3 = load<u32>(a + 0x458);
  }
  return call<u32>(0x025E8154, at<void>(a + 0x45c), at<void>(data2),
                   at<void>(data3), 1, 2, 1.f, 0, -1, 0, 0) != 0;
}
VERIFY(0x0232419C, bemos_heap2);
s32 bemos_heap3(void *actor) {
  WWHD_FUNC(0x0232435C, s32, actor);
  u32 a = gabi::ea(actor);
  gabi::Local<BemosName> name0;
  name0->text = 0x10026478;
  name0->vt = 0x10026254;
  u32 data0 =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name0.get(), 7);
  if (!data0)
    call(0x0273AA24, at<void>(0x10026394), 501, at<void>(0x100263a8));
  u32 model0 = call<u32>(0x025E38E0, at<void>(data0), 0, 0x11020203);
  store<u32>(a + 0x3d0, model0);
  if (!model0)
    return 0;
  return 1;
}
VERIFY(0x0232435C, bemos_heap3);
s32 bemos_heap(void *actor) {
  WWHD_FUNC(0x023243F8, s32, actor);
  u8 type = load<u8>(gabi::ea(actor) + 0x890);
  if (type == 0)
    return bemos_heap1(actor);
  if (type == 1)
    return bemos_heap2(actor);
  if (type == 2)
    return bemos_heap3(actor);
  return 0;
}
VERIFY(0x023243F8, bemos_heap);
s32 bemos_heapCallback(void *actor) {
  WWHD_FUNC(0x02324424, s32, actor);
  return bemos_heap(actor);
}
VERIFY(0x02324424, bemos_heapCallback);
s32 bemos_blueWaitInit(void *actor) {
  WWHD_FUNC(0x02324428, s32, actor);
  u32 a = gabi::ea(actor);
  if (!load<u32>(a + 0x4d4)) {
    gabi::Local<cXyz> pos;
    f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318),
        z = load<f32>(a + 0x31c);
    pos->x = x;
    pos->z = z;
    pos->y = y + 210.f;
    u32 play = call<u32>(0x025200D4);
    u32 emitter = call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0,
                            0x815c, pos.get(), 0, 0, 0xff, 0, -1, 0, 0, 0);
    store<u32>(a + 0x4d4, emitter);
    if (emitter) {
      store<u8>(emitter + 0x244, 160);
      store<u8>(emitter + 0x245, 180);
      store<u8>(emitter + 0x246, 255);
      u32 e = load<u32>(a + 0x4d4);
      store<u8>(e + 0x249, 155);
      store<u8>(e + 0x248, 24);
      store<u8>(e + 0x24a, 212);
    }
  }
  if (!load<u32>(a + 0x4d8)) {
    u32 angle = load<u16>(a + 0x322), trig = 0x104a44f8 + ((angle >> 3) * 8);
    f32 y = load<f32>(a + 0x3c0), x = load<f32>(a + 0x3bc);
    f32 sine = load<f32>(trig);
    gabi::Local<cXyz> pos;
    pos->y = y;
    x = gabi::fmadds(60.f, sine, x);
    f32 z = load<f32>(a + 0x3c4), cosine = load<f32>(trig + 4);
    z = gabi::fmadds(60.f, cosine, z);
    pos->x = x;
    pos->z = z;
    u32 play = call<u32>(0x025200D4);
    u32 e = call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x815d,
                      pos.get(), 0, 0, 0xff, 0, -1, 0, 0, 0);
    store<u32>(a + 0x4d8, e);
    if (e) {
      store<u8>(e + 0x244, 20);
      store<u8>(e + 0x245, 54);
      store<u8>(e + 0x246, 195);
    }
  }
  u32 old = load<u32>(a + 0x4dc);
  if (old) {
    u32 flags = load<u32>(old + 0x254);
    store<u32>(old + 0x5c, 0xffffffff);
    store<u32>(old + 0x254, flags | 1);
    store<u32>(a + 0x4dc, 0);
  }
  store<u16>(a + 0x3ac, 0);
  store<f32>(a + 0x53c, 0.75f);
  store<f32>(a + 0x540, 1.f);
  store<u16>(a + 0x3ae, 0xffff);
  store<f32>(a + 0x538, 0.f);
  store<u32>(a + 0x3b0, 0x23267cc);
  return 1;
}
VERIFY(0x02324428, bemos_blueWaitInit);
s32 bemos_redWaitInit(void *actor) {
  WWHD_FUNC(0x02324830, s32, actor);
  u32 a = gabi::ea(actor);
  if (!load<u32>(a + 0x4d4)) {
    gabi::Local<cXyz> pos;
    f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318),
        z = load<f32>(a + 0x31c);
    pos->x = x;
    pos->z = z;
    pos->y = y + 210.f;
    u32 play = call<u32>(0x025200D4);
    u32 emitter = call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0,
                            0x815c, pos.get(), 0, 0, 0xff, 0, -1, 0, 0, 0);
    store<u32>(a + 0x4d4, emitter);
    if (emitter) {
      store<u8>(emitter + 0x244, 255);
      store<u8>(emitter + 0x245, 128);
      store<u8>(emitter + 0x246, 200);
      u32 e = load<u32>(a + 0x4d4);
      store<u8>(e + 0x248, 212);
      store<u8>(e + 0x24a, 255);
      store<u8>(e + 0x249, 24);
    }
  }
  if (!load<u32>(a + 0x4d8)) {
    u32 angle = load<u16>(a + 0x322), trig = 0x104a44f8 + ((angle >> 3) * 8);
    f32 y = load<f32>(a + 0x3c0), x = load<f32>(a + 0x3bc);
    f32 sine = load<f32>(trig);
    gabi::Local<cXyz> pos;
    pos->y = y;
    x = gabi::fmadds(60.f, sine, x);
    f32 z = load<f32>(a + 0x3c4), cosine = load<f32>(trig + 4);
    z = gabi::fmadds(60.f, cosine, z);
    pos->x = x;
    pos->z = z;
    u32 play = call<u32>(0x025200D4);
    u32 e = call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x815d,
                      pos.get(), 0, 0, 0xff, 0, -1, 0, 0, 0);
    store<u32>(a + 0x4d8, e);
    if (e) {
      store<u8>(e + 0x244, 195);
      store<u8>(e + 0x245, 20);
      store<u8>(e + 0x246, 54);
    }
  }
  u32 old = load<u32>(a + 0x4dc);
  if (old) {
    u32 flags = load<u32>(old + 0x254);
    store<u32>(old + 0x5c, 0xffffffff);
    store<u32>(old + 0x254, flags | 1);
    store<u32>(a + 0x4dc, 0);
  }
  store<u16>(a + 0x3ac, 0);
  store<f32>(a + 0x53c, 0.75f);
  store<f32>(a + 0x540, 1.f);
  store<u16>(a + 0x3ae, 0xffff);
  store<f32>(a + 0x538, 0.f);
  store<u32>(a + 0x3b0, 0x232728c);
  return 1;
}
VERIFY(0x02324830, bemos_redWaitInit);
s32 bemos_yellowRange(void *actor, s32 range, void *angle) {
  WWHD_FUNC(0x0232665C, s32, actor, range, angle);
  store<u32>(gabi::ea(actor) + 0x87c, 1);
  return 1;
}
VERIFY(0x0232665C, bemos_yellowRange);
s32 bemos_blueSearchInit(void *actor) {
  WWHD_FUNC(0x023268A8, s32, actor);
  u32 a = gabi::ea(actor);
  store<u16>(a + 0x3ac, 0);
  store<u16>(a + 0x3ae, 0xffff);
  store<u32>(a + 0x3b0, 0x02326e4c);
  store<u16>(a + 0x884, 0);
  return 1;
}
VERIFY(0x023268A8, bemos_blueSearchInit);
s32 bemos_redSearchInit(void *actor) {
  WWHD_FUNC(0x023273C0, s32, actor);
  u32 a = gabi::ea(actor);
  store<u16>(a + 0x3ac, 0);
  store<u16>(a + 0x3ae, 0xffff);
  store<u32>(a + 0x3b0, 0x02327968);
  store<s16>(a + 0x88e, load<s16>(0x104692fe));
  return 1;
}
VERIFY(0x023273C0, bemos_redSearchInit);
void bemos_hioDtor(void *obj, u32 flags) {
  WWHD_FUNC(0x023288A8, void, obj, flags);
  if (obj && (flags & 1))
    call(0x0273AF40, obj);
}
VERIFY(0x023288A8, bemos_hioDtor);
s32 bemos_isDelete(void *actor) {
  WWHD_FUNC(0x023288BC, s32, actor);
  return 1;
}
VERIFY(0x023288BC, bemos_isDelete);
void bemos_actorDtor(void *obj, u32 flags) {
  WWHD_FUNC(0x023288C4, void, obj, flags);
  u32 a = gabi::ea(obj);
  if (a) {
    store<u32>(a + 0x844, 0x1002629c);
    store<u32>(a + 0x864, 0x100262bc);
    store<u32>(a + 0x870, 0x1002627c);
    call(0x02008DAC, at<void>(a + 0x824), 0);
    call(0x02515AE8, at<void>(a + 0x6f8), 2);
    call(0x02515860, at<void>(a + 0x6bc), 2);
    call(0x02515A70, at<void>(a + 0x58c), 2);
    call(0x02515860, at<void>(a + 0x550), 2);
    call(0x025D50BC, obj, 0);
    if (flags & 1)
      call(0x0273AF40, obj);
  }
}
VERIFY(0x023288C4, bemos_actorDtor);
void bemos_empty(void *actor) { WWHD_FUNC(0x02328978, void, actor); }
VERIFY(0x02328978, bemos_empty);
void *bemos_beam(void *actor) {
  WWHD_FUNC(0x02325D3C, void *, actor);
  u32 id = load<u32>(gabi::ea(actor) + 0x2e8);
  gabi::Local<be<u32>> local;
  *local = id;
  u32 found = 0;
  if (id != 0xffffffff)
    found = call<u32>(0x025D5218, at<void>(0x025e1234), local.get());
  if (!found)
    return nullptr;
  if (!call<u32>(0x025D4604, at<void>(found)))
    return nullptr;
  if (load<s16>(found + 0xe) != 0xe8)
    return nullptr;
  return at<void>(found);
}
VERIFY(0x02325D3C, bemos_beam);
s32 bemos_blueChargeInit(void *actor) {
  WWHD_FUNC(0x0232666C, s32, actor);
  u32 a = gabi::ea(actor);
  u32 existing = load<u32>(a + 0x4dc);
  store<u32>(a + 0x39c, 1);
  if (!existing) {
    u32 p = call<u32>(0x025200D4);
    u32 e = call<u32>(0x025A847C, at<void>(load<u32>(p + 0x5ab0)), 0, 0x815e,
                      at<void>(a + 0x3bc), 0, 0, 0xff, 0, -1, 0, 0, 0);
    store<u32>(a + 0x4dc, e);
    if (e) {
      store<u8>(e + 0x244, 16);
      store<u8>(e + 0x245, 35);
      store<u8>(e + 0x246, 192);
    }
  }
  u32 old = load<u32>(a + 0x4d4);
  if (old) {
    u32 flags = load<u32>(old + 0x254);
    store<u32>(old + 0x5c, 0xffffffff);
    store<u32>(old + 0x254, flags | 1);
    store<u32>(a + 0x4d4, 0);
  }
  s16 index = load<s16>(a + 0x3ae);
  store<u16>(a + 0x884, 0);
  bool waiting = false;
  if (index == -1) {
    if (index == 0)
      waiting = true;
    else if (load<s16>(a + 0x3ac) == 0 && load<u32>(a + 0x3b0) == 0x23267cc)
      waiting = true;
  }
  if (waiting) {
    store<u16>(a + 0x886, 0);
    s16 timer = load<s16>(0x104692d8);
    store<u16>(a + 0x3ac, 0);
    store<s16>(a + 0x888, timer);
    store<u32>(a + 0x3b0, 0x23268d0);
    store<u16>(a + 0x3ae, 0xffff);
  } else {
    s16 timer = load<s16>(0x104692da);
    store<s16>(a + 0x886, timer);
    timer = load<s16>(0x104692da);
    store<u16>(a + 0x3ac, 0);
    store<s16>(a + 0x888, timer);
    store<u16>(a + 0x3ae, 0xffff);
    store<u32>(a + 0x3b0, 0x23268d0);
  }
  return 1;
}
VERIFY(0x0232666C, bemos_blueChargeInit);
s32 bemos_redChargeInit(void *actor) {
  WWHD_FUNC(0x0232712C, s32, actor);
  u32 a = gabi::ea(actor);
  u32 existing = load<u32>(a + 0x4dc);
  store<u32>(a + 0x39c, 1);
  if (!existing) {
    u32 p = call<u32>(0x025200D4);
    u32 e = call<u32>(0x025A847C, at<void>(load<u32>(p + 0x5ab0)), 0, 0x815e,
                      at<void>(a + 0x3bc), 0, 0, 0xff, 0, -1, 0, 0, 0);
    store<u32>(a + 0x4dc, e);
    if (e) {
      store<u8>(e + 0x244, 192);
      store<u8>(e + 0x245, 16);
      store<u8>(e + 0x246, 35);
    }
  }
  u32 old = load<u32>(a + 0x4d4);
  if (old) {
    u32 flags = load<u32>(old + 0x254);
    store<u32>(old + 0x5c, 0xffffffff);
    store<u32>(old + 0x254, flags | 1);
    store<u32>(a + 0x4d4, 0);
  }
  s16 index = load<s16>(a + 0x3ae);
  store<u16>(a + 0x884, 0);
  bool waiting = false;
  if (index == -1) {
    if (index == 0)
      waiting = true;
    else if (load<s16>(a + 0x3ac) == 0 && load<u32>(a + 0x3b0) == 0x232728c)
      waiting = true;
  }
  if (waiting) {
    store<u16>(a + 0x886, 0);
    s16 timer = load<s16>(0x104692e4);
    store<u16>(a + 0x3ac, 0);
    store<s16>(a + 0x888, timer);
    store<u32>(a + 0x3b0, 0x23273f0);
    store<u16>(a + 0x3ae, 0xffff);
  } else {
    s16 timer = load<s16>(0x104692e6);
    store<s16>(a + 0x886, timer);
    timer = load<s16>(0x104692e6);
    store<u16>(a + 0x3ac, 0);
    store<s16>(a + 0x888, timer);
    store<u16>(a + 0x3ae, 0xffff);
    store<u32>(a + 0x3b0, 0x23273f0);
  }
  return 1;
}
VERIFY(0x0232712C, bemos_redChargeInit);
s32 bemos_dummy(void *actor) {
  WWHD_FUNC(0x023286F8, s32, actor);
  u32 a = gabi::ea(actor);
  if (load<u8>(a + 0x890) == 1)
    store<f32>(a + 0x460, 10.f);
  return 0;
}
VERIFY(0x023286F8, bemos_dummy);
void *bemos_hioCtor(void *obj) {
  WWHD_FUNC(0x02328718, void *, obj);
  u32 a = gabi::ea(obj);
  if (!a) {
    a = call<u32>(0x0273AD10, 0x50);
    if (!a)
      return nullptr;
  }
  store<f32>(a + 0x1c, 0.1f);
  store<f32>(a + 0x10, 0.3f);
  store<f32>(a + 4, 400.f);
  store<u8>(a, 0xff);
  store<f32>(a + 0x2c, 0);
  store<f32>(a + 0x30, 0);
  store<f32>(a + 0x38, 0);
  store<u16>(a + 0x26, 0);
  store<u16>(a + 0x1a, 7);
  store<u16>(a + 0x18, 3);
  store<f32>(a + 8, 900.f);
  store<f32>(a + 0x14, 0.3f);
  store<u16>(a + 0x3e, 30);
  store<u16>(a + 0x3c, 400);
  store<f32>(a + 0x34, 0);
  store<f32>(a + 0x44, 100.f);
  store<f32>(a + 0xc, 0);
  store<f32>(a + 0x40, 200.f);
  store<f32>(a + 0x28, 0);
  store<u8>(a + 0x48, 0);
  store<u32>(a + 0x4c, 0x1002631c);
  store<f32>(a + 0x20, 0.07f);
  store<u16>(a + 0x24, 0);
  return at<void>(a);
}
VERIFY(0x02328718, bemos_hioCtor);
void bemos_sinit() {
  WWHD_FUNC(0x02328808, void);
  store<u32>(0x104692b8, 0);
  store<u32>(0x104692b0, 0);
  store<u32>(0x104692bc, 0);
  store<u32>(0x104692b4, 0);
  call(0x028F026C, at<void>(0x101c80ec));
  store<f32>(0x104692a4, -3.1415927410125732f);
  store<f32>(0x104692a8, 3.1415927410125732f);
  call(0x028ED6F8, at<void>(0x104692ac));
  call(0x028F026C, at<void>(0x101c80f8));
  call(0x028EAB2C, at<void>(0x104692ad));
  call(0x028F026C, at<void>(0x101c8104));
  call(0x02328718, at<void>(0x104692c0));
}
VERIFY(0x02328808, bemos_sinit);
void bemos_copy_matrix(u32 dst) {
  f32 m[12];
  for (int i = 0; i < 12; i++)
    m[i] = load<f32>(0x1048d0cc + i * 4);
  for (int i = 0; i < 12; i++)
    store<f32>(dst + i * 4, m[i]);
}
void bemos_matrix(void *actor) {
  WWHD_FUNC(0x023252C8, void, actor);
  u32 a = gabi::ea(actor);
  f32 x = load<f32>(a + 0x330), y = load<f32>(a + 0x334),
      z = load<f32>(a + 0x338);
  u32 model = load<u32>(a + 0x3d0);
  store<f32>(model + 0xbc, x);
  store<f32>(model + 0xc0, y);
  store<f32>(model + 0xc4, z);
  x = load<f32>(a + 0x314);
  y = load<f32>(a + 0x318);
  z = load<f32>(a + 0x31c);
  call(0x028E93CC, at<void>(0x1048d0cc), x, y, z);
  call(0x025F1B48, at<void>(0x1048d0cc), load<s16>(a + 0x328),
       load<s16>(a + 0x32a), load<s16>(a + 0x32c));
  bemos_copy_matrix(load<u32>(a + 0x3d0) + 0xc8);
  if (load<u8>(a + 0x890) == 2)
    return;
  gabi::Local<cXyz> local;
  local->z = 60.f;
  local->x = 0;
  local->y = 0;
  x = load<f32>(a + 0x538);
  y = load<f32>(a + 0x53c);
  z = load<f32>(a + 0x540);
  model = load<u32>(a + 0x3d4);
  store<f32>(model + 0xbc, x);
  store<f32>(model + 0xc0, y);
  store<f32>(model + 0xc4, z);
  x = load<f32>(a + 0x3bc);
  y = load<f32>(a + 0x3c0);
  z = load<f32>(a + 0x3c4);
  call(0x028E93CC, at<void>(0x1048d0cc), x, y, z);
  s16 sum = (s16)(load<s16>(a + 0x3b6) + load<s16>(a + 0x322));
  call(0x025F1B48, at<void>(0x1048d0cc), load<s16>(a + 0x3b4), sum,
       load<s16>(a + 0x3b8));
  call(0x028E8F64, at<void>(0x1048d0cc), local.get(), at<void>(a + 0x52c));
  bemos_copy_matrix(load<u32>(a + 0x3d4) + 0xc8);
  if (load<u8>(a + 0x890) != 1)
    return;
  x = load<f32>(a + 0x314);
  y = load<f32>(a + 0x318);
  z = load<f32>(a + 0x31c);
  call(0x028E93CC, at<void>(0x1048d0cc), x, y, z);
  sum = (s16)(load<s16>(a + 0x3b6) + load<s16>(a + 0x322));
  call(0x025F1C28, at<void>(0x1048d0cc), sum);
  call(0x025F24E0, 0.f, 0.f, 0.f);
  bemos_copy_matrix(load<u32>(a + 0x3d8) + 0xc8);
}
VERIFY(0x023252C8, bemos_matrix);
s32 bemos_init1(void *actor) {
  WWHD_FUNC(0x02324628, s32, actor);
  u32 a = gabi::ea(actor);
  u8 amount = load<u8>(a + 0xb3);
  if (amount == 255)
    amount = 0;
  store<f32>(a + 0x4f8, gabi::fmadds((f32)amount, 100.f, 500.f));
  call(0x02515F14, at<void>(a + 0x6bc), 255, 255, actor);
  call(0x0251677C, at<void>(a + 0x6f8), at<void>(0x101c7ff8));
  u32 flag = load<u32>(a + 0x78c);
  store<u32>(a + 0x73c, a + 0x6bc);
  store<u32>(a + 0x78c, flag | 2);
  call(0x2324428, actor);
  f32 y = load<f32>(a + 0x318), x = load<f32>(a + 0x314),
      z = load<f32>(a + 0x31c);
  store<f32>(a + 0x3c0, y);
  store<f32>(a + 0x3c4, z);
  store<f32>(a + 0x3bc, x);
  f32 height = load<f32>(0x1047bbfc) + 260.f;
  f32 eyeY = y + height, groundY = y + 50.f;
  store<f32>(a + 0x4ec, 0);
  store<f32>(a + 0x538, 0);
  store<f32>(a + 0x4f0, 0);
  store<f32>(a + 0x848, x);
  store<f32>(a + 0x4f4, 0);
  store<f32>(a + 0x37c, x);
  store<u8>(a + 0x4e8, 255);
  store<u32>(a + 0x82c, load<u32>(a + 4));
  store<f32>(a + 0x3c0, eyeY);
  store<f32>(a + 0x390, x);
  store<f32>(a + 0x398, z);
  store<f32>(a + 0x84c, groundY);
  store<f32>(a + 0x394, eyeY + 60.f);
  store<f32>(a + 0x53c, 0.75f);
  store<f32>(a + 0x850, z);
  store<f32>(a + 0x380, eyeY);
  store<f32>(a + 0x384, z);
  store<u32>(a + 0x87c, 0);
  store<f32>(a + 0x540, 1.f);
  u32 play = call<u32>(0x025200D4);
  f32 ground =
      call<f32>(0x02008974, at<void>(play + 0x12a0), at<void>(a + 0x824));
  store<f32>(a + 0x878, ground);
  gabi::Local<cXyz> scale;
  scale->x = 2.f;
  scale->y = 2.f;
  scale->z = 10.f;
  u32 id = load<u32>(a + 4);
  s8 room = load<s8>(a + 0x1c9);
  u32 child = call<u32>(0x025D5A20, 0xe8, id, 16777216, at<void>(a + 0x3bc),
                        room, at<void>(a + 0x320), scale.get(), -1, 0);
  s16 angle = load<s16>(a + 0x32c);
  store<u32>(a + 0x2e8, child);
  store<u16>(a + 0x32c, 0);
  store<u16>(a + 0x324, 0);
  store<u8>(a + 0x897, (u16)angle & 63);
  store<u8>(a + 0x898, ((u16)angle >> 6) & 127);
  return 1;
}
VERIFY(0x02324628, bemos_init1);
s32 bemos_init2(void *actor) {
  WWHD_FUNC(0x02324A2C, s32, actor);
  u32 a = gabi::ea(actor);
  u32 param = load<u32>(a + 0xb0);
  u8 amount = param;
  s8 direction = (s8)(param >> 8);
  if (amount == 255)
    amount = 0;
  store<f32>(a + 0x4f8, gabi::fmadds((f32)amount, 100.f, 500.f));
  store<u16>(a + 0x88a, direction * 250 + (direction >= 0 ? 750 : -500));
  call(0x02515F14, at<void>(a + 0x6bc), 255, 255, actor);
  call(0x0251677C, at<void>(a + 0x6f8), at<void>(0x101c7ff8));
  u32 flag = load<u32>(a + 0x78c);
  store<u32>(a + 0x73c, a + 0x6bc);
  store<u32>(a + 0x78c, flag | 2);
  call(0x2324830, actor);
  f32 y = load<f32>(a + 0x318), x = load<f32>(a + 0x314),
      z = load<f32>(a + 0x31c);
  store<f32>(a + 0x3c0, y);
  store<f32>(a + 0x3c4, z);
  store<f32>(a + 0x3bc, x);
  f32 height = load<f32>(0x1047bbfc) + 260.f;
  f32 eyeY = y + height, groundY = y + 50.f;
  store<f32>(a + 0x4ec, 0);
  store<f32>(a + 0x538, 0);
  store<f32>(a + 0x4f0, 0);
  store<f32>(a + 0x848, x);
  store<f32>(a + 0x4f4, 0);
  store<f32>(a + 0x37c, x);
  store<u8>(a + 0x4e8, 255);
  store<u32>(a + 0x82c, load<u32>(a + 4));
  store<f32>(a + 0x3c0, eyeY);
  store<f32>(a + 0x390, x);
  store<f32>(a + 0x398, z);
  store<f32>(a + 0x84c, groundY);
  store<f32>(a + 0x394, eyeY + 60.f);
  store<f32>(a + 0x53c, 0.75f);
  store<f32>(a + 0x850, z);
  store<f32>(a + 0x380, eyeY);
  store<f32>(a + 0x384, z);
  store<u32>(a + 0x87c, 0);
  store<f32>(a + 0x540, 1.f);
  u32 play = call<u32>(0x025200D4);
  f32 ground =
      call<f32>(0x02008974, at<void>(play + 0x12a0), at<void>(a + 0x824));
  store<f32>(a + 0x878, ground);
  gabi::Local<cXyz> scale;
  scale->x = 2.f;
  scale->y = 2.f;
  scale->z = 10.f;
  u32 id = load<u32>(a + 4);
  s8 room = load<s8>(a + 0x1c9);
  u32 child = call<u32>(0x025D5A20, 0xe8, id, 0, at<void>(a + 0x3bc), room,
                        at<void>(a + 0x320), scale.get(), -1, 0);
  s16 angle = load<s16>(a + 0x32c);
  store<u32>(a + 0x2e8, child);
  store<u16>(a + 0x32c, 0);
  store<u16>(a + 0x324, 0);
  store<u8>(a + 0x897, (u16)angle & 63);
  store<u8>(a + 0x898, ((u16)angle >> 6) & 127);
  return 1;
}
VERIFY(0x02324A2C, bemos_init2);
s32 bemos_init(void *actor) {
  WWHD_FUNC(0x02325558, s32, actor);
  u32 a = gabi::ea(actor);
  if (load<u8>(a + 0x890) != 2) {
    call(0x02515F14, at<void>(a + 0x550), 255, 255, actor);
    call(0x02516518, at<void>(a + 0x58c), at<void>(0x101c8064));
    u32 flags = load<u32>(a + 0x620);
    store<u32>(a + 0x5d0, a + 0x550);
    store<u32>(a + 0x620, flags | 2);
  }
  store<u16>(a + 0x3b4, load<u16>(0x101ffb14));
  store<u16>(a + 0x3b6, load<u16>(0x101ffb16));
  u8 type = load<u8>(a + 0x890);
  u16 z = load<u16>(0x101ffb18);
  store<u32>(a + 0x4d4, 0);
  store<u16>(a + 0x3b8, z);
  store<u32>(a + 0x4e4, 0);
  store<u32>(a + 0x4d8, 0);
  store<u32>(a + 0x4e0, 0);
  store<u8>(a + 0x894, 1);
  store<u32>(a + 0x4dc, 0);
  if (type == 0)
    call(0x02324628, actor);
  else if (type == 1)
    call(0x02324A2C, actor);
  else if (type == 2)
    call(0x02324DC8, actor);
  call(0x023252C8, actor);
  u32 model = load<u32>(a + 0x3d0);
  if (model)
    model += 0xc8;
  store<u32>(a + 0x348, model);
  return 4;
}
VERIFY(0x02325558, bemos_init);
s32 bemos_delete(void *actor) {
  WWHD_FUNC(0x023258A8, s32, actor);
  u32 a = gabi::ea(actor);
  call(0x025204C8, at<void>(a + 0x3c8), at<void>(0x10026478));
  {
    u32 e = load<u32>(a + 0x4d4);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, 0xffffffff);
      store<u32>(e + 0x254, flags | 1);
    }
  }
  {
    u32 e = load<u32>(a + 0x4d8);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, 0xffffffff);
      store<u32>(e + 0x254, flags | 1);
    }
  }
  {
    u32 e = load<u32>(a + 0x4dc);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, 0xffffffff);
      store<u32>(e + 0x254, flags | 1);
    }
  }
  {
    u32 e = load<u32>(a + 0x4e0);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, 0xffffffff);
      store<u32>(e + 0x254, flags | 1);
    }
  }
  {
    u32 e = load<u32>(a + 0x4e4);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, 0xffffffff);
      store<u32>(e + 0x254, flags | 1);
    }
  }
  store<u32>(a + 0x4d8, 0);
  store<u32>(a + 0x4dc, 0);
  store<u32>(a + 0x4e0, 0);
  store<u32>(a + 0x4e4, 0);
  store<u32>(a + 0x4d4, 0);
  return 1;
}
VERIFY(0x023258A8, bemos_delete);
s32 bemos_create(void *actor) {
  WWHD_FUNC(0x02325698, s32, actor);
  u32 a = gabi::ea(actor);
  if (!load<u32>(0x101fdb68)) {
    store<u32>(0x101fdb68, 1);
    call(0xC000A848, at<void>(0x101fdb6c), at<void>(0x101c8058), 12);
  }
  u32 flags = load<u32>(a + 0x2e4);
  if (!(flags & 8)) {
    if (a) {
      call(0x025D4ED0, actor);
      store<u32>(a + 0xb4, 0x1002630c);
      call(0x025E80D0, at<void>(a + 0x3e0));
      call(0x025E80D0, at<void>(a + 0x45c));
      call(0x0200BD2C, at<void>(a + 0x550));
      call(0x02515DA0, at<void>(a + 0x56c));
      store<u32>(a + 0x568, 0x1004ae88);
      store<u32>(a + 0x56c, 0x1004aec0);
      call(0x02515FB8, at<void>(a + 0x58c));
      store<u32>(a + 0x6a0, 0x100015a8);
      store<u32>(a + 0x69c, 0x1002626c);
      call(0x02018590, at<void>(a + 0x6a4));
      store<u32>(a + 0x5c8, 0x1004b108);
      store<u32>(a + 0x6a0, 0x1004b160);
      store<u32>(a + 0x6b8, 0x1004b150);
      call(0x0200BD2C, at<void>(a + 0x6bc));
      call(0x02515DA0, at<void>(a + 0x6d8));
      store<u32>(a + 0x6d4, 0x1004ae88);
      store<u32>(a + 0x6d8, 0x1004aec0);
      call(0x025166F0, at<void>(a + 0x6f8));
      call(0x02008E0C, at<void>(a + 0x824));
      store<u8>(a + 0x86b, 0);
      store<u8>(a + 0x86c, 0);
      store<u8>(a + 0x868, 1);
      store<u8>(a + 0x869, 0);
      store<u32>(a + 0x874, 1);
      store<u32>(a + 0x844, 0x100262dc);
      store<u8>(a + 0x86e, 0);
      store<u32>(a + 0x824, a + 0x864);
      store<u32>(a + 0x834, 0x100262cc);
      store<u32>(a + 0x828, a + 0x870);
      store<u8>(a + 0x86a, 0);
      store<u32>(a + 0x870, 0x100262ec);
      store<u8>(a + 0x86d, 0);
      flags = load<u32>(a + 0x2e4);
      store<u32>(a + 0x864, 0x100262fc);
    }
    store<u32>(a + 0x2e4, flags | 8);
  }
  s32 phase = call<s32>(0x02520460, at<void>(a + 0x3c8), at<void>(0x10026478));
  u32 type = load<u32>(a + 0xb0) >> 28;
  store<u8>(a + 0x890, type);
  if (phase != 4)
    return phase;
  u32 hint = load<u32>(0x101fdb6c + type * 4);
  if (!call<u32>(0x025D63E8, actor, at<void>(0x02324424), hint))
    return 5;
  return call<s32>(0x02325558, actor);
}
VERIFY(0x02325698, bemos_create);
s32 bemos_execute(void *actor) {
  WWHD_FUNC(0x02326020, s32, actor);
  u32 a = gabi::ea(actor);
  if (load<u8>(a + 0x890) != 2 && call<u32>(0x025162A4, at<void>(a + 0x6f8))) {
    call(0x0232599C, actor);
    call(0x0251621C, at<void>(a + 0x6f8));
  }
  s16 index = load<s16>(a + 0x3ae), adjust = load<s16>(a + 0x3ac);
  u32 obj = a + (s32)adjust, target;
  if (index < 0)
    target = load<u32>(a + 0x3b0);
  else {
    u32 vt = load<u32>(obj + (s32)load<s16>(a + 0x3b2));
    target = load<u32>(vt + (u32)(s32)index * 8 + 4);
  }
  call(target, at<void>(obj));
  call(0x02325B44, actor);
  call(0x023252C8, actor);
  if (load<u8>(a + 0x890) != 2) {
    call(0x02515E50, at<void>(a + 0x56c));
    call(0x02515E50, at<void>(a + 0x6d8));
    if (call<u32>(0x025162A4, at<void>(a + 0x58c))) {
      call(0x02312E54, actor, at<void>(a + 0x58c));
      call(0x023129C4, at<void>(a + 0x314), load<s8>(a + 0x326),
           at<void>(a + 0x58c), 13);
    }
    call(0x020182E0, at<void>(a + 0x6a4), at<void>(a + 0x314));
    u32 play = call<u32>(0x025200D4);
    call(0x0200E240, at<void>(play + 0x26a4), at<void>(a + 0x58c));
  } else if (load<u8>(a + 0x891) != 2)
    call(0x02325DB8, actor);
  call(0x025DA088, actor, 39, 35, 44);
  return 0;
}
VERIFY(0x02326020, bemos_execute);
s32 bemos_draw(void *actor) {
  WWHD_FUNC(0x023261A0, s32, actor);
  u32 a = gabi::ea(actor), env = call<u32>(0x02555D0C);
  call(0x025626A4, at<void>(env), 0, at<void>(a + 0x314), at<void>(a + 0x110));
  env = call<u32>(0x02555D0C);
  call(0x02562F5C, at<void>(env), at<void>(load<u32>(a + 0x3d0)),
       at<void>(a + 0x110));
  call(0x025E2DE0, at<void>(load<u32>(a + 0x3d0)), 0);
  if (load<u8>(a + 0x890) == 2)
    return 1;
  if (load<f32>(a + 0x538) > 0.f) {
    env = call<u32>(0x02555D0C);
    call(0x02562F5C, at<void>(env), at<void>(load<u32>(a + 0x3d4)),
         at<void>(a + 0x110));
    call(0x025E2DE0, at<void>(load<u32>(a + 0x3d4)), 0);
  }
  if (load<u8>(a + 0x890) != 1)
    return 1;
  env = call<u32>(0x02555D0C);
  call(0x02562F5C, at<void>(env), at<void>(load<u32>(a + 0x3d8)),
       at<void>(a + 0x110));
  u32 m = load<u32>(a + 0x3d8);
  f32 frame = load<f32>(a + 0x460);
  call(0x025E83FC, at<void>(a + 0x45c), at<void>(load<u32>(m + 0xac)), frame);
  call(0x025E2DE0, at<void>(load<u32>(a + 0x3d8)), 0);
  return 1;
}
VERIFY(0x023261A0, bemos_draw);
void bemos_event(void *actor) {
  WWHD_FUNC(0x02325B44, void, actor);
  u32 a = gabi::ea(actor);
  if (load<u8>(a + 0x890) != 2)
    return;
  if (load<u8>(a + 0x895)) {
    u32 sw = load<u32>(a + 0x880);
    if (sw == 255)
      return;
    s8 room = load<s8>(a + 0x2fe);
    u32 save = load<u32>(0x101f84dc);
    u8 old = load<u8>(a + 0x892);
    u8 now = call<u32>(0x025BA0C0, at<void>(save + 0x20), sw, room);
    store<u8>(a + 0x892, now);
    bool start = false;
    if (now != old && now == 1) {
      save = load<u32>(0x101f84dc);
      if (!call<u32>(0x025B8B94, at<void>(save + 0x644), 0x1010)) {
        save = load<u32>(0x101f84dc);
        call(0x025B8B68, at<void>(save + 0x644), 0x1010);
        store<u8>(a + 0x893, 1);
        start = true;
      }
    }
    u8 event = load<u8>(a + 0x893);
    if (start || event == 1) {
      if (load<u16>(a + 0xf8) == 2) {
        store<u8>(a + 0x893, 2);
      } else {
        call(0x025D77DC, actor, at<void>(0x10026414), 1, 65535);
        store<u16>(a + 0xfa, load<u16>(a + 0xfa) | 2);
      }
    } else if (event == 2) {
      u32 play = call<u32>(0x025200D4);
      s32 staff = call<s32>(0x02542D88, at<void>(play + 0x52c4),
                            at<void>(0x1002640c), 0, 0);
      play = call<u32>(0x025200D4);
      u32 ended =
          call<u32>(0x0254457C, at<void>(play + 0x52c4), at<void>(0x10026414));
      play = call<u32>(0x025200D4);
      if (ended) {
        store<u16>(play + 0x52b8, load<u16>(play + 0x52b8) | 8);
        store<u8>(a + 0x893, 0);
      } else
        call(0x02543280, at<void>(play + 0x52c4), staff);
    }
    if (load<u8>(a + 0x890) != 2)
      return;
  }
  u32 sw = load<u32>(a + 0x880);
  if (sw == 255)
    return;
  u32 save = load<u32>(0x101f84dc);
  s8 room = load<s8>(a + 0x2fe);
  if (!call<u32>(0x025BA0C0, at<void>(save + 0x20), sw, room))
    return;
  u32 e = load<u32>(a + 0x4d4);
  if (e) {
    u32 flags = load<u32>(e + 0x254);
    store<u32>(e + 0x5c, 0xffffffff);
    store<u32>(e + 0x254, flags | 1);
    store<u32>(a + 0x4d4, 0);
  }
}
VERIFY(0x02325B44, bemos_event);
s32 bemos_yellowWaitInit(void *actor) {
  WWHD_FUNC(0x02324C5C, s32, actor);
  u32 a = gabi::ea(actor);
  if (!load<u32>(a + 0x4d4)) {
    gabi::Local<cXyz> pos, delta, out;
    pos->x = load<f32>(a + 0x314);
    pos->y = load<f32>(a + 0x318);
    pos->z = load<f32>(a + 0x31c);
    delta->x = 0;
    delta->y = 0;
    delta->z = 20;
    call(0x025F1AA4, at<void>(0x1048d0cc),
         s32(s16(load<s16>(a + 0x320) + 0x4000)), s32(load<s16>(a + 0x322)),
         s32(load<s16>(a + 0x324)));
    call(0x028E8F64, at<void>(0x1048d0cc), delta.get(), out.get());
    call(0x028E8D88, pos.get(), out.get(), pos.get());
    u32 play = call<u32>(0x025200D4);
    u32 e = call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x815c,
                      pos.get(), at<void>(a + 0x320), 0, 0xff, 0, -1, 0, 0, 0);
    store<u32>(a + 0x4d4, e);
    if (e) {
      store<u8>(e + 0x244, 180);
      store<u8>(e + 0x246, 120);
      store<u8>(e + 0x245, 180);
      u32 p = load<u32>(a + 0x4d4);
      store<u8>(p + 0x248, 128);
      store<u8>(p + 0x24a, 20);
      store<u8>(p + 0x249, 128);
    }
  }
  u32 e = load<u32>(a + 0x4dc);
  if (e) {
    u32 flags = load<u32>(e + 0x254);
    store<u32>(e + 0x5c, -1);
    store<u32>(e + 0x254, flags | 1);
    store<u32>(a + 0x4dc, 0);
  }
  store<s16>(a + 0x3ac, 0);
  store<s16>(a + 0x3ae, -1);
  store<u32>(a + 0x3b0, 0x02327d98);
  return 1;
}
VERIFY(0x02324C5C, bemos_yellowWaitInit);
s32 bemos_init3(void *actor) {
  WWHD_FUNC(0x02324DC8, s32, actor);
  u32 a = gabi::ea(actor), param = load<u32>(a + 0xb0);
  store<u8>(a + 0x891, param >> 8);
  store<u32>(a + 0x880, param & 255);
  u32 path =
      call<u32>(0x025AAF88, (param >> 16) & 255, s32(load<s8>(a + 0x326)));
  store<f32>(a + 0x4f8, 1000);
  if (load<u8>(a + 0x891) == 255)
    store<u8>(a + 0x891, 0);
  f32 length = 10, offset = 0;
  gabi::Local<cXyz> target, diff, flat;
  bool has = false;
  if (path) {
    u32 points = load<u32>(path + 8);
    f32 x = load<f32>(points + 4), y = load<f32>(points + 8),
        z = load<f32>(points + 12);
    store<f32>(a + 0x314, x);
    store<f32>(a + 0x318, y);
    store<f32>(a + 0x31c, z);
    store<f32>(a + 0x2ec, x);
    store<f32>(a + 0x2f0, y);
    store<f32>(a + 0x2f4, z);
    for (u32 i = 0; i < 3; i++) {
      u16 n = load<u16>(0x101ffb14 + i * 2);
      store<u16>(a + 0x328 + i * 2, n);
      store<u16>(a + 0x320 + i * 2, n);
    }
    points = load<u32>(path + 8);
    target->x = load<f32>(points + 20);
    target->y = load<f32>(points + 24);
    target->z = load<f32>(points + 28);
    call(0x025D5A20, 0xe9, load<u32>(a + 4),
         load<u32>(a + 0x880) | 0x2fff0000 | (u32(load<u8>(a + 0x891)) << 8),
         target.get(), s32(load<s8>(a + 0x1c9)), 0, 0, 2, 0);
    has = true;
  } else if (load<u32>(a + 0x2e8) != 0xffffffff) {
    gabi::Local<be<u32>> id;
    *id = load<u32>(a + 0x2e8);
    u32 parent = call<u32>(0x025D5218, at<void>(0x025E1234), id.get());
    for (u32 i = 0; i < 3; i++) {
      u16 n = load<u16>(0x101ffb14 + i * 2);
      store<u16>(a + 0x328 + i * 2, n);
      store<u16>(a + 0x320 + i * 2, n);
    }
    target->x = load<f32>(parent + 0x314);
    target->y = load<f32>(parent + 0x318);
    target->z = load<f32>(parent + 0x31c);
    has = true;
  }
  if (has) {
    s32 y = call<s32>(0x0200F93C, at<void>(a + 0x314), target.get());
    store<s16>(a + 0x3b6, y);
    s32 x = call<s32>(0x0200F974, at<void>(a + 0x314), target.get());
    s16 z = load<s16>(a + 0x324);
    store<s16>(a + 0x3b4, x);
    store<s16>(a + 0x320, x);
    store<s16>(a + 0x328, x);
    s16 yy = load<s16>(a + 0x3b6);
    store<s16>(a + 0x322, yy);
    store<s16>(a + 0x32a, yy);
    store<s16>(a + 0x32c, z);
    call(0x0201ADE0, target.get(), diff.get(), at<void>(a + 0x314));
    flat->x = diff->x;
    flat->y = 0;
    flat->z = diff->z;
    f32 sq = call<f32>(0x028E8DD0, flat.get());
    store<f32>(a + 0x4f8, call<f32>(0x028F4384, sq));
    sq = call<f32>(0x028E8DD0, diff.get());
    f32 d = call<f32>(0x028F4384, sq);
    length = 0.01f * (d - 180.f);
    offset = path ? 20.f : -20.f;
    store<u8>(a + 0x895, path ? 1 : 0);
  }
  call(0x02018428, at<void>(a + 0x6a4), 60.f);
  store<u32>(a + 0x3c4, load<u32>(a + 0x31c));
  u32 ypos = load<u32>(a + 0x318);
  s32 ax = load<s16>(a + 0x320), ay = load<s16>(a + 0x322),
      az = load<s16>(a + 0x324);
  u32 xpos = load<u32>(a + 0x314);
  store<u32>(a + 0x3c0, ypos);
  store<u32>(a + 0x3bc, xpos);
  gabi::Local<cXyz> v, out;
  v->x = 0;
  v->y = 0;
  v->z = 80;
  call(0x025F1AA4, at<void>(0x1048d0cc), ax, ay, az);
  call(0x028E8F64, at<void>(0x1048d0cc), v.get(), out.get());
  call(0x028E8D88, at<void>(a + 0x3bc), out.get(), at<void>(a + 0x3bc));
  store<u32>(a + 0x87c, 0);
  call(0x02324C5C, actor);
  gabi::Local<cXyz> scale, pos;
  scale->x = 2;
  scale->y = 2;
  scale->z = length;
  v->x = 0;
  v->y = offset;
  v->z = 40;
  call(0x028E93CC, at<void>(0x1048d0cc), load<f32>(a + 0x3bc),
       load<f32>(a + 0x3c0), load<f32>(a + 0x3c4));
  call(0x025F1B48, at<void>(0x1048d0cc), s32(load<s16>(a + 0x3b4)),
       s32(load<s16>(a + 0x3b6)), s32(load<s16>(a + 0x3b8)));
  call(0x028E8F64, at<void>(0x1048d0cc), v.get(), pos.get());
  u32 flags = load<u8>(a + 0x891) == 2 ? 0x11000000 : 0x41000000;
  u32 id = call<u32>(0x025D5A20, 0xe8, load<u32>(a + 4), flags, pos.get(),
                     s32(load<s8>(a + 0x1c9)), at<void>(a + 0x3b4), scale.get(),
                     -1, 0);
  store<u32>(a + 0x2e8, id);
  store<u8>(a + 0x893, 0);
  store<s16>(a + 0x322, load<s16>(a + 0x3b6));
  u32 sw = load<u32>(a + 0x880);
  if (sw != 255)
    store<u8>(a + 0x892,
              call<u32>(0x025BA0C0, at<void>(load<u32>(0x101f84dc) + 0x20), sw,
                        s32(load<s8>(a + 0x2fe))));
  if (load<u8>(a + 0x895) && load<u8>(a + 0x891) != 2) {
    call(0x02515F14, at<void>(a + 0x550), 255, 255, actor);
    call(0x02516518, at<void>(a + 0x58c), at<void>(0x101c80a8));
    store<u32>(a + 0x5d0, a + 0x550);
  }
  store<u32>(a + 0x2e0, load<u32>(a + 0x2e0) & 0xffffffc0);
  return 1;
}
VERIFY(0x02324DC8, bemos_init3);
s32 bemos_blueWait(void *actor) {
  WWHD_FUNC(0x023267CC, s32, actor);
  u32 a = gabi::ea(actor);
  if (call<u32>(0x02325D3C, actor)) {
    gabi::Local<csXyz> angle;
    if (call<u32>(0x02326274, actor, load<u32>(a + 0x87c), angle.get())) {
      call(0x0232666C, actor);
      return 1;
    }
    call(0x0200F4FC, at<void>(a + 0x4e8), 255, 15);
    u8 alpha = load<u8>(a + 0x4e8);
    u32 e = load<u32>(a + 0x4d8);
    if (alpha == 255)
      store<u32>(a + 0x39c, 0);
    if (e) {
      f32 y = load<f32>(a + 0x530), x = load<f32>(a + 0x52c),
          z = load<f32>(a + 0x534);
      u8 kind = load<u8>(e + 0x262);
      store<f32>(e + 0x22c, x);
      store<f32>(e + 0x230, y);
      store<f32>(e + 0x234, z);
      if (kind >= 7)
        store<f32>(e + 0x230, -load<f32>(e + 0x230));
      store<u8>(load<u32>(a + 0x4d8) + 0x247, load<u8>(a + 0x4e8));
    }
  }
  return 0;
}
VERIFY(0x023267CC, bemos_blueWait);
s32 bemos_yellowSearchInit(void *actor) {
  WWHD_FUNC(0x02327CB8, s32, actor);
  u32 a = gabi::ea(actor);
  if (!load<u32>(a + 0x4dc)) {
    u32 play = call<u32>(0x025200D4);
    u32 e = call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x815e,
                      at<void>(a + 0x3bc), 0, 0, 255, 0, -1, 0, 0, 0);
    store<u32>(a + 0x4dc, e);
    if (e) {
      store<u8>(e + 0x244, 160);
      store<u8>(e + 0x245, 192);
      store<u8>(e + 0x246, 20);
    }
  }
  u32 e = load<u32>(a + 0x4d4);
  if (e) {
    u32 flags = load<u32>(e + 0x254);
    store<u32>(e + 0x5c, -1);
    store<u32>(e + 0x254, flags | 1);
    store<u32>(a + 0x4d4, 0);
  }
  store<s16>(a + 0x884, 0);
  store<s16>(a + 0x3ac, 0);
  store<s16>(a + 0x3ae, -1);
  store<u32>(a + 0x3b0, 0x02327f08);
  return 1;
}
VERIFY(0x02327CB8, bemos_yellowSearchInit);
s32 bemos_redWait(void *actor) {
  WWHD_FUNC(0x0232728C, s32, actor);
  u32 a = gabi::ea(actor), beam = call<u32>(0x02325D3C, actor);
  call(0x025E742C, at<void>(a + 0x45c));
  if (!beam)
    return 0;
  gabi::Local<csXyz> angle;
  if (call<u32>(0x02326460, actor, load<u32>(a + 0x87c), angle.get())) {
    call(0x0232712C, actor);
    return 1;
  }
  call(0x0200F428, at<void>(a + 0x3b4), 0, 4, 0x400);
  s16 delta = load<s16>(a + 0x88a), old = load<s16>(a + 0x3b6);
  store<s16>(a + 0x3b6, old + delta);
  call(0x0200F4FC, at<void>(a + 0x4e8), 255, 15);
  u8 alpha = load<u8>(a + 0x4e8);
  u32 e = load<u32>(a + 0x4d8);
  if (alpha == 255)
    store<u32>(a + 0x39c, 0);
  if (e) {
    f32 y = load<f32>(a + 0x530), x = load<f32>(a + 0x52c),
        z = load<f32>(a + 0x534);
    u8 kind = load<u8>(e + 0x262);
    store<f32>(e + 0x22c, x);
    store<f32>(e + 0x230, y);
    store<f32>(e + 0x234, z);
    if (kind >= 7)
      store<f32>(e + 0x230, -load<f32>(e + 0x230));
    store<u8>(load<u32>(a + 0x4d8) + 0x247, load<u8>(a + 0x4e8));
  }
  return 1;
}
VERIFY(0x0232728C, bemos_redWait);
s32 bemos_breakInit(void *actor) {
  WWHD_FUNC(0x0232599C, s32, actor);
  u32 a = gabi::ea(actor);
  bool blue = false;
  if (!load<u32>(a + 0x4dc)) {
    u32 play = call<u32>(0x025200D4);
    u32 e = call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x815e,
                      at<void>(a + 0x3bc), 0, 0, 255, 0, -1, 0, 0, 0);
    store<u32>(a + 0x4dc, e);
    if (e) {
      blue = load<u8>(a + 0x890) == 0;
      store<u8>(e + 0x244, blue ? 16 : 192);
      store<u8>(e + 0x245, blue ? 35 : 16);
      store<u8>(e + 0x246, blue ? 192 : 35);
    }
  }
  u32 play = call<u32>(0x025200D4);
  call(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 13,
       at<void>(a + 0x3bc), 0, 0, 255, 0, -1, 0, 0, 0);
  u32 flags = load<u32>(a + 0x2e0);
  store<u32>(a + 0x39c, 0);
  store<s16>(a + 0x884, 0);
  store<u32>(a + 0x3b0, 0x02328120);
  store<u32>(a + 0x2e0, flags & 0xffffffc0);
  store<s16>(a + 0x3ac, 0);
  store<s16>(a + 0x3ae, -1);
  return 1;
}
VERIFY(0x0232599C, bemos_breakInit);
void bemos_guard(void *actor) {
  WWHD_FUNC(0x02325DB8, void, actor);
  u32 a = gabi::ea(actor);
  if (load<u8>(a + 0x890) != 2 || load<u8>(a + 0x895) != 1)
    return;
  u32 beam = call<u32>(0x02325D3C, actor);
  if (!beam || !load<s16>(beam + 0x844))
    return;
  u32 play = call<u32>(0x025200D4);
  gabi::Local<cXyz> diff, pos, v0, v1, out0, out1;
  call(0x0201ADE0, at<void>(load<u32>(play + 0x5b34) + 0x314), diff.get(),
       at<void>(a + 0x314));
  v0->x = load<f32>(0x101ffbcc);
  v0->z = load<f32>(0x101ffbd4);
  v0->y = load<f32>(0x101ffbd0);
  v1->x = load<f32>(0x101ffbb4);
  v1->y = load<f32>(0x101ffbb8);
  v1->z = load<f32>(0x101ffbbc);
  pos->x = load<f32>(a + 0x314);
  pos->y = load<f32>(a + 0x318);
  pos->z = load<f32>(a + 0x31c);
  call(0x025F1AA4, at<void>(0x1048d0cc), s32(load<s16>(a + 0x320)),
       s32(load<s16>(a + 0x322)), s32(load<s16>(a + 0x324)));
  call(0x028E8F64, at<void>(0x1048d0cc), v0.get(), out0.get());
  call(0x028E8F64, at<void>(0x1048d0cc), v1.get(), out1.get());
  f32 d0 = call<f32>(0x028E8F44, diff.get(), out0.get()),
      d1 = call<f32>(0x028E8F44, diff.get(), out1.get());
  call(0x028E8E64, out0.get(), out0.get(), d0);
  f32 scale = load<f32>(0x10469300) - 20.f;
  if (d1 > 0)
    scale = -scale;
  call(0x028E8E64, out1.get(), out1.get(), scale);
  call(0x028E8D88, pos.get(), out0.get(), pos.get());
  call(0x028E8D88, pos.get(), out1.get(), pos.get());
  play = call<u32>(0x025200D4);
  bool low = (load<u32>(play + 0x5cdc) & 32) != 0;
  if (low)
    pos->y = f32(pos->y) - 50.f;
  else
    pos->y = gabi::fnmsubs(load<f32>(0x10469304), 0.5f, pos->y);
  call(0x020182E0, at<void>(a + 0x6a4), pos.get());
  call(0x020184DC, at<void>(a + 0x6a4), load<f32>(0x10469300));
  call(0x02018428, at<void>(a + 0x6a4), low ? 100.f : load<f32>(0x10469304));
  play = call<u32>(0x025200D4);
  call(0x0200E240, at<void>(play + 0x26a4), at<void>(a + 0x58c));
}
VERIFY(0x02325DB8, bemos_guard);
s32 bemos_blueRange(void *actor, s32 active, csXyz *angle) {
  WWHD_FUNC(0x02326274, s32, actor, active, angle);
  u32 a = gabi::ea(actor), play = call<u32>(0x025200D4),
      player = load<u32>(play + 0x5b2c), vt = load<u32>(player + 0xb4),
      f = load<u32>(vt + 0x24);
  f32 x = load<f32>(player + 0x314), y = call<f32>(f, at<void>(player)),
      z = load<f32>(player + 0x31c);
  gabi::Local<cXyz> pos, diff, v0, v1;
  pos->x = x;
  pos->y = y;
  pos->z = z;
  call(0x0201ADE0, pos.get(), diff.get(), at<void>(a + 0x314));
  u16 yaw = load<u16>(a + 0x322);
  f32 range = load<f32>(a + 0x4f8), width = load<f32>(0x104692c4);
  u32 trig = 0x104a44f8 + ((yaw >> 3) * 8);
  f32 sine = load<f32>(trig), cosine = load<f32>(trig + 4);
  v0->x = sine;
  v0->y = 0;
  v0->z = cosine;
  v1->x = cosine;
  v1->y = 0;
  v1->z = -sine;
  f32 forward = call<f32>(0x028E8F44, v0.get(), diff.get()),
      side = call<f32>(0x028E8F44, v1.get(), diff.get());
  f32 mag = call<f32>(0x028F4384, gabi::fmadds(forward, forward, side * side)),
      dy = diff->y;
  if (active) {
    width += load<f32>(0x104692ec);
    range += load<f32>(0x104692f0);
  }
  if (forward < range && forward > 0 && __builtin_fabsf(side) < width &&
      __builtin_fabsf(dy) < 250.f) {
    f32 base = load<f32>(a + 0x3c0) - load<f32>(a + 0x318);
    s32 pitch = call<s32>(0x020195B0, dy + base, mag);
    store<s16>(gabi::ea(angle), pitch);
    store<u32>(a + 0x87c, 1);
    return 1;
  }
  store<u32>(a + 0x87c, 0);
  return 0;
}
VERIFY(0x02326274, bemos_blueRange);
s32 bemos_redRange(void *actor, s32 active, csXyz *angle) {
  WWHD_FUNC(0x02326460, s32, actor, active, angle);
  u32 a = gabi::ea(actor), play = call<u32>(0x025200D4),
      player = load<u32>(play + 0x5b2c), vt = load<u32>(player + 0xb4),
      f = load<u32>(vt + 0x24);
  f32 x = load<f32>(player + 0x314), y = call<f32>(f, at<void>(player)),
      z = load<f32>(player + 0x31c);
  gabi::Local<cXyz> pos, diff, flat, move;
  pos->x = x;
  pos->y = y;
  pos->z = z;
  call(0x0201ADE0, pos.get(), diff.get(), at<void>(a + 0x314));
  flat->x = diff->x;
  flat->y = 0;
  flat->z = diff->z;
  f32 range = load<f32>(a + 0x4f8), sq = call<f32>(0x028E8DD0, flat.get()),
      dist = call<f32>(0x028F4384, sq);
  s32 direction = call<s32>(0x020195B0, f32(diff->x), f32(diff->z));
  s16 relative = s16(direction - load<s16>(a + 0x322)),
      delta = s16(relative - load<s16>(a + 0x3b6));
  s32 absdelta = delta < 0 ? -s32(delta) : s32(delta);
  bool eligible = true;
  if (!load<u8>(a + 0x894)) {
    call(0x0201ADE0, at<void>(player + 0x314), move.get(), at<void>(a + 0x544));
    sq = call<f32>(0x028E8DD0, move.get());
    f32 moved = call<f32>(0x028F4384, sq);
    if (moved > 5.f)
      store<u8>(a + 0x894, 1);
    else if (absdelta > 0x1000)
      store<u8>(a + 0x894, 1);
    else
      eligible = false;
  }
  if (eligible) {
    if (active)
      range += load<f32>(0x104692e8);
    if ((absdelta < 0x1000 || active == 1) && dist < range) {
      f32 base = load<f32>(a + 0x3c0) - load<f32>(a + 0x318);
      s32 pitch = call<s32>(0x020195B0, base - f32(diff->y), dist);
      store<s16>(gabi::ea(angle), pitch);
      store<s16>(gabi::ea(angle) + 2, relative);
      store<u32>(a + 0x87c, 1);
      return 1;
    }
  }
  store<u32>(a + 0x87c, 0);
  return 0;
}
VERIFY(0x02326460, bemos_redRange);
s32 bemos_yellowWait(void *actor) {
  WWHD_FUNC(0x02327D98, s32, actor);
  u32 a = gabi::ea(actor), beam = call<u32>(0x02325D3C, actor);
  if (!beam)
    return 0;
  u32 sw = load<u32>(a + 0x880);
  bool off = false;
  if (sw != 255)
    off = call<u32>(0x025BA0C0, at<void>(load<u32>(0x101f84dc) + 0x20), sw,
                    s32(load<s8>(a + 0x2fe))) != 0;
  if (!off && load<u8>(a + 0x891) == 1)
    off = load<s8>(0x101d5f46) != load<s8>(a + 0x326);
  if (off) {
    s16 state = load<s16>(beam + 0x844);
    if (!state)
      return 0;
    u32 e = load<u32>(beam + 0x8e0);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, -1);
      store<u32>(e + 0x254, flags | 1);
      state = load<s16>(beam + 0x844);
      store<u32>(beam + 0x8e0, 0);
    }
    if (state == 1) {
      f32 x = load<f32>(beam + 0x718);
      if (x < 5.f)
        store<f32>(beam + 0x718, x + 1.f);
      f32 z = load<f32>(beam + 0x798);
      if (z < 4.f) {
        store<f32>(beam + 0x798, z + 1.f);
        return 0;
      }
      store<f32>(beam + 0x718, 0);
      store<f32>(beam + 0x798, 0);
      store<s16>(beam + 0x844, 0);
    } else {
      store<f32>(beam + 0x718, 0);
      store<f32>(beam + 0x798, 0);
    }
    return 0;
  }
  if (call<u32>(0x0232665C, actor, load<u32>(a + 0x87c), 0))
    call(0x02327CB8, actor);
  return 0;
}
VERIFY(0x02327D98, bemos_yellowWait);
s32 bemos_yellowSearch(void *actor) {
  WWHD_FUNC(0x02327F08, s32, actor);
  u32 a = gabi::ea(actor), beam = call<u32>(0x02325D3C, actor);
  if (!beam)
    return 0;
  bool on = call<u32>(0x0232665C, actor, load<u32>(a + 0x87c), 0) != 0;
  if (on) {
    u32 sw = load<u32>(a + 0x880);
    if (sw != 255)
      on = call<u32>(0x025BA0C0, at<void>(load<u32>(0x101f84dc) + 0x20), sw,
                     s32(load<s8>(a + 0x2fe))) == 0;
  }
  if (on) {
    if (!load<u32>(beam + 0x8e0)) {
      u32 play = call<u32>(0x025200D4);
      u32 e =
          call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x8121,
                    at<void>(beam + 0x314), 0, 0, 255, 0, -1, 0, 0, 0);
      store<u32>(beam + 0x8e0, e);
    }
    s16 state = load<s16>(beam + 0x844);
    store<f32>(beam + 0x798, 0);
    if (!state) {
      f32 x = load<f32>(beam + 0x718);
      if (x < 5.f) {
        store<f32>(beam + 0x718, x + 1.f);
        return 0;
      }
      store<f32>(beam + 0x798, 0);
      store<f32>(beam + 0x718, 5);
      store<s16>(beam + 0x844, 1);
    } else
      store<f32>(beam + 0x718, 5);
    return 0;
  }
  u32 e = load<u32>(beam + 0x8e0);
  if (e) {
    u32 flags = load<u32>(e + 0x254);
    store<u32>(e + 0x5c, -1);
    store<u32>(e + 0x254, flags | 1);
    store<u32>(beam + 0x8e0, 0);
  }
  if (load<s16>(beam + 0x844) == 1) {
    f32 x = load<f32>(beam + 0x718);
    if (x < 5.f)
      store<f32>(beam + 0x718, x + 1.f);
    f32 z = load<f32>(beam + 0x798);
    if (z < 4.f) {
      store<f32>(beam + 0x798, z + 1.f);
      return 0;
    }
    store<f32>(beam + 0x798, 0);
    store<f32>(beam + 0x718, 0);
    store<s16>(beam + 0x844, 0);
    call(0x02324C5C, actor);
  } else {
    store<f32>(beam + 0x718, 0);
    store<f32>(beam + 0x798, 0);
    call(0x02324C5C, actor);
  }
  return 0;
}
VERIFY(0x02327F08, bemos_yellowSearch);
s32 bemos_blueSearch(void *actor) {
  WWHD_FUNC(0x02326E4C, s32, actor);
  u32 a = gabi::ea(actor), beam = call<u32>(0x02325D3C, actor);
  if (!beam)
    return 0;
  gabi::Local<csXyz> angle, beamangle;
  bool on =
      call<u32>(0x02326274, actor, load<u32>(a + 0x87c), angle.get()) != 0;
  if (on) {
    call(0x0200F428, at<void>(a + 0x3b4), s32(s16(angle->x)), 4, 0x400);
    s16 z = load<s16>(a + 0x3b8),
        yaw = s16(load<s16>(a + 0x3b6) + load<s16>(a + 0x322));
    beamangle->x = load<s16>(a + 0x3b4);
    beamangle->z = z;
    if (!load<u32>(beam + 0x8e0)) {
      u32 play = call<u32>(0x025200D4);
      u32 e =
          call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x8121,
                    at<void>(beam + 0x314), 0, 0, 255, 0, -1, 0, 0, 0);
      store<u32>(beam + 0x8e0, e);
    }
    s16 state = load<s16>(beam + 0x844);
    store<f32>(beam + 0x798, 0);
    f32 width = 5;
    if (!state) {
      f32 old = load<f32>(beam + 0x718);
      if (old < 5.f)
        width = old + 1.f;
      else {
        store<f32>(beam + 0x798, 0);
        store<s16>(beam + 0x844, 1);
      }
    }
    store<f32>(beam + 0x718, width);
    for (u32 i = 0; i < 3; i++)
      store<u32>(beam + 0x314 + i * 4, load<u32>(a + 0x52c + i * 4));
    store<s16>(beam + 0x322, yaw);
    store<s16>(beam + 0x320, beamangle->x);
    store<s16>(beam + 0x324, beamangle->z);
  } else {
    u32 e = load<u32>(beam + 0x8e0);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, -1);
      store<u32>(e + 0x254, flags | 1);
      store<u32>(beam + 0x8e0, 0);
    }
    if (load<s16>(beam + 0x844) == 1) {
      f32 old = load<f32>(beam + 0x718);
      if (old < 5.f)
        store<f32>(beam + 0x718, old + 1.f);
      f32 height = load<f32>(beam + 0x798);
      if (height < 4.f)
        store<f32>(beam + 0x798, height + 1.f);
      else {
        store<f32>(beam + 0x798, 0);
        store<f32>(beam + 0x718, 0);
        store<s16>(beam + 0x844, 0);
        call(0x0232666C, actor);
      }
    } else {
      store<f32>(beam + 0x718, 0);
      store<f32>(beam + 0x798, 0);
      call(0x0232666C, actor);
    }
  }
  call(0x02018D40, at<void>(a + 0x810), at<void>(a + 0x3bc));
  u32 play = call<u32>(0x025200D4);
  call(0x0200E240, at<void>(play + 0x26a4), at<void>(a + 0x6f8));
  return 0;
}
VERIFY(0x02326E4C, bemos_blueSearch);
s32 bemos_redSearch(void *actor) {
  WWHD_FUNC(0x02327968, s32, actor);
  u32 a = gabi::ea(actor);
  call(0x025E742C, at<void>(a + 0x45c));
  u32 beam = call<u32>(0x02325D3C, actor);
  if (!beam)
    return 0;
  gabi::Local<csXyz> angle, beamangle;
  bool on =
      call<u32>(0x02326460, actor, load<u32>(a + 0x87c), angle.get()) != 0;
  if (on) {
    call(0x0200F428, at<void>(a + 0x3b4), s32(s16(angle->x)), 4, 0x400);
    call(0x0200F428, at<void>(a + 0x3b6), s32(s16(angle->y)), 1,
         s32(load<s16>(0x104692fc)));
    s16 z = load<s16>(a + 0x3b8),
        yaw = s16(load<s16>(a + 0x3b6) + load<s16>(a + 0x322));
    beamangle->x = load<s16>(a + 0x3b4);
    beamangle->z = z;
    if (!load<u32>(beam + 0x8e0)) {
      u32 play = call<u32>(0x025200D4);
      u32 e =
          call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x8121,
                    at<void>(beam + 0x314), 0, 0, 255, 0, -1, 0, 0, 0);
      store<u32>(beam + 0x8e0, e);
    }
    s16 state = load<s16>(beam + 0x844);
    store<f32>(beam + 0x798, 0);
    f32 width = 5;
    if (!state) {
      f32 old = load<f32>(beam + 0x718);
      if (old < 5.f)
        width = old + 1.f;
      else {
        store<f32>(beam + 0x798, 0);
        store<s16>(beam + 0x844, 1);
      }
    }
    store<f32>(beam + 0x718, width);
    for (u32 i = 0; i < 3; i++)
      store<u32>(beam + 0x314 + i * 4, load<u32>(a + 0x52c + i * 4));
    store<s16>(beam + 0x322, yaw);
    store<s16>(beam + 0x320, beamangle->x);
    store<s16>(beam + 0x324, beamangle->z);
    s16 timer = load<s16>(a + 0x88e);
    if (timer <= 0) {
      u32 play = call<u32>(0x025200D4), player = load<u32>(play + 0x5b2c);
      store<u32>(a + 0x544, load<u32>(player + 0x314));
      store<u32>(a + 0x548, load<u32>(player + 0x318));
      u32 z = load<u32>(player + 0x31c);
      store<u8>(a + 0x894, 0);
      store<u32>(a + 0x54c, z);
    } else
      store<s16>(a + 0x88e, timer - 1);
  } else {
    u32 e = load<u32>(beam + 0x8e0);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, -1);
      store<u32>(e + 0x254, flags | 1);
      store<u32>(beam + 0x8e0, 0);
    }
    if (load<s16>(beam + 0x844) == 1) {
      f32 old = load<f32>(beam + 0x718);
      if (old < 5.f)
        store<f32>(beam + 0x718, old + 1.f);
      f32 height = load<f32>(beam + 0x798);
      if (height < 4.f)
        store<f32>(beam + 0x798, height + 1.f);
      else {
        store<f32>(beam + 0x798, 0);
        store<f32>(beam + 0x718, 0);
        store<s16>(beam + 0x844, 0);
        call(0x0232712C, actor);
      }
    } else {
      store<f32>(beam + 0x718, 0);
      store<f32>(beam + 0x798, 0);
      call(0x0232712C, actor);
    }
  }
  call(0x02018D40, at<void>(a + 0x810), at<void>(a + 0x3bc));
  u32 play = call<u32>(0x025200D4);
  call(0x0200E240, at<void>(play + 0x26a4), at<void>(a + 0x6f8));
  return 0;
}
VERIFY(0x02327968, bemos_redSearch);
s32 bemos_blueCharge(void *actor) {
  WWHD_FUNC(0x023268D0, s32, actor);
  u32 a = gabi::ea(actor);
  gabi::Local<csXyz> angle;
  bool on =
      call<u32>(0x02326274, actor, load<u32>(a + 0x87c), angle.get()) != 0;
  if (on) {
    call(0x0200F5C8, at<void>(a + 0x538), 1.f, load<f32>(0x104692d0));
    call(0x0200F5C8, at<void>(a + 0x53c), 1.f, 0.25f * load<f32>(0x104692d0));
    s16 count = load<s16>(a + 0x886), limit = load<s16>(a + 0x888);
    if (count > limit) {
      if (!load<u32>(a + 0x4e4)) {
        u32 play = call<u32>(0x025200D4);
        u32 e =
            call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x8121,
                      at<void>(a + 0x52c), 0, 0, 255, 0, -1, 0, 0, 0);
        store<u32>(a + 0x4e4, e);
      }
      if (load<f32>(a + 0x538) == 1.f) {
        s16 timer = s16(load<s16>(a + 0x884) + 1);
        store<s16>(a + 0x884, timer);
        if (timer >= 5) {
          u32 e = load<u32>(a + 0x4e4);
          if (e) {
            u32 flags = load<u32>(e + 0x254);
            store<u32>(e + 0x5c, -1);
            store<u32>(e + 0x254, flags | 1);
            store<u32>(a + 0x4e4, 0);
          }
          call(0x023268A8, actor);
        }
      }
    } else
      store<s16>(a + 0x886, count + 1);
    call(0x0200F4FC, at<void>(a + 0x4e8), 0, 30);
    call(0x0200F428, at<void>(a + 0x3b4), s32(s16(angle->x)), 4, 0x400);
  } else {
    s16 count = load<s16>(a + 0x886);
    if (count <= 0) {
      call(0x0200F5C8, at<void>(a + 0x538), 0.f, load<f32>(0x104692d4));
      call(0x0200F5C8, at<void>(a + 0x53c), 0.75f,
           0.25f * load<f32>(0x104692d4));
      if (load<f32>(a + 0x538) == 0) {
        u32 e = load<u32>(a + 0x4e4);
        if (e) {
          u32 flags = load<u32>(e + 0x254);
          store<u32>(e + 0x5c, -1);
          store<u32>(e + 0x254, flags | 1);
          store<u32>(a + 0x4e4, 0);
        }
        call(0x02324428, actor);
      }
      call(0x0200F4FC, at<void>(a + 0x4e8), 255, 15);
      call(0x0200F428, at<void>(a + 0x3b4), 0, 4, 0x400);
      s16 timer = load<s16>(a + 0x884);
      if (timer > 0)
        store<s16>(a + 0x884, timer - 1);
    } else
      store<s16>(a + 0x886, count - 1);
  }
  f32 t = f32(load<s16>(a + 0x884)) - 2.5f,
      scale = gabi::fnmsubs(t, t, 6.25f) * 1.6f;
  store<f32>(a + 0x4ec, scale);
  store<f32>(a + 0x4f0, scale);
  store<f32>(a + 0x4f4, scale);
  call(0x02018D40, at<void>(a + 0x810), at<void>(a + 0x3bc));
  u32 play = call<u32>(0x025200D4);
  call(0x0200E240, at<void>(play + 0x26a4), at<void>(a + 0x6f8));
  u32 e = load<u32>(a + 0x4d8);
  if (e) {
    f32 y = load<f32>(a + 0x530), z = load<f32>(a + 0x534),
        x = load<f32>(a + 0x52c);
    u8 kind = load<u8>(e + 0x262);
    store<f32>(e + 0x22c, x);
    store<f32>(e + 0x230, y);
    store<f32>(e + 0x234, z);
    if (kind >= 7)
      store<f32>(e + 0x230, -load<f32>(e + 0x230));
    store<u8>(load<u32>(a + 0x4d8) + 0x247, load<u8>(a + 0x4e8));
  }
  e = load<u32>(a + 0x4e4);
  if (e) {
    f32 x = load<f32>(a + 0x52c), z = load<f32>(a + 0x534),
        y = load<f32>(a + 0x530);
    s32 rz = load<s16>(a + 0x3b8), rx = load<s16>(a + 0x3b4),
        ry = s16(load<s16>(a + 0x3b6) + load<s16>(a + 0x322));
    u8 kind = load<u8>(e + 0x262);
    store<f32>(e + 0x22c, x);
    store<f32>(e + 0x230, y);
    store<f32>(e + 0x234, z);
    if (kind >= 7)
      store<f32>(e + 0x230, -load<f32>(e + 0x230));
    call(0x028245AC, rx, ry, rz, at<void>(load<u32>(a + 0x4e4) + 0x1f0));
    x = load<f32>(a + 0x4ec);
    y = load<f32>(a + 0x4f0);
    e = load<u32>(a + 0x4e4);
    z = load<f32>(a + 0x4f4);
    store<f32>(e + 0x220, x);
    store<f32>(e + 0x224, y);
    store<f32>(e + 0x228, z);
    store<f32>(e + 0x238, x);
    store<f32>(e + 0x23c, y);
    store<f32>(e + 0x240, z);
  }
  return 0;
}
VERIFY(0x023268D0, bemos_blueCharge);
s32 bemos_redCharge(void *actor) {
  WWHD_FUNC(0x023273F0, s32, actor);
  u32 a = gabi::ea(actor);
  call(0x025E742C, at<void>(a + 0x45c));
  gabi::Local<csXyz> angle;
  bool on =
      call<u32>(0x02326460, actor, load<u32>(a + 0x87c), angle.get()) != 0;
  if (on) {
    call(0x0200F5C8, at<void>(a + 0x538), 1.f, load<f32>(0x104692dc));
    call(0x0200F5C8, at<void>(a + 0x53c), 1.f, 0.25f * load<f32>(0x104692dc));
    s16 count = load<s16>(a + 0x886), limit = load<s16>(0x104692e4);
    if (count > limit) {
      call(0x0200F378, at<void>(a + 0x3b4), s32(s16(angle->x)), 2, 0x400, 0);
      if (!load<u32>(a + 0x4e4)) {
        u32 play = call<u32>(0x025200D4);
        u32 e =
            call<u32>(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x8121,
                      at<void>(a + 0x52c), 0, 0, 255, 0, -1, 0, 0, 0);
        store<u32>(a + 0x4e4, e);
      }
      if (load<f32>(a + 0x538) == 1.f) {
        s16 timer = s16(load<s16>(a + 0x884) + 1);
        store<s16>(a + 0x884, timer);
        if (timer >= 5) {
          u32 e = load<u32>(a + 0x4e4);
          if (e) {
            u32 flags = load<u32>(e + 0x254);
            store<u32>(e + 0x5c, -1);
            store<u32>(e + 0x254, flags | 1);
            store<u32>(a + 0x4e4, 0);
          }
          call(0x023273C0, actor);
        }
      }
    } else
      store<s16>(a + 0x886, count + 1);
    call(0x0200F4FC, at<void>(a + 0x4e8), 0, 30);

  } else {
    s16 count = load<s16>(a + 0x886);
    if (count <= 0) {
      call(0x0200F5C8, at<void>(a + 0x538), 0.f, load<f32>(0x104692e0));
      call(0x0200F5C8, at<void>(a + 0x53c), 0.75f,
           0.25f * load<f32>(0x104692e0));
      if (load<f32>(a + 0x538) == 0) {
        u32 e = load<u32>(a + 0x4e4);
        if (e) {
          u32 flags = load<u32>(e + 0x254);
          store<u32>(e + 0x5c, -1);
          store<u32>(e + 0x254, flags | 1);
          store<u32>(a + 0x4e4, 0);
        }
        call(0x02324830, actor);
      }
      call(0x0200F4FC, at<void>(a + 0x4e8), 255, 15);
      call(0x0200F428, at<void>(a + 0x3b4), 0, 4, 0x400);
      s16 timer = load<s16>(a + 0x884);
      if (timer > 0)
        store<s16>(a + 0x884, timer - 1);
    } else
      store<s16>(a + 0x886, count - 1);
  }
  call(0x02018D40, at<void>(a + 0x810), at<void>(a + 0x3bc));
  u32 play = call<u32>(0x025200D4);
  call(0x0200E240, at<void>(play + 0x26a4), at<void>(a + 0x6f8));
  f32 t = f32(load<s16>(a + 0x884)) - 2.5f,
      scale = gabi::fnmsubs(t, t, 6.25f) * 1.6f;
  store<f32>(a + 0x4ec, scale);
  store<f32>(a + 0x4f0, scale);
  store<f32>(a + 0x4f4, scale);
  u32 e = load<u32>(a + 0x4d8);
  if (e) {
    f32 y = load<f32>(a + 0x530), z = load<f32>(a + 0x534),
        x = load<f32>(a + 0x52c);
    u8 kind = load<u8>(e + 0x262);
    store<f32>(e + 0x22c, x);
    store<f32>(e + 0x230, y);
    store<f32>(e + 0x234, z);
    if (kind >= 7)
      store<f32>(e + 0x230, -load<f32>(e + 0x230));
    store<u8>(load<u32>(a + 0x4d8) + 0x247, load<u8>(a + 0x4e8));
  }
  e = load<u32>(a + 0x4e4);
  if (e) {
    f32 x = load<f32>(a + 0x52c), z = load<f32>(a + 0x534),
        y = load<f32>(a + 0x530);
    s32 rz = load<s16>(a + 0x3b8), rx = load<s16>(a + 0x3b4),
        ry = s16(load<s16>(a + 0x3b6) + load<s16>(a + 0x322));
    u8 kind = load<u8>(e + 0x262);
    store<f32>(e + 0x22c, x);
    store<f32>(e + 0x230, y);
    store<f32>(e + 0x234, z);
    if (kind >= 7)
      store<f32>(e + 0x230, -load<f32>(e + 0x230));
    call(0x028245AC, rx, ry, rz, at<void>(load<u32>(a + 0x4e4) + 0x1f0));
    x = load<f32>(a + 0x4ec);
    y = load<f32>(a + 0x4f0);
    e = load<u32>(a + 0x4e4);
    z = load<f32>(a + 0x4f4);
    store<f32>(e + 0x220, x);
    store<f32>(e + 0x224, y);
    store<f32>(e + 0x228, z);
    store<f32>(e + 0x238, x);
    store<f32>(e + 0x23c, y);
    store<f32>(e + 0x240, z);
  }
  return 1;
}
VERIFY(0x023273F0, bemos_redCharge);
s32 bemos_break(void *actor) {
  WWHD_FUNC(0x02328120, s32, actor);
  u32 a = gabi::ea(actor), beam = call<u32>(0x02325D3C, actor);
  if (beam) {
    u32 e = load<u32>(beam + 0x8e0);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, -1);
      store<u32>(e + 0x254, flags | 1);
      store<u32>(beam + 0x8e0, 0);
    }
    if (load<s16>(beam + 0x844) == 1) {
      f32 width = load<f32>(beam + 0x718);
      if (width < 5.f)
        store<f32>(beam + 0x718, width + 1.f);
      f32 height = load<f32>(beam + 0x798);
      if (height < 4.f)
        store<f32>(beam + 0x798, height + 1.f);
      else {
        store<s16>(beam + 0x844, 0);
        store<f32>(beam + 0x798, 0);
        store<f32>(beam + 0x718, 0);
      }
    } else {
      store<f32>(beam + 0x798, 0);
      store<f32>(beam + 0x718, 0);
    }
  }
  call(0x0200F428, at<void>(a + 0x3b4), 0, 2, 0x800);
  call(0x0200F5C8, at<void>(a + 0x538), 1.f, 0.2f);
  call(0x0200F5C8, at<void>(a + 0x53c), 1.f, 0.05f);
  s16 timer = s16(load<s16>(a + 0x884) + 1);
  store<s16>(a + 0x884, timer);
  if (timer <= 0)
    return 1;
  for (u32 off = 0x4d4; off <= 0x4e4; off += 4) {
    u32 e = load<u32>(a + off);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<u32>(e + 0x5c, -1);
      store<u32>(e + 0x254, flags | 1);
    }
  }
  store<u32>(a + 0x4e0, 0);
  store<u32>(a + 0x4d4, 0);
  store<u32>(a + 0x4d8, 0);
  store<u32>(a + 0x4dc, 0);
  store<u32>(a + 0x4e4, 0);
  store<u32>(a + 0x538, load<u32>(0x101ffba8));
  u32 second = load<u32>(0x101ffbac);
  gabi::Local<csXyz> angle;
  s16 rz = load<s16>(a + 0x3b8), rx = load<s16>(a + 0x3b4),
      yaw = load<s16>(a + 0x322), ry = load<s16>(a + 0x3b6);
  angle->x = rx;
  angle->z = rz;
  u8 type = load<u8>(a + 0x890);
  store<u32>(a + 0x53c, second);
  store<u32>(a + 0x540, load<u32>(0x101ffbb0));
  angle->y = s16(ry + yaw);
  u32 guard = type ? 0x101fdb80 : 0x101fdb78,
      colors = type ? 0x101fec0c : 0x101fec04,
      rom = type ? 0x10026248 : 0x10026240;
  gabi::Local<be<u32>> color0, color1;
  if (!load<u32>(guard)) {
    store<u32>(guard, 1);
    call(0xC000A848, at<void>(colors), at<void>(rom), 4);
  }
  *color0 = load<u32>(colors);
  if (!load<u32>(guard + 4)) {
    store<u32>(guard + 4, 1);
    call(0xC000A848, at<void>(colors + 4), at<void>(rom + 4), 4);
  }
  *color1 = load<u32>(colors + 4);
  s32 room = load<s8>(a + 0x326);
  u32 play = call<u32>(0x025200D4);
  call(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x815f,
       at<void>(a + 0x3bc), angle.get(), 0, 255, 0, room, color0.get(), 0, 0);
  room = load<s8>(a + 0x326);
  play = call<u32>(0x025200D4);
  call(0x025A847C, at<void>(load<u32>(play + 0x5ab0)), 0, 0x8160,
       at<void>(a + 0x3bc), angle.get(), 0, 255, 0, room, color1.get(), 0, 0);
  if (beam)
    call(0x025D57E0, at<void>(beam));
  s32 reverb = call<s32>(0x02520540, s32(load<s8>(a + 0x326)));
  call(0x025E1A40, 0x6807, at<void>(a + 0x37c), 0, reverb);
  u32 item = load<u8>(a + 0x897);
  f32 x = load<f32>(a + 0x3bc), y = load<f32>(a + 0x3c0),
      z = load<f32>(a + 0x3c4);
  u16 ang = angle->y;
  u32 trig = 0x104a44f8 + ((ang >> 3) * 8);
  f32 sine = load<f32>(trig), cosine = load<f32>(trig + 4);
  gabi::Local<cXyz> pos;
  pos->x = gabi::fmadds(30.f, sine, x);
  pos->y = y;
  pos->z = gabi::fmadds(30.f, cosine, z);
  call(0x025D8120, pos.get(), item, u32(load<u8>(a + 0x898)),
       s32(load<s8>(a + 0x326)), 0, angle.get(), 1, 0);
  store<s16>(a + 0x3ac, 0);
  store<u32>(a + 0x3b0, 0x023286f8);
  store<s16>(a + 0x3ae, -1);
  return 1;
}
VERIFY(0x02328120, bemos_break);

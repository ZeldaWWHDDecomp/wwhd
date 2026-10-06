// Forsaken Fortress wooden barricade, WWHD layout and behavior.
//
#include "bindings.h"
namespace {
using gabi::at;
using gabi::call;
using gabi::load;
using gabi::store;
u32 model(u32 a) { return load<u32>(a + 0x3ec); }
u32 play() { return call<u32>(0x025200D4); }
void trans(u32 a) {
  f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318),
      z = load<f32>(a + 0x31c);
  call(0x028E93CC, at<void>(0x1048d0cc), x, y, z);
}
void matrix(u32 dst) {
  f32 m[12];
  for (int i = 0; i < 12; i++)
    m[i] = load<f32>(0x1048d0cc + i * 4);
  for (int i = 0; i < 12; i++)
    store<f32>(dst + i * 4, m[i]);
}
struct ResourceName {
  be<u32> text, vt;
};
} // namespace
s32 mjdoor_heap(void *actor) {
  WWHD_FUNC(0x0236F264, s32, actor);
  u32 a = gabi::ea(actor);
  gabi::Local<ResourceName> name;
  name->text = 0x1002c420;
  name->vt = 0x1002c364;
  u32 data =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name.get(), 4);
  if (!data)
    call(0x0273AA24, at<char>(0x1002c3ac), 0xc7, at<char>(0x1002c3c4));
  u32 m = call<u32>(0x025E38E0, at<void>(data), 0x80000, 0x11000022);
  store<u32>(a + 0x3ec, m);
  if (!m)
    return 0;
  trans(a);
  call(0x025F1C28, at<void>(0x1048d0cc), load<s16>(a + 0x322));
  call(0x028E90D4, at<void>(0x1048d0cc), at<void>(a + 0x3fc));
  u32 bg = call<u32>(0x024F23F4, at<void>(0));
  store<u32>(a + 0x3f8, bg);
  if (!bg)
    return 0;
  gabi::Local<ResourceName> mesh;
  mesh->text = 0x1002c420;
  mesh->vt = 0x1002c364;
  u32 data2 =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), mesh.get(), 7);
  return call<s32>(0x0200A030, at<void>(load<u32>(a + 0x3f8)), at<void>(data2),
                   1, at<void>(a + 0x3fc)) == 0;
}
VERIFY(0x0236F264, mjdoor_heap);
s32 mjdoor_heapCallback(void *actor) {
  WWHD_FUNC(0x0236F38C, s32, actor);
  return mjdoor_heap(actor);
}
VERIFY(0x0236F38C, mjdoor_heapCallback);
void mjdoor_hit(void *actor, void *target, void *attacker, void *attack) {
  WWHD_FUNC(0x0236F390, void, actor, target, attacker, attack);
  u32 a = gabi::ea(actor), enemy = gabi::ea(attacker);
  if (enemy && load<s16>(enemy + 8) == 0x126)
    store<u8>(a + 0x3a1, (u8)(load<u8>(a + 0x3a1) - 1));
}
VERIFY(0x0236F390, mjdoor_hit);
void mjdoor_getArg(void *actor) {
  WWHD_FUNC(0x0236F3BC, void, actor);
  u32 a = gabi::ea(actor);
  store<u8>(a + 0x3e8, (u8)load<u32>(a + 0xb0));
}
VERIFY(0x0236F3BC, mjdoor_getArg);
void mjdoor_setMtx(void *actor) {
  WWHD_FUNC(0x0236F3C8, void, actor);
  u32 a = gabi::ea(actor);
  f32 x = load<f32>(a + 0x330), y = load<f32>(a + 0x334),
      z = load<f32>(a + 0x338);
  u32 m = model(a);
  store<f32>(m + 0xbc, x);
  store<f32>(m + 0xc0, y);
  store<f32>(m + 0xc4, z);
  trans(a);
  call(0x025F1C28, at<void>(0x1048d0cc), load<s16>(a + 0x322));
  matrix(model(a) + 0xc8);
  if (load<s32>(a + 0x3ac) != 1) {
    call(0x028E90D4, at<void>(0x1048d0cc), at<void>(a + 0x3fc));
    call(0x024F43DC, at<void>(load<u32>(a + 0x3f8)));
  }
}
VERIFY(0x0236F3C8, mjdoor_setMtx);
void mjdoor_waitInit(void *actor) {
  WWHD_FUNC(0x0236F4C0, void, actor);
  store<s32>(gabi::ea(actor) + 0x3ac, 0);
}
VERIFY(0x0236F4C0, mjdoor_waitInit);
void mjdoor_init(void *actor) {
  WWHD_FUNC(0x0236F4CC, void, actor);
  u32 a = gabi::ea(actor);
  call(0x02515F14, at<void>(a + 0x430), 0xff, 0, actor);
  for (int i = 0; i < 10; i++) {
    u32 cyl = a + 0x46c + i * 0x130;
    call(0x02516518, at<void>(cyl), at<void>(0x101caae0));
    store<u32>(cyl + 0x44, a + 0x430);
    store<u32>(cyl + 0x9c, 0x0236f390);
  }
  call(0x020184DC, at<void>(a + 0x6b4), 95.f);
  u32 p = play();
  call(0x024EEA6C, at<void>(p + 0x12a0), at<void>(load<u32>(a + 0x3f8)), actor);
  mjdoor_setMtx(actor);
  mjdoor_waitInit(actor);
  store<u8>(a + 0x3a1, 30);
  store<u8>(a + 0x3a0, 30);
}
VERIFY(0x0236F4CC, mjdoor_init);
s32 mjdoor_create(void *actor) {
  WWHD_FUNC(0x0236F580, s32, actor);
  u32 a = gabi::ea(actor), flags = load<u32>(a + 0x2e4);
  if (!(flags & 8)) {
    if (a) {
      call(0x025D4ED0, actor);
      store<u32>(a + 0xb4, 0x1002c38c);
      call(0x025A5CEC, at<void>(a + 0x3c4), at<void>(0x101cab48),
           at<void>(a + 0x110), 0);
      call(0x0200BD2C, at<void>(a + 0x430));
      call(0x02515DA0, at<void>(a + 0x44c));
      store<u32>(a + 0x448, 0x1004ae88);
      store<u32>(a + 0x44c, 0x1004aec0);
      call(0x028EFFD0, at<void>(a + 0x46c), 10, 0x130, at<void>(0x0236feb4));
      flags = load<u32>(a + 0x2e4);
    }
    store<u32>(a + 0x2e4, flags | 8);
  }
  s32 phase = call<s32>(0x02520460, at<void>(a + 0x3f0), at<char>(0x1002c420));
  if (phase == 4) {
    mjdoor_getArg(actor);
    u8 sw = load<u8>(a + 0x3e8);
    if (sw != 0xff) {
      s8 room = load<s8>(a + 0x326);
      u32 save = load<u32>(0x101f84dc) + 0x20;
      if (call<s32>(0x025BA0C0, at<void>(save), sw, room))
        return 5;
    }
    if (!call<s32>(0x025D63E8, actor, at<void>(0x0236f38c), 0x820))
      return 5;
    mjdoor_init(actor);
  }
  return phase;
}
VERIFY(0x0236F580, mjdoor_create);
s32 mjdoor_createCallback(void *actor) {
  WWHD_FUNC(0x0236F6D0, s32, actor);
  return mjdoor_create(actor);
}
VERIFY(0x0236F6D0, mjdoor_createCallback);
s32 mjdoor_delete(void *actor) {
  WWHD_FUNC(0x0236F6D4, s32, actor);
  u32 a = gabi::ea(actor);
  call(0x025204C8, at<void>(a + 0x3f0), at<char>(0x1002c420));
  u32 bg = load<u32>(a + 0x3f8);
  if (bg && load<u32>(bg) < 0x100) {
    u32 p = play();
    call(0x020087EC, at<void>(p + 0x12a0), at<void>(load<u32>(a + 0x3f8)));
  }
  u32 vt = load<u32>(a + 0x3c4), fn = load<u32>(vt + 0x44);
  call(fn, at<void>(a + 0x3c4));
  return 1;
}
VERIFY(0x0236F6D4, mjdoor_delete);
s32 mjdoor_deleteCallback(void *actor) {
  WWHD_FUNC(0x0236F74C, s32, actor);
  return mjdoor_delete(actor);
}
VERIFY(0x0236F74C, mjdoor_deleteCallback);
s32 mjdoor_execute(void *actor) {
  WWHD_FUNC(0x0236F750, s32, actor);
  u32 a = gabi::ea(actor);
  if (!load<u32>(0x101fdc10)) {
    store<u32>(0x101fdc10, 1);
    call(0xC000A848, at<void>(0x101fdc14), at<void>(0x1002c354), 16);
  }
  u32 entry = 0x101fdc14 + (load<u32>(a + 0x3ac) << 3);
  s16 vi = load<s16>(entry + 2), delta = load<s16>(entry);
  u32 obj = a + (u32)(s32)delta;
  u32 fn;
  if (vi < 0)
    fn = load<u32>(entry + 4);
  else {
    u32 slot = obj + (u32)(s32)load<s16>(entry + 6);
    u32 vt = load<u32>(slot);
    fn = load<u32>(vt + ((u32)(s32)vi << 3) + 4);
  }
  call(fn, at<void>(obj));
  call(0x025D6870, actor, 0);
  mjdoor_setMtx(actor);
  return 0;
}
VERIFY(0x0236F750, mjdoor_execute);
s32 mjdoor_executeCallback(void *actor) {
  WWHD_FUNC(0x0236F830, s32, actor);
  return mjdoor_execute(actor);
}
VERIFY(0x0236F830, mjdoor_executeCallback);
s32 mjdoor_draw(void *actor) {
  WWHD_FUNC(0x0236F834, s32, actor);
  u32 a = gabi::ea(actor);
  if (load<s32>(a + 0x3ac) == 1)
    return 1;
  u32 env = call<u32>(0x02555D0C);
  call(0x025626A4, at<void>(env), 0, at<void>(a + 0x314), at<void>(a + 0x110));
  env = call<u32>(0x02555D0C);
  call(0x02562F5C, at<void>(env), at<void>(model(a)), at<void>(a + 0x110));
  call(0x025E2DE0, at<void>(model(a)), 0);
  return 1;
}
VERIFY(0x0236F834, mjdoor_draw);
s32 mjdoor_drawCallback(void *actor) {
  WWHD_FUNC(0x0236F89C, s32, actor);
  return mjdoor_draw(actor);
}
VERIFY(0x0236F89C, mjdoor_drawCallback);
void mjdoor_collision(void *actor) {
  WWHD_FUNC(0x0236F8A0, void, actor);
  u32 a = gabi::ea(actor);
  f32 offsets[10];
  for (int i = 0; i < 10; i++)
    offsets[i] = load<f32>(0x1002c3e4 + i * 4);
  u16 angle = load<u16>(a + 0x322);
  u32 trig = 0x104a44f8 + ((u32)angle >> 3) * 8;
  f32 sine = -load<f32>(trig), cosine = load<f32>(trig + 4);
  gabi::Local<cXyz> center;
  for (int i = 0; i < 10; i++) {
    f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318), offset = offsets[i],
        reg = load<f32>(0x1047bcd0), z = load<f32>(a + 0x31c);
    center->x = gabi::fmadds(offset, cosine, x);
    center->y = y - reg;
    center->z = gabi::fmadds(offset, sine, z);
    u32 cyl = a + 0x46c + i * 0x130;
    call(0x020182E0, at<void>(cyl + 0x118), center.get());
    call(0x020184DC, at<void>(cyl + 0x118), 100.f);
    call(0x02018428, at<void>(cyl + 0x118), 1350.f);
    u32 p = play();
    call(0x0200E240, at<void>(p + 0x26a4), at<void>(cyl));
  }
}
VERIFY(0x0236F8A0, mjdoor_collision);
void mjdoor_smoke(void *actor) {
  WWHD_FUNC(0x0236F9F4, void, actor);
  u32 a = gabi::ea(actor);
  if (!load<u32>(0x1046a52c)) {
    store<u32>(0x1046a52c, 1);
    store<f32>(0x1046a530, 1.25f);
    store<f32>(0x1046a538, 1.25f);
    store<f32>(0x1046a534, 1.25f);
  }
  s8 room = load<s8>(a + 0x326);
  u32 p = play();
  call(0x025A847C, at<void>(load<u32>(p + 0x5ab0)), 2, 0xa11e,
       at<void>(a + 0x3b0), at<void>(a + 0x3bc), 0, 0xb9, at<void>(a + 0x3c4),
       room, 0, 0, 0);
  u32 emitter = load<u32>(a + 0x3c8);
  if (emitter)
    store<u8>(emitter + 0x247, 200);
}
VERIFY(0x0236F9F4, mjdoor_smoke);
void mjdoor_deleteInit(void *actor) {
  WWHD_FUNC(0x0236FAAC, void, actor);
  u32 a = gabi::ea(actor);
  u8 sw = load<u8>(a + 0x3e8);
  if (sw != 0xff) {
    s8 room = load<s8>(a + 0x326);
    call(0x025B9E38, at<void>(load<u32>(0x101f84dc) + 0x20), sw, room);
  }
  s8 room = load<s8>(a + 0x326);
  store<s32>(a + 0x3ac, 1);
  s32 reverb = call<s32>(0x02520540, room);
  call(0x025E1A40, 0x6978, at<void>(a + 0x37c), 0, reverb);
  gabi::Local<cXyz> scale;
  scale->x = 1.f;
  scale->y = 1.f;
  scale->z = 1.f;
  for (int i = 0; i < 2; i++) {
    u32 p = play();
    call(0x025A847C, at<void>(load<u32>(p + 0x5ab0)), 0, 0x811a + i,
         at<void>(a + 0x314), at<void>(a + 0x320), scale.get(), 0xff, 0, -1,
         at<void>(a + 0x1a8), at<void>(a + 0x1a8), 0);
  }
  gabi::Local<cXyz> pos;
  u32 y = load<u32>(a + 0x318), z = load<u32>(a + 0x31c),
      x = load<u32>(a + 0x314);
  store<u32>(pos.a + 8, z);
  store<u32>(pos.a, x);
  store<u32>(pos.a + 4, y);
  f32 water = call<f32>(0x024F17D4, pos.get());
  pos->y = water;
  for (int i = 0; i < 2; i++) {
    u32 p = play();
    call(0x025A847C, at<void>(load<u32>(p + 0x5ab0)), 0, 0x811c + i, pos.get(),
         at<void>(a + 0x320), scale.get(), 0xff, 0, -1, 0, 0, 0);
  }
  y = load<u32>(a + 0x318);
  u16 ry = load<u16>(a + 0x322);
  store<u32>(a + 0x3b4, y);
  store<u16>(a + 0x3be, ry);
  z = load<u32>(a + 0x31c);
  x = load<u32>(a + 0x314);
  store<u32>(a + 0x3b8, z);
  store<u32>(a + 0x3b0, x);
  u16 rx = load<u16>(a + 0x320), rz = load<u16>(a + 0x324);
  store<u16>(a + 0x3bc, rx);
  store<u16>(a + 0x3c0, rz);
  mjdoor_smoke(actor);
  store<s32>(a + 0x3e4, 50);
  store<s32>(a + 0x42c, 600);
  u32 p = play();
  call(0x020087EC, at<void>(p + 0x12a0), at<void>(load<u32>(a + 0x3f8)));
  for (int i = 0; i < 10; i++) {
    u32 c = a + 0x46c + i * 0x130;
    u32 co = load<u32>(c + 0x2c), tg = load<u32>(c + 0x18);
    store<u32>(c + 0x2c, co & 0xffffff86);
    store<u32>(c + 0x18, tg & 0xfffffff6);
  }
}
VERIFY(0x0236FAAC, mjdoor_deleteInit);
void mjdoor_wait(void *actor) {
  WWHD_FUNC(0x0236FCD8, void, actor);
  u32 a = gabi::ea(actor);
  mjdoor_collision(actor);
  if (load<s8>(a + 0x3a1) < 0 || load<u8>(0x1046a50c))
    mjdoor_deleteInit(actor);
}
VERIFY(0x0236FCD8, mjdoor_wait);
void mjdoor_deleteMode(void *actor) {
  WWHD_FUNC(0x0236FD28, void, actor);
  u32 a = gabi::ea(actor);
  if (load<u32>(a + 0x3c8)) {
    if (call<s32>(0x0211D2F8, at<void>(a + 0x3e4))) {
      s32 timer = load<s32>(a + 0x3e4);
      if (timer <= 40)
        store<u8>(load<u32>(a + 0x3c8) + 0x247, (u8)((u32)timer * 5));
    } else
      call(0x025A5F88, at<void>(a + 0x3c4));
  }
  if (!call<s32>(0x0211D2F8, at<void>(a + 0x42c)))
    call(0x025D57E0, actor);
}
VERIFY(0x0236FD28, mjdoor_deleteMode);
void *mjdoor_hioCtor(void *obj) {
  WWHD_FUNC(0x0236FDB8, void *, obj);
  u32 a = gabi::ea(obj);
  if (!a) {
    a = call<u32>(0x0273AD10, 8);
    if (!a)
      return nullptr;
  }
  store<u8>(a + 4, 0);
  store<u32>(a, 0x1002c39c);
  return at<void>(a);
}
VERIFY(0x0236FDB8, mjdoor_hioCtor);
void mjdoor_sinit() {
  WWHD_FUNC(0x0236FE00, void);
  store<u32>(0x1046a524, 0);
  store<u32>(0x1046a51c, 0);
  store<u32>(0x1046a528, 0);
  store<u32>(0x1046a520, 0);
  call(0x028F026C, at<void>(0x101cab24));
  store<f32>(0x1046a510, load<f32>(0x1002c418));
  store<f32>(0x1046a514, load<f32>(0x1002c41c));
  call(0x028ED6F8, at<void>(0x1046a518));
  call(0x028F026C, at<void>(0x101cab30));
  call(0x028EAB2C, at<void>(0x1046a519));
  call(0x028F026C, at<void>(0x101cab3c));
  mjdoor_hioCtor(at<void>(0x1046a508));
}
VERIFY(0x0236FE00, mjdoor_sinit);
void mjdoor_trivialDtor(void *obj, u32 flags) {
  WWHD_FUNC(0x0236FEA0, void, obj, flags);
  if (obj && (flags & 1))
    call(0x0273AF40, obj);
}
VERIFY(0x0236FEA0, mjdoor_trivialDtor);
void *mjdoor_cylCtor(void *obj) {
  WWHD_FUNC(0x0236FEB4, void *, obj);
  u32 a = gabi::ea(obj);
  if (!a) {
    a = call<u32>(0x0273AD10, 0x130);
    if (!a)
      return nullptr;
  }
  call(0x02515FB8, at<void>(a));
  store<u32>(a + 0x114, 0x100015a8);
  store<u32>(a + 0x110, 0x1002c37c);
  call(0x02018590, at<void>(a + 0x118));
  store<u32>(a + 0x3c, 0x1004b108);
  store<u32>(a + 0x12c, 0x1004b150);
  store<u32>(a + 0x114, 0x1004b160);
  return at<void>(a);
}
VERIFY(0x0236FEB4, mjdoor_cylCtor);
s32 mjdoor_isDelete(void *actor) {
  WWHD_FUNC(0x0236FF40, s32, actor);
  return 1;
}
VERIFY(0x0236FF40, mjdoor_isDelete);
void mjdoor_dtor(void *obj, u32 flags) {
  WWHD_FUNC(0x0236FF48, void, obj, flags);
  u32 a = gabi::ea(obj);
  if (a) {
    call(0x028F0164, at<void>(a + 0x46c), 10, 0x130, at<void>(0x02515a70), 0,
         0);
    call(0x02515860, at<void>(a + 0x430), 2);
    call(0x025D50BC, obj, 0);
    if (flags & 1)
      call(0x0273AF40, obj);
  }
}
VERIFY(0x0236FF48, mjdoor_dtor);
void mjdoor_empty(void *actor) { WWHD_FUNC(0x0236FFC8, void, actor); }
VERIFY(0x0236FFC8, mjdoor_empty);

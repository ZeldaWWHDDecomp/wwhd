// Floating barrel spawner, actual WWHD translation unit.
#include "bindings.h"
using gabi::at;
using gabi::call;
using gabi::load;
using gabi::store;

void coming_initTimer(void *ctrl, void *actor) {
  WWHD_FUNC(0x0233194C, void, ctrl, actor);
  u32 c = gabi::ea(ctrl);
  u32 n = call<u32>(0x02332D84, actor, 2, 24);
  if (n >= 4)
    call(0x0273AA24, at<void>(0x10026e00), 0x104, at<void>(0x10026e14));
  u32 t = 0x10026dc8 + n * 4;
  s32 total = (s32)load<s16>(t) + (s32)load<s16>(t + 2);
  s32 v = gabi::ftoi(call<f32>(0x020198D8, (f32)total));
  store<s32>(c + 4, v);
}
VERIFY(0x0233194C, coming_initTimer);

void coming_init(void *ctrl, void *actor) {
  WWHD_FUNC(0x02331A08, void, ctrl, actor);
  u32 c = gabi::ea(ctrl);
  call(0x0233194C, ctrl, actor);
  store<u32>(c, 0);
  store<u32>(c + 8, 0xffffffff);
}
VERIFY(0x02331A08, coming_init);

void coming_waitTimer(void *ctrl, void *actor) {
  WWHD_FUNC(0x02331A44, void, ctrl, actor);
  u32 c = gabi::ea(ctrl);
  u32 n = call<u32>(0x02332D84, actor, 2, 24);
  if (n >= 4)
    call(0x0273AA24, at<void>(0x10026e34), 0x111, at<void>(0x10026e48));
  u32 t = 0x10026dc8 + n * 4;
  f32 range = (f32)load<s16>(t + 2);
  s32 base = load<s16>(t);
  s32 v = gabi::ftoi(call<f32>(0x020198D8, range));
  store<u32>(c + 4, (u32)base + (u32)v);
}
VERIFY(0x02331A44, coming_waitTimer);

void coming_retryTimer(void *ctrl, void *actor) {
  WWHD_FUNC(0x02331B00, void, ctrl, actor);
  u32 c = gabi::ea(ctrl);
  s32 n = gabi::ftoi(call<f32>(0x020198D8, 30.f));
  store<u32>(c + 4, (u32)n + 30);
}
VERIFY(0x02331B00, coming_retryTimer);

void coming_appearTimer(void *ctrl, void *actor) {
  WWHD_FUNC(0x02331B4C, void, ctrl, actor);
  u32 c = gabi::ea(ctrl);
  s32 n = gabi::ftoi(call<f32>(0x020198D8, 30.f));
  store<u32>(c + 4, (u32)n + 300);
}
VERIFY(0x02331B4C, coming_appearTimer);

s32 coming_create(void *actor) {
  WWHD_FUNC(0x02331B98, s32, actor);
  u32 a = gabi::ea(actor);
  u32 f = load<u32>(a + 0x2e4);
  if (!(f & 8)) {
    if (a) {
      call(0x025D4ED0, actor);
      f = load<u32>(a + 0x2e4);
      store<u32>(a + 0xb4, 0x10026db0);
    }
    store<u32>(a + 0x2e4, f | 8);
  }
  u32 type = call<u32>(0x02332D84, actor, 4, 0);
  store<u32>(a + 0x3ac, type);
  call(0x025DA884, at<void>(a + 0xdc));
  for (int i = 0; i < 5; i++)
    call(0x02331A08, at<void>(a + 0x3b0 + i * 12), actor);
  f32 y = load<f32>(a + 0x318), x = load<f32>(a + 0x314),
      z = load<f32>(a + 0x31c);
  y = y + 500.f;
  store<f32>(0x10469540, x);
  store<f32>(0x10469548, z);
  store<f32>(0x10469544, y);
  u32 play = call<u32>(0x025200D4);
  call<f32>(0x02008974, at<void>(play + 0x12a0), at<void>(0x1046951c));
  play = call<u32>(0x025200D4);
  u32 room =
      call<u32>(0x024EF130, at<void>(play + 0x12a0), at<void>(0x10469530));
  store<u8>(a + 0x3ec, room);
  return 4;
}
VERIFY(0x02331B98, coming_create);

s32 coming_execute(void *actor) {
  WWHD_FUNC(0x02331C94, s32, actor);
  u32 a = gabi::ea(actor);
  u32 n = call<u32>(0x02332D84, actor, 2, 28);
  if (n >= 4) {
    call(0x0273AA24, at<void>(0x10026e88), 0x497, at<void>(0x10026e9c));
    if (n >= 4)
      return 1;
  }
  s32 count = load<s16>(0x10026dc0 + n * 2);
  bool done = true;
  if (count > 0) {
    u32 ctrl = a + 0x3b0;
    do {
      s32 timer = load<s32>(ctrl + 4);
      if (timer > 0)
        store<u32>(ctrl + 4, (u32)timer - 1);
      u32 type = load<u32>(a + 0x3ac), mode = load<u32>(ctrl);
      u32 table = 0x10026e70 + (type * 3 + mode) * 8;
      s16 index = load<s16>(table + 2), adj = load<s16>(table);
      u32 obj = a + (s32)adj, target;
      if (index < 0)
        target = load<u32>(table + 4);
      else {
        s16 vo = load<s16>(table + 6);
        u32 vt = load<u32>(obj + (s32)vo);
        target = load<u32>(vt + ((u32)(s32)index) * 8 + 4);
      }
      call(target, at<void>(obj), at<void>(ctrl));
      if (load<u32>(ctrl) != 2 || load<u32>(ctrl + 8) != 0xffffffff)
        done = false;
      ctrl += 12;
    } while (--count);
  }
  if (done)
    call(0x025D57E0, actor);
  return 1;
}
VERIFY(0x02331C94, coming_execute);

u32 coming_switch(void *actor) {
  WWHD_FUNC(0x02331DD8, u32, actor);
  u32 a = gabi::ea(actor);
  u32 sw = call<u32>(0x02332D84, actor, 8, 16);
  if (sw == 255)
    return 1;
  s8 room = load<s8>(a + 0x2fe);
  return call<u32>(0x025BA0C0, at<void>(load<u32>(0x101f84dc) + 0x20), sw,
                   room) != 0;
}
VERIFY(0x02331DD8, coming_switch);

f32 coming_water(void *pos) {
  WWHD_FUNC(0x02331E50, f32, pos);
  u32 a = gabi::ea(pos);
  f32 z = load<f32>(a + 8), x = load<f32>(a);
  f32 water = -1000000000.f;
  if (call<u32>(0x0246B6A4, x, z)) {
    x = load<f32>(a);
    z = load<f32>(a + 8);
    water = call<f32>(0x0246BA0C, x, z);
  } else {
    if (!load<u32>(0x10469518)) {
      store<u32>(0x10469518, 1);
      call(0x024F22DC, at<void>(0x104694c8));
      call(0x028F026C, at<void>(0x101c89b4));
    }
    x = load<f32>(a);
    f32 y = load<f32>(a + 4);
    z = load<f32>(a + 8);
    f32 lo = y - 1000.f;
    store<f32>(0x10469500, x);
    y = y + 1000.f;
    store<f32>(0x10469508, z);
    store<f32>(0x10469504, lo);
    store<f32>(0x1046950c, y);
    u32 play = call<u32>(0x025200D4);
    if (call<u32>(0x024EF7C0, at<void>(play + 0x12a0), at<void>(0x104694c8)))
      water = load<f32>(0x10469510);
  }
  return water;
}
VERIFY(0x02331E50, coming_water);

u32 coming_speed(void *actor, void *ship) {
  WWHD_FUNC(0x02331F48, u32, actor, ship);
  return load<f32>(gabi::ea(ship) + 0x370) > 25.f;
}
VERIFY(0x02331F48, coming_speed);

u32 coming_range(void *actor, void *ship) {
  WWHD_FUNC(0x02331F64, u32, actor, ship);
  u32 a = gabi::ea(actor), s = gabi::ea(ship);
  if (call<u32>(0x02332D84, actor, 1, 6))
    return 1;
  gabi::Local<cXyz> sp, ap;
  f32 sz = load<f32>(s + 0x31c), az = load<f32>(a + 0x31c),
      ax = load<f32>(a + 0x314);
  sp->z = sz;
  ap->z = az;
  ap->y = 0;
  ap->x = ax;
  f32 sx = load<f32>(s + 0x314);
  sp->y = 0;
  sp->x = sx;
  f32 dist = call<f32>(0x028E8DE8, sp.get(), ap.get());
  dist = call<f32>(0x028F4384, dist);
  f32 limit = load<f32>(a + 0x330) * 10000.f;
  return dist < limit;
}
VERIFY(0x02331F64, coming_range);

void coming_makePos(void *actor, void *ship, void *out) {
  WWHD_FUNC(0x02332024, void, actor, ship, out);
  u32 s = gabi::ea(ship), o = gabi::ea(out);
  f32 x = load<f32>(s + 0x314), y = load<f32>(s + 0x318),
      z = load<f32>(s + 0x31c);
  call(0x028E93CC, at<void>(0x1048d0cc), x, y, z);
  call(0x025F1C28, at<void>(0x1048d0cc), load<s16>(s + 0x32a));
  f32 forward = call<f32>(0x020198D8, 6000.f) + 5000.f;
  f32 lateral = (call<f32>(0x02019788) - 0.5f) * 5000.f;
  call(0x025F24E0, lateral, 0.f, forward);
  gabi::Local<cXyz> pos;
  call(0x028E8F64, at<void>(0x1048d0cc), at<void>(0x101ffba8), pos.get());
  x = pos->x;
  z = pos->z;
  store<f32>(o, x);
  y = call<f32>(0x0246BA0C, x, z);
  z = pos->z;
  store<f32>(o + 4, y);
  store<f32>(o + 8, z);
}
VERIFY(0x02332024, coming_makePos);

u32 coming_ground(void *actor, void *tmp) {
  WWHD_FUNC(0x0233211C, u32, actor, tmp);
  u32 t = gabi::ea(tmp);
  f32 end = load<f32>(t + 0x18) + 100.f, distance = 0;
  s16 angle = 0;
  if (!(distance < end))
    return 1;
  do {
    f32 x = load<f32>(t + 12), y = load<f32>(t + 16), z = load<f32>(t + 20);
    call(0x028E93CC, at<void>(0x1048d0cc), x, y, z);
    call(0x025F1C28, at<void>(0x1048d0cc), angle);
    call(0x025F24E0, 0.f, 0.f, distance);
    gabi::Local<cXyz> pos;
    call(0x028E8F64, at<void>(0x1048d0cc), at<void>(0x101ffba8), pos.get());
    f32 water = call<f32>(0x02331E50, pos.get());
    if (!(water > -1000000000.f))
      return 0;
    y = pos->y;
    z = pos->z;
    x = pos->x;
    y = y + 20000.f;
    store<f32>(0x10469548, z);
    store<f32>(0x10469540, x);
    store<f32>(0x10469544, y);
    pos->y = y;
    u32 play = call<u32>(0x025200D4);
    f32 floor =
        call<f32>(0x02008974, at<void>(play + 0x12a0), at<void>(0x1046951c));
    if (floor > water - 100.f)
      return 0;
    distance = distance + 20.f;
    angle = (s16)(angle + 20000);
  } while (distance < end);
  return 1;
}
VERIFY(0x0233211C, coming_ground);

u32 coming_wall(void *actor, void *tmp) {
  WWHD_FUNC(0x023322EC, u32, actor, tmp);
  u32 t = gabi::ea(tmp);
  call(0x024F1AFC, at<void>(0x10469570), tmp, at<void>(t + 12), 0);
  u32 p = call<u32>(0x025200D4);
  return (
      u8)(call<u32>(0x02008860, at<void>(p + 0x12a0), at<void>(0x10469570)) ^
          1);
}
VERIFY(0x023322EC, coming_wall);

void *coming_actorCheck(void *actor, void *tmp) {
  WWHD_FUNC(0x0233233C, void *, actor, tmp);
  u32 a = gabi::ea(actor), t = gabi::ea(tmp);
  if (!call<u32>(0x025D4604, actor))
    return nullptr;
  if (a && load<s16>(a + 8) == 0xa5)
    return nullptr;
  if (a && load<s16>(a + 8) == 0xa8)
    return nullptr;
  f32 y = load<f32>(a + 0x318), ty = load<f32>(t + 16),
      height = load<f32>(t + 28);
  f32 diff = __builtin_fabsf(y - ty);
  height = height + 100.f;
  if (!(diff < height))
    return nullptr;
  gabi::Local<cXyz> ap, tp;
  ap->x = load<f32>(a + 0x314);
  ap->z = load<f32>(a + 0x31c);
  ap->y = 0;
  tp->z = load<f32>(t + 20);
  tp->x = load<f32>(t + 12);
  tp->y = 0;
  f32 d = call<f32>(0x028E8DE8, ap.get(), tp.get());
  d = call<f32>(0x028F4384, d);
  f32 radius = load<f32>(t + 24) + 100.f;
  if (!(d < radius))
    return nullptr;
  return actor;
}
VERIFY(0x0233233C, coming_actorCheck);

u32 coming_checkPos(void *actor, void *tmp) {
  WWHD_FUNC(0x02332468, u32, actor, tmp);
  if (!call<u32>(0x0233211C, actor, tmp))
    return 0;
  if (!call<u32>(0x023322EC, actor, tmp))
    return 0;
  return call<u32>(0x025D5218, at<void>(0x0233233c), tmp) == 0;
}
VERIFY(0x02332468, coming_checkPos);

u32 coming_pattern(void *actor) {
  WWHD_FUNC(0x023324E8, u32, actor);
  f32 r = call<f32>(0x02019788);
  if (r < 0.05f)
    return 1;
  if (r < 0.1f)
    return 0;
  s32 pick = gabi::ftoi(call<f32>(0x020198D8, 16.f));
  s32 table = call<s32>(0x02332D84, actor, 3, 8);
  if (table >= 7)
    table = 0;
  return load<u8>(0x10026fe8 + (u32)table * 16 + (u32)pick);
}
VERIFY(0x023324E8, coming_pattern);

u32 coming_type(u32 pattern) {
  WWHD_FUNC(0x02332590, u32, pattern);
  u32 index = load<u8>(0x10026f04 + pattern);
  return load<u32>(0x10026ef8 + index * 4);
}
VERIFY(0x02332590, coming_type);

u32 coming_item(u32 pattern) {
  WWHD_FUNC(0x023325A8, u32, pattern);
  return load<u8>(0x10026f1c + pattern) & 63;
}
VERIFY(0x023325A8, coming_item);

u32 coming_buoy(u32 pattern) {
  WWHD_FUNC(0x023325B8, u32, pattern);
  return load<u8>(0x10026f34 + pattern);
}
VERIFY(0x023325B8, coming_buoy);

u32 coming_param(void *actor, void *out) {
  WWHD_FUNC(0x023325C4, u32, actor, out);
  u32 o = gabi::ea(out);
  u32 p = call<u32>(0x025200D4), s = load<u32>(p + 0x5b3c);
  if (!s)
    return 0;
  if (!call<u32>(0x02331F48, actor, at<void>(s)))
    return 0;
  if (!call<u32>(0x02331F64, actor, at<void>(s)))
    return 0;
  gabi::Local<cXyz> origin;
  origin->x = load<f32>(s + 0x314);
  origin->y = load<f32>(s + 0x318);
  origin->z = load<f32>(s + 0x31c);
  gabi::Local<cXyz> pos;
  call(0x02332024, actor, at<void>(s), pos.get());
  gabi::Local<be<f32>[8]> tmp;
  for (int i = 0; i < 3; i++)
    (*tmp)[i] = load<f32>(origin.a + i * 4);
  for (int i = 0; i < 3; i++)
    (*tmp)[i + 3] = load<f32>(pos.a + i * 4);
  (*tmp)[6] = 200.f;
  (*tmp)[7] = 400.f;
  if (!call<u32>(0x02332468, actor, tmp.get()))
    return 0;
  f32 x = pos->x, y = pos->y, z = pos->z;
  store<f32>(o + 4, y);
  store<f32>(o + 8, z);
  store<f32>(o, x);
  u32 pattern = call<u32>(0x023324E8, actor);
  u32 type = call<u32>(0x02332590, pattern);
  store<u32>(o + 12, type);
  u32 item = call<u32>(0x023325A8, pattern);
  store<u32>(o + 16, item);
  u32 buoy = call<u32>(0x023325B8, pattern);
  store<u8>(o + 20, buoy);
  s32 angle = call<s32>(0x0200F93C, at<void>(s + 0x314), pos.get());
  s32 rand = gabi::ftoi(call<f32>(0x02019918, 8192.f));
  store<u16>(o + 22, (u32)angle + (u32)(s16)rand);
  return 1;
}
VERIFY(0x023325C4, coming_param);

void coming_wait(void *actor, void *ctrl) {
  WWHD_FUNC(0x02332728, void, actor, ctrl);
  u32 a = gabi::ea(actor), c = gabi::ea(ctrl);
  s8 room = load<s8>(0x1047e6c8);
  if (!call<u32>(0x02331DD8, actor))
    return;
  if (load<s8>(a + 0x2fe) != room && load<s8>(a + 0x3ec) != room &&
      !call<u32>(0x02332D84, actor, 1, 6))
    return;
  if (load<s32>(c + 4) > 0)
    return;
  gabi::Local<be<u8>[24]> param;
  if (call<u32>(0x023325C4, actor, param.get())) {
    s8 createRoom = load<s8>(a + 0x2fe);
    u32 item = load<u32>(param.a + 16);
    s16 angle = load<s16>(param.a + 22);
    u32 type = load<u32>(param.a + 12);
    u8 buoy = load<u8>(param.a + 20);
    gabi::Local<csXyz> rot;
    call(0x0201A478, rot.get(), 0, angle, 0);
    u32 parameters = (item & 63) | 0x007f0000 | (type << 24) |
                     ((u32)(buoy ^ 1) << 8) | 0x10000000;
    u32 id = call<u32>(0x025D5834, 0x1c9, parameters, param.get(), createRoom,
                       rot.get(), 0, -1, 0);
    store<u32>(c + 8, id);
    call(0x02331B4C, ctrl, actor);
    store<u32>(c, 1);
  } else
    call(0x02331B00, ctrl, actor);
}
VERIFY(0x02332728, coming_wait);

void coming_appear(void *actor, void *ctrl) {
  WWHD_FUNC(0x02332868, void, actor, ctrl);
  u32 c = gabi::ea(ctrl);
  if (load<s32>(c + 4) > 0)
    return;
  u32 id = load<u32>(c + 8);
  if (id == 0xffffffff)
    return;
  gabi::Local<be<u32>> found;
  if (!call<u32>(0x025D54C4, id, found.get())) {
    store<u32>(c + 8, 0xffffffff);
    return;
  }
  u32 a = *found;
  if (a) {
    if (load<s16>(a + 8) != 0x1c9)
      call(0x0273AA24, at<void>(0x10026f88), 0x465, at<void>(0x10026f58));
    store<u8>(a + 0x590, 1);
  }
  if (load<u32>(c + 8) != 0xffffffff) {
    call(0x02331A44, ctrl, actor);
    store<u32>(c, 0);
  }
}
VERIFY(0x02332868, coming_appear);

void coming_leave(void *actor, void *ctrl) {
  WWHD_FUNC(0x02332960, void, actor, ctrl);
  u32 c = gabi::ea(ctrl);
  u32 id = load<u32>(c + 8);
  if (id == 0xffffffff)
    return;
  gabi::Local<be<u32>> found;
  if (call<u32>(0x025D54C4, id, found.get())) {
    u32 a = *found;
    if (a) {
      if (load<s16>(a + 8) != 0x1c9)
        call(0x0273AA24, at<void>(0x10026fcc), 0x480, at<void>(0x10026f9c));
      store<u8>(a + 0x590, 1);
    }
  } else
    store<u32>(c + 8, 0xffffffff);
}
VERIFY(0x02332960, coming_leave);

s32 coming_createCallback(void *actor) {
  WWHD_FUNC(0x02332A00, s32, actor);
  return coming_create(actor);
}
VERIFY(0x02332A00, coming_createCallback);

s32 coming_deleteCallback(void *actor) {
  WWHD_FUNC(0x02332A04, s32, actor);
  return 1;
}
VERIFY(0x02332A04, coming_deleteCallback);

s32 coming_executeCallback(void *actor) {
  WWHD_FUNC(0x02332A0C, s32, actor);
  return coming_execute(actor);
}
VERIFY(0x02332A0C, coming_executeCallback);

s32 coming_drawCallback(void *actor) {
  WWHD_FUNC(0x02332A10, s32, actor);
  return 1;
}
VERIFY(0x02332A10, coming_drawCallback);

void coming_sinit() {
  WWHD_FUNC(0x02332A18, void);
  u32 c = 0x104694b8;
  store<u32>(c + 8, 0);
  store<u32>(c, 0);
  store<u32>(c + 12, 0);
  store<u32>(c + 4, 0);
  call(0x028F026C, at<void>(0x101c89c0));
  store<f32>(0x104694b0, 3.1415927410125732f);
  store<f32>(0x104694ac, -3.1415927410125732f);
  call(0x028ED6F8, at<void>(0x104694b4));
  call(0x028F026C, at<void>(0x101c89cc));
  call(0x028EAB2C, at<void>(0x104694b5));
  call(0x028F026C, at<void>(0x101c89d8));
  c = 0x1046951c;
  call(0x02008E0C, at<void>(c));
  store<u8>(c + 0x45, 0);
  store<u8>(c + 0x46, 0);
  store<u8>(c + 0x47, 0);
  store<u32>(c, c + 0x40);
  store<u8>(c + 0x48, 0);
  store<u8>(c + 0x49, 0);
  store<u8>(c + 0x4a, 0);
  store<u32>(c + 4, c + 0x4c);
  store<u32>(c + 0x50, 1);
  store<u8>(c + 0x44, 1);
  store<u32>(c + 0x10, 0x10026c90);
  store<u32>(c + 0x20, 0x10026ca0);
  store<u32>(c + 0x4c, 0x10026cb0);
  store<u32>(c + 0x40, 0x10026cc0);
  call(0x028F026C, at<void>(0x101c89e4));
  c = 0x10469570;
  call(0x02008FEC, at<void>(c));
  store<u8>(c + 0x5d, 0);
  store<u32>(c + 0x68, 1);
  store<u32>(c, c + 0x58);
  store<u8>(c + 0x5e, 0);
  store<u32>(c + 4, c + 0x64);
  store<u8>(c + 0x5f, 0);
  store<u32>(c + 0x10, 0x10026d70);
  store<u8>(c + 0x60, 0);
  store<u32>(c + 0x20, 0x10026d80);
  store<u32>(c + 0x64, 0x10026d90);
  store<u8>(c + 0x61, 0);
  store<u8>(c + 0x62, 0);
  store<u32>(c + 0x58, 0x10026da0);
  store<u8>(c + 0x5c, 1);
  call(0x028F026C, at<void>(0x101c89f0));
}
VERIFY(0x02332A18, coming_sinit);

void coming_gndDtor(void *obj, u32 flags) {
  WWHD_FUNC(0x02332BC0, void, obj, flags);
  u32 a = gabi::ea(obj);
  if (a) {
    store<u32>(a + 0x20, 0x10026c60);
    store<u32>(a + 0x40, 0x10026c80);
    store<u32>(a + 0x4c, 0x10026c40);
    call(0x02008DAC, at<void>(a + 0), 0);
    if (flags & 1)
      call(0x0273AF40, obj);
  }
}
VERIFY(0x02332BC0, coming_gndDtor);

void coming_waterDtor(void *obj, u32 flags) {
  WWHD_FUNC(0x02332C38, void, obj, flags);
  u32 a = gabi::ea(obj);
  if (a) {
    store<u32>(a + 0x20, 0x10026cd0);
    store<u32>(a + 0x24, 0x10026cf0);
    store<u32>(a + 0x30, 0x10026c40);
    call(0x02008B4C, at<void>(a + 16), 0);
    if (flags & 1)
      call(0x0273AF40, obj);
  }
}
VERIFY(0x02332C38, coming_waterDtor);

void coming_lineDtor(void *obj, u32 flags) {
  WWHD_FUNC(0x02332CB0, void, obj, flags);
  u32 a = gabi::ea(obj);
  if (a) {
    store<u32>(a + 0x58, 0x10026d60);
    store<u32>(a + 0x64, 0x10026c40);
    store<u32>(a + 0x20, 0x10026c30);
    call(0x02008B4C, at<void>(a + 0), 0);
    if (flags & 1)
      call(0x0273AF40, obj);
  }
}
VERIFY(0x02332CB0, coming_lineDtor);

void coming_actorDtor(void *obj, u32 flags) {
  WWHD_FUNC(0x02332D28, void, obj, flags);
  u32 a = gabi::ea(obj);
  if (a) {
    call(0x025D50BC, at<void>(a + 0), 0);
    if (flags & 1)
      call(0x0273AF40, obj);
  }
}
VERIFY(0x02332D28, coming_actorDtor);

s32 coming_isDelete(void *actor) {
  WWHD_FUNC(0x02332D7C, s32, actor);
  return 1;
}
VERIFY(0x02332D7C, coming_isDelete);

u32 coming_prm(void *actor, u32 width, u32 shift) {
  WWHD_FUNC(0x02332D84, u32, actor, width, shift);
  u32 value = load<u32>(gabi::ea(actor) + 0xb0);
  u32 mask = (width & 32) ? 0 : (1u << (width & 31));
  u32 bits = (shift & 32) ? 0 : (value >> (shift & 31));
  return bits & (mask - 1);
}
VERIFY(0x02332D84, coming_prm);

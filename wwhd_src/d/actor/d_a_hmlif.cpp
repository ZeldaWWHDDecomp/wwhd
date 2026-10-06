// Tower of the Gods glowing platform: audited HD actor layout.
#include "bindings.h"
using gabi::at;
using gabi::call;
using gabi::load;
using gabi::store;
struct HmlifName {
  be<u32> text, vt;
};
s32 hmlif_heap(void *);
void hmlif_smooth(void *);
void hmlif_normal(void *);
s32 hmlif_createPhase(void *actor) {
  WWHD_FUNC(0x021753A8, s32, actor);
  u32 a = gabi::ea(actor);
  u32 flags = load<u32>(a + 0x2e4);
  if (!(flags & 8)) {
    if (a) {
      call(0x024F1D40, actor);
      store<u32>(a + 0xb4, 0x100114f0);
      call(0x025166F0, at<void>(a + 0x3f4));
      call(0x0200BD2C, at<void>(a + 0x520));
      call(0x02515DA0, at<void>(a + 0x53c));
      flags = load<u32>(a + 0x2e4);
      store<u32>(a + 0x538, 0x1004ae88);
      store<u32>(a + 0x53c, 0x1004aec0);
    }
    store<u32>(a + 0x2e4, flags | 8);
  }
  u32 type = (load<u32>(a + 0xb0) >> 27) & 15;
  if (type >= 3)
    type = 0;
  store<u8>(a + 0x5a1, type);
  s32 phase = call<s32>(0x02520460, at<void>(a + 0x3e0),
                        at<void>(load<u32>(0x101b7168 + type * 4)));
  if (phase == 4) {
    u32 t = load<u8>(a + 0x5a1);
    phase =
        call<s32>(0x024F1D9C, actor, at<void>(load<u32>(0x101b7168 + t * 4)),
                  s32(load<s16>(0x10011448 + t * 2)), at<void>(0x024ee708),
                  s32(load<s16>(0x10011460 + t * 2)));
  }
  return phase;
}
VERIFY(0x021753A8, hmlif_createPhase);
s32 hmlif_createCallback(void *actor) {
  WWHD_FUNC(0x021754AC, s32, actor);
  return hmlif_createPhase(actor);
}
VERIFY(0x021754AC, hmlif_createCallback);
s32 hmlif_deletePhase(void *actor) {
  WWHD_FUNC(0x021754B0, s32, actor);
  u32 a = gabi::ea(actor);
  s32 result = call<s32>(0x024F1F64, actor);
  call(0x025204C8, at<void>(a + 0x3e0),
       at<void>(load<u32>(0x101b7168 + load<u8>(a + 0x5a1) * 4)));
  return result;
}
VERIFY(0x021754B0, hmlif_deletePhase);
s32 hmlif_deleteCallback(void *actor) {
  WWHD_FUNC(0x02175508, s32, actor);
  return hmlif_deletePhase(actor);
}
VERIFY(0x02175508, hmlif_deleteCallback);
s32 hmlif_drawCallback(void *actor) {
  WWHD_FUNC(0x0217550C, s32, actor);
  return call<s32>(load<u32>(load<u32>(gabi::ea(actor) + 0xb4) + 0x2c), actor);
}
VERIFY(0x0217550C, hmlif_drawCallback);
s32 hmlif_executeCallback(void *actor) {
  WWHD_FUNC(0x0217551C, s32, actor);
  return call<s32>(0x024F1E9C, actor);
}
VERIFY(0x0217551C, hmlif_executeCallback);
s32 hmlif_heap(void *actor) {
  WWHD_FUNC(0x02175520, s32, actor);
  u32 a = gabi::ea(actor);
  gabi::Local<HmlifName> name0;
  name0->vt = 0x10011364;
  name0->text = load<u32>(0x101b7168 + load<u8>(a + 0x5a1) * 4);
  u32 data = call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), name0.get(),
                       s32(load<s16>(0x10011440 + load<u8>(a + 0x5a1) * 2)));
  if (!data)
    call(0x0273AA24, at<void>(0x1001139c), 0x18f, at<void>(0x100113ac));
  s16 idx = load<s16>(0x10011450 + load<u8>(a + 0x5a1) * 2);
  u32 model = call<u32>(0x025E38E0, at<void>(data), 0x80000,
                        idx == -1 ? 0x11000022 : 0x11020022);
  store<u32>(a + 0x3e8, model);
  if (!model)
    return 0;
  u32 t = load<u8>(a + 0x5a1);
  store<u32>(a + 0x590, load<u8>(a + 0x2fd));
  store<u32>(a + 0x3ec, 0);
  idx = load<s16>(0x10011450 + t * 2);
  if (idx != -1) {
    f32 speed = load<f32>(0x1001137c);
    gabi::Local<HmlifName> name1;
    name1->vt = 0x10011364;
    name1->text = load<u32>(0x101b7168 + t * 4);
    u32 anim = call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)),
                         name1.get(), s32(idx));
    if (!anim)
      call(0x0273AA24, at<void>(0x1001139c), 0x1a6, at<void>(0x10011384));
    u32 btp = call<u32>(0x025E7820, 0);
    store<u32>(a + 0x3ec, btp);
    if (!btp)
      return 0;
    if (!call<u32>(0x025E789C, at<void>(btp), at<void>(data), at<void>(anim), 0,
                   0, speed, 0, -1, 0, 0))
      return 0;
    u32 current = load<u32>(a + 0x3ec);
    store<s16>(a + 0x5a2, 0);
    if (current) {
      s8 room = load<s8>(a + 0x2fe);
      u32 save = load<u32>(0x101f84dc), sw = load<u32>(a + 0x590);
      if (call<u32>(0x025BA0C0, at<void>(save + 0x20), sw, room))
        store<s16>(a + 0x5a2, 3);
    }
  }
  t = load<u8>(a + 0x5a1);
  store<u32>(a + 0x3f0, 0);
  idx = load<s16>(0x10011458 + t * 2);
  if (idx != -1) {
    f32 speed = load<f32>(0x1001137c);
    gabi::Local<HmlifName> name2;
    name2->vt = 0x10011364;
    name2->text = load<u32>(0x101b7168 + t * 4);
    u32 anim = call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)),
                         name2.get(), s32(idx));
    if (!anim)
      call(0x0273AA24, at<void>(0x1001139c), 0x1bf, at<void>(0x10011390));
    u32 brk = call<u32>(0x025E80D0, 0);
    store<u32>(a + 0x3f0, brk);
    if (!brk)
      return 0;
    if (!call<u32>(0x025E8154, at<void>(brk), at<void>(data), at<void>(anim), 1,
                   2, speed, 0, -1, 0, 0))
      return 0;
    store<f32>(load<u32>(a + 0x3f0), load<f32>(0x10011380));
  }
  return 1;
}
VERIFY(0x02175520, hmlif_heap);
u32 hmlif_switchPath(void *actor) {
  WWHD_FUNC(0x021757B8, u32, actor);
  return load<u32>(gabi::ea(actor) + 0x58c) != 255;
}
VERIFY(0x021757B8, hmlif_switchPath);
void hmlif_move(void *actor) {
  WWHD_FUNC(0x02175DE4, void, actor);
  u32 a = gabi::ea(actor);
  if (load<u8>(a + 0x55d) == 255)
    return;
  if (load<u8>(a + 0x596))
    hmlif_smooth(actor);
  else
    hmlif_normal(actor);
}
VERIFY(0x02175DE4, hmlif_move);
void hmlif_sound(void *actor) {
  WWHD_FUNC(0x02175E04, void, actor);
  u32 a = gabi::ea(actor);
  if (call<u32>(0x0201AFD8, at<void>(a + 0x300), at<void>(a + 0x314))) {
    s32 reverb = call<s32>(0x02520540, s32(load<s8>(a + 0x326)));
    call(0x025E1A40, 0x3025, at<void>(a + 0x37c), 0, reverb);
  }
}
VERIFY(0x02175E04, hmlif_sound);
void hmlif_anim(void *actor) {
  WWHD_FUNC(0x02175E60, void, actor);
  u32 a = gabi::ea(actor);
  s8 room = load<s8>(a + 0x2fe);
  u32 save = load<u32>(0x101f84dc);
  u32 on =
      call<u32>(0x025BA0C0, at<void>(save + 0x20), load<u32>(a + 0x590), room);
  s16 frame = s16(load<s16>(a + 0x5a2) + (on ? 1 : -1));
  if (on) {
    if (frame > 3)
      frame = 3;
  } else if (frame < 0)
    frame = 0;
  u32 brk = load<u32>(a + 0x3f0);
  store<s16>(a + 0x5a2, frame);
  if (!brk)
    return;
  u32 stopped = call<u32>(0x0201AF98, at<void>(a + 0x300), at<void>(a + 0x314));
  brk = load<u32>(a + 0x3f0);
  if (stopped)
    store<u8>(brk + 0xe, 0);
  else {
    store<u8>(brk + 0xe, 2);
    store<f32>(load<u32>(a + 0x3f0), load<f32>(0x1001137c));
  }
  call(0x025E742C, at<void>(load<u32>(a + 0x3f0)));
}
VERIFY(0x02175E60, hmlif_anim);

s32 hmlif_nextPath(void *actor) {
  WWHD_FUNC(0x021757CC, s32, actor);
  u32 a = gabi::ea(actor);
  s8 room = load<s8>(a + 0x326);
  u32 old = load<u32>(a + 0x564);
  u32 path = call<u32>(0x025AAF88, u32(load<u8>(a + 0x55c)), s32(room));
  if (!path)
    call(0x0273AA24, at<void>(0x100113c0), 0x42f, at<void>(0x100113d0));
  u32 next = call<u32>(0x025AB070, at<void>(path), s32(room));
  u32 save = load<u32>(0x101f84dc);
  s8 saveRoom = load<s8>(a + 0x2fe);
  if (call<u32>(0x025BA0C0, at<void>(save + 0x20), load<u32>(a + 0x58c),
                s32(saveRoom))) {
    if (old == path)
      store<u8>(a + 0x55f, load<u8>(a + 0x55e));
    store<u32>(a + 0x564, next);
    store<u8>(a + 0x560, 1);
    store<u8>(a + 0x55e, 1);
    return 1;
  }
  if (load<u32>(a + 0x564) == path)
    return 0;
  store<u32>(a + 0x564, path);
  store<u8>(a + 0x55d, load<u8>(a + 0x55c));
  u8 anchor = load<u8>(path + 4);
  u16 count = load<u16>(path);
  if (anchor == count)
    store<u8>(a + 0x55f, 255);
  else if (load<u8>(path + 4) == 0)
    store<u8>(a + 0x55f, 1);
  else {
    u8 dir = load<u8>(a + 0x55f);
    u32 p = load<u32>(a + 0x564);
    store<u8>(a + 0x55e, dir);
    store<u8>(a + 0x560, load<u8>(p + 4) + dir);
    return 1;
  }
  u32 p = load<u32>(a + 0x564);
  u8 dir = load<u8>(a + 0x55f);
  store<u8>(a + 0x55e, dir);
  store<u8>(a + 0x560, load<u8>(p + 4) + dir);
  return 1;
}
VERIFY(0x021757CC, hmlif_nextPath);
void hmlif_nextPoint(void *actor) {
  WWHD_FUNC(0x02175908, void, actor);
  u32 a = gabi::ea(actor);
  s32 index = load<s8>(a + 0x560);
  if (load<u8>(a + 0x55d) == 255)
    return;
  bool switched = false;
  if (call<u32>(0x021757B8, actor)) {
    s8 room = load<s8>(a + 0x326);
    u32 first = call<u32>(0x025AAF88, u32(load<u8>(a + 0x55c)), s32(room));
    u32 path = load<u32>(a + 0x564);
    bool junction = path == first && u32(index) == load<u8>(path + 4);
    if (!junction) {
      first = call<u32>(0x025AAF88, u32(load<u8>(a + 0x55c)), s32(room));
      junction = path != first && index == 0;
    }
    if (junction) {
      u32 change = call<u32>(0x021757CC, actor);
      if (!load<u32>(a + 0x564))
        call(0x0273AA24, at<void>(0x100113e0), 0x3f5, at<void>(0x100113f0));
      switched = change != 0;
    }
    if (!switched)
      index = load<s8>(a + 0x560);
  }
  if (!switched) {
    index = s8(index + load<u8>(a + 0x55e));
    u32 path = load<u32>(a + 0x564);
    store<u8>(a + 0x560, index);
    if (load<u8>(path + 5) & 1) {
      u16 count = load<u16>(path);
      if (index > s32(s8(count)) - 1)
        store<u8>(a + 0x560, 0);
      else if (index < 0)
        store<u8>(a + 0x560, count - 1);
    } else {
      u16 count = load<u16>(path);
      if (index > s32(count) - 1) {
        u32 p = load<u32>(a + 0x564);
        store<u8>(a + 0x55e, 255);
        store<u8>(a + 0x560, load<u16>(p) - 2);
      } else if (index < 0) {
        store<u8>(a + 0x55e, 1);
        store<u8>(a + 0x560, 1);
      }
    }
  }
  u32 y = load<u32>(a + 0x56c), path = load<u32>(a + 0x564);
  store<u32>(a + 0x578, y);
  u32 z = load<u32>(a + 0x570), x = load<u32>(a + 0x568);
  s32 point = load<s8>(a + 0x560);
  store<u32>(a + 0x574, x);
  store<u32>(a + 0x57c, z);
  u32 data = load<u32>(path + 8) + u32(point) * 16;
  store<f32>(a + 0x568, load<f32>(data + 4));
  store<f32>(a + 0x56c, load<f32>(data + 8));
  store<f32>(a + 0x570, load<f32>(data + 12));
}
VERIFY(0x02175908, hmlif_nextPoint);
void hmlif_smooth(void *actor) {
  WWHD_FUNC(0x02175AD0, void, actor);
  u32 a = gabi::ea(actor);
  f32 oldx = load<f32>(a + 0x574), tx = load<f32>(a + 0x568),
      oldz = load<f32>(a + 0x57c), tz = load<f32>(a + 0x570);
  s32 yaw = call<s32>(0x020195B0, tx - oldx, tz - oldz);
  f32 dy = load<f32>(a + 0x56c) - load<f32>(a + 0x318),
      dx = load<f32>(a + 0x568) - load<f32>(a + 0x314),
      dz = load<f32>(a + 0x570) - load<f32>(a + 0x31c);
  f32 square = gabi::fmadds(dz, dz, gabi::fmadds(dx, dx, dy * dy)),
      speed = load<f32>(a + 0x370);
  f32 distance = call<f32>(0x028F4384, square);
  call(0x0200F428, at<void>(a + 0x322), yaw, 10, 0x400);
  call(0x0200ED84, at<void>(a + 0x370), load<f32>(a + 0x598),
       load<f32>(0x10011400), load<f32>(0x1001137c));
  gabi::Local<cXyz> v, out;
  v->x = load<f32>(0x10011380);
  v->y = load<f32>(0x10011380);
  v->z = speed;
  call(0x025F1884, at<void>(0x1048d0cc), s32(load<s16>(a + 0x322)));
  call(0x028E8F64, at<void>(0x1048d0cc), v.get(), out.get());
  f32 y = load<f32>(a + 0x318) + f32(out->y),
      x = load<f32>(a + 0x314) + f32(out->x),
      z = load<f32>(a + 0x31c) + f32(out->z);
  f32 threshold = load<f32>(0x10011404);
  store<f32>(a + 0x318, y);
  store<f32>(a + 0x314, x);
  store<f32>(a + 0x31c, z);
  if (distance < threshold)
    call(0x02175908, actor);
}
VERIFY(0x02175AD0, hmlif_smooth);
void hmlif_normal(void *actor) {
  WWHD_FUNC(0x02175C34, void, actor);
  u32 a = gabi::ea(actor);
  u8 state = load<u8>(a + 0x5a0);
  f32 factor = load<f32>(0x10011408), zero = load<f32>(0x10011380),
      one = load<f32>(0x1001137c), divisor = load<f32>(0x1001140c);
  if (state == 0) {
    store<u32>(a + 0x59c, 0);
    store<u8>(a + 0x5a0, 1);
    call(0x02175908, actor);
    state = 1;
  }
  if (state == 1) {
    f32 target = load<f32>(a + 0x598);
    u32 count = load<u32>(a + 0x59c);
    store<u32>(a + 0x59c, count + 1);
    f32 result = call<f32>(0x0200ECD4, at<void>(a + 0x370), target, factor, one,
                           target / divisor);
    if (result == zero)
      store<u8>(a + 0x5a0, 2);
  } else if (state == 2) {
    gabi::Local<cXyz> diff;
    store<u32>(a + 0x59c, 0);
    call(0x0201ADE0, at<void>(a + 0x314), diff.get(), at<void>(a + 0x568));
    f32 square = call<f32>(0x028E8DD0, diff.get());
    f32 distance = call<f32>(0x028F4384, square);
    if (distance < load<f32>(0x10011410))
      store<u8>(a + 0x5a0, 3);
  } else if (state == 3) {
    f32 target = load<f32>(a + 0x598);
    u32 count = load<u32>(a + 0x59c);
    f32 step = target / divisor;
    f32 divider = load<f32>(0x10011414);
    store<u32>(a + 0x59c, count + 1);
    f32 result = call<f32>(0x0200ECD4, at<void>(a + 0x370), target / divider,
                           factor, one, step);
    if (result == zero)
      store<u8>(a + 0x5a0, 0);
  }
  call(0x0200F164, at<void>(a + 0x314), at<void>(a + 0x568), one,
       load<f32>(a + 0x370));
}
VERIFY(0x02175C34, hmlif_normal);

void hmlif_collision(void *actor) {
  WWHD_FUNC(0x02175F48, void, actor);
  u32 a = gabi::ea(actor);
  if (call<u32>(0x025162A4, at<void>(a + 0x3f4))) {
    u32 hit = call<u32>(0x02516300, at<void>(a + 0x3f4));
    if (hit && (load<u32>(hit + 0x10) & 0x001c4000)) {
      call(0x0251621C, at<void>(a + 0x3f4));
      u32 save = load<u32>(0x101f84dc);
      s8 room = load<s8>(a + 0x2fe);
      if (!call<u32>(0x025BA0C0, at<void>(save + 0x20), load<u32>(a + 0x590),
                     s32(room))) {
        s32 reverb = call<s32>(0x02520540, s32(load<s8>(a + 0x326)));
        call(0x025E1A40, 0x695e, at<void>(a + 0x37c), 0, reverb);
      }
      room = load<s8>(a + 0x2fe);
      save = load<u32>(0x101f84dc);
      call(0x025B9E38, at<void>(save + 0x20), load<u32>(a + 0x590), s32(room));
    }
  }
  u8 type = load<u8>(a + 0x5a1);
  if (type != 1 && type != 2)
    return;
  gabi::Local<cXyz> pos;
  pos->x = load<f32>(a + 0x314);
  pos->z = load<f32>(a + 0x31c);
  f32 y = load<f32>(a + 0x318);
  pos->y = type == 1 ? y + load<f32>(0x10011418) : y - load<f32>(0x1001141c);
  call(0x02018D40, at<void>(a + 0x50c), pos.get());
  u32 play = call<u32>(0x025200D4);
  call(0x0200E240, at<void>(play + 0x26a4), at<void>(a + 0x3f4));
}
VERIFY(0x02175F48, hmlif_collision);
void hmlif_matrix(void *actor) {
  WWHD_FUNC(0x021760A8, void, actor);
  u32 a = gabi::ea(actor);
  f32 sx = load<f32>(a + 0x330), sy = load<f32>(a + 0x334),
      sz = load<f32>(a + 0x338);
  u32 model = load<u32>(a + 0x3e8);
  store<f32>(model + 0xbc, sx);
  store<f32>(model + 0xc0, sy);
  store<f32>(model + 0xc4, sz);
  call(0x028E93CC, at<void>(0x1048d0cc), load<f32>(a + 0x314),
       load<f32>(a + 0x318), load<f32>(a + 0x31c));
  call(0x025F1B48, at<void>(0x1048d0cc), s32(load<s16>(a + 0x320)),
       s32(load<s16>(a + 0x322)), s32(load<s16>(a + 0x324)));
  f32 m[12];
  for (u32 i = 0; i < 12; i++)
    m[i] = load<f32>(0x1048d0cc + i * 4);
  model = load<u32>(a + 0x3e8);
  // All source words are cached before the first model write, including
  // aliases.
  store<f32>(model + 0xdc, m[5]);
  store<f32>(model + 0xd4, m[3]);
  store<f32>(model + 0xe4, m[7]);
  store<f32>(model + 0xec, m[9]);
  store<f32>(model + 0xd8, m[4]);
  store<f32>(model + 0xf0, m[10]);
  store<f32>(model + 0xcc, m[1]);
  store<f32>(model + 0xc8, m[0]);
  store<f32>(model + 0xe0, m[6]);
  store<f32>(model + 0xd0, m[2]);
  store<f32>(model + 0xe8, m[8]);
  store<f32>(model + 0xf4, m[11]);
  call(0x028E90D4, at<void>(0x1048d0cc), at<void>(a + 0x3b0));
}
VERIFY(0x021760A8, hmlif_matrix);
s32 hmlif_execute(void *actor, void *output) {
  WWHD_FUNC(0x02176194, s32, actor, output);
  u32 a = gabi::ea(actor);
  call(0x02175DE4, actor);
  call(0x02175E04, actor);
  call(0x02175E60, actor);
  call(0x02175F48, actor);
  store<u32>(gabi::ea(output), a + 0x3b0);
  call(0x021760A8, actor);
  return 1;
}
VERIFY(0x02176194, hmlif_execute);
s32 hmlif_draw(void *actor) {
  WWHD_FUNC(0x021761F8, s32, actor);
  u32 a = gabi::ea(actor), env = call<u32>(0x02555D0C);
  call(0x025626A4, at<void>(env), 0, at<void>(a + 0x314), at<void>(a + 0x110));
  env = call<u32>(0x02555D0C);
  call(0x02562F5C, at<void>(env), at<void>(load<u32>(a + 0x3e8)),
       at<void>(a + 0x110));
  u32 btp = load<u32>(a + 0x3ec);
  if (btp) {
    u32 model = load<u32>(a + 0x3e8);
    s16 frame = load<s16>(a + 0x5a2);
    call(0x025E7B3C, at<void>(btp), at<void>(load<u32>(model + 0xac)),
         s32(frame));
  }
  u32 brk = load<u32>(a + 0x3f0);
  if (brk) {
    u32 model = load<u32>(a + 0x3e8);
    f32 frame = load<f32>(brk + 4);
    call(0x025E83FC, at<void>(brk), at<void>(load<u32>(model + 0xac)), frame);
  }
  u32 play = call<u32>(0x025200D4);
  store<u32>(0x104b4634, load<u32>(play + 0x5d70));
  play = call<u32>(0x025200D4);
  store<u32>(0x104b4638, load<u32>(play + 0x5d74));
  call(0x025E2DE0, at<void>(load<u32>(a + 0x3e8)), 0);
  play = call<u32>(0x025200D4);
  store<u32>(0x104b4634, load<u32>(play + 0x5d78));
  play = call<u32>(0x025200D4);
  store<u32>(0x104b4638, load<u32>(play + 0x5d7c));
  return 1;
}
VERIFY(0x021761F8, hmlif_draw);
s32 hmlif_create(void *actor) {
  WWHD_FUNC(0x021762C8, s32, actor);
  u32 a = gabi::ea(actor), model = load<u32>(a + 0x3e8);
  u8 type = load<u8>(a + 0x5a1);
  u32 box = 0x100114a8 + type * 24;
  store<u32>(a + 0x348, model ? model + 0xc8 : 0);
  call(0x025D674C, actor, load<f32>(box), load<f32>(box + 4),
       load<f32>(box + 8), load<f32>(box + 12), load<f32>(box + 16),
       load<f32>(box + 20));
  store<f32>(a + 0x364, load<f32>(0x1001137c));
  call(0x021760A8, actor);
  u32 param = load<u32>(a + 0xb0);
  u8 id = param >> 8;
  store<u32>(a + 0x58c, param & 255);
  store<u8>(a + 0x55c, id);
  store<u8>(a + 0x55d, id);
  if (id == 255)
    return 0;
  u32 path = call<u32>(0x025AAF88, u32(id), s32(load<s8>(a + 0x326)));
  store<u32>(a + 0x564, path);
  if (!path) {
    store<u8>(a + 0x55d, 255);
    return 0;
  }
  u32 sw = load<u32>(a + 0x58c);
  u16 count = load<u16>(path);
  if (sw != 255 && load<u8>(path + 4) > count)
    return 0;
  u32 prm = load<u32>(a + 0xb0), start = (prm >> 22) & 31,
      points = load<u32>(path + 8);
  // The HD five-bit start field retains an unreachable comparison with255.
  if (start == 255) {
    f32 pick = call<f32>(0x020198D8, f32(s32(count) - 1));
    start = u8(gabi::ftoi(pick));
    count = load<u16>(load<u32>(a + 0x564));
  }
  if (start >= count)
    start = 0;
  u32 point = points + start * 16;
  f32 x = load<f32>(point + 4);
  store<f32>(a + 0x314, x);
  f32 y = load<f32>(point + 8);
  u32 xb = load<u32>(a + 0x314);
  store<f32>(a + 0x318, y);
  f32 z = load<f32>(point + 12);
  store<u32>(a + 0x574, xb);
  store<f32>(a + 0x31c, z);
  u32 zb = load<u32>(a + 0x31c), yb = load<u32>(a + 0x318);
  store<u8>(a + 0x560, start);
  store<u32>(a + 0x578, yb);
  store<f32>(a + 0x568, x);
  store<f32>(a + 0x56c, y);
  u32 direction = (load<u32>(a + 0xb0) >> 20) & 3;
  store<f32>(a + 0x570, z);
  store<u32>(a + 0x57c, zb);
  if (direction == 2) {
    store<u8>(a + 0x55e, 1);
    f32 choice = call<f32>(0x02019788);
    if (choice < load<f32>(0x10011428))
      store<u8>(a + 0x55f, load<u8>(a + 0x55e));
    else {
      store<u8>(a + 0x55e, 255);
      store<u8>(a + 0x55f, 255);
    }
  } else {
    u8 dir = direction == 1 ? 255 : 1;
    store<u8>(a + 0x55e, dir);
    store<u8>(a + 0x55f, dir);
  }
  prm = load<u32>(a + 0xb0);
  u32 speed = (prm >> 16) & 15;
  if (speed > 8)
    speed = 8;
  sw = load<u32>(a + 0x58c);
  f32 value = load<f32>(0x10011468 + speed * 4);
  store<u8>(a + 0x596, prm >> 31);
  store<f32>(a + 0x370, value);
  store<f32>(a + 0x598, value);
  if (sw != 255) {
    path = load<u32>(a + 0x564);
    point = load<u32>(path + 8) + load<u8>(path + 4) * 16;
    store<f32>(a + 0x580, load<f32>(point + 4));
    store<f32>(a + 0x584, load<f32>(point + 8));
    store<f32>(a + 0x588, load<f32>(point + 12));
  }
  type = load<u8>(a + 0x5a1);
  if (type == 1 || type == 2) {
    gabi::Local<cXyz> center;
    f32 cy = load<f32>(a + 0x318), cx = load<f32>(a + 0x314);
    f32 height = load<f32>(0x1001142c), radius = load<f32>(0x10011430);
    center->x = cx;
    center->z = load<f32>(a + 0x31c);
    center->y = cy + height;
    call(0x02018E88, at<void>(a + 0x50c), center.get(), radius);
    u32 flags = load<u32>(a + 0x40c);
    store<u32>(a + 0x41c, 0x001c4000);
    store<u32>(a + 0x40c, (flags & ~0xeu) | 8);
    call(0x02515F14, at<void>(a + 0x520), 255, 255, actor);
    store<u32>(a + 0x438, a + 0x520);
    call(0x0251677C, at<void>(a + 0x3f4), at<void>(0x101b70b4));
  }
  return 1;
}
VERIFY(0x021762C8, hmlif_create);
void hmlif_static() {
  WWHD_FUNC(0x0217661C, void, u32(0));
  store<u32>(0x1046473c, 0);
  store<u32>(0x10464734, 0);
  store<u32>(0x10464740, 0);
  store<u32>(0x10464738, 0);
  call(0x028F026C, at<void>(0x101b7114));
  f32 lo = load<f32>(0x10011438), hi = load<f32>(0x1001143c);
  store<f32>(0x10464728, lo);
  store<f32>(0x1046472c, hi);
  call(0x028ED6F8, at<void>(0x10464730));
  call(0x028F026C, at<void>(0x101b7120));
  call(0x028EAB2C, at<void>(0x10464731));
  call(0x028F026C, at<void>(0x101b712c));
}
VERIFY(0x0217661C, hmlif_static);
s32 hmlif_isDelete(void *actor) {
  WWHD_FUNC(0x021766B0, s32, actor);
  return 1;
}
VERIFY(0x021766B0, hmlif_isDelete);
void hmlif_destructor(void *actor, u32 flags) {
  WWHD_FUNC(0x021766B8, void, actor, flags);
  if (actor && (flags & 1))
    call(0x0273AF40, actor);
}
VERIFY(0x021766B8, hmlif_destructor);
s32 hmlif_moveBgIsDelete(void *actor) {
  WWHD_FUNC(0x021766CC, s32, actor);
  return 1;
}
VERIFY(0x021766CC, hmlif_moveBgIsDelete);
s32 hmlif_delete(void *actor) {
  WWHD_FUNC(0x021766D4, s32, actor);
  return 1;
}
VERIFY(0x021766D4, hmlif_delete);

void hmlif_actorDestructor(void *actor, u32 flags) {
  WWHD_FUNC(0x021766DC, void, actor, flags);
  u32 a = gabi::ea(actor);
  if (!a)
    return;
  call(0x02515860, at<void>(a + 0x520), 2);
  call(0x02515AE8, at<void>(a + 0x3f4), 2);
  call(0x025D50BC, actor, 0);
  if (flags & 1)
    call(0x0273AF40, actor);
}
VERIFY(0x021766DC, hmlif_actorDestructor);
void hmlif_empty() { WWHD_FUNC(0x02176748, void, u32(0)); }
VERIFY(0x02176748, hmlif_empty);

#include "gabi.h"
using namespace gabi;
struct MachineName {
  be<u32> text, vt;
};
struct MachineVector {
  be<f32> x, y, z;
};
s32 machine_delete(void *actor);
s32 machine_heap(void *actor) {
  WWHD_FUNC(0x021B7364, s32, actor);
  u32 a = ea(actor);
  Local<MachineName> n;
  n->text = 0x10014360;
  n->vt = 0x10014234;
  u32 data = call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), n.get(), 7);
  if (!data)
    call(0x0273AA24, at<void>(0x100142c8), 0x159, at<void>(0x100142e4));
  u32 model = call<u32>(0x025E38E0, at<void>(data), 0x80000, 0x11000222);
  store<u32>(a + 0x3b4, model);
  if (!model)
    return 0;
  Local<MachineName> n2;
  n2->vt = 0x10014234;
  n2->text = 0x10014360;
  u32 anim =
      call<u32>(0x026066C4, at<void>(load<u32>(0x101f4f28)), n2.get(), 4);
  if (!anim)
    call(0x0273AA24, at<void>(0x100142c8), 0x169, at<void>(0x100142d8));
  if (!call<u32>(0x025E8508, at<void>(a + 0xd20), at<void>(data),
                 at<void>(anim), 1, 0, load<f32>(0x100142c4), 0, -1, 0))
    return 0;
  model = load<u32>(a + 0x3b4);
  f32 frame = load<f32>(a + 0xd24);
  u32 md = load<u32>(model + 0xac);
  call(0x025E86B8, at<void>(a + 0xd20), at<void>(md), frame);
  return 1;
}
VERIFY(0x021B7364, machine_heap);
s32 machine_heapCallback(void *actor) {
  WWHD_FUNC(0x021B7490, s32, actor);
  return machine_heap(actor);
}
VERIFY(0x021B7490, machine_heapCallback);
s32 machine_node(void *node, s32 timing) {
  WWHD_FUNC(0x021B7494, s32, node, timing);
  if (timing == 0) {
    u32 joint = call<u32>(0x027F7878, node), model = load<u32>(0x104b462c),
        a = load<u32>(model + 0xb8);
    u16 index = load<u16>(joint + 4);
    if (a) {
      u32 buf = load<u32>(model + 0x2c);
      u16 flag = load<u16>(buf + 4);
      u32 matrices = load<u32>(buf + 0x10);
      store<u16>(buf + 4, flag | 16);
      call(0x028E90D4, at<void>(matrices + index * 48), at<void>(0x1048d0cc));
      call(0x028E90D4, at<void>(0x1048d0cc), at<void>(a + 0xdd4));
    }
  }
  return 1;
}
VERIFY(0x021B7494, machine_node);
void machine_matrix(void *actor) {
  WWHD_FUNC(0x021B751C, void, actor);
  u32 a = ea(actor), m = load<u32>(a + 0x3b4);
  f32 z = load<f32>(a + 0x338);
  s16 yrot = load<s16>(a + 0x322), xrot = load<s16>(a + 0x320);
  f32 x = load<f32>(a + 0x330), y = load<f32>(a + 0x334);
  s16 zrot = load<s16>(a + 0x324);
  store<f32>(m + 0xbc, x);
  store<f32>(m + 0xc4, z);
  store<f32>(m + 0xc0, y);
  call(0x028E93CC, at<void>(0x1048d0cc), load<f32>(a + 0x314),
       load<f32>(a + 0x318), load<f32>(a + 0x31c));
  call(0x025F1B48, at<void>(0x1048d0cc), s32(xrot), s32(yrot), s32(zrot));
  f32 v[12];
  for (u32 i = 0; i < 12; i++)
    v[i] = load<f32>(0x1048d0cc + i * 4);
  m = load<u32>(a + 0x3b4);
  const u32 order[] = {2, 3, 5, 6, 7, 8, 9, 10, 0, 1, 11, 4};
  for (u32 i : order)
    store<f32>(m + 0xc8 + i * 4, v[i]);
}
VERIFY(0x021B751C, machine_matrix);
s32 machine_delete(void *actor) {
  WWHD_FUNC(0x021B7A3C, s32, actor);
  call(0x025204C8, at<void>(ea(actor) + 0x3ac), at<void>(0x10014360));
  return 1;
}
VERIFY(0x021B7A3C, machine_delete);
s32 machine_deleteCallback(void *actor) {
  WWHD_FUNC(0x021B7A6C, s32, actor);
  return machine_delete(actor);
}
VERIFY(0x021B7A6C, machine_deleteCallback);
s32 machine_draw(void *actor) {
  WWHD_FUNC(0x021B7A70, s32, actor);
  u32 a = ea(actor);
  u32 env = call<u32>(0x02555D0C);
  call(0x025626A4, at<void>(env), 0, at<void>(a + 0x314), at<void>(a + 0x110));
  env = call<u32>(0x02555D0C);
  call(0x02562F5C, at<void>(env), at<void>(load<u32>(a + 0x3b4)),
       at<void>(a + 0x110));
  u32 m = load<u32>(a + 0x3b4);
  f32 frame = load<f32>(a + 0xd24);
  u32 md = load<u32>(m + 0xac);
  call(0x025E86B8, at<void>(a + 0xd20), at<void>(md), frame);
  call(0x025E2DE0, at<void>(load<u32>(a + 0x3b4)), 0);
  return 1;
}
VERIFY(0x021B7A70, machine_draw);
s32 machine_drawCallback(void *actor) {
  WWHD_FUNC(0x021B7AE0, s32, actor);
  return machine_draw(actor);
}
VERIFY(0x021B7AE0, machine_drawCallback);
u32 machine_search(void *actor) {
  WWHD_FUNC(0x021B7AE4, u32, actor);
  Local<be<s16>> id;
  *id = 0x72;
  return call<u32>(0x025D5218, at<void>(0x025e121c), id.get());
}
VERIFY(0x021B7AE4, machine_search);
void machine_speed(void *actor) {
  WWHD_FUNC(0x021B7B18, void, actor);
  u32 a = ea(actor), wind = machine_search(actor);
  f32 ratio;
  if (wind) {
    u8 id = load<u8>(wind + 0x1534);
    s16 speed = load<s16>(wind + 0x1538),
        base = load<s16>(0x1004bcf0 + u32(id) * 2);
    ratio = f32(speed) / f32(base);
  } else
    ratio = load<f32>(0x10014320);
  Local<be<f32>> value;
  *value = load<f32>(a + 0x370);
  call(0x0200ECD4, value.get(), load<f32>(0x10014324) * ratio,
       load<f32>(0x10014328), load<f32>(0x100142c4), load<f32>(0x1001432c));
  store<f32>(a + 0x370, *value);
}
VERIFY(0x021B7B18, machine_speed);
void machine_next(void *actor) {
  WWHD_FUNC(0x021B7C24, void, actor);
  u32 a = ea(actor);
  if (load<u8>(a + 0xdac) == 255)
    return;
  s32 index = s8(load<u8>(a + 0xdad) + load<u8>(a + 0xdae));
  u32 path = load<u32>(a + 0xdb0);
  store<u8>(a + 0xdad, index);
  if (load<u8>(path + 5) & 1) {
    u16 count = load<u16>(path);
    if (index > s32(s8(count)) - 1)
      store<u8>(a + 0xdad, 0);
    else if (index < 0)
      store<u8>(a + 0xdad, count - 1);
  } else {
    u16 count = load<u16>(path);
    if (index > s32(count) - 1) {
      path = load<u32>(a + 0xdb0);
      store<u8>(a + 0xdae, 255);
      count = load<u16>(path);
      store<u8>(a + 0xdad, count - 2);
    } else if (index < 0) {
      store<u8>(a + 0xdae, 1);
      store<u8>(a + 0xdad, 1);
    }
  }
  u32 y = load<u32>(a + 0xdb8), x = load<u32>(a + 0xdb4);
  store<u32>(a + 0xdc4, y);
  store<u32>(a + 0xdc0, x);
  u32 z = load<u32>(a + 0xdbc);
  s8 idx = load<s8>(a + 0xdad);
  path = load<u32>(a + 0xdb0);
  store<u32>(a + 0xdc8, z);
  u32 p = load<u32>(path + 8) + u32(s32(idx) * 16);
  store<f32>(a + 0xdb4, load<f32>(p + 4));
  store<f32>(a + 0xdb8, load<f32>(p + 8));
  store<f32>(a + 0xdbc, load<f32>(p + 12));
}
VERIFY(0x021B7C24, machine_next);
void machine_path(void *actor) {
  WWHD_FUNC(0x021B7D14, void, actor);
  u32 a = ea(actor);
  if (load<u8>(a + 0xdac) == 255)
    return;
  call(0x0200F62C, at<void>(a + 0x314), at<void>(a + 0xdb4),
       load<f32>(a + 0x370));
  Local<MachineVector> v;
  call(0x0201ADE0, at<void>(a + 0xdb4), v.get(), at<void>(a + 0x314));
  f32 sq = call<f32>(0x028E8DD0, v.get());
  f32 dist = call<f32>(0x028F4384, sq);
  if (dist < load<f32>(0x10014330))
    machine_next(actor);
}
VERIFY(0x021B7D14, machine_path);
s32 machine_isDelete(void *actor) {
  WWHD_FUNC(0x021B8260, s32, actor);
  return 1;
}
VERIFY(0x021B8260, machine_isDelete);

void machine_cube(void *actor) {
  WWHD_FUNC(0x021B7D8C, void, actor);
  u32 a = ea(actor);
  Local<MachineVector> near, far;
  far->z = load<f32>(0x10014308);
  far->x = load<f32>(0x10014320);
  near->x = load<f32>(0x10014320);
  near->y = load<f32>(0x10014320);
  far->y = load<f32>(0x10014320);
  near->z = load<f32>(0x10014334);
  call(0x028E93CC, at<void>(0x1048d0cc), load<f32>(a + 0x314),
       load<f32>(a + 0x318), load<f32>(a + 0x31c));
  call(0x025F1C28, at<void>(0x1048d0cc), s32(load<s16>(a + 0x322)));
  call(0x028E8F64, at<void>(0x1048d0cc), far.get(), far.get());
  call(0x028E8F64, at<void>(0x1048d0cc), near.get(), near.get());
  call(0x020181B0, at<void>(a + 0xcec), near.get(), far.get(),
       load<f32>(0x10014338));
}
VERIFY(0x021B7D8C, machine_cube);
void machine_attack(void *actor) {
  WWHD_FUNC(0x021B7E48, void, actor);
  u32 a = ea(actor), play = call<u32>(0x025200D4),
      player = load<u32>(play + 0x5b2c);
  if (!player)
    return;
  machine_cube(actor);
  Local<MachineVector> pos;
  pos->x = load<f32>(player + 0x314);
  f32 y = load<f32>(player + 0x318);
  pos->y = y;
  pos->z = load<f32>(player + 0x31c);
  pos->y = y + load<f32>(0x1001433c);
  call(0x02018D40, at<void>(a + 0xd0c), pos.get());
  call(0x02018C8C, at<void>(a + 0xd0c), load<f32>(0x10014340));
  u8 state = load<u8>(a + 0xe10);
  if (state == 0) {
    Local<be<f32>[3]> cross; /* cXyz: cM3d_Cross_CpsSph writes the 12-byte cross point (extent lint 2026-10-05) */
    if (call<u32>(0x020168D4, at<void>(a + 0xcec), at<void>(a + 0xd0c),
                  cross.get()))
      store<u8>(a + 0xe10, 1);
    return;
  }
  if (state == 1) {
    s32 reverb = call<s32>(0x02520540, s32(load<s8>(a + 0x326)));
    call(0x025E1A40, 0x69c5, at<void>(a + 0x37c), 0, reverb);
    u8 next = load<u8>(a + 0xe10) + 1;
    store<f32>(a + 0xd24, load<f32>(0x10014320));
    store<u8>(a + 0xe10, next);
    store<f32>(a + 0xd20, load<f32>(0x100142c4));
  } else if (state != 2)
    return;
  if (call<u32>(0x025E742C, at<void>(a + 0xd20)))
    store<u8>(a + 0xe10, 0);
}
VERIFY(0x021B7E48, machine_attack);
void machine_body(void *actor) {
  WWHD_FUNC(0x021B7F84, void, actor);
  u32 a = ea(actor);
  Local<MachineVector> v[3];
  f32 zero = load<f32>(0x10014320);
  v[0]->x = zero;
  v[1]->z = load<f32>(0x10014348);
  v[2]->y = zero;
  v[2]->z = load<f32>(0x1001434c);
  v[0]->y = zero;
  v[1]->y = zero;
  v[1]->x = zero;
  v[0]->z = load<f32>(0x10014344);
  v[2]->x = zero;
  call(0x028E93CC, at<void>(0x1048d0cc), load<f32>(a + 0x314),
       load<f32>(a + 0x318), load<f32>(a + 0x31c));
  call(0x025F1B48, at<void>(0x1048d0cc), s32(load<s16>(a + 0x320)),
       s32(load<s16>(a + 0x322)), s32(load<s16>(a + 0x324)));
  for (u32 i = 0; i < 3; i++) {
    u32 sph = a + 0x520 + i * 0x12c;
    call(0x028E8F64, at<void>(0x1048d0cc), v[i].get(), v[i].get());
    call(0x02018D40, at<void>(sph + 0x118), v[i].get());
    u32 play = call<u32>(0x025200D4);
    call(0x0200E240, at<void>(play + 0x26a4), at<void>(sph));
  }
  u32 flags = load<u32>(a + 0x520);
  store<u32>(a + 0x520, flags | 1);
  call(0x02018C8C, at<void>(a + 0x638), load<f32>(0x10014300));
}
VERIFY(0x021B7F84, machine_body);
void machine_at(void *actor) {
  WWHD_FUNC(0x021B8080, void, actor);
  u32 a = ea(actor);
  f32 frame = load<f32>(a + 0xd24);
  if (load<f32>(0x10014324) < frame && load<f32>(0x10014350) > frame) {
    f32 y = load<f32>(a + 0xdf0), z = load<f32>(a + 0xe00),
        x = load<f32>(a + 0xde0);
    store<f32>(a + 0xe08, y);
    store<f32>(a + 0xe0c, z);
    store<f32>(a + 0xe04, x);
    call(0x02018D40, at<void>(a + 0x50c), at<void>(a + 0xe04));
    u32 play = call<u32>(0x025200D4);
    call(0x0200E240, at<void>(play + 0x26a4), at<void>(a + 0x3f4));
  }
}
VERIFY(0x021B8080, machine_at);
s32 machine_execute(void *actor) {
  WWHD_FUNC(0x021B8104, s32, actor);
  u32 a = ea(actor);
  machine_speed(actor);
  Local<MachineVector> v;
  call(0x0201ADE0, at<void>(0x101ffba8), v.get(), at<void>(a + 0x314));
  s32 angle = call<s32>(0x020195B0, f32(v->x), f32(v->z));
  store<s16>(a + 0x322, angle);
  machine_path(actor);
  machine_attack(actor);
  machine_body(actor);
  machine_at(actor);
  machine_matrix(actor);
  if (load<f32>(a + 0x370) != load<f32>(0x10014320)) {
    s32 reverb = call<s32>(0x02520540, s32(load<s8>(a + 0x326)));
    call(0x025E1A40, 0x7045, at<void>(a + 0x37c), 0, reverb);
  }
  return 1;
}
VERIFY(0x021B8104, machine_execute);
s32 machine_executeCallback(void *actor) {
  WWHD_FUNC(0x021B81B4, s32, actor);
  return machine_execute(actor);
}
VERIFY(0x021B81B4, machine_executeCallback);

void machine_init(void *actor) {
  WWHD_FUNC(0x021B7600, void, actor);
  u32 a = ea(actor);
  f32 scale = load<f32>(0x100142f8);
  store<f32>(a + 0x334, scale);
  u32 model = load<u32>(a + 0x3b4);
  store<f32>(a + 0x338, scale);
  store<f32>(a + 0x330, scale);
  store<u32>(a + 0x348, model ? model + 0xc8 : 0);
  call(0x025D674C, actor, load<f32>(0x100142fc), load<f32>(0x100142fc),
       load<f32>(0x10014304), load<f32>(0x10014300), load<f32>(0x10014300),
       load<f32>(0x10014308));
  u32 status = a + 0x3b8;
  call(0x02515F14, at<void>(status), 255, 255, actor);
  call(0x0251677C, at<void>(a + 0x3f4), at<void>(0x101b91a8));
  store<u32>(a + 0x438, status);
  for (u32 i = 0; i < 3; i++) {
    u32 sph = a + 0x520 + i * 0x12c;
    call(0x0251677C, at<void>(sph), at<void>(0x101b91e8));
    store<u32>(sph + 0x44, status);
  }
  call(0x0251677C, at<void>(a + 0x8a4), at<void>(0x101b91e8));
  store<u32>(a + 0x8e8, status);
  call(0x024EFF3C, at<void>(a + 0xb94), load<f32>(0x1001430c));
  call(0x024F06B4, at<void>(a + 0x9d0), at<void>(a + 0x314),
       at<void>(a + 0x300), actor, 1, at<void>(a + 0xb94), at<void>(a + 0x33c),
       0, 0);
  machine_matrix(actor);
  model = load<u32>(a + 0x3b4);
  store<u32>(model + 0xb8, a);
  model = load<u32>(a + 0x3b4);
  u32 table = call<u32>(0x027F68FC, at<void>(load<u32>(model + 0xac)));
  u32 rel = load<u32>(table + 0x10), name = rel ? table + 0x10 + rel : 0;
  s32 index = call<s32>(0x027DF9B0, at<void>(name), at<void>(0x10014310));
  if (index >= 0) {
    model = load<u32>(a + 0x3b4);
    u32 data = load<u32>(model + 0xac), count = load<u32>(data + 4),
        joints = load<u32>(data + 8);
    u32 i = u16(index);
    if (i < count)
      joints += i * 28;
    store<u32>(joints + 8, 0x021b7494);
  }
  call(0x027F4D5C, at<void>(load<u32>(a + 0x3b4)));
  u8 id = load<u8>(a + 0xb3);
  store<u8>(a + 0xdac, id);
  if (id == 255)
    return;
  u32 path = call<u32>(0x025AAF88, u32(id), s32(load<s8>(a + 0x326)));
  store<u32>(a + 0xdb0, path);
  if (!path) {
    store<u8>(a + 0xdac, 255);
    return;
  }
  path = load<u32>(a + 0xdb0);
  store<u8>(a + 0xdae, 1);
  store<u8>(a + 0xdad, 1);
  u32 points = load<u32>(path + 8);
  // Preserve the recompiled original's immediate load/store NaN bits; cached
  // FPR values used later undergo the lfs conversion.
  u32 xb = load<u32>(points + 0x14);
  store<u32>(a + 0xdb4, xb);
  u32 yb = load<u32>(points + 0x18);
  store<u32>(a + 0xdb8, yb);
  u32 zb = load<u32>(points + 0x1c);
  store<f32>(a + 0xdc0, f32_from_bits(xb));
  store<u32>(a + 0xdbc, zb);
  store<f32>(a + 0xdc4, f32_from_bits(yb));
  store<f32>(a + 0xdc8, f32_from_bits(zb));
  points = load<u32>(path + 8);
  store<u32>(a + 0x314, load<u32>(points + 4));
  points = load<u32>(path + 8);
  store<u32>(a + 0x318, load<u32>(points + 8));
  points = load<u32>(path + 8);
  store<u32>(a + 0x31c, load<u32>(points + 12));
}
VERIFY(0x021B7600, machine_init);
s32 machine_create(void *actor) {
  WWHD_FUNC(0x021B7844, s32, actor);
  u32 a = ea(actor), flags = load<u32>(a + 0x2e4);
  if (!(flags & 8)) {
    if (a) {
      call(0x025D4ED0, actor);
      store<u32>(a + 0xb4, 0x100142b4);
      call(0x0200BD2C, at<void>(a + 0x3b8));
      call(0x02515DA0, at<void>(a + 0x3d4));
      store<u32>(a + 0x3d0, 0x1004ae88);
      store<u32>(a + 0x3d4, 0x1004aec0);
      call(0x025166F0, at<void>(a + 0x3f4));
      call(0x028EFFD0, at<void>(a + 0x520), 3, 0x12c, at<void>(0x025166f0));
      call(0x025166F0, at<void>(a + 0x8a4));
      call(0x024F0474, at<void>(a + 0x9d0));
      store<u32>(a + 0x9e0, 0x10014284);
      store<u32>(a + 0x9e4, 0x100142a4);
      store<u8>(a + 0x9e8, 1);
      store<u32>(a + 0x9f0, 0x10014294);
      call(0x024EFE94, at<void>(a + 0xb94));
      call(0x02515FB8, at<void>(a + 0xbd4));
      store<u32>(a + 0xce8, 0x100015a8);
      store<u32>(a + 0xce4, 0x1001424c);
      call(0x02018150, at<void>(a + 0xcec));
      store<u32>(a + 0xc10, 0x1004af18);
      store<u32>(a + 0xd04, 0x1004af60);
      store<u32>(a + 0xce8, 0x1004af70);
      call(0x02018C40, at<void>(a + 0xd0c));
      call(0x027F2BC0, at<void>(a + 0xd20), 0);
      store<u32>(a + 0xd30, 0x1016e54c);
      call(0x027DA984, at<void>(a + 0xd34));
      store<u32>(a + 0xd78, 0);
      store<u32>(a + 0xda4, 0);
      store<u32>(a + 0xda0, 0);
      store<u32>(a + 0xd30, 0x1001425c);
      store<u32>(a + 0xd9c, 0);
      store<u32>(a + 0xda8, 0);
      flags = load<u32>(a + 0x2e4);
      store<u32>(a + 0xd68, 0x1016d820);
    }
    store<u32>(a + 0x2e4, flags | 8);
  }
  s32 phase = call<s32>(0x02520460, at<void>(a + 0x3ac), at<void>(0x10014360));
  if (phase == 4) {
    if (!call<u32>(0x025D63E8, actor, at<void>(0x021b7490), 0xb00))
      return 5;
    machine_init(actor);
  }
  return phase;
}
VERIFY(0x021B7844, machine_create);
s32 machine_createCallback(void *actor) {
  WWHD_FUNC(0x021B7A38, s32, actor);
  return machine_create(actor);
}
VERIFY(0x021B7A38, machine_createCallback);
void machine_static() {
  WWHD_FUNC(0x021B81B8, void, u32(0));
  store<u32>(0x10464e9c, 0);
  store<u32>(0x10464e94, 0);
  store<u32>(0x10464ea0, 0);
  store<u32>(0x10464e98, 0);
  call(0x028F026C, at<void>(0x101b9248));
  f32 lo = load<f32>(0x10014358), hi = load<f32>(0x1001435c);
  store<f32>(0x10464e88, lo);
  store<f32>(0x10464e8c, hi);
  call(0x028ED6F8, at<void>(0x10464e90));
  call(0x028F026C, at<void>(0x101b9254));
  call(0x028EAB2C, at<void>(0x10464e91));
  call(0x028F026C, at<void>(0x101b9260));
}
VERIFY(0x021B81B8, machine_static);
void machine_nameDestructor(void *object, u32 flags) {
  WWHD_FUNC(0x021B824C, void, object, flags);
  if (ea(object) && (flags & 1))
    call(0x0273AF40, object);
}
VERIFY(0x021B824C, machine_nameDestructor);
void machine_destructor(void *actor, u32 flags) {
  WWHD_FUNC(0x021B8268, void, actor, flags);
  u32 a = ea(actor);
  if (!a)
    return;
  call(0x027F3628, at<void>(a + 0xd30), 0);
  call(0x02515980, at<void>(a + 0xbd4), 2);
  call(0x02018034, at<void>(a + 0xba8), 2);
  store<u32>(a + 0x9f0, 0x10014294);
  store<u32>(a + 0x9e4, 0x100142a4);
  call(0x024EFD9C, at<void>(a + 0x9d0), 0);
  call(0x02515AE8, at<void>(a + 0x8a4), 2);
  call(0x028F0164, at<void>(a + 0x520), 3, 0x12c, at<void>(0x02515ae8), 0, 0);
  call(0x02515AE8, at<void>(a + 0x3f4), 2);
  call(0x02515860, at<void>(a + 0x3b8), 2);
  call(0x025D50BC, actor, 0);
  if (flags & 1)
    call(0x0273AF40, actor);
}
VERIFY(0x021B8268, machine_destructor);
void machine_empty(void *object) { WWHD_FUNC(0x021B8348, void, object); }
VERIFY(0x021B8348, machine_empty);

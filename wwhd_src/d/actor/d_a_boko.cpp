#include "d/actor/d_a_boko.h"
namespace boko {
template <class T> struct Local {
  struct Frame {
    be<u32> linkage[2];
    T payload;
  };
  gabi::Local<Frame> storage;
  T *get() const { return gabi::at<T>(storage.a + 8); }
  T *operator->() const { return get(); }
};
template <class T> T read(u32 p, u32 off = 0) { return gabi::load<T>(p + off); }
template <class T> void write(u32 p, u32 off, T v) {
  gabi::store<T>(p + off, v);
}
void *ptr(u32 a) { return gabi::at<void>(a); }
constexpr u32 Matrix = 0x1048D0CC;
} // namespace boko
using namespace boko;
BOOL boko_no_draw(daBoko_c *a) {
  WWHD_FUNC(0x020C0BFC, BOOL, a);
  if (a->hidden || a->mode == 4)
    return 1;
  if (a->mode == 3) {
    u32 p = gabi::call<u32>(0x025200D4);
    if (read<u32>(p, 0x5CD8) & 0x80)
      return 1;
  }
  return 0;
}
VERIFY(0x020C0BFC, boko_no_draw);
void boko_line_draw(daBoko_c *a) {
  WWHD_FUNC(0x020C0C5C, void, a);
  gabi::call<void>(0x025EA548, a->line.get(), 10, ptr(0x10191F00), 2,
                   &a->tevStr, 1.25f);
  u32 line = gabi::ea(a->line.get()), p = gabi::call<u32>(0x025200D4);
  u32 table = read<u32>(line, 0x130), target = read<u32>(table, 0x14);
  s32 index = gabi::call<s32>(target, ptr(line));
  gabi::call<void>(0x025EDD04, ptr(p + 0x5FB4 + u32(index) * 0x9C), ptr(line));
}
VERIFY(0x020C0C5C, boko_line_draw);
BOOL boko_draw(daBoko_c *a) {
  WWHD_FUNC(0x020C0CE0, BOOL, a);
  u32 p = gabi::call<u32>(0x025200D4), player = read<u32>(p, 0x5B2C);
  BOOL hidden = boko_no_draw(a);
  u32 emitter = read<u32>(gabi::ea(a), 0x4B0);
  if (hidden) {
    if (emitter)
      write<u32>(emitter, 0x254, read<u32>(emitter, 0x254) | 4);
    return 1;
  }
  if (emitter)
    write<u32>(emitter, 0x254, read<u32>(emitter, 0x254) & ~4u);
  if (a->mode == 3) {
    u32 target = read<u32>(read<u32>(player, 0xB4), 0xCC);
    if (gabi::call<BOOL>(target, ptr(player)))
      return 1;
  }
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x025626A4, ptr(env), 0, &a->current.pos, &a->tevStr);
  env = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x02562F5C, ptr(env), a->model.get(), &a->tevStr);
  if (a->mParameters == 5) {
    u32 model = gabi::ea(a->model.get());
    f32 frame = read<f32>(gabi::ea(a), 0x3BC);
    gabi::call<void>(0x025E83FC, ptr(gabi::ea(a) + 0x3B8),
                     ptr(read<u32>(model, 0xAC)), frame);
  }
  gabi::call<void>(0x025E2E5C, a->model.get());
  if (a->mParameters == 5) {
    u32 model = gabi::ea(a->model.get());
    write<u32>(read<u32>(model, 0xAC), 0x48, 0);
  }
  if (a->line.get())
    boko_line_draw(a);
  return 1;
}
VERIFY(0x020C0CE0, boko_draw);
BOOL boko_draw_wrapper(daBoko_c *a) {
  WWHD_FUNC(0x020C0E04, BOOL, a);
  return boko_draw(a);
}
VERIFY(0x020C0E04, boko_draw_wrapper);
void boko_top_root(daBoko_c *a, s32 usePosition) {
  WWHD_FUNC(0x020C0E08, void, a, usePosition);
  u32 type = a->mParameters;
  if (usePosition) {
    f32 y = f32(a->current.pos.y) + read<f32>(0x1000A488), x = a->current.pos.x,
        z = a->current.pos.z;
    gabi::call<void>(0x028E93CC, ptr(Matrix), x, y, z);
    s16 rx = a->shape_angle.x, ry = a->shape_angle.y, rz = a->shape_angle.z;
    gabi::call<void>(0x025F1B48, ptr(Matrix), rx, ry, rz);
  } else {
    u32 model = gabi::ea(a->model.get());
    gabi::call<void>(0x028E90D4, ptr(model ? model + 0xC8 : 0), ptr(Matrix));
  }
  gabi::call<void>(0x028E8F64, ptr(Matrix), ptr(0x10192198 + type * 12),
                   &a->top);
  gabi::call<void>(0x028E8F64, ptr(Matrix), ptr(0x10192228 + type * 12),
                   &a->root);
}
VERIFY(0x020C0E08, boko_top_root);
BOOL boko_is_delete(daBoko_c *a) {
  WWHD_FUNC(0x020C1D10, BOOL, a);
  return 1;
}
VERIFY(0x020C1D10, boko_is_delete);
BOOL boko_delete(daBoko_c *a) {
  WWHD_FUNC(0x020C1D18, BOOL, a);
  gabi::call<void>(0x025E1B34, &a->top);
  u32 table = read<u32>(gabi::ea(a), 0x4AC), target = read<u32>(table, 0x44);
  gabi::call<void>(target, ptr(gabi::ea(a) + 0x4AC));
  gabi::call<void>(0x0255A374, ptr(gabi::ea(a) + 0x47C));
  u32 type = a->mParameters, name = read<u32>(0x1019204C + type * 4);
  gabi::call<void>(0x025204C8, ptr(gabi::ea(a) + 0x3AC), ptr(name));
  return 1;
}
VERIFY(0x020C1D18, boko_delete);
BOOL boko_delete_wrapper(daBoko_c *a) {
  WWHD_FUNC(0x020C1D84, BOOL, a);
  boko_delete(a);
  return 1;
}
VERIFY(0x020C1D84, boko_delete_wrapper);
BOOL boko_wait_init(daBoko_c *a) {
  WWHD_FUNC(0x020C25A8, BOOL, a);
  u32 flags = read<u32>(gabi::ea(a), 0x39C);
  f32 zero = read<f32>(0x1000A4B8);
  a->procedure = 0x020C3964;
  a->mode = 0;
  write<u32>(gabi::ea(a), 0x39C, flags | 0x10);
  a->rotationSpeed = 0;
  a->angleSpeed = 0;
  a->speedF = zero;
  a->procedureSlot = -1;
  a->procedureDelta = 0;
  gabi::call<void>(0x025D9D24, a);
  a->gravity = read<f32>(0x1000A548);
  return 1;
}
VERIFY(0x020C25A8, boko_wait_init);
BOOL boko_move_init(daBoko_c *a) {
  WWHD_FUNC(0x020C2624, BOOL, a);
  a->procedureDelta = 0;
  a->mode = 1;
  a->procedure = 0x020C2C90;
  a->procedureSlot = -1;
  gabi::call<void>(0x025D9D0C, a, 0);
  write<u32>(gabi::ea(a), 0x39C, read<u32>(gabi::ea(a), 0x39C) & ~0x10u);
  s16 count = read<s16>(0x1047BC2E);
  a->floorFlag = 0;
  a->moveCounter = u8(s32(count) + 5);
  return 1;
}
VERIFY(0x020C2624, boko_move_init);
daBoko_c *boko_construct(daBoko_c *a) {
  WWHD_FUNC(0x020C1F58, daBoko_c *, a);
  if (!a) {
    a = gabi::call<daBoko_c *>(0x0273AD10, 0xA34);
    if (!a)
      return a;
  }
  u32 p = gabi::ea(a);
  gabi::call<void>(0x025D4ED0, a);
  write<u32>(p, 0xB4, 0x1000A478);
  gabi::call<void>(0x025E80D0, ptr(p + 0x3B8));
  write<f32>(p, 0x49C, read<f32>(0x1000A48C));
  gabi::call<void>(0x025A5894, ptr(p + 0x4AC), 0, 0);
  gabi::call<void>(0x024F0474, ptr(p + 0x4C0));
  write<u32>(p, 0x4E0, 0x1000A3D8);
  write<u32>(p, 0x4D0, 0x1000A3C8);
  write<u8>(p, 0x4D8, 1);
  write<u32>(p, 0x4D4, 0x1000A3E8);
  gabi::call<void>(0x024EFE94, ptr(p + 0x684));
  gabi::call<void>(0x0200BD2C, ptr(p + 0x6C4));
  gabi::call<void>(0x02515DA0, ptr(p + 0x6E0));
  write<u32>(p, 0x6DC, 0x1004AE88);
  write<u32>(p, 0x6E0, 0x1004AEC0);
  gabi::call<void>(0x025166F0, ptr(p + 0x700));
  gabi::call<void>(0x02515FB8, ptr(p + 0x82C));
  write<u32>(p, 0x940, 0x100015A8);
  write<u32>(p, 0x93C, 0x1000A318);
  gabi::call<void>(0x02018150, ptr(p + 0x944));
  write<u32>(p, 0x868, 0x1004AF18);
  write<u32>(p, 0x95C, 0x1004AF60);
  write<u32>(p, 0x940, 0x1004AF70);
  return a;
}
VERIFY(0x020C1F58, boko_construct);
void boko_global_delete(void *p, s32 flags) {
  WWHD_FUNC(0x020C3ED4, void, p, flags);
  if (p && (flags & 1))
    gabi::call<void>(0x0273AF40, p);
}
VERIFY(0x020C3ED4, boko_global_delete);
void boko_ground_delete(void *object, s32 flags) {
  WWHD_FUNC(0x020C3EE8, void, object, flags);
  if (!object)
    return;
  u32 p = gabi::ea(object);
  write<u32>(p, 0x20, 0x1000A358);
  write<u32>(p, 0x40, 0x1000A378);
  write<u32>(p, 0x4C, 0x1000A338);
  gabi::call<void>(0x02008DAC, object, 0);
  if (flags & 1)
    gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x020C3EE8, boko_ground_delete);
void boko_line_delete(void *object, s32 flags) {
  WWHD_FUNC(0x020C3F60, void, object, flags);
  if (!object)
    return;
  u32 p = gabi::ea(object);
  write<u32>(p, 0x58, 0x1000A428);
  write<u32>(p, 0x64, 0x1000A338);
  write<u32>(p, 0x20, 0x1000A328);
  gabi::call<void>(0x02008B4C, object, 0);
  if (flags & 1)
    gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x020C3F60, boko_line_delete);
void boko_destruct(daBoko_c *a, s32 flags) {
  WWHD_FUNC(0x020C3FD8, void, a, flags);
  if (!a)
    return;
  u32 p = gabi::ea(a);
  gabi::call<void>(0x02515980, ptr(p + 0x82C), 2);
  gabi::call<void>(0x02515AE8, ptr(p + 0x700), 2);
  gabi::call<void>(0x02515860, ptr(p + 0x6C4), 2);
  gabi::call<void>(0x02018034, ptr(p + 0x698), 2);
  write<u32>(p, 0x4E0, 0x1000A3D8);
  write<u32>(p, 0x4D4, 0x1000A3E8);
  gabi::call<void>(0x024EFD9C, ptr(p + 0x4C0), 0);
  gabi::call<void>(0x025D50BC, a, 0);
  if (flags & 1)
    gabi::call<void>(0x0273AF40, a);
}
VERIFY(0x020C3FD8, boko_destruct);
void boko_empty(void *object) { WWHD_FUNC(0x020C4080, void, object); }
VERIFY(0x020C4080, boko_empty);
void boko_get_top(daBoko_c *a, cXyz *out) {
  WWHD_FUNC(0x020C4084, void, a, out);
  u32 model = gabi::ea(a->model.get()), table = 0x10192198;
  if (model) {
    u32 type = a->mParameters;
    gabi::call<void>(0x028E8F64, ptr(model + 0xC8), ptr(table + type * 12),
                     out);
  } else {
    u32 type = a->mParameters;
    Local<cXyz> pos;
    gabi::call<void>(0x0201AD78, &a->current.pos, pos.get(),
                     ptr(table + type * 12));
    u32 x = read<u32>(gabi::ea(pos.get())),
        z = read<u32>(gabi::ea(pos.get()), 8);
    write<u32>(gabi::ea(out), 0, x);
    u32 y = read<u32>(gabi::ea(pos.get()), 4);
    write<u32>(gabi::ea(out), 8, z);
    write<u32>(gabi::ea(out), 4, y);
  }
}
VERIFY(0x020C4084, boko_get_top);
void boko_get_blur(daBoko_c *a, cXyz *out) {
  WWHD_FUNC(0x020C4128, void, a, out);
  u32 model = gabi::ea(a->model.get()), table = 0x101921E0;
  if (model) {
    u32 type = a->mParameters;
    gabi::call<void>(0x028E8F64, ptr(model + 0xC8), ptr(table + type * 12),
                     out);
  } else {
    u32 type = a->mParameters;
    Local<cXyz> pos;
    gabi::call<void>(0x0201AD78, &a->current.pos, pos.get(),
                     ptr(table + type * 12));
    u32 x = read<u32>(gabi::ea(pos.get())),
        z = read<u32>(gabi::ea(pos.get()), 8);
    write<u32>(gabi::ea(out), 0, x);
    u32 y = read<u32>(gabi::ea(pos.get()), 4);
    write<u32>(gabi::ea(out), 8, z);
    write<u32>(gabi::ea(out), 4, y);
  }
}
VERIFY(0x020C4128, boko_get_blur);
void boko_static_tail() {
  WWHD_FUNC(0x020C41CC, void);
  write<u32>(0x10462664, 8, 0);
  write<u32>(0x10462664, 0, 0);
  write<u32>(0x10462664, 12, 0);
  write<u32>(0x10462664, 4, 0);
  gabi::call<void>(0x028F026C, ptr(0x101920F4));
  f32 lower = read<f32>(0x1000A5C4), upper = read<f32>(0x1000A5C8);
  write<f32>(0x10462658, 0, lower);
  write<f32>(0x1046265C, 0, upper);
  gabi::call<void>(0x028ED6F8, ptr(0x10462660));
  gabi::call<void>(0x028F026C, ptr(0x10192100));
  gabi::call<void>(0x028EAB2C, ptr(0x10462661));
  gabi::call<void>(0x028F026C, ptr(0x1019210C));
}
VERIFY(0x020C41CC, boko_static_tail);
void boko_base_matrix(daBoko_c *a) {
  WWHD_FUNC(0x020C2088, void, a);
  f32 y = f32(a->current.pos.y) + read<f32>(0x1000A488), x = a->current.pos.x,
      z = a->current.pos.z;
  gabi::call<void>(0x028E93CC, ptr(Matrix), x, y, z);
  s16 rx = a->shape_angle.x, ry = a->shape_angle.y, rz = a->shape_angle.z;
  gabi::call<void>(0x025F1B48, ptr(Matrix), rx, ry, rz);
  if (a->mParameters == 2) {
    gabi::call<void>(0x025F1BF4, ptr(Matrix), -1000);
    f32 zero = read<f32>(0x1000A4B8), height = read<f32>(0x1000A4EC);
    gabi::call<void>(0x025F24E0, zero, height, zero);
  }
  // Load all values before stores, matching the overlapping matrix-copy
  // semantics.
  f32 values[12];
  for (u32 i = 0; i < 12; i++)
    values[i] = read<f32>(Matrix, i * 4);
  u32 model = gabi::ea(a->model.get());
  for (u32 i = 0; i < 12; i++)
    write<f32>(model, 0xC8 + i * 4, values[i]);
}
VERIFY(0x020C2088, boko_base_matrix);
void boko_static_init() {
  WWHD_FUNC(0x020C3D0C, void);
  write<u32>(0x1046257C, 4, 0);
  write<u32>(0x1046257C, 12, 0);
  write<u32>(0x1046257C, 0, 0);
  write<u32>(0x1046257C, 8, 0);
  gabi::call<void>(0x028F026C, ptr(0x10191FB0));
  f32 upper = read<f32>(0x1000A5AC), lower = read<f32>(0x1000A5A8);
  write<f32>(0x10462574, 0, upper);
  write<f32>(0x10462570, 0, lower);
  gabi::call<void>(0x028ED6F8, ptr(0x10462578));
  gabi::call<void>(0x028F026C, ptr(0x10191FBC));
  gabi::call<void>(0x028EAB2C, ptr(0x10462579));
  gabi::call<void>(0x028F026C, ptr(0x10191FC8));
  f32 zero = read<f32>(0x1000A4B8), distance = read<f32>(0x1000A550);
  write<f32>(0x1046258C, 0, zero);
  write<f32>(0x1046258C, 8, distance);
  write<f32>(0x1046258C, 4, zero);
  u32 ground = 0x10462598;
  gabi::call<void>(0x02008E0C, ptr(ground));
  write<u32>(ground, 0, ground + 0x40);
  write<u32>(ground, 4, ground + 0x4C);
  write<u32>(ground, 0x50, 1);
  write<u8>(ground, 0x45, 0);
  write<u32>(ground, 0x10, 0x1000A388);
  write<u8>(ground, 0x46, 0);
  write<u8>(ground, 0x47, 0);
  write<u8>(ground, 0x48, 0);
  write<u8>(ground, 0x49, 0);
  write<u32>(ground, 0x20, 0x1000A398);
  write<u8>(ground, 0x4A, 0);
  write<u32>(ground, 0x4C, 0x1000A3A8);
  write<u32>(ground, 0x40, 0x1000A3B8);
  write<u8>(ground, 0x44, 1);
  gabi::call<void>(0x028F026C, ptr(0x10191FD4));
  u32 line = 0x104625EC;
  gabi::call<void>(0x02008FEC, ptr(line));
  write<u8>(line, 0x5D, 0);
  write<u8>(line, 0x5E, 0);
  write<u8>(line, 0x5F, 0);
  write<u32>(line, 0, line + 0x58);
  write<u8>(line, 0x60, 0);
  write<u8>(line, 0x61, 0);
  write<u8>(line, 0x62, 0);
  write<u32>(line, 4, line + 0x64);
  write<u32>(line, 0x10, 0x1000A438);
  write<u32>(line, 0x20, 0x1000A448);
  write<u32>(line, 0x68, 1);
  write<u8>(line, 0x5C, 1);
  write<u32>(line, 0x64, 0x1000A458);
  write<u32>(line, 0x58, 0x1000A468);
  gabi::call<void>(0x028F026C, ptr(0x10191FE0));
}
VERIFY(0x020C3D0C, boko_static_init);
BOOL boko_heap(daBoko_c *a) {
  WWHD_FUNC(0x020C1DA8, BOOL, a);
  u32 type = a->mParameters;
  if (type >= 6)
    gabi::call<void>(0x0273AA24, ptr(0x1000A51C), 0x84E, ptr(0x1000A4F8));
  struct Name {
    be<u32> string, vtable;
  };
  Local<Name> name;
  u32 controller = read<u32>(0x101F4F28),
      index = read<u32>(0x1000A504 + type * 4),
      text = read<u32>(0x1019204C + type * 4);
  name->vtable = 0x1000A300;
  name->string = text;
  void *data =
      gabi::call<void *>(0x026066C4, ptr(controller), name.get(), index);
  if (!data)
    gabi::call<void>(0x0273AA24, ptr(0x1000A51C), 0x854, ptr(0x1000A52C));
  void *model = gabi::call<void *>(0x025E38E0, data, 0x80000, 0x11000002);
  a->model = model;
  if (!model)
    return 0;
  if (type == 4) {
    void *line = gabi::call<void *>(0x0273AD10, 0x138C);
    if (line)
      gabi::call<void>(0x025E9960, line);
    a->line = line;
    if (!line)
      return 0;
    if (!gabi::call<BOOL>(0x025E9B80, line, 16, 10, 0))
      return 0;
    f32 rate = read<f32>(0x1047BBCC) + read<f32>(0x1000A4E0);
    write<f32>(gabi::ea(a->line.get()), 0x1388, rate);
    return 1;
  }
  if (type == 5) {
    Local<Name> animation;
    animation->vtable = 0x1000A300;
    controller = read<u32>(0x101F4F28);
    animation->string = 0x1000A4FC;
    void *resource =
        gabi::call<void *>(0x026066C4, ptr(controller), animation.get(), 7);
    f32 speed = read<f32>(0x1000A48C);
    if (!gabi::call<BOOL>(0x025E8154, ptr(gabi::ea(a) + 0x3B8), data, resource,
                          1, 0, 0, -1, 0, 0, speed))
      return 0;
  }
  return 1;
}
VERIFY(0x020C1DA8, boko_heap);
BOOL boko_heap_thunk(daBoko_c *a) {
  WWHD_FUNC(0x020C1F54, BOOL, a);
  return boko_heap(a);
}
VERIFY(0x020C1F54, boko_heap_thunk);
void boko_room(daBoko_c *a) {
  WWHD_FUNC(0x020C28E8, void, a);
  u32 actor = gabi::ea(a);
  u8 room;
  if (read<f32>(actor, 0x554) != read<f32>(0x1000A570)) {
    u32 p = gabi::call<u32>(0x025200D4);
    room = gabi::call<u8>(0x024EF130, ptr(p + 0x12A0), ptr(actor + 0x5A8));
    p = gabi::call<u32>(0x025200D4);
    u8 color = gabi::call<u8>(0x024EEEB8, ptr(p + 0x12A0), ptr(actor + 0x5A8));
    write<u8>(actor, 0x326, room);
    write<u8>(actor, 0x6E6, room);
    write<u8>(actor, 0x1CA, color);
    write<u8>(actor, 0x1C9, room);
  } else {
    room = read<u8>(0x1047E6C8);
    write<u8>(actor, 0x6E6, room);
    write<u8>(actor, 0x1C9, room);
    write<u8>(actor, 0x326, room);
  }
}
VERIFY(0x020C28E8, boko_room);
void boko_throw_reverse(daBoko_c *a, s16 direction) {
  WWHD_FUNC(0x020C26AC, void, a, direction);
  u32 type = a->mParameters;
  s8 room = a->current.roomNo;
  u32 sound = read<u32>(0x1019201C + type * 4);
  s32 reverb = gabi::call<s32>(0x02520540, s32(room));
  gabi::call<void>(0x025E1A40, sound, &a->eyePos, 100, reverb);
  boko_move_init(a);
  a->rotationSpeed = a->shape_angle.x < 0 ? 0x7FF : -0x7FF;
  type = a->mParameters;
  a->angleSpeed = 0;
  a->gravity = read<f32>(0x1000A548);
  f32 spin;
  if (type == 4) {
    f32 speed = a->speedF,
        bounce = read<f32>(0x1047BBD4) + read<f32>(0x1000A558),
        rate = read<f32>(0x1000A564);
    spin = read<f32>(0x1000A55C);
    a->speed.y = bounce;
    a->speedF = speed * rate;
  } else {
    f32 zero = read<f32>(0x1000A4B8), rate = read<f32>(0x1000A564);
    a->speed.y = zero;
    f32 speed = a->speedF;
    spin = read<f32>(0x1000A560);
    a->speedF = speed * rate;
  }
  f32 range = read<f32>(0x1000A568),
      random = gabi::call<f32>(0x02019918, range);
  s16 yaw = s16(gabi::ftoi(f32(s32(direction)) + random));
  a->current.angle.y = yaw;
  s16 shape = a->shape_angle.y;
  s32 distance = gabi::call<s32>(0x0200FAAC, yaw, shape);
  range = read<f32>(0x1000A56C);
  random = gabi::call<f32>(0x02019918, range);
  if (distance < 0x4000) {
    s16 rotation = s16(gabi::ftoi(spin + random));
    a->shape_angle.y = a->current.angle.y;
    a->rotationSpeed = rotation;
  } else {
    f32 value = spin + random;
    s16 angle = a->current.angle.y, rotation = s16(gabi::ftoi(-value));
    a->shape_angle.y = s16(u16(angle) + 0x8000);
    a->rotationSpeed = rotation;
  }
}
VERIFY(0x020C26AC, boko_throw_reverse);
s32 boko_create(daBoko_c *a) {
  WWHD_FUNC(0x020C2188, s32, a);
  u32 actor = gabi::ea(a), condition = read<u32>(actor, 0x2E4);
  if (!(condition & 8)) {
    if (a) {
      boko_construct(a);
      condition = read<u32>(actor, 0x2E4);
    }
    write<u32>(actor, 0x2E4, condition | 8);
  }
  u32 type = a->mParameters, offset;
  u32 name;
  if (type == 7) {
    a->mParameters = 0;
    write<u8>(actor, 0x2DB, 21);
    a->flameTimer = 900;
    type = 0;
    offset = 0;
    name = read<u32>(0x1019204C);
  } else {
    if (type == 8) {
      a->mParameters = 0;
      s16 delay = read<s16>(0x1047BC32);
      write<s16>(actor, 0x446, s16(delay + 10));
      f32 speed = read<f32>(0x1047BBC4) + read<f32>(0x1000A540);
      type = 0;
      a->speedF = speed;
    }
    offset = type * 4;
    name = read<u32>(0x1019204C + offset);
  }
  s32 phase = gabi::call<s32>(0x02520460, ptr(actor + 0x3AC), ptr(name));
  if (phase != 4)
    return phase;
  if (type >= 6 ||
      !gabi::call<BOOL>(0x025D63E8, a, ptr(0x020C1F54),
                        read<u32>(0x10192034 + offset)) ||
      !a->model.get())
    return 5;
  u32 model = gabi::ea(a->model.get());
  f32 y = a->scale.y, x = a->scale.x, z = a->scale.z;
  write<f32>(model, 0xBC, x);
  write<f32>(model, 0xC0, y);
  write<f32>(model, 0xC4, z);
  model = gabi::ea(a->model.get());
  write<u32>(actor, 0x348, model ? model + 0xC8 : 0);
  offset = type * 12;
  x = read<f32>(0x10192064 + offset);
  y = read<f32>(0x10192068 + offset);
  z = read<f32>(0x1019206C + offset);
  gabi::call<void>(0x025D672C, a, x, y, z);
  x = read<f32>(0x101920AC + offset);
  y = read<f32>(0x101920B0 + offset);
  z = read<f32>(0x101920B4 + offset);
  gabi::call<void>(0x025D673C, a, x, y, z);
  f32 height = read<f32>(0x1000A4EC), radius = read<f32>(0x1000A544);
  gabi::call<void>(0x024EFF44, ptr(actor + 0x684), height, radius);
  gabi::call<void>(0x024F06B4, ptr(actor + 0x4C0), ptr(actor + 0x314),
                   ptr(actor + 0x300), a, 1, ptr(actor + 0x684),
                   ptr(actor + 0x33C), ptr(actor + 0x320), ptr(actor + 0x328));
  u32 flags = read<u32>(actor, 0x39C);
  write<f32>(actor, 0x378, read<f32>(0x1000A54C));
  write<u8>(actor, 0x38C, 17);
  write<u32>(actor, 0x39C, flags | 0x10);
  a->gravity = read<f32>(0x1000A548);
  gabi::call<void>(0x02515F14, ptr(actor + 0x6C4), 10, 255, a);
  if (type == 0) {
    model = gabi::ea(a->model.get());
    u32 data = read<u32>(model, 0xAC), count = read<u32>(data, 4),
        joints = read<u32>(data, 8);
    if (count > 2)
      joints += 0x38;
    u32 joint = read<u32>(joints, 0x10), info = read<u32>(joint, 8);
    write<u8>(info, 4, 0);
  }
  gabi::call<void>(0x0251677C, ptr(actor + 0x700), ptr(0x10191F24));
  write<u32>(actor, 0x744, actor + 0x6C4);
  gabi::call<void>(0x025164C0, ptr(actor + 0x82C), ptr(0x10191F64));
  u32 currentType = a->mParameters, table = currentType * 4;
  write<u32>(actor, 0x870, actor + 0x6C4);
  u32 attack = read<u32>(0x10192180 + table);
  write<u32>(actor, 0x83C, attack);
  write<u8>(actor, 0x840, u8(read<u32>(0x10192138 + table)));
  write<u8>(actor, 0x898, read<u8>(0x10192118 + currentType));
  f32 capsuleRadius = read<f32>(0x10192120 + table);
  a->procedureDelta = 0;
  a->procedureSlot = -1;
  a->procedure = 0x020C3964;
  write<f32>(actor, 0x960, capsuleRadius);
  boko_base_matrix(a);
  boko_top_root(a, 0);
  f32 scale = read<f32>(0x1000A48C);
  if (a->flameTimer) {
    write<f32>(actor, 0x964, scale);
    gabi::call<void>(0x020C0EF8, a);
  }
  write<u32>(actor, 0x4E8, read<u32>(actor, 0x4E8) & ~8u);
  write<f32>(actor, 0x580, read<f32>(0x1000A550));
  write<u32>(0x10462638, 0, read<u32>(0x10462638) & ~0x20000000u);
  gabi::call<void>(0x027F4D5C, a->model.get());
  if (type == 0) {
    gabi::call<void>(0x025564B4, ptr(actor + 0x47C));
    write<s16>(actor, 0x488, 600);
    write<s16>(actor, 0x48A, 400);
    write<s16>(actor, 0x48C, 120);
    u32 p = gabi::call<u32>(0x025200D4), tablePointer = read<u32>(p, 0x5150),
        target = read<u32>(tablePointer, 0x15C);
    u32 stage = gabi::call<u32>(target, ptr(p + 0x5150));
    if (stage) {
      p = gabi::call<u32>(0x025200D4);
      tablePointer = read<u32>(p, 0x5150);
      target = read<u32>(tablePointer, 0x15C);
      stage = gabi::call<u32>(target, ptr(p + 0x5150));
      u32 kind = (read<u32>(stage, 12) >> 16) & 7;
      if (kind == 1 || kind == 4)
        scale = read<f32>(0x1000A4F0);
    }
    f32 power = read<f32>(0x1000A4F4) * scale, rate = read<f32>(actor, 0x964),
        distance = read<f32>(0x1000A554);
    write<f32>(actor, 0x494, distance);
    write<f32>(actor, 0x490, power * rate);
  }
  u32 py = read<u32>(actor, 0x318), modelPointer = read<u32>(actor, 0x3B4);
  write<u32>(actor, 0x474, py);
  write<u32>(actor, 0x368, modelPointer);
  u32 px = read<u32>(actor, 0x314), pz = read<u32>(actor, 0x31C);
  write<u32>(actor, 0x470, px);
  write<u32>(actor, 0x478, pz);
  return phase;
}
VERIFY(0x020C2188, boko_create);
s32 boko_create_wrapper(daBoko_c *a) {
  WWHD_FUNC(0x020C25A4, s32, a);
  return boko_create(a);
}
VERIFY(0x020C25A4, boko_create_wrapper);
void boko_flame(daBoko_c *a) {
  WWHD_FUNC(0x020C0EF8, void, a);
  u32 actor = gabi::ea(a), data = gabi::call<u32>(0x025C44AC);
  f32 one = read<f32>(0x1000A48C);
  if (data) {
    f32 step = read<f32>(0x1000A490), randomRange = read<f32>(0x1000A498),
        rate = read<f32>(0x1000A494);
    for (u32 i = 0; i < 4; i++) {
      f32 random = gabi::call<f32>(0x020198D8, randomRange);
      gabi::call<void>(0x0200ED84, ptr(actor + 0x964), random + one, rate,
                       step);
      s16 factor = read<s16>(0x1000A4B0 + i * 2);
      u32 count = read<u32>(0x101FF560);
      f32 radius = read<f32>(data, 0x10 + i * 4),
          scale = read<f32>(actor, 0x964);
      s16 angle = s16(count * u32(s32(factor)));
      f32 size = radius * scale;
      f32 x = a->top.x, z = a->top.z, y = a->top.y;
      gabi::call<void>(0x028E93CC, ptr(Matrix), x, y, z);
      gabi::call<void>(0x025F1B48, ptr(Matrix), 0, angle, angle);
      gabi::call<void>(0x025F2518, size, size, size);
      gabi::call<void>(0x028E90D4, ptr(Matrix), ptr(actor + 0x968 + i * 48));
    }
  }
  u32 emitter = read<u32>(actor, 0x4B0);
  if (!emitter) {
    u32 p = gabi::call<u32>(0x025200D4), manager = read<u32>(p, 0x5AB0);
    gabi::call<void>(0x025A847C, ptr(manager), 0, 0x1EA, &a->top, 0, &a->scale,
                     255, ptr(actor + 0x4AC), -1, 0, 0, 0);
    u32 y = read<u32>(actor, 0x450), z = read<u32>(actor, 0x454);
    write<u32>(actor, 0x4A4, y);
    write<u32>(actor, 0x4A8, z);
    u32 x = read<u32>(actor, 0x44C);
    emitter = read<u32>(actor, 0x4B0);
    write<u32>(actor, 0x4A0, x);
  }
  if (emitter) {
    f32 rate = read<f32>(0x1047B620) + read<f32>(0x1000A49C);
    f32 x = (read<f32>(actor, 0x44C) - read<f32>(actor, 0x4A0)) * rate,
        low = read<f32>(0x1000A4A0);
    if (x > one)
      x = one;
    else if (x < low)
      x = low;
    f32 z = (read<f32>(actor, 0x454) - read<f32>(actor, 0x4A8)) * rate;
    if (z > one)
      z = one;
    else if (z < low)
      z = low;
    write<f32>(emitter, 0x30, z);
    write<f32>(emitter, 0x28, x);
    write<f32>(emitter, 0x2C, read<f32>(0x1000A4A4));
    Local<cXyz> motion;
    gabi::call<void>(0x0201ADE0, &a->current.pos, motion.get(),
                     ptr(actor + 0x300));
    f32 my = motion->y, mx = motion->x, mz = motion->z;
    f32 square = gabi::fmadds(mz, mz, gabi::fmadds(mx, mx, my * my)),
        distance = gabi::call<f32>(0x028F4384, square);
    f32 growth = read<f32>(0x1047B640) + read<f32>(0x1000A4A8),
        limit = read<f32>(0x1047B644) + read<f32>(0x1000A4AC),
        scale = gabi::fmadds(distance, growth, one);
    if (scale > limit)
      scale = limit;
    write<f32>(emitter, 0x240, one);
    write<f32>(emitter, 0x238, one);
    write<f32>(emitter, 0x23C, scale);
    u32 y = read<u32>(actor, 0x450), xword = read<u32>(actor, 0x44C);
    write<u32>(actor, 0x4A4, y);
    s8 room = read<s8>(actor, 0x326);
    u32 zword = read<u32>(actor, 0x454);
    write<u32>(actor, 0x4A0, xword);
    write<u32>(actor, 0x4A8, zword);
    s32 reverb = gabi::call<s32>(0x02520540, s32(room));
    gabi::call<void>(0x025E1A40, 0x6103, &a->top, 0, reverb);
  }
  if (!boko_no_draw(a)) {
    u32 p = gabi::call<u32>(0x025200D4), manager = read<u32>(p, 0x5AB0);
    gabi::call<void>(0x025A8D40, ptr(manager), 0x4004, &a->top, 255,
                     ptr(0x101D5E98), ptr(0x101D5E98), 0);
  }
}
VERIFY(0x020C0EF8, boko_flame);

void boko_chain_calc(daBoko_c *a) {
  WWHD_FUNC(0x020C1684, void, a);
  u32 actor = gabi::ea(a), line = gabi::ea(a->line.get()), chain = line + 0x148;
  f32 goal = (read<u32>(actor, 0x2E0) & 0x2000)
                 ? read<f32>(0x1047BBCC) + read<f32>(0x1000A4E0)
                 : read<f32>(0x1000A4B8);
  gabi::call<void>(0x0200F5C8, ptr(line + 0x1388), goal, read<f32>(0x1000A4A8));
  s16 timer = read<s16>(actor, 0x444);
  u32 model = gabi::ea(a->model.get());
  if (timer)
    write<s16>(actor, 0x444, s16(timer - 1));
  gabi::call<void>(0x028E90D4, ptr(model ? model + 0xC8 : 0),
                   ptr(read<u32>(0x1018C7B0)));
  f32 amplitude = read<f32>(0x1000A4E4), base = read<f32>(0x1000A4E8),
      depth = read<f32>(0x1000A4EC);
  for (u32 i = 0; i < 16; i++) {
    gabi::call<void>(0x0200FCF0);
    f32 x = read<f32>(0x104A44F8 + ((u16(i * 0x3A98) >> 3) * 8)) * amplitude;
    f32 y = read<f32>(0x104A44F8 + ((u16(i * 0x2710) >> 3) * 8)) * amplitude;
    f32 z = gabi::fmadds(read<f32>(0x104A44F8 + ((u16(i * 0x1B58) >> 3) * 8)),
                         depth, base);
    Local<cXyz> offset;
    offset->x = x;
    offset->y = y;
    offset->z = z;
    gabi::call<void>(0x0200FCD8, offset.get(), ptr(chain));
    gabi::call<void>(0x020C1250, a, ptr(chain), i);
    gabi::call<void>(0x0200FD38);
    chain += 0x124;
  }
}
VERIFY(0x020C1684, boko_chain_calc);

BOOL boko_execute(daBoko_c *a) {
  WWHD_FUNC(0x020C183C, BOOL, a);
  u32 actor = gabi::ea(a);
  s16 delay = read<s16>(actor, 0x446);
  if (delay) {
    write<s16>(actor, 0x446, s16(delay - 1));
    return 1;
  }
  s16 slot = a->procedureSlot;
  if (slot) {
    u32 receiver = actor + u32(s32(s16(a->procedureDelta))), target;
    if (slot < 0)
      target = a->procedure;
    else {
      u32 table = read<u32>(receiver + u32(s32(read<s16>(actor, 0xA32))));
      target = read<u32>(table + u32(s32(slot)) * 8, 4);
    }
    gabi::call<void>(target, ptr(receiver));
  }
  u32 y = read<u32>(actor, 0x318), z = read<u32>(actor, 0x31C),
      x = read<u32>(actor, 0x314);
  write<u32>(actor, 0x380, y);
  write<u32>(actor, 0x398, z);
  write<u32>(actor, 0x37C, x);
  write<u32>(actor, 0x384, z);
  write<u32>(actor, 0x390, x);
  write<u32>(actor, 0x394, y);
  u32 sphere = actor + 0x700;
  bool extinguish = false;
  if (gabi::call<BOOL>(0x025162A4, ptr(sphere))) {
    u32 hit = gabi::call<u32>(0x02516300, ptr(sphere));
    s16 timer = a->flameTimer;
    if (hit && timer >= 0) {
      u32 flags = read<u32>(hit, 0x10);
      if (timer > 0) {
        if (flags & 0x100) {
          a->flameTimer = 1;
          extinguish = true;
        }
      } else if (flags & 0x20200) {
        s32 reverb = gabi::call<s32>(0x02520540, s32(read<s8>(actor, 0x326)));
        gabi::call<void>(0x025E1A40, 0x6902, &a->top, 0, reverb);
        write<u8>(actor, 0x2DB, 21);
        a->flameTimer = 900;
      }
    }
  }
  gabi::call<void>(0x027F4D5C, a->model.get());
  boko_top_root(a, 0);
  f32 zero = read<f32>(0x1000A4B8);
  if (a->flameTimer) {
    if (a->mode != 2)
      a->flameTimer = s16(s16(a->flameTimer) - 1);
    bool wet = extinguish;
    if (!wet) {
      Local<be<f32>> height;
      if (gabi::call<BOOL>(0x025D9F70, &a->top, height.get()))
        wet = !(f32(*height.get()) < f32(a->top.y));
    }
    bool stopped = wet || !a->flameTimer;
    if (wet) {
      if (a->flameTimer) {
        u32 play = gabi::call<u32>(0x025200D4);
        gabi::call<void>(0x025A847C, ptr(read<u32>(play, 0x5AB0)), 0, 0x35A,
                         &a->top, 0, 0, 255, 0, -1, 0, 0, 0);
      }
      write<f32>(actor, 0x964, zero);
      write<u8>(actor, 0x2DB, 14);
      a->flameTimer = 0;
    } else if (a->flameTimer)
      boko_flame(a);
    else {
      write<u8>(actor, 0x2DB, 14);
      write<f32>(actor, 0x964, zero);
    }
    if (stopped && read<u32>(actor, 0x4B0)) {
      s32 reverb = gabi::call<s32>(0x02520540, s32(read<s8>(actor, 0x326)));
      gabi::call<void>(0x025E1A40, 0x6904, &a->top, 0, reverb);
      gabi::call<void>(0x025A5AC8, ptr(actor + 0x4AC));
    }
    u32 topY = read<u32>(actor, 0x450), topZ = read<u32>(actor, 0x454),
        topX = read<u32>(actor, 0x44C);
    write<u32>(actor, 0x484, topZ);
    write<u32>(actor, 0x47C, topX);
    f32 factor = read<f32>(0x1000A48C);
    write<u32>(actor, 0x480, topY);
    u32 play = gabi::call<u32>(0x025200D4), stage = play + 0x5150,
        target = read<u32>(read<u32>(stage), 0x15C);
    if (gabi::call<u32>(target, ptr(stage))) {
      play = gabi::call<u32>(0x025200D4);
      stage = play + 0x5150;
      target = read<u32>(read<u32>(stage), 0x15C);
      u32 info = gabi::call<u32>(target, ptr(stage)),
          kind = (read<u32>(info, 0xC) >> 16) & 7;
      if (kind == 1 || kind == 4)
        factor = read<f32>(0x1000A4F0);
    }
    write<f32>(actor, 0x490,
               (read<f32>(0x1000A4F4) * factor) * read<f32>(actor, 0x964));
  } else {
    gabi::call<void>(0x02516264, ptr(sphere));
    write<f32>(actor, 0x490, zero);
  }
  if (a->mParameters == 0) {
    u32 flags = read<u32>(sphere);
    write<u32>(sphere, 0, a->flameTimer > 0 ? flags | 1 : flags & ~1u);
    gabi::call<void>(0x02018D40, ptr(sphere + 0x118), &a->top);
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x0200E240, ptr(play + 0x26A4), ptr(sphere));
    u8 mode = a->mode;
    play = gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x02516C14, ptr(play + 0x4EF8), ptr(sphere),
                     mode == 3 ? 1 : 4);
  }
  if (a->line.get())
    boko_chain_calc(a);
  if (a->mParameters == 5) {
    u32 save = read<u32>(0x101F84DC);
    if (gabi::call<BOOL>(0x025B8B94, ptr(save + 0x644), 0x3A08)) {
      gabi::call<void>(0x025E742C, ptr(actor + 0x3B8));
      if ((read<u8>(actor, 0x3C7) & 1) || read<f32>(actor, 0x3B8) == zero)
        gabi::call<void>(0x025D57E0, a);
    }
  }
  return 1;
}
VERIFY(0x020C183C, boko_execute);
BOOL boko_execute_wrapper(daBoko_c *a) {
  WWHD_FUNC(0x020C1D0C, BOOL, a);
  return boko_execute(a);
}
VERIFY(0x020C1D0C, boko_execute_wrapper);

void boko_chain_segment(daBoko_c *a, void *chainObject, s32 index) {
  WWHD_FUNC(0x020C1250, void, a, chainObject, index);
  u32 actor = gabi::ea(a), chain = gabi::ea(chainObject);
  if (!read<s16>(actor, 0x444))
    return;
  f32 gravity = read<f32>(0x1047BBD0) + read<f32>(0x1000A4C0);
  Local<cXyz> step;
  step->x = read<f32>(0x1000A4B8);
  step->y = read<f32>(0x1000A4B8);
  step->z = read<f32>(0x1000A4BC);
  struct Ground {
    u8 data[0x54];
  };
  Local<Ground> ground;
  u32 g = gabi::ea(ground.get());
  gabi::call<void>(0x02008E0C, ground.get());
  f32 coefficient = read<f32>(0x1047BBD4) + read<f32>(0x1000A4D0);
  write<u32>(g, 0x10, 0x1000A348);
  write<u32>(g, 0x20, 0x1000A358);
  for (u32 i = 0x44; i <= 0x4A; i++)
    write<u8>(g, i, 0);
  write<u32>(g, 0x4C, 0x1000A368);
  write<u32>(g, 0x40, 0x1000A378);
  write<u32>(g, 4, g + 0x4C);
  write<u32>(g, 0, g + 0x40);
  write<u32>(g, 0x50, 1);
  f32 heightOffset = read<f32>(0x1000A4DC), floorOffset = read<f32>(0x1000A4D8);
  u32 line = gabi::ea(a->line.get());
  f32 damping = gabi::fmadds(f32(index), coefficient, read<f32>(line, 0x1388));
  f32 sway = read<f32>(0x1000A4D4) * damping;
  u32 position = chain, velocity = chain + 0x78;
  u32 phaseX = u32(index) * 0x1388 - 0x2328, phaseZ = (u32(index) - 1) * 0x1B58;
  for (u32 i = 1; i < 10; i++) {
    position += 12;
    velocity += 12;
    f32 px = read<f32>(position), pz = read<f32>(position, 8);
    f32 dx = (px - read<f32>(position - 12)) + read<f32>(velocity);
    f32 dz = (pz - read<f32>(position - 12, 8)) + read<f32>(velocity, 8);
    u32 count = read<u32>(0x101FF560);
    f32 horizontalX =
        gabi::fmadds(read<f32>(0x104A44F8 + (u16(phaseX) >> 3) * 8), sway, dx);
    f32 horizontalZ =
        gabi::fmadds(read<f32>(0x104A44F8 + (u16(phaseZ) >> 3) * 8), sway, dz);
    u32 floorAddress = chain + 0xF0 + i * 4;
    f32 floor;
    if ((count + i + u32(index) * 10) & 7)
      floor = read<f32>(floorAddress);
    else {
      write<f32>(g, 0x2C, pz);
      write<f32>(g, 0x24, px);
      write<f32>(g, 0x28, read<f32>(position, 4) + heightOffset);
      u32 play = gabi::call<u32>(0x025200D4);
      floor = gabi::call<f32>(0x02008974, ptr(play + 0x12A0), ground.get()) +
              floorOffset;
      write<f32>(floorAddress, 0, floor);
    }
    f32 vertical = read<f32>(position, 4) + gravity;
    if (vertical < floor)
      vertical = floor;
    f32 dy = vertical - read<f32>(position - 12, 4);
    s16 pitch = s16(-gabi::call<s32>(0x020195B0, dy, horizontalZ));
    f32 square = gabi::fmadds(dy, dy, horizontalZ * horizontalZ),
        length = gabi::call<f32>(0x028F4384, square);
    s32 yaw = gabi::call<s32>(0x020195B0, horizontalX, length);
    gabi::call<void>(0x025F18EC, ptr(read<u32>(0x1018C7B0)), pitch);
    gabi::call<void>(0x025F1C28, ptr(read<u32>(0x1018C7B0)), yaw);
    Local<cXyz> transformed;
    gabi::call<void>(0x0200FCD8, step.get(), transformed.get());
    write<f32>(velocity, 0, read<f32>(position));
    write<f32>(velocity, 8, read<f32>(position, 8));
    f32 nextX = read<f32>(position - 12) + f32(transformed->x);
    write<f32>(position, 0, nextX);
    f32 nextY = read<f32>(position - 12, 4) + f32(transformed->y);
    write<f32>(position, 4, nextY);
    f32 nextZ = read<f32>(position - 12, 8) + f32(transformed->z);
    write<f32>(position, 8, nextZ);
    f32 oldX = read<f32>(velocity), oldZ = read<f32>(velocity, 8);
    write<f32>(velocity, 0, (nextX - oldX) * damping);
    write<f32>(velocity, 8, (read<f32>(position, 8) - oldZ) * damping);
    phaseZ -= 0x1B58;
    phaseX -= 0x2328;
  }
  line = gabi::ea(a->line.get());
  u32 rows = read<u32>(line, 0x144), output = read<u32>(rows + u32(index) * 16);
  for (u32 i = 0; i < 10; i++) {
    u32 input = chain + i * 12, dest = output + i * 12;
    write<u32>(dest, 0, read<u32>(input));
    write<u32>(dest, 4, read<u32>(input, 4));
    write<u32>(dest, 8, read<u32>(input, 8));
  }
  write<u32>(g, 0x40, 0x1000A378);
  write<u32>(g, 0x20, 0x1000A358);
  write<u32>(g, 0x4C, 0x1000A338);
  gabi::call<void>(0x02008DAC, ground.get(), 0);
}
VERIFY(0x020C1250, boko_chain_segment);

BOOL boko_throw(daBoko_c *a) {
  WWHD_FUNC(0x020C2978, BOOL, a);
  u32 actor = gabi::ea(a);
  Local<cXyz> previous;
  previous->x = a->root.x;
  previous->y = a->root.y;
  previous->z = a->root.z;
  write<s16>(actor, 0x444, 20);
  gabi::call<void>(0x025D6870, a, 0);
  f32 zero = read<f32>(0x1000A4B8);
  Local<cXyz> horizontal;
  horizontal->x = a->speed.x;
  horizontal->y = zero;
  horizontal->z = a->speed.z;
  f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get()),
      length = gabi::call<f32>(0x028F4384, square);
  s32 pitch = gabi::call<s32>(0x020195B0, -f32(a->speed.y), length);
  a->shape_angle.x = s16(pitch);
  s32 yaw = gabi::call<s32>(0x020195B0, f32(a->speed.x), f32(a->speed.z));
  s16 timer = a->timer;
  a->shape_angle.y = s16(yaw);
  a->current.angle.y = s16(yaw);
  if (timer > 0) {
    timer = s16(timer - 1);
    a->timer = timer;
    if (!timer)
      a->gravity = read<f32>(0x1000A548);
  }
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x024F08A8, ptr(actor + 0x4C0), ptr(play + 0x12A0));
  boko_room(a);
  boko_base_matrix(a);
  boko_top_root(a, 0);
  bool collision = false;
  s16 direction = 0;
  BOOL hit = gabi::call<BOOL>(0x025160DC, ptr(actor + 0x82C));
  u32 flags = read<u32>(actor, 0x4E8);
  if (hit || (flags & 0x30) || (flags & 0x200)) {
    collision = true;
    if ((flags & 0x200) && f32(a->speed.y) > zero)
      a->speed.y = f32(a->speed.y) * read<f32>(0x1000A574);
    if (gabi::call<BOOL>(0x025160DC, ptr(actor + 0x82C))) {
      f32 random = gabi::call<f32>(0x020198D8, read<f32>(0x1000A578));
      direction = s16(gabi::ftoi(random + f32(s32(s16(a->current.angle.y)))));
    } else if (read<u32>(actor, 0x4E8) & 0x10)
      direction = read<s16>(actor, 0x6C0);
    else
      direction = a->current.angle.y;
  }
  gabi::call<void>(0x02018808, ptr(actor + 0x944), previous.get(), &a->top);
  play = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x0200E240, ptr(play + 0x26A4), ptr(actor + 0x82C));
  if (!collision) {
    u32 line = 0x104625EC;
    gabi::call<void>(0x024F1AFC, ptr(line), ptr(actor + 0x300), &a->top, a);
    play = gabi::call<u32>(0x025200D4);
    if (gabi::call<BOOL>(0x02008860, ptr(play + 0x12A0), ptr(line))) {
      play = gabi::call<u32>(0x025200D4);
      u16 poly = read<u16>(line, 0x14), object = read<u16>(line, 0x16);
      u32 plane = gabi::call<u32>(0x020084C8, ptr(play + 0x12A0), object, poly);
      if (plane && read<f32>(plane, 4) < read<f32>(0x1000A494) &&
          !(read<f32>(plane, 4) < read<f32>(0x1000A57C)))
        direction = s16(
            gabi::call<s32>(0x020195B0, read<f32>(plane), read<f32>(plane, 8)));
      else
        direction = a->current.angle.y;
      collision = true;
    }
  }
  if (collision)
    boko_throw_reverse(a, direction);
  return 1;
}
VERIFY(0x020C2978, boko_throw);

BOOL boko_wait(daBoko_c *a) {
  WWHD_FUNC(0x020C3964, BOOL, a);
  u32 actor = gabi::ea(a);
  struct Ground {
    u8 data[0x54];
  };
  Local<Ground> ground;
  u32 g = gabi::ea(ground.get());
  gabi::call<void>(0x02008E0C, ground.get());
  f32 heightOffset = read<f32>(0x1000A4E8);
  for (u32 i = 0x44; i <= 0x4A; i++)
    write<u8>(g, i, 0);
  write<u32>(g, 0x4C, 0x1000A368);
  write<u32>(g, 0x40, 0x1000A378);
  write<u32>(g, 4, g + 0x4C);
  write<u32>(g, 0x50, 1);
  write<u32>(g, 0, g + 0x40);
  write<u32>(g, 0x10, 0x1000A348);
  write<u32>(g, 0x20, 0x1000A358);
  write<f32>(g, 0x2C, f32(a->root.z));
  write<f32>(g, 0x28, f32(a->root.y) + heightOffset);
  write<f32>(g, 0x24, f32(a->root.x));
  u32 play = gabi::call<u32>(0x025200D4);
  f32 rootFloor = gabi::call<f32>(0x02008974, ptr(play + 0x12A0), ground.get());
  write<f32>(g, 0x2C, f32(a->top.z));
  write<f32>(g, 0x24, f32(a->top.x));
  write<f32>(g, 0x28, f32(a->top.y) + heightOffset);
  Local<cXyz> difference;
  gabi::call<void>(0x0201ADE0, &a->top, difference.get(), &a->root);
  play = gabi::call<u32>(0x025200D4);
  u32 world = play + 0x12A0;
  f32 zero = read<f32>(0x1000A4B8);
  Local<cXyz> horizontal;
  horizontal->x = difference->x;
  horizontal->y = zero;
  horizontal->z = difference->z;
  f32 square = gabi::call<f32>(0x028E8DD0, horizontal.get()),
      length = gabi::call<f32>(0x028F4384, square),
      topFloor = gabi::call<f32>(0x02008974, ptr(world), ground.get());
  gabi::call<s32>(0x020195B0, -(topFloor - rootFloor), length);
  if (read<u32>(actor, 0x2E0) & 0x2000) {
    a->procedureDelta = 0;
    a->procedureSlot = -1;
    a->gravity = zero;
    a->procedure = 0x020C3480;
    a->speedF = zero;
    write<u32>(actor, 0x33C, read<u32>(0x101FFBA8));
    write<u32>(actor, 0x340, read<u32>(0x101FFBAC));
    write<u32>(actor, 0x344, read<u32>(0x101FFBB0));
    u8 mode = a->mode;
    u32 flags = read<u32>(actor, 0x39C);
    if (mode != 3)
      a->mode = 2;
    a->floorFlag = 0;
    write<u32>(actor, 0x39C, flags & ~0x10u);
    gabi::call<BOOL>(0x020C3480, a);
  } else if (!a->floorFlag) {
    gabi::call<void>(0x025D6870, a, 0);
    play = gabi::call<u32>(0x025200D4);
    gabi::call<void>(0x024F08A8, ptr(actor + 0x4C0), ptr(play + 0x12A0));
    struct Line {
      u8 data[0x6C];
    };
    Local<Line> line;
    u32 l = gabi::ea(line.get());
    gabi::call<void>(0x02008FEC, line.get());
    Local<cXyz> start, end;
    f32 x = a->current.pos.x, y = a->current.pos.y, z = a->current.pos.z,
        offset = read<f32>(0x1000A550);
    start->x = x;
    start->y = y - offset;
    start->z = z;
    end->x = x;
    end->y = y + offset;
    end->z = z;
    for (u32 i = 0x5C; i <= 0x62; i++)
      write<u8>(l, i, 0);
    write<u32>(l, 0x68, 1);
    write<u32>(l, 0, l + 0x58);
    write<u32>(l, 4, l + 0x64);
    write<u32>(l, 0x20, 0x1000A408);
    write<u32>(l, 0x10, 0x1000A3F8);
    write<u32>(l, 0x58, 0x1000A428);
    write<u32>(l, 0x64, 0x1000A418);
    gabi::call<void>(0x024F1AFC, line.get(), start.get(), end.get(), a);
    play = gabi::call<u32>(0x025200D4);
    BOOL crossed = gabi::call<BOOL>(0x02008860, ptr(play + 0x12A0), line.get());
    u32 flags = read<u32>(actor, 0x4E8);
    if (crossed) {
      f32 lower = read<f32>(0x1047BBDC) + read<f32>(0x1000A544);
      a->shape_angle.x = 0x1000;
      a->current.pos.y = f32(a->current.pos.y) - lower;
    }
    if (!(flags & 0x20))
      boko_move_init(a);
    boko_room(a);
    boko_base_matrix(a);
    write<u32>(l, 0x58, 0x1000A428);
    write<u32>(l, 0x64, 0x1000A338);
    write<u32>(l, 0x20, 0x1000A328);
    gabi::call<void>(0x02008B4C, line.get(), 0);
  }
  write<u32>(g, 0x20, 0x1000A358);
  write<u32>(g, 0x40, 0x1000A378);
  write<u32>(g, 0x4C, 0x1000A338);
  gabi::call<void>(0x02008DAC, ground.get(), 0);
  return 1;
}
VERIFY(0x020C3964, boko_wait);

BOOL boko_carry(daBoko_c *a) {
  WWHD_FUNC(0x020C3480, BOOL, a);
  u32 actor = gabi::ea(a), flags = read<u32>(actor, 0x2E0);
  write<s16>(actor, 0x444, 20);
  if ((flags & 0x2000) || !a->thrown) {
    u32 model = gabi::ea(a->model.get()), matrix = model ? model + 0xC8 : 0;
    a->current.pos.x = read<f32>(matrix, 12);
    a->current.pos.y = read<f32>(matrix, 28);
    model = gabi::ea(a->model.get());
    a->current.pos.z = read<f32>(matrix, 44);
    gabi::call<void>(0x025F232C, ptr(model ? model + 0xC8 : 0),
                     &a->shape_angle);
  }
  u32 play = gabi::call<u32>(0x025200D4), player = read<u32>(play, 0x5B34);
  f32 five = read<f32>(0x1000A488);
  Local<cXyz> playerPosition;
  playerPosition->x = read<f32>(player, 0x314);
  playerPosition->y = read<f32>(player, 0x318) + five;
  playerPosition->z = read<f32>(player, 0x31C);
  if (read<u32>(actor, 0x2E0) & 0x2000)
    return 1;
  gabi::call<void>(0x025D9D0C, a, 0);
  boko_top_root(a, 1);
  f32 zero = read<f32>(0x1000A4B8);
  u32 line = 0x104625EC;
  if (a->thrown) {
    u16 angle = u16(a->throwAngle);
    a->gravity = zero;
    a->procedureDelta = 0;
    a->mode = 6;
    a->shape_angle.x = s16(angle);
    a->procedureSlot = -1;
    a->procedure = 0x020C2978;
    s16 yaw = read<s16>(player, 0x32A);
    a->current.angle.y = yaw;
    a->shape_angle.y = yaw;
    u32 table = 0x104A44F8 + (angle >> 3) * 8;
    a->speedF = read<f32>(0x1000A598) * read<f32>(table, 4);
    a->thrown = 0;
    a->speed.y = read<f32>(0x1000A59C) * read<f32>(table);
    a->timer = 2;
    gabi::call<void>(0x02516138, ptr(actor + 0x82C));
    gabi::call<void>(0x024F1AFC, ptr(line), playerPosition.get(),
                     &a->current.pos, a);
    play = gabi::call<u32>(0x025200D4);
    if (gabi::call<BOOL>(0x02008860, ptr(play + 0x12A0), ptr(line))) {
      write<u32>(actor, 0x314, read<u32>(line, 0x30));
      write<u32>(actor, 0x318, read<u32>(line, 0x34));
      write<u32>(actor, 0x31C, read<u32>(line, 0x38));
      play = gabi::call<u32>(0x025200D4);
      u16 poly = read<u16>(line, 0x14), object = read<u16>(line, 0x16);
      u32 plane = gabi::call<u32>(0x020084C8, ptr(play + 0x12A0), object, poly);
      if (!plane)
        return 1;
      Local<cXyz> offset;
      gabi::call<void>(0x0201AE48, ptr(plane), offset.get(),
                       read<f32>(0x1000A4EC));
      gabi::call<void>(0x028E8D88, &a->current.pos, offset.get(),
                       &a->current.pos);
      s32 angle =
          gabi::call<s32>(0x020195B0, read<f32>(plane), read<f32>(plane, 8));
      boko_throw_reverse(a, s16(angle));
    } else
      boko_throw(a);
    return 1;
  }
  if (a->floorFlag) {
    if (a->shape_angle.z >= 0x4000) {
      s16 pitch = a->shape_angle.x, yaw = a->shape_angle.y;
      a->shape_angle.z = 0;
      a->shape_angle.x = s16(u16(pitch) - 0x8000);
      a->shape_angle.y = s16(u16(yaw) - 0x8000);
    }
    boko_wait_init(a);
    return 1;
  }
  a->current.angle.y = s16(u16(a->shape_angle.y) + 0x8000);
  if (a->mode == 3) {
    gabi::call<void>(0x024F1AFC, ptr(line), playerPosition.get(),
                     &a->current.pos, a);
    play = gabi::call<u32>(0x025200D4);
    if (gabi::call<BOOL>(0x02008860, ptr(play + 0x12A0), ptr(line))) {
      write<u32>(actor, 0x318, read<u32>(gabi::ea(playerPosition.get()), 4));
      write<u32>(actor, 0x31C, read<u32>(gabi::ea(playerPosition.get()), 8));
      write<u32>(actor, 0x314, read<u32>(gabi::ea(playerPosition.get())));
      Local<cXyz> offset;
      offset->x = zero;
      offset->y = read<f32>(0x1000A5A0);
      offset->z = read<f32>(0x1000A5A4);
      gabi::call<void>(0x025F1884, ptr(read<u32>(0x1018C7B0)),
                       read<s16>(player, 0x32A));
      gabi::call<void>(0x0200FCD8, offset.get(), offset.get());
      gabi::call<void>(0x028E8D88, &a->current.pos, offset.get(),
                       &a->current.pos);
      a->speed.y = zero;
      a->shape_angle.z = 0;
      a->shape_angle.x = -0x2000;
    }
  }
  boko_move_init(a);
  f32 distance = read<f32>(0x1000A4DC), invalid = read<f32>(0x1000A570);
  u32 ground = 0x10462598;
  f32 originalY = distance;
  bool found = false;
  for (u32 attempt = 0; attempt < 3; attempt++) {
    write<f32>(ground, 0x24, f32(a->current.pos.x));
    write<f32>(ground, 0x2C, f32(a->current.pos.z));
    write<f32>(ground, 0x28, f32(a->current.pos.y));
    play = gabi::call<u32>(0x025200D4);
    f32 height = gabi::call<f32>(0x02008974, ptr(play + 0x12A0), ptr(ground));
    if (height != invalid) {
      originalY = a->current.pos.y;
      found = true;
      break;
    }
    u32 table = 0x104A44F8 + (u16(a->shape_angle.y) >> 3) * 8;
    a->current.pos.x =
        gabi::fnmsubs(distance, read<f32>(table), f32(a->current.pos.x));
    a->current.pos.z =
        gabi::fnmsubs(distance, read<f32>(table, 4), f32(a->current.pos.z));
  }
  if (!found) {
    write<u32>(actor, 0x314, read<u32>(gabi::ea(playerPosition.get())));
    write<u32>(actor, 0x31C, read<u32>(gabi::ea(playerPosition.get()), 8));
  }
  f32 x = a->current.pos.x, z = a->current.pos.z;
  a->old.pos.y = originalY;
  a->gravity = read<f32>(0x1000A548);
  a->old.pos.x = x;
  a->old.pos.z = z;
  a->angleSpeed = 0;
  a->current.pos.y = originalY - five;
  gabi::call<BOOL>(0x020C2C90, a);
  return 1;
}
VERIFY(0x020C3480, boko_carry);

BOOL boko_move(daBoko_c *a) {
  WWHD_FUNC(0x020C2C90, BOOL, a);
  u32 actor = gabi::ea(a);
  bool bounced = false;
  write<s16>(actor, 0x444, 20);
  gabi::call<void>(0x025D6870, a, 0);
  f32 previousRootY = a->root.y, previousTopY = a->top.y;
  boko_top_root(a, 1);
  u32 ground = 0x10462598;
  write<f32>(ground, 0x24, f32(a->root.x));
  write<f32>(ground, 0x28, previousRootY + read<f32>(0x1000A4DC));
  write<f32>(ground, 0x2C, f32(a->root.z));
  u32 play = gabi::call<u32>(0x025200D4);
  f32 rootFloor = gabi::call<f32>(0x02008974, ptr(play + 0x12A0), ptr(ground));
  if (rootFloor == read<f32>(0x1000A570))
    rootFloor = read<f32>(actor, 0x554);
  write<f32>(ground, 0x24, f32(a->top.x));
  write<f32>(ground, 0x28, previousTopY + read<f32>(0x1000A4DC));
  f32 rootGap = f32(a->root.y) - rootFloor;
  write<f32>(ground, 0x2C, f32(a->top.z));
  play = gabi::call<u32>(0x025200D4);
  f32 topFloor = gabi::call<f32>(0x02008974, ptr(play + 0x12A0), ptr(ground));
  if (topFloor == read<f32>(0x1000A570))
    topFloor = read<f32>(actor, 0x554);
  f32 topGap = f32(a->top.y) - topFloor, upLimit = read<f32>(0x1000A584),
      damping = read<f32>(0x1000A4E0), uintThreshold = read<f32>(0x1000A580);
  auto volume = [uintThreshold](f32 value) {
    u32 result;
    if (value < uintThreshold)
      result = u32(gabi::ftoi(value));
    else
      result = u32(gabi::ftoi(value - uintThreshold)) + 0x80000000u;
    return result > 100 ? 100u : result;
  };
  if ((rootGap < topGap && rootGap < read<f32>(0x1047BBB0)) ||
      (topGap < rootGap && topGap < read<f32>(0x1047BBB0))) {
    f32 vertical = a->speed.y;
    if (vertical < read<f32>(0x1000A540)) {
      vertical *= read<f32>(0x1000A574);
      a->speed.y = vertical > upLimit ? upLimit : vertical;
    }
    if (a->moveCounter) {
      f32 selected = (rootGap - topGap) >= 0 ? topGap : rootGap;
      u8 count = a->moveCounter;
      a->moveCounter = u8(count - 1);
      a->current.pos.y = f32(a->current.pos.y) - selected;
    }
    Local<cXyz> direction;
    direction->x = f32(a->top.x) - f32(a->root.x);
    direction->y = read<f32>(0x1000A4B8);
    direction->z = f32(a->top.z) - f32(a->root.z);
    f32 difference = topFloor - rootFloor;
    f32 square = gabi::call<f32>(0x028E8DD0, direction.get()),
        length = gabi::call<f32>(0x028F4384, square);
    s32 desired = gabi::call<s32>(0x020195B0, -difference, length);
    s16 angleDifference = s16(u32(s32(s16(a->shape_angle.x))) - u32(desired));
    f32 correction = read<f32>(0x1047BBC0) + read<f32>(0x1000A588);
    s16 angular = s16(gabi::ftoi(-(f32(s32(angleDifference)) * correction)));
    if (angular)
      a->angleSpeed = angular;
    else {
      a->angleSpeed = angleDifference ? s16(-angleDifference) : 0;
      if (!a->angleSpeed &&
          __builtin_fabsf(f32(a->speedF)) < read<f32>(0x1000A48C)) {
        a->angleSpeed = s16(-angleDifference);
        u8 count = a->moveCounter;
        a->moveCounter = count > 2 ? u8(count - 2) : 0;
        a->angleSpeed = a->shape_angle.x > 0 ? -90 : 90;
      }
    }
    u32 loudness = volume(f32(a->speed.y) * read<f32>(0x1000A58C)),
        type = a->mParameters, sound = read<u32>(0x1019201C + type * 4);
    s32 reverb = gabi::call<s32>(0x02520540, s32(read<s8>(actor, 0x326)));
    gabi::call<void>(0x025E1A40, sound, &a->eyePos, loudness, reverb);
    s16 rotation = a->rotationSpeed;
    a->speedF = f32(a->speedF) * damping;
    f32 rate = read<f32>(0x1047BBB8) + damping;
    a->rotationSpeed = s16(gabi::ftoi(f32(s32(rotation)) * rate));
    bounced = true;
  }
  s32 rotation = a->rotationSpeed, angular = a->angleSpeed;
  auto magnitude = [](s32 value) { return value < 0 ? -value : value; };
  s16 pitch =
      s16(s32(s16(a->shape_angle.x)) +
          (magnitude(rotation) > magnitude(angular) ? rotation : angular));
  a->shape_angle.x = pitch;
  if (!rotation) {
    if (pitch > 0x3A00)
      a->shape_angle.x = 0x3A00;
    else if (pitch < -0x3A00)
      a->shape_angle.x = -0x3A00;
  }
  if (a->angleSpeed) {
    s32 roll = a->shape_angle.z;
    gabi::call<void>(0x0200F378, &a->shape_angle.z,
                     magnitude(roll) > 0x4000 ? -0x8000 : 0, 5, 0x1000, 0x100);
  }
  f32 vertical = a->speed.y;
  play = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x024F08A8, ptr(actor + 0x4C0), ptr(play + 0x12A0));
  boko_room(a);
  if ((read<u32>(actor, 0x4E8) & 0x20) && !(vertical > read<f32>(0x1000A590))) {
    play = gabi::call<u32>(0x025200D4);
    BOOL moving =
        gabi::call<BOOL>(0x024EEABC, ptr(play + 0x12A0), ptr(actor + 0x5A8));
    f32 bounce = read<f32>(0x1000A594);
    bool treasure = false;
    if (moving) {
      play = gabi::call<u32>(0x025200D4);
      u32 other = gabi::call<u32>(0x02008438, ptr(play + 0x12A0),
                                  read<u16>(actor, 0x5AA));
      treasure = other && read<s16>(other, 8) == 0x124;
    }
    if (treasure) {
      if (f32(a->speedF) < bounce)
        a->speedF = bounce;
      a->speed.y = upLimit;
    } else {
      play = gabi::call<u32>(0x025200D4);
      u16 poly = read<u16>(actor, 0x5A8), object = read<u16>(actor, 0x5AA);
      u32 plane = gabi::call<u32>(0x020084C8, ptr(play + 0x12A0), object, poly);
      s32 desired = 0;
      s16 difference;
      if (plane) {
        s32 yaw =
            gabi::call<s32>(0x020195B0, read<f32>(plane), read<f32>(plane, 8));
        u16 delta = u16(u32(yaw) - u32(s32(s16(a->shape_angle.y))));
        f32 z = read<f32>(plane, 8), x = read<f32>(plane),
            cosine = read<f32>(0x104A44FC + (delta >> 3) * 8);
        f32 square = gabi::fmadds(x, x, z * z),
            length = gabi::call<f32>(0x028F4384, square);
        desired =
            gabi::call<s32>(0x020195B0, length * cosine, read<f32>(plane, 4));
        difference = s16(u32(desired) - u32(s32(s16(a->shape_angle.x))));
      } else
        difference = s16(-s32(s16(a->shape_angle.x)));
      s32 threshold = s32(read<s16>(0x1047BC2C)) + 0x800;
      if (magnitude(difference) > threshold && a->moveCounter) {
        f32 speed = a->speedF;
        u8 count = a->moveCounter;
        a->speed.y = bounce;
        if (speed < bounce)
          a->speedF = bounce;
        a->rotationSpeed = 0;
        a->angleSpeed = s16(-(s32(difference) >> 2));
        a->moveCounter = count > 4 ? u8(count - 4) : 0;
      } else {
        a->shape_angle.x = s16(desired);
        a->shape_angle.z = 0;
        boko_wait_init(a);
      }
    }
    if (!bounced) {
      f32 difference =
              __builtin_fabsf(f32(a->old.pos.y) - f32(a->current.pos.y)),
          loudnessF = difference * read<f32>(0x1000A58C);
      u32 loudness = volume(loudnessF), type = a->mParameters,
          sound = read<u32>(0x1019201C + type * 4);
      s32 reverb = gabi::call<s32>(0x02520540, s32(read<s8>(actor, 0x326)));
      gabi::call<void>(0x025E1A40, sound, &a->eyePos, loudness, reverb);
      a->speedF = f32(a->speedF) * damping;
    }
  } else
    a->speed.y = vertical;
  boko_base_matrix(a);
  return 1;
}
VERIFY(0x020C2C90, boko_move);

#include "d/actor/d_a_canon.h"
#include "bindings.h"

static Mtx34 *matrix() { return gabi::at<Mtx34>(0x1048D0CC); }
static f32 sine(s16 angle) {
  return gabi::load<f32>(0x104A44F8 + 8u * ((u16)angle >> 3));
}
static f32 cosine(s16 angle) {
  return gabi::load<f32>(0x104A44FC + 8u * ((u16)angle >> 3));
}
static u32 play() { return gabi::ea(gabi::call<void *>(0x025200D4)); }
static void set_process(daCanon_c *a, u32 target) {
  a->process.slot = -1;
  a->process.target = target;
  a->process.delta = 0;
}
static bool firing(daCanon_c *a) {
  s16 slot = a->process.slot;
  return slot == -1 && (slot == 0 || ((s16)a->process.delta == 0 &&
                                      (u32)a->process.target == 0x0210B228));
}
static void sound(void *a, u32 id) {
  u32 addr = gabi::ea(a);
  if (addr && addr + 0x37Cu) {
    s8 room = gabi::load<s8>(addr + 0x326);
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call<void>(0x025E1A40, id, gabi::at<void>(addr + 0x37C), 0, reverb);
  }
}
static void render_light(daCanon_c *a, cXyz *p, bool ball) {
  void *light = gabi::call<void *>(0x02555D0C);
  gabi::call<void>(0x025626A4, light, 0, p,
                   gabi::at<void>(gabi::ea(a) + 0x110));
  light = gabi::call<void *>(0x02555D0C);
  J3DModel *m = ball ? (J3DModel *)a->ballModel : (J3DModel *)a->model;
  gabi::call<void>(0x02562F5C, light, m, gabi::at<void>(gabi::ea(a) + 0x110));
  m = ball ? (J3DModel *)a->ballModel : (J3DModel *)a->model;
  gabi::call<void>(0x025E2DE0, m, 0);
}
static void display_list(void *object) {
  u32 p = play() + 0x5D30;
  gabi::call<void>(0x0252CDC0, gabi::at<void>(p), gabi::at<void>(p + 0x1DC),
                   gabi::at<void>(p + 0x1E0), object);
}
static void finish_game(u32 p) {
  u16 flags = gabi::load<u16>(p + 0x5CE8);
  gabi::store<u8>(p + 0x5CEE, 0);
  gabi::store<u8>(p + 0x5CEA, 0);
  gabi::store<u16>(p + 0x5CE8, flags ^ 4);
}

void canon_set_matrix(daCanon_c *);
void canon_wait_init(daCanon_c *);
void canon_game_start_init(daCanon_c *);
void canon_game_init(daCanon_c *);
void canon_fire_init(daCanon_c *);
void canon_pause_init(daCanon_c *);
void canon_end_init(daCanon_c *);
void canon_make_effect(daCanon_c *, cXyz *, csXyz *, s32);
void canon_create_targets(daCanon_c *);
void canon_pad_move(daCanon_c *);
void canon_draw_info(daCanon_c *);
void canon_grid_position(daCanon_c *, cXyz *, s32, s32);
void canon_ball_endpoint(daCanon_c *, cXyz *, s16, s16);
void canon_ball_matrix(daCanon_c *, cXyz *);
Pair32 canon_break_all(daCanon_c *);

BOOL canon_node_callback(void *node, s32 timing) {
  WWHD_FUNC(0x02109054, BOOL, node, timing);
  if (timing == 0) {
    u32 model = gabi::load<u32>(0x104B462C);
    void *joint = gabi::call<void *>(0x027F7878, node);
    u16 n = gabi::load<u16>(gabi::ea(joint) + 4);
    u32 buffer = gabi::load<u32>(model + 0x2C);
    u16 flags = gabi::load<u16>(buffer + 4);
    u32 matrices = gabi::load<u32>(buffer + 0x10);
    daCanon_c *a = gabi::at<daCanon_c>(gabi::load<u32>(model + 0xB8));
    gabi::store<u16>(buffer + 4, flags | 0x10);
    gabi::call<void>(0x028E90D4, gabi::at<Mtx34>(matrices + 48u * n), matrix());
    gabi::call<void>(0x025F1C28, matrix(), (s16)a->shape_angle.x);
    buffer = gabi::load<u32>(model + 0x2C);
    flags = gabi::load<u16>(buffer + 4);
    gabi::store<u16>(buffer + 4, flags | 0x10);
    matrices = gabi::load<u32>(buffer + 0x10);
    mtx_copy(gabi::at<Mtx34>(matrices + 48u * n), matrix());
  }
  return 1;
}
VERIFY(0x02109054, canon_node_callback);
BOOL canon_target_callback(void *a) {
  WWHD_FUNC(0x02109160, BOOL, a);
  u32 addr = gabi::ea(a) + 0x2E0;
  gabi::store<u32>(addr, gabi::load<u32>(addr) & ~0x180u);
  return 1;
}
VERIFY(0x02109160, canon_target_callback);
void *canon_break_target(void *target, void *parent) {
  WWHD_FUNC(0x02109174, void *, target, parent);
  if (gabi::call<BOOL>(0x025D4604, target) && target &&
      gabi::load<s16>(gabi::ea(target) + 8) == 0x1C9) {
    u32 link = gabi::load<u32>(gabi::ea(target) + 0x2E8);
    u32 id = parent ? gabi::load<u32>(gabi::ea(parent) + 4) : 0xFFFFFFFFu;
    if (id == link) {
      u32 params = gabi::call<u32>(0x020CB8D8, 0, 0, 0);
      gabi::call<void>(
          0x025D5834, 0x126, params, gabi::at<cXyz>(gabi::ea(target) + 0x314),
          -1, (void *)nullptr, (void *)nullptr, -1, gabi::at<void>(0x02542D74));
    }
  }
  return nullptr;
}
VERIFY(0x02109174, canon_break_target);

BOOL canon_heap(daCanon_c *a) {
  WWHD_FUNC(0x02109220, BOOL, a);
  canon::Local<be<u32>[2]> name1;
  (*name1)[0] = 0x1000C520;
  (*name1)[1] = 0x1000C3C4;
  void *controller = gabi::at<void>(gabi::load<u32>(0x101F4F28));
  void *data = gabi::call<void *>(0x026066C4, controller, name1.get(), 5);
  if (!data)
    gabi::call<void>(0x0273AA24, STR(0x1000C41C), 849, STR(0x1000C440));
  J3DModel *m = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203u);
  a->model = m;
  if (!m)
    return 0;
  canon::Local<be<u32>[2]> name2;
  (*name2)[1] = 0x1000C3C4;
  controller = gabi::at<void>(gabi::load<u32>(0x101F4F28));
  (*name2)[0] = 0x1000C520;
  void *data2 = gabi::call<void *>(0x026066C4, controller, name2.get(), 8);
  if (!data2)
    gabi::call<void>(0x0273AA24, STR(0x1000C41C), 857, STR(0x1000C42C));
  void *names = gabi::call<void *>(0x027F68FC, data);
  u32 offset = gabi::load<u32>(gabi::ea(names) + 0x10);
  u32 table = offset ? gabi::ea(names) + 0x10 + offset : 0;
  s8 index = gabi::call<s8>(0x027DF9B0, gabi::at<void>(table), STR(0x1000C40C));
  u32 number = (u16)(s16)index;
  u32 size = gabi::load<u32>(gabi::ea(data) + 4),
      joint = gabi::load<u32>(gabi::ea(data) + 8);
  if (number < size)
    joint += number * 0x1C;
  gabi::store<u32>(joint + 8, 0x02109054);
  m = a->model;
  gabi::store<u32>(gabi::ea(m) + 0xB8, gabi::ea(a));
  m = gabi::call<J3DModel *>(0x025E38E0, data2, 0, 0x11020203u);
  a->ballModel = m;
  if (!m)
    return 0;
  canon::Local<be<u32>[2]> textures[8];
  const s16 indices[8] = {13, 14, 17, 15, 16, 11, 12, 18};
  void *resources[8];
  for (int i = 0; i < 2; i++) {
    (*textures[i])[1] = 0x1000C3C4;
    controller = gabi::at<void>(gabi::load<u32>(0x101F4F28));
    (*textures[i])[0] = 0x1000C414;
    resources[i] = gabi::call<void *>(0x026066C4, controller, textures[i].get(),
                                      indices[i]);
  }
  for (int i = 0; i < 10; i++) {
    void *object = gabi::call<void *>(0x0273AD10, 0x2C);
    if (object)
      object = gabi::call<void *>(0x020472CC, object);
    a->bombs[i] = object;
    if (!object)
      return 0;
    if (!gabi::call<BOOL>(0x02047358, object, resources[0], resources[1]))
      return 0;
  }
  for (int i = 2; i < 4; i++) {
    (*textures[i])[1] = 0x1000C3C4;
    controller = gabi::at<void>(gabi::load<u32>(0x101F4F28));
    (*textures[i])[0] = 0x1000C414;
    resources[i] = gabi::call<void *>(0x026066C4, controller, textures[i].get(),
                                      indices[i]);
  }
  // HD fixes the GameCube copy-paste loop: the ship array has five entries.
  for (int i = 0; i < 5; i++) {
    void *object = gabi::call<void *>(0x0273AD10, 0x2C);
    if (object)
      object = gabi::call<void *>(0x020472CC, object);
    a->ships[i] = object;
    if (!object)
      return 0;
    if (!gabi::call<BOOL>(0x02047358, object, resources[2], resources[3]))
      return 0;
  }
  for (int i = 4; i < 8; i++) {
    (*textures[i])[1] = 0x1000C3C4;
    controller = gabi::at<void>(gabi::load<u32>(0x101F4F28));
    (*textures[i])[0] = 0x1000C414;
    resources[i] = gabi::call<void *>(0x026066C4, controller, textures[i].get(),
                                      indices[i]);
  }
  void *object = gabi::call<void *>(0x0273AD10, 0x5C);
  if (object)
    object = gabi::call<void *>(0x0204698C, object);
  a->battery = object;
  if (!object)
    return 0;
  return gabi::call<BOOL>(0x02046A68, object, resources[4], resources[5],
                          resources[6], resources[7]) != 0;
}
VERIFY(0x02109220, canon_heap);
BOOL canon_heap_thunk(daCanon_c *a) {
  WWHD_FUNC(0x02109558, BOOL, a);
  return canon_heap(a);
}
VERIFY(0x02109558, canon_heap_thunk);

void canon_set_matrix(daCanon_c *a) {
  WWHD_FUNC(0x0210955C, void, a);
  f32 scale = gabi::load<f32>(0x1047BC18) + 0.65f;
  f32 y = gabi::fmadds(100.f, scale, (f32)a->current.pos.y);
  gabi::call<void>(0x028E93CC, matrix(), (f32)a->current.pos.x, y,
                   (f32)a->current.pos.z);
  gabi::call<void>(0x025F1C28, matrix(), (s16)((s16)a->shape_angle.y + 0x8000));
  if (firing(a)) {
    f32 angle = (f32)(s16)a->amplitude * sine(a->phaseAngle);
    gabi::call<void>(0x025F1C28, matrix(), (s16)gabi::ftoi(angle));
    angle = (f32)(s16)a->amplitude * cosine(a->phaseAngle);
    gabi::call<void>(0x025F1BF4, matrix(), (s16)gabi::ftoi(angle));
    gabi::call<void>(0x025F24E0, 0.f, (f32)a->recoil, 0.f);
  }
  u32 model = gabi::ea((J3DModel *)a->model);
  gabi::store<f32>(model + 0xBC, scale);
  gabi::store<f32>(model + 0xC0, scale);
  gabi::store<f32>(model + 0xC4, scale);
  model = gabi::ea((J3DModel *)a->model);
  mtx_copy(gabi::at<Mtx34>(model + 0xC8), matrix());
  gabi::call<void>(0x028E90D4, matrix(), &a->baseMatrix);
}
VERIFY(0x0210955C, canon_set_matrix);
void canon_wait_init(daCanon_c *a) {
  WWHD_FUNC(0x021097B0, void, a);
  gabi::store<u32>(gabi::ea(a) + 0x39C, 10);
  set_process(a, 0x0210AAD4);
}
VERIFY(0x021097B0, canon_wait_init);
void canon_ball_matrix(daCanon_c *a, cXyz *p) {
  WWHD_FUNC(0x02109FF0, void, a, p);
  u32 model = gabi::ea((J3DModel *)a->ballModel);
  gabi::store<f32>(model + 0xBC, 1.f);
  gabi::store<f32>(model + 0xC4, 1.f);
  gabi::store<f32>(model + 0xC0, 1.f);
  gabi::call<void>(0x028E93CC, matrix(), (f32)p->x, (f32)p->y, (f32)p->z);
  model = gabi::ea((J3DModel *)a->ballModel);
  mtx_copy(gabi::at<Mtx34>(model + 0xC8), matrix());
}
VERIFY(0x02109FF0, canon_ball_matrix);
void canon_grid_position(daCanon_c *a, cXyz *out, s32 column, s32 row) {
  WWHD_FUNC(0x0210A0B8, void, a, out, column, row);
  s16 angle = (s16)((s16)a->home.angle.y +
                    gabi::load<s16>(0x1000C3FC + 2u * (u32)column));
  f32 distance =
      gabi::fmadds((f32)row, 1500.f, 10500.f) + gabi::load<f32>(0x1047BBDC);
  f32 x = gabi::fmadds(distance, sine(angle), (f32)a->current.pos.x),
      z = gabi::fmadds(distance, cosine(angle), (f32)a->current.pos.z);
  f32 y = gabi::call<f32>(0x0246BA0C, x, z);
  if (!out)
    out = gabi::call<cXyz *>(0x0273AD10, 12);
  if (out) {
    out->y = y;
    out->z = z;
    out->x = x;
  }
}
VERIFY(0x0210A0B8, canon_grid_position);
void canon_ball_endpoint(daCanon_c *a, cXyz *out, s16 yaw, s16 pitch) {
  WWHD_FUNC(0x0210A1EC, void, a, out, yaw, pitch);
  f32 ratio =
      (f32)((s32)(s16)a->home.angle.x + 0x1000 - pitch) * 0.000244140625f;
  f32 distance =
      gabi::fmadds(ratio * 1500.f, 7.f, 10500.f) + gabi::load<f32>(0x1047BBD8);
  f32 x = gabi::fmadds(distance, sine(yaw), (f32)a->current.pos.x),
      z = gabi::fmadds(distance, cosine(yaw), (f32)a->current.pos.z);
  f32 y = gabi::call<f32>(0x0246BA0C, x, z);
  if (!out)
    out = gabi::call<cXyz *>(0x0273AD10, 12);
  if (out) {
    out->y = y;
    out->z = z;
    out->x = x;
  }
}
VERIFY(0x0210A1EC, canon_ball_endpoint);
BOOL canon_create_check(daCanon_c *a, s32 column, s32 row, s32 count) {
  WWHD_FUNC(0x0210A31C, BOOL, a, column, row, count);
  for (s32 n = count; n != 0;) {
    n = (s32)((u32)n - 1);
    if ((u32)column == gabi::load<u8>(gabi::ea(a) + 0x70C + 2u * (u32)n))
      return 0;
  }
  u32 cell = gabi::ea(a) + 0x70C + 2u * (u32)count;
  gabi::store<u8>(cell, (u8)column);
  gabi::store<u8>(cell + 1, (u8)row);
  return 1;
}
VERIFY(0x0210A31C, canon_create_check);
void canon_create_targets(daCanon_c *a) {
  WWHD_FUNC(0x0210A380, void, a);
  for (int i = 0; i < 5;) {
    s32 column = gabi::ftoi(gabi::call<f32>(0x020198D8, 8.f));
    s32 row = gabi::ftoi(gabi::call<f32>(0x020198D8, 8.f));
    if (!canon_create_check(a, column, row, i))
      continue;
    canon::Local<cXyz> grid, position;
    canon_grid_position(a, grid.get(), column, row);
    position->copy(*grid);
    u32 id = gabi::load<u32>(gabi::ea(a) + 4);
    u32 child = gabi::call<u32>(
        0x025D5A20, 0x1C9, id, 0x037F003Fu, position.get(), -1, (void *)nullptr,
        (void *)nullptr, -1, gabi::at<void>(0x02109160));
    a->targetIds[i] = child;
    i++;
  }
}
VERIFY(0x0210A380, canon_create_targets);
Pair32 canon_break_all(daCanon_c *a) {
  WWHD_FUNC(0x0210A494, Pair32, a);
  return gabi::call<Pair32>(0x025D5218, gabi::at<void>(0x02109174), a); /* fpcM_Search tail call: r3:r4 */
}
VERIFY(0x0210A494, canon_break_all);
void canon_game_start_init(daCanon_c *a) {
  WWHD_FUNC(0x0210AA30, void, a);
  a->timer = 60;
  a->shape_angle.y = a->home.angle.y;
  a->shape_angle.x = 0x2AAA;
  void *npc = gabi::at<void>(gabi::load<u32>(0x101D5F24));
  if (npc)
    sound(npc, 0x8A8);
  set_process(a, 0x0210AF4C);
}
VERIFY(0x0210AA30, canon_game_start_init);
void canon_game_init(daCanon_c *a) {
  WWHD_FUNC(0x0210AD20, void, a);
  set_process(a, 0x0210AE44);
}
VERIFY(0x0210AD20, canon_game_init);
void canon_game_start(daCanon_c *a) {
  WWHD_FUNC(0x0210AF4C, void, a);
  s16 timer = a->timer;
  a->timer = (s16)(timer - 1);
  if (timer < 0)
    canon_game_init(a);
}
VERIFY(0x0210AF4C, canon_game_start);
void canon_fire_init(daCanon_c *a) {
  WWHD_FUNC(0x0210AD40, void, a);
  f32 ratio =
      (f32)((s32)(s16)a->home.angle.x + 0x1000 - (s16)a->shape_angle.x) *
      0.000244140625f;
  f32 frames = gabi::fmadds(ratio * 8.f, 7.f, 64.f);
  a->process.target = 0x0210B228;
  a->recoil = -20.f;
  a->process.delta = 0;
  a->process.slot = -1;
  s16 duration = (s16)gabi::ftoi(frames);
  s16 remaining = (s16)((s16)a->remaining - 1);
  a->flight = duration;
  a->duration = duration;
  a->phaseAngle = 0;
  a->amplitude = 0x400;
  a->remaining = remaining;
  u32 icon = gabi::load<u32>(gabi::ea(a) + 0x69C + 4u * (u32)(s32)remaining);
  gabi::store<u8>(icon + 0x28, 1);
  u32 root = gabi::load<u32>(0x101F8344);
  u32 ui = gabi::load<u32>(root + 0x208);
  gabi::call<void>(0x02646764, gabi::at<void>(ui), 10 - (s16)a->remaining);
}
VERIFY(0x0210AD40, canon_fire_init);
void canon_local_delete(void *object, u32 flags) {
  WWHD_FUNC(0x0210B7D0, void, object, flags);
  if (object && (flags & 1))
    gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x0210B7D0, canon_local_delete);
BOOL canon_is_delete(void *a) {
  WWHD_FUNC(0x0210B7E4, BOOL, a);
  return 1;
}
VERIFY(0x0210B7E4, canon_is_delete);

static void construct_status(daCanon_c *a, u32 offset) {
  u32 addr = gabi::ea(a) + offset;
  gabi::call<void>(0x0200BD2C, gabi::at<void>(addr));
  gabi::call<void>(0x02515DA0, gabi::at<void>(addr + 0x1C));
  gabi::store<u32>(addr + 0x18, 0x1004AE88);
  gabi::store<u32>(addr + 0x1C, 0x1004AEC0);
}
static void construct_cylinder(daCanon_c *a, u32 offset) {
  u32 addr = gabi::ea(a) + offset;
  gabi::call<void>(0x02515FB8, gabi::at<void>(addr));
  gabi::store<u32>(addr + 0x114, 0x100015A8);
  gabi::store<u32>(addr + 0x110, 0x1000C3DC);
  gabi::call<void>(0x02018590, gabi::at<void>(addr + 0x118));
  gabi::store<u32>(addr + 0x3C, 0x1004B108);
  gabi::store<u32>(addr + 0x114, 0x1004B160);
  gabi::store<u32>(addr + 0x12C, 0x1004B150);
}
s32 canon_create(daCanon_c *a) {
  WWHD_FUNC(0x021097D8, s32, a);
  u32 condition = gabi::load<u32>(gabi::ea(a) + 0x2E4);
  if (!(condition & 8)) {
    if (a) {
      gabi::call<void>(0x025D4ED0, a);
      gabi::store<u32>(gabi::ea(a) + 0xB4, 0x1000C3EC);
      construct_status(a, 0x3B4);
      construct_cylinder(a, 0x3F0);
      construct_status(a, 0x520);
      construct_cylinder(a, 0x55C);
      condition = gabi::load<u32>(gabi::ea(a) + 0x2E4);
    }
    gabi::store<u32>(gabi::ea(a) + 0x2E4, condition | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, a->phase, STR(0x1000C520));
  if (phase == 4) {
    if (!gabi::call<BOOL>(0x025D63E8, a, gabi::at<void>(0x02109558), 0x62E0))
      return 5;
    canon_set_matrix(a);
    gabi::call<void>(0x028E90D4, matrix(), &a->baseMatrix);
    u32 model = gabi::ea((J3DModel *)a->model);
    gabi::store<u32>(gabi::ea(a) + 0x348, model ? model + 0xC8 : 0);
    model = gabi::ea((J3DModel *)a->model);
    gabi::call<void>(0x028E90D4, gabi::at<Mtx34>(model ? model + 0xC8 : 0),
                     &a->baseMatrix);
    gabi::call<void>(0x02515F14, &a->status1, 255, 255, a);
    gabi::call<void>(0x02516518, &a->collision1, gabi::at<void>(0x101B3A08));
    gabi::store<u32>(gabi::ea(&a->collision1) + 0x44, gabi::ea(&a->status1));
    gabi::call<void>(0x02516518, &a->collision2, gabi::at<void>(0x101B3A4C));
    gabi::store<u32>(gabi::ea(&a->collision2) + 0x44, gabi::ea(&a->status1));
    a->shape_angle.x = 0x2AAA;
    a->home.angle.x = 0x2000;
    canon_wait_init(a);
    s16 pitch = a->shape_angle.x, yaw = a->shape_angle.y;
    f32 x = a->current.pos.x, y = a->current.pos.y, z = a->current.pos.z;
    a->unused1 = 0;
    a->unused2 = 3;
    a->muzzle.z = z;
    a->muzzle.y = y;
    a->muzzle.x = x;
    a->muzzle.x = gabi::fmadds(200.f * sine(pitch), sine(yaw), x);
    a->muzzle.z = gabi::fmadds(200.f * sine(pitch), cosine(yaw), z);
    a->muzzleAngle.x = gabi::load<s16>(0x101FFB14);
    a->muzzleAngle.y = gabi::load<s16>(0x101FFB16);
    a->muzzleAngle.z = gabi::load<s16>(0x101FFB18);
    a->muzzleAngle.y = yaw;
    gabi::store<u32>(0x101D5F20, gabi::ea(a));
  }
  return phase;
}
VERIFY(0x021097D8, canon_create);
BOOL canon_delete(daCanon_c *a) {
  WWHD_FUNC(0x02109A78, BOOL, a);
  gabi::call<void>(0x025204C8, a->phase, STR(0x1000C520));
  u32 p = play();
  if (gabi::load<u8>(p + 0x5CEA) == 3) {
    p = play();
    finish_game(p);
  }
  gabi::call<void>(0x025E1B34, &a->ball);
  gabi::store<u32>(0x101D5F20, 0);
  return 1;
}
VERIFY(0x02109A78, canon_delete);
BOOL canon_execute(daCanon_c *a) {
  WWHD_FUNC(0x02109AF8, BOOL, a);
  if ((u8)a->effectActive) {
    gabi::call<void>(0x025553B8, 0, 0, 0, 0.f);
    gabi::call<void>(0x0255BA9C, gabi::at<void>(0x10462CA0));
    gabi::call<void>(0x0257D1B8, gabi::at<void>(0x10462CC4));
    a->effectActive = 0;
  }
  canon_set_matrix(a);
  s16 slot = a->process.slot, delta = a->process.delta;
  u32 thisAddr = gabi::ea(a) + (u32)(s32)delta;
  u32 target;
  if (slot < 0)
    target = a->process.target;
  else {
    u32 vptr = gabi::load<u32>(thisAddr +
                               (u32)(s32)gabi::load<s16>(gabi::ea(a) + 0x3B2));
    target = gabi::load<u32>(vptr + 8u * (u32)(s32)slot + 4);
  }
  gabi::call<void>(target, gabi::at<void>(thisAddr));
  canon::Local<cXyz> center;
  f32 x = a->current.pos.x, z = a->current.pos.z;
  s16 yaw = a->shape_angle.y;
  f32 distance = gabi::load<f32>(0x1047BBE0);
  f32 xx = gabi::fmadds(distance, sine(yaw), x),
      zz = gabi::fmadds(distance, cosine(yaw), z);
  center->y = a->current.pos.y;
  center->x = xx;
  center->z = zz;
  gabi::call<void>(0x020182E0, gabi::at<void>(gabi::ea(&a->collision1) + 0x118),
                   center.get());
  f32 radius = gabi::load<f32>(0x1047BBE4) + 75.f;
  gabi::call<void>(0x020184DC, gabi::at<void>(gabi::ea(&a->collision1) + 0x118),
                   radius);
  u32 p = play();
  gabi::call<void>(0x0200E240, gabi::at<void>(p + 0x26A4), &a->collision1);
  return 0;
}
VERIFY(0x02109AF8, canon_execute);
void canon_draw_info(daCanon_c *a) {
  WWHD_FUNC(0x02109CCC, void, a);
  for (int i = 0; i < 10; i++) {
    f32 y0 = gabi::load<f32>(0x1047BBC8) + 368.f,
        step = gabi::load<f32>(0x1047BBCC) - 27.f;
    f32 x = gabi::load<f32>(0x1047BBC4) + 30.f,
        y = gabi::fmadds(step, (f32)i, y0);
    u32 icon = gabi::ea((void *)a->bombs[i]);
    gabi::store<f32>(icon + 0xC, x);
    gabi::store<f32>(icon + 0x10, y);
    f32 scale = gabi::load<f32>(0x1047BC00) + 0.8f;
    icon = gabi::ea((void *)a->bombs[i]);
    gabi::store<f32>(icon + 0x24, scale);
    void *object = a->bombs[i];
    display_list(object);
  }
  for (int i = 0; i < 5; i++) {
    f32 y0 = gabi::load<f32>(0x1047BBD4) + 115.f,
        step = gabi::load<f32>(0x1047BBD8) + 47.f;
    f32 x = gabi::load<f32>(0x1047BBD0) + 540.f,
        y = gabi::fmadds(step, (f32)i, y0);
    u32 icon = gabi::ea((void *)a->ships[i]);
    gabi::store<f32>(icon + 0xC, x);
    gabi::store<f32>(icon + 0x10, y);
    f32 scale = gabi::load<f32>(0x1047BC04) + 1.f;
    icon = gabi::ea((void *)a->ships[i]);
    gabi::store<f32>(icon + 0x24, scale);
    void *object = a->ships[i];
    display_list(object);
  }
  f32 angle = (f32)(s16)(0x4000 - (s16)a->shape_angle.x) * 0.0054931640625f;
  gabi::call<void>(0x020470F4, (void *)a->battery, angle);
  void *object = a->battery;
  display_list(object);
}
VERIFY(0x02109CCC, canon_draw_info);
BOOL canon_draw(daCanon_c *a) {
  WWHD_FUNC(0x02109F18, BOOL, a);
  render_light(a, &a->current.pos, false);
  if (firing(a))
    render_light(a, &a->ball, true);
  u32 p = play();
  if (gabi::load<u8>(p + 0x5CEA) == 3)
    canon_draw_info(a);
  return 1;
}
VERIFY(0x02109F18, canon_draw);
void canon_pad_move(daCanon_c *a) {
  WWHD_FUNC(0x0210A4A4, void, a);
  f32 x = gabi::call<f32>(0x0200796C, 0), y = gabi::call<f32>(0x02007990, 0);
  u32 save = gabi::load<u32>(0x101F84DC);
  s16 yaw = a->shape_angle.y, pitch = a->shape_angle.x;
  void *options = gabi::call<void *>(0x027200D0, gabi::at<void>(save + 0x12C0));
  if (gabi::load<u8>(gabi::ea(options) + 2) == 0)
    y = -y;
  if (!(x <= 0.5f)) {
    s16 step = (s16)gabi::ftoi(1024.f * (0.5f - x));
    s16 home = a->home.angle.y, limit = (s16)(home - 0x1200);
    s16 current = a->shape_angle.y;
    yaw = (s16)(yaw + step);
    if (current >= limit && yaw < limit)
      yaw = limit;
  } else if (!(x >= -0.5f)) {
    s16 step = (s16)gabi::ftoi(1024.f * (-0.5f - x));
    s16 home = a->home.angle.y, limit = (s16)(home + 0x1200);
    s16 current = a->shape_angle.y;
    yaw = (s16)(yaw + step);
    if (current <= limit && yaw > limit)
      yaw = limit;
  }
  if (!(y <= 0.5f)) {
    s16 home = a->home.angle.x;
    f32 raw = 512.f * (y - 0.5f);
    s16 current = a->shape_angle.x;
    s16 limit = (s16)(home + 0x1000);
    pitch = (s16)(pitch + (s16)gabi::ftoi(raw));
    if (current <= limit && pitch > limit)
      pitch = limit;
  } else if (!(y >= -0.5f)) {
    f32 raw = 512.f * (y + 0.5f);
    s16 home = a->home.angle.x, current = a->shape_angle.x;
    pitch = (s16)(pitch + (s16)gabi::ftoi(raw));
    if (current >= home && pitch < home)
      pitch = home;
  }
  s16 yd = gabi::call<s16>(0x0200F378, &a->shape_angle.y, yaw, 4, 0x100, 0x10);
  s16 pd =
      gabi::call<s16>(0x0200F378, &a->shape_angle.x, pitch, 4, 0x100, 0x10);
  if (yd || pd) {
    sound(a, 0x2051);
    f32 angle = (f32)(s16)(0x4000 - (s16)a->shape_angle.x) * 0.0054931640625f;
    u32 root = gabi::load<u32>(0x101F8344);
    u32 ui = gabi::load<u32>(root + 0x208);
    gabi::call<void>(0x026467E0, gabi::at<void>(ui), angle);
  }
}
VERIFY(0x0210A4A4, canon_pad_move);
void canon_make_effect(daCanon_c *a, cXyz *position, csXyz *rotation,
                       s32 kind) {
  WWHD_FUNC(0x0210A7A4, void, a, position, rotation, kind);
  u32 p = play();
  s8 cameraId = gabi::load<s8>(p + 0x5B30);
  p = play();
  u32 camera = gabi::load<u32>(p + 0x5AF8 + (u32)((s32)cameraId * 0x34));
  canon::Local<cXyz> scale;
  scale->set(0.75f, 0.75f, 0.75f);
  canon::Local<csXyz> view;
  view->x = (s16)-gabi::load<s16>(camera + 0x234);
  view->z = 0;
  view->y = (s16)(gabi::load<s16>(camera + 0x236) + 0x8000);
  p = play();
  void *particles = gabi::at<void>(gabi::load<u32>(p + 0x5AB0));
  if (kind == 0)
    gabi::call<void>(0x025A866C, particles, 0x200A, position, rotation,
                     scale.get(), 255);
  else {
    gabi::call<void>(0x025A847C, particles, 0, 0xB, position, view.get(),
                     scale.get(), 255, 0, -1, (void *)nullptr, (void *)nullptr,
                     (void *)nullptr);
    p = play();
    particles = gabi::at<void>(gabi::load<u32>(p + 0x5AB0));
    gabi::call<void>(0x025A866C, particles, 0x2009, position, rotation,
                     scale.get(), 255);
    p = play();
    particles = gabi::at<void>(gabi::load<u32>(p + 0x5AB0));
    gabi::call<void>(0x025A866C, particles, 0x200A, position, rotation,
                     scale.get(), 255);
    p = play();
    particles = gabi::at<void>(gabi::load<u32>(p + 0x5AB0));
    gabi::call<void>(0x025A847C, particles, 2, 0x2008, position, rotation,
                     scale.get(), 255, 0, -1, (void *)nullptr, (void *)nullptr,
                     (void *)nullptr);
  }
  u32 light = 0x10462CA0;
  gabi::store<f32>(light, (f32)position->x);
  f32 y = position->y;
  gabi::store<f32>(light + 4, y);
  f32 z = position->z;
  gabi::store<u16>(light + 0xE, 200);
  gabi::store<f32>(light + 8, z);
  gabi::store<f32>(light + 4, y + 100.f);
  gabi::store<u16>(light + 0xC, 200);
  gabi::store<u16>(light + 0x10, 160);
  gabi::store<f32>(light + 0x14, 600.f);
  gabi::store<f32>(light + 0x18, 100.f);
  gabi::call<void>(0x0255B9C8, gabi::at<void>(light));
  u32 wind = 0x10462CC4;
  gabi::store<u32>(wind, gabi::load<u32>(gabi::ea(position)));
  gabi::store<u32>(wind + 4, gabi::load<u32>(gabi::ea(position) + 4));
  gabi::store<u32>(wind + 8, gabi::load<u32>(gabi::ea(position) + 8));
  gabi::store<f32>(wind + 0xC, 0.f);
  gabi::store<f32>(wind + 0x14, 0.f);
  gabi::store<f32>(wind + 0x18, 1000.f);
  gabi::store<f32>(wind + 0x10, 1.f);
  gabi::store<f32>(wind + 0x1C, 1.f);
  gabi::store<f32>(wind + 0x20, 0.f);
  gabi::call<void>(0x0257DC90, gabi::at<void>(wind));
  a->effectActive = 1;
  sound(a, 0x6901);
  p = play();
  canon::Local<cXyz> shock;
  shock->set(0.f, 1.f, 0.f);
  gabi::call<void>(0x025CB374, gabi::at<void>(p + 0x599C), 7, -33, shock.get());
}
VERIFY(0x0210A7A4, canon_make_effect);
void canon_wait(daCanon_c *a) {
  WWHD_FUNC(0x0210AAD4, void, a);
  u32 p = play();
  void *player = gabi::at<void>(gabi::load<u32>(p + 0x5B2C));
  canon::Local<cXyz> difference, flat;
  gabi::call<void>(0x0201ADE0, gabi::at<cXyz>(gabi::ea(player) + 0x314),
                   difference.get(), &a->current.pos);
  flat->y = 0.f;
  flat->x = difference->x;
  flat->z = difference->z;
  f32 magnitude = gabi::call<f32>(0x028E8DD0, flat.get());
  gabi::call<f32>(0x028F4384, magnitude);
  p = play();
  s32 staff = gabi::call<s32>(0x02542D88, gabi::at<void>(p + 0x52C4),
                              STR(0x1000C4D0), 0, 0);
  if (staff == -1)
    return;
  p = play();
  s32 act = gabi::call<s32>(0x02542EDC, gabi::at<void>(p + 0x52C4), staff,
                            gabi::at<void>(0x101B3A90), 1, 1, 0);
  p = play();
  if (act != 0) {
    gabi::call<void>(0x02543280, gabi::at<void>(p + 0x52C4), staff);
    return;
  }
  play();
  canon::Local<cXyz> position;
  position->copy(a->current.pos);
  p = play();
  player = gabi::at<void>(gabi::load<u32>(p + 0x5B2C));
  canon_create_targets(a);
  s16 yaw = a->home.angle.y;
  a->hits = 0;
  a->remaining = 10;
  position->x = gabi::fnmsubs(200.f, sine(yaw), (f32)position->x);
  position->z = gabi::fnmsubs(200.f, cosine(yaw), (f32)position->z);
  u32 vtable = gabi::load<u32>(gabi::ea(player) + 0xB4);
  u32 target = gabi::load<u32>(vtable + 0x114);
  gabi::call<void>(target, player, position.get(), yaw);
  p = play();
  u16 flags = gabi::load<u16>(p + 0x5CE8);
  gabi::store<u8>(p + 0x5CEA, 3);
  gabi::store<u16>(p + 0x5CE8, flags | 4);
  f32 angle = (f32)(s16)(0x4000 - (s16)a->shape_angle.x) * 0.0054931640625f;
  u32 root = gabi::load<u32>(0x101F8344), ui = gabi::load<u32>(root + 0x208);
  gabi::store<f32>(ui + 0x58, angle);
  gabi::call<void>(0x020063C0, gabi::at<void>(ui + 0x18),
                   gabi::at<void>(0x10490FFC));
  for (int i = 0; i < 10; i++)
    gabi::store<u8>(gabi::ea((void *)a->bombs[i]) + 0x28, 0);
  for (int i = 0; i < 5; i++)
    gabi::store<u8>(gabi::ea((void *)a->ships[i]) + 0x28, 0);
  canon_game_start_init(a);
}
VERIFY(0x0210AAD4, canon_wait);
void canon_game(daCanon_c *a) {
  WWHD_FUNC(0x0210AE44, void, a);
  canon_pad_move(a);
  if (!gabi::call<BOOL>(0x02007898, 0))
    return;
  canon::Local<cXyz> launch;
  s16 pitch = a->shape_angle.x, yaw = a->shape_angle.y;
  f32 reach = 200.f * sine(pitch);
  launch->x = gabi::fmadds(reach, sine(yaw), (f32)a->current.pos.x);
  launch->y = gabi::fmadds(200.f, cosine(pitch), (f32)a->current.pos.y);
  launch->z = gabi::fmadds(reach, cosine(yaw), (f32)a->current.pos.z);
  canon_make_effect(a, launch.get(), &a->shape_angle, 0);
  a->launch.copy(*launch);
  sound(a, 0x2852);
  canon_fire_init(a);
}
VERIFY(0x0210AE44, canon_game);
void canon_pause_init(daCanon_c *a) {
  WWHD_FUNC(0x0210AF64, void, a);
  a->timer = 30;
  if (gabi::call<BOOL>(0x025160DC, &a->collision2)) {
    void *target = gabi::call<void *>(
        0x02515BBC, gabi::at<void>(gabi::ea(&a->collision2) + 0x50));
    if (target && gabi::load<s16>(gabi::ea(target) + 0xE) == 0x1C9) {
      s16 hits = a->hits;
      u32 icon = gabi::load<u32>(gabi::ea(a) + 0x6C4 + 4u * (u32)(s32)hits);
      gabi::store<u8>(icon + 0x28, 1);
      canon::Local<csXyz> rotation;
      rotation->x = gabi::load<s16>(0x101FFB14);
      rotation->y = gabi::load<s16>(0x101FFB16);
      rotation->z = gabi::load<s16>(0x101FFB18);
      canon_make_effect(a, &a->ball, rotation.get(), 1);
      hits = (s16)((s16)a->hits + 1);
      a->hits = hits;
      u32 root = gabi::load<u32>(0x101F8344);
      u32 ui = gabi::load<u32>(root + 0x208);
      gabi::call<void>(0x0264679C, gabi::at<void>(ui), hits);
      s16 result = a->hits;
      u32 p = play();
      gabi::store<u16>(p + 0x5CEC, (u16)result);
      p = play();
      canon::Local<cXyz> direction;
      direction->y = 0.f;
      s16 yaw = a->shape_angle.y;
      direction->x = sine(yaw);
      direction->z = cosine(yaw);
      gabi::call<void>(0x025CB374, gabi::at<void>(p + 0x599C), 7, 0x3E,
                       direction.get());
      void *npc = gabi::at<void>(gabi::load<u32>(0x101D5F24));
      if (npc) {
        if ((s16)a->hits == 5)
          sound(npc, 0x8AB);
        else if ((s16)a->remaining == 0)
          sound(npc, 0x8AC);
        else
          sound(npc, 0x8AA);
      }
    }
    gabi::call<void>(0x02516094, &a->collision2);
  } else if ((s16)a->remaining == 0) {
    void *npc = gabi::at<void>(gabi::load<u32>(0x101D5F24));
    if (npc)
      sound(npc, 0x8AC);
  }
  set_process(a, 0x0210B5A4);
}
VERIFY(0x0210AF64, canon_pause_init);
void canon_fire(daCanon_c *a) {
  WWHD_FUNC(0x0210B228, void, a);
  gabi::call<void>(0x0200F428, &a->amplitude, 0, 4, 0x100);
  a->phaseAngle = (s16)((s16)a->phaseAngle + 0x3800);
  gabi::call<void>(0x0200ED84, &a->recoil, 0.f, 0.25f, 5.f);
  canon::Local<cXyz> endpoint;
  canon_ball_endpoint(a, endpoint.get(), (s16)a->shape_angle.y,
                      (s16)a->shape_angle.x);
  s16 old = a->flight;
  a->endpoint.copy(*endpoint);
  a->flight = (s16)(old - 1);
  if (old != 0 && !gabi::call<BOOL>(0x025160DC, &a->collision2)) {
    f32 ratio = (f32)(s16)a->flight / (f32)(s16)a->duration;
    f32 centered = ratio - 0.5f;
    f32 arc = gabi::fnmsubs(centered * centered, 4.f, 1.f);
    canon::Local<cXyz> difference, flat, start, end, result;
    gabi::call<void>(0x0201ADE0, &a->endpoint, difference.get(), &a->launch);
    s16 angle = (s16)(0x4000 - (s16)a->shape_angle.x);
    flat->y = 0.f;
    flat->z = difference->z;
    flat->x = difference->x;
    f32 length = gabi::call<f32>(0x028E8DD0, flat.get());
    length = gabi::call<f32>(0x028F4384, length);
    f32 height = (0.25f * length) * (sine(angle) / cosine(angle));
    gabi::call<void>(0x0201AE48, &a->launch, start.get(), ratio);
    gabi::call<void>(0x0201AE48, &a->endpoint, end.get(), 1.f - ratio);
    gabi::call<void>(0x0201AD78, start.get(), result.get(), end.get());
    f32 y = gabi::fmadds(height, arc, (f32)result->y);
    a->ball.x = result->x;
    a->ball.y = y;
    a->ball.z = result->z;
    canon_ball_matrix(a, &a->ball);
    gabi::call<void>(
        0x020182E0, gabi::at<void>(gabi::ea(&a->collision2) + 0x118), &a->ball);
    u32 p = play();
    gabi::call<void>(0x0200E240, gabi::at<void>(p + 0x26A4), &a->collision2);
    s32 reverb = gabi::call<s32>(0x02520540, (s8)a->current.roomNo);
    gabi::call<void>(0x025E1A40, 0x381F, &a->ball, 0, reverb);
  } else {
    gabi::call<void>(0x025E1AF8, &a->ball);
    gabi::call<void>(0x025DAE64, &a->ball, 2.f, 1.f, 1);
    canon_pause_init(a);
  }
}
VERIFY(0x0210B228, canon_fire);
void canon_end_init(daCanon_c *a) {
  WWHD_FUNC(0x0210B524, void, a);
  s16 hits = a->hits;
  a->process.slot = -1;
  a->process.target = 0x0210B5D8;
  a->timer = 60;
  a->process.delta = 0;
  u32 p = play();
  u8 result = hits == 5 ? 1 : 2;
  gabi::store<u8>(p + 0x5CEE, result);
  gabi::store<u8>(0x101D5F42, result);
}
VERIFY(0x0210B524, canon_end_init);
void canon_pause(daCanon_c *a) {
  WWHD_FUNC(0x0210B5A4, void, a);
  s16 timer = a->timer;
  a->timer = (s16)(timer - 1);
  if (timer < 0) {
    if ((s16)a->remaining > 0 && (s16)a->hits < 5)
      canon_game_init(a);
    else
      canon_end_init(a);
  }
}
VERIFY(0x0210B5A4, canon_pause);
void canon_end(daCanon_c *a) {
  WWHD_FUNC(0x0210B5D8, void, a);
  s16 timer = a->timer;
  if (timer > 0) {
    if (timer == 30) {
      canon_break_all(a);
      timer = a->timer;
    }
    a->timer = (s16)(timer - 1);
    return;
  }
  u32 p = play();
  s32 staff = gabi::call<s32>(0x02542D88, gabi::at<void>(p + 0x52C4),
                              STR(0x1000C4FC), 0, 0);
  if (staff != -1) {
    p = play();
    s32 act = gabi::call<s32>(0x02542EDC, gabi::at<void>(p + 0x52C4), staff,
                              gabi::at<void>(0x101B3A94), 1, 1, 0);
    if (act != 0)
      return;
    p = play();
    gabi::call<void>(0x02543280, gabi::at<void>(p + 0x52C4), staff);
    p = play();
    if (gabi::load<u8>(p + 0x5CEA) == 3) {
      p = play();
      finish_game(p);
      u32 root = gabi::load<u32>(0x101F8344),
          ui = gabi::load<u32>(root + 0x208);
      gabi::call<void>(0x020063C0, gabi::at<void>(ui + 0x18),
                       gabi::at<void>(0x1049102C));
    }
  }
  canon_wait_init(a);
}
VERIFY(0x0210B5D8, canon_end);
void canon_static_init() {
  WWHD_FUNC(0x0210B6FC, void);
  gabi::store<u32>(0x10462C9C, 0);
  gabi::store<u32>(0x10462C98, 0);
  gabi::store<u32>(0x10462C94, 0);
  gabi::store<u32>(0x10462C90, 0);
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101B3A98));
  gabi::store<f32>(0x10462C74, gabi::load<f32>(0x1000C510));
  gabi::store<f32>(0x10462C78, gabi::load<f32>(0x1000C514));
  gabi::call<void>(0x028ED6F8, gabi::at<void>(0x10462C8C));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101B3AA4));
  gabi::call<void>(0x028EAB2C, gabi::at<void>(0x10462C8D));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101B3AB0));
  gabi::store<f32>(0x10462CC0, 1.f);
  gabi::store<f32>(0x10462C88, 10000.f);
  gabi::store<f32>(0x10462C80, 50000.f);
  gabi::store<f32>(0x10462C7C, 50000.f);
  gabi::store<f32>(0x10462C84, 10000.f);
}
VERIFY(0x0210B6FC, canon_static_init);

// These tail helpers are owned by the actor and SafeString vtables above.
void canon_destruct(daCanon_c *a, u32 flags) {
  WWHD_FUNC(0x0210B7EC, void, a, flags);
  if (a) {
    gabi::call<void>(0x02515A70, &a->collision2, 2);
    gabi::call<void>(0x02515860, &a->status2, 2);
    gabi::call<void>(0x02515A70, &a->collision1, 2);
    gabi::call<void>(0x02515860, &a->status1, 2);
    gabi::call<void>(0x025D50BC, a, 0);
    if (flags & 1)
      gabi::call<void>(0x0273AF40, a);
  }
}
VERIFY(0x0210B7EC, canon_destruct);

void canon_safe_string_empty(void *object) {
  WWHD_FUNC(0x0210B870, void, object);
}
VERIFY(0x0210B870, canon_safe_string_empty);

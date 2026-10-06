#include "d/actor/d_a_obj_hha.h"
#include "gabi.h"
using Actor = daObjHha_c;
static u32 rd(u32 p) { return gabi::load<u32>(p); }
static s16 sh(u32 p) { return gabi::load<s16>(p); }
static u16 uh(u32 p) { return gabi::load<u16>(p); }
static u8 byte(u32 p) { return gabi::load<u8>(p); }
static f32 lf(u32 p) { return gabi::load<f32>(p); }
static void put(u32 p, u32 v) { gabi::store<u32>(p, v); }
static void ss(u32 p, s32 v) { gabi::store<s16>(p, v); }
static void sb(u32 p, u32 v) { gabi::store<u8>(p, v); }
static void sf(u32 p, f32 v) { gabi::store<f32>(p, v); }
static void words(u32 src, u32 dst, u32 count) {
  for (u32 i = 0; i < count; ++i)
    put(dst + i * 4, rd(src + i * 4));
}
static void vector_float_copy(u32 src, u32 dst) {
  f32 x = lf(src), y = lf(src + 4), z = lf(src + 8);
  sf(dst, x);
  sf(dst + 4, y);
  sf(dst + 8, z);
}
static void angle_copy(u32 src, u32 dst) {
  ss(dst, sh(src));
  ss(dst + 2, sh(src + 2));
  ss(dst + 4, sh(src + 4));
}
static void matrix_copy(u32 src, u32 dst) {
  f32 m[12];
  for (u32 i = 0; i < 12; ++i)
    m[i] = lf(src + i * 4);
  for (u32 i = 0; i < 12; ++i)
    sf(dst + i * 4, m[i]);
}
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u32 resource(u32 name, s32 index) {
  gabi::Local<u32[2]> s;
  put(gabi::ea(s.get()), name);
  put(gabi::ea(s.get()) + 4, 0x1002A348);
  u32 ctl = rd(0x101F4F28);
  return gabi::call<u32>(0x026066C4, ctl, gabi::ea(s.get()), index);
}
static void assertion(u32 line, u32 condition) {
  gabi::call(0x0273AA24, 0x1002A428u, line, condition);
}
static BOOL switch_on(u32 self) {
  u32 save = rd(0x101F84DC);
  return gabi::call<BOOL>(0x025BA0C0, save + 0x20, rd(self + 0xB6C),
                          (s8)byte(self + 0x2FE));
}
void get_offset(void *destination, u32 yaw) {
  WWHD_FUNC(0x02353924, void, destination, yaw);
  u32 dest = gabi::ea(destination);
  f32 scale = lf(0x101D5F28) * -190.f;
  gabi::Local<cXyz> vec;
  vector_float_copy(0x101FFBCC, gabi::ea(vec.get()));
  gabi::call(0x025F1884, 0x1048D0CCu, yaw);
  gabi::call(0x028E8F64, 0x1048D0CCu, 0x101FFBCCu, gabi::ea(vec.get()));
  gabi::call(0x028E8E64, gabi::ea(vec.get()), gabi::ea(vec.get()), scale);
  if (!dest)
    dest = gabi::call<u32>(0x0273AD10, 12);
  if (dest)
    vector_float_copy(gabi::ea(vec.get()), dest);
}
VERIFY(0x02353924, get_offset);
void part_init(void *part, f32 initial, f32 target, u16 frames, u8 index,
               u8 middle) {
  WWHD_FUNC(0x02353A00, void, part, initial, target, frames, index, middle);
  u32 p = gabi::ea(part);
  sf(p + 8, 0.f);
  sf(p + 12, initial);
  sf(p + 16, 0.f);
  sf(p + 20, 0.f);
  sf(p + 24, target);
  sb(p + 48, index);
  sf(p + 28, 0.f);
  sf(p + 44, (target - initial) / (f32)frames);
  gabi::Local<cXyz> diff;
  gabi::call(0x0201ADE0, p + 20, gabi::ea(diff.get()), p + 8);
  words(gabi::ea(diff.get()), p + 32, 3);
  gabi::call(0x0201B47C, p + 32);
  sb(p + 49, middle);
  ss(p + 52, 0);
  ss(p + 54, -1);
  ss(p + 60, 0);
  ss(p + 62, -1);
  put(p + 56, 0x02353D00);
  put(p + 64, 0x02353EC0);
}
VERIFY(0x02353A00, part_init);
BOOL part_model(void *part, const char *name, s32 index) {
  WWHD_FUNC(0x02353AE8, BOOL, part, name, index);
  u32 p = gabi::ea(part), data = resource(gabi::ea(name), index);
  if (!data) {
    assertion(0x1D9, 0x1002A438);
    return 0;
  }
  u32 model = gabi::call<u32>(0x025E38E0, data, 0, 0x11020203u);
  put(p, model);
  return 1;
}
VERIFY(0x02353AE8, part_model);
BOOL part_background(void *part, const char *name, s32 index) {
  WWHD_FUNC(0x02353B7C, BOOL, part, name, index);
  u32 p = gabi::ea(part), data = resource(gabi::ea(name), index), model = rd(p);
  u32 bg = gabi::call<u32>(0x024F2478, data, 1, model ? model + 0xC8 : 0);
  put(p + 4, bg);
  return bg != 0;
}
VERIFY(0x02353B7C, part_background);
void part_matrix(void *part, cXyz *position, csXyz *angle, cXyz *scale) {
  WWHD_FUNC(0x02353BFC, void, part, position, angle, scale);
  u32 p = gabi::ea(part), pos = gabi::ea(position), ang = gabi::ea(angle),
      s = gabi::ea(scale), model = rd(p);
  vector_float_copy(s, model + 0xBC);
  gabi::call(0x028E93CC, 0x1048D0CCu, lf(pos), lf(pos + 4), lf(pos + 8));
  gabi::call(0x025F1B48, 0x1048D0CCu, sh(ang), sh(ang + 2), sh(ang + 4));
  gabi::call(0x025F24E0, lf(p + 8), lf(p + 12), lf(p + 16));
  model = rd(p);
  matrix_copy(0x1048D0CC, model + 0xC8);
  gabi::call(0x027F4D5C, rd(p));
}
VERIFY(0x02353BFC, part_matrix);
static void part_parent_matrix(u32 part, u32 parent) {
  gabi::Local<cXyz> pos, scale;
  gabi::Local<csXyz> angle;
  vector_float_copy(parent + 0x314, gabi::ea(pos.get()));
  vector_float_copy(parent + 0x330, gabi::ea(scale.get()));
  angle_copy(parent + 0x328, gabi::ea(angle.get()));
  gabi::call(0x02353BFC, part, gabi::ea(pos.get()), gabi::ea(angle.get()),
             gabi::ea(scale.get()));
}
void part_normal(void *part, Actor *parent) {
  WWHD_FUNC(0x02353D00, void, part, parent);
  u32 p = gabi::ea(part);
  part_parent_matrix(p, gabi::ea(parent));
  u32 bg = rd(p + 4);
  if (bg && rd(bg) < 0x100)
    gabi::call(0x024F43DC, bg);
}
VERIFY(0x02353D00, part_normal);
void part_move(void *part, Actor *parent) {
  WWHD_FUNC(0x02353D9C, void, part, parent);
  u32 p = gabi::ea(part);
  sf(p + 12, lf(p + 12) + lf(p + 44));
  gabi::Local<cXyz> raw, diff;
  gabi::call(0x0201ADE0, p + 20, gabi::ea(raw.get()), p + 8);
  words(gabi::ea(raw.get()), gabi::ea(diff.get()), 3);
  f32 dot = gabi::call<f32>(0x028E8F44, p + 32, gabi::ea(diff.get()));
  if (!(dot > 0.f)) {
    u32 y = rd(p + 24), z = rd(p + 28);
    u8 middle = byte(p + 49);
    ss(p + 52, 0);
    put(p + 56, 0x02353D00);
    ss(p + 54, -1);
    put(p + 16, z);
    u32 x = rd(p + 20);
    put(p + 12, y);
    put(p + 8, x);
    if (!middle && !byte(p + 48)) {
      u32 pl = play();
      gabi::Local<cXyz> direction;
      direction->set(0.f, 1.f, 0.f);
      gabi::call(0x025CB374, pl + 0x599C, 4, -0x21, gabi::ea(direction.get()));
    }
  }
  gabi::call(0x02353D00, part, parent);
}
VERIFY(0x02353D9C, part_move);
void part_draw(void *part, Actor *parent) {
  WWHD_FUNC(0x02353EC0, void, part, parent);
  u32 p = gabi::ea(part), env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, env, rd(p), gabi::ea(parent) + 0x110);
  gabi::call(0x025E2DE0, rd(p), 0);
}
VERIFY(0x02353EC0, part_draw);

void splash_create(void *splash, u16 id, cXyz *position, f32 yOffset,
                   f32 zOffset, csXyz *angle) {
  WWHD_FUNC(0x02353F10, void, splash, id, position, yOffset, zOffset, angle);
  u32 p = gabi::ea(splash), pos = gabi::ea(position), ang = gabi::ea(angle);
  f32 x = lf(pos), y = lf(pos + 4), z = lf(pos + 8);
  sf(p + 32, x);
  sf(p + 40, z);
  sf(p + 36, y + yOffset);
  gabi::call(0x025F1884, 0x1048D0CCu, sh(ang + 2));
  gabi::Local<cXyz> vec;
  gabi::call(0x028E8F64, 0x1048D0CCu, 0x101FFBCCu, gabi::ea(vec.get()));
  gabi::call(0x028E8E64, gabi::ea(vec.get()), gabi::ea(vec.get()), zOffset);
  gabi::call(0x028E8D88, p + 32, gabi::ea(vec.get()), p + 32);
  u32 vx = rd(p + 32), vz = rd(p + 40);
  put(p + 20, vx);
  u32 vy = rd(p + 36);
  put(p + 28, vz);
  put(p + 24, vy);
  angle_copy(ang, p + 44);
  u32 pl = play(), ctl = rd(pl + 0x5AB0);
  gabi::call(0x025A847C, ctl, 0, id, p + 32, p + 44, 0, 0xFF, p, -1, 0, 0, 0);
  sb(p + 50, 1);
}
VERIFY(0x02353F10, splash_create);
static void gush_assert(u32 line, u32 condition) {
  gabi::call(0x0273AA24, 0x1002A44Cu, line, condition);
}
BOOL gush_create(void *gush, const char *name) {
  WWHD_FUNC(0x02354050, BOOL, gush, name);
  u32 p = gabi::ea(gush), n = gabi::ea(name);
  u32 data = resource(n, 11);
  if (!data) {
    gush_assert(0x280, 0x1002A45C);
    return 0;
  }
  u32 model = gabi::call<u32>(0x025E38E0, data, 0x80000u, 0x11000222u);
  put(p, model);
  if (!model) {
    gush_assert(0x289, 0x1002A48C);
    if (!rd(p))
      return 0;
  }
  u32 btk = resource(n, 15);
  if (!btk)
    gush_assert(0x290, 0x1002A46C);
  if (!gabi::call<BOOL>(0x025E7CE0, p + 4, rd(rd(p) + 0xAC), btk, 1, 2, 0, -1,
                        0, 1.f, 0))
    return 0;
  u32 bck = resource(n, 6);
  if (!bck)
    gush_assert(0x295, 0x1002A47C);
  return gabi::call<BOOL>(0x025E8508, p + 0x78, rd(rd(p) + 0xAC), bck, 1, 2, 0,
                          -1, 0, 1.f) != 0;
}
VERIFY(0x02354050, gush_create);
// Tev state is a sparse field copy: HD intentionally leaves the matrix/padding
// blocks alone.
static void tev_copy(u32 source, u32 destination) {
  for (u32 i = 0; i < 6; ++i)
    sf(destination + i * 4, lf(source + i * 4));
  for (u32 i = 0x18; i < 0x1C; ++i)
    sb(destination + i, byte(source + i));
  for (u32 i = 0x1C; i < 0x24; i += 2)
    ss(destination + i, sh(source + i));
  for (u32 i = 0x24; i < 0x44; i += 4)
    sf(destination + i, lf(source + i));
  for (u32 i = 0x84; i < 0x90; i += 4)
    put(destination + i, rd(source + i));
  for (u32 i = 0x90; i < 0x98; i += 2)
    ss(destination + i, uh(source + i));
  put(destination + 0x98, rd(source + 0x98));
  put(destination + 0x9C, rd(source + 0x9C));
  for (u32 i = 0xA0; i < 0xA8; i += 2)
    ss(destination + i, uh(source + i));
  for (u32 i = 0xA8; i < 0xB4; i += 4)
    sf(destination + i, lf(source + i));
  for (u32 i = 0xB4; i < 0xBD; ++i)
    sb(destination + i, byte(source + i));
  for (u32 i = 0xC0; i < 0xD8; i += 4)
    sf(destination + i, lf(source + i));
  for (u32 i = 0xD8; i < 0xDC; ++i)
    sb(destination + i, byte(source + i));
  for (u32 i = 0xDC; i < 0xE4; i += 2)
    ss(destination + i, sh(source + i));
  for (u32 i = 0xE4; i < 0x104; i += 4)
    sf(destination + i, lf(source + i));
  for (u32 i = 0x144; i < 0x15C; i += 4)
    sf(destination + i, lf(source + i));
  for (u32 i = 0x15C; i < 0x160; ++i)
    sb(destination + i, byte(source + i));
  for (u32 i = 0x160; i < 0x168; i += 2)
    ss(destination + i, sh(source + i));
  for (u32 i = 0x168; i < 0x188; i += 4)
    sf(destination + i, lf(source + i));
}
void gush_init(void *gush, cXyz *position, f32 distance, csXyz *angle,
               cXyz *scale, void *tev, u8 visible) {
  WWHD_FUNC(0x02354200, void, gush, position, distance, angle, scale, tev,
            visible);
  u32 p = gabi::ea(gush), ang = gabi::ea(angle);
  gabi::call(0x025F1884, 0x1048D0CCu, sh(ang + 2));
  gabi::Local<cXyz> vec, sum;
  gabi::call(0x028E8F64, 0x1048D0CCu, 0x101FFBCCu, gabi::ea(vec.get()));
  gabi::call(0x028E8E64, gabi::ea(vec.get()), gabi::ea(vec.get()), distance);
  gabi::call(0x0201AD78, position, gabi::ea(sum.get()), gabi::ea(vec.get()));
  u32 x = rd(gabi::ea(sum.get())), y = rd(gabi::ea(sum.get()) + 4),
      z = rd(gabi::ea(sum.get()) + 8);
  put(p + 0x2D4, z);
  put(p + 0x2CC, x);
  put(p + 0x2D8, x);
  put(p + 0x2D0, y);
  put(p + 0x2DC, y);
  put(p + 0x2E0, z);
  angle_copy(ang, p + 0x2F0);
  words(gabi::ea(scale), p + 0x2E4, 3);
  tev_copy(gabi::ea(tev), p + 0x104);
  sb(p + 0x2F6, visible);
}
VERIFY(0x02354200, gush_init);
void gush_matrix(void *gush) {
  WWHD_FUNC(0x023545DC, void, gush);
  u32 p = gabi::ea(gush), model = rd(p);
  if (model) {
    vector_float_copy(p + 0x2E4, model + 0xBC);
    gabi::call(0x028E93CC, 0x1048D0CCu, lf(p + 0x2D8), lf(p + 0x2DC),
               lf(p + 0x2E0));
    gabi::call(0x025F1B48, 0x1048D0CCu, sh(p + 0x2F0), sh(p + 0x2F2),
               sh(p + 0x2F4));
    gabi::call(0x025F2518, 13.f, 1.f, 11.f);
    model = rd(p);
    matrix_copy(0x1048D0CC, model + 0xC8);
  }
}
VERIFY(0x023545DC, gush_matrix);
void gush_draw(void *gush) {
  WWHD_FUNC(0x023546E0, void, gush);
  u32 p = gabi::ea(gush);
  if (byte(p + 0x2F6) && rd(p)) {
    u32 env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, env, 2, p + 0x2D8, p + 0x104);
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, env, rd(p), p + 0x104);
    gabi::call(0x025E7FC4, p + 4, rd(rd(p) + 0xAC), lf(p + 8));
    gabi::call(0x025E86B8, p + 0x78, rd(rd(p) + 0xAC), lf(p + 0x7C));
    gabi::call(0x025E2DE0, rd(p), 0);
  }
}
VERIFY(0x023546E0, gush_draw);

static void tev_light_copy(u32 src, u32 dst) {
  for (u32 i = 0; i < 0x18; i += 4)
    sf(dst + i, lf(src + i));
  for (u32 i = 0x18; i < 0x1C; ++i)
    sb(dst + i, byte(src + i));
  for (u32 i = 0x1C; i < 0x24; i += 2)
    ss(dst + i, sh(src + i));
  for (u32 i = 0x24; i < 0x44; i += 4)
    sf(dst + i, lf(src + i));
}
void *gush_constructor(void *gush) {
  WWHD_FUNC(0x02354778, void *, gush);
  u32 p = gabi::ea(gush);
  if (!p)
    p = gabi::call<u32>(0x0273AD10, 0x2F8);
  if (p) {
    gabi::call(0x025E7C6C, p + 4);
    gabi::call(0x027F2BC0, p + 0x78, 0);
    put(p + 0x88, 0x1016E54C);
    gabi::call(0x027DA984, p + 0x8C);
    put(p + 0x100, 0);
    put(p + 0xFC, 0);
    put(p + 0x88, 0x1002A370);
    put(p + 0xF8, 0);
    put(p + 0xC0, 0x1016D820);
    put(p + 0xD0, 0);
    put(p + 0xF4, 0);
    tev_light_copy(0x1016E414, p + 0x104);
    tev_light_copy(0x1016E414, p + 0x1C4);
    tev_light_copy(0x1016E414, p + 0x248);
  }
  return gabi::at<void>(p);
}
VERIFY(0x02354778, gush_constructor);
BOOL create_heap(Actor *actor) {
  WWHD_FUNC(0x02354974, BOOL, actor);
  u32 p = gabi::ea(actor);
  for (u32 i = 0; i < 2; ++i) {
    if (!gabi::call<BOOL>(0x02353AE8, p + 0xA70 + i * 0x44, 0x1002A578u,
                          rd(0x1002A320 + i * 4)))
      return 0;
    if (!gabi::call<BOOL>(0x02353B7C, p + 0xA70 + i * 0x44, 0x1002A578u,
                          rd(0x1002A328 + i * 4)))
      return 0;
  }
  u32 data = resource(0x1002A578, 12);
  if (!data) {
    gabi::call(0x0273AA24, 0x1002A4A4u, 0x324, 0x1002A4B4u);
    return gabi::call<BOOL>(0x02354050, p + 0x778, 0x1002A578u);
  }
  u32 model = gabi::call<u32>(0x025E38E0, data, 0x80000u, 0x11000222u);
  put(p + 0x3AC, model);
  for (u32 i = 0; i < 2; ++i) {
    u32 animation = resource(0x1002A578, rd(0x1002A330 + i * 4));
    if (!animation)
      gabi::call(0x0273AA24, 0x1002A4A4u, 0x32F, 0x1002A4C4u);
    if (!gabi::call<BOOL>(0x025E7CE0, p + 0x3B0 + i * 0x74, data, animation, 1,
                          rd(0x1002A338 + i * 4), 0, -1, 0, 1.f, 0))
      return 0;
  }
  return gabi::call<BOOL>(0x02354050, p + 0x778, 0x1002A578u);
}
VERIFY(0x02354974, create_heap);
BOOL heap_callback(Actor *actor) {
  WWHD_FUNC(0x02354B3C, BOOL, actor);
  return gabi::call<BOOL>(0x02354974, actor);
}
VERIFY(0x02354B3C, heap_callback);
void set_texture(Actor *actor, f32 frame, f32 speed, s32 index) {
  WWHD_FUNC(0x02354B40, void, actor, frame, speed, index);
  u32 p = gabi::ea(actor) + (u32)index * 0x74;
  sf(p + 0x3B4, frame);
  sf(p + 0x3B0, speed);
}
VERIFY(0x02354B40, set_texture);
void actor_matrix(Actor *actor) {
  WWHD_FUNC(0x02354B54, void, actor);
  u32 p = gabi::ea(actor), model = rd(p + 0x3AC);
  vector_float_copy(p + 0x330, model + 0xBC);
  gabi::call(0x028E93CC, 0x1048D0CCu, lf(p + 0x314), lf(p + 0x318),
             lf(p + 0x31C));
  gabi::call(0x025F1B48, 0x1048D0CCu, sh(p + 0x328), sh(p + 0x32A),
             sh(p + 0x32C));
  gabi::call(0x025F24E0, lf(p + 0xB60), lf(p + 0xB64), lf(p + 0xB68));
  gabi::call(0x025F2518, 1.f, 1.f, lf(p + 0xB74));
  model = rd(p + 0x3AC);
  matrix_copy(0x1048D0CC, model + 0xC8);
  gabi::call(0x027F4D5C, rd(p + 0x3AC));
}
VERIFY(0x02354B54, actor_matrix);
f32 water_height(Actor *actor) {
  WWHD_FUNC(0x02354C60, f32, actor);
  u32 p = gabi::ea(actor);
  gabi::Local<u8[0x50]> check;
  gabi::Local<cXyz> position, vec;
  gabi::call(0x024F22DC, gabi::ea(check.get()));
  vector_float_copy(p + 0x314, gabi::ea(position.get()));
  s16 yaw = sh(p + 0x322);
  f32 height = lf(p + 0x318);
  vector_float_copy(0x101FFBCC, gabi::ea(vec.get()));
  gabi::call(0x025F1884, 0x1048D0CCu, yaw);
  gabi::call(0x028E8F64, 0x1048D0CCu, 0x101FFBCCu, gabi::ea(vec.get()));
  gabi::call(0x028E8E64, gabi::ea(vec.get()), gabi::ea(vec.get()), 400.f);
  gabi::call(0x028E8D88, gabi::ea(position.get()), gabi::ea(vec.get()),
             gabi::ea(position.get()));
  if (gabi::call<BOOL>(0x024F15A4, gabi::ea(position.get()),
                       gabi::ea(check.get()), 1.f))
    height = lf(gabi::ea(check.get()) + 0x48);
  put(gabi::ea(check.get()) + 0x20, 0x1002A3A8);
  put(gabi::ea(check.get()) + 0x24, 0x1002A3C8);
  put(gabi::ea(check.get()) + 0x30, 0x1002A398);
  gabi::call(0x02008B4C, gabi::ea(check.get()) + 0x10, 0);
  return height;
}
VERIFY(0x02354C60, water_height);
void collision_init(Actor *actor) {
  WWHD_FUNC(0x02354D7C, void, actor);
  u32 p = gabi::ea(actor);
  gabi::Local<cXyz> center;
  gabi::call(0x025F1884, 0x1048D0CCu, sh(p + 0x322));
  gabi::call(0x028E8F64, 0x1048D0CCu, 0x101FFBCCu, gabi::ea(center.get()));
  gabi::call(0x028E8E64, gabi::ea(center.get()), gabi::ea(center.get()), 460.f);
  f32 height = gabi::call<f32>(0x02354C60, actor);
  sf(gabi::ea(center.get()) + 4, height);
  gabi::call(0x028E8D88, gabi::ea(center.get()), p + 0x314,
             gabi::ea(center.get()));
  gabi::call(0x02515F14, p + 0x4A0, 0xFF, 0xFF, actor);
  put(p + 0x520, p + 0x4A0);
  gabi::call(0x02516518, p + 0x4DC, 0x1002A5BCu);
  gabi::call(0x020182E0, p + 0x5F4, gabi::ea(center.get()));
  if (!switch_on(p) && !byte(p + 0xB70)) {
    gabi::call(0x02515F14, p + 0x610, 0xFF, 0xFF, actor);
    put(p + 0x690, p + 0x610);
    gabi::call(0x0251677C, p + 0x64C, 0x1002A57Cu);
    f32 y = lf(p + 0x318), z = lf(p + 0x31C), x = lf(p + 0x314);
    center->set(x, y - 400.f, z - 110.f);
    gabi::call(0x02018D40, p + 0x764, gabi::ea(center.get()));
    gabi::call(0x02018C8C, p + 0x764, 220.f);
  }
}
VERIFY(0x02354D7C, collision_init);

static void splash_stop(u32 splash) {
  u32 emitter = rd(splash + 4);
  if (emitter) {
    put(emitter + 0x254, rd(emitter + 0x254) | 1);
    sb(splash + 0x32, 0);
  }
}
static void splash_play(u32 splash) {
  u32 emitter = rd(splash + 4);
  if (emitter) {
    put(emitter + 0x254, rd(emitter + 0x254) & ~1u);
    sb(splash + 0x32, 1);
  }
}
s32 create(Actor *actor) {
  WWHD_FUNC(0x02354EE0, s32, actor);
  u32 p = gabi::ea(actor), flags = rd(p + 0x2E4);
  if (!(flags & 8)) {
    if (p) {
      gabi::call(0x025D4ED0, actor);
      put(p + 0xB4, 0x1002A408);
      gabi::call(0x028EFFD0, p + 0x3B0, 2, 0x74, 0x025E7C6Cu);
      gabi::call(0x0200BD2C, p + 0x4A0);
      gabi::call(0x02515DA0, p + 0x4BC);
      put(p + 0x4BC, 0x1004AEC0);
      put(p + 0x4B8, 0x1004AE88);
      gabi::call(0x02515FB8, p + 0x4DC);
      put(p + 0x5F0, 0x100015A8);
      put(p + 0x5EC, 0x1002A360);
      gabi::call(0x02018590, p + 0x5F4);
      put(p + 0x518, 0x1004B108);
      put(p + 0x608, 0x1004B150);
      put(p + 0x5F0, 0x1004B160);
      gabi::call(0x0200BD2C, p + 0x610);
      gabi::call(0x02515DA0, p + 0x62C);
      put(p + 0x628, 0x1004AE88);
      put(p + 0x62C, 0x1004AEC0);
      gabi::call(0x025166F0, p + 0x64C);
      gabi::call(0x02354778, p + 0x778);
      gabi::call(0x028EFFD0, p + 0xAF8, 2, 0x34, 0x02355F70u);
      flags = rd(p + 0x2E4);
    }
    put(p + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, p + 0x498, 0x1002A578u);
  if (phase != 4)
    return phase;
  if (!gabi::call<BOOL>(0x025D63E8, actor, 0x02354B3Cu, 0x3C80))
    return 5;
  u32 params = rd(p + 0xB0);
  put(p + 0xB6C, params & 0xFF);
  sb(p + 0xB70, (params >> 8) & 0xFF);
  u32 pl = play();
  s32 event = gabi::call<s32>(0x02543F10, pl + 0x52C4, 0x1002A540u, 0xFF);
  u8 middle = byte(p + 0xB70);
  ss(p + 0xB80, event);
  if (middle == 15)
    sb(p + 0xB70, 0);
  if (switch_on(p)) {
    middle = byte(p + 0xB70);
    sb(p + 0xB82, 2);
    sb(p + 0xB70, (middle + 1) & 1);
  }
  ss(p + 0xB72, -1);
  middle = byte(p + 0xB70);
  u32 posIndex = (middle & 1) * 8;
  for (u32 i = 0; i < 2; ++i) {
    u32 part = p + 0xA70 + i * 0x44;
    gabi::call(0x02353A00, part, lf(0x1002A520 + posIndex),
               lf(0x1002A530 + posIndex), uh(0x1002A51C + middle * 2), (u8)i,
               middle);
    pl = play();
    gabi::call(0x024EEA6C, pl + 0x12A0, rd(part + 4), actor);
    part_parent_matrix(part, p);
    posIndex += 4;
    if (i == 0)
      middle = byte(p + 0xB70);
  }
  sf(p + 0xB74, 1.f);
  sf(p + 0xB68, 50.f);
  sf(p + 0xB60, 0.f);
  sf(p + 0xB64, -1400.f);
  for (u32 i = 0; i < 2; ++i)
    gabi::call(0x02353F10, p + 0xAF8 + i * 0x34, uh(0x1002A344 + i * 2),
               p + 0x314, lf(0x1002A50C + i * 4), lf(0x1002A514 + i * 4),
               p + 0x320);
  if (!byte(p + 0xB70)) {
    gabi::call(0x02354B40, actor, 36.f, 0.f, 1);
    sb(p + 0x60C, 1);
    gabi::call(0x02354200, p + 0x778, p + 0x314, 600.f, p + 0x320, p + 0x330,
               p + 0x110, 1);
    sb(p + 0xB83, 1);
  } else {
    gabi::call(0x02354B40, actor, 0.f, 0.f, 1);
    splash_stop(p + 0xAF8);
    splash_stop(p + 0xB2C);
    gabi::call(0x02354200, p + 0x778, p + 0x314, 600.f, p + 0x320, p + 0x330,
               p + 0x110, 0);
    sb(p + 0xB83, 0);
  }
  gabi::call(0x023545DC, p + 0x778);
  gabi::call(0x02354B54, actor);
  gabi::call(0x02354D7C, actor);
  u32 model = rd(p + 0xA70);
  put(p + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, actor, -700.f, -1600.f, -400.f, 700.f, 120.f, 1000.f);
  return 4;
}
VERIFY(0x02354EE0, create);
u8 remove(Actor *actor) {
  WWHD_FUNC(0x02355394, u8, actor);
  u32 p = gabi::ea(actor);
  if (rd(p + 0xF4))
    for (u32 i = 0; i < 2; ++i) {
      u32 part = p + 0xA70 + i * 0x44, bg = rd(part + 4);
      if (bg && rd(bg) < 0x100) {
        u32 pl = play();
        gabi::call(0x020087EC, pl + 0x12A0, rd(part + 4));
        put(part + 4, 0);
      }
    }
  for (u32 i = 0; i < 2; ++i) {
    u32 splash = p + 0xAF8 + i * 0x34;
    if (rd(splash + 4)) {
      gabi::call(rd(rd(splash) + 0x44), splash);
      sb(splash + 0x32, 0);
    }
  }
  gabi::call(0x025204C8, p + 0x498, 0x1002A578u);
  return 1;
}
VERIFY(0x02355394, remove);
static void part_dispatch(u32 part, u32 parent, u32 offset) {
  s16 slot = sh(part + offset + 2);
  if (slot) {
    u32 receiver = part + (s32)sh(part + offset), function;
    if (slot < 0)
      function = rd(part + offset + 4);
    else
      function =
          rd(rd(receiver + (s32)sh(part + offset + 6)) + (u32)slot * 8 + 4);
    gabi::call(function, receiver, parent);
  }
}
void part_manager(Actor *actor) {
  WWHD_FUNC(0x02355464, void, actor);
  u32 p = gabi::ea(actor);
  s32 timer = sh(p + 0xB72);
  if (timer >= 0) {
    if (timer == 0) {
      u8 middle = byte(p + 0xB70);
      ss(p + 0xAA4, 0);
      ss(p + 0xB72, -1);
      put(p + 0xAEC, 0x02353D9C);
      ss(p + 0xAE8, 0);
      put(p + 0xAA8, 0x02353D9C);
      ss(p + 0xAA6, -1);
      ss(p + 0xAEA, -1);
      if (!middle) {
        s32 reverb = gabi::call<s32>(0x02520540, (s8)byte(p + 0x326));
        gabi::call(0x025E1A40, 0x6A12, p + 0x314, 0, reverb);
      }
    } else
      ss(p + 0xB72, timer - 1);
  }
  for (u32 i = 0; i < 2; ++i)
    part_dispatch(p + 0xA70 + i * 0x44, p, 0x34);
}
VERIFY(0x02355464, part_manager);
void splash_height(Actor *actor) {
  WWHD_FUNC(0x0235557C, void, actor);
  f32 y = gabi::call<f32>(0x02354C60, actor);
  sf(gabi::ea(actor) + 0xB1C, y);
}
VERIFY(0x0235557C, splash_height);
void splash_radius(Actor *actor) {
  WWHD_FUNC(0x023555AC, void, actor);
  u32 p = gabi::ea(actor);
  gabi::Local<cXyz> result, base;
  vector_float_copy(0x101FFBCC, gabi::ea(result.get()));
  gabi::call(0x02353924, gabi::ea(base.get()), sh(p + 0x322));
  u32 x = rd(gabi::ea(base.get()));
  sf(gabi::ea(base.get()), lf(p + 0xB0C));
  u32 y = rd(gabi::ea(base.get()) + 4);
  sf(gabi::ea(base.get()) + 4, lf(p + 0xB10));
  put(gabi::ea(result.get()), x);
  put(gabi::ea(result.get()) + 4, y);
  u32 z = rd(gabi::ea(base.get()) + 8);
  put(gabi::ea(result.get()) + 8, z);
  sf(gabi::ea(base.get()) + 8, lf(p + 0xB14));
  gabi::call(0x028E8D88, gabi::ea(result.get()), gabi::ea(base.get()),
             gabi::ea(result.get()));
  f32 vx = lf(gabi::ea(result.get())), vz = lf(gabi::ea(result.get()) + 8);
  sf(p + 0xB18, vx);
  sf(p + 0xB20, vz);
}
VERIFY(0x023555AC, splash_radius);
void splash_stop_radius(Actor *actor) {
  WWHD_FUNC(0x02355650, void, actor);
  u32 p = gabi::ea(actor);
  f32 distance = (1.f - (f32)((s32)uh(p + 0xB7C) - 15) / 75.f) * -300.f;
  gabi::Local<cXyz> vec, base;
  vector_float_copy(0x101FFBCC, gabi::ea(vec.get()));
  gabi::call(0x025F1884, 0x1048D0CCu, sh(p + 0x322));
  gabi::call(0x028E8F64, 0x1048D0CCu, 0x101FFBCCu, gabi::ea(vec.get()));
  gabi::call(0x028E8E64, gabi::ea(vec.get()), gabi::ea(vec.get()), distance);
  vector_float_copy(p + 0xB18, gabi::ea(base.get()));
  gabi::call(0x028E8D88, gabi::ea(vec.get()), gabi::ea(base.get()),
             gabi::ea(vec.get()));
  f32 x = lf(gabi::ea(vec.get())), z = lf(gabi::ea(vec.get()) + 8);
  sf(p + 0xB18, x);
  sf(p + 0xB20, z);
}
VERIFY(0x02355650, splash_stop_radius);

static void splash_remove(u32 splash) {
  gabi::call(rd(rd(splash) + 0x44), splash);
  sb(splash + 0x32, 0);
}
void water_manager(Actor *actor) {
  WWHD_FUNC(0x02355774, void, actor);
  u32 p = gabi::ea(actor);
  gabi::call(0x0235557C, actor);
  gabi::call(0x023555AC, actor);
  u8 state = byte(p + 0xB7E);
  if (state == 1) {
    f32 scale = lf(p + 0xB74) - 0.0061111110262572765f, minimum = lf(p + 0xB78);
    sf(p + 0xB74, scale);
    u32 timer = uh(p + 0xB7C);
    if (scale < minimum)
      sf(p + 0xB74, minimum);
    if (timer == 34) {
      gabi::call(0x02354B40, actor, 37.f, 1.f, 1);
      timer = uh(p + 0xB7C);
    }
    if (timer >= 15) {
      if (timer == 15) {
        u32 emitter = rd(p + 0xB30);
        if (emitter) {
          put(emitter + 0x254, rd(emitter + 0x254) | 1);
          u32 reloaded = rd(p + 0xB30);
          sb(p + 0xB5E, 0);
          if (reloaded) {
            splash_remove(p + 0xB2C);
            timer = uh(p + 0xB7C);
          } else
            timer = uh(p + 0xB7C);
        }
      } else {
        gabi::call(0x02355650, actor);
        timer = uh(p + 0xB7C);
      }
    }
    timer = (timer - 1) & 0xFFFF;
    ss(p + 0xB7C, timer);
    if (!timer) {
      sb(p + 0xB7E, 0);
      u32 emitter = rd(p + 0xAFC);
      if (emitter) {
        put(emitter + 0x254, rd(emitter + 0x254) | 1);
        u32 reloaded = rd(p + 0xAFC);
        sb(p + 0xB2A, 0);
        if (reloaded)
          splash_remove(p + 0xAF8);
      }
      sb(p + 0x60C, 0);
      sb(p + 0xA6E, 0);
      sb(p + 0xB83, 0);
    }
  } else if (state == 2) {
    u32 timer = uh(p + 0xB7C);
    if (timer == 35) {
      splash_play(p + 0xB2C);
      gabi::call(0x02354B40, actor, 0.f, 1.f, 1);
      timer = uh(p + 0xB7C);
    }
    u8 active = byte(p + 0xB2A);
    timer = (timer - 1) & 0xFFFF;
    ss(p + 0xB7C, timer);
    if (!active) {
      f32 threshold = gabi::fmadds(5.f, lf(0x101D5F28), 10.f);
      if (!((f32)timer > threshold)) {
        splash_play(p + 0xAF8);
        timer = uh(p + 0xB7C);
        sb(p + 0xB83, 1);
        sb(p + 0xA6E, 1);
      }
    }
    if (!timer) {
      gabi::call(0x02354B40, actor, 36.f, 0.f, 1);
      sb(p + 0xB7E, 0);
      sb(p + 0x60C, 1);
    }
  }
  for (u32 i = 0; i < 2; ++i)
    gabi::call(0x025E742C, p + 0x3B0 + i * 0x74);
  gabi::call(0x02354B54, actor);
}
VERIFY(0x02355774, water_manager);
void gush_manager(Actor *actor) {
  WWHD_FUNC(0x02355A18, void, actor);
  u32 p = gabi::ea(actor);
  if (byte(p + 0xA6E)) {
    gabi::Local<cXyz> base, result;
    gabi::call(0x02353924, gabi::ea(base.get()), sh(p + 0x322));
    u32 x = rd(gabi::ea(base.get()));
    sf(gabi::ea(base.get()), lf(p + 0xA44));
    u32 y = rd(gabi::ea(base.get()) + 4);
    sf(gabi::ea(base.get()) + 4, lf(p + 0xA48));
    put(gabi::ea(result.get()), x);
    put(gabi::ea(result.get()) + 4, y);
    u32 z = rd(gabi::ea(base.get()) + 8);
    put(gabi::ea(result.get()) + 8, z);
    sf(gabi::ea(base.get()) + 8, lf(p + 0xA4C));
    gabi::call(0x028E8D88, gabi::ea(result.get()), gabi::ea(base.get()),
               gabi::ea(result.get()));
    f32 height = gabi::call<f32>(0x02354C60, actor),
        vz = lf(gabi::ea(result.get()) + 8), vx = lf(gabi::ea(result.get()));
    sf(p + 0xA58, vz);
    sf(p + 0xA50, vx);
    sf(p + 0xA54, height);
    sf(gabi::ea(result.get()) + 4, height);
    sf(gabi::ea(base.get()) + 8, vz);
    sf(gabi::ea(base.get()) + 4, height);
    sf(gabi::ea(base.get()), vx);
    gabi::call(0x025E742C, p + 0x77C);
    gabi::call(0x025E742C, p + 0x7F0);
    gabi::call(0x023545DC, p + 0x778);
  }
}
VERIFY(0x02355A18, gush_manager);
u8 execute(Actor *actor) {
  WWHD_FUNC(0x02355AE8, u8, actor);
  u32 p = gabi::ea(actor);
  u8 state = byte(p + 0xB82);
  if (state == 0) {
    if (switch_on(p)) {
      if (!byte(p + 0xB70)) {
        if (uh(p + 0xF8) != 2) {
          gabi::call(0x025D7A58, actor, sh(p + 0xB80), 0xFF, 0xFFFF, 0, 1);
          ss(p + 0xFA, uh(p + 0xFA) | 2);
        } else {
          gabi::call(0x025E1988, 0x806);
          sb(p + 0xB82, 1);
          sb(p + 0xB7E, 1);
          ss(p + 0xB7C, 90);
          ss(p + 0xB72, 65);
        }
      } else {
        s32 event = sh(p + 0xB80);
        u32 pl = play();
        if (gabi::call<BOOL>(0x0254407C, pl + 0x52C4, event)) {
          sb(p + 0xB82, 2);
          sb(p + 0xB7E, 2);
          ss(p + 0xB7C, 35);
          ss(p + 0xB72, 0);
        }
      }
    }
  } else if (state == 1) {
    s32 event = sh(p + 0xB80);
    u32 pl = play();
    if (gabi::call<BOOL>(0x025440C8, pl + 0x52C4, event)) {
      pl = play();
      ss(pl + 0x52B8, uh(pl + 0x52B8) | 8);
      sb(p + 0xB82, 2);
    }
  }
  gabi::call(0x02355464, actor);
  gabi::call(0x02355774, actor);
  gabi::call(0x02355A18, actor);
  if (byte(p + 0xB83)) {
    s32 reverb = gabi::call<s32>(0x02520540, (s8)byte(p + 0x326));
    gabi::call(0x025E1A40, 0x701F, p + 0x314, 0, reverb);
  }
  if (byte(p + 0x60C)) {
    gabi::Local<cXyz> center;
    vector_float_copy(p + 0x5F4, gabi::ea(center.get()));
    f32 height = gabi::call<f32>(0x02354C60, actor);
    sf(gabi::ea(center.get()) + 4, height);
    gabi::call(0x020182E0, p + 0x5F4, gabi::ea(center.get()));
    u32 pl = play();
    gabi::call(0x0200E240, pl + 0x26A4, p + 0x4DC);
    if (!switch_on(p) && !byte(p + 0xB70)) {
      pl = play();
      gabi::call(0x0200E240, pl + 0x26A4, p + 0x64C);
    }
  }
  return 1;
}
VERIFY(0x02355AE8, execute);
u8 draw(Actor *actor) {
  WWHD_FUNC(0x02355D9C, u8, actor);
  u32 p = gabi::ea(actor), env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, env, 1, p + 0x314, p + 0x110);
  for (u32 i = 0; i < 2; ++i)
    part_dispatch(p + 0xA70 + i * 0x44, p, 0x3C);
  if (rd(p + 0x3AC)) {
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, env, 0, p + 0x314, p + 0x110);
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, env, rd(p + 0x3AC), p + 0x110);
    for (u32 i = 0; i < 2; ++i)
      gabi::call(0x025E8048, p + 0x3B0 + i * 0x74, rd(p + 0x3AC),
                 lf(p + 0x3B4 + i * 0x74));
    gabi::call(0x025E2DE0, rd(p + 0x3AC), 0);
  }
  gabi::call(0x023546E0, p + 0x778);
  return 1;
}
VERIFY(0x02355D9C, draw);
s32 create_adapter(Actor *actor) {
  WWHD_FUNC(0x02355EB8, s32, actor);
  return gabi::call<s32>(0x02354EE0, actor);
}
VERIFY(0x02355EB8, create_adapter);
u8 delete_adapter(Actor *actor) {
  WWHD_FUNC(0x02355EBC, u8, actor);
  return gabi::call<u8>(0x02355394, actor);
}
VERIFY(0x02355EBC, delete_adapter);
u8 execute_adapter(Actor *actor) {
  WWHD_FUNC(0x02355EC0, u8, actor);
  return gabi::call<u8>(0x02355AE8, actor);
}
VERIFY(0x02355EC0, execute_adapter);
u8 draw_adapter(Actor *actor) {
  WWHD_FUNC(0x02355EC4, u8, actor);
  return gabi::call<u8>(0x02355D9C, actor);
}
VERIFY(0x02355EC4, draw_adapter);
void static_initialize() {
  WWHD_FUNC(0x02355EC8, void);
  put(0x10469E6C, 0);
  put(0x10469E64, 0);
  put(0x10469E70, 0);
  put(0x10469E68, 0);
  gabi::call(0x028F026C, 0x101C9EE4u);
  sf(0x10469E58, lf(0x1002A570));
  sf(0x10469E5C, lf(0x1002A574));
  gabi::call(0x028ED6F8, 0x10469E60u);
  gabi::call(0x028F026C, 0x101C9EF0u);
  gabi::call(0x028EAB2C, 0x10469E61u);
  gabi::call(0x028F026C, 0x101C9EFCu);
}
VERIFY(0x02355EC8, static_initialize);
void static_destructor(void *object, s32 mode) {
  WWHD_FUNC(0x02355F5C, void, object, mode);
  if (object && (mode & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x02355F5C, static_destructor);
void *splash_constructor(void *splash) {
  WWHD_FUNC(0x02355F70, void *, splash);
  u32 p = gabi::ea(splash);
  if (!p)
    p = gabi::call<u32>(0x0273AD10, 0x34);
  if (p)
    gabi::call(0x025A5894, p, 0, 0);
  return gabi::at<void>(p);
}
VERIFY(0x02355F70, splash_constructor);
void splash_destructor(void *object, s32 mode) {
  WWHD_FUNC(0x02355FC0, void, object, mode);
  if (object && (mode & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x02355FC0, splash_destructor);
void actor_destructor(Actor *actor, s32 mode) {
  WWHD_FUNC(0x02355FD4, void, actor, mode);
  u32 p = gabi::ea(actor);
  if (p) {
    gabi::call(0x028F0164, p + 0xAF8, 2, 0x34, 0x02355FC0u, 0, 0);
    gabi::call(0x027F3628, p + 0x800, 0);
    gabi::call(0x02515AE8, p + 0x64C, 2);
    gabi::call(0x02515860, p + 0x610, 2);
    gabi::call(0x02515A70, p + 0x4DC, 2);
    gabi::call(0x02515860, p + 0x4A0, 2);
    gabi::call(0x025D50BC, actor, 0);
    if (mode & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x02355FD4, actor_destructor);
void noop(void *object) { WWHD_FUNC(0x02356084, void, object); }
VERIFY(0x02356084, noop);
BOOL is_delete(Actor *actor) {
  WWHD_FUNC(0x02356088, BOOL, actor);
  return 1;
}
VERIFY(0x02356088, is_delete);

// Full HD Gale Isle palm TU. Derived game code: private verification only.
#include "d/actor/d_a_obj_vyasi.h"
namespace {
using Actor = daObjVyasi_c;
struct Matrix {
  be<f32> values[12];
};
struct Quat {
  be<f32> x, y, z, w;
};
struct ResourceKey {
  be<u32> string, vtable;
};
static u32 rd(u32 p) { return gabi::load<u32>(p); }
static f32 lf(u32 p) { return gabi::load<f32>(p); }
static s16 sh(u32 p) { return gabi::load<s16>(p); }
static u16 uh(u32 p) { return gabi::load<u16>(p); }
static u8 byte(u32 p) { return gabi::load<u8>(p); }
static void put(u32 p, u32 v) { gabi::store<u32>(p, v); }
static void sf(u32 p, f32 v) { gabi::store<f32>(p, v); }
static void ss(u32 p, s32 v) { gabi::store<s16>(p, (s16)v); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static f32 sin_angle(u16 angle) {
  return lf(0x104A44F8 + (u32)(angle >> 3) * 8);
}
static f32 cos_angle(u16 angle) {
  return lf(0x104A44FC + (u32)(angle >> 3) * 8);
}
static s16 angle_float(f32 value) { return (s16)gabi::ftoi(value); }
static void words(u32 from, u32 to, u32 count) {
  for (u32 i = 0; i < count; ++i)
    put(to + 4 * i, rd(from + 4 * i));
}
static void matrix_copy(u32 from, u32 to) {
  f32 values[12];
  for (int i = 0; i < 12; ++i)
    values[i] = lf(from + 4 * i);
  for (int i = 0; i < 12; ++i)
    sf(to + 4 * i, values[i]);
}
static u32 resource(s32 index) {
  gabi::Local<ResourceKey> key;
  key->string = 0x100335B8;
  key->vtable = 0x10033350;
  return gabi::call<u32>(0x026066C4, rd(0x101F4F28), gabi::ea(key.get()),
                         index);
}
static BOOL switch_on(Actor *a) {
  s32 index = gabi::call<s32>(0x02349DFC, a, 8, 0);
  u32 save = rd(0x101F84DC);
  return gabi::call<BOOL>(0x025BA0C0, save + 0x20, index,
                          (s8)byte(gabi::ea(a) + 0x2fe));
}
static u32 joint_matrix(u32 system, u32 index) {
  u32 data = rd(system + 0x2c), base = rd(data + 0x10);
  gabi::store<u16>(data + 4, uh(data + 4) | 0x10);
  return base + index * 48;
}
u8 create_heap(Actor *a) {
  WWHD_FUNC(0x023B73EC, u8, a);
  u32 self = gabi::ea(a), data = resource(7);
  if (!data)
    gabi::call(0x0273AA24, 0x10033468u, 0x47b, 0x1003347cu);
  u32 animation = resource(4);
  put(self + 0x608, animation);
  if (!animation) {
    gabi::call(0x0273AA24, 0x10033468u, 0x480, 0x10033458u);
    animation = rd(self + 0x608);
  }
  if (animation && data) {
    u32 morf = gabi::call<u32>(0x025E4F64, 0, data, 0, 0, animation, 0, 0, -1,
                               1, 0, 0, 0x11000002u, 1.f);
    animation = rd(self + 0x608);
    put(self + 0x604, morf);
  }
  if (!animation)
    return 0;
  u32 morf = rd(self + 0x604);
  return morf && rd(morf + 0x90) != 0;
}
VERIFY(0x023B73EC, create_heap);
u8 heap_callback(Actor *a) {
  WWHD_FUNC(0x023B7524, u8, a);
  return create_heap(a);
}
VERIFY(0x023B7524, heap_callback);
BOOL process_init(Actor *a, s32 state) {
  WWHD_FUNC(0x023B7528, BOOL, a, state);
  if ((u32)state >= 5)
    return 0;
  u32 member = 0x1003348c + (u32)state * 8;
  u32 self = gabi::ea(a) + (s32)sh(member);
  s32 slot = sh(member + 2);
  u32 function;
  if (slot < 0)
    function = rd(member + 4);
  else
    function = rd(rd(self + (s32)sh(member + 6)) + (u32)slot * 8 + 4);
  if (!gabi::call<BOOL>(function, self))
    return 0;
  put(gabi::ea(a) + 0x1ae0, state);
  return 1;
}
VERIFY(0x023B7528, process_init);
void set_first_process(Actor *a) {
  WWHD_FUNC(0x023B75E0, void, a);
  gabi::call(0x023B7528, a, switch_on(a) ? 4 : 1);
  u32 self = gabi::ea(a);
  s32 yaw = sh(self + 0x32a) - 0x8000;
  ss(self + 0x65c, 0);
  ss(self + 0x32a, yaw);
  sf(self + 0x1aec, 1.f);
}
VERIFY(0x023B75E0, set_first_process);
void set_mtx(Actor *a) {
  WWHD_FUNC(0x023B7668, void, a);
  u32 self = gabi::ea(a), model = rd(rd(self + 0x604) + 0x90);
  f32 z = lf(self + 0x338), x = lf(self + 0x330), y = lf(self + 0x334);
  sf(model + 0xbc, x);
  sf(model + 0xc0, y);
  sf(model + 0xc4, z);
  gabi::call(0x028E93CC, 0x1048D0CCu, lf(self + 0x314), lf(self + 0x318),
             lf(self + 0x31c));
  gabi::call(0x025F1B48, 0x1048D0CCu, sh(self + 0x328), sh(self + 0x32a),
             sh(self + 0x32c));
  model = rd(rd(self + 0x604) + 0x90);
  matrix_copy(0x1048D0CC, model + 0xc8);
  gabi::call(0x028E90D4, 0x1048D0CCu, self + 0x5d4);
}
VERIFY(0x023B7668, set_mtx);
BOOL joint_callback(void *node, s32 stage) {
  WWHD_FUNC(0x023B775C, BOOL, node, stage);
  u32 system = rd(0x104B462C);
  u32 joint = gabi::call<u32>(0x027F7878, node);
  u32 index = uh(joint + 4), actor = rd(system + 0xb8);
  if (stage != 0)
    return 1;
  if (index >= 14)
    gabi::call(0x0273AA24, 0x100334B8u, 0x373, 0x100334CCu);
  constexpr u32 stack = 0x1048D0CC;
  gabi::call(0x028E90D4, joint_matrix(system, index), stack);
  gabi::Local<Matrix> matrix;
  gabi::call(0x028E90D4, joint_matrix(system, index), gabi::ea(matrix.get()));
  f32 x = lf(gabi::ea(matrix.get()) + 12), y = lf(gabi::ea(matrix.get()) + 28),
      z = lf(gabi::ea(matrix.get()) + 44);
  sf(gabi::ea(matrix.get()) + 44, 0.f);
  sf(gabi::ea(matrix.get()) + 12, 0.f);
  sf(gabi::ea(matrix.get()) + 28, 0.f);
  gabi::call(0x028E93CC, stack, x, y, z);
  gabi::call(0x025F25CC, actor + 0x3e4 + index * 16);
  gabi::call(0x028E9108, stack, gabi::ea(matrix.get()), stack);
  u32 data = rd(system + 0x2c);
  gabi::store<u16>(data + 4, uh(data + 4) | 0x10);
  matrix_copy(stack, rd(data + 0x10) + index * 48);
  gabi::call(0x028E90D4, stack, 0x104B4868u);
  gabi::Local<csXyz[14]> angles;
  for (u32 i = 0; i < 14; ++i)
    gabi::call(0x0201A478, gabi::ea(angles.get()) + 6 * i, 0, 0, 0);
  gabi::Local<csXyz> rotation;
  ss(gabi::ea(rotation.get()), sh(gabi::ea(angles.get()) + index * 6));
  ss(gabi::ea(rotation.get()) + 2, sh(gabi::ea(angles.get()) + index * 6 + 2));
  ss(gabi::ea(rotation.get()) + 4, sh(gabi::ea(angles.get()) + index * 6 + 4));
  gabi::call(0x0201A554, gabi::ea(rotation.get()), actor + 0x4c4 + index * 6);
  gabi::call(0x028E90D4, joint_matrix(system, index), stack);
  gabi::call(0x025F1B48, stack, sh(gabi::ea(rotation.get())),
             sh(gabi::ea(rotation.get()) + 2),
             sh(gabi::ea(rotation.get()) + 4));
  if (byte(0x101CDD44 + index) == 0)
    gabi::call(0x025F2518, lf(actor + 0x5c0), lf(actor + 0x5c4),
               lf(actor + 0x5c8));
  data = rd(system + 0x2c);
  u16 flags = uh(data + 4);
  u32 base = rd(data + 0x10);
  gabi::store<u16>(data + 4, flags | 0x10);
  matrix_copy(stack, base + index * 48);
  gabi::call(0x028E90D4, stack, 0x104B4868u);
  gabi::Local<cXyz> origin;
  origin->set(0.f, 0.f, 0.f);
  gabi::call(0x028E8F64, stack, gabi::ea(origin.get()),
             actor + 0x518 + index * 12);
  return 1;
}
VERIFY(0x023B775C, joint_callback);
s32 create(Actor *a) {
  WWHD_FUNC(0x023B7B3C, s32, a);
  u32 self = gabi::ea(a), flags = rd(self + 0x2e4);
  if (!(flags & 8)) {
    if (self) {
      gabi::call(0x025D4ED0, a);
      put(self + 0xb4, 0x10033378);
      gabi::call(0x0200BD2C, self + 0x660);
      gabi::call(0x02515DA0, self + 0x67c);
      put(self + 0x678, 0x1004AE88);
      put(self + 0x67c, 0x1004AEC0);
      gabi::call(0x02515FB8, self + 0x69c);
      put(self + 0x7b0, 0x100015A8);
      put(self + 0x7ac, 0x10033368);
      gabi::call(0x02018590, self + 0x7b4);
      put(self + 0x7b0, 0x1004B160);
      put(self + 0x6d8, 0x1004B108);
      put(self + 0x7c8, 0x1004B150);
      gabi::call(0x028EFFD0, self + 0x7cc, 5, 0x3c, 0x023B9660u);
      gabi::call(0x028EFFD0, self + 0x8f8, 5, 0x138, 0x023B96C8u);
      gabi::call(0x028EFFD0, self + 0xf9c, 8, 0x3c, 0x023B9660u);
      gabi::call(0x028EFFD0, self + 0x117c, 8, 0x12c, 0x025166F0u);
      flags = rd(self + 0x2e4);
    }
    put(self + 0x2e4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, self + 0x5cc, 0x100335B8u);
  if (phase != 4)
    return phase;
  if (!gabi::call<BOOL>(0x025D63E8, a, 0x023B7524u, 0))
    return 5;
  gabi::call(0x023B75E0, a);
  gabi::call(0x023B7668, a);
  u32 model = rd(rd(self + 0x604) + 0x90);
  put(self + 0x348, model ? model + 0xc8 : 0);
  gabi::call(0x025D674C, a, -2000.f, 0.f, -2000.f, 2000.f, 2000.f, 2000.f);
  sf(self + 0x364, 2.f);
  gabi::call(0x02515F14, self + 0x660, 0xff, 0xff, a);
  gabi::call(0x02516518, self + 0x69c, 0x100333C8u);
  put(self + 0x6e0, self + 0x660);
  u32 objectFlags = rd(self + 0x730);
  put(self + 0x750, rd(0x101FFBA8));
  put(self + 0x754, rd(0x101FFBAC));
  put(self + 0x730, objectFlags | 4);
  put(self + 0x758, rd(0x101FFBB0));
  for (u32 i = 0; i < 5; ++i) {
    u32 status = self + 0x7cc + i * 0x3c, collision = self + 0x8f8 + i * 0x138;
    gabi::call(0x02515F14, status, 0x64, 0xff, a);
    gabi::call(0x025164C0, collision, 0x1003340Cu);
    put(collision + 0x44, status);
    u32 shape = self + 0xf10 + i * 0x1c;
    words(self + 0x314, shape, 3);
    words(self + 0x314, shape + 12, 3);
    sf(shape + 24, 100.f);
  }
  for (u32 i = 0; i < 8; ++i) {
    u32 status = self + 0xf9c + i * 0x3c, collision = self + 0x117c + i * 0x12c;
    gabi::call(0x02515F14, status, 0x64, 0xff, a);
    gabi::call(0x0251677C, collision, 0x10033388u);
    put(collision + 0x44, status);
    objectFlags = rd(self + 0x730);
    put(self + 0x750, rd(0x101FFBA8));
    put(self + 0x754, rd(0x101FFBAC));
    put(self + 0x730, objectFlags | 4);
    put(self + 0x758, rd(0x101FFBB0));
  }
  model = rd(rd(self + 0x604) + 0x90);
  u32 data = rd(model + 0xac);
  put(model + 0xb8, self);
  u32 jointData = gabi::call<u32>(0x027F3F94, data);
  for (u32 i = 0; i < uh(jointData + 8); i = (u16)(i + 1)) {
    u32 node = rd(data + 8);
    if (i < rd(data + 4))
      node += i * 0x1c;
    put(node + 8, 0x023B775C);
    jointData = gabi::call<u32>(0x027F3F94, rd(model + 0xac));
  }
  for (u32 i = 0; i < 14; ++i)
    words(0x101E9C38, self + 0x3e4 + i * 16, 4);
  sf(self + 0x5c0, 1.f);
  sf(self + 0x5c4, 1.f);
  sf(self + 0x5c8, 1.f);
  return phase;
}
VERIFY(0x023B7B3C, create);
u8 remove(Actor *a) {
  WWHD_FUNC(0x023B7F1C, u8, a);
  gabi::call(0x025204C8, gabi::ea(a) + 0x5cc, 0x100335B8u);
  return 1;
}
VERIFY(0x023B7F1C, remove);
BOOL play_stop_animation(Actor *a) {
  WWHD_FUNC(0x023B7F4C, BOOL, a);
  return gabi::call<s32>(0x025E535C, rd(gabi::ea(a) + 0x604), 0, 0, 0) == 0;
}
VERIFY(0x023B7F4C, play_stop_animation);
void process_main(Actor *a) {
  WWHD_FUNC(0x023B7F84, void, a);
  u32 state = rd(gabi::ea(a) + 0x1ae0);
  if (state >= 5)
    return;
  u32 member = 0x100334F4 + state * 8, self = gabi::ea(a) + (s32)sh(member);
  s32 slot = sh(member + 2);
  u32 function;
  if (slot < 0)
    function = rd(member + 4);
  else
    function = rd(rd(self + (s32)sh(member + 6)) + (u32)slot * 8 + 4);
  gabi::call(function, self);
}
VERIFY(0x023B7F84, process_main);
static void hit_sound(u32 self, u32 collision) {
  gabi::call(0x02516300, collision);
  gabi::call(0x023129C4, self + 0x314, (s8)byte(self + 0x326), collision, 7);
}
static void sound_position(u32 self) {
  gabi::Local<cXyz> position;
  f32 y = lf(self + 0x318), z = lf(self + 0x31c), x = lf(self + 0x314);
  position->set(x, y, z);
  gabi::call(0x0255F458, gabi::ea(position.get()), 4, rd(self + 4), 0x64);
}
void set_collision(Actor *a) {
  WWHD_FUNC(0x023B7FDC, void, a);
  u32 self = gabi::ea(a), cylinder = self + 0x69c;
  if (gabi::call<BOOL>(0x025162A4, cylinder)) {
    hit_sound(self, cylinder);
    gabi::call(0x02312C8C, a, cylinder);
    sound_position(self);
    gabi::call(0x0251621C, cylinder);
  } else {
    gabi::call(0x020184DC, self + 0x7b4, 79.f);
    gabi::call(0x02018428, self + 0x7b4, 250.f);
    gabi::call(0x020182E0, self + 0x7b4, self + 0x314);
    gabi::call(0x0200E240, play() + 0x26a4, cylinder);
  }
  f32 radius = 47.400001525878906f;
  for (u32 i = 0; i < 5; ++i) {
    u32 collision = self + 0x8f8 + i * 0x138;
    if (gabi::call<BOOL>(0x025162A4, collision)) {
      hit_sound(self, collision);
      sound_position(self);
      gabi::call(0x0251621C, collision);
    } else {
      u32 shape = self + 0xf10 + i * 0x1c;
      words(self + 0x518 + i * 12, shape, 3);
      words(self + 0x518 + (i + 1) * 12, shape + 12, 3);
      sf(shape + 24, radius);
      gabi::call(0x020181FC, collision + 0x118, shape);
      gabi::call(0x0200E240, play() + 0x26a4, collision);
    }
  }
  for (u32 i = 0; i < 4; ++i) {
    u32 start = self + 0x518 + (i + 1) * 12, end = start + 12;
    f32 x = lf(start), y = lf(start + 4), z = lf(start + 8);
    f32 dx = (lf(end) - x) * 0.3333300054073334f;
    f32 dy = (lf(end + 4) - y) * 0.3333300054073334f;
    f32 dz = (lf(end + 8) - z) * 0.3333300054073334f;
    gabi::Local<cXyz> center;
    center->set(x + dx, y + dy, z + dz);
    u32 sphere = self + 0x117c + i * 0x258;
    gabi::call(0x02018D40, sphere + 0x118, gabi::ea(center.get()));
    gabi::call(0x02018C8C, sphere + 0x118, radius);
    gabi::call(0x0200E240, play() + 0x26a4, sphere);
    center->set(lf(start) + (dx + dx), lf(start + 4) + (dy + dy),
                lf(start + 8) + (dz + dz));
    sphere += 0x12c;
    gabi::call(0x02018D40, sphere + 0x118, gabi::ea(center.get()));
    gabi::call(0x02018C8C, sphere + 0x118, radius);
    gabi::call(0x0200E240, play() + 0x26a4, sphere);
  }
}
VERIFY(0x023B7FDC, set_collision);
void quaternion_main(Actor *a) {
  WWHD_FUNC(0x023B8388, void, a);
  u32 self = gabi::ea(a);
  for (u32 i = 0; i < 14; ++i) {
    gabi::Local<Quat> target;
    words(0x101E9C38, gabi::ea(target.get()), 4);
    s32 state = (s32)rd(self + 0x1ae0);
    if (state == 4 && byte(0x101CDD44 + i) == 0) {
      gabi::Local<cXyz> axis;
      axis->set(0.f, 1.f, 0.f);
      gabi::call(0x025F1884, rd(0x1018C7B0), (s16)-sh(self + 0x322));
      u32 wind = gabi::call<u32>(0x0257DAA8);
      gabi::Local<cXyz> localWind;
      gabi::call(0x0200FCD8, wind, gabi::ea(localWind.get()));
      f32 power = gabi::call<f32>(0x02578348);
      gabi::Local<cXyz> cross;
      gabi::call(0x0201B080, gabi::ea(axis.get()), gabi::ea(cross.get()),
                 gabi::ea(localWind.get()));
      f32 scale = lf(self + 0x1aec);
      u16 angle = (u16)gabi::ftoi((1400.f * power) * scale);
      f32 sine = sin_angle(angle);
      gabi::Local<Quat> rotation;
      rotation->x = sine * lf(gabi::ea(cross.get()));
      rotation->y = sine * lf(gabi::ea(cross.get()) + 4);
      rotation->z = sine * lf(gabi::ea(cross.get()) + 8);
      rotation->w = cos_angle(angle);
      s16 desired = angle_float((power * 360.f) * scale);
      gabi::call(0x0200F428, self + 0x3c8 + i * 2,
                 desired > 220 ? 220 : desired, 4, 0x20);
      f32 random = gabi::call<f32>(0x02019918, 256.f);
      f32 bend = lf(self + 0x1aec);
      s16 delta = angle_float(gabi::fmadds(power * 2048.f, bend, random));
      s32 phase = sh(self + 0x3ac + i * 2) + delta;
      ss(self + 0x3ac + i * 2, phase);
      u16 windAngle = uh(self + 0x3c8 + i * 2);
      f32 wave = sin_angle(windAngle) * sin_angle((u16)phase);
      gabi::Local<Quat> oscillation;
      oscillation->x = wave;
      oscillation->y = 0.f;
      oscillation->z = wave;
      oscillation->w = cos_angle(windAngle);
      gabi::call(0x028E8B48, gabi::ea(rotation.get()),
                 gabi::ea(oscillation.get()), gabi::ea(target.get()));
    }
    u32 destination = self + 0x3e4 + i * 16;
    if (sh(self + 0x65c) == 1)
      words(gabi::ea(target.get()), destination, 4);
    else
      gabi::call(0x028E9BC0, destination, gabi::ea(target.get()), destination,
                 0.4000000059604645f);
  }
}
VERIFY(0x023B8388, quaternion_main);
void calc_dif_angle(Actor *a) {
  WWHD_FUNC(0x023B86B8, void, a);
  u32 self = gabi::ea(a);
  gabi::Local<csXyz> desired;
  for (u32 i = 0; i < 14; ++i) {
    gabi::call(0x0201A478, gabi::ea(desired.get()), 0, 0, 0);
    s32 state = (s32)rd(self + 0x1ae0), rate = 2;
    if (state == 2) {
      u8 kind = byte(0x101CDD44 + i);
      if (kind == 2) {
        f32 wave = lf(self + 0x61c) * sin_angle(uh(self + 0x620 + i * 2));
        ss(gabi::ea(desired.get()), angle_float(20.f * wave));
        s16 yz = angle_float(40.f * wave);
        ss(gabi::ea(desired.get()) + 2, yz);
        ss(gabi::ea(desired.get()) + 4, yz);
      } else if (kind == 1) {
        f32 amplitude = lf(self + 0x61c);
        f32 wave = amplitude * sin_angle(uh(self + 0x620 + i * 2));
        ss(gabi::ea(desired.get()), angle_float(120.f * wave));
        ss(gabi::ea(desired.get()) + 2, angle_float(180.f * wave));
        s32 z = angle_float(220.f * wave);
        if (i == 1)
          z += angle_float(gabi::fmadds(3200.f, amplitude, -3200.f));
        ss(gabi::ea(desired.get()) + 4, z);
      } else if (kind == 0) {
        if (!rd(0x1046C9DC)) {
          put(0x1046C9DC, 1);
          const s32 z[14] = {0, 0,    0, 0, 0,     0,     0,
                             0, 5000, 0, 0, -5000, -7000, -2700};
          for (u32 j = 0; j < 14; ++j)
            gabi::call(0x0201A478, 0x1046C988 + j * 6, 0, 0, z[j]);
        }
        f32 wave = lf(self + 0x61c) * sin_angle(uh(self + 0x620 + i * 2));
        u32 base = 0x1046C988 + i * 6;
        s32 x = angle_float(700.f * wave) + sh(base);
        s16 yz = angle_float(1700.f * wave);
        ss(gabi::ea(desired.get()), x);
        ss(gabi::ea(desired.get()) + 2, (s32)yz + sh(base + 2));
        ss(gabi::ea(desired.get()) + 4, (s32)yz + sh(base + 4));
        rate = 1;
      }
    } else if (state == 3 && rd(self + 0x1adc) == 0 &&
               (i == 0 || i == 1 || i == 6)) {
      ss(gabi::ea(desired.get()) + 4,
         angle_float(lf(self + 0x1ae4) * sin_angle(uh(self + 0x1ae8))));
    }
    u32 rotation = self + 0x4c4 + i * 6;
    gabi::call(0x0200F428, rotation, sh(gabi::ea(desired.get())), rate, 0x4000);
    gabi::call(0x0200F428, rotation + 2, sh(gabi::ea(desired.get()) + 2), rate,
               0x4000);
    gabi::call(0x0200F428, rotation + 4, sh(gabi::ea(desired.get()) + 4), rate,
               0x4000);
    u8 kind = byte(0x101CDD44 + i);
    s32 increment = sh(self + 0x63c + i * 2);
    if (kind == 0)
      increment = angle_float((f32)increment * 1.5f);
    ss(self + 0x620 + i * 2, (s32)sh(self + 0x620 + i * 2) + increment);
  }
}
VERIFY(0x023B86B8, calc_dif_angle);
void leaf_scale_main(Actor *a) {
  WWHD_FUNC(0x023B8DC8, void, a);
  u32 self = gabi::ea(a);
  f32 x = 1.f, y = 1.f, z = 1.f;
  if (rd(self + 0x1ae0) == 2) {
    f32 amplitude = lf(self + 0x61c);
    y = gabi::fmadds(-0.5f, amplitude, 1.f);
    z = y;
    x = gabi::fmadds(0.3500000238418579f, amplitude, 1.f);
  }
  gabi::call(0x0200ED84, self + 0x5c0, x, 0.5f, 0.5f);
  gabi::call(0x0200ED84, self + 0x5c4, y, 0.5f, 0.5f);
  gabi::call(0x0200ED84, self + 0x5c8, z, 0.5f, 0.5f);
}
VERIFY(0x023B8DC8, leaf_scale_main);
BOOL execute(Actor *a) {
  WWHD_FUNC(0x023B8EB0, BOOL, a);
  u32 self = gabi::ea(a);
  if (rd(self + 0x1ae0) != 0) {
    put(self + 0x1adc, gabi::call<s32>(0x023B7F4C, a));
    gabi::call(0x023B7F84, a);
    gabi::call(0x023B7FDC, a);
    gabi::call(0x023B8388, a);
    gabi::call(0x023B86B8, a);
    gabi::call(0x023B8DC8, a);
    gabi::call(0x023B7668, a);
    gabi::call(0x025D69FC, a, 7, 79.f);
  }
  return 1;
}
VERIFY(0x023B8EB0, execute);
BOOL draw(Actor *a) {
  WWHD_FUNC(0x023B8F38, BOOL, a);
  u32 self = gabi::ea(a);
  if (rd(self + 0x1ae0) != 0) {
    u32 env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, env, 1, self + 0x314, self + 0x110);
    u32 morf = rd(self + 0x604);
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, env, rd(morf + 0x90), self + 0x110);
    put(0x104B4634, rd(play() + 0x5d70));
    put(0x104B4638, rd(play() + 0x5d74));
    gabi::call(0x025E54D8, rd(self + 0x604));
    put(0x104B4634, rd(play() + 0x5d78));
    put(0x104B4638, rd(play() + 0x5d7c));
  }
  return 1;
}
VERIFY(0x023B8F38, draw);
BOOL set_stop_animation(Actor *a, void *animation, f32 speed, f32 morph) {
  WWHD_FUNC(0x023B8FDC, BOOL, a, animation, speed, morph);
  if (!animation)
    return 0;
  u32 self = gabi::ea(a);
  gabi::call(0x025E4A98, rd(self + 0x604), animation, 0, 0, morph, speed, 0.f,
             -1.f);
  put(self + 0x1adc, 1);
  return 1;
}
VERIFY(0x023B8FDC, set_stop_animation);
BOOL sag_init(Actor *a) {
  WWHD_FUNC(0x023B9058, BOOL, a);
  u32 self = gabi::ea(a);
  if (!gabi::call<BOOL>(0x023B8FDC, a, rd(self + 0x608), 1.f, 0.f))
    return 0;
  sf(rd(self + 0x604) + 0x98, 0.f);
  return 1;
}
VERIFY(0x023B9058, sag_init);
void sag_main(Actor *a) {
  WWHD_FUNC(0x023B90EC, void, a);
  gabi::Local<be<u16>> profile;
  *profile = 0xa1;
  u32 source =
      gabi::call<u32>(0x025D5218, 0x025E121Cu, gabi::ea(profile.get()));
  if (source) {
    u32 self = gabi::ea(a);
    words(source + 0x314, self + 0x60c, 3);
    ss(self + 0x618, sh(source + 0x32a));
    gabi::call(0x023B7528, a, 2);
  }
}
VERIFY(0x023B90EC, sag_main);
BOOL sag_wind_init(Actor *a) {
  WWHD_FUNC(0x023B9160, BOOL, a);
  u32 self = gabi::ea(a);
  if (!gabi::call<BOOL>(0x023B8FDC, a, rd(self + 0x608), 1.f, 3.f))
    return 0;
  f32 distance = gabi::call<f32>(0x028E8DE8, self + 0x60c, self + 0x314);
  distance = gabi::call<f32>(0x028F4384, distance);
  // Ordered PPC comparisons also retain NaNs in the unclamped branch.
  if (distance > 2800.f)
    distance = 2800.f;
  else if (distance < 1000.f)
    distance = 1000.f;
  f32 amplitude = (distance - 2800.f) / -1800.f;
  f32 velocity = gabi::fmadds(7000.f, amplitude, 5000.f);
  sf(self + 0x61c, amplitude);
  s16 half = angle_float(velocity * 0.5f);
  for (u32 i = 0; i < 14; ++i) {
    s16 value = half;
    if (byte(0x101CDD44 + i) == 0) {
      f32 random = gabi::call<f32>(0x020198D8, 2000.f);
      f32 speed = velocity + random;
      value = angle_float((i & 1) ? -speed : speed);
    }
    ss(self + 0x63c + i * 2, value);
  }
  sf(rd(self + 0x604) + 0x98, 0.f);
  return 1;
}
VERIFY(0x023B9160, sag_wind_init);
void sag_wind_main(Actor *a) {
  WWHD_FUNC(0x023B9390, void, a);
  if (switch_on(a))
    gabi::call(0x023B7528, a, 3);
}
VERIFY(0x023B9390, sag_wind_main);
BOOL to_normal_init(Actor *a) {
  WWHD_FUNC(0x023B93F4, BOOL, a);
  return gabi::call<BOOL>(0x023B8FDC, a, rd(gabi::ea(a) + 0x608), 1.f, 0.f);
}
VERIFY(0x023B93F4, to_normal_init);
void to_normal_main(Actor *a) {
  WWHD_FUNC(0x023B940C, void, a);
  u32 self = gabi::ea(a);
  if (rd(self + 0x1adc) == 0) {
    f32 amplitude = lf(self + 0x1ae4);
    if (!(std::fabs(amplitude) > 0.10000000149011612f)) {
      if (gabi::call<BOOL>(0x023B7528, a, 4)) {
        ss(self + 0x65c, 2);
        sf(self + 0x1aec, 0.f);
        sf(self + 0x1ae4, 0.f);
      }
      amplitude = lf(self + 0x1ae4);
    }
    s32 phase = sh(self + 0x1ae8) + 0x3000;
    ss(self + 0x1ae8, phase);
    sf(self + 0x1ae4, amplitude * 0.8500000238418579f);
  } else {
    f32 amplitude = -1792.f * lf(self + 0x61c);
    ss(self + 0x1ae8, 0);
    sf(self + 0x1ae4, amplitude);
  }
}
VERIFY(0x023B940C, to_normal_main);
BOOL normal_init(Actor *a) {
  WWHD_FUNC(0x023B94D4, BOOL, a);
  u32 self = gabi::ea(a);
  if (!gabi::call<BOOL>(0x023B8FDC, a, rd(self + 0x608), -1.f, 0.f))
    return 0;
  sf(rd(self + 0x604) + 0x98, 0.f);
  return 1;
}
VERIFY(0x023B94D4, normal_init);
f32 normal_main(Actor *a) {
  WWHD_FUNC(0x023B9568, f32, a);
  u32 self = gabi::ea(a);
  s16 counter = sh(self + 0x65c);
  if (counter == 0 || counter == 1)
    ss(self + 0x65c, counter + 1);
  return gabi::call<f32>(0x0200ECD4, self + 0x1aec, 1.f, 0.009999999776482582f,
                         1.f, 0.007000000216066837f);
}
VERIFY(0x023B9568, normal_main);
s32 create_adapter(Actor *a) {
  WWHD_FUNC(0x023B95A8, s32, a);
  return create(a);
}
VERIFY(0x023B95A8, create_adapter);
u8 delete_adapter(Actor *a) {
  WWHD_FUNC(0x023B95AC, u8, a);
  return remove(a);
}
VERIFY(0x023B95AC, delete_adapter);
BOOL execute_adapter(Actor *a) {
  WWHD_FUNC(0x023B95B0, BOOL, a);
  return execute(a);
}
VERIFY(0x023B95B0, execute_adapter);
BOOL draw_adapter(Actor *a) {
  WWHD_FUNC(0x023B95B4, BOOL, a);
  return draw(a);
}
VERIFY(0x023B95B4, draw_adapter);
void sinit() {
  WWHD_FUNC(0x023B95B8, void);
  put(0x1046C980, 0);
  put(0x1046C978, 0);
  put(0x1046C984, 0);
  put(0x1046C97C, 0);
  gabi::call(0x028F026C, 0x101CDD54u);
  f32 low = lf(0x100335AC), high = lf(0x100335B0);
  sf(0x1046C96C, low);
  sf(0x1046C970, high);
  gabi::call(0x028ED6F8, 0x1046C974u);
  gabi::call(0x028F026C, 0x101CDD60u);
  gabi::call(0x028EAB2C, 0x1046C975u);
  gabi::call(0x028F026C, 0x101CDD6Cu);
}
VERIFY(0x023B95B8, sinit);
void static_destructor(void *object, u32 flags) {
  WWHD_FUNC(0x023B964C, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x023B964C, static_destructor);
void *status_ctor(void *object) {
  WWHD_FUNC(0x023B9660, void *, object);
  u32 self = gabi::ea(object);
  if (!self) {
    self = gabi::call<u32>(0x0273AD10, 0x3c);
    if (!self)
      return nullptr;
  }
  gabi::call(0x0200BD2C, self);
  gabi::call(0x02515DA0, self + 0x1c);
  put(self + 0x18, 0x1004AE88);
  put(self + 0x1c, 0x1004AEC0);
  return gabi::at<void>(self);
}
VERIFY(0x023B9660, status_ctor);
void *capsule_ctor(void *object) {
  WWHD_FUNC(0x023B96C8, void *, object);
  u32 self = gabi::ea(object);
  if (!self) {
    self = gabi::call<u32>(0x0273AD10, 0x138);
    if (!self)
      return nullptr;
  }
  gabi::call(0x02515FB8, self);
  put(self + 0x114, 0x100015A8);
  put(self + 0x110, 0x10033368);
  gabi::call(0x02018150, self + 0x118);
  put(self + 0x3c, 0x1004AF18);
  put(self + 0x130, 0x1004AF60);
  put(self + 0x114, 0x1004AF70);
  return gabi::at<void>(self);
}
VERIFY(0x023B96C8, capsule_ctor);
void capsule_noop(void *object) { WWHD_FUNC(0x023B9754, void, object); }
VERIFY(0x023B9754, capsule_noop);
void destructor(Actor *a, u32 flags) {
  WWHD_FUNC(0x023B9758, void, a, flags);
  if (!a)
    return;
  u32 self = gabi::ea(a);
  gabi::call(0x028F0164, self + 0x117c, 8, 0x12c, 0x02515AE8u, 0, 0);
  gabi::call(0x028F0164, self + 0xf9c, 8, 0x3c, 0x02515860u, 0, 0);
  gabi::call(0x028F0164, self + 0x8f8, 5, 0x138, 0x02515980u, 0, 0);
  gabi::call(0x028F0164, self + 0x7cc, 5, 0x3c, 0x02515860u, 0, 0);
  gabi::call(0x02515A70, self + 0x69c, 2);
  gabi::call(0x02515860, self + 0x660, 2);
  gabi::call(0x025D50BC, a, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, a);
}
VERIFY(0x023B9758, destructor);
BOOL none_init(Actor *a) {
  WWHD_FUNC(0x023B9848, BOOL, a);
  return 1;
}
VERIFY(0x023B9848, none_init);
void none_main(Actor *a) { WWHD_FUNC(0x023B9850, void, a); }
VERIFY(0x023B9850, none_main);
BOOL IsDelete(Actor *a) {
  WWHD_FUNC(0x023B9854, BOOL, a);
  return 1;
}
VERIFY(0x023B9854, IsDelete);
} // namespace

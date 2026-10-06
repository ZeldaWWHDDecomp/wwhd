#include "d/actor/d_a_stone.h"
using daStone::Act_c;
using gabi::load;
using gabi::store;
static u32 address(const void *p) { return gabi::ea(p); }

// A nonleaf guest callee may save LR at caller SP+4. Keep the payload
// above its 32-byte linkage area throughout every call using the object.
struct StoneLinkage {
  u8 bytes[32];
};
template <class T> class StoneLocal {
  gabi::Local<T> payload;
  gabi::Local<StoneLinkage> linkage;

public:
  T *get() const { return payload.get(); }
  T *operator->() const { return get(); }
};
static u32 data(Act_c *actor, u32 file, u32 function) {
  u32 type = load<u32>(address(actor) + 0x78C);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(file), 0x306, STR(function));
    type = load<u32>(address(actor) + 0x78C);
  }
  return 0x1003E044u + type * 0xB4u;
}
u32 parameter(Act_c *actor, s32 width, s32 shift) {
  WWHD_FUNC(0x0249E190, u32, actor, width, shift);
  u32 value = load<u32>(address(actor) + 0xB0);
  u32 leftCount = u32(width) & 63u, rightCount = u32(shift) & 63u;
  u32 mask = (leftCount < 32 ? 1u << leftCount : 0u) - 1u;
  return (rightCount < 32 ? value >> rightCount : 0u) & mask;
}
VERIFY(0x0249E190, parameter);
s32 chk_appear(Act_c *actor) {
  WWHD_FUNC(0x0249AB20, s32, actor);
  u32 sw = parameter(actor, 8, 8);
  u32 record = data(actor, 0x1003D8E8, 0x1003D8F8);
  if ((load<u32>(record + 0x70) & 8) && sw != 255) {
    u32 save = load<u32>(0x101F84DC);
    s8 room = load<s8>(address(actor) + 0x2FE);
    return !gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), sw, room);
  }
  return 1;
}
VERIFY(0x0249AB20, chk_appear);
void set_sound_environment(Act_c *actor, u32 firstSoundParameter,
                           u32 secondSoundParameter) {
  WWHD_FUNC(0x0249BE4C, void, actor, firstSoundParameter, secondSoundParameter);
  StoneLocal<cXyz> position;
  f32 x = load<f32>(address(actor) + 0x314);
  f32 y = load<f32>(address(actor) + 0x318);
  position->x = x;
  f32 z = load<f32>(address(actor) + 0x31C);
  u32 owner = load<u32>(address(actor) + 4);
  position->y = y;
  position->z = z;
  gabi::call<void>(0x0255F458, position.get(), firstSoundParameter, owner,
                   secondSoundParameter);
}
VERIFY(0x0249BE4C, set_sound_environment);
u32 chk_sink_water(Act_c *actor) {
  WWHD_FUNC(0x0249C284, u32, actor);
  return load<u8>(address(actor) + 0x61C);
}
VERIFY(0x0249C284, chk_sink_water);
s32 chk_sink_lava(Act_c *actor) {
  WWHD_FUNC(0x0249C28C, s32, actor);
  u32 record = data(actor, 0x1003DBA0, 0x1003DBB0);
  f32 minimum = load<f32>(0x1003DB9C);
  f32 offset = load<f32>(record + 4);
  if (offset < minimum) {
    // The second access asserts again and can reload an externally changed
    // type.
    record = data(actor, 0x1003DBA0, 0x1003DBB0);
    minimum = load<f32>(record + 4);
  }
  f32 threshold = load<f32>(address(actor) + 0x318) + minimum;
  return load<f32>(address(actor) + 0x610) > threshold;
}
VERIFY(0x0249C28C, chk_sink_lava);
void init_rot_clean(Act_c *actor) {
  WWHD_FUNC(0x0249C578, void, actor);
  for (u32 offset : {0x798u, 0x79Au, 0x79Cu, 0x79Eu, 0x7A0u})
    gabi::call<void>(0x02006638, gabi::at<void>(address(actor) + offset),
                     gabi::at<void>(0x101FF354));
  s16 yaw = load<s16>(address(actor) + 0x322);
  gabi::call<void>(0x02006584, gabi::at<void>(address(actor) + 0x7A2), yaw);
}
VERIFY(0x0249C578, init_rot_clean);
void mode_carry_init(Act_c *actor) {
  WWHD_FUNC(0x0249CB44, void, actor);
  u32 a = address(actor);
  u32 at = load<u32>(a + 0x674);
  u32 flags = load<u32>(a + 0x3E0) & 0xFFFFDBF1u;
  u32 co = load<u32>(a + 0x688);
  store<u8>(a + 0x7A9, 0);
  at |= 1;
  u32 attention = load<u32>(a + 0x39C);
  u32 target = load<u32>(a + 0x65C);
  store<u32>(a + 0x3E0, flags);
  store<u32>(a + 0x674, at);
  store<u32>(a + 0x65C, target & ~1u);
  store<u32>(a + 0x790, 1);
  store<u32>(a + 0x39C, attention & ~0x10u);
  store<u32>(a + 0x688, co & ~1u);
}
VERIFY(0x0249CB44, mode_carry_init);
void cam_lockoff(Act_c *actor) {
  WWHD_FUNC(0x0249CDC4, void, actor);
  u32 play = address(gabi::call<void *>(0x025200D4));
  u32 camera = load<u32>(play + 0x5AF8);
  u32 owner = load<u32>(address(actor) + 4);
  gabi::call<void>(0x025052BC, gabi::at<void>(camera + 0x248), owner);
}
VERIFY(0x0249CDC4, cam_lockoff);
void eff_land_smoke(Act_c *actor) {
  WWHD_FUNC(0x0249CE00, void, actor);
  u32 record = data(actor, 0x1003DD74, 0x1003DD84);
  f32 scale = load<f32>(record + 0x3C);
  gabi::call<void>(0x02311CD8, actor, gabi::at<void>(address(actor) + 0x48C),
                   scale);
}
VERIFY(0x0249CE00, eff_land_smoke);
void delete_storage(void *object, s32 flags) {
  WWHD_FUNC(0x0249E0A4, void, object, flags);
  if (object && (u32(flags) & 1))
    gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x0249E0A4, delete_storage);
void empty_virtual() { WWHD_FUNC(0x0249E0B8, void); }
VERIFY(0x0249E0B8, empty_virtual);
s32 is_delete(void *actor) {
  WWHD_FUNC(0x0249E188, s32, actor);
  return 1;
}
VERIFY(0x0249E188, is_delete);

void init_mtx(Act_c *actor) {
  WWHD_FUNC(0x0249B128, void, actor);
  u32 type = load<u32>(address(actor) + 0x78C);
  u32 model = load<u32>(address(actor) + 0x3B4);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003D9E4), 0x306, STR(0x1003D9F4));
    type = load<u32>(address(actor) + 0x78C);
  }
  f32 scale = load<f32>(0x1003E050u + type * 0xB4u);
  StoneLocal<cXyz> result;
  gabi::call<void>(0x0201AE48, gabi::at<void>(address(actor) + 0x330),
                   result.get(), scale);
  f32 x = result->x, z = result->z, y = result->y;
  store<f32>(model + 0xC4, z);
  store<f32>(model + 0xBC, x);
  store<f32>(model + 0xC0, y);
  gabi::call<void>(0x0249AF50, actor);
}
VERIFY(0x0249B128, init_mtx);
s32 draw(Act_c *actor) {
  WWHD_FUNC(0x0249D4D0, s32, actor);
  if (parameter(actor, 2, 6) != 1) {
    u32 record = data(actor, 0x1003DE30, 0x1003DE40);
    u32 lightType = (load<u32>(record + 0x70) >> 2) & 1;
    void *environment = gabi::call<void *>(0x02555D0C);
    gabi::call<void>(0x025626A4, environment, lightType,
                     gabi::at<void>(address(actor) + 0x314),
                     gabi::at<void>(address(actor) + 0x110));
    environment = gabi::call<void *>(0x02555D0C);
    u32 model = load<u32>(address(actor) + 0x3B4);
    gabi::call<void>(0x02562F5C, environment, gabi::at<void>(model),
                     gabi::at<void>(address(actor) + 0x110));
    model = load<u32>(address(actor) + 0x3B4);
    gabi::call<void>(0x025E2DE0, gabi::at<void>(model), 0);
  }
  return 1;
}
VERIFY(0x0249D4D0, draw);
void water_tention(Act_c *actor) {
  WWHD_FUNC(0x0249DCF8, void, actor);
  if (chk_sink_water(actor)) {
    f32 current = load<f32>(address(actor) + 0x614);
    f32 invalid = load<f32>(0x1003D92C);
    if (current != invalid) {
      f32 previous = load<f32>(address(actor) + 0x618);
      if (previous != invalid) {
        f32 negativeRate = load<f32>(0x1003DF70);
        f32 difference = current - previous;
        f32 positiveRate = load<f32>(0x1003DF6C);
        f32 rate = difference >= 0 ? positiveRate : negativeRate;
        f32 y = load<f32>(address(actor) + 0x318);
        store<f32>(address(actor) + 0x318, gabi::fmadds(difference, rate, y));
      }
    }
  }
}
VERIFY(0x0249DCF8, water_tention);
void static_init() {
  WWHD_FUNC(0x0249DFF0, void);
  store<u32>(0x1046E040, 0);
  store<u32>(0x1046E03C, 0);
  store<u32>(0x1046E038, 0);
  store<u32>(0x1046E034, 0);
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101D10C0));
  f32 a = load<f32>(0x1003DFDC), b = load<f32>(0x1003DFE0);
  store<f32>(0x1046E028, a);
  store<f32>(0x1046E02C, b);
  gabi::call<void>(0x028ED6F8, gabi::at<void>(0x1046E030));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101D10CC));
  gabi::call<void>(0x028EAB2C, gabi::at<void>(0x1046E031));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101D10D8));
  f32 x = load<f32>(0x101FFBCC), y = load<f32>(0x101FFBD0);
  store<f32>(0x1046E044, x);
  f32 z = load<f32>(0x101FFBD4);
  store<f32>(0x1046E048, y);
  store<f32>(0x1046E04C, z);
}
VERIFY(0x0249DFF0, static_init);
void destroy_actor(Act_c *actor, s32 flags) {
  WWHD_FUNC(0x0249E0BC, void, actor, flags);
  if (actor) {
    u32 a = address(actor);
    gabi::call<void>(0x02515A70, gabi::at<void>(a + 0x65C), 2);
    gabi::call<void>(0x02515860, gabi::at<void>(a + 0x620), 2);
    store<u32>(a + 0x5DC, 0x1003D7F8);
    store<u32>(a + 0x5FC, 0x1003D818);
    store<u32>(a + 0x608, 0x1003D7D8);
    gabi::call<void>(0x02008DAC, gabi::at<void>(a + 0x5BC), 0);
    gabi::call<void>(0x02018034, gabi::at<void>(a + 0x590), 2);
    store<u32>(a + 0x3D8, 0x1003D8B8);
    store<u32>(a + 0x3CC, 0x1003D8C8);
    gabi::call<void>(0x024EFD9C, gabi::at<void>(a + 0x3B8), 0);
    gabi::call<void>(0x025D50BC, actor, 0);
    if (u32(flags) & 1)
      gabi::call<void>(0x0273AF40, actor);
  }
}
VERIFY(0x0249E0BC, destroy_actor);

s32 create_heap(Act_c *actor) {
  WWHD_FUNC(0x0249ABE8, s32, actor);
  u32 record = data(actor, 0x1003D930, 0x1003D940);
  u32 archive = load<u32>(record + 0x74);
  record = data(actor, 0x1003D930, 0x1003D940);
  u16 modelIndex = load<u16>(record + 0x7C);
  u32 resources = load<u32>(0x101F4F28);
  StoneLocal<SafeString> name;
  name->mStringTop = archive;
  name->__vtbl = 0x1003D7B0;
  void *modelData = gabi::call<void *>(0x026066C4, gabi::at<void>(resources),
                                       name.get(), modelIndex);
  if (!modelData)
    gabi::call<void>(0x0273AA24, STR(0x1003D930), 0x320, STR(0x1003D964));
  void *model = gabi::call<void *>(0x025E38E0, modelData, 0x80000, 0x11000002);
  store<u32>(address(actor) + 0x3B4, address(model));
  if (!model)
    return 0;
  record = data(actor, 0x1003D930, 0x1003D940);
  f32 radius = f32(load<s16>(record + 0x82));
  f32 wallHeight = load<f32>(0x1003D928);
  gabi::call<void>(0x024EFF44, gabi::at<void>(address(actor) + 0x57C),
                   wallHeight, radius);
  u32 a = address(actor);
  gabi::call<void>(0x024F06B4, gabi::at<void>(a + 0x3B8),
                   gabi::at<void>(a + 0x314), gabi::at<void>(a + 0x300), actor,
                   1, gabi::at<void>(a + 0x57C), gabi::at<void>(a + 0x33C),
                   gabi::at<void>(a + 0x320), gabi::at<void>(a + 0x328));
  u32 flags = load<u32>(a + 0x3E0) & 0xFFFFFBF7u;
  u32 type = load<u32>(a + 0x78C);
  store<u32>(a + 0x3E0, flags);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003D930), 0x306, STR(0x1003D940));
    type = load<u32>(a + 0x78C);
  }
  f32 groundOffset = f32(load<s16>(0x1003E0CAu + type * 0xB4u));
  f32 invalid = load<f32>(0x1003D92C);
  store<f32>(a + 0x610, invalid);
  store<f32>(a + 0x614, invalid);
  store<f32>(a + 0x618, invalid);
  store<u8>(a + 0x61C, 0);
  store<u8>(a + 0x61D, 0);
  store<f32>(a + 0x478, groundOffset);
  return 1;
}
VERIFY(0x0249ABE8, create_heap);
s32 create_heap_callback(Act_c *actor) {
  WWHD_FUNC(0x0249ADFC, s32, actor);
  return create_heap(actor);
}
VERIFY(0x0249ADFC, create_heap_callback);
s32 delete_actor(Act_c *actor) {
  WWHD_FUNC(0x0249BA60, s32, actor);
  if (load<u8>(address(actor) + 0x7A8)) {
    u32 record = data(actor, 0x1003DA90, 0x1003DAA0);
    if (load<u32>(record + 0x74) != 0x1003DFE8) {
      record = data(actor, 0x1003DA90, 0x1003DAA0);
      u32 archive = load<u32>(record + 0x74);
      gabi::call<void>(0x025204C8, gabi::at<void>(address(actor) + 0x3AC),
                       STR(archive));
    }
  }
  return 1;
}
VERIFY(0x0249BA60, delete_actor);
void cull_set_draw(Act_c *actor) {
  WWHD_FUNC(0x0249AE00, void, actor);
  u32 x = data(actor, 0x1003D974, 0x1003D984);
  u32 y = data(actor, 0x1003D974, 0x1003D984);
  u32 z = data(actor, 0x1003D974, 0x1003D984);
  u32 radius = data(actor, 0x1003D974, 0x1003D984);
  f32 fx = f32(load<s16>(x + 0xA8)), fy = f32(load<s16>(y + 0xAA));
  f32 fz = f32(load<s16>(z + 0xAC)), fr = f32(load<s16>(radius + 0xAE));
  gabi::call<void>(0x025D6768, actor, fx, fy, fz, fr);
}
VERIFY(0x0249AE00, cull_set_draw);

void cull_set_move(Act_c *actor) {
  WWHD_FUNC(0x0249BB2C, void, actor);
  u32 x = data(actor, 0x1003DAC4, 0x1003DAD4);
  u32 y = data(actor, 0x1003DAC4, 0x1003DAD4);
  u32 z = data(actor, 0x1003DAC4, 0x1003DAD4);
  u32 radius = data(actor, 0x1003DAC4, 0x1003DAD4);
  f32 fx = f32(load<s16>(x + 0xA0)), fy = f32(load<s16>(y + 0xA2));
  f32 fz = f32(load<s16>(z + 0xA4)), fr = f32(load<s16>(radius + 0xA6));
  gabi::call<void>(0x025D6768, actor, fx, fy, fz, fr);
}
VERIFY(0x0249BB2C, cull_set_move);
void set_mtx(Act_c *actor) {
  WWHD_FUNC(0x0249AF50, void, actor);
  u32 a = address(actor);
  s16 zero = load<s16>(0x101FF354), tilt = load<s16>(a + 0x79E);
  bool tilted = tilt != zero;
  f32 x = load<f32>(a + 0x314), z = load<f32>(a + 0x31C),
      y = load<f32>(a + 0x318);
  auto matrix = gabi::at<void>(0x1048D0CC);
  gabi::call<void>(0x028E93CC, matrix, x, y, z);
  f32 noTranslation = load<f32>(0x1003D9A8);
  if (tilted) {
    u32 record = data(actor, 0x1003D9B0, 0x1003D9C0);
    gabi::call<void>(0x025F24E0, noTranslation, load<f32>(record + 4),
                     noTranslation);
    tilt = load<s16>(a + 0x79E);
    f32 radians = f32(tilt) * load<f32>(0x1003D9AC);
    u32 rotation = load<u32>(0x1048D3FC);
    gabi::call<void>(0x028E9B14, gabi::at<void>(rotation),
                     gabi::at<void>(0x1046E044), radians);
    rotation = load<u32>(0x1048D3FC);
    gabi::call<void>(0x025F25CC, gabi::at<void>(rotation));
  }
  s16 rz = load<s16>(a + 0x32C), rx = load<s16>(a + 0x328),
      ry = load<s16>(a + 0x32A);
  gabi::call<void>(0x025F1B48, matrix, rx, ry, rz);
  if (tilted) {
    u32 record = data(actor, 0x1003D9B0, 0x1003D9C0);
    f32 height = load<f32>(record + 4);
    gabi::call<void>(0x025F24E0, noTranslation, -height, noTranslation);
  }
  f32 values[12];
  for (u32 i = 0; i != 12; ++i)
    values[i] = load<f32>(0x1048D0CC + i * 4);
  u32 model = load<u32>(a + 0x3B4);
  // GHS snapshots the full matrix before writing the model transform.
  for (u32 i : {5u, 6u, 7u, 9u, 10u, 0u, 1u, 2u, 3u, 4u, 8u, 11u})
    store<f32>(model + 0xC8 + i * 4, values[i]);
}
VERIFY(0x0249AF50, set_mtx);
void mode_wait(Act_c *actor) {
  WWHD_FUNC(0x0249D588, void, actor);
  u32 a = address(actor);
  if (load<u32>(a + 0x3E0) & 0x20) {
    gabi::call<void>(0x025D6870, actor, gabi::at<void>(a + 0x620));
    store<u32>(a + 0x39C, load<u32>(a + 0x39C) | 0x10);
  } else {
    u32 first = data(actor, 0x1003DE64, 0x1003DE74);
    u32 second = data(actor, 0x1003DE64, 0x1003DE74);
    f32 friction = load<f32>(first + 0x20), speed = load<f32>(second + 0x24);
    gabi::call<void>(0x023123C0, actor, gabi::at<void>(a + 0x620),
                     gabi::at<void>(0x101FFBA8), friction, speed);
    store<u32>(a + 0x39C, load<u32>(a + 0x39C) & ~0x10u);
  }
}
VERIFY(0x0249D588, mode_wait);

void mode_wait_init(Act_c *actor) {
  WWHD_FUNC(0x0249B1C0, void, actor);
  u32 a = address(actor);
  u32 co = load<u32>(a + 0x688), flags = load<u32>(a + 0x3E0);
  u32 at = load<u32>(a + 0x674);
  f32 zero = load<f32>(0x1003D9A8);
  u32 target = load<u32>(a + 0x65C);
  store<u32>(a + 0x674, at | 1);
  store<u32>(a + 0x3E0, (flags | 8) & 0xFFFFDBF9u);
  u32 type = load<u32>(a + 0x78C);
  store<u32>(a + 0x65C, target & ~1u);
  store<f32>(a + 0x370, zero);
  store<u32>(a + 0x688, co | 1);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DA18), 0x306, STR(0x1003DA28));
    type = load<u32>(a + 0x78C);
  }
  u32 record = 0x1003E044u + type * 0xB4u;
  type = load<u32>(a + 0x78C);
  f32 gravity = load<f32>(record);
  store<f32>(a + 0x374, gravity);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DA18), 0x306, STR(0x1003DA28));
    type = load<u32>(a + 0x78C);
  }
  u8 weight = load<u8>(0x1003E054u + type * 0xB4u);
  gabi::call<void>(0x02515F14, gabi::at<void>(a + 0x620), weight, 255, actor);
  store<u32>(a + 0x790, 0);
}
VERIFY(0x0249B1C0, mode_wait_init);
void eff_hit_water_splash(Act_c *actor) {
  WWHD_FUNC(0x0249C4C4, void, actor);
  StoneLocal<cXyz> position;
  f32 z = load<f32>(address(actor) + 0x31C),
      y = load<f32>(address(actor) + 0x614);
  u32 type = load<u32>(address(actor) + 0x78C);
  f32 x = load<f32>(address(actor) + 0x314);
  position->z = z;
  position->x = x;
  position->y = y;
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DC08), 0x306, STR(0x1003DC18));
    type = load<u32>(address(actor) + 0x78C);
  }
  u32 first = 0x1003E044u + type * 0xB4u;
  u32 second = data(actor, 0x1003DC08, 0x1003DC18);
  f32 scale = load<f32>(first + 0x5C), height = load<f32>(second + 0x60);
  gabi::call<void>(0x025DAE64, position.get(), 0, scale, height);
}
VERIFY(0x0249C4C4, eff_hit_water_splash);
void eff_hit_lava_splash(Act_c *actor) {
  WWHD_FUNC(0x0249C914, void, actor);
  StoneLocal<cXyz> position;
  f32 x = load<f32>(address(actor) + 0x314),
      y = load<f32>(address(actor) + 0x610);
  position->x = x;
  u32 type = load<u32>(address(actor) + 0x78C);
  f32 z = load<f32>(address(actor) + 0x31C);
  position->y = y;
  position->z = z;
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DCA4), 0x306, STR(0x1003DCB4));
    type = load<u32>(address(actor) + 0x78C);
  }
  f32 scale = load<f32>(0x1003E0A8u + type * 0xB4u);
  gabi::call<void>(0x025DAF3C, position.get(), scale);
}
VERIFY(0x0249C914, eff_hit_lava_splash);
void mode_drop(Act_c *actor) {
  WWHD_FUNC(0x0249DB04, void, actor);
  gabi::call<void>(0x02312968, actor, gabi::at<void>(address(actor) + 0x48C));
  void *movement = gabi::call<void *>(0x02311F80, actor, load<f32>(0x1003DF34));
  u32 first = data(actor, 0x1003DF38, 0x1003DF48);
  u32 second = data(actor, 0x1003DF38, 0x1003DF48);
  f32 friction = load<f32>(first + 0x20), speed = load<f32>(second + 0x24);
  gabi::call<void>(0x023123C0, actor, gabi::at<void>(address(actor) + 0x620),
                   movement, friction, speed);
  gabi::call<void>(0x0249D948, actor);
}
VERIFY(0x0249DB04, mode_drop);

static void update_rotation(Act_c *actor, u32 linearOffset, u32 quadraticOffset,
                            u32 file, u32 function) {
  for (u32 velocityOffset : {0x79Cu, 0x7A0u}) {
    auto velocity = gabi::at<void>(address(actor) + velocityOffset);
    f32 value = gabi::call<f32>(0x02006760, velocity);
    u32 first = data(actor, file, function),
        second = data(actor, file, function);
    f32 signedSquare = std::fabs(value) * value;
    f32 quadratic = signedSquare * load<f32>(second + quadraticOffset);
    f32 linear = load<f32>(first + linearOffset);
    s32 adjustment =
        gabi::call<s32>(0x02019510, gabi::fmadds(value, linear, quadratic));
    gabi::call<void>(0x0200692C, velocity, adjustment);
    gabi::call<void>(0x020068CC,
                     gabi::at<void>(address(actor) + velocityOffset - 2),
                     velocity);
  }
  auto matrix = gabi::at<void>(0x1048D0CC);
  gabi::call<void>(0x025F1884, matrix, load<s16>(address(actor) + 0x7A2));
  gabi::call<void>(0x025F1BF4, matrix, load<s16>(address(actor) + 0x79A));
  gabi::call<void>(0x025F1C5C, matrix, load<s16>(address(actor) + 0x798));
  gabi::call<void>(0x025F1C28, matrix, 0x4000);
  gabi::call<void>(0x028E9044, matrix, gabi::at<void>(0x101FFBCC),
                   gabi::at<void>(0x1046E044));
}
void set_drop_rot(Act_c *actor) {
  WWHD_FUNC(0x0249D948, void, actor);
  update_rotation(actor, 0x30, 0x34, 0x1003DF00, 0x1003DF10);
}
VERIFY(0x0249D948, set_drop_rot);
void set_sink_rot(Act_c *actor) {
  WWHD_FUNC(0x0249DD68, void, actor);
  update_rotation(actor, 0x50, 0x54, 0x1003DF74, 0x1003DF84);
}
VERIFY(0x0249DD68, set_sink_rot);
void mode_sink(Act_c *actor) {
  WWHD_FUNC(0x0249DF24, void, actor);
  gabi::call<void>(0x02312968, actor, gabi::at<void>(address(actor) + 0x48C));
  water_tention(actor);
  u32 first = data(actor, 0x1003DFA8, 0x1003DFB8),
      second = data(actor, 0x1003DFA8, 0x1003DFB8);
  f32 friction = load<f32>(first + 0x48), speed = load<f32>(second + 0x4C);
  gabi::call<void>(0x023123C0, actor, gabi::at<void>(address(actor) + 0x620),
                   gabi::at<void>(0x101FFBA8), friction, speed);
  set_sink_rot(actor);
}
VERIFY(0x0249DF24, mode_sink);
void init_rot_throw(Act_c *actor) {
  WWHD_FUNC(0x0249D670, void, actor);
  u32 record = data(actor, 0x1003DE98, 0x1003DEA8);
  gabi::call<void>(0x02006584, gabi::at<void>(address(actor) + 0x798),
                   load<s16>(record + 0x28));
  f32 random = gabi::call<f32>(0x02019788);
  gabi::call<void>(0x020069A0, gabi::at<void>(address(actor) + 0x798), random);
  f32 angle = gabi::call<f32>(0x02019918, load<f32>(0x1003DA4C));
  s16 yaw = s16(u16(gabi::ftoi(angle)));
  gabi::call<void>(0x02006584, gabi::at<void>(address(actor) + 0x79A), yaw);
  record = data(actor, 0x1003DE98, 0x1003DEA8);
  gabi::call<void>(0x02006584, gabi::at<void>(address(actor) + 0x79C),
                   load<s16>(record + 0x2A));
  gabi::call<void>(0x02006638, gabi::at<void>(address(actor) + 0x79E),
                   gabi::at<void>(0x101FF354));
  record = data(actor, 0x1003DE98, 0x1003DEA8);
  gabi::call<void>(0x02006584, gabi::at<void>(address(actor) + 0x7A0),
                   load<s16>(record + 0x2C));
  yaw = load<s16>(address(actor) + 0x322);
  gabi::call<void>(0x02006584, gabi::at<void>(address(actor) + 0x7A2), yaw);
}
VERIFY(0x0249D670, init_rot_throw);

void bg_crr_lava(Act_c *actor) {
  WWHD_FUNC(0x0249CD04, void, actor);
  u32 type = load<u32>(address(actor) + 0x78C);
  f32 x = load<f32>(address(actor) + 0x314);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DD40), 0x306, STR(0x1003DD50));
    type = load<u32>(address(actor) + 0x78C);
  }
  f32 z = load<f32>(address(actor) + 0x31C),
      oldY = load<f32>(address(actor) + 0x304);
  f32 offset = load<f32>(0x1003E048u + type * 0xB4u);
  f32 height = (oldY + offset) + load<f32>(0x1003D7A8);
  store<f32>(address(actor) + 0x5E0, x);
  store<f32>(address(actor) + 0x5E8, z);
  store<f32>(address(actor) + 0x5E4, height);
  void *play = gabi::call<void *>(0x025200D4);
  f32 ground =
      gabi::call<f32>(0x02008974, gabi::at<void>(address(play) + 0x12A0),
                      gabi::at<void>(address(actor) + 0x5BC));
  store<f32>(address(actor) + 0x610, ground);
}
VERIFY(0x0249CD04, bg_crr_lava);
void mode_drop_init(Act_c *actor) {
  WWHD_FUNC(0x0249D7A8, void, actor);
  u32 play = address(gabi::call<void *>(0x025200D4));
  u32 type = load<u32>(address(actor) + 0x78C);
  u32 player = load<u32>(play + 0x5B2C);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DECC), 0x306, STR(0x1003DEDC));
    type = load<u32>(address(actor) + 0x78C);
  }
  u32 first = 0x1003E044u + type * 0xB4u;
  u32 second = data(actor, 0x1003DECC, 0x1003DEDC);
  u32 a = address(actor);
  u32 target = load<u32>(a + 0x65C);
  f32 playerSpeed = load<f32>(player + 0x370);
  u32 co = load<u32>(a + 0x688), at = load<u32>(a + 0x674);
  f32 base = load<f32>(first + 0x18), multiplier = load<f32>(second + 0x1C);
  type = load<u32>(a + 0x78C);
  f32 speed = gabi::fmadds(playerSpeed, multiplier, base);
  store<u32>(a + 0x674, at | 1);
  store<u32>(a + 0x65C, target | 1);
  store<u32>(a + 0x688, co | 1);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DECC), 0x306, STR(0x1003DEDC));
    type = load<u32>(a + 0x78C);
  }
  u8 weight = load<u8>(0x1003E054u + type * 0xB4u);
  gabi::call<void>(0x02515F14, gabi::at<void>(a + 0x620), weight, 255, actor);
  u32 flags = load<u32>(a + 0x3E0);
  type = load<u32>(a + 0x78C);
  u32 attention = load<u32>(a + 0x39C);
  store<u32>(a + 0x3E0, (flags & 0xFFFFFBF1u) | 0x2000);
  store<u32>(a + 0x39C, attention & ~0x10u);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DECC), 0x306, STR(0x1003DEDC));
    type = load<u32>(a + 0x78C);
  }
  u32 record = 0x1003E044u + type * 0xB4u;
  type = load<u32>(a + 0x78C);
  f32 verticalSpeed = load<f32>(record + 0x14);
  store<f32>(a + 0x370, speed);
  store<f32>(a + 0x340, verticalSpeed);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DECC), 0x306, STR(0x1003DEDC));
    type = load<u32>(a + 0x78C);
  }
  f32 gravity = load<f32>(0x1003E044u + type * 0xB4u);
  store<u32>(a + 0x790, 2);
  store<f32>(a + 0x374, gravity);
}
VERIFY(0x0249D7A8, mode_drop_init);
void mode_carry(Act_c *actor) {
  WWHD_FUNC(0x0249DBC8, void, actor);
  u32 a = address(actor);
  if (load<u32>(a + 0x2E0) & 0x2000) {
    u32 play = address(gabi::call<void *>(0x025200D4));
    u8 started = load<u8>(a + 0x7A9);
    u32 player = load<u32>(play + 0x5B2C);
    if (!started) {
      u32 vtable = load<u32>(player + 0xB4), target = load<u32>(vtable + 0xBC);
      u32 carried = gabi::call<u32>(target, gabi::at<void>(player));
      if (carried == load<u32>(a + 4)) {
        if (load<u32>(player + 0x3C0) & 0x8000)
          store<u8>(a + 0x7A9, 1);
      } else
        store<u8>(a + 0x7A9, 0);
    }
  } else {
    f32 speed = load<f32>(a + 0x370), zero = load<f32>(0x1003D9A8),
        y = load<f32>(a + 0x318);
    store<f32>(a + 0x7A4, y);
    if (speed > zero || (load<u32>(a + 0x2E0) & 0x10000)) {
      init_rot_throw(actor);
      mode_drop_init(actor);
      mode_drop(actor);
    } else {
      gabi::call<void>(0x02312968, actor, gabi::at<void>(a + 0x48C));
      store<u8>(a + 0x797, 2);
      mode_wait_init(actor);
    }
  }
}
VERIFY(0x0249DBC8, mode_carry);

void mode_sink_init(Act_c *actor) {
  WWHD_FUNC(0x0249C5F4, void, actor);
  u32 a = address(actor);
  u32 at = load<u32>(a + 0x674), target = load<u32>(a + 0x65C),
      type = load<u32>(a + 0x78C);
  u32 co = load<u32>(a + 0x688);
  store<u32>(a + 0x65C, target & ~1u);
  store<u32>(a + 0x674, at | 1);
  store<u32>(a + 0x688, co | 1);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DC3C), 0x306, STR(0x1003DC4C));
    type = load<u32>(a + 0x78C);
  }
  u8 weight = load<u8>(0x1003E054u + type * 0xB4u);
  gabi::call<void>(0x02515F14, gabi::at<void>(a + 0x620), weight, 255, actor);
  u32 flags = load<u32>(a + 0x3E0);
  type = load<u32>(a + 0x78C);
  store<u32>(a + 0x3E0, (flags & 0xFFFFFBF1u) | 0x2000);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DC3C), 0x306, STR(0x1003DC4C));
    type = load<u32>(a + 0x78C);
  }
  u32 first = 0x1003E044u + type * 0xB4u;
  u32 second = data(actor, 0x1003DC3C, 0x1003DC4C);
  f32 horizontal = load<f32>(a + 0x370), gravity = load<f32>(first);
  f32 horizontalSquare = horizontal * horizontal;
  f32 buoyancy = load<f32>(second + 0x44), vertical = load<f32>(a + 0x340);
  f32 acceleration = gravity + buoyancy;
  f32 energy = gabi::fmadds(vertical, vertical, horizontalSquare);
  store<f32>(a + 0x374, acceleration);
  f32 speed = gabi::call<f32>(0x028F4384, energy);
  u32 record = data(actor, 0x1003DC3C, 0x1003DC4C);
  f32 limit = load<f32>(record + 0x58);
  if (speed > limit) {
    record = data(actor, 0x1003DC3C, 0x1003DC4C);
    f32 ratio = load<f32>(record + 0x58) / speed;
    gabi::call<void>(0x028E8E64, gabi::at<void>(a + 0x33C),
                     gabi::at<void>(a + 0x33C), ratio);
    store<f32>(a + 0x370, load<f32>(a + 0x370) * ratio);
  }
  u32 attention = load<u32>(a + 0x39C);
  store<u32>(a + 0x790, 3);
  store<u32>(a + 0x39C, attention & ~0x10u);
}
VERIFY(0x0249C5F4, mode_sink_init);
void se_fall_lava(Act_c *actor) {
  WWHD_FUNC(0x0249C7C0, void, actor);
  s32 sound = 0x17;
  for (u32 offset : {0x5D0u, 0x4A0u}) {
    if (load<u16>(address(actor) + offset + 2) < 0x100) {
      u32 play = address(gabi::call<void *>(0x025200D4));
      sound = gabi::call<s32>(0x024EECAC, gabi::at<void>(play + 0x12A0),
                              gabi::at<void>(address(actor) + offset));
      break;
    }
  }
  u32 record = data(actor, 0x1003DC70, 0x1003DC80);
  s8 room = load<s8>(address(actor) + 0x326);
  u32 soundId = load<u32>(record + 0x98);
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call<void>(0x025E1A40, soundId, gabi::at<void>(address(actor) + 0x37C),
                   sound, reverb);
  u32 first = data(actor, 0x1003DC70, 0x1003DC80),
      second = data(actor, 0x1003DC70, 0x1003DC80);
  u8 secondSoundParameter = load<u8>(second + 0x6D),
     firstSoundParameter = load<u8>(first + 0x6C);
  set_sound_environment(actor, firstSoundParameter, secondSoundParameter);
}
VERIFY(0x0249C7C0, se_fall_lava);
s32 method_create(Act_c *actor) {
  WWHD_FUNC(0x0249DFE0, s32, actor);
  return gabi::call<s32>(0x0249B2C4, actor);
}
VERIFY(0x0249DFE0, method_create);
s32 method_delete(Act_c *actor) {
  WWHD_FUNC(0x0249DFE4, s32, actor);
  return delete_actor(actor);
}
VERIFY(0x0249DFE4, method_delete);
s32 method_execute(Act_c *actor) {
  WWHD_FUNC(0x0249DFE8, s32, actor);
  return gabi::call<s32>(0x0249D2BC, actor);
}
VERIFY(0x0249DFE8, method_execute);
s32 method_draw(Act_c *actor) {
  WWHD_FUNC(0x0249DFEC, s32, actor);
  return draw(actor);
}
VERIFY(0x0249DFEC, method_draw);

void bg_crr_water(Act_c *actor) {
  WWHD_FUNC(0x0249CB98, void, actor);
  u32 a = address(actor);
  f32 x = load<f32>(a + 0x314), z = load<f32>(a + 0x31C),
      water = load<f32>(a + 0x574);
  s32 seaHit = gabi::call<s32>(0x0246B6A4, x, z);
  z = load<f32>(a + 0x31C);
  x = load<f32>(a + 0x314);
  f32 sea = gabi::call<f32>(0x0246BA0C, x, z);
  u32 record = data(actor, 0x1003DD0C, 0x1003DD1C);
  u32 flags = load<u32>(a + 0x3E0);
  f32 y = load<f32>(a + 0x318), offset = load<f32>(record + 4);
  f32 bottom = y + offset;
  bool waterHit = (flags & 0x1000) && bottom < water;
  f32 previous = load<f32>(a + 0x614);
  bool seaInside = seaHit && bottom < sea;
  store<f32>(a + 0x618, previous);
  if (waterHit && (!seaInside || water > sea)) {
    store<f32>(a + 0x614, water);
    store<u8>(a + 0x61D, 0);
    store<u8>(a + 0x61C, 1);
  } else if (seaInside) {
    store<f32>(a + 0x614, sea);
    store<u8>(a + 0x61C, 1);
    store<u8>(a + 0x61D, 1);
  } else {
    store<u8>(a + 0x61C, 0);
    f32 invalid = load<f32>(0x1003D92C);
    store<u8>(a + 0x61D, 0);
    store<f32>(a + 0x614, invalid);
  }
}
VERIFY(0x0249CB98, bg_crr_water);
void se_fall_water(Act_c *actor) {
  WWHD_FUNC(0x0249C358, void, actor);
  u32 a = address(actor);
  u32 water = load<u8>(a + 0x61D) ? 0 : a + 0x52C;
  s32 sound = 0x13;
  for (u32 polygon : {water, a + 0x4A0}) {
    if (polygon && load<u16>(polygon + 2) < 0x100) {
      u32 play = address(gabi::call<void *>(0x025200D4));
      sound = gabi::call<s32>(0x024EECAC, gabi::at<void>(play + 0x12A0),
                              gabi::at<void>(polygon));
      break;
    }
  }
  u32 record = data(actor, 0x1003DBD4, 0x1003DBE4);
  s8 room = load<s8>(a + 0x326);
  u32 soundId = load<u32>(record + 0x94);
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call<void>(0x025E1A40, soundId, gabi::at<void>(a + 0x37C), sound,
                   reverb);
  u32 first = data(actor, 0x1003DBD4, 0x1003DBE4),
      second = data(actor, 0x1003DBD4, 0x1003DBE4);
  u8 secondSoundParameter = load<u8>(second + 0x6D),
     firstSoundParameter = load<u8>(first + 0x6C);
  set_sound_environment(actor, firstSoundParameter, secondSoundParameter);
}
VERIFY(0x0249C358, se_fall_water);

s32 damage_cc_proc(Act_c *actor) {
  WWHD_FUNC(0x0249C0E4, s32, actor);
  auto cylinder = gabi::at<void>(address(actor) + 0x65C);
  if (gabi::call<s32>(0x025160DC, cylinder)) {
    gabi::call<void>(0x02516094, cylinder);
    gabi::call<void>(0x0249BE90, actor, 3);
    return 1;
  }
  if (!gabi::call<s32>(0x025162A4, cylinder))
    return 0;
  u32 hit = address(gabi::call<void *>(0x02516300, cylinder));
  u32 type = load<u32>(address(actor) + 0x78C);
  if (hit) {
    u32 attack = load<u32>(hit + 0x10);
    if ((type != 3 && (attack & 0x20)) || (attack & 0x10000)) {
      gabi::call<void>(0x0249BE90, actor, 7);
      gabi::call<void>(0x0251621C, cylinder);
      return 1;
    }
  }
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DB68), 0x306, STR(0x1003DB78));
    type = load<u32>(address(actor) + 0x78C);
  }
  s8 room = load<s8>(address(actor) + 0x326);
  u32 sound = load<u32>(0x1003E0E0u + type * 0xB4u);
  gabi::call<void>(0x023129C4, gabi::at<void>(address(actor) + 0x37C), room,
                   cylinder, sound);
  u32 first = data(actor, 0x1003DB68, 0x1003DB78),
      second = data(actor, 0x1003DB68, 0x1003DB78);
  u8 secondSoundParameter = load<u8>(second + 0x6B),
     firstSoundParameter = load<u8>(first + 0x6A);
  set_sound_environment(actor, firstSoundParameter, secondSoundParameter);
  gabi::call<void>(0x02312E54, actor, cylinder);
  gabi::call<void>(0x0251621C, cylinder);
  return 0;
}
VERIFY(0x0249C0E4, damage_cc_proc);
s32 damage_bg_proc(Act_c *actor) {
  WWHD_FUNC(0x0249C994, s32, actor);
  bool grounded = (load<u32>(address(actor) + 0x3E0) & 0x20) != 0;
  u32 water = chk_sink_water(actor);
  s32 lava = chk_sink_lava(actor);
  s32 mode = load<s32>(address(actor) + 0x790);
  if (mode == 0 || mode == 2) {
    if (mode == 0 && grounded)
      return 0;
    if (water) {
      se_fall_water(actor);
      eff_hit_water_splash(actor);
      if (mode == 0)
        init_rot_clean(actor);
      mode_sink_init(actor);
    } else if (lava) {
      se_fall_lava(actor);
      eff_hit_lava_splash(actor);
      u32 record = data(actor, 0x1003DCD8, 0x1003DCE8);
      if (load<u32>(record + 0x70) & 1) {
        if (mode == 0)
          init_rot_clean(actor);
        mode_sink_init(actor);
      } else {
        gabi::call<void>(0x0249BE90, actor, 2);
        return 1;
      }
    }
  } else if (mode == 3 && (grounded || !(water | u32(lava)))) {
    mode_wait_init(actor);
  }
  return 0;
}
VERIFY(0x0249C994, damage_bg_proc);

s32 damage_bg_proc_directly(Act_c *actor) {
  WWHD_FUNC(0x0249CE6C, s32, actor);
  u32 a = address(actor), flags = load<u32>(a + 0x3E0);
  s32 mode = load<s32>(a + 0x790);
  bool grounded = flags & 0x20;
  s32 damaged = 0;
  if (mode == 0 && grounded) {
    f32 y;
    if (flags & 0x80) {
      u32 record = data(actor, 0x1003DDA8, 0x1003DDB8);
      f32 previous = load<f32>(a + 0x7A4);
      y = load<f32>(a + 0x318);
      f32 fall = previous - y, limit = load<f32>(record + 0x38);
      if (fall > limit) {
        gabi::call<void>(0x0249BE90, actor, 2);
        damaged = 1;
        y = load<f32>(a + 0x318);
      }
    } else
      y = load<f32>(a + 0x318);
    s8 delay = load<s8>(a + 0x795);
    store<f32>(a + 0x7A4, y);
    if (delay > 0) {
      store<u8>(a + 0x795, u8(delay - 1));
      return damaged;
    }
  } else if (mode == 2) {
    u32 water = chk_sink_water(actor);
    bool roof = flags & 0x200, wall = flags & 0x10;
    s32 lava = chk_sink_lava(actor);
    if (grounded) {
      gabi::call<void>(0x0249BE90, actor, 2);
      damaged = 1;
    } else if (wall || roof) {
      gabi::call<void>(0x0249BE90, actor, 3);
      damaged = 1;
    }
    if (grounded || wall || roof || (water | u32(lava)))
      cam_lockoff(actor);
  }
  s8 delay = load<s8>(a + 0x795);
  if (delay > 0)
    store<u8>(a + 0x795, u8(delay - 1));
  else if (grounded) {
    if (!load<u8>(a + 0x794) && load<s32>(a + 0x790) == 0) {
      if (!damaged) {
        u32 record = data(actor, 0x1003DDA8, 0x1003DDB8);
        u32 soundId = load<u32>(record + 0x90);
        u32 play = address(gabi::call<void *>(0x025200D4));
        s32 sound = gabi::call<s32>(0x024EECAC, gabi::at<void>(play + 0x12A0),
                                    gabi::at<void>(a + 0x4A0));
        s8 room = load<s8>(a + 0x326);
        s32 reverb = gabi::call<s32>(0x02520540, room);
        gabi::call<void>(0x025E1A40, soundId, gabi::at<void>(a + 0x37C), sound,
                         reverb);
        eff_land_smoke(actor);
      }
      store<u8>(a + 0x794, 1);
    }
  } else
    store<u8>(a + 0x794, 0);
  return damaged;
}
VERIFY(0x0249CE6C, damage_bg_proc_directly);
s32 mode_proc_call(Act_c *actor) {
  WWHD_FUNC(0x0249D078, s32, actor);
  u32 a = address(actor);
  s32 mode;
  if (load<u32>(a + 0x2E0) & 0x2000) {
    mode = load<s32>(a + 0x790);
    if (mode != 1) {
      init_rot_clean(actor);
      mode_carry_init(actor);
      mode = load<s32>(a + 0x790);
    }
  } else
    mode = load<s32>(a + 0x790);
  bool restorePosition = parameter(actor, 2, 6) == 1;
  u32 member = 0x1003DDDCu + u32(mode) * 8u;
  f32 x = load<f32>(a + 0x314);
  s16 adjustment = load<s16>(member), index = load<s16>(member + 2);
  u32 object = a + u32(s32(adjustment));
  f32 z = load<f32>(a + 0x31C), y = load<f32>(a + 0x318);
  u32 target;
  if (index < 0)
    target = load<u32>(member + 4);
  else {
    s16 vtableOffset = load<s16>(member + 6);
    u32 vtable = load<u32>(object + u32(s32(vtableOffset)));
    target = load<u32>(vtable + u32(s32(index)) * 8u + 4);
  }
  gabi::call<void>(target, gabi::at<void>(object));
  if (load<s32>(a + 0x790) == 1) {
    x = load<f32>(a + 0x314);
    y = load<f32>(a + 0x318);
    z = load<f32>(a + 0x31C);
    restorePosition = true;
  }
  u32 play = address(gabi::call<void *>(0x025200D4));
  gabi::call<void>(0x024F08A8, gabi::at<void>(a + 0x3B8),
                   gabi::at<void>(play + 0x12A0));
  bg_crr_water(actor);
  bg_crr_lava(actor);
  play = address(gabi::call<void *>(0x025200D4));
  if (gabi::call<s32>(0x024EEB2C, gabi::at<void>(play + 0x12A0),
                      gabi::at<void>(a + 0x4A0))) {
    play = address(gabi::call<void *>(0x025200D4));
    if (gabi::call<s32>(0x024EEABC, gabi::at<void>(play + 0x12A0),
                        gabi::at<void>(a + 0x4A0)))
      store<u8>(a + 0x796, 1);
    if (parameter(actor, 2, 6) == 1) {
      u32 parameters = load<u32>(a + 0xB0);
      y = load<f32>(a + 0x44C);
      store<u32>(a + 0xB0, parameters | 0xC0);
      store<f32>(a + 0x7A4, y);
    }
  }
  if (restorePosition) {
    store<f32>(a + 0x318, y);
    store<f32>(a + 0x314, x);
    store<f32>(a + 0x31C, z);
  }
  if (damage_bg_proc_directly(actor))
    return 0;
  if (load<s32>(a + 0x790) != 1) {
    store<u8>(a + 0x1C9, load<u8>(a + 0x326));
    play = address(gabi::call<void *>(0x025200D4));
    u8 room = gabi::call<u8>(0x024EEEB8, gabi::at<void>(play + 0x12A0),
                             gabi::at<void>(a + 0x4A0));
    store<u8>(a + 0x1CA, room);
  }
  return 1;
}
VERIFY(0x0249D078, mode_proc_call);

void eff_break(Act_c *actor) {
  WWHD_FUNC(0x0249BC7C, void, actor);
  u32 type = load<u32>(address(actor) + 0x78C);
  f32 x = load<f32>(address(actor) + 0x314);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DB00), 0x306, STR(0x1003DB10));
    type = load<u32>(address(actor) + 0x78C);
  }
  f32 y = load<f32>(address(actor) + 0x318),
      offset = load<f32>(0x1003E048u + type * 0xB4u);
  StoneLocal<cXyz> position;
  position->x = x;
  f32 z = load<f32>(address(actor) + 0x31C);
  position->z = z;
  position->y = y + offset;
  u32 play = address(gabi::call<void *>(0x025200D4));
  u32 particles = load<u32>(play + 0x5AB0);
  auto color = gabi::at<void>(address(actor) + 0x1A8);
  gabi::call<void *>(0x025A847C, gabi::at<void>(particles), 0, 0x3E3,
                     position.get(), nullptr, nullptr, 255, nullptr, -1, color,
                     color, nullptr);
  u32 resources = load<u32>(0x101F4F28);
  StoneLocal<SafeString> firstName;
  firstName->mStringTop = 0x1003DAF8;
  firstName->__vtbl = 0x1003D7B0;
  void *model = gabi::call<void *>(0x026066C4, gabi::at<void>(resources),
                                   firstName.get(), 0x30);
  resources = load<u32>(0x101F4F28);
  StoneLocal<SafeString> secondName;
  secondName->mStringTop = 0x1003DAF8;
  secondName->__vtbl = 0x1003D7B0;
  void *animation = gabi::call<void *>(0x026066C4, gabi::at<void>(resources),
                                       secondName.get(), 0x66);
  play = address(gabi::call<void *>(0x025200D4));
  particles = load<u32>(play + 0x5AB0);
  void *emitter = gabi::call<void *>(
      0x025A847C, gabi::at<void>(particles), 0, 0x3E2, position.get(), nullptr,
      nullptr, 255, nullptr, -1, nullptr, nullptr, nullptr);
  if (emitter) {
    u32 record = data(actor, 0x1003DB00, 0x1003DB10);
    u16 colorIndex = load<u16>(record + 0x88);
    void *instance = gabi::call<void *>(0x025A3BDC, nullptr, emitter, model, 1,
                                        gabi::at<void>(address(actor) + 0x110),
                                        animation, colorIndex, nullptr);
    if (instance) {
      play = address(gabi::call<void *>(0x025200D4));
      particles = load<u32>(play + 0x5AB0);
      u32 list = load<u32>(particles + 0x130);
      gabi::call<void>(0x0200FE78, gabi::at<void>(list), instance);
    }
  }
}
VERIFY(0x0249BC7C, eff_break);
void damaged(Act_c *actor, s32 kind) {
  WWHD_FUNC(0x0249BE90, void, actor, kind);
  u32 item = parameter(actor, 6, 0), save = parameter(actor, 7, 16);
  StoneLocal<csXyz> rotation;
  s16 yaw = load<s16>(address(actor) + 0x2FA);
  gabi::call<void>(0x0201A478, rotation.get(), 0, yaw, 0);
  s8 room = load<s8>(address(actor) + 0x2FE);
  gabi::call<void>(0x025D8120, gabi::at<void>(address(actor) + 0x314), item,
                   save, room, nullptr, rotation.get(), kind, nullptr);
  gabi::call<void>(0x025D9D24, actor);
  eff_break(actor);
  u32 record = data(actor, 0x1003DB34, 0x1003DB44);
  if (load<s16>(record + 0x40) > 0) {
    u32 play = address(gabi::call<void *>(0x025200D4));
    record = data(actor, 0x1003DB34, 0x1003DB44);
    f32 zero = load<f32>(0x1003D9A8), up = load<f32>(0x1003D7A8);
    StoneLocal<cXyz> direction;
    direction->z = zero;
    s16 power = load<s16>(record + 0x40);
    direction->x = zero;
    direction->y = up;
    gabi::call<void>(0x025CB374, gabi::at<void>(play + 0x599C), power, -33,
                     direction.get());
  }
  u32 first = data(actor, 0x1003DB34, 0x1003DB44),
      second = data(actor, 0x1003DB34, 0x1003DB44);
  u8 secondSoundParameter = load<u8>(second + 0x69),
     firstSoundParameter = load<u8>(first + 0x68);
  set_sound_environment(actor, firstSoundParameter, secondSoundParameter);
  record = data(actor, 0x1003DB34, 0x1003DB44);
  room = load<s8>(address(actor) + 0x326);
  u32 sound = load<u32>(record + 0x8C);
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call<void>(0x025E1A40, sound, gabi::at<void>(address(actor) + 0x37C), 0,
                   reverb);
  record = data(actor, 0x1003DB34, 0x1003DB44);
  if (load<u32>(record + 0x70) & 8) {
    u32 sw = parameter(actor, 8, 8);
    if (sw != 255) {
      u32 saveInfo = load<u32>(0x101F84DC);
      room = load<s8>(address(actor) + 0x2FE);
      gabi::call<void>(0x025B9E38, gabi::at<void>(saveInfo + 0x20), sw, room);
    }
  }
}
VERIFY(0x0249BE90, damaged);
s32 execute(Act_c *actor) {
  WWHD_FUNC(0x0249D2BC, s32, actor);
  u32 a = address(actor);
  cull_set_move(actor);
  bool skip = false;
  if (!load<u8>(a + 0x796) && load<s32>(a + 0x790) == 0) {
    u32 flags = load<u32>(a + 0x3E0);
    if ((flags & 0x20) && !(flags & 0x80) && parameter(actor, 3, 28) &&
        (load<u32>(a + 0x2E4) & 4) && gabi::call<s32>(0x025D6CE8, actor) &&
        parameter(actor, 2, 6) != 1)
      skip = true;
  }
  if (!skip) {
    store<u8>(a + 0x796, 0);
    bool remove = true;
    if (!damage_cc_proc(actor) && !damage_bg_proc(actor)) {
      u8 delay = load<u8>(a + 0x797);
      if (delay)
        store<u8>(a + 0x797, u8(delay - 1));
      if (mode_proc_call(actor)) {
        remove = false;
        set_mtx(actor);
        store<u8>(a + 0x642, load<u8>(a + 0x326));
        gabi::call<void>(0x025165A4, gabi::at<void>(a + 0x65C),
                         gabi::at<void>(a + 0x314));
        u32 play = address(gabi::call<void *>(0x025200D4));
        gabi::call<void>(0x0200E240, gabi::at<void>(play + 0x26A4),
                         gabi::at<void>(a + 0x65C));
        s32 mode = load<s32>(a + 0x790);
        if (mode == 2 || mode == 3 || load<u8>(a + 0x797)) {
          play = address(gabi::call<void *>(0x025200D4));
          gabi::call<void>(0x02516C14, gabi::at<void>(play + 0x4EF8),
                           gabi::at<void>(a + 0x65C), 3);
        }
        u32 type = load<u32>(a + 0x78C);
        store<f32>(a + 0x390, load<f32>(a + 0x314));
        if (type >= 5) {
          gabi::call<void>(0x0273AA24, STR(0x1003DDFC), 0x306, STR(0x1003DE0C));
          type = load<u32>(a + 0x78C);
        }
        f32 y = load<f32>(a + 0x318), x = load<f32>(a + 0x390);
        f32 height = load<f32>(0x1003E04Cu + type * 0xB4u),
            z = load<f32>(a + 0x31C);
        y += height;
        store<f32>(a + 0x37C, x);
        store<f32>(a + 0x398, z);
        store<f32>(a + 0x394, y);
        store<f32>(a + 0x384, z);
        store<f32>(a + 0x380, y);
      }
    }
    u32 model = 0;
    if (load<s32>(a + 0x790) == 1 && load<u8>(a + 0x7A9))
      model = load<u32>(a + 0x3B4);
    store<u32>(a + 0x368, model);
    if (remove)
      gabi::call<void>(0x025D57E0, actor);
  }
  cull_set_draw(actor);
  return 1;
}
VERIFY(0x0249D2BC, execute);

static void construct_actor(Act_c *actor) {
  u32 a = address(actor);
  gabi::call<void>(0x025D4ED0, actor);
  store<u32>(a + 0xB4, 0x1003D8D8);
  gabi::call<void>(0x024F0474, gabi::at<void>(a + 0x3B8));
  store<u8>(a + 0x3D0, 1);
  store<u32>(a + 0x3C8, 0x1003D8A8);
  store<u32>(a + 0x3D8, 0x1003D8B8);
  store<u32>(a + 0x3CC, 0x1003D8C8);
  gabi::call<void>(0x024EFE94, gabi::at<void>(a + 0x57C));
  gabi::call<void>(0x02008E0C, gabi::at<void>(a + 0x5BC));
  for (u32 i = 0x601; i <= 0x606; ++i)
    store<u8>(a + i, 0);
  store<u32>(a + 0x5BC, a + 0x5FC);
  store<u32>(a + 0x5C0, a + 0x608);
  store<u32>(a + 0x5CC, 0x1003D868);
  store<u32>(a + 0x5DC, 0x1003D878);
  store<u32>(a + 0x608, 0x1003D888);
  store<u32>(a + 0x5FC, 0x1003D898);
  store<u32>(a + 0x60C, 4);
  store<u8>(a + 0x600, 1);
  gabi::call<void>(0x0200BD2C, gabi::at<void>(a + 0x620));
  gabi::call<void>(0x02515DA0, gabi::at<void>(a + 0x63C));
  store<u32>(a + 0x638, 0x1004AE88);
  store<u32>(a + 0x63C, 0x1004AEC0);
  gabi::call<void>(0x02515FB8, gabi::at<void>(a + 0x65C));
  store<u32>(a + 0x770, 0x100015A8);
  store<u32>(a + 0x76C, 0x1003D7C8);
  gabi::call<void>(0x02018590, gabi::at<void>(a + 0x774));
  store<u32>(a + 0x698, 0x1004B108);
  store<u32>(a + 0x788, 0x1004B150);
  store<u32>(a + 0x770, 0x1004B160);
  for (u32 i : {0x798u, 0x79Au, 0x79Cu, 0x79Eu, 0x7A0u, 0x7A2u})
    gabi::call<void>(0x020065FC, gabi::at<void>(a + i));
}
static bool same_stage(u32 left, u32 right) {
  if (left == right)
    return true;
  for (u32 i = 0; i < 0x40001; ++i) {
    u8 a = load<u8>(left + i), b = load<u8>(right + i);
    if (a != b)
      return false;
    if (!a)
      return true;
  }
  return false;
}
s32 create(Act_c *actor) {
  WWHD_FUNC(0x0249B2C4, s32, actor);
  u32 a = address(actor), condition = load<u32>(a + 0x2E4);
  if (!(condition & 8)) {
    if (actor) {
      construct_actor(actor);
      condition = load<u32>(a + 0x2E4);
    }
    store<u32>(a + 0x2E4, condition | 8);
  }
  u32 type = parameter(actor, 3, 24);
  store<u32>(a + 0x78C, type);
  s32 appear = chk_appear(actor);
  store<u8>(a + 0x7A8, u8(appear));
  if (!appear)
    return 5;
  u32 record = data(actor, 0x1003DA5C, 0x1003DA6C);
  s32 phase = 4;
  if (load<u32>(record + 0x74) != 0x1003DFE8) {
    record = data(actor, 0x1003DA5C, 0x1003DA6C);
    u32 archive = load<u32>(record + 0x74);
    phase =
        gabi::call<s32>(0x02520460, gabi::at<void>(a + 0x3AC), STR(archive));
    if (phase != 4)
      return phase;
  }
  record = data(actor, 0x1003DA5C, 0x1003DA6C);
  u32 heapSize = load<u32>(record + 0x78);
  if (!gabi::call<s32>(0x025D63E8, actor, gabi::at<void>(0x0249ADFC), heapSize))
    return 5;
  if (!load<s16>(a + 0x2FA)) {
    record = data(actor, 0x1003DA5C, 0x1003DA6C);
    if (!(load<u32>(record + 0x70) & 0x10)) {
      f32 angle = gabi::call<f32>(0x02019918, load<f32>(0x1003DA4C));
      u16 yaw = u16(gabi::ftoi(angle));
      store<u16>(a + 0x2FA, yaw);
      store<u16>(a + 0x322, yaw);
      store<u16>(a + 0x32A, yaw);
    }
  }
  u32 model = load<u32>(a + 0x3B4);
  store<u32>(a + 0x348, model ? model + 0xC8 : 0);
  cull_set_draw(actor);
  StoneLocal<SafeString> stageName;
  stageName->mStringTop = 0x1003DA58;
  stageName->__vtbl = 0x1003D7B0;
  u32 play = address(gabi::call<void *>(0x025200D4));
  StoneLocal<SafeString> currentStage;
  currentStage->__vtbl = 0x1003D7B0;
  currentStage->mStringTop = play + 0x5134;
  u32 vtable = load<u32>(address(stageName.get()) + 4);
  gabi::call<void>(load<u32>(vtable + 0x14), stageName.get());
  vtable = load<u32>(address(stageName.get()) + 4);
  gabi::call<void>(load<u32>(vtable + 0x14), stageName.get());
  vtable = load<u32>(address(currentStage.get()) + 4);
  u32 left = load<u32>(address(stageName.get()));
  gabi::call<void>(load<u32>(vtable + 0x14), currentStage.get());
  u32 right = load<u32>(address(currentStage.get()));
  if (same_stage(left, right)) {
    play = address(gabi::call<void *>(0x025200D4));
    u32 stage = play + 0x5150, table = load<u32>(stage);
    void *environment =
        gabi::call<void *>(load<u32>(table + 0x15C), gabi::at<void>(stage));
    f32 distance = f32(load<u16>(address(environment) + 0x12));
    if (distance > load<f32>(0x1003D7A8)) {
      record = data(actor, 0x1003DA5C, 0x1003DA6C);
      store<f32>(a + 0x364, load<f32>(record + 0xB0) / distance);
    }
  }
  record = data(actor, 0x1003DA5C, 0x1003DA6C);
  u8 weight = load<u8>(record + 0x10);
  gabi::call<void>(0x02515F14, gabi::at<void>(a + 0x620), weight, 255, actor);
  gabi::call<void>(0x02516518, gabi::at<void>(a + 0x65C),
                   gabi::at<void>(0x1003E000));
  type = load<u32>(a + 0x78C);
  store<u32>(a + 0x6A0, a + 0x620);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DA5C), 0x306, STR(0x1003DA6C));
    type = load<u32>(a + 0x78C);
  }
  f32 radius = f32(load<s16>(0x1003E0C6u + type * 0xB4u));
  gabi::call<void>(0x020184DC, gabi::at<void>(a + 0x774), radius);
  record = data(actor, 0x1003DA5C, 0x1003DA6C);
  f32 height = f32(load<s16>(record + 0x84));
  gabi::call<void>(0x02018428, gabi::at<void>(a + 0x774), height);
  for (u32 i = 0; i != 3; ++i)
    store<u32>(a + 0x6D8 + i * 4, load<u32>(0x101FFBA8 + i * 4));
  store<u32>(a + 0x710, load<u32>(0x101FFBA8));
  store<u32>(a + 0x714, load<u32>(0x101FFBAC));
  type = load<u32>(a + 0x78C);
  store<u32>(a + 0x718, load<u32>(0x101FFBB0));
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DA5C), 0x306, STR(0x1003DA6C));
    type = load<u32>(a + 0x78C);
  }
  store<f32>(a + 0x374, load<f32>(0x1003E044u + type * 0xB4u));
  gabi::call<void>(0x025D6870, actor, nullptr);
  play = address(gabi::call<void *>(0x025200D4));
  gabi::call<void>(0x024F08A8, gabi::at<void>(a + 0x3B8),
                   gabi::at<void>(play + 0x12A0));
  store<u32>(a + 0x3E0, load<u32>(a + 0x3E0) & ~0x80u);
  if (parameter(actor, 2, 6) == 1) {
    f32 x = load<f32>(a + 0x2EC), y = load<f32>(a + 0x2F0);
    store<f32>(a + 0x314, x);
    f32 z = load<f32>(a + 0x2F4);
    store<f32>(a + 0x318, y);
    store<f32>(a + 0x31C, z);
    play = address(gabi::call<void *>(0x025200D4));
    if (gabi::call<s32>(0x024EEB2C, gabi::at<void>(play + 0x12A0),
                        gabi::at<void>(a + 0x4A0))) {
      u32 parameters = load<u32>(a + 0xB0);
      f32 ground = load<f32>(a + 0x44C);
      store<u32>(a + 0xB0, parameters | 0xC0);
      store<f32>(a + 0x318, ground);
    }
  }
  f32 y = load<f32>(a + 0x318);
  store<u8>(a + 0x794, 1);
  store<u8>(a + 0x795, 20);
  store<f32>(a + 0x7A4, y);
  init_mtx(actor);
  mode_wait_init(actor);
  u32 attention = load<u32>(a + 0x39C);
  type = load<u32>(a + 0x78C);
  store<u32>(a + 0x39C, attention | 0x10);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DA5C), 0x306, STR(0x1003DA6C));
    type = load<u32>(a + 0x78C);
  }
  record = 0x1003E044u + type * 0xB4u;
  type = load<u32>(a + 0x78C);
  store<u8>(a + 0x38C, load<u8>(record + 0x7E));
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DA5C), 0x306, STR(0x1003DA6C));
    type = load<u32>(a + 0x78C);
  }
  u32 dataFlags = load<u32>(0x1003E0B4u + type * 0xB4u);
  type = load<u32>(a + 0x78C);
  f32 x = load<f32>(a + 0x314);
  if (dataFlags & 2)
    store<u32>(a + 0x2E0, load<u32>(a + 0x2E0) | 0x10000);
  store<f32>(a + 0x390, x);
  if (type >= 5) {
    gabi::call<void>(0x0273AA24, STR(0x1003DA5C), 0x306, STR(0x1003DA6C));
    type = load<u32>(a + 0x78C);
  }
  y = load<f32>(a + 0x318);
  f32 z = load<f32>(a + 0x31C), offset = load<f32>(0x1003E04Cu + type * 0xB4u);
  store<u8>(a + 0x796, 1);
  y += offset;
  store<u8>(a + 0x797, 0);
  store<u8>(a + 0x7A9, 0);
  store<f32>(a + 0x394, y);
  store<f32>(a + 0x398, z);
  return phase;
}
VERIFY(0x0249B2C4, create);

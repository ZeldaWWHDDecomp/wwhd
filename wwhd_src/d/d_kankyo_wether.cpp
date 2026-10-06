// HD weather controls and packets. Inventory remains open until all weather functions are ported.
#include "bindings.h"
namespace d_kankyo_wether_cpp {
static u32 environment() { return gabi::call<u32>(0x02555D0C); }
static f32 dKyw_get_wind_pow() {
 WWHD_FUNC(0x02578348, f32, (u32)0);
 return gabi::load<f32>(environment() + 0xA18);
}
VERIFY(0x02578348, dKyw_get_wind_pow);
static void dKyw_wether_draw2() {
 WWHD_FUNC(0x0257D174, void, (u32)0);
 if (gabi::load<u8>(environment() + 0xA88) != 0)
  if (gabi::load<u8>(environment() + 0xA88) < 99)
   gabi::call<void>(0x025779E4, 1u);
}
VERIFY(0x0257D174, dKyw_wether_draw2);
static void dKyw_pntwind_cut(u32 influence) {
 WWHD_FUNC(0x0257D1B8, void, influence);
 if (influence && gabi::load<u32>(influence + 0x24) < 30) {
  u32 env = environment();
  u32 index = gabi::load<u32>(influence + 0x24);
  gabi::store<u32>(env + 0x810 + index * 4, 0);
 }
}
VERIFY(0x0257D1B8, dKyw_pntwind_cut);
static u32 dKyw_get_wind_vec() {
 WWHD_FUNC(0x0257DAA8, u32, (u32)0);
 return environment() + 0x9FC;
}
VERIFY(0x0257DAA8, dKyw_get_wind_vec);
static u32 wind_angle() {
 WWHD_FUNC(0x0257DACC, u32, (u32)0);
 u32 vec = gabi::call<u32>(0x0257DAA8);
 s32 angle = gabi::call<s32>(0x020195B0, gabi::load<f32>(vec), gabi::load<f32>(vec + 8));
 return (u16)((u32)angle + 0x8000u);
}
VERIFY(0x0257DACC, wind_angle);
static u32 dKyw_get_wind_power() {
 WWHD_FUNC(0x0257DB04, u32, (u32)0);
 return environment() + 0xA18;
}
VERIFY(0x0257DB04, dKyw_get_wind_power);
static void pntwind_set(u32 influence) {
 WWHD_FUNC(0x0257DBDC, void, influence);
 for (u32 index = 0; index < 30; ++index) {
  if (gabi::load<u32>(environment() + 0x810 + 4 * index) == 0) {
   gabi::store<u32>(environment() + 0x810 + 4 * index, influence);
   u32 stored = gabi::load<u32>(environment() + 0x810 + 4 * index);
   gabi::store<u32>(stored + 0x24, index);
   return;
  }
 }
 gabi::store<u32>(influence + 0x24, 9999);
}
VERIFY(0x0257DBDC, pntwind_set);
static void dKyw_pntwind_set(u32 influence) {
 WWHD_FUNC(0x0257DC90, void, influence);
 gabi::store<u8>(influence + 0x28, 0);
 gabi::call<void>(0x0257DBDC, influence);
}
VERIFY(0x0257DC90, dKyw_pntwind_set);
static void alternate_pntwind_set(u32 influence) {
 WWHD_FUNC(0x0257DE5C, void, influence);
 // HD wrapper clears the type byte, unlike the GC cylinder setter.
 gabi::store<u8>(influence + 0x28, 0);
 gabi::call<void>(0x0257DBDC, influence);
}
VERIFY(0x0257DE5C, alternate_pntwind_set);
static void dKyw_tact_wind_set(u32 windX, u32 windY) {
 WWHD_FUNC(0x0257E490, void, windX, windY);
 u32 env = environment();
 u32 vec = gabi::call<u32>(0x0257DAA8);
 gabi::store<u16>(env + 0xA24, windX);
 gabi::store<u16>(env + 0xA26, windY);
 gabi::store<u16>(gabi::load<u32>(0x101F84DC) + 0x4A, windX);
 gabi::store<u16>(gabi::load<u32>(0x101F84DC) + 0x4C, gabi::load<u16>(env + 0xA26));
 u32 angle = gabi::call<u32>(0x020195B0, gabi::load<f32>(vec), gabi::load<f32>(vec + 8));
 s16 delta = (s16)(0u - (angle + 0x4000u) - (s32)gabi::load<s16>(env + 0xA26));
 gabi::store<u8>(env + 0xA2C, 1);
 if (delta < 0) gabi::store<u8>(env + 0xA2C, 0x81);
}
VERIFY(0x0257E490, dKyw_tact_wind_set);
static void dKyw_tact_wind_set_go() {
 WWHD_FUNC(0x0257E52C, void, (u32)0);
 u32 env = environment();
 u8 flags = gabi::load<u8>(env + 0xA2C);
 gabi::store<u8>(env + 0xA2D, 0xFF);
 gabi::store<u8>(env + 0xA2C, flags | 1);
}
VERIFY(0x0257E52C, dKyw_tact_wind_set_go);
static s32 dKyw_get_tactwind_dir() {
 WWHD_FUNC(0x0257E560, s32, (u32)0);
 return (gabi::load<u8>(environment() + 0xA2C) & 0x80) ? 1 : 0;
}
VERIFY(0x0257E560, dKyw_get_tactwind_dir);
static void dKyw_custom_windpower(f32 power) {
 WWHD_FUNC(0x0257E594, void, power);
 gabi::store<f32>(environment() + 0xA20, power);
}
VERIFY(0x0257E594, dKyw_custom_windpower);
static s32 dKyw_get_windsdir() {
 WWHD_FUNC(0x0257E5D4, s32, (u32)0);
 u32 vec = gabi::call<u32>(0x0257DAA8);
 u32 angle = gabi::call<u32>(0x020195B0, gabi::load<f32>(vec), gabi::load<f32>(vec + 8));
 u16 heading = 0u - (angle + 0x4000u);
 s32 direction = 0;
 for (u32 index = 0; index < 9; ++index) {
  u32 row = 0x10050174 + 8 * index;
  if (heading < gabi::load<u16>(row + 2) && heading >= gabi::load<u16>(row))
   direction = gabi::load<s32>(row + 4);
 }
 return direction;
}
VERIFY(0x0257E5D4, dKyw_get_windsdir);
static void dKyw_evt_wind_set(u32 windX, u32 windY) {
 WWHD_FUNC(0x0257E684, void, windX, windY);
 u32 env = environment();
 gabi::store<u16>(env + 0xA28, windX);
 gabi::store<u16>(env + 0xA2A, windY);
}
VERIFY(0x0257E684, dKyw_evt_wind_set);
static void dKyw_evt_wind_set_go() {
 WWHD_FUNC(0x0257E6C4, void, (u32)0);
 gabi::store<u8>(environment() + 0xA2D, 1);
}
VERIFY(0x0257E6C4, dKyw_evt_wind_set_go);
static void dKyw_tornado_Notice(u32 position) {
 WWHD_FUNC(0x0257E77C, void, position);
 u32 env = environment();
 for (u32 i = 0; i < 3; ++i)
  gabi::store<u32>(env + 0xA0C + 4 * i, gabi::load<u32>(position + 4 * i));
}
VERIFY(0x0257E77C, dKyw_tornado_Notice);
static void dKyw_rain_set(u32 count) {
 WWHD_FUNC(0x0257E7C0, void, count);
 u32 env = environment();
 gabi::store<u32>(env + 0xA40, count);
 gabi::store<u32>(env + 0x106C, count);
}
VERIFY(0x0257E7C0, dKyw_rain_set);
static void sun_packet_draw(u32 packet) {
 WWHD_FUNC(0x0257E7F4, void, packet);
 gabi::call<void>(0x0256F1C4, 0x104B45F8u, packet + 0x98, packet + 0xD4);
}
VERIFY(0x0257E7F4, sun_packet_draw);
static void sunlenz_packet_draw(u32 packet) {
 WWHD_FUNC(0x0257ED2C, void, packet);
 gabi::call<void>(0x02570210, 0x104B45F8u, packet + 0xA4, packet + 0x98);
}
VERIFY(0x0257ED2C, sunlenz_packet_draw);
static void wave_packet_draw(u32 packet, u32 context) {
 WWHD_FUNC(0x02582500, void, packet, context);
 gabi::call<void>(0x02574D38, 0x104B45F8u, packet + 0x98, context);
}
VERIFY(0x02582500, wave_packet_draw);
struct Vector { gabi::be<f32> x, y, z; };
static void copyVectorBits(u32 destination, u32 source) {
 for (u32 i = 0; i < 3; ++i) gabi::store<u32>(destination + 4 * i, gabi::load<u32>(source + 4 * i));
}
static void get_wind_vecpow(u32 destination) {
 WWHD_FUNC(0x0257DB28, void, destination);
 f32 power = gabi::load<f32>(environment() + 0xA18);
 u32 vec = environment() + 0x9FC;
 gabi::Local<Vector> result;
 gabi::call<void>(0x0201AE48, vec, result.a, power);
 f32 y = gabi::load<f32>(result.a + 4);
 f32 z = gabi::load<f32>(result.a + 8);
 f32 x = gabi::load<f32>(result.a);
 if (!destination) destination = gabi::call<u32>(0x0273AD10, 12u);
 if (destination) {
  gabi::store<f32>(destination + 4, y);
  gabi::store<f32>(destination, x);
  gabi::store<f32>(destination + 8, z);
 }
}
VERIFY(0x0257DB28, get_wind_vecpow);
static void get_pntwind_vecpow(u32 destination, u32 position) {
 WWHD_FUNC(0x0257E128, void, destination, position);
 gabi::Local<Vector> direction, result;
 gabi::Local<gabi::be<f32>> power;
 gabi::call<void>(0x0257DE68, position, direction.a, power.a);
 gabi::call<void>(0x0201AE48, direction.a, result.a, gabi::load<f32>(power.a));
 f32 x = gabi::load<f32>(result.a), y = gabi::load<f32>(result.a + 4), z = gabi::load<f32>(result.a + 8);
 gabi::store<f32>(direction.a + 4, y);
 gabi::store<f32>(direction.a + 8, z);
 gabi::store<f32>(direction.a, x);
 if (!destination) {
  destination = gabi::call<u32>(0x0273AD10, 12u);
  if (!destination) return;
  x = gabi::load<f32>(direction.a);
 }
 gabi::store<f32>(destination, x);
 gabi::store<f32>(destination + 4, gabi::load<f32>(direction.a + 4));
 gabi::store<f32>(destination + 8, gabi::load<f32>(direction.a + 8));
}
VERIFY(0x0257E128, get_pntwind_vecpow);
static void get_AllWind_vecpow(u32 destination, u32 position) {
 WWHD_FUNC(0x0257E34C, void, destination, position);
 u32 env = environment();
 gabi::Local<gabi::be<f32>> power;
 gabi::Local<Vector> direction, temporary, globalPart, pointPart;
 gabi::call<void>(0x0257DE68, position, direction.a, power.a);
 f32 complement = gabi::fsubs_ppc(gabi::load<f32>(0x100500C4), gabi::load<f32>(power.a));
 f32 strength = gabi::fmuls_ppc(gabi::load<f32>(env + 0xA18), complement);
 gabi::call<void>(0x0201AE48, env + 0x9FC, temporary.a, strength);
 copyVectorBits(globalPart.a, temporary.a);
 strength = gabi::fmuls_ppc(gabi::load<f32>(power.a), gabi::load<f32>(0x10050170));
 gabi::call<void>(0x0201AE48, direction.a, temporary.a, strength);
 copyVectorBits(pointPart.a, temporary.a);
 gabi::call<void>(0x0201AD78, globalPart.a, temporary.a, pointPart.a);
 f32 x = gabi::load<f32>(temporary.a), y = gabi::load<f32>(temporary.a + 4), z = gabi::load<f32>(temporary.a + 8);
 if (!destination) destination = gabi::call<u32>(0x0273AD10, 12u);
 if (destination) {
  gabi::store<f32>(destination + 4, y);
  gabi::store<f32>(destination, x);
  gabi::store<f32>(destination + 8, z);
 }
}
VERIFY(0x0257E34C, get_AllWind_vecpow);
static s32 gbwind_use_check() {
 WWHD_FUNC(0x0257E6EC, s32, (u32)0);
 if (gabi::load<u32>(environment() + 0xA08) != 0) return 0;
 s32 room = gabi::load<s8>(0x1047E6C8);
 if (room < 0) return 0;
 u32 game = gabi::call<u32>(0x025200D4);
 u32 status = gabi::call<u32>(0x025C11DC, game + 0x51CC, room);
 u32 target = gabi::load<u32>(gabi::load<u32>(status) + 0x1DC);
 u32 information = gabi::call_ptr<u32>(target, status);
 if (!information) return 0;
 return ((gabi::load<u32>(information) >> 18) & 3) <= 2;
}
VERIFY(0x0257E6EC, gbwind_use_check);
static void rain_packet_draw(u32 packet) {
 WWHD_FUNC(0x0257F32C, void, packet);
 gabi::call<void>(0x02571B1C, 0x104B45F8u, packet + 0x98);
 gabi::call<void>(0x025712A8, 0x104B45F8u, packet + 0x98);
}
VERIFY(0x0257F32C, rain_packet_draw);
// The HD renderer stores texture data and vertex state in separate subobjects.
static void release_texture_geometry(u32 object, u32 flags) {
 if (object) {
  gabi::call<void>(0x027BF880, object + 0x158, 2u);
  gabi::call<void>(0x027B5CBC, object + 4, 2u);
  if (flags & 1) gabi::call<void>(0x0273AF40, object);
 }
}
static u32 construct_texture_geometry(u32 object) {
 if (!object) {
  object = gabi::call<u32>(0x0273AD10, 0x254u);
  if (!object) return 0;
 }
 gabi::call<void>(0x027B5BD8, object + 4);
 gabi::call<void>(0x027BF734, object + 0x158);
 gabi::store<u32>(object + 0x250, 0);
 gabi::store<u32>(object + 0x24C, 0);
 return object;
}
static void release_geometry_element(u32 object, u32 base) {
 gabi::call<void>(0x027BF7E8, object + base + 0x158);
 u32 pointer = gabi::load<u32>(object + base + 0x250);
 gabi::store<u32>(object + base, 0);
 if (pointer) {
  (void)gabi::load<u32>(object + base + 0x24C);
  u32 allocator = gabi::call<u32>(0x02755FEC, gabi::load<u32>(0x101F8B4C), pointer);
  u32 target = gabi::load<u32>(gabi::load<u32>(allocator + 0xC) + 0x3C);
  gabi::call_ptr<void>(target, allocator, gabi::load<u32>(object + base + 0x250));
  gabi::store<u32>(object + base + 0x24C, 0);
  gabi::store<u32>(object + base + 0x250, 0);
 }
}
static void release_geometry_pair(u32 object, u32 flags, u32 destructor) {
 if (object) {
  release_geometry_element(object, 0);
  release_geometry_element(object, 0x254);
  gabi::store<u32>(object + 0x4B8, 0);
  gabi::call<void>(0x028F0164, object, 2u, 0x254u, destructor, 0u, 0u);
  if (flags & 1) gabi::call<void>(0x0273AF40, object);
 }
}
static u32 construct_geometry_pair(u32 object, u32 constructor, u32 count = 20) {
 if (!object) {
  object = gabi::call<u32>(0x0273AD10, 0x4C0u);
  if (!object) return 0;
 }
 gabi::call<void>(0x028EFFD0, object, 2u, 0x254u, constructor);
 gabi::store<u32>(object + 0x4B0, count);
 gabi::store<u8>(object + 0x4BC, 0);
 gabi::store<u32>(object + 0x4AC, 0);
 gabi::store<u32>(object + 0x4B8, 0);
 gabi::store<u32>(object + 0x4A8, 0);
 gabi::store<u32>(object, 0);
 gabi::store<u32>(object + 0x254, 0);
 return object;
}
static void geometry_destructor_02583FBC(u32 object, u32 flags) {
 WWHD_FUNC(0x02583FBC, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02583FBC, geometry_destructor_02583FBC);
static void geometry_destructor_02584194(u32 object, u32 flags) {
 WWHD_FUNC(0x02584194, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02584194, geometry_destructor_02584194);
static void geometry_destructor_02584248(u32 object, u32 flags) {
 WWHD_FUNC(0x02584248, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02584248, geometry_destructor_02584248);
static void geometry_destructor_02584498(u32 object, u32 flags) {
 WWHD_FUNC(0x02584498, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02584498, geometry_destructor_02584498);
static void geometry_destructor_02584700(u32 object, u32 flags) {
 WWHD_FUNC(0x02584700, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02584700, geometry_destructor_02584700);
static void geometry_destructor_02584960(u32 object, u32 flags) {
 WWHD_FUNC(0x02584960, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02584960, geometry_destructor_02584960);
static void geometry_destructor_02584ACC(u32 object, u32 flags) {
 WWHD_FUNC(0x02584ACC, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02584ACC, geometry_destructor_02584ACC);
static void geometry_destructor_02584C38(u32 object, u32 flags) {
 WWHD_FUNC(0x02584C38, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02584C38, geometry_destructor_02584C38);
static void geometry_destructor_02584DA4(u32 object, u32 flags) {
 WWHD_FUNC(0x02584DA4, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02584DA4, geometry_destructor_02584DA4);
static void geometry_destructor_02584F28(u32 object, u32 flags) {
 WWHD_FUNC(0x02584F28, void, object, flags);
 release_texture_geometry(object, flags);
}
VERIFY(0x02584F28, geometry_destructor_02584F28);
static u32 geometry_constructor_025843B4(u32 object) {
 WWHD_FUNC(0x025843B4, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x025843B4, geometry_constructor_025843B4);
static u32 geometry_constructor_02584604(u32 object) {
 WWHD_FUNC(0x02584604, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x02584604, geometry_constructor_02584604);
static u32 geometry_constructor_0258487C(u32 object) {
 WWHD_FUNC(0x0258487C, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x0258487C, geometry_constructor_0258487C);
static void geometry_pair_destructor_0258401C(u32 object, u32 flags) {
 WWHD_FUNC(0x0258401C, void, object, flags);
 release_geometry_pair(object, flags, 0x02583FBC);
}
VERIFY(0x0258401C, geometry_pair_destructor_0258401C);
static void geometry_pair_destructor_025842A8(u32 object, u32 flags) {
 WWHD_FUNC(0x025842A8, void, object, flags);
 release_geometry_pair(object, flags, 0x02584248);
}
VERIFY(0x025842A8, geometry_pair_destructor_025842A8);
static void geometry_pair_destructor_025844F8(u32 object, u32 flags) {
 WWHD_FUNC(0x025844F8, void, object, flags);
 release_geometry_pair(object, flags, 0x02584498);
}
VERIFY(0x025844F8, geometry_pair_destructor_025844F8);
static void geometry_pair_destructor_025849C0(u32 object, u32 flags) {
 WWHD_FUNC(0x025849C0, void, object, flags);
 release_geometry_pair(object, flags, 0x02584960);
}
VERIFY(0x025849C0, geometry_pair_destructor_025849C0);
static void geometry_pair_destructor_02584B2C(u32 object, u32 flags) {
 WWHD_FUNC(0x02584B2C, void, object, flags);
 release_geometry_pair(object, flags, 0x02584ACC);
}
VERIFY(0x02584B2C, geometry_pair_destructor_02584B2C);
static void geometry_pair_destructor_02584C98(u32 object, u32 flags) {
 WWHD_FUNC(0x02584C98, void, object, flags);
 release_geometry_pair(object, flags, 0x02584C38);
}
VERIFY(0x02584C98, geometry_pair_destructor_02584C98);
static void geometry_pair_destructor_02584F88(u32 object, u32 flags) {
 WWHD_FUNC(0x02584F88, void, object, flags);
 release_geometry_pair(object, flags, 0x02584F28);
}
VERIFY(0x02584F88, geometry_pair_destructor_02584F88);
static u32 geometry_pair_constructor_02584410(u32 object) {
 WWHD_FUNC(0x02584410, u32, object);
 return construct_geometry_pair(object, 0x025843B4);
}
VERIFY(0x02584410, geometry_pair_constructor_02584410);
static u32 geometry_pair_constructor_025848D8(u32 object) {
 WWHD_FUNC(0x025848D8, u32, object);
 return construct_geometry_pair(object, 0x0258487C);
}
VERIFY(0x025848D8, geometry_pair_constructor_025848D8);
static void empty_02584F24() { WWHD_FUNC(0x02584F24, void, (u32)0); }
VERIFY(0x02584F24, empty_02584F24);
static void empty_02585094() { WWHD_FUNC(0x02585094, void, (u32)0); }
VERIFY(0x02585094, empty_02585094);
static void texture_pair_destructor(u32 object, u32 flags) {
 WWHD_FUNC(0x02584128, void, object, flags);
 if (object) {
  gabi::call<void>(0x027FB528, object + 0xB4, 0u);
  gabi::call<void>(0x027FB528, object + 0xC, 0u);
  gabi::call<void>(0x027FD764, object, 2u);
  if (flags & 1) gabi::call<void>(0x0273AF40, object);
 }
}
VERIFY(0x02584128, texture_pair_destructor);
static void texture_destructor(u32 object, u32 flags) {
 WWHD_FUNC(0x025841F4, void, object, flags);
 if (object) {
  gabi::call<void>(0x027FB528, object, 0u);
  if (flags & 1) gabi::call<void>(0x0273AF40, object);
 }
}
VERIFY(0x025841F4, texture_destructor);
static u32 four_geometry_constructor(u32 object) {
 WWHD_FUNC(0x02584660, u32, object);
 if (!object) {
  object = gabi::call<u32>(0x0273AD10, 0x968u);
  if (!object) return 0;
 }
 gabi::call<void>(0x028EFFD0, object, 4u, 0x254u, 0x02584604u);
 gabi::store<u32>(object + 0x950, 0);
 gabi::store<u32>(object + 0x958, 12);
 gabi::store<u8>(object + 0x964, 0);
 gabi::store<u32>(object + 0x954, 0);
 gabi::store<u32>(object + 0x960, 0);
 for (u32 pair = 0; pair < 2; ++pair)
  for (u32 side = 0; side < 2; ++side)
   gabi::store<u32>(object + pair * 0x254 + side * 0x4A8, 0);
 return object;
}
VERIFY(0x02584660, four_geometry_constructor);
static void release_geometry_array(u32 object, u32 flags, u32 pairs, u32 arrayCount, u32 countOffset, u32 destructor) {
 if (!object) return;
 for (u32 pair = 0; pair < pairs; ++pair) {
  release_geometry_element(object, pair * 0x4A8);
  release_geometry_element(object, pair * 0x4A8 + 0x254);
 }
 gabi::store<u32>(object + countOffset, 0);
 gabi::call<void>(0x028F0164, object, arrayCount, 0x254u, destructor, 0u, 0u);
 if (flags & 1) gabi::call<void>(0x0273AF40, object);
}
static void four_geometry_destructor(u32 object, u32 flags) {
 WWHD_FUNC(0x02584760, void, object, flags);
 release_geometry_array(object, flags, 2, 4, 0x960, 0x02584700);
}
VERIFY(0x02584760, four_geometry_destructor);
static void precipitation_geometry_destructor(u32 object, u32 flags) {
 WWHD_FUNC(0x02584E04, void, object, flags);
 release_geometry_array(object, flags, 300, 600, 0x574F0, 0x02584DA4);
}
VERIFY(0x02584E04, precipitation_geometry_destructor);static u32 geometry_constructor_02582CFC(u32 object) {
 WWHD_FUNC(0x02582CFC, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x02582CFC, geometry_constructor_02582CFC);
static u32 geometry_constructor_025830D0(u32 object) {
 WWHD_FUNC(0x025830D0, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x025830D0, geometry_constructor_025830D0);
static u32 geometry_constructor_0258312C(u32 object) {
 WWHD_FUNC(0x0258312C, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x0258312C, geometry_constructor_0258312C);
static u32 geometry_constructor_025834B0(u32 object) {
 WWHD_FUNC(0x025834B0, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x025834B0, geometry_constructor_025834B0);
static u32 geometry_constructor_02583594(u32 object) {
 WWHD_FUNC(0x02583594, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x02583594, geometry_constructor_02583594);
static u32 geometry_constructor_02583B18(u32 object) {
 WWHD_FUNC(0x02583B18, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x02583B18, geometry_constructor_02583B18);
static u32 geometry_constructor_02583C1C(u32 object) {
 WWHD_FUNC(0x02583C1C, u32, object);
 return construct_texture_geometry(object);
}
VERIFY(0x02583C1C, geometry_constructor_02583C1C);
static u32 geometry_pair_constructor_02582D58(u32 object) {
 WWHD_FUNC(0x02582D58, u32, object);
 return construct_geometry_pair(object, 0x02582CFC, 152);
}
VERIFY(0x02582D58, geometry_pair_constructor_02582D58);
static u32 geometry_pair_constructor_02583188(u32 object) {
 WWHD_FUNC(0x02583188, u32, object);
 return construct_geometry_pair(object, 0x0258312C, 20);
}
VERIFY(0x02583188, geometry_pair_constructor_02583188);
static u32 geometry_pair_constructor_0258350C(u32 object) {
 WWHD_FUNC(0x0258350C, u32, object);
 return construct_geometry_pair(object, 0x025834B0, 20);
}
VERIFY(0x0258350C, geometry_pair_constructor_0258350C);
static u32 geometry_pair_constructor_025835F0(u32 object) {
 WWHD_FUNC(0x025835F0, u32, object);
 return construct_geometry_pair(object, 0x02583594, 20);
}
VERIFY(0x025835F0, geometry_pair_constructor_025835F0);
static u32 geometry_pair_constructor_02583C78(u32 object) {
 WWHD_FUNC(0x02583C78, u32, object);
 return construct_geometry_pair(object, 0x02583C1C, 20);
}
VERIFY(0x02583C78, geometry_pair_constructor_02583C78);
static u32 precipitation_geometry_constructor(u32 object) {
 WWHD_FUNC(0x02583B74, u32, object);
 if (!object) {
  object = gabi::call<u32>(0x0273AD10, 0x574F8u);
  if (!object) return 0;
 }
 gabi::call<void>(0x028EFFD0, object, 600u, 0x254u, 0x02583B18u);
 gabi::store<u32>(object + 0x574E0, 0);
 gabi::store<u32>(object + 0x574F0, 0);
 gabi::store<u32>(object + 0x574E8, 20);
 gabi::store<u32>(object + 0x574E4, 0);
 gabi::store<u8>(object + 0x574F4, 0);
 for (u32 side = 0; side < 2; ++side)
  for (u32 index = 0; index < 300; ++index)
   gabi::store<u32>(object + side * 0x254 + index * 0x4A8, 0);
 return object;
}
VERIFY(0x02583B74, precipitation_geometry_constructor);
static u32 allocate_empty16(u32 object) {
 WWHD_FUNC(0x02582DE0, u32, object);
 return object ? object : gabi::call<u32>(0x0273AD10, 16u);
}
VERIFY(0x02582DE0, allocate_empty16);
static u32 safe_string_constructor(u32 object, u32 text) {
 WWHD_FUNC(0x02582BB8, u32, object, text);
 if (!object) {
  object = gabi::call<u32>(0x0273AD10, 8u);
  if (!object) return 0;
 }
 gabi::store<u32>(object, text);
 gabi::store<u32>(object + 4, 0x1004FDD0);
 return object;
}
VERIFY(0x02582BB8, safe_string_constructor);
static void safe_string_destructor(u32 object, u32 flags) {
 WWHD_FUNC(0x02582CE8, void, object, flags);
 if (object && (flags & 1)) gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x02582CE8, safe_string_destructor);
static void __sinit_d_kankyo_wether_cpp() {
 WWHD_FUNC(0x02582B24, void, (u32)0);
 gabi::store<u32>(0x104775A8, 0);
 gabi::store<u32>(0x104775A0, 0);
 gabi::store<u32>(0x104775AC, 0);
 gabi::store<u32>(0x104775A4, 0);
 gabi::call<void>(0x028F026C, 0x101E9A4Cu);
 f32 first = gabi::load<f32>(0x100501E0);
 f32 second = gabi::load<f32>(0x100501E4);
 gabi::store<f32>(0x10477590, first);
 gabi::store<f32>(0x10477594, second);
 gabi::call<void>(0x028ED6F8, 0x1047759Cu);
 gabi::call<void>(0x028F026C, 0x101E9A58u);
 gabi::call<void>(0x028EAB2C, 0x1047759Du);
 gabi::call<void>(0x028F026C, 0x101E9A64u);
}
VERIFY(0x02582B24, __sinit_d_kankyo_wether_cpp);
static void effect_destructor(u32 object, u32 flags) {
 WWHD_FUNC(0x025776F8, void, object, flags);
 if (object && (flags & 1)) gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x025776F8, effect_destructor);
static void empty_effect_constructor() { WWHD_FUNC(0x0257770C, void, (u32)0); }
VERIFY(0x0257770C, empty_effect_constructor);
static u32 setDrawPacketList(u32 packet, u32 type) {
 WWHD_FUNC(0x02577710, u32, packet, type);
 if (packet) {
  u32 game = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x027F0E04, gabi::load<u32>(game + 0x5D80), packet, 0u);
 }
 return packet;
}
VERIFY(0x02577710, setDrawPacketList);
static u32 setDrawPacketListSky(u32 packet, u32 type) {
 WWHD_FUNC(0x02577754, u32, packet, type);
 if (packet) {
  u32 first = gabi::load<u32>(gabi::call<u32>(0x025200D4) + 0x5D4C);
  gabi::store<u32>(0x104B4634, first);
  u32 second = gabi::load<u32>(gabi::call<u32>(0x025200D4) + 0x5D50);
  gabi::store<u32>(0x104B4638, second);
  u32 buffer = gabi::load<u32>(0x104B4634 + (type < 2 ? type * 4 : 0));
  gabi::call<void>(0x027F0E04, buffer, packet, 0u);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(gabi::call<u32>(0x025200D4) + 0x5D78));
  gabi::store<u32>(0x104B4638, gabi::load<u32>(gabi::call<u32>(0x025200D4) + 0x5D7C));
 }
 return packet;
}
VERIFY(0x02577754, setDrawPacketListSky);
static void draw_weather_025777F0(u32 type) {
 WWHD_FUNC(0x025777F0, void, type);
 gabi::call<void>(0x02577754, gabi::load<u32>(environment() + 0xA34), type);
}
VERIFY(0x025777F0, draw_weather_025777F0);
static void draw_weather_02577864(u32 type) {
 WWHD_FUNC(0x02577864, void, type);
 gabi::call<void>(0x02577710, gabi::load<u32>(environment() + 0xA44), type);
}
VERIFY(0x02577864, draw_weather_02577864);
static void draw_weather_0257789C(u32 type) {
 WWHD_FUNC(0x0257789C, void, type);
 gabi::call<void>(0x02577710, gabi::load<u32>(environment() + 0xA50), type);
}
VERIFY(0x0257789C, draw_weather_0257789C);
static void draw_weather_025778D4(u32 type) {
 WWHD_FUNC(0x025778D4, void, type);
 gabi::call<void>(0x02577754, gabi::load<u32>(environment() + 0xA60), type);
}
VERIFY(0x025778D4, draw_weather_025778D4);
static void draw_weather_02577974(u32 type) {
 WWHD_FUNC(0x02577974, void, type);
 gabi::call<void>(0x02577710, gabi::load<u32>(environment() + 0xA78), type);
}
VERIFY(0x02577974, draw_weather_02577974);
static void draw_weather_025779AC(u32 type) {
 WWHD_FUNC(0x025779AC, void, type);
 gabi::call<void>(0x02577710, gabi::load<u32>(environment() + 0xA84), type);
}
VERIFY(0x025779AC, draw_weather_025779AC);
static void draw_lenzflare(u32 type) {
 WWHD_FUNC(0x02577828, void, type);
 (void)environment();
 gabi::call<void>(0x02577710, gabi::load<u32>(environment() + 0xA38), type);
}
VERIFY(0x02577828, draw_lenzflare);
static u32 draw_cloud() {
 WWHD_FUNC(0x0257790C, u32, (u32)0);
 u32 packet = gabi::load<u32>(environment() + 0xA6C);
 u32 position = gabi::load<u32>(environment() + 0xA6C) + 0xBC24;
 if (packet) {
  u32 game = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x0252F3B0, game + 0x5D30, gabi::load<u32>(game + 0x5D7C), packet, position);
 }
 return packet;
}
VERIFY(0x0257790C, draw_cloud);
static void delete_weather_packet(u32 owner) {
 u32 packet = gabi::load<u32>(owner);
 if (packet) {
  u32 target = gabi::load<u32>(gabi::load<u32>(packet + 0xC) + 0xC);
  gabi::call_ptr<void>(target, packet, 2u);
  gabi::store<u32>(owner, 0);
  gabi::call<void>(0x025F0148, packet);
 }
}
static void delete_packet_02577E38(u32 owner) {
 WWHD_FUNC(0x02577E38, void, owner);
 delete_weather_packet(owner);
}
VERIFY(0x02577E38, delete_packet_02577E38);
static void delete_packet_02577E9C(u32 owner) {
 WWHD_FUNC(0x02577E9C, void, owner);
 delete_weather_packet(owner);
}
VERIFY(0x02577E9C, delete_packet_02577E9C);
static void delete_packet_02577F00(u32 owner) {
 WWHD_FUNC(0x02577F00, void, owner);
 delete_weather_packet(owner);
}
VERIFY(0x02577F00, delete_packet_02577F00);
static void delete_packet_02577F64(u32 owner) {
 WWHD_FUNC(0x02577F64, void, owner);
 delete_weather_packet(owner);
}
VERIFY(0x02577F64, delete_packet_02577F64);
static void delete_packet_02577FC8(u32 owner) {
 WWHD_FUNC(0x02577FC8, void, owner);
 delete_weather_packet(owner);
}
VERIFY(0x02577FC8, delete_packet_02577FC8);
static void wether_init2() {
 WWHD_FUNC(0x02577E00, void, (u32)0);
 gabi::store<u8>(environment() + 0xA88, 0);
 gabi::store<u32>(environment() + 0xA8C, 0);
}
VERIFY(0x02577E00, wether_init2);
static void wether_delete2() {
 WWHD_FUNC(0x025782D0, void, (u32)0);
 if (gabi::load<u8>(environment() + 0xA88)) {
  u32 env = environment();
  delete_weather_packet(env + 0xA94);
 }
}
VERIFY(0x025782D0, wether_delete2);

static u32 draw_sky_packet(u32 packet, u32 type, u32 gameOffset) {
 if (packet) {
  u32 game = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(game + gameOffset));
  u32 buffer = gabi::load<u32>(0x104B4634 + (type < 2 ? type * 4 : 0));
  gabi::call<void>(0x027F0E04, buffer, packet, (u32)0);
  game = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(game + 0x5D78));
  game = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(game + 0x5D7C));
 }
 return packet;
}
static u32 draw_vrkumo(u32 type) {
 WWHD_FUNC(0x025779E4, u32, type);
 return draw_sky_packet(gabi::load<u32>(environment() + 0xA94), type, 0x5DA4);
}
VERIFY(0x025779E4, draw_vrkumo);
static u32 draw_wave(u32 type) {
 WWHD_FUNC(0x02577A80, u32, type);
 if (gabi::load<s16>(environment() + 0x9F8) <= 0) return 0;
 if (!gabi::load<u32>(environment() + 0xAA0)) return 0;
 return draw_sky_packet(gabi::load<u32>(environment() + 0xAA0), type, 0x5D74);
}
VERIFY(0x02577A80, draw_wave);
static void wind_init() {
 WWHD_FUNC(0x02577B5C, void, (u32)0);
 u32 env = environment();
 float x = gabi::load<float>(0x1005001C);
 float zero = gabi::load<float>(0x10050020);
 gabi::store<float>(env + 0x9FC, x);
 gabi::store<float>(environment() + 0xA00, zero);
 gabi::store<float>(environment() + 0xA04, zero);
 gabi::store<float>(environment() + 0xA18, zero);
 u32 state = gabi::load<u32>(0x101F84DC);
 bool unset = gabi::load<s16>(state + 0x4A) == -1 && gabi::load<s16>(state + 0x4C) == -1;
 if (unset) {
  gabi::store<u8>(environment() + 0xA2C, 0);
  gabi::store<u16>(environment() + 0xA24, 0);
  gabi::store<u16>(environment() + 0xA26, 0);
 } else {
  gabi::store<u8>(environment() + 0xA2C, 1);
  env = environment();
  gabi::store<u16>(env + 0xA24, gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x4A));
  u16 angle = gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x4C);
  gabi::store<u16>(environment() + 0xA26, angle);
 }
 gabi::store<u32>(environment() + 0xA08, 0);
 gabi::store<float>(environment() + 0xA1C, zero);
 env = environment();
 float distant = gabi::load<float>(0x10050024);
 gabi::store<u8>(env + 0xA2D, 0);
 gabi::store<float>(environment() + 0xA0C, distant);
 gabi::store<float>(environment() + 0xA10, distant);
 gabi::store<float>(environment() + 0xA14, distant);
}
VERIFY(0x02577B5C, wind_init);
static void sun_packet_destructor(u32 object, u32 flags) {
 WWHD_FUNC(0x0257E80C, void, object, flags);
 if (!object) return;
 gabi::call<void>(0x028F0164, object+0x2460, (u32)6, (u32)0x4C, (u32)0x027FE4B0, (u32)0, (u32)0);
 gabi::call<void>(0x028F0164, object+0x13F0, (u32)4, (u32)0x41C, (u32)0x02584128, (u32)0, (u32)0);
 gabi::call<void>(0x028F0164, object+0xF0, (u32)4, (u32)0x4C0, (u32)0x0258401C, (u32)0, (u32)0);
 gabi::call<void>(0x027F13DC, object, (u32)0);
 if (flags & 1) gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x0257E80C, sun_packet_destructor);
static void wave_packet_destructor(u32 object, u32 flags) {
 WWHD_FUNC(0x02582A50, void, object, flags);
 if (!object) return;
 gabi::call<void>(0x027FE4B0, object+0x9DA90+0xB4, (u32)2);
 gabi::call<void>(0x027FB528, object+0x9DA90+0xC, (u32)0);
 gabi::call<void>(0x027FD764, object+0x9DA90, (u32)2);
 gabi::call<void>(0x028F0164, object+0x5D350, (u32)300, (u32)0x370, (u32)0x025841F4, (u32)0, (u32)0);
 gabi::call<void>(0x028F0164, object+0x424C, (u32)300, (u32)0x4C0, (u32)0x02584F88, (u32)0, (u32)0);
 gabi::call<void>(0x027F13DC, object, (u32)0);
 if (flags & 1) gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x02582A50, wave_packet_destructor);

static void wether_init() {
 WWHD_FUNC(0x02577CB0, void, (u32)0);
 gabi::store<u8>(environment()+0xA30,0);
 gabi::store<u8>(environment()+0xAB0,0);
 gabi::store<u32>(environment()+0xAB4,0);
 gabi::store<u8>(environment()+0xA3C,0);
 gabi::store<u32>(environment()+0xA40,0);
 gabi::store<u8>(environment()+0xA48,0);
 gabi::store<u32>(environment()+0xA4C,0);
 gabi::store<u8>(environment()+0xA54,0);
 u32 env=environment();
 float zero=gabi::load<float>(0x10050020);
 gabi::store<u32>(env+0xA58,0);
 gabi::store<float>(environment()+0xA5C,zero);
 gabi::store<u8>(environment()+0xA7C,0);
 gabi::store<u32>(environment()+0xA80,0);
 gabi::store<u8>(environment()+0xA7D,0);
 gabi::store<u8>(environment()+0xA70,0);
 gabi::store<u32>(environment()+0xA74,0);
 gabi::store<u8>(environment()+0xA64,0);
 gabi::store<u32>(environment()+0xA68,0);
 gabi::store<float>(environment()+0xA20,zero);
 gabi::store<u8>(environment()+0xA98,0);
 gabi::store<u32>(environment()+0xA9C,0);
 gabi::store<u8>(environment()+0xAA4,0);
 gabi::store<u32>(environment()+0xAA8,0);
 gabi::call<void>(0x02577B5C);
 for(u32 i=0;i<30;++i) gabi::store<u32>(environment()+0x810+4*i,0);
 for(u32 i=0;i<5;++i) gabi::store<u8>(environment()+0x888+0x3C*i,0);
}
VERIFY(0x02577CB0,wether_init);

static void retire_particle_slot(u32 slot) {
 u32 particle=gabi::load<u32>(slot);
 if (particle) {
  gabi::call<void>(0x0281DE68,particle,(u32)1);
  particle=gabi::load<u32>(slot);
  u32 flags=gabi::load<u32>(particle+0x254);
  gabi::store<u32>(particle+0x5C,0xFFFFFFFF);
  gabi::store<u32>(particle+0x254,flags|1);
  gabi::store<u32>(slot,0);
 }
}
static void wether_delete() {
 WWHD_FUNC(0x0257802C,void,(u32)0);
 if(gabi::load<u8>(environment()+0xA30)) {
  if(gabi::load<u32>(environment()+0xA38)) {
   u32 packet=gabi::load<u32>(environment()+0xA38);
   if(packet) {
    u32 target=gabi::load<u32>(gabi::load<u32>(packet+0xC)+0xC);
    gabi::call_ptr<void>(target,packet,(u32)3);
   }
   gabi::store<u32>(environment()+0xA38,0);
  }
  delete_weather_packet(environment()+0xA34);
 }
 if(gabi::load<u8>(environment()+0xA3C)) gabi::call<void>(0x02577E38,environment()+0xA44);
 if(gabi::load<u8>(environment()+0xA48)) delete_weather_packet(environment()+0xA50);
 if(gabi::load<u8>(environment()+0xA54)) gabi::call<void>(0x02577E9C,environment()+0xA60);
 if(gabi::load<u8>(environment()+0xA64)) delete_weather_packet(environment()+0xA6C);
 if(gabi::load<u8>(environment()+0xA70)) gabi::call<void>(0x02577F00,environment()+0xA78);
 if(gabi::load<u8>(environment()+0xA7C)) gabi::call<void>(0x02577F64,environment()+0xA84);
 if(gabi::load<u8>(environment()+0xA98)) gabi::call<void>(0x02577FC8,environment()+0xAA0);
 if(gabi::load<u8>(environment()+0xAA4)) {
  u32 packet=gabi::load<u32>(environment()+0xAAC);
  for(u32 i=0;i<30;++i) retire_particle_slot(packet+0x34*i);
  for(u32 i=0;i<2;++i) retire_particle_slot(packet+0x618+0x20*i);
  if(gabi::load<u32>(environment()+0xAAC)) {
   u32 env=environment();
   u32 saved=gabi::load<u32>(env+0xAAC);
   if(saved) {
    gabi::store<u32>(env+0xAAC,0);
    gabi::call<void>(0x025F0148,saved);
   }
  }
 }
}
VERIFY(0x0257802C,wether_delete);

static void packet_destructor_02580FC8(u32 object,u32 flags) {
 WWHD_FUNC(0x02580FC8,void,object,flags);
 if(!object)return;
 gabi::call<void>(0x028F0164,object+0x498528,(u32)8,(u32)0x41C,(u32)0x02584128,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0x8A8,(u32)2000,(u32)0x968,(u32)0x02584760,(u32)0,(u32)0);
 gabi::call<void>(0x027F13DC,object,(u32)0);
 if(flags&1)gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02580FC8,packet_destructor_02580FC8);

static void packet_destructor_02581440(u32 object,u32 flags) {
 WWHD_FUNC(0x02581440,void,object,flags);
 if(!object)return;
 gabi::call<void>(0x027FE4B0,object+0x20BC5C,(u32)2);
 gabi::call<void>(0x027FB528,object+0x20BBB4,(u32)0);
 gabi::call<void>(0x027FD764,object+0x20BBA8,(u32)2);
 gabi::call<void>(0x028F0164,object+0x134E28,(u32)1000,(u32)0x370,(u32)0x025841F4,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0xC028,(u32)1000,(u32)0x4C0,(u32)0x025849C0,(u32)0,(u32)0);
 gabi::call<void>(0x027F13DC,object,(u32)0);
 if(flags&1)gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02581440,packet_destructor_02581440);

static void packet_destructor_02581B60(u32 object,u32 flags) {
 WWHD_FUNC(0x02581B60,void,object,flags);
 if(!object)return;
 gabi::call<void>(0x027FE4B0,object+0x139204,(u32)2);
 gabi::call<void>(0x027FB528,object+0x13915C,(u32)0);
 gabi::call<void>(0x027FD764,object+0x139150,(u32)2);
 gabi::call<void>(0x028F0164,object+0xB82D0,(u32)600,(u32)0x370,(u32)0x025841F4,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0x60CC,(u32)600,(u32)0x4C0,(u32)0x02584B2C,(u32)0,(u32)0);
 gabi::call<void>(0x027F13DC,object,(u32)0);
 if(flags&1)gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02581B60,packet_destructor_02581B60);

static void packet_destructor_02582020(u32 object,u32 flags) {
 WWHD_FUNC(0x02582020,void,object,flags);
 if(!object)return;
 gabi::call<void>(0x027FE4B0,object+0xFFEC,(u32)2);
 gabi::call<void>(0x027FB528,object+0xFF44,(u32)0);
 gabi::call<void>(0x027FD764,object+0xFF38,(u32)2);
 gabi::call<void>(0x028F0164,object+0x9818,(u32)30,(u32)0x370,(u32)0x025841F4,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0x994,(u32)30,(u32)0x4C0,(u32)0x02584C98,(u32)0,(u32)0);
 gabi::call<void>(0x027F13DC,object,(u32)0);
 if(flags&1)gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02582020,packet_destructor_02582020);

static void get_AllWind_vec(u32 position,u32 direction,u32 power) {
 WWHD_FUNC(0x0257E1B8,void,position,direction,power);
 u32 env=environment();
 gabi::Local<Vector> temporary,globalPart,pointPart,normalized;
 gabi::call<void>(0x0257DE68,position,direction,power);
 float complement=gabi::fsubs_ppc(gabi::load<float>(0x100500C4),gabi::load<float>(power));
 float scale=gabi::fmuls_ppc(gabi::load<float>(env+0xA18),complement);
 gabi::call<void>(0x0201AE48,env+0x9FC,temporary.a,scale);
 copyVectorBits(globalPart.a,temporary.a);
 scale=gabi::fmuls_ppc(gabi::load<float>(power),gabi::load<float>(0x10050170));
 gabi::call<void>(0x0201AE48,direction,temporary.a,scale);
 copyVectorBits(pointPart.a,temporary.a);
 gabi::call<void>(0x0201AD78,globalPart.a,temporary.a,pointPart.a);
 copyVectorBits(normalized.a,temporary.a);
 float lengthSquared=gabi::call<float>(0x028E8DD0,normalized.a);
 float length=gabi::call<float>(0x028F4384,lengthSquared);
 gabi::store<float>(power,length);
 gabi::call<void>(0x0201B12C,normalized.a,temporary.a);
 copyVectorBits(normalized.a,temporary.a);
 u32 nonzero=gabi::call<u32>(0x0201AFD8,normalized.a,(u32)0x101FFBA8);
 if(nonzero)copyVectorBits(direction,normalized.a);
 else copyVectorBits(direction,env+0x9FC);
}
VERIFY(0x0257E1B8,get_AllWind_vec);
static void normalize_color(u32 output,u32 rgba) {
 WWHD_FUNC(0x0257963C,void,output,rgba);
 // HD returns a four-float color through the EABI result buffer.
 float red=(float)gabi::load<u8>(rgba);
 float green=(float)gabi::load<u8>(rgba+1);
 float denominator=gabi::load<float>(0x10050048);
 float blue=(float)gabi::load<u8>(rgba+2);
 float alpha=(float)gabi::load<u8>(rgba+3);
 float values[4]={(float)(red/denominator),(float)(green/denominator),(float)(blue/denominator),(float)(alpha/denominator)};
 for(u32 i=0;i<4;++i)gabi::store<float>(output+4*i,values[i]);
}
VERIFY(0x0257963C,normalize_color);

struct ColorFloat { gabi::be<f32> red,green,blue,alpha; };
static void set_sun_colors(u32 packet,u32 index,u32 colorA,u32 colorB) {
 WWHD_FUNC(0x0257E8C0,void,packet,index,colorA,colorB);
 gabi::Local<ColorFloat> first,second;
 u32 destination=packet+index*0x41C+0x1558;
 gabi::call<void>(0x0257963C,first.a,colorA);
 for(u32 i=0;i<4;++i)gabi::store<u32>(destination+4*i,gabi::load<u32>(first.a+4*i));
 gabi::call<void>(0x0257963C,second.a,colorB);
 for(u32 i=0;i<4;++i)gabi::store<u32>(destination+16+4*i,gabi::load<u32>(second.a+4*i));
}
VERIFY(0x0257E8C0,set_sun_colors);
static void copy_matrix(u32 output,u32 input) {
 WWHD_FUNC(0x0257E93C,void,output,input);
 float matrix[12];
 for(u32 i=0;i<12;++i)matrix[i]=gabi::load<float>(input+4*i);
 for(u32 i=0;i<12;++i)gabi::store<float>(output+4*i,matrix[i]);
}
VERIFY(0x0257E93C,copy_matrix);

static float divide_single(float numerator,float denominator) {
 if(numerator!=numerator)return gabi::ppc_qnan(numerator);
 if(denominator!=denominator)return gabi::ppc_qnan(denominator);
 return (float)((double)numerator/(double)denominator);
}
static float subtract_product(float first,float multiplier,float from) {
 if(first!=first)return gabi::ppc_qnan(first);
 if(multiplier!=multiplier)return gabi::ppc_qnan(multiplier);
 if(from!=from)return gabi::ppc_qnan(from);
 return gabi::fnmsubs(first,multiplier,from);
}
static void pntwind_get_info(u32 position,u32 direction,u32 power) {
 WWHD_FUNC(0x0257DE68,void,position,direction,power);
 gabi::Local<Vector> origin;
 float zero=gabi::load<float>(0x10050020);
 gabi::store<float>(direction+4,zero);
 gabi::store<float>(direction,zero);
 gabi::store<float>(direction+8,zero);
 gabi::store<float>(power,zero);
 for(u32 i=0;i<30;++i) {
  if(!gabi::load<u32>(environment()+0x810+4*i))continue;
  u32 influence=gabi::load<u32>(environment()+0x810+4*i);
  float squared=gabi::call<float>(0x028E8DE8,position,influence);
  float distance=gabi::call<float>(0x028F4384,squared);
  float radius=gabi::load<float>(influence+0x18);
  if(!(distance<radius))continue;
  float strength=gabi::load<float>(influence+0x1C);
  if(!(strength>zero) || distance==zero)continue;
  bool cylinder=gabi::load<u8>(influence+0x28)!=0;
  float one=gabi::load<float>(0x100500C4);
  if(!cylinder) {
   float attenuation=one;
   if(radius>zero) {
    float relative=divide_single(distance,radius);
    attenuation=subtract_product(relative,relative,one);
   }
   gabi::store<float>(power,gabi::fmuls_ppc(strength,attenuation));
   for(u32 axis=0;axis<3;++axis) {
    float vector=gabi::load<float>(influence+0xC+4*axis);
    float currentRadius=gabi::load<float>(influence+0x18);
    float center=gabi::load<float>(influence+4*axis);
    gabi::store<float>(origin.a+4*axis,subtract_product(vector,currentRadius,center));
   }
   gabi::call<void>(0x02563F64,origin.a,position,direction);
   squared=gabi::call<float>(0x028E8DE8,position,origin.a);
   distance=gabi::call<float>(0x028F4384,squared);
   radius=gabi::load<float>(influence+0x18);
   if(distance<radius) {
    gabi::store<float>(direction+4,zero);
    gabi::store<float>(direction,zero);
    gabi::store<float>(direction+8,zero);
   }
   return;
  }
  u32 x=gabi::load<u32>(influence+0xC);
  float relative=radius>zero?divide_single(distance,radius):zero;
  gabi::store<u32>(direction,x);
  gabi::store<u32>(direction+4,gabi::load<u32>(influence+0x10));
  gabi::store<u32>(direction+8,gabi::load<u32>(influence+0x14));
  strength=gabi::load<float>(influence+0x1C);
  float attenuation=radius>zero?gabi::fsubs_ppc(one,relative):one;
  gabi::store<float>(power,gabi::fmuls_ppc(strength,attenuation));
  return;
 }
}
VERIFY(0x0257DE68,pntwind_get_info);

static float add_product(float first,float multiplier,float to) {
 if(first!=first)return gabi::ppc_qnan(first);
 if(multiplier!=multiplier)return gabi::ppc_qnan(multiplier);
 if(to!=to)return gabi::ppc_qnan(to);
 return gabi::fmadds(first,multiplier,to);
}
static void squal_proc() {
 WWHD_FUNC(0x0257D208,void,(u32)0);
 u32 env=environment();
 float one=gabi::load<float>(0x100500C4);
 float damping=gabi::load<float>(0x10050148);
 float cutoff=gabi::load<float>(0x100500FC);
 float zero=gabi::load<float>(0x10050020);
 float speed=gabi::load<float>(0x10050140);
 float minimum=gabi::load<float>(0x10050144);
 for(u32 i=0;i<5;++i) {
  u32 squall=env+0x888+0x3C*i;
  u32 influence=squall+0x10;
  if(gabi::load<u8>(squall)!=1)continue;
  float rate=gabi::load<float>(squall+8);
  float x=gabi::load<float>(influence);
  float dx=gabi::load<float>(influence+0xC);
  float y=gabi::load<float>(influence+4);
  gabi::store<float>(influence,add_product(dx,rate,x));
  rate=gabi::load<float>(squall+8);
  float dy=gabi::load<float>(influence+0x10);
  float dz=gabi::load<float>(influence+0x14);
  gabi::store<float>(influence+4,add_product(dy,rate,y));
  float z=gabi::load<float>(influence+8);
  rate=gabi::load<float>(squall+8);
  gabi::store<float>(influence+8,add_product(dz,rate,z));
  float step=gabi::load<float>(squall+0xC);
  gabi::call<void>(0x0200ECD4,influence+0x1C,zero,damping,step,minimum);
  float current=gabi::load<float>(influence+0x1C);
  float amplitude=gabi::load<float>(squall+4);
  float target=gabi::fsubs_ppc(one,current);
  float maxStep=gabi::fmuls_ppc(gabi::fmuls_ppc(target,amplitude),speed);
  gabi::call<void>(0x0200ECD4,influence+0x18,amplitude,target,maxStep,cutoff);
  if(gabi::load<float>(influence+0x1C)<cutoff) {
   gabi::call<void>(0x0257D1B8,influence);
   gabi::store<u8>(squall,0);
  }
 }
}
VERIFY(0x0257D208,squal_proc);

static void squal_set(u32 position,u32 angleX,u32 angleY,float initial,float amplitude,float zero,float speed,float damping) {
 WWHD_FUNC(0x0257DC9C,void,position,angleX,angleY,initial,amplitude,zero,speed,damping);
 u32 env=environment();
 u32 tableX=0x104A44F8+((angleX&0xFFFF)>>3)*8;
 u32 tableY=0x104A44F8+((angleY&0xFFFF)>>3)*8;
 for(u32 i=0;i<5;++i) {
  u32 slot=env+0x888+0x3C*i;
  if(gabi::load<u8>(slot))continue;
  gabi::store<float>(slot+0xC,damping);
  gabi::store<float>(slot+4,amplitude);
  gabi::store<float>(slot+8,speed);
  gabi::store<u8>(slot,1);
  copyVectorBits(slot+0x10,position);
  float cosine=gabi::load<float>(tableX+4);
  float sine=gabi::load<float>(tableY);
  gabi::store<float>(slot+0x1C,gabi::fmuls_ppc(cosine,sine));
  // Native lfs/stfs pair with no arithmetic: a bit copy (keeps a signalling-NaN payload).
  gabi::store<u32>(slot+0x20,gabi::load<u32>(tableX));
  cosine=gabi::load<float>(tableX+4);
  float cosineY=gabi::load<float>(tableY+4);
  gabi::store<float>(slot+0x2C,zero);
  gabi::store<float>(slot+0x28,initial);
  gabi::store<float>(slot+0x30,initial);
  gabi::store<float>(slot+0x24,gabi::fmuls_ppc(cosine,cosineY));
  gabi::call<void>(0x0257DC90,slot+0x10);
  return;
 }
}
VERIFY(0x0257DC9C,squal_set);

static u32 sun_packet_constructor(u32 object) {
 WWHD_FUNC(0x025786F0,u32,object);
 if(!object)object=gabi::call<u32>(0x0273AD10,(u32)0x2628);
 if(!object)return 0;
 gabi::call<void>(0x027F1278,object);
 gabi::store<u32>(object+0xC,0x10050224);
 gabi::call<void>(0x028F521C,object+0xB0,(u32)0x14);
 float zero=gabi::load<float>(0x10050020);
 gabi::store<u8>(object+0xC4,0);
 gabi::store<float>(object+0xC8,zero);
 gabi::store<float>(object+0xD0,zero);
 gabi::store<u8>(object+0xC5,0);
 gabi::store<float>(object+0xCC,zero);
 gabi::call<void>(0x028F521C,object+0xD4,(u32)0x14);
 gabi::store<u32>(object+0xE8,0);
 gabi::store<u32>(object+0xEC,0);
 gabi::call<void>(0x028EFFD0,object+0xF0,(u32)4,(u32)0x4C0,(u32)0x02582D58);
 gabi::call<void>(0x028EFFD0,object+0x13F0,(u32)4,(u32)0x41C,(u32)0x02582E0C);
 gabi::call<void>(0x028EFFD0,object+0x2460,(u32)6,(u32)0x4C,(u32)0x027FE344);
 return object;
}
VERIFY(0x025786F0,sun_packet_constructor);
static u32 housi_packet_constructor(u32 object) {
 WWHD_FUNC(0x02579D2C,u32,object);
 if(!object)object=gabi::call<u32>(0x0273AD10,(u32)0x10038);
 if(!object)return 0;
 gabi::call<void>(0x027F1278,object);
 gabi::store<u32>(object+0xC,0x10050374);
 gabi::call<void>(0x028F521C,object+0x98,(u32)4);
 gabi::store<u32>(object+0x9C,0);
 gabi::call<void>(0x028F521C,object+0xA0,(u32)0x8E8);
 float zero=gabi::load<float>(0x10050020);
 gabi::store<u32>(object+0x990,0);
 gabi::store<float>(object+0x98C,zero);
 gabi::store<u16>(object+0x988,0);
 gabi::call<void>(0x028EFFD0,object+0x994,(u32)30,(u32)0x4C0,(u32)0x025835F0);
 gabi::call<void>(0x028EFFD0,object+0x9818,(u32)30,(u32)0x370,(u32)0x02583210);
 gabi::call<void>(0x027FD6F4,object+0xFF38);
 gabi::call<void>(0x027FB40C,object+0xFF44);
 gabi::store<u32>(object+0xFF50,0x1016EF84);
 gabi::call<void>(0x028F521C,object+0xFFB8,(u32)0x34);
 if(object+0xFFB8==0)gabi::call<void>(0x0273AD10,(u32)0x30);
 gabi::call<void>(0x027FE344,object+0xFFEC);
 return object;
}
VERIFY(0x02579D2C,housi_packet_constructor);

static void construct_texture_constants(u32 object,u32 block) {
 float zero=gabi::load<float>(0x10145180);
 float one=gabi::load<float>(0x1014517C);
 for(u32 i=0;i<44;++i)gabi::store<float>(object+block+4*i,i%4==3?one:zero);
 for(u32 offset=0xB0;offset<=0xF0;offset+=0x20)
  gabi::call<void>(0x028EFFD0,object+block+offset,(u32)2,(u32)16,(u32)0x02582DE0);
 for(u32 offset=0x110;offset<=0x260;offset+=0x30)
  if(object+block+offset==0)gabi::call<void>(0x0273AD10,(u32)0x30);
 for(u32 offset=0x290;offset<=0x2E0;offset+=0x10)
  if(object+block+offset==0)gabi::call<void>(0x0273AD10,(u32)16);
}
static u32 sun_geometry_packet_constructor(u32 object) {
 WWHD_FUNC(0x025787D4,u32,object);
 if(!object)object=gabi::call<u32>(0x0273AD10,(u32)0xA088);
 if(!object)return 0;
 gabi::call<void>(0x027F1278,object);gabi::store<u32>(object+12,0x10050254);
 gabi::call<void>(0x028F521C,object+0x98,(u32)12);
 float zero=gabi::load<float>(0x10050020);
 gabi::store<u16>(object+0x128,0);gabi::store<u8>(object+0x12A,0);gabi::store<u32>(object+0x12C,0);
 for(u32 offset=0x110;offset<=0x124;offset+=4)gabi::store<float>(object+offset,zero);
 gabi::call<void>(0x028EFFD0,object+0x130,(u32)9,(u32)0x4C0,(u32)0x02582D58);
 gabi::call<void>(0x028EFFD0,object+0x2BF0,(u32)9,(u32)0x41C,(u32)0x02582E0C);
 gabi::call<void>(0x028EFFD0,object+0x50EC,(u32)3,(u32)0x4C,(u32)0x027FE344);
 u32 geometry=object+0x51D4;gabi::store<u32>(object+0x51D0,0);
 if(!geometry)geometry=gabi::call<u32>(0x0273AD10,(u32)0x4A98);
 if(geometry) {
  gabi::call<void>(0x028EFFD0,geometry,(u32)32,(u32)0x254,(u32)0x025830D0);
  gabi::store<u32>(geometry+0x4A80,0);gabi::store<u32>(geometry+0x4A90,0);
  gabi::store<u32>(geometry+0x4A88,0x98);gabi::store<u8>(geometry+0x4A94,0);gabi::store<u32>(geometry+0x4A84,0);
  for(u32 side=0;side<2;++side)for(u32 i=0;i<16;++i)gabi::store<u32>(geometry+side*0x254+i*0x4A8,0);
 }
 u32 renderer=object+0x9C6C;
 gabi::call<void>(0x027FD6F4,renderer);gabi::call<void>(0x027FB40C,renderer+12);
 gabi::store<u32>(renderer+24,0x1016EF84);gabi::call<void>(0x028F521C,renderer+0x80,(u32)0x34);
 if(renderer+0x80==0)gabi::call<void>(0x0273AD10,(u32)0x30);
 gabi::call<void>(0x027FB40C,renderer+0xB4);gabi::store<u32>(renderer+0xC0,0x1016EFB4);
 gabi::call<void>(0x028F521C,renderer+0x128,(u32)0x2F0);construct_texture_constants(renderer,0x128);
 gabi::store<u8>(renderer+0x418,0);return object;
}
VERIFY(0x025787D4,sun_geometry_packet_constructor);
static u32 rain_packet_constructor(u32 object) {
 WWHD_FUNC(0x02579280,u32,object);
 if(!object)object=gabi::call<u32>(0x0273AD10,(u32)0x1ACA40);
 if(!object)return 0;
 gabi::call<void>(0x027F1278,object);gabi::store<u32>(object+12,0x10050284);
 gabi::call<void>(0x028F521C,object+0x98,(u32)8);gabi::call<void>(0x028F521C,object+0xA0,(u32)0x36B0);
 float zero=gabi::load<float>(0x10050020);
 gabi::store<u16>(object+0x3758,0);gabi::store<u8>(object+0x379C,0);gabi::store<u32>(object+0x375C,0);
 for(u32 offset: {0x3790u,0x3794u,0x3784u,0x378Cu,0x3754u,0x3798u,0x3788u,0x3750u})gabi::store<float>(object+offset,zero);
 gabi::call<void>(0x028F521C,object+0x379D,(u32)250);gabi::store<u32>(object+0x3898,0);
 gabi::call<void>(0x028EFFD0,object+0x389C,(u32)1000,(u32)0x4C0,(u32)0x02583188);
 gabi::call<void>(0x028EFFD0,object+0x12C69C,(u32)250,(u32)0x4C0,(u32)0x02583188);
 gabi::call<void>(0x028EFFD0,object+0x176A20,(u32)250,(u32)0x370,(u32)0x02583210);
 u32 texture=object+0x1AC580;
 gabi::call<void>(0x027FB40C,texture);gabi::store<u32>(texture+12,0x1016EFB4);
 gabi::call<void>(0x028F521C,texture+0x74,(u32)0x2F0);construct_texture_constants(texture,0x74);
 gabi::store<u32>(texture+12,0x1016EFE4);gabi::store<u32>(texture+0x368,0);gabi::store<u32>(texture+0x36C,0);
 u32 renderer=object+0x1AC8F0;
 gabi::call<void>(0x027FD6F4,renderer);gabi::call<void>(0x027FB40C,renderer+12);
 gabi::store<u32>(renderer+24,0x1016EF84);gabi::call<void>(0x028F521C,renderer+0x80,(u32)0x34);
 if(renderer+0x80==0)gabi::call<void>(0x0273AD10,(u32)0x30);
 gabi::call<void>(0x028EFFD0,object+0x1AC9A4,(u32)2,(u32)0x4C,(u32)0x027FE344);
 return object;
}
VERIFY(0x02579280,rain_packet_constructor);
static u32 dual_texture_constructor(u32 object) {
 WWHD_FUNC(0x02582E0C,u32,object);
 if(!object)object=gabi::call<u32>(0x0273AD10,(u32)0x41C);
 if(!object)return 0;
 gabi::call<void>(0x027FD6F4,object);
 gabi::call<void>(0x027FB40C,object+0xC);
 gabi::store<u32>(object+0x18,0x1016EF84);
 gabi::call<void>(0x028F521C,object+0x80,(u32)0x34);
 if(object+0x80==0)gabi::call<void>(0x0273AD10,(u32)0x30);
 gabi::call<void>(0x027FB40C,object+0xB4);
 gabi::store<u32>(object+0xC0,0x1016EFB4);
 gabi::call<void>(0x028F521C,object+0x128,(u32)0x2F0);
 construct_texture_constants(object,0x128);
 gabi::store<u8>(object+0x418,0);
 return object;
}
VERIFY(0x02582E0C,dual_texture_constructor);
static u32 texture_with_constants_constructor(u32 object) {
 WWHD_FUNC(0x02583210,u32,object);
 if(!object)object=gabi::call<u32>(0x0273AD10,(u32)0x370);
 if(!object)return 0;
 gabi::call<void>(0x027FB40C,object);
 gabi::store<u32>(object+0xC,0x1016EFB4);
 gabi::call<void>(0x028F521C,object+0x74,(u32)0x2F0);
 construct_texture_constants(object,0x74);
 gabi::store<u32>(object+0x36C,0);
 gabi::store<u32>(object+0xC,0x1016EFE4);
 gabi::store<u32>(object+0x368,0);
 return object;
}
VERIFY(0x02583210,texture_with_constants_constructor);

static void precipitation_packet_destructor(u32 object,u32 flags) {
 WWHD_FUNC(0x02582410,void,object,flags);
 if(!object)return;
 gabi::call<void>(0x028F0164,object+0xC5958,(u32)3,(u32)0x198,(u32)0x027BE2B0,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0xAFFE8,(u32)100,(u32)0x370,(u32)0x025841F4,(u32)0,(u32)0);
 gabi::call<void>(0x027FB528,object+0xAFBCC+0xB4,(u32)0);
 gabi::call<void>(0x027FB528,object+0xAFBCC+0xC,(u32)0);
 gabi::call<void>(0x027FD764,object+0xAFBCC,(u32)2);
 gabi::call<void>(0x028F0164,object+0x11DC,(u32)2,(u32)0x574F8,(u32)0x02584E04,(u32)0,(u32)0);
 gabi::call<void>(0x027F13DC,object,(u32)0);
 if(flags&1)gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02582410,precipitation_packet_destructor);

static void draw_packet_02580204(u32 packet) {
 WWHD_FUNC(0x02580204,void,packet);
 gabi::call<void>(0x02572FC4,(u32)0x104B45F8,packet+0x98);
}
VERIFY(0x02580204,draw_packet_02580204);

static void draw_packet_02580FB8(u32 packet) {
 WWHD_FUNC(0x02580FB8,void,packet);
 gabi::call<void>(0x02574144,(u32)0x104B45F8,packet+0x98);
}
VERIFY(0x02580FB8,draw_packet_02580FB8);

static void draw_packet_0258142C(u32 packet) {
 WWHD_FUNC(0x0258142C,void,packet);
 gabi::call<void>(0x025720D8,(u32)0x104B45F8,packet+0xBC34);
}
VERIFY(0x0258142C,draw_packet_0258142C);

static void draw_packet_02581B50(u32 packet) {
 WWHD_FUNC(0x02581B50,void,packet);
 gabi::call<void>(0x025727BC,(u32)0x104B45F8,packet+0x98);
}
VERIFY(0x02581B50,draw_packet_02581B50);

static void draw_packet_02582010(u32 packet) {
 WWHD_FUNC(0x02582010,void,packet);
 gabi::call<void>(0x0257556C,(u32)0x104B45F8,packet+0x98);
}
VERIFY(0x02582010,draw_packet_02582010);

static void draw_packet_02582400(u32 packet) {
 WWHD_FUNC(0x02582400,void,packet);
 gabi::call<void>(0x02575B6C,(u32)0x104B45F8,packet+0x98);
}
VERIFY(0x02582400,draw_packet_02582400);

static void cloud_packet_destructor(u32 object,u32 flags) {
 WWHD_FUNC(0x0257ED44,void,object,flags);
 if(!object)return;
 gabi::call<void>(0x027FB528,object+0x9C6C+0xB4,(u32)0);
 gabi::call<void>(0x027FB528,object+0x9C6C+0xC,(u32)0);
 gabi::call<void>(0x027FD764,object+0x9C6C,(u32)2);
 u32 geometry=object+0x51D4;
 if(geometry) {
  for(u32 i=0;i<16;++i) {
   release_geometry_element(geometry+i*0x4A8,0);
   release_geometry_element(geometry+i*0x4A8,0x254);
  }
  gabi::store<u32>(geometry+0x4A90,0);
  gabi::call<void>(0x028F0164,geometry,(u32)32,(u32)0x254,(u32)0x02584194,(u32)0,(u32)0);
 }
 gabi::call<void>(0x028F0164,object+0x50EC,(u32)3,(u32)0x4C,(u32)0x027FE4B0,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0x2BF0,(u32)9,(u32)0x41C,(u32)0x02584128,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0x130,(u32)9,(u32)0x4C0,(u32)0x0258401C,(u32)0,(u32)0);
 gabi::call<void>(0x027F13DC,object,(u32)0);
 if(flags&1)gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x0257ED44,cloud_packet_destructor);
static void rain_packet_destructor(u32 object,u32 flags) {
 WWHD_FUNC(0x0257F378,void,object,flags);
 if(!object)return;
 gabi::call<void>(0x028F0164,object+0x1AC580+0x424,(u32)2,(u32)0x4C,(u32)0x027FE4B0,(u32)0,(u32)0);
 gabi::call<void>(0x027FB528,object+0x1AC580+0x37C,(u32)0);
 gabi::call<void>(0x027FD764,object+0x1AC580+0x370,(u32)2);
 gabi::call<void>(0x027FB528,object+0x1AC580,(u32)0);
 gabi::call<void>(0x028F0164,object+0x176A20,(u32)250,(u32)0x370,(u32)0x025841F4,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0x12C69C,(u32)250,(u32)0x4C0,(u32)0x025842A8,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0x389C,(u32)1000,(u32)0x4C0,(u32)0x025842A8,(u32)0,(u32)0);
 gabi::call<void>(0x027F13DC,object,(u32)0);
 if(flags&1)gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x0257F378,rain_packet_destructor);
static void snow_packet_destructor(u32 object,u32 flags) {
 WWHD_FUNC(0x02580214,void,object,flags);
 if(!object)return;
 gabi::call<void>(0x027FE4B0,object+0x92B6E0+0xB4,(u32)2);
 gabi::call<void>(0x027FB528,object+0x92B6E0+0xC,(u32)0);
 gabi::call<void>(0x027FD764,object+0x92B6E0,(u32)2);
 gabi::call<void>(0x028F0164,object+0x5D00E0,(u32)4000,(u32)0x370,(u32)0x025841F4,(u32)0,(u32)0);
 gabi::call<void>(0x027FB528,object+0x5CFD70,(u32)0);
 gabi::call<void>(0x028F0164,object+0x12C56C,(u32)4000,(u32)0x4C0,(u32)0x025844F8,(u32)0,(u32)0);
 gabi::call<void>(0x028F0164,object+0x376C,(u32)1000,(u32)0x4C0,(u32)0x025844F8,(u32)0,(u32)0);
 gabi::call<void>(0x027F13DC,object,(u32)0);
 if(flags&1)gabi::call<void>(0x0273AF40,object);
}
VERIFY(0x02580214,snow_packet_destructor);

struct SafeString { gabi::be<u32> text,vtable; };
static void string_prepare(u32 object) {
 u32 table=gabi::load<u32>(object+4);
 gabi::call_ptr<void>(gabi::load<u32>(table+0x14),object);
}
static bool same_string_bytes(u32 first,u32 second) {
 if(first==second)return true;
 for(u32 i=0;i<0x40001;++i) {
  u8 a=gabi::load<u8>(first+i),b=gabi::load<u8>(second+i);
  if(a!=b)return false;
  if(a==0)return true;
 }
 return false;
}
static void windline_proc() {
 WWHD_FUNC(0x0257836C,void,(u32)0);
 u8 phase=gabi::load<u8>(environment()+0xAB0);
 if(phase==0) {
  if(gabi::load<u32>(environment()+0xAB4)) {
   gabi::call<void>(0x02577110);
   gabi::store<u8>(environment()+0xAB0,1);
  }
 } else if(phase==1)gabi::call<void>(0x02577138);
 gabi::call<void>(0x025200D4);
 s32 room=gabi::load<s8>(0x1047E6C8);
 u32 roomInfo=0;
 if(room>=0) {
  u32 game=gabi::call<u32>(0x025200D4);
  u32 status=gabi::call<u32>(0x025C11DC,game+0x51CC,room);
  u32 target=gabi::load<u32>(gabi::load<u32>(status)+0x1DC);
  roomInfo=gabi::call_ptr<u32>(target,status);
 }
 gabi::store<u32>(environment()+0xAA8,0);
 if(roomInfo && (gabi::load<u32>(roomInfo)&0x100000)) {
  gabi::Local<SafeString> stage,gameStage;
  gabi::store<u32>(stage.a,0x1004FD60);
  gabi::store<u32>(stage.a+4,0x1004FDD0);
  u32 game=gabi::call<u32>(0x025200D4);
  gabi::store<u32>(gameStage.a,game+0x5134);
  gabi::store<u32>(gameStage.a+4,0x1004FDD0);
  string_prepare(stage.a);
  string_prepare(stage.a);
  u32 stageText=gabi::load<u32>(stage.a);
  string_prepare(gameStage.a);
  u32 gameText=gabi::load<u32>(gameStage.a);
  bool same=stageText==gameText;
  if(!same)same=same_string_bytes(gabi::load<u32>(stage.a),gabi::load<u32>(gameStage.a));
  if(!same) {
   u32 env=environment();
   float power=gabi::call<float>(0x02578348);
   float scaled=gabi::fmuls_ppc(power,gabi::load<float>(0x10050028));
   gabi::store<s32>(env+0xAA8,gabi::ftoi(scaled));
  }
 }
 float disabled=gabi::load<float>(0x10050024);
 if(gabi::load<float>(environment()+0xA10)<disabled) {
  gabi::store<u32>(environment()+0xAA8,0);
  gabi::store<float>(environment()+0xA10,disabled);
 }
 phase=gabi::load<u8>(environment()+0xAA4);
 if(phase==0) {
  if(!gabi::load<u32>(environment()+0xAA8))return;
  // The native scope constructor allocates/writes exactly one guest pointer (4 bytes).
  gabi::Local<gabi::be<u32>> heapScope;
  gabi::call<void>(0x025F01D8,heapScope.a,(u32)0x1004FE84,(u32)0x82C,(u32)0);
  u32 env=environment();
  u32 packet=gabi::call<u32>(0x0273AE48,(u32)0x768,(u32)32);
  gabi::store<u32>(env+0xAAC,packet);
  if(gabi::load<u32>(environment()+0xAAC)) {
   for(u32 i=0;i<30;++i) {
    packet=gabi::load<u32>(environment()+0xAAC);
    gabi::store<u32>(packet+0x34*i+0x24,0);
    packet=gabi::load<u32>(environment()+0xAAC);
    gabi::store<u32>(packet+0x34*i,0);
   }
   gabi::call<void>(0x02564184);
   gabi::call<void>(0x025642AC);
   gabi::store<u8>(environment()+0xAA4,1);
  } else gabi::call<void>(0x025F02D0,heapScope.a);
  gabi::call<void>(0x025F0270,heapScope.a,(u32)2);
 } else if(phase==1)gabi::call<void>(0x025642AC);
}
VERIFY(0x0257836C,windline_proc);

static u32 safe_string_equal(u32 first,u32 second) {
 WWHD_FUNC(0x02582C08,u32,first,second);
 string_prepare(first);
 string_prepare(first);
 u32 cached=gabi::load<u32>(first);
 string_prepare(second);
 if(cached==gabi::load<u32>(second))return 1;
 return same_string_bytes(gabi::load<u32>(first),gabi::load<u32>(second));
}
VERIFY(0x02582C08,safe_string_equal);

static u32 archive_resource_array(u32 archive) {
 u32 offset=gabi::load<u32>(archive+0x24);
 return offset?archive+0x24+offset:0;
}
static void wave_proc() {
 WWHD_FUNC(0x02583D00,void,(u32)0);
 u8 phase=gabi::load<u8>(environment()+0xA98);
 if(phase==0) {
  if(gabi::load<s16>(environment()+0x9F8)==0)return;
  gabi::Local<gabi::be<u32>> heapScope;
  gabi::call<void>(0x025F01D8,heapScope.a,(u32)0x1004FE68,(u32)0x17FA10,(u32)0);
  u32 packet=gabi::call<u32>(0x0273AE48,(u32)0x9DCC0,(u32)32);
  if(packet)packet=gabi::call<u32>(0x0257A920,packet);
  gabi::store<u32>(environment()+0xAA0,packet);
  if(gabi::load<u32>(environment()+0xAA0)) {
   gabi::Local<SafeString> package,entry;
   gabi::store<u32>(package.a,0x1004FDC8);
   gabi::store<u32>(package.a+4,0x1004FDD0);
   gabi::store<u32>(entry.a,0x1005000C);
   gabi::store<u32>(entry.a+4,0x1004FDD0);
   u32 resource=gabi::call<u32>(0x026124B0,gabi::load<u32>(0x101F4F7C),package.a,entry.a,(u32)0);
   u32 archive=gabi::call<u32>(0x027E2DC0,resource);
   u32 texture=gabi::call<u32>(0x027DFA24,archive_resource_array(archive),(u32)0x1004FEF8);
   packet=gabi::load<u32>(environment()+0xAA0);
   gabi::store<u32>(packet+0x98,texture);
   texture=gabi::call<u32>(0x027DFA24,archive_resource_array(archive),(u32)0x1004FDF8);
   packet=gabi::load<u32>(environment()+0xAA0);
   gabi::store<u32>(packet+0x9C,texture);
   gabi::call<void>(0x0257AA4C,gabi::load<u32>(environment()+0xAA0));
   for(u32 i=0;i<300;++i) {
    packet=gabi::load<u32>(environment()+0xAA0);
    gabi::store<u8>(packet+0xD4+0x38*i,0);
   }
   gabi::call<void>(0x0256A448);
   gabi::store<u8>(environment()+0xA98,1);
  } else gabi::call<void>(0x025F02D0,heapScope.a);
  gabi::call<void>(0x025F0270,heapScope.a,(u32)2);
 } else if(phase==1) {
  if(gabi::load<s16>(environment()+0x9F8)==0)gabi::store<u8>(environment()+0xA98,2);
  else gabi::call<void>(0x0256A448);
 } else if(phase==2)gabi::store<u8>(environment()+0xA98,3);
 else if(phase==3) {
  gabi::store<u8>(environment()+0xA98,0);
  gabi::call<void>(0x02577FC8,environment()+0xAA0);
 }
}
VERIFY(0x02583D00,wave_proc);

static u32 wave_packet_constructor(u32 object) {
 WWHD_FUNC(0x0257A920,u32,object);
 if(!object)object=gabi::call<u32>(0x0273AD10,(u32)0x9DCC0);
 if(!object)return 0;
 gabi::call<void>(0x027F1278,object);
 gabi::store<u32>(object+0xC,0x100503A4);
 gabi::call<void>(0x028F521C,object+0x98,(u32)8);
 gabi::call<void>(0x028F521C,object+0xA0,(u32)0x41A0);
 float zero=gabi::load<float>(0x10050020);
 gabi::store<float>(object+0x4240,zero);
 gabi::store<u32>(object+0x4248,0);
 gabi::store<float>(object+0x4244,zero);
 gabi::call<void>(0x028EFFD0,object+0x424C,(u32)300,(u32)0x4C0,(u32)0x02583C78);
 gabi::call<void>(0x028EFFD0,object+0x5D350,(u32)300,(u32)0x370,(u32)0x02583210);
 gabi::call<void>(0x027FD6F4,object+0x9DA90);
 gabi::call<void>(0x027FB40C,object+0x9DA9C);
 gabi::store<u32>(object+0x9DAA8,0x1016EF84);
 gabi::call<void>(0x028F521C,object+0x9DB10,(u32)0x34);
 if(object+0x9DB10==0)gabi::call<void>(0x0273AD10,(u32)0x30);
 gabi::call<void>(0x027FE344,object+0x9DB44);
 gabi::call<void>(0x028F521C,object+0x9DB90,(u32)300);
 return object;
}
VERIFY(0x0257A920,wave_packet_constructor);
static u32 precipitation_packet_constructor(u32 object) {
 WWHD_FUNC(0x0257A42C,u32,object);
 if(!object)object=gabi::call<u32>(0x0273AD10,(u32)0xC5E20);
 if(!object)return 0;
 gabi::call<void>(0x027F1278,object);
 gabi::store<u32>(object+0xC,0x100503D4);
 gabi::call<void>(0x028F521C,object+0x98,(u32)12);
 gabi::call<void>(0x028F521C,object+0xA4,(u32)0x1130);
 float zero=gabi::load<float>(0x10050020);
 gabi::store<u32>(object+0x11D8,0);
 gabi::store<float>(object+0x11D4,zero);
 gabi::call<void>(0x028EFFD0,object+0x11DC,(u32)2,(u32)0x574F8,(u32)0x02583B74);
 u32 renderer=object+0xAFBCC;
 gabi::call<void>(0x027FD6F4,renderer);
 gabi::call<void>(0x027FB40C,renderer+0xC);
 gabi::store<u32>(renderer+0x18,0x1016EF84);
 gabi::call<void>(0x028F521C,renderer+0x80,(u32)0x34);
 if(renderer+0x80==0)gabi::call<void>(0x0273AD10,(u32)0x30);
 gabi::call<void>(0x027FB40C,renderer+0xB4);
 gabi::store<u32>(renderer+0xC0,0x1016EFB4);
 gabi::call<void>(0x028F521C,renderer+0x128,(u32)0x2F0);
 construct_texture_constants(renderer,0x128);
 gabi::store<u8>(renderer+0x418,0);
 gabi::call<void>(0x028EFFD0,object+0xAFFE8,(u32)100,(u32)0x370,(u32)0x02583210);
 gabi::call<void>(0x028EFFD0,object+0xC57A8,(u32)3,(u32)0x90,(u32)0x027BE6B8);
 gabi::call<void>(0x028EFFD0,object+0xC5958,(u32)3,(u32)0x198,(u32)0x027BDF7C);
 gabi::call<void>(0x0257A0E8,object);
 return object;
}
VERIFY(0x0257A42C,precipitation_packet_constructor);
static void refresh_precipitation_textures(u32 packet) {
 WWHD_FUNC(0x0257A7A8,void,packet);
 gabi::call<void>(0x0274FBF8,gabi::load<u32>(0x101F8B18));
 for(u32 i=0;i<3;++i) {
  u32 resource=gabi::load<u32>(packet+0x98+4*i);
  if(!resource) continue;
  u32 source=packet+0xC57A8+0x90*i;
  u32 target=packet+0xC5958+0x198*i;
  gabi::call<void>(0x02773798,source,gabi::load<u32>(resource+0x20));
  bool same=true;
  for(u32 offset : {4u,8u,12u,16u,20u,24u,56u,52u,28u})
   if(gabi::load<u32>(target+offset)!=gabi::load<u32>(source+offset)) { same=false; break; }
  if(!same) {
   gabi::call<void>(0x027BDEB4,target,source);
   u8 flags=gabi::load<u8>(target+0x190);
   gabi::store<u32>(target+0x150,1);
   gabi::store<u32>(target+0x154,1);
   gabi::store<u32>(target+0x158,1);
   gabi::store<u8>(target+0x190,flags|4);
  } else {
   u8 flags=gabi::load<u8>(target+0x190);
   u32 data=gabi::load<u32>(source+0x28),size=gabi::load<u32>(source+0x30);
   gabi::store<u32>(target+0x28,data);
   gabi::store<u8>(target+0x190,flags|4);
   gabi::store<u32>(target+0xDC,size);
   gabi::store<u32>(target+0x30,size);
   gabi::store<u32>(target+0x154,1);
   gabi::store<u32>(target+0xD4,data);
   gabi::store<u32>(target+0x158,1);
   gabi::store<u32>(target+0x150,1);
  }
 }
 gabi::call<void>(0x0274FCCC,gabi::load<u32>(0x101F8B18));
}
VERIFY(0x0257A7A8,refresh_precipitation_textures);
static void set_wave_colors(u32 packet,u32 index,u32 colorA,u32 colorB) {
 WWHD_FUNC(0x02582518,void,packet,index,colorA,colorB);
 gabi::Local<ColorFloat> normalized,secondary;
 gabi::call<void>(0x0257963C,normalized.a,colorA);
 u32 target=packet+0x5D350+index*0x370;
 u32 environment=gabi::call<u32>(0x02555D0C);
 gabi::call<void>(0x0274D458,target+0xB4,normalized.a,gabi::load<float>(environment+0x10BC));
 float denominator=gabi::load<float>(0x10050048);
 gabi::store<float>(target+0xC0,divide_single((float)gabi::load<u8>(colorA+3),denominator));
 gabi::call<void>(0x027FC500,target,target+0xB4,(u32)0);
 gabi::call<void>(0x0257963C,normalized.a,colorB);
 environment=gabi::call<u32>(0x02555D0C);
 gabi::call<void>(0x0274D458,secondary.a,normalized.a,gabi::load<float>(environment+0x10B8));
 gabi::store<float>(secondary.a+12,divide_single((float)gabi::load<u8>(colorB+3),denominator));
 gabi::call<void>(0x027FC650,target,secondary.a,(u32)0);
 gabi::call<void>(0x027FC650,target,secondary.a,(u32)3);
}
VERIFY(0x02582518,set_wave_colors);
static u32 mud_packet_constructor(u32 object) {
 WWHD_FUNC(0x02579934,u32,object);
 if(!object) { object=gabi::call<u32>(0x0273AD10,(u32)0x139250); if(!object) return 0; }
 gabi::call<void>(0x027F1278,object);
 gabi::store<u32>(object+0xC,0x10050344);
 gabi::call<void>(0x028F521C,object+0x98,(u32)4);
 gabi::call<void>(0x028F521C,object+0x9C,(u32)0x5DC0);
 float zero=gabi::load<float>(0x10050020);
 gabi::store<float>(object+0x5E60,zero);
 gabi::store<float>(object+0x5E5C,zero);
 gabi::store<float>(object+0x5E64,zero);
 gabi::store<u16>(object+0x5E68,0);
 gabi::store<u32>(object+0x5E6C,0);
 gabi::call<void>(0x028F521C,object+0x5E70,(u32)0x258);
 gabi::store<u32>(object+0x60C8,0);
 gabi::call<void>(0x028EFFD0,object+0x60CC,(u32)600,(u32)0x4C0,(u32)0x0258350C);
 gabi::call<void>(0x028EFFD0,object+0xB82D0,(u32)600,(u32)0x370,(u32)0x02583210);
 u32 renderer=object+0x139150;
 gabi::call<void>(0x027FD6F4,renderer);
 gabi::call<void>(0x027FB40C,renderer+0xC);
 gabi::store<u32>(renderer+0x18,0x1016EF84);
 gabi::call<void>(0x028F521C,renderer+0x80,(u32)0x34);
 if(!(renderer+0x80)) gabi::call<u32>(0x0273AD10,(u32)0x30);
 gabi::call<void>(0x027FE344,renderer+0xB4);
 return object;
}
VERIFY(0x02579934,mud_packet_constructor);
static u32 large_precipitation_packet_constructor(u32 object) {
 WWHD_FUNC(0x0257FE7C,u32,object);
 if(!object)object=gabi::call<u32>(0x0273AD10,(u32)0x92B7E0);
 if(!object)return 0;
 gabi::call<void>(0x027F1278,object);gabi::store<u32>(object+12,0x100502B4);
 gabi::call<void>(0x028F521C,object+0x98,(u32)4);
 gabi::call<void>(0x028F521C,object+0x9C,(u32)0x36B0);
 float zero=gabi::load<float>(0x10050020);
 gabi::store<float>(object+0x375C,zero);gabi::store<u16>(object+0x3762,0);
 gabi::store<float>(object+0x3758,zero);gabi::store<u32>(object+0x3768,0);
 gabi::store<u32>(object+0x3764,0);gabi::store<u16>(object+0x3760,0);
 gabi::call<void>(0x028EFFD0,object+0x376C,(u32)1000,(u32)0x4C0,(u32)0x02584410);
 gabi::call<void>(0x028EFFD0,object+0x12C56C,(u32)4000,(u32)0x4C0,(u32)0x02584410);
 u32 texture=object+0x5CFD70;
 gabi::call<void>(0x027FB40C,texture);gabi::store<u32>(texture+12,0x1016EFB4);
 gabi::call<void>(0x028F521C,texture+0x74,(u32)0x2F0);
 construct_texture_constants(texture,0x74);
 gabi::store<u32>(texture+12,0x1016EFE4);gabi::store<u32>(texture+0x36C,0);gabi::store<u32>(texture+0x368,0);
 gabi::call<void>(0x028EFFD0,object+0x5D00E0,(u32)4000,(u32)0x370,(u32)0x02583210);
 u32 renderer=object+0x92B6E0;
 gabi::call<void>(0x027FD6F4,renderer);gabi::call<void>(0x027FB40C,renderer+12);
 gabi::store<u32>(renderer+24,0x1016EF84);
 gabi::call<void>(0x028F521C,renderer+0x80,(u32)0x34);
 if(renderer+0x80==0)gabi::call<void>(0x0273AD10,(u32)0x30);
 gabi::call<void>(0x027FE344,renderer+0xB4);
 return object;
}
VERIFY(0x0257FE7C,large_precipitation_packet_constructor);
static u32 lookup_weather_material(u32 packet,u32 name) {
 gabi::Local<SafeString> text;
 gabi::store<u32>(text.a,name);gabi::store<u32>(text.a+4,0x1004FDD0);
 u32 state=gabi::call<u32>(0x027FFCBC,packet);
 u32 index=gabi::call<u32>(0x027B90AC,gabi::load<u32>(state+4),text.a);
 if((s32)index<0) return 0;
 u32 count=gabi::load<u32>(state+8),table=gabi::load<u32>(state+12);
 u32 entry=table+(index<count?index*36:0);
 if(!gabi::load<u8>(entry+32)) {
  u32 materials=gabi::load<u32>(state+4),materialCount=gabi::load<u32>(materials+28);
  u32 material=index<materialCount?gabi::load<u32>(materials+32)+index*132:0;
  gabi::call<void>(0x02800B0C,entry,material,(u32)0);
  count=gabi::load<u32>(state+8);table=gabi::load<u32>(state+12);
 }
 return table+(index<count?index*36:0);
}
static void prepare_weather_pair(u32 pair,u32 material,u32 attributes) {
 gabi::store<u32>(pair+0x4AC,17);
 gabi::store<u32>(pair+0x4B4,attributes);
 for(u32 side=0;side<2;++side) {
  u32 element=pair+side*0x254;
  u32 data=gabi::load<u32>(element);
  if(!data) {
   u32 allocator=gabi::call<u32>(0x02756140,gabi::load<u32>(0x101F8B4C),data);
   u32 table=gabi::load<u32>(allocator+12);
   u32 allocation=gabi::call_ptr<u32>(gabi::load<u32>(table+0x34),allocator,(u32)0x50,(u32)0x40);
   if(allocation) {gabi::store<u32>(element+0x250,allocation);gabi::store<u32>(element+0x24C,4);}
   data=gabi::load<u32>(element+0x250);
   gabi::store<u32>(element,data);
  }
  gabi::call<void>(0x027FF478,element+4,data,(u32)4,pair+0x4AC);
  if(material && material!=gabi::load<u32>(pair+0x4B8))
   gabi::call<void>(0x027FF530,material,element+0x158,element+4,pair+0x4AC,(u32)0);
 }
 gabi::store<u32>(pair+0x4B8,material);
 gabi::store<u8>(pair+0x4BC,1);
}
struct WeatherTextureDescriptor { u8 bytes[0x90]; };
static bool weather_stage_equals(u32 name);
static void setup_large_precipitation_geometry(u32 packet) {
 WWHD_FUNC(0x02580318,void,packet);
 gabi::store<u32>(packet+0x3768,lookup_weather_material(packet,0x100501CC));
 bool special=weather_stage_equals(0x100501C4);
 if(special) {
  for(u32 i=0;i<4000;++i) {
   prepare_weather_pair(packet+0x12C56C+i*0x4C0,gabi::load<u32>(packet+0x3768),0x100501F0);
   gabi::call<void>(0x027FB5D4,packet+0x5D00E0+i*0x370,(u32)0);
  }
 } else {
  for(u32 i=0;i<250;++i)
   for(u32 quad=0;quad<4;++quad)
    prepare_weather_pair(packet+0x376C+i*0x1300+quad*0x4C0,gabi::load<u32>(packet+0x3768),0x100501F0);
 }
 gabi::Local<WeatherTextureDescriptor> descriptor;
 gabi::call<void>(0x027BE6B8,descriptor.a);
 u32 resource=gabi::load<u32>(packet+0x98);
 gabi::call<void>(0x0274FBF8,gabi::load<u32>(0x101F8B18));
 gabi::call<void>(0x02773798,descriptor.a,resource);
 gabi::call<void>(0x0274FCCC,gabi::load<u32>(0x101F8B18));
 u32 renderer=packet+0x92B6E0;
 gabi::call<void>(0x027FE500,renderer+0xB4,(u32)0,descriptor.a,(u32)0);
 u32 textureState=gabi::call<u32>(0x027FE640,renderer+0xB4,(u32)0);
 u8 flags=gabi::load<u8>(textureState+0x190);
 gabi::store<u32>(textureState+0x15C,1);gabi::store<u32>(textureState+0x160,1);
 gabi::store<u32>(textureState+0x164,1);gabi::store<u8>(textureState+0x190,flags|2);
 gabi::call<void>(0x027FB5D4,packet+0x5CFD70,(u32)0);
 gabi::call<void>(0x027FD838,renderer,(u32)1,(u32)0);
 gabi::call<void>(0x027FB5D4,renderer+12,(u32)0);
}
VERIFY(0x02580318,setup_large_precipitation_geometry);
static void setup_housi_geometry(u32 packet) {
 WWHD_FUNC(0x02579E4C,void,packet);
 gabi::store<u16>(packet+0x988,0);
 gabi::store<float>(packet+0x98C,gabi::load<float>(0x10050020));
 gabi::store<u32>(packet+0x990,lookup_weather_material(packet,0x1005008C));
 for(u32 i=0;i<30;++i) {
  u32 material=gabi::load<u32>(packet+0x990);
  prepare_weather_pair(packet+0x994+i*0x4C0,material,0x10050208);
 }
 gabi::Local<WeatherTextureDescriptor> texture;
 gabi::call<void>(0x027BE6B8,texture.a);
 u32 resource=gabi::load<u32>(packet+0x98);
 gabi::call<void>(0x0274FBF8,gabi::load<u32>(0x101F8B18));
 gabi::call<void>(0x02773798,texture.a,resource);
 gabi::call<void>(0x0274FCCC,gabi::load<u32>(0x101F8B18));
 gabi::call<void>(0x027FE500,packet+0xFFEC,(u32)0,texture.a,(u32)0);
 u32 target=gabi::call<u32>(0x027FE640,packet+0xFFEC,(u32)0);
 u8 flags=gabi::load<u8>(target+0x190);
 gabi::store<u32>(target+0x15C,1);gabi::store<u32>(target+0x164,1);
 gabi::store<u8>(target+0x190,flags|2);gabi::store<u32>(target+0x160,1);
 for(u32 i=0;i<30;++i)gabi::call<void>(0x027FB5D4,packet+0x9818+i*0x370,(u32)0);
 gabi::call<void>(0x027FD838,packet+0xFF38,(u32)1,(u32)0);
 gabi::call<void>(0x027FB5D4,packet+0xFF44,(u32)0);
}
VERIFY(0x02579E4C,setup_housi_geometry);
static void setup_mud_geometry(u32 packet) {
 WWHD_FUNC(0x02579A68,void,packet);
 gabi::store<u32>(packet+0x60C8,lookup_weather_material(packet,0x10050080));
 for(u32 i=0;i<600;++i)prepare_weather_pair(packet+0x60CC+i*0x4C0,gabi::load<u32>(packet+0x60C8),0x10050200);
 gabi::Local<WeatherTextureDescriptor> texture;
 gabi::call<void>(0x027BE6B8,texture.a);
 u32 resource=gabi::load<u32>(packet+0x98);
 gabi::call<void>(0x0274FBF8,gabi::load<u32>(0x101F8B18));
 gabi::call<void>(0x02773798,texture.a,resource);
 gabi::call<void>(0x0274FCCC,gabi::load<u32>(0x101F8B18));
 gabi::call<void>(0x027FE500,packet+0x139204,(u32)0,texture.a,(u32)0);
 u32 target=gabi::call<u32>(0x027FE640,packet+0x139204,(u32)0);
 u8 flags=gabi::load<u8>(target+0x190);
 gabi::store<u32>(target+0x164,1);gabi::store<u32>(target+0x15C,1);
 gabi::store<u8>(target+0x190,flags|2);gabi::store<u32>(target+0x160,1);
 for(u32 i=0;i<600;++i)gabi::call<void>(0x027FB5D4,packet+0xB82D0+i*0x370,(u32)0);
 gabi::call<void>(0x027FD838,packet+0x139150,(u32)1,(u32)0);
 gabi::call<void>(0x027FB5D4,packet+0x13915C,(u32)0);
}
VERIFY(0x02579A68,setup_mud_geometry);
static void prepare_weather_element(u32 element,u32 attributes,u32 previous,u32 material,u32 vertexCount=4,u32 allocationSize=0x50) {
 u32 data=gabi::load<u32>(element);
 if(!data) {
  u32 allocator=gabi::call<u32>(0x02756140,gabi::load<u32>(0x101F8B4C),data);
  u32 table=gabi::load<u32>(allocator+12);
  u32 allocation=gabi::call_ptr<u32>(gabi::load<u32>(table+0x34),allocator,allocationSize,(u32)0x40);
  if(allocation) {gabi::store<u32>(element+0x250,allocation);gabi::store<u32>(element+0x24C,vertexCount);}
  data=gabi::load<u32>(element+0x250);gabi::store<u32>(element,data);
 }
 gabi::call<void>(0x027FF478,element+4,data,vertexCount,attributes);
 if(material&&material!=gabi::load<u32>(previous))gabi::call<void>(0x027FF530,material,element+0x158,element+4,attributes,(u32)0);
}
static void setup_precipitation_geometry(u32 packet) {
 WWHD_FUNC(0x0257A0E8,void,packet);
 gabi::store<u32>(packet+0x11D8,lookup_weather_material(packet,0x1005009C));
 for(u32 group=0;group<2;++group) {
  u32 geometry=packet+0x11DC+group*0x574F8,attributes=geometry+0x574E4;
  gabi::store<u32>(attributes,17);gabi::store<u32>(attributes+8,0x10050210);
  u32 material=gabi::load<u32>(packet+0x11D8);
  for(u32 side=0;side<2;++side)
   for(u32 i=0;i<300;++i)prepare_weather_element(geometry+side*0x254+i*0x4A8,attributes,attributes+12,material);
  gabi::store<u32>(attributes+12,material);gabi::store<u8>(attributes+16,1);
 }
 gabi::call<void>(0x027FE084,packet+0xAFBCC,(u32)1,(u32)0);
 for(u32 i=0;i<100;++i)gabi::call<void>(0x027FB5D4,packet+0xAFFE8+i*0x370,(u32)0);
 gabi::store<float>(packet+0x11D4,gabi::load<float>(0x10050020));
}
VERIFY(0x0257A0E8,setup_precipitation_geometry);
static void setup_snow_geometry(u32 packet) {
 WWHD_FUNC(0x025796F0,void,packet);
 gabi::store<u32>(packet+0x8A4,lookup_weather_material(packet,0x10050058));
 for(u32 i=0;i<2000;++i) {
  u32 geometry=packet+0x8A8+i*0x968;
  gabi::store<u32>(geometry+0x95C,0x10050220);
  gabi::store<u32>(geometry+0x954,1);
  u32 material=gabi::load<u32>(packet+0x8A4);
  for(u32 side=0;side<2;++side)
   for(u32 half=0;half<2;++half)prepare_weather_element(geometry+side*0x254+half*0x4A8,geometry+0x954,geometry+0x960,material,3,0x24);
  gabi::store<u32>(geometry+0x960,material);gabi::store<u8>(geometry+0x964,1);
 }
 gabi::Local<ColorFloat> color;
 for(u32 i=0;i<8;++i) {
  u32 renderer=packet+0x498528+i*0x41C;
  gabi::call<void>(0x027FE084,renderer,(u32)1,(u32)0);
  gabi::call<void>(0x0257963C,color.a,(u32)(0x10050060+4*i));
  for(u32 j=0;j<4;++j)gabi::store<u32>(renderer+0x168+4*j,gabi::load<u32>(color.a+4*j));
 }
}
VERIFY(0x025796F0,setup_snow_geometry);
static void setup_wave_geometry(u32 packet) {
 WWHD_FUNC(0x0257AA4C,void,packet);
 gabi::store<u32>(packet+0x4248,lookup_weather_material(packet,0x100500A8));
 for(u32 i=0;i<300;++i)prepare_weather_pair(packet+0x424C+i*0x4C0,gabi::load<u32>(packet+0x4248),0x10050218);
 gabi::Local<WeatherTextureDescriptor> texture;
 gabi::call<void>(0x027BE6B8,texture.a);
 u32 resource=gabi::load<u32>(packet+0x98);
 gabi::call<void>(0x0274FBF8,gabi::load<u32>(0x101F8B18));
 gabi::call<void>(0x02773798,texture.a,resource);
 gabi::call<void>(0x0274FCCC,gabi::load<u32>(0x101F8B18));
 gabi::call<void>(0x027FE500,packet+0x9DB44,(u32)0,texture.a,(u32)0);
 for(u32 i=0;i<300;++i)gabi::call<void>(0x027FB5D4,packet+0x5D350+i*0x370,(u32)0);
 gabi::call<void>(0x027FD838,packet+0x9DA90,(u32)1,(u32)0);
 gabi::call<void>(0x027FB5D4,packet+0x9DA9C,(u32)0);
}
VERIFY(0x0257AA4C,setup_wave_geometry);
static void setup_cloud_geometry(u32 packet) {
 WWHD_FUNC(0x02578E60,void,packet);
 u32 first=lookup_weather_material(packet,0x1005003C);
 gabi::store<u32>(packet+0x51D0,first);
 u32 material=lookup_weather_material(first,0x10050030);
 gabi::store<u32>(packet+0x12C,material);
 for(u32 i=0;i<9;++i) {
  u32 pair=packet+0x130+i*0x4C0;
  for(u32 side=0;side<2;++side)prepare_weather_element(pair+side*0x254,pair+0x4AC,pair+0x4B8,material,4,0x260);
  gabi::store<u32>(pair+0x4B8,material);gabi::store<u8>(pair+0x4BC,1);
  gabi::call<void>(0x027FE084,packet+0x2BF0+i*0x41C,(u32)1,(u32)0);
  if(i!=8)material=gabi::load<u32>(packet+0x12C);
 }
 gabi::Local<WeatherTextureDescriptor> texture;
 gabi::call<void>(0x027BE6B8,texture.a);
 for(u32 i=0;i<3;++i) {
  u32 manager=gabi::load<u32>(0x101F8B18);
  u32 resource=gabi::load<u32>(packet+0x98+4*i);
  gabi::call<void>(0x0274FBF8,manager);
  gabi::call<void>(0x02773798,texture.a,resource);
  gabi::call<void>(0x0274FCCC,gabi::load<u32>(0x101F8B18));
  u32 renderer=packet+0x50EC+i*0x4C;
  gabi::call<void>(0x027FE500,renderer,(u32)0,texture.a,(u32)0);
  if(i) {
   u32 target=gabi::call<u32>(0x027FE640,renderer,(u32)0);
   u8 flags=gabi::load<u8>(target+0x190);
   gabi::store<u32>(target+0x160,1);gabi::store<u32>(target+0x15C,1);
   gabi::store<u8>(target+0x190,flags|2);gabi::store<u32>(target+0x164,1);
  }
 }
 material=gabi::load<u32>(packet+0x51D0);
 u32 geometry=packet+0x51D4;
 for(u32 side=0;side<2;++side)
  for(u32 i=0;i<16;++i)prepare_weather_element(geometry+side*0x254+i*0x4A8,geometry+0x4A84,geometry+0x4A90,material,3,0x1C8);
 gabi::store<u32>(geometry+0x4A90,material);gabi::store<u8>(geometry+0x4A94,1);
 gabi::call<void>(0x027FE084,packet+0x9C6C,(u32)1,(u32)0);
}
VERIFY(0x02578E60,setup_cloud_geometry);
static void setup_sun_geometry(u32 packet,u32 alternateTexture) {
 WWHD_FUNC(0x02578BC0,void,packet,alternateTexture);
 u32 material=lookup_weather_material(packet,0x1005002C);
 gabi::store<u32>(packet+0xEC,material);
 for(u32 i=0;i<4;++i) {
  u32 pair=packet+0xF0+i*0x4C0;
  for(u32 side=0;side<2;++side)prepare_weather_element(pair+side*0x254,pair+0x4AC,pair+0x4B8,material,4,0x260);
  gabi::store<u32>(pair+0x4B8,material);gabi::store<u8>(pair+0x4BC,1);
  gabi::call<void>(0x027FE084,packet+0x13F0+i*0x41C,(u32)1,(u32)0);
  if(i!=3)material=gabi::load<u32>(packet+0xEC);
 }
 for(u32 i=0;i<6;++i) {
  u32 renderer=packet+0x2460+i*0x4C;
  gabi::call<void>(0x027FE458,renderer);
  gabi::Local<WeatherTextureDescriptor> texture;
  gabi::call<void>(0x027BE6B8,texture.a);
  u32 manager=gabi::load<u32>(0x101F8B18);
  u32 resource=i==5?alternateTexture:gabi::load<u32>(packet+0xD4+4*i);
  gabi::call<void>(0x0274FBF8,manager);
  gabi::call<void>(0x02773798,texture.a,resource);
  gabi::call<void>(0x0274FCCC,gabi::load<u32>(0x101F8B18));
  gabi::call<void>(0x027FE500,renderer,(u32)0,texture.a,(u32)0);
 }
 gabi::store<u32>(packet+0xE8,0);
}
VERIFY(0x02578BC0,setup_sun_geometry);
static bool weather_stage_equals(u32 name) {
 gabi::Local<SafeString> expected,actual;
 gabi::store<u32>(expected.a,name);gabi::store<u32>(expected.a+4,0x1004FDD0);
 u32 game=gabi::call<u32>(0x025200D4);
 gabi::store<u32>(actual.a,game+0x5134);gabi::store<u32>(actual.a+4,0x1004FDD0);
 string_prepare(expected.a);string_prepare(expected.a);
 u32 first=gabi::load<u32>(expected.a);
 string_prepare(actual.a);
 u32 second=gabi::load<u32>(actual.a);
 if(first==second)return true;
 return same_string_bytes(gabi::load<u32>(expected.a),second);
}
static void weather_draw() {
 WWHD_FUNC(0x0257CE00,void);
 if(!weather_stage_equals(0x10050138)) {
  u32 environment=gabi::call<u32>(0x02555D0C);
  if(gabi::load<u8>(environment+0xA64))gabi::call<void>(0x0257790C,(u32)1);
  environment=gabi::call<u32>(0x02555D0C);
  if(gabi::load<u8>(environment+0xA98)==1)gabi::call<void>(0x02577A80,(u32)1);
  environment=gabi::call<u32>(0x02555D0C);
  if(gabi::load<u8>(environment+0xA7C)==1)gabi::call<void>(0x025779AC,(u32)1);
 }
 if(!weather_stage_equals(0x10050138)) {
  u32 environment=gabi::call<u32>(0x02555D0C);
  if(gabi::load<u8>(environment+0xA30)) {
   gabi::call<void>(0x025777F0,(u32)1);gabi::call<void>(0x02577828,(u32)1);
  }
 }
 u32 environment=gabi::call<u32>(0x02555D0C);
 if(gabi::load<u8>(environment+0xA54)==1)gabi::call<void>(0x025778D4,(u32)1);
 if(weather_stage_equals(0x10050138))return;
 environment=gabi::call<u32>(0x02555D0C);
 if(gabi::load<u8>(environment+0xA3C)==1)gabi::call<void>(0x02577864,(u32)1);
 environment=gabi::call<u32>(0x02555D0C);
 if(gabi::load<u8>(environment+0xA48))gabi::call<void>(0x0257789C,(u32)1);
 environment=gabi::call<u32>(0x02555D0C);
 if(gabi::load<u8>(environment+0xA70)==1)gabi::call<void>(0x02577974,(u32)1);
}
VERIFY(0x0257CE00,weather_draw);
static u32 snow_packet_constructor(u32 object) {
 WWHD_FUNC(0x02580ED8,u32,object);
 if(!object) {object=gabi::call<u32>(0x0273AD10,(u32)0x49ADD8);if(!object)return 0;}
 gabi::call<void>(0x027F1278,object);gabi::store<u32>(object+12,0x100502E4);
 gabi::call<void>(0x028F521C,object+0x98,(u32)4);
 gabi::call<void>(0x028F521C,object+0x9C,(u32)0x30);
 gabi::store<u16>(object+0xCC,0);gabi::store<u32>(object+0xD0,0);
 gabi::call<void>(0x028F521C,object+0xD4,(u32)2000);
 gabi::store<u32>(object+0x8A4,0);
 gabi::call<void>(0x028EFFD0,object+0x8A8,(u32)2000,(u32)0x968,(u32)0x02584660);
 gabi::call<void>(0x028EFFD0,object+0x498528,(u32)8,(u32)0x41C,(u32)0x02582E0C);
 gabi::call<void>(0x028F521C,object+0x49A608,(u32)2000);
 return object;
}
VERIFY(0x02580ED8,snow_packet_constructor);
static u32 smoke_packet_constructor(u32 object) {
 WWHD_FUNC(0x025812EC,u32,object);
 if(!object) {object=gabi::call<u32>(0x0273AD10,(u32)0x20BCA8);if(!object)return 0;}
 gabi::call<void>(0x027F1278,object);gabi::store<u32>(object+12,0x10050314);
 gabi::call<void>(0x028F521C,object+0x98,(u32)0xBB80);
 u32 state=object+0xBC30;
 gabi::store<u32>(state,0);gabi::call<void>(0x028F521C,state+4,(u32)4);
 gabi::store<float>(state+8,gabi::load<float>(0x10050020));
 gabi::call<void>(0x028F521C,state+12,(u32)1000);
 gabi::store<u32>(state+0x3F4,0);
 gabi::call<void>(0x028EFFD0,state+0x3F8,(u32)1000,(u32)0x4C0,(u32)0x025848D8);
 gabi::call<void>(0x028EFFD0,object+0x134E28,(u32)1000,(u32)0x370,(u32)0x02583210);
 u32 renderer=object+0x20BBA8;
 gabi::call<void>(0x027FD6F4,renderer);gabi::call<void>(0x027FB40C,renderer+12);
 gabi::store<u32>(renderer+24,0x1016EF84);
 gabi::call<void>(0x028F521C,renderer+0x80,(u32)0x34);
 if(!(renderer+0x80))gabi::call<u32>(0x0273AD10,(u32)0x30);
 gabi::call<void>(0x027FE344,renderer+0xB4);
 return object;
}
VERIFY(0x025812EC,smoke_packet_constructor);
static void draw_snow_color(u32 packet,u32 colorIndex) {
 WWHD_FUNC(0x02581060,void,packet,colorIndex);
 gabi::call<void>(0x027FE118,packet+0x498528+colorIndex*0x41C,gabi::load<u32>(packet+0x8A4),(u32)0);
 s32 count=(s16)gabi::load<u16>(packet+0xCC);
 for(u32 i=0;(s32)i<count;++i) {
  if((u32)(s32)(s8)gabi::load<u8>(packet+0x49A608+i)==colorIndex&&gabi::load<u8>(packet+0xD4+i)) {
   u32 geometry=packet+0x8A8+i*0x968;
   u32 side=gabi::load<u32>(geometry+0x950)==0?1:0;
   u32 element=geometry+side*0x254;
   for(u32 half=0;half<2;++half) {
    if(gabi::load<u32>(geometry+0x960))gabi::call<void>(0x027BFE5C,element+0x158);
    gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4A88),(u32)3,gabi::load<u32>(0x104B4A84),gabi::load<u32>(0x104B4A8C),(u32)0,(u32)1);
    element+=0x4A8;
   }
   count=(s16)gabi::load<u16>(packet+0xCC);
  }
 }
}
VERIFY(0x02581060,draw_snow_color);
struct WeatherRenderState { u8 bytes[0x11C]; };
struct WeatherMatrix { gabi::be<float> values[12]; };
static u32 bind_weather_shader(u32 materialField) {
 u32 cache=gabi::call<u32>(0x027F29D4,(u32)0x104B45C0);
 u32 material=gabi::load<u32>(materialField);
 u32 shader=gabi::load<u32>(material);
 if(shader==gabi::load<u32>(cache+4))return material;
 u8 flags=gabi::load<u8>(shader);u32 oldProgram=gabi::load<u32>(cache);
 if(flags&2) {gabi::store<u8>(shader,flags&~2u);gabi::call<void>(0x027BB9E0,shader,(u32)0);}
 u32 handle=gabi::load<u32>(shader+0x7C);u32 program=gabi::load<u32>(handle+0x28);
 if(oldProgram!=program)gabi::call<void>(0x027B9F68,program);
 u32 size=gabi::load<u32>(shader+12);
 if(size) {
  gabi::call<void>(0xC00060E0,gabi::load<u32>(shader+4),size);
  gabi::store<u32>(cache,program);gabi::store<u32>(cache+4,shader);
 } else {
  gabi::call<void>(0x027BB7CC,shader);
  gabi::store<u32>(cache+4,shader);gabi::store<u32>(cache,program);
 }
 return gabi::load<u32>(materialField);
}
static void draw_snow_packet(u32 packet) {
 WWHD_FUNC(0x0258115C,void,packet);
 gabi::Local<WeatherRenderState> state;
 gabi::call<void>(0x02750250,state.a);
 gabi::store<float>(state.a+0xE8,gabi::load<float>(0x10050020));
 gabi::store<u32>(state.a+0xE4,4);gabi::store<u32>(state.a+8,2);
 u32 bits=gabi::load<u32>(state.a+0xEC);
 gabi::store<u8>(state.a+1,0);gabi::store<u8>(state.a+0xE0,1);
 gabi::store<u32>(state.a+12,3);
 gabi::store<u32>(state.a+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
 gabi::call<void>(0x02750370,state.a);
 gabi::Local<WeatherMatrix> matrix;
 for(u32 i=0;i<8;++i) {
  gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
  u32 renderer=packet+0x498528+i*0x41C;
  u32 camera=gabi::load<u32>(0x104B4708);
  gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,(u32)0x104B470C,camera+0x240);
  gabi::call<void>(0x027FE0DC,renderer,(u32)0);
 }
 bind_weather_shader(packet+0x8A4);
 for(u32 i=0;i<8;++i)gabi::call<void>(0x02581060,packet,i);
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x0258115C,draw_snow_packet);
static void setup_smoke_geometry(u32 packet) {
 WWHD_FUNC(0x02581518,void,packet);
 gabi::store<u32>(packet+0xC024,lookup_weather_material(packet,0x100501D4));
 for(u32 i=0;i<1000;++i) {
  u32 material=gabi::load<u32>(packet+0xC024);
  prepare_weather_pair(packet+0xC028+i*0x4C0,material,0x100501F8);
 }
 gabi::Local<WeatherTextureDescriptor> texture;
 gabi::call<void>(0x027BE6B8,texture.a);
 u32 resource=gabi::load<u32>(packet+0xBC34);
 gabi::call<void>(0x0274FBF8,gabi::load<u32>(0x101F8B18));
 gabi::call<void>(0x02773798,texture.a,resource);
 gabi::call<void>(0x0274FCCC,gabi::load<u32>(0x101F8B18));
 gabi::call<void>(0x027FE500,packet+0x20BC5C,(u32)0,texture.a,(u32)0);
 u32 target=gabi::call<u32>(0x027FE640,packet+0x20BC5C,(u32)0);
 u8 flags=gabi::load<u8>(target+0x190);
 gabi::store<u32>(target+0x160,1);gabi::store<u32>(target+0x15C,1);
 gabi::store<u32>(target+0x164,1);gabi::store<u8>(target+0x190,flags|2);
 for(u32 i=0;i<1000;++i)gabi::call<void>(0x027FB5D4,packet+0x134E28+i*0x370,(u32)0);
 gabi::call<void>(0x027FD838,packet+0x20BBA8,(u32)1,(u32)0);
 gabi::call<void>(0x027FB5D4,packet+0x20BBB4,(u32)0);
}
VERIFY(0x02581518,setup_smoke_geometry);
struct WeatherProjection { gabi::be<float> values[16]; };
static void bind_weather_texture(u32 texture,u32 material) {
 u32 table=gabi::load<u32>(texture+12);
 gabi::call_ptr<void>(gabi::load<u32>(table+0x2C),texture,material);
}
static void bind_weather_uniforms(u32 renderer,u32 materialField) {
 u32 info=gabi::load<u32>(renderer+4);
 u32 slot=gabi::load<u32>(info+0x4C);
 u32 uniform=info+0x10+slot*28;
 u32 material=gabi::load<u32>(materialField);
 u32 descriptor=gabi::load<u32>(material+12)?gabi::load<u32>(material+16):0;
 s32 vertex=(s16)gabi::load<u16>(descriptor+12);
 u32 size=gabi::load<u32>(uniform+12);
 s32 pixel=(s16)gabi::load<u16>(descriptor+14);
 u32 data=gabi::load<u32>(uniform+4);
 s32 geometry=(s16)gabi::load<u16>(descriptor+16);
 if(pixel!=-1)gabi::call<void>(0xC0006900,(u32)pixel,size,data);
 if(vertex!=-1)gabi::call<void>(0xC0006A38,(u32)vertex,size,data);
 if(geometry!=-1)gabi::call<void>(0xC00068A8,(u32)geometry,size,data);
}
static void draw_smoke_packet(u32 packet) {
 WWHD_FUNC(0x025817B0,void,packet);
 gabi::Local<WeatherRenderState> state;
 gabi::call<void>(0x02750250,state.a);
 u32 bits=gabi::load<u32>(state.a+0xEC);
 gabi::store<u32>(state.a+8,2);
 gabi::store<float>(state.a+0xE8,gabi::load<float>(0x10050020));
 gabi::store<u32>(state.a+0xE4,4);gabi::store<u8>(state.a+1,0);
 gabi::store<u32>(state.a+12,3);gabi::store<u8>(state.a+0xE0,1);
 gabi::store<u32>(state.a+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
 gabi::Local<WeatherMatrix> matrix;gabi::Local<WeatherProjection> projection;
 gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
 for(u32 i=0;i<16;++i)gabi::store<u32>(projection.a+4*i,gabi::load<u32>(0x104B470C+4*i));
 u32 renderer=packet+0x20BBA8;
 gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,projection.a,gabi::load<u32>(0x104B4708)+0x240);
 gabi::call<void>(0x027FE010,renderer);
 u32 texture=renderer+12;
 gabi::call<void>(0x027FB678,texture);
 s32 count=(s32)gabi::load<u32>(environment()+0xA68);
 for(u32 i=0;(s32)i<count;++i) {
  if(gabi::load<u8>(packet+0xBC3C+i))gabi::call<void>(0x027FB678,packet+0x134E28+i*0x370);
  count=(s32)gabi::load<u32>(environment()+0xA68);
 }
 u32 material=bind_weather_shader(packet+0xC024);
 gabi::call<void>(0x027FE5B8,renderer+0xB4,material);
 bind_weather_texture(texture,gabi::load<u32>(packet+0xC024));
 bind_weather_uniforms(renderer,packet+0xC024);
 gabi::call<void>(0x02750370,state.a);
 count=(s32)gabi::load<u32>(environment()+0xA68);
 for(u32 i=0;(s32)i<count;++i) {
  if(gabi::load<u8>(packet+0xBC3C+i)) {
   bind_weather_texture(packet+0x134E28+i*0x370,gabi::load<u32>(packet+0xC024));
   u32 pair=packet+0xC028+i*0x4C0;
   u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
   if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
   gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
  }
  count=(s32)gabi::load<u32>(environment()+0xA68);
 }
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x025817B0,draw_smoke_packet);
static void draw_housi_packet(u32 packet,s32 count) {
 WWHD_FUNC(0x025820F4,void,packet,count);
 gabi::Local<WeatherRenderState> state;
 gabi::call<void>(0x02750250,state.a);
 u32 bits=gabi::load<u32>(state.a+0xEC);
 gabi::store<u8>(state.a+0xE0,1);gabi::store<u32>(state.a+8,1);
 gabi::store<u32>(state.a+0xE4,4);gabi::store<u32>(state.a+4,3);
 gabi::store<u32>(state.a+12,3);gabi::store<float>(state.a+0xE8,gabi::load<float>(0x10050020));
 gabi::store<u32>(state.a+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
 gabi::store<u8>(state.a+1,0);gabi::store<u8>(state.a,0);
 gabi::call<void>(0x02750370,state.a);
 gabi::Local<WeatherMatrix> matrix;gabi::Local<WeatherProjection> projection;
 gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
 for(u32 i=0;i<16;++i)gabi::store<u32>(projection.a+4*i,gabi::load<u32>(0x104B470C+4*i));
 u32 renderer=packet+0xFF38,texture=renderer+12;
 gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,projection.a,gabi::load<u32>(0x104B4708)+0x240);
 gabi::call<void>(0x027FE010,renderer);gabi::call<void>(0x027FB678,texture);
 u32 material=bind_weather_shader(packet+0x990);
 gabi::call<void>(0x027FE5B8,renderer+0xB4,material);
 bind_weather_texture(texture,gabi::load<u32>(packet+0x990));
 bind_weather_uniforms(renderer,packet+0x990);
 if(count>0)for(u32 i=0;i<(u32)count;++i) {
  u32 particle=packet+0x9818+i*0x370;
  gabi::call<void>(0x027FB678,particle);
  bind_weather_texture(particle,gabi::load<u32>(packet+0x990));
  u32 pair=packet+0x994+i*0x4C0;
  u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
  if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
  gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
 }
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x025820F4,draw_housi_packet);
static void draw_mud_packet(u32 packet) {
 WWHD_FUNC(0x02581C34,void,packet);
 gabi::Local<WeatherRenderState> state;
 gabi::call<void>(0x02750250,state.a);
 u32 bits=gabi::load<u32>(state.a+0xEC);
 gabi::store<u32>(state.a+8,2);
 gabi::store<float>(state.a+0xE8,gabi::load<float>(0x10050020));
 gabi::store<u32>(state.a+0xE4,4);gabi::store<u8>(state.a+1,0);
 gabi::store<u32>(state.a+12,3);gabi::store<u8>(state.a+0xE0,1);
 gabi::store<u32>(state.a+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
 gabi::Local<WeatherMatrix> matrix;gabi::Local<WeatherProjection> projection;
 gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
 for(u32 i=0;i<16;++i)gabi::store<u32>(projection.a+4*i,gabi::load<u32>(0x104B470C+4*i));
 u32 renderer=packet+0x139150;
 gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,projection.a,gabi::load<u32>(0x104B4708)+0x240);
 gabi::call<void>(0x027FE010,renderer);
 u32 texture=renderer+12;
 gabi::call<void>(0x027FB678,texture);
 s32 count;
 for(u32 group=0;group<2;++group) {
  count=(s32)gabi::load<u32>(environment()+0xA74);
  for(u32 i=0;(s32)i<count;++i) {
   u32 index=group*300+i;
   if(gabi::load<u8>(packet+0x5E70+index))gabi::call<void>(0x027FB678,packet+0xB82D0+index*0x370);
   count=(s32)gabi::load<u32>(environment()+0xA74);
  }
 }
 u32 material=bind_weather_shader(packet+0x60C8);
 gabi::call<void>(0x027FE5B8,renderer+0xB4,material);
 bind_weather_texture(texture,gabi::load<u32>(packet+0x60C8));
 bind_weather_uniforms(renderer,packet+0x60C8);
 for(u32 group=0;group<2;++group) {
  gabi::store<u32>(state.a+4,group?6:3);
  gabi::call<void>(0x02750370,state.a);
  count=(s32)gabi::load<u32>(environment()+0xA74);
  for(u32 i=0;(s32)i<count;++i) {
   u32 index=group*300+i;
   if(gabi::load<u8>(packet+0x5E70+index)) {
    bind_weather_texture(packet+0xB82D0+index*0x370,gabi::load<u32>(packet+0x60C8));
    u32 pair=packet+0x60CC+index*0x4C0;
    u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
    if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
    gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
   }
   count=(s32)gabi::load<u32>(environment()+0xA74);
  }
 }
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x02581C34,draw_mud_packet);
static void emit_weather_uniforms(u32 info,u32 descriptor) {
 u32 uniform=info+0x10+gabi::load<u32>(info+0x4C)*28;
 s32 vertex=(s16)gabi::load<u16>(descriptor+12);
 u32 data=gabi::load<u32>(uniform+4);
 s32 pixel=(s16)gabi::load<u16>(descriptor+14);
 u32 size=gabi::load<u32>(uniform+12);
 s32 geometry=(s16)gabi::load<u16>(descriptor+16);
 if(pixel!=-1)gabi::call<void>(0xC0006900,(u32)pixel,size,data);
 if(vertex!=-1)gabi::call<void>(0xC0006A38,(u32)vertex,size,data);
 if(geometry!=-1)gabi::call<void>(0xC00068A8,(u32)geometry,size,data);
}
static void draw_wave_packet(u32 packet,u32 context) {
 WWHD_FUNC(0x02582650,void,packet,context);
 gabi::Local<WeatherMatrix> matrix;gabi::Local<WeatherProjection> projection;
 gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
 for(u32 i=0;i<16;++i)gabi::store<u32>(projection.a+4*i,gabi::load<u32>(0x104B470C+4*i));
 u32 renderer=packet+0x9DA90,texture=renderer+12;
 gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,projection.a,gabi::load<u32>(0x104B4708)+0x240);
 gabi::call<void>(0x027FE010,renderer);gabi::call<void>(0x027FB678,texture);
 u32 material=bind_weather_shader(packet+0x4248);
 gabi::call<void>(0x027FE5B8,renderer+0xB4,material);
 bind_weather_texture(texture,gabi::load<u32>(packet+0x4248));
 bind_weather_uniforms(renderer,packet+0x4248);
 gabi::Local<WeatherRenderState> state;
 gabi::call<void>(0x02750250,state.a);
 u32 bits=gabi::load<u32>(state.a+0xEC);
 gabi::store<u32>(state.a+0xE4,4);gabi::store<u8>(state.a+0xE0,1);
 gabi::store<u32>(state.a+8,2);gabi::store<u32>(state.a+12,3);
 gabi::store<float>(state.a+0xE8,gabi::load<float>(0x100500F8));
 gabi::store<u32>(state.a+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
 if(gabi::load<u32>(context+12)==2) {
  u32 info=gabi::load<u32>(context+0x30);
  if(info) {
   u32 material=gabi::load<u32>(packet+0x4248);
   u32 descriptor=gabi::load<u32>(material+12)>4?gabi::load<u32>(material+16)+0x50:0;
   emit_weather_uniforms(info,descriptor);
  }
  gabi::call<void>(0x027FFE54,context,gabi::load<u32>(packet+0x4248));
 }
 gabi::call<void>(0x02750370,state.a);
 s32 count=(s16)gabi::load<u16>(environment()+0x9F8);
 for(u32 i=0;(s32)i<count;++i) {
  if(gabi::load<u8>(packet+0x9DB90+i)) {
   u32 particle=packet+0x5D350+i*0x370;
   gabi::call<void>(0x027FB678,particle);bind_weather_texture(particle,gabi::load<u32>(packet+0x4248));
   u32 pair=packet+0x424C+i*0x4C0;
   u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
   if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
   gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
  }
  count=(s16)gabi::load<u16>(environment()+0x9F8);
 }
 gabi::call<void>(0x0255F84C);
}
VERIFY(0x02582650,draw_wave_packet);
static void housi_proc() {
 WWHD_FUNC(0x02583678,void);
 u8 phase=gabi::load<u8>(environment()+0xA7C);
 if(phase==0) {
  if(!gabi::load<u32>(environment()+0xA80))return;
  gabi::Local<gabi::be<u32>> heapScope;
  gabi::call<void>(0x025F01D8,heapScope.a,(u32)0x1004FF70,(u32)0x27510,(u32)0);
  u32 packet=gabi::call<u32>(0x0273AE48,(u32)0x10038,(u32)32);
  if(packet)packet=gabi::call<u32>(0x02579D2C,packet);
  gabi::store<u32>(environment()+0xA84,packet);
  packet=gabi::load<u32>(environment()+0xA84);
  if(packet) {
   gabi::Local<SafeString> package,entry;
   gabi::store<u32>(package.a,0x1004FDA8);gabi::store<u32>(package.a+4,0x1004FDD0);
   gabi::store<u32>(entry.a,0x1004FF90);gabi::store<u32>(entry.a+4,0x1004FDD0);
   u32 resource=gabi::call<u32>(0x026124B0,gabi::load<u32>(0x101F4F7C),package.a,entry.a,(u32)0);
   u32 archive=gabi::call<u32>(0x027E2DC0,resource);
   u32 texture=gabi::call<u32>(0x027DFA24,archive_resource_array(archive),(u32)0x1004FEC8);
   packet=gabi::load<u32>(environment()+0xA84);gabi::store<u32>(packet+0x98,texture);
   gabi::call<void>(0x02579E4C,gabi::load<u32>(environment()+0xA84));
   for(u32 i=0;i<30;++i)gabi::store<u8>(gabi::load<u32>(environment()+0xA84)+0xA0+0x4C*i,0);
   gabi::store<u32>(gabi::load<u32>(environment()+0xA84)+0x9C,0);
   gabi::call<void>(0x0256BB6C);
   u32 env=environment();gabi::store<u8>(env+0xA7C,gabi::load<u8>(env+0xA7C)+1);
  } else gabi::call<void>(0x025F02D0,heapScope.a);
  gabi::call<void>(0x025F0270,heapScope.a,(u32)2);
 } else if(phase==1) {
  bool special=false;
  if(gabi::load<u8>(environment()+0xA7D)==0) {
   float power=gabi::call<float>(0x02578348);
   if(power>gabi::load<float>(0x10050148)&&gabi::load<u32>(environment()+0xA80)) {
    if(weather_stage_equals(0x1004FD40)) {
     if(gabi::load<u8>(environment()+0x109E))special=true;
     else gabi::store<u32>(environment()+0x1058,0x105A);
    }
    if(special)gabi::store<u32>(environment()+0x1058,0x1061);
    gabi::call<void>(0x0256BB6C);
    if(!gabi::load<u32>(environment()+0xA80)&&!gabi::load<u32>(gabi::load<u32>(environment()+0xA84)+0x9C)) {
     u32 env=environment();gabi::store<u8>(env+0xA7C,gabi::load<u8>(env+0xA7C)+1);
    }
    return;
   }
  }
  u8 mode=gabi::load<u8>(environment()+0xA7D);
  if(mode==1)gabi::store<u32>(environment()+0x1058,0x1061);
  else if(gabi::load<u8>(environment()+0xA7D)==2&&weather_stage_equals(0x1004FDB0))gabi::store<u32>(environment()+0x1058,0x1062);
  gabi::call<void>(0x0256BB6C);
  if(!gabi::load<u32>(environment()+0xA80)&&!gabi::load<u32>(gabi::load<u32>(environment()+0xA84)+0x9C)) {
   u32 env=environment();gabi::store<u8>(env+0xA7C,gabi::load<u8>(env+0xA7C)+1);
  }
 } else if(phase==2) {
  u32 env=environment();gabi::store<u8>(env+0xA7C,gabi::load<u8>(env+0xA7C)+1);
 } else if(phase==3) {
  gabi::store<u8>(environment()+0xA7C,0);gabi::call<void>(0x02577F64,environment()+0xA84);
 }
}
VERIFY(0x02583678,housi_proc);
static void draw_rain_splashes(u32 packet) {
 WWHD_FUNC(0x0257F804,void,packet);
 gabi::Local<WeatherRenderState> state;
 gabi::call<void>(0x02750250,state.a);
 u32 bits=gabi::load<u32>(state.a+0xEC);
 gabi::store<u8>(state.a+0xE0,1);gabi::store<u32>(state.a+8,2);
 gabi::store<u32>(state.a+0xE4,4);
 gabi::store<u32>(state.a+12,3);gabi::store<float>(state.a+0xE8,gabi::load<float>(0x10050020));
 gabi::store<u32>(state.a+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
 gabi::store<u8>(state.a+1,0);
 gabi::call<void>(0x02750370,state.a);
 gabi::Local<WeatherMatrix> matrix;gabi::Local<WeatherProjection> projection;
 gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
 for(u32 i=0;i<16;++i)gabi::store<u32>(projection.a+4*i,gabi::load<u32>(0x104B470C+4*i));
 u32 renderer=packet+0x1AC8F0,texture=renderer+12;
 gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,projection.a,gabi::load<u32>(0x104B4708)+0x240);
 gabi::call<void>(0x027FE010,renderer);gabi::call<void>(0x027FB678,texture);
 u32 material=bind_weather_shader(packet+0x3898);
 gabi::call<void>(0x027FE5B8,renderer+0xB4,material);
 bind_weather_texture(texture,gabi::load<u32>(packet+0x3898));
 bind_weather_uniforms(renderer,packet+0x3898);
 s32 count=(s16)gabi::load<u16>(packet+0x3758);
 for(u32 i=0;(s32)i<count;++i) {
  if(gabi::load<u8>(packet+0x379D+i)) {
   u32 particle=packet+0x176A20+i*0x370;
   gabi::call<void>(0x027FB678,particle);
   bind_weather_texture(particle,gabi::load<u32>(packet+0x3898));
   for(u32 quad=0;quad<4;++quad) {
    u32 pair=packet+0x389C+i*0x1300+quad*0x4C0;
    u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
    if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
    gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
   }
   count=(s16)gabi::load<u16>(packet+0x3758);
  }
 }
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x0257F804,draw_rain_splashes);
static void draw_rain_streaks(u32 packet) {
 WWHD_FUNC(0x0257FB4C,void,packet);
 gabi::Local<WeatherRenderState> state;gabi::call<void>(0x02750250,state.a);
 gabi::store<u8>(state.a+1,0);float zero=gabi::load<float>(0x10050020);
 u32 bits=gabi::load<u32>(state.a+0xEC);
 gabi::store<u32>(state.a+8,2);gabi::store<u32>(state.a+4,6);
 gabi::store<u32>(state.a+0xE4,4);gabi::store<u32>(state.a+12,3);
 gabi::store<u32>(state.a+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
 gabi::store<u8>(state.a+0xE0,1);gabi::store<float>(state.a+0xE8,zero);
 gabi::call<void>(0x02750370,state.a);
 gabi::Local<WeatherMatrix> matrix;gabi::Local<WeatherProjection> projection;
 gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
 for(u32 i=0;i<16;++i)gabi::store<u32>(projection.a+4*i,gabi::load<u32>(0x104B470C+4*i));
 u32 renderer=packet+0x1AC8F0,texture=renderer+12,extraTexture=packet+0x1AC580;
 gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,projection.a,gabi::load<u32>(0x104B4708)+0x240);
 gabi::call<void>(0x027FE010,renderer);gabi::call<void>(0x027FB678,texture);
 gabi::call<void>(0x027FB678,extraTexture);
 u32 material=bind_weather_shader(packet+0x3898);
 gabi::call<void>(0x027FE5B8,packet+0x1AC9F0,material);
 bind_weather_texture(texture,gabi::load<u32>(packet+0x3898));
 bind_weather_uniforms(renderer,packet+0x3898);
 bind_weather_texture(extraTexture,gabi::load<u32>(packet+0x3898));
 s32 count=gabi::load<s32>(environment()+0xA40)>>1;
 for(u32 i=0;(s32)i<count;++i) {
  u32 pair=packet+0x12C69C+i*0x4C0;
  u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
  if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
  gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
  count=gabi::load<s32>(environment()+0xA40)>>1;
 }
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x0257FB4C,draw_rain_streaks);
static void draw_large_precipitation_splashes(u32 packet) {
 WWHD_FUNC(0x02580870,void,packet);
 gabi::Local<WeatherRenderState> state;gabi::call<void>(0x02750250,state.a);
 u32 bits=gabi::load<u32>(state.a+0xEC);float zero=gabi::load<float>(0x10050020);
 gabi::store<u32>(state.a+8,2);gabi::store<float>(state.a+0xE8,zero);
 gabi::store<u32>(state.a+0xE4,4);gabi::store<u8>(state.a+1,0);
 gabi::store<u8>(state.a+0xE0,1);gabi::store<u32>(state.a+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
 gabi::store<u32>(state.a+12,3);
 gabi::Local<WeatherMatrix> matrix;gabi::Local<WeatherProjection> projection;
 gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
 for(u32 i=0;i<16;++i)gabi::store<u32>(projection.a+4*i,gabi::load<u32>(0x104B470C+4*i));
 u32 renderer=packet+0x92B6E0,texture=renderer+12,extra=packet+0x5CFD70;
 gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,projection.a,gabi::load<u32>(0x104B4708)+0x240);
 gabi::call<void>(0x027FE010,renderer);gabi::call<void>(0x027FB678,texture);gabi::call<void>(0x027FB678,extra);
 u32 material=bind_weather_shader(packet+0x3768);
 gabi::call<void>(0x027FE5B8,renderer+0xB4,material);
 bind_weather_texture(texture,gabi::load<u32>(packet+0x3768));
 bind_weather_uniforms(renderer,packet+0x3768);bind_weather_texture(extra,gabi::load<u32>(packet+0x3768));
 gabi::call<void>(0x02750370,state.a);
 s32 count=gabi::load<s16>(packet+0x3760);
 for(u32 i=0;(s32)i<count;++i) {
  for(u32 q=0;q<4;++q) {
   u32 pair=packet+0x376C+i*0x1300+q*0x4C0;
   u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
   if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
   gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
  }
  count=gabi::load<s16>(packet+0x3760);
 }
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x02580870,draw_large_precipitation_splashes);
static void draw_large_precipitation_particles(u32 packet,u32 count,u32 blendStart) {
 WWHD_FUNC(0x02580B98,void,packet,count,blendStart);
 gabi::Local<WeatherRenderState> state;gabi::call<void>(0x02750250,state.a);
 u32 bits=gabi::load<u32>(state.a+0xEC);
 gabi::store<u32>(state.a+8,2);gabi::store<u8>(state.a+1,0);
 gabi::store<u32>(state.a+0xE4,4);gabi::store<u32>(state.a+12,3);
 float zero=gabi::load<float>(0x10050020);
 gabi::store<u8>(state.a+0xE0,1);gabi::store<float>(state.a+0xE8,zero);
 gabi::store<u32>(state.a+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
 gabi::store<u32>(state.a+4,3);gabi::call<void>(0x02750370,state.a);
 gabi::Local<WeatherMatrix> matrix;gabi::Local<WeatherProjection> projection;
 gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
 for(u32 i=0;i<16;++i)gabi::store<u32>(projection.a+4*i,gabi::load<u32>(0x104B470C+4*i));
 u32 renderer=packet+0x92B6E0,texture=renderer+12;
 gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,projection.a,gabi::load<u32>(0x104B4708)+0x240);
 gabi::call<void>(0x027FE010,renderer);gabi::call<void>(0x027FB678,texture);
 u32 material=bind_weather_shader(packet+0x3768);gabi::call<void>(0x027FE5B8,renderer+0xB4,material);
 bind_weather_texture(texture,gabi::load<u32>(packet+0x3768));bind_weather_uniforms(renderer,packet+0x3768);
 s32 limit=(s32)count;if(limit>4000)limit=4000;
 for(u32 i=0;(s32)i<limit;++i) {
  if(i==blendStart) {gabi::store<u32>(state.a+4,6);gabi::call<void>(0x02750534,state.a);}
  u32 particle=packet+0x5D00E0+i*0x370;
  gabi::call<void>(0x027FB678,particle);
  u32 table=gabi::load<u32>(particle+12),target=gabi::load<u32>(table+0x2C);
  // Native uses r5/r7 for the vtable and target; resolved callees can read these registers.
  gabi::cpu->r[5]=table;gabi::cpu->r[7]=target;
  gabi::call_ptr<void>(target,particle,gabi::load<u32>(packet+0x3768));
  u32 pair=packet+0x12C56C+i*0x4C0;u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
  if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
  gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
 }
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x02580B98,draw_large_precipitation_particles);
static void update_global_wind() {
 WWHD_FUNC(0x0257D398,void);
 u32 env=environment();float zero=gabi::load<float>(0x10050020),x,y,z,power;
 gabi::Local<Vector> direction;
 if(gabi::load<u32>(env+0xA08)) {
  u32 custom=gabi::load<u32>(environment()+0xA08);
  for(u32 i=0;i<3;++i)gabi::store<u32>(direction.a+4*i,gabi::load<u32>(custom+4*i));
  env=environment();x=gabi::load<float>(direction.a);z=gabi::load<float>(direction.a+8);
  power=gabi::load<float>(env+0xA1C);gabi::call<u32>(0x020195B0,x,z);
  gabi::Local<Vector> horizontal;
  gabi::store<float>(horizontal.a,x);gabi::store<float>(horizontal.a+4,zero);gabi::store<float>(horizontal.a+8,z);
  float squared=gabi::call<float>(0x028E8DD0,horizontal.a);
  float length=gabi::call<float>(0x028F4384,squared);
  y=gabi::load<float>(direction.a+4);gabi::call<u32>(0x020195B0,length,y);
 } else {
  s32 room=gabi::load<s8>(0x1047E6C8);u32 roomWind=0;
  if(room>=0) {
   u32 game=gabi::call<u32>(0x025200D4);
   u32 info=gabi::call<u32>(0x025C11DC,game+0x51CC,(u32)room);
   roomWind=gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(info)+0x1DC),info);
  }
  u32 first=0,second=0;
  if(gabi::load<u8>(environment()+0xA2D)&&gabi::load<u8>(environment()+0xA2D)!=255) {
   first=gabi::load<u16>(environment()+0xA28);second=gabi::load<u16>(environment()+0xA2A);
  } else if(gabi::load<u8>(environment()+0xA2C)) {
   first=gabi::load<u16>(environment()+0xA24);second=gabi::load<u16>(environment()+0xA26);
  }
  u32 a=0x104A44F8+(first&~7u),b=0x104A44F8+(second&~7u);
  float cosine=gabi::load<float>(a+4),otherCosine=gabi::load<float>(b+4);
  y=gabi::load<float>(a);u32 yBits=gabi::load<u32>(a);x=gabi::fmuls_ppc(cosine,otherCosine);
  z=gabi::fmuls_ppc(cosine,gabi::load<float>(b));
  u32 mode=roomWind?(gabi::load<u32>(roomWind)>>18)&3:0;
  power=mode==0?gabi::load<float>(0x1005014C):mode==1?gabi::load<float>(0x10050150):mode==2?gabi::load<float>(0x10050154):zero;
  if(weather_stage_equals(0x10050160)) {y=zero;yBits=gabi::load<u32>(0x10050020);x=gabi::load<float>(0x1005001C);power=gabi::load<float>(0x1005014C);z=x;}
  // y reaches the stack vector by lfs/stfs only (no arithmetic): store its bits, as the native register copy does.
  gabi::store<float>(direction.a,x);gabi::store<u32>(direction.a+4,yBits);gabi::store<float>(direction.a+8,z);
 }
 if(gabi::load<float>(environment()+0xA20)>zero) {
  power=gabi::load<float>(environment()+0xA20);gabi::store<float>(environment()+0xA18,power);
 }
 if(gabi::load<u8>(environment()+0xA2D)==2)power=zero;
 if(weather_stage_equals(0x10050168))power=gabi::load<float>(0x10050150);
 else if(power>gabi::load<float>(0x100500C4))power=gabi::load<float>(0x100500C4);
 if(gabi::load<u8>(environment()+0x109C)) {
  env=environment();for(u32 i=0;i<3;++i)gabi::store<u32>(env+0x9FC+4*i,gabi::load<u32>(direction.a+4*i));
  gabi::store<float>(environment()+0xA18,power);
 } else {
  float damping=gabi::load<float>(0x100500F8),minimum=gabi::load<float>(0x10050144),step=gabi::load<float>(0x10050158);
  gabi::call<void>(0x0200ECD4,environment()+0x9FC,x,damping,step,minimum);
  gabi::call<void>(0x0200ECD4,environment()+0xA00,y,damping,step,minimum);
  gabi::call<void>(0x0200ECD4,environment()+0xA04,z,damping,step,minimum);
  env=environment();float maxStep=gabi::load<float>(0x100500C4),cutoff=gabi::load<float>(0x1005015C);
  gabi::call<void>(0x0200ECD4,env+0xA18,power,damping,maxStep,cutoff);
 }
}
VERIFY(0x0257D398,update_global_wind);
static void move_volumetric_clouds() {
 WWHD_FUNC(0x0257C5B0,void);
 u32 game=gabi::call<u32>(0x025200D4),camera=gabi::load<u32>(game+0x5AF8);
 if(!gabi::load<u32>(0x10477598)) {
  float x=gabi::load<float>(0x10050110);gabi::store<u32>(0x10477598,1);gabi::store<float>(0x104775B0,x);
  float y=gabi::load<float>(0x10050114),z=gabi::load<float>(0x10050118);
  gabi::store<float>(0x104775B4,y);gabi::store<float>(0x104775B8,z);
 }
 if(weather_stage_equals(0x1004FDB8))gabi::store<u32>(environment()+0xA8C,70);
 else {
  game=gabi::call<u32>(0x025200D4);
  if((gabi::load<u16>(game+0x5ACA)&1)&&!gabi::load<u8>(environment()+0x109E)) {
   u32 env=environment();float one=gabi::load<float>(0x100500C4),zero=gabi::load<float>(0x10050020);
   gabi::store<u32>(env+0xA8C,50);
   bool growing=false;
   if(gabi::load<u8>(environment()+0x108D)==1&&gabi::load<float>(environment()+0xFC8)>zero)growing=true;
   else if(gabi::load<u8>(environment()+0x108C)==1&&gabi::load<float>(environment()+0xFC8)<one)growing=true;
   else if(gabi::load<u8>(environment()+0x108D)==2&&gabi::load<float>(environment()+0xFC8)>zero)growing=true;
   else if(gabi::load<u8>(environment()+0x108C)==2&&gabi::load<float>(environment()+0xFC8)<one)growing=true;
   float step=gabi::load<float>(growing?0x1005011C:0x10050128);
   float minimum=gabi::load<float>(growing?0x10050120:0x1005012C);
   float damping=gabi::load<float>(growing?0x100500F8:0x10050124);
   gabi::call<void>(0x0200ECD4,environment()+0xA90,growing?one:zero,damping,step,minimum);
   if(weather_stage_equals(0x1004FD4C)&&gabi::load<s8>(0x1047E6C8)==9) {
    float squared=gabi::call<float>(0x028E8DE8,camera+0xDC,(u32)0x104775B0);
    float distance=gabi::call<float>(0x028F4384,squared);
    if(distance<gabi::load<float>(0x10050130))gabi::store<float>(environment()+0xA90,one);
   }
   float fifty=gabi::load<float>(0x10050134);env=environment();
   float strength=gabi::load<float>(environment()+0xA90);
   gabi::store<s32>(env+0xA8C,(s16)gabi::ftoi(add_product(fifty,strength,fifty)));
   if(weather_stage_equals(0x1004FDC0))gabi::store<u32>(environment()+0xA8C,0);
  } else gabi::store<u32>(environment()+0xA8C,0);
 }
 u8 status=gabi::load<u8>(environment()+0xA88);
 if(status==1) {gabi::call<void>(0x0256DDF8);return;}
 if(status!=0)return;
 if(!gabi::load<u32>(environment()+0xA8C))return;
 gabi::Local<gabi::be<u32>> heapScope;
 gabi::call<void>(0x025F01D8,heapScope.a,(u32)0x1004FED8,(u32)0x18F0D0,(u32)0);
 u32 packet=gabi::call<u32>(0x0273AE48,(u32)0xC5E20,(u32)0x20);
 if(packet)packet=gabi::call<u32>(0x0257A42C,packet);
 gabi::store<u32>(environment()+0xA94,packet);
 if(!gabi::load<u32>(environment()+0xA94)) {
  gabi::call<void>(0x025F02D0,heapScope.a);gabi::call<void>(0x025F0270,heapScope.a,(u32)2);return;
 }
 bool model=gabi::call<u32>(0x0252447C,(u32)0x1004FD58,(u32)0x1004FFA0)!=0;
 packet=gabi::load<u32>(environment()+0xA94);
 u32 resource=gabi::call<u32>(0x0252447C,(u32)0x1004FD58,model?(u32)0x1004FFAC:(u32)0x1004FFDC);
 gabi::store<u32>(packet+0x98,resource);
 packet=gabi::load<u32>(environment()+0xA94);
 resource=gabi::call<u32>(0x0252447C,(u32)0x1004FD58,model?(u32)0x1004FFBC:(u32)0x1004FFEC);
 gabi::store<u32>(packet+0x9C,resource);
 resource=gabi::call<u32>(0x0252447C,(u32)0x1004FD58,model?(u32)0x1004FFCC:(u32)0x1004FFFC);
 gabi::store<u32>(gabi::load<u32>(environment()+0xA94)+0xA0,resource);
 if(!gabi::load<u32>(gabi::load<u32>(environment()+0xA94)+0x98))gabi::store<u8>(environment()+0xA88,99);
 for(u32 i=0;i<100;++i)gabi::store<u8>(gabi::load<u32>(environment()+0xA94)+0xA4+i*0x2C,0);
 gabi::call<void>(0x0256DDF8);
 u32 env=environment();gabi::store<u8>(env+0xA88,gabi::load<u8>(env+0xA88)+1);
 gabi::call<void>(0x0257A7A8,gabi::load<u32>(environment()+0xA94));
 gabi::call<void>(0x025F0270,heapScope.a,(u32)2);
}
VERIFY(0x0257C5B0,move_volumetric_clouds);
static u32 weather_packet(u32 offset) { return gabi::load<u32>(environment()+offset); }
static void start_sun_weather(float zero) {
 gabi::Local<gabi::be<u32>> heapScope;
 gabi::call<void>(0x025F01D8,heapScope.a,(u32)0x1004FF10,(u32)0x2B9D0,(u32)0);
 u32 packet=gabi::call<u32>(0x0273AE48,(u32)0x2628,(u32)0x20);
 if(packet)packet=gabi::call<u32>(0x025786F0,packet);
 gabi::store<u32>(environment()+0xA34,packet);
 packet=gabi::call<u32>(0x0273AE48,(u32)0xA088,(u32)0x20);
 if(packet)packet=gabi::call<u32>(0x025787D4,packet);
 gabi::store<u32>(environment()+0xA38,packet);
 if(!weather_packet(0xA34)||!weather_packet(0xA38))gabi::call<void>(0x025F02D0,heapScope.a);
 else {
  gabi::Local<SafeString> archiveName,resourceName;
  gabi::store<u32>(archiveName.a,0x1004FD80);gabi::store<u32>(archiveName.a+4,0x1004FDD0);
  gabi::store<u32>(resourceName.a,0x1004FF30);gabi::store<u32>(resourceName.a+4,0x1004FDD0);
  u32 entry=gabi::call<u32>(0x026124B0,gabi::load<u32>(0x101F4F7C),archiveName.a,resourceName.a,(u32)0);
  u32 archive=gabi::call<u32>(0x027E2DC0,entry);
  const u32 names[]={0x1004FE10,0x1004FE1C,0x1004FE28,0x1004FE34,0x1004FE9C};
  for(u32 i=0;i<5;++i) {
   u32 resource=gabi::call<u32>(0x027DFA24,archive_resource_array(archive),names[i]);
   gabi::store<u32>(weather_packet(0xA34)+0xD4+4*i,resource);
  }
  gabi::store<u8>(weather_packet(0xA34)+0xC4,0);gabi::store<u8>(weather_packet(0xA34)+0xC5,0);
  for(u32 offset=0xC8;offset<=0xD0;offset+=4)gabi::store<float>(weather_packet(0xA34)+offset,zero);
  for(u32 offset=0xB0;offset<=0xBC;offset+=4)gabi::store<u32>(weather_packet(0xA34)+offset,0);
  const u32 cloudNames[]={0x1004FEA8,0x1004FDE8,0x1004FEB4};
  for(u32 i=0;i<3;++i) {
   u32 resource=gabi::call<u32>(0x027DFA24,archive_resource_array(archive),cloudNames[i]);
   gabi::store<u32>(weather_packet(0xA38)+0x98+4*i,resource);
  }
  u32 cloud=weather_packet(0xA38);gabi::store<float>(cloud+0x118,gabi::load<float>(0x100500B0));
  gabi::store<float>(weather_packet(0xA38)+0x11C,zero);gabi::store<float>(weather_packet(0xA38)+0x124,zero);
  gabi::store<u8>(weather_packet(0xA38)+0x12A,0);
  u32 texture=gabi::load<u32>(weather_packet(0xA38)+0x98);
  gabi::call<void>(0x02578BC0,weather_packet(0xA34),texture);
  gabi::call<void>(0x02578E60,weather_packet(0xA38));
  gabi::call<void>(0x02565E0C);gabi::call<void>(0x02565B70);
  gabi::store<u8>(environment()+0xA30,1);
 }
 gabi::call<void>(0x025F0270,heapScope.a,(u32)2);
}
static void move_rain_weather() {
 u8 status=gabi::load<u8>(environment()+0xA3C);
 switch(status) {
 case 0:
  if(gabi::load<u32>(environment()+0xA40)) {
   gabi::Local<gabi::be<u32>> heapScope;
   gabi::call<void>(0x025F01D8,heapScope.a,(u32)0x1004FE40,(u32)0x3637D0,(u32)0);
   u32 packet=gabi::call<u32>(0x0273AE48,(u32)0x1ACA40,(u32)0x20);
   if(packet)packet=gabi::call<u32>(0x02579280,packet);
   gabi::store<u32>(environment()+0xA44,packet);
   if(!weather_packet(0xA44))gabi::call<void>(0x025F02D0,heapScope.a);
   else {
    gabi::call<void>(0x0256670C);gabi::call<void>(0x02566B88);
    gabi::store<u8>(environment()+0xA3C,1);
    if(gabi::load<u32>(environment()+0xA40)!=250)gabi::call<void>(0x025E1988,(u32)0x185C);
   }
   gabi::call<void>(0x025F0270,heapScope.a,(u32)2);
  }
  break;
 case 1:
  gabi::call<void>(0x02566B88);
  if(!gabi::load<u32>(environment()+0xA4C)) {
   float count=(float)gabi::load<s32>(environment()+0xA40);
   gabi::call<void>(0x025E200C,(u32)(count<gabi::load<float>(0x100500C0)?0:1));
  }
  if(!gabi::load<u32>(environment()+0xA40)) {
   gabi::call<void>(0x025E1988,(u32)0x185D);
   u32 env=environment();gabi::store<u8>(env+0xA3C,gabi::load<u8>(env+0xA3C)+1);
  }
  break;
 case 2: {
  u32 env=environment();gabi::store<u8>(env+0xA3C,gabi::load<u8>(env+0xA3C)+1);break;
 }
 case 3:
  gabi::store<u8>(environment()+0xA3C,0);gabi::call<void>(0x02577E38,environment()+0xA44);break;
 }
}
static void move_snow_weather() {
 u8 status=gabi::load<u8>(environment()+0xA48);
 if(status==0) {
  if(gabi::load<u32>(environment()+0xA4C)) {
   gabi::call<void>(0x025688C4);
   if(weather_packet(0xA50)) {
    if(weather_stage_equals(0x1004FD30))gabi::call<void>(0x02569408);
    else gabi::call<void>(0x02568BD4);
    gabi::store<u8>(environment()+0xA48,1);
   }
  }
 } else if(status==1) {
  if(weather_stage_equals(0x1004FD30))gabi::call<void>(0x02569408);
  else gabi::call<void>(0x02568BD4);
 }
}
static void update_star_amount(float zero) {
 float time=gabi::call<float>(0x02560E08,environment());
 bool special=weather_stage_equals(0x1004FD98);
 float target=gabi::load<float>(0x100500C4),rate=gabi::load<float>(0x100500C8);
 if(special) {
  if(!(time<gabi::load<float>(0x100500CC))) {}
  else if(time<gabi::load<float>(0x100500D0)) {}
  else if(time>gabi::load<float>(0x100500D4))target=subtract_product(gabi::fsubs_ppc(gabi::load<float>(0x100500CC),time),gabi::load<float>(0x100500D8),target);
  else if(time<gabi::load<float>(0x100500DC))target=gabi::fmuls_ppc(gabi::fsubs_ppc(gabi::load<float>(0x100500DC),time),rate);
  else target=zero;
 } else {
  if(!(time<gabi::load<float>(0x100500E0))) {}
  else if(time<gabi::load<float>(0x100500E4)) {}
  else if(time>gabi::load<float>(0x100500E8))target=subtract_product(gabi::fsubs_ppc(gabi::load<float>(0x100500E0),time),gabi::load<float>(0x100500EC),target);
  else if(time<gabi::load<float>(0x100500F0))target=gabi::fmuls_ppc(gabi::fsubs_ppc(gabi::load<float>(0x100500F0),time),rate);
  else target=zero;
 }
 if(gabi::load<u8>(environment()+0x1092))target=zero;
 if(gabi::load<u8>(environment()+0x108D)&&gabi::load<float>(environment()+0xFC8)>gabi::load<float>(0x100500F4))target=zero;
 u32 env=environment();
 gabi::call<void>(0x0200ECD4,env+0xA5C,target,gabi::load<float>(0x100500F8),gabi::load<float>(0x100500FC),gabi::load<float>(0x10050100));
 env=environment();float strength=gabi::load<float>(environment()+0xA5C);
 gabi::store<s32>(env+0xA58,(s16)gabi::ftoi(gabi::fmuls_ppc(gabi::load<float>(0x10050104),strength)));
}
static void move_star_weather() {
 u8 status=gabi::load<u8>(environment()+0xA54);
 switch(status) {
 case 0:
  if(gabi::load<u32>(environment()+0xA58)) {
   gabi::call<void>(0x0256A25C);
   if(weather_packet(0xA60)) {
    gabi::call<void>(0x025796F0,weather_packet(0xA60));gabi::call<void>(0x0256A388);
    gabi::store<u8>(environment()+0xA54,1);
   }
  }break;
 case 1:
  gabi::call<void>(0x0256A388);
  if(gabi::load<u32>(environment()+0xA58))break;
  [[fallthrough]];
 case 2: {
  u32 env=environment();gabi::store<u8>(env+0xA54,gabi::load<u8>(env+0xA54)+1);break;
 }
 case 3:
  gabi::store<u8>(environment()+0xA54,0);gabi::call<void>(0x02577E9C,environment()+0xA60);break;
 }
}
static void start_mud_weather(float zero) {
 gabi::Local<gabi::be<u32>> heapScope;
 gabi::call<void>(0x025F01D8,heapScope.a,(u32)0x1004FF40,(u32)0x2FBF10,(u32)0);
 u32 packet=gabi::call<u32>(0x0273AE48,(u32)0x139250,(u32)0x20);
 if(packet)packet=gabi::call<u32>(0x02579934,packet);
 gabi::store<u32>(environment()+0xA78,packet);
 if(!weather_packet(0xA78))gabi::call<void>(0x025F02D0,heapScope.a);
 else {
  gabi::Local<SafeString> archiveName,resourceName;
  gabi::store<u32>(archiveName.a,0x1004FDA0);gabi::store<u32>(archiveName.a+4,0x1004FDD0);
  gabi::store<u32>(resourceName.a,0x1004FF60);gabi::store<u32>(resourceName.a+4,0x1004FDD0);
  u32 entry=gabi::call<u32>(0x026124B0,gabi::load<u32>(0x101F4F7C),archiveName.a,resourceName.a,(u32)0);
  u32 archive=gabi::call<u32>(0x027E2DC0,entry);
  u32 resource=gabi::call<u32>(0x027DFA24,archive_resource_array(archive),(u32)0x1004FE5C);
  gabi::store<u32>(weather_packet(0xA78)+0x98,resource);
  gabi::store<float>(weather_packet(0xA78)+0x5E64,zero);
  for(u32 i=0;i<300;++i)gabi::store<u8>(weather_packet(0xA78)+0x9C+i*0x50,0);
  gabi::call<void>(0x02579A68,weather_packet(0xA78));gabi::call<void>(0x02567F68);
  gabi::store<u8>(environment()+0xA70,1);
 }
 gabi::call<void>(0x025F0270,heapScope.a,(u32)2);
}
static void move_mud_weather(float zero) {
 u8 status=gabi::load<u8>(environment()+0xA70);
 switch(status) {
 case 0:if(gabi::load<u32>(environment()+0xA74))start_mud_weather(zero);break;
 case 1:
  if(gabi::load<u32>(environment()+0xA74)||gabi::load<float>(weather_packet(0xA78)+0x5E64)>zero) {
   gabi::call<void>(0x02567F68);gabi::store<u32>(environment()+0xA74,0);
  } else {u32 env=environment();gabi::store<u8>(env+0xA70,gabi::load<u8>(env+0xA70)+1);}
  break;
 case 2: {
  gabi::store<u32>(environment()+0xA74,0);u32 env=environment();gabi::store<u8>(env+0xA70,gabi::load<u8>(env+0xA70)+1);break;
 }
 case 3:
  gabi::store<u32>(environment()+0xA74,0);gabi::store<u8>(environment()+0xA70,0);
  gabi::call<void>(0x02577F00,environment()+0xA78);break;
 }
 gabi::call<void>(0x02583678);
}
static bool weather_stage_wrapped_equal(u32 name) {
 gabi::Local<SafeString> expected,actual;
 u32 first=gabi::call<u32>(0x02582BB8,expected.a,name);
 u32 game=gabi::call<u32>(0x025200D4);
 u32 second=gabi::call<u32>(0x02582BB8,actual.a,game+0x5134);
 u32 result=gabi::call<u32>(0x02582C08,first,second);
 gabi::call<void>(0x02582CE8,actual.a,(u32)2);gabi::call<void>(0x02582CE8,expected.a,(u32)2);
 return result==1;
}
static void move_weather_frame() {
 WWHD_FUNC(0x0257ACAC,void);
 gabi::store<u32>(environment()+0x1058,0);
 bool special=weather_stage_equals(0x10050108);float zero=gabi::load<float>(0x10050020);
 if(!special) {
  u32 game=gabi::call<u32>(0x025200D4);
  bool sun=(gabi::load<u16>(game+0x5ACA)&1)&&!gabi::load<u8>(environment()+0x109E)&&
   !weather_stage_equals(0x1004FD68)&&!weather_stage_equals(0x1004FD70)&&!weather_stage_equals(0x1004FD78);
  if(sun) {
   u8 state=gabi::load<u8>(environment()+0xA30);
   if(state==0)start_sun_weather(zero);
   else if(state==1) {gabi::call<void>(0x02565E0C);gabi::call<void>(0x02565B70);}
  }
  move_rain_weather();move_snow_weather();
 }
 u32 game=gabi::call<u32>(0x025200D4);
 bool stars=(gabi::load<u16>(game+0x5ACA)&1)&&!gabi::load<u8>(environment()+0x109E)&&
  !weather_stage_equals(0x1004FD38)&&!weather_stage_equals(0x1004FD88)&&!weather_stage_equals(0x1004FD90);
 if(!stars)stars=weather_stage_equals(0x1004FD98);
 if(stars) {update_star_amount(zero);move_star_weather();}
 if(weather_stage_equals(0x10050108)) {
  if(!weather_stage_wrapped_equal(0x10050108))gabi::call<void>(0x02583D00);
  return;
 }
 if(weather_stage_equals(0x1004FD50))gabi::store<u32>(environment()+0xA68,500);
 u8 poison=gabi::load<u8>(environment()+0xA64);
 if(poison==0) {
  if(gabi::load<u32>(environment()+0xA68)) {gabi::call<void>(0x0256DC04);gabi::store<u8>(environment()+0xA64,1);}
 } else if(poison==1)gabi::call<void>(0x0256CA54);
 move_mud_weather(zero);
 if(!weather_stage_wrapped_equal(0x10050108))gabi::call<void>(0x02583D00);
}
VERIFY(0x0257ACAC,move_weather_frame);
static void configure_cloud_render_state(u32 state,float zero,bool clearHead=true) {
 gabi::store<u8>(state+1,0);
 u32 bits=gabi::load<u32>(state+0xEC);
 gabi::store<u32>(state+0xE4,4);gabi::store<u32>(state+8,2);
 gabi::store<float>(state+0xE8,zero);gabi::store<u8>(state+0xE0,1);
 if(clearHead)gabi::store<u8>(state,0);gabi::store<u32>(state+4,3);gabi::store<u32>(state+12,3);
 gabi::store<u32>(state+0xEC,(((bits&~15u)+7)&~0xF0u)+16);
}
static void draw_cloud_packet(u32 packet) {
 WWHD_FUNC(0x0257EF00,void,packet);
 gabi::Local<WeatherRenderState> state;
 gabi::call<void>(0x02750250,state.a);
 float zero=gabi::load<float>(0x10050020);
 configure_cloud_render_state(state.a,zero);gabi::call<void>(0x02750370,state.a);
 gabi::Local<WeatherMatrix> matrix;
 for(u32 i=0;i<9;++i) {
  gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
  u32 renderer=packet+0x2BF0+i*0x41C;
  gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,(u32)0x104B470C,gabi::load<u32>(0x104B4708)+0x240);
  gabi::call<void>(0x027FE0DC,renderer,(u32)0);
 }
 u32 material=bind_weather_shader(packet+0x12C);
 for(u32 i=0;i<9;++i) {
  if(i)material=gabi::load<u32>(packet+0x12C);
  u32 texture=i<2?0:i==2?2:1;
  gabi::call<void>(0x027FE5B8,packet+0x50EC+texture*0x4C,material);
  gabi::call<void>(0x027FE118,packet+0x2BF0+i*0x41C,gabi::load<u32>(packet+0x12C),(u32)0);
  u32 pair=packet+0x130+i*0x4C0;
  u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
  if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
  gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
 }
 gabi::call<void>(0x02750250,state.a);configure_cloud_render_state(state.a,zero);
 gabi::call<void>(0x02750370,state.a);
 gabi::Local<WeatherMatrix> secondMatrix;
 gabi::call<void>(0x0257E93C,secondMatrix.a,(u32)0x104B45F8);
 gabi::call<void>(0x027FDA54,packet+0x9C6C,(u32)0,secondMatrix.a,(u32)0x104B470C,gabi::load<u32>(0x104B4708)+0x240);
 gabi::call<void>(0x027FE0DC,packet+0x9C6C,(u32)0);
 u32 geometry=packet+0x51D4;
 u32 head=gabi::load<u32>(geometry+0x4A80);
 for(u32 i=0;i<16;++i) {
  u32 element=geometry+head*0x254+i*0x4A8;
  gabi::call<void>(0x027B5E94,element+4,(u32)0,gabi::load<u32>(element+0x150));
 }
 gabi::store<u32>(geometry+0x4A80,gabi::load<u32>(geometry+0x4A80)==0?1:0);
 material=bind_weather_shader(packet+0x51D0);
 gabi::call<void>(0x027FE118,packet+0x9C6C,material,(u32)0);
 u32 side=gabi::load<u32>(geometry+0x4A80)==0?1:0;
 for(u32 i=0;i<16;++i) {
  if(gabi::load<u32>(geometry+0x4A90))gabi::call<void>(0x027BFE5C,geometry+side*0x254+i*0x4A8+0x158);
  gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4A88),(u32)3,gabi::load<u32>(0x104B4A84),gabi::load<u32>(0x104B4A8C),(u32)0,(u32)1);
 }
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x0257EF00,draw_cloud_packet);
static void draw_sun_packet(u32 packet,u32 firstTexture,u32 enabled,u32 allRows) {
 WWHD_FUNC(0x0257E9DC,void,packet,firstTexture,enabled,allRows);
 gabi::Local<WeatherMatrix> matrix;
 for(u32 i=0;i<4;++i) {
  gabi::call<void>(0x0257E93C,matrix.a,(u32)0x104B45F8);
  u32 renderer=packet+0x13F0+i*0x41C;
  gabi::call<void>(0x027FDA54,renderer,(u32)0,matrix.a,(u32)0x104B470C,gabi::load<u32>(0x104B4708)+0x240);
  gabi::call<void>(0x027FE0DC,renderer,(u32)0);
  u32 pair=packet+0xF0+i*0x4C0;
  u32 element=pair+gabi::load<u32>(pair+0x4A8)*0x254;
  gabi::call<void>(0x027B5E94,element+4,(u32)0,gabi::load<u32>(element+0x150));
  gabi::store<u32>(pair+0x4A8,gabi::load<u32>(pair+0x4A8)==0?1:0);
 }
 bind_weather_shader(packet+0xEC);
 gabi::Local<WeatherRenderState> state;
 gabi::call<void>(0x02750250,state.a);
 configure_cloud_render_state(state.a,gabi::load<float>(0x10050020),false);
 gabi::call<void>(0x02750370,state.a);
 for(u32 i=0;i<4;++i) {
  if(i<2?!enabled:!allRows)continue;
  gabi::call<void>(0x027FE118,packet+0x13F0+i*0x41C,gabi::load<u32>(packet+0xEC),(u32)0);
  u32 texture=i==0?firstTexture:i==2?4:5;
  gabi::call<void>(0x027FE5B8,packet+0x2460+texture*0x4C,gabi::load<u32>(packet+0xEC));
  u32 pair=packet+0xF0+i*0x4C0;
  u32 side=gabi::load<u32>(pair+0x4A8)==0?1:0;
  if(gabi::load<u32>(pair+0x4B8))gabi::call<void>(0x027BFE5C,pair+side*0x254+0x158);
  gabi::call<void>(0xC0006178,gabi::load<u32>(0x104B4D00),(u32)4,gabi::load<u32>(0x104B4CFC),gabi::load<u32>(0x104B4D04),(u32)0,(u32)1);
 }
 gabi::call<void>(0x02750370,(u32)0x104B474C);
}
VERIFY(0x0257E9DC,draw_sun_packet);
static void setup_rain_geometry(u32 packet) {
 WWHD_FUNC(0x0257F490,void,packet);
 gabi::store<u32>(packet+0x3898,lookup_weather_material(packet,0x100501BC));
 for(u32 i=0;i<250;++i) {
  for(u32 quad=0;quad<4;++quad)
   prepare_weather_pair(packet+0x389C+i*0x1300+quad*0x4C0,gabi::load<u32>(packet+0x3898),0x100501E8);
  gabi::call<void>(0x027FB5D4,packet+0x176A20+i*0x370,(u32)0);
  prepare_weather_pair(packet+0x12C69C+i*0x4C0,gabi::load<u32>(packet+0x3898),0x100501E8);
 }
 gabi::Local<WeatherTextureDescriptor> texture;
 gabi::call<void>(0x027BE6B8,texture.a);
 for(u32 i=0;i<2;++i) {
  u32 manager=gabi::load<u32>(0x101F8B18),resource=gabi::load<u32>(packet+0x98+4*i);
  gabi::call<void>(0x0274FBF8,manager);
  gabi::call<void>(0x02773798,texture.a,resource);
  gabi::call<void>(0x0274FCCC,gabi::load<u32>(0x101F8B18));
  u32 target=packet+0x1AC9A4+i*0x4C;
  gabi::call<void>(0x027FE500,target,(u32)0,texture.a,(u32)0);
  target=gabi::call<u32>(0x027FE640,target,(u32)0);
  u8 flags=gabi::load<u8>(target+0x190);
  gabi::store<u32>(target+0x164,1);gabi::store<u32>(target+0x15C,1);gabi::store<u32>(target+0x160,1);
  gabi::store<u8>(target+0x190,flags|2);
 }
 gabi::call<void>(0x027FD838,packet+0x1AC8F0,(u32)1,(u32)0);
 gabi::call<void>(0x027FB5D4,packet+0x1AC8FC,(u32)0);
 gabi::call<void>(0x027FB5D4,packet+0x1AC580,(u32)0);
}
VERIFY(0x0257F490,setup_rain_geometry);
}

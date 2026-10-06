// HD quake/light detection TU02529BC4..0252A243; adjacent initializer inferred.
#include "bindings.h"
namespace d_detect_cpp {
static f32 constant(u32 ea) { return gabi::load<f32>(ea); }
static u32 PlaceCtor(u32 self) {
 WWHD_FUNC(0x02529BC4, u32, self);
 if (self == 0) self = gabi::call<u32>(0x0273AD10, 16u);
 if (self != 0) {
  gabi::store<f32>(self, constant(0x101FFBA8));
  gabi::store<f32>(self + 4, constant(0x101FFBAC));
  f32 z = constant(0x101FFBB0);
  gabi::store<u16>(self + 12, 0);
  gabi::store<f32>(self + 8, z);
 }
 return self;
}
VERIFY(0x02529BC4, PlaceCtor);
static s32 PlaceEnabled(u32 self) {
 WWHD_FUNC(0x02529C1C, s32, self);
 return gabi::load<s16>(self + 12) != 0;
}
VERIFY(0x02529C1C, PlaceEnabled);
static u8 QuakeArea(u32 self, u32 pos) {
 WWHD_FUNC(0x02529C2C, u8, self, pos);
 u32 game = gabi::call<u32>(0x025200D4);
 u32 player = gabi::load<u32>(game + 0x5B2C);
 f32 x = gabi::load<f32>(pos);
 f32 pz = gabi::load<f32>(player + 0x31C);
 f32 px = gabi::load<f32>(player + 0x314);
 gabi::Local<cXyz> a, b;
 b->x = x;
 a->x = px;
 f32 z = gabi::load<f32>(pos + 8);
 f32 zero = constant(0x1004C4EC);
 b->z = z;
 b->y = zero;
 a->z = pz;
 a->y = zero;
 f32 dist = gabi::call<f32>(0x028E8DE8, a.get(), b.get());
 f32 py = gabi::load<f32>(player + 0x318);
 f32 bound = constant(0x1004C4F0);
 f32 y = gabi::load<f32>(pos + 4);
 f32 diff = y - py;
 if (dist > bound) return 0;
 if (diff > constant(0x1004C4F4)) return 0;
 if (diff < constant(0x1004C4F8)) return 0;
 return 1;
}
VERIFY(0x02529C2C, QuakeArea);
static s32 CheckQuake(u32 self, u32 pos) {
 WWHD_FUNC(0x02529CE8, s32, self, pos);
 u32 game = gabi::call<u32>(0x025200D4);
 s16 timer = gabi::load<s16>(self + 16);
 u32 player = gabi::load<u32>(game + 0x5B2C);
 s32 result = 0;
 if (timer > 0) return 1;
 if ((gabi::load<u32>(player + 0x3C0) & 0x2000) != 0) {
  if (pos == 0 || QuakeArea(self, pos) != 0) result = 1;
 }
 if (gabi::load<s16>(self + 12) > 0) result = 1;
 return result;
}
VERIFY(0x02529CE8, CheckQuake);
static void SetQuake(u32 self, u32 pos) {
 WWHD_FUNC(0x02529D7C, void, self, pos);
 if (pos == 0) { gabi::store<u16>(self + 16, 0xFFFF); return; }
 if (PlaceEnabled(self) != 0) return;
 gabi::store<u16>(self + 12, 0xFFFF);
 for (u32 i = 0; i < 3; i++) gabi::store<u32>(self + i * 4, gabi::load<u32>(pos + i * 4));
}
VERIFY(0x02529D7C, SetQuake);
static u32 SearchTagLight(u32 actor, u32 pos) {
 WWHD_FUNC(0x02529DE8, u32, actor, pos);
 if (gabi::call<s32>(0x025D4604, actor) == 0) return 0;
 if (actor == 0 || gabi::load<s16>(actor + 8) != 469 || gabi::load<u8>(actor + 0x420) == 0) return 0;
 gabi::Local<cXyz> local;
 gabi::call<void>(0x028E8F64, actor + 0x3F0, pos, local.get());
 s32 shape = gabi::load<s32>(actor + 0x3BC);
 if (shape == 0) {
  f32 x = local->x;
  if (x < constant(0x1004BCF4) || x > constant(0x1004BCF8)) return 0;
  f32 y = local->y;
  if (y < constant(0x1004BCFC) || y > constant(0x1004BD00)) return 0;
  f32 z = local->z;
  if (z < constant(0x1004BD04) || z > constant(0x1004BD08)) return 0;
  return actor;
 }
 if (shape != 1) return 0;
 f32 y = local->y;
 f32 low = constant(0x1004BD0C);
 if (y < low) return 0;
 f32 high = constant(0x1004BD10);
 if (y > high) return 0;
 f32 x = local->x;
 f32 zero = constant(0x1004C4EC);
 f32 z = local->z;
 gabi::Local<cXyz> radial;
 radial->y = zero;
 radial->x = x;
 radial->z = z;
 f32 dist2 = gabi::call<f32>(0x028E8DD0, radial.get());
 double radius = gabi::call<double>(0x028F4384, (double)dist2);
 y = local->y;
 f32 range = high - low;
 f32 dy = y - low;
 f32 one = constant(0x1004C4FC);
 f32 ratio = dy / range;
 s32 param = gabi::call<s32>(0x024AE670, actor, 4u, 10u);
 f32 p = (f32)param;
 f32 scale = constant(0x1004C508);
 f32 inv = one - ratio;
 f32 pp = p * scale;
 f32 factor = gabi::fmadds(ratio, pp, inv);
 f32 bound = factor * constant(0x1004BD14);
 if (radius > bound) return 0;
 return actor;
}
VERIFY(0x02529DE8, SearchTagLight);
static s32 CheckLight(u32 self, u32 pos) {
 WWHD_FUNC(0x0252A038, s32, self, pos);
 return gabi::call<u32>(0x025D5218, 0x02529DE8u, pos) != 0;
}
VERIFY(0x0252A038, CheckLight);
static s32 CheckAttention(u32 self, u32 out) {
 WWHD_FUNC(0x0252A068, s32, self, out);
 u32 game = gabi::call<u32>(0x025200D4);
 u32 target = gabi::load<u32>(game + 0x5968);
 u32 actor = gabi::call<u32>(0x024EC0A0, game + 0x595C, target);
 if (actor == 0) return 0;
 for (u32 i = 0; i < 3; i++) gabi::store<f32>(out + i * 4, gabi::load<f32>(actor + 0x37C + i * 4));
 return 1;
}
VERIFY(0x0252A068, CheckAttention);
static u32 DetectCtor(u32 self) {
 WWHD_FUNC(0x0252A0E0, u32, self);
 if (self == 0) self = gabi::call<u32>(0x0273AD10, 20u);
 if (self != 0) {
  gabi::call<void>(0x028EFFD0, self, 1u, 16u, 0x02529BC4u);
  gabi::store<u16>(self + 16, 0);
 }
 return self;
}
VERIFY(0x0252A0E0, DetectCtor);
static void PlaceDtor(u32 self, u32 flags) {
 WWHD_FUNC(0x0252A140, void, self, flags);
 if (self != 0 && (flags & 1) != 0) gabi::call<void>(0x0273AF40, self);
}
VERIFY(0x0252A140, PlaceDtor);
static void Proc(u32 self) {
 WWHD_FUNC(0x0252A154, void, self);
 for (u32 off : {12u,16u}) {
  s32 timer = gabi::load<s16>(self + off);
  if (timer > 0) gabi::store<u16>(self + off, (u16)(timer - 1));
  else if (timer < 0) gabi::store<u16>(self + off, 1);
 }
}
VERIFY(0x0252A154, Proc);
static void __sinit_d_detect_cpp() {
 WWHD_FUNC(0x0252A1B0, void, (u32)0);
 sinit_header_statics(0x104756E8, 0x101D603C);
}
VERIFY(0x0252A1B0, __sinit_d_detect_cpp);
}

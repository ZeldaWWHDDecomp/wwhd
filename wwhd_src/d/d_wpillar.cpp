// Water pillar HD contiguous unit025D22F8..025D38BB.
#include "bindings.h"
namespace d_wpillar_cpp {
static f32 constant(u32 ea) { return gabi::load<f32>(ea); }
struct Float4 { be<f32> value[4]; };
static s32 Joint(u32 self, s32 joint) {
 WWHD_FUNC(0x025D22F8, s32, self, joint);
 u32 model = gabi::load<u32>(self + 0xFC);
 u32 anim = gabi::load<u32>(model + 0x2C);
 u32 mats = gabi::load<u32>(anim + 16);
 u16 flags = gabi::load<u16>(anim + 4);
 u32 mtx = mats + u32(joint) * 48;
 gabi::store<u16>(anim + 4, flags | 16);
 if (u32(joint) - 1 >= 3) gabi::call<void>(0x0273AA24, 0x10056C08u, 86u, 0x10056C18u);
 f32 y = gabi::load<f32>(self + 0xE4);
 f32 scale = gabi::load<f32>(self + 0xEC);
 f32 low = constant(0x10056BF8 + u32(joint) * 4);
 f32 base = y != y ? y : gabi::fmadds(low, scale, y);
 f32 old = gabi::load<f32>(mtx + 28);
 f32 delta = f32(f64(old) - f64(base));
 f32 height = gabi::load<f32>(self + 0x3CC);
 f32 result = gabi::fmadds(delta, height, base);
 // Match the reference model additive operand when both operands are NaNs.
 if (base != base) result = base;
 gabi::store<f32>(mtx + 28, result);
 gabi::call<void>(0x028E90D4, mtx, 0x104B4868u);
 return 1;
}
VERIFY(0x025D22F8, Joint);
static s32 JointCallback(u32 node, s32 phase) {
 WWHD_FUNC(0x025D23BC, s32, node, phase);
 if (phase == 0) {
  u32 joint = gabi::call<u32>(0x027F7878, node);
  u32 model = gabi::load<u32>(0x104B462C);
  u16 no = gabi::load<u16>(joint + 4);
  Joint(gabi::load<u32>(model + 0xB8), no);
 }
 return 1;
}
VERIFY(0x025D23BC, JointCallback);
static void ColorFloats(u32 out, u32 in) {
 WWHD_FUNC(0x025D23FC, void, out, in);
 u8 a = gabi::load<u8>(in);
 u8 b = gabi::load<u8>(in + 1);
 f32 div = constant(0x10056C38);
 u8 c = gabi::load<u8>(in + 2);
 f32 x = f32(a) / div;
 u8 d = gabi::load<u8>(in + 3);
 f32 y = f32(b) / div;
 f32 z = f32(c) / div;
 f32 w = f32(d) / div;
 gabi::store<f32>(out, x);
 gabi::store<f32>(out + 4, y);
 gabi::store<f32>(out + 8, z);
 gabi::store<f32>(out + 12, w);
}
VERIFY(0x025D23FC, ColorFloats);
static s32 Execute(u32 self) {
 WWHD_FUNC(0x025D2BA4, s32, self);
 gabi::call<void>(0x025E742C, self + 0x100);
 bool stopped = (gabi::load<u8>(self + 0x10F) & 1) != 0;
 if (!stopped) stopped = gabi::load<f32>(self + 0x100) == constant(0x10056C50);
 if (stopped) gabi::call<void>(0x025DAD48, self);
 else {
  gabi::Local<be<f32>> water;
  if (gabi::call<s32>(0x025D9F70, self + 0xE0, water.get()) != 0) {
   u32 model = gabi::load<u32>(self + 0xFC);
   f32 y = *water;
   u32 matrix = model == 0 ? 0 : model + 0xC8;
   gabi::store<f32>(matrix + 28, y);
   gabi::store<f32>(self + 0xE4, *water);
  }
 }
 return 1;
}
VERIFY(0x025D2BA4, Execute);
static s32 ExecuteWrapper(u32 self) { WWHD_FUNC(0x025D2C3C, s32, self); return Execute(self); }
VERIFY(0x025D2C3C, ExecuteWrapper);
static s32 IsDelete(u32 self) { WWHD_FUNC(0x025D2C40, s32, self); return 1; }
VERIFY(0x025D2C40, IsDelete);
static s32 Delete(u32 self) {
 WWHD_FUNC(0x025D2C48, s32, self);
 u32 heap = gabi::load<u32>(self + 0x200);
 if (heap != 0) gabi::call<void>(0x025E3868, heap);
 gabi::call<void>(0x027F3628, self + 0x110, 0u);
 gabi::call<void>(0x025DD630, self, 0u);
 return 1;
}
VERIFY(0x025D2C48, Delete);
static s32 DeleteWrapper(u32 self) { WWHD_FUNC(0x025D2C9C, s32, self); return Delete(self); }
VERIFY(0x025D2C9C, DeleteWrapper);
static u32 Constructor(u32 self) {
 WWHD_FUNC(0x025D2CA0, u32, self);
 if (self == 0) self = gabi::call<u32>(0x0273AD10, 0x3D0u);
 if (self == 0) return self;
 gabi::call<void>(0x025DD5F0, self);
 gabi::store<u32>(self + 0xB4, 0x10056B34);
 gabi::call<void>(0x027F2BC0, self + 0x100, 0u);
 gabi::store<u32>(self + 0x110, 0x1016E54C);
 gabi::call<void>(0x027DA984, self + 0x114);
 gabi::store<u32>(self + 0x188, 0);
 gabi::store<u32>(self + 0x158, 0);
 gabi::store<u32>(self + 0x180, 0);
 gabi::store<u32>(self + 0x110, 0x10056B44);
 gabi::store<u32>(self + 0x17C, 0);
 gabi::store<u32>(self + 0x184, 0);
 gabi::store<u32>(self + 0x148, 0x1016D820);
 gabi::call<void>(0x025E7C6C, self + 0x18C);
 f32 v0 = constant(0x1016E414);
 gabi::store<f32>(self + 0x204, v0);
 f32 v1 = constant(0x1016E418);
 gabi::store<f32>(self + 0x208, v1);
 f32 v2 = constant(0x1016E41C);
 gabi::store<f32>(self + 0x20C, v2);
 f32 v3 = constant(0x1016E420);
 gabi::store<f32>(self + 0x210, v3);
 f32 v4 = constant(0x1016E424);
 gabi::store<f32>(self + 0x214, v4);
 f32 v5 = constant(0x1016E428);
 gabi::store<f32>(self + 0x218, v5);
 u8 v6 = gabi::load<u8>(0x1016E42C);
 gabi::store<u8>(self + 0x21C, v6);
 u8 v7 = gabi::load<u8>(0x1016E42D);
 gabi::store<u8>(self + 0x21D, v7);
 u8 v8 = gabi::load<u8>(0x1016E42E);
 gabi::store<u8>(self + 0x21E, v8);
 u8 v9 = gabi::load<u8>(0x1016E42F);
 gabi::store<u8>(self + 0x21F, v9);
 u16 v10 = gabi::load<u16>(0x1016E430);
 gabi::store<u16>(self + 0x220, v10);
 u16 v11 = gabi::load<u16>(0x1016E432);
 gabi::store<u16>(self + 0x222, v11);
 u16 v12 = gabi::load<u16>(0x1016E434);
 gabi::store<u16>(self + 0x224, v12);
 u16 v13 = gabi::load<u16>(0x1016E436);
 gabi::store<u16>(self + 0x226, v13);
 f32 v14 = constant(0x1016E438);
 gabi::store<f32>(self + 0x228, v14);
 f32 v15 = constant(0x1016E43C);
 gabi::store<f32>(self + 0x22C, v15);
 f32 v16 = constant(0x1016E440);
 gabi::store<f32>(self + 0x230, v16);
 f32 v17 = constant(0x1016E444);
 gabi::store<f32>(self + 0x234, v17);
 f32 v18 = constant(0x1016E448);
 gabi::store<f32>(self + 0x238, v18);
 f32 v19 = constant(0x1016E44C);
 gabi::store<f32>(self + 0x23C, v19);
 f32 v20 = constant(0x1016E450);
 gabi::store<f32>(self + 0x240, v20);
 f32 v21 = constant(0x1016E454);
 gabi::store<f32>(self + 0x2D8, v5);
 gabi::store<f32>(self + 0x304, v21);
 gabi::store<f32>(self + 0x2F4, v17);
 gabi::store<f32>(self + 0x244, v21);
 gabi::store<u8>(self + 0x360, v6);
 gabi::store<f32>(self + 0x348, v0);
 gabi::store<u16>(self + 0x36A, v13);
 gabi::store<f32>(self + 0x2D0, v3);
 gabi::store<u16>(self + 0x2E2, v11);
 gabi::store<f32>(self + 0x300, v20);
 gabi::store<f32>(self + 0x2D4, v4);
 gabi::store<f32>(self + 0x374, v16);
 gabi::store<u8>(self + 0x2DF, v9);
 gabi::store<f32>(self + 0x2C4, v0);
 gabi::store<u16>(self + 0x364, v10);
 gabi::store<f32>(self + 0x354, v3);
 gabi::store<f32>(self + 0x2C8, v1);
 gabi::store<u8>(self + 0x2DD, v7);
 gabi::store<u8>(self + 0x362, v8);
 gabi::store<f32>(self + 0x2F0, v16);
 gabi::store<f32>(self + 0x358, v4);
 gabi::store<f32>(self + 0x2CC, v2);
 gabi::store<u8>(self + 0x363, v9);
 gabi::store<f32>(self + 0x34C, v1);
 gabi::store<f32>(self + 0x370, v15);
 gabi::store<u16>(self + 0x368, v12);
 gabi::store<u16>(self + 0x366, v11);
 gabi::store<f32>(self + 0x378, v17);
 gabi::store<u16>(self + 0x2E0, v10);
 gabi::store<f32>(self + 0x2FC, v19);
 gabi::store<u16>(self + 0x2E6, v13);
 gabi::store<f32>(self + 0x350, v2);
 gabi::store<f32>(self + 0x2EC, v15);
 gabi::store<u8>(self + 0x361, v7);
 gabi::store<u16>(self + 0x2E4, v12);
 gabi::store<u8>(self + 0x2DC, v6);
 gabi::store<f32>(self + 0x35C, v5);
 gabi::store<f32>(self + 0x2F8, v18);
 gabi::store<u8>(self + 0x2DE, v8);
 gabi::store<f32>(self + 0x37C, v18);
 gabi::store<f32>(self + 0x36C, v14);
 gabi::store<f32>(self + 0x2E8, v14);
 gabi::store<f32>(self + 0x380, v19);
 gabi::store<f32>(self + 0x384, v20);
 gabi::store<f32>(self + 0x388, v21);
 return self;
}
VERIFY(0x025D2CA0, Constructor);
struct Color8 { be<s16> v[4]; };
static u32 tevCall(u32 mat, u32 slot, u32 index, u32 color = 0, bool pointer = false) {
 u32 tev = gabi::load<u32>(mat + 24);
 u32 vt = gabi::load<u32>(tev + 4);
 u32 target = gabi::load<u32>(vt + slot);
 if (pointer) return gabi::call_ptr<u32>(target, tev, index, color);
 return gabi::call_ptr<u32>(target, tev, index);
}
static void normalizeS10(u32 out, u32 color, f32 divisor) {
 for (u32 i = 0; i < 4; i++) gabi::store<f32>(out + i * 4, f32(gabi::load<s16>(color + i * 2)) / divisor);
}
static s32 Draw(u32 self) {
 WWHD_FUNC(0x025D24B0, s32, self);
 u32 env = gabi::call<u32>(0x02555D0C);
 gabi::call<void>(0x025626A4, env, 2u, self + 0xE0, self + 0x204);
 env = gabi::call<u32>(0x02555D0C);
 u32 model = gabi::load<u32>(self + 0xFC);
 gabi::call<void>(0x02562F5C, env, model, self + 0x204);
 model = gabi::load<u32>(self + 0xFC);
 f32 frame = gabi::load<f32>(self + 0x104);
 u32 data = gabi::load<u32>(model + 0xAC);
 gabi::call<void>(0x025E86B8, self + 0x100, data, frame);
 bool bomb = gabi::load<s32>(self + 0xF8) == 1;
 f32 divisor = constant(0x10056C38);
 gabi::Local<Color8> color, key;
 gabi::Local<Float4> normalized, transformed, normalizedKey, transformedKey;
 if (!bomb) {
  for (u32 i = 0; i < 4; i++) color->v[i] = gabi::load<s16>(self + 0x294 + i * 2);
  gabi::store<u32>(gabi::ea(key.get()), gabi::load<u32>(self + 0x29C));
  model = gabi::load<u32>(self + 0xFC);
  frame = gabi::load<f32>(self + 0x104);
  data = gabi::load<u32>(model + 0xAC);
  gabi::call<void>(0x025E7FC4, self + 0x18C, data, frame);
 }
 model = gabi::load<u32>(self + 0xFC);
 data = gabi::load<u32>(model + 0xAC);
 u32 table = gabi::call<u32>(0x027F3F8C, data);
 u32 count = gabi::load<u16>(table + 0x24);
 for (u32 remaining = count; remaining != 0; remaining--) {
  u32 index = (remaining - 1) & 0xFFFF;
  model = gabi::load<u32>(self + 0xFC);
  data = gabi::load<u32>(model + 0xAC);
  u32 bound = gabi::load<u32>(data + 12);
  u32 mat = gabi::load<u32>(data + 16);
  if (index < bound) mat += index * 924;
  if (bomb) {
   for (u32 i = 0; i < 4; i++) color->v[i] = gabi::load<s16>(self + 0x294 + i * 2);
   u32 c = tevCall(mat, 52, 2);
   color->v[3] = gabi::load<s16>(c + 6);
   gabi::store<u32>(gabi::ea(key.get()), gabi::load<u32>(self + 0x29C));
   c = tevCall(mat, 52, 1);
   gabi::store<u8>(gabi::ea(key.get()) + 3, gabi::load<s16>(c + 6));
  }
  env = gabi::call<u32>(0x02555D0C);
  f32 factor = gabi::load<f32>(env + 0x10B8);
  tevCall(mat, 36, bomb ? 2u : 0u, gabi::ea(color.get()), true);
  normalizeS10(gabi::ea(normalized.get()), gabi::ea(color.get()), divisor);
  // GHS upper half of the bomb color temporary aliases the saved normalized-red scratch word.
  if (bomb) gabi::store<f32>(gabi::ea(key.get()) + 4, normalized->value[0]);
  gabi::call<void>(0x0274D458, transformed.get(), normalized.get(), factor);
  // Slot request may return a different color pointer; retain its result.
  u32 flags = gabi::load<u32>(mat + 0xA0);
  u32 slot = bomb ? 6u : 4u;
  gabi::store<u32>(mat + 0xA0, flags | (1u << slot));
  u32 out = gabi::call<u32>(0x027F9F0C, mat + 0xA0, slot);
  f32 alpha = f32(gabi::load<s16>(gabi::ea(color.get()) + 6)) / divisor;
  f32 x = transformed->value[0], y = transformed->value[1], z = transformed->value[2];
  gabi::store<f32>(out + 4, y); gabi::store<f32>(out + 8, z); gabi::store<f32>(out, x); gabi::store<f32>(out + 12, alpha);
  env = gabi::call<u32>(0x02555D0C);
  factor = gabi::load<f32>(env + 0x10BC);
  if (bomb) {
   tevCall(mat, 36, 1, gabi::ea(key.get()), true);
   normalizeS10(gabi::ea(normalizedKey.get()), gabi::ea(key.get()), divisor);
  } else {
   tevCall(mat, 60, 0, gabi::ea(key.get()), true);
   ColorFloats(gabi::ea(normalizedKey.get()), gabi::ea(key.get()));
  }
  gabi::call<void>(0x0274D458, transformedKey.get(), normalizedKey.get(), factor);
  flags = gabi::load<u32>(mat + 0xA0);
  slot = bomb ? 5u : 7u;
  gabi::store<u32>(mat + 0xA0, flags | (1u << slot));
  out = gabi::call<u32>(0x027F9F0C, mat + 0xA0, slot);
  alpha = bomb ? f32(gabi::load<s16>(gabi::ea(key.get()) + 6)) / divisor : f32(gabi::load<u8>(gabi::ea(key.get()) + 3)) / divisor;
  x = transformedKey->value[0]; y = transformedKey->value[1]; z = transformedKey->value[2];
  gabi::store<f32>(out + 4, y); gabi::store<f32>(out + 8, z); gabi::store<f32>(out, x); gabi::store<f32>(out + 12, alpha);
 }
 if (bomb) {
  for (u32 pass = 0; pass < 2; pass++) {
   u32 game = gabi::call<u32>(0x025200D4);
   u32 player = gabi::load<u32>(game + 0x5B34);
   frame = gabi::load<f32>(self + 0x104);
   u32 anim = gabi::load<u32>(player + (pass == 0 ? 0x455C : 0x462C));
   gabi::store<f32>(player + (pass == 0 ? 0x4550 : 0x45C8), frame);
   gabi::store<f32>(anim, frame);
   u32 ctrl = gabi::load<u32>(player + (pass == 0 ? 0x456C : 0x45D4));
   u32 target = gabi::load<u32>(ctrl + 16);
   f32 a = gabi::load<f32>(ctrl + 4), b = gabi::load<f32>(ctrl + 8);
   u32 arg = gabi::load<u32>(ctrl + 20);
   f32 result = gabi::call_ptr<f32>(target, arg, frame, a, b);
   gabi::store<f32>(ctrl, result);
   gabi::call<void>(0x027DF40C, player + (pass == 0 ? 0x456C : 0x45D4));
  }
 }
 model = gabi::load<u32>(self + 0xFC);
 gabi::call<void>(0x025E2DE0, model, 0u);
 return 1;
}
VERIFY(0x025D24B0, Draw);
static s32 DrawWrapper(u32 self) { WWHD_FUNC(0x025D2BA0, s32, self); return Draw(self); }
VERIFY(0x025D2BA0, DrawWrapper);
struct StringPair { be<u32> text, vtable; };
struct GroundLocal { u8 bytes[0x54]; };
static s32 Create(u32 self) {
 WWHD_FUNC(0x025D2EB0, s32, self);
 if (self != 0) Constructor(self);
 u32 heap = gabi::call<u32>(0x025E3630, 0u, 32u);
 gabi::store<u32>(self + 0x200, heap);
 if (heap == 0) return 5;
 f32 rate = constant(0x10056C54);
 gabi::store<u32>(heap + 16, 0x10056C5C);
 bool normal = gabi::load<s32>(self + 0xF8) == 0;
 gabi::Local<StringPair> modelName, bckName, btkName;
 modelName->vtable = 0x10056B1C;
 modelName->text = normal ? 0x10056C64 : 0x10056C6C;
 u32 resources = gabi::load<u32>(0x101F4F28);
 u32 data = gabi::call<u32>(0x026066C4, resources, modelName.get(), normal ? 0x3Bu : 0x42u);
 if (data == 0) gabi::call<void>(0x0273AA24, 0x10056C74u, normal ? 0x148u : 0x163u, 0x10056C84u);
 u32 model = gabi::call<u32>(0x025E38E0, data, 0x80000u, 0x11000202u);
 bckName->vtable = 0x10056B1C;
 gabi::store<u32>(self + 0xFC, model);
 resources = gabi::load<u32>(0x101F4F28);
 bckName->text = normal ? 0x10056C64 : 0x10056C6C;
 u32 bck = gabi::call<u32>(0x026066C4, resources, bckName.get(), normal ? 0x16u : 0xEu);
 s32 initialized = gabi::call<s32>(0x025E8508, self + 0x100, data, bck, 1u, 0u, rate, 0u, -1, 0u);
 gabi::Local<Color8> color1, color2;
 gabi::Local<Float4> normalized1, normalized2, transformed1, transformed2;
 if (normal) {
  btkName->vtable = 0x10056B1C;
  resources = gabi::load<u32>(0x101F4F28);
  btkName->text = 0x10056C64;
  u32 btk = gabi::call<u32>(0x026066C4, resources, btkName.get(), 0x5Eu);
  initialized &= gabi::call<s32>(0x025E7CE0, self + 0x18C, data, btk, 0u, 2u, rate, 0u, -1, 0u, 0u);
 } else {
  model = gabi::load<u32>(self + 0xFC);
  if (model != 0) {
   u32 count = gabi::load<u16>(model + 0x2A);
   if (count != 0) {
    f32 divisor = constant(0x10056C38);
    u32 offset = 0;
    for (u32 i = 0; i < count; i++, offset += 60) {
     u32 md = gabi::load<u32>(model + 0xAC);
     u32 bound = gabi::load<u32>(md + 12);
     u32 colors = gabi::load<u32>(model + 0x34);
     u32 mat = gabi::load<u32>(md + 16);
     if (u16(i) < bound) mat += u16(i) * 924u;
     gabi::call<void>(0x027FA160, color1.get(), colors + offset, 1u);
     tevCall(mat, 36, 1u, gabi::ea(color1.get()), true);
     normalizeS10(gabi::ea(normalized1.get()), gabi::ea(color1.get()), divisor);
     gabi::call<void>(0x0274D458, transformed1.get(), normalized1.get(), rate);
     u32 flags = gabi::load<u32>(mat + 0xA0);
     gabi::store<u32>(mat + 0xA0, flags | 32);
     u32 out = gabi::call<u32>(0x027F9F0C, mat + 0xA0, 5u);
     f32 alpha = f32(color1->v[3]) / divisor;
     f32 x = transformed1->value[0], y = transformed1->value[1], z = transformed1->value[2];
     gabi::store<f32>(out + 4, y); gabi::store<f32>(out + 8, z); gabi::store<f32>(out, x); gabi::store<f32>(out + 12, alpha);
     model = gabi::load<u32>(self + 0xFC);
     colors = gabi::load<u32>(model + 0x34);
     gabi::call<void>(0x027FA160, color2.get(), colors + offset, 2u);
     tevCall(mat, 36, 2u, gabi::ea(color2.get()), true);
     normalizeS10(gabi::ea(normalized2.get()), gabi::ea(color2.get()), divisor);
     gabi::call<void>(0x0274D458, transformed2.get(), normalized2.get(), rate);
     flags = gabi::load<u32>(mat + 0xA0);
     gabi::store<u32>(mat + 0xA0, flags | 64);
     out = gabi::call<u32>(0x027F9F0C, mat + 0xA0, 6u);
     alpha = f32(color2->v[3]) / divisor;
     x = transformed2->value[0]; y = transformed2->value[1]; z = transformed2->value[2];
     gabi::store<f32>(out + 4, y); gabi::store<f32>(out + 8, z); gabi::store<f32>(out, x); gabi::store<f32>(out + 12, alpha);
     model = gabi::load<u32>(self + 0xFC);
     count = gabi::load<u16>(model + 0x2A);
    }
   }
  }
 }
 gabi::call<void>(0x025E37D8);
 heap = gabi::load<u32>(self + 0x200);
 gabi::call<void>(0x025E3678, heap);
 model = gabi::load<u32>(self + 0xFC);
 if (model == 0 || initialized == 0) return 5;
 f32 height = gabi::load<f32>(self + 0xF0);
 f32 scale = gabi::load<f32>(self + 0xEC);
 gabi::store<f32>(self + 0x3CC, height);
 gabi::store<f32>(self + 0xF0, scale);
 model = gabi::load<u32>(self + 0xFC);
 f32 scaleZ = gabi::load<f32>(self + 0xF4);
 gabi::store<f32>(model + 0xBC, scale);
 gabi::store<f32>(model + 0xC0, scale);
 gabi::store<f32>(model + 0xC4, scaleZ);
 f32 posY = gabi::load<f32>(self + 0xE4), posZ = gabi::load<f32>(self + 0xE8), posX = gabi::load<f32>(self + 0xE0);
 gabi::call<void>(0x028E93CC, 0x1048D0CCu, posX, posY, posZ);
 f32 matrix[12]; for (u32 i = 0; i < 12; i++) matrix[i] = constant(0x1048D0CC + i * 4);
 model = gabi::load<u32>(self + 0xFC);
 for (u32 i = 0; i < 12; i++) gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
 normal = gabi::load<s32>(self + 0xF8) == 0;
 if (normal) {
  u32 joints = gabi::call<u32>(0x027F3F94, data);
  for (u32 i = 1; i < gabi::load<u16>(joints + 8);) {
   u32 count = gabi::load<u32>(data + 4);
   u32 joint = gabi::load<u32>(data + 8);
   if (i < count) joint += i * 28;
   i = u16(i + 1);
   gabi::store<u32>(joint + 8, 0x025D23BC);
   joints = gabi::call<u32>(0x027F3F94, data);
  }
  model = gabi::load<u32>(self + 0xFC);
  gabi::store<u32>(model + 0xB8, self);
  for (u32 id = 0x3D; id <= 0x40; id++) {
   u32 game = gabi::call<u32>(0x025200D4);
   u32 particles = gabi::load<u32>(game + 0x5AB0);
   gabi::call<void>(0x025A847C, particles, id == 0x40 ? 0u : 5u, id, self + 0xE0, 0u, self + 0xEC, 255u, id == 0x40 ? 0u : 0x1047B2E4u, -1, 0u, 0u, 0u);
  }
 } else {
  u32 game = gabi::call<u32>(0x025200D4);
  u32 particles = gabi::load<u32>(game + 0x5AB0);
  gabi::call<void>(0x025A866C, particles, 0x200Au, self + 0xE0, 0u, 0u, 255u);
  for (u32 pass = 0; pass < 2; pass++) {
   game = gabi::call<u32>(0x025200D4);
   particles = gabi::load<u32>(game + 0x5AB0);
   gabi::call<void>(0x025A847C, particles, pass == 0 ? 3u : 1u, pass == 0 ? 0x2041u : 0x3Cu, self + 0xE0, 0u, 0u, 255u, 0u, -1, 0u, 0u, 0u);
  }
 }
 gabi::Local<GroundLocal> ground;
 gabi::call<void>(0x02008E0C, ground.get());
 u32 g = gabi::ea(ground.get());
 for (u32 i = 0; i < 3; i++) gabi::store<u32>(g + 36 + i * 4, gabi::load<u32>(self + 0xE0 + i * 4));
 for (u32 i = 69; i < 75; i++) gabi::store<u8>(g + i, 0);
 gabi::store<u32>(g, g + 64); gabi::store<u32>(g + 4, g + 76);
 gabi::store<u32>(g + 16, 0x10056BBC); gabi::store<u32>(g + 32, 0x10056BCC);
 gabi::store<u32>(g + 64, 0x10056BEC); gabi::store<u8>(g + 68, 1);
 gabi::store<u32>(g + 76, 0x10056BDC); gabi::store<u32>(g + 80, 1);
 u32 game = gabi::call<u32>(0x025200D4);
 f32 y = gabi::call<f32>(0x02008974, game + 0x12A0, ground.get());
 s32 room; u8 color;
 if (y == constant(0x10056C58)) { room = gabi::load<s8>(0x1047E6C8); color = 255; }
 else {
  game = gabi::call<u32>(0x025200D4);
  room = gabi::call<s32>(0x024EF130, game + 0x12A0, g + 20);
  game = gabi::call<u32>(0x025200D4);
  color = gabi::call<u32>(0x024EEEB8, game + 0x12A0, g + 20);
 }
 gabi::call<void>(0x0255FFF4, self + 0x204, s32(s8(room)), color);
 gabi::store<u32>(g + 32, 0x10056B8C);
 gabi::store<u32>(g + 64, 0x10056BAC);
 gabi::store<u32>(g + 76, 0x10056B6C);
 gabi::call<void>(0x02008DAC, ground.get(), 0u);
 return 4;
}
VERIFY(0x025D2EB0, Create);
static s32 CreateWrapper(u32 self) { WWHD_FUNC(0x025D38B8, s32, self); return Create(self); }
VERIFY(0x025D38B8, CreateWrapper);

/* 025D38BC __sinit: the TU's header-statics initializer. The header statics block at
 * 10487374 (zeroed 16-byte object at +0xC, {-pi, pi} from 10056C98, two one-byte objects at +8/+9),
 * each registered for destruction (records 101F2F80, +0xC, +0x18). */
static void __sinit_d_wpillar_cpp() {
    WWHD_FUNC(0x025D38BC, void);
    const u32 bss = 0x10487374, rec = 0x101F2F80, ro = 0x10056C98;
    gabi::store<u32>(bss + 0x14, 0); gabi::store<u32>(bss + 0xC, 0); gabi::store<u32>(bss + 0x18, 0); gabi::store<u32>(bss + 0x10, 0);
    gabi::call(0x028F026C, rec);
    f32 lo = gabi::load<f32>(ro), hi = gabi::load<f32>(ro + 4);
    gabi::store<f32>(bss, lo);
    gabi::store<f32>(bss + 4, hi);
    gabi::call(0x028ED6F8, bss + 8);
    gabi::call(0x028F026C, rec + 0xC);
    gabi::call(0x028EAB2C, bss + 9);
    gabi::call(0x028F026C, rec + 0x18);
}
VERIFY(0x025D38BC, __sinit_d_wpillar_cpp);

}

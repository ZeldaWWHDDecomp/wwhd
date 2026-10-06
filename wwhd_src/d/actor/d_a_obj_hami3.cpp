// WWHD Hami3 rotating grate. Derived game code:
// TU boundary: 0234EC78..0234F910; all 27 entry points are verified.
// HD changes the GC joint-name scan to the model-data name lookup and uses
// relative name-table pointers. The callback marks animation matrices dirty.
// The grate rotates about Y in the joint callback; its actor X angle remains
// home.angle.x + rotation. Keep the call/reload order for clobbering callees.
#include "bindings.h"
namespace daObjHami3 {
struct Act_c : fopAc_ac_c {
  u8 pad[0x3e0 - sizeof(fopAc_ac_c)];
  be<s16> rotation;
  be<s16> unused;
  u8 phase[8];
  gptr<void> model;
  be<s32> state;
  be<s16> event;
  be<s16> unused2;
};
WWHD_OFFSET(Act_c, rotation, 0x3e0);
WWHD_OFFSET(Act_c, model, 0x3ec);
WWHD_OFFSET(Act_c, state, 0x3f0);
// Local bindings use addresses (kept local to this unit).
static u32 rd(u32 a) { return gabi::load<u32>(a); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
// Snapshot all FPR-loaded values before any store, preserving overlap and SNaNs.
static void matrix_copy(u32 src, u32 dst) {
  f32 v[12];
  for (int i = 0; i < 12; ++i)
    v[i] = gabi::load<f32>(src + 4 * i);
  for (int i = 0; i < 12; ++i)
    gabi::store<f32>(dst + 4 * i, v[i]);
}
static void order(Act_c *a) {
  gabi::call(0x025D7A58, a, (s16)a->event, 0xff, 0xffff, 0, 1);
}
static s32 parameter(Act_c *a, u32 width, u32 shift) {
  WWHD_FUNC(0x0234F8F8, s32, a, width, shift);
  // PPC variable shifts use the low six bits and yield zero for 32..63.
  u32 w = width & 63, s = shift & 63;
  return ((s < 32 ? rd(gabi::ea(a) + 0xb0) >> s : 0) &
          ((w < 32 ? 1u << w : 0) - 1));
}
VERIFY(0x0234F8F8, parameter);
static BOOL nodeCallBack(void *node, s32 timing) {
  WWHD_FUNC(0x0234EC78, BOOL, node, timing);
  if (timing == 0) {
    u32 joint = gabi::call<u32>(0x027F7878, node);
    u32 model = rd(0x104B462C), actor = rd(model + 0xb8);
    u32 index = gabi::load<u16>(joint + 4) * 0x30;
    if (actor) {
      u32 matrices = rd(model + 0x2c);
      auto flags = gabi::load<u16>(matrices + 4);
      u32 base = rd(matrices + 0x10);
      gabi::store<u16>(matrices + 4, flags | 0x10);
      gabi::call(0x028E90D4, base + index, rd(0x1018C7B0));
      u32 calc = rd(0x1018C7B0);
      s16 angle = gabi::load<s16>(actor + 0x3e0);
      gabi::call(0x025F1C28, calc, angle);
      matrices = rd(model + 0x2c);
      flags = gabi::load<u16>(matrices + 4);
      calc = rd(0x1018C7B0);
      base = rd(matrices + 0x10);
      gabi::store<u16>(matrices + 4, flags | 0x10);
      matrix_copy(calc, base + index);
      gabi::call(0x028E90D4, rd(0x1018C7B0), 0x104B4868u);
    }
  }
  return 1;
}
VERIFY(0x0234EC78, nodeCallBack);
s32 Mthd_Create(Act_c *a) {
  WWHD_FUNC(0x0234ED9C, s32, a);
  u32 self = gabi::ea(a), flags = rd(self + 0x2e4);
  if (!(flags & 8)) {
    if (self) {
      gabi::call(0x024F1D40, a);
      flags = rd(self + 0x2e4);
      gabi::store<u32>(self + 0xb4, 0x10029DA0);
    }
    gabi::store<u32>(self + 0x2e4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, self + 0x3e4, 0x10029D98u);
  if (phase == 4) {
    phase = gabi::call<s32>(0x024F1D9C, a, 0x10029D98u, 7, 0x024EE658u, 0x1fc0);
    if (phase != 4 && phase != 5)
      gabi::call(0x0273AA24, 0x10029CF4u, 0xbf, 0x10029D08u);
  }
  return phase;
}
VERIFY(0x0234ED9C, Mthd_Create);
BOOL Mthd_Delete(Act_c *a) {
  WWHD_FUNC(0x0234EE70, BOOL, a);
  BOOL result = gabi::call<BOOL>(0x024F1F64, a);
  gabi::call(0x025204C8, gabi::ea(a) + 0x3e4, 0x10029D98u);
  return result;
}
VERIFY(0x0234EE70, Mthd_Delete);
BOOL CreateHeap(Act_c *a) {
  WWHD_FUNC(0x0234EEBC, BOOL, a);
  gabi::Local<SafeString> name;
  name->mStringTop = 0x10029D98;
  name->__vtbl = 0x10029CD4;
  u32 data = gabi::call<u32>(0x026066C4, rd(0x101F4F28), name.get(), 4);
  if (!data)
    gabi::call(0x0273AA24, 0x10029D54u, 0x71, 0x10029D68u);
  u32 model = gabi::call<u32>(0x025E38E0, data, 0, 0x11020203u);
  a->model = gabi::at<void>(model);
  if (!model)
    return 0;
  u32 table = gabi::call<u32>(0x027F68FC, rd(model + 0xac));
  u32 offset = rd(table + 0x10);
  u32 names = offset ? table + 0x10 + offset : 0;
  s32 index = gabi::call<s32>(0x027DF9B0, names, 0x10029D4Cu);
  if (index >= 0) {
    model = gabi::ea(a->model.get());
    data = rd(model + 0xac);
    u32 count = rd(data + 4), joints = rd(data + 8), i = (u16)index;
    if (i < count)
      joints += i * 0x1c;
    gabi::store<u32>(joints + 8, 0x0234EC78);
  }
  model = gabi::ea(a->model.get());
  gabi::store<u32>(model + 0xb8, gabi::ea(a));
  return 1;
}
VERIFY(0x0234EEBC, CreateHeap);
void set_mtx(Act_c *a) {
  WWHD_FUNC(0x0234EFD4, void, a);
  u32 self = gabi::ea(a);
  f32 x = gabi::load<f32>(self + 0x314), z = gabi::load<f32>(self + 0x31c),
      y = gabi::load<f32>(self + 0x318);
  gabi::call(0x028E93CC, 0x1048D0CCu, x, y, z);
  s16 ax = gabi::load<s16>(self + 0x328), az = gabi::load<s16>(self + 0x32c),
      ay = gabi::load<s16>(self + 0x32a);
  gabi::call(0x025F1B48, 0x1048D0CCu, ax, ay, az);
  matrix_copy(0x1048D0CC, gabi::ea(a->model.get()) + 0xc8);
  gabi::call(0x028E90D4, 0x1048D0CCu, 0x10469D68u);
}
VERIFY(0x0234EFD4, set_mtx);
void init_mtx(Act_c *a) {
  WWHD_FUNC(0x0234F0A8, void, a);
  u32 model = gabi::ea(a->model.get()), self = gabi::ea(a);
  f32 y = gabi::load<f32>(self + 0x334), x = gabi::load<f32>(self + 0x330),
      z = gabi::load<f32>(self + 0x338);
  gabi::store<f32>(model + 0xbc, x);
  gabi::store<f32>(model + 0xc0, y);
  gabi::store<f32>(model + 0xc4, z);
  set_mtx(a);
}
VERIFY(0x0234F0A8, init_mtx);
BOOL Create(Act_c *a) {
  WWHD_FUNC(0x0234F0C8, BOOL, a);
  u32 self = gabi::ea(a), model = gabi::ea(a->model.get());
  gabi::store<u32>(self + 0x348, model ? model + 0xc8 : 0);
  s32 sw = parameter(a, 8, 0);
  u32 save = rd(0x101F84DC);
  s8 room = gabi::load<s8>(self + 0x2fe);
  BOOL on = gabi::call<BOOL>(0x025BA0C0, save + 0x20, sw, room);
  a->state = on ? 3 : 0;
  s16 home = gabi::load<s16>(self + 0x2f8);
  a->rotation = on ? 0x4000 : 0;
  gabi::store<s16>(self + 0x328, home + (on ? 0x4000 : 0));
  init_mtx(a);
  gabi::call(0x025D674C, a, -1200.f, -1200.f, -1200.f, 1200.f, 1200.f, 1200.f);
  u32 p = play();
  a->event = gabi::call<s32>(0x02543F10, p + 0x52c4, 0x10029D84u, 0xff);
  return 1;
}
VERIFY(0x0234F0C8, Create);
void close_stop(Act_c *a) {
  WWHD_FUNC(0x0234F210, void, a);
  s32 sw = parameter(a, 8, 0);
  s8 room = gabi::load<s8>(gabi::ea(a) + 0x2fe);
  u32 save = rd(0x101F84DC);
  if (gabi::call<BOOL>(0x025BA0C0, save + 0x20, sw, room)) {
    order(a);
    a->state = 1;
  }
}
VERIFY(0x0234F210, close_stop);
void open_demo_wait(Act_c *a) {
  WWHD_FUNC(0x0234F290, void, a);
  if (gabi::load<u16>(gabi::ea(a) + 0xf8) == 2) {
    if (parameter(a, 1, 16) == 0)
      gabi::call(0x025E1988, 0x806);
    a->state = 2;
  } else
    order(a);
}
VERIFY(0x0234F290, open_demo_wait);
static void shock_reset() {
  u32 p = play();
  gabi::Local<cXyz> direction;
  direction->set(0.f, 1.f, 0.f);
  gabi::call(0x025CB374, p + 0x599c, 4, -0x21, direction.get());
  p = play();
  gabi::store<u16>(p + 0x52b8, gabi::load<u16>(p + 0x52b8) | 8);
}
static void sound(Act_c *a, s8 room) {
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call(0x025E1A40, 0x61cb, gabi::ea(a) + 0x314, 0, reverb);
}
void open_demo(Act_c *a) {
  WWHD_FUNC(0x0234F318, void, a);
  s16 angle = a->rotation;
  s8 room = gabi::load<s8>(gabi::ea(a) + 0x326);
  a->rotation = angle + 0x100;
  sound(a, room);
  if ((s16)a->rotation >= 0x4000) {
    a->rotation = 0x4000;
    shock_reset();
    s32 sw = parameter(a, 8, 8);
    room = gabi::load<s8>(gabi::ea(a) + 0x2fe);
    u32 save = rd(0x101F84DC);
    gabi::call(0x025B9E38, save + 0x20, sw, room);
    a->state = 3;
  }
}
VERIFY(0x0234F318, open_demo);
void open_stop(Act_c *a) {
  WWHD_FUNC(0x0234F3F8, void, a);
  s32 sw = parameter(a, 8, 0);
  u32 save = rd(0x101F84DC);
  s8 room = gabi::load<s8>(gabi::ea(a) + 0x2fe);
  if (!gabi::call<BOOL>(0x025BA0C0, save + 0x20, sw, room)) {
    order(a);
    a->state = 4;
  }
}
VERIFY(0x0234F3F8, open_stop);
void close_demo_wait(Act_c *a) {
  WWHD_FUNC(0x0234F478, void, a);
  if (gabi::load<u16>(gabi::ea(a) + 0xf8) == 2)
    a->state = 5;
  else
    order(a);
}
VERIFY(0x0234F478, close_demo_wait);
void close_demo(Act_c *a) {
  WWHD_FUNC(0x0234F4AC, void, a);
  s8 room = gabi::load<s8>(gabi::ea(a) + 0x326);
  s16 angle = a->rotation;
  a->rotation = angle - 0x100;
  sound(a, room);
  if ((s16)a->rotation <= 0) {
    a->rotation = 0;
    shock_reset();
    a->state = 0;
  }
}
VERIFY(0x0234F4AC, close_demo);
BOOL Execute(Act_c *a, u32 *output) {
  WWHD_FUNC(0x0234F564, BOOL, a, output);
  switch ((s32)a->state) {
  case 0:
    close_stop(a);
    break;
  case 1:
    open_demo_wait(a);
    break;
  case 2:
    open_demo(a);
    break;
  case 3:
    open_stop(a);
    break;
  case 4:
    close_demo_wait(a);
    break;
  case 5:
    close_demo(a);
    break;
  }
  s16 angle = a->rotation, home = gabi::load<s16>(gabi::ea(a) + 0x2f8);
  gabi::store<s16>(gabi::ea(a) + 0x328, home + angle);
  set_mtx(a);
  gabi::store<u32>(gabi::ea(output), 0x10469D68);
  return 1;
}
VERIFY(0x0234F564, Execute);
BOOL Draw(Act_c *a) {
  WWHD_FUNC(0x0234F724, BOOL, a);
  u32 self = gabi::ea(a);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, env, 1, self + 0x314, self + 0x110);
  env = gabi::call<u32>(0x02555D0C);
  u32 model = gabi::ea(a->model.get());
  gabi::call(0x02562F5C, env, model, self + 0x110);
  u32 p = play();
  gabi::store<u32>(0x104B4634, rd(p + 0x5d70));
  p = play();
  gabi::store<u32>(0x104B4638, rd(p + 0x5d74));
  model = gabi::ea(a->model.get());
  gabi::call(0x025E2DE0, model, 0);
  p = play();
  gabi::store<u32>(0x104B4634, rd(p + 0x5d78));
  p = play();
  gabi::store<u32>(0x104B4638, rd(p + 0x5d7c));
  return 1;
}
VERIFY(0x0234F724, Draw);
BOOL Delete(Act_c *a) {
  WWHD_FUNC(0x0234F89C, BOOL, a);
  return 1;
}
VERIFY(0x0234F89C, Delete);
BOOL create_adapter(Act_c *a) {
  WWHD_FUNC(0x0234F7BC, BOOL, a);
  return Mthd_Create(a);
}
VERIFY(0x0234F7BC, create_adapter);
BOOL delete_adapter(Act_c *a) {
  WWHD_FUNC(0x0234F7C0, BOOL, a);
  return Mthd_Delete(a);
}
VERIFY(0x0234F7C0, delete_adapter);
BOOL execute_adapter(Act_c *a) {
  WWHD_FUNC(0x0234F7C4, BOOL, a);
  return gabi::call<BOOL>(0x024F1E9C, a);
}
VERIFY(0x0234F7C4, execute_adapter);
BOOL draw_adapter(Act_c *a) {
  WWHD_FUNC(0x0234F7C8, BOOL, a);
  return gabi::call<BOOL>(rd(rd(gabi::ea(a) + 0xb4) + 0x2c), a);
}
VERIFY(0x0234F7C8, draw_adapter);
BOOL isdelete_adapter(Act_c *a) {
  WWHD_FUNC(0x0234F7D8, BOOL, a);
  return gabi::call<BOOL>(rd(rd(gabi::ea(a) + 0xb4) + 0x3c), a);
}
VERIFY(0x0234F7D8, isdelete_adapter);
void sinit() {
  WWHD_FUNC(0x0234F7E8, void);
  for (int i = 0; i < 4; ++i)
    gabi::store<u32>(0x10469D58 + 4 * i, 0);
  gabi::call(0x028F026C, 0x101C9BC8u);
  f32 lo = gabi::load<f32>(0x10029D90), hi = gabi::load<f32>(0x10029D94);
  gabi::store<f32>(0x10469D4C, lo);
  gabi::store<f32>(0x10469D50, hi);
  gabi::call(0x028ED6F8, 0x10469D54u);
  gabi::call(0x028F026C, 0x101C9BD4u);
  gabi::call(0x028EAB2C, 0x10469D55u);
  gabi::call(0x028F026C, 0x101C9BE0u);
}
VERIFY(0x0234F7E8, sinit);
void static_destructor(void *a, u32 flags) {
  WWHD_FUNC(0x0234F87C, void, a, flags);
  if (a && (flags & 1))
    gabi::call(0x0273AF40, a);
}
VERIFY(0x0234F87C, static_destructor);
BOOL IsDelete(Act_c *a) {
  WWHD_FUNC(0x0234F890, BOOL, a);
  return 1;
}
VERIFY(0x0234F890, IsDelete);
void noop(void *a) { WWHD_FUNC(0x0234F898, void, a); }
VERIFY(0x0234F898, noop);
void destructor(Act_c *a, u32 flags) {
  WWHD_FUNC(0x0234F8A4, void, a, flags);
  if (a) {
    gabi::call(0x025D50BC, a, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, a);
  }
}
VERIFY(0x0234F8A4, destructor);
} // namespace daObjHami3

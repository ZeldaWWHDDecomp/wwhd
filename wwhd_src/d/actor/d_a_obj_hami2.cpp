/* Hami2 hinged gate. Derived from WWHD disassembly;
 */
#include "gabi.h"
namespace hami2 {
using gabi::call;
using gabi::load;
using gabi::store;
static u32 play() { return call<u32>(0x025200D4); }
static void copyMatrix(u32 src, u32 dst) {
  f32 v[12];
  for (int i = 0; i < 12; ++i)
    v[i] = load<f32>(src + 4 * i);
  for (int i = 0; i < 12; ++i)
    store<f32>(dst + 4 * i, v[i]);
}
u32 PrmAbstract(u32 a, s32 width, s32 shift) {
  WWHD_FUNC(0x0234EC5C, u32, a, width, shift);
  u32 w = (u32)width & 63, s = (u32)shift & 63;
  return (s < 32 ? load<u32>(a + 0xB0) >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}
VERIFY(0x0234EC5C, PrmAbstract);
s32 Mthd_Create(u32 a) {
  WWHD_FUNC(0x0234E09C, s32, a);
  u32 flags = load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (a) {
      call<void>(0x024F1D40, a);
      flags = load<u32>(a + 0x2E4);
      store<u32>(a + 0xB4, 0x10029C80);
    }
    store<u32>(a + 0x2E4, flags | 8);
  }
  s32 phase = call<s32>(0x02520460, a + 0x3E4, 0x10029C78u);
  if (phase == 4) {
    phase = call<s32>(0x024F1D9C, a, 0x10029C78u, 8, 0x024EE708u, 0x34E0);
    u32 p = play();
    u32 bg = load<u32>(a + 0x3F0);
    call<void>(0x024EEA6C, p + 0x12A0, bg, a);
    if (phase != 4 && phase != 5)
      call<void>(0x0273AA24, 0x10029BBCu, 0xD0, 0x10029BD0u);
  }
  return phase;
}
VERIFY(0x0234E09C, Mthd_Create);
s32 Mthd_Delete(u32 a) {
  WWHD_FUNC(0x0234E184, s32, a);
  if (load<u32>(a + 0xF4)) {
    u32 bg = load<u32>(a + 0x3F0);
    if (bg && load<u32>(bg) < 256) {
      u32 p = play();
      call<void>(0x020087EC, p + 0x12A0, load<u32>(a + 0x3F0));
    }
  }
  s32 r = call<s32>(0x024F1F64, a);
  call<void>(0x025204C8, a + 0x3E4, 0x10029C78u);
  return r;
}
VERIFY(0x0234E184, Mthd_Delete);
s32 CreateHeap(u32 a) {
  WWHD_FUNC(0x0234E208, s32, a);
  gabi::Local<u32[2]> name;
  store<u32>(name.a, 0x10029C78);
  store<u32>(name.a + 4, 0x10029B9C);
  u32 data = call<u32>(0x026066C4, load<u32>(0x101F4F28), name.a, 4);
  if (!data)
    call<void>(0x0273AA24, 0x10029C1Cu, 0x68, 0x10029C30u);
  u32 model = call<u32>(0x025E38E0, data, 0, 0x11020203u);
  store<u32>(a + 0x3EC, model);
  if (!model)
    return 0;
  u32 table = call<u32>(0x027F68FC, load<u32>(model + 0xAC));
  u32 off = load<u32>(table + 0x10);
  s32 idx = call<s32>(0x027DF9B0, off ? table + 0x10 + off : 0, 0x10029C14u);
  if (idx >= 0) {
    u32 m = load<u32>(a + 0x3EC), joints = load<u32>(m + 0xAC),
        count = load<u32>(joints + 4), ptr = load<u32>(joints + 8);
    u32 i = (u16)idx;
    if (i < count)
      ptr += i * 0x1C;
    store<u32>(ptr + 8, 0x0234DF78);
  }
  store<u32>(load<u32>(a + 0x3EC) + 0xB8, a);
  f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318),
      z = load<f32>(a + 0x31C);
  call<void>(0x028E93CC, 0x1048D0CCu, x, y, z);
  call<void>(0x025F1C28, 0x1048D0CCu, load<s16>(a + 0x32A));
  x = load<f32>(a + 0x330);
  y = load<f32>(a + 0x334);
  z = load<f32>(a + 0x338);
  call<void>(0x025F2518, x, y, z);
  call<void>(0x028E90D4, 0x1048D0CCu, a + 0x3F4);
  u32 bg = call<u32>(0x024F23F4, 0);
  store<u32>(a + 0x3F0, bg);
  if (!bg)
    return 0;
  gabi::Local<u32[2]> name2;
  store<u32>(name2.a, 0x10029C78);
  store<u32>(name2.a + 4, 0x10029B9C);
  data = call<u32>(0x026066C4, load<u32>(0x101F4F28), name2.a, 7);
  s32 r = call<s32>(0x0200A030, load<u32>(a + 0x3F0), data, 1, a + 0x3F4);
  return r == 0;
}
VERIFY(0x0234E208, CreateHeap);
void set_mtx(u32 a) {
  WWHD_FUNC(0x0234E3B0, void, a);
  f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318),
      z = load<f32>(a + 0x31C);
  call<void>(0x028E93CC, 0x1048D0CCu, x, y, z);
  s16 rx = load<s16>(a + 0x328), ry = load<s16>(a + 0x32A),
      rz = load<s16>(a + 0x32C);
  call<void>(0x025F1B48, 0x1048D0CCu, rx, ry, rz);
  copyMatrix(0x1048D0CC, load<u32>(a + 0x3EC) + 0xC8);
  call<void>(0x025F1C28, 0x1048D0CCu, load<s16>(a + 0x3E0));
  call<void>(0x028E90D4, 0x1048D0CCu, 0x10469D1Cu);
}
VERIFY(0x0234E3B0, set_mtx);
void init_mtx(u32 a) {
  WWHD_FUNC(0x0234E490, void, a);
  u32 model = load<u32>(a + 0x3EC);
  f32 x = load<f32>(a + 0x330), y = load<f32>(a + 0x334),
      z = load<f32>(a + 0x338);
  store<f32>(model + 0xBC, x);
  store<f32>(model + 0xC0, y);
  store<f32>(model + 0xC4, z);
  set_mtx(a);
}
VERIFY(0x0234E490, init_mtx);
static s32 switchState(u32 a) {
  u32 sw = PrmAbstract(a, 8, 0);
  u32 save = load<u32>(0x101F84DC);
  return call<s32>(0x025BA0C0, save + 0x20, sw, (s8)load<u8>(a + 0x2FE));
}
s32 Create(u32 a) {
  WWHD_FUNC(0x0234E4B0, s32, a);
  u32 m = load<u32>(a + 0x3EC);
  store<u32>(a + 0x348, m ? m + 0xC8 : 0);
  bool open = switchState(a) != 0;
  store<u32>(a + 0x424, open ? 3 : 0);
  store<s16>(a + 0x3E0, open ? 0x4000 : 0);
  init_mtx(a);
  call<void>(0x025D674C, a, -1200.0f, -40000.0f, -1200.0f, 1200.0f, 40000.0f,
             1200.0f);
  u32 p = play();
  s32 e = call<s32>(0x02543F10, p + 0x52C4, 0x10029C54u, 255);
  store<s16>(a + 0x428, e);
  p = play();
  e = call<s32>(0x02543F10, p + 0x52C4, 0x10029C60u, 255);
  store<s16>(a + 0x42A, e);
  return 1;
}
VERIFY(0x0234E4B0, Create);
static void order(u32 a, u32 off) {
  call<void>(0x025D7A58, a, load<s16>(a + off), 255, 65535, 0, 1);
}
void close_stop(u32 a) {
  WWHD_FUNC(0x0234E628, void, a);
  if (switchState(a)) {
    order(a, 0x428);
    store<u32>(a + 0x424, 1);
  }
}
VERIFY(0x0234E628, close_stop);
void open_stop(u32 a) {
  WWHD_FUNC(0x0234E7D8, void, a);
  if (!switchState(a)) {
    order(a, 0x42A);
    store<u32>(a + 0x424, 4);
  }
}
VERIFY(0x0234E7D8, open_stop);
void open_demo_wait(u32 a) {
  WWHD_FUNC(0x0234E6A8, void, a);
  if (load<u16>(a + 0xF8) == 2) {
    s32 room = (s8)load<u8>(a + 0x326);
    store<u32>(a + 0x424, 2);
    s32 reverb = call<s32>(0x02520540, room);
    call<void>(0x025E1A40, 0x69C4, a + 0x314, 0, reverb);
    call<void>(0x025E1988, 0x806);
  } else
    order(a, 0x428);
}
VERIFY(0x0234E6A8, open_demo_wait);
void close_demo_wait(u32 a) {
  WWHD_FUNC(0x0234E858, void, a);
  if (load<u16>(a + 0xF8) == 2)
    store<u32>(a + 0x424, 5);
  else
    order(a, 0x42A);
}
VERIFY(0x0234E858, close_demo_wait);
static void shock() {
  u32 p = play();
  gabi::Local<f32[3]> direction;
  store<f32>(direction.a, 0);
  store<f32>(direction.a + 4, 1);
  store<f32>(direction.a + 8, 0);
  call<void>(0x025CB374, p + 0x599C, 4, -33, direction.a);
  p = play();
  store<u16>(p + 0x52B8, load<u16>(p + 0x52B8) | 8);
}
void open_demo(u32 a) {
  WWHD_FUNC(0x0234E740, void, a);
  s16 rot = (s16)(load<s16>(a + 0x3E0) + 0x100);
  if (rot < 0x4000)
    store<s16>(a + 0x3E0, rot);
  else {
    store<s16>(a + 0x3E0, 0x4000);
    store<u32>(a + 0x424, 3);
    shock();
  }
}
VERIFY(0x0234E740, open_demo);
void close_demo(u32 a) {
  WWHD_FUNC(0x0234E88C, void, a);
  s16 rot = (s16)(load<s16>(a + 0x3E0) - 0x100);
  if (rot > 0)
    store<s16>(a + 0x3E0, rot);
  else {
    store<s16>(a + 0x3E0, 0);
    shock();
    store<u32>(a + 0x424, 0);
  }
}
VERIFY(0x0234E88C, close_demo);
s32 Execute(u32 a, u32 out) {
  WWHD_FUNC(0x0234E938, s32, a, out);
  switch (load<u32>(a + 0x424)) {
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
  set_mtx(a);
  store<u32>(out, 0x10469D1C);
  return 1;
}
VERIFY(0x0234E938, Execute);
s32 Draw(u32 a) {
  WWHD_FUNC(0x0234EA88, s32, a);
  u32 env = call<u32>(0x02555D0C);
  call<void>(0x025626A4, env, 1, a + 0x314, a + 0x110);
  env = call<u32>(0x02555D0C);
  call<void>(0x02562F5C, env, load<u32>(a + 0x3EC), a + 0x110);
  u32 p = play();
  store<u32>(0x104B4634, load<u32>(p + 0x5D70));
  p = play();
  store<u32>(0x104B4638, load<u32>(p + 0x5D74));
  call<void>(0x025E2DE0, load<u32>(a + 0x3EC), 0);
  p = play();
  store<u32>(0x104B4634, load<u32>(p + 0x5D78));
  p = play();
  store<u32>(0x104B4638, load<u32>(p + 0x5D7C));
  return 1;
}
VERIFY(0x0234EA88, Draw);
s32 Delete(u32 a) {
  WWHD_FUNC(0x0234EC00, s32, a);
  return 1;
}
VERIFY(0x0234EC00, Delete);
s32 nodeCallback(u32 joint, s32 phase) {
  WWHD_FUNC(0x0234DF78, s32, joint, phase);
  if (phase == 0) {
    u32 j = call<u32>(0x027F7878, joint), model = load<u32>(0x104B462C),
        actor = load<u32>(model + 0xB8);
    u32 idx = load<u16>(j + 4);
    if (actor) {
      u32 block = load<u32>(model + 0x2C);
      u32 flags = load<u16>(block + 4), matrices = load<u32>(block + 0x10);
      store<u16>(block + 4, flags | 16);
      u32 stack = load<u32>(0x1018C7B0);
      call<void>(0x028E90D4, matrices + idx * 48, stack);
      stack = load<u32>(0x1018C7B0);
      call<void>(0x025F1C28, stack, load<s16>(actor + 0x3E0));
      block = load<u32>(model + 0x2C);
      flags = load<u16>(block + 4);
      stack = load<u32>(0x1018C7B0);
      matrices = load<u32>(block + 0x10);
      store<u16>(block + 4, flags | 16);
      copyMatrix(stack, matrices + idx * 48);
      call<void>(0x028E90D4, load<u32>(0x1018C7B0), 0x104B4868u);
    }
  }
  return 1;
}
VERIFY(0x0234DF78, nodeCallback);
s32 wrapperCreate(u32 a) {
  WWHD_FUNC(0x0234EB20, s32, a);
  return Mthd_Create(a);
}
VERIFY(0x0234EB20, wrapperCreate);
s32 wrapperDelete(u32 a) {
  WWHD_FUNC(0x0234EB24, s32, a);
  return Mthd_Delete(a);
}
VERIFY(0x0234EB24, wrapperDelete);
s32 wrapperExecute(u32 a) {
  WWHD_FUNC(0x0234EB28, s32, a);
  return call<s32>(0x024F1E9C, a);
}
VERIFY(0x0234EB28, wrapperExecute);
s32 wrapperIsDelete(u32 a) {
  WWHD_FUNC(0x0234EB2C, s32, a);
  return call<s32>(load<u32>(load<u32>(a + 0xB4) + 0x2C), a);
}
VERIFY(0x0234EB2C, wrapperIsDelete);
s32 wrapperDraw(u32 a) {
  WWHD_FUNC(0x0234EB3C, s32, a);
  return call<s32>(load<u32>(load<u32>(a + 0xB4) + 0x3C), a);
}
VERIFY(0x0234EB3C, wrapperDraw);
void staticInit() {
  WWHD_FUNC(0x0234EB4C, void);
  store<u32>(0x10469D14, 0);
  store<u32>(0x10469D0C, 0);
  store<u32>(0x10469D18, 0);
  store<u32>(0x10469D10, 0);
  call<void>(0x028F026C, 0x101C9B54u);
  store<f32>(0x10469D00, -3.1415927410125732f);
  store<f32>(0x10469D04, 3.1415927410125732f);
  call<void>(0x028ED6F8, 0x10469D08u);
  call<void>(0x028F026C, 0x101C9B60u);
  call<void>(0x028EAB2C, 0x10469D09u);
  call<void>(0x028F026C, 0x101C9B6Cu);
}
VERIFY(0x0234EB4C, staticInit);
void staticDtor(u32 a, s32 flags) {
  WWHD_FUNC(0x0234EBE0, void, a, flags);
  if (a && (flags & 1))
    call<void>(0x0273AF40, a);
}
VERIFY(0x0234EBE0, staticDtor);
void actorDtor(u32 a, s32 flags) {
  WWHD_FUNC(0x0234EC08, void, a, flags);
  if (a) {
    call<void>(0x025D50BC, a, 0);
    if (flags & 1)
      call<void>(0x0273AF40, a);
  }
}
VERIFY(0x0234EC08, actorDtor);
} // namespace hami2

/* ---- leftover functions of the translation unit ---- */

/* 0234EBF4 dBgS_MoveBgActor::IsDelete (out-of-line copy; daObjHami2::Act_c vtable slot 10029CBC) */
static BOOL hami2_MoveBgActor_IsDelete(void* p) {
    WWHD_FUNC(0x0234EBF4, BOOL, p);
    return TRUE;
}
VERIFY(0x0234EBF4, hami2_MoveBgActor_IsDelete);

/* 0234EBFC sead::SafeStringBase<char>::assureTerminationImpl_ (empty); vtable slot 10029BB0, after the destructor 0234EBE0 */
static void hami2_SafeString_assureTermination(void* p) {
    WWHD_FUNC(0x0234EBFC, void, p);
}
VERIFY(0x0234EBFC, hami2_SafeString_assureTermination);

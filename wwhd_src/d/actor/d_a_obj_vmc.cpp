/* Soft soil mound and Makar tree, ported from GameCube source and HD
 * disassembly. */
#include "gabi.h"
namespace vmc {
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
static u32 resource(s32 index) {
  gabi::Local<u32[2]> name;
  store<u32>(name.a, 0x10032E60);
  store<u32>(name.a + 4, 0x10032D48);
  return call<u32>(0x026066C4, load<u32>(0x101F4F28), name.a, index);
}
static void assertResource(u32 ptr, s32 line, u32 msg) {
  if (!ptr)
    call<void>(0x0273AA24, 0x10032DACu, line, msg);
}
u32 PrmAbstract(u32 a, s32 width, s32 shift) {
  WWHD_FUNC(0x023B3450, u32, a, width, shift);
  u32 w = (u32)width & 63, s = (u32)shift & 63;
  return (s < 32 ? load<u32>(a + 0xB0) >> s : 0) & ((w < 32 ? 1u << w : 0) - 1);
}
VERIFY(0x023B3450, PrmAbstract);
bool create_heap(u32 a) {
  WWHD_FUNC(0x023B2294, bool, a);
  u32 base = resource(10);
  assertResource(base, 200, 0x10032DD4);
  u32 model = call<u32>(0x025E38E0, base, 0, 0x11020203u);
  store<u32>(a + 0x3BC, model);
  if (!model)
    return false;
  u32 tree = resource(13);
  assertResource(tree, 207, 0x10032DE8);
  model = call<u32>(0x025E38E0, tree, 0, 0x11020203u);
  store<u32>(a + 0x3C0, model);
  if (!model)
    return false;
  u32 grow = resource(6);
  assertResource(grow, 215, 0x10032DBC);
  s32 ok = call<s32>(0x025E8508, a + 0x3C4, tree, grow, 1, 0, 0, -1, 0, 1.0f);
  store<f32>(a + 0x3C4, 0.75f);
  if (!ok)
    return false;
  u32 hook = resource(7);
  assertResource(hook, 226, 0x10032DC8);
  ok = call<s32>(0x025E8508, a + 0x450, tree, hook, 1, 0, 0, -1, 0, 1.0f);
  if (!ok)
    return false;
  f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318),
      z = load<f32>(a + 0x31C);
  call<void>(0x028E93CC, 0x1048D0CCu, x, y, z);
  call<void>(0x025F1C28, 0x1048D0CCu, load<s16>(a + 0x32A));
  call<void>(0x028E90D4, 0x1048D0CCu, a + 0x648);
  call<void>(0x028E90D4, 0x1048D0CCu, a + 0x678);
  bool success = true;
  u32 bg = call<u32>(0x024F23F4, 0);
  store<u32>(a + 0x3B4, bg);
  if (!bg)
    success = false;
  else {
    u32 mesh = resource(16);
    s32 r = call<s32>(0x0200A030, load<u32>(a + 0x3B4), mesh, 1, a + 0x648);
    if (r)
      success = false;
  }
  bg = call<u32>(0x024F23F4, 0);
  store<u32>(a + 0x3B8, bg);
  if (!bg)
    return false;
  u32 mesh = resource(17);
  s32 r = call<s32>(0x0200A030, load<u32>(a + 0x3B8), mesh, 1, a + 0x678);
  return r == 0 && success;
}
VERIFY(0x023B2294, create_heap);
bool solidHeapCB(u32 a) {
  WWHD_FUNC(0x023B2574, bool, a);
  return create_heap(a);
}
VERIFY(0x023B2574, solidHeapCB);
void set_mtx(u32 a) {
  WWHD_FUNC(0x023B2578, void, a);
  f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318),
      z = load<f32>(a + 0x31C);
  call<void>(0x028E93CC, 0x1048D0CCu, x, y, z);
  s16 rx = load<s16>(a + 0x320), ry = load<s16>(a + 0x322),
      rz = load<s16>(a + 0x324);
  call<void>(0x025F1B48, 0x1048D0CCu, rx, ry, rz);
  copyMatrix(0x1048D0CC, load<u32>(a + 0x3BC) + 0xC8);
  x = load<f32>(a + 0x314);
  y = load<f32>(a + 0x318);
  z = load<f32>(a + 0x31C);
  call<void>(0x028E93CC, 0x1048D0CCu, x, y, z);
  rx = load<s16>(a + 0x328);
  ry = load<s16>(a + 0x32A);
  rz = load<s16>(a + 0x32C);
  call<void>(0x025F1B48, 0x1048D0CCu, rx, ry, rz);
  copyMatrix(0x1048D0CC, load<u32>(a + 0x3C0) + 0xC8);
}
VERIFY(0x023B2578, set_mtx);
void init_mtx(u32 a) {
  WWHD_FUNC(0x023B26C8, void, a);
  u32 model = load<u32>(a + 0x3BC);
  f32 x = load<f32>(a + 0x330), y = load<f32>(a + 0x334),
      z = load<f32>(a + 0x338);
  store<f32>(model + 0xBC, x);
  store<f32>(model + 0xC0, y);
  store<f32>(model + 0xC4, z);
  model = load<u32>(a + 0x3C0);
  x = load<f32>(a + 0x330);
  y = load<f32>(a + 0x334);
  z = load<f32>(a + 0x338);
  store<f32>(model + 0xC4, z);
  store<f32>(model + 0xBC, x);
  store<f32>(model + 0xC0, y);
  set_mtx(a);
}
VERIFY(0x023B26C8, init_mtx);
void CreateInit(u32 a) {
  WWHD_FUNC(0x023B2704, void, a);
  u32 model = load<u32>(a + 0x3BC);
  store<u32>(a + 0x348, model ? model + 0xC8 : 0);
  call<void>(0x025D674C, a, -300.0f, -0.0f, -300.0f, 300.0f, 500.0f, 300.0f);
  call<void>(0x02515F14, a + 0x4DC, 255, 255, a);
  call<void>(0x02516518, a + 0x518, 0x101CDA04u);
  store<u32>(a + 0x55C, a + 0x4DC);
  u32 p = play();
  call<void>(0x024EEA6C, p + 0x12A0, load<u32>(a + 0x3B4), a);
  u32 sw = PrmAbstract(a, 8, 8);
  s32 room = (s8)load<u8>(a + 0x2FE);
  u32 save = load<u32>(0x101F84DC);
  if (call<s32>(0x025BA0C0, save + 0x20, sw, room)) {
    p = play();
    s32 r = call<s32>(0x024EEA6C, p + 0x12A0, load<u32>(a + 0x3B8), a);
    if (!r)
      store<u8>(a + 0x6D5, 1);
    f32 end = (f32)load<s16>(a + 0x3CE);
    store<u8>(a + 0x6D6, 1);
    store<s32>(a + 0x6CC, 3);
    store<f32>(a + 0x3C8, end);
  } else {
    store<s32>(a + 0x6CC, 0);
    store<u8>(a + 0x6D6, 0);
  }
  init_mtx(a);
  f32 x = load<f32>(a + 0x314), y = load<f32>(a + 0x318) + 40.0f,
      z = load<f32>(a + 0x31C);
  store<f32>(a + 0x390, x);
  store<f32>(a + 0x394, y);
  store<f32>(a + 0x398, z);
  store<f32>(a + 0x37C, x);
  store<f32>(a + 0x380, y);
  store<f32>(a + 0x384, z);
  store<u8>(a + 0x389, 0xA7);
  store<u8>(a + 0x38B, 0xA7);
  store<u8>(a + 0x6D4, 0);
  store<u8>(a + 0x6B9, 0);
  store<u8>(a + 0x6C8, 1);
  call<void>(0x0253E9B0, a + 0xF8, 0x10032E1Cu);
}
VERIFY(0x023B2704, CreateInit);
s32 create(u32 a) {
  WWHD_FUNC(0x023B2928, s32, a);
  u32 flags = load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (a) {
      call<void>(0x025D4ED0, a);
      store<u32>(a + 0xB4, 0x10032D98);
      call<void>(0x027F2BC0, a + 0x3C4, 0);
      store<u32>(a + 0x3D4, 0x1016E54C);
      call<void>(0x027DA984, a + 0x3D8);
      store<u32>(a + 0x44C, 0);
      store<u32>(a + 0x41C, 0);
      store<u32>(a + 0x440, 0);
      store<u32>(a + 0x448, 0);
      store<u32>(a + 0x40C, 0x1016D820);
      store<u32>(a + 0x444, 0);
      store<u32>(a + 0x3D4, 0x10032D70);
      call<void>(0x027F2BC0, a + 0x450, 0);
      store<u32>(a + 0x460, 0x1016E54C);
      call<void>(0x027DA984, a + 0x464);
      store<u32>(a + 0x460, 0x10032D70);
      store<u32>(a + 0x4D0, 0);
      store<u32>(a + 0x4D8, 0);
      store<u32>(a + 0x4D4, 0);
      store<u32>(a + 0x4CC, 0);
      store<u32>(a + 0x498, 0x1016D820);
      store<u32>(a + 0x4A8, 0);
      call<void>(0x0200BD2C, a + 0x4DC);
      call<void>(0x02515DA0, a + 0x4F8);
      store<u32>(a + 0x4F4, 0x1004AE88);
      store<u32>(a + 0x4F8, 0x1004AEC0);
      call<void>(0x02515FB8, a + 0x518);
      store<u32>(a + 0x62C, 0x100015A8);
      store<u32>(a + 0x628, 0x10032D60);
      call<void>(0x02018590, a + 0x630);
      store<u32>(a + 0x554, 0x1004B108);
      store<u32>(a + 0x644, 0x1004B150);
      store<u32>(a + 0x62C, 0x1004B160);
      call<void>(0x025A5B18, a + 0x6A8, 1);
      flags = load<u32>(a + 0x2E4);
    }
    store<u32>(a + 0x2E4, flags | 8);
  }
  store<u8>(a + 0x6D5, 0);
  s32 phase = call<s32>(0x02520460, a + 0x3AC, 0x10032E60u);
  if (phase == 4) {
    if (call<s32>(0x025D63E8, a, 0x023B2574u, 0x32A0))
      CreateInit(a);
    else
      phase = 5;
  }
  return phase;
}
VERIFY(0x023B2928, create);
bool remove(u32 a) {
  WWHD_FUNC(0x023B2ADC, bool, a);
  u32 p = play();
  call<void>(0x020087EC, p + 0x12A0, load<u32>(a + 0x3B4));
  if (load<u8>(a + 0x6D5)) {
    p = play();
    call<void>(0x020087EC, p + 0x12A0, load<u32>(a + 0x3B8));
  }
  u32 vt = load<u32>(a + 0x6A8), fn = load<u32>(vt + 0x44);
  call<void>(fn, a + 0x6A8);
  call<void>(0x025204C8, a + 0x3AC, 0x10032E60u);
  return true;
}
VERIFY(0x023B2ADC, remove);
void base_main(u32 a) {
  WWHD_FUNC(0x023B2B58, void, a);
  if (load<u8>(a + 0x6D6)) {
    s32 room = (s8)load<u8>(a + 0x326);
    u32 p = play(), particles = load<u32>(p + 0x5AB0);
    call<void>(0x025A847C, particles, 2, 0xA1BC, a + 0x314, a + 0x320, 0, 255,
               a + 0x6A8, room, 0, 0, 0);
    u32 flags = load<u32>(a + 0x39C);
    store<u32>(a + 0x6CC, 1);
    store<u32>(a + 0x39C, flags & ~0x10000000u);
    p = play();
    u32 player = load<u32>(p + 0x5B2C);
    s32 angle = call<s32>(0x025D6894, a, player);
    store<s32>(a + 0x6D0, 0);
    store<s16>(a + 0x32A, angle + 0x1800);
  }
}
VERIFY(0x023B2B58, base_main);
void tree_demo_wait(u32 a) {
  WWHD_FUNC(0x023B2C18, void, a);
  store<s32>(a + 0x6CC, 2);
}
VERIFY(0x023B2C18, tree_demo_wait);
void tree_demo_main(u32 a) {
  WWHD_FUNC(0x023B2C24, void, a);
  u32 timer = load<u32>(a + 0x6D0) + 1;
  store<u32>(a + 0x6D0, timer);
  if (timer == 10 || timer == 40) {
    s32 reverb = call<s32>(0x02520540, (s8)load<u8>(a + 0x326));
    call<void>(0x025E1A40, timer == 10 ? 0x6A13 : 0x6A14, a + 0x314, 0, reverb);
  }
  call<void>(0x025E742C, a + 0x3C4);
  if ((load<u8>(a + 0x3D3) & 1) || load<f32>(a + 0x3C4) == 0.0f) {
    store<s32>(a + 0x6CC, 3);
    u32 p = play();
    s32 r = call<s32>(0x024EEA6C, p + 0x12A0, load<u32>(a + 0x3B8), a);
    u32 status = load<u32>(a + 0x2E0);
    if (!r)
      store<u8>(a + 0x6D5, 1);
    store<u32>(a + 0x2E0, status | 0x200000);
    u32 sw = PrmAbstract(a, 8, 8);
    s32 room = (s8)load<u8>(a + 0x2FE);
    u32 save = load<u32>(0x101F84DC);
    call<void>(0x025B9E38, save + 0x20, sw, room);
  }
}
VERIFY(0x023B2C24, tree_demo_main);
void tree_main(u32 a) {
  WWHD_FUNC(0x023B2D50, void, a);
  if (!load<u8>(a + 0x6D4)) {
    if (call<s32>(0x02516464, a + 0x518)) {
      u32 anim = resource(7);
      if (!anim)
        call<void>(0x0273AA24, 0x10032E28u, 429, 0x10032E38u);
      u32 model = load<u32>(a + 0x3C0), data = load<u32>(model + 0xAC);
      call<void>(0x025E8508, a + 0x450, data, anim, 1, 0, 0, -1, 1, 1.0f);
      store<u8>(a + 0x6D4, 1);
    }
  } else {
    if ((load<u8>(a + 0x45F) & 1) || load<f32>(a + 0x450) == 0.0f)
      store<u8>(a + 0x6D4, 0);
    else
      call<void>(0x025E742C, a + 0x450);
  }
  if (call<s32>(0x025162A4, a + 0x518)) {
    u32 hit = call<u32>(0x02516300, a + 0x518);
    if (hit) {
      u32 stts = load<u32>(hit + 0x44);
      if (!stts)
        return;
      u32 actor = load<u32>(stts + 0xC);
      if (actor && load<s16>(actor + 8) == 0xA9)
        call<void>(0x025B8B68, load<u32>(0x101F84DC) + 0x644, 0x3420);
    }
  }
}
VERIFY(0x023B2D50, tree_main);
bool execute(u32 a) {
  WWHD_FUNC(0x023B2EF4, bool, a);
  call<void>(0x020182E0, a + 0x630, a + 0x314);
  u32 state = load<u32>(a + 0x6CC);
  if (state == 0) {
    u32 p = play();
    call<void>(0x0200E240, p + 0x26A4, a + 0x518);
    state = load<u32>(a + 0x6CC);
  }
  switch (state) {
  case 0:
    base_main(a);
    break;
  case 1:
    tree_demo_wait(a);
    break;
  case 2:
    tree_demo_main(a);
    break;
  case 3:
    tree_main(a);
    break;
  }
  set_mtx(a);
  u32 p = play(), player = load<u32>(p + 0x5B34);
  gabi::Local<f32[3]> dist;
  call<void>(0x0201ADE0, player + 0x314, dist.a, a + 0x314);
  bool range = load<u8>(a + 0x6C8) != 0;
  f32 x = load<f32>(dist.a), z = load<f32>(dist.a + 8);
  gabi::Local<f32[3]> xz;
  store<f32>(xz.a, x);
  store<f32>(xz.a + 4, 0.0f);
  store<f32>(xz.a + 8, z);
  f32 mag = call<f32>(0x028E8DD0, xz.a);
  f64 distance = call<f64>(0x028F4384, (f64)mag);
  if (!range) {
    bool away = distance > 110.0;
    if (!away) {
      f32 y = load<f32>(dist.a + 4);
      away = y < -500.0f || y > 500.0f;
    }
    if (away) {
      store<u8>(a + 0x6C8, 1);
      range = true;
    } else
      range = load<u8>(a + 0x6C8) != 0;
  } else {
    if (distance < 100.0) {
      f32 y = load<f32>(dist.a + 4);
      if (y > -490.0f && y < 490.0f) {
        store<u8>(a + 0x6C8, 0);
        range = false;
      } else
        range = load<u8>(a + 0x6C8) != 0;
    } else
      range = load<u8>(a + 0x6C8) != 0;
  }
  if (!range) {
    store<u32>(a + 0x39C, load<u32>(a + 0x39C) & ~0x1000000Au);
    return true;
  }
  p = play();
  u32 cb = load<u32>(p + 0x5B38);
  if (cb) {
    p = play();
    cb = load<u32>(p + 0x5B38);
    if (cb && load<s16>(cb + 8) == 0x14E)
      store<u16>(a + 0xFA, load<u16>(a + 0xFA) | 1);
  }
  if (load<u8>(a + 0x6C8) && !load<u8>(a + 0x6D6)) {
    p = play();
    u32 controlled = load<u32>(p + 0x5B2C);
    if (controlled && load<s16>(controlled + 8) == 0x14E) {
      store<u32>(a + 0x39C, load<u32>(a + 0x39C) | 0x1000000Au);
      return true;
    }
  }
  store<u32>(a + 0x39C, load<u32>(a + 0x39C) & ~0x1000000Au);
  return true;
}
VERIFY(0x023B2EF4, execute);
bool draw(u32 a) {
  WWHD_FUNC(0x023B320C, bool, a);
  u32 env = call<u32>(0x02555D0C);
  call<void>(0x025626A4, env, 1, a + 0x314, a + 0x110);
  env = call<u32>(0x02555D0C);
  call<void>(0x02562F5C, env, load<u32>(a + 0x3BC), a + 0x110);
  env = call<u32>(0x02555D0C);
  call<void>(0x02562F5C, env, load<u32>(a + 0x3C0), a + 0x110);
  s32 state = load<s32>(a + 0x6CC);
  u32 model = load<u32>(a + 0x3C0), data = load<u32>(model + 0xAC);
  u32 anim = state < 3 ? a + 0x3C4 : a + 0x450;
  f32 frame = load<f32>(anim + 4);
  call<void>(0x025E86B8, anim, data, frame);
  call<void>(0x025E2DE0, load<u32>(a + 0x3C0), 0);
  state = load<s32>(a + 0x6CC);
  model = load<u32>(a + 0x3C0);
  data = load<u32>(model + 0xAC);
  store<u32>(load<u32>(data + 8) + 0x14, 0);
  call<void>(0x025E2DE0, load<u32>(a + 0x3BC), 0);
  return true;
}
VERIFY(0x023B320C, draw);
s32 wrapperCreate(u32 a) {
  WWHD_FUNC(0x023B3308, s32, a);
  return create(a);
}
VERIFY(0x023B3308, wrapperCreate);
bool wrapperDelete(u32 a) {
  WWHD_FUNC(0x023B330C, bool, a);
  return remove(a);
}
VERIFY(0x023B330C, wrapperDelete);
bool wrapperExecute(u32 a) {
  WWHD_FUNC(0x023B3310, bool, a);
  return execute(a);
}
VERIFY(0x023B3310, wrapperExecute);
bool wrapperDraw(u32 a) {
  WWHD_FUNC(0x023B3314, bool, a);
  return draw(a);
}
VERIFY(0x023B3314, wrapperDraw);
void staticInit() {
  WWHD_FUNC(0x023B3318, void);
  store<u32>(0x1046C8A8, 0);
  store<u32>(0x1046C8A0, 0);
  store<u32>(0x1046C8AC, 0);
  store<u32>(0x1046C8A4, 0);
  call<void>(0x028F026C, 0x101CDA48u);
  store<f32>(0x1046C894, -3.1415927410125732f);
  store<f32>(0x1046C898, 3.1415927410125732f);
  call<void>(0x028ED6F8, 0x1046C89Cu);
  call<void>(0x028F026C, 0x101CDA54u);
  call<void>(0x028EAB2C, 0x1046C89Du);
  call<void>(0x028F026C, 0x101CDA60u);
}
VERIFY(0x023B3318, staticInit);
void staticDtor(u32 a, s32 flags) {
  WWHD_FUNC(0x023B33AC, void, a, flags);
  if (a && (flags & 1))
    call<void>(0x0273AF40, a);
}
VERIFY(0x023B33AC, staticDtor);
void empty(u32 a) { WWHD_FUNC(0x023B33C0, void, a); }
VERIFY(0x023B33C0, empty);
void actorDtor(u32 a, s32 flags) {
  WWHD_FUNC(0x023B33C4, void, a, flags);
  if (a) {
    call<void>(0x02515A70, a + 0x518, 2);
    call<void>(0x02515860, a + 0x4DC, 2);
    call<void>(0x027F3628, a + 0x460, 0);
    call<void>(0x027F3628, a + 0x3D4, 0);
    call<void>(0x025D50BC, a, 0);
    if (flags & 1)
      call<void>(0x0273AF40, a);
  }
}
VERIFY(0x023B33C4, actorDtor);
s32 isDelete(u32 a) {
  WWHD_FUNC(0x023B3448, s32, a);
  return 1;
}
VERIFY(0x023B3448, isDelete);
} // namespace vmc

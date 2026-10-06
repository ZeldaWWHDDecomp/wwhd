// Ganon's Tower trial door, full HD TU. Derived game code:
#include "d/actor/d_a_obj_vgnfd.h"
namespace {
using Actor = daObjVgnfd_c;
static u32 rd(u32 a) { return gabi::load<u32>(a); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static void assertion(u32 file, s32 line, u32 condition) {
  gabi::call(0x0273AA24, file, line, condition);
}
struct ResourceKey {
  be<u32> string;
  be<u32> vtable;
};
static u32 resource(s32 index) {
  gabi::Local<ResourceKey> key;
  key->string = 0x10032D10;
  key->vtable = 0x10032BBC;
  return gabi::call<u32>(0x026066C4, rd(0x101F4F28), key.get(), index);
}
static void matrix_copy(u32 from, u32 to) {
  f32 values[12];
  for (int i = 0; i < 12; ++i)
    values[i] = gabi::load<f32>(from + 4 * i);
  for (int i = 0; i < 12; ++i)
    gabi::store<f32>(to + 4 * i, values[i]);
}
static BOOL event_bit(u32 table, u32 index) {
  u32 save = rd(0x101F84DC);
  u16 bit = gabi::load<u16>(table + 2 * index);
  return gabi::call<BOOL>(0x025B8B94, save + 0x644, bit);
}
static void order(Actor *a, s32 event) {
  gabi::call(0x025D7A58, a, event, 0xff, 0xffff, 0, 1);
}
static void condition(Actor *a) {
  u32 addr = gabi::ea(a) + 0xfa;
  gabi::store<u16>(addr, gabi::load<u16>(addr) | 2);
}
static void vibration(u32 function, s32 power, s32 duration) {
  u32 p = play();
  gabi::Local<cXyz> direction;
  direction->set(0.f, 1.f, 0.f);
  gabi::call(function, p + 0x599c, power, duration, direction.get());
}
static void audio(Actor *a, s32 id) {
  s8 room = gabi::load<s8>(gabi::ea(a) + 0x326);
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call(0x025E1A40, id, gabi::ea(a) + 0x314, 0, reverb);
}
static void cut_end(Actor *a) {
  s32 staff = a->staff;
  u32 p = play();
  gabi::call(0x02543280, p + 0x52c4, staff);
}
BOOL check_fin(Actor *a) {
  WWHD_FUNC(0x023B1124, BOOL, a);
  for (u32 i = 0; i < 4; ++i)
    if (!event_bit(0x10032D00, i))
      return 0;
  return 1;
}
VERIFY(0x023B1124, check_fin);
BOOL create_bdl_brk(Actor *a, s32 index) {
  WWHD_FUNC(0x023B11A0, BOOL, a, index);
  u32 self = gabi::ea(a), off = (u32)index * 4;
  s32 bdl = gabi::load<s32>(0x10032D18 + off);
  u32 data = resource(bdl);
  if (!data) {
    assertion(0x10032C18, 0xfe, 0x10032C2C);
    return 0;
  }
  u32 model = gabi::call<u32>(0x025E38E0, data, 0, 0x11020203u);
  gabi::store<u32>(self + 0x3ac + off, model);
  if (!model)
    return 0;
  s32 brk = gabi::load<s32>(0x10032D2C + off);
  if (brk == -1)
    return 1;
  u32 animation = resource(brk);
  if (!animation) {
    assertion(0x10032C18, 0x105, 0x10032C3C);
    return 0;
  }
  BOOL result = gabi::call<BOOL>(0x025E8154, self + 0x444 + (u32)index * 0x78,
                                 data, animation, 1, 0, 0, -1, 0, 0, 1.f);
  return result != 0;
}
VERIFY(0x023B11A0, create_bdl_brk);
BOOL create_heap(Actor *a) {
  WWHD_FUNC(0x023B12D8, BOOL, a);
  u32 self = gabi::ea(a);
  for (s32 i = 0; i < 5; ++i)
    if (!create_bdl_brk(a, i))
      return 0;
  for (u32 i = 0; i < 2; ++i) {
    s32 index = gabi::load<s32>(0x10032CF8 + 4 * i);
    u32 data = resource(index);
    if (!data) {
      assertion(0x10032C48, 0x133, 0x10032C5C);
      return 0;
    }
    u32 model = gabi::call<u32>(0x025E38E0, data, 0, 0x11020203u);
    gabi::store<u32>(self + 0x3c0 + 4 * i, model);
    if (!model)
      return 0;
  }
  u32 animation = resource(0x16);
  if (!animation) {
    assertion(0x10032C48, 0x144, 0x10032C6C);
    return 0;
  }
  u32 model = rd(self + 0x3c4), data = rd(model + 0xac);
  if (!gabi::call<BOOL>(0x025E7CE0, self + 0x3c8, data, animation, 1, 0, 0, -1,
                        0, 0, 1.f))
    return 0;
  u32 bg = resource(0x19);
  model = rd(self + 0x3ac);
  u32 matrix = model ? model + 0xc8 : 0;
  u32 world = gabi::call<u32>(0x024F2478, bg, 1, matrix);
  a->bgw = gabi::at<void>(world);
  if (!world) {
    assertion(0x10032C48, 0x151, 0x10032C7C);
    return a->bgw.get() != nullptr;
  }
  return 1;
}
VERIFY(0x023B12D8, create_heap);
BOOL solidHeapCB(Actor *a) {
  WWHD_FUNC(0x023B149C, BOOL, a);
  return create_heap(a);
}
VERIFY(0x023B149C, solidHeapCB);
void init_mtx(Actor *a) {
  WWHD_FUNC(0x023B14A0, void, a);
  u32 self = gabi::ea(a);
  f32 x = gabi::load<f32>(self + 0x330), y = gabi::load<f32>(self + 0x334);
  u32 model = rd(self + 0x3ac);
  f32 z = gabi::load<f32>(self + 0x338);
  gabi::store<f32>(model + 0xbc, x);
  gabi::store<f32>(model + 0xc0, y);
  gabi::store<f32>(model + 0xc4, z);
  x = gabi::load<f32>(self + 0x314);
  y = gabi::load<f32>(self + 0x318);
  z = gabi::load<f32>(self + 0x31c);
  gabi::call(0x028E93CC, 0x1048D0CCu, x, y, z);
  s16 ax = gabi::load<s16>(self + 0x328), ay = gabi::load<s16>(self + 0x32a),
      az = gabi::load<s16>(self + 0x32c);
  gabi::call(0x025F1B48, 0x1048D0CCu, ax, ay, az);
  for (u32 i = 0; i < 5; ++i)
    matrix_copy(0x1048D0CC, rd(self + 0x3ac + 4 * i) + 0xc8);
  for (u32 i = 0; i < 2; ++i)
    matrix_copy(0x1048D0CC, rd(self + 0x3c0 + 4 * i) + 0xc8);
}
VERIFY(0x023B14A0, init_mtx);
s32 get_start_demo_idx(Actor *a) {
  WWHD_FUNC(0x023B1604, s32, a);
  for (u32 i = 0; i < 4; ++i)
    if (!event_bit(0x10032D00, i) && event_bit(0x10032D08, i))
      return i;
  return -1;
}
VERIFY(0x023B1604, get_start_demo_idx);
s32 create(Actor *a) {
  WWHD_FUNC(0x023B1694, s32, a);
  u32 self = gabi::ea(a), flags = rd(self + 0x2e4);
  if (!(flags & 8)) {
    if (self) {
      gabi::call(0x025D4ED0, a);
      gabi::store<u32>(self + 0xb4, 0x10032BD4);
      gabi::call(0x025E7C6C, self + 0x3c8);
      gabi::call(0x028EFFD0, self + 0x444, 5, 0x78, 0x025E80D0u);
      gabi::call(0x025A5B18, self + 0x69c, 1);
      flags = rd(self + 0x2e4);
    }
    gabi::store<u32>(self + 0x2e4, flags | 8);
  }
  if (check_fin(a))
    return 5;
  s32 phase = gabi::call<s32>(0x02520460, self + 0x43c, 0x10032D10u);
  if (phase != 4)
    return phase;
  if (!gabi::call<BOOL>(0x025D63E8, a, 0x023B149Cu, 0))
    return 5;
  u32 model = rd(self + 0x3ac);
  gabi::store<u32>(self + 0x348, model ? model + 0xc8 : 0);
  init_mtx(a);
  gabi::store<f32>(self + 0x444, 0.f);
  for (u32 i = 1; i < 5; ++i) {
    u32 controller = self + 0x444 + 0x78 * i;
    gabi::store<f32>(controller, 0.f);
    if (event_bit(0x10032D00, i - 1))
      gabi::store<f32>(controller + 4, (f32)gabi::load<s16>(controller + 0xa));
  }
  gabi::store<f32>(self + 0x3c8, 0.f);
  s32 demo = get_start_demo_idx(a);
  a->demo = demo;
  if (demo != -1) {
    s32 current = a->demo;
    a->state = 1;
    u32 name = rd(0x101CD95C + (u32)current * 4);
    u32 p = play();
    a->event = gabi::call<s32>(0x02543F10, p + 0x52c4, name, 0xff);
  }
  gabi::call(0x025D674C, a, -260.f, -10.f, -50.f, 260.f, 510.f, 100.f);
  gabi::store<u32>(self + 0x6b8, self + 0x110);
  gabi::store<u8>(self + 0x6ad, 0);
  gabi::store<u8>(self + 0x6b1, 1);
  gabi::store<u8>(self + 0x6ae, 1);
  u32 p = play();
  u32 world = gabi::ea(a->bgw.get());
  gabi::call(0x024EEA6C, p + 0x12a0, world, a);
  world = gabi::ea(a->bgw.get());
  gabi::call(0x024F43DC, world);
  a->initialized = 1;
  return 4;
}
VERIFY(0x023B1694, create);
u8 remove(Actor *a) {
  WWHD_FUNC(0x023B18F8, u8, a);
  u32 self = gabi::ea(a);
  if (rd(self + 0xf4)) {
    u32 world = gabi::ea(a->bgw.get());
    if (world && rd(world) < 0x100) {
      u32 p = play();
      world = gabi::ea(a->bgw.get());
      gabi::call(0x020087EC, p + 0x12a0, world);
      a->bgw = nullptr;
    }
  }
  u32 vtable = rd(self + 0x69c), method = rd(vtable + 0x44);
  gabi::call(method, self + 0x69c);
  gabi::call(0x025204C8, self + 0x43c, 0x10032D10u);
  return 1;
}
VERIFY(0x023B18F8, remove);
void set_timer(Actor *a) {
  WWHD_FUNC(0x023B1984, void, a);
  s32 staff = a->staff;
  u32 p = play();
  u32 value = gabi::call<u32>(0x0254487C, p + 0x52c4, staff, 0x10032CB0u, 3);
  a->timer = 0;
  if (value)
    a->timer = gabi::load<s32>(value);
}
VERIFY(0x023B1984, set_timer);
void on_fin(Actor *a) {
  WWHD_FUNC(0x023B19EC, void, a);
  gabi::call(0x025B8B68, rd(0x101F84DC) + 0x644, 0x3204);
}
VERIFY(0x023B19EC, on_fin);
u8 execute(Actor *a) {
  WWHD_FUNC(0x023B1A00, u8, a);
  u32 self = gabi::ea(a);
  bool done = false;
  u8 state = a->state;
  if (state == 1 || state == 2) {
    if (state == 1 && (s32)a->demo == -1)
      assertion(0x10032CBC, 0x253, 0x10032CD0);
    if (gabi::load<u16>(self + 0xf8) != 2) {
      order(a, (s16)a->event);
      condition(a);
    } else {
      s16 event = a->event;
      u32 p = play();
      BOOL end = gabi::call<BOOL>(0x025440C8, p + 0x52c4, event);
      p = play();
      if (end) {
        gabi::store<u16>(p + 0x52b8, gabi::load<u16>(p + 0x52b8) | 8);
        if (state == 1) {
          if (check_fin(a)) {
            p = play();
            s32 event =
                gabi::call<s32>(0x02543F10, p + 0x52c4, 0x10032CE4u, 0xff);
            a->event = event;
            order(a, event);
            u16 flags = gabi::load<u16>(self + 0xfa);
            a->state = 2;
            gabi::store<u16>(self + 0xfa, flags | 2);
          } else
            a->state = 0;
        } else {
          on_fin(a);
          done = true;
          a->state = 0;
        }
      } else {
        s32 staff = gabi::call<s32>(0x02542D88, p + 0x52c4, 0x10032D10u, 0, 0);
        a->staff = staff;
        if (staff != -1) {
          p = play();
          s32 action = gabi::call<s32>(0x02542EDC, p + 0x52c4, staff,
                                       state == 1 ? 0x101CD954u : 0x101CD96Cu,
                                       state == 1 ? 2 : 6, 0, 0);
          staff = a->staff;
          p = play();
          BOOL advance = gabi::call<BOOL>(0x025447C8, p + 0x52c4, staff);
          if (state == 1) {
            if (action == 0) {
              if (advance) {
                s32 demo = a->demo;
                u32 save = rd(0x101F84DC);
                u16 bit = gabi::load<u16>(0x10032D00 + (u32)demo * 2);
                gabi::call(0x025B8B68, save + 0x644, bit);
                set_timer(a);
              }
              s32 timer = (s32)((u32)(s32)a->timer - 1u);
              a->timer = timer;
              if (timer <= 0) {
                cut_end(a);
                s32 demo = a->demo;
                gabi::store<f32>(self + 0x4bc + (u32)demo * 0x78, 1.f);
                gabi::call(0x025E1988, 0x6a55);
                if (check_fin(a))
                  gabi::call(0x025E1988, 0x806);
              }
            }
          } else {
            if (advance) {
              set_timer(a);
              switch (action) {
              case 1:
                gabi::store<f32>(self + 0x444, 1.f);
                for (u32 i = 1; i < 5; ++i)
                  gabi::store<f32>(self + 0x444 + 0x78 * i, -1.f);
                break;
              case 2:
                vibration(0x025CB374, 8, 23);
                audio(a, 0x6a1d);
                break;
              case 3:
                vibration(0x025CB408, 2, 11);
                break;
              case 4: {
                gabi::store<f32>(self + 0x3c8, 1.f);
                a->currentModel = 1;
                for (u32 i = 0; i < 6; ++i) {
                  u16 effect = gabi::load<u16>(0x101CD984 + 2 * i);
                  p = play();
                  u32 particles = rd(p + 0x5ab0);
                  gabi::call(0x025A847C, particles, 0, effect, self + 0x314,
                             self + 0x320, self + 0x330, 0xff, 0, -1, 0, 0, 0);
                }
                p = play();
                u32 particles = rd(p + 0x5ab0);
                gabi::call(0x025A847C, particles, 2, 0xa344, self + 0x314,
                           self + 0x320, self + 0x330, 0xa0, self + 0x69c, -1,
                           0, 0, 0);
                vibration(0x025CB408, 4, 11);
                audio(a, 0x6a1e);
                break;
              }
              case 5:
                p = play();
                gabi::call(0x025CB610, p + 0x599c, -1);
                vibration(0x025CB374, 8, 27);
                {
                  s8 room = gabi::load<s8>(self + 0x326);
                  a->currentModel = 2;
                  s32 reverb = gabi::call<s32>(0x02520540, room);
                  gabi::call(0x025E1A40, 0x6a1f, self + 0x314, 0, reverb);
                }
                break;
              }
            }
            s32 timer = a->timer;
            if (timer > 0) {
              timer = (s32)((u32)timer - 1u);
              a->timer = timer;
            }
            if ((u32)action <= 4 && timer <= 0)
              cut_end(a);
          }
        }
      }
    }
  }
  for (u32 i = 0; i < 5; ++i)
    gabi::call(0x025E742C, self + 0x444 + i * 0x78);
  gabi::call(0x025E742C, self + 0x3c8);
  if (done)
    gabi::call(0x025D57E0, a);
  return 1;
}
VERIFY(0x023B1A00, execute);
u8 draw(Actor *a) {
  WWHD_FUNC(0x023B203C, u8, a);
  u32 self = gabi::ea(a);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, env, 1, self + 0x314, self + 0x110);
  s32 current = a->currentModel;
  if (current == 0) {
    env = gabi::call<u32>(0x02555D0C);
    current = a->currentModel;
    u32 model = rd(self + 0x3c0 + (u32)current * 4);
    gabi::call(0x02562F5C, env, model, self + 0x110);
    current = a->currentModel;
    model = rd(self + 0x3c0 + (u32)current * 4);
    gabi::call(0x025E2DE0, model, 0);
    for (s32 i = 4; i >= 0; --i) {
      env = gabi::call<u32>(0x02555D0C);
      model = rd(self + 0x3ac + (u32)i * 4);
      gabi::call(0x02562F5C, env, model, self + 0x110);
      if (gabi::load<s32>(0x10032D2C + (u32)i * 4) != -1) {
        model = rd(self + 0x3ac + (u32)i * 4);
        u32 controller = self + 0x444 + (u32)i * 0x78;
        f32 frame = gabi::load<f32>(controller + 4);
        u32 data = rd(model + 0xac);
        gabi::call(0x025E83FC, controller, data, frame);
      }
      model = rd(self + 0x3ac + (u32)i * 4);
      gabi::call(0x025E2DE0, model, 0);
    }
  } else if (current == 1) {
    env = gabi::call<u32>(0x02555D0C);
    current = a->currentModel;
    u32 model = rd(self + 0x3c0 + (u32)current * 4);
    gabi::call(0x02562F5C, env, model, self + 0x110);
    current = a->currentModel;
    f32 frame = gabi::load<f32>(self + 0x3cc);
    model = rd(self + 0x3c0 + (u32)current * 4);
    u32 data = rd(model + 0xac);
    gabi::call(0x025E7FC4, self + 0x3c8, data, frame);
    current = a->currentModel;
    model = rd(self + 0x3c0 + (u32)current * 4);
    gabi::call(0x025E2DE0, model, 0);
  }
  return 1;
}
VERIFY(0x023B203C, draw);
s32 create_adapter(Actor *a) {
  WWHD_FUNC(0x023B217C, s32, a);
  return create(a);
}
VERIFY(0x023B217C, create_adapter);
u8 delete_adapter(Actor *a) {
  WWHD_FUNC(0x023B2180, u8, a);
  return remove(a);
}
VERIFY(0x023B2180, delete_adapter);
u8 execute_adapter(Actor *a) {
  WWHD_FUNC(0x023B2184, u8, a);
  return execute(a);
}
VERIFY(0x023B2184, execute_adapter);
u8 draw_adapter(Actor *a) {
  WWHD_FUNC(0x023B2188, u8, a);
  return draw(a);
}
VERIFY(0x023B2188, draw_adapter);
void sinit() {
  WWHD_FUNC(0x023B218C, void);
  for (u32 i = 0; i < 4; ++i)
    gabi::store<u32>(0x1046C884 + 4 * i, 0);
  gabi::call(0x028F026C, 0x101CD990u);
  f32 lo = gabi::load<f32>(0x10032CF0), hi = gabi::load<f32>(0x10032CF4);
  gabi::store<f32>(0x1046C878, lo);
  gabi::store<f32>(0x1046C87C, hi);
  gabi::call(0x028ED6F8, 0x1046C880u);
  gabi::call(0x028F026C, 0x101CD99Cu);
  gabi::call(0x028EAB2C, 0x1046C881u);
  gabi::call(0x028F026C, 0x101CD9A8u);
}
VERIFY(0x023B218C, sinit);
void static_destructor(void *a, u32 flags) {
  WWHD_FUNC(0x023B2220, void, a, flags);
  if (a && (flags & 1))
    gabi::call(0x0273AF40, a);
}
VERIFY(0x023B2220, static_destructor);
void destructor(Actor *a, u32 flags) {
  WWHD_FUNC(0x023B2234, void, a, flags);
  if (a) {
    gabi::call(0x025D50BC, a, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, a);
  }
}
VERIFY(0x023B2234, destructor);
void noop(void *a) { WWHD_FUNC(0x023B2288, void, a); }
VERIFY(0x023B2288, noop);
BOOL IsDelete(Actor *a) {
  WWHD_FUNC(0x023B228C, BOOL, a);
  return 1;
}
VERIFY(0x023B228C, IsDelete);
} // namespace

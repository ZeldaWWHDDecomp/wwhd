/* Fire hazard, derived from GameCube source and audited HD disassembly. */
#include "gabi.h"
namespace fire {
using gabi::call;
using gabi::load;
using gabi::store;
void normal_proc(u32 a) { WWHD_FUNC(0x0213E8BC, void, a); }
VERIFY(0x0213E8BC, normal_proc);
void stop(u32 a) {
  WWHD_FUNC(0x0213E8C0, void, a);
  for (int i = 0; i < 3; ++i) {
    u32 e = load<u32>(a + 0x9D8 + 4 * i);
    if (e)
      store<u32>(e + 0x254, load<u32>(e + 0x254) | 1);
  }
  u32 x = load<u32>(a + 0x3E8), y = load<u32>(a + 0x7A4),
      z = load<u32>(a + 0x8A8);
  store<u32>(a + 0x3E8, x & ~1u);
  store<u32>(a + 0x7A4, y & ~1u);
  store<u32>(a + 0x8A8, z & ~1u);
}
VERIFY(0x0213E8C0, stop);
void stopNow(u32 a) {
  WWHD_FUNC(0x0213EE90, void, a);
  for (int i = 0; i < 3; ++i) {
    u32 e = load<u32>(a + 0x9D8 + 4 * i);
    if (e)
      call<void>(0x0281DE68, e, 1);
  }
  u32 x = load<u32>(a + 0x3E8), y = load<u32>(a + 0x7A4),
      z = load<u32>(a + 0x8A8);
  store<u32>(a + 0x3E8, x & ~1u);
  store<u32>(a + 0x7A4, y & ~1u);
  store<u32>(a + 0x8A8, z & ~1u);
}
VERIFY(0x0213EE90, stopNow);
void playFire(u32 a) {
  WWHD_FUNC(0x0213EF18, void, a);
  for (int i = 0; i < 3; ++i) {
    u32 e = load<u32>(a + 0x9D8 + 4 * i);
    if (e)
      store<u32>(e + 0x254, load<u32>(e + 0x254) & ~1u);
  }
  u32 x = load<u32>(a + 0x3E8), y = load<u32>(a + 0x7A4),
      z = load<u32>(a + 0x8A8);
  store<u32>(a + 0x3E8, x | 1);
  store<u32>(a + 0x7A4, y | 1);
  store<u32>(a + 0x8A8, z | 1);
}
VERIFY(0x0213EF18, playFire);
void searchWind(u32 a) {
  WWHD_FUNC(0x0213EE34, void, a);
  gabi::Local<u32> type;
  store<u16>(type.a, 0x187);
  u32 w = call<u32>(0x025D5218, 0x025E121Cu, type.a);
  store<u32>(a + 0xA0C, w ? load<u32>(w + 4) : 0xFFFFFFFFu);
}
VERIFY(0x0213EE34, searchWind);
void setDir(u32 a, u32 d) {
  WWHD_FUNC(0x0213EF88, void, a, d);
  call<void>(0x0200F5C8, a + 0x9F0, load<f32>(d), 1.0f);
  call<void>(0x0200F5C8, a + 0x9F4, load<f32>(d + 4), 1.0f);
  call<void>(0x0200F5C8, a + 0x9F8, load<f32>(d + 8), 1.0f);
  for (int i = 0; i < 3; ++i) {
    u32 e = load<u32>(a + 0x9D8 + 4 * i);
    if (e) {
      f32 y = load<f32>(a + 0x9F4), z = load<f32>(a + 0x9F8),
          x = load<f32>(a + 0x9F0);
      store<f32>(e + 0x30, z);
      store<f32>(e + 0x2C, y);
      store<f32>(e + 0x28, x);
    }
  }
}
VERIFY(0x0213EF88, setDir);
bool CreateInit(u32 a) {
  WWHD_FUNC(0x0213E930, bool, a);
  u32 param = load<u32>(a + 0xB0);
  store<s32>(a + 0x9E4, param & 255);
  store<u32>(a + 0x9E8, (param >> 12) & 31);
  bool stopped = call<s32>(0x025BA0C0, load<u32>(0x101F84DC) + 0x20,
                           load<s32>(a + 0x9E4), load<s8>(a + 0x2FE)) != 0;
  if (!stopped)
    stopped = call<s32>(0x025B8C74, load<u32>(0x101F84DC) + 0x798,
                        load<u32>(a + 0x9E8)) != 0;
  store<u8>(a + 0xA08, stopped);
  if (call<s32>(0x025B8C74, load<u32>(0x101F84DC) + 0x798,
                load<u32>(a + 0x9E8)))
    return false;
  param = load<u32>(a + 0xB0);
  u8 type = (param >> 17) & 7;
  store<u8>(a + 0xA1C, type);
  if (type == 0 && load<s32>(a + 0x9E4) == 255)
    store<u8>(a + 0xA1C, 2);
  store<s8>(a + 0xA1D, 30);
  u8 shape = (param >> 8) & 15;
  store<u8>(a + 0x9EC, shape);
  if (shape == 1)
    store<f32>(a + 0x338, 0.0f);
  if (shape == 2) {
    u32 e = load<u32>(a + 0x9D8);
    store<f32>(a + 0x330, 1.25f);
    store<f32>(a + 0x334, 1.25f);
    store<f32>(a + 0x338, 1.25f);
    if (e) {
      f32 z = load<f32>(a + 0x338), y = load<f32>(a + 0x334);
      store<f32>(e + 0x228, z);
      store<f32>(e + 0x224, y);
      store<f32>(e + 0x238, 1.25f);
      store<f32>(e + 0x23C, y);
      store<f32>(e + 0x240, z);
      store<f32>(e + 0x220, 1.25f);
    }
    for (int i = 1; i < 3; ++i) {
      e = load<u32>(a + 0x9D8 + 4 * i);
      if (e) {
        f32 z = load<f32>(a + 0x338), y = load<f32>(a + 0x334),
            x = load<f32>(a + 0x330);
        store<f32>(e + 0x224, y);
        store<f32>(e + 0x228, z);
        store<f32>(e + 0x238, x);
        store<f32>(e + 0x240, z);
        store<f32>(e + 0x23C, y);
        store<f32>(e + 0x220, x);
      }
    }
  }
  u32 st = a + 0x3AC;
  call<void>(0x02515F14, st, 255, 255, a);
  if (load<u8>(a + 0x9EC) == 1) {
    for (int i = 0; i < 3; ++i) {
      u32 c = a + 0x3E8 + 0x130 * i;
      call<void>(0x02516518, c, 0x101B4F64u);
      store<u32>(c + 0x44, st);
      call<void>(0x020184DC, c + 0x118, 50.0f);
    }
  } else {
    call<void>(0x02516518, a + 0x3E8, 0x101B4F64u);
    store<u32>(a + 0x42C, st);
  }
  call<void>(0x02516518, a + 0x8A8, 0x101B4FECu);
  store<u32>(a + 0x8EC, st);
  call<void>(0x02516518, a + 0x778, 0x101B4FA8u);
  store<u32>(a + 0x7BC, st);
  f32 x = load<f32>(0x101FFBA8), y = load<f32>(0x101FFBAC),
      z = load<f32>(0x101FFBB0);
  store<f32>(a + 0x464, x);
  store<f32>(a + 0x468, y);
  store<f32>(a + 0x46C, z);
  call<void>(0x025DA884, a + 0xDC);
  x = load<f32>(0x101FFBC0);
  store<f32>(a + 0x9F0, x);
  y = load<f32>(0x101FFBC4);
  type = load<u8>(a + 0xA1C);
  store<f32>(a + 0x9F4, y);
  z = load<f32>(0x101FFBC8);
  store<f32>(a + 0x9FC, x);
  store<f32>(a + 0x9F8, z);
  store<f32>(a + 0xA00, y);
  store<f32>(a + 0xA04, z);
  s32 sw;
  if (type == 0) {
    u32 sound = (load<u32>(a + 0xB0) >> 20) & 15;
    s16 off = call<s16>(0x02543F10, call<u32>(0x025200D4) + 0x52C4,
                        sound ? 0x1000F0F0u : 0x1000F118u, 255);
    store<s16>(a + 0xA1A, off);
    s16 on =
        call<s16>(0x02543F10, call<u32>(0x025200D4) + 0x52C4, 0x1000F100u, 255);
    sw = load<s32>(a + 0x9E4);
    store<s16>(a + 0xA18, on);
  } else if (type == 1) {
    u32 sound = (load<u32>(a + 0xB0) >> 20) & 15;
    s16 off = call<s16>(0x02543F10, call<u32>(0x025200D4) + 0x52C4,
                        sound ? 0x1000F128u : 0x1000F10Cu, 255);
    sw = load<s32>(a + 0x9E4);
    store<s16>(a + 0xA1A, off);
    store<s16>(a + 0xA18, -1);
  } else {
    sw = load<s32>(a + 0x9E4);
    store<s16>(a + 0xA1A, -1);
    store<s16>(a + 0xA18, -1);
  }
  if (sw == 255) {
    store<u8>(a + 0xA1E, 1);
    return true;
  }
  if (call<s32>(0x025BA0C0, load<u32>(0x101F84DC) + 0x20, sw,
                load<s8>(a + 0x2FE))) {
    store<u8>(a + 0xA1E, 0);
    stop(a);
  } else
    store<u8>(a + 0xA1E, 1);
  return true;
}
VERIFY(0x0213E930, CreateInit);
void ctrlEffect(u32 a) {
  WWHD_FUNC(0x0213F078, void, a);
  if (!load<u8>(a + 0xA1E))
    return;
  bool stopped = false;
  searchWind(a);
  gabi::Local<u32> id;
  u32 n = load<u32>(a + 0xA0C);
  store<u32>(id.a, n);
  u32 wind = 0;
  if (n != 0xFFFFFFFFu)
    wind = call<u32>(0x025D5218, 0x025E1234u, id.a);
  s32 hit = call<s32>(0x025162A4, a + 0x3E8);
  f32 zero = 0.0f, one = 1.0f;
  if (hit) {
    u32 o = call<u32>(0x02516300, a + 0x3E8);
    bool cleared = false;
    if (o) {
      u32 t = load<u32>(o + 0x10);
      if ((t & 0x200000) && wind) {
        u32 owner = call<u32>(0x02515BBC, a + 0x47C);
        if (owner == wind) {
          stopped = true;
          call<void>(0x0251621C, a + 0x3E8);
          cleared = true;
        } else
          t = load<u32>(o + 0x10);
      }
      if (!cleared) {
        if (t & 0x80100) {
          store<u8>(a + 0x9EE, 80);
          store<u8>(a + 0x9ED, 0);
          stopped = true;
          stopNow(a);
          call<u32>(0x025A847C, load<u32>(call<u32>(0x025200D4) + 0x5AB0), 0,
                    0x463, a + 0x314, 0, 0, 255, 0, -1, 0, 0, 0);
        } else if (t & 0x200000) {
          u32 owner = call<u32>(0x02515BBC, a + 0x47C);
          if (owner != wind) {
            f32 z = load<f32>(a + 0x4B0), x = load<f32>(a + 0x4A8), z2 = z * z,
                y = load<f32>(a + 0x4AC);
            f32 sq = gabi::fmadds(x, x, z2);
            store<f32>(a + 0xA00, y);
            store<f32>(a + 0x9FC, x);
            store<f32>(a + 0xA04, z);
            store<f32>(a + 0xA00, call<f32>(0x028F4384, sq / 1000.0f));
            if (!call<s32>(0x0201B47C, a + 0x9FC)) {
              store<f32>(a + 0xA00, one);
              store<f32>(a + 0x9FC, zero);
              store<f32>(a + 0xA04, zero);
            }
          }
        }
      }
    }
    if (!cleared)
      call<void>(0x0251621C, a + 0x3E8);
  } else {
    call<void>(0x0200F5C8, a + 0x9FC, zero, 0.025f);
    call<void>(0x0200F5C8, a + 0xA00, one, 0.025f);
    call<void>(0x0200F5C8, a + 0xA04, zero, 0.025f);
  }
  if (load<u8>(a + 0x9ED) == 0 && load<u8>(a + 0x9EE) != 0) {
    u8 c = load<u8>(a + 0x9EE);
    store<u8>(a + 0x9EE, c - 1);
    stopped = true;
    stop(a);
  } else if (stopped)
    stop(a);
  else {
    playFire(a);
    s32 rev = call<s32>(0x02520540, load<s8>(a + 0x326));
    call<void>(0x025E1A40, 0x614F, a + 0x37C, 0, rev);
  }
  if (load<u8>(a + 0xA08) != (u8)stopped) {
    u32 snd = stopped ? 0x6950 : 0x694E;
    s32 rev = call<s32>(0x02520540, load<s8>(a + 0x326));
    call<void>(0x025E1A40, snd, a + 0x37C, 0, rev);
  }
  setDir(a, a + 0x9FC);
  store<u8>(a + 0xA08, stopped);
}
VERIFY(0x0213F078, ctrlEffect);
u32 cylinderCtor(u32 a) {
  WWHD_FUNC(0x0213FBEC, u32, a);
  if (!a)
    a = call<u32>(0x0273AD10, 0x130);
  if (!a)
    return 0;
  call<void>(0x02515FB8, a);
  store<u32>(a + 0x114, 0x100015A8);
  store<u32>(a + 0x110, 0x1000F0C4);
  call<void>(0x02018590, a + 0x118);
  store<u32>(a + 0x3C, 0x1004B108);
  store<u32>(a + 0x12C, 0x1004B150);
  store<u32>(a + 0x114, 0x1004B160);
  return a;
}
VERIFY(0x0213FBEC, cylinderCtor);
s32 create(u32 a) {
  WWHD_FUNC(0x0213F3D0, s32, a);
  u32 flags = load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (a) {
      call<void>(0x025D4ED0, a);
      store<u32>(a + 0xB4, 0x1000F0D4);
      call<void>(0x0200BD2C, a + 0x3AC);
      call<void>(0x02515DA0, a + 0x3C8);
      store<u32>(a + 0x3C4, 0x1004AE88);
      store<u32>(a + 0x3C8, 0x1004AEC0);
      call<void>(0x028EFFD0, a + 0x3E8, 3, 0x130, 0x0213FBECu);
      call<void>(0x02515FB8, a + 0x778);
      store<u32>(a + 0x888, 0x1000F0C4);
      store<u32>(a + 0x88C, 0x100015A8);
      call<void>(0x02018590, a + 0x890);
      store<u32>(a + 0x7B4, 0x1004B108);
      store<u32>(a + 0x8A4, 0x1004B150);
      store<u32>(a + 0x88C, 0x1004B160);
      call<void>(0x02515FB8, a + 0x8A8);
      store<u32>(a + 0x9BC, 0x100015A8);
      store<u32>(a + 0x9B8, 0x1000F0C4);
      call<void>(0x02018590, a + 0x9C0);
      store<u32>(a + 0x9D4, 0x1004B150);
      store<u32>(a + 0x9BC, 0x1004B160);
      flags = load<u32>(a + 0x2E4);
      store<u32>(a + 0x8E4, 0x1004B108);
    }
    store<u32>(a + 0x2E4, flags | 8);
  }
  store<u32>(a + 0x9D8,
             call<u32>(0x025A847C, load<u32>(call<u32>(0x025200D4) + 0x5AB0), 0,
                       0x461, a + 0x314, 0, 0, 255, 0, -1, 0, 0, 0));
  store<u32>(a + 0x9DC,
             call<u32>(0x025A847C, load<u32>(call<u32>(0x025200D4) + 0x5AB0), 0,
                       0x462, a + 0x314, 0, 0, 255, 0, -1, 0, 0, 0));
  store<u32>(a + 0x9E0,
             call<u32>(0x025A847C, load<u32>(call<u32>(0x025200D4) + 0x5AB0), 4,
                       0x445B, a + 0x314, 0, 0, 255, 0, -1, 0, 0, 0));
  if (!CreateInit(a))
    return 5;
  ctrlEffect(a);
  return 4;
}
VERIFY(0x0213F3D0, create);
s32 createWrapper(u32 a) {
  WWHD_FUNC(0x0213F5CC, s32, a);
  return create(a);
}
VERIFY(0x0213F5CC, createWrapper);
bool remove(u32 a) {
  WWHD_FUNC(0x0213F5D0, bool, a);
  for (int i = 0; i < 3; ++i) {
    u32 e = load<u32>(a + 0x9D8 + 4 * i);
    if (e) {
      u32 flags = load<u32>(e + 0x254);
      store<s32>(e + 0x5C, -1);
      store<u32>(e + 0x254, flags | 1);
      store<u32>(a + 0x9D8 + 4 * i, 0);
    }
  }
  return true;
}
VERIFY(0x0213F5D0, remove);
bool deleteWrapper(u32 a) {
  WWHD_FUNC(0x0213F648, bool, a);
  return remove(a);
}
VERIFY(0x0213F648, deleteWrapper);
bool draw(u32 a) {
  WWHD_FUNC(0x0213F64C, bool, a);
  return true;
}
VERIFY(0x0213F64C, draw);
void demo(u32 a) {
  WWHD_FUNC(0x0213F654, void, a);
  s32 staff = load<s32>(a + 0xA14);
  u32 act = call<u32>(0x02542EDC, call<u32>(0x025200D4) + 0x52C4, staff,
                      0x101B5030u, 3, 0, 0);
  if (act == 1) {
    if (!load<u8>(a + 0xA1E)) {
      s32 rev = call<s32>(0x02520540, load<s8>(a + 0x326));
      call<void>(0x025E1A40, 0x694E, a + 0x37C, 0, rev);
    }
    playFire(a);
    staff = load<s32>(a + 0xA14);
    store<u8>(a + 0xA1E, 1);
  } else if (act == 2) {
    u32 e = load<u32>(a + 0x9D8);
    if (e && (load<u32>(e + 0x1B4) + load<u32>(e + 0x1C0)) &&
        load<u8>(a + 0xA1E)) {
      s32 rev = call<s32>(0x02520540, load<s8>(a + 0x326));
      call<void>(0x025E1A40, 0x6950, a + 0x37C, 0, rev);
    }
    stop(a);
    staff = load<s32>(a + 0xA14);
    store<u8>(a + 0xA1E, 0);
  } else
    staff = load<s32>(a + 0xA14);
  call<void>(0x02543280, call<u32>(0x025200D4) + 0x52C4, staff);
}
VERIFY(0x0213F654, demo);
void checkOrder(u32 a) {
  WWHD_FUNC(0x0213F7A4, void, a);
  if (load<u16>(a + 0xF8) != 2)
    return;
  s16 ev = load<s16>(a + 0xA18);
  bool reset = call<s32>(0x0254407C, call<u32>(0x025200D4) + 0x52C4, ev) &&
               load<u32>(a + 0xA10) != 0;
  if (!reset) {
    ev = load<s16>(a + 0xA1A);
    reset = call<s32>(0x0254407C, call<u32>(0x025200D4) + 0x52C4, ev) &&
            load<u32>(a + 0xA10) != 0;
  }
  if (reset)
    store<u32>(a + 0xA10, 0);
  ev = load<s16>(a + 0xA18);
  bool ended =
      ev != -1 && call<s32>(0x025440C8, call<u32>(0x025200D4) + 0x52C4, ev);
  if (!ended) {
    ev = load<s16>(a + 0xA1A);
    ended =
        ev != -1 && call<s32>(0x025440C8, call<u32>(0x025200D4) + 0x52C4, ev);
  }
  if (ended) {
    u32 p = call<u32>(0x025200D4);
    store<u16>(p + 0x52B8, load<u16>(p + 0x52B8) | 8);
  }
  u32 name = load<u32>(0x101B503C + 4 * load<u8>(a + 0xA1C));
  store<s32>(a + 0xA14,
             call<s32>(0x02542D88, call<u32>(0x025200D4) + 0x52C4, name, 0, 0));
  demo(a);
}
VERIFY(0x0213F7A4, checkOrder);
void eventOrder(u32 a) {
  WWHD_FUNC(0x0213F8C4, void, a);
  u8 off = call<u32>(0x025B8C74, load<u32>(0x101F84DC) + 0x798,
                     load<u32>(a + 0x9E8));
  u8 type = load<u8>(a + 0xA1C);
  if (type == 1) {
    if (call<s32>(0x025D98E8, load<s8>(a + 0x326)))
      store<u8>(a + 0xA1F, 5);
    else if (!call<u8>(0x0207A9A0, a + 0xA1F))
      off = 1;
  } else if (type == 0 && call<s32>(0x025BA0C0, load<u32>(0x101F84DC) + 0x20,
                                    load<s32>(a + 0x9E4), load<s8>(a + 0x2FE)))
    off = 1;
  s32 state = load<s32>(a + 0xA10);
  if (state == 1 || state == 2) {
    s16 ev = load<s16>(a + (state == 1 ? 0xA18 : 0xA1A));
    call<s32>(0x025D7A58, a, ev, 255, 65535, 0, 1);
    store<u16>(a + 0xFA, load<u16>(a + 0xFA) | 2);
    store<u8>(a + 0xA09, off != 0);
    return;
  }
  if (state == 0 && load<s8>(a + 0xA1D) < 0 && load<u8>(a + 0xA09) != off)
    store<u32>(a + 0xA10, off ? 2 : 1);
  store<u8>(a + 0xA09, off != 0);
}
VERIFY(0x0213F8C4, eventOrder);
void checkCol(u32 a) {
  WWHD_FUNC(0x0213FA58, void, a);
  if (load<u8>(a + 0x9EC) != 1) {
    call<void>(0x020182E0, a + 0x500, a + 0x314);
    call<void>(0x0200E240, call<u32>(0x025200D4) + 0x26A4, a + 0x3E8);
  }
  call<void>(0x020182E0, a + 0x9C0, a + 0x314);
  call<void>(0x0200E240, call<u32>(0x025200D4) + 0x26A4, a + 0x8A8);
  call<void>(0x020182E0, a + 0x890, a + 0x314);
  call<void>(0x0200E240, call<u32>(0x025200D4) + 0x26A4, a + 0x778);
}
VERIFY(0x0213FA58, checkCol);
bool execute(u32 a) {
  WWHD_FUNC(0x0213FAF4, bool, a);
  s8 t = load<s8>(a + 0xA1D);
  if (t >= 0)
    store<s8>(a + 0xA1D, t - 1);
  checkOrder(a);
  ctrlEffect(a);
  eventOrder(a);
  checkCol(a);
  return true;
}
VERIFY(0x0213FAF4, execute);
bool executeWrapper(u32 a) {
  WWHD_FUNC(0x0213FB54, bool, a);
  return execute(a);
}
VERIFY(0x0213FB54, executeWrapper);
void staticInit() {
  WWHD_FUNC(0x0213FB58, void);
  store<u32>(0x10463DCC, 0);
  store<u32>(0x10463DC4, 0);
  store<u32>(0x10463DD0, 0);
  store<u32>(0x10463DC8, 0);
  call<void>(0x028F026C, 0x101B5044u);
  f32 lo = load<f32>(0x1000F168), hi = load<f32>(0x1000F16C);
  store<f32>(0x10463DB8, lo);
  store<f32>(0x10463DBC, hi);
  call<void>(0x028ED6F8, 0x10463DC0u);
  call<void>(0x028F026C, 0x101B5050u);
  call<void>(0x028EAB2C, 0x10463DC1u);
  call<void>(0x028F026C, 0x101B505Cu);
}
VERIFY(0x0213FB58, staticInit);
bool isDelete(u32 a) {
  WWHD_FUNC(0x0213FC78, bool, a);
  return true;
}
VERIFY(0x0213FC78, isDelete);
void actorDtor(u32 a, u32 flags) {
  WWHD_FUNC(0x0213FC80, void, a, flags);
  if (a) {
    call<void>(0x02515A70, a + 0x8A8, 2);
    call<void>(0x02515A70, a + 0x778, 2);
    call<void>(0x028F0164, a + 0x3E8, 3, 0x130, 0x02515A70u, 0, 0);
    call<void>(0x02515860, a + 0x3AC, 2);
    call<void>(0x025D50BC, a, 0);
    if (flags & 1)
      call<void>(0x0273AF40, a);
  }
}
VERIFY(0x0213FC80, actorDtor);
} // namespace fire

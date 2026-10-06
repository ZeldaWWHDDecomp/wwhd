// Dragon Roost lava slab. Reconstructed from the HD translation unit.
#include "d/actor/d_a_obj_magmarock.h"
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
template <class T> static T read(u32 a) { return gabi::load<T>(a); }
template <class T> static void write(u32 a, T v) { gabi::store<T>(a, v); }
static void process(u32 a, u32 fn) {
  write<s16>(a + 0x3F8, 0);
  write<s16>(a + 0x3FA, -1);
  write<u32>(a + 0x3FC, fn);
}
static void shock() {
  u32 g = gabi::call<u32>(0x025200D4);
  gabi::Local<be<f32>[3]> axis;
  (*axis)[0] = 0;
  (*axis)[1] = 1;
  (*axis)[2] = 0;
  gabi::call(0x025CB374, ptr(g + 0x599C), 4, 1, axis.get());
}
static u32 resource(s32 index) {
  gabi::Local<be<u32>[2]> name;
  (*name)[0] = 0x1002C328;
  (*name)[1] = 0x1002C1F0;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), name.get(),
                         index);
}
static void sound(u32 a, u32 id, u32 position) {
  s32 rv = gabi::call<s32>(0x02520540, read<s8>(a + 0x326));
  gabi::call(0x025E1A40, id, ptr(position), 0, rv);
}
static BOOL createHeap(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236CFC8, BOOL, self);
  u32 a = gabi::ea(self);
  u32 modelData = resource(9);
  if (!modelData)
    gabi::call(0x0273AA24, ptr(0x1002C230), 0x14D, ptr(0x1002C248));
  write<u32>(a + 0x408,
             gabi::call<u32>(0x025E38E0, ptr(modelData), 0, 0x11020203));
  write<u32>(a + 0x40C, resource(12));
  u32 anim = resource(6);
  write<u32>(a + 0x488, anim);
  if (!read<u32>(a + 0x40C)) {
    gabi::call(0x0273AA24, ptr(0x1002C230), 0x155, ptr(0x1002C25C));
    anim = read<u32>(a + 0x488);
  }
  if (!anim)
    gabi::call(0x0273AA24, ptr(0x1002C230), 0x156, ptr(0x1002C26C));
  s32 brk = gabi::call<s32>(0x025E8154, ptr(a + 0x410), ptr(modelData),
                            ptr(read<u32>(a + 0x40C)), 0, 2, 0, 1.0f, -1, 0, 0);
  s32 bck = gabi::call<s32>(0x025E8508, ptr(a + 0x48C), ptr(modelData),
                            ptr(read<u32>(a + 0x488)), 0, 2, 0, 1.0f, -1, 0);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), read<f32>(a + 0x314),
             read<f32>(a + 0x318), read<f32>(a + 0x31C));
  gabi::call(0x025F1C28, ptr(0x1048D0CC), read<s16>(a + 0x32A));
  gabi::call(0x025F2518, read<f32>(a + 0x330), read<f32>(a + 0x334),
             read<f32>(a + 0x338));
  gabi::call(0x028E90D4, ptr(0x1048D0CC), ptr(a + 0x518));
  u32 bg = gabi::call<u32>(0x024F2478, ptr(resource(15)), 1, ptr(a + 0x518));
  write<u32>(a + 0x548, bg);
  if (!bg)
    return 0;
  write<u32>(bg + 0xA8, 0x024EE658);
  return read<u32>(a + 0x408) && brk && bck;
}
VERIFY(0x0236CFC8, createHeap);
static BOOL checkHeap(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236D20C, BOOL, self);
  return createHeap(self);
}
VERIFY(0x0236D20C, checkHeap);
static void setMtx(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236D210, void, self);
  u32 a = gabi::ea(self);
  f32 x = read<f32>(a + 0x330), y = read<f32>(a + 0x334),
      z = read<f32>(a + 0x338);
  u32 model = read<u32>(a + 0x408);
  write<f32>(model + 0xBC, x);
  write<f32>(model + 0xC0, y);
  write<f32>(model + 0xC4, z);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), read<f32>(a + 0x314),
             read<f32>(a + 0x318), read<f32>(a + 0x31C));
  gabi::call(0x025F1B48, ptr(0x1048D0CC), read<s16>(a + 0x328),
             read<s16>(a + 0x32A), read<s16>(a + 0x32C));
  gabi::Local<be<f32>[4]> quat;
  gabi::call(0x028E8B48, ptr(a + 0x3C8), ptr(a + 0x3E8), quat.get());
  gabi::call(0x025F25CC, quat.get());
  f32 m[12];
  for (int i = 0; i < 12; i++)
    m[i] = read<f32>(0x1048D0CC + i * 4);
  model = read<u32>(a + 0x408);
  for (int i = 0; i < 12; i++)
    write<f32>(model + 0xC8 + i * 4, m[i]);
  gabi::call(0x028E90D4, ptr(0x1048D0CC), ptr(a + 0x518));
}
VERIFY(0x0236D210, setMtx);
static void rideCallback(u8 *bg, daObjMagmarock_c *self, u8 *rider) {
  WWHD_FUNC(0x0236D314, void, bg, self, rider);
  u32 a = gabi::ea(self);
  gabi::Local<be<u32>[3]> delta, axis, cross;
  gabi::call(0x0201ADE0, ptr(gabi::ea(rider) + 0x314), delta.get(),
             ptr(a + 0x314));
  (*axis)[0] = 0;
  (*axis)[1] = 0x3F800000;
  (*axis)[2] = 0;
  gabi::call(0x0201B080, delta.get(), cross.get(), axis.get());
  for (int i = 0; i < 3; i++)
    (*delta)[i] = (u32)(*cross)[i];
  f32 magnitude = gabi::call<f32>(0x028E8DD0, delta.get());
  magnitude = gabi::call<f32>(0x028F4384, magnitude);
  if (gabi::call<s32>(0x0201B47C, delta.get())) {
    f32 displacement = read<f32>(a + 0x318) - read<f32>(a + 0x2F0);
    f32 rate = gabi::fmadds(4.0f, displacement * 0.001f, 2.0f);
    gabi::call(0x0200F428, ptr(a + 0x3B0), (s16)gabi::ftoi(-(magnitude * rate)),
               8, 0x200);
    u32 table = 0x104A44F8 + (read<u16>(a + 0x3B0) >> 3) * 8;
    write<s16>(a + 0x3B4, 1);
    write<u8>(a + 0x3B6, 1);
    f32 s = read<f32>(table);
    for (int i = 0; i < 3; i++)
      write<f32>(a + 0x3D8 + i * 4,
                 read<f32>(gabi::ea(delta.get()) + i * 4) * s);
    write<f32>(a + 0x3E4, read<f32>(table + 4));
  }
}
VERIFY(0x0236D314, rideCallback);
static void stayInit(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236D46C, void, self);
  u32 a = gabi::ea(self);
  u32 n = read<u8>(a + 0xB3);
  if (n == 255)
    n = 0;
  write<f32>(a + 0x740, 30);
  write<f32>(a + 0x73C, 30);
  write<u32>(a + 0x754, 330);
  write<u32>(a + 0x750, n * 15 + 30);
  u32 g = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, ptr(g + 0x12A0), ptr(read<u32>(a + 0x548)), self);
  process(a, 0x0236E810);
}
VERIFY(0x0236D46C, stayInit);
static void appearInit(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236D4F8, void, self);
  write<u32>(gabi::ea(self) + 0x750, 30);
  process(gabi::ea(self), 0x0236E738);
}
VERIFY(0x0236D4F8, appearInit);
static void waitInit(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E710, void, self);
  write<u32>(gabi::ea(self) + 0x750, 300);
  process(gabi::ea(self), 0x0236E800);
}
VERIFY(0x0236E710, waitInit);
static void vanishInit(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E834, void, self);
  write<u32>(gabi::ea(self) + 0x750, 90);
  process(gabi::ea(self), 0x0236E8E8);
}
VERIFY(0x0236E834, vanishInit);
static void calcGroundQuat(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236DD08, void, self);
  u32 a = gabi::ea(self);
  u32 g = gabi::call<u32>(0x025200D4);
  f32 height;
  if (read<u32>(g + 0x5AB4)) {
    g = gabi::call<u32>(0x025200D4);
    height =
        gabi::call<f32>(0x0258CAEC, ptr(read<u32>(g + 0x5AB4)), ptr(a + 0x314));
  } else
    height = read<f32>(a + 0x318) - 10.0f;
  if (height > -99999992.0f)
    write<f32>(a + 0x2F0, height + 25.0f);
  write<f32>(a + 0x2EC, read<f32>(a + 0x314));
  write<f32>(a + 0x734, -60);
  write<f32>(a + 0x71C, 120);
  write<f32>(a + 0x72C, -103.9f);
  write<f32>(a + 0x724, 0);
  write<f32>(a + 0x718, 0);
  write<f32>(a + 0x730, 0);
  write<f32>(a + 0x714, 0);
  write<f32>(a + 0x728, -60);
  write<f32>(a + 0x2F4, read<f32>(a + 0x31C));
  write<f32>(a + 0x720, 103.9f);
  for (int i = 0; i < 3; i++) {
    u32 v = a + 0x714 + i * 12;
    gabi::call(0x028E8D88, ptr(v), ptr(a + 0x2EC), ptr(v));
    g = gabi::call<u32>(0x025200D4);
    if (read<u32>(g + 0x5AB4)) {
      g = gabi::call<u32>(0x025200D4);
      height = gabi::call<f32>(0x0258CAEC, ptr(read<u32>(g + 0x5AB4)), ptr(v));
    } else
      height = read<f32>(a + 0x318) - 10.0f;
    if (height > -99999992.0f)
      write<f32>(v + 4, height + 15.0f);
  }
  gabi::call(0x02588544, ptr(a + 0x3E8), ptr(a + 0x714), ptr(a + 0x720),
             ptr(a + 0x72C), 0.25f);
}
VERIFY(0x0236DD08, calcGroundQuat);
static void demoMove(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236DEC4, void, self);
  u32 a = gabi::ea(self);
  if (gabi::call<s32>(0x025B8B94, ptr(read<u32>(0x101F84DC) + 0x644), 0x380) ||
      read<u8>(a + 0x3B7))
    return;
  s16 state = read<s16>(a + 0x762);
  if (!state) {
    if (read<u16>(a + 0xF8) == 2) {
      write<s16>(a + 0x762, 1);
      return;
    }
    gabi::call(0x025D77DC, self, ptr(0x1002C2E0), 1, 0xFFFF);
    write<u16>(a + 0xFA, read<u16>(a + 0xFA) | 2);
    return;
  }
  if (state == 1) {
    u32 g = gabi::call<u32>(0x025200D4);
    s32 staff =
        gabi::call<s32>(0x02542D88, ptr(g + 0x52C4), ptr(0x1002C2D8), 0, 0);
    g = gabi::call<u32>(0x025200D4);
    s32 ended = gabi::call<s32>(0x0254457C, ptr(g + 0x52C4), ptr(0x1002C2E0));
    g = gabi::call<u32>(0x025200D4);
    if (ended) {
      write<u16>(g + 0x52B8, read<u16>(g + 0x52B8) | 8);
      write<s16>(a + 0x762, (s16)(read<s16>(a + 0x762) + 1));
      gabi::call(0x025B8B68, ptr(read<u32>(0x101F84DC) + 0x644), 0x380);
    } else
      gabi::call(0x02543280, ptr(g + 0x52C4), staff);
  }
}
VERIFY(0x0236DEC4, demoMove);
static u32 particle(u32 a, u32 group, u32 effect, u32 alpha) {
  u32 g = gabi::call<u32>(0x025200D4);
  return gabi::call<u32>(0x025A847C, ptr(read<u32>(g + 0x5AB0)), group, effect,
                         ptr(a + 0x314), 0, 0, alpha, 0, -1, 0, 0, 0);
}
static void endParticle(u32 a, u32 off) {
  u32 p = read<u32>(a + off);
  if (p) {
    u32 flags = read<u32>(p + 0x254);
    write<u32>(p + 0x5C, -1);
    write<u32>(p + 0x254, flags | 1);
    write<u32>(a + off, 0);
  }
}
static void moveParticle(u32 a, u32 p) {
  u8 kind = read<u8>(p + 0x262);
  f32 y = read<f32>(a + 0x318), x = read<f32>(a + 0x314),
      z = read<f32>(a + 0x31C);
  if (kind >= 7)
    y = -y;
  write<f32>(p + 0x230, y);
  write<f32>(p + 0x22C, x);
  write<f32>(p + 0x234, z);
}
static void controlEffect(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E054, void, self);
  u32 a = gabi::ea(self);
  if (read<s16>(a + 0x764)) {
    s16 before = read<s16>(a + 0x766);
    u32 p = read<u32>(a + 0x3C0);
    if (before) {
      if (!p)
        write<u32>(a + 0x3C0, particle(a, 0, 0x8104, 255));
      else
        moveParticle(a, p);
      return;
    }
    endParticle(a, 0x3C0);
    p = read<u32>(a + 0x3C4);
    if (!p) {
      shock();
      write<u32>(a + 0x3C4, particle(a, 2, 0x8105, 255));
    } else
      moveParticle(a, p);
  } else
    endParticle(a, 0x3C4);
}
VERIFY(0x0236E054, controlEffect);
static s32 animationFrames(u32 p) {
  u32 table = read<u32>(p + 4);
  return gabi::call<s32>(read<u32>(table + 0x14), ptr(p));
}
static void playAnim(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E268, void, self);
  u32 a = gabi::ea(self);
  s32 elapsed = read<s32>(a + 0x754);
  bool fade = false;
  if (elapsed > 375) {
    f32 frame = read<f32>(a + 0x740);
    if (frame > 0) {
      write<f32>(a + 0x740, frame - 1.0f);
      elapsed = read<s32>(a + 0x754);
      fade = true;
    }
  }
  if (!fade && ((u32)elapsed - 15) >= 46) {
    s32 frames = animationFrames(read<u32>(a + 0x40C));
    if (read<f32>(a + 0x740) < (f32)frames)
      write<f32>(a + 0x740, read<f32>(a + 0x740) + 1.0f);
    elapsed = read<s32>(a + 0x754);
  }
  if (elapsed < 60) {
    s32 frames = animationFrames(read<u32>(a + 0x488));
    if (read<f32>(a + 0x73C) < (f32)frames) {
      write<f32>(a + 0x73C, read<f32>(a + 0x73C) + 1.0f);
      return;
    }
    elapsed = read<s32>(a + 0x754);
  }
  if (elapsed > 375) {
    f32 frame = read<f32>(a + 0x73C);
    if (frame > 0)
      write<f32>(a + 0x73C, frame - 1.0f);
  }
}
VERIFY(0x0236E268, playAnim);
static bool isProcess(u32 a, u32 fn) {
  return read<s16>(a + 0x3FA) == -1 && read<s16>(a + 0x3F8) == 0 &&
         read<u32>(a + 0x3FC) == fn;
}
static BOOL liftUp(daObjMagmarock_c *self, u8 *position) {
  WWHD_FUNC(0x0236E400, BOOL, self, position);
  u32 a = gabi::ea(self), p = gabi::ea(position);
  write<u32>(a + 0x744, read<u32>(p));
  write<u32>(a + 0x748, read<u32>(p + 4));
  write<u32>(a + 0x74C, read<u32>(p + 8));
  if (!isProcess(a, 0x0236E800)) {
    if (isProcess(a, 0x0236E738)) {
      gabi::Local<be<f32>[3]> delta;
      gabi::call(0x0201ADE0, ptr(a + 0x314), delta.get(), ptr(a + 0x744));
      (*delta)[1] = 0;
      if (!gabi::call<s32>(0x0201B47C, delta.get())) {
        (*delta)[0] = 0;
        (*delta)[1] = 0;
        (*delta)[2] = 1;
      }
      gabi::call(0x028E8E64, delta.get(), delta.get(), 10.0f);
      gabi::call(0x028E8D88, ptr(a + 0x314), delta.get(), ptr(a + 0x314));
    }
    return 0;
  }
  gabi::call(0x0200F164, ptr(a + 0x314), position, 0.05f, 5.0f);
  gabi::call(0x0200ED84, ptr(a + 0x738), 750.0f, 0.5f, 40.0f);
  gabi::call(0x0200F428, ptr(a + 0x75E), 0x1200, 4, 0x100);
  write<s16>(a + 0x75C, (s16)(read<s16>(a + 0x75C) + read<s16>(a + 0x75E)));
  gabi::call(0x0200ED84, ptr(a + 0x318), read<f32>(p + 4), 0.25f, 150.0f);
  write<s16>(a + 0x764, 1);
  return 1;
}
VERIFY(0x0236E400, liftUp);
static BOOL beforeLift(daObjMagmarock_c *self, u8 *position) {
  WWHD_FUNC(0x0236E5BC, BOOL, self, position);
  u32 a = gabi::ea(self), p = gabi::ea(position);
  f32 minY = read<f32>(a + 0x2F0) + 25.0f;
  write<f32>(a + 0x744, read<f32>(p));
  f32 y = read<f32>(p + 4);
  write<f32>(a + 0x748, y);
  write<f32>(a + 0x74C, read<f32>(p + 8));
  if (y < minY)
    write<f32>(a + 0x748, minY);
  if (!isProcess(a, 0x0236E800))
    return 0;
  gabi::call(0x0200F164, ptr(a + 0x314), ptr(a + 0x744), 0.05f, 5.0f);
  gabi::call(0x0200ED84, ptr(a + 0x738), 500.0f, 0.25f, 20.0f);
  gabi::call(0x0200F428, ptr(a + 0x75E), 0xA00, 8, 0x100);
  write<s16>(a + 0x75C, (s16)(read<s16>(a + 0x75C) + read<s16>(a + 0x75E)));
  gabi::call(0x0200ED84, ptr(a + 0x318), read<f32>(a + 0x748), 0.25f, 150.0f);
  write<s16>(a + 0x766, 1);
  write<s16>(a + 0x764, 1);
  return 1;
}
VERIFY(0x0236E5BC, beforeLift);
static void appear(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E738, void, self);
  u32 a = gabi::ea(self);
  s32 timer = read<s32>(a + 0x750);
  if (timer == 10) {
    u32 g = gabi::call<u32>(0x025200D4);
    gabi::call(0x024EEA6C, ptr(g + 0x12A0), ptr(read<u32>(a + 0x548)), self);
    timer = read<s32>(a + 0x750);
  }
  if (!timer)
    waitInit(self);
}
VERIFY(0x0236E738, appear);
static void quakeInit(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E794, void, self);
  u32 a = gabi::ea(self);
  sound(a, 0x380F, a + 0x37C);
  write<u32>(a + 0x750, 45);
  process(a, 0x0236E85C);
}
VERIFY(0x0236E794, quakeInit);
static void waitProc(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E800, void, self);
  if (!read<u32>(gabi::ea(self) + 0x750))
    quakeInit(self);
}
VERIFY(0x0236E800, waitProc);
static void stay(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E810, void, self);
  u32 a = gabi::ea(self);
  if (read<u8>(a + 0x3B6)) {
    u32 timer = read<u32>(a + 0x750);
    write<u32>(a + 0x750, timer - 1);
    if (!timer)
      quakeInit(self);
  }
}
VERIFY(0x0236E810, stay);
static void quake(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E85C, void, self);
  u32 a = gabi::ea(self);
  write<s16>(a + 0x75C, (s16)(read<s16>(a + 0x75E) + read<s16>(a + 0x75C)));
  gabi::call(0x0200F428, ptr(a + 0x75E), 0x1000, 2, 0x100);
  gabi::call(0x0200ED84, ptr(a + 0x738), read<f32>(0x1047BBD8) + 750.0f, 0.25f,
             50.0f);
  if (!read<u32>(a + 0x750))
    vanishInit(self);
}
VERIFY(0x0236E85C, quake);
static void vanish(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E8E8, void, self);
  u32 a = gabi::ea(self);
  write<s16>(a + 0x75C, (s16)(read<s16>(a + 0x75C) + read<s16>(a + 0x75E)));
  gabi::call(0x0200F428, ptr(a + 0x75E), 0, 4, 0x40);
  s32 timer = read<s32>(a + 0x750);
  if (timer == 80) {
    u32 g = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, ptr(g + 0x12A0), ptr(read<u32>(a + 0x548)));
    timer = read<s32>(a + 0x750);
  }
  if (timer < 0)
    gabi::call(0x025D57E0, self);
}
VERIFY(0x0236E8E8, vanish);
static BOOL deleteActor(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236EA48, BOOL, self);
  u32 a = gabi::ea(self);
  gabi::call(0x025204C8, ptr(a + 0x400), ptr(0x1002C328));
  if (read<u32>(a + 0xF4) && read<u32>(read<u32>(a + 0x548)) < 256) {
    u32 g = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, ptr(g + 0x12A0), ptr(read<u32>(a + 0x548)));
  }
  return 1;
}
VERIFY(0x0236EA48, deleteActor);
static void initStatics() {
  WWHD_FUNC(0x0236F150, void);
  for (int i = 0; i < 4; i++)
    write<u32>(0x1046A4F8 + i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CAA48));
  write<f32>(0x1046A4EC, -3.1415927410125732f);
  write<f32>(0x1046A4F0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046A4F4));
  gabi::call(0x028F026C, ptr(0x101CAA54));
  gabi::call(0x028EAB2C, ptr(0x1046A4F5));
  gabi::call(0x028F026C, ptr(0x101CAA60));
}
VERIFY(0x0236F150, initStatics);
static void destroyStatic(u8 *self, u32 flags) {
  WWHD_FUNC(0x0236F1E4, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x0236F1E4, destroyStatic);
static void emptyStatic(u8 *self) { WWHD_FUNC(0x0236F1F8, void, self); }
VERIFY(0x0236F1F8, emptyStatic);
static void destroyMagmarock(daObjMagmarock_c *self, u32 flags) {
  WWHD_FUNC(0x0236F1FC, void, self, flags);
  if (self) {
    gabi::call(0x027F3628, ptr(gabi::ea(self) + 0x49C), 0);
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x0236F1FC, destroyMagmarock);
static BOOL isDelete(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236F25C, BOOL, self);
  return 1;
}
VERIFY(0x0236F25C, isDelete);
static daObjMagmarock_c *construct(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236CDB8, daObjMagmarock_c *, self);
  if (!self) {
    self = gabi::call<daObjMagmarock_c *>(0x0273AD10, 0x768);
    if (!self)
      return self;
  }
  u32 a = gabi::ea(self);
  gabi::call(0x025D4ED0, self);
  write<u32>(a + 0xB4, 0x1002C330);
  gabi::call(0x025E80D0, ptr(a + 0x410));
  gabi::call(0x027F2BC0, ptr(a + 0x48C), 0);
  write<u32>(a + 0x49C, 0x1016E54C);
  gabi::call(0x027DA984, ptr(a + 0x4A0));
  write<u32>(a + 0x508, 0);
  write<u32>(a + 0x50C, 0);
  write<u32>(a + 0x49C, 0x1002C208);
  write<u32>(a + 0x510, 0);
  write<u32>(a + 0x514, 0);
  write<u32>(a + 0x4D4, 0x1016D820);
  write<u32>(a + 0x4E4, 0);
  write<f32>(a + 0x54C, read<f32>(0x1016E414));
  write<f32>(a + 0x550, read<f32>(0x1016E418));
  write<f32>(a + 0x554, read<f32>(0x1016E41C));
  write<f32>(a + 0x558, read<f32>(0x1016E420));
  write<f32>(a + 0x55C, read<f32>(0x1016E424));
  write<f32>(a + 0x560, read<f32>(0x1016E428));
  write<u8>(a + 0x564, read<u8>(0x1016E42C));
  write<u8>(a + 0x565, read<u8>(0x1016E42D));
  write<u8>(a + 0x566, read<u8>(0x1016E42E));
  write<u8>(a + 0x567, read<u8>(0x1016E42F));
  write<s16>(a + 0x568, read<s16>(0x1016E430));
  write<s16>(a + 0x56A, read<s16>(0x1016E432));
  write<s16>(a + 0x56C, read<s16>(0x1016E434));
  write<s16>(a + 0x56E, read<s16>(0x1016E436));
  write<f32>(a + 0x570, read<f32>(0x1016E438));
  write<f32>(a + 0x574, read<f32>(0x1016E43C));
  write<f32>(a + 0x578, read<f32>(0x1016E440));
  write<f32>(a + 0x57C, read<f32>(0x1016E444));
  write<f32>(a + 0x580, read<f32>(0x1016E448));
  write<f32>(a + 0x584, read<f32>(0x1016E44C));
  write<f32>(a + 0x588, read<f32>(0x1016E450));
  write<f32>(a + 0x634, read<f32>(0x1016E43C));
  write<f32>(a + 0x58C, read<f32>(0x1016E454));
  write<f32>(a + 0x6A4, read<f32>(0x1016E428));
  write<f32>(a + 0x64C, read<f32>(0x1016E454));
  write<s16>(a + 0x6B0, read<s16>(0x1016E434));
  write<f32>(a + 0x620, read<f32>(0x1016E428));
  write<u8>(a + 0x626, read<u8>(0x1016E42E));
  write<f32>(a + 0x640, read<f32>(0x1016E448));
  write<f32>(a + 0x61C, read<f32>(0x1016E424));
  write<s16>(a + 0x62E, read<s16>(0x1016E436));
  write<f32>(a + 0x6B8, read<f32>(0x1016E43C));
  write<s16>(a + 0x628, read<s16>(0x1016E430));
  write<f32>(a + 0x630, read<f32>(0x1016E438));
  write<u8>(a + 0x6A9, read<u8>(0x1016E42D));
  write<f32>(a + 0x698, read<f32>(0x1016E41C));
  write<f32>(a + 0x638, read<f32>(0x1016E440));
  write<f32>(a + 0x60C, read<f32>(0x1016E414));
  write<u8>(a + 0x6AB, read<u8>(0x1016E42F));
  write<f32>(a + 0x614, read<f32>(0x1016E41C));
  write<f32>(a + 0x69C, read<f32>(0x1016E420));
  write<s16>(a + 0x62C, read<s16>(0x1016E434));
  write<s16>(a + 0x6AC, read<s16>(0x1016E430));
  write<f32>(a + 0x690, read<f32>(0x1016E414));
  write<f32>(a + 0x6B4, read<f32>(0x1016E438));
  write<f32>(a + 0x644, read<f32>(0x1016E44C));
  write<u8>(a + 0x6AA, read<u8>(0x1016E42E));
  write<f32>(a + 0x6BC, read<f32>(0x1016E440));
  write<u8>(a + 0x627, read<u8>(0x1016E42F));
  write<s16>(a + 0x6AE, read<s16>(0x1016E432));
  write<u8>(a + 0x625, read<u8>(0x1016E42D));
  write<f32>(a + 0x694, read<f32>(0x1016E418));
  write<f32>(a + 0x610, read<f32>(0x1016E418));
  write<f32>(a + 0x648, read<f32>(0x1016E450));
  write<u8>(a + 0x624, read<u8>(0x1016E42C));
  write<f32>(a + 0x63C, read<f32>(0x1016E444));
  write<f32>(a + 0x6A0, read<f32>(0x1016E424));
  write<u8>(a + 0x6A8, read<u8>(0x1016E42C));
  write<s16>(a + 0x62A, read<s16>(0x1016E432));
  write<f32>(a + 0x6C0, read<f32>(0x1016E444));
  write<s16>(a + 0x6B2, read<s16>(0x1016E436));
  write<f32>(a + 0x618, read<f32>(0x1016E420));
  write<f32>(a + 0x6C4, read<f32>(0x1016E448));
  write<f32>(a + 0x6C8, read<f32>(0x1016E44C));
  write<f32>(a + 0x6CC, read<f32>(0x1016E450));
  write<f32>(a + 0x6D0, read<f32>(0x1016E454));
  return self;
}
VERIFY(0x0236CDB8, construct);
// Apply the slab's warm light bias to the three signed TEV channels and RGB.
static void brighten(u32 a) {
  for (int i = 0; i < 3; i++) {
    s32 v = read<s16>(a + 0x5DC + i * 2);
    write<u16>(a + 0x5DC + i * 2, (u8)(v + gabi::ftoi((f32)(255 - v) * 0.12f)));
  }
  for (int i = 0; i < 3; i++) {
    s32 v = read<u8>(a + 0x5E4 + i);
    write<u8>(a + 0x5E4 + i, (u8)(v + gabi::ftoi((f32)(255 - v) * 0.12f)));
  }
}
static BOOL createInit(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236D520, BOOL, self);
  u32 a = gabi::ea(self);
  write<f32>(a + 0x330, 1);
  u32 model = read<u32>(a + 0x408);
  write<f32>(a + 0x334, 1);
  write<f32>(a + 0x338, 1);
  write<u32>(a + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, self, -200.0f, -30.0f, -200.0f, 200.0f, 30.0f, 200.0f);
  model = read<u32>(a + 0x408);
  gabi::call(0x028E90D4, ptr(model ? model + 0xC8 : 0), ptr(a + 0x518));
  f32 x = read<f32>(a + 0x314), y = read<f32>(a + 0x318);
  write<s16>(a + 0x75C, 0);
  write<s16>(a + 0x75E, 0);
  write<f32>(a + 0x2EC, x);
  write<s16>(a + 0x762, 0);
  write<u32>(a + 0x758, 0);
  write<f32>(a + 0x33C, 0);
  write<f32>(a + 0x340, 0);
  f32 homeY = y + 15.0f;
  f32 z = read<f32>(a + 0x31C);
  write<f32>(a + 0x344, 0);
  write<f32>(a + 0x374, -2.5f);
  write<s16>(a + 0x3B0, 0);
  write<s16>(a + 0x3B2, 0);
  write<f32>(a + 0x2F4, z);
  write<f32>(a + 0x738, 0);
  write<u8>(a + 0x3B6, 0);
  write<u32>(a + 0x750, 30);
  write<f32>(a + 0x2F0, homeY);
  write<f32>(a + 0x740, 0);
  write<u32>(a + 0x754, 0);
  for (int i = 0; i < 4; i++) {
    u32 value = read<u32>(0x101E9C38 + i * 4);
    write<u32>(a + 0x3E8 + i * 4, value);
    write<u32>(a + 0x3D8 + i * 4, value);
    write<u32>(a + 0x3C8 + i * 4, value);
  }
  setMtx(self);
  write<u32>(read<u32>(a + 0x548) + 0xB0, 0x0236D314);
  u8 stayFlag = (read<u32>(a + 0xB0) >> 24);
  write<u8>(a + 0x3B7, stayFlag);
  if (stayFlag) {
    stayInit(self);
    return 1;
  }
  appearInit(self);
  if (read<u32>(a + 0x3B8))
    return 1;
  sound(a, 0x380E, a + 0x37C);
  shock();
  // Copy the existing actor lighting to the slab's independent lighting record.
  write<u8>(a + 0x564, read<u8>(a + 0x128));
  write<s16>(a + 0x568, read<s16>(a + 0x12C));
  write<s16>(a + 0x56A, read<s16>(a + 0x12E));
  write<u8>(a + 0x565, read<u8>(a + 0x129));
  write<u8>(a + 0x566, read<u8>(a + 0x12A));
  write<s16>(a + 0x56C, read<s16>(a + 0x130));
  write<f32>(a + 0x54C, read<f32>(a + 0x110));
  write<u8>(a + 0x567, read<u8>(a + 0x12B));
  write<f32>(a + 0x550, read<f32>(a + 0x114));
  write<f32>(a + 0x554, read<f32>(a + 0x118));
  write<f32>(a + 0x558, read<f32>(a + 0x11C));
  write<s16>(a + 0x56E, read<s16>(a + 0x132));
  write<u16>(a + 0x5DC, read<u16>(a + 0x1A0));
  write<f32>(a + 0x55C, read<f32>(a + 0x120));
  write<f32>(a + 0x560, read<f32>(a + 0x124));
  write<f32>(a + 0x570, read<f32>(a + 0x134));
  write<f32>(a + 0x574, read<f32>(a + 0x138));
  write<f32>(a + 0x578, read<f32>(a + 0x13C));
  write<f32>(a + 0x57C, read<f32>(a + 0x140));
  write<u16>(a + 0x5DE, read<u16>(a + 0x1A2));
  write<u16>(a + 0x5E0, read<u16>(a + 0x1A4));
  write<u32>(a + 0x5D0, read<u32>(a + 0x194));
  write<u32>(a + 0x5D4, read<u32>(a + 0x198));
  write<u32>(a + 0x5D8, read<u32>(a + 0x19C));
  write<u16>(a + 0x5E2, read<u16>(a + 0x1A6));
  write<f32>(a + 0x580, read<f32>(a + 0x144));
  write<f32>(a + 0x584, read<f32>(a + 0x148));
  write<f32>(a + 0x588, read<f32>(a + 0x14C));
  write<f32>(a + 0x58C, read<f32>(a + 0x150));
  write<f32>(a + 0x5F4, read<f32>(a + 0x1B8));
  write<f32>(a + 0x5F8, read<f32>(a + 0x1BC));
  write<u16>(a + 0x5EC, read<u16>(a + 0x1B0));
  write<f32>(a + 0x5FC, read<f32>(a + 0x1C0));
  write<f32>(a + 0x60C, read<f32>(a + 0x1D0));
  write<f32>(a + 0x610, read<f32>(a + 0x1D4));
  write<f32>(a + 0x614, read<f32>(a + 0x1D8));
  write<u8>(a + 0x600, read<u8>(a + 0x1C4));
  write<u8>(a + 0x601, read<u8>(a + 0x1C5));
  write<u8>(a + 0x602, read<u8>(a + 0x1C6));
  write<u8>(a + 0x603, read<u8>(a + 0x1C7));
  write<u8>(a + 0x604, read<u8>(a + 0x1C8));
  write<u8>(a + 0x605, read<u8>(a + 0x1C9));
  write<u8>(a + 0x606, read<u8>(a + 0x1CA));
  write<u16>(a + 0x5EE, read<u16>(a + 0x1B2));
  write<u16>(a + 0x5F0, read<u16>(a + 0x1B4));
  write<u8>(a + 0x607, read<u8>(a + 0x1CB));
  write<u8>(a + 0x608, read<u8>(a + 0x1CC));
  write<u8>(a + 0x624, read<u8>(a + 0x1E8));
  write<u8>(a + 0x625, read<u8>(a + 0x1E9));
  write<u8>(a + 0x626, read<u8>(a + 0x1EA));
  write<u8>(a + 0x627, read<u8>(a + 0x1EB));
  write<f32>(a + 0x618, read<f32>(a + 0x1DC));
  write<f32>(a + 0x61C, read<f32>(a + 0x1E0));
  write<f32>(a + 0x620, read<f32>(a + 0x1E4));
  write<f32>(a + 0x630, read<f32>(a + 0x1F4));
  write<f32>(a + 0x690, read<f32>(a + 0x254));
  write<f32>(a + 0x634, read<f32>(a + 0x1F8));
  write<f32>(a + 0x638, read<f32>(a + 0x1FC));
  write<f32>(a + 0x63C, read<f32>(a + 0x200));
  write<f32>(a + 0x640, read<f32>(a + 0x204));
  write<f32>(a + 0x644, read<f32>(a + 0x208));
  write<s16>(a + 0x628, read<s16>(a + 0x1EC));
  write<s16>(a + 0x62A, read<s16>(a + 0x1EE));
  write<f32>(a + 0x648, read<f32>(a + 0x20C));
  write<f32>(a + 0x64C, read<f32>(a + 0x210));
  write<s16>(a + 0x62C, read<s16>(a + 0x1F0));
  write<s16>(a + 0x62E, read<s16>(a + 0x1F2));
  write<u16>(a + 0x5F2, read<u16>(a + 0x1B6));
  write<s16>(a + 0x6AC, read<s16>(a + 0x270));
  write<s16>(a + 0x6AE, read<s16>(a + 0x272));
  write<s16>(a + 0x6B0, read<s16>(a + 0x274));
  write<s16>(a + 0x6B2, read<s16>(a + 0x276));
  write<u8>(a + 0x6A8, read<u8>(a + 0x26C));
  write<u8>(a + 0x6A9, read<u8>(a + 0x26D));
  write<u8>(a + 0x6AA, read<u8>(a + 0x26E));
  write<u8>(a + 0x6AB, read<u8>(a + 0x26F));
  write<f32>(a + 0x694, read<f32>(a + 0x258));
  write<f32>(a + 0x698, read<f32>(a + 0x25C));
  write<f32>(a + 0x69C, read<f32>(a + 0x260));
  write<f32>(a + 0x6A0, read<f32>(a + 0x264));
  write<f32>(a + 0x6A4, read<f32>(a + 0x268));
  write<f32>(a + 0x6B4, read<f32>(a + 0x278));
  write<f32>(a + 0x6B8, read<f32>(a + 0x27C));
  write<f32>(a + 0x6BC, read<f32>(a + 0x280));
  write<f32>(a + 0x6C0, read<f32>(a + 0x284));
  write<f32>(a + 0x6C4, read<f32>(a + 0x288));
  write<f32>(a + 0x6C8, read<f32>(a + 0x28C));
  write<f32>(a + 0x6CC, read<f32>(a + 0x290));
  write<f32>(a + 0x6D0, read<f32>(a + 0x294));
  write<u32>(a + 0x5E4, read<u32>(a + 0x1A8));
  write<u32>(a + 0x5E8, read<u32>(a + 0x1AC));
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 0, ptr(a + 0x314), ptr(a + 0x54C));
  brighten(a);
  u8 alpha =
      (u8)gabi::ftoi(gabi::fmadds(102.0f, read<f32>(0x1047BC14), 153.0f));
  write<u32>(a + 0x3B8, particle(a, 2, 0x8072, alpha));
  alpha = (u8)gabi::ftoi(gabi::fmadds(102.0f, read<f32>(0x1047BC18), 153.0f));
  u32 second = particle(a, 2, 0x8073, alpha);
  u32 first = read<u32>(a + 0x3B8);
  write<u32>(a + 0x3BC, second);
  if (first) {
    for (int i = 0; i < 3; i++)
      write<u8>(first + 0x244 + i, read<u8>(0x101CAA6C + i));
    first = read<u32>(a + 0x3B8);
    for (int i = 0; i < 3; i++)
      write<u8>(first + 0x248 + i, read<u8>(0x101CAA6C + i));
    second = read<u32>(a + 0x3BC);
  }
  if (second) {
    for (int i = 0; i < 3; i++)
      write<u8>(second + 0x244 + i, read<u8>(0x101CAA6C + i));
    second = read<u32>(a + 0x3BC);
    for (int i = 0; i < 3; i++)
      write<u8>(second + 0x248 + i, read<u8>(0x101CAA6C + i));
  }
  return 1;
}
VERIFY(0x0236D520, createInit);
static s32 createActor(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236E964, s32, self);
  u32 a = gabi::ea(self);
  u32 status = read<u32>(a + 0x2E4);
  if (!(status & 8)) {
    if (self) {
      construct(self);
      status = read<u32>(a + 0x2E4);
    }
    write<u32>(a + 0x2E4, status | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, ptr(a + 0x400), ptr(0x1002C328));
  if (phase == 4) {
    u32 g = gabi::call<u32>(0x025200D4);
    if (!read<u32>(g + 0x5AB4))
      return 0;
    if (!gabi::call<s32>(0x025D63E8, self, ptr(0x0236D20C), 0x5D40))
      return 5;
    createInit(self);
  }
  return phase;
}
VERIFY(0x0236E964, createActor);
static BOOL drawActor(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236EECC, BOOL, self);
  u32 a = gabi::ea(self);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 1, ptr(a + 0x314), ptr(a + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 0, ptr(a + 0x314), ptr(a + 0x54C));
  brighten(a);
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), ptr(read<u32>(a + 0x408)), ptr(a + 0x110));
  s16 frame = (s16)gabi::ftoi(read<f32>(a + 0x740));
  u32 model = read<u32>(a + 0x408);
  gabi::call(0x025E83FC, ptr(a + 0x410), ptr(read<u32>(model + 0xAC)),
             (f32)frame);
  frame = (s16)gabi::ftoi(read<f32>(a + 0x73C));
  model = read<u32>(a + 0x408);
  gabi::call(0x025E86B8, ptr(a + 0x48C), ptr(read<u32>(model + 0xAC)),
             (f32)frame);
  gabi::call(0x025E2DE0, ptr(read<u32>(a + 0x408)), 0);
  return 1;
}
VERIFY(0x0236EECC, drawActor);
static BOOL executeActor(daObjMagmarock_c *self) {
  WWHD_FUNC(0x0236EAB0, BOOL, self);
  u32 a = gabi::ea(self);
  calcGroundQuat(self);
  f32 y, home, limit;
  if (!read<s16>(a + 0x764)) {
    if (!isProcess(a, 0x0236E85C) && !isProcess(a, 0x0236E8E8)) {
      gabi::call(0x0200ED84, ptr(a + 0x738), 0.0f, 0.2f, 20.0f);
      gabi::call(0x0200F428, ptr(a + 0x75E), 0, 4, 0x100);
    }
    y = read<f32>(a + 0x318);
    f32 velocity = read<f32>(a + 0x340);
    home = read<f32>(a + 0x2F0);
    limit = home + 100.0f;
    y = y + velocity;
    velocity = velocity + read<f32>(a + 0x374);
    write<f32>(a + 0x318, y);
    write<f32>(a + 0x340, velocity);
  } else {
    home = read<f32>(a + 0x2F0);
    limit = home + 100.0f;
    y = read<f32>(a + 0x318);
    write<f32>(a + 0x340, 0);
  }
  // Buoyancy and damping once the slab is below the lava surface.
  if (y < limit) {
    if (!(read<f32>(a + 0x304) < limit)) {
      shock();
      y = read<f32>(a + 0x318);
      home = read<f32>(a + 0x2F0);
    }
    if (!(y < home)) {
      write<f32>(a + 0x340,
                 read<f32>(a + 0x340) * (0.65f - read<f32>(0x1047BC14)));
    } else {
      f32 bottom = home - 30.0f;
      if (!(y >= bottom)) {
        y = bottom;
        home = read<f32>(a + 0x2F0);
        write<f32>(a + 0x318, y);
      }
      f32 spring = read<f32>(0x1047BC18) + 0.4f;
      f32 delta = y - home;
      f32 velocity = gabi::fnmsubs(spring, delta, read<f32>(a + 0x340));
      write<f32>(a + 0x340, velocity);
      write<f32>(a + 0x340, velocity * (0.65f - read<f32>(0x1047BC14)));
    }
  }
  if (!read<s16>(a + 0x764) && !isProcess(a, 0x0236E810)) {
    write<u32>(a + 0x750, read<u32>(a + 0x750) - 1);
    write<u32>(a + 0x754, read<u32>(a + 0x754) + 1);
  }
  setMtx(self);
  demoMove(self);
  controlEffect(self);
  s16 adjust = read<s16>(a + 0x3F8), slot = read<s16>(a + 0x3FA);
  u32 receiver = a + (s32)adjust;
  write<s16>(a + 0x766, 0);
  write<s16>(a + 0x764, 0);
  u32 action;
  if (slot < 0)
    action = read<u32>(a + 0x3FC);
  else {
    u32 vt = read<u32>(receiver + (s32)read<s16>(a + 0x3FE));
    action = read<u32>(vt + (s32)slot * 8 + 4);
  }
  gabi::call(action, ptr(receiver));
  playAnim(self);
  u32 table = 0x104A44F8 + (read<u16>(a + 0x75C) >> 3) * 8;
  f32 amplitude = read<f32>(a + 0x738);
  write<s16>(a + 0x328, (s16)gabi::ftoi(amplitude * read<f32>(table + 4)));
  write<s16>(a + 0x32C, (s16)gabi::ftoi(amplitude * read<f32>(table)));
  if (!read<s16>(a + 0x3B4)) {
    for (int i = 0; i < 4; i++)
      write<u32>(a + 0x3D8 + i * 4, read<u32>(0x101E9C38 + i * 4));
  }
  gabi::Local<be<u32>[4]> quat;
  gabi::call(0x028E9BC0, ptr(a + 0x3C8), ptr(a + 0x3D8), quat.get(), 0.25f);
  u32 bg = read<u32>(a + 0x548);
  for (int i = 0; i < 4; i++)
    write<u32>(a + 0x3C8 + i * 4, (u32)(*quat)[i]);
  write<s16>(a + 0x3B4, 0);
  if (read<u32>(bg) < 256) {
    write<u32>(bg + 0x78, read<u32>(bg + 0x78) | 4);
    gabi::call(0x024F43DC, ptr(read<u32>(a + 0x548)));
  }
  return 0;
}
VERIFY(0x0236EAB0, executeActor);

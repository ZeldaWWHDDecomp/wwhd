/* HD collision mass manager, 025168E8..02517588. GC layout and behavior;
 * HD Chk guards optional hit-info before clearing and null object/GObjInf
 * pointers before getter/attack work. */
#include "gabi.h"
#include <cmath>
using namespace gabi;
namespace mass {
static u32 ld(u32 p) { return load<u32>(p); }
static void st(u32 p, u32 v) { store<u32>(p, v); }
static u8 byte(u32 p) { return load<u8>(p); }
static void sb(u32 p, u32 v) { store<u8>(p, (u8)v); }
static f32 fl(u32 p) { return load<f32>(p); }
static void sf(u32 p, f32 v) { store<f32>(p, v); }
static void *ptr(u32 p) { return at<void>(p); }
// Address-passed stack objects keep the complete HD callee layout.
struct Aab {
  u8 bytes[0x1c];
};
struct Divide {
  u8 bytes[8];
};
static_assert(sizeof(Aab) == 0x1c && sizeof(Divide) == 8);
struct Vec {
  be<f32> x, y, z;
};
static u32 shape(u32 obj) {
  return call_ptr<u32>(ld(ld(obj + 0x3c) + 0x2c), ptr(obj));
}
static void assertion(u32 line, u32 text) {
  call<void>(0x0273AA24, ptr(0x1004B350), line, ptr(text));
}
} // namespace mass
using namespace mass;
void *mass_obj_ctor(void *p) {
  WWHD_FUNC(0x025168E8, void *, p);
  u32 a = ea(p);
  if (!a)
    a = call<u32>(0x0273AD10, 0x18);
  if (a) {
    st(a + 8, 0);
    sb(a + 4, 0);
    st(a + 0x14, 0x1004B30C);
    st(a, 0);
    call<void>(0x0200B6C0, ptr(a + 0xc));
  }
  return ptr(a);
}
VERIFY(0x025168E8, mass_obj_ctor);
void *mass_hit_ctor(void *p) {
  WWHD_FUNC(0x0251694C, void *, p);
  u32 a = ea(p);
  if (!a)
    a = call<u32>(0x0273AD10, 0x14);
  if (a) {
    st(a + 0x10, 0x1004B31C);
    st(a, 0);
    st(a + 4, 0);
    sf(a + 0xc, fl(0x1004B33C));
    st(a + 8, 0);
  }
  return ptr(a);
}
VERIFY(0x0251694C, mass_hit_ctor);
void mass_clear(void *p) {
  WWHD_FUNC(0x025169A8, void, p);
  u32 a = ea(p);
  st(a + 0x40, 0);
  st(a + 0xbc, 0);
  for (u32 i = 0; i < 5; i++) {
    u32 o = a + 0x44 + 24 * i;
    st(o, 0);
    sb(o + 4, 5);
    st(o + 8, 0);
    st(o + 0xc, 0);
  }
  for (u32 i = 0; i < 2; i++) {
    u32 o = a + 0xc0 + 24 * i;
    st(o, 0);
    sb(o + 4, 5);
    st(o + 8, 0);
    st(o + 0xc, 0);
  }
  f32 z = fl(0x1004B33C);
  call<void>(0x020184DC, ptr(a + 0x110), z);
  call<void>(0x02018428, ptr(a + 0x110), z);
  sb(a + 0x128, 0);
  sb(a + 0x129, 4);
}
VERIFY(0x025169A8, mass_clear);
void mass_ct(void *p) {
  WWHD_FUNC(0x02516A78, void, p);
  u32 a = ea(p);
  f32 z = fl(0x1004B33C), n = fl(0x1004B340);
  st(a + 0x12c, 0);
  sf(a + 0x144, z);
  sf(a + 0x148, n);
  st(a + 0x130, 0);
  sf(a + 0x14c, z);
  sf(a + 0x13c, z);
  sf(a + 0x134, z);
  sf(a + 0x138, n);
  call<void>(0x025169A8, p);
}
VERIFY(0x02516A78, mass_ct);
void *mass_ctor(void *p) {
  WWHD_FUNC(0x02516AB0, void *, p);
  u32 a = ea(p);
  if (!a)
    a = call<u32>(0x0273AD10, 0x1a0);
  if (a) {
    st(a + 0x19c, 0x1004B32C);
    call<void>(0x0200B750, ptr(a));
    st(a + 0x40, 0);
    call<void>(0x028EFFD0, ptr(a + 0x44), 5, 0x18, ptr(0x025168E8));
    st(a + 0xbc, 0);
    call<void>(0x028EFFD0, ptr(a + 0xc0), 2, 0x18, ptr(0x025168E8));
    st(a + 0x108, 0x1004B2FC);
    st(a + 0x10c, 0x100015A8);
    call<void>(0x02018590, ptr(a + 0x110));
    sb(a + 0x129, 0);
    st(a + 0x130, 0);
    st(a + 0x124, 0x100017D8);
    sb(a + 0x128, 0);
    st(a + 0x10c, 0x10001738);
    st(a + 0x12c, 0);
    call<void>(0x028F521C, ptr(a + 0x134), 12);
    f32 z = fl(0x1004B33C);
    sf(a + 0x140, z);
    call<void>(0x028F521C, ptr(a + 0x144), 12);
    st(a + 0x170, 0x100015A8);
    st(a + 0x16c, 0x1004B2FC);
    sf(a + 0x150, z);
    call<void>(0x02018150, ptr(a + 0x174));
    st(a + 0x170, 0x10001678);
    st(a + 0x18c, 0x10001718);
    call<void>(0x0200B6C0, ptr(a + 0x194));
    call<void>(0x02516A78, ptr(a));
  }
  return ptr(a);
}
VERIFY(0x02516AB0, mass_ctor);
void mass_set(void *p, void *obj, u32 priority) {
  WWHD_FUNC(0x02516C14, void, p, obj, priority);
  u32 a = ea(p), o;
  s32 count = (s32)ld(a + 0x40);
  if (count >= 5) {
    for (u32 i = 0; i < 5; i++) {
      o = a + 0x44 + 24 * i;
      s32 old = byte(o + 4);
      if (old > (s32)priority ||
          (old == (s32)priority &&
           call<f32>(0x020198D8, fl(0x1004B348)) < fl(0x1004B344))) {
        sb(o + 4, priority);
        st(o, ea(obj));
        st(o + 8, 0);
        return;
      }
    }
  } else {
    o = a + 0x44 + (u32)count * 24;
    st(o, ea(obj));
    sb(o + 4, priority);
    st(o + 8, 0);
    st(a + 0x40, ld(a + 0x40) + 1);
  }
}
VERIFY(0x02516C14, mass_set);
void mass_area_set(void *p, void *obj, u32 priority, void *cb) {
  WWHD_FUNC(0x02516D48, void, p, obj, priority, cb);
  u32 a = ea(p), o;
  s32 count = (s32)ld(a + 0xbc);
  if (count >= 2) {
    for (u32 i = 0; i < 2; i++) {
      o = a + 0xc0 + 24 * i;
      if (byte(o + 4) > priority) {
        sb(o + 4, priority);
        st(o + 8, ea(cb));
        st(o, ea(obj));
        return;
      }
    }
  } else {
    o = a + 0xc0 + (u32)count * 24;
    st(o, ea(obj));
    sb(o + 4, priority);
    st(o + 8, ea(cb));
    st(a + 0xbc, ld(a + 0xbc) + 1);
  }
}
VERIFY(0x02516D48, mass_area_set);
void mass_cam_set(void *p, void *cps) {
  WWHD_FUNC(0x02516DB4, void, p, cps);
  u32 a = ea(p);
  call<void>(0x02018240, ptr(a + 0x174), cps);
  st(a + 0x130, 0);
  st(a + 0x12c, ld(a + 0x12c) | 1);
}
VERIFY(0x02516DB4, mass_cam_set);
u32 mass_cam_result(void *p) {
  WWHD_FUNC(0x02516DF8, u32, p);
  return ld(ea(p) + 0x130);
}
VERIFY(0x02516DF8, mass_cam_result);
void mass_cam_top(void *p, void *out) {
  WWHD_FUNC(0x02516E00, void, p, out);
  u32 a = ea(p), b = ea(out);
  st(b, ld(a + 0x134));
  st(b + 4, ld(a + 0x138));
  st(b + 8, ld(a + 0x13c));
}
VERIFY(0x02516E00, mass_cam_top);
void mass_prepare(void *p) {
  WWHD_FUNC(0x02516E1C, void, p);
  u32 a = ea(p);
  // Dispatch preserves HD argument-register setup as well as the receiver.
  // Targets may consume live-in r4; no nolivein exemption is used.
  Local<Aab> area;
  st(ea(area.get()) + 0x18, 0x1004B2FC);
  call<void>(0x02017DAC, area.get());
  for (u32 group = 0; group < 2; group++) {
    u32 base = a + (group ? 0xc0 : 0x44), count = a + (group ? 0xbc : 0x40);
    if (!group)
      cpu->r[4] = base + ld(count) * 24;
    for (u32 o = base; o < base + ld(count) * 24; o += 24) {
      u32 obj = ld(o);
      if (!obj)
        assertion(group ? 0x69 : 0x5d, group ? 0x1004B36C : 0x1004B360);
      u32 v = ld(obj + 0x3c), target = ld(v + 0x2c);
      if (!group) {
        cpu->r[5] = v;
        cpu->r[6] = target;
      } else {
        cpu->r[10] = v;
      }
      u32 s = call_ptr<u32>(target, ptr(obj));
      u32 sv = ld(s + 0x1c), calc = ld(sv + 0x94);
      if (!group) {
        cpu->r[7] = sv;
        cpu->r[8] = calc;
      } else
        cpu->r[4] = sv;
      call_ptr<void>(calc, ptr(s));
      call<void>(0x02017E98, area.get(), ptr(s));
    }
  }
  if (ld(a + 0x12c) & 1) {
    call<void>(0x0200C2D8, ptr(a + 0x154));
    call<void>(0x02017E98, area.get(), ptr(a + 0x154));
  }
  call<void>(0x0200B7D4, p, area.get());
  for (u32 group = 0; group < 2; group++) {
    u32 base = a + (group ? 0xc0 : 0x44), count = a + (group ? 0xbc : 0x40);
    for (u32 o = base; o < base + ld(count) * 24; o += 24) {
      u32 obj = ld(o);
      if (!obj)
        assertion(group ? 0x88 : 0x7d, group ? 0x1004B36C : 0x1004B360);
      u32 v = ld(obj + 0x3c), target = ld(v + 0x2c);
      if (group) {
        cpu->r[4] = v;
        cpu->r[5] = target;
      } else
        cpu->r[6] = v;
      u32 s = call_ptr<u32>(target, ptr(obj));
      call<void>(0x0200B8CC, p, ptr(o + 0xc), ptr(s), 0);
    }
  }
  if (ld(a + 0x12c) & 1)
    call<void>(0x0200B8CC, p, ptr(a + 0x194), ptr(a + 0x154), 0);
  f32 z = fl(0x1004B33C), n = fl(0x1004B340), d = fl(0x1004B34C);
  sf(a + 0x148, n);
  sf(a + 0x13c, z);
  sf(a + 0x144, z);
  sf(a + 0x134, z);
  sf(a + 0x140, d);
  sf(a + 0x138, n);
  sf(a + 0x150, d);
  sf(a + 0x14c, z);
}
VERIFY(0x02516E1C, mass_prepare);
u32 mass_chk(void *p, void *pos, void *actor, void *hit) {
  WWHD_FUNC(0x025170D8, u32, p, pos, actor, hit);
  u32 a = ea(p), h = ea(hit), result = 0;
  Local<Divide> divide;
  Local<Vec> move;
  Local<f32> len, areaLen, camLen;
  Local<Vec> top, atHit;
  call<void>(0x0200B6C0, divide.get());
  st(ea(actor), 0);
  call<void>(0x020182E0, ptr(a + 0x110), pos);
  call<void>(0x0200C864, ptr(a + 0xf0));
  call<void>(0x0200BAC0, p, divide.get(), ptr(a + 0xf0));
  f32 z = fl(0x1004B33C);
  if (h) {
    st(h + 4, 0);
    st(h + 8, 0);
    st(h, 0);
    sf(h + 0xc, z);
  }
  if (byte(a + 0x128) & 8) {
    u32 base = a + 0xc0;
    for (u32 o = base; o < base + ld(a + 0xbc) * 24; o += 24) {
      if (!call<s32>(0x0200B71C, ptr(o + 0xc), divide.get()))
        continue;
      u32 obj = ld(o);
      if (!obj)
        call<void>(0x0273AA24, ptr(0x1004B380), 0xd0, ptr(0x1004B390));
      u32 s = shape(obj);
      if (!(ld(obj + 0x2c) & 1) ||
          !call<s32>(0x0200C7EC, ptr(a + 0xf0), ptr(s), areaLen.get()))
        continue;
      result |= 4;
      st(ea(actor), ld(ld(obj + 0x44) + 0xc));
      if (h)
        st(h, obj);
      u32 cb = ld(o + 8);
      if (cb)
        call_ptr<void>(cb, ptr(ld(ld(obj + 0x44) + 0xc)), pos,
                       (u32)byte(a + 0x129));
    }
  }
  {
    u32 base = a + 0x44;
    for (u32 o = base; o < base + ld(a + 0x40) * 24; o += 24) {
      if (!call<s32>(0x0200B71C, ptr(o + 0xc), divide.get()))
        continue;
      u32 obj = ld(o);
      if (!obj) {
        call<void>(0x0273AA24, ptr(0x1004B380), 0xf6, ptr(0x1004B390));
        continue;
      }
      u32 g = call_ptr<u32>(ld(ld(obj + 0x3c) + 0x1c), ptr(obj)),
          s = call_ptr<u32>(ld(ld(obj + 0x3c) + 0x2c), ptr(obj),
                            ptr(ld(obj + 0x3c)),
                            ptr(ld(ld(obj + 0x3c) + 0x2c)));
      if (g && (ld(obj) & 1) && !(ld(g + 0x50) & 8) &&
          call<s32>(0x0200C79C, ptr(a + 0xf0), ptr(s), atHit.get()) &&
          (byte(a + 0x128) & 1)) {
        st(ea(actor), ld(ld(obj + 0x44) + 0xc));
        result |= 1;
        if (h)
          st(h + 4, obj);
      }
      if (!(ld(obj + 0x2c) & 1) ||
          !call<s32>(0x0200C7EC, ptr(a + 0xf0), ptr(s), len.get()) ||
          !(byte(a + 0x128) & 2))
        continue;
      u32 act = ld(ld(obj + 0x44) + 0xc);
      st(ea(actor), act);
      result |= 2;
      if (byte(a + 0x128) & 0x10) {
        call<void>(0x028E8DAC, ptr(act + 0x314), pos, move.get());
        move->y = z;
        f32 mag = call<f32>(0x028E8E10, move.get());
        if (std::fabs(mag) < fl(0x100030B8)) {
          move->x = fl(0x1004B348);
          call<void>(0x0200BE28, ptr(ld(obj + 0x44)), (f32)move->x,
                     (f32)move->y, (f32)move->z);
        } else {
          f32 scale = (f32)((f64)fl(ea(len.get())) / (f64)mag);
          call<void>(0x028E8E64, move.get(), move.get(), scale);
          call<void>(0x0200BE28, ptr(ld(obj + 0x44)), (f32)move->x,
                     (f32)move->y, (f32)move->z);
        }
      }
      if (h) {
        st(h + 8, obj);
        sf(h + 0xc, fl(ea(len.get())));
      }
    }
  }
  if ((ld(a + 0x12c) & 1) &&
      call<s32>(0x0200B71C, ptr(a + 0x194), divide.get()) &&
      call<s32>(0x0200C808, ptr(a + 0xf0), ptr(a + 0x154), camLen.get())) {
    u32 shift = (u32)byte(a + 0x129) + 1;
    u32 bit = (shift & 0x20) ? 0 : 1u << (shift & 31);
    u32 cam = ld(a + 0x130) | 1 | bit;
    st(a + 0x130, cam);
    if (cam & 0xa) {
      f32 y = fadds_ppc(fl(ea(pos) + 4), fl(a + 0x120));
      f32 raised = fadds_ppc(y, fl(0x1004B37C));
      top->x = fl(ea(pos));
      top->y = y;
      top->z = fl(ea(pos) + 8);
      if (fl(a + 0x138) < raised) {
        f32 dist = call<f32>(0x028E8DE8, top.get(), ptr(a + 0x174));
        if (fl(a + 0x140) > dist) {
          st(a + 0x138, ld(ea(top.get()) + 4));
          st(a + 0x13c, ld(ea(top.get()) + 8));
          st(a + 0x134, ld(ea(top.get())));
          sf(a + 0x140, dist);
        }
      }
      if (fl(a + 0x148) < raised) {
        f32 dist = call<f32>(0x028E8DE8, top.get(), ptr(a + 0x180));
        if (fl(a + 0x150) > dist) {
          st(a + 0x148, ld(ea(top.get()) + 4));
          st(a + 0x14c, ld(ea(top.get()) + 8));
          st(a + 0x144, ld(ea(top.get())));
          sf(a + 0x150, dist);
        }
      }
    }
  }
  call<void>(0x0200B708, divide.get(), 2);
  return result;
}
VERIFY(0x025170D8, mass_chk);
void mass_sinit() {
  WWHD_FUNC(0x02517588, void);
  st(0x1046EFF8, 0);
  st(0x1046EFF0, 0);
  st(0x1046EFFC, 0);
  st(0x1046EFF4, 0);
  call<void>(0x028F026C, ptr(0x101D573C));
  sf(0x1046EFE4, fl(0x1004B3A0));
  sf(0x1046EFE8, fl(0x1004B3A4));
  call<void>(0x028ED6F8, ptr(0x1046EFEC));
  call<void>(0x028F026C, ptr(0x101D5748));
  call<void>(0x028EAB2C, ptr(0x1046EFED));
  call<void>(0x028F026C, ptr(0x101D5754));
}
VERIFY(0x02517588, mass_sinit);

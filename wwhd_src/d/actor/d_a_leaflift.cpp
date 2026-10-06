#include "d/actor/d_a_leaflift.h"
#include "bindings.h"
using Actor = daLlift_c;
template <unsigned N> struct Opaque {
  be<u8> bytes[N];
};
// Keep the current guest frame linkage area clear for actual nonleaf callees.
template <class T> struct ProtectedLocal {
  struct Frame {
    u8 linkage[16];
    T value;
  };
  gabi::Local<Frame> storage;
  T *get() const { return gabi::at<T>(storage.a + offsetof(Frame, value)); }
  T *operator->() const { return get(); }
  T &operator*() const { return *get(); }
  operator T *() const { return get(); }
};
static constexpr u32 MTX = 0x1048D0CC, SINCOS = 0x104A44F8;
template <class T> static T read(u32 p, u32 off) {
  return gabi::load<T>(p + off);
}
template <class T> static void write(u32 p, u32 off, T v) {
  gabi::store<T>(p + off, v);
}
static void *ptr(u32 a) { return gabi::at<void>(a); }
static u32 address(Actor *a, u32 off) { return gabi::ea(a) + off; }
static f32 sine(s16 a) { return gabi::load<f32>(SINCOS + (u16(a) >> 3) * 8); }
static f32 cosine(s16 a) {
  return gabi::load<f32>(SINCOS + (u16(a) >> 3) * 8 + 4);
}
static void resetRotation(Actor *a) {
  u32 x = gabi::load<u32>(0x101E9C38), y = gabi::load<u32>(0x101E9C3C);
  u32 z = gabi::load<u32>(0x101E9C40), w = gabi::load<u32>(0x101E9C44);
  write<u32>(gabi::ea(a), 0x574, x);
  write<u32>(gabi::ea(a), 0x578, y);
  write<u32>(gabi::ea(a), 0x57C, z);
  write<u32>(gabi::ea(a), 0x580, w);
}
static u32 animationMatrix(u32 model, u32 joint) {
  u32 buffer = read<u32>(model, 0x2C), matrices = read<u32>(buffer, 0x10);
  u16 flags = read<u16>(buffer, 4);
  write<u16>(buffer, 4, flags | 0x10);
  return matrices + joint * 0x30;
}
static void invalidate(u32 emitter) {
  u32 flags = read<u32>(emitter, 0x254);
  write<s32>(emitter, 0x5C, -1);
  write<u32>(emitter, 0x254, flags | 1);
}
static void stop(u32 emitter) {
  write<u32>(emitter, 0x254, read<u32>(emitter, 0x254) | 1);
}
static void play(u32 emitter) {
  write<u32>(emitter, 0x254, read<u32>(emitter, 0x254) & ~1u);
}
static u32 particle(u16 id, void *pos, void *angle) {
  u32 playObject = gabi::call<u32>(0x025200D4),
      control = read<u32>(playObject, 0x5AB0);
  return gabi::call<u32>(0x025A847C, ptr(control), u32(0), id, pos, angle,
                         u32(0), u8(255), u32(0), s8(-1), u32(0), u32(0),
                         u32(0));
}
static void colorEmitter(Actor *a, u32 off, void *color) {
  u32 emitter = read<u32>(gabi::ea(a), off);
  if (!emitter)
    return;
  u8 r = read<u8>(gabi::ea(color), 0), g = read<u8>(gabi::ea(color), 1),
     b = read<u8>(gabi::ea(color), 2);
  write<u8>(emitter, 0x244, r);
  write<u8>(emitter, 0x245, g);
  write<u8>(emitter, 0x246, b);
  emitter = read<u32>(gabi::ea(a), off);
  write<u8>(emitter, 0x248, r);
  write<u8>(emitter, 0x249, g);
  write<u8>(emitter, 0x24A, b);
}
static void rideCallback(void *bg, Actor *a, fopAc_ac_c *other) {
  WWHD_FUNC(0x021B3028, void, bg, a, other);
  ProtectedLocal<cXyz> offset, cross;
  gabi::call(0x0201ADE0, gabi::at<cXyz>(gabi::ea(other) + 0x314), offset.get(),
             &a->current.pos);
  f32 factor = 2.f;
  if (other && gabi::load<s16>(gabi::ea(other) + 8) == 0xA8) {
    a->ridden = 1;
    a->hasRider = 1;
    gabi::call(0x0201B080, offset.get(), cross.get(), ptr(0x10464E0C));
    offset->copy(*cross);
    gabi::call(0x025F1884, ptr(MTX), s16(-s16(a->current.angle.y)));
    gabi::call(0x028E8F64, ptr(MTX), offset.get(), offset.get());
    if (a->risingTilt) {
      offset->x = f32(other->current.pos.x) + 200.f;
      offset->y = other->current.pos.y;
      offset->z = other->current.pos.z;
      gabi::call(0x0201B080, offset.get(), cross.get(), ptr(0x10464E0C));
      offset->copy(*cross);
      gabi::call(0x025F1884, ptr(MTX), s16(-s16(a->current.angle.y)));
      gabi::call(0x028E8F64, ptr(MTX), offset.get(), offset.get());
      u16 angle = u16(u16(a->lifetime) << 11);
      factor = (sine(s16(angle)) + sine(s16(angle))) * 0.25f;
      f32 random = gabi::call<f32>(0x02019918, 0.2f);
      factor *= random + 1.f;
    }
    f32 sq = gabi::call<f32>(0x028E8DD0, offset.get());
    f32 length = gabi::call<f32>(0x028F4384, sq);
    if (!gabi::call<s32>(0x0201B47C, offset.get()))
      return;
    s16 target = s16(gabi::ftoi(-(length * factor)));
    gabi::call(0x0200F428, &a->tilt, target, s16(8), s16(0x200));
    f32 tilt = sine(a->tilt);
    a->targetRotation[0] = f32(offset->x) * tilt;
    a->targetRotation[1] = f32(offset->y) * tilt;
    a->targetRotation[2] = f32(offset->z) * tilt;
    a->targetRotation[3] = cosine(a->tilt);
  }
  a->risingTilt = 0;
}
VERIFY(0x021B3028, rideCallback);
static s32 createHeap(Actor *a) {
  WWHD_FUNC(0x021B3260, s32, a);
  ProtectedLocal<SafeString> name;
  name->mStringTop = 0x10013F20;
  name->__vtbl = 0x10013E78;
  u32 resources = gabi::load<u32>(0x101F4F28);
  u32 data = gabi::call<u32>(0x026066C4, ptr(resources), name.get(), s32(4));
  if (!data)
    gabi::call(0x0273AA24, STR(0x10013EC0), s32(0x14E), STR(0x10013ED4));
  u32 model =
      gabi::call<u32>(0x025E38E0, ptr(data), u32(0x80000), u32(0x11000022));
  a->model = ptr(model);
  if (!model)
    return 0;
  write<u32>(model, 0xB8, gabi::ea(a));
  u32 bg = gabi::call<u32>(0x024F23F4, u32(0));
  a->background = ptr(bg);
  if (!bg)
    return 0;
  ProtectedLocal<SafeString> bgname;
  bgname->__vtbl = 0x10013E78;
  bgname->mStringTop = 0x10013F20;
  resources = gabi::load<u32>(0x101F4F28);
  data = gabi::call<u32>(0x026066C4, ptr(resources), bgname.get(), s32(7));
  bg = gabi::ea(a->background.get());
  if (gabi::call<s32>(0x0200A030, ptr(bg), ptr(data), u32(1), &a->transform) !=
      0)
    return 0;
  bg = gabi::ea(a->background.get());
  write<u32>(bg, 0xA8, 0x024EE708);
  bg = gabi::ea(a->background.get());
  write<u32>(bg, 0xB0, 0x021B3028);
  return 1;
}
VERIFY(0x021B3260, createHeap);
static s32 heapCallback(Actor *a) {
  WWHD_FUNC(0x021B337C, s32, a);
  return createHeap(a);
}
VERIFY(0x021B337C, heapCallback);
static s32 nodeCallback(void *node, s32 timing) {
  WWHD_FUNC(0x021B3380, s32, node, timing);
  if (timing == 0) {
    u32 joint = gabi::call<u32>(0x027F7878, node),
        model = gabi::load<u32>(0x104B462C), actor = read<u32>(model, 0xB8);
    u32 index = read<u16>(joint, 4);
    if (actor) {
      u32 matrix = animationMatrix(model, index);
      gabi::call(0x028E90D4, ptr(matrix), ptr(MTX));
      gabi::call(0x025F25CC, ptr(actor + 0x564));
      gabi::call(0x028E90D4, ptr(MTX), ptr(0x104B4868));
      matrix = animationMatrix(model, index);
      mtx_copy(gabi::at<Mtx34>(matrix), gabi::at<Mtx34>(MTX));
      gabi::call(0x028E90D4, ptr(MTX), ptr(actor + 0x588));
    }
  }
  return 1;
}
VERIFY(0x021B3380, nodeCallback);
static void setMoveBGMtx(Actor *a) {
  WWHD_FUNC(0x021B34AC, void, a);
  f32 x = a->current.pos.x, y = a->current.pos.y, z = a->current.pos.z;
  gabi::call(0x028E93CC, ptr(MTX), x, y, z);
  gabi::call(0x025F1C28, ptr(MTX), s16(a->current.angle.y));
  gabi::call(0x025F25CC, &a->rotation);
  gabi::call(0x028E90D4, ptr(MTX), &a->transform);
}
VERIFY(0x021B34AC, setMoveBGMtx);
static void setMtx(Actor *a) {
  WWHD_FUNC(0x021B3514, void, a);
  f32 x = a->scale.x, y = a->scale.y, z = a->scale.z;
  u32 model = gabi::ea(a->model.get());
  write<f32>(model, 0xBC, x);
  write<f32>(model, 0xC0, y);
  write<f32>(model, 0xC4, z);
  x = a->current.pos.x;
  y = a->current.pos.y;
  z = a->current.pos.z;
  gabi::call(0x028E93CC, ptr(MTX), x, y, z);
  s16 rx = a->current.angle.x, ry = a->current.angle.y, rz = a->current.angle.z;
  gabi::call(0x025F1B48, ptr(MTX), rx, ry, rz);
  model = gabi::ea(a->model.get());
  mtx_copy(gabi::at<Mtx34>(model + 0xC8), gabi::at<Mtx34>(MTX));
}
VERIFY(0x021B3514, setMtx);
static void createInit(Actor *a) {
  WWHD_FUNC(0x021B35F4, void, a);
  u32 model = gabi::ea(a->model.get());
  a->cullMtx = model ? model + 0xC8 : 0;
  gabi::call(0x025D674C, a, -300.f, -600.f, -300.f, 300.f, 100.f, 300.f);
  write<f32>(gabi::ea(a), 0x364, 1.f);
  gabi::call(0x02515F14, ptr(address(a, 0x3B8)), u8(255), u8(255), a);
  gabi::call(0x02516518, ptr(address(a, 0x3F4)), ptr(0x101B8F90));
  ProtectedLocal<cXyz> water, particlePosition;
  water->x = a->current.pos.x;
  water->z = a->current.pos.z;
  water->y = f32(a->current.pos.y) + 200.f;
  write<u32>(gabi::ea(a), 0x438, address(a, 0x3B8));
  f32 height = gabi::call<f32>(0x024F1478, water.get());
  a->waterY = height;
  if (height != -1000000000.f) {
    particlePosition->x = a->current.pos.x;
    particlePosition->z = a->current.pos.z;
    particlePosition->y = height + 1.f;
    u32 emitter = particle(0x82AA, particlePosition.get(), &a->current.angle);
    a->emitter3 = ptr(emitter);
    if (emitter)
      stop(emitter);
  }
  f32 y = f32(a->home.pos.y) + 15.f;
  model = gabi::ea(a->model.get());
  a->home.pos.y = y;
  a->current.pos.y = y;
  u32 data = read<u32>(model, 0xAC),
      names = gabi::call<u32>(0x027F68FC, ptr(data));
  u32 relative = read<u32>(names, 0x10),
      stringTable = relative ? names + 0x10 + relative : 0;
  s32 index = gabi::call<s32>(0x027DF9B0, ptr(stringTable), STR(0x10013F00));
  if (index >= 0) {
    model = gabi::ea(a->model.get());
    data = read<u32>(model, 0xAC);
    u32 count = read<u32>(data, 4), joints = read<u32>(data, 8);
    u32 n = u16(index);
    if (n < count)
      joints += n * 0x1C;
    write<u32>(joints, 8, 0x021B3380);
  }
  resetRotation(a);
  for (u32 i = 0; i < 4; i++)
    write<u32>(gabi::ea(a), 0x564 + i * 4,
               read<u32>(gabi::ea(a), 0x574 + i * 4));
  if ((u32(a->mParameters) & 15) == 1)
    a->current.pos.y = f32(a->home.pos.y) + 560.f;
  else {
    u32 emitter = gabi::ea(a->emitter3.get());
    if (emitter)
      play(emitter);
  }
  model = gabi::ea(a->model.get());
  a->settleTimer = 30;
  gabi::call(0x027F4D5C, ptr(model));
  setMoveBGMtx(a);
  u32 p = gabi::call<u32>(0x025200D4);
  void *bg = a->background.get();
  gabi::call(0x024EEA6C, ptr(p + 0x12A0), bg, a);
  setMtx(a);
  gabi::call(0x024F43DC, a->background.get());
}
VERIFY(0x021B35F4, createInit);
static s32 create(Actor *a) {
  WWHD_FUNC(0x021B38D0, s32, a);
  if (!(u32(a->actor_condition) & 8)) {
    gabi::call(0x025D4ED0, a);
    write<u32>(gabi::ea(a), 0xB4, 0x10013EA0);
    gabi::call(0x0200BD2C, ptr(address(a, 0x3B8)));
    gabi::call(0x02515DA0, ptr(address(a, 0x3D4)));
    write<u32>(gabi::ea(a), 0x3D0, 0x1004AE88);
    write<u32>(gabi::ea(a), 0x3D4, 0x1004AEC0);
    gabi::call(0x02515FB8, ptr(address(a, 0x3F4)));
    write<u32>(gabi::ea(a), 0x508, 0x100015A8);
    write<u32>(gabi::ea(a), 0x504, 0x10013E90);
    gabi::call(0x02018590, ptr(address(a, 0x50C)));
    write<u32>(gabi::ea(a), 0x430, 0x1004B108);
    write<u32>(gabi::ea(a), 0x508, 0x1004B160);
    write<u32>(gabi::ea(a), 0x520, 0x1004B150);
    a->actor_condition = u32(a->actor_condition) | 8;
  }
  s32 phase = gabi::call<s32>(0x02520460, &a->phase, STR(0x10013F20));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, a, u32(0x021B337C), u32(0xF40)))
      return 5;
    createInit(a);
  }
  return phase;
}
VERIFY(0x021B38D0, create);
static s32 createWrapper(Actor *a) {
  WWHD_FUNC(0x021B3A08, s32, a);
  return create(a);
}
VERIFY(0x021B3A08, createWrapper);
static s32 remove(Actor *a) {
  WWHD_FUNC(0x021B3A0C, s32, a);
  u32 e = gabi::ea(a->emitter1.get());
  if (e) {
    invalidate(e);
    a->emitter1 = nullptr;
  }
  e = gabi::ea(a->emitter2.get());
  if (e) {
    invalidate(e);
    a->emitter2 = nullptr;
  }
  e = gabi::ea(a->emitter3.get());
  if (e) {
    invalidate(e);
    a->emitter3 = nullptr;
  }
  if (a->heap) {
    u32 p = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, ptr(p + 0x12A0), a->background.get());
  }
  gabi::call(0x025204C8, &a->phase, STR(0x10013F20));
  return 1;
}
VERIFY(0x021B3A0C, remove);
static s32 deleteWrapper(Actor *a) {
  WWHD_FUNC(0x021B3AD4, s32, a);
  return remove(a);
}
VERIFY(0x021B3AD4, deleteWrapper);
static s32 draw(Actor *a) {
  WWHD_FUNC(0x021B3AD8, s32, a);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), s32(1), &a->current.pos, &a->tevStr);
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), a->model.get(), &a->tevStr);
  u32 p = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, read<u32>(p, 0x5D70));
  p = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, read<u32>(p, 0x5D74));
  gabi::call(0x025E2E5C, a->model.get());
  p = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, read<u32>(p, 0x5D78));
  p = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, read<u32>(p, 0x5D7C));
  return 1;
}
VERIFY(0x021B3AD8, draw);
static s32 drawWrapper(Actor *a) {
  WWHD_FUNC(0x021B3B6C, s32, a);
  return draw(a);
}
VERIFY(0x021B3B6C, drawWrapper);
static s32 moveDown(Actor *a) {
  WWHD_FUNC(0x021B3B70, s32, a);
  if (!a->descending)
    return 1;
  f32 delta = gabi::call<f32>(0x0200ECD4, &a->current.pos.y, f32(a->home.pos.y),
                              0.1f, 10.f, 5.f);
  if (delta == 0.f) {
    u32 e = gabi::ea(a->emitter3.get());
    a->moving = 0;
    a->descending = 0;
    if (e)
      play(e);
    return 1;
  }
  if (!a->moving) {
    s32 reverb = gabi::call<s32>(0x02520540, s32(a->current.roomNo));
    gabi::call(0x025E1A40, u32(0x6980), &a->eyePos, u32(0), reverb);
    a->moving = 1;
  }
  return 0;
}
VERIFY(0x021B3B70, moveDown);
static void emitterCtrl(Actor *a) {
  WWHD_FUNC(0x021B3C4C, void, a);
  if (s32(a->emitterTimer) == 200) {
    u32 e = gabi::ea(a->emitter1.get());
    if (e) {
      invalidate(e);
      a->emitter1 = nullptr;
    }
    e = gabi::ea(a->emitter2.get());
    if (e) {
      invalidate(e);
      a->emitter2 = nullptr;
    }
  }
  u32 e = gabi::ea(a->emitter1.get());
  if (e)
    gabi::call(0x028249B0, &a->particleTransform, ptr(e + 0x1F0),
               ptr(e + 0x22C));
  e = gabi::ea(a->emitter2.get());
  if (e)
    gabi::call(0x028249B0, &a->particleTransform, ptr(e + 0x1F0),
               ptr(e + 0x22C));
}
VERIFY(0x021B3C4C, emitterCtrl);
static s32 execute(Actor *a) {
  WWHD_FUNC(0x021B3D04, s32, a);
  u32 p = gabi::call<u32>(0x025200D4);
  f32 distance = gabi::call<f32>(0x025D6958, a, ptr(read<u32>(p, 0x5B2C)));
  s16 life = s16(s16(a->lifetime) + 1);
  u8 timer = a->settleTimer, ridden = a->ridden;
  a->lifetime = life;
  if (timer) {
    timer--;
    a->settleTimer = timer;
    if (!timer)
      a->hasRider = 1;
  }
  if (!ridden && a->hasRider) {
    resetRotation(a);
    if (distance > 270.f)
      a->descending = 1;
  }
  ProtectedLocal<Opaque<4>> color, secondary;
  gabi::call(0x025602F0, color.get(), secondary.get());
  colorEmitter(a, 0x5C4, color.get());
  colorEmitter(a, 0x5C8, color.get());
  colorEmitter(a, 0x5BC, color.get());
  moveDown(a);
  setMtx(a);
  setMoveBGMtx(a);
  void *model = a->model.get();
  a->ridden = 0;
  gabi::call(0x027F4D5C, model);
  ProtectedLocal<Opaque<16>> rotation;
  gabi::call(0x028E9BC0, &a->rotation, &a->targetRotation, rotation.get(),
             0.25f);
  for (u32 i = 0; i < 4; i++)
    write<u32>(gabi::ea(a), 0x564 + i * 4,
               read<u32>(gabi::ea(rotation.get()), i * 4));
  gabi::call(0x024F43DC, a->background.get());
  ProtectedLocal<cXyz> pos;
  pos->x = a->current.pos.x;
  pos->y = f32(a->current.pos.y) - 560.f;
  pos->z = a->current.pos.z;
  gabi::call(0x020182E0, ptr(address(a, 0x50C)), pos.get());
  p = gabi::call<u32>(0x025200D4);
  gabi::call(0x0200E240, ptr(p + 0x26A4), ptr(address(a, 0x3F4)));
  emitterCtrl(a);
  return 1;
}
VERIFY(0x021B3D04, execute);
static s32 executeWrapper(Actor *a) {
  WWHD_FUNC(0x021B3F4C, s32, a);
  return execute(a);
}
VERIFY(0x021B3F4C, executeWrapper);
static void staticInit() {
  WWHD_FUNC(0x021B3F50, void);
  gabi::store<u32>(0x10464E04, 0);
  gabi::store<u32>(0x10464DFC, 0);
  gabi::store<u32>(0x10464E08, 0);
  gabi::store<u32>(0x10464E00, 0);
  gabi::call(0x028F026C, ptr(0x101B8FD4));
  gabi::store<f32>(0x10464DF0, -3.1415927410125732f);
  gabi::store<f32>(0x10464DF4, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x10464DF8));
  gabi::call(0x028F026C, ptr(0x101B8FE0));
  gabi::call(0x028EAB2C, ptr(0x10464DF9));
  gabi::call(0x028F026C, ptr(0x101B8FEC));
  gabi::store<f32>(0x10464E0C, 0.f);
  gabi::store<f32>(0x10464E10, 1.f);
  gabi::store<f32>(0x10464E14, 0.f);
}
VERIFY(0x021B3F50, staticInit);
static void deletingStatic(void *p, s32 flags) {
  WWHD_FUNC(0x021B4004, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x021B4004, deletingStatic);
static s32 isDelete(Actor *a) {
  WWHD_FUNC(0x021B4018, s32, a);
  return 1;
}
VERIFY(0x021B4018, isDelete);
static void destructor(Actor *a, s32 flags) {
  WWHD_FUNC(0x021B4020, void, a, flags);
  if (a) {
    gabi::call(0x02515A70, ptr(address(a, 0x3F4)), s32(2));
    gabi::call(0x02515860, ptr(address(a, 0x3B8)), s32(2));
    gabi::call(0x025D50BC, a, s32(0));
    if (flags & 1)
      gabi::call(0x0273AF40, a);
  }
}
VERIFY(0x021B4020, destructor);
static void empty() { WWHD_FUNC(0x021B408C, void); }
VERIFY(0x021B408C, empty);
static s32 moveUp(Actor *a) {
  WWHD_FUNC(0x021B4090, s32, a);
  f32 target = f32(a->home.pos.y) + 560.f, y = a->current.pos.y;
  a->emitterTimer = s32(u32(a->emitterTimer) + 1);
  if (y != target) {
    target = f32(a->home.pos.y) + 560.f;
    a->risingTilt = 1;
  }
  f32 delta =
      gabi::call<f32>(0x0200ECD4, &a->current.pos.y, target, 0.1f, 10.f, 5.f);
  s32 result = 0;
  ProtectedLocal<Opaque<4>> color, secondary;
  if (delta == 0.f) {
    a->moving = 0;
    result = 1;
  } else if (!a->moving) {
    s32 reverb = gabi::call<s32>(0x02520540, s32(a->current.roomNo));
    gabi::call(0x025E1A40, u32(0x697F), &a->eyePos, u32(0), reverb);
    a->moving = 1;
    a->emitter1 = ptr(particle(0x82AC, &a->current.pos, &a->current.angle));
    ProtectedLocal<cXyz> pos;
    pos->z = a->current.pos.z;
    pos->x = a->current.pos.x;
    pos->y = a->waterY;
    a->emitter2 = nullptr;
    a->emitter4 = ptr(particle(0x82AB, pos.get(), &a->current.angle));
    u32 e = gabi::ea(a->emitter3.get());
    a->emitterTimer = 0;
    if (e)
      stop(e);
  }
  gabi::call(0x025602F0, color.get(), secondary.get());
  colorEmitter(a, 0x5C4, color.get());
  colorEmitter(a, 0x5C8, color.get());
  colorEmitter(a, 0x5BC, color.get());
  return result;
}
VERIFY(0x021B4090, moveUp);
static s32 checkEndDown(Actor *a) {
  WWHD_FUNC(0x021B430C, s32, a);
  return !(f32(a->current.pos.y) > f32(a->home.pos.y));
}
VERIFY(0x021B430C, checkEndDown);

static void staticInitStaticTU() {
  WWHD_FUNC(0x021B4328, void);
  gabi::store<u32>(0x10464E2C, 0);
  gabi::store<u32>(0x10464E24, 0);
  gabi::store<u32>(0x10464E30, 0);
  gabi::store<u32>(0x10464E28, 0);
  gabi::call(0x028F026C, ptr(0x101B9028));
  gabi::store<f32>(0x10464E18, -3.1415927410125732f);
  gabi::store<f32>(0x10464E1C, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x10464E20));
  gabi::call(0x028F026C, ptr(0x101B9034));
  gabi::call(0x028EAB2C, ptr(0x10464E21));
  gabi::call(0x028F026C, ptr(0x101B9040));
}
VERIFY(0x021B4328, staticInitStaticTU);

#include "d/actor/d_a_obj_hlift.h"
#include "bindings.h"
using daObjHlift::Act_c;
namespace {
constexpr u32 ARC = 0x1002A790, DIST = 0x1002A798, SIZE = 0x1002A7A8;
constexpr u32 CONTROL = 0x101C9F7C, LAG = 0x101C9F80, MOVING = 0x101C9F84,
              MTX = 0x1048D0CC;
static s32 parameter(Act_c *a, s32 width, s32 shift) {
  WWHD_FUNC(0x02357208, s32, a, width, shift);
  u32 bits = a->mParameters;
  u32 w = u32(width) & 63, sh = u32(shift) & 63;
  u32 mask = w >= 32 ? 0 : 1u << w;
  return (sh >= 32 ? 0 : bits >> sh) & (mask - 1);
}
VERIFY(0x02357208, parameter);
static f32 distance(Act_c *a) {
  return f32(gabi::load<s16>(DIST + u32(s32(a->mDistance)) * 2));
}
static u32 play() { return gabi::call<u32>(0x025200D4); }
static s32 switched(Act_c *a, s32 sw) {
  u32 save = gabi::load<u32>(0x101F84DC);
  s8 room = a->home.roomNo;
  return gabi::call<s32>(0x025BA0C0, save + 0x20, sw, room);
}
static void sound(Act_c *a, u32 id) {
  s8 room = a->current.roomNo;
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call(0x025E1A40, id, &a->eyePos, 0, reverb);
}
static void copyMatrix(u32 dst) {
  f32 values[12];
  for (int i = 0; i < 12; i++)
    values[i] = gabi::load<f32>(MTX + 4 * i);
  for (int i = 0; i < 12; i++)
    gabi::store<f32>(dst + 4 * i, values[i]);
}
static void upperInit(Act_c *a) {
  WWHD_FUNC(0x023562EC, void, a);
  f32 d = distance(a);
  f32 y = a->home.pos.y;
  a->mMode = 2;
  a->current.pos.y = y + d;
}
VERIFY(0x023562EC, upperInit);
static void lowerInit(Act_c *a) {
  WWHD_FUNC(0x02356344, void, a);
  f32 y = a->home.pos.y;
  a->mMode = 0;
  a->current.pos.y = y;
}
VERIFY(0x02356344, lowerInit);
static void setMtx(Act_c *a) {
  WWHD_FUNC(0x02356358, void, a);
  f32 offset = a->mVibOffset;
  f32 y = a->current.pos.y;
  y = (y + offset) + 30.0f;
  f32 z = a->current.pos.z;
  f32 x = a->current.pos.x;
  gabi::call(0x028E93CC, MTX, x, y, z);
  s16 az = a->shape_angle.z, ax = a->shape_angle.x, ay = a->shape_angle.y;
  gabi::call(0x025F1B48, MTX, ax, ay, az);
  copyMatrix(gabi::ea(a->mModel1.get()) + 0xC8);
  gabi::call(0x028E90D4, MTX, 0x10469E90);
  x = a->home.pos.x;
  y = a->home.pos.y;
  z = a->home.pos.z;
  gabi::call(0x028E93CC, MTX, x, y, z);
  copyMatrix(gabi::ea(a->mModel2.get()) + 0xC8);
}
VERIFY(0x02356358, setMtx);
static void initMtx(Act_c *a) {
  WWHD_FUNC(0x023564B8, void, a);
  for (int n = 0; n < 2; n++) {
    u32 model = gabi::ea(n ? a->mModel2.get() : a->mModel1.get());
    f32 x = a->scale.x, y = a->scale.y, z = a->scale.z;
    gabi::store<f32>(model + 0xBC, x);
    gabi::store<f32>(model + 0xC0, y);
    gabi::store<f32>(model + 0xC4, z);
  }
  setMtx(a);
}
VERIFY(0x023564B8, initMtx);
static s32 create(Act_c *a) {
  WWHD_FUNC(0x023564F4, s32, a);
  a->mVibOffset = 0;
  s32 sw = parameter(a, 8, 8);
  if (switched(a, sw))
    upperInit(a);
  else
    lowerInit(a);
  a->mNextMode = 5;
  a->mDemo = 0;
  u8 event = parameter(a, 8, 16);
  u32 p = play();
  s32 eid = gabi::call<s32>(0x02543F10, p + 0x52C4, STR(0x1002A7B8), event);
  u32 model = gabi::ea(a->mModel1.get());
  a->mEventId = eid;
  a->cullMtx = model ? model + 0xC8 : 0;
  initMtx(a);
  gabi::call(0x025D674C, a, -151.f, -1005.f, -151.f, 151.f, 1.f, 151.f);
  if (gabi::load<u32>(CONTROL) == 0xFFFFFFFF)
    gabi::store<u32>(CONTROL, gabi::load<u32>(gabi::ea(a) + 4));
  return 1;
}
VERIFY(0x023564F4, create);
static u8 demoEnd(Act_c *a) {
  WWHD_FUNC(0x02356664, u8, a);
  u8 demo = a->mDemo;
  if (demo) {
    s16 id = a->mEventId;
    u32 p = play();
    if (gabi::call<s32>(0x025440C8, p + 0x52C4, id)) {
      p = play();
      u16 flag = gabi::load<u16>(p + 0x52B8);
      gabi::store<u16>(p + 0x52B8, flag | 8);
      a->mDemo = 0;
      demo = 0;
    } else
      demo = a->mDemo;
  }
  return demo ^ 1;
}
VERIFY(0x02356664, demoEnd);
static void rotate(Act_c *a) {
  WWHD_FUNC(0x023566EC, void, a);
  f32 y = a->current.pos.y, home = a->home.pos.y;
  a->shape_angle.y =
      s16(0u - u32(gabi::ftoi((y - home) * 262.144012451171875f)));
}
VERIFY(0x023566EC, rotate);
static void vibrate(Act_c *a) {
  WWHD_FUNC(0x02356728, void, a);
  s16 timer = a->mVibTimer;
  if (timer > 0) {
    u16 angle = a->mVibAngle;
    f32 factor = gabi::fmadds(f32(10 - timer), 0.05000000074505806f, 0.5f);
    f32 sine = gabi::load<f32>(0x104A44F8 + (u32(angle) >> 3) * 8);
    timer = a->mVibTimer;
    a->mVibAngle = angle + 0x4000;
    a->mVibTimer = timer - 1;
    a->mVibOffset = sine * (2.5f * factor);
  } else
    a->mVibOffset = 0;
}
VERIFY(0x02356728, vibrate);
static void soundWhole(Act_c *a) {
  WWHD_FUNC(0x023567D0, void, a);
  u32 id = gabi::load<u32>(gabi::ea(a) + 4);
  if (gabi::load<u32>(CONTROL) == id && gabi::load<u8>(MOVING)) {
    gabi::call(0x025E1988, 0x301A);
    gabi::store<u8>(MOVING, 0);
  }
}
VERIFY(0x023567D0, soundWhole);
static s32 execute(Act_c *a, u32 *out) {
  WWHD_FUNC(0x02356828, s32, a, out);
  demoEnd(a);
  ptmf_call(0x1002A70C + u32(s32(a->mMode)) * 8, a);
  s32 mode = a->mMode;
  if (mode == 1 || mode == 3 || mode == 4)
    gabi::store<u8>(MOVING, 1);
  rotate(a);
  vibrate(a);
  soundWhole(a);
  f32 x = a->current.pos.x, z = a->current.pos.z, y = a->current.pos.y;
  a->eyePos.z = z;
  a->eyePos.y = y;
  a->eyePos.x = x;
  setMtx(a);
  gabi::store<u32>(gabi::ea(out), 0x10469E90);
  return 1;
}
VERIFY(0x02356828, execute);
static s32 draw(Act_c *a) {
  WWHD_FUNC(0x02356928, s32, a);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, env, 1, &a->current.pos, &a->tevStr);
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, env, a->mModel1.get(), &a->tevStr);
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, env, a->mModel2.get(), &a->tevStr);
  u32 p = play();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D70));
  p = play();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D74));
  gabi::call(0x025E2DE0, a->mModel1.get(), 0);
  gabi::call(0x025E2DE0, a->mModel2.get(), 0);
  p = play();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D78));
  p = play();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D7C));
  return 1;
}
VERIFY(0x02356928, draw);
static s32 remove(Act_c *a) {
  WWHD_FUNC(0x023569DC, s32, a);
  u32 id = gabi::load<u32>(gabi::ea(a) + 4);
  if (gabi::load<u32>(CONTROL) == id)
    gabi::store<u32>(CONTROL, 0xFFFFFFFF);
  return 1;
}
VERIFY(0x023569DC, remove);
static void vibSet(Act_c *a) {
  WWHD_FUNC(0x02356A00, void, a);
  a->mVibTimer = 10;
  a->mVibAngle = 0x4000;
}
VERIFY(0x02356A00, vibSet);
static void demoInit(Act_c *a, s32 next) {
  WWHD_FUNC(0x02356A14, void, a, next);
  if (next != 3 && next != 1)
    gabi::call(0x0273AA24, STR(0x1002A734), 0x241, STR(0x1002A748));
  a->mNextMode = next;
  a->mDemo = 0;
  u8 event = parameter(a, 8, 16);
  s16 id = a->mEventId;
  gabi::call(0x025D7A58, a, id, event, 0xFFFF, 0, 1);
  u16 flags = gabi::load<u16>(gabi::ea(a) + 0xFA);
  a->mMode = 4;
  gabi::store<u16>(gabi::ea(a) + 0xFA, flags | 2);
}
VERIFY(0x02356A14, demoInit);
static void lower(Act_c *a) {
  WWHD_FUNC(0x02356AC8, void, a);
  s32 sw = parameter(a, 8, 8);
  if (switched(a, sw))
    demoInit(a, 1);
}
VERIFY(0x02356AC8, lower);
static void lagSet(Act_c *a) {
  s32 lag = gabi::load<s32>(LAG);
  a->mLag = gabi::load<u8>(0x1002A644 + u32(lag));
  lag = gabi::load<s32>(LAG);
  s32 next = s32(u32(lag) + 1);
  gabi::store<s32>(LAG, next % 5);
}
static void risingInit(Act_c *a) {
  WWHD_FUNC(0x02356B2C, void, a);
  vibSet(a);
  a->mMoveSpeed = 0;
  a->mMode = 1;
  sound(a, 0x6949);
  lagSet(a);
}
VERIFY(0x02356B2C, risingInit);
static void rising(Act_c *a) {
  WWHD_FUNC(0x02356BD0, void, a);
  s16 lag = a->mLag;
  if (lag > 0) {
    a->mLag = lag - 1;
    return;
  }
  f32 y = a->current.pos.y, home = a->home.pos.y;
  f32 height = y - home;
  if (height < 50.f)
    gabi::call<s32>(0x0200F5C8, &a->mMoveSpeed, 10.f, 0.3f);
  else if (height < distance(a) - 50.f)
    a->mMoveSpeed = 10.f;
  else
    gabi::call<s32>(0x0200F5C8, &a->mMoveSpeed, 2.f, 1.2f);
  f32 d = distance(a);
  home = a->home.pos.y;
  f32 speed = a->mMoveSpeed;
  if (gabi::call<s32>(0x0200F5C8, &a->current.pos.y, home + d, speed)) {
    vibSet(a);
    sound(a, 0x694A);
    upperInit(a);
  }
}
VERIFY(0x02356BD0, rising);
static void upper(Act_c *a) {
  WWHD_FUNC(0x02356DDC, void, a);
  s32 sw = parameter(a, 8, 8);
  if (!switched(a, sw))
    demoInit(a, 3);
}
VERIFY(0x02356DDC, upper);
static void fallingInit(Act_c *a) {
  WWHD_FUNC(0x02356E40, void, a);
  vibSet(a);
  a->mMoveSpeed = 0;
  sound(a, 0x6949);
  lagSet(a);
  a->mMode = 3;
}
VERIFY(0x02356E40, fallingInit);
static void falling(Act_c *a) {
  WWHD_FUNC(0x02356EE0, void, a);
  s16 lag = a->mLag;
  if (lag > 0) {
    a->mLag = lag - 1;
    return;
  }
  f32 home = a->home.pos.y, y = a->current.pos.y;
  f32 height = y - home;
  if (height < 50.f)
    gabi::call<s32>(0x0200F5C8, &a->mMoveSpeed, 2.f, 1.2f);
  else if (height < distance(a) - 50.f)
    a->mMoveSpeed = 10.f;
  else
    gabi::call<s32>(0x0200F5C8, &a->mMoveSpeed, 10.f, 0.3f);
  home = a->home.pos.y;
  f32 speed = a->mMoveSpeed;
  if (gabi::call<s32>(0x0200F5C8, &a->current.pos.y, home, speed)) {
    vibSet(a);
    sound(a, 0x694A);
    lowerInit(a);
  }
}
VERIFY(0x02356EE0, falling);
static void demo(Act_c *a) {
  WWHD_FUNC(0x02357040, void, a);
  s16 id = a->mEventId;
  u32 p = play();
  if (gabi::call<u32>(0x02544044, p + 0x52C4, id) &&
      gabi::load<u16>(gabi::ea(a) + 0xF8) == 2)
    a->mDemo = 1;
  if (s32(a->mNextMode) == 3)
    fallingInit(a);
  else
    risingInit(a);
}
VERIFY(0x02357040, demo);
static s32 createHeap(Act_c *a) {
  WWHD_FUNC(0x023561E4, s32, a);
  gabi::Local<SafeString> name;
  name->__vtbl = 0x1002A608;
  s16 resource = gabi::load<s16>(SIZE + u32(s32(a->mSize)) * 8);
  u32 control = gabi::load<u32>(0x101F4F28);
  name->mStringTop = ARC;
  void *data = gabi::call<void *>(0x026066C4, control, name.get(), resource);
  if (!data)
    gabi::call(0x0273AA24, STR(0x1002A6B4), 0x120, STR(0x1002A6A4));
  a->mModel1 = gabi::call<J3DModel *>(0x025E38E0, data, 0x80000, 0x11000022);
  gabi::Local<SafeString> name2;
  name2->__vtbl = 0x1002A608;
  control = gabi::load<u32>(0x101F4F28);
  name2->mStringTop = ARC;
  data = gabi::call<void *>(0x026066C4, control, name2.get(), 6);
  if (!data)
    gabi::call(0x0273AA24, STR(0x1002A6B4), 0x12A, STR(0x1002A6C8));
  J3DModel *model =
      gabi::call<J3DModel *>(0x025E38E0, data, 0x80000, 0x11000022);
  J3DModel *first = a->mModel1;
  a->mModel2 = model;
  return first && model;
}
VERIFY(0x023561E4, createHeap);
static s32 methodCreate(Act_c *a) {
  WWHD_FUNC(0x02356090, s32, a);
  if (!(u32(a->actor_condition) & 8)) {
    if (a) {
      gabi::call(0x024F1D40, a);
      gabi::store<u32>(gabi::ea(a) + 0xB4, 0x1002A7C4);
    }
    a->actor_condition = u32(a->actor_condition) | 8;
  }
  s32 state = gabi::call<s32>(0x02520460, &a->mPhase, STR(ARC));
  if (state == 4) {
    a->mDistance = parameter(a, 3, 0);
    s32 size = parameter(a, 1, 4);
    a->mSize = size;
    u32 row = SIZE + u32(size) * 8;
    u32 heap = gabi::load<u32>(row + 4);
    s16 dzb = gabi::load<s16>(row + 2);
    state = gabi::call<s32>(0x024F1D9C, a, STR(ARC), dzb, 0x024EE708, heap);
    if (state != 4 && state != 5)
      gabi::call(0x0273AA24, STR(0x1002A64C), 0x16E, STR(0x1002A660));
  }
  return state;
}
VERIFY(0x02356090, methodCreate);
static s32 methodDelete(Act_c *a) {
  WWHD_FUNC(0x02356198, s32, a);
  s32 result = gabi::call<s32>(0x024F1F64, a);
  gabi::call(0x025204C8, &a->mPhase, STR(ARC));
  return result;
}
VERIFY(0x02356198, methodDelete);
static s32 wrapperCreate(Act_c *a) {
  WWHD_FUNC(0x023570D4, s32, a);
  return methodCreate(a);
}
VERIFY(0x023570D4, wrapperCreate);
static s32 wrapperDelete(Act_c *a) {
  WWHD_FUNC(0x023570D8, s32, a);
  return methodDelete(a);
}
VERIFY(0x023570D8, wrapperDelete);
static s32 wrapperExecute(Act_c *a) {
  WWHD_FUNC(0x023570DC, s32, a);
  return gabi::call<s32>(0x024F1E9C, a);
}
VERIFY(0x023570DC, wrapperExecute);
static s32 wrapperIsDelete(Act_c *a) {
  WWHD_FUNC(0x023570E0, s32, a);
  u32 vt = gabi::load<u32>(gabi::ea(a) + 0xB4);
  return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x2C), a);
}
VERIFY(0x023570E0, wrapperIsDelete);
static s32 wrapperDraw(Act_c *a) {
  WWHD_FUNC(0x023570F0, s32, a);
  u32 vt = gabi::load<u32>(gabi::ea(a) + 0xB4);
  return gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x3C), a);
}
VERIFY(0x023570F0, wrapperDraw);
static void sinit() {
  WWHD_FUNC(0x02357100, void, (u32)0);
  sinit_header_statics(0x10469E74, 0x101C9F58);
}
VERIFY(0x02357100, sinit);
static void headerDelete(void *a, s32 flags) {
  WWHD_FUNC(0x02357194, void, a, flags);
  if (a && (flags & 1))
    gabi::call(0x0273AF40, a);
}
VERIFY(0x02357194, headerDelete);
static s32 isDelete(Act_c *a) {
  WWHD_FUNC(0x023571A8, s32, a);
  return 1;
}
VERIFY(0x023571A8, isDelete);
static void empty(Act_c *a) { WWHD_FUNC(0x023571B0, void, a); }
VERIFY(0x023571B0, empty);
static void actorDelete(Act_c *a, s32 flags) {
  WWHD_FUNC(0x023571B4, void, a, flags);
  if (a) {
    gabi::call(0x025D50BC, a, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, a);
  }
}
VERIFY(0x023571B4, actorDelete);
} // namespace

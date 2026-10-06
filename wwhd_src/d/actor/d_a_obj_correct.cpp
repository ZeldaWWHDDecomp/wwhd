#include "d/actor/d_a_obj_correct.h"
#include "bindings.h"
using daObjCorrect::Act_c;
namespace {
constexpr u32 ATTR = 0x10027080, SEARCH = 0x10027190;
static s32 abstractBits(fopAc_ac_c *a, s32 width, s32 shift) {
  u32 bits = a->mParameters, w = u32(width) & 63, sh = u32(shift) & 63;
  u32 mask = w >= 32 ? 0 : 1u << w;
  return (sh >= 32 ? 0 : bits >> sh) & (mask - 1);
}
static s32 parameter(Act_c *a, s32 w, s32 s) {
  WWHD_FUNC(0x02333CA0, s32, a, w, s);
  return abstractBits(a, w, s);
}
VERIFY(0x02333CA0, parameter);
static s32 tryParameter(fopAc_ac_c *a, s32 w, s32 s) {
  WWHD_FUNC(0x02333CBC, s32, a, w, s);
  return abstractBits(a, w, s);
}
VERIFY(0x02333CBC, tryParameter);
static s32 boxParameter(fopAc_ac_c *a, s32 w, s32 s) {
  WWHD_FUNC(0x02333CD8, s32, a, w, s);
  return abstractBits(a, w, s);
}
VERIFY(0x02333CD8, boxParameter);
static u32 attr(Act_c *a, u32 offset) {
  return ATTR + u32(s32(a->mType)) * 16 + offset;
}
static u32 play() { return gabi::call<u32>(0x025200D4); }
static void onInit(Act_c *a) {
  WWHD_FUNC(0x02332DA0, void, a);
  f32 radius = gabi::load<f32>(attr(a, 4));
  a->mMode = 1;
  a->mRadiusSq = radius;
}
VERIFY(0x02332DA0, onInit);
static void offInit(Act_c *a) {
  WWHD_FUNC(0x02332DC4, void, a);
  f32 radius = gabi::load<f32>(attr(a, 8));
  a->mMode = 0;
  a->mRadiusSq = radius;
}
VERIFY(0x02332DC4, offInit);
static void nonInit(Act_c *a) {
  WWHD_FUNC(0x02332DE8, void, a);
  a->mDemo = 0;
}
VERIFY(0x02332DE8, nonInit);
static s32 create(Act_c *a) {
  WWHD_FUNC(0x02332DF4, s32, a);
  s32 sw = parameter(a, 8, 8);
  if (!(u32(a->actor_condition) & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, a);
      a->__vtbl = 0x10027058;
    }
    a->actor_condition = u32(a->actor_condition) | 8;
  }
  if (sw == 255)
    return 5;
  a->mType = parameter(a, 4, 0);
  gabi::call(0x025DA884, gabi::at<void>(gabi::ea(a) + 0xDC));
  s8 room = a->home.roomNo;
  u32 save = gabi::load<u32>(0x101F84DC);
  if (gabi::call<s32>(0x025BA0C0, save + 0x20, sw, room))
    onInit(a);
  else
    offInit(a);
  u32 name = gabi::load<u32>(attr(a, 0));
  if (name) {
    u8 event = parameter(a, 8, 16);
    u32 p = play();
    a->mEventId = gabi::call<s32>(0x02543F10, p + 0x52C4, name, event);
  } else
    a->mEventId = -1;
  nonInit(a);
  return 4;
}
VERIFY(0x02332DF4, create);
static u8 execute(Act_c *a) {
  WWHD_FUNC(0x02332F6C, u8, a);
  ptmf_call(0x10027100 + u32(s32(a->mMode)) * 8, a);
  ptmf_call(0x10027110 + u32(s32(a->mDemo)) * 8, a);
  return 1;
}
VERIFY(0x02332F6C, execute);
static fopAc_ac_c *chkTry0(fopAc_ac_c *a) {
  WWHD_FUNC(0x0233305C, fopAc_ac_c *, a);
  if (gabi::call<s32>(0x025D4604, a) && a &&
      gabi::load<s16>(gabi::ea(a) + 8) == 0x1CA)
    return a;
  return nullptr;
}
VERIFY(0x0233305C, chkTry0);
static fopAc_ac_c *chkTry1(Act_c *a, fopAc_ac_c *other, s32 type, f32 radius) {
  WWHD_FUNC(0x023330AC, fopAc_ac_c *, a, other, type, radius);
  if (tryParameter(other, 4, 0) != type)
    return nullptr;
  f32 dist = gabi::call<f32>(0x028E8DE8, &other->current.pos, &a->current.pos);
  if (!(dist < radius))
    return nullptr;
  s32 sw = parameter(a, 8, 8);
  if (sw != tryParameter(other, 8, 8))
    return nullptr;
  u32 dst = gabi::ea(other);
  f32 x = a->current.pos.x;
  s16 angle = a->shape_angle.y;
  gabi::store<f32>(dst + 0x7B8, x);
  f32 y = a->current.pos.y;
  gabi::store<f32>(dst + 0x7BC, y);
  f32 z = a->current.pos.z;
  gabi::store<u8>(dst + 0x7C9, 1);
  gabi::store<f32>(dst + 0x7C0, z);
  gabi::store<s16>(dst + 0x7C4, angle);
  gabi::store<u8>(dst + 0x7C8, 1);
  return other;
}
VERIFY(0x023330AC, chkTry1);
static fopAc_ac_c *chkTry2(Act_c *a, fopAc_ac_c *other, s32 type, u8 enabled,
                           f32 radius, f32 depress) {
  WWHD_FUNC(0x023331BC, fopAc_ac_c *, a, other, type, enabled, radius, depress);
  if (!(radius <= depress))
    gabi::call(0x0273AA24, STR(0x10027128), 346, STR(0x1002713C));
  if (tryParameter(other, 4, 0) != type)
    return nullptr;
  f32 dist = gabi::call<f32>(0x028E8DE8, &other->current.pos, &a->current.pos);
  if (!(dist < depress))
    return nullptr;
  bool match = false;
  if (dist < radius) {
    s32 sw = parameter(a, 8, 8);
    match = sw == tryParameter(other, 8, 8);
  }
  u32 dst = gabi::ea(other);
  f32 x = a->current.pos.x;
  s16 angle = a->shape_angle.y;
  gabi::store<f32>(dst + 0x7B8, x);
  f32 y = a->current.pos.y;
  gabi::store<f32>(dst + 0x7BC, y);
  f32 z = a->current.pos.z;
  gabi::store<s16>(dst + 0x7C4, angle);
  gabi::store<f32>(dst + 0x7C0, z);
  gabi::store<u8>(dst + 0x7C8, match);
  gabi::store<u8>(dst + 0x7C9, enabled);
  return match ? other : nullptr;
}
VERIFY(0x023331BC, chkTry2);
static void *searchBox(fopAc_ac_c *actor, Act_c *a) {
  WWHD_FUNC(0x02333324, void *, actor, a);
  u32 id = actor ? gabi::load<u32>(gabi::ea(actor) + 4) : 0xFFFFFFFF;
  if (!gabi::call<s32>(0x025DD868, id)) {
    if (gabi::call<s32>(0x025D4604, actor) && actor &&
        gabi::load<s16>(gabi::ea(actor) + 8) == 0x2B) {
      f32 dist =
          gabi::call<f32>(0x028E8DE8, &actor->current.pos, &a->current.pos);
      f32 radius = a->mRadiusSq;
      if (dist < radius) {
        s32 sw = boxParameter(actor, 8, 8);
        if (sw == parameter(a, 8, 8) && boxParameter(actor, 1, 30) == 0) {
          a->mSearchValid = 1;
          return actor;
        }
      }
      a->mSearchValid = 0;
    }
  } else if (actor && gabi::load<s16>(gabi::ea(actor) + 8) == 0x2B)
    a->mSearchValid = 0;
  return nullptr;
}
VERIFY(0x02333324, searchBox);
static void *sun(fopAc_ac_c *actor, Act_c *a) {
  WWHD_FUNC(0x02333458, void *, actor, a);
  fopAc_ac_c *res = chkTry0(actor);
  if (res)
    return chkTry1(a, res, 2, a->mRadiusSq);
  return nullptr;
}
VERIFY(0x02333458, sun);
static void *mercury(fopAc_ac_c *actor, Act_c *a) {
  WWHD_FUNC(0x023334B4, void *, actor, a);
  fopAc_ac_c *res = chkTry0(actor);
  if (res)
    return chkTry1(a, res, 3, a->mRadiusSq);
  return nullptr;
}
VERIFY(0x023334B4, mercury);
static void *jupiter(fopAc_ac_c *actor, Act_c *a) {
  WWHD_FUNC(0x02333510, void *, actor, a);
  fopAc_ac_c *res = chkTry0(actor);
  if (res)
    return chkTry1(a, res, 4, a->mRadiusSq);
  return nullptr;
}
VERIFY(0x02333510, jupiter);
static void *keySearch(fopAc_ac_c *actor, Act_c *a, s32 type) {
  u32 id = actor ? gabi::load<u32>(gabi::ea(actor) + 4) : 0xFFFFFFFF;
  if (gabi::call<s32>(0x025DD868, id)) {
    if (actor && gabi::load<s16>(gabi::ea(actor) + 8) == 0x1CA)
      a->mSearchValid = 0;
    return nullptr;
  }
  fopAc_ac_c *res = chkTry0(actor);
  if (!res)
    return nullptr;
  f32 radius = a->mRadiusSq, depress = gabi::load<f32>(attr(a, 12));
  return chkTry2(a, res, type, 1, radius, depress);
}
static void *gate(fopAc_ac_c *actor, Act_c *a) {
  WWHD_FUNC(0x0233356C, void *, actor, a);
  return keySearch(actor, a, 5);
}
VERIFY(0x0233356C, gate);
static void *door(fopAc_ac_c *actor, Act_c *a) {
  WWHD_FUNC(0x02333624, void *, actor, a);
  return keySearch(actor, a, 6);
}
VERIFY(0x02333624, door);
static void *green(fopAc_ac_c *actor, Act_c *a) {
  WWHD_FUNC(0x023336DC, void *, actor, a);
  fopAc_ac_c *res = chkTry0(actor);
  if (!res)
    return nullptr;
  void *result = nullptr;
  for (int i = 0; i < 6; i++) {
    s32 type = gabi::load<s32>(0x1002715C + i * 4);
    f32 radius = a->mRadiusSq, depress = gabi::load<f32>(attr(a, 12));
    if (chkTry2(a, res, type, 1, radius, depress))
      result = res;
  }
  return result;
}
VERIFY(0x023336DC, green);
static void *blue(fopAc_ac_c *actor, Act_c *a) {
  WWHD_FUNC(0x02333788, void *, actor, a);
  return green(actor, a);
}
VERIFY(0x02333788, blue);
static void switchSet(Act_c *a, u32 target) {
  s32 sw = parameter(a, 8, 8);
  u32 save = gabi::load<u32>(0x101F84DC);
  s8 room = a->home.roomNo;
  gabi::call(target, save + 0x20, sw, room);
}
static void off(Act_c *a) {
  WWHD_FUNC(0x0233378C, void, a);
  s32 type = a->mType;
  a->mSearchValid = 1;
  u32 callback = gabi::load<u32>(SEARCH + u32(type) * 4);
  void *res = gabi::call<void *>(0x025D5218, callback, a);
  s16 valid = a->mSearchValid;
  if (valid == 1 && res) {
    switchSet(a, 0x025B9E38);
    onInit(a);
  }
}
VERIFY(0x0233378C, off);
static void on(Act_c *a) {
  WWHD_FUNC(0x02333820, void, a);
  s32 type = a->mType;
  a->mSearchValid = 1;
  u32 callback = gabi::load<u32>(SEARCH + u32(type) * 4);
  void *res = gabi::call<void *>(0x025D5218, callback, a);
  s16 valid = a->mSearchValid;
  if (valid == 1 && !res) {
    switchSet(a, 0x025B9F7C);
    offInit(a);
  }
}
VERIFY(0x02333820, on);
static void requestInit(Act_c *a) {
  WWHD_FUNC(0x023338B4, void, a);
  s16 id = a->mEventId;
  u32 p = play();
  if (gabi::call<u32>(0x02544044, p + 0x52C4, id)) {
    u8 event = parameter(a, 8, 16);
    id = a->mEventId;
    gabi::call(0x025D7A58, a, id, event, 0xFFFF, 0, 1);
    u16 flags = gabi::load<u16>(gabi::ea(a) + 0xFA);
    a->mDemo = 1;
    gabi::store<u16>(gabi::ea(a) + 0xFA, flags | 2);
  } else
    nonInit(a);
}
VERIFY(0x023338B4, requestInit);
static void non(Act_c *a) {
  WWHD_FUNC(0x02333968, void, a);
  s32 type = a->mType;
  if (type != 4 && type != 5)
    return;
  u32 p = play();
  fopAc_ac_c *player = gabi::at<fopAc_ac_c>(gabi::load<u32>(p + 0x5B2C));
  u32 flags = gabi::load<u32>(gabi::ea(player) + 0x3C0);
  if (!(flags & 0x400000))
    return;
  u32 vt = player->__vtbl;
  s32 id = gabi::call_ptr<s32>(gabi::load<u32>(vt + 0xBC), player);
  fopAc_ac_c *actor = nullptr;
  if (id != -1) {
    gabi::Local<be<s32>> localId;
    *localId = id;
    actor = gabi::call<fopAc_ac_c *>(0x025D5218, 0x025E1234, localId.get());
  }
  if (!actor)
    return;
  actor = chkTry0(actor);
  if (!actor)
    return;
  type = tryParameter(actor, 4, 0);
  if (!((type == 5 && s32(a->mType) == 4) || (type == 6 && s32(a->mType) == 5)))
    return;
  gabi::Local<cXyz> playerXZ, correctXZ;
  f32 x = a->current.pos.x;
  f32 px = player->current.pos.x, pz = player->current.pos.z;
  correctXZ->x = x;
  correctXZ->y = 0;
  playerXZ->z = pz;
  f32 z = a->current.pos.z;
  playerXZ->x = px;
  correctXZ->z = z;
  playerXZ->y = 0;
  f32 dist = gabi::call<f32>(0x028E8DE8, playerXZ.get(), correctXZ.get());
  f32 py = player->current.pos.y, y = a->current.pos.y;
  f32 dy = std::fabs(py - y);
  s16 angle = gabi::call<s16>(0x025D6894, player, a);
  s16 facing = player->shape_angle.y;
  angle = s16(s32(facing) - angle);
  if (((dist < 40000.f && angle >= -0x4000 && angle <= 0x4000) ||
       dist < 1600.f) &&
      dy < 60.f)
    requestInit(a);
}
VERIFY(0x02333968, non);
static void runInit(Act_c *a) {
  WWHD_FUNC(0x02333B14, void, a);
  a->mDemo = 2;
}
VERIFY(0x02333B14, runInit);
static void request(Act_c *a) {
  WWHD_FUNC(0x02333B20, void, a);
  if (gabi::load<u16>(gabi::ea(a) + 0xF8) == 2)
    runInit(a);
  else
    nonInit(a);
}
VERIFY(0x02333B20, request);
static void run(Act_c *a) {
  WWHD_FUNC(0x02333B34, void, a);
  s16 id = a->mEventId;
  u32 p = play();
  if (gabi::call<s32>(0x025440C8, p + 0x52C4, id)) {
    p = play();
    u16 flags = gabi::load<u16>(p + 0x52B8);
    gabi::store<u16>(p + 0x52B8, flags | 8);
    nonInit(a);
  }
}
VERIFY(0x02333B34, run);
static s32 wrapperCreate(Act_c *a) {
  WWHD_FUNC(0x02333B98, s32, a);
  return create(a);
}
VERIFY(0x02333B98, wrapperCreate);
static s32 wrapperDelete(Act_c *a) {
  WWHD_FUNC(0x02333B9C, s32, a);
  return 1;
}
VERIFY(0x02333B9C, wrapperDelete);
static u8 wrapperExecute(Act_c *a) {
  WWHD_FUNC(0x02333BA4, u8, a);
  return execute(a);
}
VERIFY(0x02333BA4, wrapperExecute);
static s32 wrapperDraw(Act_c *a) {
  WWHD_FUNC(0x02333BA8, s32, a);
  return 1;
}
VERIFY(0x02333BA8, wrapperDraw);
static void sinit() {
  WWHD_FUNC(0x02333BB0, void, (u32)0);
  sinit_header_statics(0x104695DC, 0x101C8A4C);
}
VERIFY(0x02333BB0, sinit);
static void actorDelete(Act_c *a, s32 flags) {
  WWHD_FUNC(0x02333C44, void, a, flags);
  if (a) {
    gabi::call(0x025D50BC, a, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, a);
  }
}
VERIFY(0x02333C44, actorDelete);
static s32 wrapperIsDelete(Act_c *a) {
  WWHD_FUNC(0x02333C98, s32, a);
  return 1;
}
VERIFY(0x02333C98, wrapperIsDelete);
} // namespace

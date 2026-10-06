/* Stalfos; derived from WWHD disassembly. */
#include "d/actor/d_a_st.h"
namespace Stalfos {
template <class T> T *at(u32 p, u32 off = 0) { return gabi::at<T>(p + off); }
template <class T> T *member(st_class *actor, u32 off) {
  return at<T>(gabi::ea(actor), off);
}
u32 word(u32 p) { return *at<be<u32>>(p); }
// Nonleaf guests may save LR at callerSP+4 even with no stack arguments.
struct OutgoingLinkage {
  be<u32> backchain, savedLR;
};
template <class R = void, class... A> R guest_call(u32 target, A... args) {
  gabi::Local<OutgoingLinkage> frame;
  frame->backchain = frame.a + frame.kSize;
  return gabi::call<R>(target, args...);
}
template <class R = void, class... A> R guest_call_ptr(u32 target, A... args) {
  gabi::Local<OutgoingLinkage> frame;
  frame->backchain = frame.a + frame.kSize;
  return gabi::call_ptr<R>(target, args...);
}
struct ResourceLocal {
  be<u32> linkage[2], text, vtable;
};
u32 resource(gabi::Local<ResourceLocal> &local, u32 name, u32 id) {
  local->text = name;
  local->vtable = 0x1003D4C4;
  return guest_call<u32>(0x026066C4, at<void>(word(0x101F4F28)), &local->text,
                         id);
}
void anm_init(st_class *actor, s32 animation, f32 morph, u32 loop, f32 speed,
              s32 sound) {
  WWHD_FUNC(0x02491208, void, actor, animation, morph, loop, speed, sound);
  gabi::Local<ResourceLocal> animationName, soundName;
  u32 animationData = resource(animationName, 0x1003D5E4, animation);
  u32 soundData = sound >= 0 ? resource(soundName, 0x1003D5E4, sound) : 0;
  guest_call(0x025E4A98, (void *)actor->bodyAnimation, at<void>(animationData),
             loop, morph, speed, 0.0f, -1.0f, at<void>(soundData));
}
VERIFY(0x02491208, anm_init);
void head_anm_init(st_class *actor, s32 animation, f32 morph, u32 loop,
                   f32 speed, s32 sound) {
  WWHD_FUNC(0x02491330, void, actor, animation, morph, loop, speed, sound);
  gabi::Local<ResourceLocal> animationName, soundName;
  u32 animationData = resource(animationName, 0x1003D5E7, animation);
  u32 soundData = sound >= 0 ? resource(soundName, 0x1003D5E7, sound) : 0;
  guest_call(0x025E4A98, (void *)actor->headAnimation, at<void>(animationData),
             loop, morph, speed, 0.0f, -1.0f, at<void>(soundData));
}
VERIFY(0x02491330, head_anm_init);
void wait_set(st_class *actor) {
  WWHD_FUNC(0x0249210C, void, actor);
  if (*member<be<s8>>(actor, 0x2010)) {
    anm_init(actor, 0x15, 10.0f, 2, 1.0f, -1);
    *member<be<s8>>(actor, 0x2010) = 2;
    if (actor && gabi::ea(actor) + 0x37C) {
      s8 room = actor->current.roomNo;
      u32 id = *member<be<u32>>(actor, 4);
      s32 reverb = guest_call<s32>(0x02520540, room);
      guest_call(0x025E1AA4, 0x4882, &actor->eyePos, id, 0, reverb);
    }
  } else {
    anm_init(actor, *member<be<s8>>(actor, 0x3E8) ? 0x21 : 0x33, 10.0f, 2, 1.0f,
             -1);
  }
}
VERIFY(0x0249210C, wait_set);
void anm_init_default(st_class *actor, s32 animation, f32 morph, u32 loop) {
  WWHD_FUNC(0x02492200, void, actor, animation, morph, loop);
  anm_init(actor, animation, morph, loop, 1.0f, -1);
}
VERIFY(0x02492200, anm_init_default);
BOOL daSt_IsDelete(st_class *actor) {
  WWHD_FUNC(0x024957AC, BOOL, actor);
  return 1;
}
VERIFY(0x024957AC, daSt_IsDelete);
BOOL daSt_Delete(st_class *actor) {
  WWHD_FUNC(0x024957B4, BOOL, actor);
  guest_call(0x025204C8, &actor->phase, STR(0x1003D6CC));
  if (*member<be<u8>>(actor, 0x417)) {
    s8 child = *at<be<s8>>(0x1046DFF0);
    *at<be<u8>>(0x101D0E78) = 0;
    guest_call(0x025F0A18, child);
  }
  for (u32 i = 0; i < 3; ++i) {
    u32 callback = gabi::ea(actor) + 0x20D0 + i * 0x20;
    guest_call_ptr(word(word(callback) + 0x44), at<void>(callback));
  }
  u32 callback = gabi::ea(actor) + 0x21D0;
  guest_call_ptr(word(word(callback) + 0x44), at<void>(callback));
  for (u32 i = 0; i < 20; ++i) {
    u32 emitter = gabi::ea((void *)actor->emitters[i]);
    if (emitter) {
      u32 flags = *at<be<u32>>(emitter, 0x254);
      *at<be<u32>>(emitter, 0x5C) = 0xFFFFFFFF;
      *at<be<u32>>(emitter, 0x254) = flags | 1;
      actor->emitters[i] = nullptr;
    }
  }
  return 1;
}
VERIFY(0x024957B4, daSt_Delete);
u32 smoke_construct(void *callback) {
  WWHD_FUNC(0x02495AC8, u32, callback);
  return guest_call<u32>(0x025A5B18, callback, 1); /* dPa_smokeEcallBack ctor, returns this */
}
VERIFY(0x02495AC8, smoke_construct);
void hio_destroy(void *self, u32 flags) {
  WWHD_FUNC(0x024961A4, void, self, flags);
  if (self && (flags & 1))
    guest_call(0x0273AF40, self);
}
VERIFY(0x024961A4, hio_destroy);
void smoke_destroy(void *self, u32 flags) {
  WWHD_FUNC(0x0249A0FC, void, self, flags);
  if (self && (flags & 1))
    guest_call(0x0273AF40, self);
}
VERIFY(0x0249A0FC, smoke_destroy);
void part_destroy(st_p *self, u32 flags) {
  WWHD_FUNC(0x0249A110, void, self, flags);
  if (self && (flags & 1))
    guest_call(0x0273AF40, self);
}
VERIFY(0x0249A110, part_destroy);
st_p *part_construct(st_p *part) {
  WWHD_FUNC(0x0249A0B0, st_p *, part);
  if (!part)
    part = at<st_p>(guest_call<u32>(0x0273AD10, 0x6C));
  if (part)
    guest_call(0x025A5B18, &part->smoke, 1);
  return part;
}
VERIFY(0x0249A0B0, part_construct);

st_class *actor_construct(st_class *actor) {
  WWHD_FUNC(0x02495AD0, st_class *, actor);
  if (!actor)
    actor = at<st_class>(guest_call<u32>(0x0273AD10, 0x25B0));
  if (!actor)
    return actor;
  guest_call(0x025D4ED0, actor);
  actor->__vtbl = 0x1003D5BC;
  guest_call(0x024EFE94, &actor->acchCir);
  guest_call(0x024F0474, &actor->acch);
  *member<be<u32>>(actor, 0x478) = 0x1003D54C;
  *member<be<u8>>(actor, 0x480) = 1;
  *member<be<u32>>(actor, 0x488) = 0x1003D55C;
  *member<be<u32>>(actor, 0x47C) = 0x1003D56C;
  guest_call(0x0200BD2C, &actor->status);
  guest_call(0x02515DA0, member<void>(actor, 0x648));
  *member<be<u32>>(actor, 0x644) = 0x1004AE88;
  *member<be<u32>>(actor, 0x648) = 0x1004AEC0;
  guest_call(0x028EFFD0, &actor->spheres, 7, 0x12C, at<void>(0x025166F0));
  guest_call(0x025166F0, &actor->weaponSphere);
  guest_call(0x028EFFD0, &actor->parts, 26, 0x6C, at<void>(0x0249A0B0));
  guest_call(0x025E9960, &actor->lineMaterial);
  guest_call(0x028EFFD0, &actor->smoke, 3, 0x20, at<void>(0x02495AC8));
  guest_call(0x025A5B18, &actor->spinSmoke, 1);
  guest_call(0x0200BD2C, member<void>(actor, 0x2220));
  guest_call(0x02515DA0, member<void>(actor, 0x223C));
  *member<be<u32>>(actor, 0x2238) = 0x1004AE88;
  *member<be<u32>>(actor, 0x223C) = 0x1004AEC0;
  guest_call(0x02515FB8, member<void>(actor, 0x225C));
  *member<be<u32>>(actor, 0x2370) = 0x100015A8;
  *member<be<u32>>(actor, 0x236C) = 0x1003D4DC;
  guest_call(0x02018590, member<void>(actor, 0x2374));
  *member<be<u32>>(actor, 0x2298) = 0x1004B108;
  *member<be<u32>>(actor, 0x2388) = 0x1004B150;
  *member<be<u32>>(actor, 0x2370) = 0x1004B160;
  guest_call(0x024EFE94, member<void>(actor, 0x23A4));
  guest_call(0x024F0474, member<void>(actor, 0x23E4));
  *member<be<u32>>(actor, 0x23F4) = 0x1003D54C;
  *member<be<u32>>(actor, 0x23F8) = 0x1003D56C;
  *member<be<u8>>(actor, 0x23FC) = 1;
  *member<be<u32>>(actor, 0x2404) = 0x1003D55C;
  return actor;
}
VERIFY(0x02495AD0, actor_construct);
void actor_destroy(st_class *actor, u32 flags) {
  WWHD_FUNC(0x0249A124, void, actor, flags);
  if (!actor)
    return;
  *member<be<u32>>(actor, 0x2404) = 0x1003D55C;
  *member<be<u32>>(actor, 0x23F8) = 0x1003D56C;
  guest_call(0x024EFD9C, member<void>(actor, 0x23E4), 0);
  guest_call(0x02018034, member<void>(actor, 0x23B8), 2);
  guest_call(0x02515A70, member<void>(actor, 0x225C), 2);
  guest_call(0x02515860, member<void>(actor, 0x2220), 2);
  guest_call(0x028F0164, &actor->smoke, 3, 0x20, at<void>(0x0249A0FC), 0, 0);
  guest_call(0x025E99E0, &actor->lineMaterial, 2);
  guest_call(0x028F0164, &actor->parts, 26, 0x6C, at<void>(0x0249A110), 0, 0);
  guest_call(0x02515AE8, &actor->weaponSphere, 2);
  guest_call(0x028F0164, &actor->spheres, 7, 0x12C, at<void>(0x02515AE8), 0, 0);
  guest_call(0x02515860, &actor->status, 2);
  *member<be<u32>>(actor, 0x488) = 0x1003D55C;
  *member<be<u32>>(actor, 0x47C) = 0x1003D56C;
  guest_call(0x024EFD9C, &actor->acch, 0);
  guest_call(0x02018034, member<void>(actor, 0x43C), 2);
  guest_call(0x025D50BC, actor, 0);
  if (flags & 1)
    guest_call(0x0273AF40, actor);
}
VERIFY(0x0249A124, actor_destroy);
void hio_message(void *self, void *context) {
  WWHD_FUNC(0x0249A274, void, self, context);
}
VERIFY(0x0249A274, hio_message);
void static_initialize() {
  WWHD_FUNC(0x024960B4, void, (u32)0);
  *at<be<u32>>(0x1046DFE8) = 0;
  *at<be<u32>>(0x1046DFE0) = 0;
  *at<be<u32>>(0x1046DFEC) = 0;
  *at<be<u32>>(0x1046DFE4) = 0;
  guest_call(0x028F026C, at<void>(0x101D0FAC));
  *at<be<f32>>(0x1046DFD4) = -3.1415927410125732f;
  *at<be<f32>>(0x1046DFD8) = 3.1415927410125732f;
  guest_call(0x028ED6F8, at<void>(0x1046DFDD));
  guest_call(0x028F026C, at<void>(0x101D0FB8));
  guest_call(0x028EAB2C, at<void>(0x1046DFDE));
  guest_call(0x028F026C, at<void>(0x101D0FC4));
  *at<be<f32>>(0x1046DFFC) = 6.0f;
  *at<be<s16>>(0x1046DFF2) = 9;
  *at<be<u32>>(0x1046E008) = 0x1003D5CC;
  *at<be<s16>>(0x1046DFF4) = 0x2C;
  *at<be<s16>>(0x1046DFF6) = 0x15;
  *at<be<f32>>(0x1046E000) = 7.0f;
  *at<be<s16>>(0x1046DFF8) = 0x35;
  *at<be<s16>>(0x1046E004) = 0x46;
  *at<be<s16>>(0x1046E006) = 200;
}
VERIFY(0x024960B4, static_initialize);

BOOL createHeap(st_class *actor) {
  WWHD_FUNC(0x024958A0, BOOL, actor);
  gabi::Local<ResourceLocal> bodyModelName, bodyAnimationName, headModelName,
      headAnimationName, partModelName;
  u32 modelData = resource(bodyModelName, 0x1003D6CF, 0x39);
  u32 animationData = resource(bodyAnimationName, 0x1003D6CF, 0x33);
  u32 animation =
      guest_call<u32>(0x025E4F64, (void *)nullptr, at<void>(modelData),
                      (void *)nullptr, (void *)nullptr, at<void>(animationData),
                      2, 1.0f, (void *)nullptr, -1, 1, 0, 0x80000, 0x11000022);
  actor->bodyAnimation = at<void>(animation);
  if (!animation || !word(animation + 0x90))
    return 0;
  modelData = resource(headModelName, 0x1003D6CF, 0x38);
  animationData = resource(headAnimationName, 0x1003D6CF, 0x13);
  animation =
      guest_call<u32>(0x025E4F64, (void *)nullptr, at<void>(modelData),
                      (void *)nullptr, (void *)nullptr, at<void>(animationData),
                      2, 1.0f, (void *)nullptr, -1, 1, 0, 0x80000, 0x11000022);
  actor->headAnimation = at<void>(animation);
  if (!animation || !word(animation + 0x90))
    return 0;
  for (u32 i = 0; i < 26; ++i) {
    u16 modelIndex = *at<be<u16>>(0x101D0EB8, i * 2);
    if (modelIndex) {
      u32 data = resource(partModelName, 0x1003D6CF, modelIndex);
      u32 model =
          guest_call<u32>(0x025E38E0, at<void>(data), 0x80000, 0x11000022);
      actor->parts[i].model = at<void>(model);
      if (!model)
        return 0;
    }
  }
  return guest_call<s32>(0x025E9B80, &actor->lineMaterial, 3, 10, 0) != 0;
}
VERIFY(0x024958A0, createHeap);
BOOL daSt_Draw(st_class *actor) {
  WWHD_FUNC(0x02491F34, BOOL, actor);
  if (actor->ambushState || *member<be<s8>>(actor, 0x20CD) ||
      *member<be<s8>>(actor, 0x416))
    return 1;
  guest_call(0x025BED80, 0xC3, actor, 1.0f, 1.0f, 1.0f);
  for (u32 i = 0; i < 26; ++i) {
    if (actor->parts[i].state >= 10)
      continue;
    if (*member<be<s8>>(actor, 0x2010) && (i == 5 || i == 6 || i == 7))
      continue;
    if (i == 11)
      continue;
    u32 model = gabi::ea((void *)actor->parts[i].model);
    if (!model)
      continue;
    if ((i == 15 || i == 16 || i == 17) &&
        *member<be<s8>>(actor, 0x2020) == 2) {
      if (i != 15)
        continue;
      model = word(gabi::ea((void *)actor->headAnimation) + 0x90);
      u32 environment = guest_call<u32>(0x02555D0C);
      guest_call(0x02562F5C, at<void>(environment), at<void>(model),
                 &actor->tevStr);
      guest_call(0x025E5590, (void *)actor->headAnimation);
    } else {
      u32 environment = guest_call<u32>(0x02555D0C);
      guest_call(0x02562F5C, at<void>(environment), at<void>(model),
                 &actor->tevStr);
      guest_call(0x025E2DE0, at<void>(model), 0);
    }
  }
  if (*member<be<s16>>(actor, 0x3E2) == 0x21 ||
      *member<be<s16>>(actor, 0x3E0) == 0x23 ||
      *member<be<s8>>(actor, 0x21F6) || *member<be<s16>>(actor, 0x40C))
    return 1;
  guest_call(0x025EA548, &actor->lineMaterial, 10, 1.2f, at<void>(0x101D0E74),
             2, &actor->tevStr);
  u32 play = guest_call<u32>(0x025200D4);
  u32 packet = guest_call_ptr<u32>(word(word(gabi::ea(actor) + 0x1F9C) + 0x14),
                                   &actor->lineMaterial);
  guest_call(0x025EDD04, at<void>(play + 0x5FB4 + packet * 0x9C),
             &actor->lineMaterial);
  return 1;
}
VERIFY(0x02491F34, daSt_Draw);

struct VectorLocal {
  be<u32> linkage[2];
  cXyz value;
};
void speed_pos_calc(st_class *actor) {
  WWHD_FUNC(0x024961B8, void, actor);
  gabi::Local<VectorLocal> direction, rotated;
  if ((*member<be<u32>>(actor, 0x490) & 0x20) &&
      *member<be<s16>>(actor, 0x400)) {
    f32 x = *member<be<f32>>(actor, 0x3EC) - actor->current.pos.x;
    f32 z = *member<be<f32>>(actor, 0x3F4) - actor->current.pos.z;
    s16 angle = guest_call<s16>(0x020195B0, x, z);
    s16 step = *member<be<s16>>(actor, 0x400);
    guest_call(0x0200F428, &actor->current.angle.y, angle, 2, step);
    *member<be<s16>>(actor, 0x400) = 0;
  }
  s16 angle =
      (s16)((s16)actor->current.angle.y + *member<be<s16>>(actor, 0x41E));
  guest_call(0x025F1884, at<void>(word(0x1018C7B0)), angle);
  direction->value.x = 0.0f;
  direction->value.y = 0.0f;
  direction->value.z = actor->speedF;
  guest_call(0x0200FCD8, &direction->value, &rotated->value);
  f32 target = *member<be<f32>>(actor, 0x3F8),
      step = *member<be<f32>>(actor, 0x3FC);
  guest_call(0x0200ED84, &actor->speedF, target, 1.0f, step);
  actor->speed.x = rotated->value.x;
  f32 extra = *member<be<f32>>(actor, 0x2028);
  actor->speed.z = rotated->value.z;
  *member<be<f32>>(actor, 0x3FC) = 1.0f;
  // PPC ble skips the branch for unordered comparisons.
  if (extra > 0.1f) {
    s16 extraAngle = *member<be<s16>>(actor, 0x2026);
    guest_call(0x025F1884, at<void>(word(0x1018C7B0)), extraAngle);
    direction->value.x = 0.0f;
    direction->value.y = 0.0f;
    direction->value.z = *member<be<f32>>(actor, 0x2028);
    guest_call(0x0200FCD8, &direction->value, &rotated->value);
    guest_call(0x028E8D88, &actor->speed, &rotated->value, &actor->speed);
  }
  f32 decay = *at<be<f32>>(0x1047B634) + 2.5f;
  guest_call(0x0200EDC8, member<void>(actor, 0x2028), 1.0f, decay);
  f32 x = actor->current.pos.x, vertical = actor->speed.y, dx = actor->speed.x;
  f32 falling = vertical - 5.0f, y = actor->current.pos.y;
  actor->current.pos.x = x + dx;
  actor->current.pos.y = y + vertical;
  if (falling < -50.0f)
    falling = -50.0f;
  f32 z = actor->current.pos.z, dz = actor->speed.z;
  actor->speed.y = falling;
  actor->current.pos.z = z + dz;
  u32 play = guest_call<u32>(0x025200D4);
  guest_call(0x024F08A8, &actor->acch, at<void>(play + 0x12A0));
}
VERIFY(0x024961B8, speed_pos_calc);
cPhs_State daSt_Create(st_class *actor) {
  WWHD_FUNC(0x02495C70, cPhs_State, actor);
  u32 condition = actor->actor_condition;
  if (!(condition & 8)) {
    if (actor)
      actor_construct(actor);
    actor->actor_condition = (u32)actor->actor_condition | 8;
  }
  s32 phase = guest_call<s32>(0x02520460, &actor->phase, STR(0x1003D6DC));
  if (phase != 4)
    return phase;
  for (u32 i = 0; i < 2; ++i)
    *member<be<u8>>(actor, 0x20E2 + i * 0x20) = 1;
  u32 parameters = actor->mParameters;
  s16 deathSwitch = actor->current.angle.z;
  actor->gbaName = 4;
  actor->sightRange = parameters >> 8;
  actor->behavior = parameters & 0xF;
  actor->deathSwitch = deathSwitch;
  actor->unusedParam = parameters >> 16;
  actor->ambushSwitch = parameters >> 24;
  actor->current.angle.z = 0;
  u8 switchID = (u8)deathSwitch;
  if (switchID) {
    if (switchID <= 0x7F)
      actor->actor_status = (u32)actor->actor_status | 0x04000000;
    if (switchID == 0xFF)
      actor->deathSwitch = 0;
    else if (guest_call<s32>(0x025BA0C0, at<void>(word(0x101F84DC) + 0x20),
                             switchID, (s8)actor->current.roomNo))
      return 5;
  }
  u32 play = guest_call<u32>(0x025200D4);
  actor->itemTableIdx =
      guest_call<s32>(0x0200E814, at<void>(play + 0x50AC), STR(0x1003D6E0), 0);
  if (!guest_call<s32>(0x025D63E8, actor, at<void>(0x024958A0), 0))
    return 5;
  if (!*at<be<u8>>(0x101D0E78)) {
    s32 child =
        guest_call<s32>(0x025F0A10, STR(0x1003D6E8), at<void>(0x1046DFF0));
    *at<be<s8>>(0x1046DFF0) = child;
    *member<be<u8>>(actor, 0x417) = 1;
    *at<be<u8>>(0x101D0E78) = 1;
  }
  u32 body = gabi::ea((void *)actor->bodyAnimation);
  if (!body || !word(body + 0x90))
    return 5;
  u8 behavior = actor->behavior;
  if (behavior == 1 || behavior == 2 || behavior == 3) {
    *member<be<s16>>(actor, 0x3E2) = behavior == 1 ? 0xF : 0x10;
    u8 ambushSwitch = actor->ambushSwitch;
    actor->ambushState = ambushSwitch == 0xFF ? 0xFF : (u8)(ambushSwitch + 1);
  }
  actor->actor_status = (u32)actor->actor_status | 0x4000;
  actor->emergenceDelay = 5;
  u32 model = word(gabi::ea((void *)actor->bodyAnimation) + 0x90);
  *at<be<u32>>(model, 0xB8) = gabi::ea(actor);
  actor->mBtBodyR = 125.0f;
  actor->mBtHeight = 162.5f;
  actor->actor_status = (u32)actor->actor_status | 0x20;
  *member<be<u32>>(actor, 0x39C) = 4;
  guest_call(0x024F06B4, &actor->acch, &actor->current.pos, &actor->old.pos,
             actor, 1, &actor->acchCir, &actor->speed, (void *)nullptr,
             (void *)nullptr);
  guest_call(0x024EFF44, &actor->acchCir, 60.0f, 100.0f);
  guest_call(0x02515F14, &actor->status, 0x32, 2, actor);
  for (u32 i = 0; i < 7; ++i) {
    guest_call(0x0251677C, &actor->spheres[i],
               at<void>(i ? 0x101D0F2C : 0x101D0EEC));
    *member<be<u32>>(actor, 0x6AC + i * 0x12C) = gabi::ea(actor) + 0x62C;
  }
  guest_call(0x0251677C, &actor->weaponSphere, at<void>(0x101D0F6C));
  *member<be<u32>>(actor, 0xEE0) = gabi::ea(actor) + 0x62C;
  if (actor->behavior != 14) {
    u32 weapon = guest_call<u32>(0x025D5834, 0x1CF, 2, &actor->current.pos,
                                 (s8)actor->current.roomNo, (void *)nullptr,
                                 (void *)nullptr, -1, (void *)nullptr);
    *member<be<u32>>(actor, 0x21F0) = gabi::ea(actor);
    actor->health = 16;
    *member<be<u8>>(actor, 0x20CC) = 16;
    actor->max_health = 16;
    *member<be<u8>>(actor, 0xFEE) = 0x19;
    *member<be<u32>>(actor, 0x2014) = weapon;
    *member<be<u32>>(actor, 0x201C) = 0xFFF;
    *member<be<u8>>(actor, 0x2018) = 1;
  } else {
    actor->health = 16;
    *member<be<u8>>(actor, 0x20CC) = 5;
    *member<be<s16>>(actor, 0x3E2) = 0x20;
    *member<be<u32>>(actor, 0x201C) = 0xFFF;
    *member<be<u32>>(actor, 0x21F0) = gabi::ea(actor);
    *member<be<u8>>(actor, 0xFEF) = 0x14;
    actor->max_health = 16;
    *member<be<u8>>(actor, 0xFEE) = 0x19;
  }
  *member<be<u8>>(actor, 0x23A1) = actor->deathSwitch;
  actor->stealItemLeft = 3;
  *member<be<s16>>(actor, 0x412) = 15;
  guest_call(0x02493FB0, actor);
  return phase;
}
VERIFY(0x02495C70, daSt_Create);

struct LineCheckLocal {
  be<u32> linkage[2];
  u8 check[0x6C];
};
void initialize_line(gabi::Local<LineCheckLocal> &local) {
  u32 check = gabi::ea(&local->check);
  guest_call(0x02008FEC, at<void>(check));
  for (u32 i = 0; i < 7; ++i)
    *at<be<u8>>(check, 0x5C + i) = 0;
  *at<be<u32>>(check, 0x64) = 0x1003D59C;
  *at<be<u32>>(check, 0x10) = 0x1003D57C;
  *at<be<u32>>(check, 0) = check + 0x58;
  *at<be<u32>>(check, 4) = check + 0x64;
  *at<be<u32>>(check, 0x68) = 1;
  *at<be<u32>>(check, 0x20) = 0x1003D58C;
  *at<be<u32>>(check, 0x58) = 0x1003D5AC;
}
void finish_line(gabi::Local<LineCheckLocal> &local) {
  u32 check = gabi::ea(&local->check);
  *at<be<u32>>(check, 0x58) = 0x1003D5AC;
  *at<be<u32>>(check, 0x64) = 0x1003D4FC;
  *at<be<u32>>(check, 0x20) = 0x1003D4EC;
  guest_call(0x02008B4C, at<void>(check), 0);
}
bool line_cross(gabi::Local<LineCheckLocal> &local, cXyz *start, cXyz *end,
                st_class *actor) {
  guest_call(0x024F1AFC, &local->check, start, end, actor);
  u32 play = guest_call<u32>(0x025200D4);
  return guest_call<s32>(0x02008860, at<void>(play + 0x12A0), &local->check) !=
         0;
}
f32 part_posmove(st_class *actor, st_p *part) {
  WWHD_FUNC(0x02492210, f32, actor, part);
  gabi::Local<VectorLocal> start, end, difference;
  gabi::Local<LineCheckLocal> line;
  initialize_line(line);
  // The previous position is a raw word copy, preserving NaN payload bits.
  u32 y = word(gabi::ea(part) + 0xC), x = word(gabi::ea(part) + 8),
      z = word(gabi::ea(part) + 0x10);
  *at<be<u32>>(gabi::ea(part), 0x14) = x;
  *at<be<u32>>(gabi::ea(part), 0x1C) = z;
  *at<be<u32>>(gabi::ea(part), 0x18) = y;
  guest_call(0x028E8D88, &part->pos, &part->velocity, &part->pos);
  f32 vertical = part->velocity.y;
  if (!(vertical < -50.0f))
    part->velocity.y = vertical - 5.0f;
  f32 floor = (f32)part->pos.y - 100.0f;
  guest_call(0x0201A554, &part->angle, &part->angularSpeed);
  f32 px = part->pos.x, py = part->pos.y, pz = part->pos.z;
  start->value.x = px;
  start->value.y = py + 100.0f;
  start->value.z = pz;
  end->value.x = px;
  end->value.y = py - 100.0f;
  end->value.z = pz;
  if (part->velocity.y > 0.0f) {
    if (line_cross(line, &end->value, &start->value, actor)) {
      u32 check = gabi::ea(&line->check);
      f32 crossZ = *at<be<f32>>(check, 0x38),
          crossY = *at<be<f32>>(check, 0x34),
          crossX = *at<be<f32>>(check, 0x30);
      end->value.z = crossZ;
      end->value.y = crossY;
      end->value.x = crossX;
      if (!((f32)part->pos.y < crossY)) {
        part->velocity.y = -5.0f;
        part->pos.y = crossY - 10.0f;
      }
    }
  } else if (line_cross(line, &start->value, &end->value, actor)) {
    u32 check = gabi::ea(&line->check);
    f32 crossX = *at<be<f32>>(check, 0x30);
    floor = *at<be<f32>>(check, 0x34);
    end->value.x = crossX;
    end->value.y = floor;
    end->value.z = *at<be<f32>>(check, 0x38);
  }
  guest_call(0x0201ADE0, &part->pos, &difference->value, &part->previous);
  *at<be<u32>>(gabi::ea(&start->value), 0) = word(gabi::ea(&difference->value));
  *at<be<u32>>(gabi::ea(&start->value), 8) =
      word(gabi::ea(&difference->value) + 8);
  start->value.y = 0.0f;
  f32 magnitudeSquared = guest_call<f32>(0x028E8DD0, &start->value);
  f64 magnitude = guest_call<f64>(0x028F4384, magnitudeSquared);
  if (magnitude > 0.0f) {
    u32 matrix = word(0x1018C7B0);
    s16 angle =
        guest_call<s16>(0x020195B0, (f32)start->value.x, (f32)start->value.z);
    guest_call(0x025F1884, at<void>(matrix), angle);
    start->value.x = 0.0f;
    start->value.y = 30.0f;
    f32 velocitySquared = guest_call<f32>(0x028E8DD0, &part->velocity);
    f64 velocityMagnitude = guest_call<f64>(0x028F4384, velocitySquared);
    start->value.z = (f32)(velocityMagnitude + 30.0f);
    guest_call(0x0200FCD8, &start->value, &end->value);
    f32 y = part->pos.y, x = part->pos.x, z = part->pos.z;
    start->value.x = x;
    start->value.y = y + 30.0f;
    start->value.z = z;
    guest_call(0x028E8D88, &end->value, &part->pos, &end->value);
    if (line_cross(line, &start->value, &end->value, actor)) {
      f32 vx = part->velocity.x, px = part->previous.x, vz = part->velocity.z,
          pz = part->previous.z;
      part->pos.x = px;
      part->pos.z = pz;
      part->velocity.x = vx * -0.3f;
      part->velocity.z = vz * -0.3f;
    }
  }
  finish_line(line);
  return floor;
}
VERIFY(0x02492210, part_posmove);

struct IDLocal {
  be<u32> linkage[2], id;
};
void sound_at(st_class *actor, u32 sound, cXyz *position) {
  if (actor && gabi::ea(position)) {
    s32 reverb = guest_call<s32>(0x02520540, (s8)actor->current.roomNo);
    guest_call(0x025E1A40, sound, position, 0, reverb);
  }
}
void st_break_wait(st_class *actor) {
  WWHD_FUNC(0x024963A4, void, actor);
  auto &state = *member<be<s16>>(actor, 0x3E0);
  auto &recoverTimer = *member<be<s16>>(actor, 0x408);
  auto &lifetime = *member<be<s16>>(actor, 0x414);
  *member<be<f32>>(actor, 0x3F8) = 0.0f;
  if (!*member<be<s16>>(actor, 0x2024))
    *member<be<s16>>(actor, 0x412) = 10;
  *member<be<s8>>(actor, 0xFEC) = 1;
  switch ((s16)state) {
  case 0:
    anm_init(actor, 0xE, 10.0f, 2, 1.0f, -1);
    state = 1;
    recoverTimer = *at<be<s16>>(0x1046E006);
    *member<be<s16>>(actor, 0x2024) = *at<be<s16>>(0x1046E006);
    [[fallthrough]];
  case 1:
    if (!recoverTimer) {
      state = 2;
      *member<be<s8>>(actor, 0xFEF) = 2;
      sound_at(actor, 0x5892, &actor->current.pos);
      lifetime = 0x1F3; // The HD body stores499, unlike GC500.
      return;
    }
    break;
  case 2:
  case 3:
  case 0xB:
    *member<be<s8>>(actor, 0xFEC) = 2;
    *member<be<s8>>(actor, 0x21B0) = 1;
    if (*member<be<s8>>(actor, 0xFEE) >= 25 &&
        ((s16)state != 3 || *member<be<s8>>(actor, 0x2018) == 10)) {
      state = 100;
      recoverTimer = 10;
    }
    break;
  case 0xA:
    anm_init(actor, 0xE, 10.0f, 2, 1.0f, -1);
    state = 0xB;
    *member<be<s8>>(actor, 0xFEC) = 2;
    *member<be<s8>>(actor, 0x21B0) = 1;
    if (*member<be<s8>>(actor, 0xFEE) >= 25) {
      state = 100;
      recoverTimer = 10;
    }
    break;
  case 0x14:
  case 0x23:
    *member<be<s16>>(actor, 0x2024) = 100;
    recoverTimer = 100;
    if (state == 0x23) {
      *member<be<u32>>(actor, 0x39C) = 0;
      actor->actor_status = (u32)actor->actor_status & ~0x20u;
    }
    break;
  case 100:
    *member<be<s8>>(actor, 0x21B0) = 2;
    if (!recoverTimer) {
      *member<be<s16>>(actor, 0x3E2) = 0;
      wait_set(actor);
      state = 1;
      f64 random = guest_call<f64>(0x020198D8, 40.0f);
      *member<be<u8>>(actor, 0x20CC) = 16;
      *member<be<s16>>(actor, 0x40A) = (s16)gabi::ftoi(random);
    }
    break;
  }
  if (lifetime) {
    --lifetime;
    if (!lifetime) {
      gabi::Local<VectorLocal> effect;
      effect->value.x = actor->current.pos.x;
      effect->value.y = actor->current.pos.y;
      effect->value.z = actor->current.pos.z;
      effect->value.y = (f32)effect->value.y + 100.0f;
      guest_call(0x025D99E8, actor, &effect->value, 10, 0, 0xFF);
      guest_call(0x025D57E0, actor);
      if (actor->deathSwitch)
        guest_call(0x025B9E38, at<void>(word(0x101F84DC) + 0x20),
                   (u8)actor->deathSwitch, (s8)actor->current.roomNo);
      guest_call(0x025BA5D4, at<void>(word(0x101F84DC) + 0x20),
                 (u16)actor->setID, (s8)actor->home.roomNo);
      gabi::Local<IDLocal> weapon;
      weapon->id = *member<be<u32>>(actor, 0x2014);
      u32 held = 0;
      if ((u32)weapon->id != 0xFFFFFFFF)
        held = guest_call<u32>(0x025D5218, at<void>(0x025E1234), &weapon->id);
      if (held)
        guest_call(0x025D9D24, at<void>(held));
    }
  }
}
VERIFY(0x024963A4, st_break_wait);
struct AngleLocal {
  be<u32> linkage[2];
  csXyz value;
};
void head_effect(st_class *actor, gabi::Local<VectorLocal> &zero,
                 gabi::Local<VectorLocal> &effect,
                 gabi::Local<AngleLocal> &angle) {
  u32 model = word(gabi::ea((void *)actor->bodyAnimation) + 0x90);
  u32 matrixData = word(model + 0x2C);
  u16 flags = *at<be<u16>>(matrixData, 4);
  u32 matrix = word(matrixData + 0x10);
  *at<be<u16>>(matrixData, 4) = flags | 0x10;
  guest_call(0x028E90D4, at<void>(matrix + 0x2D0), at<void>(word(0x1018C7B0)));
  guest_call(0x0200FCD8, &zero->value, &effect->value);
  u32 play = guest_call<u32>(0x025200D4);
  guest_call(0x025A847C, at<void>(word(play + 0x5AB0)), 0, 0x817A,
             &effect->value, &angle->value, (void *)nullptr, 0xFF,
             (void *)nullptr, -1, (void *)nullptr, (void *)nullptr,
             (void *)nullptr);
  sound_at(actor, 0x5890, &actor->eyePos);
}
void head_damage(st_class *actor) {
  WWHD_FUNC(0x02496718, void, actor);
  gabi::Local<VectorLocal> zero, effect;
  gabi::Local<AngleLocal> angle;
  zero->value.x = 0.0f;
  zero->value.z = 0.0f;
  zero->value.y = 0.0f;
  angle->value.x = actor->shape_angle.x;
  angle->value.y = actor->shape_angle.y;
  angle->value.z = actor->shape_angle.z;
  angle->value.y = (s16)((s16)angle->value.y - 0x8000);
  *member<be<f32>>(actor, 0x3F8) = 0.0f;
  auto &state = *member<be<s16>>(actor, 0x3E0);
  switch ((s16)state) {
  case 0:
  case 10: {
    bool upper = state == 10;
    anm_init(actor, upper ? 0x23 : 0x25, 5.0f, 0, 1.0f, -1);
    state = upper ? 11 : 1;
    sound_at(actor, 0x588F, &actor->eyePos);
    break;
  }
  case 1:
  case 11: {
    bool upper = state == 11;
    u32 animation = gabi::ea((void *)actor->bodyAnimation);
    s32 frame = gabi::ftoi(*at<be<f32>>(animation, 0x9C));
    if (frame == (upper ? 30 : 32))
      head_effect(actor, zero, effect, angle);
    animation = gabi::ea((void *)actor->bodyAnimation);
    if ((*at<be<u8>>(animation, 0xA7) & 1) ||
        *at<be<f32>>(animation, 0x98) == 0.0f) {
      if (upper) {
        *member<be<s16>>(actor, 0x3E2) = 0x20;
        state = -1;
      } else {
        *member<be<s16>>(actor, 0x3E2) = 0;
        wait_set(actor);
        state = 1;
        f64 random = guest_call<f64>(0x020198D8, 40.0f);
        *member<be<s16>>(actor, 0x40A) = (s16)gabi::ftoi(random);
      }
    }
    break;
  }
  }
}
VERIFY(0x02496718, head_damage);

void raw_vector_copy(u32 destination, u32 source) {
  u32 x = word(source), y = word(source + 4), z = word(source + 8);
  *at<be<u32>>(destination) = x;
  *at<be<u32>>(destination + 4) = y;
  *at<be<u32>>(destination + 8) = z;
}
u32 player_pointer() {
  u32 play = guest_call<u32>(0x025200D4);
  return word(play + 0x5B2C);
}
s16 player_angle(st_class *actor) {
  return guest_call<s16>(0x025D6894, actor, at<void>(player_pointer()));
}
f64 player_distance(st_class *actor) {
  return guest_call<f64>(0x025D68EC, actor, at<void>(player_pointer()));
}
bool animation_stopped(st_class *actor) {
  u32 animation = gabi::ea((void *)actor->bodyAnimation);
  return (*at<be<u8>>(animation, 0xA7) & 1) ||
         *at<be<f32>>(animation, 0x98) == 0.0f;
}
void ue_move(st_class *actor) {
  WWHD_FUNC(0x02496AA0, void, actor);
  u32 player = player_pointer();
  actor->actor_status = (u32)actor->actor_status | 0x20;
  auto &state = *member<be<s16>>(actor, 0x3E0);
  *member<be<u32>>(actor, 0x39C) = 4;
  switch ((s16)state) {
  case -1:
    anm_init(actor, 0x2F, 2.0f, 0, 1.0f, -1);
    state = 1;
    actor->speed.y = 30.0f;
    break;
  case 0:
    anm_init(actor, 0x2F, 2.0f, 0, 1.0f, -1);
    state = 1;
    actor->current.angle.y = (s16)(player_angle(actor) + 0x8000);
    actor->speed.y = 40.0f;
    actor->speedF = 20.0f;
    *member<be<f32>>(actor, 0x3F8) = 20.0f;
    break;
  case 1:
    if (*member<be<u32>>(actor, 0x490) & 0x20) {
      anm_init(actor, 0x30, 2.0f, 0, 1.0f, -1);
      state = 2;
    }
    break;
  case 2:
    if (animation_stopped(actor)) {
      anm_init(actor, 0x32, 5.0f, 2, 1.2f, -1);
      state = 3;
    }
    break;
  case 3: {
    gabi::Local<VectorLocal> direction, rotated, target;
    u32 matrix = word(0x1018C7B0);
    s16 angle = player_angle(actor);
    f64 random = guest_call<f64>(0x02019918, 10000.0f);
    angle = (s16)(angle + 0x8000 + (s16)gabi::ftoi(random));
    guest_call(0x025F1884, at<void>(matrix), angle);
    direction->value.x = 0.0f;
    direction->value.y = 0.0f;
    direction->value.z = 500.0f;
    guest_call(0x0200FCD8, &direction->value, &rotated->value);
    guest_call(0x0201AD78, at<void>(player + 0x314), &target->value,
               &rotated->value);
    raw_vector_copy(gabi::ea(actor) + 0x3EC, gabi::ea(&target->value));
    state = 4;
    *member<be<s16>>(actor, 0x408) = 20;
    [[fallthrough]];
  }
  case 4: {
    s32 frame =
        gabi::ftoi(*at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x9C));
    if (frame == 10) {
      u32 play = guest_call<u32>(0x025200D4);
      u32 emitter = guest_call<u32>(
          0x025A847C, at<void>(word(play + 0x5AB0)), 0, 0x8179, &actor->eyePos,
          (void *)nullptr, (void *)nullptr, 0xFF, (void *)nullptr, -1,
          (void *)nullptr, (void *)nullptr, (void *)nullptr);
      if (emitter) {
        u32 model = word(gabi::ea((void *)actor->bodyAnimation) + 0x90);
        u32 matrixData = word(model + 0x2C);
        u16 flags = *at<be<u16>>(matrixData, 4);
        u32 matrix = word(matrixData + 0x10);
        *at<be<u16>>(matrixData, 4) = flags | 0x10;
        guest_call(0x028249B0, at<void>(matrix + 0x2D0),
                   at<void>(emitter + 0x1F0), at<void>(emitter + 0x22C));
      }
    }
    *member<be<s16>>(actor, 0x400) = 0x1000;
    *member<be<f32>>(actor, 0x3F8) = *at<be<f32>>(0x1047BA94) + 10.0f;
    if (!*member<be<s16>>(actor, 0x408))
      state = 3;
    if (player_distance(actor) > 530.0f) {
      anm_init(actor, 0x31, 5.0f, 2, 1.0f, -1);
      state = 5;
      *member<be<s16>>(actor, 0x408) = 10;
    }
    break;
  }
  case 5:
    *member<be<f32>>(actor, 0x3F8) = 0.0f;
    *member<be<s16>>(actor, 0x400) = 0x2000;
    raw_vector_copy(gabi::ea(actor) + 0x3EC, player + 0x314);
    if (!*member<be<s16>>(actor, 0x408) && player_distance(actor) < 480.0f)
      state = -1;
    break;
  }
  if (*member<be<f32>>(actor, 0x4FC) == -1000000000.0f) {
    u32 x = word(gabi::ea(actor) + 0x2EC), y = word(gabi::ea(actor) + 0x2F0),
        z = word(gabi::ea(actor) + 0x2F4);
    *member<be<u32>>(actor, 0x314) = x;
    *member<be<u32>>(actor, 0x318) = y;
    *member<be<u32>>(actor, 0x31C) = z;
    *member<be<u32>>(actor, 0x300) = x;
    *member<be<u32>>(actor, 0x304) = y;
    *member<be<u32>>(actor, 0x308) = z;
  }
}
VERIFY(0x02496AA0, ue_move);
void sita_move(st_class *actor) {
  WWHD_FUNC(0x02496F34, void, actor);
  u32 player = player_pointer();
  f32 y = actor->current.pos.y + 70.0f, z = actor->current.pos.z;
  actor->eyePos.z = z;
  actor->eyePos.y = y;
  *member<be<f32>>(actor, 0x394) = y;
  f32 x = actor->current.pos.x;
  actor->eyePos.x = x;
  *member<be<f32>>(actor, 0x390) = x;
  *member<be<u32>>(actor, 0x39C) = 4;
  actor->actor_status = (u32)actor->actor_status | 0x20;
  *member<be<f32>>(actor, 0x398) = z;
  auto &state = *member<be<s16>>(actor, 0x3E0);
  switch ((s16)state) {
  case 0: {
    state = 1;
    *member<be<s16>>(actor, 0x408) = 60;
    f64 random = guest_call<f64>(0x020198D8, 200.0f);
    *member<be<s16>>(actor, 0x40A) = (s16)gabi::ftoi((f32)(random + 350.0f));
    [[fallthrough]];
  }
  case 1:
    anm_init(actor, 0x2A, 5.0f, 2, 1.2f, -1);
    [[fallthrough]];
  case 2:
    state = 3;
    [[fallthrough]];
  case 3:
    *member<be<s16>>(actor, 0x400) = 0x800;
    *member<be<f32>>(actor, 0x3F8) = *at<be<f32>>(0x1047BA94) + 10.0f;
    raw_vector_copy(gabi::ea(actor) + 0x3EC, player + 0x314);
    if (!*member<be<s16>>(actor, 0x408) && player_distance(actor) < 260.0f)
      state = 5;
    break;
  case 5:
    raw_vector_copy(gabi::ea(actor) + 0x3EC, player + 0x314);
    *member<be<s16>>(actor, 0x400) = 0x2000;
    anm_init(actor, 0x1E, 3.0f, 0, 1.0f, -1);
    state = 6;
    break;
  case 6:
    *member<be<f32>>(actor, 0x3F8) = *at<be<f32>>(0x1047BA94) + 5.0f;
    if (animation_stopped(actor)) {
      anm_init(actor, 0x1F, 1.0f, 0, 1.0f, -1);
      state = 7;
    }
    break;
  case 7:
    if (gabi::ftoi(*at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x9C)) <
        10)
      *member<be<s8>>(actor, 0xFED) = 3;
    *member<be<f32>>(actor, 0x3F8) = *at<be<f32>>(0x1047BA94);
    if (animation_stopped(actor)) {
      state = 1;
      *member<be<s16>>(actor, 0x408) = 20;
    }
    break;
  }
  gabi::Local<IDLocal> upperID;
  upperID->id = *member<be<u32>>(actor, 0x201C);
  u32 upper = 0;
  if ((u32)upperID->id != 0xFFFFFFFF)
    upper = guest_call<u32>(0x025D5218, at<void>(0x025E1234), &upperID->id);
  if (upper) {
    s16 timer = *member<be<s16>>(actor, 0x40A);
    if (timer < 5)
      *member<be<s8>>(actor, 0xFEC) = 2;
    *member<be<s8>>(actor, 0x20CE) = 1;
    if (!timer) {
      *member<be<s8>>(actor, 0xFEF) = 11;
      *member<be<s16>>(actor, 0x3E2) = 30;
      state = 10;
      *member<be<s16>>(actor, 0x414) = 0;
      guest_call(0x025D57E0, at<void>(upper));
      *member<be<s8>>(actor, 0x20CE) = 0;
    }
  }
}
VERIFY(0x02496F34, sita_move);

struct RopeChain {
  cXyz position[10], velocity[10];
  csXyz angle;
  u8 padF6[0x36];
};
WWHD_SIZE(RopeChain, 0x12C);
struct GroundCheckLocal {
  be<u32> linkage[2];
  u8 check[0x54];
};
void ke_control(st_class *actor) {
  WWHD_FUNC(0x02491458, void, actor);
  gabi::Local<VectorLocal> direction, bias, rotated;
  for (u32 chainIndex = 0; chainIndex < 3; ++chainIndex) {
    auto *chain = member<RopeChain>(actor, 0x1AE8 + chainIndex * 0x12C);
    f32 floor = -1000000000.0f;
    if (*member<be<s8>>(actor, 0x2020)) {
      gabi::Local<GroundCheckLocal> ground;
      u32 check = gabi::ea(&ground->check);
      guest_call(0x02008E0C, at<void>(check));
      for (u32 i = 0; i < 7; ++i)
        *at<be<u8>>(check, 0x44 + i) = 0;
      *at<be<u32>>(check, 0x4C) = 0x1003D52C;
      *at<be<u32>>(check, 0x40) = 0x1003D53C;
      *at<be<u32>>(check, 0x50) = 1;
      *at<be<u32>>(check, 0x20) = 0x1003D51C;
      *at<be<u32>>(check, 4) = check + 0x4C;
      *at<be<u32>>(check, 0x10) = 0x1003D50C;
      *at<be<u32>>(check) = check + 0x40;
      *at<be<f32>>(check, 0x2C) = chain->position[1].z;
      *at<be<f32>>(check, 0x24) = chain->position[1].x;
      *at<be<f32>>(check, 0x28) = (f32)chain->position[1].y + 50.0f;
      u32 play = guest_call<u32>(0x025200D4);
      f64 height =
          guest_call<f64>(0x02008974, at<void>(play + 0x12A0), at<void>(check));
      floor = (f32)(height + 3.0f);
      f32 currentY = chain->position[1].y;
      if (floor - currentY > 50.0f)
        floor = currentY;
      *at<be<u32>>(check, 0x20) = 0x1003D51C;
      *at<be<u32>>(check, 0x40) = 0x1003D53C;
      *at<be<u32>>(check, 0x4C) = 0x1003D4FC;
      guest_call(0x02008DAC, at<void>(check), 0);
    }
    guest_call(0x025F1884, at<void>(word(0x1018C7B0)), (s16)chain->angle.y);
    guest_call(0x025F1BF4, at<void>(word(0x1018C7B0)), (s16)chain->angle.x);
    direction->value.x = 0.0f;
    direction->value.y = 30.0f;
    direction->value.z = -20.0f;
    guest_call(0x0200FCD8, &direction->value, &bias->value);
    direction->value.x = 0.0f;
    direction->value.y = 0.0f;
    direction->value.z = 17.5f;
    f32 damping = gabi::fmadds((f32)chainIndex, 0.001f, 0.75f);
    for (u32 i = 1; i < 10; ++i) {
      f32 bx = 0.0f, by = 0.0f, bz = 0.0f;
      if (i <= 5) {
        f32 weight = (f32)(5 - (s32)i);
        bz = ((f32)bias->value.z * weight) * 0.1f;
        by = ((f32)bias->value.y * weight) * 0.1f;
        bx = ((f32)bias->value.x * weight) * 0.1f;
      }
      auto &position = chain->position[i];
      auto &previous = chain->position[i - 1];
      auto &velocity = chain->velocity[i];
      f32 x = (((f32)position.x - (f32)previous.x) + (f32)velocity.x) + bx;
      f32 y = (((f32)position.y + (f32)velocity.y) + by) - 10.0f;
      y = y + *at<be<f32>>(0x1047B618);
      f32 z = (((f32)position.z - (f32)previous.z) + (f32)velocity.z) + bz;
      if (y < floor)
        y = floor;
      y = y - (f32)previous.y;
      s16 angleX = (s16)-guest_call<s32>(0x020195B0, y, z);
      f32 squared = gabi::fmadds(y, y, z * z);
      f64 length = guest_call<f64>(0x028F4384, squared);
      s32 angleY = guest_call<s32>(0x020195B0, x, length);
      guest_call(0x025F18EC, at<void>(word(0x1018C7B0)), angleX);
      guest_call(0x025F1C28, at<void>(word(0x1018C7B0)), angleY);
      guest_call(0x0200FCD8, &direction->value, &rotated->value);
      // Keep the interleaved raw-word copy order of the actor's chain update.
      for (u32 component = 0; component < 3; ++component)
        *at<be<u32>>(gabi::ea(&velocity), component * 4) =
            word(gabi::ea(&position) + component * 4);
      f32 newX = (f32)previous.x + (f32)rotated->value.x;
      position.x = newX;
      position.y = (f32)previous.y + (f32)rotated->value.y;
      position.z = (f32)previous.z + (f32)rotated->value.z;
      velocity.x = (newX - (f32)velocity.x) * damping;
      velocity.y = ((f32)position.y - (f32)velocity.y) * damping;
      velocity.z = ((f32)position.z - (f32)velocity.z) * damping;
    }
    u32 lineArray = word(gabi::ea(actor) + 0x1FB0);
    u32 output = word(lineArray + chainIndex * 0x10);
    for (u32 point = 0; point < 10; ++point)
      for (u32 component = 0; component < 3; ++component)
        *at<be<u32>>(output + point * 12, component * 4) =
            word(gabi::ea(&chain->position[point]) + component * 4);
  }
}
VERIFY(0x02491458, ke_control);

struct MaceChain {
  cXyz position[3];
  csXyz angle[3];
  u8 pad36[2];
  cXyz velocity[3];
};
WWHD_SIZE(MaceChain, 0x5C);
void copy_model_matrix(u32 model, u32 matrix) {
  f32 values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = *at<be<f32>>(matrix, i * 4);
  for (u32 i = 0; i < 12; ++i)
    *at<be<f32>>(model, 0xC8 + i * 4) = values[i];
}
void nun_control(st_class *actor) {
  WWHD_FUNC(0x0249196C, void, actor);
  gabi::Local<VectorLocal> direction, bias, rotated;
  auto *chain = member<MaceChain>(actor, 0x1FB4);
  u32 modelData =
      word(word(gabi::ea((void *)actor->bodyAnimation) + 0x90) + 0x2C);
  u16 flags = *at<be<u16>>(modelData, 4);
  u32 matrices = word(modelData + 0x10);
  *at<be<u16>>(modelData, 4) = flags | 0x10;
  guest_call(0x028E90D4, at<void>(matrices + 0x210),
             at<void>(word(0x1018C7B0)));
  f32 tx = *at<be<f32>>(0x1047BAA4) + 10.0f, ty = *at<be<f32>>(0x1047BAA8),
      tz = *at<be<f32>>(0x1047BAAC);
  guest_call(0x0200FAD8, 1, tx, ty, tz);
  guest_call(0x025F1C28, at<void>(word(0x1018C7B0)),
             (s16)*at<be<s16>>(0x1047B692));
  guest_call(0x025F1C5C, at<void>(word(0x1018C7B0)),
             (s16)*at<be<s16>>(0x1047B694));
  f32 horizontal = *member<be<f32>>(actor, 0x2394),
      vertical = *member<be<f32>>(actor, 0x2398);
  guest_call(0x0200FC74, 1, horizontal, vertical, horizontal);
  u32 matrix = word(0x1018C7B0),
      model = gabi::ea((void *)actor->parts[4].model);
  copy_model_matrix(model, matrix);
  tx = *at<be<f32>>(0x1047BAB0) + 60.0f;
  ty = *at<be<f32>>(0x1047BAB4);
  tz = *at<be<f32>>(0x1047BAB8);
  guest_call(0x0200FAD8, 1, tx, ty, tz);
  guest_call(0x025F1C28, at<void>(word(0x1018C7B0)),
             (s16)*at<be<s16>>(0x1047BB16));
  guest_call(0x025F1C5C, at<void>(word(0x1018C7B0)),
             (s16)*at<be<s16>>(0x1047BB18));
  direction->value.z = *at<be<f32>>(0x1047B640);
  direction->value.y = *at<be<f32>>(0x1047B63C);
  direction->value.x = *at<be<f32>>(0x1047B638);
  guest_call(0x0200FCD8, &direction->value, &chain->position[1]);
  if (*member<be<s8>>(actor, 0x2010) == 2) {
    bias->value.x = 0.0f;
    bias->value.y = 0.0f;
    bias->value.z = 0.0f;
    s16 angle =
        (s16)((s16)actor->shape_angle.y - 8000 + *at<be<s16>>(0x1047BB0E));
    guest_call(0x025F1884, at<void>(word(0x1018C7B0)), angle);
    guest_call(0x025F1BF4, at<void>(word(0x1018C7B0)),
               (s16)*at<be<s16>>(0x1047BB10));
    s32 count = *member<be<u32>>(actor, 0x404);
    angle = (s16)((u32)count * (u32)(*at<be<s16>>(0x1047BB12) + 0x1800));
    guest_call(0x025F1C5C, at<void>(word(0x1018C7B0)), angle);
    direction->value.y = 0.0f;
    direction->value.z = 0.0f;
    direction->value.x = *at<be<f32>>(0x1047BA94) + 200.0f;
    guest_call(0x0200FCD8, &direction->value, &bias->value);
    if (!(*member<be<u32>>(actor, 0x404) & 7))
      sound_at(actor, 0x5898, &actor->eyePos);
  } else {
    bias->value.x = 0.0f;
    bias->value.y = 0.0f;
    bias->value.z = 0.0f;
  }
  f32 damping = *at<be<f32>>(0x1047B978) + 1.3f;
  direction->value.x = 0.0f;
  direction->value.y = 0.0f;
  direction->value.z = *at<be<f32>>(0x1047B980) + 30.0f;
  s8 state = *member<be<s8>>(actor, 0x2010);
  if (state == 2)
    damping = 0.3f;
  else if (state == 3)
    damping = *at<be<f32>>(0x1047BAC8) + 2.5f;
  auto &position = chain->position[2];
  auto &previous = chain->position[1];
  auto &velocity = chain->velocity[2];
  f32 y = (((f32)position.y - (f32)previous.y) + (f32)velocity.y) - 20.0f;
  y = y + (f32)bias->value.y;
  f32 z = (((f32)position.z - (f32)previous.z) + (f32)velocity.z) +
          (f32)bias->value.z;
  f32 x = (((f32)position.x - (f32)previous.x) + (f32)velocity.x) +
          (f32)bias->value.x;
  s16 angleX = (s16)-guest_call<s32>(0x020195B0, y, z);
  f64 length = guest_call<f64>(0x028F4384, gabi::fmadds(y, y, z * z));
  s32 angleY = guest_call<s32>(0x020195B0, x, length);
  guest_call(0x025F18EC, at<void>(word(0x1018C7B0)), angleX);
  guest_call(0x025F1C28, at<void>(word(0x1018C7B0)), angleY);
  guest_call(0x0200FCD8, &direction->value, &rotated->value);
  chain->angle[1].y = angleY;
  chain->angle[1].x = angleX;
  guest_call(0x0200FCD8, &direction->value, &rotated->value);
  for (u32 i = 0; i < 3; ++i)
    *at<be<u32>>(gabi::ea(&velocity), i * 4) =
        word(gabi::ea(&position) + i * 4);
  f32 newX = (f32)previous.x + (f32)rotated->value.x;
  position.x = newX;
  position.y = (f32)previous.y + (f32)rotated->value.y;
  position.z = (f32)previous.z + (f32)rotated->value.z;
  velocity.x = (newX - (f32)velocity.x) * damping;
  velocity.y = ((f32)position.y - (f32)velocity.y) * damping;
  velocity.z = ((f32)position.z - (f32)velocity.z) * damping;
  guest_call(0x0200FAD8, 0, (f32)chain->position[1].x,
             (f32)chain->position[1].y, (f32)chain->position[1].z);
  guest_call(0x025F1BF4, at<void>(word(0x1018C7B0)), (s16)chain->angle[1].x);
  guest_call(0x025F1C28, at<void>(word(0x1018C7B0)), (s16)chain->angle[1].y);
  s16 angle = (s16)(*at<be<s16>>(0x1047BB16) - 0x4000);
  guest_call(0x025F1C28, at<void>(word(0x1018C7B0)), angle);
  tx = *at<be<f32>>(0x1047B998) + 80.0f;
  ty = *at<be<f32>>(0x1047B99C);
  tz = *at<be<f32>>(0x1047B9A0);
  guest_call(0x0200FAD8, 1, tx, ty, tz);
  guest_call(0x025F1C28, at<void>(word(0x1018C7B0)), (s16)-0x8000);
  horizontal = *member<be<f32>>(actor, 0x2394);
  vertical = *member<be<f32>>(actor, 0x2398);
  guest_call(0x0200FC74, 1, horizontal, vertical, horizontal);
  matrix = word(0x1018C7B0);
  model = gabi::ea((void *)actor->parts[3].model);
  copy_model_matrix(model, matrix);
}
VERIFY(0x0249196C, nun_control);

u32 find_actor(u32 id) {
  gabi::Local<IDLocal> local;
  local->id = id;
  return id == 0xFFFFFFFF
             ? 0
             : guest_call<u32>(0x025D5218, at<void>(0x025E1234), &local->id);
}
f64 random_positive(f32 range) { return guest_call<f64>(0x020198D8, range); }
f64 random_signed(f32 range) { return guest_call<f64>(0x02019918, range); }
f32 angle_sine(s32 angle) {
  return *at<be<f32>>(0x104A44F8, ((u16)angle >> 3) * 8);
}
f32 angle_cosine(s32 angle) {
  return *at<be<f32>>(0x104A44FC, ((u16)angle >> 3) * 8);
}
f64 magnitude(cXyz *vector) {
  f64 squared = guest_call<f64>(0x028E8DD0, vector);
  return guest_call<f64>(0x028F4384, squared);
}
u32 animation_matrix(u32 animation, s32 joint) {
  u32 model = word(animation + 0x90), data = word(model + 0x2C);
  u16 flags = *at<be<u16>>(data, 4);
  u32 matrices = word(data + 0x10);
  *at<be<u16>>(data, 4) = flags | 0x10;
  return matrices + (u32)joint * 0x30;
}
void joint_matrix(st_class *actor, s32 joint) {
  u32 matrix = animation_matrix(gabi::ea((void *)actor->bodyAnimation), joint);
  guest_call(0x028E90D4, at<void>(matrix), at<void>(word(0x1018C7B0)));
}
u32 particle(u32 id, cXyz *position) {
  u32 play = guest_call<u32>(0x025200D4);
  return guest_call<u32>(0x025A847C, at<void>(word(play + 0x5AB0)), 0, id,
                         position, (void *)nullptr, (void *)nullptr, 0xFF,
                         (void *)nullptr, -1, (void *)nullptr, (void *)nullptr,
                         (void *)nullptr);
}
void scatter_part(st_p *part) {
  part->mode = 1;
  part->velocity.x = random_signed(20.0f);
  part->velocity.y = (f32)(random_positive(20.0f) + 40.0f);
  part->velocity.z = random_signed(20.0f);
}
void part_move(st_class *actor, s32 joint) {
  WWHD_FUNC(0x024925B8, void, actor, joint);
  guest_call<u32>(0x025200D4);
  auto *part = member<st_p>(actor, 0xFF0 + (u32)joint * 0x6C);
  gabi::Local<VectorLocal> direction, target, difference;
  direction->value.x = 0.0f;
  direction->value.y = 0.0f;
  direction->value.z = 0.0f;
  u32 weapon = joint == 11 ? find_actor(*member<be<u32>>(actor, 0x2014)) : 0;
  ++part->timer3E;
  if (part->wait)
    --part->wait;
  if (!*member<be<s8>>(actor, 0x21F6)) {
    switch ((s8)part->state) {
    case 0:
      joint_matrix(actor, joint);
      guest_call(0x0200FCD8, &direction->value, &part->pos);
      if (*member<be<s8>>(actor, 0xFEF) == 1) {
        scatter_part(part);
        if (*member<be<s8>>(actor, 0xFEE) && joint != 11)
          --*member<be<s8>>(actor, 0xFEE);
        if (joint == 15) {
          *member<be<s8>>(actor, 0x2020) = 1;
          part->state = 6;
          part->angularSpeed.x = 0;
          part->angularSpeed.y = 0;
          part->angularSpeed.z = 0;
          *member<be<s16>>(actor, 0x2022) = part->angle.y;
          head_anm_init(actor, 0x11, 1.0f, 0, 1.0f, -1);
        } else {
          part->state = 5;
          part->angularSpeed.x = (s16)gabi::ftoi(random_signed(6000.0f));
          part->angularSpeed.y = (s16)gabi::ftoi(random_signed(6000.0f));
        }
      }
      if (*member<be<s8>>(actor, 0xFEF) == 10) {
        if (joint < 19) {
          part->state = 10;
          if (joint == 11) {
            part->state = 5;
            scatter_part(part);
            part->angularSpeed.x = (s16)gabi::ftoi(random_signed(6000.0f));
            part->angularSpeed.y = (s16)gabi::ftoi(random_signed(6000.0f));
          }
          if (*member<be<s8>>(actor, 0xFEE) && joint != 11)
            --*member<be<s8>>(actor, 0xFEE);
        }
      } else if (*member<be<s8>>(actor, 0xFEF) == 20 && joint >= 19) {
        part->state = 11;
        if (*member<be<s8>>(actor, 0xFEE) && joint != 11)
          --*member<be<s8>>(actor, 0xFEE);
      }
      break;
    case 1:
      if (part->wait) {
        part->angle.y =
            (s16)((s16)part->angle.y + ((part->wait & 4) ? 0x100 : -0x100));
      } else {
        joint_matrix(actor, joint);
        guest_call(0x0200FCD8, &direction->value, &target->value);
        if (joint == 15)
          target->value.y =
              (f32)target->value.y - (*at<be<f32>>(0x1047B9A4) + 20.0f);
        guest_call(0x0201ADE0, &target->value, &difference->value, &part->pos);
        raw_vector_copy(gabi::ea(&direction->value),
                        gabi::ea(&difference->value));
        f64 distance = magnitude(&direction->value);
        if (distance < *at<be<f32>>(0x1047BD6C) + 60.0f)
          part->state = 2;
        else {
          f32 amplitude =
              (f32)(distance - 50.0f) * (*at<be<f32>>(0x1047BAB0) + 1.0f);
          if (amplitude > *at<be<f32>>(0x1047BAB4) + 300.0f)
            amplitude = *at<be<f32>>(0x1047BAB8) + 300.0f;
          s16 count = part->timer3E;
          direction->value.x = gabi::fmadds(amplitude, angle_sine(count * 1000),
                                            direction->value.x);
          f32 oscillation = (amplitude * angle_cosine(count * 1100)) * 0.7f;
          f32 vertical = gabi::fmadds(amplitude, 0.5f, oscillation);
          direction->value.y = (f32)direction->value.y + vertical;
          direction->value.z = gabi::fmadds(
              amplitude, angle_cosine(count * 1200), direction->value.z);
          s16 angle = guest_call<s16>(0x020195B0, (f32)direction->value.x,
                                      (f32)direction->value.z);
          guest_call(0x0200F428, &part->timer3A, angle, 4, 0x2000);
          f32 x = direction->value.x, z = direction->value.z;
          f64 horizontal =
              guest_call<f64>(0x028F4384, gabi::fmadds(x, x, z * z));
          s16 elevation = (s16)-guest_call<s32>(
              0x020195B0, (f32)direction->value.y, horizontal);
          guest_call(0x0200F428, &part->timer38, elevation, 4, 0x2000);
          guest_call(0x025F1884, at<void>(word(0x1018C7B0)),
                     (s16)part->timer3A);
          guest_call(0x025F1BF4, at<void>(word(0x1018C7B0)),
                     (s16)part->timer38);
          direction->value.x = 0.0f;
          direction->value.y = 0.0f;
          direction->value.z = (f32)part->factor * 20.0f;
          guest_call(0x0200FCD8, &direction->value, &part->velocity);
          guest_call(0x028E8D88, &part->pos, &part->velocity, &part->pos);
          if (joint == 15) {
            part->angle.x = part->timer38;
            part->angle.y = part->timer3A;
            part->angle.z = part->timer3C;
          } else
            guest_call(0x0201A554, &part->angle, &part->angularSpeed);
          guest_call(0x0200ED84, &part->factor, 1.0f, 1.0f, 0.01f);
        }
      }
      break;
    case 2: {
      joint_matrix(actor, joint);
      guest_call(0x0200FCD8, &direction->value, &target->value);
      if (joint == 15)
        target->value.y =
            (f32)target->value.y - (*at<be<f32>>(0x1047B9A4) + 20.0f);
      guest_call(0x0200ED84, &part->pos.x, (f32)target->value.x, 1.0f,
                 std::fabs((f32)part->velocity.x) + 5.0f);
      guest_call(0x0200ED84, &part->pos.y, (f32)target->value.y, 1.0f,
                 std::fabs((f32)part->velocity.y) + 5.0f);
      guest_call(0x0200ED84, &part->pos.z, (f32)target->value.z, 1.0f,
                 std::fabs((f32)part->velocity.z) + 5.0f);
      guest_call(0x0201ADE0, &part->pos, &difference->value, &target->value);
      raw_vector_copy(gabi::ea(&direction->value),
                      gabi::ea(&difference->value));
      guest_call(0x0201A554, &part->angle, &part->angularSpeed);
      if (!(magnitude(&direction->value) > 5.0f)) {
        part->state = 0;
        part->mode = 0;
        if (*member<be<s8>>(actor, 0xFEE) < 25 && joint != 11)
          ++*member<be<s8>>(actor, 0xFEE);
        if (joint == 11)
          *member<be<s8>>(actor, 0x2018) = 10;
        if (joint == 15)
          *member<be<s8>>(actor, 0x2020) = 0;
      }
      break;
    }
    case 5: {
      f32 floor = part_posmove(actor, part) + 10.0f;
      if (std::fabs((f32)actor->home.pos.y - (f32)part->pos.y) > 2000.0f) {
        floor = actor->current.pos.y;
        raw_vector_copy(gabi::ea(&part->pos), gabi::ea(&actor->current.pos));
      }
      if (!(part->pos.y > floor)) {
        part->pos.y = floor;
        if (joint == 11 && weapon && *member<be<s8>>(actor, 0x2018) >= 2) {
          guest_call(0x025D9D24, at<void>(weapon));
          *member<be<s8>>(actor, 0x2018) = 0;
        }
        if (part->velocity.y < -40.0f) {
          part->velocity.y = (f32)(random_positive(10.0f) + 20.0f);
          f64 random = random_signed(0.3f);
          part->velocity.x = (f32)((f32)part->velocity.x * random);
          random = random_signed(0.3f);
          part->velocity.z = (f32)((f32)part->velocity.z * random);
        } else {
          part->velocity.x = 0.0f;
          part->velocity.y = 0.0f;
          part->velocity.z = 0.0f;
          part->angularSpeed.x = 0;
          part->angularSpeed.y = 0;
          part->angularSpeed.z = 0;
          guest_call(0x0200F428, &part->angle.x, (s16)-0x8000, 1, 0xC00);
          guest_call(0x0200F428, &part->angle.y, 0, 1, 0xC00);
        }
      }
      s8 command = *member<be<s8>>(actor, 0xFEF);
      if (command == 2 || command == 11) {
        part->timer3E = (s16)gabi::ftoi(random_positive(65536.0f));
        part->factor = 0.0f;
        if (joint == 11) {
          if (weapon && !(word(weapon + 0x2E0) & 0x2000)) {
            guest_call(0x025D9D0C, at<void>(weapon), 0);
            raw_vector_copy(gabi::ea(&part->pos), weapon + 0x314);
            *member<be<s8>>(actor, 0x2018) = 2;
            part->wait = 0;
            part->angularSpeed.y = 0x800;
            part->angularSpeed.x = 0;
            part->state = 1;
            if (*member<be<s8>>(actor, 0x2010)) {
              *member<be<s16>>(actor, 0x3E2) = 29;
              *member<be<s16>>(actor, 0x3E0) = 50;
            } else {
              *member<be<s16>>(actor, 0x3E2) = 30;
              *member<be<s16>>(actor, 0x3E0) = 3;
              *member<be<s16>>(actor, 0x414) = 500;
              anm_init(actor, 0xE, 10.0f, 2, 1.0f, -1);
            }
          }
        } else {
          part->state = 1;
          part->angularSpeed.x = (s16)gabi::ftoi(random_signed(4000.0f));
          part->angularSpeed.y = (s16)gabi::ftoi(random_signed(4000.0f));
          part->wait = (s8)gabi::ftoi((f32)(random_positive(50.0f) + 30.0f));
        }
      }
      break;
    }
    case 6: {
      if (!*member<be<s16>>(actor, 0x2024) && !(part->velocity.y > 0.0f)) {
        part->timer3E = (s16)gabi::ftoi(random_positive(65536.0f));
        part->factor = 0.0f;
        part->state = 1;
        part->wait = 0;
        break;
      }
      f32 floor = part_posmove(actor, part);
      if (std::fabs((f32)actor->home.pos.y - (f32)part->pos.y) > 2000.0f) {
        floor = actor->current.pos.y;
        raw_vector_copy(gabi::ea(&part->pos), gabi::ea(&actor->current.pos));
        part->velocity.y = 0.0f;
      }
      if (!(part->pos.y > floor)) {
        part->pos.y = floor;
        if (part->velocity.y < -10.0f) {
          part->wait = (s8)gabi::ftoi((f32)(random_positive(10.0f) + 2.0f));
          head_anm_init(actor, 0x12, 1.0f, 0, 1.0f, -1);
          sound_at(actor, 0x5896, &actor->eyePos);
        }
        if (part->wait) {
          part->velocity.x = 0.0f;
          part->velocity.y = 0.0f;
          part->velocity.z = 0.0f;
          if (part->wait == 1) {
            guest_call(0x0201ADE0, &actor->current.pos, &difference->value,
                       &part->pos);
            raw_vector_copy(gabi::ea(&direction->value),
                            gabi::ea(&difference->value));
            s16 angle = guest_call<s16>(0x020195B0, (f32)direction->value.x,
                                        (f32)direction->value.z);
            angle = (s16)(angle + (s16)gabi::ftoi(random_signed(20000.0f)));
            *member<be<s16>>(actor, 0x2022) = angle;
            guest_call(0x025F1884, at<void>(word(0x1018C7B0)), angle);
            direction->value.x = 0.0f;
            f64 random = random_positive(35.0f);
            direction->value.y =
                (f32)((f32)(random + 20.0f) + *at<be<f32>>(0x1047BD60));
            direction->value.z = *at<be<f32>>(0x1047BD64) + 20.0f;
            guest_call(0x0200FCD8, &direction->value, &part->velocity);
            head_anm_init(actor, 0x11, 1.0f, 0, 1.0f, -1);
            u32 actorID = word(gabi::ea(actor) + 4);
            s32 reverb = guest_call<s32>(0x02520540, (s8)actor->current.roomNo);
            guest_call(0x025E1AA4, 0x487D, &actor->eyePos, actorID, 0, reverb);
          }
        }
        guest_call(0x0200F428, &part->angle.x, 0, 2, 0xC00);
        guest_call(0x0200F428, &part->angle.y,
                   (s16)*member<be<s16>>(actor, 0x2022), 2, 0xC00);
      } else {
        s32 frame = gabi::ftoi(
            *at<be<f32>>(gabi::ea((void *)actor->headAnimation), 0x9C));
        if (frame == 5) {
          u32 emitter = particle(0x8179, &part->pos);
          if (emitter) {
            u32 matrix =
                animation_matrix(gabi::ea((void *)actor->headAnimation), 1);
            guest_call(0x028249B0, at<void>(matrix), at<void>(emitter + 0x1F0),
                       at<void>(emitter + 0x22C));
          }
        }
      }
      break;
    }
    case 7: {
      if (!*member<be<s16>>(actor, 0x2024) && !(part->velocity.y > 0.0f)) {
        part->timer3E = (s16)gabi::ftoi(random_positive(65536.0f));
        part->factor = 0.0f;
        part->state = 1;
        part->wait = 0;
      } else {
        f32 floor = part_posmove(actor, part);
        if (std::fabs((f32)actor->home.pos.y - (f32)part->pos.y) > 2000.0f) {
          floor = actor->current.pos.y;
          raw_vector_copy(gabi::ea(&part->pos), gabi::ea(&actor->current.pos));
          part->velocity.y = 0.0f;
        }
        if (!(part->pos.y > floor)) {
          part->pos.y = floor;
          if (animation_stopped(actor) || part->velocity.y < -20.0f)
            head_anm_init(actor, 0x13, 1.0f, 2, 1.0f, -1);
          part->velocity.x = 0.0f;
          part->velocity.y = 0.0f;
          part->velocity.z = 0.0f;
        }
        if (!part->wait)
          part->state = 6;
      }
      break;
    }
    case 8: {
      *member<be<s16>>(actor, 0x412) = 10;
      f32 floor = part_posmove(actor, part) + 20.0f;
      if (std::fabs((f32)actor->home.pos.y - (f32)part->pos.y) > 3000.0f)
        floor = part->pos.y;
      if (!(!(part->pos.y > floor)))
        break;
      part->pos.y = floor;
      for (u32 i = 0; i < 26; ++i) {
        if (i != 11 && gabi::ea((void *)actor->parts[i].model)) {
          if (i == 15)
            guest_call(0x025D99E8, actor, &actor->parts[i].pos, 5, 0, 0xFF);
          else
            particle(0x817E, &actor->parts[i].pos);
        }
      }
      *member<be<s8>>(actor, 0x20CD) = 5;
      if (actor->deathSwitch)
        guest_call(0x025B9E38, at<void>(word(0x101F84DC) + 0x20),
                   (u8)actor->deathSwitch, (s8)actor->current.roomNo);
      guest_call(0x025BA5D4, at<void>(word(0x101F84DC) + 0x20),
                 (u16)actor->setID, (s8)actor->home.roomNo);
      u32 held = find_actor(*member<be<u32>>(actor, 0x2014));
      if (held)
        guest_call(0x025D9D24, at<void>(held));
      return;
    }
    case 10:
      if (*member<be<s8>>(actor, 0xFEF) == 11) {
        u32 upper = find_actor(*member<be<u32>>(actor, 0x201C));
        if (upper) {
          part->state = 1;
          raw_vector_copy(gabi::ea(&part->pos),
                          upper + 0xFF8 + (u32)joint * 0x6C);
          if (joint != 15) {
            part->angularSpeed.x = (s16)gabi::ftoi(random_signed(4000.0f));
            part->angularSpeed.y = (s16)gabi::ftoi(random_signed(4000.0f));
          } else {
            gabi::Local<AngleLocal> zero;
            u32 result = guest_call<u32>(0x0201A478, &zero->value, 0, 0, 0);
            u16 x = *at<be<u16>>(result), y = *at<be<u16>>(result, 2),
                z = *at<be<u16>>(result, 4);
            part->angularSpeed.x = x;
            part->angularSpeed.y = y;
            part->angularSpeed.z = z;
            part->angle.x = x;
            part->angle.y = y;
            part->angle.z = z;
          }
          part->timer3E = (s16)gabi::ftoi(random_positive(65536.0f));
          part->wait = 0;
          part->factor = 0.5f;
          part->mode = 1;
        }
      }
      break;
    }
  }
  switch ((s8)part->mode) {
  case 0: {
    joint_matrix(actor, joint);
    s8 shake = *member<be<s8>>(actor, 0x202C);
    if (shake) {
      f32 amplitude = (f32)shake * (*at<be<f32>>(0x1047BAAC) + 2.0f);
      u32 count = *member<be<u32>>(actor, 0x404);
      f32 x = angle_sine(count * 0x3800 + (u32)joint * 0x3000) * amplitude;
      f32 z = angle_cosine(count * 0x3A00 + (u32)joint * 0x3100) * amplitude;
      f32 y = angle_cosine(count * 0x3C00 + (u32)joint * 0x2800) * amplitude;
      guest_call(0x0200FAD8, 1, x, y, z);
    }
    copy_model_matrix(gabi::ea((void *)part->model), word(0x1018C7B0));
    if (joint == 11 && weapon && *member<be<s8>>(actor, 0x2018)) {
      s8 state = *member<be<s8>>(actor, 0x2018);
      if (state == 1) {
        guest_call(0x025D9D0C, at<void>(weapon), 0);
        *member<be<s8>>(actor, 0x2018) = 10;
        return;
      } else if (state >= 2 && (word(weapon + 0x2E0) & 0x2000)) {
        guest_call(0x025F1C28, at<void>(word(0x1018C7B0)),
                   (s16)(*at<be<s16>>(0x1047BB0A) + 16000));
        guest_call(0x025F1BF4, at<void>(word(0x1018C7B0)),
                   (s16)*at<be<s16>>(0x1047BB0C));
        guest_call(0x025F1C5C, at<void>(word(0x1018C7B0)),
                   (s16)*at<be<s16>>(0x1047BB0E));
        guest_call(0x0200FAD8, 1, (f32)*at<be<f32>>(0x1047BAB4),
                   (f32)*at<be<f32>>(0x1047BAB8),
                   *at<be<f32>>(0x1047BABC) + 65.0f);
        if (actor->ambushState)
          guest_call(0x0200FAD8, 0, 20000.0f, 20000.0f, 20000.0f);
        u32 model = word(weapon + 0x3B4);
        if (model)
          copy_model_matrix(model, word(0x1018C7B0));
      }
    }
    if (joint == 15) {
      guest_call(0x0201A4DC, &actor->shape_angle, member<void>(actor, 0x418));
      // csXyz value return occupies r3/r4 in the guest ABI.
      u32 first = gabi::cpu->r[3], second = gabi::cpu->r[4];
      part->angle.x = first >> 16;
      part->angle.y = first;
      part->angle.z = second >> 16;
    }
    break;
  }
  case 1: {
    guest_call(0x0200FAD8, 0, (f32)part->pos.x, (f32)part->pos.y,
               (f32)part->pos.z);
    guest_call(0x025F1C28, at<void>(word(0x1018C7B0)), (s16)part->angle.y);
    guest_call(0x025F1BF4, at<void>(word(0x1018C7B0)), (s16)part->angle.x);
    f32 scaleX = *member<be<f32>>(actor, 0x2394),
        scaleY = *member<be<f32>>(actor, 0x2398);
    guest_call(0x0200FC74, 1, scaleX, scaleY, scaleX);
    if (joint == 15 && *member<be<s8>>(actor, 0x2020)) {
      auto &shakeTimer = *member<be<s16>>(actor, 0x20CA);
      if (shakeTimer) {
        --shakeTimer;
        f32 amplitude =
            (f32)(s16)shakeTimer * (*at<be<f32>>(0x1047BAA4) + 1.0f);
        u32 count = *member<be<u32>>(actor, 0x404);
        guest_call(0x0200FAD8, 1, amplitude * angle_sine(count * 0x3800), 0.0f,
                   amplitude * angle_cosine(count * 0x3C00));
      }
      u32 model = word(gabi::ea((void *)actor->headAnimation) + 0x90);
      copy_model_matrix(model, word(0x1018C7B0));
      *member<be<s8>>(actor, 0x2020) = 2;
      if (*member<be<s8>>(actor, 0x21F6)) {
        *member<be<f32>>(actor, 0x21F8) = *at<be<f32>>(0x1047BA9C) + 10.0f;
        raw_vector_copy(gabi::ea(&actor->current.pos), gabi::ea(&part->pos));
      }
    } else
      copy_model_matrix(gabi::ea((void *)part->model), word(0x1018C7B0));
    if (weapon && joint == 11 && *member<be<s8>>(actor, 0x2018) >= 2) {
      u32 model = word(weapon + 0x3B4);
      if (model)
        copy_model_matrix(model, word(0x1018C7B0));
    }
    break;
  }
  }
  if (joint == 15) {
    if (!*member<be<s8>>(actor, 0x20CE)) {
      raw_vector_copy(gabi::ea(&actor->eyePos), gabi::ea(&part->pos));
      *member<be<f32>>(actor, 0x390) = part->pos.x;
      *member<be<f32>>(actor, 0x394) = part->pos.y;
      *member<be<f32>>(actor, 0x398) = part->pos.z;
      *member<be<f32>>(actor, 0x394) =
          (f32)*member<be<f32>>(actor, 0x394) + 30.0f;
    }
    if (*member<be<s8>>(actor, 0x2020) == 2) {
      u32 matrix = animation_matrix(gabi::ea((void *)actor->headAnimation), 2);
      guest_call(0x028E90D4, at<void>(matrix), at<void>(word(0x1018C7B0)));
      direction->value.x = *at<be<f32>>(0x1047B620);
      direction->value.y = *at<be<f32>>(0x1047B624);
      direction->value.z = *at<be<f32>>(0x1047B628);
    } else {
      u32 model = gabi::ea((void *)part->model);
      guest_call(0x028E90D4, at<void>(model ? model + 0xC8 : 0),
                 at<void>(word(0x1018C7B0)));
      direction->value.x = *at<be<f32>>(0x1047B620) - 5.0f;
      direction->value.y = *at<be<f32>>(0x1047B624) - 25.0f;
      direction->value.z = *at<be<f32>>(0x1047B628);
    }
    for (u32 i = 0; i < 3; ++i) {
      auto *chain = member<RopeChain>(actor, 0x1AE8 + i * 0x12C);
      guest_call(0x0200FCD8, &direction->value, &chain->position[0]);
      for (u32 k = 0; k < 3; ++k)
        *at<be<u16>>(gabi::ea(&chain->angle), k * 2) =
            *at<be<u16>>(gabi::ea(&part->angle), k * 2);
      chain->angle.y = (s16)((s16)chain->angle.y + ((s32)i - 1) * 0x800);
      if (i == 1)
        chain->angle.x = (s16)((s16)chain->angle.x + 0x500);
    }
  }
}
VERIFY(0x024925B8, part_move);
void sound_at_level(st_class *actor, u32 sound, u32 level) {
  if (!actor || !gabi::ea(&actor->eyePos))
    return;
  s32 reverb = guest_call<s32>(0x02520540, (s8)actor->current.roomNo);
  guest_call(0x025E1A40, sound, &actor->eyePos, level, reverb);
}
void monster_sound(st_class *actor, u32 sound, u32 level) {
  if (!actor || !gabi::ea(&actor->eyePos))
    return;
  u32 id = word(gabi::ea(actor) + 4);
  s32 reverb = guest_call<s32>(0x02520540, (s8)actor->current.roomNo);
  guest_call(0x025E1AA4, sound, &actor->eyePos, id, level, reverb);
}
u32 particle_with_angle(u32 id, cXyz *position, csXyz *angle) {
  u32 play = guest_call<u32>(0x025200D4);
  return guest_call<u32>(0x025A847C, at<void>(word(play + 0x5AB0)), 0, id,
                         position, angle, (void *)nullptr, 0xFF,
                         (void *)nullptr, -1, (void *)nullptr, (void *)nullptr,
                         (void *)nullptr);
}
u32 toon_particle(st_class *actor, u32 id, cXyz *position, csXyz *angle,
                  void *callback) {
  s32 room = (s8)actor->current.roomNo;
  u32 play = guest_call<u32>(0x025200D4);
  return guest_call<u32>(0x025A847C, at<void>(word(play + 0x5AB0)), 2, id,
                         position, angle, (void *)nullptr, 0xB9, callback, room,
                         (void *)nullptr, (void *)nullptr, (void *)nullptr);
}
void buki_smoke_set(st_class *actor) {
  gabi::Local<VectorLocal> direction, center, start, end;
  guest_call(0x025A5F88, &actor->smoke[1]);
  joint_matrix(actor, 11);
  direction->value.x = *at<be<f32>>(0x1047B644) + 110.0f;
  direction->value.y = 0.0f;
  direction->value.z = 0.0f;
  guest_call(0x0200FCD8, &direction->value, &center->value);
  center->value.y =
      (*member<be<f32>>(actor, 0x4FC) + 30.0f) + *at<be<f32>>(0x1047B630);
  u32 emitter =
      toon_particle(actor, 0x2027, &center->value, nullptr, &actor->smoke[1]);
  if (emitter) {
    *at<be<u32>>(emitter, 0x5C) = 1;
    *at<be<f32>>(emitter, 0x34) = 20.0f;
    *at<be<f32>>(emitter, 0x68) = 20.0f;
    *at<be<f32>>(emitter, 8) = 1.0f;
    *at<be<f32>>(emitter, 0xC) = 0.0f;
    *at<be<f32>>(emitter, 0x10) = 1.0f;
  }
  gabi::Local<LineCheckLocal> line;
  initialize_line(line);
  raw_vector_copy(gabi::ea(&start->value), gabi::ea(&center->value));
  raw_vector_copy(gabi::ea(&end->value), gabi::ea(&center->value));
  start->value.y = (f32)start->value.y + 100.0f;
  end->value.y = (f32)end->value.y - 200.0f;
  if (line_cross(line, &start->value, &end->value, actor)) {
    u32 play = guest_call<u32>(0x025200D4);
    s32 material = guest_call<s32>(0x024EECAC, at<void>(play + 0x12A0),
                                   at<void>(gabi::ea(&line->check) + 0x14));
    s32 reverb = guest_call<s32>(0x02520540, (s8)actor->current.roomNo);
    guest_call(0x025E1A40, 0x2855, &actor->current.pos, material, reverb);
  }
  finish_line(line);
}
void jyunkai(st_class *actor) {
  u32 player = player_pointer();
  *member<be<u32>>(actor, 0x39C) = 4;
  actor->actor_status = (u32)actor->actor_status | 0x20;
  actor->targetSpeed = 0.0f;
  actor->speedStep = 5.0f;
  switch ((s16)actor->state) {
  case 0: {
    actor->smokeMode = 2;
    s32 frame =
        gabi::ftoi(*at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x9C));
    s16 start1 = *at<be<s16>>(0x1046DFF2), start2 = *at<be<s16>>(0x1046DFF4);
    if ((frame >= start1 && frame <= *at<be<s16>>(0x1046DFF6)) ||
        (frame >= start2 && frame <= *at<be<s16>>(0x1046DFF8))) {
      actor->targetSpeed = *at<be<f32>>(0x1046DFFC);
      actor->turnStep = 0x800;
      if (frame == start1 || frame == start2)
        sound_at_level(actor, 0x3832, 0);
    }
    raw_vector_copy(gabi::ea(&actor->playerPosition), player + 0x314);
    if (!actor->timers[0]) {
      wait_set(actor);
      actor->state = 1;
      actor->timers[0] = gabi::ftoi((f32)random_positive(60.0f) + 100.0f);
    }
    break;
  }
  case 1:
    if (!actor->timers[0]) {
      f32 speed = *at<be<f32>>(0x1047BA90) + 1.0f;
      if (actor->maceState) {
        anm_init(actor, 0x15, 10.0f, 2, 1.0f, -1);
        actor->maceState = 2;
        monster_sound(actor, 0x4882, 0);
      } else
        anm_init(actor, actor->attackSide ? 0x22 : 0x34, 10.0f, 2, speed, -1);
      actor->state = 0;
      actor->timers[0] = gabi::ftoi((f32)random_positive(60.0f) + 100.0f);
      actor->smokeMode = 1;
    }
    break;
  }
  u32 play = guest_call<u32>(0x025200D4);
  if (!*at<be<u8>>(play, 0x5292) && !actor->timers[1] &&
      player_distance(actor) < 280.0f) {
    if (actor->weaponState == 10) {
      actor->action = 5;
      actor->state = 0;
      actor->spinBlend = 0.0f;
      if (random_positive(1.0f) < 0.5f)
        actor->fightBehavior = actor->attackSide ? 5 : 0;
      else
        actor->fightBehavior = actor->attackSide ? 15 : 10;
    } else if (!actor->weaponState) {
      actor->action = 29;
      actor->state = actor->maceState ? 3 : 0;
    }
  }
}
bool ambush_trigger(st_class *actor) {
  if ((u8)actor->ambushState != 0xFF)
    return guest_call<s32>(0x025BA0C0, at<void>(word(0x101F84DC) + 0x20),
                           (s32)(u8)actor->ambushState - 1,
                           (s8)actor->current.roomNo) != 0;
  f32 range =
      actor->sightRange == 0xFF ? 500.0f : (f32)(u8)actor->sightRange * 10.0f;
  return player_distance(actor) < range;
}
void ground_wait(st_class *actor) {
  actor->collisionInvulnerability = 10;
  actor->collisionMode = 2;
  switch ((s16)actor->state) {
  case 0:
    *member<be<u32>>(actor, 0x39C) = 0;
    actor->actor_status = (u32)actor->actor_status & ~0x20u;
    if (ambush_trigger(actor))
      actor->state = 1;
    break;
  case 1:
    actor->ambushState = 0;
    sound_at_level(actor, 0x589A, 0);
    anm_init(actor, 0x2B, 1.0f, 0, 1.0f, -1);
    actor->state = 2;
    *member<be<u32>>(actor, 0x39C) = 4;
    actor->actor_status = (u32)actor->actor_status | 0x20;
    guest_call(0x025A5F88, &actor->smoke[0]);
    toon_particle(actor, 0xA17C, &actor->current.pos, nullptr,
                  &actor->smoke[0]);
    actor->timers[0] = 60;
    break;
  case 2:
    if (((u32)actor->actor_status & 0x4000000) && actor->timers[0] &&
        ((s16)actor->timers[0] & 7) == 0 && random_positive(1.0f) < 0.5f) {
      u32 play = guest_call<u32>(0x025200D4);
      gabi::Local<VectorLocal> direction;
      direction->value.x = 0.0f;
      direction->value.y = 1.0f;
      direction->value.z = 0.0f;
      guest_call(0x025CB374, at<void>(play + 0x599C),
                 (s32)*at<be<s16>>(0x1047B68C) + 4, -0x21, &direction->value);
    }
    if (animation_stopped(actor)) {
      actor->action = 0;
      actor->state = 0;
    }
    break;
  }
}
void kan_wait(st_class *actor) {
  actor->collisionInvulnerability = 10;
  actor->collisionMode = 2;
  switch ((s16)actor->state) {
  case 0:
    *member<be<u32>>(actor, 0x39C) = 0;
    actor->actor_status = (u32)actor->actor_status & ~0x20u;
    if (ambush_trigger(actor))
      actor->state = 1;
    break;
  case 1:
    anm_init(actor, actor->behavior == 2 ? 0x35 : 0x2C, 1.0f, 0, 0.0f, -1);
    actor->state = 2;
    actor->ambushState = 0;
    actor->timers[0] = *at<be<s16>>(0x1046E004);
    actor->timers[2] = (s16)actor->timers[0] + 20;
    [[fallthrough]];
  case 2:
    if (!actor->timers[0]) {
      *at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x98) = 1.0f;
      actor->state = 3;
      *member<be<u32>>(actor, 0x39C) = 4;
      actor->actor_status = (u32)actor->actor_status | 0x20;
    }
    break;
  case 3:
    if (animation_stopped(actor)) {
      actor->action = 0;
      actor->state = 0;
    }
    break;
  }
}
void fight(st_class *actor) {
  f64 distance;
  f32 spinTarget;
  f32 spinStep;
  gabi::Local<VectorLocal> local;
  auto &local_d8 = local->value;

  u32 player = player_pointer();
  spinTarget = 0.0f;
  spinStep = 0.02f;
  distance = player_distance(actor);
  raw_vector_copy(gabi::ea(&actor->playerPosition), player + 0x314);
  actor->turnStep = 0x200;
  switch (actor->state) {
  case 0:
    actor->targetSpeed = 0.0f;
    break;
  }
  actor->attackMode = 0;
  switch (actor->fightBehavior) {
  case 0:
    anm_init_default(actor, 0x9, 5.0f, 0);
    actor->fightBehavior++;
    break;
  case 1:
    if (gabi::ftoi(
            *at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x9C)) == 10) {
      sound_at_level(actor, 0x5893, 0);
      monster_sound(actor, 0x487F, 0);
    }
    if (animation_stopped(actor)) {
      anm_init_default(actor, 0xA, 1.0f, 0);
      actor->fightBehavior++;
    }
    break;
  case 2:
    if (gabi::ftoi(
            *at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x9C)) == 6) {
      sound_at_level(actor, 0x5894, 0);
      actor->spinSmokeTimer = (*at<be<s16>>(0x1047B692)) + 0x17;
      actor->spinSmokeMode = 0;
      actor->spinSmokeHeading =
          (actor->current.angle.y + 0x4ee0) + (*at<be<s16>>(0x1047B694));
    }
    actor->attackMode = 1;
    if (animation_stopped(actor)) {
      anm_init_default(actor, 0x29, 1.0f, 2);
      actor->fightBehavior++;
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
    }
    break;
  case 3:
    actor->targetSpeed = (*at<be<f32>>(0x1047BA9C)) + -2.0f;
    actor->forwardAngle = gabi::ftoi(
        ((*at<be<f32>>(0x1047B644)) + 10000.0f) *
        angle_sine(actor->frameCounter * ((*at<be<s16>>(0x1047B698)) + 500)));
    if (actor->timers[0] == 0) {
      if ((random_positive(1.0f) < 0.5f) || (distance < 300.0f)) {
        if (random_positive(1.0f) < 0.5f) {
          anm_init_default(actor, 0xC, 5.0f, 0);
          actor->attackSide = 1;
          actor->fightBehavior = 0x65;
        } else {
          anm_init_default(actor, 0xB, 5.0f, 0);
          actor->attackSide = 0;
          actor->fightBehavior = 100;
        }
      } else {
        actor->fightBehavior = 0x19;
      }
    }
    break;
  case 5:
    anm_init_default(actor, 0x4, 5.0f, 0);
    actor->fightBehavior++;
    break;
  case 6:
    if (gabi::ftoi(
            *at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x9C)) == 15) {
      sound_at_level(actor, 0x5893, 0);
      monster_sound(actor, 0x487F, 0);
    }
    if (animation_stopped(actor)) {
      anm_init_default(actor, 0x5, 1.0f, 0);
      actor->fightBehavior++;
    }
    break;
  case 7:
    if (gabi::ftoi(
            *at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x9C)) == 8) {
      sound_at_level(actor, 0x5894, 0);
      actor->spinSmokeTimer = (*at<be<s16>>(0x1047B692)) + 0x17;
      actor->spinSmokeMode = 1;
      actor->spinSmokeHeading =
          (actor->current.angle.y + 0x10000 + (*at<be<s16>>(0x1047B694))) -
          0x72a0;
    }
    actor->attackMode = 1;
    if (animation_stopped(actor)) {
      anm_init_default(actor, 0x28, 1.0f, 2);
      actor->fightBehavior++;
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
    }
    break;
  case 8:
    actor->targetSpeed = (*at<be<f32>>(0x1047BA9C)) + -2.0f;
    actor->forwardAngle = gabi::ftoi(
        ((*at<be<f32>>(0x1047B644)) + 10000.0f) *
        angle_sine(actor->frameCounter * ((*at<be<s16>>(0x1047B698)) + 500)));
    if (actor->timers[0] == 0) {
      if ((random_positive(1.0f) < 0.5f) || (distance < 300.0f)) {
        if (random_positive(1.0f) < 0.5f) {
          anm_init_default(actor, 0x7, 5.0f, 0);
          actor->attackSide = 0;
          actor->fightBehavior = 0x65;
        } else {
          anm_init_default(actor, 0x6, 5.0f, 0);
          actor->attackSide = 1;
          actor->fightBehavior = 100;
        }
      } else {
        actor->fightBehavior = 0x14;
      }
    }
    break;
  case 10:
    anm_init_default(actor, 0x9, 5.0f, 0);
    actor->fightBehavior++;
    monster_sound(actor, 0x4880, 0);
    break;
  case 0xb:
    if (animation_stopped(actor)) {
      anm_init_default(actor, 0x1A, 1.0f, 0);
      actor->fightBehavior++;
    }
    break;
  case 0xc:
    spinTarget = 1.0f;
    spinStep = 0.1f;
    if (animation_stopped(actor)) {
      actor->fightBehavior++;
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
      actor->spinSmokeTimer = 0xf;
      actor->spinSmokeMode = 2;
      actor->spinSmokeHeading =
          (actor->current.angle.y + 0x2000) + (*at<be<s16>>(0x1047B698));
    }
    break;
  case 0xd:
    actor->spinSmokeTimer = 0xf;
    actor->targetSpeed = (*at<be<f32>>(0x1047BA9C)) + 10.0f;
    spinTarget = 1.0f;
    spinStep = 0.1f;
    actor->forwardAngle = (s16)gabi::ftoi(
        ((*at<be<f32>>(0x1047B644)) + 10000.0f) *
        angle_sine(actor->frameCounter * ((*at<be<s16>>(0x1047B698)) + 500)));
    if (((actor->timers[0] == 0) && (-0x400 < actor->spinAngle)) &&
        (actor->spinAngle < 0x400)) {
      anm_init_default(actor, 0x1B, 1.0f, 0);
      actor->fightBehavior++;
    }
    break;
  case 0xe:
    if ((animation_stopped(actor)) && (std::fabs(actor->spinBlend) < 0.01f)) {
      anm_init_default(actor, 0x29, 5.0f, 2);
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
      actor->fightBehavior = 3;
    }
    break;
  case 0xf:
    anm_init_default(actor, 0x4, 5.0f, 0);
    actor->fightBehavior++;
    monster_sound(actor, 0x4880, 0);
    break;
  case 0x10:
    if (animation_stopped(actor)) {
      anm_init_default(actor, 0x16, 1.0f, 0);
      actor->fightBehavior++;
    }
    break;
  case 0x11:
    spinTarget = -1.0f;
    spinStep = 0.1f;
    if (animation_stopped(actor)) {
      actor->fightBehavior++;
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
      actor->spinSmokeTimer = 0xf;
      actor->spinSmokeMode = 2;
      actor->spinSmokeHeading =
          (actor->current.angle.y + 0x2000) + (*at<be<s16>>(0x1047B69A));
    }
    break;
  case 0x12:
    actor->spinSmokeTimer = 0xf;
    actor->targetSpeed = (*at<be<f32>>(0x1047BA9C)) + 10.0f;
    spinTarget = -1.0f;
    spinStep = 0.1f;
    actor->forwardAngle = (s16)gabi::ftoi(
        ((*at<be<f32>>(0x1047B644)) + 10000.0f) *
        angle_sine(actor->frameCounter * ((*at<be<s16>>(0x1047B698)) + 500)));
    if (((actor->timers[0] == 0) && (-0x400 < actor->spinAngle)) &&
        (actor->spinAngle < 0x400)) {
      anm_init_default(actor, 0x17, 1.0f, 0);
      actor->fightBehavior++;
    }
    break;
  case 0x13:
    if ((animation_stopped(actor)) && (std::fabs(actor->spinBlend) < 0.01f)) {
      anm_init_default(actor, 0x28, 5.0f, 2);
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
      actor->fightBehavior = 8;
    }
    break;
  case 0x14:
    anm_init_default(actor, 0x1C, 5.0f, 0);
    actor->fightBehavior++;
    break;
  case 0x15:
    spinTarget = 1.0f;
    spinStep = 0.1f;
    if (animation_stopped(actor)) {
      actor->fightBehavior++;
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
      actor->spinSmokeTimer = 0xf;
      actor->spinSmokeMode = 2;
      actor->spinSmokeHeading =
          (actor->current.angle.y + 0x2000) + (*at<be<s16>>(0x1047B698));
    }
    break;
  case 0x16:
    actor->spinSmokeTimer = 0xf;
    actor->targetSpeed = (*at<be<f32>>(0x1047BA9C)) + 10.0f;
    spinTarget = 1.0f;
    spinStep = 0.1f;
    actor->forwardAngle = (s16)gabi::ftoi(
        ((*at<be<f32>>(0x1047B644)) + 10000.0f) *
        angle_sine(actor->frameCounter * ((*at<be<s16>>(0x1047B698)) + 500)));
    if (((actor->timers[0] == 0) && (-0x400 < actor->spinAngle)) &&
        (actor->spinAngle < 0x400)) {
      anm_init_default(actor, 0x1D, 1.0f, 0);
      actor->fightBehavior++;
    }
    break;
  case 0x17:
    if ((animation_stopped(actor)) && (std::fabs(actor->spinBlend) < 0.01f)) {
      anm_init_default(actor, 0x29, 5.0f, 2);
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
      actor->fightBehavior = 3;
    }
    break;
  case 0x19:
    anm_init_default(actor, 0x18, 5.0f, 0);
    actor->fightBehavior++;
    break;
  case 0x1a:
    actor->attackMode = 2;
    spinTarget = -1.0f;
    spinStep = 0.1f;
    if (animation_stopped(actor)) {
      actor->fightBehavior++;
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
      actor->spinSmokeTimer = 0xf;
      actor->spinSmokeMode = 2;
      actor->spinSmokeHeading =
          (actor->current.angle.y + 0x2000) + (*at<be<s16>>(0x1047B69A));
    }
    break;
  case 0x1b:
    actor->spinSmokeTimer = 0xf;
    actor->targetSpeed = (*at<be<f32>>(0x1047BA9C)) + 10.0f;
    spinTarget = -1.0f;
    spinStep = 0.1f;
    actor->forwardAngle = (s16)gabi::ftoi(
        ((*at<be<f32>>(0x1047B644)) + 10000.0f) *
        angle_sine(actor->frameCounter * ((*at<be<s16>>(0x1047B698)) + 500)));
    if (((actor->timers[0] == 0) && (-0x400 < actor->spinAngle)) &&
        (actor->spinAngle < 0x400)) {
      anm_init_default(actor, 0x19, 1.0f, 0);
      actor->fightBehavior++;
    }
    break;
  case 0x1c:
    if ((animation_stopped(actor)) && (std::fabs(actor->spinBlend) < 0.01f)) {
      anm_init_default(actor, 0x28, 5.0f, 2);
      actor->fightBehavior = 8;
      actor->timers[0] = gabi::ftoi((f32)random_positive(50.0f) + 30.0f);
    }
    break;
  case 100:
    if (gabi::ftoi(
            *at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x9C)) == 15) {
      sound_at_level(actor, 0x5894, 0);
    }
    if (gabi::ftoi(
            *at<be<f32>>(gabi::ea((void *)actor->bodyAnimation), 0x9C)) == 24) {
      buki_smoke_set(actor);
    }
    actor->attackMode = 1;
  case 0x65:
    if (animation_stopped(actor)) {
      actor->action = 0;
      wait_set(actor);
      actor->state = 1;
      actor->timers[1] = gabi::ftoi(random_positive(40.0f));
    }
    break;
  }
  guest_call(0x0200ED84, &actor->spinBlend, spinTarget, 1.0f, spinStep);
  actor->spinAngle += (s16)gabi::ftoi(((*at<be<f32>>(0x1047B63C)) + 5000.0f) *
                                      actor->spinBlend);
  if (std::fabs(actor->spinBlend) > 0.05f) {
    s16 r4 = actor->soundCounter;
    actor->soundCounter =
        r4 + gabi::ftoi(((*at<be<f32>>(0x1047B63C)) + 5000.0f) *
                        std::fabs(actor->spinBlend));
    if ((actor->soundCounter >= 100) && (r4 < 100)) {
      u32 r23 = gabi::ftoi(std::fabs((f32)actor->spinBlend * 100.0f));
      if (r23 < 0x32) {
        r23 = 0x32;
      }
      sound_at_level(actor, 0x5895, r23);
    }
  } else {
    actor->soundCounter = 0;
  }
  joint_matrix(actor, 11);
  local_d8.x = (*at<be<f32>>(0x1047B638)) + 80.0f;
  local_d8.y = 0.0f;
  local_d8.z = 0.0f;
  raw_vector_copy(gabi::ea(&actor->previousWeaponTip),
                  gabi::ea(&actor->weaponTip));
  guest_call(0x0200FCD8, &local_d8, &actor->weaponTip);
  if (std::fabs(actor->spinBlend) > 0.5f) {
    gabi::Local<LineCheckLocal> line;
    initialize_line(line);
    actor->attackMode = 2;
    if ((std::fabs(actor->spinBlend) > 0.9f) &&
        (std::fabs(actor->speedF) > std::fabs(actor->targetSpeed) * 0.9f)) {
      if (line_cross(line, &actor->previousWeaponTip, &actor->weaponTip,
                     actor)) {
        actor->action = 0;
        wait_set(actor);
        actor->state = 1;
        actor->timers[1] = gabi::ftoi(random_positive(40.0f));
      }
    }
    finish_line(line);
  }
  if (std::fabs(actor->spinBlend) > 0.5f) {
    actor->attackMode = 2;
  }
}
void fight2(st_class *actor) {
  guest_call<u32>(0x025200D4);
  gabi::Local<VectorLocal> zero, effect;
  auto &local_2c = zero->value;
  auto &cStack_38 = effect->value;

  bool r30 = false;
  local_2c.x = 0.0f;
  local_2c.y = 0.0f;
  local_2c.z = 0.0f;
  actor->targetSpeed = 0.0f;
  switch (actor->state) {
  case 0:
    actor->collisionInvulnerability = 10;
    actor->collisionMode = 1;
    anm_init_default(actor, 0x20, 5.0f, 0);
    actor->state++;
    monster_sound(actor, 0x4881, 0);
    break;
  case 1:
    actor->collisionInvulnerability = 10;
    actor->collisionMode = 1;
    if (animation_stopped(actor)) {
      anm_init_default(actor, 0x26, 1.0f, 0);
      actor->state++;
    }
    break;
  case 2:
    actor->collisionInvulnerability = 10;
    actor->collisionMode = 1;
    if (animation_stopped(actor)) {
      anm_init_default(actor, 0x27, 1.0f, 0);
      actor->state = 0x65;
      actor->maceState = 1;
      joint_matrix(actor, 3);
      guest_call(0x0200FCD8, &local_2c, &cStack_38);
      particle_with_angle(0x817A, &cStack_38, &actor->shape_angle);
      sound_at_level(actor, 0x5897, 0);
    }
    break;
  case 3:
    actor->maceState = 3;
    anm_init_default(actor, 0x14, 3.0f, 2);
    actor->state++;
    actor->timers[0] = gabi::ftoi(random_positive(3.0f)) * 0x28;
    monster_sound(actor, 0x4883, 0);
    break;
  case 4:
    actor->attackMode = 1;
    if ((((gabi::ftoi(*at<be<f32>>(gabi::ea((void *)actor->bodyAnimation),
                                   0x9C)) == 1) ||
          (gabi::ftoi(*at<be<f32>>(gabi::ea((void *)actor->bodyAnimation),
                                   0x9C)) == 11)) ||
         (gabi::ftoi(*at<be<f32>>(gabi::ea((void *)actor->bodyAnimation),
                                  0x9C)) == 21)) ||
        (gabi::ftoi(*at<be<f32>>(gabi::ea((void *)actor->bodyAnimation),
                                 0x9C)) == 31)) {
      sound_at_level(actor, 0x5899, 0);
    }
    if (actor->timers[0] == 0) {
      r30 = true;
    }
    break;
  case 0x32:
    anm_init(actor, 0x33, 10.0f, 2, 1.0f, -1);
    actor->maceState = 1;
    actor->state++;
    actor->timers[0] = 0x14;
  case 0x33:
    actor->collisionInvulnerability = 10;
    actor->collisionMode = 1;
    if (actor->timers[0] == 0) {
      anm_init_default(actor, 0x2D, 5.0f, 0);
      actor->state++;
    }
    break;
  case 0x34:
    actor->collisionInvulnerability = 10;
    actor->collisionMode = 1;
    if (animation_stopped(actor)) {
      anm_init_default(actor, 0x2E, 1.0f, 0);
      actor->state++;
      actor->maceState = 0;
    }
    break;
  case 0x35:
    actor->collisionInvulnerability = 10;
    actor->collisionMode = 1;
    if (animation_stopped(actor)) {
      actor->state = 100;
      anm_init(actor, 0xE, 10.0f, 2, 1.0f, -1);
    }
    break;
  case 0x64:
    if (actor->weaponState == 10) {
      r30 = true;
    }
    break;
  case 0x65:
    if (animation_stopped(actor)) {
      r30 = true;
    }
    break;
  }
  if (r30) {
    actor->action = 0;
    wait_set(actor);
    actor->state = 1;
    actor->timers[1] = gabi::ftoi(random_positive(40.0f));
  }
}

void St_move(st_class *actor) {
  WWHD_FUNC(0x024972A4, void, actor);
  guest_call<u32>(0x025200D4);
  joint_matrix(actor, 11);
  gabi::Local<VectorLocal> direction;
  direction->value.x = 160.0f;
  direction->value.y = *at<be<f32>>(0x1047B63C);
  direction->value.z = *at<be<f32>>(0x1047B640);
  guest_call(0x0200FCD8, &direction->value, &actor->weaponPosition);
  actor->smokeMode = 0;
  switch ((s16)actor->action) {
  case 0:
    actor->bodyForm = 0;
    jyunkai(actor);
    break;
  case 5:
    actor->bodyForm = 0;
    fight(actor);
    break;
  case 15:
    actor->bodyForm = 0;
    ground_wait(actor);
    break;
  case 16:
    actor->bodyForm = 0;
    kan_wait(actor);
    break;
  case 29:
    actor->bodyForm = 0;
    fight2(actor);
    break;
  case 30:
    actor->bodyForm = 0;
    st_break_wait(actor);
    break;
  case 31:
    head_damage(actor);
    break;
  case 32:
    actor->bodyForm = 1;
    ue_move(actor);
    break;
  case 33:
    actor->bodyForm = 2;
    sita_move(actor);
    break;
  }
  if (actor->action != 16)
    speed_pos_calc(actor);
  if (actor->smokeMode && actor->weaponState == 10) {
    if (actor->smokeMode == 1 && !word(gabi::ea(actor) + 0x21D4))
      toon_particle(actor, 0xA17D, &actor->weaponPosition, &actor->shape_angle,
                    &actor->spinSmoke);
  } else
    guest_call(0x025A5F88, &actor->spinSmoke);
}
VERIFY(0x024972A4, St_move);
struct AttackInfo {
  gptr<void> object, attacker;
  be<u8> damage, dead, result, pad;
  csXyz direction;
  be<u16> cutBits;
  gptr<cXyz> position;
  be<s32> sound;
};
struct AttackInfoLocal {
  be<u32> linkage[2];
  AttackInfo value;
};
void damage_check(st_class *actor) {
  u32 player = player_pointer();
  guest_call(0x02515E50, member<void>(actor, 0x648));
  if (!actor->collisionInvulnerability) {
    gabi::Local<AttackInfoLocal> local;
    auto &info = local->value;
    info.cutBits = 0;
    info.sound = 0;
    for (u32 index = 0; index < 7; ++index) {
      u32 sphere = gabi::ea(&actor->spheres[index]);
      if (!guest_call<s32>(0x025162A4, at<void>(sphere)))
        continue;
      u32 object = guest_call<u32>(0x02516300, at<void>(sphere));
      info.object = at<void>(object);
      info.position = at<cXyz>(sphere + 0xCC);
      if (word(object + 0x10) & 0x100000) {
        *member<be<s8>>(actor, 0x21F6) = 1;
        u32 held = find_actor(actor->weaponID);
        if (held)
          guest_call(0x025D9D24, at<void>(held));
        actor->weaponState = 0;
        return;
      }
      actor->collisionInvulnerability = 5;
      if (index == 0 && actor->headState) {
        s8 remaining = *member<be<s8>>(actor, 0x3A9);
        *member<be<s8>>(actor, 0x3A9) = 0;
        guest_call(0x025192A8, actor, &info);
        *member<be<s8>>(actor, 0x3A9) = remaining;
        if (info.result == 1 && *at<be<u8>>(player, 0x69E8))
          actor->collisionInvulnerability = 2;
        auto *head = &actor->parts[15];
        if (*member<be<s8>>(actor, 0x3A1) <= 0 || info.result == 9 ||
            info.result == 2) {
          head_anm_init(actor, 0x10, 1.0f, 0, 1.0f, -1);
          head->state = 8;
          if (info.result == 9 && *at<be<u8>>(player, 0x3AC) != 0x11) {
            head->velocity.x = 0.0f;
            head->velocity.y = -5.0f;
            head->velocity.z = 0.0f;
            *at<be<u8>>(0x101EACB7) = 6;
          } else {
            head->angularSpeed.x = gabi::ftoi(random_signed(9000.0f));
            head->angularSpeed.z = gabi::ftoi(random_signed(9000.0f));
            guest_call(0x025F1884, at<void>(word(0x1018C7B0)),
                       (s16)*at<be<s16>>(player, 0x32A));
            gabi::Local<VectorLocal> direction;
            if (*at<be<u8>>(player, 0x3AC) == 0x11) {
              direction->value.x = *at<be<f32>>(0x1047BAA0) - 100.0f;
              direction->value.y = 30.0f;
              direction->value.z = 0.0f;
              *at<be<u8>>(0x101EACB7) = 6;
            } else {
              direction->value.x = 0.0f;
              direction->value.y = 50.0f;
              direction->value.z = 50.0f;
            }
            guest_call(0x0200FCD8, &direction->value, &head->velocity);
          }
          head->wait = 30;
          monster_sound(actor, 0x4884, 0);
        } else {
          head_anm_init(actor, 0xF, 1.0f, 0, 1.0f, -1);
          head->state = 7;
          head->wait = 127;
          actor->headShakeTimer = *at<be<s16>>(0x1047BB0C) + 20;
          monster_sound(actor, 0x487B, 0);
        }
      } else {
        guest_call(0x02518DB0, &info);
        if (info.damage)
          guest_call(0x025E1FD8);
        u32 attacker = gabi::ea((void *)info.attacker);
        if (attacker && *at<be<s16>>(attacker, 8) == 0x1BE &&
            *member<be<s8>>(actor, 0x3A9)) {
          --*member<be<s8>>(actor, 0x3A9);
          guest_call(0x025D9000, &actor->current.pos,
                     word(gabi::ea(actor) + 0x3A4), (s8)actor->current.roomNo,
                     0, (u8)*member<be<u8>>(actor, 0x3A8));
          ++*member<be<u8>>(actor, 0x3A8);
          attacker = gabi::ea((void *)info.attacker);
        }
        if ((attacker && *at<be<s16>>(attacker, 8) == 0xA8) ||
            info.result == 2) {
          if (info.result == 2)
            actor->bodyHealth = 0;
          guest_call(0x02518CC8, actor, (void *)info.object, 0x34);
          monster_sound(actor, 0x487B, 0);
          actor->bodyHealth = (s8)actor->bodyHealth - (u8)info.damage;
          actor->shakeIntensity =
              *at<be<s16>>(info.result == 9 ? 0x1047B9F6 : 0x1047B9F4) +
              (info.result == 9 ? 20 : 10);
          if (info.result == 1 && *at<be<u8>>(player, 0x69E8)) {
            actor->collisionInvulnerability = 2;
            u8 cut = *at<be<u8>>(player, 0x3AC);
            if (cut == 8 || cut == 9)
              actor->bodyHealth = (u8)actor->bodyHealth - (u8)info.damage;
          }
          guest_call(0x025E1D30, (s32)info.sound);
          if ((s8)actor->bodyHealth <= 0) {
            actor->maceState = 0;
            u8 form = actor->bodyForm;
            if (info.cutBits == 0x80 && !form) {
              actor->action = 33;
              actor->state = 0;
              actor->partCommand = 10;
              actor->upperBodyID =
                  guest_call<u32>(0x025D5834, 0xBE, 0xE, &actor->current.pos,
                                  (s8)actor->current.roomNo, &actor->home.angle,
                                  (void *)nullptr, -1, (void *)nullptr);
              actor->bodyHealth = 5;
              actor->upperBodyPresent = 0;
            } else {
              actor->state = form == 2 ? 35 : form == 1 ? 20 : 0;
              actor->action = 30;
              actor->partCommand = 1;
              actor->lifetime = 0;
              sound_at_level(actor, 0x5891, 0);
              monster_sound(actor, 0x487C, 0);
            }
          }
        } else if (index == 0) {
          actor->shakeIntensity = *at<be<s16>>(0x1047B9F6) + 5;
          u8 form = actor->bodyForm;
          if (form == 1) {
            actor->action = 31;
            actor->state = 10;
          } else if (!form) {
            actor->action = 31;
            actor->state = 0;
          }
          sound_at_level(actor, 0x588E, 0);
          monster_sound(actor, 0x487A, 0);
        } else {
          guest_call(0x02518CC8, actor, (void *)info.object, 0x34);
          monster_sound(actor, 0x487B, 0);
          actor->shakeIntensity = *at<be<s16>>(0x1047B9F6) + 8;
          if (!actor->bodyForm) {
            actor->action = 0;
            wait_set(actor);
            actor->state = 1;
            actor->timers[1] = 10;
          }
        }
      }
      break;
    }
  }
  if (actor->shakeIntensity)
    --actor->shakeIntensity;
}

void collision_register(u32 sphere) {
  u32 play = guest_call<u32>(0x025200D4);
  guest_call(0x0200E240, at<void>(play + 0x26A4), at<void>(sphere));
}
void cc_set(st_class *actor) {
  gabi::Local<VectorLocal> zero, center, disabled, disabledWeapon;
  zero->value.x = 0.0f;
  zero->value.y = 0.0f;
  zero->value.z = 0.0f;
  disabled->value.x = -50000.0f;
  disabled->value.y = -50000.0f;
  disabled->value.z = 0.0f;
  disabledWeapon->value.x = 50000.0f;
  disabledWeapon->value.y = -50000.0f;
  disabledWeapon->value.z = 0.0f;
  for (u32 index = 0; index < 7; ++index) {
    bool active = false;
    if (actor->collisionMode != 2) {
      if (actor->action == 33 || actor->state == 35)
        active = !actor->collisionMode && index >= 5;
      else if (actor->collisionMode)
        active = index == 0;
      else
        active = actor->action != 32 || index < 5;
    }
    u32 sphere = gabi::ea(&actor->spheres[index]);
    if (active) {
      if (!index && actor->headState) {
        u32 model = word(gabi::ea((void *)actor->headAnimation) + 0x90);
        guest_call(0x028E90D4, at<void>(model ? model + 0xC8 : 0),
                   at<void>(word(0x1018C7B0)));
      } else
        joint_matrix(actor, word(0x101D0E9C + index * 4));
      guest_call(0x0200FCD8, &zero->value, &center->value);
      guest_call(0x02018D40, at<void>(sphere + 0x118), &center->value);
      guest_call(0x02018C8C, at<void>(sphere + 0x118), index ? 25.0f : 45.0f);
      *at<be<u32>>(sphere, 0x18) = word(sphere + 0x18) | 1;
    } else {
      guest_call(0x02018C8C, at<void>(sphere + 0x118), -500.0f);
      guest_call(0x02018D40, at<void>(sphere + 0x118), &disabled->value);
    }
    collision_register(sphere);
  }
  u32 weapon = gabi::ea(&actor->weaponSphere);
  if (actor->attackMode && !actor->collisionMode) {
    if (actor->maceState) {
      raw_vector_copy(gabi::ea(&center->value), gabi::ea(actor) + 0x1FCC);
      guest_call(0x02018C8C, at<void>(weapon + 0x118), 30.0f);
    } else if (actor->attackMode == 3) {
      joint_matrix(actor, 25);
      zero->value.x = 0.0f;
      zero->value.y = 0.0f;
      zero->value.z = 0.0f;
      guest_call(0x0200FCD8, &zero->value, &center->value);
      guest_call(0x02018C8C, at<void>(weapon + 0x118), 30.0f);
      *at<be<u8>>(weapon, 0x6F) = 0;
    } else {
      *at<be<u8>>(weapon, 0x6F) = 6;
      joint_matrix(actor, 11);
      f32 factor = 1.0f;
      if (actor->attackMode == 2) {
        f32 blend = std::fabs((f32)actor->spinBlend);
        if (blend > 0.5f)
          *at<be<u8>>(weapon, 0x6F) = 7;
        factor = blend * 1.5f;
      }
      bool odd = (u32)actor->frameCounter & 1;
      zero->value.x = *at<be<f32>>(0x1047B638) + (odd ? 80.0f : 30.0f);
      guest_call(0x02018C8C, at<void>(weapon + 0x118),
                 factor * (odd ? 70.0f : 50.0f));
      zero->value.y = 0.0f;
      zero->value.z = 0.0f;
      guest_call(0x0200FCD8, &zero->value, &center->value);
    }
    if (actor->otherFlag)
      guest_call(0x025167E4, &actor->weaponSphere, &center->value);
    else {
      guest_call(0x025167C0, &actor->weaponSphere, &center->value);
      ++actor->otherFlag;
    }
    *at<be<u32>>(weapon) = word(weapon) | 1;
  } else {
    actor->otherFlag = 0;
    guest_call(0x02018D40, at<void>(weapon + 0x118), &disabledWeapon->value);
  }
  collision_register(weapon);
  actor->collisionMode = 0;
  actor->attackMode = 0;
}
void eff_move(st_class *actor) {
  if (actor->emitterTimer)
    --actor->emitterTimer;
  if (actor->recoveryEffect == 1) {
    actor->effectScale = 1.0f;
    if (!actor->emitterTimer) {
      actor->emitterTimer = 15;
      for (u32 index = 0; index < 20; ++index)
        if (!actor->emitters[index]) {
          u32 emitter = particle(0x817B, &actor->current.pos);
          actor->emitters[index] = at<void>(emitter);
          if (emitter) {
            *at<be<u32>>(emitter, 0x254) = word(emitter + 0x254) | 0x40;
            actor->emitterTimers[index] = 200;
          }
          break;
        }
    }
  }
  if (actor->recoveryEffect == 2)
    guest_call(0x0200EDC8, &actor->effectScale, 1.0f, 0.1f);
  for (u32 index = 0; index < 20; ++index) {
    u32 emitter = gabi::ea((void *)actor->emitters[index]);
    if (emitter) {
      *at<be<f32>>(emitter, 0x238) = actor->effectScale;
      if (actor->emitterTimers[index] &&
          (--actor->emitterTimers[index], !actor->emitterTimers[index])) {
        emitter = gabi::ea((void *)actor->emitters[index]);
        *at<be<u32>>(emitter, 0x5C) = 0xFFFFFFFF;
        *at<be<u32>>(emitter, 0x254) = word(emitter + 0x254) | 1;
        actor->emitters[index] = nullptr;
      }
    }
  }
}
void spin_smoke_set(st_class *actor, f32 rate) {
  gabi::Local<LineCheckLocal> line;
  initialize_line(line);
  gabi::Local<VectorLocal> start, end;
  raw_vector_copy(gabi::ea(&start->value), gabi::ea(&actor->spinSmokePosition));
  raw_vector_copy(gabi::ea(&end->value), gabi::ea(&actor->spinSmokePosition));
  start->value.y = (f32)start->value.y + 100.0f;
  end->value.y = (f32)end->value.y - 100.0f;
  u32 attribute = 0;
  if (line_cross(line, &start->value, &end->value, actor)) {
    u32 check = gabi::ea(&line->check);
    actor->spinSmokePosition.y = *at<be<f32>>(check, 0x34) + 30.0f;
    u32 play = guest_call<u32>(0x025200D4);
    attribute = guest_call<u32>(0x024EF0F4, at<void>(play + 0x12A0),
                                at<void>(check + 0x14));
  } else
    actor->spinSmokePosition.y = (f32)actor->spinSmokePosition.y - 20000.0f;
  if (!actor->spinSmokeCount || attribute == 4) {
    ++actor->spinSmokeCount;
    if (attribute < 4 || attribute == 11) {
      guest_call(0x025A5F88, &actor->smoke[2]);
      u32 emitter = toon_particle(actor, 0x2022, &actor->spinSmokePosition,
                                  &actor->spinSmokeAngle, &actor->smoke[2]);
      if (emitter) {
        *at<be<f32>>(emitter, 0x34) = rate;
        *at<be<f32>>(emitter, 0x58) = 1.0f;
        *at<be<f32>>(emitter, 0x220) = 0.8f;
        *at<be<f32>>(emitter, 0x224) = 0.8f;
        *at<be<f32>>(emitter, 0x228) = 0.8f;
        f32 scale = *at<be<f32>>(0x1047B63C) + 2.0f;
        *at<be<f32>>(emitter, 0x238) = scale;
        *at<be<f32>>(emitter, 0x23C) = scale;
        *at<be<f32>>(emitter, 0x240) = scale;
      }
    } else if (attribute == 4) {
      u32 emitter = particle_with_angle(0x24, &actor->spinSmokePosition,
                                        &actor->spinSmokeAngle);
      if (emitter) {
        *at<be<u32>>(emitter, 0x5C) = 3;
        *at<be<f32>>(emitter, 0x34) = rate * 0.5f;
      }
    }
  }
  finish_line(line);
}
void spin_smoke_move(st_class *actor) {
  if (!actor->spinSmokeTimer)
    return;
  --actor->spinSmokeTimer;
  if (actor->spinSmokeTimer >= 10) {
    gabi::Local<VectorLocal> direction;
    direction->value.x = 0.0f;
    direction->value.y = 0.0f;
    actor->spinSmokeAngle.x = 0;
    actor->spinSmokeAngle.z = 0;
    guest_call(0x0200FAD8, 0, (f32)actor->current.pos.x,
               (f32)actor->current.pos.y + 7.5f, (f32)actor->current.pos.z);
    direction->value.z = (*at<be<f32>>(0x1047B618) + 1.0f) * -175.0f;
    guest_call(0x025F1C28, at<void>(word(0x1018C7B0)),
               (s16)actor->spinSmokeHeading);
    guest_call(0x0200FCD8, &direction->value, &actor->spinSmokePosition);
    actor->spinSmokeAngle.y = actor->spinSmokeHeading;
    spin_smoke_set(actor, *at<be<f32>>(0x1047B61C) + 3.0f);
    if (actor->spinSmokeMode == 2)
      actor->spinSmokeHeading += (s16)gabi::ftoi(
          (*at<be<f32>>(0x1047B63C) + 5000.0f) * (f32)actor->spinBlend);
    else {
      s16 step = *at<be<s16>>(0x1047B696) + 2000;
      actor->spinSmokeHeading += (actor->spinSmokeMode == 0 ? step : -step);
    }
  } else
    actor->spinSmokePosition.y = (f32)actor->current.pos.y + 20000.0f;
  if (!actor->spinSmokeTimer) {
    guest_call(0x025A5F88, &actor->smoke[2]);
    actor->spinSmokeCount = 0;
  }
}
void move_visible_parts(st_class *actor) {
  for (u32 joint = 0; joint < 26; ++joint)
    if (actor->parts[joint].model &&
        (!actor->maceState || (joint != 3 && joint != 4)))
      part_move(actor, joint);
  if (actor->action != 33 && actor->state != 35)
    ke_control(actor);
  if (actor->maceState)
    nun_control(actor);
}
s32 daSt_Execute(st_class *actor) {
  WWHD_FUNC(0x02493FB0, s32, actor);
  guest_call<u32>(0x025200D4);
  if (actor->deathCountdown) {
    --actor->deathCountdown;
    if (!actor->deathCountdown)
      guest_call(0x025D57E0, actor);
    return 1;
  }
  if (actor->hidden ||
      ((f32)actor->home.pos.y - (f32)actor->current.pos.y > 4000.0f)) {
    guest_call(0x025D57E0, actor);
    return 1;
  }
  if (actor->emergenceDelay &&
      (--actor->emergenceDelay, !actor->emergenceDelay))
    actor->actor_status = (u32)actor->actor_status & ~0x4000u;
  if (guest_call<s32>(0x020402C8, &actor->ice)) {
    u32 model = word(gabi::ea((void *)actor->bodyAnimation) + 0x90);
    guest_call(0x0200FAD8, 0, (f32)actor->current.pos.x,
               (f32)actor->current.pos.y, (f32)actor->current.pos.z);
    guest_call(0x025F1C28, at<void>(word(0x1018C7B0)),
               (s16)((s16)actor->shape_angle.y + (s16)actor->spinAngle));
    f32 horizontal = *member<be<f32>>(actor, 0x2394),
        vertical = *member<be<f32>>(actor, 0x2398);
    guest_call(0x0200FC74, 1, horizontal, vertical, horizontal);
    copy_model_matrix(model, word(0x1018C7B0));
    guest_call(0x025E55A0, (void *)actor->bodyAnimation);
    guest_call(0x025E55A0, (void *)actor->headAnimation);
    move_visible_parts(actor);
    return 1;
  }
  *at<be<u8>>(0x1046DFDC) = 0;
  if (actor->upperBodyID != 0xFFF) {
    u32 upper = find_actor(actor->upperBodyID);
    if (upper && *at<be<s8>>(upper, 0x20CD)) {
      guest_call(0x025D57E0, actor);
      for (u32 index = 0; index < 26; ++index)
        if (actor->parts[index].state < 10)
          particle(0x817E, &actor->parts[index].pos);
      u32 weapon = find_actor(actor->weaponID);
      if (weapon)
        guest_call(0x025D9D24, at<void>(weapon));
      return 1;
    }
  }
  ++actor->frameCounter;
  for (u32 index = 0; index < 5; ++index)
    if (actor->timers[index])
      --actor->timers[index];
  if (actor->collisionInvulnerability)
    --actor->collisionInvulnerability;
  if (actor->recoverTimer)
    --actor->recoverTimer;
  St_move(actor);
  damage_check(actor);
  guest_call(0x0200F428, &actor->shape_angle.y,
             (s16)((s16)actor->current.angle.y + (s16)actor->forwardAngle), 2,
             0x1000);
  actor->forwardAngle = 0;
  guest_call(0x025E535C, (void *)actor->bodyAnimation, &actor->eyePos, 0, 0);
  guest_call(0x025E535C, (void *)actor->headAnimation, &actor->eyePos, 0, 0);
  u32 model = word(gabi::ea((void *)actor->bodyAnimation) + 0x90);
  f32 sx = actor->scale.x, sy = actor->scale.y, sz = actor->scale.z;
  *at<be<f32>>(model, 0xBC) = sx;
  *at<be<f32>>(model, 0xC0) = sy;
  *at<be<f32>>(model, 0xC4) = sz;
  guest_call(0x0200FAD8, 0, (f32)actor->current.pos.x,
             (f32)actor->current.pos.y, (f32)actor->current.pos.z);
  guest_call(0x025F1C28, at<void>(word(0x1018C7B0)),
             (s16)((s16)actor->shape_angle.y + (s16)actor->spinAngle));
  copy_model_matrix(model, word(0x1018C7B0));
  guest_call(0x025E55A0, (void *)actor->bodyAnimation);
  guest_call(0x025E55A0, (void *)actor->headAnimation);
  cc_set(actor);
  for (u32 joint = 0; joint < 26; ++joint)
    if (actor->parts[joint].model &&
        (!actor->maceState || (joint != 3 && joint != 4)))
      part_move(actor, joint);
  if (actor->partCommand != 2)
    actor->partCommand = 0;
  guest_call(0x0200F428, &actor->spinAngle, 0, 2, 0x40);
  if (actor->action != 33 && actor->state != 35)
    ke_control(actor);
  if (actor->maceState)
    nun_control(actor);
  eff_move(actor);
  spin_smoke_move(actor);
  u32 environment = guest_call<u32>(0x02555D0C);
  guest_call(0x025626A4, at<void>(environment), 0, &actor->current.pos,
             &actor->tevStr);
  return 1;
}
VERIFY(0x02493FB0, daSt_Execute);

} // namespace Stalfos

/**
 * d_a_warpgn.cpp (WWHD)
 * Warpgn - warp portal to Forsaken Fortress (Gmjwp, daWarpgn_c)
 *
 * Written from the WWHD
 * code with the GameCube decompilation (zeldaret/tww src/d/actor/d_a_warpgn.cpp) as reference,
 * verified against cking.rpx.
 */
#include "d/actor/d_a_warpgn.h"
#include "bindings.h"
using Warpgn::Actor;
template <class T> static T read(u32 ea) {
  return *gabi::at<be<T>>(ea);
}
template <class T> static void write(u32 ea, T value) {
  *gabi::at<be<T>>(ea) = value;
}
s32 warpgn_createMethod(Actor *actor) {
  WWHD_FUNC(0x024D9158, s32, actor);
  return gabi::call<s32>(0x024D90A0, actor);
}
VERIFY(0x024D9158, warpgn_createMethod);
s32 warpgn_deleteMethod(Actor *actor) {
  WWHD_FUNC(0x024D91FC, s32, actor);
  return gabi::call<s32>(0x024D915C, actor);
}
VERIFY(0x024D91FC, warpgn_deleteMethod);
s32 warpgn_drawMethod(Actor *actor) {
  WWHD_FUNC(0x024D951C, s32, actor);
  return gabi::call<s32>(0x024D92B4, actor);
}
VERIFY(0x024D951C, warpgn_drawMethod);
s32 warpgn_executeMethod(Actor *actor) {
  WWHD_FUNC(0x024D9F54, s32, actor);
  return gabi::call<s32>(0x024D9E00, actor);
}
VERIFY(0x024D9F54, warpgn_executeMethod);
s32 warpgn_heapCallback(Actor *actor) {
  WWHD_FUNC(0x024D8BB0, s32, actor);
  return gabi::call<s32>(0x024D885C, actor);
}
VERIFY(0x024D8BB0, warpgn_heapCallback);
s32 warpgn_actWait(Actor *actor) {
  WWHD_FUNC(0x024DA00C, s32, actor);
  gabi::call<void>(0x024D9520, actor, 0);
  return 1;
}
VERIFY(0x024DA00C, warpgn_actWait);
s32 warpgn_actWarp(Actor *actor) {
  WWHD_FUNC(0x024DA03C, s32, actor);
  gabi::call<void>(0x024D9520, actor, 0);
  return 1;
}
VERIFY(0x024DA03C, warpgn_actWarp);
s32 warpgn_actWarpArrive(Actor *actor) {
  WWHD_FUNC(0x024DA0D4, s32, actor);
  gabi::call<void>(0x024D9520, actor, 0);
  return 1;
}
VERIFY(0x024DA0D4, warpgn_actWarpArrive);
s32 warpgn_actStartWarp(Actor *actor) {
  WWHD_FUNC(0x024DA208, s32, actor);
  gabi::call<void>(0x024D9520, actor, 0);
  return 1;
}
VERIFY(0x024DA208, warpgn_actStartWarp);
u32 warpgn_initWarp() {
  WWHD_FUNC(0x024DA034, u32);
  return gabi::call<u32>(0x025E1988, 0x2893);
}
VERIFY(0x024DA034, warpgn_initWarp);
void warpgn_globalDtor1() {
  WWHD_FUNC(0x024DA39C, void);
}
VERIFY(0x024DA39C, warpgn_globalDtor1);
void warpgn_globalDtor2() {
  WWHD_FUNC(0x024DA3A0, void);
}
VERIFY(0x024DA3A0, warpgn_globalDtor2);
void warpgn_emptyVirtual() {
  WWHD_FUNC(0x024DA3F8, void);
}
VERIFY(0x024DA3F8, warpgn_emptyVirtual);
s32 warpgn_isDelete() {
  WWHD_FUNC(0x024DA394, s32);
  return 1;
}
VERIFY(0x024DA394, warpgn_isDelete);
void warpgn_initAppear(Actor *actor) {
  WWHD_FUNC(0x024DA230, void, actor);
  actor->timer = 100;
}
VERIFY(0x024DA230, warpgn_initAppear);
void warpgn_actorDtor(Actor *actor, s32 flags) {
  WWHD_FUNC(0x024DA3A4, void, actor, flags);
  if (actor) {
    gabi::call<void>(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call<void>(0x0273AF40, actor);
  }
}
VERIFY(0x024DA3A4, warpgn_actorDtor);
void warpgn_emptyDtor(void *object, s32 flags) {
  WWHD_FUNC(0x024DA380, void, object, flags);
  if (object && (flags & 1))
    gabi::call<void>(0x0273AF40, object);
}
VERIFY(0x024DA380, warpgn_emptyDtor);

static void resumeEmitter(u32 emitter) {
  if (emitter)
    write<u32>(emitter + 0x254, read<u32>(emitter + 0x254) & ~1u);
}
void warpgn_initStartWarp(Actor *actor) {
  WWHD_FUNC(0x024DA160, void, actor);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call<void>(0x02543714, gabi::at<void>(play + 0x52C4), &actor->current.pos);
  u32 zero = read<u32>(0x10042548), p = actor->btk1;
  if (p)
    write<u32>(p + 4, zero);
  p = actor->btk2;
  if (p)
    write<u32>(p + 4, zero);
  resumeEmitter(actor->emitter1);
  resumeEmitter(actor->emitter2);
  resumeEmitter(actor->emitter3);
}
VERIFY(0x024DA160, warpgn_initStartWarp);
void warpgn_initWarpArrive(Actor *actor) {
  WWHD_FUNC(0x024DA064, void, actor);
  gabi::call<void>(0x024D9F58, actor);
  resumeEmitter(actor->emitter1);
  resumeEmitter(actor->emitter2);
  resumeEmitter(actor->emitter3);
  gabi::call<void>(0x025E1988, 0x2892);
}
VERIFY(0x024DA064, warpgn_initWarpArrive);
s32 warpgn_actWarpArriveEnd(Actor *actor) {
  WWHD_FUNC(0x024DA0FC, s32, actor);
  gabi::call<void>(0x024D9520, actor, 4);
  u32 animation = actor->btk2;
  if (!animation)
    return 0;
  return read<f32>(animation + 4) < read<f32>(0x10042620) ? 1 : 0;
}
VERIFY(0x024DA0FC, warpgn_actWarpArriveEnd);
s32 warpgn_actAppear(Actor *actor) {
  WWHD_FUNC(0x024DA23C, s32, actor);
  s32 done = 0;
  if (gabi::call<s32>(0x0211D2F8, &actor->timer) == 0) {
    resumeEmitter(actor->emitter1);
    resumeEmitter(actor->emitter2);
    resumeEmitter(actor->emitter3);
    actor->timer = 100;
    done = 1;
  }
  gabi::call<void>(0x024D9520, actor, 1);
  actor->appearing = 1;
  return done;
}
VERIFY(0x024DA23C, warpgn_actAppear);
s32 warpgn_delete(Actor *actor) {
  WWHD_FUNC(0x024D915C, s32, actor);
  u32 p = actor->emitter1;
  if (p) {
    u32 flags = read<u32>(p + 0x254);
    write<u32>(p + 0x5C, 0xFFFFFFFF);
    write<u32>(p + 0x254, flags | 1);
    actor->emitter1 = 0;
  }
  p = actor->emitter2;
  if (p) {
    u32 flags = read<u32>(p + 0x254);
    write<u32>(p + 0x5C, 0xFFFFFFFF);
    write<u32>(p + 0x254, flags | 1);
    actor->emitter2 = 0;
  }
  p = actor->emitter3;
  if (p) {
    u32 flags = read<u32>(p + 0x254);
    write<u32>(p + 0x5C, 0xFFFFFFFF);
    write<u32>(p + 0x254, flags | 1);
    actor->emitter3 = 0;
  }
  gabi::call<void>(0x025204C8, &actor->phase, STR(0x10042634));
  return 1;
}
VERIFY(0x024D915C, warpgn_delete);
s32 warpgn_checkValidWarp(Actor *actor) {
  WWHD_FUNC(0x024D8D6C, s32, actor);
  s8 room = actor->home.roomNo;
  u32 save = read<u32>(0x101F84DC), bit = actor->switchNo;
  if (gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), bit, room))
    return 1;
  save = read<u32>(0x101F84DC);
  return gabi::call<s32>(0x025B8B94, gabi::at<void>(save + 0x644), 0x3D02) != 0 ? 1 : 0;
}
VERIFY(0x024D8D6C, warpgn_checkValidWarp);

static void playAnimation(Actor *actor, u32 field, u32 speed) {
  u32 p = read<u32>(gabi::ea(actor) + field);
  if (p) {
    write<u32>(p, speed);
    p = read<u32>(gabi::ea(actor) + field);
    gabi::call<s32>(0x025E742C, gabi::at<void>(p));
  }
}
static void endAnimation(u32 p) {
  if (p)
    write<f32>(p + 4, f32(read<s16>(p + 0xA)));
}
void warpgn_setEndAnim(Actor *actor) {
  WWHD_FUNC(0x024D9F58, void, actor);
  endAnimation(actor->btk2);
  endAnimation(actor->brk);
  endAnimation(actor->bck);
}
VERIFY(0x024D9F58, warpgn_setEndAnim);
void warpgn_animPlay(Actor *actor, s32 mode) {
  WWHD_FUNC(0x024D9520, void, actor, mode);
  u32 speed = read<u32>(0x10042544);
  if (mode == 0) {
    playAnimation(actor, 0x3B8, speed);
    playAnimation(actor, 0x3BC, speed);
  } else if (mode == 1) {
    playAnimation(actor, 0x3B8, speed);
    playAnimation(actor, 0x3C4, speed);
    playAnimation(actor, 0x3C0, speed);
  } else if (mode == 2) {
    playAnimation(actor, 0x3B8, speed);
    endAnimation(actor->brk);
    endAnimation(actor->bck);
  } else if (mode == 3)
    playAnimation(actor, 0x3B8, speed);
  else if (mode == 4) {
    playAnimation(actor, 0x3B8, speed);
    u32 p = actor->btk2;
    if (p) {
      write<u32>(p, read<u32>(0x100425D0));
      p = actor->btk2;
      gabi::call<s32>(0x025E742C, gabi::at<void>(p));
    }
  }
}
VERIFY(0x024D9520, warpgn_animPlay);
void warpgn_eventOrder(Actor *actor) {
  WWHD_FUNC(0x024D9CA4, void, actor);
  s32 state = actor->eventState;
  if (state == 2 || state == 1) {
    s16 event = state == 2 ? s16(actor->warpEvent) : s16(actor->appearEvent);
    gabi::call<s32>(0x025D7A58, actor, event, 0xFF, 0xFFFF, 0, 1);
    u32 ea = gabi::ea(actor) + 0xFA;
    write<u16>(ea, read<u16>(ea) | 2);
  }
}
VERIFY(0x024D9CA4, warpgn_eventOrder);
s32 warpgn_demoExecute(Actor *actor) {
  WWHD_FUNC(0x024D9D3C, s32, actor);
  u8 id = actor->demoActorID;
  if (id && id <= 0x20) {
    u32 object = read<u32>(0x101D5FFC);
    if (!object) {
      gabi::call<void>(0x0273AA24, STR(0x10042538), 0x23A, STR(0x10042528));
      object = read<u32>(0x101D5FFC);
    }
    u32 demo = gabi::call<u32>(0x02526E70, gabi::at<void>(object), id);
    if (demo) {
      u32 shape = read<u32>(demo + 0x28);
      actor->demoShape = shape;
      if (shape == 0)
        gabi::call<void>(0x024D9520, actor, 0);
      else if (shape == 1)
        gabi::call<void>(0x024D9520, actor, 1);
    }
  }
  return 1;
}
VERIFY(0x024D9D3C, warpgn_demoExecute);
s32 warpgn_create(Actor *actor) {
  WWHD_FUNC(0x024D90A0, s32, actor);
  u32 condition = actor->actor_condition;
  if (!(condition & 8)) {
    if (actor) {
      gabi::call<u32>(0x024D8BB4, actor);
      condition = actor->actor_condition;
    }
    actor->actor_condition = condition | 8;
  }
  s32 phase = gabi::call<s32>(0x02520460, &actor->phase, STR(0x10042634));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, actor, gabi::at<void>(0x024D8BB0), 0x3000))
      return 5;
    gabi::call<void>(0x024D8DE8, actor);
  }
  return phase;
}
VERIFY(0x024D90A0, warpgn_create);

void warpgn_staticInit() {
  WWHD_FUNC(0x024DA2EC, void);
  write<u32>(0x1046EB5C, 0);
  write<u32>(0x1046EB54, 0);
  write<u32>(0x1046EB60, 0);
  write<u32>(0x1046EB58, 0);
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101D30B4));
  u32 lo = read<u32>(0x10042628), hi = read<u32>(0x1004262C);
  write<u32>(0x1046EB48, lo);
  write<u32>(0x1046EB4C, hi);
  gabi::call<void>(0x028ED6F8, gabi::at<void>(0x1046EB50));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101D30C0));
  gabi::call<void>(0x028EAB2C, gabi::at<void>(0x1046EB51));
  gabi::call<void>(0x028F026C, gabi::at<void>(0x101D30CC));
}
VERIFY(0x024DA2EC, warpgn_staticInit);
void warpgn_colorConvert(void *destination, void *source) {
  WWHD_FUNC(0x024D9200, void, destination, source);
  u32 src = gabi::ea(source), dst = gabi::ea(destination);
  u8 r = read<u8>(src), g = read<u8>(src + 1), b = read<u8>(src + 2), a = read<u8>(src + 3);
  f32 divisor = read<f32>(0x100425BC);
  f32 rf = f32(r) / divisor, gf = f32(g) / divisor, bf = f32(b) / divisor, af = f32(a) / divisor;
  write<f32>(dst, rf);
  write<f32>(dst + 4, gf);
  write<f32>(dst + 8, bf);
  write<f32>(dst + 12, af);
}
VERIFY(0x024D9200, warpgn_colorConvert);
s32 warpgn_normalExecute(Actor *actor) {
  WWHD_FUNC(0x024D9880, s32, actor);
  s8 room = actor->home.roomNo;
  u32 save = read<u32>(0x101F84DC), bit = actor->switchNo;
  s32 switched = gabi::call<s32>(0x025BA0C0, gabi::at<void>(save + 0x20), bit, room);
  save = read<u32>(0x101F84DC);
  if (!gabi::call<s32>(0x025B8B94, gabi::at<void>(save + 0x644), 0x3D02)) {
    if (switched && !actor->isSwitch)
      actor->eventState = 1;
  } else {
    gabi::call<void>(0x024D9520, actor, 2);
    resumeEmitter(actor->emitter1);
    resumeEmitter(actor->emitter2);
    resumeEmitter(actor->emitter3);
  }
  if (gabi::call<s32>(0x024D9760, actor)) {
    u32 play = gabi::call<u32>(0x025200D4);
    if (!read<u8>(play + 0x5292))
      actor->eventState = 2;
  }
  actor->isSwitch = u8(switched);
  return 1;
}
VERIFY(0x024D9880, warpgn_normalExecute);
s32 warpgn_execute(Actor *actor) {
  WWHD_FUNC(0x024D9E00, s32, actor);
  if (!actor->demoActorID) {
    gabi::call<void>(0x024D999C, actor);
    gabi::call<void>(0x024D9AE0, actor);
    gabi::call<void>(0x024D9CA4, actor);
  } else {
    actor->eventState = 0;
    gabi::call<s32>(0x024D9D3C, actor);
  }
  bool sounding = actor->appearing != 0;
  if (!sounding) {
    u32 save = read<u32>(0x101F84DC);
    sounding = gabi::call<s32>(0x025B8B94, gabi::at<void>(save + 0x644), 0x3D02) != 0;
  }
  if (sounding) {
    s32 reverb = gabi::call<s32>(0x02520540, s8(actor->current.roomNo));
    gabi::call<void>(0x025E1A40, 0x1089, &actor->eyePos, 0, reverb);
  }
  u32 model = actor->model, base = gabi::ea(actor);
  f32 z = read<f32>(base + 0x338), x = read<f32>(base + 0x330), y = read<f32>(base + 0x334);
  write<f32>(model + 0xBC, x);
  write<f32>(model + 0xC4, z);
  write<f32>(model + 0xC0, y);
  f32 px = actor->current.pos.x, py = actor->current.pos.y, pz = actor->current.pos.z;
  gabi::call<void>(0x028E93CC, gabi::at<void>(0x1048D0CC), px, py, pz);
  u32 m = 0x1048D0CC;
  f32 a3 = read<f32>(m + 12), a1 = read<f32>(m + 4), a4 = read<f32>(m + 16), a7 = read<f32>(m + 28),
      a10 = read<f32>(m + 40), a5 = read<f32>(m + 20), a2 = read<f32>(m + 8),
      a8 = read<f32>(m + 32), a0 = read<f32>(m);
  model = actor->model;
  f32 a9 = read<f32>(m + 36), a11 = read<f32>(m + 44), a6 = read<f32>(m + 24);
  write<f32>(model + 0xF4, a11);
  write<f32>(model + 0xE8, a8);
  write<f32>(model + 0xC8, a0);
  write<f32>(model + 0xDC, a5);
  write<f32>(model + 0xCC, a1);
  write<f32>(model + 0xD8, a4);
  write<f32>(model + 0xE0, a6);
  write<f32>(model + 0xEC, a9);
  write<f32>(model + 0xF0, a10);
  write<f32>(model + 0xD4, a3);
  write<f32>(model + 0xE4, a7);
  write<f32>(model + 0xD0, a2);
  return 1;
}
VERIFY(0x024D9E00, warpgn_execute);

s32 warpgn_checkWarp(Actor *actor) {
  WWHD_FUNC(0x024D9760, s32, actor);
  struct Frame {
    u8 prefix[8];
    cXyz playerXZ, shipXZ, playerDelta, shipDelta;
    u8 saved[24];
  };
  static_assert(sizeof(Frame) == 0x50);
  gabi::Local<Frame> frame;
  gabi::Local<be<u32>[4]> linkage;
  u32 play = gabi::call<u32>(0x025200D4), player = read<u32>(play + 0x5B2C);
  gabi::call<void>(0x0201ADE0, gabi::at<void>(player + 0x314), &frame->playerDelta,
                   &actor->current.pos);
  f32 zero = read<f32>(0x10042548);
  write<f32>(gabi::ea(&frame->playerXZ), read<f32>(gabi::ea(&frame->playerDelta)));
  write<f32>(gabi::ea(&frame->playerXZ) + 4, zero);
  write<f32>(gabi::ea(&frame->playerXZ) + 8, read<f32>(gabi::ea(&frame->playerDelta) + 8));
  f32 length = gabi::call<f32>(0x028E8DD0, &frame->playerXZ);
  gabi::call<f32>(0x028F4384, length);
  if (!gabi::call<s32>(0x024D8D6C, actor))
    return 0;
  play = gabi::call<u32>(0x025200D4);
  if (!(read<u32>(play + 0x5CD8) & 0x10000))
    return 0;
  play = gabi::call<u32>(0x025200D4);
  if (!read<u32>(play + 0x5B3C))
    return 0;
  play = gabi::call<u32>(0x025200D4);
  u32 ship = read<u32>(play + 0x5B3C);
  gabi::call<void>(0x0201ADE0, gabi::at<void>(ship + 0x314), &frame->shipDelta,
                   &actor->current.pos);
  f32 x = read<f32>(gabi::ea(&frame->shipDelta)), z = read<f32>(gabi::ea(&frame->shipDelta) + 8);
  write<f32>(gabi::ea(&frame->shipXZ), x);
  write<f32>(gabi::ea(&frame->shipXZ) + 4, zero);
  write<f32>(gabi::ea(&frame->shipXZ) + 8, z);
  length = gabi::call<f32>(0x028E8DD0, &frame->shipXZ);
  f32 distance = gabi::call<f32>(0x028F4384, length);
  return distance < read<f32>(0x100425D4) ? 1 : 0;
}
VERIFY(0x024D9760, warpgn_checkWarp);

static u32 eventManager() {
  return gabi::call<u32>(0x025200D4) + 0x52C4;
}
void warpgn_checkOrder(Actor *actor) {
  WWHD_FUNC(0x024D999C, void, actor);
  if (read<u16>(gabi::ea(actor) + 0xF8) == 2) {
    s16 event = actor->warpEvent;
    u32 manager = eventManager();
    if (gabi::call<s32>(0x0254407C, gabi::at<void>(manager), event) && actor->eventState)
      actor->eventState = 0;
    event = actor->appearEvent;
    manager = eventManager();
    if (gabi::call<s32>(0x0254407C, gabi::at<void>(manager), event) && actor->eventState)
      actor->eventState = 0;
    event = actor->warpEvent;
    manager = eventManager();
    if (gabi::call<s32>(0x025440C8, gabi::at<void>(manager), event)) {
      s8 room = actor->current.roomNo;
      u8 scene = read<u8>(gabi::ea(actor) + 0x3DF);
      gabi::call<void>(0x02587EFC, scene, room);
    }
    event = actor->appearEvent;
    manager = eventManager();
    if (gabi::call<s32>(0x025440C8, gabi::at<void>(manager), event)) {
      u32 save = read<u32>(0x101F84DC);
      gabi::call<void>(0x025B8B68, gabi::at<void>(save + 0x644), 0x3D02);
      actor->appearEvent = -1;
      u32 play = gabi::call<u32>(0x025200D4);
      write<u16>(play + 0x52B8, read<u16>(play + 0x52B8) | 8);
    }
  } else if (!actor->eventState)
    gabi::call<s32>(0x024D9880, actor);
}
VERIFY(0x024D999C, warpgn_checkOrder);
static s32 eventMethod(Actor *actor, u32 descriptor, s32 staff) {
  s16 index = read<s16>(descriptor + 2), adjustment = read<s16>(descriptor);
  u32 receiver = gabi::ea(actor) + s32(adjustment), target;
  if (index < 0)
    target = read<u32>(descriptor + 4);
  else {
    u32 table = read<u32>(receiver + s32(read<s16>(descriptor + 6)));
    target = read<u32>(table + u32(index) * 8 + 4);
  }
  return gabi::call_ptr<s32>(target, gabi::at<void>(receiver), staff);
}
void warpgn_demoProc(Actor *actor) {
  WWHD_FUNC(0x024D9AE0, void, actor);
  u32 manager = eventManager();
  actor->staffId = gabi::call<s32>(0x02542D88, gabi::at<void>(manager), STR(0x100425D8), 0, 0);
  u32 play = gabi::call<u32>(0x025200D4);
  if (!read<u8>(play + 0x5292) || read<u16>(gabi::ea(actor) + 0xF8) == 1)
    return;
  s32 staff = actor->staffId;
  if (staff == -1)
    return;
  manager = eventManager();
  s32 action = gabi::call<s32>(0x02542EDC, gabi::at<void>(manager), staff,
                               gabi::at<void>(0x101D309C), 6, 0, 0);
  staff = actor->staffId;
  if (action == -1) {
    manager = eventManager();
    gabi::call<void>(0x02543280, gabi::at<void>(manager), staff);
    return;
  }
  manager = eventManager();
  u32 displacement = u32(action) * 8;
  if (gabi::call<s32>(0x025447C8, gabi::at<void>(manager), staff))
    eventMethod(actor, 0x101D301C + displacement, actor->staffId);
  if (eventMethod(actor, 0x101D304C + displacement, actor->staffId)) {
    staff = actor->staffId;
    manager = eventManager();
    gabi::call<void>(0x02543280, gabi::at<void>(manager), staff);
  }
}
VERIFY(0x024D9AE0, warpgn_demoProc);

Actor *warpgn_construct(Actor *actor) {
  WWHD_FUNC(0x024D8BB4, Actor *, actor);
  if (!actor) {
    actor = gabi::at<Actor>(gabi::call<u32>(0x0273AD10, 0x5C0));
    if (!actor)
      return nullptr;
  }
  gabi::call<void>(0x025D4ED0, actor);
  u32 destination = gabi::ea(actor);
  write<u32>(destination + 0xB4, 0x10042518);
  // Cache each default-lighting component at its original load point, using lfs conversion for
  // floating components.
  const f32 lighting0 = read<f32>(0x1016E414 + 0x0);
  write<f32>(destination + 0x3f4, lighting0);
  const f32 lighting1 = read<f32>(0x1016E414 + 0x4);
  write<f32>(destination + 0x3f8, lighting1);
  const f32 lighting2 = read<f32>(0x1016E414 + 0x8);
  write<f32>(destination + 0x3fc, lighting2);
  const f32 lighting3 = read<f32>(0x1016E414 + 0xC);
  write<f32>(destination + 0x400, lighting3);
  const f32 lighting4 = read<f32>(0x1016E414 + 0x10);
  write<f32>(destination + 0x404, lighting4);
  const f32 lighting5 = read<f32>(0x1016E414 + 0x14);
  write<f32>(destination + 0x408, lighting5);
  const u8 lighting6 = read<u8>(0x1016E414 + 0x18);
  write<u8>(destination + 0x40c, lighting6);
  const u8 lighting7 = read<u8>(0x1016E414 + 0x19);
  write<u8>(destination + 0x40d, lighting7);
  const u8 lighting8 = read<u8>(0x1016E414 + 0x1A);
  write<u8>(destination + 0x40e, lighting8);
  const u8 lighting9 = read<u8>(0x1016E414 + 0x1B);
  write<u8>(destination + 0x40f, lighting9);
  const s16 lighting10 = read<s16>(0x1016E414 + 0x1C);
  write<s16>(destination + 0x410, lighting10);
  const s16 lighting11 = read<s16>(0x1016E414 + 0x1E);
  write<s16>(destination + 0x412, lighting11);
  const s16 lighting12 = read<s16>(0x1016E414 + 0x20);
  write<s16>(destination + 0x414, lighting12);
  const s16 lighting13 = read<s16>(0x1016E414 + 0x22);
  write<s16>(destination + 0x416, lighting13);
  const f32 lighting14 = read<f32>(0x1016E414 + 0x24);
  write<f32>(destination + 0x418, lighting14);
  const f32 lighting15 = read<f32>(0x1016E414 + 0x28);
  write<f32>(destination + 0x41c, lighting15);
  const f32 lighting16 = read<f32>(0x1016E414 + 0x2C);
  write<f32>(destination + 0x420, lighting16);
  const f32 lighting17 = read<f32>(0x1016E414 + 0x30);
  write<f32>(destination + 0x424, lighting17);
  const f32 lighting18 = read<f32>(0x1016E414 + 0x34);
  write<f32>(destination + 0x428, lighting18);
  const f32 lighting19 = read<f32>(0x1016E414 + 0x38);
  write<f32>(destination + 0x42c, lighting19);
  const f32 lighting20 = read<f32>(0x1016E414 + 0x3C);
  write<f32>(destination + 0x430, lighting20);
  const f32 lighting21 = read<f32>(0x1016E414 + 0x40);
  write<f32>(destination + 0x56c, lighting18);
  write<f32>(destination + 0x4f4, lighting21);
  write<s16>(destination + 0x556, lighting11);
  write<f32>(destination + 0x434, lighting21);
  write<f32>(destination + 0x4c0, lighting3);
  write<s16>(destination + 0x4d2, lighting11);
  write<f32>(destination + 0x4b8, lighting1);
  write<f32>(destination + 0x538, lighting0);
  write<f32>(destination + 0x568, lighting17);
  write<f32>(destination + 0x4c4, lighting4);
  write<s16>(destination + 0x558, lighting12);
  write<u8>(destination + 0x550, lighting6);
  write<f32>(destination + 0x4e4, lighting17);
  write<f32>(destination + 0x53c, lighting1);
  write<f32>(destination + 0x4dc, lighting15);
  write<f32>(destination + 0x574, lighting20);
  write<u8>(destination + 0x4ce, lighting8);
  write<f32>(destination + 0x4c8, lighting5);
  write<s16>(destination + 0x4d6, lighting13);
  write<u8>(destination + 0x553, lighting9);
  write<f32>(destination + 0x540, lighting2);
  write<f32>(destination + 0x560, lighting15);
  write<f32>(destination + 0x4ec, lighting19);
  write<f32>(destination + 0x4b4, lighting0);
  write<s16>(destination + 0x4d0, lighting10);
  write<u8>(destination + 0x4cc, lighting6);
  write<u8>(destination + 0x552, lighting8);
  write<f32>(destination + 0x4bc, lighting2);
  write<f32>(destination + 0x548, lighting4);
  write<f32>(destination + 0x55c, lighting14);
  write<f32>(destination + 0x578, lighting21);
  write<f32>(destination + 0x570, lighting19);
  write<s16>(destination + 0x55a, lighting13);
  write<f32>(destination + 0x544, lighting3);
  write<u8>(destination + 0x4cf, lighting9);
  write<s16>(destination + 0x554, lighting10);
  write<s16>(destination + 0x4d4, lighting12);
  write<f32>(destination + 0x4e0, lighting16);
  write<u8>(destination + 0x4cd, lighting7);
  write<f32>(destination + 0x4e8, lighting18);
  write<f32>(destination + 0x564, lighting16);
  write<f32>(destination + 0x4f0, lighting20);
  write<u8>(destination + 0x551, lighting7);
  write<f32>(destination + 0x4d8, lighting14);
  write<f32>(destination + 0x54c, lighting5);
  return actor;
}
VERIFY(0x024D8BB4, warpgn_construct);
void warpgn_CreateInit(Actor *actor) {
  WWHD_FUNC(0x024D8DE8, void, actor);
  u32 ea = gabi::ea(actor), model = actor->model;
  write<u32>(ea + 0x348, model ? model + 0xC8 : 0);
  gabi::call<void>(0x025D674C, actor, read<f32>(0x10042594), read<f32>(0x10042548),
                   read<f32>(0x10042594), read<f32>(0x10042598), read<f32>(0x1004259C),
                   read<f32>(0x10042598));
  model = actor->model;
  f32 scaleZ = read<f32>(ea + 0x338);
  write<u32>(ea + 0x364, read<u32>(0x10042544));
  f32 scaleX = read<f32>(ea + 0x330), scaleY = read<f32>(ea + 0x334);
  write<f32>(model + 0xC4, scaleZ);
  write<f32>(model + 0xBC, scaleX);
  write<f32>(model + 0xC0, scaleY);
  gabi::call<void>(0x028E93CC, gabi::at<void>(0x1048D0CC), f32(actor->current.pos.x),
                   f32(actor->current.pos.y), f32(actor->current.pos.z));
  f32 m24 = read<f32>(0x1048D0F0), m2C = read<f32>(0x1048D0F8), m20 = read<f32>(0x1048D0EC),
      m04 = read<f32>(0x1048D0D0), m1C = read<f32>(0x1048D0E8), m00 = read<f32>(0x1048D0CC),
      m10 = read<f32>(0x1048D0DC), m0C = read<f32>(0x1048D0D8), m28 = read<f32>(0x1048D0F4),
      m14 = read<f32>(0x1048D0E0), m18 = read<f32>(0x1048D0E4);
  model = actor->model;
  f32 m08 = read<f32>(0x1048D0D4);
  write<f32>(model + 0xE4, m1C);
  write<f32>(model + 0xDC, m14);
  write<f32>(model + 0xE0, m18);
  write<f32>(model + 0xD4, m0C);
  write<f32>(model + 0xF4, m2C);
  write<f32>(model + 0xF0, m28);
  write<f32>(model + 0xE8, m20);
  write<f32>(model + 0xC8, m00);
  write<f32>(model + 0xEC, m24);
  write<f32>(model + 0xD0, m08);
  write<f32>(model + 0xD8, m10);
  write<f32>(model + 0xCC, m04);
  actor->emitter1 =
      gabi::call<u32>(0x025A847C, gabi::at<void>(read<u32>(gabi::call<u32>(0x025200D4) + 0x5AB0)),
                      0, 0x83FD, &actor->current.pos, 0, 0, 0xFF, 0, -1, 0, 0, 0);
  actor->emitter2 =
      gabi::call<u32>(0x025A847C, gabi::at<void>(read<u32>(gabi::call<u32>(0x025200D4) + 0x5AB0)),
                      0, 0x83FE, &actor->current.pos, 0, 0, 0xFF, 0, -1, 0, 0, 0);
  actor->emitter3 =
      gabi::call<u32>(0x025A847C, gabi::at<void>(read<u32>(gabi::call<u32>(0x025200D4) + 0x5AB0)),
                      4, 0xC3FC, &actor->current.pos, 0, 0, 0xFF, 0, -1, 0, 0, 0);
  u32 p = actor->emitter1;
  if (p)
    write<u32>(p + 0x254, read<u32>(p + 0x254) | 1);
  p = actor->emitter2;
  if (p)
    write<u32>(p + 0x254, read<u32>(p + 0x254) | 1);
  p = actor->emitter3;
  if (p)
    write<u32>(p + 0x254, read<u32>(p + 0x254) | 1);
  actor->warpEvent =
      gabi::call<s32>(0x02543F10, gabi::at<void>(gabi::call<u32>(0x025200D4) + 0x52C4),
                      gabi::at<void>(0x100425AC), 0xFF);
  u32 appear = gabi::call<u32>(0x02543F10, gabi::at<void>(gabi::call<u32>(0x025200D4) + 0x52C4),
                               gabi::at<void>(0x100425A0), 0xFF);
  u32 parameters = read<u32>(ea + 0xB0);
  actor->appearEvent = appear;
  actor->switchNo = (parameters >> 8) & 0xFF;
  s32 valid = gabi::call<s32>(0x024D8D6C, actor);
  s8 room = read<s8>(ea + 0x326);
  u8 scene = read<u8>(ea + 0xB3);
  if (valid) {
    actor->isSwitch = 1;
    actor->appearEvent = -1;
  }
  actor->scene = scene;
  gabi::call<void>(0x0255FFF4, gabi::at<void>(ea + 0x3F4), room, 0xFF);
}
VERIFY(0x024D8DE8, warpgn_CreateInit);
s32 warpgn_CreateHeap(Actor *actor) {
  WWHD_FUNC(0x024D885C, s32, actor);
  struct SafeName {
    be<u32> text, vtable;
  };
  struct Names {
    u8 prefix[0x14];
    SafeName model, btk1, btk2, brk, bck;
    u8 tail[0x44];
  };
  static_assert(sizeof(Names) == 0x80);
  static_assert(offsetof(Names, model) == 0x14);
  gabi::Local<Names> names;
  gabi::Local<be<u32>[4]> outgoing;
  names->model.vtable = 0x100424D8;
  names->model.text = 0x10042634;
  u32 data = gabi::call<u32>(0x026066C4, gabi::at<void>(read<u32>(0x101F4F28)), &names->model, 9);
  if (!data)
    gabi::call<void>(0x0273AA24, gabi::at<void>(0x10042570), 0xCF, gabi::at<void>(0x10042580));
  u32 model = gabi::call<u32>(0x025E38E0, gabi::at<void>(data), 0x80000, 0x11000222);
  actor->model = model;
  if (!model)
    return 0;
  names->btk1.vtable = 0x100424D8;
  names->btk1.text = 0x10042634;
  u32 resource =
      gabi::call<u32>(0x026066C4, gabi::at<void>(read<u32>(0x101F4F28)), &names->btk1, 0xF);
  if (!resource)
    gabi::call<void>(0x0273AA24, gabi::at<void>(0x10042570), 0xDE, gabi::at<void>(0x1004254C));
  u32 animation = gabi::call<u32>(0x025E7C6C, 0);
  actor->btk1 = animation;
  if (!animation)
    return 0;
  f32 speed = read<f32>(0x10042544);
  if (!gabi::call<s32>(0x025E7CE0, gabi::at<void>(animation), gabi::at<void>(data),
                       gabi::at<void>(resource), 1, 2, 0, -1, 0, speed, 0))
    return 0;
  write<f32>(u32(actor->btk1), speed);
  names->btk2.vtable = 0x100424D8;
  names->btk2.text = 0x10042634;
  resource = gabi::call<u32>(0x026066C4, gabi::at<void>(read<u32>(0x101F4F28)), &names->btk2, 0x10);
  if (!resource)
    gabi::call<void>(0x0273AA24, gabi::at<void>(0x10042570), 0xED, gabi::at<void>(0x1004254C));
  animation = gabi::call<u32>(0x025E7C6C, 0);
  actor->btk2 = animation;
  if (!animation)
    return 0;
  if (!gabi::call<s32>(0x025E7CE0, gabi::at<void>(animation), gabi::at<void>(data),
                       gabi::at<void>(resource), 1, 0, 0, -1, 0, speed, 0))
    return 0;
  u32 zero = read<u32>(0x10042548);
  write<u32>(u32(actor->btk2), zero);
  names->brk.vtable = 0x100424D8;
  names->brk.text = 0x10042634;
  resource = gabi::call<u32>(0x026066C4, gabi::at<void>(read<u32>(0x101F4F28)), &names->brk, 0xC);
  if (!resource)
    gabi::call<void>(0x0273AA24, gabi::at<void>(0x10042570), 0xFF, gabi::at<void>(0x10042558));
  animation = gabi::call<u32>(0x025E80D0, 0);
  actor->brk = animation;
  if (!animation)
    return 0;
  if (!gabi::call<s32>(0x025E8154, gabi::at<void>(animation), gabi::at<void>(data),
                       gabi::at<void>(resource), 1, 0, 0, -1, 0, speed, 0))
    return 0;
  write<u32>(u32(actor->brk), zero);
  names->bck.vtable = 0x100424D8;
  names->bck.text = 0x10042634;
  resource = gabi::call<u32>(0x026066C4, gabi::at<void>(read<u32>(0x101F4F28)), &names->bck, 6);
  if (!resource)
    gabi::call<void>(0x0273AA24, gabi::at<void>(0x10042570), 0x10F, gabi::at<void>(0x10042564));
  animation = gabi::call<u32>(0x0273AD10, 0x8C);
  if (animation) {
    gabi::call<void>(0x027F2BC0, gabi::at<void>(animation), 0);
    write<u32>(animation + 0x10, 0x1016E54C);
    gabi::call<void>(0x027DA984, gabi::at<void>(animation + 0x14));
    write<u32>(animation + 0x88, 0);
    write<u32>(animation + 0x7C, 0);
    write<u32>(animation + 0x48, 0x1016D820);
    write<u32>(animation + 0x58, 0);
    write<u32>(animation + 0x80, 0);
    write<u32>(animation + 0x10, 0x100424F0);
    write<u32>(animation + 0x84, 0);
  }
  actor->bck = animation;
  if (!animation)
    return 0;
  if (!gabi::call<s32>(0x025E8508, gabi::at<void>(animation), gabi::at<void>(data),
                       gabi::at<void>(resource), 1, 0, 0, -1, 0, speed))
    return 0;
  write<u32>(u32(actor->bck), zero);
  return 1;
}
VERIFY(0x024D885C, warpgn_CreateHeap);
static u32 materialColor(u32 material) {
  u32 block = read<u32>(material + 0x18), vtable = read<u32>(block + 4);
  return gabi::call_ptr<u32>(read<u32>(vtable + 0x4C), gabi::at<void>(block), 1);
}
s32 warpgn_draw(Actor *actor) {
  WWHD_FUNC(0x024D92B4, s32, actor);
  u32 ea = gabi::ea(actor);
  struct ColorFrame {
    u8 prefix[0x18];
    be<u32> rgb[4];
    be<f32> rgba[4];
    u8 tail[0x28];
  };
  static_assert(sizeof(ColorFrame) == 0x60);
  static_assert(offsetof(ColorFrame, rgb) == 0x18);
  static_assert(offsetof(ColorFrame, rgba) == 0x28);
  gabi::Local<ColorFrame> frame;
  gabi::Local<be<u32>[4]> outgoing;
  u32 environment = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x025626A4, gabi::at<void>(environment), 0, &actor->current.pos,
                   gabi::at<void>(ea + 0x110));
  environment = gabi::call<u32>(0x02555D0C);
  u32 model = actor->model;
  gabi::call<void>(0x02562F5C, gabi::at<void>(environment), gabi::at<void>(model),
                   gabi::at<void>(ea + 0x110));
  environment = gabi::call<u32>(0x02555D0C);
  gabi::call<void>(0x025626A4, gabi::at<void>(environment), 2, &actor->current.pos,
                   gabi::at<void>(ea + 0x3F4));
  model = actor->model;
  u32 data = read<u32>(model + 0xAC), table = gabi::call<u32>(0x027F3F8C, gabi::at<void>(data));
  u16 index = 0;
  if (index < read<u16>(table + 0x24)) {
    f32 intensity = read<f32>(0x10042544);
    do {
      u32 count = read<u32>(data + 0xC), material = read<u32>(data + 0x10);
      if (index < count)
        material += u32(index) * 0x39C;
      u32 color = materialColor(material);
      write<u8>(color, read<u8>(ea + 0x48C));
      color = materialColor(material);
      write<u8>(color + 1, read<u8>(ea + 0x48D));
      color = materialColor(material);
      write<u8>(color + 2, read<u8>(ea + 0x48E));
      color = materialColor(material);
      u32 block = read<u32>(material + 0x18), vtable = read<u32>(block + 4);
      gabi::call_ptr<void>(read<u32>(vtable + 0x3C), gabi::at<void>(block), 1,
                           gabi::at<void>(color));
      gabi::call<void>(0x024D9200, &frame->rgba, gabi::at<void>(color));
      gabi::call<void>(0x0274D458, &frame->rgb, &frame->rgba, intensity);
      write<u32>(material + 0xA0, read<u32>(material + 0xA0) | 0x100);
      u32 output = gabi::call<u32>(0x027F9F0C, gabi::at<void>(material + 0xA0), 8);
      f32 alpha = f32(read<u8>(color + 3)) / read<f32>(0x100425BC);
      f32 red = read<f32>(gabi::ea(&frame->rgb[0])), green = read<f32>(gabi::ea(&frame->rgb[1])),
          blue = read<f32>(gabi::ea(&frame->rgb[2]));
      write<f32>(output + 4, green);
      write<f32>(output + 8, blue);
      write<f32>(output, red);
      index = u16(index + 1);
      write<f32>(output + 0xC, alpha);
      table = gabi::call<u32>(0x027F3F8C, gabi::at<void>(data));
    } while (index < read<u16>(table + 0x24));
  }
  u32 animation = actor->btk1;
  if (animation) {
    f32 time = read<f32>(animation + 4);
    model = actor->model;
    gabi::call<void>(0x025E8048, gabi::at<void>(animation), gabi::at<void>(model), time);
  }
  animation = actor->btk2;
  if (animation) {
    f32 time = read<f32>(animation + 4);
    model = actor->model;
    gabi::call<void>(0x025E8048, gabi::at<void>(animation), gabi::at<void>(model), time);
  }
  animation = actor->brk;
  if (animation) {
    model = actor->model;
    f32 time = read<f32>(animation + 4);
    gabi::call<void>(0x025E83FC, gabi::at<void>(animation), gabi::at<void>(read<u32>(model + 0xAC)),
                     time);
  }
  animation = actor->bck;
  if (animation) {
    model = actor->model;
    f32 time = read<f32>(animation + 4);
    gabi::call<void>(0x025E86B8, gabi::at<void>(animation), gabi::at<void>(read<u32>(model + 0xAC)),
                     time);
  }
  gabi::call<void>(0x025E2DE0, gabi::at<void>(u32(actor->model)), 0);
  return 1;
}
VERIFY(0x024D92B4, warpgn_draw);

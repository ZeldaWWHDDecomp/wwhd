// Tower of the Gods gate. Ported from the HD implementation; GC source is
// stubbed.
#include "d/actor/d_a_obj_htetu1.h"
#include "bindings.h"
#include <cmath>
namespace {
template <class T> T read(u32 a, u32 o = 0) { return *gabi::at<be<T>>(a + o); }
template <class T> void write(u32 a, u32 o, T v) {
  *gabi::at<be<T>>(a + o) = v;
}
void *ptr(u32 a, u32 o = 0) { return gabi::at<void>(a + o); }
u32 play() { return gabi::call<u32>(0x025200D4); }
struct Archive {
  be<u32> name, vt;
};
u32 resource(s32 index) {
  gabi::Local<Archive> name;
  name->name = 0x1002B0A0;
  name->vt = 0x1002AF88;
  u32 ctl = read<u32>(0x101F4F28);
  return gabi::call<u32>(0x026066C4, ptr(ctl), name.get(), index);
}
void stop(daObjHtetu1Splash_c &s) {
  u32 a = gabi::ea(&s);
  if (s.emitter) {
    u32 target = read<u32>(read<u32>(a), 0x44);
    gabi::call(target, &s);
    s.playing = 0;
  }
}
void particle(daObjHtetu1Splash_c &s, s16 timer) {
  u32 emitter = s.emitter;
  if (emitter) {
    write<u32>(emitter, 0x254, read<u32>(emitter, 0x254) & ~1u);
    s.timer = timer;
    s.playing = 1;
  }
}
} // namespace
void htetu_createSplash(daObjHtetu1Splash_c *s, u16 id, cXyz *pos, csXyz *rot,
                        void *tev) {
  WWHD_FUNC(0x0235C534, void, s, id, pos, rot, tev);
  s->position.copy(*pos);
  s->rotation.x = rot->x;
  s->rotation.y = rot->y;
  s->rotation.z = rot->z;
  u32 game = play();
  u32 particles = read<u32>(game, 0x5AB0);
  gabi::call(0x025A847C, ptr(particles), 0, (u32)id, &s->position, &s->rotation,
             0, 255, s, -1, 0, 0, 0);
  u32 emitter = s->emitter;
  if (emitter) {
    u32 light = gabi::ea(tev);
    u8 r = read<u8>(light, 0x91), b = read<u8>(light, 0x95);
    write<u8>(emitter, 0x244, r);
    u8 g = read<u8>(light, 0x93);
    write<u8>(emitter, 0x246, b);
    write<u8>(emitter, 0x245, g);
    emitter = s->emitter;
    if (emitter)
      write<u32>(emitter, 0x254, read<u32>(emitter, 0x254) | 1u);
  }
  s->playing = 0;
  s->timer = -2;
}
VERIFY(0x0235C534, htetu_createSplash);
BOOL htetu_createHeap(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235C624, BOOL, gate);
  u32 data = resource(4);
  if (!data) {
    gabi::call(0x0273AA24, ptr(0x1002B030), 0x11A, ptr(0x1002B020));
    return 0;
  }
  gate->model = gabi::call<u32>(0x025E38E0, ptr(data), 0, 0x11020203);
  u32 dzb = resource(7), model = gate->model;
  gate->background =
      gabi::call<u32>(0x024F2478, ptr(dzb), 1, ptr(model ? model + 0xC8 : 0));
  return gate->background != 0;
}
VERIFY(0x0235C624, htetu_createHeap);
BOOL htetu_heapCB(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235C6FC, BOOL, gate);
  return gabi::call<BOOL>(0x0235C624, gate);
}
VERIFY(0x0235C6FC, htetu_heapCB);
void htetu_initMatrix(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235C700, void, gate);
  J3DModel_setBaseScale(gabi::at<J3DModel>(gate->model), &gate->scale);
  mDoMtx_stack_c::transS(gate->current.pos.x, gate->current.pos.y,
                         gate->current.pos.z);
  gabi::call(0x025F1B48, mDoMtx_stack_c::get(), (s32)(s16)gate->shape_angle.x,
             (s32)(s16)gate->shape_angle.y, (s32)(s16)gate->shape_angle.z);
  J3DModel_setBaseTRMtx(gabi::at<J3DModel>(gate->model), mDoMtx_stack_c::get());
  gabi::call(0x027F4D5C, gabi::at<J3DModel>(gate->model));
}
VERIFY(0x0235C700, htetu_initMatrix);
s32 htetu_create(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235C7E8, s32, gate);
  u32 a = gabi::ea(gate);
  if (!(read<u32>(a, 0x2E4) & 8)) {
    u32 flags = read<u32>(a, 0x2E4);
    if (a) {
      gabi::call(0x025D4ED0, gate);
      write<u32>(a, 0xB4, 0x1002B010);
      gabi::call(0x028EFFD0, &gate->splash[0], 2, 0x2C, ptr(0x0235D37C));
      flags = read<u32>(a, 0x2E4);
    }
    write<u32>(a, 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, ptr(a, 0x3B0), ptr(0x1002B0A0));
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, gate, ptr(0x0235C6FC), 0xAE0))
    return 5;
  u32 model = gate->model;
  f32 height = gate->current.pos.y - 2300.0f;
  u32 sw = read<u8>(a, 0xB3);
  write<u32>(a, 0x348, model ? model + 0xC8 : 0);
  gate->switchBit = sw;
  gate->floorHeight = height;
  u32 save = read<u32>(0x101F84DC);
  s32 room = read<s8>(a, 0x2FE);
  if (gabi::call<s32>(0x025BA0C0, ptr(save, 0x20), sw, room)) {
    f32 y = gate->floorHeight;
    gate->eventState = 2;
    gate->current.pos.y = y;
  }
  gate->quakeTimer = -1;
  gabi::call(0x0235C700, gate);
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(light), 2, &gate->current.pos, ptr(a, 0x110));
  for (u32 i = 0; i < 2; ++i) {
    u16 id = read<u16>(0x1002AF84, i * 2);
    gabi::call(0x0235C534, &gate->splash[i], (u32)id, &gate->current.pos,
               ptr(a, 0x320), ptr(a, 0x110));
  }
  gabi::call(0x025D674C, gate, -950.0f, -1000.0f, -100.0f, 950.0f, 1300.0f,
             100.0f);
  u32 game = play();
  gabi::call(0x024EEA6C, ptr(game, 0x12A0), ptr(gate->background), gate);
  game = play();
  gate->eventIndex =
      gabi::call<s32>(0x02543F10, ptr(game, 0x52C4), ptr(0x1002B060), 255);
  return 4;
}
VERIFY(0x0235C7E8, htetu_create);
BOOL htetu_delete(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235C9C8, BOOL, gate);
  u32 a = gabi::ea(gate);
  if (gate->quakeTimer > 0) {
    u32 game = play();
    gabi::call(0x025CB610, ptr(game, 0x599C), -1);
    gate->quakeTimer = -1;
  }
  for (auto &s : gate->splash)
    stop(s);
  if (read<u32>(a, 0xF4) && gate->background) {
    u32 bg = gate->background;
    if (read<u32>(bg) < 256) {
      u32 game = play();
      gabi::call(0x020087EC, ptr(game, 0x12A0), ptr(gate->background));
      gate->background = 0;
    }
  }
  gabi::call(0x025204C8, ptr(a, 0x3B0), ptr(0x1002B0A0));
  return 1;
}
VERIFY(0x0235C9C8, htetu_delete);
void htetu_unlock(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235CAB8, void, gate);
  gabi::Local<cXyz> shake;
  shake->x = read<f32>(0x101FFBC0);
  shake->y = read<f32>(0x101FFBC4);
  shake->z = read<f32>(0x101FFBC8);
  gabi::call(0x028E8DAC, &gate->movingPosition, &gate->shakeOffset,
             &gate->movingPosition);
  u32 angle = ((u32)(u16)gate->unlockTimer * 0x859) & 0xFFFF;
  f32 amplitude = gate->shakeAmplitude;
  f32 sine = read<f32>(0x104A44F8, (angle >> 3) * 8);
  f32 scale = std::fabs((f32)(s16)gabi::ftoi(amplitude * sine));
  gabi::call(0x028E8E64, shake.get(), shake.get(), scale);
  gabi::call(0x028E8D88, &gate->movingPosition, shake.get(),
             &gate->movingPosition);
  gate->shakeOffset.copy(*shake);
  gabi::call(0x0200ECD4, &gate->shakeAmplitude, 0.0f, 0.13f, 50.0f, 1.0f);
}
VERIFY(0x0235CAB8, htetu_unlock);
f32 htetu_waterHeight(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235CBC4, f32, gate);
  struct Water {
    u8 bytes[80];
  };
  gabi::Local<Water> water;
  gabi::Local<cXyz> pos, offset;
  gabi::call(0x024F22DC, water.get());
  u32 a = gabi::ea(gate);
  s32 angle = read<s16>(a, 0x322);
  pos->x = read<f32>(a, 0x2EC);
  pos->z = read<f32>(a, 0x2F4);
  pos->y = read<f32>(a, 0x2F0);
  f32 height = gate->current.pos.y;
  gabi::call(0x025F1884, ptr(0x1048D0CC), angle);
  gabi::call(0x028E8F64, ptr(0x1048D0CC), ptr(0x101FFBCC), offset.get());
  gabi::call(0x028E8E64, offset.get(), offset.get(), 400.0f);
  gabi::call(0x028E8D88, pos.get(), offset.get(), pos.get());
  if (gabi::call<s32>(0x024F15A4, pos.get(), water.get(), 1.0f))
    height = read<f32>(gabi::ea(water.get()), 0x48);
  u32 w = gabi::ea(water.get());
  write<u32>(w, 0x20, 0x1002AFB0);
  write<u32>(w, 0x24, 0x1002AFD0);
  write<u32>(w, 0x30, 0x1002AFA0);
  gabi::call(0x02008B4C, ptr(w, 0x10), 0);
  return height;
}
VERIFY(0x0235CBC4, htetu_waterHeight);
void htetu_splashManager(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235CCCC, void, gate);
  f32 height = gabi::call<f32>(0x0235CBC4, gate);
  for (auto &s : gate->splash) {
    s.position.y = height;
    s16 timer = s.timer;
    if (timer < 0 && timer != -1)
      continue;
    if (timer == 0) {
      if (s.playing && s.emitter) {
        u32 e = s.emitter;
        write<u32>(e, 0x254, read<u32>(e, 0x254) | 1u);
        s.playing = 0;
      }
      continue;
    }
    bool above = gate->current.pos.y + 1400.0f > height;
    if (above) {
      if (!s.playing && s.emitter) {
        u32 e = s.emitter;
        write<u32>(e, 0x254, read<u32>(e, 0x254) & ~1u);
        s.playing = 1;
      }
    } else if (s.playing && s.emitter) {
      u32 e = s.emitter;
      write<u32>(e, 0x254, read<u32>(e, 0x254) | 1u);
      s.playing = 0;
    }
    if (timer > 0)
      s.timer = (s16)((s16)s.timer - 1);
  }
}
VERIFY(0x0235CCCC, htetu_splashManager);
BOOL htetu_execute(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235CDF0, BOOL, gate);
  u32 a = gabi::ea(gate);
  u8 state = gate->eventState;
  if (state == 0) {
    u32 save = read<u32>(0x101F84DC);
    s32 room = read<s8>(a, 0x2FE);
    u32 sw = gate->switchBit;
    if (gabi::call<s32>(0x025BA0C0, ptr(save, 0x20), sw, room)) {
      if (read<u16>(a, 0xF8) != 2) {
        s32 event = gate->eventIndex;
        gabi::call(0x025D7A58, gate, event, 255, 65535, 0, 1);
        write<u16>(a, 0xFA, read<u16>(a, 0xFA) | 2);
      } else {
        gate->unlockTimer = 70;
        gate->shakeAmplitude = 50.0f;
        gabi::Local<cXyz> shake;
        gabi::Local<cXyz> linkage; // reserve outgoing SP+4 below live vector
        gabi::call(0x0201AE48, ptr(0x101FFBC0), shake.get(), 50.0f);
        gate->shakeOffset.copy(*shake);
        gabi::call(0x025E1988, 0x806);
        gate->motionState = 1;
        gate->eventState = 1;
        u32 game = play();
        shake->x = 0;
        shake->y = 1;
        shake->z = 0;
        gabi::call(0x025CB374, ptr(game, 0x599C), 5, -33, shake.get());
        for (auto &s : gate->splash)
          particle(s, 30);
      }
    }
  } else if (state == 1) {
    s32 event = gate->eventIndex;
    u32 game = play();
    if (gabi::call<s32>(0x025440C8, ptr(game, 0x52C4), event)) {
      game = play();
      write<u16>(game, 0x52B8, read<u16>(game, 0x52B8) | 8);
      gate->eventState = 2;
    }
  }
  f32 y = gate->current.pos.y;
  gate->movingPosition.x = gate->current.pos.x;
  gate->movingPosition.y = y;
  gate->movingPosition.z = gate->current.pos.z;
  u8 motion = gate->motionState;
  if (motion == 1) {
    gabi::call(0x0235CAB8, gate);
    u16 timer = gate->unlockTimer;
    if (timer) {
      s32 room = read<s8>(a, 0x326);
      gate->unlockTimer = timer - 1;
      s32 reverb = gabi::call<s32>(0x02520540, room);
      gabi::call(0x025E1A40, 0x107B, &gate->current.pos, 0, reverb);
    } else {
      u32 game = play();
      gabi::Local<cXyz> direction;
      direction->x = 0;
      direction->y = 1;
      direction->z = 0;
      gabi::call(0x025CB408, ptr(game, 0x599C), 6, 3, direction.get());
      gate->motionState = 2;
      gate->quakeTimer = 200;
      for (auto &s : gate->splash)
        particle(s, -1);
    }
    y = gate->movingPosition.y;
  } else if (motion == 2) {
    gate->movingPosition.y = y - 5.0f;
    s32 room = read<s8>(a, 0x326);
    s32 reverb = gabi::call<s32>(0x02520540, room);
    gabi::call(0x025E1A40, 0x107B, &gate->current.pos, 0, reverb);
    y = gate->movingPosition.y;
    f32 floor = gate->floorHeight;
    if (!(y > floor)) {
      for (auto &s : gate->splash)
        stop(s);
      y = gate->floorHeight;
      gate->motionState = 0;
      gate->movingPosition.y = y;
    }
  }
  gate->current.pos.x = gate->movingPosition.x;
  gate->current.pos.y = y;
  gate->current.pos.z = gate->movingPosition.z;
  gabi::call(0x0235C700, gate);
  gabi::call(0x0235CCCC, gate);
  s16 quake = gate->quakeTimer;
  if (quake == 0) {
    u32 game = play();
    gabi::call(0x025CB610, ptr(game, 0x599C), -1);
    gate->quakeTimer = -1;
  } else if (quake > 0)
    gate->quakeTimer = quake - 1;
  if (read<u32>(a, 0xF4)) {
    u32 bg = gate->background;
    if (bg && read<u32>(bg) < 256)
      gabi::call(0x024F43DC, ptr(bg));
  }
  return 1;
}
VERIFY(0x0235CDF0, htetu_execute);
BOOL htetu_draw(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235D268, BOOL, gate);
  u32 a = gabi::ea(gate);
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(light), 1, &gate->current.pos, ptr(a, 0x110));
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(light), gabi::at<J3DModel>(gate->model),
             ptr(a, 0x110));
  gabi::call(0x025E2DE0, gabi::at<J3DModel>(gate->model), 0);
  return 1;
}
VERIFY(0x0235D268, htetu_draw);
s32 htetu_Create(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235D2C4, s32, gate);
  return gabi::call<s32>(0x0235C7E8, gate);
}
VERIFY(0x0235D2C4, htetu_Create);
BOOL htetu_Delete(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235D2C8, BOOL, gate);
  return gabi::call<BOOL>(0x0235C9C8, gate);
}
VERIFY(0x0235D2C8, htetu_Delete);
BOOL htetu_Execute(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235D2CC, BOOL, gate);
  return gabi::call<BOOL>(0x0235CDF0, gate);
}
VERIFY(0x0235D2CC, htetu_Execute);
BOOL htetu_Draw(daObjHtetu1_c *gate) {
  WWHD_FUNC(0x0235D2D0, BOOL, gate);
  return gabi::call<BOOL>(0x0235D268, gate);
}
VERIFY(0x0235D2D0, htetu_Draw);
void htetu_staticInit() {
  WWHD_FUNC(0x0235D2D4, void);
  write<u32>(0x1046A02C, 8, 0);
  write<u32>(0x1046A02C, 0, 0);
  write<u32>(0x1046A02C, 12, 0);
  write<u32>(0x1046A02C, 4, 0);
  gabi::call(0x028F026C, ptr(0x101CA2F0));
  write<f32>(0x1046A020, 0, read<f32>(0x1002B098));
  write<f32>(0x1046A024, 0, read<f32>(0x1002B09C));
  gabi::call(0x028ED6F8, ptr(0x1046A028));
  gabi::call(0x028F026C, ptr(0x101CA2FC));
  gabi::call(0x028EAB2C, ptr(0x1046A029));
  gabi::call(0x028F026C, ptr(0x101CA308));
}
VERIFY(0x0235D2D4, htetu_staticInit);
void htetu_trivialDestructor(void *object, u32 flags) {
  WWHD_FUNC(0x0235D368, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x0235D368, htetu_trivialDestructor);
void *htetu_splashCtor(void *object) {
  WWHD_FUNC(0x0235D37C, void *, object);
  if (!object)
    object = gabi::call<void *>(0x0273AD10, 0x2C);
  if (object)
    gabi::call(0x025A5894, object, 0, 0);
  return object;
}
VERIFY(0x0235D37C, htetu_splashCtor);
void htetu_splashDestructor(void *object, u32 flags) {
  WWHD_FUNC(0x0235D3CC, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x0235D3CC, htetu_splashDestructor);
void htetu_actorDestructor(void *object, u32 flags) {
  WWHD_FUNC(0x0235D3E0, void, object, flags);
  if (object) {
    gabi::call(0x028F0164, ptr(gabi::ea(object), 0x3EC), 2, 0x2C,
               ptr(0x0235D3CC), 0, 0);
    gabi::call(0x025D50BC, object, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, object);
  }
}
VERIFY(0x0235D3E0, htetu_actorDestructor);
void htetu_noop(void *object) { WWHD_FUNC(0x0235D454, void, object); }
VERIFY(0x0235D454, htetu_noop);
BOOL htetu_IsDelete() {
  WWHD_FUNC(0x0235D458, BOOL);
  return 1;
}
VERIFY(0x0235D458, htetu_IsDelete);

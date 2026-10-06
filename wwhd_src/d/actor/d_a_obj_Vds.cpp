#include "d/actor/d_a_obj_Vds.h"
namespace {
template <class T> T read(u32 a, u32 off = 0) { return gabi::load<T>(a + off); }
template <class T> void write(u32 a, u32 off, T v) {
  gabi::store<T>(a + off, v);
}
void *ptr(u32 a, u32 off = 0) { return gabi::at<void>(a + off); }
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 resource(s32 id) {
  struct SafeString {
    be<u32> text, vtable;
  };
  gabi::Local<SafeString> name;
  name->text = 0x10032A58;
  name->vtable = 0x10032928;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), name.get(),
                         id);
}
void assertion(s32 line, u32 expression) {
  gabi::call(0x0273AA24, ptr(0x10032950), line, ptr(expression));
}
u32 dispatch(u32 actor, u32 descriptor) {
  s32 delta = read<s16>(descriptor), index = read<s16>(descriptor, 2);
  u32 object = actor + delta;
  u32 target;
  if (index < 0)
    target = read<u32>(descriptor, 4);
  else {
    s32 vptrOffset = read<s16>(descriptor, 6);
    u32 table = read<u32>(object + vptrOffset);
    target = read<u32>(table, (u32)index * 8 + 4);
  }
  return gabi::call<u32>(target, ptr(object));
}
u32 actorById(s32 id) {
  gabi::Local<be<s32>> argument;
  *argument = id;
  if (id == -1)
    return 0;
  return gabi::call<u32>(0x025D5218, ptr(0x025E1234), argument.get());
}
} // namespace
daObjVds_c *vds_construct(daObjVds_c *actor) {
  WWHD_FUNC(0x023AEE1C, daObjVds_c *, actor);
  if (!actor)
    actor = gabi::call<daObjVds_c *>(0x0273AD10, 0x574);
  if (actor) {
    gabi::call(0x025D4ED0, actor);
    write<u32>(gabi::ea(actor), 0xB4, 0x10032940);
    gabi::call(0x025E80D0, actor->brk0);
    gabi::call(0x025E80D0, actor->brk1);
    gabi::call(0x028EFFD0, &actor->lights[0], 2, 0x24, ptr(0x023B04D8));
  }
  return actor;
}
VERIFY(0x023AEE1C, vds_construct);
void vds_setMatrix(daObjVds_c *actor) {
  WWHD_FUNC(0x023AEE98, void, actor);
  u32 model = read<u32>(actor->morf0, 0x90);
  f32 z = actor->scale.z, x = actor->scale.x, y = actor->scale.y;
  write<f32>(model, 0xBC, x);
  write<f32>(model, 0xC0, y);
  write<f32>(model, 0xC4, z);
  u32 matrix = 0x1048D0CC;
  gabi::call(0x028E93CC, ptr(matrix), (f32)actor->current.pos.x,
             (f32)actor->current.pos.y, (f32)actor->current.pos.z);
  gabi::call(0x025F1B48, ptr(matrix), (s32)(s16)actor->shape_angle.x,
             (s32)(s16)actor->shape_angle.y, (s32)(s16)actor->shape_angle.z);
  model = read<u32>(actor->morf0, 0x90);
  f32 values[12];
  for (u32 i = 0; i < 12; i++)
    values[i] = read<f32>(matrix, i * 4);
  for (u32 i = 0; i < 12; i++)
    write<f32>(model, 0xC8 + i * 4, values[i]);
  model = read<u32>(actor->morf1, 0x90);
  for (u32 i = 0; i < 12; i++)
    values[i] = read<f32>(matrix, i * 4);
  for (u32 i = 0; i < 12; i++)
    write<f32>(model, 0xC8 + i * 4, values[i]);
  gabi::call(0x028E90D4, ptr(matrix), actor->matrix);
}
VERIFY(0x023AEE98, vds_setMatrix);
BOOL vds_createHeap(daObjVds_c *actor) {
  WWHD_FUNC(0x023AEFF4, BOOL, actor);
  u32 model0 = resource(10);
  if (!model0)
    assertion(0x359, 0x10032988);
  actor->joint0 = resource(6);
  if (!actor->joint0)
    assertion(0x35D, 0x10032998);
  if (model0 && actor->joint0)
    actor->morf0 =
        gabi::call<u32>(0x025E4F64, 0, ptr(model0), 0, 0, ptr(actor->joint0), 0,
                        0, -1, 1, 0, 0, 0x11020203, 1.0f);
  if (!actor->morf0)
    assertion(0x36A, 0x10032960);
  u32 model1 = resource(11);
  if (!model1)
    assertion(0x36E, 0x100329AC);
  actor->joint1 = resource(7);
  if (!actor->joint1)
    assertion(0x372, 0x100329BC);
  if (model1 && actor->joint1)
    actor->morf1 =
        gabi::call<u32>(0x025E4F64, 0, ptr(model1), 0, 0, ptr(actor->joint1), 0,
                        0, -1, 1, 0, 0, 0x11020203, 1.0f);
  if (!actor->morf1)
    assertion(0x37F, 0x1003296C);
  actor->color0 = resource(14);
  u32 color = actor->color0;
  if (!color) {
    assertion(0x384, 0x100329D0);
    color = actor->color0;
  }
  s32 brk0 = gabi::call<s32>(0x025E8154, actor->brk0, ptr(model0), ptr(color),
                             1, 0, 0, -1, 0, 0, 1.0f);
  actor->color1 = resource(15);
  color = actor->color1;
  if (!color) {
    assertion(0x391, 0x100329E4);
    color = actor->color1;
  }
  s32 brk1 = gabi::call<s32>(0x025E8154, actor->brk1, ptr(model1), ptr(color),
                             1, 0, 0, -1, 0, 0, 1.0f);
  gabi::call(0x023AEE98, actor);
  u32 collision = resource(18);
  if (!collision)
    assertion(0x3A7, 0x10032978);
  else {
    actor->background = gabi::call<u32>(0x024F23F4, 0);
    u32 bg = actor->background;
    if (bg)
      gabi::call(0x0200A030, ptr(bg), ptr(collision), 1, actor->matrix);
  }
  return actor->joint0 && actor->morf0 && read<u32>(actor->morf0, 0x90) &&
         actor->joint1 && actor->morf1 && read<u32>(actor->morf1, 0x90) &&
         actor->background && actor->color0 && actor->color1 && brk0 && brk1;
}
VERIFY(0x023AEFF4, vds_createHeap);
BOOL vds_heapCallback(daObjVds_c *actor) {
  WWHD_FUNC(0x023AF400, BOOL, actor);
  return gabi::call<BOOL>(0x023AEFF4, actor);
}
VERIFY(0x023AF400, vds_heapCallback);
BOOL vds_processInit(daObjVds_c *actor, s32 process) {
  WWHD_FUNC(0x023AF404, BOOL, actor, process);
  if ((u32)process >= 2 ||
      !dispatch(gabi::ea(actor), 0x100329F8 + (u32)process * 8))
    return 0;
  actor->process = process;
  return 1;
}
VERIFY(0x023AF404, vds_processInit);
void vds_firstProcess(daObjVds_c *actor) {
  WWHD_FUNC(0x023AF4BC, void, actor);
  s32 sw = gabi::call<s32>(0x02349DFC, actor, 8, 0);
  u32 save = read<u32>(0x101F84DC);
  s32 room = read<s8>(gabi::ea(actor), 0x2FE);
  s32 enabled = gabi::call<s32>(0x025BA0C0, ptr(save, 0x20), sw, room);
  gabi::call(0x023AF404, actor, enabled != 0);
}
VERIFY(0x023AF4BC, vds_firstProcess);
void vds_eventInit(daObjVds_c *actor) {
  WWHD_FUNC(0x023AF51C, void, actor);
  actor->event = -1;
  actor->eventState = 0;
}
VERIFY(0x023AF51C, vds_eventInit);
s32 vds_create(daObjVds_c *actor) {
  WWHD_FUNC(0x023AF530, s32, actor);
  u32 a = gabi::ea(actor);
  u32 flags = read<u32>(a, 0x2E4);
  if (!(flags & 8)) {
    if (actor) {
      gabi::call(0x023AEE1C, actor);
      flags = read<u32>(a, 0x2E4);
    }
    write<u32>(a, 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, actor->phase, ptr(0x10032A58));
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, actor, ptr(0x023AF400), 0))
    return 5;
  gabi::call(0x023AF4BC, actor);
  u32 model = read<u32>(actor->morf0, 0x90);
  write<u32>(a, 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, actor, -2000.0f, -2000.0f, -2000.0f, 2000.0f, 2000.0f,
             2000.0f);
  u32 game = play();
  u32 bg = actor->background;
  gabi::call(0x024EEA6C, ptr(game, 0x12A0), ptr(bg), actor);
  bg = actor->background;
  write<u32>(bg, 0xA8, 0);
  gabi::call(0x023AF51C, actor);
  game = play();
  actor->eventIndex =
      gabi::call<s32>(0x02543F10, ptr(game, 0x52C4), ptr(0x10032A10), 255);
  for (auto &id : actor->switchActor)
    id = -1;
  return phase;
}
VERIFY(0x023AF530, vds_create);
void vds_deleteLights(daObjVds_c *actor) {
  WWHD_FUNC(0x023AF674, void, actor);
  for (auto &light : actor->lights)
    gabi::call(0x0255A374, &light);
}
VERIFY(0x023AF674, vds_deleteLights);
BOOL vds_delete(daObjVds_c *actor) {
  WWHD_FUNC(0x023AF6BC, BOOL, actor);
  u32 a = gabi::ea(actor), bg = actor->background;
  if (read<u32>(a, 0xF4) && bg && read<u32>(bg) < 256) {
    u32 game = play();
    bg = actor->background;
    gabi::call(0x020087EC, ptr(game, 0x12A0), ptr(bg));
  }
  gabi::call(0x023AF674, actor);
  gabi::call(0x025204C8, actor->phase, ptr(0x10032A58));
  return 1;
}
VERIFY(0x023AF6BC, vds_delete);
void vds_eventExecute(daObjVds_c *actor) {
  WWHD_FUNC(0x023AF734, void, actor);
  s32 state = actor->eventState;
  u32 a = gabi::ea(actor);
  if (state == 1) {
    if (read<u16>(a, 0xF8) == 2)
      actor->eventState = 2;
    else {
      gabi::call(0x025D7A58, actor, (s32)(s16)actor->event, 255, 65535, 0, 1);
      write<u16>(a, 0xFA, read<u16>(a, 0xFA) | 2);
    }
  } else if (state == 2) {
    s32 event = actor->event;
    u32 game = play();
    if (gabi::call<s32>(0x025440C8, ptr(game, 0x52C4), event)) {
      game = play();
      write<u16>(game, 0x52B8, read<u16>(game, 0x52B8) | 8);
      gabi::call(0x023AF51C, actor);
    }
  }
}
VERIFY(0x023AF734, vds_eventExecute);
BOOL vds_playAnimation(daObjVds_c *actor) {
  WWHD_FUNC(0x023AF834, BOOL, actor);
  gabi::call(0x025E535C, ptr(actor->morf0), 0, 0, 0);
  gabi::call(0x025E535C, ptr(actor->morf1), 0, 0, 0);
  return 1;
}
VERIFY(0x023AF834, vds_playAnimation);
u32 vds_searchSwitch(daObjVds_c *actor, void *candidate) {
  WWHD_FUNC(0x023AF888, u32, actor, candidate);
  u32 c = gabi::ea(candidate);
  if (gabi::call<s32>(0x025D4604, candidate) && c && read<s16>(c, 8) == 30) {
    for (auto &id : actor->switchActor)
      if (id == -1) {
        id = read<u32>(c, 4);
        break;
      }
  }
  return 0;
}
VERIFY(0x023AF888, vds_searchSwitch);
u32 vds_searchCallback(void *candidate, daObjVds_c *actor) {
  WWHD_FUNC(0x023AF924, u32, candidate, actor);
  return gabi::call<u32>(0x023AF888, actor, candidate);
}
VERIFY(0x023AF924, vds_searchCallback);
void vds_createLight(daObjVds_c *actor, s32 index, cXyz *pos) {
  WWHD_FUNC(0x023AF934, void, actor, index, pos);
  u32 i = (u32)index & 1;
  auto &light = actor->lights[i];
  light.position.copy(*pos);
  actor->lightPosition[i].copy(*pos);
  light.red = 600;
  light.green = 360;
  light.blue = 80;
  light.radius = 0;
  light.fluctuation = 0;
  gabi::call(0x025564B4, &light);
}
VERIFY(0x023AF934, vds_createLight);
void vds_executeLights(daObjVds_c *actor) {
  WWHD_FUNC(0x023AF9A8, void, actor);
  for (u32 i = 0; i < 2; i++) {
    auto &light = actor->lights[i];
    light.radius = (f32)actor->strength[i] * 700.0f;
    light.position.copy(actor->lightPosition[i]);
    light.blue = 80;
    light.green = 360;
    light.red = 600;
  }
}
VERIFY(0x023AF9A8, vds_executeLights);
void vds_processCommon(daObjVds_c *actor) {
  WWHD_FUNC(0x023AFA08, void, actor);
  u32 state = actor->lightStage;
  if (state == 0) {
    for (auto &id : actor->switchActor)
      id = -1;
    gabi::call(0x025D5218, ptr(0x023AF924), actor);
    s32 id0 = actor->switchActor[0], id1 = actor->switchActor[1];
    if (id0 == -1 || id1 == -1)
      return;
    u32 first = actorById(id0);
    u32 second = actorById(actor->switchActor[1]);
    if (!first || !second)
      return;
    s16 difference =
        (s16)((s32)read<s16>(first, 0x32A) - (s32)(s16)actor->shape_angle.y);
    if (difference >= 0) {
      id0 = actor->switchActor[0];
      id1 = actor->switchActor[1];
      actor->switchActor[0] = id1;
      actor->switchActor[1] = id0;
    }
    actor->lightStage = 1;
  } else if (state == 1) {
    u32 first = actorById(actor->switchActor[0]),
        second = actorById(actor->switchActor[1]);
    if (first && second) {
      gabi::call(0x023AF934, actor, 0, ptr(first, 0x314));
      gabi::call(0x023AF934, actor, 1, ptr(second, 0x314));
      actor->lightStage = 2;
    }
  } else if (state == 2)
    gabi::call(0x023AF9A8, actor);
}
VERIFY(0x023AFA08, vds_processCommon);
void vds_processMain(daObjVds_c *actor) {
  WWHD_FUNC(0x023AFBF4, void, actor);
  u32 state = actor->process;
  if (state < 2)
    dispatch(gabi::ea(actor), 0x10032A1C + state * 8);
}
VERIFY(0x023AFBF4, vds_processMain);
BOOL vds_execute(daObjVds_c *actor) {
  WWHD_FUNC(0x023AFC4C, BOOL, actor);
  gabi::call(0x023AF734, actor);
  gabi::call(0x023AF834, actor);
  f32 first = actor->strength[0];
  bool full = !(first < 1.0f) && !((f32)actor->strength[1] < 1.0f);
  if (!full)
    first = (f32)(first * 0.25f);
  u32 color = actor->color0;
  u32 target = read<u32>(read<u32>(color, 4), 0x14);
  s32 frame = gabi::call<s32>(target, ptr(color));
  write<f32>(gabi::ea(actor), 0x3F0, (f32)(first * (f32)(s32)((u32)frame - 1)));
  color = actor->color1;
  target = read<u32>(read<u32>(color, 4), 0x14);
  f32 second = 0;
  if (!full)
    second = (f32)((f32)actor->strength[1] * 0.25f);
  frame = gabi::call<s32>(target, ptr(color));
  if (full)
    second = actor->strength[1];
  write<f32>(gabi::ea(actor), 0x474,
             (f32)(second * (f32)(s32)((u32)frame - 1)));
  gabi::call(0x023AFA08, actor);
  gabi::call(0x023AFBF4, actor);
  gabi::call(0x023AEE98, actor);
  u32 bg = actor->background;
  if (bg)
    gabi::call(0x024F43DC, ptr(bg));
  return 1;
}
VERIFY(0x023AFC4C, vds_execute);
void *vds_color(void *out, void *input) {
  WWHD_FUNC(0x023AFE48, void *, out, input);
  u32 in = gabi::ea(input), o = gabi::ea(out);
  f32 rgba[4];
  for (u32 i = 0; i < 4; i++)
    rgba[i] = (f32)((f32)read<s16>(in, i * 2) / 255.0f);
  for (u32 i = 0; i < 4; i++)
    write<f32>(o, i * 4, rgba[i]);
  return out;
}
VERIFY(0x023AFE48, vds_color);
BOOL vds_draw(daObjVds_c *actor) {
  WWHD_FUNC(0x023AFF0C, BOOL, actor);
  u32 a = gabi::ea(actor), light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(light), 1, &actor->current.pos, ptr(a, 0x110));
  u32 morf = actor->morf0;
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(light), ptr(read<u32>(morf, 0x90)), ptr(a, 0x110));
  morf = actor->morf1;
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(light), ptr(read<u32>(morf, 0x90)), ptr(a, 0x110));
  morf = actor->morf0;
  u32 model = read<u32>(morf, 0x90);
  gabi::call(0x025E83FC, actor->brk0, ptr(read<u32>(model, 0xAC)),
             read<f32>(a, 0x3F0));
  morf = actor->morf1;
  model = read<u32>(morf, 0x90);
  gabi::call(0x025E83FC, actor->brk1, ptr(read<u32>(model, 0xAC)),
             read<f32>(a, 0x474));
  gabi::call(0x025E54D8, ptr(actor->morf0));
  gabi::call(0x025E54D8, ptr(actor->morf1));
  for (u32 i = 0; i < 2; i++)
    if ((f32)actor->strength[i] > 0.0f) {
      struct Color {
        be<f32> rgba[4];
      };
      gabi::Local<cXyz> position;
      position->copy(actor->lights[i].position);
      gabi::Local<Color> color;
      gabi::call(0x023AFE48, color.get(), &actor->lights[i].red);
      f32 strength = actor->strength[i];
      gabi::call(0x0273862C, ptr(read<u32>(0x101F8A20)), position.get(),
                 color.get(), 1, ptr(0x104A01CC), 1, 0, 0,
                 (f32)(400.0f * strength), strength, 0.0f);
    }
  return 1;
}
VERIFY(0x023AFF0C, vds_draw);
BOOL vds_setAnimation(daObjVds_c *actor, void *first, void *second, f32 speed,
                      f32 morph) {
  WWHD_FUNC(0x023B0080, BOOL, actor, first, second, speed, morph);
  gabi::call(0x025E4A98, ptr(actor->morf0), first, 2, 0, morph, speed, 0.0f,
             -1.0f);
  gabi::call(0x025E4A98, ptr(actor->morf1), second, 2, 0, morph, speed, 0.0f,
             -1.0f);
  actor->loopAnimation = 1;
  return 1;
}
VERIFY(0x023B0080, vds_setAnimation);
BOOL vds_offInit(daObjVds_c *actor) {
  WWHD_FUNC(0x023B016C, BOOL, actor);
  u32 color = actor->color0, model = read<u32>(actor->morf0, 0x90);
  if (!gabi::call<s32>(0x025E8154, actor->brk0, ptr(read<u32>(model, 0xAC)),
                       ptr(color), 1, 0, 0, -1, 1, 0, 1.0f))
    return 0;
  write<f32>(gabi::ea(actor), 0x3EC, 0.0f);
  color = actor->color1;
  model = read<u32>(actor->morf1, 0x90);
  if (!gabi::call<s32>(0x025E8154, actor->brk1, ptr(read<u32>(model, 0xAC)),
                       ptr(color), 1, 0, 0, -1, 1, 0, 1.0f))
    return 0;
  write<f32>(gabi::ea(actor), 0x470, 0.0f);
  return 1;
}
VERIFY(0x023B016C, vds_offInit);
void vds_offMain(daObjVds_c *actor) {
  WWHD_FUNC(0x023B0280, void, actor);
  for (u32 i = 0; i < 2; i++) {
    u32 sw = actorById(actor->switchActor[i]);
    f32 strength = sw ? read<f32>(sw, 0x1128) : 0.0f;
    actor->strength[i] = (f32)(strength * strength);
  }
  s32 sw = gabi::call<s32>(0x02349DFC, actor, 8, 0);
  u32 save = read<u32>(0x101F84DC);
  s32 room = read<s8>(gabi::ea(actor), 0x2FE);
  if (gabi::call<s32>(0x025BA0C0, ptr(save, 0x20), sw, room))
    gabi::call(0x023AF404, actor, 1);
}
VERIFY(0x023B0280, vds_offMain);
BOOL vds_onInit(daObjVds_c *actor) {
  WWHD_FUNC(0x023B037C, BOOL, actor);
  u32 first = actor->joint0, second = actor->joint1;
  if (!gabi::call<s32>(0x023B0080, actor, ptr(first), ptr(second), 1.0f, 0.0f))
    return 0;
  for (auto &strength : actor->strength)
    strength = 1.0f;
  return 1;
}
VERIFY(0x023B037C, vds_onInit);
s32 vds_createMethod(daObjVds_c *actor) {
  WWHD_FUNC(0x023B0420, s32, actor);
  return gabi::call<s32>(0x023AF530, actor);
}
VERIFY(0x023B0420, vds_createMethod);
BOOL vds_deleteMethod(daObjVds_c *actor) {
  WWHD_FUNC(0x023B0424, BOOL, actor);
  return gabi::call<BOOL>(0x023AF6BC, actor);
}
VERIFY(0x023B0424, vds_deleteMethod);
BOOL vds_executeMethod(daObjVds_c *actor) {
  WWHD_FUNC(0x023B0428, BOOL, actor);
  return gabi::call<BOOL>(0x023AFC4C, actor);
}
VERIFY(0x023B0428, vds_executeMethod);
BOOL vds_drawMethod(daObjVds_c *actor) {
  WWHD_FUNC(0x023B042C, BOOL, actor);
  return gabi::call<BOOL>(0x023AFF0C, actor);
}
VERIFY(0x023B042C, vds_drawMethod);
void vds_staticInit() {
  WWHD_FUNC(0x023B0430, void);
  write<u32>(0x1046C818, 8, 0);
  write<u32>(0x1046C818, 0, 0);
  write<u32>(0x1046C818, 12, 0);
  write<u32>(0x1046C818, 4, 0);
  gabi::call(0x028F026C, ptr(0x101CD828));
  write<f32>(0x1046C80C, 0, -3.1415927410125732f);
  write<f32>(0x1046C810, 0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046C814));
  gabi::call(0x028F026C, ptr(0x101CD834));
  gabi::call(0x028EAB2C, ptr(0x1046C815));
  gabi::call(0x028F026C, ptr(0x101CD840));
}
VERIFY(0x023B0430, vds_staticInit);
void vds_simpleDestructor(void *object, s32 flags) {
  WWHD_FUNC(0x023B04C4, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x023B04C4, vds_simpleDestructor);
VdsLight *vds_lightConstructor(VdsLight *light) {
  WWHD_FUNC(0x023B04D8, VdsLight *, light);
  if (!light)
    light = gabi::call<VdsLight *>(0x0273AD10, 0x24);
  if (light)
    light->scale = 1.0f;
  return light;
}
VERIFY(0x023B04D8, vds_lightConstructor);
void vds_lightDestructor(void *object) { WWHD_FUNC(0x023B0518, void, object); }
VERIFY(0x023B0518, vds_lightDestructor);
void vds_actorDestructor(daObjVds_c *actor, s32 flags) {
  WWHD_FUNC(0x023B051C, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x023B051C, vds_actorDestructor);
void vds_onMain(daObjVds_c *actor) { WWHD_FUNC(0x023B0570, void, actor); }
VERIFY(0x023B0570, vds_onMain);
BOOL vds_isDelete(daObjVds_c *actor) {
  WWHD_FUNC(0x023B0574, BOOL, actor);
  return 1;
}
VERIFY(0x023B0574, vds_isDelete);

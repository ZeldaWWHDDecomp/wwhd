// Full actual HD TU 023A2598..023A3CC3, including unnamed helpers/tails.
#include "d/actor/d_a_obj_tntrap.h"
#include "bindings.h"
using Trap = daObjTnTrap_c;
namespace {
template <class T> T read(u32 a, u32 o = 0) { return *gabi::at<be<T>>(a + o); }
template <class T> void write(u32 a, u32 o, T v) {
  *gabi::at<be<T>>(a + o) = v;
}
void *ptr(u32 a, u32 o = 0) { return gabi::at<void>(a + o); }
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 save() { return read<u32>(0x101F84DC); }
s32 switchOn(Trap *trap, u32 id) {
  return gabi::call<s32>(0x025BA0C0, ptr(save(), 0x20), id,
                         (s32)(s8)read<u8>(gabi::ea(trap), 0x2FE));
}
s32 eventOn(u32 id) {
  return gabi::call<s32>(0x025B8B94, ptr(save(), 0x644), id);
}
u32 memberTarget(u32 descriptor, u32 &self) {
  self += (s32)read<s16>(descriptor);
  s16 slot = read<s16>(descriptor, 2);
  return slot < 0 ? read<u32>(descriptor, 4)
                  : read<u32>(read<u32>(self, (s32)read<s16>(descriptor, 6)),
                              (u32)slot * 8 + 4);
}
struct Archive {
  be<u32> name, vtable;
};
} // namespace
BOOL tntrap_checkAppear(Trap *trap) {
  WWHD_FUNC(0x023A2598, BOOL, trap);
  trap->switchSave = gabi::call<u32>(0x023A3CA8, trap, 8, 0);
  trap->switchSave2 = gabi::call<u32>(0x023A3CA8, trap, 8, 8);
  trap->argument = gabi::call<u32>(0x023A3CA8, trap, 1, 16);
  u32 type = gabi::call<u32>(0x023A3CA8, trap, 2, 17);
  trap->mapType = type;
  if (type == 0) {
    if (eventOn(0x3A04) != 1)
      return 0;
    u32 sw = trap->switchSave;
    if (sw == 255 || switchOn(trap, sw) != 1) {
      trap->trapType = 0;
      return 1;
    }
    if (gabi::call<s32>(0x025B7E00, ptr(save(), 0xD4)) != 8 ||
        trap->argument != 0)
      return 0;
    if (eventOn(0x2C01) == 1) {
      sw = trap->switchSave2;
      if (sw == 255 || switchOn(trap, sw) != 0)
        return 0;
      trap->trapType = 2;
      return 1;
    }
    trap->trapType = 1;
    return 1;
  }
  if (type == 1) {
    u32 sw = trap->switchSave;
    if (sw == 255 || switchOn(trap, sw) != 0)
      return 0;
    trap->trapType = 3;
    return 1;
  }
  if (type == 2) {
    u32 sw = trap->switchSave;
    if (sw != 255 && switchOn(trap, sw) != 0)
      return 0;
    trap->trapType = 5;
    return 1;
  }
  gabi::call(0x0273AA24, ptr(0x10031180), 0x17C, ptr(0x1003117C));
  return 0;
}
VERIFY(0x023A2598, tntrap_checkAppear);
BOOL tntrap_createHeap(Trap *trap) {
  WWHD_FUNC(0x023A2794, BOOL, trap);
  gabi::Local<Archive> archive;
  archive->name = 0x10031064;
  archive->vtable = 0x1003106C;
  u32 data =
      gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(), 3);
  u32 background = gabi::call<u32>(0x024F2478, ptr(data), 1, &trap->matrix);
  trap->background = background;
  return background != 0;
}
VERIFY(0x023A2794, tntrap_createHeap);
BOOL tntrap_heapCallback(Trap *trap) {
  WWHD_FUNC(0x023A2814, BOOL, trap);
  return gabi::call<s32>(0x023A2794, trap);
}
VERIFY(0x023A2814, tntrap_heapCallback);
void tntrap_setParticleOffset(Trap *trap) {
  WWHD_FUNC(0x023A2818, void, trap);
  if (trap->trapType == 5) {
    u32 player = read<u32>(play(), 0x5B2C);
    if (player) {
      f32 difference = read<f32>(player, 0x380) - (f32)trap->home.pos.y;
      f32 rows = difference / 180.f;
      trap->particleOffsetY[0] = (f32)gabi::ftoi(rows) * 180.f;
    }
  } else
    trap->particleOffsetY[0] = 0.f;
}
VERIFY(0x023A2818, tntrap_setParticleOffset);
void tntrap_setMatrix(Trap *trap) {
  WWHD_FUNC(0x023A28C0, void, trap);
  gabi::call(0x028E93CC, mDoMtx_stack_c::get(), (f32)trap->home.pos.x,
             (f32)trap->home.pos.y, (f32)trap->home.pos.z);
  gabi::call(0x025F19F8, mDoMtx_stack_c::get(), (s32)(s16)trap->shape_angle.x,
             (s32)(s16)trap->shape_angle.y, (s32)(s16)trap->shape_angle.z);
  gabi::call(0x025F24E0, 0.f, -9000.f, -94.f);
  gabi::call(0x025F2518, (f32)trap->scale.x, 100.f, (f32)trap->scale.z);
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &trap->matrix);
}
VERIFY(0x023A28C0, tntrap_setMatrix);
void tntrap_setTriangles(Trap *trap, s32 group) {
  WWHD_FUNC(0x023A2958, void, trap, group);
  f32 y = (f32)trap->home.pos.y + (f32)trap->particleOffsetY[group];
  gabi::call(0x028E93CC, mDoMtx_stack_c::get(), (f32)trap->home.pos.x, y,
             (f32)trap->home.pos.z);
  gabi::call(0x025F19F8, mDoMtx_stack_c::get(), (s32)(s16)trap->shape_angle.x,
             (s32)(s16)trap->shape_angle.y, (s32)(s16)trap->shape_angle.z);
  gabi::Local<cXyz[3]> points;
  for (u32 triangle = 0; triangle < 4; ++triangle) {
    for (u32 vertex = 0; vertex < 3; ++vertex) {
      u32 index = read<u32>(0x101CD344, (triangle * 3 + vertex) * 4),
          source = 0x10031134 + index * 12;
      cXyz *point = &(*points)[vertex];
      point->x = read<f32>(source);
      point->y = read<f32>(source, 4);
      point->z = read<f32>(source, 8);
      gabi::call(0x028E8F64, mDoMtx_stack_c::get(), point, point);
    }
    gabi::call(0x0201924C,
               ptr(gabi::ea(&trap->triangles[group * 4 + triangle]), 0x118),
               &(*points)[0], &(*points)[1], &(*points)[2]);
  }
}
VERIFY(0x023A2958, tntrap_setTriangles);
void tntrap_setupAction(Trap *trap, s32 action) {
  WWHD_FUNC(0x023A2A5C, void, trap, action);
  u32 self = gabi::ea(trap), descriptor = 0x101CD3AC + (u32)action * 8;
  u32 target = memberTarget(descriptor, self);
  gabi::call_ptr(target, ptr(self));
  descriptor = 0x101CD374 + (u32)action * 8;
  write<u32>(gabi::ea(trap), 0xED0, read<u32>(descriptor));
  trap->action = action;
  write<u32>(gabi::ea(trap), 0xED4, read<u32>(descriptor, 4));
}
VERIFY(0x023A2A5C, tntrap_setupAction);
s32 tntrap_Create(Trap *trap) {
  WWHD_FUNC(0x023A2B14, s32, trap);
  u32 a = gabi::ea(trap), flags = read<u32>(a, 0x2E4);
  s32 phase = 5;
  if (!(flags & 8)) {
    if (trap) {
      gabi::call(0x025D4ED0, trap);
      write<u32>(a, 0xB4, 0x10031094);
      gabi::call(0x0200BD2C, &trap->status);
      gabi::call(0x02515DA0, ptr(a, 0x3D4));
      write<u32>(a, 0x3D0, 0x1004AE88);
      write<u32>(a, 0x3D4, 0x1004AEC0);
      gabi::call(0x028EFFD0, trap->triangles, 8, 0x150, ptr(0x023A3B80));
      flags = read<u32>(a, 0x2E4);
    }
    write<u32>(a, 0x2E4, flags | 8);
  }
  if (read<u8>(a, 0xC) == 0) {
    s32 appears = gabi::call<s32>(0x023A2598, trap);
    trap->appears = (u8)appears;
    if (!appears)
      return phase;
  } else if (!trap->appears)
    return phase;
  phase = gabi::call<s32>(0x02520460, &trap->phase, ptr(0x10031064));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, trap, ptr(0x023A2814), 0x2E0))
      return 5;
    if (trap->trapType != 3) {
      u32 world = play();
      if (gabi::call<s32>(0x024EEA6C, ptr(world, 0x12A0), ptr(trap->background),
                          trap))
        return 5;
    }
    gabi::call(0x023A2818, trap);
    gabi::call(0x023A28C0, trap);
    gabi::call(0x02515F14, &trap->status, 255, 255, trap);
    for (u32 group = 0; group < 2; ++group) {
      for (u32 i = 0; i < 4; ++i) {
        auto *triangle = &trap->triangles[group * 4 + i];
        gabi::call(0x0251650C, triangle, ptr(0x100310E0));
        triangle->status = gabi::ea(&trap->status);
      }
      gabi::call(0x023A2958, trap, group);
    }
    gabi::call(0x023A2A5C, trap, trap->trapType == 3 ? 6 : 0);
  }
  return phase;
}
VERIFY(0x023A2B14, tntrap_Create);
s32 tntrap_createWrapper(Trap *trap) {
  WWHD_FUNC(0x023A2CE8, s32, trap);
  return gabi::call<s32>(0x023A2B14, trap);
}
VERIFY(0x023A2CE8, tntrap_createWrapper);
void tntrap_clearParticles(Trap *trap, s32 group) {
  WWHD_FUNC(0x023A2CEC, void, trap, group);
  if (trap->particlesActive[group]) {
    for (u32 i = 0; i < 2; ++i) {
      u32 emitter = trap->emittersA[group][i];
      if (emitter) {
        u32 flags = read<u32>(emitter, 0x254);
        write<s32>(emitter, 0x5C, -1);
        write<u32>(emitter, 0x254, flags | 1);
        trap->emittersA[group][i] = 0;
      }
    }
    for (u32 i = 0; i < 3; ++i) {
      u32 emitter = trap->emittersB[group][i];
      if (emitter) {
        u32 flags = read<u32>(emitter, 0x254);
        write<s32>(emitter, 0x5C, -1);
        write<u32>(emitter, 0x254, flags | 1);
        trap->emittersB[group][i] = 0;
      }
    }
  }
  trap->particlesActive[group] = 0;
}
VERIFY(0x023A2CEC, tntrap_clearParticles);
BOOL tntrap_Delete(Trap *trap) {
  WWHD_FUNC(0x023A2D88, BOOL, trap);
  if (trap->appears) {
    gabi::call(0x025204C8, &trap->phase, ptr(0x10031064));
    u32 a = gabi::ea(trap), background = trap->background;
    if (read<u32>(a, 0xF4) && background) {
      if (read<u32>(background) < 256) {
        u32 world = play();
        gabi::call(0x020087EC, ptr(world, 0x12A0), ptr(trap->background));
      }
      trap->background = 0;
    }
    for (u32 i = 0; i < 2; ++i)
      gabi::call(0x023A2CEC, trap, i);
  }
  return 1;
}
VERIFY(0x023A2D88, tntrap_Delete);
BOOL tntrap_deleteWrapper(Trap *trap) {
  WWHD_FUNC(0x023A2E30, BOOL, trap);
  return gabi::call<s32>(0x023A2D88, trap);
}
VERIFY(0x023A2E30, tntrap_deleteWrapper);
void tntrap_setSound(Trap *trap) {
  WWHD_FUNC(0x023A2E34, void, trap);
  u32 action = trap->action;
  if (action >= 1 && action <= 4) {
    s32 reverb =
        gabi::call<s32>(0x02520540, (s32)(s8)read<u8>(gabi::ea(trap), 0x326));
    gabi::call(0x025E1A40, 0x6239, &trap->current.pos, 0, reverb);
  }
}
VERIFY(0x023A2E34, tntrap_setSound);
BOOL tntrap_Execute(Trap *trap) {
  WWHD_FUNC(0x023A2E90, BOOL, trap);
  u32 background = trap->background;
  if (background && read<u32>(background) < 256)
    gabi::call(0x024F43DC, ptr(background));
  for (u32 i = 0; i < 2; ++i)
    gabi::call(0x023A2958, trap, i);
  gabi::call(0x02515E50, ptr(gabi::ea(trap), 0x3D4));
  u32 self = gabi::ea(trap),
      target = memberTarget(gabi::ea(&trap->actionMethod), self);
  if (gabi::call_ptr<s32>(target, ptr(self))) {
    for (u32 group = 0; group < 2; ++group)
      for (u32 i = 0; i < 4; ++i) {
        u32 world = play();
        gabi::call(0x0200E240, ptr(world, 0x26A4),
                   &trap->triangles[group * 4 + i]);
      }
    gabi::call(0x023A2E34, trap);
  }
  return 1;
}
VERIFY(0x023A2E90, tntrap_Execute);
BOOL tntrap_executeWrapper(Trap *trap) {
  WWHD_FUNC(0x023A2F90, BOOL, trap);
  return gabi::call<s32>(0x023A2E90, trap);
}
VERIFY(0x023A2F90, tntrap_executeWrapper);
BOOL tntrap_draw(Trap *trap) {
  WWHD_FUNC(0x023A2F94, BOOL, trap);
  return 1;
}
VERIFY(0x023A2F94, tntrap_draw);
BOOL tntrap_isDelete(Trap *trap) {
  WWHD_FUNC(0x023A2F9C, BOOL, trap);
  return 1;
}
VERIFY(0x023A2F9C, tntrap_isDelete);
void tntrap_setParticles(Trap *trap, s32 group, f32 height) {
  WWHD_FUNC(0x023A2FA4, void, trap, group, height);
  if (trap->particlesActive[group]) {
    if ((f32)trap->particleOffsetY[group] == height)
      return;
    gabi::call(0x023A2CEC, trap, group);
  }
  for (u32 i = 0; i < 2; ++i) {
    if (!trap->emittersA[group][i]) {
      u32 offset = 0x100310A4 + i * 12;
      f32 y = read<f32>(offset, 4) + height, z = read<f32>(offset, 8),
          x = read<f32>(offset);
      u32 controller = read<u32>(play(), 0x5AB0);
      u32 emitter = gabi::call<u32>(0x025A847C, ptr(controller), 0, 0x82EA,
                                    &trap->home.pos, &trap->shape_angle, 0, 255,
                                    0, -1, 0, 0, 0);
      trap->emittersA[group][i] = emitter;
      write<f32>(emitter, 0x14, x);
      write<f32>(emitter, 0x1C, z);
      write<f32>(emitter, 0x18, y);
    }
  }
  for (u32 i = 0; i < 3; ++i) {
    if (!trap->emittersB[group][i]) {
      u32 offset = 0x100310BC + i * 12;
      f32 y = read<f32>(offset, 4) + height, z = read<f32>(offset, 8),
          x = read<f32>(offset);
      u32 controller = read<u32>(play(), 0x5AB0);
      u32 emitter = gabi::call<u32>(0x025A847C, ptr(controller), 0, 0x82EB,
                                    &trap->home.pos, &trap->shape_angle, 0, 255,
                                    0, -1, 0, 0, 0);
      trap->emittersB[group][i] = emitter;
      write<f32>(emitter, 0x14, x);
      write<f32>(emitter, 0x1C, z);
      write<f32>(emitter, 0x18, y);
    }
  }
  trap->particleOffsetY[group] = height;
  trap->particlesActive[group] = 1;
}
VERIFY(0x023A2FA4, tntrap_setParticles);
BOOL tntrap_checkEvents(Trap *trap) {
  WWHD_FUNC(0x023A319C, BOOL, trap);
  u32 type = trap->trapType;
  if (type == 0) {
    u32 sw = trap->switchSave;
    if (sw != 255 && switchOn(trap, sw) == 1) {
      s32 action = 4;
      if (trap->argument == 0) {
        action = 2;
        gabi::call(0x025B8B68, ptr(save(), 0x644), 0x3B40);
      }
      gabi::call(0x023A2A5C, trap, action);
    }
  } else if (type == 2) {
    if (trap->action == 1)
      gabi::call(0x023A2A5C, trap, 2);
  } else if (type == 3) {
    u32 sw = trap->switchSave;
    if (sw != 255 && switchOn(trap, sw) == 1) {
      u32 boat = read<u32>(play(), 0x5B3C);
      if (boat) {
        write<u32>(boat, 0x644, read<u32>(boat, 0x644) & ~0x00800000u);
        gabi::call(0x025D57E0, trap);
        return 0;
      }
    }
  } else if (type == 5) {
    u32 sw = trap->switchSave;
    if (sw != 255 && switchOn(trap, sw) == 1) {
      gabi::call(0x025D57E0, trap);
      return 0;
    }
  }
  return 1;
}
VERIFY(0x023A319C, tntrap_checkEvents);
void tntrap_offInit(Trap *trap) {
  WWHD_FUNC(0x023A3310, void, trap);
  for (u32 i = 0; i < 2; ++i)
    gabi::call(0x023A2CEC, trap, i);
}
VERIFY(0x023A3310, tntrap_offInit);
void tntrap_onInit(Trap *trap) {
  WWHD_FUNC(0x023A334C, void, trap);
  gabi::call(0x023A2818, trap);
  gabi::call(0x023A2FA4, trap, 0, (f32)trap->particleOffsetY[0]);
}
VERIFY(0x023A334C, tntrap_onInit);
void tntrap_registerInit(Trap *trap) {
  WWHD_FUNC(0x023A3388, void, trap);
  trap->particleOffsetY[0] = 0.f;
  gabi::call(0x023A2FA4, trap, 0, 0.f);
  s32 type = trap->trapType;
  u32 world = play();
  trap->eventId =
      (s16)gabi::call<s32>(0x02543F10, ptr(world, 0x52C4),
                           ptr(type == 2 ? 0x100311B0 : 0x100311C0), 255);
}
VERIFY(0x023A3388, tntrap_registerInit);
void tntrap_wait2Init(Trap *trap) {
  WWHD_FUNC(0x023A3408, void, trap);
  trap->particleOffsetY[0] = 0.f;
  gabi::call(0x023A2FA4, trap, 0, 0.f);
}
VERIFY(0x023A3408, tntrap_wait2Init);
void tntrap_endInit(Trap *trap) {
  WWHD_FUNC(0x023A341C, void, trap);
  for (u32 i = 0; i < 2; ++i)
    gabi::call(0x023A2CEC, trap, i);
}
VERIFY(0x023A341C, tntrap_endInit);
BOOL tntrap_offWait(Trap *trap) {
  WWHD_FUNC(0x023A3458, BOOL, trap);
  u32 player = read<u32>(play(), 0x5B2C);
  if (player) {
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, ptr(player, 0x314), difference.get(),
               &trap->home.pos);
    gabi::Local<cXyz> horizontal;
    horizontal->x = difference->x;
    horizontal->y = 0.f;
    horizontal->z = difference->z;
    f64 square = gabi::call<f64>(0x028E8DD0, horizontal.get());
    f64 distance = gabi::call<f64>(0x028F4384, square);
    if (distance < 500.f)
      gabi::call(0x023A2A5C, trap, 1);
  }
  return gabi::call<s32>(0x023A319C, trap);
}
VERIFY(0x023A3458, tntrap_offWait);
BOOL tntrap_onWait(Trap *trap) {
  WWHD_FUNC(0x023A34EC, BOOL, trap);
  u32 player = read<u32>(play(), 0x5B2C);
  if (player) {
    gabi::Local<cXyz> difference;
    gabi::call(0x0201ADE0, ptr(player, 0x314), difference.get(),
               &trap->home.pos);
    gabi::Local<cXyz> horizontal;
    horizontal->x = difference->x;
    horizontal->y = 0.f;
    horizontal->z = difference->z;
    f64 square = gabi::call<f64>(0x028E8DD0, horizontal.get());
    f64 distance = gabi::call<f64>(0x028F4384, square);
    if (distance > 500.f) {
      gabi::call(0x023A2A5C, trap, 0);
      return gabi::call<s32>(0x023A319C, trap);
    }
    if (trap->trapType == 5) {
      f32 distances[2];
      for (u32 i = 0; i < 2; ++i) {
        f32 center =
            ((f32)trap->home.pos.y + (f32)trap->particleOffsetY[i]) + 90.f;
        f32 delta = read<f32>(player, 0x318) - center;
        distances[i] = delta;
        if (std::fabs((f64)delta) > 150.f)
          gabi::call(0x023A2CEC, trap, i);
      }
      for (u32 i = 0; i < 2; ++i) {
        f32 delta = distances[i];
        if (std::fabs((f64)delta) > 80.f) {
          f32 home = trap->home.pos.y, offset = trap->particleOffsetY[i];
          f32 next = delta > 0.f ? offset + 180.f : offset - 180.f;
          f32 center = (home + next) + 90.f,
              further = read<f32>(player, 0x318) - center;
          if (!(std::fabs((f64)further) > 150.f))
            gabi::call(0x023A2FA4, trap, i ^ 1, next);
        }
      }
    }
  }
  return gabi::call<s32>(0x023A319C, trap);
}
VERIFY(0x023A34EC, tntrap_onWait);
BOOL tntrap_registerWait(Trap *trap) {
  WWHD_FUNC(0x023A3710, BOOL, trap);
  s32 event = trap->eventId;
  if (event != -1) {
    if (read<u16>(gabi::ea(trap), 0xF8) == 2)
      gabi::call(0x023A2A5C, trap, 3);
    else
      gabi::call(0x025D7A58, trap, event, 255, 65535, 0, 1);
  } else {
    s32 type = trap->trapType;
    u32 world = play();
    trap->eventId =
        (s16)gabi::call<s32>(0x02543F10, ptr(world, 0x52C4),
                             ptr(type == 2 ? 0x100311E0 : 0x100311F0), 255);
  }
  return 1;
}
VERIFY(0x023A3710, tntrap_registerWait);
namespace {
bool sameString(u32 a, u32 b) {
  while (true) {
    u8 x = read<u8>(a++), y = read<u8>(b++);
    if (x != y)
      return false;
    if (!x)
      return true;
  }
}
} // namespace
BOOL tntrap_demoWait(Trap *trap) {
  WWHD_FUNC(0x023A37D0, BOOL, trap);
  s32 event = trap->eventId;
  u32 world = play();
  if (gabi::call<u32>(0x02544044, ptr(world, 0x52C4), event)) {
    u32 manager = play();
    s32 staff = gabi::call<s32>(0x02542D88, ptr(manager, 0x52C4),
                                ptr(0x10031200), 0, 0);
    if (staff != -1) {
      manager = play();
      u32 cut = gabi::call<u32>(0x02544830, ptr(manager, 0x52C4), staff);
      if (sameString(cut, 0x10031208))
        gabi::call(0x023A2A5C, trap, 5);
    }
  }
  return 1;
}
VERIFY(0x023A37D0, tntrap_demoWait);
BOOL tntrap_demoWait2(Trap *trap) {
  WWHD_FUNC(0x023A3894, BOOL, trap);
  u32 first = play(), manager = play();
  s32 event =
      gabi::call<s32>(0x02543F10, ptr(manager, 0x52C4), ptr(0x10031220), 255);
  if (gabi::call<u32>(0x02544044, ptr(first, 0x52C4), event)) {
    manager = play();
    s32 staff = gabi::call<s32>(0x02542D88, ptr(manager, 0x52C4),
                                ptr(0x10031218), 0, 0);
    if (staff != -1) {
      manager = play();
      u32 cut = gabi::call<u32>(0x02544830, ptr(manager, 0x52C4), staff);
      if (sameString(cut, 0x10031210)) {
        gabi::call(0x025D57E0, trap);
        return 0;
      }
    }
  }
  return 1;
}
VERIFY(0x023A3894, tntrap_demoWait2);
BOOL tntrap_endWait(Trap *trap) {
  WWHD_FUNC(0x023A3978, BOOL, trap);
  s32 event = trap->eventId;
  u32 world = play();
  if (!gabi::call<s32>(0x025440C8, ptr(world, 0x52C4), event))
    return 1;
  world = play();
  write<u16>(world, 0x52B8, read<u16>(world, 0x52B8) | 8);
  u32 type = trap->trapType;
  if (type == 0)
    gabi::call(0x025E1988, 0x806);
  else if (type == 2) {
    u32 sw = trap->switchSave2;
    if (sw != 255)
      gabi::call(0x025B9E38, ptr(save(), 0x20), sw,
                 (s32)(s8)read<u8>(gabi::ea(trap), 0x2FE));
  }
  gabi::call(0x025D57E0, trap);
  return 0;
}
VERIFY(0x023A3978, tntrap_endWait);
BOOL tntrap_hideWait(Trap *trap) {
  WWHD_FUNC(0x023A3A50, BOOL, trap);
  u32 sw = trap->switchSave2;
  if (sw != 255 && switchOn(trap, sw) == 1) {
    u32 boat = read<u32>(play(), 0x5B3C);
    if (boat) {
      write<u32>(boat, 0x644, read<u32>(boat, 0x644) | 0x00800000u);
      u32 world = play();
      if (!gabi::call<s32>(0x024EEA6C, ptr(world, 0x12A0),
                           ptr(trap->background), trap))
        gabi::call(0x023A2A5C, trap, 0);
    }
  }
  return 0;
}
VERIFY(0x023A3A50, tntrap_hideWait);
void tntrap_staticInit() {
  WWHD_FUNC(0x023A3AEC, void);
  for (u32 i = 0; i < 4; ++i)
    write<u32>(0x1046C514, i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CD3E4));
  write<f32>(0x1046C508, 0, -3.1415927410125732f);
  write<f32>(0x1046C50C, 0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046C510));
  gabi::call(0x028F026C, ptr(0x101CD3F0));
  gabi::call(0x028EAB2C, ptr(0x1046C511));
  gabi::call(0x028F026C, ptr(0x101CD3FC));
}
VERIFY(0x023A3AEC, tntrap_staticInit);
void *tntrap_triangleCtor(void *self) {
  WWHD_FUNC(0x023A3B80, void *, self);
  if (!self)
    self = gabi::call<void *>(0x0273AD10, 0x150);
  if (self) {
    u32 a = gabi::ea(self);
    gabi::call(0x02515FB8, self);
    write<u32>(a, 0x114, 0x100015A8);
    write<u32>(a, 0x110, 0x10031084);
    gabi::call(0x02019040, ptr(a, 0x118));
    write<u32>(a, 0x3C, 0x1004B010);
    write<u32>(a, 0x128, 0x1004B058);
    write<u32>(a, 0x114, 0x1004B068);
  }
  return self;
}
VERIFY(0x023A3B80, tntrap_triangleCtor);
void tntrap_staticDestructor(void *self, s32 flags) {
  WWHD_FUNC(0x023A3C0C, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x023A3C0C, tntrap_staticDestructor);
void tntrap_dummy(Trap *trap) { WWHD_FUNC(0x023A3C20, void, trap); }
VERIFY(0x023A3C20, tntrap_dummy);
void tntrap_actorDestructor(Trap *trap, s32 flags) {
  WWHD_FUNC(0x023A3C24, void, trap, flags);
  if (trap) {
    gabi::call(0x028F0164, trap->triangles, 8, 0x150, ptr(0x025159F8), 0, 0);
    gabi::call(0x02515860, &trap->status, 2);
    gabi::call(0x025D50BC, trap, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, trap);
  }
}
VERIFY(0x023A3C24, tntrap_actorDestructor);
void tntrap_baseNoop(Trap *trap) { WWHD_FUNC(0x023A3CA4, void, trap); }
VERIFY(0x023A3CA4, tntrap_baseNoop);
u32 tntrap_parameter(Trap *trap, u32 bits, u32 shift) {
  WWHD_FUNC(0x023A3CA8, u32, trap, bits, shift);
  u32 value = read<u32>(gabi::ea(trap), 0xB0),
      one = bits & 32 ? 0u : 1u << (bits & 31);
  u32 mask = one - 1;
  return (shift & 32 ? 0u : value >> (shift & 31)) & mask;
}
VERIFY(0x023A3CA8, tntrap_parameter);

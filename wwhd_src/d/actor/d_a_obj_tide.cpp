// Full actual HD TU023A0418..023A2153: tide modes, resources, audio and
// wrappers.
#include "d/actor/d_a_obj_tide.h"
#include "bindings.h"
using Tide = daObjTide::Act_c;
namespace {
template <class T> T read(u32 address, u32 offset = 0) {
  return *gabi::at<be<T>>(address + offset);
}
template <class T> void write(u32 address, u32 offset, T value) {
  *gabi::at<be<T>>(address + offset) = value;
}
void *ptr(u32 address, u32 offset = 0) {
  return gabi::at<void>(address + offset);
}
u32 play() { return gabi::call<u32>(0x025200D4); }
u32 save() { return read<u32>(0x101F84DC); }
u32 parameters(Tide *tide) { return 0x10030BBC + (u32)tide->type * 0x48; }
u32 resource(u32 archive, s32 index) {
  gabi::Local<daObjTide::Archive> name;
  name->name = archive;
  name->vtable = 0x10030B60;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), name.get(),
                         index);
}
void assertResource(u32 data, s32 line, u32 text) {
  if (!data)
    gabi::call(0x0273AA24, ptr(0x10030DF0), line, ptr(text));
}
} // namespace
void tide_soundInit(Tide *tide) {
  WWHD_FUNC(0x023A0418, void, tide);
  tide->outflow = 0;
  tide->gurgleId = -1;
  tide->outflowId = -1;
  tide->finished = 0;
  tide->upward = 0;
  tide->gurgle = 0;
  tide->upwardId = -1;
  tide->hasDungeonItem =
      (u8)(gabi::call<s32>(0x025B9100, ptr(save(), 0x798), 3) != 0);
}
VERIFY(0x023A0418, tide_soundInit);
s32 tide_methodCreate(Tide *tide) {
  WWHD_FUNC(0x023A0484, s32, tide);
  u32 a = gabi::ea(tide), flags = read<u32>(a, 0x2E4);
  if (!(flags & 8)) {
    if (tide) {
      gabi::call(0x024F1D40, tide);
      flags = read<u32>(a, 0x2E4);
      write<u32>(a, 0xB4, 0x10030FD8);
    }
    write<u32>(a, 0x2E4, flags | 8);
  }
  tide->type = gabi::call<s32>(0x023A2138, tide, 3, 16);
  gabi::call(0x023A0418, tide);
  tide->ready = 1;
  if (tide->type == 3) {
    gabi::call(0x025B8B7C, ptr(save(), 0x1178), 0x401);
    if (!tide->ready)
      return 5;
  }
  u32 archive = read<u32>(parameters(tide));
  s32 phase = gabi::call<s32>(0x02520460, &tide->phase, ptr(archive));
  if (phase == 4) {
    u32 data = parameters(tide);
    if (read<u8>(data, 0x40)) {
      f32 factor = 1.f - read<f32>(0x101D5F28);
      f32 height = (f32)read<s16>(data, 0x20);
      tide->current.pos.y = gabi::fmadds(factor, height, tide->home.pos.y);
      data = parameters(tide);
    }
    phase = gabi::call<s32>(0x024F1D9C, tide, ptr(read<u32>(data)),
                            (s32)read<s16>(data, 6), ptr(0x024EE76C),
                            read<u32>(data, 0x1C));
    tide->registered = 1;
    if (phase != 4 && phase != 5)
      gabi::call(0x0273AA24, ptr(0x10030D78), 0x283, ptr(0x10030D8C));
  }
  return phase;
}
VERIFY(0x023A0484, tide_methodCreate);
s32 tide_methodDelete(Tide *tide) {
  WWHD_FUNC(0x023A0658, s32, tide);
  s32 result = 1;
  if (tide->ready) {
    result = gabi::call<s32>(0x024F1F64, tide);
    gabi::call(0x025204C8, &tide->phase, ptr(read<u32>(parameters(tide))));
  }
  return result;
}
VERIFY(0x023A0658, tide_methodDelete);
BOOL tide_createHeap(Tide *tide) {
  WWHD_FUNC(0x023A06C0, BOOL, tide);
  u32 archive = read<u32>(parameters(tide));
  u32 modelData = resource(archive, read<s16>(parameters(tide), 4));
  assertResource(modelData, 0x1BA, 0x10030DD0);
  u32 flags = read<u32>(parameters(tide), 0x10);
  tide->model =
      flags ? gabi::call<u32>(0x025E38E0, ptr(modelData), 0x80000, flags)
            : gabi::call<u32>(0x025E38E0, ptr(modelData), 0, 0x11020203);
  bool bckOK = true, brkOK = true, btkOK = true, secondModelOK = true,
       secondBrkOK = true;
  s32 index = read<s16>(parameters(tide), 8);
  if (index >= 0) {
    u32 animation = resource(archive, index);
    assertResource(animation, 0x1CB, 0x10030E04);
    u32 bck = gabi::call<u32>(0x0273AD10, 0x8C);
    if (bck) {
      gabi::call(0x027F2BC0, ptr(bck), 0);
      write<u32>(bck, 0x10, 0x1016E54C);
      gabi::call(0x027DA984, ptr(bck, 0x14));
      write<u32>(bck, 0x48, 0x1016D820);
      write<u32>(bck, 0x10, 0x10030B78);
      write<u32>(bck, 0x58, 0);
      write<u32>(bck, 0x7C, 0);
      write<u32>(bck, 0x80, 0);
      write<u32>(bck, 0x84, 0);
      write<u32>(bck, 0x88, 0);
    }
    tide->bck = bck;
    if (!bck || !gabi::call<s32>(0x025E8508, ptr(bck), ptr(modelData),
                                 ptr(animation), 1, 0, 0, 0x12B, 0, 1.f))
      bckOK = false;
  }
  index = read<s16>(parameters(tide), 0xA);
  if (index >= 0) {
    u32 animation = resource(archive, index);
    assertResource(animation, 0x1DE, 0x10030E10);
    u32 brk = gabi::call<u32>(0x0273AD10, 0x78);
    if (brk)
      brk = gabi::call<u32>(0x025E80D0, ptr(brk));
    tide->brk = brk;
    if (!brk || !gabi::call<s32>(0x025E8154, ptr(brk), ptr(modelData),
                                 ptr(animation), 1, 2, 0, -1, 0, 0, 1.f))
      brkOK = false;
  }
  index = read<s16>(parameters(tide), 0xC);
  if (index >= 0) {
    u32 animation = resource(archive, index);
    assertResource(animation, 0x1EB, 0x10030E1C);
    u32 btk = gabi::call<u32>(0x0273AD10, 0x74);
    if (btk)
      btk = gabi::call<u32>(0x025E7C6C, ptr(btk));
    tide->btk = btk;
    if (!btk || !gabi::call<s32>(0x025E7CE0, ptr(btk), ptr(modelData),
                                 ptr(animation), 1, 2, 0, -1, 0, 0, 1.f))
      btkOK = false;
  }
  u32 data = parameters(tide);
  if (read<s16>(data, 0x14) >= 0 && read<s16>(data, 0x16) >= 0) {
    u32 secondData = resource(archive, read<s16>(data, 0x14));
    assertResource(secondData, 0x1FC, 0x10030E28);
    tide->secondModel = gabi::call<u32>(0x025E38E0, ptr(secondData), 0x80000,
                                        read<u32>(parameters(tide), 0x18));
    if (!tide->secondModel)
      secondModelOK = false;
    u32 animation = resource(archive, read<s16>(parameters(tide), 0x16));
    assertResource(animation, 0x206, 0x10030DE0);
    u32 brk = gabi::call<u32>(0x0273AD10, 0x78);
    if (brk)
      brk = gabi::call<u32>(0x025E80D0, ptr(brk));
    tide->secondBrk = brk;
    if (!brk ||
        !gabi::call<s32>(0x025E8154, ptr(brk), ptr(secondData), ptr(animation),
                         1, 0, 0, -1, 0, 0, read<f32>(parameters(tide), 0x44)))
      secondBrkOK = false;
    else {
      u32 result = tide->secondBrk;
      write<f32>(result, 4, (f32)read<s16>(result, 0xA));
    }
  }
  return tide->model && bckOK && brkOK && btkOK && secondModelOK && secondBrkOK;
}
VERIFY(0x023A06C0, tide_createHeap);
void tide_setMatrix(Tide *tide) {
  WWHD_FUNC(0x023A0BA8, void, tide);
  if (tide->secondModel) {
    mDoMtx_stack_c::transS(tide->home.pos.x, tide->home.pos.y,
                           tide->home.pos.z);
    gabi::call(0x025F1B48, mDoMtx_stack_c::get(), (s32)(s16)tide->shape_angle.x,
               (s32)(s16)tide->shape_angle.y, (s32)(s16)tide->shape_angle.z);
    J3DModel_setBaseTRMtx(gabi::at<J3DModel>(tide->secondModel),
                          mDoMtx_stack_c::get());
  }
  mDoMtx_stack_c::transS(tide->current.pos.x, tide->current.pos.y,
                         tide->current.pos.z);
  gabi::call(0x025F1B48, mDoMtx_stack_c::get(), (s32)(s16)tide->shape_angle.x,
             (s32)(s16)tide->shape_angle.y, (s32)(s16)tide->shape_angle.z);
  J3DModel_setBaseTRMtx(gabi::at<J3DModel>(tide->model), mDoMtx_stack_c::get());
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), ptr(0x1046C4BC));
}
VERIFY(0x023A0BA8, tide_setMatrix);
void tide_initMatrix(Tide *tide) {
  WWHD_FUNC(0x023A0D14, void, tide);
  J3DModel_setBaseScale(gabi::at<J3DModel>(tide->model), &tide->scale);
  if (tide->secondModel)
    J3DModel_setBaseScale(gabi::at<J3DModel>(tide->secondModel), &tide->scale);
  gabi::call(0x023A0BA8, tide);
}
VERIFY(0x023A0D14, tide_initMatrix);
void tide_preInit(Tide *tide) {
  WWHD_FUNC(0x023A0D58, void, tide);
  if (read<u8>(parameters(tide), 0x40))
    gabi::call(0x0273AA24, ptr(0x10030E3C), 0x351, ptr(0x10030E50));
  tide->mode = 1;
  tide->stage = 0;
}
VERIFY(0x023A0D58, tide_preInit);
void tide_normalInit(Tide *tide) {
  WWHD_FUNC(0x023A0DC0, void, tide);
  tide->mode = 0;
}
VERIFY(0x023A0DC0, tide_normalInit);
BOOL tide_Create(Tide *tide) {
  WWHD_FUNC(0x023A0DCC, BOOL, tide);
  u32 model = tide->model;
  write<u32>(gabi::ea(tide), 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x023A0D14, tide);
  u32 data = parameters(tide);
  gabi::call(0x025D674C, tide, (f32)read<s16>(data, 0x34),
             (f32)read<s16>(data, 0x36), (f32)read<s16>(data, 0x38),
             (f32)read<s16>(data, 0x3A), (f32)read<s16>(data, 0x3C),
             (f32)read<s16>(data, 0x3E));
  tide->timer = 0;
  tide->stage = 0;
  tide->deleteRequested = 0;
  tide->step = 0.f;
  if (tide->type == 3)
    gabi::call(0x023A0D58, tide);
  else
    gabi::call(0x023A0DC0, tide);
  return 1;
}
VERIFY(0x023A0DCC, tide_Create);
BOOL tide_Execute(Tide *tide, be<u32> *matrixOut) {
  WWHD_FUNC(0x023A0F08, BOOL, tide, matrixOut);
  u32 descriptor = 0x10030E74 + (u32)tide->mode * 8;
  s16 slot = read<s16>(descriptor, 2);
  void *self = ptr(gabi::ea(tide) + (s32)read<s16>(descriptor));
  u32 target =
      slot < 0
          ? read<u32>(descriptor, 4)
          : read<u32>(read<u32>(gabi::ea(self), (s32)read<s16>(descriptor, 6)),
                      (u32)slot * 8 + 4);
  gabi::call_ptr(target, self);
  write<f32>(gabi::ea(tide), 0x380, tide->current.pos.y);
  gabi::call(0x023A0BA8, tide);
  *matrixOut = 0x1046C4BC;
  if (tide->deleteRequested)
    gabi::call(0x025D57E0, tide);
  return 1;
}
VERIFY(0x023A0F08, tide_Execute);
BOOL tide_Draw(Tide *tide) {
  WWHD_FUNC(0x023A0FE8, BOOL, tide);
  gabi::call(0x025626A4, ptr(gabi::call<u32>(0x02555D0C)), 2,
             &tide->current.pos, ptr(gabi::ea(tide), 0x110));
  u32 environment = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(environment), ptr(tide->model),
             ptr(gabi::ea(tide), 0x110));
  if (tide->secondModel)
    gabi::call(0x02562F5C, ptr(gabi::call<u32>(0x02555D0C)),
               ptr(tide->secondModel), ptr(gabi::ea(tide), 0x110));
  u32 animation = tide->bck;
  if (animation)
    gabi::call(0x025E86B8, ptr(animation), ptr(read<u32>(tide->model, 0xAC)),
               read<f32>(animation, 4));
  animation = tide->brk;
  if (animation)
    gabi::call(0x025E83FC, ptr(animation), ptr(read<u32>(tide->model, 0xAC)),
               read<f32>(animation, 4));
  animation = tide->btk;
  if (animation)
    gabi::call(0x025E7FC4, ptr(animation), ptr(read<u32>(tide->model, 0xAC)),
               read<f32>(animation, 4));
  animation = tide->secondBrk;
  if (animation && tide->secondModel)
    gabi::call(0x025E83FC, ptr(animation),
               ptr(read<u32>(tide->secondModel, 0xAC)),
               read<f32>(animation, 4));
  write<u32>(0x104B4634, 0, read<u32>(play(), 0x5D70));
  write<u32>(0x104B4634, 4, read<u32>(play(), 0x5D74));
  gabi::call(0x025E2DE0, ptr(tide->model), 0);
  write<u32>(0x104B4634, 0, read<u32>(play(), 0x5D78));
  write<u32>(0x104B4634, 4, read<u32>(play(), 0x5D7C));
  if (tide->secondModel)
    gabi::call(0x025E2DE0, ptr(tide->secondModel), 0);
  return 1;
}
VERIFY(0x023A0FE8, tide_Draw);
void tide_gurgleDelete(Tide *tide) {
  WWHD_FUNC(0x023A112C, void, tide);
  s32 id = tide->gurgleId;
  if (id != -1) {
    u32 effect = gabi::call<u32>(0x025DAC50, id);
    if (effect)
      gabi::call(0x025DAD48, ptr(effect));
    tide->gurgleId = -1;
  }
}
VERIFY(0x023A112C, tide_gurgleDelete);
void tide_outflowDelete(Tide *tide) {
  WWHD_FUNC(0x023A1178, void, tide);
  s32 id = tide->outflowId;
  if (id != -1) {
    u32 effect = gabi::call<u32>(0x025DAC50, id);
    if (effect)
      gabi::call(0x025DAD48, ptr(effect));
    tide->outflowId = -1;
  }
}
VERIFY(0x023A1178, tide_outflowDelete);
void tide_upwardDelete(Tide *tide) {
  WWHD_FUNC(0x023A11C4, void, tide);
  s32 id = tide->upwardId;
  if (id != -1) {
    u32 effect = gabi::call<u32>(0x025DAC50, id);
    if (effect)
      gabi::call(0x025DAD48, ptr(effect));
    tide->upwardId = -1;
  }
}
VERIFY(0x023A11C4, tide_upwardDelete);
BOOL tide_Delete(Tide *tide) {
  WWHD_FUNC(0x023A1210, BOOL, tide);
  gabi::call(0x023A112C, tide);
  gabi::call(0x023A1178, tide);
  gabi::call(0x023A11C4, tide);
  gabi::call(0x025E1B34, &tide->home.pos);
  return 1;
}
VERIFY(0x023A1210, tide_Delete);
void tide_moveToAim(Tide *tide, BOOL sound, f32 aim) {
  WWHD_FUNC(0x023A1258, void, tide, sound, aim);
  f32 difference = aim - (f32)tide->current.pos.y;
  if (std::fabs((f64)difference) < .1f) {
    tide->step = 0.f;
    tide->current.pos.y = aim;
    return;
  }
  u32 data = parameters(tide);
  f32 step = (f32)tide->step + read<f32>(data, 0x24);
  tide->step = step;
  f32 maximum = read<f32>(data, 0x28);
  if (step > maximum) {
    step = maximum;
    tide->step = step;
    data = parameters(tide);
  }
  f32 minimum = read<f32>(data, 0x2C);
  if (!(step > minimum)) {
    step = minimum + .001f;
    tide->step = step;
    data = parameters(tide);
    minimum = read<f32>(data, 0x2C);
  }
  gabi::call(0x0200ECD4, &tide->current.pos.y, aim, read<f32>(data, 0x30), step,
             minimum);
  if (sound)
    gabi::call(0x025E1988, difference > 0.f ? 0x701B : 0x701C);
}
VERIFY(0x023A1258, tide_moveToAim);
bool tide_checkDemo(Tide *tide) {
  WWHD_FUNC(0x023A1398, bool, tide);
  if (!read<u8>(play(), 0x5292))
    return false;
  return gabi::call<s32>(0x025445B8, ptr(play(), 0x52C4), ptr(0x10030E9C)) != 0;
}
VERIFY(0x023A1398, tide_checkDemo);
BOOL tide_demoUpStart(Tide *tide) {
  WWHD_FUNC(0x023A13F4, BOOL, tide);
  u32 index = read<u8>(gabi::ea(tide), 0x2DC);
  if (index == 0 || index > 32)
    return 0;
  u32 manager = read<u32>(0x101D5FFC);
  if (!manager) {
    gabi::call(0x0273AA24, ptr(0x10030BB0), 0x23A, ptr(0x10030BA0));
    manager = read<u32>(0x101D5FFC);
  }
  u32 actor = gabi::call<u32>(0x02526E70, ptr(manager), index);
  if (!actor || !(read<u16>(actor, 4) & 1))
    return 0;
  return read<u32>(actor, 0x4C) == 1;
}
VERIFY(0x023A13F4, tide_demoUpStart);
void tide_normal(Tide *tide) {
  WWHD_FUNC(0x023A14A8, void, tide);
  u32 data = parameters(tide);
  f32 level = read<f32>(0x101D5F28), home = tide->home.pos.y;
  u32 flags = read<u32>(0x101D5F2C), low = flags & 1;
  tide->current.pos.y =
      gabi::fmadds(1.f - level, (f32)read<s16>(data, 0x20), home);
  s16 timer = tide->timer;
  if (timer > 0) {
    timer = (s16)(timer - 1);
    tide->timer = timer;
    if (!timer) {
      write<f32>(tide->secondBrk, 4, 0.f);
      write<f32>(tide->secondBrk, 0, read<f32>(parameters(tide), 0x44));
    }
  }
  u32 animation = tide->secondBrk;
  if (!low && (flags & 2))
    tide->timer = read<s16>(data, 0x42);
  if (animation)
    gabi::call(0x025E742C, ptr(animation));
  animation = tide->btk;
  if (animation)
    gabi::call(0x025E742C, ptr(animation));
  if (!(flags & 4)) {
    if (low) {
      if (level != 1.f)
        gabi::call(0x025E1988, 0x701B);
    } else if (level != 0.f)
      gabi::call(0x025E1988, 0x701C);
  }
}
VERIFY(0x023A14A8, tide_normal);
void tide_demoInit(Tide *tide) {
  WWHD_FUNC(0x023A1644, void, tide);
  if (read<u8>(parameters(tide), 0x40))
    gabi::call(0x0273AA24, ptr(0x10030EA4), 0x3BF, ptr(0x10030EB8));
  tide->mode = 3;
  tide->stage = 0;
}
VERIFY(0x023A1644, tide_demoInit);
void tide_flowInit(Tide *tide) {
  WWHD_FUNC(0x023A16AC, void, tide);
  if (read<u8>(parameters(tide), 0x40))
    gabi::call(0x0273AA24, ptr(0x10030EDC), 0x373, ptr(0x10030EF0));
  u32 animation = tide->bck;
  if (animation)
    write<f32>(animation, 4, (f32)read<s16>(animation, 0xA));
  animation = tide->brk;
  if (animation) {
    write<u16>(animation, 0xC, 400);
    write<f32>(tide->brk, 4, 401.f);
  }
  animation = tide->btk;
  if (animation) {
    write<u16>(animation, 0xC, 400);
    write<f32>(tide->btk, 4, 401.f);
  }
  tide->stage = 2;
  tide->latched = 0;
  tide->countdown = 300;
  tide->mode = 2;
}
VERIFY(0x023A16AC, tide_flowInit);
void tide_pre(Tide *tide) {
  WWHD_FUNC(0x023A1798, void, tide);
  bool demo = gabi::call<s32>(0x023A1398, tide) != 0;
  f32 home = tide->home.pos.y;
  if (demo) {
    tide->current.pos.y = home;
    if (tide->registered) {
      gabi::call(0x020087EC, ptr(play(), 0x12A0), ptr(tide->background));
      tide->registered = 0;
    }
    gabi::call(0x023A1644, tide);
  } else {
    tide->current.pos.y = home + 100.f;
    gabi::call(0x023A16AC, tide);
  }
}
VERIFY(0x023A1798, tide_pre);
void tide_gurgleStart(Tide *tide) {
  WWHD_FUNC(0x023A182C, void, tide);
  if (tide->gurgleId != -1)
    gabi::call(0x0273AA24, ptr(0x10030F14), 0x461, ptr(0x10030F28));
  tide->gurgleId =
      gabi::call<s32>(0x025DADA4, 0x18, 0x702A, &tide->home.pos, 0, 0);
}
VERIFY(0x023A182C, tide_gurgleStart);
void tide_outflowStart(Tide *tide) {
  WWHD_FUNC(0x023A1894, void, tide);
  if (tide->outflowId != -1)
    gabi::call(0x0273AA24, ptr(0x10030F74), 0x473, ptr(0x10030F50));
  tide->outflowId =
      gabi::call<s32>(0x025DADA4, 0x18, 0x702B, &tide->home.pos, 0, 0);
}
VERIFY(0x023A1894, tide_outflowStart);
void tide_upwardStart(Tide *tide) {
  WWHD_FUNC(0x023A18FC, void, tide);
  if (tide->upwardId != -1)
    gabi::call(0x0273AA24, ptr(0x10030F88), 0x485, ptr(0x10030F9C));
  tide->upwardId =
      gabi::call<s32>(0x025DADA4, 0x18, 0x702C, &tide->current.pos, 0, 0);
}
VERIFY(0x023A18FC, tide_upwardStart);
void tide_updateUpward(Tide *tide) {
  WWHD_FUNC(0x023A1964, void, tide);
  s32 id = tide->upwardId;
  if (id != -1) {
    u32 effect = gabi::call<u32>(0x025DAC50, id);
    if (effect)
      write<f32>(effect, 0xE4, tide->current.pos.y);
  }
}
VERIFY(0x023A1964, tide_updateUpward);
void tide_soundSet(Tide *tide) {
  WWHD_FUNC(0x023A19AC, void, tide);
  tide->finished = 0;
  u8 item = tide->hasDungeonItem;
  u8 oldGurgle = tide->gurgle;
  tide->gurgle = 0;
  u8 oldOut = tide->outflow;
  tide->outflow = 0;
  u8 oldUp = tide->upward;
  tide->upward = 0;
  bool finished = item != 0;
  if (!finished) {
    f32 distance = ((f32)tide->home.pos.y + 2923.5f) - (f32)tide->current.pos.y;
    finished = std::fabs((f64)distance) < .01f;
  }
  if (finished)
    tide->finished = 1;
  else {
    tide->gurgle = 1;
    u8 stage = tide->stage;
    if ((stage == 1 && tide->registered) || stage == 2) {
      tide->outflow = 1;
      tide->upward = 1;
    }
  }
  if (tide->gurgle) {
    if (!oldGurgle)
      gabi::call(0x023A182C, tide);
  } else if (oldGurgle)
    gabi::call(0x023A112C, tide);
  if (tide->outflow) {
    if (!oldOut)
      gabi::call(0x023A1894, tide);
  } else if (oldOut)
    gabi::call(0x023A1178, tide);
  if (tide->upward) {
    if (!oldUp)
      gabi::call(0x023A18FC, tide);
    else
      gabi::call(0x023A1964, tide);
  } else if (oldUp)
    gabi::call(0x023A11C4, tide);
  if (tide->finished) {
    s32 reverb =
        gabi::call<s32>(0x02520540, (s32)(s8)read<u8>(gabi::ea(tide), 0x326));
    gabi::call(0x025E1A40, 0x702D, ptr(gabi::ea(tide), 0x37C), 0, reverb);
  }
}
VERIFY(0x023A19AC, tide_soundSet);
void tide_flow(Tide *tide) {
  WWHD_FUNC(0x023A1B78, void, tide);
  f32 aim = 2923.5f;
  bool stopped = false;
  bool latched = tide->latched != 0;
  if (!latched) {
    u32 sw = gabi::call<u32>(0x023A2138, tide, 8, 24);
    bool on = gabi::call<s32>(0x025BA0C0, ptr(save(), 0x20), sw,
                              (s32)(s8)read<u8>(gabi::ea(tide), 0x2FE)) != 0;
    if (on) {
      aim = (f32)tide->home.pos.y + aim;
      tide->latched = 1;
    } else
      aim = (f32)tide->home.pos.y + 100.f;
  } else {
    aim = (f32)tide->home.pos.y + aim;
    u32 sw = gabi::call<u32>(0x023A2138, tide, 8, 24);
    if (!gabi::call<s32>(0x025BA0C0, ptr(save(), 0x20), sw,
                         (s32)(s8)read<u8>(gabi::ea(tide), 0x2FE)))
      tide->current.pos.y = aim;
  }
  u32 animation = tide->bck;
  if (animation && gabi::call<s32>(0x025E742C, ptr(animation)))
    stopped = true;
  animation = tide->brk;
  if (animation)
    gabi::call(0x025E742C, ptr(animation));
  animation = tide->btk;
  if (animation)
    gabi::call(0x025E742C, ptr(animation));
  if (stopped)
    gabi::call(0x023A1258, tide, 0, aim);
  gabi::call(0x023A19AC, tide);
  if (gabi::call<s32>(0x025B8B94, ptr(save(), 0x1178), 0x401)) {
    s32 count = tide->countdown;
    if (count > 0)
      tide->countdown = count - 1;
    else
      tide->deleteRequested = 1;
  } else
    tide->countdown = 300;
}
VERIFY(0x023A1B78, tide_flow);
void tide_demo(Tide *tide) {
  WWHD_FUNC(0x023A1D68, void, tide);
  u8 stage = tide->stage;
  if (!stage) {
    if (gabi::call<s32>(0x023A13F4, tide)) {
      tide->stage = 1;
      tide->timer = 90;
    }
    stage = tide->stage;
  }
  f32 aim = (stage == 0 || stage == 1) ? (f32)tide->home.pos.y
                                       : (f32)tide->home.pos.y + 100.f;
  bool stopped = false;
  if (stage == 1) {
    s16 timer = (s16)((s32)(s16)tide->timer - 1);
    tide->timer = timer;
    if (timer <= 0) {
      tide->stage = 2;
      gabi::call(0x023A19AC, tide);
      return;
    }
    if (timer == 59) {
      u32 particle = read<u32>(play(), 0x5AB0);
      gabi::call(0x025A847C, ptr(particle), 0, 0x814D, &tide->current.pos, 0, 0,
                 255, 0, -1, 0, 0, 0);
      gabi::call(0x024EEA6C, ptr(play(), 0x12A0), ptr(tide->background), tide);
      tide->registered = 1;
      gabi::call(0x023A19AC, tide);
      return;
    }
  } else if (stage == 2 || stage == 3) {
    u32 animation = tide->bck;
    if (animation && gabi::call<s32>(0x025E742C, ptr(animation)))
      stopped = true;
    animation = tide->brk;
    if (animation) {
      gabi::call(0x025E742C, ptr(animation));
      animation = tide->brk;
      if ((f32)read<s16>(animation, 0xC) == 0.f &&
          gabi::call<s32>(0x027F2BF8, ptr(animation), 400.f))
        write<u16>(tide->brk, 0xC, 400);
    }
    animation = tide->btk;
    if (animation) {
      gabi::call(0x025E742C, ptr(animation));
      animation = tide->btk;
      if ((f32)read<s16>(animation, 0xC) == 0.f &&
          gabi::call<s32>(0x027F2BF8, ptr(animation), 400.f))
        write<u16>(tide->btk, 0xC, 400);
    }
    if (stopped)
      gabi::call(0x023A1258, tide, 0, aim);
  }
  gabi::call(0x023A19AC, tide);
}
VERIFY(0x023A1D68, tide_demo);
s32 tide_createWrapper(Tide *tide) {
  WWHD_FUNC(0x023A2004, s32, tide);
  return gabi::call<s32>(0x023A0484, tide);
}
VERIFY(0x023A2004, tide_createWrapper);
s32 tide_deleteWrapper(Tide *tide) {
  WWHD_FUNC(0x023A2008, s32, tide);
  return gabi::call<s32>(0x023A0658, tide);
}
VERIFY(0x023A2008, tide_deleteWrapper);
s32 tide_executeWrapper(Tide *tide) {
  WWHD_FUNC(0x023A200C, s32, tide);
  return gabi::call<s32>(0x024F1E9C, tide);
}
VERIFY(0x023A200C, tide_executeWrapper);
s32 tide_drawWrapper(Tide *tide) {
  WWHD_FUNC(0x023A2010, s32, tide);
  return gabi::call_ptr<s32>(read<u32>(read<u32>(gabi::ea(tide), 0xB4), 0x2C),
                             tide);
}
VERIFY(0x023A2010, tide_drawWrapper);
s32 tide_isDeleteWrapper(Tide *tide) {
  WWHD_FUNC(0x023A2020, s32, tide);
  return gabi::call_ptr<s32>(read<u32>(read<u32>(gabi::ea(tide), 0xB4), 0x3C),
                             tide);
}
VERIFY(0x023A2020, tide_isDeleteWrapper);
void tide_staticInit() {
  WWHD_FUNC(0x023A2030, void);
  for (u32 i = 0; i < 4; i++)
    write<u32>(0x1046C4AC, i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CD23C));
  write<f32>(0x1046C4A0, 0, read<f32>(0x10030FCC));
  write<f32>(0x1046C4A4, 0, read<f32>(0x10030FD0));
  gabi::call(0x028ED6F8, ptr(0x1046C4A8));
  gabi::call(0x028F026C, ptr(0x101CD248));
  gabi::call(0x028EAB2C, ptr(0x1046C4A9));
  gabi::call(0x028F026C, ptr(0x101CD254));
}
VERIFY(0x023A2030, tide_staticInit);
void tide_staticDestructor(void *object, s32 flags) {
  WWHD_FUNC(0x023A20C4, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x023A20C4, tide_staticDestructor);
BOOL tide_baseIsDelete(Tide *tide) {
  WWHD_FUNC(0x023A20D8, BOOL, tide);
  return 1;
}
VERIFY(0x023A20D8, tide_baseIsDelete);
void tide_baseNoop(Tide *tide) { WWHD_FUNC(0x023A20E0, void, tide); }
VERIFY(0x023A20E0, tide_baseNoop);
void tide_actorDestructor(Tide *tide, s32 flags) {
  WWHD_FUNC(0x023A20E4, void, tide, flags);
  if (tide) {
    gabi::call(0x025D50BC, tide, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, tide);
  }
}
VERIFY(0x023A20E4, tide_actorDestructor);
u32 tide_parameter(Tide *tide, u32 bits, u32 shift) {
  WWHD_FUNC(0x023A2138, u32, tide, bits, shift);
  u32 mask = (bits & 32) ? 0u : 1u << (bits & 31),
      value =
          (shift & 32) ? 0u : read<u32>(gabi::ea(tide), 0xB0) >> (shift & 31);
  return value & (mask - 1);
}
VERIFY(0x023A2138, tide_parameter);

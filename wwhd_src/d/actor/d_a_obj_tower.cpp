/* Tower of the Gods exterior; actual HD TU023A6490..023A6AF7.
 * Includes trailing destructor and SafeString virtual noop. */
#include "d/actor/d_a_obj_tower.h"
#include "bindings.h"
namespace {
template <class T> T read(u32 a, u32 o = 0) { return *gabi::at<be<T>>(a + o); }
template <class T> void write(u32 a, u32 o, T v) {
  *gabi::at<be<T>>(a + o) = v;
}
void *ptr(u32 a, u32 o = 0) { return gabi::at<void>(a + o); }
struct TowerArchive {
  be<u32> name, vt;
};
u32 resource(s32 index) {
  gabi::Local<TowerArchive> archive;
  archive->name = 0x10031688;
  archive->vt = 0x10031644;
  return gabi::call<u32>(0x026066C4, ptr(read<u32>(0x101F4F28)), archive.get(),
                         index);
}
} // namespace
BOOL tower_createHeap(daObjTower_c *tower) {
  WWHD_FUNC(0x023A6490, BOOL, tower);
  u32 modelData = resource(4);
  if (!modelData)
    gabi::call(0x0273AA24, ptr(0x10031690), 86, ptr(0x100316A4));
  tower->towerModel =
      gabi::call<u32>(0x025E38E0, ptr(modelData), 0, 0x11020203);
  if (!tower->towerModel)
    return 0;
  mDoMtx_stack_c::transS(tower->current.pos.x, tower->current.pos.y,
                         tower->current.pos.z);
  mDoMtx_stack_c::YrotM(tower->shape_angle.y);
  mDoMtx_stack_c::scaleM(tower->scale.x, tower->scale.y, tower->scale.z);
  gabi::call(0x028E90D4, mDoMtx_stack_c::get(), &tower->backgroundMatrix);
  tower->background = gabi::call<u32>(0x024F23F4, ptr(0));
  if (!tower->background)
    return 0;
  u32 collision = resource(7);
  return gabi::call<s32>(0x0200A030, ptr(tower->background), ptr(collision), 1,
                         &tower->backgroundMatrix)
             ? 0
             : 1;
}
VERIFY(0x023A6490, tower_createHeap);
BOOL tower_heapCallback(daObjTower_c *tower) {
  WWHD_FUNC(0x023A65C8, BOOL, tower);
  return tower_createHeap(tower);
}
VERIFY(0x023A65C8, tower_heapCallback);
void tower_setMatrix(daObjTower_c *tower) {
  WWHD_FUNC(0x023A65CC, void, tower);
  J3DModel_setBaseScale(gabi::at<J3DModel>((u32)tower->towerModel),
                        &tower->scale);
  mDoMtx_stack_c::transS(tower->current.pos.x, tower->current.pos.y,
                         tower->current.pos.z);
  mDoMtx_stack_c::YrotM(tower->current.angle.y);
  J3DModel_setBaseTRMtx(gabi::at<J3DModel>((u32)tower->towerModel),
                        mDoMtx_stack_c::get());
}
VERIFY(0x023A65CC, tower_setMatrix);
void tower_createInit(daObjTower_c *tower) {
  WWHD_FUNC(0x023A66A4, void, tower);
  u32 model = tower->towerModel;
  tower->cullMtx = model ? model + 0xC8 : 0;
  gabi::call(0x025D674C, tower, -30000.f, -5000.f, -30000.f, 30000.f, 40000.f,
             30000.f);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, ptr(play, 0x12A0), ptr(tower->background), tower);
  tower->backgroundRegistered = 1;
  tower_setMatrix(tower);
}
VERIFY(0x023A66A4, tower_createInit);
s32 tower_create(daObjTower_c *tower) {
  WWHD_FUNC(0x023A6734, s32, tower);
  if (!(tower->actor_condition & 8)) {
    if (tower) {
      fopAc_ac_c_ct(tower);
      tower->__vtbl = 0x1003165C;
    }
    tower->actor_condition = tower->actor_condition | 8;
  }
  tower->backgroundRegistered = 0;
  u32 save = read<u32>(0x101F84DC);
  if (!gabi::call<s32>(0x025B8B94, ptr(save, 0x644), 0x1E40))
    return 3;
  s32 phase = gabi::call<s32>(0x02520460, &tower->phase, ptr(0x10031638));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, tower, ptr(0x023A65C8), 0x1C6C0))
      return 5;
    tower_createInit(tower);
  }
  return phase;
}
VERIFY(0x023A6734, tower_create);
BOOL tower_delete(daObjTower_c *tower) {
  WWHD_FUNC(0x023A683C, BOOL, tower);
  if (tower->backgroundRegistered) {
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, ptr(play, 0x12A0), ptr(tower->background));
  }
  if (read<u8>(gabi::ea(tower), 0xD) != 3)
    gabi::call(0x025204C8, &tower->phase, ptr(0x100316C8));
  return 1;
}
VERIFY(0x023A683C, tower_delete);
BOOL tower_draw(daObjTower_c *tower) {
  WWHD_FUNC(0x023A68A0, BOOL, tower);
  auto light = dKy_getEnvlight();
  settingTevStruct(light, 1, &tower->current.pos, &tower->tevStr);
  light = dKy_getEnvlight();
  setLightTevColorType(light, gabi::at<J3DModel>((u32)tower->towerModel),
                       &tower->tevStr);
  u32 play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 0, read<u32>(play, 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 4, read<u32>(play, 0x5D74));
  mDoExt_modelUpdateDL(gabi::at<J3DModel>((u32)tower->towerModel), 0);
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 0, read<u32>(play, 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  write<u32>(0x104B4634, 4, read<u32>(play, 0x5D7C));
  return 1;
}
VERIFY(0x023A68A0, tower_draw);
BOOL tower_execute(daObjTower_c *tower) {
  WWHD_FUNC(0x023A6938, BOOL, tower);
  u8 id = tower->demoActorID;
  if (id && id <= 0x20) {
    u32 manager = read<u32>(0x101D5FFC);
    if (!manager) {
      gabi::call(0x0273AA24, ptr(0x1003167C), 570, ptr(0x1003166C));
      manager = read<u32>(0x101D5FFC);
    }
    u32 actor = gabi::call<u32>(0x02526E70, ptr(manager), id);
    if (actor && (read<u16>(actor, 4) & 2)) {
      tower->current.pos.x = read<f32>(actor, 8);
      tower->current.pos.y = read<f32>(actor, 0xC);
      tower->current.pos.z = read<f32>(actor, 0x10);
    }
  }
  tower_setMatrix(tower);
  return 1;
}
VERIFY(0x023A6938, tower_execute);
void tower_staticInit() {
  WWHD_FUNC(0x023A69F0, void);
  for (u32 o = 0; o < 16; o += 4)
    write<u32>(0x1046C584, o, 0);
  gabi::call(0x028F026C, ptr(0x101CD550));
  write<f32>(0x1046C578, 0, -3.1415927410125732f);
  write<f32>(0x1046C57C, 0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046C580));
  gabi::call(0x028F026C, ptr(0x101CD55C));
  gabi::call(0x028EAB2C, ptr(0x1046C581));
  gabi::call(0x028F026C, ptr(0x101CD568));
}
VERIFY(0x023A69F0, tower_staticInit);
void tower_staticDtor(void *object, u32 flags) {
  WWHD_FUNC(0x023A6A84, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x023A6A84, tower_staticDtor);
BOOL tower_IsDelete(daObjTower_c *tower) {
  WWHD_FUNC(0x023A6A98, BOOL, tower);
  return 1;
}
VERIFY(0x023A6A98, tower_IsDelete);
void tower_destructor(daObjTower_c *tower, u32 flags) {
  WWHD_FUNC(0x023A6AA0, void, tower, flags);
  if (tower) {
    gabi::call(0x025D50BC, tower, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, tower);
  }
}
VERIFY(0x023A6AA0, tower_destructor);
void tower_stringNoop(void *object) { WWHD_FUNC(0x023A6AF4, void, object); }
VERIFY(0x023A6AF4, tower_stringNoop);

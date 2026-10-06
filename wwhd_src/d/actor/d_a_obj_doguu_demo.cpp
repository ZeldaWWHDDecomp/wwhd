/* WWHD DoguuD. Ported from zeldaret/tww with HD layout/behavior. */
#include "d/actor/d_a_obj_doguu_demo.h"
namespace daObjDoguuDemo {
static void *resource(s32 index) {
  gabi::Local<SafeString> name;
  name->mStringTop = 0x10027618;
  name->__vtbl = 0x100275D4;
  u32 controller = gabi::load<u32>(0x101F4F28);
  return gabi::call<void *>(0x026066C4, gabi::at<u8>(controller), name.get(),
                            index);
}
BOOL Act_c::CreateHeap() {
  WWHD_FUNC(0x023371E8, BOOL, this);
  mUnusedParam = mParameters;
  void *data = resource(4);
  if (!data)
    gabi::call(0x0273AA24, STR(0x10027620), 0x65, STR(0x10027638));
  J3DModel *model = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203);
  mModel = model;
  if (!model)
    return FALSE;
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
  gabi::call(0x028E93CC, matrix, x, y, z);
  gabi::call(0x025F1C28, matrix, (s16)shape_angle.y);
  x = scale.x;
  y = scale.y;
  z = scale.z;
  gabi::call(0x025F2518, x, y, z);
  gabi::call(0x028E90D4, matrix, &mMatrix);
  dBgW *bg = gabi::call<dBgW *>(0x024F23F4, nullptr);
  mBgW = bg;
  if (!bg)
    return FALSE;
  data = resource(7);
  bg = mBgW;
  return gabi::call<s32>(0x0200A030, bg, data, 1, &mMatrix) == 0;
}
VERIFY(0x023371E8, &Act_c::CreateHeap);
BOOL CheckCreateHeap(Act_c *actor) {
  WWHD_FUNC(0x02337328, BOOL, actor);
  return actor->CreateHeap();
}
VERIFY(0x02337328, CheckCreateHeap);
void Act_c::set_mtx() {
  WWHD_FUNC(0x0233732C, void, this);
  f32 x = scale.x, y = scale.y, z = scale.z;
  u32 model = gabi::ea((J3DModel *)mModel);
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  x = current.pos.x;
  y = current.pos.y;
  z = current.pos.z;
  gabi::call(0x028E93CC, matrix, x, y, z);
  gabi::call(0x025F1C28, matrix, (s16)current.angle.y);
  f32 values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = gabi::load<f32>(0x1048D0CC + 4 * i);
  model = gabi::ea((J3DModel *)mModel);
  for (u32 i = 0; i < 12; ++i)
    gabi::store<f32>(model + 0xC8 + 4 * i, values[i]);
}
VERIFY(0x0233732C, &Act_c::set_mtx);
void Act_c::CreateInit() {
  WWHD_FUNC(0x02337404, void, this);
  u32 model = gabi::ea((J3DModel *)mModel);
  gabi::store<u32>(gabi::ea(this) + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, this, -30000.0f, -5000.0f, -30000.0f, 30000.0f,
             40000.0f, 30000.0f);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, gabi::at<u8>(play + 0x12A0), (dBgW *)mBgW, this);
  mRegistered = 1;
  set_mtx();
}
VERIFY(0x02337404, &Act_c::CreateInit);
s32 Create(Act_c *actor) {
  WWHD_FUNC(0x02337494, s32, actor);
  u32 base = gabi::ea(actor), status = gabi::load<u32>(base + 0x2E4);
  if (!(status & 8)) {
    if (actor) {
      gabi::call(0x025D4ED0, actor);
      status = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x100275EC);
    }
    gabi::store<u32>(base + 0x2E4, status | 8);
  }
  actor->mRegistered = 0;
  s32 phase = gabi::call<s32>(0x02520460, &actor->mPhs, STR(0x100275CC));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, actor, gabi::at<u8>(0x02337328), 0x1460))
      return 5;
    actor->CreateInit();
  }
  return phase;
}
VERIFY(0x02337494, Create);
BOOL Delete(Act_c *actor) {
  WWHD_FUNC(0x02337560, BOOL, actor);
  if (gabi::load<u32>(gabi::ea(actor) + 0xF4) && actor->mRegistered) {
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, gabi::at<u8>(play + 0x12A0), (dBgW *)actor->mBgW);
  }
  gabi::call(0x025204C8, &actor->mPhs, STR(0x1002765C));
  return TRUE;
}
VERIFY(0x02337560, Delete);
BOOL Draw(Act_c *actor) {
  WWHD_FUNC(0x023375C4, BOOL, actor);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(env), 1, &actor->current.pos,
             gabi::at<u8>(gabi::ea(actor) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, gabi::at<u8>(env), (J3DModel *)actor->mModel,
             gabi::at<u8>(gabi::ea(actor) + 0x110));
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
  gabi::call(0x025E2DE0, (J3DModel *)actor->mModel, 0);
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
  return TRUE;
}
VERIFY(0x023375C4, Draw);
BOOL Execute(Act_c *actor) {
  WWHD_FUNC(0x0233765C, BOOL, actor);
  u8 demo = gabi::load<u8>(gabi::ea(actor) + 0x2DC);
  if (demo && demo <= 32) {
    u32 object = gabi::load<u32>(0x101D5FFC);
    if (!object) {
      gabi::call(0x0273AA24, STR(0x1002760C), 0x23A, STR(0x100275FC));
      object = gabi::load<u32>(0x101D5FFC);
    }
    u32 playback = gabi::call<u32>(0x02526E70, gabi::at<u8>(object), demo);
    if (playback && (gabi::load<u16>(playback + 4) & 16))
      actor->mShape = gabi::load<u32>(playback + 0x28);
  }
  actor->set_mtx();
  return TRUE;
}
VERIFY(0x0233765C, Execute);
void staticInitialize() {
  WWHD_FUNC(0x02337704, void);
  gabi::store<u32>(0x10469670, 0);
  gabi::store<u32>(0x10469668, 0);
  gabi::store<u32>(0x10469674, 0);
  gabi::store<u32>(0x1046966C, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8CD4));
  gabi::store<f32>(0x1046965C, -3.1415927410125732f);
  gabi::store<f32>(0x10469660, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10469664));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8CE0));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10469665));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8CEC));
}
VERIFY(0x02337704, staticInitialize);
void deleteStatic(void *object, u32 flags) {
  WWHD_FUNC(0x02337798, void, object, flags);
  if (object && (flags & 1))
    gabi::call(0x0273AF40, object);
}
VERIFY(0x02337798, deleteStatic);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x023377AC, BOOL, actor);
  return TRUE;
}
VERIFY(0x023377AC, IsDelete);
void destruct(Act_c *actor, u32 flags) {
  WWHD_FUNC(0x023377B4, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x023377B4, destruct);
void emptyVirtual() { WWHD_FUNC(0x02337808, void); }
VERIFY(0x02337808, emptyVirtual);
} // namespace daObjDoguuDemo

/* HD Cafelmp full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_cafelmp.h"
BOOL daObjCafelmp_c::CreateHeap() {
  WWHD_FUNC(0x0232F934, BOOL, this);
  gabi::Local<SafeString> name;
  name->__vtbl = 0x100269F4;
  name->mStringTop = 0x10026A1C;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 3);
  if (!data)
    gabi::call(0x0273AA24, STR(0x10026A24), 0x51, STR(0x10026A38));
  mModel = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203);
  return mModel != nullptr;
}
VERIFY(0x0232F934, &daObjCafelmp_c::CreateHeap);
BOOL solidHeapCB(daObjCafelmp_c *actor) {
  WWHD_FUNC(0x0232F9D0, BOOL, actor);
  return actor->CreateHeap();
}
VERIFY(0x0232F9D0, solidHeapCB);
void daObjCafelmp_c::set_mtx() {
  WWHD_FUNC(0x0232F9D4, void, this);
  f32 x = scale.x, y = scale.y;
  u32 model = gabi::ea((J3DModel *)mModel);
  f32 z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  x = current.pos.x;
  y = current.pos.y;
  z = current.pos.z;
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  gabi::call(0x028E93CC, matrix, x, y, z);
  s16 angle = gabi::load<s16>(gabi::ea(this) + 0x322);
  gabi::call(0x025F1C28, matrix, angle);
  f32 m00 = gabi::load<f32>(0x1048D0CC);
  f32 m01 = gabi::load<f32>(0x1048D0D0);
  f32 m02 = gabi::load<f32>(0x1048D0D4);
  f32 m03 = gabi::load<f32>(0x1048D0D8);
  f32 m10 = gabi::load<f32>(0x1048D0DC);
  f32 m11 = gabi::load<f32>(0x1048D0E0);
  f32 m12 = gabi::load<f32>(0x1048D0E4);
  f32 m13 = gabi::load<f32>(0x1048D0E8);
  f32 m20 = gabi::load<f32>(0x1048D0EC);
  f32 m21 = gabi::load<f32>(0x1048D0F0);
  f32 m22 = gabi::load<f32>(0x1048D0F4);
  f32 m23 = gabi::load<f32>(0x1048D0F8);
  model = gabi::ea((J3DModel *)mModel);
  gabi::store<f32>(model + 0xC8, m00);
  gabi::store<f32>(model + 0xCC, m01);
  gabi::store<f32>(model + 0xD0, m02);
  gabi::store<f32>(model + 0xD4, m03);
  gabi::store<f32>(model + 0xD8, m10);
  gabi::store<f32>(model + 0xDC, m11);
  gabi::store<f32>(model + 0xE0, m12);
  gabi::store<f32>(model + 0xE4, m13);
  gabi::store<f32>(model + 0xE8, m20);
  gabi::store<f32>(model + 0xEC, m21);
  gabi::store<f32>(model + 0xF0, m22);
  gabi::store<f32>(model + 0xF4, m23);
}
VERIFY(0x0232F9D4, &daObjCafelmp_c::set_mtx);
void daObjCafelmp_c::CreateInit() {
  WWHD_FUNC(0x0232FAAC, void, this);
  u32 model = gabi::ea((J3DModel *)mModel);
  f32 highY = gabi::load<f32>(0x10026A58), low = gabi::load<f32>(0x10026A4C),
      high = gabi::load<f32>(0x10026A50), lowY = gabi::load<f32>(0x10026A54);
  gabi::store<u32>(gabi::ea(this) + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, this, low, lowY, low, high, highY, high);
  gabi::store<f32>(gabi::ea(this) + 0x364, gabi::load<f32>(0x10026A5C));
  set_mtx();
}
VERIFY(0x0232FAAC, &daObjCafelmp_c::CreateInit);
s32 daObjCafelmp_c::Create() {
  WWHD_FUNC(0x0232FB2C, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      flags = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x10026A0C);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x100269E8));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, this, 0x0232F9D0, 0x680))
      return 5;
    CreateInit();
  }
  return phase;
}
VERIFY(0x0232FB2C, &daObjCafelmp_c::Create);
BOOL daObjCafelmp_c::Delete() {
  WWHD_FUNC(0x0232FBF0, BOOL, this);
  gabi::call(0x025204C8, &mPhase, STR(0x10026A60));
  return TRUE;
}
VERIFY(0x0232FBF0, &daObjCafelmp_c::Delete);
BOOL daObjCafelmp_c::Draw() {
  WWHD_FUNC(0x0232FC20, BOOL, this);
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(light), 1, &current.pos,
             gabi::at<u8>(gabi::ea(this) + 0x110));
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, gabi::at<u8>(light), (J3DModel *)mModel,
             gabi::at<u8>(gabi::ea(this) + 0x110));
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
  gabi::call(0x025E2DE0, (J3DModel *)mModel, 0);
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
  return TRUE;
}
VERIFY(0x0232FC20, &daObjCafelmp_c::Draw);
BOOL daObjCafelmp_c::Execute() {
  WWHD_FUNC(0x0232FCB8, BOOL, this);
  u16 angle = gabi::load<u16>(gabi::ea(this) + 0x322);
  gabi::store<u16>(gabi::ea(this) + 0x322, (u16)(angle + 0xDAu));
  set_mtx();
  return TRUE;
}
VERIFY(0x0232FCB8, &daObjCafelmp_c::Execute);
void staticInitialize() {
  WWHD_FUNC(0x0232FCE8, void);
  gabi::store<u32>(0x1046943C, 0);
  gabi::store<u32>(0x10469434, 0);
  gabi::store<u32>(0x10469440, 0);
  gabi::store<u32>(0x10469438, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C88EC));
  gabi::store<f32>(0x10469428, -3.1415927410125732f);
  gabi::store<f32>(0x1046942C, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10469430));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C88F8));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10469431));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8904));
}
VERIFY(0x0232FCE8, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x0232FD7C, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x0232FD7C, deleteStatic);
void destruct(daObjCafelmp_c *actor, s32 flags) {
  WWHD_FUNC(0x0232FD98, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x0232FD98, destruct);
void emptyVirtual() { WWHD_FUNC(0x0232FDEC, void); }
VERIFY(0x0232FDEC, emptyVirtual);
BOOL IsDelete(daObjCafelmp_c *actor) {
  WWHD_FUNC(0x0232FD90, BOOL, actor);
  return TRUE;
}
VERIFY(0x0232FD90, IsDelete);

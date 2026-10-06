/* HD Gaship2 full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_gaship2.h"
BOOL daObjGaship2::Act_c::CreateHeap() {
  WWHD_FUNC(0x0234AA30, BOOL, this);
  gabi::Local<SafeString> name;
  name->__vtbl = 0x10029630;
  name->mStringTop = 0x10029698;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 4);
  if (!data)
    gabi::call(0x0273AA24, STR(0x10029658), 0x5A, STR(0x1002966C));
  mModel = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11000002);
  set_mtx();
  gabi::Local<SafeString> bgName;
  bgName->__vtbl = 0x10029630;
  bgName->mStringTop = 0x10029698;
  void *bgData = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), bgName.get(), 7);
  if (!bgData)
    gabi::call(0x0273AA24, STR(0x10029658), 0x67, STR(0x1002967C));
  else {
    mBgW = gabi::call<dBgW *>(0x024F23F4, 0);
    if (mBgW && gabi::call<s32>(0x0200A030, (dBgW *)mBgW, bgData, 1, &mMatrix))
      return FALSE;
  }
  return data && mModel && bgData && mBgW;
}
VERIFY(0x0234AA30, &daObjGaship2::Act_c::CreateHeap);
BOOL solidHeapCB(daObjGaship2::Act_c *actor) {
  WWHD_FUNC(0x0234AB80, BOOL, actor);
  return actor->CreateHeap();
}
VERIFY(0x0234AB80, solidHeapCB);
void daObjGaship2::Act_c::set_mtx() {
  WWHD_FUNC(0x0234A93C, void, this);
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
  s16 xAngle = gabi::load<s16>(gabi::ea(this) + 0x328),
      yAngle = gabi::load<s16>(gabi::ea(this) + 0x32A),
      zAngle = gabi::load<s16>(gabi::ea(this) + 0x32C);
  gabi::call(0x025F1B48, matrix, xAngle, yAngle, zAngle);
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
  gabi::call(0x028E90D4, matrix, &mMatrix);
  gabi::call(0x027F4D5C, (J3DModel *)mModel);
}
VERIFY(0x0234A93C, &daObjGaship2::Act_c::set_mtx);
s32 daObjGaship2::Act_c::Create() {
  WWHD_FUNC(0x0234AB84, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      flags = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x10029648);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x10029698));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, this, 0x0234AB80, 0))
      return 5;
    u32 model = gabi::ea((J3DModel *)mModel);
    gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x024EEA6C, gabi::at<u8>(play + 0x12A0), (dBgW *)mBgW, this);
    gabi::store<u32>(gabi::ea((dBgW *)mBgW) + 0xA8, 0x024EE658);
  }
  return phase;
}
VERIFY(0x0234AB84, &daObjGaship2::Act_c::Create);
BOOL daObjGaship2::Act_c::Delete() {
  WWHD_FUNC(0x0234AC64, BOOL, this);
  if (gabi::load<u32>(gabi::ea(this) + 0xF4) && mBgW &&
      gabi::load<u32>(gabi::ea((dBgW *)mBgW)) < 256) {
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, gabi::at<u8>(play + 0x12A0), (dBgW *)mBgW);
  }
  gabi::call(0x025204C8, &mPhase, STR(0x10029698));
  return TRUE;
}
VERIFY(0x0234AC64, &daObjGaship2::Act_c::Delete);
BOOL daObjGaship2::Act_c::Draw() {
  WWHD_FUNC(0x0234AD0C, BOOL, this);
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(light), 1, &current.pos,
             gabi::at<u8>(gabi::ea(this) + 0x110));
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, gabi::at<u8>(light), (J3DModel *)mModel,
             gabi::at<u8>(gabi::ea(this) + 0x110));
  gabi::call(0x025E2DE0, (J3DModel *)mModel, 0);
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
  return TRUE;
}
VERIFY(0x0234AD0C, &daObjGaship2::Act_c::Draw);
BOOL daObjGaship2::Act_c::Execute() {
  WWHD_FUNC(0x0234ACD4, BOOL, this);
  set_mtx();
  gabi::call(0x024F43DC, (dBgW *)mBgW);
  return TRUE;
}
VERIFY(0x0234ACD4, &daObjGaship2::Act_c::Execute);
void staticInitialize() {
  WWHD_FUNC(0x0234ADB4, void);
  gabi::store<u32>(0x10469C50, 0);
  gabi::store<u32>(0x10469C48, 0);
  gabi::store<u32>(0x10469C54, 0);
  gabi::store<u32>(0x10469C4C, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C97E4));
  gabi::store<f32>(0x10469C3C, -3.1415927410125732f);
  gabi::store<f32>(0x10469C40, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10469C44));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C97F0));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10469C45));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C97FC));
}
VERIFY(0x0234ADB4, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x0234AE48, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x0234AE48, deleteStatic);
void destruct(daObjGaship2::Act_c *actor, s32 flags) {
  WWHD_FUNC(0x0234AE60, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x0234AE60, destruct);
void emptyVirtual() { WWHD_FUNC(0x0234AE5C, void); }
VERIFY(0x0234AE5C, emptyVirtual);
BOOL IsDelete(daObjGaship2::Act_c *actor) {
  WWHD_FUNC(0x0234AEB4, BOOL, actor);
  return TRUE;
}
VERIFY(0x0234AEB4, IsDelete);

s32 createWrapper(daObjGaship2::Act_c *p) {
  WWHD_FUNC(0x0234ADA4, s32, p);
  return p->Create();
}
VERIFY(0x0234ADA4, createWrapper);
BOOL deleteWrapper(daObjGaship2::Act_c *p) {
  WWHD_FUNC(0x0234ADA8, BOOL, p);
  return p->Delete();
}
VERIFY(0x0234ADA8, deleteWrapper);
BOOL executeWrapper(daObjGaship2::Act_c *p) {
  WWHD_FUNC(0x0234ADAC, BOOL, p);
  return p->Execute();
}
VERIFY(0x0234ADAC, executeWrapper);
BOOL drawWrapper(daObjGaship2::Act_c *p) {
  WWHD_FUNC(0x0234ADB0, BOOL, p);
  return p->Draw();
}
VERIFY(0x0234ADB0, drawWrapper);

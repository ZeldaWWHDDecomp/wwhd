/* HD Gaship full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_gaship.h"
BOOL daObjGaship::Act_c::CreateHeap() {
  WWHD_FUNC(0x0234A3E4, BOOL, this);
  gabi::Local<SafeString> name;
  name->__vtbl = 0x100295B8;
  name->mStringTop = 0x10029628;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 3);
  if (!data)
    gabi::call(0x0273AA24, STR(0x100295F0), 0x8C, STR(0x100295E0));
  if (data)
    mModel = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11000002);
  set_mtx();
  return data && mModel != nullptr;
}
VERIFY(0x0234A3E4, &daObjGaship::Act_c::CreateHeap);
BOOL solidHeapCB(daObjGaship::Act_c *actor) {
  WWHD_FUNC(0x0234A4B8, BOOL, actor);
  return actor->CreateHeap();
}
VERIFY(0x0234A4B8, solidHeapCB);
void daObjGaship::Act_c::set_mtx() {
  WWHD_FUNC(0x0234A2F0, void, this);
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
VERIFY(0x0234A2F0, &daObjGaship::Act_c::set_mtx);
s32 daObjGaship::Act_c::Create() {
  WWHD_FUNC(0x0234A4BC, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      flags = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x100295D0);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x10029628));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, this, 0x0234A4B8, 0))
      return 5;
    u32 model = gabi::ea((J3DModel *)mModel);
    gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  }
  return phase;
}
VERIFY(0x0234A4BC, &daObjGaship::Act_c::Create);
BOOL daObjGaship::Act_c::Delete() {
  WWHD_FUNC(0x0234A578, BOOL, this);
  gabi::call(0x025204C8, &mPhase, STR(0x10029628));
  return TRUE;
}
VERIFY(0x0234A578, &daObjGaship::Act_c::Delete);
BOOL daObjGaship::Act_c::Draw() {
  WWHD_FUNC(0x0234A7C8, BOOL, this);
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(light), 1, &current.pos,
             gabi::at<u8>(gabi::ea(this) + 0x110));
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, gabi::at<u8>(light), (J3DModel *)mModel,
             gabi::at<u8>(gabi::ea(this) + 0x110));
  gabi::call(0x025E2DE0, (J3DModel *)mModel, 0);
  return TRUE;
}
VERIFY(0x0234A7C8, &daObjGaship::Act_c::Draw);
BOOL daObjGaship::Act_c::Execute() {
  WWHD_FUNC(0x0234A790, BOOL, this);
  set_mtx();
  birth_flag();
  return TRUE;
}
VERIFY(0x0234A790, &daObjGaship::Act_c::Execute);
void staticInitialize() {
  WWHD_FUNC(0x0234A834, void);
  gabi::store<u32>(0x10469C20, 0);
  gabi::store<u32>(0x10469C18, 0);
  gabi::store<u32>(0x10469C24, 0);
  gabi::store<u32>(0x10469C1C, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C9770));
  gabi::store<f32>(0x10469BF4, -3.1415927410125732f);
  gabi::store<f32>(0x10469BF8, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10469BFC));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C977C));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10469BFD));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C9788));
}
VERIFY(0x0234A834, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x0234A8C8, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x0234A8C8, deleteStatic);
void destruct(daObjGaship::Act_c *actor, s32 flags) {
  WWHD_FUNC(0x0234A8E0, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x0234A8E0, destruct);
void emptyVirtual() { WWHD_FUNC(0x0234A8DC, void); }
VERIFY(0x0234A8DC, emptyVirtual);
BOOL IsDelete(daObjGaship::Act_c *actor) {
  WWHD_FUNC(0x0234A934, BOOL, actor);
  return TRUE;
}
VERIFY(0x0234A934, IsDelete);

void daObjGaship::Act_c::birth_flag() {
  WWHD_FUNC(0x0234A5A8, void, this);
  f32 a = gabi::load<f32>(0x10029604), b = gabi::load<f32>(0x10029608),
      c = gabi::load<f32>(0x1002960C), d = gabi::load<f32>(0x10029610),
      e = gabi::load<f32>(0x10029614), f = gabi::load<f32>(0x10029618);
  for (s32 i = 0; i < 2; ++i) {
    if (!birthFlag[i]) {
      u32 angleGuard = gabi::load<u32>(0x10469C38);
      if (!gabi::load<u32>(0x10469C34)) {
        gabi::store<f32>(0x10469C10, b);
        gabi::store<f32>(0x10469C00, f);
        gabi::store<f32>(0x10469C04, e);
        gabi::store<f32>(0x10469C14, a);
        gabi::store<f32>(0x10469C0C, c);
        gabi::store<f32>(0x10469C08, d);
        gabi::store<u32>(0x10469C34, 1);
      }
      if (!angleGuard) {
        gabi::store<u32>(0x10469C38, 1);
        gabi::call(0x0201A478, gabi::at<csXyz>(0x10469C28), 0x222, 0, 0xAAA);
        gabi::call(0x0201A478, gabi::at<csXyz>(0x10469C2E), -0x38E, 0, 0xCCC);
      }
      gabi::Local<cXyz> offset;
      gabi::call(0x028E8F64, &mMatrix, gabi::at<cXyz>(0x10469C00 + 12 * i),
                 offset.get());
      s8 room = gabi::load<s8>(gabi::ea(this) + 0x326);
      u32 angleBase = 0x10469C28 + 6 * i;
      u16 x = gabi::load<u16>(angleBase), y = gabi::load<u16>(angleBase + 2),
          z = gabi::load<u16>(angleBase + 4);
      gabi::Local<csXyz> angle;
      angle->x = x;
      angle->y = y;
      angle->z = z;
      s32 pid = gabi::call<s32>(0x025D5834, 0xAE, 1, offset.get(), room,
                                angle.get(), 0, -1, 0);
      if (pid != -1)
        birthFlag[i] = 1;
    }
  }
}
VERIFY(0x0234A5A8, &daObjGaship::Act_c::birth_flag);
s32 createWrapper(daObjGaship::Act_c *p) {
  WWHD_FUNC(0x0234A824, s32, p);
  return p->Create();
}
VERIFY(0x0234A824, createWrapper);
BOOL deleteWrapper(daObjGaship::Act_c *p) {
  WWHD_FUNC(0x0234A828, BOOL, p);
  return p->Delete();
}
VERIFY(0x0234A828, deleteWrapper);
BOOL executeWrapper(daObjGaship::Act_c *p) {
  WWHD_FUNC(0x0234A82C, BOOL, p);
  return p->Execute();
}
VERIFY(0x0234A82C, executeWrapper);
BOOL drawWrapper(daObjGaship::Act_c *p) {
  WWHD_FUNC(0x0234A830, BOOL, p);
  return p->Draw();
}
VERIFY(0x0234A830, drawWrapper);

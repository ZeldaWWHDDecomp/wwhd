/* HD Xfuta full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_xfuta.h"
namespace daObjXfuta {
BOOL Act_c::create_heap() {
  WWHD_FUNC(0x023BBF28, BOOL, this);
  gabi::Local<SafeString> name;
  name->__vtbl = 0x100338B8;
  name->mStringTop = 0x10033910;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 3);
  if (!data)
    gabi::call(0x0273AA24, STR(0x100338E0), 0x105, STR(0x100338F4));
  mModel = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11000002);
  return data && mModel != nullptr;
}
VERIFY(0x023BBF28, &Act_c::create_heap);
BOOL solidHeapCB(Act_c *actor) {
  WWHD_FUNC(0x023BBFD8, BOOL, actor);
  return actor->create_heap();
}
VERIFY(0x023BBFD8, solidHeapCB);
void Act_c::set_mtx() {
  WWHD_FUNC(0x023BBFDC, void, this);
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
  gabi::call(0x025F1B48, matrix, (s16)shape_angle.x, (s16)shape_angle.y,
             (s16)shape_angle.z);
  f32 values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = gabi::load<f32>(0x1048D0CC + 4 * i);
  model = gabi::ea((J3DModel *)mModel);
  for (u32 i = 0; i < 12; ++i)
    gabi::store<f32>(model + 0xC8 + 4 * i, values[i]);
  gabi::call(0x028E90D4, matrix, &mMatrix);
  gabi::call(0x027F4D5C, (J3DModel *)mModel);
}
VERIFY(0x023BBFDC, &Act_c::set_mtx);
s32 Act_c::Create() {
  WWHD_FUNC(0x023BC0D0, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      flags = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x100338D0);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x10033910));
  if (phase == 4) {
    if (gabi::call<s32>(0x025D63E8, this, 0x023BBFD8, 0)) {
      set_mtx();
      u32 model = gabi::ea((J3DModel *)mModel);
      gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
    } else
      phase = 5;
  }
  return phase;
}
VERIFY(0x023BC0D0, &Act_c::Create);
BOOL Act_c::Delete() {
  WWHD_FUNC(0x023BC194, BOOL, this);
  gabi::call(0x025204C8, &mPhase, STR(0x10033910));
  return TRUE;
}
VERIFY(0x023BC194, &Act_c::Delete);
BOOL Act_c::Execute() {
  WWHD_FUNC(0x023BC1C4, BOOL, this);
  f32 x = gabi::load<f32>(gabi::ea(this) + 0x2EC),
      y = gabi::load<f32>(gabi::ea(this) + 0x2F0),
      z = gabi::load<f32>(gabi::ea(this) + 0x2F4);
  current.pos.y = y;
  current.pos.z = z;
  current.pos.x = x;
  set_mtx();
  return TRUE;
}
VERIFY(0x023BC1C4, &Act_c::Execute);
BOOL Act_c::Draw() {
  WWHD_FUNC(0x023BC200, BOOL, this);
  if (gabi::load<s32>(0x101D6010) != 1) {
    f32 zero = gabi::load<f32>(0x10033904);
    gabi::Local<cXyz> pos;
    pos->x = zero;
    pos->y = zero;
    pos->z = zero;
    u32 light = gabi::call<u32>(0x02555D0C);
    s32 mode = gabi::load<s32>(0x101CDEB4);
    gabi::call(0x025626A4, gabi::at<u8>(light), mode, pos.get(),
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
  }
  return TRUE;
}
VERIFY(0x023BC200, &Act_c::Draw);
s32 wrapperCreate(Act_c *actor) {
  WWHD_FUNC(0x023BC2C0, s32, actor);
  return actor->Create();
}
VERIFY(0x023BC2C0, wrapperCreate);
BOOL wrapperDelete(Act_c *actor) {
  WWHD_FUNC(0x023BC2C4, BOOL, actor);
  return actor->Delete();
}
VERIFY(0x023BC2C4, wrapperDelete);
BOOL wrapperExecute(Act_c *actor) {
  WWHD_FUNC(0x023BC2C8, BOOL, actor);
  return actor->Execute();
}
VERIFY(0x023BC2C8, wrapperExecute);
BOOL wrapperDraw(Act_c *actor) {
  WWHD_FUNC(0x023BC2CC, BOOL, actor);
  return actor->Draw();
}
VERIFY(0x023BC2CC, wrapperDraw);
void staticInitialize() {
  WWHD_FUNC(0x023BC2D0, void);
  gabi::store<u32>(0x1046CA48, 0);
  gabi::store<u32>(0x1046CA40, 0);
  gabi::store<u32>(0x1046CA4C, 0);
  gabi::store<u32>(0x1046CA44, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101CDEE8));
  gabi::store<f32>(0x1046CA34, -3.1415927410125732f);
  gabi::store<f32>(0x1046CA38, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x1046CA3C));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CDEF4));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x1046CA3D));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CDF00));
}
VERIFY(0x023BC2D0, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x023BC364, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x023BC364, deleteStatic);
void destruct(Act_c *actor, s32 flags) {
  WWHD_FUNC(0x023BC37C, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x023BC37C, destruct);
void emptyVirtual() { WWHD_FUNC(0x023BC378, void); }
VERIFY(0x023BC378, emptyVirtual);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x023BC3D0, BOOL, actor);
  return TRUE;
}
VERIFY(0x023BC3D0, IsDelete);
} // namespace daObjXfuta

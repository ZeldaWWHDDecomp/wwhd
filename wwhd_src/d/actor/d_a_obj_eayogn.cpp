/* WWHD Eayogn. Ported from zeldaret/tww with audited HD layout/behavior.
 */
#include "d/actor/d_a_obj_eayogn.h"
namespace daObjEayogn {
static void *resource(s32 index) {
  gabi::Local<SafeString> name;
  name->mStringTop = 0x10027950;
  name->__vtbl = 0x100278DC;
  return gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), index);
}
BOOL Act_c::create_heap() {
  WWHD_FUNC(0x02339330, BOOL, this);
  void *data = resource(4);
  if (!data) {
    gabi::call(0x0273AA24, STR(0x10027914), 0x5C, STR(0x10027904));
    return FALSE;
  }
  mModel = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203);
  if (!mModel)
    return FALSE;
  data = resource(7);
  u32 model = gabi::ea((J3DModel *)mModel);
  mBgW = gabi::call<dBgW *>(0x024F2478, data, 1,
                            gabi::at<Mtx34>(model ? model + 0xC8 : 0));
  return mBgW != nullptr;
}
VERIFY(0x02339330, &Act_c::create_heap);
BOOL solidHeapCB(Act_c *actor) {
  WWHD_FUNC(0x02339410, BOOL, actor);
  return actor->create_heap();
}
VERIFY(0x02339410, solidHeapCB);
void Act_c::init_mtx() {
  WWHD_FUNC(0x02339414, void, this);
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
  gabi::call(0x025F1B48, matrix, (s16)shape_angle.x, (s16)shape_angle.y,
             (s16)shape_angle.z);
  f32 values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = gabi::load<f32>(0x1048D0CC + 4 * i);
  model = gabi::ea((J3DModel *)mModel);
  for (u32 i = 0; i < 12; ++i)
    gabi::store<f32>(model + 0xC8 + 4 * i, values[i]);
}
VERIFY(0x02339414, &Act_c::init_mtx);
s32 Act_c::Create() {
  WWHD_FUNC(0x023394F4, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(base + 0xB4, 0x100278F4);
      flags = gabi::load<u32>(base + 0x2E4);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  u32 save = gabi::load<u32>(0x101F84DC);
  if (!gabi::call<s32>(0x025B7D90, gabi::at<u8>(save + 0xD4), 1))
    return 5;
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x10027950));
  if (phase != 4)
    return phase;
  if (!gabi::call<s32>(0x025D63E8, this, 0x02339410, 0))
    return 5;
  u32 model = gabi::ea((J3DModel *)mModel);
  gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  init_mtx();
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::call(0x024EEA6C, gabi::at<u8>(play + 0x12A0), (dBgW *)mBgW, this);
  gabi::call(0x024F43DC, (dBgW *)mBgW);
  gabi::call(0x025D674C, this, -550.0f, -50.0f, -600.0f, 550.0f, 50.0f, 600.0f);
  return 4;
}
VERIFY(0x023394F4, &Act_c::Create);
BOOL Act_c::Delete() {
  WWHD_FUNC(0x0233962C, BOOL, this);
  if (gabi::load<u32>(gabi::ea(this) + 0xF4)) {
    u32 bg = gabi::ea((dBgW *)mBgW);
    if (bg && gabi::load<u32>(bg) < 256) {
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::call(0x020087EC, gabi::at<u8>(play + 0x12A0), (dBgW *)mBgW);
      mBgW = nullptr;
    }
  }
  gabi::call(0x025204C8, &mPhase, STR(0x10027950));
  return TRUE;
}
VERIFY(0x0233962C, &Act_c::Delete);
BOOL Act_c::Draw() {
  WWHD_FUNC(0x023396A4, BOOL, this);
  u32 base = gabi::ea(this), light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(light), 1, &current.pos,
             gabi::at<u8>(base + 0x110));
  light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, gabi::at<u8>(light), (J3DModel *)mModel,
             gabi::at<u8>(base + 0x110));
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
VERIFY(0x023396A4, &Act_c::Draw);
s32 wrapperCreate(Act_c *actor) {
  WWHD_FUNC(0x0233973C, s32, actor);
  return actor->Create();
}
VERIFY(0x0233973C, wrapperCreate);
BOOL wrapperDelete(Act_c *actor) {
  WWHD_FUNC(0x02339740, BOOL, actor);
  return actor->Delete();
}
VERIFY(0x02339740, wrapperDelete);
BOOL Execute(Act_c *actor) {
  WWHD_FUNC(0x02339744, BOOL, actor);
  return TRUE;
}
VERIFY(0x02339744, Execute);
BOOL wrapperDraw(Act_c *actor) {
  WWHD_FUNC(0x0233974C, BOOL, actor);
  return actor->Draw();
}
VERIFY(0x0233974C, wrapperDraw);
void staticInitialize() {
  WWHD_FUNC(0x02339750, void);
  gabi::store<u32>(0x104696D4, 0);
  gabi::store<u32>(0x104696CC, 0);
  gabi::store<u32>(0x104696D8, 0);
  gabi::store<u32>(0x104696D0, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8E50));
  gabi::store<f32>(0x104696C0, -3.1415927410125732f);
  gabi::store<f32>(0x104696C4, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x104696C8));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8E5C));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x104696C9));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8E68));
}
VERIFY(0x02339750, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x023397E4, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x023397E4, deleteStatic);
void destruct(Act_c *actor, s32 flags) {
  WWHD_FUNC(0x023397F8, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x023397F8, destruct);
void emptyVirtual() { WWHD_FUNC(0x0233984C, void); }
VERIFY(0x0233984C, emptyVirtual);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x02339850, BOOL, actor);
  return TRUE;
}
VERIFY(0x02339850, IsDelete);
} // namespace daObjEayogn

/* HD _Bscurtain full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_bscurtain.h"
BOOL daObj_Bscurtain_c::CreateHeap() {
  WWHD_FUNC(0x0232897C, BOOL, this);
  gabi::Local<SafeString> name;
  name->__vtbl = 0x10026484;
  name->mStringTop = 0x10026500;
  s32 index = 3;
  if (gabi::load<u8>(gabi::ea(this) + 0xB3) == 1 &&
      !gabi::call<s32>(0x025B8B94,
                       gabi::at<u8>(gabi::load<u32>(0x101F84DC) + 0x644),
                       0x1F08))
    index = 4;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), index);
  if (!data)
    gabi::call(0x0273AA24, STR(0x100264BC), 0xA9, STR(0x100264D4));
  mModel = gabi::call<J3DModel *>(0x025E38E0, data, 0, 0x11020203);
  return mModel != nullptr;
}
VERIFY(0x0232897C, &daObj_Bscurtain_c::CreateHeap);
BOOL solidHeapCB(daObj_Bscurtain_c *actor) {
  WWHD_FUNC(0x02328A74, BOOL, actor);
  return actor->CreateHeap();
}
VERIFY(0x02328A74, solidHeapCB);
void daObj_Bscurtain_c::set_mtx() {
  WWHD_FUNC(0x02328A78, void, this);
  if (!gabi::load<u32>(0x10469348)) {
    gabi::store<u32>(0x10469348, 1);
    f32 x = gabi::load<f32>(0x101FFBA8), y = gabi::load<f32>(0x101FFBAC),
        z = gabi::load<f32>(0x101FFBB0);
    gabi::store<f32>(0x1046933C, x);
    gabi::store<f32>(0x10469340, y);
    gabi::store<f32>(0x10469344, z);
  }
  f32 x = gabi::load<f32>(0x1046933C), y = gabi::load<f32>(0x10469340),
      z = gabi::load<f32>(0x10469344);
  gabi::call(0x028E93CC, gabi::at<Mtx34>(0x1048D0CC), x, y, z);
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
  u32 model = gabi::ea((J3DModel *)mModel);
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
VERIFY(0x02328A78, &daObj_Bscurtain_c::set_mtx);
s32 daObj_Bscurtain_c::CreateInit() {
  WWHD_FUNC(0x02328BE0, s32, this);
  set_mtx();
  u32 model = gabi::ea((J3DModel *)mModel);
  gabi::store<u32>(gabi::ea(this) + 0x348, model ? model + 0xC8 : 0);
  return 4;
}
VERIFY(0x02328BE0, &daObj_Bscurtain_c::CreateInit);
s32 daObj_Bscurtain_c::Create() {
  WWHD_FUNC(0x02328C24, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      flags = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x100264AC);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x10026500));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, this, 0x02328A74, 0x10000))
      return 5;
    return CreateInit();
  }
  return phase;
}
VERIFY(0x02328C24, &daObj_Bscurtain_c::Create);
BOOL daObj_Bscurtain_c::Delete() {
  WWHD_FUNC(0x02328CC4, BOOL, this);
  gabi::call(0x025204C8, &mPhase, STR(0x10026500));
  return TRUE;
}
VERIFY(0x02328CC4, &daObj_Bscurtain_c::Delete);
BOOL daObj_Bscurtain_c::Draw() {
  WWHD_FUNC(0x02328D18, BOOL, this);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
  u32 light = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, gabi::at<u8>(light), 1, &current.pos,
             gabi::at<u8>(gabi::ea(this) + 0x110));
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
VERIFY(0x02328D18, &daObj_Bscurtain_c::Draw);
BOOL daObj_Bscurtain_c::Execute() {
  WWHD_FUNC(0x02328CF4, BOOL, this);
  set_mtx();
  return FALSE;
}
VERIFY(0x02328CF4, &daObj_Bscurtain_c::Execute);
void staticInitialize() {
  WWHD_FUNC(0x02328E0C, void);
  gabi::store<u32>(0x10469334, 0);
  gabi::store<u32>(0x1046932C, 0);
  gabi::store<u32>(0x10469338, 0);
  gabi::store<u32>(0x10469330, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8160));
  gabi::store<f32>(0x10469310, -3.1415927410125732f);
  gabi::store<f32>(0x10469314, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10469318));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C816C));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10469319));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8178));
  gabi::call(0x02328DB0, gabi::at<u8>(0x1046931C));
}
VERIFY(0x02328E0C, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x02328EAC, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x02328EAC, deleteStatic);
void destruct(daObj_Bscurtain_c *actor, s32 flags) {
  WWHD_FUNC(0x02328EC8, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x02328EC8, destruct);
void emptyVirtual() { WWHD_FUNC(0x02328F1C, void); }
VERIFY(0x02328F1C, emptyVirtual);
BOOL IsDelete(daObj_Bscurtain_c *actor) {
  WWHD_FUNC(0x02328EC0, BOOL, actor);
  return TRUE;
}
VERIFY(0x02328EC0, IsDelete);

void *constructHIO(void *p) {
  WWHD_FUNC(0x02328DB0, void *, p);
  if (!p)
    p = gabi::call<void *>(0x0273AD10, 0x10);
  if (p) {
    auto *hio = gabi::at<BscurtainHIO>(gabi::ea(p));
    hio->entry = -1;
    hio->value = gabi::load<f32>(0x100264E8);
    hio->flags = 0;
    hio->vtable = 0x1002649C;
  }
  return p;
}
VERIFY(0x02328DB0, constructHIO);
void colorToFloat(void *out, void *color) {
  WWHD_FUNC(0x02328F20, void, out, color);
  u32 src = gabi::ea(color), dst = gabi::ea(out);
  f32 divisor = gabi::load<f32>(0x10026508);
  f32 r = (f32)gabi::load<u8>(src) / divisor;
  f32 g = (f32)gabi::load<u8>(src + 1) / divisor;
  f32 b = (f32)gabi::load<u8>(src + 2) / divisor;
  f32 a = (f32)gabi::load<u8>(src + 3) / divisor;
  gabi::store<f32>(dst, r);
  gabi::store<f32>(dst + 4, g);
  gabi::store<f32>(dst + 8, b);
  gabi::store<f32>(dst + 12, a);
}
VERIFY(0x02328F20, colorToFloat);

/* HD Adnno full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_adnno.h"
BOOL daObjAdnno_c::CreateHeap() {
  WWHD_FUNC(0x02312FB8, BOOL, this);
  gabi::Local<SafeString> name;
  name->__vtbl = 0x10024714;
  name->mStringTop = 0x1002473C;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 4);
  if (!data)
    gabi::call(0x0273AA24, STR(0x10024744), 0x5C, STR(0x10024758));
  for (s32 i = 0; i < 16; ++i) {
    mModel[i] = gabi::call<J3DModel *>(0x025E38E0, data, 0x80000, 0x37441422);
    if (!mModel[i])
      return FALSE;
  }
  return TRUE;
}
VERIFY(0x02312FB8, &daObjAdnno_c::CreateHeap);
BOOL solidHeapCB(daObjAdnno_c *actor) {
  WWHD_FUNC(0x023130A0, BOOL, actor);
  return actor->CreateHeap();
}
VERIFY(0x023130A0, solidHeapCB);
void daObjAdnno_c::set_mtx() {
  WWHD_FUNC(0x023130A4, void, this);
  f32 step = gabi::load<f32>(0x10024770), edge = gabi::load<f32>(0x1002476C),
      zero = gabi::load<f32>(0x10024774);
  for (s32 i = 0; i < 16; ++i) {
    f32 x = scale.x, y = scale.y;
    u32 model = gabi::ea((J3DModel *)mModel[i]);
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
    gabi::call(0x025F24E0, (f32)(i % 4) * step - edge,
               edge - (f32)(i / 4) * step, zero);
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
    model = gabi::ea((J3DModel *)mModel[i]);
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
}
VERIFY(0x023130A4, &daObjAdnno_c::set_mtx);
void daObjAdnno_c::CreateInit() {
  WWHD_FUNC(0x0231323C, void, this);
  u32 model = gabi::ea((J3DModel *)mModel[0]);
  f32 highY = gabi::load<f32>(0x1002478C), low = gabi::load<f32>(0x10024780),
      high = gabi::load<f32>(0x10024784), lowY = gabi::load<f32>(0x10024788);
  gabi::store<u32>(gabi::ea(this) + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, this, low, lowY, low, high, highY, high);
  gabi::store<f32>(gabi::ea(this) + 0x364, gabi::load<f32>(0x10024790));
  set_mtx();
}
VERIFY(0x0231323C, &daObjAdnno_c::CreateInit);
s32 daObjAdnno_c::Create() {
  WWHD_FUNC(0x023132BC, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      flags = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x1002472C);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x1002470C));
  if (phase == 4) {
    if (!gabi::call<s32>(0x025D63E8, this, 0x023130A0, 0x9C00))
      return 5;
    CreateInit();
  }
  return phase;
}
VERIFY(0x023132BC, &daObjAdnno_c::Create);
BOOL daObjAdnno_c::Delete() {
  WWHD_FUNC(0x02313384, BOOL, this);
  gabi::call(0x025204C8, &mPhase, STR(0x10024794));
  return TRUE;
}
VERIFY(0x02313384, &daObjAdnno_c::Delete);
BOOL daObjAdnno_c::Draw() {
  WWHD_FUNC(0x023133B4, BOOL, this);
  u32 play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D70));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D74));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4634, gabi::load<u32>(play + 0x5D78));
  play = gabi::call<u32>(0x025200D4);
  gabi::store<u32>(0x104B4638, gabi::load<u32>(play + 0x5D7C));
  return TRUE;
}
VERIFY(0x023133B4, &daObjAdnno_c::Draw);
BOOL daObjAdnno_c::Execute() {
  WWHD_FUNC(0x02313410, BOOL, this);
  set_mtx();
  return TRUE;
}
VERIFY(0x02313410, &daObjAdnno_c::Execute);
void staticInitialize() {
  WWHD_FUNC(0x02313434, void);
  gabi::store<u32>(0x10468F18, 0);
  gabi::store<u32>(0x10468F10, 0);
  gabi::store<u32>(0x10468F1C, 0);
  gabi::store<u32>(0x10468F14, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C790C));
  gabi::store<f32>(0x10468F04, -3.1415927410125732f);
  gabi::store<f32>(0x10468F08, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x10468F0C));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C7918));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x10468F0D));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C7924));
}
VERIFY(0x02313434, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x023134C8, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x023134C8, deleteStatic);
void destruct(daObjAdnno_c *actor, s32 flags) {
  WWHD_FUNC(0x023134E4, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x023134E4, destruct);
void emptyVirtual() { WWHD_FUNC(0x02313538, void); }
VERIFY(0x02313538, emptyVirtual);
BOOL IsDelete(daObjAdnno_c *actor) {
  WWHD_FUNC(0x023134DC, BOOL, actor);
  return TRUE;
}
VERIFY(0x023134DC, IsDelete);

void colorToFloat(void *out, void *color) {
  WWHD_FUNC(0x0231353C, void, out, color);
  u32 src = gabi::ea(color), dst = gabi::ea(out);
  f32 divisor = gabi::load<f32>(0x100247A8);
  f32 r = (f32)gabi::load<u8>(src) / divisor;
  f32 g = (f32)gabi::load<u8>(src + 1) / divisor;
  f32 b = (f32)gabi::load<u8>(src + 2) / divisor;
  f32 a = (f32)gabi::load<u8>(src + 3) / divisor;
  gabi::store<f32>(dst, r);
  gabi::store<f32>(dst + 4, g);
  gabi::store<f32>(dst + 8, b);
  gabi::store<f32>(dst + 12, a);
}
VERIFY(0x0231353C, colorToFloat);

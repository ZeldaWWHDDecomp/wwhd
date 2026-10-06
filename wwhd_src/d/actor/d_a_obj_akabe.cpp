/* WWHD Akabe full actor port derived from zeldaret/tww. */
#include "d/actor/d_a_obj_akabe.h"
namespace daObjAkabe {
u32 prmAbstract(Act_c *actor, u32 width, u32 shift) {
  WWHD_FUNC(0x02316A94, u32, actor, width, shift);
  u32 parameters = gabi::load<u32>(gabi::ea(actor) + 0xB0);
  width &= 63;
  shift &= 63;
  u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
  return (shift < 32 ? parameters >> shift : 0u) & mask;
}
VERIFY(0x02316A94, prmAbstract);

static u32 archive(s32 type) {
  return gabi::load<u32>(0x10024AE0 + (u32)type * 4u);
}
BOOL Act_c::chk_appear() {
  WWHD_FUNC(0x0231645C, BOOL, this);
  if (prmAbstract(this, 1, 12))
    return TRUE;
  u32 sw = prmAbstract(this, 8, 0), save = gabi::load<u32>(0x101F84DC);
  if (sw == 255)
    return gabi::load<u8>(save + 0xD4) == 0;
  s32 room = gabi::load<s8>(gabi::ea(this) + 0x2FE);
  return !gabi::call<s32>(0x025BA0C0, gabi::at<u8>(save + 0x20), sw, room);
}
VERIFY(0x0231645C, &Act_c::chk_appear);
void Act_c::init_scale() {
  WWHD_FUNC(0x0231650C, void, this);
  u32 mode = prmAbstract(this, 2, 8);
  if (mode == 1) {
    f32 x = scale.x, y = scale.y;
    x = x * 10.0f;
    scale.z = 1.0f;
    y = y * 10.0f;
    scale.x = x;
    scale.y = y;
  } else if (mode == 3)
    gabi::call(0x028E8E64, &scale, &scale, 10.0f);
  else if (mode != 2)
    scale.z = 1.0f;
}
VERIFY(0x0231650C, &Act_c::init_scale);
void Act_c::init_mtx() {
  WWHD_FUNC(0x023165BC, void, this);
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  f32 x = current.pos.x, z = current.pos.z, y = current.pos.y;
  gabi::call(0x028E93CC, matrix, x, y, z);
  gabi::call(0x025F1B48, matrix, (s16)shape_angle.x, (s16)shape_angle.y,
             (s16)shape_angle.z);
  y = scale.y;
  x = scale.x;
  z = scale.z;
  gabi::call(0x025F2518, x, y, z);
  gabi::call(0x028E90D4, matrix, &mMatrix);
}
VERIFY(0x023165BC, &Act_c::init_mtx);
BOOL Act_c::create_heap() {
  WWHD_FUNC(0x02316634, BOOL, this);
  BOOL success = FALSE;
  mBgW = gabi::call<dBgW *>(0x024F23F4, 0);
  if (mBgW) {
    s32 type = mType;
    u32 controller = gabi::load<u32>(0x101F4F28);
    u32 name = archive(type);
    s32 index = gabi::load<s16>(0x10024A8C + (u32)type * 2u);
    gabi::Local<SafeString> key;
    key->mStringTop = name;
    key->__vtbl = 0x10024A60;
    void *data = gabi::call<void *>(0x026066C4, gabi::at<u8>(controller),
                                    key.get(), index);
    if (!data)
      gabi::call(0x0273AA24, STR(0x10024A94), 0x82, STR(0x10024AA8));
    if (!gabi::call<s32>(0x0200A030, (dBgW *)mBgW, data, 1, &mMatrix))
      success = TRUE;
  }
  if (!success)
    mBgW = nullptr;
  return success;
}
VERIFY(0x02316634, &Act_c::create_heap);
BOOL solidHeapCB(Act_c *actor) {
  WWHD_FUNC(0x02316718, BOOL, actor);
  return actor->create_heap();
}
VERIFY(0x02316718, solidHeapCB);
s32 Act_c::Create() {
  WWHD_FUNC(0x0231671C, s32, this);
  u32 type = prmAbstract(this, 4, 16);
  if (type != 1 && type != 2 && type != 3)
    type = 0;
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  mType = (s32)type;
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      flags = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x10024A78);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  BOOL appear = chk_appear();
  mAppear = (u8)appear;
  s32 phase = 5;
  if (appear) {
    u32 name = archive(mType);
    phase = gabi::call<s32>(0x02520460, &mPhase, STR(name));
    if (phase == 4) {
      init_scale();
      init_mtx();
      u32 size = gabi::load<u32>(0x10024AC8 + (u32)(s32)mType * 4u);
      if (gabi::call<s32>(0x025D63E8, this, 0x02316718, size)) {
        u32 play = gabi::call<u32>(0x025200D4);
        gabi::call(0x024EEA6C, gabi::at<u8>(play + 0x12A0), (dBgW *)mBgW, this);
        u32 bg = gabi::ea((dBgW *)mBgW);
        f32 lowX = gabi::load<f32>(0x10024AB8);
        gabi::store<u32>(bg + 0xA8, 0);
        bg = gabi::ea((dBgW *)mBgW);
        f32 highZ = gabi::load<f32>(0x10024AC0);
        gabi::store<u8>(bg + 0x75, 1);
        f32 highY = gabi::load<f32>(0x10024AC4);
        s32 kind = mType;
        f32 lowY = gabi::load<f32>(0x10024ABC);
        gabi::store<u32>(base + 0x348, base + 0x3B8);
        if (kind == 3)
          gabi::call(0x025D674C, this, lowX, lowY, lowX, highZ, highY, highZ);
        else
          gabi::call(0x025D674C, this, lowX, lowY, lowY, highZ, highY,
                     gabi::load<f32>(0x10024A38));
      } else
        phase = 5;
    }
  }
  return phase;
}
VERIFY(0x0231671C, &Act_c::Create);
BOOL Act_c::Delete() {
  WWHD_FUNC(0x023168BC, BOOL, this);
  if (mAppear) {
    u32 bg = gabi::ea((dBgW *)mBgW);
    if (bg && gabi::load<u32>(bg) < 256) {
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::call(0x020087EC, gabi::at<u8>(play + 0x12A0), (dBgW *)mBgW);
    }
    u32 name = archive(mType);
    gabi::call(0x025204C8, &mPhase, STR(name));
  }
  return TRUE;
}
VERIFY(0x023168BC, &Act_c::Delete);
BOOL Act_c::Execute() {
  WWHD_FUNC(0x02316938, BOOL, this);
  if (!chk_appear())
    gabi::call(0x025D57E0, this);
  return TRUE;
}
VERIFY(0x02316938, &Act_c::Execute);
s32 wrapperCreate(Act_c *actor) {
  WWHD_FUNC(0x02316978, s32, actor);
  return actor->Create();
}
VERIFY(0x02316978, wrapperCreate);
BOOL Delete(Act_c *actor) {
  WWHD_FUNC(0x0231697C, BOOL, actor);
  return actor->Delete();
}
VERIFY(0x0231697C, Delete);
BOOL wrapperExecute(Act_c *actor) {
  WWHD_FUNC(0x02316980, BOOL, actor);
  return actor->Execute();
}
VERIFY(0x02316980, wrapperExecute);
void staticInitialize() {
  WWHD_FUNC(0x0231698C, void);
  gabi::store<u32>(0x104690B4, 0);
  gabi::store<u32>(0x104690AC, 0);
  gabi::store<u32>(0x104690B8, 0);
  gabi::store<u32>(0x104690B0, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C7B4C));
  gabi::store<f32>(0x104690A0, -3.1415927410125732f);
  gabi::store<f32>(0x104690A4, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x104690A8));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C7B58));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x104690A9));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C7B64));
}
VERIFY(0x0231698C, staticInitialize);
void destruct(Act_c *actor, s32 flags) {
  WWHD_FUNC(0x02316A38, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x02316A38, destruct);
BOOL Draw(Act_c *actor) {
  WWHD_FUNC(0x02316984, BOOL, actor);
  return TRUE;
}
VERIFY(0x02316984, Draw);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x02316A8C, BOOL, actor);
  return TRUE;
}
VERIFY(0x02316A8C, IsDelete);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x02316A20, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x02316A20, deleteStatic);
void emptyVirtual() { WWHD_FUNC(0x02316A34, void); }
VERIFY(0x02316A34, emptyVirtual);
} // namespace daObjAkabe

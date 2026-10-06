/* Bomb-shop counter flap. GC reconstruction corrected against WWHD.
 */
#include "d/actor/d_a_obj_pbco.h"
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
static bool counterClosed() {
  u32 save = gabi::load<u32>(0x101F84DC);
  return gabi::call<s32>(0x025B8B94, ptr(save + 0x644), 0xA02) &&
         !gabi::call<s32>(0x0254DA50, 0x69, 1);
}
static u8 *resource(s32 index) {
  gabi::Local<be<u32>[2]> name;
  (*name)[0] = 0x1002E5C8;
  (*name)[1] = 0x1002E554;
  u32 control = gabi::load<u32>(0x101F4F28);
  return gabi::call<u8 *>(0x026066C4, ptr(control), name.get(), index);
}
BOOL daObj_Pbco_c::CreateHeap() {
  WWHD_FUNC(0x02381EF4, BOOL, this);
  u8 *data = resource(counterClosed() ? 4 : 5);
  if (!data)
    gabi::call(0x0273AA24, ptr(0x1002E58C), 0xA9, ptr(0x1002E5A0));
  mpModel = gabi::call<u8 *>(0x025E38E0, data, 0, 0x11020203);
  if (counterClosed())
    mpBgW = nullptr;
  else {
    data = resource(8);
    u32 model = gabi::ea((u8 *)mpModel);
    mpBgW =
        gabi::call<u8 *>(0x024F2478, data, 1, ptr(model ? model + 0xC8 : 0));
    if (mpBgW == nullptr)
      return 0;
  }
  return mpModel != nullptr;
}
VERIFY(0x02381EF4, &daObj_Pbco_c::CreateHeap);
BOOL pbcoHeap(daObj_Pbco_c *self) {
  WWHD_FUNC(0x02382078, BOOL, self);
  return self->CreateHeap();
}
VERIFY(0x02382078, pbcoHeap);
void daObj_Pbco_c::set_mtx() {
  WWHD_FUNC(0x0238207C, void, this);
  f32 x = scale.x, y = scale.y;
  u32 model = gabi::ea((u8 *)mpModel);
  f32 z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)current.pos.x,
             (f32)current.pos.y, (f32)current.pos.z);
  u32 a = gabi::ea(this);
  gabi::call(0x025F1B48, ptr(0x1048D0CC), gabi::load<s16>(a + 0x328),
             gabi::load<s16>(a + 0x32A), gabi::load<s16>(a + 0x32C));
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  model = gabi::ea((u8 *)mpModel);
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
}
VERIFY(0x0238207C, &daObj_Pbco_c::set_mtx);
s32 daObj_Pbco_c::CreateInit() {
  WWHD_FUNC(0x0238215C, s32, this);
  set_mtx();
  u32 model = gabi::ea((u8 *)mpModel);
  u8 *bg = mpBgW;
  gabi::store<u32>(gabi::ea(this) + 0x348, model ? model + 0xC8 : 0);
  if (bg) {
    u32 p = gabi::call<u32>(0x025200D4);
    bg = mpBgW;
    gabi::call(0x024EEA6C, ptr(p + 0x12A0), bg, this);
  }
  return 4;
}
VERIFY(0x0238215C, &daObj_Pbco_c::CreateInit);
s32 pbcoCreate(daObj_Pbco_c *self) {
  WWHD_FUNC(0x023821C0, s32, self);
  u32 a = gabi::ea(self), flags = gabi::load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (self) {
      gabi::call(0x025D4ED0, self);
      flags = gabi::load<u32>(a + 0x2E4);
      gabi::store<u32>(a + 0xB4, 0x1002E57C);
    }
    gabi::store<u32>(a + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, self->mPhase, ptr(0x1002E5C8));
  if (phase == 4) {
    if (gabi::call<s32>(0x025D63E8, self, ptr(0x02382078), 0x10000))
      return self->CreateInit();
    return 5;
  }
  return phase;
}
VERIFY(0x023821C0, pbcoCreate);
BOOL pbcoDelete(daObj_Pbco_c *self) {
  WWHD_FUNC(0x02382260, BOOL, self);
  if (self->mpBgW != nullptr) {
    u32 p = gabi::call<u32>(0x025200D4);
    gabi::call(0x020087EC, ptr(p + 0x12A0), (u8 *)self->mpBgW);
  }
  gabi::call(0x025204C8, self->mPhase, ptr(0x1002E5C8));
  return 1;
}
VERIFY(0x02382260, pbcoDelete);
BOOL pbcoExecute(daObj_Pbco_c *self) {
  WWHD_FUNC(0x023822B8, BOOL, self);
  u8 *bg = self->mpBgW;
  if (bg)
    gabi::call(0x024F43DC, bg);
  self->set_mtx();
  return 0;
}
VERIFY(0x023822B8, pbcoExecute);
BOOL pbcoDraw(daObj_Pbco_c *self) {
  WWHD_FUNC(0x023822FC, BOOL, self);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 1, &self->current.pos,
             ptr(gabi::ea(self) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (u8 *)self->mpModel,
             ptr(gabi::ea(self) + 0x110));
  gabi::call(0x025E2DE0, (u8 *)self->mpModel, 0);
  return 1;
}
VERIFY(0x023822FC, pbcoDraw);
static daObj_Pbco_HIO_c *pbcoHioConstruct(daObj_Pbco_HIO_c *self) {
  WWHD_FUNC(0x02382358, daObj_Pbco_HIO_c *, self);
  if (!self)
    self = gabi::call<daObj_Pbco_HIO_c *>(0x0273AD10, 8);
  if (self) {
    self->mNo = -1;
    self->mVtable = 0x1002E56C;
  }
  return self;
}
VERIFY(0x02382358, pbcoHioConstruct);
static void pbcoStaticInit() {
  WWHD_FUNC(0x023823A0, void);
  for (u32 i = 0; i < 4; i++)
    gabi::store<u32>(0x1046BBD0 + i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CC2E0));
  gabi::store<f32>(0x1046BBC4, gabi::load<f32>(0x1002E5BC));
  gabi::store<f32>(0x1046BBC8, gabi::load<f32>(0x1002E5C0));
  gabi::call(0x028ED6F8, ptr(0x1046BBCC));
  gabi::call(0x028F026C, ptr(0x101CC2EC));
  gabi::call(0x028EAB2C, ptr(0x1046BBCD));
  gabi::call(0x028F026C, ptr(0x101CC2F8));
  pbcoHioConstruct(gabi::at<daObj_Pbco_HIO_c>(0x1046BBBC));
}
VERIFY(0x023823A0, pbcoStaticInit);
static void pbcoHioDelete(u8 *self, s32 flags) {
  WWHD_FUNC(0x02382440, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x02382440, pbcoHioDelete);
BOOL pbcoIsDelete(daObj_Pbco_c *self) {
  WWHD_FUNC(0x02382454, BOOL, self);
  return 1;
}
VERIFY(0x02382454, pbcoIsDelete);
static void pbcoDestructor(daObj_Pbco_c *self, s32 flags) {
  WWHD_FUNC(0x0238245C, void, self, flags);
  if (self) {
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x0238245C, pbcoDestructor);
static void pbcoEmpty() { WWHD_FUNC(0x023824B0, void); }
VERIFY(0x023824B0, pbcoEmpty);

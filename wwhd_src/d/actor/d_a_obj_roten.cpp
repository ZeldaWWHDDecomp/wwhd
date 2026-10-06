/* Zunari's outdoor shop models. GameCube source corrected against WWHD
 * disassembly. */
#include "d/actor/d_a_obj_roten.h"
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
BOOL daObj_Roten_c::CreateHeap() {
  WWHD_FUNC(0x02388908, BOOL, this);
  u32 idx = mType;
  u32 modelIndex = gabi::load<u32>(0x101CC94C + idx * 4);
  u32 controller = gabi::load<u32>(0x101F4F28);
  gabi::Local<be<u32>[2]> name;
  (*name)[0] = 0x1002EE60;
  (*name)[1] = 0x1002EDE4;
  u8 *data =
      gabi::call<u8 *>(0x026066C4, ptr(controller), name.get(), modelIndex);
  if (!data)
    gabi::call(0x0273AA24, ptr(0x1002EE20), 0xB0, ptr(0x1002EE34));
  mpModel = gabi::call<u8 *>(0x025E38E0, data, 0, 0x11020203);
  u8 *bg = gabi::call<u8 *>(0x024F23F4, (u8 *)nullptr);
  mpBgW = bg;
  if (bg) {
    idx = mType;
    controller = gabi::load<u32>(0x101F4F28);
    u32 collisionIndex = gabi::load<u32>(0x101CC958 + idx * 4);
    gabi::Local<be<u32>[2]> bgName;
    (*bgName)[0] = 0x1002EE60;
    (*bgName)[1] = 0x1002EDE4;
    u8 *bgData = gabi::call<u8 *>(0x026066C4, ptr(controller), bgName.get(),
                                  collisionIndex);
    u32 matrix = gabi::ea((u8 *)mpModel);
    bg = mpBgW;
    if (matrix)
      matrix += 0xC8;
    gabi::call(0x0200A030, bg, bgData, 1, ptr(matrix));
  }
  return mpModel != nullptr && mpBgW != nullptr;
}
VERIFY(0x02388908, &daObj_Roten_c::CreateHeap);
BOOL CheckCreateHeap(daObj_Roten_c *self) {
  WWHD_FUNC(0x02388A18, BOOL, self);
  return self->CreateHeap();
}
VERIFY(0x02388A18, CheckCreateHeap);
void daObj_Roten_c::set_mtx() {
  WWHD_FUNC(0x02388A1C, void, this);
  f32 x = scale.x, y = scale.y;
  u32 model = gabi::ea((u8 *)mpModel);
  f32 z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  x = current.pos.x;
  y = current.pos.y;
  z = current.pos.z;
  gabi::call(0x028E93CC, ptr(0x1048D0CC), x, y, z);
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  model = gabi::ea((u8 *)mpModel);
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
}
VERIFY(0x02388A1C, &daObj_Roten_c::set_mtx);
s32 daObj_Roten_c::CreateInit() {
  WWHD_FUNC(0x02388AE8, s32, this);
  set_mtx();
  u32 model = gabi::ea((u8 *)mpModel), bg = gabi::ea((u8 *)mpBgW);
  gabi::store<u32>(gabi::ea(this) + 0x348, model ? model + 0xC8 : 0);
  gabi::store<u32>(bg + 0xA8, 0x024EE658);
  u32 play = gabi::call<u32>(0x025200D4);
  u8 *w = mpBgW;
  gabi::call(0x024EEA6C, ptr(play + 0x12A0), w, this);
  return 4;
}
VERIFY(0x02388AE8, &daObj_Roten_c::CreateInit);
s32 daObj_RotenCreate(daObj_Roten_c *self) {
  WWHD_FUNC(0x02388B50, s32, self);
  u32 a = gabi::ea(self), flags = gabi::load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (self) {
      gabi::call(0x025D4ED0, self);
      flags = gabi::load<u32>(a + 0x2E4);
      gabi::store<u32>(a + 0xB4, 0x1002EE0C);
    }
    gabi::store<u32>(a + 0x2E4, flags | 8);
  }
  u32 type = gabi::load<u32>(a + 0xB0) >> 24;
  if (type > 2)
    type = 2;
  self->mType = type;
  s32 count = 0;
  for (u32 i = 0; i < 12; i++) {
    u32 save = gabi::load<u32>(0x101F84DC);
    if (gabi::call<s32>(0x025B7840, ptr(save + 0xB0), i) || i == 0)
      count++;
  }
  s32 needed = type == 0 ? 3 : type == 1 ? 6 : 9;
  if (count <= needed) {
    self->mRejected = 1;
    return 5;
  }
  self->mRejected = 0;
  s32 phase = gabi::call<s32>(0x02520460, self->mPhs, ptr(0x1002EE60));
  if (phase != 4)
    return phase;
  // Reload after resource loading: HD stores these heap limits in its scratch
  // frame.
  const u32 sizes[3] = {0x2BA0, 0x2A60, 0x2C60};
  u32 slot = self->mType;
  if (!gabi::call<s32>(0x025D63E8, self, 0x02388A18, sizes[slot]))
    return 5;
  return self->CreateInit();
}
VERIFY(0x02388B50, daObj_RotenCreate);
BOOL daObj_RotenDelete(daObj_Roten_c *self) {
  WWHD_FUNC(0x02388CC4, BOOL, self);
  if (!self->mRejected) {
    u32 play = gabi::call<u32>(0x025200D4);
    u8 *w = self->mpBgW;
    gabi::call(0x020087EC, ptr(play + 0x12A0), w);
    gabi::call(0x025204C8, self->mPhs, ptr(0x1002EE60));
  }
  return TRUE;
}
VERIFY(0x02388CC4, daObj_RotenDelete);
BOOL daObj_RotenExecute(daObj_Roten_c *self) {
  WWHD_FUNC(0x02388D1C, BOOL, self);
  self->set_mtx();
  gabi::call(0x024F43DC, (u8 *)self->mpBgW);
  return FALSE;
}
VERIFY(0x02388D1C, daObj_RotenExecute);
BOOL daObj_RotenDraw(daObj_Roten_c *self) {
  WWHD_FUNC(0x02388D54, BOOL, self);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 1, &self->current.pos, &self->tevStr);
  env = gabi::call<u32>(0x02555D0C);
  u8 *model = self->mpModel;
  gabi::call(0x02562F5C, ptr(env), model, &self->tevStr);
  model = self->mpModel;
  gabi::call(0x025E2DE0, model, 0);
  return TRUE;
}
VERIFY(0x02388D54, daObj_RotenDraw);
daObj_Roten_HIO_HD *hioConstructor(daObj_Roten_HIO_HD *self) {
  WWHD_FUNC(0x02388DB0, daObj_Roten_HIO_HD *, self);
  if (!self)
    self = gabi::call<daObj_Roten_HIO_HD *>(0x0273AD10, 0x10);
  if (self) {
    self->mNo = -1;
    self->mOffset = 0.0f;
    self->mFlags = 0;
    self->mVtable = 0x1002EDFC;
  }
  return self;
}
VERIFY(0x02388DB0, hioConstructor);
void initStatics() {
  WWHD_FUNC(0x02388E0C, void);
  for (u32 i = 0; i < 16; i += 4)
    gabi::store<u32>(0x1046BD94 + i, 0);
  gabi::call(0x028F026C, ptr(0x101CC964));
  gabi::store<f32>(0x1046BD78, -3.1415927410125732f);
  gabi::store<f32>(0x1046BD7C, 3.1415927410125732f);
  gabi::call(0x028ED6F8, ptr(0x1046BD80));
  gabi::call(0x028F026C, ptr(0x101CC970));
  gabi::call(0x028EAB2C, ptr(0x1046BD81));
  gabi::call(0x028F026C, ptr(0x101CC97C));
  hioConstructor(gabi::at<daObj_Roten_HIO_HD>(0x1046BD84));
}
VERIFY(0x02388E0C, initStatics);
void hioDestructor(daObj_Roten_HIO_HD *self, s32 flags) {
  WWHD_FUNC(0x02388EAC, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x02388EAC, hioDestructor);
BOOL daObj_RotenIsDelete(daObj_Roten_c *self) {
  WWHD_FUNC(0x02388EC0, BOOL, self);
  return TRUE;
}
VERIFY(0x02388EC0, daObj_RotenIsDelete);
void actorDestructor(daObj_Roten_c *self, s32 flags) {
  WWHD_FUNC(0x02388EC8, void, self, flags);
  if (self) {
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x02388EC8, actorDestructor);
void emptyVirtual(daObj_Roten_c *self) { WWHD_FUNC(0x02388F1C, void, self); }
VERIFY(0x02388F1C, emptyVirtual);

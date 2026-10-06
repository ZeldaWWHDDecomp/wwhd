// WWHD treasure chest. Verified-source decompilation.
#include "d/actor/d_a_tbox.h"
#include "bindings.h"
static u32 addr(const void *p) { return gabi::ea(p); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static u32 save() { return gabi::load<u32>(0x101F84DC) + 0x20; }
static void setAction(daTbox_c *self, u32 target) {
  self->mActionAdjustment = 0;
  self->mActionIndex = -1;
  self->mActionFunc = target;
}
static s32 getShapeType(daTbox_c *self) {
  WWHD_FUNC(0x024B4314, s32, self);
  s32 n = (u32)self->mParameters >> 20 & 15;
  return n < 4 ? n : 0;
}
VERIFY(0x024B4314, getShapeType);
static u32 getModelInfo(daTbox_c *self) {
  WWHD_FUNC(0x024B432C, u32, self);
  return 0x101D2320 + getShapeType(self) * 12;
}
VERIFY(0x024B432C, getModelInfo);
static s32 getFuncType(daTbox_c *self) {
  WWHD_FUNC(0x024B435C, s32, self);
  return (u32)self->mParameters & 127;
}
VERIFY(0x024B435C, getFuncType);
static BOOL checkEnv(daTbox_c *self) {
  WWHD_FUNC(0x024B481C, BOOL, self);
  s32 n = getShapeType(self);
  return gabi::load<s16>(0x101D2326 + n * 12) > 0;
}
VERIFY(0x024B481C, checkEnv);
static BOOL checkOpen(daTbox_c *self) {
  WWHD_FUNC(0x024B4AB4, BOOL, self);
  s32 type = getFuncType(self);
  u32 number = (u32)self->mParameters >> 7 & 31;
  if (type == 7 || type == 8)
    return gabi::call<BOOL>(0x02520864, 1, number);
  return gabi::call<BOOL>(0x025B8C74, gabi::load<u32>(0x101F84DC) + 0x798,
                          number);
}
VERIFY(0x024B4AB4, checkOpen);
static BOOL checkNormal(daTbox_c *self) {
  WWHD_FUNC(0x024B59CC, BOOL, self);
  s32 type = getFuncType(self);
  if (type == 0 || type == 3 || type == 5 || type == 7)
    return TRUE;
  s32 room = self->mRoomNo;
  if (room == -1 || room == 63)
    return FALSE;
  s32 sw = (u32)self->mParameters >> 12 & 255;
  if (sw >= 192)
    return FALSE;
  return gabi::call<BOOL>(0x025BA0C0, save(), sw, room) != 0;
}
VERIFY(0x024B59CC, checkNormal);
static void lightReady(daTbox_c *self) {
  WWHD_FUNC(0x024B5A64, void, self);
  f32 x = self->current.pos.x, z = self->current.pos.z, y = self->current.pos.y;
  u32 a = addr(self);
  gabi::store<f32>(a + 0x714, x);
  gabi::store<f32>(a + 0x71C, z);
  gabi::store<u16>(a + 0x748, 100);
  self->mAllColRatio = 0;
  gabi::store<f32>(a + 0x718, y + 55);
  gabi::store<f32>(a + 0x72C, 0);
  gabi::store<u16>(a + 0x746, 255);
  gabi::store<u16>(a + 0x722, 255);
  gabi::store<f32>(a + 0x73C, y + 50);
  gabi::store<u16>(a + 0x744, 255);
  gabi::store<u16>(a + 0x724, 255);
  gabi::store<u16>(a + 0x720, 255);
  gabi::store<f32>(a + 0x728, 0);
  gabi::store<f32>(a + 0x740, z);
  gabi::store<f32>(a + 0x74C, 0);
  gabi::store<f32>(a + 0x750, 0);
  gabi::store<f32>(a + 0x738, x);
}
VERIFY(0x024B5A64, lightReady);
static void setCollision(daTbox_c *self) {
  WWHD_FUNC(0x024B5AE0, void, self);
  u32 a = addr(self);
  gabi::call(0x020182E0, a + 0xAEC, &self->current.pos);
  gabi::call(0x020184DC, a + 0xAEC, 40.0f);
  gabi::call(0x02018428, a + 0xAEC, 110.0f);
  u32 p = play();
  gabi::call(0x0200E240, p + 0x26A4, a + 0x9D4);
}
VERIFY(0x024B5AE0, setCollision);
static void clrDzb(daTbox_c *self) {
  WWHD_FUNC(0x024B5B50, void, self);
  if ((void *)self->mpBgWCurrent) {
    u32 p = play();
    gabi::call(0x020087EC, p + 0x12A0, (void *)self->mpBgWCurrent);
    self->mpBgWCurrent = nullptr;
    u32 a = addr(self) + 0xA00;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~1U);
  }
}
VERIFY(0x024B5B50, clrDzb);
static void setDzb(daTbox_c *self) {
  WWHD_FUNC(0x024B5BA8, void, self);
  clrDzb(self);
  if (checkOpen(self))
    self->mpBgWCurrent = (void *)self->mpBgWOpen;
  else if (getFuncType(self) == 3 &&
           !gabi::call<BOOL>(0x025BA0C0, save(),
                             (u32)self->mParameters >> 12 & 255,
                             (s32)self->mRoomNo))
    self->mpBgWCurrent = (void *)self->mpBgWVines;
  else
    self->mpBgWCurrent = (void *)self->mpBgWClosed;
  u32 p = play();
  s32 result =
      gabi::call<s32>(0x024EEA6C, p + 0x12A0, (void *)self->mpBgWCurrent, self);
  if (result)
    gabi::call(0x0273AA24, 0x1003FF74, 0x249, 0x1003FF70);
  u32 w = addr((void *)self->mpBgWCurrent);
  gabi::store<u16>(w + 0xB8, gabi::load<u16>(addr(self) + 0x3AE));
  u32 a = addr(self) + 0xA00;
  gabi::store<u32>(a, gabi::load<u32>(a) | 1);
}
VERIFY(0x024B5BA8, setDzb);
static void searchRoomNo(daTbox_c *self) {
  WWHD_FUNC(0x024B5CBC, void, self);
  s32 room = self->mRoomNo;
  u16 flags = self->mFlags;
  if (room == -1) {
    room = (s16)self->home.angle.x & 63;
    self->mRoomNo = room;
  }
  if (flags & 2)
    clrDzb(self);
  else if (room != -1 && !(void *)self->mpBgWCurrent)
    setDzb(self);
}
VERIFY(0x024B5CBC, searchRoomNo);
static BOOL actionSwOnWait2(daTbox_c *self) {
  WWHD_FUNC(0x024B6334, BOOL, self);
  if (gabi::call<BOOL>(0x025BA0C0, save(), (u32)self->mParameters >> 12 & 255,
                       (s32)self->mRoomNo)) {
    setAction(self, 0x024B7394);
    setDzb(self);
  }
  return TRUE;
}
VERIFY(0x024B6334, actionSwOnWait2);
static void lightUpProc(daTbox_c *self) {
  WWHD_FUNC(0x024B6D0C, void, self);
  u32 a = addr(self);
  f32 p = gabi::load<f32>(a + 0x728);
  if (p < 130)
    gabi::store<f32>(a + 0x728, p + 13);
  f32 e = gabi::load<f32>(a + 0x74C);
  if (e < 120)
    gabi::store<f32>(a + 0x74C, e + 12);
}
VERIFY(0x024B6D0C, lightUpProc);
static void lightDownProc(daTbox_c *self) {
  WWHD_FUNC(0x024B6D58, void, self);
  u32 a = addr(self);
  f32 p = gabi::load<f32>(a + 0x728);
  if (p > 5.2f) {
    f32 e = gabi::load<f32>(a + 0x74C);
    gabi::store<f32>(a + 0x728, p - 5.2f);
    gabi::store<f32>(a + 0x74C, e > 4.8f ? e - 4.8f : 0);
  } else {
    f32 e = gabi::load<f32>(a + 0x74C);
    gabi::store<f32>(a + 0x728, 0);
    gabi::store<f32>(a + 0x74C, e > 4.8f ? e - 4.8f : 0);
  }
}
VERIFY(0x024B6D58, lightDownProc);
static void darkProc(daTbox_c *self) {
  WWHD_FUNC(0x024B6DBC, void, self);
  u16 time = self->mOpenTimer;
  if (time > 150)
    self->mAllColRatio = 1;
  else if (time > 120)
    self->mAllColRatio = gabi::fmadds((f32)(time - 120) / 30.0f, 0.6f, 0.4f);
}
VERIFY(0x024B6DBC, darkProc);
static void volmProc(daTbox_c *self) {
  WWHD_FUNC(0x024B6E38, void, self);
  u16 time = self->mOpenTimer;
  if (time == 36)
    gabi::store<u8>(addr((void *)self->mSmokeEmitter) + 0x247, 255);
  else if (time >= 181) {
    gabi::call(0x0255A374, addr(self) + 0x714);
    gabi::call(0x0255BA9C, addr(self) + 0x738);
    u32 e = addr((void *)self->mSmokeEmitter);
    gabi::store<u8>(e + 0x247, 0);
    e = addr((void *)self->mSmokeEmitter);
    u32 flags = gabi::load<u32>(e + 0x254);
    gabi::store<u32>(e + 0x5C, 0xFFFFFFFF);
    gabi::store<u32>(e + 0x254, flags | 1);
    self->mSmokeEmitter = nullptr;
  } else if (time > 156)
    gabi::store<u8>(addr((void *)self->mSmokeEmitter) + 0x247,
                    1810 - time * 10);
}
VERIFY(0x024B6E38, volmProc);
static u32 daTbox_HIO_ct(u32 self) {
  WWHD_FUNC(0x024B7888, u32, self);
  if (!self) {
    self = gabi::call<u32>(0x0273AD10, 16);
    if (!self)
      return 0;
  }
  gabi::store<u16>(self + 4, 180);
  gabi::store<u8>(self, 255);
  gabi::store<u32>(self + 12, 0x1003FE54);
  gabi::store<u16>(self + 8, 30);
  gabi::store<u16>(self + 2, 130);
  gabi::store<u16>(self + 6, 48);
  return self;
}
VERIFY(0x024B7888, daTbox_HIO_ct);
static void daTbox_HIO_dt(u32 self, u32 flags) {
  WWHD_FUNC(0x024B7990, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x024B7990, daTbox_HIO_dt);
static BOOL daTbox_IsDelete(daTbox_c *self) {
  WWHD_FUNC(0x024B79A4, BOOL, self);
  return TRUE;
}
VERIFY(0x024B79A4, daTbox_IsDelete);
static BOOL actionWait(daTbox_c *self) {
  WWHD_FUNC(0x024B79AC, BOOL, self);
  return TRUE;
}
VERIFY(0x024B79AC, actionWait);
static void emptyVirtual(u32 self) { WWHD_FUNC(0x024B7A68, void, self); }
VERIFY(0x024B7A68, emptyVirtual);
static BOOL CreateHeap(daTbox_c *self) {
  WWHD_FUNC(0x024B4E90, BOOL, self);
  if (gabi::call<s32>(0x024B4368, self) != 4)
    return FALSE;
  if (checkEnv(self) && gabi::call<s32>(0x024B4858, self) != 4)
    return FALSE;
  if (!checkOpen(self) && gabi::call<s32>(0x024B4B20, self) != 4)
    return FALSE;
  return gabi::call<s32>(0x024B4CDC, self) == 4;
}
VERIFY(0x024B4E90, CreateHeap);
static BOOL CheckCreateHeap(daTbox_c *self) {
  WWHD_FUNC(0x024B4F30, BOOL, self);
  return CreateHeap(self);
}
VERIFY(0x024B4F30, CheckCreateHeap);
static BOOL checkRoomDisp(daTbox_c *self, s32 room) {
  WWHD_FUNC(0x024B4F34, BOOL, self, room);
  play();
  u32 r = 0x1047E6CC + (u32)room * 0x22C;
  if (gabi::load<u8>(r + 0x21C) & 8)
    return FALSE;
  play();
  return (gabi::load<u8>(r + 0x21C) & 16) != 0;
}
VERIFY(0x024B4F34, checkRoomDisp);
static BOOL daTbox_Execute(daTbox_c *self) {
  WWHD_FUNC(0x024B577C, BOOL, self);
  return gabi::call<BOOL>(0x024B54F8, self);
}
VERIFY(0x024B577C, daTbox_Execute);
static BOOL daTbox_Delete(daTbox_c *self) {
  WWHD_FUNC(0x024B5780, BOOL, self);
  if ((void *)self->mpBgWCurrent) {
    u32 p = play();
    gabi::call(0x020087EC, p + 0x12A0, (void *)self->mpBgWCurrent);
  }
  u32 v = gabi::load<u32>(addr(self) + 0x75C);
  gabi::call_ptr(gabi::load<u32>(v + 0x44), addr(self) + 0x75C);
  gabi::call(0x025204C8, &self->mPhase, 0x1003FF5C);
  s8 h = gabi::load<s8>(0x1046E6DC);
  if (h >= 0) {
    gabi::call(0x025F0A18, (s32)h);
    gabi::store<u8>(0x1046E6DC, 255);
  }
  return TRUE;
}
VERIFY(0x024B5780, daTbox_Delete);
static void OpenInit_com(daTbox_c *self) {
  WWHD_FUNC(0x024B63A8, void, self);
  gabi::store<f32>(addr(self) + 0x3BC, 1);
  s32 type = getFuncType(self);
  if (type == 7 || type == 8)
    gabi::call(0x025207D8, 1, (u32)self->mParameters >> 7 & 31);
  else
    gabi::call(0x025B8C0C, gabi::load<u32>(0x101F84DC) + 0x798,
               (u32)self->mParameters >> 7 & 31);
  u8 sw = gabi::load<u8>(addr(self) + 0x2FD);
  if (sw != 255)
    gabi::call(0x025B9E38, save(), sw, (s32)self->mRoomNo);
  setDzb(self);
  s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
  gabi::call(0x025E1A40, 0x690C, &self->eyePos, 0, reverb);
  reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
  gabi::call(0x025E1A40, 0x690B, &self->eyePos, 0, reverb);
}
VERIFY(0x024B63A8, OpenInit_com);
static void demoProcOpen(daTbox_c *self) {
  WWHD_FUNC(0x024B6EF8, void, self);
  u16 time = self->mOpenTimer;
  if (time < 1000) {
    time++;
    self->mOpenTimer = time;
    if (time < 156)
      lightUpProc(self);
    else
      lightDownProc(self);
  } else
    lightDownProc(self);
  if ((u16)self->mOpenTimer == 36) {
    u16 flags = self->mFlags;
    gabi::store<f32>(addr(self) + 0x464, 1);
    self->mAllColRatio = 0.4f;
    gabi::store<f32>(addr(self) + 0x4F0, 1);
    gabi::store<f32>(addr(self) + 0x3BC, 1);
    gabi::store<f32>(addr(self) + 0x564, 1);
    self->mFlags = flags | 8;
  }
  darkProc(self);
  if ((void *)self->mSmokeEmitter)
    volmProc(self);
}
VERIFY(0x024B6EF8, demoProcOpen);
static BOOL boxCheck(daTbox_c *self) {
  WWHD_FUNC(0x024B72C8, BOOL, self);
  u32 p = play(), player = gabi::load<u32>(p + 0x5B2C);
  gabi::Local<cXyz> difference, flat;
  gabi::call(0x0201ADE0, player + 0x314, difference.get(), &self->home.pos);
  flat->x = difference->x;
  flat->y = 0;
  flat->z = difference->z;
  if (!(gabi::call<f32>(0x028E8DD0, flat.get()) < 10000))
    return FALSE;
  p = play();
  u32 a = gabi::load<u32>(p + 0x5B2C);
  if (gabi::call<s32>(0x025D68A0, self, a) >= 0x2000)
    return FALSE;
  return gabi::call<s32>(0x025D68A0, player, self) < 0x2000;
}
VERIFY(0x024B72C8, boxCheck);
static BOOL actionDemo(daTbox_c *self) {
  WWHD_FUNC(0x024B7708, BOOL, self);
  s16 event = gabi::load<s16>(addr(self) + 0xFC);
  u32 p = play();
  if (!gabi::call<BOOL>(0x025440C8, p + 0x52C4, event)) {
    gabi::call<s32>(0x024B6F9C, self);
    return TRUE;
  }
  setAction(self, 0x024B79AC);
  p = play();
  gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
  gabi::call(0x02560444, 1.0f);
  self->mFlags = (u16)self->mFlags & 0xFFE7;
  u32 control = play() + 0x51D0;
  u32 id = gabi::call<u32>(0x0253F124, control, 0);
  gabi::store<u32>(control + 0xD0, id);
  if ((void *)self->mSmokeEmitter) {
    gabi::call(0x0255A374, addr(self) + 0x714);
    gabi::call(0x0255BA9C, addr(self) + 0x738);
    u32 e = addr((void *)self->mSmokeEmitter),
        flags = gabi::load<u32>(e + 0x254);
    gabi::store<u32>(e + 0x5C, 0xFFFFFFFF);
    gabi::store<u32>(e + 0x254, flags | 1);
    self->mSmokeEmitter = nullptr;
  }
  return TRUE;
}
VERIFY(0x024B7708, actionDemo);
static BOOL actionDemo2(daTbox_c *self) {
  WWHD_FUNC(0x024B7804, BOOL, self);
  u32 p = play();
  if (gabi::call<BOOL>(0x0254457C, p + 0x52C4, 0x10040090)) {
    setAction(self, 0x024B7394);
    p = play();
    gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
  } else
    gabi::call<s32>(0x024B6F9C, self);
  return TRUE;
}
VERIFY(0x024B7804, actionDemo2);
static void surfaceProc(daTbox_c *self) {
  WWHD_FUNC(0x024B6C10, void, self);
  if (!(void *)self->mpBgWCurrent || !((u16)self->mFlags & 32))
    return;
  f32 offset = self->mAppearingYOffset;
  if (offset < -1) {
    offset += 1;
    f32 x = self->current.pos.x, y = self->current.pos.y,
        z = self->current.pos.z;
    self->mAppearingYOffset = offset;
    gabi::call(0x028E93CC, 0x1048D0CC, x, y + offset, z);
  } else {
    self->mFlags = (u16)self->mFlags & 0xFFDF;
    f32 y = self->current.pos.y;
    f32 x = self->current.pos.x, z = self->current.pos.z;
    self->mAppearingYOffset = 0;
    gabi::call(0x028E93CC, 0x1048D0CC, x, y + 0.0f, z);
  }
  gabi::call(0x025F1C28, 0x1048D0CC, (s16)self->current.angle.y);
  gabi::call(0x028E90D4, 0x1048D0CC, addr(self) + 0x6E4);
  gabi::call(0x024F43DC, (void *)self->mpBgWCurrent);
}
VERIFY(0x024B6C10, surfaceProc);
static void sinit_d_a_tbox() {
  WWHD_FUNC(0x024B78F0, void, (u32)0);
  gabi::store<u32>(0x1046E6F4, 0);
  gabi::store<u32>(0x1046E6EC, 0);
  gabi::store<u32>(0x1046E6F8, 0);
  gabi::store<u32>(0x1046E6F0, 0);
  gabi::call(0x028F026C, 0x101D2380);
  gabi::store<f32>(0x1046E6D0, gabi::load<f32>(0x100400A8));
  gabi::store<f32>(0x1046E6D4, gabi::load<f32>(0x100400AC));
  gabi::call(0x028ED6F8, 0x1046E6D8);
  gabi::call(0x028F026C, 0x101D238C);
  gabi::call(0x028EAB2C, 0x1046E6D9);
  gabi::call(0x028F026C, 0x101D2398);
  daTbox_HIO_ct(0x1046E6DC);
}
VERIFY(0x024B78F0, sinit_d_a_tbox);
static void daTbox_dtor(daTbox_c *self, u32 flags) {
  WWHD_FUNC(0x024B79B4, void, self, flags);
  if (!self)
    return;
  u32 a = addr(self);
  gabi::call(0x02515A70, a + 0x9D4, 2);
  gabi::call(0x02515860, a + 0x998, 2);
  gabi::call(0x02018034, a + 0x96C, 2);
  gabi::store<u32>(a + 0x7B4, 0x1003FE24);
  gabi::store<u32>(a + 0x7A8, 0x1003FE34);
  gabi::call(0x024EFD9C, a + 0x794, 0);
  gabi::call(0x027F3628, a + 0x474, 0);
  gabi::call(0x027F3628, a + 0x3CC, 0);
  gabi::call(0x025D50BC, self, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, self);
}
VERIFY(0x024B79B4, daTbox_dtor);
static u32 particle(daTbox_c *self, u32 id, u32 angle, u32 type = 0, u32 cb = 0,
                    u32 alpha = 255) {
  u32 p = play(), manager = gabi::load<u32>(p + 0x5AB0);
  return gabi::call<u32>(0x025A847C, manager, type, id, &self->current.pos,
                         angle, 0, alpha, cb, -1, 0, 0, 0);
}
static void OpenInit(daTbox_c *self) {
  WWHD_FUNC(0x024B6494, void, self);
  OpenInit_com(self);
  u16 flags = self->mFlags;
  self->mIsFlashPlaying = 1;
  gabi::store<f32>(addr(self) + 0x464, 1);
  self->mOpenTimer = 0;
  gabi::store<f32>(addr(self) + 0x4F0, 1);
  self->mFlags = flags | 16;
  gabi::store<f32>(addr(self) + 0x564, 1);
  particle(self, 0x1F1, addr(&self->current.angle));
  particle(self, 0x1F2, addr(&self->current.angle));
  particle(self, 0x1F3, addr(&self->current.angle));
  particle(self, 0x1F4, addr(&self->current.angle));
  particle(self, 0x1F6, addr(&self->current.angle));
  u32 e = particle(self, 0x1F5, addr(&self->current.angle));
  self->mSmokeEmitter = gabi::at<void>(e);
  if (e)
    gabi::store<u8>(e + 0x247, 0);
}
VERIFY(0x024B6494, OpenInit);
static void demoInitAppear_Tact(daTbox_c *self) {
  WWHD_FUNC(0x024B6664, void, self);
  gabi::Local<csXyz> rot;
  rot->x = gabi::load<u16>(addr(&self->current.angle));
  rot->y = gabi::load<u16>(addr(&self->current.angle) + 2);
  rot->z = gabi::load<u16>(addr(&self->current.angle) + 4);
  particle(self, 0x82F1, addr(rot.get()));
  particle(self, 0x82F0, addr(rot.get()));
  rot->y = (s16)rot->y + 0x5555;
  particle(self, 0x82F0, addr(rot.get()));
  rot->y = (s16)rot->y + 0x5555;
  particle(self, 0x82F0, addr(rot.get()));
  s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
  gabi::call(0x025E1A40, 0x6A15, &self->eyePos, 0, reverb);
}
VERIFY(0x024B6664, demoInitAppear_Tact);
static void demoInitAppear(daTbox_c *self) {
  WWHD_FUNC(0x024B67E4, void, self);
  s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
  gabi::call(0x025E1A40, 0x692F, &self->eyePos, 0, reverb);
  particle(self, 0x3EB, 0);
  particle(self, 0x3EC, 0);
}
VERIFY(0x024B67E4, demoInitAppear);
static void demoProcAppear_Tact(daTbox_c *self) {
  WWHD_FUNC(0x024B68B0, void, self);
  s32 total = gabi::load<s16>(0x1046E6E0), reveal = gabi::load<s16>(0x1046E6DE);
  u16 timer = self->mAppearTimer;
  if ((u32)timer == (u32)(total - reveal)) {
    u16 flags = self->mFlags;
    u32 anim = addr((void *)self->mpAppearRegAnm);
    self->mFlags = flags & 0xFFFE;
    self->mInvisibleScrollVal = 2;
    gabi::store<f32>(anim + 4, (f32)gabi::load<s16>(anim + 10));
    gabi::call(0x025E742C, (void *)self->mpAppearRegAnm);
    self->mFlags = (u16)self->mFlags & 0xFFFB;
    gabi::call(0x027F58E0, (void *)self->mpChestMdl, 0);
  }
  gabi::call(0x025E742C, addr(self) + 0x65C);
  timer = self->mAppearTimer;
  if (timer) {
    timer--;
    self->mAppearTimer = timer;
    s32 down = gabi::load<s16>(0x1046E6E2);
    total = gabi::load<s16>(0x1046E6E0);
    f32 ratio = 0.4f;
    if ((s32)timer > total - down)
      ratio = gabi::fmadds((f32)((s32)timer + down - total), 0.6f / (f32)down,
                           ratio);
    else {
      s32 up = gabi::load<s16>(0x1046E6E4);
      if ((s32)timer < up)
        ratio = gabi::fmadds((f32)(up - timer), 0.6f / (f32)up, ratio);
    }
    gabi::call(0x02560444, ratio);
  } else {
    gabi::call(0x02560444, 1.0f);
    s32 staff = self->mStaffId;
    u32 p = play();
    gabi::call(0x02543280, p + 0x52C4, staff);
  }
}
VERIFY(0x024B68B0, demoProcAppear_Tact);
static void demoProcAppear(daTbox_c *self) {
  WWHD_FUNC(0x024B6AB8, void, self);
  u16 timer = self->mAppearTimer;
  if ((u32)timer - 1 < 120) {
    gabi::call(0x0200F5C8, &self->mInvisibleScrollVal, 2.0f, 0.033333335f);
    timer = self->mAppearTimer;
  }
  if (timer == 60) {
    gabi::store<f32>(addr((void *)self->mpAppearRegAnm) + 4, 150);
    timer = self->mAppearTimer;
  }
  if (timer == 5) {
    u32 e = particle(self, 0x2022, 0, 2, addr(self) + 0x75C, 185);
    if (e) {
      gabi::store<f32>(e + 0x70, 25);
      gabi::store<f32>(e + 0x58, 1);
      gabi::store<f32>(e + 0x34, 100);
    }
    timer = self->mAppearTimer;
  }
  if (timer == 4) {
    if (!gabi::load<u32>(addr(self) + 0x760)) {
      self->mAppearTimer = timer - 1;
      goto advance;
    }
    gabi::call(0x025A5F88, addr(self) + 0x75C);
    timer = self->mAppearTimer;
  }
  if (timer)
    self->mAppearTimer = timer - 1;
advance:
  if (gabi::call<s32>(0x025E742C, (void *)self->mpAppearRegAnm)) {
    s32 staff = self->mStaffId;
    u32 p = play();
    gabi::call(0x02543280, p + 0x52C4, staff);
    u16 flags = self->mFlags;
    self->mFlags = flags & 0xFFFB;
    gabi::call(0x027F58E0, (void *)self->mpChestMdl, 0);
  }
}
VERIFY(0x024B6AB8, demoProcAppear);
static BOOL actionOpenWait(daTbox_c *self) {
  WWHD_FUNC(0x024B7394, BOOL, self);
  u32 a = addr(self);
  if (gabi::load<u16>(a + 0xF8) == 3) {
    u32 p = play();
    gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 4);
    u32 item = (gabi::load<s16>(a + 0x2FC) >> 8) & 255;
    u32 id =
        gabi::call<u32>(0x025D7E88, &self->current.pos, item, -1, -1, 0, 0);
    if (id != 0xFFFFFFFF) {
      p = play();
      gabi::store<u32>(p + 0x52A0, id);
    }
    if (getShapeType(self)) {
      gabi::call(0x025E1918, 0x80000009);
      self->mAllColRatio = 0.4f;
      self->mFlags = (u16)self->mFlags | 8;
      gabi::call(0x02560444, 0.4f);
      lightReady(self);
      gabi::store<f32>(a + 0x728, 0);
      gabi::store<f32>(a + 0x74C, 0);
      gabi::call(0x0255A2B8, a + 0x714);
      gabi::call(0x0255B9C8, a + 0x738);
    }
    setAction(self, 0x024B7708);
    p = play();
    self->mStaffId = gabi::call<s32>(0x02542D88, p + 0x52C4, 0x10040014, 0, 0);
    gabi::call<s32>(0x024B6F9C, self);
  } else if (boxCheck(self)) {
    gabi::store<u16>(a + 0xFA, gabi::load<u16>(a + 0xFA) | 4);
    s32 shape = getShapeType(self);
    gabi::call(0x0253E9B0, a + 0xF8, shape ? 0x10040034 : 0x10040020);
  }
  return TRUE;
}
VERIFY(0x024B7394, actionOpenWait);
static BOOL actionSwOnWait(daTbox_c *self) {
  WWHD_FUNC(0x024B7514, BOOL, self);
  u32 a = addr(self);
  if (gabi::load<u16>(a + 0xF8) == 2) {
    setAction(self, 0x024B7804);
    u32 p = play();
    self->mStaffId = gabi::call<s32>(0x02542D88, p + 0x52C4, 0x10040060, 0, 0);
    gabi::call<s32>(0x024B6F9C, self);
  } else if (gabi::call<BOOL>(0x025BA0C0, save(),
                              (u32)self->mParameters >> 12 & 255,
                              (s32)self->mRoomNo)) {
    gabi::call(0x025D77DC, self, 0x10040048, 1, 65535);
    gabi::store<u16>(a + 0xFA, gabi::load<u16>(a + 0xFA) | 2);
  }
  return TRUE;
}
VERIFY(0x024B7514, actionSwOnWait);
static BOOL actionGenocide(daTbox_c *self) {
  WWHD_FUNC(0x024B75EC, BOOL, self);
  u32 a = addr(self);
  if (gabi::load<u16>(a + 0xF8) == 2) {
    setAction(self, 0x024B7804);
    u32 p = play();
    self->mStaffId = gabi::call<s32>(0x02542D88, p + 0x52C4, 0x10040084, 0, 0);
    gabi::call<s32>(0x024B6F9C, self);
    return TRUE;
  }
  s32 room = self->mRoomNo;
  if (room == -1 || room != gabi::load<s8>(0x1047E6C8))
    return TRUE;
  if (gabi::call<u32>(0x025D98E8, (s8)room))
    return TRUE;
  u8 delay = self->mGenocideDelayTimer;
  if (delay) {
    self->mGenocideDelayTimer = delay - 1;
    return TRUE;
  }
  gabi::call(0x025D77DC, self, 0x1004006C, 1, 65535);
  u16 flags = gabi::load<u16>(a + 0xFA);
  u32 prm = self->mParameters;
  gabi::store<u16>(a + 0xFA, flags | 2);
  gabi::call(0x025B9E38, save(), prm >> 12 & 255, (s32)self->mRoomNo);
  return TRUE;
}
VERIFY(0x024B75EC, actionGenocide);
struct SafeString_l {
  be<u32> text, vtable;
};
static u32 objectRes(u32 name, s32 index) {
  u32 control = gabi::load<u32>(0x101F4F28);
  gabi::Local<SafeString_l> str;
  str->text = name;
  str->vtable = 0x1003FDC4;
  return gabi::call<u32>(0x026066C4, control, str.get(), index);
}
static void copyMtx(u32 src, u32 dst) {
  f32 v[12];
  for (int i = 0; i < 12; i++)
    v[i] = gabi::load<f32>(src + i * 4);
  for (int i = 0; i < 12; i++)
    gabi::store<f32>(dst + i * 4, v[i]);
}
static s32 commonShapeSet(daTbox_c *self) {
  WWHD_FUNC(0x024B4368, s32, self);
  u32 info = getModelInfo(self);
  u32 data = objectRes(0x1003FE8C, gabi::load<s16>(info));
  if (!data)
    gabi::call(0x0273AA24, 0x1003FEA0, 0xA4, 0x1003FEB0);
  u32 anim = objectRes(0x1003FE8C, gabi::load<s16>(info + 2));
  if (!gabi::call<BOOL>(0x025E8508, addr(self) + 0x3BC, data, anim, 1, 0, 1.0f,
                        0, -1, 0))
    return 5;
  u32 flags = 0x11000022;
  if (gabi::load<s16>(info + 4) > 0) {
    u32 p = gabi::call<u32>(0x025E7C6C, 0);
    self->mpAppearTexAnm = gabi::at<void>(p);
    if (!p)
      return 5;
    anim = objectRes(0x1003FE8C, gabi::load<s16>(info + 4));
    if (!gabi::call<BOOL>(0x025E7CE0, (void *)self->mpAppearTexAnm, data, anim,
                          1, 2, 1.0f, 0, -1, 0, 0))
      return 5;
    flags = 0x11001222;
  }
  if (gabi::load<s16>(info + 6) > 0) {
    u32 p = gabi::call<u32>(0x025E80D0, 0);
    self->mpAppearRegAnm = gabi::at<void>(p);
    if (!p)
      return 5;
    anim = objectRes(0x1003FE8C, gabi::load<s16>(info + 6));
    if (!gabi::call<BOOL>(0x025E8154, (void *)self->mpAppearRegAnm, data, anim,
                          1, 0, 1.0f, 0, -1, 0, 0))
      return 5;
    flags |= 0x0F020000;
  }
  u32 model = gabi::call<u32>(0x025E38E0, data, 0x80000, flags);
  self->mpChestMdl = gabi::at<void>(model);
  if (!model)
    return 5;
  if (getFuncType(self) == 6) {
    data = objectRes(0x1003FE8C, 23);
    if (!data)
      gabi::call(0x0273AA24, 0x1003FEA0, 0xE9, 0x1003FE94);
    u32 p = gabi::call<u32>(0x025E38E0, data, 0x80000, 0x11000000);
    self->mpTactPlatformMdl = gabi::at<void>(p);
    if (!p)
      return 5;
    anim = objectRes(0x1003FE8C, 31);
    if (!gabi::call<BOOL>(0x025E8154, addr(self) + 0x65C, data, anim, 1, 0,
                          1.0f, 0, -1, 0, 0))
      return 5;
    model = addr((void *)self->mpChestMdl);
  }
  f32 x = self->scale.x, y = self->scale.y, z = self->scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  x = self->current.pos.x;
  y = self->current.pos.y;
  z = self->current.pos.z;
  gabi::call(0x028E93CC, 0x1048D0CC, x, y, z);
  gabi::call(0x025F1C28, 0x1048D0CC, (s16)self->current.angle.y);
  copyMtx(0x1048D0CC, addr((void *)self->mpChestMdl) + 0xC8);
  if (getFuncType(self) == 6) {
    gabi::call(0x025F24E0, 0.0f, 1.0f, 0.0f);
    copyMtx(0x1048D0CC, addr((void *)self->mpTactPlatformMdl) + 0xC8);
  }
  gabi::call(0x028E90D4, 0x1048D0CC, addr(self) + 0x6E4);
  return 4;
}
VERIFY(0x024B4368, commonShapeSet);
static s32 effectShapeSet(daTbox_c *self) {
  WWHD_FUNC(0x024B4B20, s32, self);
  u32 data = objectRes(0x1003FEF0, 22);
  if (!data)
    gabi::call(0x0273AA24, 0x1003FF10, 0x11E, 0x1003FEF8);
  u32 model = gabi::call<u32>(0x025E38E0, data, 0x80000, 0x1000200);
  self->mpFlashMdl = gabi::at<void>(model);
  if (!model)
    return 5;
  u32 anim = objectRes(0x1003FEF0, 11);
  if (!gabi::call<BOOL>(0x025E8508, addr(self) + 0x464, data, anim, 1, 0, 1.0f,
                        0, -1, 0))
    return 5;
  anim = objectRes(0x1003FEF0, 37);
  if (!gabi::call<BOOL>(0x025E7CE0, addr(self) + 0x4F0, data, anim, 1, 0, 1.0f,
                        0, -1, 0, 0))
    return 5;
  anim = objectRes(0x1003FEF0, 30);
  return gabi::call<BOOL>(0x025E8154, addr(self) + 0x564, data, anim, 1, 0,
                          1.0f, 0, -1, 0, 0)
             ? 4
             : 5;
}
VERIFY(0x024B4B20, effectShapeSet);
static s32 bgCheckSet(daTbox_c *self) {
  WWHD_FUNC(0x024B4CDC, s32, self);
  u32 info = getModelInfo(self);
  u32 data = objectRes(0x1003FF20, gabi::load<s16>(info + 8));
  if (!data)
    gabi::call(0x0273AA24, 0x1003FF28, 0x1AA, 0x1003FF38);
  u32 w = gabi::call<u32>(0x024F23F4, 0);
  self->mpBgWClosed = gabi::at<void>(w);
  if (!w || gabi::call<BOOL>(0x0200A030, w, data, 1, addr(self) + 0x6E4))
    return 5;
  data = objectRes(0x1003FF20, gabi::load<s16>(info + 10));
  if (!data)
    gabi::call(0x0273AA24, 0x1003FF28, 0x1BB, 0x1003FF38);
  w = gabi::call<u32>(0x024F23F4, 0);
  self->mpBgWOpen = gabi::at<void>(w);
  if (!w || gabi::call<BOOL>(0x0200A030, w, data, 1, addr(self) + 0x6E4))
    return 5;
  if (getFuncType(self) == 3) {
    data = objectRes(0x1003FF20, 46);
    if (!data)
      gabi::call(0x0273AA24, 0x1003FF28, 0x1CE, 0x1003FF38);
    w = gabi::call<u32>(0x024F23F4, 0);
    self->mpBgWVines = gabi::at<void>(w);
    if (!w || gabi::call<BOOL>(0x0200A030, w, data, 1, addr(self) + 0x6E4))
      return 5;
  }
  gabi::call(0x024F43DC, (void *)self->mpBgWClosed);
  gabi::call(0x024F43DC, (void *)self->mpBgWOpen);
  w = addr((void *)self->mpBgWVines);
  if (w)
    gabi::call(0x024F43DC, w);
  self->mpBgWCurrent = nullptr;
  return 4;
}
VERIFY(0x024B4CDC, bgCheckSet);
static BOOL execute(daTbox_c *self) {
  WWHD_FUNC(0x024B54F8, BOOL, self);
  s32 room = self->mRoomNo;
  if (room == -1 || checkRoomDisp(self, room) != 1)
    return TRUE;
  s16 index = self->mActionIndex, adjust = self->mActionAdjustment;
  u32 target;
  if (index < 0)
    target = self->mActionFunc;
  else {
    u32 vt = gabi::load<u32>(addr(self) + adjust +
                             gabi::load<s16>(addr(self) + 0x6DA));
    target = gabi::load<u32>(vt + (u32)(s32)index * 8 + 4);
  }
  gabi::call_ptr(target, addr(self) + adjust);
  u32 anim = addr((void *)self->mpAppearTexAnm);
  if (anim)
    gabi::call(0x025E742C, anim);
  if ((u8)self->mIsFlashPlaying) {
    gabi::call(0x025E742C, addr(self) + 0x464);
    gabi::call(0x025E742C, addr(self) + 0x4F0);
    if (gabi::call<BOOL>(0x025E742C, addr(self) + 0x564))
      self->mIsFlashPlaying = 0;
    u32 model = addr((void *)self->mpFlashMdl);
    gabi::store<f32>(model + 0xBC, 1.42857146f);
    gabi::store<f32>(model + 0xC4, 1);
    gabi::store<f32>(model + 0xC0, 1);
    f32 x = self->current.pos.x, y = self->current.pos.y,
        z = self->current.pos.z;
    gabi::call(0x028E93CC, 0x1048D0CC, x, y + 50, z);
    gabi::call(0x025F1C28, 0x1048D0CC,
               (s16)((s16)self->current.angle.y + 32767));
    copyMtx(0x1048D0CC, addr((void *)self->mpFlashMdl) + 0xC8);
  }
  if (getFuncType(self) == 5) {
    gabi::call(0x025D6870, self, 0);
    u32 p = play();
    gabi::call(0x024F08A8, addr(self) + 0x794, p + 0x12A0);
    u32 a = addr(self);
    gabi::store<u32>(a + 0x390, gabi::load<u32>(a + 0x314));
    f32 y = self->current.pos.y, z = self->current.pos.z;
    gabi::store<u32>(a + 0x398, gabi::load<u32>(a + 0x31C));
    u32 ybits = gabi::load<u32>(a + 0x318);
    f32 x = self->current.pos.x;
    gabi::store<u32>(a + 0x394, ybits);
    gabi::call(0x028E93CC, 0x1048D0CC, x, y, z);
    gabi::call(0x025F1C28, 0x1048D0CC, (s16)self->home.angle.y);
    copyMtx(0x1048D0CC, addr((void *)self->mpChestMdl) + 0xC8);
    gabi::call(0x028E90D4, 0x1048D0CC, a + 0x6E4);
    u32 w = addr((void *)self->mpBgWCurrent);
    if (w)
      gabi::call(0x024F43DC, w);
  }
  return TRUE;
}
VERIFY(0x024B54F8, execute);
static s32 daTbox_Create(daTbox_c *self) {
  WWHD_FUNC(0x024B6268, s32, self);
  u32 condition = self->actor_condition;
  if (!(condition & 8)) {
    if (self) {
      gabi::call<u32>(0x024B5808, self);
      condition = self->actor_condition;
    }
    self->actor_condition = condition | 8;
  }
  s32 phase = gabi::call<s32>(0x02520460, &self->mPhase, 0x1003FFAC);
  if (phase != 4)
    return phase;
  self->mRoomNo = (s16)self->home.angle.x & 63;
  if (!gabi::call<BOOL>(0x025D63E8, self, 0x024B4F30, 0))
    return 5;
  gabi::call(0x024B5CFC, self);
  gabi::store<u32>(addr(self) + 0x39C, 64);
  return phase;
}
VERIFY(0x024B6268, daTbox_Create);
static void cutEnd(daTbox_c *self) {
  s32 staff = self->mStaffId;
  u32 p = play();
  gabi::call(0x02543280, p + 0x52C4, staff);
}
static s32 demoProc(daTbox_c *self) {
  WWHD_FUNC(0x024B6F9C, s32, self);
  s32 staff = self->mStaffId;
  u32 p = play();
  u32 action =
      gabi::call<u32>(0x02542EDC, p + 0x52C4, staff, 0x101D2370, 4, 0, 0);
  staff = self->mStaffId;
  p = play();
  if (gabi::call<BOOL>(0x025447C8, p + 0x52C4, staff)) {
    self->mHasOpenAnmFinished = 0;
    if (action == 1) {
      OpenInit(self);
      lightReady(self);
      gabi::store<f32>(addr(self) + 0x728, 0);
      gabi::store<f32>(addr(self) + 0x74C, 0);
    } else if (action == 2) {
      self->mFlags = (u16)self->mFlags | 32;
      self->mAppearingYOffset = -130;
      setDzb(self);
      s32 type = getFuncType(self);
      u16 flags = self->mFlags;
      self->mFlags = flags & (type == 6 ? 0xFFFD : 0xFFFC);
      if (type == 6)
        demoInitAppear_Tact(self);
      else
        demoInitAppear(self);
    } else if (action == 3)
      OpenInit_com(self);
  }
  if (action == 1 || action == 3) {
    if ((u8)self->mHasOpenAnmFinished)
      cutEnd(self);
    else if (gabi::call<BOOL>(0x025E742C, addr(self) + 0x3BC)) {
      staff = self->mStaffId;
      self->mHasOpenAnmFinished = 1;
      p = play();
      gabi::call(0x02543280, p + 0x52C4, staff);
      s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
      gabi::call(0x025E1A40, 0x690D, &self->eyePos, 0, reverb);
    }
  } else if (action == 2) {
    if (getFuncType(self) == 6)
      demoProcAppear_Tact(self);
    else
      demoProcAppear(self);
    surfaceProc(self);
  } else
    cutEnd(self);
  u16 flags = self->mFlags;
  if (flags & 16) {
    demoProcOpen(self);
    flags = self->mFlags;
  }
  if (flags & 8)
    gabi::call(0x02560444, (f32)self->mAllColRatio);
  return 0;
}
VERIFY(0x024B6F9C, demoProc);
static void CreateInit(daTbox_c *self) {
  WWHD_FUNC(0x024B5CFC, void, self);
  s32 type = getFuncType(self);
  u32 a = addr(self), model = addr((void *)self->mpChestMdl);
  gabi::store<u8>(a + 0x771, 1);
  self->mFlags = 0;
  gabi::store<f32>(a + 0x3BC, 0);
  gabi::call(0x027F58E0, model, 0);
  if (checkOpen(self)) {
    f32 end = gabi::load<s16>(a + 0x3C6);
    setAction(self, 0x024B79AC);
    gabi::store<f32>(a + 0x3C0, end);
    if (checkEnv(self)) {
      u32 anim = addr((void *)self->mpAppearRegAnm);
      self->mInvisibleScrollVal = 2;
      gabi::store<f32>(anim + 4, (f32)gabi::load<s16>(anim + 10));
    }
  } else if (!checkEnv(self))
    setAction(self, 0x024B7394);
  else if (checkNormal(self)) {
    BOOL vines =
        type == 3 && !gabi::call<BOOL>(0x025BA0C0, save(),
                                       (u32)self->mParameters >> 12 & 255,
                                       (s32)self->mRoomNo);
    self->mInvisibleScrollVal = 2;
    u32 anim = addr((void *)self->mpAppearRegAnm);
    setAction(self, vines ? 0x024B6334 : 0x024B7394);
    gabi::store<f32>(anim + 4, (f32)gabi::load<s16>(anim + 10));
  } else {
    self->mFlags = (u16)self->mFlags | 4;
    gabi::call(0x027F58E0, (void *)self->mpChestMdl, 1);
    switch (type) {
    case 2:
      setAction(self, 0x024B75EC);
      self->mGenocideDelayTimer = 65;
      self->mFlags = (u16)self->mFlags | 3;
      self->mAppearTimer = 120;
      break;
    case 1:
    case 8:
      self->mFlags = (u16)self->mFlags | 3;
      self->mAppearTimer = 120;
      setAction(self, 0x024B7514);
      break;
    case 6:
      setAction(self, 0x024B7514);
      self->mFlags = (u16)self->mFlags | 3;
      self->mAppearTimer = gabi::load<s16>(0x1046E6E0);
      break;
    case 4:
      self->mFlags = (u16)self->mFlags | 2;
      setAction(self, 0x024B7514);
      self->mAppearTimer = 90;
      gabi::store<f32>(addr((void *)self->mpAppearRegAnm) + 4, 30);
      break;
    default:
      gabi::call(0x0273AA24, 0x1003FF9C, 0x340, 0x1003FF90);
      break;
    }
    self->mInvisibleScrollVal = -2;
  }
  lightReady(self);
  self->mAllColRatio = 1;
  if (gabi::load<s8>(0x1046E6DC) < 0) {
    u8 child = gabi::call<u8>(0x025F0A10, 0x1003FF94, 0x1046E6DC);
    gabi::store<u8>(0x1046E6DC, child);
  }
  self->shape_angle.z = 0;
  self->shape_angle.x = 0;
  self->current.angle.z = 0;
  self->current.angle.x = 0;
  gabi::call(0x02515F14, a + 0x998, 255, 255, self);
  gabi::call(0x02516518, a + 0x9D4, 0x101EA190);
  gabi::store<u32>(a + 0xA18, a + 0x998);
  setCollision(self);
  gabi::store<u32>(a + 0xA00, gabi::load<u32>(a + 0xA00) & ~1U);
  searchRoomNo(self);
  if (type == 5) {
    gabi::call(0x024EFF44, a + 0x958, 30.0f, 0.0f);
    gabi::call(0x024F06B4, a + 0x794, &self->current.pos, &self->old.pos, self,
               1, a + 0x958, &self->speed, 0, 0);
    self->gravity = -2.5f;
  }
  self->mTboxNo = (u32)self->mParameters >> 7 & 31;
}
VERIFY(0x024B5CFC, CreateInit);
static daTbox_c *daTbox_ct(daTbox_c *self) {
  WWHD_FUNC(0x024B5808, daTbox_c *, self);
  if (!self) {
    self = gabi::at<daTbox_c>(gabi::call<u32>(0x0273AD10, 0xB08));
    if (!self)
      return nullptr;
  }
  u32 a = addr(self);
  gabi::call(0x025D4ED0, self);
  self->__vtbl = 0x1003FE44;
  gabi::call(0x027F2BC0, a + 0x3BC, 0);
  gabi::store<u32>(a + 0x3CC, 0x1016E54C);
  gabi::call(0x027DA984, a + 0x3D0);
  gabi::store<u32>(a + 0x440, 0);
  gabi::store<u32>(a + 0x444, 0);
  gabi::store<u32>(a + 0x43C, 0);
  gabi::store<u32>(a + 0x414, 0);
  gabi::store<u32>(a + 0x3CC, 0x1003FDEC);
  gabi::store<u32>(a + 0x438, 0);
  gabi::store<u32>(a + 0x404, 0x1016D820);
  gabi::call(0x027F2BC0, a + 0x464, 0);
  gabi::store<u32>(a + 0x474, 0x1016E54C);
  gabi::call(0x027DA984, a + 0x478);
  gabi::store<u32>(a + 0x4E8, 0);
  gabi::store<u32>(a + 0x4E4, 0);
  gabi::store<u32>(a + 0x4BC, 0);
  gabi::store<u32>(a + 0x4AC, 0x1016D820);
  gabi::store<u32>(a + 0x4EC, 0);
  gabi::store<u32>(a + 0x474, 0x1003FDEC);
  gabi::store<u32>(a + 0x4E0, 0);
  gabi::call(0x025E7C6C, a + 0x4F0);
  gabi::call(0x025E80D0, a + 0x564);
  gabi::call(0x025E80D0, a + 0x5E0);
  gabi::call(0x025E80D0, a + 0x65C);
  gabi::store<f32>(a + 0x734, 1);
  gabi::store<f32>(a + 0x758, 1);
  gabi::call(0x025A5B18, a + 0x75C, 1);
  gabi::call(0x024F0474, a + 0x794);
  gabi::store<u32>(a + 0x7A4, 0x1003FE14);
  gabi::store<u32>(a + 0x7A8, 0x1003FE34);
  gabi::store<u8>(a + 0x7AC, 1);
  gabi::store<u32>(a + 0x7B4, 0x1003FE24);
  gabi::call(0x024EFE94, a + 0x958);
  gabi::call(0x0200BD2C, a + 0x998);
  gabi::call(0x02515DA0, a + 0x9B4);
  gabi::store<u32>(a + 0x9B0, 0x1004AE88);
  gabi::store<u32>(a + 0x9B4, 0x1004AEC0);
  gabi::call(0x02515FB8, a + 0x9D4);
  gabi::store<u32>(a + 0xAE8, 0x100015A8);
  gabi::store<u32>(a + 0xAE4, 0x1003FDDC);
  gabi::call(0x02018590, a + 0xAEC);
  gabi::store<u32>(a + 0xA10, 0x1004B108);
  gabi::store<u32>(a + 0xB00, 0x1004B150);
  gabi::store<u32>(a + 0xAE8, 0x1004B160);
  return self;
}
VERIFY(0x024B5808, daTbox_ct);
static u32 relativePtr(u32 object, u32 offset) {
  u32 p = object + offset, v = gabi::load<u32>(p);
  return v ? p + v : 0;
}
static void markShaderParameter(u32 material, u32 descriptor, s32 index) {
  if (gabi::load<s32>(descriptor + 4) < 0)
    return;
  u16 flags = gabi::load<u16>(material + 4);
  u32 mask = gabi::load<u32>(material + 12);
  gabi::store<u16>(material + 4, flags | 4);
  u32 word = mask + (u32)(index >> 5) * 4;
  gabi::store<u32>(word, gabi::load<u32>(word) | (1U << ((u32)index & 31)));
}
static void updateInvisibleMaterial(u32 material, u32 name, f32 value,
                                    bool scale) {
  u32 shader = gabi::load<u32>(material);
  u32 names = relativePtr(shader, 0x38);
  s32 index = gabi::call<s32>(0x027DF9B0, names, name);
  if (index < 0)
    return;
  shader = gabi::load<u32>(material);
  u32 table = relativePtr(shader, 0x34), desc = table + (u32)index * 20;
  markShaderParameter(material, desc, index);
  shader = gabi::load<u32>(material);
  table = relativePtr(shader, 0x34);
  u16 parent = gabi::load<u16>(desc + 12);
  markShaderParameter(material, table + parent * 20, parent);
  u32 parameters = gabi::load<u32>(material + 0x28) + gabi::load<u16>(desc + 2);
  if (scale) {
    gabi::store<f32>(parameters + 12, value);
    gabi::store<f32>(parameters, value);
  } else
    gabi::store<f32>(parameters + 8, value);
}
static void drawList(u32 source) {
  u32 p = play();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + source));
  p = play();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + source + 4));
}
static BOOL daTbox_Draw(daTbox_c *self) {
  WWHD_FUNC(0x024B4FA8, BOOL, self);
  s32 room = self->mRoomNo;
  if (room != -1 && !checkRoomDisp(self, room))
    return TRUE;
  u16 flags = self->mFlags;
  u8 number;
  if ((flags & 1) || (checkEnv(self) && (flags & 4)))
    number = self->mTboxNo;
  else
    number = 255;
  if (!checkOpen(self)) {
    u32 a = addr(self);
    s8 mapRoom = gabi::load<s8>(a + 0x3AF);
    u8 gba = self->gbaName;
    f32 x = self->current.pos.x, y = self->current.pos.y,
        z = self->current.pos.z;
    gabi::call(0x02590488, 5, mapRoom, -32768, number, gba, 0, x, y, z);
  }
  self->tevStr.mRoomNo = (s32)self->mRoomNo;
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, env, 0, &self->current.pos, &self->tevStr);
  if (getFuncType(self) == 6) {
    u32 data = gabi::load<u32>(addr((void *)self->mpTactPlatformMdl) + 0xAC);
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, env, (void *)self->mpTactPlatformMdl, &self->tevStr);
    gabi::call(0x025E83FC, addr(self) + 0x65C, data,
               gabi::load<f32>(addr(self) + 0x660));
    gabi::call(0x025E2DE0, (void *)self->mpTactPlatformMdl, 0);
  }
  if ((u16)self->mFlags & 1)
    return TRUE;
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, env, (void *)self->mpChestMdl, &self->tevStr);
  u32 data = gabi::load<u32>(addr((void *)self->mpChestMdl) + 0xAC);
  gabi::call(0x025E86B8, addr(self) + 0x3BC, data,
             gabi::load<f32>(addr(self) + 0x3C0));
  u32 anim = addr((void *)self->mpAppearTexAnm);
  if (anim)
    gabi::call(0x025E7FC4, anim, data, gabi::load<f32>(anim + 4));
  anim = addr((void *)self->mpAppearRegAnm);
  if (anim)
    gabi::call(0x025E83FC, anim, data, gabi::load<f32>(anim + 4));
  bool invisible = checkEnv(self) && ((u16)self->mFlags & 4);
  if (invisible) {
    f32 scroll = (f32)self->mInvisibleScrollVal - (-2.0f),
        whole = (s8)gabi::ftoi(scroll),
        scale = gabi::fmadds(scroll - whole, 0.5f, 0.5f);
    u8 i = 0;
    u32 md = gabi::call<u32>(0x027F3F8C, data);
    while (i < gabi::load<u16>(md + 0x24)) {
      u32 model = addr((void *)self->mpChestMdl),
          materials = gabi::load<u32>(model + 0x34),
          material = materials + i * 60;
      updateInvisibleMaterial(material, 0x1003FE64, scale, true);
      updateInvisibleMaterial(material, 0x1003FE74, whole, false);
      i++;
      md = gabi::call<u32>(0x027F3F8C, data);
    }
    if ((u16)self->mFlags & 4) {
      drawList(0x5D8C);
      gabi::call(0x025E2DE0, (void *)self->mpChestMdl, 0);
      drawList(0x5D78);
    } else
      gabi::call(0x025E2DE0, (void *)self->mpChestMdl, 0);
  } else
    gabi::call(0x025E2DE0, (void *)self->mpChestMdl, 0);
  if ((u8)self->mIsFlashPlaying && (u16)self->mOpenTimer >= 36) {
    data = gabi::load<u32>(addr((void *)self->mpFlashMdl) + 0xAC);
    gabi::call(0x025E86B8, addr(self) + 0x464, data,
               gabi::load<f32>(addr(self) + 0x468));
    gabi::call(0x025E83FC, addr(self) + 0x564, data,
               gabi::load<f32>(addr(self) + 0x568));
    gabi::call(0x025E7FC4, addr(self) + 0x4F0, data,
               gabi::load<f32>(addr(self) + 0x4F4));
    drawList(0x5D84);
    gabi::call(0x025E2DE0, (void *)self->mpFlashMdl, 0);
    drawList(0x5D78);
  }
  return TRUE;
}
VERIFY(0x024B4FA8, daTbox_Draw);

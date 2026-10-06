/* Warp-pot actor: GameCube reconstruction corrected against WWHD disassembly.
 */
#include "d/actor/d_a_obj_warpt.h"
static u8 *ptr(u32 a) { return gabi::at<u8>(a); }
static u32 play() { return gabi::call<u32>(0x025200D4); }
static bool used(u8 *bg) { return bg && gabi::load<u32>(gabi::ea(bg)) < 0x100; }
static s8 stay() { return gabi::load<s8>(0x1047E6C8); }
static u32 save() { return gabi::load<u32>(0x101F84DC); }
u8 daObj_Warpt_c::isHuta() {
  WWHD_FUNC(0x023B985C, u8, this);
  return m2C6;
}
VERIFY(0x023B985C, &daObj_Warpt_c::isHuta);
bool daObj_Warpt_c::isSp() {
  WWHD_FUNC(0x023B9864, bool, this);
  return ((u32)(s32)m2B4 - 2u) <= 2;
}
VERIFY(0x023B9864, &daObj_Warpt_c::isSp);
bool daObj_Warpt_c::isOtherHuta() {
  WWHD_FUNC(0x023B9884, bool, this);
  if (!isSp())
    return false;
  s32 sw = m2AC;
  if (sw == 255)
    return false;
  u32 s = save();
  s8 room = stay();
  return !gabi::call<s32>(0x025BA0C0, ptr(s + 0x20), sw, room);
}
VERIFY(0x023B9884, &daObj_Warpt_c::isOtherHuta);
bool daObj_Warpt_c::isRealHuta() {
  WWHD_FUNC(0x023B98F4, bool, this);
  return isHuta() && !isOtherHuta();
}
VERIFY(0x023B98F4, &daObj_Warpt_c::isRealHuta);
void daObj_Warpt_c::onWarpBit(u8 bit) {
  WWHD_FUNC(0x023B9E94, void, this, bit);
  u32 index = m2B8, s = save();
  u16 event = gabi::load<u16>(0x100337C8 + index * 2);
  u32 flags = gabi::call<u32>(0x025B8BB0, ptr(s + 0x644), event);
  index = m2B8;
  s = save();
  event = gabi::load<u16>(0x100337C8 + index * 2);
  gabi::call(0x025B8AF4, ptr(s + 0x644), event, (u8)(flags | bit));
}
VERIFY(0x023B9E94, &daObj_Warpt_c::onWarpBit);
bool daObj_Warpt_c::isWarpBit(u8 bit) {
  WWHD_FUNC(0x023B9F1C, bool, this, bit);
  u32 index = m2B8, s = save();
  u16 event = gabi::load<u16>(0x100337C8 + index * 2);
  return (u8)(gabi::call<u32>(0x025B8BB0, ptr(s + 0x644), event) & bit) != 0;
}
VERIFY(0x023B9F1C, &daObj_Warpt_c::isWarpBit);
void daObj_Warpt_c::warp(s32 stage) {
  WWHD_FUNC(0x023BB64C, void, this, stage);
  gabi::call(0x025E19CC, 0x6934, 0);
  if (stage != 255)
    gabi::call(0x02587EFC, (u8)stage, (s8)current.roomNo);
}
VERIFY(0x023BB64C, &daObj_Warpt_c::warp);
bool daObj_Warpt_c::spWarp() {
  WWHD_FUNC(0x023BB6A4, bool, this);
  switch ((s32)m2B4) {
  case 2:
    if (isWarpBit(2))
      warp(m2A4);
    else if (isWarpBit(4))
      warp(m2A8);
    else
      return false;
    break;
  case 3:
    if (isWarpBit(4))
      warp(m2A8);
    else if (isWarpBit(1))
      warp(m2A0);
    else
      return false;
    break;
  case 4:
    if (isWarpBit(1))
      warp(m2A0);
    else if (isWarpBit(2))
      warp(m2A4);
    else
      return false;
    break;
  }
  return true;
}
VERIFY(0x023BB6A4, &daObj_Warpt_c::spWarp);
bool daObj_Warpt_c::normalWarp() {
  WWHD_FUNC(0x023BB7AC, bool, this);
  s8 room = stay();
  u32 s = save();
  s32 sw = m2B0;
  if (!gabi::call<s32>(0x025BA0C0, ptr(s + 0x20), sw, room))
    return false;
  warp(m29C);
  return true;
}
VERIFY(0x023BB7AC, &daObj_Warpt_c::normalWarp);
void daObj_Warpt_c::openHuta() {
  WWHD_FUNC(0x023BB400, void, this);
  if (isSp()) {
    if (m2BC == 255)
      gabi::call(0x025E1988, 0x806);
    onWarpBit(m2C4);
  } else {
    s8 room = stay();
    u32 s = save();
    s32 sw = m2AC;
    gabi::call(0x025B9E38, ptr(s + 0x20), sw, room);
  }
  m2C6 = 0;
}
VERIFY(0x023BB400, &daObj_Warpt_c::openHuta);
void daObj_Warpt_c::initCollision() {
  WWHD_FUNC(0x023BA134, void, this);
  gabi::call(0x02515F14, &mStts, 255, 255, this);
  gabi::call(0x02516518, mCyl1, ptr(0x100337FC));
  u8 *bg = mpLidBgW;
  gabi::store<u32>(gabi::ea(mCyl1) + 0x44, gabi::ea(&mStts));
  if (bg && mpLidModel1 != nullptr) {
    gabi::call(0x02516518, mCyl2, ptr(0x10033840));
    gabi::store<u32>(gabi::ea(mCyl2) + 0x44, gabi::ea(&mStts));
  }
}
VERIFY(0x023BA134, &daObj_Warpt_c::initCollision);
void daObj_Warpt_c::setCollision() {
  WWHD_FUNC(0x023BA1BC, void, this);
  gabi::call(0x020182E0, ptr(gabi::ea(mCyl1) + 0x118), &current.pos);
  u32 p = play();
  gabi::call(0x0200E240, ptr(p + 0x26A4), mCyl1);
  if (mpLidBgW != nullptr && mpLidModel1 != nullptr) {
    u32 a = gabi::ea(mCyl2) + 0x2C;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~1u);
    gabi::call(0x020182E0, ptr(gabi::ea(mCyl2) + 0x118), &m830);
    p = play();
    gabi::call(0x0200E240, ptr(p + 0x26A4), mCyl2);
  }
}
VERIFY(0x023BA1BC, &daObj_Warpt_c::setCollision);
void daObj_Warpt_c::modeOpen() {
  WWHD_FUNC(0x023BB338, void, this);
  s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
  gabi::call(0x025E1A40, 0x3019, &eyePos, 0, reverb);
}
VERIFY(0x023BB338, &daObj_Warpt_c::modeOpen);
void daObj_Warpt_c::modeBreakFireInit() {
  WWHD_FUNC(0x023BB57C, void, this);
  s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
  gabi::call(0x025E1A40, 0x69EC, &eyePos, 0, reverb);
  m83C = 60;
}
VERIFY(0x023BB57C, &daObj_Warpt_c::modeBreakFireInit);
void daObj_Warpt_c::modeBreakFire() {
  WWHD_FUNC(0x023BB5CC, void, this);
  gabi::call(0x025E742C, mLidBrk);
  if (!gabi::call<s32>(0x0211D2F8, &m83C)) {
    if (used(mpLidBgW)) {
      u32 p = play();
      u8 *bg = mpLidBgW;
      gabi::call(0x020087EC, ptr(p + 0x12A0), bg);
    }
    openHuta();
    gabi::call(0x023B9CB0, this, 0, 0);
  }
}
VERIFY(0x023BB5CC, &daObj_Warpt_c::modeBreakFire);
void daObj_Warpt_c::modeClose() {
  WWHD_FUNC(0x023BB488, void, this);
  if (mpLidBgW != nullptr && mpLidModel1 != nullptr) {
    u32 hit = gabi::call<u32>(0x02516300, mCyl2);
    if (hit) {
      u32 type = gabi::load<u32>(hit + 0x10);
      if (!(type & 0x20) && (type & 0x60200)) {
        gabi::call(0x023BAA28, this, 0);
        gabi::call(0x023B9CB0, this, 0, 2);
      } else {
        gabi::call(0x023BAA28, this, 1);
        openHuta();
        gabi::call(0x023B9CB0, this, 0, 0);
      }
    }
  } else {
    s8 room = stay();
    u32 s = save();
    s32 sw = m2AC;
    if (gabi::call<s32>(0x025BA0C0, ptr(s + 0x20), sw, room)) {
      openHuta();
      gabi::call(0x023B9CB0, this, 0, 0);
    }
  }
}
VERIFY(0x023BB488, &daObj_Warpt_c::modeClose);

static u8 *resource(s32 index) {
  gabi::Local<be<u32>[2]> name;
  (*name)[0] = 0x100337C0;
  (*name)[1] = 0x100335C8;
  u32 control = gabi::load<u32>(0x101F4F28);
  return gabi::call<u8 *>(0x026066C4, ptr(control), name.get(), index);
}
bool daObj_Warpt_c::createHutaHeap() {
  WWHD_FUNC(0x023B9944, bool, this);
  u8 *data = resource(7);
  if (!data)
    gabi::call(0x0273AA24, ptr(0x10033648), 0x117, ptr(0x1003365C));
  mpLidModel1 = gabi::call<u8 *>(0x025E38E0, data, 0x80000, 0x11000022);
  if (mpLidModel1 == nullptr)
    return false;
  mpLidBgW = gabi::call<u8 *>(0x024F23F4, (u8 *)nullptr);
  if (mpLidBgW == nullptr)
    return false;
  data = resource(17);
  if (gabi::call<s32>(0x0200A030, (u8 *)mpLidBgW, data, 1, m344))
    return false;
  data = resource(10);
  if (!data)
    return false;
  mpLidModel2 = gabi::call<u8 *>(0x025E38E0, data, 0x80000, 0x11000022);
  if (mpLidModel2 == nullptr)
    return false;
  u8 *animation = resource(13);
  if (!animation)
    gabi::call(0x0273AA24, ptr(0x10033648), 0x132, ptr(0x10033644));
  return gabi::call<s32>(0x025E8154, mLidBrk, data, animation, 1, 2, 1.0f, 0,
                         -1, 0, 0) != 0;
}
VERIFY(0x023B9944, &daObj_Warpt_c::createHutaHeap);
bool daObj_Warpt_c::createBodyHeap() {
  WWHD_FUNC(0x023B9AE8, bool, this);
  u8 *data = resource(6);
  if (!data)
    gabi::call(0x0273AA24, ptr(0x10033668), 0x140, ptr(0x1003367C));
  mpBodyModel = gabi::call<u8 *>(0x025E38E0, data, 0x80000, 0x11000022);
  if (mpBodyModel == nullptr)
    return false;
  mpBodyBgW2 = gabi::call<u8 *>(0x024F23F4, (u8 *)nullptr);
  if (mpBodyBgW2 == nullptr)
    return false;
  data = resource(16);
  if (gabi::call<s32>(0x0200A030, (u8 *)mpBodyBgW2, data, 1, m310))
    return false;
  mpBodyBgW1 = gabi::call<u8 *>(0x024F23F4, (u8 *)nullptr);
  if (mpBodyBgW1 == nullptr)
    return false;
  data = resource(18);
  return gabi::call<s32>(0x0200A030, (u8 *)mpBodyBgW1, data, 1, m2DC) == 0;
}
VERIFY(0x023B9AE8, &daObj_Warpt_c::createBodyHeap);
BOOL daObj_Warpt_c::createHeap() {
  WWHD_FUNC(0x023B9C28, BOOL, this);
  if (isRealHuta()) {
    if (!createHutaHeap())
      return false;
  } else {
    mpLidBgW = nullptr;
    mpLidModel2 = nullptr;
    mpLidModel1 = nullptr;
  }
  return createBodyHeap();
}
VERIFY(0x023B9C28, &daObj_Warpt_c::createHeap);
BOOL warptHeapCallback(daObj_Warpt_c *self) {
  WWHD_FUNC(0x023B9CAC, BOOL, self);
  return self->createHeap();
}
VERIFY(0x023B9CAC, warptHeapCallback);

static void warptModeCall(u32 entry, void *self) {
  s16 delta = gabi::load<s16>(entry), index = gabi::load<s16>(entry + 2);
  u32 adjusted = gabi::ea(self) + delta, target;
  if (index < 0)
    target = gabi::load<u32>(entry + 4);
  else {
    u32 table = gabi::load<u32>(adjusted + gabi::load<s16>(entry + 6));
    target = gabi::load<u32>(table + (u32)index * 8 + 4);
  }
  gabi::call_ptr(target, ptr(adjusted));
}
void daObj_Warpt_c::modeProc(s32 operation, s32 mode) {
  WWHD_FUNC(0x023B9CB0, void, this, operation, mode);
  if (operation == 0) {
    mMode = mode;
    warptModeCall(0x10033688 + (u32)mode * 20, this);
  } else if (operation == 1)
    warptModeCall(0x10033690 + (u32)(s32)mMode * 20, this);
}
VERIFY(0x023B9CB0, &daObj_Warpt_c::modeProc);
void daObj_Warpt_c::getArg() {
  WWHD_FUNC(0x023B9F74, void, this);
  u32 parameters = gabi::load<u32>(gabi::ea(this) + 0xB0);
  s16 z = gabi::load<s16>(gabi::ea(this) + 0x2FC);
  s16 x = gabi::load<s16>(gabi::ea(this) + 0x2F8);
  m2B4 = parameters & 15;
  if (!isSp()) {
    m29C = (parameters >> 12) & 255;
    m298 = (parameters >> 4) & 255;
    m2AC = (u8)x;
    m2B0 = ((u16)x >> 8);
    if (m2B4 == 1) {
      s8 room = stay();
      u32 s = save();
      gabi::call(0x025B9E38, ptr(s + 0x20), (s32)m2AC, room);
    }
    s8 room = stay();
    u32 s = save();
    m2C6 = !gabi::call<s32>(0x025BA0C0, ptr(s + 0x20), (s32)m2AC, room);
  } else {
    m2A0 = (parameters >> 8) & 255;
    m2A8 = parameters >> 24;
    m2B8 = (parameters >> 4) & 15;
    m2A4 = (parameters >> 16) & 255;
    switch ((s32)m2B4) {
    case 2:
      m2C4 = 1;
      m2C5 = 0;
      break;
    case 3:
      m2C4 = 2;
      m2C5 = 1;
      break;
    case 4:
      m2C4 = 4;
      m2C5 = 2;
      break;
    }
    m2AC = (u8)x;
    m2B0 = (u16)x >> 8;
    m2BC = (u8)z;
    m2C0 = (u16)z >> 8;
    if (m2C0 != 255)
      onWarpBit(m2C4);
    m2C6 = !isWarpBit(m2C4);
  }
}
VERIFY(0x023B9F74, &daObj_Warpt_c::getArg);
static f32 warptLinear(u32 value) {
  WWHD_FUNC(0x023BA9A8, f32, value);
  f32 ratio = (f32)value / 255.0f;
  f32 result = gabi::call<f32>(0x028F4560, ratio, 2.2f);
  if (result < 0.0f)
    result = 0.0f;
  else if (result > 1.0f)
    result = 1.0f;
  return result;
}
VERIFY(0x023BA9A8, warptLinear);
static void warptColor(u8 *color) {
  WWHD_FUNC(0x023BB1FC, void, color);
  u32 a = gabi::ea(color);
  f32 value = warptLinear(gabi::load<u8>(a));
  u8 next = gabi::load<u8>(a + 1);
  gabi::store<u8>(a, (u8)gabi::ftoi(value * 255.0f));
  value = warptLinear(next);
  u8 last = gabi::load<u8>(a + 2);
  gabi::store<u8>(a + 1, (u8)gabi::ftoi(value * 255.0f));
  value = warptLinear(last);
  gabi::store<u8>(a + 2, (u8)gabi::ftoi(value * 255.0f));
}
VERIFY(0x023BB1FC, warptColor);
void daObj_Warpt_c::modeEventOpen() {
  WWHD_FUNC(0x023BB9F8, void, this);
  play();
  if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2) {
    u32 p = play();
    if (gabi::call<s32>(0x0254457C, ptr(p + 0x52C4), ptr(0x100337A0))) {
      p = play();
      gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
      modeProc(0, 0);
    }
  }
}
VERIFY(0x023BB9F8, &daObj_Warpt_c::modeEventOpen);

BOOL daObj_Warpt_c::remove() {
  WWHD_FUNC(0x023BA8C8, BOOL, this);
  gabi::call(0x025204C8, mPhase, ptr(0x100337C0));
  if (used(mpLidBgW)) {
    u32 p = play();
    gabi::call(0x020087EC, ptr(p + 0x12A0), (u8 *)mpLidBgW);
  }
  if (used(mpBodyBgW2)) {
    u32 p = play();
    gabi::call(0x020087EC, ptr(p + 0x12A0), (u8 *)mpBodyBgW2);
  }
  if (used(mpBodyBgW1)) {
    u32 p = play();
    gabi::call(0x020087EC, ptr(p + 0x12A0), (u8 *)mpBodyBgW1);
  }
  u32 table = m844.mVtable;
  gabi::call_ptr(gabi::load<u32>(table + 0x44), &m844);
  table = m858.mVtable;
  gabi::call_ptr(gabi::load<u32>(table + 0x44), &m858);
  return 1;
}
VERIFY(0x023BA8C8, &daObj_Warpt_c::remove);
BOOL warptDelete(daObj_Warpt_c *self) {
  WWHD_FUNC(0x023BA9A4, BOOL, self);
  return self->remove();
}
VERIFY(0x023BA9A4, warptDelete);
BOOL warptIsDelete(daObj_Warpt_c *self) {
  WWHD_FUNC(0x023BBC28, BOOL, self);
  return 1;
}
VERIFY(0x023BBC28, warptIsDelete);
static void warptEmpty30() { WWHD_FUNC(0x023BBC30, void); }
VERIFY(0x023BBC30, warptEmpty30);
static void warptEmpty34() { WWHD_FUNC(0x023BBC34, void); }
VERIFY(0x023BBC34, warptEmpty34);
static void warptEmptyE0() { WWHD_FUNC(0x023BBCE0, void); }
VERIFY(0x023BBCE0, warptEmptyE0);
static void warptHioDelete(u8 *self, s32 flags) {
  WWHD_FUNC(0x023BBC14, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x023BBC14, warptHioDelete);
static void warptDestructor(daObj_Warpt_c *self, s32 flags) {
  WWHD_FUNC(0x023BBC38, void, self, flags);
  if (self) {
    gabi::call(0x02018034, ptr(gabi::ea(self) + 0x980), 2);
    u32 a = gabi::ea(self);
    gabi::store<u32>(a + 0x7C8, 0x10033600);
    gabi::store<u32>(a + 0x7BC, 0x10033610);
    gabi::call(0x024EFD9C, self->mAcch, 0);
    gabi::call(0x02515A70, self->mCyl2, 2);
    gabi::call(0x02515A70, self->mCyl1, 2);
    gabi::call(0x02515860, &self->mStts, 2);
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x023BBC38, warptDestructor);

void daObj_Warpt_c::ride(fopAc_ac_c *rider) {
  WWHD_FUNC(0x023B9D5C, void, this, rider);
  if (!rider || gabi::load<s16>(gabi::ea(rider) + 8) != 0xA8)
    return;
  u32 p = play();
  u32 player = gabi::load<u32>(p + 0x5B2C);
  gabi::call<s32>(0x025D6894, this, ptr(player));
  gabi::Local<cXyz> position;
  *position = m830;
  gabi::call<s32>(0x025881A4, position.get(), 1.0f, 20.0f);
  if (gabi::load<u8>(0x1046CA02))
    return;
  u32 table = gabi::load<u32>(gabi::ea(rider) + 0xB4);
  if (gabi::call_ptr<s32>(gabi::load<u32>(table + 0x4C), rider))
    return;
  if (mMode == 3)
    return;
  gabi::Local<cXyz> inner;
  *inner = m830;
  f32 radius = gabi::load<f32>(0x1046CA08);
  if (gabi::call<s32>(0x025881A4, inner.get(), radius, 20.0f))
    modeProc(0, 3);
}
VERIFY(0x023B9D5C, &daObj_Warpt_c::ride);
static void warptRideCallback(u8 *bg, daObj_Warpt_c *self, fopAc_ac_c *rider) {
  WWHD_FUNC(0x023B9E88, void, bg, self, rider);
  self->ride(rider);
}
VERIFY(0x023B9E88, warptRideCallback);

void daObj_Warpt_c::modeOpenInit() {
  WWHD_FUNC(0x023BB2A4, void, this);
  if (m844.mEmitter == nullptr) {
    warptColor(ptr(gabi::ea(&m86C)));
    warptColor(ptr(gabi::ea(&m870)));
    u32 p = play();
    u32 control = gabi::load<u32>(p + 0x5AB0);
    gabi::call(0x025A847C, ptr(control), 0, 0x8161, &current.pos, (u8 *)nullptr,
               (u8 *)nullptr, 255, &m844, -1, &m86C, &m870, 0);
  }
  gabi::call(0x025A5AC8, &m858);
}
VERIFY(0x023BB2A4, &daObj_Warpt_c::modeOpenInit);
void daObj_Warpt_c::modeCloseInit() {
  WWHD_FUNC(0x023BB380, void, this);
  warptColor(ptr(gabi::ea(&m86C)));
  warptColor(ptr(gabi::ea(&m870)));
  u32 p = play();
  u32 control = gabi::load<u32>(p + 0x5AB0);
  gabi::call(0x025A847C, ptr(control), 0, 0x8162, &current.pos, (u8 *)nullptr,
             (u8 *)nullptr, 255, &m858, -1, &m86C, &m870, 0);
}
VERIFY(0x023BB380, &daObj_Warpt_c::modeCloseInit);

static void warptModelMatrix(u8 *model) {
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  u32 destination = gabi::ea(model) + 0xC8;
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(destination + i * 4, matrix[i]);
}
void daObj_Warpt_c::setMtx() {
  WWHD_FUNC(0x023BA250, void, this);
  gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)current.pos.x,
             (f32)current.pos.y, (f32)current.pos.z);
  gabi::call(0x025F1C28, ptr(0x1048D0CC),
             gabi::load<s16>(gabi::ea(this) + 0x32A));
  gabi::call(0x025F2518, (f32)scale.x, (f32)scale.y, (f32)scale.z);
  warptModelMatrix(mpBodyModel);
  gabi::call(0x028E90D4, ptr(0x1048D0CC), m310);
  gabi::call(0x028E90D4, ptr(0x1048D0CC), m2DC);
  gabi::call(0x024F43DC, (u8 *)mpBodyBgW1);
  m830.z = current.pos.z;
  m830.x = current.pos.x;
  f32 y = current.pos.y;
  m830.y = y;
  m830.y = y + gabi::load<f32>(0x1046CA04);
  play();
  if (m840 == 1 && mpBodyBgW2 != nullptr && !used(mpBodyBgW2)) {
    u32 p = play();
    gabi::call(0x024EEA6C, ptr(p + 0x12A0), (u8 *)mpBodyBgW2, this);
  }
  if (used(mpBodyBgW2))
    gabi::call(0x024F43DC, (u8 *)mpBodyBgW2);
  if (mpLidBgW != nullptr && mpLidModel1 != nullptr) {
    gabi::call(0x028E93CC, ptr(0x1048D0CC), (f32)m830.x, (f32)m830.y,
               (f32)m830.z);
    gabi::call(0x025F1C28, ptr(0x1048D0CC),
               gabi::load<s16>(gabi::ea(this) + 0x32A));
    gabi::call(0x025F2518, (f32)scale.x, (f32)scale.y, (f32)scale.z);
    warptModelMatrix(mpLidModel1);
    warptModelMatrix(mpLidModel2);
    gabi::call(0x028E90D4, ptr(0x1048D0CC), m344);
    gabi::call(0x024F43DC, (u8 *)mpLidBgW);
  }
}
VERIFY(0x023BA250, &daObj_Warpt_c::setMtx);

void daObj_Warpt_c::createInit() {
  WWHD_FUNC(0x023BA4E0, void, this);
  m86C = gabi::load<u32>(0x100337D4 + (u32)(s32)m2B4 * 4);
  m870 = gabi::load<u32>(0x100337E8 + (u32)(s32)m2B4 * 4);
  u32 model = gabi::ea((u8 *)mpBodyModel);
  gabi::store<u32>(gabi::ea(this) + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x025D674C, this, -80.0f, -0.0f, -80.0f, 80.0f, 160.0f, 80.0f);
  if (mpLidBgW != nullptr) {
    gabi::store<u32>(gabi::ea((u8 *)mpLidBgW) + 0xA8, 0x024EE658);
    u32 p = play();
    gabi::call(0x024EEA6C, ptr(p + 0x12A0), (u8 *)mpLidBgW, this);
  }
  gabi::store<u32>(gabi::ea((u8 *)mpBodyBgW1) + 0xA8, 0x024EE658);
  u32 p = play();
  gabi::call(0x024EEA6C, ptr(p + 0x12A0), (u8 *)mpBodyBgW1, this);
  modeProc(0, isHuta() ? 1 : 0);
  gabi::store<u32>(gabi::ea((u8 *)mpBodyBgW2) + 0xB0, 0x023B9E88);
  initCollision();
  setCollision();
  gabi::call(0x024EFF44, mAcchCir, 30.0f, 50.0f);
  u32 a = gabi::ea(this);
  gabi::call(0x024F06B4, mAcch, ptr(a + 0x314), ptr(a + 0x300), this, 1,
             mAcchCir, ptr(a + 0x33C), (u8 *)nullptr, (u8 *)nullptr);
  gabi::store<u32>(a + 0x7D0, gabi::load<u32>(a + 0x7D0) | 0x40C);
  gabi::store<f32>(a + 0x374, -6.5f);
  gabi::call(0x025D6870, this, (u8 *)nullptr);
  gabi::Local<cXyz> position;
  position->x = (f32)current.pos.x;
  position->y = (f32)current.pos.y;
  position->z = (f32)current.pos.z;
  s32 cooldown = 5;
  if (gabi::call<s32>(0x025881A4, position.get(), 50.0f, 100.0f))
    cooldown = 30;
  m840 = cooldown;
  setMtx();
}
VERIFY(0x023BA4E0, &daObj_Warpt_c::createInit);

s32 daObj_Warpt_c::create() {
  WWHD_FUNC(0x023BA6F0, s32, this);
  u32 a = gabi::ea(this), flags = gabi::load<u32>(a + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(a + 0xB4, 0x10033620);
      gabi::call(0x025E80D0, mLidBrk);
      gabi::call(0x0200BD2C, &mStts);
      gabi::call(0x02515DA0, ptr(a + 0x528));
      gabi::store<u32>(a + 0x524, 0x1004AE88);
      gabi::store<u32>(a + 0x528, 0x1004AEC0);
      gabi::call(0x02515FB8, mCyl1);
      gabi::store<u32>(a + 0x658, 0x100335E0);
      gabi::store<u32>(a + 0x65C, 0x100015A8);
      gabi::call(0x02018590, ptr(a + 0x660));
      gabi::store<u32>(a + 0x584, 0x1004B108);
      gabi::store<u32>(a + 0x65C, 0x1004B160);
      gabi::store<u32>(a + 0x674, 0x1004B150);
      gabi::call(0x02515FB8, mCyl2);
      gabi::store<u32>(a + 0x78C, 0x100015A8);
      gabi::store<u32>(a + 0x788, 0x100335E0);
      gabi::call(0x02018590, ptr(a + 0x790));
      gabi::store<u32>(a + 0x7A4, 0x1004B150);
      gabi::store<u32>(a + 0x78C, 0x1004B160);
      gabi::store<u32>(a + 0x6B4, 0x1004B108);
      gabi::call(0x024F0474, mAcch);
      gabi::store<u32>(a + 0x7B8, 0x100335F0);
      gabi::store<u32>(a + 0x7BC, 0x10033610);
      gabi::store<u8>(a + 0x7C0, 1);
      gabi::store<u32>(a + 0x7C8, 0x10033600);
      gabi::call(0x024EFE94, mAcchCir);
      gabi::call(0x025A5894, &m844, (u8 *)nullptr, (u8 *)nullptr);
      gabi::call(0x025A5894, &m858, (u8 *)nullptr, (u8 *)nullptr);
      flags = gabi::load<u32>(a + 0x2E4);
    }
    gabi::store<u32>(a + 0x2E4, flags | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, mPhase, ptr(0x100337C0));
  if (phase == 4) {
    getArg();
    u32 heapSize = isRealHuta() ? 0x1F00 : 0xE20;
    if (!gabi::call<s32>(0x025D63E8, this, ptr(0x023B9CAC), heapSize))
      return 5;
    createInit();
  }
  return phase;
}
VERIFY(0x023BA6F0, &daObj_Warpt_c::create);
s32 warptCreate(daObj_Warpt_c *self) {
  WWHD_FUNC(0x023BA8C4, s32, self);
  return self->create();
}
VERIFY(0x023BA8C4, warptCreate);

static u8 *warptHioConstruct(u8 *self) {
  WWHD_FUNC(0x023BBA6C, u8 *, self);
  if (!self)
    self = gabi::call<u8 *>(0x0273AD10, 0x1C);
  if (self) {
    u32 a = gabi::ea(self);
    gabi::store<u32>(a, 0x10033630);
    gabi::store<f32>(a + 0xC, 30.0f);
    gabi::store<f32>(a + 0x10, -2.5f);
    gabi::store<f32>(a + 8, 150.0f);
    gabi::store<u8>(a + 4, 0);
    gabi::store<u8>(a + 5, 0);
    gabi::store<u8>(a + 6, 0);
    if (!gabi::load<u32>(0x101FDC64)) {
      gabi::store<u32>(0x101FDC64, 1);
      gabi::call(0xC000A848, ptr(0x101FEC1D), ptr(0x100335C0), 4);
    }
    gabi::store<u32>(a + 0x14, gabi::load<u32>(0x101FEC1D));
    if (!gabi::load<u32>(0x101FDC64)) {
      gabi::store<u32>(0x101FDC64, 1);
      gabi::call(0xC000A848, ptr(0x101FEC1D), ptr(0x100335C0), 4);
    }
    gabi::store<u32>(a + 0x18, gabi::load<u32>(0x101FEC1D));
  }
  return self;
}
VERIFY(0x023BBA6C, warptHioConstruct);
static void warptStaticInit() {
  WWHD_FUNC(0x023BBB74, void);
  for (u32 i = 0; i < 4; i++)
    gabi::store<u32>(0x1046C9EC + i * 4, 0);
  gabi::call(0x028F026C, ptr(0x101CDDE8));
  gabi::store<f32>(0x1046C9E0, gabi::load<f32>(0x100337B8));
  gabi::store<f32>(0x1046C9E4, gabi::load<f32>(0x100337BC));
  gabi::call(0x028ED6F8, ptr(0x1046C9E8));
  gabi::call(0x028F026C, ptr(0x101CDDF4));
  gabi::call(0x028EAB2C, ptr(0x1046C9E9));
  gabi::call(0x028F026C, ptr(0x101CDE00));
  warptHioConstruct(ptr(0x1046C9FC));
}
VERIFY(0x023BBB74, warptStaticInit);

void daObj_Warpt_c::breakHuta(s32 type) {
  WWHD_FUNC(0x023BAA28, void, this, type);
  if (mpLidBgW == nullptr || mpLidModel1 == nullptr)
    return;
  if (type == 0) {
    u32 p = play();
    u32 control = gabi::load<u32>(p + 0x5AB0);
    gabi::call(0x025A847C, ptr(control), 0, 0x8165, &m830, (u8 *)nullptr,
               (u8 *)nullptr, 255, (u8 *)nullptr, -1, (u8 *)nullptr,
               (u8 *)nullptr, 0);
    p = play();
    control = gabi::load<u32>(p + 0x5AB0);
    gabi::call(0x025A847C, ptr(control), 0, 0x8166, &m830, (u8 *)nullptr,
               (u8 *)nullptr, 255, (u8 *)nullptr, -1, (u8 *)nullptr,
               (u8 *)nullptr, 0);
  } else if (type == 1) {
    s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
    gabi::call(0x025E1A40, 0x6847, &eyePos, 0, reverb);
    u32 p = play();
    u32 control = gabi::load<u32>(p + 0x5AB0);
    u8 *color = ptr(gabi::ea(this) + 0x1A8);
    gabi::call(0x025A847C, ptr(control), 0, 0x3E5, &m830, (u8 *)nullptr,
               (u8 *)nullptr, 255, (u8 *)nullptr, -1, color, color, 0);
    if (used(mpLidBgW)) {
      p = play();
      gabi::call(0x020087EC, ptr(p + 0x12A0), (u8 *)mpLidBgW);
    }
  }
}
VERIFY(0x023BAA28, &daObj_Warpt_c::breakHuta);

static void warptDrawLists(u32 offset) {
  u32 p = play();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + offset));
  p = play();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + offset + 4));
}
BOOL daObj_Warpt_c::draw() {
  WWHD_FUNC(0x023BB074, BOOL, this);
  warptDrawLists(0x5D70);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, ptr(env), 0, &current.pos,
             ptr(gabi::ea(this) + 0x110));
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, ptr(env), (u8 *)mpBodyModel,
             ptr(gabi::ea(this) + 0x110));
  gabi::call(0x025E2DE0, (u8 *)mpBodyModel, 0);
  warptDrawLists(0x5D78);
  if (isHuta()) {
    if (mMode == 2 && mpLidModel2 != nullptr) {
      env = gabi::call<u32>(0x02555D0C);
      gabi::call(0x025626A4, ptr(env), 0, &m830, ptr(gabi::ea(this) + 0x110));
      env = gabi::call<u32>(0x02555D0C);
      gabi::call(0x02562F5C, ptr(env), (u8 *)mpLidModel2,
                 ptr(gabi::ea(this) + 0x110));
      u32 model = gabi::ea((u8 *)mpLidModel2);
      u32 data = gabi::load<u32>(model + 0xAC);
      f32 frame = gabi::load<f32>(gabi::ea(this) + 0x498);
      gabi::call(0x025E83FC, mLidBrk, ptr(data), frame);
      gabi::call(0x025E2DE0, (u8 *)mpLidModel2, 0);
      return 1;
    }
    if (mpLidBgW != nullptr && mpLidModel1 != nullptr) {
      warptDrawLists(0x5D70);
      env = gabi::call<u32>(0x02555D0C);
      gabi::call(0x025626A4, ptr(env), 0, &m830, ptr(gabi::ea(this) + 0x110));
      env = gabi::call<u32>(0x02555D0C);
      gabi::call(0x02562F5C, ptr(env), (u8 *)mpLidModel1,
                 ptr(gabi::ea(this) + 0x110));
      gabi::call(0x025E2DE0, (u8 *)mpLidModel1, 0);
      warptDrawLists(0x5D78);
    }
  }
  return 1;
}
VERIFY(0x023BB074, &daObj_Warpt_c::draw);
BOOL warptDraw(daObj_Warpt_c *self) {
  WWHD_FUNC(0x023BB1F8, BOOL, self);
  return self->draw();
}
VERIFY(0x023BB1F8, warptDraw);

void daObj_Warpt_c::checkHitSE() {
  WWHD_FUNC(0x023BABD8, void, this);
  u32 hit = gabi::call<u32>(0x02516300, mCyl1);
  gabi::call(0x02515E50, ptr(gabi::ea(this) + 0x528));
  if (!hit)
    return;
  u32 type = gabi::load<u32>(hit + 0x10);
  switch (type) {
  case 2:
  case 8:
  case 0x400:
  case 0x800:
  case 0x4000:
  case 0x8000:
  case 0x10000:
  case 0x40000:
  case 0x80000:
  case 0x100000:
  case 0x1000000:
  case 0x4000000:
  case 0x8000000:
  case 0x10000000:
  case 0x20000000:
    break;
  default:
    return;
  }
  u32 p = play();
  u32 player = gabi::load<u32>(p + 0x5B2C);
  u8 cut = gabi::load<u8>(player + 0x3AC);
  if (cut == 8 || cut == 9)
    return;
  s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
  gabi::call(0x025E1A40, 0x6822, &eyePos, 0, reverb);
  gabi::call(0x02312E54, this, mCyl1);
}
VERIFY(0x023BABD8, &daObj_Warpt_c::checkHitSE);

static bool warptCustomColor(u32 color) {
  return gabi::load<u8>(color) || gabi::load<u8>(color + 1) ||
         gabi::load<u8>(color + 2);
}
static void warptEmitterColor(u32 emitter, u32 color, u32 offset) {
  f32 red = warptLinear(gabi::load<u8>(color));
  u8 r = (u8)gabi::ftoi(red * 255.0f);
  f32 green = warptLinear(gabi::load<u8>(color + 1));
  u8 g = (u8)gabi::ftoi(green * 255.0f);
  f32 blue = warptLinear(gabi::load<u8>(color + 2));
  gabi::store<u8>(emitter + offset, r);
  gabi::store<u8>(emitter + offset + 1, g);
  gabi::store<u8>(emitter + offset + 2, (u8)gabi::ftoi(blue * 255.0f));
}
BOOL daObj_Warpt_c::execute() {
  WWHD_FUNC(0x023BAD74, BOOL, this);
  u32 emitter = gabi::ea((u8 *)m844.mEmitter);
  if (emitter) {
    if (warptCustomColor(0x1046CA10))
      warptEmitterColor(emitter, 0x1046CA10, 0x244);
    if (warptCustomColor(0x1046CA14)) {
      emitter = gabi::ea((u8 *)m844.mEmitter);
      warptEmitterColor(emitter, 0x1046CA14, 0x248);
    }
  }
  emitter = gabi::ea((u8 *)m858.mEmitter);
  if (emitter) {
    if (warptCustomColor(0x1046CA10))
      warptEmitterColor(emitter, 0x1046CA10, 0x244);
    if (warptCustomColor(0x1046CA14)) {
      emitter = gabi::ea((u8 *)m858.mEmitter);
      warptEmitterColor(emitter, 0x1046CA14, 0x248);
    }
  }
  if (gabi::load<u8>(0x1046CA00)) {
    gabi::store<u8>(0x1046CA00, 0);
    breakHuta(0);
    modeProc(0, 2);
  }
  modeProc(1, 5);
  gabi::call(0x025D6870, this, &mStts);
  u32 p = play();
  gabi::call(0x024F08A8, mAcch, ptr(p + 0x12A0));
  setMtx();
  setCollision();
  checkHitSE();
  gabi::call(0x022ED850, &m840);
  return 1;
}
VERIFY(0x023BAD74, &daObj_Warpt_c::execute);
BOOL warptExecute(daObj_Warpt_c *self) {
  WWHD_FUNC(0x023BB070, BOOL, self);
  return self->execute();
}
VERIFY(0x023BB070, warptExecute);

static bool warptCutWarp(u32 text) {
  u32 reference = 0x10033764;
  for (;;) {
    u8 a = gabi::load<u8>(text++), b = gabi::load<u8>(reference++);
    if (a != b)
      return false;
    if (!a)
      return true;
  }
}
void daObj_Warpt_c::modeEventWarp() {
  WWHD_FUNC(0x023BB820, void, this);
  u32 p = play();
  u32 player = gabi::load<u32>(p + 0x5B2C);
  if (gabi::load<u16>(gabi::ea(this) + 0xF8) != 2) {
    gabi::call(0x025D77DC, this, ptr(0x10033790), 4, 0xFFFF);
    return;
  }
  if (used(mpBodyBgW2)) {
    p = play();
    gabi::call(0x020087EC, ptr(p + 0x12A0), (u8 *)mpBodyBgW2);
  }
  gabi::store<u32>(player + 0x3B8, gabi::load<u32>(player + 0x3B8) | 0x40000);
  p = play();
  s32 staff =
      gabi::call<s32>(0x02542D88, ptr(p + 0x52C4), ptr(0x1003375C), 0, 0);
  p = play();
  u32 cut = gabi::call<u32>(0x02544830, ptr(p + 0x52C4), staff);
  bool warped = true;
  if (!cut) {
    gabi::call(0x0273AA24, ptr(0x1003377C), 0x33C, ptr(0x1003376C));
  } else if (warptCutWarp(cut)) {
    warped = isSp() ? spWarp() : normalWarp();
    p = play();
    gabi::call(0x02543280, ptr(p + 0x52C4), staff);
  }
  p = play();
  if (gabi::call<s32>(0x0254457C, ptr(p + 0x52C4), ptr(0x10033790)) &&
      !warped) {
    m840 = 30;
    p = play();
    gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
    gabi::store<u32>(player + 0x3BC, gabi::load<u32>(player + 0x3BC) | 0x10000);
    modeProc(0, 0);
  }
}
VERIFY(0x023BB820, &daObj_Warpt_c::modeEventWarp);

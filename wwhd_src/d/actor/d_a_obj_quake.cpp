/* HD Quake full actor reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_obj_quake.h"
u32 prmAbstract(daObjQuake_c *actor, u32 width, u32 shift) {
  WWHD_FUNC(0x02387230, u32, actor, width, shift);
  u32 parameters = gabi::load<u32>(gabi::ea(actor) + 0xB0);
  width &= 63;
  shift &= 63;
  u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
  return (shift < 32 ? parameters >> shift : 0u) & mask;
}
VERIFY(0x02387230, prmAbstract);

static QuakeHIO *hio() { return gabi::at<QuakeHIO>(0x1046BD08); }
u8 daObjQuake_c::getPrmType() {
  WWHD_FUNC(0x02386B20, u8, this);
  return (u8)prmAbstract(this, 3, 8);
}
VERIFY(0x02386B20, &daObjQuake_c::getPrmType);
s32 daObjQuake_c::Create() {
  WWHD_FUNC(0x02386B4C, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      flags = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0xB4, 0x1002EBDC);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  u32 save = gabi::load<u32>(0x101F84DC);
  if (gabi::call<s32>(0x025B7D90, gabi::at<u8>(save + 0xD4), 1))
    return 3;
  if (getPrmType() >= 3)
    return 5;
  gabi::call(0x025DA884, gabi::at<u8>(base + 0xDC));
  mType = getPrmType();
  mActive = 0;
  u32 play = gabi::call<u32>(0x025200D4), object = play + 0x5150;
  u32 table = gabi::load<u32>(object);
  u32 info =
      gabi::call_ptr<u32>(gabi::load<u32>(table + 0x15C), gabi::at<u8>(object));
  f32 seconds = (f32)gabi::load<u8>(info + 0xF);
  mDuration = seconds * 30.0f;
  u8 type = mType;
  if (type == 0 || type == 2) {
    mStart = 4.0f;
    mEnd = 6.0f;
  } else if (type == 1) {
    mStart = 0.0f;
    mEnd = 6.0f;
  }
  if (hio()->mNo < 0)
    hio()->mNo = (s8)gabi::call<s32>(0x025F0A10, STR(0x1002EC08), hio());
  hio()->mCount = (u32)hio()->mCount + 1u;
  return 4;
}
VERIFY(0x02386B4C, &daObjQuake_c::Create);
s32 wrapperCreate(daObjQuake_c *actor) {
  WWHD_FUNC(0x02386CE8, s32, actor);
  return actor->Create();
}
VERIFY(0x02386CE8, wrapperCreate);
BOOL Delete(daObjQuake_c *actor) {
  WWHD_FUNC(0x02386CEC, BOOL, actor);
  return TRUE;
}
VERIFY(0x02386CEC, Delete);
s32 daObjQuake_c::getPrmPower() {
  WWHD_FUNC(0x02386CF4, s32, this);
  return (s32)(prmAbstract(this, 3, 11) + 1u);
}
VERIFY(0x02386CF4, &daObjQuake_c::getPrmPower);
u8 daObjQuake_c::getPrmSch() {
  WWHD_FUNC(0x02386D20, u8, this);
  return (u8)prmAbstract(this, 8, 0);
}
VERIFY(0x02386D20, &daObjQuake_c::getPrmSch);
BOOL daObjQuake_c::Execute() {
  WWHD_FUNC(0x02386D4C, BOOL, this);
  mTimer = (f32)gabi::call<s32>(0x025602CC);
  u32 mask = 0x2E;
  s32 power = getPrmPower(), cameraPower = power, motorPower = power;
  u8 cameraOverride = hio()->mCameraOverride,
     motorOverride = hio()->mMotorOverride;
  if (cameraOverride)
    cameraPower = (u8)hio()->mCameraPower + 1;
  if (motorOverride) {
    motorPower = (u8)hio()->mMotorPower + 1;
    mask = hio()->mCamera ? (mask | 2u) : (mask & ~2u);
    mask = hio()->mSound ? (mask | 4u) : (mask & ~4u);
    mask = hio()->mMotor ? (mask | 8u) : (mask & ~8u);
    mask = hio()->mExtra ? (mask | 0x10u) : (mask & ~0x10u);
  }
  u8 schedule = getPrmSch();
  BOOL active = FALSE;
  if (schedule & gabi::call<u32>(0x025602A8)) {
    u32 play = gabi::call<u32>(0x025200D4);
    if (!(gabi::load<u16>(play + 0x52B8) & 4)) {
      f32 end = mEnd, divisor = gabi::load<f32>(0x1002EC04);
      end = end / divisor;
      f32 start = mStart;
      start = start / divisor;
      f32 duration = mDuration, lower = start * duration, timer = mTimer,
          upper = end * duration;
      if (!(timer < lower) && timer < upper)
        active = TRUE;
    }
  }
  if (mActive) {
    if (mType == 1)
      gabi::call(0x025E1988, 0x105E);
    if (!active) {
      u8 type = mType;
      if (type == 0 || type == 2) {
        u32 play = gabi::call<u32>(0x025200D4);
        gabi::call(0x025CB610, gabi::at<u8>(play + 0x599C),
                   type == 0 ? mask : 1u);
      }
      mActive = 0;
    }
  } else if (active) {
    u8 type = mType;
    f32 zero = gabi::load<f32>(0x1002EC00), one = gabi::load<f32>(0x1002EC20);
    if (type == 0 || type == 2) {
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::Local<cXyz> direction;
      direction->x = zero;
      direction->y = one;
      direction->z = zero;
      gabi::call(0x025CB408, gabi::at<u8>(play + 0x599C),
                 type == 0 ? motorPower : cameraPower, type == 0 ? mask : 1u,
                 direction.get());
    }
    mActive = 1;
  }
  return TRUE;
}
VERIFY(0x02386D4C, &daObjQuake_c::Execute);
BOOL wrapperExecute(daObjQuake_c *actor) {
  WWHD_FUNC(0x02387008, BOOL, actor);
  return actor->Execute();
}
VERIFY(0x02387008, wrapperExecute);
BOOL daObjQuake_c::IsDelete() {
  WWHD_FUNC(0x0238700C, BOOL, this);
  if (mActive) {
    u8 type = mType;
    if (type == 0 || type == 2) {
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::call(0x025CB610, gabi::at<u8>(play + 0x599C), type == 0 ? 0x3E : 1);
    }
  }
  s32 number = (s8)hio()->mNo;
  if (number >= 0) {
    u32 count = (u32)hio()->mCount - 1u;
    hio()->mCount = count;
    if (count == 0) {
      gabi::call(0x025F0A18, number);
      hio()->mNo = -1;
    }
  }
  return TRUE;
}
VERIFY(0x0238700C, &daObjQuake_c::IsDelete);
BOOL wrapperIsDelete(daObjQuake_c *actor) {
  WWHD_FUNC(0x023870B8, BOOL, actor);
  return actor->IsDelete();
}
VERIFY(0x023870B8, wrapperIsDelete);
QuakeHIO *constructHIO(QuakeHIO *p) {
  WWHD_FUNC(0x023870BC, QuakeHIO *, p);
  if (!p)
    p = gabi::call<QuakeHIO *>(0x0273AD10, 0x14);
  if (p) {
    p->mCount = 0;
    p->mNo = -1;
    p->mVtable = 0x1002EBCC;
    p->mMotorOverride = 0;
    p->mSound = 1;
    p->mMotorPower = 3;
    p->mMotor = 1;
    p->mCameraPower = 3;
    p->mCamera = 1;
    p->mExtra = 0;
    p->mCameraOverride = 0;
  }
  return p;
}
VERIFY(0x023870BC, constructHIO);
void staticInitialize() {
  WWHD_FUNC(0x02387134, void);
  gabi::store<u32>(0x1046BD00, 0);
  gabi::store<u32>(0x1046BCF8, 0);
  gabi::store<u32>(0x1046BD04, 0);
  gabi::store<u32>(0x1046BCFC, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101CC718));
  gabi::store<f32>(0x1046BCEC, -3.1415927410125732f);
  gabi::store<f32>(0x1046BCF0, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x1046BCF4));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CC724));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x1046BCF5));
  gabi::call(0x028F026C, gabi::at<u8>(0x101CC730));
  constructHIO(hio());
}
VERIFY(0x02387134, staticInitialize);
BOOL Draw(daObjQuake_c *actor) {
  WWHD_FUNC(0x023871D4, BOOL, actor);
  return TRUE;
}
VERIFY(0x023871D4, Draw);
void destruct(daObjQuake_c *actor, s32 flags) {
  WWHD_FUNC(0x023871DC, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x023871DC, destruct);

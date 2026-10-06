/* WWHD Drift. Ported from zeldaret/tww with audited HD layout/behavior.
 */
#include "d/actor/d_a_obj_drift.h"
namespace daObjDrift {
struct Quaternion {
  be<f32> x, y, z, w;
};
void Act_c::set_mtx() {
  WWHD_FUNC(0x023384F0, void, this);
  auto *matrix = gabi::at<Mtx34>(0x1048D0CC);
  f32 x = current.pos.x, y = current.pos.y, z = current.pos.z;
  gabi::call(0x028E93CC, matrix, x, y, z);
  gabi::Local<cXyz> axis;
  axis->x = mTiltX;
  axis->y = 1.0f;
  axis->z = mTiltZ;
  gabi::Local<Quaternion> quat;
  gabi::call(0x02312548, quat.get(), axis.get());
  gabi::call(0x025F25CC, quat.get());
  s16 ay = shape_angle.y, az = shape_angle.z, ax = shape_angle.x;
  gabi::call(0x025F1B48, matrix, ax, ay, az);
  f32 values[12];
  for (u32 i = 0; i < 12; ++i)
    values[i] = gabi::load<f32>(0x1048D0CC + 4 * i);
  u32 model = gabi::ea((J3DModel *)mModel);
  for (u32 i = 0; i < 12; ++i)
    gabi::store<f32>(model + 0xC8 + 4 * i, values[i]);
  gabi::call(0x028E90D4, matrix, &mMtx);
  f32 height = mHeight, prev = mPrevPos.y;
  mPrevHeight = height;
  mHeight = prev;
  mPrevPos.x = gabi::load<f32>(0x1048D0D8);
  mPrevPos.y = gabi::load<f32>(0x1048D0E8);
  mPrevPos.z = gabi::load<f32>(0x1048D0F8);
}
VERIFY(0x023384F0, &Act_c::set_mtx);
void Act_c::init_mtx() {
  WWHD_FUNC(0x02338618, void, this);
  u32 model = gabi::ea((J3DModel *)mModel);
  f32 y = scale.y, x = scale.x, z = scale.z;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC0, y);
  gabi::store<f32>(model + 0xC4, z);
  set_mtx();
}
VERIFY(0x02338618, &Act_c::init_mtx);
void Act_c::calc_flower_param(cXyz *pos, csXyz *angle) {
  WWHD_FUNC(0x02338638, void, this, pos, angle);
  if (!gabi::load<u32>(0x104696BC)) {
    gabi::store<u32>(0x104696BC, 1);
    gabi::store<f32>(0x104696B8, 0);
    gabi::store<f32>(0x104696B0, -489.3500061035156f);
    gabi::store<f32>(0x104696B4, 600.0f);
  }
  gabi::call(0x028E8F64, &mMtx, gabi::at<cXyz>(0x104696B0), pos);
  gabi::call(0x025F232C, &mMtx, angle);
}
VERIFY(0x02338638, &Act_c::calc_flower_param);
BOOL Act_c::Mthd_Delete() {
  WWHD_FUNC(0x02338408, BOOL, this);
  BOOL ret = MoveBGDelete();
  gabi::call(0x025204C8, &mPhase, STR(0x10027838));
  return ret;
}
VERIFY(0x02338408, &Act_c::Mthd_Delete);
BOOL Act_c::CreateHeap() {
  WWHD_FUNC(0x02338454, BOOL, this);
  gabi::Local<SafeString> name;
  name->mStringTop = 0x10027838;
  name->__vtbl = 0x10027710;
  void *data = gabi::call<void *>(
      0x026066C4, gabi::at<u8>(gabi::load<u32>(0x101F4F28)), name.get(), 4);
  if (!data)
    gabi::call(0x0273AA24, STR(0x100277A0), 0x13A, STR(0x10027790));
  mModel = gabi::call<J3DModel *>(0x025E38E0, data, 0x80000, 0x11000022);
  return mModel != nullptr;
}
VERIFY(0x02338454, &Act_c::CreateHeap);
void Act_c::make_flower() {
  WWHD_FUNC(0x023386CC, void, this);
  if ((u32)mType < 2) {
    gabi::Local<cXyz> pos;
    gabi::Local<csXyz> angle;
    calc_flower_param(pos.get(), angle.get());
    s32 type = mType;
    s8 room = gabi::load<s8>(gabi::ea(this) + 0x2FE);
    s16 profile = gabi::load<s16>(0x100277C0 + (u32)type * 2);
    mFlowerPid = gabi::call<u32>(0x025D5834, profile, 0, pos.get(), room,
                                 angle.get(), 0, -1, 0);
  } else
    mFlowerPid = 0xFFFFFFFF;
}
VERIFY(0x023386CC, &Act_c::make_flower);
void Act_c::set_flower_current() {
  WWHD_FUNC(0x02338D28, void, this);
  u32 pid = mFlowerPid;
  if (pid == 0xFFFFFFFF)
    return;
  gabi::Local<be<u32>> id;
  *id.get() = pid;
  u32 flower = gabi::call<u32>(0x025D5218, 0x025E1234, id.get());
  if (!flower)
    return;
  gabi::Local<cXyz> pos;
  gabi::Local<csXyz> angle;
  calc_flower_param(pos.get(), angle.get());
  gabi::store<f32>(flower + 0x314, pos->x);
  gabi::store<f32>(flower + 0x318, pos->y);
  gabi::store<f32>(flower + 0x31C, pos->z);
  u32 offset = mType == 0 ? 0x328 : 0x320;
  gabi::store<u16>(flower + offset, (s16)angle->x);
  gabi::store<u16>(flower + offset + 2, (s16)angle->y);
  gabi::store<u16>(flower + offset + 4, (s16)angle->z);
}
VERIFY(0x02338D28, &Act_c::set_flower_current);
BOOL Act_c::Draw() {
  WWHD_FUNC(0x02338EC8, BOOL, this);
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
VERIFY(0x02338EC8, &Act_c::Draw);
void Act_c::mode_wait_init() {
  WWHD_FUNC(0x02338898, void, this);
  u32 base = gabi::ea(this);
  gabi::store<u32>(base + 0x2E0, gabi::load<u32>(base + 0x2E0) | 0x80);
  mMode = 0;
}
VERIFY(0x02338898, &Act_c::mode_wait_init);
void Act_c::mode_rot_init() {
  WWHD_FUNC(0x02338F60, void, this);
  u32 base = gabi::ea(this), status = gabi::load<u32>(base + 0x2E0) & ~0x80u;
  s8 room = current.roomNo;
  s16 angle = shape_angle.y;
  mRotTimer = 0;
  mMode = 1;
  mAngleChaseY = 0;
  shape_angle.y = (s16)(angle + 10);
  gabi::store<u32>(base + 0x2E0, status);
  mTargetAngleY = (s16)(angle + 0x8000);
  s32 reverb = gabi::call<s32>(0x02520540, room);
  gabi::call(0x025E1A40, 0x6A1C, &eyePos, 0, reverb);
}
VERIFY(0x02338F60, &Act_c::mode_rot_init);
void Act_c::mode_wait() {
  WWHD_FUNC(0x02338FF0, void, this);
  u32 base = gabi::ea(this);
  if (gabi::call<s32>(0x025162A4, mCylinder)) {
    gabi::call(0x0251621C, mCylinder);
    u32 play = gabi::call<u32>(0x025200D4),
        player = gabi::load<u32>(play + 0x5B2C);
    f32 distance =
        gabi::call<f32>(0x025D69AC, this, gabi::at<fopAc_ac_c>(player));
    if (distance > 302500.0f) {
      f32 x = gabi::load<f32>(base + 0x4E8), z = gabi::load<f32>(base + 0x4F0);
      mHitDir.x = x;
      f32 y = gabi::load<f32>(base + 0x4EC);
      mHitDir.z = z;
      mHitDir.y = y;
      gabi::Local<cXyz> output;
      gabi::call(0x0201B3C0, &mHitDir, output.get());
      mode_rot_init();
    }
  } else {
    gabi::call(0x025165A4, mCylinder, &current.pos);
    u32 play = gabi::call<u32>(0x025200D4);
    gabi::call(0x0200E240, gabi::at<u8>(play + 0x26A4), mCylinder);
  }
}
VERIFY(0x02338FF0, &Act_c::mode_wait);
void Act_c::mode_rot() {
  WWHD_FUNC(0x023390AC, void, this);
  s16 diff = (s16)((s16)mTargetAngleY - (s16)shape_angle.y);
  if (diff <= 0) {
    shape_angle.y = mTargetAngleY;
    mHitDir.x = gabi::load<f32>(0x101FFBA8);
    mHitDir.y = gabi::load<f32>(0x101FFBAC);
    mHitDir.z = gabi::load<f32>(0x101FFBB0);
    mode_wait_init();
    f32 timer = mRotTimer;
    if (timer < 10.0f)
      mRotTimer = timer + 1.0f;
    else {
      mEffect = 1;
      mRotTimer = 0;
    }
  } else {
    if (diff < 0x2000)
      gabi::call(0x0200F564, &mAngleChaseY, 20, 10);
    else
      gabi::call(0x0200F564, &mAngleChaseY, 400, 20);
    shape_angle.y = (s16)((s16)shape_angle.y + (s16)mAngleChaseY);
    f32 timer = mRotTimer;
    if (timer < 10.0f)
      mRotTimer = timer + 1.0f;
    else {
      mEffect = 1;
      mRotTimer = 0;
    }
  }
}
VERIFY(0x023390AC, &Act_c::mode_rot);
u32 prmAbstract(Act_c *actor, u32 width, u32 shift) {
  WWHD_FUNC(0x02339314, u32, actor, width, shift);
  u32 parameters = actor->mParameters, w = width & 63, s = shift & 63;
  u32 mask = (w < 32 ? 1u << w : 0u) - 1u,
      value = s < 32 ? parameters >> s : 0u;
  return value & mask;
}
VERIFY(0x02339314, prmAbstract);
s32 Act_c::Mthd_Create() {
  WWHD_FUNC(0x023382AC, s32, this);
  u32 base = gabi::ea(this), status = gabi::load<u32>(base + 0x2E4);
  if (!(status & 8)) {
    if (this) {
      gabi::call(0x024F1D40, this);
      gabi::store<u32>(base + 0xB4, 0x10027884);
      gabi::call(0x0200BD2C, mCollisionStatus);
      gabi::call(0x02515DA0, gabi::at<u8>(base + 0x408));
      gabi::store<u32>(base + 0x404, 0x1004AE88);
      gabi::store<u32>(base + 0x408, 0x1004AEC0);
      gabi::call(0x02515FB8, mCylinder);
      gabi::store<u32>(base + 0x53C, 0x100015A8);
      gabi::store<u32>(base + 0x538, 0x10027728);
      gabi::call(0x02018590, gabi::at<u8>(base + 0x540));
      gabi::store<u32>(base + 0x464, 0x1004B108);
      gabi::store<u32>(base + 0x53C, 0x1004B160);
      status = gabi::load<u32>(base + 0x2E4);
      gabi::store<u32>(base + 0x554, 0x1004B150);
    }
    gabi::store<u32>(base + 0x2E4, status | 8);
  }
  s32 phase = gabi::call<s32>(0x02520460, &mPhase, STR(0x10027838));
  if (phase == 4) {
    mType = prmAbstract(this, 3, 0);
    phase = gabi::call<s32>(0x024F1D9C, this, STR(0x10027838), 7, 0x024EE708,
                            0x1B80);
    if (phase != 4 && phase != 5)
      gabi::call(0x0273AA24, STR(0x10027738), 0x19A, STR(0x1002774C));
  }
  return phase;
}
VERIFY(0x023382AC, &Act_c::Mthd_Create);
BOOL wrapperCreate(Act_c *actor) {
  WWHD_FUNC(0x023391C0, BOOL, actor);
  return actor->Mthd_Create();
}
VERIFY(0x023391C0, wrapperCreate);
BOOL wrapperDelete(Act_c *actor) {
  WWHD_FUNC(0x023391C4, BOOL, actor);
  return actor->Mthd_Delete();
}
VERIFY(0x023391C4, wrapperDelete);
BOOL wrapperExecute(Act_c *actor) {
  WWHD_FUNC(0x023391C8, BOOL, actor);
  return actor->MoveBGExecute();
}
VERIFY(0x023391C8, wrapperExecute);
BOOL wrapperDraw(Act_c *actor) {
  WWHD_FUNC(0x023391CC, BOOL, actor);
  u32 vt = actor->__vtbl;
  return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + 0x2C), actor);
}
VERIFY(0x023391CC, wrapperDraw);
BOOL wrapperIsDelete(Act_c *actor) {
  WWHD_FUNC(0x023391DC, BOOL, actor);
  u32 vt = actor->__vtbl;
  return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + 0x3C), actor);
}
VERIFY(0x023391DC, wrapperIsDelete);
void staticInitialize() {
  WWHD_FUNC(0x023391EC, void);
  gabi::store<u32>(0x104696A8, 0);
  gabi::store<u32>(0x104696A0, 0);
  gabi::store<u32>(0x104696AC, 0);
  gabi::store<u32>(0x104696A4, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8DDC));
  gabi::store<f32>(0x10469694, -3.1415927410125732f);
  gabi::store<f32>(0x10469698, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x1046969C));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8DE8));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x1046969D));
  gabi::call(0x028F026C, gabi::at<u8>(0x101C8DF4));
}
VERIFY(0x023391EC, staticInitialize);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x02339280, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x02339280, deleteStatic);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x02339294, BOOL, actor);
  return TRUE;
}
VERIFY(0x02339294, IsDelete);
void emptyVirtual() { WWHD_FUNC(0x0233929C, void); }
VERIFY(0x0233929C, emptyVirtual);
BOOL Delete(Act_c *actor) {
  WWHD_FUNC(0x023392A0, BOOL, actor);
  return TRUE;
}
VERIFY(0x023392A0, Delete);
void destruct(Act_c *actor, s32 flags) {
  WWHD_FUNC(0x023392A8, void, actor, flags);
  if (actor) {
    gabi::call(0x02515A70, actor->mCylinder, 2);
    gabi::call(0x02515860, actor->mCollisionStatus, 2);
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x023392A8, destruct);
void rideCB(dBgW *bg, Act_c *actor, fopAc_ac_c *rider) {
  WWHD_FUNC(0x02338758, void, bg, actor, rider);
  if (!rider || gabi::load<s16>(gabi::ea(rider) + 0xE) != 0xA8)
    return;
  actor->mRideYOff = -20.0f;
  gabi::Local<cXyz> delta;
  gabi::call(0x0201ADE0, &rider->current.pos, delta.get(), &actor->current.pos);
  gabi::Local<Mtx34> inverse;
  if (gabi::call<s32>(0x028E91EC, &actor->mMtx, inverse.get())) {
    u32 play = gabi::call<u32>(0x025200D4);
    if (!(gabi::load<u32>(play + 0x5CD8) & 0x100)) {
      gabi::Local<cXyz> local;
      gabi::call(0x028E9044, inverse.get(), delta.get(), local.get());
      gabi::Local<cXyz> flat;
      flat->x = local->x;
      flat->y = 0;
      flat->z = local->z;
      f32 mag = gabi::call<f32>(0x028E8DD0, flat.get()), y = local->y;
      if ((y < 400.0f && mag < 129600.0f) ||
          (!(y < 400.0f) && y < 550.0f && mag < 48400.0f)) {
        actor->mRideFlag = 1;
        return;
      }
      if (!actor->mRideFlag) {
        f32 z = delta->z, x = delta->x;
        actor->mTiltTargetZ = z * 0.00024000000848900527f;
        actor->mTiltTargetX = x * 0.00024000000848900527f;
      }
      return;
    }
  }
  actor->mRideFlag = 1;
}
VERIFY(0x02338758, rideCB);
BOOL Act_c::Create() {
  WWHD_FUNC(0x023388B0, BOOL, this);
  u32 base = gabi::ea(this), model = gabi::ea((J3DModel *)mModel);
  gabi::store<u32>(base + 0x348, model ? model + 0xC8 : 0);
  init_mtx();
  gabi::call(0x025D674C, this, -550.0f, -180.0f, -550.0f, 550.0f, 605.0f,
             550.0f);
  gabi::call(0x02515F14, mCollisionStatus, 255, 255, this);
  gabi::call(0x02516518, mCylinder, gabi::at<u8>(0x10027840));
  gabi::store<u32>(base + 0x46C, base + 0x3EC);
  f32 zx = gabi::load<f32>(0x101FFBA8), zy = gabi::load<f32>(0x101FFBAC),
      zz = gabi::load<f32>(0x101FFBB0);
  u32 flags = gabi::load<u32>(base + 0x4BC);
  gabi::store<f32>(base + 0x4DC, zx);
  gabi::store<f32>(base + 0x4E0, zy);
  gabi::store<u32>(base + 0x4BC, flags | 4);
  gabi::store<f32>(base + 0x4E4, zz);
  make_flower();
  gabi::store<u32>(gabi::ea((dBgW *)mpBgW) + 0xB0, 0x02338758);
  mode_wait_init();
  gabi::Local<be<f32>> water;
  if (gabi::call<s32>(0x025D9F70, &home.pos, water.get()))
    eyePos.y = *water.get();
  mRideYOff = 0;
  mTiltTargetZ = 0;
  mTiltTargetX = 0;
  mTiltVelZ = 0;
  mTiltVelX = 0;
  mTiltZ = 0;
  mTiltX = 0;
  mWavePhaseX = (s16)gabi::ftoi(gabi::call<f32>(0x02019918, 32768.0f));
  mWavePhaseY = (s16)gabi::ftoi(gabi::call<f32>(0x02019918, 32768.0f));
  s32 phase = gabi::ftoi(gabi::call<f32>(0x02019918, 32768.0f));
  mHitDir.x = zx;
  mHitDir.y = zy;
  f32 x = current.pos.x, y = current.pos.y;
  mPrevPos.x = x;
  mPrevPos.y = y;
  mHitDir.z = zz;
  f32 z = current.pos.z;
  mScratch.x = 5;
  mPrevPos.z = z;
  mRideFlag = 0;
  mScratch.y = 5;
  mScratch.z = 5;
  mHeight = y;
  mPrevHeight = y;
  mEffect = 0;
  mWavePhaseZ = (s16)phase;
  mTargetAngleY = 0;
  mAngleChaseY = 0;
  mUnusedEffect = 0;
  return TRUE;
}
VERIFY(0x023388B0, &Act_c::Create);
void Act_c::set_current() {
  WWHD_FUNC(0x02338AF8, void, this);
  f32 y = (f32)mRideYOff + (f32)home.pos.y;
  u16 phase = (s16)mWavePhaseX + 400;
  mWavePhaseX = (s16)phase;
  f32 delta = y - (f32)current.pos.y,
      sine = gabi::load<f32>(0x104A44F8 + (phase >> 3) * 8),
      wave = sine * 0.30000001192092896f;
  gravity = gabi::fmadds(delta, 0.019999999552965164f, wave);
  gabi::call(0x023123C0, this, 0, gabi::at<cXyz>(0x101FFBA8),
             0.03999999910593033f, 0.009999999776482582f);
  f32 factor = mRideFlag ? 0.20000000298023224f : 1.0f,
      chase = mMode == 1 ? 0.5f : 0.800000011920929f;
  gabi::call(0x028E8E64, &mHitDir, &mHitDir, 0.949999988079071f);
  u16 py = (s16)mWavePhaseY + 1000;
  f32 targetZ = mTiltTargetZ;
  u16 pz = (s16)mWavePhaseZ + 1111;
  mWavePhaseY = (s16)py;
  mWavePhaseZ = (s16)pz;
  f32 tz = mTiltZ, sinZ = gabi::load<f32>(0x104A44F8 + (pz >> 3) * 8),
      diffZ = tz - targetZ, targetX = mTiltTargetX;
  f32 waveX = sinZ * 0.009999999776482582f,
      sinY = gabi::load<f32>(0x104A44F8 + (py >> 3) * 8), dampZ = diffZ * chase,
      tx = mTiltX;
  f32 waveZ = sinY * 0.009999999776482582f, diffX = tx - targetX;
  f32 forceZ = gabi::fmsubs(waveZ, factor, dampZ), hitZ = mHitDir.z,
      dampX = diffX * chase;
  mRideFlag = 0;
  f32 accelZ = gabi::fmadds(hitZ, 0.5f, forceZ), velZ = mTiltVelZ,
      forceX = gabi::fmsubs(waveX, factor, dampX), hitX = mHitDir.x;
  velZ += accelZ;
  f32 accelX = gabi::fmadds(hitX, 0.5f, forceX), velX = mTiltVelX;
  f32 nextVelZ = velZ * 0.07000000029802322f;
  mTiltTargetZ = 0;
  mRideYOff = 0;
  velX += accelX;
  f32 nextZ = tz + nextVelZ;
  mTiltTargetX = 0;
  f32 nextVelX = velX * 0.07000000029802322f;
  mTiltZ = nextZ;
  mTiltVelZ = nextVelZ;
  f32 nextX = tx + nextVelX;
  mTiltVelX = nextVelX;
  mTiltX = nextX;
}
VERIFY(0x02338AF8, &Act_c::set_current);
BOOL Act_c::Execute(gptr<Mtx34> *matrix) {
  WWHD_FUNC(0x02338DFC, BOOL, this, matrix);
  u32 entry = 0x10027814 + (u32)mMode * 8;
  s16 adjust = gabi::load<s16>(entry), index = gabi::load<s16>(entry + 2);
  u32 object = gabi::ea(this) + (s32)adjust, target;
  if (index < 0)
    target = gabi::load<u32>(entry + 4);
  else {
    s16 offset = gabi::load<s16>(entry + 6);
    u32 vt = gabi::load<u32>(object + (s32)offset);
    target = gabi::load<u32>(vt + (u32)index * 8 + 4);
  }
  gabi::call_ptr(target, gabi::at<Act_c>(object));
  set_current();
  set_mtx();
  *matrix = &mMtx;
  set_flower_current();
  return TRUE;
}
VERIFY(0x02338DFC, &Act_c::Execute);
} // namespace daObjDrift

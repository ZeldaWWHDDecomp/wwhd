/** Bomb actor, ported from zeldaret/tww d_a_bomb3.inc and adapted to WWHD. */
#include "d/actor/d_a_bomb.h"
static bool chk_attrState(const daBomb_c *self, u32 mask) {
  WWHD_FUNC(0x020C4260, bool, self, mask);
  s32 state = self->prm_get_state();
  return (gabi::load<u32>(0x1000A73C + state * 4) & mask) != 0;
}
VERIFY(0x020C4260, chk_attrState);
bool daBomb_c::checkExplodeTimer() {
  WWHD_FUNC(0x020C6CC4, bool, this);
  bool explode = false;
  if (!chk_attrState(this, 0x200) && mRestTime > 0) {
    mRestTime = mRestTime - 1;
    if (mRestTime == 0)
      explode = procExplode_init() != 0;
  }
  return explode;
}
VERIFY(0x020C6CC4, &daBomb_c::checkExplodeTimer);
bool daBomb_c::checkExplode() {
  WWHD_FUNC(0x020C6D38, bool, this);
  return checkExplodeCc() || checkExplodeTimer();
}
VERIFY(0x020C6D38, &daBomb_c::checkExplode);
u8 daBomb_c::chk_water_in() {
  WWHD_FUNC(0x020C6E3C, u8, this);
  return mbWaterIn;
}
VERIFY(0x020C6E3C, &daBomb_c::chk_water_in);
u8 daBomb_c::chk_water_land() {
  WWHD_FUNC(0x020C7AEC, u8, this);
  return field_0x560;
}
VERIFY(0x020C7AEC, &daBomb_c::chk_water_land);
bool daBomb_c::chk_water_sink() {
  WWHD_FUNC(0x020C7A98, bool, this);
  return chk_water_in() && field_0x558 - current.pos.y > 30.0f;
}
VERIFY(0x020C7A98, &daBomb_c::chk_water_sink);
bool daBomb_c::chk_lava_hit() {
  WWHD_FUNC(0x020C7AF4, bool, this);
  return field_0x554 != -1000000000.0f && current.pos.y < field_0x554;
}
VERIFY(0x020C7AF4, &daBomb_c::chk_lava_hit);
bool daBomb_c::chk_dead_zone() {
  WWHD_FUNC(0x020C7EB0, bool, this);
  return mAcch.m_ground_h == -1000000000.0f && field_0x558 == -1000000000.0f &&
         field_0x554 == -1000000000.0f;
}
VERIFY(0x020C7EB0, &daBomb_c::chk_dead_zone);
bool daBomb_c::waitState_cannon() {
  WWHD_FUNC(0x020C7118, bool, this);
  se_cannon_fly_set();
  return false;
}
VERIFY(0x020C7118, &daBomb_c::waitState_cannon);
void daBomb_c::se_cannon_fly_stop() {
  WWHD_FUNC(0x020C5078, void, this);
  if (field_0x77F) {
    gabi::call(0x025E1AF8, &current.pos);
    field_0x77F = 0;
  }
}
VERIFY(0x020C5078, &daBomb_c::se_cannon_fly_stop);
void daBomb_c::water_tention() {
  WWHD_FUNC(0x020C6E44, void, this);
  if (chk_water_in()) {
    f32 height = field_0x558;
    if (height != -1000000000.0f) {
      f32 oldheight = field_0x55C;
      if (oldheight != -1000000000.0f) {
        f32 difference = height - oldheight;
        f32 factor = difference >= 0 ? 0.2f : 0.8f;
        current.pos.y = gabi::fmadds(difference, factor, current.pos.y);
      }
    }
  }
}
VERIFY(0x020C6E44, &daBomb_c::water_tention);
void daBomb_c::waitState_bomtyu() {
  WWHD_FUNC(0x020C713C, void, this);
  cLib_chaseF(&scale.x, 1, 0.05f);
  cLib_chaseF(&scale.y, 1, 0.05f);
  cLib_chaseF(&scale.z, 1, 0.05f);
  if (mBombFire) {
    setFuseEffect();
    mBombFire = 0;
    field_0x6F4 = 1;
    change_state(1);
  }
}
VERIFY(0x020C713C, &daBomb_c::waitState_bomtyu);
bool daBomb_c::procWait_init() {
  WWHD_FUNC(0x020C6394, bool, this);
  gabi::store<u16>(gabi::ea(this) + 0x3AC, 0);
  gabi::store<u16>(gabi::ea(this) + 0x3AE, 65535);
  mFunc[1] = 0x020C7358;
  if (chk_attrState(this, 0x80))
    change_state(1);
  u32 addr = gabi::ea(this) + 0x94C;
  gabi::store<u32>(addr, gabi::load<u32>(addr) | 1);
  return true;
}
VERIFY(0x020C6394, &daBomb_c::procWait_init);
void daBomb_c::anm_play_nut() {
  WWHD_FUNC(0x020C4C04, void, this);
  if (chk_state(5) || chk_state(6)) {
    mBck0.play();
    mBrk0.play();
  } else if (mRestTime + 2 <= 135) {
    mBck1.play();
    mBrk1.play();
  }
}
VERIFY(0x020C4C04, &daBomb_c::anm_play_nut);
void daBomb_c::draw_nut() {
  WWHD_FUNC(0x020C42A8, void, this);
  if (chk_state(5) || chk_state(6)) {
    J3DModelData *data = J3DModel_getModelData(mpModel);
    mBck0.entry(data, mBck0.getFrame());
    data = J3DModel_getModelData(mpModel);
    mBrk0.entry(data, mBrk0.getFrame());
  } else {
    J3DModelData *data = J3DModel_getModelData(mpModel);
    mBck1.entry(data, mBck1.getFrame());
    data = J3DModel_getModelData(mpModel);
    mBrk1.entry(data, mBrk1.getFrame());
  }
  u32 p = gabi::ea(dComIfGp_get());
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D58));
  p = gabi::ea(dComIfGp_get());
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D60));
  mDoExt_modelUpdateDL(mpModel, 0);
  p = gabi::ea(dComIfGp_get());
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D78));
  p = gabi::ea(dComIfGp_get());
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D7C));
}
VERIFY(0x020C42A8, &daBomb_c::draw_nut);
static u32 play_bomb() { return gabi::ea(dComIfGp_get()); }
static void drawBombModel(daBomb_c *self) {
  u32 p = play_bomb();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D58));
  p = play_bomb();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D60));
  mDoExt_modelUpdateDL(self->mpModel, 0);
  p = play_bomb();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D78));
  p = play_bomb();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D7C));
}
void daBomb_c::draw_norm() {
  WWHD_FUNC(0x020C4398, void, this);
  u32 player = gabi::load<u32>(play_bomb() + 0x5B34);
  s16 end = gabi::load<s16>(player + 0x44DE);
  f32 frame = (s32)end - (s32)mRestTime + 2;
  if (frame < 0)
    frame = 0;
  else if (frame >= (f32)end)
    frame = (f32)end - 0.001f;
  u32 brk = player + 0x44D4;
  u32 animation = gabi::load<u32>(brk + 0x10);
  gabi::store<f32>(brk + 4, frame);
  gabi::store<f32>(animation, frame);
  u32 curve = gabi::load<u32>(brk + 0x20);
  u32 function = gabi::load<u32>(curve + 0x10);
  u32 object = gabi::load<u32>(curve + 0x14);
  f32 scale = gabi::load<f32>(curve + 4), offset = gabi::load<f32>(curve + 8);
  f32 result = gabi::call_ptr<f32>(function, object, frame, scale, offset);
  gabi::store<f32>(curve, result);
  gabi::call(0x027DF40C, brk + 0x20);
  u32 bck = gabi::load<u32>(gabi::ea(this) + 0x448);
  u32 vt = gabi::load<u32>(bck + 4);
  s32 length = gabi::call_ptr<s32>(gabi::load<u32>(vt + 0x14), bck);
  frame = length - (s32)mRestTime;
  if (frame < 0)
    frame = 0;
  else if (frame >= (f32)length)
    frame = (f32)length - 0.001f;
  mBck0.entry(J3DModel_getModelData(mpModel), frame);
  drawBombModel(this);
  player = gabi::load<u32>(play_bomb() + 0x5B34);
  gabi::call(0x023D9340, player, gabi::ea(this) + 0x5CC, mpModel.get());
}
VERIFY(0x020C4398, &daBomb_c::draw_norm);
BOOL daBomb_c::draw() {
  WWHD_FUNC(0x020C4660, BOOL, this);
  if (chk_attrState(this, 0x20))
    return TRUE;
  if (chk_state(4) && field_0x77D != 0)
    return TRUE;
  gabi::Local<cXyz> position;
  u32 self = gabi::ea(this);
  for (u32 i = 0; i < 3; i++)
    gabi::store<u32>(gabi::ea(position.get()) + 4 * i,
                     gabi::load<u32>(self + 0x314 + 4 * i));
  if (gabi::call<BOOL>(0x02838148, 0x1048CFF0, 0x104B45F8, position.get(),
                       80.0f))
    return TRUE;
  if (field_0x7C8 > 0)
    return TRUE;
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, env, 0, &current.pos, &tevStr);
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, env, mpModel.get(), &tevStr);
  if (mType == 1)
    draw_nut();
  else
    draw_norm();
  return TRUE;
}
VERIFY(0x020C4660, &daBomb_c::draw);
BOOL daBomb_Draw(daBomb_c *self) {
  WWHD_FUNC(0x020C4758, BOOL, self);
  return self->draw();
}
VERIFY(0x020C4758, daBomb_Draw);
bool daBomb_c::checkExplodeCc_norm() {
  WWHD_FUNC(0x020C77F4, bool, this);
  bool explode = false;
  if (mSph.ChkTgHit()) {
    void *obj = mSph.GetTgHitObj();
    if (obj) {
      u32 type = gabi::load<u32>(gabi::ea(obj) + 0x10);
      explode = (type & 0x20) || !(type & 0x200000);
    }
    mSph.ClrTgHit();
  }
  return explode ? procExplode_init() != 0 : false;
}
VERIFY(0x020C77F4, &daBomb_c::checkExplodeCc_norm);
bool daBomb_c::checkExplodeCc_nut() {
  WWHD_FUNC(0x020C789C, bool, this);
  bool explode = false, hit = false;
  if (mSph.ChkTgHit()) {
    void *obj = mSph.GetTgHitObj();
    if (obj) {
      u32 type = gabi::load<u32>(gabi::ea(obj) + 0x10);
      if (type & 0x20)
        explode = true;
      else if (!(type & 0x200000))
        hit = true;
    }
    mSph.ClrTgHit();
  }
  if (mSph.ChkCoHit()) {
    if (field_0x780)
      hit = true;
    gabi::call(0x0251641C, &mSph);
  }
  if (explode)
    return procExplode_init() != 0;
  if (hit) {
    change_state(1);
    setFuseEffect();
    if (mRestTime > 30) {
      mRestTime = 30;
      mBrk1.mFrameCtrl.setFrame(105);
      mBck1.mFrameCtrl.setFrame(105);
    }
  }
  return false;
}
VERIFY(0x020C789C, &daBomb_c::checkExplodeCc_nut);
bool daBomb_c::checkExplodeCc_cannon() {
  WWHD_FUNC(0x020C79B8, bool, this);
  bool explode = false;
  if (mSph.ChkTgHit()) {
    void *obj = mSph.GetTgHitObj();
    if (obj)
      explode = !(gabi::load<u32>(gabi::ea(obj) + 0x10) & 0x200000);
    mSph.ClrTgHit();
  }
  if (mSph.ChkCoHit()) {
    gabi::call(0x0251641C, &mSph);
    return procExplode_init() != 0;
  }
  return explode ? procExplode_init() != 0 : false;
}
VERIFY(0x020C79B8, &daBomb_c::checkExplodeCc_cannon);
static u32 invokeBombMember(daBomb_c *self, u32 descriptor) {
  s16 index = gabi::load<s16>(descriptor + 2),
      delta = gabi::load<s16>(descriptor);
  u32 receiver = gabi::ea(self) + delta, target;
  if (index < 0)
    target = gabi::load<u32>(descriptor + 4);
  else {
    s16 offset = gabi::load<s16>(descriptor + 6);
    u32 vt = gabi::load<u32>(receiver + offset);
    target = gabi::load<u32>(vt + index * 8 + 4);
  }
  return gabi::call_ptr<u32>(target, receiver);
}
u32 daBomb_c::checkExplodeCc() {
  WWHD_FUNC(0x020C6C64, u32, this);
  return mRestTime > 0 ? invokeBombMember(this, 0x1000A86C + mType * 8) : false;
}
VERIFY(0x020C6C64, &daBomb_c::checkExplodeCc);
u32 checkExplodeBg_bomb(daBomb_c *self) {
  WWHD_FUNC(0x020C7090, u32, self);
  return invokeBombMember(self, 0x1000A898 + self->mType * 8);
}
VERIFY(0x020C7090, checkExplodeBg_bomb);
void daBomb_c::se_cannon_fly_set() {
  WWHD_FUNC(0x020C70DC, void, this);
  gabi::call(0x025E19CC, 0x381F, &current.pos);
  field_0x77F = 1;
}
VERIFY(0x020C70DC, &daBomb_c::se_cannon_fly_set);
bool daBomb_c::procCarry_init() {
  WWHD_FUNC(0x020C6D90, bool, this);
  if (chk_attrState(this, 0x100))
    setFuseEffect();
  u32 self = gabi::ea(this);
  gabi::store<u16>(self + 0x3AC, 0);
  mFunc[1] = 0x020C76E0;
  gabi::store<u16>(self + 0x3AE, 65535);
  change_state(2);
  speedF = 0;
  for (u32 i = 0; i < 3; i++)
    gabi::store<u32>(self + 0x33C + i * 4, gabi::load<u32>(0x101FFBA8 + i * 4));
  gabi::store<u32>(self + 0x39C, gabi::load<u32>(self + 0x39C) & ~0x10);
  gabi::store<u32>(self + 0x94C, gabi::load<u32>(self + 0x94C) & ~1);
  return true;
}
VERIFY(0x020C6D90, &daBomb_c::procCarry_init);
void daBomb_c::setRoomInfo() {
  WWHD_FUNC(0x020C5B1C, void, this);
  u32 self = gabi::ea(this);
  u8 room;
  if (mAcch.m_ground_h != -1000000000.0f) {
    u32 p = play_bomb();
    s32 id = gabi::call<s32>(0x024EF130, p + 0x12A0, self + 0x764);
    p = play_bomb();
    u32 color = gabi::call<u32>(0x024EEEB8, p + 0x12A0, self + 0x764);
    room = id;
    current.roomNo = room;
    gabi::store<u8>(self + 0x1C9, room);
    gabi::store<u8>(self + 0x1CA, color);
    gabi::store<u8>(self + 0x906, room);
  } else {
    room = gabi::load<u8>(0x1047E6C8);
    gabi::store<u8>(self + 0x1C9, room);
    gabi::store<u8>(self + 0x906, room);
    current.roomNo = room;
  }
}
VERIFY(0x020C5B1C, &daBomb_c::setRoomInfo);
void daBomb_c::bgCrrPos_lava() {
  WWHD_FUNC(0x020C5980, void, this);
  u32 self = gabi::ea(this);
  f32 z = current.pos.z, y = old.pos.y + 1, x = current.pos.x;
  gabi::store<f32>(self + 0x8AC, z);
  gabi::store<f32>(self + 0x8A4, x);
  gabi::store<f32>(self + 0x8A8, y);
  u32 p = play_bomb();
  field_0x554 = gabi::call<f32>(0x02008974, p + 0x12A0, self + 0x880);
}
VERIFY(0x020C5980, &daBomb_c::bgCrrPos_lava);
void daBomb_c::bgCrrPos() {
  WWHD_FUNC(0x020C5BAC, void, this);
  u32 p = play_bomb();
  gabi::call(0x024F08A8, &mAcch, p + 0x12A0);
  bgCrrPos_lava();
  bgCrrPos_water();
  setRoomInfo();
}
VERIFY(0x020C5BAC, &daBomb_c::bgCrrPos);
void daBomb_c::bgCrrPos_water() {
  WWHD_FUNC(0x020C59EC, void, this);
  f32 waterheight = gabi::load<f32>(gabi::ea(this) + 0x838);
  s32 area =
      gabi::call<s32>(0x0246B6A4, (f32)current.pos.x, (f32)current.pos.z);
  f32 wave =
      gabi::call<f32>(0x0246BA0C, (f32)current.pos.x, (f32)current.pos.z);
  bool water = (mAcch.m_flags & 0x1000) != 0,
       sea = area && current.pos.y < wave;
  field_0x55C = field_0x558;
  bool in = false;
  if (water && sea) {
    if (waterheight > wave)
      sea = false;
    else
      water = false;
  }
  if (water) {
    field_0x558 = waterheight;
    in = true;
    field_0x562 = 0;
  } else if (sea) {
    field_0x558 = wave;
    in = true;
    field_0x562 = 1;
  } else {
    field_0x562 = 0;
    field_0x558 = -1000000000.0f;
  }
  field_0x560 = in && mbWaterIn == 0;
  mbWaterIn = in;
}
VERIFY(0x020C59EC, &daBomb_c::bgCrrPos_water);
void daBomb_c::bound(f32 previousSpeed) {
  WWHD_FUNC(0x020C71FC, void, this, previousSpeed);
  u32 flags = mAcch.m_flags;
  if (flags & 0x10) {
    s16 angle = mCir.m_wall_angle_y;
    f32 horizontal = speedF * 0.8f;
    flags = mAcch.m_flags;
    speedF = horizontal;
    current.angle.y = (angle * 2) - (current.angle.y + 0x8000);
  }
  if (flags & 0x80) {
    gabi::call(0x02311CD8, this, gabi::ea(this) + 0x750, 0.6f);
    previousSpeed = previousSpeed * -0.6f;
    if (previousSpeed < 19.5f)
      field_0x780 = 0;
    else {
      speedF = speedF * 0.9f;
      f32 difference = 13 - previousSpeed;
      speed.y = difference >= 0 ? previousSpeed : 13;
    }
  } else if (flags & 0x20)
    gabi::call<f32>(0x0200ECD4, &speedF, 0.0f, 0.5f, 5.0f, 1.0f);
}
VERIFY(0x020C71FC, &daBomb_c::bound);
bool daBomb_c::checkExplodeBg_norm() {
  WWHD_FUNC(0x020C7C34, bool, this);
  bool sink = chk_water_sink();
  u8 land = chk_water_land();
  bool burn = chk_lava_hit();
  if (sink)
    field_0x781 = 1;
  if (land)
    eff_water_splash();
  return burn ? procExplode_init() != 0 : false;
}
VERIFY(0x020C7C34, &daBomb_c::checkExplodeBg_norm);
bool daBomb_c::checkExplodeBg_nut() {
  WWHD_FUNC(0x020C7DC4, bool, this);
  u8 sink = chk_water_in();
  bool burn = chk_lava_hit();
  u32 flags = mAcch.m_flags;
  bool hit = (flags & 0x30) || (flags & 0x200);
  if (burn)
    return procExplode_init() != 0;
  if (sink) {
    makeWaterEffect();
    field_0x781 = 1;
  } else if (hit && field_0x780) {
    change_state(1);
    setFuseEffect();
    if (mRestTime > 30) {
      mRestTime = 30;
      mBrk1.mFrameCtrl.setFrame(105);
      mBck1.mFrameCtrl.setFrame(105);
    }
  }
  return false;
}
VERIFY(0x020C7DC4, &daBomb_c::checkExplodeBg_nut);
bool daBomb_c::checkExplodeBg_cannon() {
  WWHD_FUNC(0x020C7EEC, bool, this);
  u8 sink = chk_water_in();
  bool burn = chk_lava_hit();
  u32 flags = mAcch.m_flags;
  bool hit = (flags & 0x30) || (flags & 0x200);
  bool dead = chk_dead_zone();
  if (burn || hit)
    return procExplode_init() != 0;
  if (sink) {
    makeWaterEffect();
    field_0x781 = 1;
  } else if (dead)
    field_0x781 = 1;
  return false;
}
VERIFY(0x020C7EEC, &daBomb_c::checkExplodeBg_cannon);
void daBomb_c::set_mtx() {
  WWHD_FUNC(0x020C4C84, void, this);
  u32 model = gabi::ea(mpModel.get());
  f32 z = scale.z, x = scale.x, y = scale.y;
  gabi::store<f32>(model + 0xBC, x);
  gabi::store<f32>(model + 0xC4, z);
  gabi::store<f32>(model + 0xC0, y);
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
  s16 angleX = 0;
  bool zero = gabi::call<bool>(0x020CB9E4, this);
  s16 angleY = shape_angle.y, angleZ = shape_angle.z;
  if (!zero)
    angleX = shape_angle.x;
  gabi::call(0x025F1B48, 0x1048D0CC, angleX, angleY, angleZ);
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = gabi::load<f32>(0x1048D0CC + i * 4);
  model = gabi::ea(mpModel.get());
  for (u32 i = 0; i < 12; i++)
    gabi::store<f32>(model + 0xC8 + i * 4, matrix[i]);
}
VERIFY(0x020C4C84, &daBomb_c::set_mtx);
u32 daBomb_c::procWait() {
  WWHD_FUNC(0x020C7358, u32, this);
  if (checkExplode())
    return 1;
  u32 self = gabi::ea(this);
  if (gabi::load<u32>(self + 0x2E0) & 0x2000)
    return gabi::call<u32>(0x020C6D90, this);
  posMoveF();
  f32 yspeed = speed.y;
  bgCrrPos();
  if (checkExplodeBg_bomb(this))
    return 1;
  if (chk_state(4)) {
    if (waitState_cannon())
      return 1;
  } else if (chk_state(7))
    waitState_bomtyu();
  if (field_0x781)
    return 1;
  bound(yspeed);
  u32 flags = mAcch.m_flags;
  bool ground = (flags & 0x20) != 0;
  if (!ground) {
    ground = chk_state(5);
    flags = mAcch.m_flags;
  }
  u32 attention = gabi::load<u32>(self + 0x39C);
  gabi::store<u32>(self + 0x39C, ground && !(flags & 0x80) ? attention | 0x10
                                                           : attention & ~0x10);
  return 1;
}
VERIFY(0x020C7358, &daBomb_c::procWait);
u32 daBomb_c::procCarry() {
  WWHD_FUNC(0x020C76E0, u32, this);
  if (!(gabi::load<u32>(gabi::ea(this) + 0x2E0) & 0x2000)) {
    if (speedF > 0)
      field_0x780 = 1;
    procWait_init();
    return procWait();
  }
  if (!checkExplode()) {
    f32 y = current.pos.y, z = current.pos.z, x = current.pos.x;
    bgCrrPos();
    current.pos.z = z;
    current.pos.x = x;
    current.pos.y = y;
  }
  return 1;
}
VERIFY(0x020C76E0, &daBomb_c::procCarry);
BOOL daBomb_IsDelete(daBomb_c *self) {
  WWHD_FUNC(0x020C5070, BOOL, self);
  return TRUE;
}
VERIFY(0x020C5070, daBomb_IsDelete);
void daBomb_c::posMoveF() {
  WWHD_FUNC(0x020C6EB4, void, this);
  bool suspended = mNoGravityTime > 0;
  f32 originalGravity = 0;
  if (suspended) {
    originalGravity = gravity;
    gravity = 0;
  }
  if (mType == 0 && chk_water_in()) {
    f32 vertical = speed.y * 0.9f, horizontal = speedF * 0.9f;
    speed.y = vertical;
    speedF = horizontal;
  }
  if (!chk_state(5) && !chk_state(6) && field_0x6F3 != 1) {
    water_tention();
    u32 p = play_bomb(), self = gabi::ea(this);
    u16 polygon = gabi::load<u16>(self + 0x764),
        background = gabi::load<u16>(self + 0x766);
    u32 plane = gabi::call<u32>(0x020084C8, p + 0x12A0, background, polygon);
    f32 magnitude = gabi::call<f32>(0x028E8DD0, &mWindVec);
    if (magnitude > 0.01f) {
      f32 friction = 0, grade = 0;
      if (plane) {
        grade = gabi::load<f32>(0x104A4F44);
        friction = 0.06f;
      }
      gabi::call(0x023121C4, this, &mStts, &mWindVec, plane, 0, 0.002f, 0.0005f,
                 friction, grade);
    } else
      gabi::call(0x025D6870, this, &mStts);
  }
  if (suspended) {
    s16 timer = mNoGravityTime;
    gravity = originalGravity;
    mNoGravityTime = timer - 1;
  }
}
VERIFY(0x020C6EB4, &daBomb_c::posMoveF);
void daBomb_c::set_real_shadow_flag() {
  WWHD_FUNC(0x020C4B10, void, this);
  bool state2 = chk_state(2), state3 = chk_state(3), shadow = false;
  u32 self = gabi::ea(this);
  s32 timer = field_0x7C8;
  if (timer <= 1 && (state2 || state3)) {
    if (gabi::load<u32>(self + 0x368) || state3 || timer == 1)
      shadow = true;
    else {
      u32 player = gabi::load<u32>(play_bomb() + 0x5B2C);
      u32 vt = gabi::load<u32>(player + 0xB4);
      u32 grabbed = gabi::call_ptr<u32>(gabi::load<u32>(vt + 0xBC), player);
      if (grabbed == gabi::load<u32>(self + 4) &&
          (gabi::load<u32>(player + 0x3C0) & 0x8000))
        shadow = true;
    }
  }
  if (field_0x7C8 > 0)
    field_0x7C8 = field_0x7C8 - 1;
  gabi::store<u32>(self + 0x368, shadow ? gabi::ea(mpModel.get()) : 0);
}
VERIFY(0x020C4B10, &daBomb_c::set_real_shadow_flag);
BOOL daBomb_c::execute() {
  WWHD_FUNC(0x020C4D84, BOOL, this);
  if (!gabi::load<u32>(0x104626A4)) {
    gabi::store<f32>(0x104626A0, 5);
    gabi::store<f32>(0x1046269C, 60);
    gabi::store<f32>(0x10462698, 0);
    gabi::store<u32>(0x104626A4, 1);
  }
  if (chk_state(4) && !field_0x77C) {
    f32 y = current.pos.y, x = current.pos.x, z = current.pos.z;
    gabi::Local<cXyz> position;
    position->set(x, y + 20, z);
    field_0x77C = 1;
    makeFireEffect(position.get(), &shape_angle);
  }
  set_wind_vec();
  u32 self = gabi::ea(this);
  if (gabi::load<s16>(self + 0x3AE) != 0)
    invokeBombMember(this, self + 0x3AC);
  if (field_0x781)
    gabi::call(0x025D57E0, this);
  else {
    set_real_shadow_flag();
    if (mType == 1)
      anm_play_nut();
    gabi::call(0x02515E50, self + 0x900);
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y + 50;
    gabi::store<f32>(self + 0x398, z);
    gabi::store<f32>(self + 0x390, x);
    eyePos.x = x;
    eyePos.y = y;
    eyePos.z = z;
    gabi::store<f32>(self + 0x394, y);
    if (!chk_attrState(this, 0x20)) {
      set_mtx();
      u32 oldFuse[3], oldSecond[3];
      for (u32 i = 0; i < 3; i++) {
        oldFuse[i] = gabi::load<u32>(self + 0xA84 + i * 4);
        oldSecond[i] = gabi::load<u32>(self + 0xA90 + i * 4);
      }
      for (u32 i = 0; i < 3; i++) {
        gabi::store<u32>(self + 0xA9C + i * 4, oldSecond[i]);
        gabi::store<u32>(self + 0xA90 + i * 4, oldFuse[i]);
      }
      PSMTXMultVec(gabi::at<Mtx34>(0x1048D0CC), gabi::at<cXyz>(0x10462698),
                   &mFusePos);
      y = current.pos.y;
      x = current.pos.x;
      z = current.pos.z;
      gabi::Local<cXyz> center;
      center->set(x, y + 30, z);
      mSph.SetC(center.get());
      mSph.SetR(scale.x * 30);
      if ((u32)mMassCounter != gabi::load<u32>(0x101FF558)) {
        u32 p = play_bomb();
        gabi::call(0x0200E240, p + 0x26A4, &mSph);
        p = play_bomb();
        gabi::call(0x02516C14, p + 0x4EF8, &mSph, 3);
        mMassCounter = gabi::load<s32>(0x101FF558);
      }
    }
    if (chk_attrState(this, 0x10) || field_0x6F4 == 1) {
      s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
      mDoAud_seStart(0x6100, &eyePos, 0, reverb);
    }
  }
  if (field_0x782)
    field_0x784 = (u32)field_0x784 + 1;
  return TRUE;
}
VERIFY(0x020C4D84, &daBomb_c::execute);
BOOL daBomb_Execute(daBomb_c *self) {
  WWHD_FUNC(0x020C506C, BOOL, self);
  return self->execute();
}
VERIFY(0x020C506C, daBomb_Execute);
bool daBomb_c::bombDelete() {
  WWHD_FUNC(0x020C50BC, bool, this);
  if (mSmoke.mpEmitter) {
    u32 emitter = gabi::ea(mSmoke.mpEmitter.get());
    gabi::store<u32>(emitter + 0x1E4, 0);
    emitter = gabi::ea(mSmoke.mpEmitter.get());
    u32 flags = gabi::load<u32>(emitter + 0x254);
    gabi::store<u32>(emitter + 0x5C, 0xFFFFFFFF);
    gabi::store<u32>(emitter + 0x254, flags | 1);
  }
  mSmoke.mpEmitter = nullptr;
  if (mSparks.mpEmitter) {
    u32 emitter = gabi::ea(mSparks.mpEmitter.get());
    gabi::store<u32>(emitter + 0x1E4, 0);
    emitter = gabi::ea(mSparks.mpEmitter.get());
    u32 flags = gabi::load<u32>(emitter + 0x254);
    gabi::store<u32>(emitter + 0x5C, 0xFFFFFFFF);
    gabi::store<u32>(emitter + 0x254, flags | 1);
  }
  mSparks.mpEmitter = nullptr;
  if (field_0x6F0) {
    u32 player = gabi::load<u32>(play_bomb() + 0x5B34);
    u8 count = gabi::load<u8>(player + 0x690C);
    if (count)
      gabi::store<u8>(player + 0x690C, count - 1);
  }
  se_cannon_fly_stop();
  if (mType == 1) {
    s32 type = mType;
    const char *name = gabi::at<char>(gabi::load<u32>(0x1000A8F4 + type * 8));
    dComIfG_resDelete(&mPhase, name);
  }
  gabi::call(0x025553B8, 0, 0, 0, 0.0f);
  gabi::call(0x0255BA9C, &mPntLight);
  gabi::call(0x0257D1B8, &mPntWind);
  return true;
}
VERIFY(0x020C50BC, &daBomb_c::bombDelete);
BOOL daBomb_Delete(daBomb_c *self) {
  WWHD_FUNC(0x020C51BC, BOOL, self);
  self->bombDelete();
  return TRUE;
}
VERIFY(0x020C51BC, daBomb_Delete);
void daBomb_c::makeFireEffect(cXyz *position, csXyz *angle) {
  WWHD_FUNC(0x020C475C, void, this, position, angle);
  play_bomb();
  play_bomb();
  angle->x = angle->x + 0x4000;
  gabi::Local<cXyz> effectScale;
  effectScale->set(0.3f, 0.3f, 0.3f);
  u32 controller = gabi::load<u32>(play_bomb() + 0x5AB0);
  gabi::call(0x025A866C, controller, 0x200A, position, angle, effectScale.get(),
             255);
  u32 p = play_bomb();
  gabi::Local<cXyz> direction;
  direction->set(0, 1, 0);
  gabi::call(0x025CB374, p + 0x599C, 7, -33, direction.get());
}
VERIFY(0x020C475C, &daBomb_c::makeFireEffect);
void daBomb_c::setFuseEffect() {
  WWHD_FUNC(0x020C584C, void, this);
  if (field_0x77E)
    return;
  f32 x = current.pos.x, y = current.pos.y + 60;
  field_0x77E = 1;
  mFusePos.x = x;
  mFusePos.y = y;
  f32 z = current.pos.z;
  mFusePos.z = z;
  mFusePos2.z = z;
  mFusePos3.x = mFusePos.x;
  mFusePos2.y = y;
  mFusePos3.y = mFusePos.y;
  mFusePos2.x = x;
  mFusePos3.z = mFusePos.z;
  u32 controller = gabi::load<u32>(play_bomb() + 0x5AB0);
  gabi::call(0x025A847C, controller, 1, 0x11, &mFusePos, 0, &scale, 255,
             &mSparks, -1, 0, 0, 0);
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, env, 0, &mFusePos, &tevStr);
  s8 room = current.roomNo;
  controller = gabi::load<u32>(play_bomb() + 0x5AB0);
  gabi::call(0x025A847C, controller, 3, 0x2012, &mFusePos, 0, &scale, 220,
             &mSmoke, room, gabi::ea(this) + 0x1A8, gabi::ea(this) + 0x1A8, 0);
  mSmoke.field_0x0C = &mFusePos2;
  mSmoke.field_0x04 = 20;
  mSmoke.field_0x10 = &mFusePos3;
}
VERIFY(0x020C584C, &daBomb_c::setFuseEffect);
void daBomb_c::eff_water_splash() {
  WWHD_FUNC(0x020C7B24, void, this);
  gabi::Local<cXyz> position;
  position->set(current.pos.x, field_0x558, current.pos.z);
  gabi::call(0x025DAE64, position.get(), 0, 0.5f, 0.75f);
  u32 self = gabi::ea(this);
  u32 polygons[2] = {field_0x562 ? 0 : self + 0x7F0, self + 0x764};
  s32 material = 19;
  for (u32 i = 0; i < 2; i++) {
    u32 polygon = polygons[i];
    if (polygon && gabi::load<u16>(polygon + 2) < 256) {
      u32 p = play_bomb();
      material = gabi::call<s32>(0x024EECAC, p + 0x12A0, polygon);
      break;
    }
  }
  s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
  mDoAud_seStart(0x6918, &eyePos, material, reverb);
}
VERIFY(0x020C7B24, &daBomb_c::eff_water_splash);
void daBomb_c::makeWaterEffect() {
  WWHD_FUNC(0x020C7CC0, void, this);
  if (field_0x77D)
    return;
  s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
  mDoAud_seStart(0x6982, &eyePos, 0, reverb);
  gabi::call(0x025DAE64, &current.pos, 1, 1.0f, 1.0f);
  u32 self = gabi::ea(this);
  u32 co = gabi::load<u32>(self + 0x94C), at = gabi::load<u32>(self + 0x920),
      tg = gabi::load<u32>(self + 0x938);
  gabi::store<u32>(self + 0x94C, co & ~1);
  gabi::store<u32>(self + 0x920, at | 1);
  gabi::store<u32>(self + 0x938, tg & ~1);
  mSph.SetR(200);
  mSph.SetC(&current.pos);
  if ((u32)mMassCounter != gabi::load<u32>(0x101FF558)) {
    u32 p = play_bomb();
    gabi::call(0x0200E240, p + 0x26A4, &mSph);
    p = play_bomb();
    gabi::call(0x02516C14, p + 0x4EF8, &mSph, 3);
    field_0x782 = 1;
    mMassCounter = gabi::load<s32>(0x101FF558);
  }
  field_0x77D = 1;
}
VERIFY(0x020C7CC0, &daBomb_c::makeWaterEffect);
void daBomb_c::set_wind_vec() {
  WWHD_FUNC(0x020C480C, void, this);
  gabi::call(0x028E8E64, &mWindVec, &mWindVec, 0.95f);
  if (gabi::call<f32>(0x028E8DD0, &mWindVec) < 0.1f) {
    mWindVec.y = 0;
    mWindVec.x = 0;
    mWindVec.z = 0;
  }
  if (!mSph.ChkTgHit())
    return;
  void *hit = mSph.GetTgHitObj();
  if (!hit || !(gabi::load<u32>(gabi::ea(hit) + 0x10) & 0x200000))
    return;
  gabi::Local<cXyz> recoil;
  u32 self = gabi::ea(this);
  recoil->set(gabi::load<f32>(self + 0x9E0), gabi::load<f32>(self + 0x9E4),
              gabi::load<f32>(self + 0x9E8));
  f32 magnitude = gabi::call<f32>(0x028E8DD0, recoil.get());
  if (magnitude > 32400) {
    f32 length = gabi::call<f32>(0x028F4384, magnitude);
    gabi::call(0x028E8E64, recoil.get(), recoil.get(), 180.0f / length);
  }
  u32 vt = gabi::load<u32>(gabi::ea(hit) + 0x3C);
  u32 shape = gabi::call_ptr<u32>(gabi::load<u32>(vt + 0x2C), hit);
  gabi::Local<cXyz> normal;
  normal->set(gabi::load<f32>(0x101FFBA8), gabi::load<f32>(0x101FFBAC),
              gabi::load<f32>(0x101FFBB0));
  f32 normalFactor = 1, verticalFactor = 1;
  vt = gabi::load<u32>(shape + 0x1C);
  if (gabi::call_ptr<BOOL>(gabi::load<u32>(vt + 0x9C), shape, &current.pos,
                           normal.get())) {
    gabi::call(0x028E8E64, normal.get(), normal.get(), 50.0f);
    gabi::call<f32>(0x028E8DD0, &mWindVec);
    u32 actor = gabi::call<u32>(0x02515BBC, self + 0x9B4);
    if (actor && gabi::load<s16>(actor + 0xE) == 0xA8) {
      s32 angle = gabi::call<s32>(0x020195B0, (f32)normal->x, (f32)normal->z);
      s16 yaw = gabi::load<s16>(actor + 0x32A);
      u32 lookup = ((u16)(yaw - angle) >> 3) * 8;
      f32 cosine = gabi::load<f32>(0x104A44FC + lookup);
      if (cosine > 0) {
        normalFactor = (cosine + cosine) + 1;
        verticalFactor = gabi::fmadds(cosine, 0.3f, 1);
      }
    }
  }
  f32 difference = 0.01f - magnitude;
  f32 recoilWeight = difference >= 0 ? 0 : 0.9f,
      normalWeight = difference >= 0 ? 1 : 0.1f;
  gabi::Local<cXyz> scaledRecoil, scaledNormal, weightedNormal, sum;
  gabi::call(0x0201AE48, recoil.get(), scaledRecoil.get(), recoilWeight);
  gabi::call(0x0201AE48, normal.get(), scaledNormal.get(), normalWeight);
  gabi::call(0x0201AE48, scaledNormal.get(), weightedNormal.get(),
             normalFactor);
  gabi::call(0x0201AD78, scaledRecoil.get(), sum.get(), weightedNormal.get());
  f32 y = sum->y, x = sum->x, z = sum->z;
  mWindVec.x = x;
  mWindVec.z = z;
  mWindVec.y = y;
  if (std::fabs(y) < 5) {
    f32 lift = (100.0f * normalWeight) * verticalFactor;
    lift = gabi::fmadds(140.0f, recoilWeight, lift);
    mWindVec.y = y + lift;
  }
}
VERIFY(0x020C480C, &daBomb_c::set_wind_vec);
void bomb_sinit() {
  WWHD_FUNC(0x020C7FAC, void, 0);
  for (u32 i = 0; i < 4; i++)
    gabi::store<u32>(0x10462688 + i * 4, 0);
  __register_global_object(0x101922D0);
  gabi::store<f32>(0x10462674, -3.1415927f);
  gabi::store<f32>(0x10462678, 3.1415927f);
  gabi::call(0x028ED6F8, 0x10462684);
  __register_global_object(0x101922DC);
  gabi::call(0x028EAB2C, 0x10462685);
  __register_global_object(0x101922E8);
  gabi::store<f32>(0x101922AC, 30);
  gabi::store<f32>(0x10462680, 66);
  gabi::store<f32>(0x1046267C, 36);
}
VERIFY(0x020C7FAC, bomb_sinit);
void bomb_generic_dtor(void *self, s32 flags) {
  WWHD_FUNC(0x020C8070, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x020C8070, bomb_generic_dtor);
void bomb_smoke_execute(void *self, void *emitter) {
  WWHD_FUNC(0x020C8084, void, self, emitter);
}
VERIFY(0x020C8084, bomb_smoke_execute);
void bomb_smoke_draw(void *self, void *emitter) {
  WWHD_FUNC(0x020C8088, void, self, emitter);
}
VERIFY(0x020C8088, bomb_smoke_draw);
void bomb_smoke_drawAfter(void *self, void *emitter) {
  WWHD_FUNC(0x020C808C, void, self, emitter);
}
VERIFY(0x020C808C, bomb_smoke_drawAfter);
void bomb_smoke_cleanup(void *self, void *emitter) {
  WWHD_FUNC(0x020C8090, void, self, emitter);
}
VERIFY(0x020C8090, bomb_smoke_cleanup);
void bomb_smoke_setup(daBomb_fuseSmokeEcallBack *self, void *emitter,
                      cXyz *pos) {
  WWHD_FUNC(0x020C8094, void, self, emitter, pos);
  self->mpPos = pos;
  self->mpEmitter = emitter;
}
VERIFY(0x020C8094, bomb_smoke_setup);
void bomb_sparks_draw(void *self, void *emitter) {
  WWHD_FUNC(0x020C80A0, void, self, emitter);
}
VERIFY(0x020C80A0, bomb_sparks_draw);
void bomb_sparks_setup(daBomb_fuseSparksEcallBack *self, void *emitter,
                       cXyz *pos) {
  WWHD_FUNC(0x020C80A4, void, self, emitter, pos);
  self->mpPos = pos;
  self->mpEmitter = emitter;
}
VERIFY(0x020C80A4, bomb_sparks_setup);
void bomb_actor_dtor(daBomb_c *self, s32 flags) {
  WWHD_FUNC(0x020C80B0, void, self, flags);
  if (!self)
    return;
  u32 receiver = gabi::ea(self);
  gabi::call(0x02515AE8, &self->mSph, 2);
  gabi::call(0x02515860, &self->mStts, 2);
  gabi::store<u32>(receiver + 0x8A0, 0x1000A64C);
  gabi::store<u32>(receiver + 0x8C0, 0x1000A66C);
  gabi::store<u32>(receiver + 0x8CC, 0x1000A62C);
  gabi::call(0x02008DAC, receiver + 0x880, 0);
  gabi::call(0x02018034, receiver + 0x854, 2);
  gabi::store<u32>(receiver + 0x69C, 0x1000A70C);
  gabi::store<u32>(receiver + 0x690, 0x1000A71C);
  gabi::call(0x024EFD9C, &self->mAcch, 0);
  gabi::call(0x02082DDC, receiver + 0x5CC, 2);
  gabi::call(0x027F3628, receiver + 0x45C, 0);
  gabi::call(0x027F3628, receiver + 0x3D0, 0);
  gabi::call(0x025D50BC, self, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, self);
}
VERIFY(0x020C80B0, bomb_actor_dtor);
void bomb_acch_virtual(void *self) { WWHD_FUNC(0x020C81A0, void, self); }
VERIFY(0x020C81A0, bomb_acch_virtual);
static void initBombBck(u32 receiver) {
  gabi::call(0x027F2BC0, receiver, 0);
  gabi::store<u32>(receiver + 0x10, 0x1016E54C);
  gabi::call(0x027DA984, receiver + 0x14);
  gabi::store<u32>(receiver + 0x84, 0);
  gabi::store<u32>(receiver + 0x88, 0);
  gabi::store<u32>(receiver + 0x80, 0);
  gabi::store<u32>(receiver + 0x58, 0);
  gabi::store<u32>(receiver + 0x10, 0x1000A604);
  gabi::store<u32>(receiver + 0x7C, 0);
  gabi::store<u32>(receiver + 0x48, 0x1016D820);
}
daBomb_c *bomb_ctor(daBomb_c *self) {
  WWHD_FUNC(0x020C5660, daBomb_c *, self);
  if (!self) {
    self = gabi::call<daBomb_c *>(0x0273AD10, 0xB50);
    if (!self)
      return nullptr;
  }
  gabi::call(0x025D4ED0, self);
  u32 receiver = gabi::ea(self);
  gabi::store<u32>(receiver + 0xB4, 0x1000A72C);
  initBombBck(receiver + 0x3C0);
  initBombBck(receiver + 0x44C);
  gabi::call(0x025E80D0, &self->mBrk0);
  gabi::call(0x025E80D0, &self->mBrk1);
  gabi::call(0x02080404, receiver + 0x5CC);
  gabi::call(0x024F0474, &self->mAcch);
  gabi::store<u32>(receiver + 0x68C, 0x1000A6FC);
  gabi::store<u32>(receiver + 0x690, 0x1000A71C);
  gabi::store<u32>(receiver + 0x69C, 0x1000A70C);
  gabi::store<u8>(receiver + 0x698, 1);
  gabi::call(0x024EFE94, &self->mCir);
  gabi::call(0x02008E0C, receiver + 0x880);
  for (u32 offset : {0x8C5, 0x8CA, 0x8C7, 0x8C9, 0x8C8})
    gabi::store<u8>(receiver + offset, 0);
  gabi::store<u32>(receiver + 0x8C0, 0x1000A6EC);
  gabi::store<u32>(receiver + 0x8D0, 4);
  gabi::store<u32>(receiver + 0x8CC, 0x1000A6DC);
  gabi::store<u8>(receiver + 0x8C4, 1);
  gabi::store<u32>(receiver + 0x884, receiver + 0x8CC);
  gabi::store<u32>(receiver + 0x8A0, 0x1000A6CC);
  gabi::store<u32>(receiver + 0x890, 0x1000A6BC);
  gabi::store<u8>(receiver + 0x8C6, 0);
  gabi::store<u32>(receiver + 0x880, receiver + 0x8C0);
  gabi::call(0x0200BD2C, &self->mStts);
  gabi::call(0x02515DA0, receiver + 0x900);
  gabi::store<u32>(receiver + 0x8FC, 0x1004AE88);
  gabi::store<u32>(receiver + 0x900, 0x1004AEC0);
  gabi::call(0x025166F0, &self->mSph);
  gabi::store<u32>(receiver + 0xA64, 0x1000A94C);
  gabi::store<f32>(receiver + 0xAC8, 1);
  gabi::store<u32>(receiver + 0xA4C, 0x1000A90C);
  return self;
}
VERIFY(0x020C5660, bomb_ctor);
void bomb_init_mtx(daBomb_c *self) {
  WWHD_FUNC(0x020C6404, void, self);
  self->set_mtx();
}
VERIFY(0x020C6404, bomb_init_mtx);
void daBomb_c::create_init() {
  WWHD_FUNC(0x020C6408, void, this);
  mCir.SetWall(30, 30);
  mAcch.Set(&current.pos, &old.pos, this, 1, &mCir, &speed, &current.angle,
            &shape_angle);
  u32 flags = mAcch.m_flags;
  u32 self = gabi::ea(this);
  u32 model = gabi::ea(mpModel.get());
  field_0x558 = -1000000000.0f;
  field_0x562 = 0;
  mAcch.m_flags = (flags & ~0x408) | 0x2000;
  mbWaterIn = 0;
  mAcch.m_roof_crr_height = 50;
  field_0x55C = -1000000000.0f;
  field_0x560 = 0;
  field_0x554 = -1000000000.0f;
  gabi::store<u32>(self + 0x348, model ? model + 0xC8 : 0);
  gabi::call(0x02515F14, &mStts, 200, 255, this);
  gabi::call(0x0251677C, &mSph, 0x10192270);
  gabi::store<u32>(self + 0x964, gabi::ea(&mStts));
  if (chk_state(8))
    gabi::store<u8>(self + 0x934, 2);
  play_bomb();
  gravity = -2.9f;
  maxFallSpeed = -100;
  mRestTime = 150;
  mInitialState = prm_get_state();
  if (!chk_state(4))
    gabi::store<u32>(self + 0x39C, gabi::load<u32>(self + 0x39C) | 0x10);
  field_0x6F4 = 0;
  field_0x781 = 0;
  field_0x77D = 0;
  field_0x780 = 0;
  field_0x77C = 0;
  field_0x77F = 0;
  field_0x782 = 0;
  field_0x77E = 0;
  field_0x784 = 0;
  mWindVec.x = gabi::load<f32>(0x101FFBA8);
  mWindVec.y = gabi::load<f32>(0x101FFBAC);
  mWindVec.z = gabi::load<f32>(0x101FFBB0);
  mMassCounter = gabi::load<u32>(0x101FF558) - 1;
  if (chk_attrState(this, 2)) {
    scale.y = 0;
    scale.x = 0;
    scale.z = 0;
  }
  if (chk_attrState(this, 4))
    setFuseEffect();
  if (chk_attrState(this, 8))
    bgCrrPos();
  if (chk_state(3)) {
    field_0x6F0 = 1;
    field_0x7C8 = 2;
  } else
    field_0x7C8 = 0;
  if (chk_attrState(this, 1))
    procExplode_init();
  else
    procWait_init();
  f32 radius = gabi::load<f32>(0x1046267C);
  gabi::call(0x025D672C, this, -radius, 0.0f, -radius);
  radius = gabi::load<f32>(0x1046267C);
  f32 height = gabi::load<f32>(0x10462680);
  gabi::call(0x025D673C, this, radius, height, radius);
  gabi::store<f32>(self + 0x364, 10);
  bomb_init_mtx(this);
}
VERIFY(0x020C6408, &daBomb_c::create_init);
cPhs_State daBomb_c::create() {
  WWHD_FUNC(0x020C6724, cPhs_State, this);
  s32 state = prm_get_state();
  mType = (state == 5 || state == 6) ? 1 : state == 4 ? 2 : 0;
  u32 self = gabi::ea(this), condition = gabi::load<u32>(self + 0x2E4);
  if (!(condition & 8)) {
    if (this) {
      bomb_ctor(this);
      condition = gabi::load<u32>(self + 0x2E4);
    }
    gabi::store<u32>(self + 0x2E4, condition | 8);
  }
  cPhs_State status = cPhs_COMPLEATE_e;
  if (mType == 1) {
    const char *name = gabi::at<char>(gabi::load<u32>(0x1000A8F4 + mType * 8));
    status = dComIfG_resLoad(&mPhase, name);
  }
  if (status == cPhs_COMPLEATE_e) {
    u32 heap = gabi::load<u32>(0x1000A8F8 + mType * 8);
    if (gabi::call<BOOL>(0x025D63E8, this, 0x020C565C, heap))
      create_init();
    else
      status = cPhs_ERROR_e;
  }
  return status;
}
VERIFY(0x020C6724, &daBomb_c::create);
cPhs_State daBomb_Create(daBomb_c *self) {
  WWHD_FUNC(0x020C685C, cPhs_State, self);
  return self->create();
}
VERIFY(0x020C685C, daBomb_Create);
BOOL daBomb_createHeap(daBomb_c *self) {
  WWHD_FUNC(0x020C565C, BOOL, self);
  return self->createHeap();
}
VERIFY(0x020C565C, daBomb_createHeap);

void daBomb_c::eff_explode_normal(const csXyz *angle) {
  WWHD_FUNC(0x020C5D54, void, this, angle);
  u32 c = gabi::load<u32>(play_bomb() + 0x5AB0);
  gabi::call(0x025A847C, c, 1, 0xB, &current.pos, angle, &scale, 255, 0, -1, 0,
             0, 0);
  c = gabi::load<u32>(play_bomb() + 0x5AB0);
  gabi::call(0x025A866C, c, 0x2009, &current.pos, 0, &scale, 255);
  c = gabi::load<u32>(play_bomb() + 0x5AB0);
  gabi::call(0x025A866C, c, 0x200A, &current.pos, 0, &scale, 255);
  c = gabi::load<u32>(play_bomb() + 0x5AB0);
  gabi::call(0x025A847C, c, 3, 0x2008, &current.pos, 0, &scale, 255, 0, -1, 0,
             0, 0);
}
VERIFY(0x020C5D54, &daBomb_c::eff_explode_normal);
void daBomb_c::eff_explode_cheap(const csXyz *angle) {
  WWHD_FUNC(0x020C5BFC, void, this, angle);
  u32 c = gabi::load<u32>(play_bomb() + 0x5AB0);
  u32 emitter = gabi::call<u32>(0x025A847C, c, 1, 0xB, &current.pos, angle,
                                &scale, 255, 0, -1, 0, 0, 0);
  if (emitter) {
    gabi::store<f32>(emitter + 0x240, 1);
    gabi::store<f32>(emitter + 0x23C, 0.67f);
    gabi::store<f32>(emitter + 0x238, 0.5f);
    gabi::store<s16>(emitter + 0x60, 12);
  }
  c = gabi::load<u32>(play_bomb() + 0x5AB0);
  gabi::call(0x025A866C, c, 0x232A, &current.pos, 0, &scale, 255);
  c = gabi::load<u32>(play_bomb() + 0x5AB0);
  emitter =
      gabi::call<u32>(0x025A866C, c, 0x200A, &current.pos, 0, &scale, 255);
  if (emitter)
    gabi::store<s16>(emitter + 0x60, 70);
  c = gabi::load<u32>(play_bomb() + 0x5AB0);
  emitter = gabi::call<u32>(0x025A847C, c, 3, 0x2008, &current.pos, 0, &scale,
                            255, 0, -1, 0, 0, 0);
  if (emitter) {
    gabi::store<f32>(emitter + 0x6C, 25);
    gabi::store<f32>(emitter + 0x70, 35);
    gabi::store<s16>(emitter + 0x60, 70);
  }
}
VERIFY(0x020C5BFC, &daBomb_c::eff_explode_cheap);
void daBomb_c::eff_explode() {
  WWHD_FUNC(0x020C5E4C, void, this);
  s8 id = gabi::load<s8>(play_bomb() + 0x5B30);
  u32 camera = gabi::load<u32>(play_bomb() + id * 0x34 + 0x5AF8);
  gabi::Local<csXyz> angle;
  angle->x = -gabi::load<s16>(camera + 0x234);
  angle->z = 0;
  angle->y = gabi::load<s16>(camera + 0x236) + 0x8000;
  if (gabi::call<bool>(0x020CB9A0, this))
    eff_explode_cheap(angle.get());
  else
    eff_explode_normal(angle.get());
}
VERIFY(0x020C5E4C, &daBomb_c::eff_explode);

bool daBomb_c::procExplode() {
  WWHD_FUNC(0x020C74A0, bool, this);
  u32 p = play_bomb();
  f32 intensity = field_0x778;
  mPntLight.mPower = 1500 * intensity;
  gabi::store<f32>(gabi::ea(this) + 0xAE8, intensity);
  u32 camera = gabi::load<u32>(p + 0x5AF8);
  f32 distance = gabi::call<f32>(0x028E8DE8, &current.pos, camera + 0xDC);
  distance = gabi::call<f32>(0x028F4384, distance);
  f32 factor = 0;
  if (distance < 1500)
    factor = 1 - distance / 1500;
  gabi::call(0x025548F0, 200, 180, 100, (f32)field_0x778 * factor);
  gabi::call(0x02554B18, 180, 160, 60, (f32)field_0x778 * factor);
  gabi::call(0x02554C2C, 255, 225, 120, (f32)field_0x778 * factor);
  switch (field_0x774) {
  case 0:
    gabi::call<f32>(0x0200ECD4, &field_0x778, 1.0f, 0.5f, 0.4f, 0.01f);
    if (!(field_0x778 < 0.99f))
      field_0x774 = field_0x774 + 1;
    break;
  case 1:
    gabi::call<f32>(0x0200ECD4, &field_0x778, 0.0f, 0.05f, 0.04f, 0.001f);
    if (!(field_0x778 > 0.01f))
      field_0x774 = field_0x774 + 1;
    break;
  }
  if (field_0x6FE) {
    field_0x6FE = field_0x6FE - 1;
    mSph.SetC(&current.pos);
    if (mMassCounter != gabi::load<u32>(0x101FF558)) {
      p = play_bomb();
      gabi::call(0x0200E240, p + 0x26A4, &mSph);
      p = play_bomb();
      gabi::call(0x02516C14, p + 0x4EF8, &mSph, 3);
      field_0x782 = 1;
      mMassCounter = gabi::load<u32>(0x101FF558);
    }
  } else if (field_0x774 > 1)
    field_0x781 = 1;
  return true;
}
VERIFY(0x020C74A0, &daBomb_c::procExplode);

BOOL daBomb_c::procExplode_init() {
  WWHD_FUNC(0x020C5EF8, BOOL, this);
  u32 self = gabi::ea(this);
  s8 id = gabi::load<s8>(play_bomb() + 0x5B30);
  u32 camera = gabi::load<u32>(play_bomb() + id * 0x34 + 0x5AF8);
  gabi::call(0x025052BC, camera + 0x248, gabi::load<u32>(self + 4));
  u32 water = 0;
  if (chk_state(8)) {
    gabi::Local<be<f32>> height;
    water = gabi::call<u32>(0x025D9F70, &current.pos, height.get());
    if (water) {
      gabi::Local<bomb_ground_l> ground;
      u32 g = gabi::ea(ground.get());
      gabi::call(0x02008E0C, g);
      gabi::store<u8>(g + 0x49, 0);
      gabi::store<u8>(g + 0x47, 0);
      gabi::store<f32>(g + 0x24, current.pos.x);
      gabi::store<f32>(g + 0x28, current.pos.y + 1);
      gabi::store<f32>(g + 0x2C, current.pos.z);
      gabi::store<u32>(g + 0x20, 0x1000A68C);
      gabi::store<u32>(g + 0x4C, 0x1000A69C);
      gabi::store<u8>(g + 0x45, 0);
      gabi::store<u8>(g + 0x4A, 0);
      gabi::store<u32>(g + 4, g + 0x4C);
      gabi::store<u32>(g, g + 0x40);
      gabi::store<u32>(g + 0x10, 0x1000A67C);
      gabi::store<u8>(g + 0x48, 0);
      gabi::store<u32>(g + 0x40, 0x1000A6AC);
      gabi::store<u32>(g + 0x50, 1);
      gabi::store<u8>(g + 0x46, 0);
      gabi::store<u8>(g + 0x44, 1);
      u32 p = play_bomb();
      f32 cross = gabi::call<f32>(0x02008974, p + 0x12A0, g);
      if (cross > *height.get() || current.pos.y > *height.get() + 50)
        water = 0;
      gabi::store<u32>(g + 0x20, 0x1000A64C);
      gabi::store<u32>(g + 0x40, 0x1000A66C);
      gabi::store<u32>(g + 0x4C, 0x1000A62C);
      gabi::call(0x02008DAC, g, 0);
    }
  }
  if (!water)
    eff_explode();
  else
    gabi::call(0x025DAE64, &current.pos, 1, 1.0f, 1.0f);
  for (u32 offset : {0xA60u, 0xA6Cu}) {
    u32 emitter = gabi::load<u32>(self + offset);
    if (emitter) {
      gabi::store<u32>(emitter + 0x1E4, 0);
      emitter = gabi::load<u32>(self + offset);
      u32 flags = gabi::load<u32>(emitter + 0x254);
      gabi::store<s32>(emitter + 0x5C, -1);
      gabi::store<u32>(emitter + 0x254, flags | 1);
    }
    gabi::store<u32>(self + offset, 0);
  }
  gabi::store<u16>(self + 0xAB6, 200);
  gabi::store<u16>(self + 0xAB4, 200);
  gabi::store<f32>(self + 0xAA8, current.pos.x);
  gabi::store<u16>(self + 0xAB8, 160);
  gabi::store<f32>(self + 0xABC, 600);
  gabi::store<f32>(self + 0xAAC, current.pos.y + 100);
  gabi::store<f32>(self + 0xAC0, 100);
  gabi::store<f32>(self + 0xAB0, current.pos.z);
  gabi::call(0x0255B9C8, &mPntLight);
  gabi::store<f32>(self + 0xADC, 1);
  gabi::store<f32>(self + 0xAE4, 500);
  gabi::store<f32>(self + 0xAE8, 0.5f);
  gabi::store<f32>(self + 0xAE0, 0);
  gabi::store<f32>(self + 0xAD8, 0);
  for (u32 i = 0; i < 3; i++)
    gabi::store<u32>(self + 0xACC + i * 4,
                     gabi::load<u32>(self + 0x314 + i * 4));
  gabi::store<f32>(self + 0xAEC, 0);
  gabi::call(0x0257DC90, &mPntWind);
  speedF = 0;
  gabi::store<u16>(self + 0x3AC, 0);
  field_0x774 = 0;
  field_0x778 = 0;
  mFunc[1] = 0x020C74A0;
  gabi::store<u16>(self + 0x3AE, 65535);
  for (u32 i = 0; i < 3; i++)
    gabi::store<u32>(self + 0x33C + i * 4, gabi::load<u32>(0x101FFBA8 + i * 4));
  gravity = 0;
  if (!chk_state(8))
    change_state(0);
  gabi::store<u32>(self + 0x39C, gabi::load<u32>(self + 0x39C) & ~0x10);
  if (field_0x6F0) {
    u32 player = gabi::load<u32>(play_bomb() + 0x5B34);
    u8 count = gabi::load<u8>(player + 0x690C);
    if (count)
      gabi::store<u8>(player + 0x690C, count - 1);
    field_0x6F0 = 0;
  }
  gabi::store<u32>(self + 0x920, gabi::load<u32>(self + 0x920) | 1);
  gabi::store<u32>(self + 0x938, gabi::load<u32>(self + 0x938) & ~1);
  gabi::store<u32>(self + 0x94C, gabi::load<u32>(self + 0x94C) & ~1);
  mSph.SetR(200);
  mSph.SetC(&current.pos);
  if (mMassCounter != gabi::load<u32>(0x101FF558)) {
    u32 p = play_bomb();
    gabi::call(0x0200E240, p + 0x26A4, &mSph);
    p = play_bomb();
    gabi::call(0x02516C14, p + 0x4EF8, &mSph, 3);
    field_0x782 = 1;
    mMassCounter = gabi::load<u32>(0x101FF558);
  }
  s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
  gabi::call(0x025E1A40, water ? 0x6982 : 0x6901, self + 0x37C, 0, reverb);
  gabi::call(0x025D9D24, this);
  gabi::Local<cXyz> position;
  position->x = current.pos.x;
  position->y = current.pos.y;
  position->z = current.pos.z;
  gabi::call(0x0255F458, position.get(), 255, gabi::load<u32>(self + 4), 10);
  mRestTime = 0;
  field_0x6FE = 2;
  u32 p = play_bomb();
  position->x = 0;
  position->y = 1;
  position->z = 0;
  gabi::call(0x025CB374, p + 0x599C, 7, -33, position.get());
  se_cannon_fly_stop();
  return TRUE;
}
VERIFY(0x020C5EF8, &daBomb_c::procExplode_init);

void daBomb_fuseSparksEcallBack::execute(void *emitter) {
  WWHD_FUNC(0x020C6C04, void, this, emitter);
  u32 e = gabi::ea(emitter);
  f32 y = mpPos->y, z = mpPos->z, x = mpPos->x;
  u8 type = gabi::load<u8>(e + 0x262);
  u32 link = gabi::load<u32>(e + 0x1AC);
  gabi::store<f32>(e + 0x234, z);
  gabi::store<f32>(e + 0x22C, x);
  gabi::store<f32>(e + 0x230, y);
  if (type >= 7)
    gabi::store<f32>(e + 0x230, -gabi::load<f32>(e + 0x230));
  while (link) {
    u32 next = gabi::load<u32>(link + 0xC), particle = gabi::load<u32>(link);
    gabi::store<f32>(particle + 0x18, z);
    gabi::store<f32>(particle + 0x14, y);
    gabi::store<f32>(particle + 0x10, x);
    link = next;
  }
}
VERIFY(0x020C6C04, &daBomb_fuseSparksEcallBack::execute);

// Retain the original arithmetic NaN payload when multiple inputs are NaNs.
static f32 bombSmokeFma(f32 a, f32 c, f32 b) {
  if (std::isnan(c))
    return c;
  if (std::isnan(a))
    return a;
  if (std::isnan(b))
    return b;
  return gabi::fmadds(a, c, b);
}
static f32 bombSmokeSub(f32 a, f32 b) {
  if (std::isnan(b))
    return b;
  if (std::isnan(a))
    return a;
  return a - b;
}
void daBomb_fuseSmokeEcallBack::executeAfter(void *emitter) {
  WWHD_FUNC(0x020C6860, void, this, emitter);
  u32 e = gabi::ea(emitter);
  f32 z = mpPos->z, x = mpPos->x, y = mpPos->y;
  f32 px = field_0x0C->x, py = field_0x0C->y, pz = field_0x0C->z;
  u8 type = gabi::load<u8>(e + 0x262);
  gabi::store<f32>(e + 0x22C, x);
  gabi::store<f32>(e + 0x230, y);
  gabi::store<f32>(e + 0x234, z);
  if (type >= 7)
    gabi::store<f32>(e + 0x230, -gabi::load<f32>(e + 0x230));
  f32 distance = gabi::call<f32>(0x028E8DE8, mpPos.get(), field_0x0C.get());
  distance = gabi::call<f32>(0x028F4384, distance);
  s16 life = gabi::ftoi(bombSmokeFma(20 - distance, 0.5f, 10));
  if (life < 10)
    life = 10;
  gabi::store<s16>(e + 0x60, life);
  f32 dx = bombSmokeSub(x, px) * 0.5f, dy = bombSmokeSub(y, py) * 0.5f,
      dz = bombSmokeSub(z, pz) * 0.5f;
  f32 tx = (px - field_0x10->x) * 0.5f, ty = (py - field_0x10->y) * 0.5f,
      tz = (pz - field_0x10->z) * 0.5f;
  distance = gabi::call<f32>(0x028E8DE8, mpPos.get(), field_0x0C.get());
  distance = gabi::call<f32>(0x028F4384, distance);
  f32 count = distance * 0.1f;
  if (!(count <= 1)) {
    f32 step = 1 / count;
    s16 delta = gabi::ftoi((f32)((s32)field_0x04 - (s32)life) * step);
    s16 lifetime = life + delta;
    for (f32 t = step; t < 1; t = t + step, lifetime = lifetime + delta) {
      f32 square = t * t, cube = square * t, three = 3 * square;
      f32 b = bombSmokeFma(-2, cube, three), a = (cube + cube) - three + 1,
          c = (cube - (square + square)) + t, d = cube - square;
      f32 ox = bombSmokeFma(a, px, b * x), oy = bombSmokeFma(a, py, b * y),
          oz = bombSmokeFma(a, pz, b * z);
      ox = bombSmokeFma(c, tx, ox);
      oy = bombSmokeFma(c, ty, oy);
      oz = bombSmokeFma(c, tz, oz);
      ox = bombSmokeFma(d, dx, ox);
      oy = bombSmokeFma(d, dy, oy);
      oz = bombSmokeFma(d, dz, oz);
      gabi::store<s16>(e + 0x60, lifetime);
      u32 particle = gabi::call<u32>(0x0281DCB8, e);
      if (particle) {
        gabi::store<f32>(particle + 0x10, ox);
        gabi::store<f32>(particle + 0x14, oy);
        gabi::store<f32>(particle + 0x18, oz);
      }
    }
  }
  field_0x04 = life;
}
VERIFY(0x020C6860, &daBomb_fuseSmokeEcallBack::executeAfter);

struct bomb_safe_l {
  be<u32> text, vt;
};
static u32 bombGetResource(daBomb_c *self, u32 index) {
  gabi::Local<bomb_safe_l> name;
  name->text = gabi::load<u32>(0x1000A8F4 + (s32)self->mType * 8);
  name->vt = 0x1000A5EC;
  return gabi::call<u32>(0x026066C4, gabi::load<u32>(0x101F4F28), name.get(),
                         index);
}
static void bombAssert(u32 value, u32 line, u32 message) {
  if (!value)
    gabi::call(0x0273AA24, 0x1000A7C0, line, message);
}
BOOL daBomb_c::createHeap() {
  WWHD_FUNC(0x020C51E0, BOOL, this);
  if (mType == 1) {
    u32 data = bombGetResource(this, 12);
    bombAssert(data, 0x9BC, 0x1000A7D0);
    mpModel = gabi::at<J3DModel>(
        gabi::call<u32>(0x025E38E0, data, 0x80000, 0x11000022));
    u32 anm = bombGetResource(this, 8);
    bombAssert(anm, 0x9C5, 0x1000A7E4);
    u32 a =
        gabi::call<u32>(0x025E8508, &mBck0, data, anm, 1, 0, 0, -1, 0, 1.0f);
    anm = bombGetResource(this, 7);
    bombAssert(anm, 0x9CB, 0x1000A7F8);
    u32 b =
        gabi::call<u32>(0x025E8508, &mBck1, data, anm, 1, 0, 0, -1, 0, 1.0f);
    anm = bombGetResource(this, 17);
    bombAssert(anm, 0x9D2, 0x1000A80C);
    u32 c =
        gabi::call<u32>(0x025E8154, &mBrk0, data, anm, 1, 0, 0, -1, 0, 0, 1.0f);
    anm = bombGetResource(this, 16);
    bombAssert(anm, 0x9D8, 0x1000A820);
    u32 d =
        gabi::call<u32>(0x025E8154, &mBrk1, data, anm, 1, 0, 0, -1, 0, 0, 1.0f);
    return mpModel && a && b && c && d;
  }
  u32 data = bombGetResource(this, 60);
  bombAssert(data, 0x9E5, 0x1000A7D0);
  mpModel = gabi::at<J3DModel>(
      gabi::call<u32>(0x025E38E0, data, 0x80000, 0x11000002));
  if (!mpModel)
    return FALSE;
  u32 anm = bombGetResource(this, 11);
  if (!gabi::call<u32>(0x025E8508, &mBck0, data, anm, 0, 2, 0, -1, 0, 1.0f))
    return FALSE;
  gabi::Local<bomb_safe_l> name;
  name->text = 0x1000A7B8;
  name->vt = 0x1000A5EC;
  u32 p = play_bomb();
  gabi::Local<bomb_safe_l> stage;
  stage->text = p + 0x5134;
  stage->vt = 0x1000A5EC;
  u32 vt = name->vt;
  gabi::call_ptr(gabi::load<u32>(vt + 0x14), name.get());
  vt = name->vt;
  gabi::call_ptr(gabi::load<u32>(vt + 0x14), name.get());
  vt = stage->vt;
  u32 text = name->text;
  gabi::call_ptr(gabi::load<u32>(vt + 0x14), stage.get());
  u32 other = stage->text;
  bool equal = text == other;
  if (!equal) {
    equal = true;
    for (u32 i = 0; i < 0x40001; i++) {
      u8 a = gabi::load<u8>(text + i), b = gabi::load<u8>(other + i);
      if (a != b) {
        equal = false;
        break;
      }
      if (!a)
        break;
    }
  }
  if (equal && mType == 0)
    gabi::call(0x0207FD38, gabi::ea(this) + 0x5CC, 0);
  return TRUE;
}
VERIFY(0x020C51E0, &daBomb_c::createHeap);

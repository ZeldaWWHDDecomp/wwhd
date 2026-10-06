#include "d/actor/d_a_bomb2.h"
namespace daBomb2 {
static void invokeBomb2(u32 obj, u32 d) {
  s32 adjust = gabi::load<s16>(d), index = gabi::load<s16>(d + 2);
  u32 fn = gabi::load<u32>(d + 4);
  obj += adjust;
  if (index >= 0) {
    u32 vt = gabi::load<u32>(obj + (s16)fn);
    fn = gabi::load<u32>(vt + index * 8 + 4);
  }
  gabi::call(fn, obj);
}
static u32 play2() { return gabi::ea(dComIfGp_get()); }
void Act_c::off_carry() {
  WWHD_FUNC(0x020C8C58, void, this);
  u32 s = gabi::ea(this) + 0x39C;
  gabi::store<u32>(s, gabi::load<u32>(s) & ~0x10);
}
VERIFY(0x020C8C58, &Act_c::off_carry);
void Act_c::on_carry() {
  WWHD_FUNC(0x020C8C68, void, this);
  u32 s = gabi::ea(this) + 0x39C;
  gabi::store<u32>(s, gabi::load<u32>(s) | 0x10);
}
VERIFY(0x020C8C68, &Act_c::on_carry);
u8 Act_c::chk_water_in() {
  WWHD_FUNC(0x020CA064, u8, this);
  return mbWaterIn;
}
VERIFY(0x020CA064, &Act_c::chk_water_in);
bool Act_c::chk_lava_in() {
  WWHD_FUNC(0x020CA790, bool, this);
  return field_0x51C != -1000000000.0f && current.pos.y < field_0x51C;
}
VERIFY(0x020CA790, &Act_c::chk_lava_in);
void Act_c::set_nut_exp_interval() {
  WWHD_FUNC(0x020CA610, void, this);
  if (mBombTimer > 30) {
    mBombTimer = 30;
    mBrk0.mFrameCtrl.setFrame(105);
    mBck0.mFrameCtrl.setFrame(105);
  }
}
VERIFY(0x020CA610, &Act_c::set_nut_exp_interval);
void Act_c::mode_wait_init() {
  WWHD_FUNC(0x020CA638, void, this);
  u32 s = gabi::ea(this) + 0x78C;
  u32 flags = gabi::load<u32>(s);
  mState = 0;
  gravity = -2.9f;
  gabi::store<u32>(s, flags | 1);
}
VERIFY(0x020CA638, &Act_c::mode_wait_init);
void Act_c::vib_init() {
  WWHD_FUNC(0x020C8EBC, void, this);
  field_0x7A8 = 0;
  field_0x79C = 0;
  field_0x784 = 0;
  field_0x788 = 0;
  field_0x790 = 0;
  field_0x798 = 0;
  field_0x78C = 0;
  field_0x7A0 = 0;
  field_0x794 = 0;
  field_0x7A4 = 0;
}
VERIFY(0x020C8EBC, &Act_c::vib_init);
void Act_c::tensor_init() {
  WWHD_FUNC(0x020C8EF4, void, this);
  gabi::call(0x028E9098, gabi::ea(this) + 0x950);
  vib_init();
}
VERIFY(0x020C8EF4, &Act_c::tensor_init);
void Act_c::start_carry() {
  WWHD_FUNC(0x020CA694, void, this);
  mode_carry_init();
}
VERIFY(0x020CA694, &Act_c::start_carry);
u32 Act_c::create_heap() {
  WWHD_FUNC(0x020C8B50, u32, this);
  return gabi::call<u32>(0x020C89BC, this);
}
VERIFY(0x020C8B50, &Act_c::create_heap);
void Act_c::start_explode_interval() {
  WWHD_FUNC(0x020CA65C, void, this);
  eff_fuse_start();
  set_nut_exp_interval();
  mode_wait_init();
}
VERIFY(0x020CA65C, &Act_c::start_explode_interval);
bool Act_c::chk_exp_bg() {
  WWHD_FUNC(0x020CAC84, bool, this);
  return chk_exp_bg_nut();
}
VERIFY(0x020CAC84, &Act_c::chk_exp_bg);
bool Act_c::chk_sink_bg() {
  WWHD_FUNC(0x020CAD38, bool, this);
  return chk_sink_bg_nut();
}
VERIFY(0x020CAD38, &Act_c::chk_sink_bg);
u8 Act_c::chk_exp_post() {
  WWHD_FUNC(0x020CAD94, u8, this);
  return gabi::call<u8>(0x020CAC84, this);
}
VERIFY(0x020CAD94, &Act_c::chk_exp_post);
u8 Act_c::chk_sink_post() {
  WWHD_FUNC(0x020CAD98, u8, this);
  return gabi::call<u8>(0x020CAD38, this);
}
VERIFY(0x020CAD98, &Act_c::chk_sink_post);
bool Act_c::chk_exp_timer() {
  WWHD_FUNC(0x020CAC88, bool, this);
  if (mBombTimer > 0) {
    mBombTimer = mBombTimer - 1;
    if (mBombTimer == 0) {
      eff_explode();
      return true;
    }
  }
  return false;
}
VERIFY(0x020CAC88, &Act_c::chk_exp_timer);
u8 Act_c::chk_exp_pre() {
  WWHD_FUNC(0x020CAD3C, u8, this);
  return chk_exp_cc() || chk_exp_timer();
}
VERIFY(0x020CAD3C, &Act_c::chk_exp_pre);
void Act_c::init_mtx() {
  WWHD_FUNC(0x020C916C, void, this);
  tensor_init();
  eff_fuse_init();
  set_mtx();
}
VERIFY(0x020C916C, &Act_c::init_mtx);
void Act_c::eff_fuse_end() {
  WWHD_FUNC(0x020C95F4, void, this);
  mSmoke.deleteCallBack();
  mSparks.deleteCallBack();
  field_0x744 = 0;
}
VERIFY(0x020C95F4, &Act_c::eff_fuse_end);
void Act_c::mode_explode() {
  WWHD_FUNC(0x020CAE14, void, this);
  mEnv.proc(&current.pos);
  if (field_0x73C == 0 && mEnv.is_end())
    field_0x740 = 1;
}
VERIFY(0x020CAE14, &Act_c::mode_explode);
void Act_c::tensor_wait() {
  WWHD_FUNC(0x020CB398, void, this);
  if (mAcch.m_flags & 0x20)
    tensor_wait_ground();
  else
    tensor_wait_drop();
}
VERIFY(0x020CB398, &Act_c::tensor_wait);
void Act_c::tensor_carry() {
  WWHD_FUNC(0x020CB3AC, void, this);
  field_0x788 = 0;
  field_0x7A4 = 0.4f;
  field_0x784 = 0;
}
VERIFY(0x020CB3AC, &Act_c::tensor_carry);
void Act_c::tensor_explode() {
  WWHD_FUNC(0x020CB3CC, void, this);
  field_0x788 = 0;
  field_0x7A4 = 0.4f;
  field_0x784 = 0;
}
VERIFY(0x020CB3CC, &Act_c::tensor_explode);
void Act_c::tensor_sink() {
  WWHD_FUNC(0x020CB3EC, void, this);
  field_0x788 = 0;
  field_0x7A4 = 0.4f;
  field_0x784 = 0;
}
VERIFY(0x020CB3EC, &Act_c::tensor_sink);
void Act_c::eff_fuse_init() {
  WWHD_FUNC(0x020C8F2C, void, this);
  f32 x = gabi::load<f32>(0x101FFBA8), y = gabi::load<f32>(0x101FFBAC),
      z = gabi::load<f32>(0x101FFBB0);
  field_0x6C0.x = x;
  field_0x6C0.y = y;
  field_0x6D8.y = y;
  field_0x6C0.z = z;
  field_0x6CC.y = y;
  field_0x6CC.z = z;
  field_0x6D8.x = x;
  field_0x6D8.z = z;
  field_0x744 = 0;
  field_0x6CC.x = x;
}
VERIFY(0x020C8F2C, &Act_c::eff_fuse_init);
void Act_c::crr_init() {
  WWHD_FUNC(0x020C8B58, void, this);
  mCir.SetWall(30, 30);
  mAcch.Set(&current.pos, &old.pos, this, 1, &mCir, &speed, &current.angle,
            &shape_angle);
  u32 flags = mAcch.m_flags;
  field_0x526 = 0;
  field_0x524 = 0;
  mAcch.m_flags = (flags & ~0x408) | 0x2000;
  field_0x528 = -1000000000.0f;
  field_0x51C = -1000000000.0f;
  mbWaterIn = 0;
  mAcch.m_roof_crr_height = 50;
  field_0x520 = -1000000000.0f;
}
VERIFY(0x020C8B58, &Act_c::crr_init);
void Act_c::cc_init() {
  WWHD_FUNC(0x020C8C04, void, this);
  gabi::call(0x02515F14, &mStts, 200, 255, this);
  gabi::call(0x0251677C, &mSph, 0x10192378);
  gabi::store<u32>(gabi::ea(this) + 0x7A4, gabi::ea(&mStts));
}
VERIFY(0x020C8C04, &Act_c::cc_init);
bool Act_c::_delete() {
  WWHD_FUNC(0x020C962C, bool, this);
  eff_fuse_end();
  mEnv.clean();
  dComIfG_resDelete(&mPhase, gabi::at<char>(0x1000A9F4));
  return true;
}
VERIFY(0x020C962C, &Act_c::_delete);

void Env_c::set(const cXyz *pos) {
  WWHD_FUNC(0x020C81A4, void, this, pos);
  mPntLight.mPos.x = pos->x;
  f32 y = pos->y;
  mPntLight.mPos.y = y;
  f32 z = pos->z;
  mPntLight.mFluctuation = 100;
  gabi::store<u16>(gabi::ea(this) + 0xC, 200);
  gabi::store<u16>(gabi::ea(this) + 0xE, 200);
  gabi::store<u16>(gabi::ea(this) + 0x10, 160);
  mPntLight.mPos.y = y + 100;
  mPntLight.mPos.z = z;
  mPntLight.mPower = 600;
  gabi::call(0x0255B9C8, this);
  u32 s = gabi::ea(this);
  gabi::store<f32>(s + 0x24, pos->x);
  gabi::store<f32>(s + 0x28, pos->y);
  gabi::store<f32>(s + 0x44, 0);
  gabi::store<f32>(s + 0x2C, pos->z);
  gabi::store<f32>(s + 0x3C, 500);
  gabi::store<f32>(s + 0x34, 1);
  gabi::store<f32>(s + 0x38, 0);
  gabi::store<f32>(s + 0x30, 0);
  gabi::store<f32>(s + 0x40, 0.5f);
  gabi::call(0x0257DC90, s + 0x24);
  field_0x50 = 0;
  field_0x4C = 0;
}
VERIFY(0x020C81A4, &Env_c::set);
void Env_c::clean() {
  WWHD_FUNC(0x020C82A4, void, this);
  gabi::call(0x025553B8, 0, 0, 0, 0.0f);
  gabi::call(0x0255BA9C, this);
  gabi::call(0x0257D1B8, gabi::ea(this) + 0x24);
}
VERIFY(0x020C82A4, &Env_c::clean);
bool Env_c::is_end() {
  WWHD_FUNC(0x020C82F4, bool, this);
  return field_0x4C > 1;
}
VERIFY(0x020C82F4, &Env_c::is_end);
void Env_c::proc(const cXyz *pos) {
  WWHD_FUNC(0x020C8308, void, this, pos);
  u32 p = play2();
  f32 intensity = field_0x50;
  u32 camera = gabi::load<u32>(p + 0x5AF8);
  gabi::store<f32>(gabi::ea(this) + 0x40, intensity);
  mPntLight.mPower = 1500 * intensity;
  f32 distance = gabi::call<f32>(0x028E8DE8, pos, camera + 0xDC);
  distance = gabi::call<f32>(0x028F4384, distance);
  f32 factor = 0;
  if (distance < 1500)
    factor = gabi::fnmsubs(distance, 0.0006666666595265269f, 1);
  factor = field_0x50 * factor;
  gabi::call(0x025548F0, 200, 180, 100, factor);
  gabi::call(0x02554B18, 180, 160, 60, factor);
  gabi::call(0x02554C2C, 255, 225, 120, factor);
  switch (field_0x4C) {
  case 0:
    gabi::call<f32>(0x0200ECD4, &field_0x50, 1.0f, 0.5f, 0.4f, 0.01f);
    if (!(field_0x50 < 0.99f))
      field_0x4C = field_0x4C + 1;
    break;
  case 1:
    gabi::call<f32>(0x0200ECD4, &field_0x50, 0.0f, 0.05f, 0.04f, 0.001f);
    if (!(field_0x50 > 0.01f))
      field_0x4C = field_0x4C + 1;
    break;
  }
}
VERIFY(0x020C8308, &Env_c::proc);
Env_c *bomb2EnvCtor(Env_c *self) {
  WWHD_FUNC(0x020C84D8, Env_c *, self);
  if (!self)
    self = gabi::at<Env_c>(gabi::call<u32>(0x0273AD10, 0x58));
  if (self)
    gabi::store<f32>(gabi::ea(self) + 0x20, 1);
  return self;
}
VERIFY(0x020C84D8, bomb2EnvCtor);
void FuseSmokeCB_c::setOldPosP(const cXyz *a, const cXyz *b) {
  WWHD_FUNC(0x020C8518, void, this, a, b);
  field_0x0C = a;
  field_0x10 = b;
  field_0x04 = 20;
}
VERIFY(0x020C8518, &FuseSmokeCB_c::setOldPosP);
void FuseSmokeCB_c::deleteCallBack() {
  WWHD_FUNC(0x020C852C, void, this);
  u32 e = gabi::ea(mpEmitter.get());
  if (e) {
    gabi::store<u32>(e + 0x1E4, 0);
    e = gabi::ea(mpEmitter.get());
    u32 flags = gabi::load<u32>(e + 0x254);
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, flags | 1);
  }
  mpEmitter = nullptr;
}
VERIFY(0x020C852C, &FuseSmokeCB_c::deleteCallBack);
void FuseSparksCB_c::deleteCallBack() {
  WWHD_FUNC(0x020C8918, void, this);
  u32 e = gabi::ea(mpEmitter.get());
  if (e) {
    gabi::store<u32>(e + 0x1E4, 0);
    e = gabi::ea(mpEmitter.get());
    u32 flags = gabi::load<u32>(e + 0x254);
    gabi::store<s32>(e + 0x5C, -1);
    gabi::store<u32>(e + 0x254, flags | 1);
  }
  mpEmitter = nullptr;
}
VERIFY(0x020C8918, &FuseSparksCB_c::deleteCallBack);
void smoke_execute(void *self) { WWHD_FUNC(0x020C8560, void, self); }
VERIFY(0x020C8560, smoke_execute);
void smoke_draw(void *self) { WWHD_FUNC(0x020C8908, void, self); }
VERIFY(0x020C8908, smoke_draw);
void sparks_draw(void *self) { WWHD_FUNC(0x020C89AC, void, self); }
VERIFY(0x020C89AC, sparks_draw);
u32 bomb2Create(Act_c *self) {
  WWHD_FUNC(0x020CB40C, u32, self);
  return gabi::call<u32>(0x020C93EC, self);
}
VERIFY(0x020CB40C, bomb2Create);
u32 bomb2Delete(Act_c *self) {
  WWHD_FUNC(0x020CB410, u32, self);
  return gabi::call<u32>(0x020C962C, self);
}
VERIFY(0x020CB410, bomb2Delete);
u32 bomb2Execute(Act_c *self) {
  WWHD_FUNC(0x020CB414, u32, self);
  return gabi::call<u32>(0x020C9E78, self);
}
VERIFY(0x020CB414, bomb2Execute);
u32 bomb2Draw(Act_c *self) {
  WWHD_FUNC(0x020CB418, u32, self);
  return gabi::call<u32>(0x020CA000, self);
}
VERIFY(0x020CB418, bomb2Draw);
void empty1(void *self) { WWHD_FUNC(0x020CB4F4, void, self); }
VERIFY(0x020CB4F4, empty1);
void empty2(void *self) { WWHD_FUNC(0x020CB4F8, void, self); }
VERIFY(0x020CB4F8, empty2);
void empty3(void *self) { WWHD_FUNC(0x020CB4FC, void, self); }
VERIFY(0x020CB4FC, empty3);
void smoke_setup(FuseSmokeCB_c *self, void *emitter, const cXyz *pos,
                 const csXyz *angle, s8 room) {
  WWHD_FUNC(0x020C890C, void, self, emitter, pos, angle, room);
  self->mpPos = pos;
  self->mpEmitter = emitter;
}
VERIFY(0x020C890C, smoke_setup);
void sparks_setup(FuseSparksCB_c *self, void *emitter, const cXyz *pos,
                  const csXyz *angle, s8 room) {
  WWHD_FUNC(0x020C89B0, void, self, emitter, pos, angle, room);
  self->mpPos = pos;
  self->mpEmitter = emitter;
}
VERIFY(0x020C89B0, sparks_setup);

bool bomb2IsDelete(Act_c *self) {
  WWHD_FUNC(0x020CB5D8, bool, self);
  return true;
}
VERIFY(0x020CB5D8, bomb2IsDelete);
u32 bomb2PrmAbstract(const Act_c *self, u32 width, u32 shift) {
  WWHD_FUNC(0x020CB5E0, u32, self, width, shift);
  u32 value = gabi::load<u32>(gabi::ea(self) + 0xB0);
  u32 mask = (width & 32 ? 0 : 1u << (width & 31)) - 1;
  return (shift & 32 ? 0 : value >> (shift & 31)) & mask;
}
VERIFY(0x020CB5E0, bomb2PrmAbstract);
void bomb2GenericDtor(void *self, u32 flags) {
  WWHD_FUNC(0x020CB4E0, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x020CB4E0, bomb2GenericDtor);
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
void FuseSmokeCB_c::executeAfter(void *emitter) {
  WWHD_FUNC(0x020C8564, void, this, emitter);
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
VERIFY(0x020C8564, &FuseSmokeCB_c::executeAfter);
void FuseSparksCB_c::execute(void *emitter) {
  WWHD_FUNC(0x020C894C, void, this, emitter);
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
VERIFY(0x020C894C, &FuseSparksCB_c::execute);
void Act_c::bgCrrPos_lava() {
  WWHD_FUNC(0x020C8C78, void, this);
  u32 self = gabi::ea(this);
  f32 z = current.pos.z, y = old.pos.y + 1, x = current.pos.x;
  gabi::store<f32>(self + 0x6EC, z);
  gabi::store<f32>(self + 0x6E4, x);
  gabi::store<f32>(self + 0x6E8, y);
  u32 p = play2();
  field_0x51C = gabi::call<f32>(0x02008974, p + 0x12A0, self + 0x6C0);
}
VERIFY(0x020C8C78, &Act_c::bgCrrPos_lava);

void Act_c::bgCrrPos_water() {
  WWHD_FUNC(0x020C8CE4, void, this);
  f32 water = gabi::load<f32>(gabi::ea(this) + 0x678);
  s32 area =
      gabi::call<s32>(0x0246B6A4, (f32)current.pos.x, (f32)current.pos.z);
  f32 wave =
      gabi::call<f32>(0x0246BA0C, (f32)current.pos.x, (f32)current.pos.z);
  // GHS branches invert the GT/LT bit; keep the unordered cases explicit.
  bool in = (mAcch.m_flags & 0x1000) != 0, sea = area && current.pos.y < wave;
  if (in && (!sea || water > wave)) {
    field_0x520 = water;
    field_0x526 = 0;
    mbWaterIn = 1;
  } else if (sea) {
    field_0x520 = wave;
    mbWaterIn = 1;
    field_0x526 = 1;
  } else {
    mbWaterIn = 0;
    field_0x526 = 0;
    field_0x520 = -1000000000.0f;
  }
}
VERIFY(0x020C8CE4, &Act_c::bgCrrPos_water);

void Act_c::setRoomInfo() {
  WWHD_FUNC(0x020C8DDC, void, this);
  u32 self = gabi::ea(this);
  u8 room;
  if (mAcch.m_ground_h != -1000000000.0f) {
    u32 p = play2();
    s32 id = gabi::call<s32>(0x024EF130, p + 0x12A0, self + 0x5A4);
    p = play2();
    u32 color = gabi::call<u32>(0x024EEEB8, p + 0x12A0, self + 0x5A4);
    room = id;
    current.roomNo = room;
    gabi::store<u8>(self + 0x1C9, room);
    gabi::store<u8>(self + 0x1CA, color);
    gabi::store<u8>(self + 0x746, room);
  } else {
    room = gabi::load<u8>(0x1047E6C8);
    gabi::store<u8>(self + 0x1C9, room);
    gabi::store<u8>(self + 0x746, room);
    current.roomNo = room;
  }
}
VERIFY(0x020C8DDC, &Act_c::setRoomInfo);

void Act_c::bgCrrPos() {
  WWHD_FUNC(0x020C8E6C, void, this);
  u32 p = play2();
  gabi::call(0x024F08A8, &mAcch, p + 0x12A0);
  bgCrrPos_lava();
  bgCrrPos_water();
  setRoomInfo();
}
VERIFY(0x020C8E6C, &Act_c::bgCrrPos);

bool Act_c::is_draw() {
  WWHD_FUNC(0x020C9F18, bool, this);
  return mState != 2 && !field_0x743 && !field_0x742;
}
VERIFY(0x020C9F18, &Act_c::is_draw);

void Act_c::draw_nut() {
  WWHD_FUNC(0x020C9F50, void, this);
  u32 data = gabi::load<u32>(gabi::ea(mpModel.get()) + 0xAC);
  mBck0.entry(gabi::at<J3DModelData>(data), mBck0.getFrame());
  mBrk0.entry(gabi::at<J3DModelData>(data), mBrk0.getFrame());
  u32 p = play2();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D58));
  p = play2();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D60));
  mDoExt_modelUpdateDL(mpModel.get(), 0);
  p = play2();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D78));
  p = play2();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D7C));
  u32 obj = gabi::load<u32>(data + 8);
  gabi::store<u32>(obj + 0x14, 0);
  gabi::store<u32>(data + 0x48, 0);
}
VERIFY(0x020C9F50, &Act_c::draw_nut);

bool Act_c::_draw() {
  WWHD_FUNC(0x020CA000, bool, this);
  if (is_draw()) {
    u32 env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x025626A4, env, 0, &current.pos, &tevStr);
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, env, mpModel.get(), &tevStr);
    draw_nut();
  }
  return true;
}
VERIFY(0x020CA000, &Act_c::_draw);

void Act_c::se_ignition() {
  WWHD_FUNC(0x020C8F6C, void, this);
  s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
  gabi::call(0x025E1A40, 0x6100, &eyePos, 0, reverb);
}
VERIFY(0x020C8F6C, &Act_c::se_ignition);

void Act_c::se_explode_water() {
  WWHD_FUNC(0x020CA06C, void, this);
  s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
  gabi::call(0x025E1A40, 0x6982, &eyePos, 0, reverb);
}
VERIFY(0x020CA06C, &Act_c::se_explode_water);

void Act_c::se_explode() {
  WWHD_FUNC(0x020CA25C, void, this);
  s32 reverb = gabi::call<s32>(0x02520540, (s8)current.roomNo);
  gabi::call(0x025E1A40, 0x6901, &eyePos, 0, reverb);
}
VERIFY(0x020CA25C, &Act_c::se_explode);

void Act_c::set_sound_env(s32 a, s32 b) {
  WWHD_FUNC(0x020CA0B4, void, this, a, b);
  gabi::Local<cXyz> p;
  p->x = current.pos.x;
  p->y = current.pos.y;
  p->z = current.pos.z;
  gabi::call(0x0255F458, p.get(), a, gabi::load<u32>(gabi::ea(this) + 4), b);
}
VERIFY(0x020CA0B4, &Act_c::set_sound_env);

void Act_c::eff_explode_water() {
  WWHD_FUNC(0x020CA0F8, void, this);
  if (field_0x743)
    return;
  gabi::call(0x025DAE64, &current.pos, 1, 1.0f, 1.0f);
  field_0x743 = 1;
  se_explode_water();
  set_sound_env(255, 10);
}
VERIFY(0x020CA0F8, &Act_c::eff_explode_water);

void Act_c::eff_explode_normal(const csXyz *angle) {
  WWHD_FUNC(0x020CA164, void, this, angle);
  u32 c = gabi::load<u32>(play2() + 0x5AB0);
  gabi::call(0x025A847C, c, 1, 0xB, &current.pos, angle, &scale, 255, 0, -1, 0,
             0, 0);
  c = gabi::load<u32>(play2() + 0x5AB0);
  gabi::call(0x025A866C, c, 0x2009, &current.pos, 0, &scale, 255);
  c = gabi::load<u32>(play2() + 0x5AB0);
  gabi::call(0x025A866C, c, 0x200A, &current.pos, 0, &scale, 255);
  c = gabi::load<u32>(play2() + 0x5AB0);
  gabi::call(0x025A847C, c, 3, 0x2008, &current.pos, 0, &scale, 255, 0, -1, 0,
             0, 0);
}
VERIFY(0x020CA164, &Act_c::eff_explode_normal);

void Act_c::eff_explode() {
  WWHD_FUNC(0x020CA2A4, void, this);
  if (field_0x742)
    return;
  field_0x742 = 1;
  u32 p = play2();
  s32 id = gabi::load<s8>(p + 0x5B30);
  u32 cam = gabi::load<u32>(play2() + id * 0x34 + 0x5AF8);
  gabi::Local<csXyz> a;
  a->x = -gabi::load<s16>(cam + 0x234);
  a->y = gabi::load<s16>(cam + 0x236) + 0x8000;
  a->z = 0;
  eff_explode_normal(a.get());
  se_explode();
  set_sound_env(255, 10);
}
VERIFY(0x020CA2A4, &Act_c::eff_explode);

void Act_c::camera_lockoff() {
  WWHD_FUNC(0x020CA34C, void, this);
  s32 id = gabi::load<s8>(play2() + 0x5B30);
  u32 cam = gabi::load<u32>(play2() + id * 0x34 + 0x5AF8);
  gabi::call(0x025052BC, cam + 0x248, gabi::load<u32>(gabi::ea(this) + 4));
}
VERIFY(0x020CA34C, &Act_c::camera_lockoff);

void Act_c::mode_explode_init() {
  WWHD_FUNC(0x020CA3A4, void, this);
  mState = 2;
  camera_lockoff();
  eff_fuse_end();
  mEnv.set(&current.pos);
  speedF = 0;
  u32 s = gabi::ea(this);
  // The optimized original copies cXyz::Zero with lwz/stw, preserving raw
  // words (including signaling NaNs), rather than lfs/stfs float transport.
  for (int i = 0; i < 3; i++)
    gabi::store<u32>(s + 0x33C + 4 * i, gabi::load<u32>(0x101FFBA8 + 4 * i));
  gravity = 0;
  off_carry();
  u32 co = gabi::load<u32>(s + 0x78C), at = gabi::load<u32>(s + 0x760),
      tg = gabi::load<u32>(s + 0x778);
  gabi::store<u32>(s + 0x78C, co & ~1);
  gabi::store<u32>(s + 0x760, at | 1);
  gabi::store<u32>(s + 0x778, tg & ~1);
  gabi::call(0x025D9D24, this);
  mBombTimer = 0;
  field_0x73C = 4;
  u32 p = play2();
  gabi::Local<cXyz> d;
  d->x = 0;
  d->y = 1;
  d->z = 0;
  gabi::call(0x025CB374, p + 0x599C, 7, -33, d.get());
}
VERIFY(0x020CA3A4, &Act_c::mode_explode_init);

void Act_c::start_explode_instant() {
  WWHD_FUNC(0x020CA49C, void, this);
  if (chk_water_in())
    eff_explode_water();
  else
    eff_explode();
  mode_explode_init();
}
VERIFY(0x020CA49C, &Act_c::start_explode_instant);

void Act_c::eff_fuse_start() {
  WWHD_FUNC(0x020CA4F0, void, this);
  if (field_0x744)
    return;
  f32 x = current.pos.x, y = current.pos.y;
  field_0x744 = 1;
  field_0x6C0.x = x;
  field_0x6C0.y = (y + 60.0f);
  field_0x6C0.z = current.pos.z;
  field_0x6CC.x = field_0x6C0.x;
  field_0x6CC.y = field_0x6C0.y;
  field_0x6CC.z = field_0x6C0.z;
  u32 s = gabi::ea(this);
  for (int i = 0; i < 3; i++)
    gabi::store<u32>(s + 0x8D0 + 4 * i, gabi::load<u32>(s + 0x8B8 + 4 * i));
  u32 c = gabi::load<u32>(play2() + 0x5AB0);
  gabi::call(0x025A847C, c, 1, 0x11, &field_0x6C0, 0, &scale, 255, &mSparks, -1,
             0, 0, 0);
  s32 room = current.roomNo;
  c = gabi::load<u32>(play2() + 0x5AB0);
  gabi::call(0x025A847C, c, 3, 0x2012, &field_0x6C0, 0, &scale, 220, &mSmoke,
             room, s + 0x1A8, s + 0x1A8, 0);
  mSmoke.setOldPosP(&field_0x6CC, &field_0x6D8);
}
VERIFY(0x020CA4F0, &Act_c::eff_fuse_start);

void Act_c::mode_carry_init() {
  WWHD_FUNC(0x020C9980, void, this);
  mState = 1;
  speedF = 0;
  speed.x = gabi::load<f32>(0x101FFBA8);
  speed.y = gabi::load<f32>(0x101FFBAC);
  speed.z = gabi::load<f32>(0x101FFBB0);
  off_carry();
  u32 s = gabi::ea(this) + 0x78C;
  gabi::store<u32>(s, gabi::load<u32>(s) & ~1);
}
VERIFY(0x020C9980, &Act_c::mode_carry_init);

bool Act_c::create_heap_nut() {
  WWHD_FUNC(0x020C89BC, bool, this);
  struct Name {
    be<u32> a, b;
  };
  gabi::Local<Name> n;
  n->a = 0x1000A9F4;
  n->b = 0x1000A9FC;
  u32 data =
      gabi::call<u32>(0x026066C4, gabi::load<u32>(0x101F4F28), n.get(), 12);
  if (!data)
    gabi::call(0x0273AA24, 0x1000AB80, 0x307, 0x1000AB50);
  mpModel = gabi::at<J3DModel>(
      gabi::call<u32>(0x025E38E0, data, 0x80000, 0x11000022));
  u32 anm =
      gabi::call<u32>(0x026066C4, gabi::load<u32>(0x101F4F28), n.get(), 7);
  if (!anm)
    gabi::call(0x0273AA24, 0x1000AB80, 0x311, 0x1000AB60);
  u32 b = gabi::call<u32>(0x025E8508, &mBck0, data, anm, 1, 0, 0, -1, 0, 1.0f);
  anm = gabi::call<u32>(0x026066C4, gabi::load<u32>(0x101F4F28), n.get(), 16);
  if (!anm)
    gabi::call(0x0273AA24, 0x1000AB80, 0x318, 0x1000AB70);
  u32 k =
      gabi::call<u32>(0x025E8154, &mBrk0, data, anm, 1, 0, 0, -1, 0, 0, 1.0f);
  return mpModel && b && k;
}
VERIFY(0x020C89BC, &Act_c::create_heap_nut);

void Act_c::create_init() {
  WWHD_FUNC(0x020C9218, void, this);
  crr_init();
  cc_init();
  u32 s = gabi::ea(this), mdl = mpModel.v.get();
  gabi::store<u32>(s + 0x348, mdl ? mdl + 0xC8 : 0);
  f32 r = gabi::load<f32>(0x104626B0), h = gabi::load<f32>(0x104626B4);
  gabi::call(0x025D674C, this, -r, 0.0f, -r, r, h, r);
  gabi::store<f32>(s + 0x364, 10);
  if (gabi::call<u32>(0x020CB5E0, this, 1, 8))
    off_carry();
  else
    on_carry();
  gravity = -2.9f;
  maxFallSpeed = -100;
  bgCrrPos();
  f32 x = home.pos.x, z = home.pos.z;
  current.pos.x = x;
  speed.y = 0;
  f32 y = home.pos.y;
  speedF = 0;
  current.pos.y = y;
  current.pos.z = z;
  init_mtx();
  field_0x743 = 0;
  mBombTimer = 150;
  field_0x740 = 0;
  field_0x742 = 0;
  field_0x741 = 0;
  mWindVec.x = gabi::load<f32>(0x101FFBA8);
  mWindVec.y = gabi::load<f32>(0x101FFBAC);
  mWindVec.z = gabi::load<f32>(0x101FFBB0);
  start_proc_call();
}
VERIFY(0x020C9218, &Act_c::create_init);

void Act_c::eff_fuse_move() {
  WWHD_FUNC(0x020C8FB4, void, this);
  if (!gabi::load<u32>(0x104626E4)) {
    gabi::store<u32>(0x104626E4, 1);
    gabi::store<f32>(0x104626E0, 5);
    gabi::store<f32>(0x104626D8, 0);
    gabi::store<f32>(0x104626DC, 60);
  }
  u32 s = gabi::ea(this);
  for (int i = 0; i < 3; i++) {
    u32 old = gabi::load<u32>(s + 0x8C4 + 4 * i),
        cur = gabi::load<u32>(s + 0x8B8 + 4 * i);
    gabi::store<u32>(s + 0x8D0 + 4 * i, old);
    gabi::store<u32>(s + 0x8C4 + 4 * i, cur);
  }
  gabi::call(0x028E8F64, 0x1048D0CC, 0x104626D8, &field_0x6C0);
  if (field_0x744)
    se_ignition();
}
VERIFY(0x020C8FB4, &Act_c::eff_fuse_move);

void Act_c::set_mtx() {
  WWHD_FUNC(0x020C9074, void, this);
  u32 mdl = mpModel.v.get();
  gabi::store<f32>(mdl + 0xBC, scale.x);
  gabi::store<f32>(mdl + 0xC0, scale.y);
  gabi::store<f32>(mdl + 0xC4, scale.z);
  gabi::call(0x028E93CC, 0x1048D0CC, (f32)current.pos.x, (f32)current.pos.y,
             (f32)current.pos.z);
  gabi::call(0x028E9108, 0x1048D0CC, gabi::ea(this) + 0x950, 0x1048D0CC);
  gabi::call(0x025F1B48, 0x1048D0CC, (s16)shape_angle.x, (s16)shape_angle.y,
             (s16)shape_angle.z);
  f32 m[12];
  for (int i = 0; i < 12; i++)
    m[i] = gabi::load<f32>(0x1048D0CC + 4 * i);
  mdl = mpModel.v.get();
  for (int i = 0; i < 12; i++)
    gabi::store<f32>(mdl + 0xC8 + 4 * i, m[i]);
  eff_fuse_move();
}
VERIFY(0x020C9074, &Act_c::set_mtx);

void Act_c::anm_play() {
  WWHD_FUNC(0x020C9CBC, void, this);
  if ((s32)((u32)mBombTimer + 1) <= 135) {
    gabi::call(0x025E742C, &mBck0);
    gabi::call(0x025E742C, &mBrk0);
  }
}
VERIFY(0x020C9CBC, &Act_c::anm_play);

void Act_c::tensor_wait_drop() {
  WWHD_FUNC(0x020CB34C, void, this);
  if (field_0x7A8 > 0) {
    field_0x79C = (f32)speed.z * -0.005f;
    field_0x7A0 = (f32)speed.x * -0.005f;
  }
  field_0x788 = 0;
  field_0x7A4 = 0.1f;
  field_0x784 = 0;
}
VERIFY(0x020CB34C, &Act_c::tensor_wait_drop);

void Act_c::tensor_wait_ground() {
  WWHD_FUNC(0x020CB2BC, void, this);
  u32 p = play2(), s = gabi::ea(this);
  u32 plane =
      gabi::call<u32>(0x020084C8, p + 0x12A0, gabi::load<u16>(s + 0x5A6),
                      gabi::load<u16>(s + 0x5A4));
  if (plane) {
    field_0x784 = gabi::load<f32>(plane + 8) * 1.5f;
    f32 x = gabi::load<f32>(plane) * 1.5f;
    field_0x7A4 = 0.4f;
    field_0x788 = x;
  } else {
    field_0x784 = 0;
    field_0x7A4 = 0.4f;
    field_0x788 = 0;
  }
}
VERIFY(0x020CB2BC, &Act_c::tensor_wait_ground);

void Act_c::mode_sink() {
  WWHD_FUNC(0x020CB218, void, this);
  gabi::Local<be<f32>> y;
  if (gabi::call<u32>(0x025D9F70, &current.pos, y.get()) &&
      field_0x528 != -1000000000.0f) {
    field_0x698 = (s32)((u32)field_0x698 - 1);
    if (field_0x698 > 0) {
      f32 dy = y->get() - (f32)field_0x528;
      current.pos.y = (f32)current.pos.y + dy;
      field_0x528 = y->get();
      posMoveF();
      return;
    }
  }
  field_0x740 = 1;
}
VERIFY(0x020CB218, &Act_c::mode_sink);

void Act_c::set_vib_tensor() {
  WWHD_FUNC(0x020C9BB8, void, this);
  gabi::Local<cXyz> d;
  struct Quat {
    be<f32> x, y, z, w;
  };
  gabi::Local<Quat> q;
  d->x = field_0x790;
  d->y = 1;
  d->z = field_0x78C;
  gabi::call(0x02312548, q.get(), d.get());
  gabi::call(0x028E8C78, gabi::ea(this) + 0x950, q.get());
}
VERIFY(0x020C9BB8, &Act_c::set_vib_tensor);

void bomb2Sinit() {
  WWHD_FUNC(0x020CB41C, void, 0);
  for (u32 i = 0; i < 16; i += 4)
    gabi::store<u32>(0x104626BC + i, 0);
  gabi::call(0x028F026C, 0x10192324);
  gabi::store<f32>(0x104626A8, -3.1415927410125732f);
  gabi::store<f32>(0x104626AC, 3.1415927410125732f);
  gabi::call(0x028ED6F8, 0x104626B8);
  gabi::call(0x028F026C, 0x10192330);
  gabi::call(0x028EAB2C, 0x104626B9);
  gabi::call(0x028F026C, 0x1019233C);
  gabi::store<f32>(0x101923B4, 30);
  gabi::store<f32>(0x104626B4, 66);
  gabi::store<f32>(0x104626B0, 36);
}
VERIFY(0x020CB41C, bomb2Sinit);
void bomb2ActDtor(Act_c *a, u32 flags) {
  WWHD_FUNC(0x020CB500, void, a, flags);
  if (!a)
    return;
  u32 s = gabi::ea(a);
  gabi::call(0x02515AE8, s + 0x760, 2);
  gabi::call(0x02515860, s + 0x724, 2);
  gabi::store<u32>(s + 0x6E0, 0x1000AA5C);
  gabi::store<u32>(s + 0x700, 0x1000AA7C);
  gabi::store<u32>(s + 0x70C, 0x1000AA3C);
  gabi::call(0x02008DAC, s + 0x6C0, 0);
  gabi::call(0x02018034, s + 0x694, 2);
  gabi::store<u32>(s + 0x4DC, 0x1000AB1C);
  gabi::store<u32>(s + 0x4D0, 0x1000AB2C);
  gabi::call(0x024EFD9C, s + 0x4BC, 0);
  gabi::call(0x027F3628, s + 0x3C8, 0);
  gabi::call(0x025D50BC, a, 0);
  if (flags & 1)
    gabi::call(0x0273AF40, a);
}
VERIFY(0x020CB500, bomb2ActDtor);

cPhs_State Act_c::_create() {
  WWHD_FUNC(0x020C93EC, cPhs_State, this);
  u32 s = gabi::ea(this), flags = gabi::load<u32>(s + 0x2E4);
  if (!(flags & 8)) {
    if (s) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(s + 0xB4, 0x1000AB3C);
      gabi::call(0x027F2BC0, s + 0x3B8, 0);
      gabi::store<u32>(s + 0x3C8, 0x1016E54C);
      gabi::call(0x027DA984, s + 0x3CC);
      gabi::store<u32>(s + 0x400, 0x1016D820);
      for (u32 o : {0x43Cu, 0x434u, 0x438u, 0x440u, 0x410u})
        gabi::store<u32>(s + o, 0);
      gabi::store<u32>(s + 0x3C8, 0x1000AA14);
      gabi::call(0x025E80D0, s + 0x444);
      gabi::call(0x024F0474, s + 0x4BC);
      gabi::store<u32>(s + 0x4CC, 0x1000AB0C);
      gabi::store<u32>(s + 0x4D0, 0x1000AB2C);
      gabi::store<u8>(s + 0x4D8, 1);
      gabi::store<u32>(s + 0x4DC, 0x1000AB1C);
      gabi::call(0x024EFE94, s + 0x680);
      gabi::call(0x02008E0C, s + 0x6C0);
      for (u32 o : {0x707u, 0x709u, 0x706u, 0x705u, 0x70Au, 0x708u})
        gabi::store<u8>(s + o, 0);
      gabi::store<u8>(s + 0x704, 1);
      gabi::store<u32>(s + 0x6C0, s + 0x700);
      gabi::store<u32>(s + 0x6C4, s + 0x70C);
      gabi::store<u32>(s + 0x6E0, 0x1000AADC);
      gabi::store<u32>(s + 0x710, 4);
      gabi::store<u32>(s + 0x70C, 0x1000AAEC);
      gabi::store<u32>(s + 0x700, 0x1000AAFC);
      gabi::store<u32>(s + 0x6D0, 0x1000AACC);
      gabi::call(0x0200BD2C, s + 0x724);
      gabi::call(0x02515DA0, s + 0x740);
      gabi::store<u32>(s + 0x73C, 0x1004AE88);
      gabi::store<u32>(s + 0x740, 0x1004AEC0);
      gabi::call(0x025166F0, s + 0x760);
      gabi::store<u32>(s + 0x894, 0x1000AC68);
      gabi::store<u32>(s + 0x8AC, 0x1000ACA8);
      gabi::call(0x020C84D8, s + 0x8DC);
      flags = gabi::load<u32>(s + 0x2E4);
    }
    gabi::store<u32>(s + 0x2E4, flags | 8);
  }
  u32 state = gabi::call<u32>(0x02520460, s + 0x3AC, 0x1000A9F4);
  if (state == 4) {
    if (gabi::call<u32>(0x025D63E8, this, 0x020C8B54, 0x920))
      create_init();
    else
      state = 5;
  }
  return (cPhs_State)state;
}
VERIFY(0x020C93EC, &Act_c::_create);

bool Act_c::_execute() {
  WWHD_FUNC(0x020C9E78, bool, this);
  set_wind_vec();
  mode_proc_call();
  if (field_0x740) {
    gabi::call(0x025D57E0, this);
    return true;
  }
  tensor_proc_call();
  anm_play();
  f32 x = current.pos.x, y = (f32)current.pos.y + 50, z = current.pos.z;
  u32 s = gabi::ea(this);
  for (u32 o : {0x37Cu, 0x390u}) {
    gabi::store<f32>(s + o, x);
    gabi::store<f32>(s + o + 4, y);
    gabi::store<f32>(s + o + 8, z);
  }
  set_mtx();
  cc_set();
  return true;
}
VERIFY(0x020C9E78, &Act_c::_execute);

void Act_c::set_real_shadow_flag() {
  WWHD_FUNC(0x020C99DC, void, this);
  u32 s = gabi::ea(this);
  bool on = false;
  if (mState == 1) {
    if (gabi::load<u32>(s + 0x368))
      on = true;
    else {
      u32 link = gabi::load<u32>(play2() + 0x5B2C),
          vt = gabi::load<u32>(link + 0xB4), fn = gabi::load<u32>(vt + 0xBC);
      if (gabi::call<u32>(fn, link) == gabi::load<u32>(s + 4) &&
          (gabi::load<u32>(link + 0x3C0) & 0x8000))
        on = true;
    }
  }
  gabi::store<u32>(s + 0x368, on ? mpModel.v.get() : 0);
}
VERIFY(0x020C99DC, &Act_c::set_real_shadow_flag);

void Act_c::start_proc_call() {
  WWHD_FUNC(0x020C91A4, void, this);
  u32 p = gabi::call<u32>(0x020CB5E0, this, 2, 0);
  invokeBomb2(gabi::ea(this), 0x1000ABA4 + 8 * p);
}
VERIFY(0x020C91A4, &Act_c::start_proc_call);

void Act_c::mode_proc_call() {
  WWHD_FUNC(0x020C9A6C, void, this);
  u32 s = gabi::ea(this);
  s32 state = mState;
  if (gabi::load<u32>(s + 0x2E0) & 0x2000) {
    if (state != 1)
      mode_carry_init();
    else
      goto dispatch;
    state = mState;
  }
dispatch:
  invokeBomb2(s, 0x1000ABDC + 8 * state);
  set_real_shadow_flag();
}
VERIFY(0x020C9A6C, &Act_c::mode_proc_call);

void Act_c::tensor_proc_call() {
  WWHD_FUNC(0x020C9C14, void, this);
  if (!gabi::call<u32>(0x020CB5E0, this, 1, 8)) {
    invokeBomb2(gabi::ea(this), 0x1000AC00 + 8 * (s32)mState);
    vib_proc();
    set_vib_tensor();
  }
}
VERIFY(0x020C9C14, &Act_c::tensor_proc_call);

void Act_c::posMoveF() {
  WWHD_FUNC(0x020CA698, void, this);
  if (gabi::call<f32>(0x028E8DD0, &mWindVec) > 0.01f) {
    u32 p = play2(), s = gabi::ea(this);
    u32 plane =
        gabi::call<u32>(0x020084C8, p + 0x12A0, gabi::load<u16>(s + 0x5A6),
                        gabi::load<u16>(s + 0x5A4));
    gabi::call(0x023121C4, this, &mStts, &mWindVec, plane, 0, 0.002f, 0.0005f,
               plane ? 0.06f : 0.0f,
               plane ? gabi::load<f32>(0x104A4F44) : 0.0f);
  } else
    gabi::call(0x025D6870, this, &mStts);
}
VERIFY(0x020CA698, &Act_c::posMoveF);

void Act_c::se_fall_water() {
  WWHD_FUNC(0x020CA91C, void, this);
  u32 s = gabi::ea(this), polys[2] = {field_0x526 ? 0 : s + 0x630, s + 0x5A4};
  u32 material = 19;
  for (u32 poly : polys) {
    if (poly && gabi::load<u16>(poly + 2) < 256) {
      material = gabi::call<u32>(0x024EECAC, play2() + 0x12A0, poly);
      break;
    }
  }
  u32 rev = gabi::call<u32>(0x02520540, (s32)current.roomNo);
  gabi::call(0x025E1A40, 0x6918, s + 0x37C, material, rev);
}
VERIFY(0x020CA91C, &Act_c::se_fall_water);

void Act_c::eff_water_splash() {
  WWHD_FUNC(0x020CA9F8, void, this);
  gabi::Local<cXyz> p;
  p->x = current.pos.x;
  p->y = field_0x520;
  p->z = current.pos.z;
  gabi::call(0x025DAE64, p.get(), 0, 0.5f, 0.75f);
  se_fall_water();
  set_sound_env(100, 5);
}
VERIFY(0x020CA9F8, &Act_c::eff_water_splash);

bool Act_c::chk_exp_cc_nut() {
  WWHD_FUNC(0x020CAA6C, bool, this);
  bool explode = false, fuse = false;
  if (gabi::call<u32>(0x025162A4, &mSph)) {
    u32 hit = gabi::call<u32>(0x02516300, &mSph);
    if (hit) {
      u32 f = gabi::load<u32>(hit + 0x10);
      if (f & 0x20)
        explode = true;
      else if (!(f & 0x200000))
        fuse = true;
    }
    gabi::call(0x0251621C, &mSph);
  }
  if (gabi::call<u32>(0x02516464, &mSph)) {
    if (field_0x741)
      fuse = true;
    else {
      f32 mag = gabi::call<f32>(0x028E8DD0, &speed);
      mag = gabi::call<f32>(0x028F4384, mag);
      if (mag > 25)
        fuse = true;
    }
    gabi::call(0x0251641C, &mSph);
  }
  if (fuse) {
    eff_fuse_start();
    set_nut_exp_interval();
  }
  if (explode)
    eff_explode();
  return explode;
}
VERIFY(0x020CAA6C, &Act_c::chk_exp_cc_nut);

u32 Act_c::chk_exp_cc() {
  WWHD_FUNC(0x020CAB6C, u32, this);
  if (mBombTimer <= 0)
    return 0;
  return gabi::call<u32>(0x020CAA6C, this);
}
VERIFY(0x020CAB6C, &Act_c::chk_exp_cc);

bool Act_c::chk_exp_bg_nut() {
  WWHD_FUNC(0x020CAB88, bool, this);
  u8 water = chk_water_in();
  bool lava = chk_lava_in(), hit = (mAcch.m_flags & (0x30 | 0x200)) != 0;
  if (lava) {
    eff_explode();
    return true;
  }
  if (water) {
    f32 mag = gabi::call<f32>(0x028E8DD0, &speed);
    mag = gabi::call<f32>(0x028F4384, mag);
    if (mag > 20) {
      eff_explode_water();
      return true;
    }
  }
  if (hit) {
    bool fuse = field_0x741;
    if (!fuse) {
      f32 mag = gabi::call<f32>(0x028E8DD0, &speed);
      mag = gabi::call<f32>(0x028F4384, mag);
      fuse = mag > 25;
    }
    if (fuse) {
      eff_fuse_start();
      set_nut_exp_interval();
    }
  }
  return false;
}
VERIFY(0x020CAB88, &Act_c::chk_exp_bg_nut);

bool Act_c::chk_sink_bg_nut() {
  WWHD_FUNC(0x020CACCC, bool, this);
  if (chk_water_in()) {
    f32 mag = gabi::call<f32>(0x028E8DD0, &speed);
    mag = gabi::call<f32>(0x028F4384, mag);
    if (!(mag > 20)) {
      eff_water_splash();
      return true;
    }
  }
  return false;
}
VERIFY(0x020CACCC, &Act_c::chk_sink_bg_nut);

void Act_c::carry_fuse_start() {
  WWHD_FUNC(0x020CAD9C, void, this);
  if (field_0x744)
    return;
  u32 link = gabi::load<u32>(play2() + 0x5B2C),
      vt = gabi::load<u32>(link + 0xB4), fn = gabi::load<u32>(vt + 0xBC);
  if (gabi::call<u32>(fn, link) != gabi::load<u32>(gabi::ea(this) + 4) ||
      (gabi::load<u32>(link + 0x3C0) & 0x20))
    eff_fuse_start();
}
VERIFY(0x020CAD9C, &Act_c::carry_fuse_start);

void Act_c::mode_carry() {
  WWHD_FUNC(0x020CAE6C, void, this);
  carry_fuse_start();
  if (chk_exp_pre()) {
    mode_explode_init();
    mode_explode();
    return;
  }
  if (!(gabi::load<u32>(gabi::ea(this) + 0x2E0) & 0x2000)) {
    if (speedF > 0) {
      field_0x741 = 1;
      field_0x7A8 = 2;
    }
    mode_wait_init();
    mode_wait();
    return;
  }
  f32 y = current.pos.y, z = current.pos.z, x = current.pos.x;
  bgCrrPos();
  current.pos.y = y;
  current.pos.x = x;
  current.pos.z = z;
}
VERIFY(0x020CAE6C, &Act_c::mode_carry);

void Act_c::mode_sink_init() {
  WWHD_FUNC(0x020CAFB4, void, this);
  u32 s = gabi::ea(this), co = gabi::load<u32>(s + 0x78C),
      at = gabi::load<u32>(s + 0x760), tg = gabi::load<u32>(s + 0x778);
  speed.y = (f32)speed.y * 0.8f;
  speedF = (f32)speedF * 0.8f;
  mState = 3;
  gabi::store<u32>(s + 0x760, at & ~1);
  gabi::store<u32>(s + 0x778, tg & ~1);
  gabi::store<u32>(s + 0x78C, co & ~1);
  off_carry();
  gabi::call(0x025D9D24, this);
  field_0x698 = 4;
  gabi::call(0x025D9F70, &current.pos, &field_0x528);
}
VERIFY(0x020CAFB4, &Act_c::mode_sink_init);

void Act_c::mode_wait() {
  WWHD_FUNC(0x020CB044, void, this);
  if (chk_exp_pre()) {
    mode_explode_init();
    mode_explode();
    return;
  }
  if (gabi::load<u32>(gabi::ea(this) + 0x2E0) & 0x2000) {
    mode_carry_init();
    mode_carry();
    return;
  }
  u32 fixed = gabi::call<u32>(0x020CB5E0, this, 1, 8);
  f32 oldY = 0;
  if (!fixed) {
    if (!field_0x745)
      posMoveF();
    oldY = speed.y;
    bgCrrPos();
  }
  if (chk_exp_post()) {
    mode_explode_init();
    return;
  }
  if (chk_sink_post()) {
    mode_sink_init();
    return;
  }
  if (!field_0x740) {
    if (!fixed) {
      bound(oldY);
      if (mAcch.m_flags & 0x20) {
        on_carry();
        return;
      }
    }
    off_carry();
  }
}
VERIFY(0x020CB044, &Act_c::mode_wait);

void Act_c::vib_proc() {
  WWHD_FUNC(0x020C9B18, void, this);
  f32 dx = (f32)field_0x78C - (f32)field_0x784,
      dz = (f32)field_0x790 - (f32)field_0x788;
  f32 vx = gabi::fnmsubs(dx, 0.03f, field_0x79C),
      vz = gabi::fnmsubs(dz, 0.03f, field_0x7A0);
  f32 ax = (f32)field_0x794 + (vx + vx), az = (f32)field_0x798 + (vz + vz),
      keep = 1 - (f32)field_0x7A4;
  f32 nx = ax * keep, nz = az * keep;
  field_0x79C = 0;
  field_0x7A0 = 0;
  field_0x78C = (f32)field_0x78C + nx;
  field_0x794 = nx;
  field_0x798 = nz;
  field_0x790 = (f32)field_0x790 + nz;
  if (field_0x7A8 > 0)
    field_0x7A8 = field_0x7A8 - 1;
}
VERIFY(0x020C9B18, &Act_c::vib_proc);

void Act_c::bound(f32 value) {
  WWHD_FUNC(0x020CA7C0, void, this, value);
  u32 s = gabi::ea(this), f = mAcch.m_flags;
  if (f & 0x10) {
    s16 wall = gabi::load<s16>(s + 0x6BC);
    speedF = (f32)speedF * 0.8f;
    s16 dir = gabi::load<s16>(s + 0x322);
    f = mAcch.m_flags;
    gabi::store<s16>(s + 0x322, 2 * (s32)wall - ((s32)dir + 0x8000));
  }
  if (f & 0x80) {
    gabi::call(0x02311CD8, this, s + 0x590, 0.6f);
    f32 bounce = value * -0.6f;
    if (bounce < 19.5f) {
      field_0x741 = 0;
      return;
    }
    speedF = (f32)speedF * 0.9f;
    speed.y = (13.0f - bounce) >= 0 ? bounce : 13.0f;
    return;
  }
  if (f & 0x20)
    gabi::call<f32>(0x0200ECD4, &speedF, 0.0f, 0.5f, 5.0f, 1.0f);
}
VERIFY(0x020CA7C0, &Act_c::bound);

void Act_c::cc_set() {
  WWHD_FUNC(0x020C9D04, void, this);
  gabi::Local<cXyz> pos;
  pos->x = current.pos.x;
  pos->y = current.pos.y;
  pos->z = current.pos.z;
  s32 state = mState;
  f32 radius = 200;
  if (state == 2) {
    if (field_0x73C <= 0)
      return;
    field_0x73C = field_0x73C - 1;
    if (field_0x73C <= 0)
      return;
  } else {
    if (state == 3)
      return;
    radius = 30 * (f32)scale.x;
    if (!gabi::load<u32>(0x104626E8)) {
      gabi::store<u32>(0x104626E8, 1);
      gabi::store<f32>(0x104626D4, 0);
      gabi::store<f32>(0x104626D0, 30);
      gabi::store<f32>(0x104626CC, 0);
    }
    u32 mdl = mpModel.v.get();
    gabi::call(0x028E90D4, mdl ? mdl + 0xC8 : 0, 0x1048D0CC);
    gabi::call(0x025F2518, (f32)scale.x, (f32)scale.y, (f32)scale.z);
    gabi::call(0x028E8F64, 0x1048D0CC, 0x104626CC, pos.get());
  }
  u32 s = gabi::ea(this);
  gabi::call(0x02515E50, s + 0x740);
  gabi::call(0x02018C8C, s + 0x878, radius);
  gabi::call(0x02018D40, s + 0x878, pos.get());
  gabi::call(0x0200E240, play2() + 0x26A4, &mSph);
  gabi::call(0x02516C14, play2() + 0x4EF8, &mSph, 3);
}
VERIFY(0x020C9D04, &Act_c::cc_set);

void Act_c::set_wind_vec() {
  WWHD_FUNC(0x020C9674, void, this);
  gabi::call(0x028E8E64, &mWindVec, &mWindVec, 0.95f);
  f32 mag = gabi::call<f32>(0x028E8DD0, &mWindVec);
  if (mag < 0.1f) {
    mWindVec.y = 0;
    mWindVec.x = 0;
    mWindVec.z = 0;
  }
  if (!gabi::call<u32>(0x025162A4, &mSph))
    return;
  u32 hit = gabi::call<u32>(0x02516300, &mSph);
  if (!hit || !(gabi::load<u32>(hit + 0x10) & 0x200000))
    return;
  gabi::Local<cXyz> push, dir, v1, v2, v3, sum;
  u32 s = gabi::ea(this);
  push->x = gabi::load<f32>(s + 0x820);
  push->y = gabi::load<f32>(s + 0x824);
  push->z = gabi::load<f32>(s + 0x828);
  f32 sq = gabi::call<f32>(0x028E8DD0, push.get());
  if (sq > 32400) {
    f32 root = gabi::call<f32>(0x028F4384, sq);
    gabi::call(0x028E8E64, push.get(), push.get(), 180 / root);
  }
  u32 vt = gabi::load<u32>(hit + 0x3C), fn = gabi::load<u32>(vt + 0x2C);
  u32 shape = gabi::call<u32>(fn, hit);
  dir->x = gabi::load<f32>(0x101FFBA8);
  dir->z = gabi::load<f32>(0x101FFBB0);
  dir->y = gabi::load<f32>(0x101FFBAC);
  vt = gabi::load<u32>(shape + 0x1C);
  fn = gabi::load<u32>(vt + 0x9C);
  f32 boostY = 1, boostH = 1;
  if (gabi::call<u32>(fn, shape, &current.pos, dir.get())) {
    gabi::call(0x028E8E64, dir.get(), dir.get(), 50.0f);
    gabi::call<f32>(0x028E8DD0, &mWindVec);
    u32 actor = gabi::call<u32>(0x02515BBC, s + 0x7F4);
    if (actor && gabi::load<s16>(actor + 0xE) == 0xA8) {
      s16 angle = gabi::call<s16>(0x020195B0, (f32)dir->x, (f32)dir->z);
      u16 delta = gabi::load<s16>(actor + 0x32A) - angle;
      f32 cosine = gabi::load<f32>(0x104A44FC + ((u32)delta >> 3) * 8);
      if (cosine > 0) {
        boostY = gabi::fmadds(cosine, 0.3f, 1);
        boostH = (cosine + cosine) + 1;
      }
    }
  }
  f32 selector = 0.01f - sq;
  f32 pushScale = selector >= 0 ? 0 : 0.9f, dirScale = selector >= 0 ? 1 : 0.1f;
  gabi::call(0x0201AE48, push.get(), v1.get(), pushScale);
  gabi::call(0x0201AE48, dir.get(), v2.get(), dirScale);
  gabi::call(0x0201AE48, v2.get(), v3.get(), boostH);
  gabi::call(0x0201AD78, v1.get(), sum.get(), v3.get());
  f32 y = sum->y;
  mWindVec.x = sum->x;
  mWindVec.z = sum->z;
  mWindVec.y = y;
  if (std::fabs(y) < 5) {
    f32 launch = (100 * dirScale) * boostY;
    launch = gabi::fmadds(140, pushScale, launch);
    mWindVec.y = y + launch;
  }
  field_0x7A8 = 2;
}
VERIFY(0x020C9674, &Act_c::set_wind_vec);

u32 bomb2HeapCB(Act_c *a) {
  WWHD_FUNC(0x020C8B54, u32, a);
  return gabi::call<u32>(0x020C8B50, a);
}
VERIFY(0x020C8B54, bomb2HeapCB);

} // namespace daBomb2

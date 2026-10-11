/** Bomb actor, ported from zeldaret/tww d_a_bomb3.inc and adapted to WWHD. */
#include "d/actor/d_a_bomb.h"

// HD: GHS emits separate inline-constructor vtables in this translation unit.
const gc_tu_data gc_tu = {
  .safeString = 0x1000A5EC,
  .objAcch = {0x1000A6FC, 0x1000A71C, 0x1000A70C},
  .objAcchDt = {0x1000A70C, 0x1000A71C},
  .gndChk = {0x1000A67C, 0x1000A68C, 0x1000A6AC, 0x1000A69C},
  .gndChkDt = {0x1000A64C, 0x1000A66C, 0x1000A62C},
  .headerStatics = {0x10462674, 0x101922D0, 0x10462688},
};
static const dBgS_GndChk_vt l_lavaVtables = {0x1000A6BC, 0x1000A6CC, 0x1000A6EC, 0x1000A6DC};

static BombAttrTable l_attrType;
static BombStaticPointer l_resourceControl;
static Mtx34 l_currentMtx;
static cXyz l_zero;
static BombCounter l_counter;
static BombStayRoom l_stayRoom;
static BombNoGradeCos l_noGradeCos;
static BombVectorWords l_zeroWords;
static BombFuseOffset l_fuseOffset;
static BombCullSize l_cullSize;
static BombHeaderAngles l_headerAngles;
static BombRandomState l_randomState;
static BombSphereSource l_sphSrc;
static BombSinCosTable l_sinCos;

static BombCheckTable l_checkCc;
static BombCheckTable l_checkBg;

static const u32 l_attrState[] = {0xA1, 0x1C, 0x9C, 0x9C, 0x20C, 0x300, 0x200, 2, 0x21, 0};

static u32 play_bomb() { return gabi::ea(dComIfGp_get()); }

// HD: free actor wrappers and compiler helpers keep their original signatures.
static bool chk_attrState(const daBomb_c *self, u32 mask);
BOOL daBomb_Draw(daBomb_c *self);
BOOL daBomb_IsDelete(daBomb_c *self);
BOOL daBomb_Execute(daBomb_c *self);
BOOL daBomb_Delete(daBomb_c *self);
void __sinit_d_a_bomb_cpp();
void bomb_generic_dtor(void *self, s32 flags);
void daBomb_c__dt(daBomb_c *self, s32 flags);
void bomb_acch_virtual(void *self);
daBomb_c *daBomb_c__ct(daBomb_c *self);
cPhs_State daBomb_Create(daBomb_c *self);
BOOL daBomb_createHeap(daBomb_c *self);

static void drawBombModel(daBomb_c *self) {
  dComIfGd_setListP1();
  mDoExt_modelUpdateDL(self->mpModel, 0);
  dComIfGd_setList();
}

// HD: the embedded animation/matrix-calculator construction retains its GHS write order.
static void initBombBck(u32 receiver) {
  bomb_J3DFrameCtrl_init(receiver, 0);
  gabi::store<u32>(receiver + 0x10, 0x1016E54C);
  bomb_J3DAnmCurve_ct(receiver + 0x14);
  gabi::store<u32>(receiver + 0x84, 0);
  gabi::store<u32>(receiver + 0x88, 0);
  gabi::store<u32>(receiver + 0x80, 0);
  gabi::store<u32>(receiver + 0x58, 0);
  gabi::store<u32>(receiver + 0x10, 0x1000A604);
  gabi::store<u32>(receiver + 0x7C, 0);
  gabi::store<u32>(receiver + 0x48, 0x1016D820);
}

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

static u32 bombGetResource(daBomb_c *self, u32 index) {
  gabi::Local<BombSafeString> name;
  name->text = gabi::ea(l_attrType.mAttr[(s32)self->mType].resName.get());
  name->vt = gc_tu.safeString;
  return bomb_dRes_control_c_getRes((u32)l_resourceControl.mValue, name.get(),
                         index);
}
static void bombAssert(u32 value, u32 line, u32 message) {
  if (!value)
    bomb_JUTAssertion_showAssert(gabi::guest_addr("d_a_bomb3.inc"), line, message);
}

/* 020C8084 */
void daBomb_fuseSmokeEcallBack::execute(void *emitter) {
}

/* 020C8088 */
void daBomb_fuseSmokeEcallBack::draw(void *emitter) {
}

/* 020C808C */
void daBomb_fuseSmokeEcallBack::drawAfter(void *emitter) {
}

/* 020C8090 */
void daBomb_fuseSmokeEcallBack::cleanup(void *emitter) {
}

/* 020C8094 */
// HD: the callback setup ABI omits GC angle and room arguments.
void daBomb_fuseSmokeEcallBack::setup(void *emitter, cXyz *pos) {
  mpPos = pos;
  mpEmitter = emitter;
}

/* 020C80A0 */
void daBomb_fuseSparksEcallBack::draw(void *emitter) {
}

/* 020C80A4 */
// HD: the callback setup ABI omits GC angle and room arguments.
void daBomb_fuseSparksEcallBack::setup(void *emitter, cXyz *pos) {
  mpPos = pos;
  mpEmitter = emitter;
}

/* 020C4260 */
static bool chk_attrState(const daBomb_c *self, u32 mask) {
  s32 state = self->prm_get_state();
  return (l_attrState[state] & mask) != 0;
}

/* 020C6860 */
void daBomb_fuseSmokeEcallBack::executeAfter(void *emitter) {
  BombEmitter* e = static_cast<BombEmitter*>(emitter);
  f32 z = mpPos->z, x = mpPos->x, y = mpPos->y;
  f32 px = field_0x0C->x, py = field_0x0C->y, pz = field_0x0C->z;
  u8 type = (u8)e->mVolumeType;
  e->mGlobalTranslation.x = x;
  e->mGlobalTranslation.y = y;
  e->mGlobalTranslation.z = z;
  if (type >= 7)
    e->mGlobalTranslation.y = -(f32)e->mGlobalTranslation.y;
  f32 distance = bomb_PSVECSquareDistance(mpPos.get(), field_0x0C.get());
  distance = bomb_sqrtf(distance);
  s16 life = gabi::ftoi(bombSmokeFma(20 - distance, 0.5f, 10));
  if (life < 10)
    life = 10;
  e->mLifeTime = life;
  f32 dx = bombSmokeSub(x, px) * 0.5f, dy = bombSmokeSub(y, py) * 0.5f,
      dz = bombSmokeSub(z, pz) * 0.5f;
  f32 tx = (px - field_0x10->x) * 0.5f, ty = (py - field_0x10->y) * 0.5f,
      tz = (pz - field_0x10->z) * 0.5f;
  distance = bomb_PSVECSquareDistance(mpPos.get(), field_0x0C.get());
  distance = bomb_sqrtf(distance);
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
      e->mLifeTime = lifetime;
      u32 particle = bomb_JPABaseEmitter_createParticle(gabi::ea(e));
      if (particle) {
        gabi::at<BombParticle>(particle)->mPosition.x = ox;
        gabi::at<BombParticle>(particle)->mPosition.y = oy;
        gabi::at<BombParticle>(particle)->mPosition.z = oz;
      }
    }
  }
  field_0x04 = life;
}

/* 020C6C04 */
void daBomb_fuseSparksEcallBack::execute(void *emitter) {
  BombEmitter* e = static_cast<BombEmitter*>(emitter);
  f32 y = mpPos->y, z = mpPos->z, x = mpPos->x;
  u8 type = (u8)e->mVolumeType;
  u32 link = (u32)e->mAlivePtclHead;
  e->mGlobalTranslation.z = z;
  e->mGlobalTranslation.x = x;
  e->mGlobalTranslation.y = y;
  if (type >= 7)
    e->mGlobalTranslation.y = -(f32)e->mGlobalTranslation.y;
  while (link) {
    u32 next = (u32)gabi::at<BombParticleNode>(link)->mNext, particle = (u32)gabi::at<BombParticleNode>(link)->mObject;
    gabi::at<BombParticle>(particle)->mPosition.z = z;
    gabi::at<BombParticle>(particle)->mPosition.y = y;
    gabi::at<BombParticle>(particle)->mPosition.x = x;
    link = next;
  }
}

/* 020C4398 */
void daBomb_c::draw_norm() {
  // HD: Link owns the bomb BRK curve and draws an additional bomb-model packet.
  u32 player = gabi::ea(dComIfGp_getLinkPlayer());
  s16 end = (s16)gabi::at<BombLinkView>(player)->mBombBrk.mEnd;
  f32 frame = (s32)end - (s32)mRestTime + 2;
  if (frame < 0)
    frame = 0;
  else if (frame >= (f32)end)
    frame = (f32)end - 0.001f;
  u32 brk = player + 0x44D4;
  u32 animation = (u32)gabi::at<BombLinkBrk>(brk)->mpAnm;
  gabi::at<BombLinkBrk>(brk)->mFrame = frame;
  gabi::at<BombAnmTransform>(animation)->mFrame = frame;
  u32 curve = (u32)gabi::at<BombLinkBrk>(brk)->mpCurve;
  u32 function = gabi::at<BombCurve>(curve)->mCalc.get();
  u32 object = (u32)gabi::at<BombCurve>(curve)->mObject;
  f32 scale = (f32)gabi::at<BombCurve>(curve)->mScale, offset = (f32)gabi::at<BombCurve>(curve)->mOffset;
  f32 result = gabi::call_ptr<f32>(function, object, frame, scale, offset);
  gabi::at<BombCurve>(curve)->mValue = result;
  bomb_J3DAnmCurve_update(brk + 0x20);
  u32 bck = (u32)reinterpret_cast<be<u32>*>(&mBck0)[0x88 / 4];
  u32 vt = gabi::ea(gabi::at<BombAnmTransform>(bck)->__vtbl.get());
  s32 length = gabi::call_ptr<s32>(gabi::at<BombAnmVtable>(vt)->getFrameMax.get(), bck);
  frame = length - (s32)mRestTime;
  if (frame < 0)
    frame = 0;
  else if (frame >= (f32)length)
    frame = (f32)length - 0.001f;
  mBck0.entry(J3DModel_getModelData(mpModel), frame);
  drawBombModel(this);
  player = gabi::ea(dComIfGp_getLinkPlayer());
  bomb_Link_drawBombExtra(player, gabi::ea(hdExtra), mpModel.get());
}

/* 020C42A8 */
void daBomb_c::draw_nut() {
  if (chk_state(STATE_5) || chk_state(STATE_6)) {
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
  dComIfGd_setListP1();
  mDoExt_modelUpdateDL(mpModel, 0);
  dComIfGd_setList();
}

/* 020C4660 */
BOOL daBomb_c::draw() {
  // HD: the clipper and any nonzero water-effect flag suppress drawing; GC simple shadow is absent.
  if (chk_attrState(this, daBomb_c::ATTR_STATE_20))
    return TRUE;
  if (chk_state(STATE_4) && field_0x77D != 0)
    return TRUE;
  gabi::Local<cXyz> position;
  u32 self = gabi::ea(this);
  for (u32 i = 0; i < 3; i++)
    reinterpret_cast<BombVectorWords*>(position.get())->words[i] =
        (u32)reinterpret_cast<BombVectorWords*>(&current.pos)->words[i];
  if (bomb_J3DUClipper_clip(0x1048CFF0, 0x104B45F8, position.get(),
                       80.0f))
    return TRUE;
  if (field_0x7C8 > 0)
    return TRUE;
  u32 env = bomb_dKy_getEnvlight();
  bomb_dScnKy_env_light_c_settingTevStruct(env, 0, &current.pos, &tevStr);
  env = bomb_dKy_getEnvlight();
  bomb_dScnKy_env_light_c_setLightTevColorType(env, mpModel.get(), &tevStr);
  if (mType == 1)
    draw_nut();
  else
    draw_norm();
  return TRUE;
}

/* 020C4758 */
BOOL daBomb_Draw(daBomb_c *self) {
  return self->draw();
}

/* 020C77F4 */
bool daBomb_c::checkExplodeCc_norm() {
  bool explode = false;
  if (mSph.ChkTgHit()) {
    void *obj = mSph.GetTgHitObj();
    if (obj) {
      u32 type = (u32)static_cast<dCcD_GObjInf*>(obj)->mAtType;
      explode = (type & 0x20) || !(type & 0x200000);
    }
    mSph.ClrTgHit();
  }
  return explode ? procExplode_init() != 0 : false;
}

/* 020C789C */
bool daBomb_c::checkExplodeCc_nut() {
  bool explode = false, hit = false;
  if (mSph.ChkTgHit()) {
    void *obj = mSph.GetTgHitObj();
    if (obj) {
      u32 type = (u32)static_cast<dCcD_GObjInf*>(obj)->mAtType;
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
    bomb_dCcD_GObjInf_ClrCoHit(&mSph);
  }
  if (explode)
    return procExplode_init() != 0;
  if (hit) {
    change_state(STATE_1);
    setFuseEffect();
    if (mRestTime > 30) {
      mRestTime = 30;
      mBrk1.mFrameCtrl.setFrame(105);
      mBck1.mFrameCtrl.setFrame(105);
    }
  }
  return false;
}

/* 020C79B8 */
bool daBomb_c::checkExplodeCc_cannon() {
  bool explode = false;
  if (mSph.ChkTgHit()) {
    void *obj = mSph.GetTgHitObj();
    if (obj)
      explode = !((u32)static_cast<dCcD_GObjInf*>(obj)->mAtType & 0x200000);
    mSph.ClrTgHit();
  }
  if (mSph.ChkCoHit()) {
    bomb_dCcD_GObjInf_ClrCoHit(&mSph);
    return procExplode_init() != 0;
  }
  return explode ? procExplode_init() != 0 : false;
}

/* 020C6C64 */
u32 daBomb_c::checkExplodeCc() {
  return mRestTime > 0 ? (this->*l_checkCc.mProc[(s32)mType])() : false;
}

/* 020C6CC4 */
bool daBomb_c::checkExplodeTimer() {
  bool explode = false;
  if (!chk_attrState(this, daBomb_c::ATTR_STATE_200) && mRestTime > 0) {
    mRestTime = mRestTime - 1;
    if (mRestTime == 0)
      explode = procExplode_init() != 0;
  }
  return explode;
}

/* 020C6D38 */
bool daBomb_c::checkExplode() {
  return checkExplodeCc() || checkExplodeTimer();
}

/* 020C7C34 */
bool daBomb_c::checkExplodeBg_norm() {
  bool sink = chk_water_sink();
  u8 land = chk_water_land();
  bool burn = chk_lava_hit();
  if (sink)
    field_0x781 = 1;
  if (land)
    eff_water_splash();
  return burn ? procExplode_init() != 0 : false;
}

/* 020C7DC4 */
bool daBomb_c::checkExplodeBg_nut() {
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
    change_state(STATE_1);
    setFuseEffect();
    if (mRestTime > 30) {
      mRestTime = 30;
      mBrk1.mFrameCtrl.setFrame(105);
      mBck1.mFrameCtrl.setFrame(105);
    }
  }
  return false;
}

/* 020C7EEC */
bool daBomb_c::checkExplodeBg_cannon() {
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

/* 020C7090 */
u32 daBomb_c::checkExplodeBg() {
  return (this->*l_checkBg.mProc[(s32)mType])();
}

/* 020C6E44 */
void daBomb_c::water_tention() {
  // HD: GHS fused the height correction, including its NaN operand order.
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

/* 020C6EB4 */
void daBomb_c::posMoveF() {
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
  if (!chk_state(STATE_5) && !chk_state(STATE_6) && field_0x6F3 != 1) {
    water_tention();
    u32 p = play_bomb(), self = gabi::ea(this);
    u16 polygon = (u16)mAcch.m_gnd.mPolyIndex,
        background = (u16)mAcch.m_gnd.mBgIndex;
    u32 plane = bomb_cBgS_GetTriPla(p + 0x12A0, background, polygon);
    f32 magnitude = bomb_PSVECSquareMag(&mWindVec);
    if (magnitude > 0.01f) {
      f32 friction = 0, grade = 0;
      if (plane) {
        grade = (f32)l_noGradeCos.mValue;
        friction = 0.06f;
      }
      bomb_daObj_posMoveF_grade(this, &mStts, &mWindVec, plane, 0, 0.002f, 0.0005f,
                 friction, grade);
    } else
      bomb_fopAcM_posMoveF(this, &mStts);
  }
  if (suspended) {
    s16 timer = mNoGravityTime;
    gravity = originalGravity;
    mNoGravityTime = timer - 1;
  }
}

/* 020C5BAC */
void daBomb_c::bgCrrPos() {
  u32 p = play_bomb();
  bomb_dBgS_Acch_CrrPos(&mAcch, p + 0x12A0);
  bgCrrPos_lava();
  bgCrrPos_water();
  setRoomInfo();
}

/* 020C5980 */
void daBomb_c::bgCrrPos_lava() {
  u32 self = gabi::ea(this);
  f32 z = current.pos.z, y = old.pos.y + 1, x = current.pos.x;
  mGndChk.m_pos.z = z;
  mGndChk.m_pos.x = x;
  mGndChk.m_pos.y = y;
  u32 p = play_bomb();
  field_0x554 = bomb_cBgS_GroundCross(p + 0x12A0, self + 0x880);
}

/* 020C59EC */
void daBomb_c::bgCrrPos_water() {
  f32 waterheight = (f32)mAcch.m_wtr.m_height;
  s32 area =
      bomb_daSea_ChkArea((f32)current.pos.x, (f32)current.pos.z);
  f32 wave =
      bomb_daSea_calcWave((f32)current.pos.x, (f32)current.pos.z);
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

/* 020C7AEC */
u8 daBomb_c::chk_water_land() {
  return field_0x560;
}

/* 020C6E3C */
u8 daBomb_c::chk_water_in() {
  return mbWaterIn;
}

/* 020C7A98 */
bool daBomb_c::chk_water_sink() {
  return chk_water_in() && field_0x558 - current.pos.y > 30.0f;
}

/* 020C7AF4 */
bool daBomb_c::chk_lava_hit() {
  return field_0x554 != -1000000000.0f && current.pos.y < field_0x554;
}

/* 020C7EB0 */
bool daBomb_c::chk_dead_zone() {
  return mAcch.m_ground_h == -1000000000.0f && field_0x558 == -1000000000.0f &&
         field_0x554 == -1000000000.0f;
}

/* 020C71FC */
void daBomb_c::bound(f32 previousSpeed) {
  u32 flags = mAcch.m_flags;
  if (flags & 0x10) {
    s16 angle = mCir.m_wall_angle_y;
    f32 horizontal = speedF * 0.8f;
    flags = mAcch.m_flags;
    speedF = horizontal;
    current.angle.y = (angle * 2) - (current.angle.y + 0x8000);
  }
  if (flags & 0x80) {
    bomb_daObj_make_land_effect(this, gabi::ea(&mAcch.m_gnd), 0.6f);
    previousSpeed = previousSpeed * -0.6f;
    if (previousSpeed < 19.5f)
      field_0x780 = 0;
    else {
      speedF = speedF * 0.9f;
      f32 difference = 13 - previousSpeed;
      speed.y = difference >= 0 ? previousSpeed : 13;
    }
  } else if (flags & 0x20)
    bomb_cLib_addCalc(&speedF, 0.0f, 0.5f, 5.0f, 1.0f);
}

/* 020C4B10 */
void daBomb_c::set_real_shadow_flag() {
  bool state2 = chk_state(STATE_2), state3 = chk_state(STATE_3), shadow = false;
  u32 self = gabi::ea(this);
  s32 timer = field_0x7C8;
  if (timer <= 1 && (state2 || state3)) {
    if ((u32)model || state3 || timer == 1)
      shadow = true;
    else {
      u32 player = gabi::ea(dComIfGp_getPlayer(0));
      u32 vt = (u32)gabi::at<BombPlayerView>(player)->__vtbl;
      u32 grabbed = gabi::call_ptr<u32>(gabi::at<BombPlayerVtable>(vt)->getGrabActorID.get(), player);
      if (grabbed == (u32)mBsPcId &&
          ((u32)gabi::at<BombPlayerView>(player)->mResetFlg0 & 0x8000))
        shadow = true;
    }
  }
  if (field_0x7C8 > 0)
    field_0x7C8 = field_0x7C8 - 1;
  model = shadow ? gabi::ea(mpModel.get()) : 0;
}

/* 020C5B1C */
void daBomb_c::setRoomInfo() {
  u32 self = gabi::ea(this);
  u8 room;
  if (mAcch.m_ground_h != -1000000000.0f) {
    u32 p = play_bomb();
    s32 id = bomb_dBgS_GetRoomId(p + 0x12A0, self + 0x764);
    p = play_bomb();
    u32 color = bomb_dBgS_GetPolyColor(p + 0x12A0, self + 0x764);
    room = id;
    current.roomNo = room;
    tevStr.mRoomNo = room;
    tevStr.mEnvrIdxOverride = color;
    mStts.mRoomId = room;
  } else {
    room = (u8)l_stayRoom.mValue;
    tevStr.mRoomNo = room;
    mStts.mRoomId = room;
    current.roomNo = room;
  }
}

/* 020C475C */
void daBomb_c::makeFireEffect(cXyz *position, csXyz *angle) {
  // HD: retain both play lookups even though their camera result is unused.
  play_bomb();
  play_bomb();
  angle->x = angle->x + 0x4000;
  gabi::Local<cXyz> effectScale;
  effectScale->set(0.3f, 0.3f, 0.3f);
  u32 controller = gabi::ea(dComIfGp_getParticle());
  bomb_dPa_control_c_setBombSmoke(controller, BombParticleId::ID_IT_JT_BMEX_SMOKE02, position, angle, effectScale.get(),
             255);
  u32 p = play_bomb();
  gabi::Local<cXyz> direction;
  direction->set(0, 1, 0);
  bomb_dVibration_c_StartShock(p + 0x599C, 7, -33, direction.get());
}

/* 020C7CC0 */
void daBomb_c::makeWaterEffect() {
  if (field_0x77D)
    return;
  s32 reverb = bomb_dComIfGp_getReverb((s8)current.roomNo);
  mDoAud_seStart(0x6982, &eyePos, 0, reverb);
  bomb_fopKyM_createWpillar(&current.pos, 1, 1.0f, 1.0f);
  u32 self = gabi::ea(this);
  u32 co = (u32)mSph.mObjCo.mSPrm, at = (u32)mSph.mObjAt.mSPrm,
      tg = (u32)mSph.mObjTg.mSPrm;
  mSph.mObjCo.mSPrm = co & ~1;
  mSph.mObjAt.mSPrm = at | 1;
  mSph.mObjTg.mSPrm = tg & ~1;
  mSph.SetR(200);
  mSph.SetC(&current.pos);
  if ((u32)mMassCounter != (u32)l_counter.mValue) {
    u32 p = play_bomb();
    bomb_cCcS_Set(p + 0x26A4, &mSph);
    p = play_bomb();
    bomb_dCcMassS_Mng_Set(p + 0x4EF8, &mSph, 3);
    field_0x782 = 1;
    mMassCounter = (s32)(u32)l_counter.mValue;
  }
  field_0x77D = 1;
}

/* 020C584C */
void daBomb_c::setFuseEffect() {
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
  u32 controller = gabi::ea(dComIfGp_getParticle());
  bomb_dPa_control_c_set(controller, 1, BombParticleId::ID_IT_JN_BM_HIBANA, &mFusePos, 0, &scale, 255,
             &mSparks, -1, 0, 0, 0);
  u32 env = bomb_dKy_getEnvlight();
  bomb_dScnKy_env_light_c_settingTevStruct(env, 0, &mFusePos, &tevStr);
  s8 room = current.roomNo;
  controller = gabi::ea(dComIfGp_getParticle());
  bomb_dPa_control_c_set(controller, 3, BombParticleId::ID_IT_JT_BM_SMOKE, &mFusePos, 0, &scale, 220,
             &mSmoke, room, gabi::ea(&tevStr.mColorK0), gabi::ea(&tevStr.mColorK0), 0);
  mSmoke.field_0x0C = &mFusePos2;
  mSmoke.field_0x04 = 20;
  mSmoke.field_0x10 = &mFusePos3;
}

/* 020C5D54 */
void daBomb_c::eff_explode_normal(const csXyz *angle) {
  u32 c = gabi::ea(dComIfGp_getParticle());
  bomb_dPa_control_c_set(c, 1, BombParticleId::ID_IT_JN_BMEX_SENKO, &current.pos, angle, &scale, 255, 0, -1, 0,
             0, 0);
  c = gabi::ea(dComIfGp_getParticle());
  bomb_dPa_control_c_setBombSmoke(c, BombParticleId::ID_IT_JT_BMEX_SMOKE01, &current.pos, 0, &scale, 255);
  c = gabi::ea(dComIfGp_getParticle());
  bomb_dPa_control_c_setBombSmoke(c, BombParticleId::ID_IT_JT_BMEX_SMOKE02, &current.pos, 0, &scale, 255);
  c = gabi::ea(dComIfGp_getParticle());
  bomb_dPa_control_c_set(c, 3, BombParticleId::ID_IT_JT_BMEX_HAHEN, &current.pos, 0, &scale, 255, 0, -1, 0,
             0, 0);
}

/* 020C5BFC */
void daBomb_c::eff_explode_cheap(const csXyz *angle) {
  u32 c = gabi::ea(dComIfGp_getParticle());
  u32 emitter = bomb_dPa_control_c_set_result(c, 1, BombParticleId::ID_IT_JN_BMEX_SENKO, &current.pos, angle,
                                &scale, 255, 0, -1, 0, 0, 0);
  if (emitter) {
    gabi::at<BombEmitter>(emitter)->mGlobalParticleScale.z = 1;
    gabi::at<BombEmitter>(emitter)->mGlobalParticleScale.y = 0.67f;
    gabi::at<BombEmitter>(emitter)->mGlobalParticleScale.x = 0.5f;
    gabi::at<BombEmitter>(emitter)->mLifeTime = 12;
  }
  c = gabi::ea(dComIfGp_getParticle());
  bomb_dPa_control_c_setBombSmoke(c, BombParticleId::ID_IT_JT_BMEX_SMOKELIGHT, &current.pos, 0, &scale, 255);
  c = gabi::ea(dComIfGp_getParticle());
  emitter =
      bomb_dPa_control_c_setBombSmoke_result(c, BombParticleId::ID_IT_JT_BMEX_SMOKE02, &current.pos, 0, &scale, 255);
  if (emitter)
    gabi::at<BombEmitter>(emitter)->mLifeTime = 70;
  c = gabi::ea(dComIfGp_getParticle());
  emitter = bomb_dPa_control_c_set_result(c, 3, BombParticleId::ID_IT_JT_BMEX_HAHEN, &current.pos, 0, &scale,
                            255, 0, -1, 0, 0, 0);
  if (emitter) {
    gabi::at<BombEmitter>(emitter)->mInitialVelAxis = 25;
    gabi::at<BombEmitter>(emitter)->mInitialVelDir = 35;
    gabi::at<BombEmitter>(emitter)->mLifeTime = 70;
  }
}

/* 020C5E4C */
void daBomb_c::eff_explode() {
  s8 id = dComIfGp_getPlayerCameraID(0);
  u32 camera = gabi::ea(dComIfGp_getCamera(id));
  gabi::Local<csXyz> angle;
  angle->x = -(s16)gabi::at<BombCameraView>(camera)->mAngle.x;
  angle->z = 0;
  angle->y = (s16)gabi::at<BombCameraView>(camera)->mAngle.y + 0x8000;
  if (bomb_daBomb_c_prm_get_cheapEff(this))
    eff_explode_cheap(angle.get());
  else
    eff_explode_normal(angle.get());
}

/* 020C5EF8 */
BOOL daBomb_c::procExplode_init() {
  u32 self = gabi::ea(this);
  s8 id = dComIfGp_getPlayerCameraID(0);
  u32 camera = gabi::ea(dComIfGp_getCamera(id));
  bomb_dCamera_c_ForceLockOff(camera + 0x248, (u32)mBsPcId);
  u32 water = 0;
  if (chk_state(STATE_8)) {
    gabi::Local<be<f32>> height;
    water = bomb_fopAcM_getWaterY(&current.pos, height.get());
    if (water) {
      gabi::Local<dBgS_GndChk> ground;
      u32 g = gabi::ea(ground.get());
      bomb_cBgS_GndChk_cBgS_GndChk(g);
      ground->mbBoomerangThrough = 0;
      ground->mbArrowThrough = 0;
      ground->m_pos.x = current.pos.x;
      ground->m_pos.y = current.pos.y + 1;
      ground->m_pos.z = current.pos.z;
      ground->cBgS_PolyInfo::__vtbl = gc_tu.gndChk.v20;
      ground->dBgS_GrpPassChk::__vtbl = gc_tu.gndChk.v4C;
      ground->mbCamThrough = 0;
      ground->mbRopeThrough = 0;
      ground->mGrpPassChk = g + 0x4C;
      ground->mPolyPassChk = g + 0x40;
      ground->cBgS_Chk::__vtbl = gc_tu.gndChk.v10;
      ground->mbBombThrough = 0;
      ground->dBgS_PolyPassChk::__vtbl = gc_tu.gndChk.v40;
      ground->mGrp = 1;
      ground->mbLinkThrough = 0;
      ground->mbObjThrough = 1;
      u32 p = play_bomb();
      f32 cross = bomb_cBgS_GroundCross(p + 0x12A0, g);
      if (cross > *height.get() || current.pos.y > *height.get() + 50)
        water = 0;
      ground->cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
      ground->dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
      ground->dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
      bomb_cBgS_Chk_cBgS_Chk(g, 0);
    }
  }
  if (!water)
    eff_explode();
  else
    bomb_fopKyM_createWpillar(&current.pos, 1, 1.0f, 1.0f);
  for (gptr<void>* emitterSlot : {&mSmoke.mpEmitter, &mSparks.mpEmitter}) {
    u32 emitter = gabi::ea(emitterSlot->get());
    if (emitter) {
      gabi::at<BombEmitter>(emitter)->mpEmitterCallBack = 0;
      emitter = gabi::ea(emitterSlot->get());
      u32 flags = (u32)gabi::at<BombEmitter>(emitter)->mStatus;
      gabi::at<BombEmitter>(emitter)->mMaxFrame = -1;
      gabi::at<BombEmitter>(emitter)->mStatus = flags | 1;
    }
    *emitterSlot = nullptr;
  }
  mPntLight.mColorG = 200;
  mPntLight.mColorR = 200;
  mPntLight.mPos.x = current.pos.x;
  mPntLight.mColorB = 160;
  mPntLight.mPower = 600;
  mPntLight.mPos.y = current.pos.y + 100;
  mPntLight.mFluctuation = 100;
  mPntLight.mPos.z = current.pos.z;
  bomb_dKy_efplight_set(&mPntLight);
  mPntWind.mDir.y = 1;
  mPntWind.mRadius = 500;
  mPntWind.mStrength = 0.5f;
  mPntWind.mDir.z = 0;
  mPntWind.mDir.x = 0;
  for (u32 i = 0; i < 3; i++)
    reinterpret_cast<BombVectorWords*>(&mPntWind.mPos)->words[i] =
        (u32)reinterpret_cast<BombVectorWords*>(&current.pos)->words[i];
  mPntWind.field_0x20 = 0;
  bomb_dKyw_pntwind_set(&mPntWind);
  speedF = 0;
  mFunc.d = 0;
  field_0x774 = 0;
  field_0x778 = 0;
  mFunc.f = 0x020C74A0;
  mFunc.i = -1;
  for (u32 i = 0; i < 3; i++)
    reinterpret_cast<BombVectorWords*>(&speed)->words[i] = (u32)l_zeroWords.words[i];
  gravity = 0;
  if (!chk_state(STATE_8))
    change_state(STATE_0);
  attention_info.flags = (u32)attention_info.flags & ~0x10;
  if (field_0x6F0) {
    u32 player = gabi::ea(dComIfGp_getLinkPlayer());
    u8 count = (u8)gabi::at<BombLinkView>(player)->mBombCnt;
    if (count)
      gabi::at<BombLinkView>(player)->mBombCnt = count - 1;
    field_0x6F0 = 0;
  }
  mSph.mObjAt.mSPrm = (u32)mSph.mObjAt.mSPrm | 1;
  mSph.mObjTg.mSPrm = (u32)mSph.mObjTg.mSPrm & ~1;
  mSph.mObjCo.mSPrm = (u32)mSph.mObjCo.mSPrm & ~1;
  mSph.SetR(200);
  mSph.SetC(&current.pos);
  if (mMassCounter != (u32)l_counter.mValue) {
    u32 p = play_bomb();
    bomb_cCcS_Set(p + 0x26A4, &mSph);
    p = play_bomb();
    bomb_dCcMassS_Mng_Set(p + 0x4EF8, &mSph, 3);
    field_0x782 = 1;
    mMassCounter = (u32)l_counter.mValue;
  }
  s32 reverb = bomb_dComIfGp_getReverb((s8)current.roomNo);
  bomb_mDoAud_seStartLevel(water ? 0x6982 : 0x6901, self + 0x37C, 0, reverb);
  bomb_fopAcM_cancelCarryNow(this);
  gabi::Local<cXyz> position;
  position->x = current.pos.x;
  position->y = current.pos.y;
  position->z = current.pos.z;
  bomb_dKy_Sound_set(position.get(), 255, (u32)mBsPcId, 10);
  mRestTime = 0;
  field_0x6FE = 2;
  u32 p = play_bomb();
  position->x = 0;
  position->y = 1;
  position->z = 0;
  bomb_dVibration_c_StartShock(p + 0x599C, 7, -33, position.get());
  se_cannon_fly_stop();
  return TRUE;
}

/* 020C74A0 */
bool daBomb_c::procExplode() {
  // HD: unordered fade comparisons advance the state as the original floating branches do.
  u32 p = play_bomb();
  f32 intensity = field_0x778;
  mPntLight.mPower = 1500 * intensity;
  mPntWind.mStrength = intensity;
  u32 camera = gabi::at<dComIfG_inf_c>(p)->play.mCameraInfo[0].mpCamera;
  f32 distance = bomb_PSVECSquareDistance(&current.pos, camera + 0xDC);
  distance = bomb_sqrtf(distance);
  f32 factor = 0;
  if (distance < 1500)
    factor = 1 - distance / 1500;
  bomb_dKy_actor_addcol_amb_set(200, 180, 100, (f32)field_0x778 * factor);
  bomb_dKy_bg_addcol_amb_set(180, 160, 60, (f32)field_0x778 * factor);
  bomb_dKy_bg_addcol_dif_set(255, 225, 120, (f32)field_0x778 * factor);
  switch (field_0x774) {
  case 0:
    bomb_cLib_addCalc(&field_0x778, 1.0f, 0.5f, 0.4f, 0.01f);
    if (!(field_0x778 < 0.99f))
      field_0x774 = field_0x774 + 1;
    break;
  case 1:
    bomb_cLib_addCalc(&field_0x778, 0.0f, 0.05f, 0.04f, 0.001f);
    if (!(field_0x778 > 0.01f))
      field_0x774 = field_0x774 + 1;
    break;
  }
  if (field_0x6FE) {
    field_0x6FE = field_0x6FE - 1;
    mSph.SetC(&current.pos);
    if (mMassCounter != (u32)l_counter.mValue) {
      p = play_bomb();
      bomb_cCcS_Set(p + 0x26A4, &mSph);
      p = play_bomb();
      bomb_dCcMassS_Mng_Set(p + 0x4EF8, &mSph, 3);
      field_0x782 = 1;
      mMassCounter = (u32)l_counter.mValue;
    }
  } else if (field_0x774 > 1)
    field_0x781 = 1;
  return true;
}

/* 020C6D90 */
bool daBomb_c::procCarry_init() {
  if (chk_attrState(this, daBomb_c::ATTR_STATE_100))
    setFuseEffect();
  u32 self = gabi::ea(this);
  mFunc.d = 0;
  mFunc.f = 0x020C76E0;
  mFunc.i = -1;
  change_state(STATE_2);
  speedF = 0;
  for (u32 i = 0; i < 3; i++)
    reinterpret_cast<BombVectorWords*>(&speed)->words[i] = (u32)l_zeroWords.words[i];
  attention_info.flags = (u32)attention_info.flags & ~0x10;
  mSph.mObjCo.mSPrm = (u32)mSph.mObjCo.mSPrm & ~1;
  return true;
}

/* 020C76E0 */
u32 daBomb_c::procCarry() {
  if (!((u32)actor_status & 0x2000)) {
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

/* 020C6394 */
bool daBomb_c::procWait_init() {
  mFunc.d = 0;
  mFunc.i = -1;
  mFunc.f = 0x020C7358;
  if (chk_attrState(this, daBomb_c::ATTR_STATE_80))
    change_state(STATE_1);
  mSph.mObjCo.mSPrm = (u32)mSph.mObjCo.mSPrm | 1;
  return true;
}

/* 020C7358 */
u32 daBomb_c::procWait() {
  // HD: proc dispatch forwards the complete r3 result rather than narrowing it to bool.
  if (checkExplode())
    return 1;
  u32 self = gabi::ea(this);
  if ((u32)actor_status & 0x2000)
    return bomb_daBomb_c_procCarry_init(this);
  posMoveF();
  f32 yspeed = speed.y;
  bgCrrPos();
  if (checkExplodeBg())
    return 1;
  if (chk_state(STATE_4)) {
    if (waitState_cannon())
      return 1;
  } else if (chk_state(STATE_7))
    waitState_bomtyu();
  if (field_0x781)
    return 1;
  bound(yspeed);
  u32 flags = mAcch.m_flags;
  bool ground = (flags & 0x20) != 0;
  if (!ground) {
    ground = chk_state(STATE_5);
    flags = mAcch.m_flags;
  }
  u32 attention = (u32)attention_info.flags;
  attention_info.flags = ground && !(flags & 0x80) ? attention | 0x10
                                                           : attention & ~0x10;
  return 1;
}

/* 020C7118 */
bool daBomb_c::waitState_cannon() {
  se_cannon_fly_set();
  return false;
}

/* 020C713C */
void daBomb_c::waitState_bomtyu() {
  cLib_chaseF(&scale.x, 1, 0.05f);
  cLib_chaseF(&scale.y, 1, 0.05f);
  cLib_chaseF(&scale.z, 1, 0.05f);
  if (mBombFire) {
    setFuseEffect();
    mBombFire = 0;
    field_0x6F4 = 1;
    change_state(STATE_1);
  }
}

/* 020C4D84 */
BOOL daBomb_c::execute() {
  if (!(u32)l_fuseOffset.mGuard) {
    l_fuseOffset.mOffset.z = 5;
    l_fuseOffset.mOffset.y = 60;
    l_fuseOffset.mOffset.x = 0;
    l_fuseOffset.mGuard = 1;
  }
  if (chk_state(STATE_4) && !field_0x77C) {
    f32 y = current.pos.y, x = current.pos.x, z = current.pos.z;
    gabi::Local<cXyz> position;
    position->set(x, y + 20, z);
    field_0x77C = 1;
    makeFireEffect(position.get(), &shape_angle);
  }
  set_wind_vec();
  u32 self = gabi::ea(this);
  if ((s16)mFunc.i != 0)
    (this->*mFunc)();
  if (field_0x781)
    bomb_fopAcM_delete(this);
  else {
    set_real_shadow_flag();
    if (mType == 1)
      anm_play_nut();
    bomb_dCcD_GStts_Move(self + 0x900);
    f32 x = current.pos.x, z = current.pos.z, y = current.pos.y + 50;
    attention_info.position.z = z;
    attention_info.position.x = x;
    eyePos.x = x;
    eyePos.y = y;
    eyePos.z = z;
    attention_info.position.y = y;
    if (!chk_attrState(this, daBomb_c::ATTR_STATE_20)) {
      set_mtx();
      u32 oldFuse[3], oldSecond[3];
      for (u32 i = 0; i < 3; i++) {
        oldFuse[i] = (u32)reinterpret_cast<BombVectorWords*>(&mFusePos)->words[i];
        oldSecond[i] = (u32)reinterpret_cast<BombVectorWords*>(&mFusePos2)->words[i];
      }
      for (u32 i = 0; i < 3; i++) {
        reinterpret_cast<BombVectorWords*>(&mFusePos3)->words[i] = oldSecond[i];
        reinterpret_cast<BombVectorWords*>(&mFusePos2)->words[i] = oldFuse[i];
      }
      PSMTXMultVec(&l_currentMtx, &l_fuseOffset.mOffset,
                   &mFusePos);
      y = current.pos.y;
      x = current.pos.x;
      z = current.pos.z;
      gabi::Local<cXyz> center;
      center->set(x, y + 30, z);
      mSph.SetC(center.get());
      mSph.SetR(scale.x * 30);
      if ((u32)mMassCounter != (u32)l_counter.mValue) {
        u32 p = play_bomb();
        bomb_cCcS_Set(p + 0x26A4, &mSph);
        p = play_bomb();
        bomb_dCcMassS_Mng_Set(p + 0x4EF8, &mSph, 3);
        mMassCounter = (s32)(u32)l_counter.mValue;
      }
    }
    if (chk_attrState(this, daBomb_c::ATTR_STATE_10) || field_0x6F4 == 1) {
      s32 reverb = bomb_dComIfGp_getReverb((s8)current.roomNo);
      mDoAud_seStart(0x6100, &eyePos, 0, reverb);
    }
  }
  if (field_0x782)
    field_0x784 = (u32)field_0x784 + 1;
  return TRUE;
}

/* 020C506C */
BOOL daBomb_Execute(daBomb_c *self) {
  return self->execute();
}

/* 020C480C */
void daBomb_c::set_wind_vec() {
  bomb_PSVECScale(&mWindVec, &mWindVec, 0.95f);
  if (bomb_PSVECSquareMag(&mWindVec) < 0.1f) {
    mWindVec.y = 0;
    mWindVec.x = 0;
    mWindVec.z = 0;
  }
  if (!mSph.ChkTgHit())
    return;
  void *hit = mSph.GetTgHitObj();
  if (!hit || !((u32)static_cast<dCcD_GObjInf*>(hit)->mAtType & 0x200000))
    return;
  gabi::Local<cXyz> recoil;
  u32 self = gabi::ea(this);
  recoil->set((f32)mSph.mGObjTg.mRVec.x, (f32)mSph.mGObjTg.mRVec.y,
              (f32)mSph.mGObjTg.mRVec.z);
  f32 magnitude = bomb_PSVECSquareMag(recoil.get());
  if (magnitude > 32400) {
    f32 length = bomb_sqrtf(magnitude);
    bomb_PSVECScale(recoil.get(), recoil.get(), 180.0f / length);
  }
  u32 vt = gabi::ea(static_cast<BombHitView*>(hit)->__vtbl.get());
  u32 shape = gabi::call_ptr<u32>(gabi::at<BombHitVtable>(vt)->GetShapeAttr.get(), hit);
  gabi::Local<cXyz> normal;
  normal->set((f32)l_zero.x, (f32)l_zero.y,
              (f32)l_zero.z);
  f32 normalFactor = 1, verticalFactor = 1;
  vt = gabi::ea(gabi::at<BombShape>(shape)->__vtbl.get());
  if (gabi::call_ptr<BOOL>(gabi::at<BombShapeVtable>(vt)->GetNVec.get(), shape, &current.pos,
                           normal.get())) {
    bomb_PSVECScale(normal.get(), normal.get(), 50.0f);
    bomb_PSVECSquareMag(&mWindVec);
    u32 actor = bomb_dCcD_GAtTgCoCommonBase_GetAc(self + 0x9B4);
    if (actor && (s16)gabi::at<fopAc_ac_c>(actor)->mProfName == 0xA8) {
      s32 angle = bomb_cM_atan2s((f32)normal->x, (f32)normal->z);
      s16 yaw = (s16)gabi::at<fopAc_ac_c>(actor)->shape_angle.y;
      u32 lookup = (u16)(yaw - angle) >> 3;
      f32 cosine = l_sinCos.mSinCos[lookup][1];
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
  bomb_cXyz___ml(recoil.get(), scaledRecoil.get(), recoilWeight);
  bomb_cXyz___ml(normal.get(), scaledNormal.get(), normalWeight);
  bomb_cXyz___ml(scaledNormal.get(), weightedNormal.get(),
             normalFactor);
  bomb_cXyz___pl(scaledRecoil.get(), sum.get(), weightedNormal.get());
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

/* 020C4C04 */
void daBomb_c::anm_play_nut() {
  if (chk_state(STATE_5) || chk_state(STATE_6)) {
    mBck0.play();
    mBrk0.play();
  } else if (mRestTime + 2 <= 135) {
    mBck1.play();
    mBrk1.play();
  }
}

/* 020C4C84 */
void daBomb_c::set_mtx() {
  u32 model = gabi::ea(mpModel.get());
  f32 z = scale.z, x = scale.x, y = scale.y;
  gabi::at<J3DModel>(model)->mBaseScale.x = x;
  gabi::at<J3DModel>(model)->mBaseScale.z = z;
  gabi::at<J3DModel>(model)->mBaseScale.y = y;
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
  s16 angleX = 0;
  bool zero = bomb_daBomb_c_prm_get_angXZero(this);
  s16 angleY = shape_angle.y, angleZ = shape_angle.z;
  if (!zero)
    angleX = shape_angle.x;
  bomb_mDoMtx_ZXYrotM(0x1048D0CC, angleX, angleY, angleZ);
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = (f32)l_currentMtx.m[i / 4][i % 4];
  model = gabi::ea(mpModel.get());
  for (u32 i = 0; i < 12; i++)
    gabi::at<J3DModel>(model)->mBaseTransformMtx.m[i / 4][i % 4] = matrix[i];
}

/* 020C6404 */
void daBomb_c::init_mtx() {
  set_mtx();
}

/* 020C70DC */
void daBomb_c::se_cannon_fly_set() {
  bomb_mDoAud_seStart(0x381F, &current.pos);
  field_0x77F = 1;
}

/* 020C5078 */
void daBomb_c::se_cannon_fly_stop() {
  if (field_0x77F) {
    bomb_mDoAud_seStopActor(&current.pos);
    field_0x77F = 0;
  }
}

/* 020C7B24 */
void daBomb_c::eff_water_splash() {
  gabi::Local<cXyz> position;
  position->set(current.pos.x, field_0x558, current.pos.z);
  bomb_fopKyM_createWpillar(position.get(), 0, 0.5f, 0.75f);
  u32 self = gabi::ea(this);
  u32 polygons[2] = {field_0x562 ? 0 : self + 0x7F0, self + 0x764};
  s32 material = 19;
  for (u32 i = 0; i < 2; i++) {
    u32 polygon = polygons[i];
    if (polygon && (u16)gabi::at<cBgS_PolyInfo>(polygon)->mBgIndex < 256) {
      u32 p = play_bomb();
      material = bomb_dBgS_GetMtrlSndId(p + 0x12A0, polygon);
      break;
    }
  }
  s32 reverb = bomb_dComIfGp_getReverb((s8)current.roomNo);
  mDoAud_seStart(0x6918, &eyePos, material, reverb);
}

/* 020C5070 */
BOOL daBomb_IsDelete(daBomb_c *self) {
  return TRUE;
}

/* 020C50BC */
bool daBomb_c::bombDelete() {
  if (mSmoke.mpEmitter) {
    u32 emitter = gabi::ea(mSmoke.mpEmitter.get());
    gabi::at<BombEmitter>(emitter)->mpEmitterCallBack = 0;
    emitter = gabi::ea(mSmoke.mpEmitter.get());
    u32 flags = (u32)gabi::at<BombEmitter>(emitter)->mStatus;
    gabi::at<BombEmitter>(emitter)->mMaxFrame = 0xFFFFFFFF;
    gabi::at<BombEmitter>(emitter)->mStatus = flags | 1;
  }
  mSmoke.mpEmitter = nullptr;
  if (mSparks.mpEmitter) {
    u32 emitter = gabi::ea(mSparks.mpEmitter.get());
    gabi::at<BombEmitter>(emitter)->mpEmitterCallBack = 0;
    emitter = gabi::ea(mSparks.mpEmitter.get());
    u32 flags = (u32)gabi::at<BombEmitter>(emitter)->mStatus;
    gabi::at<BombEmitter>(emitter)->mMaxFrame = 0xFFFFFFFF;
    gabi::at<BombEmitter>(emitter)->mStatus = flags | 1;
  }
  mSparks.mpEmitter = nullptr;
  if (field_0x6F0) {
    u32 player = gabi::ea(dComIfGp_getLinkPlayer());
    u8 count = (u8)gabi::at<BombLinkView>(player)->mBombCnt;
    if (count)
      gabi::at<BombLinkView>(player)->mBombCnt = count - 1;
  }
  se_cannon_fly_stop();
  if (mType == 1) {
    s32 type = mType;
    const char *name = gabi::at<char>(gabi::ea(l_attrType.mAttr[type].resName.get()));
    dComIfG_resDelete(&mPhase, name);
  }
  bomb_dKy_actor_addcol_set(0, 0, 0, 0.0f);
  bomb_dKy_efplight_cut(&mPntLight);
  bomb_dKyw_pntwind_cut(&mPntWind);
  return true;
}

/* 020C51BC */
BOOL daBomb_Delete(daBomb_c *self) {
  self->bombDelete();
  return TRUE;
}

/* 020C51E0 */
BOOL daBomb_c::createHeap() {
  // HD: full-width animation-init flags and the GanonK extra-model reset are retained.
  if (mType == 1) {
    u32 data = bombGetResource(this, BombResource::dRes_INDEX_VBAKH_BDL_VBAKM_e);
    bombAssert(data, 0x9BC, gabi::guest_addr("modelData != (0)"));
    mpModel = gabi::at<J3DModel>(
        bomb_mDoExt_J3DModel__create(data, 0x80000, 0x11000022));
    u32 anm = bombGetResource(this, BombResource::dRes_INDEX_VBAKH_BCK_VBAMX_e);
    bombAssert(anm, 0x9C5, gabi::guest_addr("bck0_data != (0)"));
    u32 a =
        bomb_mDoExt_bckAnm_init(&mBck0, data, anm, 1, 0, 0, -1, 0, 1.0f);
    anm = bombGetResource(this, BombResource::dRes_INDEX_VBAKH_BCK_VBAKM_e);
    bombAssert(anm, 0x9CB, gabi::guest_addr("bck1_data != (0)"));
    u32 b =
        bomb_mDoExt_bckAnm_init(&mBck1, data, anm, 1, 0, 0, -1, 0, 1.0f);
    anm = bombGetResource(this, BombResource::dRes_INDEX_VBAKH_BRK_VBAMX_e);
    bombAssert(anm, 0x9D2, gabi::guest_addr("brk0_data != (0)"));
    u32 c =
        bomb_mDoExt_brkAnm_init(&mBrk0, data, anm, 1, 0, 0, -1, 0, 0, 1.0f);
    anm = bombGetResource(this, BombResource::dRes_INDEX_VBAKH_BRK_VBAKM_e);
    bombAssert(anm, 0x9D8, gabi::guest_addr("brk1_data != (0)"));
    u32 d =
        bomb_mDoExt_brkAnm_init(&mBrk1, data, anm, 1, 0, 0, -1, 0, 0, 1.0f);
    return mpModel && a && b && c && d;
  }
  u32 data = bombGetResource(this, BombResource::dRes_INDEX_LINK_BDL_BOMB_e);
  bombAssert(data, 0x9E5, gabi::guest_addr("modelData != (0)"));
  mpModel = gabi::at<J3DModel>(
      bomb_mDoExt_J3DModel__create(data, 0x80000, 0x11000002));
  if (!mpModel)
    return FALSE;
  u32 anm = bombGetResource(this, BombResource::dRes_INDEX_LINK_BCK_BOMB_e);
  if (!bomb_mDoExt_bckAnm_init(&mBck0, data, anm, 0, 2, 0, -1, 0, 1.0f))
    return FALSE;
  gabi::Local<BombSafeString> name;
  name->text = gabi::guest_addr("GanonK");
  name->vt = gc_tu.safeString;
  u32 p = play_bomb();
  gabi::Local<BombSafeString> stage;
  stage->text = p + 0x5134;
  stage->vt = gc_tu.safeString;
  u32 vt = name->vt;
  gabi::call_ptr(gabi::at<BombSafeStringVtable>(vt)->assureTermination.get(), name.get());
  vt = name->vt;
  gabi::call_ptr(gabi::at<BombSafeStringVtable>(vt)->assureTermination.get(), name.get());
  vt = stage->vt;
  u32 text = name->text;
  gabi::call_ptr(gabi::at<BombSafeStringVtable>(vt)->assureTermination.get(), stage.get());
  u32 other = stage->text;
  bool equal = text == other;
  if (!equal) {
    equal = true;
    for (u32 i = 0; i < 0x40001; i++) {
      u8 a = (u8)gabi::at<be<u8>>(text)[i], b = (u8)gabi::at<be<u8>>(other)[i];
      if (a != b) {
        equal = false;
        break;
      }
      if (!a)
        break;
    }
  }
  if (equal && mType == 0)
    bomb_HDExtraModel_reset(gabi::ea(hdExtra), 0);
  return TRUE;
}

/* 020C565C */
BOOL daBomb_createHeap(daBomb_c *self) {
  return self->createHeap();
}

/* 020C6724 */
cPhs_State daBomb_c::create() {
  s32 state = prm_get_state();
  mType = (state == 5 || state == 6) ? 1 : state == 4 ? 2 : 0;
  u32 self = gabi::ea(this), condition = (u32)actor_condition;
  if (!(condition & 8)) {
    if (this) {
      daBomb_c__ct(this);
      condition = (u32)actor_condition;
    }
    actor_condition = condition | 8;
  }
  cPhs_State status = cPhs_COMPLEATE_e;
  if (mType == 1) {
    const char *name = gabi::at<char>(gabi::ea(l_attrType.mAttr[mType].resName.get()));
    status = dComIfG_resLoad(&mPhase, name);
  }
  if (status == cPhs_COMPLEATE_e) {
    u32 heap = (u32)l_attrType.mAttr[mType].heapSize;
    if (bomb_fopAcM_entrySolidHeap(this, gabi::guest_fn((const void*)&daBomb_createHeap), heap))
      create_init();
    else
      status = cPhs_ERROR_e;
  }
  return status;
}

/* 020C685C */
cPhs_State daBomb_Create(daBomb_c *self) {
  return self->create();
}

/* 020C6408 */
void daBomb_c::create_init() {
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
  cullMtx = model ? model + 0xC8 : 0;
  bomb_dCcD_Stts_Init(&mStts, 200, 255, this);
  bomb_dCcD_Sph_Set(&mSph, gabi::ea(&l_sphSrc));
  mSph.mStts = &mStts;
  if (chk_state(STATE_8))
    mSph.mAtp = 2;
  play_bomb();
  gravity = -2.9f;
  maxFallSpeed = -100;
  mRestTime = 150;
  mInitialState = prm_get_state();
  if (!chk_state(STATE_4))
    attention_info.flags = (u32)attention_info.flags | 0x10;
  field_0x6F4 = 0;
  field_0x781 = 0;
  field_0x77D = 0;
  field_0x780 = 0;
  field_0x77C = 0;
  field_0x77F = 0;
  field_0x782 = 0;
  field_0x77E = 0;
  field_0x784 = 0;
  mWindVec.x = (f32)l_zero.x;
  mWindVec.y = (f32)l_zero.y;
  mWindVec.z = (f32)l_zero.z;
  mMassCounter = (u32)l_counter.mValue - 1;
  if (chk_attrState(this, ATTR_STATE_2)) {
    scale.y = 0;
    scale.x = 0;
    scale.z = 0;
  }
  if (chk_attrState(this, ATTR_STATE_4))
    setFuseEffect();
  if (chk_attrState(this, ATTR_STATE_8))
    bgCrrPos();
  if (chk_state(STATE_3)) {
    field_0x6F0 = 1;
    field_0x7C8 = 2;
  } else
    field_0x7C8 = 0;
  if (chk_attrState(this, ATTR_STATE_1))
    procExplode_init();
  else
    procWait_init();
  f32 radius = (f32)l_cullSize.mRadius;
  bomb_fopAcM_setCullSizeBoxMin(this, -radius, 0.0f, -radius);
  radius = (f32)l_cullSize.mRadius;
  f32 height = (f32)l_cullSize.mHeight;
  bomb_fopAcM_setCullSizeBoxMax(this, radius, height, radius);
  cullSizeFar = 10;
  init_mtx();
}

// HD: compiler-generated and template code of this translation unit.

/* 020C7FAC */
void __sinit_d_a_bomb_cpp() {
  for (u32 i = 0; i < 4; i++)
    l_randomState.mWords[i] = 0;
  __register_global_object(0x101922D0);
  l_headerAngles.mNegativePi = -3.1415927f;
  l_headerAngles.mPositivePi = 3.1415927f;
  bomb_randomSecondObject_ct(0x10462684);
  __register_global_object(0x101922DC);
  bomb_randomFirstObject_ct(0x10462685);
  __register_global_object(0x101922E8);
  l_sphSrc.mRadius = 30;
  l_cullSize.mHeight = 66;
  l_cullSize.mRadius = 36;
}

/* 020C8070 */
void bomb_generic_dtor(void *self, s32 flags) {
  if (self && (flags & 1))
    bomb_dl(self);
}

/* 020C80B0 */
void daBomb_c__dt(daBomb_c *self, s32 flags) {
  if (!self)
    return;
  u32 receiver = gabi::ea(self);
  bomb_dCcD_Sph_dtor(&self->mSph, 2);
  bomb_dCcD_Stts_dtor(&self->mStts, 2);
  self->mGndChk.cBgS_PolyInfo::__vtbl = gc_tu.gndChkDt[0];
  self->mGndChk.dBgS_PolyPassChk::__vtbl = gc_tu.gndChkDt[1];
  self->mGndChk.dBgS_GrpPassChk::__vtbl = gc_tu.gndChkDt[2];
  bomb_cBgS_Chk_cBgS_Chk(receiver + 0x880, 0);
  bomb_cM3dGCir_dtor(receiver + 0x854, 2);
  self->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
  self->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
  bomb_dBgS_Acch_dtor(&self->mAcch, 0);
  bomb_HDExtraModel_dtor(receiver + 0x5CC, 2);
  bomb_J3DFrameCtrl_dtor(receiver + 0x45C, 0);
  bomb_J3DFrameCtrl_dtor(receiver + 0x3D0, 0);
  bomb_fopAc_ac_c_fopAc_ac_c(self, 0);
  if (flags & 1)
    bomb_dl(self);
}

/* 020C81A0 */
void bomb_acch_virtual(void *self) { }

/* 020C5660 */
daBomb_c *daBomb_c__ct(daBomb_c *self) {
  if (!self) {
    self = bomb_operator_new(0xB50);
    if (!self)
      return nullptr;
  }
  bomb_fopAc_ac_c_fopAc_ac_c(self);
  u32 receiver = gabi::ea(self);
  self->__vtbl = 0x1000A72C;
  initBombBck(receiver + 0x3C0);
  initBombBck(receiver + 0x44C);
  bomb_mDoExt_brkAnm_init(&self->mBrk0);
  bomb_mDoExt_brkAnm_init(&self->mBrk1);
  bomb_HDExtraModel_ct(receiver + 0x5CC);
  bomb_dBgS_Acch_dBgS_Acch(&self->mAcch);
  self->mAcch.cBgS_Chk::__vtbl = gc_tu.objAcch.v10;
  self->mAcch.dBgS_PolyPassChk::__vtbl = gc_tu.objAcch.v14;
  self->mAcch.dBgS_GrpPassChk::__vtbl = gc_tu.objAcch.v20;
  self->mAcch.mbBombThrough = 1;
  bomb_dBgS_AcchCir_dBgS_AcchCir(&self->mCir);
  bomb_cBgS_GndChk_cBgS_GndChk(receiver + 0x880);
  self->mGndChk.mbCamThrough = 0;
  self->mGndChk.mbRopeThrough = 0;
  self->mGndChk.mbArrowThrough = 0;
  self->mGndChk.mbBoomerangThrough = 0;
  self->mGndChk.mbBombThrough = 0;
  self->mGndChk.dBgS_PolyPassChk::__vtbl = l_lavaVtables.v40;
  self->mGndChk.mGrp = 4;
  self->mGndChk.dBgS_GrpPassChk::__vtbl = l_lavaVtables.v4C;
  self->mGndChk.mbObjThrough = 1;
  self->mGndChk.mGrpPassChk = receiver + 0x8CC;
  self->mGndChk.cBgS_PolyInfo::__vtbl = l_lavaVtables.v20;
  self->mGndChk.cBgS_Chk::__vtbl = l_lavaVtables.v10;
  self->mGndChk.mbLinkThrough = 0;
  self->mGndChk.mPolyPassChk = receiver + 0x8C0;
  bomb_cCcD_Stts_ct(&self->mStts);
  bomb_dCcD_GStts_dCcD_GStts(receiver + 0x900);
  self->mStts.__vtbl = 0x1004AE88;
  self->mStts.__vtbl_gstts = 0x1004AEC0;
  bomb_dCcD_Sph_ct(&self->mSph);
  self->mSparks.__vtbl = 0x1000A94C;
  self->mPntLight.mHD20 = 1;
  self->mSmoke.__vtbl = 0x1000A90C;
  return self;
}

/* WWHD glue
str 1000A820 "brk1_data != (0)"
str 1000A80C "brk0_data != (0)"
str 1000A7F8 "bck1_data != (0)"
str 1000A7E4 "bck0_data != (0)"
str 1000A7D0 "modelData != (0)"
str 1000A7C0 "d_a_bomb3.inc"
str 1000A7B8 "GanonK"
data l_attrState 1000A73C
data l_attrType 1000A8F4
data l_resourceControl 101F4F28
data l_currentMtx 1048D0CC
data l_zero 101FFBA8
data l_zeroWords 101FFBA8
data l_counter 101FF558
data l_stayRoom 1047E6C8
data l_noGradeCos 104A4F44
data l_fuseOffset 10462698
data l_cullSize 1046267C
data l_headerAngles 10462674
data l_randomState 10462688
data l_sphSrc 10192270
data l_sinCos 104A44F8
data l_checkCc 1000A86C
data l_checkBg 1000A898
ext 025E1AF8 void bomb_mDoAud_seStopActor(cXyz *)
ext 027DF40C void bomb_J3DAnmCurve_update(u32)
ext 023D9340 void bomb_Link_drawBombExtra(u32, u32, J3DModel *)
ext 02838148 s32 bomb_J3DUClipper_clip(s32, s32, cXyz *, f32)
ext 02555D0C u32 bomb_dKy_getEnvlight()
ext 025626A4 void bomb_dScnKy_env_light_c_settingTevStruct(u32, s32, cXyz *, dKy_tevstr_c *)
ext 02562F5C void bomb_dScnKy_env_light_c_setLightTevColorType(u32, J3DModel *, dKy_tevstr_c *)
ext 0251641C void bomb_dCcD_GObjInf_ClrCoHit(dCcD_Sph *)
ext 025E19CC void bomb_mDoAud_seStart(s32, cXyz *)
ext 024EF130 s32 bomb_dBgS_GetRoomId(u32, u32)
ext 024EEEB8 u32 bomb_dBgS_GetPolyColor(u32, u32)
ext 02008974 f32 bomb_cBgS_GroundCross(u32, u32)
ext 024F08A8 void bomb_dBgS_Acch_CrrPos(dBgS_Acch *, u32)
ext 0246B6A4 s32 bomb_daSea_ChkArea(f32, f32)
ext 0246BA0C f32 bomb_daSea_calcWave(f32, f32)
ext 02311CD8 void bomb_daObj_make_land_effect(daBomb_c *, u32, f32)
ext 0200ECD4 f32 bomb_cLib_addCalc(be<f32> *, f32, f32, f32, f32)
ext 020CB9E4 bool bomb_daBomb_c_prm_get_angXZero(daBomb_c *)
ext 025F1B48 void bomb_mDoMtx_ZXYrotM(s32, s16, s16, s16)
ext 020C6D90 u32 bomb_daBomb_c_procCarry_init(daBomb_c *)
ext 020084C8 u32 bomb_cBgS_GetTriPla(u32, u16, u16)
ext 028E8DD0 f32 bomb_PSVECSquareMag(cXyz *)
ext 023121C4 void bomb_daObj_posMoveF_grade(daBomb_c *, dCcD_Stts *, cXyz *, u32, s32, f32, f32, f32, f32)
ext 025D6870 void bomb_fopAcM_posMoveF(daBomb_c *, dCcD_Stts *)
ext 025D57E0 void bomb_fopAcM_delete(daBomb_c *)
ext 02515E50 void bomb_dCcD_GStts_Move(u32)
ext 0200E240 void bomb_cCcS_Set(u32, dCcD_Sph *)
ext 02516C14 void bomb_dCcMassS_Mng_Set(u32, dCcD_Sph *, s32)
ext 02520540 s32 bomb_dComIfGp_getReverb(s8)
ext 025553B8 void bomb_dKy_actor_addcol_set(s32, s32, s32, f32)
ext 0255BA9C void bomb_dKy_efplight_cut(LIGHT_INFLUENCE *)
ext 0257D1B8 void bomb_dKyw_pntwind_cut(BombWindInfluence *)
ext 025A866C void bomb_dPa_control_c_setBombSmoke(u32, s32, cXyz *, csXyz *, cXyz *, s32)
ext 025CB374 void bomb_dVibration_c_StartShock(u32, s32, s32, cXyz *)
ext 025A847C void bomb_dPa_control_c_set(u32, s32, s32, cXyz *, s32, cXyz *, s32, daBomb_fuseSparksEcallBack *, s32, s32, s32, s32)
ext 025A847C void bomb_dPa_control_c_set(u32, s32, s32, cXyz *, s32, cXyz *, s32, daBomb_fuseSmokeEcallBack *, s8, u32, u32, s32)
ext 025DAE64 void bomb_fopKyM_createWpillar(cXyz *, s32, f32, f32)
ext 024EECAC s32 bomb_dBgS_GetMtrlSndId(u32, u32)
ext 028E8E64 void bomb_PSVECScale(cXyz *, cXyz *, f32)
ext 028F4384 f32 bomb_sqrtf(f32)
ext 02515BBC u32 bomb_dCcD_GAtTgCoCommonBase_GetAc(u32)
ext 020195B0 s32 bomb_cM_atan2s(f32, f32)
ext 0201AE48 void bomb_cXyz___ml(cXyz *, cXyz *, f32)
ext 0201AD78 void bomb_cXyz___pl(cXyz *, cXyz *, cXyz *)
ext 028ED6F8 void bomb_randomSecondObject_ct(s32)
ext 028EAB2C void bomb_randomFirstObject_ct(s32)
ext 0273AF40 void bomb_dl(void *)
ext 02515AE8 void bomb_dCcD_Sph_dtor(dCcD_Sph *, s32)
ext 02515860 void bomb_dCcD_Stts_dtor(dCcD_Stts *, s32)
ext 02008DAC void bomb_cBgS_Chk_cBgS_Chk(u32, s32)
ext 02018034 void bomb_cM3dGCir_dtor(u32, s32)
ext 024EFD9C void bomb_dBgS_Acch_dtor(dBgS_Acch *, s32)
ext 02082DDC void bomb_HDExtraModel_dtor(u32, s32)
ext 027F3628 void bomb_J3DFrameCtrl_dtor(u32, s32)
ext 025D50BC void bomb_fopAc_ac_c_fopAc_ac_c(daBomb_c *, s32)
ext 0273AF40 void bomb_dl(daBomb_c *)
ext 027F2BC0 void bomb_J3DFrameCtrl_init(u32, s32)
ext 027DA984 void bomb_J3DAnmCurve_ct(u32)
ext 0273AD10 daBomb_c * bomb_operator_new(s32)
ext 025D4ED0 void bomb_fopAc_ac_c_fopAc_ac_c(daBomb_c *)
ext 025E80D0 void bomb_mDoExt_brkAnm_init(mDoExt_brkAnm *)
ext 02080404 void bomb_HDExtraModel_ct(u32)
ext 024F0474 void bomb_dBgS_Acch_dBgS_Acch(dBgS_Acch *)
ext 024EFE94 void bomb_dBgS_AcchCir_dBgS_AcchCir(dBgS_AcchCir *)
ext 02008E0C void bomb_cBgS_GndChk_cBgS_GndChk(u32)
ext 0200BD2C void bomb_cCcD_Stts_ct(dCcD_Stts *)
ext 02515DA0 void bomb_dCcD_GStts_dCcD_GStts(u32)
ext 025166F0 void bomb_dCcD_Sph_ct(dCcD_Sph *)
ext 02515F14 void bomb_dCcD_Stts_Init(dCcD_Stts *, s32, s32, daBomb_c *)
ext 0251677C void bomb_dCcD_Sph_Set(dCcD_Sph *, s32)
ext 025D672C void bomb_fopAcM_setCullSizeBoxMin(daBomb_c *, f32, f32, f32)
ext 025D673C void bomb_fopAcM_setCullSizeBoxMax(daBomb_c *, f32, f32, f32)
ext 025D63E8 s32 bomb_fopAcM_entrySolidHeap(daBomb_c *, s32, u32)
ext 025A847C void bomb_dPa_control_c_set(u32, s32, s32, cXyz *, const csXyz *, cXyz *, s32, s32, s32, s32, s32, s32)
ext 025A866C void bomb_dPa_control_c_setBombSmoke(u32, s32, cXyz *, s32, cXyz *, s32)
ext 025A847C void bomb_dPa_control_c_set(u32, s32, s32, cXyz *, s32, cXyz *, s32, s32, s32, s32, s32, s32)
ext 025A847C u32 bomb_dPa_control_c_set_result(u32, s32, s32, cXyz *, const csXyz *, cXyz *, s32, s32, s32, s32, s32, s32)
ext 025A866C u32 bomb_dPa_control_c_setBombSmoke_result(u32, s32, cXyz *, s32, cXyz *, s32)
ext 025A847C u32 bomb_dPa_control_c_set_result(u32, s32, s32, cXyz *, s32, cXyz *, s32, s32, s32, s32, s32, s32)
ext 020CB9A0 bool bomb_daBomb_c_prm_get_cheapEff(daBomb_c *)
ext 028E8DE8 f32 bomb_PSVECSquareDistance(cXyz *, u32)
ext 025548F0 void bomb_dKy_actor_addcol_amb_set(s32, s32, s32, f32)
ext 02554B18 void bomb_dKy_bg_addcol_amb_set(s32, s32, s32, f32)
ext 02554C2C void bomb_dKy_bg_addcol_dif_set(s32, s32, s32, f32)
ext 025052BC void bomb_dCamera_c_ForceLockOff(u32, u32)
ext 025D9F70 u32 bomb_fopAcM_getWaterY(cXyz *, gabi::be<float> *)
ext 0255B9C8 void bomb_dKy_efplight_set(LIGHT_INFLUENCE *)
ext 0257DC90 void bomb_dKyw_pntwind_set(BombWindInfluence *)
ext 025E1A40 void bomb_mDoAud_seStartLevel(s32, u32, s32, s32)
ext 025D9D24 void bomb_fopAcM_cancelCarryNow(daBomb_c *)
ext 0255F458 void bomb_dKy_Sound_set(cXyz *, s32, u32, s32)
ext 028E8DE8 f32 bomb_PSVECSquareDistance(cXyz *, cXyz *)
ext 0281DCB8 u32 bomb_JPABaseEmitter_createParticle(u32)
ext 026066C4 u32 bomb_dRes_control_c_getRes(u32, BombSafeString *, u32)
ext 0273AA24 void bomb_JUTAssertion_showAssert(s32, u32, u32)
ext 025E38E0 u32 bomb_mDoExt_J3DModel__create(u32, s32, s32)
ext 025E8508 u32 bomb_mDoExt_bckAnm_init(mDoExt_bckAnm *, u32, u32, s32, s32, s32, s32, s32, f32)
ext 025E8154 u32 bomb_mDoExt_brkAnm_init(mDoExt_brkAnm *, u32, u32, s32, s32, s32, s32, s32, s32, f32)
ext 0207FD38 void bomb_HDExtraModel_reset(u32, s32)
*/

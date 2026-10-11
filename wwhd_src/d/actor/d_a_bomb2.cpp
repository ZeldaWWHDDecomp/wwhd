/** Bomb flower bomb: tww structure with verified WWHD behavior. */
#include "d/actor/d_a_bomb2.h"
// HD: per-translation-unit inline constructor and destructor vtables.
const gc_tu_data gc_tu = {
  .safeString = 0x1000A9FC,
  .objAcch = {0x1000AB0C, 0x1000AB2C, 0x1000AB1C},
  .objAcchDt = {0x1000AB1C, 0x1000AB2C},
  .gndChk = {0x1000AACC, 0x1000AADC, 0x1000AAFC, 0x1000AAEC},
  .gndChkDt = {0x1000AA5C, 0x1000AA7C, 0x1000AA3C},
  .headerStatics = {0x104626A8, 0x10192324, 0x104626BC},
};

using namespace daBomb2;
static cXyz l_zero;
static Bomb2VectorWords l_zeroWords;
static Mtx34 l_currentMtx;
static Bomb2Word l_resourceControl;
static Bomb2Byte l_stayRoom;
static Bomb2Float l_noGradeCos;
static Bomb2Angles l_headerAngles;
static Bomb2RandomObjects l_randomObjects;
static Bomb2Words l_randomState;
static Bomb2CullSize l_cullSize;
static Bomb2FuseOffset l_fuseOffset;
static cXyz l_localCenter;
static Bomb2Word l_centerGuard;
static Bomb2SphereSource M_sph_src;
static Bomb2SinCosTable l_sinCos;
static Bomb2Registration l_randomRegistration;
static Bomb2Registration l_secondRegistration;
static Bomb2Registration l_firstRegistration;

static Bomb2StartTable l_startProc;
static Bomb2ModeTable l_modeProc;
static Bomb2ModeTable l_tensorProc;

// HD: GHS descriptors contain an adjusted receiver and an eight-byte virtual slot.
// The verified HD dispatch uses a direct stub call even for virtual targets.
static void invokeBomb2(u32 object, const Bomb2PtMf& descriptor) {
  s32 adjust = descriptor.d, index = descriptor.i;
  u32 target = (u32)descriptor.f;
  object += adjust;
  if (index >= 0) {
    u32 vtable = gabi::load<u32>(object + (s16)target);
    target = gabi::load<u32>(vtable + index * 8 + 4);
  }
  gabi::call(target, object);
}
static u32 play2() { return gabi::ea(dComIfGp_get()); }
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

/* 020C81A4 */
void daBomb2::Env_c::set(const cXyz *pos) {
  mPntLight.mPos.x = pos->x;
  f32 y = pos->y;
  mPntLight.mPos.y = y;
  f32 z = pos->z;
  mPntLight.mFluctuation = 100;
  mPntLight.mColorR = 200;
  mPntLight.mColorG = 200;
  mPntLight.mColorB = 160;
  mPntLight.mPos.y = y + 100;
  mPntLight.mPos.z = z;
  mPntLight.mPower = 600;
  bomb2_dKy_efplight_set(this);
  u32 s = gabi::ea(this);
  mPntWind.mPos.x = pos->x;
  mPntWind.mPos.y = pos->y;
  mPntWind.field_0x20 = 0;
  mPntWind.mPos.z = pos->z;
  mPntWind.mRadius = 500;
  mPntWind.mDir.y = 1;
  mPntWind.mDir.z = 0;
  mPntWind.mDir.x = 0;
  mPntWind.mStrength = 0.5f;
  bomb2_dKyw_pntwind_set( s + 0x24);
  field_0x50 = 0;
  field_0x4C = 0;
}

/* 020C82A4 */
void daBomb2::Env_c::clean() {
  bomb2_dKy_actor_addcol_set(0, 0, 0, 0.0f);
  bomb2_dKy_efplight_cut(this);
  bomb2_dKyw_pntwind_cut(gabi::ea(this) + 0x24);
}

/* 020C82F4 */
bool daBomb2::Env_c::is_end() {
  return field_0x4C > 1;
}

/* 020C8308 */
void daBomb2::Env_c::proc(const cXyz *pos) {
  u32 p = play2();
  f32 intensity = field_0x50;
  u32 camera = (u32)gabi::at<dComIfG_inf_c>(p)->play.mCameraInfo[0].mpCamera;
  mPntWind.mStrength = intensity;
  mPntLight.mPower = 1500 * intensity;
  f32 distance = bomb2_PSVECSquareDistance_f32(pos, camera + 0xDC);
  distance = bomb2_sqrtf_f32(distance);
  f32 factor = 0;
  if (distance < 1500)
    factor = gabi::fnmsubs(distance, 0.0006666666595265269f, 1);
  factor = field_0x50 * factor;
  bomb2_dKy_actor_addcol_amb_set( 200, 180, 100, factor);
  bomb2_dKy_bg_addcol_amb_set( 180, 160, 60, factor);
  bomb2_dKy_bg_addcol_dif_set( 255, 225, 120, factor);
  switch (field_0x4C) {
  case 0:
    bomb2_cLib_addCalc_f32(&field_0x50, 1.0f, 0.5f, 0.4f, 0.01f);
    if (!(field_0x50 < 0.99f))
      field_0x4C = field_0x4C + 1;
    break;
  case 1:
    bomb2_cLib_addCalc_f32(&field_0x50, 0.0f, 0.05f, 0.04f, 0.001f);
    if (!(field_0x50 > 0.01f))
      field_0x4C = field_0x4C + 1;
    break;
  }
}

/* 020C8518 */
void daBomb2::FuseSmokeCB_c::setOldPosP(const cXyz *a, const cXyz *b) {
  field_0x0C = a;
  field_0x10 = b;
  field_0x04 = 20;
}

/* 020C852C */
void daBomb2::FuseSmokeCB_c::deleteCallBack() {
  u32 e = gabi::ea(mpEmitter.get());
  if (e) {
    gabi::at<Bomb2Emitter>(e)->mpEmitterCallBack = 0;
    e = gabi::ea(mpEmitter.get());
    u32 flags = (u32)gabi::at<Bomb2Emitter>(e)->mStatus;
    gabi::at<Bomb2Emitter>(e)->mMaxFrame = -1;
    gabi::at<Bomb2Emitter>(e)->mStatus = flags | 1;
  }
  mpEmitter = nullptr;
}

/* 020C8564 */
void daBomb2::FuseSmokeCB_c::executeAfter(void *emitter) {
  u32 e = gabi::ea(emitter);
  f32 z = mpPos->z, x = mpPos->x, y = mpPos->y;
  f32 px = field_0x0C->x, py = field_0x0C->y, pz = field_0x0C->z;
  // HD: volume types at and above seven mirror the emitter Y translation.
  u8 type = (u8)gabi::at<Bomb2Emitter>(e)->mVolumeType;
  gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.x = x;
  gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.y = y;
  gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.z = z;
  if (type >= 7)
    gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.y = -(f32)gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.y;
  f32 distance = bomb2_PSVECSquareDistance_f32( mpPos.get(), field_0x0C.get());
  distance = bomb2_sqrtf_f32(distance);
  s16 life = gabi::ftoi(bombSmokeFma(20 - distance, 0.5f, 10));
  if (life < 10)
    life = 10;
  gabi::at<Bomb2Emitter>(e)->mLifeTime = life;
  f32 dx = bombSmokeSub(x, px) * 0.5f, dy = bombSmokeSub(y, py) * 0.5f,
      dz = bombSmokeSub(z, pz) * 0.5f;
  f32 tx = (px - field_0x10->x) * 0.5f, ty = (py - field_0x10->y) * 0.5f,
      tz = (pz - field_0x10->z) * 0.5f;
  distance = bomb2_PSVECSquareDistance_f32( mpPos.get(), field_0x0C.get());
  distance = bomb2_sqrtf_f32(distance);
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
      gabi::at<Bomb2Emitter>(e)->mLifeTime = lifetime;
      u32 particle = bomb2_JPABaseEmitter__createParticle_u32(e);
      if (particle) {
        gabi::at<Bomb2Particle>(particle)->mPosition.x = ox;
        gabi::at<Bomb2Particle>(particle)->mPosition.y = oy;
        gabi::at<Bomb2Particle>(particle)->mPosition.z = oz;
      }
    }
  }
  field_0x04 = life;
}

/* 020C890C */
void daBomb2::FuseSmokeCB_c::setup(void *emitter, const cXyz *pos,
                 const csXyz *angle, s8 room) {
  mpPos = pos;
  mpEmitter = emitter;
}

/* 020C8918 */
void daBomb2::FuseSparksCB_c::deleteCallBack() {
  u32 e = gabi::ea(mpEmitter.get());
  if (e) {
    gabi::at<Bomb2Emitter>(e)->mpEmitterCallBack = 0;
    e = gabi::ea(mpEmitter.get());
    u32 flags = (u32)gabi::at<Bomb2Emitter>(e)->mStatus;
    gabi::at<Bomb2Emitter>(e)->mMaxFrame = -1;
    gabi::at<Bomb2Emitter>(e)->mStatus = flags | 1;
  }
  mpEmitter = nullptr;
}

/* 020C894C */
void daBomb2::FuseSparksCB_c::execute(void *emitter) {
  u32 e = gabi::ea(emitter);
  f32 y = mpPos->y, z = mpPos->z, x = mpPos->x;
  // HD: volume types at and above seven mirror the emitter Y translation.
  u8 type = (u8)gabi::at<Bomb2Emitter>(e)->mVolumeType;
  u32 link = (u32)gabi::at<Bomb2Emitter>(e)->mAlivePtclHead;
  gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.z = z;
  gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.x = x;
  gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.y = y;
  if (type >= 7)
    gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.y = -(f32)gabi::at<Bomb2Emitter>(e)->mGlobalTranslation.y;
  while (link) {
    u32 next = (u32)gabi::at<Bomb2ParticleNode>(link)->mNext, particle = (u32)gabi::at<Bomb2ParticleNode>(link)->mObject;
    gabi::at<Bomb2Particle>(particle)->mPosition.z = z;
    gabi::at<Bomb2Particle>(particle)->mPosition.y = y;
    gabi::at<Bomb2Particle>(particle)->mPosition.x = x;
    link = next;
  }
}

/* 020C89B0 */
void daBomb2::FuseSparksCB_c::setup(void *emitter, const cXyz *pos,
                  const csXyz *angle, s8 room) {
  mpPos = pos;
  mpEmitter = emitter;
}

/* 020C89BC */
bool daBomb2::Act_c::create_heap_nut() {

  gabi::Local<Bomb2SafeString> n;
  n->a = gabi::guest_addr("VbakH");
  n->b = gc_tu.safeString;
  u32 data =
      bomb2_dRes_control_c__getRes_u32((u32)l_resourceControl.mValue, n.get(), dRes_INDEX_VBAKH_BDL_VBAKM_e);
  if (!data)
    bomb2_JUTAssertion__getSDevice(gabi::guest_addr("d_a_bomb2.cpp"), 0x307, gabi::guest_addr("mdl_data != (0)"));
  mpModel = gabi::at<J3DModel>(
      bomb2_mDoExt_J3DModel__create_u32( data, 0x80000, 0x11000022));
  u32 anm =
      bomb2_dRes_control_c__getRes_u32((u32)l_resourceControl.mValue, n.get(), dRes_INDEX_VBAKH_BCK_VBAKM_e);
  if (!anm)
    bomb2_JUTAssertion__getSDevice(gabi::guest_addr("d_a_bomb2.cpp"), 0x311, gabi::guest_addr("bck_data != (0)"));
  u32 b = bomb2_mDoExt_bckAnm__init_u32(&mBck0, data, anm, 1, 0, 0, -1, 0, 1.0f);
  anm = bomb2_dRes_control_c__getRes_u32((u32)l_resourceControl.mValue, n.get(), dRes_INDEX_VBAKH_BRK_VBAKM_e);
  if (!anm)
    bomb2_JUTAssertion__getSDevice(gabi::guest_addr("d_a_bomb2.cpp"), 0x318, gabi::guest_addr("brk_data != (0)"));
  u32 k =
      bomb2_mDoExt_brkAnm_init_u32(&mBrk0, data, anm, 1, 0, 0, -1, 0, 0, 1.0f);
  return mpModel && b && k;
}

/* 020C8B50 */
u32 daBomb2::Act_c::create_heap() {
  return bomb2_daBomb2__Act_c__create_heap_nut_u32(this);
}

/* 020C8B58 */
void daBomb2::Act_c::crr_init() {
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

/* 020C8C04 */
void daBomb2::Act_c::cc_init() {
  bomb2_dCcD_Stts__Init(&mStts, 200, 255, this);
  bomb2_dCcD_Sph__Set(&mSph, gabi::ea(&M_sph_src));
  mSph.mStts = &mStts;
}

/* 020CA49C */
void daBomb2::Act_c::start_explode_instant() {
  if (chk_water_in())
    eff_explode_water();
  else
    eff_explode();
  mode_explode_init();
}

/* 020CA65C */
void daBomb2::Act_c::start_explode_interval() {
  eff_fuse_start();
  set_nut_exp_interval();
  mode_wait_init();
}

/* 020CA694 */
void daBomb2::Act_c::start_carry() {
  mode_carry_init();
}

/* 020C91A4 */
void daBomb2::Act_c::start_proc_call() {
  u32 p = bomb2_PrmAbstract_u32(this, PRM_2_W, PRM_2_S);
  invokeBomb2(gabi::ea(this), l_startProc.mProc[p]);
}

/* 020C9218 */
void daBomb2::Act_c::create_init() {
  crr_init();
  cc_init();
  u32 s = gabi::ea(this), mdl = mpModel.v.get();
  cullMtx = mdl ? mdl + 0xC8 : 0;
  f32 r = (f32)l_cullSize.mRadius, h = (f32)l_cullSize.mHeight;
  bomb2_fopAcM_setCullSizeBox(this, -r, 0.0f, -r, r, h, r);
  cullSizeFar = 10;
  if (bomb2_PrmAbstract_u32(this, PRM_1_W, PRM_1_S))
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
  mWindVec.x = (f32)l_zero.x;
  mWindVec.y = (f32)l_zero.y;
  mWindVec.z = (f32)l_zero.z;
  start_proc_call();
}

/* 020C93EC */
cPhs_State daBomb2::Act_c::_create() {
  u32 s = gabi::ea(this), flags = (u32)actor_condition;
  if (!(flags & 8)) {
    if (s) {
      bomb2_fopAc_ac_c__fopAc_ac_c(this);
      gabi::store<u32>(s + 0xB4, 0x1000AB3C);
      bomb2_J3DFrameCtrl__init( s + 0x3B8, 0);
      gabi::store<u32>(s + 0x3C8, 0x1016E54C);
      bomb2_J3DAnmCurve_ct( s + 0x3CC);
      gabi::store<u32>(s + 0x400, 0x1016D820);
      for (u32 o : {0x43Cu, 0x434u, 0x438u, 0x440u, 0x410u})
        gabi::store<u32>(s + o, 0);
      gabi::store<u32>(s + 0x3C8, 0x1000AA14);
      bomb2_mDoExt_brkAnm__init( s + 0x444);
      bomb2_dBgS_Acch__dBgS_Acch( s + 0x4BC);
      gabi::store<u32>(s + 0x4CC, 0x1000AB0C);
      gabi::store<u32>(s + 0x4D0, 0x1000AB2C);
      gabi::store<u8>(s + 0x4D8, 1);
      gabi::store<u32>(s + 0x4DC, 0x1000AB1C);
      bomb2_dBgS_AcchCir__dBgS_AcchCir( s + 0x680);
      bomb2_cBgS_GndChk__cBgS_GndChk( s + 0x6C0);
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
      bomb2_cCcD_Stts_ct( s + 0x724);
      bomb2_dCcD_GStts__dCcD_GStts( s + 0x740);
      gabi::store<u32>(s + 0x73C, 0x1004AE88);
      gabi::store<u32>(s + 0x740, 0x1004AEC0);
      bomb2_dCcD_Sph_ct( s + 0x760);
      gabi::store<u32>(s + 0x894, 0x1000AC68);
      gabi::store<u32>(s + 0x8AC, 0x1000ACA8);
      bomb2_Env_c_ctor( s + 0x8DC);
      flags = (u32)actor_condition;
    }
    actor_condition = flags | 8;
  }
  u32 state = bomb2_dComIfG_resLoad_u32( s + 0x3AC, gabi::guest_addr("VbakH"));
  if (state == cPhs_COMPLEATE_e) {
    if (bomb2_fopAcM_entrySolidHeap_u32(this, 0x020C8B54, 0x920))
      create_init();
    else
      state = cPhs_ERROR_e;
  }
  return (cPhs_State)state;
}

/* 020C962C */
bool daBomb2::Act_c::_delete() {
  eff_fuse_end();
  mEnv.clean();
  dComIfG_resDelete(&mPhase, "VbakH");
  return true;
}

/* 020C9074 */
void daBomb2::Act_c::set_mtx() {
  u32 mdl = mpModel.v.get();
  gabi::at<Bomb2ModelView>(mdl)->mBaseScale.x = scale.x;
  gabi::at<Bomb2ModelView>(mdl)->mBaseScale.y = scale.y;
  gabi::at<Bomb2ModelView>(mdl)->mBaseScale.z = scale.z;
  bomb2_PSMTXTrans(gabi::ea(&l_currentMtx), (f32)current.pos.x, (f32)current.pos.y,
             (f32)current.pos.z);
  bomb2_PSMTXConcat(gabi::ea(&l_currentMtx), gabi::ea(&field_0x754), gabi::ea(&l_currentMtx));
  bomb2_mDoMtx_ZXYrotM(gabi::ea(&l_currentMtx), (s16)shape_angle.x, (s16)shape_angle.y,
             (s16)shape_angle.z);
  f32 m[12];
  for (int i = 0; i < 12; i++)
    m[i] = (f32)l_currentMtx.m[i / 4][i % 4];
  mdl = mpModel.v.get();
  for (int i = 0; i < 12; i++)
    gabi::at<Bomb2ModelView>(mdl)->mBaseTRMtx[i] = m[i];
  eff_fuse_move();
}

/* 020C916C */
void daBomb2::Act_c::init_mtx() {
  tensor_init();
  eff_fuse_init();
  set_mtx();
}

/* 020C9D04 */
void daBomb2::Act_c::cc_set() {
  gabi::Local<cXyz> pos;
  pos->x = current.pos.x;
  pos->y = current.pos.y;
  pos->z = current.pos.z;
  s32 state = mState;
  f32 radius = 200;
  if (state == Mode_EXPLODE_e) {
    if (field_0x73C <= 0)
      return;
    field_0x73C = field_0x73C - 1;
    if (field_0x73C <= 0)
      return;
  } else {
    if (state == Mode_SINK_e)
      return;
    radius = 30 * (f32)scale.x;
    if (!(u32)l_centerGuard.mValue) {
      l_centerGuard.mValue = 1;
      l_localCenter.z = 0;
      l_localCenter.y = 30;
      l_localCenter.x = 0;
    }
    u32 mdl = mpModel.v.get();
    bomb2_PSMTXCopy( mdl ? mdl + 0xC8 : 0, gabi::ea(&l_currentMtx));
    bomb2_mDoMtx_stack_c__scaleM((f32)scale.x, (f32)scale.y, (f32)scale.z);
    bomb2_PSMTXMultVec(gabi::ea(&l_currentMtx), gabi::ea(&l_localCenter), pos.get());
  }
  u32 s = gabi::ea(this);
  bomb2_dCcD_GStts__Move(gabi::ea(&mStts.__vtbl_gstts));
  bomb2_cM3dGSph__SetR(gabi::ea(&mSph.mSph), radius);
  bomb2_cM3dGSph__SetC(gabi::ea(&mSph.mSph), pos.get());
  bomb2_cCcS__Set( play2() + 0x26A4, &mSph);
  bomb2_dCcMassS_Mng__Set( play2() + 0x4EF8, &mSph, 3);
}

/* 020CA34C */
void daBomb2::Act_c::camera_lockoff() {
  s32 id = (s8)gabi::at<dComIfG_inf_c>(play2())->play.mPlayerInfo[0].mCameraID;
  u32 cam = (u32)gabi::at<dComIfG_inf_c>(play2())->play.mCameraInfo[id].mpCamera;
  bomb2_dCamera_c__ForceLockOff(cam + 0x248, (u32)mBsPcId);
}

/* 020CA698 */
void daBomb2::Act_c::posMoveF() {
  if (bomb2_PSVECSquareMag_f32(&mWindVec) > 0.01f) {
    u32 p = play2(), s = gabi::ea(this);
    u32 plane =
        bomb2_cBgS__GetTriPla_u32( p + 0x12A0, (u16)mAcch.m_gnd.mBgIndex,
                        (u16)mAcch.m_gnd.mPolyIndex);
    bomb2_daObj__posMoveF_grade(this, &mStts, &mWindVec, plane, 0, 0.002f, 0.0005f,
               plane ? 0.06f : 0.0f,
               plane ? (f32)l_noGradeCos.mValue : 0.0f);
  } else
    bomb2_fopAcM_posMoveF(this, &mStts);
}

/* 020C8E6C */
void daBomb2::Act_c::bgCrrPos() {
  u32 p = play2();
  bomb2_dBgS_Acch__CrrPos(&mAcch, p + 0x12A0);
  bgCrrPos_lava();
  bgCrrPos_water();
  setRoomInfo();
}

/* 020C8C78 */
void daBomb2::Act_c::bgCrrPos_lava() {
  u32 self = gabi::ea(this);
  f32 z = current.pos.z, y = old.pos.y + 1, x = current.pos.x;
  mGndChk.m_pos.z = z;
  mGndChk.m_pos.x = x;
  mGndChk.m_pos.y = y;
  u32 p = play2();
  field_0x51C = bomb2_cBgS__GroundCross_f32( p + 0x12A0, self + 0x6C0);
}

/* 020C8CE4 */
void daBomb2::Act_c::bgCrrPos_water() {
  f32 water = (f32)mAcch.m_wtr.m_height;
  s32 area =
      bomb2_daSea_ChkArea_s32((f32)current.pos.x, (f32)current.pos.z);
  f32 wave =
      bomb2_daSea_calcWave_f32((f32)current.pos.x, (f32)current.pos.z);
  // GHS branches invert the GT/LT bit; keep the unordered cases explicit.
  bool in = (mAcch.m_flags & dBgS_Acch::WATER_IN) != 0, sea = area && current.pos.y < wave;
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

/* 020CA064 */
u8 daBomb2::Act_c::chk_water_in() {
  return mbWaterIn;
}

/* 020CA790 */
bool daBomb2::Act_c::chk_lava_in() {
  return field_0x51C != -1000000000.0f && current.pos.y < field_0x51C;
}

/* 020C8DDC */
void daBomb2::Act_c::setRoomInfo() {
  u32 self = gabi::ea(this);
  u8 room;
  if (mAcch.m_ground_h != -1000000000.0f) {
    u32 p = play2();
    s32 id = bomb2_dBgS__GetRoomId_s32( p + 0x12A0, gabi::ea(&mAcch.m_gnd.mPolyIndex));
    p = play2();
    u32 color = bomb2_dBgS__GetPolyColor_u32( p + 0x12A0, gabi::ea(&mAcch.m_gnd.mPolyIndex));
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

/* 020CA7C0 */
void daBomb2::Act_c::bound(f32 value) {
  u32 s = gabi::ea(this), f = mAcch.m_flags;
  if (f & 0x10) {
    s16 wall = (s16)mCir.m_wall_angle_y;
    speedF = (f32)speedF * 0.8f;
    s16 dir = (s16)current.angle.y;
    f = mAcch.m_flags;
    current.angle.y = 2 * (s32)wall - ((s32)dir + 0x8000);
  }
  if (f & 0x80) {
    bomb2_daObj__make_land_effect(this, gabi::ea(&mAcch.m_gnd), 0.6f);
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
    bomb2_cLib_addCalc_f32(&speedF, 0.0f, 0.5f, 5.0f, 1.0f);
}

/* 020CA610 */
void daBomb2::Act_c::set_nut_exp_interval() {
  if (mBombTimer > 30) {
    mBombTimer = 30;
    mBrk0.mFrameCtrl.setFrame(105);
    mBck0.mFrameCtrl.setFrame(105);
  }
}

/* 020C9CBC */
void daBomb2::Act_c::anm_play() {
  if ((s32)((u32)mBombTimer + 1) <= 135) {
    bomb2_mDoExt_baseAnm__play(&mBck0);
    bomb2_mDoExt_baseAnm__play(&mBrk0);
  }
}

/* 020C9674 */
void daBomb2::Act_c::set_wind_vec() {
  bomb2_PSVECScale(&mWindVec, &mWindVec, 0.95f);
  f32 mag = bomb2_PSVECSquareMag_f32(&mWindVec);
  if (mag < 0.1f) {
    mWindVec.y = 0;
    mWindVec.x = 0;
    mWindVec.z = 0;
  }
  if (!bomb2_dCcD_GObjInf__ChkTgHit_u32(&mSph))
    return;
  u32 hit = bomb2_dCcD_GObjInf__GetTgHitObj_u32(&mSph);
  if (!hit || !((u32)gabi::at<dCcD_GObjInf>(hit)->mAtType & 0x200000))
    return;
  gabi::Local<cXyz> push, dir, v1, v2, v3, sum;
  u32 s = gabi::ea(this);
  push->x = (f32)mSph.mGObjTg.mRVec.x;
  push->y = (f32)mSph.mGObjTg.mRVec.y;
  push->z = (f32)mSph.mGObjTg.mRVec.z;
  f32 sq = bomb2_PSVECSquareMag_f32(push.get());
  if (sq > 32400) {
    f32 root = bomb2_sqrtf_f32( sq);
    bomb2_PSVECScale(push.get(), push.get(), 180 / root);
  }
  u32 vt = gabi::ea(gabi::at<Bomb2HitView>(hit)->__vtbl.get()), fn = gabi::at<Bomb2HitVtable>(vt)->GetShapeAttr.get();
  u32 shape = gabi::call<u32>(fn, hit);
  dir->x = (f32)l_zero.x;
  dir->z = (f32)l_zero.z;
  dir->y = (f32)l_zero.y;
  vt = gabi::ea(gabi::at<Bomb2Shape>(shape)->__vtbl.get());
  fn = gabi::at<Bomb2ShapeVtable>(vt)->GetNVec.get();
  f32 boostY = 1, boostH = 1;
  if (gabi::call<u32>(fn, shape, &current.pos, dir.get())) {
    bomb2_PSVECScale(dir.get(), dir.get(), 50.0f);
    bomb2_PSVECSquareMag_f32(&mWindVec);
    u32 actor = bomb2_dCcD_GAtTgCoCommonBase__GetAc_u32(gabi::ea(&mSph.mGObjTg));
    if (actor && (s16)gabi::at<fopAc_ac_c>(actor)->mProfName == fpcNm_PLAYER_e) {
      s16 angle = bomb2_cM_atan2s_s16((f32)dir->x, (f32)dir->z);
      u16 delta = (s16)gabi::at<fopAc_ac_c>(actor)->shape_angle.y - angle;
      f32 cosine = (f32)l_sinCos.mSinCos[(u32)delta >> 3][1];
      if (cosine > 0) {
        // HD: the vertical factor uses 0.3 while the horizontal factor uses 2.0.
        boostY = gabi::fmadds(cosine, 0.3f, 1);
        boostH = (cosine + cosine) + 1;
      }
    }
  }
  // HD: unordered squared lengths select the push branch; keep the fused launch arithmetic.
  f32 selector = 0.01f - sq;
  f32 pushScale = selector >= 0 ? 0 : 0.9f, dirScale = selector >= 0 ? 1 : 0.1f;
  bomb2_cXyz____ml(push.get(), v1.get(), pushScale);
  bomb2_cXyz____ml(dir.get(), v2.get(), dirScale);
  bomb2_cXyz____ml(v2.get(), v3.get(), boostH);
  bomb2_cXyz____pl(v1.get(), sum.get(), v3.get());
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

/* 020CA2A4 */
void daBomb2::Act_c::eff_explode() {
  if (field_0x742)
    return;
  field_0x742 = 1;
  u32 p = play2();
  s32 id = (s8)gabi::at<dComIfG_inf_c>(p)->play.mPlayerInfo[0].mCameraID;
  u32 cam = (u32)gabi::at<dComIfG_inf_c>(play2())->play.mCameraInfo[id].mpCamera;
  gabi::Local<csXyz> a;
  a->x = -(s16)gabi::at<Bomb2CameraView>(cam)->mAngle.x;
  a->y = (s16)gabi::at<Bomb2CameraView>(cam)->mAngle.y + 0x8000;
  a->z = 0;
  eff_explode_normal(a.get());
  se_explode();
  set_sound_env(255, 10);
}

/* 020CA164 */
void daBomb2::Act_c::eff_explode_normal(const csXyz *angle) {
  u32 c = gabi::ea(dComIfGp_getParticle());
  bomb2_dPa_control_c__set(c, 1, 0xB, &current.pos, angle, &scale, 255, 0, -1, 0,
             0, 0);
  c = gabi::ea(dComIfGp_getParticle());
  bomb2_dPa_control_c__setBombSmoke(c, 0x2009, &current.pos, 0, &scale, 255);
  c = gabi::ea(dComIfGp_getParticle());
  bomb2_dPa_control_c__setBombSmoke(c, 0x200A, &current.pos, 0, &scale, 255);
  c = gabi::ea(dComIfGp_getParticle());
  bomb2_dPa_control_c__set(c, 3, 0x2008, &current.pos, 0, &scale, 255, 0, -1, 0,
             0, 0);
}

/* 020CA0F8 */
void daBomb2::Act_c::eff_explode_water() {
  if (field_0x743)
    return;
  bomb2_fopKyM_createWpillar(&current.pos, 1, 1.0f, 1.0f);
  field_0x743 = 1;
  se_explode_water();
  set_sound_env(255, 10);
}

/* 020C8F2C */
void daBomb2::Act_c::eff_fuse_init() {
  f32 x = (f32)l_zero.x, y = (f32)l_zero.y,
      z = (f32)l_zero.z;
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

/* 020CA4F0 */
void daBomb2::Act_c::eff_fuse_start() {
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
    reinterpret_cast<Bomb2VectorWords*>(&field_0x6D8)->mWords[i] =
        (u32)reinterpret_cast<Bomb2VectorWords*>(&field_0x6C0)->mWords[i];
  u32 c = gabi::ea(dComIfGp_getParticle());
  bomb2_dPa_control_c__set(c, 1, 0x11, &field_0x6C0, 0, &scale, 255, &mSparks, -1,
             0, 0, 0);
  s32 room = current.roomNo;
  c = gabi::ea(dComIfGp_getParticle());
  bomb2_dPa_control_c__set(c, 3, 0x2012, &field_0x6C0, 0, &scale, 220, &mSmoke,
             room, s + 0x1A8, s + 0x1A8, 0);
  mSmoke.setOldPosP(&field_0x6CC, &field_0x6D8);
}

/* 020C8FB4 */
void daBomb2::Act_c::eff_fuse_move() {
  if (!(u32)l_fuseOffset.mGuard) {
    l_fuseOffset.mGuard = 1;
    l_fuseOffset.mOffset.z = 5;
    l_fuseOffset.mOffset.x = 0;
    l_fuseOffset.mOffset.y = 60;
  }
  u32 s = gabi::ea(this);
  for (int i = 0; i < 3; i++) {
    u32 old = (u32)reinterpret_cast<Bomb2VectorWords*>(&field_0x6CC)->mWords[i],
        cur = (u32)reinterpret_cast<Bomb2VectorWords*>(&field_0x6C0)->mWords[i];
    reinterpret_cast<Bomb2VectorWords*>(&field_0x6D8)->mWords[i] = old;
    reinterpret_cast<Bomb2VectorWords*>(&field_0x6CC)->mWords[i] = cur;
  }
  bomb2_PSMTXMultVec(gabi::ea(&l_currentMtx), gabi::ea(&l_fuseOffset.mOffset), &field_0x6C0);
  if (field_0x744)
    se_ignition();
}

/* 020C95F4 */
void daBomb2::Act_c::eff_fuse_end() {
  mSmoke.deleteCallBack();
  mSparks.deleteCallBack();
  field_0x744 = 0;
}

/* 020CA9F8 */
void daBomb2::Act_c::eff_water_splash() {
  gabi::Local<cXyz> p;
  p->x = current.pos.x;
  p->y = field_0x520;
  p->z = current.pos.z;
  bomb2_fopKyM_createWpillar(p.get(), 0, 0.5f, 0.75f);
  se_fall_water();
  set_sound_env(100, 5);
}

/* 020CA91C */
void daBomb2::Act_c::se_fall_water() {
  u32 s = gabi::ea(this), polys[2] = {field_0x526 ? 0 : gabi::ea(&mAcch.m_wtr), gabi::ea(&mAcch.m_gnd.mPolyIndex)};
  u32 material = 19;
  for (u32 poly : polys) {
    if (poly && (u16)gabi::at<cBgS_PolyInfo>(poly)->mBgIndex < 256) {
      material = bomb2_dBgS__GetMtrlSndId_u32( play2() + 0x12A0, poly);
      break;
    }
  }
  u32 rev = bomb2_dComIfGp_getReverb_u32((s32)current.roomNo);
  bomb2_mDoAud_seStartLevel(0x6918, gabi::ea(&eyePos), material, rev);
}

/* 020CA25C */
void daBomb2::Act_c::se_explode() {
  s32 reverb = bomb2_dComIfGp_getReverb_s32((s8)current.roomNo);
  bomb2_mDoAud_seStartLevel(0x6901, &eyePos, 0, reverb);
}

/* 020CA06C */
void daBomb2::Act_c::se_explode_water() {
  s32 reverb = bomb2_dComIfGp_getReverb_s32((s8)current.roomNo);
  bomb2_mDoAud_seStartLevel(0x6982, &eyePos, 0, reverb);
}

/* 020C8F6C */
void daBomb2::Act_c::se_ignition() {
  s32 reverb = bomb2_dComIfGp_getReverb_s32((s8)current.roomNo);
  bomb2_mDoAud_seStartLevel(0x6100, &eyePos, 0, reverb);
}

/* 020CA0B4 */
void daBomb2::Act_c::set_sound_env(s32 a, s32 b) {
  gabi::Local<cXyz> p;
  p->x = current.pos.x;
  p->y = current.pos.y;
  p->z = current.pos.z;
  bomb2_dKy_Sound_set(p.get(), a, (u32)mBsPcId, b);
}

/* 020CAA6C */
bool daBomb2::Act_c::chk_exp_cc_nut() {
  bool explode = false, fuse = false;
  if (bomb2_dCcD_GObjInf__ChkTgHit_u32(&mSph)) {
    u32 hit = bomb2_dCcD_GObjInf__GetTgHitObj_u32(&mSph);
    if (hit) {
      u32 f = (u32)gabi::at<dCcD_GObjInf>(hit)->mAtType;
      if (f & 0x20)
        explode = true;
      else if (!(f & 0x200000))
        fuse = true;
    }
    bomb2_dCcD_GObjInf__ClrTgHit(&mSph);
  }
  if (bomb2_dCcD_GObjInf__ChkCoHit_u32(&mSph)) {
    if (field_0x741)
      fuse = true;
    else {
      f32 mag = bomb2_PSVECSquareMag_f32(&speed);
      mag = bomb2_sqrtf_f32( mag);
      if (mag > 25)
        fuse = true;
    }
    bomb2_dCcD_GObjInf__ClrCoHit(&mSph);
  }
  if (fuse) {
    eff_fuse_start();
    set_nut_exp_interval();
  }
  if (explode)
    eff_explode();
  return explode;
}

/* 020CAB6C */
// HD: this wrapper forwards the full collision-check register.
u32 daBomb2::Act_c::chk_exp_cc() {
  if (mBombTimer <= 0)
    return 0;
  return bomb2_daBomb2__Act_c__chk_exp_cc_nut_u32(this);
}

/* 020CAB88 */
bool daBomb2::Act_c::chk_exp_bg_nut() {
  u8 water = chk_water_in();
  bool lava = chk_lava_in(), hit = (mAcch.m_flags & (0x30 | 0x200)) != 0;
  if (lava) {
    eff_explode();
    return true;
  }
  if (water) {
    f32 mag = bomb2_PSVECSquareMag_f32(&speed);
    mag = bomb2_sqrtf_f32( mag);
    if (mag > 20) {
      eff_explode_water();
      return true;
    }
  }
  if (hit) {
    bool fuse = field_0x741;
    if (!fuse) {
      f32 mag = bomb2_PSVECSquareMag_f32(&speed);
      mag = bomb2_sqrtf_f32( mag);
      fuse = mag > 25;
    }
    if (fuse) {
      eff_fuse_start();
      set_nut_exp_interval();
    }
  }
  return false;
}

/* 020CAC84 */
bool daBomb2::Act_c::chk_exp_bg() {
  return chk_exp_bg_nut();
}

/* 020CAC88 */
bool daBomb2::Act_c::chk_exp_timer() {
  if (mBombTimer > 0) {
    mBombTimer = mBombTimer - 1;
    if (mBombTimer == 0) {
      eff_explode();
      return true;
    }
  }
  return false;
}

/* 020CACCC */
bool daBomb2::Act_c::chk_sink_bg_nut() {
  if (chk_water_in()) {
    f32 mag = bomb2_PSVECSquareMag_f32(&speed);
    mag = bomb2_sqrtf_f32( mag);
    if (!(mag > 20)) {
      eff_water_splash();
      return true;
    }
  }
  return false;
}

/* 020CAD38 */
bool daBomb2::Act_c::chk_sink_bg() {
  return chk_sink_bg_nut();
}

/* 020CAD3C */
u8 daBomb2::Act_c::chk_exp_pre() {
  return chk_exp_cc() || chk_exp_timer();
}

/* 020CAD94 */
u8 daBomb2::Act_c::chk_exp_post() {
  return bomb2_chk_exp_bg_u8(this);
}

/* 020CAD98 */
u8 daBomb2::Act_c::chk_sink_post() {
  return bomb2_chk_sink_bg_u8(this);
}

/* 020C99DC */
void daBomb2::Act_c::set_real_shadow_flag() {
  u32 s = gabi::ea(this);
  bool on = false;
  if (mState == Mode_CARRY_e) {
    if ((u32)model)
      on = true;
    else {
      u32 link = gabi::ea(dComIfGp_getPlayer(0)),
          vt = (u32)gabi::at<Bomb2PlayerView>(link)->__vtbl, fn = gabi::at<Bomb2PlayerVtable>(vt)->getGrabActorID.get();
      if (gabi::call<u32>(fn, link) == (u32)mBsPcId &&
          ((u32)gabi::at<Bomb2PlayerView>(link)->mResetFlg0 & 0x8000))
        on = true;
    }
  }
  model = on ? mpModel.v.get() : 0;
}

/* 020CAD9C */
void daBomb2::Act_c::carry_fuse_start() {
  if (field_0x744)
    return;
  u32 link = gabi::ea(dComIfGp_getPlayer(0)),
      vt = (u32)gabi::at<Bomb2PlayerView>(link)->__vtbl, fn = gabi::at<Bomb2PlayerVtable>(vt)->getGrabActorID.get();
  if (gabi::call<u32>(fn, link) != (u32)mBsPcId ||
      ((u32)gabi::at<Bomb2PlayerView>(link)->mResetFlg0 & 0x20))
    eff_fuse_start();
}

/* 020C8C68 */
void daBomb2::Act_c::on_carry() {
  attention_info.flags = (u32)attention_info.flags | fopAc_Attn_ACTION_CARRY_e;
}

/* 020C8C58 */
void daBomb2::Act_c::off_carry() {
  attention_info.flags = (u32)attention_info.flags & ~fopAc_Attn_ACTION_CARRY_e;
}

/* 020CA638 */
void daBomb2::Act_c::mode_wait_init() {
  u32 flags = (u32)mSph.mObjCo.mSPrm;
  mState = Mode_WAIT_e;
  gravity = -2.9f;
  mSph.mObjCo.mSPrm = flags | 1;
}

/* 020CB044 */
void daBomb2::Act_c::mode_wait() {
  if (chk_exp_pre()) {
    mode_explode_init();
    mode_explode();
    return;
  }
  if ((u32)actor_status & fopAcStts_CARRY_e) {
    mode_carry_init();
    mode_carry();
    return;
  }
  u32 fixed = bomb2_PrmAbstract_u32(this, PRM_1_W, PRM_1_S);
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
      if (mAcch.m_flags & dBgS_Acch::GROUND_HIT) {
        on_carry();
        return;
      }
    }
    off_carry();
  }
}

/* 020C9980 */
void daBomb2::Act_c::mode_carry_init() {
  mState = Mode_CARRY_e;
  speedF = 0;
  speed.x = (f32)l_zero.x;
  speed.y = (f32)l_zero.y;
  speed.z = (f32)l_zero.z;
  off_carry();
  mSph.mObjCo.mSPrm = (u32)mSph.mObjCo.mSPrm & ~1;
}

/* 020CAE6C */
void daBomb2::Act_c::mode_carry() {
  carry_fuse_start();
  if (chk_exp_pre()) {
    mode_explode_init();
    mode_explode();
    return;
  }
  if (!((u32)actor_status & fopAcStts_CARRY_e)) {
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

/* 020CA3A4 */
void daBomb2::Act_c::mode_explode_init() {
  mState = Mode_EXPLODE_e;
  camera_lockoff();
  eff_fuse_end();
  mEnv.set(&current.pos);
  speedF = 0;
  u32 s = gabi::ea(this);
  // The optimized original copies cXyz::Zero with lwz/stw, preserving raw
  // words (including signaling NaNs), rather than lfs/stfs float transport.
  for (int i = 0; i < 3; i++)
    reinterpret_cast<Bomb2VectorWords*>(&speed)->mWords[i] = (u32)l_zeroWords.mWords[i];
  gravity = 0;
  off_carry();
  u32 co = (u32)mSph.mObjCo.mSPrm, at = (u32)mSph.mObjAt.mSPrm,
      tg = (u32)mSph.mObjTg.mSPrm;
  mSph.mObjCo.mSPrm = co & ~1;
  mSph.mObjAt.mSPrm = at | 1;
  mSph.mObjTg.mSPrm = tg & ~1;
  bomb2_fopAcM_cancelCarryNow(this);
  mBombTimer = 0;
  field_0x73C = 4;
  u32 p = play2();
  gabi::Local<cXyz> d;
  d->x = 0;
  d->y = 1;
  d->z = 0;
  bomb2_dVibration_c__StartShock( p + 0x599C, 7, -33, d.get());
}

/* 020CAE14 */
void daBomb2::Act_c::mode_explode() {
  mEnv.proc(&current.pos);
  if (field_0x73C == 0 && mEnv.is_end())
    field_0x740 = 1;
}

/* 020CAFB4 */
void daBomb2::Act_c::mode_sink_init() {
  u32 s = gabi::ea(this), co = (u32)mSph.mObjCo.mSPrm,
      at = (u32)mSph.mObjAt.mSPrm, tg = (u32)mSph.mObjTg.mSPrm;
  speed.y = (f32)speed.y * 0.8f;
  speedF = (f32)speedF * 0.8f;
  mState = Mode_SINK_e;
  mSph.mObjAt.mSPrm = at & ~1;
  mSph.mObjTg.mSPrm = tg & ~1;
  mSph.mObjCo.mSPrm = co & ~1;
  off_carry();
  bomb2_fopAcM_cancelCarryNow(this);
  field_0x698 = 4;
  bomb2_fopAcM_getWaterY(&current.pos, &field_0x528);
}

/* 020CB218 */
void daBomb2::Act_c::mode_sink() {
  gabi::Local<be<f32>> y;
  if (bomb2_fopAcM_getWaterY_u32(&current.pos, y.get()) &&
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

/* 020C9A6C */
void daBomb2::Act_c::mode_proc_call() {
  u32 s = gabi::ea(this);
  s32 state = mState;
  if ((u32)actor_status & fopAcStts_CARRY_e) {
    if (state != Mode_CARRY_e) {
      mode_carry_init();
      state = mState;
    }
  }
  invokeBomb2(s, l_modeProc.mProc[state]);
  set_real_shadow_flag();
}

/* 020C8EF4 */
void daBomb2::Act_c::tensor_init() {
  bomb2_PSMTXIdentity(gabi::ea(&field_0x754));
  vib_init();
}

/* 020C8EBC */
void daBomb2::Act_c::vib_init() {
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

/* 020C9B18 */
void daBomb2::Act_c::vib_proc() {
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

/* 020C9BB8 */
void daBomb2::Act_c::set_vib_tensor() {
  gabi::Local<cXyz> d;

  gabi::Local<Bomb2Quaternion> q;
  d->x = field_0x790;
  d->y = 1;
  d->z = field_0x78C;
  bomb2_daObj__quat_rotBaseY2(q.get(), d.get());
  bomb2_PSMTXQuat(gabi::ea(&field_0x754), q.get());
}

/* 020CB398 */
void daBomb2::Act_c::tensor_wait() {
  if (mAcch.m_flags & dBgS_Acch::GROUND_HIT)
    tensor_wait_ground();
  else
    tensor_wait_drop();
}

/* 020CB34C */
void daBomb2::Act_c::tensor_wait_drop() {
  if (field_0x7A8 > 0) {
    field_0x79C = (f32)speed.z * -0.005f;
    field_0x7A0 = (f32)speed.x * -0.005f;
  }
  field_0x788 = 0;
  field_0x7A4 = 0.1f;
  field_0x784 = 0;
}

/* 020CB2BC */
void daBomb2::Act_c::tensor_wait_ground() {
  u32 p = play2(), s = gabi::ea(this);
  u32 plane =
      bomb2_cBgS__GetTriPla_u32( p + 0x12A0, (u16)mAcch.m_gnd.mBgIndex,
                      (u16)mAcch.m_gnd.mPolyIndex);
  if (plane) {
    field_0x784 = (f32)gabi::at<Bomb2Plane>(plane)->mNormal.z * 1.5f;
    f32 x = (f32)gabi::at<Bomb2Plane>(plane)->mNormal.x * 1.5f;
    field_0x7A4 = 0.4f;
    field_0x788 = x;
  } else {
    field_0x784 = 0;
    field_0x7A4 = 0.4f;
    field_0x788 = 0;
  }
}

/* 020CB3AC */
void daBomb2::Act_c::tensor_carry() {
  field_0x788 = 0;
  field_0x7A4 = 0.4f;
  field_0x784 = 0;
}

/* 020CB3CC */
void daBomb2::Act_c::tensor_explode() {
  field_0x788 = 0;
  field_0x7A4 = 0.4f;
  field_0x784 = 0;
}

/* 020CB3EC */
void daBomb2::Act_c::tensor_sink() {
  field_0x788 = 0;
  field_0x7A4 = 0.4f;
  field_0x784 = 0;
}

/* 020C9C14 */
void daBomb2::Act_c::tensor_proc_call() {
  if (!bomb2_PrmAbstract_u32(this, PRM_1_W, PRM_1_S)) {
    invokeBomb2(gabi::ea(this), l_tensorProc.mProc[(s32)mState]);
    vib_proc();
    set_vib_tensor();
  }
}

/* 020C9E78 */
bool daBomb2::Act_c::_execute() {
  set_wind_vec();
  mode_proc_call();
  if (field_0x740) {
    bomb2_fopAcM_delete(this);
    return true;
  }
  tensor_proc_call();
  anm_play();
  f32 x = current.pos.x, y = (f32)current.pos.y + 50, z = current.pos.z;
  u32 s = gabi::ea(this);
  eyePos.x = x;
  eyePos.y = y;
  eyePos.z = z;
  attention_info.position.x = x;
  attention_info.position.y = y;
  attention_info.position.z = z;
  set_mtx();
  cc_set();
  return true;
}

/* 020C9F18 */
bool daBomb2::Act_c::is_draw() {
  return mState != Mode_EXPLODE_e && !field_0x743 && !field_0x742;
}

/* 020C9F50 */
void daBomb2::Act_c::draw_nut() {
  u32 data = (u32)gabi::at<Bomb2ModelView>(gabi::ea(mpModel.get()))->mpModelData;
  mBck0.entry(gabi::at<J3DModelData>(data), mBck0.getFrame());
  mBrk0.entry(gabi::at<J3DModelData>(data), mBrk0.getFrame());
  dComIfGd_setListP1();
  mDoExt_modelUpdateDL(mpModel.get(), 0);
  dComIfGd_setList();
  u32 obj = (u32)gabi::at<Bomb2ModelDataView>(data)->mJointTree;
  gabi::at<Bomb2JointTreeView>(obj)->mpTransformAnimator = 0;
  gabi::at<Bomb2ModelDataView>(data)->mpTevRegAnimator = 0;
}

/* 020CA000 */
bool daBomb2::Act_c::_draw() {
  if (is_draw()) {
    u32 env = bomb2_dKy_getEnvlight_u32();
    bomb2_dScnKy_env_light_c__settingTevStruct(env, 0, &current.pos, &tevStr);
    env = bomb2_dKy_getEnvlight_u32();
    bomb2_dScnKy_env_light_c_setLightTevColorType(env, mpModel.get(), &tevStr);
    draw_nut();
  }
  return true;
}

// HD: compiler-generated wrappers and template code of this translation unit.

/* 020C84D8 */
Env_c *bomb2EnvCtor(Env_c *self) {
  if (!self)
    self = gabi::at<Env_c>(bomb2_operator_new_u32(0x58));
  if (self)
    self->mPntLight.mHD20 = 1;
  return self;
}

/* 020C8560 */
void smoke_execute(void *self) { }

/* 020C8908 */
void smoke_draw(void *self) { }

/* 020C89AC */
void sparks_draw(void *self) { }

/* 020CB40C */
u32 Mthd_Create(Act_c *self) {
  return bomb2_Act_c_create_u32(self);
}

/* 020CB410 */
u32 Mthd_Delete(Act_c *self) {
  return bomb2_Act_c_delete_u32(self);
}

/* 020CB414 */
u32 Mthd_Execute(Act_c *self) {
  return bomb2_Act_c_execute_u32(self);
}

/* 020CB418 */
u32 Mthd_Draw(Act_c *self) {
  return bomb2_Act_c_draw_u32(self);
}

/* 020CB4F4 */
void bomb2_acch_virtual_first(void *self) { }

/* 020CB4F8 */
void bomb2_acch_virtual_second(void *self) { }

/* 020CB4FC */
void bomb2_acch_virtual_third(void *self) { }

/* 020CB5D8 */
bool Mthd_IsDelete(Act_c *self) {
  return true;
}

/* 020CB5E0 */
u32 bomb2PrmAbstract(const Act_c *self, u32 width, u32 shift) {
  u32 value = (u32)self->mParameters;
  u32 mask = (width & 32 ? 0 : 1u << (width & 31)) - 1;
  return (shift & 32 ? 0 : value >> (shift & 31)) & mask;
}

/* 020CB4E0 */
void bomb2GenericDtor(void *self, u32 flags) {
  if (self && (flags & 1))
    bomb2___dl(self);
}

/* 020CB41C */
void __sinit_d_a_bomb2_cpp() {
  for (u32 i = 0; i < 16; i += 4)
    l_randomState.mWords[i / 4] = 0;
  bomb2___register_global_object(gabi::ea(&l_randomRegistration));
  l_headerAngles.mNegativePi = -3.1415927410125732f;
  l_headerAngles.mPositivePi = 3.1415927410125732f;
  bomb2_randomSecondObject_ct(gabi::ea(&l_randomObjects.mSecond));
  bomb2___register_global_object(gabi::ea(&l_secondRegistration));
  bomb2_randomFirstObject_ct(gabi::ea(&l_randomObjects.mFirst));
  bomb2___register_global_object(gabi::ea(&l_firstRegistration));
  M_sph_src.mRadius = 30;
  l_cullSize.mHeight = 66;
  l_cullSize.mRadius = 36;
}

/* 020CB500 */
void bomb2ActDtor(Act_c *a, u32 flags) {
  if (!a)
    return;
  u32 s = gabi::ea(a);
  bomb2_dCcD_Sph_dtor( s + 0x760, 2);
  bomb2_dCcD_Stts_dtor( s + 0x724, 2);
  gabi::store<u32>(s + 0x6E0, 0x1000AA5C);
  gabi::store<u32>(s + 0x700, 0x1000AA7C);
  gabi::store<u32>(s + 0x70C, 0x1000AA3C);
  bomb2_cBgS_Chk___cBgS_Chk( s + 0x6C0, 0);
  bomb2_cM3dGCir_dtor( s + 0x694, 2);
  gabi::store<u32>(s + 0x4DC, 0x1000AB1C);
  gabi::store<u32>(s + 0x4D0, 0x1000AB2C);
  bomb2_dBgS_Acch_dtor( s + 0x4BC, 0);
  bomb2_J3DFrameCtrl_dtor( s + 0x3C8, 0);
  bomb2_fopAc_ac_c___fopAc_ac_c( a, 0);
  if (flags & 1)
    bomb2___dl(a);
}

/* 020C8B54 */
u32 bomb2HeapCB(Act_c *a) {
  return bomb2_create_heap_u32(a);
}

/* WWHD glue
str 1000A9F4 "VbakH"
str 1000AB80 "d_a_bomb2.cpp"
str 1000AB50 "mdl_data != (0)"
str 1000AB60 "bck_data != (0)"
str 1000AB70 "brk_data != (0)"
data l_zero 101FFBA8
data l_zeroWords 101FFBA8
data l_currentMtx 1048D0CC
data l_resourceControl 101F4F28
data l_stayRoom 1047E6C8
data l_noGradeCos 104A4F44
data l_headerAngles 104626A8
data l_randomObjects 104626B8
data l_randomState 104626BC
data l_cullSize 104626B0
data l_fuseOffset 104626D8
data l_localCenter 104626CC
data l_centerGuard 104626E8
data M_sph_src 10192378
data l_sinCos 104A44F8
data l_randomRegistration 10192324
data l_secondRegistration 10192330
data l_firstRegistration 1019233C

data l_startProc 1000ABA4
data l_modeProc 1000ABDC
data l_tensorProc 1000AC00
ext 025E83FC void daBomb2::Brk_c::entry(J3DModelData*, f32)
ext 028E9098 void bomb2_PSMTXIdentity(u32)
ext 020C89BC u32 bomb2_daBomb2__Act_c__create_heap_nut_u32(daBomb2::Act_c *)
ext 020CAC84 u8 bomb2_chk_exp_bg_u8(daBomb2::Act_c *)
ext 020CAD38 u8 bomb2_chk_sink_bg_u8(daBomb2::Act_c *)
ext 02515F14 void bomb2_dCcD_Stts__Init(dCcD_Stts *, int, int, daBomb2::Act_c *)
ext 0251677C void bomb2_dCcD_Sph__Set(dCcD_Sph *, int)
ext 0255B9C8 void bomb2_dKy_efplight_set(daBomb2::Env_c *)
ext 0257DC90 void bomb2_dKyw_pntwind_set(u32)
ext 025553B8 void bomb2_dKy_actor_addcol_set(int, int, int, float)
ext 0255BA9C void bomb2_dKy_efplight_cut(daBomb2::Env_c *)
ext 0257D1B8 void bomb2_dKyw_pntwind_cut(u32)
ext 028E8DE8 f32 bomb2_PSVECSquareDistance_f32(const cXyz *, u32)
ext 028F4384 f32 bomb2_sqrtf_f32(f32)
ext 025548F0 void bomb2_dKy_actor_addcol_amb_set(int, int, int, f32)
ext 02554B18 void bomb2_dKy_bg_addcol_amb_set(int, int, int, f32)
ext 02554C2C void bomb2_dKy_bg_addcol_dif_set(int, int, int, f32)
ext 0200ECD4 f32 bomb2_cLib_addCalc_f32(be<f32> *, float, float, float, float)
ext 0273AD10 u32 bomb2_operator_new_u32(int)
ext 020C93EC u32 bomb2_Act_c_create_u32(daBomb2::Act_c *)
ext 020C962C u32 bomb2_Act_c_delete_u32(daBomb2::Act_c *)
ext 020C9E78 u32 bomb2_Act_c_execute_u32(daBomb2::Act_c *)
ext 020CA000 u32 bomb2_Act_c_draw_u32(daBomb2::Act_c *)
ext 0273AF40 void bomb2___dl(void *)
ext 028E8DE8 f32 bomb2_PSVECSquareDistance_f32(const cXyz *, const cXyz *)
ext 0281DCB8 u32 bomb2_JPABaseEmitter__createParticle_u32(u32)
ext 02008974 f32 bomb2_cBgS__GroundCross_f32(u32, u32)
ext 0246B6A4 s32 bomb2_daSea_ChkArea_s32(f32, f32)
ext 0246BA0C f32 bomb2_daSea_calcWave_f32(f32, f32)
ext 024EF130 s32 bomb2_dBgS__GetRoomId_s32(u32, u32)
ext 024EEEB8 u32 bomb2_dBgS__GetPolyColor_u32(u32, u32)
ext 024F08A8 void bomb2_dBgS_Acch__CrrPos(dBgS_Acch *, u32)
ext 02555D0C u32 bomb2_dKy_getEnvlight_u32()
ext 025626A4 void bomb2_dScnKy_env_light_c__settingTevStruct(u32, int, cXyz *, dKy_tevstr_c *)
ext 02562F5C void bomb2_dScnKy_env_light_c_setLightTevColorType(u32, J3DModel *, dKy_tevstr_c *)
ext 02520540 s32 bomb2_dComIfGp_getReverb_s32(s8)
ext 025E1A40 void bomb2_mDoAud_seStartLevel(int, cXyz *, int, s32)
ext 0255F458 void bomb2_dKy_Sound_set(cXyz *, s32, unsigned int, s32)
ext 025DAE64 void bomb2_fopKyM_createWpillar(cXyz *, int, float, float)
ext 025A847C void bomb2_dPa_control_c__set(u32, int, int, cXyz *, const csXyz *, cXyz *, int, int, int, int, int, int)
ext 025A866C void bomb2_dPa_control_c__setBombSmoke(u32, int, cXyz *, int, cXyz *, int)
ext 025A847C void bomb2_dPa_control_c__set(u32, int, int, cXyz *, int, cXyz *, int, int, int, int, int, int)
ext 025052BC void bomb2_dCamera_c__ForceLockOff(u32, unsigned int)
ext 025D9D24 void bomb2_fopAcM_cancelCarryNow(daBomb2::Act_c *)
ext 025CB374 void bomb2_dVibration_c__StartShock(u32, int, int, cXyz *)
ext 025A847C void bomb2_dPa_control_c__set(u32, int, int, cXyz *, int, cXyz *, int, daBomb2::FuseSparksCB_c *, int, int, int, int)
ext 025A847C void bomb2_dPa_control_c__set(u32, int, int, cXyz *, int, cXyz *, int, daBomb2::FuseSmokeCB_c *, s32, u32, u32, int)
ext 026066C4 u32 bomb2_dRes_control_c__getRes_u32(unsigned int, daBomb2::Bomb2SafeString *, int)
ext 0273AA24 void bomb2_JUTAssertion__getSDevice(int, int, int)
ext 025E38E0 u32 bomb2_mDoExt_J3DModel__create_u32(u32, int, int)
ext 025E8508 u32 bomb2_mDoExt_bckAnm__init_u32(mDoExt_bckAnm *, u32, u32, int, int, int, int, int, float)
ext 025E8154 u32 bomb2_mDoExt_brkAnm_init_u32(daBomb2::Brk_c *, u32, u32, int, int, int, int, int, int, float)
ext 025D674C void bomb2_fopAcM_setCullSizeBox(daBomb2::Act_c *, f32, float, f32, f32, f32, f32)
ext 020CB5E0 u32 bomb2_PrmAbstract_u32(daBomb2::Act_c *, int, int)
ext 028E8F64 void bomb2_PSMTXMultVec(int, int, cXyz *)
ext 028E93CC void bomb2_PSMTXTrans(int, f32, f32, f32)
ext 028E9108 void bomb2_PSMTXConcat(int, u32, int)
ext 025F1B48 void bomb2_mDoMtx_ZXYrotM(int, s16, s16, s16)
ext 025E742C void bomb2_mDoExt_baseAnm__play(mDoExt_bckAnm *)
ext 025E742C void bomb2_mDoExt_baseAnm__play(daBomb2::Brk_c *)
ext 020084C8 u32 bomb2_cBgS__GetTriPla_u32(u32, unsigned short, unsigned short)
ext 025D9F70 u32 bomb2_fopAcM_getWaterY_u32(cXyz *, gabi::be<float> *)
ext 02312548 void bomb2_daObj__quat_rotBaseY2(daBomb2::Bomb2Quaternion *, cXyz *)
ext 028E8C78 void bomb2_PSMTXQuat(u32, daBomb2::Bomb2Quaternion *)
ext 028F026C void bomb2___register_global_object(int)
ext 028ED6F8 void bomb2_randomSecondObject_ct(int)
ext 028EAB2C void bomb2_randomFirstObject_ct(int)
ext 02515AE8 void bomb2_dCcD_Sph_dtor(u32, int)
ext 02515860 void bomb2_dCcD_Stts_dtor(u32, int)
ext 02008DAC void bomb2_cBgS_Chk___cBgS_Chk(u32, int)
ext 02018034 void bomb2_cM3dGCir_dtor(u32, int)
ext 024EFD9C void bomb2_dBgS_Acch_dtor(u32, int)
ext 027F3628 void bomb2_J3DFrameCtrl_dtor(u32, int)
ext 025D50BC void bomb2_fopAc_ac_c___fopAc_ac_c(daBomb2::Act_c *, int)
ext 0273AF40 void bomb2___dl(daBomb2::Act_c *)
ext 025D4ED0 void bomb2_fopAc_ac_c__fopAc_ac_c(daBomb2::Act_c *)
ext 027F2BC0 void bomb2_J3DFrameCtrl__init(u32, int)
ext 027DA984 void bomb2_J3DAnmCurve_ct(u32)
ext 025E80D0 void bomb2_mDoExt_brkAnm__init(u32)
ext 024F0474 void bomb2_dBgS_Acch__dBgS_Acch(u32)
ext 024EFE94 void bomb2_dBgS_AcchCir__dBgS_AcchCir(u32)
ext 02008E0C void bomb2_cBgS_GndChk__cBgS_GndChk(u32)
ext 0200BD2C void bomb2_cCcD_Stts_ct(u32)
ext 02515DA0 void bomb2_dCcD_GStts__dCcD_GStts(u32)
ext 025166F0 void bomb2_dCcD_Sph_ct(u32)
ext 020C84D8 void bomb2_Env_c_ctor(u32)
ext 02520460 u32 bomb2_dComIfG_resLoad_u32(u32, int)
ext 025D63E8 u32 bomb2_fopAcM_entrySolidHeap_u32(daBomb2::Act_c *, int, int)
ext 025D57E0 void bomb2_fopAcM_delete(daBomb2::Act_c *)
ext 028E8DD0 f32 bomb2_PSVECSquareMag_f32(cXyz *)
ext 023121C4 void bomb2_daObj__posMoveF_grade(daBomb2::Act_c *, dCcD_Stts *, cXyz *, u32, int, float, float, float, float)
ext 025D6870 void bomb2_fopAcM_posMoveF(daBomb2::Act_c *, dCcD_Stts *)
ext 024EECAC u32 bomb2_dBgS__GetMtrlSndId_u32(u32, u32)
ext 02520540 u32 bomb2_dComIfGp_getReverb_u32(s32)
ext 025E1A40 void bomb2_mDoAud_seStartLevel(int, u32, u32, u32)
ext 025162A4 u32 bomb2_dCcD_GObjInf__ChkTgHit_u32(dCcD_Sph *)
ext 02516300 u32 bomb2_dCcD_GObjInf__GetTgHitObj_u32(dCcD_Sph *)
ext 0251621C void bomb2_dCcD_GObjInf__ClrTgHit(dCcD_Sph *)
ext 02516464 u32 bomb2_dCcD_GObjInf__ChkCoHit_u32(dCcD_Sph *)
ext 0251641C void bomb2_dCcD_GObjInf__ClrCoHit(dCcD_Sph *)
ext 020CAA6C u32 bomb2_daBomb2__Act_c__chk_exp_cc_nut_u32(daBomb2::Act_c *)
ext 025D9F70 void bomb2_fopAcM_getWaterY(cXyz *, be<f32> *)
ext 02311CD8 void bomb2_daObj__make_land_effect(daBomb2::Act_c *, u32, float)
ext 028E90D4 void bomb2_PSMTXCopy(u32, int)
ext 025F2518 void bomb2_mDoMtx_stack_c__scaleM(f32, f32, f32)
ext 02515E50 void bomb2_dCcD_GStts__Move(u32)
ext 02018C8C void bomb2_cM3dGSph__SetR(u32, f32)
ext 02018D40 void bomb2_cM3dGSph__SetC(u32, cXyz *)
ext 0200E240 void bomb2_cCcS__Set(u32, dCcD_Sph *)
ext 02516C14 void bomb2_dCcMassS_Mng__Set(u32, dCcD_Sph *, int)
ext 028E8E64 void bomb2_PSVECScale(cXyz *, cXyz *, float)
ext 02515BBC u32 bomb2_dCcD_GAtTgCoCommonBase__GetAc_u32(u32)
ext 020195B0 s16 bomb2_cM_atan2s_s16(f32, f32)
ext 0201AE48 void bomb2_cXyz____ml(cXyz *, cXyz *, f32)
ext 0201AD78 void bomb2_cXyz____pl(cXyz *, cXyz *, cXyz *)
ext 020C8B50 u32 bomb2_create_heap_u32(daBomb2::Act_c *)
*/

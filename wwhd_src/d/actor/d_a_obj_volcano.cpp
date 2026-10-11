/* Fire Mountain exterior, WWHD.
 * HD: expanded actor/SDK layouts, material uniform color conversion, and explicit
 * compiler-generated routines. Guest reloads and write order preserve aliasing. */
#include "d/actor/d_a_obj_volcano.h"
using namespace daObjVolcano;
/* 023B3B94 */
static void color_to_float(FloatColor *output, u8 *input) {
  u32 p_input = gabi::ea(input);
  f32 r = static_cast<u8>(HD::view<HD::Color>(p_input)->r),
      g = static_cast<u8>(HD::view<HD::Color>(p_input)->g),
      b = static_cast<u8>(HD::view<HD::Color>(p_input)->b),
      a = static_cast<u8>(HD::view<HD::Color>(p_input)->a);
  output->r = r / 255.0f;
  output->g = g / 255.0f;
  output->b = b / 255.0f;
  output->a = a / 255.0f;
}
/* 023B587C */
// HD: PPC shift masking preserves the width/shift=32 edge cases.
static s32 parameter(Act_c *actor, s32 width, s32 shift) {
  u32 value = actor->mParameters,
      mask = (width & 32) ? 0 : (1u << (width & 31));
  return ((shift & 32) ? 0 : (value >> (shift & 31))) & (mask - 1);
}
/* 023B3C48 */
s32 Act_c::Mthd_Create() {
  u32 flags = actor_condition;
  if (!(flags & fopAcCnd_INIT_e)) {
    if (gabi::ea(this)) {
      dBgS_MoveBgActor::ct(this);
      __vtbl = 0x100330B0;
      mDoExt_btkAnm::ct(&mBtk);
      mDoExt_btkAnm::ct(&mFireBtk);
      HD::brkConstruct(&mFireBrk);
      dCcD_Stts_ct(&mStts);
      dCcD_Cyl_ct(&mCyl, 0x10032F78);
      flags = actor_condition;
    }
    actor_condition = flags | fopAcCnd_INIT_e;
  }
  s32 phase = dComIfG_resLoad(&mPhase, STR(0x100330A8));
  if (phase == cPhs_COMPLEATE_e) {
    phase = MoveBGCreate(STR(0x100330A8), 0x11, 0, 0xAED0);
    if (phase != cPhs_COMPLEATE_e && phase != cPhs_ERROR_e)
      JUT_ASSERT_fail(STR(0x10032F88), 0x142, STR(0x10032F9C));
  }
  return phase;
}
/* 023B3DA8 */
BOOL Act_c::Mthd_Delete() {
  BOOL result = MoveBGDelete();
  HD::deleteAudioObject(&mFirePos);
  dComIfG_resDelete(&mPhase, STR(0x100330A8));
  return result;
}
/* 023B3DFC */
BOOL Act_c::CreateHeap() {
  auto *data = (J3DModelData *)dComIfG_getObjectRes(
      STR(0x100330A8), dRes_INDEX_YKZYG_BDL_QKZYG_e, 0x10032F60);
  if (!data)
    JUT_ASSERT_fail(STR(0x10032FF0), 0xBF, STR(0x10032FE0));
  mpModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000222);
  auto *texture = (J3DAnmTextureSRTKey *)dComIfG_getObjectRes(STR(0x100330A8),
                                                              13, 0x10032F60);
  if (!texture)
    JUT_ASSERT_fail(STR(0x10032FF0), 0xCA, STR(0x10033004));
  BOOL first = mBtk.init(data, texture, true, 2, 1.0f, 0, -1, false, 0);
  auto *fireData = (J3DModelData *)dComIfG_getObjectRes(
      STR(0x100330A8), dRes_INDEX_YKZYG_BDL_YMNKZ00_e, 0x10032F60);
  if (!fireData)
    JUT_ASSERT_fail(STR(0x10032FF0), 0xE1, STR(0x10033010));
  mpFireModel = mDoExt_J3DModel__create(fireData, 0x80000, 0x11000222);
  texture = (J3DAnmTextureSRTKey *)dComIfG_getObjectRes(
      STR(0x100330A8), dRes_INDEX_YKZYG_BTK_YMNKZ00_e, 0x10032F60);
  if (!texture)
    JUT_ASSERT_fail(STR(0x10032FF0), 0xEC, STR(0x10033028));
  BOOL second =
      mFireBtk.init(fireData, texture, true, 2, 1.0f, 0, -1, false, 0);
  u32 colors = gabi::ea(dComIfG_getObjectRes(
      STR(0x100330A8), dRes_INDEX_YKZYG_BRK_YMNKZ00_e, 0x10032F60));
  if (!colors)
    JUT_ASSERT_fail(STR(0x10032FF0), 0xF3, STR(0x10033034));
  BOOL third = HD::brkInit(&mFireBrk, fireData, gabi::at<u8>(colors), true, 0,
                           1.0f, 0, -1, false, 0);
  mFireVisible = 1;
  return mpModel && mpFireModel && first && second && third;
}
/* 023B4060 */
void Act_c::set_mtx() {
  auto *stack = gabi::at<Mtx34>(0x1048D0CC);
  HD::matrixTranslate(stack, (f32)current.pos.x, (f32)current.pos.y,
                      (f32)current.pos.z);
  HD::matrixZXYrot(stack, (s16)shape_angle.x, (s16)shape_angle.y,
                   (s16)shape_angle.z);
  J3DModel_setBaseTRMtx(mpModel, stack);
  HD::matrixCopy(stack, gabi::at<Mtx34>(0x1046C904));
  HD::matrixTranslate(stack, (f32)mFirePos.x, (f32)mFirePos.y, (f32)mFirePos.z);
  HD::matrixZXYrot(stack, (s16)shape_angle.x, (s16)shape_angle.y,
                   (s16)shape_angle.z);
  J3DModel_setBaseTRMtx(mpFireModel, stack);
}
/* 023B41C0 */
void Act_c::init_mtx() {
  u32 model = gabi::ea((J3DModel *)mpModel);
  f32 y = scale.y, x = scale.x, z = scale.z;
  (HD::view<J3DModel>(model)->mBaseScale.x = x);
  (HD::view<J3DModel>(model)->mBaseScale.y = y);
  (HD::view<J3DModel>(model)->mBaseScale.z = z);
  model = gabi::ea((J3DModel *)mpFireModel);
  x = scale.x;
  z = scale.z;
  y = scale.y;
  (HD::view<J3DModel>(model)->mBaseScale.z = z);
  (HD::view<J3DModel>(model)->mBaseScale.x = x);
  (HD::view<J3DModel>(model)->mBaseScale.y = y);
  set_mtx();
}
/* 023B41FC */
void Act_c::StopFire() {
  dBgW *bg = mpBgW;
  if (bg && dBgW_ChkUsed(bg)) {
    u32 play = dComIfGp_ea();
    bg = mpBgW;
    HD::releaseBg(&HD::game(play)->mBg, bg);
  }
  for (u32 i = 0; i < 10; i++) {
    u32 emitter = gabi::ea((HD::Emitter *)mEmitters[i]);
    if (emitter) {
      u32 flags = static_cast<u32>(HD::view<HD::Emitter>(emitter)->mFlags);
      (HD::view<HD::Emitter>(emitter)->mUserWork = 0xFFFFFFFF);
      (HD::view<HD::Emitter>(emitter)->mFlags = flags | 1);
    }
  }
}
static u32 particle(Act_c *actor, u32 id, cXyz *pos, cXyz *size) {
  u32 play = dComIfGp_ea(),
      control = static_cast<u32>(HD::view<HD::Game>(play)->mpParticle);
  return HD::setParticle(gabi::at<u8>(control), 0, id, pos,
                         &actor->current.angle, size, 255, 0, -1, 0, 0, 0);
}
/* 023B4284 */
void Act_c::StartFire() {
  mFireVisible = 1;
  (mFireBrk.mFrameCtrl.mRate = 1.0f);
  (mFireBrk.mFrameCtrl.mFrame = 0.0f);
  mTimer = 0;
  dBgW *bg = mpBgW;
  if (bg && !dBgW_ChkUsed(bg)) {
    u32 play = dComIfGp_ea();
    bg = mpBgW;
    HD::registerBg(&HD::game(play)->mBg, bg, this);
  }
  mEmitters[0] = HD::view<HD::Emitter>(
      particle(this, ID_AK_SN_O_FIRESHAFTHEAD, &mHeadPos, &mParticleScale));
  mEmitters[1] = HD::view<HD::Emitter>(
      particle(this, ID_AK_SN_O_FIRESHAFTBODY, &mMiddlePos, &mParticleScale));
  mEmitters[2] = HD::view<HD::Emitter>(
      particle(this, ID_AK_SN_O_FIRESHAFTFOOT, &mFootPos, &mParticleScale));
  for (u32 i = 3; i < 10; i++)
    mEmitters[i] = HD::view<HD::Emitter>(particle(
        this, ID_AK_SN_MINIKAZANSTEAM00 + i - 3, &current.pos, nullptr));
}
/* 023B456C */
BOOL Act_c::Create() {
  u32 model = gabi::ea((J3DModel *)mpModel);
  cullMtx = model ? model + 0xC8 : 0;
  init_mtx();
  fopAcM_setCullSizeBox(this, -1600.0f, -500.0f, -1800.0f, 1700.0f, 2500.0f,
                        1900.0f);
  mStts.Init(255, 255, this);
  mCyl.Set(gabi::at<dCcD_SrcCyl>(0x101CDBA4));
  mFirePos.copy(current.pos);
  mCyl.SetC(&mFirePos);
  mHeight = static_cast<f32>(mCyl.mCyl.mHeight);
  f32 y = current.pos.y, headY = y + 5000.0f, footY = y + 2400.0f,
      x = current.pos.x;
  mMiddlePos.y = headY;
  mHeadPos.x = x;
  mFootPos.x = x;
  mRadius = static_cast<f32>(mCyl.mCyl.mRadius);
  f32 z = current.pos.z;
  mParticleScale.y = 2.0f;
  mMiddlePos.z = z;
  mFootPos.z = z;
  mHeadPos.z = z;
  mMiddlePos.x = x;
  mCyl.SetStts(&mStts);
  mHeadPos.y = headY;
  mParticleScale.z = 2.0f;
  mParticleScale.x = 2.0f;
  mFootPos.y = footY;
  for (u32 i = 0; i < 10; i++)
    mEmitters[i] = nullptr;
  s32 sw = parameter(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
  bool frozen = fopAcM_isSwitch(this, sw) != 0;
  f32 homeY = home.pos.y;
  if (frozen) {
    mFirePos.y = homeY - 2000.0f;
    StopFire();
    mOpacity = 0.0f;
    u32 save = static_cast<u32>(HD::view<HD::Word>(0x101F84DC)->value);
    s16 start = static_cast<s16>(HD::view<HD::Save>(save)->mStartPoint);
    if (start == 2 &&
        current.roomNo ==
            static_cast<s8>(HD::view<HD::Save>(save)->mRestartRoom))
      mState = FAIL_DEMO_WAIT;
    else
      mState = FREEZE;
  } else {
    mFirePos.y = homeY + 2000.0f;
    StartFire();
    mState = FIRE;
    mOpacity = 1.0f;
  }
  u32 play = dComIfGp_ea();
  mFreezeEvent =
      HD::getEventIdx(&HD::game(play)->mEventManager, STR(0x1003306C), 255);
  play = dComIfGp_ea();
  mFireEvent =
      HD::getEventIdx(&HD::game(play)->mEventManager, STR(0x1003307C), 255);
  return TRUE;
}
static void eruption_sound(Act_c *actor) {
  s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
  mDoAud_seStart(JA_SE_OBJ_VOLCANO_ERUPTION, &actor->mFirePos, 0, reverb);
  reverb = dComIfGp_getReverb(actor->current.roomNo);
  mDoAud_seStart(JA_SE_ATM_VOLCANO_MAGMA, &actor->current.pos, 0, reverb);
}
static void order(Act_c *actor, s16 event) {
  HD::orderEvent(actor, event, 255, 65535, 0, 1);
}
static void finish_event(Act_c *actor, s32 state, s16 event) {
  u32 play = dComIfGp_ea();
  BOOL done = HD::endCheck(&HD::game(play)->mEventManager, event);
  if (done) {
    play = dComIfGp_ea();
    u16 flags = static_cast<u16>(HD::view<HD::Game>(play)->mEventFlags);
    (HD::view<HD::Game>(play)->mEventFlags = flags | 8);
    actor->mState = state;
  }
}
/* 023B4834 */
void Act_c::daObjVolcano_fire_main() {
  eruption_sound(this);
  if (mCyl.ChkTgHit()) {
    void *hit = mCyl.GetTgHitObj();
    if (hit && cCcD_Obj_ChkAtType(hit, AT_TYPE_ICE_ARROW)) {
      order(this, (s16)mFreezeEvent);
      mState = FREEZE_DEMO_WAIT;
    }
  }
  mDoExt_baseAnm_play(&mFireBtk);
  HD::chaseF(&mOpacity, 1.0f, 0.01666666753590107f);
}
/* 023B4910 */
void Act_c::daObjVolcano_freeze_demo_wait() {
  if (eventInfo.checkCommandDemoAccrpt()) {
    s32 sw = parameter(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
    dComIfGs_onSwitch(sw, home.roomNo);
    mTimer = 0;
    StopFire();
    for (u32 i = 0; i < 3; i++)
      particle(this, ID_AK_SN_MINIKAZANICE00 + i, &mFirePos, &scale);
    mState = FREEZE_DEMO_MAIN;
  } else
    order(this, (s16)mFreezeEvent);
}
/* 023B4A88 */
void Act_c::daObjVolcano_freeze_demo_main() {
  mTimer = u32((s32)mTimer) + 1;
  HD::chaseF(&mOpacity, 0.0f, 0.01666666753590107f);
  s32 timer = mTimer;
  if (timer == 10) {
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mDoAud_seStart(JA_SE_OBJ_VOLCANO_CHILL, &mFirePos, 0, reverb);
  } else if (timer == 150) {
    s32 reverb = dComIfGp_getReverb(current.roomNo);
    mDoAud_seStart(JA_SE_OBJ_VOLCANO_ICE_BRK, &mFirePos, 0, reverb);
    HD::startSystemSound(JA_SE_READ_RIDDLE_1);
    mFireVisible = 0;
  } else if (timer <= 10)
    mDoExt_baseAnm_play(&mFireBtk);
  mDoExt_baseAnm_play(&mFireBrk);
  finish_event(this, FREEZE, (s16)mFreezeEvent);
}
/* 023B4BDC */
void Act_c::daObjVolcano_freeze_main() {
  s32 sw = parameter(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
  if (!fopAcM_isSwitch(this, sw)) {
    order(this, (s16)mFireEvent);
    f32 y = home.pos.y;
    mState = FIRE_DEMO_WAIT;
    mFirePos.y = y - 2000.0f;
  }
}
/* 023B4C70 */
void Act_c::daObjVolcano_fire_demo_wait() {
  if (eventInfo.checkCommandDemoAccrpt()) {
    StartFire();
    mState = FIRE_DEMO_MAIN;
  } else
    order(this, (s16)mFireEvent);
}
/* 023B4CE0 */
void Act_c::daObjVolcano_fire_demo_main() {
  eruption_sound(this);
  f32 currentY = current.pos.y, fireY = mFirePos.y;
  s16 event = mFireEvent;
  if (fireY < currentY + 2300.0f)
    mFirePos.y = fireY + 100.0f;
  finish_event(this, FIRE, event);
  mDoExt_baseAnm_play(&mFireBtk);
  HD::chaseF(&mOpacity, 1.0f, 0.01666666753590107f);
}
/* 023B4DD0 */
void Act_c::daObjVolcano_fail_demo_wait() {
  s32 sw = parameter(this, PRM_SWSAVE_W, PRM_SWSAVE_S);
  if (!fopAcM_isSwitch(this, sw)) {
    StartFire();
    mState = FAIL_DEMO_MAIN;
  }
}
/* 023B4E38 */
void Act_c::daObjVolcano_fail_demo_main() {
  eruption_sound(this);
  f32 y = current.pos.y, fireY = mFirePos.y;
  if (fireY < y + 2300.0f)
    mFirePos.y = fireY + 100.0f;
  mDoExt_baseAnm_play(&mFireBtk);
  HD::chaseF(&mOpacity, 1.0f, 0.01666666753590107f);
}
/* 023B4EEC */
BOOL Act_c::Execute(gptr<Mtx34> *output) {
  mCyl.SetC(&mFirePos);
  mCyl.SetH(mHeight);
  mCyl.SetR(mRadius);
  u32 play = dComIfGp_ea();
  HD::setCollision(&HD::game(play)->mCollision, &mCyl);
  switch ((u32)mState) {
  case FIRE:
    daObjVolcano_fire_main();
    break;
  case FREEZE_DEMO_WAIT:
    daObjVolcano_freeze_demo_wait();
    break;
  case FREEZE_DEMO_MAIN:
    daObjVolcano_freeze_demo_main();
    break;
  case FREEZE:
    daObjVolcano_freeze_main();
    break;
  case FIRE_DEMO_WAIT:
    daObjVolcano_fire_demo_wait();
    break;
  case FIRE_DEMO_MAIN:
    daObjVolcano_fire_demo_main();
    break;
  case FAIL_DEMO_WAIT:
    daObjVolcano_fail_demo_wait();
    break;
  case FAIL_DEMO_MAIN:
    daObjVolcano_fail_demo_main();
    break;
  }
  mDoExt_baseAnm_play(&mBtk);
  u32 env = HD::getEnvLight();
  u8 er = static_cast<u8>(HD::view<HD::Environment>(env)->mBG2_C0.r),
     eg = static_cast<u8>(HD::view<HD::Environment>(env)->mBG2_C0.g),
     eb = static_cast<u8>(HD::view<HD::Environment>(env)->mBG2_C0.b);
  u8 pr = static_cast<u8>(HD::view<HD::Environment>(env)->mBG2_K0.r),
     pg = static_cast<u8>(HD::view<HD::Environment>(env)->mBG2_K0.g),
     pb = static_cast<u8>(HD::view<HD::Environment>(env)->mBG2_K0.b);
  for (u32 i = 3; i < 10; i++) {
    u32 p = gabi::ea((HD::Emitter *)mEmitters[i]);
    if (!p)
      continue;
    (HD::view<HD::Emitter>(p)->mEnvColor.r = er);
    (HD::view<HD::Emitter>(p)->mEnvColor.g = eg);
    (HD::view<HD::Emitter>(p)->mEnvColor.b = eb);
    p = gabi::ea((HD::Emitter *)mEmitters[i]);
    (HD::view<HD::Emitter>(p)->mPrmColor.g = pg);
    (HD::view<HD::Emitter>(p)->mPrmColor.r = pr);
    (HD::view<HD::Emitter>(p)->mPrmColor.b = pb);
  }
  set_mtx();
  *output = gabi::at<Mtx34>(0x1046C904);
  return TRUE;
}
/* 023B5414 */
// HD: refresh the SDK material color uniform after the TEV alpha update.
static void set_material(u8 *material, u32 alpha) {
  u32 mat = gabi::ea(material);
  while (mat) {
    u32 shape = static_cast<u32>(HD::view<HD::Material>(mat)->mpShape);
    (HD::view<HD::Shape>(shape)->mVisible = alpha ? 1 : 0);
    if (alpha) {
      u32 tev = static_cast<u32>(HD::view<HD::Material>(mat)->mpTev),
          vt = static_cast<u32>(HD::view<HD::Tev>(tev)->mpVtable);
      u32 color = gabi::call_ptr<u32>(
          static_cast<u32>(HD::view<HD::TevVtable>(vt)->mGetKColor),
          gabi::at<u8>(tev), 3);
      (HD::view<HD::Color>(color)->a = (u8)alpha);
      tev = static_cast<u32>(HD::view<HD::Material>(mat)->mpTev);
      vt = static_cast<u32>(HD::view<HD::Tev>(tev)->mpVtable);
      color = gabi::call_ptr<u32>(
          static_cast<u32>(HD::view<HD::TevVtable>(vt)->mGetKColor),
          gabi::at<u8>(tev), 3);
      tev = static_cast<u32>(HD::view<HD::Material>(mat)->mpTev);
      vt = static_cast<u32>(HD::view<HD::Tev>(tev)->mpVtable);
      gabi::call_ptr(static_cast<u32>(HD::view<HD::TevVtable>(vt)->mSetKColor),
                     gabi::at<u8>(tev), 3, gabi::at<u8>(color));
      gabi::Local<FloatColor> input, output;
      color_to_float(input.get(), gabi::at<u8>(color));
      HD::linearToSrgb(output.get(), input.get(), 1.0f);
      (HD::view<HD::Material>(mat)->mUniformFlags =
           static_cast<u32>(HD::view<HD::Material>(mat)->mUniformFlags) |
           0x400);
      u32 dest = HD::getMaterialUniform(
          &HD::view<HD::Material>(mat)->mUniformFlags, 10);
      f32 opacity = static_cast<u8>(HD::view<HD::Color>(color)->a),
          r = output->r, g = output->g, b = output->b;
      (HD::view<FloatColor>(dest)->r = r);
      (HD::view<FloatColor>(dest)->g = g);
      (HD::view<FloatColor>(dest)->b = b);
      (HD::view<FloatColor>(dest)->a = opacity / 255.0f);
    }
    mat = static_cast<u32>(HD::view<HD::Material>(mat)->mpNext);
  }
}
/* 023B55B4 */
BOOL Act_c::Draw() {
  u32 env = HD::getEnvLight();
  HD::settingTevStruct(gabi::at<u8>(env), 2, &current.pos, &tevStr);
  env = HD::getEnvLight();
  J3DModel *model = mpModel;
  HD::setLightTevColor(gabi::at<u8>(env), model, &tevStr);
  env = HD::getEnvLight();
  model = mpFireModel;
  HD::setLightTevColor(gabi::at<u8>(env), model, &tevStr);
  model = mpModel;
  u32 data = gabi::ea(model->getModelData());
  f32 frame = static_cast<f32>(mBtk.mFrameCtrl.mFrame);
  mBtk.entry(gabi::at<J3DModelData>(data), frame);
  u32 play = dComIfGp_ea();
  (HD::view<HD::DrawLists>(0x104B4634)->mOpa =
       static_cast<u32>(HD::view<HD::Game>(play)->mBGOpa));
  play = dComIfGp_ea();
  (HD::view<HD::DrawLists>(0x104B4634)->mXlu =
       static_cast<u32>(HD::view<HD::Game>(play)->mBGXlu));
  u8 alpha = (u8)gabi::ftoi(255.5f * (f32)mOpacity);
  model = mpModel;
  data = gabi::ea(model->getModelData());
  u32 count = static_cast<u32>(HD::view<HD::ModelData>(data)->mJointNum),
      joint = static_cast<u32>(HD::view<HD::ModelData>(data)->mpJoints);
  if (count > 2)
    joint += 0x38;
  set_material(
      gabi::at<u8>(static_cast<u32>(HD::view<HD::Joint>(joint)->mpMaterial)),
      alpha);
  count = static_cast<u32>(HD::view<HD::ModelData>(data)->mJointNum);
  joint = static_cast<u32>(HD::view<HD::ModelData>(data)->mpJoints);
  if (count > 1)
    joint += 0x1C;
  set_material(
      gabi::at<u8>(static_cast<u32>(HD::view<HD::Joint>(joint)->mpMaterial)),
      alpha);
  model = mpModel;
  HD::modelUpdateDL(model, 0);
  if (mFireVisible) {
    model = mpFireModel;
    frame = static_cast<f32>(mFireBtk.mFrameCtrl.mFrame);
    data = gabi::ea(model->getModelData());
    mFireBtk.entry(gabi::at<J3DModelData>(data), frame);
    model = mpFireModel;
    frame = static_cast<f32>(mFireBrk.mFrameCtrl.mFrame);
    data = gabi::ea(model->getModelData());
    HD::brkEntry(&mFireBrk, gabi::at<J3DModelData>(data), frame);
    model = mpFireModel;
    HD::modelUpdateDL(model, 0);
  }
  play = dComIfGp_ea();
  (HD::view<HD::DrawLists>(0x104B4634)->mOpa =
       static_cast<u32>(HD::view<HD::Game>(play)->mOpa));
  play = dComIfGp_ea();
  (HD::view<HD::DrawLists>(0x104B4634)->mXlu =
       static_cast<u32>(HD::view<HD::Game>(play)->mXlu));
  return TRUE;
}
/* 023B5728 */
static s32 create(Act_c *actor) { return actor->Mthd_Create(); }
/* 023B572C */
static BOOL remove(Act_c *actor) { return actor->Mthd_Delete(); }
/* 023B5730 */
static BOOL execute(Act_c *actor) { return actor->MoveBGExecute(); }
/* 023B5734 */
static BOOL draw(Act_c *actor) { return actor->Draw_v(); }
/* 023B5744 */
static BOOL is_delete(Act_c *actor) { return actor->IsDelete_v(); }
/* 023B5754 */
static void __sinit_d_a_obj_volcano_cpp() {
  for (u32 off : {8u, 0u, 12u, 4u})
    (HD::view<HD::HeaderStatics>(0x1046C8E8)->mWords[off / 4] = 0);
  HD::safeStringConstruct(gabi::at<u8>(0x101CDBE8));
  (HD::view<HD::HeaderStatics>(0x1046C8E8)->mMinAngle = -3.1415927410125732f);
  (HD::view<HD::HeaderStatics>(0x1046C8E8)->mMaxAngle = 3.1415927410125732f);
  HD::directionConstruct(gabi::at<u8>(0x1046C8F0));
  HD::safeStringConstruct(gabi::at<u8>(0x101CDBF4));
  HD::axisConstruct(gabi::at<u8>(0x1046C8F1));
  HD::safeStringConstruct(gabi::at<u8>(0x101CDC00));
}
/* 023B57E8 */
static void HD_empty_destructor(u8 *object, s32 flags) {
  if (object && (flags & 1))
    HD::deleteObject(object);
}
/* 023B57FC */
static BOOL HD_moveBgIsDelete(Act_c *actor) { return TRUE; }
/* 023B5804 */
static void HD_moveBgDestructorBody(Act_c *actor) {}
/* 023B5808 */
BOOL Act_c::Delete() { return TRUE; }
/* 023B5810 */
static void Act_c_dt(Act_c *actor, s32 flags) {
  if (actor) {
    dCcD_Cyl_dt(&actor->mCyl, 2);
    dCcD_Stts_dt(&actor->mStts, 2);
    HD::moveBgDestruct(actor, 0);
    if (flags & 1)
      HD::deleteObject(actor);
  }
} // namespace daObjVolcano

/* WWHD glue (tools/verify/autoglue.py): data accessed through typed guest views. */

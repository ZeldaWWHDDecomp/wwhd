/** Tower of the Gods large gate. GameCube-style source verified against HD. */
#include "d/actor/d_a_obj_htetu1.h"
#include "gc_api/gc_api.h"
#include <cmath>
const gc_tu_data gc_tu = {.safeString = 0x1002AF88};
static const char *l_arcName = "Htetu1";
static cXyz l_baseY;
static cXyz l_baseZ;
static HtetuSinTableHD l_sinTable;
static HtetuParticleIdsHD l_particleIds;
static HtetuHeaderStaticsHD l_headerStatics;
static HtetuRegistrationTableHD l_registration;
static HtetuAngleLimitsHD l_angleLimits;
void *daObjHtetu1Splash_c__ct(void *object);
void daObjHtetu1Splash_c_dtor(void *object, u32 flags);
using HtetuCtorCallback = void *(*)(void *);
using HtetuDtorCallback = void (*)(void *, u32);
using HtetuHeapCallback = BOOL (*)(daObjHtetu1_c *);

dBgW *htetu1_createBackground(cBgD_t *mesh, u32 flags, Mtx34 *matrix);
void htetu1_ZXYrotM(Mtx34 *matrix, s32 x, s32 y, s32 z);
void htetu1_constructSplashArray(daObjHtetu1Splash_c *first, u32 count,
                                 u32 size, HtetuCtorCallback ctor);
s32 htetu1_entryHeap(daObjHtetu1_c *actor, HtetuHeapCallback callback,
                     u32 size);
s32 htetu1_getEventIdx(void *manager, const char *name, u32 priority);
void htetu1_stopQuake(void *vibration, s32 flags);
void htetu1_subtractVectors(const cXyz *left, const cXyz *right, cXyz *result);
void htetu1_constructWaterCheck(HtetuWaterCheckHD *check);
s32 htetu1_checkWater(cXyz *position, HtetuWaterCheckHD *check, f32 offset);
void htetu1_deleteWaterBase(HtetuWaterBaseHD *base, u32 flags);
void htetu1_orderEvent(daObjHtetu1_c *actor, s32 event, s32 priority, s32 flags,
                       s32 parameter, s32 enable);
void htetu1_scaleVector(const cXyz *source, cXyz *result, f32 scale);
void htetu1_playOpeningSound(u32 sound);
void htetu1_startShock(void *vibration, s32 strength, s32 flags,
                       cXyz *direction);
void htetu1_startQuake(void *vibration, s32 strength, s32 flags,
                       cXyz *direction);
s32 htetu1_eventEnded(void *manager, s32 event);
void htetu1_registerObject(HtetuRegistrationHD *registration);
void htetu1_initRandom(be<u8> *state);
void htetu1_initMath(be<u8> *state);
void htetu1_constructFollow(void *callback, u32 flags, u32 parameter);
void htetu1_deleteSplashArray(daObjHtetu1Splash_c *first, u32 count, u32 size,
                              HtetuDtorCallback destructor, u32 flags,
                              u32 parameter);
void htetu1_deleteActor(void *actor, u32 flags);
BOOL htetu1_nativeCreateHeap(daObjHtetu1_c *actor);
void htetu1_nativeInitMtx(daObjHtetu1_c *actor);
void htetu1_nativeCreateSplash(daObjHtetu1Splash_c *splash, u32 id,
                               cXyz *position, csXyz *angle, dKy_tevstr_c *tev);
f32 htetu1_nativeWaterHeight(daObjHtetu1_c *actor);
void htetu1_nativeUnlock(daObjHtetu1_c *actor);
void htetu1_nativeSplashManager(daObjHtetu1_c *actor);
s32 htetu1_nativeCreate(daObjHtetu1_c *actor);
BOOL htetu1_nativeDelete(daObjHtetu1_c *actor);
BOOL htetu1_nativeExecute(daObjHtetu1_c *actor);
BOOL htetu1_nativeDraw(daObjHtetu1_c *actor);
void htetu1_setParticle(dPa_control_c *control, u32 group, u32 id,
                        cXyz *position, csXyz *rotation, u32 scale, u32 alpha,
                        daObjHtetu1Splash_c *callback, s32 setup, u32 primary,
                        u32 environment, u32 scale2D);

s32 htetu1_isSwitch(dSv_info_c *save, u32 bit, s32 room);
void htetu1_registerBackground(dBgS *manager, dBgW *background,
                               daObjHtetu1_c *actor);
void htetu1_releaseBackground(dBgS *manager, dBgW *background);
void htetu1_moveBackground(dBgW *background);
void htetu1_calcModel(J3DModel *model);
void htetu1_YrotS(Mtx34 *matrix, s32 angle);
void htetu1_addCalc(be<f32> *value, f32 target, f32 scale, f32 maxStep,
                    f32 minStep);
/* inlined into _delete */
// HD: callback removal remains a native virtual call.
void daObjHtetu1Splash_c::delete_s() {
  if (mpEmitter) {
    remove();
    mPlaying = 0;
  }
}
/* inlined into _execute */
void daObjHtetu1Splash_c::timer_play_particle(s16 timer) {
  HtetuEmitterHD *e = getEmitterHD();
  if (e) {
    e->mFlags &= ~1u;
    mTimer = timer;
    mPlaying = 1;
  }
}
/* 0235C534 */
void daObjHtetu1Splash_c::create_s(u16 id, cXyz *pos, csXyz *rot, void *tev) {
  mPosition.copy(*pos);
  mRotation.x = rot->x;
  mRotation.y = rot->y;
  mRotation.z = rot->z;
  dComIfG_inf_c *game = (dComIfG_inf_c *)dComIfGp_get();
  dPa_control_c *particles = game->play.mParticle;
  htetu1_setParticle(particles, 0, (u32)id, &mPosition, &mRotation, 0, 255,
                     this, -1, 0, 0, 0);
  HtetuEmitterHD *emitter = getEmitterHD();
  if (emitter) {
    // HD: the emitter takes the low byte of each signed-10-bit light color.
    HtetuLightBytesHD *light = (HtetuLightBytesHD *)tev;
    u8 r = (u8)light->mRed, b = (u8)light->mBlue;
    emitter->mRed = r;
    u8 g = (u8)light->mGreen;
    emitter->mBlue = b;
    emitter->mGreen = g;
    emitter = getEmitterHD();
    if (emitter)
      emitter->mFlags |= 1u;
  }
  mPlaying = 0;
  mTimer = -2;
}
/* 0235C6FC */
static BOOL solidHeapCB(daObjHtetu1_c *gate) { return htetu1_nativeCreateHeap(gate); }
/* 0235C624 */
BOOL daObjHtetu1_c::create_heap() {
  J3DModelData *data = (J3DModelData *)dComIfG_getObjectRes(
      l_arcName, dRes_INDEX_HTETU1_BDL_HTETU1_e);
  if (!data) {
    JUTAssertion::showAssert("d_a_obj_htetu1.cpp", 0x11A, "mdl_data != 0");
    return 0;
  }
  mpModel = mDoExt_J3DModel__create(data, 0, 0x11020203);
  cBgD_t *dzb =
      (cBgD_t *)dComIfG_getObjectRes(l_arcName, dRes_INDEX_HTETU1_DZB_HTETU1_e);
  J3DModel *model = mpModel;
  mpBgW = htetu1_createBackground(dzb, 1, model ? model->getBaseTRMtx() : NULL);
  return mpBgW != 0;
}
/* 0235C7E8 */
s32 daObjHtetu1_c::_create() {
  if (!((u32)actor_condition & 8)) {
    u32 flags = (u32)actor_condition;
    if (gabi::ea(this)) {
      fopAc_ac_c_ct(this);
      __vtbl = 0x1002B010;
      htetu1_constructSplashArray(&mSplash[0], 2, 0x2C,
                                  daObjHtetu1Splash_c__ct);
      flags = (u32)actor_condition;
    }
    actor_condition = flags | 8;
  }
  s32 phase = dComIfG_resLoad(&mPhase, l_arcName);
  if (phase != cPhs_COMPLEATE_e)
    return phase;
  if (!htetu1_entryHeap(this, solidHeapCB, 0xAE0))
    return cPhs_ERROR_e;
  J3DModel *model = mpModel;
  f32 height = current.pos.y - 2300.0f;
  u32 sw = (u8)(u32)mParameters;
  cullMtx = model ? gabi::ea(model->getBaseTRMtx()) : 0;
  mSwitchBit = sw;
  mFloorHeight = height;
  dSv_info_c *save = dComIfGs_info();
  s32 room = (s8)home.roomNo;
  if (htetu1_isSwitch(save, sw, room)) {
    f32 y = mFloorHeight;
    mEventState = EVENT_DONE;
    current.pos.y = y;
  }
  mQuakeTimer = -1;
  htetu1_nativeInitMtx(this);
  dScnKy_env_light_c *light = dKy_getEnvlight();
  settingTevStruct(light, 2, &current.pos, &tevStr);
  for (u32 i = 0; i < 2; ++i) {
    u16 id = (u16)l_particleIds.mIds[i];
    htetu1_nativeCreateSplash(&mSplash[i], (u32)id, &current.pos,
                              &current.angle, &tevStr);
  }
  fopAcM_setCullSizeBox(this, -950.0f, -1000.0f, -100.0f, 950.0f, 1300.0f,
                        100.0f);
  dComIfG_inf_c *game = (dComIfG_inf_c *)dComIfGp_get();
  htetu1_registerBackground((dBgS *)&game->play, mpBgW.get(), this);
  game = (dComIfG_inf_c *)dComIfGp_get();
  mEventIndex = htetu1_getEventIdx(&game->play.mEvtManager, "htetu1_open", 255);
  return cPhs_COMPLEATE_e;
}
/* 0235C9C8 */
BOOL daObjHtetu1_c::_delete() {
  if (mQuakeTimer > 0) {
    dComIfG_inf_c *game = (dComIfG_inf_c *)dComIfGp_get();
    htetu1_stopQuake(&game->play.mVibration, -1);
    mQuakeTimer = -1;
  }
  for (auto &s : mSplash)
    s.delete_s();
  if (heap.get() && mpBgW) {
    dBgW *bg = mpBgW;
    if ((u32)bg->m_id < 256) {
      dComIfG_inf_c *game = (dComIfG_inf_c *)dComIfGp_get();
      htetu1_releaseBackground((dBgS *)&game->play, mpBgW.get());
      mpBgW = 0;
    }
  }
  dComIfG_resDelete(&mPhase, l_arcName);
  return 1;
}
/* 0235C700 */
void daObjHtetu1_c::init_mtx() {
  // GHS loads all three scale components before storing any of them.
  J3DModel *model = mpModel;
  f32 scaleX = scale.x, scaleY = scale.y, scaleZ = scale.z;
  model->mBaseScale.x = scaleX;
  model->mBaseScale.y = scaleY;
  model->mBaseScale.z = scaleZ;
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
  htetu1_ZXYrotM(mDoMtx_stack_c::get(), (s32)(s16)shape_angle.x,
                 (s32)(s16)shape_angle.y, (s32)(s16)shape_angle.z);
  mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
  htetu1_calcModel(mpModel.get());
}
/* 0235CAB8 */
void daObjHtetu1_c::unlock() {
  cXyz shake;
  shake.x = (f32)l_baseY.x;
  shake.y = (f32)l_baseY.y;
  shake.z = (f32)l_baseY.z;
  htetu1_subtractVectors(&mMovingPosition, &mShakeOffset, &mMovingPosition);
  u32 angle = ((u32)(u16)mUnlockTimer * 0x859) & 0xFFFF;
  f32 amplitude = mShakeAmplitude;
  f32 sine = (f32)l_sinTable.mEntries[angle >> 3].mSin;
  f32 scale = std::fabs((f32)(s16)gabi::ftoi(amplitude * sine));
  PSVECScale(&shake, &shake, scale);
  PSVECAdd(&mMovingPosition, &shake, &mMovingPosition);
  mShakeOffset.copy(shake);
  htetu1_addCalc(&mShakeAmplitude, 0.0f, 0.13f, 50.0f, 1.0f);
}
/* 0235CBC4 */
f32 daObjHtetu1_c::get_water_h() {
  HtetuWaterCheckHD water;
  cXyz pos;
  cXyz offset;
  htetu1_constructWaterCheck(&water);

  s32 angle = (s16)current.angle.y;
  pos.x = (f32)home.pos.x;
  pos.z = (f32)home.pos.z;
  pos.y = (f32)home.pos.y;
  f32 height = current.pos.y;
  htetu1_YrotS(mDoMtx_stack_c::get(), angle);
  PSMTXMultVec(mDoMtx_stack_c::get(), &l_baseZ, &offset);
  PSVECScale(&offset, &offset, 400.0f);
  PSVECAdd(&pos, &offset, &pos);
  if (htetu1_checkWater(&pos, &water, 1.0f))
    height = (f32)water.mHeight;

  water.mCheckVtable = 0x1002AFB0;
  water.mPolyVtable = 0x1002AFD0;
  water.mObjectVtable = 0x1002AFA0;
  htetu1_deleteWaterBase(&water.mBase, 0);
  return height;
}
/* 0235CCCC */
void daObjHtetu1_c::splash_manager() {
  f32 height = htetu1_nativeWaterHeight(this);
  for (auto &s : mSplash) {
    s.mPosition.y = height;
    s16 timer = s.mTimer;
    if (timer < 0 && timer != -1)
      continue;
    if (timer == 0) {
      if (s.mPlaying && s.mpEmitter) {
        HtetuEmitterHD *e = s.getEmitterHD();
        e->mFlags |= 1u;
        s.mPlaying = 0;
      }
      continue;
    }
    bool above = current.pos.y + 1400.0f > height;
    if (above) {
      if (!s.mPlaying && s.mpEmitter) {
        HtetuEmitterHD *e = s.getEmitterHD();
        e->mFlags &= ~1u;
        s.mPlaying = 1;
      }
    } else if (s.mPlaying && s.mpEmitter) {
      HtetuEmitterHD *e = s.getEmitterHD();
      e->mFlags |= 1u;
      s.mPlaying = 0;
    }
    if (timer > 0)
      s.mTimer = (s16)((s16)s.mTimer - 1);
  }
}
/* 0235CDF0 */
BOOL daObjHtetu1_c::_execute() {
  u8 state = mEventState;
  if (state == EVENT_WAIT) {
    dSv_info_c *save = dComIfGs_info();
    s32 room = (s8)home.roomNo;
    u32 sw = mSwitchBit;
    if (htetu1_isSwitch(save, sw, room)) {
      if ((u16)eventInfo.mCommand != dEvtCmd_INDEMO_e) {
        s32 event = mEventIndex;
        htetu1_orderEvent(this, event, 255, 65535, 0, 1);
        eventInfo.mCondition |= dEvtCnd_UNK2_e;
      } else {
        mUnlockTimer = 70;
        mShakeAmplitude = 50.0f;
        cXyz shake;
        // HD: retain the native outgoing linkage slot below the live vector.
        cXyz linkage;
        htetu1_scaleVector(&l_baseY, &shake, 50.0f);
        mShakeOffset.copy(shake);
        htetu1_playOpeningSound(0x806);
        mMotionState = MOTION_SHAKE;
        mEventState = EVENT_OPEN;
        dComIfG_inf_c *game = (dComIfG_inf_c *)dComIfGp_get();
        shake.x = 0;
        shake.y = 1;
        shake.z = 0;
        htetu1_startShock(&game->play.mVibration, 5, -33, &shake);
        for (auto &s : mSplash)
          s.timer_play_particle(30);
      }
    }
  } else if (state == EVENT_OPEN) {
    s32 event = mEventIndex;
    dComIfG_inf_c *game = (dComIfG_inf_c *)dComIfGp_get();
    if (htetu1_eventEnded(&game->play.mEvtManager, event)) {
      game = (dComIfG_inf_c *)dComIfGp_get();
      game->play.mEvtCtrl.mEventFlag |= 8;
      mEventState = EVENT_DONE;
    }
  }
  f32 y = current.pos.y;
  mMovingPosition.x = current.pos.x;
  mMovingPosition.y = y;
  mMovingPosition.z = current.pos.z;
  u8 motion = mMotionState;
  if (motion == MOTION_SHAKE) {
    htetu1_nativeUnlock(this);
    u16 timer = mUnlockTimer;
    if (timer) {
      s32 room = (s8)current.roomNo;
      mUnlockTimer = timer - 1;
      s32 reverb = dComIfGp_getReverb(room);
      mDoAud_seStart(0x107B, &current.pos, 0, reverb);
    } else {
      dComIfG_inf_c *game = (dComIfG_inf_c *)dComIfGp_get();
      cXyz direction;
      direction.x = 0;
      direction.y = 1;
      direction.z = 0;
      htetu1_startQuake(&game->play.mVibration, 6, 3, &direction);
      mMotionState = MOTION_LOWER;
      mQuakeTimer = 200;
      for (auto &s : mSplash)
        s.timer_play_particle(-1);
    }
    y = mMovingPosition.y;
  } else if (motion == MOTION_LOWER) {
    mMovingPosition.y = y - 5.0f;
    s32 room = (s8)current.roomNo;
    s32 reverb = dComIfGp_getReverb(room);
    mDoAud_seStart(0x107B, &current.pos, 0, reverb);
    y = mMovingPosition.y;
    f32 floor = mFloorHeight;
    if (!(y > floor)) {
      for (auto &s : mSplash)
        s.delete_s();
      y = mFloorHeight;
      mMotionState = MOTION_WAIT;
      mMovingPosition.y = y;
    }
  }
  current.pos.x = mMovingPosition.x;
  current.pos.y = y;
  current.pos.z = mMovingPosition.z;
  htetu1_nativeInitMtx(this);
  htetu1_nativeSplashManager(this);
  s16 quake = mQuakeTimer;
  if (quake == 0) {
    dComIfG_inf_c *game = (dComIfG_inf_c *)dComIfGp_get();
    htetu1_stopQuake(&game->play.mVibration, -1);
    mQuakeTimer = -1;
  } else if (quake > 0)
    mQuakeTimer = quake - 1;
  if (heap.get()) {
    dBgW *bg = mpBgW;
    if (bg && (u32)bg->m_id < 256)
      htetu1_moveBackground(bg);
  }
  return 1;
}
/* 0235D268 */
BOOL daObjHtetu1_c::_draw() {
  dScnKy_env_light_c *light = dKy_getEnvlight();
  settingTevStruct(light, 1, &current.pos, &tevStr);
  light = dKy_getEnvlight();
  setLightTevColorType(light, mpModel.get(), &tevStr);
  mDoExt_modelUpdateDL(mpModel.get(), 0);
  return 1;
}
/* 0235D2C4 */
static s32 Mthd_Create(daObjHtetu1_c *gate) { return htetu1_nativeCreate(gate); }
/* 0235D2C8 */
static BOOL Mthd_Delete(daObjHtetu1_c *gate) { return htetu1_nativeDelete(gate); }
/* 0235D2CC */
static BOOL Mthd_Execute(daObjHtetu1_c *gate) { return htetu1_nativeExecute(gate); }
/* 0235D2D0 */
static BOOL Mthd_Draw(daObjHtetu1_c *gate) { return htetu1_nativeDraw(gate); }
/* ---------------- HD: compiler-generated code of this translation unit
 * ---------------- */
/* 0235D458 */
static BOOL Mthd_IsDelete() { return 1; }
/* ---------------- HD: compiler-generated code of this translation unit
 * ---------------- */

/* 0235D2D4 */
static static void __sinit_d_a_obj_htetu1_cpp() {
  l_headerStatics.mObject[2] = 0;
  l_headerStatics.mObject[0] = 0;
  l_headerStatics.mObject[3] = 0;
  l_headerStatics.mObject[1] = 0;
  htetu1_registerObject(&l_registration.mEntries[0]);
  l_headerStatics.mMinAngle = l_angleLimits.mMinAngle;
  l_headerStatics.mMaxAngle = l_angleLimits.mMaxAngle;
  htetu1_initRandom(&l_headerStatics.mRandomState);
  htetu1_registerObject(&l_registration.mEntries[1]);
  htetu1_initMath(&l_headerStatics.mMathState);
  htetu1_registerObject(&l_registration.mEntries[2]);
}
/* 0235D368 */
static void SafeString_dtor(void *object, u32 flags) {
  if (object && (flags & 1))
    operator_delete(object);
}
/* 0235D37C */
void *daObjHtetu1Splash_c__ct(void *object) {
  if (!object)
    object = operator_new(0x2C);
  if (object)
    htetu1_constructFollow(object, 0, 0);
  return object;
}
/* 0235D3CC */
void daObjHtetu1Splash_c_dtor(void *object, u32 flags) {
  if (object && (flags & 1))
    operator_delete(object);
}
/* 0235D3E0 */
void daObjHtetu1_c_dtor(void *object, u32 flags) {
  if (object) {
    htetu1_deleteSplashArray(((daObjHtetu1_c *)object)->mSplash, 2, 0x2C,
                             daObjHtetu1Splash_c_dtor, 0, 0);
    htetu1_deleteActor(object, 0);
    if (flags & 1)
      operator_delete(object);
  }
}
/* 0235D454 */
static void SafeString_assureTerminationImpl(void *object) {}
/* WWHD glue (tools/verify/autoglue.py): this translation unit's data and private native adapters
data l_baseY 101FFBC0
data l_baseZ 101FFBCC
data l_sinTable 104A44F8
data l_particleIds 1002AF84
data l_headerStatics 1046A020
data l_registration 101CA2F0
data l_angleLimits 1002B098
str 1002B0A0
str 1002B030
str 1002B020
str 1002B060
ext 0200ECD4 void htetu1_addCalc(be<f32>* value,f32 target,f32 scale,f32 maxStep,f32 minStep)
ext 024F2478 dBgW* htetu1_createBackground(cBgD_t* mesh,u32 flags,Mtx34* matrix)
ext 025F1B48 void htetu1_ZXYrotM(Mtx34* matrix,s32 x,s32 y,s32 z)
ext 028EFFD0 void htetu1_constructSplashArray(daObjHtetu1Splash_c* first,u32 count,u32 size,HtetuCtorCallback ctor)
ext 025D63E8 s32 htetu1_entryHeap(daObjHtetu1_c* actor,HtetuHeapCallback callback,u32 size)
ext 02543F10 s32 htetu1_getEventIdx(void* manager,const char* name,u32 priority)
ext 025CB610 void htetu1_stopQuake(void* vibration,s32 flags)
ext 028E8DAC void htetu1_subtractVectors(const cXyz* left,const cXyz* right,cXyz* result)
ext 024F22DC void htetu1_constructWaterCheck(HtetuWaterCheckHD* check)
ext 024F15A4 s32 htetu1_checkWater(cXyz* position,HtetuWaterCheckHD* check,f32 offset)
ext 02008B4C void htetu1_deleteWaterBase(HtetuWaterBaseHD* base,u32 flags)
ext 025D7A58 void htetu1_orderEvent(daObjHtetu1_c* actor,s32 event,s32 priority,s32 flags,s32 parameter,s32 enable)
ext 0201AE48 void htetu1_scaleVector(const cXyz* source,cXyz* result,f32 scale)
ext 025E1988 void htetu1_playOpeningSound(u32 sound)
ext 025CB374 void htetu1_startShock(void* vibration,s32 strength,s32 flags,cXyz* direction)
ext 025CB408 void htetu1_startQuake(void* vibration,s32 strength,s32 flags,cXyz* direction)
ext 025440C8 s32 htetu1_eventEnded(void* manager,s32 event)
ext 028F026C void htetu1_registerObject(HtetuRegistrationHD* registration)
ext 028ED6F8 void htetu1_initRandom(be<u8>* state)
ext 028EAB2C void htetu1_initMath(be<u8>* state)
ext 025A5894 void htetu1_constructFollow(void* callback,u32 flags,u32 parameter)
ext 028F0164 void htetu1_deleteSplashArray(daObjHtetu1Splash_c* first,u32 count,u32 size,HtetuDtorCallback destructor,u32 flags,u32 parameter)
ext 025D50BC void htetu1_deleteActor(void* actor,u32 flags)
ext 0235C624 BOOL htetu1_nativeCreateHeap(daObjHtetu1_c* actor)
ext 0235C700 void htetu1_nativeInitMtx(daObjHtetu1_c* actor)
ext 0235C534 void htetu1_nativeCreateSplash(daObjHtetu1Splash_c* splash,u32 id,cXyz* position,csXyz* angle,dKy_tevstr_c* tev)
ext 0235CBC4 f32 htetu1_nativeWaterHeight(daObjHtetu1_c* actor)
ext 0235CAB8 void htetu1_nativeUnlock(daObjHtetu1_c* actor)
ext 0235CCCC void htetu1_nativeSplashManager(daObjHtetu1_c* actor)
ext 0235C7E8 s32 htetu1_nativeCreate(daObjHtetu1_c* actor)
ext 0235C9C8 BOOL htetu1_nativeDelete(daObjHtetu1_c* actor)
ext 0235CDF0 BOOL htetu1_nativeExecute(daObjHtetu1_c* actor)
ext 0235D268 BOOL htetu1_nativeDraw(daObjHtetu1_c* actor)
ext 025A847C void htetu1_setParticle(dPa_control_c* control,u32 group,u32 id,cXyz* position,csXyz* rotation,u32 scale,u32 alpha,daObjHtetu1Splash_c* callback,s32 setup,u32 primary,u32 environment,u32 scale2D)
ext 025BA0C0 s32 htetu1_isSwitch(dSv_info_c* save,u32 bit,s32 room)
ext 024EEA6C void htetu1_registerBackground(dBgS* manager,dBgW* background,daObjHtetu1_c* actor)
ext 020087EC void htetu1_releaseBackground(dBgS* manager,dBgW* background)
ext 024F43DC void htetu1_moveBackground(dBgW* background)
ext 027F4D5C void htetu1_calcModel(J3DModel* model)
ext 025F1884 void htetu1_YrotS(Mtx34* matrix,s32 angle)
*/

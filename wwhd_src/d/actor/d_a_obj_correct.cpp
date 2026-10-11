// Placement correction trigger for movable boxes and Try objects. Verified WWHD behavior.
#include "d/actor/d_a_obj_correct.h"
#include "bindings.h"
#include "d/actor/d_a_obj_try.h"
#include "d/actor/d_a_player.h"
using daObjCorrect::Act_c;
// HD: actor members follow the expanded 0x3AC actor base.
static CorrectAttributes L_attr;
static CorrectSearchTable M_search_proc;
static CorrectModeTable mode_proc;
static CorrectDemoTable demo_proc;
static CorrectTryTypes type;
static CorrectSaveData* sSaveData;
static CorrectHeaderStatics sHeaderStatics;
static CorrectDestructorRecords sDestructorRecords;
static s32 abstractBits(fopAc_ac_c *a, s32 width, s32 shift) {
  u32 bits = a->mParameters, w = u32(width) & 63, sh = u32(shift) & 63;
  u32 mask = w >= 32 ? 0 : 1u << w;
  return (sh >= 32 ? 0 : bits >> sh) & (mask - 1);
}
/* 02333CA0 */
s32 daObjCorrect::Act_c::prm_get(s32 w, s32 s) {
  return abstractBits(this, w, s);
}
/* 02333CBC */
static s32 tryParameter(fopAc_ac_c *a, s32 w, s32 s) {
  return abstractBits(a, w, s);
}
/* 02333CD8 */
static s32 boxParameter(fopAc_ac_c *a, s32 w, s32 s) {
  return abstractBits(a, w, s);
}


/* 02332DA0 */
void daObjCorrect::Act_c::mode_on_init() {
  f32 radius = L_attr.mEntries[(s32)mType].mOnRadiusSq;
  mMode = MODE_ON;
  mRadiusSq = radius;
}
/* 02332DC4 */
void daObjCorrect::Act_c::mode_off_init() {
  f32 radius = L_attr.mEntries[(s32)mType].mOffRadiusSq;
  mMode = MODE_OFF;
  mRadiusSq = radius;
}
/* 02332DE8 */
void daObjCorrect::Act_c::demo_non_init() {
  mDemo = DEMO_NON;
}
/* 02332DF4 */
s32 daObjCorrect::Act_c::_create() {
  s32 sw = prm_get(8, 8);
  if (!(u32(actor_condition) & fopAcCnd_INIT_e)) {
    if (this) {
      correctConstructActor(this);
      __vtbl = 0x10027058;
    }
    actor_condition = u32(actor_condition) | fopAcCnd_INIT_e;
  }
  if (sw == 255)
    return cPhs_ERROR_e;
  mType = prm_get(4, 0);
  correctRemoveDrawTag(&draw_tag);
  s8 room = home.roomNo;
  CorrectSaveData* save = sSaveData;
  if (correctCheckSwitch(&save->mSwitch, sw, room))
    mode_on_init();
  else
    mode_off_init();
  const char* name = L_attr.mEntries[(s32)mType].name;
  if (name) {
    u8 event = prm_get(8, 16);
    CorrectGameInfo* p = correctGetGameInfo();
    mEventIdx = correctGetEventIndex(&p->mEvtManager, name, event);
  } else
    mEventIdx = -1;
  demo_non_init();
  return cPhs_COMPLEATE_e;
}
/* 02332F6C */
u8 daObjCorrect::Act_c::_execute() {
  (this->*mode_proc.mEntries[u32(s32(mMode))])();
  (this->*demo_proc.mEntries[u32(s32(mDemo))])();
  return 1;
}
/* 0233305C */
static fopAc_ac_c *chk_try_actor0(fopAc_ac_c *a) {
  if (correctIsActor(a) && a &&
      a->mProcName == daObjCorrect::fpcNm_Obj_Try_e)
    return a;
  return nullptr;
}
/* 023330AC */
fopAc_ac_c * daObjCorrect::Act_c::chk_try_actor1(fopAc_ac_c* other, s32 type, f32 radius) {
  if (tryParameter(other, 4, 0) != type)
    return nullptr;
  f32 dist = correctSquareDistance(&other->current.pos, &current.pos);
  if (!(dist < radius))
    return nullptr;
  s32 sw = prm_get(8, 8);
  if (sw != tryParameter(other, 8, 8))
    return nullptr;
  daObjTry_c* dst = static_cast<daObjTry_c*>(other);
  f32 x = current.pos.x;
  s16 angle = shape_angle.y;
  dst->correctPosition.x=x;
  f32 y = current.pos.y;
  dst->correctPosition.y=y;
  f32 z = current.pos.z;
  dst->correctImmediate=1;
  dst->correctPosition.z=z;
  dst->correctYaw=angle;
  dst->correctEnabled=1;
  return other;
}
/* 023331BC */
fopAc_ac_c * daObjCorrect::Act_c::chk_try_actor2(fopAc_ac_c* other, s32 type, u8 enabled, f32 radius, f32 depress) {
  if (!(radius <= depress))
    correctAssert("d_a_obj_correct.cpp", 346, "i_sw_r_sq <= i_depression_r_sq");
  if (tryParameter(other, 4, 0) != type)
    return nullptr;
  f32 dist = correctSquareDistance(&other->current.pos, &current.pos);
  if (!(dist < depress))
    return nullptr;
  bool match = false;
  if (dist < radius) {
    s32 sw = prm_get(8, 8);
    match = sw == tryParameter(other, 8, 8);
  }
  daObjTry_c* dst = static_cast<daObjTry_c*>(other);
  f32 x = current.pos.x;
  s16 angle = shape_angle.y;
  dst->correctPosition.x=x;
  f32 y = current.pos.y;
  dst->correctPosition.y=y;
  f32 z = current.pos.z;
  dst->correctYaw=angle;
  dst->correctPosition.z=z;
  dst->correctEnabled=match;
  dst->correctImmediate=enabled;
  return match ? other : nullptr;
}
/* 02333324 */
static void *search_movebox(fopAc_ac_c *actor, Act_c *a) {
  u32 id = actor ? actor->mBsPcId : 0xFFFFFFFF;
  if (!correctIsCreating(id)) {
    if (correctIsActor(actor) && actor &&
        actor->mProcName == daObjCorrect::fpcNm_Obj_Movebox_e) {
      f32 dist =
          correctSquareDistance(&actor->current.pos, &a->current.pos);
      f32 radius = a->mRadiusSq;
      if (dist < radius) {
        s32 sw = boxParameter(actor, 8, 8);
        if (sw == a->prm_get(8, 8) && boxParameter(actor, 1, 30) == 0) {
          a->mSearchValid = 1;
          return actor;
        }
      }
      a->mSearchValid = 0;
    }
  } else if (actor && actor->mProcName == daObjCorrect::fpcNm_Obj_Movebox_e)
    a->mSearchValid = 0;
  return nullptr;
}
/* 02333458 */
static void *search_tryColSun(fopAc_ac_c *actor, Act_c *a) {
  fopAc_ac_c *res = chk_try_actor0(actor);
  if (res)
    return a->chk_try_actor1(res, 2, a->mRadiusSq);
  return nullptr;
}
/* 023334B4 */
static void *search_tryColMer(fopAc_ac_c *actor, Act_c *a) {
  fopAc_ac_c *res = chk_try_actor0(actor);
  if (res)
    return a->chk_try_actor1(res, 3, a->mRadiusSq);
  return nullptr;
}
/* 02333510 */
static void *search_tryColJup(fopAc_ac_c *actor, Act_c *a) {
  fopAc_ac_c *res = chk_try_actor0(actor);
  if (res)
    return a->chk_try_actor1(res, 4, a->mRadiusSq);
  return nullptr;
}
static void *keySearch(fopAc_ac_c *actor, Act_c *a, s32 type) {
  u32 id = actor ? actor->mBsPcId : 0xFFFFFFFF;
  if (correctIsCreating(id)) {
    if (actor && actor->mProcName == daObjCorrect::fpcNm_Obj_Try_e)
      a->mSearchValid = 0;
    return nullptr;
  }
  fopAc_ac_c *res = chk_try_actor0(actor);
  if (!res)
    return nullptr;
  f32 radius = a->mRadiusSq, depress = L_attr.mEntries[(s32)a->mType].mDepressionRadiusSq;
  return a->chk_try_actor2(res, type, 1, radius, depress);
}
/* 0233356C */
static void *search_tryKeyGate(fopAc_ac_c *actor, Act_c *a) {
  return keySearch(actor, a, 5);
}
/* 02333624 */
static void *search_tryKeyDoor(fopAc_ac_c *actor, Act_c *a) {
  return keySearch(actor, a, 6);
}
/* 023336DC */
static void *search_tryColGreen(fopAc_ac_c *actor, Act_c *a) {
  fopAc_ac_c *res = chk_try_actor0(actor);
  if (!res)
    return nullptr;
  void *result = nullptr;
  for (int i = 0; i < 6; i++) {
    s32 type = ::type.mEntries[i];
    f32 radius = a->mRadiusSq, depress = L_attr.mEntries[(s32)a->mType].mDepressionRadiusSq;
    if (a->chk_try_actor2(res, type, 1, radius, depress))
      result = res;
  }
  return result;
}
/* 02333788 */
static void *search_tryColBlue(fopAc_ac_c *actor, Act_c *a) {
  return search_tryColGreen(actor, a);
}
static void switchSet(Act_c *a, bool on) {
  s32 sw = a->prm_get(8, 8);
  CorrectSaveData* save = sSaveData;
  s8 room = a->home.roomNo;
  if (on) correctOnSwitch(&save->mSwitch, sw, room);
  else correctOffSwitch(&save->mSwitch, sw, room);
}
/* 0233378C */
void daObjCorrect::Act_c::mode_off() {
  s32 type = mType;
  mSearchValid = 1;
  u32 callback = M_search_proc.mEntries[u32(type)].get();
  void *res = correctSearch(callback, this);
  s16 valid = mSearchValid;
  if (valid == 1 && res) {
    switchSet(this, true);
    mode_on_init();
  }
}
/* 02333820 */
void daObjCorrect::Act_c::mode_on() {
  s32 type = mType;
  mSearchValid = 1;
  u32 callback = M_search_proc.mEntries[u32(type)].get();
  void *res = correctSearch(callback, this);
  s16 valid = mSearchValid;
  if (valid == 1 && !res) {
    switchSet(this, false);
    mode_off_init();
  }
}
/* 023338B4 */
void daObjCorrect::Act_c::demo_req_init() {
  s16 id = mEventIdx;
  CorrectGameInfo* p = correctGetGameInfo();
  if (correctEventExists(&p->mEvtManager, id)) {
    u8 event = prm_get(8, 16);
    id = mEventIdx;
    correctOrderEvent(this, id, event, 0xFFFF, 0, 1);
    u16 flags = eventInfo.mCondition;
    mDemo = DEMO_REQ;
    eventInfo.mCondition=flags | 2;
  } else
    demo_non_init();
}
/* 02333968 */
void daObjCorrect::Act_c::demo_non() {
  s32 type = mType;
  if (type != daObjCorrect::UNK_4 && type != daObjCorrect::UNK_5)
    return;
  CorrectGameInfo* p = correctGetGameInfo();
  fopAc_ac_c *player = p->mpPlayer.get();
  u32 flags = static_cast<daPy_py_c*>(player)->mResetFlg0;
  if (!(flags & daPy_py_c::daPyRFlg0_GRAB_PUT_START))
    return;
  // HD: the grab actor ID is read through the player virtual slot.
  CorrectPlayerVtable* vt = gabi::at<CorrectPlayerVtable>(player->__vtbl);
  s32 id = gabi::call_ptr<s32>(vt->mGetGrabActorID.get(), player);
  fopAc_ac_c *actor = nullptr;
  if (id != -1) {
    actor = fopAcM_SearchByID((u32)id);
  }
  if (!actor)
    return;
  actor = chk_try_actor0(actor);
  if (!actor)
    return;
  type = tryParameter(actor, 4, 0);
  if (!((type == 5 && s32(mType) == daObjCorrect::UNK_4) || (type == 6 && s32(mType) == daObjCorrect::UNK_5)))
    return;
  gabi::Local<cXyz> playerXZ, correctXZ;
  f32 x = current.pos.x;
  f32 px = player->current.pos.x, pz = player->current.pos.z;
  correctXZ->x = x;
  correctXZ->y = 0;
  playerXZ->z = pz;
  f32 z = current.pos.z;
  playerXZ->x = px;
  correctXZ->z = z;
  playerXZ->y = 0;
  f32 dist = correctSquareDistance(playerXZ.get(), correctXZ.get());
  f32 py = player->current.pos.y, y = current.pos.y;
  f32 dy = std::fabs(py - y);
  s16 angle = correctSearchAngleY(player, this);
  s16 facing = player->shape_angle.y;
  angle = s16(s32(facing) - angle);
  if (((dist < 40000.f && angle >= -0x4000 && angle <= 0x4000) ||
       dist < 1600.f) &&
      dy < 60.f)
    demo_req_init();
}
/* 02333B14 */
void daObjCorrect::Act_c::demo_run_init() {
  mDemo = DEMO_RUN;
}
/* 02333B20 */
void daObjCorrect::Act_c::demo_req() {
  if (eventInfo.mCommand == dEvtCmd_INDEMO_e)
    demo_run_init();
  else
    demo_non_init();
}
/* 02333B34 */
void daObjCorrect::Act_c::demo_run() {
  s16 id = mEventIdx;
  CorrectGameInfo* p = correctGetGameInfo();
  if (correctEventEndCheck(&p->mEvtManager, id)) {
    p = correctGetGameInfo();
    u16 flags = p->mEventFlag;
    p->mEventFlag=flags | 8;
    demo_non_init();
  }
}
/* 02333B98 */
static s32 wrapperCreate(Act_c *a) {
  return a->_create();
}
/* 02333B9C */
static s32 wrapperDelete(Act_c *a) {
  return 1;
}
/* 02333BA4 */
static u8 wrapperExecute(Act_c *a) {
  return a->_execute();
}
/* 02333BA8 */
static s32 wrapperDraw(Act_c *a) {
  return 1;
}
/* 02333BB0 */
static void sinit() {
  for (int i=0;i<4;i++) sHeaderStatics.mZero[i]=0;
  correctRegisterGlobal(&sDestructorRecords.mRecords[0]);
  sHeaderStatics.mMinAngle=-3.1415927f;
  sHeaderStatics.mMaxAngle=3.1415927f;
  correctInitializeHeaderByte(&sHeaderStatics.mHeaderByte);
  correctRegisterGlobal(&sDestructorRecords.mRecords[1]);
  correctInitializeHeaderFlag(&sHeaderStatics.mHeaderFlag);
  correctRegisterGlobal(&sDestructorRecords.mRecords[2]);
}
/* 02333C44 */
static void actorDelete(Act_c *a, s32 flags) {
  if (a) {
    correctDestroyActor(a, 0);
    if (flags & 1)
      correctDeleteObject(a);
  }
}
/* 02333C98 */
static s32 wrapperIsDelete(Act_c *a) {
  return 1;
}

/* WWHD glue
ext 025B9E38 void correctOnSwitch(void*, s32, s8)
ext 025B9F7C void correctOffSwitch(void*, s32, s8)
ext 028F026C void correctRegisterGlobal(void*)
ext 028ED6F8 void correctInitializeHeaderByte(void*)
ext 028EAB2C void correctInitializeHeaderFlag(void*)

data L_attr 10027080
data M_search_proc 10027190
data mode_proc 10027100
data demo_proc 10027110
data type 1002715C
data sSaveData 101F84DC
data sHeaderStatics 104695DC
data sDestructorRecords 101C8A4C
ext 025D4ED0 void correctConstructActor(Act_c*)
ext 025DA884 void correctRemoveDrawTag(void*)
ext 025BA0C0 s32 correctCheckSwitch(void*, s32, s8)
ext 02543F10 s32 correctGetEventIndex(void*, const char*, u8)
ext 025D4604 s32 correctIsActor(fopAc_ac_c*)
ext 028E8DE8 f32 correctSquareDistance(const cXyz*, const cXyz*)
ext 0273AA24 void correctAssert(const char*, s32, const char*)
ext 025DD868 s32 correctIsCreating(u32)
ext 025D5218 void* correctSearch(u32, void*)
ext 02544044 u32 correctEventExists(void*, s16)
ext 025D7A58 void correctOrderEvent(Act_c*, s16, u8, u32, u32, u32)
ext 025D6894 s16 correctSearchAngleY(fopAc_ac_c*, Act_c*)
ext 025440C8 s32 correctEventEndCheck(void*, s16)
ext 025D50BC void correctDestroyActor(Act_c*, s32)
ext 0273AF40 void correctDeleteObject(void*)
ext 025200D4 CorrectGameInfo* correctGetGameInfo()
str 10027128
str 1002713C
*/

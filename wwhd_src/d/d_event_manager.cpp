// Local WWHD event-manager reconstruction.
// HD: manager caches the current event and debug mode uses SafeString callbacks.
#include "d/d_event_manager.h"
using namespace gabi;
namespace event_manager {
/* 02542D74 */
u32 dEv_extra_createCB(u32 actor) {

  view<fopAc_ac_c>(actor)->actor_status =
      (u32)view<fopAc_ac_c>(actor)->actor_status | fopAcStts_UNK800_e;
  return 4;
}
// HD: the manager retains the current event name for debug action lookup.
/* 02542E94 */
u32 event_manager::dEvent_manager_c::getCurrentEventNameHD() {

  u32 p = (u32)this->mCurrentEvent;
  return p ? p : EmptyCurrentEvent;
}
/* 02542EAC */
u32 event_manager::dEvent_manager_c::getCurrentCutHD(s32 staff) {

  if (staff == -1)
    return 0;
  u32 entry =
      (u32)this->mList.mStaffP.v + u32(staff) * u32(sizeof(dEvDtStaff_c));
  return (u32)this->mList.mCutP.v +
         (u32)view<dEvDtStaff_c>(entry)->mCurCutIdx * u32(sizeof(dEvDtStaff_c));
}
// HD: the inlined constructor also initializes state and the extra word.
/* 02543378 */
u32 constructExceptionHD(u32 self) {

  if (!self)
    self = sdk::allocate<u32>(12u);
  if (self) {
    view<dEvent_exception_c>(self)->mState = 0;
    view<dEvent_exception_c>(self)->mEventInfoIdx = 0xffffffff;
    view<dEvent_exception_c>(self)->field_0x4 = 0;
  }
  return self;
}
/* 0254351C */
void event_manager::dEvent_exception_c::init() {

  this->mEventInfoIdx = 0xffffffff;
  this->mState = 0;
  this->field_0x4 = 0xffffffff;
}
/* 02543638 */
u32 constructSearchParamHD(u32 self, u32 name, u32 actor, u32 flags) {

  if (!self)
    self = sdk::allocate<u32>(16u);
  if (self) {
    view<dEv_seach_prm>(self)->mName = name;
    view<dEv_seach_prm>(self)->mValue = flags;
    view<dEv_seach_prm>(self)->mCastInFlag = 0;
    view<dEv_seach_prm>(self)->mMask = actor;
  }
  return self;
}
// HD: allocation bridge retains the native nullable constructor result.
/* 025436A4 */
u32 constructManagerHD(u32 self) {

  if (!self)
    self = sdk::allocate<u32>(0x540u);
  if (self) {
    sdk::initializeEventList<void>(self);
    view<dEvent_manager_c>(self)->mCameraPlay = 0;
    sdk::constructException<void>(
        ea(&view<dEvent_manager_c>(self)->mException));
    view<dEvent_manager_c>(self)->mCurrentEvent = 0;
  }
  return self;
}
/* 02543708 */
void event_manager::dEvent_manager_c::closeProc(u32 event) {

  view<dEvDtEvent_c>(event)->mEventState = CLOSE_e;
}
/* 02543714 */
void event_manager::dEvent_manager_c::setGoal(u32 vec) {

  this->mGoal.x = (f32)view<cXyz>(vec)->x;
  this->mGoal.y = (f32)view<cXyz>(vec)->y;
  this->mGoal.z = (f32)view<cXyz>(vec)->z;
}
/* 02544044 */
u32 event_manager::dEvent_manager_c::getEventData(s32 index) {

  u32 header = (u32)this->mList.mHeaderP.v;
  if (!header || index < 0 ||
      index >= (s32)view<event_binary_data_header>(header)->eventNum)
    return 0;
  return (u32)this->mList.mEventP.v + u32(index) * u32(sizeof(dEvDtEvent_c));
}
/* 02544534 */
u32 event_manager::dEvent_manager_c::order(s32 index) {
  u32 self = ea(this);

  u32 data = sdk::getEventData<u32>(self, index);
  if (!data)
    return 0;
  view<dEvDtEvent_c>(data)->mEventState = ORDER_e;
  return 1;
}
/* 0254457C */
u32 event_manager::dEvent_manager_c::endCheckOld(u32 name) {
  u32 self = ea(this);

  s32 id = sdk::getEventIdx<s32>(self, name, 255u);
  return sdk::endCheck<u32>(self, id);
}
/* 025445B8 */
u32 event_manager::dEvent_manager_c::startCheckOld(u32 name) {
  u32 self = ea(this);

  s32 id = sdk::getEventIdx<s32>(self, name, 255u);
  return sdk::startCheck<u32>(self, id);
}
/* 025445F4 */
u32 event_manager::dEvent_manager_c::getEventPrio(s32 index) {
  u32 self = ea(this);

  u32 event = sdk::getEventData<u32>(self, index);
  return event ? (u32)view<dEvDtEvent_c>(event)->mPriority : 0;
}
/* 02544628 */
u32 event_manager::dEvent_manager_c::getEventEndSound(s32 index) {
  u32 self = ea(this);

  u32 event = sdk::getEventData<u32>(self, index);
  return event ? (u8)view<dEvDtEvent_c>(event)->mEventEndSound : 0;
}
/* 025447C8 */
u32 event_manager::dEvent_manager_c::getIsAddvance(s32 staff) {

  return staff == -1
             ? 0
             : (u8)view<dEvDtStaff_c>((u32)this->mList.mStaffP.v +
                                      u32(staff) * u32(sizeof(dEvDtStaff_c)))
                   ->mAdvance;
}
/* 025447EC */
u32 event_manager::dEvent_manager_c::getMyActName(s32 staff) {
  u32 self = ea(this);

  if (staff == -1)
    return 0;
  return sdk::getCurrentCut<u32>(self, staff);
}
/* 02544830 */
u32 event_manager::dEvent_manager_c::getMyNowCutName(s32 staff) {
  u32 self = ea(this);

  u32 cut = sdk::getCurrentCut<u32>(self, staff);
  if (!cut)
    sdk::assertion<void>(CutAssertionFile, 0x55Du, CutAssertionMessage);
  return cut;
}
/* 02544900 */
u32 event_manager::dEvent_manager_c::getGoal() { return ea(&this->mGoal); }
/* 02544944 */
u32 event_manager::dEvent_manager_c::cancelStaff(u32 data) {

  return sdk::actorJudge<u32>(extraOffObjectCallBack, data);
}
/* 02544950 */
u32 event_manager::dEvent_manager_c::ChkPresentEnd() {

  return sdk::talkmanAction<s32>(ACT_WAIT) > 0;
}
/* 02544980 */
u32 event_manager::dEvent_manager_c::CancelPresent() {

  return sdk::talkmanAction<u32>(ACT_TALK0) == ACT_TALK0;
}
/* 025449B0 */
u32 event_manager::dEvent_manager_c::checkStartDemo() {

  u32 play = sdk::getGameInfo<u32>();
  return (u8)view<GameInfo>(play)->mEvent.mMode != 0 &&
         (u32)this->mException.mEventInfoIdx != 0xffffffff;
}
// HD: compiler-generated global initializers and deleting destructor.
/* 02544A10 */
void eventManagerStaticInitializer() {

  initializers()->words[2] = 0;
  initializers()->words[0] = 0;
  initializers()->words[3] = 0;
  initializers()->words[1] = 0;
  sdk::constructVector<void>(InitializerVector0);
  f32 first = (f32)constant(InitialFloat0),
      second = (f32)constant(InitialFloat1);
  initializers()->first = first;
  initializers()->second = second;
  sdk::initializeByte<void>(InitializerByte);
  sdk::constructVector<void>(InitializerVector1);
  sdk::initializeFlag<void>(InitializerFlag);
  sdk::constructVector<void>(InitializerVector2);
}
/* 02544AA4 */
void destroyManagerHD(u32 self, u32 flags) {
  if (self && (flags & 1))
    sdk::deallocate<void>(self);
}
/* 02544AB8 */
void event_manager::dEvent_manager_c::debugBfProc() {}

static bool string_equal(u32 a, u32 b) {
  for (;; ++a, ++b) {
    u8 x = (u8)*view<be<u8>>(a), y = (u8)*view<be<u8>>(b);
    if (x != y)
      return false;
    if (!x)
      return true;
  }
}
/* 02542A38 */
u32 findObjectCallBack(u32 actor, u32 param) {

  if (!param)
    return 0;
  u32 profile =
      sdk::searchObjectName<u32>((u32)view<dEv_seach_prm>(param)->mName);
  if (!profile)
    return 0;
  s16 actual = (s16)view<EventObjectName>(profile)->procname,
      expected = actor ? (s16)view<fopAc_ac_c>(actor)->mProfName : 0x7fff;
  if (actual != expected)
    return 0;
  if ((s8)view<fopAc_ac_c>(actor)->argument !=
      (s8)view<EventObjectName>(profile)->argument)
    return 0;
  u32 mask = (u32)view<dEv_seach_prm>(param)->mMask;
  if (mask && ((u32)view<fopAc_ac_c>(actor)->mParameters & mask) !=
                  (u32)view<dEv_seach_prm>(param)->mValue)
    return 0;
  return actor;
}
/* 02542AF4 */
u32 extraOnObjectCallBack(u32 actor, u32 param) {

  if (!param)
    return 0;
  u32 name = (u32)view<dEv_seach_prm>(param)->mName;
  if (!string_equal(name, AllActorName)) {
    u32 profile = sdk::searchObjectName<u32>(name);
    if (!profile)
      return 0;
    if ((s16)view<EventObjectName>(profile)->procname !=
        (actor ? (s16)view<fopAc_ac_c>(actor)->mProfName : 0x7fff))
      return 0;
    if ((s8)view<fopAc_ac_c>(actor)->argument !=
        (s8)view<EventObjectName>(profile)->argument)
      return 0;
    u32 mask = (u32)view<dEv_seach_prm>(param)->mMask;
    if (mask && ((u32)view<fopAc_ac_c>(actor)->mParameters & mask) !=
                    (u32)view<dEv_seach_prm>(param)->mValue)
      return 0;
  }
  u32 flags = (u32)view<fopAc_ac_c>(actor)->actor_status | fopAcStts_UNK800_e;
  view<fopAc_ac_c>(actor)->actor_status = flags;
  if ((u16)view<dEv_seach_prm>(param)->mCastInFlag & 1)
    view<fopAc_ac_c>(actor)->actor_status = flags | fopAcStts_FORCEMOVE_e;
  return 0;
}
/* 02542BE0 */
u32 extraOffObjectCallBack(u32 actor, u32 name) {

  if (string_equal(name, AllReleaseName)) {
    view<fopAc_ac_c>(actor)->actor_status =
        (u32)view<fopAc_ac_c>(actor)->actor_status &
        ~u32(fopAcStts_UNK800_e | fopAcStts_FORCEMOVE_e);
    return 0;
  }
  u32 profile = sdk::searchObjectName<u32>(name);
  if (!profile)
    return 0;
  if ((s16)view<EventObjectName>(profile)->procname !=
      (actor ? (s16)view<fopAc_ac_c>(actor)->mProfName : 0x7fff))
    return 0;
  if ((s8)view<fopAc_ac_c>(actor)->argument !=
      (s8)view<EventObjectName>(profile)->argument)
    return 0;
  view<fopAc_ac_c>(actor)->actor_status =
      (u32)view<fopAc_ac_c>(actor)->actor_status & ~u32(fopAcStts_UNK800_e);
  return 0;
}
/* 02542C9C */
u32 allOffObjectCallBack(u32 actor) {
  view<fopAc_ac_c>(actor)->actor_status =
      (u32)view<fopAc_ac_c>(actor)->actor_status &
      ~u32(fopAcStts_UNK800_e | fopAcStts_UNK1000_e | fopAcStts_FORCEMOVE_e);
  return 0;
}
/* 02542CB8 */
u32 findShutterCallBack(u32 actor, u32 param) {

  if ((s16)view<find_shutter_param>(param)->profName !=
      (actor ? (s16)view<fopAc_ac_c>(actor)->mProfName : 0x7fff))
    return 0;
  Local<cXyz> delta;
  sdk::subtractVector<void>(
      ea(&view<fopAc_ac_c>(actor)->home.pos), delta.get(),
      ea(&view<fopAc_ac_c>(view<find_shutter_param>(param)->actor)->home.pos));
  f32 x = (f32)delta->x, y = (f32)delta->y, z = (f32)delta->z,
      hi = (f32)constant(ShutterUpperBound);
  if (!(x < hi))
    return 0;
  f32 lo = (f32)constant(ShutterLowerBound);
  if (!(x > lo) || !(y < hi) || !(y > lo) || !(z < hi) || !(z > lo))
    return 0;
  return actor;
}

/* 0254407C */
u32 event_manager::dEvent_manager_c::startCheck(s32 id) {
  u32 self = ea(this);
  u32 e = sdk::getEventData<u32>(self, id);
  return e && (u32)view<dEvDtEvent_c>(e)->mEventState == PLAY_e;
}
/* 025440C8 */
u32 event_manager::dEvent_manager_c::endCheck(s32 id) {
  u32 self = ea(this);
  u32 e = sdk::getEventData<u32>(self, id);
  return e && (u32)view<dEvDtEvent_c>(e)->mEventState == CLOSE_e;
}
/* 02544374 */
void event_manager::dEvent_manager_c::runProc() {
  u32 self = ea(this);
  sdk::exceptionProc<void>(self);
  u32 play = sdk::getGameInfo<u32>();
  sdk::checkEvent<void>(ea(&view<GameInfo>(play)->mEvent));
  sdk::mainProc<void>(self);
}
/* 025443B4 */
void event_manager::dEvent_manager_c::setData(u32 data) {

  if (!data)
    return;
  this->mList.mHeaderP.v = data;
  u32 header = data;
  for (u32 i = 0; i < 7; ++i) {
    if ((s32)view<ResourceChunks>(header)->chunks[i].count > 0) {
      u32 off = (u32)view<ResourceChunks>(header)->chunks[i].offset;
      if (i < 6)
        header = (u32)this->mList.mHeaderP.v;
      this->mListWords[i + 1] = data + off;
    }
  }
}
/* 02544488 */
u32 event_manager::dEvent_manager_c::create() {
  u32 self = ea(this);
  sdk::initializeEventList<void>(self);
  this->mCameraPlay = 0;
  sdk::initializeException<void>(ea(&this->mException));
  sdk::initializeFlags<void>(ea(&this->mFlag));
  u32 data = sdk::getStageResource<u32>(StageArchiveName, EventResourceName);
  sdk::setData<void>(self, data);
  return 1;
}
/* 025444F0 */
void event_manager::dEvent_manager_c::remove() {
  u32 self = ea(this);
  sdk::initializeEventList<void>(self);
  this->mCameraPlay = 0;
  sdk::initializeException<void>(ea(&this->mException));
  sdk::initializeFlags<void>(ea(&this->mFlag));
}
/* 0254487C */
u32 event_manager::dEvent_manager_c::getMySubstanceP(s32 staff, u32 name,
                                                     s32 type) {
  u32 self = ea(this);
  u32 data = sdk::getMyData<u32>(self, staff, name, 0u);
  return data ? sdk::getSubstance<u32>(self, data, type) : 0;
}
/* 025448CC */
u32 event_manager::dEvent_manager_c::getMySubstanceNum(s32 staff, u32 name) {
  u32 self = ea(this);
  u32 data = sdk::getMyData<u32>(self, staff, name, 0u);
  return data ? (u32)view<dEvDtData_c>(data)->mSubstanceSize : 0;
}

/* 02543280 */
void event_manager::dEvent_manager_c::cutEnd(s32 staff) {

  if (staff == -1)
    return;
  u32 entry =
      (u32)this->mList.mStaffP.v + u32(staff) * u32(sizeof(dEvDtStaff_c));
  u32 cut =
      (u32)this->mList.mCutP.v +
      (u32)view<dEvDtStaff_c>(entry)->mCurCutIdx * u32(sizeof(dEvDtStaff_c));
  if (cut)
    sdk::setFlag<void>(ea(&this->mFlag), (u32)view<dEvDtCut_c>(cut)->mFlagIdx);
}
/* 02543944 */
u32 event_manager::dEvent_manager_c::getMyDataP(s32 staff, u32 name,
                                                u32 initial) {

  if (staff == -1)
    return 0;
  u32 staff_base = (u32)this->mList.mStaffP.v,
      cut_base = (u32)this->mList.mCutP.v,
      entry = staff_base + u32(staff) * u32(sizeof(dEvDtStaff_c));
  u32 cut = cut_base + (initial ? (u32)view<dEvDtStaff_c>(entry)->mFirstCutIdx
                                : (u32)view<dEvDtStaff_c>(entry)->mCurCutIdx) *
                           u32(sizeof(dEvDtStaff_c)),
      index = (u32)view<dEvDtCut_c>(cut)->mFirstDataIdx;
  if (index == 0xffffffff)
    return 0;
  u32 data_base = (u32)this->mList.mDataP.v;
  for (;;) {
    u32 data = data_base + index * u32(sizeof(dEvDtData_c));
    if (string_equal(name, data))
      return data;
    index = (u32)view<dEvDtData_c>(data)->mNextIdx;
    if (index == 0xffffffff)
      return 0;
  }
}
/* 025439E0 */
u32 event_manager::dEvent_manager_c::getSubstance(u32 data, s32 requested) {

  if ((s32)view<dEvDtData_c>(data)->mSubstanceIdx < 0 ||
      (s32)view<dEvDtData_c>(data)->mSubstanceSize <= 0) {
    sdk::assertion<void>(SubstanceAssertionFile, 0x18Cu,
                         SubstanceAssertionMessage);
    return 0;
  }
  u32 type;
  if (requested != -1) {
    type = (u32)view<dEvDtData_c>(data)->mSubstanceType;
    if (type != u32(requested)) {
      sdk::assertion<void>(SubstanceAssertionFile, 0x191u,
                           SubstanceAssertionMessage);
      type = (u32)view<dEvDtData_c>(data)->mSubstanceType;
    }
  } else
    type = (u32)view<dEvDtData_c>(data)->mSubstanceType;
  if (type < 3) {
    u32 base = (u32)this->mList.mFDataP.v;
    return base + ((u32)view<dEvDtData_c>(data)->mSubstanceIdx << 2);
  }
  if (type == 3) {
    u32 base = (u32)this->mList.mIDataP.v;
    return base + ((u32)view<dEvDtData_c>(data)->mSubstanceIdx << 2);
  }
  if (type == 4) {
    u32 base = (u32)this->mList.mSDataP.v;
    return base + (u32)view<dEvDtData_c>(data)->mSubstanceIdx;
  }
  sdk::assertion<void>(SubstanceAssertionFile, 0x1A5u,
                       SubstanceAssertionMessage);
  return 0;
}
/* 02543838 */
u32 event_manager::dEvent_manager_c::specialCast(u32 name, u32 move) {
  u32 self = ea(this);
  u32 actor = 0;
  if (!name || !string_equal(name, ShutterDoorName))
    return 0;
  for (u32 type : {u32(fpcNm_DOOR10_e), u32(fpcNm_DOOR12_e),
                   u32(fpcNm_KDDOOR_e), u32(fpcNm_KNOB00_e)}) {
    actor = sdk::castShutter<u32>(self, type, move);
    if (actor)
      break;
  }
  if (actor) {
    u32 play = sdk::getGameInfo<u32>();
    view<GameInfo>(play)->mEvent.mEventFlag =
        (u16)view<GameInfo>(play)->mEvent.mEventFlag | 0x10;
    u32 flags = (u32)view<fopAc_ac_c>(actor)->actor_status;
    view<fopAc_ac_c>(actor)->actor_status =
        move ? flags | fopAcStts_UNK1000_e : flags & ~u32(fopAcStts_UNK1000_e);
  }
  return actor;
}
/* 02544908 */
u32 event_manager::dEvent_manager_c::issueStaff(u32 name) {

  Local<dEv_seach_prm> param;
  sdk::constructSearchParam<void>(param.get(), name, 0u, 0u);
  return sdk::actorJudge<u32>(extraOnObjectCallBack, param.get());
}

/* 02543B38 */
void event_manager::dEvent_manager_c::setPrmStaff(u32 param, s32 staff) {
  u32 self = ea(this);

  u32 entry =
      (u32)this->mList.mStaffP.v + u32(staff) * u32(sizeof(dEvDtStaff_c));
  if (!entry)
    sdk::assertion<void>(StaffAssertionFile, 0x72Du, StaffAssertionMessage);
  if (!param)
    sdk::assertion<void>(StaffAssertionFile, 0x72Eu,
                         SearchParamAssertionMessage);
  view<dEv_seach_prm>(param)->mName = entry;
  u32 data = sdk::getMyData<u32>(self, staff, DevParamMaskName, 1u);
  if (!data) {
    view<dEv_seach_prm>(param)->mMask = 0;
    view<dEv_seach_prm>(param)->mValue = 0;
    view<dEv_seach_prm>(param)->mCastInFlag = 0;
  } else {
    u32 values = sdk::getSubstance<u32>(self, data, 3u);
    if (!values)
      sdk::assertion<void>(StaffAssertionFile, 0x73Au,
                           IntegerDataAssertionMessage);
    view<dEv_seach_prm>(param)->mMask = (u32)view<be<u32>>(values)[0];
    u32 second = (u32)view<be<u32>>(values)[1];
    view<dEv_seach_prm>(param)->mCastInFlag = 0;
    view<dEv_seach_prm>(param)->mValue = second;
  }
  if (sdk::getMyData<u32>(self, staff, CastInName, 1u))
    view<dEv_seach_prm>(param)->mCastInFlag = 1;
}
/* 02543C6C */
void event_manager::dEvent_manager_c::startProc(u32 event) {
  u32 self = ea(this);
  Local<dEv_seach_prm> param;
  sdk::constructSearchParam<void>(param.get(), 0u, 0u, 0u);
  for (u32 i = 0; s32(i) < (s32)view<dEvDtEvent_c>(event)->mNStaff; ++i) {
    u32 staff = (u32)view<dEvDtEvent_c>(event)->mStaffIdx[i],
        entry = (u32)this->mList.mStaffP.v + staff * u32(sizeof(dEvDtStaff_c)),
        kind = (u32)view<dEvDtStaff_c>(entry)->mStaffType;
    if (kind == 0) {
      u32 actor = sdk::specialCast<u32>(self, entry, 1u);
      if (!actor) {
        sdk::setParamStaff<void>(self, param.get(), staff);
        actor = sdk::actorJudge<u32>(findObjectCallBack, param.get());
      }
      if (actor)
        view<fopAc_ac_c>(actor)->actor_status =
            (u32)view<fopAc_ac_c>(actor)->actor_status | fopAcStts_FORCEMOVE_e;
      kind = (u32)view<dEvDtStaff_c>(entry)->mStaffType;
    }
    if (kind == 1) {
      sdk::setParamStaff<void>(self, param.get(), staff);
      sdk::actorJudge<void>(extraOnObjectCallBack, param.get());
    }
    sdk::initializeStaff<void>(entry);
  }
  view<dEvDtEvent_c>(event)->mEventState = PLAY_e;
  sdk::initializeFlags<void>(ea(&this->mFlag));
}
/* 02543D80 */
void event_manager::dEvent_manager_c::mainProc() {
  u32 self = ea(this);
  u32 header = (u32)this->mList.mHeaderP.v;
  if (!header)
    return;
  s32 count = (s32)view<event_binary_data_header>(header)->eventNum;
  for (u32 i = 0; s32(i) < count; ++i) {
    u32 event = (u32)this->mList.mEventP.v + i * u32(sizeof(dEvDtEvent_c));
    if ((u32)view<dEvDtEvent_c>(event)->mEventState == PLAY_e) {
      if (sdk::eventFinished<u32>(event))
        sdk::closeProc<void>(self, event);
      count =
          (s32)view<event_binary_data_header>(this->mList.mHeaderP.v)->eventNum;
    }
  }
  for (u32 i = 0; s32(i) < count; ++i) {
    u32 event = (u32)this->mList.mEventP.v + i * u32(sizeof(dEvDtEvent_c));
    if ((u32)view<dEvDtEvent_c>(event)->mEventState == ORDER_e) {
      sdk::startProc<void>(self, event);
      count =
          (s32)view<event_binary_data_header>(this->mList.mHeaderP.v)->eventNum;
    }
  }
  for (u32 i = 0; s32(i) < count; ++i) {
    u32 event = (u32)this->mList.mEventP.v + i * u32(sizeof(dEvDtEvent_c));
    if ((u32)view<dEvDtEvent_c>(event)->mEventState == PLAY_e) {
      sdk::advanceCut<void>(self, event);
      count =
          (s32)view<event_binary_data_header>(this->mList.mHeaderP.v)->eventNum;
    }
  }
  header = (u32)this->mList.mHeaderP.v;
  this->mCurrentEvent = 0;
  count = (s32)view<event_binary_data_header>(header)->eventNum;
  for (u32 i = 0; s32(i) < count; ++i) {
    u32 event = (u32)this->mList.mEventP.v + i * u32(sizeof(dEvDtEvent_c));
    if ((u32)view<dEvDtEvent_c>(event)->mEventState == PLAY_e) {
      sdk::specialStaffProc<void>(event, (u32)this->mList.mStaffP.v);
      header = (u32)this->mList.mHeaderP.v;
      this->mCurrentEvent = event;
      count = (s32)view<event_binary_data_header>(header)->eventNum;
    }
  }
}
/* 02543F10 */
s32 event_manager::dEvent_manager_c::getEventIdx(u32 name, s32 mapid) {
  u32 self = ea(this);

  u32 stage = ea(&view<GameInfo>(sdk::getGameInfo<u32>())->stage);
  u32 map = call_ptr<u32>(
      (u32)view<StageMethods>((u32)view<Stage>(stage)->vtable)->getEventInfo.v,
      stage);
  u32 header = (u32)this->mList.mHeaderP.v;
  if (!header)
    return -1;
  if (map && mapid != 255 && (s32)view<dStage_EventInfo_c>(map)->num > mapid)
    return sdk::getEventIdx<s32>(
        self,
        (u32)view<dStage_EventInfo_c>(map)->events +
            u32(mapid) * u32(sizeof(dStage_Event_dt_c)) + 1,
        255u);
  if (!name)
    return -1;
  s32 count = (s32)view<event_binary_data_header>(header)->eventNum;
  if (count <= 0)
    return -1;
  u32 base = (u32)this->mList.mEventP.v;
  for (u32 i = 0; s32(i) < count; ++i)
    if (string_equal(name, base + i * u32(sizeof(dEvDtEvent_c))))
      return s16(i);
  return -1;
}
/* 0254465C */
void event_manager::dEvent_manager_c::endProc(s32 id, u32 force) {
  u32 self = ea(this);
  u32 event = sdk::getEventData<u32>(self, id);
  if (!event) {
    sdk::assertion<void>(EndAssertionFile, 0x388u, EndAssertionMessage);
    return;
  }
  if (force)
    sdk::closeProc<void>(self, event);
  if ((u32)view<dEvDtEvent_c>(event)->mEventState == CLOSE_e) {
    sdk::actorJudge<void>(allOffObjectCallBack, AllEndName);
    this->mCameraPlay = 0;
    view<dEvDtEvent_c>(event)->mEventState = NONE_e;
  }
}
/* 02544708 */
u32 event_manager::dEvent_manager_c::getToolId(s32 id, s32 target) {

  u32 stage = ea(&view<GameInfo>(sdk::getGameInfo<u32>())->stage),
      map =
          call_ptr<u32>((u32)view<StageMethods>((u32)view<Stage>(stage)->vtable)
                            ->getEventInfo.v,
                        stage);
  if (id == 255 || !map || id >= (s32)view<dStage_EventInfo_c>(map)->num ||
      target < 0)
    return 255;
  u32 base = (u32)view<dStage_EventInfo_c>(map)->events,
      remaining = u32(target);
  for (u32 i = 0; i < u32(target) + 1; ++i) {
    u32 entry = base + u32(id) * u32(sizeof(dStage_Event_dt_c));
    u8 match = (u8)view<dStage_Event_dt_c>(entry)->field_0x12;
    if ((match == 255 && remaining == 0) ||
        (match != 255 && match == u32(target)))
      return u32(id);
    id = (u8)view<dStage_Event_dt_c>(entry)->field_0x0;
    if (id == 255)
      return 255;
    --remaining;
  }
  return 255;
}

/* 025432B4 */
s32 dEv_talkman_get_action(u32 action) {
  u32 play = sdk::getGameInfo<u32>();
  s32 staff = sdk::getMyStaffId<s32>(ea(&view<GameInfo>(play)->mEventManager),
                                     TalkmanName, 0u, 0u);
  if (staff == -1)
    return -1;
  play = sdk::getGameInfo<u32>();
  s32 act = sdk::getMyActIdx<s32>(ea(&view<GameInfo>(play)->mEventManager),
                                  staff, TalkmanActions, 3u, 0u, 0u);
  if (u32(act) == action) {
    play = sdk::getGameInfo<u32>();
    sdk::cutEnd<void>(ea(&view<GameInfo>(play)->mEventManager), staff);
  }
  return act;
}
/* 025433C0 */
u32 event_manager::dEvent_exception_c::getEventName() {

  u32 stage = ea(&view<GameInfo>(sdk::getGameInfo<u32>())->stage);
  u32 events = call_ptr<u32>(
      (u32)view<StageMethods>((u32)view<Stage>(stage)->vtable)->getEventInfo.v,
      stage);
  stage = ea(&view<GameInfo>(sdk::getGameInfo<u32>())->stage);
  u32 info = call_ptr<u32>(
      (u32)view<StageMethods>((u32)view<Stage>(stage)->vtable)->getStageInfo.v,
      stage);
  u32 index = (u32)this->mEventInfoIdx;
  if (index == 0xffffffff)
    return 0;
  if (index == 207) {
    if (!info)
      sdk::assertion<void>(ExceptionAssertionFile, 0xADu,
                           StageInfoAssertionMessage);
    if ((((u32)view<StageInfo>(info)->mFlags >> 16) & 7) == 3)
      return BossShutterStartName;
    index = (u32)this->mEventInfoIdx;
  }
  if (index - 201u < 13)
    return (u32)exceptionNames()[index];
  if (!events || (s32)view<dStage_EventInfo_c>(events)->num < s32(index))
    return 0;
  return (u32)view<dStage_EventInfo_c>(events)->events +
         index * u32(sizeof(dStage_Event_dt_c)) + 1;
}
/* 02543534 */
s32 event_manager::dEvent_exception_c::setStartDemo(s32 index) {

  u32 stage = ea(&view<GameInfo>(sdk::getGameInfo<u32>())->stage);
  u32 events = call_ptr<u32>(
      (u32)view<StageMethods>((u32)view<Stage>(stage)->vtable)->getEventInfo.v,
      stage);
  if (index == 255) {
    this->mEventInfoIdx = 206;
    return 255;
  }
  if (index < 200) {
    if (!events || index == -1 ||
        (s32)view<dStage_EventInfo_c>(events)->num < index)
      return 255;
    u32 flag = (u8)view<dStage_Event_dt_c>(
                   (u32)view<dStage_EventInfo_c>(events)->events +
                   u32(index) * u32(sizeof(dStage_Event_dt_c)))
                   ->mSpawnSwitchNo;
    if (flag != 255) {
      u32 save = saveDataAddress();
      s32 room = (s8)stayRoom();
      if (sdk::checkSwitch<u32>(save, flag, room)) {
        this->mEventInfoIdx = 206;
        return 255;
      }
      save = saveDataAddress();
      room = (s8)stayRoom();
      sdk::onSwitch<void>(save, flag, room);
    }
  }
  this->mEventInfoIdx = u32(index);
  return index;
}
/* 02543730 */
u32 event_manager::dEvent_manager_c::specialCast_Shutter(s32 profile,
                                                         u32 move) {

  Local<ShutterWork> storage;
  u32 param = ea(&storage->param);
  view<find_shutter_param>(param)->profName = u16(profile);
  u32 play = sdk::getGameInfo<u32>(),
      player = (u32)view<GameInfo>(play)->player;
  view<find_shutter_param>(param)->actor = player;
  if (!player)
    sdk::assertion<void>(ShutterAssertionFile, 0x6E5u, PlayerAssertionMessage);
  u32 actor = sdk::actorJudge<u32>(findShutterCallBack, param);
  if (actor && move) {
    storage->goal.x = (f32)view<fopAc_ac_c>(actor)->home.pos.x;
    storage->goal.y = (f32)view<fopAc_ac_c>(actor)->home.pos.y;
    player = (u32)view<find_shutter_param>(param)->actor;
    storage->goal.z = (f32)view<fopAc_ac_c>(actor)->home.pos.z;
    u32 angle = u16((s16)view<fopAc_ac_c>(player)->home.angle.y + 0x8000);
    u32 table = sineSampleAddress(angle);
    f32 scale = (f32)constant(ShutterGoalDistance),
        sin = (f32)view<SinCosSample>(table)->sine,
        cos = (f32)view<SinCosSample>(table)->cosine;
    storage->goal.x = fmadds(sin, scale, (f32)storage->goal.x);
    storage->goal.z = fmadds(cos, scale, (f32)storage->goal.z);
    play = sdk::getGameInfo<u32>();
    sdk::setGoal<void>(ea(&view<GameInfo>(play)->mEventManager), storage.get());
    u32 control = ea(&view<GameInfo>(sdk::getGameInfo<u32>())->mEvent);
    u32 id = sdk::getActorProcessId<u32>(control, actor);
    view<dEvt_control_c>(control)->mPt2 = id;
  }
  return actor;
}

/* 02542D88 */
s32 event_manager::dEvent_manager_c::getMyStaffId(u32 name, u32 actor, u32 id) {

  u32 play = sdk::getGameInfo<u32>();
  if (!(u8)view<GameInfo>(play)->mEvent.mMode)
    return -1;
  u32 header = (u32)this->mList.mHeaderP.v;
  if (!header)
    return -1;
  if (actor &&
      !((u32)view<fopAc_ac_c>(actor)->actor_status & fopAcStts_FORCEMOVE_e))
    return -1;
  s32 count = (s32)view<event_binary_data_header>(header)->eventNum;
  if (count <= 0)
    return -1;
  u32 events = (u32)this->mList.mEventP.v;
  for (u32 i = 0; s32(i) < count; ++i) {
    u32 event = events + i * u32(sizeof(dEvDtEvent_c));
    if ((u32)view<dEvDtEvent_c>(event)->mEventState - u32(PLAY_e) >
        u32(CLOSE_e - PLAY_e))
      continue;
    s32 slots = (s32)view<dEvDtEvent_c>(event)->mNStaff;
    if (slots <= 0)
      continue;
    u32 base = (u32)this->mList.mStaffP.v;
    for (u32 j = 0; s32(j) < slots; ++j) {
      u32 index = (u32)view<dEvDtEvent_c>(event)->mStaffIdx[j],
          staff = base + index * u32(sizeof(dEvDtStaff_c));
      if ((u32)view<dEvDtStaff_c>(staff)->mStaffType != 1 &&
          string_equal(name, staff) &&
          (u32)view<dEvDtStaff_c>(staff)->mTagID == id)
        return s32(index);
    }
  }
  return -1;
}
/* 02544114 */
void event_manager::dEvent_manager_c::exceptionProc() {
  u32 self = ea(this);
  u32 name = sdk::getExceptionName<u32>(ea(&this->mException));
  s32 id = sdk::getEventIdx<s32>(self, name, 255u);
  if (id == -1) {
    this->mException.mEventInfoIdx = 0xffffffff;
    return;
  }
  s32 index = (s32)this->mException.mEventInfoIdx;
  u32 tool = index < 200 ? u8(index) : 255,
      state = (u32)this->mException.mState;
  if (state == EXCEPTION_ORDER) {
    this->mException.mState = EXCEPTION_WAIT_START;
    sdk::orderOtherEventId<void>(0u, id, tool, 65535u, 0u, 1u);
    return;
  }
  if (state == EXCEPTION_WAIT_START) {
    if (!name)
      sdk::assertion<void>(ExceptionProcAssertionFile, 0x61Eu,
                           ExceptionNameAssertionMessage);
    if (sdk::startCheck<u32>(self, id)) {
      this->mException.mState = EXCEPTION_WAIT_END;
      if (string_equal(name, MeetShipName))
        sdk::onEventBit<void>(eventBitsAddress(), 0xF80u);
      if (string_equal(name, LookTetraName))
        sdk::onEventBit<void>(eventBitsAddress(), 0x280u);
    } else
      sdk::orderOtherEvent<void>(0u, name, 1u, 65535u);
  } else if (state == EXCEPTION_WAIT_END && sdk::endCheck<u32>(self, id)) {
    u32 play = sdk::getGameInfo<u32>();
    view<GameInfo>(play)->mEvent.mEventFlag =
        (u16)view<GameInfo>(play)->mEvent.mEventFlag | 8;
    this->mException.mState = EXCEPTION_ORDER;
    this->mException.mEventInfoIdx = 0xffffffff;
  }
}

static bool bounded_equal(u32 a, u32 b) {
  if (a == b)
    return true;
  for (u32 i = 0; i < 0x40001; ++i) {
    u8 x = (u8)view<be<u8>>(a)[i], y = (u8)view<be<u8>>(b)[i];
    if (x != y)
      return false;
    if (!x)
      return true;
  }
  return false;
}
static u32 string_length(u32 p) {
  u32 n = 0;
  while ((u8)view<be<u8>>(p)[n])
    ++n;
  return n;
}
/* 02542EDC */
s32 event_manager::dEvent_manager_c::getMyActIdx(s32 staff, u32 names,
                                                 s32 count, u32 force,
                                                 u32 prefix) {
  u32 self = ea(this);
  if (staff == -1)
    return -1;
  u32 entry =
      (u32)this->mList.mStaffP.v + u32(staff) * u32(sizeof(dEvDtStaff_c));
  bool reuse = (u8)view<dEvDtStaff_c>(entry)->mbHasAction && !force;
  if (!reuse) {
    u32 play = sdk::getGameInfo<u32>();
    reuse = false;
    if ((s8)view<GameInfo>(play)->mDebugEvent) {
      Local<DebugStrings> objects;
      u32 a = ea(&objects->a), b = ea(&objects->b), c = ea(&objects->c),
          d = ea(&objects->d);
      view<EventSafeString>(c)->mStringPtr.v = entry;
      view<EventSafeString>(c)->__vtbl.v = SafeStringVtable;
      view<EventSafeString>(a)->mStringPtr.v = DebugStaffName;
      view<EventSafeString>(a)->__vtbl.v = SafeStringVtable;
      call_ptr<void>(debugBeforeCallbackAddress, a);
      call_ptr<void>((u32)view<EventSafeStringVtable>(
                         (u32)view<EventSafeString>(a)->__vtbl.v)
                         ->assureTermination,
                     a);
      u32 left = (u32)view<EventSafeString>(a)->mStringPtr.v;
      call_ptr<void>((u32)view<EventSafeStringVtable>(
                         (u32)view<EventSafeString>(c)->__vtbl.v)
                         ->assureTermination,
                     c);
      u32 right = (u32)view<EventSafeString>(c)->mStringPtr.v;
      if (bounded_equal(left, right)) {
        view<EventSafeString>(b)->__vtbl.v = SafeStringVtable;
        view<EventSafeString>(b)->mStringPtr.v = DebugEventName;
        u32 current = sdk::getCurrentEventName<u32>(self);
        view<EventSafeString>(d)->mStringPtr.v = current;
        view<EventSafeString>(d)->__vtbl.v = SafeStringVtable;
        call_ptr<void>(debugBeforeCallbackAddress, b);
        call_ptr<void>((u32)view<EventSafeStringVtable>(
                           (u32)view<EventSafeString>(b)->__vtbl.v)
                           ->assureTermination,
                       b);
        left = (u32)view<EventSafeString>(b)->mStringPtr.v;
        call_ptr<void>((u32)view<EventSafeStringVtable>(
                           (u32)view<EventSafeString>(d)->__vtbl.v)
                           ->assureTermination,
                       d);
        right = (u32)view<EventSafeString>(d)->mStringPtr.v;
        reuse = bounded_equal(left, right);
      }
    }
  }
  if (reuse)
    return s32((u32)view<dEvDtStaff_c>(entry)->mCurActionIdx);
  u32 cut = sdk::getCurrentCut<u32>(self, staff);
  if (!cut)
    return -1;
  view<dEvDtStaff_c>(entry)->mbHasAction = 1;
  for (u32 i = 0; s32(i) < count; ++i) {
    u32 p = (u32)view<be<u32>>(names)[i];
    if (!p) {
      sdk::assertion<void>(ActionAssertionFile, 0x515u, ActionAssertionMessage);
      p = (u32)view<be<u32>>(names)[i];
    }
    bool equal;
    if (prefix) {
      u32 a = string_length(p), b = string_length(cut);
      if (a >= 100 || b >= 100) {
        sdk::assertion<void>(PrefixAssertionFile, 0x4EDu,
                             PrefixAssertionMessage);
        continue;
      }
      equal = a <= b;
      for (u32 j = 0; equal && j < a; ++j)
        if ((u8)view<be<u8>>(p)[j] != (u8)view<be<u8>>(cut)[j])
          equal = false;
    } else
      equal = string_equal(p, cut);
    if (equal) {
      view<dEvDtStaff_c>(entry)->mCurActionIdx = i;
      return s32(i);
    }
  }
  view<dEvDtStaff_c>(entry)->mCurActionIdx = 0xffffffff;
  return -1;
}
} // namespace event_manager

using namespace event_manager;
/* WWHD glue (tools/verify/autoglue.py)
*/

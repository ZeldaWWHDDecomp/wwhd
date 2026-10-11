/** Island arrival and Wind Waker lesson event tag. Verified HD behaviour; local source. */
#include "d/actor/d_a_tag_island.h"

static constexpr u32 tagIslandVtable = 0x1003F3BC;
enum { ACT_WAIT, ACT_ARRIVAL, ACT_HUNT, ACT_READY, ACT_EVENT, ACT_TACT };
enum { EVENT_WAIT, EVENT_SPEAK, EVENT_TACT_BF, EVENT_TACT_MD, EVENT_TACT_AF };
static u32 l_msgId;
// HD: dialogue uses a global manager instead of a searched msg_class.
static IslandMessageManager* l_msgManager;
static char* action_table[5] = {"WAIT", "SPEAK", "TACT_BF", "TACT_MD", "TACT_AF"};
static u16 arrival_flags[7] = { 0x0902, 0x0A20, 0x0A02, 0x1F04, 0x2E04, 0x2E02, 0x3E10};
static IslandHeaderZeros headerZeros;
static f32 headerMin;
static f32 headerMax;
static u8 headerByte;
static u8 headerFlag;

/* inlined into event procedures */
static void cutEnd(daTag_Island_c* a) { s32 staff = a->mStaffId; dComIfGp_evmng_cutEnd(staff); }
/* inlined into event procedures */
static void setTalkPartner() {
  fopAc_ac_c* ship = ((dComIfG_inf_c*)dComIfGp_get())->play.mpPlayerPtr[2];
  dComIfGp_event_setTalkPartner(ship);
}
/* inlined into event actions */
static void orderOther(daTag_Island_c* a) {
  s16 event = a->mEventId;
  fopAcM_orderOtherEventId(a, event, 0xFF, 0xFFFF, 0, 1);
}
/* inlined into event actions */
static void orderChange(daTag_Island_c* a) {
  dComIfG_inf_c* info = (dComIfG_inf_c*)dComIfGp_get();
  s16 event = a->mEventId;
  fopAc_ac_c* player = info->play.mpPlayerPtr[0];
  fopAcM_orderChangeEventId(a, player, event, 0, 0xFFFF);
}
/* inlined into event procedures */
static s16 eventNamed(const char* name) { return dComIfGp_evmng_getEventIdx(name, 0xFF); }
/* inlined into event procedures */
static be<u32>* substance(daTag_Island_c* a, const char* name) {
  s32 staff = a->mStaffId;
  return (be<u32>*)dComIfGp_evmng_getMyIntegerP(staff, name);
}

/* 024AA808 */
u8 daTag_Island_c::getEventNo() {
  return u32(mParameters) >> 24;
}

/* 024AA6B0 */
u8 daTag_Island_c::getSwbit() {
  return (u32(mParameters) >> 8) & 0xFF;
}

/* 024AA5A0 */
u8 daTag_Island_c::getType() {
  return u32(mParameters) & 0xFF;
}

/* 024AA744 */
BOOL daTag_Island_c::checkArea() {
  u32 p = dComIfGp_ea();
  fopAc_ac_c* player = ((dComIfG_inf_c*)(uintptr_t)p)->play.mpPlayerPtr[0];
  cXyz delta;
  cXyz horizontal;
  subtractPosition( &player->current.pos, &delta,
                   &current.pos);
  f32 y = delta.y;
  f32 x = delta.x;
  if (y < 0.0f)
    delta.y = -y;
  horizontal.x = x;
  horizontal.z = delta.z;
  horizontal.y = 0.0f;
  f32 squared = PSVECSquareMag( &horizontal);
  f32 distance = sqrtfHD( squared);
  if (!(distance < f32(scale.x) * 10000.0f))
    return FALSE;
  // HD: unordered height comparisons succeed, unlike a <= test.
  return !(f32(delta.y) > f32(scale.y) * 10000.0f);
}

/* 024AA814 */
void daTag_Island_c::makeEvId() {
  u32 event = getEventNo();
  u32 p = dComIfGp_ea();
  mEventId = eventIndex(&((dComIfG_inf_c*)(uintptr_t)p)->play.mEvtManager, nullptr, event);
  p = dComIfGp_ea();
  if (u32(((dComIfG_inf_c*)(uintptr_t)p)->play.mPlayerStatus[0][0]) & 0x10000)
    return;
  u32 type = getType();
  const char* name;
  switch (type) {
  case 1:
    name = "ARRIVAL_DRG2";
    break;
  case 5:
    name = "ARRIVAL_GND2";
    break;
  case 6:
    name = "ARRIVAL_WND2";
    break;
  case 4:
    name = "ARRIVAL_TWN2";
    break;
  case 2:
    name = "ARRIVAL_FST2";
    break;
  case 3:
    name = "ARRIVAL_BRK2";
    break;
  case 7:
    name = "PUROLO_RETURN2";
    break;
  default:
    return;
  }
  s32 idx = eventNamed(name);
  if (idx != -1)
    mEventId = idx;
}

/* 024AA5AC */
u16 daTag_Island_c::getArrivalFlag() {
  u32 type = getType();
  if (type < 1 || type > 7)
    return 0;
  return u16(arrival_flags[type - 1]);
}

/* 024AA6BC */
BOOL daTag_Island_c::otherCheck() {
  u32 type = getType();
  if (type == 5)
    return dComIfGs_isEventBit(0x1608) ? TRUE : FALSE;
  if (type == 6)
    return dComIfGs_isEventBit(0x1604) ? TRUE : FALSE;
  return TRUE;
}

/* 024AA600 */
BOOL daTag_Island_c::arrivalTerms() {
  u16 flag = getArrivalFlag();
  if (dComIfGs_isEventBit(flag))
    return FALSE;
  u32 type = getType();
  if (type == 4)
    return dKy_checkEventNightStop() ? TRUE : FALSE;
  if (type == 7) {
    if (!dKy_checkEventNightStop())
      return FALSE;
    if (!dComIfGs_checkGetItem(dItemNo_BOMB_BAG_e))
      return FALSE;
  }
  return TRUE;
}

/* 024AAACC */
void daTag_Island_c::demoInitProc() {
  m290 = 0;
  u16 flag = getArrivalFlag();
  dComIfGs_onEventBit(flag);
}

/* 024AB088 */
s32 daTag_Island_c::next_msgStatus(be<u32> *message) {
  return fopMsgStts_MSG_ENDS_e;
}

/* 024AB080 */
u32 daTag_Island_c::getMsg() {
  return m2AC;
}

/* 024AAE0C */
void daTag_Island_c::talkInit() {
  mTalkState = 0;
}

/* 024AB090 */
u16 daTag_Island_c::talk() {
  // HD: snapshot the manager before any message calls.
  IslandMessageManager* messages = l_msgManager;
  s8 state = mTalkState;
  u16 status = 0xFF;
  if (state == 0) {
    l_msgId = fpcM_ERROR_PROCESS_ID_e;
    mMsg = getMsg();
    mTalkState = 1;
  } else if (state != -1) {
    if (u32(l_msgId) == 0xFFFFFFFF) {
      u32 message = mMsg;
      u32 id =
          setMessage(messages, message, &eyePos);
      l_msgId = id;
    } else if (state == 1)
      mTalkState = 2;
    else if (state == 2) {
      status = u16(getMessageStatus(messages));
      if (status == 0xE) {
        s32 next = next_msgStatus(&mMsg);
        setMessageStatus(messages, next);
        if (s32(getMessageStatus(messages)) == 0xF) {
          u32 message = mMsg;
          setMessage(messages, message, nullptr);
        }
      } else if (status == 0x12) {
        setMessageStatus(messages, 0x13);
        mTalkState = -1;
      }
    }
  }
  return status;
}

/* 024AAF18 */
void daTag_Island_c::demoInitTact_Bf() {
  talkInit();
  be<u32>* param = substance(this, "prm0");
  if (!param) {
    JUTAssertion::showAssert( "d_a_tag_island.cpp", 0x167, "a_intP");
    return;
  }
  // HD: the melody byte is read before the play lookup and the full message parameter after it.
  u8 melody = u8(*param);
  u32 p = dComIfGp_ea();
  ((dComIfG_inf_c*)(uintptr_t)p)->play.mMelodyNum = melody;
  m2AC = u32(*param) == 6 ? 0x5B4 : 0x5B3;
}

/* 024AB274 */
BOOL daTag_Island_c::demoProcTact_Bf() {
  if (talk() == 0x15)
    cutEnd(this);
  return TRUE;
}

/* 024AAFD8 */
void daTag_Island_c::demoInitTact_Af() {
  u32 p = dComIfGp_ea();
  // HD: snapshot the manager before any message calls.
  IslandMessageManager* messages = l_msgManager;
  IslandPlayer* player = static_cast<IslandPlayer*>(((dComIfG_inf_c*)(uintptr_t)p)->play.mpPlayerPtr[0].get());
  setMessageStatus(messages, 0x10);
  messages->mSendOn = 1;
  s32 cancel = player->getTactTimerCancel();
  u16 flags = mFlag;
  mFlag = cancel > 0 ? flags | daTagIsl_UNK_04 : flags & 0xFFFB;
}

/* 024AB2C0 */
BOOL daTag_Island_c::demoProcTact_Af() {
  u16 status = talk();
  if (status == 0x12 || status == 0xFE) {
    u16 flags = mFlag;
    if (flags & daTagIsl_UNK_01) {
      flags &= 0xFFFE;
      mFlag = flags;
      const char* name;
      if (flags & daTagIsl_UNK_04) {
        u8 stage = m2A0;
        u32 p = dComIfGp_ea();
        name = stage == 1 ? "TACT_TEARCH4" : "TACT_TEARCH1";
        mEventId = eventIndex(&((dComIfG_inf_c*)(uintptr_t)p)->play.mEvtManager, name, 0xFF);
      } else {
        if (u8(m2A0) == 0) {
          m2A0 = 1;
          name = "TACT_TEARCH2";
        } else
          name = "TACT_TEARCH3";
        mEventId = eventNamed(name);
      }
      mFlag = u16(mFlag) | daTagIsl_UNK_02;
      orderChange(this);
    } else {
      JUTAssertion::showAssert( "d_a_tag_island.cpp", 0x1B4, "0");
      cutEnd(this);
    }
  }
  return TRUE;
}

/* 024AAE18 */
void daTag_Island_c::demoInitSpeak() {
  talkInit();
  be<u32>* param = substance(this, "MsgNo");
  if (!param)
    JUTAssertion::showAssert( "d_a_tag_island.cpp", 0x1C3, "a_intP");
  u32 message = u32(*param);
  m2AC = message;
  if (message == 0x638 && u8(m2A0) != 0)
    m2AC = 0x63A;
}

/* 024AB1C8 */
BOOL daTag_Island_c::demoProcSpeak() {
  u16 status = talk();
  if (status == 0x12 || status == 0xFE)
    cutEnd(this);
  return FALSE;
}

/* 024AAEB4 */
void daTag_Island_c::demoInitWait() {
  // HD: read the signed low half of the serialized integer.
  be<u32>* param = substance(this, "Timer");
  mTimer = param ? s16((*param) & 0xFFFF) : 0;
}

/* 024AB21C */
BOOL daTag_Island_c::demoProcWait() {
  s16 timer = mTimer;
  if (timer > 0)
    mTimer = timer - 1;
  else
    cutEnd(this);
  return FALSE;
}

/* 024AADFC */
void daTag_Island_c::demoInitCom() {
  mFlag = u16(mFlag) | daTagIsl_UNK_01;
}

/* 024AAB08 */
void daTag_Island_c::demoProcCom_ikada() {
  IslandRaft* ikada = searchRaft("ikada_h", nullptr, nullptr);
  if (!ikada) return;
  // HD: keep word copies and their interleaved order, including possible aliasing.
  u32 x = coordinateWord(ikada->mInitPos.x);
  u32 y = coordinateWord(ikada->mInitPos.y);
  coordinateWord(ikada->current.pos.x) = x;
  coordinateWord(ikada->mCurPathP0.y) = y;
  u32 z = coordinateWord(ikada->mInitPos.z);
  coordinateWord(ikada->current.pos.y) = y;
  ikada->mCurPathPoint = 0;
  coordinateWord(ikada->mCurPathP0.x) = x;
  path_layout::dPath* path = ikada->mpPath;
  coordinateWord(ikada->current.pos.z) = z;
  coordinateWord(ikada->mCurPathP0.z) = z;
  ikada->m02CC = path->m_points[0].m_position;
  ikada->mCurPathP1 = path->m_points[1].m_position;
  ikada->mPathPosTarget = path->m_points[0].m_position;
  // HD: the raft matrix update is an explicit call after resetting its path state.
  setRaftMtx(ikada);
}

/* 024AADB0 */
s32 daTag_Island_c::getNowEventAction() {
  s32 staff = mStaffId;
  u32 p = dComIfGp_ea();
  return eventAction(&((dComIfG_inf_c*)(uintptr_t)p)->play.mEvtManager, staff, action_table, 5, 0, 1);
}

/* 024AB4B4 */
BOOL daTag_Island_c::demoProc() {
  if (u16(mFlag) & daTagIsl_UNK_02) {
    u32 p = dComIfGp_ea();
    mStaffId = eventStaff(&((dComIfG_inf_c*)(uintptr_t)p)->play.mEvtManager, "TagIsl", nullptr, 0);
    mFlag = u16(mFlag) & 0xFFFD;
    setTalkPartner();
  }
  s32 action = getNowEventAction(), staff = mStaffId;
  u32 p = dComIfGp_ea();
  if (eventAdvance(&((dComIfG_inf_c*)(uintptr_t)p)->play.mEvtManager, staff)) {
    demoInitCom();
    switch (action) {
    case EVENT_WAIT:
      demoInitWait();
      break;
    case EVENT_SPEAK:
      demoInitSpeak();
      break;
    case EVENT_TACT_BF:
      demoInitTact_Bf();
      break;
    case EVENT_TACT_AF:
      demoInitTact_Af();
      break;
    }
  }
  switch (action) {
  case EVENT_WAIT:
    demoProcWait();
    return FALSE;
  case EVENT_SPEAK:
    return demoProcSpeak();
  case EVENT_TACT_BF:
    return demoProcTact_Bf();
  case EVENT_TACT_AF:
    return demoProcTact_Af();
  default:
    cutEnd(this);
    return FALSE;
  }
}

/* 024AB664 */
BOOL daTag_Island_c::actionTact() {
  s16 event = mEventId;
  u32 p = dComIfGp_ea();
  if (eventEndCheck(&((dComIfG_inf_c*)(uintptr_t)p)->play.mEvtManager, event)) {
    mAction = ACT_WAIT;
    dComIfGp_event_reset();
  } else
    demoProc();
  return TRUE;
}

/* 024AABC8 */
BOOL daTag_Island_c::actionEvent() {
  s16 event = mEventId;
  u32 p = dComIfGp_ea();
  if (eventEndCheck(&((dComIfG_inf_c*)(uintptr_t)p)->play.mEvtManager, event)) {
    if (getType() == 1) {
      mAction = ACT_TACT;
      mFlag = u16(mFlag) | daTagIsl_UNK_02;
      mEventId = eventNamed("TACT_TEARCH0");
      orderChange(this);
      m2A0 = 0;
    } else {
      mAction = ACT_WAIT;
      dComIfGp_event_reset();
    }
  } else
    demoProcCom_ikada();
  return TRUE;
}

/* 024AACAC */
BOOL daTag_Island_c::actionReady() {
  s32 sw = getSwbit();
  if (eventInfo.checkCommandDemoAccrpt()) {
    demoInitProc();
    setTalkPartner();
    mAction = ACT_EVENT;
    actionEvent();
    if (sw != 0xFF)
      dComIfGs_onSwitch(sw, s8(current.roomNo));
  } else if (sw != 0xFF && dComIfGs_isSwitch(sw, s8(current.roomNo)))
    mAction = ACT_WAIT;
  else {
    makeEvId();
    orderOther(this);
  }
  return TRUE;
}

/* 024AA9B8 */
BOOL daTag_Island_c::actionHunt() {
  s32 sw = getSwbit();
  if (sw != 0xFF &&
      dComIfGs_isSwitch(sw, s8(current.roomNo)))
    mAction = ACT_WAIT;
  else if (otherCheck() && checkArea()) {
    makeEvId();
    mAction = ACT_READY;
    orderOther(this);
  }
  return TRUE;
}

/* 024AAA74 */
BOOL daTag_Island_c::actionArrival() {
  if (arrivalTerms()) {
    mAction = ACT_HUNT;
    actionHunt();
  } else
    mAction = ACT_WAIT;
  return TRUE;
}

/* 024AB780 */
s32 daTag_Island_c::create() {
  if (!(u32(actor_condition) & fopAcCnd_INIT_e)) {
    if (this != nullptr) {
      fopAc_ac_c_ct(this);
      __vtbl = tagIslandVtable;
    }
    actor_condition = u32(actor_condition) | fopAcCnd_INIT_e;
  }
  s32 sw = getSwbit();
  makeEvId();
  eventInfo.mEventId = s16(mEventId);
  u8 event = getEventNo();
  s16 idx = mEventId;
  eventInfo.mMapToolId = event;
  if (idx != -1 && sw != 0xFF &&
      !dComIfGs_isSwitch(sw, s8(current.roomNo))) {
    mAction = ACT_ARRIVAL;
    // HD: immediately evaluate arrival state during creation.
    actionArrival();
    shape_angle.x = 0;
    current.angle.x = 0;
    shape_angle.z = 0;
    current.angle.z = 0;
  } else {
    current.angle.z = 0;
    shape_angle.x = 0;
    current.angle.x = 0;
    mAction = ACT_WAIT;
    shape_angle.z = 0;
  }
  return cPhs_COMPLEATE_e;
}

/* 024AB6DC */
BOOL daTag_Island_c::execute() {
  switch (u8(mAction)) {
  case ACT_ARRIVAL:
    actionArrival();
    break;
  case ACT_HUNT:
    actionHunt();
    break;
  case ACT_READY:
    actionReady();
    break;
  case ACT_EVENT:
    actionEvent();
    break;
  case ACT_TACT:
    actionTact();
    break;
  }
  return TRUE;
}

/* 024AB928 */
BOOL daTag_Island_Draw(daTag_Island_c *a) {
  return TRUE;
}

/* 024AB770 */
BOOL daTag_Island_IsDelete(daTag_Island_c *a) {
  return TRUE;
}

/* 024AB778 */
// HD: deletion callback is empty; the deleting destructor is a separate entry.
BOOL daTag_Island_Delete(daTag_Island_c *a) {
  return TRUE;
}

/* 024AB890 */
s32 daTag_Island_Create(daTag_Island_c *a) {
  return a->create();
}

// HD: compiler-generated code of this translation unit.
/* 024AB894 */
void __sinit_d_a_tag_island_cpp() {
  // HD: retain GHS's nonsequential zero-store order.
  headerZeros.word2 = 0;
  headerZeros.word0 = 0;
  headerZeros.word3 = 0;
  headerZeros.word1 = 0;
  __register_global_object(0x101D1B10);
  headerMin = -3.1415927f;
  headerMax = 3.1415927f;
  initializeHeaderByte(&headerByte);
  __register_global_object(0x101D1B1C);
  initializeHeaderFlag(&headerFlag);
  __register_global_object(0x101D1B28);
}

/* 024AB930 */
void daTag_Island_dt(daTag_Island_c *a, s32 flags) {
  if (a) {
    __dt(static_cast<fopAc_ac_c*>(a));
    if (flags & 1)
      operator_delete(a);
  }
}


/* WWHD glue
str 1003F404 "ARRIVAL_TWN2"
str 1003F484 "MsgNo"
str 1003F508 "TACT_TEARCH3"
str 1003F48C "a_intP" in 024AAE18
str 1003F414 "ARRIVAL_FST2"
str 1003F494 "d_a_tag_island.cpp" in 024AAE18
str 1003F518 "d_a_tag_island.cpp" in 024AB2C0
str 1003F424 "ARRIVAL_BRK2"
str 1003F4A8 "Timer"
str 1003F4B0 "prm0"
str 1003F434 "PUROLO_RETURN2"
str 1003F4B8 "a_intP" in 024AAF18
str 1003F4C0 "d_a_tag_island.cpp" in 024AAF18
str 1003F44C "TACT_TEARCH0"
str 1003F3D4 "ARRIVAL_DRG2"
str 1003F4D4 "0"
str 1003F4D8 "TACT_TEARCH4"
str 1003F3E4 "ARRIVAL_GND2"
str 1003F4E8 "TACT_TEARCH1"
str 1003F3F4 "ARRIVAL_WND2"
str 1003F4F8 "TACT_TEARCH2"
str 1003F52C "TagIsl"
str 1003F444 "ikada_h"
data arrival_flags 1003F3A8
data action_table 101D1AFC
data l_msgId 1046E34C
data l_msgManager 101F4B5C
data headerZeros 1046E35C
data headerMin 1046E350
data headerMax 1046E354
data headerByte 1046E358
data headerFlag 1046E359
ext 02556BC0 BOOL dKy_checkEventNightStop()
ext 02543F10 s16 eventIndex(dEvent_manager_c*, const char*, s32)
ext 025440C8 BOOL eventEndCheck(dEvent_manager_c*, s16)
ext 025447C8 BOOL eventAdvance(dEvent_manager_c*, s32)
ext 02542EDC s32 eventAction(dEvent_manager_c*, s32, gptr<char>*, s32, s32, s32)
ext 02542D88 s32 eventStaff(dEvent_manager_c*, const char*, void*, s32)
ext 025D9F38 IslandRaft* searchRaft(const char*, void*, void*)
ext 02360420 void setRaftMtx(IslandRaft*)
ext 0201ADE0 void subtractPosition(const cXyz*, cXyz*, const cXyz*)
ext 028F4384 f32 sqrtfHD(f32)
ext 025F7DB0 u32 setMessage(IslandMessageManager*, u32, cXyz*)
ext 025F795C u32 getMessageStatus(IslandMessageManager*)
ext 025F74D0 void setMessageStatus(IslandMessageManager*, s32)
ext 028ED6F8 void initializeHeaderByte(be<u8>*)
ext 028EAB2C void initializeHeaderFlag(be<u8>*)
*/

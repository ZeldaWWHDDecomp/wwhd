#include "d/actor/d_a_tag_island.h"
#include "bindings.h"

// Keep guest callees' linkage stores below addressable vector temporaries.
template <class R = void, class... A>
static R islandCall(u32 target, A... args) {
  u32 sp = gabi::cpu->r[1];
  gabi::Local<be<u32>[4]> linkage;
  gabi::store<u32>(linkage.a, sp);
  return gabi::call<R>(target, args...);
}
template <class R = void, class... A>
static R islandVirtualCall(u32 target, A... args) {
  u32 sp = gabi::cpu->r[1];
  gabi::Local<be<u32>[4]> linkage;
  gabi::store<u32>(linkage.a, sp);
  return gabi::call_ptr<R>(target, args...);
}
#undef WWHD_FUNC
#define WWHD_FUNC(addr, R, ...)                                                \
  if (gabi::Activation::nested())                                              \
    return islandCall<R>(addr __VA_OPT__(, ) __VA_ARGS__);                     \
  gabi::Activation wwhd_activation_

// These path-coordinate copies have no arithmetic; preserve their IEEE bits.
static f32 pathCoordinate(u32 address) {
  u32 bits = gabi::load<u32>(address);
  f32 value;
  memcpy(&value, &bits, 4);
  return value;
}
static u32 play() { return islandCall<u32>(0x025200D4); }
static u32 save() { return gabi::load<u32>(0x101F84DC); }
static void cutEnd(daTag_Island_c *a) {
  s32 staff = a->staffId;
  u32 p = play();
  islandCall<void>(0x02543280, p + 0x52C4, staff);
}
static void resetEvent() {
  u32 p = play();
  u16 flags = gabi::load<u16>(p + 0x52B8);
  gabi::store<u16>(p + 0x52B8, flags | 8);
}
static void setTalkPartner() {
  u32 p = play();
  u32 ship = gabi::load<u32>(p + 0x5B3C);
  p = play();
  u32 id = islandCall<u32>(0x0253F124, p + 0x51D0, ship);
  gabi::store<u32>(p + 0x529C, id);
}
static void orderOther(daTag_Island_c *a) {
  s16 event = a->eventId;
  islandCall<void>(0x025D7A58, a, event, 0xFF, 0xFFFF, 0, 1);
}
static void orderChange(daTag_Island_c *a) {
  u32 p = play();
  s16 event = a->eventId;
  u32 player = gabi::load<u32>(p + 0x5B34);
  islandCall<void>(0x025D7970, a, player, event, 0, 0xFFFF);
}
static s16 eventNamed(u32 name) {
  u32 p = play();
  return islandCall<s16>(0x02543F10, p + 0x52C4, name, 0xFF);
}
static u32 substance(daTag_Island_c *a, u32 name) {
  s32 staff = a->staffId;
  u32 p = play();
  return islandCall<u32>(0x0254487C, p + 0x52C4, staff, name, 3);
}

u8 islandType(daTag_Island_c *a) {
  WWHD_FUNC(0x024AA5A0, u8, a);
  return u32(a->mParameters) & 0xFF;
}
VERIFY(0x024AA5A0, islandType);
u16 islandArrivalFlag(daTag_Island_c *a) {
  WWHD_FUNC(0x024AA5AC, u16, a);
  u32 type = islandType(a);
  if (type < 1 || type > 7)
    return 0;
  return gabi::load<u16>(0x1003F3A6 + type * 2);
}
VERIFY(0x024AA5AC, islandArrivalFlag);
BOOL islandArrivalTerms(daTag_Island_c *a) {
  WWHD_FUNC(0x024AA600, BOOL, a);
  u16 flag = islandArrivalFlag(a);
  if (islandCall<BOOL>(0x025B8B94, save() + 0x644, flag))
    return FALSE;
  u32 type = islandType(a);
  if (type == 4)
    return islandCall<BOOL>(0x02556BC0) ? TRUE : FALSE;
  if (type == 7) {
    if (!islandCall<BOOL>(0x02556BC0))
      return FALSE;
    if (!islandCall<BOOL>(0x02520C0C, 0x31))
      return FALSE;
  }
  return TRUE;
}
VERIFY(0x024AA600, islandArrivalTerms);
u8 islandSwitch(daTag_Island_c *a) {
  WWHD_FUNC(0x024AA6B0, u8, a);
  return (u32(a->mParameters) >> 8) & 0xFF;
}
VERIFY(0x024AA6B0, islandSwitch);
BOOL islandOtherCheck(daTag_Island_c *a) {
  WWHD_FUNC(0x024AA6BC, BOOL, a);
  u32 type = islandType(a);
  if (type == 5)
    return islandCall<BOOL>(0x025B8B94, save() + 0x644, 0x1608) ? TRUE : FALSE;
  if (type == 6)
    return islandCall<BOOL>(0x025B8B94, save() + 0x644, 0x1604) ? TRUE : FALSE;
  return TRUE;
}
VERIFY(0x024AA6BC, islandOtherCheck);
BOOL islandCheckArea(daTag_Island_c *a) {
  WWHD_FUNC(0x024AA744, BOOL, a);
  u32 p = play();
  u32 player = gabi::load<u32>(p + 0x5B34);
  gabi::Local<cXyz> delta, horizontal;
  islandCall<void>(0x0201ADE0, gabi::at<cXyz>(player + 0x314), delta.get(),
                   &a->current.pos);
  f32 y = delta->y;
  f32 x = delta->x;
  if (y < 0.0f)
    delta->y = -y;
  horizontal->x = x;
  horizontal->z = delta->z;
  horizontal->y = 0.0f;
  f32 squared = islandCall<f32>(0x028E8DD0, horizontal.get());
  f32 distance = islandCall<f32>(0x028F4384, squared);
  if (!(distance < f32(a->scale.x) * 10000.0f))
    return FALSE;
  return !(f32(delta->y) > f32(a->scale.y) * 10000.0f);
}
VERIFY(0x024AA744, islandCheckArea);
u8 islandEventNo(daTag_Island_c *a) {
  WWHD_FUNC(0x024AA808, u8, a);
  return u32(a->mParameters) >> 24;
}
VERIFY(0x024AA808, islandEventNo);
void islandMakeEvent(daTag_Island_c *a) {
  WWHD_FUNC(0x024AA814, void, a);
  u32 event = islandEventNo(a);
  u32 p = play();
  a->eventId = islandCall<s16>(0x02543F10, p + 0x52C4, 0, event);
  p = play();
  if (gabi::load<u32>(p + 0x5CD8) & 0x10000)
    return;
  u32 type = islandType(a), name;
  switch (type) {
  case 1:
    name = 0x1003F3D4;
    break;
  case 5:
    name = 0x1003F3E4;
    break;
  case 6:
    name = 0x1003F3F4;
    break;
  case 4:
    name = 0x1003F404;
    break;
  case 2:
    name = 0x1003F414;
    break;
  case 3:
    name = 0x1003F424;
    break;
  case 7:
    name = 0x1003F434;
    break;
  default:
    return;
  }
  s32 idx = eventNamed(name);
  if (idx != -1)
    a->eventId = idx;
}
VERIFY(0x024AA814, islandMakeEvent);
BOOL islandActionHunt(daTag_Island_c *a) {
  WWHD_FUNC(0x024AA9B8, BOOL, a);
  s32 sw = islandSwitch(a);
  if (sw != 0xFF &&
      islandCall<BOOL>(0x025BA0C0, save() + 0x20, sw, s8(a->current.roomNo)))
    a->action = 0;
  else if (islandOtherCheck(a) && islandCheckArea(a)) {
    islandMakeEvent(a);
    a->action = 3;
    orderOther(a);
  }
  return TRUE;
}
VERIFY(0x024AA9B8, islandActionHunt);
BOOL islandActionArrival(daTag_Island_c *a) {
  WWHD_FUNC(0x024AAA74, BOOL, a);
  if (islandArrivalTerms(a)) {
    a->action = 2;
    islandActionHunt(a);
  } else
    a->action = 0;
  return TRUE;
}
VERIFY(0x024AAA74, islandActionArrival);
void islandDemoInit(daTag_Island_c *a) {
  WWHD_FUNC(0x024AAACC, void, a);
  a->arrivalState = 0;
  u16 flag = islandArrivalFlag(a);
  islandCall<void>(0x025B8B68, save() + 0x644, flag);
}
VERIFY(0x024AAACC, islandDemoInit);
void islandResetRaft(daTag_Island_c *a) {
  WWHD_FUNC(0x024AAB08, void, a);
  u32 raft = islandCall<u32>(0x025D9F38, STR(0x1003F444), 0, 0);
  if (!raft)
    return;
  u32 x = gabi::load<u32>(raft + 0x1688), y = gabi::load<u32>(raft + 0x168C);
  gabi::store<u32>(raft + 0x314, x);
  gabi::store<u32>(raft + 0x3C8, y);
  u32 z = gabi::load<u32>(raft + 0x1690);
  gabi::store<u32>(raft + 0x318, y);
  gabi::store<u8>(raft + 0x3D0, 0);
  gabi::store<u32>(raft + 0x3C4, x);
  u32 path = gabi::load<u32>(raft + 0x3D4);
  gabi::store<u32>(raft + 0x31C, z);
  gabi::store<u32>(raft + 0x3CC, z);
  u32 points = gabi::load<u32>(path + 8);
  for (u32 i = 0; i < 3; i++)
    gabi::store<f32>(raft + 0x3E8 + i * 4, pathCoordinate(points + 4 + i * 4));
  points = gabi::load<u32>(path + 8);
  for (u32 i = 0; i < 3; i++)
    gabi::store<f32>(raft + 0x3DC + i * 4,
                     pathCoordinate(points + 0x14 + i * 4));
  points = gabi::load<u32>(path + 8);
  for (u32 i = 0; i < 3; i++)
    gabi::store<f32>(raft + 0x3F8 + i * 4, pathCoordinate(points + 4 + i * 4));
  islandCall<void>(0x02360420, raft);
}
VERIFY(0x024AAB08, islandResetRaft);
BOOL islandActionEvent(daTag_Island_c *a) {
  WWHD_FUNC(0x024AABC8, BOOL, a);
  s16 event = a->eventId;
  u32 p = play();
  if (islandCall<BOOL>(0x025440C8, p + 0x52C4, event)) {
    if (islandType(a) == 1) {
      a->action = 5;
      a->flags = u16(a->flags) | 2;
      a->eventId = eventNamed(0x1003F44C);
      orderChange(a);
      a->lessonStage = 0;
    } else {
      a->action = 0;
      resetEvent();
    }
  } else
    islandResetRaft(a);
  return TRUE;
}
VERIFY(0x024AABC8, islandActionEvent);
BOOL islandActionReady(daTag_Island_c *a) {
  WWHD_FUNC(0x024AACAC, BOOL, a);
  s32 sw = islandSwitch(a);
  if (gabi::load<u16>(gabi::ea(a) + 0xF8) == 2) {
    islandDemoInit(a);
    setTalkPartner();
    a->action = 4;
    islandActionEvent(a);
    if (sw != 0xFF)
      islandCall<void>(0x025B9E38, save() + 0x20, sw, s8(a->current.roomNo));
  } else if (sw != 0xFF && islandCall<BOOL>(0x025BA0C0, save() + 0x20, sw,
                                            s8(a->current.roomNo)))
    a->action = 0;
  else {
    islandMakeEvent(a);
    orderOther(a);
  }
  return TRUE;
}
VERIFY(0x024AACAC, islandActionReady);
s32 islandEventAction(daTag_Island_c *a) {
  WWHD_FUNC(0x024AADB0, s32, a);
  s32 staff = a->staffId;
  u32 p = play();
  return islandCall<s32>(0x02542EDC, p + 0x52C4, staff, 0x101D1AFC, 5, 0, 1);
}
VERIFY(0x024AADB0, islandEventAction);
void islandInitCommon(daTag_Island_c *a) {
  WWHD_FUNC(0x024AADFC, void, a);
  a->flags = u16(a->flags) | 1;
}
VERIFY(0x024AADFC, islandInitCommon);
void islandTalkInit(daTag_Island_c *a) {
  WWHD_FUNC(0x024AAE0C, void, a);
  a->talkState = 0;
}
VERIFY(0x024AAE0C, islandTalkInit);
void islandInitSpeak(daTag_Island_c *a) {
  WWHD_FUNC(0x024AAE18, void, a);
  islandTalkInit(a);
  u32 param = substance(a, 0x1003F484);
  if (!param)
    islandCall<void>(0x0273AA24, STR(0x1003F494), 0x1C3, STR(0x1003F48C));
  u32 message = gabi::load<u32>(param);
  a->nextMessage = message;
  if (message == 0x638 && u8(a->lessonStage) != 0)
    a->nextMessage = 0x63A;
}
VERIFY(0x024AAE18, islandInitSpeak);
void islandInitWait(daTag_Island_c *a) {
  WWHD_FUNC(0x024AAEB4, void, a);
  u32 param = substance(a, 0x1003F4A8);
  a->timer = param ? gabi::load<s16>(param + 2) : 0;
}
VERIFY(0x024AAEB4, islandInitWait);
void islandInitTactBefore(daTag_Island_c *a) {
  WWHD_FUNC(0x024AAF18, void, a);
  islandTalkInit(a);
  u32 param = substance(a, 0x1003F4B0);
  if (!param) {
    islandCall<void>(0x0273AA24, STR(0x1003F4C0), 0x167, STR(0x1003F4B8));
    return;
  }
  u8 melody = gabi::load<u8>(param + 3);
  u32 p = play();
  gabi::store<u8>(p + 0x5BDA, melody);
  a->nextMessage = gabi::load<u32>(param) == 6 ? 0x5B4 : 0x5B3;
}
VERIFY(0x024AAF18, islandInitTactBefore);
void islandInitTactAfter(daTag_Island_c *a) {
  WWHD_FUNC(0x024AAFD8, void, a);
  u32 p = play();
  u32 messages = gabi::load<u32>(0x101F4B5C),
      player = gabi::load<u32>(p + 0x5B34);
  islandCall<void>(0x025F74D0, messages, 0x10);
  gabi::store<u8>(messages + 0x921, 1);
  u32 vt = gabi::load<u32>(player + 0xB4), target = gabi::load<u32>(vt + 0x34);
  s32 cancel = islandVirtualCall<s32>(target, player);
  u16 flags = a->flags;
  a->flags = cancel > 0 ? flags | 4 : flags & 0xFFFB;
}
VERIFY(0x024AAFD8, islandInitTactAfter);
u32 islandGetMessage(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB080, u32, a);
  return a->nextMessage;
}
VERIFY(0x024AB080, islandGetMessage);
s32 islandNextMessage(daTag_Island_c *a, be<u32> *message) {
  WWHD_FUNC(0x024AB088, s32, a, message);
  return 0x10;
}
VERIFY(0x024AB088, islandNextMessage);
u16 islandTalk(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB090, u16, a);
  u32 messages = gabi::load<u32>(0x101F4B5C);
  s8 state = a->talkState;
  u16 status = 0xFF;
  if (state == 0) {
    gabi::store<u32>(0x1046E34C, 0xFFFFFFFF);
    a->message = islandGetMessage(a);
    a->talkState = 1;
  } else if (state != -1) {
    if (gabi::load<u32>(0x1046E34C) == 0xFFFFFFFF) {
      u32 message = a->message;
      u32 id =
          islandCall<u32>(0x025F7DB0, messages, message, gabi::ea(a) + 0x37C);
      gabi::store<u32>(0x1046E34C, id);
    } else if (state == 1)
      a->talkState = 2;
    else if (state == 2) {
      status = islandCall<u16>(0x025F795C, messages);
      if (status == 0xE) {
        s32 next = islandNextMessage(a, &a->message);
        islandCall<void>(0x025F74D0, messages, next);
        if (islandCall<s32>(0x025F795C, messages) == 0xF) {
          u32 message = a->message;
          islandCall<void>(0x025F7DB0, messages, message, 0);
        }
      } else if (status == 0x12) {
        islandCall<void>(0x025F74D0, messages, 0x13);
        a->talkState = -1;
      }
    }
  }
  return status;
}
VERIFY(0x024AB090, islandTalk);
BOOL islandProcSpeak(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB1C8, BOOL, a);
  u16 status = islandTalk(a);
  if (status == 0x12 || status == 0xFE)
    cutEnd(a);
  return FALSE;
}
VERIFY(0x024AB1C8, islandProcSpeak);
BOOL islandProcWait(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB21C, BOOL, a);
  s16 timer = a->timer;
  if (timer > 0)
    a->timer = timer - 1;
  else
    cutEnd(a);
  return FALSE;
}
VERIFY(0x024AB21C, islandProcWait);
BOOL islandProcTactBefore(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB274, BOOL, a);
  if (islandTalk(a) == 0x15)
    cutEnd(a);
  return TRUE;
}
VERIFY(0x024AB274, islandProcTactBefore);
BOOL islandProcTactAfter(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB2C0, BOOL, a);
  u16 status = islandTalk(a);
  if (status == 0x12 || status == 0xFE) {
    u16 flags = a->flags;
    if (flags & 1) {
      flags &= 0xFFFE;
      a->flags = flags;
      u32 name;
      if (flags & 4) {
        u8 stage = a->lessonStage;
        u32 p = play();
        name = stage == 1 ? 0x1003F4D8 : 0x1003F4E8;
        a->eventId = islandCall<s16>(0x02543F10, p + 0x52C4, name, 0xFF);
      } else {
        if (u8(a->lessonStage) == 0) {
          a->lessonStage = 1;
          name = 0x1003F4F8;
        } else
          name = 0x1003F508;
        a->eventId = eventNamed(name);
      }
      a->flags = u16(a->flags) | 2;
      orderChange(a);
    } else {
      islandCall<void>(0x0273AA24, STR(0x1003F518), 0x1B4, STR(0x1003F4D4));
      cutEnd(a);
    }
  }
  return TRUE;
}
VERIFY(0x024AB2C0, islandProcTactAfter);
BOOL islandDemoProc(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB4B4, BOOL, a);
  if (u16(a->flags) & 2) {
    u32 p = play();
    a->staffId = islandCall<s32>(0x02542D88, p + 0x52C4, STR(0x1003F52C), 0, 0);
    a->flags = u16(a->flags) & 0xFFFD;
    setTalkPartner();
  }
  s32 action = islandEventAction(a), staff = a->staffId;
  u32 p = play();
  if (islandCall<BOOL>(0x025447C8, p + 0x52C4, staff)) {
    islandInitCommon(a);
    switch (action) {
    case 0:
      islandInitWait(a);
      break;
    case 1:
      islandInitSpeak(a);
      break;
    case 2:
      islandInitTactBefore(a);
      break;
    case 4:
      islandInitTactAfter(a);
      break;
    }
  }
  switch (action) {
  case 0:
    islandProcWait(a);
    return FALSE;
  case 1:
    return islandProcSpeak(a);
  case 2:
    return islandProcTactBefore(a);
  case 4:
    return islandProcTactAfter(a);
  default:
    cutEnd(a);
    return FALSE;
  }
}
VERIFY(0x024AB4B4, islandDemoProc);
BOOL islandActionTact(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB664, BOOL, a);
  s16 event = a->eventId;
  u32 p = play();
  if (islandCall<BOOL>(0x025440C8, p + 0x52C4, event)) {
    a->action = 0;
    resetEvent();
  } else
    islandDemoProc(a);
  return TRUE;
}
VERIFY(0x024AB664, islandActionTact);
BOOL islandExecute(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB6DC, BOOL, a);
  switch (u8(a->action)) {
  case 1:
    islandActionArrival(a);
    break;
  case 2:
    islandActionHunt(a);
    break;
  case 3:
    islandActionReady(a);
    break;
  case 4:
    islandActionEvent(a);
    break;
  case 5:
    islandActionTact(a);
    break;
  }
  return TRUE;
}
VERIFY(0x024AB6DC, islandExecute);
BOOL islandIsDelete(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB770, BOOL, a);
  return TRUE;
}
VERIFY(0x024AB770, islandIsDelete);
BOOL islandDelete(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB778, BOOL, a);
  return TRUE;
}
VERIFY(0x024AB778, islandDelete);
s32 islandCreateBody(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB780, s32, a);
  if (!(u32(a->actor_condition) & 8)) {
    if (a) {
      islandCall<void>(0x025D4ED0, a);
      a->__vtbl = 0x1003F3BC;
    }
    a->actor_condition = u32(a->actor_condition) | 8;
  }
  s32 sw = islandSwitch(a);
  islandMakeEvent(a);
  gabi::store<s16>(gabi::ea(a) + 0xFC, s16(a->eventId));
  u8 event = islandEventNo(a);
  s16 idx = a->eventId;
  gabi::store<u8>(gabi::ea(a) + 0xFE, event);
  if (idx != -1 && sw != 0xFF &&
      !islandCall<BOOL>(0x025BA0C0, save() + 0x20, sw, s8(a->current.roomNo))) {
    a->action = 1;
    islandActionArrival(a);
    a->shape_angle.x = 0;
    a->current.angle.x = 0;
    a->shape_angle.z = 0;
    a->current.angle.z = 0;
  } else {
    a->current.angle.z = 0;
    a->shape_angle.x = 0;
    a->current.angle.x = 0;
    a->action = 0;
    a->shape_angle.z = 0;
  }
  return 4;
}
VERIFY(0x024AB780, islandCreateBody);
s32 islandCreate(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB890, s32, a);
  return islandCreateBody(a);
}
VERIFY(0x024AB890, islandCreate);
void islandStaticInit() {
  WWHD_FUNC(0x024AB894, void);
  gabi::store<u32>(0x1046E364, 0);
  gabi::store<u32>(0x1046E35C, 0);
  gabi::store<u32>(0x1046E368, 0);
  gabi::store<u32>(0x1046E360, 0);
  islandCall<void>(0x028F026C, 0x101D1B10);
  gabi::store<f32>(0x1046E350, -3.1415927410125732f);
  gabi::store<f32>(0x1046E354, 3.1415927410125732f);
  islandCall<void>(0x028ED6F8, 0x1046E358);
  islandCall<void>(0x028F026C, 0x101D1B1C);
  islandCall<void>(0x028EAB2C, 0x1046E359);
  islandCall<void>(0x028F026C, 0x101D1B28);
}
VERIFY(0x024AB894, islandStaticInit);
BOOL islandDraw(daTag_Island_c *a) {
  WWHD_FUNC(0x024AB928, BOOL, a);
  return TRUE;
}
VERIFY(0x024AB928, islandDraw);
void islandDestructor(daTag_Island_c *a, s32 flags) {
  WWHD_FUNC(0x024AB930, void, a, flags);
  if (a) {
    islandCall<void>(0x025D50BC, a, 0);
    if (flags & 1)
      islandCall<void>(0x0273AF40, a);
  }
}
VERIFY(0x024AB930, islandDestructor);

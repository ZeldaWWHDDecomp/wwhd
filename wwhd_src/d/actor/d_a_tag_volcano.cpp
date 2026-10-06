/* HD Volcano tag reconstruction derived from zeldaret/tww. */
#include "d/actor/d_a_tag_volcano.h"
namespace daTagvolcano {
u32 prmAbstract(Act_c *actor, u32 width, u32 shift) {
  WWHD_FUNC(0x024B3970, u32, actor, width, shift);
  u32 parameters = gabi::load<u32>(gabi::ea(actor) + 0xB0);
  width &= 63;
  shift &= 63;
  u32 mask = (width < 32 ? 1u << width : 0u) - 1u;
  return (shift < 32 ? parameters >> shift : 0u) & mask;
}
VERIFY(0x024B3970, prmAbstract);

static u32 play() { return gabi::call<u32>(0x025200D4); }
static s32 room(Act_c *a) { return gabi::load<s8>(gabi::ea(a) + 0x2FE); }
static void offSwitch(Act_c *a) {
  u32 sw = prmAbstract(a, 8, 8), save = gabi::load<u32>(0x101F84DC);
  gabi::call(0x025B9F7C, gabi::at<u8>(save + 0x20), sw, room(a));
}
s32 Act_c::Create() {
  WWHD_FUNC(0x024B2DC0, s32, this);
  u32 base = gabi::ea(this), flags = gabi::load<u32>(base + 0x2E4);
  if (!(flags & 8)) {
    if (this) {
      gabi::call(0x025D4ED0, this);
      gabi::store<u32>(base + 0xB4, 0x1003FCB8);
      flags = gabi::load<u32>(base + 0x2E4);
    }
    gabi::store<u32>(base + 0x2E4, flags | 8);
  }
  mTimerCreated = 0;
  mTimerStarted = 0;
  u32 type = prmAbstract(this, 2, 6);
  mCountdown = 0;
  mType = (s32)type;
  if (type == 0) {
    s32 currentRoom = gabi::load<s8>(base + 0x326);
    u32 save = gabi::load<u32>(0x101F84DC);
    BOOL complete = gabi::call<s32>(0x025B8B94, gabi::at<u8>(save + 0x644),
                                    currentRoom == 20 ? 0x1902 : 0x1901);
    if (complete) {
      u32 sw = prmAbstract(this, 8, 8);
      save = gabi::load<u32>(0x101F84DC);
      gabi::call(0x025B9E38, gabi::at<u8>(save + 0x20), sw, room(this));
    } else {
      save = gabi::load<u32>(0x101F84DC);
      if (gabi::load<s16>(save + 0x115C) == 2 &&
          gabi::load<s8>(base + 0x326) == gabi::load<s8>(save + 0x1148)) {
        mCountdown = 10;
        u32 sw = prmAbstract(this, 8, 8);
        save = gabi::load<u32>(0x101F84DC);
        gabi::call(0x025B9E38, gabi::at<u8>(save + 0x20), sw, room(this));
      } else {
        u32 sw = prmAbstract(this, 8, 8);
        gabi::call(0x025B9F7C, gabi::at<u8>(save + 0x20), sw, room(this));
      }
    }
  }
  mEventPending = 0;
  return 4;
}
VERIFY(0x024B2DC0, &Act_c::Create);
BOOL Act_c::check_timer_clear() {
  WWHD_FUNC(0x024B3028, BOOL, this);
  if (!gabi::load<s8>(play() + 0x514C))
    return TRUE;
  for (u32 name : {0x1003FCC8u, 0x1003FCD0u}) {
    gabi::Local<SafeString> a, b;
    a->mStringTop = name;
    a->__vtbl = 0x1003FCA0;
    b->mStringTop = play() + 0x5140;
    b->__vtbl = 0x1003FCA0;
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.get());
    u32 left = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.get());
    u32 right = b->mStringTop;
    if (left == right)
      return FALSE;
    for (u32 i = 0; i < 0x40001; ++i) {
      u8 c = gabi::load<u8>(left + i);
      if (c != gabi::load<u8>(right + i))
        break;
      if (!c)
        return FALSE;
    }
  }
  return TRUE;
}
VERIFY(0x024B3028, &Act_c::check_timer_clear);
static void deleteTimer() {
  if (gabi::load<s32>(play() + 0x5CFC) == 3) {
    u32 timer = gabi::load<u32>(play() + 0x5CF0);
    if (timer)
      gabi::call(0x025C58D8, gabi::at<u8>(timer));
  }
}
BOOL Act_c::Delete() {
  WWHD_FUNC(0x024B31F8, BOOL, this);
  if (check_timer_clear())
    deleteTimer();
  offSwitch(this);
  return TRUE;
}
VERIFY(0x024B31F8, &Act_c::Delete);
static s32 restTime() {
  u32 end = gabi::load<u32>(play() + 0x5CF8);
  u32 start = gabi::load<u32>(play() + 0x5CF4);
  return (s32)((end - start) * 1000u) / 30;
}
static void expire() {
  gabi::call(0x025E1988, 0x8B2);
  u32 p = play();
  gabi::Local<cXyz> direction;
  direction->x = 0.0f;
  direction->y = 1.0f;
  direction->z = 0.0f;
  gabi::call(0x025CB374, gabi::at<u8>(p + 0x599C), 6, -33, direction.get());
}
BOOL Act_c::Execute() {
  WWHD_FUNC(0x024B3280, BOOL, this);
  if (mType == 0) {
    u32 sw = prmAbstract(this, 8, 8), save = gabi::load<u32>(0x101F84DC);
    if (!gabi::call<s32>(0x025BA0C0, gabi::at<u8>(save + 0x20), sw, room(this)))
      return TRUE;
    s32 delay = mCountdown;
    if (delay > 0) {
      delay = (s32)((u32)delay - 1u);
      mCountdown = delay;
      if (delay == 0)
        offSwitch(this);
      return TRUE;
    }
    if (mTimerCreated) {
      if (!gabi::load<u32>(play() + 0x5CF0))
        return TRUE;
      u8 started = mTimerStarted;
      u32 p = play();
      if (!started) {
        if (gabi::load<s32>(p + 0x5CFC) == 3) {
          u32 timer = gabi::load<u32>(play() + 0x5CF0);
          if (timer)
            gabi::call(0x025C5864, gabi::at<u8>(timer));
        }
        mTimerStarted = 1;
        return TRUE;
      }
      u8 running = gabi::load<u8>(p + 0x5292);
      if (gabi::load<s32>(play() + 0x5CFC) == 3) {
        u32 timer = gabi::load<u32>(play() + 0x5CF0);
        if (timer)
          gabi::call_ptr(running ? 0x025C5794 : 0x025C57D4, gabi::at<u8>(timer),
                         2);
      }
      if (restTime() <= 0) {
        expire();
        offSwitch(this);
        deleteTimer();
        mTimerCreated = 0;
        mTimerStarted = 0;
      }
    } else {
      u16 duration = (u16)(prmAbstract(this, 8, 16) * 10u);
      s32 currentRoom = gabi::load<s8>(gabi::ea(this) + 0x326);
      u32 p = play();
      if (gabi::call<s32>(0x0254457C, gabi::at<u8>(p + 0x52C4),
                          STR(currentRoom == 20 ? 0x1003FCFC : 0x1003FD0C))) {
        gabi::call(0x025C60F4, 3, duration, 1, 0, 221.0f, 439.0f, 32.0f,
                   419.0f);
        mTimerCreated = 1;
      }
    }
  } else {
    u32 bit = prmAbstract(this, 6, 0), save = gabi::load<u32>(0x101F84DC);
    BOOL chest = gabi::call<s32>(0x025B8C74, gabi::at<u8>(save + 0x798), bit);
    u32 timer = gabi::load<u32>(play() + 0x5CF0);
    if (chest) {
      if (timer)
        deleteTimer();
      s32 type = mType;
      save = gabi::load<u32>(0x101F84DC);
      gabi::call(0x025B8B68, gabi::at<u8>(save + 0x644),
                 type == 1 ? 0x1902 : 0x1901);
      mEventPending = 0;
      return TRUE;
    }
    if (timer) {
      u8 running = gabi::load<u8>(play() + 0x5292);
      if (gabi::load<s32>(play() + 0x5CFC) == 3) {
        timer = gabi::load<u32>(play() + 0x5CF0);
        if (timer)
          gabi::call_ptr(running ? 0x025C5794 : 0x025C57D4, gabi::at<u8>(timer),
                         2);
      }
      if (restTime() <= 0) {
        expire();
        deleteTimer();
        mEventPending = 1;
        gabi::call(0x025D77DC, this, STR(0x1003FCF0), 1, 0xFFFF);
      }
    } else if (!mEventPending) {
      expire();
      mEventPending = 1;
      gabi::call(0x025D77DC, this, STR(0x1003FCF0), 1, 0xFFFF);
    }
    if (mEventPending) {
      if (gabi::load<u16>(gabi::ea(this) + 0xF8) == 2) {
        s32 type = mType;
        gabi::call(0x0252012C, STR(0x1003FCEC), 2, type == 1 ? 20 : 40, -1, 0,
                   1, 0, 0.0f);
      } else
        gabi::call(0x025D77DC, this, STR(0x1003FCF0), 1, 0xFFFF);
    }
  }
  return TRUE;
}
VERIFY(0x024B3280, &Act_c::Execute);
s32 wrapperCreate(Act_c *actor) {
  WWHD_FUNC(0x024B3854, s32, actor);
  return actor->Create();
}
VERIFY(0x024B3854, wrapperCreate);
BOOL Delete(Act_c *actor) {
  WWHD_FUNC(0x024B3858, BOOL, actor);
  return actor->Delete();
}
VERIFY(0x024B3858, Delete);
BOOL wrapperExecute(Act_c *actor) {
  WWHD_FUNC(0x024B385C, BOOL, actor);
  return actor->Execute();
}
VERIFY(0x024B385C, wrapperExecute);
void staticInitialize() {
  WWHD_FUNC(0x024B3868, void);
  gabi::store<u32>(0x1046E690, 0);
  gabi::store<u32>(0x1046E688, 0);
  gabi::store<u32>(0x1046E694, 0);
  gabi::store<u32>(0x1046E68C, 0);
  gabi::call(0x028F026C, gabi::at<u8>(0x101D2184));
  gabi::store<f32>(0x1046E67C, -3.1415927410125732f);
  gabi::store<f32>(0x1046E680, 3.1415927410125732f);
  gabi::call(0x028ED6F8, gabi::at<u8>(0x1046E684));
  gabi::call(0x028F026C, gabi::at<u8>(0x101D2190));
  gabi::call(0x028EAB2C, gabi::at<u8>(0x1046E685));
  gabi::call(0x028F026C, gabi::at<u8>(0x101D219C));
}
VERIFY(0x024B3868, staticInitialize);
void destruct(Act_c *actor, s32 flags) {
  WWHD_FUNC(0x024B3914, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x024B3914, destruct);
BOOL Draw(Act_c *actor) {
  WWHD_FUNC(0x024B3860, BOOL, actor);
  return TRUE;
}
VERIFY(0x024B3860, Draw);
BOOL IsDelete(Act_c *actor) {
  WWHD_FUNC(0x024B3968, BOOL, actor);
  return TRUE;
}
VERIFY(0x024B3968, IsDelete);
void deleteStatic(void *p, s32 flags) {
  WWHD_FUNC(0x024B38FC, void, p, flags);
  if (p && (flags & 1))
    gabi::call(0x0273AF40, p);
}
VERIFY(0x024B38FC, deleteStatic);
void emptyVirtual() { WWHD_FUNC(0x024B3910, void); }
VERIFY(0x024B3910, emptyVirtual);
} // namespace daTagvolcano

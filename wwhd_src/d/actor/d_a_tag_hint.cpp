/* Local WWHD hint trigger reconstruction. */
#include "d/actor/d_a_tag_hint.h"
u8 daTag_Hint_c::getSwbit() {
  WWHD_FUNC(0x024A8F70, u8, this);
  return ((u32)mParameters >> 8) & 255;
}
VERIFY(0x024A8F70, &daTag_Hint_c::getSwbit);
u8 daTag_Hint_c::getSwbit2() {
  WWHD_FUNC(0x024A8F7C, u8, this);
  return ((u32)mParameters >> 16) & 255;
}
VERIFY(0x024A8F7C, &daTag_Hint_c::getSwbit2);
u16 daTag_Hint_c::getEventFlag() {
  WWHD_FUNC(0x024A8F88, u16, this);
  return (u16)home.angle.z;
}
VERIFY(0x024A8F88, &daTag_Hint_c::getEventFlag);
u8 daTag_Hint_c::getType() {
  WWHD_FUNC(0x024A8F90, u8, this);
  return (u32)mParameters & 63;
}
VERIFY(0x024A8F90, &daTag_Hint_c::getType);
u8 daTag_Hint_c::getEventNo() {
  WWHD_FUNC(0x024A9494, u8, this);
  return (u32)mParameters >> 24;
}
VERIFY(0x024A9494, &daTag_Hint_c::getEventNo);
u8 daTag_Hint_c::getType2() {
  WWHD_FUNC(0x024A95B8, u8, this);
  return ((u32)mParameters >> 6) & 3;
}
VERIFY(0x024A95B8, &daTag_Hint_c::getType2);
s32 daTag_Hint_c::rangeCheck_local(cXyz *pos) {
  WWHD_FUNC(0x024A8BC0, s32, this, pos);
  gabi::Local<cXyz> diff, flat;
  gabi::call(0x0201ADE0, pos, diff.get(), &current.pos);
  f32 y = diff->y, x = diff->x, z = diff->z;
  if (y < 0.0f)
    y = -y;
  flat->z = z;
  flat->x = x;
  flat->y = 0.0f;
  f32 mag = gabi::call<f32>(0x028E8DD0, flat.get());
  if (!(mag < m2A8))
    return 0;
  if (y > m2AC)
    return 0;
  return 1;
}
VERIFY(0x024A8BC0, &daTag_Hint_c::rangeCheck_local);
static fopAc_ac_c *findObjectCallBack(fopAc_ac_c *object, daTag_Hint_c *hint) {
  WWHD_FUNC(0x024A8C84, fopAc_ac_c *, object, hint);
  if (!object || gabi::load<s16>(gabi::ea(object) + 14) != 0x2B)
    return nullptr;
  if (!hint->rangeCheck_local(gabi::at<cXyz>(gabi::ea(object) + 0x2EC)))
    return nullptr;
  gabi::Local<cXyz> diff, flat;
  gabi::call(0x0201ADE0, &object->current.pos, diff.get(),
             gabi::at<cXyz>(gabi::ea(object) + 0x2EC));
  f32 x = diff->x, z = diff->z;
  flat->y = 0.0f;
  flat->x = x;
  flat->z = z;
  f32 mag = gabi::call<f32>(0x028E8DD0, flat.get());
  return mag > 0.0f ? object : nullptr;
}
VERIFY(0x024A8C84, findObjectCallBack);
s32 daTag_Hint_c::moveBoxCheck() {
  WWHD_FUNC(0x024A8F9C, s32, this);
  return gabi::call<u32>(0x025D5218, 0x024A8C84u, this) != 0;
}
VERIFY(0x024A8F9C, &daTag_Hint_c::moveBoxCheck);
s32 daTag_Hint_c::rangeCheck() {
  WWHD_FUNC(0x024A9930, s32, this);
  u32 play = gabi::call<u32>(0x025200D4);
  u32 player = gabi::load<u32>(play + 0x5B34);
  return rangeCheck_local(gabi::at<cXyz>(player + 0x314));
}
VERIFY(0x024A9930, &daTag_Hint_c::rangeCheck);
s32 daTag_Hint_c::actionLight() {
  WWHD_FUNC(0x024A8F08, s32, this);
  f32 v = m2B0;
  if (!(v < 0.95f)) {
    mCurProc = 0;
    m2B0 = 1.0f;
  } else
    m2B0 = v + 0.05f;
  darkProc();
  return 1;
}
VERIFY(0x024A8F08, &daTag_Hint_c::actionLight);
void daTag_Hint_c::darkProc() {
  WWHD_FUNC(0x024A8D2C, void, this);
  struct String {
    be<u32> text;
    be<u32> vt;
  };
  gabi::Local<String> name, stage;
  name->text = 0x1003F300;
  name->vt = 0x1003F210;
  u32 play = gabi::call<u32>(0x025200D4);
  stage->text = play + 0x5134;
  stage->vt = 0x1003F210;
  auto prepare = [](String *s) {
    u32 vt = s->vt;
    gabi::call(gabi::load<u32>(vt + 0x14), s);
  };
  prepare(name.get());
  prepare(name.get());
  u32 a = name->text;
  prepare(stage.get());
  u32 b = stage->text;
  if (a == b)
    return;
  a = name->text;
  b = stage->text;
  for (u32 i = 0; i < 0x40001; ++i) {
    u8 av = gabi::load<u8>(a + i), bv = gabi::load<u8>(b + i);
    if (av != bv)
      break;
    if (av == 0)
      return;
  }
  gabi::call(0x0255A51C, (f32)m2B0);
  gabi::call(0x0255A458, (f32)m2B0);
  f32 t = ((f32)m2B0 - 0.15f) * 1.1764705181121826f;
  if (t > 0.5f) {
    gabi::call(0x0255671C, 300.0f, 2000.0f, 2.0f - (t + t));
  } else {
    f32 v = t * 0.85f;
    gabi::call(0x0255A418, (v + v) + 0.15f);
    gabi::call(0x0255671C, 300.0f, gabi::fmadds(3000.0f, t, 500.0f), 1.0f);
  }
}
VERIFY(0x024A8D2C, &daTag_Hint_c::darkProc);
static u32 save() { return gabi::load<u32>(0x101F84DC); }
static s32 eventBit(u16 bit, u32 off = 0x644) {
  return gabi::call<s32>(0x025B8B94, save() + off, bit);
}
static s32 isSwitch(daTag_Hint_c *a, s32 sw) {
  u32 s = save();
  s32 room = a->current.roomNo;
  return gabi::call<s32>(0x025BA0C0, s + 0x20, sw, room);
}
s32 daTag_Hint_c::waitTerms() {
  WWHD_FUNC(0x024A8FD0, s32, this);
  s32 sw = getSwbit(), sw2 = getSwbit2();
  u16 ev = getEventFlag();
  u8 type = getType();
  if (sw != 255 && isSwitch(this, sw))
    return 1;
  if ((type == 0 || (type >= 3 && type <= 5)) && ev != 65535 && eventBit(ev))
    return 1;
  switch (type) {
  case 1:
    if (moveBoxCheck()) {
      if (sw != 255) {
        u32 s = save();
        s32 room = current.roomNo;
        gabi::call(0x025B9E38, s + 0x20, sw, room);
      }
      gabi::call(0x025B8B68, save() + 0x644, 0x404);
      return 1;
    }
    break;
  case 6:
    return eventBit(0x2A10) != 0;
  case 7:
    return sw2 != 255 && !isSwitch(this, sw2);
  case 9:
    if (eventBit(0x304, 0x1178))
      return 1;
    return eventBit(0x1801) != 0;
  case 8:
  case 10:
  case 16:
    return sw2 != 255 && isSwitch(this, sw2);
  case 11:
    return gabi::load<u8>(save() + 0x68) != 255;
  case 13:
    return gabi::call<s32>(0x025B9100, save() + 0x798, 3) != 0;
  case 14:
    return eventBit(0x3C01) != 0;
  case 15:
    return eventBit(0x3F20) != 0;
  case 17:
    return gabi::call<s32>(0x02520C0C, 0x28) != 0;
  }
  return 0;
}
VERIFY(0x024A8FD0, &daTag_Hint_c::waitTerms);
s32 daTag_Hint_c::arrivalTerms() {
  WWHD_FUNC(0x024A9284, s32, this);
  s32 sw = getSwbit2();
  u16 ev = getEventFlag();
  u8 type = getType();
  if (type != 8 && type != 10 && type != 16 && sw != 255 && !isSwitch(this, sw))
    return 0;
  if ((type == 2 || type == 8 || type == 10) && ev != 65535 && !eventBit(ev))
    return 0;
  if (type == 17 && !eventBit(0x4020))
    return 0;
  switch (type) {
  case 1:
    m2A0 = 600;
    break;
  case 4:
  case 8:
    m2A0 = 3600;
    break;
  case 9:
    if (!eventBit(0x308, 0x1178))
      return 0;
    m2A0 = 600;
    break;
  case 11:
    if (gabi::load<u8>(0x101D5F4C) != 1)
      return 0;
    {
      s8 room = current.roomNo;
      s8 stay = gabi::load<s8>(0x1047E6C8);
      if (stay != room)
        return 0;
    }
    break;
  case 12:
  case 14:
    m2A0 = 1800;
    break;
  case 15:
    m2A0 = 900;
    break;
  default:
    m2A0 = 0;
    break;
  }
  return 1;
}
VERIFY(0x024A9284, &daTag_Hint_c::arrivalTerms);
u16 daTag_Hint_c::getMessage() {
  WWHD_FUNC(0x024A9928, u16, this);
  return (u16)home.angle.x;
}
VERIFY(0x024A9928, &daTag_Hint_c::getMessage);
u32 daTag_Hint_c::getMsg() {
  WWHD_FUNC(0x024A9E58, u32, this);
  return mMsgNo;
}
VERIFY(0x024A9E58, &daTag_Hint_c::getMsg);
void daTag_Hint_c::talkInit() {
  WWHD_FUNC(0x024A9CF8, void, this);
  m294 = 0;
}
VERIFY(0x024A9CF8, &daTag_Hint_c::talkInit);
s32 daTag_Hint_c::getPriority() {
  WWHD_FUNC(0x024A9B44, s32, this);
  return getType() == 2 ? 490 : 511;
}
VERIFY(0x024A9B44, &daTag_Hint_c::getPriority);
s32 daTag_Hint_c::otherCheck() {
  WWHD_FUNC(0x024A996C, s32, this);
  u32 play = gabi::call<u32>(0x025200D4);
  u32 player = gabi::load<u32>(play + 0x5B34);
  u8 type = getType();
  if (type == 4 || type == 8 || type == 9 || type == 12) {
    s16 timer = m2A0;
    if (timer > 0) {
      m2A0 = timer - 1;
      return 0;
    }
  }
  if (!rangeCheck())
    return 0;
  if (type == 0) {
    u32 vt = gabi::load<u32>(player + 0xB4);
    if (gabi::call<s32>(gabi::load<u32>(vt + 0xC4), player))
      return 0;
    f32 v = gabi::load<f32>(player + 0x3CC);
    if (v < 0.0f)
      return 0;
  } else if (type == 1) {
    gabi::call(0x025B8B68, save() + 0x644, 0x404);
    if (!eventBit(0x401))
      return 0;
  } else if (type == 6) {
    if (gabi::call<u32>(0x025B8BB0, save() + 0x644, 0xA507) < 3)
      return 0;
  } else if (type == 14 || type == 15) {
    play = gabi::call<u32>(0x025200D4);
    if (gabi::load<u8>(play + 0x5292))
      return 0;
  }
  if (m2A0 <= 0)
    return 1;
  u32 control = gabi::load<u32>(0x101F4B5C);
  if (!gabi::call<u32>(0x025F795C, control))
    m2A0 = (s16)m2A0 - 1;
  return 0;
}
VERIFY(0x024A996C, &daTag_Hint_c::otherCheck);
void daTag_Hint_c::makeEventId() {
  WWHD_FUNC(0x024A94A0, void, this);
  u32 play = gabi::call<u32>(0x025200D4), ship = gabi::load<u32>(play + 0x5B2C);
  u32 vt = gabi::load<u32>(ship + 0xB4);
  s32 id = gabi::call<s32>(gabi::load<u32>(vt + 0xBC), ship);
  u8 type = getType();
  u32 name;
  u8 special = 0;
  if (type == 10)
    name = 0x1003F310;
  else {
    gabi::Local<be<s32>> search;
    search.get()->operator=(id);
    if (id != -1 && gabi::call<u32>(0x025D5218, 0x025E1234u, search.get())) {
      name = 0x1003F31C;
      special = 1;
    } else
      name = 0x1003F32C;
  }
  u8 event = getEventNo();
  play = gabi::call<u32>(0x025200D4);
  s32 idx = gabi::call<s32>(0x02543F10, play + 0x52C4, name, event);
  m2A4 = idx;
  m2A3 = special;
}
VERIFY(0x024A94A0, &daTag_Hint_c::makeEventId);
void daTag_Hint_c::startProc() {
  WWHD_FUNC(0x024A97E4, void, this);
  s32 sw = getSwbit();
  u8 type = getType();
  if (type == 0)
    gabi::call(0x025B8B68, save() + 0x644, 0x401);
  else if (type != 6 && type != 12 && sw != 255) {
    u32 s = save();
    s32 room = current.roomNo;
    gabi::call(0x025B9E38, s + 0x20, sw, room);
  }
  u32 play = gabi::call<u32>(0x025200D4);
  s32 staff = gabi::call<s32>(0x02542D88, play + 0x52C4, 0x1003F340u, 0, 0);
  m29C = staff;
  m2B0 = 1.0f;
  if (type != 10 && !(getType2() & 1)) {
    gabi::call(0x025E1988, 0x853);
    setPlayerAngle();
  }
  u16 flag = getEventFlag();
  if (type == 5)
    gabi::call(0x025B8B68, save() + 0x644, flag);
}
VERIFY(0x024A97E4, &daTag_Hint_c::startProc);
s32 daTag_Hint_c::actionHunt() {
  WWHD_FUNC(0x024A9B78, s32, this);
  u32 ea = gabi::ea(this);
  if (gabi::load<u16>(ea + 0xF8) == 1) {
    mCurProc = 4;
    startProc();
    mMsgNo = getMessage();
    return 1;
  }
  if (waitTerms()) {
    mCurProc = 0;
    return 1;
  }
  if (otherCheck()) {
    makeEventId();
    gabi::store<u16>(ea + 0xFC, (s16)m2A4);
    u8 event = getEventNo();
    gabi::store<u8>(ea + 0xFE, event);
    if (getType2() & 1)
      gabi::call(0x025D76A8, this);
    else {
      s32 priority = getPriority();
      u32 play = gabi::call<u32>(0x025200D4);
      gabi::call(0x024EBA68, play + 0x5928, this, priority);
    }
    u16 flags = gabi::load<u16>(ea + 0xFA);
    gabi::store<u16>(ea + 0xFA, flags | 1);
  }
  return 1;
}
VERIFY(0x024A9B78, &daTag_Hint_c::actionHunt);
s32 daTag_Hint_c::actionArrival() {
  WWHD_FUNC(0x024A9C74, s32, this);
  if (waitTerms()) {
    mCurProc = 0;
    return 1;
  }
  if (arrivalTerms()) {
    makeEventId();
    u32 ea = gabi::ea(this);
    gabi::store<u16>(ea + 0xFC, (s16)m2A4);
    u8 event = getEventNo();
    gabi::store<u8>(ea + 0xFE, event);
    mCurProc = 3;
    actionHunt();
  }
  return 1;
}
VERIFY(0x024A9C74, &daTag_Hint_c::actionArrival);
void daTag_Hint_c::initLight() {
  WWHD_FUNC(0x024A9D04, void, this);
  gabi::store<f32>(0x1046E31C, 93.80000305175781f);
  gabi::store<f32>(0x1046E318, 104.34782409667969f);
  gabi::store<u16>(0x1046E310, 0);
  gabi::store<u16>(0x1046E334, 0);
  gabi::store<f32>(0x1046E33C, 60.0f);
  gabi::store<u16>(0x1046E336, 47);
  gabi::store<u16>(0x1046E338, 233);
  gabi::store<u16>(0x1046E312, 351);
  gabi::store<f32>(0x1046E340, 93.80000305175781f);
  gabi::store<u16>(0x1046E314, 510);
}
VERIFY(0x024A9D04, &daTag_Hint_c::initLight);
void daTag_Hint_c::setLightPos() {
  WWHD_FUNC(0x024A9D6C, void, this);
  u32 play = gabi::call<u32>(0x025200D4),
      player = gabi::load<u32>(play + 0x5B34);
  f32 y = gabi::load<f32>(player + 0x400), x = gabi::load<f32>(player + 0x3FC),
      z = gabi::load<f32>(player + 0x404);
  gabi::store<f32>(0x1046E304, x);
  f32 height = y + 5.66f;
  gabi::store<f32>(0x1046E30C, z);
  gabi::store<f32>(0x1046E308, height);
  u32 angle = gabi::load<u16>(player + 0x32A);
  u32 table = 0x104A44F8 + ((angle >> 3) << 3);
  f32 sin = gabi::load<f32>(table), cos = gabi::load<f32>(table + 4);
  f32 offset = sin * 11.13f;
  f32 lx = x + gabi::fmadds(cos, 10.25f, offset);
  gabi::store<f32>(0x1046E304, lx);
  angle = gabi::load<u16>(player + 0x32A);
  table = 0x104A44F8 + ((angle >> 3) << 3);
  sin = gabi::load<f32>(table);
  offset = sin * 10.25f;
  cos = gabi::load<f32>(table + 4);
  f32 oz = gabi::fmsubs(cos, 11.13f, offset);
  gabi::store<f32>(0x1046E328, lx);
  f32 lz = z + oz;
  gabi::store<f32>(0x1046E32C, height);
  gabi::store<f32>(0x1046E30C, lz);
  gabi::store<f32>(0x1046E330, lz);
}
VERIFY(0x024A9D6C, &daTag_Hint_c::setLightPos);
void daTag_Hint_c::makeLight() {
  WWHD_FUNC(0x024A9E28, void, this);
  initLight();
  gabi::call(0x024A9D6C);
  gabi::call(0x0255A2B8, 0x1046E304u);
}
VERIFY(0x024A9E28, &daTag_Hint_c::makeLight);
void daTag_Hint_c::deleteLight() {
  WWHD_FUNC(0x024A9FA0, void, this);
  gabi::call(0x0255A374, 0x1046E304u);
}
VERIFY(0x024A9FA0, &daTag_Hint_c::deleteLight);
u16 daTag_Hint_c::next_msgStatus(u32 *msg) {
  WWHD_FUNC(0x024A9E60, u16, this, msg);
  return 16;
}
VERIFY(0x024A9E60, &daTag_Hint_c::next_msgStatus);
u16 daTag_Hint_c::talk() {
  WWHD_FUNC(0x024A9E68, u16, this);
  s8 state = m294;
  u32 control = gabi::load<u32>(0x101F4B5C);
  u16 status = 255;
  if (state == 0) {
    gabi::store<u32>(0x1046E2E4, 0xFFFFFFFF);
    m290 = getMsg();
    m294 = 1;
  } else if (state != -1) {
    s32 id = gabi::load<s32>(0x1046E2E4);
    if (id == -1) {
      u32 msg = m290;
      u32 res = gabi::call<u32>(0x025F7DB0, control, msg,
                                gabi::at<cXyz>(gabi::ea(this) + 0x37C));
      gabi::store<u32>(0x1046E2E4, res);
    } else if (state == 1)
      m294 = 2;
    else if (state == 2) {
      status = (u16)gabi::call<u32>(0x025F795C, control);
      if (status == 14) {
        u16 next = next_msgStatus(reinterpret_cast<u32 *>(&m290));
        gabi::call(0x025F74D0, control, next);
        if (gabi::call<u32>(0x025F795C, control) == 15)
          gabi::call(0x025F7DB0, control, (u32)m290, 0);
      } else if (status == 18) {
        gabi::call(0x025F74D0, control, 19);
        m294 = -1;
      }
    }
  }
  return status;
}
VERIFY(0x024A9E68, &daTag_Hint_c::talk);
s32 daTag_Hint_c::actionEvent() {
  WWHD_FUNC(0x024A9FAC, s32, this);
  s32 staff = m29C;
  u32 play = gabi::call<u32>(0x025200D4);
  s32 action =
      gabi::call<s32>(0x02542EDC, play + 0x52C4, staff, 0x101D1A74u, 5, 0, 0);
  staff = m29C;
  play = gabi::call<u32>(0x025200D4);
  if (gabi::call<s32>(0x025447C8, play + 0x52C4, staff)) {
    m2A6 = (u16)m2A6 | 1;
    switch (action) {
    case 1:
      talkInit();
      gabi::call(0x024A9E28);
      gabi::call(0x025E1988, 0x854);
      break;
    case 2:
      talkInit();
      gabi::call(0x025E1988, 0x853);
      gabi::call(0x025E1988, 0x854);
      break;
    case 3:
      gabi::call(0x025E1988, 0x852);
      play = gabi::call<u32>(0x025200D4);
      {
        u32 p = gabi::load<u32>(play + 0x5B34), vt = gabi::load<u32>(p + 0xB4);
        gabi::call(gabi::load<u32>(vt + 0xE4), p, 28);
      }
      break;
    case 4:
      talkInit();
      gabi::call(0x025E1988, 0x854);
      break;
    }
  }
  auto endCut = [&]() {
    s32 id = m29C;
    u32 p = gabi::call<u32>(0x025200D4);
    gabi::call(0x02543280, p + 0x52C4, id);
  };
  auto resetEvent = []() {
    u32 p = gabi::call<u32>(0x025200D4);
    u16 flags = gabi::load<u16>(p + 0x52B8);
    gabi::store<u16>(p + 0x52B8, flags | 8);
  };
  if (action == 1) {
    if (talk() == 18) {
      gabi::call(0x025E1988, 0x855);
      endCut();
      mCurProc = 1;
      deleteLight();
      resetEvent();
    }
    f32 v = m2B0;
    if (v > 0.15f) {
      if (v > 0.2f) {
        m2B0 = v - 0.05f;
        darkProc();
        return 1;
      }
      m2B0 = 0.15f;
    }
    darkProc();
  } else if (action == 2) {
    u16 status = talk();
    u16 flags = m2A6;
    if (flags & 1) {
      if (status == 18) {
        gabi::call(0x025E1988, 0x855);
        s32 id = m29C;
        mMsgNo = 0x454;
        u32 p = gabi::call<u32>(0x025200D4);
        gabi::call(0x02543280, p + 0x52C4, id);
      }
    } else
      endCut();
  } else if (action == 4) {
    if (talk() == 18) {
      mCurProc = 0;
      resetEvent();
    }
  } else
    endCut();
  return 1;
}
VERIFY(0x024A9FAC, &daTag_Hint_c::actionEvent);
static s32 daTag_Hint_Execute(daTag_Hint_c *a) {
  WWHD_FUNC(0x024AA254, s32, a);
  switch ((u8)a->mCurProc) {
  case 1:
    a->actionLight();
    break;
  case 2:
    a->actionArrival();
    break;
  case 3:
    a->actionHunt();
    break;
  case 4:
    a->actionEvent();
    break;
  }
  return 1;
}
VERIFY(0x024AA254, daTag_Hint_Execute);
static s32 daTag_Hint_IsDelete(daTag_Hint_c *a) {
  WWHD_FUNC(0x024AA2C8, s32, a);
  return 1;
}
VERIFY(0x024AA2C8, daTag_Hint_IsDelete);
static s32 daTag_Hint_Draw(daTag_Hint_c *a) {
  WWHD_FUNC(0x024AA52C, s32, a);
  return 1;
}
VERIFY(0x024AA52C, daTag_Hint_Draw);
static s32 daTag_Hint_Delete(daTag_Hint_c *a) {
  WWHD_FUNC(0x024AA2D0, s32, a);
  u8 type = a->getType();
  if (type == 9)
    gabi::call(0x025B8B7C, save() + 0x1178, 0x308);
  else if (type == 17)
    gabi::call(0x025B8B7C, save() + 0x644, 0x4020);
  return 1;
}
VERIFY(0x024AA2D0, daTag_Hint_Delete);
void daTag_Hint_c::setPlayerAngle() {
  WWHD_FUNC(0x024A95C4, void, this);
  u32 play = gabi::call<u32>(0x025200D4),
      player = gabi::load<u32>(play + 0x5B34);
  struct Check {
    u8 data[0x70];
  };
  gabi::Local<Check> check;
  u32 c = gabi::ea(check.get());
  gabi::call(0x02008FEC, check.get());
  u32 a = c + 0x58, b = c + 0x64;
  gabi::store<u32>(c, a);
  gabi::store<u32>(b, 0x1003F2A8);
  gabi::store<u32>(b + 4, 1);
  for (u32 o : {0x5Eu, 0x5Fu, 0x60u, 0x62u})
    gabi::store<u8>(c + o, 0);
  gabi::store<u32>(c + 4, b);
  gabi::store<u8>(c + 0x5D, 1);
  gabi::store<u32>(c + 0x20, 0x1003F298);
  gabi::store<u32>(c + 0x10, 0x1003F288);
  gabi::store<u32>(a, 0x1003F2B8);
  gabi::store<u8>(c + 0x61, 0);
  gabi::store<u8>(c + 0x5C, 0);
  gabi::Local<cXyz> start, end;
  start->x = gabi::load<f32>(player + 0x390);
  start->y = gabi::load<f32>(player + 0x394);
  start->z = gabi::load<f32>(player + 0x398);
  s16 angle = 0;
  bool clear = false;
  for (u32 i = 0; i < 8; ++i) {
    u8 special = m2A3;
    s16 heading = gabi::load<s16>(player + 0x32A);
    s16 offset = gabi::load<s16>((special ? 0x101D1A64 : 0x101D1A54) + 2 * i);
    angle = (s16)(heading + offset);
    u32 index = ((u16)angle >> 3) << 3;
    f32 x = start->x, z = start->z;
    f32 sn = gabi::load<f32>(0x104A44F8 + index),
        co = gabi::load<f32>(0x104A44FC + index);
    end->y = (f32)start->y;
    end->x = gabi::fmadds(sn, 120.0f, x);
    end->z = gabi::fmadds(co, 120.0f, z);
    gabi::call(0x024F1AFC, check.get(), start.get(), end.get(), 0);
    play = gabi::call<u32>(0x025200D4);
    if (!gabi::call<s32>(0x02008860, play + 0x12A0, check.get())) {
      clear = true;
      break;
    }
  }
  if (!clear)
    angle = gabi::load<s16>(player + 0x32A);
  gabi::store<u16>(player + 0x422, angle);
  gabi::store<u32>(a, 0x1003F278);
  gabi::store<u32>(b, 0x1003F238);
  gabi::store<u32>(c + 0x20, 0x1003F228);
  gabi::call(0x02008B4C, check.get(), 0);
}
VERIFY(0x024A95C4, &daTag_Hint_c::setPlayerAngle);
static s32 daTag_Hint_Create(daTag_Hint_c *a) {
  WWHD_FUNC(0x024AA334, s32, a);
  u32 ea = gabi::ea(a), flags = gabi::load<u32>(ea + 0x2E4);
  if (!(flags & 8)) {
    if (a) {
      gabi::call(0x025D4ED0, a);
      flags = gabi::load<u32>(ea + 0x2E4);
      gabi::store<u32>(ea + 0xB4, 0x1003F2C8);
    }
    gabi::store<u32>(ea + 0x2E4, flags | 8);
  }
  f32 x = a->scale.x, y = a->scale.y;
  a->m2A8 = (x * x) * 10000.0f;
  a->m2AC = y * 100.0f;
  u8 type = a->getType();
  if (type == 14) {
    a->m2AC = 2410.0f;
    type = a->getType();
  }
  u8 sw = a->getSwbit();
  if (type == 1 && eventBit(0x404) && sw != 255) {
    u32 s = save();
    s32 room = a->current.roomNo;
    gabi::call(0x025B9E38, s + 0x20, sw, room);
  }
  a->mCurProc = a->waitTerms() ? 0 : 2;
  for (u32 off : {0x328u, 0x324u, 0x320u, 0x3BCu, 0x32Cu})
    gabi::store<u16>(ea + off, 0);
  return 4;
}
VERIFY(0x024AA334, daTag_Hint_Create);
static void __sinit_d_a_tag_hint_cpp() {
  WWHD_FUNC(0x024AA480, void);
  for (u32 off : {8u, 0u, 12u, 4u})
    gabi::store<u32>(0x1046E2F4 + off, 0);
  gabi::call(0x028F026C, 0x101D1A88u);
  gabi::store<f32>(0x1046E2E8, -3.1415927410125732f);
  gabi::store<f32>(0x1046E2EC, 3.1415927410125732f);
  gabi::call(0x028ED6F8, 0x1046E2F0u);
  gabi::call(0x028F026C, 0x101D1A94u);
  gabi::call(0x028EAB2C, 0x1046E2F1u);
  gabi::call(0x028F026C, 0x101D1AA0u);
  gabi::store<f32>(0x1046E348, 1.0f);
  gabi::store<f32>(0x1046E324, 1.0f);
}
VERIFY(0x024AA480, __sinit_d_a_tag_hint_cpp);
static void daTag_Hint_destructor(daTag_Hint_c *actor, s32 flags) {
  WWHD_FUNC(0x024AA548, void, actor, flags);
  if (actor) {
    gabi::call(0x025D50BC, actor, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, actor);
  }
}
VERIFY(0x024AA548, daTag_Hint_destructor);

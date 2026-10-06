/** Stone sliding doors, ported from zeldaret/tww and adapted to WWHD.
 * Derived game source: */
#include "d/actor/d_a_door10.h"
static u32 play() { return gabi::ea(dComIfGp_get()); }
static u32 save() { return gabi::load<u32>(0x101F84DC) + 0x20; }
static BOOL isSwitch(u8 sw, s32 room) {
  return gabi::call<BOOL>(0x025BA0C0, save(), sw, room);
}
static bool roomStatus(u8 room) {
  play();
  return (gabi::load<u8>(0x1047E8E8 + room * 0x22C) & 1) != 0;
}
static void sound(daDoor10_c *self, u32 id) {
  s32 reverb = gabi::call<s32>(0x02520540, (s8)self->current.roomNo);
  mDoAud_seStart(id, &self->eyePos, 0, reverb);
}
static void cutEnd(daDoor10_c *self) {
  s32 staff = self->mStaffId;
  u32 p = play();
  gabi::call(0x02543280, p + 0x52C4, staff);
}
s32 daDoor10_c::chkMakeKey() {
  WWHD_FUNC(0x02125E84, s32, this);
  u8 type = getType();
  if (type == 4 || type == 5)
    return 1;
  return type == 1 ? 2 : 0;
}
VERIFY(0x02125E84, &daDoor10_c::chkMakeKey);
BOOL daDoor10_c::chkMakeStop() {
  WWHD_FUNC(0x02125ED8, BOOL, this);
  if (getSwbit2() != 255)
    return TRUE;
  return !chkMakeKey() && getSwbit() != 255;
}
VERIFY(0x02125ED8, &daDoor10_c::chkMakeStop);
const char *daDoor10_c::getBdlName() {
  WWHD_FUNC(0x02125E28, const char *, this);
  u8 type = getType();
  return gabi::at<char>(type == 1   ? 0x1000DE18
                        : type == 3 ? 0x1000DE24
                                    : 0x1000DE30);
}
VERIFY(0x02125E28, &daDoor10_c::getBdlName);
const char *daDoor10_c::getDzbName() {
  WWHD_FUNC(0x02125F48, const char *, this);
  u8 type = getType();
  return gabi::at<char>(type == 1   ? 0x1000DE3C
                        : type == 3 ? 0x1000DE48
                                    : 0x1000DE54);
}
VERIFY(0x02125F48, &daDoor10_c::getDzbName);
void daDoor10_c::calcMtx() {
  WWHD_FUNC(0x02125FA4, void, this);
  mDoMtx_stack_c::transS(current.pos.x, current.pos.y, current.pos.z);
  mDoMtx_stack_c::YrotM(home.angle.y);
  mDoMtx_stack_c::transM(0, m358, 0);
  J3DModel_setBaseTRMtx(mpModel, mDoMtx_stack_c::get());
}
VERIFY(0x02125FA4, &daDoor10_c::calcMtx);
s32 daDoor10_c::chkStopF() {
  WWHD_FUNC(0x021263D0, s32, this);
  u8 type = getType(), sw = getSwbit(), room = getFRoomNo();
  if (sw == 255)
    return 0;
  if (type == 0 || type == 2 || type == 3) {
    if (!roomStatus(room))
      return -1;
    return !isSwitch(sw, room);
  }
  return 0;
}
VERIFY(0x021263D0, &daDoor10_c::chkStopF);
s32 daDoor10_c::chkStopB() {
  WWHD_FUNC(0x021264C4, s32, this);
  u8 sw = getSwbit2(), room = getBRoomNo();
  if (sw == 255)
    return 0;
  if (!roomStatus(room))
    return -1;
  return !isSwitch(sw, room);
}
VERIFY(0x021264C4, &daDoor10_c::chkStopB);
void daDoor10_c::setStop() {
  WWHD_FUNC(0x02126594, void, this);
  if (chkMakeStop() && mStopBars.mpModel) {
    u8 front = mFrontCheck;
    mStopBars.mFrontCheck = front;
    if (front == 0) {
      mStopBars.m8 = chkStopF();
      mStopBars.mA = chkStopB();
    } else {
      mStopBars.m8 = chkStopB();
      mStopBars.mA = chkStopF();
    }
    mStopBars.mOffsY = 0;
  }
}
VERIFY(0x02126594, &daDoor10_c::setStop);
void daDoor10_c::openInit() {
  WWHD_FUNC(0x021262D8, void, this);
  openInitCom(1);
  onFlag(1);
  u32 p = play();
  cBgS_Release(gabi::at<dBgS>(p + 0x12A0), mpBgW);
  m358 = 0;
  speedF = 0; /* HD delays start sound until openProc actually moves. */
}
VERIFY(0x021262D8, &daDoor10_c::openInit);
BOOL daDoor10_c::openProc() {
  WWHD_FUNC(0x0212662C, BOOL, this);
  u8 room = mToRoomNo;
  if (room != 63 && !roomStatus(room))
    return FALSE;
  if (std::fabs((f32)speedF) < 0.000003814697265625f)
    sound(this, 0x6907);
  cLib_chaseF(&speedF, 30, 4);
  BOOL done = cLib_chaseF(&m358, 300, speedF) != 0;
  calcMtx();
  if (mHkyo.chkUse())
    mHkyo.calcMtx(this, m358);
  return done;
}
VERIFY(0x0212662C, &daDoor10_c::openProc);
void daDoor10_c::openEnd() {
  WWHD_FUNC(0x0212673C, void, this);
  sound(this, 0x6909);
  offFlag(1);
  m358 = 300;
  speedF = 0;
}
VERIFY(0x0212673C, &daDoor10_c::openEnd);
void daDoor10_c::closeInit() {
  WWHD_FUNC(0x02126334, void, this);
  onFlag(2);
  u32 p = play();
  if (dBgS_Regist(gabi::at<dBgS>(p + 0x12A0), mpBgW, this))
    JUT_ASSERT_fail(gabi::at<char>(0x1000DED4), 557,
                    gabi::at<char>(0x1000DED0));
  gabi::store<u8>(0x1047A964, 0);
  sound(this, 0x6908);
}
VERIFY(0x02126334, &daDoor10_c::closeInit);
BOOL daDoor10_c::closeProc() {
  WWHD_FUNC(0x021267A8, BOOL, this);
  cLib_chaseF(&speedF, 60, 6);
  BOOL done = cLib_chaseF(&m358, 0, speedF) != 0;
  calcMtx();
  if (mHkyo.chkUse())
    mHkyo.calcMtx(this, m358);
  return done;
}
VERIFY(0x021267A8, &daDoor10_c::closeProc);
void daDoor10_c::closeEnd() {
  WWHD_FUNC(0x0212683C, void, this);
  offFlag(2);
  closeEndCom();
  u32 p = play();
  gabi::Local<cXyz> dir;
  dir->set(0, 1, 0);
  gabi::call(0x025CB374, p + 0x599C, 4, -33, dir.get());
  sound(this, 0x690A);
}
VERIFY(0x0212683C, &daDoor10_c::closeEnd);
BOOL daDoor10_c::chkStopOpen() {
  WWHD_FUNC(0x02126D8C, BOOL, this);
  u8 type = getType(), sw, room;
  if (mFrontCheck == 0) {
    sw = getSwbit();
    room = getFRoomNo();
  } else {
    sw = getSwbit2();
    room = getBRoomNo();
  }
  u8 front = mFrontCheck;
  if ((front == 0 && type == 2) || (front == 1 && type == 5)) {
    u32 p = play();
    bool running = gabi::load<u8>(p + 0x5292) != 0;
    if (!running || m2A1 == 0) {
      p = play();
      if (gabi::call<BOOL>(0x025C4334, p + 0x51CC, room) &&
          !gabi::call<void *>(0x025D98E8, (s8)room)) {
        if (m2A1 != 0) {
          m2A1 = m2A1 - 1;
          return FALSE;
        }
        if (sw != 255)
          gabi::call(0x025B9E38, save(), sw, room);
        return TRUE;
      }
      m2A1 = getArg1() == 15 ? 0 : 65;
    }
  } else if (sw != 255 && isSwitch(sw, room))
    return TRUE;
  return FALSE;
}
VERIFY(0x02126D8C, &daDoor10_c::chkStopOpen);
void daDoor10_c::setStopDemo() {
  WWHD_FUNC(0x02126F48, void, this);
  m2C6 = mFrontCheck != 0;
}
VERIFY(0x02126F48, &daDoor10_c::setStopDemo);
BOOL daDoor10_c::chkStopClose() {
  WWHD_FUNC(0x02126F5C, BOOL, this);
  u8 type = getType();
  if (!mStopBars.mpModel || type == 1)
    return FALSE;
  u8 sw, room;
  if (mFrontCheck == 0) {
    if (type == 2)
      return FALSE;
    sw = getSwbit();
    room = getFRoomNo();
  } else {
    if (type == 5)
      return FALSE;
    sw = getSwbit2();
    room = getBRoomNo();
  }
  if (sw == 255)
    return FALSE;
  return !isSwitch(sw, room);
}
VERIFY(0x02126F5C, &daDoor10_c::chkStopClose);
f32 daDoor10_c::getSize2X() {
  WWHD_FUNC(0x02127038, f32, this);
  return getType() == 3 ? 225.0f : 12100.0f;
}
VERIFY(0x02127038, &daDoor10_c::getSize2X);
void daDoor10_c::setKey() {
  WWHD_FUNC(0x02127554, void, this);
  if (chkMakeKey() == 1) {
    u8 sw = getSwbit();
    if (!isSwitch(sw, -1)) {
      mKeyLock.keyOn();
      return;
    }
  }
  if (chkMakeKey() == 2) {
    if (getSwbit() == 255) {
      mKeyLock.keyOff();
      return;
    }
    if (getSwbit() >= 128) {
      mKeyLock.keyOn();
      return;
    }
    u8 sw = getSwbit();
    if (!isSwitch(sw, -1)) {
      mKeyLock.keyOn();
      return;
    }
  }
  mKeyLock.keyOff();
}
VERIFY(0x02127554, &daDoor10_c::setKey);
void daDoor10_c::setEventPrm() {
  WWHD_FUNC(0x02127080, void, this);
  if (mFrontCheck == 0) {
    m2C6 = 2;
    if (mStopBars.mA == 255)
      mStopBars.mA = chkStopB();
  } else {
    m2C6 = 3;
    if (getType() == 1 && getArg1() != 16)
      return;
    if (mStopBars.mA == 255)
      mStopBars.mA = chkStopF();
  }
  if (mStopBars.m8 != 0)
    return;
  if (getType() == 1 && getArg1() != 16)
    m2C6 = 6;
  else if (mStopBars.mA == 1)
    m2C6 = m2C6 + 2;
  if (mKeyLock.mbEnabled) {
    u8 type = getType();
    u32 s = gabi::load<u32>(0x101F84DC);
    if (type == 1) {
      if (!gabi::call<BOOL>(0x025B9100, s + 0x798, 2))
        return;
    } else if (!gabi::load<u8>(s + 0x7B8))
      return;
  }
  f32 size = getSize2X();
  if (gabi::call<BOOL>(0x0252AE04, this, size, 12100.0f, 62500.0f)) {
    u8 index = m2C6;
    u16 condition = gabi::load<u16>(gabi::ea(this) + 0xFA);
    gabi::store<s16>(gabi::ea(this) + 0xFC, mEventIdx[index]);
    u8 tool = mToolId[index];
    gabi::store<u16>(gabi::ea(this) + 0xFA, condition | 4);
    gabi::store<u8>(gabi::ea(this) + 0xFE, tool);
  }
}
VERIFY(0x02127080, &daDoor10_c::setEventPrm);
BOOL daDoor10_c::CreateHeap() {
  WWHD_FUNC(0x02126074, BOOL, this);
  auto object = [](s32 id) {
    return dComIfG_getObjectRes(gabi::at<char>(0x1000DEAC), id, 0x1000DE68);
  };
  auto stage = [](const char *name) {
    return gabi::call<void *>(0x0252447C, gabi::at<char>(0x1000DEA4), name);
  };
  J3DModelData *data;
  if (m364 != 0)
    data = (J3DModelData *)object(m364);
  else {
    const char *name = getBdlName();
    data = (J3DModelData *)stage(name);
  }
  if (!data)
    JUT_ASSERT_fail(gabi::at<char>(0x1000DEC0), 356,
                    gabi::at<char>(0x1000DEB4));
  mpModel = mDoExt_J3DModel__create(data, 0x80000, 0x11000022);
  if (!mpModel)
    return FALSE;
  switch (chkMakeKey()) {
  case 1:
    if (!mKeyLock.keyCreate(0))
      return FALSE;
    break;
  case 2:
    if (!mKeyLock.keyCreate(1))
      return FALSE;
    break;
  }
  if (mHkyo.chkUse() && !mHkyo.create())
    return FALSE;
  if (chkMakeStop() && !mStopBars.create())
    return FALSE;
  mpBgW = new_dBgW();
  if (!mpBgW)
    return FALSE;
  cBgD_t *bg;
  if (m364 != 0)
    bg = (cBgD_t *)object(m364 == 4 ? 8 : 9);
  else {
    const char *name = getDzbName();
    bg = (cBgD_t *)stage(name);
  }
  if (!bg)
    return FALSE;
  calcMtx();
  mKeyLock.calcMtx(this);
  mStopBars.calcMtx(this);
  if (mHkyo.chkUse())
    mHkyo.calcMtx(this, 0);
  J3DModel *model = mpModel;
  dBgW *world = mpBgW;
  return !cBgW_Set(world, bg, 1, J3DModel_getBaseTRMtx(model));
}
VERIFY(0x02126074, &daDoor10_c::CreateHeap);
static BOOL CheckCreateHeap(daDoor10_c *self) {
  WWHD_FUNC(0x021262D4, BOOL, self);
  return self->CreateHeap();
}
VERIFY(0x021262D4, CheckCreateHeap);
static void orderEvent(daDoor10_c *self) {
  u8 idx = self->m2C6;
  s16 evt = self->mEventIdx[idx];
  u8 tool = self->mToolId[idx];
  gabi::call(0x025D7A58, self, evt, tool, 65535, 0, 1);
}
BOOL daDoor10_actionWait(daDoor10_c *self) {
  WWHD_FUNC(0x02127218, BOOL, self);
  u16 command = gabi::load<u16>(gabi::ea(self) + 0xF8);
  if (command == 3) {
    self->initOpenDemo(1);
    self->setAction(3);
    self->demoProc();
  } else {
    if (self->mStopBars.m8 != 0) {
      if (command == 2) {
        u32 p = play();
        self->mStaffId = gabi::call<s32>(0x02542D88, p + 0x52C4,
                                         gabi::at<char>(0x1000DF08), 0, 0);
        self->shape_angle.y = self->current.angle.y;
        if (self->mFrontCheck == 1)
          self->shape_angle.y = self->shape_angle.y + 0x7fff;
        self->setAction(3);
        self->demoProc();
        return TRUE;
      }
      if (self->chkStopOpen()) {
        self->setStopDemo();
        orderEvent(self);
        return TRUE;
      }
    }
    if (self->mStopBars.m8 == 0 && self->chkStopClose()) {
      self->mStopBars.m8 = 1;
      self->mStopBars.closeInit(self);
      self->mStopBars.calcMtx(self);
      self->setAction(2);
      return TRUE;
    }
    if (self->mHkyo.chkUse() && self->mHkyo.chkFirst()) {
      self->setAction(4);
      return TRUE;
    }
    if (self->mStopBars.m8 == 0)
      self->setEventPrm();
  }
  return TRUE;
}
VERIFY(0x02127218, daDoor10_actionWait);
BOOL daDoor10_actionStopClose(daDoor10_c *self) {
  WWHD_FUNC(0x021273B4, BOOL, self);
  if (self->mStopBars.closeProc(self))
    self->setAction(1);
  self->mStopBars.calcMtx(self);
  return TRUE;
}
VERIFY(0x021273B4, daDoor10_actionStopClose);
BOOL daDoor10_actionDemo(daDoor10_c *self) {
  WWHD_FUNC(0x02127408, BOOL, self);
  s16 evt = self->mEventIdx[self->m2C6];
  u32 p = play();
  if (gabi::call<BOOL>(0x025440C8, p + 0x52C4, evt)) {
    self->setAction(1);
    p = play();
    gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
    self->shape_angle.y = self->current.angle.y;
  } else
    self->demoProc();
  return TRUE;
}
VERIFY(0x02127408, daDoor10_actionDemo);
BOOL daDoor10_actionHkyo(daDoor10_c *self) {
  WWHD_FUNC(0x02127494, BOOL, self);
  if (!self->mHkyo.chkUse()) {
    self->setAction(1);
    return TRUE;
  }
  if (gabi::load<u16>(gabi::ea(self) + 0xF8) == 2) {
    u32 p = play();
    self->mStaffId = gabi::call<s32>(0x02542D88, p + 0x52C4,
                                     gabi::at<char>(0x1000DF18), 0, 0);
    self->setAction(3);
    self->demoProc();
  } else if (self->mHkyo.chkStart()) {
    self->m2C6 = 10;
    orderEvent(self);
  }
  return TRUE;
}
VERIFY(0x02127494, daDoor10_actionHkyo);
BOOL daDoor10_actionInit(daDoor10_c *self) {
  WWHD_FUNC(0x02127634, BOOL, self);
  self->setKey();
  self->mKeyLock.calcMtx(self);
  self->setStop();
  self->mStopBars.calcMtx(self);
  if (self->mHkyo.chkUse())
    self->mHkyo.calcMtx(self, 0);
  daDoor10_actionWait(self);
  self->setAction(1);
  return TRUE;
}
VERIFY(0x02127634, daDoor10_actionInit);
BOOL daDoor10_c::draw() {
  WWHD_FUNC(0x021276B0, BOOL, this);
  if (!drawCheck(getType() == 1))
    return TRUE;
  u32 env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x025626A4, env, 0, &current.pos, &tevStr);
  env = gabi::call<u32>(0x02555D0C);
  gabi::call(0x02562F5C, env, mpModel.get(), &tevStr);
  u32 p = play();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D70));
  p = play();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D74));
  mDoExt_modelUpdateDL(mpModel, 0);
  p = play();
  gabi::store<u32>(0x104B4634, gabi::load<u32>(p + 0x5D78));
  p = play();
  gabi::store<u32>(0x104B4638, gabi::load<u32>(p + 0x5D7C));
  if (mKeyLock.mbEnabled)
    mKeyLock.draw(this);
  if (mStopBars.m8 && mStopBars.mpModel) {
    env = gabi::call<u32>(0x02555D0C);
    gabi::call(0x02562F5C, env, mStopBars.mpModel.get(), &tevStr);
    mDoExt_modelUpdateDL(mStopBars.mpModel, 0);
  }
  if (mHkyo.chkUse())
    mHkyo.draw(this);
  return TRUE;
}
VERIFY(0x021276B0, &daDoor10_c::draw);
BOOL daDoor10_Draw(daDoor10_c *self) {
  WWHD_FUNC(0x021277CC, BOOL, self);
  return self->draw();
}
VERIFY(0x021277CC, daDoor10_Draw);
BOOL daDoor10_IsDelete(daDoor10_c *self) {
  WWHD_FUNC(0x021278F8, BOOL, self);
  return TRUE;
}
VERIFY(0x021278F8, daDoor10_IsDelete);
BOOL daDoor10_Delete(daDoor10_c *self) {
  WWHD_FUNC(0x02127900, BOOL, self);
  if (gabi::load<u32>(gabi::ea(self) + 0xF4)) {
    u32 p = play();
    cBgS_Release(gabi::at<dBgS>(p + 0x12A0), self->mpBgW);
  }
  if (self->m364 != 0)
    dComIfG_resDelete(&self->mPhase, gabi::at<char>(0x1000DF28));
  if (self->chkMakeKey())
    self->mKeyLock.keyResDelete();
  if (self->mHkyo.chkUse())
    self->mHkyo.resDelete();
  self->m2D0.smokeEnd();
  return TRUE;
}
VERIFY(0x02127900, daDoor10_Delete);
void daDoor10_c::demoProc() {
  WWHD_FUNC(0x021268C8, void, this);
  s32 action = getDemoAction();
  s32 staff = mStaffId;
  u32 p = play();
  if (gabi::call<BOOL>(0x025447C8, p + 0x52C4, staff)) {
    switch (action) {
    case 3:
      openInit();
      break;
    case 4:
      closeInit();
      break;
    case 7:
      setGoal();
      break;
    case 5:
      m2D0.smokeInit(this);
      break;
    case 6:
      m2D0.smokeEnd();
      break;
    case 8:
      mKeyLock.keyInit(this);
      break;
    case 2:
      setStop();
      if (mStopBars.m8)
        mStopBars.closeInit(this);
      break;
    case 1:
      if (mHkyo.chkUse() && mHkyo.chkFirst())
        mHkyo.onFirst();
      else
        mStopBars.openInit(this);
      break;
    }
  }
  switch (action) {
  case 3:
    if (!checkFlag(1)) {
      cutEnd(this);
    } else if (openProc()) {
      openEnd();
      cutEnd(this);
    }
    break;
  case 4:
    if (!checkFlag(2))
      cutEnd(this);
    else if (closeProc()) {
      closeEnd();
      cutEnd(this);
    }
    break;
  case 5:
    m2D0.smokeProc(this);
    cutEnd(this);
    break;
  case 8:
    if (mKeyLock.keyProc())
      cutEnd(this);
    mKeyLock.calcMtx(this);
    break;
  case 2:
    if (mStopBars.closeProc(this))
      cutEnd(this);
    mStopBars.calcMtx(this);
    break;
  case 1:
    if (mHkyo.chkUse() && mHkyo.chkFirst())
      cutEnd(this);
    else if (mStopBars.openProc(this))
      cutEnd(this);
    mStopBars.calcMtx(this);
    break;
  case 19:
    p = play();
    if (gabi::load<u16>(p + 0x52B8) & 1) {
      setAction(1);
      p = play();
      gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) | 8);
      shape_angle.y = current.angle.y;
      p = play();
      gabi::store<u16>(p + 0x52B8, gabi::load<u16>(p + 0x52B8) & ~1);
      p = play();
      if (gabi::call<BOOL>(0x025449B0, p + 0x52C4)) {
        p = play();
        gabi::call(0x0254351C, p + 0x52E8);
      }
    }
    cutEnd(this);
    break;
  default:
    cutEnd(this);
    break;
  }
}
VERIFY(0x021268C8, &daDoor10_c::demoProc);
BOOL daDoor10_Execute(daDoor10_c *self) {
  WWHD_FUNC(0x021277D0, BOOL, self);
  if (!gabi::load<u32>(0x101FDA94)) {
    gabi::store<u32>(0x101FDA94, 1);
    memcpy_g(gabi::at<void>(0x101FDA98), gabi::at<void>(0x101B45E8), 20);
  }
  s32 state = self->checkExecute();
  if (self->mHkyo.chkUse())
    self->mHkyo.proc(self);
  switch (state) {
  case 0:
    self->setAction(0);
    break;
  case 2:
    gabi::call_ptr(gabi::load<u32>(0x101FDA98 + self->m354 * 4), self);
    break;
  case 1:
    self->startDemoProc();
    self->demoProc();
    break;
  default:
    JUT_ASSERT_fail(gabi::at<char>(0x1000DE90), 1049,
                    gabi::at<char>(0x1000DE64));
    break;
  }
  self->mRoomNo2 = gabi::load<s8>(0x1047E6C8);
  return TRUE;
}
VERIFY(0x021277D0, daDoor10_Execute);
BOOL daDoor10_c::CreateInit() {
  WWHD_FUNC(0x02127998, BOOL, this);
  u32 p = play();
  if (dBgS_Regist(gabi::at<dBgS>(p + 0x12A0), mpBgW, this))
    JUT_ASSERT_fail(gabi::at<char>(0x1000DF3C), 623,
                    gabi::at<char>(0x1000DF38));
  gabi::store<u8>(gabi::ea(this) + 0x1C9, current.roomNo);
  m358 = 0;
  setAction(0);
  u8 type = getType();
  f32 a = gabi::load<f32>(gabi::ea(this) + 0x394),
      b = gabi::load<f32>(gabi::ea(this) + 0x380),
      height = type == 1 ? 250 : 150;
  gabi::store<u32>(gabi::ea(this) + 0x39C, 32);
  gabi::store<f32>(gabi::ea(this) + 0x394, a + height);
  gabi::store<f32>(gabi::ea(this) + 0x380, b + height);
  calcMtx();
  gabi::call(0x024F43DC, mpBgW.get());
  u32 world = gabi::ea(mpBgW.get());
  u8 room = getFRoomNo();
  gabi::store<u16>(world + 0xB8, room);
  initProc(1);
  if (mHkyo.chkUse()) {
    mHkyo.init();
    mHkyo.calcMtx(this, 0);
  }
  m2A1 = 65;
  return TRUE;
}
VERIFY(0x02127998, &daDoor10_c::CreateInit);
cPhs_State daDoor10_c::create() {
  WWHD_FUNC(0x02127B20, cPhs_State, this);
  u32 arg = getArg1();
  if (arg >= 1 && arg <= 4)
    mHkyo.onUse(getArg1());
  else
    mHkyo.offUse();
  arg = getArg1();
  m364 = arg == 13 ? 4 : arg == 14 ? 5 : 0;
  cPhs_State state;
  if (m364) {
    state = dComIfG_resLoad(&mPhase, gabi::at<char>(0x1000DF4C));
    if (state != cPhs_COMPLEATE_e)
      return state;
  }
  if (chkMakeKey()) {
    state = mKeyLock.keyResLoad();
    if (state != cPhs_COMPLEATE_e)
      return state;
  }
  if (mHkyo.chkUse()) {
    state = mHkyo.resLoad();
    if (state != cPhs_COMPLEATE_e)
      return state;
  }
  current.roomNo = getFRoomNo();
  if (!gabi::call<BOOL>(0x025D63E8, this, 0x021262D4, 0x3800))
    return cPhs_ERROR_e;
  CreateInit();
  return cPhs_COMPLEATE_e;
}
VERIFY(0x02127B20, &daDoor10_c::create);
cPhs_State daDoor10_Create(daDoor10_c *self) {
  WWHD_FUNC(0x02127C80, cPhs_State, self);
  u32 condition = gabi::load<u32>(gabi::ea(self) + 0x2E4);
  if (!(condition & 8)) {
    if (self) {
      gabi::call(0x0252A244, self);
      gabi::store<u32>(gabi::ea(self) + 0xB4, 0x1000DE80);
      gabi::call(0x0252B190, &self->m2D0);
      gabi::call(0x0252B3D0, &self->mKeyLock);
      gabi::call(0x0252BA44, &self->mStopBars);
      gabi::call(0x0252C0BC, &self->mHkyo);
      condition = gabi::load<u32>(gabi::ea(self) + 0x2E4);
    }
    gabi::store<u32>(gabi::ea(self) + 0x2E4, condition | 8);
  }
  return self->create();
}
VERIFY(0x02127C80, daDoor10_Create);
void door10_sinit() {
  WWHD_FUNC(0x02127D04, void, 0);
  sinit_header_statics(0x10463BB8, 0x101B45FC);
}
VERIFY(0x02127D04, door10_sinit);
void door10_genericDtor(void *self, s32 flags) {
  WWHD_FUNC(0x02127D98, void, self, flags);
  if (self && (flags & 1))
    gabi::call(0x0273AF40, self);
}
VERIFY(0x02127D98, door10_genericDtor);
void door10_actorDtor(daDoor10_c *self, s32 flags) {
  WWHD_FUNC(0x02127DAC, void, self, flags);
  if (self) {
    gabi::call(0x027F3628, gabi::ea(self) + 0x43C, 0);
    gabi::call(0x025D50BC, self, 0);
    if (flags & 1)
      gabi::call(0x0273AF40, self);
  }
}
VERIFY(0x02127DAC, door10_actorDtor);
void door10_empty(void *self) { WWHD_FUNC(0x02127E0C, void, self); }
VERIFY(0x02127E0C, door10_empty);

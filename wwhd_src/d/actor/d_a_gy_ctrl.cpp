/* Gyorg encounter controller. Restored tww structure with retained HD behavior. */
#include "d/actor/d_a_gy_ctrl.h"
#include "gc_api/gc_api.h"

static daGy_Ctrl_HIO_c l_HIO;
static GyHideRegsHD l_hideRegs;
static GyHeaderZerosHD l_headerZeros;
static GyRegistrationHD l_registration0;
static GyRegistrationHD l_registration1;
static GyRegistrationHD l_registration2;
static f32 l_minAngle;
static f32 l_maxAngle;
static u8 l_angleInit;
static u8 l_limitInit;
static GyModeEntryHD mode_tbl[4] = {
    {&daGy_Ctrl_c::modeSwWaitInit, &daGy_Ctrl_c::modeSwWait, "SW_WAIT"},
    {&daGy_Ctrl_c::modeCreateInit, &daGy_Ctrl_c::modeCreate, "CREATE"},
    {&daGy_Ctrl_c::modeWaitInit, &daGy_Ctrl_c::modeWait, "WAIT"},
    {&daGy_Ctrl_c::modeHideInit, &daGy_Ctrl_c::modeHide, "HIDE"}};
enum GyNative_e {
  GY_NAME_JUDGE_e = 0x025E121C,
  fpcNm_GY_CTRL_e = 0xE5,
  fpcNm_GY_CTRLB_e = 0xE6,
  fpcNm_GY_e = 0xE4,
  fpcNm_DAIOCTA_e = 0xE1,
  fpcNm_OBJ_IKADA_e = 0x44,
  fpcNm_NPC_SO_e = 0x76,
  dItemNo_BOOMERANG_e = 0x2D,
  daPyStts0_SHIP_RIDE_e = 0x10000,
  daPyStts0_UNK1000000_e = 0x1000000,
  daPyStts0_SWIM_e = 0x100000
};
s32 Gy_calcTimer(be<s32> *timer);
f64 Gy_sqrtDistance(f32 square);
void *Gy_getCameraBody();
void Gy_setCameraType(void *camera, const char *type, u32 argument);
u32 Gy_createChild(s32 name, u32 parent, s32 parameter, cXyz *position,
                   s32 room, csXyz *angle, u32 scale, s32 subtype, u32 append);
void Gy_initAngles(csXyz *, s16, s16, s16);
void Gy_registerGlobal(GyRegistrationHD *);
void Gy_initAngle(be<u8> *);
void Gy_initLimit(be<u8> *);
void *searchNearActor_CB(void *, void *);
daGy_Ctrl_HIO_c *HIO_constructHD(daGy_Ctrl_HIO_c *);

// HD: retain this TU's constructor stores and their order for the embedded line
// check.
daGyCtrl_c::daGyCtrl_c() {
  fopAc_ac_c_ct(this);
  __vtbl = 0x10010D78;
  Gy_constructLine(&mLinChk);
  mLinChk.mbBombThrough = 0;
  mLinChk.mbCamThrough = 0;
  mLinChk.cBgS_PolyInfo::__vtbl = 0x10010D48;
  mLinChk.mGrp = 1;
  mLinChk.mbBoomerangThrough = 0;
  mLinChk.mbArrowThrough = 0;
  mLinChk.dBgS_GrpPassChk::__vtbl = 0x10010D58;
  mLinChk.mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(&mLinChk));
  mLinChk.mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(&mLinChk));
  mLinChk.dBgS_PolyPassChk::__vtbl = 0x10010D68;
  mLinChk.mbObjThrough = 1;
  mLinChk.cBgS_Chk::__vtbl = 0x10010D38;
  mLinChk.mbLinkThrough = 0;
  mLinChk.mbRopeThrough = 0;
}

/* 0216B7B4 */
void *searchNearActor_CB(void *actor, void *controller) {
  return static_cast<daGy_Ctrl_c *>(controller)
      ->searchNearActor(static_cast<fopAc_ac_c *>(actor));
}

/* 0216B6E8 */
void *daGy_Ctrl_c::searchNearActor(fopAc_ac_c *actor) {
  if (!fopAc_IsActor(actor))
    return nullptr;
  f32 distance = fopAcM_searchActorDistanceXZ(this, actor);
  s16 id = actor ? actor->mProcName.get() : 0;
  if (mType == TYPE_NO_AREA_e && actor && id == fpcNm_GY_CTRLB_e &&
      distance < 6000.0f)
    return actor;
  if (actor &&
      (id == fpcNm_DAIOCTA_e || id == fpcNm_OBJ_IKADA_e || id == fpcNm_NPC_SO_e) &&
      distance < 6000.0f)
    return actor;
  return nullptr;
}

/* 0216BBBC */
void daGy_Ctrl_c::setTarget() {
  if (dComIfGp_checkPlayerStatus0(0, daPyStts0_SHIP_RIDE_e) ||
      dComIfGp_checkPlayerStatus0(0, daPyStts0_UNK1000000_e)) {
    if (mPathClear && !Gy_calcTimer(&mCameraTimer))
      mCameraTimer = 30;
    // HD: the camera mode store also occurs when the path is blocked, before
    // the common clear.
    mCameraMode = 2;
    mTarget = TARGET_SHIP_e;
  } else {
    if (dComIfGp_checkPlayerStatus0(0, daPyStts0_SWIM_e)) {
      if (!Gy_calcTimer(&mCameraTimer))
        mCameraTimer = 30;
      mCameraMode = 1;
    } else {
      mCameraMode = 0;
      mCameraTimer = 30;
    }
    mTarget = TARGET_PLAYER_e;
  }
  if (!mPathClear || !mTargetInRange)
    mCameraMode = 0;
  mAllChildrenDead = 1;
  for (u32 i = 0; i < u8(mCount); ++i) {
    u32 id = mChildIds[i];
    GyChildHD *child = static_cast<GyChildHD *>(fopAcM_SearchByID(id));
    if (child && child->mAlive)
      mAllChildrenDead = 0;
  }
  if (mAllChildrenDead)
    mCameraMode = 0;
  else {
    s32 mode = mCameraMode;
    if (mode == 1 || mode == 2) {
      void *camera = Gy_getCameraBody();
      Gy_setCameraType(camera, mode == 1 ? "WaterBattle" : "BoatBattle", 0);
    }
  }
}

/* 0216BDD4 */
BOOL daGy_Ctrl_c::setPathTargetPos() {
  cXyz pos;
  if (mTarget == TARGET_PLAYER_e) {
    pos.copy(dComIfGp_getPlayer(0)->current.pos);
    mAngle = s16(s16(mAngle) + s16(l_HIO.mPlayerAngleStep));
    mDesiredRadius = l_HIO.mPlayerRadius;
  } else if (mTarget == TARGET_SHIP_e) {
    fopAc_ac_c *ship = dComIfGp_getShipActor();
    if (ship) {
      pos.copy(ship->current.pos);
      mDesiredRadius = l_HIO.mShipRadius;
      mAngle = s16(s16(mAngle) + s16(l_HIO.mShipAngleStep));
    }
  }
  pos.y = getWaterY(&pos);
  if (mType != TYPE_NO_AREA_e) {
    cXyz difference = mCenter - pos;
    cXyz flat;
    flat.x = difference.x;
    flat.y = 0;
    flat.z = difference.z;
    f32 square = PSVECSquareMag(&flat);
    // HD: retain the double-precision distance returned in the full FP
    // register.
    f64 distance = Gy_sqrtDistance(square);
    if (!(distance < f32(mMaxDistance)))
      return FALSE;
  }
  current.pos.x = pos.x;
  current.pos.y = pos.y;
  current.pos.z = pos.z;
  return TRUE;
}

/* 0216BDB8 */
f32 daGy_Ctrl_c::getWaterY(cXyz *pos) {
  pos->y = f32(pos->y) + 1000.0f;
  return dBgS_GetWaterHeight(pos);
}

/* 0216BF50 */
BOOL daGy_Ctrl_c::lineCheck(cXyz *start, cXyz *end) {
  mLinChk.Set(start, end, this);
  if (!dComIfG_Bgsp()->LineCross(&mLinChk))
    return FALSE;
  end->copy(mLinChk.mLin.mEnd);
  return TRUE;
}

/* 0216BFE0 */
BOOL daGy_Ctrl_c::checkPath() {
  if (dComIfGp_getEvent()->mMode)
    return FALSE;
  for (u32 i = 0; i < 16; ++i) {
    f32 radius = mRadius;
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::YrotM(s16(i * 0xFFF));
    mDoMtx_stack_c::transM(radius, 0.0f, 0.0f);
    cXyz *path = &mRadialPaths[i];
    cXyz start;
    Gy_copyTranslationHD(*path, *mDoMtx_stack_c::get());
    start.x = path->x;
    start.y = f32(path->y) + f32(l_HIO.mLineStartHeight);
    start.z = path->z;
    path->y = f32(mDoMtx_stack_c::get()->m[1][3]) + f32(l_HIO.mLineEndHeight);
    if (lineCheck(&start, path))
      return FALSE;
  }
  return TRUE;
}

/* 0216C27C */
void daGy_Ctrl_c::setPathPos() {
  for (u32 i = 0; i < u8(mCount); ++i) {
    u32 count = u8(mCount);
    s16 angle = s16(s16(mAngle) + (65535 / count) * i);
    mSpawnAngles[i] = angle;
    f32 radius = gabi::fmadds(cM_ssin(u16(angle)), l_HIO.mRadiusWave, mRadius);
    s16 checkAngle = s16(angle + 0x500);
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::YrotM(angle);
    mDoMtx_stack_c::transM(radius, 0.0f, 0.0f);
    cXyz *spawn = &mSpawnPositions[i];
    cXyz *check = &mCheckPositions[i];
    Gy_copyTranslationHD(*spawn, *mDoMtx_stack_c::get());
    spawn->y = getWaterY(spawn);
    f32 height = l_HIO.mLineEndHeight;
    mDoMtx_stack_c::transS(current.pos);
    mDoMtx_stack_c::YrotM(checkAngle);
    mDoMtx_stack_c::transM(radius, 0.0f, height);
    Gy_copyTranslationHD(*check, *mDoMtx_stack_c::get());
    cXyz start;
    start.x = check->x;
    start.y = check->y;
    start.z = check->z;
    start.y = f32(start.y) + f32(l_HIO.mLineStartHeight);
    check->y = getWaterY(check);
    // HD: XOR and byte truncation preserve non-boolean callee returns.
    mPathFree[i] = u8(u32(lineCheck(&start, check)) ^ 1u);
  }
  cLib_addCalc2(&mRadius, mDesiredRadius, 0.1f, l_HIO.mRadiusStep);
}

/* 0216B924 */
void daGy_Ctrl_c::modeProc(s32 action, s32 mode) {
  if (action == PROC_INIT_e) {
    mMode = mode;
    (this->*mode_tbl[mode].init)();
  } else if (action == PROC_EXEC_e)
    (this->*mode_tbl[mMode].exec)();
}

/* 0216C9D0 */
void daGy_Ctrl_c::modeSwWaitInit() {}

/* 0216C1E0 */
void daGy_Ctrl_c::modeSwWait() {
  s32 room = mType == TYPE_AREA_e ? s32(current.roomNo) : s32(mRoom);
  // HD: capture the room before the save pointer accessor, as in the retained
  // caller.
  dSv_info_c *save = dComIfGs_info();
  u8 sw = mSwitch;
  if (save->isSwitch(sw, room))
    modeProc(PROC_INIT_e, MODE_CREATE_e);
}

/* 0216C26C */
void daGy_Ctrl_c::modeCreateInit() { mCreateTimer = l_HIO.mCreateDelay; }

/* inlined into daGy_Ctrl_c::modeCreate and modeWait */
static bool mustHide(daGy_Ctrl_c *self) {
  return (self->mType == daGy_Ctrl_c::TYPE_AREA_e && !self->mTargetInRange) ||
         !self->mPathClear ||
         fopAcM_Search(searchNearActor_CB, self) != nullptr;
}

/* 0216C4A8 */
void daGy_Ctrl_c::modeCreate() {
  if (mustHide(this)) {
    modeProc(PROC_INIT_e, MODE_HIDE_e);
    return;
  }
  s32 index = mSpawnIndex;
  u32 count = u8(mCount);
  bool pending = index < s32(count);
  if (pending && !Gy_calcTimer(&mCreateTimer)) {
    index = mSpawnIndex;
    cXyz pos;
    cXyz *path = &mSpawnPositions[index];
    pos.x = path->x;
    pos.y = path->y;
    pos.z = path->z;
    csXyz angles;
    Gy_initAngles(&angles, 0, 0, 0);
    s32 room = tevStr.mRoomNo;
    index = mSpawnIndex;
    pos.y = -1000.0f;
    u32 parent = mBsPcId;
    angles.y = mSpawnAngles[index];
    be<u32> *destination = &mChildIds[index];
    *destination =
        Gy_createChild(fpcNm_GY_e, parent, -1, &pos, room, &angles, 0, -1, 0);
    index = mSpawnIndex;
    mChildActive[index] = 1;
    mCreateTimer = l_HIO.mSpawnDelay;
  }
  if (pending) {
    count = u8(mCount);
    index = mSpawnIndex;
  }
  if (u32(index) == count)
    modeProc(PROC_INIT_e, MODE_WAIT_e);
  setPathPos();
}

/* 0216C9D4 */
void daGy_Ctrl_c::modeWaitInit() {}

/* 0216C6E8 */
void daGy_Ctrl_c::modeWait() {
  if (mustHide(this)) {
    modeProc(PROC_INIT_e, MODE_HIDE_e);
    return;
  }
  deadCheckGy();
  setPathPos();
}

/* 0216C780 */
void daGy_Ctrl_c::modeHideInit() { mHideTimer = s16(l_hideRegs.initial) + 30; }

/* 0216C794 */
void daGy_Ctrl_c::modeHide() {
  bool ready =
      (mType == TYPE_AREA_e ? mTargetInRange != 0 : mType == TYPE_NO_AREA_e) &&
      mPathClear != 0;
  if (Gy_calcTimer(&mHideTimer))
    return;
  if (fopAcM_Search(searchNearActor_CB, this)) {
    mHideTimer = s16(l_hideRegs.retry) + 30;
    return;
  }
  if (ready && mPathClear)
    modeProc(PROC_INIT_e, MODE_CREATE_e);
}

/* 0216C634 */
void daGy_Ctrl_c::deadCheckGy() {
  u32 count = u8(mCount), dead = 0;
  for (u32 i = 0; i < count; ++i) {
    if (!mChildActive[i])
      continue;
    u32 id = mChildIds[i];
    if (id == 0xFFFFFFFFu)
      ++dead;
    else {
      fopAc_ac_c *child = fopAcM_SearchByID(id);
      count = u8(mCount);
      if (!child)
        ++dead;
    }
  }
  if (dead == count)
    fopAcM_delete(this);
}

/* 0216C13C */
s32 daGy_Ctrl_c::_execute() {
  setTarget();
  mTargetInRange = setPathTargetPos();
  mPathClear = checkPath();
  modeProc(PROC_EXEC_e, 4);
  mRoom = current.roomNo;
  return 0;
}

/* 0216C1A4 */
BOOL daGy_Ctrl_c::_draw() {
  // HD: debug draw retains only the HIO read and empty iteration loops.
  if (l_HIO.mDrawDebug) {
    for (u32 i = 0; i < u8(mCount); ++i) {
    }
    for (u32 i = 0; i < 16; ++i) {
    }
  }
  return TRUE;
}

/* 0216B9D0 */
void daGy_Ctrl_c::createInitNoArer() {
  fopAcM_setStageLayer(this);
  mLayerInitialized = 1;
  mRoom = current.roomNo;
}

/* 0216BA10 */
void daGy_Ctrl_c::createInit() {
  modeProc(PROC_INIT_e, mSwitch == 255 ? MODE_CREATE_e : MODE_SWITCH_WAIT_e);
  mCenter.copy(current.pos);
  mTarget = TARGET_PLAYER_e;
  if (mType == TYPE_NO_AREA_e)
    createInitNoArer();
}

/* 0216B7C4 */
u8 daGy_Ctrl_c::getParam(u32 param, u32 shift, u32 bits) {
  // HD: keep PPC shift semantics for the full incoming registers.
  shift &= 63;
  bits &= 63;
  return u8((shift < 32 ? param >> shift : 0) &
            ((bits < 32 ? 1u << bits : 0) - 1));
}

/* 0216B7E0 */
void daGy_Ctrl_c::getArg() {
  u32 param = mParameters;
  mType = getParam(param, 0, 4);
  mCount = getParam(param, 4, 4);
  u8 distance = getParam(param, 8, 8);
  mSwitch = getParam(param, 24, 8);
  if (mType == 15)
    mType = TYPE_NO_AREA_e;
  if (mCount == 15)
    mCount = 1;
  mMaxDistance = distance == 255 ? 10000.0f : f32(distance) * 1000.0f;
}

/* 0216B8B8 */
BOOL daGy_Ctrl_c::checkGyCtrlExist() {
  gabi::Local<be<s16>> id;
  *id = fpcNm_GY_CTRL_e;
  daGy_Ctrl_c *actor =
      static_cast<daGy_Ctrl_c *>(fopAcIt_Judge(GY_NAME_JUDGE_e, id.get()));
  return actor && actor->mType == TYPE_NO_AREA_e &&
         actor->mLayerInitialized != 0;
}

/* 0216BA8C */
s32 daGy_Ctrl_c::_create() {
  fopAcM_ct(this, daGy_Ctrl_c);
  getArg();
  if (mType == TYPE_NO_AREA_e && checkGyCtrlExist())
    return cPhs_ERROR_e;
  if (!dComIfGs_checkGetItem(dItemNo_BOOMERANG_e))
    return cPhs_ERROR_e;
  createInit();
  return cPhs_COMPLEATE_e;
}

/* 0216BBB0 */
s32 daGy_CtrlCreate(daGy_Ctrl_c *i_this) { return i_this->_create(); }
/* 0216BBB4 */
BOOL daGy_CtrlDelete(daGy_Ctrl_c *i_this) { return TRUE; }
/* 0216C1A0 */
s32 daGy_CtrlExecute(daGy_Ctrl_c *i_this) { return i_this->_execute(); }
/* 0216C1DC */
BOOL daGy_CtrlDraw(daGy_Ctrl_c *i_this) { return i_this->_draw(); }
/* 0216C9C8 */
BOOL daGy_CtrlIsDelete(daGy_Ctrl_c *i_this) { return TRUE; }

// HD: compiler-generated and template code of this translation unit.
/* 0216C9D8 */
void Gy_destructHD(daGy_Ctrl_c *i_this, u32 flags) {
  if (!i_this)
    return;
  i_this->mLinChk.dBgS_PolyPassChk::__vtbl = 0x10010D28;
  i_this->mLinChk.dBgS_GrpPassChk::__vtbl = 0x10010CE8;
  i_this->mLinChk.cBgS_PolyInfo::__vtbl = 0x10010CD8;
  Gy_destructLine(&i_this->mLinChk, 0);
  __dt(i_this);
  if (flags & 1)
    operator_delete(i_this);
}

/* 0216C870 */
daGy_Ctrl_HIO_c *HIO_constructHD(daGy_Ctrl_HIO_c *i_this) {
  if (!i_this)
    i_this =
        static_cast<daGy_Ctrl_HIO_c *>(operator_new(sizeof(daGy_Ctrl_HIO_c)));
  if (!i_this)
    return nullptr;
  i_this->mLineEndHeight = -500.0f;
  i_this->mShipAngleStep = 250;
  i_this->mPlayerRadius = 700.0f;
  i_this->mRadiusWave = 10.0f;
  i_this->mSpawnDelay = 240;
  i_this->mShipRadius = 1000.0f;
  i_this->mMaximumDistance = 10000.0f;
  i_this->mPlayerAngleStep = 200;
  i_this->mCreateDelay = 100;
  i_this->mDrawDebug = 0;
  i_this->mRadiusStep = 10.0f;
  i_this->mDebugEnabled = 0;
  i_this->mLineStartHeight = 500.0f;
  i_this->mVtable = 0x10010D88;
  return i_this;
}

/* 0216C928 */
void __sinit_d_a_gy_ctrl_cpp() {
  l_headerZeros.words[2] = 0;
  l_headerZeros.words[0] = 0;
  l_headerZeros.words[3] = 0;
  l_headerZeros.words[1] = 0;
  Gy_registerGlobal(&l_registration0);
  l_minAngle = -3.1415927f;
  l_maxAngle = 3.1415927f;
  Gy_initAngle(&l_angleInit);
  Gy_registerGlobal(&l_registration1);
  Gy_initLimit(&l_limitInit);
  Gy_registerGlobal(&l_registration2);
  HIO_constructHD(&l_HIO);
}

/* WWHD glue
data l_HIO 10464634
data l_hideRegs 1047BB18
data mode_tbl 10010C64
data l_headerZeros 10464664
data l_registration0 101B6D70
data l_registration1 101B6D7C
data l_registration2 101B6D88
data l_minAngle 10464628
data l_maxAngle 1046462C
data l_angleInit 10464630
data l_limitInit 10464631
str 10010CB4 "SW_WAIT"
str 10010CBC "CREATE"
str 10010CC4 "WAIT"
str 10010CCC "HIDE"
str 10010D98 "WaterBattle"
str 10010DA4 "BoatBattle"
ext 0211D2F8 s32 Gy_calcTimer(be<s32>*)
ext 028F4384 f64 Gy_sqrtDistance(f32)
ext 024F8044 void* Gy_getCameraBody()
ext 02514EE4 void Gy_setCameraType(void*,const char*,u32)
ext 025D5A20 u32 Gy_createChild(s32,u32,s32,cXyz*,s32,csXyz*,u32,s32,u32)
ext 0201A478 void Gy_initAngles(csXyz*,s16,s16,s16)
ext 02008FEC void Gy_constructLine(cBgS_LinChk*)
ext 02008B4C void Gy_destructLine(cBgS_LinChk*,s32)
ext 028F026C void Gy_registerGlobal(GyRegistrationHD*)
ext 028ED6F8 void Gy_initAngle(be<u8>*)
ext 028EAB2C void Gy_initLimit(be<u8>*)
*/

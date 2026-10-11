/* Warp-pot actor: GameCube reconstruction corrected against WWHD disassembly.
 */
#include "d/actor/d_a_obj_warpt.h"
static const WarptTU gc_tu = {
    0x100337FC, // mBodyCylinderData
    0x10033840, // mLidCylinderData
    0x100337C0, // mArchiveName
    0x100335C8, // mSafeStringVtable
    0x10033648, // mLidAssertFile
    0x1003365C, // mLidModelAssert
    0x10033644, // mLidBrkAssert
    0x10033668, // mBodyAssertFile
    0x1003367C, // mBodyModelAssert
    0x10033600, // mAcchPolyVtable
    0x10033610, // mAcchVtable
    0x024EE658, // mMoveBgTypical
    0x023B9E88, // mRideCallback
    0x10033620, // mActorVtable
    0x1004AE88, // mStatusBaseVtable
    0x1004AEC0, // mStatusGameVtable
    0x100335E0, // mBoundsVtable
    0x100015A8, // mShapeBaseVtable
    0x1004B108, // mCylinderVtable
    0x1004B160, // mCylinderShapeVtable
    0x1004B150, // mCylinderGeometryVtable
    0x100335F0, // mAcchBaseVtable
    0x023B9CAC, // mHeapCallback
    0x10033630, // mHioVtable
    0x100335C0, // mDefaultColor
    0x101CDDE8, // mHeaderLink0
    0x101CDDF4, // mHeaderLink1
    0x101CDE00, // mHeaderLink2
    0x10033764, // mWarpCut
    0x1003377C, // mEventAssertFile
    0x1003376C, // mCutNameAssert
};

static u16 m_event_reg[6] = {41479, 41223, 40967, 40711, 41735, 41991};
static u32 m_def_smoke_prm_color[5] = {2675940480, 2675940480, 2582404992, 1552285568, 4294924160};
static u32 m_def_smoke_env_color[5] = {797433472, 797433472, 1028706688, 4293963648, 4278209408};
static f32 l_headerFloats[2] = {-3.1415927410125732f, 3.1415927410125732f};
static WarptModeEntry mode_tbl[5] = {
  {0, -1, 0x023BB2A4, 0, -1, 0x023BB338, 0x100336EC},
  {0, -1, 0x023BB380, 0, -1, 0x023BB488, 0x100336F4},
  {0, -1, 0x023BB57C, 0, -1, 0x023BB5CC, 0x100336FC},
  {0, -1, 0x023BBC30, 0, -1, 0x023BB820, 0x10033708},
  {0, -1, 0x023BBC34, 0, -1, 0x023BB9F8, 0x10033714},
};
static u32 l_resourceControl = 0;
static u32 l_saveInfo = 0;
static s8 l_stayNo = 0;
static u32 l_colorGuard = 0;
static u32 l_savedColor = 0;
static daObj_Warpt_HIO_c l_HIO = {};
static f32 l_matrix[12] = {};
static u32 l_headerWords[4] = {};
static f32 l_headerValues[2] = {};
static u8 l_headerGuard = 0;
static u8 l_headerByte = 0;
static u32 l_opaqueList = 0;
static u32 l_translucentList = 0;
static u32 play() { return Warpt_getPlay(); }
static f32 warptLinear(u32 value);
static void warptColor(u8* color);
static bool used(u8 *bg) { return bg && Warpt_guest<WarptBgW>(gabi::ea(bg))->mUseFlags < 0x100; }
static s8 stay() { return l_stayNo; }
static u32 save() { return l_saveInfo; }











static u8 *resource(s32 index) {
  gabi::Local<WarptSafeString> name;
  name->mText = gabi::guest_addr("ltubw");
  name->mVtable = gc_tu.mSafeStringVtable;
  u32 control = l_resourceControl;
  return Warpt_getObjectRes(Warpt_guest<u8>(control), name.get(), index);
}

















static void warptModelMatrix(u8 *model) {
  f32 matrix[12];
  for (u32 i = 0; i < 12; i++)
    matrix[i] = l_matrix[i];
  WarptModel* destination = Warpt_guest<WarptModel>(gabi::ea(model));
  for (u32 i = 0; i < 12; i++)
    destination->mBaseTransformMtx[i] = matrix[i];
}








static void warptDrawLists(bool background) {
  u32 p = play();
  l_opaqueList = background ? (u32)Warpt_guest<WarptPlay>(p)->mBgList.mOpaque : (u32)Warpt_guest<WarptPlay>(p)->mList.mOpaque;
  p = play();
  l_translucentList = background ? (u32)Warpt_guest<WarptPlay>(p)->mBgList.mTranslucent : (u32)Warpt_guest<WarptPlay>(p)->mList.mTranslucent;
}



static bool warptCustomColor(u32 color) {
  return Warpt_guest<WarptColor>(color)->r || Warpt_guest<WarptColor>(color)->g ||
         Warpt_guest<WarptColor>(color)->b;
}
static void warptEmitterColor(WarptColor* destination, u32 color) {
  f32 red = warptLinear(Warpt_guest<WarptColor>(color)->r);
  u8 r = (u8)gabi::ftoi(red * 255.0f);
  f32 green = warptLinear(Warpt_guest<WarptColor>(color)->g);
  u8 g = (u8)gabi::ftoi(green * 255.0f);
  f32 blue = warptLinear(Warpt_guest<WarptColor>(color)->b);
  destination->r = r;
  destination->g = g;
  destination->b = (u8)gabi::ftoi(blue * 255.0f);
}


static bool warptCutWarp(u32 text) {
  u32 reference = gc_tu.mWarpCut;
  for (;;) {
    u8 a = *Warpt_guest<be<u8>>(text++), b = *Warpt_guest<be<u8>>(reference++);
    if (a != b)
      return false;
    if (!a)
      return true;
  }
}


/* 023B9CAC */
BOOL createHeap_CB(daObj_Warpt_c *self) {
  return self->_createHeap();
}
/* 023B9C28 */
BOOL daObj_Warpt_c::_createHeap() {
  if (isRealHuta()) {
    if (!createHutaHeap())
      return false;
  } else {
    mpLidBgW = nullptr;
    mpLidModel2 = nullptr;
    mpLidModel1 = nullptr;
  }
  return createBodyHeap();
}
/* 023B9944 */
bool daObj_Warpt_c::createHutaHeap() {
  u8 *data = resource(dRes_INDEX_LTUBW_BDL_ITUHU_e);
  if (!data)
    Warpt_assert(Warpt_guest<u8>(gc_tu.mLidAssertFile), 0x117, Warpt_guest<u8>(gc_tu.mLidModelAssert));
  mpLidModel1 = Warpt_modelCreate(data, 0x80000, 0x11000022);
  if (mpLidModel1 == nullptr)
    return false;
  mpLidBgW = Warpt_bgNew((u8 *)nullptr);
  if (mpLidBgW == nullptr)
    return false;
  data = resource(dRes_INDEX_LTUBW_DZB_ITUHT_e);
  if (Warpt_bgSet((u8 *)mpLidBgW, data, 1, m344))
    return false;
  data = resource(dRes_INDEX_LTUBW_BDL_YWPFM00_e);
  if (!data)
    return false;
  mpLidModel2 = Warpt_modelCreate(data, 0x80000, 0x11000022);
  if (mpLidModel2 == nullptr)
    return false;
  u8 *animation = resource(dRes_INDEX_LTUBW_BRK_YWPFM00_e);
  if (!animation)
    Warpt_assert(Warpt_guest<u8>(gc_tu.mLidAssertFile), 0x132, Warpt_guest<u8>(gc_tu.mLidBrkAssert));
  return Warpt_brkInit(&mLidBrk, data, animation, 1, 2, 1.0f, 0, -1, 0, 0) != 0;
}
/* 023B9AE8 */
bool daObj_Warpt_c::createBodyHeap() {
  u8 *data = resource(dRes_INDEX_LTUBW_BDL_ITUBW_e);
  if (!data)
    Warpt_assert(Warpt_guest<u8>(gc_tu.mBodyAssertFile), 0x140, Warpt_guest<u8>(gc_tu.mBodyModelAssert));
  mpBodyModel = Warpt_modelCreate(data, 0x80000, 0x11000022);
  if (mpBodyModel == nullptr)
    return false;
  mpBodyBgW2 = Warpt_bgNew((u8 *)nullptr);
  if (mpBodyBgW2 == nullptr)
    return false;
  data = resource(dRes_INDEX_LTUBW_DZB_ITUBW_e);
  if (Warpt_bgSet((u8 *)mpBodyBgW2, data, 1, m310))
    return false;
  mpBodyBgW1 = Warpt_bgNew((u8 *)nullptr);
  if (mpBodyBgW1 == nullptr)
    return false;
  data = resource(dRes_INDEX_LTUBW_DZB_ITUSO_e);
  return Warpt_bgSet((u8 *)mpBodyBgW1, data, 1, m2DC) == 0;
}
/* 023B9E88 */
static void ride_CB(u8 *bg, daObj_Warpt_c *self, fopAc_ac_c *rider) {
  self->_ride(rider);
}
/* 023B9D5C */
void daObj_Warpt_c::_ride(fopAc_ac_c *rider) {
  if (!rider || rider->mProcName != 0xA8)
    return;
  u32 p = play();
  u32 player = Warpt_guest<WarptPlay>(p)->mpPlayer;
  Warpt_searchPlayerAngleY(this, Warpt_guest<u8>(player));
  gabi::Local<cXyz> position;
  *position = m830;
  Warpt_checkPlayerInCircle(position.get(), 1.0f, 20.0f);
  if (l_HIO.m06)
    return;
  u32 table = rider->__vtbl;
  if (Warpt_checkPlayerFly(table, rider))
    return;
  if (m290 == MODE_EVENT_WARP)
    return;
  gabi::Local<cXyz> inner;
  *inner = m830;
  f32 radius = l_HIO.m0C;
  if (Warpt_checkPlayerInCircle(inner.get(), radius, 20.0f))
    modeProc(PROC_INIT_e, 3);
}
/* 023B985C */
u8 daObj_Warpt_c::isHuta() {
  return m2C6;
}
/* 023B9864 */
bool daObj_Warpt_c::isSp() {
  return ((u32)(s32)m2B4 - 2u) <= 2;
}
/* 023B9884 */
bool daObj_Warpt_c::isOtherHuta() {
  if (!isSp())
    return false;
  s32 sw = m2AC;
  if (sw == 255)
    return false;
  u32 s = save();
  s8 room = stay();
  return !Warpt_isSwitch(&Warpt_guest<WarptSave>(s)->mZone, sw, room);
}
/* 023B98F4 */
bool daObj_Warpt_c::isRealHuta() {
  return isHuta() && !isOtherHuta();
}
/* 023BB64C */
void daObj_Warpt_c::warp(s32 stage) {
  Warpt_seStart(0x6934, 0);
  if (stage != 255)
    Warpt_setNextStageBySclsNum((u8)stage, (s8)current.roomNo);
}
/* 023B9E94 */
void daObj_Warpt_c::onWarpBit(u8 bit) {
  u32 index = m2B8, s = save();
  u16 event = m_event_reg[(index * 2) / 2];
  u32 flags = Warpt_getEventReg(&Warpt_guest<WarptSave>(s)->mEvent, event);
  index = m2B8;
  s = save();
  event = m_event_reg[(index * 2) / 2];
  Warpt_setEventReg(&Warpt_guest<WarptSave>(s)->mEvent, event, (u8)(flags | bit));
}
/* 023B9F1C */
bool daObj_Warpt_c::isWarpBit(u8 bit) {
  u32 index = m2B8, s = save();
  u16 event = m_event_reg[(index * 2) / 2];
  return (u8)(Warpt_getEventReg(&Warpt_guest<WarptSave>(s)->mEvent, event) & bit) != 0;
}
/* 023BA250 */
void daObj_Warpt_c::setMtx() {
  Warpt_transS(l_matrix, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
  Warpt_YrotM(l_matrix, shape_angle.y);
  Warpt_scaleM((f32)scale.x, (f32)scale.y, (f32)scale.z);
  warptModelMatrix(mpBodyModel);
  Warpt_matrixCopy(l_matrix, m310);
  Warpt_matrixCopy(l_matrix, m2DC);
  Warpt_bgMove((u8 *)mpBodyBgW1);
  m830.z = current.pos.z;
  m830.x = current.pos.x;
  f32 y = current.pos.y;
  m830.y = y;
  m830.y = y + l_HIO.m08;
  play();
  if (m840 == 1 && mpBodyBgW2 != nullptr && !used(mpBodyBgW2)) {
    u32 p = play();
    Warpt_bgRegist(&Warpt_guest<WarptPlay>(p)->mBgS, (u8 *)mpBodyBgW2, this);
  }
  if (used(mpBodyBgW2))
    Warpt_bgMove((u8 *)mpBodyBgW2);
  if (mpLidBgW != nullptr && mpLidModel1 != nullptr) {
    Warpt_transS(l_matrix, (f32)m830.x, (f32)m830.y, (f32)m830.z);
    Warpt_YrotM(l_matrix, shape_angle.y);
    Warpt_scaleM((f32)scale.x, (f32)scale.y, (f32)scale.z);
    warptModelMatrix(mpLidModel1);
    warptModelMatrix(mpLidModel2);
    Warpt_matrixCopy(l_matrix, m344);
    Warpt_bgMove((u8 *)mpLidBgW);
  }
}
/* 023BA134 */
void daObj_Warpt_c::initCollision() {
  Warpt_statusInit(&mStts, 255, 255, this);
  Warpt_setCylinder(&mCyl1, Warpt_guest<u8>(gc_tu.mBodyCylinderData));
  u8 *bg = mpLidBgW;
  mCyl1.mStts = &mStts;
  if (bg && mpLidModel1 != nullptr) {
    Warpt_setCylinder(&mCyl2, Warpt_guest<u8>(gc_tu.mLidCylinderData));
    mCyl2.mStts = &mStts;
  }
}
/* 023BA1BC */
void daObj_Warpt_c::setCollision() {
  Warpt_cylinderSetC(&mCyl1.mCyl, &current.pos);
  u32 p = play();
  Warpt_collisionSet(&Warpt_guest<WarptPlay>(p)->mCcS, &mCyl1);
  if (mpLidBgW != nullptr && mpLidModel1 != nullptr) {
    mCyl2.mObjCo.mSPrm = mCyl2.mObjCo.mSPrm & ~1u;
    Warpt_cylinderSetC(&mCyl2.mCyl, &m830);
    p = play();
    Warpt_collisionSet(&Warpt_guest<WarptPlay>(p)->mCcS, &mCyl2);
  }
}
/* 023BAA28 */
void daObj_Warpt_c::breakHuta(s32 type) {
  if (mpLidBgW == nullptr || mpLidModel1 == nullptr)
    return;
  if (type == 0) {
    u32 p = play();
    u32 control = Warpt_guest<WarptPlay>(p)->mpParticle;
    Warpt_setParticle(Warpt_guest<u8>(control), 0, 0x8165, &m830, (u8 *)nullptr, (u8 *)nullptr, 255, (u8 *)nullptr, -1, (u8 *)nullptr, (u8 *)nullptr, 0);
    p = play();
    control = Warpt_guest<WarptPlay>(p)->mpParticle;
    Warpt_setParticle(Warpt_guest<u8>(control), 0, 0x8166, &m830, (u8 *)nullptr, (u8 *)nullptr, 255, (u8 *)nullptr, -1, (u8 *)nullptr, (u8 *)nullptr, 0);
  } else if (type == 1) {
    s32 reverb = Warpt_getReverb((s8)current.roomNo);
    Warpt_seStartCurrent(0x6847, &eyePos, 0, reverb);
    u32 p = play();
    u32 control = Warpt_guest<WarptPlay>(p)->mpParticle;
    void *color = &tevStr.mColorK0;
    Warpt_setParticle(Warpt_guest<u8>(control), 0, 0x3E5, &m830, (u8 *)nullptr, (u8 *)nullptr, 255, (u8 *)nullptr, -1, color, color, 0);
    if (used(mpLidBgW)) {
      p = play();
      Warpt_bgRelease(&Warpt_guest<WarptPlay>(p)->mBgS, (u8 *)mpLidBgW);
    }
  }
}
/* 023BB400 */
void daObj_Warpt_c::openHuta() {
  if (isSp()) {
    if (m2BC == 255)
      Warpt_seStartGlobal(0x806);
    onWarpBit(m2C4);
  } else {
    s8 room = stay();
    u32 s = save();
    s32 sw = m2AC;
    Warpt_onSwitch(&Warpt_guest<WarptSave>(s)->mZone, sw, room);
  }
  m2C6 = 0;
}
/* 023BABD8 */
void daObj_Warpt_c::checkHitSE() {
  u32 hit = Warpt_getTargetHit(&mCyl1);
  Warpt_statusMove(&mStts.__vtbl_gstts);
  if (!hit)
    return;
  u32 type = Warpt_guest<WarptAttack>(hit)->mType;
  switch (type) {
  case 2:
  case 8:
  case 0x400:
  case 0x800:
  case 0x4000:
  case 0x8000:
  case 0x10000:
  case 0x40000:
  case 0x80000:
  case 0x100000:
  case 0x1000000:
  case 0x4000000:
  case 0x8000000:
  case 0x10000000:
  case 0x20000000:
    break;
  default:
    return;
  }
  u32 p = play();
  u32 player = Warpt_guest<WarptPlay>(p)->mpPlayer;
  u8 cut = Warpt_guest<WarptPlayer>(player)->mCutType;
  if (cut == 8 || cut == 9)
    return;
  s32 reverb = Warpt_getReverb((s8)current.roomNo);
  Warpt_seStartCurrent(0x6822, &eyePos, 0, reverb);
  Warpt_hitEffHibana(this, &mCyl1);
}
/* 023BB2A4 */
void daObj_Warpt_c::modeOpenInit() {
  if (m844.mEmitter == nullptr) {
    warptColor(Warpt_guest<u8>(gabi::ea(&m86C)));
    warptColor(Warpt_guest<u8>(gabi::ea(&m870)));
    u32 p = play();
    u32 control = Warpt_guest<WarptPlay>(p)->mpParticle;
    Warpt_setParticle(Warpt_guest<u8>(control), 0, 0x8161, &current.pos, (u8 *)nullptr, (u8 *)nullptr, 255, &m844, -1, &m86C, &m870, 0);
  }
  Warpt_followRemove(&m858);
}
/* 023BB338 */
void daObj_Warpt_c::modeOpen() {
  s32 reverb = Warpt_getReverb((s8)current.roomNo);
  Warpt_seStartCurrent(0x3019, &eyePos, 0, reverb);
}
/* 023BB380 */
void daObj_Warpt_c::modeCloseInit() {
  warptColor(Warpt_guest<u8>(gabi::ea(&m86C)));
  warptColor(Warpt_guest<u8>(gabi::ea(&m870)));
  u32 p = play();
  u32 control = Warpt_guest<WarptPlay>(p)->mpParticle;
  Warpt_setParticle(Warpt_guest<u8>(control), 0, 0x8162, &current.pos, (u8 *)nullptr, (u8 *)nullptr, 255, &m858, -1, &m86C, &m870, 0);
}
/* 023BB488 */
void daObj_Warpt_c::modeClose() {
  if (mpLidBgW != nullptr && mpLidModel1 != nullptr) {
    u32 hit = Warpt_getTargetHit(&mCyl2);
    if (hit) {
      u32 type = Warpt_guest<WarptAttack>(hit)->mType;
      if (!(type & 0x20) && (type & 0x60200)) {
        breakHuta(0);
        modeProc(PROC_INIT_e, 2);
      } else {
        breakHuta(1);
        openHuta();
        modeProc(PROC_INIT_e, 0);
      }
    }
  } else {
    s8 room = stay();
    u32 s = save();
    s32 sw = m2AC;
    if (Warpt_isSwitch(&Warpt_guest<WarptSave>(s)->mZone, sw, room)) {
      openHuta();
      modeProc(PROC_INIT_e, 0);
    }
  }
}
/* 023BB57C */
void daObj_Warpt_c::modeBreakFireInit() {
  s32 reverb = Warpt_getReverb((s8)current.roomNo);
  Warpt_seStartCurrent(0x69EC, &eyePos, 0, reverb);
  m83C = 60;
}
/* 023BB5CC */
void daObj_Warpt_c::modeBreakFire() {
  Warpt_animationPlay(&mLidBrk);
  if (!Warpt_calcTimerInt(&m83C)) {
    if (used(mpLidBgW)) {
      u32 p = play();
      u8 *bg = mpLidBgW;
      Warpt_bgRelease(&Warpt_guest<WarptPlay>(p)->mBgS, bg);
    }
    openHuta();
    modeProc(PROC_INIT_e, 0);
  }
}
/* 023BB820 */
void daObj_Warpt_c::modeEventWarp() {
  u32 p = play();
  u32 player = Warpt_guest<WarptPlay>(p)->mpPlayer;
  if (eventInfo.mCommand != dEvtCmd_INDEMO_e) {
    Warpt_orderOtherEvent(this, "DEFAULT_WARP", 4, 0xFFFF);
    return;
  }
  if (used(mpBodyBgW2)) {
    p = play();
    Warpt_bgRelease(&Warpt_guest<WarptPlay>(p)->mBgS, (u8 *)mpBodyBgW2);
  }
  Warpt_guest<WarptPlayer>(player)->mNoResetFlg0 = Warpt_guest<WarptPlayer>(player)->mNoResetFlg0 | 0x40000;
  p = play();
  s32 staff =
      Warpt_getMyStaffId(&Warpt_guest<WarptPlay>(p)->mEventManager, "Warpt", 0, 0);
  p = play();
  u32 cut = Warpt_getMyNowCutName(&Warpt_guest<WarptPlay>(p)->mEventManager, staff);
  bool warped = true;
  if (!cut) {
    Warpt_assert(Warpt_guest<u8>(gc_tu.mEventAssertFile), 0x33C, Warpt_guest<u8>(gc_tu.mCutNameAssert));
  } else if (warptCutWarp(cut)) {
    warped = isSp() ? spWarp() : normalWarp();
    p = play();
    Warpt_cutEnd(&Warpt_guest<WarptPlay>(p)->mEventManager, staff);
  }
  p = play();
  if (Warpt_eventEndCheck(&Warpt_guest<WarptPlay>(p)->mEventManager, "DEFAULT_WARP") &&
      !warped) {
    m840 = 30;
    p = play();
    Warpt_guest<WarptPlay>(p)->mEventFlags = Warpt_guest<WarptPlay>(p)->mEventFlags | 8;
    Warpt_guest<WarptPlayer>(player)->mNoResetFlg1 = Warpt_guest<WarptPlayer>(player)->mNoResetFlg1 | 0x10000;
    modeProc(PROC_INIT_e, 0);
  }
}
/* 023BB7AC */
bool daObj_Warpt_c::normalWarp() {
  s8 room = stay();
  u32 s = save();
  s32 sw = m2B0;
  if (!Warpt_isSwitch(&Warpt_guest<WarptSave>(s)->mZone, sw, room))
    return false;
  warp(m29C);
  return true;
}
/* 023BB6A4 */
bool daObj_Warpt_c::spWarp() {
  switch ((s32)m2B4) {
  case 2:
    if (isWarpBit(2))
      warp(m2A4);
    else if (isWarpBit(4))
      warp(m2A8);
    else
      return false;
    break;
  case 3:
    if (isWarpBit(4))
      warp(m2A8);
    else if (isWarpBit(1))
      warp(m2A0);
    else
      return false;
    break;
  case 4:
    if (isWarpBit(1))
      warp(m2A0);
    else if (isWarpBit(2))
      warp(m2A4);
    else
      return false;
    break;
  }
  return true;
}
/* 023BB9F8 */
void daObj_Warpt_c::modeEventOpen() {
  play();
  if (eventInfo.mCommand == dEvtCmd_INDEMO_e) {
    u32 p = play();
    if (Warpt_eventEndCheck(&Warpt_guest<WarptPlay>(p)->mEventManager, "WARPT_OPEN")) {
      p = play();
      Warpt_guest<WarptPlay>(p)->mEventFlags = Warpt_guest<WarptPlay>(p)->mEventFlags | 8;
      modeProc(PROC_INIT_e, 0);
    }
  }
}
/* 023B9CB0 */
void daObj_Warpt_c::modeProc(s32 operation, s32 mode) {
  if (operation == PROC_INIT_e) {
    m290 = mode;
    (this->*Warpt_init(mode_tbl[(u32)mode]))();
  } else if (operation == PROC_EXEC_e)
    (this->*Warpt_run(mode_tbl[(u32)(s32)m290]))();
}
/* 023BAD74 */
BOOL daObj_Warpt_c::_execute() {
  u32 emitter = gabi::ea((u8 *)m844.mEmitter);
  if (emitter) {
    if (warptCustomColor(gabi::ea(&l_HIO.m14)))
      warptEmitterColor(&Warpt_guest<WarptEmitter>(emitter)->mPrmColor, gabi::ea(&l_HIO.m14));
    if (warptCustomColor(gabi::ea(&l_HIO.m18))) {
      emitter = gabi::ea((u8 *)m844.mEmitter);
      warptEmitterColor(&Warpt_guest<WarptEmitter>(emitter)->mEnvColor, gabi::ea(&l_HIO.m18));
    }
  }
  emitter = gabi::ea((u8 *)m858.mEmitter);
  if (emitter) {
    if (warptCustomColor(gabi::ea(&l_HIO.m14)))
      warptEmitterColor(&Warpt_guest<WarptEmitter>(emitter)->mPrmColor, gabi::ea(&l_HIO.m14));
    if (warptCustomColor(gabi::ea(&l_HIO.m18))) {
      emitter = gabi::ea((u8 *)m858.mEmitter);
      warptEmitterColor(&Warpt_guest<WarptEmitter>(emitter)->mEnvColor, gabi::ea(&l_HIO.m18));
    }
  }
  if (l_HIO.m04) {
    l_HIO.m04 = 0;
    breakHuta(0);
    modeProc(PROC_INIT_e, 2);
  }
  modeProc(PROC_EXEC_e, 5);
  Warpt_posMoveF(this, &mStts);
  u32 p = play();
  Warpt_acchCrrPos(&mAcch, &Warpt_guest<WarptPlay>(p)->mBgS);
  setMtx();
  setCollision();
  checkHitSE();
  Warpt_calcTimerChar(&m840);
  return 1;
}
/* 023BB074 */
BOOL daObj_Warpt_c::_draw() {
  warptDrawLists(true);
  u32 env = Warpt_getEnvLight();
  Warpt_settingTevStruct(Warpt_guest<u8>(env), 0, &current.pos, &tevStr);
  env = Warpt_getEnvLight();
  Warpt_setLightTevColorType(Warpt_guest<u8>(env), (u8 *)mpBodyModel, &tevStr);
  Warpt_modelUpdateDL((u8 *)mpBodyModel, 0);
  warptDrawLists(false);
  if (isHuta()) {
    if (m290 == MODE_BREAK_FIRE && mpLidModel2 != nullptr) {
      env = Warpt_getEnvLight();
      Warpt_settingTevStruct(Warpt_guest<u8>(env), 0, &m830, &tevStr);
      env = Warpt_getEnvLight();
      Warpt_setLightTevColorType(Warpt_guest<u8>(env), (u8 *)mpLidModel2, &tevStr);
      u32 model = gabi::ea((u8 *)mpLidModel2);
      u32 data = Warpt_guest<WarptModel>(model)->mModelData;
      f32 frame = mLidBrk.mFrameCtrl.mFrame;
      Warpt_brkEntry(&mLidBrk, Warpt_guest<u8>(data), frame);
      Warpt_modelUpdateDL((u8 *)mpLidModel2, 0);
      return 1;
    }
    if (mpLidBgW != nullptr && mpLidModel1 != nullptr) {
      warptDrawLists(true);
      env = Warpt_getEnvLight();
      Warpt_settingTevStruct(Warpt_guest<u8>(env), 0, &m830, &tevStr);
      env = Warpt_getEnvLight();
      Warpt_setLightTevColorType(Warpt_guest<u8>(env), (u8 *)mpLidModel1, &tevStr);
      Warpt_modelUpdateDL((u8 *)mpLidModel1, 0);
      warptDrawLists(false);
    }
  }
  return 1;
}
/* 023B9F74 */
void daObj_Warpt_c::getArg() {
  u32 parameters = mParameters;
  s16 z = home.angle.z;
  s16 x = home.angle.x;
  m2B4 = parameters & 15;
  if (!isSp()) {
    m29C = (parameters >> 12) & 255;
    m298 = (parameters >> 4) & 255;
    m2AC = (u8)x;
    m2B0 = ((u16)x >> 8);
    if (m2B4 == 1) {
      s8 room = stay();
      u32 s = save();
      Warpt_onSwitch(&Warpt_guest<WarptSave>(s)->mZone, (s32)m2AC, room);
    }
    s8 room = stay();
    u32 s = save();
    m2C6 = !Warpt_isSwitch(&Warpt_guest<WarptSave>(s)->mZone, (s32)m2AC, room);
  } else {
    m2A0 = (parameters >> 8) & 255;
    m2A8 = parameters >> 24;
    m2B8 = (parameters >> 4) & 15;
    m2A4 = (parameters >> 16) & 255;
    switch ((s32)m2B4) {
    case 2:
      m2C4 = 1;
      m2C5 = 0;
      break;
    case 3:
      m2C4 = 2;
      m2C5 = 1;
      break;
    case 4:
      m2C4 = 4;
      m2C5 = 2;
      break;
    }
    m2AC = (u8)x;
    m2B0 = (u16)x >> 8;
    m2BC = (u8)z;
    m2C0 = (u16)z >> 8;
    if (m2C0 != 255)
      onWarpBit(m2C4);
    m2C6 = !isWarpBit(m2C4);
  }
}
/* 023BA4E0 */
void daObj_Warpt_c::createInit() {
  m86C = m_def_smoke_prm_color[(u32)(s32)m2B4];
  m870 = m_def_smoke_env_color[(u32)(s32)m2B4];
  u32 model = gabi::ea((u8 *)mpBodyModel);
  cullMtx = model ? gabi::ea(&Warpt_guest<WarptModel>(model)->mBaseTransformMtx) : 0;
  Warpt_setCullBox(this, -80.0f, -0.0f, -80.0f, 80.0f, 160.0f, 80.0f);
  if (mpLidBgW != nullptr) {
    Warpt_guest<WarptBgW>(gabi::ea((u8*)mpLidBgW))->mCorrection = 0x024EE658;
    u32 p = play();
    Warpt_bgRegist(&Warpt_guest<WarptPlay>(p)->mBgS, (u8 *)mpLidBgW, this);
  }
  Warpt_guest<WarptBgW>(gabi::ea((u8*)mpBodyBgW1))->mCorrection = 0x024EE658;
  u32 p = play();
  Warpt_bgRegist(&Warpt_guest<WarptPlay>(p)->mBgS, (u8 *)mpBodyBgW1, this);
  modeProc(PROC_INIT_e, isHuta() ? 1 : 0);
  Warpt_guest<WarptBgW>(gabi::ea((u8*)mpBodyBgW2))->mRideCallback = 0x023B9E88;
  initCollision();
  setCollision();
  Warpt_setWall(&mAcchCir, 30.0f, 50.0f);
  Warpt_acchSet(&mAcch, &current.pos, &old.pos, this, 1, &mAcchCir, &speed, (u8 *)nullptr, (u8 *)nullptr);
  mAcch.mFlags = mAcch.mFlags | 0x40C;
  gravity = -6.5f;
  Warpt_posMoveF(this, (u8 *)nullptr);
  gabi::Local<cXyz> position;
  position->x = (f32)current.pos.x;
  position->y = (f32)current.pos.y;
  position->z = (f32)current.pos.z;
  s32 cooldown = 5;
  if (Warpt_checkPlayerInCircle(position.get(), 50.0f, 100.0f))
    cooldown = 30;
  m840 = cooldown;
  setMtx();
}
/* 023BA6F0 */
s32 daObj_Warpt_c::_create() {
  u32 flags = actor_condition;
  if (!(flags & 8)) {
    if (gabi::ea(this)) {
      Warpt_actorCtor(this);
      __vtbl = gc_tu.mActorVtable;
      Warpt_brkCtor(&mLidBrk);
      Warpt_statusBaseCtor(&mStts);
      Warpt_statusGameCtor(&mStts.__vtbl_gstts);
      mStts.__vtbl = gc_tu.mStatusBaseVtable;
      mStts.__vtbl_gstts = gc_tu.mStatusGameVtable;
      Warpt_collisionObjectCtor(&mCyl1);
      Warpt_guest<WarptShapeAttribute>(gabi::ea(mCyl1.mShapeAttr))->mBoundsVtable = gc_tu.mBoundsVtable;
      Warpt_guest<WarptShapeAttribute>(gabi::ea(mCyl1.mShapeAttr))->mVtable = gc_tu.mShapeBaseVtable;
      Warpt_cylinderGeometryCtor(&mCyl1.mCyl);
      mCyl1.__vtbl_hitinf = gc_tu.mCylinderVtable;
      Warpt_guest<WarptShapeAttribute>(gabi::ea(mCyl1.mShapeAttr))->mVtable = gc_tu.mCylinderShapeVtable;
      mCyl1.mCyl.__vtbl = gc_tu.mCylinderGeometryVtable;
      Warpt_collisionObjectCtor(&mCyl2);
      Warpt_guest<WarptShapeAttribute>(gabi::ea(mCyl2.mShapeAttr))->mVtable = gc_tu.mShapeBaseVtable;
      Warpt_guest<WarptShapeAttribute>(gabi::ea(mCyl2.mShapeAttr))->mBoundsVtable = gc_tu.mBoundsVtable;
      Warpt_cylinderGeometryCtor(&mCyl2.mCyl);
      mCyl2.mCyl.__vtbl = gc_tu.mCylinderGeometryVtable;
      Warpt_guest<WarptShapeAttribute>(gabi::ea(mCyl2.mShapeAttr))->mVtable = gc_tu.mCylinderShapeVtable;
      mCyl2.__vtbl_hitinf = gc_tu.mCylinderVtable;
      Warpt_acchCtor(&mAcch);
      mAcch.mBaseVtable = gc_tu.mAcchBaseVtable;
      mAcch.mVtable = gc_tu.mAcchVtable;
      mAcch.mObject = 1;
      mAcch.mPolyVtable = gc_tu.mAcchPolyVtable;
      Warpt_acchCirCtor(&mAcchCir);
      Warpt_followCtor(&m844, (u8 *)nullptr, (u8 *)nullptr);
      Warpt_followCtor(&m858, (u8 *)nullptr, (u8 *)nullptr);
      flags = actor_condition;
    }
    actor_condition = flags | 8;
  }
  s32 phase = Warpt_resLoad(&mPhase, "ltubw");
  if (phase == 4) {
    getArg();
    u32 heapSize = isRealHuta() ? 0x1F00 : 0xE20;
    if (!Warpt_entrySolidHeap(this, Warpt_guest<u8>(0x023B9CAC), heapSize))
      return 5;
    createInit();
  }
  return phase;
}
/* 023BA8C8 */
BOOL daObj_Warpt_c::_delete() {
  Warpt_resDelete(&mPhase, "ltubw");
  if (used(mpLidBgW)) {
    u32 p = play();
    Warpt_bgRelease(&Warpt_guest<WarptPlay>(p)->mBgS, (u8 *)mpLidBgW);
  }
  if (used(mpBodyBgW2)) {
    u32 p = play();
    Warpt_bgRelease(&Warpt_guest<WarptPlay>(p)->mBgS, (u8 *)mpBodyBgW2);
  }
  if (used(mpBodyBgW1)) {
    u32 p = play();
    Warpt_bgRelease(&Warpt_guest<WarptPlay>(p)->mBgS, (u8 *)mpBodyBgW1);
  }
  Warpt_removeFollow(&m844);
  Warpt_removeFollow(&m858);
  return 1;
}
/* 023BA8C4 */
s32 daObj_WarptCreate(daObj_Warpt_c *self) {
  return self->_create();
}
/* 023BA9A4 */
BOOL daObj_WarptDelete(daObj_Warpt_c *self) {
  return self->_delete();
}
/* 023BB070 */
BOOL daObj_WarptExecute(daObj_Warpt_c *self) {
  return self->_execute();
}
/* 023BB1F8 */
BOOL daObj_WarptDraw(daObj_Warpt_c *self) {
  return self->_draw();
}
/* 023BBC28 */
BOOL daObj_WarptIsDelete(daObj_Warpt_c *self) {
  return 1;
}

// HD: compiler-generated, gamma conversion and zero-argument weak init routines.
/* 023BA9A8 */
// HD: gamma-to-linear particle colors retain clamping and ftoi truncation.
static f32 warptLinear(u32 value) {
  f32 ratio = (f32)value / 255.0f;
  f32 result = Warpt_pow(ratio, 2.2f);
  if (result < 0.0f)
    result = 0.0f;
  else if (result > 1.0f)
    result = 1.0f;
  return result;
}
/* 023BB1FC */
static void warptColor(u8 *color) {
  u32 a = gabi::ea(color);
  f32 value = warptLinear(Warpt_guest<WarptColor>(a)->r);
  u8 next = Warpt_guest<WarptColor>(a)->g;
  Warpt_guest<WarptColor>(a)->r = (u8)gabi::ftoi(value * 255.0f);
  value = warptLinear(next);
  u8 last = Warpt_guest<WarptColor>(a)->b;
  Warpt_guest<WarptColor>(a)->g = (u8)gabi::ftoi(value * 255.0f);
  value = warptLinear(last);
  Warpt_guest<WarptColor>(a)->b = (u8)gabi::ftoi(value * 255.0f);
}
/* 023BBC30 */
static void modeEventWarpInit() { }
/* 023BBC34 */
static void modeEventOpenInit() { }
/* 023BBCE0 */
static void SafeString_assureTerminationImpl() { }
/* 023BBC14 */
static void warptHioDelete(u8 *self, s32 flags) {
  if (self && (flags & 1))
    Warpt_delete(self);
}
/* 023BBC38 */
static void warptDestructor(daObj_Warpt_c *self, s32 flags) {
  if (self) {
    Warpt_circleDtor(self->mAcchCir.mBase, 2);
    self->mAcch.mPolyVtable = gc_tu.mAcchPolyVtable;
    self->mAcch.mVtable = gc_tu.mAcchVtable;
    Warpt_acchDtor(&self->mAcch, 0);
    Warpt_cylinderDtor(&self->mCyl2, 2);
    Warpt_cylinderDtor(&self->mCyl1, 2);
    Warpt_statusDtor(&self->mStts, 2);
    Warpt_actorDtor(self, 0);
    if (flags & 1)
      Warpt_delete(self);
  }
}
/* 023BBA6C */
static u8 *warptHioConstruct(u8 *self) {
  if (!self)
    self = Warpt_new(0x1C);
  if (self) {
    u32 a = gabi::ea(self);
    Warpt_guest<daObj_Warpt_HIO_c>(a)->mVtable = gc_tu.mHioVtable;
    Warpt_guest<daObj_Warpt_HIO_c>(a)->m0C = 30.0f;
    Warpt_guest<daObj_Warpt_HIO_c>(a)->m10 = -2.5f;
    Warpt_guest<daObj_Warpt_HIO_c>(a)->m08 = 150.0f;
    Warpt_guest<daObj_Warpt_HIO_c>(a)->m04 = 0;
    Warpt_guest<daObj_Warpt_HIO_c>(a)->m05 = 0;
    Warpt_guest<daObj_Warpt_HIO_c>(a)->m06 = 0;
    if (!l_colorGuard) {
      l_colorGuard = 1;
      Warpt_copyMemory(&l_savedColor, Warpt_guest<u8>(gc_tu.mDefaultColor), 4);
    }
    Warpt_guest<daObj_Warpt_HIO_c>(a)->m14 = l_savedColor;
    if (!l_colorGuard) {
      l_colorGuard = 1;
      Warpt_copyMemory(&l_savedColor, Warpt_guest<u8>(gc_tu.mDefaultColor), 4);
    }
    Warpt_guest<daObj_Warpt_HIO_c>(a)->m18 = l_savedColor;
  }
  return self;
}
/* 023BBB74 */
static void __sinit_d_a_obj_warpt_cpp() {
  for (u32 i = 0; i < 4; i++)
    l_headerWords[i] = 0;
  Warpt_registerDestructor(Warpt_guest<u8>(gc_tu.mHeaderLink0));
  l_headerValues[0] = l_headerFloats[0];
  l_headerValues[1] = l_headerFloats[1];
  Warpt_headerGuardCtor(&l_headerGuard);
  Warpt_registerDestructor(Warpt_guest<u8>(gc_tu.mHeaderLink1));
  Warpt_headerByteCtor(&l_headerByte);
  Warpt_registerDestructor(Warpt_guest<u8>(gc_tu.mHeaderLink2));
  warptHioConstruct(reinterpret_cast<u8*>(&l_HIO));
}

/* WWHD glue
str 100337C0 "ltubw"
str 100337A0 "WARPT_OPEN"
str 10033790 "DEFAULT_WARP"
str 1003375C "Warpt"
data mode_tbl 10033688
data m_event_reg 100337C8
data m_def_smoke_prm_color 100337D4
data m_def_smoke_env_color 100337E8
data l_headerFloats 100337B8
data l_resourceControl 101F4F28 nocheck
data l_saveInfo 101F84DC nocheck
data l_stayNo 1047E6C8 nocheck
data l_colorGuard 101FDC64 nocheck
data l_savedColor 101FEC1D nocheck
data l_HIO 1046C9FC nocheck
data l_matrix 1048D0CC nocheck
data l_headerWords 1046C9EC nocheck
data l_headerValues 1046C9E0 nocheck
data l_headerGuard 1046C9E8 nocheck
data l_headerByte 1046C9E9 nocheck
data l_opaqueList 104B4634 nocheck
data l_translucentList 104B4638 nocheck
ext 025200D4 u32 Warpt_getPlay()
ext 025BA0C0 s32 Warpt_isSwitch(void*, u32, u32)
ext 025B8BB0 u32 Warpt_getEventReg(void*, u32)
ext 025B8AF4 void Warpt_setEventReg(void*, u32, u32)
ext 025E19CC void Warpt_seStart(u32, u32)
ext 02587EFC void Warpt_setNextStageBySclsNum(u32, u32)
ext 025E1988 void Warpt_seStartGlobal(u32)
ext 025B9E38 void Warpt_onSwitch(void*, u32, u32)
ext 02515F14 void Warpt_statusInit(void*, u32, u32, void*)
ext 02516518 void Warpt_setCylinder(void*, void*)
ext 020182E0 void Warpt_cylinderSetC(void*, void*)
ext 0200E240 void Warpt_collisionSet(void*, void*)
ext 02520540 s32 Warpt_getReverb(u32)
ext 025E1A40 void Warpt_seStartCurrent(u32, void*, u32, u32)
ext 025E742C void Warpt_animationPlay(void*)
ext 0211D2F8 s32 Warpt_calcTimerInt(void*)
ext 020087EC void Warpt_bgRelease(void*, void*)
ext 02516300 u32 Warpt_getTargetHit(void*)
ext 026066C4 u8* Warpt_getObjectRes(void*, void*, u32)
ext 0273AA24 void Warpt_assert(void*, u32, void*)
ext 025E38E0 u8* Warpt_modelCreate(void*, u32, u32)
ext 024F23F4 u8* Warpt_bgNew(void*)
ext 0200A030 s32 Warpt_bgSet(void*, void*, u32, void*)
ext 025E8154 s32 Warpt_brkInit(void*, void*, void*, u32, u32, f32, u32, u32, u32, u32)
ext 028F4560 f32 Warpt_pow(f32, f32)
ext 0254457C s32 Warpt_eventEndCheck(void*, const char*)
ext 025204C8 void Warpt_resDelete(void*, const char*)
ext 0273AF40 void Warpt_delete(void*)
ext 02018034 void Warpt_circleDtor(void*, u32)
ext 024EFD9C void Warpt_acchDtor(void*, u32)
ext 02515A70 void Warpt_cylinderDtor(void*, u32)
ext 02515860 void Warpt_statusDtor(void*, u32)
ext 025D50BC void Warpt_actorDtor(void*, u32)
ext 025D6894 s32 Warpt_searchPlayerAngleY(void*, void*)
ext 025881A4 s32 Warpt_checkPlayerInCircle(void*, f32, f32)
ext 025A847C void Warpt_setParticle(void*, u32, u32, void*, void*, void*, u32, void*, u32, void*, void*, u32)
ext 025A5AC8 void Warpt_followRemove(void*)
ext 028E93CC void Warpt_transS(void*, f32, f32, f32)
ext 025F1C28 void Warpt_YrotM(void*, u32)
ext 025F2518 void Warpt_scaleM(f32, f32, f32)
ext 028E90D4 void Warpt_matrixCopy(void*, void*)
ext 024F43DC void Warpt_bgMove(void*)
ext 024EEA6C void Warpt_bgRegist(void*, void*, void*)
ext 025D674C void Warpt_setCullBox(void*, f32, f32, f32, f32, f32, f32)
ext 024EFF44 void Warpt_setWall(void*, f32, f32)
ext 024F06B4 void Warpt_acchSet(void*, void*, void*, void*, u32, void*, void*, void*, void*)
ext 025D6870 void Warpt_posMoveF(void*, void*)
ext 025D4ED0 void Warpt_actorCtor(void*)
ext 025E80D0 void Warpt_brkCtor(void*)
ext 0200BD2C void Warpt_statusBaseCtor(void*)
ext 02515DA0 void Warpt_statusGameCtor(void*)
ext 02515FB8 void Warpt_collisionObjectCtor(void*)
ext 02018590 void Warpt_cylinderGeometryCtor(void*)
ext 024F0474 void Warpt_acchCtor(void*)
ext 024EFE94 void Warpt_acchCirCtor(void*)
ext 025A5894 void Warpt_followCtor(void*, void*, void*)
ext 02520460 s32 Warpt_resLoad(void*, const char*)
ext 025D63E8 s32 Warpt_entrySolidHeap(void*, void*, u32)
ext 0273AD10 u8* Warpt_new(u32)
ext C000A848 void Warpt_copyMemory(void*, void*, u32)
ext 028F026C void Warpt_registerDestructor(void*)
ext 028ED6F8 void Warpt_headerGuardCtor(void*)
ext 028EAB2C void Warpt_headerByteCtor(void*)
ext 02555D0C u32 Warpt_getEnvLight()
ext 025626A4 void Warpt_settingTevStruct(void*, u32, void*, void*)
ext 02562F5C void Warpt_setLightTevColorType(void*, void*, void*)
ext 025E2DE0 void Warpt_modelUpdateDL(void*, u32)
ext 025E83FC void Warpt_brkEntry(void*, void*, f32)
ext 02515E50 void Warpt_statusMove(void*)
ext 02312E54 void Warpt_hitEffHibana(void*, void*)
ext 024F08A8 void Warpt_acchCrrPos(void*, void*)
ext 022ED850 void Warpt_calcTimerChar(void*)
ext 025D77DC void Warpt_orderOtherEvent(void*, const char*, u32, u32)
ext 02542D88 s32 Warpt_getMyStaffId(void*, const char*, u32, u32)
ext 02544830 u32 Warpt_getMyNowCutName(void*, u32)
ext 02543280 void Warpt_cutEnd(void*, u32)
*/

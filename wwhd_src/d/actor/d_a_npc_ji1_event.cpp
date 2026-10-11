/**
 * d_a_npc_ji1_event.cpp (WWHD)
 * NPC - Orca: event functions (eventAction, privateCut and the evn_* cuts),
 * initPos, createItem, particles.
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout
 * and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_event.h"

static daNpc_Ji1_HIO_c l_hio;
static Ji1EventMessagePointerHD l_messageManager;
static Ji1EventWordHD l_msgId;
static Ji1EventLifeHD l_gameLife;
static Ji1EventWordHD l_hitScaleGuard;
static cXyz l_hitScale;
static Ji1EventAngleHD l_zeroAngle;
static Ji1EventActionNamesHD ActionNames;
static Ji1EventCutNamesHD cut_name_tbl;

enum { fpcNm_TSUBO_e = 0x1C5 };
enum {
  JA_SE_CV_JI_FUTTOBI = 0x4932,
  JA_SE_CM_JI_DAMAGE = 0x5824,
  JA_SE_VS_JI_ENDING = 0x85C
};

/* setAction(p, arg) with a PTMF read from memory (eventAction:
 * setAction(field_0x2C8, 0)) */
static inline void ji1_setActionP(daNpc_Ji1_c *t, Ji1EventProcHD &src,
                                  void *arg) {
  s16 ad = src.d;
  s16 ai = src.i;
  u32 af = src.f;
  s16 mi = t->mAction.i;
  bool eq;
  if (mi == ai) {
    eq = mi == 0 || (t->mAction.d == ad && t->mAction.f == af);
  } else {
    eq = false;
  }
  if (!eq) {
    if (mi != 0) {
      t->field_0xC78 = -1;
      ptmf_invoke(t->mAction, t, arg);
    }
    ptmf_copy(t->field_0x2BC, t->mAction);
    t->mAction.d = ad;
    t->mAction.i = ai;
    t->mAction.f = af;
    t->field_0xC78 = 0;
    ptmf_invoke(t->mAction, t, arg);
  }
}

/* 0224FBAC */
u32 daNpc_Ji1_c::setParticle(int max, f32 rate, f32 spread) {

  dtParticle();
  if (mSmokeCb.mpEmitter.get() == nullptr) {
    s8 roomNo = fopAcM_GetRoomNo(this);
    /* dComIfGp_particle_setToon(ID_AK_JT_ELEMENTSMOKE00, &current.pos, 0, 0,
     * 0xB9, &mSmokeCb, roomNo) */
    JPABaseEmitter *emitter = dPa_control_set(
        dComIfGp_getParticle(), 2, ID_AK_JT_ELEMENTSMOKE00, &current.pos,
        nullptr, nullptr, 0xB9, (dPa_levelEcallBack *)&mSmokeCb, roomNo,
        nullptr, nullptr, nullptr);
    if (emitter) {
      auto *e = reinterpret_cast<Ji1EventEmitterHD *>(emitter);
      e->mRate = rate;
      e->mSpread = spread;
      e->mInitialVelRndm = 1.5f;
      e->mMaxFrame = max;
      return true;
    }
  }
  return false;
}

/* 0225B720 */
u32 daNpc_Ji1_c::setParticleAT(int max, f32 rate, f32 spread) {

  if (mSmokeCbAT.mpEmitter.get() == nullptr) {
    s8 roomNo = fopAcM_GetRoomNo(this);
    JPABaseEmitter *emitter = dPa_control_set(
        dComIfGp_getParticle(), 2, ID_AK_JT_ELEMENTSMOKE00, &field_0x320,
        nullptr, nullptr, 0xB9, (dPa_levelEcallBack *)&mSmokeCbAT, roomNo,
        nullptr, nullptr, nullptr);
    if (mSmokeCbAT.mpEmitter.get()) {
      auto *e = reinterpret_cast<Ji1EventEmitterHD *>(emitter);
      /* setGlobalParticleScale(2, 2, 2) */
      e->mGlobalPtclScl.x = 2.0f;
      e->mGlobalPtclScl.y = 2.0f;
      e->mGlobalPtclScl.z = 2.0f;
      e->mVolumeSweep = 0.15f;
      e->mRate = rate;
      e->mSpread = spread;
      e->mInitialVelRndm = 1.5f;
      e->mMaxFrame = max;
      return true;
    }
  }
  return false;
}

/* 02255358 */
void daNpc_Ji1_c::setHitParticle(cXyz *param_1, u32 param_2) {

  cXyz *pRVec = &field_0x7E0.mGObjTg.mRVec;
  // HD: preserve the angle and scale layout of the native temporary block.
  gabi::Local<Ji1EventHitParticleArgsHD> loc;
  cXyz *scale = &loc->scale;
  csXyz *angle = &loc->angle;
  scale->set(0.75f, 0.75f, 0.75f);
  angle->y = cM_atan2s(pRVec->x, pRVec->z);
  angle->x = cM_atan2s(pRVec->z, pRVec->y);
  if (param_1) {
    scale->copy(*param_1);
  }
  dComIfGp_particle_set(ID_AK_JN_OK, field_0x7E0.GetTgHitPosP(), angle, scale);
  dComIfGp_particle_set(ID_AK_JN_CRITICALHITFLASH, field_0x7E0.GetTgHitPosP(),
                        nullptr, scale);
  ji1_seStart(this, JA_SE_CM_JI_DAMAGE, 0);
  ji1_seStart(this, param_2, 0);
  gabi::Local<cXyz> pos;
  cXyz *hit = field_0x7E0.GetTgHitPosP();
  pos->z = (f32)hit->z;
  pos->y = (f32)hit->y;
  pos->x = (f32)hit->x;
  ji1EventSwordFlushHD(pos, 0);
}

/* 022513D8 */
s32 daNpc_Ji1_c::getEventActionNo(int staffIdx) {

  /* static char* ActionNames[7] = {"00_dummy", ...} at 0x101BEBA4 */
  return dComIfGp_evmng_getMyActIdx(staffIdx, ActionNames.mNames, 7, FALSE, 0);
}

/* 022542A0 */
static void *ji1EventInitPosObjectHD(void *pActor, void *pData) {

  if (fopAc_IsActor(pActor) && pActor != nullptr &&
      fpcM_GetName(pActor) == fpcNm_TSUBO_e) {
    /* daTsubo::Act_c::pos_init() (inline; m678 at 0x794) */
    auto *a = reinterpret_cast<Ji1EventTsuboHD *>(pActor);
    if (a->m678 == 2) {
      a->current.pos.set(a->home.pos.x, a->home.pos.y, a->home.pos.z);
      a->current.angle.x = (s16)a->home.angle.x;
      a->current.angle.y = (s16)a->home.angle.y;
      a->current.angle.z = (s16)a->home.angle.z;
      a->shape_angle.x = (s16)a->home.angle.x;
      a->shape_angle.y = (s16)a->home.angle.y;
      a->shape_angle.z = (s16)a->home.angle.z;

      ji1EventAngleCopyHD(&a->m688, &l_zeroAngle);
      ji1EventAngleCopyHD(&a->m68A, &l_zeroAngle);
      ji1EventAngleCopyHD(&a->m68C, &l_zeroAngle);
      ji1EventAngleCopyHD(&a->m68E, &l_zeroAngle);
      ji1EventAngleCopyHD(&a->m690, &l_zeroAngle);
      ji1EventAngleSetHD(&a->m692, a->current.angle.y);
      ji1EventIdentityHD(&a->mPoseMtx);
    }
  }
  return 0;
}

// HD: the glue tool cannot mark a static class method yet; preserve its public
// API.
void *daNpc_Ji1_c::initPosObject(void *actor, void *data) {
  return ji1EventInitPosObjectHD(actor, data);
}

/* 02254390 */
void daNpc_Ji1_c::initPos(int param_1) {

  if (param_1 >= 0) {
    setAnm(param_1, 0.0f, 0);
  }
  fopAcIt_Judge(gabi::guest_fn((const void *)ji1EventInitPosObjectHD),
                this); /* fopAcM_Search */
  old.pos.copy(field_0xD28);
  current.pos.copy(field_0xD28);
  gabi::Local<csXyz> a;
  csXyz *r = ji1EventConstructAnglesHD(a.get(), (s16)home.angle.y, (s16)0,
                                       (s16)0); /* csXyz::csXyz returns this */
  current.angle.x = (s16)r->x;
  current.angle.y = (s16)r->y;
  current.angle.z = (s16)r->z;
}

// HD: present items use the current room (GameCube demo used -1).
/* 02258574 */
void daNpc_Ji1_c::createItem() {

  u8 itemNo;
  if (field_0xD7C) {
    itemNo = dItemNo_SWORD_e;
  } else if (field_0xD7B == 1) {
    itemNo = dItemNo_HURRICANE_SPIN_e;
  } else if (dComIfGs_getEventReg(0xD003) == 1) {
    itemNo = dItemNo_PURPLE_RUPEE_e;
  } else if (dComIfGs_getEventReg(0xD003) == 2) {
    itemNo = dItemNo_ORANGE_RUPEE_e;
  } else if (field_0xD70 >= l_hio.field_0x60[3] &&
             dComIfGs_isEventBit(0x0F10)) {
    itemNo = dItemNo_SILVER_RUPEE_e;
  } else {
    itemNo = dItemNo_HEART_PIECE_e;
    dComIfGs_onEventBit(0x0F10);
  }
  u32 itemPID = ji1EventPresentItemHD(&current.pos, itemNo, 0, -1,
                                      current.roomNo, nullptr, nullptr);
  if (itemPID != fpcM_ERROR_PROCESS_ID_e) {
    dComIfGp_event_setItemPartnerId(itemPID);
  }
  mCreateItemNo = itemNo;
}

/* 022596CC */
BOOL daNpc_Ji1_c::eventAction(void *arg) {

  if (field_0xC78 == 0) {
    s32 staffIdx =
        dComIfGp_evmng_getMyStaffId(mEventCut.getActorName(), nullptr, 0);
    if (!eventInfo_checkCommandDemoAccrpt(this) && staffIdx == -1) {
      fopAcM_orderOtherEventId(this, mEventIdx[field_0xC84], 0xFF, 0xFFFF, 0,
                               1);
      eventInfo_onCondition(this, 2 /* dEvtCnd_UNK2_e */);
      return false;
    }
    field_0xC78 += 1;
  } else if (field_0xC78 == -1) {
    field_0xC84 = 0x12;
  } else {
    s32 staffIdx =
        dComIfGp_evmng_getMyStaffId(mEventCut.getActorName(), nullptr, 0);
    privateCut();
    AnimeControlToWait();
    if (dComIfGp_evmng_endCheck(mEventIdx[field_0xC84])) {
      dComIfGp_event_reset();
      ji1_setActionP(this, field_0x2C8, nullptr);
    } else if (staffIdx == -1) {
      ji1_setActionP(this, field_0x2C8, nullptr);
    }
  }
  return true;
}

/* 02258508 */
u32 daNpc_Ji1_c::evn_init_pos_init(int staffIdx) {

  be<s32> *data = dComIfGp_evmng_getMyIntegerP(staffIdx, "AnmNo");
  u32 value = 0;
  if (data) {
    value = s32(*data);
  }
  initPos(value);
  return 1;
}

/* 022582E4 */
u32 daNpc_Ji1_c::evn_setAnm_init(int staffIdx) {

  be<s32> *iData = dComIfGp_evmng_getMyIntegerP(staffIdx, "AnmNo");
  be<f32> *fData = dComIfGp_evmng_getMyFloatP(staffIdx, "hokan");
  if (iData) {
    int iVal = s32(*iData);
    f32 fVal = 0.0f;
    if (fData) {
      fVal = f32(*fData);
    }
    if (field_0xD84 != Ji1Event_HARPOON_HELD && iVal == Ji1Event_BCK_KAMAE) {
      iVal = Ji1Event_BCK_WAIT02;
    }
    setAnm(iVal, fVal, 0);
  }
  return 1;
}

// HD: the message manager replaces the GameCube message-box pointer.
/* 022583A4 */
u32 daNpc_Ji1_c::evn_talk_init(int staffIdx) {

  be<s32> *pMsgNo = dComIfGp_evmng_getMyIntegerP(staffIdx, "MsgNo");
  be<s32> *pEndMsgNo = dComIfGp_evmng_getMyIntegerP(staffIdx, "EndMsgNo");
  l_msgId = fpcM_ERROR_PROCESS_ID_e;
  if (pMsgNo) {
    u32 no = u32(s32(*pMsgNo));
    mMsgNo = no;
    if (no == 0x9A4) {
      if (field_0xC38 == 2) {
        mMsgNo = 0x9A9;
      } else if (mCreateItemNo == 7) {
        mMsgNo = 0x9A6;
      }
    } else if (no == 0x9A1) {
      if (field_0xC38 == 2) {
        mMsgNo = 0x9A7;
      }
    } else if (no == 0x9B7) {
      if (ptmf_eq(field_0x2C8, ACT_kaitenwaitAction)) {
        mMsgNo = 0x95E;
      } else if (ptmf_eq(field_0x2C8, ACT_teachAction)) {
        mMsgNo = 0x95D;
      }
    }
  } else {
    mMsgNo = 0;
  }
  if (pEndMsgNo) {
    mEndMsgNo = u32(s32(*pEndMsgNo));
  } else {
    mEndMsgNo = 0;
  }
  return 1;
}

/* The two talk cuts are the same code. privateCut calls 02258CB0 for TALKMSG
 * and 02259178 for CONTINUETALK, so 02258CB0 is evn_talk and 02259178
 * evn_continue_talk (the matcher has them swapped). HD: the message manager
 * replaces l_msg; a failed messageSet skips demoMsgFlagOn. */
static inline u32 ji1_talk_proc(daNpc_Ji1_c *t) {
  Ji1EventMessageManagerHD *mgr = l_messageManager;
  if (l_msgId == fpcM_ERROR_PROCESS_ID_e) {
    u32 id = ji1EventMessageSetHD(mgr, t->mMsgNo, &t->eyePos);
    l_msgId = id;
    if (id != fpcM_ERROR_PROCESS_ID_e) {
      ji1EventDemoFlagOnHD();
    }
    return false;
  }
  t->setAnimFromMsgNo(t->mMsgNo);
  if (ji1EventMessageStatusHD(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
    /* next_msgStatus's r3 goes to the status setter as is (no u16 re-extension)
     */
    ji1EventSetMessageStatusHD(mgr, ji1EventNextStatusHD(t, &t->mMsgNo));
    if (ji1EventMessageStatusHD(mgr) == fopMsgStts_MSG_CONTINUES_e) {
      ji1EventMessageSetHD(mgr, t->mMsgNo, nullptr);
    } else if (ji1EventMessageStatusHD(mgr) == fopMsgStts_MSG_ENDS_e) {
      if (t->mMsgNo == 0x9BA) {
        t->field_0xD7B = 0;
        t->field_0xD6C = 0;
      } else if (t->mMsgNo == 0x95A) {
        t->field_0xD7C = 0;
        t->field_0xD6C = 0;
      }
    }
    return false;
  }
  if (ji1EventMessageStatusHD(mgr) == fopMsgStts_BOX_CLOSED_e) {
    ji1EventSetMessageStatusHD(mgr, fopMsgStts_MSG_DESTROYED_e);
    l_msgId = fpcM_ERROR_PROCESS_ID_e;
    return true;
  }
  if ((ji1EventMessageStatusHD(mgr) == fopMsgStts_BOX_OPENING_e ||
       ji1EventMessageStatusHD(mgr) == fopMsgStts_MSG_TYPING_e) &&
      t->mMsgNo == t->mEndMsgNo) {
    t->mEndMsgNo = 0;
    return true;
  }
  return false;
}

// HD: (matcher: evn_continue_talk)
/* 02258CB0 */
u32 daNpc_Ji1_c::evn_talk() { return ji1_talk_proc(this); }

// HD: (matcher: evn_talk)
/* 02259178 */
u32 daNpc_Ji1_c::evn_continue_talk() { return ji1_talk_proc(this); }

/* 02258BE8 */
u32 daNpc_Ji1_c::evn_continue_talk_init(int staffIdx) {

  be<s32> *data = dComIfGp_evmng_getMyIntegerP(staffIdx, "EndMsgNo");
  if (data) {
    mEndMsgNo = u32(s32(*data));
  } else {
    mEndMsgNo = 0;
  }
  return 1;
}

/* 02258C4C */
u32 daNpc_Ji1_c::evn_setAngle_init(int staffIdx) {

  be<s32> *data = dComIfGp_evmng_getMyIntegerP(staffIdx, "angle");
  if (data) {
    current.angle.y = (s16)s32(*data);
  }
  return 1;
}

/* 02258794 */
u32 daNpc_Ji1_c::evn_sound_proc_init(int staffIdx) {

  be<s32> *data = dComIfGp_evmng_getMyIntegerP(staffIdx, "prm");
  if (data) {
    switch (u32(s32(*data))) {
    case 0:
      ji1EventBgmStopHD(45);
      break;
    case 1:
      ji1EventBgmStartHD(0x80000018 /* JA_BGM_HOUSE_G */);
      break;
    case 2:
      ji1_seStart(this, JA_SE_VS_JI_ENDING, 0);
      break;
    case 3: {
      gabi::Local<cXyz> v;
      v->set(0.0f, 1.0f, 0.0f);
      dComIfGp_getVibration_StartShock(5, -0x11, v);
      break;
    }
    }
  }
  return 1;
}

/* 02258894 */
u32 daNpc_Ji1_c::evn_head_swing_init(int staffIdx) {

  be<s32> *pData = dComIfGp_evmng_getMyIntegerP(staffIdx, "prm");
  u32 value;
  if (!pData) {
    value = 0;
  } else {
    value = u32(s32(*pData));
  }
  switch (value) {
  case 0:
    ji1EventSwingHorizontalHD(mHeadAnm, 2, 0x1000, 0x1000, 1);
    break;
  case 1:
    ji1EventSwingVerticalHD(mHeadAnm, 2, 0x1000, 0x800, 1);
    break;
  case 2:
    ji1EventSwingVerticalHD(mHeadAnm, 1, 0x1400, 0x1000, 1);
    break;
  }
  return true;
}

/* 0225896C */
u32 daNpc_Ji1_c::evn_harpoon_proc_init(int staffIdx) {

  be<s32> *pData = dComIfGp_evmng_getMyIntegerP(staffIdx, "prm");
  u32 value;
  if (!pData) {
    value = 0;
  } else {
    value = u32(s32(*pData));
  }
  switch (value) {
  case 0:
    harpoonRelease(nullptr);
    break;
  case 1:
    field_0xD84 = Ji1Event_HARPOON_HIDDEN;
    break;
  case 2:
    field_0xD84 = Ji1Event_HARPOON_HELD;
    break;
  }
  return true;
}

/* 02258A14 */
u32 daNpc_Ji1_c::evn_RollAtControl_init(int staffIdx) {

  fopAc_ac_c *player = daPy_getPlayerActorClass();
  field_0xD18 = 400.0f;
  s16 a = cLib_targetAngleY(&current.pos, &player->current.pos);
  field_0xD16 = (s16)(a + 0x2000);
  field_0xC30 = 30;
  field_0xD68 = 0;
  return 1;
}

/* 02258E6C */
u32 daNpc_Ji1_c::evn_RollAtControl() {

  // HD: GHS stores the local-static guard before the x, z, y initialization.
  cXyz *hit_scale = &l_hitScale;
  if (l_hitScaleGuard == 0) {
    l_hitScaleGuard = 1;
    hit_scale->x = 1.25f;
    hit_scale->z = 1.25f;
    hit_scale->y = 1.25f;
  }
  field_0xD16 = (s16)(field_0xD16 + 0x540);
  cLib_chaseF(&field_0xD18, 0.0f, 1.0f);
  f32 z = current.pos.z;
  f32 y = current.pos.y;
  f32 x = current.pos.x;
  field_0xD1C.z = z;
  field_0xD1C.x = x;
  field_0xD1C.y = y;
  f32 d18 = field_0xD18;
  field_0xD1C.x = gabi::fmadds(d18, cM_ssin(field_0xD16), x);
  field_0xD1C.z = gabi::fmadds(d18, cM_scos(field_0xD16), z);
  dComIfGp_evmng_setGoal(&field_0xD1C);
  field_0x6B0.SetC(&current.pos);
  dComIfG_Ccsp_Set(&field_0x6B0);
  field_0x7E0.SetC(&current.pos);
  dComIfG_Ccsp_Set(&field_0x7E0);
  if (field_0xC30 == 0xA) {
    setAnm(Ji1Event_BCK_DAMAGE, 0.0f, 0);
    BackSlideInit();
    setHitParticle(hit_scale, JA_SE_CV_JI_FUTTOBI);
  }
  if (mAnimation == Ji1Event_BCK_DAMAGE) {
    if (mpOrcaMorf->getFrame() < 10.0f) {
      fopAc_ac_c *player = daPy_getPlayerActorClass();
      gabi::Local<cXyz> posDiff;
      cXyz_mi(&player->current.pos, posDiff, &current.pos);
      cLib_addCalcAngleS2(&current.angle.y, cM_atan2s(posDiff->x, posDiff->z),
                          2, 0x2000);
    } else {
      if (mpOrcaMorf->checkFrame(20.0f) && field_0xD68++ < 2) {
        setHitParticle(hit_scale, JA_SE_CV_JI_FUTTOBI);
        BackSlideInit();
        setAnm(Ji1Event_BCK_DAMAGE, 4.0f, 1);
      }
    }
    BackSlide(120.0f, 10.0f);
  }
  if (field_0xC30 > 0) {
    field_0xC30 -= 1;
  } else {
    return true;
  }
  return false;
}

/* 02258A78 */
u32 daNpc_Ji1_c::evn_game_mode_init(int staffIdx) {

  be<s32> *data = dComIfGp_evmng_getMyIntegerP(staffIdx, "prm");
  u32 value;
  if (data) {
    value = u32(s32(*data));
  } else {
    value = 0;
  }
  switch (value) {
  case 0:
    dComIfGp_endMiniGame(1 << 5); /* dComIfGp_endMiniGame(6) */
    break;
  case 1:
    dComIfGp_startMiniGame(6, 1 << 5);
    l_gameLife = 3;
    /* fallthrough */
  case 2:
    dComIfGp_endMiniGame(1 << 1); /* dComIfGp_endMiniGame(2) */
    break;
  }
  return 1;
}

/* 0225911C */
u32 daNpc_Ji1_c::evn_turn_to_player() {

  fopAc_ac_c *player = daPy_getPlayerActorClass();
  s16 target = cLib_targetAngleY(&current.pos, &player->current.pos);
  if (cLib_addCalcAngleS(&current.angle.y, target, 4, 0x1000, 0x100) == 0) {
    return 1;
  } else {
    return 0;
  }
}

/* 02258B48 */
u32 daNpc_Ji1_c::evn_hide_init(int staffIdx) {

  fopAc_ac_c *player = daPy_getPlayerActorClass();
  be<s32> *pData = dComIfGp_evmng_getMyIntegerP(staffIdx, "prm");
  u32 value;
  if (!pData) {
    value = 0;
  } else {
    value = u32(s32(*pData));
  }
  mHide = value & 1;
  /* daPy_py_c::onPlayerNoDraw / offPlayerNoDraw: mModeFlg (+0x3B8) bit
   * 0x08000000 */
  be<u32> &flg = reinterpret_cast<Ji1EventPlayerHD *>(player)->mNoResetFlg0;
  if (value & 2) {
    flg |= 0x08000000u;
  } else {
    flg &= ~0x08000000u;
  }
  return true;
}

/* 022595EC */
void daNpc_Ji1_c::AnimeControlToWait() {

  mDoExt_McaMorf *morf = mpOrcaMorf;
  u8 play_mode = morf->getPlayMode();
  if (play_mode == J3DFrameCtrl::EMode_NONE) {
    if (morf->checkFrame(morf->getEndFrame() - 1.0f)) {
      if (isAttackAnim() || isGuardAnim()) {
        setAnm(Ji1Event_BCK_KAMAE, 4.0f, 0);
      } else if (isItemWaitAnim()) {
        setAnm(Ji1Event_BCK_WAIT01, 4.0f, 0);
      }
    }
  }
}

// HD: (TALKMSG -> 02258CB0, CONTINUETALK -> 02259178: see evn_talk)
/* 02259334 */
u32 daNpc_Ji1_c::privateCut() {

  s32 staffIdx =
      dComIfGp_evmng_getMyStaffId(mEventCut.getActorName(), nullptr, 0);
  if (staffIdx == -1) {
    return false;
  }
  enum {
    ACT_SETANM,
    ACT_TALKMSG,
    ACT_INITPOS,
    ACT_CREATEITEM,
    ACT_SOUND,
    ACT_HEADSWING,
    ACT_HARPOON,
    ACT_ROLLAT_CNT,
    ACT_GAME_MODE,
    ACT_TURN_TO_PLAYER,
    ACT_HIDE,
    ACT_CONTINUETALK,
    ACT_SETANGLE,
  };
  /* static char* cut_name_tbl[13] at 0x101BEBC0 */
  s32 actIdx =
      dComIfGp_evmng_getMyActIdx(staffIdx, cut_name_tbl.mNames, 13, TRUE, 0);
  dEvent_manager_c *mgr = dComIfGp_getPEvtManager();
  if (actIdx == -1) {
    ji1EventCutEndHD(mgr, staffIdx); /* cutEnd */
  } else {
    if (ji1EventIsAdvanceHD(mgr, staffIdx) /* getIsAddvance */) {
      switch (actIdx) {
      case ACT_SETANM:
        evn_setAnm_init(staffIdx);
        break;
      case ACT_TALKMSG:
        evn_talk_init(staffIdx);
        break;
      case ACT_INITPOS:
        evn_init_pos_init(staffIdx);
        break;
      case ACT_CREATEITEM:
        createItem();
        break;
      case ACT_SOUND:
        evn_sound_proc_init(staffIdx);
        break;
      case ACT_HEADSWING:
        evn_head_swing_init(staffIdx);
        break;
      case ACT_HARPOON:
        evn_harpoon_proc_init(staffIdx);
        break;
      case ACT_ROLLAT_CNT:
        evn_RollAtControl_init(staffIdx);
        break;
      case ACT_GAME_MODE:
        evn_game_mode_init(staffIdx);
        break;
      case ACT_HIDE:
        evn_hide_init(staffIdx);
        break;
      case ACT_CONTINUETALK:
        evn_continue_talk_init(staffIdx);
        break;
      case ACT_SETANGLE:
        evn_setAngle_init(staffIdx);
        break;
      }
    }
    BOOL end;
    switch (actIdx) {
    case ACT_TALKMSG:
      end = evn_talk();
      break;
    case ACT_ROLLAT_CNT:
      end = evn_RollAtControl();
      break;
    case ACT_TURN_TO_PLAYER:
      end = evn_turn_to_player();
      break;
    case ACT_CONTINUETALK:
      end = evn_continue_talk();
      break;
    default:
      end = true;
      break;
    }
    if (end) {
      dComIfGp_evmng_cutEnd(staffIdx);
    }
  }
  return true;
}

/* WWHD glue
ext 025F7DB0 u32 ji1EventMessageSetHD(Ji1EventMessageManagerHD* manager, u32 number, cXyz* pos)
ext 025DB58C void ji1EventDemoFlagOnHD()
ext 025F795C u32 ji1EventMessageStatusHD(Ji1EventMessageManagerHD* manager)
ext 025F74D0 void ji1EventSetMessageStatusHD(Ji1EventMessageManagerHD* manager, u32 status)
ext 0224DA74 u32 ji1EventNextStatusHD(daNpc_Ji1_c* actor, be<u32>* number)
ext 025E1904 void ji1EventBgmStopHD(u32 frames)
ext 025E18EC void ji1EventBgmStartHD(u32 id)
ext 025D7DEC u32 ji1EventPresentItemHD(cXyz* pos, s32 item, u8 flag, s32 bit, s32 room, csXyz* angle, cXyz* scale)
ext 0255F554 void ji1EventSwordFlushHD(cXyz* pos, s32 parameter)
ext 0259F514 void ji1EventSwingHorizontalHD(void* head, s32 a, s32 b, s32 c, s32 d)
ext 0259F36C void ji1EventSwingVerticalHD(void* head, s32 a, s32 b, s32 c, s32 d)
ext 02006638 void ji1EventAngleCopyHD(be<s16>* angle, be<s16>* source)
ext 02006584 void ji1EventAngleSetHD(be<s16>* angle, s16 value)
ext 028E9098 void ji1EventIdentityHD(Mtx34* matrix)
ext 0201A478 csXyz* ji1EventConstructAnglesHD(csXyz* angles, s16 x, s16 y, s16 z)
ext 02543280 void ji1EventCutEndHD(dEvent_manager_c* manager, s32 staff)
ext 025447C8 BOOL ji1EventIsAdvanceHD(dEvent_manager_c* manager, s32 staff)
data l_hio 104672AC
data l_messageManager 101F4B5C
data l_msgId 10467228
data l_gameLife 101D5F44
data l_hitScaleGuard 104673C0
data l_hitScale 10467264
data l_zeroAngle 101FF354
data ActionNames 101BEBA4
data cut_name_tbl 101BEBC0
str 1001B2E0 "AnmNo" in 02258508
str 1001B2BC "AnmNo" in 022582E4
str 1001B2C4 "hokan" in 022582E4
str 1001B2CC "MsgNo" in 022583A4
str 1001B2D4 "EndMsgNo" in 022583A4
str 1001B300 "EndMsgNo" in 02258BE8
str 1001B30C "angle" in 02258C4C
str 1001B2E8 "prm" in 02258794
str 1001B2EC "prm" in 02258894
str 1001B2F0 "prm" in 0225896C
str 1001B2F8 "prm" in 02258A78
str 1001B2FC "prm" in 02258B48
*/

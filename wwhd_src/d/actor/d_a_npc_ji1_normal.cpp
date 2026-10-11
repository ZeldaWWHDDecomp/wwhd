/**
 * d_a_npc_ji1_normal.cpp (WWHD)
 * NPC - Orca: guard sub-actions, normal and spin-attack (kaiten) actions
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout
 * and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_normal.h"

// HD: message status and the next-status return keep their native register
// widths.
u32 ji1NormalMessageSetHD(Ji1NormalMessageHD *, u32, cXyz *);
u8 ji1NormalMessageStatusHD(Ji1NormalMessageHD *);
void ji1NormalSetMessageStatusHD(Ji1NormalMessageHD *, u32);
u32 ji1NormalNextMessageStatusHD(daNpc_Ji1_c *, be<u32> *);
void ji1NormalSoundHD(u32, cXyz *);
void ji1NormalBgmStopHD(u32);
void ji1NormalSetQuakeHD(dDetect_c *, const cXyz *);
// HD: callback belongs to the separately verified Orca core.
static constexpr u32 kXyCheckCallbackHD = 0x02248C64;
static Ji1NormalMessagePointerHD l_messageManager;
static Ji1NormalMessageIdHD l_msgId;
static daNpc_Ji1_HIO_c l_hio;

enum {
  JA_SE_CV_JI_ATTACK = 0x4843,
  JA_SE_CV_JI_DEFENCE = 0x4833,
  JA_SE_OBJ_COL_SWS_NMTLP = 0x6817,
  JA_SE_CM_JI_SHOTEI = 0x58A0,
  JA_SE_CM_AJ_ANGRY_FOOT = 0x58A1,
  JA_SE_VS_JI_OPENING = 0x85B,
};
enum {
  fopMsgStts_MSG_DISPLAYED_e = 0xE,
  fopMsgStts_MSG_CONTINUES_e = 0xF,
  fopMsgStts_BOX_CLOSED_e = 0x12,
  fopMsgStts_MSG_DESTROYED_e = 0x13,
};

// HD: these helpers retain the verified float rounding and temporary sizes.
/* the absXZ of (a - b): cXyz::operator- into a temporary, then
 * sqrtf(PSVECSquareMag({x, 0, z})) */
/* inlined into normalAction, kaitenwaitAction, kaitenExpAction */
static inline f32 ji1_absXZ_diff(cXyz *a, cXyz *b) {
  gabi::Local<cXyz> d;
  cXyz_mi(a, d, b);
  gabi::Local<cXyz> xz;
  xz->x = (f32)d->x;
  xz->y = 0.0f;
  xz->z = (f32)d->z;
  return std_sqrtf(PSVECSquareMag(xz));
}
/* cM_atan2s of (a - b).x, .z */
/* inlined into normalAction, kaitenwaitAction, kaitenspeakAction, kaitenAction
 */
static inline s16 ji1_atan2_diff(cXyz *a, cXyz *b) {
  gabi::Local<cXyz> d;
  cXyz_mi(a, d, b);
  return cM_atan2s(d->x, d->z);
}
/* mDoExt_McaMorf::checkFrame(getEndFrame() - d) */
static inline BOOL ji1_checkEndFrame(daNpc_Ji1_c *i_this, f32 d) {
  mDoExt_McaMorf *m = i_this->mpOrcaMorf;
  return m->checkFrame(m->getEndFrame() - d);
}
/* dComIfGp_particle_set(ID_AK_JN_NG, field_0x7E0.GetTgHitPosP()) */
static inline void ji1_ng_particle(daNpc_Ji1_c *i_this) {
  dComIfGp_particle_set(dPa_name::Ji1Normal_ID_AK_JN_NG,
                        i_this->field_0x7E0.GetTgHitPosP());
}
/* inlined into normalSubActionHarpoonGuard, normalSubActionGuard */
static inline bool ji1_guard_event(daNpc_Ji1_c *i_this, u32 n) {
  i_this->setAnm(Ji1Normal_KAMAE, 4.0f, 0);
  i_this->field_0xC84 = n;
  ptmf_copy(i_this->field_0x2C8, i_this->mAction);
  ji1_setAction(i_this, ACT_eventAction, nullptr);
  return true;
}

/* 0224FCC4 */
void daNpc_Ji1_c::normalSubActionHarpoonGuard(s16 param_1) {
  if (field_0xD6C == 0) {
    if (field_0x7E0.ChkTgHit()) {
      field_0xD6C = 1;
      field_0xD74 += 1;
      field_0xD68 = 0;
      setAnm(Ji1Normal_YKGUARD, 0.0f, 1);
      ji1_ng_particle(this);
      ji1_seStart(this, JA_SE_CV_JI_DEFENCE, 0);
      ji1_seStart(this, JA_SE_OBJ_COL_SWS_NMTLP, 0);
    } else {
      if (mAnimation == Ji1Normal_WARAI) {
        if (ji1_checkEndFrame(this, 2.0f)) {
          s32 n = field_0xD68 + 1;
          field_0xD68 = n;
          if (n > 1) {
            setAnm(Ji1Normal_WAIT01, 12.0f, 0);
          }
        }
      }
    }
  }

  if (field_0xD6C == 1) {
    cLib_addCalcAngleS2(&current.angle.y, param_1, 2, 0x2000);
    if (mAnimation == Ji1Normal_TOKAMAE) {
      if (ji1_checkEndFrame(this, 14.0f)) {
        setParticle(3, 3.0f, 0.5f);
        ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
      } else if (ji1_checkEndFrame(this, 2.0f)) {
        dtParticle();
        s32 n = field_0xD68;
        field_0xD68 = n + 1;
        if (n > 2) {
          field_0xD6C = 0;
          setAnm(Ji1Normal_WAIT01, 16.0f, 0);
        } else {
          setAnm(Ji1Normal_TOKAMAE, 4.0f, 1);
          mpOrcaMorf->setPlaySpeed(2.0f);
        }
      }
    } else {
      if (ji1_checkEndFrame(this, 2.0f)) {
        field_0xD6C = 0;
        if (field_0xD74 == 3) {
          ji1_guard_event(this, Ji1Normal_CUT_GUARD_THREE);
        } else if (field_0xD74 == 5) {
          ji1_guard_event(this, Ji1Normal_CUT_GUARD_FIVE);
        } else if (field_0xD74 > 5) {
          ji1_guard_event(this, Ji1Normal_CUT_GUARD_MANY);
          field_0xD74 = 0;
        } else {
          setAnm(Ji1Normal_WARAI, 8.0f, 0);
        }
      }
    }
  }
}

/* 022504E4 */
void daNpc_Ji1_c::normalSubActionGuard(s16 param_1) {
  if (field_0xD6C == 0) {
    if (field_0x7E0.ChkTgHit()) {
      field_0xD6C = 1;
      field_0xD68 = 0;
      setAnm(Ji1Normal_GUARD, 0.0f, 1);
      ji1_seStart(this, JA_SE_CV_JI_DEFENCE, 0);
    }
  }

  if (field_0xD6C == 1) {
    cLib_addCalcAngleS2(&current.angle.y, param_1, 2, 0x2000);
    if (ji1_checkEndFrame(this, 2.0f)) {
      s32 n = field_0xD68;
      field_0xD68 = n + 1;
      if (n > 1) {
        field_0xD6C = 0;
        if (field_0xD74 == 3) {
          ji1_guard_event(this, Ji1Normal_CUT_GUARD_THREE);
        } else if (field_0xD74 == 5) {
          ji1_guard_event(this, Ji1Normal_CUT_GUARD_FIVE);
        } else if (field_0xD74 > 5) {
          ji1_guard_event(this, Ji1Normal_CUT_GUARD_MANY);
        } else {
          setAnm(Ji1Normal_WAIT02, 8.0f, 0);
        }
      }
    }
  }
}

/* 02250A1C */
BOOL daNpc_Ji1_c::normalAction(void *arg) {
  fopAc_ac_c *player = daPy_getPlayerActorClass();
  f32 temp = ji1_absXZ_diff(&player->current.pos, &current.pos);

  if (field_0xC78 == 0) {
    field_0x7E0.mGObjTg.mSPrm |= 1; /* OnTgShield (dCcD_GObjTg SPrm) */
    if (field_0xD84 == 1) {
      setAnm(Ji1Normal_WAIT01, 8.0f, 0);
    } else {
      setAnm(Ji1Normal_WAIT02, 8.0f, 0);
    }
    if (dComIfGs_isEventBit(Ji1NormalEventFlag::UNK_0520)) {
      eventInfo.mpCheckCB = kXyCheckCallbackHD;
    }
    field_0xC78 += 1;
  } else if (field_0xC78 != -1) {
    dComIfGp_get(); /* HD: a daPy_getPlayerActorClass() whose result is unused
                     */
    s16 temp2 = ji1_atan2_diff(&player->current.pos, &current.pos);

    if (eventInfo.checkCommandTalk()) {
      s16 temp3 = cLib_targetAngleY(&current.pos, &player->current.pos);
      temp3 -= m_jnt.getHead_y();
      temp3 -= m_jnt.getBackbone_y();
      cLib_addCalcAngleS2(&current.angle.y, temp3, 8, 0x800);
      if (cLib_distanceAngleS(current.angle.y, temp3) < 0x100) {
        ji1_setAction(this, ACT_talkAction, nullptr);
      }
    }

    if (temp < l_hio.field_0x2C && isGuardAnim() == 0 &&
        mAnimation != Ji1Normal_WARAI) {
      eventInfo.onCondition(dEvtCnd_CANTALK_e);
      if (dComIfGs_isEventBit(Ji1NormalEventFlag::UNK_0520)) {
        eventInfo.onCondition(dEvtCnd_CANTALKITEM_e);
      }
    }

    if (field_0xD84 == 1) {
      normalSubActionHarpoonGuard(temp2);
    } else {
      normalSubActionGuard(temp2);
    }
  }
  return true;
}

/* 02251690 */
BOOL daNpc_Ji1_c::kaitenExpAction(void *arg) {
  fopAc_ac_c *player = daPy_getPlayerActorClass();
  ji1_absXZ_diff(&player->current.pos, &current.pos); /* unused */

  if (mAnimation == Ji1Normal_BTKAMASI) {
    if (mpOrcaMorf->checkFrame(19.0f)) {
      if (field_0xD68 == 1) {
        gabi::Local<cXyz> v;
        v->set(0.0f, 1.0f, 0.0f);
        dComIfGp_getVibration_StartShock(5, -0x11, v);
      }
      ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
      ji1_seStart(this, JA_SE_CM_JI_SHOTEI, 0);
      setParticle(3, 3.0f, 0.5f);
    }
    if (ji1_checkEndFrame(this, 2.0f)) {
      s32 n = field_0xD68 + 1;
      if (n > 1) {
        field_0xD68 = 0;
        setAnm(Ji1Normal_BTKAMAE, 4.0f, 0);
      } else {
        field_0xD68 = n;
      }
      dtParticle();
    }
  }

  if (field_0xC78 == 0) {
    setAnm(Ji1Normal_BTKAMASI, 4.0f, 0);
    if (!eventInfo.checkCommandDemoAccrpt()) {
      fopAcM_orderOtherEventId(this, mEventIdx[Ji1Normal_EVENT_SPIN_EXPLAIN],
                               0xFF, 0xFFFF, 0, 1);
      eventInfo.onCondition(dEvtCmd_INDEMO_e);
      return false;
    }
    field_0xC78 += 1;
  } else if (field_0xC78 != -1) {
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
    if (getEventActionNo(staffIdx) == 2) {
      ji1_setAction(this, ACT_kaitenspeakAction, nullptr);
    } else {
      dComIfGp_evmng_cutEnd(staffIdx);
    }
  }
  return true;
}


/* 02251424 */
BOOL daNpc_Ji1_c::kaitenspeakAction(void *arg) {
  if (field_0xC78 == 0) {
    setAnm(Ji1Normal_WAIT02, 8.0f, 0);
    field_0xC78 += 1;
  } else if (field_0xC78 != -1) {
    fopAc_ac_c *player = daPy_getPlayerActorClass();
    s16 temp2 = (s16)(ji1_atan2_diff(&player->current.pos, &current.pos) -
                      current.angle.y);
    m_jnt.mbTrn = 1; /* setTrn */
    if (abs(temp2) < 0x1000) {
      setAnm(Ji1Normal_TALK02, 8.0f, 0);
    }
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
    if (getEventActionNo(staffIdx) == 2) {
      if (temp2 == 0) {
        m_jnt.mbTrn = 0; /* clrTrn */
        ji1_setAction(this, ACT_talkAction, nullptr);
      }
    } else {
      dComIfGp_evmng_cutEnd(staffIdx);
    }
  }
  return true;
}

/* 02250D54 */
BOOL daNpc_Ji1_c::kaitenwaitAction(void *arg) {
  fopAc_ac_c *player = daPy_getPlayerActorClass();
  f32 temp = ji1_absXZ_diff(&player->current.pos, &current.pos);

  if (field_0xC78 == 0) {
    if (!dComIfGs_isEventBit(Ji1NormalEventFlag::UNK_0501)) {
      setAnm(Ji1Normal_UDEGUMI, 16.0f, 0);
    } else {
      setAnm(Ji1Normal_WAIT02, 4.0f, 0);
    }
    field_0xC78 += 1;
  } else if (field_0xC78 != -1) {
    if (!dComIfGs_isEventBit(Ji1NormalEventFlag::UNK_0501) &&
        daNpc_Ji1_plRoomOutCheck()) {
      field_0xC84 = Ji1Normal_CUT_WAIT;
      ji1_setAction(this, ACT_eventAction, nullptr);
      ptmf_set(field_0x2C8, ACT_kaitenwaitAction);
      return true;
    }

    fopAc_ac_c *rollPlayer = daPy_getPlayerActorClass();
    s16 temp2 = ji1_atan2_diff(&player->current.pos, &current.pos);

    if (!dComIfGs_isEventBit(Ji1NormalEventFlag::UNK_0501) &&
        isGuardAnim() == 0) {
      s16 temp3 = ji1_atan2_diff(&rollPlayer->current.pos, &current.pos);
      cLib_addCalcAngleS2(&current.angle.y, temp3, 0x10, 0x800);
      if (static_cast<Ji1NormalPlayerHD *>(rollPlayer)->checkFrontRoll()) {
        setAnm(Ji1Normal_WAIT02, 6.0f, 0);
      } else {
        setAnm(Ji1Normal_UDEGUMI, 16.0f, 0);
      }
    }

    if (eventInfo.checkCommandTalk()) {
      s16 temp3 = cLib_targetAngleY(&current.pos, &player->current.pos);
      temp3 -= m_jnt.getHead_y();
      temp3 -= m_jnt.getBackbone_y();
      cLib_addCalcAngleS2(&current.angle.y, temp3, 8, 0x800);
      if (cLib_distanceAngleS(current.angle.y, temp3) < 0x100) {
        ji1_setAction(this, ACT_talkAction, nullptr);
      }
    }

    if (temp < l_hio.field_0x2C && isGuardAnim() == 0 &&
        mAnimation != Ji1Normal_WARAI && rollPlayer->speedF < 1.0f) {
      eventInfo.onCondition(dEvtCnd_CANTALK_e);
    }

    if (static_cast<Ji1NormalPlayerHD *>(player)->checkFrontRollCrash() &&
        (!dComIfGs_isEventBit(Ji1NormalEventFlag::UNK_0501) ||
         l_hio.field_0x30 != 0)) {
      field_0xD79 = 1;
      ji1NormalSetQuakeHD(
          &reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())->play.mDetect,
          nullptr);
      ji1_setAction(this, ACT_kaitenAction, nullptr);
    }

    if (field_0xD84 == 1) {
      normalSubActionHarpoonGuard(temp2);
    } else {
      normalSubActionGuard(temp2);
    }
  }
  return true;
}

// HD: native message-manager get/set replaces msg_class, and step two no longer
// searches for a message box.
/* 02251ED0 */
BOOL daNpc_Ji1_c::kaitenAction(void *arg) {
  Ji1NormalMessageHD *mgr = l_messageManager.get();
  if (field_0xC78 == 0) {
    if (!eventInfo.checkCommandDemoAccrpt()) {
      fopAcM_orderOtherEventId(this, mEventIdx[Ji1Normal_EVENT_SPIN], 0xFF,
                               0xFFFF, 0, 1);
      eventInfo.onCondition(dEvtCmd_INDEMO_e);
      return false;
    }
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
    l_msgId = fpcM_ERROR_PROCESS_ID_e;
    mMsgNo = Ji1Normal_MSG_SPIN_START;
    dComIfGp_evmng_cutEnd(staffIdx);
    current.angle.y = 0;
    field_0xC78 += 1;
  } else if (field_0xC78 != -1) {
    s32 staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
    int actionNo = getEventActionNo(staffIdx);
    if (l_msgId == fpcM_ERROR_PROCESS_ID_e) {
      if (actionNo == Ji1Normal_CUT_TALK) {
        // HD: create the message before changing the facing or ending its cut.
        u32 id = ji1NormalMessageSetHD(mgr, mMsgNo, &eyePos);
        l_msgId = id;
        if (id != fpcM_ERROR_PROCESS_ID_e) {
          if (mMsgNo == Ji1Normal_MSG_SPIN_START) {
            current.angle.y = -0x8000;
          } else if (mMsgNo == Ji1Normal_MSG_SPIN_END) {
            dComIfGp_evmng_cutEnd(staffIdx);
          }
        }
      } else if (actionNo == Ji1Normal_CUT_HEADLOCK) {
        m_jnt.onHeadLock();
        m_jnt.onBackBoneLock();
        setAnm(Ji1Normal_AKIRE, 8.0f, 0);
        dComIfGp_evmng_cutEnd(staffIdx);
      }
    } else if (field_0xC78 == 1) {
      if (actionNo == Ji1Normal_CUT_TALK) {
        gabi::Local<cXyz> v;
        v->set(0.0f, 1.0f, 0.0f);
        dComIfGp_getVibration_StartShock(5, -0x11, v);
        ji1NormalSoundHD(JA_SE_CM_AJ_ANGRY_FOOT, nullptr);
      }
      field_0xC78 += 1;
    } else if (ji1NormalMessageStatusHD(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
      // HD: next_msgStatus is passed on without narrowing its native return
      // register.
      ji1NormalSetMessageStatusHD(mgr,
                                  ji1NormalNextMessageStatusHD(this, &mMsgNo));
      if (ji1NormalMessageStatusHD(mgr) == fopMsgStts_MSG_CONTINUES_e) {
        ji1NormalMessageSetHD(mgr, mMsgNo, nullptr);
        setAnimFromMsgNo(mMsgNo);
      }
    } else if (ji1NormalMessageStatusHD(mgr) == fopMsgStts_BOX_CLOSED_e) {
      if (actionNo == Ji1Normal_CUT_TALK) {
        ji1NormalSetMessageStatusHD(mgr, fopMsgStts_MSG_DESTROYED_e);
        l_msgId = fpcM_ERROR_PROCESS_ID_e;
        mMsgNo = Ji1Normal_MSG_SPIN_END;
        field_0xC78 = 1;
        return true;
      }
      if (staffIdx != -1) {
        dComIfGp_evmng_cutEnd(staffIdx);
      }
      if (dComIfGp_evmng_endCheck(mEventIdx[Ji1Normal_EVENT_SPIN])) {
        dComIfGs_onEventBit(Ji1NormalEventFlag::UNK_0501);
        field_0xD28.copy(home.pos);
        home.angle.y = 0;
        ji1NormalSetMessageStatusHD(mgr, fopMsgStts_MSG_DESTROYED_e);
        if (mMsgNo == Ji1Normal_MSG_OPENING) {
          field_0xD70 = -1;
          ji1_seStart(this, JA_SE_VS_JI_OPENING, 0);
          ji1NormalBgmStopHD(45);
          ji1_setAction(this, ACT_plmoveAction, nullptr);
        } else {
          setAnm(Ji1Normal_WAIT02, 8.0f, 0);
          ji1_setAction(this, ACT_normalAction, nullptr);
        }
        dComIfGp_event_reset();
      }
    } else if (reinterpret_cast<dComIfG_inf_c *>(dComIfGp_get())
                   ->play.mMesgSendButton &&
               actionNo == Ji1Normal_CUT_SEND &&
               mMsgNo == Ji1Normal_MSG_SPIN_WAIT) {
      setAnm(Ji1Normal_TALK01, 4.0f, 0);
      m_jnt.offHeadLock();
      m_jnt.offBackBoneLock();
      dComIfGp_evmng_cutEnd(staffIdx);
    }

    if (actionNo == Ji1Normal_CUT_FACE_PLAYER) {
      fopAc_ac_c *player = daPy_getPlayerActorClass();
      s16 temp = ji1_atan2_diff(&player->current.pos, &current.pos);
      cLib_addCalcAngleS2(&current.angle.y, temp, 4, 0x1000);
    }
  }
  return true;
}
/* WWHD glue
ext 025F7DB0 u32 ji1NormalMessageSetHD(Ji1NormalMessageHD* manager, u32 number, cXyz* position)
ext 025F795C u8 ji1NormalMessageStatusHD(Ji1NormalMessageHD* manager)
ext 025F74D0 void ji1NormalSetMessageStatusHD(Ji1NormalMessageHD* manager, u32 status)
ext 0224DA74 u32 ji1NormalNextMessageStatusHD(daNpc_Ji1_c* actor, be<u32>* message)
ext 025E19CC void ji1NormalSoundHD(u32 sound, cXyz* position)
ext 025E1904 void ji1NormalBgmStopHD(u32 fade)
ext 02529D7C void ji1NormalSetQuakeHD(dDetect_c* detect, const cXyz* position)
data l_messageManager 101F4B5C
data l_msgId 10467228
data l_hio 104672AC
str 1001B258 "Ji1" in 02251424
str 1001B260 "Ji1" in 02251690
str 1001B264 "Ji1" in 02251ED0
*/

/**
 * d_a_npc_ji1_teachsp.cpp (WWHD)
 * NPC - Orca: spin attack lesson (teachSPRollCutAction, teachSpRollCutMove),
 * checkCutType, battleGuardCheck.
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout
 * and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_teachsp.h"

// HD: these are views of the same translation-unit HIO and guarded hit scale.
static daNpc_Ji1_HIO_c l_hio;
static cXyz l_hitScale;
static Ji1TeachspGuardHD l_hitScaleGuard;
void ji1TeachspDeleteTimerHD(void *timer);

/* fopAcM_orderOtherEventId(this, idx) with the defaults (0xFF, 0xFFFF, 0, 1) */
static inline void orderOtherEvent(fopAc_ac_c *a, s16 idx) {
  fopAcM_orderOtherEventId(a, idx, 0xFF, 0xFFFF, 0, 1);
}
/* dCcD_GObjInf::OnTgShield / OffTgShield: mGObjTg.mSPrm bit 0; ChkAtShieldHit:
 * mGObjAt.mRPrm bit 0 */
static inline void OnTgShield(dCcD_GObjInf *o) { o->mGObjTg.mSPrm |= 1u; }
static inline void OffTgShield(dCcD_GObjInf *o) { o->mGObjTg.mSPrm &= ~1u; }
static inline bool ChkAtShieldHit(dCcD_GObjInf *o) {
  return (o->mGObjAt.mRPrm & 1) != 0;
}
// HD: GHS keeps the virtual index across isAttackFrame; other descriptor fields
// are reloaded.
static inline bool ptmf_eq_i(s16 i, ProcFunc_l &p, u32 fn) {
  return i == -1 && p.d == 0 && p.f == fn;
}

enum {
  CUT_TYPE_NONE = 0,
  CUT_TYPE_CUT_A = 1,
  CUT_TYPE_CUT_F = 2,
  CUT_TYPE_CUT_R = 3,
  CUT_TYPE_CUT_L = 4,
  CUT_TYPE_BT_JUMPCUT = 5,
  CUT_TYPE_CUT_EA = 6,
  CUT_TYPE_CUT_EB = 7,
  CUT_TYPE_CUT_TURN = 8,
  CUT_TYPE_CUT_ROLL = 9,
  CUT_TYPE_JUMPCUT_SWORD = 0xA,
  CUT_TYPE_BT_ROLLCUT = 0xF,
  CUT_TYPE_BT_VERTICALJUMPCUT = 0x10,
  CUT_TYPE_CUT_EXA = 0x1A,
  CUT_TYPE_CUT_EXB = 0x1B,
  CUT_TYPE_CUT_EXMJ = 0x1E,
  CUT_TYPE_CUT_KESA = 0x1F,
};
enum {
  JA_SE_CV_JI_DEFENCE = 0x4833,
  JA_SE_CV_JI_FUTTOBI = 0x4932,
  JA_SE_LK_SW_CRT_HIT = 0x2806,
  JA_SE_CM_JI_SLIP = 0x5021,
};

static inline f32 endFrame(daNpc_Ji1_c *t) {
  return t->mpOrcaMorf->getEndFrame();
}

/* 0224F368 */
BOOL daNpc_Ji1_c::teachSpRollCutMove(f32 param_1) {
  fopAc_ac_c *player = dComIfGp_getPlayer(0);
  gabi::Local<cXyz> temp;
  cXyz_mi(&home.pos, temp, &current.pos);
  gabi::Local<cXyz> temp2;
  cXyz_mi(&player->current.pos, temp2, &current.pos);
  std_sqrtf(PSVECSquareMag(temp));
  std_sqrtf(PSVECSquareMag(temp2));
  gabi::Local<cXyz> temp3;
  cXyz_mi(&player->current.pos, temp3, &home.pos);
  std_sqrtf(PSVECSquareMag(temp3));

  cM_atan2s(temp->x, temp->z);
  s16 temp4 = cM_atan2s(temp2->x, temp2->z);
  BOOL temp5 = FALSE;
  s16 d = (s16)(temp4 - current.angle.y);
  if ((d < 0 ? -d : d) > 0x1000) {
    temp5 = TRUE;
  }
  cLib_addCalcAngleS2(&current.angle.y, temp4, 8, 0x1000);
  if (temp5 == TRUE) {
    if (!isGuardAnim() || mpOrcaMorf->checkFrame(endFrame(this) - 2.0f)) {
      setAnm(Ji1Teachsp_KROT, 4.0f, 0);
    }
    if (mAnimation == Ji1Teachsp_KROT) {
      mpOrcaMorf->setPlaySpeed(l_hio.field_0x48 * param_1);
    }
    return TRUE;
  }
  return FALSE;
}

// HD: magic refills every frame; a spin attack must be within 20 height units and costs 10 Knight's Crests.
/* 02256F1C */
BOOL daNpc_Ji1_c::teachSPRollCutAction(void *arg) {
  fopAc_ac_c *player = daPy_getPlayerActorClass();

  if (field_0xC78 == 0) {
    dComIfGp_setItemMagicCount(dComIfGs_getMaxMagic());
    setAnm(Ji1Teachsp_KAMAE, 0.0f, 0);
    attention_info.flags |= fopAc_Attn_LOCKON_MISC_e;
    field_0xC30 = 0;
    field_0xD34 = 0;
    field_0xC90 = 0;
    dComIfGs_onTmpBit(Ji1TeachspTmpFlag::UNK_0402);
    field_0xD70 = 6;
    field_0xC78 += 1;
  } else if (field_0xC78 != -1) {
    dComIfGp_setItemMagicCount(dComIfGs_getMaxMagic());
    if (daNpc_Ji1_plRoomOutCheck()) {
      field_0xC84 = Ji1Teachsp_CUT_LEAVE_ROOM;
      ji1_setAction(this, ACT_eventAction, nullptr);
      ptmf_set(field_0x2C8, ACT_teachSPRollCutAction);
      return true;
    }

    gabi::Local<cXyz> temp;
    cXyz_mi(&player->current.pos, temp, &current.pos);
    cM_atan2s(temp->x, temp->z);
    gabi::Local<cXyz> xz;
    xz->x = (f32)temp->x;
    xz->y = 0.0f;
    xz->z = (f32)temp->z;
    std_sqrtf(PSVECSquareMag(xz));

    s32 cutType = static_cast<Ji1TeachspPlayerHD *>(player)->getCutType();
    f32 y_diff = std::fabs((f32)temp->y);
    if (cutType == CUT_TYPE_CUT_ROLL && y_diff < 20.0f) {
      dComIfGs_onEventBit(Ji1TeachspEventFlag::UNK_0B20);
      dComIfGs_offTmpBit(Ji1TeachspTmpFlag::UNK_0402);
      dComIfGp_setItemBeastNumCount(Ji1Teachsp_KNIGHTS_CREST, -10);
      OffTgShield(&field_0x7E0);
      field_0xC84 = Ji1Teachsp_CUT_SPIN_SUCCESS;
      ji1_setAction(this, ACT_eventAction, nullptr);
      ptmf_set(field_0x2C8, ACT_normalAction);
      return TRUE;
    }
    if (cutType == CUT_TYPE_CUT_TURN) {
      field_0xC84 = Ji1Teachsp_CUT_TURN;
      ji1_setAction(this, ACT_eventAction, nullptr);
      ptmf_set(field_0x2C8, ACT_teachSPRollCutAction);
      return TRUE;
    }

    OnTgShield(&field_0x7E0);
    if (field_0x7E0.ChkTgHit() && field_0xD6C != 1) {
      switch ((u32)field_0xC24) {
      case CUT_TYPE_CUT_A:
      case CUT_TYPE_CUT_EA:
      case CUT_TYPE_CUT_TURN:
        setAnm(Ji1Teachsp_TATEGUARD, 0.0f, 1);
        break;
      case CUT_TYPE_CUT_ROLL:
        break;
      case CUT_TYPE_JUMPCUT_SWORD:
        setAnm(Ji1Teachsp_JPGUARD, 0.0f, 1);
        field_0xD38.copy(current.pos);
        field_0xC9C = 0.0f;
        break;
      default:
        setAnm(Ji1Teachsp_YKGUARD, 0.0f, 1);
        break;
      }
      if (field_0xC24 == CUT_TYPE_CUT_ROLL) {
        setHitParticle(nullptr, JA_SE_CV_JI_DEFENCE);
        field_0xD38.copy(current.pos);
        field_0xC9C = 0.0f;
        setParticle(0x10, 1.0f, 0.1f);
      } else {
        setGuardParticle();
      }
      field_0xD6C = 1;
      field_0xC8C = field_0xC24;
    }

    if (field_0xD6C == 1) {
      if (!static_cast<Ji1TeachspPlayerHD *>(player)->getCutAtFlg() ||
          mpOrcaMorf->getFrame() > endFrame(this) - 2.0f) {
        if (field_0xC90 == 0) {
          if (field_0xC8C == CUT_TYPE_CUT_ROLL) {
            field_0xC84 = Ji1Teachsp_CUT_SPIN_SUCCESS;
            ji1_setAction(this, ACT_eventAction, nullptr);
            ptmf_set(field_0x2C8, ACT_normalAction);
            return true;
          }
          orderOtherEvent(this, mEventIdx[Ji1Teachsp_EVENT_BAD_CUT]);
          eventInfo.onCondition(dEvtCnd_UNK2_e);
          field_0xC90 = 2;
        }
      }

      if (mAnimation == Ji1Teachsp_JPGUARD &&
          field_0xC8C == CUT_TYPE_CUT_ROLL) {
        f32 h34 = l_hio.field_0x34;
        cLib_addCalc2(&field_0xC9C, h34 + h34, 0.25f, l_hio.field_0x38);
        gabi::Local<cXyz> temp3;
        f32 x = field_0xD38.x;
        f32 c9c = field_0xC9C;
        f32 y = field_0xD38.y;
        f32 s = cM_ssin(current.angle.y);
        f32 c = cM_scos(current.angle.y);
        f32 z = field_0xD38.z;
        temp3->x = gabi::fnmsubs(c9c, s, x);
        temp3->y = y;
        temp3->z = gabi::fnmsubs(c9c, c, z);
        f32 temp2 = 45.0f;
        if (mAcch.ChkWallHit()) {
          temp2 = 10.0f;
        }
        cLib_addCalcPos2(&current.pos, *temp3, 0.8f, temp2);
        ji1_seStart(this, JA_SE_CM_JI_SLIP, 0);
      }

      if (mpOrcaMorf->getFrame() > endFrame(this) - 2.0f) {
        OffTgShield(&field_0x6B0);
        OnTgShield(&field_0x7E0);
        field_0xC9C = 0.0f;
        field_0xD6C = 0;
        if (field_0xC90 == 2) {
          field_0xD38.copy(current.pos);
          field_0xC4C = 0.0f;
          field_0xC50 = 0.0f;
          field_0xC2C = 0;
          ji1_setAction(this, ACT_speakBadAction, nullptr);
        } else {
          field_0xD38.copy(current.pos);
          field_0xC90 = 0;
          ji1_setAction(this, ACT_speakAction, nullptr);
        }
      }
    } else {
      if (!teachSpRollCutMove(1.0f)) {
        if (mpOrcaMorf->checkFrame(endFrame(this) - 2.0f)) {
          setAnm(Ji1Teachsp_KAMAE, 4.0f, 0);
        }
      }
    }

    if (field_0xD6C != 1) {
      dtParticle();
    }
    if (field_0xD6C != 1 && isGuardAnim()) {
      if (mpOrcaMorf->getFrame() > endFrame(this) - 2.0f) {
        setAnm(Ji1Teachsp_KAMAE, 4.0f, 0);
      }
    }
  }
  return true;
}

/* 0225A534 */
BOOL daNpc_Ji1_c::battleGuardCheck() {
  if (field_0x7E0.ChkTgHit() && field_0xD6C != 1) {
    int anm = Ji1Teachsp_YKGUARD;
    switch ((u32)field_0xC24) {
    case CUT_TYPE_BT_ROLLCUT:
    case CUT_TYPE_BT_VERTICALJUMPCUT:
      battleSubActionJumpInit();
      break;
    case CUT_TYPE_BT_JUMPCUT:
      current.angle.y += (s16)-0x8000;
      battleSubActionDamageInit();
      break;
    case CUT_TYPE_JUMPCUT_SWORD:
      battleSubActionJpGuardInit();
      break;
    case CUT_TYPE_CUT_A:
    case CUT_TYPE_CUT_EA:
    case CUT_TYPE_CUT_TURN:
    case CUT_TYPE_CUT_ROLL:
    case CUT_TYPE_CUT_EXA:
      anm = Ji1Teachsp_TATEGUARD;
      /* fallthrough */
    default: {
      BOOL atk = isAttackAnim();
      s16 si = mSubAction.i;
      if (!atk) {
        if (!ptmf_eq_i(si, mSubAction, SUB_battleSubActionNockBack)) {
          setAnm(anm, 0.0f, 1);
          battleSubActionGuardInit();
          break;
        }
      }
      int attackFrame = isAttackFrame();
      if (ptmf_eq_i(si, mSubAction, SUB_battleSubActionNockBack)) {
        setAnm(anm, 0.0f, 1);
        battleSubActionGuardInit();
      } else {
        if (attackFrame == 0) {
          battleSubActionNockBackInit(1);
          field_0xD6C = 1;
          return 0;
        }
        if (attackFrame == 1) {
          setAnm(anm, 0.0f, 1);
          battleSubActionGuardInit();
          break;
        }
        return 0;
      }
      break;
    }
    }

    s32 c24 = field_0xC24;
    if (c24 == CUT_TYPE_NONE) {
      setGuardParticle();
    } else {
      if (c24 == CUT_TYPE_BT_ROLLCUT || c24 == CUT_TYPE_BT_VERTICALJUMPCUT ||
          c24 == CUT_TYPE_BT_JUMPCUT) {
        ji1_seStart(this, JA_SE_LK_SW_CRT_HIT, 0);
        cXyz *scale = &l_hitScale;
        // GHS sets the guard before the X/Z/Y stores.
        if (l_hitScaleGuard == 0) {
          l_hitScaleGuard = 1;
          scale->x = 1.25f;
          scale->z = 1.25f;
          scale->y = 1.25f;
        }
        if (field_0xC24 == CUT_TYPE_BT_JUMPCUT) {
          setHitParticle(scale, JA_SE_CV_JI_FUTTOBI);
        } else {
          setHitParticle(scale, JA_SE_CV_JI_DEFENCE);
        }
        s32 lim = l_hio.field_0x60[3] - 1;
        if (field_0xD70 < lim) {
          field_0xD70 += 2;
          dComIfGp_plusMiniGameRupee(2);
        } else {
          field_0xD70 += 1;
          dComIfGp_plusMiniGameRupee(1);
        }
      } else {
        setHitParticle(nullptr, JA_SE_CV_JI_DEFENCE);
        field_0xD70 += 1;
        dComIfGp_plusMiniGameRupee(1);
      }

      if (field_0xD70 >= l_hio.field_0x60[3]) {
        field_0xC84 = Ji1Teachsp_CUT_HIT_LIMIT;
        s32 c38;
        if (dComIfGs_isEventBit(Ji1TeachspEventFlag::UNK_0F20)) {
          c38 = 2;
        } else {
          c38 = dComIfGs_isEventBit(Ji1TeachspEventFlag::UNK_0F10) == 0;
        }
        field_0xC38 = c38;
        ji1_setAction(this, ACT_eventAction, nullptr);
        dComIfGp_setMessageCountNumber((s16)field_0xD70);
        ptmf_set(field_0x2C8, ACT_normalAction);
        if (dComIfG_getTimerPtr()) {
          ji1TeachspDeleteTimerHD(dComIfG_getTimerPtr());
          setClearRecord((s16)field_0xD70);
        }
        return TRUE;
      }
    }
    field_0xD6C = 1;
    mBtNowFrame = 30.0f; /* setBtNowFrame */
  }

  if (!ChkAtShieldHit(&field_0xA40) && field_0xA40.ChkAtHit()) {
    field_0xA40.ClrAtHit();
  }

  if (field_0xC3C > 2) {
    if (ptmf_eq(mAction, ACT_battleAction)) {
      if (dComIfG_getTimerPtr() != 0) {
        ji1TeachspDeleteTimerHD(dComIfG_getTimerPtr());
      }
      if (isClearRecord((s16)field_0xD70)) {
        setClearRecord((s16)field_0xD70);
        field_0xC84 = Ji1Teachsp_CUT_NEW_RECORD;
        ji1_setAction(this, ACT_eventAction, nullptr);
        dComIfGp_setMessageCountNumber((s16)field_0xD70);
        ptmf_set(field_0x2C8, ACT_normalAction);
      } else {
        ji1_setAction(this, ACT_endspeakAction, nullptr);
      }
    }
  }
  return TRUE;
}

/* 02255100 */
BOOL daNpc_Ji1_c::checkCutType(int cutType, int param_2) {
  fopAc_ac_c *player = daPy_getPlayerActorClass();
  BOOL ret = false;
  switch (param_2) {
  case 0:
    switch (cutType) {
    case CUT_TYPE_CUT_R:
    case CUT_TYPE_CUT_L:
    case CUT_TYPE_CUT_EB:
    case CUT_TYPE_CUT_EXA:
    case CUT_TYPE_CUT_EXB:
    case CUT_TYPE_CUT_EXMJ:
    case CUT_TYPE_CUT_KESA:
      ret = true;
      break;
    case CUT_TYPE_CUT_TURN:
      if (static_cast<Ji1TeachspPlayerHD *>(player)->checkComboCutTurn())
        ret = true;
      break;
    default:
      break;
    }
    break;
  case 1:
    switch (cutType) {
    case CUT_TYPE_CUT_A:
    case CUT_TYPE_CUT_EA:
    case CUT_TYPE_CUT_EXA:
    case CUT_TYPE_CUT_EXB:
    case CUT_TYPE_CUT_EXMJ:
    case CUT_TYPE_CUT_KESA:
      ret = true;
      break;
    case CUT_TYPE_CUT_TURN:
      if (static_cast<Ji1TeachspPlayerHD *>(player)->checkComboCutTurn())
        ret = true;
      break;
    default:
      break;
    }
    break;
  case 2:
    switch (cutType) {
    case CUT_TYPE_CUT_F:
    case CUT_TYPE_CUT_EA:
    case CUT_TYPE_CUT_EXA:
    case CUT_TYPE_CUT_EXB:
    case CUT_TYPE_CUT_EXMJ:
    case CUT_TYPE_CUT_KESA:
      ret = true;
      break;
    case CUT_TYPE_CUT_TURN:
      if (static_cast<Ji1TeachspPlayerHD *>(player)->checkComboCutTurn())
        ret = true;
      break;
    default:
      break;
    }
    break;
  case 3:
    switch (cutType) {
    case CUT_TYPE_CUT_TURN:
    case CUT_TYPE_CUT_ROLL:
      ret = true;
      break;
    default:
      break;
    }
    break;
  case 4:
    switch (cutType) {
    case CUT_TYPE_BT_JUMPCUT:
    case CUT_TYPE_BT_ROLLCUT:
    case CUT_TYPE_BT_VERTICALJUMPCUT:
      ret = true;
      break;
    default:
      break;
    }
    break;
  default:
    switch (cutType) {
    case CUT_TYPE_JUMPCUT_SWORD:
      ret = true;
      break;
    }
    break;
  }
  return ret;
}


/* WWHD glue
ext 025C58D8 void ji1TeachspDeleteTimerHD(void* timer)
data l_hio 104672AC
data l_hitScale 10467270
data l_hitScaleGuard 104673C4
*/

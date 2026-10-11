/**
 * d_a_npc_ji1_msg.cpp (WWHD)
 * NPC - Orca: messages (getMsg*, next_msgStatus, talkAction, speakAction,
 * speakBadAction)
 *
 * Ported from the GameCube
 * decompilation (zeldaret/tww src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout
 * and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1_msg.h"

// HD: one message-manager pointer read per action, full native status
// registers.
static Ji1MsgManagerPointerHD l_messageManager;
static Ji1MsgIdHD l_msgId;
static Ji1MsgReplyFlagHD l_replyFlag;
static daNpc_Ji1_HIO_c l_hio;
u32 ji1MsgMessageSetHD(Ji1MsgManagerHD *, u32, cXyz *);
u32 ji1MsgStatusHD(Ji1MsgManagerHD *);
void ji1MsgSetStatusHD(Ji1MsgManagerHD *, u32);
u32 ji1MsgNextStatusHD(daNpc_Ji1_c *, be<u32> *);
void ji1MsgBgmStopHD(u32);

enum {
  fopMsgStts_MSG_DISPLAYED_e = 0xE,
  fopMsgStts_MSG_CONTINUES_e = 0xF,
  fopMsgStts_MSG_ENDS_e = 0x10,
  fopMsgStts_BOX_CLOSED_e = 0x12,
  fopMsgStts_MSG_DESTROYED_e = 0x13,
};
/* setAction(field_0x2BC, arg): the PTMF argument is copied on entry */
static inline void ji1_setActionP(daNpc_Ji1_c *t, ProcFunc_l &src, void *arg) {
  s16 nd = src.d;
  s16 ni = src.i;
  u32 nf = src.f;
  ProcFunc_l &m = t->mAction;
  bool eq;
  if ((s16)m.i == ni) {
    eq = ni == 0 || ((s16)m.d == nd && (u32)m.f == nf);
  } else {
    eq = false;
  }
  if (!eq) {
    if (ptmf_nonnull(m)) {
      t->field_0xC78 = -1;
      ptmf_invoke(m, t, arg);
    }
    ptmf_copy(t->field_0x2BC, m);
    m.d = nd;
    m.i = ni;
    m.f = nf;
    t->field_0xC78 = 0;
    ptmf_invoke(m, t, arg);
  }
}

enum {
  JA_SE_CV_JI_ATTACK = 0x4843,
  JA_SE_CM_JI_ATTACK = 0x5822,
  JA_SE_VS_JI_OPENING = 0x85B,
};

/* 0224D8E0 */
u32 daNpc_Ji1_c::getMsg1stType() {
  if (!dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0001)) {
    if (!dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0501)) {
      if (dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0640)) {
        return 0x964;
      }
      dComIfGs_onEventBit(Ji1MsgEventFlag::UNK_0640);
      return 0x961;
    }
    if (dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_2F40)) {
      return 0x95B;
    }
    if (field_0xD79) {
      return 0x977;
    }
    if (!field_0xD78) {
      field_0xD78 = 1;
      return 0x978;
    }
    return 0x97B;
  }
  if (dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_2F10)) {
    return 0x973;
  }
  if (!dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0108)) {
    dComIfGs_onEventBit(Ji1MsgEventFlag::UNK_0108);
    eventInfo.setEventId(-1);
    return 0x965;
  }
  return 0x94D;
}

// HD: the PAL order of the checks (UNK_0B20 before the crest count)
/* 0224D718 */
u32 daNpc_Ji1_c::getMsg2ndType() {
  u32 msgNo;
  if (dComIfGp_event_chkTalkXY()) {
    m_jnt.offBackBoneLock();
    if (!dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0D80)) {
      dComIfGs_onEventBit(Ji1MsgEventFlag::UNK_0D80);
      msgNo = 0x9AE;
    } else if (dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0B20)) {
      msgNo = 0x9BC;
    } else if (dComIfGs_getBeastNum(Ji1Msg_KNIGHTS_CREST) < 10) {
      msgNo = 0x9BB;
    } else {
      msgNo = 0x9B0;
    }
  } else if (dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0002)) {
    if (dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0F20)) {
      if (dComIfGs_getEventReg(Ji1MsgEventFlag::UNK_CF03) >= 3) {
        msgNo = 0x9AC;
      } else {
        msgNo = 0x9AD;
      }
    } else if (dComIfGs_getEventReg(Ji1MsgEventFlag::UNK_CF03) >= 3) {
      msgNo = 0x9AA;
    } else {
      msgNo = 0x9AB;
    }
  } else {
    msgNo = 0x986;
    dComIfGs_onEventBit(Ji1MsgEventFlag::UNK_0002);
  }
  dComIfGs_setEventReg(Ji1MsgEventFlag::UNK_CF03, 0);
  return msgNo;
}

// HD: the selection is read from the message manager; PAL crest count check
/* 0224DA74 */
u16 daNpc_Ji1_c::next_msgStatus(be<u32> *pMsgNo) {
  u16 status = fopMsgStts_MSG_CONTINUES_e;
  Ji1MsgManagerHD *mgr = l_messageManager.get();
  switch ((u32)mMsgNo) {
  case 0x951:
  case 0x952:
  case 0x956:
  case 0x957:
  case 0x959:
  case 0x95B:
  case 0x95F:
  case 0x961:
  case 0x962:
  case 0x965:
  case 0x975:
  case 0x978:
  case 0x979:
  case 0x986:
  case 0x987:
  case 0x988:
  case 0x989:
  case 0x98B:
  case 0x98D:
  case 0x98F:
  case 0x991:
  case 0x993:
  case 0x99B:
  case 0x99D:
  case 0x99F:
  case 0x9A1:
  case 0x9A7:
  case 0x9AA:
  case 0x9AC:
  case 0x9AE:
  case 0x9B0:
  case 0x9B1:
  case 0x9B2:
  case 0x9B3:
  case 0x9B8:
  case 0x9B9:
    mMsgNo = mMsgNo + 1;
    break;
  case 0x97B:
    mMsgNo = 0x9C0;
    break;
  case 0x9A2:
    if (dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0F10)) {
      mMsgNo = 0x9A3;
    } else {
      mMsgNo = 0x9A5;
    }
    break;
  case 0x9AF:
    if (dComIfGs_getBeastNum(Ji1Msg_KNIGHTS_CREST) >= 10) {
      mMsgNo = 0x9B0;
    } else {
      mMsgNo = 0x9BB;
    }
    break;
  case 0x997:
  case 0x999:
    mMsgNo = 0x998;
    break;
  case 0x99A:
    if (dComIfGs_getEventReg(Ji1MsgEventFlag::UNK_D003) == 1) {
      mMsgNo = 0x99B;
    } else if (dComIfGs_getEventReg(Ji1MsgEventFlag::UNK_D003) == 2) {
      mMsgNo = 0x99D;
    } else {
      mMsgNo = 0x99F;
    }
    break;
  case 0x95C:
  case 0x973:
  case 0x976:
  case 0x977:
  case 0x97A:
  case 0x9C0:
    mMsgNo = 0x967;
    break;
  case 0x94D:
  case 0x966:
    mMsgNo = 0x94E;
    break;
  case 0x967:
    if (s32(mgr->mSelectNum) == 0) {
      status = fopMsgStts_MSG_ENDS_e;
    } else if (s32(mgr->mSelectNum) == 1) {
      mMsgNo = 0x968;
    }
    break;
  case 0x94E:
    if (s32(mgr->mSelectNum) == 0) {
      status = fopMsgStts_MSG_ENDS_e;
    } else if (s32(mgr->mSelectNum) == 1) {
      mMsgNo = 0x94F;
    }
    break;
  case 0x98A:
  case 0x9AB:
  case 0x9AD:
    if (s32(mgr->mSelectNum) == 0) {
      status = fopMsgStts_MSG_ENDS_e;
    } else if (s32(mgr->mSelectNum) == 1) {
      mMsgNo = 0x995;
    }
    break;
  default:
    status = fopMsgStts_MSG_ENDS_e;
    break;
  }
  return status;
}

/* tail of talkAction: the retreat while explaining */
static inline void talk_backslide(daNpc_Ji1_c *t) {
  if (t->mAnimation == Ji1Msg_BIBIRI &&
      !(t->mpOrcaMorf->getFrame() < l_hio.field_0xB4)) {
    t->BackSlide(l_hio.field_0xAC, l_hio.field_0xB0);
  }
}

// HD: messages through the message manager (no l_msg search step), the next
// message's animation only when it was set
/* 0225268C */
BOOL daNpc_Ji1_c::talkAction(void *arg) {
  Ji1MsgManagerHD *mgr = l_messageManager.get();
  if (field_0xC78 == 0) {
    if (dComIfGp_event_chkTalkXY() && !dComIfGp_evmng_ChkPresentEnd()) {
      if (dComIfGs_isEventBit(Ji1MsgEventFlag::UNK_0D80) == 0) {
        fopAc_ac_c *player = daPy_getPlayerActorClass();
        gabi::Local<cXyz> delta;
        cXyz_mi(&player->current.pos, delta, &current.pos);
        gabi::Local<cXyz> xz;
        xz->x = (f32)delta->x;
        xz->y = 0.0f;
        xz->z = (f32)delta->z;
        std_sqrtf(PSVECSquareMag(xz)); /* absXZ, unused */
        cLib_addCalcAngleS2(&current.angle.y, cM_atan2s(delta->x, delta->z), 8,
                            0x1000);
        m_jnt.onBackBoneLock();
        field_0xD7E = 1;
      }
      return 0;
    }
    field_0xD7E = 0;
    mMsgNo = getMsg();
    field_0xC78 = field_0xC78 + 1;
    l_msgId = fpcM_ERROR_PROCESS_ID_e;
  } else if (field_0xC78 != -1) {
    u32 msgNo = mMsgNo;
    if (l_msgId == fpcM_ERROR_PROCESS_ID_e) {
      l_msgId = ji1MsgMessageSetHD(mgr, msgNo, &eyePos);
      return TRUE;
    }
    setAnimFromMsgNo(msgNo);
    u32 st = ji1MsgStatusHD(mgr);
    if (st == fopMsgStts_MSG_DISPLAYED_e) {
      // HD: pass next_msgStatus r3 without narrowing it to the declared u16.
      ji1MsgSetStatusHD(mgr, ji1MsgNextStatusHD(this, &mMsgNo));
      if (ji1MsgStatusHD(mgr) == fopMsgStts_MSG_CONTINUES_e) {
        if (ji1MsgMessageSetHD(mgr, mMsgNo, nullptr) != 0) {
          setAnimFromMsgNo(mMsgNo);
        }
      }
    } else if (st == fopMsgStts_BOX_CLOSED_e) {
      ji1MsgSetStatusHD(mgr, fopMsgStts_MSG_DESTROYED_e);
      dComIfGp_event_reset();
      if (mMsgNo == 0x963 || mMsgNo == 0x98A) {
        int staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
        dComIfGp_evmng_cutEnd(staffIdx);
        if (dComIfGp_evmng_endCheck(mEventIdx[Ji1Msg_EVENT_SPIN])) {
          if (mMsgNo == 0x98A) {
            field_0xD70 = -1;
            ji1_seStart(this, JA_SE_VS_JI_OPENING, 0);
            ji1MsgBgmStopHD(45);
            ji1_setAction(this, ACT_plmoveAction, nullptr);
          } else {
            ji1_setAction(this, ACT_kaitenwaitAction, nullptr);
          }
        }
      } else if (mMsgNo == 0x94E || mMsgNo == 0x967 || mMsgNo == 0x9AB ||
                 mMsgNo == 0x9AD || mMsgNo == 0x9B4) {
        if (mMsgNo == 0x9B4) {
          field_0xD7B = 1;
        }
        field_0xD70 = -1;
        ji1_seStart(this, JA_SE_VS_JI_OPENING, 0);
        ji1MsgBgmStopHD(45);
        ji1_setAction(this, ACT_plmoveAction, nullptr);
      } else if (mMsgNo == 0x995) {
        ji1_setAction(this, ACT_normalAction, nullptr);
      } else {
        ji1_setActionP(this, field_0x2BC, nullptr);
      }
    }
    talk_backslide(this);
  }
  return TRUE;
}

// HD: messages through the message manager; the HD reply flag is cleared
/* 02252FD0 */
BOOL daNpc_Ji1_c::speakAction(void *arg) {
  Ji1MsgManagerHD *mgr = l_messageManager.get();
  if (field_0xC78 == 0) {
    if (!eventInfo.checkCommandDemoAccrpt()) {
      fopAcM_orderOtherEventId(this, mEventIdx[Ji1Msg_EVENT_REPLY], 0xFF,
                               0xFFFF, 0, 1);
      eventInfo.onCondition(dEvtCnd_UNK2_e);
      return FALSE;
    }
    dComIfGp_evmng_cutEnd(dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0));
    if (field_0xD70 == -1) {
      mMsgNo = 0x969;
      l_replyFlag = 0;
    } else {
      switch ((u32)field_0xD70) {
      case 0:
        if (field_0xC90 == 1) {
          mMsgNo = 0x97C;
          field_0xC94 = 1;
        } else if (field_0xC90 == 3) {
          mMsgNo = 0x96A;
          l_replyFlag = 0;
        } else {
          mMsgNo = 0x96F;
        }
        break;
      case 1:
        if (field_0xC90 == 1) {
          mMsgNo = 0x97D;
          field_0xC94 = 1;
        } else if (field_0xC90 == 3) {
          mMsgNo = 0x96B;
          l_replyFlag = 0;
        } else {
          mMsgNo = 0x970;
        }
        break;
      case 2:
        if (field_0xC90 == 1) {
          mMsgNo = 0x97E;
          field_0xC94 = 1;
        } else if (field_0xC90 == 3) {
          mMsgNo = 0x97F;
          l_replyFlag = 0;
        } else {
          mMsgNo = 0x971;
        }
        break;
      case 3:
        if (field_0xC90 == 1) {
          mMsgNo = 0x981;
          field_0xC94 = 1;
        } else if (field_0xC90 == 3) {
          mMsgNo = 0x983;
          attention_info.flags |= fopAc_Attn_LOCKON_BATTLE_e;
          attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 3;
          l_replyFlag = 0;
        } else {
          mMsgNo = 0x980;
        }
        break;
      case 4:
        if (field_0xC90 == 1) {
          mMsgNo = 0x984;
          field_0xC94 = 1;
        } else if (field_0xC90 == 3) {
          mMsgNo = 0x96C;
          attention_info.flags &= ~fopAc_Attn_LOCKON_BATTLE_e;
          attention_info.distances[fopAc_Attn_TYPE_BATTLE_e] = 0xB5;
          l_replyFlag = 0;
        } else {
          mMsgNo = 0x985;
        }
        break;
      default:
        if (field_0xC90 == 1) {
          mMsgNo = 0x982;
          field_0xC94 = 1;
        } else if (field_0xC90 == 3) {
          mMsgNo = 0x96D;
          l_replyFlag = 0;
        } else {
          mMsgNo = 0x972;
        }
        break;
      }
      if (field_0xC90 == 3) {
        field_0xD70 = field_0xD70 + 1;
        field_0xC94 = 0;
        reinterpret_cast<Ji1MsgHeadAnmHD *>(mHeadAnm)->swing_vertical_init(
            2, 0x1000, 0x800, 1);
        setAnm(Ji1Msg_KAMAE, 4.0f, 0);
        l_replyFlag = 0;
      } else {
        setAnm(Ji1Msg_KAMAE, 4.0f, 0);
      }
    }
    field_0xC78 = field_0xC78 + 1;
    l_msgId = fpcM_ERROR_PROCESS_ID_e;
  } else if (field_0xC78 != -1) {
    if (l_msgId == fpcM_ERROR_PROCESS_ID_e) {
      l_msgId = ji1MsgMessageSetHD(mgr, mMsgNo, &eyePos);
      if (l_msgId != fpcM_ERROR_PROCESS_ID_e) {
        setAnimFromMsgNo(mMsgNo);
      }
    } else {
      setAnimFromMsgNo(mMsgNo);
      if (field_0xC78 == 1) {
        field_0xC78 = field_0xC78 + 1;
      } else if (ji1MsgStatusHD(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
        // HD: pass next_msgStatus r3 without narrowing it to the declared u16.
        ji1MsgSetStatusHD(mgr, ji1MsgNextStatusHD(this, &mMsgNo));
        if (ji1MsgStatusHD(mgr) == fopMsgStts_MSG_CONTINUES_e) {
          ji1MsgMessageSetHD(mgr, mMsgNo, nullptr);
        }
      } else if (ji1MsgStatusHD(mgr) == fopMsgStts_BOX_CLOSED_e) {
        int staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
        if (staffIdx != -1) {
          dComIfGp_evmng_cutEnd(staffIdx);
        }
        if (dComIfGp_evmng_endCheck(mEventIdx[Ji1Msg_EVENT_REPLY])) {
          ji1MsgSetStatusHD(mgr, fopMsgStts_MSG_DESTROYED_e);
          if (mMsgNo == 0x969) {
            field_0xD70 = 0;
            field_0xC94 = 0;
            field_0xC98 = l_hio.field_0x54[0];
            ji1_setAction(this, ACT_teachAction, nullptr);
          } else {
            if (field_0xC90 != 1) {
              field_0xC98 = l_hio.field_0x54[(s32)field_0xD70];
            }
            ji1_setActionP(this, field_0x2BC, nullptr);
          }
          dComIfGp_event_reset();
        }
      }
    }
  }
  return TRUE;
}

// HD: messages through the message manager
/* 02254A64 */
BOOL daNpc_Ji1_c::speakBadAction(void *arg) {
  Ji1MsgManagerHD *mgr = l_messageManager.get();
  int staffIdx = dComIfGp_evmng_getMyStaffId("Ji1", nullptr, 0);
  if (field_0xC78 == 0) {
    if (!eventInfo.checkCommandDemoAccrpt()) {
      fopAcM_orderOtherEventId(this, mEventIdx[Ji1Msg_EVENT_BAD_CUT], 0xFF,
                               0xFFFF, 0, 1);
      eventInfo.onCondition(dEvtCnd_UNK2_e);
      return false;
    }
    if (MoveToPlayer(280.0f, 0)) {
      return false;
    }
    dComIfGp_evmng_cutEnd(staffIdx);
    switch ((u32)field_0xD70) {
    case 0:
      mMsgNo = 0x96F;
      break;
    case 1:
      mMsgNo = 0x970;
      break;
    case 2:
      mMsgNo = 0x971;
      break;
    case 3:
      mMsgNo = 0x980;
      break;
    case 4:
      mMsgNo = 0x985;
      break;
    case 5:
      mMsgNo = 0x972;
      break;
    default:
      mMsgNo = 0x9B6;
      break;
    }
    setAnm(Ji1Msg_INASI, 4.0f, 0);
    field_0xC78 = field_0xC78 + 1;
    l_msgId = fpcM_ERROR_PROCESS_ID_e;
  } else if (field_0xC78 != -1) {
    if (l_msgId == fpcM_ERROR_PROCESS_ID_e) {
      if (mAnimation == Ji1Msg_INASI) {
        if (mpOrcaMorf->getFrame() > mpOrcaMorf->getEndFrame() - 2.0f) {
          l_msgId = ji1MsgMessageSetHD(mgr, mMsgNo, &eyePos);
          if (l_msgId != fpcM_ERROR_PROCESS_ID_e) {
            reinterpret_cast<Ji1MsgHeadAnmHD *>(mHeadAnm)->swing_horizone_init(
                2, 0x1000, 0x1000, 1);
            setAnm(Ji1Msg_KAMAE, 0.0f, 0);
          }
        } else if (mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 10.0f)) {
          dComIfGp_evmng_cutEnd(staffIdx);
          ji1_seStart(this, JA_SE_CV_JI_ATTACK, 0);
          gabi::Local<cXyz> dir;
          dir->x = 0.0f;
          dir->y = 1.0f;
          dir->z = 0.0f;
          dComIfGp_getVibration_StartShock(5, -0x11, dir);
        } else if (mpOrcaMorf->checkFrame(mpOrcaMorf->getEndFrame() - 14.0f)) {
          ji1_seStart(this, JA_SE_CM_JI_ATTACK, 0);
        }
      }
    } else {
      if (field_0xC78 == 1) {
        field_0xC78 = field_0xC78 + 1;
      } else {
        if (ji1MsgStatusHD(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
          setAnimFromMsgNo(mMsgNo);
          // HD: pass next_msgStatus r3 without narrowing it to the declared
          // u16.
          ji1MsgSetStatusHD(mgr, ji1MsgNextStatusHD(this, &mMsgNo));
          if (ji1MsgStatusHD(mgr) == fopMsgStts_MSG_CONTINUES_e) {
            ji1MsgMessageSetHD(mgr, mMsgNo, nullptr);
          }
        } else if (ji1MsgStatusHD(mgr) == fopMsgStts_BOX_CLOSED_e) {
          if (staffIdx != -1) {
            dComIfGp_evmng_cutEnd(staffIdx);
          }
          if (dComIfGp_evmng_endCheck(mEventIdx[Ji1Msg_EVENT_BAD_CUT])) {
            ji1MsgSetStatusHD(mgr, fopMsgStts_MSG_DESTROYED_e);
            if (mMsgNo == 0x969) {
              field_0xD70 = 0;
              ji1_setAction(this, ACT_teachAction, nullptr);
            } else {
              ji1_setActionP(this, field_0x2BC, nullptr);
            }
            dComIfGp_event_reset();
          }
        }
      }
    }
  }
  return true;
}

/* WWHD glue
ext 025F7DB0 u32 ji1MsgMessageSetHD(Ji1MsgManagerHD* manager, u32 number, cXyz* position)
ext 025F795C u32 ji1MsgStatusHD(Ji1MsgManagerHD* manager)
ext 025F74D0 void ji1MsgSetStatusHD(Ji1MsgManagerHD* manager, u32 status)
ext 0224DA74 u32 ji1MsgNextStatusHD(daNpc_Ji1_c* actor, be<u32>* number)
ext 025E1904 void ji1MsgBgmStopHD(u32 fade)
ext 0259F36C void ji1MsgSwingVerticalHD(void* head, s16 a, s16 b, s16 c, s32 d)
ext 0259F514 void ji1MsgSwingHorizoneHD(void* head, s16 a, s16 b, s16 c, s32 d)
data l_messageManager 101F4B5C
data l_msgId 10467228
data l_replyFlag 10467244
data l_hio 104672AC
str 1001B270 "Ji1" in 0225268C
str 1001B274 "Ji1" in 02252FD0
str 1001B28C "Ji1" in 02254A64
*/

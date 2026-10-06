/**
 * d_a_npc_ji1_msg.cpp (WWHD)
 * NPC - Orca: messages (getMsg*, next_msgStatus, talkAction, speakAction, speakBadAction)
 *
 * Ported from the GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ji1.cpp) to the WWHD layout and verified against the WWHD code (cking.rpx).
 */
#include "d/actor/d_a_npc_ji1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* HD messages: the GameCube msg_class* (l_msg) is replaced by the message manager at
 * *(0x101F4B5C); the actions read the pointer once on entry. */
static inline u32 msgMgr() { return gabi::load<u32>(0x101F4B5C); }
/* 025F7DB0 fopMsgM_messageSet(mgr, msgNo, cXyz* pos) -> process id (HD: through the manager) */
static inline u32 fopMsgM_messageSet(u32 mgr, u32 msgNo, cXyz* pos) { return gabi::call<u32>(0x025F7DB0, mgr, msgNo, pos); }
/* 025F795C (matcher: fopMsgM_SearchByID): the manager's message status */
static inline u32 msgMgr_getStatus(u32 mgr) { return gabi::call<u32>(0x025F795C, mgr); }
/* 025F74D0: sets the manager's message status */
static inline void msgMgr_setStatus(u32 mgr, u32 st) { gabi::call(0x025F74D0, mgr, st); }
/* next_msgStatus(&mMsgNo) as GHS passes it on: the u16 result register is not re-extended */
static inline u32 ji1_next_msgStatus(daNpc_Ji1_c* t) { return gabi::call<u32>(0x0224DA74, t, &t->mMsgNo); }
/* mSelectNum of the current message: manager + 0x948 */
static inline s32 msgMgr_selectNum(u32 mgr) { return gabi::load<s32>(mgr + 0x948); }
enum {
    fopMsgStts_MSG_DISPLAYED_e = 0xE,
    fopMsgStts_MSG_CONTINUES_e = 0xF,
    fopMsgStts_MSG_ENDS_e = 0x10,
    fopMsgStts_BOX_CLOSED_e = 0x12,
    fopMsgStts_MSG_DESTROYED_e = 0x13,
};
/* dComIfGp_event_chkTalkXY(): play+0x52B0 (talk type) in 1..4 */
static inline bool dComIfGp_event_chkTalkXY() { return (u32)(gabi::load<u8>(dComIfGp_ea() + 0x52B0) - 1) <= 3; }
/* 02544950 dEvent_manager_c::ChkPresentEnd */
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
/* dComIfGs_getBeastNum(dBeastIdx_KNIGHTS_CREST_e): save byte at *(0x101F84DC) + 0xBF */
static inline u8 dComIfGs_getBeastNum_crest() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0xBF); }
/* 025E1904 mDoAud_bgmStop(u32) */
static inline void mDoAud_bgmStop(u32 t) { gabi::call(0x025E1904, t); }
/* dNpc_HeadAnm_c (0x24) */
static inline void HeadAnm_swing_vertical_init(void* h, s16 a, s16 b, s16 c, s32 d) { gabi::call(0x0259F36C, h, a, b, c, d); }
static inline void HeadAnm_swing_horizone_init(void* h, s16 a, s16 b, s16 c, s32 d) { gabi::call(0x0259F514, h, a, b, c, d); }
/* file statics: l_msgId 0x10467228; an HD byte at 0x10467244 cleared when Orca's reply starts */
static inline be<u32>& l_msgId() { return *gabi::at<be<u32>>(0x10467228); }
static inline be<u8>& l_hd_flag() { return *gabi::at<be<u8>>(0x10467244); }

/* setAction(field_0x2BC, arg): the PTMF argument is copied on entry */
static inline void ji1_setActionP(daNpc_Ji1_c* t, ProcFunc_l& src, void* arg) {
    s16 nd = src.d;
    s16 ni = src.i;
    u32 nf = src.f;
    ProcFunc_l& m = t->mAction;
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

/* 0224D718 HD: the PAL order of the checks (UNK_0B20 before the crest count) */
u32 daNpc_Ji1_c::getMsg2ndType() {
    WWHD_FUNC(0x0224D718, u32, this);
    u32 msgNo;
    if (dComIfGp_event_chkTalkXY()) {
        m_jnt.mbBackBoneLock = 0; /* offBackBoneLock */
        if (!dComIfGs_isEventBit(0x0D80)) {
            dComIfGs_onEventBit(0x0D80);
            msgNo = 0x9AE;
        } else if (dComIfGs_isEventBit(0x0B20)) {
            msgNo = 0x9BC;
        } else if (dComIfGs_getBeastNum_crest() < 10) {
            msgNo = 0x9BB;
        } else {
            msgNo = 0x9B0;
        }
    } else if (dComIfGs_isEventBit(0x0002)) {
        if (dComIfGs_isEventBit(0x0F20)) {
            if (dComIfGs_getEventReg(0xCF03) >= 3) {
                msgNo = 0x9AC;
            } else {
                msgNo = 0x9AD;
            }
        } else if (dComIfGs_getEventReg(0xCF03) >= 3) {
            msgNo = 0x9AA;
        } else {
            msgNo = 0x9AB;
        }
    } else {
        msgNo = 0x986;
        dComIfGs_onEventBit(0x0002);
    }
    dComIfGs_setEventReg(0xCF03, 0);
    return msgNo;
}
VERIFY(0x0224D718, &daNpc_Ji1_c::getMsg2ndType);

/* 0224D8E0 */
u32 daNpc_Ji1_c::getMsg1stType() {
    WWHD_FUNC(0x0224D8E0, u32, this);
    if (!dComIfGs_isEventBit(0x0001)) {
        if (!dComIfGs_isEventBit(0x0501)) {
            if (dComIfGs_isEventBit(0x0640)) {
                return 0x964;
            }
            dComIfGs_onEventBit(0x0640);
            return 0x961;
        }
        if (dComIfGs_isEventBit(0x2F40)) {
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
    if (dComIfGs_isEventBit(0x2F10)) {
        return 0x973;
    }
    if (!dComIfGs_isEventBit(0x0108)) {
        dComIfGs_onEventBit(0x0108);
        gabi::store<s16>(gabi::ea(this) + 0xFC, -1); /* eventInfo.setEventId(-1) */
        return 0x965;
    }
    return 0x94D;
}
VERIFY(0x0224D8E0, &daNpc_Ji1_c::getMsg1stType);

/* 0224DA74 HD: the selection is read from the message manager; PAL crest count check */
u16 daNpc_Ji1_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x0224DA74, u16, this, pMsgNo);
    u16 status = fopMsgStts_MSG_CONTINUES_e;
    u32 mgr = msgMgr();
    switch ((u32)mMsgNo) {
    case 0x951: case 0x952: case 0x956: case 0x957: case 0x959: case 0x95B: case 0x95F:
    case 0x961: case 0x962: case 0x965: case 0x975: case 0x978: case 0x979: case 0x986:
    case 0x987: case 0x988: case 0x989: case 0x98B: case 0x98D: case 0x98F: case 0x991:
    case 0x993: case 0x99B: case 0x99D: case 0x99F: case 0x9A1: case 0x9A7: case 0x9AA:
    case 0x9AC: case 0x9AE: case 0x9B0: case 0x9B1: case 0x9B2: case 0x9B3: case 0x9B8:
    case 0x9B9:
        mMsgNo = mMsgNo + 1;
        break;
    case 0x97B:
        mMsgNo = 0x9C0;
        break;
    case 0x9A2:
        if (dComIfGs_isEventBit(0x0F10)) {
            mMsgNo = 0x9A3;
        } else {
            mMsgNo = 0x9A5;
        }
        break;
    case 0x9AF:
        if (dComIfGs_getBeastNum_crest() >= 10) {
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
        if (dComIfGs_getEventReg(0xD003) == 1) {
            mMsgNo = 0x99B;
        } else if (dComIfGs_getEventReg(0xD003) == 2) {
            mMsgNo = 0x99D;
        } else {
            mMsgNo = 0x99F;
        }
        break;
    case 0x95C: case 0x973: case 0x976: case 0x977: case 0x97A: case 0x9C0:
        mMsgNo = 0x967;
        break;
    case 0x94D:
    case 0x966:
        mMsgNo = 0x94E;
        break;
    case 0x967:
        if (msgMgr_selectNum(mgr) == 0) {
            status = fopMsgStts_MSG_ENDS_e;
        } else if (msgMgr_selectNum(mgr) == 1) {
            mMsgNo = 0x968;
        }
        break;
    case 0x94E:
        if (msgMgr_selectNum(mgr) == 0) {
            status = fopMsgStts_MSG_ENDS_e;
        } else if (msgMgr_selectNum(mgr) == 1) {
            mMsgNo = 0x94F;
        }
        break;
    case 0x98A:
    case 0x9AB:
    case 0x9AD:
        if (msgMgr_selectNum(mgr) == 0) {
            status = fopMsgStts_MSG_ENDS_e;
        } else if (msgMgr_selectNum(mgr) == 1) {
            mMsgNo = 0x995;
        }
        break;
    default:
        status = fopMsgStts_MSG_ENDS_e;
        break;
    }
    return status;
}
VERIFY(0x0224DA74, &daNpc_Ji1_c::next_msgStatus);

/* tail of talkAction: the retreat while explaining */
static inline void talk_backslide(daNpc_Ji1_c* t) {
    if (t->mAnimation == 0x16 && !(t->mpOrcaMorf->getFrame() < l_HIO().field_0xB4)) {
        t->BackSlide(l_HIO().field_0xAC, l_HIO().field_0xB0);
    }
}

/* 0225268C HD: messages through the message manager (no l_msg search step), the next message's
 * animation only when it was set */
BOOL daNpc_Ji1_c::talkAction(void* arg) {
    WWHD_FUNC(0x0225268C, BOOL, this, arg);
    u32 mgr = msgMgr();
    if (field_0xC78 == 0) {
        if (dComIfGp_event_chkTalkXY() && !dComIfGp_evmng_ChkPresentEnd()) {
            if (dComIfGs_isEventBit(0x0D80) == 0) {
                fopAc_ac_c* player = daPy_getPlayerActorClass();
                gabi::Local<cXyz> delta;
                cXyz_mi(&player->current.pos, delta, &current.pos);
                gabi::Local<cXyz> xz;
                xz->x = (f32)delta->x;
                xz->y = 0.0f;
                xz->z = (f32)delta->z;
                std_sqrtf(PSVECSquareMag(xz)); /* absXZ, unused */
                cLib_addCalcAngleS2(&current.angle.y, cM_atan2s(delta->x, delta->z), 8, 0x1000);
                m_jnt.mbBackBoneLock = 1; /* onBackBoneLock */
                field_0xD7E = 1;
            }
            return 0;
        }
        field_0xD7E = 0;
        mMsgNo = getMsg();
        field_0xC78 = field_0xC78 + 1;
        l_msgId() = 0xFFFFFFFF;
    } else if (field_0xC78 != -1) {
        u32 msgNo = mMsgNo;
        if (l_msgId() == 0xFFFFFFFF) {
            l_msgId() = fopMsgM_messageSet(mgr, msgNo, &eyePos);
            return TRUE;
        }
        setAnimFromMsgNo(msgNo);
        u32 st = msgMgr_getStatus(mgr);
        if (st == fopMsgStts_MSG_DISPLAYED_e) {
            msgMgr_setStatus(mgr, ji1_next_msgStatus(this));
            if (msgMgr_getStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                if (fopMsgM_messageSet(mgr, mMsgNo, nullptr) != 0) {
                    setAnimFromMsgNo(mMsgNo);
                }
            }
        } else if (st == fopMsgStts_BOX_CLOSED_e) {
            msgMgr_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
            dComIfGp_event_reset();
            if (mMsgNo == 0x963 || mMsgNo == 0x98A) {
                int staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B270) /* "Ji1" */, nullptr, 0);
                dComIfGp_evmng_cutEnd(staffIdx);
                if (dComIfGp_evmng_endCheck(mEventIdx[6])) {
                    if (mMsgNo == 0x98A) {
                        field_0xD70 = -1;
                        ji1_seStart(this, JA_SE_VS_JI_OPENING, 0);
                        mDoAud_bgmStop(45);
                        ji1_setAction(this, ACT_plmoveAction, nullptr);
                    } else {
                        ji1_setAction(this, ACT_kaitenwaitAction, nullptr);
                    }
                }
            } else if (mMsgNo == 0x94E || mMsgNo == 0x967 || mMsgNo == 0x9AB || mMsgNo == 0x9AD || mMsgNo == 0x9B4) {
                if (mMsgNo == 0x9B4) {
                    field_0xD7B = 1;
                }
                field_0xD70 = -1;
                ji1_seStart(this, JA_SE_VS_JI_OPENING, 0);
                mDoAud_bgmStop(45);
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
VERIFY(0x0225268C, &daNpc_Ji1_c::talkAction);

/* 02252FD0 HD: messages through the message manager; the HD flag 0x10467244 is cleared */
BOOL daNpc_Ji1_c::speakAction(void* arg) {
    WWHD_FUNC(0x02252FD0, BOOL, this, arg);
    u32 mgr = msgMgr();
    if (field_0xC78 == 0) {
        if (!eventInfo_checkCommandDemoAccrpt(this)) {
            fopAcM_orderOtherEventId(this, mEventIdx[1], 0xFF, 0xFFFF, 0, 1);
            eventInfo_onCondition(this, 2 /* dEvtCnd_UNK2_e */);
            return FALSE;
        }
        dComIfGp_evmng_cutEnd(dComIfGp_evmng_getMyStaffId(STR(0x1001B274) /* "Ji1" */, nullptr, 0));
        if (field_0xD70 == -1) {
            mMsgNo = 0x969;
            l_hd_flag() = 0;
        } else {
            switch ((u32)field_0xD70) {
            case 0:
                if (field_0xC90 == 1) {
                    mMsgNo = 0x97C;
                    field_0xC94 = 1;
                } else if (field_0xC90 == 3) {
                    mMsgNo = 0x96A;
                    l_hd_flag() = 0;
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
                    l_hd_flag() = 0;
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
                    l_hd_flag() = 0;
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
                    attn_flags(this) |= 4u; /* fopAc_Attn_LOCKON_BATTLE_e */
                    attn_distance(this, 2) = 3;
                    l_hd_flag() = 0;
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
                    attn_flags(this) &= ~4u;
                    attn_distance(this, 2) = 0xB5;
                    l_hd_flag() = 0;
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
                    l_hd_flag() = 0;
                } else {
                    mMsgNo = 0x972;
                }
                break;
            }
            if (field_0xC90 == 3) {
                field_0xD70 = field_0xD70 + 1;
                field_0xC94 = 0;
                HeadAnm_swing_vertical_init(mHeadAnm, 2, 0x1000, 0x800, 1);
                setAnm(5, 4.0f, 0);
                l_hd_flag() = 0;
            } else {
                setAnm(5, 4.0f, 0);
            }
        }
        field_0xC78 = field_0xC78 + 1;
        l_msgId() = 0xFFFFFFFF;
    } else if (field_0xC78 != -1) {
        if (l_msgId() == 0xFFFFFFFF) {
            l_msgId() = fopMsgM_messageSet(mgr, mMsgNo, &eyePos);
            if (l_msgId() != 0xFFFFFFFF) {
                setAnimFromMsgNo(mMsgNo);
            }
        } else {
            setAnimFromMsgNo(mMsgNo);
            if (field_0xC78 == 1) {
                field_0xC78 = field_0xC78 + 1;
            } else if (msgMgr_getStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
                msgMgr_setStatus(mgr, ji1_next_msgStatus(this));
                if (msgMgr_getStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                    fopMsgM_messageSet(mgr, mMsgNo, nullptr);
                }
            } else if (msgMgr_getStatus(mgr) == fopMsgStts_BOX_CLOSED_e) {
                int staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B274), nullptr, 0);
                if (staffIdx != -1) {
                    dComIfGp_evmng_cutEnd(staffIdx);
                }
                if (dComIfGp_evmng_endCheck(mEventIdx[1])) {
                    msgMgr_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
                    if (mMsgNo == 0x969) {
                        field_0xD70 = 0;
                        field_0xC94 = 0;
                        field_0xC98 = l_HIO().field_0x54[0];
                        ji1_setAction(this, ACT_teachAction, nullptr);
                    } else {
                        if (field_0xC90 != 1) {
                            field_0xC98 = l_HIO().field_0x54[(s32)field_0xD70];
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
VERIFY(0x02252FD0, &daNpc_Ji1_c::speakAction);

/* 02254A64 HD: messages through the message manager */
BOOL daNpc_Ji1_c::speakBadAction(void* arg) {
    WWHD_FUNC(0x02254A64, BOOL, this, arg);
    u32 mgr = msgMgr();
    int staffIdx = dComIfGp_evmng_getMyStaffId(STR(0x1001B28C) /* "Ji1" */, nullptr, 0);
    if (field_0xC78 == 0) {
        if (!eventInfo_checkCommandDemoAccrpt(this)) {
            fopAcM_orderOtherEventId(this, mEventIdx[3], 0xFF, 0xFFFF, 0, 1);
            eventInfo_onCondition(this, 2 /* dEvtCnd_UNK2_e */);
            return false;
        }
        if (MoveToPlayer(280.0f, 0)) {
            return false;
        }
        dComIfGp_evmng_cutEnd(staffIdx);
        switch ((u32)field_0xD70) {
        case 0: mMsgNo = 0x96F; break;
        case 1: mMsgNo = 0x970; break;
        case 2: mMsgNo = 0x971; break;
        case 3: mMsgNo = 0x980; break;
        case 4: mMsgNo = 0x985; break;
        case 5: mMsgNo = 0x972; break;
        default: mMsgNo = 0x9B6; break;
        }
        setAnm(7, 4.0f, 0);
        field_0xC78 = field_0xC78 + 1;
        l_msgId() = 0xFFFFFFFF;
    } else if (field_0xC78 != -1) {
        if (l_msgId() == 0xFFFFFFFF) {
            if (mAnimation == 7) {
                if (mpOrcaMorf->getFrame() > mpOrcaMorf->getEndFrame() - 2.0f) {
                    l_msgId() = fopMsgM_messageSet(mgr, mMsgNo, &eyePos);
                    if (l_msgId() != 0xFFFFFFFF) {
                        HeadAnm_swing_horizone_init(mHeadAnm, 2, 0x1000, 0x1000, 1);
                        setAnm(5, 0.0f, 0);
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
                if (msgMgr_getStatus(mgr) == fopMsgStts_MSG_DISPLAYED_e) {
                    setAnimFromMsgNo(mMsgNo);
                    msgMgr_setStatus(mgr, ji1_next_msgStatus(this));
                    if (msgMgr_getStatus(mgr) == fopMsgStts_MSG_CONTINUES_e) {
                        fopMsgM_messageSet(mgr, mMsgNo, nullptr);
                    }
                } else if (msgMgr_getStatus(mgr) == fopMsgStts_BOX_CLOSED_e) {
                    if (staffIdx != -1) {
                        dComIfGp_evmng_cutEnd(staffIdx);
                    }
                    if (dComIfGp_evmng_endCheck(mEventIdx[3])) {
                        msgMgr_setStatus(mgr, fopMsgStts_MSG_DESTROYED_e);
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
VERIFY(0x02254A64, &daNpc_Ji1_c::speakBadAction);

/**
 * d_a_npc_tc_msg.cpp (WWHD)
 * NPC - Tingle, Ankle, David Jr., Knuckle: messages (GameCube d_a_npc_tc.cpp message functions,
 * d_a_npc_tc_msg_normal2.inc, d_a_npc_tc_msg_white.inc, d_a_npc_tc_msg_red.inc) and stopTower,
 * which lie in one WWHD range (022EA7BC..022EB96F).
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww) to the WWHD layout and code, verified against cking.rpx. "HD:" marks where
 * WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_tc.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline dSv_event_c* tc_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(tc_event(), f); }
/* 02520864 dComIfGs_isStageTbox(stageNo, no) */
static inline BOOL dComIfGs_isStageTbox(s32 stageNo, s32 no) { return gabi::call<BOOL>(0x02520864, stageNo, no); }
/* dSv_player_map_c at save + 0xE4 (dComIfGs_isCollectMapTriforce(i) etc. take i - 1) */
static inline u32 tc_playerMap() { return gabi::load<u32>(0x101F84DC) + 0xE4; }
static inline BOOL dComIfGs_isCollectMapTriforce(int i) { return gabi::call<BOOL>(0x025B8308, tc_playerMap(), i - 1); }
static inline BOOL dComIfGs_isGetCollectMap(int i) { return gabi::call<BOOL>(0x025B7F68, tc_playerMap(), i - 1); }
/* dComIfGs_getClearCount(): save + 0x1C0 (u8) */
static inline u8 dComIfGs_getClearCount() { return gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x1C0); }
/* dComIfGs_getRupee(): save + 0x24 (u16) */
static inline u16 dComIfGs_getRupee() { return gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x24); }
/* HD message manager (*(0x101F4B5C)): the current message's select number at +0x948
 * (GameCube mpCurrMsg->mSelectNum) */
static inline u32 tc_msgManager() { return gabi::load<u32>(0x101F4B5C); }
static inline s32 msg_selectNum(u32 mgr) { return gabi::load<s32>(mgr + 0x948); }
/* daObjSmplbg::Act_c::onStop(): byte at +0x3F0 */
static inline void smplbg_onStop(fopAc_ac_c* tower) { gabi::store<u8>(gabi::ea(tower) + 0x3F0, 1); }

enum {
    ANM_PRM_IDX_DANCE01_TO_TALK01 = 17, ANM_PRM_IDX_DANCE02_TO_WAIT03 = 18, ANM_PRM_IDX_WAIT04 = 20,
};
enum { fopMsgStts_MSG_DISPLAYED_e = 0xE, fopMsgStts_MSG_CONTINUES_e = 0xF, fopMsgStts_MSG_ENDS_e = 0x10 };

/* the common head of the next_msgStatus* functions */
static inline bool tc_waitDanceEnd(daNpc_Tc_c* i_this) {
    return (i_this->mAnmPrmIdx == ANM_PRM_IDX_DANCE01_TO_TALK01 || i_this->mAnmPrmIdx == ANM_PRM_IDX_DANCE02_TO_WAIT03) &&
           !i_this->mpMorf->isStop();
}

/* 022EA7BC */
u32 daNpc_Tc_c::setFinishMsg() {
    WWHD_FUNC(0x022EA7BC, u32, this);
    m_jnt.mbHeadLock = 1; /* onHeadLock() */
    return 0xDC7;
}
VERIFY(0x022EA7BC, &daNpc_Tc_c::setFinishMsg);

/* 022EA7CC */
u16 daNpc_Tc_c::next_msgStatusRed(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022EA7CC, u16, this, pMsgNo);
    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e;
    if (tc_waitDanceEnd(this)) {
        msgStatus = fopMsgStts_MSG_DISPLAYED_e;
    } else {
        switch ((u32)*pMsgNo) {
        case 0xDB7:
        case 0xDE1:
            *pMsgNo = 0xDE2;
            break;
        case 0xDE2:
            if (dComIfGs_getClearCount() != 0) {
                *pMsgNo = 0xDB8;
            } else {
                *pMsgNo = 0xDE3;
            }
            break;
        case 0xDB8:
        case 0xDE3:
            *pMsgNo = 0xDE4;
            break;
        case 0xDE4:
        case 0xDE5:
            *pMsgNo = setFinishMsg();
            break;
        case 0xDE6:
            if (dComIfGs_isEventBit(0x1A20)) {
                *pMsgNo = 0xDEB;
            } else {
                *pMsgNo = 0xDE7;
            }
            field_0x814 = 1;
            break;
        case 0xDEB:
            if (field_0x814 && dComIfGs_isEventBit(0x1A20)) {
                *pMsgNo = 0xDEC;
            } else {
                *pMsgNo = setFinishMsg();
            }
            break;
        case 0xDEC:
            *pMsgNo = 0xDED;
            break;
        case 0xDED:
            *pMsgNo = setFinishMsg();
            break;
        case 0xDE7:
            *pMsgNo = 0xDE8;
            break;
        case 0xDE8:
            *pMsgNo = 0xDE9;
            break;
        case 0xDE9:
            *pMsgNo = 0xDEA;
            break;
        case 0xDEA:
            *pMsgNo = setFinishMsg();
            break;
        case 0xDEE:
            *pMsgNo = 0xDEF;
            break;
        case 0xDEF:
            mStatus = STATUS_GET_RUPEE;
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        case 0xDF1:
            *pMsgNo = 0xDF2;
            break;
        case 0xDF2:
            mStatus = STATUS_MONUMENT_COMPLETE;
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        case 0xDF3:
            *pMsgNo = 0xDF4;
            break;
        case 0xDF4:
            *pMsgNo = 0xDF5;
            break;
        case 0xDF5:
        case 0xDF0:
            *pMsgNo = setFinishMsg();
            break;
        case 0xDC7:
            m_jnt.mbHeadLock = 1; /* onHeadLock() */
            msgStatus = fopMsgStts_MSG_ENDS_e;
            field_0x814 = 0;
            break;
        default:
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        }
    }
    return msgStatus;
}
VERIFY(0x022EA7CC, &daNpc_Tc_c::next_msgStatusRed);

/* 022EAA84 */
bool daNpc_Tc_c::existTcMonument() {
    WWHD_FUNC(0x022EAA84, bool, this);
    if (dComIfGs_isStageTbox(3 /* STAGE_DRC */, 0xF) || dComIfGs_isStageTbox(4 /* STAGE_FW */, 0xF) ||
        dComIfGs_isStageTbox(5 /* STAGE_TOTG */, 0xF) || dComIfGs_isStageTbox(7 /* STAGE_WT */, 0xF) ||
        dComIfGs_isStageTbox(6 /* STAGE_ET */, 0xF)) {
        return true;
    }
    return false;
}
VERIFY(0x022EAA84, &daNpc_Tc_c::existTcMonument);

/* 022EAB1C */
u16 daNpc_Tc_c::next_msgStatusWhite(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022EAB1C, u16, this, pMsgNo);
    m_jnt.mbHeadLock = 0; /* offHeadLock() */
    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e;
    if (tc_waitDanceEnd(this)) {
        msgStatus = fopMsgStts_MSG_DISPLAYED_e;
    } else {
        switch ((u32)*pMsgNo) {
        case 0xDC5:
            *pMsgNo = 0xDC6;
            break;
        case 0xDC6:
            *pMsgNo = setFinishMsg();
            break;
        case 0xDC8:
            if (dComIfGs_isEventBit(0xB80)) {
                if (!field_0x80E) {
                    field_0x80E = 1;
                    *pMsgNo = 0xDD7;
                }
            } else if (!field_0x80F) {
                field_0x80F = 1;
                *pMsgNo = 0xDC9;
            }
            break;
        case 0xDD7:
            *pMsgNo = 0xDD8;
            break;
        case 0xDD8:
            *pMsgNo = setFinishMsg();
            break;
        case 0xDC9:
            if (msg_selectNum(tc_msgManager()) == 0) {
                *pMsgNo = 0xDCA;
            } else {
                *pMsgNo = 0xDD3;
            }
            break;
        case 0xDCA: *pMsgNo = 0xDCB; break;
        case 0xDCB: *pMsgNo = 0xDCC; break;
        case 0xDCC: *pMsgNo = 0xDCD; break;
        case 0xDCD: *pMsgNo = 0xDCE; break;
        case 0xDCE: *pMsgNo = 0xDCF; break;
        case 0xDCF: *pMsgNo = 0xDD0; break;
        case 0xDD0: *pMsgNo = 0xDD1; break;
        case 0xDD1: *pMsgNo = 0xDD2; break;
        case 0xDD2:
            *pMsgNo = setFinishMsg();
            break;
        case 0xDD3: *pMsgNo = 0xDD4; break;
        case 0xDD4: *pMsgNo = 0xDD5; break;
        case 0xDD5:
            *pMsgNo = setFinishMsg();
            break;
        case 0xDC7:
            m_jnt.mbHeadLock = 1; /* onHeadLock() */
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        case 0xDD9:
            *pMsgNo = 0xDDA;
            break;
        case 0xDDA:
            if (existTcMonument()) {
                *pMsgNo = setFinishMsg();
            } else {
                *pMsgNo = 0xDDB;
            }
            break;
        case 0xDDB:
            *pMsgNo = 0xDDC;
            break;
        case 0xDDC:
            *pMsgNo = setFinishMsg();
            break;
        default:
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        }
    }
    return msgStatus;
}
VERIFY(0x022EAB1C, &daNpc_Tc_c::next_msgStatusWhite);

/* 022EADA4 */
u16 daNpc_Tc_c::next_msgStatusBlue(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022EADA4, u16, this, pMsgNo);
    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e;
    if (tc_waitDanceEnd(this)) {
        msgStatus = fopMsgStts_MSG_DISPLAYED_e;
    } else {
        switch ((u32)*pMsgNo) {
        case 0xDDD:
            if (dComIfGs_isStageTbox(3, 0xF) || dComIfGs_isStageTbox(4, 0xF) || dComIfGs_isStageTbox(5, 0xF) ||
                dComIfGs_isStageTbox(7, 0xF) || dComIfGs_isStageTbox(6, 0xF)) {
                *pMsgNo = 0xDDE;
            } else {
                *pMsgNo = 0xDF9;
            }
            break;
        case 0xDDE:
            *pMsgNo = 0xDDF;
            break;
        case 0xDDF:
            *pMsgNo = 0xDE0;
            break;
        case 0xDF9:
            *pMsgNo = 0xDFA;
            break;
        default:
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        }
    }
    return msgStatus;
}
VERIFY(0x022EADA4, &daNpc_Tc_c::next_msgStatusBlue);

/* 022EAF04 */
u16 daNpc_Tc_c::next_msgStatusNormal(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022EAF04, u16, this, pMsgNo);
    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e;
    if (tc_waitDanceEnd(this)) {
        msgStatus = fopMsgStts_MSG_DISPLAYED_e;
    } else {
        switch ((u32)*pMsgNo) {
        case 0x10CD: *pMsgNo = 0x10CE; break;
        case 0x10DF: *pMsgNo = 0x10CE; break;
        case 0x10CE: *pMsgNo = 0x10CF; break;
        case 0x10CF: *pMsgNo = 0x10D0; break;
        case 0x10D0: *pMsgNo = 0x10D1; break;
        case 0x10D1: *pMsgNo = 0x10D2; break;
        case 0x10D3: *pMsgNo = 0x10D2; break;
        case 0x10D4: *pMsgNo = 0x10D5; break;
        case 0x10D5: *pMsgNo = 0x10D6; break;
        case 0x10D6: *pMsgNo = 0x10D7; break;
        case 0x10D7: *pMsgNo = 0x10D8; break;
        case 0x10D9: *pMsgNo = 0x10D8; break;
        case 0x10DA: *pMsgNo = 0x10DB; break;
        case 0x10DB: *pMsgNo = 0x10DC; break;
        case 0x10DD: *pMsgNo = 0x10DE; break;
        case 0xDAD: *pMsgNo = 0xDAE; break;
        case 0xDAF: *pMsgNo = 0xDB0; break;
        default:
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        }
    }
    return msgStatus;
}
VERIFY(0x022EAF04, &daNpc_Tc_c::next_msgStatusNormal);

/* 022EB090 */
void daNpc_Tc_c::stopTower() {
    WWHD_FUNC(0x022EB090, void, this);
    if (m_tower_actor.get() == nullptr) /* JUT_ASSERT(1023, m_tower_actor != NULL) */
        JUT_ASSERT_fail(STR(0x10022B60), 0x3FF, STR(0x10022B70));
    fopAc_ac_c* tower = m_tower_actor;
    switch (mType) {
    case TYPE_RED:
    case TYPE_WHITE:
        m_jnt.mbHeadLock = 0; /* offHeadLock() */
        mAnmPrmIdx = ANM_PRM_IDX_WAIT04;
        smplbg_onStop(tower);
        break;
    case TYPE_NORMAL2:
        smplbg_onStop(tower);
        break;
    }
}
VERIFY(0x022EB090, &daNpc_Tc_c::stopTower);

/* 022EB10C (unnamed by the matcher) */
bool daNpc_Tc_c::existUnknownCollectMap() {
    WWHD_FUNC(0x022EB10C, bool, this);
    for (int i = 1; i <= 8; i++) {
        if (!dComIfGs_isCollectMapTriforce(i) && dComIfGs_isGetCollectMap(i)) {
            return true;
        }
    }
    return false;
}
VERIFY(0x022EB10C, &daNpc_Tc_c::existUnknownCollectMap);

/* 022EB1AC (unnamed by the matcher) */
bool daNpc_Tc_c::existKnownCollectMap() {
    WWHD_FUNC(0x022EB1AC, bool, this);
    for (int i = 1; i <= 8; i++) {
        if (dComIfGs_isGetCollectMap(i) && dComIfGs_isCollectMapTriforce(i)) {
            return true;
        }
    }
    return false;
}
VERIFY(0x022EB1AC, &daNpc_Tc_c::existKnownCollectMap);

/* 022EB24C */
u32 daNpc_Tc_c::getMsgNormal2() {
    WWHD_FUNC(0x022EB24C, u32, this);
    u32 msg;
    stopTower();
    if (field_0x80D) {
        if (existUnknownCollectMap()) {
            msg = 0xDC3;
            field_0x80D = 0;
        } else {
            msg = 0xDC4;
        }
    } else if (!field_0x808) {
        field_0x808 = 1;
        if (dComIfGs_isEventBit(0x1A20)) {
            if (dComIfGs_isEventBit(0x1708)) {
                msg = 0xDB3;
            } else {
                msg = 0xDFB;
            }
        } else {
            msg = 0xDB2;
        }
    } else if (existUnknownCollectMap()) {
        if (!field_0x809) {
            field_0x809 = 1;
            msg = 0xDBB;
        } else {
            msg = 0xDBE;
        }
    } else {
        /* HD: the Japanese version's code (no setRupeeSizeMsg / wallet messages) */
        if (existKnownCollectMap()) {
            msg = 0xDBA;
        } else {
            msg = 0xDB5;
        }
    }
    return msg;
}
VERIFY(0x022EB24C, &daNpc_Tc_c::getMsgNormal2);

/* 022EB35C */
u16 daNpc_Tc_c::next_msgStatusNormal2(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022EB35C, u16, this, pMsgNo);
    u16 msgStatus = fopMsgStts_MSG_CONTINUES_e;
    if (tc_waitDanceEnd(this)) {
        msgStatus = fopMsgStts_MSG_DISPLAYED_e;
    } else {
        /* HD: no 0x38AA..0x38AD (wallet size) messages, as in the Japanese version */
        switch ((u32)*pMsgNo) {
        case 0xDB2:
        case 0xDB3:
        case 0xDFB:
            *pMsgNo = 0xDB4;
            break;
        case 0xDB4:
            *pMsgNo = getMsgNormal2();
            break;
        case 0xDB5:
            *pMsgNo = 0xDBA;
            break;
        case 0xDBB:
            *pMsgNo = 0xDBC;
            break;
        case 0xDBC:
            *pMsgNo = 0xDBD;
            break;
        case 0xDBD:
            *pMsgNo = 0xDBE;
            break;
        case 0xDBE:
            if (msg_selectNum(tc_msgManager()) == 0) {
                if (dComIfGs_getRupee() >= 398) {
                    mStatus = STATUS_PAY_RUPEE;
                    field_0x80D = 1;
                    field_0x80C = 1;
                    msgStatus = fopMsgStts_MSG_ENDS_e;
                } else {
                    *pMsgNo = 0xDC0;
                }
            } else {
                *pMsgNo = 0xDBF;
            }
            break;
        case 0xDC0:
            if (field_0x80B) {
                *pMsgNo = 0xDC4;
            } else {
                *pMsgNo = 0xDC1;
            }
            break;
        case 0xDBF:
            *pMsgNo = 0xDC1;
            break;
        case 0xDC3:
            if (msg_selectNum(tc_msgManager()) == 0) {
                if (dComIfGs_getRupee() >= 398) {
                    mStatus = STATUS_PAY_RUPEE;
                    field_0x80D = 1;
                    field_0x80C = 1;
                    msgStatus = fopMsgStts_MSG_ENDS_e;
                } else {
                    field_0x80B = 1;
                    *pMsgNo = 0xDC0;
                }
            } else {
                *pMsgNo = 0xDC4;
            }
            break;
        case 0xDC4:
            field_0x80D = 0;
            field_0x80B = 0;
            /* fallthrough */
        default:
            msgStatus = fopMsgStts_MSG_ENDS_e;
            break;
        }
    }
    return msgStatus;
}
VERIFY(0x022EB35C, &daNpc_Tc_c::next_msgStatusNormal2);

/* 022EB55C */
u16 daNpc_Tc_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022EB55C, u16, this, pMsgNo);
    u16 msg_status;
    switch (mType) {
    case TYPE_NORMAL:
        msg_status = next_msgStatusNormal(pMsgNo);
        break;
    case TYPE_NORMAL2:
        msg_status = next_msgStatusNormal2(pMsgNo);
        break;
    case TYPE_BLUE:
        msg_status = next_msgStatusBlue(pMsgNo);
        break;
    case TYPE_RED:
        msg_status = next_msgStatusRed(pMsgNo);
        break;
    case TYPE_WHITE:
        msg_status = next_msgStatusWhite(pMsgNo);
        break;
    default:
        msg_status = fopMsgStts_MSG_ENDS_e;
        break;
    }
    return msg_status;
}
VERIFY(0x022EB55C, &daNpc_Tc_c::next_msgStatus);

/* 022EB5D0 (unnamed by the matcher) */
u32 daNpc_Tc_c::setFirstMsg(be<u8>* param_1, u32 param_2, u32 param_3) {
    WWHD_FUNC(0x022EB5D0, u32, this, param_1, param_2, param_3);
    if (!*param_1) {
        *param_1 = 1;
    } else {
        param_2 = param_3;
    }
    return param_2;
}
VERIFY(0x022EB5D0, &daNpc_Tc_c::setFirstMsg);

/* 022EB5F4 */
u32 daNpc_Tc_c::getMsgRed() {
    WWHD_FUNC(0x022EB5F4, u32, this);
    stopTower();
    if (field_0x813) {
        field_0x813 = 0;
        return 0xDF3;
    }
    if (field_0x812) {
        field_0x812 = 0;
        if (gabi::call<bool>(0x022E979C, this) /* checkAllMonumentFee (d_a_npc_tc_cut.cpp) */) {
            return 0xDF1;
        }
        return 0xDF0;
    }
    if (dComIfGs_isEventBit(0xB80)) {
        if (!field_0x810) {
            field_0x810 = 1;
            if (dComIfGs_isStageTbox(3 /* STAGE_DRC */, 0xF) && !dComIfGs_isEventBit(0x1240)) {
                return 0xDEE;
            }
            if (dComIfGs_isStageTbox(4 /* STAGE_FW */, 0xF) && !dComIfGs_isEventBit(0x1D08)) {
                return 0xDEE;
            }
            if (dComIfGs_isStageTbox(5 /* STAGE_TOTG */, 0xF) && !dComIfGs_isEventBit(0x1D04)) {
                return 0xDEE;
            }
            if (dComIfGs_isStageTbox(7 /* STAGE_WT */, 0xF) && !dComIfGs_isEventBit(0x1D02)) {
                return 0xDEE;
            }
            if (dComIfGs_isStageTbox(6 /* STAGE_ET */, 0xF) && !dComIfGs_isEventBit(0x1D01)) {
                return 0xDEE;
            }
            return 0xDE6;
        }
        if (dComIfGs_isEventBit(0x1A20)) {
            return 0xDEB;
        }
        return 0xDEA;
    }
    if (dComIfGs_getClearCount() != 0) {
        return setFirstMsg(&field_0x811, 0xDB7, 0xDE5);
    }
    return setFirstMsg(&field_0x811, 0xDE1, 0xDE5);
}
VERIFY(0x022EB5F4, &daNpc_Tc_c::getMsgRed);

/* 022EB7E8 */
u32 daNpc_Tc_c::getMsgWhite() {
    WWHD_FUNC(0x022EB7E8, u32, this);
    u32 msg;
    stopTower();
    if (dComIfGs_isEventBit(0x1708)) {
        msg = 0xDD9;
        m_jnt.mbHeadLock = 1;
    } else if (dComIfGs_isEventBit(0xB80)) {
        if (!field_0x80E) {
            msg = 0xDC8;
            m_jnt.mbHeadLock = 1;
        } else {
            msg = 0xDD6;
        }
    } else if (!field_0x80F) {
        msg = 0xDC8;
        m_jnt.mbHeadLock = 1;
    } else {
        msg = 0xDC5;
        m_jnt.mbHeadLock = 1;
    }
    return msg;
}
VERIFY(0x022EB7E8, &daNpc_Tc_c::getMsgWhite);

/* 022EB89C */
u32 daNpc_Tc_c::getMsgNormal() {
    WWHD_FUNC(0x022EB89C, u32, this);
    u32 msg;
    if (!dComIfGs_isEventBit(0xB40)) {
        msg = 0x10D3;
        mHasTalkedNearJail = 1;
    } else {
        msg = 0x10D9;
    }
    return msg;
}
VERIFY(0x022EB89C, &daNpc_Tc_c::getMsgNormal);

/* 022EB8F0 */
u32 daNpc_Tc_c::getMsgBlue() {
    WWHD_FUNC(0x022EB8F0, u32, this);
    return 0xDDD;
}
VERIFY(0x022EB8F0, &daNpc_Tc_c::getMsgBlue);

/* 022EB8F8 */
u32 daNpc_Tc_c::getMsg() {
    WWHD_FUNC(0x022EB8F8, u32, this);
    u32 msg = 0;
    switch (mType) {
    case TYPE_NORMAL:
        msg = getMsgNormal();
        break;
    case TYPE_NORMAL2:
        msg = getMsgNormal2();
        break;
    case TYPE_BLUE:
        msg = getMsgBlue();
        break;
    case TYPE_RED:
        msg = getMsgRed();
        break;
    case TYPE_WHITE:
        msg = getMsgWhite();
        break;
    }
    return msg;
}
VERIFY(0x022EB8F8, &daNpc_Tc_c::getMsg);

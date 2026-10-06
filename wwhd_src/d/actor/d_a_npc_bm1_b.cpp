/**
 * d_a_npc_bm1_b.cpp (WWHD)
 * NPC - Rito (generic Ritos on Dragon Roost Island): look-back, movement, execute/draw,
 * animation attributes, messages, status/action functions
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_bm1.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Part B: 021FFFFC .. 02204358 (the rest is in d_a_npc_bm1.cpp).
 */
#include "d/actor/d_a_npc_bm1.h"

/* ---- local bindings (SHARED-CANDIDATE; most are the same as in d_a_npc_ba1.cpp) ---- */
static inline u32 dComIfGs_save() { return gabi::load<u32>(0x101F84DC); }
static inline dSv_event_c* dComIfGs_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL dComIfGs_isEventBit(u16 f) { return dSv_event_isEventBit(dComIfGs_event(), f); }
static inline void dComIfGs_onEventBit(u16 f) { dSv_event_onEventBit(dComIfGs_event(), f); }
static inline u8 dComIfGp_event_getTalkXYBtn() { return gabi::load<u8>(dComIfGp_ea() + 0x52B0); }
static inline BOOL dComIfGp_event_chkTalkXY() { return (u32)(dComIfGp_event_getTalkXYBtn() - 1) <= 3; }
static inline u8 dComIfGp_event_getPreItemNo() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
static inline BOOL dComIfGp_evmng_ChkPresentEnd() { return gabi::call<BOOL>(0x02544950, dComIfGp_getPEvtManager()); }
static inline u8 dComIfGp_getMesgAnimeAttrInfo() { return gabi::load<u8>(dComIfGp_ea() + PLAY_BGS + 0x4925); }
/* dComIfG_MesgCamInfo_c at play + 0x5C30: mBasicID +4, mActor[10] +8 */
static inline u32 dComIfGp_mesgCamInfo() { return dComIfGp_ea() + 0x5C30; }
/* 0259DED0 dNpc_JntCtrl_c::lookAtTarget(s16* outY, cXyz* target, cXyz eye (pointer to a copy), s16 yrot, s16 maxVel, bool headOnly) */
static inline void dNpc_JntCtrl_lookAtTarget(dNpc_JntCtrl_c* j, be<s16>* outY, cXyz* target, cXyz* eye, s16 yrot, s16 vel, u8 headOnly) {
    gabi::call(0x0259DED0, j, outY, target, eye, yrot, vel, headOnly);
}
static inline void dNpc_playerEyePos_l(cXyz* out, f32 offs) { gabi::call(0x0259D54C, out, offs); }
/* HD message manager (*(0x101F4B5C)) */
static inline u8 dComIfGs_getEventReg(u16 reg) { return dSv_event_getEventReg(dComIfGs_event(), reg); }
static inline void dComIfGs_setEventReg(u16 reg, u8 v) { dSv_event_setEventReg(dComIfGs_event(), reg, v); }
/* 025B7D90 dSv_player_collect_c::isSymbol (save + 0xD4) */
static inline BOOL dComIfGs_isSymbol(u8 i) { return gabi::call<BOOL>(0x025B7D90, dComIfGs_save() + 0xD4, i); }
/* 025B7840 dSv_player_get_bag_item_c::isReserve (save + 0xB0) */
static inline BOOL dComIfGs_isGetItemReserve(u8 i) { return gabi::call<BOOL>(0x025B7840, dComIfGs_save() + 0xB0, i); }
/* dComIfGs_getBeastNum(dBeastIdx_GOLDEN_FEATHER_e): save + 0xBE */
static inline u8 dComIfGs_getBeastNum_GoldenFeather() { return gabi::load<u8>(dComIfGs_save() + 0xBE); }
/* dComIfGp_setItemBeastNumCount(dBeastIdx_GOLDEN_FEATHER_e, n): play + 0x5B74 (s16) += n */
static inline void dComIfGp_setItemBeastNumCount_GoldenFeather(s16 n) {
    u32 a = dComIfGp_ea() + 0x5B74;
    gabi::store<s16>(a, (s16)(gabi::load<s16>(a) + n));
}
/* dAttention_c (play + 0x5804); HD: 024EC8D0 is LockonTarget, 024EE464 ActionTarget (matcher swapped, see ba1) */
static inline bool dAttention_LockonTruth(dAttention_c* a) { return gabi::call<bool>(0x024EDFCC, a); }
static inline fopAc_ac_c* dAttention_LockonTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EC8D0, a, i); }
static inline fopAc_ac_c* dAttention_ActionTarget(dAttention_c* a, s32 i) { return gabi::call<fopAc_ac_c*>(0x024EE464, a, i); }
/* 025F795C: the message manager's current status (matcher: fopMsgM_SearchByID) */
static inline u32 dMsgMng_getStatus(u32 mng) { return gabi::call<u32>(0x025F795C, mng); }
/* 02055B64 cLib_calcTimer<s16> (out of line) */
static inline s16 cLib_calcTimer(be<s16>* t) { return gabi::call<s16>(0x02055B64, t); }
/* g_Counter.mCounter0 */
static inline u32 g_Counter_mCounter0() { return gabi::load<u32>(0x101FF558); }
/* dNpc_PathRun_c: 0259E778 getPoint(idx) returns cXyz through a hidden result pointer (r4);
 * 0259EE4C pointArg(idx) */
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_bm1* p, cXyz* out, u8 idx) { gabi::call(0x0259E778, p, out, idx); }
static inline u8 dNpc_PathRun_pointArg(dNpc_PathRun_bm1* p, u8 idx) { return gabi::call<u8>(0x0259EE4C, p, idx); }
static inline BOOL dNpc_PathRun_chkPointPass(dNpc_PathRun_bm1* p, cXyz* pos /* by value: a copy */, u32 dir) {
    return gabi::call<BOOL>(0x0259E838, p, pos, dir);
}
static inline void dNpc_PathRun_nextIdxAuto(dNpc_PathRun_bm1* p) { gabi::call(0x0259ED58, p); }
static inline s32 dNpc_PathRun_maxPoint(dNpc_PathRun_bm1* p) { return gabi::call<s32>(0x0259EDB8, p); }
/* dPath_ChkClose(path): bit 0 of the path's loop flag (+5) */
static inline bool dPath_ChkClose(dPath* p) { return (gabi::load<u8>(gabi::ea(p) + 5) & 1) != 0; }
/* 02586D5C dLetter_send(u16) */
static inline void dLetter_send(u16 no) { gabi::call(0x02586D5C, no); }
/* dBgS_GndChk on the stack (0x54), this TU's vtables */
struct GndChk_bm1 { u8 _00[0x54]; };
static const dBgS_GndChk_vt kGndChkVt = {0x10016FA8, 0x10016FB8, 0x10016FD8, 0x10016FC8};
/* (this->*pmf)(arg): GHS pointer to member (8 bytes; i < 0: not virtual) */
static inline void pmf_call(void* self, ProcFunc_l* pmf, void* arg) {
    s16 i = pmf->i;
    u32 thisp = gabi::ea(self) + (s32)(s16)pmf->d;
    if (i < 0) {
        gabi::call_ptr<BOOL>(pmf->f, thisp, arg);
    } else {
        s16 voff = gabi::load<s16>(gabi::ea(pmf) + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr<BOOL>(gabi::load<u32>(vt + i * 8 + 4), thisp, arg);
    }
}
static inline u8 dComIfGp_event_runCheck() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
/* function-local static GXColor initialised on first use (guard word, then a memcpy from .data) */
static inline void static_color_init(u32 guard, u32 obj, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(obj), gabi::at<u8>(src), 4); /* 028FEAC0 memcpy */
    }
}
/* fsel: a >= 0 ? c : b (NaN: b) -- cLib_maxLimit / cLib_minLimit as GHS compiled them */
static inline f32 fsel(f32 a, f32 c, f32 b) { return a >= 0.0f ? c : b; }
static inline u32 fopMsgM_getStatus() { return gabi::call<u32>(0x025F795C, gabi::load<u32>(0x101F4B5C)); }

/* daNpc_Bm1_HIO_c l_HIO (0x1046623C, size 0x354): vtable 0, m4 +4, m8 +8, children[10] +0xC
 * (daNpc_Bm1_childHIO_c 0x54: vtable 0, hio_prm +4) */
struct bm1_hio_prm_c {
    /* 0x00 */ be<s16> mMaxHeadX;
    /* 0x02 */ be<s16> mMaxHeadY;
    /* 0x04 */ be<s16> mMinHeadX;
    /* 0x06 */ be<s16> mMinHeadY;
    /* 0x08 */ be<s16> mMaxBackboneX;
    /* 0x0A */ be<s16> mMaxBackboneY;
    /* 0x0C */ be<s16> mMinBackboneX;
    /* 0x0E */ be<s16> mMinBackboneY;
    /* 0x10 */ be<s16> mMaxTurnStep;
    /* 0x12 */ be<s16> mCalcAngleTarget;
    /* 0x14 */ be<f32> mAttPosOffsetY;
    /* 0x18 */ be<u8> m18;
    /* 0x19 */ u8 _19;
    /* 0x1A */ be<s16> mFlyScale;
    /* 0x1C */ be<s16> mFlyMaxStep;
    /* 0x1E */ u8 _1E[2];
    /* 0x20 */ be<f32> m20;
    /* 0x24 */ be<f32> m24;
    /* 0x28 */ be<f32> m28;
    /* 0x2C */ be<f32> m2C;
    /* 0x30 */ be<f32> m30;
    /* 0x34 */ be<s16> m34;
    /* 0x36 */ be<s16> m36;
    /* 0x38 */ be<f32> m38;
    /* 0x3C */ be<f32> m3C;
    /* 0x40 */ be<f32> m40;
    /* 0x44 */ be<f32> m44;
    /* 0x48 */ be<f32> m48;
};
WWHD_SIZE(bm1_hio_prm_c, 0x4C);
struct daNpc_Bm1_childHIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ bm1_hio_prm_c hio_prm;
    /* 0x50 */ be<u32> m50;
};
WWHD_SIZE(daNpc_Bm1_childHIO_c, 0x54);
struct daNpc_Bm1_HIO_c {
    /* 0x000 */ be<u32> __vtbl;
    /* 0x004 */ be<s8> m4;
    /* 0x005 */ u8 _5[3];
    /* 0x008 */ be<s32> m8;
    /* 0x00C */ daNpc_Bm1_childHIO_c children[10];
};
WWHD_SIZE(daNpc_Bm1_HIO_c, 0x354);
static daNpc_Bm1_HIO_c& l_HIO() { return *gabi::at<daNpc_Bm1_HIO_c>(0x1046623C); }
/* l_HIO.children[mType - 1].hio_prm */
static bm1_hio_prm_c& hio_prm(s8 type) { return l_HIO().children[type - 1].hio_prm; }

/* 021FFFFC */
void daNpc_Bm1_c::lookBack() {
    WWHD_FUNC(0x021FFFFC, void, this);
    gabi::Local<cXyz> dstPos;
    cXyz* dstPos_p;
    s16 desiredYrot = current.angle.y;
    m860 = desiredYrot;
    m85E = m_jnt.mAngles[1][1];
    dstPos->set(0.0f, 0.0f, 0.0f);
    f32 srcY = eyePos.y;
    f32 srcX = current.pos.x;
    u8 headOnlyFollow = m896;
    dstPos_p = nullptr;
    f32 srcZ = current.pos.z;
    m85C = m_jnt.mAngles[0][1];

    switch ((u32)(s32)mLookBackState) {
    case 1: {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos_l(eye, -20.0f);
        dstPos->copy(*eye);
        srcX = current.pos.x;
        srcZ = current.pos.z;
        srcY = eyePos.y;
        dstPos_p = dstPos;
        break;
    }
    case 2:
        dstPos->copy(m82C);
        srcX = current.pos.x;
        srcY = eyePos.y;
        srcZ = current.pos.z;
        dstPos_p = dstPos;
        break;
    case 3:
        desiredYrot = m876;
        break;
    }
    cLib_addCalcAngleS2(&mHeadLookAtMaxVel, hio_prm(mType).mCalcAngleTarget, 4, 0x800);
    if (!m_jnt.mbTrn) { /* !m_jnt.trnChk() */
        mHeadLookAtMaxVel = 0;
    }
    gabi::Local<cXyz> src_pos; /* passed by value: a copy on the stack */
    src_pos->x = srcX;
    src_pos->y = srcY;
    src_pos->z = srcZ;
    dNpc_JntCtrl_lookAtTarget(&m_jnt, &current.angle.y, dstPos_p, src_pos, desiredYrot, mHeadLookAtMaxVel, headOnlyFollow);
}
VERIFY(0x021FFFFC, &daNpc_Bm1_c::lookBack);

/* 02200A40 */
void daNpc_Bm1_c::bm_clcMovSpd() {
    WWHD_FUNC(0x02200A40, void, this);
    s16 target = cLib_targetAngleY(&current.pos, &mTargetPos);
    bm1_hio_prm_c& prm = hio_prm(mType);
    cLib_addCalcAngleS(&current.angle.y, target, prm.m34, prm.m36, 0);
    cLib_chaseF(&speedF, mTargetFlySpeed, mTargetFlyStep);
}
VERIFY(0x02200A40, &daNpc_Bm1_c::bm_clcMovSpd);

/* 02200AB4 */
void daNpc_Bm1_c::setPlaySpd(f32 i_speed) {
    WWHD_FUNC(0x02200AB4, void, this, i_speed);
    if (mbHasArms) {
        mpArmMorf->setPlaySpeed(i_speed);
        mpHeadMorf->setPlaySpeed(i_speed);
        mpMorf->setPlaySpeed(i_speed);
    } else {
        mpWingMorf->setPlaySpeed(i_speed);
        mpHeadMorf->setPlaySpeed(i_speed);
        mpMorf->setPlaySpeed(i_speed);
    }
}
VERIFY(0x02200AB4, &daNpc_Bm1_c::setPlaySpd);

/* 02200CD0 */
void daNpc_Bm1_c::eventOrder() {
    WWHD_FUNC(0x02200CD0, void, this);
    s8 condition = m8FD;
    if (condition == 1 || condition == 2) {
        eventInfo_onCondition(this, 1); /* dEvtCnd_CANTALK_e */
        if (mSpecificType == SPECIFIC_TYPE_Hoskit_e) {
            eventInfo_onCondition(this, 0x20); /* dEvtCnd_CANTALKITEM_e */
        }
        if (m8FD == 1) {
            fopAcM_orderSpeakEvent(this);
        }
    } else if (condition >= 3) {
        mEventIdx = condition - 3;
        fopAcM_orderOtherEventId(this, mEventIdTable[mEventIdx], 0xFF, 0xFFFF, 0, 1);
    }
}
VERIFY(0x02200CD0, &daNpc_Bm1_c::eventOrder);

/* 02200F8C */
static BOOL daNpc_Bm1_Execute(daNpc_Bm1_c* i_this) {
    WWHD_FUNC(0x02200F8C, BOOL, i_this);
    return i_this->_execute();
}
VERIFY(0x02200F8C, daNpc_Bm1_Execute);

/* 0220124C */
static BOOL daNpc_Bm1_Draw(daNpc_Bm1_c* i_this) {
    WWHD_FUNC(0x0220124C, BOOL, i_this);
    return i_this->_draw();
}
VERIFY(0x0220124C, daNpc_Bm1_Draw);

/* 02201250 */
static BOOL daNpc_Bm1_IsDelete(daNpc_Bm1_c*) {
    WWHD_FUNC(0x02201250, BOOL, (daNpc_Bm1_c*)nullptr);
    return TRUE;
}
VERIFY(0x02201250, daNpc_Bm1_IsDelete);

/* 02201258 */
bool daNpc_Bm1_c::setAnm() {
    WWHD_FUNC(0x02201258, bool, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BC994);
    if (a_anm_prm_tbl[mStatus].btpNum >= 0) {
        setAnm_tex(a_anm_prm_tbl[mStatus].btpNum);
    }
    if (a_anm_prm_tbl[mStatus].anmNum >= 0) {
        setAnm_anm(&a_anm_prm_tbl[mStatus]);
    }
    return true;
}
VERIFY(0x02201258, &daNpc_Bm1_c::setAnm);

/* 022012DC */
void daNpc_Bm1_c::setAnm_ATR(int i_param_1) {
    WWHD_FUNC(0x022012DC, void, this, i_param_1);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101BCB10); /* [15] */
    if (i_param_1 != 0) {
        setAnm_tex(a_anm_prm_tbl[m8F7].btpNum);
    }
    setAnm_anm(&a_anm_prm_tbl[m8F7]);
}
VERIFY(0x022012DC, &daNpc_Bm1_c::setAnm_ATR);

/* 0220134C */
void daNpc_Bm1_c::chg_anmAtr(u8 i_param_1) {
    WWHD_FUNC(0x0220134C, void, this, i_param_1);
    if (i_param_1 >= 0xF || i_param_1 == m8F7) {
        return;
    }
    m8F7 = i_param_1;
    if (mSpecificType == SPECIFIC_TYPE_Skett_e || mSpecificType == SPECIFIC_TYPE_Akoot_e) {
        switch (m8F7) {
        case 9:
            mLookBackState = 1;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
            break;
        case 0xD:
            if (mPartnerProcID != 0xFFFFFFFF) {
                fopAc_ac_c* actor = searchByID(mPartnerProcID);
                m82C.copy(actor->eyePos);
                mLookBackState = 2;
            }
            break;
        }
    }
    setAnm_ATR(1);
}
VERIFY(0x0220134C, &daNpc_Bm1_c::chg_anmAtr);

/* 0220141C */
void daNpc_Bm1_c::control_anmAtr() {
    WWHD_FUNC(0x0220141C, void, this);
    switch (m8F7) {
    case 8:
        if (mbMorfAnimStopped) {
            m87B = m87B + 1;
            if (m87B >= 2) {
                m8F7 = 6;
                setAnm_NUM(0x16, 1);
            }
        }
        break;
    }
}
VERIFY(0x0220141C, &daNpc_Bm1_c::control_anmAtr);

/* 02201460 */
void daNpc_Bm1_c::anmAtr(u16 i_param_1) {
    WWHD_FUNC(0x02201460, void, this, i_param_1);
    u32 caminfo = dComIfGp_mesgCamInfo();
    if (mbManzai != 0 && gabi::ea(this) != gabi::load<u32>(caminfo + 4 + 4 * gabi::load<s32>(caminfo + 4))) {
        /* control_anmTag(): empty */
        control_anmAtr();
        return;
    }
    switch (i_param_1) {
    case 6: {
        if (m905 == 0) {
            m8F7 = 0xFF;
            chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
            m905 = m905 + 1;
        }
        u8 tag = gabi::load<u8>(dComIfGp_ea() + 0x5BC6); /* dComIfGp_getMesgAnimeTagInfo() */
        gabi::store<u8>(dComIfGp_ea() + 0x5BC6, 0xFF);    /* dComIfGp_clearMesgAnimeTagInfo() */
        if (tag != 0xFF && m8F8 != tag) {
            m8F8 = tag;
            /* chg_anmTag(): empty */
        }
        break;
    }
    case 0xE:
        m905 = 0;
        break;
    }
    /* control_anmTag(): empty */
    control_anmAtr();
}
VERIFY(0x02201460, &daNpc_Bm1_c::anmAtr);

/* 02201558 */
u8 daNpc_Bm1_c::chk_manzai() {
    WWHD_FUNC(0x02201558, u8, this);
    u8 o_retval = 1;
    if (mPartnerProcID != 0xFFFFFFFF) {
        daNpc_Bm1_c* actor = static_cast<daNpc_Bm1_c*>(searchByID(mPartnerProcID));
        if (actor == nullptr) {
            return 0;
        }
        mbManzai = actor->mStatus == 9;
        if (mbManzai) {
            /* dComIfGp_setMesgCameraInfoActor(this, actor, NULL x8) */
            u32 ci = dComIfGp_mesgCamInfo();
            gabi::store<u32>(ci + 8, gabi::ea(this));
            gabi::store<u32>(ci + 0xC, gabi::ea(actor));
            for (int i = 0x10; i < 0x30; i += 4)
                gabi::store<u32>(ci + i, 0);
        } else {
            actor->actor_status |= 0x4000; /* fopAcM_OnStatus(actor, fopAcStts_UNK4000_e) */
            actor->mbManzai = 1;          /* set_manzai() */
            o_retval = 0;
        }
    }
    return o_retval;
}
VERIFY(0x02201558, &daNpc_Bm1_c::chk_manzai);

/* 0220163C */
bool daNpc_Bm1_c::chk_talk() {
    WWHD_FUNC(0x0220163C, bool, this);
    bool ret = true;
    m87D = 0xFF;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            m87D = dComIfGp_event_getPreItemNo();
        } else {
            ret = false;
        }
    }
    return ret;
}
VERIFY(0x0220163C, &daNpc_Bm1_c::chk_talk);

/* 022016BC */
u8 daNpc_Bm1_c::chk_partsNotMove() {
    WWHD_FUNC(0x022016BC, u8, this);
    return m85C == m_jnt.mAngles[0][1] && m85E == m_jnt.mAngles[1][1] && m860 == current.angle.y;
}
VERIFY(0x022016BC, &daNpc_Bm1_c::chk_partsNotMove);

/* 022016FC */
u16 daNpc_Bm1_c::next_msgStatus(be<u32>* i_msg_no) {
    WWHD_FUNC(0x022016FC, u16, this, i_msg_no);
    u16 msg_status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    /* HD: the message (GameCube mpCurrMsg) is the message manager at *(0x101F4B5C) */
    u32 msg = gabi::load<u32>(0x101F4B5C);
    switch (*i_msg_no) {
    case 0x1771: *i_msg_no = 0x1772; break;
    case 0x1772: *i_msg_no = 0x1773; break;
    case 0x1773: *i_msg_no = 0x1774; break;
    case 0x1775: *i_msg_no = 0x1776; break;
    case 0x18A1: *i_msg_no = 0x18A2; break;
    case 0x5FE: *i_msg_no = 0x5FF; break;
    case 0x601: *i_msg_no = 0x602; break;
    case 0x602: *i_msg_no = 0x603; break;
    case 0x603: *i_msg_no = 0x604; break;
    case 0x604: *i_msg_no = 0x605; break;
    case 0x186B: *i_msg_no = 0x186C; break;
    case 0x186C: *i_msg_no = 0x186D; break;
    case 0x186D: *i_msg_no = 0x186E; break;
    case 0x1873: *i_msg_no = 0x1874; break;
    case 0x1874: *i_msg_no = 0x1875; break;
    case 0x1875: *i_msg_no = 0x1876; break;
    case 0x187B: *i_msg_no = 0x187C; break;
    case 0x187C: *i_msg_no = 0x187D; break;
    case 0x187D: *i_msg_no = 0x187E; break;
    case 0x1883: *i_msg_no = 0x1884; break;
    case 0x1884: *i_msg_no = 0x1885; break;
    case 0x1885: *i_msg_no = 0x1886; break;
    case 0x186F: *i_msg_no = 0x1870; break;
    case 0x1870: *i_msg_no = 0x1871; break;
    case 0x1871: *i_msg_no = 0x1872; break;
    case 0x1877: *i_msg_no = 0x1878; break;
    case 0x1878: *i_msg_no = 0x1879; break;
    case 0x1879: *i_msg_no = 0x187A; break;
    case 0x187F: *i_msg_no = 0x1880; break;
    case 0x1880: *i_msg_no = 0x1881; break;
    case 0x1881: *i_msg_no = 0x1882; break;
    case 0x1887: *i_msg_no = 0x1888; break;
    case 0x1888: *i_msg_no = 0x1889; break;
    case 0x1889: *i_msg_no = 0x188A; break;
    case 0x18A6: *i_msg_no = 0x18A7; break;
    case 0x18BC: *i_msg_no = 0x18BD; break;
    case 0x1996: *i_msg_no = 0x1997; break;
    case 0x1998: *i_msg_no = 0x1999; break;
    case 0x199E: *i_msg_no = 0x199F; break;
    case 0x199F:
        if (dComIfGs_getBeastNum_GoldenFeather() >= 20) {
            *i_msg_no = 0x19A1;
        } else {
            *i_msg_no = 0x19A0;
        }
        break;
    case 0x19A1:
        switch (gabi::load<u32>(msg + 0x948) /* mSelectNum */) {
        case 0:
            *i_msg_no = 0x19A3;
            dComIfGp_setItemBeastNumCount_GoldenFeather(-20);
            break;
        case 1:
            *i_msg_no = 0x19A2;
            break;
        }
        break;
    case 0x19A3: *i_msg_no = 0x19A4; break;
    case 0x1EF0: *i_msg_no = 0x1EF1; break;
    case 0x1EF1: *i_msg_no = 0x1EF2; break;
    case 0x1EF2: *i_msg_no = 0x1EF3; break;
    case 0x1EF3: *i_msg_no = 0x1EF4; break;
    case 0x1EF4: *i_msg_no = 0x1EF5; break;
    case 0x1EF5:
        switch (gabi::load<u32>(msg + 0x948) /* mSelectNum */) {
        case 0:
            *i_msg_no = 0x1EF6;
            break;
        case 1:
            *i_msg_no = 0x1EF8;
            break;
        }
        break;
    default:
        msg_status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return msg_status;
}
VERIFY(0x022016FC, &daNpc_Bm1_c::next_msgStatus);

/* 02201C70 */
s32 daNpc_Bm1_c::getBitMask() {
    WWHD_FUNC(0x02201C70, s32, this);
    s8 o_retval = 0;
    switch (mSpecificType) {
    case SPECIFIC_TYPE_Basht_e: o_retval = 1; break;
    case SPECIFIC_TYPE_Bisht_e: o_retval = 2; break;
    case SPECIFIC_TYPE_Pashli_e: o_retval = 4; break;
    case SPECIFIC_TYPE_Namali_e: o_retval = 8; break;
    case SPECIFIC_TYPE_Kogoli_e: o_retval = 0x10; break;
    }
    return o_retval;
}
VERIFY(0x02201C70, &daNpc_Bm1_c::getBitMask);

/* 02201CC4 */
u32 daNpc_Bm1_c::getMsg_PST_1() {
    WWHD_FUNC(0x02201CC4, u32, this);
    return dComIfGs_isEventBit(0x1401) ? 0xC93 : 0xC92;
}
VERIFY(0x02201CC4, &daNpc_Bm1_c::getMsg_PST_1);

/* 02201D04 */
u32 daNpc_Bm1_c::getMsg_PST_3() {
    WWHD_FUNC(0x02201D04, u32, this);
    return dComIfGs_isEventBit(0x2202) ? 0x18A3 : 0x18A1;
}
VERIFY(0x02201D04, &daNpc_Bm1_c::getMsg_PST_3);

/* 02201D44 */
u32 daNpc_Bm1_c::getMsg_SKT_0() {
    WWHD_FUNC(0x02201D44, u32, this);
    if (dComIfGs_isSymbol(1 /* dSymbol_DIN_e */)) {
        if (!dComIfGs_isEventBit(0x1A80)) {
            return 0x186B;
        } else if (!dComIfGs_isEventBit(0x1820)) {
            return 0x1873;
        } else {
            return !dComIfGs_isEventBit(0x2E04) ? 0x187B : 0x1883;
        }
    }
    return 0;
}
VERIFY(0x02201D44, &daNpc_Bm1_c::getMsg_SKT_0);

/* 02201E1C */
u32 daNpc_Bm1_c::getMsg_KKT_0() {
    WWHD_FUNC(0x02201E1C, u32, this);
    if (dComIfGs_isSymbol(1 /* dSymbol_DIN_e */)) {
        if (!dComIfGs_isEventBit(0x1A80)) {
            return 0x186F;
        } else if (!dComIfGs_isEventBit(0x1820)) {
            return 0x1877;
        } else {
            return !dComIfGs_isEventBit(0x2E04) ? 0x187F : 0x1887;
        }
    }
    return 0;
}
VERIFY(0x02201E1C, &daNpc_Bm1_c::getMsg_KKT_0);

/* getMsg_BMB_0/BMB_1/BMC_3/BMD_0 share one shape: a message per spawn condition (1..4),
 * depending on whether this Rito's bit is already set in event register 0xB8FF */
static u32 getMsg_spawnCond(daNpc_Bm1_c* i_this, const u16* known, const u16* unknown) {
    s8 reg = dComIfGs_getEventReg(0xB8FF);
    s32 mask = i_this->getBitMask();
    u32 msg = 0;
    const u16* tbl = (reg & mask) ? known : unknown;
    switch (i_this->mSpawnCondition) {
    case 1: msg = tbl[0]; break;
    case 2: msg = tbl[1]; break;
    case 3: msg = tbl[2]; break;
    case 4: msg = tbl[3]; break;
    }
    if (msg != 0) {
        dComIfGs_setEventReg(0xB8FF, (u8)(reg | mask));
    }
    return msg;
}

/* 02201EF4 (unnamed by the matcher) */
u32 daNpc_Bm1_c::getMsg_BMB_0() {
    WWHD_FUNC(0x02201EF4, u32, this);
    static const u16 known[] = {0x18A0, 0x18B1, 0x18B3, 0x18B5};
    static const u16 unknown[] = {0x189F, 0x18B0, 0x18B2, 0x18B4};
    return getMsg_spawnCond(this, known, unknown);
}
VERIFY(0x02201EF4, &daNpc_Bm1_c::getMsg_BMB_0);

/* 02201FF0 (unnamed by the matcher) */
u32 daNpc_Bm1_c::getMsg_BMB_1() {
    WWHD_FUNC(0x02201FF0, u32, this);
    static const u16 known[] = {0x18A9, 0x18BF, 0x18C1, 0x18C3};
    static const u16 unknown[] = {0x18A8, 0x18BE, 0x18C0, 0x18C2};
    return getMsg_spawnCond(this, known, unknown);
}
VERIFY(0x02201FF0, &daNpc_Bm1_c::getMsg_BMB_1);

/* 022020EC */
u32 daNpc_Bm1_c::getMsg_BMB_2() {
    WWHD_FUNC(0x022020EC, u32, this);
    if (m87C != 0) {
        return 0x199C;
    }
    if (m87D == 0x47) {
        if (dComIfGs_isEventBit(0x2180)) {
            return 0x199D;
        }
        if (dComIfGs_isEventBit(0x2140)) {
            if (dComIfGs_getBeastNum_GoldenFeather() >= 20) {
                return 0x19A1;
            } else {
                return 0x19A0;
            }
        }
        dComIfGs_onEventBit(0x2140);
        return 0x199E;
    } else if (m87D != 0xFF) {
        return 0x19A5;
    }
    if (dComIfGs_isEventBit(0x2180)) {
        return 0x199B;
    }
    if (dComIfGs_isEventBit(0x2001)) {
        return 0x199A;
    }
    if (dComIfGs_isEventBit(0x2120)) {
        return 0x1998;
    }
    return 0x1996;
}
VERIFY(0x022020EC, &daNpc_Bm1_c::getMsg_BMB_2);

/* 02202280 */
u32 daNpc_Bm1_c::getMsg_BMC_0() {
    WWHD_FUNC(0x02202280, u32, this);
    switch (mSpawnCondition) {
    case 1:
        return 0x18A6;
    case 2:
        return 0x18BA;
    case 3:
        return 0x18BC;
    case 4:
        break;
    }
    return 0;
}
VERIFY(0x02202280, &daNpc_Bm1_c::getMsg_BMC_0);

/* 022022B4 */
u32 daNpc_Bm1_c::getMsg_BMC_2() {
    WWHD_FUNC(0x022022B4, u32, this);
    if (m87C != 0) {
        return 0x1EF7;
    }
    if (dComIfGs_isGetItemReserve(0xF)) {
        return 0x1EF9;
    }
    return dComIfGs_isEventBit(0x1908) ? 0x1EF2 : 0x1EF0;
}
VERIFY(0x022022B4, &daNpc_Bm1_c::getMsg_BMC_2);

/* 02202354 (unnamed by the matcher) */
u32 daNpc_Bm1_c::getMsg_BMC_3() {
    WWHD_FUNC(0x02202354, u32, this);
    static const u16 known[] = {0x18C5, 0x18C7, 0x18C9, 0x18CB};
    static const u16 unknown[] = {0x18C4, 0x18C6, 0x18C8, 0x18CA};
    return getMsg_spawnCond(this, known, unknown);
}
VERIFY(0x02202354, &daNpc_Bm1_c::getMsg_BMC_3);

/* 02202450 (unnamed by the matcher) */
u32 daNpc_Bm1_c::getMsg_BMD_0() {
    WWHD_FUNC(0x02202450, u32, this);
    static const u16 known[] = {0x189E, 0x18AB, 0x18AD, 0x18AF};
    static const u16 unknown[] = {0x189D, 0x18AA, 0x18AC, 0x18AE};
    return getMsg_spawnCond(this, known, unknown);
}
VERIFY(0x02202450, &daNpc_Bm1_c::getMsg_BMD_0);

/* 0220254C */
u32 daNpc_Bm1_c::getMsg_BMD_1() {
    WWHD_FUNC(0x0220254C, u32, this);
    s8 reg = dComIfGs_getEventReg(0xB8FF);
    s32 mask = getBitMask();
    u32 msg = 0;
    if (reg & mask) {
        if (!dComIfGs_isEventBit(0x1820)) {
            msg = 0x18A5;
        } else {
            switch (mSpawnCondition) {
            case 1:
            case 2: msg = 0x18A5; break;
            case 3: msg = 0x18B7; break;
            case 4: msg = 0x18B9; break;
            }
        }
    } else {
        if (!dComIfGs_isEventBit(0x1820)) {
            msg = 0x18A4;
        } else {
            switch (mSpawnCondition) {
            case 1:
            case 2: msg = 0x18A4; break;
            case 3: msg = 0x18B6; break;
            case 4: msg = 0x18B8; break;
            }
        }
    }
    if (msg != 0) {
        dComIfGs_setEventReg(0xB8FF, (u8)(reg | mask));
    }
    return msg;
}
VERIFY(0x0220254C, &daNpc_Bm1_c::getMsg_BMD_1);

/* 0220269C */
u32 daNpc_Bm1_c::getMsg() {
    WWHD_FUNC(0x0220269C, u32, this);
    u32 o_retval = 0;
    switch (mSpecificType) {
    case SPECIFIC_TYPE_Quill_1_e: o_retval = getMsg_PST_1(); break;
    case SPECIFIC_TYPE_Quill_3_e: o_retval = getMsg_PST_3(); break;
    case SPECIFIC_TYPE_Skett_e: o_retval = getMsg_SKT_0(); break;
    case SPECIFIC_TYPE_Akoot_e: o_retval = getMsg_KKT_0(); break;
    case SPECIFIC_TYPE_Basht_e: o_retval = getMsg_BMB_0(); break;
    case SPECIFIC_TYPE_Bisht_e: o_retval = getMsg_BMB_1(); break;
    case SPECIFIC_TYPE_Hoskit_e: o_retval = getMsg_BMB_2(); break;
    case SPECIFIC_TYPE_Ilari_0xA_e: o_retval = getMsg_BMC_0(); break;
    case SPECIFIC_TYPE_Ilari_0xC_e: o_retval = getMsg_BMC_2(); break;
    case SPECIFIC_TYPE_Pashli_e: o_retval = getMsg_BMC_3(); break;
    case SPECIFIC_TYPE_Namali_e: o_retval = getMsg_BMD_0(); break;
    case SPECIFIC_TYPE_Kogoli_e: o_retval = getMsg_BMD_1(); break;
    }
    return o_retval;
}
VERIFY(0x0220269C, &daNpc_Bm1_c::getMsg);

/* 022027B0 */
u8 daNpc_Bm1_c::chkAttention() {
    WWHD_FUNC(0x022027B0, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x022027B0, &daNpc_Bm1_c::chkAttention);

/* 02202838 */
void daNpc_Bm1_c::setStt(s8 i_status) {
    WWHD_FUNC(0x02202838, void, this, i_status);
    s8 previous_status = mStatus;
    mStatus = i_status;
    m86E = 0;
    switch ((u32)(s32)i_status) {
    case 1:
        m870 = 0x50;
        break;
    case 3:
        m880 = 1;
        m870 = 0x5A;
        break;
    case 5:
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        m8F7 = 0xFF;
        mLookBackState = 1;
        m8FF = previous_status;
        return;
    case 8:
        m870 = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        break;
    case 9:
        m8F7 = 0xFF;
        m8FF = previous_status;
        return;
    case 10: {
        gabi::Local<cXyz> local_20;
        gabi::Local<cXyz> rel;
        mLookBackState = 0;
        local_20->set(0.0f, 9999.0f, 100.0f);
        eInit_calcRelativPos(rel, local_20, nullptr);
        mTargetFlySpeed = 0.0f;
        m870 = 0x5A;
        mTargetPos.copy(*rel);
        mTargetFlyStep = 0.0f;
        bm1_hio_prm_c& prm = hio_prm(mType);
        mFlySpeedY = prm.m28;
        mFlyAccelY = prm.m2C;
        speedF = 0.0f;
        m858 = prm.m30;
        speed.y = 0.0f;
        gravity = 0.0f;
        m889 = 1;
        m8F4 = 1;
        break;
    }
    case 0xD: {
        gabi::Local<cXyz> local_20;
        gabi::Local<cXyz> rel;
        local_20->set(0.0f, 1000.0f, 100.0f);
        mLookBackState = 2;
        eInit_calcRelativPos(rel, local_20, nullptr);
        m82C.copy(*rel);
        break;
    }
    case 0x10:
        m872 = (s16)((g_Counter_mCounter0() & 3) + 1);
        break;
    case 0x11: {
        bm1_hio_prm_c& prm = hio_prm(mType);
        mTargetFlySpeed = prm.m3C;
        mTargetFlyStep = prm.m40;
        m858 = prm.m44;
        break;
    }
    case 0x12:
        speedF = 0.0f;
        m870 = 0x3C;
        break;
    }
    setAnm();
}
VERIFY(0x02202838, &daNpc_Bm1_c::setStt);

/* 02202B48 */
BOOL daNpc_Bm1_c::d_wait() {
    WWHD_FUNC(0x02202B48, BOOL, this);
    if (m881 && m882) {
        setStt(2);
        /* fopAcM_SetStatusMap(this, 7) (HD: map value 0x27) */
        actor_status = (actor_status & ~0x3Fu) | 0x27;
    }
    return TRUE;
}
VERIFY(0x02202B48, &daNpc_Bm1_c::d_wait);

/* 02202BA8 */
BOOL daNpc_Bm1_c::lookup() {
    WWHD_FUNC(0x02202BA8, BOOL, this);
    if (mbMorfAnimStopped) {
        setStt(3);
    }
    return TRUE;
}
VERIFY(0x02202BA8, &daNpc_Bm1_c::lookup);

/* 02202BDC */
BOOL daNpc_Bm1_c::orooro() {
    WWHD_FUNC(0x02202BDC, BOOL, this);
    if (dComIfGs_isEventBit(0x0001)) {
        fopAcM_delete(this);
    }
    return TRUE;
}
VERIFY(0x02202BDC, &daNpc_Bm1_c::orooro);

/* 02202C2C */
BOOL daNpc_Bm1_c::wait_1() {
    WWHD_FUNC(0x02202C2C, BOOL, this);
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk()) {
            setStt(5);
            m896 = false;
        }
        return TRUE;
    }
    m8FD = 2;
    mLookBackState = 0;
    m896 = true;
    if (mbAttention != 0) {
        m86E = 0x28;
    }
    if (cLib_calcTimer(&m86E) != 0) {
        mLookBackState = 1;
    }
    return TRUE;
}
VERIFY(0x02202C2C, &daNpc_Bm1_c::wait_1);

/* 02202CF0 */
BOOL daNpc_Bm1_c::talk_1() {
    WWHD_FUNC(0x02202CF0, BOOL, this);
    u8 temp_r31 = chk_partsNotMove();
    /* HD: the message status comes from the message manager (GameCube: mpCurrMsg->mStatus);
     * its pointer is read once */
    u32 mng = gabi::load<u32>(0x101F4B5C);
    if (mbHasMsg) {
        mOldMsgStat = (u16)dMsgMng_getStatus(mng);
    } else {
        mOldMsgStat = 0;
    }
    talk(1);
    if (mbHasMsg) {
        if (dMsgMng_getStatus(mng) == 0x13 /* fopMsgStts_MSG_DESTROYED_e */) {
            switch (mCurrMsgNo) {
            case 0xC92:
                dComIfGs_onEventBit(0x1401);
                break;
            case 0x18A2:
                dComIfGs_onEventBit(0x2202);
                break;
            case 0x1997:
                dComIfGs_onEventBit(0x2120);
                break;
            case 0x19A2:
                dComIfGs_onEventBit(0x2001);
                break;
            case 0x19A4:
                m8FD = 5;
                break;
            case 0x1EF6:
                m8FD = 3;
                break;
            case 0x1EF8:
                dComIfGs_onEventBit(0x1908);
                break;
            case 0x1EF7:
                m87C = 0;
                break;
            }
            m87D = 0xFF;
            m895 = false;
            switch (mSpecificType) {
            case SPECIFIC_TYPE_Akoot_e:
            case SPECIFIC_TYPE_Skett_e:
                setStt(0xB);
                break;
            case SPECIFIC_TYPE_Ilari_0xA_e:
                setStt(0xA);
                break;
            default:
                setStt(m8FF);
                m86E = 0x28;
                endEvent();
            }
        }
    }
    return temp_r31;
}
VERIFY(0x02202CF0, &daNpc_Bm1_c::talk_1);

/* 02202FF4 */
BOOL daNpc_Bm1_c::talk_2() {
    WWHD_FUNC(0x02202FF4, BOOL, this);
    if (mPartnerProcID == 0xFFFFFFFF) {
        return TRUE;
    }
    daNpc_Bm1_c* actor = static_cast<daNpc_Bm1_c*>(searchByID(mPartnerProcID));
    if (actor) {
        bool notManzai = actor->mStatus != 9;
        actor->mbManzai = 0; /* clr_manzai() */
        if (notManzai) {
            actor->actor_status &= ~0x4000u; /* fopAcM_OffStatus(actor, fopAcStts_UNK4000_e) */
            setStt(m8FF);
            endEvent();
            mbManzai = 0; /* clr_manzai() */
        }
    }
    return TRUE;
}
VERIFY(0x02202FF4, &daNpc_Bm1_c::talk_2);

/* 02203084 */
BOOL daNpc_Bm1_c::manzai() {
    WWHD_FUNC(0x02203084, BOOL, this);
    if (mPartnerProcID == 0xFFFFFFFF) {
        return TRUE;
    }
    if (mbManzai != 0) {
        daNpc_Bm1_c* partner = (daNpc_Bm1_c*)searchByID(mPartnerProcID);
        if (partner == nullptr) /* HD: JUT_ASSERT(0x1111, partner != NULL) */
            JUT_ASSERT_fail(STR(0x100179E8), 0x1111, STR(0x100179F8));
        /* HD: anmAtr is virtual (vtable slot +0x24) */
        u16 stat = partner->mOldMsgStat;
        gabi::call_ptr(gabi::load<u32>(__vtbl + 0x24), this, stat);
    } else {
        setStt(m8FF);
    }
    return TRUE;
}
VERIFY(0x02203084, &daNpc_Bm1_c::manzai);

/* 02203124 */
BOOL daNpc_Bm1_c::wait_4() {
    WWHD_FUNC(0x02203124, BOOL, this);
    cLib_addCalcAngleS(&current.angle.y, m818.y, 4, 0x800, 0x80);
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk()) {
            setStt(5);
            m896 = false;
        }
        return TRUE;
    } else {
        m8FD = 2;
        mLookBackState = 0;
        m896 = true;
        if (mbAttention) {
            m86E = 0x28;
        }
    }
    if (cLib_calcTimer(&m86E)) {
        mLookBackState = 1;
    }
    return TRUE;
}
VERIFY(0x02203124, &daNpc_Bm1_c::wait_4);

/* 02203200 */
BOOL daNpc_Bm1_c::flyawy() {
    WWHD_FUNC(0x02203200, BOOL, this);
    if (cLib_calcTimer(&m870) == 0) {
        endEvent();
        fopAcM_delete(this);
    } else {
        m88A = 0;
    }
    return TRUE;
}
VERIFY(0x02203200, &daNpc_Bm1_c::flyawy);

/* 0220325C */
BOOL daNpc_Bm1_c::wait_5() {
    WWHD_FUNC(0x0220325C, BOOL, this);
    if (m8FD == 0) {
        gabi::Local<cXyz> diff;
        cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, diff, &current.pos);
        gabi::Local<cXyz> xz; /* absXZ(): cXyz(x, 0, z).abs() */
        xz->x = diff->x;
        xz->y = 0.0f;
        xz->z = diff->z;
        f32 fVar2 = std_sqrtf(PSVECSquareMag(xz));
        f32 fVar1 = dComIfGp_getLinkPlayer()->current.pos.y - current.pos.y;
        if (fVar2 < hio_prm(mType).m48 && -40.0f < fVar1 && fVar1 < 300.0f) {
            m8FD = 4;
        }
    }
    return TRUE;
}
VERIFY(0x0220325C, &daNpc_Bm1_c::wait_5);

/* 02203348 */
BOOL daNpc_Bm1_c::h_wait() {
    WWHD_FUNC(0x02203348, BOOL, this);
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk()) {
            setStt(5);
        }
        return TRUE;
    }
    m8FD = 2;
    if (mbAttention != 0) {
        m86E = 0x28;
    }
    if (cLib_calcTimer(&m86E)) {
        mLookBackState = 1;
        m896 = 0;
    } else {
        mLookBackState = 3;
        m876 = m818.y;
        m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    }
    return TRUE;
}
VERIFY(0x02203348, &daNpc_Bm1_c::h_wait);

/* 02203414 */
BOOL daNpc_Bm1_c::wait_7() {
    WWHD_FUNC(0x02203414, BOOL, this);
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk() && chk_manzai()) {
            setStt(5);
        }
        return TRUE;
    } else if (mbManzai) {
        setStt(9);
        return TRUE;
    } else {
        m8FD = 2;
        if (mbAttention != 0) {
            m86E = 0x28;
        }
        if (cLib_calcTimer(&m86E)) {
            mLookBackState = 1;
        } else {
            mLookBackState = 3;
            m876 = m818.y;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
    }
    return TRUE;
}
VERIFY(0x02203414, &daNpc_Bm1_c::wait_7);

/* 02203508 */
BOOL daNpc_Bm1_c::wait_3() {
    WWHD_FUNC(0x02203508, BOOL, this);
    cLib_addCalcAngleS(&current.angle.y, m818.y, 4, 0x800, 0x80);
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (m895) {
        if (chk_talk()) {
            setStt(5);
            m896 = false;
        }
        return TRUE;
    } else {
        m8FD = 2;
        mLookBackState = 0;
        m896 = true;
        if (mbAttention) {
            m86E = 0x28;
        }
    }
    if (cLib_calcTimer(&m86E)) {
        mLookBackState = 1;
        m870 = (s16)gabi::ftoi(cM_rndF(180.0f) + 180.0f);
        return TRUE;
    }
    if (!cLib_calcTimer(&m870)) {
        setStt(0x10);
    }
    return TRUE;
}
VERIFY(0x02203508, &daNpc_Bm1_c::wait_3);

/* 02203648 */
BOOL daNpc_Bm1_c::wait_8() {
    WWHD_FUNC(0x02203648, BOOL, this);
    m8FD = 2;
    if (mbMorfAnimStopped) {
        if (!cLib_calcTimer(&m872)) {
            setStt(8);
        }
        if (m895 && chk_talk()) {
            setStt(8);
        }
    }
    return TRUE;
}
VERIFY(0x02203648, &daNpc_Bm1_c::wait_8);

/* 022036CC */
BOOL daNpc_Bm1_c::wait_2() {
    WWHD_FUNC(0x022036CC, BOOL, this);
    if (m8FD == 1 || m8FD >= 3) {
        return TRUE;
    }
    if (dComIfGs_isEventBit(0x0A02 /* ENDLESS_NIGHT */)) {
        m8FD = 6;
    }
    return TRUE;
}
VERIFY(0x022036CC, &daNpc_Bm1_c::wait_2);

/* 02203734 */
BOOL daNpc_Bm1_c::walk_1() {
    WWHD_FUNC(0x02203734, BOOL, this);
    if (m895) {
        setAnm_NUM(4, 1);
        speedF = 0.0f;
        if (chk_talk()) {
            setStt(5);
            m896 = false;
        }
        return TRUE;
    }
    gabi::Local<cXyz> pt;
    dNpc_PathRun_getPoint(&mPathRun, pt, mPathRun.mIdx);
    mTargetPos.copy(*pt);
    mLookBackState = 0;
    m896 = true;
    m8FD = 2;
    if (m88B) {
        if (m87E) {
            setStt(0x12);
        }
        m88B = 0;
    }
    m87E = dNpc_PathRun_pointArg(&mPathRun, mPathRun.mIdx);
    return TRUE;
}
VERIFY(0x02203734, &daNpc_Bm1_c::walk_1);

/* 02203830 */
BOOL daNpc_Bm1_c::CHKwai() {
    WWHD_FUNC(0x02203830, BOOL, this);
    if (m895) {
        if (chk_talk()) {
            setStt(5);
            mLookBackState = 1;
            m896 = false;
            m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
        }
        return TRUE;
    }
    m8FD = 2;
    if (!cLib_calcTimer(&m870)) {
        setStt(0x11);
        return TRUE;
    }
    mLookBackState = 2;
    gabi::Local<cXyz> pt;
    dNpc_PathRun_getPoint(&mPathRun, pt, mPathRun.mIdx);
    m82C.copy(*pt);
    m82C.y = eyePos.y;
    m896 = false;
    m_jnt.mbTrn = 1; /* m_jnt.setTrn() */
    return TRUE;
}
VERIFY(0x02203830, &daNpc_Bm1_c::CHKwai);

/* 0220390C */
BOOL daNpc_Bm1_c::demo_action1(void*) {
    WWHD_FUNC(0x0220390C, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(1);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 1:
            mbSetEyePos = d_wait();
            break;
        case 2:
            mbSetEyePos = lookup();
            break;
        case 3:
            mbSetEyePos = orooro();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x0220390C, &daNpc_Bm1_c::demo_action1);

/* 022039E0 */
BOOL daNpc_Bm1_c::wait_action1(void*) {
    WWHD_FUNC(0x022039E0, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(4);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 4:
            mbSetEyePos = wait_1();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x022039E0, &daNpc_Bm1_c::wait_action1);

/* 02203A9C */
BOOL daNpc_Bm1_c::wait_action2(void*) {
    WWHD_FUNC(0x02203A9C, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(0xC);
        if (mSpecificType == SPECIFIC_TYPE_Kogoli_e && dComIfGs_isEventBit(0x1820) && !dKy_daynight_check()) {
            current.angle.y -= 0x6000;
            m818.y = current.angle.y;
        }
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0xC:
            mbSetEyePos = wait_4();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        case 0xA:
            mbSetEyePos = flyawy();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x02203A9C, &daNpc_Bm1_c::wait_action2);

/* 02203BAC */
BOOL daNpc_Bm1_c::wait_action3(void*) {
    WWHD_FUNC(0x02203BAC, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(0xD);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0xD:
            mbSetEyePos = wait_5();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x02203BAC, &daNpc_Bm1_c::wait_action3);

/* 02203C44 */
BOOL daNpc_Bm1_c::wait_action4(void*) {
    WWHD_FUNC(0x02203C44, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setAnm_NUM(4, 1);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x02203C44, &daNpc_Bm1_c::wait_action4);

/* 02203C98 */
BOOL daNpc_Bm1_c::wait_action5(void*) {
    WWHD_FUNC(0x02203C98, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(0xE);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0xE:
            mbSetEyePos = wait_4();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x02203C98, &daNpc_Bm1_c::wait_action5);

/* 02203D48 */
BOOL daNpc_Bm1_c::wait_action6(void*) {
    WWHD_FUNC(0x02203D48, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(7);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 7:
            mbSetEyePos = h_wait();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x02203D48, &daNpc_Bm1_c::wait_action6);

/* 02203DF8 */
BOOL daNpc_Bm1_c::wait_action7(void*) {
    WWHD_FUNC(0x02203DF8, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(0xF);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0xF:
            mbSetEyePos = wait_7();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        case 0xB:
            mbSetEyePos = talk_2();
            break;
        case 9:
            mbSetEyePos = manzai();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x02203DF8, &daNpc_Bm1_c::wait_action7);

/* 02203ED8 */
BOOL daNpc_Bm1_c::wait_action8(void*) {
    WWHD_FUNC(0x02203ED8, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(8);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 8:
            mbSetEyePos = wait_3();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        case 0x10:
            mbSetEyePos = wait_8();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x02203ED8, &daNpc_Bm1_c::wait_action8);

/* 02203FAC */
BOOL daNpc_Bm1_c::wait_action9(void*) {
    WWHD_FUNC(0x02203FAC, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(6);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 6:
            mbSetEyePos = wait_2();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x02203FAC, &daNpc_Bm1_c::wait_action9);

/* 02204044 */
BOOL daNpc_Bm1_c::wait_actionA(void*) {
    WWHD_FUNC(0x02204044, BOOL, this, (void*)nullptr);
    switch (m904) {
    case 0:
        setStt(0x11);
        m904 = m904 + 1;
        break;
    case 1:
    case 2:
    case 3:
        mbAttention = chkAttention();
        switch (mStatus) {
        case 0x11:
            mbSetEyePos = walk_1();
            break;
        case 0x12:
            mbSetEyePos = CHKwai();
            break;
        case 5:
            mbSetEyePos = talk_1();
            break;
        }
        lookBack();
        break;
    case 9:
        break;
    }
    return TRUE;
}
VERIFY(0x02204044, &daNpc_Bm1_c::wait_actionA);

/* 0220026C */
void daNpc_Bm1_c::event_proc(int i_staff_idx) {
    WWHD_FUNC(0x0220026C, void, this, i_staff_idx);
    if (dComIfGp_evmng_endCheck(mEventIdTable[mEventIdx])) {
        switch ((u32)(s32)mEventIdx) {
        case 0:
            m8FD = 1;
            m87C = 1;
            break;
        case 1:
            dComIfGs_onEventBit(0x1F40);
            fopAcM_delete(this);
            break;
        case 2:
            dLetter_send(0xAE03);
            dComIfGs_onEventBit(0x2180);
            break;
        case 3:
            dComIfGs_onEventBit(0x1E80);
            fopAcM_delete(this);
            break;
        }
        endEvent();
    } else {
        if (!mEventCut.cutProc()) {
            privateCut(i_staff_idx);
        }
        lookBack();
    }
}
VERIFY(0x0220026C, &daNpc_Bm1_c::event_proc);

/* 02200420 */
void daNpc_Bm1_c::bm_clcFlySpd() {
    WWHD_FUNC(0x02200420, void, this);
    if (m8F4 == 2 || m8F4 == 5) {
        s16 target = cLib_targetAngleY(&current.pos, &mTargetPos);
        bm1_hio_prm_c& prm = hio_prm(mType);
        cLib_addCalcAngleS(&current.angle.y, target, prm.mFlyScale, prm.mFlyMaxStep, 0);
    }
    switch ((u32)(s32)m8F4) {
    case 3:
        speed.y = speed.y - mFlyAccelY;
        break;
    case 2:
    case 5:
        if (m889 != 0) {
            if (mTargetPos.y > current.pos.y) {
                f32 v = speed.y + mFlyAccelY; /* cLib_maxLimit(v, mFlySpeedY) */
                f32 max = mFlySpeedY;
                speed.y = fsel(v - max, max, v);
            } else {
                f32 v = speed.y - mFlyAccelY; /* cLib_minLimit(v, 0.0f) */
                speed.y = fsel(v, v, 0.0f);
            }
        } else if (mTargetPos.y > current.pos.y) {
            f32 v = speed.y + mFlyAccelY; /* cLib_maxLimit(v, 0.0f) */
            speed.y = fsel(v, 0.0f, v);
        } else {
            speed.y = speed.y - mFlyAccelY;
        }
        break;
    }
    cLib_chaseF(&speedF, mTargetFlySpeed, mTargetFlyStep);
}
VERIFY(0x02200420, &daNpc_Bm1_c::bm_clcFlySpd);

/* 0220057C */
u32 daNpc_Bm1_c::bm_movPass(u32 i_param_1) {
    WWHD_FUNC(0x0220057C, u32, this, i_param_1);
    u8 point_idx = 0;
    u32 o_retval = 0;
    if (mPathRun.mPath.get() != nullptr && (point_idx = mPathRun.mIdx, dPath_ChkClose(mPathRun.mPath))) {
        gabi::Local<cXyz> pos; /* passed by value */
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        if (!dNpc_PathRun_chkPointPass(&mPathRun, pos, mPathRun.mbDir != 0)) {
            return o_retval;
        }
        dNpc_PathRun_nextIdxAuto(&mPathRun);
        o_retval = 1;
        if (i_param_1 == 0) {
            return o_retval;
        }
    } else {
        gabi::Local<cXyz> diff;
        cXyz_mi(&mTargetPos, diff, &current.pos);
        gabi::Local<cXyz> xz; /* absXZ(): cXyz(x, 0, z).abs() */
        xz->x = diff->x;
        xz->y = 0.0f;
        xz->z = diff->z;
        f32 dist_xz = std_sqrtf(PSVECSquareMag(xz));
        if (!(dist_xz > m858)) { /* dist_xz <= m858 (GHS: NaN passes) */
            o_retval = 1;
            /* HD: without a path the pass flag is not updated (GameCube: also with i_param_1) */
            if (mPathRun.mPath.get() == nullptr) {
                return o_retval;
            }
            if ((s32)point_idx < dNpc_PathRun_maxPoint(&mPathRun) - 2) {
                dNpc_PathRun_nextIdxAuto(&mPathRun);
            } else {
                o_retval = 2;
            }
            if (i_param_1 == 0) {
                return o_retval;
            }
        } else {
            return o_retval;
        }
    }
    gabi::Local<cXyz> pt;
    dNpc_PathRun_getPoint(&mPathRun, pt, mPathRun.mIdx);
    f32 idxY = pt->y;
    dNpc_PathRun_getPoint(&mPathRun, pt, point_idx);
    m889 = !(pt->y > idxY); /* cmpCoords.y <= idxCoords.y (GHS: NaN gives true) */
    return o_retval;
}
VERIFY(0x0220057C, &daNpc_Bm1_c::bm_movPass);

/* 02200714 */
bool daNpc_Bm1_c::bm_flyMove() {
    WWHD_FUNC(0x02200714, bool, this);
    if (m8F4 == 0) {
        return false;
    }
    if (m88A != 0) {
        m887 = 1;
        return true;
    }
    if (mPathRun.mPath.get() != nullptr) {
        gabi::Local<cXyz> pt;
        dNpc_PathRun_getPoint(&mPathRun, pt, mPathRun.mIdx);
        mTargetPos.copy(*pt);
    }
    switch ((u32)(s32)m8F4) {
    case 1:
        if (mAnmNum != 0xA) {
            setAnm_NUM(0xA, 1);
        } else if (mbMorfAnimStopped) {
            m8F4 = 2;
        }
        break;
    case 2:
        if (mAnmNum != 0xB) {
            setAnm_NUM(0xB, 1);
            speed.y = mTargetFlySpeed * 0.7f;
        } else if (mbMorfAnimStopped) {
            m8F4 = 5;
            m88A = 1;
            bm_setFlyAnm();
        }
        goto case_5;
    case 3:
        setAnm_NUM(9, 1);
        if (!mObjAcch.ChkGroundHit()) {
            bm_clcFlySpd();
        } else {
            speedF = 0.0f;
            speed.y = 0.0f;
            m8F4 = 4;
            m88A = 1;
        }
        break;
    case 4:
        if (mAnmNum != 0xD) {
            setAnm_NUM(0xD, 1);
        } else if (mbMorfAnimStopped) {
            gravity = -4.5f;
            setAnm_NUM(4, 1);
            m8F4 = 0;
            m88A = 1;
        }
        break;
    case 5:
    case_5: {
        bm_clcFlySpd();
        u32 ret = bm_movPass(1);
        switch (ret) {
        case 1:
            if (m8F4 != 2) {
                bm_setFlyAnm();
                m8F4 = 5;
            }
            m88A = 1;
            break;
        case 2:
            mTargetFlySpeed = 0.0f;
            m88A = 1;
            m8F4 = 3;
            break;
        }
        break;
    }
    }
    return true;
}
VERIFY(0x02200714, &daNpc_Bm1_c::bm_flyMove);

/* 02200AF8 */
void daNpc_Bm1_c::bm_nMove() {
    WWHD_FUNC(0x02200AF8, void, this);
    if (m88B) {
        m887 = 1;
        return;
    }
    switch ((u32)(s32)mAnmNum) {
    case 0x14:
    case 0xE: {
        bm_clcMovSpd();
        f32 spd = speedF * hio_prm(mType).m38;
        if (spd < 0.5f) {
            spd = 0.5f;
        }
        setPlaySpd(spd);
        u32 ret = bm_movPass(0);
        if (ret >= 1 && ret <= 2) {
            m88B = 1;
        }
        /* HD: a ground check below the next position; above 50 units over the ground the
         * HD-only flag 0xA08 is set */
        gabi::Local<GndChk_bm1> gnd;
        dBgS_GndChk_ct(gnd, kGndChkVt, false);
        gabi::Local<cXyz> pos;
        pos->x = old.pos.x + speed.x;
        pos->y = old.pos.y + 50.0f;
        pos->z = old.pos.z + speed.z;
        dBgS_GndChk_SetPos(gnd, pos);
        f32 gndY = cBgS_GroundCross(dComIfG_Bgsp(), gnd);
        if (gndY + 50.0f < current.pos.y) {
            mHD_A08 = 1;
        }
        /* ~dBgS_GndChk */
        u32 b = gabi::ea(gnd.get());
        gabi::store<u32>(b + 0x20, 0x10016FB8);
        gabi::store<u32>(b + 0x40, 0x10016FD8);
        gabi::store<u32>(b + 0x4C, 0x10016F98);
        gabi::call(0x02008DAC, gnd.get(), 0); /* cBgS_GndChk::~cBgS_GndChk */
        break;
    }
    }
}
VERIFY(0x02200AF8, &daNpc_Bm1_c::bm_nMove);

/* 02200D58 */
BOOL daNpc_Bm1_c::_execute() {
    WWHD_FUNC(0x02200D58, BOOL, this);
    mHD_A08 = 0; /* HD */
    if (!mbRanExecute) {
        mInitialPos.copy(current.pos);
        mbRanExecute = true;
        m818.x = current.angle.x;
        m818.y = current.angle.y;
        m818.z = current.angle.z;
    }
    bm1_hio_prm_c& prm = hio_prm(mType);
    m_jnt.setParam(prm.mMaxBackboneX, prm.mMaxBackboneY, prm.mMinBackboneX, prm.mMinBackboneY, prm.mMaxHeadX,
                   prm.mMaxHeadY, prm.mMinHeadX, prm.mMinHeadY, prm.mMaxTurnStep);
    if (!m881 && mbInitPostman0 && demoActorID == 0) {
        return TRUE;
    }
    m887 = 0;
    mbInitPostman0 = false;
    partner_srch();
    checkOrder();
    if (!demo()) {
        s32 iVar1 = -1;
        if (dComIfGp_event_runCheck() && gabi::load<u16>(gabi::ea(this) + 0xF8) != 1 /* !eventInfo.checkCommandTalk() */) {
            iVar1 = isEventEntry();
        }
        if (iVar1 >= 0) {
            event_proc(iVar1);
        } else {
            pmf_call(this, &mCurrActionFunc, nullptr); /* (this->*mCurrActionFunc)(NULL) */
        }
        if (!bm_flyMove()) {
            bm_nMove();
        }
        if (!m887) {
            fopAcM_posMoveF(this, gabi::at<cXyz>(gabi::ea(&mStts))); /* mStts.GetCCMoveP() */
        }
        if (!mbSetShapeAngle) {
            shape_angle.x = current.angle.x;
            shape_angle.y = current.angle.y;
            shape_angle.z = current.angle.z;
        }
    }
    eventOrder();
    setMtx(0);
    if (!mbInDemo) {
        setCollision(50.0f, 160.0f);
    }
    return TRUE;
}
VERIFY(0x02200D58, &daNpc_Bm1_c::_execute);

/* 02200F90 */
BOOL daNpc_Bm1_c::_draw() {
    WWHD_FUNC(0x02200F90, BOOL, this);
    J3DModel* model = mpHeadMorf->getModel();
    J3DModelData* model_data = J3DModel_getModelData(model);
    J3DModel* morf_model = mpMorf->getModel();
    if (mbInitPostman0 || m888) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call (GameCube: the bm02.bmt material table for some types) */
    mpMorf->entryDL(); /* HD: always without the material table */
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mHeadBtpAnm, model_data, mBlinkFrame);
    mpHeadMorf->entryDL();
    gabi::store<u32>(gabi::ea(model_data) + 0x38, 0); /* mHeadBtpAnm.remove(model_data) */
    setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
    if (mbHasArms) {
        J3DModel* arm = mpArmMorf->getModel();
        setLightTevColorType(dKy_getEnvlight(), arm, &tevStr);
        mpArmMorf->entryDL(); /* HD: always without the bmarm02.bmt material table */
    } else {
        J3DModel* wing = mpWingMorf->getModel();
        setLightTevColorType(dKy_getEnvlight(), wing, &tevStr);
        mpWingMorf->entryDL();
    }
    if (mpBinderModel.get() != nullptr) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpBinderModel, &tevStr);
        mDoExt_modelEntryDL(mpBinderModel);
    }
    if (mpBagModel.get() != nullptr) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpBagModel, &tevStr);
        mDoExt_modelEntryDL(mpBagModel);
    }
    if (mpKnifeModel.get() != nullptr) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpKnifeModel, &tevStr);
        mDoExt_modelEntryDL(mpKnifeModel);
    }
    if (mpStickModel.get() != nullptr) {
        dScnKy_env_light_c* env = dKy_getEnvlight();
        setLightTevColorType(env, mpStickModel, &tevStr);
        mDoExt_modelEntryDL(mpStickModel);
    }
    /* HD: no shadowDraw() */
    switch ((u32)(s32)mType) {
    case TYPE_Quill_e: dSnap_RegistFig(0x8D /* DSNAP_TYPE_NPC_BM1_PST */, this, 1.0f, 1.0f, 1.0f); break;
    case TYPE_Akoot_e: dSnap_RegistFig(0x8E /* DSNAP_TYPE_NPC_BM1_SKT_KKT */, this, 1.0f, 1.0f, 1.0f); break;
    case TYPE_Skett_e: dSnap_RegistFig(0x8E /* DSNAP_TYPE_NPC_BM1_SKT_KKT */, this, 1.0f, 1.0f, 1.0f); break;
    case TYPE_Basht_e: dSnap_RegistFig(0x93 /* DSNAP_TYPE_NPC_BM1_BMB_0_1 */, this, 1.0f, 1.0f, 1.0f); break;
    case TYPE_Bisht_e: dSnap_RegistFig(0x93 /* DSNAP_TYPE_NPC_BM1_BMB_0_1 */, this, 1.0f, 1.0f, 1.0f); break;
    case TYPE_Hoskit_e: dSnap_RegistFig(0x91 /* DSNAP_TYPE_NPC_BM1_BMB_2 */, this, 1.0f, 1.0f, 1.0f); break;
    case TYPE_Ilari_e: dSnap_RegistFig(0x90 /* DSNAP_TYPE_NPC_BM1_BMC_0_1_2 */, this, 1.0f, 1.0f, 1.0f); break;
    case TYPE_Pashli_e: dSnap_RegistFig(0x97 /* DSNAP_TYPE_NPC_BM1_BMC_3 */, this, 1.0f, 1.0f, 1.0f); break;
    case TYPE_Namali_e: dSnap_RegistFig(0x92 /* DSNAP_TYPE_NPC_BM1_BMD_0 */, this, 1.0f, 1.0f, 1.0f); break;
    case TYPE_Kogoli_e: dSnap_RegistFig(0x8F /* DSNAP_TYPE_NPC_BM1_BMD_1 */, this, 1.0f, 1.0f, 1.0f); break;
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (hio_prm(mType).m18 != 0) {
        static_color_init(0x101FDA50, 0x101FEBF4, 0x10016F70); /* (GXColor){0xFF, 0x00, 0x00, 0x80} */
        static_color_init(0x101FDAC0, 0x101FEBF8, 0x10016F74); /* (GXColor){0x00, 0x00, 0xFF, 0x80} */
        if (mSpecificType == SPECIFIC_TYPE_Quill_2_e) {
            static_color_init(0x101FDA48, 0x101FEBEC, 0x10016F78);
        }
    }
    return TRUE;
}
VERIFY(0x02200F90, &daNpc_Bm1_c::_draw);

/* ---- compiler-generated / static initialisation (no GameCube function bodies) ---- */
/* 028EFFD0 __construct_array(array, n, size, ctor) (GHS runtime) */
static inline void __construct_array(void* p, s32 n, u32 size, u32 ctor) { gabi::call(0x028EFFD0, p, n, size, ctor); }

/* 02204118 daNpc_Bm1_childHIO_c::daNpc_Bm1_childHIO_c (HD: allocates when this == NULL) */
static daNpc_Bm1_childHIO_c* daNpc_Bm1_childHIO_c_ct(daNpc_Bm1_childHIO_c* i_this) {
    WWHD_FUNC(0x02204118, daNpc_Bm1_childHIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Bm1_childHIO_c*)operator_new(0x54);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10017008;
    return i_this;
}
VERIFY(0x02204118, daNpc_Bm1_childHIO_c_ct);

/* 02204158 daNpc_Bm1_HIO_c::daNpc_Bm1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Bm1_HIO_c* daNpc_Bm1_HIO_c_ct(daNpc_Bm1_HIO_c* i_this) {
    WWHD_FUNC(0x02204158, daNpc_Bm1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Bm1_HIO_c*)operator_new(0x354);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10017018;
    __construct_array(i_this->children, 10, 0x54, 0x02204118 /* daNpc_Bm1_childHIO_c_ct */);
    for (int i = 0; i < 10; i++) {
        i_this->children[i].m50 = i;
        /* memcpy(&children[i].hio_prm, &a_prm_tbl[i] (.data 0x101BCC3C), sizeof(hio_prm_c)) */
        memcpy_g(&i_this->children[i].hio_prm, gabi::at<u8>(0x101BCC3C + 0x4C * i), 0x4C); /* 028FEAC0 memcpy */
    }
    i_this->m8 = -1;
    i_this->m4 = -1;
    return i_this;
}
VERIFY(0x02204158, daNpc_Bm1_HIO_c_ct);

/* 02204208: static initialisation of the translation unit */
static void __sinit_d_a_npc_bm1_cpp() {
    WWHD_FUNC(0x02204208, void);
    sinit_header_statics(0x104661D4, 0x101BCF34);
    daNpc_Bm1_HIO_c_ct(&l_HIO()); /* static daNpc_Bm1_HIO_c l_HIO */
}
VERIFY(0x02204208, __sinit_d_a_npc_bm1_cpp);

/* 022042A8: sead::SafeString deleting destructor (this TU's copy; vtable 0x10016F80 slot +0xC) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022042A8, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022042A8, SafeString_dt);

/* 022042BC: daNpc_Bm1_c deleting destructor (compiler-generated, HD virtual destructor): only
 * the fopNpc_npc_c members have destructors */
static void daNpc_Bm1_c_dt(daNpc_Bm1_c* i_this, s32 flags) {
    WWHD_FUNC(0x022042BC, void, i_this, flags);
    if (i_this != nullptr) {
        dCcD_Cyl_dt(&i_this->mCyl, 2);
        dCcD_Stts_dt(&i_this->mStts, 2);
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        /* ~dBgS_ObjAcch: this TU's vtables of its sub-objects, then dBgS_Acch::~dBgS_Acch */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10016FE8);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10016FF8);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022042BC, daNpc_Bm1_c_dt);

/* 02204358: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x02204358, void, (SafeString*)nullptr);
}
VERIFY(0x02204358, SafeString_assureTerminationImpl);

/**
 * d_a_npc_ko1_c.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island), part C: animations, messages, attention, path, particles.
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 */
#define SAFESTRING_VTBL 0x1001C87C /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ko1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* 025163BC dCcD_GObjInf::GetCoHitObj (cCcD_Obj*; its dCcD_Stts* at +0x44, the stts' actor at +0xC) */
static inline u32 dCcD_GetCoHitObj(dCcD_GObjInf* o) { return gabi::call<u32>(0x025163BC, o); }
/* 0259E778 dNpc_PathRun_c::getPoint(u8): cXyz through a hidden result pointer (r4) */
static inline void dNpc_PathRun_getPoint(dNpc_PathRun_l* p, cXyz* o_pnt, u8 idx) { gabi::call(0x0259E778, p, o_pnt, idx); }
/* 0259ED58 dNpc_PathRun_c::nextIdxAuto */
static inline BOOL dNpc_PathRun_nextIdxAuto(dNpc_PathRun_l* p) { return gabi::call<BOOL>(0x0259ED58, p); }
static inline void dPa_rippleEcallBack_end_l(dPa_rippleEcallBack_l* cb) { gabi::call(0x025A9270, cb); }

/* anm_prm_c tables: HD stride 0x14 (the struct has 4 more bytes after mLoopMode) */
static inline daNpc_Ko1_c::anm_prm_c* anm_prm(u32 tbl, s32 i) { return gabi::at<daNpc_Ko1_c::anm_prm_c>(tbl + i * 0x14); }
enum : u32 {
    a_balloon_anm_prm_tbl = 0x101BF7D8,
    a_anm_prm_tbl = 0x101BF800,
    a_stt_anm_prm_tbl = 0x101BF918,
    a_atr_anm_prm_tbl = 0x101BFB70,
};

/* fields typed differently from the header (s8 where the header has u8) */
#define mAnmNum_s8 (*gabi::at<be<s8>>(gabi::ea(this) + 0xA0F))
#define mBlnAnmNum_s8 (*gabi::at<be<s8>>(gabi::ea(this) + 0xA11))
#define mbMorfAnimStopped_s8 (*gabi::at<be<s8>>(gabi::ea(this) + 0x9D0))

/* 02276B2C */
s32 daNpc_Ko1_c::anmNum_toResID(s32 i_anmNum) {
    WWHD_FUNC(0x02276B2C, s32, this, i_anmNum);
    return gabi::load<s32>(0x1001CBC0 + i_anmNum * 4); /* a_bck_resID_tbl */
}
VERIFY(0x02276B2C, &daNpc_Ko1_c::anmNum_toResID);

/* 02276B40 */
s32 daNpc_Ko1_c::headAnmNum_toResID(s32 i_anmNum) {
    WWHD_FUNC(0x02276B40, s32, this, i_anmNum);
    if (mType == 1) {
        return 0x25;
    }
    return gabi::load<s32>(0x1001CBF8 + i_anmNum * 4); /* a_hed_bck_resID_tbl */
}
VERIFY(0x02276B40, &daNpc_Ko1_c::headAnmNum_toResID);

/* 02276B68 */
s32 daNpc_Ko1_c::balloon_anmNum_toResID(s32 i_anmNum) {
    WWHD_FUNC(0x02276B68, s32, this, i_anmNum);
    return gabi::load<s32>(0x1001CC30 + i_anmNum * 4); /* a_bln_bck_resID_tbl */
}
VERIFY(0x02276B68, &daNpc_Ko1_c::balloon_anmNum_toResID);

/* 02276B7C */
u32 daNpc_Ko1_c::setAnm_tex(s8 i_btpNum) {
    WWHD_FUNC(0x02276B7C, u32, this, i_btpNum);
    if (mBtpNum != i_btpNum) {
        mBtpNum = i_btpNum;
        return gabi::call<u32>(0x02273C88, this, 1); /* iniTexPttrnAnm(true) */
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}
VERIFY(0x02276B7C, &daNpc_Ko1_c::setAnm_tex);

/* 02276B9C */
BOOL daNpc_Ko1_c::setAnm_anm(anm_prm_c* i_prm) {
    WWHD_FUNC(0x02276B9C, BOOL, this, i_prm);
    if (mAnmNum_s8 == i_prm->mAnmNum) {
        return TRUE;
    }
    s8 anmNum = i_prm->mAnmNum;
    mAnmNum_s8 = anmNum;
    s32 resID = anmNum_toResID(anmNum);
    dNpc_setAnmIDRes(mpMorf, i_prm->mLoopMode, i_prm->mMorf, i_prm->mSpeed, resID, -1, STR(0x1001CC68));
    resID = headAnmNum_toResID(mAnmNum_s8);
    dNpc_setAnmIDRes(mpMorf824, i_prm->mLoopMode, i_prm->mMorf, i_prm->mSpeed, resID, -1, STR(0x1001CC68));
    mbMorfAnimStopped_s8 = 0;
    m994 = 0.0f;
    m9D1 = 0;
    return TRUE;
}
VERIFY(0x02276B9C, &daNpc_Ko1_c::setAnm_anm);

/* 02276C70 */
BOOL daNpc_Ko1_c::set_balloonAnm_anm(anm_prm_c* i_prm) {
    WWHD_FUNC(0x02276C70, BOOL, this, i_prm);
    if (mBlnAnmNum_s8 == i_prm->mAnmNum) {
        return TRUE;
    }
    s8 anmNum = i_prm->mAnmNum;
    mBlnAnmNum_s8 = anmNum;
    s32 resID = balloon_anmNum_toResID(anmNum);
    dNpc_setAnmIDRes(mpMorf81C, i_prm->mLoopMode, i_prm->mMorf, i_prm->mSpeed, resID, -1, STR(0x1001CC6B));
    m9CF = 0;
    m998 = 0.0f;
    m9CE = 0;
    return TRUE;
}
VERIFY(0x02276C70, &daNpc_Ko1_c::set_balloonAnm_anm);

/* 02276D04 */
/* tail call: set_balloonAnm_anm's result is passed through */
BOOL daNpc_Ko1_c::set_balloonAnm_NUM(s32 i_idx) {
    WWHD_FUNC(0x02276D04, BOOL, this, i_idx);
    return set_balloonAnm_anm(anm_prm(a_balloon_anm_prm_tbl, i_idx));
}
VERIFY(0x02276D04, &daNpc_Ko1_c::set_balloonAnm_NUM);

/* 02276D18 */
void daNpc_Ko1_c::setAnm_NUM(s32 i_idx, s32 i_setTex) {
    WWHD_FUNC(0x02276D18, void, this, i_idx, i_setTex);
    if (i_setTex != 0) {
        setAnm_tex(anm_prm(a_anm_prm_tbl, i_idx)->mBtpNum);
    }
    setAnm_anm(anm_prm(a_anm_prm_tbl, i_idx));
}
VERIFY(0x02276D18, &daNpc_Ko1_c::setAnm_NUM);

/* 02276D84 */
bool daNpc_Ko1_c::setAnm() {
    WWHD_FUNC(0x02276D84, bool, this);
    s8 btpNum = anm_prm(a_stt_anm_prm_tbl, mA13)->mBtpNum;
    if (btpNum >= 0) {
        setAnm_tex(btpNum);
    }
    if (anm_prm(a_stt_anm_prm_tbl, mA13)->mAnmNum >= 0) {
        setAnm_anm(anm_prm(a_stt_anm_prm_tbl, mA13));
    }
    return true;
}
VERIFY(0x02276D84, &daNpc_Ko1_c::setAnm);

/* 02276E08 */
void daNpc_Ko1_c::chg_anmTag() {
    WWHD_FUNC(0x02276E08, void, this);
    if (mA0D == 10) {
        setAnm_NUM(4, 1);
    }
}
VERIFY(0x02276E08, &daNpc_Ko1_c::chg_anmTag);

/* 02276E20 */
void daNpc_Ko1_c::setAnm_ATR(s32 i_setTex) {
    WWHD_FUNC(0x02276E20, void, this, i_setTex);
    if (i_setTex != 0) {
        setAnm_tex(anm_prm(a_atr_anm_prm_tbl, mA0C)->mBtpNum);
    }
    setAnm_anm(anm_prm(a_atr_anm_prm_tbl, mA0C));
}
VERIFY(0x02276E20, &daNpc_Ko1_c::setAnm_ATR);

/* 02276E90 */
void daNpc_Ko1_c::control_anmTag() {
    WWHD_FUNC(0x02276E90, void, this);
    if (mA0D == 10 && mbMorfAnimStopped_s8 != 0) {
        mA0D = 0xFF;
        setAnm_ATR(1);
    }
}
VERIFY(0x02276E90, &daNpc_Ko1_c::control_anmTag);

/* 02276EB8 */
void daNpc_Ko1_c::chg_anmAtr(u8 i_atr) {
    WWHD_FUNC(0x02276EB8, void, this, i_atr);
    u32 msgNo = m9B8;
    if (msgNo != 0xFFFFFFFF && (msgNo == 0xB03 || msgNo == 0xB0A || (msgNo >= 0xB64 && msgNo <= 0xB65))) {
        m9E6 = 0;
        mA15 = 1;
        m_jnt.mbTrn = 1;
    }
    if (i_atr >= 0xD || i_atr == mA0C) {
        return;
    }
    mA0C = i_atr;
    if (i_atr == 0xB) {
        m9E6 = 0;
        mA15 = 1;
        m_jnt.mbTrn = 1;
    }
    setAnm_ATR(1);
}
VERIFY(0x02276EB8, &daNpc_Ko1_c::chg_anmAtr);

/* 02276F30 */
void daNpc_Ko1_c::control_anmAtr() {
    WWHD_FUNC(0x02276F30, void, this);
    if (mA0C == 0xB && mbMorfAnimStopped_s8 != 0) {
        mA0C = 0;
        setAnm_NUM(0, 1);
    }
}
VERIFY(0x02276F30, &daNpc_Ko1_c::control_anmAtr);

/* 02276F58 */
void daNpc_Ko1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x02276F58, void, this, i_msgStatus);
    u32 play = dComIfGp_ea();
    /* in a two-person talk (field_0x7d8 == 2) only the current speaker follows the message tags:
     * speaker list at play + 0x5C34 (index) / + 0x5C38 */
    bool skip = false;
    if (gabi::load<u8>(gabi::ea(this) + 0x7D8) == 2) { /* fopNpc_npc_c field_0x7d8 */
        u32 idx = gabi::load<u32>(play + 0x5C34);
        if (gabi::ea(this) != gabi::load<u32>(play + 0x5C34 + idx * 4)) {
            skip = true;
        }
    }
    if (!skip) {
        switch (i_msgStatus) {
        case 6: {
            if (mA19 == 0) {
                mA0C = 0xFF;
                chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
                mA19 = mA19 + 1;
            }
            u8 tag = dComIfGp_getMesgAnimeTagInfo();
            dComIfGp_clearMesgAnimeTagInfo();
            if (tag != 0xFF && mA0D != tag) {
                mA0D = tag;
                chg_anmTag();
            }
            break;
        }
        case 0xE:
            mA19 = 0;
            break;
        }
    }
    control_anmTag();
    control_anmAtr();
}
VERIFY(0x02276F58, &daNpc_Ko1_c::anmAtr);

/* 02277070 */
bool daNpc_Ko1_c::chk_talk() {
    WWHD_FUNC(0x02277070, bool, this);
    bool ret = false;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            m9D3 = dComIfGp_event_getPreItemNo();
            ret = true;
        }
    } else {
        m9D3 = 0xFF;
        ret = true;
    }
    return ret;
}
VERIFY(0x02277070, &daNpc_Ko1_c::chk_talk);

/* 022770F0: the matcher calls it cLib_calcTimer<s> (d_a_npc_ac1); it is searchByID */
fopAc_ac_c* daNpc_Ko1_c::searchByID(fpc_ProcID i_procID) {
    WWHD_FUNC(0x022770F0, fopAc_ac_c*, this, i_procID);
    gabi::Local<gptr<fopAc_ac_c>> o_actor;
    *o_actor = nullptr;
    gabi::call(0x025D54C4, i_procID, o_actor.get()); /* fopAcM_SearchByID(id, &actor) */
    return *o_actor;
}
VERIFY(0x022770F0, &daNpc_Ko1_c::searchByID);

/* talk partners of the two-person talk (play + 0x5C38..0x5C5C) */
static inline void setTalkPartners(u32 a0, u32 a1, u32 a2) {
    u32 play = dComIfGp_ea();
    gabi::store<u32>(play + 0x5C38, a0);
    gabi::store<u32>(play + 0x5C3C, a1);
    gabi::store<u32>(play + 0x5C40, a2);
    gabi::store<u32>(play + 0x5C44, 0);
    gabi::store<u32>(play + 0x5C48, 0);
    gabi::store<u32>(play + 0x5C4C, 0);
    gabi::store<u32>(play + 0x5C50, 0);
    gabi::store<u32>(play + 0x5C54, 0);
    gabi::store<u32>(play + 0x5C58, 0);
    gabi::store<u32>(play + 0x5C5C, 0);
}

/* 02277124 */
bool daNpc_Ko1_c::chk_manzai_1() {
    WWHD_FUNC(0x02277124, bool, this);
    /* partner process ids at 0x924 (m92C of them) */
    s32 ready = 0;
    u32 n;
    for (s32 i = 0; i < (s32)(n = m92C); i++) {
        u32 a = gabi::ea(searchByID(gabi::load<u32>(gabi::ea(this) + 0x924 + i * 4)));
        if (a == 0) /* JUT_ASSERT(1414, a_actor_p != NULL) */
            JUT_ASSERT_fail(STR(0x1001CC70), 0x586, STR(0x1001CC80));
        if (gabi::load<u8>(a + 0x7D8) == 2) {
            ready++;
        } else {
            gabi::store<u32>(a + 0x2E0, gabi::load<u32>(a + 0x2E0) | 0x4000);
            u32 myId = gabi::load<u32>(gabi::ea(this) + 4);
            gabi::store<u8>(a + 0x7D8, 1);
            gabi::store<u32>(a + 0x7D0, myId);
        }
    }
    bool ret = (u32)ready == n;
    if (ret) {
        u32 p0 = gabi::ea(searchByID(m924));
        u32 self = gabi::ea(this);
        switch (mSpecificType) {
        case 1:
            setTalkPartners(p0, self, 0);
            break;
        case 3: {
            u32 p1 = gabi::ea(searchByID(m928));
            setTalkPartners(p0, p1, self);
            break;
        }
        case 6:
            setTalkPartners(self, p0, 0);
            break;
        case 7: {
            u32 p1 = gabi::ea(searchByID(m928));
            setTalkPartners(self, p1, p0);
            break;
        }
        default:
            break;
        }
        gabi::store<u8>(gabi::ea(this) + 0x7D8, 2); /* fopNpc_npc_c field_0x7d8 */
    }
    return ret;
}
VERIFY(0x02277124, &daNpc_Ko1_c::chk_manzai_1);

/* 0227732C */
u8 daNpc_Ko1_c::chk_partsNotMove() {
    WWHD_FUNC(0x0227732C, u8, this);
    return m9B0 == m_jnt.mAngles[0][1] && m9B2 == m_jnt.mAngles[1][1] && m9B4 == current.angle.y;
}
VERIFY(0x0227732C, &daNpc_Ko1_c::chk_partsNotMove);

/* 0227736C */
s8 daNpc_Ko1_c::bitCount(u8 i_bits) {
    WWHD_FUNC(0x0227736C, s8, this, i_bits);
    s8 n = 0;
    u32 b = i_bits;
    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            n++;
        }
        b = (b >> 1) & 0xFF;
    }
    return n;
}
VERIFY(0x0227736C, &daNpc_Ko1_c::bitCount);

/* 02277394 */
u16 daNpc_Ko1_c::next_msgStatus(be<u32>* i_msgNo) {
    WWHD_FUNC(0x02277394, u16, this, i_msgNo);
    u16 ret = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch ((u32)*i_msgNo) {
    case 0xAF4: *i_msgNo = 0xB57; break;
    case 0xAF5: *i_msgNo = 0xB58; break;
    case 0xAF6: *i_msgNo = 0xAF7; break;
    case 0xAF7: *i_msgNo = 0xAF8; break;
    case 0xAF8: *i_msgNo = 0xAF9; break;
    case 0xAF9: *i_msgNo = 0xAFA; break;
    case 0xAFA: *i_msgNo = 0xAFB; break;
    case 0xAFB:
        if (bitCount(dSv_event_getEventReg(dComIfGs_event(), 0xBFFF)) != 0) {
            *i_msgNo = 0xAFC;
        } else {
            *i_msgNo = 0xAFD;
        }
        break;
    case 0xAFC:
    case 0xAFD: *i_msgNo = 0xAFE; break;
    case 0xAFE: *i_msgNo = 0xAFF; break;
    case 0xB00:
    case 0xB07: *i_msgNo = 0xB01; break;
    case 0xB01: *i_msgNo = 0xB02; break;
    case 0xB02: *i_msgNo = 0xB03; break;
    case 0xB03: *i_msgNo = 0xB04; break;
    case 0xB04: *i_msgNo = 0xB05; break;
    case 0xB05: *i_msgNo = 0xB06; break;
    case 0xB08: *i_msgNo = 0xB09; break;
    case 0xB09: *i_msgNo = 0xB0A; break;
    case 0xB0A: *i_msgNo = 0xB0B; break;
    case 0xB0B: *i_msgNo = 0xB0C; break;
    case 0xB0C: *i_msgNo = 0xB0D; break;
    case 0xB59: *i_msgNo = 0xB5A; break;
    case 0xB5A: *i_msgNo = 0xB5B; break;
    case 0xB5B: *i_msgNo = 0xB5C; break;
    case 0xB5C: *i_msgNo = 0xB5D; break;
    case 0xB5D: *i_msgNo = 0xB64; break;
    case 0xB5E: *i_msgNo = 0xB5F; break;
    case 0xB5F: *i_msgNo = 0xB60; break;
    case 0xB60: *i_msgNo = 0xB61; break;
    case 0xB61: *i_msgNo = 0xB65; break;
    default:
        ret = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return ret;
}
VERIFY(0x02277394, &daNpc_Ko1_c::next_msgStatus);

/* 022776C4 */
u32 daNpc_Ko1_c::getMsg_HNA_0() {
    WWHD_FUNC(0x022776C4, u32, this);
    return dComIfGs_isEventBit(0x0220) ? 0xB56 : 0xB55;
}
VERIFY(0x022776C4, &daNpc_Ko1_c::getMsg_HNA_0);

/* 02277704 */
u32 daNpc_Ko1_c::getMsg_HNA_1() {
    WWHD_FUNC(0x02277704, u32, this);
    return dComIfGs_isEventBit(0x0240) ? 0xAF5 : 0xAF4;
}
VERIFY(0x02277704, &daNpc_Ko1_c::getMsg_HNA_1);

/* 02277744 */
u32 daNpc_Ko1_c::getMsg_HNA_2() {
    WWHD_FUNC(0x02277744, u32, this);
    return dComIfGs_isEventBit(0x3101) ? 0xB63 : 0xB62;
}
VERIFY(0x02277744, &daNpc_Ko1_c::getMsg_HNA_2);

/* 02277784 */
u32 daNpc_Ko1_c::getMsg_HNA_3() {
    WWHD_FUNC(0x02277784, u32, this);
    if (dComIfGs_isEventBit(0x2C04)) {
        return m9DD == 0 ? 0xB5A : 0xB5E;
    }
    return 0xB59;
}
VERIFY(0x02277784, &daNpc_Ko1_c::getMsg_HNA_3);

/* 022777F4 */
u32 daNpc_Ko1_c::getMsg_BOU_0() {
    WWHD_FUNC(0x022777F4, u32, this);
    if (dComIfGs_isEventBit(0x0104)) {
        return 0xAF3;
    }
    return dComIfGs_isEventBit(0x0210) ? 0xAF2 : 0xAF1;
}
VERIFY(0x022777F4, &daNpc_Ko1_c::getMsg_BOU_0);

/* 0227786C */
u32 daNpc_Ko1_c::getMsg_BOU_1() {
    WWHD_FUNC(0x0227786C, u32, this);
    return dComIfGs_isEventBit(0x0208) ? 0xAF5 : 0xAF4;
}
VERIFY(0x0227786C, &daNpc_Ko1_c::getMsg_BOU_1);

/* 022778AC */
u32 daNpc_Ko1_c::getMsg_BOU_2() {
    WWHD_FUNC(0x022778AC, u32, this);
    if (!dComIfGs_isEventBit(0x3801)) {
        return 0xAF6;
    }
    if (dComIfGs_isEventBit(0x3340)) {
        return 0xB08;
    }
    if (bitCount(dSv_event_getEventReg(dComIfGs_event(), 0xBFFF)) < 2) {
        return 0xB00;
    }
    return 0xB07;
}
VERIFY(0x022778AC, &daNpc_Ko1_c::getMsg_BOU_2);

/* 02277978 */
u32 daNpc_Ko1_c::getMsg() {
    WWHD_FUNC(0x02277978, u32, this);
    u32 msg = 0;
    switch (mSpecificType) {
    case 0: msg = getMsg_HNA_0(); break;
    case 1: msg = getMsg_HNA_1(); break;
    case 2: msg = getMsg_HNA_2(); break;
    case 3: msg = getMsg_HNA_3(); break;
    case 5: msg = getMsg_BOU_0(); break;
    case 6: msg = getMsg_BOU_1(); break;
    case 7: msg = getMsg_BOU_2(); break;
    }
    return msg;
}
VERIFY(0x02277978, &daNpc_Ko1_c::getMsg);

/* 02277A30 */
u8 daNpc_Ko1_c::chkAttention() {
    WWHD_FUNC(0x02277A30, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x02277A30, &daNpc_Ko1_c::chkAttention);

/* 02277AB8 */
bool daNpc_Ko1_c::check_landOn() {
    WWHD_FUNC(0x02277AB8, bool, this);
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    gabi::Local<cXyz> landPos;
    landPos->set(-198160.0f, 150.0f, 319690.0f);
    gabi::Local<cXyz> diff;
    cXyz_mi(&player->current.pos, diff, landPos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->x = diff->x;
    diffXZ->y = 0.0f;
    diffXZ->z = diff->z;
    f32 dist = std_sqrtf(PSVECSquareMag(diffXZ));
    if (player->current.pos.y == landPos->y && dist < 280.0f &&
        (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 0x40)) {
        dComIfGs_onEventBit(0x0104);
        return true;
    }
    return false;
}
VERIFY(0x02277AB8, &daNpc_Ko1_c::check_landOn);

/* 02277B9C */
void daNpc_Ko1_c::ko_setPthPos() {
    WWHD_FUNC(0x02277B9C, void, this);
    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    gabi::Local<cXyz> pnt;
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    current.pos.copy(*pnt);
    dNpc_PathRun_nextIdxAuto(&mPathRun);
    dNpc_PathRun_getPoint(&mPathRun, pnt, mPathRun.mIdx);
    gabi::Local<cXyz> next;
    next->copy(*pnt);
    current.angle.y = cLib_targetAngleY(&current.pos, next);
}
VERIFY(0x02277B9C, &daNpc_Ko1_c::ko_setPthPos);

/* 02277C44 */
/* returns cXyz through the hidden result pointer (r4); i_pos is the caller's copy */
void daNpc_Ko1_c::set_tgtPos(cXyz* o_result, cXyz* i_pos) {
    WWHD_FUNC(0x02277C44, void, this, o_result, i_pos);
    gabi::Local<cXyz> offset;
    offset->set(0.0f, 0.0f, 0.0f);
    PSMTXTrans(mDoMtx_stack_c::get(), i_pos->x, i_pos->y, i_pos->z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), dComIfGp_getLinkPlayer()->current.angle.y);
    u16 angle = m9CA;
    offset->x = 80.0f * cM_ssin(angle);
    m9CA = (u16)(angle + 0x400);
    offset->z = 40.0f * cM_scos(angle);
    gabi::Local<cXyz> pos;
    PSMTXMultVec(mDoMtx_stack_c::get(), offset, pos);
    if (o_result == nullptr) {
        o_result = (cXyz*)operator_new(0xC);
        if (o_result == nullptr) {
            return;
        }
    }
    o_result->copy(*pos); /* the recompiled lfs/stfs pair keeps the bits */
}
VERIFY(0x02277C44, &daNpc_Ko1_c::set_tgtPos);

/* 02277D48 */
void daNpc_Ko1_c::setPrtcl_Hamon(f32 i_scale, f32 i_rate) {
    WWHD_FUNC(0x02277D48, void, this, i_scale, i_rate);
    gabi::Local<cXyz> scale;
    scale->set(i_scale, i_scale, i_scale);
    dPa_rippleEcallBack_end_l(&mRipple);
    JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &current.pos, nullptr, scale, 0xFF,
                                              (dPa_levelEcallBack*)&mRipple, -1, nullptr, nullptr, nullptr);
    m9FC = gabi::ea(emitter);
    if (emitter != nullptr) {
        mRipple.m10 = i_rate;
    }
}
VERIFY(0x02277D48, &daNpc_Ko1_c::setPrtcl_Hamon);

/* 02277DEC */
bool daNpc_Ko1_c::chk_start_swim() {
    WWHD_FUNC(0x02277DEC, bool, this);
    u32 acch = gabi::ea(&mObjAcch);
    if (!(gabi::load<u32>(acch + 0x28) & 0x1000)) { /* mObjAcch.ChkWaterIn() */
        dPa_rippleEcallBack_end_l(&mRipple);
        m9FC = 0;
        return false;
    }
    /* water depth: mObjAcch water height (+0x1BC) - ground height (+0x94) */
    bool ret = gabi::load<f32>(acch + 0x1BC) - gabi::load<f32>(acch + 0x94) > 62.0f;
    if (ret) {
        if (mA13 != 7) {
            setPrtcl_Hamon(1.0f, 0.0f);
        }
    } else {
        if (mA13 == 7) {
            setPrtcl_Hamon(0.5f, 1.0f);
        }
    }
    return ret;
}
VERIFY(0x02277DEC, &daNpc_Ko1_c::chk_start_swim);

/* 02277EC0 */
fpc_ProcID daNpc_Ko1_c::get_crsActorID() {
    WWHD_FUNC(0x02277EC0, fpc_ProcID, this);
    if (mCyl.ChkCoHit()) {
        u32 obj = dCcD_GetCoHitObj(&mCyl);
        if (obj != 0) {
            u32 stts = gabi::load<u32>(obj + 0x44);
            if (stts == 0) {
                return fpcM_ERROR_PROCESS_ID_e;
            }
            u32 actor = gabi::load<u32>(stts + 0xC);
            if (actor != 0) {
                return gabi::load<u32>(actor + 4); /* fopAcM_GetID */
            }
        }
    }
    return fpcM_ERROR_PROCESS_ID_e;
}
VERIFY(0x02277EC0, &daNpc_Ko1_c::get_crsActorID);

/* 02277F58 */
bool daNpc_Ko1_c::chk_areaIn(f32 i_radius, cXyz* i_pos) {
    WWHD_FUNC(0x02277F58, bool, this, i_radius, i_pos);
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, diff, i_pos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->y = 0.0f;
    diffXZ->x = diff->x;
    diffXZ->z = diff->z;
    return std_sqrtf(PSVECSquareMag(diffXZ)) < i_radius;
}
VERIFY(0x02277F58, &daNpc_Ko1_c::chk_areaIn);

/* 02277FE8 */
void daNpc_Ko1_c::setPrtcl_HanaPachi() {
    WWHD_FUNC(0x02277FE8, void, this);
    J3DModel* model = mpMorf81C->getModel();
    PSMTXCopy(getAnmMtx(model, m7E9), mDoMtx_stack_c::get());
    Mtx34* stk = mDoMtx_stack_c::get();
    gabi::Local<cXyz> pos;
    pos->y = stk->m[1][3];
    pos->z = stk->m[2][3];
    pos->x = stk->m[0][3];
    s8 roomNo = current.roomNo;
    JPABaseEmitter* e = dPa_control_set(dComIfGp_getParticle(), 0, 0x830C, pos, &current.angle, nullptr, 0xFF, nullptr,
                                        roomNo, nullptr, nullptr, nullptr);
    mA00 = gabi::ea(e);
    roomNo = current.roomNo;
    e = dPa_control_set(dComIfGp_getParticle(), 0, 0x8313, pos, &current.angle, nullptr, 0xFF, nullptr, roomNo,
                        nullptr, nullptr, nullptr);
    mA04 = gabi::ea(e);
}
VERIFY(0x02277FE8, &daNpc_Ko1_c::setPrtcl_HanaPachi);

/* 0227810C */
void daNpc_Ko1_c::clrSpd() {
    WWHD_FUNC(0x0227810C, void, this);
    speed.y = 0.0f;
    gravity = -4.5f;
    m9A4 = 0.0f;
    m99C = 0.0f;
    speedF = 0.0f;
}
VERIFY(0x0227810C, &daNpc_Ko1_c::clrSpd);

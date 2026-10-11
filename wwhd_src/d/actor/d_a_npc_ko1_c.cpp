/**
 * d_a_npc_ko1_c.cpp (WWHD)
 * NPC - Joel & Zill (Outset Island), part C: animations, messages, attention, path, particles.
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ko1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 */
#include "d/actor/d_a_npc_ko1_c.h"

const gc_tu_data gc_tu = {.safeString = 0x1001C87C};

static s32 a_bck_resID_tbl[] = {22, 15, 24, 21, 20, 13, 23, 16, 17, 18, 19, 14, 12, 38};
static s32 a_hed_bck_resID_tbl[] = {10, 5, 11, 9, 8, 3, 10, 10, 10, 6, 7, 4, 2, 10};
static s32 a_bln_bck_resID_tbl[] = {1, 0};
static daNpc_Ko1_c::anm_prm_c a_balloon_anm_prm_tbl[] = {
    {0, -1, {0, 0}, 0.0f, 0.0f, 2, {0, 0, 0, 0}},
    {1, -1, {0, 0}, 0.0f, 0.0f, 0, {0, 0, 0, 0}},
};
static daNpc_Ko1_c::anm_prm_c a_anm_prm_tbl[] = {
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {1, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {2, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {3, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {4, 1, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {5, 0, {0, 0}, 8.0f, 1.0f, 0, {0, 0, 0, 0}},
    {6, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {7, 2, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {8, 2, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {9, 2, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {10, 3, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {11, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {12, 2, {0, 0}, 8.0f, 1.0f, 0, {0, 0, 0, 0}},
    {13, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
};
static daNpc_Ko1_c::anm_prm_c a_stt_anm_prm_tbl[] = {
    {-1, -1, {0, 0}, 0.0f, 0.0f, -1, {0, 0, 0, 0}},
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {6, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {-1, -1, {0, 0}, 0.0f, 0.0f, -1, {0, 0, 0, 0}},
    {2, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {4, 1, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {1, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {2, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {2, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {5, 0, {0, 0}, 8.0f, 1.0f, 0, {0, 0, 0, 0}},
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {2, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {1, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {2, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {5, 0, {0, 0}, 8.0f, 1.0f, 0, {0, 0, 0, 0}},
    {1, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {5, 0, {0, 0}, 8.0f, 1.0f, 0, {0, 0, 0, 0}},
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {-1, -1, {0, 0}, 0.0f, 0.0f, -1, {0, 0, 0, 0}},
    {-1, -1, {0, 0}, 0.0f, 0.0f, -1, {0, 0, 0, 0}},
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {1, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {2, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {5, 0, {0, 0}, 8.0f, 1.0f, 0, {0, 0, 0, 0}},
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {7, 2, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {9, 2, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
};
static daNpc_Ko1_c::anm_prm_c a_atr_anm_prm_tbl[] = {
    {0, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {1, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {2, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {3, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {4, 1, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {5, 0, {0, 0}, 8.0f, 1.0f, 0, {0, 0, 0, 0}},
    {6, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {7, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {8, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {9, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {10, 3, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {11, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
    {13, 0, {0, 0}, 8.0f, 1.0f, 2, {0, 0, 0, 0}},
};


/* talk partners of the two-person talk (play + 0x5C38..0x5C5C) */
static inline void setTalkPartners(u32 a0, u32 a1, u32 a2) {
    u32 play = dComIfGp_ea();
    Ko1TalkPlayView* slots = Ko1_talkPlay(play);
    slots->mIndexAndActors[1] = a0;
    slots->mIndexAndActors[2] = a1;
    slots->mIndexAndActors[3] = a2;
    for (s32 i = 4; i < 11; ++i) slots->mIndexAndActors[i] = 0;
}

/* 02276B2C */
s32 daNpc_Ko1_c::anmNum_toResID(s32 i_anmNum) {
    return a_bck_resID_tbl[i_anmNum]; /* a_bck_resID_tbl */
}

/* 02276B40 */
s32 daNpc_Ko1_c::headAnmNum_toResID(s32 i_anmNum) {
    if (mType == 1) {
        return 0x25;
    }
    return a_hed_bck_resID_tbl[i_anmNum]; /* a_hed_bck_resID_tbl */
}

/* 02276B68 */
s32 daNpc_Ko1_c::balloon_anmNum_toResID(s32 i_anmNum) {
    return a_bln_bck_resID_tbl[i_anmNum]; /* a_bln_bck_resID_tbl */
}

/* 02276B7C */
u32 daNpc_Ko1_c::setAnm_tex(s8 i_btpNum) {
    if (mBtpNum != i_btpNum) {
        mBtpNum = i_btpNum;
        return Ko1_initTextureAnimation(this, 1); /* iniTexPttrnAnm(true) */
    }
    return (u32)gabi::ea(this); /* original: r3 still this */
}

/* 02276B9C */
BOOL daNpc_Ko1_c::setAnm_anm(anm_prm_c* i_prm) {
    if (mAnimationNum == i_prm->mAnmNum) {
        return TRUE;
    }
    s8 anmNum = i_prm->mAnmNum;
    mAnimationNum = anmNum;
    s32 resID = anmNum_toResID(anmNum);
    dNpc_setAnmIDRes(mpMorf, i_prm->mLoopMode, i_prm->mMorf, i_prm->mSpeed, resID, -1, "Ko");
    resID = headAnmNum_toResID(mAnimationNum);
    dNpc_setAnmIDRes(mpHeadMorf, i_prm->mLoopMode, i_prm->mMorf, i_prm->mSpeed, resID, -1, "Ko");
    mAnimationEnded = 0;
    mAnimationFrame = 0.0f;
    mAnimationState = 0;
    return TRUE;
}

/* 02276C70 */
BOOL daNpc_Ko1_c::set_balloonAnm_anm(anm_prm_c* i_prm) {
    if (mBalloonAnimationNum == i_prm->mAnmNum) {
        return TRUE;
    }
    s8 anmNum = i_prm->mAnmNum;
    mBalloonAnimationNum = anmNum;
    s32 resID = balloon_anmNum_toResID(anmNum);
    dNpc_setAnmIDRes(mpBalloonMorf, i_prm->mLoopMode, i_prm->mMorf, i_prm->mSpeed, resID, -1, "Ko");
    mbBalloonAnimationStopped = 0;
    mBalloonAnimationFrame = 0.0f;
    mBalloonAnimationState = 0;
    return TRUE;
}

/* 02276D04 */
/* tail call: set_balloonAnm_anm's result is passed through */
BOOL daNpc_Ko1_c::set_balloonAnm_NUM(s32 i_idx) {
    return set_balloonAnm_anm((&a_balloon_anm_prm_tbl[i_idx]));
}

/* 02276D18 */
void daNpc_Ko1_c::setAnm_NUM(s32 i_idx, s32 i_setTex) {
    if (i_setTex != 0) {
        setAnm_tex((&a_anm_prm_tbl[i_idx])->mBtpNum);
    }
    setAnm_anm((&a_anm_prm_tbl[i_idx]));
}

/* 02276D84 */
bool daNpc_Ko1_c::setAnm() {
    s8 btpNum = (&a_stt_anm_prm_tbl[mStatus])->mBtpNum;
    if (btpNum >= 0) {
        setAnm_tex(btpNum);
    }
    if ((&a_stt_anm_prm_tbl[mStatus])->mAnmNum >= 0) {
        setAnm_anm((&a_stt_anm_prm_tbl[mStatus]));
    }
    return true;
}

/* 02276E08 */
void daNpc_Ko1_c::chg_anmTag() {
    if (mAnimationTag == 10) {
        setAnm_NUM(4, 1);
    }
}

/* 02276E90 */
void daNpc_Ko1_c::control_anmTag() {
    if (mAnimationTag == 10 && mAnimationEnded != 0) {
        mAnimationTag = 0xFF;
        setAnm_ATR(1);
    }
}

/* 02276EB8 */
void daNpc_Ko1_c::chg_anmAtr(u8 i_atr) {
    u32 msgNo = mPartnerMsgNo;
    if (msgNo != 0xFFFFFFFF && (msgNo == 0xB03 || msgNo == 0xB0A || (msgNo >= 0xB64 && msgNo <= 0xB65))) {
        mbMoving = 0;
        mLookTarget = 1;
        m_jnt.mbTrn = 1;
    }
    if (i_atr >= 0xD || i_atr == mAnimationSelector) {
        return;
    }
    mAnimationSelector = i_atr;
    if (i_atr == 0xB) {
        mbMoving = 0;
        mLookTarget = 1;
        m_jnt.mbTrn = 1;
    }
    setAnm_ATR(1);
}

/* 02276F30 */
void daNpc_Ko1_c::control_anmAtr() {
    if (mAnimationSelector == 0xB && mAnimationEnded != 0) {
        mAnimationSelector = 0;
        setAnm_NUM(0, 1);
    }
}

/* 02276E20 */
void daNpc_Ko1_c::setAnm_ATR(s32 i_setTex) {
    if (i_setTex != 0) {
        setAnm_tex((&a_atr_anm_prm_tbl[mAnimationSelector])->mBtpNum);
    }
    setAnm_anm((&a_atr_anm_prm_tbl[mAnimationSelector]));
}

/* 02276F58 */
void daNpc_Ko1_c::anmAtr(u16 i_msgStatus) {
    u32 play = dComIfGp_ea();
    /* in a two-person talk (field_0x7d8 == 2) only the current speaker follows the message tags:
     * speaker list at play + 0x5C34 (index) / + 0x5C38 */
    bool skip = false;
    if (mTalkState == 2) {
        u32 idx = Ko1_talkPlay(play)->mIndexAndActors[0];
        if (gabi::ea(this) != Ko1_talkPlay(play)->mIndexAndActors[idx]) {
            skip = true;
        }
    }
    if (!skip) {
        switch (i_msgStatus) {
        case 6: {
            if (mbMessageTagsStarted == 0) {
                mAnimationSelector = 0xFF;
                chg_anmAtr(dComIfGp_getMesgAnimeAttrInfo());
                mbMessageTagsStarted = mbMessageTagsStarted + 1;
            }
            u8 tag = dComIfGp_getMesgAnimeTagInfo();
            dComIfGp_clearMesgAnimeTagInfo();
            if (tag != 0xFF && mAnimationTag != tag) {
                mAnimationTag = tag;
                chg_anmTag();
            }
            break;
        }
        case 0xE:
            mbMessageTagsStarted = 0;
            break;
        }
    }
    control_anmTag();
    control_anmAtr();
}

/* 02277070 */
bool daNpc_Ko1_c::chk_talk() {
    bool ret = false;
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mMessageSelector = dComIfGp_event_getPreItemNo();
            ret = true;
        }
    } else {
        mMessageSelector = 0xFF;
        ret = true;
    }
    return ret;
}

/* 02277124 */
bool daNpc_Ko1_c::chk_manzai_1() {
    /* partner process ids at 0x924 (mPartnerMetadata.mCount of them) */
    s32 ready = 0;
    u32 n;
    for (s32 i = 0; i < (s32)(n = mPartnerMetadata.mCount); i++) {
        fopNpc_npc_c* a = reinterpret_cast<fopNpc_npc_c*>(searchByID(mPartnerIds[i]));
        if (a == 0) /* JUT_ASSERT(1414, a_actor_p != NULL) */
            JUT_ASSERT_fail("d_a_npc_ko1.cpp", 0x586, "(0) != a_actor");
        if (a->mTalkState == 2) {
            ready++;
        } else {
            a->actor_status |= fopAcStts_UNK4000_e;
            u32 myId = mBsPcId;
            a->mTalkState = 1;
            a->mPartnerProcId = myId;
        }
    }
    bool ret = (u32)ready == n;
    if (ret) {
        u32 p0 = gabi::ea(searchByID(mPartnerIds[0]));
        u32 self = gabi::ea(this);
        switch (mSpecificType) {
        case 1:
            setTalkPartners(p0, self, 0);
            break;
        case 3: {
            u32 p1 = gabi::ea(searchByID(mPartnerIds[1]));
            setTalkPartners(p0, p1, self);
            break;
        }
        case 6:
            setTalkPartners(self, p0, 0);
            break;
        case 7: {
            u32 p1 = gabi::ea(searchByID(mPartnerIds[1]));
            setTalkPartners(self, p1, p0);
            break;
        }
        default:
            break;
        }
        mTalkState = 2;
    }
    return ret;
}

/* 0227732C */
u8 daNpc_Ko1_c::chk_partsNotMove() {
    return mHeadAngleY == m_jnt.mAngles[0][1] && mBodyAngleY == m_jnt.mAngles[1][1] && mActorAngleY == current.angle.y;
}

/* 02277394 */
u16 daNpc_Ko1_c::next_msgStatus(be<u32>* i_msgNo) {
    u16 ret = fopMsgStts_MSG_CONTINUES_e;
    switch ((u32)*i_msgNo) {
    case 0xAF4: *i_msgNo = 0xB57; break;
    case 0xAF5: *i_msgNo = 0xB58; break;
    case 0xAF6: *i_msgNo = 0xAF7; break;
    case 0xAF7: *i_msgNo = 0xAF8; break;
    case 0xAF8: *i_msgNo = 0xAF9; break;
    case 0xAF9: *i_msgNo = 0xAFA; break;
    case 0xAFA: *i_msgNo = 0xAFB; break;
    case 0xAFB:
        if (bitCount(dSv_event_getEventReg(dComIfGs_getEvent(), 0xBFFF)) != 0) {
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
        ret = fopMsgStts_MSG_ENDS_e;
        break;
    }
    return ret;
}

/* 022776C4 */
u32 daNpc_Ko1_c::getMsg_HNA_0() {
    return dComIfGs_isEventBit(0x0220) ? 0xB56 : 0xB55;
}

/* 02277704 */
u32 daNpc_Ko1_c::getMsg_HNA_1() {
    return dComIfGs_isEventBit(0x0240) ? 0xAF5 : 0xAF4;
}

/* 02277744 */
u32 daNpc_Ko1_c::getMsg_HNA_2() {
    return dComIfGs_isEventBit(0x3101) ? 0xB63 : 0xB62;
}

/* 02277784 */
u32 daNpc_Ko1_c::getMsg_HNA_3() {
    if (dComIfGs_isEventBit(0x2C04)) {
        return mbTalkTriggered == 0 ? 0xB5A : 0xB5E;
    }
    return 0xB59;
}

/* 022777F4 */
u32 daNpc_Ko1_c::getMsg_BOU_0() {
    if (dComIfGs_isEventBit(0x0104)) {
        return 0xAF3;
    }
    return dComIfGs_isEventBit(0x0210) ? 0xAF2 : 0xAF1;
}

/* 0227786C */
u32 daNpc_Ko1_c::getMsg_BOU_1() {
    return dComIfGs_isEventBit(0x0208) ? 0xAF5 : 0xAF4;
}

/* 0227736C */
s8 daNpc_Ko1_c::bitCount(u8 i_bits) {
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

/* 022778AC */
u32 daNpc_Ko1_c::getMsg_BOU_2() {
    if (!dComIfGs_isEventBit(0x3801)) {
        return 0xAF6;
    }
    if (dComIfGs_isEventBit(0x3340)) {
        return 0xB08;
    }
    if (bitCount(dSv_event_getEventReg(dComIfGs_getEvent(), 0xBFFF)) < 2) {
        return 0xB00;
    }
    return 0xB07;
}

/* 02277978 */
u32 daNpc_Ko1_c::getMsg() {
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

/* 02277A30 */
u8 daNpc_Ko1_c::chkAttention() {
    dAttention_c* attention = dComIfGp_getAttention();
    if (Ko1_lockonTruth(attention)) {
        return this == (void*)Ko1_lockonTarget(attention, 0);
    }
    return this == (void*)Ko1_actionTarget(attention, 0);
}

/* 022770F0 */
fopAc_ac_c* daNpc_Ko1_c::searchByID(fpc_ProcID i_procID) {
    gabi::Local<gptr<fopAc_ac_c>> o_actor;
    *o_actor = nullptr;
    Ko1_findActor(i_procID, o_actor.get()); /* fopAcM_SearchByID(id, &actor) */
    return *o_actor;
}



/* 02277AB8 */
bool daNpc_Ko1_c::check_landOn() {
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
        (reinterpret_cast<Ko1PlayerView*>(player)->mGroundFlags & 0x40)) {
        dComIfGs_onEventBit(0x0104);
        return true;
    }
    return false;
}

/* 02277B9C */
void daNpc_Ko1_c::ko_setPthPos() {
    if (mPathRun.mPath.get() == nullptr) {
        return;
    }
    gabi::Local<cXyz> pnt;
    Ko1_pathPoint(&mPathRun, pnt, mPathRun.mIdx);
    current.pos.copy(*pnt);
    Ko1_nextPathIndex(&mPathRun);
    Ko1_pathPoint(&mPathRun, pnt, mPathRun.mIdx);
    gabi::Local<cXyz> next;
    next->copy(*pnt);
    current.angle.y = cLib_targetAngleY(&current.pos, next);
}

/* 02277C44 */
/* returns cXyz through the hidden result pointer (r4); i_pos is the caller's copy */
void daNpc_Ko1_c::set_tgtPos(cXyz* o_result, cXyz* i_pos) {
    gabi::Local<cXyz> offset;
    offset->set(0.0f, 0.0f, 0.0f);
    PSMTXTrans(mDoMtx_stack_c::get(), i_pos->x, i_pos->y, i_pos->z);
    mDoMtx_YrotM(mDoMtx_stack_c::get(), dComIfGp_getLinkPlayer()->current.angle.y);
    u16 angle = mTargetOffsetAngle;
    offset->x = 80.0f * cM_ssin(angle);
    mTargetOffsetAngle = (u16)(angle + 0x400);
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

/* 02277DEC */
bool daNpc_Ko1_c::chk_start_swim() {
    if (!(mObjAcch.m_flags & dBgS_Acch::WATER_IN)) { /* mObjAcch.ChkWaterIn() */
        Ko1_endRipple(&mRipple);
        mRippleEmitter = 0;
        return false;
    }
    /* water depth: mObjAcch water height (+0x1BC) - ground height (+0x94) */
    bool ret = mObjAcch.m_wtr.m_height - mObjAcch.m_ground_h > 62.0f;
    if (ret) {
        if (mStatus != STATUS_SWIM_1) {
            setPrtcl_Hamon(1.0f, 0.0f);
        }
    } else {
        if (mStatus == STATUS_SWIM_1) {
            setPrtcl_Hamon(0.5f, 1.0f);
        }
    }
    return ret;
}

/* 02277EC0 */
fpc_ProcID daNpc_Ko1_c::get_crsActorID() {
    if (mCyl.ChkCoHit()) {
        Ko1CollisionObject* obj = Ko1_getCoHitObj(&mCyl);
        if (obj != 0) {
            dCcD_Stts* stts = obj->mpStts;
            if (stts == 0) {
                return fpcM_ERROR_PROCESS_ID_e;
            }
            fopAc_ac_c* actor = stts->mp_actor;
            if (actor != 0) {
                return actor->mBsPcId;
            }
        }
    }
    return fpcM_ERROR_PROCESS_ID_e;
}

/* 02277F58 */
bool daNpc_Ko1_c::chk_areaIn(f32 i_radius, cXyz* i_pos) {
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, diff, i_pos);
    gabi::Local<cXyz> diffXZ;
    diffXZ->y = 0.0f;
    diffXZ->x = diff->x;
    diffXZ->z = diff->z;
    return std_sqrtf(PSVECSquareMag(diffXZ)) < i_radius;
}

/* 02277D48 */
void daNpc_Ko1_c::setPrtcl_Hamon(f32 i_scale, f32 i_rate) {
    gabi::Local<cXyz> scale;
    scale->set(i_scale, i_scale, i_scale);
    Ko1_endRipple(&mRipple);
    JPABaseEmitter* emitter = dPa_control_set(dComIfGp_getParticle(), 5, 0x33, &current.pos, nullptr, scale, 0xFF,
                                              (dPa_levelEcallBack*)&mRipple, -1, nullptr, nullptr, nullptr);
    mRippleEmitter = gabi::ea(emitter);
    if (emitter != nullptr) {
        mRipple.mRate = i_rate;
    }
}

/* 02277FE8 */
void daNpc_Ko1_c::setPrtcl_HanaPachi() {
    J3DModel* model = mpBalloonMorf->getModel();
    PSMTXCopy(model->getAnmMtx(mBalloonJoint), mDoMtx_stack_c::get());
    Mtx34* stk = mDoMtx_stack_c::get();
    gabi::Local<cXyz> pos;
    pos->y = stk->m[1][3];
    pos->z = stk->m[2][3];
    pos->x = stk->m[0][3];
    s8 roomNo = current.roomNo;
    JPABaseEmitter* e = dPa_control_set(dComIfGp_getParticle(), 0, 0x830C, pos, &current.angle, nullptr, 0xFF, nullptr,
                                        roomNo, nullptr, nullptr, nullptr);
    mBalloonEmitter = gabi::ea(e);
    roomNo = current.roomNo;
    e = dPa_control_set(dComIfGp_getParticle(), 0, 0x8313, pos, &current.angle, nullptr, 0xFF, nullptr, roomNo,
                        nullptr, nullptr, nullptr);
    mBalloonLightEmitter = gabi::ea(e);
}

/* 0227810C */
void daNpc_Ko1_c::clrSpd() {
    speed.y = 0.0f;
    gravity = -4.5f;
    mMoveAcceleration = 0.0f;
    mMoveSpeed = 0.0f;
    speedF = 0.0f;
}

/* WWHD glue
ext 025163BC Ko1CollisionObject* Ko1_getCoHitObj(dCcD_GObjInf* obj)
ext 0259E778 void Ko1_pathPoint(Ko1PathRun* path, cXyz* out, u8 index)
ext 0259ED58 BOOL Ko1_nextPathIndex(Ko1PathRun* path)
ext 025A9270 void Ko1_endRipple(Ko1RippleCallback* ripple)
ext 02273C88 u32 Ko1_initTextureAnimation(daNpc_Ko1_c* actor, u32 enabled)
ext 025D54C4 void Ko1_findActor(fpc_ProcID id, gptr<fopAc_ac_c>* out)
ext 024EDFCC bool Ko1_lockonTruth(dAttention_c* attention)
ext 024EC8D0 fopAc_ac_c* Ko1_lockonTarget(dAttention_c* attention, s32 index)
ext 024EE464 fopAc_ac_c* Ko1_actionTarget(dAttention_c* attention, s32 index)
data a_bck_resID_tbl 1001CBC0
data a_hed_bck_resID_tbl 1001CBF8
data a_bln_bck_resID_tbl 1001CC30
data a_balloon_anm_prm_tbl 101BF7D8
data a_anm_prm_tbl 101BF800
data a_stt_anm_prm_tbl 101BF918
data a_atr_anm_prm_tbl 101BFB70
str 1001CC68 "Ko" in 02276B9C
str 1001CC6B "Ko" in 02276C70
str 1001CC70 "d_a_npc_ko1.cpp"
str 1001CC80 "(0) != a_actor"
*/

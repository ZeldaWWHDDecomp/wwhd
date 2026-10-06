/**
 * d_a_npc_ym1_b.cpp (WWHD)
 * NPC - Mesa & Abe (Outset Island), part B
 *
 * The GameCube decompilation (zeldaret/tww
 * src/d/actor/d_a_npc_ym1.cpp) has only "Nonmatching" stubs for this actor: the functions are
 * written from the WWHD code (cking.rpx) and verified against it, keeping the GameCube names.
 * Part B: draw, animations, messages, status and action functions, compiler-generated tail.
 */
#define SAFESTRING_VTBL 0x10023448 /* this TU's sead::SafeString vtable */
#include "d/actor/d_a_npc_ym1.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* calls into the other ym1 parts (not linked into this unit) */
static inline void ym1_init_texPttrnAnm(daNpc_Ym1_c* i_this, s8 btp, u32 modify) { gabi::call(0x022F6F38, i_this, btp, modify); }

static inline void ym1_endEvent(daNpc_Ym1_c* i_this) { gabi::call(0x022F8544, i_this); }
/* 0259D8BC dNpc_chkAttn(actor, cXyz pos (pointer to a copy), f32, f32, f32, bool) */
static inline BOOL dNpc_chkAttn(fopAc_ac_c* a, cXyz* pos, f32 p3, f32 p4, f32 p5, u32 p6) {
    return gabi::call<BOOL>(0x0259D8BC, a, pos, p6, p3, p4, p5);
}

/* daNpc_Ym1_HIO_c parameters per mSubType (0x2C each, around 0x10468978) */
static inline u32 l_HIO_prm(daNpc_Ym1_c* i_this) { return 0x10468978 + i_this->mSubType * 0x2C; }

/* 022F9150 */
/* tail call: _draw's result register is passed through (typed u32) */
static u32 daNpc_Ym1_Draw(daNpc_Ym1_c* i_this) {
    WWHD_FUNC(0x022F9150, u32, i_this);
    return gabi::call<u32>(0x022F8F30, i_this); /* i_this->_draw() */
}
VERIFY(0x022F9150, daNpc_Ym1_Draw);

/* 022F9154 */
static BOOL daNpc_Ym1_IsDelete(daNpc_Ym1_c*) {
    WWHD_FUNC(0x022F9154, BOOL, (daNpc_Ym1_c*)nullptr);
    return TRUE;
}
VERIFY(0x022F9154, daNpc_Ym1_IsDelete);

/* 022F915C */
s32 daNpc_Ym1_c::bckResID(int i_bckNum) {
    WWHD_FUNC(0x022F915C, s32, this, i_bckNum);
    return gabi::load<s32>(0x10023648 + i_bckNum * 4); /* a_bck_resID_tbl */
}
VERIFY(0x022F915C, &daNpc_Ym1_c::bckResID);

/* 022F9170 */
void daNpc_Ym1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    WWHD_FUNC(0x022F9170, void, this, i_anmPrmP);
    s8 bck = i_anmPrmP->mAnmNum;
    if (bck < 0 || (u32)(s32)mA1B == (u32)(s32)bck) {
        return;
    }
    s32 resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->mLoopMode;
    f32 morf = i_anmPrmP->mMorf;
    f32 speed = i_anmPrmP->mSpeed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    s8 num = i_anmPrmP->mAnmNum;
    mA09 = 0;
    m9EC = 0.0f;
    mA08 = 0;
    mA1B = num;
    if (num == 8) {
        /* HIO (0x1046897C, 0x2C per mSubType): morf of this animation */
        mpMorf->setMorf(gabi::load<f32>(0x1046897C + mSubType * 0x2C));
    }
}
VERIFY(0x022F9170, &daNpc_Ym1_c::setAnm_anm);

/* 022F9230 */
void daNpc_Ym1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {
    WWHD_FUNC(0x022F9230, void, this, i_anmNum, i_setBtp);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C6A68);
    if (i_setBtp != 0) {
        ym1_init_texPttrnAnm(this, a_anm_prm_tbl[i_anmNum].mBtpNum, 1);
    }
    setAnm_anm(&a_anm_prm_tbl[i_anmNum]);
}
VERIFY(0x022F9230, &daNpc_Ym1_c::setAnm_NUM);

/* 022F92A0 */
void daNpc_Ym1_c::setAnm() {
    WWHD_FUNC(0x022F92A0, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C6B38);
    ym1_init_texPttrnAnm(this, a_anm_prm_tbl[mA1D].mBtpNum, 1);
    setAnm_anm(&a_anm_prm_tbl[mA1D]);
}
VERIFY(0x022F92A0, &daNpc_Ym1_c::setAnm);

/* 022F9310 */
void daNpc_Ym1_c::setAnm_ATR() {
    WWHD_FUNC(0x022F9310, void, this);
    anm_prm_c* a_anm_prm_tbl = gabi::at<anm_prm_c>(0x101C6BF8);
    ym1_init_texPttrnAnm(this, a_anm_prm_tbl[(u8)mA18].mBtpNum, 1);
    setAnm_anm(&a_anm_prm_tbl[(u8)mA18]);
}
VERIFY(0x022F9310, &daNpc_Ym1_c::setAnm_ATR);

/* 022F9378 */
void daNpc_Ym1_c::chngAnmAtr(u8 i_atr) {
    WWHD_FUNC(0x022F9378, void, this, i_atr);
    if (i_atr == (u8)mA18 || i_atr >= 0xD) {
        return;
    }
    mA18 = i_atr;
    setAnm_ATR();
}
VERIFY(0x022F9378, &daNpc_Ym1_c::chngAnmAtr);

/* 022F9394 */
void daNpc_Ym1_c::anmAtr(u16 i_msgStatus) {
    WWHD_FUNC(0x022F9394, void, this, i_msgStatus);
    switch (i_msgStatus) {
    case 6: {
        if (mA23 == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            mA23 = mA23 + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo();
        if (tag != 0xFF && tag != (u8)mA19) {
            dComIfGp_clearMesgAnimeTagInfo();
            mA19 = tag; /* chngAnmTag(): empty */
        }
        break;
    }
    case 0xE:
        mA23 = 0;
        break;
    }
}
VERIFY(0x022F9394, &daNpc_Ym1_c::anmAtr);

/* 022F9758 */
u32 daNpc_Ym1_c::getMsg_YM1_0() {
    WWHD_FUNC(0x022F9758, u32, this);
    if (dComIfGs_checkCollect(0)) {
        return dComIfGs_isEventBit(0x0010) ? 0x903 : 0x901;
    }
    if (dComIfGs_isEventBit(0x0020)) {
        return 0x8FF;
    }
    return dComIfGs_isEventBit(0x2A80) ? 0x8FD : 0x90A;
}
VERIFY(0x022F9758, &daNpc_Ym1_c::getMsg_YM1_0);

/* 022F980C */
u32 daNpc_Ym1_c::getMsg_YM1_1() {
    WWHD_FUNC(0x022F980C, u32, this);
    return dComIfGs_isEventBit(0x2904) ? 0x907 : 0x904;
}
VERIFY(0x022F980C, &daNpc_Ym1_c::getMsg_YM1_1);

/* 022F9858 */
bool daNpc_Ym1_c::chk_BlackPig() {
    WWHD_FUNC(0x022F9858, bool, this);
    return (dSv_event_getEventReg(dComIfGs_event(), 0xBFFF) >> 2) & 1;
}
VERIFY(0x022F9858, &daNpc_Ym1_c::chk_BlackPig);

/* 022F9890 */
u32 daNpc_Ym1_c::getMsg_YM2_0() {
    WWHD_FUNC(0x022F9890, u32, this);
    if (chk_BlackPig()) {
        return dComIfGs_isEventBit(0x0008) ? 0xA2C : 0xA2B;
    }
    return dComIfGs_isEventBit(0x0080) ? 0xA2A : 0xA29;
}
VERIFY(0x022F9890, &daNpc_Ym1_c::getMsg_YM2_0);

/* 022F9914 */
u32 daNpc_Ym1_c::getMsg_YM2_1() {
    WWHD_FUNC(0x022F9914, u32, this);
    return dComIfGs_isEventBit(0x0B01) ? 0xA2E : 0xA2D;
}
VERIFY(0x022F9914, &daNpc_Ym1_c::getMsg_YM2_1);

/* 022F9960 */
u32 daNpc_Ym1_c::getMsg_YM2_2() {
    WWHD_FUNC(0x022F9960, u32, this);
    if (!dComIfGs_isEventBit(0x3140)) {
        return 0xA2F;
    }
    if (dKy_daynight_check() == 1) {
        return dComIfGs_isEventBit(0x3402) ? 0xA3A : 0xA39;
    }
    if (!dComIfGs_isEventBit(0x3580)) {
        return 0xA3B;
    }
    return dComIfGs_isEventBit(0x3540) ? 0xA41 : 0xA3E;
}
VERIFY(0x022F9960, &daNpc_Ym1_c::getMsg_YM2_2);

/* 022F9A44 */
/* tail call: getMsg_YM2_2's result register is passed through */
u32 daNpc_Ym1_c::getMsg_YM2_3() {
    WWHD_FUNC(0x022F9A44, u32, this);
    return gabi::call<u32>(0x022F9960, this); /* getMsg_YM2_2() */
}
VERIFY(0x022F9A44, &daNpc_Ym1_c::getMsg_YM2_3);

/* 022F9A48 */
u32 daNpc_Ym1_c::getMsg() {
    WWHD_FUNC(0x022F9A48, u32, this);
    switch (mStaff) {
    case 0:
        return getMsg_YM1_0();
    case 1:
        return getMsg_YM1_1();
    case 2:
        return getMsg_YM2_0();
    case 3:
        return getMsg_YM2_1();
    case 4:
        return getMsg_YM2_2();
    case 5:
        return getMsg_YM2_3();
    }
    return 0;
}
VERIFY(0x022F9A48, &daNpc_Ym1_c::getMsg);

/* 022F9ACC */
bool daNpc_Ym1_c::chk_talk() {
    WWHD_FUNC(0x022F9ACC, bool, this);
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            mA0A = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    mA0A = 0xFF;
    return true;
}
VERIFY(0x022F9ACC, &daNpc_Ym1_c::chk_talk);

/* 022F9B64 */
u8 daNpc_Ym1_c::chk_parts_notMov() {
    WWHD_FUNC(0x022F9B64, u8, this);
    return m9F0.y == m_jnt.mAngles[0][1] && m9F0.z == m_jnt.mAngles[1][1] && m9F0.x == current.angle.y;
}
VERIFY(0x022F9B64, &daNpc_Ym1_c::chk_parts_notMov);

/* 022F9BA4 */
u8 daNpc_Ym1_c::chkAttention() {
    WWHD_FUNC(0x022F9BA4, u8, this);
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}
VERIFY(0x022F9BA4, &daNpc_Ym1_c::chkAttention);

/* 022F9464 */
u16 daNpc_Ym1_c::next_msgStatus(be<u32>* o_msgNoP) {
    WWHD_FUNC(0x022F9464, u16, this, o_msgNoP);
    u16 status = 0xF; /* fopMsgStts_MSG_CONTINUES_e */
    switch (*o_msgNoP) {
    case 0x8FD:
        *o_msgNoP = 0x8FE;
        break;
    case 0x8FF:
        *o_msgNoP = 0x900;
        break;
    case 0x901:
        *o_msgNoP = 0x902;
        break;
    case 0x90A:
        *o_msgNoP = 0x90B;
        break;
    case 0x904:
        *o_msgNoP = 0x905;
        break;
    case 0x905:
        *o_msgNoP = 0x906;
        break;
    case 0x907:
        *o_msgNoP = 0x908;
        break;
    case 0x908:
        if (dComIfGs_isEventBit(0x3004)) {
            return 0x10;
        }
        *o_msgNoP = 0x909;
        break;
    case 0xA2F:
        if (dKy_daynight_check() == 1) {
            *o_msgNoP = 0xA30;
        } else if (dSv_event_getEventReg(dComIfGs_event(), 0xBFFF) != 0) {
            *o_msgNoP = dComIfGs_isEventBit(0x3402) ? 0xA31 : 0xA33;
        } else {
            *o_msgNoP = dComIfGs_isEventBit(0x3402) ? 0xA37 : 0xA35;
        }
        break;
    case 0xA31:
        *o_msgNoP = 0xA32;
        break;
    case 0xA33:
        *o_msgNoP = 0xA34;
        break;
    case 0xA35:
        *o_msgNoP = 0xA36;
        break;
    case 0xA37:
        *o_msgNoP = 0xA38;
        break;
    case 0xA3B:
        *o_msgNoP = dComIfGs_isEventBit(0x3402) ? 0xA3C : 0xA3D;
        break;
    case 0xA3E:
        *o_msgNoP = dComIfGs_isEventBit(0x3402) ? 0xA40 : 0xA3F;
        break;
    case 0xA41:
        *o_msgNoP = 0xA42;
        break;
    default:
        status = 0x10; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return status;
}
VERIFY(0x022F9464, &daNpc_Ym1_c::next_msgStatus);

/* 022F9C2C */
void daNpc_Ym1_c::setStt(s8 i_status) {
    WWHD_FUNC(0x022F9C2C, void, this, i_status);
    s8 old = mA1D;
    mA1D = i_status;
    switch ((u32)(s32)i_status) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 8:
    case 10:
    case 11:
        mA1C = 0;
        break;
    case 5:
    case 7:
    case 9:
        mA1C = 0;
        mA18 = (s8)0xFF;
        mA1E = old;
        mA19 = (s8)0xFF;
        mA23 = 0;
        break;
    case 6:
        mA1C = 0;
        mKariTimer = (s16)cLib_getRndValue(90, 180);
        break;
    }
    setAnm();
}
VERIFY(0x022F9C2C, &daNpc_Ym1_c::setStt);

/* 022F9D04 */
/* i_pos: cXyz by value (pointer to the caller's copy) */
bool daNpc_Ym1_c::chk_areaIN(f32 i_dist, cXyz* i_pos) {
    WWHD_FUNC(0x022F9D04, bool, this, i_dist, i_pos);
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, diff, i_pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    f32 dist = std_sqrtf(PSVECSquareMag(xz));
    f32 dy = dComIfGp_getLinkPlayer()->current.pos.y - i_pos->y;
    if (dist < i_dist && std::fabs(dy) < 300.0f) {
        return true;
    }
    return false;
}
VERIFY(0x022F9D04, &daNpc_Ym1_c::chk_areaIN);

/* 022F9E00 */
BOOL daNpc_Ym1_c::kari_1() {
    WWHD_FUNC(0x022F9E00, BOOL, this);
    /* HD: the player distance is computed and not used */
    gabi::Local<cXyz> diff;
    cXyz_mi(&dComIfGp_getLinkPlayer()->current.pos, diff, &current.pos);
    gabi::Local<cXyz> xz;
    f32 dx = diff->x, dz = diff->z;
    xz->y = 0.0f;
    xz->x = dx;
    xz->z = dz;
    std_sqrtf(PSVECSquareMag(xz));
    dComIfGp_get();
    if (cLib_calcTimer(&mKariTimer) == 0) {
        gabi::Local<cXyz> pos;
        pos->x = current.pos.x;
        pos->y = current.pos.y;
        pos->z = current.pos.z;
        if (chk_areaIN(gabi::load<f32>(l_HIO_prm(this)), pos) || mA0C == 0) {
            setStt(2);
            mA1F = 0;
            mA14 = 1;
            setAnm_NUM(2, 1);
        }
    }
    return TRUE;
}
VERIFY(0x022F9E00, &daNpc_Ym1_c::kari_1);

/* 022F9EF4 */
BOOL daNpc_Ym1_c::wait_1() {
    WWHD_FUNC(0x022F9EF4, BOOL, this);
    if (mA1B == 2) {
        if (mA08 == 0) {
            return TRUE;
        }
        setAnm_NUM(3, 1);
        mA1F = 0;
        mA14 = 1;
        m9FE = (s16)cLib_getRndValue(15, 30);
        return TRUE;
    }
    if (mA15) {
        cLib_addCalcAngleS(&current.angle.y, mRotYTarget, 4, gabi::load<s16>(l_HIO_prm(this) - 0xA), 0x80);
    }
    if (mA13) {
        if (chk_talk()) {
            setStt(5);
            mA14 = 0;
            mA15 = 0;
            mA1F = 1;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    mA1F = 1;
    mA1C = 2;
    mA14 = 1;
    if (mA12) {
        m9FE = (s16)cLib_getRndValue(15, 30);
    }
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (!chk_areaIN(gabi::load<f32>(l_HIO_prm(this)) + 50.0f, pos) && cLib_calcTimer(&m9FE) == 0) {
        setStt(3);
        mA1F = 0;
        mA14 = 1;
    }
    return TRUE;
}
VERIFY(0x022F9EF4, &daNpc_Ym1_c::wait_1);

/* 022FA0A0 */
BOOL daNpc_Ym1_c::wait_2() {
    WWHD_FUNC(0x022FA0A0, BOOL, this);
    s16 diff = 0;
    if (mA15) {
        cLib_addCalcAngleS(&current.angle.y, mRotYTarget, 4, gabi::load<s16>(l_HIO_prm(this) - 0xA), 0x80);
        s16 angle = current.angle.y;
        diff = (s16)(mRotYTarget - angle);
    }
    if (mA13) {
        if (chk_talk()) {
            setStt(5);
            mA14 = 0;
            mA1F = 1;
            mA15 = 0;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    mA1F = 0;
    mA1C = 2;
    mA14 = 1;
    if (diff != 0) {
        return TRUE;
    }
    f32 attnDist, attnRange;
    if (mSubType == 1) {
        attnDist = 120.0f;
        attnRange = 250.0f;
    } else {
        attnDist = 92.0f;
        attnRange = 200.0f;
    }
    if (mA12) {
        m9FE = (s16)cLib_getRndValue(15, 30);
    }
    if (cLib_calcTimer(&m9FE)) {
        mA1F = 1;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    u32 flag = mA1F == 1;
    gabi::Local<cXyz> pos;
    pos->x = player->current.pos.x;
    pos->y = player->current.pos.y;
    pos->z = player->current.pos.z;
    if (!dNpc_chkAttn(this, pos, attnRange, 50.0f, attnDist, flag)) {
        mA1F = 0;
        mA15 = 1;
    }
    return TRUE;
}
VERIFY(0x022FA0A0, &daNpc_Ym1_c::wait_2);

/* 022FA288 */
u8 daNpc_Ym1_c::talk_1() {
    WWHD_FUNC(0x022FA288, u8, this);
    u8 ret = chk_parts_notMov();
    talk(1);
    if (mbHasMsg && fopMsgM_getStatus() == 0x13) {
        switch (mCurrMsgNo) {
        case 0x8FE:
        case 0x90B:
            dComIfGs_onEventBit(0x0020);
            break;
        case 0x902:
            dComIfGs_onEventBit(0x0010);
            break;
        case 0x906:
            dComIfGs_onEventBit(0x2904);
            break;
        case 0xA29:
            dComIfGs_onEventBit(0x0080);
            break;
        case 0xA2B:
            dComIfGs_onEventBit(0x0008);
            break;
        case 0xA2D:
            dComIfGs_onEventBit(0x0B01);
            break;
        case 0xA30:
        case 0xA32:
        case 0xA34:
        case 0xA36:
        case 0xA38:
            dComIfGs_onEventBit(0x3140);
            break;
        case 0xA3C:
        case 0xA3D:
            dComIfGs_onEventBit(0x3580);
            break;
        case 0xA40:
            dComIfGs_onEventBit(0x3540);
            break;
        }
        mA0A = 0xFF;
        mA13 = 0;
        setStt(mA1E);
        m9FE = (s16)cLib_getRndValue(15, 30);
        ym1_endEvent(this);
    }
    return ret;
}
VERIFY(0x022FA288, &daNpc_Ym1_c::talk_1);

/* 022FA4E4 */
BOOL daNpc_Ym1_c::turn_1() {
    WWHD_FUNC(0x022FA4E4, BOOL, this);
    cLib_addCalcAngleS(&current.angle.y, mRotYTarget, 4, gabi::load<s16>(l_HIO_prm(this) - 0xA), 0x80);
    if ((s16)(mRotYTarget - current.angle.y) != 0) {
        return TRUE;
    }
    if (mA0C) {
        setStt(1);
        mpMorf->setMorf(10.0f);
        mA1F = 0;
        mA14 = 1;
        mKariTimer = (s16)cLib_getRndValue(30, 60);
        return TRUE;
    }
    setStt(4);
    mA1F = 0;
    mA14 = 1;
    return TRUE;
}
VERIFY(0x022FA4E4, &daNpc_Ym1_c::turn_1);

/* 022FA5B8 */
BOOL daNpc_Ym1_c::NBTwai() {
    WWHD_FUNC(0x022FA5B8, BOOL, this);
    if (mA13) {
        if (chk_talk()) {
            setStt(7);
            mA1F = 0;
            mA14 = 1;
        }
        return TRUE;
    }
    mA1F = 0;
    mA14 = 1;
    mA1C = 2;
    if (chk_BlackPig()) {
        setStt(8);
        mA1F = 0;
        mA14 = 1;
        mA15 = 1;
        return TRUE;
    }
    if (mA1B == 6) {
        if (mA08 != 0 || mA12 != 0) {
            setAnm_NUM(5, 1);
            mKariTimer = (s16)cLib_getRndValue(90, 180);
        }
        return TRUE;
    }
    if (mA12) {
        m9FE = (s16)cLib_getRndValue(15, 30);
    }
    if (cLib_calcTimer(&m9FE)) {
        mA1F = 1;
        return TRUE;
    }
    if (cLib_calcTimer(&mKariTimer) == 0) {
        setAnm_NUM(6, 1);
    }
    return TRUE;
}
VERIFY(0x022FA5B8, &daNpc_Ym1_c::NBTwai);

/* 022FA70C */
BOOL daNpc_Ym1_c::SITwai() {
    WWHD_FUNC(0x022FA70C, BOOL, this);
    if (mA13) {
        if (chk_talk()) {
            setStt(5);
            mA1F = 1;
            mA14 = 1;
        }
        return TRUE;
    }
    mA1C = 2;
    mA1F = 0;
    mA14 = 1;
    if (mA12) {
        m9FE = (s16)cLib_getRndValue(15, 30);
    }
    if (cLib_calcTimer(&m9FE)) {
        mA1F = 1;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    u32 flag = mA1F == 1;
    gabi::Local<cXyz> pos;
    pos->x = player->current.pos.x;
    pos->y = player->current.pos.y;
    pos->z = player->current.pos.z;
    if (!dNpc_chkAttn(this, pos, 250.0f, 50.0f, 120.0f, flag)) {
        mA1F = 0;
    }
    return TRUE;
}
VERIFY(0x022FA70C, &daNpc_Ym1_c::SITwai);

/* the action functions: mA22 is the action phase (0: init, 1..3: run; set_action uses others) */

/* 022FA828 */
BOOL daNpc_Ym1_c::wait_action1(void*) {
    WWHD_FUNC(0x022FA828, BOOL, this, (void*)nullptr);
    s8 phase = mA22;
    if (phase == 0) {
        setStt(1);
        mA15 = 1;
        mA22 = mA22 + 1;
    } else if ((u32)(s32)phase <= 3) {
        mA12 = chkAttention();
        switch (mA1D) {
        case 1:
            m9F8 = kari_1();
            break;
        case 2:
            m9F8 = wait_1();
            break;
        case 3:
            m9F8 = turn_1();
            break;
        case 4:
            m9F8 = wait_2();
            break;
        case 5:
            m9F8 = gabi::call<u32>(0x022FA288, this); /* talk_1(): the result register is stored as a word */
            break;
        }
    }
    return TRUE;
}
VERIFY(0x022FA828, &daNpc_Ym1_c::wait_action1);

/* 022FA940 */
BOOL daNpc_Ym1_c::wait_action2(void*) {
    WWHD_FUNC(0x022FA940, BOOL, this, (void*)nullptr);
    s8 phase = mA22;
    if (phase == 0) {
        if (mStaff == 5) {
            setStt(10);
        } else {
            setStt(8);
        }
        mA15 = 1;
        mA22 = mA22 + 1;
    } else if ((u32)(s32)phase <= 3) {
        mA12 = chkAttention();
        switch (mA1D) {
        case 5:
            m9F8 = gabi::call<u32>(0x022FA288, this); /* talk_1(): the result register is stored as a word */
            break;
        case 8:
        case 10:
            m9F8 = wait_2();
            break;
        }
    }
    return TRUE;
}
VERIFY(0x022FA940, &daNpc_Ym1_c::wait_action2);

/* 022FAA28 */
BOOL daNpc_Ym1_c::wait_action3(void*) {
    WWHD_FUNC(0x022FAA28, BOOL, this, (void*)nullptr);
    gabi::Local<cXyz> offset; /* cXyz(0.0f, 0.0f, 110.0f) */
    offset->x = 0.0f;
    offset->z = 110.0f;
    offset->y = 0.0f;
    s8 phase = mA22;
    if (phase == 0) {
        if (chk_BlackPig()) {
            setStt(8);
            mA15 = 1;
            mA22 = mA22 + 1;
        } else {
            setStt(6);
            PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), m9B6.y);
            PSMTXMultVec(mDoMtx_stack_c::get(), offset, &m9D4);
            mA22 = mA22 + 1;
        }
    } else if ((u32)(s32)phase <= 3) {
        mA12 = chkAttention();
        switch (mA1D) {
        case 5:
        case 7:
            m9F8 = gabi::call<u32>(0x022FA288, this); /* talk_1(): the result register is stored as a word */
            break;
        case 6:
            m9F8 = NBTwai();
            break;
        case 8:
            m9F8 = wait_2();
            break;
        }
    }
    return TRUE;
}
VERIFY(0x022FAA28, &daNpc_Ym1_c::wait_action3);

/* 022FAB88 */
BOOL daNpc_Ym1_c::wait_action4(void*) {
    WWHD_FUNC(0x022FAB88, BOOL, this, (void*)nullptr);
    s8 phase = mA22;
    if (phase == 0) {
        setStt(11);
        mA22 = mA22 + 1;
    } else if ((u32)(s32)phase <= 3) {
        mA12 = chkAttention();
        switch (mA1D) {
        case 5:
            m9F8 = gabi::call<u32>(0x022FA288, this); /* talk_1(): the result register is stored as a word */
            break;
        case 11:
            m9F8 = SITwai();
            break;
        }
    }
    return TRUE;
}
VERIFY(0x022FAB88, &daNpc_Ym1_c::wait_action4);

/* 022FAC34 */
BOOL daNpc_Ym1_c::demo_action1(void*) {
    WWHD_FUNC(0x022FAC34, BOOL, this, (void*)nullptr);
    s8 phase = mA22;
    if (phase == 0) {
        mA22 = phase + 1;
    } else if ((u32)(s32)phase <= 3) {
        mA12 = chkAttention();
    }
    return TRUE;
}
VERIFY(0x022FAC34, &daNpc_Ym1_c::demo_action1);

/* static initialisation of a 4-byte function-local constant on first use (028FEAC0 memcpy) */
static inline void local_static_init(u32 guard, u32 dst, u32 src) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        memcpy_g(gabi::at<u8>(dst), gabi::at<u8>(src), 4); /* 028FEAC0: memcpy import stub */
    }
}
/* 025BEBB8 dSnap_RegistFig(type, actor, const cXyz& pos, s16 angle, f32, f32, f32) */
static inline void dSnap_RegistFig_pos(s32 type, fopAc_ac_c* a, cXyz* pos, s16 ang, f32 x, f32 y, f32 z) {
    gabi::call(0x025BEBB8, type, a, pos, ang, x, y, z);
}

/* 022F8F30 */
BOOL daNpc_Ym1_c::_draw() {
    WWHD_FUNC(0x022F8F30, BOOL, this);
    J3DModel* head_p = mpHeadModel;
    J3DModelData* head_info_p = J3DModel_getModelData_l(head_p);
    J3DModel* morf_model_p = mpMorf->getModel();
    if (mA0E || mA10) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), head_p, &tevStr);
    dComIfGp_get(); /* HD: an unused accessor call */
    if (mSubType == 1 || mSubType == 2) {
        mpMorf->entryDL();
    }
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, head_info_p, mBlinkFrame);
    mDoExt_modelEntryDL(head_p);
    gabi::store<u32>(gabi::ea(head_info_p) + 0x38, 0); /* mBtpAnm.remove() */
    if (m7E8.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), m7E8, &tevStr);
        mDoExt_modelEntryDL(m7E8);
    }
    /* HD: no shadowDraw() */
    switch (mSubType) {
    case 1:
        dSnap_RegistFig(0x50, this, 1.0f, 1.0f, 1.0f);
        break;
    case 2:
        dSnap_RegistFig_pos(0x4E, this, &eyePos, shape_angle.y, 1.0f, 1.0f, 1.0f);
        break;
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (gabi::load<u8>(0x10468974 + mSubType * 0x2C)) {
        local_static_init(0x101FDA50, 0x101FEBF4, 0x10023438);
        local_static_init(0x101FDAC0, 0x101FEBF8, 0x1002343C);
        local_static_init(0x101FDA44, 0x101FEBE8, 0x10023440);
    }
    u8 v = mKariFlag;
    mKariFlag = 0;
    mA0C = v;
    return TRUE;
}
VERIFY(0x022F8F30, &daNpc_Ym1_c::_draw);

/* ---- compiler-generated tail ---- */
/* daNpc_Ym1_childHIO_c (0x2C, HD: vtable at 0) and daNpc_Ym1_HIO_c (0x64): l_HIO at 0x10468978 */
struct daNpc_Ym1_childHIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ u8 mPrm[0x24];   /* the per-character parameters (copied from .data 0x101C6CC8) */
    /* 0x28 */ be<s32> mNo;
};
WWHD_SIZE(daNpc_Ym1_childHIO_c, 0x2C);
struct daNpc_Ym1_HIO_c {
    /* 0x00 */ be<u32> __vtbl;
    /* 0x04 */ be<s8> mNo;
    /* 0x05 */ u8 _05[3];
    /* 0x08 */ be<s32> m08;
    /* 0x0C */ daNpc_Ym1_childHIO_c mChild[2];
};
WWHD_SIZE(daNpc_Ym1_HIO_c, 0x64);

/* 022FAC9C daNpc_Ym1_childHIO_c::daNpc_Ym1_childHIO_c (HD: allocates when this == NULL) */
static daNpc_Ym1_childHIO_c* daNpc_Ym1_childHIO_c_ct(daNpc_Ym1_childHIO_c* i_this) {
    WWHD_FUNC(0x022FAC9C, daNpc_Ym1_childHIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Ym1_childHIO_c*)operator_new(0x2C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x10023490;
    return i_this;
}
VERIFY(0x022FAC9C, daNpc_Ym1_childHIO_c_ct);

/* 022FACDC daNpc_Ym1_HIO_c::daNpc_Ym1_HIO_c (HD: allocates when this == NULL) */
static daNpc_Ym1_HIO_c* daNpc_Ym1_HIO_c_ct(daNpc_Ym1_HIO_c* i_this) {
    WWHD_FUNC(0x022FACDC, daNpc_Ym1_HIO_c*, i_this);
    if (i_this == nullptr) {
        i_this = (daNpc_Ym1_HIO_c*)operator_new(0x64);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = 0x100234A0;
    gabi::call(0x028EFFD0, i_this->mChild, 2, 0x2C, 0x022FAC9C); /* __construct_array(mChild, 2, size, daNpc_Ym1_childHIO_c ctor) */
    for (s32 i = 0; i < 2; i++) {
        i_this->mChild[i].mNo = i;
        memcpy_g(i_this->mChild[i].mPrm, gabi::at<u8>(0x101C6CC8 + i * 0x24), 0x24); /* a_prm_tbl[i]; 028FEAC0 memcpy */
    }
    i_this->m08 = -1;
    i_this->mNo = -1;
    return i_this;
}
VERIFY(0x022FACDC, daNpc_Ym1_HIO_c_ct);

/* 022FAD8C: static initialisation of the translation unit */
static void __sinit_d_a_npc_ym1_cpp() {
    WWHD_FUNC(0x022FAD8C, void);
    sinit_header_statics(0x10468950, 0x101C6D10);
    daNpc_Ym1_HIO_c_ct(gabi::at<daNpc_Ym1_HIO_c>(0x10468978)); /* static daNpc_Ym1_HIO_c l_HIO */
}
VERIFY(0x022FAD8C, __sinit_d_a_npc_ym1_cpp);

/* 022FAE2C: sead::SafeString deleting destructor (this TU's copy) */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    WWHD_FUNC(0x022FAE2C, void, i_this, flags);
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}
VERIFY(0x022FAE2C, SafeString_dt);

/* 022FAE40: daNpc_Ym1_c deleting destructor (compiler-generated, HD virtual destructor) */
static void daNpc_Ym1_c_dt(daNpc_Ym1_c* i_this, s32 flags) {
    WWHD_FUNC(0x022FAE40, void, i_this, flags);
    if (i_this != nullptr) {
        gabi::call(0x02515A70, &i_this->mCyl, 2);                 /* dCcD_Cyl::~dCcD_Cyl (own mCyl) */
        gabi::call(0x02515A70, &i_this->fopNpc_npc_c::mCyl, 2);   /* dCcD_Cyl::~dCcD_Cyl (fopNpc_npc_c::mCyl) */
        gabi::call(0x02515860, &i_this->mStts, 2);                /* dCcD_Stts::~dCcD_Stts */
        gabi::call(0x02018034, gabi::ea(&i_this->mAcchCir) + 0x14, 2); /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x20, 0x10023470);
        gabi::store<u32>(gabi::ea(&i_this->mObjAcch) + 0x14, 0x10023480);
        gabi::call(0x024EFD9C, &i_this->mObjAcch, 0);
        gabi::call(0x025D50BC, i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}
VERIFY(0x022FAE40, daNpc_Ym1_c_dt);

/* 022FAEE8: sead::SafeString::assureTerminationImpl_ (this TU's copy; empty) */
static void SafeString_assureTerminationImpl(SafeString*) {
    WWHD_FUNC(0x022FAEE8, void, (SafeString*)nullptr);
}
VERIFY(0x022FAEE8, SafeString_assureTerminationImpl);

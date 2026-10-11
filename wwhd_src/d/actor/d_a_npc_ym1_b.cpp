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
#include "d/actor/d_a_npc_ym1_b.h"
using namespace ym1_b;
void ym1InitTexPattern(daNpc_Ym1_c*, s8, u32);
void ym1EndEvent(daNpc_Ym1_c*);
BOOL ym1CheckAttention(fopAc_ac_c*, cXyz*, u32, f32, f32, f32);
u32 ym1DrawResult(daNpc_Ym1_c*);
u32 ym1GetMesaMsg2(daNpc_Ym1_c*);
u32 ym1TalkWordResult(daNpc_Ym1_c*);
void ym1RegisterFigureAt(s32, fopAc_ac_c*, cXyz*, s16, f32, f32, f32);
void ym1ConstructArray(void*, s32, s32, u32);
void ym1DestroyCylinder(dCcD_Cyl*, s32);
void ym1DestroyStatus(dCcD_Stts*, s32);
void ym1DestroyCircle(cM3dGCir_l*, s32);
void ym1DestroyAcch(dBgS_ObjAcch*, s32);
void ym1DestroyActor(daNpc_Ym1_c*, s32);
// HD: private aliases of existing TU storage; constructors retain ownership in this part.
static ChildSettings l_settings;
static daNpc_Ym1_HIO_c l_HIO;
static s32 a_bck_resID_tbl[13] = {0, 1, 2, 5, 3, 8, 6, 7, 4, 19, 16, 17, 18};
static daNpc_Ym1_c::anm_prm_c a_anm_num_tbl[13] = {
    {0, 0, {}, 8.0f, 1.0f, 2}, {1, 0, {}, 8.0f, 1.0f, 2},  {2, 0, {}, 8.0f, 1.0f, 0},
    {3, 0, {}, 8.0f, 1.0f, 2}, {4, 0, {}, 8.0f, 1.0f, 2},  {5, 0, {}, 8.0f, 1.0f, 2},
    {6, 0, {}, 8.0f, 1.0f, 0}, {7, 0, {}, 8.0f, 1.0f, 2},  {8, 0, {}, 8.0f, 1.0f, 2},
    {9, 0, {}, 8.0f, 1.0f, 2}, {10, 0, {}, 8.0f, 1.0f, 2}, {11, 0, {}, 8.0f, 1.0f, 2},
    {12, 0, {}, 8.0f, 1.0f, 2}};
static daNpc_Ym1_c::anm_prm_c a_anm_prm_tbl[12] = {
    {-1, -1, {}, 0.0f, 0.0f, -1}, {1, 0, {}, 8.0f, 1.0f, 2},    {3, 0, {}, 8.0f, 1.0f, 2},
    {3, 0, {}, 8.0f, 1.0f, 2},    {3, 0, {}, 8.0f, 1.0f, 2},    {-1, -1, {}, 0.0f, 0.0f, -1},
    {5, 0, {}, 8.0f, 1.0f, 2},    {-1, -1, {}, 0.0f, 0.0f, -1}, {0, 0, {}, 8.0f, 1.0f, 2},
    {-1, -1, {}, 0.0f, 0.0f, -1}, {9, 0, {}, 8.0f, 1.0f, 2},    {10, 0, {}, 8.0f, 1.0f, 2}};
static daNpc_Ym1_c::anm_prm_c a_anm_atr_tbl[13] = {
    {0, 0, {}, 8.0f, 1.0f, 2}, {1, 0, {}, 8.0f, 1.0f, 2},  {2, 0, {}, 8.0f, 1.0f, 0},
    {3, 0, {}, 8.0f, 1.0f, 2}, {4, 0, {}, 8.0f, 1.0f, 2},  {5, 0, {}, 8.0f, 1.0f, 2},
    {6, 0, {}, 8.0f, 1.0f, 0}, {7, 0, {}, 8.0f, 1.0f, 2},  {8, 0, {}, 8.0f, 1.0f, 2},
    {9, 0, {}, 8.0f, 1.0f, 2}, {10, 0, {}, 8.0f, 1.0f, 2}, {11, 0, {}, 8.0f, 1.0f, 2},
    {12, 0, {}, 8.0f, 1.0f, 2}};
static ParameterTable a_prm_tbl;
static u32 l_redInitialized;
static u32 l_blueInitialized;
static u32 l_yellowInitialized;
static ColorWord l_red;
static ColorWord l_blue;
static ColorWord l_yellow;
static ColorWord l_redDefault = {0xFF000080};
static ColorWord l_blueDefault = {0x0000FF80};
static ColorWord l_yellowDefault = {0xFFFF0080};

// HD: publish each guard before copying the four-byte debug color through the import.
static void initializeColor(be<u32>* guard, ColorWord* dst, const ColorWord* src) {
    if (*guard == 0) {
        *guard = 1;
        memcpy_g(dst, src, 4);
    }
}

/* 022F915C */
s32 daNpc_Ym1_c::bckResID(int i_bckNum) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return s32(*entry(a_bck_resID_tbl, i_bckNum)); /* a_bck_resID_tbl */
}

/* 022F9170 */
void daNpc_Ym1_c::setAnm_anm(anm_prm_c* i_anmPrmP) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 bck = i_anmPrmP->mAnmNum;
    if (bck < 0 || (u32)(s32)state(this)->m8AB == (u32)(s32)bck) {
        return;
    }
    s32 resID = bckResID(bck);
    s32 loopMode = i_anmPrmP->mLoopMode;
    f32 morf = i_anmPrmP->mMorf;
    f32 speed = i_anmPrmP->mSpeed;
    dNpc_setAnmIDRes(mpMorf, loopMode, morf, speed, resID, -1, mArcName);
    s8 num = i_anmPrmP->mAnmNum;
    state(this)->mAnimationWrapped = 0;
    state(this)->mPreviousMorfFrame = 0.0f;
    state(this)->mAnimationStopped = 0;
    state(this)->m8AB = num;
    if (num == 8) {

        mpMorf->setMorf(f32(settings(&l_settings, mSubType)->mMorf));
    }
}

/* 022F9230 */
void daNpc_Ym1_c::setAnm_NUM(int i_anmNum, int i_setBtp) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    anm_prm_c* table = a_anm_num_tbl;
    if (i_setBtp != 0) {
        ym1InitTexPattern(this, entry(table, i_anmNum)->mBtpNum, 1);
    }
    setAnm_anm(entry(table, i_anmNum));
}

/* 022F92A0 */
void daNpc_Ym1_c::setAnm() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    anm_prm_c* table = a_anm_prm_tbl;
    ym1InitTexPattern(this, entry(table, state(this)->mStatus)->mBtpNum, 1);
    setAnm_anm(entry(table, state(this)->mStatus));
}

/* 022F9378 */
void daNpc_Ym1_c::chngAnmAtr(u8 i_atr) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_atr == (u8)state(this)->mAnimationAttribute || i_atr >= 0xD) {
        return;
    }
    state(this)->mAnimationAttribute = i_atr;
    setAnm_ATR();
}

/* 022F9310 */
void daNpc_Ym1_c::setAnm_ATR() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    anm_prm_c* table = a_anm_atr_tbl;
    ym1InitTexPattern(this, entry(table, (u8)state(this)->mAnimationAttribute)->mBtpNum, 1);
    setAnm_anm(entry(table, (u8)state(this)->mAnimationAttribute));
}

/* 022F9394 */
void daNpc_Ym1_c::anmAtr(u16 i_msgStatus) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    switch (i_msgStatus) {
    case MSG_TALKING: {
        if (state(this)->mMessageAnimationState == 0) {
            chngAnmAtr(dComIfGp_getMesgAnimeAttrInfo());
            state(this)->mMessageAnimationState = state(this)->mMessageAnimationState + 1;
        }
        u8 tag = dComIfGp_getMesgAnimeTagInfo();
        if (tag != 0xFF && tag != (u8)state(this)->mAnimationTag) {
            dComIfGp_clearMesgAnimeTagInfo();
            state(this)->mAnimationTag = tag; /* chngAnmTag(): empty */
        }
        break;
    }
    case MSG_DISPLAYED:
        state(this)->mMessageAnimationState = 0;
        break;
    }
}

/* 022F9464 */
u16 daNpc_Ym1_c::next_msgStatus(be<u32>* o_msgNoP) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u16 status = MSG_CONTINUES; /* fopMsgStts_MSG_CONTINUES_e */
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
            return MSG_ENDS;
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
        status = MSG_ENDS; /* fopMsgStts_MSG_ENDS_e */
        break;
    }
    return status;
}

/* 022F9758 */
u32 daNpc_Ym1_c::getMsg_YM1_0() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGs_checkCollect(0)) {
        return dComIfGs_isEventBit(0x0010) ? 0x903 : 0x901;
    }
    if (dComIfGs_isEventBit(0x0020)) {
        return 0x8FF;
    }
    return dComIfGs_isEventBit(0x2A80) ? 0x8FD : 0x90A;
}

/* 022F980C */
u32 daNpc_Ym1_c::getMsg_YM1_1() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return dComIfGs_isEventBit(0x2904) ? 0x907 : 0x904;
}

/* 022F9890 */
u32 daNpc_Ym1_c::getMsg_YM2_0() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (chk_BlackPig()) {
        return dComIfGs_isEventBit(0x0008) ? 0xA2C : 0xA2B;
    }
    return dComIfGs_isEventBit(0x0080) ? 0xA2A : 0xA29;
}

/* 022F9914 */
u32 daNpc_Ym1_c::getMsg_YM2_1() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return dComIfGs_isEventBit(0x0B01) ? 0xA2E : 0xA2D;
}

/* 022F9960 */
u32 daNpc_Ym1_c::getMsg_YM2_2() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
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

// HD: retain the full result of the message helper tail call.
/* 022F9A44 */
u32 daNpc_Ym1_c::getMsg_YM2_3() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ym1GetMesaMsg2(this); /* getMsg_YM2_2() */
}

/* 022F9A48 */
u32 daNpc_Ym1_c::getMsg() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
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

/* 022F9858 */
bool daNpc_Ym1_c::chk_BlackPig() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return (dSv_event_getEventReg(dComIfGs_event(), 0xBFFF) >> 2) & 1;
}

/* 022F9ACC */
bool daNpc_Ym1_c::chk_talk() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (dComIfGp_event_chkTalkXY()) {
        if (dComIfGp_evmng_ChkPresentEnd()) {
            state(this)->mPresentItem = dComIfGp_event_getPreItemNo();
            return true;
        }
        return false;
    }
    state(this)->mPresentItem = 0xFF;
    return true;
}

/* 022F9B64 */
u8 daNpc_Ym1_c::chk_parts_notMov() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return state(this)->mPreviousAngles.y == m_jnt.mAngles[0][1] &&
           state(this)->mPreviousAngles.z == m_jnt.mAngles[1][1] &&
           state(this)->mPreviousAngles.x == current.angle.y;
}

/* 022F9BA4 */
u8 daNpc_Ym1_c::chkAttention() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    dAttention_c* attention = dComIfGp_getAttention();
    if (dAttention_LockonTruth(attention)) {
        return this == (void*)dAttention_LockonTarget(attention, 0);
    }
    return this == (void*)dAttention_ActionTarget(attention, 0);
}

/* 022F9C2C */
void daNpc_Ym1_c::setStt(s8 i_status) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 old = state(this)->mStatus;
    state(this)->mStatus = i_status;
    switch ((u32)(s32)i_status) {
    case STT_KARI:
    case STT_WAIT_1:
    case STT_TURN:
    case STT_WAIT_2:
    case STT_MESA_WAIT:
    case STT_MESA_LATE_WAIT:
    case STT_SIT_WAIT:
        state(this)->mActionSubstate = 0;
        break;
    case STT_TALK:
    case STT_NBT_TALK:
    case STT_OTHER_TALK:
        state(this)->mActionSubstate = 0;
        state(this)->mAnimationAttribute = (s8)0xFF;
        state(this)->mReturnStatus = old;
        state(this)->mAnimationTag = (s8)0xFF;
        state(this)->mMessageAnimationState = 0;
        break;
    case STT_NBT_WAIT:
        state(this)->mActionSubstate = 0;
        mKariTimer = (s16)cLib_getRndValue(90, 180);
        break;
    }
    setAnm();
}

// HD: i_pos points to the caller's by-value copy.
/* 022F9D04 */
bool daNpc_Ym1_c::chk_areaIN(f32 i_dist, cXyz* i_pos) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
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

/* 022F9E00 */
BOOL daNpc_Ym1_c::kari_1() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    // HD: retain the unused distance calculation and play accessor call.
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
        if (chk_areaIN(f32(settings(&l_settings, mSubType)->mAreaDistance), pos) || state(this)->m89C == 0) {
            setStt(STT_WAIT_1);
            state(this)->m8AF = 0;
            state(this)->m8A4 = 1;
            setAnm_NUM(2, 1);
        }
    }
    return TRUE;
}

/* 022F9EF4 */
BOOL daNpc_Ym1_c::wait_1() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (state(this)->m8AB == 2) {
        if (state(this)->mAnimationStopped == 0) {
            return TRUE;
        }
        setAnm_NUM(3, 1);
        state(this)->m8AF = 0;
        state(this)->m8A4 = 1;
        state(this)->mAttentionTimer = (s16)cLib_getRndValue(15, 30);
        return TRUE;
    }
    if (state(this)->mTurnToPlayer) {
        cLib_addCalcAngleS(&current.angle.y, mRotYTarget, 4, s16(settings(&l_settings, mSubType)->mTurnSpeed),
                           0x80);
    }
    if (state(this)->mTalkRequested) {
        if (chk_talk()) {
            setStt(STT_TALK);
            state(this)->m8A4 = 0;
            state(this)->mTurnToPlayer = 0;
            state(this)->m8AF = 1;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    state(this)->m8AF = 1;
    state(this)->mActionSubstate = 2;
    state(this)->m8A4 = 1;
    if (state(this)->mPlayerAttending) {
        state(this)->mAttentionTimer = (s16)cLib_getRndValue(15, 30);
    }
    gabi::Local<cXyz> pos;
    pos->x = current.pos.x;
    pos->y = current.pos.y;
    pos->z = current.pos.z;
    if (!chk_areaIN(f32(settings(&l_settings, mSubType)->mAreaDistance) + 50.0f, pos) &&
        cLib_calcTimer(&state(this)->mAttentionTimer) == 0) {
        setStt(STT_TURN);
        state(this)->m8AF = 0;
        state(this)->m8A4 = 1;
    }
    return TRUE;
}

/* 022FA0A0 */
BOOL daNpc_Ym1_c::wait_2() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s16 diff = 0;
    if (state(this)->mTurnToPlayer) {
        cLib_addCalcAngleS(&current.angle.y, mRotYTarget, 4, s16(settings(&l_settings, mSubType)->mTurnSpeed),
                           0x80);
        s16 angle = current.angle.y;
        diff = (s16)(mRotYTarget - angle);
    }
    if (state(this)->mTalkRequested) {
        if (chk_talk()) {
            setStt(STT_TALK);
            state(this)->m8A4 = 0;
            state(this)->m8AF = 1;
            state(this)->mTurnToPlayer = 0;
            m_jnt.mbTrn = 1;
        }
        return TRUE;
    }
    state(this)->m8AF = 0;
    state(this)->mActionSubstate = 2;
    state(this)->m8A4 = 1;
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
    if (state(this)->mPlayerAttending) {
        state(this)->mAttentionTimer = (s16)cLib_getRndValue(15, 30);
    }
    if (cLib_calcTimer(&state(this)->mAttentionTimer)) {
        state(this)->m8AF = 1;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    u32 flag = state(this)->m8AF == 1;
    gabi::Local<cXyz> pos;
    pos->x = player->current.pos.x;
    pos->y = player->current.pos.y;
    pos->z = player->current.pos.z;
    if (!ym1CheckAttention(this, pos, flag, attnRange, 50.0f, attnDist)) {
        state(this)->m8AF = 0;
        state(this)->mTurnToPlayer = 1;
    }
    return TRUE;
}

/* 022FA288 */
u8 daNpc_Ym1_c::talk_1() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    u8 ret = chk_parts_notMov();
    talk(1);
    if (mbHasMsg && fopMsgM_getStatus() == MSG_DESTROYED) {
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
        state(this)->mPresentItem = 0xFF;
        state(this)->mTalkRequested = 0;
        setStt(state(this)->mReturnStatus);
        state(this)->mAttentionTimer = (s16)cLib_getRndValue(15, 30);
        ym1EndEvent(this);
    }
    return ret;
}

/* 022FA4E4 */
BOOL daNpc_Ym1_c::turn_1() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    cLib_addCalcAngleS(&current.angle.y, mRotYTarget, 4, s16(settings(&l_settings, mSubType)->mTurnSpeed),
                       0x80);
    if ((s16)(mRotYTarget - current.angle.y) != 0) {
        return TRUE;
    }
    if (state(this)->m89C) {
        setStt(STT_KARI);
        mpMorf->setMorf(10.0f);
        state(this)->m8AF = 0;
        state(this)->m8A4 = 1;
        mKariTimer = (s16)cLib_getRndValue(30, 60);
        return TRUE;
    }
    setStt(STT_WAIT_2);
    state(this)->m8AF = 0;
    state(this)->m8A4 = 1;
    return TRUE;
}

/* 022FA5B8 */
BOOL daNpc_Ym1_c::NBTwai() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (state(this)->mTalkRequested) {
        if (chk_talk()) {
            setStt(STT_NBT_TALK);
            state(this)->m8AF = 0;
            state(this)->m8A4 = 1;
        }
        return TRUE;
    }
    state(this)->m8AF = 0;
    state(this)->m8A4 = 1;
    state(this)->mActionSubstate = 2;
    if (chk_BlackPig()) {
        setStt(STT_MESA_WAIT);
        state(this)->m8AF = 0;
        state(this)->m8A4 = 1;
        state(this)->mTurnToPlayer = 1;
        return TRUE;
    }
    if (state(this)->m8AB == 6) {
        if (state(this)->mAnimationStopped != 0 || state(this)->mPlayerAttending != 0) {
            setAnm_NUM(5, 1);
            mKariTimer = (s16)cLib_getRndValue(90, 180);
        }
        return TRUE;
    }
    if (state(this)->mPlayerAttending) {
        state(this)->mAttentionTimer = (s16)cLib_getRndValue(15, 30);
    }
    if (cLib_calcTimer(&state(this)->mAttentionTimer)) {
        state(this)->m8AF = 1;
        return TRUE;
    }
    if (cLib_calcTimer(&mKariTimer) == 0) {
        setAnm_NUM(6, 1);
    }
    return TRUE;
}

/* 022FA70C */
BOOL daNpc_Ym1_c::SITwai() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (state(this)->mTalkRequested) {
        if (chk_talk()) {
            setStt(STT_TALK);
            state(this)->m8AF = 1;
            state(this)->m8A4 = 1;
        }
        return TRUE;
    }
    state(this)->mActionSubstate = 2;
    state(this)->m8AF = 0;
    state(this)->m8A4 = 1;
    if (state(this)->mPlayerAttending) {
        state(this)->mAttentionTimer = (s16)cLib_getRndValue(15, 30);
    }
    if (cLib_calcTimer(&state(this)->mAttentionTimer)) {
        state(this)->m8AF = 1;
    }
    fopAc_ac_c* player = dComIfGp_getLinkPlayer();
    u32 flag = state(this)->m8AF == 1;
    gabi::Local<cXyz> pos;
    pos->x = player->current.pos.x;
    pos->y = player->current.pos.y;
    pos->z = player->current.pos.z;
    if (!ym1CheckAttention(this, pos, flag, 250.0f, 50.0f, 120.0f)) {
        state(this)->m8AF = 0;
    }
    return TRUE;
}

/* 022FA828 */
BOOL daNpc_Ym1_c::wait_action1(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 phase = state(this)->mActionPhase;
    if (phase == 0) {
        setStt(STT_KARI);
        state(this)->mTurnToPlayer = 1;
        state(this)->mActionPhase = state(this)->mActionPhase + 1;
    } else if ((u32)(s32)phase <= 3) {
        state(this)->mPlayerAttending = chkAttention();
        switch (state(this)->mStatus) {
        case STT_KARI:
            state(this)->mEyePositionEnabled = kari_1();
            break;
        case STT_WAIT_1:
            state(this)->mEyePositionEnabled = wait_1();
            break;
        case STT_TURN:
            state(this)->mEyePositionEnabled = turn_1();
            break;
        case STT_WAIT_2:
            state(this)->mEyePositionEnabled = wait_2();
            break;
        case STT_TALK:
            state(this)->mEyePositionEnabled =
                ym1TalkWordResult(this); /* talk_1(): the result register is stored as a word */
            break;
        }
    }
    return TRUE;
}

/* 022FA940 */
BOOL daNpc_Ym1_c::wait_action2(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 phase = state(this)->mActionPhase;
    if (phase == 0) {
        if (mStaff == 5) {
            setStt(STT_MESA_LATE_WAIT);
        } else {
            setStt(STT_MESA_WAIT);
        }
        state(this)->mTurnToPlayer = 1;
        state(this)->mActionPhase = state(this)->mActionPhase + 1;
    } else if ((u32)(s32)phase <= 3) {
        state(this)->mPlayerAttending = chkAttention();
        switch (state(this)->mStatus) {
        case STT_TALK:
            state(this)->mEyePositionEnabled =
                ym1TalkWordResult(this); /* talk_1(): the result register is stored as a word */
            break;
        case STT_MESA_WAIT:
        case STT_MESA_LATE_WAIT:
            state(this)->mEyePositionEnabled = wait_2();
            break;
        }
    }
    return TRUE;
}

/* 022FAA28 */
BOOL daNpc_Ym1_c::wait_action3(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    gabi::Local<cXyz> offset; /* cXyz(0.0f, 0.0f, 110.0f) */
    offset->x = 0.0f;
    offset->z = 110.0f;
    offset->y = 0.0f;
    s8 phase = state(this)->mActionPhase;
    if (phase == 0) {
        if (chk_BlackPig()) {
            setStt(STT_MESA_WAIT);
            state(this)->mTurnToPlayer = 1;
            state(this)->mActionPhase = state(this)->mActionPhase + 1;
        } else {
            setStt(STT_NBT_WAIT);
            PSMTXTrans(mDoMtx_stack_c::get(), current.pos.x, current.pos.y, current.pos.z);
            mDoMtx_YrotM(mDoMtx_stack_c::get(), state(this)->m846.y);
            PSMTXMultVec(mDoMtx_stack_c::get(), offset, &state(this)->mNbtAttentionPosition);
            state(this)->mActionPhase = state(this)->mActionPhase + 1;
        }
    } else if ((u32)(s32)phase <= 3) {
        state(this)->mPlayerAttending = chkAttention();
        switch (state(this)->mStatus) {
        case STT_TALK:
        case STT_NBT_TALK:
            state(this)->mEyePositionEnabled =
                ym1TalkWordResult(this); /* talk_1(): the result register is stored as a word */
            break;
        case STT_NBT_WAIT:
            state(this)->mEyePositionEnabled = NBTwai();
            break;
        case STT_MESA_WAIT:
            state(this)->mEyePositionEnabled = wait_2();
            break;
        }
    }
    return TRUE;
}

/* 022FAB88 */
BOOL daNpc_Ym1_c::wait_action4(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 phase = state(this)->mActionPhase;
    if (phase == 0) {
        setStt(STT_SIT_WAIT);
        state(this)->mActionPhase = state(this)->mActionPhase + 1;
    } else if ((u32)(s32)phase <= 3) {
        state(this)->mPlayerAttending = chkAttention();
        switch (state(this)->mStatus) {
        case STT_TALK:
            state(this)->mEyePositionEnabled =
                ym1TalkWordResult(this); /* talk_1(): the result register is stored as a word */
            break;
        case STT_SIT_WAIT:
            state(this)->mEyePositionEnabled = SITwai();
            break;
        }
    }
    return TRUE;
}

/* 022FAC34 */
BOOL daNpc_Ym1_c::demo_action1(void*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    s8 phase = state(this)->mActionPhase;
    if (phase == 0) {
        state(this)->mActionPhase = phase + 1;
    } else if ((u32)(s32)phase <= 3) {
        state(this)->mPlayerAttending = chkAttention();
    }
    return TRUE;
}

/* 022F8F30 */
BOOL daNpc_Ym1_c::_draw() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    J3DModel* head_p = mpHeadModel;
    J3DModelData* head_info_p = J3DModel_getModelData_l(head_p);
    J3DModel* morf_model_p = mpMorf->getModel();
    if (state(this)->m89E || state(this)->m8A0) {
        return TRUE;
    }
    settingTevStruct(dKy_getEnvlight(), TEV_TYPE_ACTOR, &current.pos, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), morf_model_p, &tevStr);
    setLightTevColorType(dKy_getEnvlight(), head_p, &tevStr);
    dComIfGp_get(); // HD: retain the unused accessor call.
    if (mSubType == 1 || mSubType == 2) {
        mpMorf->entryDL();
    }
    mDoExt_btpAnm_entry((mDoExt_btpAnm*)(void*)mBtpAnm, head_info_p, mBlinkFrame);
    mDoExt_modelEntryDL(head_p);
    removeTexPattern(head_info_p); /* mBtpAnm.remove() */
    if (state(this)->m6D0.get() != nullptr) {
        setLightTevColorType(dKy_getEnvlight(), state(this)->m6D0, &tevStr);
        mDoExt_modelEntryDL(state(this)->m6D0);
    }
    // HD: the GameCube shadow pass is absent.
    switch (mSubType) {
    case 1:
        dSnap_RegistFig(0x50, this, 1.0f, 1.0f, 1.0f);
        break;
    case 2:
        ym1RegisterFigureAt(0x4E, this, &eyePos, shape_angle.y, 1.0f, 1.0f, 1.0f);
        break;
    }
    /* debug leftovers: function-local static colors initialised on first use */
    if (u8(settings(&l_settings, mSubType)->mDebugDraw)) {
        initializeColor(&l_redInitialized, &l_red, &l_redDefault);
        initializeColor(&l_blueInitialized, &l_blue, &l_blueDefault);
        initializeColor(&l_yellowInitialized, &l_yellow, &l_yellowDefault);
    }
    u8 v = mKariFlag;
    mKariFlag = 0;
    state(this)->m89C = v;
    return TRUE;
}

// HD: actor glue and compiler-generated entries.
// HD: draw's full-word result passes through the tail call.
/* 022F9150 */
static u32 daNpc_Ym1_Draw(daNpc_Ym1_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return ym1DrawResult(i_this); /* i_this->_draw() */
}

/* 022F9154 */
static BOOL daNpc_Ym1_IsDelete(daNpc_Ym1_c*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    return TRUE;
}

// HD: compiler-generated initialization, HIO constructors and deleting destructors.
// HD: constructors allocate storage when passed null.
/* 022FAC9C */
static daNpc_Ym1_childHIO_c* daNpc_Ym1_childHIO_c_ct(daNpc_Ym1_childHIO_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this == nullptr) {
        i_this = (daNpc_Ym1_childHIO_c*)operator_new(0x2C);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = CHILD_HIO_VTABLE;
    return i_this;
}

/* 022FACDC */
static daNpc_Ym1_HIO_c* daNpc_Ym1_HIO_c_ct(daNpc_Ym1_HIO_c* i_this) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this == nullptr) {
        i_this = (daNpc_Ym1_HIO_c*)operator_new(0x64);
        if (i_this == nullptr)
            return i_this;
    }
    i_this->__vtbl = HIO_VTABLE;
    ym1ConstructArray(
        i_this->mChild, 2, 0x2C,
        gabi::guest_fn((const void*)&daNpc_Ym1_childHIO_c_ct)); /* __construct_array(mChild, 2, size,
                                                                   daNpc_Ym1_childHIO_c ctor) */
    for (s32 i = 0; i < 2; i++) {
        i_this->mChild[i].mNo = i;
        memcpy_g(i_this->mChild[i].mPrm, &a_prm_tbl.entries[i], 0x24); /* a_prm_tbl[i]; 028FEAC0 memcpy */
    }
    i_this->m08 = -1;
    i_this->mNo = -1;
    return i_this;
}

/* 022FAD8C */
static void __sinit_d_a_npc_ym1_cpp() {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    sinit_header_statics(0x10468950, 0x101C6D10);
    daNpc_Ym1_HIO_c_ct(&l_HIO); /* static daNpc_Ym1_HIO_c l_HIO */
}

/* 022FAE2C */
static void SafeString_dt(SafeString* i_this, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this != nullptr && (flags & 1))
        operator_delete(i_this);
}

/* 022FAE40 */
static void daNpc_Ym1_c_dt(daNpc_Ym1_c* i_this, s32 flags) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
    if (i_this != nullptr) {
        ym1DestroyCylinder(&i_this->mCyl, 2);               /* dCcD_Cyl::~dCcD_Cyl (own mCyl) */
        ym1DestroyCylinder(&i_this->fopNpc_npc_c::mCyl, 2); /* dCcD_Cyl::~dCcD_Cyl (fopNpc_npc_c::mCyl) */
        ym1DestroyStatus(&i_this->mStts, 2);                /* dCcD_Stts::~dCcD_Stts */
        ym1DestroyCircle(&i_this->mAcchCir.m_cir, 2);       /* cM3dGCir::~cM3dGCir (mAcchCir.m_cir) */
        acchVtables(&i_this->mObjAcch)->actorTable = ACCH_ACTOR_VTABLE;
        acchVtables(&i_this->mObjAcch)->baseTable = ACCH_BASE_VTABLE;
        ym1DestroyAcch(&i_this->mObjAcch, 0);
        ym1DestroyActor(i_this, 0); /* fopAc_ac_c::~fopAc_ac_c */
        if (flags & 1)
            operator_delete(i_this);
    }
}

/* 022FAEE8 */
static void SafeString_assureTerminationImpl(SafeString*) {
    auto originalFrame = gabi::AutoFrame(gabi::current_function());
}

// clang-format off
/* WWHD glue (tools/verify/autoglue.py)
data l_settings 1046896E
data l_HIO 10468978
data a_bck_resID_tbl 10023648
data a_anm_num_tbl 101C6A68
data a_anm_prm_tbl 101C6B38
data a_anm_atr_tbl 101C6BF8
data a_prm_tbl 101C6CC8
data l_redInitialized 101FDA50
data l_blueInitialized 101FDAC0
data l_yellowInitialized 101FDA44
data l_red 101FEBF4
data l_blue 101FEBF8
data l_yellow 101FEBE8
data l_redDefault 10023438
data l_blueDefault 1002343C
data l_yellowDefault 10023440
ext 022F6F38 void ym1InitTexPattern(daNpc_Ym1_c*,s8,u32)
ext 022F8544 void ym1EndEvent(daNpc_Ym1_c*)
ext 0259D8BC BOOL ym1CheckAttention(fopAc_ac_c*,cXyz*,u32,f32,f32,f32)
ext 022F8F30 u32 ym1DrawResult(daNpc_Ym1_c*)
ext 022F9960 u32 ym1GetMesaMsg2(daNpc_Ym1_c*)
ext 022FA288 u32 ym1TalkWordResult(daNpc_Ym1_c*)
ext 025BEBB8 void ym1RegisterFigureAt(s32,fopAc_ac_c*,cXyz*,s16,f32,f32,f32)
ext 028EFFD0 void ym1ConstructArray(void*,s32,s32,u32)
ext 02515A70 void ym1DestroyCylinder(dCcD_Cyl*,s32)
ext 02515860 void ym1DestroyStatus(dCcD_Stts*,s32)
ext 02018034 void ym1DestroyCircle(cM3dGCir_l*,s32)
ext 024EFD9C void ym1DestroyAcch(dBgS_ObjAcch*,s32)
ext 025D50BC void ym1DestroyActor(daNpc_Ym1_c*,s32)
*/
// clang-format on

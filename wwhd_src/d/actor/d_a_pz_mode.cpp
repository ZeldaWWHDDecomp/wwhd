/**
 * d_a_pz_mode.cpp (WWHD): d_a_pz mode (state) functions. 
 * Written from the WWHD code (the GameCube decompilation has only stubs for d_a_pz), verified
 * against cking.rpx.
 */
#include "d/actor/d_a_pz.h"

static PzModeEntryHD mode_tbl[11] = {
    {&daPz_c::modeWaitInit, &daPz_c::modeWait, "WAIT"},
    {&daPz_c::modeMoveInit, &daPz_c::modeMove, "MOVE"},
    {&daPz_c::modeAttackWaitInit, &daPz_c::modeAttackWait, "ATTACKWAIT"},
    {&daPz_c::modeAttackInit, &daPz_c::modeAttack, "ATTACK"},
    {&daPz_c::modeDefendInit, &daPz_c::modeDefend, "DEFEND"},
    {&daPz_c::modeDownInit, &daPz_c::modeDown, "DOWN"},
    {&daPz_c::modeAfraidInit, &daPz_c::modeAfraid, "AFRAID"},
    {&daPz_c::modeSideStepInit, &daPz_c::modeSideStep, "SIDE_STEP"},
    {&daPz_c::modeBackStepInit, &daPz_c::modeBackStep, "BACK_STEP"},
    {&daPz_c::modeTalkInit, &daPz_c::modeTalk, "TALK"},
    {&daPz_c::modeFollowInit, &daPz_c::modeFollow, "FOLLOW"}};

static inline f32 REG12_F(int i) {
    return REG_F(12, i);
}
/* 025A8BFC dPa_control_c::setSimpleLand(poly, pos, angle, tevstr, f32, f32, f32, int* out, int) */
static inline u32 setSimpleLand(daPz_c* a, gabi::Local<be<s32>>& out) {
    return pzSetSimpleLand(
        dComIfGp_getParticle(), gabi::ea(static_cast<cBgS_PolyInfo*>(&a->mObjAcch.m_gnd)),
        &a->current.pos, &a->shape_angle, &a->tevStr, 1.25f, 1.5f, 1.0f, out.get(), 7);
}
// HD: / 02544830 / 02544044 dEvent_manager_c
// Native binding 02542D88
static inline const char* dComIfGp_evmng_getMyNowCutName(s32 staffId) {
    return pzGetNowCutName(dComIfGp_getPEvtManager(), staffId);
}
static inline s32 strcmp_g(const char* a, const char* literal) {
    u32 p = gabi::ea(a);
    u32 q = gabi::guest_str(literal);
    for (;; p++, q++) {
        u8 c = pzStringByte(p), d = pzStringByte(q);
        if (c != d || c == 0)
            return (s32)c - (s32)d;
    }
}

/* 0245588C */
void daPz_c::modeWait() {

    cXyz eye;
    dNpc_playerEyePos(&eye, l_HIO.mPlayerEyeYOffset);
    m08C4.copy(eye);
    u32 play = dComIfGp_ea();
    /* HD: getEventIdx's result register is passed on unextended */
    u32 evIdx =
        pzGetEventIndex(dComIfGp_getPEvtManager(), "btl_of_swroom" /* "btl_of_swroom" */, 0xFF);
    if (pzGetEventData(play + PLAY_EVTMANAGER, evIdx) == 0) { /* getEventData */
        m_jnt.mbTrn = 0;
        mbHeadOnly = 1;
        m_jnt.mbBackBoneLock = 1;
        m_jnt.mbHeadLock = 1;
        return;
    }
    s32 staffId = dComIfGp_evmng_getMyStaffId("p_zelda" /* "p_zelda" */, nullptr, 0);
    if (staffId == -1)
        return;
    if (strcmp_g(dComIfGp_evmng_getMyNowCutName(staffId), "Turn") == 0) {
        m_jnt.mbBackBoneLock = 0;
        mbHeadOnly = 0;
        m_jnt.mbHeadLock = 0;
        m_jnt.mbTrn = 1;
        if (pzCLib_calcTimer(&mModeTimer) == 0)
            dComIfGp_evmng_cutEnd(staffId);
        return;
    }
    if (strcmp_g(dComIfGp_evmng_getMyNowCutName(staffId), "Retire") == 0) {
        mBlinkTimer = 0;
        s32 frame = REG_S(8, 0) + 4;
        if ((s32)mEyeFrame < frame)
            return;
        mEyeFrame = (u8)frame;
        mEventIce.mLightShrinkTimer = 1;
        mBlinkTimer = 1000;
        dComIfGp_evmng_cutEnd(staffId);
        return;
    }
    if (mEventIce.mLightShrinkTimer == 0)
        mBlinkTimer = 1000;
    m_jnt.mbTrn = 0;
}

/* 02455AB8 */
void daPz_c::modeMoveInit() {

    u32 o = 2 * m08B0;
    be<u32>& attnFlags = attention_info.flags;
    attnFlags |= 0xA;
    f32 rnd = cM_rndF((f32)l_HIO.mMoveWaitRandom[o / 2]);
    mModeTimer = gabi::ftoi((f32)l_HIO.mMoveWaitTime[o / 2] + rnd);
    mAttackWaitTimer = 10;
    mDodgeTimer = 120;
    setAnm(ANM_RUN, 0, 0xF);
    if (cM_rndF(100.0f) < l_HIO.mReverseDirectionChance)
        m0920 = m0920 == 1 ? -1 : 1;
    if (l_HIO.mbLookAtPlayer == 0) {
        if (mbHasGanondorf != 0) {
            m08C4.copy(mGanondorfPos4);
            mCircleRadius = l_HIO.mCircleRadius[m08B0];
            mCircleSpeed = l_HIO.mCircleSpeed;
            mCircleCenter.x = m08C4.x;
            mCircleCenter.y = m08C4.y;
            mCircleCenter.z = m08C4.z;
            mCircleAngleSpeed = (s16)((REG12_S(0) + 0x150) * m0920);
            pzDLib_setCirclePath(&mCircleCenter);
            m_jnt.mbTrn = 0;
            m_jnt.mbHeadLock = 0;
            mbHeadOnly = 0;
            m_jnt.mbBackBoneLock = 0;
            return;
        }
    } else {
        cXyz eye;
        dNpc_playerEyePos(&eye, l_HIO.mPlayerEyeYOffset);
        m08C4.copy(eye);
    }
    /* dLib_circle_path_c around the look position */
    mCircleRadius = l_HIO.mCircleRadius[m08B0];
    mCircleSpeed = l_HIO.mCircleSpeed;
    mCircleCenter.x = m08C4.x;
    mCircleCenter.y = m08C4.y;
    mCircleCenter.z = m08C4.z;
    mCircleAngleSpeed = (s16)((REG12_S(0) + 0x150) * m0920);
    pzDLib_setCirclePath(&mCircleCenter);
    mbHeadOnly = 0;
    m_jnt.mbTrn = 0;
    m_jnt.mbHeadLock = 0;
    m_jnt.mbBackBoneLock = 0;
}

/* 02456D34 */
void daPz_c::modeAttackInit() {

    s8 anm = m06D3;
    mDodgeTimer = 30;
    s32 lvl = m08B0;
    if (m0F65 == 0)
        lvl = 0;
    if (anm != ANM_BOW_READY && anm != ANM_BOW_HOLD) {
        f32 rnd = cM_rndF((f32)l_HIO.mAttackChargeRandom[lvl]);
        mModeTimer = gabi::ftoi((f32)l_HIO.mAttackChargeTime[lvl] + rnd);
        mAttackWaitTimer = l_HIO.mAttackWaitTime[lvl];
        setAnm(ANM_BOW_READY, 0, 0xF);
    }
    if (cM_rndF(100.0f) < l_HIO.mPlayerAimChance[lvl]) {
        mbHeadOnly = 0;
        m_jnt.mbHeadLock = 1;
        m0F48 = 1;
        m_jnt.mbBackBoneLock = 0;
        return;
    }
    m0F48 = 0;
    if (cM_rndF(100.0f) < l_HIO.mStraightAimChance[lvl]) {
        mbHeadOnly = 0;
        m_jnt.mbHeadLock = 1;
        m_jnt.mbBackBoneLock = 0;
        mAttackAimSpread = 0.0f;
    } else {
        mAttackAimSpread = cM_rndF(l_HIO.mAimSpread[lvl]);
        mbHeadOnly = 0;
        m_jnt.mbHeadLock = 1;
        m_jnt.mbBackBoneLock = 0;
    }
}

/* 024574BC */
void daPz_c::modeDefend() {

    m_jnt.mbTrn = 0;
    mTargetSpeed = 0.0f;
    s16 target = cLib_targetAngleY(&current.pos, &mHitPos);
    cLib_addCalcAngleS2(&shape_angle.y, target, 4, 0x1000);
    cLib_distanceAngleS(shape_angle.y, target); /* result unused */
    if (mHitSlideSpeed > 0.01f) {
        /* pushed back along the hit direction */
        cXyz out;
        out.x = 0.0f;
        cXyz in;
        in.x = 0.0f;
        out.y = 0.0f;
        in.z = -mHitSlideSpeed;
        in.y = 0.0f;
        out.z = 0.0f;
        mDoMtx_YrotS(mDoMtx_stack_c::get(), mHitAngle);
        PSMTXMultVec(mDoMtx_stack_c::get(), &in, &out);
        PSVECAdd(&current.pos, &out, &current.pos);
        cLib_addCalc0(&mHitSlideSpeed, 1.0f, 7.0f);
        gabi::Local<be<s32>> land;
        setSimpleLand(this, land);
        checkTgHit();
    } else if (mbTargetPlayer != 0 || mbTargetGanondorf != 0) {
        modeProc(PROC_INIT_e, MODE_ATTACK_WAIT);
        checkTgHit();
    } else {
        if (pzCLib_calcTimer(&mModeTimer) == 0)
            modeProc(PROC_INIT_e, MODE_MOVE);
        checkTgHit();
    }
}

/* 02457D68 */
void daPz_c::modeAfraid() {

    m_jnt.mbTrn = 0;
    speedF = 0.0f;
    m_jnt.mbHeadLock = 0;
    mTargetSpeed = 0.0f;
    m_jnt.mbBackBoneLock = 0;
    mbHeadOnly = 0;
    m073F = 0;
    gabi::Local<be<u32>> gndId;
    if (!pzFopAcM_SearchByName(fpcNm_GND_e, gndId) || *gndId == 0)
        return;
    fopAc_ac_c* gnd = pzGuest<fopAc_ac_c>(*gndId);
    s16 dbg = REG12_S(9);
    if (dbg == 1) {
        m08C4.x = gnd->current.pos.x;
        f32 y = gnd->current.pos.y;
        m08C4.y = y;
        m08C4.z = gnd->current.pos.z;
        m08C4.y = y + (REG12_F(10) + 1200.0f);
    } else if (dbg == 2) {
        m08C4.x = gnd->eyePos.x;
        f32 y = gnd->eyePos.y;
        m08C4.y = y;
        m08C4.z = gnd->eyePos.z;
        m08C4.y = y + (REG12_F(10) + 800.0f);
    } else {
        m08C4.copy(gnd->current.pos);
        cXyz d;
        cXyz_mi(&current.pos, &d, &gnd->current.pos);
        cXyz xz;
        xz.z = d.z;
        xz.x = d.x;
        xz.y = 0.0f;
        f64 dist = pzSqrtf_d(PSVECSquareMag(&xz));
        gnd = pzGuest<fopAc_ac_c>(*gndId);
        f32 y = (f32)(-((dist * 0.5) - (f64)(f32)gnd->eyePos.y));
        y = y * 8.0f;
        f32 lim = gnd->current.pos.y + 50.0f;
        m08C4.y = (y > lim) ? y : lim;
    }
    s16 target = cLib_targetAngleY(&current.pos, &gnd->current.pos);
    cLib_addCalcAngleS2(&shape_angle.y, target, 4, 0x400);
}

/* 02458000 */
void daPz_c::modeSideStep() {

    if (checkTgHit()) {
        mWaistAngleY = 0;
        return;
    }
    mTargetSpeed = 0.0f;
    m_jnt.mbTrn = 1;
    speedF = 0.0f;
    mbHeadOnly = 0;
    m073F = 0;
    cLib_addCalcAngleS2(&mWaistAngleY, (s16)(m0920 * 0x1194), 4, 0x800);
    s16 angle = cLib_targetAngleY(&mEyePos2, &m08C4);
    cXyz out;
    cXyz in;
    s32 dir = m0920;
    f32 step = mStepSpeed;
    out.y = 0.0f;
    out.x = 0.0f;
    out.z = 0.0f;
    in.y = 0.0f;
    in.z = 0.0f;
    in.x = gabi::fmuls_ppc(step, (f32)dir);
    mDoMtx_YrotS(mDoMtx_stack_c::get(), angle);
    PSMTXMultVec(mDoMtx_stack_c::get(), &in, &out);
    PSVECAdd(&current.pos, &out, &current.pos);
    if (mObjAcch.ChkGroundHit()) {
        mWaistAngleY = 0;
        gabi::Local<be<s32>> land;
        setSimpleLand(this, land);
        if (mPreviousMode == 2 || mPreviousMode == 3)
            modeProc(PROC_INIT_e, MODE_ATTACK);
        else
            modeProc(PROC_INIT_e, MODE_MOVE);
    }
}

/* 02458274 */
void daPz_c::modeBackStep() {

    if (checkTgHit()) {
        mWaistAngleZ = 0;
        return;
    }
    m073F = 0;
    m_jnt.mbTrn = 1;
    speedF = 0.0f;
    mbHeadOnly = 0;
    mTargetSpeed = 0.0f;
    cLib_addCalcAngleS2(&mWaistAngleZ, -0x1194, 4, 0x800);
    s16 angle = cLib_targetAngleY(&current.pos, &m08C4);
    cXyz out;
    cXyz in;
    f32 step = mStepSpeed;
    out.x = 0.0f;
    out.y = 0.0f;
    in.x = 0.0f;
    out.z = 0.0f;
    in.y = 0.0f;
    in.z = -step;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), angle);
    PSMTXMultVec(mDoMtx_stack_c::get(), &in, &out);
    PSVECAdd(&current.pos, &out, &current.pos);
    if (mObjAcch.ChkGroundHit()) {
        mWaistAngleZ = 0;
        gabi::Local<be<s32>> land;
        setSimpleLand(this, land);
        if (mPreviousMode == 2 || mPreviousMode == 3)
            modeProc(PROC_INIT_e, MODE_ATTACK);
        else
            modeProc(PROC_INIT_e, MODE_MOVE);
    }
}

static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    pzMonsSeStart(id, &a->eyePos, fopAcM_GetID(a), param, reverb);
}

/* 02455CFC */
BOOL daPz_c::checkTgHit() {

    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    mStts.Move();
    if (pzCLib_calcTimer_u8(&mTgHitTimer) != 0)
        return FALSE;
    if (!mCyl.ChkTgHit())
        return FALSE;
    BOOL damage = TRUE;
    void* hitObj = mCyl.GetTgHitObj();
    mTgHitTimer = (u8)l_HIO.mHitCooldown;
    if (hitObj == nullptr)
        return FALSE;
    u32 atType = reinterpret_cast<dCcD_GObjInf*>(hitObj)->mAtType;
    switch (atType) {
    case AT_TYPE_SWORD:
    case AT_TYPE_MACHETE:
    case AT_TYPE_UNK800:
    case AT_TYPE_DARKNUT_SWORD:
    case AT_TYPE_MOBLIN_SPEAR: {
        /* the player's current action (+0x3AC) decides between a strong and a normal hit */
        u8 proc = reinterpret_cast<PzPlayerHD*>(player)->mAction;
        bool strong;
        if (proc >= 0x15)
            strong = proc == 0x15 || proc == 0x17 || (proc >= 0x19 && proc <= 0x1B);
        else
            strong = (proc >= 5 && proc <= 0xA) || proc == 0xC || (proc >= 0xE && proc <= 0x10);
        mHitType = strong ? 1 : 0;
        break;
    }
    case AT_TYPE_BOMB:
        mHitType = 6;
        break;
    case AT_TYPE_BOOMERANG:
    case AT_TYPE_BOKO_STICK:
        mHitType = 4;
        break;
    case AT_TYPE_NORMAL_ARROW:
    case AT_TYPE_FIRE_ARROW:
    case AT_TYPE_ICE_ARROW:
    case AT_TYPE_LIGHT_ARROW:
        mHitType = 5;
        break;
    case AT_TYPE_HOOKSHOT:
        mHitType = 0xC;
        break;
    case AT_TYPE_SKULL_HAMMER:
    case AT_TYPE_STALFOS_MACE:
        mHitType = 7;
        if (reinterpret_cast<PzPlayerHD*>(player)->mAction == 0x11)
            mHitType = 8;
        break;
    case AT_TYPE_WIND:
        mHitType = 3;
        damage = FALSE;
        break;
    case AT_TYPE_GRAPPLING_HOOK:
        mHitType = 0xE;
        damage = FALSE;
        break;
    }
    fopAc_ac_c* hitAc = mCyl.GetTgHitAc();
    if (hitAc != nullptr && fpcM_GetName(hitAc) == fpcNm_ARROW_e) {
        /* Zelda's own light arrow (reflected?) */
        if (reinterpret_cast<PzArrowHD*>(hitAc)->mbReflected != 0) {
            mHitType = 5;
        } else if (reinterpret_cast<PzArrowHD*>(hitAc)->mbInactive != 0) {
            return FALSE;
        }
    }
    if (!damage) {
        if (mHitType == 0xE) {
            fopAcM_monsSeStart(this, JA_SE_CV_ZL_GN_DAMAGE, 0);
            u16 v = dComIfGs_getLife(); /* save info: player life */
            s32 itemNo = v > 0xC ? 0 : 0x1E;
            pzFopAcM_fastCreateItem(&current.pos, itemNo, current.roomNo, &shape_angle, nullptr,
                                    0.0f, 0.0f, -6.0f, -1,
                                    gabi::guest_fn((const void*)stealItem_CB));
            modeProc(PROC_INIT_e, MODE_DEFEND);
        }
        return TRUE;
    }
    cXyz* hitPos = mCyl.GetTgHitPosP(); /* mCyl.GetTgHitPosP() */
    mHitAngle = fopAcM_searchActorAngleY(this, hitAc);
    mCyl.GetTgHitObj();
    pzDef_se_set(this, mCyl.GetTgHitObj(), 0x41);
    cXyz flush;
    flush.x = hitPos->x;
    flush.y = hitPos->y;
    flush.z = hitPos->z;
    pzDKy_SordFlush_set(&flush, 0);
    if (mHitType == 1 || mHitType == 7 || mHitType == 8)
        mHitSlideSpeed = 60.0f;
    else
        mHitSlideSpeed = 40.0f;
    dPa_control_set(dComIfGp_getParticle(), 0, PZ_PARTICLE_HIT, hitPos, &player->shape_angle,
                    nullptr, 0xFF, nullptr, -1, nullptr, nullptr, nullptr);
    modeProc(PROC_INIT_e, MODE_DEFEND);
    bool se;
    if (hitAc != nullptr && fpcM_GetName(hitAc) == fpcNm_GND_e) {
        mHitSlideSpeed = (f32)((f64)mHitSlideSpeed * 1.5);
        mbTargetPlayer = 0;
        mbLastHitFromPlayer = 0;
        s32 n = mGanondorfHitCount + 1;
        mGanondorfHitCount = n;
        mPlayerHitCount = 0;
        if (n > l_HIO.mGanondorfHitLimit) {
            mGanondorfHitCount = 0;
            mbTargetGanondorf = 1;
            mGanondorfTargetAttackCount = l_HIO.mGanondorfTargetAttacks;
        }
        se = cM_rndF(100.0f) < 60.0f;
    } else {
        s32 n = mPlayerHitCount + 1;
        mbTargetGanondorf = 0;
        mGanondorfHitCount = 0;
        mPlayerHitCount = n;
        mbLastHitFromPlayer = 1;
        if (n > l_HIO.mPlayerHitLimit) {
            mPlayerHitCount = 0;
            mbTargetPlayer = 1;
            mPlayerTargetAttackCount = l_HIO.mPlayerTargetAttacks;
        }
        se = cM_rndF(100.0f) < 30.0f;
    }
    if (se)
        fopAcM_monsSeStart(this, JA_SE_CV_ZL_GN_DAMAGE, 0);
    mHitPos.copy(*hitPos);
    return TRUE;
}

/* cXyz::absXZ of (a - b): GHS keeps the square root unrounded */
/* inlined into daPz_c methods */
static f64 distXZ(cXyz* a, cXyz* b) {
    cXyz d;
    cXyz_mi(a, &d, b);
    cXyz xz;
    xz.x = d.x;
    xz.y = 0.0f;
    xz.z = d.z;
    return pzSqrtf_d(PSVECSquareMag(&xz));
}
/* the player (look position) or Ganondorf came within the dodge distance */
static bool isNearForDodge(daPz_c* i_this) {
    f64 distPlayer = distXZ(&i_this->current.pos, &i_this->m08C4);
    f64 distGnd = 100000.0;
    if (i_this->mbHasGanondorf)
        distGnd = distXZ(&i_this->current.pos, &i_this->mGanondorfPosCurrent);
    if (i_this->mbEyesFollowGanondorf == 0)
        return false;
    f32 lim = l_HIO.mDodgeDistance;
    return distPlayer < lim || distGnd < lim;
}
/* inlined into daPz_c methods */
static void lookAtPlayer(daPz_c* i_this) {
    cXyz eye;
    dNpc_playerEyePos(&eye, l_HIO.mPlayerEyeYOffset);
    i_this->m08C4.copy(eye);
}
/* look at the player when hit too often by others (Ganondorf strong enough, player healthy), at
 * Ganondorf when he hit too often */
static void selectLookTarget(daPz_c* i_this) {
    if (i_this->m08B0 == 0) {
        gabi::Local<be<u32>> gndId;
        if (pzFopAcM_SearchByName(fpcNm_GND_e, gndId) && *gndId != 0 &&
            pzGuest<fopAc_ac_c>(*gndId)->health > 0x3C /* health */ &&
            dComIfGs_getLife() >= 0xC /* player life */ && i_this->mbTargetPlayer != 0)
            lookAtPlayer(i_this);
    } else {
        if (i_this->mbTargetPlayer != 0)
            lookAtPlayer(i_this);
    }
    if (i_this->mbTargetGanondorf != 0 && i_this->mbHasGanondorf)
        i_this->m08C4.copy(i_this->mGanondorfPos4);
}

/* 024569F4 */
void daPz_c::modeAttackWait() {

    if (checkTgHit())
        return;
    bool check;
    if (l_HIO.mbLookAtPlayer == 0) {
        if (mbHasGanondorf)
            m08C4.copy(mGanondorfPos4);
    } else {
        lookAtPlayer(this);
    }
    check = pzCLib_calcTimer(&mAttackWaitTimer) == 0;
    if (check && isNearForDodge(this)) {
        mPreviousMode = mMode;
        if (cM_rndF(100.0f) < 50.0f)
            modeProc(PROC_INIT_e, MODE_BACK_STEP);
        else
            modeProc(PROC_INIT_e, MODE_SIDE_STEP);
        return;
    }
    selectLookTarget(this);
    if (l_HIO.mbEnableAttack != 0 && pzCLib_calcTimer(&mModeTimer) == 0)
        modeProc(PROC_INIT_e, MODE_ATTACK);
    m_jnt.mbTrn = 1;
}

/* 02456ED8 */
void daPz_c::modeAttack() {

    if (checkTgHit())
        return;
    if (m0F48 == 0) {
        if (mbHasGanondorf)
            m08C4.copy(mGanondorfPos4);
    } else {
        lookAtPlayer(this);
    }
    selectLookTarget(this);
    s8 anm = m06D3;
    if (anm == ANM_BOW_READY || anm == ANM_BOW_HOLD) {
        if (pzCLib_calcTimer(&mDodgeTimer) == 0 && isNearForDodge(this)) {
            mPreviousMode = mMode;
            if (cM_rndF(100.0f) < 50.0f)
                modeProc(PROC_INIT_e, MODE_BACK_STEP);
            else
                modeProc(PROC_INIT_e, MODE_SIDE_STEP);
            return;
        }
        anm = m06D3;
        if (anm == ANM_BOW_READY) {
            if (mpMorf->isStop()) {
                m_jnt.mbTrn = 1;
                setAnm(ANM_BOW_HOLD, 0, 0xF);
            }
            return;
        }
        if (anm == ANM_BOW_HOLD) {
            m_jnt.mbTrn = 1;
            if (pzCLib_calcTimer(&mModeTimer) == 0)
                setAnm(ANM_BOW_SHOOT, 0, 0xF);
            return;
        }
    }
    if (anm != ANM_BOW_SHOOT)
        return;
    if (!mpMorf->isStop())
        return;
    m_jnt.mbTrn = 0;
    if (mbTargetPlayer != 0) {
        mPlayerTargetAttackCount -= 1;
        if (pzCLib_calcTimer(&mPlayerTargetAttackCount) == 0) {
            mbTargetPlayer = 0;
            return;
        }
        modeProc(PROC_INIT_e, MODE_ATTACK_WAIT);
        return;
    }
    if (mbTargetGanondorf != 0) {
        mGanondorfTargetAttackCount -= 1;
        if (pzCLib_calcTimer(&mGanondorfTargetAttackCount) == 0) {
            mbTargetGanondorf = 0;
            return;
        }
        modeProc(PROC_INIT_e, MODE_ATTACK_WAIT);
        return;
    }
    if (pzCLib_calcTimer(&mAttackWaitTimer) == 0) {
        modeProc(PROC_INIT_e, MODE_MOVE);
        return;
    }
    mbHeadOnly = 1;
}

/* land effect with a static scale (setSimpleLand(2.5, 3.0, 2.0)) */
static void setDownLand(daPz_c* i_this, cXyz* pos, be<u32>* guard, cXyz* scale) {
    if (guard->get() == 0) {
        *guard = 1;
        scale->y = 0.6f;
        scale->z = 0.6f;
        scale->x = 0.6f;
    }
    gabi::Local<be<s32>> land;
    u32 e = pzSetSimpleLand(dComIfGp_getParticle(),
                            gabi::ea(static_cast<cBgS_PolyInfo*>(&i_this->mObjAcch.m_gnd)), pos,
                            &i_this->shape_angle, &i_this->tevStr, 2.5f, 3.0f, 2.0f, land.get(), 7);
    if (e != 0) {
        pzGuest<PzEmitterHD>(e)->mRate = 18.0f;
        pzGuest<PzEmitterHD>(e)->mSpread = 1.0f;
        f32 sx = scale->x;
        pzGuest<PzEmitterHD>(e)->mGlobalDynamicsScale.x = sx;
        f32 sy = scale->y;
        pzGuest<PzEmitterHD>(e)->mGlobalDynamicsScale.y = sy;
        f32 sz = scale->z;
        pzGuest<PzEmitterHD>(e)->mGlobalDynamicsScale.z = sz;
        pzGuest<PzEmitterHD>(e)->mGlobalParticleScale.x = sx;
        pzGuest<PzEmitterHD>(e)->mGlobalParticleScale.y = sy;
        pzGuest<PzEmitterHD>(e)->mGlobalParticleScale.z = sz;
    }
}

/* 02457718 */
void daPz_c::modeDown() {

    s8 anm = m06D3;
    speedF = 0.0f;
    m_jnt.mbTrn = 0;
    mTargetSpeed = 0.0f;
    if (anm == ANM_HIT && mpMorf->mCurMorf == 1.0f && mObjAcch.ChkGroundHit())
        setAnm(ANM_DOWN, 0, 0xF);
    s32 frame;
    bool down;
    if (pzCLib_calcTimer(&mModeTimer) == 0) {
        frame = gabi::ftoi(mpMorf->mFrameCtrl.mFrame);
        down = m06D3 == ANM_DOWN;
    } else {
        /* sliding back */
        cXyz out;
        out.y = 0.0f;
        out.z = 0.0f;
        cXyz in;
        s16 angle = shape_angle.y;
        in.x = mStepSpeed;
        out.x = 0.0f;
        in.y = 0.0f;
        in.z = 0.0f;
        mDoMtx_YrotS(mDoMtx_stack_c::get(), angle);
        PSMTXMultVec(mDoMtx_stack_c::get(), &in, &out);
        PSVECAdd(&current.pos, &out, &current.pos);
        if (m06D3 == ANM_DOWN && mpMorf->isStop()) {
            gabi::Local<be<s32>> land;
            setSimpleLand(this, land);
        }
        frame = gabi::ftoi(mpMorf->mFrameCtrl.mFrame);
        down = m06D3 == ANM_DOWN;
    }
    if (down) {
        f32 f = (f32)frame;
        f32 reg = REG12_F(10);
        if (f == reg + 1.0f) {
            setDownLand(this, &current.pos, &l_downFootScaleInitialized, &l_downFootScale);
            reg = REG12_F(10);
        }
        if (f == reg + 2.0f) {
            setDownLand(this, &mWaistPos, &l_downWaistScaleInitialized, &l_downWaistScale);
            reg = REG12_F(10);
        }
        if (f == reg + 3.0f)
            setDownLand(this, &mEyePos2, &l_downHeadScaleInitialized, &l_downHeadScale);
        if (m06D3 == ANM_DOWN && mpMorf->isStop())
            mbHeadOnly = 1;
    }
    if (m08B0 == 1)
        return;
    if (m06D3 == ANM_WAKE) {
        if (mpMorf->isStop()) {
            mbHeadOnly = 0;
            setAnm(ANM_READY, 0, 0xF);
        }
        return;
    }
    if (mbWakeUp != 0) {
        mbWakeUp = 0;
        mbTargetPlayer = 0;
        m0F7C = l_HIO.mTalkWaitTime[2];
        m0F80 = 0;
        mbTargetGanondorf = 0;
        modeProc(PROC_INIT_e, MODE_MOVE);
    } else {
        setAnm(ANM_WAKE, 0, 0xF);
    }
}

/* 024563D4 */
void daPz_c::modeMove() {

    /* HIO debug buttons */
    if (l_HIO.mbDebugSideStep != 0) {
        l_HIO.mbDebugSideStep = 0;
        modeProc(PROC_INIT_e, MODE_SIDE_STEP);
        return;
    }
    if (l_HIO.mDebugButtons[0] != 0) {
        l_HIO.mDebugButtons[0] = 0;
        modeProc(PROC_INIT_e, MODE_BACK_STEP);
        return;
    }
    BOOL blocked = FALSE;
    pzLineSet(&mLinChk, &current.pos, &mCirclePos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk))
        blocked = TRUE;
    if (mObjAcch.ChkWallHit())
        blocked = TRUE;
    if (pzCLib_calcTimer(&mWallTurnTimer) == 0 && blocked) {
        mWallTurnTimer = 60;
        s32 dir = m0920 == 1 ? -1 : 1;
        m0920 = dir;
        mCircleAngleSpeed = (s16)((REG12_S(0) + 0x4000) * dir);
        if (pzCLib_calcTimer(&mDodgeTimer) == 0) {
            pzDLib_setCirclePath(&mCircleCenter);
            modeProc(PROC_INIT_e, MODE_SIDE_STEP);
        }
    }
    if (checkTgHit())
        return;
    if (l_HIO.mbLookAtPlayer == 0) {
        if (mbHasGanondorf)
            m08C4.copy(mGanondorfPos4);
    } else {
        lookAtPlayer(this);
    }
    m_jnt.mbTrn = 0;
    cLib_addCalc2(&mCircleRadius, l_HIO.mCircleRadius[m08B0], 0.1f, 10.0f);
    f64 distPath = distXZ(&current.pos, &mCirclePos);
    distXZ(&current.pos, &m08C4); /* result unused */
    f64 distLook = distXZ(&m08C4, &mCirclePos);
    bool setPath;
    if (distPath > REG12_F(7) + 200.0f || mbEyesFollowGanondorf == 0) {
        mTargetSpeed = l_HIO.mMoveSpeed;
    } else {
        mTargetSpeed = 0.0f;
    }
    if (blocked) {
        setPath = true;
    } else if (!(distPath > 200.0f)) {
        setPath = true;
    } else {
        f32 r = l_HIO.mCircleRadius[m08B0];
        setPath = !(distLook < r + r) || mbEyesFollowGanondorf != 0;
    }
    if (setPath) {
        mCircleSpeed = l_HIO.mCircleSpeed;
        mCircleCenter.z = m08C4.z;
        mCircleAngleSpeed = (s16)((REG12_S(0) + 0x150) * m0920);
        mCircleCenter.y = m08C4.y;
        mCircleCenter.x = m08C4.x;
        pzDLib_setCirclePath(&mCircleCenter);
    }
    if (pzCLib_calcTimer(&mAttackWaitTimer) == 0 && isNearForDodge(this)) {
        mPreviousMode = mMode;
        modeProc(PROC_INIT_e, MODE_BACK_STEP);
        return;
    }
    s16 target = cLib_targetAngleY(&current.pos, &mCirclePos);
    cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
    setAnm(speedF < 0.5f ? 2 : 3, 0, 0xF);
    if (m08B0 == 2) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        cXyz pos;
        pos.x = link->current.pos.x;
        pos.y = link->current.pos.y;
        pos.z = link->current.pos.z;
        mbInPlayerFan =
            pzCheckActorInFan(&pos, this, (s16)link->shape_angle.y, 0x2500, 30000.0f, 1000.0f);
    }
    if (l_HIO.mDebugButtons[1] == 0 && pzCLib_calcTimer(&mModeTimer) == 0) {
        if (m08B0 == 0) {
            modeProc(PROC_INIT_e, MODE_ATTACK_WAIT);
        } else if (m08B0 == 2) {
            if (m0F65 == 0) {
                mbInPlayerFan = 1;
                modeProc(PROC_INIT_e, MODE_ATTACK_WAIT);
            } else if (mbInPlayerFan != 0) {
                modeProc(PROC_INIT_e, MODE_ATTACK_WAIT);
            }
        }
    }
    m0F82 = 2;
}

/* 02458550 */
void daPz_c::modeFollow() {

    BOOL blocked = FALSE;
    pzLineSet(&mLinChk, &current.pos, &mFollowPos, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk))
        blocked = TRUE;
    if (mObjAcch.ChkWallHit())
        blocked = TRUE;
    if (checkTgHit())
        return;
    if (l_HIO.mbLookAtPlayer == 0) {
        if (mbHasGanondorf)
            m08C4.copy(mGanondorfPos4);
    } else {
        lookAtPlayer(this);
    }
    m_jnt.mbTrn = 0;
    f64 dist = distXZ(&current.pos, &mFollowPos);
    f32 speed;
    if (mbEyesFollowGanondorf != 0)
        speed = 0.0f;
    else if (blocked)
        speed = 2.0f;
    else if (dist > 200.0f)
        speed = l_HIO.mMoveSpeed;
    else
        speed = 1.0f;
    mTargetSpeed = speed;
    /* follow point: beside Ganondorf, as seen from Link */
    f32 dist2 = l_HIO.mFollowDistance;
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    mFollowPos.copy(mGanondorfPosCurrent);
    s16 angle = cLib_targetAngleY(&link->current.pos, &mGanondorfPosCurrent);
    cLib_distanceAngleS(angle, shape_angle.y); /* result unused */
    mFollowPos.x = gabi::fmadds(dist2, cM_ssin(angle + REG12_S(0) + 0x4000), mFollowPos.x);
    s32 a2 = angle + REG12_S(0) + 0x4000;
    f32 z = mFollowPos.z;
    mFollowPos.y = mFollowPos.y + 200.0f;
    mFollowPos.z = gabi::fmadds(dist2, cM_scos(a2), z);
    if (pzCLib_calcTimer(&mAttackWaitTimer) == 0 && isNearForDodge(this)) {
        mPreviousMode = mMode;
        modeProc(PROC_INIT_e, MODE_BACK_STEP);
        return;
    }
    s16 target = cLib_targetAngleY(&current.pos, &mFollowPos);
    cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
    setAnm(speedF < 0.5f ? 2 : 3, 0, 0xF);
    if (l_HIO.mDebugButtons[1] == 0 && pzCLib_calcTimer(&mModeTimer) == 0)
        modeProc(PROC_INIT_e, MODE_ATTACK_WAIT);
    m0F82 = 2;
}

/* 0245282C */
void daPz_c::modeProc(s32 i_proc, s32 i_mode) {

    if (i_proc == PROC_INIT_e) {
        mMode = i_mode;
        (this->*mode_tbl[i_mode].mInit)();
    } else if (i_proc == PROC_EXEC_e) {
        (this->*mode_tbl[mMode].mExecute)();
    }
}

/* 02455828 */
void daPz_c::modeWaitInit() {

    mTargetSpeed = 0.0f;
    setAnm(ANM_WAIT, 0, 0xF);
    m_jnt.mbTrn = 0;
    m_jnt.mbBackBoneLock = 1;
    mModeTimer = 30;
    m_jnt.mbHeadLock = 1;
    mbHeadOnly = 1;
}

/* 02456990 */
void daPz_c::modeAttackWaitInit() {

    mModeTimer = 20;
    mAttackWaitTimer = 10;
    mTargetSpeed = 0.0f;
    setAnm(ANM_READY, 0, 0xF);
    mbHeadOnly = 0;
    m_jnt.mbHeadLock = 0;
    m_jnt.mbBackBoneLock = 0;
}

/* 02457464 */
void daPz_c::modeDefendInit() {

    setAnm(ANM_DEFEND, 1, 0xF);
    mModeTimer = 30;
    m_jnt.mbTrn = 0;
    m_jnt.mbHeadLock = 1;
    m_jnt.mbBackBoneLock = 1;
    mbHeadOnly = 1;
}

/* 02457664 */
void daPz_c::modeDownInit() {

    be<u32>& attnFlags = attention_info.flags;
    attnFlags &= ~0xAu;
    setAnm(ANM_HIT, 1, 0xF);
    fopAcM_monsSeStart(this, JA_SE_CV_ZL_GN_DAMAGE, 0);
    mbHeadOnly = 0;
    mTargetSpeed = 0.0f;
    m_jnt.mbHeadLock = 1;
    m_jnt.mbTrn = 0;
    speedF = 0.0f;
    m_jnt.mbBackBoneLock = 1;
    speed.y = l_HIO.mDownJumpSpeed;
    mStepSpeed = l_HIO.mDownSlideSpeed;
    mModeTimer = l_HIO.mDownSlideTime;
}

/* 02457D10 */
void daPz_c::modeAfraidInit() {

    setAnm(ANM_AFRAID, 1, 0xF);
    mbHeadOnly = 0;
    speedF = 0.0f;
    m_jnt.mbHeadLock = 0;
    mTargetSpeed = 0.0f;
    m_jnt.mbBackBoneLock = 0;
}

/* 02457F80 */
void daPz_c::modeSideStepInit() {

    if (mPreviousMode != 2 && mPreviousMode != 3)
        setAnm(ANM_AFRAID, 1, 0xF);
    mbHeadOnly = 0;
    speedF = 0.0f;
    mTargetSpeed = 0.0f;
    speed.y = l_HIO.mSideStepJumpSpeed;
    mStepSpeed = l_HIO.mSideStepSpeed;
}

/* 024581F4 */
void daPz_c::modeBackStepInit() {

    if (mPreviousMode != 2 && mPreviousMode != 3)
        setAnm(ANM_AFRAID, 1, 0xF);
    mbHeadOnly = 0;
    speedF = 0.0f;
    mTargetSpeed = 0.0f;
    speed.y = l_HIO.mBackStepJumpSpeed;
    mStepSpeed = l_HIO.mBackStepSpeed;
}

/* 02458438 */
void daPz_c::modeTalk() {

    cXyz eye;
    dNpc_playerEyePos(&eye, l_HIO.mPlayerEyeYOffset);
    m_jnt.mbTrn = 1;
    m08C4.copy(eye);
    if (talk(1) == fopMsgStts_BOX_CLOSED_e) {
        dComIfGp_event_reset();
        modeProc(PROC_INIT_e, MODE_ATTACK_WAIT);
    }
}

/* 024584C0 */
void daPz_c::modeTalkInit() {

    setAnm(ANM_READY, 1, 0xF);
    m_jnt.mbHeadLock = 0;
    mTargetSpeed = 0.0f;
    mbHeadOnly = 0;
    speedF = 0.0f;
    m_jnt.mbBackBoneLock = 0;
    if (m08B0 == 0)
        m0F80 = 1;
    else
        m0F7C = l_HIO.mTalkWaitTime[m08B0];
}

/* 0245897C */
void daPz_c::modeFollowInit() {

    be<u32>& attnFlags = attention_info.flags;
    attnFlags |= 0xA;
    u32 o = 2 * m08B0;
    f32 rnd = cM_rndF((f32)l_HIO.mMoveWaitRandom[o / 2]);
    mModeTimer = gabi::ftoi((f32)l_HIO.mMoveWaitTime[o / 2] + rnd);
    mAttackWaitTimer = 10;
    mDodgeTimer = 120;
    setAnm(ANM_RUN, 0, 0xF);
    m_jnt.mbTrn = 0;
    m_jnt.mbHeadLock = 0;
    mbHeadOnly = 0;
    m_jnt.mbBackBoneLock = 0;
}
// clang-format off
/* WWHD glue
str 10038C34 in 0245588C
str 10038C3C in 0245588C
data mode_tbl 100387F0
str 10038C44 in 0245588C
str 10038C2C in 0245588C
str 10038D00
*/
// clang-format on

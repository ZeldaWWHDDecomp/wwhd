/**
 * d_a_pz_mode.cpp (WWHD): d_a_pz mode (state) functions. 
 * Written from the WWHD code (the GameCube decompilation has only stubs for d_a_pz), verified
 * against cking.rpx.
 */
#include "d/actor/d_a_pz.h"

static inline BOOL fopAcM_SearchByName(s16 name, be<u32>* out) { return gabi::call<BOOL>(0x025D5578, name, out); }
static inline s32 cLib_calcTimer(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
static inline f64 sqrtf_d(f32 x) { return gabi::call<f64>(0x028F4384, x); }
static inline f32 REG12_F(int i) { return REG_F(12, i); }
/* 025A8BFC dPa_control_c::setSimpleLand(poly, pos, angle, tevstr, f32, f32, f32, int* out, int) */
static inline u32 setSimpleLand(daPz_c* a, gabi::Local<be<s32>>& out) {
    return gabi::call<u32>(0x025A8BFC, dComIfGp_getParticle(), gabi::ea(&a->mObjAcch) + 0xD4 + 0x14, &a->current.pos,
                           &a->shape_angle, &a->tevStr, 1.25f, 1.5f, 1.0f, out.get(), 7);
}
/* 02542D88 / 02544830 / 02544044 dEvent_manager_c */
static inline const char* dComIfGp_evmng_getMyNowCutName(s32 staffId) {
    return gabi::call<const char*>(0x02544830, dComIfGp_getPEvtManager(), staffId);
}
static inline s32 strcmp_g(const char* a, u32 b) {
    u32 p = gabi::ea(a);
    for (;; p++, b++) {
        u8 c = gabi::load<u8>(p), d = gabi::load<u8>(b);
        if (c != d || c == 0) return (s32)c - (s32)d;
    }
}
static inline void dLib_setCirclePath(void* path) { gabi::call(0x02587128, path); }

/* 0245588C */
void daPz_c::modeWait() {
    WWHD_FUNC(0x0245588C, void, this);
    gabi::Local<cXyz> eye;
    dNpc_playerEyePos(eye, hio_f32(0x4));
    mLookPos.copy(*eye);
    u32 play = dComIfGp_ea();
    /* HD: getEventIdx's result register is passed on unextended */
    u32 evIdx = gabi::call<u32>(0x02543F10, dComIfGp_getPEvtManager(), STR(0x10038C44) /* "btl_of_swroom" */, 0xFF);
    if (gabi::call<u32>(0x02544044, play + PLAY_EVTMANAGER, evIdx) == 0) { /* getEventData */
        m_jnt.mbTrn = 0;
        m0B16 = 1;
        m_jnt.mbBackBoneLock = 1;
        m_jnt.mbHeadLock = 1;
        return;
    }
    s32 staffId = dComIfGp_evmng_getMyStaffId(STR(0x10038C2C) /* "p_zelda" */, nullptr, 0);
    if (staffId == -1)
        return;
    if (strcmp_g(dComIfGp_evmng_getMyNowCutName(staffId), 0x10038C34 /* "Turn" */) == 0) {
        m_jnt.mbBackBoneLock = 0;
        m0B16 = 0;
        m_jnt.mbHeadLock = 0;
        m_jnt.mbTrn = 1;
        if (cLib_calcTimer(&m0B18) == 0)
            dComIfGp_evmng_cutEnd(staffId);
        return;
    }
    if (strcmp_g(dComIfGp_evmng_getMyNowCutName(staffId), 0x10038C3C /* "Retire" */) == 0) {
        m07F0 = 0;
        s32 frame = REG_S(8, 0) + 4;
        if ((s32)m1197 < frame)
            return;
        m1197 = (u8)frame;
        mEnemyIce.mLightShrinkTimer = 1;
        m07F0 = 1000;
        dComIfGp_evmng_cutEnd(staffId);
        return;
    }
    if (mEnemyIce.mLightShrinkTimer == 0)
        m07F0 = 1000;
    m_jnt.mbTrn = 0;
}
VERIFY(0x0245588C, &daPz_c::modeWait);

/* 02455AB8 */
void daPz_c::modeMoveInit() {
    WWHD_FUNC(0x02455AB8, void, this);
    u32 o = 2 * m0ADC;
    u32 attnFlags = gabi::ea(this) + 0x39C;
    gabi::store<u32>(attnFlags, gabi::load<u32>(attnFlags) | 0xA);
    f32 rnd = cM_rndF((f32)hio_s16(0x8C + o));
    m0B18 = gabi::ftoi((f32)hio_s16(0x86 + o) + rnd);
    m0B1C = 10;
    m0B20 = 120;
    setAnm(3, 0, 0xF);
    if (cM_rndF(100.0f) < hio_f32(0xD8))
        m0B4C = m0B4C == 1 ? -1 : 1;
    if (hio_u8(0x30) == 0) {
        if (mbHasGanondorf == 0)
            goto circle;
        mLookPos.copy(mGanondorfPos4);
        m0B40 = hio_f32(0xA0 + 4 * m0ADC);
        m0B44 = hio_f32(0xAC);
        m0B28.x = mLookPos.x;
        m0B28.y = mLookPos.y;
        m0B28.z = mLookPos.z;
        m0B4A = (s16)((REG12_S(0) + 0x150) * m0B4C);
        dLib_setCirclePath(&m0B28);
        m_jnt.mbTrn = 0;
        m_jnt.mbHeadLock = 0;
        m0B16 = 0;
        m_jnt.mbBackBoneLock = 0;
        return;
    } else {
        gabi::Local<cXyz> eye;
        dNpc_playerEyePos(eye, hio_f32(0x4));
        mLookPos.copy(*eye);
    }
circle:
    /* dLib_circle_path_c around the look position */
    m0B40 = hio_f32(0xA0 + 4 * m0ADC);
    m0B44 = hio_f32(0xAC);
    m0B28.x = mLookPos.x;
    m0B28.y = mLookPos.y;
    m0B28.z = mLookPos.z;
    m0B4A = (s16)((REG12_S(0) + 0x150) * m0B4C);
    dLib_setCirclePath(&m0B28);
    m0B16 = 0;
    m_jnt.mbTrn = 0;
    m_jnt.mbHeadLock = 0;
    m_jnt.mbBackBoneLock = 0;
}
VERIFY(0x02455AB8, &daPz_c::modeMoveInit);

/* 02456D34 */
void daPz_c::modeAttackInit() {
    WWHD_FUNC(0x02456D34, void, this);
    s8 anm = mAnm;
    m0B20 = 30;
    s32 lvl = m0ADC;
    if (m1195 == 0)
        lvl = 0;
    if (anm != 4 && anm != 5) {
        f32 rnd = cM_rndF((f32)hio_s16(0x98 + 2 * lvl));
        m0B18 = gabi::ftoi((f32)hio_s16(0x92 + 2 * lvl) + rnd);
        m0B1C = hio_s16(0x80 + 2 * lvl);
        setAnm(4, 0, 0xF);
    }
    if (cM_rndF(100.0f) < hio_f32(0x68 + 4 * lvl)) {
        m0B16 = 0;
        m_jnt.mbHeadLock = 1;
        m1178 = 1;
        m_jnt.mbBackBoneLock = 0;
        return;
    }
    m1178 = 0;
    if (cM_rndF(100.0f) < hio_f32(0x74 + 4 * lvl)) {
        m0B16 = 0;
        m_jnt.mbHeadLock = 1;
        m_jnt.mbBackBoneLock = 0;
        m1174 = 0.0f;
    } else {
        m1174 = cM_rndF(hio_f32(0x5C + 4 * lvl));
        m0B16 = 0;
        m_jnt.mbHeadLock = 1;
        m_jnt.mbBackBoneLock = 0;
    }
}
VERIFY(0x02456D34, &daPz_c::modeAttackInit);

/* 024574BC */
void daPz_c::modeDefend() {
    WWHD_FUNC(0x024574BC, void, this);
    m_jnt.mbTrn = 0;
    m0B50 = 0.0f;
    s16 target = cLib_targetAngleY(&current.pos, &mHitPos);
    cLib_addCalcAngleS2(&shape_angle.y, target, 4, 0x1000);
    cLib_distanceAngleS(shape_angle.y, target); /* result unused */
    if (m1198 > 0.01f) {
        /* pushed back along the hit direction */
        gabi::Local<cXyz> out;
        out->x = 0.0f;
        gabi::Local<cXyz> in;
        in->x = 0.0f;
        out->y = 0.0f;
        in->z = -m1198;
        in->y = 0.0f;
        out->z = 0.0f;
        mDoMtx_YrotS(mDoMtx_stack_c::get(), m119C);
        PSMTXMultVec(mDoMtx_stack_c::get(), in, out);
        PSVECAdd(&current.pos, out, &current.pos);
        cLib_addCalc0(&m1198, 1.0f, 7.0f);
        gabi::Local<be<s32>> land;
        setSimpleLand(this, land);
        checkTgHit();
    } else if (m0874 != 0 || m0880 != 0) {
        modeProc(PROC_INIT_e, 2);
        checkTgHit();
    } else {
        if (cLib_calcTimer(&m0B18) == 0)
            modeProc(PROC_INIT_e, 1);
        checkTgHit();
    }
}
VERIFY(0x024574BC, &daPz_c::modeDefend);

/* 02457D68 */
void daPz_c::modeAfraid() {
    WWHD_FUNC(0x02457D68, void, this);
    m_jnt.mbTrn = 0;
    speedF = 0.0f;
    m_jnt.mbHeadLock = 0;
    m0B50 = 0.0f;
    m_jnt.mbBackBoneLock = 0;
    m0B16 = 0;
    m0857 = 0;
    gabi::Local<be<u32>> gndId;
    if (!fopAcM_SearchByName(0xF6 /* PROC_GND */, gndId) || *gndId == 0)
        return;
    fopAc_ac_c* gnd = gabi::at<fopAc_ac_c>(*gndId);
    s16 dbg = REG12_S(9);
    if (dbg == 1) {
        mLookPos.x = gnd->current.pos.x;
        f32 y = gnd->current.pos.y;
        mLookPos.y = y;
        mLookPos.z = gnd->current.pos.z;
        mLookPos.y = y + (REG12_F(10) + 1200.0f);
    } else if (dbg == 2) {
        mLookPos.x = gnd->eyePos.x;
        f32 y = gnd->eyePos.y;
        mLookPos.y = y;
        mLookPos.z = gnd->eyePos.z;
        mLookPos.y = y + (REG12_F(10) + 800.0f);
    } else {
        mLookPos.copy(gnd->current.pos);
        gabi::Local<cXyz> d;
        cXyz_mi(&current.pos, d, &gnd->current.pos);
        gabi::Local<cXyz> xz;
        xz->z = d->z;
        xz->x = d->x;
        xz->y = 0.0f;
        f64 dist = sqrtf_d(PSVECSquareMag(xz));
        gnd = gabi::at<fopAc_ac_c>(*gndId);
        f32 y = (f32)(-((dist * 0.5) - (f64)(f32)gnd->eyePos.y));
        y = y * 8.0f;
        f32 lim = gnd->current.pos.y + 50.0f;
        mLookPos.y = (y > lim) ? y : lim;
    }
    s16 target = cLib_targetAngleY(&current.pos, &gnd->current.pos);
    cLib_addCalcAngleS2(&shape_angle.y, target, 4, 0x400);
}
VERIFY(0x02457D68, &daPz_c::modeAfraid);

/* 02458000 */
void daPz_c::modeSideStep() {
    WWHD_FUNC(0x02458000, void, this);
    if (checkTgHit()) {
        mWaistAngleY = 0;
        return;
    }
    m0B50 = 0.0f;
    m_jnt.mbTrn = 1;
    speedF = 0.0f;
    m0B16 = 0;
    m0857 = 0;
    cLib_addCalcAngleS2(&mWaistAngleY, (s16)(m0B4C * 0x1194), 4, 0x800);
    s16 angle = cLib_targetAngleY(&mEyePos2, &mLookPos);
    gabi::Local<cXyz> out;
    gabi::Local<cXyz> in;
    s32 dir = m0B4C;
    f32 step = m11A0;
    out->y = 0.0f;
    out->x = 0.0f;
    out->z = 0.0f;
    in->y = 0.0f;
    in->z = 0.0f;
    in->x = gabi::fmuls_ppc(step, (f32)dir);
    mDoMtx_YrotS(mDoMtx_stack_c::get(), angle);
    PSMTXMultVec(mDoMtx_stack_c::get(), in, out);
    PSVECAdd(&current.pos, out, &current.pos);
    if (mObjAcch.ChkGroundHit()) {
        mWaistAngleY = 0;
        gabi::Local<be<s32>> land;
        setSimpleLand(this, land);
        if (m07E0 == 2 || m07E0 == 3)
            modeProc(PROC_INIT_e, 3);
        else
            modeProc(PROC_INIT_e, 1);
    }
}
VERIFY(0x02458000, &daPz_c::modeSideStep);

/* 02458274 */
void daPz_c::modeBackStep() {
    WWHD_FUNC(0x02458274, void, this);
    if (checkTgHit()) {
        mWaistAngleZ = 0;
        return;
    }
    m0857 = 0;
    m_jnt.mbTrn = 1;
    speedF = 0.0f;
    m0B16 = 0;
    m0B50 = 0.0f;
    cLib_addCalcAngleS2(&mWaistAngleZ, -0x1194, 4, 0x800);
    s16 angle = cLib_targetAngleY(&current.pos, &mLookPos);
    gabi::Local<cXyz> out;
    gabi::Local<cXyz> in;
    f32 step = m11A0;
    out->x = 0.0f;
    out->y = 0.0f;
    in->x = 0.0f;
    out->z = 0.0f;
    in->y = 0.0f;
    in->z = -step;
    mDoMtx_YrotS(mDoMtx_stack_c::get(), angle);
    PSMTXMultVec(mDoMtx_stack_c::get(), in, out);
    PSVECAdd(&current.pos, out, &current.pos);
    if (mObjAcch.ChkGroundHit()) {
        mWaistAngleZ = 0;
        gabi::Local<be<s32>> land;
        setSimpleLand(this, land);
        if (m07E0 == 2 || m07E0 == 3)
            modeProc(PROC_INIT_e, 3);
        else
            modeProc(PROC_INIT_e, 1);
    }
}
VERIFY(0x02458274, &daPz_c::modeBackStep);

static inline s32 cLib_calcTimer_u8(be<u8>* t) { return gabi::call<s32>(0x0207A9A0, t); }
static inline void def_se_set(fopAc_ac_c* a, void* obj, u32 mtrl) { gabi::call(0x02518CC8, a, obj, mtrl); }
static inline void dKy_SordFlush_set(cXyz* pos, s32 p) { gabi::call(0x0255F554, pos, p); }
static inline void fopAcM_monsSeStart(fopAc_ac_c* a, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    gabi::call(0x025E1AA4, id, &a->eyePos, fopAcM_GetID(a), param, reverb);
}
static inline fopAc_ac_c* fopAcM_fastCreateItem(cXyz* pos, s32 itemNo, s32 roomNo, csXyz* rot, cXyz* scale, f32 speedF, f32 speedY,
                                                f32 gravity, s32 bitNo, u32 createFunc) {
    return gabi::call<fopAc_ac_c*>(0x025D8AB0, pos, itemNo, roomNo, rot, scale, speedF, speedY, gravity, bitNo, createFunc);
}

/* 02455CFC */
BOOL daPz_c::checkTgHit() {
    WWHD_FUNC(0x02455CFC, BOOL, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    mStts.Move();
    if (cLib_calcTimer_u8(&mTgHitTimer) != 0)
        return FALSE;
    if (!mCyl.ChkTgHit())
        return FALSE;
    BOOL damage = TRUE;
    void* hitObj = mCyl.GetTgHitObj();
    mTgHitTimer = (u8)hio_s16(0xE0);
    if (hitObj == nullptr)
        return FALSE;
    u32 atType = gabi::load<u32>(gabi::ea(hitObj) + 0x10);
    switch (atType) {
    case 0x2:        /* AT_TYPE_SWORD */
    case 0x400:
    case 0x800:
    case 0x4000000:
    case 0x10000000: {
        /* the player's current action (+0x3AC) decides between a strong and a normal hit */
        u8 proc = gabi::load<u8>(gabi::ea(player) + 0x3AC);
        bool strong;
        if (proc >= 0x15)
            strong = proc == 0x15 || proc == 0x17 || (proc >= 0x19 && proc <= 0x1B);
        else
            strong = (proc >= 5 && proc <= 0xA) || proc == 0xC || (proc >= 0xE && proc <= 0x10);
        mHitType = strong ? 1 : 0;
        break;
    }
    case 0x20:
        mHitType = 6;
        break;
    case 0x40:
    case 0x80:
        mHitType = 4;
        break;
    case 0x4000:
    case 0x40000:
    case 0x80000:
    case 0x100000:
        mHitType = 5;
        break;
    case 0x8000:
        mHitType = 0xC;
        break;
    case 0x10000:
    case 0x1000000:
        mHitType = 7;
        if (gabi::load<u8>(gabi::ea(player) + 0x3AC) == 0x11)
            mHitType = 8;
        break;
    case 0x200000:
        mHitType = 3;
        damage = FALSE;
        break;
    case 0x8000000:
        mHitType = 0xE;
        damage = FALSE;
        break;
    }
    fopAc_ac_c* hitAc = mCyl.GetTgHitAc();
    if (hitAc != nullptr && fpcM_GetName(hitAc) == 0x1D8) {
        /* Zelda's own light arrow (reflected?) */
        if (gabi::load<u8>(gabi::ea(hitAc) + 0x780) != 0) {
            mHitType = 5;
        } else if (gabi::load<u8>(gabi::ea(hitAc) + 0x3AC) != 0) {
            return FALSE;
        }
    }
    if (!damage) {
        if (mHitType == 0xE) {
            fopAcM_monsSeStart(this, 0x4960, 0);
            u16 v = gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x22); /* save info: player life */
            s32 itemNo = v > 0xC ? 0 : 0x1E;
            fopAcM_fastCreateItem(&current.pos, itemNo, current.roomNo, &shape_angle, nullptr, 0.0f, 0.0f, -6.0f, -1,
                                  0x02450D80 /* stealItem_CB */);
            modeProc(PROC_INIT_e, 4);
        }
        return TRUE;
    }
    cXyz* hitPos = gabi::at<cXyz>(gabi::ea(&mCyl) + 0xCC); /* mCyl.GetTgHitPosP() */
    m119C = fopAcM_searchActorAngleY(this, hitAc);
    mCyl.GetTgHitObj();
    def_se_set(this, mCyl.GetTgHitObj(), 0x41);
    gabi::Local<cXyz> flush;
    flush->x = hitPos->x;
    flush->y = hitPos->y;
    flush->z = hitPos->z;
    dKy_SordFlush_set(flush, 0);
    if (mHitType == 1 || mHitType == 7 || mHitType == 8)
        m1198 = 60.0f;
    else
        m1198 = 40.0f;
    dPa_control_set(dComIfGp_getParticle(), 0, 0xC, hitPos, &player->shape_angle, nullptr, 0xFF, nullptr, -1, nullptr, nullptr,
                    nullptr);
    modeProc(PROC_INIT_e, 4);
    bool se;
    if (hitAc != nullptr && fpcM_GetName(hitAc) == 0xF6 /* PROC_GND */) {
        m1198 = (f32)((f64)m1198 * 1.5);
        m0874 = 0;
        m085C = 0;
        s32 n = m0878 + 1;
        m0878 = n;
        m086C = 0;
        if (n > hio_s16(0xE2)) {
            m0878 = 0;
            m0880 = 1;
            m087C = hio_s16(0xE4);
        }
        se = cM_rndF(100.0f) < 60.0f;
    } else {
        s32 n = m086C + 1;
        m0880 = 0;
        m0878 = 0;
        m086C = n;
        m085C = 1;
        if (n > hio_s16(0xE6)) {
            m086C = 0;
            m0874 = 1;
            m0870 = hio_s16(0xE8);
        }
        se = cM_rndF(100.0f) < 30.0f;
    }
    if (se)
        fopAcM_monsSeStart(this, 0x4960, 0);
    mHitPos.copy(*hitPos);
    return TRUE;
}
VERIFY(0x02455CFC, &daPz_c::checkTgHit);

/* cXyz::absXZ of (a - b): GHS keeps the square root unrounded */
static f64 distXZ(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, d, b);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->y = 0.0f;
    xz->z = d->z;
    return sqrtf_d(PSVECSquareMag(xz));
}
/* the player (look position) or Ganondorf came within the dodge distance */
static bool isNearForDodge(daPz_c* i_this) {
    f64 distPlayer = distXZ(&i_this->current.pos, &i_this->mLookPos);
    f64 distGnd = 100000.0;
    if (i_this->mbHasGanondorf)
        distGnd = distXZ(&i_this->current.pos, &i_this->mGanondorfPosCurrent);
    if (i_this->m0856 == 0)
        return false;
    f32 lim = hio_f32(0xD0);
    return distPlayer < lim || distGnd < lim;
}
static void lookAtPlayer(daPz_c* i_this) {
    gabi::Local<cXyz> eye;
    dNpc_playerEyePos(eye, hio_f32(0x4));
    i_this->mLookPos.copy(*eye);
}
/* look at the player when hit too often by others (Ganondorf strong enough, player healthy), at
 * Ganondorf when he hit too often */
static void selectLookTarget(daPz_c* i_this) {
    if (i_this->m0ADC == 0) {
        gabi::Local<be<u32>> gndId;
        if (fopAcM_SearchByName(0xF6 /* PROC_GND */, gndId) && *gndId != 0 &&
            gabi::load<s8>(*gndId + 0x3A1) > 0x3C /* health */ &&
            gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x22) >= 0xC /* player life */ && i_this->m0874 != 0)
            lookAtPlayer(i_this);
    } else {
        if (i_this->m0874 != 0)
            lookAtPlayer(i_this);
    }
    if (i_this->m0880 != 0 && i_this->mbHasGanondorf)
        i_this->mLookPos.copy(i_this->mGanondorfPos4);
}

/* 024569F4 */
void daPz_c::modeAttackWait() {
    WWHD_FUNC(0x024569F4, void, this);
    if (checkTgHit())
        return;
    bool check;
    if (hio_u8(0x30) == 0) {
        if (mbHasGanondorf)
            mLookPos.copy(mGanondorfPos4);
    } else {
        lookAtPlayer(this);
    }
    check = cLib_calcTimer(&m0B1C) == 0;
    if (check && isNearForDodge(this)) {
        m07E0 = mMode;
        if (cM_rndF(100.0f) < 50.0f)
            modeProc(PROC_INIT_e, 8);
        else
            modeProc(PROC_INIT_e, 7);
        return;
    }
    selectLookTarget(this);
    if (hio_u8(0x2D) != 0 && cLib_calcTimer(&m0B18) == 0)
        modeProc(PROC_INIT_e, 3);
    m_jnt.mbTrn = 1;
}
VERIFY(0x024569F4, &daPz_c::modeAttackWait);

/* 02456ED8 */
void daPz_c::modeAttack() {
    WWHD_FUNC(0x02456ED8, void, this);
    if (checkTgHit())
        return;
    if (m1178 == 0) {
        if (mbHasGanondorf)
            mLookPos.copy(mGanondorfPos4);
    } else {
        lookAtPlayer(this);
    }
    selectLookTarget(this);
    s8 anm = mAnm;
    if (anm == 4 || anm == 5) {
        if (cLib_calcTimer(&m0B20) == 0 && isNearForDodge(this)) {
            m07E0 = mMode;
            if (cM_rndF(100.0f) < 50.0f)
                modeProc(PROC_INIT_e, 8);
            else
                modeProc(PROC_INIT_e, 7);
            return;
        }
        anm = mAnm;
        if (anm == 4) {
            if (mpMorf->isStop()) {
                m_jnt.mbTrn = 1;
                setAnm(5, 0, 0xF);
            }
            return;
        }
        if (anm == 5) {
            m_jnt.mbTrn = 1;
            if (cLib_calcTimer(&m0B18) == 0)
                setAnm(6, 0, 0xF);
            return;
        }
    }
    if (anm != 6)
        return;
    if (!mpMorf->isStop())
        return;
    m_jnt.mbTrn = 0;
    if (m0874 != 0) {
        m0870 -= 1;
        if (cLib_calcTimer(&m0870) == 0) {
            m0874 = 0;
            return;
        }
        modeProc(PROC_INIT_e, 2);
        return;
    }
    if (m0880 != 0) {
        m087C -= 1;
        if (cLib_calcTimer(&m087C) == 0) {
            m0880 = 0;
            return;
        }
        modeProc(PROC_INIT_e, 2);
        return;
    }
    if (cLib_calcTimer(&m0B1C) == 0) {
        modeProc(PROC_INIT_e, 1);
        return;
    }
    m0B16 = 1;
}
VERIFY(0x02456ED8, &daPz_c::modeAttack);

/* land effect with a static scale (setSimpleLand(2.5, 3.0, 2.0)) */
static void setDownLand(daPz_c* i_this, cXyz* pos, u32 guard, u32 sc) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::store<f32>(sc + 4, 0.6f);
        gabi::store<f32>(sc + 8, 0.6f);
        gabi::store<f32>(sc + 0, 0.6f);
    }
    gabi::Local<be<s32>> land;
    u32 e = gabi::call<u32>(0x025A8BFC, dComIfGp_getParticle(), gabi::ea(&i_this->mObjAcch) + 0xD4 + 0x14, pos,
                            &i_this->shape_angle, &i_this->tevStr, 2.5f, 3.0f, 2.0f, land.get(), 7);
    if (e != 0) {
        gabi::store<f32>(e + 0x34, 18.0f);
        gabi::store<f32>(e + 0x58, 1.0f);
        f32 sx = gabi::load<f32>(sc + 0);
        gabi::store<f32>(e + 0x220, sx);
        f32 sy = gabi::load<f32>(sc + 4);
        gabi::store<f32>(e + 0x224, sy);
        f32 sz = gabi::load<f32>(sc + 8);
        gabi::store<f32>(e + 0x228, sz);
        gabi::store<f32>(e + 0x238, sx);
        gabi::store<f32>(e + 0x23C, sy);
        gabi::store<f32>(e + 0x240, sz);
    }
}

/* 02457718 */
void daPz_c::modeDown() {
    WWHD_FUNC(0x02457718, void, this);
    s8 anm = mAnm;
    speedF = 0.0f;
    m_jnt.mbTrn = 0;
    m0B50 = 0.0f;
    if (anm == 7 && gabi::load<f32>(gabi::ea(mpMorf.get()) + 0xB0) == 1.0f && mObjAcch.ChkGroundHit())
        setAnm(8, 0, 0xF);
    s32 frame;
    bool down;
    if (cLib_calcTimer(&m0B18) == 0) {
        frame = gabi::ftoi(mpMorf->mFrameCtrl.mFrame);
        down = mAnm == 8;
    } else {
        /* sliding back */
        gabi::Local<cXyz> out;
        out->y = 0.0f;
        out->z = 0.0f;
        gabi::Local<cXyz> in;
        s16 angle = shape_angle.y;
        in->x = m11A0;
        out->x = 0.0f;
        in->y = 0.0f;
        in->z = 0.0f;
        mDoMtx_YrotS(mDoMtx_stack_c::get(), angle);
        PSMTXMultVec(mDoMtx_stack_c::get(), in, out);
        PSVECAdd(&current.pos, out, &current.pos);
        if (mAnm == 8 && mpMorf->isStop()) {
            gabi::Local<be<s32>> land;
            setSimpleLand(this, land);
        }
        frame = gabi::ftoi(mpMorf->mFrameCtrl.mFrame);
        down = mAnm == 8;
    }
    if (down) {
        f32 f = (f32)frame;
        f32 reg = REG12_F(10);
        if (f == reg + 1.0f) {
            setDownLand(this, &current.pos, 0x1046D604, 0x1046D610);
            reg = REG12_F(10);
        }
        if (f == reg + 2.0f) {
            setDownLand(this, &mWaistPos, 0x1046D608, 0x1046D61C);
            reg = REG12_F(10);
        }
        if (f == reg + 3.0f)
            setDownLand(this, &mEyePos2, 0x1046D60C, 0x1046D628);
        if (mAnm == 8 && mpMorf->isStop())
            m0B16 = 1;
    }
    if (m0ADC == 1)
        return;
    if (mAnm == 9) {
        if (mpMorf->isStop()) {
            m0B16 = 0;
            setAnm(2, 0, 0xF);
        }
        return;
    }
    if (m0858 != 0) {
        m0858 = 0;
        m0874 = 0;
        m11AC = hio_s16(0xFC);
        m11B0 = 0;
        m0880 = 0;
        modeProc(PROC_INIT_e, 1);
    } else {
        setAnm(9, 0, 0xF);
    }
}
VERIFY(0x02457718, &daPz_c::modeDown);

static inline void dBgS_LinChk_Set_l(dBgS_LinChk* chk, cXyz* start, cXyz* end, fopAc_ac_c* actor) {
    gabi::call(0x024F1AFC, chk, start, end, actor);
}

/* 024563D4 */
void daPz_c::modeMove() {
    WWHD_FUNC(0x024563D4, void, this);
    /* HIO debug buttons */
    if (hio_u8(0x32) != 0) {
        gabi::store<u8>(L_HIO + 0x32, 0);
        modeProc(PROC_INIT_e, 7);
        return;
    }
    if (hio_u8(0x34) != 0) {
        gabi::store<u8>(L_HIO + 0x34, 0);
        modeProc(PROC_INIT_e, 8);
        return;
    }
    BOOL blocked = FALSE;
    dBgS_LinChk_Set_l(&mLinChk, &current.pos, &m0B34, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk))
        blocked = TRUE;
    if (mObjAcch.ChkWallHit())
        blocked = TRUE;
    if (cLib_calcTimer(&m0B24) == 0 && blocked) {
        m0B24 = 60;
        s32 dir = m0B4C == 1 ? -1 : 1;
        m0B4C = dir;
        m0B4A = (s16)((REG12_S(0) + 0x4000) * dir);
        if (cLib_calcTimer(&m0B20) == 0) {
            dLib_setCirclePath(&m0B28);
            modeProc(PROC_INIT_e, 7);
        }
    }
    if (checkTgHit())
        return;
    if (hio_u8(0x30) == 0) {
        if (mbHasGanondorf)
            mLookPos.copy(mGanondorfPos4);
    } else {
        lookAtPlayer(this);
    }
    m_jnt.mbTrn = 0;
    cLib_addCalc2(&m0B40, hio_f32(0xA0 + 4 * m0ADC), 0.1f, 10.0f);
    f64 distPath = distXZ(&current.pos, &m0B34);
    distXZ(&current.pos, &mLookPos); /* result unused */
    f64 distLook = distXZ(&mLookPos, &m0B34);
    bool setPath;
    if (distPath > REG12_F(7) + 200.0f || m0856 == 0) {
        m0B50 = hio_f32(0x50);
    } else {
        m0B50 = 0.0f;
    }
    if (blocked) {
        setPath = true;
    } else if (!(distPath > 200.0f)) {
        setPath = true;
    } else {
        f32 r = hio_f32(0xA0 + 4 * m0ADC);
        setPath = !(distLook < r + r) || m0856 != 0;
    }
    if (setPath) {
        m0B44 = hio_f32(0xAC);
        m0B28.z = mLookPos.z;
        m0B4A = (s16)((REG12_S(0) + 0x150) * m0B4C);
        m0B28.y = mLookPos.y;
        m0B28.x = mLookPos.x;
        dLib_setCirclePath(&m0B28);
    }
    if (cLib_calcTimer(&m0B1C) == 0 && isNearForDodge(this)) {
        m07E0 = mMode;
        modeProc(PROC_INIT_e, 8);
        return;
    }
    s16 target = cLib_targetAngleY(&current.pos, &m0B34);
    cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
    setAnm(speedF < 0.5f ? 2 : 3, 0, 0xF);
    if (m0ADC == 2) {
        fopAc_ac_c* link = dComIfGp_getLinkPlayer();
        gabi::Local<cXyz> pos;
        pos->x = link->current.pos.x;
        pos->y = link->current.pos.y;
        pos->z = link->current.pos.z;
        m11B1 = gabi::call<u8>(0x02588230 /* dLib_checkActorInFan */, pos.get(), this, (s16)link->shape_angle.y, 0x2500,
                               30000.0f, 1000.0f);
    }
    if (hio_u8(0x35) == 0 && cLib_calcTimer(&m0B18) == 0) {
        if (m0ADC == 0) {
            modeProc(PROC_INIT_e, 2);
        } else if (m0ADC == 2) {
            if (m1195 == 0) {
                m11B1 = 1;
                modeProc(PROC_INIT_e, 2);
            } else if (m11B1 != 0) {
                modeProc(PROC_INIT_e, 2);
            }
        }
    }
    mOrderState = 2;
}
VERIFY(0x024563D4, &daPz_c::modeMove);

/* 02458550 */
void daPz_c::modeFollow() {
    WWHD_FUNC(0x02458550, void, this);
    BOOL blocked = FALSE;
    dBgS_LinChk_Set_l(&mLinChk, &current.pos, &m0A5C, this);
    if (cBgS_LineCross(dComIfG_Bgsp(), &mLinChk))
        blocked = TRUE;
    if (mObjAcch.ChkWallHit())
        blocked = TRUE;
    if (checkTgHit())
        return;
    if (hio_u8(0x30) == 0) {
        if (mbHasGanondorf)
            mLookPos.copy(mGanondorfPos4);
    } else {
        lookAtPlayer(this);
    }
    m_jnt.mbTrn = 0;
    f64 dist = distXZ(&current.pos, &m0A5C);
    f32 speed;
    if (m0856 != 0)
        speed = 0.0f;
    else if (blocked)
        speed = 2.0f;
    else if (dist > 200.0f)
        speed = hio_f32(0x50);
    else
        speed = 1.0f;
    m0B50 = speed;
    /* follow point: beside Ganondorf, as seen from Link */
    f32 dist2 = hio_f32(0xEC);
    fopAc_ac_c* link = dComIfGp_getLinkPlayer();
    m0A5C.copy(mGanondorfPosCurrent);
    s16 angle = cLib_targetAngleY(&link->current.pos, &mGanondorfPosCurrent);
    cLib_distanceAngleS(angle, shape_angle.y); /* result unused */
    m0A5C.x = gabi::fmadds(dist2, cM_ssin(angle + REG12_S(0) + 0x4000), m0A5C.x);
    s32 a2 = angle + REG12_S(0) + 0x4000;
    f32 z = m0A5C.z;
    m0A5C.y = m0A5C.y + 200.0f;
    m0A5C.z = gabi::fmadds(dist2, cM_scos(a2), z);
    if (cLib_calcTimer(&m0B1C) == 0 && isNearForDodge(this)) {
        m07E0 = mMode;
        modeProc(PROC_INIT_e, 8);
        return;
    }
    s16 target = cLib_targetAngleY(&current.pos, &m0A5C);
    cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
    setAnm(speedF < 0.5f ? 2 : 3, 0, 0xF);
    if (hio_u8(0x35) == 0 && cLib_calcTimer(&m0B18) == 0)
        modeProc(PROC_INIT_e, 2);
    mOrderState = 2;
}
VERIFY(0x02458550, &daPz_c::modeFollow);

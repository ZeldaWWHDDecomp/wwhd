/**
 * d_a_npc_so_cut.cpp (WWHD)
 * NPC - Fishman: event cuts and the mini-game camera
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_so.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_so.h"

/* ---- this TU's statics (HD addresses) ---- */
#define SO_REG 0x1047B608 /* g_regHIO (HD debug registers) */
static inline f32 so_REG_F(u32 off) { return gabi::load<f32>(SO_REG + off); }
static inline s16 so_REG_S(u32 off) { return gabi::load<s16>(SO_REG + off); }
/* l_HIO fields read by the cuts (absolute addresses; GameCube names) */
#define SO_HIO_m68 0x10468734 /* s16 jump angle x */

/* ---- calls into the other parts of the unit (by address) ---- */
static inline void so_offsetZero(daNpc_So_c* t) { gabi::call(0x022DE6AC, t); }
static inline void so_offsetDive(daNpc_So_c* t) { gabi::call(0x022E005C, t); }
static inline void so_offsetSwim(daNpc_So_c* t) { gabi::call(0x022E0AB4, t); }
static inline void so_offsetAppear(daNpc_So_c* t) { gabi::call(0x022E0020, t); }
static inline void so_setAnm(daNpc_So_c* t, s8 idx, bool b) { gabi::call(0x022DE6D4, t, idx, b); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* dLib_getWaterY(pos, acch) */
static inline f32 so_getWaterY(cXyz* pos, dBgS_ObjAcch* acch) { return gabi::call<f32>(0x025871F8, pos, acch); }
/* cLib_calcTimer<int> (out of line; the whole r3 is tested) */
static inline s32 so_calcTimerI(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); }
/* fopAcM_seStart(this, id, 0): HD inline without the NULL checks */
static inline void so_seStart(fopAc_ac_c* a, u32 id) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    mDoAud_seStart(id, &a->eyePos, 0, reverb);
}
/* (a - b).absXZ(): cXyz::operator- into a temporary, then sqrtf(PSVECSquareMag(x, 0, z));
 * the f1 result is used unrounded */
static inline f64 so_absXZ(cXyz* a, cXyz* b) {
    gabi::Local<cXyz> d;
    gabi::call(0x0201ADE0, a, d.get(), b);
    gabi::Local<cXyz> v;
    v->y = 0.0f;
    v->x = d->x;
    v->z = d->z;
    f64 sq = gabi::call<f64>(0x028E8DD0, v.get());
    return gabi::call<f64>(0x028F4384, sq);
}
static inline void* so_getMyP(s32 staffId, u32 name, s32 type) { return dComIfGp_evmng_getMySubstanceP(staffId, STR(name), type); }
/* fopAcM_Search(searchEsa_CB, this) */
static inline fopAc_ac_c* so_searchEsa(daNpc_So_c* t) { return fopAcIt_Judge(0x022DDCA0, t); }
/* inline strcmp (GHS) */
static inline int so_strcmp(u32 a, u32 b) {
    for (;;) {
        u8 x = gabi::load<u8>(a++);
        u8 y = gabi::load<u8>(b++);
        if (x != y || x == 0) return (int)x - (int)y;
    }
}
static inline daNpc_So_c* so_ret(u32 r3) { return gabi::at<daNpc_So_c>(r3); }
/* dLib_circle_path_c (0x24), as used by the cuts (GameCube names) */
struct dLib_circle_path_l {
    /* 0x00 */ cXyz mTranslation;
    /* 0x0C */ cXyz mPos;
    /* 0x18 */ be<f32> mRadius;
    /* 0x1C */ u8 _1C[4];
    /* 0x20 */ be<s16> mAngle;
    /* 0x22 */ be<s16> mAngleSpeed;
};
WWHD_SIZE(dLib_circle_path_l, 0x24);
static inline dLib_circle_path_l* so_path(daNpc_So_c* t) { return gabi::at<dLib_circle_path_l>(gabi::ea(t->mB10)); }
/* the ship / Link actors (play + 0x5B3C / 0x5B34) */
static inline fopAc_ac_c* so_getShip() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B3C)); }
static inline fopAc_ac_c* so_getLink() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B34)); }
/* esa_class field_0x298 (HD +0x3B4) */
#define SO_ESA_EAT 0x3B4
/* mA78 (equip flag): HD +0xBF1 (the header names that byte mA79) */
#define SO_EQUIP 0xBF1
#define SO_L_HIO 0x104686CC /* daNpc_So_HIO_c l_HIO (GameCube field offsets) */
static inline f32 so_hioF(u32 off) { return gabi::load<f32>(SO_L_HIO + off); }
/* dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)): two calls to the play getter */
static inline u32 so_getCamera() {
    s32 id = gabi::load<s8>(dComIfGp_ea() + 0x5B30);
    return gabi::load<u32>(dComIfGp_ea() + id * 0x34 + 0x5AF8);
}
/* dCamera_c (camera_process_class + 0x248) */
static inline void so_camReset(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x0251510C, cam + 0x248, center, eye); }
static inline void so_camStart(u32 cam) { gabi::call(0x02514F38, cam + 0x248); }
static inline void so_camStop(u32 cam) { gabi::call(0x02514F2C, cam + 0x248); }
static inline void so_camSetTrimSize(u32 cam, s32 n) { gabi::call(0x02515280, cam + 0x248, n); }
static inline void so_camSet(u32 cam, cXyz* center, cXyz* eye) { gabi::call(0x02514F50, cam + 0x248, center, eye); }
/* daPy_py_c demo (HD: mDemo at +0x420: type, param0 +0x428, mode +0x430) */
static inline void so_changeOriginalDemo(fopAc_ac_c* pl) {
    gabi::store<u16>(gabi::ea(pl) + 0x420, 3);
    gabi::store<u32>(gabi::ea(pl) + 0x428, 0);
}
static inline void so_changeDemoMode(fopAc_ac_c* pl, u32 mode) { gabi::store<u32>(gabi::ea(pl) + 0x430, mode); }
static inline void so_changeDemoParam0(fopAc_ac_c* pl, s32 v) { gabi::store<u32>(gabi::ea(pl) + 0x428, v); }
/* p + r * angle-vector (fmadds, as cM_ssin/cM_scos are inlined) */
static inline void so_offsetXZ(cXyz* out, cXyz* base, u16 ang, f32 r) {
    out->x = gabi::fmadds(r, cM_ssin(ang), base->x);
    out->z = gabi::fmadds(r, cM_scos(ang), base->z);
}

/* 022E1E74 */
void daNpc_So_c::cutSwimProc() {
    WWHD_FUNC(0x022E1E74, void, this);
    gabi::Local<cXyz> pos;
    pos->x = mEventCut.mPos.x; /* mEventCut.getAttnPos() */
    pos->y = mEventCut.mPos.y;
    pos->z = mEventCut.mPos.z;
    mAFC = gabi::fadds_ppc(so_REG_F(0x6F0), 7.0f); /* REG12_F(10) + 7.0f */
    f64 abs = so_absXZ(&current.pos, pos);
    s16 target = cLib_targetAngleY(&current.pos, pos);
    cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
    if (abs < gabi::fadds_ppc(so_REG_F(0x6E4), 150.0f)) { /* REG12_F(7) + 150.0f */
        mAFC = 0.0f;
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E1E74, &daNpc_So_c::cutSwimProc);

/* 022E1F90 */
void daNpc_So_c::cutJumpStart() {
    WWHD_FUNC(0x022E1F90, void, this);
    mA9C = 0xF;
    current.pos.y = so_getWaterY(&current.pos, &mAcch);
    be<f32>* pSpeedY = (be<f32>*)so_getMyP(mB6C, 0x100222B8 /* "Speed_y" */, 0);
    be<f32>* pSpeedF = (be<f32>*)so_getMyP(mB6C, 0x100222C0 /* "SpeedF" */, 0);
    shape_angle.x = gabi::load<s16>(SO_HIO_m68);
    if (pSpeedY != nullptr) {
        speed.y = *pSpeedY;
    } else {
        speed.y = 40.0f;
    }
    if (pSpeedF != nullptr) {
        mAFC = *pSpeedF;
    } else {
        mAFC = 5.0f;
    }
    so_offsetSwim(this);
    speedF = mAFC;
    so_setAnm(this, 4, false);
    so_seStart(this, 0x5938 /* JA_SE_CM_SO_JUMP_L */);
}
VERIFY(0x022E1F90, &daNpc_So_c::cutJumpStart);

/* 022E20EC */
void daNpc_So_c::cutJumpProc() {
    WWHD_FUNC(0x022E20EC, void, this);
    f32 waterHeight = so_getWaterY(&current.pos, &mAcch);
    if (so_calcTimerI(&mA9C) == 0 && !(current.pos.y > waterHeight)) {
        so_seStart(this, 0x5939 /* JA_SE_CM_SO_LANDING_L */);
        fopKyM_createWpillar(&current.pos, gabi::fmuls_ppc(scale.x, 1.4f), 1.4f, 0);
        mAFC = 0.0f;
        speedF = 0.0f;
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E20EC, &daNpc_So_c::cutJumpProc);

/* 022E21C0 */
void daNpc_So_c::cutAppearStart() {
    WWHD_FUNC(0x022E21C0, void, this);
    so_seStart(this, 0x593A /* JA_SE_CM_SO_RISE */);
    so_offsetAppear(this);
}
VERIFY(0x022E21C0, &daNpc_So_c::cutAppearStart);

/* 022E2210 (matcher: unnamed) */
void daNpc_So_c::cutAppearProc() {
    WWHD_FUNC(0x022E2210, void, this);
    if (std::fabs(gabi::fsubs_ppc(mB34, mB38.x)) < 10.0f) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E2210, &daNpc_So_c::cutAppearProc);

/* 022E2268 (matcher: unnamed) */
void daNpc_So_c::cutDiveStart() {
    WWHD_FUNC(0x022E2268, void, this);
    so_offsetDive(this);
}
VERIFY(0x022E2268, &daNpc_So_c::cutDiveStart);

/* 022E226C (matcher: unnamed) */
void daNpc_So_c::cutDiveProc() {
    WWHD_FUNC(0x022E226C, void, this);
    if (std::fabs(gabi::fsubs_ppc(mB34, mB38.x)) < 10.0f) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E226C, &daNpc_So_c::cutDiveProc);

/* 022E22C4 */
void daNpc_So_c::cutDisappearStart() {
    WWHD_FUNC(0x022E22C4, void, this);
    so_offsetDive(this);
    so_seStart(this, 0x593B /* JA_SE_CM_SO_DIVE */);
    fopKyM_createWpillar(&current.pos, gabi::fmuls_ppc(scale.x, 1.2f), 1.4f, 0);
}
VERIFY(0x022E22C4, &daNpc_So_c::cutDisappearStart);

/* 022E2334 */
void daNpc_So_c::cutDisappearProc() {
    WWHD_FUNC(0x022E2334, void, this);
    if (std::fabs(gabi::fsubs_ppc(mB34, mB38.x)) < 10.0f) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E2334, &daNpc_So_c::cutDisappearProc);

/* 022E238C */
void daNpc_So_c::cutSetAnmStart() {
    WWHD_FUNC(0x022E238C, void, this);
    u32 name = gabi::ea(so_getMyP(mB6C, 0x100222C8 /* "Name" */, 4));
    if (name != 0) {
        if (so_strcmp(name, 0x100222D0 /* "WAIT" */) == 0) {
            mAnmPrmIdx = 1;
        } else if (so_strcmp(name, 0x100222D8 /* "TALK" */) == 0) {
            mAnmPrmIdx = 3;
        } else if (so_strcmp(name, 0x100222E0 /* "JUMP" */) == 0) {
            mAnmPrmIdx = 4;
        } else if (so_strcmp(name, 0x100222E8 /* "SWIM" */) == 0) {
            mAnmPrmIdx = 2;
        }
    } else {
        mAnmPrmIdx = 1;
    }
}
VERIFY(0x022E238C, &daNpc_So_c::cutSetAnmStart);

/* 022E24C0 */
void daNpc_So_c::cutSetAnmProc() {
    WWHD_FUNC(0x022E24C0, void, this);
    so_getMyP(mB6C, 0x100222F0 /* "Name" */, 4); /* unused */
    if (isAnm(1) || isAnm(4) || isAnm(3) || isAnm(2)) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
    /* mpMorf->isStop(): the frame control's stop flag, or a zero play speed */
    u32 morf = gabi::ea(mpMorf.get());
    if ((gabi::load<u8>(morf + 0xA7) & 1) || gabi::load<f32>(morf + 0x98) == 0.0f) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E24C0, &daNpc_So_c::cutSetAnmProc);

/* 022E2570 */
void daNpc_So_c::cutEffectStart() {
    WWHD_FUNC(0x022E2570, void, this);
    gabi::Local<cXyz> pos;
    gabi::Local<cXyz> scl;
    pos->x = current.pos.x;
    pos->y = gabi::fsubs_ppc(current.pos.y, 80.0f);
    pos->z = current.pos.z;
    scl->x = 1.0f;
    scl->y = 1.0f;
    scl->z = 1.0f;
    u32 em = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 0, 0x8152 /* dPa_name::ID_IT_SN_PF_BIKON00 */, pos, nullptr, scl, 0xFF,
                                      nullptr, -1, nullptr, nullptr, nullptr));
    /* pEmitter->setGlobalParticleScale(0.62f, 0.6f) (HD: a 3-vector, z = 1) */
    gabi::store<f32>(em + 0x240, 1.0f);
    gabi::store<f32>(em + 0x238, 0.62f);
    gabi::store<f32>(em + 0x23C, 0.6f);
    so_seStart(this, 0x58BD /* JA_SE_CM_CMN_NOTICE */);
}
VERIFY(0x022E2570, &daNpc_So_c::cutEffectStart);

/* 022E266C (matcher: unnamed) */
void daNpc_So_c::cutEffectProc() {
    WWHD_FUNC(0x022E266C, void, this);
    dComIfGp_evmng_cutEnd(mB6C);
}
VERIFY(0x022E266C, &daNpc_So_c::cutEffectProc);

/* 022E26A4 (matcher: unnamed) */
void daNpc_So_c::cutEquipProc() {
    WWHD_FUNC(0x022E26A4, void, this);
    s32 id = mB6C;
    gabi::store<u8>(gabi::ea(this) + SO_EQUIP, 1);
    dComIfGp_evmng_cutEnd(id);
}
VERIFY(0x022E26A4, &daNpc_So_c::cutEquipProc);

/* 022E26E4 (matcher: unnamed) */
void daNpc_So_c::cutUnequipProc() {
    WWHD_FUNC(0x022E26E4, void, this);
    s32 id = mB6C;
    gabi::store<u8>(gabi::ea(this) + SO_EQUIP, 0);
    dComIfGp_evmng_cutEnd(id);
}
VERIFY(0x022E26E4, &daNpc_So_c::cutUnequipProc);

/* 022E2724 */
void daNpc_So_c::cutEatesaStart() {
    WWHD_FUNC(0x022E2724, void, this);
    /* GHS keeps `this` in r3 across offsetZero (known to return it) */
    daNpc_So_c* t = so_ret(gabi::call<u32>(0x022DE6AC, this));
    t->mAFC = 0.0f;
    t->speedF = 0.0f;
}
VERIFY(0x022E2724, &daNpc_So_c::cutEatesaStart);

/* 022E2754 */
void daNpc_So_c::cutEatesaProc() {
    WWHD_FUNC(0x022E2754, void, this);
    fopAc_ac_c* esa = so_searchEsa(this);
    if (esa != nullptr) {
        gabi::store<u8>(gabi::ea(esa) + SO_ESA_EAT, 0x23); /* HD: 0x23 (GameCube esa->field_0x298 = 1) */
    } else {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E2754, &daNpc_So_c::cutEatesaProc);

/* 022E27C4 */
void daNpc_So_c::cutEatesaFirstStart() {
    WWHD_FUNC(0x022E27C4, void, this);
    so_offsetZero(this);
    fopAc_ac_c* ship = so_getShip();
    dLib_circle_path_l* path = so_path(this);
    path->mRadius = gabi::fadds_ppc(so_REG_F(0x4B4), 650.0f); /* REG8_F(11) + 650.0f */
    path->mAngle = ship->shape_angle.y;
    mBDA = 1;
    mAFC = 0.0f;
    speedF = 0.0f;
}
VERIFY(0x022E27C4, &daNpc_So_c::cutEatesaFirstStart);

/* 022E2830 */
void daNpc_So_c::cutEatesaFirstProc() {
    WWHD_FUNC(0x022E2830, void, this);
    fopAc_ac_c* link = so_getLink();
    fopAc_ac_c* ship = so_getShip();
    gabi::Local<cXyz> target; /* local_28 */
    target->x = link->current.pos.x;
    target->y = link->current.pos.y;
    target->z = link->current.pos.z;
    if (ship != nullptr) {
        s32 idx = (s16)(ship->shape_angle.y - 0x4000 + so_REG_S(0x74C)); /* REG12_S(6) */
        target->x = ship->current.pos.x;
        target->y = ship->current.pos.y;
        f32 z = ship->current.pos.z;
        target->x = gabi::fmadds(260.0f, cM_ssin(idx), target->x);
        target->z = gabi::fmadds(260.0f, cM_scos(idx), z);
    }
    dLib_circle_path_l* path = so_path(this);
    if (so_absXZ(&current.pos, target) < gabi::fadds_ppc(so_REG_F(0x4B4), 700.0f)) { /* REG8_F(11) */
        cLib_addCalc2(&path->mRadius, 30.0f, 0.05f, gabi::fadds_ppc(so_REG_F(0x4B0), 8.0f)); /* REG8_F(10) */
    }
    path->mAngleSpeed = 0x300;
    path->mTranslation.copy(*target);
    path->mTranslation.y = so_getWaterY(&path->mTranslation, &mAcch);
    gabi::call(0x02587128, path); /* dLib_setCirclePath */
    path->mPos.y = so_getWaterY(&path->mPos, &mAcch);
    gabi::Local<cXyz> goal; /* local_34 */
    goal->x = path->mPos.x;
    goal->z = path->mPos.z;
    goal->y = gabi::fsubs_ppc(path->mPos.y, 100.0f);
    f64 abs1 = so_absXZ(&current.pos, goal);
    f64 abs2 = so_absXZ(&current.pos, target);
    mEventCut.mbAttention = 0; /* setAttnFlag(0) */
    mEventCut.mPos.x = goal->x; /* setAttnPos(local_34) */
    mEventCut.mPos.y = goal->y;
    mEventCut.mPos.z = goal->z;
    mB44.x = goal->x;
    mB44.y = goal->y;
    mB44.z = goal->z;
    f32 spd;
    bool turn;
    if (abs2 < gabi::fadds_ppc(so_REG_F(0x6E4), 100.0f)) { /* REG12_F(7) + 100.0f */
        spd = 2.0f;
        turn = true;
        if (!(abs2 > 50.0f)) {
            spd = 0.0f;
            fopAc_ac_c* esa = so_searchEsa(this);
            if (esa != nullptr) {
                gabi::store<u8>(gabi::ea(esa) + SO_ESA_EAT, 0x23); /* HD: 0x23 (GameCube 1) */
            } else {
                dComIfGp_evmng_cutEnd(mB6C);
                mBDA = 0;
                mB04 = 0.0f;
            }
            turn = false;
        }
    } else if (abs1 < gabi::fadds_ppc(so_REG_F(0x6E4), 200.0f)) {
        spd = 12.0f;
        turn = true;
    } else {
        spd = gabi::fadds_ppc(so_REG_F(0x6F0), 20.0f); /* REG12_F(10) + 20.0f */
        turn = spd != 0.0f;
    }
    if (turn) {
        s16 ang = cLib_targetAngleY(&mB54, goal);
        cLib_addCalcAngleS2(&shape_angle.y, ang, 3, 0x1200);
    }
    gabi::call(0x0200F164, &current.pos, goal.get(), 0.1f, spd); /* cLib_addCalcPos2 */
    current.pos.y = gabi::fsubs_ppc(so_getWaterY(&current.pos, &mAcch), 100.0f);
}
VERIFY(0x022E2830, &daNpc_So_c::cutEatesaFirstProc);

/* 022E2C18 */
void daNpc_So_c::cutJumpMapopenStart() {
    WWHD_FUNC(0x022E2C18, void, this);
    mBBC = 0;
    /* GHS keeps `this` in r3 across offsetZero (known to return it) */
    daNpc_So_c* t = so_ret(gabi::call<u32>(0x022DE6AC, this));
    so_setAnm(t, 2, false);
    mBDA = 1;
}
VERIFY(0x022E2C18, &daNpc_So_c::cutJumpMapopenStart);

/* 022E2C5C */
void daNpc_So_c::cutJumpMapopenProc() {
    WWHD_FUNC(0x022E2C5C, void, this);
    fopAc_ac_c* ship = so_getShip();
    if (ship == nullptr) {
        dComIfGp_evmng_cutEnd(mB6C);
        return;
    }
    if (mBBC == 0) {
        mB34 = mB38.x;
        gravity = so_hioF(0x80);
        speedF = 0.0f;
        f32 vy = so_hioF(0x84);
        mB00 = vy;
        mAFC = 0.0f;
        speed.y = vy;
        shape_angle.x = gabi::load<s16>(SO_L_HIO + 0x68);
        so_seStart(this, 0x593C /* JA_SE_CM_SO_JUMP_DRAW */);
        mBBC = mBBC + 1;
    } else if (mBBC == 1) {
        mBB0.x = ship->current.pos.x;
        mBB0.y = ship->current.pos.y;
        mBB0.z = ship->current.pos.z;
        u16 idx = (u16)(ship->shape_angle.y + 0x4000);
        so_offsetXZ(&mBB0, &mBB0, idx, -so_hioF(0x88));
        f32 y = current.pos.y;
        if (speed.y < 1.0) {
            speed.y = 1.0f;
        }
        if (gabi::call<BOOL>(0x0200F764, &current.pos, &mBB0, so_hioF(0x90)) != 0) { /* cLib_chasePosXZ */
            current.pos.y = y;
            if (gabi::load<u8>(SO_L_HIO + 0x94) == 0) {
                gabi::store<u8>(dComIfGp_ea() + 0x5BDB, 2); /* dComIfGp_fmapOpenFishOn() */
            }
            shape_angle.y = shape_angle.y + 0x8000;
            mBBC = mBBC + 1;
        }
    } else if (mBBC == 2) {
        mBB0.x = ship->current.pos.x;
        mBB0.y = ship->current.pos.y;
        mBB0.z = ship->current.pos.z;
        u16 idx = (u16)(ship->shape_angle.y + 0x4000);
        so_offsetXZ(&mBB0, &mBB0, idx, -so_hioF(0x8C));
        f32 y = current.pos.y;
        if (gabi::call<BOOL>(0x0200F764, &current.pos, &mBB0, so_hioF(0x90)) != 0) {
            current.pos.y = y;
            so_seStart(this, 0x593B /* JA_SE_CM_SO_DIVE */);
            fopKyM_createWpillar(&current.pos, gabi::fmuls_ppc(scale.x, 1.4f), 1.4f, 0);
            mB00 = 0.0f;
            speedF = 0.0f;
            mAFC = 0.0f;
            speed.y = 0.0f;
            dComIfGp_evmng_cutEnd(mB6C);
            so_offsetDive(this);
            mBDA = 0;
        }
    }
}
VERIFY(0x022E2C5C, &daNpc_So_c::cutJumpMapopenProc);

/* the camera centre/eye in front of / behind Link (cutMiniGameStart, initCam) */
static inline void so_camAroundLink(daNpc_So_c* t, fopAc_ac_c* link) {
    t->mBCC.x = link->current.pos.x;
    t->mBCC.y = link->current.pos.y;
    t->mBCC.z = link->current.pos.z;
    t->mBCC.x = gabi::fmadds(100.0f, cM_ssin((u16)link->shape_angle.y), t->mBCC.x);
    t->mBCC.y = gabi::fadds_ppc(t->mBCC.y, 50.0f);
    t->mBCC.z = gabi::fmadds(100.0f, cM_scos((u16)link->shape_angle.y), t->mBCC.z);
    t->mBC0.x = link->current.pos.x;
    t->mBC0.y = link->current.pos.y;
    t->mBC0.z = link->current.pos.z;
    t->mBC0.x = gabi::fmadds(-200.0f, cM_ssin((u16)link->shape_angle.y), t->mBC0.x);
    t->mBC0.y = gabi::fadds_ppc(t->mBC0.y, 100.0f);
    t->mBC0.z = gabi::fmadds(-200.0f, cM_scos((u16)link->shape_angle.y), t->mBC0.z);
}

/* 022E2F8C */
void daNpc_So_c::cutMiniGameStart() {
    WWHD_FUNC(0x022E2F8C, void, this);
    mB74 = 0;
    mB78 = 0;
    mB80 = 0;
    mB7C = 0;
    so_offsetDive(this);
    /* dComIfGp_startMiniGame(8) */
    u32 play = dComIfGp_ea();
    gabi::store<u8>(play + 0x5CEA, 8);
    gabi::store<u16>(play + 0x5CE8, gabi::load<u16>(play + 0x5CE8) | 0x80);
    mDoAud_seStart(0x8F0 /* JA_SE_START_WHISTLE */, nullptr, 0, 0);
    fopAc_ac_c* link = so_getLink();
    u32 cam = so_getCamera();
    if (so_REG_S(0x502) == 0) { /* REG8_S(1) */
        so_camAroundLink(this, link);
        gabi::Local<cXyz> center;
        gabi::Local<cXyz> eye;
        center->x = mBCC.x;
        center->y = mBCC.y;
        center->z = mBCC.z;
        eye->x = mBC0.x;
        eye->y = mBC0.y;
        eye->z = mBC0.z;
        so_camReset(cam, center, eye);
        so_camStart(cam);
    }
    so_changeOriginalDemo(link);
    so_changeDemoMode(link, 0x44 /* daPy_demo_c::DEMO_BOW_MINIGAME_e */);
    mBDB = 0;
    mBDE = 0;
}
VERIFY(0x022E2F8C, &daNpc_So_c::cutMiniGameStart);

/* 022E3158 */
void daNpc_So_c::initCam() {
    WWHD_FUNC(0x022E3158, void, this);
    if (mBDE == 0) { /* GameCube: mBDE != 1 */
        mBDE = 1;
        u32 cam = so_getCamera();
        if (so_REG_S(0x502) == 0) { /* REG8_S(1) */
            so_camStop(cam);
            so_camSetTrimSize(cam, 1);
            so_camAroundLink(this, so_getLink());
        }
    }
}
VERIFY(0x022E3158, &daNpc_So_c::initCam);

/* 022E32AC */
void daNpc_So_c::moveCam() {
    WWHD_FUNC(0x022E32AC, void, this);
    if (so_REG_S(0x502) == 0) { /* REG8_S(1) */
        u32 cam = so_getCamera();
        fopAc_ac_c* link = so_getLink();
        gabi::Local<cXyz> eye;    /* local_2C */
        gabi::Local<cXyz> center; /* local_20 */
        center->x = gabi::fmadds(150.0f, cM_ssin((u16)link->shape_angle.y), link->current.pos.x);
        center->y = gabi::fadds_ppc(link->current.pos.y, 100.0f);
        center->z = gabi::fmadds(150.0f, cM_scos((u16)link->shape_angle.y), link->current.pos.z);
        eye->x = gabi::fmadds(-400.0f, cM_ssin((u16)link->shape_angle.y), link->current.pos.x);
        eye->y = gabi::fadds_ppc(link->current.pos.y, 300.0f);
        eye->z = gabi::fmadds(-400.0f, cM_scos((u16)link->shape_angle.y), link->current.pos.z);
        gabi::call(0x0200F164, &mBC0, eye.get(), 0.05f, 5.0f); /* cLib_addCalcPos2 */
        gabi::call(0x0200F164, &mBCC, center.get(), 0.05f, 5.0f);
        gabi::Local<cXyz> c;
        gabi::Local<cXyz> e;
        c->x = mBCC.x;
        c->y = mBCC.y;
        e->x = mBC0.x;
        e->z = mBC0.z;
        c->z = mBCC.z;
        e->y = mBC0.y;
        so_camSet(cam, c, e);
    }
}
VERIFY(0x022E32AC, &daNpc_So_c::moveCam);

/* ---- cutMiniGameProc (022E3484) helpers ---- */
#define SO_MTX_NOW 0x1048D0CC /* mDoMtx_stack_c::now */
/* dComIfGp_endMiniGame(8) */
static inline void so_endMiniGame() {
    u32 play = dComIfGp_ea();
    u16 f = gabi::load<u16>(play + 0x5CE8);
    gabi::store<u8>(play + 0x5CEE, 0);
    gabi::store<u8>(play + 0x5CEA, 0);
    gabi::store<u16>(play + 0x5CE8, f ^ 0x80);
}
/* the end of the mini-game: the player back to talk, the camera on Link, the whistle */
static inline void so_miniGameFinish(daNpc_So_c* t, fopAc_ac_c* player) {
    so_changeDemoMode(player, 6 /* daPy_demo_c::DEMO_N_TALK_e */);
    so_changeDemoParam0(player, 1);
    t->initCam();
    t->moveCam();
    mDoAud_seStart(0x8F1 /* JA_SE_END_WHISTLE */, nullptr, 0, 0);
    so_endMiniGame();
    s32 id = t->mB6C;
    t->mAFC = 0.0f;
    t->speedF = 0.0f;
    dComIfGp_evmng_cutEnd(id);
}
/* the next attention point: (mB88, 0, mB8C) turned by angY, plus base */
static inline void so_miniGameAttnPos(daNpc_So_c* t, s16 angY, cXyz* base, f32 x, f32 z) {
    gabi::Local<cXyz> ofs;
    gabi::Local<cXyz> out;
    ofs->x = x;
    ofs->y = 0.0f;
    ofs->z = z;
    out->x = 0.0f;
    out->y = 0.0f;
    out->z = 0.0f;
    gabi::call(0x025F1884, SO_MTX_NOW, angY);               /* mDoMtx_YrotS(now, angY) */
    gabi::call(0x028E8F64, SO_MTX_NOW, ofs.get(), out.get()); /* PSMTXMultVec */
    gabi::call(0x028E8D88, out.get(), base, out.get());     /* PSVECAdd */
    t->mEventCut.mPos.copy(*out);                           /* mEventCut.setAttnPos (word copy) */
}
/* turn towards the attention point; returns the XZ distance (unrounded) */
static inline f64 so_miniGameTurn(daNpc_So_c* t, cXyz* p) {
    f64 abs = so_absXZ(&t->current.pos, p);
    s16 ang = cLib_targetAngleY(&t->current.pos, p);
    cLib_addCalcAngleS2(&t->shape_angle.y, ang, 8, 0x400);
    return abs;
}
/* case 7: the jump's hit sphere and the landing */
static inline void so_miniGameJump(daNpc_So_c* t, fopAc_ac_c* player, bool done) {
    f32 water = so_getWaterY(&t->current.pos, &t->mAcch);
    if (t->mB84 == 0 && t->mB7C < 10) {
        if (t->m6D8 == 0) {
            u32 sph = gabi::ea(&t->mSph) + 0x118; /* mSph's cM3dGSph */
            if (!(t->current.pos.y < gabi::fadds_ppc(water, 50.0f))) {
                gabi::call(0x02018C8C, sph, so_hioF(0x3C)); /* SetR(l_HIO.m3C) */
                gabi::call(0x02018D40, sph, &t->current.pos); /* SetC */
                dComIfG_Ccsp_Set(&t->mSph);
            } else {
                gabi::Local<cXyz> c;
                c->z = 0.0f;
                c->x = 0.0f;
                c->y = 30000.0f;
                gabi::call(0x02018C8C, sph, 0.0f);
                gabi::call(0x02018D40, sph, c.get());
                dComIfG_Ccsp_Set(&t->mSph);
            }
        }
        if (gabi::call<bool>(0x022E160C, t)) { /* checkTgHit */
            s32 n = t->mB7C + 1;
            t->mB7C = n < 10 ? n : 10;
        }
    }
    if (!(t->current.pos.y > water)) {
        so_seStart(t, 0x5939 /* JA_SE_CM_SO_LANDING_L */);
        fopKyM_createWpillar(&t->current.pos, gabi::fmuls_ppc(t->scale.x, 1.4f), 1.4f, 0);
        s32 n = t->mB80;
        t->mAFC = 0.0f;
        t->speedF = 0.0f;
        t->mB80 = n + 1;
        if (n + 1 >= 10 || done) {
            so_miniGameFinish(t, player);
        } else {
            t->mB74 = 0;
        }
    }
}

/* 022E3484 */
void daNpc_So_c::cutMiniGameProc() {
    WWHD_FUNC(0x022E3484, void, this);
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 angY = player->shape_angle.y;
    gabi::Local<cXyz> base; /* local_34 */
    base->x = player->current.pos.x;
    base->y = player->current.pos.y;
    base->z = player->current.pos.z;
    bool done = false;
    dComIfGp_ea();
    dComIfGp_ea();
    fopAc_ac_c* ship = so_getShip(); /* dComIfGp_getShipActor() */
    if (ship != nullptr) {
        angY = (s16)(ship->shape_angle.y - 0x4000 + so_REG_S(0x74C)); /* REG12_S(6) */
        base->copy(ship->current.pos);
    }
    if (gabi::load<u32>(gabi::ea(player) + 0x3C0) & 0x20000000) { /* player->checkArrowShoot() */
        mB78 = mB78 + 1;
    }
    if (mB78 >= 10) {
        done = true;
        if (mB74 <= 6) {
            so_miniGameFinish(this, player);
            return;
        }
    }
    switch ((u32)(s32)mB74) {
    case 0:
        so_setAnm(this, 2, false);
        mAFC = 0.0f;
        speedF = 0.0f;
        mB74 = mB74 + 1;
        mA9C = 30;
        mB84 = 0;
        so_offsetSwim(this);
        break;
    case 1:
        if (so_calcTimerI(&mA9C) == 0) {
            mB74 = mB74 + 1;
        }
        break;
    case 2: {
        so_setAnm(this, 2, false);
        mB8C = gabi::fadds_ppc(cM_rndF(400.0f), 1000.0f);
        f32 x = gabi::fadds_ppc(-500.0f, cM_rndF(1000.0f));
        mB88 = x;
        so_miniGameAttnPos(this, angY, base, x, mB8C);
        mB74 = mB74 + 1;
        so_offsetSwim(this);
        break;
    }
    case 3: {
        gabi::Local<cXyz> p;
        p->x = mEventCut.mPos.x;
        p->y = mEventCut.mPos.y;
        p->z = mEventCut.mPos.z;
        mAFC = gabi::fadds_ppc(so_REG_F(0x6F0), 12.0f); /* REG12_F(10) + 12.0f */
        f64 abs = so_miniGameTurn(this, p);
        if (abs < gabi::fadds_ppc(so_REG_F(0x6E4), 150.0f)) { /* REG12_F(7) + 150.0f */
            mAFC = 0.0f;
            mB74 = mB74 + 1;
        }
        break;
    }
    case 4: {
        f32 r = cM_rndF(300.0f);
        f32 z = mB8C;
        f32 x = gabi::fadds_ppc(mB88, gabi::fadds_ppc(-150.0f, r));
        mB88 = x;
        so_miniGameAttnPos(this, angY, base, x, z);
        mAFC = 0.0f;
        mB74 = mB74 + 1;
        speedF = 0.0f;
        mA9C = 30;
        so_offsetSwim(this);
        break;
    }
    case 5: {
        gabi::Local<cXyz> p;
        p->y = mEventCut.mPos.y;
        p->z = mEventCut.mPos.z;
        p->x = mEventCut.mPos.x;
        mAFC = 0.0f;
        so_miniGameTurn(this, p);
        if (so_calcTimerI(&mA9C) == 0) {
            mB74 = mB74 + 1;
        }
        break;
    }
    case 6: {
        current.pos.y = so_getWaterY(&current.pos, &mAcch);
        f32 spd = gabi::fmuls_ppc(gabi::fadds_ppc(cM_rndF(5.0f), 5.0f), mB08);
        mAFC = spd;
        speedF = spd;
        f32 vy = gabi::fmadds(18.0f, mB08, gabi::fmuls_ppc(spd, 4.0f));
        speed.y = vy;
        f32 mx = so_hioF(0x50);
        if (vy > mx) { /* ble: a NaN keeps vy */
            speed.y = mx;
        }
        gravity = -0.8f;
        so_setAnm(this, 4, false);
        shape_angle.x = gabi::load<s16>(SO_L_HIO + 0x68);
        so_offsetZero(this);
        mB74 = mB74 + 1;
        so_seStart(this, 0x5938 /* JA_SE_CM_SO_JUMP_L */);
        break;
    }
    case 7:
        so_miniGameJump(this, player, done);
        break;
    }
}
VERIFY(0x022E3484, &daNpc_So_c::cutMiniGameProc);

/* 022E3CF0 (matcher: unnamed) */
void daNpc_So_c::cutTurnStart() {
    WWHD_FUNC(0x022E3CF0, void, this);
    mA9C = 30;
}
VERIFY(0x022E3CF0, &daNpc_So_c::cutTurnStart);

/* 022E3CFC */
void daNpc_So_c::cutTurnProc() {
    WWHD_FUNC(0x022E3CFC, void, this);
    fopAc_ac_c* ship = so_getShip();
    if (ship == nullptr) {
        dComIfGp_evmng_cutEnd(mB6C);
        return;
    }
    u16 idx = (u16)(ship->shape_angle.y + 0x4000);
    gabi::Local<cXyz> p;
    p->x = ship->current.pos.x;
    p->y = ship->current.pos.y;
    f32 z = ship->current.pos.z;
    p->x = gabi::fmadds(-300.0f, cM_ssin(idx), p->x);
    p->z = gabi::fmadds(-300.0f, cM_scos(idx), z);
    s16 target = cLib_targetAngleY(&current.pos, p);
    cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
    s32 dist = gabi::call<s32>(0x0200FAAC, (s16)shape_angle.y, target); /* cLib_distanceAngleS (the whole r3 is compared) */
    if (so_calcTimerI(&mA9C) == 0 || dist <= 0x400) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E3CFC, &daNpc_So_c::cutTurnProc);

/* daPy_py_c::setPlayerPosAndAngle (virtual, HD vtable slot 0x114) */
static inline void so_setPlayerPosAndAngle(fopAc_ac_c* pl, cXyz* pos, s16 ang) {
    u32 fn = gabi::load<u32>(gabi::load<u32>(gabi::ea(pl) + 0xB4) + 0x114);
    gabi::call_ptr(fn, pl, pos, ang);
}
/* daShip_c::initStartPos(pos, angle) */
static inline void so_shipInitStartPos(fopAc_ac_c* ship, cXyz* pos, s16 ang) { gabi::call(0x024832E0, ship, pos, ang); }
/* the actor 300 units beside the ship (cutMiniGameWarp/PlTurn/Return) */
static inline void so_besideShip(daNpc_So_c* t, fopAc_ac_c* ship) {
    u16 idx = (u16)(ship->shape_angle.y - 0x4000 + so_REG_S(0x74C)); /* REG12_S(6) */
    f32 x = ship->current.pos.x;
    t->current.pos.x = x;
    t->current.pos.y = ship->current.pos.y;
    f32 z = ship->current.pos.z;
    t->current.pos.z = z;
    t->current.pos.x = gabi::fmadds(300.0f, cM_ssin(idx), x);
    t->current.pos.z = gabi::fmadds(300.0f, cM_scos(idx), z);
}
static inline void so_camResetStart(daNpc_So_c* t) {
    u32 cam = so_getCamera();
    gabi::Local<cXyz> center;
    gabi::Local<cXyz> eye;
    center->x = t->mBCC.x;
    center->y = t->mBCC.y;
    center->z = t->mBCC.z;
    eye->x = t->mBC0.x;
    eye->y = t->mBC0.y;
    eye->z = t->mBC0.z;
    so_camReset(cam, center, eye);
    so_camStart(cam);
}

/* 022E3E08 */
void daNpc_So_c::cutMiniGameWarpStart() {
    WWHD_FUNC(0x022E3E08, void, this);
    gabi::call(0x025E201C, 1); /* JAIZelBasic::zel_basic->field_0x00bf = 1 */
    fopAc_ac_c* link = so_getLink();
    so_changeOriginalDemo(link);
    fopAc_ac_c* ship = so_getShip();
    mB90.y = so_getWaterY(&mB90, &mAcch);
    mBA0.copy(ship->current.pos);
    mBAC = ship->shape_angle.y;
    so_shipInitStartPos(ship, &mB90, (s16)(mB9C + 0x4000));
    so_setPlayerPosAndAngle(link, &mB90, (s16)(mB9C + so_REG_S(0x50C))); /* REG8_S(6) */
    speedF = 0.0f;
    mAFC = 0.0f;
    u16 idx = (u16)(ship->shape_angle.y - 0x4000 + so_REG_S(0x74C)); /* REG12_S(6) */
    f32 x = mB90.x;
    current.pos.x = x;
    current.pos.y = mB90.y;
    f32 z = mB90.z;
    current.pos.z = z;
    current.pos.x = gabi::fmadds(300.0f, cM_ssin(idx), x);
    mBDB = 1;
    current.pos.z = gabi::fmadds(300.0f, cM_scos(idx), z);
    mA9C = so_REG_S(0x742) + 10; /* REG12_S(1) + 10 */
    so_camResetStart(this);
}
VERIFY(0x022E3E08, &daNpc_So_c::cutMiniGameWarpStart);

/* 022E3FD0 */
void daNpc_So_c::cutMiniGameWarpProc() {
    WWHD_FUNC(0x022E3FD0, void, this);
    fopAc_ac_c* link = so_getLink();
    so_changeOriginalDemo(link);
    fopAc_ac_c* ship = so_getShip();
    so_changeDemoParam0(link, 1);
    so_changeDemoMode(link, 6 /* daPy_demo_c::DEMO_N_TALK_e */);
    so_besideShip(this, ship);
    if (so_calcTimerI(&mA9C) == 0) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E3FD0, &daNpc_So_c::cutMiniGameWarpProc);

/* 022E40B8 */
void daNpc_So_c::cutMiniGameReturnStart() {
    WWHD_FUNC(0x022E40B8, void, this);
    gabi::call(0x025E201C, 0); /* JAIZelBasic::zel_basic->field_0x00bf = 0 */
    fopAc_ac_c* link = so_getLink();
    so_changeOriginalDemo(link);
    /* fopAcM_SearchByName(fpcNm_SHIP_e) */
    gabi::Local<be<s16>> name;
    *name = 0xA5;
    fopAc_ac_c* ship = fopAcIt_Judge(0x025E121C /* fpcSch_JudgeForPName */, name.get());
    if (ship == nullptr) {
        gabi::call(0x0273AA24, 0x10022350, 0x3BA, 0x10022344); /* HD: JUT_ASSERT(ship) */
    }
    so_setPlayerPosAndAngle(link, &mBA0, (s16)(mB9C + so_REG_S(0x50C))); /* REG8_S(6) */
    if (ship != nullptr) {
        so_shipInitStartPos(ship, &mBA0, mBAC);
    }
    mBDB = 1;
    mAFC = 0.0f;
    speedF = 0.0f;
    so_setAnm(this, 2, false);
    so_offsetDive(this);
}
VERIFY(0x022E40B8, &daNpc_So_c::cutMiniGameReturnStart);

/* the camera behind Link (cutMiniGameReturnProc, cutMiniGamePlUpProc) */
static inline void so_camBehindLink(daNpc_So_c* t, fopAc_ac_c* link) {
    gabi::Local<cXyz> ofs; /* local_1C */
    gabi::Local<cXyz> out; /* local_28 */
    out->z = 0.0f;
    ofs->x = 0.0f;
    out->y = 0.0f;
    ofs->y = 0.0f;
    out->x = 0.0f;
    ofs->z = 0.0f;
    gabi::call(0x025F1884, gabi::load<u32>(0x1018C7B0) /* calc_mtx */, (s16)link->shape_angle.y); /* mDoMtx_YrotS */
    ofs->x = gabi::fadds_ppc(so_REG_F(0x18), -50.0f); /* REG0_F(4) */
    ofs->y = gabi::fadds_ppc(so_REG_F(0x1C), 50.0f);  /* REG0_F(5) */
    ofs->z = gabi::fadds_ppc(so_REG_F(0x20), 100.0f); /* REG0_F(6) */
    gabi::call(0x0200FCD8, ofs.get(), out.get());     /* MtxPosition */
    gabi::Local<cXyz> eye;
    gabi::call(0x0201AD78, &link->current.pos, eye.get(), out.get()); /* cXyz::operator+ */
    t->mBC0.copy(*eye);
    t->mBCC.x = link->current.pos.x;
    f32 y = link->current.pos.y;
    t->mBCC.y = y;
    t->mBCC.z = link->current.pos.z;
    t->mBCC.y = gabi::fadds_ppc(y, gabi::fadds_ppc(so_REG_F(0x24), 90.0f)); /* REG0_F(7) + 90.0f */
}

/* 022E41BC */
void daNpc_So_c::cutMiniGameReturnProc() {
    WWHD_FUNC(0x022E41BC, void, this);
    fopAc_ac_c* ship = so_getShip();
    so_besideShip(this, ship);
    shape_angle.x = 0;
    if (so_absXZ(&mBA0, &current.pos) < 400.0f) {
        fopAc_ac_c* link = so_getLink();
        so_changeDemoMode(link, 6 /* daPy_demo_c::DEMO_N_TALK_e */);
        so_changeOriginalDemo(link);
        so_changeDemoParam0(link, 1);
        so_camBehindLink(this, link);
        so_camResetStart(this);
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E41BC, &daNpc_So_c::cutMiniGameReturnProc);

/* 022E442C */
void daNpc_So_c::cutPartnerShipProc() {
    WWHD_FUNC(0x022E442C, void, this);
    fopAc_ac_c* ship = so_getShip();
    /* dComIfGp_event_setTalkPartner(ship): mPtTalk = getPId(ship) */
    u32 evt = dComIfGp_ea() + 0x51D0;
    gabi::store<u32>(evt + 0xCC, gabi::call<u32>(0x0253F124, evt, ship));
    dComIfGp_evmng_cutEnd(mB6C);
}
VERIFY(0x022E442C, &daNpc_So_c::cutPartnerShipProc);

/* 022E4498 */
void daNpc_So_c::cutMiniGameWaitStart() {
    WWHD_FUNC(0x022E4498, void, this);
    u32 cam = so_getCamera();
    if (so_REG_S(0x502) == 0) { /* REG8_S(1) */
        so_camStop(cam);
        so_camSetTrimSize(cam, 1);
    }
}
VERIFY(0x022E4498, &daNpc_So_c::cutMiniGameWaitStart);

/* 022E44FC */
void daNpc_So_c::cutMiniGameWaitProc() {
    WWHD_FUNC(0x022E44FC, void, this);
    dComIfGp_ea(); /* HD: a dead camera lookup (two play getter calls) */
    dComIfGp_ea();
    dComIfGp_evmng_cutEnd(mB6C);
}
VERIFY(0x022E44FC, &daNpc_So_c::cutMiniGameWaitProc);

/* 022E4540 */
void daNpc_So_c::cutMiniGameEndStart() {
    WWHD_FUNC(0x022E4540, void, this);
    mBDE = 0;
    mA9C = so_REG_S(0x740) + 0x2D; /* REG12_S(0) + 45 */
}
VERIFY(0x022E4540, &daNpc_So_c::cutMiniGameEndStart);

/* 022E455C */
void daNpc_So_c::cutMiniGameEndProc() {
    WWHD_FUNC(0x022E455C, void, this);
    initCam();
    moveCam();
    if (so_calcTimerI(&mA9C) == 0) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E455C, &daNpc_So_c::cutMiniGameEndProc);

/* 022E45B4 */
void daNpc_So_c::cutMiniGamePlTurnStart() {
    WWHD_FUNC(0x022E45B4, void, this);
    mBDE = 0;
    mA9C = so_REG_S(0x744) + 10; /* REG12_S(2) + 10 */
}
VERIFY(0x022E45B4, &daNpc_So_c::cutMiniGamePlTurnStart);

/* 022E45D0 */
void daNpc_So_c::cutMiniGamePlTurnProc() {
    WWHD_FUNC(0x022E45D0, void, this);
    fopAc_ac_c* link = so_getLink();
    so_changeOriginalDemo(link);
    fopAc_ac_c* ship = so_getShip();
    so_changeDemoParam0(link, 1);
    so_changeDemoMode(link, 6 /* daPy_demo_c::DEMO_N_TALK_e */);
    so_besideShip(this, ship);
    if (so_calcTimerI(&mA9C) == 0) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E45D0, &daNpc_So_c::cutMiniGamePlTurnProc);

/* 022E46B8 */
void daNpc_So_c::cutMiniGamePlUpStart() {
    WWHD_FUNC(0x022E46B8, void, this);
    mA9C = 30;
    u32 cam = so_getCamera();
    so_camStop(cam);
    so_camSetTrimSize(cam, 1);
}
VERIFY(0x022E46B8, &daNpc_So_c::cutMiniGamePlUpStart);

/* 022E4714 */
void daNpc_So_c::cutMiniGamePlUpProc() {
    WWHD_FUNC(0x022E4714, void, this);
    fopAc_ac_c* link = so_getLink();
    so_changeDemoMode(link, 6 /* daPy_demo_c::DEMO_N_TALK_e */);
    so_camBehindLink(this, link);
    u32 cam = so_getCamera();
    gabi::Local<cXyz> center;
    gabi::Local<cXyz> eye;
    center->x = mBCC.x;
    center->y = mBCC.y;
    center->z = mBCC.z;
    eye->x = mBC0.x;
    eye->y = mBC0.y;
    eye->z = mBC0.z;
    so_camSet(cam, center, eye);
    if (so_calcTimerI(&mA9C) == 0) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}
VERIFY(0x022E4714, &daNpc_So_c::cutMiniGamePlUpProc);

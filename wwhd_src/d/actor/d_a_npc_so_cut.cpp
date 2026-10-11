//
// d_a_npc_so_cut.cpp (WWHD)
// NPC - Fishman: event cuts and the mini-game camera
//
// Ported from the GameCube decompilation
// (zeldaret/tww src/d/actor/d_a_npc_so.cpp) to the WWHD layout and code, verified against
// cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
#include "d/actor/d_a_npc_so_cut.h"
#include "gc_api/f_op/f_op_actor_mng.h"

/* ---- this TU's statics (HD addresses) ---- */
/* l_HIO fields read by the cuts (absolute addresses; GameCube names) */

/* ---- calls into the other parts of the unit (by address) ---- */
static inline void so_offsetZero(daNpc_So_c* t) { soCut_offsetZero(t); }
static inline void so_offsetDive(daNpc_So_c* t) { soCut_offsetDive(t); }
static inline void so_offsetSwim(daNpc_So_c* t) { soCut_offsetSwim(t); }
static inline void so_offsetAppear(daNpc_So_c* t) { soCut_offsetAppear(t); }
static inline void so_setAnm(daNpc_So_c* t, s8 idx, bool b) { soCut_setAnm(t, idx, b); }

// Private bindings preserve the verified HD return widths.
/* dLib_getWaterY( &pos, acch) */
static inline f32 so_getWaterY(cXyz* pos, dBgS_ObjAcch* acch) { return soCut_getWaterY(pos, acch); }
/* cLib_calcTimer<int> (out of line; the whole r3 is tested) */
static inline s32 so_calcTimerI(be<s32>* t) { return soCut_calcTimer(t); }
/* fopAcM_seStart(this, id, 0): HD inline without the NULL checks */
static inline void so_seStart(fopAc_ac_c* a, u32 id) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(a));
    mDoAud_seStart(id, &a->eyePos, 0, reverb);
}
/* (a - b).absXZ(): cXyz::operator- into a temporary, then sqrtf(PSVECSquareMag(x, 0, z));
 * the f1 result is used unrounded */
/* inlined into Fishman cuts */
static inline f64 so_absXZ(cXyz* a, cXyz* b) {
    cXyz d;
    soCut_subtract(a, &d, b);
    cXyz v;
    v.y = 0.0f;
    v.x = d.x;
    v.z = d.z;
    f64 sq = soCut_squareMagnitude(&v);
    return soCut_sqrtUnrounded(sq);
}
static inline void* so_getMyP(s32 staffId, const char* name, s32 type) {
    return dComIfGp_evmng_getMySubstanceP(staffId, name, type);
}
/* fopAcM_Search(searchEsa_CB, this) */
static inline fopAc_ac_c* so_searchEsa(daNpc_So_c* t) {
    return static_cast<fopAc_ac_c*>(fopAcM_Search(soCut_searchEsa_CB, t));
}
/* inline strcmp (GHS) */
static inline int so_strcmp(u32 a, const char* literal) {
    u32 b = gabi::guest_addr(literal);
    for (;;) {
        u8 x = (u8)soCutBytes(a)[0];
        u8 y = (u8)soCutBytes(b)[0];
        ++a;
        ++b;
        if (x != y || x == 0)
            return (int)x - (int)y;
    }
}
/* dLib_circle_path_c (0x24), as used by the cuts (GameCube names) */

static inline SoCutCirclePath* so_path(daNpc_So_c* t) {
    return reinterpret_cast<SoCutCirclePath*>(t->mB10);
}
/* the ship / Link actors (play + 0x5B3C / 0x5B34) */
static inline fopAc_ac_c* so_getShip() { return dComIfGp_getShipActor(); }
static inline fopAc_ac_c* so_getLink() { return dComIfGp_getLinkPlayer(); }
/* esa_class field_0x298 (HD +0x3B4) */
/* mA78 (equip flag): HD +0xBF1 (the header names that byte mA79) */
/* dComIfGp_getCamera(dComIfGp_getPlayerCameraID(0)): two calls to the play getter */
static inline u32 so_getCamera() {
    s32 id = dComIfGp_getPlayerCameraID(0);
    return (u32)soCutPlay(dComIfGp_ea()).mCamera[id].mpCamera;
}
/* dCamera_c (camera_process_class + 0x248) */
static inline void so_camReset(u32 cam, cXyz* center, cXyz* eye) {
    soCut_resetCamera(cam + 0x248, center, eye);
}
static inline void so_camStart(u32 cam) { soCut_startCamera(cam + 0x248); }
static inline void so_camStop(u32 cam) { soCut_stopCamera(cam + 0x248); }
static inline void so_camSetTrimSize(u32 cam, s32 n) { soCut_trimCamera(cam + 0x248, n); }
static inline void so_camSet(u32 cam, cXyz* center, cXyz* eye) {
    soCut_setCamera(cam + 0x248, center, eye);
}
// daPy_py_c demo (HD: mDemo at +0x420: type, param0 +0x428, mode +0x430)
static inline void so_changeOriginalDemo(fopAc_ac_c* pl) {
    soCutPlayer(pl).mDemoType = 3;
    soCutPlayer(pl).mDemoParam0 = 0;
}
static inline void so_changeDemoMode(fopAc_ac_c* pl, u32 mode) { soCutPlayer(pl).mDemoMode = mode; }
static inline void so_changeDemoParam0(fopAc_ac_c* pl, s32 v) { soCutPlayer(pl).mDemoParam0 = v; }
/* p + r * angle-vector (fmadds, as cM_ssin/cM_scos are inlined) */
static inline void so_offsetXZ(cXyz* out, cXyz* base, u16 ang, f32 r) {
    out->x = gabi::fmadds(r, cM_ssin(ang), base->x);
    out->z = gabi::fmadds(r, cM_scos(ang), base->z);
}

/* 022E1E74 */
void daNpc_So_c::cutSwimProc() {
    cXyz pos;
    pos.x = mEventCut.mPos.x; /* mEventCut.getAttnPos() */
    pos.y = mEventCut.mPos.y;
    pos.z = mEventCut.mPos.z;
    mAFC = gabi::fadds_ppc(soCutRegisters().REG12_F10, 7.0f); /* REG12_F(10) + 7.0f */
    f64 abs = so_absXZ(&current.pos, &pos);
    s16 target = cLib_targetAngleY(&current.pos, &pos);
    cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
    if (abs < gabi::fadds_ppc(soCutRegisters().REG12_F7, 150.0f)) { /* REG12_F(7) + 150.0f */
        mAFC = 0.0f;
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E1F90 */
void daNpc_So_c::cutJumpStart() {
    mA9C = 0xF;
    current.pos.y = so_getWaterY(&current.pos, &mAcch);
    be<f32>* pSpeedY = (be<f32>*)so_getMyP(mB6C, "Speed_y", 0);
    be<f32>* pSpeedF = (be<f32>*)so_getMyP(mB6C, "SpeedF", 0);
    shape_angle.x = (s16)soCutHIO().m68;
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
    so_seStart(this, SoCutConstants::JA_SE_CM_SO_JUMP_L);
}

/* 022E20EC */
void daNpc_So_c::cutJumpProc() {
    f32 waterHeight = so_getWaterY(&current.pos, &mAcch);
    if (so_calcTimerI(&mA9C) == 0 && !(current.pos.y > waterHeight)) {
        so_seStart(this, SoCutConstants::JA_SE_CM_SO_LANDING_L);
        fopKyM_createWpillar(&current.pos, gabi::fmuls_ppc(scale.x, 1.4f), 1.4f, 0);
        mAFC = 0.0f;
        speedF = 0.0f;
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E21C0 */
void daNpc_So_c::cutAppearStart() {
    so_seStart(this, SoCutConstants::JA_SE_CM_SO_RISE);
    so_offsetAppear(this);
}

/* 022E2210 */
void daNpc_So_c::cutAppearProc() {
    if (std::fabs(gabi::fsubs_ppc(mB34, mB38.x)) < 10.0f) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E2268 */
void daNpc_So_c::cutDiveStart() { so_offsetDive(this); }

/* 022E226C */
void daNpc_So_c::cutDiveProc() {
    if (std::fabs(gabi::fsubs_ppc(mB34, mB38.x)) < 10.0f) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E22C4 */
void daNpc_So_c::cutDisappearStart() {
    so_offsetDive(this);
    so_seStart(this, SoCutConstants::JA_SE_CM_SO_DIVE);
    fopKyM_createWpillar(&current.pos, gabi::fmuls_ppc(scale.x, 1.2f), 1.4f, 0);
}

/* 022E2334 */
void daNpc_So_c::cutDisappearProc() {
    if (std::fabs(gabi::fsubs_ppc(mB34, mB38.x)) < 10.0f) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E238C */
void daNpc_So_c::cutSetAnmStart() {
    u32 name = gabi::ea(so_getMyP(mB6C, "Name", 4));
    if (name != 0) {
        if (so_strcmp(name, "WAIT") == 0) {
            mAnmPrmIdx = 1;
        } else if (so_strcmp(name, "TALK") == 0) {
            mAnmPrmIdx = 3;
        } else if (so_strcmp(name, "JUMP") == 0) {
            mAnmPrmIdx = 4;
        } else if (so_strcmp(name, "SWIM") == 0) {
            mAnmPrmIdx = 2;
        }
    } else {
        mAnmPrmIdx = 1;
    }
}

/* 022E24C0 */
void daNpc_So_c::cutSetAnmProc() {
    so_getMyP(mB6C, "Name", 4); /* unused */
    if (isAnm(1) || isAnm(4) || isAnm(3) || isAnm(2)) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
    /* mpMorf->isStop(): the frame control's stop flag, or a zero play speed */
    if (((u8)mpMorf->mFrameCtrl.mState & 1) || (f32)mpMorf->mFrameCtrl.mRate == 0.0f) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E2570 */
void daNpc_So_c::cutEffectStart() {
    cXyz pos;
    cXyz scl;
    pos.x = current.pos.x;
    pos.y = gabi::fsubs_ppc(current.pos.y, 80.0f);
    pos.z = current.pos.z;
    scl.x = 1.0f;
    scl.y = 1.0f;
    scl.z = 1.0f;
    u32 em = gabi::ea(dPa_control_set(dComIfGp_getParticle(), 0,
                                      SoCutConstants::ID_IT_SN_PF_BIKON00, &pos, nullptr, &scl,
                                      0xFF, nullptr, -1, nullptr, nullptr, nullptr));
    // pEmitter->setGlobalParticleScale(0.62f, 0.6f) (HD: a 3-vector, z = 1)
    soCutEmitter(em).mGlobalParticleScale.z = 1.0f;
    soCutEmitter(em).mGlobalParticleScale.x = 0.62f;
    soCutEmitter(em).mGlobalParticleScale.y = 0.6f;
    so_seStart(this, SoCutConstants::JA_SE_CM_CMN_NOTICE);
}

/* 022E266C */
void daNpc_So_c::cutEffectProc() { dComIfGp_evmng_cutEnd(mB6C); }

/* 022E26A4 */
void daNpc_So_c::cutEquipProc() {
    s32 id = mB6C;
    mA78 = 1;
    dComIfGp_evmng_cutEnd(id);
}

/* 022E26E4 */
void daNpc_So_c::cutUnequipProc() {
    s32 id = mB6C;
    mA78 = 0;
    dComIfGp_evmng_cutEnd(id);
}

/* 022E2724 */
void daNpc_So_c::cutEatesaStart() {
    /* GHS keeps `this` in r3 across offsetZero (known to return it) */
    daNpc_So_c* t = soCut_offsetZeroResult(this);
    t->mAFC = 0.0f;
    t->speedF = 0.0f;
}

/* 022E2754 */
void daNpc_So_c::cutEatesaProc() {
    fopAc_ac_c* esa = so_searchEsa(this);
    if (esa != nullptr) {
        static_cast<esa_class*>(esa)->field_0x298 =
            0x23; // HD: 0x23 (GameCube esa->field_0x298 = 1)
    } else {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E27C4 */
void daNpc_So_c::cutEatesaFirstStart() {
    so_offsetZero(this);
    fopAc_ac_c* ship = so_getShip();
    SoCutCirclePath* path = so_path(this);
    path->mRadius = gabi::fadds_ppc(soCutRegisters().REG8_F11, 650.0f); /* REG8_F(11) + 650.0f */
    path->mAngle = ship->shape_angle.y;
    mBDA = 1;
    mAFC = 0.0f;
    speedF = 0.0f;
}

/* 022E2830 */
void daNpc_So_c::cutEatesaFirstProc() {
    fopAc_ac_c* link = so_getLink();
    fopAc_ac_c* ship = so_getShip();
    cXyz target; /* local_28 */
    target.x = link->current.pos.x;
    target.y = link->current.pos.y;
    target.z = link->current.pos.z;
    if (ship != nullptr) {
        s32 idx = (s16)(ship->shape_angle.y - 0x4000 + soCutRegisters().REG12_S6); /* REG12_S(6) */
        target.x = ship->current.pos.x;
        target.y = ship->current.pos.y;
        f32 z = ship->current.pos.z;
        target.x = gabi::fmadds(260.0f, cM_ssin(idx), target.x);
        target.z = gabi::fmadds(260.0f, cM_scos(idx), z);
    }
    SoCutCirclePath* path = so_path(this);
    if (so_absXZ(&current.pos, &target) <
        gabi::fadds_ppc(soCutRegisters().REG8_F11, 700.0f)) { /* REG8_F(11) */
        cLib_addCalc2(&path->mRadius, 30.0f, 0.05f,
                      gabi::fadds_ppc(soCutRegisters().REG8_F10, 8.0f)); /* REG8_F(10) */
    }
    path->mAngleSpeed = 0x300;
    path->mTranslation.copy(target);
    path->mTranslation.y = so_getWaterY(&path->mTranslation, &mAcch);
    soCut_setCirclePath(path); /* dLib_setCirclePath */
    path->mPos.y = so_getWaterY(&path->mPos, &mAcch);
    cXyz goal; /* local_34 */
    goal.x = path->mPos.x;
    goal.z = path->mPos.z;
    goal.y = gabi::fsubs_ppc(path->mPos.y, 100.0f);
    f64 abs1 = so_absXZ(&current.pos, &goal);
    f64 abs2 = so_absXZ(&current.pos, &target);
    mEventCut.mbAttention = 0; /* setAttnFlag(0) */
    mEventCut.mPos.x = goal.x; /* setAttnPos(local_34) */
    mEventCut.mPos.y = goal.y;
    mEventCut.mPos.z = goal.z;
    mB44.x = goal.x;
    mB44.y = goal.y;
    mB44.z = goal.z;
    f32 spd;
    bool turn;
    if (abs2 < gabi::fadds_ppc(soCutRegisters().REG12_F7, 100.0f)) { /* REG12_F(7) + 100.0f */
        spd = 2.0f;
        turn = true;
        if (!(abs2 > 50.0f)) {
            spd = 0.0f;
            fopAc_ac_c* esa = so_searchEsa(this);
            if (esa != nullptr) {
                static_cast<esa_class*>(esa)->field_0x298 = 0x23; // HD: 0x23 (GameCube 1)
            } else {
                dComIfGp_evmng_cutEnd(mB6C);
                mBDA = 0;
                mB04 = 0.0f;
            }
            turn = false;
        }
    } else if (abs1 < gabi::fadds_ppc(soCutRegisters().REG12_F7, 200.0f)) {
        spd = 12.0f;
        turn = true;
    } else {
        spd = gabi::fadds_ppc(soCutRegisters().REG12_F10, 20.0f); /* REG12_F(10) + 20.0f */
        turn = spd != 0.0f;
    }
    if (turn) {
        s16 ang = cLib_targetAngleY(&mB54, &goal);
        cLib_addCalcAngleS2(&shape_angle.y, ang, 3, 0x1200);
    }
    soCut_addCalcPos(&current.pos, &goal, 0.1f, spd); /* cLib_addCalcPos2 */
    current.pos.y = gabi::fsubs_ppc(so_getWaterY(&current.pos, &mAcch), 100.0f);
}

/* 022E2C18 */
void daNpc_So_c::cutJumpMapopenStart() {
    mBBC = 0;
    /* GHS keeps `this` in r3 across offsetZero (known to return it) */
    daNpc_So_c* t = soCut_offsetZeroResult(this);
    so_setAnm(t, 2, false);
    mBDA = 1;
}

/* 022E2C5C */
void daNpc_So_c::cutJumpMapopenProc() {
    fopAc_ac_c* ship = so_getShip();
    if (ship == nullptr) {
        dComIfGp_evmng_cutEnd(mB6C);
        return;
    }
    if (mBBC == 0) {
        mB34 = mB38.x;
        gravity = soCutHIO().m80;
        speedF = 0.0f;
        f32 vy = soCutHIO().m84;
        mB00 = vy;
        mAFC = 0.0f;
        speed.y = vy;
        shape_angle.x = (s16)soCutHIO().m68;
        so_seStart(this, SoCutConstants::JA_SE_CM_SO_JUMP_DRAW);
        mBBC = mBBC + 1;
    } else if (mBBC == 1) {
        mBB0.x = ship->current.pos.x;
        mBB0.y = ship->current.pos.y;
        mBB0.z = ship->current.pos.z;
        u16 idx = (u16)(ship->shape_angle.y + 0x4000);
        so_offsetXZ(&mBB0, &mBB0, idx, -soCutHIO().m88);
        f32 y = current.pos.y;
        if (speed.y < 1.0) {
            speed.y = 1.0f;
        }
        if (soCut_chasePosXZ(&current.pos, &mBB0, soCutHIO().m90) != 0) { /* cLib_chasePosXZ */
            current.pos.y = y;
            if ((u8)soCutHIO().m94 == 0) {
                soCutPlay(dComIfGp_ea()).mFmapFishFlag = 2; /* dComIfGp_fmapOpenFishOn() */
            }
            shape_angle.y = shape_angle.y + 0x8000;
            mBBC = mBBC + 1;
        }
    } else if (mBBC == 2) {
        mBB0.x = ship->current.pos.x;
        mBB0.y = ship->current.pos.y;
        mBB0.z = ship->current.pos.z;
        u16 idx = (u16)(ship->shape_angle.y + 0x4000);
        so_offsetXZ(&mBB0, &mBB0, idx, -soCutHIO().m8C);
        f32 y = current.pos.y;
        if (soCut_chasePosXZ(&current.pos, &mBB0, soCutHIO().m90) != 0) {
            current.pos.y = y;
            so_seStart(this, SoCutConstants::JA_SE_CM_SO_DIVE);
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
    mB74 = 0;
    mB78 = 0;
    mB80 = 0;
    mB7C = 0;
    so_offsetDive(this);
    /* dComIfGp_startMiniGame(8) */
    dComIfGp_startMiniGame(8, 0x80);
    mDoAud_seStart(SoCutConstants::JA_SE_START_WHISTLE, nullptr, 0, 0);
    fopAc_ac_c* link = so_getLink();
    u32 cam = so_getCamera();
    if (soCutRegisters().REG8_S1 == 0) { /* REG8_S(1) */
        so_camAroundLink(this, link);
        cXyz center;
        cXyz eye;
        center.x = mBCC.x;
        center.y = mBCC.y;
        center.z = mBCC.z;
        eye.x = mBC0.x;
        eye.y = mBC0.y;
        eye.z = mBC0.z;
        so_camReset(cam, &center, &eye);
        so_camStart(cam);
    }
    so_changeOriginalDemo(link);
    so_changeDemoMode(link, SoCutConstants::DEMO_BOW_MINIGAME_e);
    mBDB = 0;
    mBDE = 0;
}

/* 022E3158 */
void daNpc_So_c::initCam() {
    if (mBDE == 0) { /* GameCube: mBDE != 1 */
        mBDE = 1;
        u32 cam = so_getCamera();
        if (soCutRegisters().REG8_S1 == 0) { /* REG8_S(1) */
            so_camStop(cam);
            so_camSetTrimSize(cam, 1);
            so_camAroundLink(this, so_getLink());
        }
    }
}

/* 022E32AC */
void daNpc_So_c::moveCam() {
    if (soCutRegisters().REG8_S1 == 0) { /* REG8_S(1) */
        u32 cam = so_getCamera();
        fopAc_ac_c* link = so_getLink();
        cXyz eye;    /* local_2C */
        cXyz center; /* local_20 */
        center.x = gabi::fmadds(150.0f, cM_ssin((u16)link->shape_angle.y), link->current.pos.x);
        center.y = gabi::fadds_ppc(link->current.pos.y, 100.0f);
        center.z = gabi::fmadds(150.0f, cM_scos((u16)link->shape_angle.y), link->current.pos.z);
        eye.x = gabi::fmadds(-400.0f, cM_ssin((u16)link->shape_angle.y), link->current.pos.x);
        eye.y = gabi::fadds_ppc(link->current.pos.y, 300.0f);
        eye.z = gabi::fmadds(-400.0f, cM_scos((u16)link->shape_angle.y), link->current.pos.z);
        soCut_addCalcPos(&mBC0, &eye, 0.05f, 5.0f); /* cLib_addCalcPos2 */
        soCut_addCalcPos(&mBCC, &center, 0.05f, 5.0f);
        cXyz c;
        cXyz e;
        c.x = mBCC.x;
        c.y = mBCC.y;
        e.x = mBC0.x;
        e.z = mBC0.z;
        c.z = mBCC.z;
        e.y = mBC0.y;
        so_camSet(cam, &c, &e);
    }
}

/* ---- cutMiniGameProc (022E3484) helpers ---- */
/* dComIfGp_endMiniGame(8) */
static inline void so_endMiniGame() { dComIfGp_endMiniGame(0x80); }
/* the end of the mini-game: the player back to talk, the camera on Link, the whistle */
static inline void so_miniGameFinish(daNpc_So_c* t, fopAc_ac_c* player) {
    so_changeDemoMode(player, SoCutConstants::DEMO_N_TALK_e);
    so_changeDemoParam0(player, 1);
    t->initCam();
    t->moveCam();
    mDoAud_seStart(SoCutConstants::JA_SE_END_WHISTLE, nullptr, 0, 0);
    so_endMiniGame();
    s32 id = t->mB6C;
    t->mAFC = 0.0f;
    t->speedF = 0.0f;
    dComIfGp_evmng_cutEnd(id);
}
/* the next attention point: (mB88, 0, mB8C) turned by angY, plus base */
/* inlined into Fishman cuts */
static inline void so_miniGameAttnPos(daNpc_So_c* t, s16 angY, cXyz* base, f32 x, f32 z) {
    cXyz ofs;
    cXyz out;
    ofs.x = x;
    ofs.y = 0.0f;
    ofs.z = z;
    out.x = 0.0f;
    out.y = 0.0f;
    out.z = 0.0f;
    soCut_matrixYrot(gabi::ea(mDoMtx_stack_c::get()), angY);          /* mDoMtx_YrotS(now, angY) */
    soCut_matrixMultVec(gabi::ea(mDoMtx_stack_c::get()), &ofs, &out); /* PSMTXMultVec */
    soCut_vectorAdd(&out, base, &out);                                /* PSVECAdd */
    t->mEventCut.mPos.copy(out); /* mEventCut.setAttnPos (word copy) */
}
/* turn towards the attention point; returns the XZ distance (unrounded) */
static inline f64 so_miniGameTurn(daNpc_So_c* t, cXyz* p) {
    f64 abs = so_absXZ(&t->current.pos, p);
    s16 ang = cLib_targetAngleY(&t->current.pos, p);
    cLib_addCalcAngleS2(&t->shape_angle.y, ang, 8, 0x400);
    return abs;
}
/* case 7: the jump's hit sphere and the landing */
/* inlined into Fishman cuts */
static inline void so_miniGameJump(daNpc_So_c* t, fopAc_ac_c* player, bool done) {
    f32 water = so_getWaterY(&t->current.pos, &t->mAcch);
    if (t->mB84 == 0 && t->mB7C < 10) {
        if (t->m6D8 == 0) {
            cM3dGSph* sph = &t->mSph.mSph; /* mSph's cM3dGSph */
            if (!(t->current.pos.y < gabi::fadds_ppc(water, 50.0f))) {
                soCut_sphereRadius(sph, soCutHIO().m3C);  /* SetR(l_HIO.m3C) */
                soCut_sphereCenter(sph, &t->current.pos); /* SetC */
                dComIfG_Ccsp_Set(&t->mSph);
            } else {
                cXyz c;
                c.z = 0.0f;
                c.x = 0.0f;
                c.y = 30000.0f;
                soCut_sphereRadius(sph, 0.0f);
                soCut_sphereCenter(sph, &c);
                dComIfG_Ccsp_Set(&t->mSph);
            }
        }
        if (soCut_checkTargetHit(t)) { /* checkTgHit */
            s32 n = t->mB7C + 1;
            t->mB7C = n < 10 ? n : 10;
        }
    }
    if (!(t->current.pos.y > water)) {
        so_seStart(t, SoCutConstants::JA_SE_CM_SO_LANDING_L);
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
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    s16 angY = player->shape_angle.y;
    cXyz base; /* local_34 */
    base.x = player->current.pos.x;
    base.y = player->current.pos.y;
    base.z = player->current.pos.z;
    bool done = false;
    dComIfGp_ea();
    dComIfGp_ea();
    fopAc_ac_c* ship = so_getShip(); /* dComIfGp_getShipActor() */
    if (ship != nullptr) {
        angY = (s16)(ship->shape_angle.y - 0x4000 + soCutRegisters().REG12_S6); /* REG12_S(6) */
        base.copy(ship->current.pos);
    }
    if ((u32)soCutPlayer(player).mStatus & 0x20000000) { /* player->checkArrowShoot() */
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
        so_miniGameAttnPos(this, angY, &base, x, mB8C);
        mB74 = mB74 + 1;
        so_offsetSwim(this);
        break;
    }
    case 3: {
        cXyz p;
        p.x = mEventCut.mPos.x;
        p.y = mEventCut.mPos.y;
        p.z = mEventCut.mPos.z;
        mAFC = gabi::fadds_ppc(soCutRegisters().REG12_F10, 12.0f); /* REG12_F(10) + 12.0f */
        f64 abs = so_miniGameTurn(this, &p);
        if (abs < gabi::fadds_ppc(soCutRegisters().REG12_F7, 150.0f)) { /* REG12_F(7) + 150.0f */
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
        so_miniGameAttnPos(this, angY, &base, x, z);
        mAFC = 0.0f;
        mB74 = mB74 + 1;
        speedF = 0.0f;
        mA9C = 30;
        so_offsetSwim(this);
        break;
    }
    case 5: {
        cXyz p;
        p.y = mEventCut.mPos.y;
        p.z = mEventCut.mPos.z;
        p.x = mEventCut.mPos.x;
        mAFC = 0.0f;
        so_miniGameTurn(this, &p);
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
        f32 mx = soCutHIO().m50;
        if (vy > mx) { /* ble: a NaN keeps vy */
            speed.y = mx;
        }
        gravity = -0.8f;
        so_setAnm(this, 4, false);
        shape_angle.x = (s16)soCutHIO().m68;
        so_offsetZero(this);
        mB74 = mB74 + 1;
        so_seStart(this, SoCutConstants::JA_SE_CM_SO_JUMP_L);
        break;
    }
    case 7:
        so_miniGameJump(this, player, done);
        break;
    }
}

/* 022E3CF0 */
void daNpc_So_c::cutTurnStart() { mA9C = 30; }

/* 022E3CFC */
void daNpc_So_c::cutTurnProc() {
    fopAc_ac_c* ship = so_getShip();
    if (ship == nullptr) {
        dComIfGp_evmng_cutEnd(mB6C);
        return;
    }
    u16 idx = (u16)(ship->shape_angle.y + 0x4000);
    cXyz p;
    p.x = ship->current.pos.x;
    p.y = ship->current.pos.y;
    f32 z = ship->current.pos.z;
    p.x = gabi::fmadds(-300.0f, cM_ssin(idx), p.x);
    p.z = gabi::fmadds(-300.0f, cM_scos(idx), z);
    s16 target = cLib_targetAngleY(&current.pos, &p);
    cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
    s32 dist = soCut_distanceAngle((s16)shape_angle.y,
                                   target); /* cLib_distanceAngleS (the whole r3 is compared) */
    if (so_calcTimerI(&mA9C) == 0 || dist <= 0x400) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* daPy_py_c::setPlayerPosAndAngle (virtual, HD vtable slot 0x114) */
static inline void so_setPlayerPosAndAngle(fopAc_ac_c* pl, cXyz* pos, s16 ang) {
    u32 fn = (u32)soCutPlayerVtable(pl->__vtbl).setPlayerPosAndAngle;
    gabi::call_ptr(fn, pl, pos, ang);
}
/* daShip_c::initStartPos( &pos, angle) */
static inline void so_shipInitStartPos(fopAc_ac_c* ship, cXyz* pos, s16 ang) {
    soCut_shipInitStartPos(ship, pos, ang);
}
/* the actor 300 units beside the ship (cutMiniGameWarp/PlTurn/Return) */
static inline void so_besideShip(daNpc_So_c* t, fopAc_ac_c* ship) {
    u16 idx = (u16)(ship->shape_angle.y - 0x4000 + soCutRegisters().REG12_S6); /* REG12_S(6) */
    f32 x = ship->current.pos.x;
    t->current.pos.x = x;
    t->current.pos.y = ship->current.pos.y;
    f32 z = ship->current.pos.z;
    t->current.pos.z = z;
    t->current.pos.x = gabi::fmadds(300.0f, cM_ssin(idx), x);
    t->current.pos.z = gabi::fmadds(300.0f, cM_scos(idx), z);
}
/* inlined into Fishman cuts */
static inline void so_camResetStart(daNpc_So_c* t) {
    u32 cam = so_getCamera();
    cXyz center;
    cXyz eye;
    center.x = t->mBCC.x;
    center.y = t->mBCC.y;
    center.z = t->mBCC.z;
    eye.x = t->mBC0.x;
    eye.y = t->mBC0.y;
    eye.z = t->mBC0.z;
    so_camReset(cam, &center, &eye);
    so_camStart(cam);
}

/* 022E3E08 */
void daNpc_So_c::cutMiniGameWarpStart() {
    soCut_soundMiniGame(1); /* JAIZelBasic::zel_basic->field_0x00bf = 1 */
    fopAc_ac_c* link = so_getLink();
    so_changeOriginalDemo(link);
    fopAc_ac_c* ship = so_getShip();
    mB90.y = so_getWaterY(&mB90, &mAcch);
    mBA0.copy(ship->current.pos);
    mBAC = ship->shape_angle.y;
    so_shipInitStartPos(ship, &mB90, (s16)(mB9C + 0x4000));
    so_setPlayerPosAndAngle(link, &mB90, (s16)(mB9C + soCutRegisters().REG8_S6)); /* REG8_S(6) */
    speedF = 0.0f;
    mAFC = 0.0f;
    u16 idx = (u16)(ship->shape_angle.y - 0x4000 + soCutRegisters().REG12_S6); /* REG12_S(6) */
    f32 x = mB90.x;
    current.pos.x = x;
    current.pos.y = mB90.y;
    f32 z = mB90.z;
    current.pos.z = z;
    current.pos.x = gabi::fmadds(300.0f, cM_ssin(idx), x);
    mBDB = 1;
    current.pos.z = gabi::fmadds(300.0f, cM_scos(idx), z);
    mA9C = soCutRegisters().REG12_S1 + 10; /* REG12_S(1) + 10 */
    so_camResetStart(this);
}

/* 022E3FD0 */
void daNpc_So_c::cutMiniGameWarpProc() {
    fopAc_ac_c* link = so_getLink();
    so_changeOriginalDemo(link);
    fopAc_ac_c* ship = so_getShip();
    so_changeDemoParam0(link, 1);
    so_changeDemoMode(link, SoCutConstants::DEMO_N_TALK_e);
    so_besideShip(this, ship);
    if (so_calcTimerI(&mA9C) == 0) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E40B8 */
void daNpc_So_c::cutMiniGameReturnStart() {
    soCut_soundMiniGame(0); /* JAIZelBasic::zel_basic->field_0x00bf = 0 */
    fopAc_ac_c* link = so_getLink();
    so_changeOriginalDemo(link);
    /* fopAcM_SearchByName(fpcNm_SHIP_e) */
    SoCutProcessName name;
    name = SoCutConstants::fpcNm_SHIP_e;
    fopAc_ac_c* ship = static_cast<fopAc_ac_c*>(fopAcM_Search(soCut_fpcSch_JudgeForPName, &name));
    if (ship == nullptr) {
        soCut_assertion("d_a_npc_so_dproc.inc", 0x3BA, "ship != (0)"); // HD: JUT_ASSERT(ship)
    }
    so_setPlayerPosAndAngle(link, &mBA0, (s16)(mB9C + soCutRegisters().REG8_S6)); /* REG8_S(6) */
    if (ship != nullptr) {
        so_shipInitStartPos(ship, &mBA0, mBAC);
    }
    mBDB = 1;
    mAFC = 0.0f;
    speedF = 0.0f;
    so_setAnm(this, 2, false);
    so_offsetDive(this);
}

/* the camera behind Link (cutMiniGameReturnProc, cutMiniGamePlUpProc) */
/* inlined into Fishman cuts */
static inline void so_camBehindLink(daNpc_So_c* t, fopAc_ac_c* link) {
    cXyz ofs; /* local_1C */
    cXyz out; /* local_28 */
    out.z = 0.0f;
    ofs.x = 0.0f;
    out.y = 0.0f;
    ofs.y = 0.0f;
    out.x = 0.0f;
    ofs.z = 0.0f;
    soCut_matrixYrot(soCutCalcMtx() /* calc_mtx */, (s16)link->shape_angle.y); /* mDoMtx_YrotS */
    ofs.x = gabi::fadds_ppc(soCutRegisters().REG0_F4, -50.0f);                 /* REG0_F(4) */
    ofs.y = gabi::fadds_ppc(soCutRegisters().REG0_F5, 50.0f);                  /* REG0_F(5) */
    ofs.z = gabi::fadds_ppc(soCutRegisters().REG0_F6, 100.0f);                 /* REG0_F(6) */
    soCut_matrixPosition(&ofs, &out);                                          /* MtxPosition */
    cXyz eye;
    soCut_add(&link->current.pos, &eye, &out); /* cXyz::operator+ */
    t->mBC0.copy(eye);
    t->mBCC.x = link->current.pos.x;
    f32 y = link->current.pos.y;
    t->mBCC.y = y;
    t->mBCC.z = link->current.pos.z;
    t->mBCC.y = gabi::fadds_ppc(
        y, gabi::fadds_ppc(soCutRegisters().REG0_F7, 90.0f)); /* REG0_F(7) + 90.0f */
}

/* 022E41BC */
void daNpc_So_c::cutMiniGameReturnProc() {
    fopAc_ac_c* ship = so_getShip();
    so_besideShip(this, ship);
    shape_angle.x = 0;
    if (so_absXZ(&mBA0, &current.pos) < 400.0f) {
        fopAc_ac_c* link = so_getLink();
        so_changeDemoMode(link, SoCutConstants::DEMO_N_TALK_e);
        so_changeOriginalDemo(link);
        so_changeDemoParam0(link, 1);
        so_camBehindLink(this, link);
        so_camResetStart(this);
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E442C */
void daNpc_So_c::cutPartnerShipProc() {
    fopAc_ac_c* ship = so_getShip();
    /* dComIfGp_event_setTalkPartner(ship): mPtTalk = getPId(ship) */
    dComIfGp_event_setTalkPartner(ship);
    dComIfGp_evmng_cutEnd(mB6C);
}

/* 022E4498 */
void daNpc_So_c::cutMiniGameWaitStart() {
    u32 cam = so_getCamera();
    if (soCutRegisters().REG8_S1 == 0) { /* REG8_S(1) */
        so_camStop(cam);
        so_camSetTrimSize(cam, 1);
    }
}

/* 022E44FC */
void daNpc_So_c::cutMiniGameWaitProc() {
    dComIfGp_ea(); // HD: a dead camera lookup (two play getter calls)
    dComIfGp_ea();
    dComIfGp_evmng_cutEnd(mB6C);
}

/* 022E4540 */
void daNpc_So_c::cutMiniGameEndStart() {
    mBDE = 0;
    mA9C = soCutRegisters().REG12_S0 + 0x2D; /* REG12_S(0) + 45 */
}

/* 022E455C */
void daNpc_So_c::cutMiniGameEndProc() {
    initCam();
    moveCam();
    if (so_calcTimerI(&mA9C) == 0) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E45B4 */
void daNpc_So_c::cutMiniGamePlTurnStart() {
    mBDE = 0;
    mA9C = soCutRegisters().REG12_S2 + 10; /* REG12_S(2) + 10 */
}

/* 022E45D0 */
void daNpc_So_c::cutMiniGamePlTurnProc() {
    fopAc_ac_c* link = so_getLink();
    so_changeOriginalDemo(link);
    fopAc_ac_c* ship = so_getShip();
    so_changeDemoParam0(link, 1);
    so_changeDemoMode(link, SoCutConstants::DEMO_N_TALK_e);
    so_besideShip(this, ship);
    if (so_calcTimerI(&mA9C) == 0) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* 022E46B8 */
void daNpc_So_c::cutMiniGamePlUpStart() {
    mA9C = 30;
    u32 cam = so_getCamera();
    so_camStop(cam);
    so_camSetTrimSize(cam, 1);
}

/* 022E4714 */
void daNpc_So_c::cutMiniGamePlUpProc() {
    fopAc_ac_c* link = so_getLink();
    so_changeDemoMode(link, SoCutConstants::DEMO_N_TALK_e);
    so_camBehindLink(this, link);
    u32 cam = so_getCamera();
    cXyz center;
    cXyz eye;
    center.x = mBCC.x;
    center.y = mBCC.y;
    center.z = mBCC.z;
    eye.x = mBC0.x;
    eye.y = mBC0.y;
    eye.z = mBC0.z;
    so_camSet(cam, &center, &eye);
    if (so_calcTimerI(&mA9C) == 0) {
        dComIfGp_evmng_cutEnd(mB6C);
    }
}

/* WWHD glue (tools/verify/autoglue.py): Fishman event cuts
ext 022DDCA0 void* soCut_searchEsa_CB(void*, void*)
ext 025E121C void* soCut_fpcSch_JudgeForPName(void*, void*)
str 100222B8 "Speed_y"
str 100222C0 "SpeedF"
str 100222C8 "Name" in 022E238C
str 100222D0 "WAIT"
str 100222D8 "TALK"
str 100222E0 "JUMP"
str 100222E8 "SWIM"
str 100222F0 "Name" in 022E24C0
str 10022350 "d_a_npc_so_dproc.inc"
str 10022344 "ship != (0)"
ext 022DE6AC void soCut_offsetZero(daNpc_So_c*)
ext 022DE6AC daNpc_So_c* soCut_offsetZeroResult(daNpc_So_c*)
ext 022E005C void soCut_offsetDive(daNpc_So_c*)
ext 022E0AB4 void soCut_offsetSwim(daNpc_So_c*)
ext 022E0020 void soCut_offsetAppear(daNpc_So_c*)
ext 022DE6D4 void soCut_setAnm(daNpc_So_c*, s8, bool)
ext 025871F8 f32 soCut_getWaterY(cXyz*, dBgS_ObjAcch*)
ext 0211D2F8 s32 soCut_calcTimer(be<s32>*)
ext 0201ADE0 void soCut_subtract(cXyz*, cXyz*, cXyz*)
ext 028E8DD0 f64 soCut_squareMagnitude(cXyz*)
ext 028F4384 f64 soCut_sqrtUnrounded(f64)
ext 0251510C void soCut_resetCamera(u32, cXyz*, cXyz*)
ext 02514F38 void soCut_startCamera(u32)
ext 02514F2C void soCut_stopCamera(u32)
ext 02515280 void soCut_trimCamera(u32, s32)
ext 02514F50 void soCut_setCamera(u32, cXyz*, cXyz*)
ext 02587128 void soCut_setCirclePath(SoCutCirclePath*)
ext 0200F164 void soCut_addCalcPos(cXyz*, cXyz*, f32, f32)
ext 0200F764 BOOL soCut_chasePosXZ(cXyz*, cXyz*, f32)
ext 025F1884 void soCut_matrixYrot(u32, s16)
ext 028E8F64 void soCut_matrixMultVec(u32, cXyz*, cXyz*)
ext 028E8D88 void soCut_vectorAdd(cXyz*, cXyz*, cXyz*)
ext 02018C8C void soCut_sphereRadius(cM3dGSph*, f32)
ext 02018D40 void soCut_sphereCenter(cM3dGSph*, cXyz*)
ext 022E160C bool soCut_checkTargetHit(daNpc_So_c*)
ext 0200FAAC s32 soCut_distanceAngle(s16, s16)
ext 024832E0 void soCut_shipInitStartPos(fopAc_ac_c*, cXyz*, s16)
ext 025E201C void soCut_soundMiniGame(s32)
ext 0273AA24 void soCut_assertion(const char*, s32, const char*)
ext 0200FCD8 void soCut_matrixPosition(cXyz*, cXyz*)
ext 0201AD78 void soCut_add(cXyz*, cXyz*, cXyz*)
*/

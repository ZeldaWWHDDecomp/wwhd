//
// d_a_npc_so_mode.cpp (WWHD)
// NPC - Fishman: draw, offsets, modes, hit check, messages
//
// Ported from the GameCube decompilation
// (zeldaret/tww src/d/actor/d_a_npc_so.cpp) to the WWHD layout and code, verified against
// cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
#include "d/actor/d_a_npc_so_mode.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"

// Bindings used by the HD mode implementation.
static inline u32 so_play() { return dComIfGp_ea(); }
static inline fopAc_ac_c* so_player() { return dComIfGp_getPlayer(0); }
static inline fopAc_ac_c* so_ship() { return dComIfGp_getShipActor(); } /* dComIfGp_getShipActor */
static inline u32 so_playerStatus0() { return dComIfGp_getPlayerStatus0(0); }
// fopAcM_seStart (inline, HD: no NULL checks): (id, &eyePos, param, reverb)
static inline void so_seStart(daNpc_So_c* t, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(t));
    soMode_seStart(id, &t->eyePos, param, reverb);
}
/* fopAcM_monsSeStart: (id, &eyePos, base id (this+4), param, reverb) */
static inline void so_monsSeStart(daNpc_So_c* t, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(t));
    soMode_monsSeStart(id, &t->eyePos, (u32)t->mBsPcId, param, reverb);
}
static inline dSv_event_c* so_event() { return dComIfGs_getEvent(); }
static inline BOOL so_isEventBit(u16 f) { return dSv_event_isEventBit(so_event(), f); }
static inline void so_onEventBit(u16 f) { dSv_event_onEventBit(so_event(), f); }
/* dComIfGp_event_onEventFlag(8): play+0x52B8 |= 8 */
static inline void so_event_onEventFlag8() { dComIfGp_event_onEventFlag(8); }
static inline BOOL so_endCheckOld(const char* name) {
    return soMode_endCheckOld(dComIfGp_ea() + 0x52C4, name);
}
static inline BOOL so_endCheck(s16 idx) { return soMode_endCheck(dComIfGp_ea() + 0x52C4, idx); }
static inline f64 so_getWaterY(u32 pos, u32 acch) {
    return soMode_getWaterY(pos, acch);
} /* dLib_getWaterY */
static inline void so_setCirclePath(u32 path) {
    soMode_setCirclePath(path);
} /* dLib_setCirclePath */
static inline void cXyz_mi(u32 a, u32 out, u32 b) { soMode_subtract(a, out, b); }
static inline void cXyz_ml(u32 a, u32 out, f32 s) { soMode_multiply(a, out, s); }
static inline void cXyz_pl(u32 a, u32 out, u32 b) { soMode_add(a, out, b); }
/* (a - b).absXZ(): f1 of sqrtf is used unrounded */
/* inlined into modeJump, modeSwim, modeNearSwim, modeEventFirstWait */
static inline f64 so_absXZ(u32 a, u32 b) {
    cXyz d;
    cXyz_mi(a, gabi::ea(&d), b);
    cXyz v;
    v.x = (f32)d.x;
    v.y = 0.0f;
    v.z = (f32)d.z;
    f64 sq = soMode_squareMagnitude(&v); /* PSVECSquareMag */
    return soMode_sqrtUnrounded(sq);     /* std::sqrtf */
}
static inline s32 so_calcTimerI(be<s32>* t) {
    return soMode_calcTimer(t);
} /* cLib_calcTimer<int> */
/* the other parts of the unit (P1): setAnm , modeProc  */
static inline void so_setAnm(daNpc_So_c* t, s8 idx, bool force) { soMode_setAnm(t, idx, force); }
static inline void so_modeProcInit(daNpc_So_c* t, int mode) {
    soMode_modeProc(t, 0 /* PROC_INIT_e */, mode);
}
static inline void so_word_copy3(u32 dst, u32 src) {
    soModeWords(dst).x = (u32)soModeWords(src).x;
    soModeWords(dst).y = (u32)soModeWords(src).y;
    soModeWords(dst).z = (u32)soModeWords(src).z;
}
// strcmp(dComIfGp_getStartStageName(), lit) == 0, HD: sead::SafeString operator==
/* inlined into next_msgStatus, getMsg */
static inline bool so_isStartStage(const char* lit) {
    SafeString sa;
    sa.mStringTop = gabi::guest_addr(lit);
    sa.__vtbl = SoModeSafeStringVtable::ADDRESS;
    u32 stage = dComIfGp_ea() + 0x5134;
    SafeString sb;
    sb.mStringTop = stage;
    sb.__vtbl = SoModeSafeStringVtable::ADDRESS;
    soModeAssureTermination(&sa);
    soModeAssureTermination(&sa);
    u32 pa = sa.mStringTop;
    soModeAssureTermination(&sb);
    u32 pb = sb.mStringTop;
    if (pa == pb)
        return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = (u8)soModeBytes(pa)[i];
        if (ca != (u8)soModeBytes(pb)[i])
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* dLib_circle_path_c (0x24): mTranslation 0x00, mPos 0x0C, mRadius 0x18, mWobbleAmplitude 0x1C,
 * mAngle 0x20, mAngleSpeed 0x22 */
static inline SoModeCirclePath* so_path(daNpc_So_c* t) {
    return reinterpret_cast<SoModeCirclePath*>(t->mB10);
}

/* ====================================================================== */

// HD: Debug shapes are compiled out; the guarded colors are still initialized.
/* 022DFBD8 */
void daNpc_So_c::debugDraw() {
    static u32 offset_guard;
    static u32 target_guard;
    static u32 circle_guard;
    static u32 attention_guard;
    static u32 line_guard;
    static u32 offset_color;
    static u32 target_color;
    static u32 circle_color;
    static u32 attention_color;
    static u32 line_color;
    static u32 offset_color_default;
    static u32 target_color_default;
    static u32 circle_color_default;
    static u32 attention_color_default;
    static u32 line_color_default;
    dComIfGp_ea(); /* dComIfGp_getPlayer(0) (result unused) */
    auto init = [](be<u32>* guard, be<u32>* dst, be<u32>* src) {
        if (*guard == 0) {
            *guard = 1;
            soMode_copyColor(gabi::ea(dst), gabi::ea(src), 4); /* memcpy */
        }
    };
    init(&offset_guard, &offset_color, &offset_color_default);
    init(&target_guard, &target_color, &target_color_default);
    init(&offset_guard, &offset_color, &offset_color_default);
    init(&target_guard, &target_color, &target_color_default);
    init(&offset_guard, &offset_color, &offset_color_default);
    init(&target_guard, &target_color, &target_color_default);
    init(&circle_guard, &circle_color, &circle_color_default);
    init(&attention_guard, &attention_color, &attention_color_default);
    init(&attention_guard, &attention_color, &attention_color_default);
    init(&line_guard, &line_color, &line_color_default);
    init(&line_guard, &line_color, &line_color_default);
}

/* 022DFDFC */
void daNpc_So_c::hudeDraw() {
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel.get(), &tevStr);
    // mDoMtx_stack_c::copy(mpMorf->getModel()->getAnmMtx(11)) (HD: marks the joint matrices dirty)
    J3DModel* model = mpMorf->getModel();
    J3DMtxBuffer_hd* blk = model->mpMtxBuffer;
    u16 flags = blk->mFlags;
    u32 mtx = blk->mpAnmMtx;
    blk->mFlags = flags | 0x10;
    soMode_matrixCopy(mtx + 11 * 0x30, gabi::ea(mDoMtx_stack_c::get())); /* PSMTXCopy */
    /* mpModel->setBaseTRMtx(mDoMtx_stack_c::get()): lfs/stfs copy */
    mpModel->setBaseTRMtx(mDoMtx_stack_c::get());
    mDoExt_modelUpdateDL(mpModel.get(), 0);
}

/* 022DFEDC */
bool daNpc_So_c::_draw() {
    if (soModeHIO().debugDraw != 0) { /* soModeHIO().mNpc.m22 */
        debugDraw();
    }
    if (m6CC == 5) {
        return true;
    }
    if (soModeHIO().m31 == 0) {
        J3DModel* model = mpMorf->getModel();
        J3DModelData* modelData = model->getModelData();
        settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), model, &tevStr);
        soMode_enterBtp(mBtpAnm, gabi::ea(modelData),
                        (s16)m86C); /* mBtpAnm.entry(modelData, m86C) */
        mpMorf->entryDL();
        soModeModelData(modelData).mpTexNoAnimator = 0; /* removeTexNoAnimator (HD inline) */
        if (mA78 != 0 || soModeHIO().m2C != 0) {
            hudeDraw();
        }
    }
    cXyz tmp;
    f32 y = gabi::fadds_ppc(current.pos.y, mB34);
    tmp.x = (f32)current.pos.x;
    tmp.z = (f32)current.pos.z;
    tmp.y = y;
    soMode_registerFigure(0x7D, this, &tmp, (s16)shape_angle.y, 1.0f, 1.0f,
                          1.0f); /* dSnap_RegistFig */
    // HD: no dComIfGd_setShadow; a second morf (0xBEC) is drawn while 0xBF0 is clear
    if (mBF0 == 0) {
        soMode_enterMorf(mA74.get()); /* mDoExt_McaMorf::entryDL */
    }
    return true;
}

/* 022E001C */
static BOOL daNpc_SoDraw(void* i_this) { return ((daNpc_So_c*)i_this)->_draw(); }

/* 022E0020 */
void daNpc_So_c::offsetAppear() {
    mB38.x = 110.0f;
    mB38.y = 22.0f;
    mB38.z = 0.4f;
}

/* 022E0048 */
void daNpc_So_c::modeWaitInit() {
    speedF = 0.0f;
    mAFC = 0.0f;
    offsetAppear();
}

/* 022E005C */
void daNpc_So_c::offsetDive() {
    mB38.x = -150.0f;
    mB38.y = 10.0f;
    mB38.z = 0.3f;
}

/* 022E0084 */
void daNpc_So_c::modeHideInit() {
    mBDB = 1;
    offsetDive();
    mA7C = 0.0f;
}

/* 022E00B8 */
void daNpc_So_c::modeHide() {
    so_word_copy3(gabi::ea(&current.pos), gabi::ea(&mA80)); /* current.pos = mA80 */
}

/* 022E00D4 */
void daNpc_So_c::modeJumpInit() {
    f32 r = cM_rndF(5.0f);
    f32 b08 = mB08;
    f32 spd = gabi::fmuls_ppc(gabi::fadds_ppc(r, 5.0f), b08);
    f32 sp4 = gabi::fmuls_ppc(spd, 4.0f);
    f32 vy = gabi::fmadds(30.0f, b08, sp4);
    mAFC = spd;
    speedF = spd;
    speed.y = vy;
    f32 mx = soModeHIO().m50;
    if (vy > mx) {
        vy = mx;
        speed.y = vy;
    }
    mB00 = vy;
    shape_angle.x = soModeHIO().m68;
    so_setAnm(this, 4, false);
    m_jnt.mbBackBoneLock = 1;
    so_seStart(this, SoModeConstants::JA_SE_CM_SO_JUMP_L, 0);
    soMode_createWaterPillar(&current.pos, gabi::fmuls_ppc(scale.x, 0.8f), 1.4f,
                             0); // HD: fopKyM_createWpillar at the jump
}

/* 022E01D8 */
void daNpc_So_c::modeJump() {
    // HD: while the player is near, the camera mode follows the Fishman
    fopAc_ac_c* pl = so_player();
    f64 d = so_absXZ(gabi::ea(&pl->current.pos), gabi::ea(&mA80));
    if (d < mA7C) {
        mB70 = 2;
    }
    f64 wy = so_getWaterY(gabi::ea(&current.pos), gabi::ea(&mAcch));
    if (current.pos.y < wy) {
        so_seStart(this, SoModeConstants::JA_SE_CM_SO_LANDING_L, 0);
        soMode_createWaterPillar(&current.pos, gabi::fmuls_ppc(scale.x, 1.4f), 1.4f,
                                 0); /* fopKyM_createWpillar */
        f64 d2 = so_absXZ(gabi::ea(&mA80), gabi::ea(&current.pos));
        if (d2 > mA7C) {
            u32 dst = gabi::ea(&current.pos), src = gabi::ea(&mA80);
            u32 z = soModeWords(src).z, y = soModeWords(src).y;
            soModeWords(dst).z = z;
            soModeWords(dst).y = y;
            soModeWords(dst).x = (u32)soModeWords(src).x;
        }
        so_modeProcInit(this, MODE_SWIM_e);
    }
}

/* 022E0324 */
void daNpc_So_c::modeSwimInit() {
    attention_info.flags = fopAc_Attn_TALKFLAG_NOTALK_e | fopAc_Attn_ACTION_SPEAK_e |
                           fopAc_Attn_LOCKON_TALK_e; /* attention_info.flags */
    mA90 = gabi::ftoi(gabi::fadds_ppc(cM_rndF(90.0f), 30.0f));
    so_setAnm(this, 2, false);
    m_jnt.mbBackBoneLock = 1;
    offsetDive();
}

/* 022E03A4 */
void daNpc_So_c::modeSwim() {
    mBDB = 0;
    fopAc_ac_c* pl = so_player();
    f64 abs = so_absXZ(gabi::ea(&pl->current.pos), gabi::ea(&mA80));
    fopAc_ac_c* ship = so_ship();
    if (ship == nullptr)
        return;
    SoModeCirclePath* path = so_path(this);
    cLib_addCalc2(&path->mRadius, 200.0f, 0.1f, 10.0f);
    f32 ty = mA80.y, tz = mA80.z;
    path->mTranslation.z = tz;
    path->mAngleSpeed = 0x150;
    f32 tx = mA80.x;
    path->mTranslation.y = ty;
    path->mTranslation.x = tx;
    path->mWobbleAmplitude = 50.0f;
    path->mTranslation.y = (f32)so_getWaterY(gabi::ea(path), gabi::ea(&mAcch));
    so_setCirclePath(gabi::ea(path));
    f32 wy = (f32)so_getWaterY(gabi::ea(&path->mPos), gabi::ea(&mAcch));
    path->mPos.y = gabi::fadds_ppc(wy, mB34);
    f64 d2 = so_absXZ(gabi::ea(&current.pos), gabi::ea(&path->mPos));
    bool rideCheck;
    if (d2 > 150.0f || ship->speedF > 10.0f) {
        mAFC = 12.0f;
        s16 target = cLib_targetAngleY(&current.pos, &path->mPos);
        cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
        f32 a7c = mA7C;
        mB04 = 0.0f;
        rideCheck = abs < a7c;
    } else {
        cLib_addCalc2(&mB04, 1.0f, 0.01f, 0.05f);
        cXyz d;
        cXyz_mi(gabi::ea(&path->mPos), gabi::ea(&d), gabi::ea(&current.pos));
        cXyz m;
        cXyz_ml(gabi::ea(&d), gabi::ea(&m), mB04);
        cXyz p;
        cXyz_pl(gabi::ea(&current.pos), gabi::ea(&p), gabi::ea(&m));
        s16 ang = (s16)((s16)path->mAngle + 0x8000);
        so_word_copy3(gabi::ea(&current.pos), gabi::ea(&p));
        cLib_addCalcAngleS2(&shape_angle.y, ang, 4, 0x400);
        rideCheck = abs < mA7C;
    }
    if (rideCheck && (so_playerStatus0() & SoModeConstants::daPyStts0_SHIP_RIDE_e)) {
        so_modeProcInit(this, MODE_NEAR_SWIM_e);
    } else if (so_calcTimerI(&mA90) == 0) {
        so_modeProcInit(this, MODE_JUMP_e);
    }
}

/* 022E0660 */
void daNpc_So_c::modeNearSwimInit() {
    so_setAnm(this, 2, false);
    offsetDive();
    m_jnt.mbBackBoneLock = 1;
}

/* 022E06A4 */
void daNpc_So_c::modeNearSwim() {
    // HD: the battle camera only while riding the boat (or in the 0x01000000 state); "WaterBattle" while swimming
    if ((so_playerStatus0() & SoModeConstants::daPyStts0_SHIP_RIDE_e) ||
        (so_playerStatus0() & SoModeConstants::daPyStts0_UNK01000000_e)) {
        u32 cam = soMode_cameraBody(); /* dCam_getBody */
        soMode_cameraType(cam, "BoatBattle", 0);
    } else if (so_playerStatus0() & SoModeConstants::daPyStts0_SWIM_e) {
        u32 cam = soMode_cameraBody();
        soMode_cameraType(cam, "WaterBattle", 0);
    }
    mB70 = 2;
    fopAc_ac_c* ship = so_ship();
    if (ship == nullptr)
        return;
    fopAc_ac_c* pl = so_player();
    f64 abs1 = so_absXZ(gabi::ea(&pl->current.pos), gabi::ea(&mA80));
    SoModeCirclePath* path = so_path(this);
    cLib_addCalc2(&path->mRadius, 400.0f, 0.1f, 10.0f);
    path->mAngleSpeed = 0x100;
    path->mWobbleAmplitude = 50.0f;
    path->mTranslation.x = (f32)pl->current.pos.x;
    path->mTranslation.y = (f32)pl->current.pos.y;
    path->mTranslation.z = (f32)pl->current.pos.z;
    path->mTranslation.y = (f32)so_getWaterY(gabi::ea(path), gabi::ea(&mAcch));
    so_setCirclePath(gabi::ea(path));
    f32 wy = (f32)so_getWaterY(gabi::ea(&path->mPos), gabi::ea(&mAcch));
    path->mPos.y = gabi::fadds_ppc(wy, mB34);
    f64 d2 = so_absXZ(gabi::ea(&current.pos), gabi::ea(&path->mPos));
    bool keep;
    if (d2 > 150.0f || ship->speedF > 10.0f) {
        mAFC = 12.0f;
        s16 target = cLib_targetAngleY(&current.pos, &path->mPos);
        cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
        f32 a7c = mA7C;
        f32 y = gabi::fadds_ppc(current.pos.y, mB34);
        mB04 = 0.0f;
        current.pos.y = y;
        keep = abs1 < a7c;
    } else {
        cLib_addCalc2(&mB04, 1.0f, 0.01f, 0.05f);
        cXyz d;
        cXyz_mi(gabi::ea(&path->mPos), gabi::ea(&d), gabi::ea(&current.pos));
        cXyz m;
        cXyz_ml(gabi::ea(&d), gabi::ea(&m), mB04);
        cXyz p;
        cXyz_pl(gabi::ea(&current.pos), gabi::ea(&p), gabi::ea(&m));
        s16 ang = (s16)((s16)path->mAngle + 0x8000);
        so_word_copy3(gabi::ea(&current.pos), gabi::ea(&p));
        cLib_addCalcAngleS2(&shape_angle.y, ang, 4, 0x400);
        f32 y = gabi::fadds_ppc(current.pos.y, mB34);
        f32 a7c = mA7C;
        current.pos.y = y;
        keep = abs1 < a7c;
    }
    if (!keep) {
        so_modeProcInit(this, MODE_SWIM_e);
    }
}

/* 022E09B0 */
void daNpc_So_c::modeEventFirstWaitInit() {
    mA90 = 150;
    mAFC = 0.0f;
    speedF = 0.0f;
    offsetDive();
}

/* 022E09CC */
void daNpc_So_c::modeEventFirstWait() {
    fopAc_ac_c* ship = so_ship();
    if (ship == nullptr)
        return;
    so_word_copy3(gabi::ea(&current.pos),
                  gabi::ea(&ship->current.pos)); /* current.pos = ship->current.pos */
    f64 abs = so_absXZ(gabi::ea(&ship->current.pos), gabi::ea(&mAAC));
    if (abs < soModeHIO().m54)
        return;
    // HD: the sail or item 0x77
    if (!dComIfGs_checkGetItem(SoModeConstants::dItemNo_SAIL_e) &&
        !dComIfGs_checkGetItem(SoModeConstants::dItemNo_SAIL_FAST_e))
        return;
    if (!dComIfGs_isStageBossEnemy(SoModeConstants::STAGE_DRC))
        return;
    if (so_playerStatus0() & SoModeConstants::daPyStts0_SHIP_RIDE_e) {
        so_modeProcInit(this, MODE_EVENT_FIRST_e);
    }
}

/* 022E0AB4 */
void daNpc_So_c::offsetSwim() {
    mB38.x = -40.0f;
    mB38.y = 10.0f;
    mB38.z = 0.3f;
}

/* 022E0ADC */
void daNpc_So_c::modeEventFirstInit() {
    offsetSwim();
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}

/* 022E0B14 */
void daNpc_So_c::cutProc() {
    static const char* action_table[] = {
        "SWIM",           "JUMP",          "APPEAR",        "DIVE",
        "DISAPPEAR",      "SET_ANM",       "EFFECT",        "EQUIP",
        "UNEQUIP",        "EATESA",        "EATESA1ST",     "JUMP_MAPOPEN",
        "MINIGAME",       "TURN",          "MINIGAME_WARP", "MINIGAME_RETURN",
        "PARTNER_SHIP",   "MINIGAME_WAIT", "MINIGAME_END",  "MINIGAME_PL_TURN",
        "MINIGAME_PL_UP",
    };
    static SoModeCutTable cut_tbl;
    s32 staffId = soMode_staffId(dComIfGp_ea() + 0x52C4, "NpcSo", 0, 0); /* getMyStaffId */
    mB6C = staffId;
    if (staffId == -1)
        return;
    s32 actIdx = soMode_actionIndex(dComIfGp_ea() + 0x52C4, staffId, gabi::ea(action_table), 21, 1,
                                    0); /* getMyActIdx */
    staffId = mB6C;
    if (actIdx == -1) {
        soMode_cutEnd(dComIfGp_ea() + 0x52C4, staffId); /* cutEnd */
        return;
    }
    BOOL adv = soMode_isAdvance(dComIfGp_ea() + 0x52C4, staffId); /* getIsAddvance */
    if (adv) {
        (this->*cut_tbl.mEntries[actIdx].init)(); /* start */
    }
    (this->*cut_tbl.mEntries[actIdx].run)(); /* proc */
}

static inline bool so_demoAccept(daNpc_So_c* t) {
    return t->eventInfo.mCommand == dEvtCmd_INDEMO_e;
} /* eventInfo.checkCommandDemoAccrpt() */
static inline void so_cut(daNpc_So_c* t) {
    if (!t->mEventCut.cutProc()) {
        t->cutProc();
    }
}

/* 022E0C80 */
void daNpc_So_c::modeEventFirst() {
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld("SO_1ST_MEET")) {
            so_event_onEventFlag8();
            mB70 = 1;
            so_modeProcInit(this, MODE_TALK_e);
            attention_info.flags = fopAc_Attn_TALKFLAG_NOTALK_e | fopAc_Attn_ACTION_SPEAK_e |
                                   fopAc_Attn_LOCKON_TALK_e; /* attention_info.flags */
        }
    } else {
        mB70 = 3;
    }
}

/* 022E0D38 */
void daNpc_So_c::modeEventFirstEndInit() {
    offsetAppear();
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}

/* 022E0D70 */
void daNpc_So_c::modeEventFirstEnd() {
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld("SO_1ST_MEET_END")) {
            so_event_onEventFlag8();
            so_modeProcInit(this, MODE_DISAPPEAR_e);
        }
    } else if (talk(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mB70 = 4;
    }
}

/* 022E0E28 */
void daNpc_So_c::modeEventEsaInit() {
    offsetSwim();
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}

/* 022E0E60 */
void daNpc_So_c::modeEventEsa() {
    so_cut(this);
    if (so_endCheck(mBDC)) {
        so_event_onEventFlag8();
        mB70 = 1;
        mBDC = -1;
        so_modeProcInit(this, MODE_TALK_e);
    }
}

/* 022E0EF4 */
void daNpc_So_c::modeEventMapopenInit() {
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}

/* 022E0F10 */
void daNpc_So_c::modeEventMapopen() {
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld("SO_MAPOPEN")) {
            so_event_onEventFlag8();
            mB70 = 1;
            mBD8 = 1;
            so_modeProcInit(this, MODE_TALK_e);
        }
    } else if (talk(1) == 0x12) {
        mB70 = 5;
    }
}

/* 022E0FD4 */
void daNpc_So_c::modeEventBowInit() {
    offsetSwim();
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}

/* 022E100C */
void daNpc_So_c::modeEventBow() {
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld("SO_BOW")) {
            mB0C = 1;
            so_event_onEventFlag8();
            mBDB = 0;
            so_onEventBit(0x3A10);
            s8 camId = dComIfGp_getPlayerCameraID(0); /* dComIfGp_getPlayerCameraID(0) */
            u32 play = dComIfGp_ea();
            /* the reset position and centre are passed by value (lfs/stfs copies) */
            cXyz ctr;
            cXyz eye;
            f32 bc0x = mBC0.x, bc0z = mBC0.z, bccx = mBCC.x, bccy = mBCC.y, bc0y = mBC0.y;
            u32 cam = (u32)soModePlayCamera(play).mCamera[camId].mpCamera;
            ctr.y = bccy;
            eye.y = bc0y;
            eye.x = bc0x;
            ctr.x = bccx;
            f32 bccz = mBCC.z;
            eye.z = bc0z;
            ctr.z = bccz;
            soMode_resetCamera(cam + 0x248, &ctr, &eye); /* dCamera_c::Reset(mBCC, mBC0) */
            soMode_startCamera(cam + 0x248);             /* dCamera_c::Start */
            mB70 = 1;
            so_modeProcInit(this, MODE_TALK_e);
        }
    } else if (talk(1) == 0x12) {
        mB70 = 6;
    }
}

/* 022E116C */
void daNpc_So_c::modeTalkInit() {
    offsetAppear();
    so_setAnm(this, 3, false);
    m_jnt.mbBackBoneLock = 0;
}

/* 022E11AC */
void daNpc_So_c::modeTalk() {
    if (talk(1) == 0x12) {
        so_event_onEventFlag8();
        so_modeProcInit(this, MODE_DISAPPEAR_e);
    }
}

/* 022E1204 */
void daNpc_So_c::modeDisappearInit() {
    if (!so_isEventBit(0x901)) {
        so_onEventBit(0x901);
    }
    offsetDive();
    so_seStart(this, SoModeConstants::JA_SE_CM_SO_DIVE, 0);
    soMode_createWaterPillar(&current.pos, gabi::fmuls_ppc(scale.x, 1.2f), 1.4f,
                             0); /* fopKyM_createWpillar */
    m_jnt.mbBackBoneLock = 0;
}

/* 022E12B4 */
void daNpc_So_c::modeDisappear() {
    if (std::fabs(gabi::fsubs_ppc(mB34, mB38.x)) < 10.0f) {
        // HD: no new hiding place is searched here (no searchTagSo loop, mA79 and current.pos
        // are left alone)
        mBD8 = 0;
        mB84 = 0;
        mB90.x = 0.0f;
        mBA0.z = 0.0f;
        mBA0.y = 0.0f;
        mB8C = 0.0f;
        mB90.y = 0.0f;
        mB88 = 0.0f;
        mBA0.x = 0.0f;
        mB7C = 0;
        mBAC = 0;
        mB74 = 0;
        mB0C = 0;
        mB9C = 0;
        mBDB = 1;
        mB80 = 0;
        mB78 = 0;
        mBD9 = 0;
        mB90.z = 0.0f;
        so_modeProcInit(this, MODE_HIDE_e);
        gravity = -2.5f;
    }
}

/* 022E136C */
void daNpc_So_c::modeDebugInit() {
    mAFC = 0.0f;
    speedF = 0.0f;
    so_setAnm(this, 1, false);
    fopAc_ac_c* pl = so_player();
    f32 y = pl->current.pos.y, x = pl->current.pos.x;
    s16 a = pl->shape_angle.y;
    f32 z = pl->current.pos.z;
    f32 c = cM_scos(a), s = cM_ssin(a);
    current.pos.y = y;
    current.pos.x = gabi::fmadds(100.0f, c, x);
    current.pos.z = gabi::fmadds(100.0f, s, z);
    offsetAppear();
}

/* 022E140C */
void daNpc_So_c::modeGetRupee() {
    static s16 REG12_S9;
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld("SO_GET_RUPEE")) {
            so_event_onEventFlag8();
            mB70 = 1;
            mBD9 = 1;
            so_modeProcInit(this, MODE_TALK_e);
        }
    } else if (talk(1) == 0x12) {
        if (REG12_S9 != 0) { /* REG12_S(9) */
            so_event_onEventFlag8();
        }
        /* daPy_py_c::cancelOriginalDemo() (inline): mDemo.setDemoType(2), mDemo.setParam0(1)? */
        u32 pl = gabi::ea(so_player());
        soModePlayerDemo(pl).mDemoType = 2;
        soModePlayerDemo(pl).mDemoMode = 1;
        mB70 = 7;
    }
}

/* 022E1508 */
void daNpc_So_c::modeEventTriForceInit() {
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
    offsetAppear();
    so_setAnm(this, 1, false);
}

/* 022E1548 */
void daNpc_So_c::modeEventTriForce() {
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld("SO_TRIFORCE_CHECK")) {
            attention_info.flags = fopAc_Attn_TALKFLAG_NOTALK_e | fopAc_Attn_ACTION_SPEAK_e |
                                   fopAc_Attn_LOCKON_TALK_e; /* attention_info.flags */
            so_event_onEventFlag8();
            so_modeProcInit(this, MODE_DISAPPEAR_e);
            so_onEventBit(0x3A20);
        }
    } else {
        mB70 = 8;
    }
}

/* 022E160C */
bool daNpc_So_c::checkTgHit() {
    fopAc_ac_c* player = so_player();
    soMode_moveStatus(gabi::ea(&mStts2.__vtbl_gstts)); /* mStts2.Move() (dCcD_GStts::Move) */
    if (so_calcTimerI(&m6D8) != 0)
        return false;
    if (!soMode_checkTargetHit(&mSph))
        return false;                        /* ChkTgHit */
    cXyz* pos = &mSph.mGObjTg.mHitPos;       /* GetTgHitPosP() */
    u32 obj = soMode_targetHitObject(&mSph); /* GetTgHitObj() */
    m6D8 = soModeHIO().m7C;
    if (obj == 0)
        return false;
    if (soModeHitObject(obj).mAtType == SoModeConstants::AT_TYPE_NORMAL_ARROW) {
        so_seStart(this, SoModeConstants::JA_SE_LK_ARROW_HIT, 0x20);
    }
    so_monsSeStart(this, SoModeConstants::JA_SE_CV_SO_DAMAGE, 0);
    dComIfGp_particle_set(SoModeConstants::ID_AK_JN_CRITICALHITFLASH, pos);
    cXyz sc;
    sc.x = 2.0f;
    sc.z = 2.0f;
    sc.y = 2.0f;
    dComIfGp_particle_set(SoModeConstants::ID_AK_JN_CRITICALHIT, pos, &player->shape_angle, &sc);
    so_seStart(this, SoModeConstants::JA_SE_LK_LAST_HIT, 0);
    return true;
}

/* 022E179C */
u16 daNpc_So_c::next_msgStatus(be<u32>* pMsgNo) {
    u16 ret = 0xF;
    u32 msgNo = *pMsgNo;
    s32 m6d0 = m6D0;
    SoModeMessage* msgMgr = soModeMessage(); /* HD message manager (select number at +0x948) */
    if (msgNo == (u32)m6d0) {
        if (mBD8) {
            bool first = false;
            if (!so_isEventBit(0x901) && so_isStartStage("sea") &&
                fopAcM_GetRoomNo(this) == 0xD /* dIsleRoom_DragonRoostIsland_e */) {
                first = true;
            }
            *pMsgNo = first ? 0x32CE : 0x32D2;
        } else {
            *pMsgNo = 0x32D6;
        }
        return ret;
    }
    // HD: The first-meeting and map-hint messages also notify the HD map object.
    bool notify = msgNo == 0x32CA || msgNo == 0x32D0;
    SoModeHDInfo* hd = soModeHDInfo();
    switch (msgNo) {
    case 0x633:
        if (dComIfGs_getTriforceNum() == 8) { /* dComIfGs_getTriforceNum() */
            *pMsgNo = 0x635;
        } else {
            *pMsgNo = 0x634;
        }
        break;
    case 0x32CA:
        *pMsgNo = 0x32CB;
        break;
    case 0x32CB:
        *pMsgNo = 0x32CC;
        break;
    case 0x32CC:
        *pMsgNo = 0x32CD;
        break;
    case 0x32CE:
        ret = 0x10;
        so_modeProcInit(this, MODE_EVENT_FIRST_END_e);
        break;
    case 0x32D0:
        // HD: dComIfGs_isSaveArriveGrid through the HD map object
        if (soMode_mapCheck((u32)soModeHDInfo()->mpMap, fopAcM_GetRoomNo(this) - 1) ||
            soModeHIO().m2F != 0) {
            *pMsgNo = 0x32D4;
            break;
        }
        *pMsgNo = 0x32D1;
        break;
    case 0x32CD:
    case 0x32D1:
        ret = 0x10;
        so_modeProcInit(this, MODE_EVENT_MAPOPEN_e);
        break;
    case 0x32D4:
        *pMsgNo = m6d0;
        break;
    case 0x32D2:
        *pMsgNo = 0x32D3;
        break;
    case 0x32D6:
        soMode_searchMinigame(0x022DDFF4 /* searchMinigameTagSo_CB */, this); /* fopAcM_Search */
        if (soModeHIO().m30 != 0 || mBAE != 0) {
            if (dComIfGs_getItem(SoModeConstants::dInvSlot_BOW_e) != 0xFF /* dComIfGs_getItem(dInvSlot_BOW_e) */ &&
                dComIfG_getTimerPtr() == nullptr /* dComIfG_getTimerPtr() */) {
                if (!so_isEventBit(0x3A10)) {
                    *pMsgNo = 0x32D8;
                    break;
                }
                *pMsgNo = 0x32DC;
                break;
            }
        }
        *pMsgNo = 0x32D7;
        break;
    case 0x32D8:
        if ((u32)msgMgr->mSelectNumber == 0) {
            *pMsgNo = 0x32DA;
            break;
        }
        *pMsgNo = 0x32D9;
        break;
    case 0x32DC:
        if ((u32)msgMgr->mSelectNumber == 0) {
            *pMsgNo = 0x32DB;
            break;
        }
        *pMsgNo = 0x32D9;
        break;
    case 0x32DA:
        *pMsgNo = 0x32DB;
        break;
    case 0x32DB:
        ret = 0x10;
        so_modeProcInit(this, MODE_EVENT_BOW_e);
        break;
    case 0x32DD:
        *pMsgNo = 0x32DE;
        break;
    case 0x32DE:
        ret = 0x10;
        so_modeProcInit(this, MODE_GET_RUPEE_e);
        break;
    case 0x32DF:
    case 0x32E0: {
        s32 add = mB7C * 10;
        dComIfGp_setItemRupeeCount(add);
        *pMsgNo = 0x32E2;
        break;
    }
    case 0x32E1:
        *pMsgNo = 0x32E2;
        break;
    case 0x32D9:
        *pMsgNo = 0x32D7;
        break;
    default:
        ret = 0x10;
        break;
    }
    if (notify) {
        soMode_mapNotify((u32)hd->mpMap);
    }
    return ret;
}

/* 022E1CA8 */
u32 daNpc_So_c::getMsg() {
    if (mB0C) {
        s32 n;
        if (soModeHIO().m2E != 0 || (n = mB7C) >= 10) {
            if (mBD9) {
                return 0x32E2;
            }
            return 0x32DD;
        }
        if (n == 0) {
            return 0x32E1;
        }
        if (n == 1) {
            return 0x32E0;
        }
        dComIfGp_setMessageCountNumber((s16)n); /* dComIfGp_setMessageCountNumber */
        return 0x32DF;
    }
    bool first = false;
    if (!so_isEventBit(0x901) && so_isStartStage("sea") && fopAcM_GetRoomNo(this) == 0xD) {
        first = true;
    }
    if (mBD8) {
        return (u32)(s32)m6D0;
    }
    return first ? 0x32CA : 0x32D0;
}

/* modeWait / modeDebug / modeGetRupeeInit (empty, merged) */
/* 022E1E70 */
void daNpc_So_c::modeWait() {}

/* WWHD glue (tools/verify/autoglue.py): Fishman mode bindings
data action_table 101C6314
data cut_tbl 10022004
str 10022154 "SWIM" in 022E0B14
str 1002215C "JUMP" in 022E0B14
str 10022164 "APPEAR"
str 1002216C "DIVE"
str 10022174 "DISAPPEAR"
str 10022180 "SET_ANM"
str 10022188 "EFFECT"
str 10022190 "EQUIP"
str 10022198 "UNEQUIP"
str 100221A0 "EATESA"
str 100221A8 "EATESA1ST"
str 100221B4 "JUMP_MAPOPEN"
str 100221C4 "MINIGAME"
str 100221D0 "TURN"
str 100221D8 "MINIGAME_WARP"
str 100221E8 "MINIGAME_RETURN"
str 100221F8 "PARTNER_SHIP"
str 10022208 "MINIGAME_WAIT"
str 10022218 "MINIGAME_END"
str 10022228 "MINIGAME_PL_TURN"
str 1002223C "MINIGAME_PL_UP"
str 10021FEC "BoatBattle"
str 10021FE0 "WaterBattle"
str 10021FFC "NpcSo" in 022E0B14
str 1002224C "SO_1ST_MEET"
str 10022258 "SO_1ST_MEET_END"
str 10022268 "SO_MAPOPEN"
str 10022274 "SO_BOW"
str 10022280 "SO_GET_RUPEE"
str 10022290 "SO_TRIFORCE_CHECK"
str 100222A8 "sea" in 022E179C
str 100222AC "sea" in 022E1CA8
data offset_guard 101FDA48
data target_guard 101FDA50
data circle_guard 101FDA4C
data attention_guard 101FDA44
data line_guard 101FDAC0
data offset_color 101FEBEC
data target_color 101FEBF4
data circle_color 101FEBF0
data attention_color 101FEBE8
data line_color 101FEBF8
data offset_color_default 10021B50
data target_color_default 10021B54
data circle_color_default 10021B58
data attention_color_default 10021B5C
data line_color_default 10021B60
data REG12_S9 1047BD5A
ext 025E1A40 void soMode_seStart(u32, cXyz*, u32, s32)
ext 025E1AA4 void soMode_monsSeStart(u32, cXyz*, u32, u32, s32)
ext 0254457C BOOL soMode_endCheckOld(u32, const char*)
ext 025440C8 BOOL soMode_endCheck(u32, s16)
ext 025871F8 f64 soMode_getWaterY(u32, u32)
ext 02587128 void soMode_setCirclePath(u32)
ext 0201ADE0 void soMode_subtract(u32, u32, u32)
ext 0201AE48 void soMode_multiply(u32, u32, f32)
ext 0201AD78 void soMode_add(u32, u32, u32)
ext 028E8DD0 f64 soMode_squareMagnitude(cXyz*)
ext 028F4384 f64 soMode_sqrtUnrounded(f64)
ext 0211D2F8 s32 soMode_calcTimer(be<s32>*)
ext 022DE6D4 void soMode_setAnm(daNpc_So_c*, s8, bool)
ext 022DE7D8 void soMode_modeProc(daNpc_So_c*, s32, s32)
ext C000A848 void soMode_copyColor(u32, u32, s32)
ext 028E90D4 void soMode_matrixCopy(u32, u32)
ext 025E7B3C void soMode_enterBtp(u8*, u32, s16)
ext 025BEBB8 void soMode_registerFigure(s32, daNpc_So_c*, cXyz*, s16, f32, f32, f32)
ext 025E5590 void soMode_enterMorf(mDoExt_McaMorf*)
ext 025DAE64 void soMode_createWaterPillar(cXyz*, f32, f32, s32)
ext 024F8044 u32 soMode_cameraBody()
ext 02514EE4 void soMode_cameraType(u32, const char*, s32)
ext 02542D88 s32 soMode_staffId(u32, const char*, s32, s32)
ext 02542EDC s32 soMode_actionIndex(u32, s32, u32, s32, s32, s32)
ext 02543280 void soMode_cutEnd(u32, s32)
ext 025447C8 BOOL soMode_isAdvance(u32, s32)
ext 0251510C void soMode_resetCamera(u32, cXyz*, cXyz*)
ext 02514F38 void soMode_startCamera(u32)
ext 02515E50 void soMode_moveStatus(u32)
ext 025162A4 BOOL soMode_checkTargetHit(dCcD_Sph*)
ext 02516300 u32 soMode_targetHitObject(dCcD_Sph*)
ext 02689174 BOOL soMode_mapCheck(u32, s32)
ext 025D5218 void soMode_searchMinigame(u32, daNpc_So_c*)
ext 0268A858 void soMode_mapNotify(u32)
*/

/**
 * d_a_npc_so_mode.cpp (WWHD)
 * NPC - Fishman: draw, offsets, modes, hit check, messages
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_npc_so.cpp) to the WWHD layout and code, verified against
 * cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 */
#include "d/actor/d_a_npc_so.h"

/* ---- this TU's statics (HD addresses) ---- */
#define SO_L_HIO 0x104686CC            /* l_HIO (daNpc_So_HIO_c); raw offsets below are HD offsets */
#define SO_SAFESTRING_VTBL 0x10021B78  /* this TU's sead::SafeString vtable */
#define SO_CUT_TBL 0x10022004          /* cut table: {start, proc} GHS pointers to member, 16 bytes per entry */
#define SO_CUT_NAME_TBL 0x101C6314     /* cut names (21) */

static inline f32 so_hioF(u32 off) { return gabi::load<f32>(SO_L_HIO + off); }
static inline s16 so_hioS(u32 off) { return gabi::load<s16>(SO_L_HIO + off); }
static inline u8 so_hioU8(u32 off) { return gabi::load<u8>(SO_L_HIO + off); }

/* ---- local bindings (SHARED-CANDIDATE) ---- */
static inline BOOL dComIfGs_checkGetItem(u8 item) { return gabi::call<BOOL>(0x02520C0C, item); }
static inline BOOL dComIfGs_isStageBossEnemy(s32 no) { return gabi::call<BOOL>(0x02520A84, no); }
static inline u32 so_play() { return dComIfGp_ea(); }
static inline fopAc_ac_c* so_player() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B2C)); }
static inline fopAc_ac_c* so_ship() { return gabi::at<fopAc_ac_c>(gabi::load<u32>(dComIfGp_ea() + 0x5B3C)); } /* dComIfGp_getShipActor */
static inline u32 so_playerStatus0() { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8); }
/* fopAcM_seStart (inline, HD: no NULL checks): 025E1A40(id, &eyePos, param, reverb) */
static inline void so_seStart(daNpc_So_c* t, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(t));
    gabi::call(0x025E1A40, id, &t->eyePos, param, reverb);
}
/* fopAcM_monsSeStart: 025E1AA4(id, &eyePos, base id (this+4), param, reverb) */
static inline void so_monsSeStart(daNpc_So_c* t, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(fopAcM_GetRoomNo(t));
    gabi::call(0x025E1AA4, id, &t->eyePos, gabi::load<u32>(gabi::ea(t) + 4), param, reverb);
}
static inline dSv_event_c* so_event() { return gabi::at<dSv_event_c>(gabi::load<u32>(0x101F84DC) + 0x644); }
static inline BOOL so_isEventBit(u16 f) { return dSv_event_isEventBit(so_event(), f); }
static inline void so_onEventBit(u16 f) { dSv_event_onEventBit(so_event(), f); }
/* dComIfGp_event_onEventFlag(8): play+0x52B8 |= 8 */
static inline void so_event_onEventFlag8() {
    u32 p = dComIfGp_ea() + 0x52B8;
    gabi::store<u16>(p, gabi::load<u16>(p) | 8);
}
static inline BOOL so_endCheckOld(u32 name) { return gabi::call<BOOL>(0x0254457C, dComIfGp_ea() + 0x52C4, name); }
static inline BOOL so_endCheck(s16 idx) { return gabi::call<BOOL>(0x025440C8, dComIfGp_ea() + 0x52C4, idx); }
static inline f64 so_getWaterY(u32 pos, u32 acch) { return gabi::call<f64>(0x025871F8, pos, acch); } /* dLib_getWaterY */
static inline void so_setCirclePath(u32 path) { gabi::call(0x02587128, path); }                         /* dLib_setCirclePath */
static inline void cXyz_mi(u32 a, u32 out, u32 b) { gabi::call(0x0201ADE0, a, out, b); }
static inline void cXyz_ml(u32 a, u32 out, f32 s) { gabi::call(0x0201AE48, a, out, s); }
static inline void cXyz_pl(u32 a, u32 out, u32 b) { gabi::call(0x0201AD78, a, out, b); }
/* (a - b).absXZ(): f1 of sqrtf is used unrounded */
static inline f64 so_absXZ(u32 a, u32 b) {
    gabi::Local<cXyz> d;
    cXyz_mi(a, gabi::ea(d.get()), b);
    gabi::Local<cXyz> v;
    v->x = (f32)d->x;
    v->y = 0.0f;
    v->z = (f32)d->z;
    f64 sq = gabi::call<f64>(0x028E8DD0, v.get()); /* PSVECSquareMag */
    return gabi::call<f64>(0x028F4384, sq);        /* std::sqrtf */
}
static inline s32 so_calcTimerI(be<s32>* t) { return gabi::call<s32>(0x0211D2F8, t); } /* cLib_calcTimer<int> */
/* the other parts of the unit (P1): setAnm 022DE6D4, modeProc 022DE7D8 */
static inline void so_setAnm(daNpc_So_c* t, s8 idx, bool force) { gabi::call(0x022DE6D4, t, idx, force); }
static inline void so_modeProcInit(daNpc_So_c* t, int mode) { gabi::call(0x022DE7D8, t, 0 /* PROC_INIT_e */, mode); }
static inline void so_word_copy3(u32 dst, u32 src) {
    gabi::store<u32>(dst, gabi::load<u32>(src));
    gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
}
/* GHS pointer to member call without argument */
static inline void so_pmf_call(daNpc_So_c* self, u32 pmf) {
    s16 i = gabi::load<s16>(pmf + 2);
    u32 thisp = gabi::ea(self) + gabi::load<s16>(pmf);
    if (i < 0) {
        gabi::call_ptr(gabi::load<u32>(pmf + 4), thisp);
    } else {
        s16 voff = gabi::load<s16>(pmf + 6);
        u32 vt = gabi::load<u32>(thisp + voff);
        gabi::call_ptr(gabi::load<u32>(vt + i * 8 + 4), thisp);
    }
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0, HD: sead::SafeString operator== */
static inline bool so_isStartStage(u32 lit) {
    gabi::Local<SafeString> sa;
    sa->mStringTop = lit;
    sa->__vtbl = SO_SAFESTRING_VTBL;
    u32 stage = dComIfGp_ea() + 0x5134;
    gabi::Local<SafeString> sb;
    sb->mStringTop = stage;
    sb->__vtbl = SO_SAFESTRING_VTBL;
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    gabi::call_ptr(gabi::load<u32>(sa->__vtbl + 0x14), sa.get());
    u32 pa = sa->mStringTop;
    gabi::call_ptr(gabi::load<u32>(sb->__vtbl + 0x14), sb.get());
    u32 pb = sb->mStringTop;
    if (pa == pb) return true;
    for (u32 i = 0; i < 0x40001; i++) {
        u8 ca = gabi::load<u8>(pa + i);
        if (ca != gabi::load<u8>(pb + i)) return false;
        if (ca == 0) return true;
    }
    return false;
}
/* dLib_circle_path_c (0x24): mTranslation 0x00, mPos 0x0C, mRadius 0x18, mWobbleAmplitude 0x1C,
 * mAngle 0x20, mAngleSpeed 0x22 */
static inline u32 so_path(daNpc_So_c* t) { return gabi::ea(t) + 0xC88; }

/* ====================================================================== */

/* 022DFBD8: debugDraw (HD: the debug shapes are compiled out; only the function-local static
 * colours of the GameCube _draw remain, initialised on first use) */
void daNpc_So_c::debugDraw() {
    WWHD_FUNC(0x022DFBD8, void, this);
    dComIfGp_ea(); /* dComIfGp_getPlayer(0) (result unused) */
    auto init = [](u32 guard, u32 dst, u32 src) {
        if (gabi::load<u32>(guard) == 0) {
            gabi::store<u32>(guard, 1);
            gabi::call(0xC000A848, dst, src, 4); /* memcpy */
        }
    };
    init(0x101FDA48, 0x101FEBEC, 0x10021B50);
    init(0x101FDA50, 0x101FEBF4, 0x10021B54);
    init(0x101FDA48, 0x101FEBEC, 0x10021B50);
    init(0x101FDA50, 0x101FEBF4, 0x10021B54);
    init(0x101FDA48, 0x101FEBEC, 0x10021B50);
    init(0x101FDA50, 0x101FEBF4, 0x10021B54);
    init(0x101FDA4C, 0x101FEBF0, 0x10021B58);
    init(0x101FDA44, 0x101FEBE8, 0x10021B5C);
    init(0x101FDA44, 0x101FEBE8, 0x10021B5C);
    init(0x101FDAC0, 0x101FEBF8, 0x10021B60);
    init(0x101FDAC0, 0x101FEBF8, 0x10021B60);
}
VERIFY(0x022DFBD8, &daNpc_So_c::debugDraw);

/* 022DFDFC */
void daNpc_So_c::hudeDraw() {
    WWHD_FUNC(0x022DFDFC, void, this);
    dScnKy_env_light_c* env = dKy_getEnvlight();
    setLightTevColorType(env, mpModel.get(), &tevStr);
    /* mDoMtx_stack_c::copy(mpMorf->getModel()->getAnmMtx(11)) (HD: marks the joint matrices dirty) */
    u32 model = gabi::load<u32>(gabi::ea(mpMorf.get()) + 0x90);
    u32 blk = gabi::load<u32>(model + 0x2C);
    u16 flags = gabi::load<u16>(blk + 4);
    u32 mtx = gabi::load<u32>(blk + 0x10);
    gabi::store<u16>(blk + 4, flags | 0x10);
    gabi::call(0x028E90D4, mtx + 11 * 0x30, gabi::ea(mDoMtx_stack_c::get())); /* PSMTXCopy */
    /* mpModel->setBaseTRMtx(mDoMtx_stack_c::get()): lfs/stfs copy */
    f32 v[12];
    u32 s = gabi::ea(mDoMtx_stack_c::get());
    for (int i = 0; i < 12; i++) v[i] = gabi::load<f32>(s + 4 * i);
    u32 m = gabi::ea(mpModel.get());
    for (int i = 0; i < 12; i++) gabi::store<f32>(m + 0xC8 + 4 * i, v[i]);
    mDoExt_modelUpdateDL(mpModel.get(), 0);
}
VERIFY(0x022DFDFC, &daNpc_So_c::hudeDraw);

/* 022DFEDC */
bool daNpc_So_c::_draw() {
    WWHD_FUNC(0x022DFEDC, bool, this);
    if (so_hioU8(0x22) != 0) { /* l_HIO.mNpc.m22 */
        debugDraw();
    }
    if (m6CC == 5) {
        return true;
    }
    if (so_hioU8(0x31) == 0) {
        u32 model = gabi::load<u32>(gabi::ea(mpMorf.get()) + 0x90);
        u32 modelData = gabi::load<u32>(model + 0xAC);
        settingTevStruct(dKy_getEnvlight(), 0 /* TEV_TYPE_ACTOR */, &current.pos, &tevStr);
        setLightTevColorType(dKy_getEnvlight(), gabi::at<J3DModel>(model), &tevStr);
        gabi::call(0x025E7B3C, mBtpAnm, modelData, (s16)m86C); /* mBtpAnm.entry(modelData, m86C) */
        mpMorf->entryDL();
        gabi::store<u32>(modelData + 0x38, 0); /* removeTexNoAnimator (HD inline) */
        if (mA78 != 0 || so_hioU8(0x2C) != 0) {
            hudeDraw();
        }
    }
    gabi::Local<cXyz> tmp;
    f32 y = gabi::fadds_ppc(current.pos.y, mB34);
    tmp->x = (f32)current.pos.x;
    tmp->z = (f32)current.pos.z;
    tmp->y = y;
    gabi::call(0x025BEBB8, 0x7D, this, tmp.get(), (s16)shape_angle.y, 1.0f, 1.0f, 1.0f); /* dSnap_RegistFig */
    /* HD: no dComIfGd_setShadow; a second morf (0xBEC) is drawn while 0xBF0 is clear */
    if (mBF0 == 0) {
        gabi::call(0x025E5590, mA74.get()); /* mDoExt_McaMorf::entryDL */
    }
    return true;
}
VERIFY(0x022DFEDC, &daNpc_So_c::_draw);

/* 022E001C */
static BOOL daNpc_SoDraw(void* i_this) {
    WWHD_FUNC(0x022E001C, BOOL, i_this);
    return ((daNpc_So_c*)i_this)->_draw();
}
VERIFY(0x022E001C, daNpc_SoDraw);

/* 022E0020 */
void daNpc_So_c::offsetAppear() {
    WWHD_FUNC(0x022E0020, void, this);
    mB38.x = 110.0f;
    mB38.y = 22.0f;
    mB38.z = 0.4f;
}
VERIFY(0x022E0020, &daNpc_So_c::offsetAppear);

/* 022E0048 */
void daNpc_So_c::modeWaitInit() {
    WWHD_FUNC(0x022E0048, void, this);
    speedF = 0.0f;
    mAFC = 0.0f;
    offsetAppear();
}
VERIFY(0x022E0048, &daNpc_So_c::modeWaitInit);

/* 022E005C */
void daNpc_So_c::offsetDive() {
    WWHD_FUNC(0x022E005C, void, this);
    mB38.x = -150.0f;
    mB38.y = 10.0f;
    mB38.z = 0.3f;
}
VERIFY(0x022E005C, &daNpc_So_c::offsetDive);

/* 022E0084 */
void daNpc_So_c::modeHideInit() {
    WWHD_FUNC(0x022E0084, void, this);
    mBDB = 1;
    offsetDive();
    mA7C = 0.0f;
}
VERIFY(0x022E0084, &daNpc_So_c::modeHideInit);

/* 022E00B8 */
void daNpc_So_c::modeHide() {
    WWHD_FUNC(0x022E00B8, void, this);
    so_word_copy3(gabi::ea(&current.pos), gabi::ea(&mA80)); /* current.pos = mA80 */
}
VERIFY(0x022E00B8, &daNpc_So_c::modeHide);

/* 022E00D4 */
void daNpc_So_c::modeJumpInit() {
    WWHD_FUNC(0x022E00D4, void, this);
    f32 r = cM_rndF(5.0f);
    f32 b08 = mB08;
    f32 spd = gabi::fmuls_ppc(gabi::fadds_ppc(r, 5.0f), b08);
    f32 sp4 = gabi::fmuls_ppc(spd, 4.0f);
    f32 vy = gabi::fmadds(30.0f, b08, sp4);
    mAFC = spd;
    speedF = spd;
    speed.y = vy;
    f32 mx = so_hioF(0x50);
    if (vy > mx) {
        vy = mx;
        speed.y = vy;
    }
    mB00 = vy;
    shape_angle.x = so_hioS(0x68);
    so_setAnm(this, 4, false);
    m_jnt.mbBackBoneLock = 1;
    so_seStart(this, 0x5938 /* JA_SE_CM_SO_JUMP_L */, 0);
    gabi::call(0x025DAE64, &current.pos, gabi::fmuls_ppc(scale.x, 0.8f), 1.4f, 0); /* HD: fopKyM_createWpillar at the jump */
}
VERIFY(0x022E00D4, &daNpc_So_c::modeJumpInit);

/* 022E01D8 */
void daNpc_So_c::modeJump() {
    WWHD_FUNC(0x022E01D8, void, this);
    /* HD: while the player is near, the camera mode follows the Fishman */
    fopAc_ac_c* pl = so_player();
    f64 d = so_absXZ(gabi::ea(&pl->current.pos), gabi::ea(&mA80));
    if (d < mA7C) {
        mB70 = 2;
    }
    f64 wy = so_getWaterY(gabi::ea(&current.pos), gabi::ea(&mAcch));
    if (current.pos.y < wy) {
        so_seStart(this, 0x5939 /* JA_SE_CM_SO_LANDING_L */, 0);
        gabi::call(0x025DAE64, &current.pos, gabi::fmuls_ppc(scale.x, 1.4f), 1.4f, 0); /* fopKyM_createWpillar */
        f64 d2 = so_absXZ(gabi::ea(&mA80), gabi::ea(&current.pos));
        if (d2 > mA7C) {
            u32 dst = gabi::ea(&current.pos), src = gabi::ea(&mA80);
            u32 z = gabi::load<u32>(src + 8), y = gabi::load<u32>(src + 4);
            gabi::store<u32>(dst + 8, z);
            gabi::store<u32>(dst + 4, y);
            gabi::store<u32>(dst, gabi::load<u32>(src));
        }
        so_modeProcInit(this, MODE_SWIM_e);
    }
}
VERIFY(0x022E01D8, &daNpc_So_c::modeJump);

/* 022E0324 */
void daNpc_So_c::modeSwimInit() {
    WWHD_FUNC(0x022E0324, void, this);
    gabi::store<u32>(gabi::ea(this) + 0x39C, 0x0200000A); /* attention_info.flags */
    mA90 = gabi::ftoi(gabi::fadds_ppc(cM_rndF(90.0f), 30.0f));
    so_setAnm(this, 2, false);
    m_jnt.mbBackBoneLock = 1;
    offsetDive();
}
VERIFY(0x022E0324, &daNpc_So_c::modeSwimInit);

/* 022E03A4 */
void daNpc_So_c::modeSwim() {
    WWHD_FUNC(0x022E03A4, void, this);
    mBDB = 0;
    fopAc_ac_c* pl = so_player();
    f64 abs = so_absXZ(gabi::ea(&pl->current.pos), gabi::ea(&mA80));
    fopAc_ac_c* ship = so_ship();
    if (ship == nullptr) return;
    u32 path = so_path(this);
    cLib_addCalc2(gabi::at<be<f32>>(path + 0x18), 200.0f, 0.1f, 10.0f);
    f32 ty = mA80.y, tz = mA80.z;
    gabi::store<f32>(path + 8, tz);
    gabi::store<s16>(path + 0x22, 0x150);
    f32 tx = mA80.x;
    gabi::store<f32>(path + 4, ty);
    gabi::store<f32>(path + 0, tx);
    gabi::store<f32>(path + 0x1C, 50.0f);
    gabi::store<f32>(path + 4, (f32)so_getWaterY(path, gabi::ea(&mAcch)));
    so_setCirclePath(path);
    f32 wy = (f32)so_getWaterY(path + 0xC, gabi::ea(&mAcch));
    gabi::store<f32>(path + 0x10, gabi::fadds_ppc(wy, mB34));
    f64 d2 = so_absXZ(gabi::ea(&current.pos), path + 0xC);
    bool rideCheck;
    if (d2 > 150.0f || ship->speedF > 10.0f) {
        mAFC = 12.0f;
        s16 target = cLib_targetAngleY(&current.pos, gabi::at<cXyz>(path + 0xC));
        cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
        f32 a7c = mA7C;
        mB04 = 0.0f;
        rideCheck = abs < a7c;
    } else {
        cLib_addCalc2(&mB04, 1.0f, 0.01f, 0.05f);
        gabi::Local<cXyz> d;
        cXyz_mi(path + 0xC, gabi::ea(d.get()), gabi::ea(&current.pos));
        gabi::Local<cXyz> m;
        cXyz_ml(gabi::ea(d.get()), gabi::ea(m.get()), mB04);
        gabi::Local<cXyz> p;
        cXyz_pl(gabi::ea(&current.pos), gabi::ea(p.get()), gabi::ea(m.get()));
        s16 ang = (s16)(gabi::load<s16>(path + 0x20) + 0x8000);
        so_word_copy3(gabi::ea(&current.pos), gabi::ea(p.get()));
        cLib_addCalcAngleS2(&shape_angle.y, ang, 4, 0x400);
        rideCheck = abs < mA7C;
    }
    if (rideCheck && (so_playerStatus0() & 0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        so_modeProcInit(this, MODE_NEAR_SWIM_e);
    } else if (so_calcTimerI(&mA90) == 0) {
        so_modeProcInit(this, MODE_JUMP_e);
    }
}
VERIFY(0x022E03A4, &daNpc_So_c::modeSwim);

/* 022E0660 */
void daNpc_So_c::modeNearSwimInit() {
    WWHD_FUNC(0x022E0660, void, this);
    so_setAnm(this, 2, false);
    offsetDive();
    m_jnt.mbBackBoneLock = 1;
}
VERIFY(0x022E0660, &daNpc_So_c::modeNearSwimInit);

/* 022E06A4 */
void daNpc_So_c::modeNearSwim() {
    WWHD_FUNC(0x022E06A4, void, this);
    /* HD: the battle camera only while riding the boat (or in the 0x01000000 state); "WaterBattle" while swimming */
    if ((so_playerStatus0() & 0x10000) || (so_playerStatus0() & 0x01000000)) {
        u32 cam = gabi::call<u32>(0x024F8044); /* dCam_getBody */
        gabi::call(0x02514EE4, cam, 0x10021FEC /* "BoatBattle" */, 0);
    } else if (so_playerStatus0() & 0x00100000) {
        u32 cam = gabi::call<u32>(0x024F8044);
        gabi::call(0x02514EE4, cam, 0x10021FE0 /* "WaterBattle" */, 0);
    }
    mB70 = 2;
    fopAc_ac_c* ship = so_ship();
    if (ship == nullptr) return;
    fopAc_ac_c* pl = so_player();
    f64 abs1 = so_absXZ(gabi::ea(&pl->current.pos), gabi::ea(&mA80));
    u32 path = so_path(this);
    cLib_addCalc2(gabi::at<be<f32>>(path + 0x18), 400.0f, 0.1f, 10.0f);
    gabi::store<s16>(path + 0x22, 0x100);
    gabi::store<f32>(path + 0x1C, 50.0f);
    gabi::store<f32>(path + 0, (f32)pl->current.pos.x);
    gabi::store<f32>(path + 4, (f32)pl->current.pos.y);
    gabi::store<f32>(path + 8, (f32)pl->current.pos.z);
    gabi::store<f32>(path + 4, (f32)so_getWaterY(path, gabi::ea(&mAcch)));
    so_setCirclePath(path);
    f32 wy = (f32)so_getWaterY(path + 0xC, gabi::ea(&mAcch));
    gabi::store<f32>(path + 0x10, gabi::fadds_ppc(wy, mB34));
    f64 d2 = so_absXZ(gabi::ea(&current.pos), path + 0xC);
    bool keep;
    if (d2 > 150.0f || ship->speedF > 10.0f) {
        mAFC = 12.0f;
        s16 target = cLib_targetAngleY(&current.pos, gabi::at<cXyz>(path + 0xC));
        cLib_addCalcAngleS2(&shape_angle.y, target, 8, 0x400);
        f32 a7c = mA7C;
        f32 y = gabi::fadds_ppc(current.pos.y, mB34);
        mB04 = 0.0f;
        current.pos.y = y;
        keep = abs1 < a7c;
    } else {
        cLib_addCalc2(&mB04, 1.0f, 0.01f, 0.05f);
        gabi::Local<cXyz> d;
        cXyz_mi(path + 0xC, gabi::ea(d.get()), gabi::ea(&current.pos));
        gabi::Local<cXyz> m;
        cXyz_ml(gabi::ea(d.get()), gabi::ea(m.get()), mB04);
        gabi::Local<cXyz> p;
        cXyz_pl(gabi::ea(&current.pos), gabi::ea(p.get()), gabi::ea(m.get()));
        s16 ang = (s16)(gabi::load<s16>(path + 0x20) + 0x8000);
        so_word_copy3(gabi::ea(&current.pos), gabi::ea(p.get()));
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
VERIFY(0x022E06A4, &daNpc_So_c::modeNearSwim);

/* 022E09B0 */
void daNpc_So_c::modeEventFirstWaitInit() {
    WWHD_FUNC(0x022E09B0, void, this);
    mA90 = 150;
    mAFC = 0.0f;
    speedF = 0.0f;
    offsetDive();
}
VERIFY(0x022E09B0, &daNpc_So_c::modeEventFirstWaitInit);

/* 022E09CC */
void daNpc_So_c::modeEventFirstWait() {
    WWHD_FUNC(0x022E09CC, void, this);
    fopAc_ac_c* ship = so_ship();
    if (ship == nullptr) return;
    so_word_copy3(gabi::ea(&current.pos), gabi::ea(&ship->current.pos)); /* current.pos = ship->current.pos */
    f64 abs = so_absXZ(gabi::ea(&ship->current.pos), gabi::ea(&mAAC));
    if (abs < so_hioF(0x54)) return;
    /* HD: the sail or item 0x77 */
    if (!dComIfGs_checkGetItem(0x78) && !dComIfGs_checkGetItem(0x77)) return;
    if (!dComIfGs_isStageBossEnemy(3 /* dSv_save_c::STAGE_DRC */)) return;
    if (so_playerStatus0() & 0x10000 /* daPyStts0_SHIP_RIDE_e */) {
        so_modeProcInit(this, MODE_EVENT_FIRST_e);
    }
}
VERIFY(0x022E09CC, &daNpc_So_c::modeEventFirstWait);

/* 022E0AB4 */
void daNpc_So_c::offsetSwim() {
    WWHD_FUNC(0x022E0AB4, void, this);
    mB38.x = -40.0f;
    mB38.y = 10.0f;
    mB38.z = 0.3f;
}
VERIFY(0x022E0AB4, &daNpc_So_c::offsetSwim);

/* 022E0ADC */
void daNpc_So_c::modeEventFirstInit() {
    WWHD_FUNC(0x022E0ADC, void, this);
    offsetSwim();
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}
VERIFY(0x022E0ADC, &daNpc_So_c::modeEventFirstInit);

/* 022E0B14 (GameCube: Nonmatching) */
void daNpc_So_c::cutProc() {
    WWHD_FUNC(0x022E0B14, void, this);
    s32 staffId = gabi::call<s32>(0x02542D88, dComIfGp_ea() + 0x52C4, 0x10021FFC /* "NpcSo" */, 0, 0); /* getMyStaffId */
    mB6C = staffId;
    if (staffId == -1) return;
    s32 actIdx = gabi::call<s32>(0x02542EDC, dComIfGp_ea() + 0x52C4, staffId, SO_CUT_NAME_TBL, 0x15, 1, 0); /* getMyActIdx */
    staffId = mB6C;
    if (actIdx == -1) {
        gabi::call(0x02543280, dComIfGp_ea() + 0x52C4, staffId); /* cutEnd */
        return;
    }
    BOOL adv = gabi::call<BOOL>(0x025447C8, dComIfGp_ea() + 0x52C4, staffId); /* getIsAddvance */
    u32 ent = SO_CUT_TBL + actIdx * 16;
    if (adv) {
        so_pmf_call(this, ent);     /* start */
    }
    so_pmf_call(this, ent + 8);     /* proc */
}
VERIFY(0x022E0B14, &daNpc_So_c::cutProc);

static inline bool so_demoAccept(daNpc_So_c* t) { return gabi::load<u16>(gabi::ea(t) + 0xF8) == 2; } /* eventInfo.checkCommandDemoAccrpt() */
static inline void so_cut(daNpc_So_c* t) {
    if (!t->mEventCut.cutProc()) {
        t->cutProc();
    }
}

/* 022E0C80 */
void daNpc_So_c::modeEventFirst() {
    WWHD_FUNC(0x022E0C80, void, this);
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld(0x1002224C /* "SO_1ST_MEET" */)) {
            so_event_onEventFlag8();
            mB70 = 1;
            so_modeProcInit(this, MODE_TALK_e);
            gabi::store<u32>(gabi::ea(this) + 0x39C, 0x0200000A); /* attention_info.flags */
        }
    } else {
        mB70 = 3;
    }
}
VERIFY(0x022E0C80, &daNpc_So_c::modeEventFirst);

/* 022E0D38 */
void daNpc_So_c::modeEventFirstEndInit() {
    WWHD_FUNC(0x022E0D38, void, this);
    offsetAppear();
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}
VERIFY(0x022E0D38, &daNpc_So_c::modeEventFirstEndInit);

/* 022E0D70 */
void daNpc_So_c::modeEventFirstEnd() {
    WWHD_FUNC(0x022E0D70, void, this);
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld(0x10022258 /* "SO_1ST_MEET_END" */)) {
            so_event_onEventFlag8();
            so_modeProcInit(this, MODE_DISAPPEAR_e);
        }
    } else if (talk(1) == 0x12 /* fopMsgStts_BOX_CLOSED_e */) {
        mB70 = 4;
    }
}
VERIFY(0x022E0D70, &daNpc_So_c::modeEventFirstEnd);

/* 022E0E28 */
void daNpc_So_c::modeEventEsaInit() {
    WWHD_FUNC(0x022E0E28, void, this);
    offsetSwim();
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}
VERIFY(0x022E0E28, &daNpc_So_c::modeEventEsaInit);

/* 022E0E60 */
void daNpc_So_c::modeEventEsa() {
    WWHD_FUNC(0x022E0E60, void, this);
    so_cut(this);
    if (so_endCheck(mBDC)) {
        so_event_onEventFlag8();
        mB70 = 1;
        mBDC = -1;
        so_modeProcInit(this, MODE_TALK_e);
    }
}
VERIFY(0x022E0E60, &daNpc_So_c::modeEventEsa);

/* 022E0EF4 */
void daNpc_So_c::modeEventMapopenInit() {
    WWHD_FUNC(0x022E0EF4, void, this);
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}
VERIFY(0x022E0EF4, &daNpc_So_c::modeEventMapopenInit);

/* 022E0F10 */
void daNpc_So_c::modeEventMapopen() {
    WWHD_FUNC(0x022E0F10, void, this);
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld(0x10022268 /* "SO_MAPOPEN" */)) {
            so_event_onEventFlag8();
            mB70 = 1;
            mBD8 = 1;
            so_modeProcInit(this, MODE_TALK_e);
        }
    } else if (talk(1) == 0x12) {
        mB70 = 5;
    }
}
VERIFY(0x022E0F10, &daNpc_So_c::modeEventMapopen);

/* 022E0FD4 */
void daNpc_So_c::modeEventBowInit() {
    WWHD_FUNC(0x022E0FD4, void, this);
    offsetSwim();
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
}
VERIFY(0x022E0FD4, &daNpc_So_c::modeEventBowInit);

/* 022E100C */
void daNpc_So_c::modeEventBow() {
    WWHD_FUNC(0x022E100C, void, this);
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld(0x10022274 /* "SO_BOW" */)) {
            mB0C = 1;
            so_event_onEventFlag8();
            mBDB = 0;
            so_onEventBit(0x3A10);
            s8 camId = gabi::load<s8>(dComIfGp_ea() + 0x5B30); /* dComIfGp_getPlayerCameraID(0) */
            u32 play = dComIfGp_ea();
            /* the reset position and centre are passed by value (lfs/stfs copies) */
            gabi::Local<cXyz> ctr;
            gabi::Local<cXyz> eye;
            f32 bc0x = mBC0.x, bc0z = mBC0.z, bccx = mBCC.x, bccy = mBCC.y, bc0y = mBC0.y;
            u32 cam = gabi::load<u32>(play + camId * 0x34 + 0x5AF8);
            ctr->y = bccy;
            eye->y = bc0y;
            eye->x = bc0x;
            ctr->x = bccx;
            f32 bccz = mBCC.z;
            eye->z = bc0z;
            ctr->z = bccz;
            gabi::call(0x0251510C, cam + 0x248, ctr.get(), eye.get()); /* dCamera_c::Reset(mBCC, mBC0) */
            gabi::call(0x02514F38, cam + 0x248);                      /* dCamera_c::Start */
            mB70 = 1;
            so_modeProcInit(this, MODE_TALK_e);
        }
    } else if (talk(1) == 0x12) {
        mB70 = 6;
    }
}
VERIFY(0x022E100C, &daNpc_So_c::modeEventBow);

/* 022E116C */
void daNpc_So_c::modeTalkInit() {
    WWHD_FUNC(0x022E116C, void, this);
    offsetAppear();
    so_setAnm(this, 3, false);
    m_jnt.mbBackBoneLock = 0;
}
VERIFY(0x022E116C, &daNpc_So_c::modeTalkInit);

/* 022E11AC */
void daNpc_So_c::modeTalk() {
    WWHD_FUNC(0x022E11AC, void, this);
    if (talk(1) == 0x12) {
        so_event_onEventFlag8();
        so_modeProcInit(this, MODE_DISAPPEAR_e);
    }
}
VERIFY(0x022E11AC, &daNpc_So_c::modeTalk);

/* 022E1204 */
void daNpc_So_c::modeDisappearInit() {
    WWHD_FUNC(0x022E1204, void, this);
    if (!so_isEventBit(0x901)) {
        so_onEventBit(0x901);
    }
    offsetDive();
    so_seStart(this, 0x593B /* JA_SE_CM_SO_DIVE */, 0);
    gabi::call(0x025DAE64, &current.pos, gabi::fmuls_ppc(scale.x, 1.2f), 1.4f, 0); /* fopKyM_createWpillar */
    m_jnt.mbBackBoneLock = 0;
}
VERIFY(0x022E1204, &daNpc_So_c::modeDisappearInit);

/* 022E12B4 */
void daNpc_So_c::modeDisappear() {
    WWHD_FUNC(0x022E12B4, void, this);
    if (std::fabs(gabi::fsubs_ppc(mB34, mB38.x)) < 10.0f) {
        /* HD: no new hiding place is searched here (no searchTagSo loop, mA79 and current.pos
         * are left alone) */
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
VERIFY(0x022E12B4, &daNpc_So_c::modeDisappear);

/* 022E136C */
void daNpc_So_c::modeDebugInit() {
    WWHD_FUNC(0x022E136C, void, this);
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
VERIFY(0x022E136C, &daNpc_So_c::modeDebugInit);

/* 022E140C */
void daNpc_So_c::modeGetRupee() {
    WWHD_FUNC(0x022E140C, void, this);
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld(0x10022280 /* "SO_GET_RUPEE" */)) {
            so_event_onEventFlag8();
            mB70 = 1;
            mBD9 = 1;
            so_modeProcInit(this, MODE_TALK_e);
        }
    } else if (talk(1) == 0x12) {
        if (gabi::load<s16>(0x1047BD5A) != 0) { /* REG12_S(9) */
            so_event_onEventFlag8();
        }
        /* daPy_py_c::cancelOriginalDemo() (inline): mDemo.setDemoType(2), mDemo.setParam0(1)? */
        u32 pl = gabi::ea(so_player());
        gabi::store<u16>(pl + 0x420, 2);
        gabi::store<u32>(pl + 0x430, 1);
        mB70 = 7;
    }
}
VERIFY(0x022E140C, &daNpc_So_c::modeGetRupee);

/* 022E1508 */
void daNpc_So_c::modeEventTriForceInit() {
    WWHD_FUNC(0x022E1508, void, this);
    m_jnt.mbBackBoneLock = 0;
    mAFC = 0.0f;
    speedF = 0.0f;
    offsetAppear();
    so_setAnm(this, 1, false);
}
VERIFY(0x022E1508, &daNpc_So_c::modeEventTriForceInit);

/* 022E1548 */
void daNpc_So_c::modeEventTriForce() {
    WWHD_FUNC(0x022E1548, void, this);
    if (so_demoAccept(this)) {
        so_cut(this);
        if (so_endCheckOld(0x10022290 /* "SO_TRIFORCE_CHECK" */)) {
            gabi::store<u32>(gabi::ea(this) + 0x39C, 0x0200000A); /* attention_info.flags */
            so_event_onEventFlag8();
            so_modeProcInit(this, MODE_DISAPPEAR_e);
            so_onEventBit(0x3A20);
        }
    } else {
        mB70 = 8;
    }
}
VERIFY(0x022E1548, &daNpc_So_c::modeEventTriForce);

/* 022E160C */
bool daNpc_So_c::checkTgHit() {
    WWHD_FUNC(0x022E160C, bool, this);
    fopAc_ac_c* player = so_player();
    gabi::call(0x02515E50, gabi::ea(&mStts2) + 0x1C); /* mStts2.Move() (dCcD_GStts::Move) */
    if (so_calcTimerI(&m6D8) != 0) return false;
    if (!gabi::call<BOOL>(0x025162A4, &mSph)) return false; /* ChkTgHit */
    u32 pos = gabi::ea(&mSph) + 0xCC;                        /* GetTgHitPosP() */
    u32 obj = gabi::call<u32>(0x02516300, &mSph);            /* GetTgHitObj() */
    m6D8 = so_hioS(0x7C);
    if (obj == 0) return false;
    if (gabi::load<u32>(obj + 0x10) == 0x4000 /* AT_TYPE_NORMAL_ARROW */) {
        so_seStart(this, 0x2879 /* JA_SE_LK_ARROW_HIT */, 0x20);
    }
    so_monsSeStart(this, 0x4991 /* JA_SE_CV_SO_DAMAGE */, 0);
    dComIfGp_particle_set(0x10 /* ID_AK_JN_CRITICALHITFLASH */, gabi::at<cXyz>(pos));
    gabi::Local<cXyz> sc;
    sc->x = 2.0f;
    sc->z = 2.0f;
    sc->y = 2.0f;
    dComIfGp_particle_set(0xF /* ID_AK_JN_CRITICALHIT */, gabi::at<cXyz>(pos), &player->shape_angle, sc.get());
    so_seStart(this, 0x2828 /* JA_SE_LK_LAST_HIT */, 0);
    return true;
}
VERIFY(0x022E160C, &daNpc_So_c::checkTgHit);

/* 022E179C */
u16 daNpc_So_c::next_msgStatus(be<u32>* pMsgNo) {
    WWHD_FUNC(0x022E179C, u16, this, pMsgNo);
    u16 ret = 0xF;
    u32 msgNo = *pMsgNo;
    s32 m6d0 = m6D0;
    u32 msgMgr = gabi::load<u32>(0x101F4B5C); /* HD message manager (select number at +0x948) */
    if (msgNo == (u32)m6d0) {
        if (mBD8) {
            bool first = false;
            if (!so_isEventBit(0x901) && so_isStartStage(0x100222A8 /* "sea" */) &&
                fopAcM_GetRoomNo(this) == 0xD /* dIsleRoom_DragonRoostIsland_e */)
            {
                first = true;
            }
            *pMsgNo = first ? 0x32CE : 0x32D2;
        } else {
            *pMsgNo = 0x32D6;
        }
        return ret;
    }
    /* HD: after the first-meeting and the map-hint messages, an HD-only notification (0268A858) */
    bool notify = msgNo == 0x32CA || msgNo == 0x32D0;
    u32 hd = gabi::load<u32>(0x101F8344);
    switch (msgNo) {
    case 0x633:
        if (gabi::call<s32>(0x025B7E00, gabi::load<u32>(0x101F84DC) + 0xD4) == 8) { /* dComIfGs_getTriforceNum() */
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
        /* HD: dComIfGs_isSaveArriveGrid through the HD map object */
        if (gabi::call<BOOL>(0x02689174, gabi::load<u32>(gabi::load<u32>(0x101F8344) + 0x218), fopAcM_GetRoomNo(this) - 1) ||
            so_hioU8(0x2F) != 0)
        {
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
        gabi::call(0x025D5218, 0x022DDFF4 /* searchMinigameTagSo_CB */, this); /* fopAcM_Search */
        if (so_hioU8(0x30) != 0 || mBAE != 0) {
            if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x68) != 0xFF /* dComIfGs_getItem(dInvSlot_BOW_e) */ &&
                gabi::load<u32>(dComIfGp_ea() + 0x5CF0) == 0 /* dComIfG_getTimerPtr() */)
            {
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
        if (gabi::load<u32>(msgMgr + 0x948) == 0) {
            *pMsgNo = 0x32DA;
            break;
        }
        *pMsgNo = 0x32D9;
        break;
    case 0x32DC:
        if (gabi::load<u32>(msgMgr + 0x948) == 0) {
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
        u32 p = dComIfGp_ea() + 0x5B48; /* dComIfGp_setItemRupeeCount */
        gabi::store<s32>(p, gabi::load<s32>(p) + add);
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
        gabi::call(0x0268A858, gabi::load<u32>(hd + 0x218));
    }
    return ret;
}
VERIFY(0x022E179C, &daNpc_So_c::next_msgStatus);

/* 022E1CA8 */
u32 daNpc_So_c::getMsg() {
    WWHD_FUNC(0x022E1CA8, u32, this);
    if (mB0C) {
        s32 n;
        if (so_hioU8(0x2E) != 0 || (n = mB7C) >= 10) {
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
        gabi::store<s16>(dComIfGp_ea() + 0x5BA0, (s16)n); /* dComIfGp_setMessageCountNumber */
        return 0x32DF;
    }
    bool first = false;
    if (!so_isEventBit(0x901) && so_isStartStage(0x100222AC /* "sea" */) && fopAcM_GetRoomNo(this) == 0xD) {
        first = true;
    }
    if (mBD8) {
        return (u32)(s32)m6D0;
    }
    return first ? 0x32CA : 0x32D0;
}
VERIFY(0x022E1CA8, &daNpc_So_c::getMsg);

/* 022E1E70: modeWait / modeDebug / modeGetRupeeInit (empty, merged) */
void daNpc_So_c::modeWait() {
    WWHD_FUNC(0x022E1E70, void, this);
}
VERIFY(0x022E1E70, &daNpc_So_c::modeWait);

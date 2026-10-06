/**
 * d_a_player_main_03.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #03 (023EDF70..02400B13): proc inits (hammer, guard,
 * rolls, food, bottle, ship, damage, dead, swim), item/button action checks, demo data, stick data,
 * damage dispatch, position/ground/foot handling.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Functions of other ranges are called by address (gabi::call), not as C++ members.
 * HD folds the GameCube HIO values (m_HIO->...) into constants; they are written as literals.
 */
#include "d/actor/d_a_player_main.h"

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* play + 0x5CD8 / 0x5CDC: dComIfGp_setPlayerStatus0/1(0, flag) */
static inline void dComIfGp_onPlayerStatus0_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CD8;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
static inline void dComIfGp_onPlayerStatus1_l(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
static inline u32 dComIfGp_checkPlayerStatus0_l(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & flag; }
static inline u32 dComIfGp_checkPlayerStatus1_l(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & flag; }
/* daPy_matAnm_c::offMabaFlg(): the static blink flag bytes */
static inline void daPy_matAnm_offMabaFlg_l() {
    gabi::store<u8>(0x101CEF19, 1);
    gabi::store<u8>(0x101CEF18, 0);
}
/* dAttention_c (play + 0x5804, cached at this + 0x6894): Lockon() = LockonTruth() || flag 0x20000000 at +0x20 */
static inline BOOL dAttention_LockonTruth_l(u32 att) { return gabi::call<BOOL>(0x024EDFCC, att); }

/* ---- daPy_lk_c members in the opaque blocks of the shared header ---- */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define mpEquipItemModel_ea (gabi::ea(this) + 0x4440) /* J3DModel* mpEquipItemModel (GameCube 0x2FA8 area) */
#define mSwordAnim_ea (gabi::ea(this) + 0x4444)       /* mDoExt_bckAnm mSwordAnim */

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_commonProcInit = 0x023DFDD8,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_setBlendMoveAnime = 0x023E14A0,
    LK_setBlurPosResource = 0x023E55A4,
    LK_setAtParam = 0x023E5600,
    LK_getItemAnimeResource = 0x023DDEEC, /* unnamed by the matcher */
    LK_deleteEquipItem = 0x023DC7AC,
    LK_checkHeavyStateOn = 0x023DBC24,
    LK_setItemHeap = 0x023DDF64,
    LK_initModel = 0x023D4BB8,
    LK_checkBowAnime = 0x023D6A18,
    LK_checkShipRideUseItem = 0x023E26EC,
    LK_initShipRideUseItem = 0x023E2E18,
    LK_setBottleModel = 0x023DF0FC,
    LK_setTextureAnime = 0x023DD768,
    LK_getReadyItem = 0x023EAA80,
    LK_setAnimeUnequipItem = 0x023ECD98,
    LK_changeDragonShield = 0x023E9D50,
    LK_procBootsEquip_init = 0x023E9DD0,
    LK_checkDrinkBottleItem = 0x023E9A6C, /* unnamed by the matcher */
    LK_checkOpenBottleItem = 0x023E9A84,  /* unnamed by the matcher */
    LK_procBottleSwing_init = 0x023EDD9C,
    LK_checkSetItemTrigger = 0x023E9AF4,
    LK_setAnimeEquipSword = 0x023E9F60,
    LK_setAnimeUnequip = 0x023ECEF0,
    LK_resetActAnimeUpper = 0x023DC6A4,
    LK_changeCutProc = 0x023E73FC,
    LK_procScope_init = 0x023E2358,
    LK_procFanSwing_init = 0x023E79DC,
    LK_checkNextBowMode = 0x023E8984,
    LK_checkNextHookshotMode = 0x023E8BD4,
    LK_checkNextBoomerangMode = 0x023E8F2C,
    LK_checkNextRopeMode = 0x023E91E4,
    LK_procTactWait_init = 0x023E92A0,
    LK_checkNextActionGrab = 0x023EA1AC,
    LK_checkNextActionHookshotReady = 0x023EAD5C,
    LK_checkNextActionBowReady = 0x023EB208,
    LK_checkNextActionBoomerangReady = 0x023EB510,
    LK_checkRopeAnime = 0x023D6A5C,
    LK_checkNextActionRopeReady = 0x023EC084,
    LK_setDoStatusBasic = 0x023EA680,
    LK_setDoStatus = 0x023EC498,
    LK_orderTalk = 0x023EC724,
    LK_procShipReady_init = 0x023ECA94,
    LK_procPushPullWait_init = 0x023ECF74,
    LK_changeWaitProc = 0x023E32AC,
    LK_procGrabReady_init = 0x023ED0F0,
    LK_procJumpCut_init = 0x023ED5A4,
    LK_procWHideReady_init = 0x023ED788,
    LK_procCutTurnCharge_init = 0x023ED970,
    LK_procCutTurn_init = 0x023E6380,
    LK_procWeaponNormalSwing_init = 0x023ED9E0,
    LK_procWeaponSideSwing_init = 0x023EDB6C,
    LK_procWeaponFrontSwingReady_init = 0x023EDC14,
    LK_procWeaponThrow_init = 0x023EDD14,
    LK_itemTrigger = 0x023EAB40,
    LK_changeSpecialBattle = 0x023E5400,
    LK_checkUpperReadyThrowAnime = 0x023D6ABC,
    LK_setBlendAtnMoveAnime = 0x023E81E4,
    LK_setFrontWallType = 0x023E3850,
    LK_procVomitJump_init = 0x023E47D4,
    LK_changeSlideProc = 0x023E4CD0,
    LK_checkBoomerangAnime = 0x023D69F8,
    LK_setActAnimeUpper = 0x023DE7E8,
    LK_setShipRidePos = 0x023E287C,
    LK_checkShipNotNormalMode = 0x023D69CC,
    LK_setOldRootQuaternion = 0x023E278C,
    LK_resetDemoTextureAnime = 0x023DDD48,
    LK_procControllWait_init = 0x023E2EFC,
    LK_procWait_init = 0x023E2FF4,
};
/* mDoExt_blkAnm-style ratio set (inline in HD): the weight of animation `idx` for every joint of the
 * model's joint tree; 027F3F94 (matcher: __nw) resolves a self-relative offset, 027E0174 counts the joints */
static inline void lk_pbSetRatio(u32 pb, u32 idx, f32 v) {
    u32 res = gabi::call<u32>(0x027F3F94, gabi::load<u32>(gabi::load<u32>(pb + 0x80) + 0xAC));
    s32 n = gabi::call<s32>(0x027E0174, res, 0);
    if (n > 0) {
        for (u32 i = 0; i < (u32)n; i++) {
            gabi::store<f32>(gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + 0xC + idx * 0x10) + i * 4, v);
        }
    }
}
static inline void cXyz_pl_l(u32 a, cXyz* res, u32 b) { gabi::call(0x0201AD78, a, res, b); }
static inline void lk_copy12(u32 dst, cXyz* src) {
    u32 s = gabi::ea(src);
    gabi::store<u32>(dst + 0, gabi::load<u32>(s + 0));
    gabi::store<u32>(dst + 4, gabi::load<u32>(s + 4));
    gabi::store<u32>(dst + 8, gabi::load<u32>(s + 8));
}
/* dComIfGp_getShipActor(): play + 0x5B3C; daShip_c mode byte at +0x636 (setCannon/setCrane/... store it) */
static inline u32 dComIfGp_getShipActor_l() { return gabi::load<u32>(dComIfGp_ea() + 0x5B3C); }
/* mCyl's co hit actor (dCcD_GAtTgCoCommonBase::GetAc of the cylinder's co object, this + 0x7738) and ChkCoHit */
#define LK_cylGetCoHitAc() gabi::call<u32>(0x02515BBC, gabi::ea(this) + 0x7738)
#define LK_cylChkCoHit() gabi::call<BOOL>(0x02516464, gabi::ea(this) + 0x765C)
#define LK_callB(addr) gabi::call<BOOL>(addr, this)
#define LK_upperIdx() gabi::load<u16>(gabi::ea(this) + 0x5888) /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
/* abs() of an s32 as GHS computes it (abs(INT_MIN) stays negative) */
static inline s32 lk_abs(s32 v) { u32 m = (u32)(v >> 31); return (s32)(((u32)v ^ m) - m); }
/* dComIfGp_getRStatus/setRStatus: play + 0x5BB5; play + 0x5BB6 (HD: checked by the Boko throw) */
static inline u8 dComIfGp_getRStatus_l() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB5); }
static inline void dComIfGp_setRStatus_l(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB5, s); }
/* camera attention status of a camera info index: play + 0x5B00 + idx * 0x34 */
static inline u32 dComIfGp_getCameraAttentionStatus_l(s32 idx) { return gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5B00); }
/* checkAttentionLock(): dAttention_c::Lockon() of mpAttention */
static inline BOOL lk_checkAttentionLock(u32 att) { return dAttention_LockonTruth_l(att) || (gabi::load<u32>(att + 0x20) & 0x20000000); }
/* dComIfGp_getSelectItem(btn): play + 0x5BBB + btn */
static inline u8 dComIfGp_getSelectItem_l(u32 btn) { return gabi::load<u8>(dComIfGp_ea() + btn + 0x5BBB); }
/* dComIfGp_getDoStatus(): play + 0x5BB7 */
static inline u8 dComIfGp_getDoStatus_l() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB7); }
/* daPy_lk_c::checkPlayerFly() (inline): the midair/hang/... mode flags */
#define LK_checkPlayerFly() (mModeFlg & 0x10452822)
/* virtual daPy_lk_c::checkPlayerGuard() (vtable slot 0x3C) */
#define LK_checkPlayerGuardV() gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0x3C), this)
/* daPy_dmEcallBack_c::checkCurse(): static u16 at 0x101CEF16 */
#define LK_checkCurse() (gabi::load<u16>(0x101CEF16) == 1)
#define mMaxNormalSpeed LK_FIELD(f32, 0x3C4) /* daPy_py_c (GameCube 0x2A8) */
#define mHDThrowActor LK_FIELD(u32, 0x8268)   /* HD-only (tail): the thrown actor followed by the camera */
#define mHDThrowAngle LK_FIELD(s16, 0x826C)   /* HD-only (tail) */
#define mHDThrowPos (gabi::ea(this) + 0x8270) /* HD-only (tail): cXyz */
#define mHD8265 LK_FIELD(u8, 0x8265)          /* HD-only (tail) */
#define mGndChkPos (*gabi::at<cXyz>(gabi::ea(this) + 0xB38)) /* mGndChk's position (dBgS_GndChk + 0x24) */
#define mHD8260 LK_FIELD(u32, 0x8260) /* HD-only (tail): blocks the item buttons when set; 0x8264 a byte set by the Tingle event order */
/* dComIfGp_event_compulsory(this): dEvt_control_c::compulsory(play + 0x51D0, actor, 0, 0xFFFF) */
static inline BOOL dComIfGp_event_compulsory_l(void* ac) { return gabi::call<BOOL>(0x02540310, dComIfGp_ea() + 0x51D0, ac, 0, 0xFFFF); }
static inline u8 dComIfGp_event_runCheck_l() { return gabi::load<u8>(dComIfGp_ea() + 0x5292); }
static inline u8 dComIfGp_event_getTalkXYBtn_l() { return gabi::load<u8>(dComIfGp_ea() + 0x52B0); }
static inline u8 dComIfGp_event_getPreItemNo_l() { return gabi::load<u8>(dComIfGp_ea() + 0x52B1); }
/* dCam_getBody()->StartEventCamera(0x12, fopAcM_GetID(this), "Type" (a per-use literal), &param, 0) (varargs) */
static inline void lk_StartEventCamera_l(fopAc_ac_c* ac, u32 typeStr, void* param) {
    u32 cam = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
    gabi::call(0x0253E70C, cam, 0x12, gabi::load<u32>(gabi::ea(ac) + 4), typeStr, param, 0);
}
/* dComIfGp_getVibration().StartShock(6, -0x21, cXyz(0, 1, 0)) (play + 0x599C) */
static inline void lk_startShockUp_l(int strength = 6) {
    u32 vib = dComIfGp_ea() + 0x599C;
    gabi::Local<cXyz> dir;
    dir->x = 0.0f;
    dir->y = 1.0f;
    dir->z = 0.0f;
    gabi::call(0x025CB374 /* dVibration_c::StartShock */, vib, strength, -0x21, dir.get());
}
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline u32 lk_getAnmMtx(u32 model, s32 jnt) {
    u32 blk = gabi::load<u32>(model + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::load<u32>(blk + 0x10) + jnt * 0x30;
}
/* dComIfGs: the save data object *(0x101F84DC) */
static inline u32 dComIfGs_base_l() { return gabi::load<u32>(0x101F84DC); }
/* mDemo (daPy_demo_c at 0x41C): demo type u16 at +4 (0x420), demo mode at +0x14 (0x430) */
#define LK_demoType LK_FIELD(u16, 0x420)
#define LK_demoMode LK_FIELD(s32, 0x430)
/* mDoExt_bckAnm::changeBckOnly (HD 025E871C) */
static inline void bckAnm_changeBckOnly_l(u32 anm, u32 bck) { gabi::call(0x025E871C, anm, bck); }
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) gabi::call_ptr(gabi::load<u32>(__vtbl + 0xE4), this, (u32)(n))
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }
/* fsel: a >= 0 ? b : c (NaN -> c) */
static inline f32 fsel_l(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }

/* 023EDF70 */
BOOL daPy_lk_c::procHammerSideSwing_init() {
    WWHD_FUNC(0x023EDF70, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x51 /* daPyProc_HAMMER_SIDE_SWING_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xAA /* ANM_HAMSWINGA */, 1.2f, 4.0f, 0x3C, 2.0f);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    LK_voiceStart(1);
    mProcVar2 = shape_angle.y;
    gabi::call(LK_setBlurPosResource, this, 0x289 /* dRes_INDEX_LKANM__HAMMERSIDE_POS_e */);
    gabi::call(LK_setAtParam, this, 0x10000 /* AT_TYPE_SKULL_HAMMER */, 4, 0, 5, 0xD, 0x11, 50.0f);
    u32 bck = gabi::call<u32>(LK_getItemAnimeResource, this, 0x99 /* dRes_INDEX_LKANM_BCK_HAMSWINGAA_e */);
    bckAnm_changeBckOnly_l(mSwordAnim_ea, bck);
    m35EC = 4.0f;
    return TRUE;
}
VERIFY(0x023EDF70, &daPy_lk_c::procHammerSideSwing_init);

/* 023EE068 */
BOOL daPy_lk_c::procHammerFrontSwingReady_init() {
    WWHD_FUNC(0x023EE068, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x52 /* daPyProc_HAMMER_FRONT_SWING_READY_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xAB /* ANM_HAMSWINGBPRE */, 1.0f, 2.0f, 0x1C, 3.0f);
    u32 att = mpAttention;
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    /* checkAttentionLock() || mStickDistance <= 0.05f */
    if (dAttention_LockonTruth_l(att) || (gabi::load<u32>(att + 0x20) & 0x20000000) || !(mStickDistance > 0.05f)) {
        mProcVar2 = shape_angle.y;
    } else {
        mProcVar2 = m34E8;
    }
    gabi::call(LK_setAtParam, this, 0x10000, 4, 0, 5, 0xD, 0x12 /* CUT_TYPE_HAMMER_FRONTSWING */, 50.0f);
    LK_voiceStart(7);
    u32 bck = gabi::call<u32>(LK_getItemAnimeResource, this, 0x9E /* dRes_INDEX_LKANM_BCK_HAMSWINGBPREA_e */);
    bckAnm_changeBckOnly_l(mSwordAnim_ea, bck);
    m35EC = 2.0f;
    m355C = 0;
    return TRUE;
}
VERIFY(0x023EE068, &daPy_lk_c::procHammerFrontSwingReady_init);

/* 023EE208 */
BOOL daPy_lk_c::procCall_init() {
    WWHD_FUNC(0x023EE208, BOOL, this);
    gabi::call(LK_commonProcInit, this, 2 /* daPyProc_CALL_e */);
    mNormalSpeed = 0.0f;
    daPy_matAnm_offMabaFlg_l();
    gabi::call(LK_setSingleMoveAnime, this, 0xC2 /* ANM_YOBU */, 1.0f, 0.0f, -1, 2.4f);
    current.angle.y = shape_angle.y;
    if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
        gabi::call(LK_deleteEquipItem, this, 0);
    }
    LK_voiceStart(0x2A);
    return TRUE;
}
VERIFY(0x023EE208, &daPy_lk_c::procCall_init);

/* 023EE2B8 */
BOOL daPy_lk_c::procCrouch_init() {
    WWHD_FUNC(0x023EE2B8, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0xE /* daPyProc_CROUCH_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x3F /* ANM_CROUCH */, 0.8f, 0.0f, -1, 5.0f);
    gabi::call(LK_deleteEquipItem, this, 0);
    current.angle.y = shape_angle.y;
    return TRUE;
}
VERIFY(0x023EE2B8, &daPy_lk_c::procCrouch_init);

#define LK_SAFESTRING_VTBL 0x10034B24u /* sead::SafeString vtable of this TU */
struct lk_SafeString_l {
    be<u32> mStr;
    be<u32> __vtbl;
};
static inline void lk_ss_assure(lk_SafeString_l* s) { gabi::call_ptr(gabi::load<u32>(s->__vtbl + 0x14), s); }
/* inline sead::SafeString::operator==(a, b) */
static inline bool lk_ss_cmp(lk_SafeString_l* a, lk_SafeString_l* b) {
    lk_ss_assure(a);
    lk_ss_assure(a);
    u32 pa = a->mStr;
    lk_ss_assure(b);
    u32 pb = b->mStr;
    if (pa == pb)
        return true;
    pa = a->mStr;
    pb = b->mStr;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(pa + n);
        u8 cb = gabi::load<u8>(pb + n);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
/* strcmp(dComIfGp_getStartStageName(), lit) == 0 (the start stage name at play + 0x5134) */
static inline bool lk_isStartStage(u32 lit) {
    gabi::Local<lk_SafeString_l> a;
    a->__vtbl = LK_SAFESTRING_VTBL;
    a->mStr = lit;
    u32 play = dComIfGp_ea();
    gabi::Local<lk_SafeString_l> b;
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mStr = play + 0x5134;
    return lk_ss_cmp(a.get(), b.get());
}
/* dComIfGp_getStageStagInfo(): play + 0x5150 is the stage object, virtual getStagInfo at slot 0x15C */
static inline u32 dComIfGp_getStageStagInfo_l() {
    u32 stg = dComIfGp_ea() + 0x5150;
    return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(stg) + 0x15C), stg);
}

/* 023EE328 */
BOOL daPy_lk_c::checkGuardAccept() {
    WWHD_FUNC(0x023EE328, BOOL, this);
    if ((mModeFlg & 0x40) && !gabi::call<BOOL>(LK_checkBowAnime, this)) {
        u32 info = dComIfGp_getStageStagInfo_l();
        /* dStage_stagInfo_GetSTType */
        if (((gabi::load<u32>(info + 0xC) >> 16) & 7) != 2 /* dStageType_MISC_e */ ||
            lk_isStartStage(0x10035708 /* "Ojhous" */) || lk_isStartStage(0x10035710 /* "Orichh" */)) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x023EE328, &daPy_lk_c::checkGuardAccept);

/* 023EE510 */
BOOL daPy_lk_c::procCrouchDefense_init() {
    WWHD_FUNC(0x023EE510, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0xC /* daPyProc_CROUCH_DEFENSE_e */);
    daPy_matAnm_offMabaFlg_l();
    gabi::call(LK_setSingleMoveAnime, this, 0x16 /* ANM_DIFENCE */, 0.8f, 0.0f, -1, 2.0f);
    current.angle.y = shape_angle.y;
    mProcVar2 = 0;
    dComIfGp_onPlayerStatus1_l(0x80000 /* daPyStts1_UNK80000_e */);
    return TRUE;
}
VERIFY(0x023EE510, &daPy_lk_c::procCrouchDefense_init);

/* 023EE5B0 */
BOOL daPy_lk_c::procSideStep_init(int dir) {
    WWHD_FUNC(0x023EE5B0, BOOL, this, dir);
    gabi::call(LK_commonProcInit, this, 0xA /* daPyProc_SIDE_STEP_e */);
    mDirection = dir;
    int anm;
    if (dir == 2 /* DIR_LEFT */) {
        anm = 0x10; /* ANM_ATNJL */
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    } else {
        anm = 0x11; /* ANM_ATNJR */
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, 0.3f, 0.0f, 4, 1.0f);
    mNormalSpeed = 30.0f * cM_scos(0x1838);
    gravity = -2.4f;
    mProcVar6 = 0;
    speed.y = 30.0f * cM_ssin(0x1838);
    LK_voiceStart(5);
    return TRUE;
}
VERIFY(0x023EE5B0, &daPy_lk_c::procSideStep_init);

/* 023EE710 */
BOOL daPy_lk_c::procBackJump_init() {
    WWHD_FUNC(0x023EE710, BOOL, this);
    if (mCurProc == 0x22 /* daPyProc_BACK_JUMP_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x22);
    if (gabi::call<bool>(LK_checkHeavyStateOn, this)) {
        mNormalSpeed = 11.25f;
        gabi::call(LK_setSingleMoveAnime, this, 0x3D /* ANM_ROLLB */, 1.2f, 2.0f, 0xB, 0.0f);
    } else {
        mNormalSpeed = 22.5f;
        gabi::call(LK_setSingleMoveAnime, this, 0x3D, 0.8f, 2.0f, 0xB, 0.0f);
    }
    speed.y = 19.0f;
    gravity = -3.0f;
    current.angle.y = (s16)(shape_angle.y + 0x8000);
    LK_voiceStart(7);
    return TRUE;
}
VERIFY(0x023EE710, &daPy_lk_c::procBackJump_init);

/* 023EE86C */
BOOL daPy_lk_c::procFrontRoll_init(f32 morf) {
    WWHD_FUNC(0x023EE86C, BOOL, this, morf);
    gabi::call(LK_commonProcInit, this, 0x1E /* daPyProc_FRONT_ROLL_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x32 /* ANM_ROLLF */, 1.1f, morf, 0x13, 2.0f);
    mNormalSpeed = gabi::fmadds(speedF, 1.53f, 20.0f);
    if (gabi::call<bool>(LK_checkHeavyStateOn, this)) {
        mNormalSpeed = mNormalSpeed * 0.5f;
    }
    if (mNormalSpeed < 5.0f) {
        mNormalSpeed = 5.0f;
    } else {
        f32 max = 26.01f;
        if (gabi::call<bool>(LK_checkHeavyStateOn, this)) {
            max = 13.005f;
        }
        if (mNormalSpeed > max) {
            mNormalSpeed = max;
        }
    }
    current.angle.y = shape_angle.y;
    if ((mAcchCir[0].m_flags & 2 /* WALL_HIT */) &&
        cLib_distanceAngleS((s16)(current.angle.y + 0x8000), gabi::load<s16>(gabi::ea(this) + 0x788) /* mAcchCir[0] wall angle */) <= 0x1388) {
        mProcVar6 = 0;
    } else {
        mProcVar6 = 1;
    }
    mNoResetFlg0 = mNoResetFlg0 & ~8u;
    mProcVar2 = 0;
    LK_voiceStart(7);
    return TRUE;
}
VERIFY(0x023EE86C, &daPy_lk_c::procFrontRoll_init);

/* 023EEA30 */
BOOL daPy_lk_c::procSubjectivity_init(int crouch) {
    WWHD_FUNC(0x023EEA30, BOOL, this, crouch);
    gabi::call(LK_commonProcInit, this, 1 /* daPyProc_SUBJECTIVITY_e */);
    mNormalSpeed = 0.0f;
    setSubjectMode();
    if (!crouch) {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f);
    }
    mProcVar6 = crouch;
    return TRUE;
}
VERIFY(0x023EEA30, &daPy_lk_c::procSubjectivity_init);

/* 023EEAA0 */
void daPy_lk_c::setHyoiModel() {
    WWHD_FUNC(0x023EEAA0, void, this);
    if (mEquipItem == 0x83 /* dItemNo_HYOI_PEAR_e */) {
        return;
    }
    u32 oldHeap = gabi::call<u32>(LK_setItemHeap, this);
    gabi::call(LK_initModel, this, mpEquipItemModel_ea, 0x1F /* dRes_INDEX_LINK_BDL_HYOINOMI_e */, 0x13000022);
    gabi::call(0x025E3570 /* mDoExt_setCurrentHeap */, oldHeap);
    mEquipItem = 0x83;
}
VERIFY(0x023EEAA0, &daPy_lk_c::setHyoiModel);

/* 023EEB10 */
void daPy_lk_c::keepItemData() {
    WWHD_FUNC(0x023EEB10, void, this);
    mKeepItem = mEquipItem;
    gabi::call(LK_deleteEquipItem, this, 0);
    if (mKeepItem == 0x101 /* daPyItem_BOKO_e */) {
        mKeepItem = 0x100; /* daPyItem_NONE_e */
    } else if (mKeepItem == 0x100) {
        mKeepItem = 0x10B; /* daPyItem_UNK10B_e */
    }
}
VERIFY(0x023EEB10, &daPy_lk_c::keepItemData);

/* 023EEB70 */
BOOL daPy_lk_c::procFoodSet_init() {
    WWHD_FUNC(0x023EEB70, BOOL, this);
    if (mCurProc == 0xA8 /* daPyProc_FOOD_SET_e */) {
        if (demoParam0() == 1) {
            if (mEquipItem == 0x83 /* dItemNo_HYOI_PEAR_e */) {
                if (dComIfGp_event_getTalkXYBtn_l() == 1 /* dTalkBtn_X_e */) {
                    gabi::call(0x025B58B8 /* dSv_player_item_c::setEquipBottleItemEmpty */, dComIfGs_base_l() + 0x5C, 0);
                } else {
                    int btn = dComIfGp_event_getTalkXYBtn_l() == 2 /* dTalkBtn_Y_e */ ? 1 : 2;
                    gabi::call(0x025B58B8, dComIfGs_base_l() + 0x5C, btn);
                }
            }
            gabi::call(LK_deleteEquipItem, this, 0);
        } else {
            setHyoiModel();
        }
        return TRUE;
    }
    if (!dComIfGp_event_runCheck_l()) {
        if (!dComIfGp_event_compulsory_l(this)) {
            return FALSE;
        }
        LK_demoType = 5; /* mDemo.setSpecialDemoType() */
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xA8);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xC5 /* ANM_SETHYOINOMI */, 1.1f, 2.0f, 0x15, 4.0f);
    keepItemData();
    setHyoiModel();
    if (LK_demoType == 5 /* checkSpecialDemoMode() */) {
        mProcVar6 = 1;
        lk_StartEventCamera_l(this, 0x10035734 /* "Type" */, &mProcVar6);
    }
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    mProcVar0 = 30;
    return TRUE;
}
VERIFY(0x023EEB70, &daPy_lk_c::procFoodSet_init);

/* 023EED74 */
BOOL daPy_lk_c::procFoodThrow_init() {
    WWHD_FUNC(0x023EED74, BOOL, this);
    if (mCurProc == 0xA7 /* daPyProc_FOOD_THROW_e */) {
        return TRUE;
    }
    if (m3630 != 0xFFFFFFFF /* fpcM_ERROR_PROCESS_ID_e */) {
        gabi::call(0x025E1988 /* seStartSystem */, 0x883 /* JA_SE_ITEM_TARGET_OUT */);
        return FALSE;
    }
    if (!dComIfGp_event_runCheck_l()) {
        if (!dComIfGp_event_compulsory_l(this)) {
            return FALSE;
        }
        LK_demoType = 5;
    } else {
        gabi::call(LK_deleteEquipItem, this, 0);
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xA7);
    if (use == 0) {
        f32 offset1 = 150.0f * cM_ssin(shape_angle.y);
        f32 offset2 = 150.0f * cM_scos(shape_angle.y);
        cXyz* attn = gabi::at<cXyz>(gabi::ea(this) + 0x390); /* attention_info.position */
        gabi::Local<cXyz> start;
        gabi::Local<cXyz> end;
        start->x = attn->x;
        start->y = attn->y - 50.0f;
        start->z = attn->z;
        end->x = start->x + offset1;
        end->y = start->y;
        end->z = start->z + offset2;
        void* chk = mLinkLinChk;
        dBgS_LinChk_Set(chk, start, end, this);
        if (cBgS_LineCross(dComIfG_Bgsp(), chk)) {
            end->x = start->x - offset1;
            end->z = start->z - offset2;
            dBgS_LinChk_Set(chk, start, end, this);
            if (!cBgS_LineCross(dComIfG_Bgsp(), chk)) {
                shape_angle.y = (s16)(shape_angle.y - 0x8000);
            } else {
                end->x = start->x + offset2;
                end->z = start->z - offset1;
                dBgS_LinChk_Set(chk, start, end, this);
                if (!cBgS_LineCross(dComIfG_Bgsp(), chk)) {
                    shape_angle.y = (s16)(shape_angle.y + 0x4000);
                } else {
                    end->x = start->x - offset2;
                    end->z = start->z + offset1;
                    dBgS_LinChk_Set(chk, start, end, this);
                    if (!cBgS_LineCross(dComIfG_Bgsp(), chk)) {
                        shape_angle.y = (s16)(shape_angle.y - 0x4000);
                    }
                }
            }
        }
    }
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    daPy_matAnm_offMabaFlg_l();
    gabi::call(LK_setSingleMoveAnime, this, 0xC4 /* ANM_ESAMAKI */, 0.6f, 0.0f, 0x11, 4.0f);
    keepItemData();
    if (LK_demoType == 5) {
        mProcVar6 = 6;
        lk_StartEventCamera_l(this, 0x1003573C /* "Type" */, &mProcVar6);
    }
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    m3630 = 0xFFFFFFFF;
    return TRUE;
}
VERIFY(0x023EED74, &daPy_lk_c::procFoodThrow_init);

/* 023EF260 */
int daPy_lk_c::changeBottleDrinkFace(int item) {
    WWHD_FUNC(0x023EF260, int, this, item);
    if (item == 0x55 /* dItemNo_SOUP_BOTTLE_e */ || item == 0x54 /* dItemNo_HALF_SOUP_BOTTLE_e */) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023EF260, &daPy_lk_c::changeBottleDrinkFace);

/* 023EF280 */
BOOL daPy_lk_c::procBottleDrink_init(u16 item) {
    WWHD_FUNC(0x023EF280, BOOL, this, item);
    if (!dComIfGp_event_compulsory_l(this)) {
        return FALSE;
    }
    LK_demoType = 5;
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xA3 /* daPyProc_BOTTLE_DRINK_e */);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xB6 /* ANM_BINDRINKPRE */, 1.0f, 0.0f, 0x51, 5.0f);
    if (changeBottleDrinkFace(item)) {
        gabi::call(LK_setTextureAnime, this, 0x85, 0);
    }
    keepItemData();
    gabi::call(LK_setBottleModel, this, (u32)item);
    dComIfGp_onPlayerStatus1_l(0x2000 /* daPyStts1_UNK2000_e */);
    mProcVar6 = 1;
    lk_StartEventCamera_l(this, 0x10035744 /* "Type" */, &mProcVar6);
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    return TRUE;
}
VERIFY(0x023EF280, &daPy_lk_c::procBottleDrink_init);

/* 023EF3E0 */
BOOL daPy_lk_c::procBottleOpen_init(u16 item) {
    WWHD_FUNC(0x023EF3E0, BOOL, this, item);
    if (mCurProc == 0xA4 /* daPyProc_BOTTLE_OPEN_e */) {
        return TRUE;
    }
    if (LK_demoMode != 0x3D /* daPy_demo_c::DEMO_BO_OPEN_e */) {
        if (!dComIfGp_event_compulsory_l(this)) {
            return FALSE;
        }
        LK_demoType = 5;
    } else {
        item = dComIfGp_event_getPreItemNo_l();
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xA4);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xB9 /* ANM_BINOPENPRE */, 1.0f, 0.0f, 0x2D, 2.0f);
    keepItemData();
    gabi::call(LK_setBottleModel, this, (u32)item);
    dComIfGp_onPlayerStatus1_l(0x4000 /* daPyStts1_UNK4000_e */);
    if (mEquipItem == 0x57 /* dItemNo_FAIRY_BOTTLE_e */) {
        mProcVar6 = 3;
    } else {
        mProcVar6 = 2;
    }
    if (LK_demoMode != 0x3D) {
        lk_StartEventCamera_l(this, 0x1003574C /* "Type" */, &mProcVar6);
    }
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    mProcVar2 = 0;
    mProcVar3 = 0;
    mProcVar0 = -1;
    mProcVar4 = 0;
    return TRUE;
}
VERIFY(0x023EF3E0, &daPy_lk_c::procBottleOpen_init);

/* 023EF61C */
BOOL daPy_lk_c::procNotUse_init(int item) {
    WWHD_FUNC(0x023EF61C, BOOL, this, item);
    if (!dComIfGp_event_compulsory_l(this)) {
        return FALSE;
    }
    LK_demoType = 5;
    gabi::call(LK_commonProcInit, this, 0xA2 /* daPyProc_NOT_USE_e */);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xB5 /* ANM_PRESENTATIONB */, 1.0f, 0.0f, 0x3E, 5.0f);
    keepItemData();
    dComIfGp_onPlayerStatus1_l(0x800 /* daPyStts1_UNK800_e */);
    mGameOverId = 0xFFFFFFFF;
    mProcVar6 = item;
    mProcVar7 = 5;
    lk_StartEventCamera_l(this, 0x10035754 /* "Type" */, &mProcVar7);
    if (gabi::call<BOOL>(0x02550FAC /* isDaizaItem */, (u32)(u8)mProcVar6)) {
        m3624 = 0xF0C;
    } else {
        m3624 = 0x835;
    }
    mProcVar2 = 0;
    return TRUE;
}
VERIFY(0x023EF61C, &daPy_lk_c::procNotUse_init);

/* 023EF750 */
void daPy_lk_c::setAnimeEquipItem() {
    WWHD_FUNC(0x023EF750, void, this);
    u32 thrown = gabi::ea(mActorKeepThrow.mActor.get());
    if (thrown != 0) {
        /* HD: the boomerang's cancel flag is set before the ready item is checked */
        gabi::store<u8>(thrown + 0x26274, 1); /* daBoomerang_c::onCancelFlg() */
        if (gabi::call<int>(LK_getReadyItem, this) == 0x2D /* dItemNo_BOOMERANG_e */) {
            return;
        }
    }
    m3562 = (u8)gabi::call<int>(LK_getReadyItem, this);
    u32 a = dComIfGp_ea() + 0x5CD8; /* dComIfGp_clearPlayerStatus0(0, daPyStts0_BOOMERANG_WAIT_e) */
    gabi::store<u32>(a, gabi::load<u32>(a) & ~0x400000u);
    gabi::call(LK_setAnimeUnequipItem, this, (u32)m3562);
}
VERIFY(0x023EF750, &daPy_lk_c::setAnimeEquipItem);

/* 023EF7CC */
BOOL daPy_lk_c::checkNewItemChange(u8 btn) {
    WWHD_FUNC(0x023EF7CC, BOOL, this, btn);
    u8 itemNo = dComIfGp_getSelectItem_l(btn);
    if (itemNo == 0x2A /* dItemNo_MAGIC_ARMOR_e */) {
        return gabi::call<BOOL>(LK_changeDragonShield, this, 1);
    }
    /* HD: item 0x77 is in the list too (0x77..0x78) */
    if (itemNo == 0x21 /* dItemNo_TINGLE_TUNER_e */ || itemNo == 0x29 /* dItemNo_IRON_BOOTS_e */ || itemNo == 0x2B ||
        (itemNo >= 0x77 && itemNo <= 0x78 /* dItemNo_SAIL_e */) || gabi::call<BOOL>(0x0255101C /* isEsa */, itemNo) ||
        gabi::call<BOOL>(0x02550FAC /* isDaizaItem */, itemNo) || gabi::call<BOOL>(0x02550FF4 /* isEmono */, itemNo) ||
        checkBottleItem(itemNo) || (u32)(itemNo - 0x98) < 7 /* dItemNo_FATHER_LETTER_e..dItemNo_FILL_UP_COUPON_e */) {
        if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !LK_checkPlayerFly()) {
            if (itemNo == 0x29 || itemNo == 0x2B) {
                gabi::call(LK_procBootsEquip_init, this, (u32)itemNo);
            } else if (gabi::call<BOOL>(0x0255101C /* isEsa */, itemNo)) {
                if (itemNo == 0x83 /* dItemNo_HYOI_PEAR_e */) {
                    return procFoodSet_init();
                } else {
                    return procFoodThrow_init();
                }
            } else if (itemNo == 0x21) {
                /* HD: the Tingle item orders a talk event with the HD object at this + 0x8264 (GameCube:
                 * fopAcM_orderTalkEvent with the AGB), only when no event runs */
                if (dComIfGp_event_runCheck_l()) {
                    return FALSE;
                }
                u32 evt = dComIfGp_ea() + 0x51D0;
                u32 hd = gabi::ea(this) + 0x8264;
                if (gabi::call<BOOL>(0x0253EC0C /* dEvt_control_c::order */, evt, 3, 1, 0, 0xFFFF, this, hd, -1, 0xFF)) {
                    gabi::store<u8>(hd, 1);
                }
            } else if (gabi::call<bool>(LK_checkDrinkBottleItem, this, (u32)itemNo)) {
                return procBottleDrink_init(itemNo);
            } else if (gabi::call<bool>(LK_checkOpenBottleItem, this, (u32)itemNo)) {
                return procBottleOpen_init(itemNo);
            } else if (itemNo == 0x50 /* dItemNo_EMPTY_BOTTLE_e */) {
                return gabi::call<BOOL>(LK_procBottleSwing_init, this, 1);
            } else if (gabi::call<BOOL>(0x02520FE4 /* dComIfGs_checkGetItemNum */, itemNo) != 0) {
                return procNotUse_init(itemNo);
            } else {
                return FALSE;
            }
            return TRUE;
        }
        if (itemNo != 0x50) {
            return FALSE;
        }
    }
    if (mEquipItem != itemNo && itemNo != 0xFF /* dItemNo_NONE_e */) {
        if (itemNo == 0x31 /* dItemNo_BOMB_BAG_e */ &&
            (mActivePlayerBombs >= 3 || gabi::load<u8>(dComIfGs_base_l() + 0x8A) /* dComIfGs_getBombNum() */ == 0)) {
            gabi::call(0x025E1988 /* seStartSystem */, 0x883 /* JA_SE_ITEM_TARGET_OUT */);
            return FALSE;
        }
        mReadyItemBtn = btn;
        setAnimeEquipItem();
    }
    return FALSE;
}
VERIFY(0x023EF7CC, &daPy_lk_c::checkNewItemChange);

/* 023EFB68 */
BOOL daPy_lk_c::checkItemChangeFromButton() {
    WWHD_FUNC(0x023EFB68, BOOL, this);
    /* HD: the sword button ends the HD proc 0xDB */
    if ((mItemTrigger & 2) && mCurProc == 0xDB) {
        gabi::call(LK_deleteEquipItem, this, 0);
        dComIfGp_evmng_cutEnd(mStaffIdx);
    }
    if ((mModeFlg & 4) && !checkEquipAnime() && mActorKeepGrab.mActor.get() == nullptr && !LK_checkPlayerGuardV()) {
        if (!LK_checkCurse()) {
            u8 trig = mItemTrigger;
            if (trig & 2 /* swordTrigger() */) {
                /* checkSwordEquip(): dComIfGs_getSelectEquip(0) != NONE || checkSwordMiniGame() */
                if (gabi::load<u8>(dComIfGs_base_l() + 0x2E) != 0xFF || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2) {
                    if (mEquipItem != 0x103 /* daPyItem_SWORD_e */) {
                        gabi::call(LK_setAnimeEquipSword, this, 1);
                        return FALSE;
                    }
                }
                if (mHD8260 != 0) {
                    goto do_trigger;
                }
                trig = mItemTrigger;
            } else if (mHD8260 != 0) {
                goto do_trigger;
            }
            if (trig & 4 /* itemTriggerX() */) {
                if (checkNewItemChange(0)) {
                    mReadyItemBtn = 0;
                    return TRUE;
                }
            } else if (trig & 8 /* itemTriggerY() */) {
                if (checkNewItemChange(1)) {
                    mReadyItemBtn = 1;
                    return TRUE;
                }
            } else if (trig & 0x10 /* itemTriggerZ() */) {
                if (checkNewItemChange(2)) {
                    mReadyItemBtn = 2;
                    return TRUE;
                }
            } else if (trig & 0x80 /* HD: a fourth item button */) {
                if (checkNewItemChange(3)) {
                    mReadyItemBtn = 3;
                    return TRUE;
                }
            } else {
            do_trigger:
                if ((mItemTrigger & 1) && dComIfGp_getDoStatus_l() == 8 /* dActStts_PUT_AWAY_e */) {
                    gabi::call(LK_setAnimeUnequip, this);
                }
            }
        } else {
            if ((noResetFlg1() & 1 /* daPyFlg1_EQUIP_DRAGON_SHIELD */) && gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x2A, 0)) {
                setNoResetFlg1(noResetFlg1() & ~1u);
                return FALSE;
            }
            if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !LK_checkPlayerFly()) {
                if ((mNoResetFlg0 & 0x02000000 /* daPyFlg0_EQUIP_HEAVY_BOOTS */) && gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x29, 0)) {
                    return gabi::call<BOOL>(LK_procBootsEquip_init, this, 0x29);
                } else if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x105 /* daPyItem_DRINK_BOTTLE_e */, 0)) {
                    u8 btn = mReadyItemBtn;
                    return procBottleDrink_init(dComIfGp_getSelectItem_l(btn));
                } else if (gabi::call<BOOL>(LK_checkSetItemTrigger, this, 0x57 /* dItemNo_FAIRY_BOTTLE_e */, 0)) {
                    return procBottleOpen_init(0x57);
                }
            }
            if ((mItemTrigger & 1) && dComIfGp_getDoStatus_l() == 8) {
                gabi::call(LK_setAnimeUnequip, this);
            }
        }
    }
    return FALSE;
}
VERIFY(0x023EFB68, &daPy_lk_c::checkItemChangeFromButton);

/* 023EFF20 */
BOOL daPy_lk_c::checkNextActionFromButton() {
    WWHD_FUNC(0x023EFF20, BOOL, this);
    if (resetFlg0() & 0x80 /* daPyRFlg0_UNK80 */) {
        u16 eq = mEquipItem;
        if (eq == 0x103 /* daPyItem_SWORD_e */) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
            return LK_callB(LK_changeCutProc);
        }
        if (checkPhotoBoxItem(eq) || eq == 0x20 /* dItemNo_TELESCOPE_e */) {
            return gabi::call<BOOL>(LK_procScope_init, this, (u32)eq);
        }
        if (eq == 0x34 /* dItemNo_DEKU_LEAF_e */) {
            return LK_callB(LK_procFanSwing_init);
        }
        if (checkBowItem(eq)) {
            return LK_callB(LK_checkNextBowMode);
        }
        if (eq == 0x2F /* dItemNo_HOOKSHOT_e */) {
            return LK_callB(LK_checkNextHookshotMode);
        }
        if (eq == 0x2D /* dItemNo_BOOMERANG_e */) {
            return LK_callB(LK_checkNextBoomerangMode);
        }
        if (eq == 0x25 /* dItemNo_GRAPPLING_HOOK_e */) {
            return LK_callB(LK_checkNextRopeMode);
        }
        if (eq == 0x22 /* dItemNo_WIND_WAKER_e */) {
            return gabi::call<BOOL>(LK_procTactWait_init, this, -1);
        }
    }
    u16 upper = LK_upperIdx();
    if (upper == 0x95 || upper == 0x96 /* checkGrabAnime() */) {
        if (LK_callB(LK_checkNextActionGrab)) {
            return TRUE;
        }
        gabi::call(LK_setDoStatusBasic, this);
    } else if (upper == 0xA7 /* checkHookshotReadyAnime() */) {
        if (LK_callB(LK_checkNextActionHookshotReady)) {
            return TRUE;
        }
        gabi::call(LK_setDoStatusBasic, this);
    } else if (LK_callB(LK_checkBowAnime)) {
        if (LK_callB(LK_checkNextActionBowReady)) {
            return TRUE;
        }
        gabi::call(LK_setDoStatusBasic, this);
    } else if (upper == 0x34 /* checkBoomerangThrowAnime() */) {
        return TRUE;
    } else if (upper == 0x35 /* checkBoomerangReadyAnime() */) {
        if (LK_callB(LK_checkNextActionBoomerangReady)) {
            return TRUE;
        }
        gabi::call(LK_setDoStatusBasic, this);
    } else if (LK_callB(LK_checkRopeAnime)) {
        if (LK_callB(LK_checkNextActionRopeReady)) {
            return TRUE;
        }
        gabi::call(LK_setDoStatusBasic, this);
    } else {
        gabi::call(LK_setDoStatus, this);
        /* HD: the GameCube's spActionTrigger push/pull check moved under the do button (do status 0x11) */
        if (LK_callB(LK_orderTalk)) {
            return TRUE;
        }
        if (mItemTrigger & 1 /* doTrigger() */) {
            u8 doStatus = dComIfGp_getDoStatus_l();
            u32 play = dComIfGp_ea();
            if (doStatus == 0x31 /* dActStts_ba_sake__dupe_31 */) {
                /* dComIfGp_att_getZHint(): dAttCatch_c at play + 0x5928 */
                u32 zhint = gabi::call<u32>(0x024EBAE8 /* dAttCatch_c::convPId */, play + 0x5928, gabi::load<u32>(play + 0x5930));
                gabi::call(0x025D7640 /* fopAcM_orderZHintEvent */, this, zhint);
                return TRUE;
            }
            if (gabi::load<u8>(play + 0x5BB7) == 0x1C /* dActStts_GET_IN_SHIP_e */) {
                return LK_callB(LK_procShipReady_init);
            }
            if (dComIfGp_getDoStatus_l() == 0x11) {
                return gabi::call<BOOL>(LK_procPushPullWait_init, this, 1);
            }
            /* HD: no CLIMB (procHangWallCatch_init / procVerticalJump_init) here */
            if (dComIfGp_getDoStatus_l() == 0xB /* dActStts_OPEN_e */) {
                u32 entry = mpAttnEntryA;
                fopAc_ac_c* actor = mpAttnActorA;
                if (gabi::load<s32>(entry + 8) /* mType */ == 5) {
                    gabi::call(0x025D7710 /* fopAcM_orderDoorEvent */, this, actor);
                    gabi::call(LK_changeWaitProc, this);
                } else {
                    gabi::call(0x025D7C08 /* fopAcM_orderTreasureEvent */, this, actor);
                }
                return TRUE;
            }
            if (dComIfGp_getDoStatus_l() == 4 /* dActStts_LIFT_e */ || dComIfGp_getDoStatus_l() == 0x1B /* dActStts_PICK_UP_e */) {
                return LK_callB(LK_procGrabReady_init);
            }
            if (dComIfGp_getDoStatus_l() == 0x51 /* dActStts_UNK43 */) {
                return gabi::call<BOOL>(LK_procJumpCut_init, this, 0);
            }
            if (dComIfGp_getDoStatus_l() == 0x10 /* dActStts_SIDLE_e */) {
                return gabi::call<BOOL>(LK_procWHideReady_init, this, nullptr, &m3724);
            }
        }
    }
    if (!checkEquipAnime() && !LK_checkPlayerGuardV()) {
        if (!LK_checkCurse()) {
            u16 eq = mEquipItem;
            if (eq == 0x103 /* daPyItem_SWORD_e */) {
                if (mNoResetFlg0 & 4 /* daPyFlg0_UNK4 */) {
                    return LK_callB(LK_procCutTurnCharge_init);
                }
                if (mItemTrigger & 2 /* swordTrigger() */) {
                    if (lk_abs(m3578) > 0xF800) {
                        return gabi::call<BOOL>(LK_procCutTurn_init, this, 1);
                    }
                    return LK_callB(LK_changeCutProc);
                }
                if (m34C5 != 0) {
                    return LK_callB(LK_changeCutProc);
                }
            } else if (eq == 0x101 /* daPyItem_BOKO_e */) {
                if (mNoResetFlg0 & 4) {
                    return LK_callB(LK_procCutTurnCharge_init);
                }
                /* HD: the do button swings the Boko stick; the sword button throws it (play + 0x5BB6 == 0xE) */
                if (mItemTrigger & 1) {
                    fopAc_ac_c* weapon = mActorKeepEquip.mActor;
                    if (weapon != nullptr) {
                        if (lk_abs(m3578) > 0xF800) {
                            return gabi::call<BOOL>(LK_procCutTurn_init, this, 1);
                        }
                        u32 prm = weapon->mParameters;
                        if (prm == 0 || prm == 1) {
                            return LK_callB(LK_procWeaponNormalSwing_init);
                        } else if (prm == 4) {
                            return LK_callB(LK_procWeaponSideSwing_init);
                        } else {
                            return LK_callB(LK_procWeaponFrontSwingReady_init);
                        }
                    }
                    if (dComIfGp_getDoStatus_l() == 0xE /* dActStts_THROW_e */) {
                        return LK_callB(LK_procWeaponThrow_init);
                    }
                }
                if ((mItemTrigger & 2) && gabi::load<u8>(dComIfGp_ea() + 0x5BB6) == 0xE) {
                    return LK_callB(LK_procWeaponThrow_init);
                }
            }
            u32 ready;
            if (LK_callB(LK_itemTrigger) && (ready = gabi::call<u32>(LK_getReadyItem, this), mEquipItem == ready)) {
                eq = mEquipItem;
                if (eq == 0x2F) {
                    if (LK_upperIdx() != 0xA7) {
                        return LK_callB(LK_checkNextHookshotMode);
                    }
                } else if (eq == 0x25) {
                    if (!LK_callB(LK_checkRopeAnime)) {
                        return LK_callB(LK_checkNextRopeMode);
                    }
                } else {
                    if (eq == 0x2D) {
                        return LK_callB(LK_checkNextBoomerangMode);
                    }
                    if (checkBowItem(eq)) {
                        return LK_callB(LK_checkNextBowMode);
                    }
                    if (eq == 0x34) {
                        return LK_callB(LK_procFanSwing_init);
                    }
                    if (eq == 0x50 /* dItemNo_EMPTY_BOTTLE_e */) {
                        return gabi::call<BOOL>(LK_procBottleSwing_init, this, 0);
                    }
                    if (eq == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
                        int dir = getDirectionFromShapeAngle();
                        if (mStickDistance > 0.05f && (dir == 2 /* DIR_LEFT */ || dir == 3 /* DIR_RIGHT */)) {
                            return procHammerSideSwing_init();
                        }
                        return procHammerFrontSwingReady_init();
                    }
                    if (checkPhotoBoxItem(eq) || eq == 0x20) {
                        return gabi::call<BOOL>(LK_procScope_init, this, (u32)eq);
                    }
                    if (eq == 0x22) {
                        return gabi::call<BOOL>(LK_procTactWait_init, this, -1);
                    }
                }
            }
        }
        if (LK_callB(LK_changeSpecialBattle)) {
            return TRUE;
        }
    }
    if (dComIfGp_getRStatus_l() == 0 /* dActStts_BLANK_e */ && !LK_callB(LK_checkUpperReadyThrowAnime) && !checkEquipAnime()) {
        u16 up = LK_upperIdx();
        if (up != 0x95 && up != 0x96 /* !checkGrabAnime() */) {
            if (noResetFlg1() & 2 /* daPyFlg1_NPC_CALL_COMMAND */) {
                dComIfGp_setRStatus_l(0x24 /* dActStts_CALL_e */);
                if (mItemTrigger & 0x40 /* spActionTrigger() */) {
                    return procCall_init();
                }
            } else if (mEquipItem == 0x101) {
                dComIfGp_setRStatus_l(9 /* dActStts_DROP_e */);
                if (mItemTrigger & 0x40) {
                    gabi::call(LK_deleteEquipItem, this, 0);
                    gabi::call(0x025E3F3C /* mDoExt_MtxCalcOldFrame::initOldFrameMorf */, (u32)m_old_fdata, 5.0f, 0, 0x2A);
                    return TRUE;
                }
            } else if (gabi::load<u8>(dComIfGs_base_l() + 0x2F) /* dComIfGs_getSelectEquip(1) */ == 0xFF || mEquipItem == 0x100) {
                if (mModeFlg & 1) {
                    dComIfGp_setRStatus_l(0xF /* dActStts_CROUCH_e */);
                    if (mItemButton & 0x40 /* spActionButton() */) {
                        return procCrouch_init();
                    }
                }
            } else {
                if (checkGuardAccept()) {
                    dComIfGp_setRStatus_l(0x36 /* dActStts_DEFEND_e */);
                    if (mItemButton & 0x40) {
                        return procCrouchDefense_init();
                    }
                }
            }
        }
    }
    if (mItemTrigger & 1) {
        if (dComIfGp_getDoStatus_l() == 0x12 /* dActStts_JUMP_e */) {
            int dir = getDirectionFromShapeAngle();
            if (dir == 2 || dir == 3) {
                return procSideStep_init(dir);
            }
            if (dir == 1 /* DIR_BACKWARD */) {
                return procBackJump_init();
            }
        } else if (dComIfGp_getDoStatus_l() == 0xC /* dActStts_ATTACK_e */) {
            if (!lk_checkAttentionLock(mpAttention) && mStickDistance > 0.05f) {
                shape_angle.y = m34E8;
            }
            return procFrontRoll_init(0.0f);
        }
    }
    if (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) == gabi::ea(this) /* daPy_getPlayerActorClass() == this */ &&
        !dComIfGp_event_runCheck_l() && !(LK_FIELD(f32, 0x3CC) < 0.0f) /* !checkGrabWear() */) {
        setResetFlg0(resetFlg0() | 0x04000000 /* daPyRFlg0_SUBJECT_ACCEPT */);
        s32 camIdx = mCameraInfoIdx;
        if (dComIfGp_getCameraAttentionStatus_l(camIdx) & 0x1000) {
            return procSubjectivity_init(0);
        }
    }
    return checkItemChangeFromButton();
}
VERIFY(0x023EFF20, &daPy_lk_c::checkNextActionFromButton);

/* 023F0CE4 */
BOOL daPy_lk_c::checkAtnWaitAnime() {
    WWHD_FUNC(0x023F0CE4, BOOL, this);
    u32 lock = gabi::ea(mpAttnActorLockOn.get());
    if ((lock != 0 && (gabi::load<u8>(lock + 0x2DA) == 2 /* fopAc_ENEMY_e */ ||
                       (lock != 0 && gabi::load<s16>(lock + 8) == 0x1B0 /* fpcNm_BOOMERANG_e */))) ||
        LK_demoMode == 0x17 /* daPy_demo_c::DEMO_A_WAIT_e */) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023F0CE4, &daPy_lk_c::checkAtnWaitAnime);

/* 023F0D2C */
BOOL daPy_lk_c::procAtnActorWait_init() {
    WWHD_FUNC(0x023F0D2C, BOOL, this);
    if (mCurProc == 8 /* daPyProc_ATN_ACTOR_WAIT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 8);
    s32 angle;
    fopAc_ac_c* lock = mpAttnActorLockOn;
    if (lock != nullptr) {
        angle = gabi::call<s32>(0x025D6894 /* fopAcM_searchActorAngleY */, this, lock);
    } else {
        angle = 0;
    }
    if (mDirection == 2 /* DIR_LEFT */ || (mDirection != 3 /* DIR_RIGHT */ && angle - shape_angle.y >= 0)) {
        mDirection = 2;
    } else {
        mDirection = 3;
    }
    if (checkAtnWaitAnime()) {
        gabi::call(LK_setBlendAtnMoveAnime, this, 2.4f);
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f);
    }
    return TRUE;
}
VERIFY(0x023F0D2C, &daPy_lk_c::procAtnActorWait_init);

/* 023F0E18 */
BOOL daPy_lk_c::procAtnActorMove_init() {
    WWHD_FUNC(0x023F0E18, BOOL, this);
    if (mCurProc == 9 /* daPyProc_ATN_ACTOR_MOVE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 9);
    gabi::call(LK_setBlendAtnMoveAnime, this, 2.4f);
    mProcVar0 = 0x14;
    return TRUE;
}
VERIFY(0x023F0E18, &daPy_lk_c::procAtnActorMove_init);

/* 023F0E8C */
BOOL daPy_lk_c::procAtnMove_init() {
    WWHD_FUNC(0x023F0E8C, BOOL, this);
    if (mCurProc == 7 /* daPyProc_ATN_MOVE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 7);
    gabi::call(LK_setBlendAtnMoveAnime, this, 2.4f);
    mProcVar0 = 0x14;
    return TRUE;
}
VERIFY(0x023F0E8C, &daPy_lk_c::procAtnMove_init);

/* 023F0F00 */
BOOL daPy_lk_c::procWaitTurn_init() {
    WWHD_FUNC(0x023F0F00, BOOL, this);
    if (mCurProc == 0x17 /* daPyProc_WAIT_TURN_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x17);
    gabi::call(LK_setSingleMoveAnime, this, 0x14 /* ANM_ROT */, 1.0f, 0.0f, -1, 2.4f);
    if (dComIfGp_event_runCheck_l()) {
        mNormalSpeed = 0.0f;
    }
    mProcVar2 = m34E8;
    current.angle.y = shape_angle.y;
    return TRUE;
}
VERIFY(0x023F0F00, &daPy_lk_c::procWaitTurn_init);

/* 023F0FD0 */
BOOL daPy_lk_c::procMoveTurn_init(int param) {
    WWHD_FUNC(0x023F0FD0, BOOL, this, param);
    if (mCurProc == 0x18 /* daPyProc_MOVE_TURN_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x18);
    gabi::call(LK_setBlendMoveAnime, this, 2.4f);
    dComIfGp_onPlayerStatus0_l(0x800 /* daPyStts0_UNK800_e */);
    if (param != 0) {
        mProcVar2 = 0x7936;
        mProcVar3 = 0x1770;
        mProcVar0 = 2;
        current.angle.y = m34E8;
        mNormalSpeed = mNormalSpeed * 0.5f;
    } else {
        mProcVar2 = 0x1770;
        mProcVar3 = 0xBB8;
        mProcVar0 = 3;
    }
    return TRUE;
}
VERIFY(0x023F0FD0, &daPy_lk_c::procMoveTurn_init);

/* 023F10B8 */
BOOL daPy_lk_c::procSlip_init() {
    WWHD_FUNC(0x023F10B8, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x19 /* daPyProc_SLIP_e */);
    daPy_matAnm_offMabaFlg_l();
    gabi::call(LK_setSingleMoveAnime, this, 0x34 /* ANM_SLIP */, 0.4f, 0.0f, -1, 1.7f);
    mNormalSpeed = speedF * 1.1f; /* HD: the HIO max-speed mode is 0 (no clamp) */
    mFootEffectPosType = 2;
    seStartMapInfo(0x280D /* JA_SE_LK_RUN_SLIP */);
    return TRUE;
}
VERIFY(0x023F10B8, &daPy_lk_c::procSlip_init);

/* 023F1154 */
BOOL daPy_lk_c::procMove_init() {
    WWHD_FUNC(0x023F1154, BOOL, this);
    if (mCurProc == 6 /* daPyProc_MOVE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 6);
    gabi::call(LK_setBlendMoveAnime, this, 2.4f);
    mProcVar0 = 0x14;
    return TRUE;
}
VERIFY(0x023F1154, &daPy_lk_c::procMove_init);

/* 023F11C8 */
BOOL daPy_lk_c::procVomitWait_init() {
    WWHD_FUNC(0x023F11C8, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x97 /* daPyProc_VOMIT_WAIT_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xA7 /* ANM_VOMITJMP */, 1.0f, 0.0f, -1, 5.0f);
    u32 ac = LK_cylGetCoHitAc();
    u32 me = gabi::ea(this);
    /* current.pos = ac->current.pos; old.pos = current.pos (integer copies) */
    u32 x = gabi::load<u32>(ac + 0x314);
    gabi::store<u32>(me + 0x314, x);
    u32 y = gabi::load<u32>(ac + 0x318);
    gabi::store<u32>(me + 0x318, y);
    u32 z = gabi::load<u32>(ac + 0x31C);
    current.angle.y = shape_angle.y;
    gabi::store<u32>(me + 0x31C, z);
    gabi::store<u32>(me + 0x300, x);
    m34C2 = 11;
    gabi::store<u32>(me + 0x304, y);
    mProcVar2 = 0;
    mNormalSpeed = 0.0f;
    gabi::store<u32>(me + 0x308, z);
    dComIfGp_onPlayerStatus0_l(0x80 /* daPyStts0_UNK80_e */);
    mProcVar0 = 0; /* as the GameCube PAL version */
    return TRUE;
}
VERIFY(0x023F11C8, &daPy_lk_c::procVomitWait_init);

/* 023F12A4 */
BOOL daPy_lk_c::procVomitReady_init(s16 angle, f32 dist) {
    WWHD_FUNC(0x023F12A4, BOOL, this, angle, dist);
    gabi::call(LK_commonProcInit, this, 0x96 /* daPyProc_VOMIT_READY_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x72 /* ANM_MSTEPOVER */, 0.3f, 1.0f, 6, 10.0f);
    f32 g = gravity;
    speed.y = 28.0f;
    mNormalSpeed = -((dist * g) / 56.0f);
    if (gabi::call<bool>(LK_checkHeavyStateOn, this)) {
        speed.y = speed.y * 1.5f;
        mNormalSpeed = mNormalSpeed * 1.5f;
    }
    current.angle.y = angle;
    shape_angle.y = angle;
    LK_voiceStart(6);
    return TRUE;
}
VERIFY(0x023F12A4, &daPy_lk_c::procVomitReady_init);

/* 023F13A0 */
BOOL daPy_lk_c::checkJumpFlower() {
    WWHD_FUNC(0x023F13A0, BOOL, this);
    u32 ac = LK_cylGetCoHitAc();
    if (LK_cylChkCoHit() && ac != 0 && gabi::load<s16>(ac + 8) == 0xD5 /* fpcNm_JBO_e */) {
        if (mModeFlg & 2 /* ModeFlg_MIDAIR */) {
            if (!(speed.y > 0.0f) && current.pos.y > gabi::load<f32>(ac + 0x318) - 50.0f) {
                return procVomitWait_init();
            }
        } else {
            s32 angle = gabi::call<s32>(0x025D6894 /* fopAcM_searchActorAngleY */, this, ac);
            if (gabi::call<s32>(0x0200FAAC /* cLib_distanceAngleS */, angle, (s32)shape_angle.y) < 0x2000 && mStickDistance > 0.2f) {
                f64 dist = gabi::call<f64>(0x025D6958 /* fopAcM_searchActorDistanceXZ */, this, ac);
                return gabi::call<BOOL>(0x023F12A4 /* procVomitReady_init */, this, angle, dist);
            }
        }
    }
    return FALSE;
}
VERIFY(0x023F13A0, &daPy_lk_c::checkJumpFlower);

/* 023F14E0 */
BOOL daPy_lk_c::checkNextMode(int noStick) {
    WWHD_FUNC(0x023F14E0, BOOL, this, noStick);
    f32 oldMax = mMaxNormalSpeed;
    gabi::call(LK_setFrontWallType, this);
    if (noResetFlg1() & 0x10 /* daPyFlg1_FORCE_VOMIT_JUMP */) {
        return gabi::call<BOOL>(LK_procVomitJump_init, this, 2);
    }
    if (noResetFlg1() & 0x10000 /* daPyFlg1_FORCE_VOMIT_JUMP_SHORT */) {
        return gabi::call<BOOL>(LK_procVomitJump_init, this, 3);
    }
    BOOL atn = lk_checkAttentionLock(mpAttention) ||
               (mActorKeepThrow.mActor.get() != nullptr && gabi::ea(mpAttnActorLockOn.get()) == gabi::ea(mActorKeepThrow.mActor.get())) ||
               LK_callB(LK_checkUpperReadyThrowAnime) || LK_demoMode == 0x17 /* DEMO_A_WAIT_e */;
    if (atn) {
        mMaxNormalSpeed = 12.0f;
    } else {
        mMaxNormalSpeed = 17.0f;
    }
    if (LK_callB(LK_changeSlideProc)) {
        return TRUE;
    }
    if (checkNextActionFromButton()) {
        return TRUE;
    }
    if (noStick != 0 && !(mStickDistance > 0.05f) && !(mItemButton & 0x40 /* spActionButton() */)) {
        mMaxNormalSpeed = oldMax;
        return FALSE;
    }
    BOOL r;
    if (atn) {
        if (LK_callB(LK_checkBoomerangAnime)) {
            r = LK_callB(LK_checkNextBoomerangMode);
        } else if (LK_callB(LK_checkBowAnime)) {
            r = LK_callB(LK_checkNextBowMode);
        } else if (LK_upperIdx() == 0xA7 /* checkHookshotReadyAnime() */) {
            r = LK_callB(LK_checkNextHookshotMode);
        } else if (LK_callB(LK_checkRopeAnime)) {
            r = LK_callB(LK_checkNextRopeMode);
        } else if (mpAttnActorLockOn.get() != nullptr || LK_demoMode == 0x17) {
            if (std::fabs((f32)mNormalSpeed) > 0.001f) {
                r = procAtnActorMove_init();
            } else {
                r = procAtnActorWait_init();
            }
        } else if (std::fabs((f32)mNormalSpeed) > 0.001f) {
            r = procAtnMove_init();
        } else {
            r = LK_callB(LK_changeWaitProc);
        }
    } else {
        mDirection = 4; /* DIR_NONE */
        int dir = getDirectionFromCurrentAngle();
        if (!(std::fabs((f32)mNormalSpeed) > 0.001f)) {
            if (cLib_distanceAngleS(m34E8, current.angle.y) > 0x7800 && mStickDistance > 0.05f) {
                r = procWaitTurn_init();
                if (!r && !dComIfGp_event_runCheck_l() && LK_demoType == 0 /* !checkPlayerDemoMode() */) {
                    r = LK_callB(LK_changeWaitProc);
                }
            } else {
                r = LK_callB(LK_changeWaitProc);
            }
        } else {
            s16 angleY = current.angle.y;
            if (mCurProc == 0x18 /* daPyProc_MOVE_TURN_e */ && angleY != shape_angle.y) {
                r = procMoveTurn_init(0);
            } else if (cLib_distanceAngleS(m34E8, angleY) > 0x7800 && mStickDistance > 0.05f) {
                if (speedF / mMaxNormalSpeed > 0.6f && mCurrAttributeCode != 0xF /* dBgS_Attr_ICE_e */ &&
                    LK_upperIdx() != 0x95 && LK_upperIdx() != 0x96 &&
                    getDirectionFromAngle((s16)(m34EA - m34DC)) == 1 /* DIR_BACKWARD */) {
                    r = procSlip_init();
                } else {
                    r = procMoveTurn_init(1);
                }
            } else if (dir == 1 && mStickDistance > 0.05f) {
                r = procMoveTurn_init(1);
            } else {
                r = procMove_init();
            }
        }
    }
    if (!r) {
        r = checkJumpFlower();
    }
    return r;
}
VERIFY(0x023F14E0, &daPy_lk_c::checkNextMode);

/* 023F19D0 */
void daPy_lk_c::initShipBaseAnime() {
    WWHD_FUNC(0x023F19D0, void, this);
    if (gabi::load<u16>(gabi::ea(this) + 0x5848) /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */ != 0xF4 /* dRes_INDEX_LKANM_BCK_SHIP_JUMP2_e */) {
        gabi::call(LK_setSingleMoveAnime, this, 0x7D /* ANM_VOYAGE1 */, 1.0f, 0.0f, -1, 5.0f);
    }
    gabi::call(LK_setActAnimeUpper, this, 0x89 /* dRes_INDEX_LKANM_BCK_FN_HAIR1_e */, 2 /* UPPER_MOVE2_e */, 1.0f, 0.0f, -1, -1.0f);
    lk_pbSetRatio(m_pbCalc[1], 2, 0.0f); /* m_pbCalc[PART_UPPER_e]->setRatio(2, 0.0f) */
}
VERIFY(0x023F19D0, &daPy_lk_c::initShipBaseAnime);

/* 023F1AC8 */
BOOL daPy_lk_c::procShipCannon_init() {
    WWHD_FUNC(0x023F1AC8, BOOL, this);
    if (mCurProc == 0x8E /* daPyProc_SHIP_CANNON_e */) {
        return FALSE;
    }
    u32 ship = dComIfGp_getShipActor_l();
    gabi::call(LK_commonProcInit, this, 0x8E);
    gabi::call(LK_deleteEquipItem, this, 1);
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    gabi::store<u8>(ship + 0x636, 9); /* ship->setCannon() */
    initShipBaseAnime();
    gabi::call(LK_setShipRidePos, this, 1);
    dComIfGp_onPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */);
    dComIfGp_onPlayerStatus1_l(4 /* daPyStts1_UNK4_e */);
    return TRUE;
}
VERIFY(0x023F1AC8, &daPy_lk_c::procShipCannon_init);

/* 023F1B90 */
void daPy_lk_c::initShipCraneAnime() {
    WWHD_FUNC(0x023F1B90, void, this);
    int anm;
    if (gabi::load<s16>(dComIfGp_getShipActor_l() + 0x682) /* getCraneBaseAngle() */ > 0) {
        anm = 0xCD; /* ANM_SALVRWAIT */
        mProcVar7 = 3;
        mProcVar6 = 3;
    } else {
        anm = 0xCE; /* ANM_SALVLWAIT */
        mProcVar7 = 2;
        mProcVar6 = 2;
    }
    daPy_matAnm_offMabaFlg_l();
    gabi::call(LK_setSingleMoveAnime, this, anm, 1.0f, 0.0f, -1, 10.0f);
}
VERIFY(0x023F1B90, &daPy_lk_c::initShipCraneAnime);

/* 023F1C68 */
BOOL daPy_lk_c::procShipCrane_init() {
    WWHD_FUNC(0x023F1C68, BOOL, this);
    if (mCurProc == 0x8F /* daPyProc_SHIP_CRANE_e */) {
        return FALSE;
    }
    u32 ship = dComIfGp_getShipActor_l();
    gabi::call(LK_commonProcInit, this, 0x8F);
    gabi::call(LK_deleteEquipItem, this, 1);
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    gabi::store<u8>(ship + 0x636, 0xA); /* ship->setCrane() */
    if (gabi::load<s16>(ship + 0x686) /* getRopeCnt() */ != 0) {
        initShipCraneAnime();
    } else {
        initShipBaseAnime();
    }
    gabi::call(LK_setShipRidePos, this, 1);
    dComIfGp_onPlayerStatus0_l(0x10000);
    dComIfGp_onPlayerStatus1_l(2 /* daPyStts1_UNK2_e */);
    mProcVar2 = 0;
    return TRUE;
}
VERIFY(0x023F1C68, &daPy_lk_c::procShipCrane_init);

/* 023F1D84 */
BOOL daPy_lk_c::procShipSteer_init() {
    WWHD_FUNC(0x023F1D84, BOOL, this);
    mNoResetFlg0 = mNoResetFlg0 & ~1u; /* offNoResetFlg0(daPyFlg0_UNK1) */
    u32 ship = dComIfGp_getShipActor_l();
    if (mCurProc == 0x88 /* daPyProc_SHIP_STEER_e */) {
        gabi::store<u8>(ship + 0x636, 1); /* ship->setSteerMove() */
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x88);
    gabi::call(LK_deleteEquipItem, this, 1);
    gravity = 0.0f;
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    gabi::store<u8>(ship + 0x636, 1);
    initShipBaseAnime();
    gabi::call(LK_setShipRidePos, this, 1);
    dComIfGp_onPlayerStatus0_l(0x10000);
    return TRUE;
}
VERIFY(0x023F1D84, &daPy_lk_c::procShipSteer_init);

/* 023F1E60 */
BOOL daPy_lk_c::procShipPaddle_init() {
    WWHD_FUNC(0x023F1E60, BOOL, this);
    mNoResetFlg0 = mNoResetFlg0 & ~0x800000u; /* offNoResetFlg0(daPyFlg0_UNK800000) */
    if (mCurProc == 0x89 /* daPyProc_SHIP_PADDLE_e */) {
        return FALSE;
    }
    u32 ship = dComIfGp_getShipActor_l();
    BOOL riding = dComIfGp_checkPlayerStatus0_l(0x10000) && !LK_callB(LK_checkShipNotNormalMode);
    gabi::call(LK_commonProcInit, this, 0x89);
    gabi::call(LK_deleteEquipItem, this, 1);
    mNormalSpeed = 0.0f;
    gravity = 0.0f;
    speed.y = 0.0f;
    gabi::store<u8>(ship + 0x636, 2); /* ship->setPaddleMove() */
    initShipBaseAnime();
    if (!riding) {
        gabi::call(LK_setOldRootQuaternion, this, 0, (s32)(s16)(shape_angle.y - gabi::load<s16>(ship + 0x32A)), 0);
    }
    gabi::call(LK_setShipRidePos, this, 1);
    dComIfGp_onPlayerStatus0_l(0x10000);
    mProcVar0 = (s16)gabi::ftoi(cM_rndF(150.0f) + 300.0f);
    return TRUE;
}
VERIFY(0x023F1E60, &daPy_lk_c::procShipPaddle_init);

/* 023F2048 */
void daPy_lk_c::endDemoMode() {
    WWHD_FUNC(0x023F2048, void, this);
    u32 mode = (u32)LK_demoMode;
    if (mode == 6 || mode == 0x25) {
        mHD8260 = 0x1E; /* HD: after a talk/present demo the item buttons stay blocked for 30 frames */
    }
    if (mCurProc != 0x68 /* daPyProc_LARGE_DAMAGE_e */) {
        current.angle.y = shape_angle.y;
    }
    mNoResetFlg0 = mNoResetFlg0 & ~0x100000u; /* offNoResetFlg0(daPyFlg0_UNK100000) */
    u32 st = dComIfGp_ea() + 0x5CD8; /* dComIfGp_clearPlayerStatus0(0, daPyStts0_UNK10_e) */
    gabi::store<u32>(st, gabi::load<u32>(st) & ~0x10u);
    BOOL wasSpecial = LK_demoType == 5; /* HD */
    LK_demoMode = 0;
    LK_FIELD(s32, 0x42C) = 0; /* mDemo.setParam1(0) */
    LK_FIELD(s32, 0x428) = 0; /* mDemo.setParam0(0) */
    LK_demoType = 0;
    LK_FIELD(f32, 0x434) = 1.0f; /* mDemo.setStick(1.0f) */
    gabi::call(LK_resetDemoTextureAnime, this);
    if (mEquipItem == 0x104 /* daPyItem_UNK104_e */ || mEquipItem == 0x10A /* daPyItem_UNK10A_e */) {
        gabi::call(LK_deleteEquipItem, this, 0);
        setNoResetFlg1(noResetFlg1() & ~0x1000u); /* offShipTact() */
    }
    gabi::call(0x025E1E88 /* mDoAud_taktModeMuteOff */);
    if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !(mModeFlg & 0x14452822) && mCurProc != 0x69 /* LARGE_DAMAGE_UP */ &&
        mCurProc != 0xB2 /* DEMO_DEAD */ && mCurProc != 0xB0 /* DEMO_LAVA_DAMAGE */) {
        if (dComIfGp_checkPlayerStatus0_l(0x200000 /* daPyStts0_TELESCOPE_LOOK_e */)) {
            gabi::call(LK_procScope_init, this, 0x20);
            return;
        }
        gabi::call(LK_changeWaitProc, this);
        return;
    }
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        u32 ship = dComIfGp_getShipActor_l();
        if (ship == 0) {
            checkNextMode(0);
        } else if (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) != gabi::ea(this) /* daPy_getPlayerActorClass() != this */) {
            gabi::call(LK_procControllWait_init, this);
        } else {
            u8 part = gabi::load<u8>(ship + 0x637); /* ship->getPart() */
            if (part == 2 /* PART_CANNON_e */) {
                procShipCannon_init();
            } else if (part == 3 /* PART_CRANE_e */) {
                procShipCrane_init();
            } else if (part == 1 /* PART_STEER_e */) {
                procShipSteer_init();
            } else {
                procShipPaddle_init();
            }
        }
    } else {
        s32 proc = mCurProc;
        /* HD: a special demo also ends in procWait_init unless the proc is one of three ranges */
        if (proc == 0xC9 /* daPyProc_DEMO_BOSS_WARP_e */ ||
            (wasSpecial && !((u32)(proc - 0xF) < 4) && !((u32)(proc - 0x96) < 4) && !((u32)(proc - 0x76) < 0xA))) {
            gabi::call(LK_procWait_init, this);
        }
    }
}
VERIFY(0x023F2048, &daPy_lk_c::endDemoMode);

/* ---- HD-only procs (no GameCube source): the inits of the HD message-bottle procs whose bodies are in
 * range #06 (dProcHDBottleThrow 0xDC, dProcHDLetterWrite 0xDB). Names are descriptive. ---- */

/* 023F42EC HD: init of proc 0xDC (throw the message bottle; the camera follows it) */
BOOL daPy_lk_c::dProcHDBottleThrow_init() {
    WWHD_FUNC(0x023F42EC, BOOL, this);
    if (mCurProc == 0xDC) {
        return TRUE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::call(LK_commonProcInit, this, 0xDC);
    mNormalSpeed = 0.0f;
    gabi::call(LK_initShipRideUseItem, this, use, 2);
    gabi::call(LK_deleteEquipItem, this, 0);
    u32 item = gabi::call<u32>(0x025D7F20 /* fopAcM_fastCreateItem2 */, &current.pos, 0x13, -1, -1, 0, 0, 0, 0);
    gabi::call(0x023DE638 /* daPy_actorKeep_c::setData */, &mActorKeepGrab, item);
    mEquipItem = 0x100;
    gabi::call(LK_setSingleMoveAnime, this, 0 /* ANM_WAITS */, 1.0f, 0.0f, -1, 5.0f);
    gabi::call(LK_setActAnimeUpper, this, 0x95, 2, 0.0f, 0.0f, -1, 5.0f);
    for (s32 i = 0; i < LK_FIELD(s32, 0x5840); i++) { /* [?] the HD weight array (count +0x5840, data +0x5844) */
        gabi::store<f32>(LK_FIELD(u32, 0x5844) + i * 4, 0.0f);
    }
    u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
    m35A4 = 0.0f;
    if (grab == 0) {
        return FALSE;
    }
    gabi::call(0x025D9D0C /* fopAcM_setCarryNow */, grab, 0);
    mHDThrowActor = grab;
    mProcVar7 = 1;
    m35C8 = 17.0f;
    lk_StartEventCamera_l(this, 0x100357F8 /* "Type" */, &mProcVar7);
    u32 body = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
    gabi::call(0x0253E860 /* dCamera_c::EndEventCamera */, body, gabi::load<u32>(gabi::ea(this) + 4));
    s32 camIdx = mCameraInfoIdx;
    u32 cam = gabi::load<u32>(dComIfGp_ea() + camIdx * 0x34 + 0x5AF8);
    u32 tid = 0xFFFFFFFF;
    if (mHDThrowActor != 0) {
        tid = gabi::load<u32>(mHDThrowActor + 4);
    }
    body = gabi::call<u32>(0x024F8044);
    gabi::call(0x0253E70C /* dCamera_c::StartEventCamera */, body, 6, tid, 0x100357F8, 1, 0);
    gabi::call(0x02514F2C /* dCamera_c::Stop */, cam + 0x248);
    tid = 0xFFFFFFFF;
    if (mHDThrowActor != 0) {
        tid = gabi::load<u32>(mHDThrowActor + 4);
    }
    gabi::call(0x02515470 /* dCamera_c::ForceLockOn */, cam + 0x248, tid);
    gabi::Local<cXyz> eye;
    gabi::Local<cXyz> eye2;
    cXyz_pl_l(cam + 0x264, eye, cam + 0x7C0); /* the camera eye */
    lk_copy12(gabi::ea(&m370C), eye);
    cXyz_pl_l(cam + 0x264, eye, cam + 0x7C0);
    lk_copy12(mHDThrowPos, eye);
    cXyz_pl_l(cam + 0x264, eye, cam + 0x7C0);
    cXyz_pl_l(cam + 0x264, eye2, cam + 0x7C0);
    mHDThrowAngle = cM_atan2s(current.pos.x - eye->x, current.pos.z - eye2->z);
    if (mHD8265 != 0) {
        mProcVar6 = 3;
        gabi::Local<cXyz> center;
        gabi::Local<cXyz> eye3;
        cXyz_pl_l(cam + 0x258, center, cam + 0x7B4);
        cXyz_pl_l(cam + 0x264, eye3, cam + 0x7C0);
        gabi::call(0x02514F88 /* dCamera_c::Set */, cam + 0x248, center.get(), eye3.get(), 60.0f, 0);
        m35A8 = 0.0f;
        m35A0 = 0.0f;
        mHD8265 = 0;
        m35A4 = 0.0f;
    } else {
        m35A8 = 0.0f;
        m35A4 = 0.0f;
        mProcVar6 = 4;
        m35A0 = 0.0f;
    }
    return TRUE;
}
VERIFY(0x023F42EC, &daPy_lk_c::dProcHDBottleThrow_init);

/* 023F4688 HD: init of proc 0xDB (the message: letter animation and a text input) */
BOOL daPy_lk_c::dProcHDLetterWrite_init() {
    WWHD_FUNC(0x023F4688, BOOL, this);
    if (mCurProc != 0xDB) {
        mProcVar7 = 1;
        lk_StartEventCamera_l(this, 0x10035800 /* "Type" */, &mProcVar7);
        int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
        gabi::call(LK_commonProcInit, this, 0xDB);
        mNormalSpeed = 0.0f;
        gabi::call(LK_setSingleMoveAnime, this, 0xB2, 1.0f, 0.0f, -1, 5.0f);
        gabi::call(LK_deleteEquipItem, this, 0);
        gabi::call(LK_initShipRideUseItem, this, use, 2);
        m35A4 = 0.0f;
        LK_FIELD(f32, 0x8274) = 0.0f;
        mProcVar6 = 0;
        LK_FIELD(f32, 0x8278) = 0.0f;
        mHDThrowAngle = 0;
        m35A0 = 0.0f;
        mHDThrowActor = 0;
        m370C.y = 0.0f;
        m35A8 = 0.0f;
        m370C.z = 0.0f;
        LK_FIELD(f32, 0x8270) = 0.0f;
        m370C.x = 0.0f;
        u32 x = gabi::call<u32>(0x02035D78);
        gabi::call(0x02035ED4, x, 0);
    }
    return TRUE;
}
VERIFY(0x023F4688, &daPy_lk_c::dProcHDLetterWrite_init);

/* 023F47C0 */
BOOL daPy_lk_c::dProcTool_init() {
    WWHD_FUNC(0x023F47C0, BOOL, this);
    if (mCurProc == 0xA9 /* daPyProc_DEMO_TOOL_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xA9);
    speed.y = 0.0f;
    speedF = 0.0f;
    mNormalSpeed = 0.0f;
    LK_FIELD(s16, 0x584E) = -1; /* m_anm_heap_under[UNDER_MOVE0_e].field_0x6 */
    LK_FIELD(s16, 0x584C) = -1; /* m_anm_heap_under[UNDER_MOVE0_e].field_0x4 */
    LK_FIELD(s16, 0x5868) = -1; /* m_anm_heap_upper[UPPER_MOVE0_e].mIdx */
    LK_FIELD(s16, 0x5858) = -1; /* m_anm_heap_under[UNDER_MOVE1_e].mIdx */
    LK_FIELD(s16, 0x5878) = -1; /* m_anm_heap_upper[UPPER_MOVE1_e].mIdx */
    lk_pbSetRatio(m_pbCalc[1], 0, 1.0f);
    lk_pbSetRatio(m_pbCalc[1], 1, 0.0f);
    lk_pbSetRatio(m_pbCalc[0], 0, 1.0f);
    lk_pbSetRatio(m_pbCalc[0], 1, 0.0f);
    LK_FIELD(u32, 0x582C) = 0; /* mAnmRatioUpper[1].setAnmTransform(NULL) */
    mProcVar3 = 0;
    mProcVar7 = 0;
    mProcVar2 = 0;
    LK_FIELD(u32, 0x580C) = 0; /* mAnmRatioUnder[1].setAnmTransform(NULL) */
    mProcVar6 = 0;
    return TRUE;
}
VERIFY(0x023F47C0, &daPy_lk_c::dProcTool_init);

/* 023F49A4 */
int daPy_lk_c::setTalkStartBack() {
    WWHD_FUNC(0x023F49A4, int, this);
    u32 partner = gabi::call<u32>(0x025D7C6C /* fopAcM_getTalkEventPartner */, this);
    if (partner != 0) {
        gabi::Local<cXyz> d;
        cXyz_mi(&current.pos, d, gabi::at<cXyz>(partner + 0x314));
        gabi::Local<cXyz> xz; /* absXZ() */
        xz->x = d->x;
        xz->y = 0.0f;
        xz->z = d->z;
        f64 sq = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get());
        f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, sq);
        if (len < 100.0 && len > 1.0) {
            f32 k = (f32)(100.0 / len);
            f32 x = gabi::fmadds(k, d->x, gabi::load<f32>(partner + 0x314));
            f32 y = current.pos.y + 30.1f;
            m370C.x = x;
            m370C.y = y;
            gabi::store<f32>(gabi::ea(this) + 0xB3C, y); /* mGndChk.SetPos(&m370C) */
            f32 z = gabi::fmadds(k, d->z, gabi::load<f32>(partner + 0x31C));
            gabi::store<f32>(gabi::ea(this) + 0xB38, x);
            m370C.z = z;
            gabi::store<f32>(gabi::ea(this) + 0xB40, z);
            f64 gnd = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
            if (!((f32)(gnd - (f64)(f32)current.pos.y) < -30.1f)) {
                current.angle.y = cM_atan2s(d->x, d->z);
                mNormalSpeed = 5.0f;
                gabi::call(LK_setSingleMoveAnime, this, 1 /* ANM_WALK */, -1.2f, 0.0f, -1, 5.0f);
                return 1;
            }
        }
    }
    return 0;
}
VERIFY(0x023F49A4, &daPy_lk_c::setTalkStartBack);

/* 023F4B24 */
BOOL daPy_lk_c::dProcTalk_init() {
    WWHD_FUNC(0x023F4B24, BOOL, this);
    if (mCurProc == 0xAA /* daPyProc_DEMO_TALK_e */) {
        return TRUE;
    }
    mProcVar6 = 0;
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 1);
    gabi::store<u8>(dComIfGp_ea() + 0x5BD1, 0); /* HD */
    gabi::call(LK_commonProcInit, this, 0xAA);
    if (use == 0) {
        if (LK_demoMode != 8 && mEquipItem != 0x100) {
            gabi::call(LK_setAnimeUnequip, this);
        }
        mProcVar6 = setTalkStartBack();
    }
    if (mProcVar6 == 0) {
        mNormalSpeed = 0.0f;
        if (LK_demoMode == 8) {
            gabi::call(LK_setSingleMoveAnime, this, 0 /* ANM_WAITS */, 1.1f, 0.0f, -1, 5.0f);
        } else {
            gabi::call(LK_setSingleMoveAnime, this, 0x1B /* ANM_TALKA */, 0.7f, 0.0f, -1, 5.0f);
        }
        mProcVar7 = LK_FIELD(s32, 0x42C); /* mDemo.getParam1() */
        if (mProcVar7 == 1) {
            gabi::call(LK_setTextureAnime, this, 0x91, 0);
            mModeFlg = mModeFlg & ~0x80u;
        }
    }
    dComIfGp_onPlayerStatus0_l(0x10 /* daPyStts0_UNK10_e */);
    if (demoParam0() != 1) {
        gabi::call(0x025E1988 /* seStartSystem */, 0x80A /* JA_SE_TALK_START */);
    }
    gabi::call(LK_initShipRideUseItem, this, use, 0);
    return TRUE;
}
VERIFY(0x023F4B24, &daPy_lk_c::dProcTalk_init);

/* 023F4CB0 HD-only (no GameCube source; name descriptive): the damage taken while the Magic Armor is on
 * costs rupees in HD (GameCube: magic). Plays the shield sound, drops the lost rupees around Link
 * (largest denominations first, at most 200 rupees' worth) and subtracts the whole amount. */
void daPy_lk_c::setDamageRupee(f32 amount) {
    WWHD_FUNC(0x023F4CB0, void, this, amount);
    seStartMapInfo(0x2886 /* JA_SE_LK_MGC_SHIELD_DEF */);
    s32 lost = -(gabi::ftoi(amount) * 20);
    s32 left = lost;
    if (lost > 200) {
        left = 200;
    } else if (left == 0) {
        goto done;
    }
    {
        gabi::Local<cXyz> pos;
        gabi::Local<cXyz> ofs;
        gabi::Local<cXyz> p;
        gabi::Local<cXyz> rot;
        do {
            int item;
            if (left >= 150) {
                left -= 100;
                item = 6; /* SILVER_RUPEE (100) */
            } else if (left >= 80) {
                left -= 50;
                item = 5; /* ORANGE_RUPEE (50) */
            } else if (left >= 30) {
                left -= 20;
                item = 4; /* PURPLE_RUPEE (20) */
            } else if (left >= 20) {
                left -= 10;
                item = 3; /* RED_RUPEE (10) */
            } else if (left >= 10) {
                left -= 5;
                item = 2; /* BLUE_RUPEE (5) */
            } else {
                left -= 1;
                item = 1; /* GREEN_RUPEE (1) */
            }
            s16 a = (s16)gabi::ftoi(cM_rndFX(32000.0f));
            mDoMtx_YrotS(gabi::at<Mtx34>(gabi::load<u32>(0x1018C7B0) /* mDoMtx_stack_c::now */), a);
            ofs->x = 0.0f;
            ofs->y = 0.0f;
            ofs->z = 40.0f;
            MtxPosition(ofs, rot);
            cXyz_pl_l(gabi::ea(&current.pos), p, gabi::ea(rot.get()));
            pos->z = p->z;
            f32 py = p->y;
            pos->y = py;
            pos->x = p->x;
            f32 r = cM_rndF(60.0f);
            pos->y = py + (r + 40.0f);
            gabi::call(0x025D8870 /* fopAcM_createItem */, pos.get(), item, -1, -1, -1, 0, 0xF, 0);
        } while (left != 0);
    }
done:
    u32 a = dComIfGp_ea() + 0x5B48; /* the pending rupee change */
    gabi::store<s32>(a, gabi::load<s32>(a) - lost);
}
VERIFY(0x023F4CB0, &daPy_lk_c::setDamageRupee);

/* 023F51D0 */
BOOL daPy_lk_c::setDamagePoint(f32 amount) {
    WWHD_FUNC(0x023F51D0, BOOL, this, amount);
    /* HD: hero mode doubles the damage */
    u32 mode = gabi::call<u32>(0x027200D0, dComIfGs_base_l() + 0x12C0);
    if (gabi::call<BOOL>(0x0271FC5C, mode)) {
        amount = gabi::fadds_ppc(amount, amount);
    }
    if (!(noResetFlg1() & 1) && mTinkleShieldTimer == 0 /* !checkNoDamageMode() */) {
        u32 life = dComIfGp_ea() + 0x5B44; /* dComIfGp_setItemLifeCount(amount) */
        gabi::store<f32>(life, gabi::fadds_ppc(gabi::load<f32>(life), amount));
        if (amount < 0.0f) {
            setNoResetFlg1(noResetFlg1() & ~0x8000u); /* offNoResetFlg1(daPyFlg1_SOUP_POWER_UP) */
            if (gabi::load<u8>(dComIfGs_base_l() + 0x2E) != 0x3E /* !checkFinalMasterSwordEquip() */) {
                setNoResetFlg1(noResetFlg1() & ~0x200000u);
            }
        }
        return TRUE;
    }
    setDamageRupee(amount); /* HD: GameCube seStartMapInfo(JA_SE_LK_MGC_SHIELD_DEF) */
    return FALSE;
}
VERIFY(0x023F51D0, &daPy_lk_c::setDamagePoint);

/* 023F52D0 */
cXyz* daPy_lk_c::getDamageVec(dCcD_GObjInf* hitObj) {
    WWHD_FUNC(0x023F52D0, cXyz*, this, hitObj);
    u32 obj = gabi::ea(hitObj);
    cXyz* vec = gabi::at<cXyz>(obj + 0xC0); /* GetTgRVecP() */
    gabi::Local<cXyz> xz; /* abs2XZ() */
    xz->x = vec->x;
    xz->y = 0.0f;
    xz->z = vec->z;
    f64 mag = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get());
    if (resetFlg0() & 0x80000 /* daPyRFlg0_UNK80000 */) {
        f32 sn = cM_ssin(shape_angle.y);
        f32 cs = cM_scos(shape_angle.y);
        vec->y = 0.0f;
        vec->x = -10.0f * sn;
        vec->z = -10.0f * cs;
    } else if (mag < (f64)0.1f) {
        gabi::Local<cXyz> d;
        gabi::Local<cXyz> v;
        u32 ac = gabi::call<u32>(0x02515BBC /* dCcD_GAtTgCoCommonBase::GetAc (tg) */, obj + 0x94);
        if (ac != 0 && (gabi::load<s16>(ac + 0xE) == 0x1A8 || (ac != 0 && gabi::load<s16>(ac + 0xE) == 0x87))) {
            /* HD: for these two actors the direction comes from the actor's position */
            cXyz_mi(&current.pos, d, gabi::at<cXyz>(ac + 0x314));
        } else {
            cXyz_mi(&current.pos, d, gabi::at<cXyz>(obj + 0xCC) /* GetTgHitPosP() */);
        }
        lk_copy12(gabi::ea(v.get()), d);
        f64 m2 = gabi::call<f64>(0x028E8DD0, v.get());
        if (m2 < (f64)0.1f) {
            f32 sn = cM_ssin(shape_angle.y);
            f32 cs = cM_scos(shape_angle.y);
            vec->y = 0.0f;
            vec->x = -10.0f * sn;
            vec->z = -10.0f * cs;
        } else {
            f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, m2);
            PSVECScale(v, v, (f32)(10.0 / len));
            vec->y = v->y;
            vec->x = v->x;
            vec->z = v->z;
        }
    }
    return vec;
}
VERIFY(0x023F52D0, &daPy_lk_c::getDamageVec);

/* 023F54E8 */
BOOL daPy_lk_c::procLargeDamage_init(int type, int large, s16 rootX, s16 rootZ) {
    WWHD_FUNC(0x023F54E8, BOOL, this, type, large, rootX, rootZ);
    if (mCurProc == 0x68 /* daPyProc_LARGE_DAMAGE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x68);
    mDamageWaitTimer = LK_demoMode == 9 /* DEMO_LDAM_e */ ? 0 : 0x1E;
    int dir;
    if (type == -4) {
        mProcVar0 = 5;
        type = -1;
    } else {
        mProcVar0 = 0;
    }
    if (type == -5) {
        if (lk_isStartStage(0x10035830 /* "kinBOSS" */) || lk_isStartStage(0x10035838 /* "Xboss1" */)) {
            u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 1 /* CL_JNT_CENTER_e */);
            gabi::call(0x023D457C /* daPy_mtxFollowEcallBack_c::makeEmitter */, &m32E4, 0x80F6 /* ID_AK_SN_BKMHAKIDASHIHOUSHI00 */,
                       mtx, &current.pos, nullptr);
        }
        lk_startShockUp_l();
        dir = 0;
    } else if (type == -3) {
        dir = demoParam0();
        if (dir == 0) {
            current.angle.y = shape_angle.y;
        } else if (dir == 2) {
            current.angle.y = (s16)(shape_angle.y + 0x4000);
        } else if (dir == 3) {
            current.angle.y = (s16)(shape_angle.y - 0x4000);
        } else {
            current.angle.y = (s16)(shape_angle.y + 0x8000);
        }
    } else if (type == -2 || type == -9) {
        s16 d;
        if (type == -2) {
            current.angle.y = m3550;
            setDamagePoint(-1.0f);
            d = (s16)(current.angle.y - shape_angle.y);
        } else {
            cXyz* v = getDamageVec(&mCyl);
            s16 a = cM_atan2s(v->x, v->z);
            d = (s16)(a - shape_angle.y);
            current.angle.y = a;
        }
        if (std::fabs((f32)d) < 8192.0f) {
            d = d < 0 ? -0x2000 : 0x2000;
            current.angle.y = (s16)(d + shape_angle.y);
        }
        dir = getDirectionFromAngle(d);
        seStartOnlyReverb(0x282E /* JA_SE_LK_DAMAGE_LARGE */);
        lk_startShockUp_l();
    } else if (type == -1 || type == -6) {
        cXyz* v = getDamageVec(&mCyl);
        s16 a = cM_atan2s(v->x, v->z);
        current.angle.y = a;
        dir = getDirectionFromAngle((s16)(a - shape_angle.y));
        seStartOnlyReverb(0x282E);
        lk_startShockUp_l();
    } else if (type == -10) {
        dir = getDirectionFromAngle((s16)(current.angle.y - shape_angle.y));
    } else if (type == -7) {
        dir = 0;
        u16 sy = (u16)shape_angle.y;
        current.angle.y = (s16)sy;
        current.pos.x = gabi::fnmsubs(888.8888549804688f, cM_ssin(sy), current.pos.x);
        current.pos.z = gabi::fnmsubs(888.8888549804688f, cM_scos(sy), current.pos.z);
        mDamageWaitTimer = 0;
        mAcch.m_flags = mAcch.m_flags | 0x4004;
    } else {
        if (type == 0x60) {
            dir = 1;
        } else if (type == 0x5D) {
            dir = 2;
        } else if (type == 0x5E) {
            dir = 3;
        } else {
            dir = 0;
        }
        gabi::call(LK_setOldRootQuaternion, this, (s32)rootX, 0, (s32)rootZ);
    }
    if (dir == 0) {
        mProcVar6 = 0x5C; /* ANM_DAMFB */
        mProcVar2 = 0x3FFF;
        mProcVar3 = 4;
        shape_angle.y = current.angle.y;
    } else if (dir == 3) {
        mProcVar6 = 0x59; /* ANM_DAMFL */
        mProcVar2 = 0x3FFF;
        mProcVar3 = 0;
        shape_angle.y = (s16)(current.angle.y + 0x4000);
    } else if (dir == 2) {
        mProcVar6 = 0x5A; /* ANM_DAMFR */
        mProcVar2 = -0x3FFF;
        mProcVar3 = 0;
        shape_angle.y = (s16)(current.angle.y - 0x4000);
    } else {
        mProcVar6 = 0x5B; /* ANM_DAMFF */
        mProcVar2 = -0x3FFF;
        mProcVar3 = 4;
        shape_angle.y = (s16)(current.angle.y + 0x8000);
    }
    if (large != 0) {
        mMaxNormalSpeed = 25.0f;
        gravity = -13.0f;
        mProcVar3 = mProcVar3 | 8;
        mProcVar4 = 0x514;
        m35A0 = 16.0f;
    } else {
        mMaxNormalSpeed = 80.0f;
        gravity = -9.0f;
        m35A0 = 16.0f;
        mProcVar4 = 0x7D0;
    }
    gabi::call(LK_setSingleMoveAnime, this, (s32)mProcVar6, 1.0f, 0.0f, -1, 0.0f);
    if (type < 0) {
        if (type != -7) {
            LK_voiceStart(3);
        }
        if (type != -5) {
            if (mProcVar0 > 0) {
                mNormalSpeed = 0.0f;
                speed.y = 0.0f;
            } else if (large != 0) {
                mNormalSpeed = 25.0f;
                speed.y = 60.0f;
            } else {
                mNormalSpeed = 80.0f;
                speed.y = 50.0f;
            }
        }
    } else {
        mNormalSpeed = 16.0f;
        u16 ay = (u16)current.angle.y;
        current.pos.x = gabi::fmadds(35.0f, cM_ssin(ay), current.pos.x);
        current.pos.z = gabi::fmadds(35.0f, cM_scos(ay), current.pos.z);
        speed.y = 0.0f;
    }
    mNoResetFlg0 = mNoResetFlg0 & ~2u; /* offNoResetFlg0(daPyFlg0_UNK2) */
    gabi::store<s16>(gabi::ea(this) + 0x3D0, 0); /* mBodyAngle */
    gabi::store<s16>(gabi::ea(this) + 0x3D2, 0);
    gabi::store<s16>(gabi::ea(this) + 0x3D4, 0);
    if (type == -7) {
        mProcVar3 = mProcVar3 | 2;
        mProcVar6 = -4;
    }
    return TRUE;
}
VERIFY(0x023F54E8, &daPy_lk_c::procLargeDamage_init);

/* 023F5EAC */
void daPy_lk_c::dProcFreezeDamage_init_sub(int event) {
    WWHD_FUNC(0x023F5EAC, void, this, event);
    if (event != 0) {
        mProcVar6 = 1;
        LK_demoType = 5;
        gabi::call(0x023D4688 /* daPy_py_c::changePlayer */, this, this);
    }
    gabi::call(0x023DCA08 /* endDamageEmitter */, this);
    setNoResetFlg1((noResetFlg1() | 0x800 /* daPyFlg1_FREEZE_STATE */) & ~0x40000u /* daPyFlg1_UNK40000 */);
    gabi::call(0x025F0658 /* mDoGph_gInf_c::fadeOut */, 0x101CEAFC /* l_freeze_fade_color */, 1.0f);
    seStartMapInfo(0x2878 /* JA_SE_LK_FREEZE */);
    gabi::Local<cXyz> pos;
    for (int i = 0; i < 5; i++) {
        u16 jnt = gabi::load<u16>(0x10035840 + i * 2); /* eff_joint */
        u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), jnt);
        pos->x = gabi::load<f32>(mtx + 0xC); /* mDoMtx_multVecZero */
        pos->y = gabi::load<f32>(mtx + 0x1C);
        pos->z = gabi::load<f32>(mtx + 0x2C);
        u32 pa = gabi::load<u32>(dComIfGp_ea() + 0x5AB0); /* dComIfGp_particle_setP1 */
        gabi::call(0x025A847C, pa, 1, 0x827E /* ID_IT_SN_LK_FREEZSMOKE00 */, pos.get(), 0, 0, 0xFF, 0, -1, 0, 0, 0);
        if (i >= 3) {
            pa = gabi::load<u32>(dComIfGp_ea() + 0x5AB0);
            gabi::call(0x025A847C, pa, 1, 0x827F /* ID_IT_SN_LK_FREEZKIRA00 */, pos.get(), 0, 0, 0xFF, 0, -1, 0, 0, 0);
        }
    }
}
VERIFY(0x023F5EAC, &daPy_lk_c::dProcFreezeDamage_init_sub);

/* 023F6020 */
BOOL daPy_lk_c::procLargeDamageUp_init(int type, int large, s16 rootX, s16 rootZ) {
    WWHD_FUNC(0x023F6020, BOOL, this, type, large, rootX, rootZ);
    if (mCurProc == 0x69 /* daPyProc_LARGE_DAMAGE_UP_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x69);
    int anm;
    f32 rate, start, morf;
    s32 stop;
    if (type == -3) {
        mProcVar0 = 0;
        m35A0 = 36.0f;
        anm = 0x5F; /* ANM_DAMFFUP */
        start = 0.0f;
        rate = 0.5f;
        morf = 5.0f;
        stop = -1;
    } else if (type == -1 || type == -2) {
        anm = 0x60; /* ANM_DAMFBUP */
        start = 7.0f;
        morf = 2.0f;
        stop = -1;
        m35A0 = 36.0f;
        if (type == -1) {
            mProcVar0 = 0x1E;
            rate = 0.0f;
        } else {
            mProcVar0 = 0;
            rate = 0.5f;
        }
    } else if (type == -4 || gabi::call<BOOL>(0x025445B8 /* dEvent_manager_c::startCheckOld */, dComIfGp_ea() + 0x52C4,
                                              0x10035850 /* "ICE_FAILED" */)) {
        anm = 0x60;
        rate = 0.0f;
        start = 7.0f;
        stop = 0x24;
        m35A0 = 34.0f;
        morf = 2.0f;
        if (type == -4) {
            mProcVar0 = 0x28;
        } else {
            mProcVar0 = 30000;
            dProcFreezeDamage_init_sub(0);
        }
        mFootEffectPosType = 6;
        lk_startShockUp_l();
        seStartMapInfo(0x3811 /* JA_SE_LK_FALL_DOWN */);
    } else {
        mProcVar0 = 0;
        mFootEffectPosType = 6;
        lk_startShockUp_l();
        seStartMapInfo(0x3811);
        morf = 2.0f;
        stop = 0x24;
        if (type == 0x5C /* ANM_DAMFB */) {
            anm = 0x60;
            start = 0.0f;
            if (large != 0) {
                m35A0 = 30.0f;
                rate = 0.8f;
            } else {
                m35A0 = 34.0f;
                rate = 0.6f;
            }
        } else if (type == 0x59 /* ANM_DAMFL */ || type == 0x5A /* ANM_DAMFR */) {
            anm = type == 0x59 ? 0x5D /* ANM_DAMFLUP */ : 0x5E /* ANM_DAMFRUP */;
            start = 1.0f;
            if (large == 0) {
                m35A0 = 34.0f;
                rate = 0.6f;
            } else {
                m35A0 = 30.0f;
                rate = 0.7f;
            }
        } else {
            anm = 0x5F;
            start = 0.0f;
            if (large == 0) {
                m35A0 = 34.0f;
                rate = 0.6f;
            } else {
                m35A0 = 30.0f;
                rate = 0.7f;
            }
        }
    }
    m35E4 = 1.0f;
    if (gabi::load<u16>(dComIfGs_base_l() + 0x22) /* dComIfGs_getLife() */ == 0) {
        stop = -1;
    }
    m35A4 = 2.0f / (m35A0 - start);
    gabi::call(LK_setSingleMoveAnime, this, anm, rate, start, stop, morf);
    gabi::call(LK_setOldRootQuaternion, this, (s32)rootX, 0, (s32)rootZ);
    mProcVar6 = type;
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x023F6020, &daPy_lk_c::procLargeDamageUp_init);

/* 023F6564 */
BOOL daPy_lk_c::procFall_init(int type, f32 morf) {
    WWHD_FUNC(0x023F6564, BOOL, this, type, morf);
    if (LK_demoMode == 0x11 /* DEMO_PFALL_e */) {
        mAcch.m_flags = mAcch.m_flags | 0x4002; /* SetGrndNone(); OnLineCheckNone() */
    }
    s32 proc = mCurProc;
    BOOL notShipOff = proc != 0x90 /* daPyProc_SHIP_GET_OFF_e */;
    if (proc == 0x27 /* daPyProc_FALL_e */) {
        return FALSE;
    }
    BOOL slide = proc == 0x1A /* SLIDE_FRONT */ || proc == 0x1B /* SLIDE_BACK */;
    gabi::call(LK_commonProcInit, this, 0x27);
    if (type == 1) {
        mNormalSpeed = 0.0f;
        current.angle.y = shape_angle.y;
        speed.y = 0.0f;
    }
    mProcVar3 = type == 2;
    gabi::call(LK_setSingleMoveAnime, this, 0xD /* ANM_JMPEDS */, 0.0f, 0.0f, -1, morf);
    gabi::call(LK_setTextureAnime, this, 0x37, 0);
    resetSeAnime();
    mProcVar6 = 0;
    if (current.angle.y == shape_angle.y && LK_upperIdx() != 0x95 && LK_upperIdx() != 0x96 /* !checkGrabAnime() */) {
        if (mNormalSpeed > 1.0f) {
            mProcVar0 = 2;
        } else {
            mProcVar0 = 1;
        }
    } else {
        mProcVar0 = 0;
    }
    mProcVar1 = 8;
    if (cLib_distanceAngleS(current.angle.y, shape_angle.y) < 0x4800 && !slide) {
        mProcVar2 = 1;
    } else {
        mProcVar2 = 0;
    }
    mProcVar7 = notShipOff;
    mNoResetFlg0 = mNoResetFlg0 & ~0x40000u; /* offNoResetFlg0(daPyFlg0_NO_FALL_VOICE) */
    return TRUE;
}
VERIFY(0x023F6564, &daPy_lk_c::procFall_init);

/* 023F6794 */
BOOL daPy_lk_c::procSlowFall_init() {
    WWHD_FUNC(0x023F6794, BOOL, this);
    if (mCurProc == 0x28 /* daPyProc_SLOW_FALL_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0x28);
    maxFallSpeed = -20.0f;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xD3 /* ANM_MSTEPOVER_JMPED */, 0.0f, 0.0f, -1, 2.4f);
    f32 f = (f32)mFrameCtrlUnder[0].mEnd - 0.001f;
    mFrameCtrlUnder[0].mFrame = f;
    gabi::store<f32>(LK_FIELD(u32, 0x57FC) /* mAnmRatioUnder[0].getAnmTransform() */, f); /* ->setFrame() */
    return TRUE;
}
VERIFY(0x023F6794, &daPy_lk_c::procSlowFall_init);

/* 023F684C */
BOOL daPy_lk_c::dProcLookWait_init() {
    WWHD_FUNC(0x023F684C, BOOL, this);
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    if (mCurProc == 0xBE /* daPyProc_DEMO_LOOK_WAIT_e */) {
        return FALSE;
    }
    if (mCurProc == 0x73 /* daPyProc_GRAB_WAIT_e */) {
        lk_pbSetRatio(m_pbCalc[1], 2, 1.0f);
    }
    gabi::call(LK_commonProcInit, this, 0xBE);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setBlendMoveAnime, this, 2.4f);
    mDirection = 4; /* DIR_NONE */
    current.angle.y = shape_angle.y;
    gabi::call(LK_initShipRideUseItem, this, use, 0);
    return TRUE;
}
VERIFY(0x023F684C, &daPy_lk_c::dProcLookWait_init);

/* GHS pointer-to-member call returning r3 (see ptmf_call in bindings.h) */
static inline BOOL lk_ptmf_callB(u32 entry, void* self) {
    s16 idx = gabi::load<s16>(entry + 2);
    s16 delta = gabi::load<s16>(entry);
    u32 p = gabi::ea(self) + delta;
    if (idx < 0) {
        return gabi::call_ptr<BOOL>(gabi::load<u32>(entry + 4), p);
    }
    u32 vt = gabi::load<u32>(p + gabi::load<s16>(entry + 6));
    return gabi::call_ptr<BOOL>(gabi::load<u32>(vt + idx * 8 + 4), p);
}
#define LK_DEMO_PROC_INIT_TABLE 0x10037858u /* mDemoProcInitFuncTable (8-byte GHS pointers to members) */
#define LK_onDemoFlg() (mNoResetFlg0 = mNoResetFlg0 | 0x100000 /* onNoResetFlg0(daPyFlg0_UNK100000) */)

/* 023F695C */
BOOL daPy_lk_c::changeDemoProc() {
    WWHD_FUNC(0x023F695C, BOOL, this);
    u32 mode = (u32)LK_demoMode;
    if (!dComIfGp_event_runCheck_l()) {
        return FALSE;
    }
    u16 eq = mEquipItem;
    if (noResetFlg1() & 0x1000 /* daPyFlg1_SHIP_TACT */) {
        if (eq != 0x10A /* daPyItem_UNK10A_e */) {
            gabi::call(LK_deleteEquipItem, this, 0);
            gabi::call(0x023DEF04 /* setTactModel */, this);
            u32 model = gabi::load<u32>(mpEquipItemModel_ea);
            mEquipItem = 0x10A;
            gabi::store<f32>(model + 0xBC, 2.5f); /* setBaseScale(tact_scale) */
            gabi::store<f32>(model + 0xC4, 2.5f);
            gabi::store<f32>(model + 0xC0, 2.5f);
        }
    } else if (eq == 0x10A) {
        gabi::call(LK_deleteEquipItem, this, 0);
    }
    if (mode >= 0x4B /* DEMO_LAST_e */) {
        if (mode == 0x200 /* DEMO_NEW_ANM0_e */) {
            goto tool;
        }
        gabi::call(0x0273AA24 /* JUT_ASSERT failure */, 0x10035860, 0x2B35, 0x10035874);
    }
    if (mode == 0x34) {
        /* HD: the message-bottle demo: start the throw or the letter proc, or cancel it in midair */
        if (((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !(mNoResetFlg0 & 0x20000000) && !LK_checkPlayerFly()) ||
            dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
            u32 proc = (u32)mCurProc;
            if (proc >= 0xDB && proc <= 0xDD) {
                return FALSE;
            }
            if (mHD8265 != 0) {
                return dProcHDBottleThrow_init();
            }
            return dProcHDLetterWrite_init();
        }
        u32 body = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
        gabi::call(0x0253E860 /* dCamera_c::EndEventCamera */, body, gabi::load<u32>(gabi::ea(this) + 4));
        if (mHDThrowActor != 0) {
            u32 id = mHDThrowActor != 0 ? gabi::load<u32>(mHDThrowActor + 4) : 0xFFFFFFFF;
            body = gabi::call<u32>(0x024F8044);
            gabi::call(0x0253E860, body, id);
            id = mHDThrowActor != 0 ? gabi::load<u32>(mHDThrowActor + 4) : 0xFFFFFFFF;
            body = gabi::call<u32>(0x024F8044);
            gabi::call(0x025052BC /* dCamera_c::ForceLockOff */, body, id);
            body = gabi::call<u32>(0x024F8044);
            gabi::call(0x02515048 /* dCamera_c::Reset */, body);
            body = gabi::call<u32>(0x024F8044);
            gabi::call(0x02514F38 /* dCamera_c::Start */, body);
            gabi::store<u8>(mHDThrowActor + 0x787, 3);
            mHDThrowActor = 0;
        } else if (mHD8265 != 0) {
            u32 item = gabi::call<u32>(0x025D7F20 /* fopAcM_fastCreateItem2 */, &current.pos, 0x13, -1, -1, 0, 0, 0, 0);
            gabi::store<u8>(item + 0x787, 3);
            mHDThrowActor = 0;
        }
        mHD8265 = 0;
        u32 ev = dComIfGp_ea() + 0x52B8; /* the event flags */
        gabi::store<u16>(ev, gabi::load<u16>(ev) | 8);
        endDemoMode();
        return LK_callB(LK_changeWaitProc);
    }
    if (mode == 0x200) {
        goto tool;
    }
    /* HD: the modes that skip the ground check go straight to their handlers */
    if (mode == 0x1E || mode == 0x2F || mode == 0x3E) {
        LK_onDemoFlg();
        return lk_ptmf_callB(LK_DEMO_PROC_INIT_TABLE + mode * 8, this);
    }
    if (mode == 9) {
        goto ldam;
    }
    if (mode == 0x11) {
        goto pfall;
    }
    if (mode == 0x40) {
        goto sfall;
    }
    if (mode == 4) {
        goto initWait;
    }
    if (!dComIfGp_checkPlayerStatus0_l(0x10000) && !dComIfGp_checkPlayerStatus0_l(0x100000 /* daPyStts0_SWIM_e */)) {
        if (!(mAcch.m_flags & dBgS_Acch::GROUND_HIT) || LK_checkPlayerFly()) {
            return FALSE;
        }
    }
    if (mode == 0x200) {
    tool:
        LK_onDemoFlg();
        return dProcTool_init();
    }
    if (gabi::load<s16>(LK_DEMO_PROC_INIT_TABLE + mode * 8 + 2) != 0) {
        LK_onDemoFlg();
        return lk_ptmf_callB(LK_DEMO_PROC_INIT_TABLE + mode * 8, this);
    }
    if (mode == 4 /* DEMO_INIT_WAIT_e */) {
    initWait:
        if (dComIfGp_checkPlayerStatus0_l(0x10000)) {
            return TRUE;
        }
        BOOL r = LK_callB(LK_procWait_init);
        if (r) {
            gabi::call(0x025E3F3C /* mDoExt_MtxCalcOldFrame::initOldFrameMorf */, (u32)m_old_fdata, 0.0f, 0, 0x2A);
        }
        return r;
    }
    if (mode == 6 /* DEMO_N_TALK_e */ || mode == 8 /* DEMO_E_TALK_e */) {
        if (mModeFlg & 0x04000000) {
            return TRUE;
        }
        if (dComIfGp_checkPlayerStatus0_l(0x10000) && gabi::call<u32>(0x025D7C6C /* fopAcM_getTalkEventPartner */, this) != 0) {
            u32 ship = dComIfGp_getShipActor_l();
            if (gabi::call<u32>(0x025D7C6C, this) == ship) {
                return procShipPaddle_init();
            }
        }
        LK_onDemoFlg();
        u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
        if (grab != 0) {
            if ((gabi::load<u32>(grab + 0x2E0) & 0x10000 /* fopAcStts_UNK10000_e */) && !LK_callB(0x023DBBE8 /* checkGrabSpecialHeavyState */)) {
                return gabi::call<BOOL>(0x023E9E48 /* procGrabThrow_init */, this, 0);
            }
            return LK_callB(0x023EA054 /* procGrabPut_init */);
        }
        return dProcTalk_init();
    }
    if (mode == 9 /* DEMO_LDAM_e */) {
    ldam:
        LK_onDemoFlg();
        if (mCurProc == 0x69 /* LARGE_DAMAGE_UP */ || mCurProc == 0x6A /* LARGE_DAMAGE_WALL */) {
            return FALSE;
        }
        if (mModeFlg & 0x40000 /* ModeFlg_SWIM */) {
            dComIfGp_evmng_cutEnd(mStaffIdx);
            return FALSE;
        }
        return procLargeDamage_init(-3, 1, 0, 0);
    }
    if (mode == 0x1B /* DEMO_UNK_027_e */) {
        LK_onDemoFlg();
        return procLargeDamageUp_init(-3, 1, 0, 0);
    }
    if (mode == 0x10 /* DEMO_BJUMP_e */) {
        LK_onDemoFlg();
        if (mCurProc == 0x23 /* daPyProc_BACK_JUMP_LAND_e */) {
            return FALSE;
        }
        return procBackJump_init();
    }
    if (mode == 0x11 /* DEMO_PFALL_e */) {
    pfall:
        LK_onDemoFlg();
        return procFall_init(1, 6.0f);
    }
    if (mode == 0x40 /* DEMO_SFALL_e */) {
    sfall:
        if (mCurProc == 0x25 /* daPyProc_LAND_e */) {
            return TRUE;
        }
        return procSlowFall_init();
    }
    if (mode == 0x1F /* DEMO_LWAIT_e */) {
        BOOL look;
        if (!(mModeFlg & 0x14452822)) {
            if (LK_callB(LK_checkUpperReadyThrowAnime)) {
                look = TRUE;
            } else {
                u8 st = m34C3;
                look = (st >= 1 && st <= 2) || st == 4 || (st >= 9 && st <= 10) || mCurProc == 0x73 /* daPyProc_GRAB_WAIT_e */;
            }
        } else {
            look = FALSE;
        }
        if (look || dComIfGp_checkPlayerStatus0_l(0x10000)) {
            LK_onDemoFlg();
            return dProcLookWait_init();
        }
        dComIfGp_evmng_cutEnd(mStaffIdx);
        return TRUE;
    }
    if (mode == 0x22 /* DEMO_TACT_e */) {
        LK_onDemoFlg();
        if (mCurProc == 0x9B /* TACT_PLAY */ || mCurProc == 0x9C /* TACT_PLAY_END */) {
            return TRUE;
        }
        return gabi::call<BOOL>(LK_procTactWait_init, this, demoParam0());
    }
    if (mode == 0x33 /* DEMO_UNK_051_e */) {
        bool change = mActorKeepGrab.mActor.get() == nullptr;
        if (!change) {
            u32 play = dComIfGp_ea();
            change = gabi::ea(mActorKeepGrab.mActor.get()) != gabi::load<u32>(play + 0x5B38) /* dComIfGp_getCb1Player() */;
        }
        if (change) {
            gabi::call(LK_deleteEquipItem, this, 0);
            gabi::call(0x023DCF8C /* freeGrabItem */, this);
            u32 cb1 = gabi::load<u32>(dComIfGp_ea() + 0x5B38);
            gabi::call(0x023DE638 /* daPy_actorKeep_c::setData */, &mActorKeepGrab, cb1);
            gabi::call(0x025D9D0C /* fopAcM_setCarryNow */, mActorKeepGrab.mActor.get(), 1);
        }
        return LK_callB(0x023E2484 /* procGrabWait_init */);
    }
    u32 flg0 = mNoResetFlg0;
    if (mode == 0x3D /* DEMO_BO_OPEN_e */) {
        mNoResetFlg0 = flg0 | 0x100000;
        return procBottleOpen_init(dComIfGp_event_getPreItemNo_l());
    }
    if (!(flg0 & 0x100000) && mCurProc != 1 /* daPyProc_SUBJECTIVITY_e */ && !LK_callB(LK_checkUpperReadyThrowAnime)) {
        return FALSE;
    }
    if (!((mode >= 1 && mode <= 3) || mode == 0x12 || mode == 0x17 || mode == 0x2A)) {
        return FALSE;
    }
    mNoResetFlg0 = mNoResetFlg0 & ~0x100000u;
    if (LK_callB(LK_checkUpperReadyThrowAnime)) {
        gabi::call(LK_resetActAnimeUpper, this, 2, -1.0f);
    }
    if (dComIfGp_checkPlayerStatus0_l(0x10000)) {
        return procShipPaddle_init();
    }
    return checkNextMode(0);
}
VERIFY(0x023F695C, &daPy_lk_c::changeDemoProc);

/* 023F73B8 */
fopAc_ac_c* daPy_lk_c::makeFairy(cXyz* pos, u32 prm) {
    WWHD_FUNC(0x023F73B8, fopAc_ac_c*, this, pos, prm);
    setResetFlg0(resetFlg0() | 0x02000000 /* daPyRFlg0_FAIRY_USE */);
    resetCurse();
    return gabi::call<fopAc_ac_c*>(0x025D5928 /* fopAcM_fastCreate */, 0x168 /* fpcNm_NPC_FA1_e */, prm, pos,
                                   (s32)current.roomNo, &shape_angle, nullptr, -1, 0, 0);
}
VERIFY(0x023F73B8, &daPy_lk_c::makeFairy);

/* 023F7434 */
void daPy_lk_c::dProcDead_init_sub() {
    WWHD_FUNC(0x023F7434, void, this);
    mProcVar6 = 0;
    dComIfGp_onPlayerStatus0_l(0x20000000 /* daPyStts0_UNK20000000_e */);
    mGameOverId = gabi::call<u32>(0x025DB374 /* d_GameOver_Create: createAppend */, 0x1E3, 0, 0, 0, 0, 0);
    LK_demoType = 5;
    gabi::call(0x023D4688 /* daPy_py_c::changePlayer */, this, this);
}
VERIFY(0x023F7434, &daPy_lk_c::dProcDead_init_sub);

/* 023F74B0 */
void daPy_lk_c::dProcDead_init_sub2() {
    WWHD_FUNC(0x023F74B0, void, this);
    mFrameCtrlUnder[0].mRate = 1.0f;
    LK_voiceStart(0x16);
    gabi::call(0x025E1904 /* mDoAud_bgmStop */, 0);
    gabi::call(0x025E18EC /* mDoAud_bgmStart */, 0x8000000A /* JA_BGM_DIE_LINK */);
    u32 body = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
    gabi::call(0x0253E70C /* dCamera_c::StartEventCamera */, body, 9, gabi::load<u32>(gabi::ea(this) + 4), 0);
    if (dComIfGp_checkPlayerStatus0_l(0x100000 /* daPyStts0_SWIM_e */)) {
        u32 pa = gabi::load<u32>(dComIfGp_ea() + 0x5AB0); /* dComIfGp_particle_setShipTail */
        gabi::call(0x025A847C, pa, 5, 0x3F /* ID_IT_JN_WP_HAMON03 */, &current.pos, 0, 0x100358CC /* ripple_scale */, 0xFF,
                   0x1047B2E4 /* dPa_control_c::mSingleRippleEcallBack */, -1, 0, 0, 0);
        mProcVar3 = 1;
    }
}
VERIFY(0x023F74B0, &daPy_lk_c::dProcDead_init_sub2);

/* 023F7580 */
BOOL daPy_lk_c::dProcDead_init() {
    WWHD_FUNC(0x023F7580, BOOL, this);
    if (mCurProc == 0xB2 /* daPyProc_DEMO_DEAD_e */) {
        return TRUE;
    }
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    u32 swim = mModeFlg & 0x40000 /* ModeFlg_SWIM */;
    u32 flg100 = mNoResetFlg0 & 0x100 /* daPyFlg0_UNK100 */;
    if (swim) {
        gabi::call(0x023DFAAC /* swimOutAfter */, this, 0);
    }
    gabi::call(LK_commonProcInit, this, 0xB2);
    f32 rate;
    if (dComIfGp_event_compulsory_l(this)) {
        dProcDead_init_sub();
        mNormalSpeed = 0.0f;
        rate = 1.0f;
        mProcVar2 = 0;
    } else {
        mProcVar2 = 0;
        mNormalSpeed = 0.0f;
        rate = 0.0f;
        mProcVar6 = 1;
    }
    int anm;
    if (use != 0) {
        anm = 0x63; /* ANM_SHIPDIE */
    } else if (swim) {
        anm = 0x64; /* ANM_SWIMDIE */
        mModeFlg = mModeFlg | 0x40000;
        dComIfGp_onPlayerStatus0_l(0x100000 /* daPyStts0_SWIM_e */);
        if (flg100 == 0) {
            rate = 0.0f;
            mProcVar2 = 1;
            mNoResetFlg0 = mNoResetFlg0 & ~0x100u;
        } else {
            m34C2 = 0;
        }
    } else {
        anm = 0x62; /* ANM_DIELONG */
        mModeFlg = mModeFlg | 0x02000000;
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, rate, 0.0f, -1, 2.0f);
    m35E4 = 0.0f;
    mDamageWaitTimer = 0;
    m35A0 = 1.0f;
    if (mProcVar2 == 0) {
        speed.y = 0.0f;
    }
    gravity = 0.0f;
    mProcVar7 = 0;
    gabi::call(LK_initShipRideUseItem, this, use, 1);
    mProcVar3 = 0;
    if (rate > 0.0f) {
        dProcDead_init_sub2();
    }
    cancelNoDamageMode();
    return TRUE;
}
VERIFY(0x023F7580, &daPy_lk_c::dProcDead_init);

/* 023F7820 */
BOOL daPy_lk_c::changeDeadProc() {
    WWHD_FUNC(0x023F7820, BOOL, this);
    if (!dComIfGp_event_runCheck_l() && LK_demoType == 0 /* !checkPlayerDemoMode() */ &&
        gabi::load<u16>(dComIfGs_base_l() + 0x22) /* dComIfGs_getLife() */ == 0 && !(mModeFlg & 8 /* ModeFlg_DAMAGE */)) {
        if (gabi::call<BOOL>(0x025B5C80 /* dSv_player_item_c::checkBottle */, dComIfGs_base_l() + 0x5C, 0x57 /* FAIRY_BOTTLE */)) {
            makeFairy(&current.pos, 5 /* daNpc_Fa1_c::Type_LINK_DOWN_e */);
            gabi::call(0x025B51DC /* dSv_player_item_c::setBottleItemIn */, dComIfGs_base_l() + 0x5C, 0x57, 0x50 /* EMPTY_BOTTLE */);
            return FALSE;
        }
        if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) || dComIfGp_checkPlayerStatus0_l(0x10000) || (mModeFlg & 0x40000)) {
            return dProcDead_init();
        }
        return procFall_init(1, 6.0f);
    }
    return FALSE;
}
VERIFY(0x023F7820, &daPy_lk_c::changeDeadProc);

/* 023F7958 */
BOOL daPy_lk_c::procAutoJump_init() {
    WWHD_FUNC(0x023F7958, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x24 /* daPyProc_AUTO_JUMP_e */);
    f32 z = current.pos.z;
    f32 y = current.pos.y;
    m3688.z = z;
    m3688.y = y;
    m35F4 = y;
    m35F0 = y;
    m3688.x = current.pos.x;
    gabi::call(LK_setSingleMoveAnime, this, 0x3C /* ANM_JMPST */, 0.8f, 1.0f, 5, 1.0f);
    f32 sp = speedF;
    if (sp > 17.0f) {
        sp = 17.0f;
        speedF = sp;
    } else if (sp < 9.0f) {
        sp = 9.0f;
        speedF = sp;
    }
    mNormalSpeed = (sp * cM_scos(0x2AF8)) * 1.6f;
    speed.y = (speedF * cM_ssin(0x2AF8)) * 1.6f;
    mProcVar6 = 0;
    lk_copy12(gabi::ea(&m3700), gabi::at<cXyz>(0x101FFBA8) /* cXyz::Zero */);
    m34C2 = 1;
    current.angle.y = shape_angle.y;
    LK_voiceStart(6);
    mProcVar1 = 3;
    mProcVar0 = -1;
    mNoResetFlg0 = (mNoResetFlg0 | 0x20000 /* daPyFlg0_UNK20000 */) & ~0x40000u /* daPyFlg0_NO_FALL_VOICE */;
    return TRUE;
}
VERIFY(0x023F7958, &daPy_lk_c::procAutoJump_init);

/* 023F7AB0 */
BOOL daPy_lk_c::procClimbDownStart_init(s16 angle) {
    WWHD_FUNC(0x023F7AB0, BOOL, this, angle);
    gabi::call(LK_commonProcInit, this, 0x3E /* daPyProc_CLIMB_DOWN_START_e */);
    gravity = 0.0f;
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    speedF = 0.0f;
    gabi::call(LK_deleteEquipItem, this, 1);
    gabi::call(LK_setSingleMoveAnime, this, 0x87 /* ANM_LADDERDWST */, 1.3f, 30.0f, -1, 3.0f);
    dComIfGp_onPlayerStatus1_l(0x10000 /* daPyStts1_UNK10000_e */);
    gabi::call(LK_setOldRootQuaternion, this, 0, -0x8000, 0);
    s16 a = (s16)(angle + 0x8000);
    current.angle.y = a;
    shape_angle.y = a;
    current.pos.x = gabi::fnmsubs(35.0f, cM_ssin((u16)angle), current.pos.x);
    current.pos.y = current.pos.y - 20.0f;
    current.pos.z = gabi::fnmsubs(35.0f, cM_scos((u16)angle), current.pos.z);
    mProcVar6 = 1;
    m35E0 = 43.67353f;
    return TRUE;
}
VERIFY(0x023F7AB0, &daPy_lk_c::procClimbDownStart_init);

/* cBgS::GetTriPla(bg, poly) of mLinkLinChk's poly info (+0x14); the plane normal at +0 */
static inline u32 lk_linChkTriPla(u32 me) {
    u32 play = dComIfGp_ea();
    return gabi::call<u32>(0x020084C8, play + 0x12A0, (u32)gabi::load<u16>(me + 0x9E6), (u32)gabi::load<u16>(me + 0x9E4));
}

/* 023F7BD4 */
BOOL daPy_lk_c::procHangFallStart_init(void* pla /* cM3dGPla* */) {
    WWHD_FUNC(0x023F7BD4, BOOL, this, pla);
    u32 me = gabi::ea(this);
    u32 np = gabi::ea(pla);
    s16 a = cM_atan2s(gabi::load<f32>(np + 0), gabi::load<f32>(np + 8));
    f32 x = gabi::fnmsubs(1.5f, gabi::load<f32>(np + 0), current.pos.x);
    f32 y = current.pos.y;
    current.pos.x = x;
    f32 z = gabi::fnmsubs(1.5f, gabi::load<f32>(np + 8), current.pos.z);
    current.pos.z = z;
    u16 side = (u16)(a - 0x4000);
    f32 dx = 30.0f * cM_ssin(side);
    f32 dz = 30.0f * cM_scos(side);
    gabi::Local<cXyz> p0;
    gabi::Local<cXyz> p1;
    p0->x = gabi::fmadds(4.5f, cM_ssin((u16)a), x);
    p0->y = y - 62.5f;
    p0->z = gabi::fmadds(4.5f, cM_scos((u16)a), z);
    p1->x = p0->x - dx;
    p1->y = p0->y;
    p1->z = p0->z - dz;
    void* chk = mLinkLinChk;
    dBgS_LinChk_Set(chk, p0, p1, this);
    u32 tp;
    if (cBgS_LineCross(dComIfG_Bgsp(), chk) && (tp = lk_linChkTriPla(me)) != 0 &&
        gabi::call<s32>(0x0200FAAC /* cLib_distanceAngleS */, (s32)cM_atan2s(gabi::load<f32>(tp + 0), gabi::load<f32>(tp + 8)),
                        (s32)shape_angle.y) < 0x549F) {
        f32 cz = current.pos.z + dz;
        f32 nx = p0->x + dx;
        f32 cx = current.pos.x + dx;
        f32 nz = p0->z + dz;
        current.pos.z = cz;
        p1->y = p0->y;
        p1->x = nx + dx;
        current.pos.x = cx;
        p0->x = nx;
        p1->z = nz + dz;
        p0->z = nz;
    } else {
        p1->x = p0->x + dx;
        p1->y = p0->y;
        p1->z = p0->z + dz;
    }
    dBgS_LinChk_Set(chk, p0, p1, this);
    f32 gx, gz;
    if (cBgS_LineCross(dComIfG_Bgsp(), chk) && (tp = lk_linChkTriPla(me)) != 0 &&
        gabi::call<s32>(0x0200FAAC, (s32)cM_atan2s(gabi::load<f32>(tp + 0), gabi::load<f32>(tp + 8)), (s32)shape_angle.y) < 0x549F) {
        gx = current.pos.x - dx;
        gz = current.pos.z - dz;
        f32 gy = current.pos.y + 50.0f;
        current.pos.x = gx;
        mGndChkPos.x = gx;
        mGndChkPos.z = gz;
        current.pos.z = gz;
        mGndChkPos.y = gy;
    } else {
        gx = current.pos.x;
        f32 gy = current.pos.y + 50.0f;
        gz = current.pos.z;
        mGndChkPos.x = gx;
        mGndChkPos.z = gz;
        mGndChkPos.y = gy;
    }
    f64 gc = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
    f32 gy;
    if (gc == -1000000000.0) {
        gx = gx + dx;
        gy = current.pos.y + 50.0f;
        gz = gz + dz;
    } else {
        if (gc < (f64)(f32)(current.pos.y + -30.1f)) {
            return FALSE;
        }
        gx = gx + dx;
        current.pos.y = (f32)gc;
        gy = (f32)(gc + 50.0);
        gz = gz + dz;
    }
    mGndChkPos.x = gx;
    mGndChkPos.y = gy;
    mGndChkPos.z = gz;
    f64 gc2 = gabi::call<f64>(0x02008974, dComIfG_Bgsp(), mGndChk);
    if (gc2 < (f64)(f32)(current.pos.y - 50.0f)) {
        f32 x3 = current.pos.x - dx;
        f32 z3 = current.pos.z - dz;
        mGndChkPos.y = gy;
        mGndChkPos.x = x3;
        mGndChkPos.z = z3;
        f64 gc3 = gabi::call<f64>(0x02008974, dComIfG_Bgsp(), mGndChk);
        if (gc3 < (f64)(f32)(current.pos.y - 50.0f)) {
            return FALSE;
        }
        current.pos.y = (f32)gc3;
        current.pos.x = x3;
        current.pos.z = z3;
    }
    s16 na = (s16)(a + 0x8000);
    gabi::call(LK_setOldRootQuaternion, this, 0, (s32)(s16)(shape_angle.y - na), 0);
    s32 proc = mCurProc;
    current.angle.y = na;
    shape_angle.y = na;
    gabi::call(LK_commonProcInit, this, 0x2C /* daPyProc_HANG_FALL_START_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x50 /* ANM_HANGING */, 0.8f, 2.0f, 0x15, proc == 0x1E /* FRONT_ROLL */ ? 0.0f : 2.0f);
    mNormalSpeed = 0.0f;
    speed.y = 0.0f;
    dComIfGp_onPlayerStatus0_l(0x100 /* daPyStts0_HANG_e */);
    LK_voiceStart(0xB);
    return TRUE;
}
VERIFY(0x023F7BD4, &daPy_lk_c::procHangFallStart_init);

/* 023F81A4 */
BOOL daPy_lk_c::changeAutoJumpProc() {
    WWHD_FUNC(0x023F81A4, BOOL, this);
    if ((mModeFlg & 0x10452802) || (mAcch.m_flags & dBgS_Acch::GROUND_HIT)) {
        return FALSE;
    }
    u32 me = gabi::ea(this);
    f32 y = current.pos.y;
    f32 x = current.pos.x;
    f32 z = current.pos.z;
    f32 gndDiff = mAcch.m_ground_h - y;
    gabi::store<f32>(me + 0xCB0, x); /* mLavaGndChk.SetPos(&current.pos) */
    gabi::store<f32>(me + 0xCB4, y);
    gabi::store<f32>(me + 0xCB8, z);
    if (mNoResetFlg0 & 0x1000 /* daPyFlg0_HOVER_BOOTS */) {
        mNoResetFlg0 = mNoResetFlg0 & ~0x1000u;
        return procFall_init(2, 6.0f);
    }
    if (!(gndDiff < -30.1f)) {
        return FALSE;
    }
    f32 dz = ((speed.z + m3730.z) + m36A0.z) + m36B8.z;
    f32 dx = ((speed.x + m3730.x) + m36A0.x) + m36B8.x;
    f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, gabi::fmadds(dx, dx, dz * dz));
    f32 morf;
    if (len > (f64)0.001f) {
        s16 a = cM_atan2s(dx, dz);
        f32 ny = current.pos.y - speed.y;
        f32 cx = current.pos.x;
        f32 cz = current.pos.z;
        f32 sn = cM_ssin((u16)a);
        f32 cs = cM_scos((u16)a);
        current.pos.y = ny;
        gabi::Local<cXyz> p0;
        gabi::Local<cXyz> p1;
        p0->z = cz;
        p0->x = cx;
        p0->y = ny - 5.0f;
        p1->z = gabi::fnmsubs(50.0f, cs, cz);
        p1->x = gabi::fnmsubs(50.0f, sn, cx);
        p1->y = ny - 5.0f;
        void* chk = mLinkLinChk;
        dBgS_LinChk_Set(chk, p0, p1, this);
        u32 pla = 0;
        if (cBgS_LineCross(dComIfG_Bgsp(), chk)) {
            pla = lk_linChkTriPla(me);
        }
        f32 nspeed = mNormalSpeed;
        if (gabi::load<u16>(dComIfGs_base_l() + 0x22) /* dComIfGs_getLife() */ != 0 ||
            gabi::call<BOOL>(0x025B5C80 /* dSv_player_item_c::checkBottle */, dComIfGs_base_l() + 0x5C, 0x57)) {
            if (m357C != 3 && !(LK_FIELD(f32, 0x3CC) < 0.0f) /* !checkGrabWear() */ && !(nspeed < 9.0f) && mStickDistance > 0.85f &&
                cLib_distanceAngleS(a, shape_angle.y) < 0x800) {
                if (pla != 0) {
                    current.pos.x = gabi::load<f32>(me + 0xA00); /* mLinkLinChk.GetCrossP() */
                    current.pos.z = gabi::load<f32>(me + 0xA08);
                }
                return procAutoJump_init();
            }
            if (pla != 0 && !(std::fabs((f32)gabi::load<f32>(pla + 4)) > 0.05f) &&
                gabi::call<s32>(0x024EF080 /* dBgS::GetWallCode */, dComIfG_Bgsp(), me + 0x9E4) != 2 && gndDiff < -125.0f &&
                mWaterY - current.pos.y < -125.0f && current.pos.y - m35D4 > 125.0f) {
                current.pos.x = gabi::load<f32>(me + 0xA00);
                current.pos.z = gabi::load<f32>(me + 0xA08);
                if (gabi::call<s32>(0x024EF080, dComIfG_Bgsp(), me + 0x9E4) == 1) {
                    /* mPolyInfo = mLinkLinChk */
                    gabi::store<u16>(me + 0xCE0, gabi::load<u16>(me + 0x9E4));
                    gabi::store<u16>(me + 0xCE2, gabi::load<u16>(me + 0x9E6));
                    gabi::store<u32>(me + 0xCE4, gabi::load<u32>(me + 0x9E8));
                    gabi::store<u32>(me + 0xCE8, gabi::load<u32>(me + 0x9EC));
                    return procClimbDownStart_init(cM_atan2s(gabi::load<f32>(pla + 0), gabi::load<f32>(pla + 8)));
                }
                if (procHangFallStart_init(gabi::at<void>(pla))) {
                    return TRUE;
                }
            }
        }
        morf = mCurProc == 0x1E /* daPyProc_FRONT_ROLL_e */ ? 0.0f : 6.0f;
        if (pla != 0) {
            current.pos.x = gabi::fmadds(35.0f, gabi::load<f32>(pla + 0), current.pos.x);
            current.pos.z = gabi::fmadds(35.0f, gabi::load<f32>(pla + 8), current.pos.z);
        } else {
            current.pos.x = gabi::fmadds(35.0f, sn, current.pos.x);
            current.pos.z = gabi::fmadds(35.0f, cs, current.pos.z);
        }
    } else {
        morf = 6.0f;
    }
    s32 staff = mStaffIdx;
    if (staff != -1 && gabi::call<u32>(0x0254487C /* dEvent_manager_c::getMySubstanceP */, dComIfGp_ea() + 0x52C4, staff,
                                       0x100358E4 /* "fall" */, 3) != 0) {
        return procFall_init(0, morf);
    }
    return procFall_init(1, morf);
}
VERIFY(0x023F81A4, &daPy_lk_c::changeAutoJumpProc);

/* 023F8858 */
f32 daPy_lk_c::getSwimTimerRate() {
    WWHD_FUNC(0x023F8858, f32, this);
    if (gabi::load<u8>(dComIfGp_ea() + 0x5BB0) /* dComIfGp_getItemSwimTimerStatus() */ != 0 && !dComIfGp_event_runCheck_l()) {
        s32 cnt = gabi::load<s32>(dComIfGp_ea() + 0x5B4C); /* dComIfGp_getItemTimeCount() */
        return gabi::fnmsubs((f32)cnt, 0.0011111111f, 1.0f);
    }
    if (gabi::load<s32>(dComIfGp_ea() + 0x5B4C) <= 0) {
        return 1.0f;
    }
    return 0.0f;
}
VERIFY(0x023F8858, &daPy_lk_c::getSwimTimerRate);

/* cM_rad2s (02019510) and the cosine table entry of its angle */
static inline f32 lk_cosOfRad(f32 rad) {
    u16 a = (u16)gabi::call<s32>(0x02019510 /* cM_rad2s */, rad);
    return gabi::load<f32>(0x104A44FC + ((a >> 3) << 3));
}

/* 023F8924 */
void daPy_lk_c::setSwimTimerStartStop() {
    WWHD_FUNC(0x023F8924, void, this);
    f32 thr = mWaterY - 175.0f;
    f32 gnd = mAcch.m_ground_h;
    f32 target = 0.0f;
    u32 play = dComIfGp_ea();
    if (!(gnd > thr)) {
        gabi::store<u8>(play + 0x5BB0, 1); /* dComIfGp_startItemSwimTimer() */
        if (mNoResetFlg0 & 0x100 /* daPyFlg0_UNK100 */) {
            f32 rate = getSwimTimerRate();
            if (rate > 0.5f) {
                target = -30.0f * lk_cosOfRad((1.0f - rate) * 3.1415927f);
                if (mFrameCtrlUnder[0].checkPass(0.0f) && mCurProc != 0x35 /* daPyProc_SWIM_UP_e */) {
                    if (mProcVar6 != 0 && !(mNoResetFlg0 & 0x4000 /* daPyFlg0_UNK4000 */)) {
                        LK_voiceStart(0x22);
                        mProcVar6 = 0;
                    } else {
                        mProcVar6 = 1;
                    }
                }
            }
        }
    } else {
        gabi::store<s32>(play + 0x5B4C, 900); /* dComIfGp_setItemTimeCount(900) */
        gabi::store<u8>(play + 0x5BB0, 1);
        gabi::store<u8>(dComIfGp_ea() + 0x5BB0, 0); /* dComIfGp_stopItemSwimTimer() */
    }
    cLib_chaseF(&m3608, target, 3.0f);
}
VERIFY(0x023F8924, &daPy_lk_c::setSwimTimerStartStop);

/* 023F8B00 */
BOOL daPy_lk_c::procSwimWait_init(int keep) {
    WWHD_FUNC(0x023F8B00, BOOL, this, keep);
    J3DFrameCtrl& fc = mFrameCtrlUnder[0];
    gabi::call(LK_commonProcInit, this, 0x36 /* daPyProc_SWIM_WAIT_e */);
    f32 r;
    if (keep) {
        r = fc.mFrame / (f32)fc.mEnd;
        f32 c = std::fabs(lk_cosOfRad(r * 3.1415927f));
        f32 sp = speedF;
        f32 a = sp * 0.6f;
        f32 b = sp * c;
        gravity = 0.0f;
        mNormalSpeed = gabi::fmadds(b, 0.4f, a);
    } else {
        r = 0.0f;
        gravity = 0.0f;
    }
    f32 rate = getSwimTimerRate();
    gabi::call(LK_setSingleMoveAnime, this, 0x82 /* ANM_SWIMWAIT */, gabi::fmadds(rate, 2.5f, 0.5f), 0.0f, -1, 18.0f);
    f32 f = r * (f32)fc.mEnd;
    fc.mFrame = f;
    gabi::store<f32>(LK_FIELD(u32, 0x57FC) /* mAnmRatioUnder[0].getAnmTransform() */, f);
    dComIfGp_onPlayerStatus0_l(0x100000 /* daPyStts0_SWIM_e */);
    if (mNoResetFlg0 & 0x100) {
        if (mEquipItem != 0x100) {
            gabi::call(LK_deleteEquipItem, this, 1);
        }
        current.pos.y = mWaterY;
        speed.y = 0.0f;
    }
    mProcVar2 = 0;
    m35C4 = 1.0f;
    mProcVar6 = 0;
    return TRUE;
}
VERIFY(0x023F8B00, &daPy_lk_c::procSwimWait_init);

/* a function-local static TVec3<f32> (guard word, then x y z) */
static inline void lk_staticScale(u32 guard, u32 vec, f32 v) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::store<f32>(vec + 0, v);
        gabi::store<f32>(vec + 8, v);
        gabi::store<f32>(vec + 4, v);
    }
}

/* 023F8D60 */
BOOL daPy_lk_c::procSwimUp_init(int splash) {
    WWHD_FUNC(0x023F8D60, BOOL, this, splash);
    const u32 splashScale = 0x1046D0D4, rippleScale = 0x1046D0E0;
    lk_staticScale(0x1046D0CC, splashScale, 0.4f);
    lk_staticScale(0x1046D0D0, rippleScale, 0.3f);
    gabi::call(LK_commonProcInit, this, 0x35 /* daPyProc_SWIM_UP_e */);
    speed.y = 0.0f;
    current.pos.y = mWaterY;
    gravity = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0x81 /* ANM_SWIMP */, 0.7f, 0.0f, 0x18, 3.0f);
    dComIfGp_onPlayerStatus0_l(0x100000 /* daPyStts0_SWIM_e */);
    m35C4 = 0.0f;
    mNoResetFlg0 = mNoResetFlg0 | 0x100;
    if (splash) {
        u32 pa = gabi::load<u32>(dComIfGp_ea() + 0x5AB0);
        u32 e = gabi::call<u32>(0x025A847C, pa, 1, 0x40 /* ID_IT_JN_WP_SHIBUKI */, &current.pos, 0, 0, 0xFF, 0, -1, 0, 0, 0);
        if (e != 0) {
            gabi::store<f32>(e + 0x34, 15.0f); /* setRate */
            f32 x = gabi::load<f32>(splashScale + 0);
            gabi::store<f32>(e + 0x220, x); /* setGlobalDynamicsScale */
            f32 y = gabi::load<f32>(splashScale + 4);
            gabi::store<f32>(e + 0x224, y);
            f32 z = gabi::load<f32>(splashScale + 8);
            gabi::store<f32>(e + 0x238, x); /* setGlobalParticleScale */
            gabi::store<f32>(e + 0x228, z);
            gabi::store<f32>(e + 0x23C, y);
            gabi::store<f32>(e + 0x240, z);
        }
        pa = gabi::load<u32>(dComIfGp_ea() + 0x5AB0);
        e = gabi::call<u32>(0x025A847C, pa, 5, 0x3D /* ID_IT_JN_WP_HAMON01 */, &current.pos, 0, 0, 0xFF,
                            0x1047B2E4 /* dPa_control_c::mSingleRippleEcallBack */, -1, 0, 0, 0);
        if (e != 0) {
            f32 x = gabi::load<f32>(rippleScale + 0);
            gabi::store<f32>(e + 0x220, x);
            f32 y = gabi::load<f32>(rippleScale + 4);
            gabi::store<f32>(e + 0x224, y);
            f32 z = gabi::load<f32>(rippleScale + 8);
            gabi::store<f32>(e + 0x238, x);
            gabi::store<f32>(e + 0x228, z);
            gabi::store<f32>(e + 0x23C, y);
            gabi::store<f32>(e + 0x240, z);
        }
    }
    mProcVar6 = 0;
    return TRUE;
}
VERIFY(0x023F8D60, &daPy_lk_c::procSwimUp_init);

/* 023F8F80 */
BOOL daPy_lk_c::changeSwimProc() {
    WWHD_FUNC(0x023F8F80, BOOL, this);
    if (!(mNoResetFlg0 & 0x80 /* daPyFlg0_UNK80 */) || (mModeFlg & 0x42000 /* IN_SHIP | SWIM */)) {
        return FALSE;
    }
    /* HD: GameCube's !checkNoControll() became daPy_getPlayerActorClass() == this */
    if (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) != gabi::ea(this) || LK_demoMode == 0x11 /* DEMO_PFALL_e */ ||
        !(mWaterY - current.pos.y > 90.0f)) {
        return FALSE;
    }
    u32 play = dComIfGp_ea();
    gabi::store<s32>(play + 0x5B4C, 900); /* dComIfGp_setItemTimeCount(900) */
    gabi::store<u8>(play + 0x5BB0, 1);
    gabi::store<s32>(dComIfGp_ea() + 0x5B50, 900); /* dComIfGp_setItemTimeMax(900) */
    mNoResetFlg0 = mNoResetFlg0 & ~0x02000000u; /* offNoResetFlg0(daPyFlg0_EQUIP_HEAVY_BOOTS) */
    setNoResetFlg1(noResetFlg1() & ~1u);      /* offNoResetFlg1(daPyFlg1_EQUIP_DRAGON_SHIELD) */
    gabi::call(0x023E4E2C /* endFlameDamageEmitter */, this);
    f32 ns = mNormalSpeed * 0.75f;
    mMaxNormalSpeed = 18.0f;
    mNormalSpeed = ns;
    mNoResetFlg0 = mNoResetFlg0 & ~0x100u;
    if (!(ns < 18.0f)) {
        mNormalSpeed = 18.0f;
    }
    m35C4 = 0.0f;
    setSwimTimerStartStop();
    if (mModeFlg & 2 /* ModeFlg_MIDAIR */) {
        f32 sy = mOldSpeed.y;
        current.pos.y = current.pos.y + 90.0f;
        if (sy < -50.0f) {
            sy = -50.0f;
        } else if (sy > 0.0f) {
            sy = 0.0f;
        }
        speed.y = sy;
        f32 oy = mOldSpeed.y;
        f32 k = gabi::fmadds(oy * oy, 0.0004f, 0.2f);
        f32 wy = mWaterY;
        f32 cx = current.pos.x;
        if (k > 1.0f) {
            k = 1.0f;
        }
        gabi::Local<cXyz> pos;
        pos->x = cx;
        pos->y = wy;
        pos->z = current.pos.z;
        gabi::call(0x025DAE64 /* fopKyM_createWpillar */, pos.get(), 1.0f, k, 0);
        seStartOnlyReverb(0x3808 /* JA_SE_LK_INTO_WATER */);
        return procSwimWait_init(0);
    }
    return procSwimUp_init(0);
}
VERIFY(0x023F8F80, &daPy_lk_c::changeSwimProc);

/* 023F9214 */
BOOL daPy_lk_c::dProcFreezeDamage_init() {
    WWHD_FUNC(0x023F9214, BOOL, this);
    if (mCurProc == 0xB1 /* daPyProc_DEMO_FREEZE_DAMAGE_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xB1);
    mProcVar0 = 60;
    mNormalSpeed = 0.0f;
    if (dComIfGp_event_compulsory_l(this)) {
        dProcFreezeDamage_init_sub(1);
    } else {
        mProcVar6 = 0;
    }
    return TRUE;
}
VERIFY(0x023F9214, &daPy_lk_c::dProcFreezeDamage_init);

/* 023F92AC */
s32 daPy_lk_c::checkWallAtributeDamage(dBgS_AcchCir* cir) {
    WWHD_FUNC(0x023F92AC, s32, this, cir);
    if ((cir->m_flags & dBgS_AcchCir::WALL_HIT) && gabi::call<BOOL>(0x02008254 /* cBgS::ChkPolySafe */, dComIfG_Bgsp(), cir)) {
        s32 code = gabi::call<s32>(0x024EF0F4 /* dBgS::GetAttributeCode */, dComIfG_Bgsp(), cir);
        if (code == 9 /* dBgS_Attr_DAMAGE_e */ || code == 0x16 /* dBgS_Attr_ELECTRICITY_e */) {
            return code;
        }
    }
    return 0;
}
VERIFY(0x023F92AC, &daPy_lk_c::checkWallAtributeDamage);

/* daPy_dmEcallBack_c statics: the damage effect type (0x101CEF16) and its timer (0x101CEF14) */
#define LK_dmType() gabi::load<u16>(0x101CEF16)
static inline void lk_dmSet(u16 type, s16 timer) {
    gabi::store<u16>(0x101CEF16, type);
    gabi::store<s16>(0x101CEF14, timer);
}

/* 023F9340 */
void daPy_lk_c::setDamageElecEmitter() {
    WWHD_FUNC(0x023F9340, void, this);
    if (LK_dmType() != 2 /* !daPy_dmEcallBack_c::checkElec() */) {
        gabi::call(0x023DCA08 /* endDamageEmitter */, this);
        u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 4 /* CL_JNT_CHEST_JNT_e */);
        gabi::call(0x023D457C /* makeEmitter */, gabi::ea(this) + 0x67CC /* mDmEcallBack[0] */, 0x3ED /* ID_AK_JN_CCTHUNDER00 */, mtx,
                   &current.pos, nullptr);
        mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 4);
        gabi::call(0x023D457C, gabi::ea(this) + 0x67D8 /* mDmEcallBack[1] */, 0x3EE /* ID_AK_JN_CCTHUNDER01 */, mtx, &current.pos, nullptr);
    }
    lk_dmSet(2, 0x4B); /* daPy_dmEcallBack_c::setElec(75) */
}
VERIFY(0x023F9340, &daPy_lk_c::setDamageElecEmitter);

/* 023F93FC */
void daPy_lk_c::setDamageFlameEmitter() {
    WWHD_FUNC(0x023F93FC, void, this);
    const u32 armScale = 0x1046D0F0;
    lk_staticScale(0x1046D0EC, armScale, 0.6f);
    if (LK_dmType() != 0 /* !daPy_dmEcallBack_c::checkFlame() */) {
        gabi::call(0x023DCA08 /* endDamageEmitter */, this);
        gabi::Local<cXyz> pos;
        for (int i = 0; i < 4; i++) {
            u32 jnt = 0x10035904 + i * 2; /* flame_joint */
            u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), gabi::load<u16>(jnt));
            pos->x = gabi::load<f32>(mtx + 0xC); /* mDoMtx_multVecZero */
            pos->y = gabi::load<f32>(mtx + 0x1C);
            pos->z = gabi::load<f32>(mtx + 0x2C);
            mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), gabi::load<u16>(jnt));
            u32 cb = gabi::ea(this) + 0x67CC + i * 0xC; /* mDmEcallBack[i] */
            gabi::call(0x023D457C /* makeEmitter */, cb, 0x3F1 /* ID_AK_JN_BODYABLAZE00 */, mtx, pos.get(), nullptr);
            u32 e = gabi::load<u32>(cb + 4);
            if (e != 0 && (i == 2 || i == 3)) {
                f32 x = gabi::load<f32>(armScale + 0); /* setGlobalScale */
                gabi::store<f32>(e + 0x220, x);
                f32 y = gabi::load<f32>(armScale + 4);
                gabi::store<f32>(e + 0x224, y);
                f32 z = gabi::load<f32>(armScale + 8);
                gabi::store<f32>(e + 0x238, x);
                gabi::store<f32>(e + 0x228, z);
                gabi::store<f32>(e + 0x23C, y);
                gabi::store<f32>(e + 0x240, z);
            }
        }
    }
    lk_dmSet(0, 100); /* daPy_dmEcallBack_c::setFlame(100) */
}
VERIFY(0x023F93FC, &daPy_lk_c::setDamageFlameEmitter);

/* 023F955C */
void daPy_lk_c::setDamageEmitter() {
    WWHD_FUNC(0x023F955C, void, this);
    u32 obj = gabi::call<u32>(0x02516360 /* dCcD_GObjInf::GetTgHitGObj */, &mCyl);
    if (obj != 0) {
        u8 spl = gabi::load<u8>(obj + 0x6F); /* GetAtSpl() */
        if (spl == 3) {
            gabi::call(0x023DCB54 /* setDamageCurseEmitter */, this);
        } else if (spl == 0xB) {
            setDamageElecEmitter();
        } else if (gabi::load<u32>(obj + 0x10) & 0x200 /* ChkAtType(AT_TYPE_FIRE) */) {
            setDamageFlameEmitter();
        }
    }
}
VERIFY(0x023F955C, &daPy_lk_c::setDamageEmitter);

/* 023F95F4 */
BOOL daPy_lk_c::procElecDamage_init(const cXyz* from) {
    WWHD_FUNC(0x023F95F4, BOOL, this, from);
    if (!dComIfGp_event_compulsory_l(this)) {
        return FALSE;
    }
    LK_demoType = 5;
    int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
    if (mModeFlg & 0x40000 /* ModeFlg_SWIM */) {
        current.pos.y = current.pos.y + m35C4;
        mProcVar2 = 1;
        gabi::call(0x023DFAAC /* swimOutAfter */, this, 1);
    } else {
        mProcVar2 = 0;
    }
    gabi::call(LK_commonProcInit, this, 0x6C /* daPyProc_ELEC_DAMAGE_e */);
    if (!(mAcch.m_flags & dBgS_Acch::GROUND_HIT)) {
        gravity = 0.0f;
        if (mProcVar2 == 0) {
            mModeFlg = mModeFlg | 2;
        }
    }
    speed.y = 0.0f;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xCB /* ANM_DAMBIRI */, 1.0f, 0.0f, -1, 0.0f);
    mDamageWaitTimer = 0x1E;
    mProcVar0 = 0x14;
    if (from != nullptr) {
        gabi::Local<cXyz> d;
        cXyz_mi(&current.pos, d, from);
        lk_copy12(gabi::ea(&m370C), d);
        mProcVar3 = 1;
    } else if (gabi::call<BOOL>(0x025162A4 /* dCcD_GObjInf::ChkTgHit */, &mCyl)) {
        cXyz* v = getDamageVec(&mCyl);
        lk_copy12(gabi::ea(&m370C), v);
        mProcVar3 = 1;
    } else {
        mProcVar3 = 0;
    }
    gabi::call(LK_initShipRideUseItem, this, use, 0);
    seStartOnlyReverb(0x287C /* JA_SE_LK_ELEC_PARALYSED */);
    LK_voiceStart(0x2C);
    lk_startShockUp_l(4);
    return TRUE;
}
VERIFY(0x023F95F4, &daPy_lk_c::procElecDamage_init);

/* 023F9930 */
BOOL daPy_lk_c::checkElecReturnDamage(dCcD_GObjInf* obj, cXyz* out) {
    WWHD_FUNC(0x023F9930, BOOL, this, obj, out);
    if (gabi::call<BOOL>(0x025160DC /* dCcD_GObjInf::ChkAtHit */, obj) &&
        gabi::call<u32>(0x025161D8 /* GetAtHitGObj (matcher: GetTgHitGObj) */, obj) != 0) {
        u32 g = gabi::call<u32>(0x025161D8, obj);
        if (gabi::load<u8>(g + 0xB3) /* GetTgSpl() */ == 1) {
            lk_copy12(gabi::ea(out), gabi::at<cXyz>(gabi::ea(obj) + 0x70) /* GetAtHitPosP() */);
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x023F9930, &daPy_lk_c::checkElecReturnDamage);

/* 023F99D0 */
BOOL daPy_lk_c::procDamage_init() {
    WWHD_FUNC(0x023F99D0, BOOL, this);
    cXyz* v = getDamageVec(&mCyl);
    u16 sy = (u16)shape_angle.y;
    f32 sn = cM_ssin(sy);
    f32 cs = cM_scos(sy);
    gabi::call(LK_commonProcInit, this, 0x66 /* daPyProc_DAMAGE_e */);
    mDamageWaitTimer = 0x1E;
    f32 vx = v->x;
    f32 vy = v->y;
    f32 vz = v->z;
    f32 lz = gabi::fmadds(vz, cs, vx * sn);
    f32 lx = gabi::fmsubs(vx, cs, vz * sn);
    s16 a = cM_atan2s(lz, vy);
    f32 sq = gabi::fmadds(vy, vy, lz * lz);
    mProcVar2 = a;
    f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, sq);
    s16 b = (s16)gabi::call<s32>(0x020195B0 /* cM_atan2s */, -lx, len);
    mProcVar3 = b;
    s16 v3 = b;
    if (mProcVar2 > 0x1F40) {
        mProcVar2 = 0x1F40;
        v3 = mProcVar3;
    } else if (mProcVar2 < -0x1F40) {
        v3 = mProcVar3;
        mProcVar2 = -0x1F40;
    }
    if (v3 > 0x1F40) {
        mProcVar3 = 0x1F40;
    } else if (v3 < -0x1F40) {
        mProcVar3 = -0x1F40;
    }
    int dir = getDirectionFromAngle(cM_atan2s(lx, lz));
    int anm;
    if (dir == 1 /* DIR_BACKWARD */) {
        anm = 0x57; /* ANM_DAMF */
    } else if (dir == 2 /* DIR_LEFT */) {
        anm = 0x56; /* ANM_DAMR */
    } else if (dir == 3 /* DIR_RIGHT */) {
        anm = 0x55; /* ANM_DAML */
    } else {
        anm = 0x58; /* ANM_DAMB */
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, 0.6f, 0.0f, 9, 0.0f);
    current.angle.y = cM_atan2s(v->x, v->z);
    if (anm != 0x56) {
        mFootEffectPosType = 2;
    }
    if (anm != 0x55) {
        mFootEffectPosType = 1;
    }
    gabi::Local<cXyz> xz; /* damage_vec->absXZ() */
    f32 z = v->z;
    xz->x = v->x;
    xz->z = z;
    xz->y = 0.0f;
    f64 sq2 = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get());
    f64 len2 = gabi::call<f64>(0x028F4384, sq2);
    mNormalSpeed = (f32)(len2 * (f64)0.05f + (f64)13.0f);
    LK_voiceStart(2);
    seStartOnlyReverb(0x282D /* JA_SE_LK_DAMAGE_NORMAL */);
    mProcVar6 = anm;
    mNoResetFlg0 = mNoResetFlg0 & ~2u;
    lk_startShockUp_l(4);
    return TRUE;
}
VERIFY(0x023F99D0, &daPy_lk_c::procDamage_init);

/* 023F9D1C HD-only helper (no GameCube function; the damage-wait part of the damage dispatch) */
void daPy_lk_c::setDamagePointWait(f32 amount) {
    WWHD_FUNC(0x023F9D1C, void, this, amount);
    if (mDamageWaitTimer == 0) {
        setDamagePoint(amount);
        mDamageWaitTimer = 0x1E;
        LK_voiceStart(2);
        seStartOnlyReverb(0x282D /* JA_SE_LK_DAMAGE_NORMAL */);
    }
}
VERIFY(0x023F9D1C, &daPy_lk_c::setDamagePointWait);

/* 023F9D84 */
BOOL daPy_lk_c::checkNormalDamage(int life) {
    WWHD_FUNC(0x023F9D84, BOOL, this, life);
    u16 up = LK_upperIdx();
    if (up == 0x95 || up == 0x96 /* checkGrabAnime() */ || mCurProc != 6 /* daPyProc_MOVE_e */) {
        return TRUE;
    }
    f32 ns = mNormalSpeed;
    f32 k;
    if (m3580 == 8) {
        k = 1.0f * ns;
    } else {
        k = cM_scos((u16)m34E2) * ns;
    }
    if (std::fabs(k / mMaxNormalSpeed) < 0.9f) {
        return TRUE;
    }
    if (!(noResetFlg1() & 1) && mTinkleShieldTimer == 0 /* !checkNoDamageMode() */ &&
        !gabi::call<BOOL>(0x025B5C80 /* dSv_player_item_c::checkBottle */, dComIfGs_base_l() + 0x5C, 0x57) &&
        (s32)gabi::load<u16>(dComIfGs_base_l() + 0x22) /* dComIfGs_getLife() */ <= life) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023F9D84, &daPy_lk_c::checkNormalDamage);

/* 023F9EAC */
void daPy_lk_c::setDashDamage() {
    WWHD_FUNC(0x023F9EAC, void, this);
    gabi::call(LK_setActAnimeUpper, this, 0x64 /* dRes_INDEX_LKANM_BCK_DAMDASH_e */, 2, 2.3f, 0.0f, -1, 2.4f);
    gabi::call(0x023E7B24 /* setPriTextureAnime */, this, 0x40, 0);
    mDamageWaitTimer = 0x14;
    fcpy_l(gabi::ea(this) + 0x58DC, gabi::ea(this) + 0x58AC); /* mFrameCtrlUpper[2].setFrame(mFrameCtrlUnder[1].getFrame()) */
    LK_voiceStart(2);
    seStartOnlyReverb(0x282D);
}
VERIFY(0x023F9EAC, &daPy_lk_c::setDashDamage);

/* |damage_vec| in XZ through PSVECSquareMag + std::sqrtf (f1 as returned) */
static inline f64 lk_absXZ(cXyz* v) {
    gabi::Local<cXyz> xz;
    f32 z = v->z;
    f32 x = v->x;
    xz->z = z;
    xz->x = x;
    xz->y = 0.0f;
    f64 sq = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get());
    return gabi::call<f64>(0x028F4384 /* std::sqrtf */, sq);
}
/* the knockback (true) or guard slide (false) parameters of a guarded hit */
static inline bool lk_isKnockback(u32 obj) {
    return obj != 0 && (u32)(gabi::load<u8>(obj + 0x6F) /* GetAtSpl() */ - 5) <= 2;
}

/* 023F9F40 */
BOOL daPy_lk_c::procCrouchDefenseSlip_init() {
    WWHD_FUNC(0x023F9F40, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0xD /* daPyProc_CROUCH_DEFENSE_SLIP_e */);
    cXyz* v = getDamageVec(&mCyl);
    current.angle.y = cM_atan2s(v->x, v->z);
    u32 obj = gabi::call<u32>(0x02516360 /* dCcD_GObjInf::GetTgHitGObj */, &mCyl);
    if (lk_isKnockback(obj)) {
        gabi::call(LK_setSingleMoveAnime, this, 0x17 /* ANM_DIFENCEA */, 0.9f, 0.0f, 0xB, 2.0f);
        f64 len = lk_absXZ(v);
        m35A4 = 4.9f;
        mNormalSpeed = (f32)(len * (f64)0.02f + (f64)52.0f);
        mProcVar0 = 0;
        m35A8 = 3.0f;
        m35A0 = 0.5f;
        m35AC = 12.5f;
        f64 r = gabi::call<f64>(0x02019788 /* cM_rnd */);
        mProcVar6 = r < (f64)0.2f;
    } else {
        gabi::call(LK_setSingleMoveAnime, this, 0x17, 0.85f, 0.0f, 0xB, 2.0f);
        f64 len = lk_absXZ(v);
        mProcVar0 = 0;
        mNormalSpeed = (f32)(len * (f64)0.02f + (f64)7.5f);
        m35A0 = 0.5f;
        m35A8 = 0.3f;
        m35AC = 12.5f;
        mProcVar6 = 0;
        m35A4 = 1.125f;
    }
    if (mEquipItem == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        u32 bck = gabi::call<u32>(LK_getItemAnimeResource, this, 0x18 /* dRes_INDEX_LKANM_BCK_ATNGAHAMA_e */);
        bckAnm_changeBckOnly_l(mSwordAnim_ea, bck);
        mFootEffectPosType = 3;
        fcpy_l(gabi::ea(&m35EC), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    } else {
        mFootEffectPosType = 3;
        fcpy_l(gabi::ea(&m35E8), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    }
    dComIfGp_onPlayerStatus1_l(0x80000 /* daPyStts1_UNK80000_e */);
    return TRUE;
}
VERIFY(0x023F9F40, &daPy_lk_c::procCrouchDefenseSlip_init);

/* 023FA1B8 */
BOOL daPy_lk_c::procGuardSlip_init() {
    WWHD_FUNC(0x023FA1B8, BOOL, this);
    cXyz* v = getDamageVec(&mCyl);
    gabi::call(LK_commonProcInit, this, 0x6D /* daPyProc_GUARD_SLIP_e */);
    int anm;
    if (mEquipItem == 0x33) {
        anm = 0x3B; /* ANM_ATNGAHAM */
        u32 bck = gabi::call<u32>(LK_getItemAnimeResource, this, 0x18);
        bckAnm_changeBckOnly_l(mSwordAnim_ea, bck);
    } else if (mDirection == 2 /* DIR_LEFT */) {
        anm = 0x39; /* ANM_ATNGAL */
    } else {
        anm = 0x3A; /* ANM_ATNGAR */
    }
    u32 obj = gabi::call<u32>(0x02516360 /* dCcD_GObjInf::GetTgHitGObj */, &mCyl);
    if (lk_isKnockback(obj)) {
        gabi::call(LK_setSingleMoveAnime, this, anm, 0.9f, 0.0f, 0xB, 2.0f);
        f64 len = lk_absXZ(v);
        mProcVar0 = 0;
        m35AC = 12.5f;
        mNormalSpeed = (f32)(len * (f64)0.02f + (f64)52.0f);
        m35A0 = 0.5f;
        m35A4 = 4.9f;
        m35A8 = 3.0f;
        f64 r = gabi::call<f64>(0x02019788 /* cM_rnd */);
        mProcVar6 = r < (f64)0.2f;
    } else {
        gabi::call(LK_setSingleMoveAnime, this, anm, 0.85f, 0.0f, 0xB, 2.0f);
        f64 len = lk_absXZ(v);
        mProcVar0 = 0;
        m35A0 = 0.5f;
        mProcVar6 = 0;
        m35AC = 12.5f;
        mNormalSpeed = (f32)(len * (f64)0.02f + (f64)7.5f);
        m35A4 = 1.125f;
        m35A8 = 0.3f;
    }
    if (mEquipItem == 0x33) {
        fcpy_l(gabi::ea(&m35EC), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    } else {
        fcpy_l(gabi::ea(&m35E8), gabi::ea(&mFrameCtrlUnder[0].mFrame));
    }
    current.angle.y = cM_atan2s(v->x, v->z);
    mFootEffectPosType = 3;
    lk_pbSetRatio(m_pbCalc[1], 2, 0.0f); /* m_pbCalc[PART_UPPER_e]->setRatio(2, 0.0f) */
    return TRUE;
}
VERIFY(0x023FA1B8, &daPy_lk_c::procGuardSlip_init);

/* 023FA4C4 */
BOOL daPy_lk_c::procPolyDamage_init() {
    WWHD_FUNC(0x023FA4C4, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x67 /* daPyProc_POLY_DAMAGE_e */);
    mDamageWaitTimer = 0x1E;
    gabi::call(LK_setSingleMoveAnime, this, 0x54 /* ANM_DAM */, 0.6f, 0.0f, 9, 1.0f);
    mNormalSpeed = 0.0f;
    LK_voiceStart(2);
    seStartOnlyReverb(0x282D /* JA_SE_LK_DAMAGE_NORMAL */);
    mNoResetFlg0 = mNoResetFlg0 & ~2u;
    return TRUE;
}
VERIFY(0x023FA4C4, &daPy_lk_c::procPolyDamage_init);

/* mCyl's tg checks (dCcD_GObjInf at this + 0x765C) */
#define LK_cylChkTgHit() gabi::call<BOOL>(0x025162A4 /* dCcD_GObjInf::ChkTgHit */, &mCyl)
#define LK_cylGetTgHitGObj() gabi::call<u32>(0x02516360 /* dCcD_GObjInf::GetTgHitGObj */, &mCyl)
#define LK_changePlayer() gabi::call(0x023D4688 /* daPy_py_c::changePlayer */, this, this)

/* 023FA578 */
BOOL daPy_lk_c::changeDamageProc() {
    WWHD_FUNC(0x023FA578, BOOL, this);
    if (!(mModeFlg & 8 /* ModeFlg_DAMAGE */) && mDamageWaitTimer > 0) {
        s16 t = (s16)(mDamageWaitTimer - 1);
        mDamageWaitTimer = t;
        if (t == 0) {
            if (LK_upperIdx() == 0x64 /* checkDashDamageAnime() */) {
                gabi::call(LK_resetActAnimeUpper, this, 2, 2.4f);
            } else if (mModeFlg & 0x40000 /* ModeFlg_SWIM */) {
                gabi::call(0x023DC58C /* resetPriTextureAnime */, this);
            }
        }
    }
    if (mNoResetFlg0 & 0x01000000 /* daPyFlg0_UNK1000000 */) {
        mNoResetFlg0 = mNoResetFlg0 & ~0x01000000u;
        LK_changePlayer();
        return procLargeDamage_init(-5, 1, 0, 0);
    }
    u32 ship = dComIfGp_getShipActor_l();
    bool stop = dComIfGp_event_runCheck_l() || LK_demoType != 0 /* checkPlayerDemoMode() */;
    if (!stop) {
        /* HD: checkNoControll() is daPy_getPlayerActorClass() != this */
        stop = gabi::load<u32>(dComIfGp_ea() + 0x5B2C) != gabi::ea(this) || mCurProc == 0x86 /* daPyProc_SHIP_READY_e */;
    }
    if (!stop && dComIfGp_checkPlayerStatus0_l(0x10000) && ship != 0) {
        stop = gabi::load<u32>(ship + 0x704) != 0 || gabi::load<u32>(ship + 0x70C) != 0; /* ship->checkForceMove() */
    }
    if (stop) {
        mNoResetFlg0 = mNoResetFlg0 & ~0x200u; /* offNoResetFlg0(daPyFlg0_SHIP_DROP) */
        return FALSE;
    }
    s32 attr = 0;
    if ((noResetFlg1() & 4 /* daPyFlg1_UNK4 */) ||
        ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !(mModeFlg & 0x20 /* ModeFlg_HANG */) && mCurrAttributeCode == 0x15 /* dBgS_Attr_FREEZE_e */)) {
        LK_changePlayer();
        return dProcFreezeDamage_init();
    }
    if (dComIfGp_checkPlayerStatus0_l(0x10000) && (mNoResetFlg0 & 0x200)) {
        mNoResetFlg0 = mNoResetFlg0 & ~0x200u;
        LK_changePlayer();
        return procLargeDamage_init(-2, 1, 0, 0);
    }
    BOOL isAttr = FALSE;
    u32 damage;
    if (!(mModeFlg & 8) && mDamageWaitTimer == 0) {
        if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && ((attr = mCurrAttributeCode) == 9 || attr == 0x16)) {
            isAttr = TRUE;
        } else {
            for (int i = 0; i < 3; i++) {
                attr = checkWallAtributeDamage(&mAcchCir[i]);
                if (attr != 0) {
                    isAttr = TRUE;
                    break;
                }
            }
        }
    }
    if (isAttr) {
        damage = 1;
    } else {
        damage = gabi::load<u8>(gabi::ea(this) + 0x7636); /* mStts.GetDmg() */
        if (LK_cylChkTgHit() && LK_cylGetTgHitGObj() != 0 && gabi::load<u32>(LK_cylGetTgHitGObj() + 0x10) == 0x20 /* AT_TYPE_BOMB */) {
            damage = 1;
        }
    }
    u32 spl;
    if (LK_cylChkTgHit()) {
        u32 g = LK_cylGetTgHitGObj();
        if (g != 0) {
            spl = gabi::load<u8>(g + 0x6F); /* GetAtSpl() */
        } else {
            spl = gabi::load<u8>(gabi::ea(this) + 0x7640); /* mStts.GetAtSpl() */
        }
    } else {
        spl = 0;
    }
    if (mModeFlg & 0x04000000) {
        if (isAttr || (LK_cylChkTgHit() && !checkSuccessGuard(spl))) {
            setDamagePoint((f32)-(s32)damage);
            mDamageWaitTimer = 0x14;
            LK_voiceStart(2);
            seStartOnlyReverb(0x282D /* JA_SE_LK_DAMAGE_NORMAL */);
            LK_changePlayer();
            if (LK_cylChkTgHit() && !checkSuccessGuard(spl)) {
                setDamageEmitter();
            } else if (attr == 0x16 /* dBgS_Attr_ELECTRICITY_e */) {
                setDamageElecEmitter();
            }
            u16 type = LK_dmType();
            u32 mf = mModeFlg;
            if (type == 2 /* daPy_dmEcallBack_c::checkElec() */) {
                if ((mf & 0x800 /* ModeFlg_ROPE */) || (mf & 0x40000 /* ModeFlg_SWIM */)) {
                    return procElecDamage_init(nullptr);
                }
            } else if ((mf & 0x40000) && !(noResetFlg1() & 1) && mTinkleShieldTimer == 0) {
                gabi::call(0x023E7B24 /* setPriTextureAnime */, this, 0x40, 0);
            }
        }
    } else {
        u32 atType = gabi::load<u32>(gabi::ea(this) + 0x7B2C); /* mAtCps[0].GetAtType() */
        if (atType != 0x80 /* AT_TYPE_BOKO_STICK */ && atType != 0x100 /* AT_TYPE_WATER */) {
            gabi::Local<cXyz> hitPos;
            u32 me = gabi::ea(this);
            if (checkElecReturnDamage(gabi::at<dCcD_GObjInf>(me + 0x7B1C), hitPos) ||
                checkElecReturnDamage(gabi::at<dCcD_GObjInf>(me + 0x7C54), hitPos) ||
                checkElecReturnDamage(gabi::at<dCcD_GObjInf>(me + 0x7D8C), hitPos)) {
                setDamagePoint(-1.0f);
                setResetFlg0(resetFlg0() | 0x80000 /* daPyRFlg0_UNK80000 */);
                setDamageElecEmitter();
                LK_changePlayer();
                if (procElecDamage_init(hitPos)) {
                    return TRUE;
                }
                if (LK_checkPlayerFly()) {
                    return procLargeDamage_init(-1, 1, 0, 0);
                }
                return procDamage_init();
            }
        }
        if (LK_cylChkTgHit() && mCurProc != 0x6D && mCurProc != 0xD /* !checkGuardSlip() */ &&
            (!dComIfGp_checkPlayerStatus0_l(0x10000) || (!(noResetFlg1() & 1) && mTinkleShieldTimer == 0))) {
            gabi::call(0x023DCA80 /* freeRopeItem */, this);
            gabi::call(0x023DFA7C /* freeHookshotItem */, this);
            if (!checkSuccessGuard(spl)) {
                if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
                    gabi::call(LK_deleteEquipItem, this, 0);
                }
                setDamageEmitter();
                LK_changePlayer();
                setDamagePoint((f32)-(s32)damage);
                u16 type = LK_dmType();
                u32 grab = gabi::ea(mActorKeepGrab.mActor.get());
                if (type == 2 && procElecDamage_init(nullptr)) {
                    return TRUE;
                }
                if (mModeFlg & 0x2000 /* ModeFlg_IN_SHIP */) {
                    /* HD: on the ship the large damage happens only in ship mode 1; otherwise the damage wait */
                    if (gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 1) {
                        return procLargeDamage_init(-9, 1, 0, 0);
                    }
                    setDamagePointWait(0.0f);
                    return TRUE;
                }
                if (LK_FIELD(f32, 0x3CC) < 0.0f /* checkGrabWear() */ && gabi::call<u32>(0x02515BBC /* GetTgHitAc */, gabi::ea(this) + 0x76F0) != 0 &&
                    gabi::load<s16>(gabi::call<u32>(0x02515BBC, gabi::ea(this) + 0x76F0) + 8) == 0xC6 /* fpcNm_NZ_e */ && grab != 0) {
                    cXyz* v = getDamageVec(&mCyl);
                    gabi::store<s16>(grab + 0x32A, cM_atan2s(v->x, v->z));
                    gabi::store<s16>(grab + 0x328, 0x2000);
                    return procLargeDamage_init(-4, 1, 0, 0);
                }
                if (spl == 7 || spl == 2 || spl == 10) {
                    return procLargeDamage_init(-6, 0, 0, 0);
                }
                if (LK_checkPlayerFly() || spl == 6 || spl == 1 || spl == 9) {
                    return procLargeDamage_init(-1, 1, 0, 0);
                }
                if (checkNormalDamage(damage)) {
                    return procDamage_init();
                }
                setDashDamage();
            } else {
                u32 se = gabi::call<u32>(0x0251638C /* dCcD_GObjInf::GetTgHitObjSe */, &mCyl);
                if (se == 6) {
                    seStartOnlyReverb(0x6843 /* JA_SE_OBJ_COL_N_BDY_MPLT */);
                } else if (se == 2 || se == 5) {
                    seStartOnlyReverb(0x6817 /* JA_SE_OBJ_COL_SWS_NMTLP */);
                } else {
                    seStartOnlyReverb(se == 4 ? 0x683C /* JA_SE_OBJ_COL_NWHP_NMTL */ : 0x6815 /* JA_SE_OBJ_COL_SWM_NSWL */);
                }
                if (mCurProc == 0xC /* daPyProc_CROUCH_DEFENSE_e */) {
                    return procCrouchDefenseSlip_init();
                }
                return procGuardSlip_init();
            }
        } else if (isAttr) {
            LK_changePlayer();
            if (mEquipItem == 0x101) {
                gabi::call(LK_deleteEquipItem, this, 0);
            }
            setDamagePoint((f32)-(s32)damage);
            if (attr == 0x16) {
                setDamageElecEmitter();
                if (procElecDamage_init(nullptr)) {
                    return TRUE;
                }
            }
            if (checkNormalDamage(damage)) {
                return procPolyDamage_init();
            }
            setDashDamage();
        }
    }
    gabi::call(0x02515EE0 /* mStts.ClrTg() */, &mStts);
    gabi::call(0x02515EAC /* mStts.ClrAt() */, &mStts);
    return FALSE;
}
VERIFY(0x023FA578, &daPy_lk_c::changeDamageProc);

/* 023FB020 */
BOOL daPy_lk_c::changeBoomerangCatchProc() {
    WWHD_FUNC(0x023FB020, BOOL, this);
    if (mNoResetFlg0 & 0x20 /* daPyFlg0_UNK20 */) {
        if (!dComIfGp_event_runCheck_l() && LK_demoType == 0 && (mModeFlg & 0x20000) &&
            (LK_upperIdx() == 0xFFFF /* checkNoUpperAnime() */ || LK_upperIdx() == 0x34 /* checkBoomerangThrowAnime() */)) {
            gabi::call(LK_setActAnimeUpper, this, 0x33 /* dRes_INDEX_LKANM_BCK_BOOMCATCH_e */, 2, 1.0f, 1.0f, 0xB, 0.0f);
            gabi::call(0x023E7B24 /* setPriTextureAnime */, this, 0x49, 0);
            seStartOnlyReverb(0x2815 /* JA_SE_LK_BOOM_CATCH */);
            if (mModeFlg & 1) {
                return LK_callB(0x023E2648 /* procBoomerangCatch_init */);
            }
        }
        mNoResetFlg0 = mNoResetFlg0 & ~0x20u;
    }
    return FALSE;
}
VERIFY(0x023FB020, &daPy_lk_c::changeBoomerangCatchProc);

/* 023FB10C */
void daPy_lk_c::throwBoomerang() {
    WWHD_FUNC(0x023FB10C, void, this);
    u32 boom = gabi::ea(mActorKeepEquip.mActor.get());
    gabi::store<u32>(boom + 0xB0, 1); /* fopAcM_SetParam(boomerang, daBoomerang_c::Mode_Throw) */
    fopAc_ac_c* lock = mpAttnActorLockOn;
    if (lock != nullptr) {
        gabi::call(0x020CF87C /* daBoomerang_c::setAimActor */, boom, lock);
    }
    gabi::store<s16>(boom + 0x32A, shape_angle.y);
    u32 id = mActorKeepEquip.mID;
    u32 ac = gabi::ea(mActorKeepEquip.mActor.get());
    mActorKeepThrow.mID = id;
    gabi::store<u32>(gabi::ea(&mActorKeepThrow) + 4, ac);
    gabi::call(0x023DC63C /* daPy_actorKeep_c::clearData */, &mActorKeepEquip);
    mEquipItem = 0x100;
    dComIfGp_onPlayerStatus0_l(0x400000 /* daPyStts0_BOOMERANG_WAIT_e */);
    s16 by = gabi::load<s16>(gabi::ea(this) + 0x3D2); /* mBodyAngle.y */
    gabi::store<s16>(gabi::ea(this) + 0x3D2, 0);
    s16 a = (s16)(shape_angle.y + by);
    shape_angle.y = a;
    current.angle.y = a;
    LK_voiceStart(0);
    if (dComIfGp_checkPlayerStatus0_l(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        u32 st = dComIfGp_ea() + 0x5CD8; /* dComIfGp_clearPlayerStatus0(0, daPyStts0_BOOMERANG_AIM_e) */
        gabi::store<u32>(st, gabi::load<u32>(st) & ~0x80000u);
        mModeFlg = (mModeFlg & ~0x20000000u) | 0x80; /* offModeFlg(ModeFlg_SUBJECT); onModeFlg(ModeFlg_00000080) */
    } else if (mAcch.m_flags & dBgS_Acch::GROUND_HIT) {
        gabi::call(LK_procWait_init, this);
    }
}
VERIFY(0x023FB10C, &daPy_lk_c::throwBoomerang);

/* the item-equip pass frames of the upper animation (rest/take/take-both/single item) */
#define LK_checkPass(fc, f) gabi::call<BOOL>(0x027F2BF8 /* J3DFrameCtrl::checkPass */, fc, (f32)(f))

/* 023FB230 */
void daPy_lk_c::checkItemAction() {
    WWHD_FUNC(0x023FB230, void, this);
    /* the ready item button: HD has a fourth item button (3) */
    u8 sel;
    if ((sel = dComIfGp_getSelectItem_l(0), mEquipItem == sel) && (sel = dComIfGp_getSelectItem_l(1), mEquipItem != sel) &&
        (sel = dComIfGp_getSelectItem_l(2), mEquipItem != sel)) {
        mReadyItemBtn = 0;
    } else if ((sel = dComIfGp_getSelectItem_l(1), mEquipItem == sel) && (sel = dComIfGp_getSelectItem_l(0), mEquipItem != sel) &&
               (sel = dComIfGp_getSelectItem_l(2), mEquipItem != sel)) {
        mReadyItemBtn = 1;
    } else if ((sel = dComIfGp_getSelectItem_l(2), mEquipItem == sel) && (sel = dComIfGp_getSelectItem_l(0), mEquipItem != sel) &&
               (sel = dComIfGp_getSelectItem_l(1), mEquipItem != sel)) {
        mReadyItemBtn = 2;
    } else if ((sel = dComIfGp_getSelectItem_l(3), mEquipItem == sel)) {
        mReadyItemBtn = 3;
    }
    BOOL equipAnime = checkEquipAnime();
    u16 up = LK_upperIdx();
    if (!(equipAnime || up == 0x34 /* BOOMTHROW */ || up == 0x33 /* BOOMCATCH */ || up == 0xE2 /* ROPETHROW */)) {
        return;
    }
    J3DFrameCtrl* fc = &mFrameCtrlUpper[2];
    if (up == 0x33) {
        if (fc->mRate < 0.01f || (mStickDistance > 0.05f && fc->mFrame > 11.0f)) {
            gabi::call(LK_resetActAnimeUpper, this, 2, 2.4f);
            if (dComIfGp_checkPlayerStatus0_l(0x10000)) {
                procShipPaddle_init();
            }
        }
        return;
    }
    if (up == 0x34) {
        if (fc->mRate < 0.01f || (mStickDistance > 0.05f && fc->mFrame > 10.0f)) {
            gabi::call(LK_resetActAnimeUpper, this, 2, 2.4f);
        } else if (LK_checkPass(fc, 7.0f)) {
            throwBoomerang();
        }
        return;
    }
    if (up == 0xE2) {
        if (fc->mRate < 0.01f) {
            mFrameCtrlUpper[1].mRate = 0.0f;
            mFrameCtrlUpper[0].mRate = 1.25f;
            mFrameCtrlUnder[0].mRate = 0.0f;
            mFrameCtrlUnder[1].mRate = 0.0f;
        }
        return;
    }
    if (!equipAnime) {
        return;
    }
    if (mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        u16 u = up;
        bool se = false;
        if (u == 0xD7 /* dRes_INDEX_LKANM_BCK_REST_e */) {
            se = LK_checkPass(fc, 7.0f - fc->mRate);
            if (!se) {
                u = LK_upperIdx();
            }
        }
        if (!se && u == 0x103 /* dRes_INDEX_LKANM_BCK_TAKE_e */) {
            se = LK_checkPass(fc, 7.0f - fc->mRate);
            if (!se) {
                u = LK_upperIdx();
            }
        }
        if (!se && u == 0x104 /* dRes_INDEX_LKANM_BCK_TAKEBOTH_e */) {
            se = LK_checkPass(fc, 6.0f - fc->mRate);
        }
        if (!se && checkSingleItemEquipAnime()) {
            se = LK_checkPass(fc, 4.0f - fc->mRate);
        }
        if (se) {
            seStartOnlyReverb(0x280A /* JA_SE_LK_SW_PUTIN_S */);
        }
    }
    if (std::fabs((f32)fc->mRate) < 0.01f) {
        gabi::call(LK_resetActAnimeUpper, this, 2, 2.4f);
        return;
    }
    {
        u16 u = LK_upperIdx();
        bool pass = false;
        if (u == 0xD7) {
            pass = LK_checkPass(fc, 7.0f);
            if (!pass) {
                u = LK_upperIdx();
            }
        }
        if (!pass && u == 0x103) {
            pass = LK_checkPass(fc, 7.0f);
            if (!pass) {
                u = LK_upperIdx();
            }
        }
        if (!pass && u == 0x104) {
            pass = LK_checkPass(fc, 6.0f);
        }
        if (!pass && checkSingleItemEquipAnime()) {
            pass = LK_checkPass(fc, 4.0f);
        }
        if (!pass) {
            return;
        }
    }
    u16 eq = mEquipItem;
    if ((eq != 0x100 && eq != 0x103) || (m3562 != 0x100 && m3562 != 0x103)) {
        seStartOnlyReverb(0x2810 /* JA_SE_LK_ITEM_TAKEOUT */);
        eq = mEquipItem;
    }
    BOOL had = eq != 0x100;
    gabi::call(LK_deleteEquipItem, this, 0);
    mEquipItem = m3562;
    if (!dComIfGp_event_runCheck_l() && LK_demoType == 0) {
        if (mEquipItem != 0x103 || !(noResetFlg1() & 0x04000000 /* daPyFlg1_UNK4000000 */)) {
            if (lk_checkAttentionLock(mpAttention) || mEquipItem != 0x103 || had) {
                setResetFlg0(resetFlg0() | 0x80 /* daPyRFlg0_UNK80 */);
            }
        }
    }
    if (mCurProc == 0xC5 /* daPyProc_DEMO_STAND_ITEM_PUT_e */) {
        gabi::call(LK_resetActAnimeUpper, this, 2, -1.0f);
    } else if (mEquipItem == 0x103) {
        seStartOnlyReverb(0x2807 /* JA_SE_LK_SW_PULLOUT_S */);
        gabi::call(0x023DE0B4 /* setSwordModel */, this, 0);
    } else {
        gabi::call(0x023DF600 /* makeItemType */, this);
    }
}
VERIFY(0x023FB230, &daPy_lk_c::checkItemAction);

/* 023FB8A0 */
void daPy_lk_c::setShieldGuard() {
    WWHD_FUNC(0x023FB8A0, void, this);
    gabi::Local<be<f32>> ratio; /* m_pbCalc[PART_UPPER_e]->getRatio(2) (joint 0) */
    *ratio.get() = gabi::load<f32>(gabi::load<u32>(gabi::load<u32>(m_pbCalc[1] + 0x7C) + 0x2C));
    s32 proc = mCurProc;
    bool defend;
    if (proc == 0x6D || proc == 0xD /* checkGuardSlip() */) {
        defend = true;
    } else if (checkEquipAnime() || LK_upperIdx() == 0x95 || LK_upperIdx() == 0x96 /* checkGrabAnime() */) {
        defend = false;
    } else {
        defend = false;
        if (lk_checkAttentionLock(mpAttention)) {
            u16 eq = mEquipItem;
            if (eq != 0x101 /* daPyItem_BOKO_e */ && gabi::load<u8>(dComIfGs_base_l() + 0x2F) != 0xFF /* checkShieldEquip() */) {
                if (checkGuardAccept() && (mActorKeepEquip.mID == 0xFFFFFFFF || mActorKeepEquip.mActor.get() != nullptr)) {
                    defend = true;
                }
            }
        }
        proc = mCurProc;
    }
    if (defend) {
        dComIfGp_setRStatus_l(0x36 /* dActStts_DEFEND_e */);
        proc = mCurProc;
    }
    u16 anm = mEquipItem == 0x33 /* dItemNo_SKULL_HAMMER_e */ ? 0x1B /* ATNGHAM */ : 0x16 /* ATNG */;
    if (proc != 0x6D && proc != 0xD) {
        if ((mItemButton & 0x40 /* spActionButton() */) && dComIfGp_getRStatus_l() == 0x36) {
            gabi::call(0x023DCF8C /* freeGrabItem */, this);
            if (LK_upperIdx() != anm) {
                gabi::call(LK_resetActAnimeUpper, this, 2, -1.0f);
                gabi::call(LK_setActAnimeUpper, this, (u32)anm, 2, 1.25f, 0.0f, -1, 2.4f);
            } else if (*ratio.get() < 1.0f) {
                cLib_chaseF(ratio.get(), 1.0f, 0.41666666f);
                lk_pbSetRatio(m_pbCalc[1], 2, *ratio.get());
            }
            u16 under = gabi::load<u16>(gabi::ea(this) + 0x5848); /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */
            if (under == 0x22 /* dRes_INDEX_LKANM_BCK_ATNLS_e */ || under == 0x24 /* dRes_INDEX_LKANM_BCK_ATNRS_e */) {
                /* mAnmRatioUpper[UPPER_MOVE2_e].getAnmTransform()->setFrame(mFrameCtrlUnder[UNDER_MOVE0_e].getFrame()) */
                gabi::store<f32>(LK_FIELD(u32, 0x583C), mFrameCtrlUnder[0].mFrame);
            }
        } else if (LK_upperIdx() == 0x16 || LK_upperIdx() == 0x1B /* checkUpperGuardAnime() */) {
            cLib_chaseF(ratio.get(), 0.0f, 0.41666666f);
            f32 r = *ratio.get();
            if (!(r > 0.0f)) {
                gabi::call(LK_resetActAnimeUpper, this, 2, 2.4f);
            } else {
                lk_pbSetRatio(m_pbCalc[1], 2, r);
            }
        }
    }
    u32 tg = gabi::ea(this) + 0x76F0; /* mCyl's tg flags */
    if (LK_checkPlayerGuardV()) {
        gabi::store<u32>(tg, gabi::load<u32>(tg) | 1); /* OnTgShield() */
    } else {
        gabi::store<u32>(tg, gabi::load<u32>(tg) & ~1u); /* OffTgShield() */
    }
}
VERIFY(0x023FB8A0, &daPy_lk_c::setShieldGuard);

/* 023FBC7C */
BOOL daPy_lk_c::checkAtHitEnemy(dCcD_GObjInf* obj) {
    WWHD_FUNC(0x023FBC7C, BOOL, this, obj);
    if (gabi::call<BOOL>(0x025160DC /* dCcD_GObjInf::ChkAtHit */, obj)) {
        u32 ac = gabi::call<u32>(0x02515BBC /* GetAtHitAc */, gabi::ea(obj) + 0x50);
        if (ac != 0 && gabi::load<u8>(ac + 0x2DA) == 2 /* fopAc_ENEMY_e */) {
            return TRUE;
        }
    }
    return FALSE;
}
VERIFY(0x023FBC7C, &daPy_lk_c::checkAtHitEnemy);

/* J3DAnmBase::getFrameMax (HD: virtual, vtable at +4, slot 0x14) */
static inline s32 anm_getFrameMax(u32 anm) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14), anm); }

/* 023FBCEC */
void daPy_lk_c::playTextureAnime() {
    WWHD_FUNC(0x023FBCEC, void, this);
    u32 me = gabi::ea(this);
    u16 f4 = gabi::load<u16>(me + 0x65D4); /* m_tex_anm_heap.field_0x4 */
    u32 mf;
    if (f4 != 0xFFFF) {
        if (mCurProc == 0xA9 /* daPyProc_DEMO_TOOL_e */) {
            goto underFrame;
        }
        if (gabi::load<u16>(me + 0x65D6) == 0xFFFE && checkMabaAnimeBtp(f4)) {
            u16 m = m3530;
            if (m != 0) {
                m = (u16)(m + 1);
                m3530 = m;
                if ((s32)m >= anm_getFrameMax(mpAnmTexPatternData)) {
                    m3530 = 0;
                    m3532 = 0;
                    goto check;
                }
            } else if (gabi::call<f64>(0x02019788 /* cM_rnd */) < (f64)0.012f) {
                u16 n = (u16)(m3530 + 1);
                m3530 = n;
                m3532 = n;
                goto check;
            }
            m3532 = m3530;
        } else {
            m3530 = (u16)(m3530 + 1);
            m3532 = (u16)(m3532 + 1);
        }
        goto check;
    }
    mf = mModeFlg;
    if (gabi::load<u16>(me + 0x65D2) /* m_tex_anm_heap.field_0x2 */ != 0xFFFF) {
        if (mf & 0x40000 /* ModeFlg_SWIM */) {
            m3530 = 0;
            m3532 = 0;
        } else {
            u16 v = (u16)gabi::ftoi(mFrameCtrlUpper[2].mFrame);
            m3530 = v;
            m3532 = v;
        }
        goto check;
    }
    if ((mf & 0x400) || m34C3 == 9 || m34C3 == 10) {
    underFrame:
        u16 v = (u16)gabi::ftoi(mFrameCtrlUnder[0].mFrame);
        m3530 = v;
        m3532 = v;
        goto check;
    }
    if (mf & 0x100) {
        u16 m = m3530;
        if (m != 0) {
            m = (u16)(m + 1);
            m3530 = m;
            if ((s32)m >= anm_getFrameMax(mpAnmTexPatternData)) {
                u16 idx = gabi::load<u16>(me + 0x65E0);
                m3530 = 0;
                if (idx != 0x15D) {
                    goto same;
                }
                goto scroll;
            }
        } else if (gabi::call<f64>(0x02019788 /* cM_rnd */) < (f64)0.012f) {
            m3530 = (u16)(m3530 + 1);
        }
        if (gabi::load<u16>(me + 0x65E0) /* m_tex_scroll_heap.mIdx */ == 0x15D /* dRes_INDEX_LKANM_BTK_TABEKOBE_e */) {
        scroll:
            u16 n = (u16)(m3532 + 1);
            m3532 = n;
            if (!((s32)n < anm_getFrameMax(mpTexScrollResData))) {
                m3532 = 0;
            }
        } else {
        same:
            m3532 = m3530;
        }
    }
check:
    s32 max;
    if ((max = anm_getFrameMax(mpAnmTexPatternData), (s32)(u16)m3530 >= max)) {
        if (anm_getFrameMax(mpAnmTexPatternData) == 0) {
            m3530 = 0;
        } else {
            m3530 = (u16)anm_getFrameMax(mpAnmTexPatternData);
        }
    }
    if ((max = anm_getFrameMax(mpTexScrollResData), (s32)(u16)m3532 >= max)) {
        if (anm_getFrameMax(mpTexScrollResData) == 0) {
            m3532 = 0;
        } else {
            m3532 = (u16)anm_getFrameMax(mpTexScrollResData);
        }
    }
}
VERIFY(0x023FBCEC, &daPy_lk_c::playTextureAnime);

/* the joint count of the model data's joint tree (027F3F94 resolves the self-relative tree pointer) */
static inline s32 lk_jointNum(u32 me) {
    u32 res = gabi::call<u32>(0x027F3F94, gabi::load<u32>(me + 0x444) /* mpCLModelData */);
    return gabi::load<u16>(res + 8);
}
/* mDoExt_AnmRatioPack (HD 0x10: count at +8, weights at +0xC) */
static inline void lk_ratioFill(u32 pack, f32 v) {
    for (s32 j = 0; j < gabi::load<s32>(pack + 8); j++) {
        gabi::store<f32>(gabi::load<u32>(pack + 0xC) + j * 4, v);
    }
}
static inline void lk_ratioSet(u32 pack, s32 i, f32 v) { gabi::store<f32>(gabi::load<u32>(pack + 0xC) + i * 4, v); }
/* the per-animation joint flag updates of a blend calc (pb + 0x84: 0x68 per animation) */
static inline void lk_pbUpdateFlags(u32 me, u32 pb, s32 k, u32 a0, u32 b0, u32 a1, u32 b1) {
    gabi::call(0x025E3A6C, pb, k);
    u32 res = gabi::call<u32>(0x027F3F94, gabi::load<u32>(me + 0x444));
    if (gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + k * 0x10 + 4) != 0) {
        gabi::call(0x027E01E0, gabi::load<u32>(pb + 0x84) + k * 0x68, res, a0, b0);
    }
    res = gabi::call<u32>(0x027F3F94, gabi::load<u32>(me + 0x444));
    if (gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + k * 0x10 + 4) != 0) {
        gabi::call(0x027E01E0, gabi::load<u32>(pb + 0x84) + k * 0x68, res, a1, b1);
    }
}

/* 023FC06C HD-only (no GameCube function; name descriptive): sets the under/upper blend calcs on the model
 * data's first joints and recomputes the per-joint animation weights (mAnmRatioUnder/Upper) from the
 * blend ratios, then refreshes the joint flags of each animation */
void daPy_lk_c::setAnimeRatioHD() {
    WWHD_FUNC(0x023FC06C, void, this);
    u32 me = gabi::ea(this);
    u32 md = gabi::load<u32>(me + 0x444);
    gabi::store<u32>(gabi::load<u32>(md + 8) + 0x14, m_pbCalc[0]); /* joint 0's mtx calc */
    md = gabi::load<u32>(me + 0x444);
    u32 n = gabi::load<u32>(md + 4);
    u32 pb1 = m_pbCalc[1];
    u32 jt = gabi::load<u32>(md + 8);
    if (n > 2) {
        jt += 0x38;
    }
    gabi::store<u32>(jt + 0x14, pb1);
    u32 pb0 = m_pbCalc[0];
    for (s32 i = 0; i < lk_jointNum(me);) {
        u32 t = gabi::load<u32>(pb0 + 0x7C);
        u32 anm1 = gabi::load<u32>(t + 0x14);
        f32 w = gabi::load<f32>(gabi::load<u32>(t + 0x1C) + i * 4);
        if (anm1 != 0) {
            lk_ratioSet(me + 0x57F8 /* mAnmRatioUnder[0] */, i, 1.0f - w);
            lk_ratioSet(me + 0x5808 /* mAnmRatioUnder[1] */, i, w);
        } else {
            lk_ratioFill(me + 0x57F8, 1.0f);
            lk_ratioFill(me + 0x5808, 0.0f);
        }
        i++;
    }
    for (s32 k = 0; k < 2; k++) {
        lk_pbUpdateFlags(me, pb0, k, 0, 0, 2, 3);
    }
    pb1 = m_pbCalc[1];
    for (s32 i = 0; i < lk_jointNum(me); i++) {
        u32 mdl = gabi::load<u32>(gabi::load<u32>(me + 0x448) + 0xAC); /* mpCLModel->getModelData() */
        u32 jn = gabi::load<u32>(mdl + 4);
        u32 j = gabi::load<u32>(mdl + 8);
        if ((u32)(u16)i < jn) {
            j += (u16)i * 0x1C;
        }
        if (gabi::load<u32>(j + 8) != 0) {
            gabi::call(0x023D7A0C /* daPy_lk_c::jointCB0 */, this, i);
        }
        u32 t = gabi::load<u32>(pb1 + 0x7C);
        u32 anm2 = gabi::load<u32>(t + 0x24);
        f32 w2 = gabi::load<f32>(gabi::load<u32>(t + 0x2C) + i * 4);
        f32 w1 = gabi::load<f32>(gabi::load<u32>(t + 0x1C) + i * 4);
        u32 anm1 = gabi::load<u32>(t + 0x14);
        if (anm2 == 0) {
            w2 = 0.0f;
        }
        f32 a = (1.0f - w2) * w1;
        if (anm1 == 0) {
            a = 0.0f;
        }
        f32 base = 1.0f - (w2 + a);
        lk_ratioSet(me + 0x5818 /* mAnmRatioUpper[0] */, i, base);
        lk_ratioSet(me + 0x5828 /* mAnmRatioUpper[1] */, i, a);
        lk_ratioSet(me + 0x5838 /* mAnmRatioUpper[2] */, i, w2);
    }
    for (s32 k = 0; k < 3; k++) {
        lk_pbUpdateFlags(me, pb1, k, 0, 3, 2, 0);
    }
    gabi::store<u32>(gabi::load<u32>(gabi::load<u32>(me + 0x448) + 0x2C) + 0x30, m_pbCalc[0]);
    gabi::store<u16>(gabi::load<u32>(gabi::load<u32>(me + 0x448) + 0x2C) + 0x2E, 0);
}
VERIFY(0x023FC06C, &daPy_lk_c::setAnimeRatioHD);

/* 023FC568 */
void daPy_lk_c::setBeltConveyerPower() {
    WWHD_FUNC(0x023FC568, void, this);
    const u32 zero = 0x101FFBA8; /* cXyz::Zero */
    if (!gabi::call<bool>(LK_checkHeavyStateOn, this) && !dComIfGp_event_runCheck_l() && LK_demoType == 0 &&
        !(mNoResetFlg0 & 0xA0000000 /* daPyFlg0_UNK20000000 | daPyFlg0_UNK80000000 */) &&
        ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) || (mModeFlg & 0x40000 /* ModeFlg_SWIM */))) {
        gabi::Local<cXyz> vec;
        gabi::Local<be<s32>> spd;
        u32 gnd = gabi::ea(this) + 0x8F4; /* mAcch.m_gnd */
        if (gabi::call<BOOL>(0x02008254 /* cBgS::ChkPolySafe */, dComIfG_Bgsp(), gnd) &&
            gabi::call<BOOL>(0x025AB184 /* dPath_GetPolyRoomPathVec */, gnd, vec.get(), spd.get())) {
            gabi::Local<cXyz> unused;
            gabi::call(0x0201B3C0 /* cXyz::normalizeZP (result discarded) */, vec.get(), unused.get());
            PSVECScale(vec, vec, (f32)(s32)*spd.get());
        } else {
            u32 z = gabi::load<u32>(zero + 8);
            u32 y = gabi::load<u32>(zero + 4);
            gabi::store<u32>(gabi::ea(vec.get()) + 0, gabi::load<u32>(zero + 0));
            gabi::store<u32>(gabi::ea(vec.get()) + 4, y);
            *spd.get() = 0;
            gabi::store<u32>(gabi::ea(vec.get()) + 8, z);
            PSVECScale(vec, vec, 0.0f);
        }
        gabi::Local<cXyz> xz1;
        xz1->x = m36B8.x;
        xz1->y = 0.0f;
        xz1->z = m36B8.z;
        f64 a = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz1.get());
        gabi::Local<cXyz> xz2;
        xz2->x = vec->x;
        xz2->y = 0.0f;
        xz2->z = vec->z;
        f64 b = gabi::call<f64>(0x028E8DD0, xz2.get());
        f32 d = (f32)(b - a);
        f32 step = d >= 0.0f ? 1.0f : 3.0f; /* fsel */
        gabi::call(0x0200EF78 /* cLib_addCalcPosXZ */, &m36B8, vec.get(), 0.5f, step, 0.5f);
    } else {
        lk_copy12(gabi::ea(&m36B8), gabi::at<cXyz>(zero));
    }
}
VERIFY(0x023FC568, &daPy_lk_c::setBeltConveyerPower);

/* 023FC7F0 */
void daPy_lk_c::setWindAtPower() {
    WWHD_FUNC(0x023FC7F0, void, this);
    u32 me = gabi::ea(this);
    void* wind = gabi::at<void>(me + 0x778C); /* mWindCyl */
    if (gabi::call<BOOL>(0x025162A4 /* ChkTgHit */, wind)) {
        u32 g = gabi::call<u32>(0x02516360 /* GetTgHitGObj */, wind);
        m34BA = (g != 0 && gabi::load<u8>(g + 0x6F) == 1) ? 1 : 0;
    }
    gabi::Local<cXyz> vec;
    f32 step;
    bool zeroPath = true;
    if (gabi::call<BOOL>(0x025162A4, wind) && !(mDamageWaitTimer != 0 && m34BA != 0) && !dComIfGp_event_runCheck_l() &&
        LK_demoType == 0 && mCurProc != 0x93 /* daPyProc_FAN_GLIDE_e */) {
        zeroPath = false;
        vec->y = gabi::load<f32>(me + 0x7850); /* *mWindCyl.GetTgRVecP() */
        f32 z = gabi::load<f32>(me + 0x7854);
        f32 x = gabi::load<f32>(me + 0x784C);
        vec->x = x;
        vec->z = z;
        gabi::Local<cXyz> xz;
        xz->y = 0.0f;
        xz->x = x;
        xz->z = z;
        f64 len = gabi::call<f64>(0x028F4384, gabi::call<f64>(0x028E8DD0, xz.get()));
        f32 lim;
        if (m34BA != 0) {
            lim = 27.0f;
            step = 18.0f;
        } else {
            lim = 30.0f;
            step = 1.0f;
        }
        if (len < 1.0) {
            gabi::Local<cXyz> d;
            gabi::Local<cXyz> d2;
            cXyz_mi(&current.pos, d, gabi::at<cXyz>(me + 0x7858) /* GetTgHitPosP() */);
            cXyz_ml(d, d2, lim);
            f32 y2 = d2->y;
            f32 z2 = d2->z;
            vec->y = y2;
            f32 x2 = d2->x;
            vec->x = x2;
            vec->z = z2;
            xz->y = 0.0f;
            xz->x = x2;
            xz->z = z2;
            len = gabi::call<f64>(0x028F4384, gabi::call<f64>(0x028E8DD0, xz.get()));
        }
        if (len > (f64)lim) {
            PSVECScale(vec, vec, (f32)((f64)lim / len));
        }
    }
    if (zeroPath) {
        lk_copy12(gabi::ea(vec.get()), gabi::at<cXyz>(0x101FFBA8) /* cXyz::Zero */);
        step = m34BA != 0 ? 1.0f : 3.0f;
    }
    if (mCurProc == 0x93) {
        return;
    }
    if (dComIfGp_event_runCheck_l() || LK_demoType != 0) {
        m373C.x = 0.0f;
        m373C.z = 0.0f;
    } else {
        gabi::call(0x0200EF78 /* cLib_addCalcPosXZ */, &m373C, vec.get(), 0.5f, step, 0.5f);
    }
    if (gabi::call<bool>(LK_checkHeavyStateOn, this) || dComIfGp_event_runCheck_l() || LK_demoType != 0) {
        /* HD: the wind push is cleared only when the heavy state still holds */
        if (gabi::call<bool>(LK_checkHeavyStateOn, this)) {
            m3730.z = 0.0f;
            m3730.x = 0.0f;
        }
    } else {
        gabi::call(0x0200EF78, &m3730, vec.get(), 0.5f, step, 0.5f);
    }
}
VERIFY(0x023FC7F0, &daPy_lk_c::setWindAtPower);

/* f32 fused multiply-add with a double operand as the recompiled original computes it (a*c+b, rounded once to single) */
static inline f32 lk_fmaddsD(f64 a, f64 c, f64 b) { return (f32)(a * c + b); }

/* 023FCB9C */
void daPy_lk_c::posMoveFromFootPos() {
    WWHD_FUNC(0x023FCB9C, void, this);
    u32 me = gabi::ea(this);
    if (gabi::load<u8>(m_old_fdata) /* m_old_fdata->getOldFrameFlg() */ == 0) {
        speedF = 0.0f;
        /* mFootData[i].field_0x018 / field_0x00C = rtoe_pos_offset / rheel_pos_offset (x mirrored for the left foot) */
        gabi::store<f32>(me + 0x7520, 14.05f);
        gabi::store<f32>(me + 0x7408, -14.05f);
        gabi::store<f32>(me + 0x7524, 0.0f);
        gabi::store<f32>(me + 0x740C, 0.0f);
        gabi::store<f32>(me + 0x7410, 5.02f);
        gabi::store<f32>(me + 0x73FC, -10.85f);
        gabi::store<f32>(me + 0x7528, 5.02f);
        gabi::store<f32>(me + 0x7514, 10.85f);
        gabi::store<f32>(me + 0x7518, 0.0f);
        gabi::store<f32>(me + 0x751C, -6.52f);
        gabi::store<f32>(me + 0x7400, 0.0f);
        gabi::store<f32>(me + 0x7404, -6.52f);
        m34BC = 2;
        return;
    }
    const u32 stk = 0x1048D0CC; /* mDoMtx_stack_c::now */
    const u32 toePos = 0x101CEB9C, heelPos = 0x101CEB90; /* l_toe_pos, l_heel_pos */
    gabi::Local<cXyz> toe[2];
    gabi::Local<cXyz> heel[2];
    gabi::Local<cXyz> mid[2];
    gabi::Local<cXyz> tmp;
    gabi::call(0x028E9108 /* PSMTXConcat */, &m37B4, lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0x22 /* CL_JNT_LFOOT_JNT_e */), stk);
    gabi::call(0x028E8F64 /* PSMTXMultVec */, stk, toePos, toe[1].get());
    gabi::call(0x028E8F64, stk, heelPos, heel[1].get());
    gabi::call(0x028E9108, &m37B4, lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0x27 /* CL_JNT_RFOOT_JNT_e */), stk);
    gabi::call(0x028E8F64, stk, toePos, toe[0].get());
    gabi::call(0x028E8F64, stk, heelPos, heel[0].get());
    gabi::call(0x028E9108, &m37B4, lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0x1E /* CL_JNT_WAIST_JNT_e */), stk);
    u16 a = (u16)m34E0;
    f32 sn = cM_ssin(a);
    f32 cs = cM_scos(a);
    f32 h[2];
    for (int i = 0; i < 2; i++) {
        cXyz_pl_l(gabi::ea(heel[i].get()), tmp, gabi::ea(toe[i].get()));
        lk_copy12(gabi::ea(mid[i].get()), tmp);
        PSVECScale(mid[i], mid[i], 0.5f);
        f32 z = mid[i]->z;
        f32 y = mid[i]->y;
        f32 m13 = gabi::load<f32>(stk + 0x1C);
        f32 m23 = gabi::load<f32>(stk + 0x2C);
        f32 t = gabi::fmadds(y - m13, cs, m13);
        h[i] = gabi::fmadds(z - m23, sn, t);
    }
    u32 idx = !(h[0] < h[1]);
    m34BC = idx;
    gabi::Local<cXyz> d;
    cXyz_mi(toe[idx], d, gabi::at<cXyz>(me + 0x73F0 + idx * 0x118 + 0x18) /* mFootData[idx].field_0x018 */);
    gabi::Local<cXyz> xz;
    xz->x = d->x;
    xz->z = d->z;
    xz->y = 0.0f;
    f64 step = gabi::call<f64>(0x028F4384 /* std::sqrtf */, gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get()));
    f32 r = m3598;
    if (r < 1.0f && std::fabs((f32)(m35B4 - mStickDistance)) < 0.2f) {
        step = lk_fmaddsD(step, 0.3f, m359C * 0.7f);
    }
    f32 ns = mNormalSpeed;
    f32 v = ns * (1.0f - r);
    if (ns < 0.0f) {
        v = (f32)(-(step * (f64)r - (f64)v)); /* fnmsubs */
    } else {
        v = lk_fmaddsD(step, r, v);
    }
    if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && gabi::call<BOOL>(0x02008254 /* cBgS::ChkPolySafe */, dComIfG_Bgsp(), me + 0x8F4) &&
        m3580 != 8) {
        s32 ga = gabi::call<s32>(0x023E7D6C /* getGroundAngle */, this, me + 0x8F4, (s32)current.angle.y);
        v = v * cM_scos((u16)ga);
        if (ga < 0) {
            v = v * 0.85f;
        }
    } else {
        v = v * cM_scos(0);
    }
    u32 z0, x0, y0;
    if (!(std::fabs(v) < 0.05f)) {
        s32 proc = mCurProc;
        speedF = v;
        if (proc == 0x37 /* daPyProc_SWIM_MOVE_e */) {
            f32 fr = mFrameCtrlUnder[0].mFrame / (f32)mFrameCtrlUnder[0].mEnd;
            f32 c = lk_cosOfRad(fr * 3.1415927f);
            f32 sp = speedF;
            f64 rate = gabi::call<f64>(0x023F8858 /* getSwimTimerRate */, this);
            f32 den = lk_fmaddsD(rate, 0.35f, 1.0f);
            f32 num = gabi::fmadds(sp * std::fabs(c), 0.4f, sp * 0.6f);
            f32 k = num / den;
            u16 ay = (u16)current.angle.y;
            speed.x = k * cM_ssin(ay);
            speed.z = k * cM_scos(ay);
        } else if (proc == 0x56 /* daPyProc_CUT_ROLL_e */ && dComIfGp_event_runCheck_l() && LK_demoType == 0 &&
                   LK_demoMode != 0x2B /* DEMO_CUT_ROLL_e */) {
            speed.x = 0.0f;
            speed.z = 0.0f;
        } else {
            if (proc == 0x56) {
                v = speedF;
            }
            u16 ay = (u16)current.angle.y;
            speed.x = v * cM_ssin(ay);
            speed.z = speedF * cM_scos(ay);
        }
    } else {
        speedF = 0.0f;
        speed.x = 0.0f;
        speed.z = 0.0f;
    }
    z0 = gabi::load<u32>(me + 0x72B0);
    x0 = gabi::load<u32>(me + 0x72A8);
    y0 = gabi::load<u32>(me + 0x72AC);
    gabi::store<u32>(me + 0x72B4, x0); /* m36AC = m36A0 */
    gabi::store<u32>(me + 0x72B8, y0);
    gabi::store<u32>(me + 0x72BC, z0);
    if (!(mModeFlg & 0x11612832) && !dComIfGp_event_runCheck_l() && LK_demoType == 0 && !gabi::call<bool>(LK_checkHeavyStateOn, this) &&
        mCurrAttributeCode == 0xF /* dBgS_Attr_ICE_e */ && !(mNoResetFlg0 & 0xA0000000) && !(mAcch.m_flags & dBgS_Acch::WALL_HIT) &&
        (mAcch.m_flags & dBgS_Acch::GROUND_HIT)) {
        gabi::call(0x0200ECD4 /* cLib_addCalc */, &m36A0.x, 0.0f, 0.03f, 100.0f, 0.5f);
        gabi::call(0x0200ECD4, &m36A0.z, 0.0f, 0.03f, 100.0f, 0.5f);
        gabi::Local<cXyz> t1;
        gabi::Local<cXyz> t2;
        cXyz_mi(&mOldSpeed, t1, &speed);
        cXyz_ml(t1, t2, 0.75f);
        gabi::call(0x028E8D88 /* PSVECAdd */, &m36A0, t2.get(), &m36A0);
        cXyz_mi(&mOldSpeed, t2, &speed);
        cXyz_ml(t2, t1, 0.25f);
        gabi::call(0x028E8D88, &speed, t1.get(), &speed);
        m36A0.y = 0.0f;
    } else {
        m36A0.x = 0.0f;
        m36A0.z = 0.0f;
        m36A0.y = 0.0f;
    }
    setBeltConveyerPower();
    setWindAtPower();
    f32 oldY = speed.y;
    f32 sy;
    if (gabi::call<bool>(LK_checkHeavyStateOn, this)) {
        f32 lim = maxFallSpeed * 1.5f;
        sy = gabi::fmadds(gravity, 2.25f, speed.y);
        speed.y = sy;
        if (sy < lim) {
            sy = lim;
            speed.y = sy;
        }
    } else {
        sy = speed.y + gravity;
        f32 lim = maxFallSpeed;
        speed.y = sy;
        if (sy < lim) {
            sy = lim;
            speed.y = sy;
        }
    }
    if (!(sy > 0.0f) && oldY > 0.0f) {
        m35F0 = current.pos.y;
    }
    gabi::call(0x028E8D88 /* PSVECAdd */, &current.pos, &speed, &current.pos);
    for (int i = 0; i < 2; i++) {
        u32 fd = me + 0x73F0 + i * 0x118; /* mFootData[i] */
        lk_copy12(fd + 0x18, toe[i]);
        lk_copy12(fd + 0xC, heel[i]);
    }
    m359C = (f32)step;
}
VERIFY(0x023FCB9C, &daPy_lk_c::posMoveFromFootPos);

/* 023FD438 */
BOOL daPy_lk_c::checkNoCollisionCorret() {
    WWHD_FUNC(0x023FD438, BOOL, this);
    if ((mModeFlg & 0x10402820) || LK_demoType == 1 /* TYPE_TOOL_e */ || LK_demoMode == 0xA /* DEMO_OPEN_TREASURE_e */ ||
        LK_demoMode == 0x1E /* DEMO_UNK_030_e */ || (resetFlg0() & 0x1000 /* daPyRFlg0_CRAWL_AUTO_MOVE */) ||
        gabi::load<u16>(gabi::ea(this) + 0xF8) == 3 /* eventInfo.checkCommandDoor() */) {
        return TRUE;
    }
    u32 proc = (u32)mCurProc;
    if (proc == 0x12 /* VERTICAL_JUMP */ || proc == 0x2A /* CRAWL_END */ || proc == 0x85 /* HOOKSHOT_FLY */ ||
        proc == 0x97 /* VOMIT_WAIT */ || proc == 0xC1 /* DEMO_DOOR_OPEN */) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023FD438, &daPy_lk_c::checkNoCollisionCorret);

/* dComIfGp_setNextStage(name, point, roomNo, layer, lastSpeed, mode, wipe?, ?) (0252012C) */
static inline void lk_setNextStage(u32 name, s32 point, s32 room, u32 mode, s32 a8, s32 a9) {
    gabi::call(0x0252012C, name, point, room, -1, 0.0f, mode, a8, a9);
}

/* 023FD4E4 */
BOOL daPy_lk_c::startRestartRoom(u32 mode, int eventInfoIdx, f32 damage, int point) {
    WWHD_FUNC(0x023FD4E4, BOOL, this, mode, eventInfoIdx, damage, point);
    if (gabi::load<s8>(dComIfGp_ea() + 0x514C) != 0 /* HD */ || (mNoResetFlg0 & 0x4000 /* daPyFlg0_UNK4000 */)) {
        return FALSE;
    }
    if (point == 0 && !dComIfGp_event_compulsory_l(this)) {
        return FALSE;
    }
    LK_demoType = 3; /* mDemo.setOriginalDemoType() */
    if (point == 0) {
        LK_demoMode = 1; /* DEMO_N_WAIT_e */
    }
    mNoResetFlg0 = mNoResetFlg0 | 0x4000;
    LK_changePlayer();
    if (gabi::load<u8>(dComIfGp_ea() + 0x5CEA) /* dComIfGp_getMiniGameType() */ == 1) {
        lk_setNextStage(0x10035950 /* "sea" */, 1, 0x30 /* dIsleRoom_BoatingCourse_e */, 0, 1, 0);
        gabi::call(0x025E1988 /* seStartSystem */, 0x8B3 /* JA_SE_FORCE_BACK */);
        gabi::store<s32>(0x101D5F1C, 3); /* daNpc_Sarace_c::ship_race_result */
        mTinkleShieldTimer = 0;
        return TRUE;
    }
    u32 stType = (gabi::load<u32>(dComIfGp_getStageStagInfo_l() + 0xC) >> 16) & 7; /* dStage_stagInfo_GetSTType() */
    setDamagePoint(damage);
    mTinkleShieldTimer = 0;
    if (stType == 7 /* dStageType_SEA_e */ && !gabi::call<BOOL>(0x025B8B94 /* dSv_event_c::isEventBit */, dComIfGs_base_l() + 0x644, 0x2A08 /* RODE_KORL */)) {
        s32 room = current.roomNo;
        if ((room == 0xB || room == 0x2C) && gabi::call<BOOL>(0x025C123C /* dStage_chkPlayerId */, 0x80, room)) {
            u32 name = dComIfGp_ea() + 0x5134;
            lk_setNextStage(name, 0x80, (s32)current.roomNo, mode, 1, 0);
            u32 prm = setParamData(-1, 0, eventInfoIdx, 0);
            gabi::store<u32>(dComIfGs_base_l() + 0x116C, prm); /* dComIfGs_setRestartRoomParam() */
            gabi::call(0x025E1988, 0x8B3);
            return TRUE;
        }
    }
    s32 pt;
    if ((mNoResetFlg0 & 0x10 /* daPyFlg0_DEKU_SP_RETURN_FLG */) && mCurProc != 0xB2 /* DEMO_DEAD */) {
        pt = mDekuSpRestartPoint;
        if (pt != 0xFF) {
            u32 name = dComIfGp_ea() + 0x5134;
            lk_setNextStage(name, (s16)pt, 0x29, mode, 1, 0);
            goto param;
        }
    } else {
        pt = mRestartPoint;
        if (pt != 0xFF && gabi::call<BOOL>(0x0252174C /* dComIfGs_checkSeaLandingEvent */, (s32)current.roomNo)) {
            u32 deku = mNoResetFlg0 & 0x10;
            u32 name = dComIfGp_ea() + 0x5134;
            if (deku) {
                lk_setNextStage(name, (s16)pt, 0x29, mode, 1, 0);
            } else {
                lk_setNextStage(name, (s16)pt, (s32)current.roomNo, mode, 1, 0);
            }
            goto param;
        }
    }
    if (stType == 7) {
        u32 pos;
        if (dComIfGp_getShipActor_l() != 0) {
            pos = dComIfGp_getShipActor_l() + 0x314;
        } else {
            pos = gabi::ea(&current.pos);
        }
        s32 qx = gabi::ftoi((gabi::load<f32>(pos + 0) + 350000.0f) * 1.9999999e-05f);
        s32 qz = gabi::ftoi((gabi::load<f32>(pos + 8) + 350000.0f) * 1.9999999e-05f);
        if (qx < 0) {
            qx = 0;
        } else if (qx > 13) {
            qx = 13;
        }
        if (qz < 0) {
            qz = 0;
        } else if (qz > 13) {
            qz = 13;
        }
        s32 sector = (qz >> 1) * 7 + (qx >> 1);
        s32 idx = (sector << 2) | (qx & 1) | ((qz & 1) << 1);
        gabi::call(0x025C3748 /* dStage_changeScene */, idx, 0.0f, mode, -1);
    } else {
        if (mCurProc == 0xB2) {
            gabi::call(0x025C3748, 0, 0.0f, mode, -1);
        } else {
            u32 prm = setParamData((s8)gabi::load<u8>(dComIfGs_base_l() + 0x1148) /* dComIfGs_getRestartRoomNo() */, 0, eventInfoIdx, 0);
            gabi::call(0x025C3D68 /* dStage_restartRoom */, prm, mode);
            gabi::call(0x025E1988, 0x8B3);
        }
        return TRUE;
    }
param:
    if (mCurProc != 0xB2) {
        u32 prm = setParamData(-1, 0, eventInfoIdx, 0);
        gabi::store<u32>(dComIfGs_base_l() + 0x116C, prm);
        gabi::call(0x025E1988, 0x8B3);
    }
    return TRUE;
}
VERIFY(0x023FD4E4, &daPy_lk_c::startRestartRoom);

/* 023FDA70 */
void daPy_lk_c::posMove() {
    WWHD_FUNC(0x023FDA70, void, this);
    u32 me = gabi::ea(this);
    gabi::Local<cXyz> start; /* m3700 before the move */
    f32 sz = m3700.z;
    u32 oldFrame = m_old_fdata;
    u32 pb0 = m_pbCalc[0];
    f32 sx = m3700.x;
    f32 sy = m3700.y;
    start->z = sz;
    start->x = sx;
    start->y = sy;
    u32 ot = gabi::load<u32>(oldFrame + 0x1C); /* m_old_fdata->getOldFrameTransInfo(0) */
    setAnimeRatioHD();
    gabi::call(0x025E410C /* HD: the under blend calc */, pb0, mpCLModel.get());
    u32 bt = gabi::load<u32>(pb0 + 0xA4); /* the blended root transform */
    u16 ang = (u16)shape_angle.y;
    f32 tz = gabi::load<f32>(bt + 0x18);
    f32 ty = gabi::load<f32>(bt + 0x14);
    f32 cs = cM_scos(ang);
    f32 tx = gabi::load<f32>(bt + 0x10);
    f32 sn = cM_ssin(ang);
    u8 m = m34C2;
    if (m == 11) {
        gabi::Local<cXyz> d;
        cXyz_mi(&current.pos, d, &old.pos);
        f32 dz = d->z;
        f32 dx = d->x;
        f32 rx = gabi::fmsubs(cs, dx, sn * dz);
        f32 ox = gabi::load<f32>(ot + 0x14) - rx;
        f32 rz = gabi::fmadds(cs, dz, sn * dx);
        f32 oy = gabi::load<f32>(ot + 0x18) - d->y;
        gabi::store<f32>(ot + 0x14, ox);
        gabi::store<f32>(ot + 0x18, oy);
        gabi::store<f32>(ot + 0x1C, gabi::load<f32>(ot + 0x1C) - rz);
    } else if (m == 10) {
        f32 dx = tx - gabi::load<f32>(ot + 0x14);
        f32 dy = ty - gabi::load<f32>(ot + 0x18);
        f32 dz = tz - gabi::load<f32>(ot + 0x1C);
        gabi::Local<cXyz> sv;
        sv->x = gabi::fmadds(sn, dz, cs * dx);
        sv->y = dy;
        sv->z = gabi::fmsubs(cs, dz, sn * dx);
        gabi::call(0x028E8DAC /* PSVECSubtract */, &current.pos, sv.get(), &current.pos);
        gabi::call(0x028E8DAC, &old.pos, sv.get(), &old.pos);
        gabi::store<f32>(ot + 0x1C, tz);
        gabi::store<f32>(ot + 0x14, tx);
        gabi::store<f32>(ot + 0x18, ty);
    } else if (m == 2 || m == 6) {
        gabi::store<f32>(ot + 0x14, tx);
        gabi::store<f32>(ot + 0x1C, tz);
        if (m34C2 == 6) {
            gabi::store<f32>(ot + 0x18, ty);
        }
    } else if (m == 4) {
        start->y = ty;
        start->x = 0.0f;
        start->z = 0.0f;
        gabi::store<f32>(ot + 0x14, 0.0f);
        gabi::store<f32>(ot + 0x1C, 0.0f);
        gabi::store<f32>(ot + 0x18, ty);
        m34C2 = 5;
        m35E0 = ty;
    } else if (m == 7 || m == 3) {
        start->x = tx;
        start->y = ty;
        start->z = tz;
        m34C2 = m == 7 ? 5 : 1;
    }
    m3700.y = ty;
    m3700.z = tz;
    m3700.x = tx;
    posMoveFromFootPos();
    if (!checkNoCollisionCorret()) {
        gabi::call(0x028E8D88 /* PSVECAdd */, &current.pos, me + 0x7620 /* mStts.GetCCMoveP() */, &current.pos);
        if (!dComIfGp_event_runCheck_l() && LK_demoType == 0) {
            if (!(noResetFlg1() & 0x10000000 /* daPyFlg1_UNK10000000 */) || !gabi::call<bool>(LK_checkHeavyStateOn, this)) {
                u16 a = (u16)m3640;
                f32 k = m3644;
                current.pos.x = gabi::fmadds(k, cM_ssin(a), current.pos.x);
                current.pos.z = gabi::fmadds(k, cM_scos(a), current.pos.z);
            }
            u32 whirl = 0;
            u32 wid = mWhirlId;
            if (wid != 0xFFFFFFFF) {
                gabi::Local<be<u32>> id;
                *id.get() = wid;
                whirl = gabi::call<u32>(0x025D5218 /* fopAcIt_Judge */, 0x025E1234 /* fpcSch_JudgeByID */, id.get());
            }
            if (whirl != 0) {
                gabi::Local<cXyz> d;
                cXyz_mi(&current.pos, d, gabi::at<cXyz>(whirl + 0x314));
                u16 a = (u16)(cM_atan2s(d->x, d->z) + 0x5000);
                cLib_chaseF(&m3610, 40.0f, 5.0f);
                f32 k = m3610;
                current.pos.x = gabi::fmadds(k, cM_ssin(a), current.pos.x);
                gabi::Local<cXyz> xz;
                xz->y = 0.0f;
                xz->x = d->x;
                xz->z = d->z;
                current.pos.z = gabi::fmadds(k, cM_scos(a), current.pos.z);
                f64 len = gabi::call<f64>(0x028F4384, gabi::call<f64>(0x028E8DD0, xz.get()));
                if (len < 500.0) {
                    startRestartRoom(5, 0xC9, -1.0f, 0);
                }
            } else {
                m3610 = 0.0f;
            }
            if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && gabi::call<BOOL>(0x02008254 /* cBgS::ChkPolySafe */, dComIfG_Bgsp(), me + 0x8F4)) {
                u16 g = (u16)gabi::call<s32>(0x023E7D6C /* getGroundAngle */, this, me + 0x8F4, 0);
                current.pos.z = gabi::fmadds(m36A0.z, cM_scos(g), current.pos.z);
                g = (u16)gabi::call<s32>(0x023E7D6C, this, me + 0x8F4, 0x4000);
                current.pos.x = gabi::fmadds(m36A0.x, cM_scos(g), current.pos.x);
            }
            gabi::call(0x028E8D88 /* PSVECAdd */, &current.pos, &m36B8, &current.pos);
            if (mCurProc != 0x93 /* daPyProc_FAN_GLIDE_e */) {
                current.pos.x = current.pos.x + m3730.x;
                current.pos.z = current.pos.z + m3730.z;
            }
        }
    } else if (!dComIfGp_event_runCheck_l() && LK_demoType == 0 && gabi::load<u32>(dComIfGp_ea() + 0x5B2C) == me &&
               mCurProc == 0x2F /* daPyProc_HANG_MOVE_e */) {
        gabi::Local<cXyz> xz;
        xz->x = gabi::load<f32>(me + 0x7620);
        xz->y = 0.0f;
        xz->z = gabi::load<f32>(me + 0x7628);
        f64 len = gabi::call<f64>(0x028F4384, gabi::call<f64>(0x028E8DD0, xz.get()));
        if (len > 1.0) {
            s16 a = cM_atan2s(gabi::load<f32>(me + 0x7620), gabi::load<f32>(me + 0x7628));
            u16 s0 = (u16)shape_angle.y;
            f32 c = cM_scos(s0);
            if ((s16)(a - s0) >= 0) {
                current.pos.x = (f32)(len * (f64)c + (f64)(f32)current.pos.x);
                f32 sv = cM_ssin((u16)shape_angle.y);
                current.pos.z = (f32)(-(len * (f64)sv - (f64)(f32)current.pos.z));
            } else {
                current.pos.x = (f32)(-(len * (f64)c - (f64)(f32)current.pos.x));
                f32 sv = cM_ssin((u16)shape_angle.y);
                current.pos.z = (f32)(len * (f64)sv + (f64)(f32)current.pos.z);
            }
        }
    }
    m = m34C2;
    gabi::store<f32>(me + 0x7620, 0.0f); /* mStts.ClrCcMove() */
    gabi::store<f32>(me + 0x7624, 0.0f);
    m3644 = 0.0f;
    gabi::store<f32>(me + 0x7628, 0.0f);
    gabi::Local<cXyz> d;
    if (m == 1 || m == 5) {
        cXyz_mi(&m3700, d, start);
    } else if (m >= 8 && m <= 9) {
        cXyz_mi(start, d, &m3700);
    } else {
        return;
    }
    f32 dx = d->x;
    f32 dz = d->z;
    current.pos.x = current.pos.x + gabi::fmadds(dz, sn, dx * cs);
    current.pos.z = current.pos.z + gabi::fmsubs(dz, cs, dx * sn);
    if (m34C2 == 5) {
        current.pos.y = current.pos.y + d->y;
    }
}
VERIFY(0x023FDA70, &daPy_lk_c::posMove);

/* dPa_control_c::mStatus (0x101EA4E4): onStatus(1) / offStatus(1) */
static inline void lk_paStatus(bool on) {
    u8 v = gabi::load<u8>(0x101EA4E4);
    gabi::store<u8>(0x101EA4E4, on ? (u8)(v | 1) : (u8)(v & ~1));
}

/* 023FE224 */
void daPy_lk_c::setWaterY() {
    WWHD_FUNC(0x023FE224, void, this);
    u32 me = gabi::ea(this);
    if (!gabi::call<BOOL>(0x0246B6A4 /* daSea_ChkArea */, (f32)current.pos.x, (f32)current.pos.z)) {
        u32 flg = mNoResetFlg0;
        f32 h = gabi::load<f32>(me + 0x9C8); /* mAcch.m_wtr.GetHeight() */
        if (mAcch.m_flags & dBgS_Acch::WATER_HIT) {
            mNoResetFlg0 = flg | 0x80;
        } else {
            mNoResetFlg0 = flg & ~0x80u;
        }
        mWaterY = h;
        lk_paStatus(false);
        return;
    }
    mNoResetFlg0 = mNoResetFlg0 | 0x80;
    f64 w = gabi::call<f64>(0x0246BA0C /* daSea_calcWave */, (f32)current.pos.x, (f32)current.pos.z);
    f32 h = gabi::load<f32>(me + 0x9C8);
    mWaterY = (f32)w;
    if ((f64)h > w) {
        mWaterY = h;
        lk_paStatus(false);
    } else {
        lk_paStatus(true);
    }
}
VERIFY(0x023FE224, &daPy_lk_c::setWaterY);

/* 023FE2F0 */
void daPy_lk_c::autoGroundHit() {
    WWHD_FUNC(0x023FE2F0, void, this);
    u32 flg = mNoResetFlg0;
    u32 was = flg & 0x80000000 /* daPyFlg0_UNK80000000 */;
    flg = flg & ~0x80000000u;
    mNoResetFlg0 = flg;
    if (mTinkleHoverTimer == 0) {
        mNoResetFlg0 = flg & ~0x1000u; /* offNoResetFlg0(daPyFlg0_HOVER_BOOTS) */
    }
    if (!dComIfGp_event_runCheck_l() && LK_demoType == 0 && mTinkleHoverTimer > 0) {
        mTinkleHoverTimer = (s16)(mTinkleHoverTimer - 1);
    }
    if (mModeFlg & 0x10452822 /* checkPlayerFly() */) {
        return;
    }
    u32 acchFlg = mAcch.m_flags;
    f32 gh = mAcch.m_ground_h;
    f32 d = gh - current.pos.y;
    if (acchFlg & dBgS_Acch::GROUND_HIT) {
        return;
    }
    if (d < 0.0f && !(d < -30.1f) && was == 0) {
        speed.y = 0.0f;
        current.pos.y = gh;
        mAcch.m_flags = mAcch.m_flags | dBgS_Acch::GROUND_HIT;
        return;
    }
    if ((mNoResetFlg0 & 0x1000) && mTinkleHoverTimer > 0 && (!dComIfGp_event_runCheck_l() || LK_demoType == 5)) {
        mNoResetFlg0 = mNoResetFlg0 | 0x80000000;
        speed.y = 0.0f;
        f32 oy = old.pos.y;
        mAcch.m_flags = mAcch.m_flags | dBgS_Acch::GROUND_HIT;
        current.pos.y = oy;
    }
}
VERIFY(0x023FE2F0, &daPy_lk_c::autoGroundHit);

/* 023FE470 */
BOOL daPy_lk_c::procLavaDamage_init() {
    WWHD_FUNC(0x023FE470, BOOL, this);
    if (mCurProc == 0x6B /* daPyProc_LAVA_DAMAGE_e */) {
        if (!(speed.y > 0.0f)) {
            speed.y = 30.0f;
        }
        gravity = -2.5f;
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0x6B);
    gabi::Local<cXyz> xz; /* current.pos.absXZ() */
    xz->x = current.pos.x;
    xz->y = 0.0f;
    xz->z = current.pos.z;
    f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get()));
    f32 k = gabi::fmsubs((f32)(1700.0 - len), 0.00058823527f, 0.25f);
    gravity = -2.5f;
    k = fsel_l(k, k, 0.0f);
    mNormalSpeed = gabi::fmadds(40.0f, k, 15.0f);
    speed.y = gabi::fmadds(36.0f, k, 32.0f);
    gabi::call(LK_setSingleMoveAnime, this, 0x61 /* ANM_LAVADAM */, 1.0f, 0.0f, -1, 5.0f);
    u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0 /* CL_JNT_LINK_ROOT_e */);
    gabi::call(0x023D457C /* makeEmitter */, &m32E4, 0x8078 /* ID_AK_SN_HIDARUMAFIRE */, mtx, &current.pos, nullptr);
    LK_voiceStart(4);
    seStartMapInfo(0x3810 /* JA_SE_LK_FALL_MAGMA */);
    current.angle.y = cM_atan2s(current.pos.x, current.pos.z);
    mDamageWaitTimer = 0x1E;
    setDamagePoint(-1.0f);
    return TRUE;
}
VERIFY(0x023FE470, &daPy_lk_c::procLavaDamage_init);

/* 023FE62C */
void daPy_lk_c::dProcLavaDamage_init_sub() {
    WWHD_FUNC(0x023FE62C, void, this);
    mProcVar6 = 0;
    speed.y = 80.0f;
    LK_voiceStart(4);
    seStartMapInfo(0x3810 /* JA_SE_LK_FALL_MAGMA */);
    LK_demoType = 5;
    gravity = -2.5f;
    dComIfGp_onPlayerStatus0_l(0x10000000 /* daPyStts0_UNK10000000_e */);
    LK_changePlayer();
}
VERIFY(0x023FE62C, &daPy_lk_c::dProcLavaDamage_init_sub);

/* 023FE6B8 */
BOOL daPy_lk_c::dProcLavaDamage_init() {
    WWHD_FUNC(0x023FE6B8, BOOL, this);
    if (mCurProc == 0xB0 /* daPyProc_DEMO_LAVA_DAMAGE_e */) {
        return TRUE;
    }
    gabi::call(LK_commonProcInit, this, 0xB0);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0x61 /* ANM_LAVADAM */, 1.0f, 0.0f, -1, 5.0f);
    u32 mtx = lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0);
    gabi::call(0x023D457C /* makeEmitter */, &m32E4, 0x8078, mtx, &current.pos, nullptr);
    if (dComIfGp_event_compulsory_l(this)) {
        dProcLavaDamage_init_sub();
    } else {
        speed.y = 0.0f;
        gravity = 0.0f;
        mProcVar6 = 1;
    }
    return TRUE;
}
VERIFY(0x023FE6B8, &daPy_lk_c::dProcLavaDamage_init);

/* 023FE7C0 */
BOOL daPy_lk_c::checkLavaFace(cXyz* oldPos, int attr) {
    WWHD_FUNC(0x023FE7C0, BOOL, this, oldPos, attr);
    u32 me = gabi::ea(this);
    if (LK_demoType == 5 /* checkSpecialDemoMode() */) {
        return FALSE;
    }
    if (oldPos != nullptr) {
        f32 z = current.pos.z;
        f32 y = oldPos->y + 20.0f;
        gabi::store<f32>(me + 0xCB8, z); /* mLavaGndChk.SetPos() */
        gabi::store<f32>(me + 0xCB0, current.pos.x);
        gabi::store<f32>(me + 0xCB4, y);
        f64 gc = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), me + 0xC8C);
        f32 gh = mAcch.m_ground_h;
        f32 cy = current.pos.y;
        m35D4 = (f32)gc;
        if ((f64)gh > gc) {
            gc = -1000000000.0f;
            m35D4 = -1000000000.0f;
        }
        if (!(gc > (f64)cy)) {
            return FALSE;
        }
        attr = gabi::call<s32>(0x024EF0F4 /* dBgS::GetAttributeCode */, dComIfG_Bgsp(), me + 0xCA0);
    }
    if (attr == 6 /* dBgS_Attr_LAVA_e */) {
        if (oldPos != nullptr) {
            current.pos.y = m35D4;
        }
        if (LK_callB(0x023EB6C8 /* checkBossGomaStage */)) {
            return procLavaDamage_init();
        }
        return dProcLavaDamage_init();
    }
    if (attr == 8 /* dBgS_Attr_VOID_e */) {
        startRestartRoom(5, 0xC9, -1.0f, 0);
    }
    return FALSE;
}
VERIFY(0x023FE7C0, &daPy_lk_c::checkLavaFace);

/* 023FE918 */
BOOL daPy_lk_c::checkSwimFallCheck() {
    WWHD_FUNC(0x023FE918, BOOL, this);
    if (!(mNoResetFlg0 & 0x80) ||
        (current.pos.y > mWaterY + 30.1f && !(gabi::load<u8>(0x101EA4E4) & 1) /* !dPa_control_c::isStatus(1) */)) {
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023FE918, &daPy_lk_c::checkSwimFallCheck);

/* 023FE960 */
int daPy_lk_c::setRoomInfo() {
    WWHD_FUNC(0x023FE960, int, this);
    u32 me = gabi::ea(this);
    u32 gnd = me + 0x8F4; /* mAcch.m_gnd */
    s32 room = gabi::call<s32>(0x024EF130 /* dBgS::GetRoomId */, dComIfG_Bgsp(), gnd);
    gabi::store<u8>(me + 0x1C9, (u8)room); /* tevStr.mRoomNo */
    u8 col = (u8)gabi::call<u32>(0x024EEEB8 /* dBgS::GetPolyColor */, dComIfG_Bgsp(), gnd);
    gabi::store<u8>(me + 0x1CA, col); /* tevStr.mEnvrIdxOverride */
    s32 rev = dComIfGp_getReverb(room);
    gabi::store<u8>(me + 0x7642, (u8)room); /* mStts.SetRoomId() */
    gabi::store<u8>(me + 0x326, (u8)room); /* current.roomNo */
    mReverb = (s8)rev;
    if (((gabi::load<u32>(dComIfGp_getStageStagInfo_l() + 0xC) >> 16) & 7) == 7 /* dStageType_SEA_e */) {
        if (gabi::call<BOOL>(0x024EEABC /* dBgS::ChkMoveBG */, dComIfG_Bgsp(), gnd)) {
            mRestartPoint = 0;
        } else {
            s32 link = gabi::call<s32>(0x024EF064 /* dBgS::GetLinkNo */, dComIfG_Bgsp(), gnd);
            mRestartPoint = link;
            if (link == 0xFF) {
                /* HD: standing on the actor 0x1C7 also restarts at point 0 */
                u32 ac = gabi::call<u32>(0x02008438 /* cBgS::GetActorPointer */, dComIfG_Bgsp(), (u32)gabi::load<u16>(me + 0x8F6));
                if (ac != 0 && gabi::load<s16>(ac + 0xE) == 0x1C7) {
                    mRestartPoint = 0;
                }
            }
        }
    } else {
        mRestartPoint = 0xFF;
    }
    return room;
}
VERIFY(0x023FE960, &daPy_lk_c::setRoomInfo);

/* 023FEA6C */
void daPy_lk_c::checkFallCode() {
    WWHD_FUNC(0x023FEA6C, void, this);
    u32 me = gabi::ea(this);
    f32 gh = mAcch.m_ground_h;
    if (m3580 == 4 || gh == -1000000000.0f) {
        u32 mf = mModeFlg;
        bool restart;
        if (!(mWaterY > gh)) {
            restart = (mf & 2 /* ModeFlg_MIDAIR */) && !(mf & 0x200 /* ModeFlg_HOOKSHOT */) && m35F4 - current.pos.y > 2000.0f;
        } else {
            restart = (mf & 0x40000 /* ModeFlg_SWIM */) != 0;
        }
        if (restart || ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !(mNoResetFlg0 & 0xA0000000))) {
            startRestartRoom(5, 0xC9, -1.0f, 0);
            mAcch.m_flags = mAcch.m_flags & ~(u32)dBgS_Acch::GROUND_HIT; /* ClrGroundHit() */
        }
        return;
    }
    if ((mNoResetFlg0 & 0x10 /* daPyFlg0_DEKU_SP_RETURN_FLG */) && (mModeFlg & 0x40000)) {
        startRestartRoom(5, 0xC9, -1.0f, 0);
        return;
    }
    if (gabi::load<u16>(dComIfGp_ea() + 0x5BAC) == 0) { /* HD: the exits are disabled */
        return;
    }
    u32 gnd = me + 0x8F4;
    s32 exitId = gabi::call<s32>(0x024EECC8 /* dBgS::GetExitId */, dComIfG_Bgsp(), gnd);
    s32 room = gabi::call<s32>(0x024EF130 /* dBgS::GetRoomId */, dComIfG_Bgsp(), gnd);
    u32 ship = dComIfGp_getShipActor_l();
    if (exitId == 0x3F) {
        return;
    }
    bool go = (mModeFlg & 0x40000) || (mAcch.m_flags & dBgS_Acch::GROUND_HIT) || mCurProc == 0x93 /* daPyProc_FAN_GLIDE_e */;
    if (!go && dComIfGp_checkPlayerStatus0_l(0x10000) && ship != 0 &&
        !(gabi::load<u32>(ship + 0x704) != 0 || gabi::load<u32>(ship + 0x70C) != 0) /* !ship->checkForceMove() */) {
        go = true;
    }
    if (!go && !(m3580 == 5 && m35F4 - current.pos.y > 500.0f)) {
        return;
    }
    gabi::store<u8>(dComIfGp_ea() + 0x5BE0, 1); /* dComIfGs_startFwaterTimer() */
    f32 sp = speedF;
    f32 spd;
    if (sp < 1.5f) {
        spd = 1.5f;
    } else {
        spd = fsel_l(17.0f - sp, sp, 17.0f);
    }
    s32 type;
    if (dComIfGp_checkPlayerStatus0_l(0x10000)) {
        type = 1;
        if (dComIfGp_getShipActor_l() != 0) {
            u32 sh = dComIfGp_getShipActor_l();
            gabi::store<u32>(sh + 0x644, gabi::load<u32>(sh + 0x644) | 0x20000000); /* ship->onSceneChange() */
        }
    } else if (mModeFlg & 0x01000000 /* ModeFlg_CRAWL */) {
        type = mFrameCtrlUnder[0].mRate < 0.0f ? 3 : 2;
    } else {
        type = 0;
    }
    u16 cmd = gabi::load<u16>(me + 0xF8); /* eventInfo command */
    if (m3580 == 5) {
        mAcch.m_flags = mAcch.m_flags & ~(u32)dBgS_Acch::GROUND_HIT;
    }
    if (cmd != 3 /* !eventInfo.checkCommandDoor() */ && !dComIfGp_event_compulsory_l(this)) {
        return;
    }
    if (!gabi::call<BOOL>(0x025C38B8 /* dStage_changeSceneExitId */, gnd, spd, type, (s32)(s8)room)) {
        return;
    }
    setNoResetFlg1(noResetFlg1() | 0x00100000 /* daPyFlg1_UNK100000 */);
    if (gabi::call<BOOL>(0x025B8B94 /* dSv_event_c::isEventBit */, dComIfGs_base_l() + 0x644, 0x3E10)) {
        gabi::call(0x025B8B68 /* dSv_event_c::onEventBit */, dComIfGs_base_l() + 0x644, 0x3E01);
    }
    LK_changePlayer();
    if (lk_isStartStage(0x10035964 /* "GanonM" */) && current.roomNo == 1) {
        gabi::call(0x025E1988 /* seStartSystem */, 0x288B /* JA_SE_LK_MAZE_WARP_IN */);
    }
    if (gabi::load<u16>(me + 0xF8) == 3) {
        return;
    }
    LK_demoType = 3; /* mDemo.setOriginalDemoType() */
    if (m3580 == 5) {
        LK_demoMode = 0x11; /* DEMO_PFALL_e */
    } else if ((mModeFlg & 0x40000) || m34C3 == 1 || m34C3 == 4) {
        mNormalSpeed = mNormalSpeed * 0.75f;
        LK_demoMode = 0xE; /* DEMO_KEEP_e */
    } else {
        LK_demoMode = 2; /* DEMO_N_WALK_e */
    }
    gabi::Local<cXyz> d;
    cXyz_mi(&current.pos, d, &old.pos);
    f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, d.get()));
    if (len > (f64)0.1f) {
        LK_FIELD(s16, 0x422) = cM_atan2s(d->x, d->z); /* mDemo.setMoveAngle() */
    } else {
        LK_FIELD(s16, 0x422) = current.angle.y;
    }
}
VERIFY(0x023FEA6C, &daPy_lk_c::checkFallCode);

/* 023FF020 */
void daPy_lk_c::setShapeAngleOnGround() {
    WWHD_FUNC(0x023FF020, void, this);
    u32 me = gabi::ea(this);
    if (mNoResetFlg0 & 0xA0000000) {
        return;
    }
    f32 k = m35E4;
    if (k < 0.0f) {
        k = 0.0f;
        m35E4 = k;
    } else if (k > 1.0f) {
        k = 1.0f;
        m35E4 = k;
    }
    s16 ax;
    if (mModeFlg & 0x01000000 /* ModeFlg_CRAWL */) {
        const u32 stk = 0x1048D0CC; /* mDoMtx_stack_c::now */
        gabi::call(0x028E93CC /* PSMTXTrans */, stk, (f32)current.pos.x, (f32)current.pos.y, (f32)current.pos.z);
        gabi::call(0x025F1C28 /* mDoMtx_YrotM */, stk, (s32)shape_angle.y);
        gabi::Local<cXyz> front;
        gabi::Local<cXyz> back;
        gabi::call(0x028E8F64 /* PSMTXMultVec */, stk, 0x101CEBF0 /* l_crawl_front_up_offset */, front.get());
        gabi::call(0x028E8F64, stk, 0x101CEBFC /* l_crawl_back_up_offset */, back.get());
        lk_copy12(me + 0xB38, front); /* mGndChk.SetPos(&front) */
        f64 gc = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
        f32 ny0 = -1.0f;
        front->y = (f32)gc;
        u32 tp;
        if (gc != -1000000000.0 && (tp = gabi::call<u32>(0x020084C8 /* cBgS::GetTriPla */, dComIfG_Bgsp(),
                                                         (u32)gabi::load<u16>(me + 0xB2A), (u32)gabi::load<u16>(me + 0xB28))) != 0) {
            ny0 = gabi::load<f32>(tp + 4);
        }
        lk_copy12(me + 0xB38, back);
        gc = gabi::call<f64>(0x02008974, dComIfG_Bgsp(), mGndChk);
        f32 ny1 = -1.0f;
        back->y = (f32)gc;
        if (gc != -1000000000.0 && (tp = gabi::call<u32>(0x020084C8, dComIfG_Bgsp(), (u32)gabi::load<u16>(me + 0xB2A),
                                                         (u32)gabi::load<u16>(me + 0xB28))) != 0) {
            ny1 = gabi::load<f32>(tp + 4);
        }
        gabi::Local<cXyz> d;
        cXyz_mi(front, d, back);
        gabi::Local<cXyz> xz;
        f32 dy = d->y;
        xz->x = d->x;
        xz->y = 0.0f;
        xz->z = d->z;
        f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get()));
        s32 a = gabi::call<s32>(0x020195B0 /* cM_atan2s */, -dy, len);
        if (cLib_distanceAngleS((s16)a, shape_angle.x) < 0x1000 && ny0 > 0.0f && ny1 > 0.0f && std::fabs(ny0 - ny1) > 0.05f) {
            ax = (s16)gabi::ftoi((f32)a * m35E4);
        } else {
            ax = (s16)gabi::ftoi((f32)m34E2 * m35E4);
        }
    } else {
        ax = (s16)gabi::ftoi((f32)m34E2 * k);
    }
    shape_angle.x = ax;
    s32 g = gabi::call<s32>(0x023E7D6C /* getGroundAngle */, this, me + 0x8F4, (s32)(s16)(shape_angle.y - 0x4000));
    shape_angle.z = (s16)gabi::ftoi((f32)g * m35E4);
}
VERIFY(0x023FF020, &daPy_lk_c::setShapeAngleOnGround);

/* 023FF47C */
void daPy_lk_c::setStepsOffset() {
    WWHD_FUNC(0x023FF47C, void, this);
    u32 me = gabi::ea(this);
    gabi::call(0x0200ECD4 /* cLib_addCalc */, &m35C4, 0.0f, 0.5f, 25.0f, 5.0f);
    u16 a = (u16)m34E2;
    f32 t = cM_ssin(a) / cM_scos(a);
    f32 sp = speedF;
    u16 ay = (u16)current.angle.y;
    f32 px = gabi::fmadds(sp, cM_ssin(ay), current.pos.x);
    f32 py = current.pos.y + 30.1f;
    f32 pz = gabi::fmadds(sp, cM_scos(ay), current.pos.z);
    gabi::store<f32>(me + 0xB38, px); /* mGndChk.SetPos() */
    gabi::store<f32>(me + 0xB3C, py);
    gabi::store<f32>(me + 0xB40, pz);
    f64 gc = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), mGndChk);
    u32 flg = mNoResetFlg0 & 0x80000000;
    f32 y = current.pos.y;
    f32 sp2 = speedF;
    if (flg && (f64)y > gc) {
        gc = y;
    }
    f32 diff = gabi::fmadds(sp2, t, (f32)(gc - (f64)y));
    if (diff > 0.0f) {
        current.pos.y = (f32)gc;
        m35C4 = gabi::fnmsubs(diff, 0.7f, m35C4);
    } else if (!flg) {
        gabi::Local<cXyz> d;
        cXyz_mi(&old.pos, d, &current.pos);
        gabi::Local<cXyz> xz;
        f32 dy = d->y;
        xz->x = d->x;
        xz->y = 0.0f;
        xz->z = d->z;
        f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get()));
        f32 f = (f32)(-(len * (f64)t - (f64)dy)); /* fnmsubs */
        if (!(f < 0.0f)) {
            m35C4 = gabi::fmadds(f, 0.7f, m35C4);
        }
    }
}
VERIFY(0x023FF47C, &daPy_lk_c::setStepsOffset);

/* mpCLModel->setBaseScale(scale); mpCLModel->setBaseTRMtx(mDoMtx_stack_c::get()) (float copies) */
static inline void lk_setModelBase(u32 me, u32 stk) {
    u32 model = gabi::load<u32>(me + 0x448);
    f32 sz = gabi::load<f32>(me + 0x338);
    f32 sy = gabi::load<f32>(me + 0x334);
    f32 sx = gabi::load<f32>(me + 0x330);
    gabi::store<f32>(model + 0xC4, sz);
    gabi::store<f32>(model + 0xC0, sy);
    gabi::store<f32>(model + 0xBC, sx);
    f32 m[12];
    for (int i = 0; i < 12; i++) {
        m[i] = gabi::load<f32>(stk + i * 4);
    }
    model = gabi::load<u32>(me + 0x448);
    for (int i = 0; i < 12; i++) {
        gabi::store<f32>(model + 0xC8 + i * 4, m[i]);
    }
}

/* 023FF6A0 */
void daPy_lk_c::setWorldMatrix() {
    WWHD_FUNC(0x023FF6A0, void, this);
    u32 me = gabi::ea(this);
    const u32 stk = 0x1048D0CC; /* mDoMtx_stack_c::now */
    f32 ty = (current.pos.y + m35C4) + m3608;
    gabi::call(0x028E93CC /* PSMTXTrans */, stk, (f32)current.pos.x, ty, (f32)current.pos.z);
    u32 ship = dComIfGp_getShipActor_l();
    if (dComIfGp_checkPlayerStatus0_l(0x10000) && ship != 0) {
        gabi::call(0x025F1B48 /* mDoMtx_ZXYrotM */, stk, (s32)m353C, (s32)gabi::load<s16>(ship + 0x32A), (s32)m353E);
        gabi::call(0x025F1C28 /* mDoMtx_YrotM */, stk, (s32)(s16)(shape_angle.y - gabi::load<s16>(ship + 0x32A)));
        gabi::Local<Mtx34> rel;
        gabi::call(0x028E90D4 /* PSMTXCopy */, stk, rel.get());
        lk_setModelBase(me, stk);
        gabi::call(0x028E91EC /* PSMTXInverse */, stk, stk);
        gabi::call(0x028E90D4, stk, &m37B4);
        mDoMtx_YrotS(gabi::at<Mtx34>(stk), (s16)-shape_angle.y);
        gabi::call(0x028E9108 /* PSMTXConcat */, stk, rel.get(), stk);
        gabi::Local<cXyz> v;
        gabi::call(0x028E9044 /* PSMTXMultVecSR */, stk, 0x101FFBCC /* cXyz::BaseZ */, v.get());
        shape_angle.x = cM_atan2s(-v->y, v->z);
        gabi::call(0x028E9044, stk, 0x101FFBB4 /* cXyz::BaseX */, v.get());
        f32 vz = v->z;
        f32 vy = v->y;
        f64 len = gabi::call<f64>(0x028F4384 /* std::sqrtf */, gabi::fmadds(vy, vy, vz * vz));
        f32 y2 = v->y;
        f32 x2 = v->x;
        if (!(y2 < 0.0f)) {
            shape_angle.z = (s16)gabi::call<s32>(0x020195B0 /* cM_atan2s */, len, x2);
        } else {
            shape_angle.z = (s16)gabi::call<s32>(0x020195B0, -len, x2);
        }
        return;
    }
    if (mModeFlg & 0x10000 /* ModeFlg_CLIMB */) {
        gabi::call(0x025F1B48, stk, (s32)shape_angle.x, (s32)shape_angle.y, (s32)shape_angle.z);
        gabi::call(0x025F24E0 /* mDoMtx_stack_c::transM */, 0.0f, 0.0f, 20.5f);
        gabi::call(0x025F1C28, stk, (s32)m34EC);
        gabi::call(0x025F24E0, 0.0f, 0.0f, -20.5f);
    } else {
        gabi::call(0x025F1B48, stk, (s32)shape_angle.x, (s32)(s16)(shape_angle.y + m34EC), (s32)shape_angle.z);
    }
    lk_setModelBase(me, stk);
    gabi::call(0x028E91EC /* PSMTXInverse */, stk, stk);
    gabi::call(0x028E90D4 /* PSMTXCopy */, stk, &m37B4);
}
VERIFY(0x023FF6A0, &daPy_lk_c::setWorldMatrix);

/* 023FFB08 */
void daPy_lk_c::setWaistAngle() {
    WWHD_FUNC(0x023FFB08, void, this);
    s16 target;
    if (mModeFlg & 0x1045A823) {
        target = 0;
    } else {
        f32 k = std::fabs(mNormalSpeed / mMaxNormalSpeed);
        if (k > 1.0f) {
            k = 1.0f;
        }
        if (m3580 == 8) {
            target = 0;
        } else {
            target = (s16)gabi::ftoi(((f32)m34E2 * 0.7f) * k);
        }
    }
    cLib_addCalcAngleS(&m34E0, target, 2, 0x800, 0x200);
}
VERIFY(0x023FFB08, &daPy_lk_c::setWaistAngle);

/* 023FFBE0 */
int daPy_lk_c::setLegAngle(f32 height, int foot, s16* outUpper, s16* outLower) {
    WWHD_FUNC(0x023FFBE0, int, this, height, foot, outUpper, outLower);
    u32 me = gabi::ea(this);
    const u32 stk = 0x1048D0CC; /* mDoMtx_stack_c::now */
    const f64 zero = 3.814697265625e-06; /* cM3d_IsZero */
    if (std::fabs(height) < 0.1f) {
        return FALSE;
    }
    f32 fwd = height * 0.5f;
    if (fwd > 10.0f) {
        fwd = 10.0f;
    }
    u32 fd = me + 0x73F0 + foot * 0x118; /* mFootData[foot] */
    gabi::Local<cXyz> hip;   /* spE8 */
    gabi::Local<cXyz> knee;  /* spDC */
    gabi::Local<cXyz> ankle; /* spD0 */
    gabi::Local<cXyz> tmp;
    gabi::call(0x028E9108 /* PSMTXConcat */, &m37B4, fd + 0x88, stk);
    hip->y = gabi::load<f32>(stk + 0x1C);
    hip->x = 0.0f;
    hip->z = gabi::load<f32>(stk + 0x2C);
    gabi::call(0x028E9108, &m37B4, fd + 0xB8, stk);
    knee->z = gabi::load<f32>(stk + 0x2C);
    knee->x = 0.0f;
    knee->y = gabi::load<f32>(stk + 0x1C);
    gabi::call(0x028E9108, &m37B4, fd + 0xE8, stk);
    f32 ay = gabi::load<f32>(stk + 0x1C) + 3.25f;
    ankle->x = 0.0f;
    ankle->z = gabi::load<f32>(stk + 0x2C);
    ankle->y = ay;
    gabi::Local<cXyz> upper; /* spAC */
    gabi::Local<cXyz> lower; /* spA0 */
    cXyz_mi(knee, tmp, hip);
    lk_copy12(gabi::ea(upper.get()), tmp);
    cXyz_mi(ankle, tmp, knee);
    lk_copy12(gabi::ea(lower.get()), tmp);
    gabi::Local<cXyz> target; /* spB8 */
    f32 tz = ankle->z + fwd;
    f32 ty = ankle->y + height;
    target->z = tz;
    target->y = ty;
    target->x = ankle->x;
    if (!(ty < hip->y)) {
        return FALSE;
    }
    gabi::Local<cXyz> d; /* sp7C */
    cXyz_mi(target, tmp, hip);
    lk_copy12(gabi::ea(d.get()), tmp);
    f64 dd = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, d.get());
    if (std::fabs(dd) < zero) {
        return FALSE;
    }
    f64 uu = gabi::call<f64>(0x028E8DD0, upper.get());
    f64 ll = gabi::call<f64>(0x028E8DD0, lower.get());
    f64 su = gabi::call<f64>(0x028F4384 /* std::sqrtf */, uu);
    f64 sl = gabi::call<f64>(0x028F4384, ll);
    f32 sum = (f32)(su + sl);
    f64 sd = gabi::call<f64>(0x028F4384, dd);
    if (!((f64)sum > sd)) {
        return FALSE;
    }
    f32 a1 = (f32)(dd + uu);
    f32 a2 = (f32)(dd + dd);
    f32 a3 = (f32)((f64)a1 - ll);
    f32 k = a3 / a2;
    f32 kd = (f32)(dd * (f64)k);
    f32 h2 = (f32)(-((f64)kd * (f64)k - uu)); /* fnmsubs */
    f32 my = gabi::fmadds(k, d->y, hip->y);
    h2 = fsel_l(h2, h2, 0.0f);
    f32 mz = gabi::fmadds(k, d->z, hip->z);
    f64 h = gabi::call<f64>(0x028F4384, h2);
    gabi::Local<cXyz> n; /* sp70 */
    f32 dz = d->z;
    f32 dy = d->y;
    n->x = 0.0f;
    n->y = dz;
    n->z = -dy;
    f64 nl = gabi::call<f64>(0x028F4384, gabi::call<f64>(0x028E8DD0, n.get()));
    if (std::fabs(nl) < zero) {
        return FALSE;
    }
    f32 r = (f32)(h / nl);
    gabi::Local<cXyz> kneeNew; /* spC4 */
    kneeNew->x = 0.0f;
    kneeNew->y = gabi::fmadds(r, n->y, my);
    kneeNew->z = gabi::fmadds(r, n->z, mz);
    cXyz_mi(kneeNew, tmp, hip);
    f32 uy = tmp->y;
    f32 uz = tmp->z;
    cXyz_mi(target, tmp, kneeNew);
    f32 ly = tmp->y;
    f32 lz = tmp->z;
    gabi::call(0x028E9108 /* PSMTXConcat */, &m37B4, lk_getAnmMtx(gabi::ea(mpCLModel.get()), 0 /* CL_JNT_LINK_ROOT_e */), stk);
    s16 root = cM_atan2s(-(f32)gabi::load<f32>(stk + 0x1C), -(f32)gabi::load<f32>(stk + 0x2C));
    s16 aU = cM_atan2s(uy, uz);
    s16 aL = cM_atan2s(ly, lz);
    s16 diff = (s16)(aU - root);
    if (diff > 0x6000) {
        aU = (s16)(root + 0x6000);
    } else if (diff < -0x4000) {
        aU = (s16)(root - 0x4000);
    }
    s16 diff2 = (s16)(aL - aU);
    if (diff2 > 0) {
        gabi::store<s16>(gabi::ea(outUpper), (s16)(cM_atan2s(upper->y, upper->z) - aU));
        gabi::store<s16>(gabi::ea(outLower), (s16)(cM_atan2s(lower->y, lower->z) - aU));
    } else {
        if (diff2 < -0x7000) {
            aL = (s16)(aU - 0x7000);
        }
        gabi::store<s16>(gabi::ea(outUpper), (s16)(cM_atan2s(upper->y, upper->z) - aU));
        gabi::store<s16>(gabi::ea(outLower), (s16)(cM_atan2s(lower->y, lower->z) - aL));
    }
    return TRUE;
}
VERIFY(0x023FFBE0, &daPy_lk_c::setLegAngle);

/* 0240005C */
void daPy_lk_c::footBgCheck() {
    WWHD_FUNC(0x0240005C, void, this);
    u32 me = gabi::ea(this);
    const u32 stk = 0x1048D0CC; /* mDoMtx_stack_c::now */
    u32 model = gabi::ea(mpCLModel.get());
    u32 base = model != 0 ? model + 0xC8 : 0; /* mpCLModel->getBaseTRMtx() */
    u32 mode1 = mModeFlg & 1;
    gabi::call(0x028E9108 /* PSMTXConcat */, &m37B4, lk_getAnmMtx(model, 0x1E /* CL_JNT_WAIST_JNT_e */), stk);
    u16 wa = (u16)m34E0;
    f32 sn = cM_ssin(wa);
    f32 cs = cM_scos(wa);
    f32 h[2];
    u32 flag[2];
    gabi::Local<cXyz> p;
    gabi::Local<cXyz> t1;
    gabi::Local<cXyz> t2;
    gabi::Local<cXyz> xz;
    gabi::Local<cXyz> w;
    for (int i = 0; i < 2; i++) {
        u32 fd = me + 0x73F0 + i * 0x118; /* mFootData[i] */
        cXyz_pl_l(fd + 0x18, t1, fd + 0xC);
        cXyz_ml(t1, t2, 0.5f);
        lk_copy12(gabi::ea(p.get()), t2);
        cXyz_mi(p, t1, gabi::at<cXyz>(fd + 0x24));
        xz->z = t1->z;
        xz->x = t1->x;
        xz->y = 0.0f;
        f64 m = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get());
        if (m < 100.0 && mode1) {
            u8 tm = gabi::load<u8>(fd + 1);
            if (tm != 0) {
                gabi::store<u8>(fd + 1, (u8)(tm - 1));
            } else {
                p->x = gabi::load<f32>(fd + 0x24);
                p->y = gabi::load<f32>(fd + 0x28);
                p->z = gabi::load<f32>(fd + 0x2C);
            }
        } else {
            gabi::store<u8>(fd + 1, 5);
        }
        f32 px = p->x;
        f32 py = p->y;
        f32 pz = p->z;
        gabi::store<f32>(fd + 0x28, py);
        gabi::store<f32>(fd + 0x24, px);
        gabi::store<f32>(fd + 0x2C, pz);
        f32 m13 = gabi::load<f32>(stk + 0x1C);
        f32 m23 = gabi::load<f32>(stk + 0x2C);
        f32 lift = gabi::fmadds(p->z - m23, sn, gabi::fmadds(p->y - m13, cs, m13));
        gabi::call(0x028E8F64 /* PSMTXMultVec */, base, p.get(), w.get());
        f32 gy = current.pos.y + 30.1f;
        gabi::store<f32>(fd + 0x58, w->x); /* field_0x034.SetPos() */
        gabi::store<f32>(fd + 0x5C, gy);
        gabi::store<f32>(fd + 0x60, w->z);
        f64 gc = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), fd + 0x34);
        flag[i] = 0;
        if (mNoResetFlg0 & 0x80000000) {
            f32 y = current.pos.y;
            if ((f64)y > gc) {
                gc = y;
                flag[i] = 1;
            }
        }
        if (gc != -1000000000.0 && (f32)((f64)gy - gc) < 60.2f) {
            h[i] = (f32)gc;
            gabi::store<u8>(fd + 0, 1);
        } else {
            h[i] = current.pos.y;
            gabi::store<u8>(fd + 0, 0);
        }
        f32 v = h[i];
        if (mNoResetFlg0 & 0xA0000000) {
            f32 y = current.pos.y;
            if (!(v > y)) {
                v = y;
                h[i] = v;
            }
        }
        h[i] = v - (p->y - lift);
    }
    gabi::Local<be<s16>> upper[2];
    gabi::Local<be<s16>> lower[2];
    bool none = !(mAcch.m_flags & dBgS_Acch::GROUND_HIT) || (mModeFlg & 0x1045A822);
    int j = 0;
    f32 target;
    if (none) {
        target = 0.0f;
    } else {
        j = h[0] > h[1] ? 1 : 0;
        if (mCurProc == 0xA9 /* daPyProc_DEMO_TOOL_e */ || std::fabs((f32)m35C4) > 1.0f) {
            target = 0.0f;
        } else {
            target = fsel_l(h[1] - h[0], h[0], h[1]) - current.pos.y;
        }
    }
    gabi::call(0x0200ECD4 /* cLib_addCalc */, &m35B8, target, 0.5f, 7.5f, 2.5f);
    gabi::store<f32>(base + 0x1C, gabi::load<f32>(base + 0x1C) + m35B8);
    m37B4.m[1][3] = m37B4.m[1][3] - m35B8;
    if (none) {
        for (int i = 0; i < 2; i++) {
            *upper[i].get() = 0;
            *lower[i].get() = 0;
            gabi::store<f32>(me + 0x73F0 + i * 0x118 + 0x30, 0.0f);
        }
    } else {
        int k = (j + 1) & 1;
        u32 fj = me + 0x73F0 + j * 0x118;
        u32 fk = me + 0x73F0 + k * 0x118;
        gabi::store<f32>(fj + 0x30, 0.0f);
        if (!setLegAngle(h[j] - gabi::load<f32>(base + 0x1C), j, (s16*)upper[j].get(), (s16*)lower[j].get())) {
            *lower[j].get() = 0;
            *upper[j].get() = 0;
        }
        f32 d = h[k] - gabi::load<f32>(base + 0x1C);
        if (d > 0.0f || mode1) {
            gabi::store<f32>(fk + 0x30, d * 0.3f);
            if (!setLegAngle(d * 0.7f, k, (s16*)upper[k].get(), (s16*)lower[k].get())) {
                *lower[k].get() = 0;
                *upper[k].get() = 0;
            }
        } else {
            gabi::store<f32>(fk + 0x30, 0.0f);
            *lower[k].get() = 0;
            *upper[k].get() = 0;
        }
    }
    for (int i = 0; i < 2; i++) {
        u32 fd = me + 0x73F0 + i * 0x118;
        s32 u = *upper[i].get();
        s32 cur = gabi::load<s16>(fd + 8);
        if (u * cur < 0 && lk_abs(u - cur) >= 0x8000) {
            u = (s16)(u >= 0 ? u - 0x4000 : u + 0x4000);
            *upper[i].get() = (s16)u;
        }
        cLib_addCalcAngleS(gabi::at<be<s16>>(fd + 8), (s16)u, 2, 0x1800, 0x10);
        cLib_addCalcAngleS(gabi::at<be<s16>>(fd + 6), *lower[i].get(), 2, 0x1800, 0x10);
        s16 f6 = gabi::load<s16>(fd + 6);
        s16 f8 = gabi::load<s16>(fd + 8);
        gabi::store<s16>(fd + 0xA, (s16)(f6 - f8));
        gabi::store<s16>(fd + 2, (s16)-(m34E0 + f6));
    }
    for (int i = 0; i < 2; i++) {
        u32 fd = me + 0x73F0 + i * 0x118;
        s32 g;
        if (flag[i] == 0 && gabi::load<u8>(fd + 0) != 0 && mode1) {
            g = gabi::call<s32>(0x023E7D6C /* getGroundAngle */, this, fd + 0x48, (s32)shape_angle.y);
            gabi::store<s16>(fd + 2, (s16)(gabi::load<s16>(fd + 2) + g));
            g = gabi::call<s32>(0x023E7D6C, this, fd + 0x48, (s32)(s16)(shape_angle.y - 0x4000));
        } else {
            g = 0;
        }
        cLib_addCalcAngleS(gabi::at<be<s16>>(fd + 4), (s16)g, 2, 0x1800, 0x10);
    }
}
VERIFY(0x0240005C, &daPy_lk_c::footBgCheck);

/* 02400990 */
void daPy_lk_c::setMoveSlantAngle() {
    WWHD_FUNC(0x02400990, void, this);
    f32 k = std::fabs(speedF / mMaxNormalSpeed);
    if (mModeFlg & 0x02000000) {
        m351C = 0;
        gabi::store<s16>(gabi::ea(this) + 0x3D4, 0); /* mBodyAngle.z */
        return;
    }
    s16 v;
    if (mCurProc == 6 /* daPyProc_MOVE_e */ && k > 0.95f) {
        f32 t = (k - 0.95f) / 0.050000012f;
        s16 d = (s16)(m34DE - shape_angle.y);
        cLib_addCalcAngleS(&m351C, (s16)gabi::ftoi(((f32)d * 1.6f) * t), 4, 0xC8, 0x64);
        v = (s16)(m351C >> 1);
        gabi::store<s16>(gabi::ea(this) + 0x3D4, v);
        shape_angle.z = v;
        return;
    }
    s16 cur = m351C;
    s16 step = (s16)gabi::ftoi((f32)cur * 0.35f);
    if (step == 0) {
        m351C = 0;
        shape_angle.z = 0;
        gabi::store<s16>(gabi::ea(this) + 0x3D4, 0);
    } else {
        s16 n = (s16)(cur - step);
        m351C = n;
        v = (s16)(n >> 1);
        gabi::store<s16>(gabi::ea(this) + 0x3D4, v);
        shape_angle.z = v;
    }
}
VERIFY(0x02400990, &daPy_lk_c::setMoveSlantAngle);

/* ---- setStickData (023F32E4) ---- */
/* HD controller reads (mDoCPd/CPad of pad 0); names by the GameCube call order, probably */
enum : u32 {
    LK_CPad_GET_STICK_VALUE = 0x020079B4,
    LK_CPad_GET_STICK_ANGLE = 0x02007A1C,
    LK_CPad_TRIG_B = 0x020078BC,
    LK_CPad_TRIG_A = 0x02007898,
    LK_CPad_TRIG_X = 0x020078E8,
    LK_CPad_TRIG_Y = 0x02007914,
    LK_CPad_TRIG_Z = 0x02007814,
    LK_CPad_TRIG_L = 0x0200786C,
    LK_CPad_R_LOCK_TRIG = 0x02007D4C,
    LK_CPad_HOLD_A = 0x020076BC,
    LK_CPad_HOLD_B = 0x020076E0,
    LK_CPad_HOLD_X = 0x0200770C,
    LK_CPad_HOLD_Y = 0x02007738,
    LK_CPad_HOLD_Z = 0x02007638,
    LK_CPad_HOLD_L = 0x02007690,
    LK_CPad_R_LOCK_BUTTON = 0x02007CDC,
};
/* HD: the sead controller (GamePad) object at *0x101F5088: +0x18 trigger bits, +0x124 hold bits */
static inline u32 lk_hdPadTrig() { return gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x18); }
static inline u32 lk_hdPadHold() { return gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x124); }
/* inline dSv_player_item_c::getItem(slot) over the save's item/bag tables (save + 0x20 base) */
static inline u8 lk_saveGetItem(u32 base, u32 idx) {
    if ((s32)idx < 0x15) return gabi::load<u8>(base + idx + 0x3C);
    if ((s32)idx < 0x18) return 0xFF;
    if ((s32)idx < 0x20) return gabi::load<u8>(base + idx + 0x5E);
    if ((s32)idx < 0x24) return 0xFF;
    if ((s32)idx < 0x2C) return gabi::load<u8>(base + idx + 0x5A);
    if (idx - 0x30 < 8) return gabi::load<u8>(base + idx + 0x56);
    return 0xFF;
}
/* inline dComIfGp_setSelectItem(btn) for the HD extra buttons: the save's select slot (save + selOff) ->
 * the play item byte (play + 0x12A0 + playOff); a slot holding no item is cleared */
static inline void lk_setSelectItemHD(u32 selOff, u32 playOff) {
    u8 sel = gabi::load<u8>(dComIfGs_base_l() + selOff);
    u32 p = dComIfGp_ea() + 0x12A0;
    if (sel == 0xFF) {
        gabi::store<u8>(p + playOff, 0xFF);
        return;
    }
    u32 s = dComIfGs_base_l();
    gabi::store<u8>(p + playOff, lk_saveGetItem(s + 0x20, gabi::load<u8>(s + selOff)));
    s = dComIfGs_base_l();
    if (lk_saveGetItem(s + 0x20, gabi::load<u8>(s + selOff)) == 0xFF) gabi::store<u8>(s + selOff, 0xFF);
}
static inline s16 lk_camControledAngleY(s32 idx) {
    return gabi::call<s16>(0x024F8018 /* dCam_getControledAngleY */, gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8));
}

void daPy_lk_c::setStickData() {
    WWHD_FUNC(0x023F32E4, void, this);
    mItemTrigger = 0;
    u8 old = mItemButton;
    mItemButton = 0;
    m34CA = old;
    if (gabi::load<u32>(dComIfGp_ea() + 0x5B2C) != gabi::ea(this)) {
        mStickDistance = 0.0f;
        m34E8 = 0;
    } else if (LK_demoMode == 0x44 /* checkBowMiniGame() */) {
        mStickDistance = gabi::call<f32>(LK_CPad_GET_STICK_VALUE, 0);
        m34DC = (s16)(gabi::call<s16>(LK_CPad_GET_STICK_ANGLE, 0) + 0x8000);
        s16 cam = lk_camControledAngleY(mCameraInfoIdx);
        m34E8 = (s16)(cam + m34DC);
        /* HD: the bow mini-game shoots with the Z-hold read (GameCube: hold A -> BTN_A) */
        if (gabi::call<BOOL>(LK_CPad_HOLD_Z, 0)) mItemButton = mItemButton | 0x10;
    } else if ((dComIfGp_event_runCheck_l() || LK_demoType != 0) && !dComIfGp_checkPlayerStatus1_l(1) &&
               mCurProc != 0xA2 /* NOT_USE */ && mCurProc != 0xA6 /* BOTTLE_GET */) {
        s32 mode = LK_demoMode;
        if (mode == 2 /* DEMO_N_WALK */ || mode == 3 /* DEMO_N_DASH */) {
            u8 id = LK_FIELD(u8, 0x2DC); /* demoActorID */
            bool have = false;
            if (id != 0 && id <= 0x20) {
                u32 demo = gabi::load<u32>(0x101D5FFC);
                if (demo == 0) {
                    gabi::call(0x0273AA24 /* JUTAssertion */, 0x10035038u, 0x23A, 0x10034F74u);
                    demo = gabi::load<u32>(0x101D5FFC);
                }
                have = gabi::call<u32>(0x02526E70 /* dDemo_object_c::getActor */, demo, (u32)id) != 0;
            }
            if (have) {
                m34E8 = (s16)LK_FIELD(s16, 0x422);
                mStickDistance = 1.0f;
            } else {
                fcpy_l(gabi::ea(this) + 0x6A08, gabi::ea(this) + 0x434); /* mDemo.getStick() */
                m34E8 = (s16)LK_FIELD(s16, 0x422);
            }
        } else {
            mStickDistance = 0.0f;
            m34E8 = (s16)LK_FIELD(s16, 0x422); /* mDemo.getMoveAngle() */
        }
        if (LK_demoMode == 0xE /* DEMO_KEEP */) mItemButton = (u8)m34CA;
    } else if (gabi::call<s32>(0x025DBE00 /* fopOvlpM_IsPeek */) == 1) {
        mStickDistance = 0.0f;
        m34E8 = 0;
    } else {
        mStickDistance = gabi::call<f32>(LK_CPad_GET_STICK_VALUE, 0);
        if ((noResetFlg1() & 0x100 /* CONFUSE */) && !dComIfGp_event_runCheck_l()) {
            m34DC = gabi::call<s16>(LK_CPad_GET_STICK_ANGLE, 0);
        } else {
            m34DC = (s16)(gabi::call<s16>(LK_CPad_GET_STICK_ANGLE, 0) + 0x8000);
        }
        s16 cam = lk_camControledAngleY(mCameraInfoIdx);
        m34E8 = (s16)(cam + m34DC);
        u8 bVar2 = gabi::load<u8>(dComIfGp_ea() + 0x5BCC); /* dComIfGp_getButtonActionMode() */
        bool bVar1 = false;
        if (gabi::call<BOOL>(LK_CPad_TRIG_B, 0)) {
            /* HD: B also works while the ship's item use is active */
            if ((bVar2 & 2) || gabi::call<int>(LK_checkShipRideUseItem, this, 0) == 1) {
                mItemTrigger = mItemTrigger | 2;
            } else {
                bVar1 = true;
            }
        }
        if (gabi::call<BOOL>(LK_CPad_TRIG_A, 0)) {
            if (bVar2 & 1) {
                /* HD: on the ship with the (swift) sail item, A also assigns save select slot 0x2D */
                if (gabi::call<int>(LK_checkShipRideUseItem, this, 0) == 1 && !dComIfGp_checkPlayerStatus1_l(2) &&
                    !dComIfGp_checkPlayerStatus1_l(4) &&
                    (gabi::call<BOOL>(0x0254DA50 /* checkItemGet */, 0x78, 1) ||
                     gabi::call<BOOL>(0x0254DA50 /* checkItemGet */, 0x77, 1))) {
                    gabi::store<u8>(dComIfGs_base_l() + 0x2D, 1);
                    lk_setSelectItemHD(0x2D, 0x491F);
                }
                mItemTrigger = mItemTrigger | 1;
            } else {
                bVar1 = true;
            }
        }
        if (gabi::call<BOOL>(LK_CPad_TRIG_X, 0)) {
            if ((bVar2 & 4) && mTinkleHoverTimer == 0) {
                mItemTrigger = mItemTrigger | 4;
            } else {
                bVar1 = true;
            }
        }
        if (gabi::call<BOOL>(LK_CPad_TRIG_Y, 0)) {
            if ((bVar2 & 8) && mTinkleHoverTimer == 0) {
                mItemTrigger = mItemTrigger | 8;
            } else {
                bVar1 = true;
            }
        }
        if (gabi::call<BOOL>(LK_CPad_TRIG_Z, 0)) {
            if ((bVar2 & 0x20) && mTinkleHoverTimer == 0) {
                mItemTrigger = mItemTrigger | 0x10;
            } else {
                bVar1 = true;
            }
        }
        /* HD: the GamePad's extra item button (0x80): item 0x22 normally, items 0x31 / 0x25 on the ship;
         * it writes the save's select slot 0x2C and the play item byte */
        {
            u32 sel = 0;
            u32 need = 0;
            if (lk_hdPadTrig() & 0x10000) {
                sel = 2;
                need = 0x22;
            } else {
                int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
                if (use == 1 || gabi::call<int>(LK_checkShipRideUseItem, this, 0) == 2) {
                    if (lk_hdPadTrig() & 0x40000) {
                        sel = 0xD;
                        need = 0x31;
                    } else if (lk_hdPadTrig() & 0x80000) {
                        sel = 3;
                        need = 0x25;
                    }
                }
            }
            if (need != 0 && gabi::call<BOOL>(0x0254DA50 /* checkItemGet */, need, 1)) {
                if ((bVar2 & 0x40) && mTinkleHoverTimer == 0) {
                    gabi::store<u8>(dComIfGs_base_l() + 0x2C, (u8)sel);
                    lk_setSelectItemHD(0x2C, 0x491E);
                    mItemTrigger = mItemTrigger | 0x80;
                } else {
                    bVar1 = true;
                }
            }
        }
        if (gabi::call<BOOL>(LK_CPad_TRIG_L, 0)) mItemTrigger = mItemTrigger | 0x20;
        if (gabi::call<BOOL>(LK_CPad_R_LOCK_TRIG, 0)) mItemTrigger = mItemTrigger | 0x40;
        if (gabi::call<BOOL>(LK_CPad_HOLD_A, 0) && (bVar2 & 1)) mItemButton = mItemButton | 1;
        if (gabi::call<BOOL>(LK_CPad_HOLD_B, 0) &&
            ((bVar2 & 2) || gabi::call<int>(LK_checkShipRideUseItem, this, 0) == 1)) {
            mItemButton = mItemButton | 2;
        }
        if (mTinkleHoverTimer == 0) {
            if (gabi::call<BOOL>(LK_CPad_HOLD_X, 0) && (bVar2 & 4)) mItemButton = mItemButton | 4;
            if (gabi::call<BOOL>(LK_CPad_HOLD_Y, 0) && (bVar2 & 8)) mItemButton = mItemButton | 8;
            if (gabi::call<BOOL>(LK_CPad_HOLD_Z, 0) && (bVar2 & 0x20)) mItemButton = mItemButton | 0x10;
            bool hold = false;
            if ((lk_hdPadHold() & 0x10000) && (bVar2 & 0x40)) {
                hold = true;
            } else {
                int use = gabi::call<int>(LK_checkShipRideUseItem, this, 0);
                if (use == 1 || gabi::call<int>(LK_checkShipRideUseItem, this, 0) == 2) {
                    hold = (lk_hdPadHold() & 0x40000) || (lk_hdPadHold() & 0x80000);
                }
            }
            if (hold) mItemButton = mItemButton | 0x80;
        }
        if (gabi::call<BOOL>(LK_CPad_HOLD_L, 0)) mItemButton = mItemButton | 0x20;
        if (gabi::call<BOOL>(LK_CPad_R_LOCK_BUTTON, 0)) mItemButton = mItemButton | 0x40;
        if (gabi::call<bool>(LK_checkHeavyStateOn, this)) {
            mStickDistance = gabi::fmuls_ppc((f32)mStickDistance, 0.5f);
        }
        if (LK_FIELD(f32, 0x3CC) < 0.0f /* checkGrabWear() */) {
            mStickDistance = gabi::fmuls_ppc((f32)mStickDistance, 0.8f);
        }
        if (bVar1 && !dComIfGp_event_runCheck_l() && !dComIfGp_checkPlayerStatus1_l(8)) {
            gabi::call(0x025E1988 /* seStartSystem */, 0x883 /* JA_SE_ITEM_TARGET_OUT */);
        }
    }
    s16 angle_diff = (s16)(m34DC - m34EA);
    s32 cnt = m3578;
    s32 abs_v = lk_abs(angle_diff);
    if ((s32)((u32)cnt * (u32)(s32)angle_diff) < 0) {
        m3578 = angle_diff;
        m3524 = 4;
    } else if ((u32)(abs_v - 0x6D5) < 0x641E) {
        m3578 = cnt + angle_diff;
        m3524 = 4;
    } else if (m3524 > 0) {
        m3524 = (s16)(m3524 - 1);
    } else {
        m3578 = 0;
    }
}
VERIFY(0x023F32E4, &daPy_lk_c::setStickData);

/* ---- setDemoData (023F22FC) ---- */
/* inline sead::SafeString::operator==(a, b) where GHS calls the first assureTermination directly (02444F48) */
static inline bool lk_ss_cmp_d(lk_SafeString_l* a, lk_SafeString_l* b) {
    gabi::call(0x02444F48 /* sead::SafeString::assureTerminationImpl_ */, a);
    lk_ss_assure(a);
    u32 pa = a->mStr;
    lk_ss_assure(b);
    u32 pb = b->mStr;
    if (pa == pb)
        return true;
    for (u32 n = 0; n < 0x40001; n++) {
        u8 ca = gabi::load<u8>(a->mStr + n);
        u8 cb = gabi::load<u8>(pb + n);
        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
    return false;
}
static inline bool lk_strEq_d(u32 lit, u32 str) {
    gabi::Local<lk_SafeString_l> a;
    a->__vtbl = LK_SAFESTRING_VTBL;
    a->mStr = lit;
    gabi::Local<lk_SafeString_l> b;
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mStr = str;
    return lk_ss_cmp_d(a.get(), b.get());
}
/* strcmp(dComIfGp_getRunEventName(), lit) == 0 */
static inline bool lk_isRunEvent(u32 lit) {
    gabi::Local<lk_SafeString_l> a;
    a->__vtbl = LK_SAFESTRING_VTBL;
    a->mStr = lit;
    u32 name = gabi::call<u32>(0x02542E94 /* dEvent_manager_c::getRunEventName (matcher: dummy1) */, dComIfGp_ea() + 0x52C4);
    gabi::Local<lk_SafeString_l> b;
    b->__vtbl = LK_SAFESTRING_VTBL;
    b->mStr = name;
    return lk_ss_cmp(a.get(), b.get());
}
static inline u32 lk_evmng_getMySubstanceP(s32 staff, u32 name, s32 type) {
    return gabi::call<u32>(0x0254487C /* dEvent_manager_c::getMySubstanceP */, dComIfGp_ea() + 0x52C4, staff, name, type);
}
static inline void lk_evmng_cutEnd(s32 staff) { gabi::call(0x02543280 /* dEvent_manager_c::cutEnd */, dComIfGp_ea() + 0x52C4, staff); }
#define LK_setPlayerPosAndAngle(pos, ang) gabi::call_ptr(gabi::load<u32>(__vtbl + 0x114), this, (u32)(pos), (s32)(s16)(ang))
struct lk_TexArray_l {
    u8 b[0x168]; /* 10 objects of 0x24 (constructor 02444280) */
};

void daPy_lk_c::setDemoData() {
    WWHD_FUNC(0x023F22FC, void, this);
    u8 id = LK_FIELD(u8, 0x2DC); /* demoActorID */
    u32 demo_actor_p = 0;
    if (id != 0 && id <= 0x20) {
        u32 demo = gabi::load<u32>(0x101D5FFC);
        if (demo == 0) {
            gabi::call(0x0273AA24 /* JUTAssertion */, 0x10035038u, 0x23A, 0x10034F74u);
            demo = gabi::load<u32>(0x101D5FFC);
        }
        demo_actor_p = gabi::call<u32>(0x02526E70 /* dDemo_object_c::getActor */, demo, (u32)id);
    }
    u32 pos_p = 0;
    s16 angle = 0;
    s32 demo_mode = 1; /* DEMO_N_WAIT */
    u32 prm0_p = 0;
    u32 prm1_p = 0;
    if (!dComIfGp_event_runCheck_l()) {
        if (LK_demoType != 0) endDemoMode();
        if (m3554 > 0) {
            m3554 = (s16)(m3554 - 1);
        } else {
            setNoResetFlg1(noResetFlg1() & ~0x20000u); /* FOREST_WATER_USE */
        }
        return;
    }
    {
        u32 a = dComIfGp_ea() + 0x5CD8; /* clearPlayerStatus0(BOOMERANG_WAIT) */
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x00400000u);
    }
    s32 proc;
    if (!(gabi::load<u16>(dComIfGp_ea() + 0x52B8) & 0x20)) { /* !dComIfGp_event_chkEventFlag(0x20) */
        proc = mCurProc;
        if (proc == 0 /* SCOPE */) {
            gabi::call(LK_procWait_init, this);
            proc = mCurProc;
        } else if (proc == 0x8A /* SHIP_SCOPE */) {
            procShipPaddle_init();
            proc = mCurProc;
        }
    } else {
        proc = mCurProc;
    }
    if (proc == 1 /* SUBJECTIVITY */) gabi::call(LK_procWait_init, this);
    if (LK_demoType == 4 /* TYPE_START */ &&
        !gabi::call<BOOL>(0x025449B0 /* dEvent_manager_c::checkStartDemo */, dComIfGp_ea() + 0x52C4)) {
        LK_demoType = 2; /* setSystemDemoType() */
        LK_FIELD(u8, 0x8264) = 0;
    } else {
        /* HD: the byte set by the Tingle event order starts an original-type demo (mode 0x34) */
        if (LK_FIELD(u8, 0x8264) != 0) {
            LK_FIELD(s32, 0x428) = 0;
            LK_demoMode = 0x34;
            LK_demoType = 3;
        }
        LK_FIELD(u8, 0x8264) = 0;
    }
    if (demo_actor_p != 0) {
        gabi::store<u32>(demo_actor_p + 0x48, gabi::load<u32>(gabi::ea(this) + 0x448)); /* setModel(mpCLModel) */
        u16 type = LK_demoType;
        mStaffIdx = -1;
        if (type != 1 /* TYPE_TOOL */) {
            LK_demoType = 1;
            gabi::call(0x023DCF8C /* freeGrabItem */, this);
            gabi::call(0x023DCA80 /* freeRopeItem */, this);
            gabi::call(0x023DFA7C /* freeHookshotItem */, this);
            gabi::call(LK_deleteEquipItem, this, 0);
            gabi::call(LK_procWait_init, this);
        }
        u16 en = gabi::load<u16>(demo_actor_p + 4);
        if (en & 0x20 /* ENABLE_ANM */) demo_mode = gabi::load<s32>(demo_actor_p + 0x2C);
        pos_p = (en & 2 /* ENABLE_TRANS */) ? demo_actor_p + 8 : gabi::ea(&current.pos);
        angle = (en & 8 /* ENABLE_ROTATE */) ? gabi::load<s16>(demo_actor_p + 0x22) : (s16)shape_angle.y;
        if ((en & 0x10 /* ENABLE_SHAPE */) && gabi::load<u8>(dComIfGs_base_l() + 0x1C0) == 0 /* getClearCount() */) {
            u32 casual = noResetFlg1() & 8; /* CASUAL_CLOTHES */
            s32 shape = gabi::load<s32>(demo_actor_p + 0x28);
            if (casual ? shape == 0 : shape == 1) {
                /* HD: the Link textures are a list of 10 texture objects (0x24 bytes each) */
                u32 list = mpCurrLinktex;
                gabi::Local<lk_TexArray_l> tmp;
                gabi::call(0x028EFFD0 /* __construct_array */, tmp.get(), 10, 0x24, 0x02444280u);
                u32 t = gabi::ea(tmp.get());
                for (u32 i = 0; i < 10; i++) {
                    u32 src = 0;
                    if (i < gabi::load<u32>(list)) src = gabi::load<u32>(gabi::load<u32>(list + 8) + i * 4);
                    for (u32 k = 0; k < 0x24; k += 4) gabi::store<u32>(t + i * 0x24 + k, gabi::load<u32>(src + k));
                }
                gabi::call(0x0272B8B8 /* copy into the texture list */, (u32)mpCurrLinktex, gabi::ea(this) + 0x48C);
                for (u32 i = 0; i < 10; i++)
                    for (u32 k = 0; k < 0x24; k += 4)
                        gabi::store<u32>(gabi::ea(this) + 0x48C + i * 0x24 + k, gabi::load<u32>(t + i * 0x24 + k));
                u32 f = noResetFlg1();
                setNoResetFlg1((f & 8) ? (f & ~8u) : (f | 8u));
            }
        }
    } else {
        if (LK_demoType == 0 && dComIfGp_event_runCheck_l()) LK_demoType = 2;
        if (mStaffIdx != -1) {
            u32 cut_name = gabi::call<u32>(0x02544830 /* dEvent_manager_c::getMyNowCutName */, dComIfGp_ea() + 0x52C4,
                                           (s32)mStaffIdx);
            /* HD: two events (names at 0x100357A8 / 0x100357BC) get special cut handling */
            bool evA = lk_isRunEvent(0x100357A8);
            bool evB = lk_isRunEvent(0x100357BC);
            bool skip = cut_name != 0 && (evA || evB) && lk_strEq_d(0x100357D8, cut_name);
            if (!skip && cut_name != 0) {
                demo_mode = gabi::load<u8>(cut_name) * 100 + gabi::load<u8>(cut_name + 1) * 10 + gabi::load<u8>(cut_name + 2) - 0x14D0;
                if (LK_demoType != 4 && (demo_mode == 1 || demo_mode == 0x2A || demo_mode == 0x17)) {
                    if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) && !LK_checkPlayerFly()) {
                        speedF = 0.0f;
                        mNormalSpeed = 0.0f;
                    }
                }
                pos_p = lk_evmng_getMySubstanceP(mStaffIdx, 0x1003577C /* "pos" */, 1);
                if (pos_p == 0) pos_p = gabi::call<u32>(0x02544900 /* dEvent_manager_c::getGoal */, dComIfGp_ea() + 0x52C4);
                u32 angle_p = lk_evmng_getMySubstanceP(mStaffIdx, 0x10035780 /* "angle" */, 3);
                if (angle_p != 0) {
                    angle = gabi::load<s16>(angle_p + 2);
                } else if (demo_mode == 5 /* DEMO_WAIT_TURN */) {
                    angle = LK_FIELD(s16, 0x422);
                } else {
                    angle = shape_angle.y;
                }
                /* HD: a fixed position (static, set on first use) for one cut of event B */
                if (gabi::load<u32>(0x1046D0C8) == 0) {
                    gabi::store<u32>(0x1046D0C8, 1);
                    gabi::store<f32>(0x1046CE14 + 4, 450.0f);
                    gabi::store<f32>(0x1046CE14 + 0, -500.0f);
                    gabi::store<f32>(0x1046CE14 + 8, 560.0f);
                }
                if (evB && current.pos.z > 200.0f && lk_strEq_d(0x100357E8, cut_name)) {
                    LK_setPlayerPosAndAngle(0x1046CE14u, 0x5A00);
                }
                prm0_p = lk_evmng_getMySubstanceP(mStaffIdx, 0x10035790 /* "prm0" */, 3);
                prm1_p = lk_evmng_getMySubstanceP(mStaffIdx, 0x10035798 /* "prm1" */, 3);
                if (LK_demoType == 2 /* TYPE_SYSTEM */) {
                    u32 stick_p = lk_evmng_getMySubstanceP(mStaffIdx, 0x10035788 /* "stick" */, 0);
                    /* HD: event A's cut 0x100357CC ignores the stick value */
                    if (stick_p != 0 && !(evA && lk_strEq_d(0x100357CC, cut_name))) {
                        fcpy_l(gabi::ea(this) + 0x434, stick_p);
                    } else {
                        LK_FIELD(f32, 0x434) = 1.0f;
                    }
                }
                u32 face_p = lk_evmng_getMySubstanceP(mStaffIdx, 0x100357A0 /* "face" */, 3);
                if (face_p != 0) {
                    s32 face = gabi::load<s32>(face_p);
                    if (face == 0) {
                        gabi::call(LK_resetDemoTextureAnime, this);
                    } else if (face == 1) {
                        gabi::call_ptr(gabi::load<u32>(__vtbl + 0x134) /* changeTextureAnime */, this, 0x237, 0x1A7, -1);
                    }
                }
            }
        }
    }
    if (demo_mode == 0x27 /* DEMO_SHIP */ && dComIfGp_getShipActor_l() == 0) demo_mode = 1;
    u16 type = LK_demoType;
    if (type == 4 /* TYPE_START */) {
        if (LK_demoMode == 0xE /* DEMO_KEEP */) {
            s16 timer = LK_FIELD(s16, 0x424);
            if (timer != 0) {
                LK_FIELD(s16, 0x424) = (s16)(timer - 1);
                gabi::Local<cXyz> d;
                cXyz_mi(&current.pos, d, &home.pos);
                gabi::Local<cXyz> xz;
                xz->x = d->x;
                xz->y = 0.0f;
                xz->z = d->z;
                f64 sq = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get());
                if (sq > (f64)90000.0f) {
                    LK_FIELD(s16, 0x424) = 0;
                    lk_evmng_cutEnd(mStaffIdx);
                }
            } else {
                lk_evmng_cutEnd(mStaffIdx);
            }
        }
        return;
    }
    if (type == 1 || (type == 2 && mStaffIdx != -1)) {
        LK_FIELD(s32, 0x428) = prm0_p != 0 ? gabi::load<s32>(prm0_p) : 0;
        LK_FIELD(s32, 0x42C) = prm1_p != 0 ? gabi::load<s32>(prm1_p) : 0;
        if (demo_mode == 4 /* DEMO_INIT_WAIT */ || demo_mode == 0x2C /* DEMO_POS_INIT */) {
            mNormalSpeed = 0.0f;
            gabi::store<f32>(gabi::ea(this) + 0x7628, 0.0f); /* mStts.ClrCcMove() */
            gabi::store<f32>(gabi::ea(this) + 0x7624, 0.0f);
            gabi::store<f32>(gabi::ea(this) + 0x7620, 0.0f);
            speedF = 0.0f;
            LK_setPlayerPosAndAngle(pos_p, angle);
            LK_FIELD(s16, 0x422) = angle; /* mDemo.setMoveAngle */
            if (demo_mode == 4 && LK_FIELD(s32, 0x428) != 0) {
                gabi::call(LK_deleteEquipItem, this, 0);
                s32 p0 = LK_FIELD(s32, 0x428);
                if (p0 == 1 && (gabi::load<u8>(dComIfGs_base_l() + 0x2E) != 0xFF /* checkSwordEquip() */ ||
                                gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2)) {
                    gabi::call(0x023DE0B4 /* setSwordModel */, this, 1);
                    LK_FIELD(s32, 0x428) = 0;
                } else {
                    if (p0 == 1) p0 = LK_FIELD(s32, 0x428);
                    if (p0 == 2) {
                        LK_FIELD(s32, 0x428) = 0;
                        mEquipItem = 0x100; /* daPyItem_NONE */
                    } else {
                        if (p0 == 3) {
                            mEquipItem = 0x100;
                            if (dComIfGp_checkPlayerStatus0_l(0x10000 /* SHIP_RIDE */)) {
                                u32 a = dComIfGp_ea() + 0x5CD8;
                                gabi::store<u32>(a, gabi::load<u32>(a) & ~0x10000u);
                            }
                        }
                        LK_FIELD(s32, 0x428) = 0;
                    }
                }
            }
            if (dComIfGp_checkPlayerStatus0_l(0x10000)) {
                u32 ship = dComIfGp_getShipActor_l();
                if (ship != 0)
                    gabi::call(0x024832E0 /* daShip_c::initStartPos */, ship, &current.pos, (s32)(s16)shape_angle.y);
            }
            LK_demoMode = demo_mode;
        } else if (demo_mode == 0x2B /* DEMO_CUT_ROLL */) {
            gabi::Local<cXyz> d;
            cXyz_mi(gabi::at<cXyz>(pos_p), d, &current.pos);
            s16 old = current.angle.y;
            s16 a = (s16)gabi::call<s32>(0x020195B0 /* cM_atan2s */, (f32)d->x, (f32)d->z);
            LK_demoMode = demo_mode;
            current.angle.y = a;
            shape_angle.y = a;
            m34EC = (s16)(m34EC - (s16)(a - old));
        } else if (demo_mode == 2 /* DEMO_N_WALK */ || demo_mode == 3 /* DEMO_N_DASH */) {
            gabi::Local<cXyz> d;
            cXyz_mi(gabi::at<cXyz>(pos_p), d, &current.pos);
            f32 ratio = fabsf((f32)mNormalSpeed) / (f32)mMaxNormalSpeed;
            f32 x = d->x;
            f32 z = d->z;
            if (ratio < 0.5f) demo_mode = 2;
            gabi::Local<cXyz> xz;
            xz->z = z;
            xz->x = x;
            xz->y = 0.0f;
            f64 sq = gabi::call<f64>(0x028E8DD0 /* PSVECSquareMag */, xz.get());
            bool stop = false;
            if (sq < (f64)100.0f) {
                stop = true;
            } else if (sq < (f64)2500.0f && fabsf((f32)mNormalSpeed) < 0.001f) {
                stop = true;
            }
            if (stop) {
                demo_mode = 1;
                mNormalSpeed = 0.0f;
            } else if ((demo_mode == 2 && sq < (f64)400.0f) || sq < (f64)2500.0f) {
                LK_FIELD(f32, 0x434) = 0.0f; /* mDemo.setStick(0.0f) */
            }
            LK_FIELD(s16, 0x422) = (s16)gabi::call<s32>(0x020195B0 /* cM_atan2s */, x, z);
            LK_demoMode = demo_mode;
        } else if (demo_mode == 5 /* DEMO_WAIT_TURN */ || demo_mode == 0x18 /* DEMO_SURPRISED */) {
            LK_FIELD(s16, 0x422) = angle;
            LK_demoMode = demo_mode;
        } else {
            LK_demoMode = demo_mode;
        }
    } else if (type != 3 /* TYPE_ORIGINAL */) {
        LK_demoMode = demo_mode;
    }
    if ((mModeFlg & 0x80000) || LK_demoMode == 6 /* DEMO_N_TALK */ || LK_demoMode == 8 /* DEMO_E_TALK */) {
        dComIfGp_onPlayerStatus0_l(0x10);
    } else {
        u32 a = dComIfGp_ea() + 0x5CD8;
        gabi::store<u32>(a, gabi::load<u32>(a) & ~0x10u);
    }
    s32 staff = mStaffIdx;
    if (staff != -1) {
        u32 m = (u32)demo_mode;
        if (m == 1 || m == 4 || m == 0xE || m == 0x11 || m == 0x12 || m == 0x17 || m == 0x27 || m == 0x2A || m == 0x2B ||
            m == 0x2C || m == 0x33) {
            lk_evmng_cutEnd(staff);
        }
    }
}
VERIFY(0x023F22FC, &daPy_lk_c::setDemoData);

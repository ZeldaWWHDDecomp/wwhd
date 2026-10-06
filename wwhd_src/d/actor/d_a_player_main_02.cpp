/**
 * d_a_player_main_02.cpp (WWHD)
 * Player - Link (daPy_lk_c), address range #02 (023E0E50..023EDF6F): move animations, the wait
 * and slide procs, ship-ride helpers, the battle/cut proc inits, item-ready checks (bow,
 * boomerang, hookshot, rope), the do-status and talk logic and the item-equip animations.
 *
 * Ported from the GameCube decompilation
 * (zeldaret/tww src/d/actor/d_a_player_main.cpp and its .inc files) to the WWHD layout and code,
 * verified against cking.rpx. "HD:" marks where WWHD's code differs from the GameCube source.
 * Functions of other ranges are called by address (gabi::call), not as C++ members.
 */
#include "d/actor/d_a_player_main.h"
#include <cmath>

/* ---- local bindings (SHARED-CANDIDATE) ---- */
/* play + 0x5CD8 / 0x5CDC: dComIfGp_set/checkPlayerStatus0/1(0, flag) */
static inline void lk_onPlayerStatus0(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CD8;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
static inline void lk_offPlayerStatus0(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CD8;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~flag);
}
static inline u32 lk_checkPlayerStatus0(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CD8) & flag; }
static inline void lk_onPlayerStatus1(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) | flag);
}
static inline void lk_offPlayerStatus1(u32 flag) {
    u32 a = dComIfGp_ea() + 0x5CDC;
    gabi::store<u32>(a, gabi::load<u32>(a) & ~flag);
}
static inline u32 lk_checkPlayerStatus1(u32 flag) { return gabi::load<u32>(dComIfGp_ea() + 0x5CDC) & flag; }
/* play + 0x5B3C: dComIfGp_getShipActor() */
static inline u32 lk_getShipActor() { return gabi::load<u32>(dComIfGp_ea() + 0x5B3C); }
/* play + 0x5B2C: daPy_getPlayerActorClass() (the controlled player) */
static inline u32 lk_getPlayerActorClass() { return gabi::load<u32>(dComIfGp_ea() + 0x5B2C); }
/* play + 0x5BB7: dComIfGp_getDoStatus() */
static inline u8 lk_getDoStatus() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB7); }
/* checkAttentionLock(): dAttention_c::LockonTruth(mpAttention) (024EDFCC) || the attention flag 0x20000000 (+0x20) */
static inline BOOL lk_checkAttentionLock_l(u32 attn) {
    return gabi::call<BOOL>(0x024EDFCC, attn) || (gabi::load<u32>(attn + 0x20) & 0x20000000);
}
#define lk_checkAttentionLock() lk_checkAttentionLock_l((u32)mpAttention)
/* play + 0x5BB5 / 0x5BB7: dComIfGp_set/getRStatus, dComIfGp_setDoStatus */
static inline u8 lk_getRStatus() { return gabi::load<u8>(dComIfGp_ea() + 0x5BB5); }
static inline void lk_setRStatus(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB5, s); }
static inline void lk_setDoStatus(u8 s) { gabi::store<u8>(dComIfGp_ea() + 0x5BB7, s); }
/* 025E1988 seStartSystem(id) */
static inline void lk_seStartSystem(u32 id) { gabi::call(0x025E1988, id); }
/* dBgS (play + 0x12A0) */
static inline BOOL dBgS_ChkPolySafe_l(u32 poly) { return gabi::call<BOOL>(0x02008254, dComIfG_Bgsp(), poly); }
static inline u32 dBgS_GetTriPla_l(u32 poly) {
    return gabi::call<u32>(0x020084C8, dComIfG_Bgsp(), (u32)gabi::load<u16>(poly + 2), (u32)gabi::load<u16>(poly + 0));
}
static inline s32 dBgS_GetSpecialCode_l(u32 poly) { return gabi::call<s32>(0x024EF09C, dComIfG_Bgsp(), poly); }
static inline s32 dBgS_GetWallCode_l(u32 poly) { return gabi::call<s32>(0x024EF080, dComIfG_Bgsp(), poly); }
static inline s32 dBgS_GetGroundCode_l(u32 poly) { return gabi::call<s32>(0x024EF0BC, dComIfG_Bgsp(), poly); }
/* cBgS::LineCross (play + 0x12A0) */
static inline BOOL lk_LineCross(u32 chk) { return gabi::call<BOOL>(0x02008860, dComIfG_Bgsp(), chk); }
/* 0200ECD4 cLib_addCalc(f32* value, f32 target, f32 scale, f32 maxStep, f32 minStep) */
static inline f32 cLib_addCalc_l(be<f32>* v, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    return gabi::call<f32>(0x0200ECD4, v, target, scale, maxStep, minStep);
}
/* J3DModel::getAnmMtx (HD: marks the joint matrices dirty; block at +0x2C, flags +4, matrices +0x10) */
static inline u32 lk_getAnmMtx(J3DModel* m, s32 jnt) {
    u32 blk = gabi::load<u32>(gabi::ea(m) + 0x2C);
    gabi::store<u16>(blk + 4, gabi::load<u16>(blk + 4) | 0x10);
    return gabi::load<u32>(blk + 0x10) + jnt * 0x30;
}
/* daPy_pbCalc (J3DMtxCalc blend, HD): setRatio(idx, r) clears the ratio table of every animation slot.
 * 027F3F94 (matcher: __nw) resolves a relative offset (+0xC header), 027E0174 counts the slots */
static inline void lk_pbCalc_setRatio(u32 pb, s32, f32 r) {
    u32 res = gabi::call<u32>(0x027F3F94, gabi::load<u32>(gabi::load<u32>(pb + 0x80) + 0xAC));
    s32 n = gabi::call<s32>(0x027E0174, res, 0);
    for (s32 i = 0; i < n; i++) {
        gabi::store<f32>(gabi::load<u32>(gabi::load<u32>(pb + 0x7C) + 0x2C) + i * 4, r);
    }
}
/* fsel: a >= 0 ? b : c (NaN -> c) */
static inline f32 fsel_l(f32 a, f32 b, f32 c) { return a >= 0.0f ? b : c; }
/* a float copy through an FPR that the recompiled original keeps bit-exact (no SNaN quieting) */
static inline void fcpy_l(u32 dst, u32 src) { gmem_stf32(dst, gmem_ld32(src)); }

/* daPy_lk_c members in the opaque blocks of the shared header (offsets measured in this range) */
#define LK_FIELD(T, off) (*gabi::at<be<T>>(gabi::ea(this) + (off)))
#define mMaxNormalSpeed LK_FIELD(f32, 0x3C4) /* daPy_py_c (GameCube 0x2A8) */
#define mAcchGndPoly (gabi::ea(this) + 0x8F4) /* mAcch.m_gnd (cBgS_PolyInfo, dBgS_Acch + 0xE8) */
#define mBodyAngleY LK_FIELD(s16, 0x3D2) /* daPy_py_c mBodyAngle.y (GameCube 0x2B6) */
#define LK_underIdx() gabi::load<u16>(gabi::ea(this) + 0x5848) /* m_anm_heap_under[UNDER_MOVE0_e].mIdx */
#define LK_upperIdx() gabi::load<u16>(gabi::ea(this) + 0x5888) /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
#define LK_checkGrabAnime() (LK_upperIdx() == 0x95 || LK_upperIdx() == 0x96)
#define LK_checkNoUpperAnime() (LK_upperIdx() == 0xFFFF)
#define LK_demoMode() gabi::load<u32>(gabi::ea(this) + 0x430) /* mDemo.getDemoMode() (GameCube 0x314) */
/* virtual daPy_lk_c::voiceStart(u32) (vtable slot 0xE4) */
#define LK_voiceStart(n) gabi::call_ptr(gabi::load<u32>(__vtbl + 0xE4), this, (u32)(n))
#define LK_doTrigger() (mItemTrigger & 1)
/* checkNormalSwordEquip(): dComIfGs_getSelectEquip(0) == dItem_SWORD_e || dComIfGp_getMiniGameType() == 2 */
#define LK_checkNormalSwordEquip() \
    (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x2E) == 0x38 || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2)
/* mSwordAnim (mDoExt_bckAnm at 0x4444).changeBckOnly(bck) */
#define LK_swordAnim_changeBckOnly(bck) gabi::call(0x025E871C, gabi::ea(this) + 0x4444, (u32)(bck))
/* v = cXyz::Zero (0x101FFBA8, copied as three words) */
#define LK_cXyzZero_copy(v) do { u32 d_ = gabi::ea(v); \
        gabi::store<u32>(d_ + 0, gabi::load<u32>(0x101FFBA8)); \
        gabi::store<u32>(d_ + 4, gabi::load<u32>(0x101FFBAC)); \
        gabi::store<u32>(d_ + 8, gabi::load<u32>(0x101FFBB0)); } while (0)
/* mAtCyl (dCcD_Cyl at 0x78BC): SetAtAtp (u8 at +0x14) and its cM3dGCyl::SetR (0x020184DC, cylinder at +0x118) */
#define LK_mAtCyl_SetAtAtp(v) gabi::store<u8>(gabi::ea(this) + 0x78D0, (u8)(v))
#define LK_mAtCyl_SetR(r) gabi::call(0x020184DC, gabi::ea(this) + 0x79D4, (f32)(r))
#define mHD827C LK_FIELD(u8, 0x827C) /* HD-only (tail): cleared when a boomerang aim proc starts */
/* checkSwordEquip(): dComIfGs_getSelectEquip(0) != dItem_NONE_e || dComIfGp_getMiniGameType() == 2 */
#define LK_checkSwordEquip() \
    (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x2E) != 0xFF || gabi::load<u8>(dComIfGp_ea() + 0x5CEA) == 2)
#define mHD69E8 LK_FIELD(u8, 0x69E8) /* HD-only (0x69E8): set by a strong jump cut */
#define mHD8260 LK_FIELD(u32, 0x8260) /* HD-only (tail): while non-zero the X/Y/Z item triggers are ignored */
#define mHDAngleY LK_FIELD(s16, 0x827E) /* HD-only (tail): an angle the HD procs copy to shape_angle.y */

/* ---- daPy_lk_c functions of other ranges (by address) ---- */
enum : u32 {
    LK_commonProcInit = 0x023DFDD8,
    LK_setSingleMoveAnime = 0x023E0A04,
    LK_deleteEquipItem = 0x023DC7AC,
    LK_checkShipNotNormalMode = 0x023D69CC,
    LK_endDamageEmitter = 0x023DCA08,
    LK_mtxFollow_makeEmitterColor = 0x023D460C,
    LK_setBlendMoveAnime = 0x023E14A0,
    LK_setShipRidePos = 0x023E287C,
    LK_procGrabWait_init = 0x023E2484,
    LK_procWait_init = 0x023E2FF4,
    LK_checkGrabSpecialHeavyState = 0x023DBBE8,
    LK_setActAnimeUpper = 0x023DE7E8,
    LK_checkRestHPAnime = 0x023DD12C,
    LK_setTextureAnime = 0x023DD768,
    LK_getItemAnimeResource = 0x023DDEEC, /* unnamed by the matcher */
    LK_getDirectionFromAngle = 0x023E3358,
    LK_mtxFollow_makeEmitter = 0x023D457C,
    LK_setItemHeap = 0x023DDF64,
    LK_initModel = 0x023D4BB8,
    LK_seStartOnlyReverb = 0x023DC0F4,
    LK_loadTextureAnimeResource = 0x023DC38C, /* unnamed by the matcher */
    LK_setTextureAnimeResource = 0x023DC110,  /* unnamed by the matcher */
    LK_loadTextureScrollResource = 0x023DC4DC, /* unnamed by the matcher */
    LK_setTextureScrollResource = 0x023DC43C,
    LK_checkBowAnime = 0x023D6A18,
    LK_setMoveAnime = 0x023E0E50,
    LK_resetFootEffect = 0x023DF9C0,
    LK_setHandModel = 0x023E063C,
    LK_checkHeavyStateOn = 0x023DBC24,
    LK_itemButton = 0x023E8C74,
    LK_checkBoomerangAnime = 0x023D69F8,
    LK_dBgS_LinChk_Set = 0x024F1AFC,
    LK_fopAcM_getTalkEventPartner = 0x025D7C6C,
    LK_setTactModel = 0x023DEF04,
    LK_checkBowItem = 0x023D8600,
    LK_checkPhotoBoxItem = 0x023D85E4,
    LK_checkBottleItem = 0x023D85CC,
    LK_checkEquipAnime = 0x023D794C,
    LK_setBottleModel = 0x023DF0FC,
    LK_getAnmData = 0x023E048C,
    LK_getUnderUpperAnime = 0x023E0530,
    LK_setFrameCtrl = 0x023DE788,
    LK_setSeAnime = 0x023E07E4,
    LK_seStartMapInfo = 0x023E0470,
    LK_checkGrabBarrelSearch = 0x023DCF08,
    LK_getDirectionFromShapeAngle = 0x023E4E18,
    LK_freeGrabItem = 0x023DCF8C,
    LK_resetActAnimeUpper = 0x023DC6A4,
    LK_actorKeep_setData = 0x023DE638,
    LK_daArrow_changeArrowMp = 0x02055774, /* unnamed by the matcher: dComIfGs_getMagic() != 0 */
    LK_SafeString_vtbl = 0x10034B24,      /* this TU's sead::SafeString vtable */
};

/* 023E2358 */
BOOL daPy_lk_c::procScope_init(int param_1) {
    WWHD_FUNC(0x023E2358, BOOL, this, param_1);
    if (noResetFlg1() & 0x200) { /* HD: no scope while this flag is set */
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0 /* daPyProc_SCOPE_e */);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setBlendMoveAnime, this, 2.4f /* HD: m_HIO->mBasic.m.field_0xC folded */);
    current.angle.y = shape_angle.y;
    lk_seStartSystem(0x822 /* JA_SE_ITM_SUBMENU_IN_1 */);
    mNoResetFlg0 = mNoResetFlg0 & ~0x80000u; /* offNoResetFlg0(daPyFlg0_SCOPE_CANCEL) */
    u32 bgs = dComIfGp_ea() + 0x12A0;
    if (param_1 == 0x20) {
        gabi::store<u32>(bgs + 0x4A38, gabi::load<u32>(bgs + 0x4A38) | 0x200000); /* daPyStts0_TELESCOPE_LOOK_e */
        return TRUE;
    }
    gabi::store<u32>(bgs + 0x4A3C, gabi::load<u32>(bgs + 0x4A3C) | 8); /* daPyStts1_PICTO_BOX_AIM_e */
    lk_offPlayerStatus1(0x100000); /* HD */
    gabi::call(LK_setSingleMoveAnime, this, 0 /* ANM_WAITS */, 1.1f, 0.0f, -1, 3.0f); /* HD */
    return TRUE;
}
VERIFY(0x023E2358, &daPy_lk_c::procScope_init);

/* 023E2648 */
BOOL daPy_lk_c::procBoomerangCatch_init() {
    WWHD_FUNC(0x023E2648, BOOL, this);
    if (mCurProc == 0x82 /* daPyProc_BOOMERANG_CATCH_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x82);
    gabi::call(LK_setSingleMoveAnime, this, 0x7C /* ANM_BOOMCATCH */, 1.0f, 1.0f, 0xB, 0.0f); /* HD: HIO folded */
    mNoResetFlg0 = mNoResetFlg0 & ~0x20u; /* offNoResetFlg0(daPyFlg0_UNK20) */
    shape_angle.y = mHDAngleY; /* HD */
    return TRUE;
}
VERIFY(0x023E2648, &daPy_lk_c::procBoomerangCatch_init);

/* 023E26EC */
int daPy_lk_c::checkShipRideUseItem(int param_1) {
    WWHD_FUNC(0x023E26EC, int, this, param_1);
    if (lk_checkPlayerStatus0(0x10000 /* daPyStts0_SHIP_RIDE_e */) && lk_getShipActor() != 0) {
        if (param_1 != 0) {
            gabi::call(LK_deleteEquipItem, this, 0);
        }
        if (gabi::call<BOOL>(LK_checkShipNotNormalMode, this)) {
            return 2;
        }
        return 1;
    }
    return 0;
}
VERIFY(0x023E26EC, &daPy_lk_c::checkShipRideUseItem);

/* 023E278C */
void daPy_lk_c::setOldRootQuaternion(s16 param_1, s16 param_2, s16 param_3) {
    WWHD_FUNC(0x023E278C, void, this, param_1, param_2, param_3);
    gabi::Local<u8[0x10]> afStack_28;
    gabi::Local<u8[0x10]> local_38;
    u32 fdata = m_old_fdata;
    if (gabi::load<u8>(fdata) == 0) { /* m_old_fdata->getOldFrameFlg() */
        return;
    }
    u32 q = gabi::load<u32>(fdata + 0x20); /* getOldFrameQuaternion(0) */
    if (param_1 != 0 || param_2 != 0) {
        gabi::call(0x027ED2D0 /* JMAEulerToQuat */, (s32)param_1, (s32)param_2, 0, afStack_28.a);
        for (int i = 0; i < 0x10; i += 4) fcpy_l(local_38.a + i, q + i);
        gabi::call(0x025F2258 /* mDoMtx_QuatConcat */, afStack_28.a, local_38.a, gabi::load<u32>(m_old_fdata + 0x20));
    }
    if (param_3 != 0) {
        gabi::call(0x027ED2D0 /* JMAEulerToQuat */, 0, 0, (s32)param_3, afStack_28.a);
        for (int i = 0; i < 0x10; i += 4) fcpy_l(local_38.a + i, q + i);
        gabi::call(0x025F2258 /* mDoMtx_QuatConcat */, afStack_28.a, local_38.a, gabi::load<u32>(m_old_fdata + 0x20));
    }
}
VERIFY(0x023E278C, &daPy_lk_c::setOldRootQuaternion);

/* 023E2DC4 */
void daPy_lk_c::setShipRidePosUseItem() {
    WWHD_FUNC(0x023E2DC4, void, this);
    if (lk_checkPlayerStatus0(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
        gabi::call(LK_setShipRidePos, this, 1);
        shape_angle.y = (s16)(shape_angle.y - 0x4000);
        current.angle.y = shape_angle.y;
    }
}
VERIFY(0x023E2DC4, &daPy_lk_c::setShipRidePosUseItem);

/* 023E2E18 */
void daPy_lk_c::initShipRideUseItem(int param_1, int param_2) {
    WWHD_FUNC(0x023E2E18, void, this, param_1, param_2);
    if (param_1 == 0) {
        return;
    }
    gabi::store<u8>(lk_getShipActor() + 0x636, 2); /* daShip_c::setPaddleMove() */
    gravity = 0.0f;
    speed.y = 0.0f;
    if (param_1 == 1) {
        setOldRootQuaternion(0, 0x4000, 0);
    }
    lk_onPlayerStatus0(0x10000 /* daPyStts0_SHIP_RIDE_e */);
    if (param_2 == 2) {
        setShipRidePosUseItem();
    } else if (param_2 == 1) {
        gabi::call(LK_setShipRidePos, this, 1);
    } else {
        gabi::call(LK_setShipRidePos, this, 0);
    }
    mModeFlg = mModeFlg | 0x2000; /* onModeFlg(ModeFlg_IN_SHIP) */
}
VERIFY(0x023E2E18, &daPy_lk_c::initShipRideUseItem);

/* 023E2EFC */
BOOL daPy_lk_c::procControllWait_init() {
    WWHD_FUNC(0x023E2EFC, BOOL, this);
    if (mCurProc == 3 /* daPyProc_CONTROLL_WAIT_e */) {
        return FALSE;
    }
    int iVar3 = checkShipRideUseItem(1);
    gabi::call(LK_commonProcInit, this, 3);
    mNormalSpeed = 0.0f;
    gabi::store<u8>(0x101CEF18, 0); /* daPy_matAnm_c::offMabaFlg() */
    gabi::store<u8>(0x101CEF19, 1);
    gabi::call(LK_setSingleMoveAnime, this, 0xC3 /* ANM_NENRIKI */, 1.0f, 0.0f, -1, 2.4f);
    current.angle.y = shape_angle.y;
    gabi::call(LK_deleteEquipItem, this, 0);
    initShipRideUseItem(iVar3, 2);
    return TRUE;
}
VERIFY(0x023E2EFC, &daPy_lk_c::procControllWait_init);

/* 023E32AC */
BOOL daPy_lk_c::changeWaitProc() {
    WWHD_FUNC(0x023E32AC, BOOL, this);
    u16 upper = gabi::load<u16>(gabi::ea(this) + 0x5888); /* m_anm_heap_upper[UPPER_MOVE2_e].mIdx */
    if (upper == 0x95 || upper == 0x96) { /* checkGrabAnime() */
        return gabi::call<BOOL>(LK_procGrabWait_init, this);
    } else if (upper == 0x33 /* dRes_INDEX_LKANM_BCK_BOOMCATCH_e */) {
        return procBoomerangCatch_init();
    } else if (lk_getPlayerActorClass() != gabi::ea(this)) {
        return procControllWait_init();
    } else {
        return gabi::call<BOOL>(LK_procWait_init, this);
    }
}
VERIFY(0x023E32AC, &daPy_lk_c::changeWaitProc);

/* 023E47C4 daPy_mtxPosFollowEcallBack_c::makeEmitterColor (unnamed by the matcher) */
static void daPy_mtxPosFollowEcallBack_makeEmitterColor(u32 cb, u32 id, u32 mtx, u32 pos, u32 angle, u32 prm, u32 env) {
    WWHD_FUNC(0x023E47C4, void, cb, id, mtx, pos, angle, prm, env);
    gabi::store<u32>(cb + 0xC, angle); /* mpAngle */
    gabi::call(LK_mtxFollow_makeEmitterColor, cb, id, mtx, pos, prm, env);
}
VERIFY(0x023E47C4, daPy_mtxPosFollowEcallBack_makeEmitterColor);

/* 023E4A58 */
u32 daPy_lk_c::getSlidePolygon() {
    WWHD_FUNC(0x023E4A58, u32, this);
    if (!(mNoResetFlg0 & 0xA0000000u) && (mAcch.m_flags & dBgS_Acch::GROUND_HIT) && dBgS_ChkPolySafe_l(mAcchGndPoly)) {
        u32 pla = dBgS_GetTriPla_l(mAcchGndPoly);
        s32 special = dBgS_GetSpecialCode_l(mAcchGndPoly);
        if (pla != 0 && dBgS_GetGroundCode_l(mAcchGndPoly) != 8 && special == 1) {
            return pla;
        }
    }
    return 0;
}
VERIFY(0x023E4A58, &daPy_lk_c::getSlidePolygon);

/* 023E4B38 */
BOOL daPy_lk_c::procSlideFront_init(s16 param_1) {
    WWHD_FUNC(0x023E4B38, BOOL, this, param_1);
    if (mCurProc == 0x1A /* daPyProc_SLIDE_FRONT_e */ || mCurProc == 0x1B /* daPyProc_SLIDE_BACK_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x1A);
    m3526 = 8;
    current.angle.y = param_1;
    gabi::call(LK_setSingleMoveAnime, this, 0x35 /* ANM_SLIDEF */, 0.3f, 2.0f, 9, 5.0f); /* HD: HIO folded */
    mMaxNormalSpeed = 30.0f;
    mFootEffectPosType = 4;
    lk_onPlayerStatus1(0x100 /* daPyStts1_UNK100_e */);
    return TRUE;
}
VERIFY(0x023E4B38, &daPy_lk_c::procSlideFront_init);

/* 023E4C04 */
BOOL daPy_lk_c::procSlideBack_init(s16 param_1) {
    WWHD_FUNC(0x023E4C04, BOOL, this, param_1);
    if (mCurProc == 0x1B /* daPyProc_SLIDE_BACK_e */ || mCurProc == 0x1A /* daPyProc_SLIDE_FRONT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x1B);
    m3526 = 8;
    current.angle.y = param_1;
    gabi::call(LK_setSingleMoveAnime, this, 0x37 /* ANM_SLIDEB */, 0.3f, 0.0f, 9, 5.0f); /* HD: HIO folded */
    mMaxNormalSpeed = 30.0f;
    mFootEffectPosType = 3;
    lk_onPlayerStatus1(0x200 /* daPyStts1_UNK200_e */);
    return TRUE;
}
VERIFY(0x023E4C04, &daPy_lk_c::procSlideBack_init);

/* 023E4CD0 */
int daPy_lk_c::changeSlideProc() {
    WWHD_FUNC(0x023E4CD0, int, this);
    u32 pla = getSlidePolygon();
    if (pla == 0) {
        m3526 = 8;
        return FALSE;
    }
    s16 sVar4 = cM_atan2s(gabi::load<f32>(pla + 0), gabi::load<f32>(pla + 8));
    if (m34C3 != 0 && !lk_checkPlayerStatus0(0x2000)) { /* HD: not while daPyStts0_SUBJECT_e */
        cLib_addCalc_l(&mNormalSpeed, 0.0f, 0.4f, 5.0f, 1.0f);
        m3526 = (s16)(m3526 - 1);
        if (mNormalSpeed > 5.0f || m3526 > 0) {
            return FALSE;
        }
    }
    if (cLib_distanceAngleS(sVar4, shape_angle.y) < 0x3800 /* HD: HIO folded */) {
        return procSlideFront_init(sVar4);
    }
    return procSlideBack_init(sVar4);
}
VERIFY(0x023E4CD0, &daPy_lk_c::changeSlideProc);

/* 023E4E2C */
void daPy_lk_c::endFlameDamageEmitter() {
    WWHD_FUNC(0x023E4E2C, void, this);
    if (gabi::load<u16>(0x101CEF16) != 0) { /* !daPy_dmEcallBack_c::checkFlame() (HD: a global) */
        return;
    }
    if (current.pos.y < mWaterY - 10.0f) {
        gabi::Local<cXyz> pos;
        for (int i = 0; i < 4; i++) {
            u32 mtx = gabi::load<u32>(gabi::ea(this) + 0x67CC + i * 0xC + 8); /* mDmEcallBack[i].getMtx() */
            fcpy_l(pos.a + 0, mtx + 0xC); /* mDoMtx_multVecZero */
            fcpy_l(pos.a + 8, mtx + 0x2C);
            fcpy_l(pos.a + 4, gabi::ea(&mWaterY)); /* pos.y = mWaterY */
            dComIfGp_particle_set(0x35A /* ID_AK_JN_EVAPORATION00 */, pos);
        }
    }
    gabi::call(LK_endDamageEmitter, this);
}
VERIFY(0x023E4E2C, &daPy_lk_c::endFlameDamageEmitter);

/* 023E2484 */
BOOL daPy_lk_c::procGrabWait_init() {
    WWHD_FUNC(0x023E2484, BOOL, this);
    if (mCurProc == 0x73 /* daPyProc_GRAB_WAIT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x73);
    if (gabi::call<BOOL>(LK_checkGrabSpecialHeavyState, this)) {
        gabi::call(LK_setSingleMoveAnime, this, 0x69 /* ANM_GRABWAITB */, 1.2f, 0.0f, -1, 3.0f); /* HD: HIO folded */
        if (!LK_checkGrabAnime()) {
            gabi::call(LK_setActAnimeUpper, this, 0x96 /* dRes_INDEX_LKANM_BCK_GRABWAITB_e */, 2 /* UPPER_MOVE2_e */, 0.0f, 0.0f, -1, -1.0f);
        }
    } else {
        gabi::call(LK_setSingleMoveAnime, this, 0x68 /* ANM_GRABWAIT */, 1.0f, 0.0f, -1, 3.0f);
        if (!LK_checkGrabAnime()) {
            gabi::call(LK_setActAnimeUpper, this, 0x95 /* dRes_INDEX_LKANM_BCK_GRABWAIT_e */, 2, 0.0f, 0.0f, -1, -1.0f);
        }
    }
    lk_pbCalc_setRatio(m_pbCalc[1] /* PART_UPPER_e */, 2, 0.0f);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    m35A0 = 0.0f;
    mProcVar6 = !(m35D8 > -29.0f); /* m35D8 <= -29.0f (NaN: 1) */
    return TRUE;
}
VERIFY(0x023E2484, &daPy_lk_c::procGrabWait_init);

/* 023E287C */
void daPy_lk_c::setShipRidePos(int param_0) {
    WWHD_FUNC(0x023E287C, void, this, param_0);
    u32 ship = lk_getShipActor();
    if (ship == 0) {
        return;
    }
    if (gabi::load<u32>(ship + 0x644) & 0x4000) { /* ship->checkJumpOkFlg() */
        gabi::store<u8>(dComIfGp_ea() + 0x5BB5, 0x12); /* dComIfGp_setRStatus(dActStts_JUMP_e) */
    }
    fopAc_ac_c* sh = gabi::at<fopAc_ac_c>(ship);
    if (param_0 != 0) {
        shape_angle.y = sh->shape_angle.y;
        current.angle.y = shape_angle.y;
    }
    if (mCurProc == 0xD3) {
        m353C = sh->shape_angle.x;
        m353E = sh->shape_angle.z;
        m3540 = 0;
        m3542 = 0;
    } else {
        m353C = (s16)(m353C + m3540);
        s16 v = (s16)gabi::ftoi(gabi::fmadds((f32)(s16)(sh->shape_angle.x - m353C), 0.75f, (f32)m3540));
        if (v > 0x400) { /* HD: clamped to +-0x400 */
            v = 0x400;
        } else if (v < -0x400) {
            v = -0x400;
        }
        m3540 = v;
        cLib_addCalcAngleS(&m3540, 0, 5, 0x200, 0x100);
        m353E = (s16)(m353E + m3542);
        s16 w = (s16)(m3542 + ((s16)(sh->shape_angle.z - m353E) >> 1));
        if (w > 0x400) {
            w = 0x400;
        } else if (w < -0x400) {
            w = -0x400;
        }
        m3542 = w;
        cLib_addCalcAngleS(&m3542, 0, 5, 0x100, 0x30);
    }
    u32 off;
    if (mCurProc == 0xD7) {
        off = 0x10034F9C; /* l_ship_offset3 */
    } else if (mCurProc == 0xAE && mProcVar4 != 0) {
        off = 0x10034F90; /* l_ship_offset2 */
    } else {
        off = 0x10034F84; /* l_ship_offset */
    }
    u32 model = gabi::load<u32>(gabi::load<u32>(ship + 0x3B4) + 0x90); /* ship->getBodyMtx() */
    PSMTXMultVec(gabi::at<Mtx34>(model ? model + 0xC8 : 0), gabi::at<cXyz>(off), &current.pos);
    if (gabi::call<BOOL>(LK_checkShipNotNormalMode, this)) {
        mBodyAngleY = 0;
        return;
    }
    gabi::Local<be<f32>> sp08;
    gabi::Local<cXyz> local_38;
    gabi::call(0x0257E1B8 /* dKyw_get_AllWind_vec */, &current.pos, local_38.a, sp08.a);
    f32 fVar2 = gabi::fmadds(*sp08, 30.0f, sh->speedF) / 30.0f;
    if (fVar2 > 1.0f) {
        fVar2 = 1.0f;
    }
    mFrameCtrlUpper[2].setRate(gabi::fmadds(0.8f, fVar2, 0.2f)); /* HD: HIO folded */
    if (gabi::load<u32>(ship + 0x644) & 0x40) { /* ship->getJumpFlg() */
        if (gabi::load<f32>(ship + 0x6C8) > 0.6f) { /* ship->getJumpRate() */
            LK_voiceStart(6);
        } else if (gabi::load<f32>(ship + 0x6C8) > 0.2f) {
            LK_voiceStart(7);
        }
        gabi::call(LK_setSingleMoveAnime, this, 0x9F /* ANM_SHIP_JUMP1 */, 1.0f, 0.0f, -1, 20.0f);
    }
    if (gabi::load<u32>(ship + 0x644) & 0x80) { /* ship->getLandFlg() */
        gabi::call(LK_setSingleMoveAnime, this, 0xA0 /* ANM_SHIP_JUMP2 */, 1.0f, 3.0f, -1, 3.0f);
    }
    if (LK_underIdx() == 0xF4 /* dRes_INDEX_LKANM_BCK_SHIP_JUMP2_e */) {
        if (mFrameCtrlUnder[0].getRate() < 0.01f) {
            if (gabi::load<u32>(ship + 0x644) & 1) { /* ship->getFlyFlg() */
                gabi::call(LK_setSingleMoveAnime, this, 0x9F /* ANM_SHIP_JUMP1 */, 1.0f, 0.0f, -1, 20.0f);
            } else {
                gabi::call(LK_setSingleMoveAnime, this, 0x7D /* ANM_VOYAGE1 */, 1.0f, 0.0f, -1, 5.0f);
            }
        } else {
            mFrameCtrlUpper[2].setRate(0.0f);
        }
    }
    s16 sVar4;
    if (LK_underIdx() == 0x11B /* dRes_INDEX_LKANM_BCK_VOYAGE1_e */) {
        sVar4 = (s16)gabi::ftoi(gabi::load<f32>(ship + 0x6C0) * -5325.0f); /* -HIO * ship->getTillerAngleRate() */
    } else {
        sVar4 = 0;
    }
    cLib_addCalcAngleS(&mBodyAngleY, sVar4, 4, 0x400, 0x80);
}
VERIFY(0x023E287C, &daPy_lk_c::setShipRidePos);

/* 023E2FF4 */
BOOL daPy_lk_c::procWait_init() {
    WWHD_FUNC(0x023E2FF4, BOOL, this);
    if (LK_demoMode() == 0x2A /* daPy_demo_c::DEMO_KM_WAIT_e */ && gabi::load<u16>(gabi::ea(this) + 0x65E6) == 0xFFFF) {
        /* virtual changeTextureAnime(dRes_INDEX_LKANM_BTP_TMABACC_e, dRes_INDEX_LKANM_BTK_TEUR_e, -1) */
        gabi::call_ptr(gabi::load<u32>(__vtbl + 0x134), this, 0x231, 0x188, -1);
        LK_voiceStart(38);
        mProcVar0 = 20;
    }
    if (mCurProc == 4 /* daPyProc_WAIT_e */) {
        return FALSE;
    }
    lk_offPlayerStatus1(0x100000); /* HD */
    if (!gabi::load<u8>(dComIfGp_ea() + 0x5292) /* !dComIfGp_event_runCheck() */ && mCurProc == 5 /* daPyProc_FREE_WAIT_e */ &&
        mFrameCtrlUnder[0].getRate() > 0.01f && LK_checkNoUpperAnime()) {
        return FALSE;
    }
    int iVar3 = gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0x3C), this); /* virtual checkPlayerGuard() */
    gabi::call(LK_commonProcInit, this, 4);
    mNormalSpeed = 0.0f;
    int iVar4 = gabi::call<BOOL>(LK_checkRestHPAnime, this);
    if (iVar4 != 0 && iVar3 == 0) {
        s32 uVar2 = gabi::load<u16>(gabi::ea(this) + 0x65D0) == 0x234; /* m_tex_anm_heap.mIdx == mTexAnmIndexTable[daPyFace_TMABAF].mBtpIdx */
        u16 uVar1 = m3530;
        gabi::call(LK_setSingleMoveAnime, this, 0x1D /* ANM_WAITATOB */, 0.6f, 0.0f, 0xC, 6.0f);
        if (uVar2 == 0) {
            mModeFlg = (mModeFlg | 0x400) & ~0x100u;
        } else {
            gabi::call(LK_setTextureAnime, this, 0xE, (u32)uVar1);
        }
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f);
    }
    current.angle.y = shape_angle.y;
    mDirection = 4; /* DIR_NONE */
    m35A0 = 0.0f;
    f32 dVar5 = cM_rndF(150.0f);
    mProcVar1 = (s16)gabi::ftoi(dVar5 + 300.0f);
    return TRUE;
}
VERIFY(0x023E2FF4, &daPy_lk_c::procWait_init);

/* 023E4F04 */
BOOL daPy_lk_c::procBtJump_init(fopEn_enemy_c* enemy) {
    WWHD_FUNC(0x023E4F04, BOOL, this, enemy);
    gabi::Local<cXyz> local_34;
    cXyz_mi(&enemy->current.pos, local_34, &current.pos);
    gabi::call(LK_commonProcInit, this, 0x5D /* daPyProc_BT_JUMP_e */);
    gravity = -5.0f; /* HD: HIO folded */
    gabi::call(LK_setSingleMoveAnime, this, 0x6C /* ANM_MJMP */, 1.0f, 4.0f, -1, 1.0f);
    f32 fVar1 = gabi::fadds_ppc(enemy->mBtHeight, local_34->y);
    if (fVar1 < 30.0f) {
        fVar1 = 30.0f;
    }
    f32 dVar5 = std_sqrtf((-2.0f * fVar1) / gravity);
    m35A0 = -(dVar5 * gravity);
    gabi::Local<cXyz> xz; /* local_34.absXZ() */
    fcpy_l(xz.a + 0, local_34.a + 0);
    xz->y = 0.0f;
    fcpy_l(xz.a + 8, local_34.a + 8);
    m35A4 = std_sqrtf(PSVECSquareMag(xz)) / dVar5;
    s16 ang = fopAcM_searchActorAngleY(this, enemy);
    shape_angle.y = ang;
    current.angle.y = ang;
    lk_onPlayerStatus0(2 /* daPyStts0_UNK2_e */);
    mProcVar0 = 0;
    m370C.x = gabi::fnmsubs(enemy->mBtBodyR, cM_ssin(enemy->shape_angle.y), enemy->current.pos.x);
    fcpy_l(gabi::ea(&m370C.y), gabi::ea(&enemy->current.pos.y));
    m370C.z = gabi::fnmsubs(enemy->mBtBodyR, cM_scos(enemy->shape_angle.y), enemy->current.pos.z);
    mProcVar6 = 1; /* HD: m_HIO->mBattle.mBJump.m.field_0x18 <= 0.0f folded */
    fcpy_l(gabi::ea(&speed.y), gabi::ea(&m35A0));
    fcpy_l(gabi::ea(&mNormalSpeed), gabi::ea(&m35A4));
    LK_voiceStart(6);
    mNoResetFlg0 = mNoResetFlg0 & ~0x40000u; /* offNoResetFlg0(daPyFlg0_NO_FALL_VOICE) */
    endFlameDamageEmitter();
    return TRUE;
}
VERIFY(0x023E4F04, &daPy_lk_c::procBtJump_init);

/* 023E50E0 */
BOOL daPy_lk_c::procBtRoll_init(fopEn_enemy_c* enemy) {
    WWHD_FUNC(0x023E50E0, BOOL, this, enemy);
    gabi::call(LK_commonProcInit, this, 0x60 /* daPyProc_BT_ROLL_e */);
    mNormalSpeed = 0.0f;
    gabi::Local<cXyz> local_38;
    s16 ang = enemy->shape_angle.y;
    f32 r = enemy->mBtBodyR;
    local_38->x = gabi::fmuls_ppc(gabi::fadds_ppc(gabi::fnmsubs(r, cM_ssin(ang), enemy->current.pos.x), current.pos.x), 0.5f);
    local_38->y = gabi::fmuls_ppc(gabi::fadds_ppc(enemy->current.pos.y, current.pos.y), 0.5f);
    local_38->z = gabi::fmuls_ppc(gabi::fadds_ppc(gabi::fnmsubs(r, cM_scos(ang), enemy->current.pos.z), current.pos.z), 0.5f);
    int anm;
    if (mDirection == 2 /* DIR_LEFT */) {
        mProcVar2 = -0x7FF0;
        anm = 0x6E; /* ANM_MROLLL */
    } else {
        mProcVar2 = 0x7FF0;
        anm = 0x6F; /* ANM_MROLLR */
    }
    gabi::call(LK_setSingleMoveAnime, this, anm, 0.8f, 4.0f, 0x16, 5.0f); /* HD: HIO folded */
    gabi::Local<cXyz> local_2c;
    cXyz_mi(&current.pos, local_2c, local_38);
    gabi::Local<cXyz> xz; /* local_2c.absXZ() */
    fcpy_l(xz.a + 0, local_2c.a + 0);
    xz->y = 0.0f;
    fcpy_l(xz.a + 8, local_2c.a + 8);
    m35A0 = std_sqrtf(PSVECSquareMag(xz));
    mProcVar3 = cM_atan2s(local_2c->x, local_2c->z);
    shape_angle.y = fopAcM_searchActorAngleY(this, enemy);
    lk_onPlayerStatus0(2 /* daPyStts0_UNK2_e */);
    mFootEffectPosType = 4;
    LK_voiceStart(7);
    endFlameDamageEmitter();
    return TRUE;
}
VERIFY(0x023E50E0, &daPy_lk_c::procBtRoll_init);

/* 023E533C */
BOOL daPy_lk_c::procBtVerticalJump_init(fopEn_enemy_c* enemy) {
    WWHD_FUNC(0x023E533C, BOOL, this, enemy);
    gabi::call(LK_commonProcInit, this, 0x62 /* daPyProc_BT_VERTICAL_JUMP_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x72 /* ANM_MSTEPOVER */, 0.2f, 2.0f, 5, 6.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    gravity = -9.0f;
    speed.y = 70.0f;
    mNoResetFlg0 = mNoResetFlg0 & ~0x40000u; /* offNoResetFlg0(daPyFlg0_NO_FALL_VOICE) */
    current.angle.y = shape_angle.y;
    LK_voiceStart(6);
    lk_onPlayerStatus0(2 /* daPyStts0_UNK2_e */);
    endFlameDamageEmitter();
    return TRUE;
}
VERIFY(0x023E533C, &daPy_lk_c::procBtVerticalJump_init);

/* 023E5400 */
BOOL daPy_lk_c::changeSpecialBattle() {
    WWHD_FUNC(0x023E5400, BOOL, this);
    if (mpAttnActorLockOn.get() != nullptr && (lk_getDoStatus() == 0x1A /* dActStts_PARRY_e */ || m34C5 == 5)) {
        fopEn_enemy_c* enemy = gabi::at<fopEn_enemy_c>(gabi::ea(mpAttnActorLockOn.get()));
        /* HD: the frame test is written as !(start > now) */
        if (m34C5 == 5 || (LK_doTrigger() /* spBattleTrigger() */ && !(enemy->mBtStartFrame > enemy->mBtNowFrame))) {
            u8 type = enemy->mBtAttackType;
            if (type == 1) {
                return procBtJump_init(enemy);
            }
            if (type == 2) {
                return procBtRoll_init(enemy);
            }
            if (type == 3) {
                return procBtVerticalJump_init(enemy);
            }
            if (type == 4) {
                setNoResetFlg1(noResetFlg1() | 0x20000000); /* onNoResetFlg1(daPyFlg1_LAST_COMBO_WAIT) */
                return TRUE;
            }
        }
    }
    return FALSE;
}
VERIFY(0x023E5400, &daPy_lk_c::changeSpecialBattle);

/* 023E5518 */
int daPy_lk_c::getCutDirection() {
    WWHD_FUNC(0x023E5518, int, this);
    if (!(mStickDistance > 0.05f)) { /* HD: NaN gives DIR_NONE */
        return 5; /* DIR_NONE + 1 */
    }
    s16 angle;
    if (mpAttnActorLockOn.get() != nullptr) {
        s16 toActor = fopAcM_searchActorAngleY(this, mpAttnActorLockOn);
        angle = (s16)(m34E8 - toActor);
    } else {
        angle = (s16)(m34E8 - m34DE);
    }
    return gabi::call<int>(LK_getDirectionFromAngle, this, (s32)angle) + 1;
}
VERIFY(0x023E5518, &daPy_lk_c::getCutDirection);

/* 023E55A4 */
void daPy_lk_c::setBlurPosResource(u16 index) {
    WWHD_FUNC(0x023E55A4, void, this, index);
    /* HD: the blur positions are taken from the loaded "LkAnm" archive (GameCube: JKRReadIdxResource into a buffer) */
    u32 res = gabi::ea(dComIfG_getObjectRes(STR(0x1003561C), index, LK_SafeString_vtbl));
    gabi::store<u32>(mpSwBlur + 0xAC, res); /* mpSwBlur->mpPosBuffer */
}
VERIFY(0x023E55A4, &daPy_lk_c::setBlurPosResource);

/* 023E5600 */
void daPy_lk_c::setAtParam(u32 type, int atp, int spl, u8 se, u8 hitMark, u8 cutType, f32 radius) {
    WWHD_FUNC(0x023E5600, void, this, type, atp, spl, se, hitMark, cutType, radius);
    if (type == 2 /* AT_TYPE_SWORD */) {
        if (noResetFlg1() & 0x8000) { /* checkNoResetFlg1(daPyFlg1_SOUP_POWER_UP) */
            atp *= 2;
        }
    } else {
        gabi::store<u8>(gabi::ea(this) + 0x3AD, 0); /* mCutCount */
    }
    u32 cps = gabi::ea(this) + 0x7B1C; /* mAtCps[3] (0x138 each) */
    for (int i = 0; i < 3; i++, cps += 0x138) {
        gabi::store<u8>(cps + 0x6E, hitMark); /* SetAtHitMark */
        gabi::store<u32>(cps + 0x10, type);   /* SetAtType */
        gabi::store<u8>(cps + 0x6C, se);      /* SetAtSe */
        gabi::store<f32>(cps + 0x134, radius); /* SetR */
        gabi::store<u8>(cps + 0x14, (u8)atp); /* SetAtAtp */
        gabi::store<u8>(cps + 0x6F, (u8)spl); /* SetAtSpl */
    }
    gabi::store<u8>(gabi::ea(this) + 0x3AC, cutType); /* mCutType */
    setResetFlg0(resetFlg0() & ~0x08000000u); /* offResetFlg0(daPyRFlg0_NOT_ATTACKING) */
    mNoResetFlg0 = mNoResetFlg0 & ~0x10000000u; /* offNoResetFlg0(daPyFlg0_UNK10000000) */
}
VERIFY(0x023E5600, &daPy_lk_c::setAtParam);

/* 023E5678 */
void daPy_lk_c::setFinishCutAtParam(u8 cutType) {
    WWHD_FUNC(0x023E5678, void, this, cutType);
    if (LK_checkNormalSwordEquip()) {
        m35FC = 1.5f; /* HD: HIO folded */
        setAtParam(2 /* AT_TYPE_SWORD */, 2, 1 /* dCcG_At_Spl_UNK1 */, 1 /* dCcG_SE_UNK1 */, 0xF /* dCcG_AtHitMark_Big_e */, cutType, 20.0f);
    } else {
        m35FC = 2.5f;
        setAtParam(2, 4, 1, 1, 0xF, cutType, 30.0f);
    }
}
VERIFY(0x023E5678, &daPy_lk_c::setFinishCutAtParam);

/* 023E5B0C */
void daPy_lk_c::setNormalCutAtParam(u8 cutType) {
    WWHD_FUNC(0x023E5B0C, void, this, cutType);
    if (LK_checkNormalSwordEquip()) {
        m35FC = 1.5f; /* HD: HIO folded */
        setAtParam(2 /* AT_TYPE_SWORD */, 1, 0 /* dCcG_At_Spl_UNK0 */, 1 /* dCcG_SE_UNK1 */, 0xD /* dCcG_AtHitMark_Nrm_e */, cutType, 20.0f);
    } else {
        m35FC = 2.5f;
        setAtParam(2, 2, 0, 1, 0xD, cutType, 30.0f);
    }
}
VERIFY(0x023E5B0C, &daPy_lk_c::setNormalCutAtParam);

/* the finishing and extra cuts (EA/EB/ExB/ExA/Kesa): common head up to the SWORD_SWING status (HD: HIO folded;
 * m35EC is the start frame) */
#define LK_PROC_CUT_X_HEAD(proc, anm, rate, start, end, morf, bckA, bckMS, pv0) \
    gabi::call(LK_commonProcInit, this, proc); \
    gabi::call(LK_setSingleMoveAnime, this, anm, rate, start, end, morf); \
    if (LK_checkNormalSwordEquip()) { \
        LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, bckA)); \
    } else { \
        LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, bckMS)); \
    } \
    m35EC = start; \
    LK_cXyzZero_copy(&m3700); \
    m34C2 = 1; \
    pv0; \
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */ \
    LK_voiceStart(1); \
    lk_onPlayerStatus0(0x8000 /* daPyStts0_SWORD_SWING_e */);

/* 023E573C */
BOOL daPy_lk_c::procCutEA_init() {
    WWHD_FUNC(0x023E573C, BOOL, this);
    LK_PROC_CUT_X_HEAD(0x45 /* daPyProc_CUT_EA_e */, 0x22 /* ANM_CUTEA */, 1.0f, 4.0f, 0x13, 2.0f, 0x44 /* dRes_INDEX_LKANM_BCK_CUTEAA_e */,
                       0x45 /* dRes_INDEX_LKANM_BCK_CUTEAMS_e */, mProcVar0 = 0)
    setBlurPosResource(0x27F /* dRes_INDEX_LKANM__CUTEA_POS_e */);
    setFinishCutAtParam(6 /* CUT_TYPE_CUT_EA */);
    m34C4 = 0;
    m3522 = 0;
    return TRUE;
}
VERIFY(0x023E573C, &daPy_lk_c::procCutEA_init);

/* 023E5924 */
BOOL daPy_lk_c::procCutEB_init() {
    WWHD_FUNC(0x023E5924, BOOL, this);
    LK_PROC_CUT_X_HEAD(0x46 /* daPyProc_CUT_EB_e */, 0x23 /* ANM_CUTEB */, 0.9f, 4.0f, 0x13, 2.0f, 0x47 /* dRes_INDEX_LKANM_BCK_CUTEBA_e */,
                       0x48 /* dRes_INDEX_LKANM_BCK_CUTEBMS_e */, mProcVar0 = 0)
    setBlurPosResource(0x280 /* dRes_INDEX_LKANM__CUTEB_POS_e */);
    setFinishCutAtParam(7 /* CUT_TYPE_CUT_EB */);
    m34C4 = 0;
    m3522 = 0;
    return TRUE;
}
VERIFY(0x023E5924, &daPy_lk_c::procCutEB_init);

/* HD inline anm setFrame: frame (+4), the animation's frame (through the pointer at +ptrOff) and a
 * functor at +fnOff ({f32 result, f32 a, f32 b, ?, fn, arg}) re-evaluated, then 027DF40C(&functor)
 * (same helper as d_a_dr2.cpp; SHARED-CANDIDATE) */
static inline void lk_anm_setFrame_hd(u32 a, f32 frame, u32 ptrOff, u32 fnOff) {
    gabi::store<f32>(a + 4, frame);
    gabi::store<f32>(gabi::load<u32>(a + ptrOff), frame);
    u32 fn = gabi::load<u32>(a + fnOff);
    u32 target = gabi::load<u32>(fn + 0x10);
    f32 p3 = gabi::load<f32>(fn + 8);
    f32 p2 = gabi::load<f32>(fn + 4);
    u32 arg = gabi::load<u32>(fn + 0x14);
    f32 res = gabi::call_ptr<f32>(target, arg, frame, p2, p3);
    gabi::store<f32>(fn, res);
    gabi::call(0x027DF40C, a + fnOff);
}

/* the normal cuts F/R/A/L: common head up to voiceStart(0) (HD: HIO folded) */
#define LK_PROC_CUT_HEAD(proc, anm, end, bckA, bckMS, extra) \
    gabi::call(LK_commonProcInit, this, proc); \
    gabi::call(LK_setSingleMoveAnime, this, anm, 1.2f, 4.0f, end, 2.5f); \
    if (LK_checkNormalSwordEquip()) { \
        LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, bckA)); \
    } else { \
        LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, bckMS)); \
    } \
    m35EC = 4.0f; \
    extra; \
    LK_cXyzZero_copy(&m3700); \
    m34C2 = 1; \
    LK_voiceStart(0);

/* 023E5BD0 */
BOOL daPy_lk_c::procCutF_init(s16 param_0) {
    WWHD_FUNC(0x023E5BD0, BOOL, this, param_0);
    LK_PROC_CUT_HEAD(0x42 /* daPyProc_CUT_F_e */, 0x1F /* ANM_CUTF */, 0x13, 0x4A /* dRes_INDEX_LKANM_BCK_CUTFA_e */,
                     0x4B /* dRes_INDEX_LKANM_BCK_CUTFMS_e */,
                     lk_anm_setFrame_hd(gabi::ea(this) + 0x4980, 4.0f, 0x10, 0x18); /* mpCutfBpk->setFrame */
                     lk_anm_setFrame_hd(gabi::ea(this) + 0x49F4, 4.0f, 0x68, 0x10)) /* mpCutfBtk->setFrame */
    mProcVar2 = param_0;
    m351E = m34DC;
    m3522 = 2;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    lk_onPlayerStatus0(0x8000 /* daPyStts0_SWORD_SWING_e */);
    setNormalCutAtParam(2 /* CUT_TYPE_CUT_F */); /* HD: no setBlurPosResource */
    return TRUE;
}
VERIFY(0x023E5BD0, &daPy_lk_c::procCutF_init);

/* 023E5D94 */
BOOL daPy_lk_c::procCutR_init(s16 param_0) {
    WWHD_FUNC(0x023E5D94, BOOL, this, param_0);
    LK_PROC_CUT_HEAD(0x43 /* daPyProc_CUT_R_e */, 0x20 /* ANM_CUTR */, 0x13, 0x53 /* dRes_INDEX_LKANM_BCK_CUTRA_e */,
                     0x57 /* dRes_INDEX_LKANM_BCK_CUTRMS_e */, (void)0)
    mProcVar2 = param_0;
    m351E = m34DC;
    m3522 = 2;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    setBlurPosResource(0x286 /* dRes_INDEX_LKANM__CUTR_POS_e */);
    lk_onPlayerStatus0(0x8000 /* daPyStts0_SWORD_SWING_e */);
    setNormalCutAtParam(3 /* CUT_TYPE_CUT_R */);
    return TRUE;
}
VERIFY(0x023E5D94, &daPy_lk_c::procCutR_init);

/* 023E5F88 */
BOOL daPy_lk_c::procCutA_init(s16 param_0) {
    WWHD_FUNC(0x023E5F88, BOOL, this, param_0);
    LK_PROC_CUT_HEAD(0x41 /* daPyProc_CUT_A_e */, 0x1E /* ANM_CUTA */, 0x13, 0x40 /* dRes_INDEX_LKANM_BCK_CUTAA_e */,
                     0x41 /* dRes_INDEX_LKANM_BCK_CUTAMS_e */, current.angle.y = shape_angle.y /* HD */)
    mProcVar2 = param_0;
    lk_onPlayerStatus0(0x8000 /* daPyStts0_SWORD_SWING_e */);
    m3522 = 2;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    m351E = m34DC;
    setBlurPosResource(0x27D /* dRes_INDEX_LKANM__CUTA_POS_e */);
    setNormalCutAtParam(1 /* CUT_TYPE_CUT_A */);
    return TRUE;
}
VERIFY(0x023E5F88, &daPy_lk_c::procCutA_init);

/* 023E618C */
BOOL daPy_lk_c::procCutL_init(s16 param_0) {
    WWHD_FUNC(0x023E618C, BOOL, this, param_0);
    LK_PROC_CUT_HEAD(0x44 /* daPyProc_CUT_L_e */, 0x21 /* ANM_CUTL */, 0x12, 0x50 /* dRes_INDEX_LKANM_BCK_CUTLA_e */,
                     0x51 /* dRes_INDEX_LKANM_BCK_CUTLMS_e */, (void)0)
    mProcVar2 = param_0;
    m351E = m34DC;
    m3522 = 2;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    lk_onPlayerStatus0(0x8000 /* daPyStts0_SWORD_SWING_e */);
    setBlurPosResource(0x285 /* dRes_INDEX_LKANM__CUTL_POS_e */);
    setNormalCutAtParam(4 /* CUT_TYPE_CUT_L */);
    return TRUE;
}
VERIFY(0x023E618C, &daPy_lk_c::procCutL_init);

/* 023E6380 */
BOOL daPy_lk_c::procCutTurn_init(BOOL param_0) {
    WWHD_FUNC(0x023E6380, BOOL, this, param_0);
    gabi::call(LK_commonProcInit, this, 0x55 /* daPyProc_CUT_TURN_e */);
    f32 fVar1 = param_0 ? 5.0f : 2.0f; /* HD: HIO folded */
    if (mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        gabi::call(LK_setSingleMoveAnime, this, 0x28 /* ANM_CUTTURN */, 1.2f, fVar1, 0x15, 3.0f);
    } else {
        gabi::call(LK_setSingleMoveAnime, this, 0x29 /* ANM_CUTTURNC */, 1.2f, fVar1, 0x15, 3.0f);
    }
    if (mEquipItem == 0x103 /* daPyItem_SWORD_e */) {
        if (LK_checkNormalSwordEquip()) {
            LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, 0x59 /* dRes_INDEX_LKANM_BCK_CUTTURNA_e */));
        } else {
            LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, 0x5C /* dRes_INDEX_LKANM_BCK_CUTTURNMS_e */));
        }
        if (LK_checkNormalSwordEquip()) {
            m35A4 = 180.0f;
            LK_mAtCyl_SetAtAtp((noResetFlg1() & 0x8000) ? 4 : 2); /* daPyFlg1_SOUP_POWER_UP */
        } else {
            m35A4 = 230.0f;
            LK_mAtCyl_SetAtAtp((noResetFlg1() & 0x8000) ? 8 : 4);
        }
        setBlurPosResource(0x287 /* dRes_INDEX_LKANM__CUTTURN_POS_e */);
    } else {
        u32 boko = gabi::ea(mActorKeepEquip.mActor.get());
        u32 boko_type;
        if (boko == 0) {
            LK_mAtCyl_SetAtAtp(1);
            m35A4 = 180.0f; /* daBoko_c::Type_BOKO_STICK_e */
        } else {
            boko_type = gabi::load<u32>(boko + 0xB0); /* fopAcM_GetParam(boko) */
            LK_mAtCyl_SetAtAtp(gabi::load<u32>(0x10192138 + boko_type * 4)); /* boko->getAtPoint() */
            if (boko_type == 0) {        /* Type_BOKO_STICK_e */
                m35A4 = 180.0f;
            } else if (boko_type == 1) { /* Type_MACHETE_e */
                m35A4 = 200.0f;
            } else if (boko_type == 2) { /* Type_STALFOS_MACE_e */
                m35A4 = 250.0f;
            } else if (boko_type == 3) { /* Type_DARKNUT_SWORD_e */
                m35A4 = 270.0f;
            } else if (boko_type == 4) { /* Type_MOBLIN_SPEAR_e */
                m35A4 = 360.0f;
            } else {
                m35A4 = 250.0f;
            }
        }
        setBlurPosResource(0x28C /* dRes_INDEX_LKANM__WEAPONTURN_POS_e */);
    }
    LK_mAtCyl_SetR(gabi::fmuls_ppc(m35A4, 0.5f));
    m35EC = fVar1;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    mProcVar0 = 1; /* HD: HIO folded */
    current.angle.y = shape_angle.y;
    m3578 = 0;
    LK_voiceStart(1);
    lk_onPlayerStatus0(0x8000 /* daPyStts0_SWORD_SWING_e */);
    gabi::store<u8>(gabi::ea(this) + 0x3AC, 8); /* mCutType = CUT_TYPE_CUT_TURN */
    setResetFlg0(resetFlg0() & ~0x08000000u); /* offResetFlg0(daPyRFlg0_NOT_ATTACKING) */
    LK_mAtCyl_SetR(gabi::fmuls_ppc(m35A4, 0.5f));
    if (!(mNoResetFlg0 & 0x80000000u)) { /* !checkNoResetFlg0(daPyFlg0_UNK80000000) */
        gabi::call(LK_mtxFollow_makeEmitter, &m32E4, 0x1E /* ID_AK_JN_ROUNDATTACKTOE */,
                   lk_getAnmMtx(mpCLModel, 0x28 /* CL_JNT_RTOE_JNT_e */), &current.pos, nullptr);
        if (mCurrAttributeCode == 4 /* dBgS_Attr_GRASS_e */) {
            gabi::Local<u8[4]> color; /* tevStr.mColorC0 (HD: s16 components at 0x1A0) */
            gabi::store<u8>(color.a + 1, (u8)gabi::load<s16>(gabi::ea(this) + 0x1A2));
            gabi::store<u8>(color.a + 0, (u8)gabi::load<s16>(gabi::ea(this) + 0x1A0));
            gabi::store<u8>(color.a + 3, (u8)gabi::load<s16>(gabi::ea(this) + 0x1A6));
            gabi::store<u8>(color.a + 2, (u8)gabi::load<s16>(gabi::ea(this) + 0x1A4));
            gabi::call(LK_mtxFollow_makeEmitterColor, &m32F0, 0x21 /* ID_AK_JN_ROUNDATTACKKUSA */,
                       lk_getAnmMtx(mpCLModel, 0x23 /* CL_JNT_LTOE_JNT_e */), &current.pos, color.a, gabi::ea(this) + 0x1A8 /* &tevStr.mColorK0 */);
            m35A0 = 18.0f;
        } else if (mCurrAttributeCode == 0x13 /* dBgS_Attr_WATER_e */) {
            gabi::Local<be<u32>> amb;
            gabi::Local<be<u32>> dif;
            gabi::call(0x025602F0 /* dKy_get_seacolor */, amb.a, dif.a);
            gabi::call(LK_mtxFollow_makeEmitterColor, &m32F0, 0x20 /* ID_AK_JN_ROUNDATTACKSHIBUKI */,
                       lk_getAnmMtx(mpCLModel, 0x23 /* CL_JNT_LTOE_JNT_e */), &current.pos, amb.a, 0);
            m35A0 = 17.0f;
        } else {
            /* dComIfGp_particle_setToonP1 */
            dPa_control_set(dComIfGp_getParticle(), 3, 0x201F /* ID_AK_JT_ROUNDATTACKSMOKE */, &current.pos, nullptr, nullptr, 0xA0,
                            gabi::at<dPa_levelEcallBack>(gabi::ea(this) + 0x6710) /* &mSmokeEcallBack */, -1, nullptr, nullptr, nullptr);
            m35A0 = 21.0f;
        }
    }
    mProcVar6 = 0;
    if (m34C4 == 6) {
        m34C4 = 0;
        mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
        mProcVar6 = 1;
    } else {
        gabi::store<u8>(gabi::ea(this) + 0x3AD, 0); /* mCutCount */
        m34C4 = 0;
    }
    mNormalSpeed = 0.0f;
    endFlameDamageEmitter();
    return TRUE;
}
VERIFY(0x023E6380, &daPy_lk_c::procCutTurn_init);

/* 023E6A58 */
void daPy_lk_c::setExtraFinishCutAtParam(u8 cutType) {
    WWHD_FUNC(0x023E6A58, void, this, cutType);
    if (LK_checkNormalSwordEquip()) {
        m35FC = 1.7f; /* HD: HIO folded */
        setAtParam(2 /* AT_TYPE_SWORD */, 2, 1 /* dCcG_At_Spl_UNK1 */, 1 /* dCcG_SE_UNK1 */, 0xF /* dCcG_AtHitMark_Big_e */, cutType, 50.0f);
    } else {
        m35FC = 2.0f;
        setAtParam(2, 4, 1, 1, 0xF, cutType, 50.0f);
    }
}
VERIFY(0x023E6A58, &daPy_lk_c::setExtraFinishCutAtParam);

/* 023E6B1C */
BOOL daPy_lk_c::procCutExB_init() {
    WWHD_FUNC(0x023E6B1C, BOOL, this);
    LK_PROC_CUT_X_HEAD(0x48 /* daPyProc_CUT_EX_B_e */, 0x25 /* ANM_EXCB1 */, 0.9f, 5.0f, 0x24, 5.0f, 0x81 /* dRes_INDEX_LKANM_BCK_EXCB1A_e */,
                       0x82 /* dRes_INDEX_LKANM_BCK_EXCB1MS_e */, mProcVar0 = 0)
    setBlurPosResource(0x282 /* dRes_INDEX_LKANM__CUTEXB_POS_e */);
    setExtraFinishCutAtParam(0x1B /* CUT_TYPE_CUT_EXB */);
    m34C4 = 0;
    m3522 = 0;
    return TRUE;
}
VERIFY(0x023E6B1C, &daPy_lk_c::procCutExB_init);

/* 023E6D00 */
void daPy_lk_c::setExtraCutAtParam(u8 cutType) {
    WWHD_FUNC(0x023E6D00, void, this, cutType);
    if (LK_checkNormalSwordEquip()) {
        m35FC = 1.7f; /* HD: HIO folded */
        setAtParam(2 /* AT_TYPE_SWORD */, 1, 0 /* dCcG_At_Spl_UNK0 */, 1 /* dCcG_SE_UNK1 */, 0xD /* dCcG_AtHitMark_Nrm_e */, cutType, 50.0f);
    } else {
        m35FC = 2.0f;
        setAtParam(2, 2, 0, 1, 0xD, cutType, 50.0f);
    }
}
VERIFY(0x023E6D00, &daPy_lk_c::setExtraCutAtParam);

/* 023E6DC4 */
BOOL daPy_lk_c::procCutExMJ_init(int param_0) {
    WWHD_FUNC(0x023E6DC4, BOOL, this, param_0);
    gabi::call(LK_commonProcInit, this, 0x49 /* daPyProc_CUT_EX_MJ_e */);
    int dVar3;
    if (param_0 != 0) {
        dVar3 = 0x71; /* ANM_MROLLRC */
        mProcVar3 = 0x71;
        setBlurPosResource(0x27C /* dRes_INDEX_LKANM__BTROTATECUTR_POS_e */);
        m35A0 = 3.0f; /* HD: HIO folded */
    } else {
        dVar3 = 0x70; /* ANM_MROLLLC */
        mProcVar3 = 0x70;
        setBlurPosResource(0x27B /* dRes_INDEX_LKANM__BTROTATECUTL_POS_e */);
        m35A0 = 1.0f;
    }
    gabi::call(LK_setSingleMoveAnime, this, dVar3, 0.9f, 0.0f, 6, 1.0f);
    if (LK_checkNormalSwordEquip()) {
        LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, 0xC7 /* dRes_INDEX_LKANM_BCK_MROLLCA_e */));
    } else {
        LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, 0xC8 /* dRes_INDEX_LKANM_BCK_MROLLCMS_e */));
    }
    m35EC = 0.0f;
    mNoResetFlg0 = mNoResetFlg0 | 4; /* onNoResetFlg0(daPyFlg0_UNK4) */
    LK_voiceStart(1);
    lk_onPlayerStatus0(0x8000 /* daPyStts0_SWORD_SWING_e */);
    if (m34C4 != 6) {
        m3522 = 2;
        setExtraCutAtParam(0x1E /* CUT_TYPE_CUT_EXMJ */);
    } else {
        m34C4 = 0;
        m3522 = 0;
        setExtraFinishCutAtParam(0x1E);
    }
    speed.y = 29.0f;
    gravity = -5.0f;
    mProcVar2 = 0;
    mNoResetFlg0 = mNoResetFlg0 & ~0x40040u; /* offNoResetFlg0(daPyFlg0_NO_FALL_VOICE | daPyFlg0_CUT_AT_FLG) */
    mNormalSpeed = 7.5f;
    return TRUE;
}
VERIFY(0x023E6DC4, &daPy_lk_c::procCutExMJ_init);

/* 023E7054 */
BOOL daPy_lk_c::procCutKesa_init() {
    WWHD_FUNC(0x023E7054, BOOL, this);
    LK_PROC_CUT_X_HEAD(0x4A /* daPyProc_CUT_KESA_e */, 0xDE /* ANM_CUTKESA */, 1.3f, 4.0f, 0x31, 1.0f, 0x7E /* dRes_INDEX_LKANM_BCK_EXCA1A_e */,
                       0x7F /* dRes_INDEX_LKANM_BCK_EXCA1MS_e */, (void)0)
    setBlurPosResource(0x284 /* dRes_INDEX_LKANM__CUTKESA_POS_e */);
    setExtraCutAtParam(0x1F /* CUT_TYPE_CUT_KESA */);
    m3522 = 2;
    return TRUE;
}
VERIFY(0x023E7054, &daPy_lk_c::procCutKesa_init);

/* 023E7224 */
BOOL daPy_lk_c::procCutExA_init() {
    WWHD_FUNC(0x023E7224, BOOL, this);
    LK_PROC_CUT_X_HEAD(0x47 /* daPyProc_CUT_EX_A_e */, 0x24 /* ANM_EXCA1 */, 1.1f, 4.0f, 0x15, 1.0f, 0x7E /* dRes_INDEX_LKANM_BCK_EXCA1A_e */,
                       0x7F /* dRes_INDEX_LKANM_BCK_EXCA1MS_e */, mProcVar0 = 0)
    setBlurPosResource(0x281 /* dRes_INDEX_LKANM__CUTEXA_POS_e */);
    setExtraCutAtParam(0x1A /* CUT_TYPE_CUT_EXA */);
    m3522 = 1;
    return TRUE;
}
VERIFY(0x023E7224, &daPy_lk_c::procCutExA_init);

/* 023E73FC */
int daPy_lk_c::changeCutProc() {
    WWHD_FUNC(0x023E73FC, int, this);
    if (m34C5 == 5 && changeSpecialBattle()) {
        m34C4 = 0;
        return TRUE;
    }
    m34C4 = m34C4 + 1;
    int direction = getCutDirection() - 1;
    s16 sVar2;
    if (lk_checkAttentionLock() || !(mStickDistance > 0.05f)) {
        sVar2 = shape_angle.y;
    } else {
        sVar2 = m34E8;
    }
    u8 bVar1 = m34C4;
    if (bVar1 > 4) {
        gabi::store<u8>(gabi::ea(this) + 0x3AD, (u8)(bVar1 - 2)); /* mCutCount */
    } else {
        gabi::store<u8>(gabi::ea(this) + 0x3AD, bVar1);
    }
    bVar1 = m34C4;
    if (bVar1 == 4) {
        if (direction == 3 /* DIR_RIGHT */ || direction == 0 /* DIR_FORWARD */) {
            procCutEA_init();
        } else {
            procCutEB_init();
        }
    } else if (bVar1 < 4) {
        if (direction == 0 /* DIR_FORWARD */) {
            procCutF_init(sVar2);
        } else if (direction == 3 /* DIR_RIGHT */) {
            procCutR_init(sVar2);
        } else if (direction == 4 /* DIR_NONE */ && lk_checkAttentionLock()) {
            procCutA_init(sVar2);
        } else {
            procCutL_init(sVar2);
        }
    } else if (bVar1 == 6) {
        if (direction == 0 /* DIR_FORWARD */) {
            procCutTurn_init(TRUE);
        } else if (direction == 4 /* DIR_NONE */) {
            procCutExB_init();
        } else if (direction == 3 /* DIR_RIGHT */) {
            procCutExMJ_init(1);
        } else {
            procCutExMJ_init(0);
        }
    } else if (bVar1 < 6) {
        if (direction == 0 /* DIR_FORWARD */) {
            procCutExMJ_init(1);
        } else if (direction == 3 /* DIR_RIGHT */) {
            procCutKesa_init();
        } else {
            procCutExA_init();
        }
    }
    m3578 = 0;
    return TRUE;
}
VERIFY(0x023E73FC, &daPy_lk_c::changeCutProc);

/* 023E7660 */
void daPy_lk_c::setFanModel() {
    WWHD_FUNC(0x023E7660, void, this);
    u32 bck = gabi::call<u32>(LK_getItemAnimeResource, this, 0x112 /* dRes_INDEX_LKANM_BCK_USEFANAA_e */);
    u32 oldHeap = gabi::call<u32>(LK_setItemHeap, this);
    u32 tmp_modelData = gabi::call<u32>(LK_initModel, this, gabi::ea(this) + 0x4440 /* &mpEquipItemModel */, 0x19 /* dRes_INDEX_LINK_BDL_FAN_e */, 0x37221222);
    /* mSwordAnim.init(tmp_modelData, bck, false, J3DFrameCtrl::EMode_LOOP, 1.0f, 0, -1, false) */
    int ret = gabi::call<int>(0x025E8508, gabi::ea(this) + 0x4444, tmp_modelData, bck, 0, 2, 1.0f, 0, -1, 0);
    if (!ret) {
        JUT_ASSERT_fail(STR(0x10035650) /* "d_a_player_fan.inc" */, 0x105, STR(0x1003564C) /* "0" */);
    }
    gabi::call(0x025E3570 /* mDoExt_setCurrentHeap */, oldHeap);
}
VERIFY(0x023E7660, &daPy_lk_c::setFanModel);

/* HD: sead::SafeString equality (operator==), after the same-pointer test: the same characters (at most 0x40001) */
static inline BOOL lk_safeStringEq(u32 s1, u32 s2) {
    for (u32 i = 0; i < 0x40001; i++) {
        u8 c = gabi::load<u8>(s1 + i);
        if (c != gabi::load<u8>(s2 + i)) {
            return FALSE;
        }
        if (c == 0) {
            return TRUE;
        }
    }
    return FALSE;
}
/* compare the material's name (a relative offset at +4 of the name block at +0) with a C string through two
 * SafeString temporaries (the TU's vtable; slot 0x14 assures the termination) */
static inline BOOL lk_matNameIs(gabi::Local<SafeString>& a, gabi::Local<SafeString>& b, u32 mtl, u32 str) {
    a->__vtbl = LK_SafeString_vtbl;
    a->mStringTop = str;
    u32 blk = gabi::load<u32>(mtl);
    s32 off = gabi::load<s32>(blk + 4);
    b->__vtbl = LK_SafeString_vtbl;
    b->mStringTop = off != 0 ? blk + 4 + off : 0;
    gabi::call(0x02444F48, a.a); /* (an empty function) */
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.a);
    if (s1 == b->mStringTop) {
        return TRUE;
    }
    return lk_safeStringEq(a->mStringTop, b->mStringTop);
}

/* 023E771C */
int daPy_lk_c::setShapeFanLeaf() {
    WWHD_FUNC(0x023E771C, int, this);
    u32 mtl = 0;
    u32 model = gabi::load<u32>(gabi::ea(this) + 0x4440); /* mpEquipItemModel */
    if (model != 0) {
        /* getModelData()->getJointNodePointer(FAN_JNT_CL_FAN_e)->getMesh() (HD layout) */
        mtl = gabi::load<u32>(gabi::load<u32>(gabi::load<u32>(model + 0xAC) + 8) + 0x10);
    }
    m355C = (s16)(m355C + 0x82F);
    m3558 = (s16)gabi::ftoi(1536.0f * cM_ssin(m355C));
    m355A = (s16)gabi::ftoi(2048.0f * cM_ssin(m355C - 0xE00));
    if (gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x34) != 0) { /* dComIfGs_getMagic() >= 1 */
        for (; mtl != 0; mtl = gabi::load<u32>(mtl + 4)) {
            gabi::store<u8>(gabi::load<u32>(mtl + 8) + 4, 1); /* mtl->getShape() flag */
        }
        return TRUE;
    }
    for (u32 m = mtl; m != 0; m = gabi::load<u32>(m + 4)) {
        gabi::store<u8>(gabi::load<u32>(m + 8) + 4, 0);
    }
    /* HD: only the "veinsAMat" / "veinsBMat" material is flagged (GameCube: "leafAMat" hidden) */
    gabi::Local<SafeString> a, b, c, d;
    for (; mtl != 0; mtl = gabi::load<u32>(mtl + 4)) {
        if (lk_matNameIs(a, b, mtl, 0x1003566C /* "veinsAMat" */) || lk_matNameIs(c, d, mtl, 0x10035678 /* "veinsBMat" */)) {
            gabi::store<u8>(gabi::load<u32>(mtl + 8) + 4, 1);
            return FALSE;
        }
    }
    return FALSE;
}
VERIFY(0x023E771C, &daPy_lk_c::setShapeFanLeaf);

/* 023E79DC */
BOOL daPy_lk_c::procFanSwing_init() {
    WWHD_FUNC(0x023E79DC, BOOL, this);
    if (mCurProc == 0x92 /* daPyProc_FAN_SWING_e */) { /* HD */
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x92);
    gabi::call(LK_setSingleMoveAnime, this, 0xA1 /* ANM_USEFANA */, 1.0f, 0.0f, 0x22, 5.0f); /* HD: HIO folded */
    setFanModel();
    mProcVar6 = 0;
    LK_voiceStart(7);
    setAtParam(0x2000000 /* AT_TYPE_FAN_SWING */, 1, 0 /* dCcG_At_Spl_UNK0 */, 4 /* dCcG_SE_WOOD */, 0xD /* dCcG_AtHitMark_Nrm_e */, 0 /* CUT_TYPE_NONE */, 50.0f);
    m35EC = 0.0f;
    if (setShapeFanLeaf()) {
        mProcVar0 = 1;
        gabi::call(LK_seStartOnlyReverb, this, 0x2843 /* JA_SE_LK_FAN_PRE_SWING */);
        lk_onPlayerStatus1(0x40 /* daPyStts1_DEKU_LEAF_FAN_e */);
    } else {
        mProcVar0 = 0;
    }
    return TRUE;
}
VERIFY(0x023E79DC, &daPy_lk_c::procFanSwing_init);

/* 023E7B24 */
void daPy_lk_c::setPriTextureAnime(u16 r4, int r30) {
    WWHD_FUNC(0x023E7B24, void, this, r4, r30);
    u32 e = 0x100362A8 + r4 * 4; /* mTexAnmIndexTable[r4] */
    u16 btpIdx = gabi::load<u16>(e + 0);
    u16 btkIdx = gabi::load<u16>(e + 2);
    u32 tex = gabi::ea(this) + 0x65D0;    /* m_tex_anm_heap */
    u32 scroll = gabi::ea(this) + 0x65E0; /* m_tex_scroll_heap */
    if (gabi::load<u16>(tex + 2) != btpIdx) {
        gabi::store<u16>(tex + 2, btpIdx);
        if (gabi::load<u16>(tex + 4) == 0xFFFF) {
            u32 res = gabi::call<u32>(LK_loadTextureAnimeResource, this, (u32)btpIdx, 0);
            gabi::call(LK_setTextureAnimeResource, this, res, r30);
        }
    }
    if (gabi::load<u16>(scroll + 2) != btkIdx) {
        gabi::store<u16>(scroll + 2, btkIdx);
        if (gabi::load<u16>(scroll + 4) == 0xFFFF) {
            u32 res = gabi::call<u32>(LK_loadTextureScrollResource, this, (u32)btkIdx, 0);
            gabi::call(LK_setTextureScrollResource, this, res, r30);
        }
    }
}
VERIFY(0x023E7B24, &daPy_lk_c::setPriTextureAnime);

/* 023E7BEC */
void daPy_lk_c::setBowReadyAnime() {
    WWHD_FUNC(0x023E7BEC, void, this);
    /* HD: HIO folded (m_HIO->mItem.mBow.m.field_0x0 = 4) */
    gabi::call(LK_setActAnimeUpper, this, 0xE /* dRes_INDEX_LKANM_BCK_ARROWSHOOT_e */, 2 /* UPPER_MOVE2_e */, 1.0f, 3.999f, 4, 10.0f);
    LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, 0xF /* dRes_INDEX_LKANM_BCK_ARROWSHOOTA_e */));
    m35EC = 3.999f;
    setPriTextureAnime(0x70, 0);
    m355E = 10;
    gabi::call(LK_seStartOnlyReverb, this, 0x2810 /* JA_SE_LK_ITEM_TAKEOUT */);
}
VERIFY(0x023E7BEC, &daPy_lk_c::setBowReadyAnime);

/* 023E7C98 */
BOOL daPy_lk_c::procBowSubject_init() {
    WWHD_FUNC(0x023E7C98, BOOL, this);
    if (mCurProc == 0x94 /* daPyProc_BOW_SUBJECT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x94);
    if (!gabi::call<BOOL>(LK_checkBowAnime, this)) {
        setBowReadyAnime();
    }
    gabi::call(LK_setSingleMoveAnime, this, 8 /* ANM_ATNRS */, 0.0f, 0.0f, -1, 2.4f);
    mNormalSpeed = 0.0f;
    lk_onPlayerStatus0(0x1000 /* daPyStts0_BOW_AIM_e */);
    return TRUE;
}
VERIFY(0x023E7C98, &daPy_lk_c::procBowSubject_init);

/* 023E7D6C */
s16 daPy_lk_c::getGroundAngle(u32 param_1, s16 param_2) {
    WWHD_FUNC(0x023E7D6C, s16, this, param_1, param_2);
    u32 plane = dBgS_GetTriPla_l(param_1);
    if (plane == 0 || gabi::load<f32>(plane + 4) < 0.5f) { /* !cBgW_CheckBGround(plane->GetNP()->y) */
        return 0;
    }
    s16 a = cM_atan2s(gabi::load<f32>(plane + 0), gabi::load<f32>(plane + 8));
    f32 z = gabi::load<f32>(plane + 8);
    f32 x = gabi::load<f32>(plane + 0);
    f32 cos = cM_scos((s16)(a - param_2));
    f32 xz = std_sqrtf(gabi::fmadds(x, x, z * z));
    return cM_atan2s(gabi::fmuls_ppc(xz, cos), gabi::load<f32>(plane + 4));
}
VERIFY(0x023E7D6C, &daPy_lk_c::getGroundAngle);

/* 023E7E5C */
void daPy_lk_c::setBlendAtnBackMoveAnime(f32 param_1) {
    WWHD_FUNC(0x023E7E5C, void, this, param_1);
    f32 fVar1;
    if (m3580 == 8) {
        fVar1 = 1.0f;
    } else {
        fVar1 = cM_scos(m34E2);
    }
    f32 dVar7 = std::fabs(gabi::fmuls_ppc(fVar1, mNormalSpeed)) / mMaxNormalSpeed;
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[1]; /* UNDER_MOVE1_e */
    /* HD: HIO folded (mAtnMoveB 0x1C = 0.75, 0x20 = 1.0, 0x24 = 0.8, 0x28 = 0.95; mMove 0x38 = 1.1) */
    if (dVar7 < 0.75f) {
        f32 f = dVar7 / 0.75f;
        if (mModeFlg & 1) { /* checkModeFlg(ModeFlg_00000001) */
            m3598 = 0.0f;
            gabi::call(LK_setMoveAnime, this, f, 1.1f, 0.8f, 0 /* ANM_WAITS */, 0xE /* ANM_ATNWB */, 2, param_1);
        } else {
            m3598 = 1.0f;
            gabi::call(LK_setMoveAnime, this, f, 1.1f, 0.8f, 0, 0xE, 4, param_1);
        }
        if (!(mModeFlg & 1)) {
            if (frameCtrl->checkPass(2.0f)) {
                setResetFlg0(resetFlg0() | 0x400); /* onResetFlg0(daPyRFlg0_RIGHT_FOOT_ON_GROUND) */
            } else if (frameCtrl->checkPass(12.0f)) {
                setResetFlg0(resetFlg0() | 0x800); /* onResetFlg0(daPyRFlg0_LEFT_FOOT_ON_GROUND) */
            }
        }
    } else {
        f32 f;
        if (dVar7 < 1.0f) {
            f = (dVar7 - 0.75f) * 4.0f;
            gabi::call(LK_setMoveAnime, this, f, 0.8f, 0.95f, 0xE /* ANM_ATNWB */, 0xF /* ANM_ATNDB */, 4, param_1);
            m3598 = 1.0f - f;
        } else {
            gabi::Local<cXyz> xz; /* m36A0.abs2XZ() */
            fcpy_l(xz.a + 0, gabi::ea(&m36A0.x));
            xz->y = 0.0f;
            fcpy_l(xz.a + 8, gabi::ea(&m36A0.z));
            if (!(PSVECSquareMag(xz) < 49.0f)) { /* HD: NaN takes the fast branch */
                setResetFlg0(resetFlg0() | 0x40000); /* onResetFlg0(daPyRFlg0_UNK40000) */
                gabi::call(LK_setMoveAnime, this, 1.0f, 1.805f, 1.805f, 0xF, 0xF, 4, param_1); /* 1.9f * 0.95f */
            } else {
                gabi::call(LK_setMoveAnime, this, 1.0f, 0.95f, 0.95f, 0xF, 0xF, 4, param_1);
            }
            m3598 = 0.0f;
        }
        if (frameCtrl->checkPass(5.0f) || frameCtrl->checkPass(15.0f)) {
            setResetFlg0(resetFlg0() | 0x400);
        } else if (frameCtrl->checkPass(3.0f) || frameCtrl->checkPass(13.0f)) {
            setResetFlg0(resetFlg0() | 0x800);
        }
    }
    if (!(dVar7 < 0.9f)) { /* HD: NaN sets the flag */
        setResetFlg0(resetFlg0() | 0x10); /* onResetFlg0(daPyRFlg0_UNK10) */
    }
    if (!(resetFlg0() & 0xC00)) { /* !getFootOnGround() */
        gabi::call(LK_resetFootEffect, this);
    }
    gabi::call(LK_setHandModel, this, 0xF /* ANM_ATNDB */);
}
VERIFY(0x023E7E5C, &daPy_lk_c::setBlendAtnBackMoveAnime);

/* 023E81E4 */
void daPy_lk_c::setBlendAtnMoveAnime(f32 f30) {
    WWHD_FUNC(0x023E81E4, void, this, f30);
    s16 r3;
    if ((mAcch.m_flags & dBgS_Acch::GROUND_HIT) || !dBgS_ChkPolySafe_l(mAcchGndPoly)) {
        r3 = 0;
    } else {
        r3 = getGroundAngle(mAcchGndPoly, current.angle.y);
    }
    f32 f31 = std::fabs(gabi::fmuls_ppc(mNormalSpeed, cM_scos(r3)) / mMaxNormalSpeed);
    s16 iVar6 = (s16)(current.angle.y - shape_angle.y);
    f32 f2 = cM_ssin(iVar6);
    f32 fVar4 = cM_scos(iVar6);
    u8 uVar1 = mDirection;
    if (LK_demoMode() == 0x17 /* daPy_demo_c::DEMO_A_WAIT_e */) {
        if (demoParam0() == 1) {
            mDirection = 2; /* DIR_LEFT */
        } else {
            mDirection = 3; /* DIR_RIGHT */
        }
    } else if (mStickDistance > 0.05f) {
        /* HD: HIO folded (mAtnMoveB 0x30 = -0.99, 0x2C = 0.99); the NaN-safe forms of the original's branches */
        if (mpAttnActorLockOn.get() == nullptr && (!(fVar4 > -0.99f) || !(fVar4 < 0.99f))) {
            mDirection = !(fVar4 > -0.99f) ? 1 /* DIR_BACKWARD */ : 0 /* DIR_FORWARD */;
        } else {
            if (uVar1 == 1 || uVar1 == 0) {
                mDirection = 3; /* DIR_RIGHT */
                mMaxNormalSpeed = 12.0f;
            }
            if (f2 > 0.0f) {
                mDirection = 2; /* DIR_LEFT */
            } else if (f2 < 0.0f) {
                mDirection = 3; /* DIR_RIGHT */
            }
        }
    }
    u8 uVar2 = mDirection;
    if (uVar1 != uVar2) {
        f30 = 2.4f; /* m_HIO->mBasic.m.field_0xC */
    }
    if (uVar2 == 1 /* DIR_BACKWARD */) {
        mMaxNormalSpeed = 15.0f;
        setBlendAtnBackMoveAnime(f30);
        return;
    }
    if (uVar2 == 0 /* DIR_FORWARD */) {
        mMaxNormalSpeed = 17.0f;
        gabi::call(LK_setBlendMoveAnime, this, f30);
        return;
    }
    if (uVar2 != 3 && uVar2 != 2) {
        mDirection = 3; /* DIR_RIGHT */
        f30 = 2.4f;
    }
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[1]; /* UNDER_MOVE1_e */
    f32 f29 = gabi::call<BOOL>(LK_checkHeavyStateOn, this) ? 0.6f : 1.0f;
    int dVar8, dVar9;
    /* HD: HIO folded (mAtnMove 0x1C = 0.01, 0x20 = 0.9, 0x24 = 1.25, 0x28 = 1.0, 0x2C = 1.8) */
    if (f31 < 0.01f) {
        f32 f1 = f31 / 0.01f;
        if (mDirection == 2 /* DIR_LEFT */) {
            dVar8 = 7;  /* ANM_ATNLS */
            dVar9 = 9;  /* ANM_ATNWLS */
        } else {
            dVar8 = 8;  /* ANM_ATNRS */
            dVar9 = 10; /* ANM_ATNWRS */
        }
        if (mModeFlg & 1) { /* checkModeFlg(ModeFlg_00000001) */
            m3598 = 0.0f;
            gabi::call(LK_setMoveAnime, this, f1, 1.25f, f29, dVar8, dVar9, 2, f30);
        } else {
            m3598 = 1.0f;
            gabi::call(LK_setMoveAnime, this, f1, 1.25f, f29, dVar8, dVar9, 4, f30);
        }
    } else if (f31 < 0.9f) {
        f32 f28 = (f31 - 0.01f) / 0.89f;
        if (mDirection == 2 /* DIR_LEFT */) {
            dVar8 = 9;   /* ANM_ATNWLS */
            dVar9 = 0xB; /* ANM_ATNDLS */
        } else {
            dVar8 = 10;  /* ANM_ATNWRS */
            dVar9 = 0xC; /* ANM_ATNDRS */
        }
        gabi::call(LK_setMoveAnime, this, f28, f29, 1.8f * f29, dVar8, dVar9, 4, f30);
        m3598 = gabi::fnmsubs(f28, m3598, 1.0f); /* 1.0f - f28 * m3598 */
    } else {
        dVar9 = mDirection == 2 /* DIR_LEFT */ ? 0xB /* ANM_ATNDLS */ : 0xC /* ANM_ATNDRS */;
        gabi::Local<cXyz> xz; /* m36A0.abs2XZ() */
        fcpy_l(xz.a + 0, gabi::ea(&m36A0.x));
        xz->y = 0.0f;
        fcpy_l(xz.a + 8, gabi::ea(&m36A0.z));
        if (!(PSVECSquareMag(xz) < 49.0f)) { /* HD: NaN takes the fast branch */
            setResetFlg0(resetFlg0() | 0x40000); /* onResetFlg0(daPyRFlg0_UNK40000) */
            gabi::call(LK_setMoveAnime, this, 1.0f, 3.4199998f, 3.4199998f, dVar9, dVar9, 4, f30); /* 1.9f * 1.8f */
        } else {
            f32 f = 1.8f * f29;
            gabi::call(LK_setMoveAnime, this, 1.0f, f, f, dVar9, dVar9, 4, f30);
        }
        m3598 = 0.0f;
    }
    if (!(f31 < 0.9f)) { /* HD: NaN sets the flag */
        setResetFlg0(resetFlg0() | 0x10); /* onResetFlg0(daPyRFlg0_UNK10) */
    }
    if (!(mModeFlg & 1)) {
        if (mDirection == 2 /* DIR_LEFT */) {
            if (frameCtrl->checkPass(2.0f)) {
                setResetFlg0(resetFlg0() | 0x800); /* onResetFlg0(daPyRFlg0_LEFT_FOOT_ON_GROUND) */
            } else if (frameCtrl->checkPass(10.0f)) {
                setResetFlg0(resetFlg0() | 0x400); /* onResetFlg0(daPyRFlg0_RIGHT_FOOT_ON_GROUND) */
            } else {
                gabi::call(LK_resetFootEffect, this);
            }
        } else {
            if (frameCtrl->checkPass(10.0f)) {
                setResetFlg0(resetFlg0() | 0x800);
            } else if (frameCtrl->checkPass(2.0f)) {
                setResetFlg0(resetFlg0() | 0x400);
            } else {
                gabi::call(LK_resetFootEffect, this);
            }
        }
    }
    gabi::call(LK_setHandModel, this, dVar9);
}
VERIFY(0x023E81E4, &daPy_lk_c::setBlendAtnMoveAnime);

/* 023E88B0 */
BOOL daPy_lk_c::procBowMove_init() {
    WWHD_FUNC(0x023E88B0, BOOL, this);
    if (mCurProc == 0x95 /* daPyProc_BOW_MOVE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x95);
    if (mDirection == 2 /* DIR_LEFT */) {
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    } else {
        mDirection = 3; /* DIR_RIGHT */
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    }
    if (!gabi::call<BOOL>(LK_checkBowAnime, this)) {
        setBowReadyAnime();
    }
    setBlendAtnMoveAnime(2.4f /* m_HIO->mBasic.m.field_0xC */);
    lk_onPlayerStatus0(0x1000 /* daPyStts0_BOW_AIM_e */);
    return TRUE;
}
VERIFY(0x023E88B0, &daPy_lk_c::procBowMove_init);

/* 023E8984 */
BOOL daPy_lk_c::checkNextBowMode() {
    WWHD_FUNC(0x023E8984, BOOL, this);
    if (lk_checkAttentionLock()) {
        return procBowMove_init();
    }
    return procBowSubject_init();
}
VERIFY(0x023E8984, &daPy_lk_c::checkNextBowMode);

/* 023E89FC */
BOOL daPy_lk_c::procHookshotSubject_init() {
    WWHD_FUNC(0x023E89FC, BOOL, this);
    if (mCurProc == 0x83 /* daPyProc_HOOKSHOT_SUBJECT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x83);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setActAnimeUpper, this, 0xA7 /* dRes_INDEX_LKANM_BCK_HOOKSHOTWAIT_e */, 2 /* UPPER_MOVE2_e */, 0.0f, 0.0f, -1, -1.0f);
    gabi::call(LK_setSingleMoveAnime, this, 7 /* ANM_ATNLS */, 0.0f, 0.0f, -1, 2.4f);
    lk_onPlayerStatus0(0x4000 /* daPyStts0_HOOKSHOT_AIM_e */);
    return TRUE;
}
VERIFY(0x023E89FC, &daPy_lk_c::procHookshotSubject_init);

/* 023E8ADC */
BOOL daPy_lk_c::procHookshotMove_init() {
    WWHD_FUNC(0x023E8ADC, BOOL, this);
    if (mCurProc == 0x84 /* daPyProc_HOOKSHOT_MOVE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x84);
    if (mDirection == 3 /* DIR_RIGHT */) {
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    } else {
        mDirection = 2; /* DIR_LEFT */
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    }
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    if (gabi::load<u32>(hookshot + 0xB0) == 0) { /* hookshot->checkWait() (HD: the actor's +0xB0) */
        gabi::call(LK_setActAnimeUpper, this, 0xA7 /* dRes_INDEX_LKANM_BCK_HOOKSHOTWAIT_e */, 2, 1.0f, 0.0f, -1, -1.0f);
        setBlendAtnMoveAnime(2.4f);
    }
    lk_onPlayerStatus0(0x4000 /* daPyStts0_HOOKSHOT_AIM_e */);
    return TRUE;
}
VERIFY(0x023E8ADC, &daPy_lk_c::procHookshotMove_init);

/* 023E8BD4 */
BOOL daPy_lk_c::checkNextHookshotMode() {
    WWHD_FUNC(0x023E8BD4, BOOL, this);
    if (LK_upperIdx() != 0xA7) { /* !checkHookshotReadyAnime() */
        m355C = 10;
        m355E = 0;
        gabi::call(LK_seStartOnlyReverb, this, 0x2810 /* JA_SE_LK_ITEM_TAKEOUT */);
    }
    if (lk_checkAttentionLock()) {
        return procHookshotMove_init();
    }
    return procHookshotSubject_init();
}
VERIFY(0x023E8BD4, &daPy_lk_c::checkNextHookshotMode);

/* HD: entering a boomerang aim proc from another proc clears the HD byte at 0x827C and m355C and sets
 * m355E from the item button */
#define LK_BOOMERANG_ENTER(other) \
    if (mCurProc != (other)) { \
        mHD827C = 0; \
        m355C = 0; \
        m355E = gabi::call<BOOL>(LK_itemButton, this) == 0; \
    }

/* 023E8CA4 */
BOOL daPy_lk_c::procBoomerangSubject_init() {
    WWHD_FUNC(0x023E8CA4, BOOL, this);
    if (mCurProc == 0x80 /* daPyProc_BOOMERANG_SUBJECT_e */) {
        return FALSE;
    }
    LK_BOOMERANG_ENTER(0x81)
    gabi::call(LK_commonProcInit, this, 0x80);
    mNormalSpeed = 0.0f;
    gabi::call(LK_setActAnimeUpper, this, 0x35 /* dRes_INDEX_LKANM_BCK_BOOMWAIT_e */, 2, 0.8f, 0.0f, -1, -1.0f); /* HD: rate 0.8 */
    gabi::call(LK_setSingleMoveAnime, this, 8 /* ANM_ATNRS */, 0.0f, 0.0f, -1, 2.4f);
    lk_onPlayerStatus0(0x80000 /* daPyStts0_BOOMERANG_AIM_e */);
    current.angle.y = shape_angle.y;
    return TRUE;
}
VERIFY(0x023E8CA4, &daPy_lk_c::procBoomerangSubject_init);

/* 023E8DC8 */
BOOL daPy_lk_c::procBoomerangMove_init() {
    WWHD_FUNC(0x023E8DC8, BOOL, this);
    if (mCurProc == 0x81 /* daPyProc_BOOMERANG_MOVE_e */) {
        return FALSE;
    }
    LK_BOOMERANG_ENTER(0x80)
    gabi::call(LK_commonProcInit, this, 0x81);
    if (mDirection == 2 /* DIR_LEFT */) {
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    } else {
        mDirection = 3; /* DIR_RIGHT */
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    }
    gabi::call(LK_setActAnimeUpper, this, 0x35 /* dRes_INDEX_LKANM_BCK_BOOMWAIT_e */, 2, 0.8f, 0.0f, -1, -1.0f);
    setBlendAtnMoveAnime(2.4f);
    lk_onPlayerStatus0(0x80000 /* daPyStts0_BOOMERANG_AIM_e */);
    return TRUE;
}
VERIFY(0x023E8DC8, &daPy_lk_c::procBoomerangMove_init);

/* 023E8F2C */
BOOL daPy_lk_c::checkNextBoomerangMode() {
    WWHD_FUNC(0x023E8F2C, BOOL, this);
    if (!gabi::call<BOOL>(LK_checkBoomerangAnime, this)) {
        gabi::call(LK_seStartOnlyReverb, this, 0x2810 /* JA_SE_LK_ITEM_TAKEOUT */);
    }
    if (lk_checkAttentionLock()) {
        return procBoomerangMove_init();
    }
    return procBoomerangSubject_init();
}
VERIFY(0x023E8F2C, &daPy_lk_c::checkNextBoomerangMode);

/* 023E8FBC */
BOOL daPy_lk_c::procRopeSubject_init() {
    WWHD_FUNC(0x023E8FBC, BOOL, this);
    if (mCurProc == 0x76 /* daPyProc_ROPE_SUBJECT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x76);
    mNormalSpeed = 0.0f;
    if (gabi::load<u32>(gabi::ea(mActorKeepEquip.mActor.get()) + 0xB0) == 0) { /* fopAcM_GetParam(mActorKeepEquip.getActor()) */
        gabi::call(LK_setActAnimeUpper, this, 0xE4 /* dRes_INDEX_LKANM_BCK_ROPETHROWWAIT_e */, 2, 0.0f, 0.0f, -1, -1.0f);
        gabi::call(LK_setSingleMoveAnime, this, 8 /* ANM_ATNRS */, 0.0f, 0.0f, -1, 3.0f); /* HD: HIO folded */
    }
    lk_onPlayerStatus0(0x20000 /* daPyStts0_ROPE_AIM_e */);
    current.angle.y = shape_angle.y;
    lk_seStartSystem(0x81C /* JA_SE_CAMERA_L_MOVE */);
    mProcVar6 = 0;
    m3600 = -1.0f;
    m3604 = -1.0f;
    return TRUE;
}
VERIFY(0x023E8FBC, &daPy_lk_c::procRopeSubject_init);

/* 023E90EC */
BOOL daPy_lk_c::procRopeMove_init() {
    WWHD_FUNC(0x023E90EC, BOOL, this);
    if (mCurProc == 0x7D /* daPyProc_ROPE_MOVE_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x7D);
    if (mDirection == 2 /* DIR_LEFT */) {
        current.angle.y = (s16)(shape_angle.y + 0x4000);
    } else {
        mDirection = 3; /* DIR_RIGHT */
        current.angle.y = (s16)(shape_angle.y - 0x4000);
    }
    if (gabi::load<u32>(gabi::ea(mActorKeepEquip.mActor.get()) + 0xB0) == 0) {
        gabi::call(LK_setActAnimeUpper, this, 0xE4 /* dRes_INDEX_LKANM_BCK_ROPETHROWWAIT_e */, 2, 1.0f, 0.0f, -1, -1.0f);
        setBlendAtnMoveAnime(3.0f);
    }
    lk_onPlayerStatus0(0x20000 /* daPyStts0_ROPE_AIM_e */);
    return TRUE;
}
VERIFY(0x023E90EC, &daPy_lk_c::procRopeMove_init);

/* 023E91E4 */
BOOL daPy_lk_c::checkNextRopeMode() {
    WWHD_FUNC(0x023E91E4, BOOL, this);
    /* virtual checkRopeReadyAnime() (vtable slot 0xDC) */
    if (!gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0xDC), this) && gabi::load<u32>(gabi::ea(mActorKeepEquip.mActor.get()) + 0xB0) == 0) {
        m355C = 10;
        m355E = 0;
        gabi::call(LK_seStartOnlyReverb, this, 0x2810 /* JA_SE_LK_ITEM_TAKEOUT */);
    }
    if (lk_checkAttentionLock()) {
        return procRopeMove_init();
    }
    return procRopeSubject_init();
}
VERIFY(0x023E91E4, &daPy_lk_c::checkNextRopeMode);

/* mDoExt_AnmRatioPack::setRatio(0.0f) (HD: a counted array, count +8 and pointer +0xC, both reloaded per entry) */
static inline void lk_anmRatio_clear(u32 pack, f32 r) {
    for (s32 i = 0; i < gabi::load<s32>(pack + 8); i++) {
        gabi::store<f32>(gabi::load<u32>(pack + 0xC) + i * 4, r);
    }
}

/* 023E92A0 */
BOOL daPy_lk_c::procTactWait_init(int r30) {
    WWHD_FUNC(0x023E92A0, BOOL, this, r30);
    if (mCurProc == 0x9A /* daPyProc_TACT_WAIT_e */) {
        if ((u32)mProcVar6 != (u32)r30) {
            mProcVar4 = 0;
            mProcVar5 = 0;
            mProcVar1 = -1;
        }
        mProcVar6 = r30;
        gabi::store<u8>(dComIfGp_ea() + 0x5BD1, r30 != -4); /* dComIfGp_setMetronomeOn() / Off() */
        return TRUE;
    }
    if (r30 == -1) {
        /* dComIfGp_event_compulsory(this, NULL, -1) */
        if (!gabi::call<BOOL>(0x02540310, dComIfGp_ea() + 0x51D0, this, 0, 0xFFFF)) {
            return FALSE;
        }
        gabi::store<s16>(gabi::ea(this) + 0x420, 5); /* mDemo.setSpecialDemoType() */
    }
    int r31 = checkShipRideUseItem(1);
    if (r31 == 0) {
        if (r30 == -1) {
            u32 attn = gabi::ea(this) + 0x390; /* attention_info.position */
            gabi::Local<cXyz> startPos;
            gabi::Local<cXyz> endPos;
            f32 y = gabi::load<f32>(attn + 4) - 50.0f;
            f32 f30 = 200.0f * cM_ssin(shape_angle.y);
            f32 f31 = 200.0f * cM_scos(shape_angle.y);
            fcpy_l(startPos.a + 0, attn + 0);
            startPos->y = y;
            fcpy_l(startPos.a + 8, attn + 8);
            endPos->y = y;
            endPos->x = gabi::load<f32>(attn + 0) + f30;
            endPos->z = gabi::load<f32>(attn + 8) + f31;
            u32 chk = gabi::ea(this) + 0x9D0; /* mLinkLinChk */
            gabi::call(LK_dBgS_LinChk_Set, chk, startPos.a, endPos.a, this);
            if (lk_LineCross(chk)) {
                endPos->x = startPos->x - f30;
                endPos->z = startPos->z - f31;
                gabi::call(LK_dBgS_LinChk_Set, chk, startPos.a, endPos.a, this);
                if (!lk_LineCross(chk)) {
                    shape_angle.y = (s16)(shape_angle.y - 0x8000);
                    current.angle.y = shape_angle.y;
                } else {
                    endPos->x = startPos->x + f31;
                    endPos->z = startPos->z - f30;
                    gabi::call(LK_dBgS_LinChk_Set, chk, startPos.a, endPos.a, this);
                    if (!lk_LineCross(chk)) {
                        shape_angle.y = (s16)(shape_angle.y + 0x4000);
                        current.angle.y = shape_angle.y;
                    } else {
                        endPos->x = startPos->x - f31;
                        endPos->z = startPos->z + f30;
                        gabi::call(LK_dBgS_LinChk_Set, chk, startPos.a, endPos.a, this);
                        BOOL cross = lk_LineCross(chk);
                        s16 a = shape_angle.y;
                        if (!cross) {
                            a = (s16)(a - 0x4000);
                            shape_angle.y = a;
                        }
                        current.angle.y = a;
                    }
                }
            }
        } else if (r30 == -4 || r30 == -5) {
            if (gabi::call<u32>(LK_fopAcM_getTalkEventPartner, this) != 0) {
                fopAc_ac_c* partner = gabi::call<fopAc_ac_c*>(LK_fopAcM_getTalkEventPartner, this);
                shape_angle.y = fopAcM_searchActorAngleY(this, partner);
            }
        }
    }
    gabi::call(LK_commonProcInit, this, 0x9A);
    gabi::store<u8>(0x101CEF18, 0); /* daPy_matAnm_c::offMabaFlg() */
    gabi::store<u8>(0x101CEF19, 1);
    gabi::call(LK_setSingleMoveAnime, this, 0xA8 /* ANM_WAITTAKT */, 0.8f, 0.0f, -1, 6.0f); /* HD: HIO folded */
    gabi::call(LK_setActAnimeUpper, this, 0x127 /* dRes_INDEX_LKANM_BCK_WAITTAKT_e */, 1 /* UPPER_MOVE1_e */, 0.8f, 0.0f, -1, -1.0f);
    lk_anmRatio_clear(gabi::ea(this) + 0x5828, 0.0f); /* mAnmRatioUpper[UPPER_MOVE1_e].setRatio(0.0f) */
    gabi::call(LK_setActAnimeUpper, this, 0x127, 2 /* UPPER_MOVE2_e */, 0.8f, 0.0f, -1, -1.0f);
    lk_anmRatio_clear(gabi::ea(this) + 0x5838, 0.0f); /* mAnmRatioUpper[UPPER_MOVE2_e].setRatio(0.0f) */
    mProcVar2 = 0;
    mProcVar3 = 0;
    mNormalSpeed = 0.0f;
    lk_onPlayerStatus1(1 /* daPyStts1_WIND_WAKER_CONDUCT_e */);
    if (r30 == -1) {
        u32 cam = gabi::call<u32>(0x024F8044 /* dCam_getBody */);
        gabi::call(0x0253E70C /* dCamera_c::StartEventCamera */, cam, 0xC, gabi::load<u32>(gabi::ea(this) + 4) /* fopAcM_GetID(this) */, 0);
        lk_seStartSystem(0x86F /* JA_SE_TAKT_USE_BEGIN */);
    }
    gabi::call(0x025E1E94 /* mDoAud_tact_reset */);
    gabi::call(0x025E1EE0 /* mDoAud_tact_setBeat */, 0);
    gabi::call(0x025E1EF0 /* mDoAud_tact_setVolume */, 0.0f);
    gabi::call(0x025E1F14 /* mDoAud_tact_ambientPlay */);
    gabi::call(LK_setTactModel, this);
    initShipRideUseItem(r31, 2);
    mProcVar1 = r31 != 0 ? -1 : -30; /* HD: -30 when not on the ship */
    u32 model = gabi::load<u32>(gabi::ea(this) + 0x4440); /* mpEquipItemModel->getBaseTRMtx() */
    gabi::call(LK_mtxFollow_makeEmitter, &m32E4, 0x50 /* ID_AK_JN_TAKT00 */, model ? model + 0xC8 : 0, &current.pos, nullptr);
    mProcVar6 = r30;
    m35A0 = 0.0f;
    mProcVar5 = 0;
    mProcVar7 = -1;
    mProcVar4 = 0;
    m3624 = 0;
    m35A4 = 0.0f;
    m35A8 = 0.0f;
    mProcVar0 = -1;
    if (r30 == 6 || r30 == 7) {
        m35AC = 600.0f;
    } else {
        m35AC = 900.0f;
    }
    gabi::call_ptr(gabi::load<u32>(__vtbl + 0x84), this, -1, -1, 0); /* virtual setTactZev(-1, -1, NULL) */
    if (mProcVar6 != -4) {
        gabi::store<u8>(dComIfGp_ea() + 0x5BD1, 1); /* dComIfGp_setMetronomeOn() */
    }
    gabi::call(0x025E1E7C /* mDoAud_taktModeMute */);
    return TRUE;
}
VERIFY(0x023E92A0, &daPy_lk_c::procTactWait_init);

/* 023E99D0 */
int daPy_lk_c::setHintActor() {
    WWHD_FUNC(0x023E99D0, int, this);
    /* dComIfGp_att_getZHint(): dAttCatch_c::convPId(play + 0x5928, the Z-hint process id at play + 0x5930) */
    u32 play = dComIfGp_ea();
    if (gabi::call<u32>(0x024EBAE8, play + 0x5928, gabi::load<u32>(play + 0x5804 + 0x12C)) != 0 &&
        !gabi::load<u8>(dComIfGp_ea() + 0x5292) /* !dComIfGp_event_runCheck() */ &&
        gabi::load<u16>(gabi::ea(this) + 0x420) == 0 /* !checkPlayerDemoMode() */) {
        gabi::call(0x023D46F4 /* daPy_py_c::setDoButtonQuake */, this);
        setResetFlg0(resetFlg0() | 0x100); /* onResetFlg0(daPyRFlg0_UNK100) */
        gabi::store<u8>(dComIfGp_ea() + 0x5BB7, 0x31); /* dComIfGp_setDoStatus(dActStts_ba_sake__dupe_31) */
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023E99D0, &daPy_lk_c::setHintActor);

/* 023E9A6C daPy_lk_c::checkDrinkBottleItem (unnamed by the matcher) */
BOOL daPy_lk_c::checkDrinkBottleItem(int item) {
    WWHD_FUNC(0x023E9A6C, BOOL, this, item);
    return (u32)(item - 0x51) <= 4;
}
VERIFY(0x023E9A6C, &daPy_lk_c::checkDrinkBottleItem);

/* 023E9A84 daPy_lk_c::checkOpenBottleItem (unnamed by the matcher) */
BOOL daPy_lk_c::checkOpenBottleItem(int item) {
    WWHD_FUNC(0x023E9A84, BOOL, this, item);
    return (u32)(item - 0x56) <= 3;
}
VERIFY(0x023E9A84, &daPy_lk_c::checkOpenBottleItem);

/* 023E9A9C */
BOOL daPy_lk_c::checkGroupItem(int param_1, int itemNo) {
    WWHD_FUNC(0x023E9A9C, BOOL, this, param_1, itemNo);
    if (param_1 == 0x105 /* daPyItem_DRINK_BOTTLE_e */) {
        return checkDrinkBottleItem(itemNo);
    } else if (param_1 == 0x106 /* daPyItem_OPEN_BOTTLE_e */) {
        return checkOpenBottleItem(itemNo);
    } else if (param_1 == 0x107 /* daPyItem_ESA_e */) {
        return gabi::call<BOOL>(0x0255101C /* isEsa */, (u32)(u8)itemNo);
    } else if (param_1 == 0x108 /* daPyItem_BOW_e */) {
        return gabi::call<BOOL>(LK_checkBowItem, this, itemNo);
    } else if (param_1 == 0x109 /* daPyItem_PHOTOBOX_e */) {
        return gabi::call<BOOL>(LK_checkPhotoBoxItem, this, itemNo);
    }
    return param_1 == itemNo;
}
VERIFY(0x023E9A9C, &daPy_lk_c::checkGroupItem);

/* 023E9AF4 */
BOOL daPy_lk_c::checkSetItemTrigger(int param_1, int param_2) {
    WWHD_FUNC(0x023E9AF4, BOOL, this, param_1, param_2);
    if (param_2 != 0 && gabi::load<u16>(0x101CEF16) == 1) { /* daPy_dmEcallBack_c::checkCurse() (HD: a global) */
        return FALSE;
    }
    /* HD: the X/Y/Z(/fourth) item triggers only while the HD word at 0x8260 is 0; it is re-read after each check */
    if (mHD8260 == 0) {
        do {
            if (mItemTrigger & 4) { /* itemTriggerX() */
                if (checkGroupItem(param_1, gabi::load<u8>(dComIfGp_ea() + 0x5BBB) /* dComIfGp_getSelectItem(dItemBtn_X_e) */)) {
                    mReadyItemBtn = 0;
                    return TRUE;
                }
                if (mHD8260 != 0) break;
            }
            if (mItemTrigger & 8) { /* itemTriggerY() */
                if (checkGroupItem(param_1, gabi::load<u8>(dComIfGp_ea() + 0x5BBC))) {
                    mReadyItemBtn = 1;
                    return TRUE;
                }
                if (mHD8260 != 0) break;
            }
            if (mItemTrigger & 0x10) { /* itemTriggerZ() */
                if (checkGroupItem(param_1, gabi::load<u8>(dComIfGp_ea() + 0x5BBD))) {
                    mReadyItemBtn = 2;
                    return TRUE;
                }
                if (mHD8260 != 0) break;
            }
            if (mItemTrigger & 0x80) { /* HD: a fourth item button */
                if (checkGroupItem(param_1, gabi::load<u8>(dComIfGp_ea() + 0x5BBE))) {
                    mReadyItemBtn = 3;
                    return TRUE;
                }
            }
        } while (0);
    }
    /* HD: on the ship (normal mode) the do button selects the item of a fifth slot */
    if (checkShipRideUseItem(0) == 1 && !lk_checkPlayerStatus1(2) && !lk_checkPlayerStatus1(4) && !lk_checkPlayerStatus1(0x400) &&
        (mItemTrigger & 1) && checkGroupItem(param_1, gabi::load<u8>(dComIfGp_ea() + 0x5BBF))) {
        mReadyItemBtn = 4;
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023E9AF4, &daPy_lk_c::checkSetItemTrigger);

/* 023E9D50 */
BOOL daPy_lk_c::changeDragonShield(int param_1) {
    WWHD_FUNC(0x023E9D50, BOOL, this, param_1);
    u32 flg = noResetFlg1();
    if (flg & 1) { /* checkNoResetFlg1(daPyFlg1_EQUIP_DRAGON_SHIELD) */
        setNoResetFlg1(flg & ~1u);
        return TRUE;
    }
    if (gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x24) != 0) { /* HD: a save-data u16 (GameCube dComIfGs_getMagic()) */
        setNoResetFlg1(flg | 1);
        m3548 = 60; /* HD: HIO folded */
        mTinkleShieldTimer = 0;
        return TRUE; /* HD: no dComIfGp_setItemMagicCount(-1) */
    }
    if (param_1 != 0) {
        lk_seStartSystem(0x883 /* JA_SE_ITEM_TARGET_OUT */);
    }
    return TRUE;
}
VERIFY(0x023E9D50, &daPy_lk_c::changeDragonShield);

/* 023E9DD0 */
BOOL daPy_lk_c::procBootsEquip_init(u16 param_1) {
    WWHD_FUNC(0x023E9DD0, BOOL, this, param_1);
    gabi::call(LK_commonProcInit, this, 0xA1 /* daPyProc_BOOTS_EQUIP_e */);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    gabi::call(LK_setSingleMoveAnime, this, 0xAE /* ANM_SETBOOTS */, 1.0f, 0.0f, 0x13, 5.0f); /* HD: HIO folded */
    mProcVar6 = param_1;
    return TRUE;
}
VERIFY(0x023E9DD0, &daPy_lk_c::procBootsEquip_init);

/* 023E9E48 */
BOOL daPy_lk_c::procGrabThrow_init(int param_0) {
    WWHD_FUNC(0x023E9E48, BOOL, this, param_0);
    if (mCurProc == 0x71 /* daPyProc_GRAB_THROW_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x71);
    /* HD: HIO folded */
    if (mActorKeepGrab.mActor.get()->actor_status & 0x10000) { /* fopAcM_CheckStatus(.., fopAcStts_UNK10000_e) */
        gabi::call(LK_setSingleMoveAnime, this, 0x6B /* ANM_GRABRE */, 1.1f, 3.0f, 0xC, 2.0f);
        mProcVar7 = 0;
    } else {
        gabi::call(LK_setSingleMoveAnime, this, 0x6A /* ANM_GRABTHROW */, 0.8f, 1.0f, 0xE, 0.0f);
        mProcVar7 = 1;
    }
    fcpy_l(gabi::ea(&m35A0), gabi::ea(&mNormalSpeed)); /* m35A0 = mNormalSpeed */
    lk_offPlayerStatus1(0x40000 /* daPyStts1_UNK40000_e */);
    mProcVar6 = param_0;
    return TRUE;
}
VERIFY(0x023E9E48, &daPy_lk_c::procGrabThrow_init);

/* 023E9F60 */
void daPy_lk_c::setAnimeEquipSword(BOOL r4) {
    WWHD_FUNC(0x023E9F60, void, this, r4);
    if (!LK_checkSwordEquip()) {
        return;
    }
    m3562 = 0x103; /* daPyItem_SWORD_e */
    gabi::call(LK_setActAnimeUpper, this, 0xD7 /* dRes_INDEX_LKANM_BCK_REST_e */, 2 /* UPPER_MOVE2_e */, -1.8f, 1.0f, 0xE, 1.0f); /* HD: HIO folded */
    setPriTextureAnime(0x72, 0);
    lk_offPlayerStatus0(0x400000 /* daPyStts0_BOOMERANG_WAIT_e */);
    u32 boomerang = gabi::ea(mActorKeepThrow.mActor.get());
    if (boomerang != 0) {
        gabi::store<u8>(boomerang + 0x26274, 1); /* daBoomerang_c::onCancelFlg() */
    }
    if (!r4) {
        setNoResetFlg1(noResetFlg1() | 0x4000000); /* onNoResetFlg1(daPyFlg1_UNK4000000) */
    } else {
        setNoResetFlg1(noResetFlg1() & ~0x4000000u);
    }
}
VERIFY(0x023E9F60, &daPy_lk_c::setAnimeEquipSword);

/* 023EA054 */
BOOL daPy_lk_c::procGrabPut_init() {
    WWHD_FUNC(0x023EA054, BOOL, this);
    if (mCurProc == 0x72 /* daPyProc_GRAB_PUT_e */) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x72);
    if (LK_FIELD(f32, 0x3CC) < 0.0f) { /* checkGrabWear() (daPy_py_c, GameCube 0x2B0) */
        mProcVar6 = 1;
        gabi::call(LK_setSingleMoveAnime, this, 0x68 /* ANM_GRABWAIT */, 1.0f, 0.0f, -1, 3.0f); /* HD: HIO folded */
    } else {
        mProcVar6 = 0;
        gabi::call(LK_setSingleMoveAnime, this, 0x66 /* ANM_GRABUP */, -1.1f, 0.0f, 7, 0.0f);
    }
    mNormalSpeed = 0.0f;
    u32 grab_actor = gabi::ea(mActorKeepGrab.mActor.get());
    u32 src = grab_actor != 0 ? grab_actor + 0x314 : gabi::ea(&current.pos);
    gabi::store<u32>(gabi::ea(&m370C) + 0, gabi::load<u32>(src + 0)); /* m370C = ..->current.pos (word copy) */
    gabi::store<u32>(gabi::ea(&m370C) + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(gabi::ea(&m370C) + 8, gabi::load<u32>(src + 8));
    setResetFlg0(resetFlg0() | 0x400000); /* onResetFlg0(daPyRFlg0_GRAB_PUT_START) */
    return TRUE;
}
VERIFY(0x023EA054, &daPy_lk_c::procGrabPut_init);

/* 023EA1AC */
BOOL daPy_lk_c::checkNextActionGrab() {
    WWHD_FUNC(0x023EA1AC, BOOL, this);
    u32 grab_actor = gabi::ea(mActorKeepGrab.mActor.get());
    if (grab_actor == 0) {
        return FALSE;
    }
    int iVar3 = gabi::call<int>(LK_checkGrabBarrelSearch, this, 0);
    if ((gabi::load<u32>(grab_actor + 0x2E0) & 0x10000) && !gabi::call<BOOL>(LK_checkGrabSpecialHeavyState, this)) {
        lk_setRStatus(0xE);  /* dActStts_THROW_e */
        lk_setDoStatus(0xE);
    } else {
        lk_setRStatus(9);    /* dActStts_DROP_e */
    }
    if (iVar3 != 0) {
        lk_setDoStatus(9);   /* dActStts_DROP_e */
    } else if (mpAttnActorLockOn.get() == nullptr && mpAttnEntryA != 0 && gabi::load<s32>(mpAttnEntryA + 8) == 5 /* fopAc_Attn_TYPE_DOOR_e */ &&
               (gabi::load<u32>(grab_actor + 0x2E0) & 0x2000000) /* fopAcStts_UNK2000000_e */) {
        lk_setDoStatus(0xB); /* dActStts_OPEN_e */
    } else if (lk_checkAttentionLock()) {
        int direction = gabi::call<int>(LK_getDirectionFromShapeAngle, this);
        if (!(mStickDistance > 0.05f) || direction == 0 /* DIR_FORWARD */ || direction == 1 /* DIR_BACKWARD */) {
            lk_setDoStatus(0xE);
        }
    } else {
        lk_setDoStatus(0xE);
    }
    if (setHintActor() && LK_doTrigger()) {
        u32 play = dComIfGp_ea();
        u32 hint = gabi::call<u32>(0x024EBAE8, play + 0x5928, gabi::load<u32>(play + 0x5804 + 0x12C)); /* dComIfGp_att_getZHint() */
        gabi::call(0x025D7640 /* fopAcM_orderZHintEvent */, this, hint);
        return TRUE;
    }
    if (checkSetItemTrigger(0x2A /* dItemNo_MAGIC_ARMOR_e */, 1) || ((noResetFlg1() & 1) && checkSetItemTrigger(0x2A, 0))) {
        changeDragonShield(1);
        return TRUE;
    }
    if (checkSetItemTrigger(0x29 /* dItemNo_IRON_BOOTS_e */, 1) || ((mNoResetFlg0 & 0x2000000) /* checkEquipHeavyBoots() */ && checkSetItemTrigger(0x29, 0))) {
        return procBootsEquip_init(0x29);
    }
    if (gabi::load<s16>(grab_actor + 8) == 0x126 /* fpcNm_BOMB_e */ && checkSetItemTrigger(0x31 /* dItemNo_BOMB_BAG_e */, 0)) {
        return procGrabThrow_init(0);
    }
    if ((mItemTrigger & 2) /* cancelTrigger() */ && LK_checkSwordEquip() && iVar3 == 0) {
        if (lk_getRStatus() == 0xE) {
            return procGrabThrow_init(1);
        }
        gabi::call(LK_freeGrabItem, this);
        setAnimeEquipSword(0);
        return TRUE;
    }
    if (LK_doTrigger()) {
        if (!(mNoResetFlg0 & 0xA0000000u)) {
            if (lk_getDoStatus() == 0xB /* dActStts_OPEN_e */) {
                gabi::call(0x025D7710 /* fopAcM_orderDoorEvent */, this, mpAttnActorA.get());
                return TRUE;
            }
            if (!LK_doTrigger()) {
                goto put;
            }
        }
        if (lk_getDoStatus() == 0xE) {
            return procGrabThrow_init((mItemTrigger & 2) ? 1 : 0);
        }
    }
put:
    {
        u8 rstatus = lk_getRStatus();
        u8 t = mItemTrigger;
        if (rstatus == 0xE && ((t & 0x1F) || (t & 0x40))) { /* allTrigger() || spActionTrigger() */
            return procGrabThrow_init((t & 2) ? 1 : 0);
        }
        if (!(t & 0x1F) && !(t & 0x40)) {
            return FALSE;
        }
        if ((t & 1) && lk_getDoStatus() == 0 /* dActStts_BLANK_e */) {
            return FALSE;
        }
    }
    return procGrabPut_init();
}
VERIFY(0x023EA1AC, &daPy_lk_c::checkNextActionGrab);

/* 023EAA80 */
int daPy_lk_c::getReadyItem() {
    WWHD_FUNC(0x023EAA80, int, this);
    u8 btn = mReadyItemBtn;
    if (btn == 0 /* dItemBtn_X_e */) {
        return gabi::load<u8>(dComIfGp_ea() + 0x5BBB); /* dComIfGp_getSelectItem(dItemBtn_X_e) */
    } else if (btn == 1 /* dItemBtn_Y_e */) {
        return gabi::load<u8>(dComIfGp_ea() + 0x5BBC);
    } else if (btn == 2 /* dItemBtn_Z_e */) {
        return gabi::load<u8>(dComIfGp_ea() + 0x5BBD);
    }
    u32 play = dComIfGp_ea();
    return btn == 3 ? gabi::load<u8>(play + 0x5BBE) : gabi::load<u8>(play + 0x5BBF); /* HD: two more buttons */
}
VERIFY(0x023EAA80, &daPy_lk_c::getReadyItem);

/* 023EAB40 */
BOOL daPy_lk_c::itemTrigger() {
    WWHD_FUNC(0x023EAB40, BOOL, this);
    if (mHD8260 != 0) { /* HD */
        return FALSE;
    }
    u8 btn = mReadyItemBtn;
    u8 trig = mItemTrigger;
    if (btn == 3) { /* HD: the fourth button */
        return trig & 0x80;
    } else if (btn == 0 /* dItemBtn_X_e */) {
        return trig & 4; /* itemTriggerX() */
    } else if (btn == 1 /* dItemBtn_Y_e */) {
        return trig & 8; /* itemTriggerY() */
    }
    return trig & 0x10; /* itemTriggerZ() */
}
VERIFY(0x023EAB40, &daPy_lk_c::itemTrigger);

/* 023EAB98 */
BOOL daPy_lk_c::procHookshotFly_init() {
    WWHD_FUNC(0x023EAB98, BOOL, this);
    fopAc_ac_c* hookshot = mActorKeepEquip.mActor.get();
    gabi::call(LK_commonProcInit, this, 0x85 /* daPyProc_HOOKSHOT_FLY_e */);
    for (int i = 0; i < 3; i++) {
        gabi::call(0x024EFF3C /* dBgS_AcchCir::SetWallR */, &mAcchCir[i], 70.0f);
    }
    gabi::store<f32>(gabi::ea(&mAcchCir[0]) + 0x30, -25.0f); /* SetWallH */
    gabi::store<f32>(gabi::ea(&mAcchCir[1]) + 0x30, 0.0f);
    gabi::store<f32>(gabi::ea(&mAcchCir[2]) + 0x30, 50.0f);
    gabi::call(LK_setSingleMoveAnime, this, 0xA6 /* ANM_HOOKSHOTJMP */, 1.0f, 0.0f, -1, 0.0f);
    mNormalSpeed = 0.0f;
    LK_FIELD(s16, 0x3D0) = 0; /* mBodyAngle.x */
    gravity = 0.0f;
    m34C2 = 11;
    lk_onPlayerStatus1(0x10 /* daPyStts1_UNK10_e */);
    LK_voiceStart(6);
    u32 old_ = gabi::ea(&old.pos);
    gabi::store<u32>(gabi::ea(&m370C) + 4, gabi::load<u32>(old_ + 4)); /* m370C = old.pos (word copy) */
    gabi::store<u32>(gabi::ea(&m370C) + 8, gabi::load<u32>(old_ + 8));
    gabi::store<u32>(gabi::ea(&m370C) + 0, gabi::load<u32>(old_ + 0));
    shape_angle.x = hookshot->shape_angle.x;
    shape_angle.y = hookshot->shape_angle.y;
    return TRUE;
}
VERIFY(0x023EAB98, &daPy_lk_c::procHookshotFly_init);

/* 023EACD4 */
BOOL daPy_lk_c::cancelItemUpperReadyAnime() {
    WWHD_FUNC(0x023EACD4, BOOL, this);
    if (lk_getDoStatus() == 7 /* dActStts_RETURN_e */ && LK_doTrigger()) {
        gabi::call(LK_seStartOnlyReverb, this, 0x802 /* JA_SE_CANCEL_1 */);
        gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        procWait_init();
        return TRUE;
    }
    return FALSE;
}
VERIFY(0x023EACD4, &daPy_lk_c::cancelItemUpperReadyAnime);

/* 023EB064 */
BOOL daPy_lk_c::bowButton() {
    WWHD_FUNC(0x023EB064, BOOL, this);
    if (LK_demoMode() == 0x44) { /* checkBowMiniGame() */
        return mItemButton & 0x10; /* HD: doButton() reads 0x10 here */
    }
    return gabi::call<BOOL>(LK_itemButton, this);
}
VERIFY(0x023EB064, &daPy_lk_c::bowButton);

/* 023EB080 */
void daPy_lk_c::setBowReloadAnime() {
    WWHD_FUNC(0x023EB080, void, this);
    gabi::call(LK_setActAnimeUpper, this, 0xC /* dRes_INDEX_LKANM_BCK_ARROWRELORD_e */, 2, 1.0f, 0.0f, 6, 1.0f); /* HD: HIO folded */
    setPriTextureAnime(0x8D, 0);
    LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, 0xD /* dRes_INDEX_LKANM_BCK_ARROWRELORDA_e */));
    m35EC = 0.0f;
    gabi::call(LK_seStartOnlyReverb, this, 0x2869 /* JA_SE_LK_DRAW_BOW */);
}
VERIFY(0x023EB080, &daPy_lk_c::setBowReloadAnime);

/* dComIfGp_event_getPt1(): dEvt_control_c::convPId(play + 0x51D0, play + 0x5294) */
static inline u32 lk_event_getPt1() {
    u32 play = dComIfGp_ea();
    return gabi::call<u32>(0x0253EE04, play + 0x51D0, gabi::load<u32>(play + 0x5294));
}

/* 023EB120 */
void daPy_lk_c::makeArrow() {
    WWHD_FUNC(0x023EB120, void, this);
    BOOL make;
    if (LK_demoMode() != 0x44) { /* !checkBowMiniGame() */
        make = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x89) != 0; /* dComIfGs_getArrowNum() != 0 */
    } else {
        make = lk_event_getPt1() != 0 && gabi::load<s16>(lk_event_getPt1() + 8) == 0x76 /* fpcNm_NPC_SO_e */ &&
               (s32)(10u - gabi::load<u32>(lk_event_getPt1() + 0xCF0)) > 0; /* daNpc_So_c::getMiniGameRestArrow() > 0 (wrapping) */
    }
    if (make) {
        fopAc_ac_c* arrow_p = gabi::call<fopAc_ac_c*>(0x025D5928 /* fopAcM_fastCreate */, 0x1D8 /* fpcNm_ARROW_e */, 0, &current.pos,
                                                      (s32)current.roomNo, nullptr, nullptr, -1, 0, 0);
        gabi::call(LK_actorKeep_setData, &mActorKeepEquip, arrow_p);
    }
    m355C = 0;
}
VERIFY(0x023EB120, &daPy_lk_c::makeArrow);

/* mEquipItem == getReadyItem(), the field read after the call */
#define lk_equipIsReady() lk_equipIsReady_l(this)
static inline BOOL lk_equipIsReady_l(daPy_lk_c* t) {
    int ready = t->getReadyItem();
    return (u32)t->mEquipItem == (u32)ready;
}

/* function-local static JGeometry::TVec3<f32>: guard word, then x, z, y stored */
static inline void lk_static_vec3(u32 guard, u32 v, f32 x, f32 y, f32 z) {
    if (gabi::load<u32>(guard) == 0) {
        gabi::store<u32>(guard, 1);
        gabi::store<f32>(v + 0, x);
        gabi::store<f32>(v + 8, z);
        gabi::store<f32>(v + 4, y);
    }
}

/* 023EAD5C */
BOOL daPy_lk_c::checkNextActionHookshotReady() {
    WWHD_FUNC(0x023EAD5C, BOOL, this);
    lk_static_vec3(0x1046D098, 0x1046D0A4, 1.0f, 1.0f, 0.0f); /* eff_scale */
    lk_static_vec3(0x1046D09C, 0x1046D0B0, 0.2f, 0.2f, 0.2f); /* eff_dscale */
    lk_static_vec3(0x1046D0A0, 0x1046D0BC, 0.4f, 0.4f, 0.4f); /* eff_pscale */
    u32 hookshot = gabi::ea(mActorKeepEquip.mActor.get());
    u32 state = gabi::load<u32>(hookshot + 0xB0); /* daHookshot_c: 0 wait, 1 shot, 2 return, 3 pull (HD: +0xB0) */
    if (state == 0) { /* hookshot->checkWait() */
        BOOL shoot = FALSE;
        if (gabi::call<BOOL>(LK_itemButton, this)) {
            int ready = getReadyItem();
            if ((u32)mEquipItem == (u32)ready && m355C == 0) {
                m355E = 1;
            } else if (!gabi::call<BOOL>(LK_itemButton, this) && m355E != 0) {
                shoot = TRUE;
            }
        } else if (m355E != 0) {
            shoot = TRUE;
        }
        if (shoot) {
            m355E = 0;
            gabi::store<u32>(hookshot + 0xB0, 1); /* hookshot->setShot() */
            mNormalSpeed = 0.0f;
            gabi::store<u8>(0x101CEF18, 0); /* daPy_matAnm_c::offMabaFlg() */
            gabi::store<u8>(0x101CEF19, 1);
            gabi::call(LK_seStartOnlyReverb, this, 0x286B /* JA_SE_LK_HS_SHOOT */);
            LK_voiceStart(41);
            gabi::call(LK_resetFootEffect, this);
            if (mDirection != 2 /* DIR_LEFT */) {
                mDirection = 2;
            }
            current.angle.y = (s16)(shape_angle.y + 0x4000);
            mModeFlg = mModeFlg | 1; /* onModeFlg(ModeFlg_00000001) */
            m3598 = 0.0f;
            setBlendAtnMoveAnime(2.4f);
            mFrameCtrlUnder[0].setRate(0.0f);
            mFrameCtrlUnder[1].setRate(0.0f);
            mFrameCtrlUpper[0].setRate(1.25f); /* HD: HIO folded */
            mFrameCtrlUpper[1].setRate(0.0f);
            mFrameCtrlUpper[2].setRate(0.0f);
            return TRUE;
        }
    } else if (state == 1) { /* hookshot->checkShot() */
        if (itemTrigger() && lk_equipIsReady()) {
            gabi::store<u32>(hookshot + 0xB0, 2); /* hookshot->setReturn() */
        }
        return TRUE;
    } else if (state == 2) { /* hookshot->checkReturn() */
        return TRUE;
    } else if (state == 3) { /* hookshot->checkPull() */
        procHookshotFly_init();
        return TRUE;
    }
    if (m355C > 0) {
        m355C = (s16)(m355C - 1);
    }
    return cancelItemUpperReadyAnime();
}
VERIFY(0x023EAD5C, &daPy_lk_c::checkNextActionHookshotReady);

/* 023EB208 */
BOOL daPy_lk_c::checkNextActionBowReady() {
    WWHD_FUNC(0x023EB208, BOOL, this);
    u16 upper = LK_upperIdx();
    if (upper == 0x36) { /* checkBowWaitAnime() */
        s16 shot = m355C;
        u32 arrow = gabi::ea(mActorKeepEquip.mActor.get());
        if (shot != 0) {
            /* HD: HIO folded */
            gabi::call(LK_setActAnimeUpper, this, 0xE /* dRes_INDEX_LKANM_BCK_ARROWSHOOT_e */, 2, 0.9f, 0.0f, 4, 0.0f);
            setPriTextureAnime(0x70, 0);
            LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, 0xF /* dRes_INDEX_LKANM_BCK_ARROWSHOOTA_e */));
            m35EC = 0.0f;
        } else if (!bowButton()) {
            if (arrow != 0) {
                gabi::store<u32>(arrow + 0xB0, 1); /* fopAcM_SetParam(arrow, 1) */
                if (LK_demoMode() != 0x44) { /* !checkBowMiniGame() */
                    u32 a = dComIfGp_ea() + 0x5B68; /* dComIfGp_setItemArrowNumCount(-1) */
                    gabi::store<s16>(a, (s16)(gabi::load<s16>(a) - 1));
                }
                gabi::call(0x023DC63C /* daPy_actorKeep_c::clearData */, &mActorKeepEquip);
                gabi::call(LK_seStartOnlyReverb, this, 0x286A /* JA_SE_LK_SHOOT_ARROW */);
                LK_voiceStart(41);
                setResetFlg0(resetFlg0() | 0x20000000); /* onResetFlg0(daPyRFlg0_ARROW_SHOOT) */
            }
            m355C = 1;
        } else if (gabi::call<BOOL>(LK_daArrow_changeArrowMp)) {
            if (mEquipItem != 0x27 /* dItemNo_BOW_e */) {
                lk_setRStatus(0x22); /* dActStts_SWAP_MODES_e */
                if ((mItemTrigger & 0x40) /* spActionTrigger() */ && arrow != 0) {
                    u32 na = gabi::call<u32>(0x0205584C /* daArrow_c::changeArrowType */, arrow);
                    gabi::call(LK_actorKeep_setData, &mActorKeepEquip, na);
                    setBowReloadAnime();
                }
            }
        }
    } else if (upper == 0xC) { /* checkBowReloadAnime() */
        if (mFrameCtrlUpper[2].getRate() < 0.01f) {
            gabi::call(LK_setActAnimeUpper, this, 0x36 /* dRes_INDEX_LKANM_BCK_BOWWAIT_e */, 2, 1.0f, 0.0f, -1, -1.0f);
            LK_swordAnim_changeBckOnly(gabi::call<u32>(LK_getItemAnimeResource, this, 0x37 /* dRes_INDEX_LKANM_BCK_BOWWAITA_e */));
            m35EC = 0.0f;
            setPriTextureAnime(0x70, 0);
        }
    } else if (upper == 0xE) { /* checkBowShootAnime() */
        if (mFrameCtrlUpper[2].getRate() < 0.01f) {
            if (m355E == 0 && bowButton() && (LK_demoMode() == 0x44 || lk_equipIsReady()) &&
                !(noResetFlg1() & 0x2000) /* !checkUseArrowEffect() */) {
                setBowReloadAnime();
                makeArrow();
                if (mActorKeepEquip.mActor.get() == nullptr) {
                    lk_seStartSystem(0x883 /* JA_SE_ITEM_TARGET_OUT */);
                }
            } else if (gabi::call<BOOL>(LK_daArrow_changeArrowMp)) {
                if (mEquipItem != 0x27 /* dItemNo_BOW_e */) {
                    lk_setRStatus(0x22);
                    if (mItemTrigger & 0x40) {
                        gabi::call(0x0205578C /* daArrow_c::changeArrowTypeNotReady */);
                    }
                }
            }
        }
        if (m355E != 0) {
            m355E = (s16)(m355E - 1);
        }
    }
    return cancelItemUpperReadyAnime();
}
VERIFY(0x023EB208, &daPy_lk_c::checkNextActionBowReady);

/* 023EB510 */
BOOL daPy_lk_c::checkNextActionBoomerangReady() {
    WWHD_FUNC(0x023EB510, BOOL, this);
    /* HD: the boomerang is thrown when its button is released (pad release bits *(0x101F5088) + 0x1C), or after
     * holding it for 12 frames; the GameCube !itemButton() test is gone */
    u8 btn = mReadyItemBtn;
    u32 rel = gabi::load<u32>(gabi::load<u32>(0x101F5088) + 0x1C);
    BOOL released;
    if (btn == 0) {
        released = (rel & 8) != 0;
    } else if (btn == 1) {
        released = (rel & 0x10) != 0;
    } else {
        released = (rel & 0x4000) != 0;
    }
    BOOL throwIt = FALSE;
    if (released) {
        if (m355E != 0 || m355C >= 0xC) {
            mHD827C = 1;
            m355C = 0;
            throwIt = TRUE;
        } else {
            m355E = 1;
            m355C = 0;
            throwIt = mHD827C != 0;
        }
    } else {
        if (gabi::call<BOOL>(LK_itemButton, this)) {
            m355C = (s16)(m355C + 1);
        }
        throwIt = mHD827C != 0;
    }
    if (throwIt) {
        if (mpAttnActorLockOn.get() == nullptr) {
            s32 idx = mCameraInfoIdx;
            /* dComIfGp_checkCameraAttentionStatus(mCameraInfoIdx, dCamAttnStts_00000010_e) */
            if (!(gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5B00) & 0x10)) {
                return cancelItemUpperReadyAnime();
            }
        }
        /* HD: HIO folded */
        gabi::call(LK_setActAnimeUpper, this, 0x34 /* dRes_INDEX_LKANM_BCK_BOOMTHROW_e */, 2, 1.5f, 2.0f, 10, 3.0f);
        setPriTextureAnime(0x48, 0);
        mHD827C = 0;
        mHDAngleY = shape_angle.y;
        return TRUE;
    }
    return cancelItemUpperReadyAnime();
}
VERIFY(0x023EB510, &daPy_lk_c::checkNextActionBoomerangReady);

/* HD: SafeString == of a C string and a buffer (the C string's termination is assured twice, as compiled) */
static inline BOOL lk_strEq2(gabi::Local<SafeString>& a, gabi::Local<SafeString>& b, u32 str, u32 buf) {
    a->__vtbl = LK_SafeString_vtbl;
    a->mStringTop = str;
    b->__vtbl = LK_SafeString_vtbl;
    b->mStringTop = buf;
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.a);
    gabi::call_ptr(gabi::load<u32>(a->__vtbl + 0x14), a.a);
    u32 s1 = a->mStringTop;
    gabi::call_ptr(gabi::load<u32>(b->__vtbl + 0x14), b.a);
    if (s1 == b->mStringTop) {
        return TRUE;
    }
    return lk_safeStringEq(a->mStringTop, b->mStringTop);
}

/* 023EB6C8 */
BOOL daPy_lk_c::checkBossGomaStage() {
    WWHD_FUNC(0x023EB6C8, BOOL, this);
    gabi::Local<SafeString> a, b, c, d;
    /* dComIfGp_getStartStageName(): play + 0x5134 */
    if (lk_strEq2(a, b, 0x100356C0 /* "M_DragB" */, dComIfGp_ea() + 0x5134)) {
        return TRUE;
    }
    return lk_strEq2(c, d, 0x100356C8 /* "Xboss0" */, dComIfGp_ea() + 0x5134);
}
VERIFY(0x023EB6C8, &daPy_lk_c::checkBossGomaStage);

/* 023EB864 */
BOOL daPy_lk_c::procRopeReady_init() {
    WWHD_FUNC(0x023EB864, BOOL, this);
    fopAc_ac_c* rope = mActorKeepEquip.mActor.get();
    gabi::call(LK_commonProcInit, this, 0x77 /* daPyProc_ROPE_READY_e */);
    gabi::call(LK_setMoveAnime, this, 0.5f, 1.0f, 1.0f, 0x75 /* ANM_ROPECATCH */, 0x77 /* ANM_ROPESWINGB */, 7, 3.0f); /* HD: HIO folded */
    u32 anm = gabi::load<u32>(gabi::ea(this) + 0x57FC); /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform() */
    mFrameCtrlUnder[0].mStart = 0;
    mFrameCtrlUnder[0].mEnd = 7;
    mFrameCtrlUnder[0].mFrame = 0.0f; /* HD: the frame is set to the start (0) directly */
    gabi::store<f32>(anm, 0.0f);       /* ->setFrame() */
    fcpy_l(gabi::ea(&m3688.x), gabi::ea(&current.pos.x)); /* m3688 = current.pos */
    fcpy_l(gabi::ea(&m35F0), gabi::ea(&current.pos.y));
    fcpy_l(gabi::ea(&m35F4), gabi::ea(&current.pos.y));
    fcpy_l(gabi::ea(&m3688.y), gabi::ea(&current.pos.y));
    fcpy_l(gabi::ea(&m3688.z), gabi::ea(&current.pos.z));
    f32 y = current.pos.y;
    current.pos.y = gabi::fadds_ppc(y, gabi::load<f32>(0x1046CCEC)); /* HD: += a global (GameCube 95.0f) */
    f32 m3600v = m3600;
    fcpy_l(gabi::ea(&m370C.x), gabi::ea(&rope->current.pos.x));
    if (!(m3600v < 0.0f)) { /* m3600 >= 0.0f (HD: NaN too) */
        f32 v = rope->current.pos.y - m3600v;
        m370C.y = v;
        m3600 = v;
    } else if (checkBossGomaStage() && gabi::load<f32>(gabi::ea(&rope->current.pos.y)) - current.pos.y > 700.0f) {
        f32 v = rope->current.pos.y - 700.0f;
        m370C.y = v;
        m3600 = v;
    } else {
        u32 gnd = gabi::ea(this) + 0xB14; /* mGndChk.SetPos(&rope->current.pos) */
        fcpy_l(gnd + 0x28, gabi::ea(&rope->current.pos.y));
        fcpy_l(gnd + 0x24, gabi::ea(&rope->current.pos.x));
        fcpy_l(gnd + 0x2C, gabi::ea(&rope->current.pos.z));
        f64 dVar4 = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), gnd);
        f32 py = current.pos.y;
        if (dVar4 > (f64)(f32)(py + -175.0f)) { /* HD: (y - 125) - 50 folded */
            f32 v = (f32)(dVar4 + 175.0);
            m370C.y = v;
            m3600 = v;
        } else {
            fcpy_l(gabi::ea(&m3600), gabi::ea(&current.pos.y));
            fcpy_l(gabi::ea(&m370C.y), gabi::ea(&current.pos.y));
        }
    }
    gravity = 0.0f;
    fcpy_l(gabi::ea(&m370C.z), gabi::ea(&rope->current.pos.z));
    mNormalSpeed = 0.0f;
    m35A0 = 0.0f;
    gabi::store<u32>(gabi::ea(rope) + 0xB0, 3); /* fopAcM_SetParam(rope, 3) */
    mProcVar2 = 0x1800;
    s16 a = cM_atan2s(m370C.x - current.pos.x, m370C.z - current.pos.z);
    current.angle.y = a;
    shape_angle.y = a;
    u32 rp = gabi::ea(this) + 0x408; /* mRopePos = rope->current.pos (daPy_py_c, GameCube 0x2EC) */
    gabi::store<u32>(rp + 0, gabi::load<u32>(gabi::ea(rope) + 0x314));
    gabi::store<u32>(rp + 4, gabi::load<u32>(gabi::ea(rope) + 0x318));
    gabi::store<u32>(rp + 8, gabi::load<u32>(gabi::ea(rope) + 0x31C));
    lk_onPlayerStatus0(0x800000 /* daPyStts0_UNK800000_e */);
    return TRUE;
}
VERIFY(0x023EB864, &daPy_lk_c::procRopeReady_init);

/* 023EBC1C */
BOOL daPy_lk_c::procRopeThrowCatch_init() {
    WWHD_FUNC(0x023EBC1C, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x7E /* daPyProc_ROPE_THROW_CATCH_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x7B /* ANM_ROPETHROWCATCH */, 0.8f, 3.0f, 0xB, 0.0f); /* HD: HIO folded */
    mNormalSpeed = 0.0f;
    lk_onPlayerStatus0(0x20000 /* daPyStts0_ROPE_AIM_e */);
    mProcVar0 = 0x1E;
    return TRUE;
}
VERIFY(0x023EBC1C, &daPy_lk_c::procRopeThrowCatch_init);

/* 023EBCB0 */
BOOL daPy_lk_c::checkSightLine(f32 param_1, cXyz* param_2) {
    WWHD_FUNC(0x023EBCB0, BOOL, this, param_1, param_2);
    s32 idx = mCameraInfoIdx;
    u32 cam = gabi::load<u32>(dComIfGp_ea() + idx * 0x34 + 0x5AF8); /* dComIfGp_getCamera(mCameraInfoIdx) */
    cXyz* eye = gabi::at<cXyz>(cam + 0xDC);    /* fopCamM_GetEye_p */
    cXyz* center = gabi::at<cXyz>(cam + 0xE8); /* fopCamM_GetCenter_p */
    gabi::Local<cXyz> local_44, n, s, r;
    cXyz_mi(center, local_44, eye);
    gabi::call(0x0201B31C /* cXyz::normalize */, local_44.a, n.a);
    gabi::call(0x0201AE48 /* cXyz::operator* */, n.a, s.a, param_1);
    gabi::call(0x0201AD78 /* cXyz::operator+ */, s.a, r.a, eye);
    u32 p2 = gabi::ea(param_2);
    gabi::store<u32>(p2 + 0, gabi::load<u32>(r.a + 0));
    gabi::store<u32>(p2 + 4, gabi::load<u32>(r.a + 4));
    gabi::store<u32>(p2 + 8, gabi::load<u32>(r.a + 8));
    u32 linChk = mEquipItem == 0x2D /* dItemNo_BOOMERANG_e */ ? gabi::ea(this) + 0xAA8 /* mBoomerangLinChk */ : gabi::ea(this) + 0xA3C /* mRopeLinChk */;
    gabi::call(LK_dBgS_LinChk_Set, linChk, eye, param_2, this);
    BOOL temp_r3 = lk_LineCross(linChk);
    if (temp_r3) {
        gabi::store<u32>(p2 + 0, gabi::load<u32>(linChk + 0x30)); /* *linChk->GetCrossP() */
        gabi::store<u32>(p2 + 4, gabi::load<u32>(linChk + 0x34));
        gabi::store<u32>(p2 + 8, gabi::load<u32>(linChk + 0x38));
    }
    return temp_r3;
}
VERIFY(0x023EBCB0, &daPy_lk_c::checkSightLine);

/* 023EBE04 */
int daPy_lk_c::throwRope() {
    WWHD_FUNC(0x023EBE04, int, this);
    u32 rope = gabi::ea(mActorKeepEquip.mActor.get()); /* himo2_class* */
    gabi::call(LK_setActAnimeUpper, this, 0xE2 /* dRes_INDEX_LKANM_BCK_ROPETHROW_e */, 2, 1.3f, 1.0f, 0xB, -1.0f); /* HD: HIO folded */
    setPriTextureAnime(0x48, 0);
    mNormalSpeed = 0.0f;
    if (mDirection != 3 /* DIR_RIGHT */) {
        mDirection = 3;
    }
    current.angle.y = (s16)(shape_angle.y - 0x4000);
    mModeFlg = mModeFlg | 1; /* onModeFlg(ModeFlg_00000001) */
    setBlendAtnMoveAnime(0.0f);
    gabi::Local<cXyz> local_1c;
    if (mCurProc == 0x7D /* daPyProc_ROPE_MOVE_e */) {
        gabi::Local<cXyz> local_28;
        u32 lock = gabi::ea(mpAttnActorLockOn.get());
        f32 py = current.pos.y + 100.0f;
        if (lock != 0) {
            gabi::store<u32>(local_1c.a + 0, gabi::load<u32>(lock + 0x37C)); /* local_1c = lockOn->eyePos */
            gabi::store<u32>(local_1c.a + 4, gabi::load<u32>(lock + 0x380));
            gabi::store<u32>(local_1c.a + 8, gabi::load<u32>(lock + 0x384));
        } else {
            s16 bx = LK_FIELD(s16, 0x3D0); /* mBodyAngle.x */
            f32 fVar3 = 500.0f * cM_scos(bx);
            local_1c->x = gabi::fmadds(fVar3, cM_ssin(shape_angle.y), current.pos.x);
            local_1c->y = gabi::fnmsubs(500.0f, cM_ssin(bx), py);
            local_1c->z = gabi::fmadds(fVar3, cM_scos(shape_angle.y), current.pos.z);
        }
        fcpy_l(local_28.a + 0, gabi::ea(&current.pos.x));
        local_28->y = py;
        fcpy_l(local_28.a + 8, gabi::ea(&current.pos.z));
        u32 chk = gabi::ea(this) + 0xA3C; /* mRopeLinChk */
        gabi::call(LK_dBgS_LinChk_Set, chk, local_28.a, local_1c.a, this);
        if (lk_LineCross(chk)) {
            gabi::store<u32>(local_1c.a + 0, gabi::load<u32>(chk + 0x30)); /* local_1c = *mRopeLinChk.GetCrossP() */
            gabi::store<u32>(local_1c.a + 8, gabi::load<u32>(chk + 0x38));
            gabi::store<u32>(local_1c.a + 4, gabi::load<u32>(chk + 0x34));
        }
    } else {
        checkSightLine(2200.0f, local_1c);
    }
    gabi::call(0x0216EEB8 /* himo2_class::setTargetPos (search_target) */, rope, local_1c.a, &m3600, &m3604);
    gabi::store<u32>(rope + 0xB0, 1); /* fopAcM_SetParam(rope, 1) */
    gabi::call(LK_seStartOnlyReverb, this, 0x2817 /* JA_SE_LK_ROPE_LAUNCH */);
    gabi::store<u8>(gabi::ea(this) + 0x58EC, 0); /* mSightPacket.offDrawFlg() */
    gabi::call(LK_resetFootEffect, this);
    return TRUE;
}
VERIFY(0x023EBE04, &daPy_lk_c::throwRope);

/* 023EC084 */
BOOL daPy_lk_c::checkNextActionRopeReady() {
    WWHD_FUNC(0x023EC084, BOOL, this);
    u32 rope = gabi::ea(mActorKeepEquip.mActor.get());
    u32 uVar2 = gabi::load<u32>(rope + 0xB0); /* fopAcM_GetParam(rope) */
    if (uVar2 == 2) {
        return procRopeReady_init();
    }
    if (uVar2 == 0) {
        if (LK_upperIdx() == 0xE2 /* dRes_INDEX_LKANM_BCK_ROPETHROW_e */) {
            if (mFrameCtrlUpper[2].getRate() < 0.01f) {
                return procRopeThrowCatch_init();
            }
            return TRUE;
        } else if (!lk_checkAttentionLock()) {
            lk_setDoStatus(7); /* dActStts_RETURN_e */
        }
        BOOL doThrow = FALSE;
        if (gabi::call<BOOL>(LK_itemButton, this)) {
            if (lk_equipIsReady() && m355C == 0) {
                m355E = 1;
            } else if (!gabi::call<BOOL>(LK_itemButton, this) && m355E != 0) {
                doThrow = TRUE;
            }
        } else if (m355E != 0) {
            doThrow = TRUE;
        }
        if (doThrow) {
            m355E = 0;
            if (throwRope()) {
                return TRUE;
            }
        }
        if (cancelItemUpperReadyAnime()) {
            return TRUE;
        }
    } else {
        s32 st = gabi::load<s32>(rope + 0x3F8); /* rope->m02DC */
        if (st == 5 || st == 8 || st == 9) {
            return procRopeThrowCatch_init();
        }
        checkNextRopeMode();
        return TRUE;
    }
    if (m355C > 0) {
        m355C = (s16)(m355C - 1);
    }
    return FALSE;
}
VERIFY(0x023EC084, &daPy_lk_c::checkNextActionRopeReady);

/* 023EC260 */
BOOL daPy_lk_c::setTalkStatus() {
    WWHD_FUNC(0x023EC260, BOOL, this);
    if (mNoResetFlg0 & 0xA0000000u) {
        return FALSE;
    }
    if (mpAttnEntryA == 0) {
        return FALSE;
    }
    s32 type = gabi::load<s32>(mpAttnEntryA + 8); /* mpAttnEntryA->mType */
    u32 flags;
    if (type == 3 /* fopAc_Attn_TYPE_SPEAK_e */) {
        flags = gabi::load<u32>(gabi::ea(mpAttnActorA.get()) + 0x39C); /* attention_info.flags */
    } else if (type == 1 /* fopAc_Attn_TYPE_TALK_e */ && mpAttnActorA.get() == mpAttnActorLockOn.get()) {
        flags = gabi::load<u32>(gabi::ea(mpAttnActorA.get()) + 0x39C);
    } else {
        return FALSE;
    }
    if (flags & 0x2000000) { /* fopAc_Attn_TALKFLAG_NOTALK_e */
        return FALSE;
    }
    if (flags & 0x20000000) {        /* fopAc_Attn_TALKFLAG_CHECK_e */
        lk_setDoStatus(0xA);         /* dActStts_CHECK_e */
    } else if (flags & 0x40000000) { /* fopAc_Attn_TALKFLAG_READ_e */
        lk_setDoStatus(0x26);        /* dActStts_READ_e */
    } else {
        BOOL look = (flags & 0x8000000) != 0; /* fopAc_Attn_TALKFLAG_LOOK_e */
        lk_setDoStatus(look ? 1 /* dActStts_LOOK_e */ : 2 /* dActStts_SPEAK_e */);
    }
    return TRUE;
}
VERIFY(0x023EC260, &daPy_lk_c::setTalkStatus);

/* 023EC370 */
void daPy_lk_c::setSpecialBattle(BOOL param_0) {
    WWHD_FUNC(0x023EC370, void, this, param_0);
    if (param_0) {
        /* strcmp(dComIfGp_getStartStageName(), "GTower") != 0 */
        u32 a = dComIfGp_ea() + 0x5134;
        u32 b = 0x100356E0;
        u8 c1, c2;
        do {
            c1 = gabi::load<u8>(a++);
            c2 = gabi::load<u8>(b++);
        } while (c1 == c2 && c1 != 0);
        if (c1 != c2) {
            return;
        }
    }
    if (mpAttnEntryA == 0) {
        return;
    }
    if (gabi::load<s32>(mpAttnEntryA + 8) != 2 /* fopAc_Attn_TYPE_BATTLE_e */) {
        return;
    }
    fopEn_enemy_c* enemy = gabi::at<fopEn_enemy_c>(gabi::ea(mpAttnActorLockOn.get()));
    if (gabi::ea(enemy) == 0) {
        return;
    }
    if (enemy->group != 2 /* fopAc_ENEMY_e */) {
        return;
    }
    if (gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0x3C), this)) { /* virtual checkPlayerGuard() */
        return;
    }
    if (gabi::load<u16>(0x101CEF16) == 1) { /* daPy_dmEcallBack_c::checkCurse() (HD: a global) */
        return;
    }
    if (mEquipItem != 0x103 /* daPyItem_SWORD_e */) {
        return;
    }
    if (enemy->mBtAttackType == 0) {
        return;
    }
    /* HD: the NaN-safe forms of the original's branches; HIO folded (1.5) */
    f32 now = enemy->mBtNowFrame;
    if (enemy->mBtStartFrame - 1.5f > now) {
        return;
    }
    if (!(now < enemy->mBtEndFrame)) {
        return;
    }
    f64 dist = gabi::call<f64>(0x025D6958 /* fopAcM_searchActorDistanceXZ */, this, enemy);
    if ((f64)enemy->mBtMaxDis > dist) {
        lk_setDoStatus(0x1A); /* dActStts_PARRY_e */
    }
}
VERIFY(0x023EC370, &daPy_lk_c::setSpecialBattle);

/* HD: a status byte is only set while it is still blank (read and written through two play-object lookups) */
static inline void lk_setStatusIfBlank(u32 off, u8 v) {
    if (gabi::load<u8>(dComIfGp_ea() + off) == 0) {
        gabi::store<u8>(dComIfGp_ea() + off, v);
    }
}

/* 023EA680 */
void daPy_lk_c::setDoStatusBasic() {
    WWHD_FUNC(0x023EA680, void, this);
    /* HD: no up-front dComIfGp_getDoStatus() == BLANK test; each status (do 0x5BB7, HD 0x5BB6 for the
     * enemy-weapon throw) is set only while blank */
    int direction = gabi::call<int>(LK_getDirectionFromShapeAngle, this);
    f32 fVar1 = gabi::call<BOOL>(LK_checkHeavyStateOn, this) ? 0.375f : 0.75f; /* HD: HIO folded */
    if (lk_checkAttentionLock() || (mActorKeepThrow.mActor.get() != nullptr && mpAttnActorLockOn.get() == mActorKeepThrow.mActor.get())) {
        if (mStickDistance > 0.05f && direction != 0 /* DIR_FORWARD */) {
            if (mEquipItem == 0x101 /* daPyItem_BOKO_e */ && direction == 1 /* DIR_BACKWARD */) {
                lk_setStatusIfBlank(0x5BB6, 0xE); /* dActStts_THROW_e */
            } else {
                lk_setStatusIfBlank(0x5BB7, 0x12); /* dActStts_JUMP_e */
            }
            return;
        }
        BOOL guard = gabi::call_ptr<BOOL>(gabi::load<u32>(__vtbl + 0x3C), this); /* virtual checkPlayerGuard() */
        u16 e = mEquipItem;
        if (!guard) {
            if (e == 0x101 /* daPyItem_BOKO_e */) {
                lk_setStatusIfBlank(0x5BB6, 0xE);
                return;
            }
            if (e == 0x103 /* daPyItem_SWORD_e */ || e == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
                lk_setStatusIfBlank(0x5BB7, 0x51); /* dActStts_UNK43 (HD 0x51) */
                return;
            }
        }
        if (e == 0x101) {
            lk_setStatusIfBlank(0x5BB6, 0xE);
        } else {
            lk_setStatusIfBlank(0x5BB7, 0xC); /* dActStts_ATTACK_e */
        }
        return;
    }
    if (mStickDistance > fVar1) {
        u16 e = mEquipItem;
        if (e == 0x101) {
            lk_setStatusIfBlank(0x5BB6, 0xE);
        } else {
            lk_setStatusIfBlank(0x5BB7, 0xC); /* dActStts_ATTACK_e */
        }
        return;
    }
    if (!LK_checkNoUpperAnime()) {
        return;
    }
    u16 e = mEquipItem;
    if (e == 0x100 /* daPyItem_NONE_e */ || !(mModeFlg & 4) /* checkModeFlg(ModeFlg_00000004) */) {
        return;
    }
    if (e == 0x101) {
        lk_setStatusIfBlank(0x5BB6, 0xE);
    } else {
        lk_setStatusIfBlank(0x5BB7, 8); /* dActStts_PUT_AWAY_e */
    }
}
VERIFY(0x023EA680, &daPy_lk_c::setDoStatusBasic);

/* 023EC498 */
void daPy_lk_c::setDoStatus() {
    WWHD_FUNC(0x023EC498, void, this);
    if (setHintActor()) {
        return;
    }
    u32 entry = mpAttnEntryA;
    if (entry == 0) {
        if (mpAttnActorLockOn.get() == nullptr) {
            if (resetFlg0() & 8) { /* checkResetFlg0(daPyRFlg0_UNK8) */
                lk_setDoStatus(0x11); /* HD: no RStatus / climb status */
                return;
            }
            if (mFrontWallType == 2) {
                /* HD: no sidle status in "Obombh" */
                gabi::Local<SafeString> a, b;
                if (lk_strEq2(a, b, 0x100356E8 /* "Obombh" */, dComIfGp_ea() + 0x5134 /* dComIfGp_getStartStageName() */)) {
                    return;
                }
                lk_setDoStatus(0x10); /* dActStts_SIDLE_e */
                return;
            }
        }
        setDoStatusBasic();
        return;
    }
    s32 type = gabi::load<s32>(entry + 8); /* mpAttnEntryA->mType */
    if (!(mNoResetFlg0 & 0xA0000000u)) {
        if (type == 5 /* fopAc_Attn_TYPE_DOOR_e */ || type == 6 /* fopAc_Attn_TYPE_TREASURE_e */) {
            lk_setDoStatus(0xB); /* dActStts_OPEN_e */
            setDoStatusBasic();
            return;
        }
        if (type == 7 /* fopAc_Attn_TYPE_SHIP_e */) {
            if (!lk_checkPlayerStatus0(0x10000 /* daPyStts0_SHIP_RIDE_e */)) {
                lk_setDoStatus(0x1C); /* dActStts_GET_IN_SHIP_e */
            }
            setDoStatusBasic();
            return;
        }
    }
    if (type == 4 /* fopAc_Attn_TYPE_CARRY_e */) {
        u32 actorA = gabi::ea(mpAttnActorA.get());
        if (!(gabi::load<u32>(actorA + 0x2E0) & 0x2000)) { /* !fopAcM_CheckStatus(mpAttnActorA, fopAcStts_CARRY_e) */
            if (actorA != 0 && gabi::load<s16>(actorA + 8) == 0x1CF /* fpcNm_BOKO_e */) {
                lk_setDoStatus(0x1B); /* dActStts_PICK_UP_e */
            } else {
                lk_setDoStatus(4); /* dActStts_LIFT_e */
            }
        }
    } else if (!setTalkStatus()) {
        setSpecialBattle(0);
    }
    setDoStatusBasic();
}
VERIFY(0x023EC498, &daPy_lk_c::setDoStatus);

/* 023ECC68 */
void daPy_lk_c::setAnimeUnequipSword() {
    WWHD_FUNC(0x023ECC68, void, this);
    /* HD: HIO folded */
    if ((mModeFlg & 0x01FD2810) || gabi::load<u8>(dComIfGp_ea() + 0x5292) /* dComIfGp_event_runCheck() */ ||
        gabi::load<u16>(gabi::ea(this) + 0x420) != 0 /* checkPlayerDemoMode() */) {
        gabi::call(LK_setActAnimeUpper, this, 0xD7 /* dRes_INDEX_LKANM_BCK_REST_e */, 2, 1.1f, 0.0f, 0xC, 6.0f);
    } else {
        gabi::call(LK_setActAnimeUpper, this, 0xD7, 2, 0.8f, 0.0f, 0xF, 3.0f);
    }
    setPriTextureAnime(0x72, 0);
}
VERIFY(0x023ECC68, &daPy_lk_c::setAnimeUnequipSword);

/* 023ECD3C */
void daPy_lk_c::setAnimeEquipSingleItem(u16 bckIdx) {
    WWHD_FUNC(0x023ECD3C, void, this, bckIdx);
    gabi::call(LK_setActAnimeUpper, this, (u32)bckIdx, 2 /* UPPER_MOVE2_e */, 0.7f, 1.0f, 0xA, 3.0f); /* HD: HIO folded */
    setPriTextureAnime(0x74, 0);
}
VERIFY(0x023ECD3C, &daPy_lk_c::setAnimeEquipSingleItem);

/* 023ECD98 */
void daPy_lk_c::setAnimeUnequipItem(u16 i_itemNo) {
    WWHD_FUNC(0x023ECD98, void, this, i_itemNo);
    if (i_itemNo == 0x2D /* dItemNo_BOOMERANG_e */ || i_itemNo == 0x25 /* dItemNo_GRAPPLING_HOOK_e */ ||
        i_itemNo == 0x20 /* dItemNo_TELESCOPE_e */ || gabi::call<BOOL>(LK_checkPhotoBoxItem, this, (u32)i_itemNo) ||
        (i_itemNo >= 0x21 && i_itemNo <= 0x22) /* dItemNo_TINGLE_TUNER_e, dItemNo_WIND_WAKER_e (HD order) */ ||
        i_itemNo == 0x2F /* dItemNo_HOOKSHOT_e */ || i_itemNo == 0x34 /* dItemNo_DEKU_LEAF_e */ ||
        gabi::call<BOOL>(LK_checkBottleItem, this, (u32)i_itemNo)) {
        setAnimeEquipSingleItem(0x105 /* dRes_INDEX_LKANM_BCK_TAKEL_e */);
    } else if (gabi::call<BOOL>(LK_checkBowItem, this, (u32)i_itemNo)) {
        setAnimeEquipSingleItem(0x106 /* dRes_INDEX_LKANM_BCK_TAKER_e */);
    } else if (i_itemNo == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        gabi::call(LK_setActAnimeUpper, this, 0x104 /* dRes_INDEX_LKANM_BCK_TAKEBOTH_e */, 2, 0.8f, 3.0f, 7, 4.0f); /* HD: HIO folded */
    } else {
        gabi::call(LK_setActAnimeUpper, this, 0x103 /* dRes_INDEX_LKANM_BCK_TAKE_e */, 2, 1.0f, 2.0f, 0xF, 1.0f);
        setPriTextureAnime(0x73, 0);
    }
}
VERIFY(0x023ECD98, &daPy_lk_c::setAnimeUnequipItem);

/* 023ECEF0 */
void daPy_lk_c::setAnimeUnequip() {
    WWHD_FUNC(0x023ECEF0, void, this);
    u16 e = mEquipItem;
    if (e == 0x103 /* daPyItem_SWORD_e */) {
        setAnimeUnequipSword();
    } else if (e == 0x101 /* daPyItem_BOKO_e */) {
        gabi::call(LK_deleteEquipItem, this, 0);
        gabi::call(0x025E3F3C /* mDoExt_MtxCalcOldFrame::initOldFrameMorf */, (u32)m_old_fdata, 5.0f, 0, 0x2A);
    } else {
        setAnimeUnequipItem(e);
    }
    m3562 = 0x100; /* daPyItem_NONE_e */
}
VERIFY(0x023ECEF0, &daPy_lk_c::setAnimeUnequip);

/* 023ECF74 */
BOOL daPy_lk_c::procPushPullWait_init(int param_0) {
    WWHD_FUNC(0x023ECF74, BOOL, this, param_0);
    gabi::call(LK_commonProcInit, this, 0x32 /* daPyProc_PUSH_PULL_WAIT_e */);
    mProcVar6 = param_0;
    mNormalSpeed = 0.0f;
    if (param_0 == 0 || mEquipItem == 0x100 /* daPyItem_NONE_e */) {
        if (gabi::call<BOOL>(LK_checkEquipAnime, this)) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        }
        gabi::store<u8>(0x101CEF18, 0); /* daPy_matAnm_c::offMabaFlg() */
        gabi::store<u8>(0x101CEF19, 1);
        gabi::call(LK_setSingleMoveAnime, this, 0x7E /* ANM_WAITPUSHPULL */, 1.0f, 0.0f, -1, 5.0f); /* HD: HIO folded */
        mProcVar3 = 0;
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f);
        setAnimeUnequip();
        m3598 = 0.0f;
        mProcVar3 = 1;
    }
    if (param_0 != 0) {
        s16 a = m352C;
        mProcVar2 = (s16)(a + 0x8000);
        m370C.x = gabi::fmadds(40.0f, cM_ssin(a), m3724.x);
        fcpy_l(gabi::ea(&m370C.y), gabi::ea(&current.pos.y));
        m370C.z = gabi::fmadds(40.0f, cM_scos(a), m3724.z);
    }
    lk_onPlayerStatus0(0x4000000 /* daPyStts0_UNK4000000_e */);
    return TRUE;
}
VERIFY(0x023ECF74, &daPy_lk_c::procPushPullWait_init);

/* 023ED0F0 */
BOOL daPy_lk_c::procGrabReady_init() {
    WWHD_FUNC(0x023ED0F0, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x6E /* daPyProc_GRAB_READY_e */);
    mNormalSpeed = 0.0f;
    if (mEquipItem == 0x100 /* daPyItem_NONE_e */) {
        if (gabi::call<BOOL>(LK_checkEquipAnime, this)) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        }
        gabi::call(LK_setSingleMoveAnime, this, 0x65 /* ANM_GRABP */, 0.8f, 0.0f, 4, 1.0f); /* HD: HIO folded */
        mProcVar6 = 1;
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f);
        setAnimeUnequip();
        mProcVar6 = 0;
    }
    BOOL glove = gabi::load<u8>(gabi::load<u32>(0x101F84DC) + 0x30) == 0x28; /* checkPowerGloveEquip() */
    u32 actorA = gabi::ea(mpAttnActorA.get());
    u32 st = gabi::load<u32>(actorA + 0x2E0);
    if ((glove || !(st & 0x10000) /* fopAcStts_UNK10000_e */) && !(st & 0x2000) /* fopAcStts_CARRY_e */) {
        gabi::call(0x025D9D0C /* fopAcM_setCarryNow */, actorA, 1);
        gabi::call(LK_actorKeep_setData, &mActorKeepGrab, mpAttnActorA.get());
        gabi::call(0x023DC63C /* daPy_actorKeep_c::clearData */, &mActorKeepRope);
    } else {
        gabi::call(LK_actorKeep_setData, &mActorKeepRope, actorA);
        gabi::call(LK_freeGrabItem, this);
    }
    fopAc_ac_c* a = mpAttnActorA.get();
    if (a != nullptr && fpcM_GetName(a) == 0x1CF /* fpcNm_BOKO_e */) {
        gabi::store<u8>(gabi::ea(a) + 0x430, 3); /* boko->setNowMode(daBoko_c::Mode_PLAYER_CARRY_e) */
        a = mpAttnActorA.get();
    }
    s16 angle = fopAcM_searchActorAngleY(this, a);
    shape_angle.y = angle;
    mProcVar2 = angle;
    current.angle.y = angle;
    gabi::Local<cXyz> d;
    cXyz_mi(&mpAttnActorA.get()->current.pos, d, &m3748);
    gabi::store<u32>(gabi::ea(&m370C) + 8, gabi::load<u32>(d.a + 8)); /* m370C = .. - m3748 (word copy) */
    gabi::store<u32>(gabi::ea(&m370C) + 0, gabi::load<u32>(d.a + 0));
    gabi::store<u32>(gabi::ea(&m370C) + 4, gabi::load<u32>(d.a + 4));
    return TRUE;
}
VERIFY(0x023ED0F0, &daPy_lk_c::procGrabReady_init);

/* 023ED2B0 (unnamed by the matcher; HD-only) mAtCps[0..2].SetAtSpl(spl) */
void daPy_lk_c::setAtCpsSpl(u8 spl) {
    WWHD_FUNC(0x023ED2B0, void, this, spl);
    u32 cps = gabi::ea(this) + 0x7B1C;
    for (int i = 0; i < 3; i++, cps += 0x138) {
        gabi::store<u8>(cps + 0x6F, spl);
    }
}
VERIFY(0x023ED2B0, &daPy_lk_c::setAtCpsSpl);

/* 023ED2CC */
void daPy_lk_c::setEnemyWeaponAtParam(BOOL r4) {
    WWHD_FUNC(0x023ED2CC, void, this, r4);
    u32 boko = gabi::ea(mActorKeepEquip.mActor.get());
    u32 type = gabi::load<u32>(boko + 0xB0); /* fopAcM_GetParam(boko) */
    u8 cutType = gabi::load<u8>(0x100356F0 + type); /* cut_type[] */
    /* the daBoko_c getters read static per-type tables (at type 0x10192180, atp 0x10192138, se 0x10192118, cps R 0x10192120) */
    u32 atType = gabi::load<u32>(0x10192180 + type * 4);
    u32 atp = gabi::load<u32>(0x10192138 + type * 4);
    u8 se = gabi::load<u8>(0x10192118 + type);
    f32 r = gabi::load<f32>(0x10192120 + type * 4);
    if (r4) {
        setAtParam(atType, atp, 1 /* dCcG_At_Spl_UNK1 */, se, 0xF /* dCcG_AtHitMark_Big_e */, (u8)(cutType + 1), r);
    } else {
        setAtParam(atType, atp, 0 /* dCcG_At_Spl_UNK0 */, se, 0xD /* dCcG_AtHitMark_Nrm_e */, cutType, r);
    }
}
VERIFY(0x023ED2CC, &daPy_lk_c::setEnemyWeaponAtParam);

/* 023ED380 */
void daPy_lk_c::setJumpCutAtParam() {
    WWHD_FUNC(0x023ED380, void, this);
    gabi::store<u8>(gabi::ea(this) + 0x3AD, 0); /* mCutCount */
    u16 e = mEquipItem;
    if (e == 0x103 /* daPyItem_SWORD_e */) {
        /* HD: HIO folded */
        if (LK_checkNormalSwordEquip()) {
            m35FC = 1.7f;
            setAtParam(2 /* AT_TYPE_SWORD */, 2, 1 /* dCcG_At_Spl_UNK1 */, 1 /* dCcG_SE_UNK1 */, 0xF /* dCcG_AtHitMark_Big_e */, 0xA /* CUT_TYPE_JUMPCUT_SWORD */, 20.0f);
        } else {
            m35FC = 2.0f;
            setAtParam(2, 4, 1, 1, 0xF, 0xA, 30.0f);
        }
        /* HD: a strong (spin) jump cut (|m3578| > 0xF800) also gets the HD flag at 0x69E8 and spl 0 */
        s32 v = m3578;
        s32 av = (s32)(v < 0 ? 0u - (u32)v : (u32)v); /* abs (INT_MIN stays negative) */
        if (av > 0xF800 && gabi::load<u16>(0x101CEF16) != 1 /* !checkCurse() */) {
            u16 e2 = mEquipItem;
            if (e2 == 0x103 || e2 == 0x101 /* daPyItem_BOKO_e */) {
                mHD69E8 = 1;
                setAtCpsSpl(0);
            }
        }
    } else if (e == 0x33 /* dItemNo_SKULL_HAMMER_e */) {
        setAtParam(0x10000 /* AT_TYPE_SKULL_HAMMER */, 4, 1 /* dCcG_At_Spl_UNK1 */, 5 /* dCcG_SE_METAL */, 0xD /* dCcG_AtHitMark_Nrm_e */, 0x13 /* CUT_TYPE_JUMPCUT_HAMMER */, 50.0f);
    } else if (mActorKeepEquip.mActor.get() != nullptr) {
        setEnemyWeaponAtParam(TRUE);
    } else {
        setAtParam(0x400 /* AT_TYPE_MACHETE */, 2, 1, 1, 0xF, 0xE /* CUT_TYPE_JUMPCUT_MACHETE */, 30.0f);
    }
}
VERIFY(0x023ED380, &daPy_lk_c::setJumpCutAtParam);

/* 023ED5A4 */
BOOL daPy_lk_c::procJumpCut_init(int param_0) {
    WWHD_FUNC(0x023ED5A4, BOOL, this, param_0);
    fopAc_ac_c* equip_actor = mActorKeepEquip.mActor.get();
    if (gabi::load<u16>(0x101CEF16) == 1 /* daPy_dmEcallBack_c::checkCurse() */ ||
        (mEquipItem == 0x101 /* daPyItem_BOKO_e */ && equip_actor == nullptr)) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x5B /* daPyProc_JUMP_CUT_e */);
    /* HD: HIO folded */
    if (param_0 != 0) {
        gabi::call(LK_setSingleMoveAnime, this, 0x2B /* ANM_JATTACK */, 0.8f, 1.0f, 0xF, 5.0f);
        speed.y = 18.0f;
        gravity = -3.0f;
        mNormalSpeed = 9.0f;
    } else {
        gabi::call(LK_setSingleMoveAnime, this, 0x2B, 0.74f, 2.0f, 0xF, 1.0f);
        mNormalSpeed = 18.0f;
        speed.y = 27.0f;
        gravity = -3.0f;
    }
    mProcVar6 = 0;
    mHD69E8 = 0;
    current.angle.y = shape_angle.y;
    LK_voiceStart(1);
    lk_onPlayerStatus0(0x8000 /* daPyStts0_SWORD_SWING_e */);
    if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
        setBlurPosResource(0x28B /* dRes_INDEX_LKANM__WEAPONJUMP_POS_e */);
    } else {
        setBlurPosResource(0x283 /* dRes_INDEX_LKANM__CUTJUMP_POS_e */);
    }
    setJumpCutAtParam();
    return TRUE;
}
VERIFY(0x023ED5A4, &daPy_lk_c::procJumpCut_init);

/* 023ED788 */
BOOL daPy_lk_c::procWHideReady_init(u32 param_0, cXyz* param_1) {
    WWHD_FUNC(0x023ED788, BOOL, this, param_0, param_1);
    BOOL uVar1 = (mModeFlg >> 4) & 1; /* checkModeFlg(ModeFlg_WHIDE) */
    mNoResetFlg0 = mNoResetFlg0 & ~0x10000u; /* offNoResetFlg0(daPyFlg0_UNK10000) */
    gabi::call(LK_commonProcInit, this, 0x13 /* daPyProc_WHIDE_READY_e */);
    mNormalSpeed = 0.0f;
    current.angle.y = shape_angle.y;
    u32 p1 = gabi::ea(param_1);
    gabi::store<u32>(gabi::ea(&m370C) + 0, gabi::load<u32>(p1 + 0)); /* m370C = *param_1 (word copy) */
    gabi::store<u32>(gabi::ea(&m370C) + 4, gabi::load<u32>(p1 + 4));
    gabi::store<u32>(gabi::ea(&m370C) + 8, gabi::load<u32>(p1 + 8));
    /* HD: HIO folded (m_HIO->mWall.m.field_0x50 = 8.5) */
    if (param_0 != 0) {
        mProcVar2 = cM_atan2s(gabi::load<f32>(param_0 + 0), gabi::load<f32>(param_0 + 8));
        m370C.x = gabi::fmadds(gabi::load<f32>(param_0 + 0), 8.5f, m370C.x);
        m370C.z = gabi::fmadds(gabi::load<f32>(param_0 + 8), 8.5f, m370C.z);
    } else {
        s16 a = m352C;
        mProcVar2 = a;
        m370C.x = gabi::fmadds(cM_ssin(a), 8.5f, m370C.x);
        m370C.z = gabi::fmadds(cM_scos(a), 8.5f, m370C.z);
    }
    if (mEquipItem == 0x100 /* daPyItem_NONE_e */) {
        if (gabi::call<BOOL>(LK_checkEquipAnime, this)) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        }
        gabi::call(LK_setSingleMoveAnime, this, 0x42 /* ANM_WALL */, 0.0f, 3.0f, -1, 5.0f);
        mProcVar6 = 1;
    } else {
        gabi::call(LK_setBlendMoveAnime, this, 2.4f);
        setAnimeUnequip();
        mProcVar6 = 0;
    }
    if (uVar1 != 0) {
        lk_onPlayerStatus0(1 /* daPyStts0_UNK1_e */);
    }
    return TRUE;
}
VERIFY(0x023ED788, &daPy_lk_c::procWHideReady_init);

/* 023ED970 */
BOOL daPy_lk_c::procCutTurnCharge_init() {
    WWHD_FUNC(0x023ED970, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x58 /* daPyProc_CUT_TURN_CHARGE_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x18 /* ANM_CUTTURNP */, 0.8f, 1.0f, 9, 3.0f); /* HD: HIO folded */
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    return TRUE;
}
VERIFY(0x023ED970, &daPy_lk_c::procCutTurnCharge_init);

/* 023ED9E0 */
BOOL daPy_lk_c::procWeaponNormalSwing_init() {
    WWHD_FUNC(0x023ED9E0, BOOL, this);
    if (mActorKeepEquip.mActor.get() == nullptr) {
        return FALSE;
    }
    gabi::call(LK_commonProcInit, this, 0x4B /* daPyProc_WEAPON_NORMAL_SWING_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x26 /* ANM_CUTBOKO */, 1.1f, 1.0f, 0x22, 3.0f); /* HD: HIO folded */
    current.angle.y = shape_angle.y;
    LK_cXyzZero_copy(&m3700);
    m34C2 = 1;
    LK_voiceStart(1);
    if (lk_checkAttentionLock() || !(mStickDistance > 0.05f)) {
        mProcVar2 = shape_angle.y;
    } else {
        mProcVar2 = m34E8;
    }
    lk_onPlayerStatus0(0x8000 /* daPyStts0_SWORD_SWING_e */);
    m351E = m34DC;
    setBlurPosResource(0x27E /* dRes_INDEX_LKANM__CUTBOKO_POS_e */);
    setEnemyWeaponAtParam(FALSE);
    mProcVar0 = 0; /* HD: HIO folded */
    return TRUE;
}
VERIFY(0x023ED9E0, &daPy_lk_c::procWeaponNormalSwing_init);

/* 023EDB6C */
BOOL daPy_lk_c::procWeaponSideSwing_init() {
    WWHD_FUNC(0x023EDB6C, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x4C /* daPyProc_WEAPON_SIDE_SWING_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xAA /* ANM_HAMSWINGA */, 0.9f, 20.0f, 0x2A, 6.0f); /* HD: HIO folded */
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    LK_voiceStart(1);
    mProcVar2 = shape_angle.y;
    setBlurPosResource(0x289 /* dRes_INDEX_LKANM__HAMMERSIDE_POS_e */);
    setEnemyWeaponAtParam(FALSE);
    return TRUE;
}
VERIFY(0x023EDB6C, &daPy_lk_c::procWeaponSideSwing_init);

/* 023EDC14 */
BOOL daPy_lk_c::procWeaponFrontSwingReady_init() {
    WWHD_FUNC(0x023EDC14, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x4D /* daPyProc_WEAPON_FRONT_SWING_READY_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0xAB /* ANM_HAMSWINGBPRE */, 0.7f, 3.0f, 0xD, 3.0f); /* HD: HIO folded */
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    if (lk_checkAttentionLock() || !(mStickDistance > 0.05f)) {
        mProcVar2 = shape_angle.y;
    } else {
        mProcVar2 = m34E8;
    }
    setEnemyWeaponAtParam(FALSE);
    LK_voiceStart(7);
    return TRUE;
}
VERIFY(0x023EDC14, &daPy_lk_c::procWeaponFrontSwingReady_init);

/* 023EDD14 */
BOOL daPy_lk_c::procWeaponThrow_init() {
    WWHD_FUNC(0x023EDD14, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x50 /* daPyProc_WEAPON_THROW_e */);
    gabi::call(LK_setSingleMoveAnime, this, 0x1E /* ANM_CUTA */, 1.0f, 0.0f, -1, 5.0f);
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    m34C2 = 1;
    return TRUE;
}
VERIFY(0x023EDD14, &daPy_lk_c::procWeaponThrow_init);

/* 023EDD9C */
BOOL daPy_lk_c::procBottleSwing_init(int param_0) {
    WWHD_FUNC(0x023EDD9C, BOOL, this, param_0);
    int iVar2 = checkShipRideUseItem(1);
    gabi::call(LK_commonProcInit, this, 0xA5 /* daPyProc_BOTTLE_SWING_e */);
    if (param_0 != 0) {
        gabi::call(LK_deleteEquipItem, this, 0);
        gabi::call(LK_setBottleModel, this, 0x50 /* dItemNo_EMPTY_BOTTLE_e */);
    }
    current.angle.y = shape_angle.y;
    mNormalSpeed = 0.0f;
    u32 play = dComIfGp_ea();
    /* dComIfGp_att_getCatghTarget(): dAttLook_c::convPId(play + 0x5934, play + 0x5944) */
    u32 catch_target = gabi::call<u32>(0x024EBD10, play + 0x5934, gabi::load<u32>(play + 0x5804 + 0x140));
    BOOL bVar1;
    if (catch_target != 0) {
        u32 mtx = lk_getAnmMtx(mpCLModel, 0 /* CL_JNT_LINK_ROOT_e */);
        bVar1 = !(gabi::load<f32>(catch_target + 0x318) > gabi::load<f32>(mtx + 0x1C));
    } else {
        bVar1 = mWaterY > current.pos.y + 10.0f;
    }
    /* HD: both swings use the same (folded HIO) parameters */
    gabi::call(LK_setSingleMoveAnime, this, bVar1 ? 0xBD /* ANM_BINSWINGU */ : 0xBC /* ANM_BINSWINGS */, 1.2f, 0.0f, 0x10, 0.0f);
    m35A0 = 15.0f;
    mProcVar6 = 0;
    mProcVar2 = 0;
    mProcVar7 = 0;
    LK_voiceStart(7);
    initShipRideUseItem(iVar2, 2);
    return TRUE;
}
VERIFY(0x023EDD9C, &daPy_lk_c::procBottleSwing_init);

/* 023E47D4 */
BOOL daPy_lk_c::procVomitJump_init(int param_0) {
    WWHD_FUNC(0x023E47D4, BOOL, this, param_0);
    gabi::call(LK_commonProcInit, this, 0x98 /* daPyProc_VOMIT_JUMP_e */);
    m35C4 = 40.0f;
    f32 fVar1 = (param_0 == 2 || param_0 == 3) ? 3.0f : -1.0f;
    mProcVar6 = param_0;
    gabi::call(LK_setSingleMoveAnime, this, 0xA7 /* ANM_VOMITJMP */, 1.0f, 0.0f, -1, fVar1);
    lk_onPlayerStatus0(0x80000000u /* daPyStts0_UNK80000000_e */);
    u32 stts = gabi::ea(&mStts); /* mStts.ClrCcMove() */
    gabi::store<f32>(stts + 0, 0.0f);
    gabi::store<f32>(stts + 4, 0.0f);
    gabi::store<f32>(stts + 8, 0.0f);
    mProcVar0 = 2;
    mProcVar2 = 0;
    u32 mtx = lk_getAnmMtx(mpCLModel, 0x12 /* CL_JNT_CHIN_JNT_e */);
    daPy_mtxPosFollowEcallBack_makeEmitterColor(gabi::ea(this) + 0x67BC /* m33A8 */, 0x8112 /* ID_IT_SN_LK_BLUR00 */, mtx,
                                                gabi::ea(&current.pos), gabi::ea(&shape_angle), gabi::ea(this) + 0x1A8 /* &tevStr.mColorK0 */,
                                                gabi::ea(this) + 0x1A8);
    if (param_0 == 0) {
        gabi::Local<cXyz> v;
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
        dComIfGp_getVibration_StartShock(4, -0x21, v);
    }
    if (param_0 == 1) {
        gravity = 0.0f;
        mProcVar1 = 20;
    } else {
        LK_voiceStart(0);
        if (param_0 == 3) {
            mProcVar6 = 1;
            speed.y = 46.0f;
        } else {
            speed.y = 64.5f; /* HD: HIO folded */
        }
        mProcVar1 = 0;
    }
    return TRUE;
}
VERIFY(0x023E47D4, &daPy_lk_c::procVomitJump_init);

/* 023ECA94 */
BOOL daPy_lk_c::procShipReady_init() {
    WWHD_FUNC(0x023ECA94, BOOL, this);
    gabi::call(LK_commonProcInit, this, 0x86 /* daPyProc_SHIP_READY_e */);
    gabi::call(LK_deleteEquipItem, this, 1);
    fopAc_ac_c* ship = gabi::at<fopAc_ac_c>(lk_getShipActor());
    gabi::call(LK_setSingleMoveAnime, this, 0x4F /* ANM_VJMPCL */, 0.75f, 0.0f, 0x18, 5.0f); /* HD: HIO folded */
    gravity = 0.0f;
    mNormalSpeed = 0.0f;
    s16 sVar3 = fopAcM_searchActorAngleY(ship, this);
    u32 model = gabi::load<u32>(gabi::load<u32>(gabi::ea(ship) + 0x3B4) + 0x90); /* ship->getBodyMtx() */
    s16 a;
    if ((s16)(sVar3 - ship->shape_angle.y) > 0) {
        PSMTXMultVec(gabi::at<Mtx34>(model ? model + 0xC8 : 0), gabi::at<cXyz>(0x10034FB4) /* l_ship_ledge */, &current.pos);
        a = (s16)(ship->shape_angle.y - 0x4000);
        shape_angle.y = a;
        mProcVar2 = a;
        mProcVar0 = 0;
    } else {
        PSMTXMultVec(gabi::at<Mtx34>(model ? model + 0xC8 : 0), gabi::at<cXyz>(0x10034FA8) /* l_ship_redge */, &current.pos);
        a = (s16)(ship->shape_angle.y + 0x4000);
        shape_angle.y = a;
        mProcVar2 = (s16)(a + 0x8000);
        mProcVar0 = 1;
    }
    current.angle.y = a;
    mProcVar6 = 0;
    gabi::store<u8>(gabi::ea(ship) + 0x636, 3); /* ship->setReadyFirst() */
    lk_onPlayerStatus0(0x1000000 /* daPyStts0_UNK1000000_e */);
    gabi::call(0x023DFAAC /* daPy_lk_c::swimOutAfter */, this, 1);
    mNoResetFlg0 = mNoResetFlg0 & ~0x200u; /* offNoResetFlg0(daPyFlg0_SHIP_DROP) */
    m3540 = 0;
    m3542 = 0;
    m353C = 0;
    m353E = 0;
    return TRUE;
}
VERIFY(0x023ECA94, &daPy_lk_c::procShipReady_init);

/* orderTalk: the talk test of one item button (entry, actor, trigger bit, selected item, order function);
 * returns 1 (ordered), 0 (refused) or -1 (not this button) */
static inline int lk_orderTalkBtn(daPy_lk_c* t, u32 entry, u32 actor, u8 trig, u32 itemOff, u32 orderFn) {
    if (entry == 0) {
        return -1;
    }
    s32 type = gabi::load<s32>(entry + 8);
    if (!(type == 3 /* fopAc_Attn_TYPE_SPEAK_e */ ||
          (type == 1 /* fopAc_Attn_TYPE_TALK_e */ && gabi::ea(t->mpAttnActorLockOn.get()) == gabi::load<u32>(actor)))) {
        return -1;
    }
    if (gabi::load<u32>(gabi::ea(t) + 0x8260) != 0 || !(t->mItemTrigger & trig)) { /* HD word 0x8260 blocks it */
        return -1;
    }
    if (gabi::call<BOOL>(0x0255101C /* isEsa */, (u32)gabi::load<u8>(dComIfGp_ea() + itemOff)) &&
        gabi::load<u8>(dComIfGp_ea() + itemOff) != 0x83 /* dItemNo_HYOI_PEAR_e */ && (s32)t->m3630 != -1 /* fpcM_ERROR_PROCESS_ID_e */) {
        lk_seStartSystem(0x883 /* JA_SE_ITEM_TARGET_OUT */);
        return 0;
    }
    gabi::call(orderFn, t, gabi::load<u32>(actor));
    return 1;
}

/* 023EC724 */
int daPy_lk_c::orderTalk() {
    WWHD_FUNC(0x023EC724, int, this);
    if (lk_getDoStatus() == 2 /* dActStts_SPEAK_e */ || lk_getDoStatus() == 1 /* dActStts_LOOK_e */ ||
        lk_getDoStatus() == 0x26 /* dActStts_READ_e */ || lk_getDoStatus() == 0xA /* dActStts_CHECK_e */) {
        if (mHD8260 == 0 && LK_doTrigger() /* talkTrigger() */) {
            gabi::call(0x025D744C /* fopAcM_orderTalkEvent */, this, mpAttnActorA.get());
            return TRUE;
        }
    }
    int r = lk_orderTalkBtn(this, mpAttnEntryX, gabi::ea(&mpAttnActorX), 4, 0x5BBB, 0x025D74B0 /* fopAcM_orderTalkXBtnEvent */);
    if (r >= 0) return r;
    r = lk_orderTalkBtn(this, mpAttnEntryY, gabi::ea(&mpAttnActorY), 8, 0x5BBC, 0x025D7514 /* fopAcM_orderTalkYBtnEvent */);
    if (r >= 0) return r;
    r = lk_orderTalkBtn(this, mpAttnEntryZ, gabi::ea(&mpAttnActorZ), 0x10, 0x5BBD, 0x025D7578 /* fopAcM_orderTalkZBtnEvent */);
    if (r >= 0) return r;
    /* HD: the fourth item button */
    r = lk_orderTalkBtn(this, mpAttnEntryHD, gabi::ea(&mpAttnActorHD), 0x80, 0x5BBE, 0x025D75DC);
    if (r >= 0) return r;
    return FALSE;
}
VERIFY(0x023EC724, &daPy_lk_c::orderTalk);

/* J3DAnmTransform (HD: vtable at +4): getFrameMax (slot 0x14, an int) and getAttribute (slot 0xC); the frame at +0 */
static inline s32 lk_anm_getFrameMax(u32 anm) { return gabi::call_ptr<s32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0x14), anm); }
static inline u32 lk_anm_getAttribute(u32 anm) { return gabi::call_ptr<u32>(gabi::load<u32>(gabi::load<u32>(anm + 4) + 0xC), anm); }

/* 023E0E50 */
int daPy_lk_c::setMoveAnime(f32 f27, f32 f28, f32 f25, int r27, int r28, int r29, f32 i_morf) {
    WWHD_FUNC(0x023E0E50, int, this, f27, f28, f25, r27, r28, r29, i_morf);
    u32 r3 = gabi::load<u32>(gabi::ea(this) + 0x57FC); /* mAnmRatioUnder[UNDER_MOVE0_e].getAnmTransform() */
    J3DFrameCtrl* frameCtrl0 = &mFrameCtrlUnder[0];
    J3DFrameCtrl* frameCtrl1 = &mFrameCtrlUnder[1];
    f32 f31;
    u8 prev = m34C3;
    if (prev == 0 || prev == 9 || prev == 10) {
        f31 = 0.0f;
    } else {
        s32 fm = lk_anm_getFrameMax(r3);
        f31 = gabi::load<f32>(r3) / (f32)fm; /* r3->getFrame() / r3->getFrameMax() */
    }
    u32 r25 = gabi::call<u32>(LK_getAnmData, this, r27);
    u32 r24 = gabi::call<u32>(LK_getAnmData, this, r28);
    gabi::Local<be<u32>> sp14, sp18, sp10, sp1C; /* under/upper of the two animations */
    gabi::call(LK_getUnderUpperAnime, this, r25, sp14.a, sp18.a, 0, 0x2400);
    gabi::call(LK_getUnderUpperAnime, this, r24, sp10.a, sp1C.a, 1, 0x2400);
    f32 inv = 1.0f - f27;
    lk_anmRatio_clear(gabi::ea(this) + 0x57F8, inv); /* mAnmRatioUnder[UNDER_MOVE0_e].setRatio(1.0f - f27) */
    lk_anmRatio_clear(gabi::ea(this) + 0x5808, f27);  /* mAnmRatioUnder[UNDER_MOVE1_e].setRatio(f27) */
    lk_anmRatio_clear(gabi::ea(this) + 0x5818, inv);  /* mAnmRatioUpper[UPPER_MOVE0_e] */
    lk_anmRatio_clear(gabi::ea(this) + 0x5828, f27);  /* mAnmRatioUpper[UPPER_MOVE1_e] */
    f32 f3 = (f32)lk_anm_getFrameMax(*sp14);
    s32 fmB = lk_anm_getFrameMax(*sp10);
    f32 f30 = 1.0f / f3;
    f32 f26 = (f32)fmB;
    f32 rate = gabi::fmadds((f25 * f3) / f26 - f28, f27, f28); /* f28 + f27 * ((f25 * f3) / f26 - f28) */
    u32 attr = lk_anm_getAttribute(*sp14);
    gabi::call(LK_setFrameCtrl, this, frameCtrl0, attr, 0, (s32)(s16)gabi::ftoi(f3), rate, f31 * f3);
    fcpy_l(*sp14, gabi::ea(frameCtrl0) + 4); /* sp14->setFrame(frameCtrl0.getFrame()) */
    attr = lk_anm_getAttribute(*sp10);
    gabi::call(LK_setFrameCtrl, this, frameCtrl1, attr, 0, (s32)(s16)gabi::ftoi(f26), (rate * f26) * f30, f31 * f26);
    fcpy_l(*sp10, gabi::ea(frameCtrl1) + 4);
    gabi::store<u32>(gabi::ea(this) + 0x57FC, *sp14); /* mAnmRatioUnder[0].setAnmTransform(sp14) */
    gabi::store<u32>(gabi::ea(this) + 0x580C, *sp10); /* mAnmRatioUnder[1].setAnmTransform(sp0C) */
    if (*sp18 != 0) {
        f32 f2 = (f32)lk_anm_getFrameMax(*sp18);
        gabi::store<u32>(gabi::ea(this) + 0x581C, *sp18); /* mAnmRatioUpper[0].setAnmTransform(sp10) */
        attr = lk_anm_getAttribute(*sp18);
        gabi::call(LK_setFrameCtrl, this, &mFrameCtrlUpper[0], attr, 0, (s32)(s16)gabi::ftoi(f2), (rate * f2) * f30, f31 * f2);
        fcpy_l(*sp18, gabi::ea(&mFrameCtrlUpper[0]) + 4);
    } else {
        gabi::store<u32>(gabi::ea(this) + 0x581C, *sp14);
    }
    if (*sp1C != 0) {
        f32 f2 = (f32)lk_anm_getFrameMax(*sp1C);
        gabi::store<u32>(gabi::ea(this) + 0x582C, *sp1C); /* mAnmRatioUpper[1].setAnmTransform(sp08) */
        attr = lk_anm_getAttribute(*sp1C);
        gabi::call(LK_setFrameCtrl, this, &mFrameCtrlUpper[1], attr, 0, (s32)(s16)gabi::ftoi(f2), (rate * f2) * f30, f31 * f2);
        fcpy_l(*sp1C, gabi::ea(&mFrameCtrlUpper[1]) + 4);
    } else {
        gabi::store<u32>(gabi::ea(this) + 0x582C, *sp10);
    }
    if (!(i_morf < 0.0f)) { /* HD: NaN morphs too */
        gabi::call(0x025E3F3C /* mDoExt_MtxCalcOldFrame::initOldFrameMorf */, (u32)m_old_fdata, i_morf, 0, 0x2A);
    }
    /* mAnmDataTable (0x100366A0, 8 bytes each): mTexAnmIdx at +6 */
    if (mDirection == 1 /* DIR_BACKWARD */ || (mCurProc == 0x78 /* daPyProc_ROPE_SWING_e */ && !(mModeFlg & 0x400))) {
        gabi::call(LK_setTextureAnime, this, (u32)gabi::load<u16>(0x100366A0 + r28 * 8 + 6), 0);
    } else {
        gabi::call(LK_setTextureAnime, this, (u32)gabi::load<u16>(0x100366A0 + r27 * 8 + 6), 0);
    }
    if (r29 == 5 || r29 == 2) {
        gabi::call(LK_setSeAnime, this, gabi::load<u32>(gabi::ea(this) + 0x57FC), gabi::ea(this) + 0x5848 /* &m_anm_heap_under[0] */, frameCtrl0);
    } else {
        gabi::call(LK_setSeAnime, this, gabi::load<u32>(gabi::ea(this) + 0x580C), gabi::ea(this) + 0x5858 /* &m_anm_heap_under[1] */, frameCtrl1);
    }
    m34C3 = (u8)r29;
    return TRUE;
}
VERIFY(0x023E0E50, &daPy_lk_c::setMoveAnime);

/* v.abs2XZ() through a stack cXyz {x, 0, z} and PSVECSquareMag */
static inline f32 lk_abs2XZ(cXyz* v) {
    gabi::Local<cXyz> xz;
    fcpy_l(xz.a + 0, gabi::ea(&v->x));
    xz->y = 0.0f;
    fcpy_l(xz.a + 8, gabi::ea(&v->z));
    return PSVECSquareMag(xz);
}

/* 023E14A0 */
void daPy_lk_c::setBlendMoveAnime(f32 param_1) {
    WWHD_FUNC(0x023E14A0, void, this, param_1);
    f32 f1_1 = m3580 == 8 ? 1.0f : cM_scos(m34E2);
    f32 f30 = std::fabs(gabi::fmuls_ppc(f1_1, mNormalSpeed)) / mMaxNormalSpeed;
    J3DFrameCtrl* frameCtrl = &mFrameCtrlUnder[1]; /* UNDER_MOVE1_e */
    /* HD: HIO folded */
    f32 f29, f28;
    if (gabi::load<u8>(dComIfGp_ea() + 0x5292) /* dComIfGp_event_runCheck() */ || gabi::load<u16>(gabi::ea(this) + 0x420) != 0 /* checkPlayerDemoMode() */) {
        f29 = 1.2f;
        f28 = 0.87f;
    } else if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        f29 = 0.85f;
        f28 = 0.7f;
    } else {
        f29 = 0.8f;
        f28 = 1.0f;
    }
    int r28 = 0; /* ANM_WAITS */
    int r27, r26;
    if (LK_FIELD(f32, 0x3CC) < 0.0f) { /* checkGrabWear() */
        r27 = 0x95; /* ANM_WALKBARREL */
        r26 = 0x95;
    } else if (gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
        if ((lk_abs2XZ(&m373C) > 25.0f && cLib_distanceAngleS(cM_atan2s(m373C.x, m373C.z), shape_angle.y) >= 0x4000) ||
            ((noResetFlg1() & 0x10000000) && m3644 > 5.0f && cLib_distanceAngleS(m3640, shape_angle.y) >= 0x4000)) {
            r27 = 5; /* ANM_WALKHBOOTSKAZE */
            r26 = 5;
            if (!(noResetFlg1() & 0x1000000)) { /* daPyFlg1_UNK1000000 */
                param_1 = 2.4f;
                setNoResetFlg1(noResetFlg1() | 0x1000000);
            }
        } else {
            r27 = 4; /* ANM_WALKHBOOTS */
            r26 = 4;
            if (noResetFlg1() & 0x1000000) {
                param_1 = 2.4f;
                setNoResetFlg1(noResetFlg1() & ~0x1000000u);
            }
        }
    } else if (m3580 != 8 && m34E2 <= -0x11C7) {
        r27 = 6; /* ANM_WALKSLOPE */
        r26 = 6;
        if (!(noResetFlg1() & 0x80)) { /* daPyFlg1_UNK80 */
            param_1 = 2.4f;
            setNoResetFlg1(noResetFlg1() | 0x80);
        }
    } else {
        r27 = 1; /* ANM_WALK */
        if ((lk_abs2XZ(&m3730) > 25.0f && cLib_distanceAngleS(cM_atan2s(m3730.x, m3730.z), shape_angle.y) >= 0x4000) ||
            ((noResetFlg1() & 0x10000000) && m3644 > 5.0f && cLib_distanceAngleS(m3640, shape_angle.y) >= 0x4000)) {
            r26 = 3; /* ANM_DASHKAZE */
            if (!(noResetFlg1() & 0x1000000)) {
                param_1 = 2.4f;
                setNoResetFlg1(noResetFlg1() | 0x1000000);
            }
        } else {
            r26 = 2; /* ANM_DASH */
            if (noResetFlg1() & 0x1000000) {
                param_1 = 2.4f;
                setNoResetFlg1(noResetFlg1() & ~0x1000000u);
            }
        }
        if (noResetFlg1() & 0x80) {
            param_1 = 2.4f;
            setNoResetFlg1(noResetFlg1() & ~0x80u);
        }
    }
    BOOL r25 = FALSE;
    f32 f31 = lk_abs2XZ(&m36A0);
    f32 f25 = lk_abs2XZ(&m3730);
    f32 f1_2 = lk_abs2XZ(&m36B8);
    BOOL bVar3;
    if (f31 > f25) {
        f1_2 = fsel_l(f31 - f1_2, f31, f1_2);
        bVar3 = TRUE;
    } else {
        if (f25 > f1_2) {
            f1_2 = f25;
        }
        bVar3 = FALSE;
    }
    /* HD: the NaN-safe forms of the original's branches (NaN slips) */
    BOOL slip = FALSE;
    if (mStickDistance < 0.05f) {
        if (!(f1_2 < 25.0f)) {
            slip = TRUE;
        } else if (!(f1_2 < 0.09f) && (m34C3 == 9 || m34C3 == 10)) {
            slip = TRUE;
        }
    }
    if (slip) {
        gabi::call(LK_seStartMapInfo, this, 0x205A /* JA_SE_LK_SLIP_SUS */);
        u8 c = m34C3;
        if ((c != 9 && bVar3) || (c != 10 && !bVar3)) {
            if (bVar3) {
                gabi::call(LK_setSingleMoveAnime, this, 0xA9 /* ANM_SLIPICE */, 0.9f, 0.0f, 0x1D, 2.0f);
                m34C3 = 9;
            } else {
                gabi::call(LK_setSingleMoveAnime, this, 0x9D /* ANM_WAITQ */, 1.3f, 0.0f, -1, 2.4f);
                gabi::call(LK_setTextureAnime, this, 0x68, 0);
                m34C3 = 10;
            }
            LK_voiceStart(35);
        }
        if (LK_checkGrabAnime()) {
            gabi::call(LK_resetActAnimeUpper, this, 2 /* UPPER_MOVE2_e */, -1.0f);
        }
        gabi::call(LK_freeGrabItem, this);
        if (mEquipItem == 0x101 /* daPyItem_BOKO_e */) {
            gabi::call(LK_deleteEquipItem, this, 0);
        }
        m3598 = 0.0f;
    } else {
        if (m34C3 == 9) {
            param_1 = 2.4f;
        }
        if (f30 < 0.5f || gabi::call<BOOL>(LK_checkHeavyStateOn, this)) {
            BOOL heavy = gabi::call<BOOL>(LK_checkHeavyStateOn, this);
            f32 f25_2 = f30 + f30; /* f30 / 0.5f */
            if (heavy && f25_2 > 0.55f) {
                r25 = TRUE;
            }
            r28 = 0; /* ANM_WAITS */
            if (mModeFlg & 1) { /* checkModeFlg(ModeFlg_00000001) */
                if (lk_checkAttentionLock() && (LK_upperIdx() == 0x16 || LK_upperIdx() == 0x1B) /* checkUpperGuardAnime() */) {
                    gabi::call(LK_setMoveAnime, this, 0.0f, 1.25f, 1.0f, 8 /* ANM_ATNRS */, 0xA /* ANM_ATNWRS */, 2, 2.4f);
                    return;
                }
                m3598 = 0.0f;
                if ((noResetFlg1() & 0x100) /* daPyFlg1_CONFUSE */ && gabi::load<u16>(gabi::ea(this) + 0x420) == 0 &&
                    !gabi::load<u8>(dComIfGp_ea() + 0x5292)) {
                    if (LK_underIdx() != 0x125 /* dRes_INDEX_LKANM_BCK_WAITQ_e */) {
                        gabi::call(LK_setSingleMoveAnime, this, 0x9D /* ANM_WAITQ */, 1.3f, 0.0f, -1, 2.4f);
                        m34C3 = 2;
                    }
                    return;
                }
                if (shape_angle.y != m34DE && !lk_checkAttentionLock()) {
                    s16 r3 = (s16)(shape_angle.y - m34DE);
                    r27 = r3 > 0 ? 9 /* ANM_ATNWLS */ : 0xA /* ANM_ATNWRS */;
                    s32 ar = r3 < 0 ? -r3 : r3;
                    f25_2 = gabi::fmadds((f32)ar, 0.001f, 0.5f);
                    if (f25_2 > 1.0f) {
                        f25_2 = 1.0f;
                    }
                    if (!(noResetFlg1() & 0x800000)) { /* daPyFlg1_UNK800000 */
                        param_1 = 2.4f;
                    }
                    setNoResetFlg1(noResetFlg1() | 0x800000);
                    gabi::call(LK_setMoveAnime, this, f25_2, 1.1f, 1.0f, r28, r27, 2, param_1);
                } else {
                    f32 rate;
                    if (gabi::call<BOOL>(LK_checkRestHPAnime, this)) {
                        r28 = 0x1C; /* ANM_WAITB */
                        rate = 0.4f;
                    } else {
                        rate = 1.1f;
                    }
                    if (noResetFlg1() & 0x800000) {
                        param_1 = 2.4f;
                    }
                    setNoResetFlg1(noResetFlg1() & ~0x800000u);
                    gabi::call(LK_setMoveAnime, this, f25_2, rate, f29, r28, r27, 2, param_1);
                }
            } else {
                m3598 = gabi::fnmsubs(1.0f - f28, f25_2, 1.0f); /* 1.0f - (1.0f - f28) * f25_2 */
                gabi::call(LK_setMoveAnime, this, f25_2, 1.1f, f29, r28, r27, 1, param_1);
            }
            if (r28 == 0x1C && (!gabi::load<u8>(dComIfGp_ea() + 0x5292) || gabi::load<u16>(gabi::ea(this) + 0x420) != 3) &&
                mFrameCtrlUnder[0].checkPass(15.0f)) {
                if (gabi::load<u16>(gabi::load<u32>(0x101F84DC) + 0x22) <= 2) { /* dComIfGs_getLife() */
                    LK_voiceStart(21);
                } else {
                    LK_voiceStart(20);
                }
            }
        } else if (f30 < 1.0f) {
            f32 d = f30 - 0.5f;
            f32 f1 = d + d; /* (f30 - 0.5f) / 0.5f */
            gabi::call(LK_setMoveAnime, this, f1, f29, 2.3f, r27, r26, 1, param_1);
            m3598 = gabi::fmuls_ppc(f28, 1.0f - f1);
        } else {
            if (!(f31 < 169.0f)) { /* HD: NaN takes the fast branch */
                setResetFlg0(resetFlg0() | 0x40000); /* onResetFlg0(daPyRFlg0_UNK40000) */
                gabi::call(LK_setMoveAnime, this, 1.0f, 3.91f, 3.91f, r26, r26, 1, param_1); /* 1.7f * 2.3f */
            } else {
                gabi::call(LK_setMoveAnime, this, 1.0f, 2.3f, 2.3f, r26, r26, 1, param_1);
            }
            m3598 = 0.0f;
        }
    }
    if (!(f30 < 0.9f) || r25) {
        setResetFlg0(resetFlg0() | 0x10); /* onResetFlg0(daPyRFlg0_UNK10) */
    }
    if (!(mModeFlg & 1)) {
        if (frameCtrl->checkPass(4.0f)) {
            setResetFlg0(resetFlg0() | 0x400); /* daPyRFlg0_RIGHT_FOOT_ON_GROUND */
        } else if (frameCtrl->checkPass(20.0f)) {
            setResetFlg0(resetFlg0() | 0x800); /* daPyRFlg0_LEFT_FOOT_ON_GROUND */
        } else {
            gabi::call(LK_resetFootEffect, this);
        }
    }
    if (m34C3 != 9 && m34C3 != 10) {
        if (f30 > 0.8f) {
            gabi::call(LK_setHandModel, this, r26);
        } else if (f30 > 0.0f) {
            gabi::call(LK_setHandModel, this, r27);
        } else {
            gabi::call(LK_setHandModel, this, r28);
        }
    }
}
VERIFY(0x023E14A0, &daPy_lk_c::setBlendMoveAnime);

/* getWHideModePolygon: the side line check of one direction (s = +1 for the left side, -1 for the right) */
static inline BOOL lk_whideSideCheck(daPy_lk_c* t, u32 tri, u32 p2, f32 f31, BOOL left, s16 sVar7) {
    gabi::Local<cXyz> local_3c;
    gabi::Local<cXyz> local_48;
    f32 nz = gabi::load<f32>(tri + 8);
    f32 nx = gabi::load<f32>(tri + 0);
    f32 x0 = gabi::load<f32>(p2 + 0);
    f32 z0 = gabi::load<f32>(p2 + 8);
    f32 x, z;
    if (left) {
        x = gabi::fmadds(1.25f, nx, gabi::fmadds(f31, nz, x0));  /* p2.x + f31 * n.z + n.x * 1.25f */
        z = gabi::fmadds(1.25f, nz, gabi::fnmsubs(f31, nx, z0)); /* p2.z - f31 * n.x + n.z * 1.25f */
    } else {
        x = gabi::fmadds(1.25f, nx, gabi::fnmsubs(f31, nz, x0)); /* p2.x - f31 * n.z + n.x * 1.25f */
        z = gabi::fmadds(1.25f, nz, gabi::fmadds(f31, nx, z0));  /* p2.z + f31 * n.x + n.z * 1.25f */
    }
    fcpy_l(local_3c.a + 4, p2 + 4);
    local_3c->x = x;
    local_3c->z = z;
    fcpy_l(local_48.a + 4, p2 + 4);
    local_48->x = gabi::fnmsubs(2.5f, gabi::load<f32>(tri + 0), x); /* local_3c.x - 2.5f * n.x */
    local_48->z = gabi::fnmsubs(2.5f, gabi::load<f32>(tri + 8), z);
    u32 chk = gabi::ea(t) + 0x9D0; /* mLinkLinChk */
    gabi::call(LK_dBgS_LinChk_Set, chk, local_3c.a, local_48.a, t);
    if (!lk_LineCross(chk)) {
        return FALSE;
    }
    u32 tri2 = dBgS_GetTriPla_l(chk + 0x14);
    if (tri2 == 0) { /* HD */
        return FALSE;
    }
    return sVar7 == cM_atan2s(gabi::load<f32>(tri2 + 0), gabi::load<f32>(tri2 + 8));
}

/* 023E33AC */
u32 daPy_lk_c::getWHideModePolygon(cXyz* i_start, cXyz* i_end, cXyz* param_2, int direction) {
    WWHD_FUNC(0x023E33AC, u32, this, i_start, i_end, param_2, direction);
    u32 chk = gabi::ea(this) + 0x9D0; /* mLinkLinChk */
    if (i_start != nullptr) {
        gabi::call(LK_dBgS_LinChk_Set, chk, i_start, i_end, this);
        if (!lk_LineCross(chk)) {
            return 0;
        }
    }
    u32 triPla = dBgS_GetTriPla_l(chk + 0x14);
    if (triPla == 0) { /* HD */
        return 0;
    }
    s16 sVar7 = cM_atan2s(gabi::load<f32>(triPla + 0), gabi::load<f32>(triPla + 8));
    if (std::fabs(gabi::load<f32>(triPla + 4)) > 0.05f) {
        return 0;
    }
    if (dBgS_GetSpecialCode_l(chk + 0x14) == 3) {
        return 0;
    }
    u32 p2 = gabi::ea(param_2);
    fcpy_l(p2 + 0, chk + 0x30); /* *param_2 = mLinkLinChk.GetCross() */
    fcpy_l(p2 + 4, chk + 0x34);
    fcpy_l(p2 + 8, chk + 0x38);
    /* mGndChk.SetPos(local_3c) (HD: HIO folded, 8.5; the GameCube sin for z kept) */
    u32 gnd = gabi::ea(this) + 0xB14;
    f32 s = cM_ssin(sVar7);
    fcpy_l(gnd + 0x28, chk + 0x34);
    gabi::store<f32>(gnd + 0x24, gabi::fmadds(s, 8.5f, gabi::load<f32>(chk + 0x30)));
    gabi::store<f32>(gnd + 0x2C, gabi::fmadds(s, 8.5f, gabi::load<f32>(chk + 0x38)));
    f64 f31 = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), gnd);
    if (f31 != (f64)-1000000000.0f /* -G_CM3D_F_INF */) {
        u32 tri = dBgS_GetTriPla_l(gnd + 0x14);
        if (tri == 0 || gabi::load<f32>(tri + 4) < 0.5f) { /* !cBgW_CheckBGround() (HD: null check) */
            return 0;
        }
    }
    f32 dist = direction == 4 /* DIR_NONE */ ? 49.9f : 99.8f;
    if (direction != 3 /* DIR_RIGHT */) {
        if (!lk_whideSideCheck(this, triPla, p2, dist, TRUE, sVar7)) {
            return 0;
        }
    }
    if (direction != 2 /* DIR_LEFT */) {
        if (!lk_whideSideCheck(this, triPla, p2, dist, FALSE, sVar7)) {
            return 0;
        }
    }
    if (direction == 4 /* DIR_NONE */) {
        s16 d = (s16)(sVar7 - current.angle.y - 0x8000);
        s32 ad = d < 0 ? -d : d;
        return ad > 0x1555 ? 0 : triPla;
    }
    /* HD: HIO folded (m_HIO->mWall.m.field_0x54 = 25.0) */
    s16 d;
    if (direction == 2 /* DIR_LEFT */) {
        s16 sy = shape_angle.y;
        gabi::store<f32>(p2 + 0, gabi::fmadds(25.0f, gabi::load<f32>(triPla + 8), gabi::load<f32>(p2 + 0)));
        gabi::store<f32>(p2 + 8, gabi::fnmsubs(25.0f, gabi::load<f32>(triPla + 0), gabi::load<f32>(p2 + 8)));
        d = (s16)(sVar7 - sy);
    } else {
        s16 sy = shape_angle.y;
        gabi::store<f32>(p2 + 0, gabi::fnmsubs(25.0f, gabi::load<f32>(triPla + 8), gabi::load<f32>(p2 + 0)));
        gabi::store<f32>(p2 + 8, gabi::fmadds(25.0f, gabi::load<f32>(triPla + 0), gabi::load<f32>(p2 + 8)));
        d = (s16)(sy - sVar7);
    }
    if (d > 0x2AAA) {
        return 0;
    }
    return triPla;
}
VERIFY(0x023E33AC, &daPy_lk_c::getWHideModePolygon);

/* fnmsubs with a double first operand (an unrounded f1 result): b - a * c, rounded once */
static inline f32 lk_fnmsubs_d(f64 a, f32 c, f32 b) { return (f32)(-(a * (f64)c - (f64)b)); }
/* cBgS_PolyInfo assignment (poly/bg index, two words; not the vtable at +0xC) */
static inline void lk_polyInfo_copy(u32 dst, u32 src) {
    gabi::store<u16>(dst + 0, gabi::load<u16>(src + 0));
    gabi::store<u16>(dst + 2, gabi::load<u16>(src + 2));
    gabi::store<u32>(dst + 4, gabi::load<u32>(src + 4));
    gabi::store<u32>(dst + 8, gabi::load<u32>(src + 8));
}
/* the plane (normal.z, 0, s * normal.x) through m3724 (a cM3dGPla built by 020189EC, HD size 0x14) crossed with
 * the wall and the ground plane of mGndChk (cM3d_3PlaneCrossPos 020176D4) */
static inline BOOL lk_wallGroundCross(daPy_lk_c* t, u32 wall, f32 xsign, gabi::Local<cXyz>& out) {
    gabi::Local<cXyz> n;
    gabi::Local<cXyz> nn;
    n->x = gabi::load<f32>(wall + 8);
    n->y = 0.0f;
    if (xsign < 0.0f) {
        n->z = -gabi::load<f32>(wall + 0);
    } else {
        fcpy_l(n.a + 8, wall + 0);
    }
    gabi::call(0x0201B31C /* cXyz::normalize */, n.a, nn.a);
    f32 d = -gabi::fmadds(n->x, t->m3724.x, gabi::fmuls_ppc(n->z, t->m3724.z));
    gabi::Local<u8[0x14]> pla;
    gabi::call(0x020189EC /* cM3dGPla::cM3dGPla(const cXyz*, f32) */, pla.a, n.a, d);
    u32 gtri = dBgS_GetTriPla_l(gabi::ea(t) + 0xB28);
    return gabi::call<BOOL>(0x020176D4 /* cM3d_3PlaneCrossPos */, wall, gtri, pla.a, out.a);
}

/* 023E3850 */
void daPy_lk_c::setFrontWallType() {
    WWHD_FUNC(0x023E3850, void, this);
    f32 radius = gabi::load<f32>(gabi::ea(&mAcchCir[0]) + 0x34); /* mAcchCir[0].GetWallR() */
    if (mFrontWallType != 0) {
        return;
    }
    mFrontWallType = 1;
    if (!(mAcch.m_flags & 0x10) /* !ChkWallHit() */ && !(mModeFlg & 0x200200) /* ModeFlg_HOOKSHOT | ModeFlg_PUSHPULL */) {
        return;
    }
    if (LK_FIELD(f32, 0x3CC) < 0.0f) { /* checkGrabWear() */
        return;
    }
    s16 sy = shape_angle.y;
    f32 sn = cM_ssin(sy);
    f32 cs = cM_scos(sy);
    f32 r25 = radius + 25.0f;
    f32 dx = sn * r25;
    f32 dz = cs * r25;
    u32 chk = gabi::ea(this) + 0x9D0; /* mLinkLinChk */
    u32 chkPoly = chk + 0x14;
    gabi::Local<cXyz> start;
    gabi::Local<cXyz> end;
    int i;
    for (i = 2; i >= 0; i--) {
        f32 y = current.pos.y + gabi::load<f32>(gabi::ea(&mAcchCir[i]) + 0x30); /* GetWallH() */
        fcpy_l(start.a + 0, gabi::ea(&current.pos.x));
        start->y = y;
        fcpy_l(start.a + 8, gabi::ea(&current.pos.z));
        end->x = current.pos.x + dx;
        end->y = y;
        end->z = current.pos.z + dz;
        gabi::call(LK_dBgS_LinChk_Set, chk, start.a, end.a, this);
        if (lk_LineCross(chk)) {
            break;
        }
    }
    if (i == -1) {
        return;
    }
    u32 wall = dBgS_GetTriPla_l(chkPoly);
    if (wall == 0 || std::fabs(gabi::load<f32>(wall + 4)) > 0.05f) { /* HD: null check */
        return;
    }
    s16 a = cM_atan2s(gabi::load<f32>(wall + 0), gabi::load<f32>(wall + 8));
    s16 back = (s16)(shape_angle.y + 0x8000);
    m352C = a;
    if (cLib_distanceAngleS(a, back) > 0x2000) {
        return;
    }
    s32 wall_code;
    if (mModeFlg & 2) { /* ModeFlg_MIDAIR */
        gabi::store<u32>(gabi::ea(&m3724) + 0, gabi::load<u32>(chk + 0x30)); /* m3724 = mLinkLinChk.GetCross() */
        gabi::store<u32>(gabi::ea(&m3724) + 4, gabi::load<u32>(chk + 0x34));
        gabi::store<u32>(gabi::ea(&m3724) + 8, gabi::load<u32>(chk + 0x38));
        wall_code = dBgS_GetWallCode_l(chkPoly);
    } else {
        f64 dVar12 = gabi::call<f64>(0x02010C50 /* cM3d_SignedLenPlaAndPos */, wall, &current.pos);
        f32 x = lk_fnmsubs_d(dVar12, gabi::load<f32>(wall + 0), current.pos.x);
        f32 z = lk_fnmsubs_d(dVar12, gabi::load<f32>(wall + 8), current.pos.z);
        fcpy_l(gabi::ea(&m3724.y), gabi::ea(&current.pos.y));
        m3724.x = x;
        m3724.z = z;
        wall_code = dBgS_GetWallCode_l(chkPoly);
        if (wall_code != 4 && wall_code != 5) {
            f32 d2 = (f32)(dVar12 + dVar12);
            for (i = 2; i >= 0; i--) {
                f32 y = (current.pos.y + gabi::load<f32>(gabi::ea(&mAcchCir[i]) + 0x30)) + gabi::load<f32>(0x1047C780); /* HD: + a global */
                if (i == 2) {
                    y = (f32)((f64)y - 0.0085); /* HD: the top circle 0.0085 lower */
                }
                fcpy_l(start.a + 0, gabi::ea(&current.pos.x));
                start->y = y;
                fcpy_l(start.a + 8, gabi::ea(&current.pos.z));
                end->y = y;
                end->x = gabi::fnmsubs(d2, gabi::load<f32>(wall + 0), current.pos.x);
                end->z = gabi::fnmsubs(d2, gabi::load<f32>(wall + 8), current.pos.z);
                gabi::call(LK_dBgS_LinChk_Set, chk, start.a, end.a, this);
                if (lk_LineCross(chk)) {
                    u32 tri2 = dBgS_GetTriPla_l(chkPoly); /* HD: no null check */
                    gabi::Local<cXyz> sp90;
                    cXyz_mi(gabi::at<cXyz>(wall), sp90, gabi::at<cXyz>(tri2));
                    if (std_sqrtf(PSVECSquareMag(sp90)) < 0.001f) {
                        break;
                    }
                }
            }
            if (i == -1) {
                return;
            }
        }
        wall_code = dBgS_GetWallCode_l(chkPoly);
    }
    if (wall_code == 2) {
        return;
    }
    if (current.pos.y - m35D4 < 125.0f) {
        return;
    }
    if (mNoResetFlg0 & 0x100) { /* checkNoResetFlg0(daPyFlg0_UNK100) */
        if (wall_code == 3) {
            if (!(mModeFlg & 0x40002) /* ModeFlg_SWIM | ModeFlg_MIDAIR */) {
                setResetFlg0(resetFlg0() | 8); /* onResetFlg0(daPyRFlg0_UNK8) */
                lk_polyInfo_copy(gabi::ea(this) + 0xCE0 /* mPolyInfo */, chkPoly);
                if (mModeFlg & 0x200000) { /* ModeFlg_PUSHPULL */
                    return;
                }
            }
        } else if (wall_code == 1) {
            if (mModeFlg & 2) { /* ModeFlg_MIDAIR */
                u32 gnd = gabi::ea(this) + 0xB14; /* mGndChk.SetPos() */
                f32 py = current.pos.y;
                gabi::store<f32>(gnd + 0x28, py + 150.0f);
                gabi::store<f32>(gnd + 0x24, gabi::fnmsubs(15.0f, gabi::load<f32>(wall + 0), m3724.x));
                gabi::store<f32>(gnd + 0x2C, gabi::fnmsubs(15.0f, gabi::load<f32>(wall + 8), m3724.z));
                f64 g = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), gnd);
                if (!(g < (f64)current.pos.y)) {
                    u32 tri = dBgS_GetTriPla_l(gnd + 0x14);
                    if (tri != 0 && !(gabi::load<f32>(tri + 4) < 0.5f) /* cBgW_CheckBGround() */) {
                        gabi::Local<cXyz> sp84;
                        if (lk_wallGroundCross(this, wall, 1.0f, sp84) && !(sp84->y - current.pos.y > 150.0f)) {
                            mFrontWallType = 7;
                            fcpy_l(gabi::ea(&m3724.z), sp84.a + 8);
                            fcpy_l(gabi::ea(&m3724.x), sp84.a + 0);
                            fcpy_l(gabi::ea(&m3724.y), sp84.a + 4);
                            return;
                        }
                    }
                }
            }
            lk_polyInfo_copy(gabi::ea(this) + 0xCE0 /* mPolyInfo */, chkPoly);
            mFrontWallType = 3;
            return;
        } else if (wall_code == 4 || (wall_code == 5 && !(mModeFlg & 2))) {
            mFrontWallType = wall_code == 4 ? 4 : 5;
            gabi::Local<cXyz> sp6C, sp60, sp54;
            gabi::call(0x02008570 /* cBgS::GetTriPnt */, dComIfG_Bgsp(), chkPoly, sp6C.a, sp60.a, sp54.a);
            lk_polyInfo_copy(gabi::ea(this) + 0xCE0 /* mPolyInfo */, chkPoly);
            f32 y6C = sp6C->y;
            f32 y60 = sp60->y;
            u32 my = gabi::ea(&m3724.y);
            u32 m8 = gabi::ea(&m35F8);
            if (std::fabs(y6C - y60) < 1.0f) {
                m3724.x = gabi::fmuls_ppc(gabi::fadds_ppc(sp6C->x, sp60->x), 0.5f);
                if (sp54->y > y6C) {
                    fcpy_l(my, sp6C.a + 4);
                    fcpy_l(m8, sp54.a + 4);
                } else {
                    fcpy_l(my, sp54.a + 4);
                    fcpy_l(m8, sp6C.a + 4);
                }
                m3724.z = gabi::fmuls_ppc(gabi::fadds_ppc(sp6C->z, sp60->z), 0.5f);
            } else if (std::fabs(y6C - sp54->y) < 1.0f) {
                m3724.x = gabi::fmuls_ppc(gabi::fadds_ppc(sp6C->x, sp54->x), 0.5f);
                if (y60 > y6C) {
                    fcpy_l(my, sp6C.a + 4);
                    fcpy_l(m8, sp60.a + 4);
                } else {
                    fcpy_l(my, sp60.a + 4);
                    fcpy_l(m8, sp6C.a + 4);
                }
                m3724.z = gabi::fmuls_ppc(gabi::fadds_ppc(sp6C->z, sp54->z), 0.5f);
            } else {
                m3724.x = gabi::fmuls_ppc(gabi::fadds_ppc(sp60->x, sp54->x), 0.5f);
                if (y6C > y60) {
                    fcpy_l(my, sp60.a + 4);
                    fcpy_l(m8, sp6C.a + 4);
                } else {
                    fcpy_l(my, sp6C.a + 4);
                    fcpy_l(m8, sp60.a + 4);
                }
                m3724.z = gabi::fmuls_ppc(gabi::fadds_ppc(sp60->z, sp54->z), 0.5f);
            }
            return;
        }
    }
    /* HD: HIO folded (m_HIO->mWallCatch.m.field_0x18 = 170.0) */
    f32 py = current.pos.y;
    f32 r50 = radius + 50.0f;
    f32 y = py + 170.1f;
    fcpy_l(start.a + 0, gabi::ea(&current.pos.x));
    start->y = y;
    fcpy_l(start.a + 8, gabi::ea(&current.pos.z));
    end->x = gabi::fmadds(r50, sn, current.pos.x);
    end->y = y;
    end->z = gabi::fmadds(r50, cs, current.pos.z);
    gabi::call(LK_dBgS_LinChk_Set, chk, start.a, end.a, this);
    BOOL cVar8 = lk_LineCross(chk);
    if (!cVar8) {
        u32 roof = gabi::ea(this) + 0xB68; /* mRoofChk.SetPos(current.pos) */
        gabi::store<u32>(roof + 0x40, gabi::load<u32>(gabi::ea(&current.pos.z)));
        gabi::store<u32>(roof + 0x3C, gabi::load<u32>(gabi::ea(&current.pos.y)));
        gabi::store<u32>(roof + 0x38, gabi::load<u32>(gabi::ea(&current.pos.x)));
        f64 r = gabi::call<f64>(0x024EF6E8 /* dBgS::RoofChk */, dComIfG_Bgsp(), roof);
        cVar8 = !((f32)(r - (f64)current.pos.y) > 180.0f);
    }
    if (cVar8) {
        if (LK_checkGrabAnime() || (mModeFlg & 0x40002) /* ModeFlg_MIDAIR | ModeFlg_SWIM */) {
            return;
        }
        f32 r2 = radius + radius;
        s16 ca = current.angle.y;
        f32 y2 = current.pos.y + 149.9f;
        fcpy_l(start.a + 0, gabi::ea(&current.pos.x));
        start->y = y2;
        fcpy_l(start.a + 8, gabi::ea(&current.pos.z));
        end->x = gabi::fmadds(r2, cM_ssin(ca), current.pos.x);
        end->y = y2;
        end->z = gabi::fmadds(r2, cM_scos(ca), current.pos.z);
        gabi::Local<cXyz> sp48;
        if (getWHideModePolygon(start, end, sp48, 4 /* DIR_NONE */) != 0) {
            mFrontWallType = 2;
        }
        return;
    }
    u32 gnd = gabi::ea(this) + 0xB14; /* mGndChk.SetPos() */
    f32 py2 = current.pos.y;
    gabi::store<f32>(gnd + 0x28, py2 + 170.1f);
    gabi::store<f32>(gnd + 0x24, gabi::fnmsubs(15.0f, gabi::load<f32>(wall + 0), m3724.x));
    gabi::store<f32>(gnd + 0x2C, gabi::fnmsubs(15.0f, gabi::load<f32>(wall + 8), m3724.z));
    f64 ground_y = gabi::call<f64>(0x02008974 /* cBgS::GroundCross */, dComIfG_Bgsp(), gnd);
    if (!(mModeFlg & 0x40000) /* !ModeFlg_SWIM */ && ground_y < (f64)current.pos.y) {
        return;
    }
    if (ground_y == (f64)-1000000000.0f /* -G_CM3D_F_INF */) {
        return;
    }
    u32 tri = dBgS_GetTriPla_l(gnd + 0x14);
    if (tri != 0 && gabi::load<f32>(tri + 4) < 0.5f) { /* !cBgW_CheckBGround() (HD: null check) */
        return;
    }
    gabi::Local<cXyz> sp3C;
    if (!lk_wallGroundCross(this, wall, -1.0f, sp3C)) {
        return;
    }
    f32 fVar3 = sp3C->y - current.pos.y;
    fcpy_l(gabi::ea(&m3724.z), sp3C.a + 8); /* m3724 = sp3C */
    fcpy_l(gabi::ea(&m3724.y), sp3C.a + 4);
    fcpy_l(gabi::ea(&m3724.x), sp3C.a + 0);
    u32 mf = mModeFlg;
    /* HD: the NaN-safe forms of the original's branches; HIO folded */
    if (mf & 0x40000) { /* ModeFlg_SWIM */
        if (fVar3 > 37.6f || fVar3 < -5.0f) {
            return;
        }
        mFrontWallType = 7;
    } else if (mf & 2) { /* ModeFlg_MIDAIR */
        if (!((f32)(ground_y - (f64)mAcch.m_ground_h) > 125.0f)) {
            return;
        }
        if (mCurProc == 0x85 /* daPyProc_HOOKSHOT_FLY_e */ && !(fVar3 < 100.0f)) {
            return;
        }
        mFrontWallType = 7;
    } else {
        if (!(fVar3 < 170.1f)) {
            return;
        }
        if (LK_checkGrabAnime() && !(fVar3 < 75.1f)) {
            return;
        }
        if (fVar3 < 27.09f) {
            return;
        }
        if (fVar3 < 75.1f) {
            mFrontWallType = 6;
        } else if (fVar3 < 110.1f) {
            mFrontWallType = 7;
        } else if (fVar3 < 130.1f) {
            mFrontWallType = 8;
        } else {
            mFrontWallType = 9;
        }
    }
}
VERIFY(0x023E3850, &daPy_lk_c::setFrontWallType);

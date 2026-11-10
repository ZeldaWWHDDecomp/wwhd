#pragma once
#include "gc_api/gc_api.h"

enum : s16 { fpcNm_PLAYER_e = 0xA8 };
enum { dRes_INDEX_BDKOBJ_DZB_S_TOWER_BRIDGE_e = 0xC };
enum : u16 {
    ID_IT_SN_DK_BLOCK_S00 = 0x813E,
    ID_IT_SN_DK_BLOCK_L00 = 0x813F,
    ID_IT_SN_DK_POLE_S00 = 0x8141,
    ID_IT_SN_DK_POLE_L00 = 0x8142,
    ID_IT_SN_DK_BRIDGE_L00 = 0x8143,
    ID_IT_SN_DK_BRIDGE_S00 = 0x8144,
    ID_IT_SN_DK_BRIDGE_TENITA00 = 0x8145,
    ID_IT_ST_DK_BLOCK_SMOKE00 = 0xA13D,
    ID_IT_ST_DK_POLE_SMOKE00 = 0xA140,
    ID_IT_ST_DK_BRIDGE_SMOKE00 = 0xA146,
};
enum { JA_SE_OBJ_MJ_WBOARD_BRK = 0x6968 };

struct bdo_eff_s {
    /* 0x000 */ be<s8> mState;  /* was m000: daBdkobj_Execute 020729A8 sets 1 for the three block / pole fragments, tower_kaidan_move (in daBdkobj_Execute 020729A8) 2 for the two stair fragments; hahen_move (same) runs top_hahen_move at 1 and kaidan_hahen_move at 2; 0 = off (daBdkobj_Draw 020728A8 draws only the others; a type-9 hit, y < 8500 or no ground clears it) */
    /* 0x001 */ u8 _001[3];
    /* 0x004 */ gptr<J3DModel> mpModel;
    /* 0x008 */ cXyz mPos;  /* was m008: top_hahen_move / kaidan_hahen_move (in daBdkobj_Execute 020729A8): the fragment's position (moved by mSpeedF / mSpeedY, ground and wall checks); eff_set_mtx places the model there, mSph is centred on it */
    /* 0x014 */ cXyz mPosOld;  /* was m014: top_hahen_move / kaidan_hahen_move (in daBdkobj_Execute 020729A8) copy mPos before moving; on a wall hit x / z go back to it */
    /* 0x020 */ be<f32> mSpeedF;  /* was m020: daBdkobj_Execute 020729A8 sets 20..35 (+ reg) for a block / pole fragment; top_hahen_move (same) moves it forward along mAngle.y, eases it to 0 on the ground and clears it at a wall */
    /* 0x024 */ be<f32> mHorizontalSpeedX; /* HD: horizontal speed of the stair fragments */
    /* 0x028 */ be<f32> mHorizontalSpeedZ;
    /* 0x02C */ be<f32> mSpeedY;  /* was m024: daBdkobj_Execute 020729A8 sets 60..80 (+ reg); top_hahen_move (in daBdkobj_Execute 020729A8) -5 and kaidan_hahen_move -3 per frame, bounced (* -0.3) or cleared on the ground */
    /* 0x030 */ be<f32> mScale;  /* was m028: daBdkobj_Execute 020729A8 / tower_kaidan_move (in daBdkobj_Execute 020729A8) set 0.7 +- 0.25; eff_set_mtx scales the model by it, top_hahen_move sizes the sphere, ground offset and push by it; scale */
    /* 0x034 */ be<f32> mSpinRate;  /* was m02C: top_hahen_move (in daBdkobj_Execute 020729A8): random +-100 on each bounce; on the ground mAngleSpeed.y = mSpeedF * it */
    /* 0x038 */ csXyz mAngle;  /* was m030: daBdkobj_Execute 020729A8 / tower_kaidan_move (in daBdkobj_Execute 020729A8) set a random angle; top_hahen_move / kaidan_hahen_move add mAngleSpeed each frame; eff_set_mtx rotates the model by it */
    /* 0x03E */ csXyz mAngleSpeed;  /* was m036: daBdkobj_Execute 020729A8 / tower_kaidan_move (in daBdkobj_Execute 020729A8) set random +-6000 / +-800; added to mAngle each frame; top_hahen_move / kaidan_hahen_move ease or set it on the ground */
    /* 0x044 */ dCcD_Stts mStts;
    /* 0x080 */ dCcD_Sph mSph;
    /* 0x1AC */ be<f32> mPushSpeed;  /* was m1A4: top_hahen_move (in daBdkobj_Execute 020729A8): set on a type-8 hit (15 + rnd) * (1 - mScale), moves the fragment along mPushAngleY while > 0.1 and eases to 0; a new push only below 1 */
    /* 0x1B0 */ be<s16> mPushAngleY;  /* was m1A8: top_hahen_move (in daBdkobj_Execute 020729A8): the hitting actor's shape_angle.y +- 2000; YrotS by it for the push */
    /* 0x1B2 */ be<s16> mPushSpinY;  /* was m1AA: top_hahen_move (in daBdkobj_Execute 020729A8): random +-400 on a push, added to mAngle.y while the push runs */
    /* 0x1B4 */ be<s16> mFrameCounter;  /* was m1AC: hahen_move (in daBdkobj_Execute 020729A8) adds 1 per frame; tower_kaidan_move sets a random start; kaidan_hahen_move uses it as the phase of the rocking sines */
    /* 0x1B6 */ u8 _1B6[2];
};
WWHD_OFFSET(bdo_eff_s, mSph, 0x80);
WWHD_SIZE(bdo_eff_s, 0x1B8);

struct bdkobj_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<u8> mType;  /* was m298: daBdkobj_Create 020745AC: parameter byte 0 (0xFF -> 0); 0 = block, 1 = pole (BLOCK / POLE break particles in daBdkobj_Execute 020729A8), 2 = tower stairs (dBgW, tower_kaidan_move); useHeapInit 02074330 picks the models by it */
    /* 0x3B5 */ be<s8> mBreakTimer;  /* was m299: tower_kaidan_move (in daBdkobj_Execute 020729A8) sets 100 when the stairs break; daBdkobj_Execute 020729A8 counts it down; at 90 or less the bridge smoke fades */
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ bdo_eff_s mEffs[3];
    /* 0x8E0 */ dCcD_Stts mStts;
    /* 0x91C */ dCcD_Cyl mCyl;
    /* 0xA4C */ dPa_smokeEcallBack mSmokeCb;  /* was m918: daBdkobj_Execute 020729A8 / tower_kaidan_move (in daBdkobj_Execute 020729A8) pass it to the BLOCK / POLE / BRIDGE smoke particles (toon group) and end it first; daBdkobj_Create 020745AC sets follow off; daBdkobj_Delete 020742C4 removes it */
    /* 0xA6C */ be<u8> mSmokeAlpha;  /* was m938: tower_kaidan_move (in daBdkobj_Execute 020729A8): 0xB4 when the bridge smoke starts, -2 per frame once mBreakTimer is 90 or less; the smoke emitter's global alpha */
    /* 0xA6D */ u8 _A6D[3];
    /* 0xA70 */ Mtx34 mMtx;
    /* 0xAA0 */ gptr<dBgW> pm_bgw;
};
WWHD_OFFSET(bdkobj_class, mStts, 0x8E0);
WWHD_OFFSET(bdkobj_class, mSmokeCb, 0xA4C);
WWHD_OFFSET(bdkobj_class, mCyl, 0x91C);
WWHD_OFFSET(bdkobj_class, mMtx, 0xA70);
WWHD_SIZE(bdkobj_class, 0xAA4);

/* ---- local bindings (SHARED-CANDIDATE) ---- */
struct BdkObjAttackInfo {
    /* 0x00 */ gptr<dCcD_GObjInf> mpObj;
    /* 0x04 */ gptr<fopAc_ac_c> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
};
// HD: the original attack-info stack object is the compact 0x14-byte prefix.
WWHD_SIZE(BdkObjAttackInfo, 0x14);
fopAc_ac_c *BdkObj_powerCheck(BdkObjAttackInfo *);
void csXyz_apl(csXyz *, csXyz *);
void fopAcM_SetMin(fopAc_ac_c *, f32, f32, f32);
void fopAcM_SetMax(fopAc_ac_c *, f32, f32, f32);
void BdkObj_constructArray(void *, s32, u32, u32);
void BdkObj_destroyArray(void *, s32, u32, u32, u32, u32);
// HD: explicit construction preserves each inline vtable write and the distinct special-ground flags.
void BdkObj_groundBaseCtor(dBgS_GndChk *);
void BdkObj_lineBaseCtor(dBgS_LinChk *);
void BdkObj_groundBaseDtor(dBgS_GndChk *, s32);
void BdkObj_lineBaseDtor(dBgS_LinChk *, s32);
static inline void BdkObj_clearPass(dBgS_PolyPassChk *c, bool clearObject) {
    if (clearObject)
        c->mbObjThrough = 0;
    c->mbCamThrough = 0;
    c->mbLinkThrough = 0;
    c->mbArrowThrough = 0;
    c->mbBombThrough = 0;
    c->mbBoomerangThrough = 0;
    c->mbRopeThrough = 0;
}
static inline void BdkObj_groundCtor(dBgS_GndChk *g) {
    BdkObj_groundBaseCtor(g);
    BdkObj_clearPass(g, true);
    g->cBgS_Chk::__vtbl = 0x100083A4;
    g->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(g));
    g->cBgS_PolyInfo::__vtbl = 0x100083B4;
    g->dBgS_GrpPassChk::__vtbl = 0x100083C4;
    g->mGrp = 1;
    g->dBgS_PolyPassChk::__vtbl = 0x100083D4;
    g->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(g));
}
static inline void BdkObj_specialGroundCtor(dBgS_GndChk *g) {
    BdkObj_groundBaseCtor(g);
    BdkObj_clearPass(g, false);
    g->dBgS_GrpPassChk::__vtbl = 0x10008444;
    g->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(g));
    g->cBgS_Chk::__vtbl = 0x10008424;
    g->mGrp = 0xE;
    g->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(g));
    g->cBgS_PolyInfo::__vtbl = 0x10008434;
    g->dBgS_PolyPassChk::__vtbl = 0x10008454;
    g->mbObjThrough = 1;
}
static inline void BdkObj_groundSetPos(dBgS_GndChk *g, f32 x, f32 y, f32 z) {
    g->m_pos.x = x;
    g->m_pos.y = y;
    g->m_pos.z = z;
}
static inline void BdkObj_lineCtor(dBgS_LinChk *l) {
    BdkObj_lineBaseCtor(l);
    l->cBgS_PolyInfo::__vtbl = 0x10008474;
    BdkObj_clearPass(l, true);
    l->mGrpPassChk = gabi::ea(static_cast<dBgS_GrpPassChk *>(l));
    l->dBgS_GrpPassChk::__vtbl = 0x10008484;
    l->dBgS_PolyPassChk::__vtbl = 0x10008494;
    l->cBgS_Chk::__vtbl = 0x10008464;
    l->mPolyPassChk = gabi::ea(static_cast<dBgS_PolyPassChk *>(l));
    l->mGrp = 1;
}
static inline void BdkObj_destroyChecks(dBgS_LinChk *l, dBgS_GndChk *g) {
    l->dBgS_PolyPassChk::__vtbl = 0x10008494;
    l->dBgS_GrpPassChk::__vtbl = 0x10008394;
    l->cBgS_PolyInfo::__vtbl = 0x10008384;
    BdkObj_lineBaseDtor(l, 0);
    g->cBgS_PolyInfo::__vtbl = 0x100083B4;
    g->dBgS_PolyPassChk::__vtbl = 0x100083D4;
    g->dBgS_GrpPassChk::__vtbl = 0x10008394;
    BdkObj_groundBaseDtor(g, 0);
}

void BdkObj_firstHeaderCtor(u8 *);
void BdkObj_secondHeaderCtor(u8 *);

struct BdkObjEmitterView {
    u8 mPrefix[0x247];
    be<u8> mGlobalAlpha;
    u8 mReserved[0x254 - 0x248];
    be<u32> mStatus;
};
WWHD_OFFSET(BdkObjEmitterView, mStatus, 0x254);
struct BdkObjCameraView {
    u8 mPrefix[0xDC];
    cXyz mEye;
};
WWHD_OFFSET(BdkObjCameraView, mEye, 0xDC);
struct BdkObjBgWorldView {
    u8 mPrefix[0xA8];
    be<u32> mCrrFunc;
    u8 mReserved[4];
    be<u32> mRideCallback;
    enum : u32 { TypicalCorrection = 0x024EE658 };
};
WWHD_OFFSET(BdkObjBgWorldView, mRideCallback, 0xB0);
// GHS uses integer words for vector copies, preserving signaling NaN bit patterns.
inline be<u32> &BdkObj_word(be<f32> &field) { return reinterpret_cast<be<u32> &>(field); }
void BdkObj_sphereCtor(dCcD_Sph *);
void BdkObj_sphereDtor(dCcD_Sph *, s32);
void BdkObj_actorDtor(fopAc_ac_c *, s32);

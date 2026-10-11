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
    /* 0x000 */ be<s8> m000;
    /* 0x001 */ u8 _001[3];
    /* 0x004 */ gptr<J3DModel> mpModel;
    /* 0x008 */ cXyz m008;
    /* 0x014 */ cXyz m014;
    /* 0x020 */ be<f32> m020;
    /* 0x024 */ be<f32> mHorizontalSpeedX; /* HD: horizontal speed of the stair fragments */
    /* 0x028 */ be<f32> mHorizontalSpeedZ;
    /* 0x02C */ be<f32> m024;
    /* 0x030 */ be<f32> m028; /* scale */
    /* 0x034 */ be<f32> m02C;
    /* 0x038 */ csXyz m030;
    /* 0x03E */ csXyz m036;
    /* 0x044 */ dCcD_Stts mStts;
    /* 0x080 */ dCcD_Sph mSph;
    /* 0x1AC */ be<f32> m1A4;
    /* 0x1B0 */ be<s16> m1A8;
    /* 0x1B2 */ be<s16> m1AA;
    /* 0x1B4 */ be<s16> m1AC;
    /* 0x1B6 */ u8 _1B6[2];
};
WWHD_OFFSET(bdo_eff_s, mSph, 0x80);
WWHD_SIZE(bdo_eff_s, 0x1B8);

struct bdkobj_class : fopAc_ac_c {
    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ be<u8> m298;
    /* 0x3B5 */ be<s8> m299;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ bdo_eff_s mEffs[3];
    /* 0x8E0 */ dCcD_Stts mStts;
    /* 0x91C */ dCcD_Cyl mCyl;
    /* 0xA4C */ dPa_smokeEcallBack m918;
    /* 0xA6C */ be<u8> m938;
    /* 0xA6D */ u8 _A6D[3];
    /* 0xA70 */ Mtx34 mMtx;
    /* 0xAA0 */ gptr<dBgW> pm_bgw;
};
WWHD_OFFSET(bdkobj_class, mStts, 0x8E0);
WWHD_OFFSET(bdkobj_class, m918, 0xA4C);
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

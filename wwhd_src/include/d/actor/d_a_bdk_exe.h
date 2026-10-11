#pragma once
#include "d/actor/d_a_bdk.h"
// HD: four arrays replace tww's csXyz m9B8[4] / m9D0[4]. The fragments
// ease their yaw and roll towards separate targets instead of adding angular velocities.
struct BdkExeMaskAngles {
    be<s16> mYaw[4];
    be<s16> mTargetYaw[4];
    be<s16> mRoll[4];
    be<s16> mTargetRoll[4];
};
WWHD_OFFSET(BdkExeMaskAngles, mTargetYaw, 0x08);
WWHD_OFFSET(BdkExeMaskAngles, mRoll, 0x10);
WWHD_OFFSET(BdkExeMaskAngles, mTargetRoll, 0x18);
WWHD_SIZE(BdkExeMaskAngles, 0x20);
inline BdkExeMaskAngles &BdkExe_maskAngles(bdk_class *actor) {
    return *reinterpret_cast<BdkExeMaskAngles *>(actor->_AC8);
}

struct CcAtInfo_bdk {
    /* 0x00 */ be<u32> mpObj;
    /* 0x04 */ be<u32> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    /* 0x0B */ u8 _0B;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ be<u32> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
u32 at_power_check_bdk(CcAtInfo_bdk *info);
u32 cc_at_check_bdk(fopAc_ac_c *a, CcAtInfo_bdk *info);
void def_se_set_bdk(fopAc_ac_c *a, u32 obj, u32 se);
void dKy_SordFlush_set_bdk(cXyz *pos, s32 v);
void mDoAud_bgmStop_bdk(u32 t);
void cM3dGSph_SetR_bdk(void *sph, f32 r);
void cM3dGSph_SetC_bdk(void *sph, cXyz *c);
f32 BdkExe_groundCross(dBgS *, void *);
inline f32 bdk_GroundCross(void *chk) { return BdkExe_groundCross(dComIfG_Bgsp(), chk); }
BOOL BdkExe_lineCross(dBgS *, void *);
inline BOOL bdk_LineCross(void *chk) { return BdkExe_lineCross(dComIfG_Bgsp(), chk); }
void move(bdk_class *);
void my_effect_move(bdk_class *);
void obj_move(bdk_class *);
void demo_camera(bdk_class *);
void kankyo_cont(bdk_class *);

#include "d/actor/d_a_player.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
// HD: hammer-hit control fields beyond the shared player prefix.
struct BdkExePlayer : fopAc_ac_c {
    be<u8> mCutType;
    u8 reserved[0x69EA - 0x3AD];
    be<s16> mHammerHitTimer;
    be<u16> mHammerHitMode;
};
WWHD_OFFSET(BdkExePlayer, mHammerHitTimer, 0x69EA);
WWHD_OFFSET(BdkExePlayer, mHammerHitMode, 0x69EC);
struct BdkExePlay {
    u8 prefix[0x5B44];
    be<f32> mBossDeathValue;
};
// HD: owner-TU data views do not define another copy of any object.
struct BdkExeFootPositions {
    cXyz foot_eff_pos[2];
};
inline BdkExeFootPositions &BdkExe_feet() { return *gabi::at<BdkExeFootPositions>(0x104618D0); }
struct BdkExeFragmentStatics {
    be<u32> mInitialized;
    u8 gap[0x88];
    cXyz non_pos;
};
WWHD_OFFSET(BdkExeFragmentStatics, non_pos, 0x8C);
inline BdkExeFragmentStatics &BdkExe_fragments() { return *gabi::at<BdkExeFragmentStatics>(0x104618C8); }
struct BdkExeFragmentTables {
    be<s16> z_d[4];
    be<s16> z_d2[4];
    be<u8> kamen_break_time[4];
    be<f32> kamen_break_off_x[4];
    be<f32> kamen_break_off_y[4];
    be<f32> kamen_break_off_z[4];
};
WWHD_OFFSET(BdkExeFragmentTables, kamen_break_off_x, 0x14);
inline BdkExeFragmentTables &BdkExe_tables() { return *gabi::at<BdkExeFragmentTables>(0x10190654); }
inline be<u8> &BdkExe_pauseTimer() { return *gabi::at<be<u8>>(0x101EACB7); }
inline be<u32> &BdkExe_word(be<f32> &field) { return reinterpret_cast<be<u32> &>(field); }
inline const be<u32> &BdkExe_word(const be<f32> &field) { return reinterpret_cast<const be<u32> &>(field); }
inline void BdkExe_copyFloat(be<f32> &dst, const be<f32> &src) { BdkExe_word(dst) = BdkExe_word(src); }
inline void BdkExe_copyVector(cXyz *dst, const cXyz *src) {
    BdkExe_copyFloat(dst->x, src->x);
    BdkExe_copyFloat(dst->y, src->y);
    BdkExe_copyFloat(dst->z, src->z);
}
// HD: bit-preserving register copies retain lfs/stfs behavior without float arithmetic.
inline be<f32> &BdkExe_register(int bank, int index) {
    return *gabi::at<be<f32>>(0x1047B610 + 0x90 * bank + 4 * index);
}
struct BdkExeEmitter {
    u8 prefix[0x5C];
    be<s32> mMaxFrame;
    u8 gap[0x22C - 0x60];
    cXyz mGlobalTranslation;
    u8 tail[0x262 - 0x238];
    be<u8> mTranslationMode;
};
WWHD_OFFSET(BdkExeEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(BdkExeEmitter, mTranslationMode, 0x262);
void BdkExe_setGroundUpY(dBgS_Acch *, f32);
void BdkExe_seStartCurrent(fopAc_ac_c *, u32, u32);
void BdkExe_setEmitterStatus(JPABaseEmitter *, u32);
void BdkExe_setTranslation(cXyz *, f32, f32, f32);

// HD: add before translation because the original writes model+0xBC even for a null model.
inline cXyz &BdkExe_modelScale(J3DModel *model) { return *gabi::at<cXyz>(gabi::ea(model) + 0xBC); }

void BdkExe_linChkDestroy(dBgS_LinChk *, s32);
void BdkExe_gndChkDestroy(dBgS_GndChk *, s32);
inline void BdkExe_destroyChecks(dBgS_LinChk *lin, dBgS_GndChk *gnd) {
    lin->dBgS_PolyPassChk::__vtbl = 0x10007F38;
    lin->dBgS_GrpPassChk::__vtbl = 0x10007E88;
    lin->cBgS_PolyInfo::__vtbl = 0x10007E78;
    BdkExe_linChkDestroy(lin, 0);
    gnd->dBgS_PolyPassChk::__vtbl = 0x10007EC8;
    gnd->dBgS_GrpPassChk::__vtbl = 0x10007E88;
    gnd->cBgS_PolyInfo::__vtbl = 0x10007EA8;
    BdkExe_gndChkDestroy(gnd, 0);
}
enum BdkExeConstants {
    JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_CV_DK_LAST_DAMAGE = 0x486A,
    JA_SE_CM_DK_BREAK_MASK = 0x5875,
    ID_AK_JN_OK = 0xD,
    ID_IT_SN_DK_KAMEN_HAHEN00 = 0x8129,
    JA_SE_CV_DK_DAMAGE = 0x4868,
    ACTION_KUTI_ATTACK = 5,
    dRes_INDEX_BDK_BCK_DAMAGE1_e = 0x21,
    dRes_INDEX_BDK_BCK_DAMAGE3_e = 0x23,
    dRes_INDEX_BDK_BCK_DAMAGE2_e = 0x22,
    dRes_INDEX_BDK_BAS_DAMAGE2_e = 9,
    ID_IT_SN_DK_DMG_HANE_M00 = 0x8137,
    DK_JNT_J_DK_MUNE2_e = 0x16,
    DK_JNT_J_DK_ATAMA1_e = 0x18,
    DK_JNT_J_DK_AGO_e = 0x19,
    DK_JNT_J_DK_TOSAKA_A3_e = 0x21,
    DK_JNT_J_DK_ASHI_L3_e = 5,
    DK_JNT_J_DK_ASHI_R3_e = 0xE,
    ACTION_START = 0xF,
    JA_SE_CM_MAGBALL_BOUND = 0x5815,
    ID_IT_ST_DK_TOBITATI_SMOKE00 = 0xA136,
    JA_SE_CM_DK_NAIL = 0x5079,
    ACTION_T_DOWN = 0x67,
    ACTION_KAMEN_DEMO = 9,
    ACTION_WAIT = 3,
    ACTION_END = 10,
    ACTION_T_LASTATTACK = 0x66,
    ACTION_FLY_ATTACK = 7,
    ACTION_T_FLY = 0x64,
    ACTION_JIDA_ATTACK = 6,
};

static const dBgS_GndChk_vt bdk_gnd_vt = {0x10007E98, 0x10007EA8, 0x10007EC8, 0x10007EB8};

static const dBgS_LinChk_vt bdk_lin_vt = {0x10007F08, 0x10007F18, 0x10007F38, 0x10007F28};

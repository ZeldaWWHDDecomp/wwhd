/* ChuChu execution part: shared actor layout keeps the tww fields and HD deltas.
 * Private emitter rotation/translation view preserves the HD split-particle inlines. */
#pragma once
#include "d/actor/d_a_cc.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
struct CcExecRotation {
    be<f32> m[3][3];
};
WWHD_SIZE(CcExecRotation, 0x24);
struct CcExecEmitter {
    u8 padding[0x1F0];
    /* 0x1F0 */ CcExecRotation mGlobalRotation;
    u8 gap[0x18];
    /* 0x22C */ cXyz mGlobalTranslation;
    u8 gap238[0xC];
    /* 0x244 */ GXColor mPrmColor;
    /* 0x248 */ GXColor mEnvColor;
};
WWHD_OFFSET(CcExecEmitter, mGlobalRotation, 0x1F0);
WWHD_OFFSET(CcExecEmitter, mGlobalTranslation, 0x22C);
WWHD_OFFSET(CcExecEmitter, mPrmColor, 0x244);
WWHD_OFFSET(CcExecEmitter, mEnvColor, 0x248);
WWHD_SIZE(CcExecEmitter, 0x24C);
// HD: mapped alias to the main ChuChu translation unit's static storage, no duplicate definition.
inline be<u8>& DEMO_RET_START_FLAG() {
    return *gabi::at<be<u8>>(0x101B3AFD);
}
void naraku_check(cc_class*);
void draw_SUB(cc_class*);
void cc_eff_set(cc_class*, u8);
void anm_init(cc_class*, int, f32, u8, f32, int);
void denki_start(cc_class*);
void denki_end(cc_class*);
BOOL shock_damage_check(cc_class*);
void black_light_check(cc_class*);
BOOL body_atari_check(cc_class*);
BOOL search_angle_set(cc_class*);
void BG_check(cc_class*);
void action_noboru(cc_class*);
void action_up_check(cc_class*);
void action_tomaru(cc_class*);
void action_tubo_search(cc_class*);
void fopAcM_seStart_ool(fopAc_ac_c*, u32, u32);
CcExecRotation* CcExec_globalRotation(CcExecRotation*);
BOOL enemy_ice(enemyice_l*);
void enemy_piyo_set(fopAc_ac_c*);
void fopAcM_setGbaName(fopAc_ac_c*, u8, u8, u8);
void JPASetRMtxTVecfromMtx(Mtx34*, CcExecRotation*, cXyz*);
void CcExec_monsSeStart(u32, cXyz*, u32, u32, s32);
enum {
    dRes_INDEX_CC_BCK_ATACK01_e = 0x14,
    dRes_INDEX_CC_BCK_ATACK02_e = 0x15,
    dRes_INDEX_CC_BCK_ATACK03_e = 0x16,
    dRes_INDEX_CC_BCK_CC_BETA_e = 0x17,
    dRes_INDEX_CC_BCK_CC_PTCL_e = 0x18,
    dRes_INDEX_CC_BCK_HUKKATSU_e = 0x1A,
    dRes_INDEX_CC_BCK_HUSE2TACHI_e = 0x1B,
    dRes_INDEX_CC_BCK_HUSE_WALK_e = 0x1C,
    dRes_INDEX_CC_BCK_MAHI_e = 0x1D,
    dRes_INDEX_CC_BCK_START_e = 0x1E,
    dRes_INDEX_CC_BCK_TACHI2HUSE_e = 0x1F,
    dRes_INDEX_CC_BCK_TACHI_WALK_e = 0x20,
    dRes_INDEX_CC_BCK_TSTART01_e = 0x21,
    dRes_INDEX_CC_BCK_TSTART02_e = 0x22,
};
enum {
    JA_SE_CV_CC_STAND_UP = 0x4860,
    JA_SE_CV_CC_ATTACK = 0x4861,
    JA_SE_CV_CC_DAMAGE = 0x4862,
    JA_SE_CV_CC_DIE = 0x4863,
    JA_SE_CM_MD_PIYO = 0x50BC,
    JA_SE_CM_CC_ENTER_GND = 0x585D,
    JA_SE_CM_CC_LANDING = 0x585E,
    JA_SE_CM_CC_STAND_TO_LIE = 0x585F,
    JA_SE_CM_CC_LIE_TO_STAND = 0x5860,
    JA_SE_CM_CC_MOVE_LIE = 0x5861,
    JA_SE_CM_CC_MOVE_STAND = 0x5862,
    JA_SE_CM_CC_ATTACK = 0x5863,
    JA_SE_CM_CC_DMG_SWING = 0x5864,
    JA_SE_CM_CC_DIE_SWING = 0x5865,
    JA_SE_CM_CC_BLUE_SPARK = 0x5866,
    JA_SE_CM_B_CC_RECOVER = 0x591E,
};
enum {
    daDisItem_IBALL_e = 0,
    daDisItem_HEART_e = 0xA,
    daDisItem_MAGIC_e = 0xB,
    daDisItem_ARROW_e = 0xC,
    daDisItem_NONE13_e = 0xD,
};
enum {
    CC_JNT_BODY03_e = 3,
    dItemNo_BOW_e = 0x27,
    dItemNo_MIRROR_SHIELD_e = 0x3C,
    ID_IT_JN_PIYOHIT00 = 0x27B,
    ID_AK_SN_CCSPLIT00 = 0x82A8,
    ID_AK_JN_CCTHUNDER00 = 0x3ED,
    ID_AK_JN_CCTHUNDER01 = 0x3EE
};
enum {
    CC_TG_TYPE_ALL = 0xFFBDFEFFu,
    CC_TG_TYPE_CROUCH = 0x089DC060u,
    CC_TG_TYPE_SWIM = 0x083CC040u
};

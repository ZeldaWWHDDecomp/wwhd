/* ChuChu action helpers: uses the shared cc_class HD layout (+0x108 after mBehaviorType).
 * Private ABI views preserve the full hit-check record and the original demo-state storage. */
#pragma once
#include "d/actor/d_a_cc.h"
#include "d/actor/d_a_player.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
struct CcActionAtInfo {
    /* 0x00 */ gptr<dCcD_GObjInf> mpObj;
    /* 0x04 */ gptr<fopAc_ac_c> mpActor;
    /* 0x08 */ be<u8> mDamage;
    /* 0x09 */ be<u8> mbDead;
    /* 0x0A */ be<u8> mResultingAttackType;
    u8 padding;
    /* 0x0C */ csXyz m0C;
    /* 0x12 */ be<u16> mPlCutBit;
    /* 0x14 */ gptr<cXyz> pParticlePos;
    /* 0x18 */ be<s32> mHitSoundId;
};
WWHD_OFFSET(CcActionAtInfo, m0C, 0xC);
WWHD_OFFSET(CcActionAtInfo, pParticlePos, 0x14);
WWHD_OFFSET(CcActionAtInfo, mHitSoundId, 0x18);
WWHD_SIZE(CcActionAtInfo, 0x1C);
struct CcActionDemoFlags {
    be<u8> DEMO_COME_START_FLAG;
    be<u8> DEMO_RET_START_FLAG;
    be<u8> DEMO_SHORT_CUT_FLAG;
};
WWHD_OFFSET(CcActionDemoFlags, DEMO_RET_START_FLAG, 1);
WWHD_OFFSET(CcActionDemoFlags, DEMO_SHORT_CUT_FLAG, 2);
WWHD_SIZE(CcActionDemoFlags, 3);
// HD: aliases to the main cc source's owned statics, never duplicate definitions.
inline CcActionDemoFlags& demoFlags() {
    return *gabi::at<CcActionDemoFlags>(0x101B3AFC);
}
void anm_init(cc_class*, int, f32, u8, f32, int);
void denki_end(cc_class*);
void denki_start(cc_class*);
void cc_eff_set(cc_class*, u8);
fopAc_ac_c* cc_at_check(fopAc_ac_c*, CcActionAtInfo*);
void enemy_fire_remove(enemyfire_l*);
void CcAction_monsSeStart(u32, cXyz*, u32, u32, s32);
BOOL CcAction_detectLight(dDetect_c*, cXyz*);
enum {
    dRes_INDEX_CC_BCK_ATACK01_e = 0x14,
    dRes_INDEX_CC_BCK_ATACK02_e = 0x15,
    dRes_INDEX_CC_BCK_ATACK03_e = 0x16,
    dRes_INDEX_CC_BCK_DEKU_START_e = 0x19,
    dRes_INDEX_CC_BCK_MAHI_e = 0x1D,
    dRes_INDEX_CC_BCK_START_e = 0x1E,
    dRes_INDEX_CC_BCK_TACHI2HUSE_e = 0x1F,
    dRes_INDEX_CC_BCK_TACHI_WALK_e = 0x20,
};
enum {
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
    JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_CV_CC_DIE = 0x4863,
    JA_SE_CM_CC_DIE_SWING = 0x5865,
    JA_SE_CV_CC_DAMAGE = 0x4862,
    JA_SE_CM_CC_ATTACK = 0x5863,
    JA_SE_CM_CC_DMG_SWING = 0x5864,
    JA_SE_CM_CC_ENTER_GND = 0x585D,
    JA_SE_CM_CC_LANDING = 0x585E,
    JA_SE_CM_CC_STAND_TO_LIE = 0x585F,
    JA_SE_CM_CC_LIE_TO_STAND = 0x5860,
    JA_SE_CV_CC_ATTACK = 0x4861,
    JA_SE_CV_CC_PARASITE = 0x48BB,
    JA_SE_CM_CC_STONED = 0x58FB,
    JA_SE_CM_CC_STONED_RECOVER = 0x58FC,
    JA_SE_OBJ_PUT_STONE = 0x6929,
    JA_SE_OBJ_BREAK_STONE = 0x692B,
};
enum CcActionAttack {
    AT_TYPE_SWORD = 0x2,
    AT_TYPE_BOOMERANG = 0x40,
    AT_TYPE_BOKO_STICK = 0x80,
    AT_TYPE_WATER = 0x100,
    AT_TYPE_FIRE = 0x200,
    AT_TYPE_MACHETE = 0x400,
    AT_TYPE_UNK800 = 0x800,
    AT_TYPE_NORMAL_ARROW = 0x1000,
    AT_TYPE_UNK2000 = 0x2000,
    AT_TYPE_SPIKE = 0x4000,
    AT_TYPE_HOOKSHOT = 0x8000,
    AT_TYPE_SKULL_HAMMER = 0x10000,
    AT_TYPE_UNK20000 = 0x20000,
    AT_TYPE_FIRE_ARROW = 0x40000,
    AT_TYPE_ICE_ARROW = 0x80000,
    AT_TYPE_LIGHT_ARROW = 0x100000,
    AT_TYPE_WIND = 0x200000,
    AT_TYPE_UNK400000 = 0x400000,
    AT_TYPE_LIGHT = 0x800000,
    AT_TYPE_STALFOS_MACE = 0x1000000,
    AT_TYPE_DARKNUT_SWORD = 0x4000000,
    AT_TYPE_GRAPPLING_HOOK = 0x8000000,
    AT_TYPE_MOBLIN_SPEAR = 0x10000000,
};
enum CcActionCut {
    CUT_TYPE_BT_JUMPCUT = 0x5,
    CUT_TYPE_CUT_EA = 0x6,
    CUT_TYPE_CUT_EB = 0x7,
    CUT_TYPE_CUT_TURN = 0x8,
    CUT_TYPE_CUT_ROLL = 0x9,
    CUT_TYPE_JUMPCUT_SWORD = 0xa,
    CUT_TYPE_JUMPCUT_STICK = 0xc,
    CUT_TYPE_JUMPCUT_MACHETE = 0xe,
    CUT_TYPE_BT_ROLLCUT = 0xf,
    CUT_TYPE_BT_VERTICALJUMPCUT = 0x10,
    CUT_TYPE_HAMMER_FRONTSWING = 0x12,
    CUT_TYPE_JUMPCUT_HAMMER = 0x13,
    CUT_TYPE_JUMPCUT_CLUB = 0x15,
    CUT_TYPE_JUMPCUT_DN_SWORD = 0x17,
    CUT_TYPE_JUMPCUT_SPEAR = 0x19,
    CUT_TYPE_CUT_EXA = 0x1a,
    CUT_TYPE_CUT_EXB = 0x1b,
    CUT_TYPE_CUT_EXMJ = 0x1e,
    CUT_TYPE_CUT_KESA = 0x1f,
};
enum CcActionParticle {
    ID_AK_JN_NG = 0xC,
    ID_AK_JN_OK = 0xD,
    ID_AK_JN_CRITICALHIT = 0xF,
    ID_AK_JN_CRITICALHITFLASH = 0x10,
    ID_AK_SN_CCBREAKHAHEN00 = 0x82A9
};
enum { daDisItem_IBALL_e = 0, CC_TMP_DEKU_RETURN = 0x304 };

// HD: be byte access also supports the verifier's null/wrapped guest player pointers.
struct CcActionPlayer {
    u8 padding[0x3AC];
    /* 0x3AC */ be<u8> mCutType;
    u8 tail[0x13];
    /* 0x3C0 */ be<u32> mResetFlg0;
    u8 getCutType() {
        return mCutType;
    }
    bool checkFrontRollCrash() {
        return (mResetFlg0 & daPy_py_c::daPyRFlg0_FRONT_ROLL_CRASH) != 0;
    }
};
WWHD_OFFSET(CcActionPlayer, mCutType, 0x3AC);
WWHD_OFFSET(CcActionPlayer, mResetFlg0, 0x3C0);
WWHD_SIZE(CcActionPlayer, 0x3C4);

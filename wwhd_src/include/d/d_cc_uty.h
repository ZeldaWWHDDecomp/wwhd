/* Collision attack utilities: GameCube CcAtInfo layout and HD-only prefix views. */
#pragma once
#include "bindings.h"
#include "gc_api/f_op/f_op_actor.h"

struct CcAtInfo {
    gptr<dCcD_GObjInf> mpObj;
    gptr<fopAc_ac_c> mpActor;
    be<u8> mDamage, mbDead, mResultingAttackType;
    u8 mPadding;
    csXyz m0C;
    be<u16> mPlCutBit;
    gptr<cXyz> pParticlePos;
    be<u32> mHitSoundId;
};
WWHD_OFFSET(CcAtInfo, mDamage, 8);
WWHD_OFFSET(CcAtInfo, m0C, 0xC);
WWHD_OFFSET(CcAtInfo, mPlCutBit, 0x12);
WWHD_OFFSET(CcAtInfo, pParticlePos, 0x14);
WWHD_OFFSET(CcAtInfo, mHitSoundId, 0x18);
WWHD_SIZE(CcAtInfo, 0x1C);
struct cc_playerView : fopAc_ac_c {
    be<u8> mCutType, mCutCount;
    u8 mUnaccessed[0x69E8 - 0x3AE];
    be<u8> mJumpCutExtraDamage;
};
WWHD_OFFSET(cc_playerView, mCutType, 0x3AC);
WWHD_OFFSET(cc_playerView, mCutCount, 0x3AD);
WWHD_OFFSET(cc_playerView, mJumpCutExtraDamage, 0x69E8);
WWHD_SIZE(cc_playerView, 0x69E9);
struct cc_mtView : fopAc_ac_c {
    u8 mUnaccessed[0x570 - 0x3AC];
    be<u8> m454;
};
WWHD_OFFSET(cc_mtView, m454, 0x570);
WWHD_SIZE(cc_mtView, 0x571);
struct cc_gameInfo {
    u8 mUnaccessed[0x5AB0];
    gptr<dPa_control_c> mpParticle;
    u8 mRemaining[0x5B2C - 0x5AB4];
    gptr<cc_playerView> mpPlayer;
};
WWHD_OFFSET(cc_gameInfo, mpParticle, 0x5AB0);
WWHD_OFFSET(cc_gameInfo, mpPlayer, 0x5B2C);
WWHD_SIZE(cc_gameInfo, 0x5B30);
namespace cc_uty {
enum {
    RESULT_SWORD = 1,
    RESULT_BOMB = 2,
    RESULT_POT = 3,
    RESULT_WATER = 4,
    RESULT_FIRE = 5,
    RESULT_MT = 6,
    RESULT_MO2 = 7,
    RESULT_WIND = 8,
    RESULT_HAMMER = 9,
    RESULT_BOOMERANG = 10,
    RESULT_BDK = 11,
    RESULT_UNCLASSIFIED = 12,
    RESULT_BOMB_STATE8 = 13,
    RESULT_GRAPPLE = 14,
    ID_AK_JN_CRITICALHITFLASH = 0x10,
    ID_AK_JN_OK = 0xD,
    fpcNm_PLAYER_e = 168,
    fpcNm_BOMB_e = 294,
    fpcNm_Bomb2_e = 295,
    fpcNm_HIMO2_e = 446,
    fpcNm_BOOMERANG_e = 432,
    fpcNm_ARROW_e = 472,
    fpcNm_BDK_e = 238,
    fpcNm_TSUBO_e = 453,
    fpcNm_MO2_e = 188,
    fpcNm_MT_e = 216,
    fpcNm_ST_e = 190,
    AT_TYPE_WATER = 0x100,
    AT_TYPE_WIND = 0x200000,
    AT_TYPE_UNK20000 = 0x20000,
    AT_TYPE_FIRE = 0x200,
    AT_TYPE_FIRE_ARROW = 0x40000,
    AT_TYPE_SKULL_HAMMER = 0x10000,
    AT_TYPE_STALFOS_MACE = 0x1000000,
    AT_TYPE_HOOKSHOT = 0x8000,
    CUT_TYPE_CUT_A = 1,
    CUT_TYPE_CUT_EB = 7,
    CUT_TYPE_CUT_TURN = 8,
    CUT_TYPE_CUT_ROLL = 9,
    CUT_TYPE_JUMPCUT_SWORD = 10,
    CUT_TYPE_BT_JUMPCUT = 5,
    CUT_TYPE_BT_ROLLCUT = 15,
    CUT_TYPE_BT_VERTICALJUMPCUT = 16,
    dCcG_SE_WOOD = 4,
    dCcG_SE_METAL = 5,
    dCcG_SE_ARROW = 7,
    dCcG_SE_HOOKSHOT = 8,
    JA_SE_LK_HAMMER_HIT = 0x2855,
    JA_SE_LK_W_WEP_HIT = 0x2833,
    JA_SE_LK_MS_WEP_HIT = 0x2834,
    JA_SE_LK_W_WEP_CRT_HIT = 0x2835,
    JA_SE_LK_MS_WEP_CRT_HIT = 0x2836,
    JA_SE_LK_ARROW_HIT = 0x2879,
    JA_SE_LK_HS_SPIKE = 0x286F,
    JA_SE_LK_SW_HIT_S = 0x2803,
    JA_SE_LK_SW_CRT_HIT = 0x2806,
    JA_SE_LK_LAST_HIT = 0x2828,
    JA_SE_OBJ_MG_BALL_DMG = 0x693A,
};
}
// HD: call aliases preserve the verified register widths and floating results.
dCcD_GObjInf* cc_getGObjInf(dCcD_GObjInf* object);
u32 cc_getReverb(s32 room);
void cc_seStart(u32 id, cXyz* pos, u32 arg, u32 reverb);
u32 cc_bombCheckState(fopAc_ac_c* actor, u32 state);
u32 cc_tsuboGetType(fopAc_ac_c* actor, u32 shift, u32 width);
void cc_onEnemyDamage(fopAc_ac_c* actor);
void cc_speedSquared(cXyz* speed);
f32 cc_sqrt(f64 value);
u16 cc_atan2s(f64 y, f64 x);
void cc_createStealItem(cXyz* pos, u32 table, s32 room, u32 unused, u32 bit);
void cc_particleSet(dPa_control_c* particle, u32 group, u32 effect, cXyz* pos, csXyz* rotation,
                    cXyz* scale, u32 alpha, u32 callback, u32 room, u32 color, u32 color2,
                    u32 direction);
u16 cc_searchPlayerAngleY(fopAc_ac_c* actor, cc_playerView* player);
void mDoAud_bgmHitSound(u32 id);
void cc_assert(const char* file, u32 line, const char* condition);

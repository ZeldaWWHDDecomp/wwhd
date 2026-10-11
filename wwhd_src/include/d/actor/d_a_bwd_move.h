// Molgera movement: private HD call and data views; shared bwd_class layout is unchanged.
#pragma once
#include "d/actor/d_a_bwd.h"
#include "gc_api/f_op/f_op_actor.h"

struct BwdMovePlayerVtable {
    u8 mUnused[0x114];
    gfn mSetPlayerPosAndAngle;
    u8 mUnusedSetters[0x14];
    gfn mSetThrowDamage;
};
WWHD_OFFSET(BwdMovePlayerVtable, mSetPlayerPosAndAngle, 0x114);
WWHD_OFFSET(BwdMovePlayerVtable, mSetThrowDamage, 0x12C);
WWHD_SIZE(BwdMovePlayerVtable, 0x130);
struct BwdMovePlayerView {
    u8 mProcess[0xB4];
    gptr<BwdMovePlayerVtable> mVtable;
};
WWHD_OFFSET(BwdMovePlayerView, mVtable, 0xB4);
WWHD_SIZE(BwdMovePlayerView, 0xB8);
struct BwdMoveTargets {
    be<f32> target_x[3];
    be<f32> target_y[3];
    be<f32> target_z[3];
};
WWHD_OFFSET(BwdMoveTargets, target_y, 0xC);
WWHD_OFFSET(BwdMoveTargets, target_z, 0x18);
WWHD_SIZE(BwdMoveTargets, 0x24);
inline BwdMoveTargets& bwdMoveTargets() { return *reinterpret_cast<BwdMoveTargets*>(0x10192F90); }
inline GXColor& bwdMoveEffectColor() { return *reinterpret_cast<GXColor*>(eff_col); }
inline be<u8>& bwdMovePauseTimer() { return *reinterpret_cast<be<u8>*>(0x101EACB7); }

void bwdMoveSubtract(const cXyz*, cXyz*, const cXyz*);
void bwdMoveStopQuake(void*, s32);
void bwdMoveBgmStop(u32);
void bwdMoveBgmStart(u32);
void bwdMoveMonsSeStart(u32, cXyz*, u32, u32, s32);
void bwdMoveRemoveSmoke(dPa_smokeEcallBack_bd*);
void bwdMoveEnd(bwd_class*);

namespace bwd_move_resources {
enum dRes_INDEX_BWD {
    /* BCK  */
    dRes_INDEX_BWD_BCK_BWD_ATTACK_e = 0x7,
    dRes_INDEX_BWD_BCK_BWD_ATTACK_EAT_e = 0x8,
    dRes_INDEX_BWD_BCK_BWD_ATTACK_SHIPPAI_e = 0x9,
    dRes_INDEX_BWD_BCK_BWD_DAMAGE1_e = 0xA,
    dRes_INDEX_BWD_BCK_BWD_DAMAGE2_e = 0xB,
    dRes_INDEX_BWD_BCK_BWD_DEAD1_e = 0xC,
    dRes_INDEX_BWD_BCK_BWD_DEAD2_e = 0xD,
    dRes_INDEX_BWD_BCK_BWD_DEAD3_e = 0xE,
    dRes_INDEX_BWD_BCK_BWD_FLY_e = 0xF,
    dRes_INDEX_BWD_BCK_BWD_JATTACK1_e = 0x10,
    dRes_INDEX_BWD_BCK_BWD_JATTACK2_e = 0x11,
    dRes_INDEX_BWD_BCK_BWD_JATTACK3_e = 0x12,
    dRes_INDEX_BWD_BCK_BWD_NUKARERU_e = 0x13,
    dRes_INDEX_BWD_BCK_BWD_NUKARERU_LOOP_e = 0x14,
    dRes_INDEX_BWD_BCK_BWD_START_e = 0x15,
    dRes_INDEX_BWD_BCK_BWD_TEST_e = 0x16,
    dRes_INDEX_BWD_BCK_BWD_WAIT_e = 0x17,
    dRes_INDEX_BWD_BCK_GSP00_e = 0x18,
    dRes_INDEX_BWD_BCK_GSP01_e = 0x19,
    /* BDLM */
    dRes_INDEX_BWD_BDL_BERO_e = 0x1C,
    dRes_INDEX_BWD_BDL_BERO_SAKI_e = 0x1D,
    dRes_INDEX_BWD_BDL_BWD_e = 0x1E,
    dRes_INDEX_BWD_BDL_BWD_SHIPPOA_e = 0x1F,
    dRes_INDEX_BWD_BDL_BWD_SHIPPOB_e = 0x20,
    dRes_INDEX_BWD_BDL_GSP00_e = 0x21,
    dRes_INDEX_BWD_BDL_GSP01_e = 0x22,
    dRes_INDEX_BWD_BDL_HTAKI1_e = 0x23,
    dRes_INDEX_BWD_BDL_HTAKI2_e = 0x24,
    dRes_INDEX_BWD_BDL_HTRYF1_e = 0x25,
    /* BRK  */
    dRes_INDEX_BWD_BRK_BWD_e = 0x28,
    dRes_INDEX_BWD_BRK_BWD_SHIPPOA_e = 0x29,
    dRes_INDEX_BWD_BRK_BWD_SHIPPOB_e = 0x2A,
    dRes_INDEX_BWD_BRK_GSP00_e = 0x2B,
    dRes_INDEX_BWD_BRK_GSP01_e = 0x2C,
    dRes_INDEX_BWD_BRK_HSAND1_e = 0x2D,
    /* BTK  */
    dRes_INDEX_BWD_BTK_GSP00_e = 0x30,
    dRes_INDEX_BWD_BTK_GSP01_e = 0x31,
    dRes_INDEX_BWD_BTK_HSAND1_e = 0x32,
    dRes_INDEX_BWD_BTK_TAKI_END_e = 0x33,
    dRes_INDEX_BWD_BTK_TAKI_START_e = 0x34,
    dRes_INDEX_BWD_BTK_TAKI_WAIT_e = 0x35,
    /* DZB  */
    dRes_INDEX_BWD_DZB_HTAKI1_e = 0x38,
    dRes_INDEX_BWD_DZB_HTRYF1_e = 0x39,
};

}

namespace bwd_move_constants {
constexpr s32 fpcNm_BWDS_e = 0xDA;
enum : u32 {
    JA_SE_OBJ_BWD_SANDFALL_S = 0x69E1,
    JA_SE_CM_BWD_OUT_SAND_2 = 0x5913,
    JA_SE_CV_BWD_LAST_HIT = 0x48E1,
    JA_SE_CV_BWD_DAMAGE_L = 0x48DF,
    JA_SE_CM_BWD_IN_SAND_3 = 0x591C,
    JA_BGM_UNK_152 = 0x80000152,
    JA_BGM_UNK_124 = 0x80000124,
    JA_SE_CV_BWD_ATTACK = 0x48E0,
    JA_SE_CM_BWD_MOUTH_CLOSE_2 = 0x5915,
    JA_SE_CM_BWD_HEAD_BACK_1 = 0x5916,
    JA_SE_CM_BWD_IN_SAND_2 = 0x5914,
    JA_SE_CM_BWD_SPIT_OUT = 0x5917,
    JA_SE_CM_BWD_FLYING = 0x5110,
    JA_SE_CM_BWD_MOUTH_CLOSE_1 = 0x5911,
    JA_SE_CV_BWD_FLYING = 0x48DB,
    JA_SE_CV_BWD_GO_UNDER_G = 0x48DC,
    ID_AK_JN_CRITICALHITFLASH = 0x0010,
    ID_AK_JN_OK = 0x000D,
    ID_IT_SN_BWO_HIT_SUNA00 = 0x8253,
    ID_IT_SN_BWO_HIT_TAIEKI00 = 0x8252,
    ID_IT_SN_BWO_HITAFTER_HAKISUNA00 = 0x8254,
    ID_IT_SN_BWO_HITAFTER_SUNA00 = 0x8255,
    ID_IT_ST_BWO_HITAFTER_SMOKE00 = 0xA256,
    ID_IT_SN_BWO_ATTACK_SUNA00 = 0x824E,
    ID_IT_ST_BWO_PAKU_SMOKE00 = 0xA24F,
    ID_IT_SN_BWO_PAKUAFTER_SUNA00 = 0x8250,
    ID_IT_ST_BWO_PAKUAFTER_SMOKE00 = 0xA251,
    ID_IT_ST_BWK_OUT_SMOKE00 = 0xA24A,
    ID_IT_SN_BWK_OUT_SUNA00 = 0x8249,
};
} // namespace bwd_move_constants

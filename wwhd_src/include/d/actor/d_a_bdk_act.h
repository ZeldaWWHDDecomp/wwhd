#pragma once
#include "d/actor/d_a_bdk.h"
#undef l_HIO
#undef center_pos
#undef center_pos2
#undef wind_se_pos

// HD: views of the owner TU's statics; no duplicate object or linker symbol is introduced here.
struct BdkActStatics {
    cXyz foot_eff_pos[2];
    u8 reserved[0x10];
    cXyz center_pos;
    cXyz center_pos2;
    cXyz wind_se_pos;
    daBdk_HIO_c mHIO;
};
WWHD_OFFSET(BdkActStatics, center_pos, 0x28);
WWHD_OFFSET(BdkActStatics, mHIO, 0x4C);
inline BdkActStatics &BdkAct_statics() { return *gabi::at<BdkActStatics>(0x104618D0); }
struct BdkActMaskBreak {
    be<s8> kamen_break_time[4];
    be<f32> kamen_break_off_x[4];
    be<f32> kamen_break_off_y[4];
    be<f32> kamen_break_off_z[4];
    be<f32> kamen_break_sd_x[4];
    be<f32> kamen_break_sd_z[4];
};
WWHD_OFFSET(BdkActMaskBreak, kamen_break_sd_z, 0x44);
WWHD_SIZE(BdkActMaskBreak, 0x54);
inline const BdkActMaskBreak &BdkAct_maskBreak() { return *gabi::at<BdkActMaskBreak>(0x10190664); }
// HD: preserve integer-copy bit patterns and the original interleaved loads and stores.
inline be<u32> &BdkAct_word(be<f32> &field) { return reinterpret_cast<be<u32> &>(field); }
inline const be<u32> &BdkAct_word(const be<f32> &field) { return reinterpret_cast<const be<u32> &>(field); }
inline void BdkAct_copyFloat(be<f32> &dst, const be<f32> &src) { BdkAct_word(dst) = BdkAct_word(src); }
struct BdkActPlayerVtable {
    u8 prefix[0x4C];
    be<u32> checkPlayerFly;
};
struct BdkActPlayView {
    u8 prefix[0x5BAC];
    be<u16> mBossStartReady;
};
WWHD_OFFSET(BdkActPlayView, mBossStartReady, 0x5BAC);
void BdkAct_setGroundUpY(dBgS_Acch *, f32);
namespace BdkActSearch {
// The owner source's callbacks have internal linkage; these names identify their guest entry points.
enum : fpcM_SearchFunc {
    kamome_delete_sub = 0x02064CF4,
    dk_delete_sub = 0x02064DF0,
    kui_delete_sub = 0x02064D48,
    sea_delete_sub = 0x0206505C,
    obj_delete_sub = 0x02064E44,
    bk_delete_sub = 0x020650B0,
    boko_delete_sub = 0x02065104,
    obj_hahen_sub = 0x02064EA8,
    obj_s_sub = 0x020669B8,
    obj2_delete_sub = 0x02065158,
};
} // namespace BdkActSearch

enum BdkActConstants {
    ID_IT_SN_DK_LASTDMG_HANE_W00 = 0x8139,
    ID_IT_SN_DK_LASTDMG_HANE_W02 = 0x815A,
    ID_IT_SN_DK_LASTDMG_HANE_W01 = 0x8159,
    DK_JNT_J_DK_HANE_L2_e = 0x28,
    DK_JNT_J_DK_YUBI_LC1_e = 0xA,
    dRes_INDEX_BDK_BCK_KAKKU1_e = 0x2F,
    dRes_INDEX_BDK_BCK_DAMAGE4_e = 0x24,
    JA_SE_CM_DK_CRASH_WALL = 0x5877,
    JA_SE_CV_DK_KABE_DAMAGE = 0x4869,
    ID_IT_ST_DK_BUTSUKARI_SMOKE00 = 0xA135,
    DK_JNT_J_DK_MABUTA_e = 0x1C,
    ID_IT_SN_DK_DMG_HANE_M00 = 0x8137,
    DK_JNT_J_DK_MUNE2_e = 0x16,
    dRes_INDEX_BDK_BCK_PIKUPIKU1_e = 0x35,
    dRes_INDEX_BDK_BCK_FLY2_e = 0x2B,
    dRes_INDEX_BDK_BAS_FLY2_e = 0xC,
    ID_IT_SN_DK_TSUME_HIBANA_A00 = 0x812A,
    ID_IT_SN_DK_TSUME_HIBANA_B00 = 0x812B,
    JA_SE_CM_DK_GLIDING = 0x506F,
    dRes_INDEX_BDK_BCK_TOBITATU1_e = 0x3B,
    dRes_INDEX_BDK_BAS_TOBITATU1_e = 0x16,
    dRes_INDEX_BDK_BCK_NUKENAI1_e = 0x33,
    DK_JNT_J_DK_ATAMA1_e = 0x18,
    JA_BGM_UNK_110 = 0x80000110,
    dRes_INDEX_BDK_BCK_S_DEMO1_e = 0x39,
    JA_SE_CM_DK_ROUND_UP = 0x586B,
    ID_IT_SN_DK_DEMO_HANE00 = 0x813C,
    dRes_INDEX_BDK_BCK_S_DEMO2_e = 0x3A,
    dRes_INDEX_BDK_BAS_S_DEMO2_e = 0x15,
    JA_SE_CV_DK_ENTER = 0x4864,
    ID_IT_SN_DK_DEMO_HANE01 = 0x8158,
    dRes_INDEX_BDK_BCK_TO_DAMAGE_T1_e = 0x3D,
    dRes_INDEX_BDK_BCK_DAMAGE_T1_e = 0x25,
    dRes_INDEX_BDK_BCK_RAKKA_T1_e = 0x36,
    JA_SE_CM_DK_FALL_WATER = 0x589F,
    ID_IT_SN_DK_DOBON_HAMON00 = 0x8147,
    ID_IT_SN_DK_DOBON_HAMON01 = 0x8148,
    ID_IT_SN_DK_DOBON_SHIBUKI00 = 0x8149,
    ID_IT_SN_DK_DOBON_WP00 = 0x814A,
    JA_SE_CM_DK_WATER_COLUMN = 0x782E,
    dRes_INDEX_BDK_BCK_ATTACK_T1_e = 0x1D,
    JA_SE_CV_DK_ATTACK = 0x4865,
    dRes_INDEX_BDK_BCK_ATTACK_T2_e = 0x1E,
    JA_SE_CM_DK_FOOT_ATTACK = 0x589D,
    JA_SE_CM_DK_PECK = 0x589C,
    JA_SE_CM_DK_FOOT_ATK_COL = 0x589E,
    dRes_INDEX_BDK_BCK_TYAKUTI_T1_e = 0x3F,
    dRes_INDEX_BDK_BAS_TYAKUTI_T1_e = 0x18,
    ID_IT_SN_DK_TYAKUTI_ROCK00 = 0x8131,
    ID_IT_ST_DK_TYAKUTI_SMOKE00 = 0xA132,
    dRes_INDEX_BDK_BCK_WAIT_T1_e = 0x41,
    dRes_INDEX_BDK_BCK_ATTACK_T3_e = 0x1F,
    dRes_INDEX_BDK_BCK_LAST_DAMAGE1_e = 0x30,
    dRes_INDEX_BDK_BAS_LAST_DAMAGE1_e = 0x10,
    dRes_INDEX_BDK_BCK_LAST_DAMAGE3_e = 0x32,
    dRes_INDEX_BDK_BAS_LAST_DAMAGE3_e = 0x12,
    dRes_INDEX_BDK_BCK_LAST_DAMAGE2_e = 0x31,
    dRes_INDEX_BDK_BAS_LAST_DAMAGE2_e = 0x11,
    dRes_INDEX_BDK_BCK_DEATH1_e = 0x26,
    dRes_INDEX_BDK_BAS_DEATH1_e = 0xA,
    ID_IT_SN_DK_DEAD_HANE_A00 = 0x813A,
    JA_SE_CV_DK_DIE = 0x486C,
    dRes_INDEX_BDK_BCK_DEATH2_e = 0x27,
    JA_SE_CM_DK_DIE_EXPLODE = 0x5878,
    ID_IT_SN_DK_DEAD_HANE_B00 = 0x813B,
    JA_SE_CV_DK_MODAE = 0x486B,
    ID_IT_SN_DK_LASTDMG_HANE_M00 = 0x8138,
    ACTION_FLY = 0x0,
    ACTION_LANDING = 0x2,
    ACTION_KUTI_ATTACK = 0x5,
    ACTION_FLY_ATTACK = 0x7,
    ACTION_START = 0xf,
    ACTION_T_FLY = 0x64,
    ACTION_T_LASTATTACK = 0x66,
};

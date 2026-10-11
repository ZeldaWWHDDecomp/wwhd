/* Helmaroc King movement: private HD views and names, matching the owner TU's layout. */
#pragma once
#include "d/actor/d_a_bdk.h"
#include "gc_api/f_op/f_op_actor.h"
#include "gc_api/SSystem/c_lib.h"
#undef l_HIO

// HD: these reference the owner TU's storage without defining a second static object.
inline daBdk_HIO_c &BdkMove_hio() { return *gabi::at<daBdk_HIO_c>(0x1046191C); }
inline void BdkMove_copyFloat(be<f32> &dst, const be<f32> &src) {
    reinterpret_cast<be<u32> &>(dst) = reinterpret_cast<const be<u32> &>(src);
}
// HD: the inline destructor restores these three base vtables before its out-of-line call.
struct BdkMoveLineCheck {
    u8 prefix[0x20];
    be<u32> mPolyInfoVtable;
    u8 body[0x58 - 0x24];
    be<u32> mPolyPassVtable;
    u8 pass[8];
    be<u32> mGroupPassVtable;
    u8 group[4];
};
WWHD_OFFSET(BdkMoveLineCheck, mPolyInfoVtable, 0x20);
WWHD_OFFSET(BdkMoveLineCheck, mPolyPassVtable, 0x58);
WWHD_OFFSET(BdkMoveLineCheck, mGroupPassVtable, 0x64);
WWHD_SIZE(BdkMoveLineCheck, 0x6C);
static const dBgS_LinChk_vt BdkMove_lineVtables = {0x10007F08, 0x10007F18, 0x10007F38, 0x10007F28};
void BdkMove_destroyLineBase(BdkMoveLineCheck *, s32);
inline void BdkMove_destroyLineCheck(BdkMoveLineCheck *line) {
    line->mPolyPassVtable = 0x10007F38;
    line->mGroupPassVtable = 0x10007E88;
    line->mPolyInfoVtable = 0x10007E78;
    BdkMove_destroyLineBase(line, 0);
}
void BdkMove_report(s32, s32, const char *, s32);
BOOL BdkMove_landAreaCheck(cXyz *, f32);
void fly_attack(bdk_class *);
void wind_attack(bdk_class *);
void kamen_demo(bdk_class *);
void end(bdk_class *);
void start(bdk_class *);
void t_fly(bdk_class *);
void t_lastattack(bdk_class *);
void t_down(bdk_class *);
void BdkMove_afterFight(bdk_class *);
enum BdkMoveConstants {
    dRes_INDEX_BDK_BCK_FLY1_e = 0x2A,
    dRes_INDEX_BDK_BAS_FLY1_e = 0xB,
    dRes_INDEX_BDK_BCK_TOBITATU1_e = 0x3B,
    dRes_INDEX_BDK_BAS_TOBITATU1_e = 0x16,
    dRes_INDEX_BDK_BCK_TYAKUTI1_e = 0x3E,
    dRes_INDEX_BDK_BAS_TYAKUTI1_e = 0x17,
    dRes_INDEX_BDK_BCK_KAKKU1_e = 0x2F,
    ID_IT_SN_DK_TYAKUTI_ROCK00 = 0x8131,
    ID_IT_ST_DK_TYAKUTI_SMOKE00 = 0xA132,
    dRes_INDEX_BDK_BCK_WAIT1_e = 0x40,
    dRes_INDEX_BDK_BCK_JUMP1_e = 0x2E,
    dRes_INDEX_BDK_BAS_JUMP1_e = 0xF,
    dRes_INDEX_BDK_BCK_SENKAI_L1_e = 0x37,
    dRes_INDEX_BDK_BAS_SENKAI_L1_e = 0x13,
    dRes_INDEX_BDK_BCK_SENKAI_R1_e = 0x38,
    dRes_INDEX_BDK_BAS_SENKAI_R1_e = 0x14,
    dRes_INDEX_BDK_BCK_WALK1_e = 0x42,
    dRes_INDEX_BDK_BAS_WALK1_e = 0x19,
    dRes_INDEX_BDK_BCK_JIDANDA1_e = 0x2D,
    dRes_INDEX_BDK_BAS_JIDANDA1_e = 0xE,
    ID_IT_SN_DK_JIDANDA_ROCK00 = 0x8133,
    ID_IT_ST_DK_JIDANDA_SMOKE00 = 0xA134,
    dRes_INDEX_BDK_BCK_ATTACK1_e = 0x1C,
    dRes_INDEX_BDK_BAS_ATTACK1_e = 7,
    JA_SE_CV_DK_ATTACK = 0x4865,
    JA_SE_CM_DK_ATTACK = 0x5872,
    ID_IT_SN_DK_KUTI_SENKO00 = 0x812D,
    dRes_INDEX_BDK_BCK_NUKENAI1_e = 0x33,
    JA_SE_CM_DK_JITABATA = 0x5873,
    JA_SE_CV_DK_NUKEZU = 0x4866,
    dRes_INDEX_BDK_BCK_NUKU1_e = 0x34,
    JA_SE_CM_DK_PULL_UP_HEAD = 0x5874,
    JA_SE_CV_DK_NUKERU = 0x4867,
    ID_IT_ST_DK_KUTI_SMOKE00 = 0xA12E,
    ID_IT_SN_DK_KUTI_ROCK00 = 0x812C,
    ACTION_FLY = 0x0,
    ACTION_UP_FLY = 0x1,
    ACTION_LANDING = 0x2,
    ACTION_WAIT = 0x3,
    ACTION_JUMP = 0x4,
    ACTION_KUTI_ATTACK = 0x5,
    ACTION_JIDA_ATTACK = 0x6,
    ACTION_FLY_ATTACK = 0x7,
    ACTION_WIND_ATTACK = 0x8,
    ACTION_KAMEN_DEMO = 0x9,
    ACTION_END = 0xa,
    ACTION_START = 0xf,
    ACTION_T_FLY = 0x64,
    ACTION_T_LASTATTACK = 0x66,
    ACTION_T_DOWN = 0x67,
    ACTION_AFTER_FIGHT = 0x6e,
};

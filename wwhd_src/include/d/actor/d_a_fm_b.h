/* Floormaster part B: private HD views and full-width call adapters. */
#pragma once
#include "d/actor/d_a_fm.h"
#include "d/d_cc_uty.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"

// HD: the GC HIO fields used here retain their offsets; other fields are untouched.
struct daFm_HIO_c {
    u8 mUnaccessed0[0xF];
    be<u8> field_0x00F[6];
    u8 mUnaccessed15[0x4D];
    be<s16> mFreezeDuration;
    u8 mUnaccessed64[0x24];
    be<s16> field_0x088;
    be<s16> field_0x08A;
    be<s16> field_0x08C;
    be<s16> field_0x08E;
    u8 mUnaccessed90[0x4];
    be<s16> field_0x094;
    be<s16> field_0x096;
    be<s16> field_0x098;
    be<s16> field_0x09A;
    be<s16> field_0x09C;
    u8 mUnaccessed9E[0xA];
    be<f32> field_0x0A8;
    u8 mUnaccessedAC[0x10];
    be<f32> field_0x0BC;
    u8 mUnaccessedC0[0x4];
    be<f32> field_0x0C4;
    u8 mUnaccessedC8[0x4];
    be<f32> field_0x0CC;
    u8 mUnaccessedD0[0xC];
    be<f32> field_0x0DC;
    be<f32> field_0x0E0;
    be<f32> field_0x0E4;
    be<f32> field_0x0E8;
    be<f32> field_0x0EC;
    u8 mUnaccessedF0[0x10];
    be<s16> field_0x100;
    u8 mUnaccessed102[0xE];
    be<f32> field_0x110;
    be<f32> field_0x114;
    be<f32> field_0x118;
    u8 mUnaccessed11C[0x15C - 0x11C];
};
WWHD_OFFSET(daFm_HIO_c, field_0x00F, 0xF);
WWHD_OFFSET(daFm_HIO_c, mFreezeDuration, 0x62);
WWHD_OFFSET(daFm_HIO_c, field_0x088, 0x88);
WWHD_OFFSET(daFm_HIO_c, field_0x08A, 0x8A);
WWHD_OFFSET(daFm_HIO_c, field_0x08C, 0x8C);
WWHD_OFFSET(daFm_HIO_c, field_0x08E, 0x8E);
WWHD_OFFSET(daFm_HIO_c, field_0x094, 0x94);
WWHD_OFFSET(daFm_HIO_c, field_0x096, 0x96);
WWHD_OFFSET(daFm_HIO_c, field_0x098, 0x98);
WWHD_OFFSET(daFm_HIO_c, field_0x09A, 0x9A);
WWHD_OFFSET(daFm_HIO_c, field_0x09C, 0x9C);
WWHD_OFFSET(daFm_HIO_c, field_0x0A8, 0xA8);
WWHD_OFFSET(daFm_HIO_c, field_0x0BC, 0xBC);
WWHD_OFFSET(daFm_HIO_c, field_0x0C4, 0xC4);
WWHD_OFFSET(daFm_HIO_c, field_0x0CC, 0xCC);
WWHD_OFFSET(daFm_HIO_c, field_0x0DC, 0xDC);
WWHD_OFFSET(daFm_HIO_c, field_0x0E0, 0xE0);
WWHD_OFFSET(daFm_HIO_c, field_0x0E4, 0xE4);
WWHD_OFFSET(daFm_HIO_c, field_0x0E8, 0xE8);
WWHD_OFFSET(daFm_HIO_c, field_0x0EC, 0xEC);
WWHD_OFFSET(daFm_HIO_c, field_0x100, 0x100);
WWHD_OFFSET(daFm_HIO_c, field_0x110, 0x110);
WWHD_OFFSET(daFm_HIO_c, field_0x114, 0x114);
WWHD_OFFSET(daFm_HIO_c, field_0x118, 0x118);
WWHD_SIZE(daFm_HIO_c, 0x15C);
inline daFm_HIO_c& fm_HIO() { return *gabi::at<daFm_HIO_c>(0x10463E0C); }
inline be<f32>& fm_invKineDebug() { return *gabi::at<be<f32>>(0x1047BD34); }
inline be<u16>& fm_makarCarryFlags() { return *gabi::at<be<u16>>(0x101BDC2A); }
struct Fm_Medli : fopAc_ac_c {
    u8 mUnaccessed[0x420C - 0x3AC];
    be<u32> mNoCarryFlags;
};
WWHD_OFFSET(Fm_Medli, mNoCarryFlags, 0x420C);
struct Fm_Player : fopAc_ac_c {
    be<u8> mCutType;
    u8 mUnaccessed3AD[0x3D8 - 0x3AD];
    cXyz mHeadTopPos;
    u8 mUnaccessed3E4[0x65F0 - 0x3E4];
    be<s32> mCurrentProc;
};
WWHD_OFFSET(Fm_Player, mCutType, 0x3AC);
WWHD_OFFSET(Fm_Player, mHeadTopPos, 0x3D8);
WWHD_OFFSET(Fm_Player, mCurrentProc, 0x65F0);
struct Fm_PlayerVtable {
    u8 mUnaccessed[0xBC];
    be<u32> getGrabActorID;
    u8 mUnaccessedC0[0xE4 - 0xC0];
    be<u32> voiceStart;
    u8 mUnaccessedE8[4];
    be<u32> setOutPower;
};
WWHD_OFFSET(Fm_PlayerVtable, getGrabActorID, 0xBC);
WWHD_OFFSET(Fm_PlayerVtable, voiceStart, 0xE4);
WWHD_OFFSET(Fm_PlayerVtable, setOutPower, 0xEC);
inline Fm_Player* fm_player(fopAc_ac_c* actor) { return static_cast<Fm_Player*>(actor); }
inline Fm_PlayerVtable* fm_playerVtable(fopAc_ac_c* actor) { return gabi::at<Fm_PlayerVtable>(actor->__vtbl); }
inline u32 daPy_getGrabActorID(fopAc_ac_c* actor) { return gabi::call_ptr<u32>(fm_playerVtable(actor)->getGrabActorID, actor); }
inline void daPy_voiceStart(fopAc_ac_c* actor, u32 id) { gabi::call_ptr(fm_playerVtable(actor)->voiceStart, actor, id); }
inline void daPy_setOutPower(fopAc_ac_c* actor, f32 power, s16 angle, s32 arg) { gabi::call_ptr(fm_playerVtable(actor)->setOutPower, actor, power, angle, arg); }
inline bool daPy_checkCarryActionNow(fopAc_ac_c* actor) {
    if (fm_player(actor)->mCurrentProc == 0x72) return true;
    s32 proc = fm_player(actor)->mCurrentProc;
    return proc == 0x6F || proc == 0x71;
}
struct Fm_HitObject { u8 mUnaccessed[0x10]; be<u32> mAtType; };
WWHD_OFFSET(Fm_HitObject, mAtType, 0x10);
struct fm_nameKeys { be<s16> k0, k1; };
WWHD_SIZE(fm_nameKeys, 4);
// HD: callbacks outside this part retain their original guest entry points.
constexpr u32 FM_SEARCH_NEAR_OTHER_ACTOR_CB = 0x02140F04;
constexpr u32 FM_SEARCH_BY_NAME_CB = 0x025E121C;
inline bool fm_checkCarryNow(fopAc_ac_c* actor) { return (actor->actor_status & fopAcStts_CARRY_e) != 0; }
inline bool fm_checkPlayerStatus0(u32 mask) { return dComIfGp_checkPlayerStatus0(0, mask) != 0; }
void fm_setAnm(daFm_c* a, s32 idx, u32 force);
void fm_modeProc(daFm_c* a, s32 proc, s32 mode);
BOOL fm_isLink(daFm_c* a, fopAc_ac_c* target);
BOOL fm_isNpc(daFm_c* a, fopAc_ac_c* target);
BOOL fm_checkHeight(daFm_c* a, fopAc_ac_c* target);
BOOL fm_checkPlayerGrabTarget(daFm_c* a);
BOOL fm_isLinkControl(daFm_c* a);
void fm_moveRndBack(daFm_c* a);
void fm_resetInvKine(daFm_c* a);
void fm_cancelGrab(daFm_c* a);
BOOL fm_areaCheck(daFm_c* a);
BOOL fm_isGrabFoot(daFm_c* a);
s32 fm_setRnd(daFm_c* a, s32 low, s32 high);
void fm_turnToBaseTarget(daFm_c* a);
void fm_setCarryNow(fopAc_ac_c* a, u32 arg);
BOOL fm_bomb_chk_state(fopAc_ac_c* a, u32 state);
u32 fm_PrmAbstract(fopAc_ac_c* a, s32 width, s32 shift);
void quat_rotVec(Quaternion_fm* q, cXyz* a, cXyz* b);
void C_QUATSlerp(Quaternion_fm* p, Quaternion_fm* q, Quaternion_fm* r, f32 t);
s32 fm_calcTimer(be<s32>* timer);
void fm_addCalcPosXZ2(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep);
u32 fm_at_check(fopAc_ac_c* actor, CcAtInfo* info);
void fm_monsSound(u32 id, cXyz* pos, u32 procId, u32 param, s32 reverb);
BOOL fm_startCheckOld(void* manager, const char* name);
inline void fm_modeProcInit(daFm_c* actor, s32 mode) { fm_modeProc(actor, 0, mode); }
inline fopAc_ac_c* fm_SearchByName(be<s16>* key, s16 name) {
    *key = name;
    return fopAcIt_Judge(FM_SEARCH_BY_NAME_CB, key);
}
inline void fm_seStart(fopAc_ac_c* actor, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
    mDoAud_seStart(id, &actor->eyePos, param, reverb);
}
inline void fm_monsSeStart(fopAc_ac_c* actor, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
    fm_monsSound(id, &actor->eyePos, actor->mBsPcId, param, reverb);
}

// GameCube attack, cut and particle names; HD values from this verified unit.
enum Fm_CombatConstants {
    ID_IT_JN_PIYOHIT00 = 0x27B,
    CUT_TYPE_HAMMER_SIDESWING = 0x11,
    ID_AK_JN_CRITICALHITFLASH = 0x10,
    ID_AK_JN_CRITICALHIT = 0xF,
    ID_AK_JN_OK = 0xD,
    CUT_TYPE_BT_JUMPCUT = 0x5,
    CUT_TYPE_CUT_EA = 0x6,
    CUT_TYPE_CUT_EB = 0x7,
    CUT_TYPE_CUT_TURN = 0x8,
    CUT_TYPE_CUT_ROLL = 0x9,
    CUT_TYPE_JUMPCUT_SWORD = 0xA,
    CUT_TYPE_JUMPCUT_STICK = 0xC,
    CUT_TYPE_JUMPCUT_MACHETE = 0xE,
    CUT_TYPE_BT_ROLLCUT = 0xF,
    CUT_TYPE_JUMPCUT_CLUB = 0x15,
    CUT_TYPE_JUMPCUT_DN_SWORD = 0x17,
    CUT_TYPE_JUMPCUT_SPEAR = 0x19,
    CUT_TYPE_CUT_EXA = 0x1A,
    CUT_TYPE_CUT_EXB = 0x1B,
};

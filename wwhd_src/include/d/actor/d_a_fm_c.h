/* Floormaster part C: private HD field views and exact call adapters. */
#pragma once
#include "d/actor/d_a_fm.h"
#include "gc_api/f_op/f_op_actor_mng.h"
#include "gc_api/d/d_com_inf_game.h"
namespace fm_c {
// Names and order from the GC mode_tbl.
enum Mode {
    MODE_SW_WAIT = 0,
    MODE_HIDE = 1,
    MODE_UNDERFOOT = 2,
    MODE_PATH_MOVE = 3,
    MODE_GOAL_KEEPER = 4,
    MODE_APPEAR = 5,
    MODE_DISAPPEAR = 6,
    MODE_WAIT = 7,
    MODE_ATTACK = 8,
    MODE_THROW = 9,
    MODE_DAMAGE = 10,
    MODE_PREPARE_ITEM = 11,
    MODE_DEATH = 12,
    MODE_GRAB = 13,
    MODE_GRAB_DEMO = 14,
    MODE_GRAB_FOOT_DEMO = 15,
    MODE_GRAB_NPC_DEMO = 16,
    MODE_PLAYER_START_DEMO = 17,
    MODE_DELETE = 18,
    MODE_PARALYSIS = 19,
    MODE_BIKUBIKU = 20,
};
inline const be<u8>* stringBytes(const char* string) { return gabi::at<const be<u8>>(gabi::guest_addr(string)); }
// GC HIO fields retain their offsets in the HD object. Unused fields stay opaque.
struct HIO {
    u8 mUnaccessed0[0xD];
    be<u8> field_0x00D;
    u8 mUnaccessedE[0x5A];
    be<s16> field_0x068;
    u8 mUnaccessed6A[0x26];
    be<s16> field_0x090;
    be<s16> field_0x092;
    u8 mUnaccessed94[0x2];
    be<s16> field_0x096;
    u8 mUnaccessed98[0x2];
    be<s16> field_0x09A;
    u8 mUnaccessed9C[0x2];
    be<s16> field_0x09E;
    be<s16> field_0x0A0;
    u8 mUnaccessedA2[0x4];
    be<s16> field_0x0A6;
    u8 mUnaccessedA8[0x8];
    be<f32> field_0x0B0;
    be<f32> field_0x0B4;
    be<f32> field_0x0B8;
    u8 mUnaccessedBC[0x8];
    be<f32> field_0x0C4;
    u8 mUnaccessedC8[0x4];
    be<f32> field_0x0CC;
    u8 mUnaccessedD0[0x20];
    be<f32> field_0x0F0;
    be<f32> field_0x0F4;
    be<f32> field_0x0F8;
    be<f32> field_0x0FC;
    u8 mUnaccessed100[0x10];
    be<f32> field_0x110;
    be<f32> field_0x114;
    be<f32> field_0x118;
    be<s16> field_0x11C;
    u8 mUnaccessed11E[0x2];
    be<f32> field_0x120;
    be<f32> field_0x124;
    be<f32> field_0x128;
    u8 mUnaccessed12C[0x15C - 0x12C];
};
WWHD_OFFSET(HIO, field_0x00D, 0xD);
WWHD_OFFSET(HIO, field_0x068, 0x68);
WWHD_OFFSET(HIO, field_0x090, 0x90);
WWHD_OFFSET(HIO, field_0x092, 0x92);
WWHD_OFFSET(HIO, field_0x096, 0x96);
WWHD_OFFSET(HIO, field_0x09A, 0x9A);
WWHD_OFFSET(HIO, field_0x09E, 0x9E);
WWHD_OFFSET(HIO, field_0x0A0, 0xA0);
WWHD_OFFSET(HIO, field_0x0A6, 0xA6);
WWHD_OFFSET(HIO, field_0x0B0, 0xB0);
WWHD_OFFSET(HIO, field_0x0B4, 0xB4);
WWHD_OFFSET(HIO, field_0x0B8, 0xB8);
WWHD_OFFSET(HIO, field_0x0C4, 0xC4);
WWHD_OFFSET(HIO, field_0x0CC, 0xCC);
WWHD_OFFSET(HIO, field_0x0F0, 0xF0);
WWHD_OFFSET(HIO, field_0x0F4, 0xF4);
WWHD_OFFSET(HIO, field_0x0F8, 0xF8);
WWHD_OFFSET(HIO, field_0x0FC, 0xFC);
WWHD_OFFSET(HIO, field_0x110, 0x110);
WWHD_OFFSET(HIO, field_0x114, 0x114);
WWHD_OFFSET(HIO, field_0x118, 0x118);
WWHD_OFFSET(HIO, field_0x11C, 0x11C);
WWHD_OFFSET(HIO, field_0x120, 0x120);
WWHD_OFFSET(HIO, field_0x124, 0x124);
WWHD_OFFSET(HIO, field_0x128, 0x128);
WWHD_SIZE(HIO, 0x15C);
inline HIO& hio() { return *gabi::at<HIO>(0x10463E0C); }
struct Demo {
    enum Type { TYPE_ORIGINAL_e = 3 };
    enum Mode { DEMO_PFALL_e = 0x11, DEMO_UNK_030_e = 0x1E };
    be<u16> mDemoType;
    be<s16> mDemoMoveAngle, mTimer, mParam2;
    be<s32> mParam0, mParam1;
    be<u32> mDemoMode;
    be<f32> mStick;
};
WWHD_SIZE(Demo, 0x18);
struct Player : fopAc_ac_c {
    enum Flag { daPyFlg0_NO_DRAW = 0x08000000 };
    u8 mUnaccessed3AC[0x3B8 - 0x3AC];
    be<u32> mNoResetFlg0;
    u8 mUnaccessed3BC[0x420 - 0x3BC];
    Demo mDemo;
};
WWHD_OFFSET(Player, mNoResetFlg0, 0x3B8);
WWHD_OFFSET(Player, mDemo, 0x420);
struct PlayerVtable {
    u8 mUnaccessed[0x3C];
    be<u32> checkPlayerGuard;
    u8 mUnaccessed40[0xBC - 0x40];
    be<u32> getGrabActorID;
    u8 mUnaccessedC0[0xE4 - 0xC0];
    be<u32> voiceStart;
    u8 mUnaccessedE8[4];
    be<u32> setOutPower;
    u8 mUnaccessedF0[0x11C - 0xF0];
    be<u32> setPlayerPosAndAngle;
    u8 mUnaccessed120[0x12C - 0x120];
    be<u32> setThrowDamage;
};
WWHD_OFFSET(PlayerVtable, checkPlayerGuard, 0x3C);
WWHD_OFFSET(PlayerVtable, getGrabActorID, 0xBC);
WWHD_OFFSET(PlayerVtable, voiceStart, 0xE4);
WWHD_OFFSET(PlayerVtable, setOutPower, 0xEC);
WWHD_OFFSET(PlayerVtable, setPlayerPosAndAngle, 0x11C);
WWHD_OFFSET(PlayerVtable, setThrowDamage, 0x12C);
inline Player* player(fopAc_ac_c* actor) { return static_cast<Player*>(actor); }
inline PlayerVtable* vtable(fopAc_ac_c* actor) { return gabi::at<PlayerVtable>(actor->__vtbl); }
struct Tsubo : fopAc_ac_c {
    u8 mUnaccessed[0x92C - 0x3AC];
    be<u8> m810;
};
WWHD_OFFSET(Tsubo, m810, 0x92C);
}
inline void daPy_setOutPower(fopAc_ac_c* player, f32 power, s16 angle, int arg) { gabi::call_ptr(fm_c::vtable(player)->setOutPower, player, power, angle, arg); }
inline u32 daPy_checkPlayerGuard(fopAc_ac_c* player) { return gabi::call_ptr<u32>(fm_c::vtable(player)->checkPlayerGuard, player); }
inline u32 daPy_getGrabActorID(fopAc_ac_c* player) { return gabi::call_ptr<u32>(fm_c::vtable(player)->getGrabActorID, player); }
inline void daPy_voiceStart(fopAc_ac_c* player, u32 id) { gabi::call_ptr(fm_c::vtable(player)->voiceStart, player, id); }
inline void daPy_setPlayerPosAndAngle(fopAc_ac_c* player, cXyz* pos, csXyz* angle) { gabi::call_ptr(fm_c::vtable(player)->setPlayerPosAndAngle, player, pos, angle); }
inline void daPy_setThrowDamage(fopAc_ac_c* player, void* actor, s16 angle, f32 speedF, f32 speedY, int arg) { gabi::call_ptr(fm_c::vtable(player)->setThrowDamage, player, actor, angle, speedF, speedY, arg); }
inline void daPy_onPlayerNoDraw(fopAc_ac_c* player) { fm_c::player(player)->mNoResetFlg0 |= fm_c::Player::daPyFlg0_NO_DRAW; }
inline void daPy_offPlayerNoDraw(fopAc_ac_c* player) { fm_c::player(player)->mNoResetFlg0 &= ~u32(fm_c::Player::daPyFlg0_NO_DRAW); }
// HD: this callback resides in a different Floormaster part.
constexpr u32 FM_SEARCHNEARFM_CB = 0x02141064;
void fm_setAnm(daFm_c* t, s8 idx, bool force);
void fm_modeProc(daFm_c* t, int proc, int mode);
u32 fm_isLink(daFm_c* t, fopAc_ac_c* a);
u32 fm_isNpc(daFm_c* t, fopAc_ac_c* a);
void fm_resetInvKine(daFm_c* t);
void fm_calcInvKine(daFm_c* t, fopAc_ac_c* a);
void fm_cancelGrab(daFm_c* t);
void fm_searchTarget(daFm_c* t);
u32 fm_checkTgHit(daFm_c* t);
u32 fm_setHoleScale(daFm_c* t, f32 a, f32 b, f32 c);
void enemy_piyo_set(fopAc_ac_c* a);
void fm_setGrabPos(daFm_c* t);
u32 fm_checkPlayerGrabBomb(daFm_c* t);
u32 fm_isGrab(daFm_c* t);
u32 fm_isGrabFoot(daFm_c* t);
u32 fm_isLinkControl(daFm_c* t);
void fm_moveRndBack(daFm_c* t);
void dLib_setNextStageBySclsNum(u8 no, s8 roomNo);
void fopAcM_orderChangeEvent(fopAc_ac_c* a, const char* name, u16 flag, u16 hind);
void daPy_npc_setPointRestart(fopAc_ac_c* npc, s16 point, s8 opt);
void daNpc_Md_changeCaught02(fopAc_ac_c* md);
void cLib_chasePos(cXyz* p, const cXyz* t, f32 step);
void fm_setCarryNow(fopAc_ac_c* a, int stageLayer);
u32 daObj_PrmAbstract(fopAc_ac_c* a, u32 w, u32 sh);
s32 fm_calcTimer(be<s32>* timer);
void fm_addCalcPosXZ2(cXyz* pos, const cXyz* target, f32 scale, f32 maxStep);
void fm_monsSound(u32 id, cXyz* pos, u32 procId, u32 param, s32 reverb);
BOOL fm_endCheckOld(void* manager, const char* name);
const char* fm_getMyNowCutName(void* manager, s32 staffId);
inline void fm_modeProcInit(daFm_c* actor, int mode) { fm_modeProc(actor, daFm_c::PROC_INIT_e, mode); }
inline void fm_seStart(fopAc_ac_c* actor, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
    mDoAud_seStart(id, &actor->eyePos, param, reverb);
}
inline void fm_monsSeStart(fopAc_ac_c* actor, u32 id, u32 param) {
    s32 reverb = dComIfGp_getReverb(actor->current.roomNo);
    fm_monsSound(id, &actor->eyePos, actor->mBsPcId, param, reverb);
}
enum { fpcNm_NPC_CB1_e = 0x14E, fpcNm_NPC_MD_e = 0x16F };

enum FmCSound {
    JA_SE_CM_FM_PULL_IN = 0x58B0,
    JA_SE_CV_MD_DAMAGE = 0x48AA,
    JA_SE_CV_CB_DAMAGE = 0x48BF,
    JA_SE_CM_FM_GRAB_FOOT = 0x58AF,
    JA_SE_CM_MD_PIYO = 0x50BC,
};

void fm_setNextStage(u32 stage, s16 point, s8 roomNo, s8 layer, f32 lastSpeed, u32 lastMode, s32 setPoint, s8 wipe);

void fopAcM_orderOtherEvent2(fopAc_ac_c* actor, const char* name, u16 flag, u16 hind);

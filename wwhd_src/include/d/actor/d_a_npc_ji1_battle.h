/* Orca sword training: source-private HD adapters. */
#pragma once
// HD: keep unused legacy event inlines private while using the completed shared
// API.
#define dComIfGs_isEventBit ji1BattleLegacyIsEventBitHD
#define dComIfGs_onEventBit ji1BattleLegacyOnEventBitHD
#define dComIfGs_getEventReg ji1BattleLegacyGetEventRegHD
#define dComIfGs_setEventReg ji1BattleLegacySetEventRegHD
#include "d/actor/d_a_npc_ji1.h"
#undef dComIfGs_isEventBit
#undef dComIfGs_onEventBit
#undef dComIfGs_getEventReg
#undef dComIfGs_setEventReg
#include "gc_api/SSystem/c_lib.h"
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
// HD: a private view avoids importing unrelated legacy cylinder declarations.
struct Ji1BattleCapsuleHD {
  cXyz mStart;
  cXyz mEnd;
  be<u32> __vtbl;
  be<f32> mRadius;
};
WWHD_OFFSET(Ji1BattleCapsuleHD, mRadius, 0x1C);
WWHD_SIZE(Ji1BattleCapsuleHD, 0x20);

void ji1BattleAddCalcPosXZ2HD(cXyz *, const cXyz *, f32, f32);
void ji1BattleAddCalcPosXZHD(cXyz *, const cXyz *, f32, f32, f32);
s32 ji1BattleCalcTimerHD(be<s32> *);
void ji1BattleSetStartEndHD(Ji1BattleCapsuleHD *, const cXyz *, const cXyz *);
bool ji1BattleNormalizeHD(cXyz *);
void ji1BattleCreateTimerHD(s32, u16, u8, u8, f32, f32, f32, f32);

enum Ji1BattleAnimation {
  Ji1Battle_BCK_KAMAE = 5,
  Ji1Battle_BCK_KROT = 6,
  Ji1Battle_BCK_JPGUARD = 10,
  Ji1Battle_BCK_TATEATTACK = 19,
  Ji1Battle_BCK_YOKOATTACK = 20,
};
enum Ji1BattleCutType { Ji1Battle_CUT_TYPE_BT_JUMPCUT = 5 };
enum Ji1BattleTimerMode { Ji1Battle_TIMER_SWORD_TRAINING = 6 };
inline Ji1BattleCapsuleHD *ji1BattleCapsuleShapeHD(dCcD_Cps *capsule) {
  return reinterpret_cast<Ji1BattleCapsuleHD *>(capsule->mCps);
}

void ji1BattleVectorSubtractHD(const cXyz *, const cXyz *, cXyz *);

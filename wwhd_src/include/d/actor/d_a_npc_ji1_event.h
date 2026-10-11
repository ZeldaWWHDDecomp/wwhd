/* Orca event cuts: source-private HD views. */
#pragma once
// HD: keep the common header's unused legacy event helpers private.
#define dComIfGs_isEventBit ji1EventLegacyIsEventBitHD
#define dComIfGs_onEventBit ji1EventLegacyOnEventBitHD
#define dComIfGs_getEventReg ji1EventLegacyGetEventRegHD
#define dComIfGs_setEventReg ji1EventLegacySetEventRegHD
#include "d/actor/d_a_npc_ji1.h"
#undef dComIfGs_isEventBit
#undef dComIfGs_onEventBit
#undef dComIfGs_getEventReg
#undef dComIfGs_setEventReg
#include "gc_api/d/d_com_inf_game.h"
#include "gc_api/f_op/f_op_actor.h"
using Ji1EventProcHD = ProcFunc_l;
using Ji1EventWordHD = be<u32>;
using Ji1EventLifeHD = be<u8>;
struct Ji1EventMessageManagerHD {};
using Ji1EventMessagePointerHD = gptr<Ji1EventMessageManagerHD>;
using Ji1EventAngleHD = be<s16>;
struct Ji1EventActionNamesHD {
  gptr<char> mNames[7];
};
struct Ji1EventCutNamesHD {
  gptr<char> mNames[13];
};
struct Ji1EventEmitterHD {
  u8 _0[0x34];
  be<f32> mRate;
  u8 _38[0x58 - 0x38];
  be<f32> mSpread;
  be<s32> mMaxFrame;
  u8 _60[0x74 - 0x60];
  be<f32> mInitialVelRndm;
  u8 _78[4];
  be<f32> mVolumeSweep;
  u8 _80[0x238 - 0x80];
  cXyz mGlobalPtclScl;
};
WWHD_OFFSET(Ji1EventEmitterHD, mGlobalPtclScl, 0x238);
WWHD_OFFSET(Ji1EventEmitterHD, mVolumeSweep, 0x7C);
// daTsubo::Act_c fields keep their GameCube names with the HD +0x11C shift.
struct Ji1EventTsuboHD : fopAc_ac_c {
  u8 _3AC[0x794 - 0x3AC];
  be<s32> m678;
  u8 _798[0x7A4 - 0x798];
  be<s16> m688, m68A, m68C, m68E, m690, m692;
  u8 _7B0[0x7D8 - 0x7B0];
  Mtx34 mPoseMtx;
};
WWHD_OFFSET(Ji1EventTsuboHD, m678, 0x794);
WWHD_OFFSET(Ji1EventTsuboHD, mPoseMtx, 0x7D8);
struct Ji1EventPlayerHD : fopAc_ac_c {
  u8 _3AC[0x3B8 - 0x3AC];
  be<u32> mNoResetFlg0;
};
WWHD_OFFSET(Ji1EventPlayerHD, mNoResetFlg0, 0x3B8);

u32 ji1EventMessageSetHD(Ji1EventMessageManagerHD *manager, u32 number,
                         cXyz *pos);
void ji1EventDemoFlagOnHD();
u32 ji1EventMessageStatusHD(Ji1EventMessageManagerHD *manager);
void ji1EventSetMessageStatusHD(Ji1EventMessageManagerHD *manager, u32 status);
u32 ji1EventNextStatusHD(daNpc_Ji1_c *actor, be<u32> *number);
void ji1EventBgmStopHD(u32 frames);
void ji1EventBgmStartHD(u32 id);
u32 ji1EventPresentItemHD(cXyz *pos, s32 item, u8 flag, s32 bit, s32 room,
                          csXyz *angle, cXyz *scale);
void ji1EventSwordFlushHD(cXyz *pos, s32 parameter);
void ji1EventSwingHorizontalHD(void *head, s32 a, s32 b, s32 c, s32 d);
void ji1EventSwingVerticalHD(void *head, s32 a, s32 b, s32 c, s32 d);
void ji1EventAngleCopyHD(be<s16> *angle, be<s16> *source);
void ji1EventAngleSetHD(be<s16> *angle, s16 value);
void ji1EventIdentityHD(Mtx34 *matrix);
csXyz *ji1EventConstructAnglesHD(csXyz *angles, s16 x, s16 y, s16 z);
void ji1EventCutEndHD(dEvent_manager_c *manager, s32 staff);
BOOL ji1EventIsAdvanceHD(dEvent_manager_c *manager, s32 staff);

struct Ji1EventHitParticleArgsHD {
  csXyz angle;
  u8 _6[2];
  cXyz scale;
};
WWHD_OFFSET(Ji1EventHitParticleArgsHD, scale, 8);
WWHD_SIZE(Ji1EventHitParticleArgsHD, 20);

enum Ji1EventMessageStatus {
  fopMsgStts_BOX_OPENING_e = 2,
  fopMsgStts_MSG_TYPING_e = 6,
  fopMsgStts_MSG_DISPLAYED_e = 14,
  fopMsgStts_MSG_CONTINUES_e = 15,
  fopMsgStts_MSG_ENDS_e = 16,
  fopMsgStts_BOX_CLOSED_e = 18,
  fopMsgStts_MSG_DESTROYED_e = 19,
};
enum Ji1EventItem {
  dItemNo_SWORD_e = 0x38,
  dItemNo_HURRICANE_SPIN_e = 0xAA,
  dItemNo_PURPLE_RUPEE_e = 5,
  dItemNo_ORANGE_RUPEE_e = 6,
  dItemNo_SILVER_RUPEE_e = 15,
  dItemNo_HEART_PIECE_e = 7,
};
enum Ji1EventParticle {
  ID_AK_JN_OK = 0xD,
  ID_AK_JN_CRITICALHITFLASH = 0x10,
  ID_AK_JT_ELEMENTSMOKE00 = 0x2022,
};
enum Ji1EventAnimation {
  Ji1Event_BCK_WAIT01 = 0,
  Ji1Event_BCK_WAIT02 = 1,
  Ji1Event_BCK_KAMAE = 5,
  Ji1Event_BCK_DAMAGE = 25
};
enum Ji1EventHarpoonState { Ji1Event_HARPOON_HIDDEN, Ji1Event_HARPOON_HELD };

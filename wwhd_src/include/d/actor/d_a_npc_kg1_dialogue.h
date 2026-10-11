#pragma once
#include "d/actor/d_a_npc_kg1_anim.h"

struct Kg1Play {
    u8 mPrefix[0x51D0];
    dEvt_control_c mEvtCtrl;
    u8 mEvtManager[1];
};
WWHD_OFFSET(Kg1Play, mEvtManager, 0x52C4);
struct Kg1Save {
    u8 mPrefix[0x20];
    dSv_info_c mSave;
};
struct Kg1SavePointer {
    gptr<Kg1Save> info;
};
struct Kg1Board {
    u8 mPrefix[0x6E3];
    be<u8> mbForceEnd;
};
// HD: external static callback retains its guest address until autoglue supports its registration.
constexpr u32 fpcSch_JudgeForPName = 0x025E121C;
enum { fpcNm_MGBOARD_e = 0x3E, KG1_START_GAME_SOUND = 0x8A8 }; // HD process number.
Kg1Play *Kg1_getPlay();
s32 Kg1_getMyStaffId(void *, const char *, u32, u32);
Kg1Board *Kg1_searchBoard(u32, be<s16> *);
void daMgBoard_clrGInfoDraw(Kg1Board *);
void daMgBoard_reqStartGame(Kg1Board *);
void daMgBoard_setGInfoDraw(Kg1Board *);
u32 daMgBoard_checkEndGame(Kg1Board *);
u32 daMgBoard_checkClearGame(Kg1Board *);
u32 daMgBoard_getScore(Kg1Board *);
void daNpc_Kg1_clr_seq_flag(daNpc_Kg1_c *);
void daNpc_Kg1_kg1_talk_camera(daNpc_Kg1_c *);
s32 Kg1_talk(daNpc_Kg1_c *, s32);
u32 Kg1_calcTimer(be<u8> *);
u32 Kg1_rewardMessageReady(u32);
void Kg1_onEventBit(dSv_event_c *, u16);
u32 Kg1_getEventReg(dSv_event_c *, u16);
u32 Kg1_createItemForPresentDemo(cXyz *, s32, u32, s32, s32, void *, void *);
void Kg1_cutEnd(void *, s32);
u32 Kg1_endCheck(void *, s16);

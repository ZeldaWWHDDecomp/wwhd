#pragma once
#include "gc_api/gc_api.h"
#include "gc_api/d/d_com_inf_game.h"
#include "d/actor/d_a_player.h"

struct TagMkMessageManager;
struct TagMkHeaderZeros {
    be<u32> word0;
    be<u32> word1;
    be<u32> word2;
    be<u32> word3;
};
WWHD_OFFSET(TagMkHeaderZeros, word0, 0);
WWHD_OFFSET(TagMkHeaderZeros, word1, 4);
WWHD_OFFSET(TagMkHeaderZeros, word2, 8);
WWHD_OFFSET(TagMkHeaderZeros, word3, 12);
WWHD_SIZE(TagMkHeaderZeros, 0x10);

// HD: profile allocation is 0x3D8 bytes. Actor-owned fields retain the
// GameCube ordering and tww field names, displaced by the HD actor base (+0x11C).
struct daTag_Mk_c : fopAc_ac_c {
    enum Act_e { ACT_WAIT_e, ACT_ARRIVAL_e, ACT_HUNT_e, ACT_READY_e, ACT_EVENT_e, ACT_VILLA_e, ACT_VILLATALK_e, ACT_TAGWP_e, ACT_TAGWP2_e, ACT_DAICHI_e };
    BOOL actionArrival();
    BOOL actionHunt();
    s32 getNowEventAction();
    void demoInitWait();
    void demoInitMake();
    void demoInitDelete();
    void demoInitSetgoal();
    BOOL demoProcWait();
    BOOL demoProcMake();
    void demoProc();
    BOOL actionReady();
    BOOL actionEvent();
    void talkInit();
    BOOL checkArea(f32 width, f32 depth, f32 radius);
    BOOL actionVilla();
    u32 getMsg();
    u16 next_msgStatus(be<u32> *message);
    u16 talk();
    BOOL actionVillaTalk();
    BOOL actionTagWp();
    BOOL actionTagWp2();
    u8 getSwbit();
    u8 getSwbit2();
    BOOL actionDaichi();
    BOOL execute();
    u8 getType();
    void setTagWpEvId();
    /* 0x3AC */ be<u32> m290;
    /* 0x3B0 */ be<s8> m294;
    u8 pad3B1;
    /* 0x3B2 */ be<s16> m296;
    u8 pad3B4[4];
    /* 0x3B8 */ cXyz m29C;
    /* 0x3C4 */ be<u8> mActionTypes;
    u8 pad3C5;
    /* 0x3C6 */ be<s16> mEventIdx;
    /* 0x3C8 */ be<s32> mStaffIdx;
    /* 0x3CC */ be<s16> mCutEndTimer;
    u8 pad3CE[2];
    /* 0x3D0 */ be<u32> m2B4;
    /* 0x3D4 */ gptr<const char> m2B8;
};
static_assert(sizeof(daTag_Mk_c) == 0x3D8);
static_assert(offsetof(daTag_Mk_c, m290) == 0x3AC);
static_assert(offsetof(daTag_Mk_c, m294) == 0x3B0);
static_assert(offsetof(daTag_Mk_c, m296) == 0x3B2);
static_assert(offsetof(daTag_Mk_c, m29C) == 0x3B8);
static_assert(offsetof(daTag_Mk_c, mActionTypes) == 0x3C4);
static_assert(offsetof(daTag_Mk_c, mEventIdx) == 0x3C6);
static_assert(offsetof(daTag_Mk_c, mStaffIdx) == 0x3C8);
static_assert(offsetof(daTag_Mk_c, mCutEndTimer) == 0x3CC);
static_assert(offsetof(daTag_Mk_c, m2B4) == 0x3D0);
static_assert(offsetof(daTag_Mk_c, m2B8) == 0x3D4);

void orderOtherEventRegister(daTag_Mk_c*, s32, s32, s32, s32, s32);
fopAc_ac_c* fopAcM_createItemForKP2(cXyz*, s32, s32, csXyz*, cXyz*, f32, f32, f32, s32);
u32 setMessage(TagMkMessageManager*, u32, cXyz*);
u32 getMessageStatus(TagMkMessageManager*);
void setMessageStatus(TagMkMessageManager*, u32);
s32 getTriforceNum(dSv_player_collect_c*);
s32 getEventIndexRegister(dEvent_manager_c*, const char*, s32);
void subtractPosition(const cXyz*, cXyz*, const cXyz*);
void normalizePosition(cXyz*, cXyz*);
void initializeHeaderByte(be<u8>*);
void initializeHeaderFlag(be<u8>*);
void mDoAud_bgmAllMute(s32);
void mDoAud_subBgmStart(u32);
void mDoAud_subBgmStop();

// Item enum missing from the shared subset.
enum { dItemNo_JOY_PENDANT_e = 0x1F };

struct TagMkSaveRoot { u8 mPrefix[0x20]; dSv_info_c mInfo; };
WWHD_OFFSET(TagMkSaveRoot, mInfo, 0x20);

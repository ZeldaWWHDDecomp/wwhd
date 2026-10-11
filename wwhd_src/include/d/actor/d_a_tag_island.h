#pragma once
#include "gc_api/gc_api.h"
#include "d/d_path.h"
// HD: tww members displaced by the larger actor base (+0x11C).
struct daTag_Island_c : fopAc_ac_c {
  enum { daTagIsl_UNK_01 = 1, daTagIsl_UNK_02 = 2, daTagIsl_UNK_04 = 4 };
  u8 getType();
  u16 getArrivalFlag();
  BOOL arrivalTerms();
  u8 getSwbit();
  BOOL otherCheck();
  BOOL checkArea();
  u8 getEventNo();
  void makeEvId();
  BOOL actionHunt();
  BOOL actionArrival();
  void demoInitProc();
  void demoProcCom_ikada();
  BOOL actionEvent();
  BOOL actionReady();
  s32 getNowEventAction();
  void demoInitCom();
  void talkInit();
  void demoInitSpeak();
  void demoInitWait();
  void demoInitTact_Bf();
  void demoInitTact_Af();
  u32 getMsg();
  s32 next_msgStatus(be<u32> *message);
  u16 talk();
  BOOL demoProcSpeak();
  BOOL demoProcWait();
  BOOL demoProcTact_Bf();
  BOOL demoProcTact_Af();
  BOOL demoProc();
  BOOL actionTact();
  BOOL execute();
  s32 create();
  be<u16> m290, m292;
  /* 0x3B0 */ be<u8> mAction;
  u8 pad3B1;
  /* 0x3B2 */ be<u16> mFlag;
  /* 0x3B4 */ be<u32> mMsg;
  /* 0x3B8 */ be<s8> mTalkState;
  u8 pad3B9[3];
  /* 0x3BC */ be<u8> m2A0;
  u8 pad3BD;
  /* 0x3BE */ be<s16> mTimer;
  /* 0x3C0 */ be<s32> mStaffId;
  /* 0x3C4 */ be<s16> mEventId;
  u8 pad3C6[2];
  /* 0x3C8 */ be<u32> m2AC;
};
WWHD_OFFSET(daTag_Island_c, m290, 0x3AC);
WWHD_OFFSET(daTag_Island_c, mAction, 0x3B0);
WWHD_OFFSET(daTag_Island_c, mFlag, 0x3B2);
WWHD_OFFSET(daTag_Island_c, mMsg, 0x3B4);
WWHD_OFFSET(daTag_Island_c, mTalkState, 0x3B8);
WWHD_OFFSET(daTag_Island_c, m2A0, 0x3BC);
WWHD_OFFSET(daTag_Island_c, mTimer, 0x3BE);
WWHD_OFFSET(daTag_Island_c, mStaffId, 0x3C0);
WWHD_OFFSET(daTag_Island_c, mEventId, 0x3C4);
WWHD_OFFSET(daTag_Island_c, m2AC, 0x3C8);
static_assert(sizeof(daTag_Island_c) == 0x3CC);

// HD: unit-local views of the message-manager and raft fields used by this tag.
struct IslandMessageManager { u8 mUnused[0x921]; be<u8> mSendOn; };
WWHD_OFFSET(IslandMessageManager, mSendOn, 0x921);
WWHD_SIZE(IslandMessageManager, 0x922);
struct IslandRaft : fopAc_ac_c {
  u8 mUnused3AC[0x3C4-0x3AC];
  cXyz mCurPathP0;
  be<s8> mCurPathPoint;
  u8 mUnused3D1[3];
  gptr<path_layout::dPath> mpPath;
  u8 mUnused3D8[4];
  cXyz mCurPathP1;
  cXyz m02CC;
  be<f32> mVelocityFwdTarget;
  cXyz mPathPosTarget;
  u8 mUnused404[0x1688-0x404];
  cXyz mInitPos;
};
WWHD_OFFSET(IslandRaft, mCurPathP0, 0x3C4);
WWHD_OFFSET(IslandRaft, mCurPathPoint, 0x3D0);
WWHD_OFFSET(IslandRaft, mpPath, 0x3D4);
WWHD_OFFSET(IslandRaft, mCurPathP1, 0x3DC);
WWHD_OFFSET(IslandRaft, m02CC, 0x3E8);
WWHD_OFFSET(IslandRaft, mPathPosTarget, 0x3F8);
WWHD_OFFSET(IslandRaft, mInitPos, 0x1688);
WWHD_SIZE(IslandRaft, 0x1694);
// HD: these accesses copy IEEE words without quieting signaling NaNs.
inline be<u32>& coordinateWord(be<f32>& f) { return reinterpret_cast<be<u32>&>(f); }
struct IslandPlayerVtable { u8 mUnused[0x34]; gfn getTactTimerCancel; };
WWHD_OFFSET(IslandPlayerVtable, getTactTimerCancel, 0x34);
WWHD_SIZE(IslandPlayerVtable, 0x38);
struct IslandPlayer : fopAc_ac_c {
  s32 getTactTimerCancel() {
    // HD: virtual dispatch through the original player vtable slot.
    auto* vt = (IslandPlayerVtable*)(uintptr_t)u32(__vtbl);
    return gabi::call_ptr<s32>(vt->getTactTimerCancel.get(), this);
  }
};
struct IslandHeaderZeros { be<u32> word0, word1, word2, word3; };
WWHD_SIZE(IslandHeaderZeros, 0x10);

BOOL dKy_checkEventNightStop();
s16 eventIndex(dEvent_manager_c*, const char*, s32);
BOOL eventEndCheck(dEvent_manager_c*, s16);
BOOL eventAdvance(dEvent_manager_c*, s32);
s32 eventAction(dEvent_manager_c*, s32, gptr<char>*, s32, s32, s32);
s32 eventStaff(dEvent_manager_c*, const char*, void*, s32);
IslandRaft* searchRaft(const char*, void*, void*);
void setRaftMtx(IslandRaft*);
void subtractPosition(const cXyz*, cXyz*, const cXyz*);
f32 sqrtfHD(f32);
u32 setMessage(IslandMessageManager*, u32, cXyz*);
u32 getMessageStatus(IslandMessageManager*);
void setMessageStatus(IslandMessageManager*, s32);
void initializeHeaderByte(be<u8>*);
void initializeHeaderFlag(be<u8>*);

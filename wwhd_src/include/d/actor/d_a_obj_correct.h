// HD: tww actor-tail fields move by +0x11C; correction target offsets use the shared Try view.
#pragma once
#include "f_op/f_op_actor.h"
namespace daObjCorrect {
// HD: actor profile numbers used by this unit.
enum { fpcNm_Obj_Try_e=0x1CA, fpcNm_Obj_Movebox_e=0x2B };
enum Type_e { UNK_4=4, UNK_5=5 };
struct Act_c : fopAc_ac_c {
  enum Mode_e { MODE_OFF, MODE_ON };
  enum Demo_e { DEMO_NON, DEMO_REQ, DEMO_RUN };
  s32 prm_get(s32 w, s32 s);
  void mode_on_init();
  void mode_off_init();
  void demo_non_init();
  s32 _create();
  u8 _execute();
  fopAc_ac_c * chk_try_actor1(fopAc_ac_c* other, s32 type, f32 radius);
  fopAc_ac_c * chk_try_actor2(fopAc_ac_c* other, s32 type, u8 enabled, f32 radius, f32 depress);
  void mode_off();
  void mode_on();
  void demo_req_init();
  void demo_non();
  void demo_run_init();
  void demo_req();
  void demo_run();
  be<s16> mSearchValid;
  u8 _3ae[2];
  be<s32> mMode, mDemo, mType;
  be<s16> mEventIdx;
  u8 _3be[2];
  be<f32> mRadiusSq;
};
WWHD_OFFSET(Act_c, mSearchValid, 0x3AC);
WWHD_OFFSET(Act_c, mType, 0x3B8);
WWHD_OFFSET(Act_c, mRadiusSq, 0x3C0);
WWHD_SIZE(Act_c, 0x3C4);
} // namespace daObjCorrect

struct CorrectAttr { gptr<const char> name; be<f32> mOnRadiusSq,mOffRadiusSq,mDepressionRadiusSq; };
struct CorrectAttributes { CorrectAttr mEntries[8]; };
struct CorrectSearchTable { gfn mEntries[8]; };
struct CorrectModeTable { gabi::ptmf<void (daObjCorrect::Act_c::*)()> mEntries[2]; };
struct CorrectDemoTable { gabi::ptmf<void (daObjCorrect::Act_c::*)()> mEntries[3]; };
struct CorrectTryTypes { be<s32> mEntries[6]; };
struct CorrectSaveData { u8 prefix[0x20]; u8 mSwitch; };
struct CorrectGameInfo { u8 prefix[0x52B8]; be<u16> mEventFlag; u8 gap[10]; u8 mEvtManager; u8 gap2[0x5B2C-0x52C5]; gptr<fopAc_ac_c> mpPlayer; };
struct CorrectPlayerVtable { u8 prefix[0xBC]; gfn mGetGrabActorID; };
struct CorrectHeaderStatics { be<f32> mMinAngle,mMaxAngle; be<u8> mHeaderByte,mHeaderFlag; u8 pad[2]; be<u32> mZero[4]; };
struct CorrectDestructorRecords { u8 mRecords[3][12]; };
CorrectGameInfo* correctGetGameInfo();
void correctConstructActor(daObjCorrect::Act_c*);
void correctRemoveDrawTag(void*);
s32 correctCheckSwitch(void*, s32, s8);
s32 correctGetEventIndex(void*, const char*, u8);
s32 correctIsActor(fopAc_ac_c*);
f32 correctSquareDistance(const cXyz*, const cXyz*);
void correctAssert(const char*, s32, const char*);
s32 correctIsCreating(u32);
void* correctSearch(u32, void*);
u32 correctEventExists(void*, s16);
void correctOrderEvent(daObjCorrect::Act_c*, s16, u8, u32, u32, u32);
s16 correctSearchAngleY(fopAc_ac_c*, daObjCorrect::Act_c*);
s32 correctEventEndCheck(void*, s16);
void correctDestroyActor(daObjCorrect::Act_c*, s32);
void correctDeleteObject(void*);

void correctOnSwitch(void*, s32, s8);
void correctOffSwitch(void*, s32, s8);
void correctRegisterGlobal(void*);
void correctInitializeHeaderByte(void*);
void correctInitializeHeaderFlag(void*);
WWHD_SIZE(CorrectAttr,16);
WWHD_SIZE(CorrectAttributes,128);
WWHD_SIZE(CorrectSearchTable,32);
WWHD_SIZE(CorrectModeTable,16);
WWHD_SIZE(CorrectDemoTable,24);
WWHD_SIZE(CorrectTryTypes,24);
WWHD_OFFSET(CorrectGameInfo,mEventFlag,0x52B8);
WWHD_OFFSET(CorrectGameInfo,mEvtManager,0x52C4);
WWHD_OFFSET(CorrectGameInfo,mpPlayer,0x5B2C);
WWHD_OFFSET(CorrectPlayerVtable,mGetGrabActorID,0xBC);
WWHD_SIZE(CorrectHeaderStatics,0x1C);
WWHD_SIZE(CorrectDestructorRecords,36);

WWHD_OFFSET(daObjCorrect::Act_c,mMode,0x3B0);
WWHD_OFFSET(daObjCorrect::Act_c,mDemo,0x3B4);
WWHD_OFFSET(daObjCorrect::Act_c,mEventIdx,0x3BC);
WWHD_SIZE(CorrectGameInfo,0x5B30);
WWHD_SIZE(CorrectPlayerVtable,0xC0);
WWHD_SIZE(CorrectSaveData,0x21);

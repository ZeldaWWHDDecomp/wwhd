/* WWHD door10 layouts. Derived game source: */
#pragma once
#include "bindings.h"
struct dDoor_info_door10_l : fopAc_ac_c {
  be<s8> mRoomNo2;
  be<u8> mFromRoomNo, mToRoomNo;
  u8 _3af;
  cXyz mAngleVec;
  be<u8> mFrontCheck, m2A1;
  be<s16> mEventIdx[12];
  be<u8> mToolId[12], m2C6, m2C7;
  be<s32> mStaffId;
  be<s8> mRoomNo;
  u8 _3e9[3];
  u8 getType() { return gabi::call<u8>(0x0252AA2C, this); }
  u8 getSwbit() { return gabi::call<u8>(0x0252AA14, this); }
  u8 getSwbit2() { return gabi::call<u8>(0x0252AA20, this); }
  u8 getArg1() { return gabi::call<u8>(0x0252AA58, this); }
  u8 getFRoomNo() { return gabi::call<u8>(0x0252A37C, this); }
  u8 getBRoomNo() { return gabi::call<u8>(0x0252A388, this); }
  void openInitCom(s32 x) { gabi::call(0x0252A3A0, this, x); }
  void closeEndCom() { gabi::call(0x0252A550, this); }
  void initOpenDemo(s32 x) { gabi::call(0x0252A2DC, this, x); }
  s32 getDemoAction() { return gabi::call<s32>(0x0252A684, this); }
  void setGoal() { gabi::call(0x0252A6D0, this); }
  BOOL checkArea(f32 a, f32 b, f32 c) {
    return gabi::call<BOOL>(0x0252AE04, this, a, b, c);
  }
  void initProc(s32 x) { gabi::call(0x0252A9E0, this, x); }
  BOOL drawCheck(s32 x) { return gabi::call<BOOL>(0x0252B0FC, this, x); }
  s32 checkExecute() { return gabi::call<s32>(0x0252AC98, this); }
  void startDemoProc() { gabi::call(0x0252AD50, this); }
};
struct dDoor_smoke_door10_l {
  u8 bytes[0x38];
  void smokeInit(void *p) { gabi::call(0x0252B1E8, this, p); }
  void smokeProc(void *p) { gabi::call(0x0252B2D8, this, p); }
  void smokeEnd() { gabi::call(0x0252B3CC, this); }
};
struct dDoor_key_door10_l {
  be<u8> mbEnabled;
  u8 _1[3];
  gptr<J3DModel> mpModel;
  mDoExt_bckAnm mBckAnim;
  request_of_phase_process_class mPhase;
  be<u8> m20, mbIsBossDoor;
  u8 _9e[2];
  BOOL keyCreate(s32 x) { return gabi::call<BOOL>(0x0252B834, this, x); }
  cPhs_State keyResLoad() { return gabi::call<cPhs_State>(0x0252B484, this); }
  void keyResDelete() { gabi::call(0x0252B494, this); }
  void keyInit(void *p) { gabi::call(0x0252B4A4, this, p); }
  BOOL keyProc() { return gabi::call<BOOL>(0x0252B5B4, this); }
  void calcMtx(void *p) { gabi::call(0x0252B854, this, p); }
  void draw(void *p) { gabi::call(0x0252B9D4, this, p); }
  void keyOn() { gabi::call(0x0252B848, this); }
  void keyOff() { gabi::call(0x0252B5A8, this); }
};
struct dDoor_stop_door10_l {
  gptr<J3DModel> mpModel;
  be<f32> mOffsY;
  be<u8> m8, mFrontCheck, mA, mB;
  void calcMtx(void *p) { gabi::call(0x0252BA9C, this, p); }
  void closeInit(void *p) { gabi::call(0x0252BB9C, this, p); }
  BOOL closeProc(void *p) { return gabi::call<BOOL>(0x0252BC3C, this, p); }
  void openInit(void *p) { gabi::call(0x0252BCF4, this, p); }
  BOOL openProc(void *p) { return gabi::call<BOOL>(0x0252BD8C, this, p); }
  BOOL create() { return gabi::call<BOOL>(0x0252BE48, this); }
};
struct dDoor_hkyo_door10_l {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mpModel;
  gptr<void> mpBrk;
  be<u8> mAnmIdx, mUse;
  u8 _12[2];
  bool chkUse() { return mUse != 0; }
  void onUse(u8 x) { mUse = x; }
  void offUse() { mUse = 0; }
  cPhs_State resLoad() { return gabi::call<cPhs_State>(0x0252C11C, this); }
  void resDelete() { gabi::call(0x0252C13C, this); }
  BOOL create() { return gabi::call<BOOL>(0x0252C154, this); }
  void init() { gabi::call(0x0252C274, this); }
  void calcMtx(void *p, f32 x) { gabi::call(0x0252C280, this, p, x); }
  void draw(void *p) { gabi::call(0x0252C3F8, this, p); }
  void proc(void *p) { gabi::call(0x0252C574, this, p); }
  BOOL chkFirst() { return gabi::call<BOOL>(0x0252C390, this); }
  void onFirst() { gabi::call(0x0252C748, this); }
  BOOL chkStart() { return gabi::call<BOOL>(0x0252C788, this); }
};
struct daDoor10_c : dDoor_info_door10_l {
  dDoor_smoke_door10_l m2D0;
  dDoor_key_door10_l mKeyLock;
  dDoor_stop_door10_l mStopBars;
  dDoor_hkyo_door10_l mHkyo;
  gptr<J3DModel> mpModel;
  gptr<dBgW> mpBgW;
  be<u8> m354;
  u8 _4ed;
  be<u16> m356;
  be<f32> m358;
  request_of_phase_process_class mPhase;
  be<s32> m364;
  bool checkFlag(u16 f) { return (m356 & f) != 0; }
  void onFlag(u16 f) { m356 = m356 | f; }
  void offFlag(u16 f) { m356 = m356 & ~f; }
  void setAction(u8 x) { m354 = x; }
  s32 chkMakeKey();
  void setKey();
  BOOL chkMakeStop();
  s32 chkStopF();
  s32 chkStopB();
  void setStop();
  BOOL chkStopOpen();
  void setStopDemo();
  BOOL chkStopClose();
  const char *getBdlName();
  const char *getDzbName();
  f32 getSize2X();
  BOOL CreateHeap();
  void setEventPrm();
  void openInit();
  BOOL openProc();
  void openEnd();
  void closeInit();
  BOOL closeProc();
  void closeEnd();
  void calcMtx();
  BOOL CreateInit();
  cPhs_State create();
  void demoProc();
  BOOL draw();
};
WWHD_OFFSET(daDoor10_c, m2D0, 0x3EC);
WWHD_OFFSET(daDoor10_c, mKeyLock, 0x424);
WWHD_OFFSET(daDoor10_c, mStopBars, 0x4C4);
WWHD_OFFSET(daDoor10_c, mpModel, 0x4E4);
WWHD_SIZE(daDoor10_c, 0x500);

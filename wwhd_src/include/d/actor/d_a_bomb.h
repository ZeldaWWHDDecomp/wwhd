/* WWHD bomb layouts. Derived game source: */
#pragma once
#include "bindings.h"
struct bomb_brk_l : mDoExt_baseAnm {
  u8 bytes[0x78 - 0x10];
  void entry(J3DModelData *data, f32 frame) {
    gabi::call(0x025E83FC, this, data, frame);
  }
};
struct daBomb_fuseSmokeEcallBack {
  be<u32> __vtbl;
  be<s16> field_0x04;
  u8 _6[2];
  gptr<cXyz> mpPos, field_0x0C, field_0x10;
  gptr<void> mpEmitter;
  void executeAfter(void *);
};
struct daBomb_fuseSparksEcallBack {
  be<u32> __vtbl;
  gptr<cXyz> mpPos;
  gptr<void> mpEmitter;
  void execute(void *);
};
struct bomb_ground_l {
  u8 bytes[0x54];
};
struct bomb_wind_l {
  u8 bytes[0x2C];
};
struct daBomb_c : fopAc_ac_c {
  be<u32> mFunc[2];
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mpModel;
  mDoExt_bckAnm mBck0, mBck1;
  bomb_brk_l mBrk0, mBrk1;
  be<s32> mType;
  u8 hdExtra[0xB0];
  dBgS_Acch mAcch;
  dBgS_AcchCir mCir;
  u8 mGndChk[0x54];
  be<f32> field_0x554, field_0x558, field_0x55C;
  be<u8> field_0x560, mbWaterIn, field_0x562;
  u8 _8e3;
  dCcD_Stts mStts;
  dCcD_Sph mSph;
  daBomb_fuseSmokeEcallBack mSmoke;
  daBomb_fuseSparksEcallBack mSparks;
  be<u8> field_0x6F0, field_0x6F1, mBombFire, field_0x6F3, field_0x6F4,
      field_0x6F5, field_0x6F6, field_0x6F7;
  be<s32> mInitialState;
  be<s16> mRestTime, field_0x6FE, mNoGravityTime, field_0x702;
  cXyz mFusePos, mFusePos2, mFusePos3;
  LIGHT_INFLUENCE mPntLight;
  bomb_wind_l mPntWind;
  be<u8> field_0x774;
  u8 _af9[3];
  be<f32> field_0x778;
  be<u8> field_0x77C, field_0x77D, field_0x77E, field_0x77F, field_0x780,
      field_0x781, field_0x782;
  u8 _b07;
  be<s32> field_0x784;
  cXyz mWindVec;
  u8 field_0x794[0x30];
  be<s32> mMassCounter, field_0x7C8;
  s32 prm_get_state() const { return gabi::call<s32>(0x020CB8F0, this); }
  bool chk_state(s32 state) const {
    return gabi::call<bool>(0x020CB92C, this, state);
  }
  BOOL procExplode_init();
  u32 checkExplodeCc();
  void se_cannon_fly_set();
  void draw_norm();
  BOOL draw();
  void draw_nut();
  void anm_play_nut();
  void water_tention();
  void waitState_bomtyu();
  bool procWait_init();
  void change_state(s32 state) { gabi::call(0x020CB978, this, state); }
  void setFuseEffect();
  bool checkExplodeCc_norm();
  bool checkExplodeCc_nut();
  bool checkExplodeCc_cannon();
  bool procCarry_init();
  void setRoomInfo();
  void bgCrrPos();
  void bgCrrPos_lava();
  void bgCrrPos_water();
  void bound(f32);
  bool checkExplodeBg_norm();
  bool checkExplodeBg_nut();
  bool checkExplodeBg_cannon();
  void eff_water_splash();
  void makeWaterEffect();
  cPhs_State create();
  void create_init();
  BOOL createHeap();
  void eff_explode_cheap(const csXyz *);
  void eff_explode_normal(const csXyz *);
  void eff_explode();
  BOOL execute();
  bool bombDelete();
  void set_wind_vec();
  void makeFireEffect(cXyz *pos, csXyz *angle);
  void set_mtx();
  u32 procWait();
  u32 procCarry();
  bool procExplode();
  void posMoveF();
  void set_real_shadow_flag();
  bool checkExplodeTimer();
  bool checkExplode();
  u8 chk_water_in();
  bool chk_water_sink();
  u8 chk_water_land();
  bool chk_lava_hit();
  bool chk_dead_zone();
  bool waitState_cannon();
  void se_cannon_fly_stop();
};
WWHD_OFFSET(daBomb_c, mpModel, 0x3BC);
WWHD_OFFSET(daBomb_c, mType, 0x5C8);
WWHD_OFFSET(daBomb_c, mAcch, 0x67C);
WWHD_OFFSET(daBomb_c, mStts, 0x8E4);
WWHD_OFFSET(daBomb_c, mRestTime, 0xA7C);
WWHD_OFFSET(daBomb_c, mPntLight, 0xAA8);
WWHD_SIZE(daBomb_c, 0xB50);

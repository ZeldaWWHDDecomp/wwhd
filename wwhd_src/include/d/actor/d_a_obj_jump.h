/* Wind Temple Iron Boots springboard. HD: MoveBg base is 0x3E0 and the
 * GameCube ground-check/shadow fields are absent; TWW spring state names
 * retained. */
#pragma once
#include "bindings.h"
namespace daObjJump {
struct Attr_c {
  be<u32> heapSize;
  be<s16> dzbResIndex, bdlResIndex;
  csXyz cullSizeBoxMin, cullSizeBoxMax;
  be<u8> hasShadow;
  u8 _15;
  be<u16> springJntNum;
  be<f32> field_0x018;
  be<f32> field_0x01C;
  be<f32> field_0x020;
  be<f32> field_0x024;
  be<f32> field_0x028;
  be<f32> field_0x02C;
  be<f32> field_0x030;
  be<f32> field_0x034;
  be<f32> field_0x038;
  be<f32> field_0x03C;
  be<u8> field_0x040;
  be<u8> field_0x041;
  be<u8> field_0x042;
  be<u8> field_0x043;
  be<u8> field_0x044;
  be<u8> field_0x045;
  be<u8> field_0x046;
  be<u8> field_0x047;
  be<u8> field_0x048;
  u8 _49[3];
  be<f32> field_0x04C;
};
WWHD_OFFSET(Attr_c, springJntNum, 0x16);
WWHD_OFFSET(Attr_c, field_0x018, 0x18);
WWHD_OFFSET(Attr_c, field_0x040, 0x40);
WWHD_OFFSET(Attr_c, field_0x04C, 0x4C);
WWHD_SIZE(Attr_c, 0x50);
struct Act_c : dBgS_MoveBgActor {
  enum Type_e { Type_SPRING_e, Type_SPRING_ON_BOX_e };
  enum Mode_e {
    Mode_WAIT_e,
    Mode_WAIT_LOWER_e,
    Mode_LOWER_e,
    Mode_LOWER_UPPER_e,
    Mode_UPPER_e,
    Mode_UPPER_WAIT_e
  };
  enum Prm_e { PRM_TYPE_W = 1, PRM_TYPE_S = 0 };
  const Attr_c &attr() const;
  /* 0x3E0 */ request_of_phase_process_class mPhase;
  /* 0x3E8 */ gptr<J3DModel> mModel;
  /* 0x3EC */ be<s32> mType;
  /* 0x3F0 */ be<f32> mGroundY;
  /* 0x3F4 */ be<s32> mMode;
  /* 0x3F8 */ be<s16> mTimer;
  /* 0x3FA */ be<u8> mIsSetup;
  /* 0x3FB */ u8 pad;
  /* 0x3FC */ be<f32> field_0x338, field_0x33C, field_0x340;
  /* 0x408 */ be<u8> mIsRide, field_0x345, field_0x346, field_0x347,
      field_0x348;
  /* 0x40D */ be<u8> mIsPlayerRide, field_0x34a, field_0x34b;
  /* 0x410 */ be<u8> mIsHeavyRide, field_0x34d, field_0x34e, field_0x34f,
      field_0x350;
  /* 0x415 */ u8 pad2;
  /* 0x416 */ be<s16> field_0x352;
  /* 0x418 */ be<u8> field_0x354;
  /* 0x419 */ u8 pad3[3];
  s32 Mthd_Create();
  s32 Mthd_Delete();
  s32 CreateHeap();
  s32 Create();
  s32 Delete();
  void set_mtx();
  void init_mtx();
  void set_push_flag();
  void clear_push_flag();
  void calc_vib_pos();
  void mode_wait_init();
  void mode_wait();
  void mode_w_l_init();
  void mode_w_l();
  void mode_lower_init();
  void mode_lower();
  void mode_l_u_init();
  void mode_l_u();
  void mode_upper_init();
  void mode_upper();
  void mode_u_w_init();
  void mode_u_w();
  s32 Execute(gptr<Mtx34> *);
  s32 Draw();
};
WWHD_OFFSET(Act_c, mPhase, 0x3E0);
WWHD_OFFSET(Act_c, mModel, 0x3E8);
WWHD_OFFSET(Act_c, mType, 0x3EC);
WWHD_OFFSET(Act_c, mGroundY, 0x3F0);
WWHD_OFFSET(Act_c, mMode, 0x3F4);
WWHD_OFFSET(Act_c, mTimer, 0x3F8);
WWHD_OFFSET(Act_c, mIsSetup, 0x3FA);
WWHD_OFFSET(Act_c, pad, 0x3FB);
WWHD_OFFSET(Act_c, field_0x338, 0x3FC);
WWHD_OFFSET(Act_c, mIsRide, 0x408);
WWHD_OFFSET(Act_c, mIsPlayerRide, 0x40D);
WWHD_OFFSET(Act_c, mIsHeavyRide, 0x410);
WWHD_OFFSET(Act_c, pad2, 0x415);
WWHD_OFFSET(Act_c, field_0x352, 0x416);
WWHD_OFFSET(Act_c, field_0x354, 0x418);
WWHD_OFFSET(Act_c, pad3, 0x419);
WWHD_SIZE(Act_c, 0x41C);
} // namespace daObjJump

// HD: private views of fields used by this actor; no alternate shared class
// definitions.
struct JumpPlayerHD : fopAc_ac_c {
  u8 _3AC[12];
  be<u32> mNoResetFlg0, mNoResetFlg1;
};
WWHD_OFFSET(JumpPlayerHD, mNoResetFlg0, 0x3B8);
WWHD_OFFSET(JumpPlayerHD, mNoResetFlg1, 0x3BC);
WWHD_SIZE(JumpPlayerHD, 0x3C0);
struct JumpPlayHD {
  u8 _0[0x5B2C];
  gptr<JumpPlayerHD> mPlayer;
  u8 _5B30[0x5CD8 - 0x5B30];
  be<u32> mPlayerStatus0;
  u8 _5CDC[0x5D70 - 0x5CDC];
  be<u32> mBackgroundOpaque, mBackgroundTranslucent, mOpaque, mTranslucent;
};
WWHD_OFFSET(JumpPlayHD, mPlayer, 0x5B2C);
WWHD_OFFSET(JumpPlayHD, mPlayerStatus0, 0x5CD8);
WWHD_OFFSET(JumpPlayHD, mBackgroundOpaque, 0x5D70);
WWHD_SIZE(JumpPlayHD, 0x5D80);
struct JumpDrawListsHD {
  be<u32> mOpaque, mTranslucent;
};
WWHD_SIZE(JumpDrawListsHD, 8);
struct JumpActorVtableHD {
  static const JumpActorVtableHD *get(const daObjJump::Act_c *actor) {
    return gabi::at<JumpActorVtableHD>(actor->__vtbl);
  }
  s32 draw(daObjJump::Act_c *actor) const {
    return gabi::call<s32>(mDraw, actor);
  }
  s32 isDelete(daObjJump::Act_c *actor) const {
    return gabi::call<s32>(mIsDelete, actor);
  }
  u8 _0[0x2C];
  be<u32> mDraw;
  u8 _30[12];
  be<u32> mIsDelete;
};
WWHD_OFFSET(JumpActorVtableHD, mIsDelete, 0x3C);
WWHD_SIZE(JumpActorVtableHD, 0x40);
struct JumpAttributesHD {
  daObjJump::Attr_c mEntries[2];
};
WWHD_SIZE(JumpAttributesHD, 0xA0);
struct JumpModeTableHD {
  gabi::ptmf<void (daObjJump::Act_c::*)()> mModes[6];
};
WWHD_SIZE(JumpModeTableHD, 48);
struct JumpHeaderStaticsHD {
  be<f32> mFirst, mSecond;
  be<u8> mTrigState, mRandomState;
  u8 _A[2];
  be<u32> mZero[4];
};
WWHD_OFFSET(JumpHeaderStaticsHD, mZero, 12);
WWHD_SIZE(JumpHeaderStaticsHD, 28);
struct JumpHeaderConstantsHD {
  be<f32> mFirst, mSecond;
};
WWHD_SIZE(JumpHeaderConstantsHD, 8);
struct JumpRegistrationHD {
  u8 mData[12];
};
WWHD_SIZE(JumpRegistrationHD, 12);
struct JumpRegistrationsHD {
  JumpRegistrationHD mEntries[3];
};
WWHD_SIZE(JumpRegistrationsHD, 36);

struct JumpBackgroundHD {
  u8 _0[0xB0];
  gfn mRideCallback;
  void setRideCallback(void (*callback)(void *, daObjJump::Act_c *,
                                        fopAc_ac_c *)) {
    mRideCallback.v = gabi::guest_fn((const void *)callback);
  }
};
WWHD_OFFSET(JumpBackgroundHD, mRideCallback, 0xB0);
WWHD_SIZE(JumpBackgroundHD, 0xB4);

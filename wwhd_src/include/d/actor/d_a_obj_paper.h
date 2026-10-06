#pragma once
#include "d/d_cc_d.h"
#include "f_op/f_op_actor.h"
namespace daObjPaper {
struct Act_c : fopAc_ac_c {
  u8 mPhase[8];
  gptr<u8> mpModel;
  u8 mCylinder[0x130];
  dCcD_Stts mColStatus;
  be<u8> mHasCollision;
  u8 _525[3];
  be<s32> mMode, mMessageId, mType;
  bool create_heap();
  void set_mtx();
  void init_mtx();
  void mode_wait_init();
  s32 create();
  BOOL remove();
  void damage_cc_proc();
  BOOL execute();
  BOOL draw();
  void mode_talk0_init();
  void mode_wait();
  void mode_talk1_init();
  void mode_talk0();
  void mode_talk2_init();
  void mode_talk1();
  void mode_talk2();
};
WWHD_OFFSET(Act_c, mCylinder, 0x3B8);
WWHD_OFFSET(Act_c, mColStatus, 0x4E8);
WWHD_OFFSET(Act_c, mType, 0x530);
WWHD_SIZE(Act_c, 0x534);
} // namespace daObjPaper

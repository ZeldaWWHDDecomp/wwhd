/** Tower of the Gods gate. HD layout; GC member layout is not yet recovered.
 * The splash callback retains the follow-callback base. */
#pragma once
#include "bindings.h"

enum { dRes_INDEX_HTETU1_BDL_HTETU1_e = 4, dRes_INDEX_HTETU1_DZB_HTETU1_e = 7 };
struct HtetuEmitterHD {
  u8 mReserved[0x244];
  be<u8> mRed, mGreen, mBlue;
  u8 mReservedColor[0x254 - 0x247];
  be<u32> mFlags;
};
struct HtetuLightBytesHD {
  u8 mReserved[0x91];
  be<u8> mRed;
  u8 mReservedRed;
  be<u8> mGreen;
  u8 mReservedGreen;
  be<u8> mBlue;
};
struct HtetuWaterBaseHD {
  u8 mData[0x10];
};
struct HtetuWaterCheckHD {
  u8 mReserved[0x10];
  HtetuWaterBaseHD mBase;
  be<u32> mCheckVtable, mPolyVtable;
  u8 mReservedCheck[8];
  be<u32> mObjectVtable;
  u8 mReservedObject[0x48 - 0x34];
  be<f32> mHeight;
  u8 mReservedHeight[4];
};
struct HtetuHeaderStaticsHD {
  be<f32> mMinAngle, mMaxAngle;
  be<u8> mRandomState, mMathState;
  u8 mReserved[2];
  be<u32> mObject[4];
};
struct HtetuRegistrationHD {
  u8 mData[12];
};
struct HtetuRegistrationTableHD {
  HtetuRegistrationHD mEntries[3];
};
struct HtetuAngleLimitsHD {
  be<f32> mMinAngle, mMaxAngle;
};
struct HtetuSinCosHD {
  be<f32> mSin, mCos;
};
struct HtetuSinTableHD {
  HtetuSinCosHD mEntries[8192];
};
struct HtetuParticleIdsHD {
  be<u16> mIds[2];
};
struct daObjHtetu1Splash_c : dPa_followEcallBack {
  /* 0x14 */ cXyz mPosition;
  /* 0x20 */ csXyz mRotation;
  /* 0x26 */ be<s16> mTimer;
  /* 0x28 */ be<u8> mPlaying;
  u8 mReserved[3];
  void create_s(u16 id, cXyz *pos, csXyz *rot, void *tev);
  void delete_s();
  void timer_play_particle(s16 timer);
  HtetuEmitterHD *getEmitterHD() {
    return reinterpret_cast<HtetuEmitterHD *>(mpEmitter.get());
  }
};
struct daObjHtetu1_c : fopAc_ac_c {
  /* 0x3AC */ gptr<J3DModel> mpModel;
  /* 0x3B0 */ request_of_phase_process_class mPhase;
  /* 0x3B8 */ be<u32> mSwitchBit;
  /* 0x3BC */ cXyz mMovingPosition, mShakeOffset;
  /* 0x3D4 */ be<f32> mFloorHeight, mShakeAmplitude;
  /* 0x3DC */ be<u16> mUnlockTimer;
  /* 0x3DE */ be<u8> mMotionState, mEventPadding;
  /* 0x3E0 */ be<s16> mEventIndex;
  /* 0x3E2 */ be<u8> mEventState;
  u8 mReservedEvent;
  /* 0x3E4 */ be<s16> mQuakeTimer;
  u8 mReservedQuake[2];
  /* 0x3E8 */ gptr<dBgW> mpBgW;
  /* 0x3EC */ daObjHtetu1Splash_c mSplash[2];
  // HD: retain native return-register widths where the GC declarations return
  // void/bool.
  BOOL create_heap();
  void init_mtx();
  s32 _create();
  BOOL _delete();
  void unlock();
  f32 get_water_h();
  void splash_manager();
  BOOL _execute();
  BOOL _draw();
  enum MotionState { MOTION_WAIT, MOTION_SHAKE, MOTION_LOWER };
  enum EventState { EVENT_WAIT, EVENT_OPEN, EVENT_DONE };
};
WWHD_OFFSET(HtetuEmitterHD, mFlags, 0x254);
WWHD_SIZE(HtetuEmitterHD, 0x258);
WWHD_OFFSET(HtetuLightBytesHD, mGreen, 0x93);
WWHD_SIZE(HtetuLightBytesHD, 0x96);
WWHD_OFFSET(HtetuWaterCheckHD, mCheckVtable, 0x20);
WWHD_OFFSET(HtetuWaterCheckHD, mObjectVtable, 0x30);
WWHD_OFFSET(HtetuWaterCheckHD, mHeight, 0x48);
WWHD_SIZE(HtetuWaterCheckHD, 0x50);
WWHD_SIZE(HtetuWaterBaseHD, 0x10);
WWHD_SIZE(HtetuHeaderStaticsHD, 0x1C);
WWHD_SIZE(HtetuRegistrationHD, 12);
WWHD_SIZE(HtetuRegistrationTableHD, 36);
WWHD_SIZE(HtetuAngleLimitsHD, 8);
WWHD_SIZE(HtetuSinCosHD, 8);
WWHD_SIZE(HtetuSinTableHD, 0x10000);
WWHD_SIZE(HtetuParticleIdsHD, 4);
WWHD_OFFSET(daObjHtetu1Splash_c, mPosition, 0x14);
WWHD_OFFSET(daObjHtetu1Splash_c, mTimer, 0x26);
WWHD_SIZE(daObjHtetu1Splash_c, 0x2C);
WWHD_OFFSET(daObjHtetu1_c, mpModel, 0x3AC);
WWHD_OFFSET(daObjHtetu1_c, mMovingPosition, 0x3BC);
WWHD_OFFSET(daObjHtetu1_c, mSplash, 0x3EC);
WWHD_OFFSET(daObjHtetu1_c, mpBgW, 0x3E8);
WWHD_SIZE(daObjHtetu1_c, 0x444);

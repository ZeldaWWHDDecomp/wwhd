#pragma once
#include "f_op/f_op_actor.h"
struct daTbox_c : fopAc_ac_c {
  be<s32> mRoomNo;
  request_of_phase_process_class mPhase;
  gptr<void> mpChestMdl;
  u8 mOpenAnm[0x8C];
  gptr<void> mpAppearTexAnm, mpAppearRegAnm;
  gptr<void> mpBgWClosed, mpBgWOpen, mpBgWVines, mpBgWCurrent, mpFlashMdl;
  u8 mFlashAnm[0x8C], mFlashTexAnm[0x74], mFlashRegAnm[0x78];
  be<u32> m05dc;
  u8 mBrkAnm3[0x78];
  gptr<void> mpTactPlatformMdl;
  u8 mTactPlatformBrk[0x78];
  be<s16> mActionAdjustment, mActionIndex;
  be<u32> mActionFunc;
  be<f32> mInvisibleScrollVal;
  be<s32> mStaffId;
  u8 mMtx[0x30], mPLight[0x24], mEfLight[0x24], mSmokeCB[0x20];
  gptr<void> mSmokeEmitter;
  be<f32> mAllColRatio, mAppearingYOffset;
  be<u16> mFlags, mOpenTimer;
  be<u8> mHasOpenAnmFinished, mIsFlashPlaying;
  be<u16> mAppearTimer;
  be<u8> mGenocideDelayTimer;
  u8 pad791[3], mObjAcch[0x1C4], mAcchCir[0x40], mColStatus[0x3C],
      mColCyl[0x130];
  be<u8> mTboxNo;
  u8 padB05[3];
};
WWHD_OFFSET(daTbox_c, mpChestMdl, 0x3B8);
WWHD_OFFSET(daTbox_c, mActionFunc, 0x6D8);
WWHD_OFFSET(daTbox_c, mFlags, 0x788);
WWHD_OFFSET(daTbox_c, mColCyl, 0x9D4);
WWHD_SIZE(daTbox_c, 0xB08);

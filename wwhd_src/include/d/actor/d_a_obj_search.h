#pragma once
#include "bindings.h"
namespace daObj_Search {
struct Act_c : fopAc_ac_c {
  be<s32> mMode;
  u8 mCollision[0x710 - 0x3B0];
  gptr<void> mpSearchModel;
  gptr<void> mpBeamModel[2];
  u8 mAnimation[0x780 - 0x71C];
  be<u8> mBkControl;
  u8 mPad781[3];
  gptr<void> mpBeamBackground[2];
  gptr<void> mpBaseBackground;
  Mtx34 mBeamMatrix[2];
  Mtx34 mBaseMatrix;
  u8 mState[0xA28 - 0x820];
};
WWHD_OFFSET(Act_c, mMode, 0x3AC);
WWHD_OFFSET(Act_c, mpSearchModel, 0x710);
WWHD_OFFSET(Act_c, mBkControl, 0x780);
WWHD_OFFSET(Act_c, mpBaseBackground, 0x78C);
WWHD_OFFSET(Act_c, mBaseMatrix, 0x7F0);
static_assert(sizeof(Act_c) == 0xA28);
struct Bgc_c {
  u8 mData[0x78];
};
static_assert(sizeof(Bgc_c) == 0x78);
} // namespace daObj_Search

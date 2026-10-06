#pragma once
#include "bindings.h"
namespace daStone {
// HD actor profile allocates 0x7AC bytes. The five resource records retain
// their GameCube 0xB4 stride; derived actor fields move by 0x11C.
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> mModel;
  dBgS_ObjAcch mAcch;
  dBgS_AcchCir mAcchCir;
  u8 mGndChkYogan[0x54];
  cXyz mSurfaceHeights; // lava, current water, previous water
  be<u8> mWaterHit, mSeaHit;
  u8 pad61E[2];
  u8 mStts[0x3C];
  dCcD_Cyl mCyl;
  be<s32> mType, mMode;
  be<u8> m678;
  be<s8> m679;
  be<u8> m67A, m67B;
  be<s16> m67C, m67E, m680, m682, m684, m686;
  be<f32> mHeight;
  be<u8> m68C, m68D;
  u8 pad7AA[2];
};
WWHD_OFFSET(Act_c, mPhase, 0x3AC);
WWHD_OFFSET(Act_c, mModel, 0x3B4);
WWHD_OFFSET(Act_c, mAcch, 0x3B8);
WWHD_OFFSET(Act_c, mAcchCir, 0x57C);
WWHD_OFFSET(Act_c, mGndChkYogan, 0x5BC);
WWHD_OFFSET(Act_c, mSurfaceHeights, 0x610);
WWHD_OFFSET(Act_c, mWaterHit, 0x61C);
WWHD_OFFSET(Act_c, mSeaHit, 0x61D);
WWHD_OFFSET(Act_c, mStts, 0x620);
WWHD_OFFSET(Act_c, mCyl, 0x65C);
WWHD_OFFSET(Act_c, mType, 0x78C);
WWHD_OFFSET(Act_c, mMode, 0x790);
WWHD_OFFSET(Act_c, mHeight, 0x7A4);
WWHD_SIZE(Act_c, 0x7AC);
} // namespace daStone

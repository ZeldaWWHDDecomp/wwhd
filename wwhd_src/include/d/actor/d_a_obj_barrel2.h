// WWHD barrel2 layout. Binary-derived,
#pragma once
#include "bindings.h"
namespace daObjBarrel2 {
struct Attr_c {
  be<u16> m00, m02;
  be<u32> m04;
  be<f32> m08;
  be<f32> m0C;
  be<f32> m10;
  be<f32> m14;
  be<f32> m18;
  be<f32> m1C;
  be<f32> m20;
  be<f32> m24;
  be<f32> m28;
  be<f32> m2C;
  be<f32> m30;
  be<f32> m34;
  be<f32> m38;
  be<f32> m3C;
  be<f32> m40;
  be<f32> m44;
  be<f32> m48;
  be<f32> m4C;
  be<f32> m50;
  be<f32> m54;
  be<s16> m58;
  u8 _5a[2];
  be<f32> m5C;
  be<f32> m60;
  be<f32> m64;
  be<f32> m68;
  be<f32> m6C;
  be<f32> m70;
};
struct Act_c : fopAc_ac_c {
  request_of_phase_process_class mPhase;
  gptr<J3DModel> m298;
  gptr<mDoExt_brkAnm> m29C;
  dCcD_Stts mStts;
  dCcD_Cyl mCyl;
  be<s32> m40C, m410;
  be<f32> m414, m418, m41C;
  cXyz m420;
  be<f32> m42C, m430, m434, m438;
  be<s16> m43C;
  u8 _55a[2];
  be<f32> m440, m444, m448, m44C, m450, m454;
  be<u32> mItemId;
  be<f32> m45C;
  be<u32> m460;
  be<s32> m464;
  be<u8> m468;
  u8 _585;
  be<s16> m46A;
  be<u8> m46C, m46D;
  u8 _58e[2];
  be<s32> m470;
  be<u8> m474, m475, m476;
  u8 _593;
  Mtx34 m478;
};
} // namespace daObjBarrel2
WWHD_OFFSET(daObjBarrel2::Act_c, m40C, 0x528);
WWHD_OFFSET(daObjBarrel2::Act_c, m474, 0x590);
WWHD_OFFSET(daObjBarrel2::Act_c, m478, 0x594);
WWHD_SIZE(daObjBarrel2::Act_c, 0x5C4);

#include "SSystem/SComponent/c_API_controller.h"
using gabi::fmadds;
using gabi::fmuls_ppc;
using gabi::fnmsubs;
using gabi::frsqrte;
using gabi::fsubs_ppc;
using gabi::ftoi;
// HD: only port zero exists; pad words and stick positions use the HD layout.
// HD: retain the two different stick magnitude algorithms and PPC intermediate rounding.
static cAPIController::PadPointer l_pad;
static f32 l_zero;
static f32 l_one;
static f32 l_positiveHalfPi;
static f32 l_negativeHalfPi;
static f32 l_angleScale;
static f32 l_half;
static f32 l_three;
static f32 l_negativePi;
static f32 l_pi;
static cAPIController::HeaderStatics l_headerStatics;
static cAPIController::StaticRecords l_records;
/* 020075C0 */
u32 cAPICPad_ANY_BUTTON(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonHold;
  return buttons;
}
/* 020075E0 */
u32 CPad_CHECK_HOLD_UP(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonHold;
  return (buttons & cAPIController::BUTTON_UP) != 0;
}
/* 0200760C */
u32 CPad_CHECK_HOLD_DOWN(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonHold;
  return (buttons & cAPIController::BUTTON_DOWN) != 0;
}
/* 02007638 */
u32 CPad_CHECK_HOLD_Z(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonHold;
  return (buttons & cAPIController::BUTTON_Z) != 0;
}
/* 02007664 */
u32 CPad_CHECK_HOLD_R(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonHold;
  return (buttons & cAPIController::BUTTON_R) != 0;
}
/* 02007690 */
u32 CPad_CHECK_HOLD_L(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonHold;
  return (buttons & cAPIController::BUTTON_L) != 0;
}
/* 020076BC */
u32 CPad_CHECK_HOLD_B(s32 port) {
  if (port)
    return 0;
  return l_pad.mpPad->mButtonHold & cAPIController::BUTTON_B;
}
/* 020076E0 */
u32 CPad_CHECK_HOLD_A(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonHold;
  return (buttons & cAPIController::BUTTON_A) != 0;
}
/* 0200770C */
u32 CPad_CHECK_HOLD_X(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonHold;
  return (buttons & cAPIController::BUTTON_X) != 0;
}
/* 02007738 */
u32 CPad_CHECK_HOLD_Y(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonHold;
  return (buttons & cAPIController::BUTTON_Y) != 0;
}
/* 02007764 */
u32 CPad_CHECK_TRIG_UP(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_UP) != 0;
}
/* 02007790 */
u32 CPad_CHECK_TRIG_DOWN(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_DOWN) != 0;
}
/* 020077BC */
u32 CPad_CHECK_TRIG_LEFT(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_LEFT) != 0;
}
/* 020077E8 */
u32 CPad_CHECK_TRIG_RIGHT(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_RIGHT) != 0;
}
/* 02007814 */
u32 CPad_CHECK_TRIG_Z(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_Z) != 0;
}
/* 02007840 */
u32 CPad_CHECK_TRIG_R(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_R) != 0;
}
/* 0200786C */
u32 CPad_CHECK_TRIG_L(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_L) != 0;
}
/* 02007898 */
u32 CPad_CHECK_TRIG_B(s32 port) {
  if (port)
    return 0;
  return l_pad.mpPad->mButtonTrig & cAPIController::BUTTON_B;
}
/* 020078BC */
u32 CPad_CHECK_TRIG_A(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_A) != 0;
}
/* 020078E8 */
u32 CPad_CHECK_TRIG_X(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_X) != 0;
}
/* 02007914 */
u32 CPad_CHECK_TRIG_Y(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_Y) != 0;
}
/* 02007940 */
u32 CPad_CHECK_TRIG_START(s32 port) {
  if (port)
    return 0;
  u32 buttons = l_pad.mpPad->mButtonTrig;
  return (buttons & cAPIController::BUTTON_START) != 0;
}
/* 0200796C */
f32 CPad_GET_STICK_POS_X(s32 port) {
  if (port)
    return l_zero;
  return l_pad.mpPad->mMainStickPosX;
}
/* 02007990 */
f32 CPad_GET_STICK_POS_Y(s32 port) {
  if (port)
    return l_zero;
  return l_pad.mpPad->mMainStickPosY;
}
/* 02007AFC */
f32 CPad_GET_SUBSTICK_POS_X(s32 port) {
  if (port)
    return l_zero;
  return l_pad.mpPad->mCStickPosX;
}
/* 02007B20 */
f32 CPad_GET_SUBSTICK_POS_Y(s32 port) {
  if (port)
    return l_zero;
  return l_pad.mpPad->mCStickPosY;
}
/* 020079B4 */
f32 CPad_GET_STICK_VALUE(s32 port) {
  if (port)
    return l_zero;
  auto *pad = l_pad.mpPad.get();
  f32 y = pad->mMainStickPosY, x = pad->mMainStickPosX;
  f32 squared = fmadds(x, x, fmuls_ppc(y, y));
  f32 magnitude = cAPIController_sqrt(squared);
  f32 bound = l_one;
  return fsubs_ppc(bound, magnitude) >= 0 ? magnitude : bound;
}
/* 02007A1C */
s16 CPad_GET_STICK_ANGLE(s32 port) {
  if (port)
    return 0;
  auto *pad = l_pad.mpPad.get();
  f32 zero = l_zero, y = pad->mMainStickPosY, x = pad->mMainStickPosX;
  f32 angle;
  if (y != zero)
    angle = cAPIController_atan2(x, -y);
  else if (x > zero)
    angle = l_positiveHalfPi;
  else {
    f32 again = pad->mMainStickPosX;
    angle = again >= 0 ? zero : l_negativeHalfPi;
  }
  return s16(ftoi(fmuls_ppc(angle, l_angleScale)));
}
/* 02007B44 */
f32 CPad_GET_SUBSTICK_VALUE(s32 port) {
  f32 zero = l_zero;
  if (port)
    return zero;
  auto *pad = l_pad.mpPad.get();
  f32 y = pad->mCStickPosY, x = pad->mCStickPosX;
  f32 squared = fmadds(x, x, fmuls_ppc(y, y));
  f64 estimate = zero;
  if (squared > zero) {
    f32 three = l_three, half = l_half;
    estimate = frsqrte(squared);
    f32 esq = f32(estimate * round25(estimate));
    f32 eh = f32(estimate * round25(half));
    f32 correction = fnmsubs(esq, squared, three);
    estimate = fmuls_ppc(correction, eh);
  }
  return f32(estimate * round25(squared));
}
/* 02007BA0 */
s16 CPad_GET_SUBSTICK_ANGLE(s32 port) {
  if (port)
    return 0;
  f32 y = CPad_GET_SUBSTICK_POS_YCall(s32(0));
  f32 x = CPad_GET_SUBSTICK_POS_XCall(s32(0));
  f32 angle = cAPIController_atan2(x, -y);
  return s16(ftoi(fmuls_ppc(l_angleScale, angle)));
}
/* 02007BFC */
f32 CPad_GET_ANALOG_L(s32 port) {
  if (!port && CPad_CHECK_HOLD_LCall(s32(0)))
    return l_one;
  return l_zero;
}
/* 02007C50 */
f32 CPad_GET_ANALOG_R(s32 port) {
  if (!port && CPad_CHECK_HOLD_RCall(s32(0)))
    return l_one;
  return l_zero;
}
/* 02007CA4 */
u32 mDoCPd_L_LOCK_BUTTON(s32 port) {
  if (port)
    return 0;
  return CPad_CHECK_HOLD_LCall(s32(0)) != 0;
}
/* 02007CDC */
u32 mDoCPd_R_LOCK_BUTTON(s32 port) {
  if (port)
    return 0;
  return CPad_CHECK_HOLD_RCall(s32(0)) != 0;
}
/* 02007D14 */
u32 mDoCPd_L_LOCK_TRIGGER(s32 port) {
  if (port)
    return 0;
  return CPad_CHECK_TRIG_LCall(s32(0)) != 0;
}
/* 02007D4C */
u32 mDoCPd_R_LOCK_TRIGGER(s32 port) {
  if (port)
    return 0;
  return CPad_CHECK_TRIG_RCall(s32(0)) != 0;
}
/* 02007D84 */
void cAPICPad_staticInit() {
  l_headerStatics.mZeroWords[2] = 0;
  l_headerStatics.mZeroWords[0] = 0;
  l_headerStatics.mZeroWords[3] = 0;
  l_headerStatics.mZeroWords[1] = 0;
  cAPIController_registerStatic(&l_records.mFirst);
  f32 first = l_negativePi, second = l_pi;
  l_headerStatics.mNegativePi = first;
  l_headerStatics.mPi = second;
  cAPIController_initFlag(&l_headerStatics.mFlag);
  cAPIController_registerStatic(&l_records.mSecond);
  cAPIController_initByte(&l_headerStatics.mByte);
  cAPIController_registerStatic(&l_records.mThird);
}

/* WWHD glue (tools/verify/autoglue.py)
ext 028F4384 f32 cAPIController_sqrt(f32)
ext 028F4D28 f32 cAPIController_atan2(f32, f32)
ext 028F026C void cAPIController_registerStatic(void*)
ext 028ED6F8 void cAPIController_initFlag(void*)
ext 028EAB2C void cAPIController_initByte(void*)
ext 02007B20 f32 CPad_GET_SUBSTICK_POS_YCall(s32)
ext 02007AFC f32 CPad_GET_SUBSTICK_POS_XCall(s32)
ext 02007690 u32 CPad_CHECK_HOLD_LCall(s32)
ext 02007664 u32 CPad_CHECK_HOLD_RCall(s32)
ext 0200786C u32 CPad_CHECK_TRIG_LCall(s32)
ext 02007840 u32 CPad_CHECK_TRIG_RCall(s32)
data l_zero 10000B24
data l_one 10000B28
data l_positiveHalfPi 10000B2C
data l_negativeHalfPi 10000B30
data l_angleScale 10000B34
data l_half 10000B38
data l_three 10000B3C
data l_negativePi 10000B40
data l_pi 10000B44
data l_pad 101F5088
data l_headerStatics 101FF37C
data l_records 1018C4B4
*/

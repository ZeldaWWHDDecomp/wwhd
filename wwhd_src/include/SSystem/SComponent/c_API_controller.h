#pragma once
#include "wwhd.h"
namespace cAPIController {
// HD: button masks differ from GameCube controller_pad_buttons bitfields.
enum Button : u32 {
  BUTTON_B = 1,
  BUTTON_A = 2,
  BUTTON_L = 4,
  BUTTON_X = 8,
  BUTTON_Y = 0x10,
  BUTTON_R = 0x20,
  BUTTON_START = 0x800,
  BUTTON_Z = 0x4000,
  BUTTON_UP = 0x10000,
  BUTTON_DOWN = 0x20000,
  BUTTON_LEFT = 0x40000,
  BUTTON_RIGHT = 0x80000
};
// Names follow interface_of_controller_pad; offsets are the verified HD pad layout.
struct interface_of_controller_pad {
  u8 padding0[0x18];
  be<u32> mButtonTrig;
  u8 padding1C[0x108];
  be<u32> mButtonHold;
  u8 padding128[8];
  be<f32> mMainStickPosX, mMainStickPosY, mCStickPosX, mCStickPosY;
};
struct PadPointer {
  gptr<interface_of_controller_pad> mpPad;
};
struct HeaderStatics {
  be<f32> mNegativePi, mPi;
  be<u8> mFlag, mByte;
  u8 paddingA[2];
  be<u32> mZeroWords[4];
};
struct StaticRecord {
  u8 opaque[12];
};
struct StaticRecords {
  StaticRecord mFirst, mSecond, mThird;
};
WWHD_OFFSET(interface_of_controller_pad, mButtonTrig, 0x18);
WWHD_OFFSET(interface_of_controller_pad, mButtonHold, 0x124);
WWHD_OFFSET(interface_of_controller_pad, mMainStickPosX, 0x130);
WWHD_SIZE(interface_of_controller_pad, 0x140);
WWHD_SIZE(HeaderStatics, 0x1C);
} // namespace cAPIController
f32 cAPIController_sqrt(f32);
f32 cAPIController_atan2(f32, f32);
void cAPIController_registerStatic(void *);
void cAPIController_initFlag(void *);
void cAPIController_initByte(void *);
f32 CPad_GET_SUBSTICK_POS_YCall(s32);
f32 CPad_GET_SUBSTICK_POS_XCall(s32);
u32 CPad_CHECK_HOLD_LCall(s32);
u32 CPad_CHECK_HOLD_RCall(s32);
u32 CPad_CHECK_TRIG_LCall(s32);
u32 CPad_CHECK_TRIG_RCall(s32);

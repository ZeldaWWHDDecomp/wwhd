/* HD rumble pattern playback; there is no GameCube pattern-player counterpart. */
#pragma once
#include "bindings.h"

struct mReCPd_pattern_c {
    /* 0x00 */ be<u16> mLength;
    /* 0x02 */ be<u8> mBits[1];
};
WWHD_OFFSET(mReCPd_pattern_c, mBits, 2);
WWHD_SIZE(mReCPd_pattern_c, 3); // HD: variable-length resource prefix, including its first byte.
struct mReCPd_patternPlayer_c {
    /* 0x00 */ gptr<be<u8>> mpPattern;
    /* 0x04 */ be<u16> mLength;
    /* 0x06 */ be<u16> mPosition;
    /* 0x08 */ be<u8> mLoop;
    /* 0x09 */ u8 _09[3];
};
WWHD_OFFSET(mReCPd_patternPlayer_c, mpPattern, 0);
WWHD_OFFSET(mReCPd_patternPlayer_c, mLength, 4);
WWHD_OFFSET(mReCPd_patternPlayer_c, mPosition, 6);
WWHD_OFFSET(mReCPd_patternPlayer_c, mLoop, 8);
WWHD_SIZE(mReCPd_patternPlayer_c, 0xC);
struct mReCPd_controllerState_c {
    /* 0x000 */ u8 _000[0x1D0];
    /* 0x1D0 */ be<u32> mGamePadRumble;
};
WWHD_OFFSET(mReCPd_controllerState_c, mGamePadRumble, 0x1D0);
WWHD_SIZE(mReCPd_controllerState_c, 0x1D4);
struct mReCPd_controllerGlobal_c { gptr<mReCPd_controllerState_c> mpState; };
WWHD_SIZE(mReCPd_controllerGlobal_c, 4);

void WPADControlMotor(u32 pad, u32 on);
void VPADStopMotor(u32 pad);
void VPADControlMotor(u32 pad, be<u8>* pattern, u32 length);
void mReCPd_assert(const char* file, u32 line, const char* expression);

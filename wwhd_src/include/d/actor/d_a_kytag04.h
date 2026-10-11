/* Weather tag 4: WWHD layout.
 * GameCube -> WWHD: actor members move by +0x11C with the larger base class.
 */
#pragma once
#include "bindings.h"

void dKy_change_colpat(u8 pat);

struct kytag04_class : fopAc_ac_c {
    enum : u32 { __vt = 0x10013B64 }; // HD: virtual destructor vtable
    /* 0x3AC */ be<u8> mState;
    /* 0x3AD */ be<u8> mOffColPat;
    /* 0x3AE */ be<u8> mOnColPat;
    /* 0x3AF */ be<u8> mSwitchNo;
    /* 0x3B0 */ be<s32> mTimer;
    /* 0x3B4 */ be<s32> mTimerThreshold;
    /* 0x3B8 */ be<f32> mScaleX;
    /* 0x3BC */ be<f32> mScaleY;
};
WWHD_OFFSET(kytag04_class, mState, 0x3AC);
WWHD_OFFSET(kytag04_class, mOffColPat, 0x3AD);
WWHD_OFFSET(kytag04_class, mOnColPat, 0x3AE);
WWHD_OFFSET(kytag04_class, mSwitchNo, 0x3AF);
WWHD_OFFSET(kytag04_class, mTimer, 0x3B0);
WWHD_OFFSET(kytag04_class, mTimerThreshold, 0x3B4);
WWHD_OFFSET(kytag04_class, mScaleX, 0x3B8);
WWHD_OFFSET(kytag04_class, mScaleY, 0x3BC);
WWHD_SIZE(kytag04_class, 0x3C0);

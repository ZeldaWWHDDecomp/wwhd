// WWHD tablet motor's timer and bit pattern helpers.
#pragma once
#include "sead/DeviceConstruction.h"
struct MotorSpan { be<u32> mHigh; be<u32> mLow; };
WWHD_OFFSET(MotorSpan,mHigh,0);
WWHD_OFFSET(MotorSpan,mLow,4);
WWHD_SIZE(MotorSpan,8);

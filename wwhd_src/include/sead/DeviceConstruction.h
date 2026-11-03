// WWHD platform controller construction layouts, recovered from this game's instructions.
#pragma once
#include "sead/DeviceBuffers.h"
struct TouchPoint {
    sead::InputPair mPosition;
    be<u32> mFlags;
    void update(u32 touching,f32 x,f32 y);
};
struct TabletController : sead::Controller {
    be<u8> mConnected;
    u8 mAlignConnected[3];
    be<u32> mTouchCapacity;
    be<u32> mTouchCount;
    be<u32> mTouchIndex;
    TouchPoint mTouchPoints[3];
};
struct PadMotor {
    u8 mBase[16];
    gptr<be<u32>> mTable;
    gptr<be<u8>> mPattern;
    be<u32> mFrames;
    be<u32> mRepeats;
    be<u8> mBits[16];
    be<u32> mLength;
    be<u32> mPadding;
    be<u32> mDeadline[2];
};
WWHD_OFFSET(TouchPoint,mFlags,8);
WWHD_SIZE(TouchPoint,12);
WWHD_OFFSET(TabletController,mTouchPoints,0x16C);
WWHD_SIZE(TabletController,0x190);
WWHD_OFFSET(PadMotor,mTable,16);
WWHD_OFFSET(PadMotor,mPattern,20);
WWHD_OFFSET(PadMotor,mFrames,24);
WWHD_OFFSET(PadMotor,mRepeats,28);
WWHD_OFFSET(PadMotor,mBits,32);
WWHD_OFFSET(PadMotor,mLength,48);
WWHD_OFFSET(PadMotor,mDeadline,56);
WWHD_SIZE(PadMotor,64);

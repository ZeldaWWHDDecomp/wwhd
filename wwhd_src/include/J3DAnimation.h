/** J3D frame controller implementation layout.
 * HD: no vtable; the GameCube fields are reordered into a sixteen-byte controller. */
#pragma once
#include "wwhd.h"
class J3DFrameCtrl {
public:
    enum Attribute_e {
        EMode_NULL = -1, EMode_NONE, EMode_RESET, EMode_LOOP, EMode_REVERSE, EMode_LOOP_REVERSE,
    };
    enum State_e { STATE_STOP_E = 1, STATE_LOOP_E = 2 };
    void init(s16 end);
    s32 checkPass(f32 pass_frame);
    void update();
    /* 0x00 */ be<f32> mRate;
    /* 0x04 */ be<f32> mFrame;
    /* 0x08 */ be<s16> mStart;
    /* 0x0A */ be<s16> mEnd;
    /* 0x0C */ be<s16> mLoop;
    /* 0x0E */ be<u8> mAttribute;
    /* 0x0F */ be<u8> mState;
};
WWHD_SIZE(J3DFrameCtrl, 0x10);
WWHD_OFFSET(J3DFrameCtrl, mRate, 0);
WWHD_OFFSET(J3DFrameCtrl, mFrame, 4);
WWHD_OFFSET(J3DFrameCtrl, mStart, 8);
WWHD_OFFSET(J3DFrameCtrl, mEnd, 0xA);
WWHD_OFFSET(J3DFrameCtrl, mLoop, 0xC);
WWHD_OFFSET(J3DFrameCtrl, mAttribute, 0xE);
WWHD_OFFSET(J3DFrameCtrl, mState, 0xF);
f64 J3DFrameCtrl_fmod(f64, f64);

#pragma once
#include "f_op/f_op_actor.h"
#include "d/d_bg_s.h"
#include "d/d_cc_d.h"
#include "m_Do/m_Do_ext.h"

struct IbColorAnimation {
    J3DFrameCtrl frame;
    u8 storage[0x78 - 0x10];
};
struct IbLight {
    cXyz position;
    be<s16> red, green, blue;
    u8 padding[2];
    be<f32> power, fluctuation;
    u8 unused[8];
};
struct daIball_c : fopAc_ac_c {
    dBgS_ObjAcch acch;                    // 0x3AC
    dBgS_AcchCir wall;                    // 0x570
    dCcD_Stts collisionStatus;            // 0x5B0
    dCcD_Cyl cylinder;                    // 0x5EC
    gptr<J3DModel> model;                 // 0x71C
    mDoExt_btkAnm textureAnimation;        // 0x720
    IbColorAnimation colorAnimation[2];   // 0x794
    J3DFrameCtrl transformAnimation;      // 0x884, actual HD instance is 16 bytes
    u8 rippleCallback[0x90];             // 0x894
    be<s32> timer;                        // 0x924
    u8 unused928[4];
    be<s32> playSpeedIndex;               // 0x92C
    u8 unused930[4];
    be<f32> previousSpeedY;               // 0x934
    be<u8> mode, playedSound;             // 0x938
    u8 unused93A[2];
    IbLight light;                        // 0x93C
    be<f32> lightFlicker;                 // 0x960
};
WWHD_SIZE(IbColorAnimation, 0x78);
WWHD_SIZE(IbLight, 0x24);
WWHD_OFFSET(daIball_c, acch, 0x3AC);
WWHD_OFFSET(daIball_c, wall, 0x570);
WWHD_OFFSET(daIball_c, collisionStatus, 0x5B0);
WWHD_OFFSET(daIball_c, cylinder, 0x5EC);
WWHD_OFFSET(daIball_c, model, 0x71C);
WWHD_OFFSET(daIball_c, textureAnimation, 0x720);
WWHD_OFFSET(daIball_c, colorAnimation, 0x794);
WWHD_OFFSET(daIball_c, transformAnimation, 0x884);
WWHD_OFFSET(daIball_c, rippleCallback, 0x894);
WWHD_OFFSET(daIball_c, timer, 0x924);
WWHD_OFFSET(daIball_c, light, 0x93C);
WWHD_OFFSET(daIball_c, lightFlicker, 0x960);
WWHD_SIZE(daIball_c, 0x964);

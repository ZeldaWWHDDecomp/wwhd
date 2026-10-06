/* d_a_pirate_flag.h (WWHD). */
#pragma once
#include "f_op/f_op_actor.h"
#include "bindings.h"
// HD cloth packet keeps the two 25-vector buffers, followed by GX2 resources.
struct daPirate_Flag_packet_c {
    u8 _000[0xCC];
    cXyz mPos[2][25];
    cXyz mNormal[2][25];
    cXyz mBackNormal[2][25];
    cXyz mVelocity[25];
    be<s16> mNormalAngle;
    be<s16> mEscapeAngle;
    be<s16> mWaveAngle;
    be<u8> mBuffer;
    be<u8> mInitialized;
};
WWHD_OFFSET(daPirate_Flag_packet_c,mPos,0xCC);
WWHD_OFFSET(daPirate_Flag_packet_c,mNormal,0x324);
WWHD_OFFSET(daPirate_Flag_packet_c,mBackNormal,0x57C);
WWHD_OFFSET(daPirate_Flag_packet_c,mVelocity,0x7D4);
WWHD_OFFSET(daPirate_Flag_packet_c,mWaveAngle,0x904);
WWHD_OFFSET(daPirate_Flag_packet_c,mBuffer,0x906);

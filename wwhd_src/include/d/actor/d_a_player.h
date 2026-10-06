/** Base-player and matrix emitter callback layouts (WWHD). */
#pragma once
#include "f_op/f_op_actor.h"

struct daPy_mtxFollowEcallBack_c {
    be<u32> mVtable;
    gptr<void> mpEmitter;
    gptr<void> mpMatrix;
};
WWHD_SIZE(daPy_mtxFollowEcallBack_c, 0xC);

struct daPy_py_c : fopAc_ac_c {
    u8 mCutType;
    u8 mCutCount;
    u8 pad3AE[2];
    be<s16> mDamageWaitTimer;
    be<s16> mQuakeTimer;
    be<s32> mFace;
    be<u32> mNoResetFlg0;
    u8 pad3BC[0x414-0x3BC];
    cXyz mWindVelocity;
    u8 pad420[0x43C-0x420];
};
WWHD_SIZE(daPy_py_c, 0x43C);

WWHD_OFFSET(daPy_mtxFollowEcallBack_c, mpEmitter, 0x4);
WWHD_OFFSET(daPy_mtxFollowEcallBack_c, mpMatrix, 0x8);
WWHD_OFFSET(daPy_py_c, mQuakeTimer, 0x3B2);
WWHD_OFFSET(daPy_py_c, mNoResetFlg0, 0x3B8);
WWHD_OFFSET(daPy_py_c, mWindVelocity, 0x414);

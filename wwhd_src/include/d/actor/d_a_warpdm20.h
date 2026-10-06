#pragma once
#include "f_op/f_op_actor.h"
#include "m_Do/m_Do_ext.h"

struct daWarpdm20_c : fopAc_ac_c {
    request_of_phase_process_class mPhase; // 3AC
    gptr<J3DModel> mpModel; // 3B4
    gptr<mDoExt_baseAnm> mpBtkAnm, mpBrkAnm, mpBckAnm; // 3B8..3C0
    u8 _3C4[4];
    be<u32> mRippleVtable; // 3C8
    gptr<void> mpRippleEmitter; // 3CC
    u8 _3D0[0x3E4 - 0x3D0];
    be<s32> mEventOrderState; // 3E4
    be<u8> mVisible; // 3E8
    u8 _3E9[5];
    be<s16> mEventIdx; // 3EE
    u8 _3F0[2];
    be<u8> mType; // 3F2
    u8 _3F3;
    be<u32> mAction; // 3F4
    be<s32> mStaffIdx; // 3F8
    cXyz mEffectPosition; // 3FC
    cXyz mRippleScale; // 408
    be<u8> mWarpFlashSet; // 414
    u8 _415[3];

    void animPlay();
    void initWait(s32);
    BOOL actWait(s32);
    void initWarp(s32);
    BOOL actWarp(s32);
    void initDead(s32);
    void initWait2(s32);
    BOOL actWait2(s32);
    u32 initReturnWait(s32);
    BOOL actReturnWait(s32);
};
WWHD_SIZE(daWarpdm20_c, 0x418);
static_assert(offsetof(daWarpdm20_c, mpRippleEmitter) == 0x3CC);
static_assert(offsetof(daWarpdm20_c, mEventIdx) == 0x3EE);
static_assert(offsetof(daWarpdm20_c, mRippleScale) == 0x408);

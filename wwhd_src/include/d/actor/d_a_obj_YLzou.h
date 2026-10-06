#pragma once
#include "bindings.h"
// HD Link statue: fields confirmed from its translation-unit disassembly.
struct daObjYLzou_c : fopAc_ac_c {
    request_of_phase_process_class mPhase;
    gptr<J3DModel> mpModel;
    gptr<void> mpBgW;
    Mtx34 mBgMtx;
    be<u32> mActionMember[2];
    be<s32> mActionIdx;
    be<s32> mSaveSwitch;
    be<s16> mEventIdx;
    u8 pad3FE[2];
    be<s32> mTriforceComplete;
    be<s32> mDemoIdx;
    u8 mSmokeCallbacks[0x68];
    be<s16> mVibAngle;
    u8 pad472[2];
    be<f32> mVibHeight;
    be<u8> mSmokePrevious, mSmokeRequested;
    u8 pad47A[2];
    be<s32> mVibPrevious, mVibRequested;
};
WWHD_OFFSET(daObjYLzou_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjYLzou_c, mBgMtx, 0x3BC);
WWHD_OFFSET(daObjYLzou_c, mActionIdx, 0x3F4);
WWHD_OFFSET(daObjYLzou_c, mSmokeCallbacks, 0x408);
WWHD_OFFSET(daObjYLzou_c, mVibAngle, 0x470);
static_assert(sizeof(daObjYLzou_c) == 0x484);

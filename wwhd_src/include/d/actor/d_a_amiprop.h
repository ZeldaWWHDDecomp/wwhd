// Rotating mesh propeller; WWHD actor layout.
#pragma once
#include "gc_api/gc_api.h"

// HD: local view of dBgW's correction callback, beyond the shared prefix.
struct daAmiProp_bgW_c : dBgW {
    /* 0x04 */ u8 _04[0xA8 - 4];
    /* 0xA8 */ be<u32> mpCrrFunc;
    void SetCrrFunc(u32 callback) { mpCrrFunc = callback; }
};
WWHD_OFFSET(daAmiProp_bgW_c, mpCrrFunc, 0xA8);

class daAmiProp_c : public fopAc_ac_c {
public:
    daAmiProp_c();
    bool _delete();
    BOOL CreateHeap();
    void CreateInit();
    cPhs_State _create();
    void set_mtx();
    void setMoveBGMtx();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ dCcD_Stts mStts;
    /* 0x3F4 */ dCcD_Cyl mCyl;
    /* 0x524 */ gptr<daAmiProp_bgW_c> mpBgW;
    /* 0x528 */ Mtx34 mBgMtx;  /* was unk_40C: CreateHeap 02050800 registers mpBgW (MOVE_BG) with this matrix; set_mtx 02050918 / setMoveBGMtx 02050788 copy the model (with scale) matrix into it (tww the same) */
    /* 0x558 */ be<u32> mSwitchNo;  /* was unk_43C: CreateInit 02050A04: daAmiProp_prm::getSwitchNo; _execute 02050D14 tests fopAcM_isSwitch(this, it) (tww the same) */
    /* 0x55C */ be<s32> mPrevSwitch;  /* was unk_440: _execute 02050D14: last frame's switch state; a change from it starts the turn, stored at the end (tww the same) */
    /* 0x560 */ be<u8> mTurning;  /* was unk_444: _execute 02050D14: set when the switch state changes; while set, angle x or y (by type) eases to mTargetAngle; cleared when the turn is done (tww the same) */
    /* 0x561 */ u8 _561;
    /* 0x562 */ be<s16> mTargetAngle;  /* was unk_446: CreateInit 02050A04: angle.x + 0x4000; _execute 02050D14 eases angle x / y to it (cLib_addCalcAngleS) and adds 0x4000 when reached (tww the same) */
};
WWHD_OFFSET(daAmiProp_c, mStts, 0x3B8);
WWHD_OFFSET(daAmiProp_c, mCyl, 0x3F4);
WWHD_OFFSET(daAmiProp_c, mpBgW, 0x524);
WWHD_OFFSET(daAmiProp_c, mSwitchNo, 0x558);
WWHD_OFFSET(daAmiProp_c, mTargetAngle, 0x562);
WWHD_SIZE(daAmiProp_c, 0x564);


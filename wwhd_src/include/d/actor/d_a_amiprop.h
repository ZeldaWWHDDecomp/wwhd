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
    /* 0x528 */ Mtx34 unk_40C;
    /* 0x558 */ be<u32> unk_43C;
    /* 0x55C */ be<s32> unk_440;
    /* 0x560 */ be<u8> unk_444;
    /* 0x561 */ u8 _561;
    /* 0x562 */ be<s16> unk_446;
};
WWHD_OFFSET(daAmiProp_c, mStts, 0x3B8);
WWHD_OFFSET(daAmiProp_c, mCyl, 0x3F4);
WWHD_OFFSET(daAmiProp_c, mpBgW, 0x524);
WWHD_OFFSET(daAmiProp_c, unk_43C, 0x558);
WWHD_OFFSET(daAmiProp_c, unk_446, 0x562);
WWHD_SIZE(daAmiProp_c, 0x564);


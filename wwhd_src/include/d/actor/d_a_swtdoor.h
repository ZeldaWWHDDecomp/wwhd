/* Fortress window door: WWHD members move +0x11C from GameCube. */
#pragma once
#include "gc_api/gc_api.h"

class swtdoor_class : public fopAc_ac_c {
public:
    swtdoor_class();
    enum : u32 { __vt = 0x1003EDFC }; // HD: virtual destructor
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> model;
    /* 0x3B8 */ be<u8> field_0x29c;
    /* 0x3B9 */ be<u8> mSwitchNo;
    /* 0x3BA */ u8 _3BA[2];
};
WWHD_OFFSET(swtdoor_class, model, 0x3B4);
WWHD_OFFSET(swtdoor_class, mSwitchNo, 0x3B9);

WWHD_SIZE(swtdoor_class, 0x3BC);

enum { dRes_INDEX_SWTDOOR_BMD_SWTDOOR_e = 3 };
void fopAcM_SetMin(fopAc_ac_c*, f32, f32, f32);
void fopAcM_SetMax(fopAc_ac_c*, f32, f32, f32);

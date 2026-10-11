// Windfall bomb-shop counter flap, WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

class daObj_Pbco_c : public fopAc_ac_c {
public:
    daObj_Pbco_c();
    BOOL CreateHeap();
    cPhs_State CreateInit();
    void set_mtx();
    static const char* M_arcname;

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ Mtx34 mMtx;
};

// HD: GHS places the HIO vtable after mNo, rather than before it.
struct daObj_Pbco_HIO_c {
    /* 0x00 */ be<s8> mNo;
    /* 0x01 */ u8 _01[3];
    /* 0x04 */ be<u32> mVtable;
};
WWHD_OFFSET(daObj_Pbco_c, mPhs, 0x3AC);
WWHD_OFFSET(daObj_Pbco_c, mpModel, 0x3B4);
WWHD_OFFSET(daObj_Pbco_c, mpBgW, 0x3B8);
WWHD_OFFSET(daObj_Pbco_c, mMtx, 0x3BC);
WWHD_SIZE(daObj_Pbco_c, 0x3EC);
WWHD_SIZE(daObj_Pbco_HIO_c, 8);

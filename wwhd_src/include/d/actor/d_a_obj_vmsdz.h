/* Master Sword pedestal, WWHD members +0x11C from GameCube. */
#pragma once
#include "gc_api/gc_api.h"
class daObjVmsdz_c : public fopAc_ac_c {
public:
    daObjVmsdz_c();
    enum : u32 { __vt = 0x10032E84 };
    BOOL create_heap();
    cPhs_State _create();
    bool _delete();
    void init_mtx();
    bool _draw();
    static const char M_arcname[];
    /* 0x3AC */ gptr<J3DModel> mModel;
    /* 0x3B0 */ request_of_phase_process_class mPhs;
};
WWHD_OFFSET(daObjVmsdz_c, mModel, 0x3AC);
WWHD_OFFSET(daObjVmsdz_c, mPhs, 0x3B0);
WWHD_SIZE(daObjVmsdz_c, 0x3B8);

// Puppet Ganon intro bed, WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

// HD: XYZ matrix rotation is an out-of-line helper in this translation unit.
void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16);

class daObjGbed_c : public fopAc_ac_c {
public:
    daObjGbed_c();
    BOOL create_heap();
    void init_mtx();
    cPhs_State _create();
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
};
WWHD_SIZE(daObjGbed_c, 0x3BC);
WWHD_OFFSET(daObjGbed_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjGbed_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjGbed_c, mpBgW, 0x3B8);

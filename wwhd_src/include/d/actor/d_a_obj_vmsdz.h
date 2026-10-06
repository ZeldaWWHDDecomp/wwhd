#pragma once
#include "bindings.h"
// HD preserves the GC order: model, then the resource phase.
struct daObjVmsdz_c : fopAc_ac_c {
    gptr<J3DModel> mModel;
    request_of_phase_process_class mPhs;
    BOOL create_heap();
    void init_mtx();
    s32 _create();
    BOOL _delete();
    BOOL _draw();
};
WWHD_OFFSET(daObjVmsdz_c, mModel, 0x3AC);
WWHD_OFFSET(daObjVmsdz_c, mPhs, 0x3B0);
WWHD_SIZE(daObjVmsdz_c, 0x3B8);

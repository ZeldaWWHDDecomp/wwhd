#pragma once
#include "gc_api/gc_api.h"
struct daObjGnntakie_c : fopAc_ac_c {
    daObjGnntakie_c();
    enum : u32 { __vt = 0x1002975C };
    static const char M_arcname[];
    BOOL create_heap();
    void init_mtx();
    s32 _create();
    bool _delete();
    bool _execute();
    bool _draw();
    gptr<J3DModel> mpModel;
    request_of_phase_process_class mPhase;
    mDoExt_btkAnm mpBtkAnm;
};
WWHD_OFFSET(daObjGnntakie_c, mpModel, 0x3AC);
WWHD_OFFSET(daObjGnntakie_c, mPhase, 0x3B0);
WWHD_OFFSET(daObjGnntakie_c, mpBtkAnm, 0x3B8);
WWHD_SIZE(daObjGnntakie_c, 0x42C);

void gnntakie_actor_dt(void*, s32);

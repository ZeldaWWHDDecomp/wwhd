// Collision box: WWHD MoveBg actor grows 0x2C8 -> 0x3E0.
#pragma once
#include "gc_api/gc_api.h"

enum {
    dRes_INDEX_USOVMC_BDL_VMCBX_e = 4,
    dRes_INDEX_USOVMC_DZB_VMCBS_e = 7,
};

namespace daObjUsovmc {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c() {
        // HD: GHS constructs the base and installs this actor vtable.
        dBgS_MoveBgActor::ct(this);
        __vtbl = 0x100328D0;
    }
    static const char M_arcname[];
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** mtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mModel;
};
WWHD_OFFSET(Act_c, mModel, 0x3E8);
}  // namespace daObjUsovmc
WWHD_SIZE(daObjUsovmc::Act_c, 0x3EC);

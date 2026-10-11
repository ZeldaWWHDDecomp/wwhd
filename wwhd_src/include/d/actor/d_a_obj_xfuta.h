#pragma once
#include "gc_api/gc_api.h"
namespace daObjXfuta {
class Act_c : public fopAc_ac_c {
public:
    Act_c() { fopAc_ac_c_ct(this); __vtbl = 0x100338D0; }
    bool create_heap();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    bool _execute();
    bool _draw();
    static const char M_arcname[];
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    Mtx34 mMtx;
};
}
WWHD_OFFSET(daObjXfuta::Act_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjXfuta::Act_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjXfuta::Act_c, mMtx, 0x3B8);
WWHD_SIZE(daObjXfuta::Act_c, 0x3E8);
enum { dRes_INDEX_X_FUTA_BDL_X_FUTA_e = 3 };
u32 Xfuta_create_heap_result(daObjXfuta::Act_c*);

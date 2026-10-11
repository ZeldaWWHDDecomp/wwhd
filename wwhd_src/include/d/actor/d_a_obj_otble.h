#pragma once
#include "gc_api/gc_api.h"
namespace daObj_Otble {
class Act_c : public fopAc_ac_c {
public:
    // HD: the actor and embedded collision helpers use TU-specific vtables.
    Act_c() {
        fopAc_ac_c_ct(this);
        __vtbl = 0x1002E374;
        __ct(&mObjAcch);
        __ct(&mObjAcchCir);
    }
    BOOL _createHeap();
    void set_mtx();
    void CreateInit();
    cPhs_State _create();
    bool _delete();
    BOOL _execute();
    BOOL _draw();
    gptr<J3DModel> mModel;
    be<s32> m294;
    request_of_phase_process_class mPhase;
    gptr<dBgW> mBgW;
    Mtx34 m2A4;
    dBgS_ObjAcch mObjAcch;
    dBgS_AcchCir mObjAcchCir;
};
}
WWHD_SIZE(daObj_Otble::Act_c, 0x5F4);
WWHD_OFFSET(daObj_Otble::Act_c, m2A4, 0x3C0);
WWHD_OFFSET(daObj_Otble::Act_c, mObjAcch, 0x3F0);
WWHD_OFFSET(daObj_Otble::Act_c, mObjAcchCir, 0x5B4);
// HD: typed view of the compiler's guarded local heap-size array.
struct OtbleHeapSizes { be<u32> values[2]; };
u32 dBgS_GetPolyColor(dBgS*, cBgS_PolyInfo*);

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
    /* mVariant (0x3B0) was m294: _create 023811A0: param bits 0-7 clamped to 0 / 1; _createHeap 02380D28 picks the bdl (OTABLE / OTABLEL) and dzb by it, _create the heap size, CreateInit 02380F8C the cull box (tww the same) */
    be<s32> mVariant;
    request_of_phase_process_class mPhase;
    gptr<dBgW> mBgW;
    /* mBgMtx (0x3C0) was m2A4: _createHeap 02380D28: the model matrix that mBgW is Set with (dBgW::MOVE_BG_e); set_mtx 02380E74 copies the new matrix into it each frame before mBgW->Move() (tww the same) */
    Mtx34 mBgMtx;
    dBgS_ObjAcch mObjAcch;
    dBgS_AcchCir mObjAcchCir;
};
}
WWHD_SIZE(daObj_Otble::Act_c, 0x5F4);
WWHD_OFFSET(daObj_Otble::Act_c, mBgMtx, 0x3C0);
WWHD_OFFSET(daObj_Otble::Act_c, mObjAcch, 0x3F0);
WWHD_OFFSET(daObj_Otble::Act_c, mObjAcchCir, 0x5B4);
// HD: typed view of the compiler's guarded local heap-size array.
struct OtbleHeapSizes { be<u32> values[2]; };
u32 dBgS_GetPolyColor(dBgS*, cBgS_PolyInfo*);

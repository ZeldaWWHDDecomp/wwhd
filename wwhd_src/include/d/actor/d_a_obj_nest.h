/* Kargaroc nest, WWHD layout: MoveBgActor base grows from 0x2C8 to 0x3E0.
 * Tww numeric vibration field names retained. */
#pragma once
#include "gc_api/gc_api.h"

/* Existing unit-local binding. */
void fopAcM_setCullSizeSphere(fopAc_ac_c*, f32, f32, f32, f32);
/* Unit-local view of the existing dBgW ride callback, beyond its shared prefix. */
struct dBgW_nest_view {
    /* 0x00 */ u8 _00[0xB0];
    /* 0xB0 */ gfn mRideCallback;
    void SetRideCallback(void (*callback)(dBgW*, fopAc_ac_c*, fopAc_ac_c*)) {
        mRideCallback.v = gabi::guest_fn((const void*)callback);
    }
};
WWHD_OFFSET(dBgW_nest_view, mRideCallback, 0xB0);
enum { dRes_INDEX_MTORISU_BDL_MTORISU_e = 4, dRes_INDEX_MTORISU_DZB_MTORISU_e = 7 };
namespace daObjNest {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c() {
        dBgS_MoveBgActor::ct(this);
        __vtbl = 0x1002DEC0; // HD: this translation unit's actor vtable.
    }
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Delete();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    void vib_set(f32);
    void vib_proc();
    BOOL Execute(Mtx34**);
    BOOL Draw();
    static const char M_arcname[];

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ cXyz mcXyz_0x2D4;
    /* 0x3F8 */ csXyz mcsXyz_0x2E0;
    /* 0x3FE */ csXyz mcsXyz_0x2E6;
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mcsXyz_0x2E6, 0x3FE);
WWHD_SIZE(Act_c, 0x404);
}

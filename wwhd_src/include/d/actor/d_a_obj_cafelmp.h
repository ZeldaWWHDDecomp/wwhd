#pragma once
#include "gc_api/gc_api.h"
struct daObjCafelmp_c : fopAc_ac_c {
    daObjCafelmp_c();
    enum : u32 { __vt = 0x10026A0C };
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    BOOL CreateHeap();
    void set_mtx();
    void CreateInit();
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();
};
WWHD_OFFSET(daObjCafelmp_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjCafelmp_c, mpModel, 0x3B4);
WWHD_SIZE(daObjCafelmp_c, 0x3B8);

struct CafelmpHeaderZeros {
    be<u32> words[4];
};
void cafelmp_actor_dt(void *, s32);
void cafelmp_initA(void *);
void cafelmp_initB(void *);

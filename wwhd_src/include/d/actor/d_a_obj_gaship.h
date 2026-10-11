#pragma once
#include "gc_api/gc_api.h"
namespace daObjGaship {
struct Act_c : fopAc_ac_c {
    Act_c();
    enum : u32 { __vt = 0x100295D0 };
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mModel;
    BOOL create_heap();
    void set_mtx();
    Mtx34 mtx;
    be<u8> birthFlag[2];
    u8 pad[2];
    void birth_flag();
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();
};
} // namespace daObjGaship
WWHD_OFFSET(daObjGaship::Act_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjGaship::Act_c, mModel, 0x3B4);
WWHD_OFFSET(daObjGaship::Act_c, mtx, 0x3B8);
WWHD_OFFSET(daObjGaship::Act_c, birthFlag, 0x3E8);
WWHD_SIZE(daObjGaship::Act_c, 0x3EC);

struct GashipHeaderZeros {
    be<u32> words[4];
};
struct GashipFlagOffsets {
    cXyz values[2];
};
struct GashipFlagAngles {
    csXyz values[2];
};
struct GashipFlagLiteral {
    be<f32> secondZ, secondY, secondX, firstZ, firstY, firstX;
};
void gashipModelCalc(J3DModel *);
void gashipActorDt(void *, s32);
void gashipInitA(void *);
void gashipInitB(void *);

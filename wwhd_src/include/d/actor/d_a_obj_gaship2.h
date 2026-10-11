#pragma once
#include "gc_api/gc_api.h"
namespace daObjGaship2 {
struct Act_c : fopAc_ac_c {
    Act_c();
    enum : u32 { __vt = 0x10029648 };
    request_of_phase_process_class mphs;
    gptr<J3DModel> mpModel;
    BOOL create_heap();
    void set_mtx();
    gptr<dBgW> mpBgW;
    Mtx34 mMtx;
    s32 _create();
    BOOL _delete();
    BOOL _draw();
    BOOL _execute();
};
} // namespace daObjGaship2
WWHD_OFFSET(daObjGaship2::Act_c, mphs, 0x3AC);
WWHD_OFFSET(daObjGaship2::Act_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjGaship2::Act_c, mMtx, 0x3BC);
WWHD_OFFSET(daObjGaship2::Act_c, mpBgW, 0x3B8);
WWHD_SIZE(daObjGaship2::Act_c, 0x3EC);

struct GashipHeaderZeros {
    be<u32> words[4];
};
// HD: the movement correction callback is stored at +0xA8 of dBgW.
struct GashipBgW : dBgW {
    u8 padding[0xA8 - sizeof(dBgW)];
    be<u32> mCrrFunc;
};
WWHD_OFFSET(GashipBgW, mCrrFunc, 0xA8);
enum : u32 { dBgS_MoveBGProc_Typical = 0x024EE658 };
inline void gashipSetCrrFunc(dBgW *bg, u32 callback) {
    ((GashipBgW *)bg)->mCrrFunc = callback;
}
void gashipModelCalc(J3DModel *);
void gashipActorDt(void *, s32);
void gashipInitA(void *);
void gashipInitB(void *);

#pragma once
#include "gc_api/gc_api.h"
// HD: local extension for cBgW's correction callback at the verified offset.
struct RforceBgW : dBgW {
    u8 _04[0xA8 - 4];
    be<u32> mpCrrFunc;
    void SetCrrFunc(u32 callback) { mpCrrFunc = callback; }
};
WWHD_OFFSET(RforceBgW, mpCrrFunc, 0xA8);
namespace daObjRforce {
class Act_c : public fopAc_ac_c {
public:
    Act_c() { fopAc_ac_c_ct(this); __vtbl = 0x1002ED88; }
    bool create_heap();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    bool _execute();
    bool _draw();
    static const char M_arcname[8];
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel;
    gptr<RforceBgW> mpBgw;
    Mtx34 mtx;
};
}
WWHD_SIZE(daObjRforce::Act_c, 0x3EC);
WWHD_OFFSET(daObjRforce::Act_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjRforce::Act_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjRforce::Act_c, mpBgw, 0x3B8);
WWHD_OFFSET(daObjRforce::Act_c, mtx, 0x3BC);
enum { dRes_INDEX_STPTETU_BDL_STPTETU_e = 4, dRes_INDEX_STPTETU_DZB_STPTETU_e = 7 };
// HD: SetCrrFunc stores the guest callback address; it does not invoke it here.
constexpr u32 dBgS_MoveBGProc_Typical = 0x024EE658;
u32 Rforce_create_heap_result(daObjRforce::Act_c*);

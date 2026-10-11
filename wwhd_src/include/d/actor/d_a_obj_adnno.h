#pragma once
#include "gc_api/gc_api.h"
class daObjAdnno_c : public fopAc_ac_c {
public:
    // HD: construction also writes this TU's virtual-destructor table.
    daObjAdnno_c() { fopAc_ac_c_ct(this); __vtbl = 0x1002472C; }
    BOOL CreateHeap();
    void set_mtx();
    void CreateInit();
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();
    request_of_phase_process_class mPhs;
    gptr<J3DModel> mpModel[16];
};
WWHD_OFFSET(daObjAdnno_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjAdnno_c, mpModel, 0x3B4);
WWHD_SIZE(daObjAdnno_c, 0x3F4);
enum { dRes_INDEX_ADNNO_BDL_ADNNO_e = 4 };
// HD: unit-emitted color conversion companion uses four consecutive float channels.
struct AdnnoFloatColor { be<f32> r, g, b, a; };

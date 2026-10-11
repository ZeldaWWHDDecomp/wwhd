#pragma once
#include "gc_api/gc_api.h"
class daObj_Bscurtain_c : public fopAc_ac_c {
public:
    // HD: actor construction writes this TU's virtual-destructor table.
    daObj_Bscurtain_c() { fopAc_ac_c_ct(this); __vtbl = 0x100264AC; }
    BOOL CreateHeap();
    void set_mtx();
    cPhs_State CreateInit();
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();
    static const char M_arcname[4];
    request_of_phase_process_class mPhs;
    be<u32> field_0x298;
    gptr<J3DModel> mpModel;
};
WWHD_OFFSET(daObj_Bscurtain_c, mPhs, 0x3AC);
WWHD_OFFSET(daObj_Bscurtain_c, mpModel, 0x3B8);
WWHD_SIZE(daObj_Bscurtain_c, 0x3BC);
class daObj_Bscurtain_HIO_c {
public:
    // HD: the compiler places the HIO vtable after the data members.
    be<s8> mNo;
    u8 _01[3];
    be<f32> field_0x08;
    be<u16> field_0x0c;
    u8 _0A[2];
    be<u32> __vtbl;
};
WWHD_OFFSET(daObj_Bscurtain_HIO_c, field_0x08, 4);
WWHD_OFFSET(daObj_Bscurtain_HIO_c, __vtbl, 12);
WWHD_SIZE(daObj_Bscurtain_HIO_c, 16);
struct BscurtainFloatColor { be<f32> r, g, b, a; };
enum { dRes_INDEX_PTC_BDL_PTCO_e = 3, dRes_INDEX_PTC_BDL_PTCU_e = 4,
       EVENT_UNK_1F08 = 0x1F08 };

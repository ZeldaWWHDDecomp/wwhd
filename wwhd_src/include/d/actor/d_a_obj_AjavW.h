/* Water inside Jabun's cave: WWHD layout.
 * GameCube -> WWHD: actor members +0x11C; mDoExt_btkAnm grows from 0x14 to 0x74,
 * so mpBgW moves by a further 0x60. */
#pragma once
#include "gc_api/gc_api.h"

enum {
    dRes_INDEX_AJAVW_BDL_AJAVW_e = 5,
    dRes_INDEX_AJAVW_BTK_AJAVW_e = 8,
    dRes_INDEX_AJAVW_DZB_AJAVW_e = 0xB,
    TEV_TYPE_BG1 = 2,
};

class daObjAjavW_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x10024A04 }; // HD: actor vtable
    daObjAjavW_c();
    cPhs_State _create();
    bool _execute();
    bool _draw();
    bool _delete();
    bool create_heap();

public:
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x42C */ gptr<dBgW> mpBgW;
};
WWHD_OFFSET(daObjAjavW_c, mPhs, 0x3AC);
WWHD_OFFSET(daObjAjavW_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjAjavW_c, mBtkAnm, 0x3B8);
WWHD_OFFSET(daObjAjavW_c, mpBgW, 0x42C);
WWHD_SIZE(daObjAjavW_c, 0x430);

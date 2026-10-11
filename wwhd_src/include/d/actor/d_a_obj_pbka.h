/* Windfall Bomb Shop ceiling fan, WWHD layout.
 * GameCube -> WWHD: fopAc_ac_c grows by 0x11C; members shift by the same amount. */
#pragma once
#include "gc_api/gc_api.h"

enum { dRes_INDEX_PBKA_BDL_PBKA_e = 3 };
enum { JA_SE_OBJ_BOMB_SHOP_FAN = 0x619F };

class daObjPbka_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x1002E5F4 };
    daObjPbka_c() {
        // HD: GHS explicitly constructs the actor base and installs its vtable.
        fopAc_ac_c_ct(this);
        __vtbl = __vt;
    }
    cPhs_State _create();
    bool _delete();
    bool _draw();
    bool _execute();
    BOOL CreateHeap();
    void CreateInit();
    void set_mtx();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
};
WWHD_OFFSET(daObjPbka_c, mPhase, 0x3AC);
WWHD_OFFSET(daObjPbka_c, mpModel, 0x3B4);
WWHD_SIZE(daObjPbka_c, 0x3B8);

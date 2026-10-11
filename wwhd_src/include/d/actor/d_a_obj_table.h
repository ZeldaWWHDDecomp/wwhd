/* Table actor, WWHD layout.
 * GameCube -> WWHD: MoveBgActor base grows from 0x2C8 to 0x3E0. */
#pragma once
#include "gc_api/gc_api.h"
enum {
    dRes_INDEX_TABLE_BDL_YTBLE_e = 4,
    dRes_INDEX_TABLE_BDL_QCFIS_e = 5,
    dRes_INDEX_TABLE_DZB_YTBLE_e = 9, // HD: archive resource order
    dRes_INDEX_TABLE_DZB_QCFIS_e = 8,
};
namespace daObjTable {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c() {
        // HD: GHS explicitly constructs the base and installs the class vtable.
        dBgS_MoveBgActor::ct(this);
        __vtbl = 0x100305E8;
    }
    enum Prm_e { PRM_MDL_W = 8, PRM_MDL_S = 0 };
    u8 prm_get_mdl();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Delete();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(Mtx34** matrix);
    BOOL Draw();
    static const char M_arcname[];
    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
};
WWHD_OFFSET(Act_c, mPhs, 0x3E0);
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_SIZE(Act_c, 0x3EC);
}

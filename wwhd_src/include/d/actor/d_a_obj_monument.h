// Stone monument: WWHD actor members shift +0x11C from GameCube.
#pragma once
#include "gc_api/gc_api.h"

enum { dRes_INDEX_ESEKH_BDL_ESEKH_e=4, dRes_INDEX_ESEKH_BDL_ESEKH2_e=5,
       dRes_INDEX_ESEKH_DZB_ESEKH_e=8, dRes_INDEX_ESEKH_DZB_ESEKH2_e=9 };
namespace daObjMonument {
enum Type_e { ONE_e, TWO_e };
class Act_c : public fopAc_ac_c {
public:
    Act_c() {
        // HD: GHS constructs the actor base and installs its vtable.
        fopAc_ac_c_ct(this);
        __vtbl = 0x1002CF08;
    }
    static const char M_arcname[];
    enum Prm_e {
        PRM_TYPE_W = 0x01,
        PRM_TYPE_S = 0x00,
        PRM_SWSAVE_W = 0x08,
        PRM_SWSAVE_S = 0x08,
    };
    s32 prm_get_type();
    u32 prm_get_swSave();
    bool create_heap();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    void init_mtx();
    bool _execute();
    bool _draw();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ be<s32> mType;
    /* 0x3BC */ gptr<dBgW> mpBgW;
    /* 0x3C0 */ Mtx34 mtx;
};
WWHD_OFFSET(Act_c, mType, 0x3B8);
WWHD_OFFSET(Act_c, mtx, 0x3C0);
WWHD_SIZE(Act_c, 0x3F0);
}  // namespace daObjMonument

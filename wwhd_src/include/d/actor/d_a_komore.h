/* Foliage light shafts.
 * GameCube -> WWHD: actor members shift +0x11C; btkAnm grows to 0x74. */
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_FRLT_BDL_YFRLT00_e = 4, dRes_INDEX_FRLT_BTK_YFRLT00_e = 7 };
enum { TEV_TYPE_BG2 = 3 };
namespace daKomore {
class Act_c : public fopAc_ac_c {
public:
    Act_c() {
        // HD: GHS explicitly constructs the base and animation member.
        fopAc_ac_c_ct(this);
        __vtbl = 0x10013600;
        mDoExt_btkAnm::ct(&mBtkAnm);
    }
    static const char M_arcname[];
    BOOL create_heap();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    bool _execute();
    bool _draw();
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x42C */ Mtx34 mMtx;
};
WWHD_OFFSET(Act_c, mPhs, 0x3AC);
WWHD_OFFSET(Act_c, mpModel, 0x3B4);
WWHD_OFFSET(Act_c, mBtkAnm, 0x3B8);
WWHD_OFFSET(Act_c, mMtx, 0x42C);
WWHD_SIZE(Act_c, 0x45C);
}

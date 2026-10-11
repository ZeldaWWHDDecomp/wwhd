/* daKaji_c (Tetra's ship wheel), WWHD layout.
 * GameCube members move by +0x11C with the 0x3AC actor base; the matrix ends at 0x3EC.
 * The ship view contains only the GameCube fields used by this unit. */
#pragma once
#include "gc_api/gc_api.h"

enum {
    dRes_INDEX_KAJI_BDL_ASODA_e = 0x11,
    dRes_INDEX_KAJI_BCK_KJ_WAIT_e = 0xE,
    dRes_INDEX_KAJI_BAS_KJ_WAIT_e = 8,
};
namespace daObjPirateship {
struct Act_c : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3E4 - 0x3AC];
    /* 0x3E4 */ be<u8> m2CC; /* GameCube name: ship visible */
    /* 0x3E5 */ u8 _3E5[3];
    /* 0x3E8 */ gptr<J3DModel> mModel;
};
WWHD_OFFSET(Act_c, m2CC, 0x3E4);
WWHD_OFFSET(Act_c, mModel, 0x3E8);
WWHD_SIZE(Act_c, 0x3EC);
}
class daKaji_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x10012284 }; // HD: virtual destructor
    static const char* M_arcname;
    daKaji_c();
    void set_mtx();
    cPhs_State _create();
    bool _execute();
    bool _draw();
    BOOL CreateHeap();
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ u8 pad[4];
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3BC */ Mtx34 mMtx;
};
WWHD_OFFSET(daKaji_c, mPhs, 0x3AC);
WWHD_OFFSET(daKaji_c, mpMorf, 0x3B8);
WWHD_OFFSET(daKaji_c, mMtx, 0x3BC);
WWHD_SIZE(daKaji_c, 0x3EC);

/* GameCube mDoExt_McaMorf::update, external unit (the glue supplies its binding). */
void mDoExt_McaMorf_update(mDoExt_McaMorf*);

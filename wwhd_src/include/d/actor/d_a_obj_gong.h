// Gong actor: WWHD members follow the enlarged 0x3AC actor base.
#pragma once
#include "gc_api/gc_api.h"
enum { dRes_INDEX_VDORA_BDL_VDORA_e = 4, dRes_INDEX_VDORA_BCK_05_VDORA_CUT02_HIT_e = 7,
    DEMO_ENABLE_TRANS_e = 2, DEMO_ENABLE_ROTATE_e = 8,
    DEMO_ENABLE_ANM_e = 0x20, DEMO_ENABLE_ANM_FRAME_e = 0x40 };
void dDlst_texSpecmapST(cXyz*, dKy_tevstr_c*, J3DModel*, f32);
BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, u16*, u32, s8);
namespace daObjGong {
class Act_c : public fopAc_ac_c {
public:
    Act_c() { fopAc_ac_c_ct(this); __vtbl = 0x10029898; } // HD: virtual destructor
    static const char M_arcname[];
    cPhs_State _create();
    bool _execute();
    bool _draw();
    bool _delete();
    bool create_heap();
    void init_mtx();
    void set_mtx();
    bool demo_move();
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<mDoExt_McaMorf> mpMorf;
};
WWHD_OFFSET(Act_c, mpMorf, 0x3B4);
WWHD_SIZE(Act_c, 0x3B8);
}

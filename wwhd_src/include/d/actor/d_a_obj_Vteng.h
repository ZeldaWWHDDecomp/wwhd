/* Puppet Ganon curtain, WWHD layout. GameCube members move by +0x11C with
 * the 0x3AC actor base; size 0x3F0. */
#pragma once
#include "gc_api/gc_api.h"

enum {
    dRes_INDEX_VTENG_BCK_VTENG_e = 5,
    dRes_INDEX_VTENG_BDL_VTENG_e = 8,
    dRes_INDEX_VTENG_DZB_VTENG_e = 0xB,
    TEV_TYPE_BG3 = 4,
    DEMO_ENABLE_ANM_e = 0x20,
    DEMO_ENABLE_ANM_FRAME_e = 0x40,
};

class daObjVteng_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x10033124 }; // HD: virtual destructor
    daObjVteng_c();
    void init_mtx();
    cPhs_State _create();
    bool _execute();
    bool _draw();
    bool _delete();
    bool create_heap();
    bool jokai_demo();

    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mpMorf;
    /* 0x3BC */ gptr<dBgW> mpBgW;
    /* 0x3C0 */ Mtx34 mtx;
};
WWHD_OFFSET(daObjVteng_c, mpModel, 0x3B4);
WWHD_OFFSET(daObjVteng_c, mpBgW, 0x3BC);
WWHD_OFFSET(daObjVteng_c, mtx, 0x3C0);
WWHD_SIZE(daObjVteng_c, 0x3F0);


/* External GameCube APIs, bound by this unit's glue. */
void mDoMtx_XYZrotM(Mtx34*, s16, s16, s16);
BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, u16*, u32, s8);

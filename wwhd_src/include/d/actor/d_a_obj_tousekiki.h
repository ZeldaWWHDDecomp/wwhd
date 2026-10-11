// Catapult and its ship view: WWHD members follow the enlarged actor base.
#pragma once
#include "gc_api/gc_api.h"
class daObj_Tousekiki_c : public fopAc_ac_c {
public:
    daObj_Tousekiki_c() {
        fopAc_ac_c_ct(this); __vtbl = 0x100315F4;
        dCcD_Stts_ct(&mStts); dCcD_Cyl_ct(&mCyl, 0x100315E4);
    }
    static const char M_arcname[];
    cPhs_State _create();
    bool _delete();
    bool _execute();
    bool _draw();
    void demo_move();
    void set_mtx();
    BOOL CreateHeap();

    /* 0x3AC */ request_of_phase_process_class mPhase;
    /* 0x3B4 */ gptr<J3DModel> mModel;
    /* 0x3B8 */ gptr<mDoExt_McaMorf> mMorf;
    /* 0x3BC */ Mtx34 m2A0;
    /* 0x3EC */ be<u16> m2D0;
    /* 0x3EE */ u8 _3EE[2];
    /* 0x3F0 */ dCcD_Stts mStts;
    /* 0x42C */ dCcD_Cyl mCyl;
};
WWHD_OFFSET(daObj_Tousekiki_c, m2A0, 0x3BC);
WWHD_OFFSET(daObj_Tousekiki_c, mStts, 0x3F0);
WWHD_OFFSET(daObj_Tousekiki_c, mCyl, 0x42C);
WWHD_SIZE(daObj_Tousekiki_c, 0x55C);

/* daObjPirateship::Act_c, the fields read here (HD offsets) */
struct daObjPirateship_ActView : fopAc_ac_c {
    /* 0x3AC */ u8 _3AC[0x3E4 - 0x3AC];
    /* 0x3E4 */ be<u8> m2CC;              /* GameCube 0x2C8 */
    /* 0x3E5 */ u8 _3E5[3];
    /* 0x3E8 */ gptr<J3DModel> mModel;    /* GameCube 0x2CC */
};
WWHD_OFFSET(daObjPirateship_ActView, mModel, 0x3E8);


enum { dRes_INDEX_TOUSEKI_BCK_ATOSK_NAGE_e=5, dRes_INDEX_TOUSEKI_BDL_ATOSK_A_e=8,
    DEMO_ENABLE_ROTATE_e=8, DEMO_ENABLE_ANM_e=0x20, DEMO_ENABLE_ANM_FRAME_e=0x40 };
BOOL dDemo_setDemoData(fopAc_ac_c*, u8, mDoExt_McaMorf*, const char*, s32, u16*, u32, s8);

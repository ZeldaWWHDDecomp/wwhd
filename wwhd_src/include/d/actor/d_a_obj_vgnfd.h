// Ganon's Tower trials door.
#pragma once
#include "gc_api/gc_api.h"
struct daObjVgnfd_c : fopAc_ac_c {
    BOOL create_bdl_brk(s32);
    BOOL create_heap();
    cPhs_State _create();
    bool _delete();
    s32 get_start_demo_idx();
    BOOL check_fin();
    void on_fin();
    void init_mtx();
    void set_timer();
    bool _execute();
    bool _draw();
    /* HD: model, animation and smoke objects grew while retaining tww member order. */
    /* 0x3AC */ gptr<J3DModel> mModel[5];
    /* 0x3C0 */ gptr<J3DModel> mModel2[2];
    /* 0x3C8 */ mDoExt_btkAnm mBtkAnm;
    /* 0x43C */ request_of_phase_process_class mPhs;
    /* 0x444 */ mDoExt_brkAnm mBrkAnm[5];
    /* 0x69C */ dPa_smokeEcallBack mSmoke;
    /* 0x6BC */ be<s32> mStaffId;
    /* 0x6C0 */ be<s32> mTimer;
    /* 0x6C4 */ be<s32> M_demo_idx;
    /* 0x6C8 */ be<s32> mCurModel;
    /* 0x6CC */ be<s16> mEventIdx;
    /* 0x6CE */ be<u8> mState;
    /* 0x6CF */ be<u8> mInit;
    /* 0x6D0 */ gptr<dBgW> M_bgw;
};
WWHD_OFFSET(daObjVgnfd_c, mBtkAnm, 0x3C8);
WWHD_OFFSET(daObjVgnfd_c, mBrkAnm, 0x444);
WWHD_OFFSET(daObjVgnfd_c, mStaffId, 0x6BC);
WWHD_SIZE(daObjVgnfd_c, 0x6D4);

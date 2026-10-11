// Soft soil and Makar tree.
#pragma once
#include "gc_api/gc_api.h"
namespace daObjVmc {
struct Act_c : fopAc_ac_c {
    enum Prm_e { PRM_SWSAVE_W = 8, PRM_SWSAVE_S = 8 };
    bool create_heap();
    void CreateInit();
    cPhs_State _create();
    bool _delete();
    void set_mtx();
    void init_mtx();
    void daObjVmc_base_main();
    void daObjVmc_tree_demo_wait();
    void daObjVmc_tree_demo_main();
    void daObjVmc_tree_main();
    bool _execute();
    bool _draw();
    /* 0x3AC */ request_of_phase_process_class mPhs;
    /* 0x3B4 */ gptr<dBgW> mpBgBase;
    /* 0x3B8 */ gptr<dBgW> mpBgTree;
    /* 0x3BC */ gptr<J3DModel> mModelBase;
    /* 0x3C0 */ gptr<J3DModel> mModelTree;
    /* 0x3C4 */ mDoExt_bckAnm mBckAnmGrow;
    /* 0x450 */ mDoExt_bckAnm mBckAnmHookshot;
    /* 0x4DC */ dCcD_Stts mStts;
    /* 0x518 */ dCcD_Cyl mCyl;
    /* 0x648 */ Mtx34 mMtxBase;
    /* 0x678 */ Mtx34 mMtxTree;
    /* 0x6A8 */ dPa_smokeEcallBack mSmoke;
    /* 0x6C8 */ be<u8> mLinkRangeCheck;
    /* 0x6C9 */ u8 _6C9[3];
    /* 0x6CC */ be<s32> mState;
    /* 0x6D0 */ be<s32> mTimer;
    /* 0x6D4 */ be<u8> mHookshotAnim;
    /* 0x6D5 */ be<u8> mHasTreeBg;
    /* 0x6D6 */ be<u8> mHasTree;
    /* 0x6D7 */ u8 _6D7;
};
WWHD_OFFSET(Act_c, mBckAnmGrow, 0x3C4);
WWHD_OFFSET(Act_c, mCyl, 0x518);
WWHD_OFFSET(Act_c, mState, 0x6CC);
WWHD_SIZE(Act_c, 0x6D8);
} // namespace daObjVmc

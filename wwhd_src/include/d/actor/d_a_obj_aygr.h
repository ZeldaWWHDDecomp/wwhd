// Lookout tower and optional ladder; WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

// HD: local view of the background model's lock flag beyond its shared prefix.
struct daObjAygr_bgW_c : dBgW {
    /* 0x04 */ u8 _04[0x6C - 4];
    /* 0x6C */ be<u8> mFlags;
    enum { LOCK_e = 0x80 };
    void SetLock() { mFlags = mFlags | LOCK_e; }
};
WWHD_OFFSET(daObjAygr_bgW_c, mFlags, 0x6C);

namespace daObjAygr {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c();
    enum Prm_e { PRM_MDL_W = 1, PRM_MDL_S = 0 };
    u8 prm_get_mdl();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(gptr<Mtx34>* pMtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;      /* tower */
    /* 0x3EC */ gptr<J3DModel> mpModel2;     /* ladder (prm_get_mdl() != 0) */
    /* 0x3F0 */ gptr<dBgW> mpBgW2;           /* ladder collision */
    /* 0x3F4 */ Mtx34 mBgMtx2;
    /* 0x424 */ be<u8> mBgW2Regist;          /* mpBgW2 is set up (released in Mthd_Delete) */
    /* 0x425 */ u8 _425[3];
};
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mpBgW2, 0x3F0);
WWHD_OFFSET(Act_c, mBgW2Regist, 0x424);
WWHD_SIZE(Act_c, 0x428);
}  // namespace daObjAygr

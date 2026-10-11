// Forest Haven poles; WWHD layout.
#pragma once
#include "gc_api/gc_api.h"

// HD: local view of the lock byte beyond dBgW's shared registration-id prefix.
struct daObjOjtree_bgW_c : dBgW {
    /* 0x04 */ u8 _04[0x6C - 4];
    /* 0x6C */ be<u8> mFlags;
    enum { LOCK_e = 0x80 };
    void SetLock() { mFlags = mFlags | LOCK_e; }
};
WWHD_OFFSET(daObjOjtree_bgW_c, mFlags, 0x6C);

namespace daObjOjtree {
class Act_c : public dBgS_MoveBgActor {
public:
    Act_c();
    BOOL Delete();
    BOOL CreateHeap();
    BOOL Create();
    cPhs_State Mthd_Create();
    BOOL Mthd_Delete();
    void set_mtx();
    void init_mtx();
    BOOL Execute(gptr<Mtx34>* pMtx);
    BOOL Draw();

    /* 0x3E0 */ request_of_phase_process_class mPhs;
    /* 0x3E8 */ gptr<J3DModel> mpModel;
    /* 0x3EC */ be<u8> mLockTimer;
    /* 0x3ED */ u8 _3ED[3];
};
WWHD_SIZE(Act_c, 0x3F0);
WWHD_OFFSET(Act_c, mpModel, 0x3E8);
WWHD_OFFSET(Act_c, mLockTimer, 0x3EC);
}  // namespace daObjOjtree

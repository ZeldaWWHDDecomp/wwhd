// Forsaken Fortress barricade.
#pragma once
#include "gc_api/gc_api.h"

class daObj_MjDoor_c : public fopAc_ac_c {
  public:
    enum Mode { MODE_WAIT, MODE_DELETE };
    void set_mtx();
    BOOL _createHeap();
    void getArg();
    void CreateInit();
    cPhs_State _create();
    // HD: these entry points preserve their full-width return values.
    BOOL _delete();
    BOOL _execute();
    BOOL _draw();
    void setCollision();
    void modeWaitInit();
    void modeWait();
    void smoke_set();
    void modeDeleteInit();
    void modeDelete();
    /* 0x3AC */ be<s32> mMode;
    /* 0x3B0 */ cXyz mPos;
    /* 0x3BC */ csXyz mAngle;
    /* 0x3C2 */ u8 _3C2[2];
    /* 0x3C4 */ dPa_smokeEcallBack mSmoke;
    /* 0x3E4 */ be<s32> mSmokeTimer;
    /* 0x3E8 */ be<u8> mSwitchNo;  /* was field_0x2CC: getArg 0236F3BC: param & 0xFF; _create 0236F580 creates nothing when that switch is on; modeDeleteInit 0236FAAC sets it (tww the same) */
    /* 0x3E9 */ u8 _3E9[3];
    /* 0x3EC */ gptr<J3DModel> mpModel;
    /* 0x3F0 */ request_of_phase_process_class mPhs;
    /* 0x3F8 */ gptr<dBgW> mpBgW;
    /* 0x3FC */ Mtx34 mMtx;
    /* 0x42C */ be<s32> mDeleteTimer;
    /* 0x430 */ dCcD_Stts mStts;
    /* 0x46C */ dCcD_Cyl mCyl[10];
};
WWHD_OFFSET(daObj_MjDoor_c, mCyl, 0x46C);
WWHD_SIZE(daObj_MjDoor_c, 0x104C);

// HD: host IO has a guest vtable word rather than a native C++ virtual base.
struct daObj_MjDoorHIO_c {
    be<u32> __vtbl;
    be<u8> mForceBreak;  /* was field_0x4: modeWait 0236FCD8 breaks the door (modeDeleteInit) when it is set as when health < 0; MjDoor_hioCtor 0236FDB8 sets 0, never written elsewhere in WWHD or tww */
    u8 _05[3];
};

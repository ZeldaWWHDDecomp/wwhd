// Tower of the Gods jail door. HD: actor fields shift from GameCube 0x290 to 0x3AC.
#pragma once
#include "gc_api/gc_api.h"

class daAtdoor_c : public fopAc_ac_c {
public:
    daAtdoor_c();
    void setAction(u8 action) { mAction = action; }

    u8 getSwbit();
    BOOL CreateHeap();
    void calcMtx();
    bool CreateInit();
    cPhs_State create();

    /* 0x3AC */ request_of_phase_process_class mPhase; /* GameCube 0x290 */
    /* 0x3B4 */ gptr<J3DModel> mpModel;
    /* 0x3B8 */ gptr<dBgW> mpBgW;
    /* 0x3BC */ be<u8> mAction;
    /* 0x3BD */ u8 _3BD;
    /* 0x3BE */ be<s16> unk_2A2;
};
WWHD_OFFSET(daAtdoor_c, mpModel, 0x3B4);
WWHD_OFFSET(daAtdoor_c, unk_2A2, 0x3BE);
WWHD_SIZE(daAtdoor_c, 0x3C0);


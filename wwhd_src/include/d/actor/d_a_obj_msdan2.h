/* Obj_Msdan2: the controller for a row of sixteen staircase steps.
 * GameCube -> WWHD: actor members move by +0x11C (base size 0x3AC).
 * HD adds a C++ vtable in the actor base. */
#pragma once
#include "gc_api/gc_api.h"

namespace daObjMsdan2 {
class Act_c : public fopAc_ac_c {
public:
    enum Prm_e {
        PRM_SWSAVE_W = 8,
        PRM_SWSAVE_S = 0,
    };
    enum Mode_e {
        MODE_WAIT_e = 0,
        MODE_ORDER_e = 1,
        MODE_EVENT_e = 2,
        MODE_END_e = 3,
    };
    enum : u32 { __vt = 0x1002D828 }; // HD: actor vtable

    Act_c();
    int prm_get_swSave();
    cPhs_State Mthd_Create();
    BOOL Mthd_Execute();
    BOOL Mthd_Delete() { return TRUE; }

public:
    /* 0x3AC */ u8 field_0x290[8]; // unused, GameCube name
    /* 0x3B4 */ be<s16> mEventIdx;
    /* 0x3B6 */ u8 _3B6[2];
    /* 0x3B8 */ be<s32> mMode;
};
WWHD_OFFSET(Act_c, mEventIdx, 0x3B4);
WWHD_OFFSET(Act_c, mMode, 0x3B8);
WWHD_SIZE(Act_c, 0x3BC);
}

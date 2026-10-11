/* All-enemies-dead switch actor, WWHD layout.
 * GameCube -> WWHD: fopAc_ac_c grows by 0x11C; the actor members follow it.
 */
#pragma once
#include "bindings.h"

enum {
    ACT_WAIT,
    ACT_CHECK,
    ACT_TIMER,
};

u32 fopAcM_myRoomSearchEnemy(s8 roomNo);

class daAlldie_c : public fopAc_ac_c {
public:
    enum : u32 { __vt = 0x10006DC4 }; // HD: virtual destructor vtable
    void setActio(u8 action) { mAction = action; }
    u8 getSwbit();
    BOOL actionWait() { return TRUE; } // HD: inlined
    BOOL actionCheck();
    BOOL actionTimer();
    BOOL execute();

    /* 0x3AC */ be<u8> mAction;
    /* 0x3AD */ u8 _3AD;
    /* 0x3AE */ be<s16> mTimer;
};
WWHD_OFFSET(daAlldie_c, mAction, 0x3AC);
WWHD_OFFSET(daAlldie_c, mTimer, 0x3AE);
WWHD_SIZE(daAlldie_c, 0x3B0);

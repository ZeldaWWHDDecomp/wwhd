/* Frame counters, WWHD layout (unchanged from GameCube). */
#pragma once
#include "bindings.h"

struct counter_class {
    /* 0x00 */ be<u32> mCounter0;
    /* 0x04 */ be<s32> mCounter1;
    /* 0x08 */ be<u32> mTimer;
};
WWHD_OFFSET(counter_class, mCounter0, 0x00);
WWHD_OFFSET(counter_class, mCounter1, 0x04);
WWHD_OFFSET(counter_class, mTimer, 0x08);
WWHD_SIZE(counter_class, 0x0C);

void cCt_Counter(int resetCounter1);

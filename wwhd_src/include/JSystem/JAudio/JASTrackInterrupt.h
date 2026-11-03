/* JAudio 1 interrupt manager; Cafe layout matches the GameCube layout. */
#pragma once
#include "bindings.h"
namespace JASystem {
class TIntrMgr {
public:
    enum { INTERRUPT_COUNT = 8, TIMER_INTERRUPT = 6 };
    void init();
    void request(u32 interrupt);
    void setIntr(u32 interrupt, void* handler);
    void resetInter(u32 interrupt);
    void* checkIntr();
    void timerProcess();
    /* 0x00 */ be<u8> mEnabled;
    /* 0x01 */ be<u8> mPending;
    /* 0x02 */ be<u8> mMask;
    /* 0x03 */ be<u8> mTimerRepeats;
    /* 0x04 */ be<u32> mTimerRemaining;
    /* 0x08 */ be<u32> mTimerInterval;
    /* 0x0C */ gptr<void> mHandlers[INTERRUPT_COUNT];
};
WWHD_OFFSET(TIntrMgr, mEnabled, 0);
WWHD_OFFSET(TIntrMgr, mPending, 1);
WWHD_OFFSET(TIntrMgr, mMask, 2);
WWHD_OFFSET(TIntrMgr, mTimerRepeats, 3);
WWHD_OFFSET(TIntrMgr, mTimerRemaining, 4);
WWHD_OFFSET(TIntrMgr, mTimerInterval, 8);
WWHD_OFFSET(TIntrMgr, mHandlers, 0xC);
WWHD_SIZE(TIntrMgr, 0x2C);
}
struct InterruptHeaderStatics { be<u32> mWords[4]; };
struct InterruptDestructorRecord { be<u32> mWords[3]; };
void registerInterruptStatics(InterruptDestructorRecord*);

/* Based on CC0 tww JASTrackInterrupt.cpp; fields named for their interrupt/timer roles. */
#include "JSystem/JAudio/JASTrackInterrupt.h"
using namespace JASystem;
static InterruptHeaderStatics headerStatics;
static InterruptDestructorRecord destructorRecord;
namespace JASystem {
/* 02819FBC */
void TIntrMgr::init() {
    mTimerRemaining = 0;
    mMask = 0;
    mPending = 0;
    mEnabled = 1;
    mTimerRepeats = 0;
    mTimerInterval = 0;
    for (u32 i = 0; i < INTERRUPT_COUNT; ++i) mHandlers[i] = nullptr;
}
/* 02819FF4 */
void* TIntrMgr::checkIntr() {
    if (!mEnabled) return nullptr;
    u32 pending = mMask & mPending;
    for (u32 i = 0; pending; ++i, pending >>= 1) {
        if (pending & 1) {
            mPending = mPending & ~(1u << i);
            return mHandlers[i];
        }
    }
    return nullptr;
}
/* 0281A054 */
void TIntrMgr::request(u32 interrupt) {
    // Espresso shifts use the low six bits and produce zero for shifts of 32 or more.
    u32 bit = (interrupt & 63) < 32 ? 1u << (interrupt & 31) : 0;
    if (mMask & bit) mPending = mPending | bit;
}
/* 0281A078 */
void TIntrMgr::timerProcess() {
    if (!mTimerRemaining) return;
    mTimerRemaining = mTimerRemaining - 1;
    if (mTimerRemaining) return;
    request(TIMER_INTERRUPT);
    if (mTimerRepeats) {
        mTimerRepeats = mTimerRepeats - 1;
        if (!mTimerRepeats) return;
    }
    mTimerRemaining = mTimerInterval;
}
/* 0281A0D8 */
void TIntrMgr::setIntr(u32 interrupt, void* handler) {
    u32 bit = (interrupt & 63) < 32 ? 1u << (interrupt & 31) : 0;
    mMask = mMask | bit;
    mHandlers[interrupt] = handler;
}
/* 0281A0FC */
void TIntrMgr::resetInter(u32 interrupt) {
    u32 bit = (interrupt & 63) < 32 ? 1u << (interrupt & 31) : 0;
    mMask = mMask & ~bit;
}
}
/* 0281A114 */
void __sinit_JASTrackInterrupt_cpp() {
    headerStatics.mWords[2] = 0;
    headerStatics.mWords[0] = 0;
    headerStatics.mWords[3] = 0;
    headerStatics.mWords[1] = 0;
    registerInterruptStatics(&destructorRecord);
}
/* WWHD glue
data headerStatics 104B5690
data destructorRecord 101F9FD0
ext 028F026C void registerInterruptStatics(InterruptDestructorRecord*)
*/

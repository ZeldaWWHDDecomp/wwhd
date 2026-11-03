// Cafe sequence cursor, loop and interrupt state, reconstructed from the WWHD binary.
#include "JSystem/JAudio/JASSeqCtrl_extra.h"
using namespace JASystem;
struct Initialization { be<u32> words[4]; };
struct Registration { be<u32> words[3]; };
static Initialization sInitialization;
static Registration sRegistration;
extern void registerSeqCtrlGlobals(void*);
/* 028458D4 */
void TSeqCtrl::init() {
    mCurrentFilePtr = nullptr;
    mRawFilePtr = nullptr;
    mWaitTimer = 0;
    mLoopIndex = 0;
    for (u32 i = 0; i < 8; ++i) {
        mLoopStartPositions[i] = nullptr;
        mLoopTimers[i] = 0;
    }
    mSavedWaitTimer = 0;
    mPreviousFilePtr = nullptr;
}
/* 02845910 */
void TSeqCtrl::start(void* data, u32 offset) {
    mRawFilePtr = data;
    mCurrentFilePtr = static_cast<u8*>(data) + offset;
}
/* 02845920 */
u32 TSeqCtrl::read16() {
    auto current = static_cast<be<u8>*>(mCurrentFilePtr.get());
    mCurrentFilePtr = current + 1;
    u32 high = current[0];
    mCurrentFilePtr = current + 2;
    u32 low = current[1];
    return (high << 8) | low;
}
/* 02845948 */
u32 TSeqCtrl::read24() {
    auto current = static_cast<be<u8>*>(mCurrentFilePtr.get());
    mCurrentFilePtr = current + 1;
    u32 value = current[0];
    mCurrentFilePtr = current + 2;
    value = (value << 8) | (u32)current[1];
    mCurrentFilePtr = current + 3;
    return (value << 8) | (u32)current[2];
}
/* 02845984 */
u32 TSeqCtrl::get16(u32 offset) const {
    auto raw = static_cast<be<u8>*>(mRawFilePtr.get());
    u32 high = raw[offset++];
    u32 low = raw[offset];
    return (high << 8) | low;
}
/* 028459A0 */
u32 TSeqCtrl::get24(u32 offset) const {
    auto raw = static_cast<be<u8>*>(mRawFilePtr.get());
    u32 value = raw[offset++];
    value = (value << 8) | (u32)raw[offset++];
    return (value << 8) | (u32)raw[offset];
}
/* 028459CC */
u32 TSeqCtrl::get32(u32 offset) const {
    auto raw = static_cast<be<u8>*>(mRawFilePtr.get());
    u32 value = raw[offset++];
    value = (value << 8) | (u32)raw[offset++];
    value = (value << 8) | (u32)raw[offset++];
    return (value << 8) | (u32)raw[offset];
}
/* 02845A08 */
u32 TSeqCtrl::loopEnd() {
    u32 index = mLoopIndex;
    if (index == 0) return 0;
    u16 timer = mLoopTimers[index - 1];
    if (timer == 0 || --timer == 0) {
        mLoopIndex = index - 1;
    } else {
        mLoopTimers[index - 1] = timer;
        mCurrentFilePtr = mLoopStartPositions[(u32)mLoopIndex - 1];
    }
    return 1;
}
/* 02845A6C */
u32 TSeqCtrl::waitCountDown() {
    s32 wait = mWaitTimer;
    if (wait > 0) {
        --wait;
        mWaitTimer = wait;
        if (wait != 0)
            return 0;
    }
    return 1;
}
/* 02845A94 */
u32 TSeqCtrl::callIntr(void* handler) {
    if (mPreviousFilePtr)
        return 0;
    s32 wait = mWaitTimer;
    void* current = mCurrentFilePtr;
    mSavedWaitTimer = wait;
    mPreviousFilePtr = current;
    mCurrentFilePtr = handler;
    mWaitTimer = 0;
    return 1;
}
/* 02845AD0 */
u32 TSeqCtrl::retIntr() {
    if (!mPreviousFilePtr) return 0;
    s32 wait = mSavedWaitTimer;
    void* previous = mPreviousFilePtr;
    mPreviousFilePtr = nullptr;
    mCurrentFilePtr = previous;
    mWaitTimer = wait;
    return 1;
}
/* 02845B04 */
void __sinit_JASSeqCtrl_cpp() {
    sInitialization.words[2] = 0;
    sInitialization.words[0] = 0;
    sInitialization.words[3] = 0;
    sInitialization.words[1] = 0;
    registerSeqCtrlGlobals(&sRegistration);
}
/* WWHD glue
data sInitialization 104B6664
data sRegistration 101FC708
ext 028F026C void registerSeqCtrlGlobals(void*)
*/

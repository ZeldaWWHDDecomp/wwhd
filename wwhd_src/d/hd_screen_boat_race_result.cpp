/* HD boat-race result timing. */
#include "d/hd_minigame_hud.h"
static MinigameState resultRollState;

/* 02627714 */
void hdScreenBoatRaceResult_c::wait() {
    // HD: retain unsigned countdown wrap when the timer starts at zero.
    u32 n = u32(mWaitTimer) - 1;
    mWaitTimer = n;
    if (n == 0) minigameChangeState(&mStateMachine, &resultRollState);
}

/* 02627A80 */
void hdScreenBoatRaceResult_c::roll() {
    gfn* vtable = mVtable;
    gabi::call_ptr<void>(vtable[Minigame_ScreenRollSlot].get(), this);
    u32 n = mRollFrame;
    if (n == 0) {
        minigamePlaySound(Minigame_ResultStartSound);
        n = mRollFrame;
    } else if (n == 20 && mResult == Minigame_ResultSecondPrize) {
        minigamePlaySound(Minigame_ResultPrizeSound);
        n = mRollFrame;
    }
    // HD: reload the frame after sound callbacks before incrementing.
    mRollFrame = n + 1;
}

/* WWHD glue
data resultRollState 1048F210
ext 025E1988 void minigamePlaySound(u32)
*/

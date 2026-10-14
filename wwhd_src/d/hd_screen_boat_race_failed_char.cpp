/* HD boat-race failure animation. */
#include "d/hd_minigame_hud.h"
static MinigameState failedIdleState;

/* 02626CD8 */
void hdScreenBoatRaceFailedChar_c::wait() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    u32 n = mWaitTimer;
    if (n != 0) {
        mWaitTimer = n - 1;
        if (n - 1 != 0) return;
        // HD: the animation-name pair occupies the original frame slot.
        gabi::FrameLocal<MinigameAnimationNames> names(8);
        names->animation = gabi::guest_addr("boat race failure animation");
        names->group = gabi::guest_addr("boat race failure group");
        minigamePlayAnimation(mScene, names.get());
    } else if (!mScene->mActive) {
        minigameChangeState(&mStateMachine, &failedIdleState);
    }
}

/* WWHD glue
data failedIdleState 1048F0F0
str 100E570C "boat race failure animation" renamed
str 100E565C "boat race failure group" renamed
ext 026F5258 void minigamePlayAnimation(MinigameAnimationScene*, MinigameAnimationNames*)
ext 020063C0 void minigameChangeState(MinigameStateMachine*, MinigameState*)
*/

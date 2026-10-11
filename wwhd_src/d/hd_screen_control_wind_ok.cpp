/** HD wind-control waiting state; private decomp source. */
#include "bindings.h"
#include "d/hd_screen_command_a.h"
static hdHudEventManager* sEventManager;
static hdHudStateDescriptor sWindReadyState;

/* 0264F09C */
static void waitWindControl(hdHudWindControl* i_this) {
    // HD: the verified final stack range includes the original saved-register area.
    auto frame = gabi::AutoFrame(0x0264F09C);
    // HD: the copied event must occupy the original frame slot for its complete 40-byte result.
    gabi::FrameLocal<hdHudInputEvent> event(8);
    hdHudCopyInputEvent(event.get(), &sEventManager->mCurrentEvent);
    u32 timer = i_this->mWaitTimer;
    u32 forced = i_this->mForced;
    if (timer != 0) {
        timer = u8(timer - 1);
        i_this->mWaitTimer = timer;
    }
    if (forced || timer == 0 || hdHudIsWindEvent(event->mType)) {
        i_this->mReady = 1;
        hdHudChangeState(&i_this->mStateMachine, &sWindReadyState);
    }
}

/* WWHD glue (tools/verify/autoglue.py): HD wind event and state
 data sEventManager 101F4FF0
 data sWindReadyState 10491C00
 ext 020013C0 void hdHudCopyInputEvent(hdHudInputEvent*, hdHudInputEvent*)
 ext 026F620C u32 hdHudIsWindEvent(u32)
*/

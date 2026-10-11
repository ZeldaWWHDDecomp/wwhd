/** HD command-guide waiting state; private decomp source. */
#include "bindings.h"
#include "d/hd_screen_command_a.h"
static u8 sCommandGuideFlag;
static hdHudStateDescriptor sGuideActiveState;

/* 026489F8 */
static u32 getCommandGuideFlag(hdHudCommandGuide*) {
    return sCommandGuideFlag;
}

/* 02648A04 */
static void waitCommandGuide(hdHudCommandGuide* i_this) {
    if (i_this->canShow()) {
        u32 timer = i_this->mWaitTimer;
        if (timer != 0) {
            i_this->mWaitTimer = timer - 1;
            if (timer == 1) {
                i_this->mCachedFlag = getCommandGuideFlag(i_this);
                i_this->mCachedCanShow = i_this->canShow();
                hdHudChangeState(&i_this->mStateMachine, &sGuideActiveState);
            }
        } else {
            i_this->mWaitTimer = 3;
        }
    } else {
        i_this->mWaitTimer = 0;
    }
    i_this->updateCommand();
    i_this->update();
}

/* WWHD glue (tools/verify/autoglue.py): HD guide state
 data sCommandGuideFlag 1047B07A
 data sGuideActiveState 10491670
*/

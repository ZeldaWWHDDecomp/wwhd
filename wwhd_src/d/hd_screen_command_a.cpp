/** HD A-button command visibility and waiting states; private decomp source. */
#include "bindings.h"
#include "d/hd_screen_command_a.h"
static hdHudStateDescriptor sCommandActiveState;
static hdHudStateDescriptor sCommandWaitState;

/* 02647CC0 */
static u32 isCommandAReady(hdHudCommandA* i_this) {
    if (i_this->mEnabled == 0) {
        return 0;
    }
    if (dComIfGp_getEvent()->runCheck()) {
        return 0;
    }
    dComIfG_inf_c* game = (dComIfG_inf_c*)dComIfGp_get();
    if (game->play.mPlayerStatus[0][0] & 0x11) {
        i_this->mWaitTimer = 1;
        return 0;
    }
    return 1;
}

/* 02647D50 */
static void waitCommandA(hdHudCommandA* i_this) {
    if (!isCommandAReady(i_this)) {
        return;
    }
    u32 timer = i_this->mWaitTimer;
    if (timer != 0) {
        i_this->mWaitTimer = timer - 1;
    } else {
        hdHudChangeState(&i_this->mStateMachine, &sCommandActiveState);
    }
}

/* 02647E48 */
static void updateCommandAWait(hdHudCommandA* i_this) {
    u32 timer = i_this->mWaitTimer;
    if (timer != 0) {
        i_this->mWaitTimer = timer - 1;
    } else if (!isCommandAReady(i_this)) {
        hdHudChangeState(&i_this->mStateMachine, &sCommandWaitState);
    }
    i_this->update();
}

/* WWHD glue (tools/verify/autoglue.py): HD command states
 data sCommandActiveState 1049118C
 data sCommandWaitState 104911EC
 ext 020063C0 void hdHudChangeState(hdHudStateMachine*, hdHudStateDescriptor*)
*/

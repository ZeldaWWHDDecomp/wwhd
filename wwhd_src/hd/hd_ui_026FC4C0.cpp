/* HD message states. */
#include "d/hd_msg_window_base.h"
using namespace gabi;
static WindowState l_windowState2_DECC;
static WindowState l_windowState2_DEFC;
extern WindowPlay *hdMsgWaitGetPlay();
extern u32 windowWaitOverride();
extern void hdMsgWaitChangeState(be<u32> *, WindowState *);

/* 026FC584 */
void hdMsgWaitScreen_updateFirst(WindowBase *self) {
    u32 timer = self->mWaitTimer - 1;
    self->mWaitTimer = timer;
    if (s32(timer) >= 0)
        return;
    s32 index = hdMsgWaitGetPlay()->mControllerIndex;
    WindowPlay *play = hdMsgWaitGetPlay();
    if (!(gabi::at<WindowController>(gabi::ea(play) + u32(index * 0x34) + 0x5B00)->mFlags & 4) &&
        !windowWaitOverride())
        return;
    u32 value = hdMsgWaitGetPlay()->mWaitValue;
    self->mWaitValue = value;
    hdMsgWaitChangeState(&self->mStateMachine, &l_windowState2_DECC);
}

/* 026FC764 */
void hdMsgWaitScreen_updateSecond(WindowBase *self) {
    u32 timer = self->mWaitTimer - 1;
    self->mWaitTimer = timer;
    if (s32(timer) >= 0)
        return;
    s32 index = hdMsgWaitGetPlay()->mControllerIndex;
    WindowPlay *play = hdMsgWaitGetPlay();
    if (!(gabi::at<WindowController>(gabi::ea(play) + u32(index * 0x34) + 0x5B00)->mFlags & 4) &&
        !windowWaitOverride())
        return;
    u32 value = hdMsgWaitGetPlay()->mWaitValue;
    self->mWaitValue = value;
    hdMsgWaitChangeState(&self->mStateMachine, &l_windowState2_DEFC);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_windowState2_DECC 1049DECC
data l_windowState2_DEFC 1049DEFC
ext 025200D4 WindowPlay* hdMsgWaitGetPlay()
ext 025DBB40 u32 windowWaitOverride()
ext 020063C0 void hdMsgWaitChangeState(be<u32>*,WindowState*)
*/

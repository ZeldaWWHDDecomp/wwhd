#include "d/hd_msg_window_a.h"
using namespace gabi;
static HdMessageCorePointer l_OnlyCore;
static u8 l_OnlyWaitIcon;
static HdMessageState l_OnlyClosedState;
static HdMessageState l_OnlyOpenState;
static HdMessageState l_OnlyWaitState;
static HdMessageState l_OnlyWaitDoneState;
static HdMessageState l_OnlyStopAdvanceState;
static HdMessageState l_OnlyInputAdvanceState;
static HdMessageState l_OnlyContinueState;
static HdMessageState l_OnlyAdvanceState;
static HdMessageState l_OnlyWaitInputState;
static HdMessageManagerPointer l_manager;
static f32 l_zero;
static f32 l_fastSpeed;
static f32 l_one;
extern u32 hdMessageAnimationEnded(HdMessageOnly*,u32);

static void change(HdMessageOnly* self, HdMessageState* state) { hdMessageChangeState(&self->mState, state); }
static void refreshWait(HdMessageOnly* self) {
    HdMessageData* message = self->mMessage;
    HdMessagePane* pane = self->mWaitPane;
    hdMessageRefresh(pane, message);
}

// HD: inline typing accumulator, fast-forward and message status dispatch.
/* 026AEFF4 */
void hdMessageOnlyOpenUpdate(HdMessageOnly* self) {
    u32 flags = self->mFlags;
    HdMessageCore* core = l_OnlyCore.mCore;
    f32 zero = l_zero;
    HdMessageData* message = self->mMessage;
    u32 count = 0;
    bool showAll = false;
    if (flags & HD_MESSAGE_SHOW_ALL) {
        HdMessageText* text = message->mText;
        if (text && message->mText->mWait == 0) {
            self->mShown = HD_MESSAGE_ALL_SHOWN;
            self->mMessage->mTypingPause = 0;
            self->mTypingFraction = zero;
            count = 1;
            showAll = true;
        }
    }
    if (!showAll) {
        if (message->mTypingPause != 0) {
            s32 pause = message->mTypingPause;
            message->mTypingPause = pause > 0 ? u32(pause) - 1 : 0;
            self->mTypingFraction = zero;
            return;
        }
        f32 speed = core->mTypingSpeed;
        if (flags & HD_MESSAGE_FAST) {
            HdMessageText* text = message->mText;
            if (text && message->mText->mWait == 0)
                speed = fmuls_ppc(speed, l_fastSpeed);
        }
        f32 fraction = fadds_ppc(self->mTypingFraction, speed);
        f32 one = l_one;
        if (fraction >= one) {
            do {
                fraction = fsubs_ppc(fraction, one);
                ++count;
            } while (fraction >= one);
        }
        self->mTypingFraction = fraction;
        if (!count) return;
    }
    s32 shown = self->mShown;
    if (shown < HD_MESSAGE_ALL_SHOWN) {
        u32 next = self->mStopIndex + 1;
        // HD: an out-of-range stop index selects the first slot.
        be<s32>* stop = &self->mStops[next<10?next:0];
        shown = s32(u32(shown) + count);
        self->mShown = u32(shown);
        if (shown > (s32)*stop) self->mShown = (u32)*stop;
    }
    call_ptr<void>(self->mVTable[HD_MESSAGE_STEP_TEXT].get(), self);
    message = self->mMessage;
    if (message->mBusy) return;
    u32 status = message->mStatus;
    if (status == HD_MESSAGE_CONTINUE) {
        hdMessageSetMode(core, message->mStatus);
        change(self, &l_OnlyContinueState);
    } else if (status == HD_MESSAGE_WAIT) {
        hdMessageSetMode(core, message->mStatus);
        message = self->mMessage;
        if (message->mWait) change(self, &l_OnlyWaitState);
        else if (message->mWaitInput) change(self, &l_OnlyWaitInputState);
    } else if (status == HD_MESSAGE_ADVANCE) {
        hdMessageSetMode(core, message->mStatus);
        change(self, &l_OnlyAdvanceState);
    }
}

/* 026AF2E4 */
void hdMessageOnlyWaitUpdate(HdMessageOnly* self) {
    HdMessageData* message = self->mMessage;
    if (message->mWaitTimer == 0) {
        message->mWait = 0;
        change(self, &l_OnlyWaitDoneState);
    } else {
        s32 timer = message->mWaitTimer;
        message->mWaitTimer = timer > 0 ? message->mWaitTimer - 1 : 0;
        refreshWait(self);
    }
}

/* 026AF338 */
void hdMessageOnlyWaitInputUpdate(HdMessageOnly* self) {
    HdMessageData* message = self->mMessage;
    if (message->mWaitTimer == 0) {
        message->mWaitInput = 0;
        l_OnlyWaitIcon = 0;
        u32 n = self->mAdvanceCount + 1;
        self->mAdvanceCount = n;
        hdMessageGetPlay()->mAdvanceCount = u8(n);
        change(self, &l_OnlyWaitDoneState);
        return;
    }
    if (self->mInputFlags & 3) {
        HdMessagePlay* play = hdMessageGetPlay();
        u8 blocked = play->mInputBlocked;
        message = self->mMessage;
        if (!blocked) {
            message->mWaitInput = 0;
            l_OnlyWaitIcon = 0;
            change(self, &l_OnlyInputAdvanceState);
            return;
        }
    }
    s32 timer = message->mWaitTimer;
    message->mWaitTimer = timer > 0 ? u32(timer) - 1 : 0;
    refreshWait(self);
    l_OnlyWaitIcon = 4;
}

/* 026AF4E0 */
void hdMessageOnlyStopUpdate(HdMessageOnly* self) {
    if (self->mInputFlags & 3) {
        u32 n = self->mAdvanceCount + 1;
        self->mAdvanceCount = n;
        hdMessageGetPlay()->mAdvanceCount = u8(n);
        l_OnlyWaitIcon = 2;
        change(self, &l_OnlyStopAdvanceState);
    } else {
        HdMessagePane* pane = self->mWaitPane;
        HdMessageData* message = self->mMessage;
        hdMessageRefresh(pane, message);
        l_OnlyWaitIcon = 1;
    }
}

/* 026AF5DC */
void hdMessageOnlyCloseUpdate(HdMessageOnly* self) {
    if (!hdMessageAnimationEnded(self, 5u)) return;
    hdMessageSetMode(l_OnlyCore.mCore, 0u);
    HdMessageManager* manager = l_manager.mManager;
    manager->mWindowCount = manager->mWindowCount-1;
    call_ptr<void>(self->mVTable[HD_MESSAGE_CLOSE_FIRST].get(), self);
    call_ptr<void>(self->mVTable[HD_MESSAGE_CLOSE_SECOND].get(), self);
    hdMessageGetPlay()->mWindowSlot = 0xFF;
    hdMessageGetPlay()->mMessageId = 0xFFFFFFFF;
    hdMessageGetPlay()->mWindowActive = 0;
    hdMessageGetPlay()->mMessageFlags = 0;
    change(self, &l_OnlyClosedState);
}

/* 026AF6D8 */
void hdMessageOnlyNextStopUpdate(HdMessageOnly* self) {
    if (!call_ptr<u32>(self->mVTable[HD_MESSAGE_NEXT_STOP].get(), self)) return;
    u32 next = self->mStopIndex + 1;
    HdMessageData* message = self->mMessage;
    self->mStopIndex = next;
    self->mShown = 0;
    u32 color = message->mColor;
    HdMessagePane* pane = self->mPane;
    pane->mColor1 = color;
    pane->mColor2 = color;
    self->mStopFlag = 0;
    call_ptr<void>(self->mVTable[HD_MESSAGE_STEP_TEXT].get(), self);
    change(self, &l_OnlyOpenState);
}

/* 026AF93C */
void hdMessageOnlyAdvanceUpdate(HdMessageOnly* self) {
    call_ptr<void>(self->mVTable[HD_MESSAGE_ADVANCE_TEXT].get(), self);
    HdMessagePlay* play = hdMessageGetPlay();
    hdMessageSound(play->mKind == 39 ? 0x880u : 0x80Bu);
    u32 n = self->mAdvanceCount + 1;
    self->mAdvanceCount = n;
    hdMessageGetPlay()->mAdvanceCount = u8(n);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_OnlyCore 101F4B5C
data l_OnlyWaitIcon 1047B09E
data l_OnlyClosedState 10497F80
data l_OnlyOpenState 10497FB0
data l_OnlyWaitState 10497FE0
data l_OnlyWaitDoneState 104980A0
data l_OnlyStopAdvanceState 10498100
data l_OnlyInputAdvanceState 10498160
data l_OnlyContinueState 1049DF2C
data l_OnlyAdvanceState 1049E1CC
data l_OnlyWaitInputState 1049E25C
data l_manager 101D5FE8
data l_zero 100FD0A4
data l_fastSpeed 100FD0A8
data l_one 100FD020
ext 020063C0 void hdMessageChangeState(HdMessageStateController*,HdMessageState*)
ext 0270477C void hdMessageRefresh(HdMessagePane*,HdMessageData*)
ext 025F74D0 void hdMessageSetMode(HdMessageCore*,u32)
ext 025200D4 HdMessagePlay* hdMessageGetPlay()
ext 025E1988 void hdMessageSound(u32)
ext 026FF668 u32 hdMessageAnimationEnded(HdMessageOnly*,u32)
*/

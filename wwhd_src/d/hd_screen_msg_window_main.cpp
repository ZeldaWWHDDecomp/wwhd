#include "d/hd_msg_window_a.h"
using namespace gabi;
static HdMessageCorePointer l_MainCore;
static u8 l_MainWaitIcon;
static HdMessageState l_MainNextStopState;
static HdMessageState l_MainAdvanceState;
extern void hdMessageMainSetMode(HdMessageMain*,u32);
extern void hdMessageMainUpdateText(HdMessageMain*);

static void change(HdMessageMain* self, HdMessageState* state) { hdMessageChangeState(&self->mState, state); }
static void refresh(HdMessageMain* self) {
    HdMessagePane* pane = self->mPane;
    HdMessageData* message = self->mMessage;
    hdMessageRefresh(pane, message);
}
static void sound() {
    HdMessagePlay* play = hdMessageGetPlay();
    hdMessageSound(play->mKind == 39 ? 0x880u : 0x80Bu);
}

/* 026B7D8C */
void hdMessageMainAdvanceUpdate(HdMessageMain* self) {
    u32 flags = self->mInputFlags;
    HdMessageCore* core = l_MainCore.mCore;
    if ((flags & 3) || core->mSkip) {
        hdMessageSetMode(core, 14u);
        sound();
        u32 n = self->mAdvanceCount + 1;
        self->mAdvanceCount = n;
        hdMessageGetPlay()->mAdvanceCount = u8(n);
        l_MainWaitIcon = 0;
        change(self, &l_MainAdvanceState);
    } else {
        refresh(self);
        l_MainWaitIcon = 4;
    }
}

/* 026B7E70 */
void hdMessageMainWaitUpdate(HdMessageMain* self) {
    HdMessageData* message = self->mMessage;
    if (message->mWaitTimer == 0 || l_MainCore.mCore->mSkip) {
        message->mWait = 0;
        change(self, &l_MainAdvanceState);
    } else {
        s32 timer = message->mWaitTimer;
        message->mWaitTimer = timer > 0 ? u32(timer) - 1 : 0;
        HdMessageData* msg = self->mMessage;
        HdMessagePane* pane = self->mPane;
        hdMessageRefresh(pane, msg);
    }
}

/* 026B7ED4 */
void hdMessageMainWaitInputUpdate(HdMessageMain* self) {
    HdMessageData* message = self->mMessage;
    if (message->mWaitTimer == 0 || l_MainCore.mCore->mSkip) {
        message->mWaitInput = 0;
        l_MainWaitIcon = 0;
        u32 n = self->mAdvanceCount + 1;
        self->mAdvanceCount = n;
        hdMessageGetPlay()->mAdvanceCount = u8(n);
        change(self, &l_MainAdvanceState);
    } else if (self->mInputFlags & 1) {
        message->mWaitInput = 0;
        sound();
        u32 n = self->mAdvanceCount + 1;
        self->mAdvanceCount = n;
        hdMessageGetPlay()->mAdvanceCount = u8(n);
        l_MainWaitIcon = 0;
        change(self, &l_MainAdvanceState);
    } else {
        s32 timer = message->mWaitTimer;
        message->mWaitTimer = timer > 0 ? message->mWaitTimer - 1 : 0;
        HdMessageData* msg = self->mMessage;
        HdMessagePane* pane = self->mPane;
        hdMessageRefresh(pane, msg);
        l_MainWaitIcon = 4;
    }
}

/* 026B80F8 */
void hdMessageMainNextStopUpdate(HdMessageMain* self) {
    hdMessageMainSetMode(self, 2u);
    u32 next = self->mStopIndex + 1;
    HdMessageData* message = self->mMessage;
    self->mStopIndex = next;
    u32 color = message->mColor;
    HdMessagePane* pane = self->mPane;
    pane->mColor1 = color;
    pane->mColor2 = color;
    hdMessageMainUpdateText(self);
}

/* 026B81CC */
void hdMessageMainStopUpdate(HdMessageMain* self) {
    if (self->mInputFlags & 3) {
        u32 n = self->mAdvanceCount + 1;
        self->mAdvanceCount = n;
        hdMessageGetPlay()->mAdvanceCount = u8(n);
        l_MainWaitIcon = 2;
        u32 next = self->mStopIndex + 1;
        // HD: an out-of-range stop index selects the first slot.
        be<s32>* stop = &self->mStops[next<10?next:0];
        if ((s32)*stop >= 0) change(self, &l_MainNextStopState);
        else {
            hdMessageSetMode(l_MainCore.mCore, 14u);
            change(self, &l_MainAdvanceState);
        }
    } else {
        refresh(self);
        l_MainWaitIcon = 1;
    }
}
/* WWHD glue (tools/verify/autoglue.py)
data l_MainCore 101F4B5C
data l_MainWaitIcon 1047B09E
data l_MainNextStopState 104989B0
data l_MainAdvanceState 10498A40
ext 026B7920 void hdMessageMainSetMode(HdMessageMain*,u32)
ext 026B7830 void hdMessageMainUpdateText(HdMessageMain*)
*/

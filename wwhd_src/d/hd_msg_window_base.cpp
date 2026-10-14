/* HD message states. */
#include "d/hd_msg_window_base.h"
using namespace gabi;
static WindowCorePointer l_windowCore;
static f32 l_windowZero;
static f32 l_windowSpeedup;
static WindowNumberLimits l_windowLimits;
static u8 l_windowInputMode;
static WindowState l_windowState0_E01C;
static WindowState l_windowState0_E07C;
static WindowState l_windowState0_DF8C;
static WindowState l_windowState0_DFBC;
static WindowState l_windowState0_DF2C;
static WindowState l_windowState0_E22C;
static WindowState l_windowState0_E25C;
static WindowState l_windowState0_E2BC;
static WindowState l_windowState0_E19C;
static WindowState l_windowState0_E3AC;
static WindowState l_windowState0_E40C;
static WindowState l_windowState0_DFEC;
static WindowState l_windowState0_E10C;
static WindowState l_windowState0_E49C;
static WindowState l_windowState0_E34C;
static WindowState l_windowState0_E28C;
static WindowState l_windowState0_E43C;
static WindowState l_windowState0_E3DC;
static WindowState l_windowState0_DE9C;
static WindowState l_windowState0_DEFC;
extern void windowSetStatus(WindowCore *, u32);
extern WindowPlay *hdMsgWindowGetPlay();
extern void hdMsgWindowChangeState(be<u32> *, WindowState *);
extern void windowUpdatePane(WindowPane *, WindowMessage *);
extern u32 windowSelectionReady(void *);
extern u32 windowPlaySound0(u32);
extern void windowNumberAccept();
extern void windowNumberInput(WindowBase *, be<u32> *);
extern void windowNumberRefresh(WindowBase *);
static u32 windowVirtual(WindowBase *s, u32 offset) { return s->mpVTable[offset / 4].get(); }

/* 026FCA80 */
void hdMsgWindow_enterOpen(WindowBase *s) {
    s->mTypingAccumulatorBits = *gabi::at<be<u32>>(gabi::ea(&l_windowZero));
    call_ptr<void>(windowVirtual(s, 0x434), s);
    if (s->mOpenEnabled && !s->mOpenBlocked)
        call_ptr<void>(windowVirtual(s, 0x42C), s);
    windowSetStatus(l_windowCore.get(), 6);
}

/* 026FCB00 */
u32 hdMsgWindow_advanceTyping(WindowBase *s) {
    u32 flags = s->mTypingFlags;
    WindowMessage *msg = s->mpMessage.get();
    u32 n = 0;
    float acc = l_windowZero;
    if (flags & WINDOW_TYPING_SKIP) {
        WindowMsgInput *input = msg->mpInput.get();
        if (input && !input->mNoSkip) {
            s->mCharacterCount = 0x14F;
            s->mpMessage.get()->mTypingPause = 0;
            s->mTypingAccumulator = acc;
            return 1;
        }
    }
    s32 pause = msg->mTypingPause;
    if (pause != 0) {
        msg->mTypingPause = pause > 0 ? u32(pause) - 1 : 0;
        s->mTypingAccumulator = acc;
        return 0;
    }
    acc = s->mTypingAccumulator;
    float speed = l_windowCore.get()->mTypingSpeed;
    if (flags & WINDOW_TYPING_SPEEDUP) {
        WindowMsgInput *input = msg->mpInput.get();
        if (input && !input->mNoSkip)
            speed = fmuls_ppc(speed, l_windowSpeedup);
    }
    acc = fadds_ppc(acc, speed);
    float one = 1.0f; // RPX read-only constant 1010AC28.
    if (acc >= one)
        do {
            acc = fsubs_ppc(acc, one);
            ++n;
        } while (acc >= one);
    s->mTypingAccumulator = acc;
    return n;
}

/* 026FCEC8 */
u32 hdMsgWindow_hasInput(WindowBase *s) { return s->mInputFlags & WINDOW_INPUT_CONFIRM; }

/* 026FCC10 */
void hdMsgWindow_updateOpen(WindowBase *s) {
    WindowCore *core = l_windowCore.get();
    u32 n = hdMsgWindow_advanceTyping(s);
    if (!n)
        return;
    if (s->mCharacterCount < 0x14F) {
        u32 count = s->mCharacterCount + n;
        u32 index = s->mStopIndex + 1;
        s->mCharacterCount = count;
        s32 stop = s->mStopCounts[index < 10 ? index : 0];
        if (s32(count) > stop)
            s->mCharacterCount = u32(stop);
    }
    call_ptr<void>(windowVirtual(s, 0x434), s);
    WindowMessage *msg = s->mpMessage.get();
    if (msg->mLayoutDone)
        return;
    u32 state = msg->mState;
    WindowState *next = nullptr;
    if (msg->mNextMode == 1) {
        windowSetStatus(core, msg->mState);
        next = s->mpMessage.get()->mHasNext ? &l_windowState0_E01C : &l_windowState0_E07C;
    } else if (state == 7) {
        windowSetStatus(core, msg->mState);
        msg = s->mpMessage.get();
        next = msg->mTimedWait ? &l_windowState0_DF8C
                               : (msg->mInputWait ? &l_windowState0_DFBC : &l_windowState0_DF2C);
    } else if (state == 10) {
        windowSetStatus(core, msg->mState);
        msg = s->mpMessage.get();
        if (msg->mTimedWait)
            next = &l_windowState0_E22C;
        else if (msg->mInputWait)
            next = &l_windowState0_E25C;
    } else if (state == 14 || state == 21) {
        windowSetStatus(core, msg->mState);
        next = state == 14 ? &l_windowState0_E2BC : &l_windowState0_E19C;
    }
    if (next)
        hdMsgWindowChangeState(&s->mStateMachine, next);
}

/* 026FCEE0 */
void hdMsgWindow_updateStop(WindowBase *s) {
    if (hdMsgWindow_hasInput(s) || (s->mInputFlags & WINDOW_INPUT_SKIP)) {
        u32 count = s->mAdvanceCount + 1;
        s->mAdvanceCount = count;
        hdMsgWindowGetPlay()->mAdvanceCount = count;
        l_windowInputMode = 2;
        u32 i = s->mStopIndex + 1;
        if (s->mStopOffsets[i < 10 ? i : 0] >= 0)
            hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E3AC);
        else {
            windowSetStatus(l_windowCore.get(), 14);
            hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E2BC);
        }
    } else if (s->mpMessage.get()->mCloseRequested) {
        windowSetStatus(l_windowCore.get(), 1);
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E40C);
    } else {
        windowUpdatePane(s->mpTextPane.get(), s->mpMessage.get());
        l_windowInputMode = 1;
    }
}

/* 026FD054 */
void hdMsgWindow_updateWait(WindowBase *s) {
    WindowMessage *msg = s->mpMessage.get();
    if (msg->mWaitTimer == 0) {
        msg->mTimedWait = 0;
        u32 i = s->mStopIndex + 1;
        if (s->mStopOffsets[i < 10 ? i : 0] >= 0)
            hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E3AC);
        else {
            windowSetStatus(l_windowCore.get(), 14);
            hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E2BC);
        }
    } else {
        s32 t = msg->mWaitTimer;
        msg->mWaitTimer = t > 0 ? u32(t) - 1 : 0;
        windowUpdatePane(s->mpTextPane.get(), s->mpMessage.get());
    }
}

/* 026FD14C */
void hdMsgWindow_updateWaitEnd(WindowBase *s) {
    if (call_ptr<u32>(windowVirtual(s, 0x4C4), s))
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_DFEC);
    else {
        WindowMessage *msg = s->mpMessage.get();
        s32 t = msg->mWaitTimer;
        msg->mWaitTimer = t > 0 ? u32(t) - 1 : 0;
        windowUpdatePane(s->mpTextPane.get(), s->mpMessage.get());
    }
}

/* 026FD1DC */
void hdMsgWindow_updateWaitInput(WindowBase *s) {
    WindowMessage *msg = s->mpMessage.get();
    bool accepted = msg->mWaitTimer == 0;
    u8 mode = 0;
    if (!accepted) {
        if (hdMsgWindow_hasInput(s) || (s->mInputFlags & WINDOW_INPUT_SKIP)) {
            WindowPlay *play = hdMsgWindowGetPlay();
            msg = s->mpMessage.get();
            if (!play->mInputBlocked) {
                accepted = true;
                mode = 2;
            }
        }
    }
    if (accepted) {
        msg->mInputWait = 0;
        u32 count = s->mAdvanceCount + 1;
        s->mAdvanceCount = count;
        hdMsgWindowGetPlay()->mAdvanceCount = count;
        l_windowInputMode = mode;
        u32 i = s->mStopIndex + 1;
        if (s->mStopOffsets[i < 10 ? i : 0] < 0) {
            windowSetStatus(l_windowCore.get(), 14);
            hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E2BC);
        } else
            hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E3AC);
    } else {
        s32 t = msg->mWaitTimer;
        msg->mWaitTimer = t > 0 ? u32(t) - 1 : 0;
        windowUpdatePane(s->mpTextPane.get(), s->mpMessage.get());
        l_windowInputMode = 1;
    }
}

/* 026FD980 */
void hdMsgWindow_updateSelect2(WindowBase *s) {
    WindowCore *core = l_windowCore.get();
    bool input = hdMsgWindow_hasInput(s) != 0;
    if (!input && (s->mInputFlags & WINDOW_INPUT_SKIP))
        input = hdMsgWindowGetPlay()->mMessageMode == 39;
    if (!input)
        input = s->mpMessage.get()->mConfirmInput != 0;
    if (input && windowSelectionReady(core->mpSelector.get())) {
        if (s->mInputFlags & WINDOW_INPUT_SKIP)
            hdMsgWindowGetPlay()->mHandInput = 1;
        core->mSelection = s->mSelection;
        windowSetStatus(core, 14);
        u32 count = s->mAdvanceCount + 1;
        s->mAdvanceCount = count;
        hdMsgWindowGetPlay()->mAdvanceCount = count;
        windowPlaySound0(s->mSelection == 0 ? WINDOW_SOUND_ACCEPT : WINDOW_SOUND_CANCEL);
        l_windowInputMode = 2;
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E10C);
        return;
    }
    u32 flags = s->mInputFlags;
    u32 old = s->mSelection;
    u32 value = old;
    if (flags & WINDOW_INPUT_PREVIOUS_STICK) {
        value = 0;
        s->mSelection = value;
    } else if (flags & WINDOW_INPUT_NEXT_STICK) {
        value = 1;
        s->mSelection = value;
    } else if (flags & WINDOW_INPUT_PREVIOUS_KEY) {
        value = 0;
        s->mSelection = value;
    } else if (flags & WINDOW_INPUT_NEXT_KEY) {
        value = 1;
        s->mSelection = value;
    }
    if (value != old)
        windowPlaySound0(WINDOW_SOUND_SELECTION);
    l_windowInputMode = 1;
}

/* 026FDB08 */
void hdMsgWindow_updateSelect3(WindowBase *s) {
    WindowCore *core = l_windowCore.get();
    bool input = hdMsgWindow_hasInput(s) != 0;
    if (!input && (s->mInputFlags & WINDOW_INPUT_SKIP))
        input = hdMsgWindowGetPlay()->mMessageMode == 39;
    if (!input)
        input = s->mpMessage.get()->mConfirmInput != 0;
    if (input && windowSelectionReady(core->mpSelector.get())) {
        if (s->mInputFlags & WINDOW_INPUT_SKIP)
            hdMsgWindowGetPlay()->mHandInput = 1;
        core->mSelection = s->mSelection;
        windowSetStatus(core, 14);
        u32 count = s->mAdvanceCount + 1;
        s->mAdvanceCount = count;
        hdMsgWindowGetPlay()->mAdvanceCount = count;
        windowPlaySound0(s->mSelection == 2 ? WINDOW_SOUND_CANCEL : WINDOW_SOUND_ACCEPT);
        l_windowInputMode = 2;
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E10C);
        return;
    }
    u32 flags = s->mInputFlags;
    u32 old = s->mSelection;
    u32 value = old;
    if (flags & WINDOW_INPUT_PREVIOUS_STICK) {
        if (s32(value) > 0) {
            --value;
            s->mSelection = value;
        }
    } else if (flags & WINDOW_INPUT_NEXT_STICK) {
        if (s32(value) < 2) {
            ++value;
            s->mSelection = value;
        }
    } else if (flags & WINDOW_INPUT_PREVIOUS_KEY) {
        if (s32(value) > 0) {
            --value;
            s->mSelection = value;
        }
    } else if (flags & WINDOW_INPUT_NEXT_KEY) {
        if (s32(value) < 2) {
            ++value;
            s->mSelection = value;
        }
    }
    if (value != old)
        windowPlaySound0(WINDOW_SOUND_SELECTION);
    l_windowInputMode = 1;
}

// Additional scoped selection and wait states (UI_msg_base_b).
/* 026FDEA0 */
u32 hdMsgWindow_decreaseSelection(WindowBase *s) {
    s32 value = s->mSelection;
    if (value <= 0)
        return gabi::ea(s);
    s->mSelection = u32(value) - 1;
    return windowPlaySound0(WINDOW_SOUND_NUMBER);
}

/* 026FDEC0 */
u32 hdMsgWindow_increaseSelection(WindowBase *s, const be<u32> *limits) {
    u32 limit = *limits - 1;
    s32 value = s->mSelection;
    if (value >= s32(limit))
        return gabi::ea(s);
    s->mSelection = u32(value) + 1;
    return windowPlaySound0(WINDOW_SOUND_NUMBER);
}

/* 026FDEE8 */
u32 hdMsgWindow_approachNumber(WindowBase *s, be<s16> *number, const be<u32> *index) {
    u32 i = *index;
    if (i >= 4)
        return 0;

    u32 mode = s->mSelection;
    s32 value = *number;
    s32 target = l_windowLimits.mValues[i]; // i < 4 checked above
    if (value > target) {
        *number = u16(target);
        return 0;
    }
    if (value == target || mode > 2)
        return 0;
    s32 step = mode == 0 ? 1 : (mode == 1 ? 10 : 100);
    value = s16(value + step);
    *number = u16(value);
    i = *index;
    target = *gabi::at<be<s16>>(gabi::ea(&l_windowLimits) + i * 2);
    if (value > target)
        *number = u16(target);
    return 1;
}

/* 026FDFE0 */
u32 hdMsgWindow_decreaseNumber(WindowBase *s, be<s16> *number) {
    u32 mode = s->mSelection;
    if (mode > 2)
        return 0;
    s32 value = *number;
    if (value <= 0)
        return 0;
    s32 step = mode == 0 ? 1 : (mode == 1 ? 10 : 100);
    value = s16(value - step);
    *number = value < 0 ? 0 : u16(value);
    return 1;
}

/* 026FE414 */
void hdMsgWindow_updateNumberSelect(WindowBase *s) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (hdMsgWindow_hasInput(s)) {
        windowPlaySound0(WINDOW_SOUND_ACCEPT);
        windowNumberAccept();
        u32 n = s->mAdvanceCount + 1;
        s->mAdvanceCount = n;
        hdMsgWindowGetPlay()->mAdvanceCount = u8(n);
        WindowPane *pane = s->mpNumberPane.get();
        pane->mFlags = pane->mFlags & 0xFE;
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E49C);
        return;
    }
    bool three = s->mMessageId == 0x1CFA;
    // HD: preserve the GHS-selected stack slot passed to the number input callback.
    FrameLocal<be<u32>> count(three ? 8 : 12);
    *count = three ? 3 : 2;
    windowNumberInput(s, count.get());
    u32 selection = s->mSelection;
    WindowCore *core = l_windowCore.get();
    core->mSelection = selection;
    l_windowInputMode = 3;
    call_ptr<void>(windowVirtual(s, 0x43C), s);
    WindowPane *pane = s->mpNumberPane.get();
    pane->mFlags = (pane->mFlags & 0xFE) + 1;
    windowNumberRefresh(s);
}

/* 026FE698 */
void hdMsgWindow_updateAutoWait(WindowBase *s) {
    WindowMessage *msg = s->mpMessage.get();
    if (msg->mWaitTimer == 0) {
        msg->mTimedWait = 0;
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E34C);
    } else {
        s32 timer = msg->mWaitTimer;
        msg->mWaitTimer = timer > 0 ? msg->mWaitTimer - 1 : 0;
        WindowPane *pane = s->mpTextPane.get();
        WindowMessage *message = s->mpMessage.get();
        windowUpdatePane(pane, message);
    }
}

/* 026FE6FC */
void hdMsgWindow_updateAutoWaitEnd(WindowBase *s) {
    if (call_ptr<u32>(windowVirtual(s, 0x4C4), s))
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E28C);
    else {
        WindowMessage *msg = s->mpMessage.get();
        s32 t = msg->mWaitTimer;
        msg->mWaitTimer = t > 0 ? u32(t) - 1 : 0;
        WindowPane *pane = s->mpTextPane.get();
        WindowMessage *message = s->mpMessage.get();
        windowUpdatePane(pane, message);
    }
}

/* 026FE78C */
void hdMsgWindow_updateAutoWaitInput(WindowBase *s) {
    WindowMessage *msg = s->mpMessage.get();
    if (msg->mWaitTimer == 0) {
        msg->mInputWait = 0;
        l_windowInputMode = 0;
        u32 n = s->mAdvanceCount + 1;
        s->mAdvanceCount = n;
        hdMsgWindowGetPlay()->mAdvanceCount = u8(n);
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E34C);
        return;
    }
    bool input = hdMsgWindow_hasInput(s) != 0;
    if (!input)
        input = (s->mInputFlags & WINDOW_INPUT_SKIP) != 0;
    if (!input)
        input = msg->mConfirmInput != 0;
    if (input) {
        WindowPlay *play = hdMsgWindowGetPlay();
        u8 blocked = play->mInputBlocked;
        msg = s->mpMessage.get();
        if (!blocked) {
            msg->mInputWait = 0;
            l_windowInputMode = 0;
            hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E43C);
            return;
        }
    }
    s32 t = msg->mWaitTimer;
    msg->mWaitTimer = t > 0 ? u32(t) - 1 : 0;
    WindowPane *pane = s->mpTextPane.get();
    WindowMessage *message = s->mpMessage.get();
    windowUpdatePane(pane, message);
    l_windowInputMode = 4;
}

/* 026FEAF4 */
void hdMsgWindow_updateHandStop(WindowBase *s) {
    bool input = hdMsgWindow_hasInput(s) != 0;
    if (!input)
        input = (s->mInputFlags & WINDOW_INPUT_SKIP) != 0;
    if (!input && s->mMessageId == 0x5AC)
        input = hdMsgWindowGetPlay()->mHandInput != 0;
    if (input && !hdMsgWindowGetPlay()->mInputBlocked) {
        l_windowInputMode = 2;
        WindowPlay *play = hdMsgWindowGetPlay();
        windowPlaySound0(play->mMessageSoundMode == 39 ? WINDOW_SOUND_CANCEL : WINDOW_SOUND_CLOSE);
        u32 n = s->mAdvanceCount + 1;
        s->mAdvanceCount = n;
        hdMsgWindowGetPlay()->mAdvanceCount = u8(n);
        hdMsgWindowGetPlay()->mHandInput = 0;
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E3DC);
        return;
    }
    WindowMessage *msg = s->mpMessage.get();
    bool close = msg->mCloseRequested != 0;
    if (close) {
        WindowPlay *play = hdMsgWindowGetPlay();
        u8 blocked = play->mInputBlocked;
        msg = s->mpMessage.get();
        close = !blocked && !msg->mCloseBlocked && msg->mNextMessageNumber != msg->mMessageNumber;
    }
    if (close) {
        l_windowInputMode = 2;
        windowPlaySound0(WINDOW_SOUND_CLOSE);
        call_ptr<void>(windowVirtual(s, 0x4A4), s);
        hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_DE9C);
    } else {
        windowUpdatePane(s->mpTextPane.get(), msg);
        l_windowInputMode = 1;
    }
}

/* 026FEDA8 */
void hdMsgWindow_updateNextStop(WindowBase *s) {
    if (!call_ptr<u32>(windowVirtual(s, 0x4D4), s))
        return;
    u32 next = s->mStopIndex + 1;
    WindowMessage *msg = s->mpMessage.get();
    s->mStopIndex = next;
    s->mCharacterCount = 0;
    u32 color = msg->mTextColor;
    WindowPane *pane = s->mpTextPane.get();
    pane->mTopColor = color;
    pane->mBottomColor = color;
    s->mStopReady = 0;
    call_ptr<void>(windowVirtual(s, 0x434), s);
    hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_DEFC);
}

/* 026FEF28 */
void hdMsgWindow_updateAdvanceA(WindowBase *s) {
    call_ptr<void>(windowVirtual(s, 0x4B4), s);
    WindowPlay *play = hdMsgWindowGetPlay();
    windowPlaySound0(play->mMessageSoundMode == 39 ? WINDOW_SOUND_CANCEL : WINDOW_SOUND_CLOSE);
    u32 n = s->mAdvanceCount + 1;
    s->mAdvanceCount = n;
    hdMsgWindowGetPlay()->mAdvanceCount = u8(n);
}

/* 026FF028 */
void hdMsgWindow_updateFinish(WindowBase *s) {
    if (!call_ptr<u32>(windowVirtual(s, 0x4D4), s))
        return;
    u32 n = s->mAdvanceCount + 1;
    s->mStopReady = 0;
    s->mAdvanceCount = n;
    hdMsgWindowGetPlay()->mAdvanceCount = u8(n);
    WindowCore *core = l_windowCore.get();
    l_windowInputMode = 2;
    windowSetStatus(core, 14u);
    hdMsgWindowChangeState(&s->mStateMachine, &l_windowState0_E2BC);
}

/* 026FF0BC */
void hdMsgWindow_updateAdvanceB(WindowBase *s) {
    call_ptr<void>(windowVirtual(s, 0x4B4), s);
    WindowPlay *play = hdMsgWindowGetPlay();
    windowPlaySound0(play->mMessageSoundMode == 39 ? WINDOW_SOUND_CANCEL : WINDOW_SOUND_CLOSE);
    u32 n = s->mAdvanceCount + 1;
    s->mAdvanceCount = n;
    hdMsgWindowGetPlay()->mAdvanceCount = u8(n);
}
/* WWHD glue (tools/verify/autoglue.py)
data l_windowCore 101F4B5C
data l_windowZero 1010AC34
data l_windowSpeedup 1010AC38
data l_windowLimits 1010AC54
data l_windowInputMode 1047B09E
data l_windowState0_E01C 1049E01C
data l_windowState0_E07C 1049E07C
data l_windowState0_DF8C 1049DF8C
data l_windowState0_DFBC 1049DFBC
data l_windowState0_DF2C 1049DF2C
data l_windowState0_E22C 1049E22C
data l_windowState0_E25C 1049E25C
data l_windowState0_E2BC 1049E2BC
data l_windowState0_E19C 1049E19C
data l_windowState0_E3AC 1049E3AC
data l_windowState0_E40C 1049E40C
data l_windowState0_DFEC 1049DFEC
data l_windowState0_E10C 1049E10C
data l_windowState0_E49C 1049E49C
data l_windowState0_E34C 1049E34C
data l_windowState0_E28C 1049E28C
data l_windowState0_E43C 1049E43C
data l_windowState0_E3DC 1049E3DC
data l_windowState0_DE9C 1049DE9C
data l_windowState0_DEFC 1049DEFC
ext 025F74D0 void windowSetStatus(WindowCore*,u32)
ext 025200D4 WindowPlay* hdMsgWindowGetPlay()
ext 020063C0 void hdMsgWindowChangeState(be<u32>*,WindowState*)
ext 0270477C void windowUpdatePane(WindowPane*,WindowMessage*)
ext 0265233C u32 windowSelectionReady(void*)
ext 025E1988 u32 windowPlaySound0(u32)
ext 025E1C64 void windowNumberAccept()
ext 026FE070 void windowNumberInput(WindowBase*,be<u32>*)
ext 026FE228 void windowNumberRefresh(WindowBase*)
*/

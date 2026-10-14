/* HD event-message state transitions, in development against WWHD.
 * The merged event-window source remains the owner of its existing handlers. */
#include "d/hd_msg_window_states.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_screen_baton_sign_for_window.h"
#include "d/hd_msg_window_event_handlers.h"
#include "d/hd_msg_event_position.h"
#include "d/hd_msg_window_event_companion.h"
struct hdMsgEventSongPaneRows_l { be<u32> mRows[8]; };
WWHD_OFFSET(hdMsgEventSongPaneRows_l, mRows, 0);
WWHD_SIZE(hdMsgEventSongPaneRows_l, 0x20);
static hdMsgEventSongPaneRows_l sEventSongPaneRows;
static hdMsgEvent_state_c sEventOpenedState;
static hdMsgEvent_state_c sEventCloseWaitState;
static hdMsgEvent_state_c sEventCloseDoneState;

/* inlined into executeSpecial, beginClose and updateClose */
static hdMsgEventCallbacks_l* hdMsgEvent_callbacks(hdMsgEventHandlers_c* window) {
    auto* parts = reinterpret_cast<hdParts_c*>(window);
    return reinterpret_cast<hdMsgEventCallbacks_l*>(parts->mVtable.get());
}

/* 026B21B4 */
void hdMsgEventHandlers_c::enterMessage() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    prepareMessage();
}
/* 026B25A8 */
void hdMsgEventHandlers_c::enterTimedMessage() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    mFrame = 0;
    prepareMessage();
}
/* 026B30F8 */
void hdMsgEventHandlers_c::enterBaseWait() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    reinterpret_cast<hdMsgWindowStates_c*>(this)->enterAccept();
}
/* 026B3164 */
void hdMsgEventHandlers_c::enterTextWait() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdMsgEvent_baseEnterTextWait(this);
}
/* 026B3168 */
void hdMsgEventHandlers_c::enterCloseWait() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    reinterpret_cast<hdMsgWindowStates_c*>(this)->enterNextMessage();
}
/* 026B31E0 */
void hdMsgEventHandlers_c::exitBaseWait() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdMsgEvent_baseExitWait(this);
}
/* 026B31E4 */
void hdMsgEventHandlers_c::startCloseAnimation() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    reinterpret_cast<hdMsgWindowStates_c*>(this)->startAnimation(1, 1);
}
/* 026B216C */
void hdMsgEventHandlers_c::updateOpening() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (reinterpret_cast<hdMsgWindowStates_c*>(this)->animationFinished(0))
        hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sEventOpenedState));
}
/* 026B31F0 */
void hdMsgEventHandlers_c::updateClosing() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (reinterpret_cast<hdMsgWindowStates_c*>(this)->animationFinished(1))
        hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sEventCloseWaitState));
}
/* 026B32CC */
void hdMsgEventHandlers_c::updateClosingText() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (reinterpret_cast<hdMsgWindowStates_c*>(this)->animationFinished(2))
        hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sEventOpenedState));
}
/* 026B3314 */
void hdMsgEventHandlers_c::captureAdvanceRequest() {
    mAdvanceRequested = mpMessage->mReady;
}
/* 026B1A14 */
u32 hdMsgEventHandlers_c::songLengthFor(s32 index) {
    if (index < 0) index = 0;
    else if (index >= 8) index = 7;
    return sEventSongPaneRows.mRows[index];
}
/* 026B25D8 */
void hdMsgEventHandlers_c::initializeInput() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    u32 row = mPaneRow;
    mCompleted = 0;
    mInputRow = row;
    mShownPaneCount = 0;
    mProgress = 0;
    mBeatCount = hdMsgEvent_songLength();
    mFrame = 0;
    for (u32 i = 0; i < 6; ++i) mBeatAges[i] = 0;
    hdMsgEvent_setStatus(sCore.mpCore, 21);
    for (u32 i = 0; s32(i) < s32(u32(mDisplayCount)); ++i) {
        u32 index = paneIndex(s32(u32(mPaneRow)), s32(i));
        resetPane(index);
    }
    mInputDelay = 3;
}
/* 026B2720 */
void hdMsgEventHandlers_c::requestCancel() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (inputReady())
        reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get())->play.mMesgCancelButton = 1;
}
/* 026B316C */
void hdMsgEventHandlers_c::executeSpecial() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    void* core = sCore.mpCore;
    if (hdMsgEvent_getCoreStatus(core) == 19 || u32(mWindowType) == 2) {
        hdMsgEvent_setStatus(core, 0);
        auto* callbacks = hdMsgEvent_callbacks(this);
        gabi::call_ptr(callbacks->mClear.get(), this);
    }
}
/* 026B3074 */
void hdMsgEventHandlers_c::beginClose() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    auto* callbacks = hdMsgEvent_callbacks(this);
    gabi::call_ptr(callbacks->mBeginClose.get(), this);
    hdMsgEvent_sound(0x80B);
}
/* 026B30A8 */
void hdMsgEventHandlers_c::updateClose() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    auto* callbacks = hdMsgEvent_callbacks(this);
    if (gabi::call_ptr<u32>(callbacks->mIsCloseComplete.get(), this))
        hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sEventCloseDoneState));
}


/* Additional HD event text handlers. */


struct hdMsgEventTyping_l {
    u8 _000[0x94]; be<f32> mProgress;
};
WWHD_OFFSET(hdMsgEventTyping_l, mProgress, 0x94);
WWHD_SIZE(hdMsgEventTyping_l, 0x98);
static hdMsgEvent_state_c sEventSequenceAbort;
static hdMsgEventWindowScalar_l sEventTypingStart;
/* 026B19BC */
void hdMsgEventHandlers_c::loopVisibleWindow() {
    auto* panes = reinterpret_cast<hdMsgEventPosition_c*>(this);
    if (u8(panes->mWindowPanes[1]->mFlags) & 1)
        reinterpret_cast<hdMsgEventFrame_c*>(panes->mChildren.safeSlot(14)->get())->enterLoop();
    else if (u8(panes->mWindowPanes[7]->mFlags) & 1)
        reinterpret_cast<hdMsgEventFrame_c*>(panes->mChildren.safeSlot(13)->get())->enterLoop();
}
/* 026B2B94 */
void hdMsgEventHandlers_c::updateSequenceText() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (hdMsgEvent_getCoreStatus(sCore.mpCore) == 16) {
        hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sEventSequenceAbort));
        return;
    }
    if (reinterpret_cast<hdMsgEventMessageFlags_l*>(mpMessage.get())->mCancelRequested)
        requestCancel();
    else
        updateSequence();
    auto* panes = reinterpret_cast<hdMsgEventPosition_c*>(this);
    u32 index = panes->mTextPaneIndex;
    auto* message = mpMessage.get();
    reinterpret_cast<hdTextPane_c*>(panes->mTextPanes[index < 3 ? index : 0].get())->refresh(reinterpret_cast<PaneWork_l*>(message));
}
/* 026B2C50 */
void hdMsgEventHandlers_c::updateActiveText() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (hdMsgEvent_getCoreStatus(sCore.mpCore) == 16) {
        hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sAdvance));
        return;
    }
    auto* pane = mpTextPane.get();
    auto* message = mpMessage.get();
    reinterpret_cast<hdTextPane_c*>(pane)->refresh(reinterpret_cast<PaneWork_l*>(message));
}
/* 026B2E78 */
void hdMsgEventHandlers_c::beginTyping() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    reinterpret_cast<hdMsgEventTyping_l*>(this)->mProgress = f32(sEventTypingStart.mValue);
    refreshText();
    hdMsgEvent_setStatus(sCore.mpCore, 6);
}


struct hdMsgEventMessageWait_l {
    be<u32> mStatus; u8 _004[0x697-4]; be<u8> mWait;
    u8 _698[0x6AC-0x698]; be<u32> mWaitFrames;
    u8 _6B0[0x90C-0x6B0]; be<u8> mCancelRequested;
};
WWHD_OFFSET(hdMsgEventMessageWait_l, mWait, 0x697);
WWHD_OFFSET(hdMsgEventMessageWait_l, mWaitFrames, 0x6AC);
WWHD_OFFSET(hdMsgEventMessageWait_l, mCancelRequested, 0x90C);
WWHD_SIZE(hdMsgEventMessageWait_l, 0x90D);

/* 026B1E04 */
void hdMsgEventHandlers_c::prepareMessage() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    hdMsgEvent_initializeMessage(mpMessage, 0, 0x14F);
    u8 cancel = reinterpret_cast<hdMsgEventCancelFlag_l*>(this)->mCancelRequested;
    reinterpret_cast<hdMsgEventMessageWait_l*>(mpMessage.get())->mCancelRequested = cancel;
    reinterpret_cast<hdMsgEventMessageWait_l*>(mpMessage.get())->mWait = 1;
    reinterpret_cast<hdMsgEventMessageWait_l*>(mpMessage.get())->mWaitFrames = 30;
    auto* panes = reinterpret_cast<hdMsgEventPosition_c*>(this);
    u32 index = panes->mTextPaneIndex;
    auto* message = mpMessage.get();
    reinterpret_cast<hdTextPane_c*>(panes->mTextPanes[index < 3 ? index : 0].get())->prepareMessage(reinterpret_cast<PaneWork_l*>(message), 1);
    message = mpMessage.get();
    void* core = sCore.mpCore;
    hdMsgEvent_setStatus(core, message->mStatus);
    index = panes->mTextPaneIndex;
    panes->mTextPanes[index < 3 ? index : 0]->show();
}


struct hdMsgEventTextOffsets_l {
    u8 _000[0x98]; be<u32> mOffsets[10];
    be<u32>& offset(u32 i) { return mOffsets[i < 10 ? i : 0]; }
};
WWHD_OFFSET(hdMsgEventTextOffsets_l, mOffsets, 0x98);
WWHD_SIZE(hdMsgEventTextOffsets_l, 0xC0);
struct hdMsgEventMessageTextProgress_l {
    be<u32> mStatus; u8 _004[0x64C-4];
    be<u32> mNextOffset, mNextStop, mShownCharacters;
};
WWHD_OFFSET(hdMsgEventMessageTextProgress_l, mNextOffset, 0x64C);
WWHD_OFFSET(hdMsgEventMessageTextProgress_l, mNextStop, 0x650);
WWHD_OFFSET(hdMsgEventMessageTextProgress_l, mShownCharacters, 0x654);
WWHD_SIZE(hdMsgEventMessageTextProgress_l, 0x658);
/* 026B2CBC */
void hdMsgEventHandlers_c::refreshText() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    auto* offsets = reinterpret_cast<hdMsgEventTextOffsets_l*>(this);
    u32 stop = mStopIndex;
    u32 shown = mShownCharacters;
    auto* message = mpMessage.get();
    hdMsgEvent_initializeMessage(message, offsets->offset(stop), shown);
    auto* panes = reinterpret_cast<hdMsgEventPosition_c*>(this);
    u32 index = panes->mTextPaneIndex;
    message = mpMessage.get();
    reinterpret_cast<hdTextPane_c*>(panes->mTextPanes[index < 3 ? index : 0].get())->prepareMessage(reinterpret_cast<PaneWork_l*>(message), 0);
    auto* progress = reinterpret_cast<hdMsgEventMessageTextProgress_l*>(mpMessage.get());
    mShownCharacters = progress->mShownCharacters;
    void* core = sCore.mpCore;
    hdMsgEvent_setStatus(core, progress->mStatus);
    index = panes->mTextPaneIndex;
    panes->mTextPanes[index < 3 ? index : 0]->show();
    u32 next = u32(mStopIndex) + 1;
    if (s32(u32(offsets->offset(next))) < 0) {
        progress = reinterpret_cast<hdMsgEventMessageTextProgress_l*>(mpMessage.get());
        offsets->offset(next) = progress->mNextOffset;
        next = u32(mStopIndex) + 1;
    }
    if (s32(u32(stopPoint(next))) < 0) {
        progress = reinterpret_cast<hdMsgEventMessageTextProgress_l*>(mpMessage.get());
        stopPoint(next) = u32(progress->mNextStop) + 1;
    }
}

struct hdMsgEventMode_l { u8 _000[0x5BDA]; be<u8> mMode; };
WWHD_OFFSET(hdMsgEventMode_l, mMode, 0x5BDA);
WWHD_SIZE(hdMsgEventMode_l, 0x5BDB);
static hdMsgEvent_state_c sEventOpeningComplete;
static hdMsgEvent_state_c sEventOpeningSpecial;
/* 026B1EFC */
void hdMsgEventHandlers_c::updateOpeningMessage() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (reinterpret_cast<hdMsgWindowStates_c*>(this)->animationFinished(0)) {
        auto* panes = reinterpret_cast<hdMsgEventPosition_c*>(this);
        u32 id = panes->mpMessage->mMessageId;
        if (id == 0x5AC) {
            prepareMessage();
            hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sShortWaitDone));
        } else if (id >= 0x5B3 && id <= 0x5B4) {
            u8 mode = reinterpret_cast<hdMsgEventMode_l*>(dComIfGp_get())->mMode;
            if (mode == 1 || mode == 5) {
                prepareMessage();
                hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sShortWaitDone));
                return;
            }
            hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sEventOpeningSpecial));
        } else {
            hdParts_changeState(reinterpret_cast<hdStateMachine*>(&mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sEventOpeningComplete));
        }
    }
    auto* callbacks = hdMsgEvent_callbacks(this);
    gabi::call_ptr(callbacks->mUpdate.get(), this);
}

/* Add mUpdate at0x42C to own hdMsgEventCallbacks_l, preserving other offsets. */





/* Unapplied event-window constructor, canonical parts base and own game panes. */
struct hdMsgEventInterface_l { be<u32> mFirst; };
WWHD_OFFSET(hdMsgEventInterface_l, mFirst, 0);
WWHD_SIZE(hdMsgEventInterface_l, 4);
static hdMsgEventCallbacks_l sEventWindowVtable;
static hdMsgEventInterface_l sEventWindowSecondaryVtable;
static hdMsgEventInterface_l sEventWindowHolderVtable;
/* 026B3410 */
hdMsgEventHandlers_c* hdMsgEventHandlers_c::construct(hdMsgEventHandlers_c* self) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!self) self = reinterpret_cast<hdMsgEventHandlers_c*>(hdMsgEvent_allocateWindow(sizeof(hdMsgEventHandlers_c)));
    if (!self) return nullptr;
    hdMsgWindowStates_c::construct(reinterpret_cast<hdMsgWindowStates_c*>(self));
    auto* panes = reinterpret_cast<hdMsgEventPosition_c*>(self);
    auto* parts = reinterpret_cast<hdParts_c*>(self);
    parts->mVtable = static_cast<hdPartsVtbl_c*>(&sEventWindowVtable);
    parts->mSecondaryVtable = &sEventWindowSecondaryVtable;
    parts->mStateHolder.mHolderVtable = &sEventWindowHolderVtable;
    // GHS array construction retains wrapped guest-address allocation checks.
    u32 address = gabi::ea(self);
    if (!(address + offsetof(hdMsgEventPosition_c, mWindowPanes))) hdMsgEvent_allocateWindow(sizeof(panes->mWindowPanes));
    if (!(address + offsetof(hdMsgEventPosition_c, mTextPanes))) hdMsgEvent_allocateWindow(sizeof(panes->mTextPanes));
    panes->mTextPaneIndex = 0xFFFFFFFFu;
    if (!(address + offsetof(hdMsgEventPosition_c, mPositionPanes))) hdMsgEvent_allocateWindow(sizeof(panes->mPositionPanes));
    if (!(address + offsetof(hdMsgEventHandlers_c, mBeatAges))) hdMsgEvent_allocateWindow(sizeof(self->mBeatAges));
    self->mProgress = 0;
    self->mPaneRow = 0;
    self->mBeatCount = 0;
    self->mSong = 0;
    self->mDisplayCount = 0;
    if (!(address + offsetof(hdMsgEventHandlers_c, mBeatTimes))) hdMsgEvent_allocateWindow(sizeof(self->mBeatTimes));
    self->mCompleted = 0;
    reinterpret_cast<hdMsgEventCancelFlag_l*>(self)->mCancelRequested = 0;
    self->mInputRow = 0;
    self->mShownPaneCount = 0;
    self->mInputDelay = 0;
    self->mAdvanceRequested = 0;
    for (u32 i = 0; i < 8; ++i) panes->mWindowPanes[i] = nullptr;
    for (u32 i = 0; i < 3; ++i) panes->mTextPanes[i] = nullptr;
    for (u32 i = 0; i < 16; ++i) panes->mPositionPanes[i] = nullptr;
    for (u32 i = 0; i < 6; ++i) self->mBeatAges[i] = 0;
    for (u32 i = 0; i < 7; ++i) self->mBeatTimes[i] = 0;
    return self;
}


/* Unapplied musical-event pane colours/visibility. All lookup data remain runtime image views. */
struct hdMsgEventNoteColors_l { be<u32> mColors[48]; };
struct hdMsgEventSignIndices_l { be<u32> mIndices[3]; };
struct hdMsgEventSignFlags_l { be<u8> mFlags[9]; };
WWHD_OFFSET(hdMsgEventNoteColors_l, mColors, 0);
WWHD_SIZE(hdMsgEventNoteColors_l, 0xC0);
WWHD_OFFSET(hdMsgEventSignIndices_l, mIndices, 0);
WWHD_SIZE(hdMsgEventSignIndices_l, 0xC);
WWHD_OFFSET(hdMsgEventSignFlags_l, mFlags, 0);
WWHD_SIZE(hdMsgEventSignFlags_l, 9);
static hdMsgEventNoteColors_l sEventNoteColors;
static hdMsgEventSignIndices_l sEventSignIndices;
static hdMsgEventSignFlags_l sEventSignFlags;
/* 026B1AF0 */
void hdMsgEventHandlers_c::configureSongPanes() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    s32 row = mPaneRow;
    if (row >= 3) { row = 2; mPaneRow = row; }
    for (u32 i = 0; s32(i) < s32(u32(mDisplayCount)); ++i) {
        u32 index = paneIndex(row, s32(i));
        u32 count = mPaneCount;
        auto* panes = mpPanes.get();
        u32 song = mSong;
        auto* pane = panes[index < count ? index : 0].get();
        // This original lookup has no range clamp; retain wrapped guest array addressing.
        u32 color = *gabi::at<be<u32>>(gabi::ea(&sEventNoteColors) + (song * 6 + i) * 4);
        reinterpret_cast<hdBatonSignForWindow_c*>(pane)->setSignFrame(color);
        row = mPaneRow;
    }
    auto* position = reinterpret_cast<hdMsgEventPosition_c*>(this);
    position->mWindowPanes[2]->show();
    position->mWindowPanes[6]->hide();
    for (u32 i = 0; i < 3; ++i) {
        u32 index = sEventSignIndices.mIndices[i];
        u32 currentRow = mPaneRow;
        auto* pane = position->mWindowPanes[index < 8 ? index : 0].get();
        u8 flags = pane->mFlags;
        // Signed negative rows also remain a guest address calculation, not host-array UB.
        u8 visible = *gabi::at<be<u8>>(gabi::ea(&sEventSignFlags) + currentRow * 3 + i);
        pane->mFlags = (flags & ~1u) | visible;
    }
}




/* Message-entry methods invoke the game text-preparation callback. */
/* 026B1C80 */
void hdMsgEventHandlers_c::enterSongMessage() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    auto* callbacks = hdMsgEvent_callbacks(this);
    gabi::call_ptr(callbacks->mPrepareText.get(), this);
    u32 id = mMessageId;
    hdMsgEvent_setMessageId(mpMessage, id, 0);
    reinterpret_cast<hdMsgWindowStates_c*>(this)->startAnimation(0, 0);
    hdMsgEvent_startWindowAnimation(this);
    loopVisibleWindow();
    u32 mode = reinterpret_cast<hdMsgEventMode_l*>(dComIfGp_get())->mMode;
    auto* panes = reinterpret_cast<hdMsgEventPosition_c*>(this);
    if (mode == 0xFF) {
        mSong = mode;
        panes->mWindowPanes[3]->hide();
        panes->mWindowPanes[4]->hide();
        panes->mWindowPanes[5]->hide();
    } else {
        u32 song = mode < 8 ? mode : 7;
        mSong = song;
        mPaneRow = songLengthFor(s32(song));
        u32 count = lengthIndex(s32(u32(mSong)));
        mBeatCount = count;
        mDisplayCount = count;
        configureSongPanes();
    }
    for (u32 i = 0; s32(i) < s32(u32(mDisplayCount)); ++i) {
        u32 index = paneIndex(s32(u32(mPaneRow)), s32(i));
        resetPane(index);
    }
}
struct hdMsgEventEntryStack_l {
    u8 _00[8]; be<u32> mPaneSource, mPaneTop, mPaneBottom;
    be<u32> mMessageTopSource, mMessageTop, mMessageBottomSource, mMessageBottom;
    u8 _24[4];
};
WWHD_OFFSET(hdMsgEventEntryStack_l, mPaneSource, 8);
WWHD_OFFSET(hdMsgEventEntryStack_l, mPaneTop, 0xC);
WWHD_OFFSET(hdMsgEventEntryStack_l, mPaneBottom, 0x10);
WWHD_OFFSET(hdMsgEventEntryStack_l, mMessageTopSource, 0x14);
WWHD_OFFSET(hdMsgEventEntryStack_l, mMessageTop, 0x18);
WWHD_OFFSET(hdMsgEventEntryStack_l, mMessageBottomSource, 0x1C);
WWHD_OFFSET(hdMsgEventEntryStack_l, mMessageBottom, 0x20);
WWHD_SIZE(hdMsgEventEntryStack_l, 0x28);
struct hdMsgEventMessageColors_l { u8 _000[0x919]; be<u32> mTop, mBottom; };
WWHD_OFFSET(hdMsgEventMessageColors_l, mTop, 0x919);
WWHD_OFFSET(hdMsgEventMessageColors_l, mBottom, 0x91D);
WWHD_SIZE(hdMsgEventMessageColors_l, 0x921);
/* 026B2004 */
void hdMsgEventHandlers_c::enterTextMessage() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x28> frame;
    auto& stack = *gabi::at<hdMsgEventEntryStack_l>(frame.sp());
    auto* callbacks = hdMsgEvent_callbacks(this);
    gabi::call_ptr(callbacks->mPrepareText.get(), this);
    auto* offsets = reinterpret_cast<hdMsgEventTextOffsets_l*>(this);
    for (u32 i = 0; i < 10; ++i) offsets->mOffsets[i] = 0xFFFFFFFFu;
    for (u32 i = 0; i < 10; ++i) mStopPoints[i] = 0xFFFFFFFFu;
    mStopIndex = 0;
    offsets->mOffsets[0] = 0;
    auto* panes = reinterpret_cast<hdMsgEventPosition_c*>(this);
    u32 index = panes->mTextPaneIndex;
    mShownCharacters = 0;
    u32 color = panes->mSmallTextColor;
    auto* pane = reinterpret_cast<hdMsgEventTextPane_l*>(panes->mTextPanes[index < 3 ? index : 0].get());
    stack.mPaneSource = color;
    u32 copiedColor = stack.mPaneSource;
    stack.mPaneTop = copiedColor;
    pane->mColorTop = u32(stack.mPaneTop);
    stack.mPaneBottom = copiedColor;
    pane->mColorBottom = u32(stack.mPaneBottom);
    color = panes->mSmallTextColor;
    auto* message = reinterpret_cast<hdMsgEventMessageColors_l*>(mpMessage.get());
    stack.mMessageTopSource = color;
    stack.mMessageTop = u32(stack.mMessageTopSource);
    message->mTop = u32(stack.mMessageTop);
    color = panes->mSmallTextColor;
    message = reinterpret_cast<hdMsgEventMessageColors_l*>(mpMessage.get());
    stack.mMessageBottomSource = color;
    stack.mMessageBottom = u32(stack.mMessageBottomSource);
    message->mBottom = u32(stack.mMessageBottom);
    message = reinterpret_cast<hdMsgEventMessageColors_l*>(mpMessage.get());
    hdMsgEvent_setMessageId(reinterpret_cast<hdMsgEvent_message_c*>(message), mMessageId, 0);
    reinterpret_cast<hdMsgWindowStates_c*>(this)->startAnimation(0, 0);
    hdMsgEvent_startWindowAnimation(this);
    loopVisibleWindow();
    callbacks = hdMsgEvent_callbacks(this);
    gabi::call_ptr(callbacks->mUpdate.get(), this);
}


/* WWHD glue
 ext 026FF21C hdMsgWindowStates_c* hdMsgWindowStates_c::construct(hdMsgWindowStates_c*)
 ext 026FEC74 void hdMsgWindowStates_c::enterAccept()
 ext 026FECFC void hdMsgWindowStates_c::enterNextMessage()
 ext 026FF5EC void hdMsgWindowStates_c::startAnimation(u32, u32)
 ext 02621378 void hdBatonSignForWindow_c::setSignFrame(u32)
 ext 0270477C void hdTextPane_c::refresh(PaneWork_l*)
 ext 02704504 void hdTextPane_c::prepareMessage(PaneWork_l*, u32)
 data sEventSongPaneRows 100FD99C
 data sEventOpenedState 104984EC
 data sEventCloseWaitState 104985DC
 data sEventCloseDoneState 104985AC
 ext 025F795C u32 hdMsgEvent_getCoreStatus(void*)
 ext 027008CC void hdMsgEvent_baseEnterTextWait(hdMsgEvent_window_c*)
 ext 027008D0 void hdMsgEvent_baseExitWait(hdMsgEvent_window_c*)
 data sEventSequenceAbort 1049860C
 data sEventTypingStart 100FDAFC
 ext 025F6FCC void hdMsgEvent_initializeMessage(hdMsgEvent_message_c*, u32, u32)
 data sEventOpeningComplete 104983FC
 data sEventOpeningSpecial 1049842C
 data sEventWindowVtable 100FDE88
 data sEventWindowSecondaryVtable 100FE448
 data sEventWindowHolderVtable 100FE438
 ext 0273AD10 void* hdMsgEvent_allocateWindow(u32)
 data sEventNoteColors 100FDA3C
 data sEventSignIndices 100FDA24
 data sEventSignFlags 100FDA30
 ext 025F6B18 void hdMsgEvent_setMessageId(hdMsgEvent_message_c*, u32, u32)
 ext 026B1814 void hdMsgEvent_startWindowAnimation(hdMsgEvent_window_c*)
*/

/* HD event-window state updates and musical-beat input.
 * GameCube-style source verified as UI_msg_event; no GameCube counterpart. */
#include "d/hd_screen_msg_window_event.h"

static u32 sSongLengths[8] = {};
static u32 sPaneIndices[3][6] = {};
static hdMsgEvent_corePointer_c sCore;
static hdMsgEvent_managerPointer_c sManager;
static u8 sInputBusy;
static u8 sInputIndicator;
static hdMsgEvent_state_c sWait;
static hdMsgEvent_state_c sBeatDisplayDone;
static hdMsgEvent_state_c sShortWaitDone;
static hdMsgEvent_state_c sSequenceDone;
static hdMsgEvent_state_c sAdvance;
static hdMsgEvent_state_c sClosed;
static hdMsgEvent_state_c sNext;
static hdMsgEvent_state_c sWaitInput;
static hdMsgEvent_state_c sSpecialWait;
static hdMsgEvent_state_c sEnd;

/* inlined into event-window updates */
static dComIfG_inf_c* hdMsgEvent_game() {
    return reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
}

/* inlined into updateInput */
static u8 hdMsgEvent_inputBlocked() {
    // HD: this verified input-block byte is absent from the shared play layout.
    return reinterpret_cast<hdMsgEvent_inputView_c*>(dComIfGp_get())->mInputBlocked;
}

/* inlined into updateSequence and updateInput */
static void hdMsgEvent_advance(hdMsgEvent_window_c* window) {
    u32 frame = u32(window->mAdvanceCount) + 1;
    window->mAdvanceCount = frame;
    hdMsgEvent_game()->play.mMesgSendButton = frame;
}

/* 026B1A50 */
u32 hdMsgEvent_window_c::lengthIndex(s32 index) {
    if (index < 0) index = 0;
    else if (index >= 8) index = 7;
    return sSongLengths[index];
}

/* 026B1A8C */
u32 hdMsgEvent_window_c::paneIndex(s32 row, s32 column) {
    if (row < 0) row = 0;
    else if (row >= 3) row = 2;
    if (column < 0) column = 0;
    else if (column >= 6) column = 5;
    return sPaneIndices[row][column];
}

/* 026B1C60 */
void hdMsgEvent_window_c::resetPane(u32 index) {
    hdMsgEvent_resetPane(pane(index));
}

/* 026B21B8 */
void hdMsgEvent_window_c::enterWait() {
    hdMsgEvent_message_c* message = mpMessage;
    u8 ready = message->mReady;
    void* controller = sCore.mpCore;
    if (ready) {
        message->mWait = 1;
        message = mpMessage;
        message->mWaitTimer = 30;
        hdMsgEvent_setStatus(controller, EVENT_STATUS_WAIT);
        hdMsgEvent_changeState(&mStateMachine, &sWait);
    }
}

/* 026B2224 */
void hdMsgEvent_window_c::initializeBeats() {
    mProgress = 0;
    hdMsgEvent_prepareSong(mSong);
    for (u32 i = 0; i < 6; ++i) mBeatAges[i] = 0;
    s32 count = mBeatCount;
    mBeatTimes[0] = 0;
    if (count > 6) mBeatCount = 6;
    for (u32 i = 0; s32(i) < s32(mBeatCount); ++i) {
        be<u32>* next = &beatTime(i + 1);
        gabi::Local<be<u32>> note;
        f32 length = hdMsgEvent_noteLength(mSong, i, note.get());
        *next = gabi::ftoi(length);
        next = &beatTime(i + 1);
        *next = u32(*next) + u32(beatTime(i));
    }
    for (u32 i = 0; s32(i) < s32(mDisplayCount); ++i) {
        u32 index = paneIndex(mPaneRow, i);
        resetPane(index);
    }
    mFrame = u32(beatTime(u32(mProgress) + 1)) - 1;
}

/* 026B23A4 */
void hdMsgEvent_window_c::showPane(u32 index) {
    hdMsgEvent_showPane(pane(index));
}

/* 026B23C4 */
void hdMsgEvent_window_c::updateBeatDisplay() {
    u32 count = mBeatCount;
    s32 frame = mFrame;
    s32 end = beatTime(count);
    if (frame < end) {
        u32 nextFrame = u32(frame) + 1;
        mFrame = nextFrame;
        u32 index = mProgress;
        count = mBeatCount;
        if (s32(index) < s32(count) && s32(nextFrame) >= s32(beatTime(index + 1))) {
            beatAge(index) = 1;
            index = mProgress;
            if (s32(index) < s32(mDisplayCount)) {
                u32 pane = paneIndex(mPaneRow, index);
                showPane(pane);
                index = mProgress;
            }
            count = mBeatCount;
            mProgress = index + 1;
        }
    }
    for (u32 i = 0; s32(i) < s32(count); ++i) {
        be<u32>* ageValue = &beatAge(i);
        s32 age = *ageValue;
        if (age > 0 && age <= 35) {
            *ageValue = u32(age) + 1;
            count = mBeatCount;
        }
    }
    if (s32(beatAge(count - 1)) > 35) hdMsgEvent_changeState(&mStateMachine, &sBeatDisplayDone);
}

/* 026B25B4 */
void hdMsgEvent_window_c::updateShortWait() {
    u32 count = u32(mFrame) + 1;
    mFrame = count;
    if (s32(count) >= 5) hdMsgEvent_changeState(&mStateMachine, &sShortWaitDone);
}

/* 026B2698 */
u32 hdMsgEvent_window_c::inputReady() {
    if (hdMsgEvent_inputBusy()) {
        mInputDelay = 2;
        return 0;
    }
    u32 delay = mInputDelay;
    if (delay) {
        --delay;
        mInputDelay = delay;
    }
    if (s32(delay) > 0) return 0;
    return sInputBusy == 0;
}

/* 026B2754 */
void hdMsgEvent_window_c::hidePane(u32 index) {
    hdMsgEvent_hidePane(pane(index));
}

/* 026B2774 */
void hdMsgEvent_window_c::playEventSound(u32 alternate) {
    hdMsgEvent_flush();
    dComIfG_inf_c* game = hdMsgEvent_game();
    hdMsgEvent_sound(game->play.mDoStatusForce == 0x27 || alternate ? EVENT_SOUND_ALTERNATE : EVENT_SOUND_ADVANCE);
}

/* 026B27CC */
void hdMsgEvent_window_c::updateSequence() {
    if (inputReady()) {
        hdMsgEvent_setStatus(sCore.mpCore, EVENT_STATUS_ADVANCE);
        hdMsgEvent_game()->play.mMesgCancelButton = 1;
        hdMsgEvent_sound(EVENT_SOUND_ADVANCE);
        hdMsgEvent_flush();
        hdMsgEvent_advance(this);
        return;
    }
    u32 song = hdMsgEvent_game()->play.mMelodyNum;
    u32 expected = lengthIndex(song);
    u32 actual = hdMsgEvent_songLength();
    if (s32(mProgress) < s32(expected)) {
        if (!(expected == actual && mBeatCount == actual)) {
            for (u32 i = 0; s32(i) < s32(mShownPaneCount); ++i) {
                u32 pane = paneIndex(mPaneRow, i);
                hidePane(pane);
            }
            mBeatCount = actual;
            mCompleted = 0;
            mProgress = 0;
            mShownPaneCount = 0;
            mInputRow = actual == 3 ? 0 : actual == 4 ? 1 : 2;
        }
        // HD: rhythm flags and note number are verified partial player views.
        hdMsgEvent_actor_c* actor = reinterpret_cast<hdMsgEvent_actor_c*>(
            (fopAc_ac_c*)hdMsgEvent_game()->play.mPlayerInfo[0].mpPlayer);
        if (actor->mRhythmFlags & EVENT_RHYTHM_NOTE) {
            hdMsgEvent_player_c* player = reinterpret_cast<hdMsgEvent_player_c*>(
                (fopAc_ac_c*)hdMsgEvent_game()->play.mpPlayerPtr[0]);
            u32 note = u32(s32(player->mCurrentNote));
            gabi::Local<be<u32>> wanted;
            *wanted.get() = 0xFFFFFFFF;
            // HD: the length-returning audio call has an unused result at this call site.
            if (song != 255) hdMsgEvent_noteLength(song, mProgress, wanted.get());
            if (note == u32(*wanted.get()) && expected == actual) {
                u32 completed = mCompleted;
                u32 progress = mProgress;
                if (completed == progress) {
                    beatAge(completed) = 1;
                    if (mInputRow == mPaneRow) {
                        u32 pane = paneIndex(mPaneRow, mCompleted);
                        showPane(pane);
                        mShownPaneCount = u32(mShownPaneCount) + 1;
                    }
                    mCompleted = u32(mCompleted) + 1;
                }
            }
            u32 progress = u32(mProgress) + 1;
            mProgress = progress;
            if (s32(progress) >= s32(actual)) {
                player = reinterpret_cast<hdMsgEvent_player_c*>(
                    (fopAc_ac_c*)hdMsgEvent_game()->play.mpPlayerPtr[0]);
                if (!hdMsgEvent_playerAction(player)) {
                    if (mInputRow == mPaneRow) {
                        for (u32 i = 0; s32(i) < s32(mShownPaneCount); ++i) {
                            u32 pane = paneIndex(mPaneRow, i);
                            hidePane(pane);
                        }
                    }
                    mShownPaneCount = 0;
                    mProgress = 0;
                    mCompleted = 0;
                    for (u32 i = 0; i < 6; ++i) mBeatAges[i] = 0;
                }
            }
        } else if (song != 1 && song != 5 && song != 6 && song != 7) {
            hdMsgEvent_message_c* message = mpMessage;
            u8 ready = message->mReady;
            void* controller = sCore.mpCore;
            if (ready) {
                hdMsgEvent_setStatus(controller, EVENT_STATUS_ADVANCE);
                mFrame = 0;
                playEventSound(0);
                hdMsgEvent_flush();
                hdMsgEvent_advance(this);
                hdMsgEvent_changeState(&mStateMachine, &sAdvance);
                return;
            }
        }
    }
    s32 completed = mCompleted;
    if (completed > 0) {
        u32 index = u32(completed) - 1;
        be<u32>* ageValue = &beatAge(index);
        s32 age = *ageValue;
        if (age <= 5) *ageValue = u32(age) + 1;
        else if (index == expected - 1) {
            hdMsgEvent_setStatus(sCore.mpCore, EVENT_STATUS_ADVANCE);
            hdMsgEvent_changeState(&mStateMachine, &sSequenceDone);
        }
    }
}

/* 026B2EB4 */
void hdMsgEvent_window_c::updateTyping() {
    void* controller = sCore.mpCore;
    s32 delta = hdMsgEvent_typingDelta(this);
    if (!delta) return;
    s32 count = mShownCharacters;
    if (count < 335) {
        u32 index = u32(mStopIndex) + 1;
        count = s32(u32(count) + u32(delta));
        mShownCharacters = count;
        be<u32>* limit = &stopPoint(index);
        if (count > s32(*limit)) mShownCharacters = u32(*limit);
    }
    hdMsgEvent_refreshText(this);
    hdMsgEvent_message_c* message = mpMessage;
    if (message->mUpdating) return;
    switch (u32(message->mStatus)) {
    case EVENT_STATUS_NEXT:
        hdMsgEvent_changeState(&mStateMachine, &sNext);
        break;
    case EVENT_STATUS_WAIT:
        if (message->mWait) hdMsgEvent_changeState(&mStateMachine, &sWait);
        else if (message->mWaitInput) hdMsgEvent_changeState(&mStateMachine, &sWaitInput);
        break;
    case EVENT_STATUS_SPECIAL:
        message->mStatus = EVENT_STATUS_WAIT;
        hdMsgEvent_setStatus(controller, EVENT_STATUS_WAIT);
        hdMsgEvent_changeState(&mStateMachine, &sSpecialWait);
        break;
    case EVENT_STATUS_ADVANCE:
        if (mWindowType == 2) {
            message->mStatus = EVENT_STATUS_WAIT;
            hdMsgEvent_setStatus(controller, EVENT_STATUS_WAIT);
            hdMsgEvent_changeState(&mStateMachine, &sSpecialWait);
        } else {
            hdMsgEvent_setStatus(controller, message->mStatus);
            hdMsgEvent_changeState(&mStateMachine, &sEnd);
        }
        break;
    }
}

/* 026B30FC */
void hdMsgEvent_window_c::updateCloseWait() {
    if (hdMsgEvent_waitAnimation(this, 4)) {
        if (mWindowType == 2) {
            hdMsgEvent_manager_c* manager = sManager.mpManager;
            manager->mOpenCount = u32(manager->mOpenCount) - 1;
        }
        hdMsgEvent_changeState(&mStateMachine, &sClosed);
    }
}

/* 026B3238 */
void hdMsgEvent_window_c::advanceStopPoint() {
    mStopIndex = u32(mStopIndex) + 1;
    hdMsgEvent_message_c* message = mpMessage;
    mShownCharacters = 0;
    u32 color = message->mColor;
    hdMsgEvent_pane_c* pane = mpTextPane;
    pane->mColorTop = color;
    pane->mColorBottom = color;
    hdMsgEvent_refreshText(this);
    hdMsgEvent_setAnimation(this, 2, 1);
}

/* 026B3324 */
void hdMsgEvent_window_c::updateInput() {
    bool advanceInput = (mInputFlags & EVENT_INPUT_CONFIRM) || mpMessage->mAdvanceRequested || mAdvanceRequested;
    if (!advanceInput && mMessageId == 0x5AC) advanceInput = hdMsgEvent_game()->play.mMesgCancelButton != 0;
    if (advanceInput && !hdMsgEvent_inputBlocked()) {
        mpMessage->mAdvanceRequested = 0;
        hdMsgEvent_changeState(&mStateMachine, &sAdvance);
        sInputIndicator = 0;
        hdMsgEvent_advance(this);
        return;
    }
    sInputIndicator = 4;
}

/* WWHD glue
data sSongLengths 100FD9BC nocheck
data sPaneIndices 100FD9DC nocheck
data sCore 101F4B5C nocheck
data sManager 101D5FE8 nocheck
data sInputBusy 1047B097 nocheck
data sInputIndicator 1047B09E nocheck
data sWait 1049E22C nocheck
data sBeatDisplayDone 1049845C nocheck
data sShortWaitDone 1049848C nocheck
data sSequenceDone 104984BC nocheck
data sAdvance 1049854C nocheck
data sClosed 1049857C nocheck
data sNext 1049DF2C nocheck
data sWaitInput 1049E28C nocheck
data sSpecialWait 1049E1CC nocheck
data sEnd 1049E2BC nocheck
ext 020063C0 void hdMsgEvent_changeState(hdMsgEvent_stateMachine_c* machine, hdMsgEvent_state_c* state)
ext 025F74D0 void hdMsgEvent_setStatus(void* core, u32 status)
ext 025E1988 void hdMsgEvent_sound(u32 sound)
ext 025E1C64 void hdMsgEvent_flush()
ext 025E1F58 void hdMsgEvent_prepareSong(u32 song)
ext 025E1F68 f32 hdMsgEvent_noteLength(u32 song, u32 index, be<u32>* note)
ext 02621388 void hdMsgEvent_resetPane(hdMsgEvent_pane_c* pane)
ext 02621394 void hdMsgEvent_showPane(hdMsgEvent_pane_c* pane)
ext 026213A4 void hdMsgEvent_hidePane(hdMsgEvent_pane_c* pane)
ext 027161BC u32 hdMsgEvent_inputBusy()
ext 025E1EFC u32 hdMsgEvent_songLength()
ext 0244311C u32 hdMsgEvent_playerAction(hdMsgEvent_player_c* player)
ext 026FCB00 s32 hdMsgEvent_typingDelta(hdMsgEvent_window_c* window)
ext 026B2CBC void hdMsgEvent_refreshText(hdMsgEvent_window_c* window)
ext 026FF668 u32 hdMsgEvent_waitAnimation(hdMsgEvent_window_c* window, u32 animation)
ext 026FF5EC void hdMsgEvent_setAnimation(hdMsgEvent_window_c* window, u32 animation, u32 flags)
*/

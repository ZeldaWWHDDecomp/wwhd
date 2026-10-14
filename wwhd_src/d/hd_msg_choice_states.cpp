/**
 * hd_msg_choice_states.cpp
 * HD - the talk window with choices: its state functions (open, reopen, wait, selection, close),
 * animations and the constructor
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_choice_window.h"
#include "d/hd_screen_msg_cursor.h"
#include "d/hd_msg_window_a.h"
using namespace gabi;
struct hdTalkChoiceAnimationSlots_c { be<u32> mSlots[4]; };
struct hdTalkChoiceCore_c { gptr<void> mpCore; };
static hdTalkChoiceAnimationSlots_c sTalkChoiceAnimationSlots;
static hdTalkChoiceCore_c sTalkChoiceCore;
static hdPartsStateId_c sTalkChoiceSelectState;
static hdPartsStateId_c sTalkChoiceWaitState;
static hdPartsStateId_c sTalkChoiceClosedState;
static hdPartsStateId_c sTalkChoiceBlankState;
static f32 sTalkChoiceInitialFrame;
extern void talkChoiceSetMode(void*, u32);
extern void talkChoiceEndSound();

/* 026ACD74 */
u32 hdMsgChoiceWindow_c::isAnimationEnded(u32 animation) {
    return hdAnimSet_isEnd(animSet(), animation);
}

/* 026ACD80 */
void hdMsgChoiceWindow_c::updateOpen() {
    if (isAnimationEnded(0))
        changeState(mChoiceMode == 1 ? &sTalkChoiceSelectState : &sTalkChoiceWaitState);
}

/* 026ACECC */
void hdMsgChoiceWindow_c::updateReopen() {
    if (isAnimationEnded(0))
        changeState(mChoiceMode == 1 ? &sTalkChoiceSelectState : &sTalkChoiceWaitState);
}

/* 026AD5C4 */
void hdMsgChoiceWindow_c::enterClose() {
    playAnimation(2);
    mClosing = 0;
    talkChoiceEndSound();
}

/* 026AD600 */
void hdMsgChoiceWindow_c::updateClose() {
    if (!isAnimationEnded(2)) return;
    talkChoiceSetMode(sTalkChoiceCore.mpCore, 18);
    changeState(&sTalkChoiceClosedState);
}

/* 026AD65C */
void hdMsgChoiceWindow_c::setAnimationFrame(u32 animation, f32 frame) {
    hdAnimSet* animations = animSet();
    u32 slot = sTalkChoiceAnimationSlots.mSlots[animation];
    hdAnimSet_setFrame(animations, animation, slot, frame);
}

/* 026AD678 */
void hdMsgChoiceWindow_c::hide() {
    setAnimationFrame(0, sTalkChoiceInitialFrame);
    vCalcLayout();
    hdTextPane_c* pane = mpWindow;
    pane->mPaneFlags = pane->mPaneFlags & ~1u;
}

/* 026AD6D4 */
void hdMsgChoiceWindow_c::finishMessage() {
    hide();
    hdMessageGetPlay()->mWindowSlot = 0xFF;
    hdMessageGetPlay()->mMessageId = 0xFFFFFFFF;
    hdMessageGetPlay()->mWindowActive = 0;
    hdMessageGetPlay()->mMessageFlags = 0;
    changeState(&sTalkChoiceBlankState);
}

/* 026AD744 */
void hdMsgChoiceWindow_c::resetMessage() {
    talkChoiceSetMode(sTalkChoiceCore.mpCore, 0);
    finishMessage();
}
extern void* talkChoiceAllocate(u32);
static hdPartsVtbl_c sTalkChoiceVtable;
static u8 sTalkChoiceStateVtable;
static u8 sTalkChoiceSecondaryVtable;

/* 026AD784 */
hdMsgChoiceWindow_c* hdMsgChoiceWindow_c::construct(hdMsgChoiceWindow_c* object) {
    if (!object) {
        object = static_cast<hdMsgChoiceWindow_c*>(talkChoiceAllocate(sizeof(hdMsgChoiceWindow_c)));
        if (!object) return object;
    }
    hdParts_c::construct(object);
    object->mVtable = &sTalkChoiceVtable;
    object->mStateHolder.mHolderVtable = &sTalkChoiceStateVtable;
    object->mSecondaryVtable = &sTalkChoiceSecondaryVtable;
    object->mMessageId = 0;
    object->mInputMode = 0;
    object->mInputFlags = 0;
    object->mFlags = 0;
    // GHS's inlined fixed-array constructors retain their null-storage allocation paths.
    if (ea(&object->mpWindow) == 0) talkChoiceAllocate(8);
    if (ea(object->mpPanes) == 0) talkChoiceAllocate(16);
    object->mSelectedChoice = 0;
    object->mSelection = 0;
    object->mClosing = 0;
    object->mOnTv = 1;
    object->mChoiceChanged = 0;
    object->mpMessage = nullptr;
    object->mChoiceActive = 0;
    object->mFirstChoiceSound = 0;
    object->mChoiceMode = 0xFFFFFFFF;
    object->mSecondChoiceSound = 0;
    object->mCancelAllowed = 0;
    object->mpWindow = nullptr;
    object->mpChoiceRoot = nullptr;
    for (u32 index = 0; index < 4; ++index) object->mpPanes[index] = nullptr;
    return object;
}
struct hdTalkSoundRecord_c { u8 mReserved[0xA]; be<u8> mSound; };
struct hdTalkMessageRecord_c {
    be<u32> mStatus;
    u8 mReserved4[8];
    PaneString_l mName;
    u8 mReserved18[0x108];
    PaneString_l mLabel;
    u8 mReserved12C[0x100];
    gptr<hdTalkSoundRecord_c> mpSound;
};
struct hdTalkMessageService_c { gptr<void> mpService; };
struct hdTalkCancelPlay_c { u8 mReserved[0x5BD3]; be<u8> mCancelled; };
static hdTalkMessageService_c sTalkMessageService;
static s8 sTalkCurrentRoom;
static hdPartsStateId_c sTalkChoiceCloseState;
extern void talkChoiceBeginSound();
extern void talkChoiceSound(u32);
extern void talkChoiceNamedSound(const char*);
extern void talkChoiceResolveSound(void*, PaneWork_l*, PaneString_l*, PaneString_l*);
extern u32 talkChoiceReverb(s32);
extern void talkChoiceVoice(u32, u32, u32);

/* 026ACDDC */
void hdMsgChoiceWindow_c::enterReopen() {
    hdTextPane_c* pane = mpWindow;
    pane->mPaneFlags = (pane->mPaneFlags & ~1u) + 1;
    prepareChoices();
    playAnimation(0);
    talkChoiceBeginSound();
    if (mOnTv) talkChoiceSound(0x80C);
    else talkChoiceNamedSound("SE_TALK_WIN_OPEN_DRC");
    PaneWork_l* message = mpMessage;
    auto* record = reinterpret_cast<hdTalkMessageRecord_c*>(message);
    talkChoiceResolveSound(sTalkMessageService.mpService, message, &record->mName, &record->mLabel);
    record = reinterpret_cast<hdTalkMessageRecord_c*>(static_cast<PaneWork_l*>(mpMessage));
    hdTalkSoundRecord_c* sound = record->mpSound;
    if (sound && sound->mSound != 0) {
        u32 reverb = talkChoiceReverb(sTalkCurrentRoom);
        talkChoiceVoice(sound->mSound, 0, reverb);
    }
}

/* 026AD438 */
void hdMsgChoiceWindow_c::updateWait() {
    u32 input = mInputFlags;
    void* core = sTalkChoiceCore.mpCore;
    if (input & 1) {
        talkChoiceSetMode(core, 14);
        hdMessageGetPlay()->mAdvanceCount = 1;
        if (mOnTv) talkChoiceSound(0x80D);
        else talkChoiceNamedSound("SE_TALK_WIN_CLOSE_DRC");
        changeState(&sTalkChoiceCloseState);
        return;
    }
    if (input & 2) {
        talkChoiceSetMode(core, 14);
        reinterpret_cast<hdTalkCancelPlay_c*>(hdMessageGetPlay())->mCancelled = 1;
        if (mOnTv) talkChoiceSound(0x880);
        else talkChoiceNamedSound("SE_TALK_SEL_CANCEL_DRC");
        talkChoiceEndSound();
        hdMessageGetPlay()->mAdvanceCount = 1;
        changeState(&sTalkChoiceCloseState);
        return;
    }
    u32 mode = mChoiceMode;
    PaneWork_l* message = mpMessage;
    mpPanes[mode < 4 ? mode : 0]->refresh(message);
}
struct hdTalkSelectionCore_c {
    u8 mReserved[0x948];
    be<u32> mSelection;
    u8 mReserved94C[0x18];
    gptr<hdScreenMsgCursor_c> mpCursor;
};
enum TalkChoiceInput {
    TalkConfirm = 1, TalkCancel = 2,
    TalkLeft = 0x00400000, TalkRight = 0x00800000,
    TalkUp = 0x00040000, TalkDown = 0x00080000
};

/* 026AD0C0 */
void hdMsgChoiceWindow_c::updateSelection() {
    u32 input = mInputFlags;
    auto* core = reinterpret_cast<hdTalkSelectionCore_c*>(static_cast<void*>(sTalkChoiceCore.mpCore));
    if (input & TalkConfirm) {
        if (core->mpCursor->isLooping()) {
            core->mSelection = mSelection;
            talkChoiceSetMode(core, 14);
            hdMessageGetPlay()->mAdvanceCount = 1;
            u32 selection = mSelection;
            u32 useSound = mOnTv;
            if (selection == 0) {
                if (useSound) {
                    u32 sound = mFirstChoiceSound;
                    talkChoiceSound(sound != 0 ? sound : 0x880);
                } else talkChoiceNamedSound("SE_TALK_SEL_CANCEL_DRC");
            } else {
                if (useSound) {
                    u32 sound = mSecondChoiceSound;
                    talkChoiceSound(sound != 0 ? sound : 0x80D);
                } else talkChoiceNamedSound("SE_TALK_WIN_CLOSE_DRC");
            }
            changeState(&sTalkChoiceCloseState);
            return;
        }
        if (mCancelAllowed) {
            input = mInputFlags;
            if (input & TalkCancel) {
                talkChoiceSetMode(core, 14);
                reinterpret_cast<hdTalkCancelPlay_c*>(hdMessageGetPlay())->mCancelled = 1;
                if (mOnTv) talkChoiceSound(0x880);
                else talkChoiceNamedSound("SE_TALK_SEL_CANCEL_DRC");
                talkChoiceEndSound();
                hdMessageGetPlay()->mAdvanceCount = 1;
                core->mSelection = 0;
                mClosing = 0;
                changeState(&sTalkChoiceCloseState);
                return;
            }
        } else input = mInputFlags;
    } else if (mCancelAllowed && (input & TalkCancel)) {
        talkChoiceSetMode(core, 14);
        reinterpret_cast<hdTalkCancelPlay_c*>(hdMessageGetPlay())->mCancelled = 1;
        if (mOnTv) talkChoiceSound(0x880);
        else talkChoiceNamedSound("SE_TALK_SEL_CANCEL_DRC");
        talkChoiceEndSound();
        hdMessageGetPlay()->mAdvanceCount = 1;
        core->mSelection = 0;
        mClosing = 0;
        changeState(&sTalkChoiceCloseState);
        return;
    }
    u32 previous = mSelection;
    u32 selection = previous;
    if ((input & TalkLeft) || (!(input & TalkRight) && (input & TalkUp))) {
        selection = 0;
        mSelection = 0;
    } else if (input & (TalkRight | TalkDown)) {
        selection = 1;
        mSelection = 1;
    }
    if (selection != previous) {
        if (mOnTv) talkChoiceSound(0x83D);
        else talkChoiceNamedSound("SE_TALK_CURSOR_DRC");
    }
    u32 mode = mChoiceMode;
    PaneWork_l* message = mpMessage;
    mpPanes[mode < 4 ? mode : 0]->refresh(message);
}
struct hdTalkTransform_c { be<u32> mWords[12]; };
struct hdTalkPaneTransform_c { u8 mReserved[0x48]; hdTalkTransform_c mTransform; };
struct hdTalkChoicePositions_c { be<f32> mLeftX, mLeftY, mLeftZ, mRightX, mRightY, mRightZ, mCursorX, mCursorY, mCursorZ; };
struct hdTalkChoiceFont_c { u8 mReserved[4]; gptr<gfn> mpVtable; };
struct hdTalkChoiceCount_c { u8 mReserved[0x6BC]; be<u32> mSelection; };
extern hdTalkChoiceFont_c* talkChoiceGetFont(hdTextPane_c*);
extern void talkChoiceConfigureCursor(void*, hdTalkChoicePositions_c*, PaneString_l*, PaneString_l*, hdTalkChoiceFont_c*, f32);

/* 026ACF28 */
void hdMsgChoiceWindow_c::enterSelection() {
    PaneWork_l* message = mpMessage;
    mClosing = 1;
    mSelection = reinterpret_cast<hdTalkChoiceCount_c*>(message)->mSelection;
    hdTextPane_c* leftPane = mpPanes[2];
    hdTextPane_c* rightPane = mpPanes[3];
    hdTalkTransform_c leftTransform;
    hdTalkTransform_c rightTransform;
    auto* left = &reinterpret_cast<hdTalkPaneTransform_c*>(leftPane)->mTransform;
    auto* right = &reinterpret_cast<hdTalkPaneTransform_c*>(rightPane)->mTransform;
    if (left != &leftTransform) {
        for (u32 group = 0; group < 3; ++group)
            for (u32 word = 0; word < 4; ++word)
                leftTransform.mWords[group * 4 + word] = left->mWords[group * 4 + word].get();
    }
    if (right != &rightTransform) {
        for (u32 group = 0; group < 3; ++group)
            for (u32 word = 0; word < 4; ++word)
                rightTransform.mWords[group * 4 + word] = right->mWords[group * 4 + word].get();
    }
    hdTalkChoicePositions_c positions;
    positions.mLeftY = *reinterpret_cast<be<f32>*>(&leftTransform.mWords[7]);
    f32 leftX = *reinterpret_cast<be<f32>*>(&leftTransform.mWords[3]);
    f32 zero = sTalkChoiceInitialFrame;
    positions.mCursorY = zero;
    positions.mCursorZ = zero;
    positions.mLeftZ = zero;
    positions.mRightZ = zero;
    positions.mCursorX = zero;
    positions.mLeftX = leftX;
    positions.mRightY = *reinterpret_cast<be<f32>*>(&rightTransform.mWords[7]);
    positions.mRightX = *reinterpret_cast<be<f32>*>(&rightTransform.mWords[3]);
    hdTalkChoiceFont_c* font = talkChoiceGetFont(leftPane);
    u32 heightGetter = font->mpVtable[0x1C / 4].get();
    f32 width = leftPane->mFontWidth;
    s32 height = call_ptr<s32>(heightGetter, font);
    message = mpMessage;
    auto* text = reinterpret_cast<hdTalkChoiceText_c*>(message);
    f32 scale = width / f32(height);
    talkChoiceConfigureCursor(sTalkChoiceCore.mpCore, &positions, &text->mRight, &text->mLeft, font, scale);
}
struct hdTalkChoiceNames_c { hdUiSafeString mItems[2]; };
struct hdTalkChoiceRegistration_c { be<u32> mWords[3]; };
static hdTalkChoiceNames_c sTalkChoiceWindowNames;
static u32 sTalkChoiceWindowNamesInitialized;
static hdTalkChoiceRegistration_c sTalkChoiceWindowNameRegistration;
extern void talkChoiceRegister(hdTalkChoiceRegistration_c*);
static hdUiStringTable sTalkChoiceStringVtable;

/* 026AD890 */
void hdMsgChoiceWindow_c::findWindowPanes() {
    if (!sTalkChoiceWindowNamesInitialized) {
        sTalkChoiceWindowNamesInitialized = 1;
        sTalkChoiceWindowNames.mItems[0].mVtable = &sTalkChoiceStringVtable;
        sTalkChoiceWindowNames.mItems[0].mText.v = guest_str("N_All_00");
        sTalkChoiceWindowNames.mItems[1].mVtable = &sTalkChoiceStringVtable;
        sTalkChoiceWindowNames.mItems[1].mText.v = guest_str("N_ChoiseLR_00");
        talkChoiceRegister(&sTalkChoiceWindowNameRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 index = 0; index < 2; ++index) {
        hdUiSafeString* name = &sTalkChoiceWindowNames.mItems[index];
        hdPartsPaneSRTVtbl_c* vtable = root->srtVtbl();
        name->assureTermination();
        hdTextPane_c* pane = call_ptr<hdTextPane_c*>(vtable->mFindPane.get(), root, name->mText.get(), 1);
        if (index == 0) mpWindow = pane;
        else mpChoiceRoot = pane;
    }
}

/* WWHD glue
 data sTalkChoiceVtable 100FCD90
 data sTalkChoiceStateVtable 100FCEE8
 data sTalkChoiceSecondaryVtable 100FCEF8
 data sTalkChoiceStringVtable 100FCA8C
 data sTalkChoiceWindowNames 10497CB8
 data sTalkChoiceWindowNamesInitialized 10497D38
 data sTalkChoiceWindowNameRegistration 101F64F8
 str 100FCC84 "N_All_00"
 str 100FCC74 "N_ChoiseLR_00"
 ext 028F026C void talkChoiceRegister(hdTalkChoiceRegistration_c*)
 ext 02878098 hdTalkChoiceFont_c* talkChoiceGetFont(hdTextPane_c*)
 ext 025F85B8 void talkChoiceConfigureCursor(void*,hdTalkChoicePositions_c*,PaneString_l*,PaneString_l*,hdTalkChoiceFont_c*,f32)
 ext 0265233C u32 hdScreenMsgCursor_c::isLooping()
 str 100FCC18 "SE_TALK_SEL_CANCEL_DRC" in 026AD0C0
 str 100FCC00 "SE_TALK_WIN_CLOSE_DRC" in 026AD0C0
 str 100FCC30 "SE_TALK_CURSOR_DRC" in 026AD0C0
 data sTalkMessageService 101F4AE8
 data sTalkCurrentRoom 1047E6C8
 data sTalkChoiceCloseState 10497E00
 str 100FCBE0 "SE_TALK_WIN_OPEN_DRC" in 026ACDDC
 str 100FCC44 "SE_TALK_WIN_CLOSE_DRC" in 026AD438
 str 100FCC5C "SE_TALK_SEL_CANCEL_DRC" in 026AD438
 ext 025E1C58 void talkChoiceBeginSound()
 ext 025E1988 void talkChoiceSound(u32)
 ext 02030B38 void talkChoiceNamedSound(const char*)
 ext 025F50E8 void talkChoiceResolveSound(void*,PaneWork_l*,PaneString_l*,PaneString_l*)
 ext 02520540 u32 talkChoiceReverb(s32)
 ext 025E1C3C void talkChoiceVoice(u32,u32,u32)
 ext 0270477C void hdTextPane_c::refresh(PaneWork_l*)
 ext 0273AD10 void* talkChoiceAllocate(u32)
 data sTalkChoiceAnimationSlots 100FCB1C
 data sTalkChoiceCore 101F4B5C
 data sTalkChoiceSelectState 10497DA0
 data sTalkChoiceWaitState 10497DD0
 data sTalkChoiceClosedState 10497E30
 data sTalkChoiceBlankState 1049DD60
 data sTalkChoiceInitialFrame 100FCBBC
 ext 025F74D0 void talkChoiceSetMode(void*,u32)
 ext 025E1C64 void talkChoiceEndSound()
 ext 025200D4 HdMessagePlay* hdMessageGetPlay()
 ext 026F89E8 hdParts_c* hdParts_c::construct(hdParts_c*)
*/

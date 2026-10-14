/**
 * hd_msg_choice_window.cpp
 * HD - the talk window with choices (hdMsgChoiceWindow_c): exit-time destruction of its name tables,
 * the deleting destructor, the choice texts of a message and the window's opening (sound, voice)
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_choice_window.h"
using namespace gabi;
static u8 sChoiceStateVtable;
static u8 sChoiceSecondaryVtable;
static hdUiStringTable sChoiceSourceStringVtable;
static hdUiStringTable sChoiceDummyStringVtable;
static hdUiStringTable sChoiceDummyOwnedVtable;
struct hdChoiceArchiveNames_c { hdUiSafeString mNames[2]; };
static hdChoiceArchiveNames_c sChoiceArchiveNames;
struct hdChoicePaneNames_c { hdUiSafeString mNames[4]; };
static hdChoicePaneNames_c sChoicePaneNames;
struct hdChoiceAnimationSlots_c { be<u32> mSlots[4]; };
static hdChoiceAnimationSlots_c sChoiceAnimationSlots;
static f32 sChoiceStartFrame;
static f32 sChoiceAnimationRate;
struct hdChoiceCorePointer_c { gptr<void> mpCore; };
static hdChoiceCorePointer_c sChoiceCore;
extern void choiceDestroyString(void*);
using ChoiceStringDestructor = void (*)(void*);
extern void choiceDestroyArray(void*, u32, u32, ChoiceStringDestructor, u32, u32);
extern void choiceFree(void*);
extern void choiceClearWork(PaneWork_l*);
extern void choiceSelectWork(PaneWork_l*, u32, u32);
extern void choiceSetMode(void*, u32);
extern void choiceStartAnimation(hdAnimSet*, u32, u32, f32);

/* 026AC870 */
static void destroyChoiceArchiveNames() {
    choiceDestroyArray(&sChoiceArchiveNames, 2, sizeof(hdUiSafeString), choiceDestroyString, 0, 0);
}

/* 026AC894 */
static void destroyChoicePaneNames() {
    choiceDestroyArray(&sChoicePaneNames, 4, sizeof(hdUiSafeString), choiceDestroyString, 0, 0);
}

/* 026AC8B8 */
void hdMsgChoiceWindow_c::destroy(u32 flags) {
    if (!this) return;
    mStateHolder.mHolderVtable = &sChoiceStateVtable;
    mSecondaryVtable = &sChoiceSecondaryVtable;
    hdParts_c::destroy(this, 0);
    if (flags & 1) choiceFree(this);
}

/* 026AC924 */
void hdMsgChoiceWindow_c::prepareChoices() {
    for (u32 index = 0; index < 4; ++index) {
        hdTextPane_c* pane = mpPanes[index];
        pane->mPaneFlags = pane->mPaneFlags & ~1u;
    }
    u32 mode = mChoiceMode;
    hdTextPane_c* selected = mpPanes[mode < 4 ? mode : 0];
    selected->prepareMessage(mpMessage, 0);
    if (mChoiceMode == 1) {
        for (u32 index = 2; index < 4; ++index) {
            hdTextPane_c* pane = mpPanes[index];
            PaneWork_l* work = &pane->mWork;
            choiceClearWork(work);
            pane = mpPanes[index];
            // Preserve the original immediate floating-point copy, including signaling NaN bits.
            reinterpret_cast<hdTalkChoiceWidth_c*>(work)->mWidthBits =
                reinterpret_cast<hdTalkPaneWidth_c*>(pane)->mWidthBits.get();
            choiceSelectWork(work, 0, 0);
            auto* text = reinterpret_cast<hdTalkChoiceText_c*>(static_cast<PaneWork_l*>(mpMessage));
            PaneString_l* label = index == 2 ? &text->mLeft : &text->mRight;
            mpPanes[index]->setStringPrepared(label, work);
            pane = mpPanes[index];
            pane->mPaneFlags = (pane->mPaneFlags & ~1u) + 1;
        }
    }
    PaneWork_l* message = mpMessage;
    // The status word is the first word of the conversion workspace.
    choiceSetMode(sChoiceCore.mpCore, *reinterpret_cast<be<u32>*>(message));
    mode = mChoiceMode;
    selected = mpPanes[mode < 4 ? mode : 0];
    selected->mPaneFlags = (selected->mPaneFlags & ~1u) + 1;
}

/* 026ACAE0 */
void hdMsgChoiceWindow_c::playAnimation(u32 animation) {
    u32 slot = sChoiceAnimationSlots.mSlots[animation];
    choiceStartAnimation(animSet(), animation, slot, sChoiceStartFrame);
    hdAnimSet_play(animSet(), animation, slot, sChoiceAnimationRate);
}
struct hdChoiceDummy_c {
    hdUiSafeString mString;
    be<s32> mCapacity;
    be<u8> mBuffer[6];
    u8 mPadding[2];
};
struct hdChoiceDummyRegistration_c { be<u32> mWords[3]; };
struct hdChoiceOpenSound_c { u8 mReserved[0xA]; be<u8> mSound; };
struct hdChoiceOpenMessage_c {
    u8 mReserved[0x120];
    PaneString_l mLabel;
    u8 mReserved12C[0x100];
    gptr<hdChoiceOpenSound_c> mpSound;
};
struct hdChoiceMessageService_c { gptr<void> mpService; };
static hdChoiceDummy_c sChoiceDummy;
static u32 sChoiceDummyInitialized;
static hdChoiceDummyRegistration_c sChoiceDummyRegistration;
static hdChoiceMessageService_c sChoiceMessageService;
static s8 sChoiceRoom;
extern void choiceOpenBeginSound();
extern void choiceOpenSound(u32);
extern void choiceOpenNamedSound(const char*);
extern void choiceOpenResolveSound(void*, PaneWork_l*, hdUiSafeString*, PaneString_l*);
extern void choiceOpenAssureSource(hdUiSafeString*);
extern void choiceOpenCopy(be<u8>*, be<u8>*, u32, u32);
extern void choiceOpenRegister(hdChoiceDummyRegistration_c*);
extern u32 choiceOpenReverb(s32);
extern void choiceOpenVoice(u32, u32, u32);
enum { ChoiceStringScanLimit = 0x40000 };

/* 026ACB60 */
void hdMsgChoiceWindow_c::enterOpen() {
    hdTextPane_c* pane = mpWindow;
    pane->mPaneFlags = (pane->mPaneFlags & ~1u) + 1;
    choiceSelectWork(mpMessage, mMessageId, 0);
    prepareChoices();
    playAnimation(0);
    choiceOpenBeginSound();
    if (mOnTv) choiceOpenSound(0x80C);
    else choiceOpenNamedSound("SE_TALK_WIN_OPEN_DRC");
    if (!sChoiceDummyInitialized) {
        sChoiceDummy.mBuffer[5] = 0;
        hdUiSafeString source;
        source.mVtable = &sChoiceSourceStringVtable;
        source.mText.v = guest_str("dummy");
        sChoiceDummy.mString.mText = sChoiceDummy.mBuffer;
        sChoiceDummy.mCapacity = 6;
        sChoiceDummy.mString.mVtable = &sChoiceDummyStringVtable;
        sChoiceDummyInitialized = 1;
        choiceOpenAssureSource(&source);
        be<u8>* text = source.mText;
        s32 length = 0;
        if (*text != 0) {
            do {
                ++length;
                text = hdPartsView<be<u8>>(ea(text) + 1);
                if (length > ChoiceStringScanLimit) {
                    length = 0;
                    break;
                }
            } while (*text != 0);
        }
        s32 capacity = sChoiceDummy.mCapacity;
        u32 assure = source.mVtable->mAssureTermination.get();
        if (length >= capacity) length = s32(u32(capacity) - 1);
        call_ptr<void>(assure, &source);
        choiceOpenCopy(sChoiceDummy.mBuffer, source.mText, u32(length), 0);
        *hdPartsView<be<u8>>(ea(sChoiceDummy.mBuffer) + u32(length)) = 0;
        sChoiceDummy.mString.mVtable = &sChoiceDummyOwnedVtable;
        choiceOpenRegister(&sChoiceDummyRegistration);
    }
    PaneWork_l* message = mpMessage;
    auto* record = reinterpret_cast<hdChoiceOpenMessage_c*>(message);
    choiceOpenResolveSound(sChoiceMessageService.mpService, message, &sChoiceDummy.mString, &record->mLabel);
    record = reinterpret_cast<hdChoiceOpenMessage_c*>(static_cast<PaneWork_l*>(mpMessage));
    hdChoiceOpenSound_c* sound = record->mpSound;
    if (sound && sound->mSound != 0) {
        u32 reverb = choiceOpenReverb(sChoiceRoom);
        choiceOpenVoice(sound->mSound, 0, reverb);
    }
}

/* WWHD glue
 data sChoiceStateVtable 100FCEE8
 data sChoiceSecondaryVtable 100FCEF8
 data sChoiceSourceStringVtable 100FCA8C
 data sChoiceDummyStringVtable 100FCAEC
 data sChoiceDummyOwnedVtable 100FCB04
 data sChoiceDummy 10497D20
 data sChoiceDummyInitialized 10497D34
 data sChoiceDummyRegistration 101F64EC
 data sChoiceMessageService 101F4AE8
 data sChoiceRoom 1047E6C8
 str 100FCBC8 "SE_TALK_WIN_OPEN_DRC"
 str 100FCBC0 "dummy"
 ext 025E1C58 void choiceOpenBeginSound()
 ext 025E1988 void choiceOpenSound(u32)
 ext 02030B38 void choiceOpenNamedSound(const char*)
 ext 025F50E8 void choiceOpenResolveSound(void*,PaneWork_l*,hdUiSafeString*,PaneString_l*)
 ext 026AE920 void choiceOpenAssureSource(hdUiSafeString*)
 ext C0009988 void choiceOpenCopy(be<u8>*,be<u8>*,u32,u32)
 ext 028F026C void choiceOpenRegister(hdChoiceDummyRegistration_c*)
 ext 02520540 u32 choiceOpenReverb(s32)
 ext 025E1C3C void choiceOpenVoice(u32,u32,u32)
 data sChoiceArchiveNames 10497CB8
 data sChoicePaneNames 10497CC8
 data sChoiceAnimationSlots 100FCB1C
 data sChoiceStartFrame 100FCBBC
 data sChoiceAnimationRate 100FCBB8
 data sChoiceCore 101F4B5C
 ext 026AE8F8 void choiceDestroyString(void*) callback
 ext 028F0164 void choiceDestroyArray(void*,u32,u32,ChoiceStringDestructor,u32,u32)
 ext 0273AF40 void choiceFree(void*)
 ext 026F88F0 void hdParts_c::destroy(hdParts_c*,s32)
 ext 02704504 void hdTextPane_c::prepareMessage(PaneWork_l*,u32)
 ext 027046D4 void hdTextPane_c::setStringPrepared(PaneString_l*,PaneWork_l*)
 ext 025F6EC8 void choiceClearWork(PaneWork_l*)
 ext 025F6B18 void choiceSelectWork(PaneWork_l*,u32,u32)
 ext 025F74D0 void choiceSetMode(void*,u32)
 ext 0200579C void choiceStartAnimation(hdAnimSet*,u32,u32,f32)
*/

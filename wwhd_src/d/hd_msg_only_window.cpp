/**
 * hd_msg_only_window.cpp
 * HD - the message-only window "MsgOnly_00" (hdMsgOnlyWindow_c on the merged message-window base):
 * exit-time destruction of its name tables, the deleting destructor, its states and the constructor
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_only_window.h"
#include "d/hd_msg_choice_window.h"
using namespace gabi;
enum MsgOnlyVirtualSlot { MSG_ONLY_FINISH_ADVANCE = 0x4BC/4, MSG_ONLY_IS_REOPEN_READY = 0x4CC/4, MSG_ONLY_RESET_TEXT = 0x424/4 };
struct hdMsgOnlyNames_c { hdUiSafeString mItems[4]; };
struct hdMsgOnlyCorePointer_c { gptr<HdMessageCore> mpCore; };
struct hdMsgOnlyMessageId_l { u8 mReserved[0x5C]; be<u32> mMessageId; };
static hdMsgOnlyNames_c sMsgOnlyWindowNames;
static hdMsgOnlyNames_c sMsgOnlyTextNames;
static hdMsgOnlyNames_c sMsgOnlyChoiceNames;
static hdMsgOnlyNames_c sMsgOnlySelectionNames;
static hdMsgOnlyCorePointer_c sMsgOnlyCore;
static HdMessageState sMsgOnlyWaitState;
static HdMessageState sMsgOnlyOtherWaitState;
static HdMessageState sMsgOnlyAdvanceState;
static HdMessageState sMsgOnlyInputAdvanceState;
static gfn sMsgOnlyWindowVtable;
static u8 sMsgOnlyStateVtable;
static u8 sMsgOnlySecondaryVtable;
static f32 sMsgOnlyZero;
extern void msgOnlyDestroyString(void*);
using MsgOnlyStringDestructor = void (*)(void*);
extern void msgOnlyDestroyArray(void*, u32, u32, MsgOnlyStringDestructor, u32, u32);
extern void msgOnlyFree(void*);
extern void msgOnlyDestroyBase(void*, u32);
extern void* msgOnlyAllocate(u32);
extern u32 msgOnlyGetMode(HdMessageCore*);
extern void msgOnlyPlaySound(u32);
extern void msgOnlyPrepareCloseSound();
extern void msgOnlyStopSound(u32);

/* 026AEB40 */
static void destroyMsgOnlyWindowNames() { msgOnlyDestroyArray(&sMsgOnlyWindowNames, 4, 8, msgOnlyDestroyString, 0, 0); }

/* 026AEB64 */
static void destroyMsgOnlyTextNames() { msgOnlyDestroyArray(&sMsgOnlyTextNames, 3, 8, msgOnlyDestroyString, 0, 0); }

/* 026AEB88 */
static void destroyMsgOnlyChoiceNames() { msgOnlyDestroyArray(&sMsgOnlyChoiceNames, 2, 8, msgOnlyDestroyString, 0, 0); }

/* 026AEBAC */
static void destroyMsgOnlySelectionNames() { msgOnlyDestroyArray(&sMsgOnlySelectionNames, 1, 8, msgOnlyDestroyString, 0, 0); }

/* 026AEBD0 */
void hdMsgOnlyWindow_c::destroy(u32 flags) {
    if (!this) return;
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    parts->mStateHolder.mHolderVtable = &sMsgOnlyStateVtable;
    parts->mSecondaryVtable = &sMsgOnlySecondaryVtable;
    msgOnlyDestroyBase(this, 0);
    if (flags & 1) msgOnlyFree(this);
}

/* 026AEFAC */
void hdMsgOnlyWindow_c::enterOpen() {
    mTypingFraction = sMsgOnlyZero;
    call_ptr<void>(mVTable[HD_MESSAGE_STEP_TEXT].get(), this);
    hdMessageSetMode(sMsgOnlyCore.mpCore, 6);
}

/* 026AF454 */
void hdMsgOnlyWindow_c::updateWaitMode() {
    u32 mode = msgOnlyGetMode(sMsgOnlyCore.mpCore);
    if (mode == 15) hdMessageChangeState(&mState, &sMsgOnlyWaitState);
    else if (mode == 16) hdMessageChangeState(&mState, &sMsgOnlyOtherWaitState);
    else hdMessageRefresh(mWaitPane, mMessage);
}

/* 026AF588 */
void hdMsgOnlyWindow_c::enterClose() {
    hdMessageSetMode(sMsgOnlyCore.mpCore, 18);
    windowStates()->startAnimation(5, 1);
    msgOnlyPrepareCloseSound();
    msgOnlyPlaySound(0x80D);
}

/* 026AF6A4 */
void hdMsgOnlyWindow_c::enterAdvance() {
    call_ptr<void>(mVTable[HD_MESSAGE_ADVANCE_TEXT].get(), this);
    msgOnlyPlaySound(0x80B);
}

/* 026AF798 */
void hdMsgOnlyWindow_c::enterInputAdvance() {
    call_ptr<void>(mVTable[HD_MESSAGE_ADVANCE_TEXT].get(), this);
    msgOnlyPlaySound(0x80B);
}

/* 026AF7CC */
void hdMsgOnlyWindow_c::updateAdvance() {
    if (call_ptr<u32>(mVTable[HD_MESSAGE_NEXT_STOP].get(), this))
        hdMessageChangeState(&mState, &sMsgOnlyAdvanceState);
}

/* 026AF81C */
void hdMsgOnlyWindow_c::finishAdvance() { call_ptr<void>(mVTable[MSG_ONLY_FINISH_ADVANCE].get(), this); }

/* 026AF9B0 */
void hdMsgOnlyWindow_c::updateStoppedAdvance() {
    if (!call_ptr<u32>(mVTable[HD_MESSAGE_NEXT_STOP].get(), this)) return;
    u32 messageId = reinterpret_cast<hdMsgOnlyMessageId_l*>(this)->mMessageId;
    mStopFlag = 0;
    if (messageId == 0x1072) msgOnlyStopSound(0x3C);
    hdMessageChangeState(&mState, &sMsgOnlyInputAdvanceState);
}
WWHD_OFFSET(hdMsgOnlyMessageId_l, mMessageId, 0x5C);
struct hdMsgOnlyPaneSlots_l { u8 mReserved[0x11C]; gptr<hdPartsPaneSRT_c> mWindows[4]; gptr<hdPartsPaneSRT_c> mTextPanes[3]; };
WWHD_OFFSET(hdMsgOnlyPaneSlots_l, mTextPanes, 0x12C);
struct hdMsgOnlyOpenSound_l { u8 mReserved[0xA]; be<u8> mSound; };
struct hdMsgOnlyOpenMessage_l { u8 mReserved[0x22C]; gptr<hdMsgOnlyOpenSound_l> mpSound; };
WWHD_OFFSET(hdMsgOnlyOpenSound_l, mSound, 0xA);
WWHD_OFFSET(hdMsgOnlyOpenMessage_l, mpSound, 0x22C);
struct hdMsgOnlyPlayTail_l { u8 mReserved[0x5BD2]; be<u8> mAdvanceCount, mOtherCount; };
WWHD_OFFSET(hdMsgOnlyPlayTail_l, mAdvanceCount, 0x5BD2);
struct hdMsgOnlyVector_c { be<u32> mX, mY, mZ; };
static u32 sMsgOnlyZeroVectorInitialized;
static hdMsgOnlyVector_c sMsgOnlyZeroVector;
static s8 sMsgOnlyRoom;
static HdMessageState sMsgOnlyReopenState;
extern s32 msgOnlyGetReverb(s32);
extern void msgOnlyStartVoice(u32, u32, s32);

/* 026AF82C */
void hdMsgOnlyWindow_c::updateReopen() {
    if (!call_ptr<u32>(mVTable[MSG_ONLY_IS_REOPEN_READY].get(), this)) return;
    auto* panes = reinterpret_cast<hdMsgOnlyPaneSlots_l*>(this);
    for (u32 i = 0; i<3; ++i) {
        hdPartsPaneSRT_c* pane = panes->mTextPanes[i];
        pane->mFlags = pane->mFlags&~1u;
    }
    call_ptr<void>(mVTable[MSG_ONLY_RESET_TEXT].get(), this);
    msgOnlyPlaySound(0x80C);
    auto* message = reinterpret_cast<hdMsgOnlyOpenMessage_l*>(mMessage.get());
    hdMsgOnlyOpenSound_l* sound = message->mpSound;
    if (sound && sound->mSound != 0) {
        s32 reverb = msgOnlyGetReverb(sMsgOnlyRoom);
        msgOnlyStartVoice(sound->mSound, 0, reverb);
    }
    reinterpret_cast<hdMsgOnlyPlayTail_l*>(hdMessageGetPlay())->mAdvanceCount = 0;
    reinterpret_cast<hdMsgOnlyPlayTail_l*>(hdMessageGetPlay())->mOtherCount = 0;
    hdMessageChangeState(&mState, &sMsgOnlyReopenState);
}

/* 026AFA1C */
hdMsgOnlyWindow_c* hdMsgOnlyWindow_c::construct(hdMsgOnlyWindow_c* object) {
    if (!object) {
        object = static_cast<hdMsgOnlyWindow_c*>(msgOnlyAllocate(sizeof(hdMsgOnlyWindow_c)));
        if (!object) return object;
    }
    hdMsgWindowStates_c::construct(object->windowStates());
    object->mVTable = &sMsgOnlyWindowVtable;
    auto* parts = reinterpret_cast<hdParts_c*>(object);
    parts->mSecondaryVtable = &sMsgOnlySecondaryVtable;
    parts->mStateHolder.mHolderVtable = &sMsgOnlyStateVtable;
    auto* panes = reinterpret_cast<hdMsgOnlyPaneSlots_l*>(object);
    // These null checks preserve the guest's wrapped embedded-array constructor paths.
    if (ea(&panes->mWindows) == 0) msgOnlyAllocate(0x10);
    if (ea(&panes->mTextPanes) == 0) msgOnlyAllocate(0xC);
    if (ea(&object->mpChoicePanes) == 0) msgOnlyAllocate(8);
    if (ea(&object->mpSelectionPane) == 0) msgOnlyAllocate(4);
    if (ea(&object->mpSelectionResource) == 0) msgOnlyAllocate(4);
    object->mRevision = 0;
    object->mpRequest = nullptr;
    for (u32 i = 0; i<4; ++i) panes->mWindows[i] = nullptr;
    for (u32 i = 0; i<3; ++i) panes->mTextPanes[i] = nullptr;
    if (!sMsgOnlyZeroVectorInitialized) {
        sMsgOnlyZeroVectorInitialized = 1;
        u32 zero = std::bit_cast<u32>(sMsgOnlyZero.get());
        sMsgOnlyZeroVector.mX = zero;
        sMsgOnlyZeroVector.mY = zero;
        sMsgOnlyZeroVector.mZ = zero;
    }
    for (u32 i = 0; i<2; ++i) object->mpChoicePanes[i] = nullptr;
    object->mpSelectionPane = nullptr;
    return object;
}
struct hdMsgOnlyName_c { hdUiSafeString mString; be<u32> mCapacity; be<u8> mBuffer[256]; };
struct hdMsgOnlyGroup_c { gptr<void> mpEntries; be<u32> mCount; };
struct hdMsgOnlyAttribute_c { u8 mReserved; be<u8> mKind; };
struct hdMsgOnlyLookup_c { gptr<void> mpLookup; };
struct hdMsgOnlyLabel_l { u8 mReserved[0x120]; hdUiSafeString mLabel; };
struct hdMsgOnlyTextKind_l { u8 mReserved[0xD0]; be<u8> mKind; };
struct hdMsgOnlySave_l { u8 mReserved[0x1C0]; be<u8> mKeepMessageId; };
struct hdMsgOnlySavePointer_c { gptr<hdMsgOnlySave_l> mpSave; };
struct hdMsgOnlyRemap_c { be<u32> mSource, mDestination; };
struct hdMsgOnlyRemaps_c { u8 mFormat[8]; hdMsgOnlyRemap_c mPairs[15]; };
struct hdMsgOnlyRequest_l { u8 mReserved[0x10]; be<u32> mRevision; };
struct hdMsgOnlyCoreMessage_l { u8 mReserved[0x938]; be<u32> mMessageId; };
struct hdMsgOnlyRegistration_c { be<u32> mWords[3]; };
WWHD_OFFSET(hdMsgOnlyName_c, mBuffer, 0xC);
WWHD_OFFSET(hdMsgOnlyGroup_c, mCount, 4);
WWHD_OFFSET(hdMsgOnlyAttribute_c, mKind, 1);
WWHD_OFFSET(hdMsgOnlyLabel_l, mLabel, 0x120);
WWHD_OFFSET(hdMsgOnlyTextKind_l, mKind, 0xD0);
WWHD_OFFSET(hdMsgOnlySave_l, mKeepMessageId, 0x1C0);
WWHD_OFFSET(hdMsgOnlyRemaps_c, mPairs, 8);
WWHD_OFFSET(hdMsgOnlyRequest_l, mRevision, 0x10);
WWHD_OFFSET(hdMsgOnlyCoreMessage_l, mMessageId, 0x938);
static hdMsgOnlyLookup_c sMsgOnlyLookup;
static hdUiStringTable sMsgOnlyNameVtable;
static hdUiStringTable sMsgOnlyPaneNameVtable;
static hdMsgOnlySavePointer_c sMsgOnlySave;
static hdMsgOnlyRemaps_c sMsgOnlyRemaps;
struct hdMsgOnlyRequestPointer_c { gptr<hdMsgOnlyRequest_l> mpRequest; };
static hdMsgOnlyRequestPointer_c sMsgOnlyRequest;
static HdMessageState sMsgOnlyNewMessageState;
static u32 sMsgOnlyWindowNamesInitialized;
static hdMsgOnlyRegistration_c sMsgOnlyWindowNameRegistration;
extern void msgOnlyFormatName(hdMsgOnlyName_c*, const char*, u32);
extern hdMsgOnlyGroup_c* msgOnlyFindGroup(void*, hdUiSafeString*);
extern u32 msgOnlyFindLabel(void*, be<u8>*);
extern hdMsgOnlyAttribute_c* msgOnlyGetAttribute(void*, u32);
extern u32 msgOnlyResolveRequest(void*, hdUiSafeString*, be<u32>*, u32);
extern s32 msgOnlySelectSpecial(HdMessageCore*, u32);
extern void msgOnlyRegisterNames(hdMsgOnlyRegistration_c*);

/* 026AEC40 */
u32 hdMsgOnlyWindow_c::isSpecialMessage() {
    hdMsgOnlyName_c name;
    name.mBuffer[255] = 0;
    name.mString.mText = name.mBuffer;
    name.mCapacity = 256;
    name.mString.mVtable = &sMsgOnlyNameVtable;
    u32 id = reinterpret_cast<hdMsgOnlyMessageId_l*>(this)->mMessageId;
    name.mBuffer[0] = 0;
    msgOnlyFormatName(&name, "%05d", u16(id));
    hdMsgOnlyGroup_c* group = msgOnlyFindGroup(sMsgOnlyLookup.mpLookup, &name.mString);
    if (!group) return 0;
    name.mString.assureTermination();
    u32 index = msgOnlyFindLabel(group->mpEntries, name.mString.mText);
    if (index >= 0xFFFFFFFEu) return 0;
    hdMsgOnlyAttribute_c* attribute = at<hdMsgOnlyAttribute_c>(0);
    if (index<group->mCount) attribute = msgOnlyGetAttribute(group->mpEntries, index);
    return attribute->mKind == 9;
}

/* 026AED20 */
void hdMsgOnlyWindow_c::remapMessage() {
    if (sMsgOnlySave.mpSave->mKeepMessageId) return;
    auto* id = &reinterpret_cast<hdMsgOnlyMessageId_l*>(this)->mMessageId;
    u32 current = *id;
    for (u32 i = 0; i<15; ++i) {
        if (current == sMsgOnlyRemaps.mPairs[i].mSource) {
            *id=sMsgOnlyRemaps.mPairs[i].mDestination;
            return;
        }
    }
}

/* 026AED68 */
void hdMsgOnlyWindow_c::prepareMessage() {
    auto* label = reinterpret_cast<hdMsgOnlyLabel_l*>(mMessage.get());
    hdMsgOnlyGroup_c* group = msgOnlyFindGroup(sMsgOnlyLookup.mpLookup, &label->mLabel);
    if (!group) return;
    label = reinterpret_cast<hdMsgOnlyLabel_l*>(mMessage.get());
    label->mLabel.assureTermination();
    u32 index = msgOnlyFindLabel(group->mpEntries, label->mLabel.mText);
    if (index >= group->mCount) return;
    if (!msgOnlyGetAttribute(group->mpEntries, index)) return;
    auto* id = reinterpret_cast<hdMsgOnlyMessageId_l*>(this);
    mAdvanceCount = 0;
    u32 messageId = id->mMessageId;
    auto* pane = reinterpret_cast<hdTextPane_c*>(mWaitPane.get());
    auto* kind = reinterpret_cast<hdMsgOnlyTextKind_l*>(pane);
    u32 desired = (messageId >= 0x1076 && messageId <= 0x1078)?0:1;
    u32 changed = (u32(kind->mKind)&3) != desired;
    pane->mTextFlags = pane->mTextFlags|((changed|(u32(pane->mTextFlags)>>5&1))<<5);
    if (changed) kind->mKind = (u32(kind->mKind)&~3u)|desired;
    pane = reinterpret_cast<hdTextPane_c*>(mWaitPane.get());
    pane->mShrink = 0;
    pane = reinterpret_cast<hdTextPane_c*>(mWaitPane.get());
    auto* width = reinterpret_cast<hdTalkChoiceWidth_c*>(mMessage.get());
    width->mWidthBits = reinterpret_cast<hdTalkPaneWidth_c*>(pane)->mWidthBits.get();
}

/* 026AEEB8 */
void hdMsgOnlyWindow_c::updateRequest() {
    hdMsgOnlyRequest_l* request = sMsgOnlyRequest.mpRequest;
    mpRequest = request;
    if (!request) return;
    u32 revision = request->mRevision;
    if (mRevision == revision) return;
    auto* label = reinterpret_cast<hdMsgOnlyLabel_l*>(mMessage.get());
    auto* id = &reinterpret_cast<hdMsgOnlyMessageId_l*>(this)->mMessageId;
    if (!msgOnlyResolveRequest(sMsgOnlyLookup.mpLookup, &label->mLabel, id, revision)) return;
    HdMessageCore* core = sMsgOnlyCore.mpCore;
    if (isSpecialMessage()) {
        if (msgOnlySelectSpecial(core, *id) != -1) mRevision = revision;
        return;
    }
    remapMessage();
    reinterpret_cast<hdMsgOnlyCoreMessage_l*>(core)->mMessageId = *id;
    mRevision = revision;
    prepareMessage();
    hdMessageChangeState(&mState, &sMsgOnlyNewMessageState);
}

/* 026AFB60 */
void hdMsgOnlyWindow_c::findWindowPanes() {
    if (!sMsgOnlyWindowNamesInitialized) {
        sMsgOnlyWindowNamesInitialized = 1;
        sMsgOnlyWindowNames.mItems[0].mVtable = &sMsgOnlyPaneNameVtable;
        sMsgOnlyWindowNames.mItems[0].mText = at<be<u8>>(guest_addr("N_Position_00"));
        sMsgOnlyWindowNames.mItems[1].mVtable = &sMsgOnlyPaneNameVtable;
        sMsgOnlyWindowNames.mItems[1].mText = at<be<u8>>(guest_addr("N_All_00"));
        sMsgOnlyWindowNames.mItems[2].mVtable = &sMsgOnlyPaneNameVtable;
        sMsgOnlyWindowNames.mItems[2].mText = at<be<u8>>(guest_addr("N_Window_00"));
        sMsgOnlyWindowNames.mItems[3].mVtable = &sMsgOnlyPaneNameVtable;
        sMsgOnlyWindowNames.mItems[3].mText = at<be<u8>>(guest_addr("N_SubWindow_00"));
        msgOnlyRegisterNames(&sMsgOnlyWindowNameRegistration);
    }
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(parts->mpLayout->lyt()->mpRootPane.get());
    auto* panes = reinterpret_cast<hdMsgOnlyPaneSlots_l*>(this);
    for (u32 i = 0; i<4; ++i) {
        hdUiSafeString* name = &sMsgOnlyWindowNames.mItems[i];
        hdPartsPaneSRTVtbl_c* table = root->srtVtbl();
        name->assureTermination();
        panes->mWindows[i] = call_ptr<hdPartsPaneSRT_c*>(table->mFindPane.get(), root, name->mText.get(), 1);
    }
    // The inherited window aliases point at the all-window and subwindow panes.
    struct Aliases_l { u8 mReserved[0x60]; gptr<hdPartsPaneSRT_c> mpAll, mpSub; };
    auto* aliases = reinterpret_cast<Aliases_l*>(this);
    aliases->mpAll = panes->mWindows[1];
    aliases->mpSub = panes->mWindows[3];
}

/* WWHD glue
 data sMsgOnlyLookup 101F4AE8
 data sMsgOnlyNameVtable 100FCF5C
 data sMsgOnlyPaneNameVtable 100FCF14
 data sMsgOnlySave 101F84DC
 data sMsgOnlyRemaps 100FD024
 data sMsgOnlyRequest 104756C8
 data sMsgOnlyNewMessageState 1049DE6C
 data sMsgOnlyWindowNamesInitialized 10497F70
 data sMsgOnlyWindowNameRegistration 101F6558
 str 100FD024 "%05d" in 026AEC40
 str 100FD0B8 "N_Position_00" in 026AFB60
 str 100FD0C8 "N_All_00" in 026AFB60
 str 100FD0AC "N_Window_00" in 026AFB60
 str 100FD0D4 "N_SubWindow_00" in 026AFB60
 ext 02759C28 void msgOnlyFormatName(hdMsgOnlyName_c*,const char*,u32)
 ext 025F4FC0 hdMsgOnlyGroup_c* msgOnlyFindGroup(void*,hdUiSafeString*)
 ext 0273A1CC u32 msgOnlyFindLabel(void*,be<u8>*)
 ext 0273A574 hdMsgOnlyAttribute_c* msgOnlyGetAttribute(void*,u32)
 ext 025F60EC u32 msgOnlyResolveRequest(void*,hdUiSafeString*,be<u32>*,u32)
 ext 025F8154 s32 msgOnlySelectSpecial(HdMessageCore*,u32)
 ext 028F026C void msgOnlyRegisterNames(hdMsgOnlyRegistration_c*)
 data sMsgOnlyZeroVectorInitialized 101FD2EC
 data sMsgOnlyZeroVector 101FDD44
 data sMsgOnlyRoom 1047E6C8
 data sMsgOnlyReopenState 10497FB0
 ext 02520540 s32 msgOnlyGetReverb(s32)
 ext 025E1C3C void msgOnlyStartVoice(u32,u32,s32)
 data sMsgOnlyWindowNames 10497EA8
 data sMsgOnlyTextNames 10497EC8
 data sMsgOnlyChoiceNames 10497EE0
 data sMsgOnlySelectionNames 10497E84
 data sMsgOnlyCore 101F4B5C
 data sMsgOnlyWindowVtable 100FD368
 data sMsgOnlyStateVtable 100FD858
 data sMsgOnlySecondaryVtable 100FD868
 data sMsgOnlyZero 100FD0A4
 data sMsgOnlyWaitState 10498070
 data sMsgOnlyOtherWaitState 1049E1FC
 data sMsgOnlyAdvanceState 10498100
 data sMsgOnlyInputAdvanceState 104980A0
 ext 026B15C8 void msgOnlyDestroyString(void*) callback
 ext 028F0164 void msgOnlyDestroyArray(void*,u32,u32,MsgOnlyStringDestructor,u32,u32)
 ext 0273AF40 void msgOnlyFree(void*)
 ext 026FC4D8 void msgOnlyDestroyBase(void*,u32)
 ext 0273AD10 void* msgOnlyAllocate(u32)
 ext 025F795C u32 msgOnlyGetMode(HdMessageCore*)
 ext 025E1988 void msgOnlyPlaySound(u32)
 ext 025E1C64 void msgOnlyPrepareCloseSound()
 ext 025E1904 void msgOnlyStopSound(u32)
 ext 020063C0 void hdMessageChangeState(HdMessageStateController*,HdMessageState*)
 ext 0270477C void hdMessageRefresh(HdMessagePane*,HdMessageData*)
 ext 025F74D0 void hdMessageSetMode(HdMessageCore*,u32)
 ext 026FF21C hdMsgWindowStates_c* hdMsgWindowStates_c::construct(hdMsgWindowStates_c*)
 ext 026FF5EC void hdMsgWindowStates_c::startAnimation(u32,u32)
 ext 026FF65C void hdMsgWindowStates_c::setAnimationFrame(u32,u32,f32)
 ext 026FF668 u32 hdMsgWindowStates_c::animationFinished(u32)
*/

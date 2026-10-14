/* HD event-message pane selection and position-part controls, under development. */
#include "d/hd_msg_window_states.h"
#include "d/hd_parts_key_icon.h"
#include "d/hd_keyboard_screen.h"
#include "d/hd_parts_text_pane.h"
#include "d/hd_screen_baton_sign_for_window.h"
#include "d/hd_msg_event_position.h"
#include "d/hd_msg_window_event_companion.h"
#include "d/hd_msg_window_event_handlers.h"
#include "d/d_save_readable.h"

u32 hdMsgEvent_triforceCount(dSv_player_collect_c*);
void hdMsgEvent_bindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
u32 hdMsgEvent_samePosition(cXyz*, cXyz*);
void hdMsgEvent_projectPosition(cXyz*, cXyz*);

/* 026B3D5C */
void hdMsgEventPosition_c::selectSongTextPane() {
    u32 message = mpMessage->mMessageId;
    if (message >= 0x5B3 && message <= 0x5B4) {
        mWindowPanes[1]->show();
        mWindowPanes[2]->show();
        mWindowPanes[6]->hide();
        mWindowPanes[7]->hide();
        void* text = mTextPanes[0].get();
        mTextPaneIndex = 0;
        mpActiveTextPane = text;
    } else {
        mWindowPanes[1]->hide();
        mWindowPanes[7]->show();
        void* text = mTextPanes[2].get();
        mTextPaneIndex = 2;
        mpActiveTextPane = text;
    }
}
/* 026B3E0C */
void hdMsgEventPosition_c::selectLargeTextPane() {
    mWindowPanes[1]->show();
    mWindowPanes[2]->hide();
    mWindowPanes[6]->show();
    mWindowPanes[7]->hide();
    void* text = mTextPanes[1].get();
    mTextPaneIndex = 1;
    mpActiveTextPane = text;
}
/* 026B3E68 */
void hdMsgEventPosition_c::selectMessageTextPane() {
    u32 message = mpMessage->mMessageId;
    if (message >= 0xD2 && message <= 0xD7) {
        mWindowPanes[1]->hide();
        mWindowPanes[7]->show();
        void* text = mTextPanes[2].get();
        mTextPaneIndex = 2;
        mpActiveTextPane = text;
    } else {
        // GHS inlines the large-pane selection here.
        mWindowPanes[1]->show();
        mWindowPanes[2]->hide();
        mWindowPanes[6]->show();
        mWindowPanes[7]->hide();
        void* text = mTextPanes[1].get();
        mTextPaneIndex = 1;
        mpActiveTextPane = text;
    }
}
/* 026B49F0 */
void hdMsgEventPosition_c::hideTextPanes() {
    for (u32 i = 0; i < 3; ++i) mTextPanes[i]->hide();
}
/* 026B4A40 */
void hdMsgEventPosition_c::hidePosition() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    mPositionPanes[15]->hide();
    reinterpret_cast<hdPartsKeyIcon_c*>(positionPart())->hide();
}
/* 026B4A6C */
void hdMsgEventPosition_c::showPosition() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    mPositionPanes[15]->show();
    reinterpret_cast<hdPartsKeyIcon_c*>(positionPart())->requestIn();
}
/* 026B4A9C */
void hdMsgEventPosition_c::showPositionOnce() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    mPositionPanes[15]->show();
    reinterpret_cast<hdPartsKeyIcon_c*>(positionPart())->requestDecide();
}
/* 026B4ACC */
void hdMsgEventPosition_c::showPositionLoop() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    mPositionPanes[15]->show();
    reinterpret_cast<hdPartsKeyIcon_c*>(positionPart())->requestOut();
}
/* 026B4AFC */
u32 hdMsgEventPosition_c::positionComplete() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    return reinterpret_cast<hdPartsKeyIcon_c*>(positionPart())->isWaiting();
}
/* 026B4B18 */
u32 hdMsgEventPosition_c::positionProgress() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    return reinterpret_cast<hdPartsKeyIcon_c*>(positionPart())->isHidden();
}
/* 026B4B34 */
u32 hdMsgEventPosition_c::positionActive() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    return reinterpret_cast<hdPartsKeyIcon_c*>(positionPart())->isNotDeciding();
}


/* Event lifecycle uses own game views and canonical framework callbacks. */
struct hdMsgEventCoreLifecycle_l {
    u8 _000[0x14]; hdMsgEventPositionMessage_l mMessage;
    u8 _134[0x921-0x134]; be<u8> mEventUpdate;
    u8 _922[0x94C-0x922]; be<u32> mEventBlocked;
    u8 _950[0x95C-0x950]; gptr<hdMsgEventPosition_c> mpEventWindow;
};
WWHD_OFFSET(hdMsgEventCoreLifecycle_l, mMessage, 0x14);
WWHD_OFFSET(hdMsgEventCoreLifecycle_l, mEventUpdate, 0x921);
WWHD_OFFSET(hdMsgEventCoreLifecycle_l, mEventBlocked, 0x94C);
WWHD_OFFSET(hdMsgEventCoreLifecycle_l, mpEventWindow, 0x95C);
WWHD_SIZE(hdMsgEventCoreLifecycle_l, 0x960);
struct hdMsgEventMessageCancel_l { u8 _000[0x90C]; be<u8> mCancelRequested; };
WWHD_OFFSET(hdMsgEventMessageCancel_l, mCancelRequested, 0x90C);
WWHD_SIZE(hdMsgEventMessageCancel_l, 0x90D);
struct hdMsgEventPositionOffsets_l {
    u8 _000[0x98]; be<u32> mOffsets[10], mStops[10]; be<u32> mStopIndex;
};
WWHD_OFFSET(hdMsgEventPositionOffsets_l, mOffsets, 0x98);
WWHD_OFFSET(hdMsgEventPositionOffsets_l, mStops, 0xC0);
WWHD_OFFSET(hdMsgEventPositionOffsets_l, mStopIndex, 0xE8);
WWHD_SIZE(hdMsgEventPositionOffsets_l, 0xEC);
static hdMsgEventWindowScalar_l sEventResetFrame;
/* 026B3BFC */
void hdMsgEventPosition_c::finalizeEvent() {
    auto* core = reinterpret_cast<hdMsgEventCoreLifecycle_l*>(sCore.mpCore.get());
    core->mpEventWindow = nullptr;
    core->mEventUpdate = 0;
    auto* message = reinterpret_cast<hdMsgEventMessageCancel_l*>(mpMessage.get());
    if (message) message->mCancelRequested = 0;
    reinterpret_cast<hdMsgWindowStates_c*>(this)->update();
}
/* 026B4944 */
void hdMsgEventPosition_c::resetTextPosition() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    auto* offsets = reinterpret_cast<hdMsgEventPositionOffsets_l*>(this);
    for (u32 i = 0; i < 10; ++i) offsets->mOffsets[i] = 0xFFFFFFFFu;
    for (u32 i = 0; i < 10; ++i) offsets->mStops[i] = 0xFFFFFFFFu;
    f32 frame = sEventResetFrame.mValue;
    offsets->mStopIndex = 0;
    reinterpret_cast<hdMsgWindowStates_c*>(this)->setAnimationFrame(0, 0, frame);
    vCalcLayout();
    vExecuteChildren();
    mpAllPane->hide();
}






/* Game child factory. */
struct hdMsgEventChildDesc_l {
    u8 _00[8]; hdUiSafeString mClassName;
    u8 _10[4]; hdUiSafeString mPaneName;
};
WWHD_OFFSET(hdMsgEventChildDesc_l, mClassName, 8);
WWHD_OFFSET(hdMsgEventChildDesc_l, mPaneName, 0x14);
WWHD_SIZE(hdMsgEventChildDesc_l, 0x1C);
struct hdMsgEventHeapOwner_l { u8 _00[0x10]; gptr<void> mpHeap; };
struct hdMsgEventHeapPointer_l { gptr<hdMsgEventHeapOwner_l> mpOwner; };
WWHD_OFFSET(hdMsgEventHeapOwner_l, mpHeap, 0x10);
WWHD_SIZE(hdMsgEventHeapOwner_l, 0x14);
WWHD_SIZE(hdMsgEventHeapPointer_l, 4);
static hdMsgEventHeapPointer_l sEventHeapOwner;
struct hdMsgEventPositionNames_l { hdUiSafeString mNames[16]; };
WWHD_OFFSET(hdMsgEventPositionNames_l, mNames, 0);
WWHD_SIZE(hdMsgEventPositionNames_l, 0x80);
static hdMsgEventPositionNames_l sEventPositionNames;
static hdUiSafeString sEventPositionClass;
static hdUiSafeString sEventFrameClass;
static hdUiSafeString sEventPaneClass;
/* inlined SafeString equality in the game factory; termination callbacks remain observable. */
static bool hdMsgEvent_sameChildName(hdUiSafeString* name, hdUiSafeString* other) {
    name->assureTermination();
    u32 saved = gabi::ea(name->mText.get());
    other->assureTermination();
    if (saved == gabi::ea(other->mText.get())) return true;
    u32 left = gabi::ea(name->mText.get());
    u32 right = gabi::ea(other->mText.get());
    for (u32 n = 0; n < 0x40001; ++n) {
        u8 value = *gabi::at<be<u8>>(left + n);
        if (value != *gabi::at<be<u8>>(right + n)) return false;
        if (!value) return true;
    }
    return false;
}
/* 026B41BC */
u32 hdMsgEventPosition_c::createEventChildren(hdPartsDesc_c* desc) {
    // The caller owns the full descriptor; the event-specific names occupy its prefix.
    auto* names = reinterpret_cast<hdMsgEventChildDesc_l*>(desc);
    gabi::AutoFrame automatic{gabi::Activation::current};
    void* heap = sEventHeapOwner.mpOwner->mpHeap;
    for (u32 index = 0; index < 16; ++index) {
        names->mPaneName.assureTermination();
        if (!hdMsgEvent_sameChildName(&names->mPaneName, &sEventPositionNames.mNames[index])) continue;
        names->mClassName.assureTermination();
        hdUiSafeString* expected = index == 15 ? &sEventPositionClass
                                  : index >= 13 ? &sEventFrameClass : &sEventPaneClass;
        if (!hdMsgEvent_sameChildName(&names->mClassName, expected))
            return hdParts_c::createChild(index, desc);
        auto* destination = mChildren.safeSlot(index);
        void* child = hdMsgEvent_allocateChild(index == 15 ? 0x58 : index >= 13 ? 0x50 : 0x54, heap, 4);
        if (child) {
            if (index == 15) child = constructKeyboardKeyIcon(reinterpret_cast<hdKeyboardKeyIcon_c*>(child));
            else if (index >= 13) child = hdMsgEventFrame_c::construct(reinterpret_cast<hdMsgEventFrame_c*>(child));
            else child = hdBatonSignForWindow_c::construct(reinterpret_cast<hdBatonSignForWindow_c*>(child));
        }
        *destination = reinterpret_cast<hdParts_c*>(child);
        if (!*mChildren.safeSlot(index)) return 0;
        child = *mChildren.safeSlot(index);
        u32 loaded;
        if (index == 15) loaded = reinterpret_cast<hdKeyboardKeyIcon_c*>(child)->create(0, 0, gabi::ea(desc));
        else if (index >= 13) loaded = reinterpret_cast<hdMsgEventFrame_c*>(child)->loadFrame(0, 0, desc);
        else loaded = reinterpret_cast<hdBatonSignForWindow_c*>(child)->initialize(0, 0, desc);
        if (!loaded) return 0;
    }
    return 1;
}


struct hdMsgEventImageString_l { be<u8> mFirst; };
WWHD_OFFSET(hdMsgEventImageString_l, mFirst, 0);
WWHD_SIZE(hdMsgEventImageString_l, 1);
struct hdMsgEventWindowByte_l { be<u8> mValue; };
WWHD_OFFSET(hdMsgEventWindowByte_l, mValue, 0);
WWHD_SIZE(hdMsgEventWindowByte_l, 1);
/* Event game statics use runtime image views and canonical state objects. */
struct hdMsgEventInitializerWords_l { be<u32> mWords[4]; };
struct hdMsgEventInitializerFloats_l { be<f32> mValues[4]; };
struct hdMsgEventInitializerNames_l { hdUiSafeString mNames[16]; };
WWHD_OFFSET(hdMsgEventInitializerWords_l, mWords, 0);
WWHD_SIZE(hdMsgEventInitializerWords_l, 0x10);
WWHD_OFFSET(hdMsgEventInitializerFloats_l, mValues, 0);
WWHD_SIZE(hdMsgEventInitializerFloats_l, 0x10);
WWHD_OFFSET(hdMsgEventInitializerNames_l, mNames, 0);
WWHD_SIZE(hdMsgEventInitializerNames_l, 0x80);
static hdMsgEventInitializerWords_l sEventInitializerHeader;
static hdMsgEventInitializerFloats_l sEventDistances;
static hdMsgEventFrameAngleLimits_l sEventAngleLimits;
static hdMsgEventFrameAngleLimits_l sEventAngleValues;
static hdMsgEventFrameAngleLimits_l sEventDistanceLimits;
static hdMsgEventFrameRegistration_l sEventHeaderRegistration;
static hdMsgEventFrameRegistration_l sEventAngleRegistration;
static hdMsgEventFrameRegistration_l sEventPhaseRegistration;
static hdMsgEventWindowByte_l sEventAngle;
static hdMsgEventWindowByte_l sEventPhase;
static hdUiStringTable sEventStringVtable;
static hdPartsStateIdVtbl_c sEventStateVtable;
static hdStateId sEventNullState;
static u32 sEventStateGuard;
static hdPartsStateCounter_c sEventStateCounter;
static hdPartsStateId_c sEventOpen;
static hdMsgEventImageString_l sEventOpenName;
static hdPartsStateId_c sEventPrepare;
static hdMsgEventImageString_l sEventPrepareName;
static hdPartsStateId_c sEventOpening;
static hdMsgEventImageString_l sEventOpeningName;
static hdMsgEventImageString_l sEventOpeningSpecialName;
static hdMsgEventImageString_l sEventBeatDoneName;
static hdMsgEventImageString_l sEventShortWaitName;
static hdMsgEventImageString_l sEventSequenceDoneName;
static hdPartsStateId_c sEventOpened;
static hdMsgEventImageString_l sEventOpenedName;
static hdPartsStateId_c sEventTyping;
static hdMsgEventImageString_l sEventTypingName;
static hdMsgEventImageString_l sEventAdvanceName;
static hdMsgEventImageString_l sEventClosedName;
static hdPartsStateId_c sEventCloseDone;
static hdMsgEventImageString_l sEventCloseDoneName;
static hdPartsStateId_c sEventCloseWait;
static hdMsgEventImageString_l sEventCloseWaitName;
static hdPartsStateId_c sEventAbort;
static hdMsgEventImageString_l sEventAbortName;
struct hdMsgEventResourceVariantsNames_l { hdUiSafeString mNames[3]; };
WWHD_OFFSET(hdMsgEventResourceVariantsNames_l, mNames, 0);
WWHD_SIZE(hdMsgEventResourceVariantsNames_l, 0x18);
static hdMsgEventResourceVariantsNames_l sEventResourceVariantsNames;
struct hdMsgEventChildSettingsNames_l { hdUiSafeString mNames[5]; };
WWHD_OFFSET(hdMsgEventChildSettingsNames_l, mNames, 0);
WWHD_SIZE(hdMsgEventChildSettingsNames_l, 0x28);
static hdMsgEventChildSettingsNames_l sEventChildSettingsNames;
struct hdMsgEventLayoutResourcesNames_l { hdUiSafeString mNames[4]; };
WWHD_OFFSET(hdMsgEventLayoutResourcesNames_l, mNames, 0);
WWHD_SIZE(hdMsgEventLayoutResourcesNames_l, 0x20);
static hdMsgEventLayoutResourcesNames_l sEventLayoutResourcesNames;
struct hdMsgEventPositionAnimationsNames_l { hdUiSafeString mNames[16]; };
WWHD_OFFSET(hdMsgEventPositionAnimationsNames_l, mNames, 0);
WWHD_SIZE(hdMsgEventPositionAnimationsNames_l, 0x80);
static hdMsgEventPositionAnimationsNames_l sEventPositionAnimationsNames;
struct hdMsgEventLayoutNameNames_l { hdUiSafeString mNames[1]; };
WWHD_OFFSET(hdMsgEventLayoutNameNames_l, mNames, 0);
WWHD_SIZE(hdMsgEventLayoutNameNames_l, 0x8);
static hdMsgEventLayoutNameNames_l sEventLayoutNameNames;
static hdMsgEventImageString_l sEventChildSettingsText0;
static hdMsgEventImageString_l sEventChildSettingsText2;
static hdMsgEventImageString_l sEventChildSettingsText1;
static hdMsgEventImageString_l sEventChildSettingsText4;
static hdMsgEventImageString_l sEventPositionNamesText0;
static hdMsgEventImageString_l sEventPositionNamesText1;
static hdMsgEventImageString_l sEventPositionNamesText2;
static hdMsgEventImageString_l sEventPositionNamesText3;
static hdMsgEventImageString_l sEventPositionNamesText4;
static hdMsgEventImageString_l sEventPositionNamesText5;
static hdMsgEventImageString_l sEventPositionNamesText6;
static hdMsgEventImageString_l sEventPositionNamesText7;
static hdMsgEventImageString_l sEventPositionNamesText8;
static hdMsgEventImageString_l sEventPositionNamesText9;
static hdMsgEventImageString_l sEventPositionNamesText10;
static hdMsgEventImageString_l sEventPositionNamesText11;
static hdMsgEventImageString_l sEventPositionNamesText12;
static hdMsgEventImageString_l sEventResourceVariantsText2;
static hdMsgEventImageString_l sEventLayoutResourcesText1;
static hdMsgEventImageString_l sEventLayoutResourcesText0;
static hdMsgEventImageString_l sEventResourceVariantsText0;
static hdMsgEventImageString_l sEventResourceVariantsText1;
static hdMsgEventImageString_l sEventChildSettingsText3;
static hdMsgEventImageString_l sEventPositionNamesText13;
static hdMsgEventImageString_l sEventPositionNamesText14;
static hdMsgEventImageString_l sEventPositionNamesText15;
static hdMsgEventImageString_l sEventLayoutResourcesText2;
static hdMsgEventImageString_l sEventLayoutResourcesText3;
/* inlined initializer assignment: all destinations disjoint, no intervening callback. */
static void hdMsgEvent_name(hdUiSafeString& name, hdMsgEventImageString_l& text) {
    name.mText = &text.mFirst;
    name.mVtable = &sEventStringVtable;
}
/* The shared header exposes opaque game states; borrow the canonical complete state view at that guest object. */
static hdPartsStateId_c& hdMsgEvent_stateView(hdMsgEvent_state_c& state) {
    return reinterpret_cast<hdPartsStateId_c&>(state);
}
/* 026B4B50 */
static void hdMsgEvent_initializeStatics() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    for (s32 i = 3; i >= 0; --i) sEventInitializerHeader.mWords[i] = 0;
    hdMsgEventFrame_register(&sEventHeaderRegistration);
    f32 minimum = sEventAngleLimits.mMinimum.mValue;
    f32 maximum = sEventAngleLimits.mMaximum.mValue;
    sEventAngleValues.mMinimum.mValue = minimum;
    sEventAngleValues.mMaximum.mValue = maximum;
    hdMsgEventFrame_constructAngle(&sEventAngle.mValue);
    hdMsgEventFrame_register(&sEventAngleRegistration);
    hdMsgEventFrame_constructPhase(&sEventPhase.mValue);
    hdMsgEventFrame_register(&sEventPhaseRegistration);
    f32 largeDistance = sEventDistanceLimits.mMinimum.mValue;
    f32 smallDistance = sEventDistanceLimits.mMaximum.mValue;
    hdMsgEvent_name(sEventResourceVariantsNames.mNames[0], sEventResourceVariantsText0);
    hdMsgEvent_name(sEventResourceVariantsNames.mNames[1], sEventResourceVariantsText1);
    hdMsgEvent_name(sEventResourceVariantsNames.mNames[2], sEventResourceVariantsText2);
    hdMsgEvent_name(sEventChildSettingsNames.mNames[0], sEventChildSettingsText0);
    hdMsgEvent_name(sEventChildSettingsNames.mNames[1], sEventChildSettingsText1);
    hdMsgEvent_name(sEventChildSettingsNames.mNames[2], sEventChildSettingsText2);
    hdMsgEvent_name(sEventChildSettingsNames.mNames[3], sEventChildSettingsText3);
    hdMsgEvent_name(sEventChildSettingsNames.mNames[4], sEventChildSettingsText4);
    hdMsgEvent_name(sEventLayoutResourcesNames.mNames[0], sEventLayoutResourcesText0);
    hdMsgEvent_name(sEventLayoutResourcesNames.mNames[1], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventLayoutResourcesNames.mNames[2], sEventLayoutResourcesText2);
    hdMsgEvent_name(sEventLayoutResourcesNames.mNames[3], sEventLayoutResourcesText3);
    hdMsgEvent_name(sEventPositionNames.mNames[0], sEventPositionNamesText0);
    hdMsgEvent_name(sEventPositionNames.mNames[1], sEventPositionNamesText1);
    hdMsgEvent_name(sEventPositionNames.mNames[2], sEventPositionNamesText2);
    hdMsgEvent_name(sEventPositionNames.mNames[3], sEventPositionNamesText3);
    hdMsgEvent_name(sEventPositionNames.mNames[4], sEventPositionNamesText4);
    hdMsgEvent_name(sEventPositionNames.mNames[5], sEventPositionNamesText5);
    hdMsgEvent_name(sEventPositionNames.mNames[6], sEventPositionNamesText6);
    hdMsgEvent_name(sEventPositionNames.mNames[7], sEventPositionNamesText7);
    hdMsgEvent_name(sEventPositionNames.mNames[8], sEventPositionNamesText8);
    hdMsgEvent_name(sEventPositionNames.mNames[9], sEventPositionNamesText9);
    hdMsgEvent_name(sEventPositionNames.mNames[10], sEventPositionNamesText10);
    hdMsgEvent_name(sEventPositionNames.mNames[11], sEventPositionNamesText11);
    hdMsgEvent_name(sEventPositionNames.mNames[12], sEventPositionNamesText12);
    hdMsgEvent_name(sEventPositionNames.mNames[13], sEventPositionNamesText13);
    hdMsgEvent_name(sEventPositionNames.mNames[14], sEventPositionNamesText14);
    hdMsgEvent_name(sEventPositionNames.mNames[15], sEventPositionNamesText15);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[0], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[1], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[2], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[3], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[4], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[5], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[6], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[7], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[8], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[9], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[10], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[11], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[12], sEventLayoutResourcesText1);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[13], sEventLayoutResourcesText2);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[14], sEventLayoutResourcesText2);
    hdMsgEvent_name(sEventPositionAnimationsNames.mNames[15], sEventLayoutResourcesText3);
    hdMsgEvent_name(sEventLayoutNameNames.mNames[0], sEventLayoutResourcesText0);
    sEventDistances.mValues[3] = smallDistance;
    sEventDistances.mValues[1] = largeDistance;
    sEventDistances.mValues[0] = largeDistance;
    sEventDistances.mValues[2] = smallDistance;
    u32 initialized = sEventStateGuard;
    u32 number;
    if (initialized) number = sEventStateCounter.mValue;
    else { sEventStateGuard = 1; number = 0; }
    sEventOpen.setup(s32(++number), reinterpret_cast<const char*>(&sEventOpenName.mFirst), &sEventStateVtable, 0x9B, &sEventNullState);
    sEventPrepare.setup(s32(++number), reinterpret_cast<const char*>(&sEventPrepareName.mFirst), &sEventStateVtable, 0x9E, &sEventNullState);
    sEventOpening.setup(s32(++number), reinterpret_cast<const char*>(&sEventOpeningName.mFirst), &sEventStateVtable, 0xA1, &sEventNullState);
    hdMsgEvent_stateView(sEventOpeningSpecial).setup(s32(++number), reinterpret_cast<const char*>(&sEventOpeningSpecialName.mFirst), &sEventStateVtable, 0xA4, &sEventNullState);
    hdMsgEvent_stateView(sBeatDisplayDone).setup(s32(++number), reinterpret_cast<const char*>(&sEventBeatDoneName.mFirst), &sEventStateVtable, 0xA7, &sEventNullState);
    hdMsgEvent_stateView(sShortWaitDone).setup(s32(++number), reinterpret_cast<const char*>(&sEventShortWaitName.mFirst), &sEventStateVtable, 0x4C, &sEventNullState);
    hdMsgEvent_stateView(sSequenceDone).setup(s32(++number), reinterpret_cast<const char*>(&sEventSequenceDoneName.mFirst), &sEventStateVtable, 0xAA, &sEventNullState);
    sEventOpened.setup(s32(++number), reinterpret_cast<const char*>(&sEventOpenedName.mFirst), &sEventStateVtable, 0x22, &sEventNullState);
    sEventTyping.setup(s32(++number), reinterpret_cast<const char*>(&sEventTypingName.mFirst), &sEventStateVtable, 0x6D, &sEventNullState);
    hdMsgEvent_stateView(sAdvance).setup(s32(++number), reinterpret_cast<const char*>(&sEventAdvanceName.mFirst), &sEventStateVtable, 0x67, &sEventNullState);
    hdMsgEvent_stateView(sClosed).setup(s32(++number), reinterpret_cast<const char*>(&sEventClosedName.mFirst), &sEventStateVtable, 0x6A, &sEventNullState);
    sEventCloseDone.setup(s32(++number), reinterpret_cast<const char*>(&sEventCloseDoneName.mFirst), &sEventStateVtable, 0xAD, &sEventNullState);
    sEventCloseWait.setup(s32(++number), reinterpret_cast<const char*>(&sEventCloseWaitName.mFirst), &sEventStateVtable, 0xB0, &sEventNullState);
    sEventAbort.setup(s32(++number), reinterpret_cast<const char*>(&sEventAbortName.mFirst), &sEventStateVtable, 0xB3, &sEventNullState);
    sEventStateCounter.mValue = number;
}




/* Pane setup uses names from runtime image views. */
struct hdMsgEventWindowNames_l { hdUiSafeString mNames[8]; };
WWHD_OFFSET(hdMsgEventWindowNames_l, mNames, 0);
WWHD_SIZE(hdMsgEventWindowNames_l, 0x40);
static hdMsgEventWindowNames_l sEventWindowNames;
static u32 sEventWindowNamesGuard;
static hdMsgEventFrameRegistration_l sEventWindowNamesRegistration;
static hdMsgEventImageString_l sEventWindowPaneName0;
static hdMsgEventImageString_l sEventWindowPaneName1;
static hdMsgEventImageString_l sEventWindowPaneName2;
static hdMsgEventImageString_l sEventWindowPaneName3;
static hdMsgEventImageString_l sEventWindowPaneName4;
static hdMsgEventImageString_l sEventWindowPaneName5;
static hdMsgEventImageString_l sEventWindowPaneName6;
static hdMsgEventImageString_l sEventWindowPaneName7;
/* 026B3584 */
void hdMsgEventPosition_c::bindWindowPanes() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (!sEventWindowNamesGuard) {
        sEventWindowNamesGuard = 1;
        sEventWindowNames.mNames[0].mVtable = &sEventStringVtable;
        sEventWindowNames.mNames[0].mText = &sEventWindowPaneName0.mFirst;
        sEventWindowNames.mNames[1].mVtable = &sEventStringVtable;
        sEventWindowNames.mNames[1].mText = &sEventWindowPaneName1.mFirst;
        sEventWindowNames.mNames[2].mVtable = &sEventStringVtable;
        sEventWindowNames.mNames[2].mText = &sEventWindowPaneName2.mFirst;
        sEventWindowNames.mNames[3].mVtable = &sEventStringVtable;
        sEventWindowNames.mNames[3].mText = &sEventWindowPaneName3.mFirst;
        sEventWindowNames.mNames[4].mVtable = &sEventStringVtable;
        sEventWindowNames.mNames[4].mText = &sEventWindowPaneName4.mFirst;
        sEventWindowNames.mNames[5].mVtable = &sEventStringVtable;
        sEventWindowNames.mNames[5].mText = &sEventWindowPaneName5.mFirst;
        sEventWindowNames.mNames[6].mVtable = &sEventStringVtable;
        sEventWindowNames.mNames[6].mText = &sEventWindowPaneName6.mFirst;
        sEventWindowNames.mNames[7].mVtable = &sEventStringVtable;
        sEventWindowNames.mNames[7].mText = &sEventWindowPaneName7.mFirst;
        hdMsgEventFrame_register(&sEventWindowNamesRegistration);
    }
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 i = 0; i < 8; ++i) {
        auto* name = &sEventWindowNames.mNames[i];
        auto* table = root->srtVtbl();
        name->assureTermination();
        u32 find = table->mFindPane.get();
        const char* text = reinterpret_cast<const char*>(name->mText.get());
        mWindowPanes[i] = reinterpret_cast<hdMsgEventVisiblePane_l*>(gabi::call_ptr<hdPartsPaneSRT_c*>(find, root, text, 1));
    }
    mpAllPane = mWindowPanes[0];
}
/* 026B39C8 */
void hdMsgEventPosition_c::bindPositionPanes() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(mpLayout->lyt()->mpRootPane.get());
    for (u32 i = 0; i < 16; ++i) {
        auto* name = &sEventPositionNames.mNames[i];
        auto* table = root->srtVtbl();
        name->assureTermination();
        u32 find = table->mFindPane.get();
        const char* text = reinterpret_cast<const char*>(name->mText.get());
        mPositionPanes[i] = reinterpret_cast<hdMsgEventVisiblePane_l*>(gabi::call_ptr<hdPartsPaneSRT_c*>(find, root, text, 1));
    }
    mpPositionPane = mPositionPanes[15];
}





/* Event layout loading and update. */
static hdPartsStateId_c sEventInitialState;
/* 026B3AB0 */
u32 hdMsgEventPosition_c::loadEvent(u32 kind, u32 flags, void* description) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    auto* core = reinterpret_cast<hdMsgEventCoreLifecycle_l*>(sCore.mpCore.get());
    core->mpEventWindow = this;
    auto* message = &core->mMessage;
    core->mEventUpdate = 0;
    attachArchives(&sEventLayoutResourcesNames.mNames[0], 4);
    u32 resource = gabi::ea(mpResAccessor.get());
    if (!vLoad(&sEventLayoutNameNames.mNames[0], resource, kind, flags, description)) return 0;
    if (!gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this)) return 0;
    bindWindowPanes();
    bindTextPanes();
    bindPositionPanes();
    reinterpret_cast<hdTextPane_c*>(mTextPanes[0].get())->setCharacterCapacity(0x40);
    reinterpret_cast<hdTextPane_c*>(mTextPanes[1].get())->setCharacterCapacity(0x14F);
    reinterpret_cast<hdTextPane_c*>(mTextPanes[2].get())->setCharacterCapacity(0x40);
    auto* callbacks = reinterpret_cast<hdMsgEventCallbacks_l*>(mVtable.get());
    mpMessage = message;
    gabi::call_ptr(callbacks->mOnCreate.get(), this);
    auto* state = mStateHolder.mMachine.mpOwner->createState(reinterpret_cast<hdStateId*>(&sEventInitialState));
    mStateHolder.mMachine.mpCurrentState = state;
    state->enter();
    return 1;
}
/* 026B3C24 */
void hdMsgEventPosition_c::executeEvent() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    auto* core = reinterpret_cast<hdMsgEventCoreLifecycle_l*>(sCore.mpCore.get());
    if (core->mEventBlocked) return;
    if (reinterpret_cast<hdMsgEvent_inputView_c*>(dComIfGp_get())->mInputBlocked) {
        u32 bgm = hdMsgEvent_currentBgm();
        if (bgm != 0x80000002 && bgm != 0x80000024 && bgm != 0x80000025 &&
            bgm != 0x80000027 && bgm != 0x8000004E && bgm != 0x8000004F)
            reinterpret_cast<hdMsgEvent_inputView_c*>(dComIfGp_get())->mInputBlocked = 0;
    }
    auto* current = hdParts_getCurrentStateId(&mStateHolder.mMachine);
    u32 getCurrentKey = current->mVtable->mGetKey.get();
    auto* initialTable = sEventInitialState.mVtable.get();
    u32 currentKey = gabi::call_ptr<u32>(getCurrentKey, current);
    u32 initialKey = gabi::call_ptr<u32>(initialTable->mGetKey.get(), &sEventInitialState);
    if (currentKey != initialKey) {
        reinterpret_cast<hdMsgWindowStates_c*>(this)->readController();
        hdParts_executeStateMachine(&mStateHolder.mMachine);
        vExecuteChildren();
        vCalcLayout();
    }
    core->mEventUpdate = 0;
}


/* Event message selection. Reuses canonical save-player and state views. */
struct hdMsgEventSavePointer_l { gptr<d_save_cpp::SaveGlobal> mpSave; };
WWHD_OFFSET(hdMsgEventSavePointer_l, mpSave, 0);
WWHD_SIZE(hdMsgEventSavePointer_l, 4);
struct hdMsgEventSongMessage_l { u8 _000[0x69C]; be<u8> mSongMessage; };
WWHD_OFFSET(hdMsgEventSongMessage_l, mSongMessage, 0x69C);
WWHD_SIZE(hdMsgEventSongMessage_l, 0x69D);
static hdMsgEventSavePointer_l sEventSave;
static hdPartsStateId_c sEventPositionWaitState;
static hdPartsStateId_c sEventPositionDisplayState;
/* inlined state identity test: target vtable is captured before the current-key callback. */
static bool hdMsgEvent_isState(hdStateMachine* machine, hdPartsStateId_c* target) {
    auto* current = hdParts_getCurrentStateId(machine);
    u32 currentFunction = current->mVtable->mGetKey.get();
    auto* targetTable = target->mVtable.get();
    u32 currentKey = gabi::call_ptr<u32>(currentFunction, current);
    u32 targetKey = gabi::call_ptr<u32>(targetTable->mGetKey.get(), target);
    return currentKey == targetKey;
}
/* 026B3F18 */
u32 hdMsgEventPosition_c::setEventMessage(u32 type, u32 unused, u32 id) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    if (hdMsgEvent_isState(&mStateHolder.mMachine, &sEventOpen) ||
        hdMsgEvent_isState(&mStateHolder.mMachine, &sEventPrepare)) return 0;
    auto* window = reinterpret_cast<hdMsgEventHandlers_c*>(this);
    auto* message = reinterpret_cast<hdMsgEventMessageFlags_l*>(mpMessage.get());
    reinterpret_cast<hdMsgEventCancelFlag_l*>(window)->mCancelRequested = u8(message->mCancelRequested);
    if (hdMsgEvent_isState(&mStateHolder.mMachine, &sEventInitialState)) {
        if (type == 1 && id - 0xC6 < 8)
            id = 0xC6 + hdMsgEvent_triforceCount(&sEventSave.mpSave->mInfo.mSavedata.mPlayer.mCollect);
        auto* allPane = mWindowPanes[0].get();
        mWindowType = type;
        mMessageId = id;
        allPane->show();
        auto* callbacks = reinterpret_cast<hdMsgEventCallbacks_l*>(mVtable.get());
        gabi::call_ptr(callbacks->mOnMessage.get(), this);
        hdMsgEvent_setMessageId(reinterpret_cast<hdMsgEvent_message_c*>(mpMessage.get()), id, 0);
        if (type == 0) {
            selectSongTextPane();
            reinterpret_cast<hdMsgEventSongMessage_l*>(mpMessage.get())->mSongMessage = 1;
            hdParts_changeState(reinterpret_cast<hdStateMachine*>(&window->mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sEventOpen));
        } else {
            if (type == 3) selectLargeTextPane();
            else selectMessageTextPane();
            hdParts_changeState(reinterpret_cast<hdStateMachine*>(&window->mStateMachine), reinterpret_cast<hdPartsStateId_c*>(&sEventPrepare));
        }
        return 1;
    }
    if (hdMsgEvent_isState(&mStateHolder.mMachine, &sEventPositionWaitState) ||
        hdMsgEvent_isState(&mStateHolder.mMachine, &sEventPositionDisplayState)) {
        if (id != u32(mMessageId)) {
            mMessageId = id;
            mMessageChanged = 1;
            mWindowType = type;
            return 1;
        }
    }
    return 0;
}

/* Event layout creation uses canonical layout/animation views and runtime resource tables. */
struct hdMsgEventAnimationResources_l { be<u32> mIndices[5]; };
WWHD_OFFSET(hdMsgEventAnimationResources_l, mIndices, 0);
WWHD_SIZE(hdMsgEventAnimationResources_l, 0x14);
static hdMsgEventAnimationResources_l sEventAnimationResources;
/* 026B4670 */
u32 hdMsgEventPosition_c::createEventLayout(void* name, u32 kind, u32 option, u32 flags, void* description) {
    gabi::AutoFrame automatic{gabi::Activation::current};
    void* heap = sEventHeapOwner.mpOwner->mpHeap;
    auto* layout = reinterpret_cast<hdPartsLayout_c*>(hdMsgEvent_allocateChild(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, kind, option, flags, 5, 3, description)) return 0;
    for (u32 i = 0; i < 5; ++i) {
        auto* current = mpLayout.get();
        u32 resource = sEventAnimationResources.mIndices[i];
        auto* animation = current->animSet();
        auto* settings = &sEventChildSettingsNames.mNames[i];
        // The original resource-table lookup is unbounded and wraps in guest space.
        auto* variant = gabi::at<hdUiSafeString>(gabi::ea(&sEventResourceVariantsNames) + resource * sizeof(hdUiSafeString));
        hdMsgEvent_bindAnimation(animation, i, settings, variant);
    }
    vCalcLayout();
    return 1;
}

/* Position animation selects a runtime frame from the projected anchor. */
struct hdMsgEventAnchor_l {
    u8 _00[3]; be<u8> mMode; u8 _04[7]; be<u8> mUseAnchor;
};
WWHD_OFFSET(hdMsgEventAnchor_l, mMode, 3);
WWHD_OFFSET(hdMsgEventAnchor_l, mUseAnchor, 0xB);
WWHD_SIZE(hdMsgEventAnchor_l, 0xC);
struct hdMsgEventAnchorMessage_l {
    u8 _000[0x22C]; gptr<hdMsgEventAnchor_l> mpAnchor;
};
WWHD_OFFSET(hdMsgEventAnchorMessage_l, mpAnchor, 0x22C);
WWHD_SIZE(hdMsgEventAnchorMessage_l, 0x230);
struct hdMsgEventProjectionStack_l {
    u8 _00[8]; cXyz mAnchor, mPlayer, mWindow;
    be<u32> mMatrixPrefix[7]; be<f32> mMatrixY; be<u32> mMatrixSuffix[4];
    u8 _5C[0x78-0x5C];
};
WWHD_OFFSET(hdMsgEventProjectionStack_l, mAnchor, 8);
WWHD_OFFSET(hdMsgEventProjectionStack_l, mPlayer, 0x14);
WWHD_OFFSET(hdMsgEventProjectionStack_l, mWindow, 0x20);
WWHD_OFFSET(hdMsgEventProjectionStack_l, mMatrixPrefix, 0x2C);
WWHD_OFFSET(hdMsgEventProjectionStack_l, mMatrixY, 0x48);
WWHD_SIZE(hdMsgEventProjectionStack_l, 0x78);
static hdMsgEventWindowScalar_l sEventPositionBaseFrame;
static hdMsgEventWindowScalar_l sEventPositionOtherFrame;
static hdMsgEventWindowScalar_l sEventPositionUpperFrame;
static cXyz sEventPositionOrigin;
/* 026B479C */
void hdMsgEventPosition_c::selectPositionAnimation() {
    gabi::AutoFrame automatic{gabi::Activation::current};
    gabi::NativeFrame<0x78> frame;
    // Projection callees receive original guest-stack destinations; matrix copying can alias them.
    auto& temporary = *gabi::at<hdMsgEventProjectionStack_l>(frame.sp());
    auto* anchor = reinterpret_cast<hdMsgEventAnchorMessage_l*>(mpMessage.get())->mpAnchor.get();
    if (!anchor) return;
    u32 mode = anchor->mMode;
    f32 selected = sEventPositionBaseFrame.mValue;
    bool upper = mode == 1;
    if (mode == 0) {
        auto* game = reinterpret_cast<dComIfG_inf_c*>(dComIfGp_get());
        auto* player = game->play.mPlayerInfo[0].mpPlayer.get();
        if (hdMsgEvent_samePosition(&mWorldPosition, &sEventPositionOrigin)) {
            u32 index = mTextPaneIndex;
            if (index >= 3) index = 0;
            auto* pane = reinterpret_cast<hdMsgEventTextPane_l*>(mTextPanes[index].get());
            u32 source = gabi::ea(&pane->mMatrixWords);
            u32 destination = gabi::ea(&temporary.mMatrixPrefix);
            if (source != destination) {
                for (u32 i = 0; i < 12; ++i) {
                    u32 word = *gabi::at<be<u32>>(source + i * 4);
                    *gabi::at<be<u32>>(destination + i * 4) = word;
                }
            }
            f32 y = temporary.mMatrixY;
            upper = !(y < selected);
        } else if (u8(anchor->mUseAnchor)) {
            hdMsgEvent_projectPosition(&mWorldPosition, &temporary.mAnchor);
            f32 y = temporary.mAnchor.y;
            upper = !(-y < selected);
        } else {
            hdMsgEvent_projectPosition(&player->eyePos, &temporary.mPlayer);
            hdMsgEvent_projectPosition(&mWorldPosition, &temporary.mWindow);
            f32 playerY = temporary.mPlayer.y;
            f32 windowY = temporary.mWindow.y;
            f32 y = -gabi::fadds_ppc(playerY, windowY);
            // The original tests the LT bit, so unordered comparisons choose the upper frame.
            upper = !(y < selected);
        }
    } else if (mode != 1 && mode != 3) {
        selected = sEventPositionOtherFrame.mValue;
    }
    if (upper) selected = sEventPositionUpperFrame.mValue;
    reinterpret_cast<hdMsgWindowStates_c*>(this)->setAnimationFrame(3, 2, selected);
}

/* WWHD glue
 ext 02704AFC void hdTextPane_c::setCharacterCapacity(u32)
 ext 026FF39C void hdMsgWindowStates_c::update()
 ext 026FF5AC void hdMsgWindowStates_c::readController()
 ext 026FF65C void hdMsgWindowStates_c::setAnimationFrame(u32, u32, f32)
 ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*, u32)
 ext 026211E0 hdBatonSignForWindow_c* hdBatonSignForWindow_c::construct(hdBatonSignForWindow_c*)
 ext 026212AC u32 hdBatonSignForWindow_c::initialize(u32, u32, void*)
 ext 026F826C void hdPartsKeyIcon_c::hide()
 ext 026F8228 void hdPartsKeyIcon_c::requestIn()
 ext 026F8238 void hdPartsKeyIcon_c::requestDecide()
 ext 026F8260 void hdPartsKeyIcon_c::requestOut()
 ext 026F82CC u32 hdPartsKeyIcon_c::isWaiting()
 ext 026F83B4 u32 hdPartsKeyIcon_c::isHidden()
 ext 026F833C u8 hdPartsKeyIcon_c::isNotDeciding()
 data sEventResetFrame 100FDAFC
 data sEventHeapOwner 1018C404
 data sEventPositionNames 10498290
 data sEventPositionClass 10494FE4
 data sEventFrameClass 10498690
 data sEventPaneClass 1048E7CC
 ext 026F92C0 u32 hdParts_c::createChild(u32, hdPartsDesc_c*)
 ext 0273B050 void* hdMsgEvent_allocateChild(u32, void*, s32)
 ext 02674164 hdKeyboardKeyIcon_c* constructKeyboardKeyIcon(hdKeyboardKeyIcon_c*)
 ext 026741D0 u32 hdKeyboardKeyIcon_c::create(u32, u32, u32)
 data sEventOpen 1049839C
 data sEventOpenName 100FDDB8
 data sEventPrepare 104983CC
 data sEventPrepareName 100FDD10
 data sEventOpening 104983FC
 data sEventOpeningName 100FDDCC
 data sEventOpeningSpecialName 100FDDDC
 data sEventBeatDoneName 100FDCA4
 data sEventShortWaitName 100FDCB4
 data sEventSequenceDoneName 100FDDEC
 data sEventOpened 104984EC
 data sEventOpenedName 100FDE04
 data sEventTyping 1049851C
 data sEventTypingName 100FDE18
 data sEventAdvanceName 100FDCC4
 data sEventClosedName 100FDE44
 data sEventCloseDone 104985AC
 data sEventCloseDoneName 100FDD28
 data sEventCloseWait 104985DC
 data sEventCloseWaitName 100FDE58
 data sEventAbort 1049860C
 data sEventAbortName 100FDE70
 data sEventInitializerHeader 1049820C
 data sEventDistances 104981A0
 data sEventAngleLimits 100FDBA4
 data sEventDistanceLimits 100FDBAC
 data sEventAngleValues 10498198
 data sEventAngle 104981B0
 data sEventPhase 104981B1
 data sEventHeaderRegistration 101F65D0
 data sEventAngleRegistration 101F65DC
 data sEventPhaseRegistration 101F65E8
 data sEventStringVtable 100FD88C
 data sEventStateVtable 100FD948
 data sEventNullState 101FF32C
 data sEventStateGuard 101FD8E4
 data sEventStateCounter 101FDD50
 data sEventResourceVariantsNames 1049821C
 data sEventChildSettingsNames 10498234
 data sEventLayoutResourcesNames 1049825C
 data sEventPositionAnimationsNames 10498310
 data sEventLayoutNameNames 1049863C
 data sEventChildSettingsText0 100FDBB4
 data sEventChildSettingsText2 100FDBBC
 data sEventChildSettingsText1 100FDBC4
 data sEventChildSettingsText4 100FDBCC
 data sEventPositionNamesText0 100FDBD4
 data sEventPositionNamesText1 100FDBE4
 data sEventPositionNamesText2 100FDBF4
 data sEventPositionNamesText3 100FDC04
 data sEventPositionNamesText4 100FDC14
 data sEventPositionNamesText5 100FDC24
 data sEventPositionNamesText6 100FDC34
 data sEventPositionNamesText7 100FDC44
 data sEventPositionNamesText8 100FDC54
 data sEventPositionNamesText9 100FDC64
 data sEventPositionNamesText10 100FDC74
 data sEventPositionNamesText11 100FDC84
 data sEventPositionNamesText12 100FDC94
 data sEventResourceVariantsText2 100FDCD4
 data sEventLayoutResourcesText1 100FDCE4
 data sEventLayoutResourcesText0 100FDCFC
 data sEventResourceVariantsText0 100FDD40
 data sEventResourceVariantsText1 100FDD4C
 data sEventChildSettingsText3 100FDD58
 data sEventPositionNamesText13 100FDD64
 data sEventPositionNamesText14 100FDD74
 data sEventPositionNamesText15 100FDD84
 data sEventLayoutResourcesText2 100FDD94
 data sEventLayoutResourcesText3 100FDDAC
 data sEventWindowNames 104981B4
 data sEventWindowNamesGuard 10498394
 data sEventWindowNamesRegistration 101F65B8
 data sEventWindowPaneName0 100FDB3C
 data sEventWindowPaneName1 100FDB48
 data sEventWindowPaneName2 100FDB58
 data sEventWindowPaneName3 100FDB00
 data sEventWindowPaneName4 100FDB10
 data sEventWindowPaneName5 100FDB20
 data sEventWindowPaneName6 100FDB30
 data sEventWindowPaneName7 100FDB68
 data sEventInitialState 1049DD60
 ext 025E1E60 u32 hdMsgEvent_currentBgm()

 data sEventSave 101F84DC
 data sEventPositionWaitState 1049E2BC
 data sEventPositionDisplayState 1049E10C
 ext 025B7E00 u32 hdMsgEvent_triforceCount(dSv_player_collect_c*)


 data sEventAnimationResources 100FD8EC
 ext 02004E04 void hdMsgEvent_bindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*)


 data sEventPositionBaseFrame 100FDAFC
 data sEventPositionOtherFrame 100FD990
 data sEventPositionUpperFrame 100FDBA0
 data sEventPositionOrigin 101FFBA8
 ext 0201AF98 u32 hdMsgEvent_samePosition(cXyz*, cXyz*)
 ext 025F0EA4 void hdMsgEvent_projectPosition(cXyz*, cXyz*)
*/

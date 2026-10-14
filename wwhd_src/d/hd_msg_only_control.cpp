/**
 * hd_msg_only_control.cpp
 * HD - the message-only window: loading, the controller input, the selection sound, the text
 * steps, animations and key-icon requests
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_only_window.h"
#include "d/hd_parts_key_icon.h"
#include "d/hd_keyboard_screen.h"
#include "d/hd_input_ctrl.h"
using namespace gabi;
enum MsgOnlyAnimation { MSG_ONLY_IN_ALL = 0, MSG_ONLY_IN_SUB_WINDOW = 2, MSG_ONLY_OUT_SUB_WINDOW = 3, MSG_ONLY_CHANGE_TEXT = 4, MSG_ONLY_OUT_ALL = 5 };
static f32 sMsgControlZero;
static f32 sMsgControlOne;
static f32 sMsgControlTwo;
struct hdMsgControlStops_l { u8 mReserved[0x98]; be<s32> mFirstStops[10]; };
struct hdMsgControlAttribute_l { u8 mReserved[3]; be<u8> mKind; };
struct hdMsgControlWindows_l { u8 mReserved[0x120]; gptr<hdPartsPaneSRT_c> mpSubWindow; };
struct hdMsgControlSelection_l {
    u8 mReserved[0xEC]; be<u8> mPreviousActive; be<u8> mActive;
    u8 mReservedEE[2]; be<u32> mPreviousSelection; be<u32> mSelection;
    u8 mReservedF8[0x30]; gptr<void> mpSelector;
};
struct hdMsgControlCore_l { u8 mReserved[0x14]; HdMessageData mMessage; };
WWHD_OFFSET(hdMsgControlCore_l, mMessage, 0x14);
struct hdMsgControlCorePointer_l { gptr<hdMsgControlCore_l> mpCore; };
static hdMsgControlCorePointer_l sMsgControlCore;
extern u32 msgOnlyControlSelect(void*, u32, u32, void*, u32);
static hdPartsStateId_c sMsgControlBlankState;
static hdPartsStateId_c sMsgControlCloseState;
struct hdMsgControlArchives_l { hdUiSafeString mNames[2]; };
struct hdMsgControlRequestPointer_l { gptr<void> mpRequest; };
static hdMsgControlArchives_l sMsgControlArchives;
static hdUiSafeString sMsgControlLayoutName;
static hdMsgControlRequestPointer_l sMsgControlRequest;
struct hdMsgControlSystem_l { gptr<hdScreenUiSystem> mpSystem; };
struct hdMsgControlAnimationNames_l { hdUiSafeString mNames[5]; };
struct hdMsgControlGroupNames_l { hdUiSafeString mNames[7]; };
struct hdMsgControlAnimationSlots_l { be<u32> mSlots[7]; };
static hdMsgControlSystem_l sMsgControlSystem;
static hdMsgControlAnimationNames_l sMsgControlFiveGroups;
static hdMsgControlGroupNames_l sMsgControlSevenAnimations;
static hdMsgControlAnimationSlots_l sMsgControlAnimationSlots;
extern void* msgOnlyControlAllocate(u32, hdScreenHeap*, s32);
extern void msgOnlyControlBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
struct hdMsgControlDescriptor_l { u8 mReserved[8]; hdUiSafeString mType; u8 mReserved10[4]; hdUiSafeString mName; };
WWHD_OFFSET(hdMsgControlDescriptor_l, mType, 8);
WWHD_OFFSET(hdMsgControlDescriptor_l, mName, 0x14);
static hdUiSafeString sMsgControlSelectionName;
static hdUiSafeString sMsgControlKeyType;
struct hdMsgControlOpenFields_l {
    u8 mReserved[0x5C]; be<u32> mMessageId; u8 mReserved60[0x28]; be<u32> mSelectionId;
    u8 mReserved8C[0x6C]; hdUiSafeString mLabel; u8 mReserved100[0x12]; be<u32> mDefaultColor;
};
struct hdMsgControlGroup_l { gptr<void> mpEntries; be<u32> mCount; };
struct hdMsgControlMessageLabels_l {
    u8 mReserved[8]; gptr<hdMsgControlGroup_l> mpGroup; PaneString_l mFirstLabel;
    u8 mReserved18[0x100]; be<u32> mGroupIndex; u8 mReserved11C[4]; PaneString_l mSecondLabel;
};
struct hdMsgControlMessageColors_l { u8 mReserved[0x919]; be<u32> mFirstColor; be<u32> mSecondColor; be<u8> mReady; };
struct hdMsgControlOpenAttribute_l { be<u16> mKind; be<u16> mSubKind; be<u16> mMessageId; };
struct hdMsgControlService_l { gptr<void> mpService; };
static hdMsgControlService_l sMsgControlService;
WWHD_OFFSET(hdMsgControlOpenFields_l, mMessageId, 0x5C);
WWHD_OFFSET(hdMsgControlOpenFields_l, mSelectionId, 0x88);
WWHD_OFFSET(hdMsgControlOpenFields_l, mLabel, 0xF8);
WWHD_OFFSET(hdMsgControlOpenFields_l, mDefaultColor, 0x112);
WWHD_OFFSET(hdMsgControlMessageLabels_l, mGroupIndex, 0x118);
WWHD_OFFSET(hdMsgControlMessageLabels_l, mSecondLabel, 0x120);
WWHD_OFFSET(hdMsgControlMessageColors_l, mFirstColor, 0x919);
WWHD_OFFSET(hdMsgControlMessageColors_l, mSecondColor, 0x91D);
WWHD_OFFSET(hdMsgControlMessageColors_l, mReady, 0x921);
extern void msgOnlyControlOpenMessage(HdMessageData*, u32, hdUiSafeString*);
extern void msgOnlyControlResolveMessage(void*, HdMessageData*, PaneString_l*, PaneString_l*);
extern hdMsgControlOpenAttribute_l* msgOnlyControlGetAttribute(void*, u32);
extern hdParts_c* msgOnlyControlConstructKey(hdParts_c*);
static bool msgOnlyControlStringsMatch(hdUiSafeString* source, hdUiSafeString* expected) {
    source->assureTermination();
    source->assureTermination();
    auto* cached = source->mText.get();
    expected->assureTermination();
    if (cached == expected->mText.get()) return true;
    auto* left = source->mText.get();
    auto* right = expected->mText.get();
    for (u32 i = 0; i<0x40001; ++i) {
        u32 a = left[i];
        if (a != right[i]) return false;
        if (!a) return true;
    }
    return false;
}
enum { MSG_ONLY_PREPARE_WINDOW = 0x444/4 };
extern hdPartsStateId_c* msgOnlyControlCurrentState(hdStateMachine*);
extern void msgOnlyControlUpdateState(hdStateMachine*);
struct hdMsgControlInputMode_l { u8 mReserved[0x58]; be<u32> mInputMode; };
struct hdMsgControlTextPosition_l { u8 mReserved[0x64C]; be<s32> mFirstStop; be<u32> mLastStop; be<s32> mShown; };
WWHD_OFFSET(hdMsgControlTextPosition_l, mFirstStop, 0x64C);
WWHD_OFFSET(hdMsgControlTextPosition_l, mLastStop, 0x650);
WWHD_OFFSET(hdMsgControlTextPosition_l, mShown, 0x654);
extern void msgOnlyControlAdvanceText(HdMessageData*, s32, s32);
extern void msgOnlyControlPopulateText(HdMessagePane*, HdMessageData*, u32);
WWHD_OFFSET(hdMsgControlInputMode_l, mInputMode, 0x58);
static bool msgOnlyControlIsState(hdParts_c* parts, hdPartsStateId_c* expected) {
    auto* current = msgOnlyControlCurrentState(parts->stateMachine());
    u32 currentGetter = current->mVtable->mGetKey.get();
    auto* expectedVtable = expected->mVtable.get();
    u32 currentKey = call_ptr<u32>(currentGetter, current);
    return currentKey == call_ptr<u32>(expectedVtable->mGetKey.get(), expected);
}
WWHD_OFFSET(hdMsgControlSelection_l, mPreviousActive, 0xEC);
WWHD_OFFSET(hdMsgControlSelection_l, mPreviousSelection, 0xF0);
WWHD_OFFSET(hdMsgControlSelection_l, mSelection, 0xF4);
WWHD_OFFSET(hdMsgControlSelection_l, mpSelector, 0x128);
WWHD_OFFSET(hdMsgControlStops_l, mFirstStops, 0x98);
WWHD_OFFSET(hdMsgControlAttribute_l, mKind, 3);
WWHD_OFFSET(hdMsgControlWindows_l, mpSubWindow, 0x120);
static hdPartsKeyIcon_c* msgOnlyKeyIcon(hdMsgOnlyWindow_c* window) {
    auto* parts = reinterpret_cast<hdParts_c*>(window);
    return reinterpret_cast<hdPartsKeyIcon_c*>(parts->mChildren.mpData[0].get());
}

/* 026B01EC */
u32 hdMsgOnlyWindow_c::initialize(u32 first, u32 second, void* descriptor) {
    mpRequest = sMsgControlRequest.mpRequest;
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    parts->attachArchives(sMsgControlArchives.mNames, 2);
    auto* accessor = parts->mpResAccessor.get();
    if (!call_ptr<u32>(parts->mVtable->mLoad.get(), this, &sMsgControlLayoutName, accessor, first, second, descriptor)) return 0;
    if (!call_ptr<u32>(parts->mVtable->mCreateChildren.get(), this)) return 0;
    findWindowPanes();
    findTextPanes();
    findChoicePanes();
    findSelectionPane();
    findSelectionResource();
    auto* root = reinterpret_cast<hdPartsPaneSRT_c*>(parts->mpLayout->lyt()->mpRootPane.get());
    auto* key = call_ptr<hdPartsPaneSRT_c*>(root->srtVtbl()->mFindPane.get(), root, guest_str("L_KayIcon_00"), 1);
    if (key) key->mFlags = key->mFlags&~1u;
    mMessage = &sMsgControlCore.mpCore->mMessage;
    call_ptr<void>(mVTable[MSG_ONLY_PREPARE_WINDOW].get(), this);
    parts->startState(&sMsgControlBlankState);
    return 1;
}

/* 026B0384 */
void hdMsgOnlyWindow_c::release() {
    hdMessageGetPlay()->mAdvanceCount = 0;
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    parts->vDeleteChildren();
    parts->vDestroyLayout();
    parts->detachArchives();
}

/* 026B03E8 */
void hdMsgOnlyWindow_c::updateSelectionSound() {
    auto* selection = reinterpret_cast<hdMsgControlSelection_l*>(this);
    u32 previousActive = selection->mPreviousActive;
    if (previousActive) {
        if (!selection->mActive) {
            if (!msgOnlyControlSelect(sMsgControlCore.mpCore, 0, selection->mSelection, selection->mpSelector, 1)) return;
            selection->mPreviousActive = selection->mActive;
        } else {
            u32 value = selection->mSelection;
            if (selection->mPreviousSelection == value) return;
            if (!msgOnlyControlSelect(sMsgControlCore.mpCore, previousActive, value, selection->mpSelector, 1)) return;
        }
    } else {
        u32 active = selection->mActive;
        if (!active) return;
        if (!msgOnlyControlSelect(sMsgControlCore.mpCore, active, selection->mSelection, selection->mpSelector, 1)) return;
        selection->mPreviousActive = selection->mActive;
    }
    selection->mPreviousSelection = selection->mSelection;
}

/* 026B04B4 */
void hdMsgOnlyWindow_c::execute() {
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    if (!msgOnlyControlIsState(parts, &sMsgControlBlankState) &&
       !msgOnlyControlIsState(parts, &sMsgControlCloseState))
        hdMessageGetPlay()->mAdvanceCount = 0;
    auto* manager = hdInputGetManager();
    if (manager) {
        auto* input = manager->mInput.get();
        if (input) {
            mFlags = input->mRepeat;
            mInputFlags = input->mHold;
            u32 held = input->mHold;
            u32 released = input->mRelease;
            reinterpret_cast<hdMsgControlInputMode_l*>(this)->mInputMode = held|released;
        }
    }
    msgOnlyControlUpdateState(parts->stateMachine());
    parts->vExecuteChildren();
    parts->vCalcLayout();
    updateSelectionSound();
}

/* 026B060C */
u32 hdMsgOnlyWindow_c::createChild(hdPartsDesc_c* descriptor) {
    auto* fields = reinterpret_cast<hdMsgControlDescriptor_l*>(descriptor);
    // The heap is captured before the first source-string callback.
    auto* heap = sMsgControlSystem.mpSystem->mHeap.get();
    if (!msgOnlyControlStringsMatch(&fields->mName, &sMsgControlSelectionName)) return 1;
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    if (!msgOnlyControlStringsMatch(&fields->mType, &sMsgControlKeyType))
        return parts->hdParts_c::createChild(0, descriptor);
    auto* children = parts->mChildren.mpData.get();
    auto* key = static_cast<hdParts_c*>(msgOnlyControlAllocate(0x58, heap, 4));
    if (key) key = msgOnlyControlConstructKey(key);
    children[0] = key;
    key = parts->mChildren.mpData[0].get();
    if (!key) return 0;
    return reinterpret_cast<hdKeyboardKeyIcon_c*>(key)->create(0, 0, ea(descriptor)) ? 1 : 0;
}

/* 026B0810 */
u32 hdMsgOnlyWindow_c::loadLayout(void* name, void* resource, u32 first, u32 second, void* descriptor) {
    auto* heap = sMsgControlSystem.mpSystem->mHeap.get();
    auto* layout = static_cast<hdPartsLayout_c*>(msgOnlyControlAllocate(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    parts->mpLayout = layout;
    if (!layout || !layout->vLoad(name, ea(resource), first, second, 7, 5, descriptor)) return 0;
    for (u32 i = 0; i<7; ++i) {
        auto* animations = parts->animSet();
        u32 slot = sMsgControlAnimationSlots.mSlots[i];
        msgOnlyControlBindAnimation(animations, i, &sMsgControlSevenAnimations.mNames[i], &sMsgControlFiveGroups.mNames[slot]);
    }
    parts->vCalcLayout();
    return 1;
}

/* 026B093C */
void hdMsgOnlyWindow_c::stepText() {
    auto* first = reinterpret_cast<hdMsgControlStops_l*>(this)->mFirstStops;
    u32 index = mStopIndex;
    msgOnlyControlAdvanceText(mMessage, first[index<10?index:0], mShown);
    msgOnlyControlPopulateText(mWaitPane, mMessage, 0);
    mShown = reinterpret_cast<hdMsgControlTextPosition_l*>(mMessage.get())->mShown;
    for (u32 i = 0; i<2; ++i) {
        auto* choice = reinterpret_cast<hdPartsPaneSRT_c*>(mpChoicePanes[i].get());
        choice->mFlags = choice->mFlags&~1u;
    }
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mWaitPane.get());
    pane->mFlags = (pane->mFlags&~1u)+1;
    u32 next = mStopIndex+1;
    if (first[next<10?next:0]<0) {
        auto* message = reinterpret_cast<hdMsgControlTextPosition_l*>(mMessage.get());
        first[next<10?next:0] = message->mFirstStop;
        next = mStopIndex+1;
    }
    if (mStops[next<10?next:0]<0) {
        u32 last = reinterpret_cast<hdMsgControlTextPosition_l*>(mMessage.get())->mLastStop;
        mStops[next<10?next:0] = last+1;
    }
}

/* 026B0AC4 */
void hdMsgOnlyWindow_c::updateAttributeAnimation() {
    auto* attribute = reinterpret_cast<hdMsgControlAttribute_l*>(mMessage->mText.get());
    if (!attribute) return;
    u32 kind = attribute->mKind;
    f32 frame = kind == 1?sMsgControlTwo:kind == 2?sMsgControlOne:sMsgControlZero;
    windowStates()->setAnimationFrame(6, 3, frame);
}

/* 026B0B1C */
void hdMsgOnlyWindow_c::resetAndHide() {
    auto* stops = reinterpret_cast<hdMsgControlStops_l*>(this);
    for (u32 i = 0; i<10; ++i) stops->mFirstStops[i] = -1;
    for (u32 i = 0; i<10; ++i) mStops[i] = -1;
    f32 zero = sMsgControlZero;
    mStopIndex = 0;
    windowStates()->setAnimationFrame(MSG_ONLY_IN_ALL, 1, zero);
    windowStates()->setAnimationFrame(MSG_ONLY_IN_SUB_WINDOW, 0, zero);
    windowStates()->setAnimationFrame(MSG_ONLY_CHANGE_TEXT, 2, zero);
    auto* parts = reinterpret_cast<hdParts_c*>(this);
    parts->vCalcLayout();
    parts->vExecuteChildren();
    auto* pane = reinterpret_cast<hdMsgControlWindows_l*>(this)->mpSubWindow.get();
    mStopFlag = 0;
    pane->mFlags = pane->mFlags&~1u;
}

/* 026B0C1C */
void hdMsgOnlyWindow_c::resetAndOpenMessage() {
    auto* fields = reinterpret_cast<hdMsgControlOpenFields_l*>(this);
    auto* first = reinterpret_cast<hdMsgControlStops_l*>(this)->mFirstStops;
    for (u32 i = 0; i<10; ++i) first[i] = -1;
    for (u32 i = 0; i<10; ++i) mStops[i] = -1;
    mStopIndex = 0;
    first[0] = 0;
    mShown = 0;
    u32 color = fields->mDefaultColor;
    auto* pane = mPane.get();
    pane->mColor1 = color;
    pane->mColor2 = color;
    color = fields->mDefaultColor;
    reinterpret_cast<hdMsgControlMessageColors_l*>(mMessage.get())->mFirstColor = color;
    color = fields->mDefaultColor;
    reinterpret_cast<hdMsgControlMessageColors_l*>(mMessage.get())->mSecondColor = color;
    mStopFlag = 0;
    msgOnlyControlOpenMessage(mMessage, fields->mMessageId, &fields->mLabel);
    reinterpret_cast<hdMsgControlMessageColors_l*>(mMessage.get())->mReady = 1;
    auto* message = mMessage.get();
    auto* labels = reinterpret_cast<hdMsgControlMessageLabels_l*>(message);
    msgOnlyControlResolveMessage(sMsgControlService.mpService, message, &labels->mFirstLabel, &labels->mSecondLabel);
    labels = reinterpret_cast<hdMsgControlMessageLabels_l*>(mMessage.get());
    auto* group = labels->mpGroup.get();
    if (!group) return;
    u32 index = labels->mGroupIndex;
    if (index >= group->mCount) return;
    auto* attribute = msgOnlyControlGetAttribute(group->mpEntries, index);
    if (!attribute || (attribute->mKind != 14 && attribute->mKind != 15) || attribute->mSubKind != 5) return;
    u32 id = attribute->mMessageId;
    hdMessageGetPlay()->mMessageId = id;
    fields->mSelectionId = attribute->mMessageId;
}

/* 026B0DDC */
void hdMsgOnlyWindow_c::hideTextPanes() {
    for (u32 i = 0; i<3; ++i) {
        auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(i == 0?reinterpret_cast<hdTextPane_c*>(mWaitPane.get()):mpAdditionalText[i-1].get());
        pane->mFlags = pane->mFlags&~1u;
    }
}

/* 026B0E2C */
void hdMsgOnlyWindow_c::hideChoicePanes() {
    for (u32 i = 0; i<2; ++i) {
        auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpChoicePanes[i].get());
        pane->mFlags = pane->mFlags&~1u;
    }
}

/* 026B0E7C */
void hdMsgOnlyWindow_c::playOpen() { windowStates()->startAnimation(MSG_ONLY_IN_ALL, 1); }

/* 026B0E88 */
u32 hdMsgOnlyWindow_c::isOpenEnded() { return windowStates()->animationFinished(MSG_ONLY_IN_ALL); }

/* 026B0E90 */
void hdMsgOnlyWindow_c::playClose() { windowStates()->startAnimation(MSG_ONLY_OUT_ALL, 1); }

/* 026B0E9C */
u32 hdMsgOnlyWindow_c::isCloseEnded() { return windowStates()->animationFinished(MSG_ONLY_OUT_ALL); }

/* 026B0EA4 */
void hdMsgOnlyWindow_c::playTextChange() { windowStates()->startAnimation(MSG_ONLY_CHANGE_TEXT, 2); }

/* 026B0EB0 */
void hdMsgOnlyWindow_c::playSubWindowIn() { windowStates()->startAnimation(MSG_ONLY_IN_SUB_WINDOW, 0); }

/* 026B0EBC */
u32 hdMsgOnlyWindow_c::isSubWindowInEnded() { return windowStates()->animationFinished(MSG_ONLY_IN_SUB_WINDOW); }

/* 026B0EC4 */
void hdMsgOnlyWindow_c::playSubWindowOut() { windowStates()->startAnimation(MSG_ONLY_OUT_SUB_WINDOW, 0); }

/* 026B0ED0 */
u32 hdMsgOnlyWindow_c::isSubWindowOutEnded() { return windowStates()->animationFinished(MSG_ONLY_OUT_SUB_WINDOW); }

/* 026B0ED8 */
void hdMsgOnlyWindow_c::hideSelection() {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpSelectionPane.get());
    pane->mFlags = pane->mFlags&~1u;
    msgOnlyKeyIcon(this)->hide();
}

/* 026B0EF4 */
void hdMsgOnlyWindow_c::showSelection() {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpSelectionPane.get());
    pane->mFlags = (pane->mFlags&~1u)+1;
    msgOnlyKeyIcon(this)->requestIn();
}

/* 026B0F14 */
void hdMsgOnlyWindow_c::decideSelection() {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpSelectionPane.get());
    pane->mFlags = (pane->mFlags&~1u)+1;
    msgOnlyKeyIcon(this)->requestDecide();
}

/* 026B0F34 */
void hdMsgOnlyWindow_c::closeSelection() {
    auto* pane = reinterpret_cast<hdPartsPaneSRT_c*>(mpSelectionPane.get());
    pane->mFlags = (pane->mFlags&~1u)+1;
    msgOnlyKeyIcon(this)->requestOut();
}

/* 026B0F54 */
u32 hdMsgOnlyWindow_c::isSelectionWaiting() { return msgOnlyKeyIcon(this)->isWaiting(); }

/* 026B0F60 */
u32 hdMsgOnlyWindow_c::isSelectionHidden() { return msgOnlyKeyIcon(this)->isHidden(); }

/* 026B0F6C */
u32 hdMsgOnlyWindow_c::isSelectionNotDeciding() { return msgOnlyKeyIcon(this)->isNotDeciding(); }
struct hdMsgControlStaticPointers_l { gptr<void> mPointers[4]; };
struct hdMsgControlRegistration_l { be<u32> mWords[3]; };
static hdMsgControlStaticPointers_l sMsgControlPointers;
static hdMsgControlRegistration_l sMsgControlPointersRegistration;
static hdMsgControlRegistration_l sMsgControlFirstRegistration;
static hdMsgControlRegistration_l sMsgControlSecondRegistration;
static f32 sMsgControlMinimum;
static f32 sMsgControlMaximum;
static f32 sMsgControlMinimumConstant;
static f32 sMsgControlMaximumConstant;
static u8 sMsgControlFirstFlag;
static u8 sMsgControlSecondFlag;
static u32 sMsgControlCounterInitialized;
static u32 sMsgControlCounter;
static hdUiSafeString sMsgControlSecondaryName;
static hdUiStringTable sMsgControlStringVtable;
static hdPartsStateIdVtbl_c sMsgControlStateVtable;
static hdStateId sMsgControlNullState;
extern void msgOnlyControlRegister(hdMsgControlRegistration_l*);
extern void msgOnlyControlConstructFirstFlag(be<u8>*);
extern void msgOnlyControlConstructSecondFlag(be<u8>*);
static hdPartsStateId_c sMsgControlOutNowState;
static hdPartsStateId_c sMsgControlAutoCloseState;
static hdPartsStateId_c sMsgControlHandCloseState;
static hdPartsStateId_c sMsgControlNpcAnswerState;
static hdPartsStateId_c sMsgControlContinueState;
static hdPartsStateId_c sMsgControlDecideOutState;
static hdPartsStateId_c sMsgControlDecideOut2State;
static hdPartsStateId_c sMsgControlKeyOutState;
static hdPartsStateId_c sMsgControlDecideCloseState;

/* 026B0F78 */
static void initializeMsgWindowMainStatics() {
    for (s32 i = 3; i >= 0; --i) sMsgControlPointers.mPointers[i] = nullptr;
    msgOnlyControlRegister(&sMsgControlPointersRegistration);
    f32 minimum = sMsgControlMinimumConstant;
    f32 maximum = sMsgControlMaximumConstant;
    sMsgControlMinimum = minimum;
    sMsgControlMaximum = maximum;
    msgOnlyControlConstructFirstFlag(reinterpret_cast<be<u8>*>(&sMsgControlFirstFlag));
    msgOnlyControlRegister(&sMsgControlFirstRegistration);
    msgOnlyControlConstructSecondFlag(reinterpret_cast<be<u8>*>(&sMsgControlSecondFlag));
    msgOnlyControlRegister(&sMsgControlSecondRegistration);
    sMsgControlFiveGroups.mNames[0].mVtable = &sMsgControlStringVtable;
    sMsgControlFiveGroups.mNames[0].mText.v = guest_str("G_SubWindow_00");
    sMsgControlFiveGroups.mNames[1].mVtable = &sMsgControlStringVtable;
    sMsgControlFiveGroups.mNames[1].mText.v = guest_str("G_All_00");
    sMsgControlFiveGroups.mNames[2].mVtable = &sMsgControlStringVtable;
    sMsgControlFiveGroups.mNames[2].mText.v = guest_str("G_LoopAll_00");
    sMsgControlFiveGroups.mNames[3].mVtable = &sMsgControlStringVtable;
    sMsgControlFiveGroups.mNames[3].mText.v = guest_str("G_Position_00");
    sMsgControlFiveGroups.mNames[4].mVtable = &sMsgControlStringVtable;
    sMsgControlFiveGroups.mNames[4].mText.v = guest_str("G_LoopSelNumber_00");
    sMsgControlSevenAnimations.mNames[0].mVtable = &sMsgControlStringVtable;
    sMsgControlSevenAnimations.mNames[0].mText.v = guest_str("InAll");
    sMsgControlSevenAnimations.mNames[1].mVtable = &sMsgControlStringVtable;
    sMsgControlSevenAnimations.mNames[1].mText.v = guest_str("LoopSelNumber");
    sMsgControlSevenAnimations.mNames[2].mVtable = &sMsgControlStringVtable;
    sMsgControlSevenAnimations.mNames[2].mText.v = guest_str("InSubWindow");
    sMsgControlSevenAnimations.mNames[3].mVtable = &sMsgControlStringVtable;
    sMsgControlSevenAnimations.mNames[3].mText.v = guest_str("OutSubWindow");
    sMsgControlSevenAnimations.mNames[4].mVtable = &sMsgControlStringVtable;
    sMsgControlSevenAnimations.mNames[4].mText.v = guest_str("LoopAll");
    sMsgControlSevenAnimations.mNames[5].mVtable = &sMsgControlStringVtable;
    sMsgControlSevenAnimations.mNames[5].mText.v = guest_str("OutAll");
    sMsgControlSevenAnimations.mNames[6].mVtable = &sMsgControlStringVtable;
    sMsgControlSevenAnimations.mNames[6].mText.v = guest_str("Position");
    sMsgControlSelectionName.mVtable = &sMsgControlStringVtable;
    sMsgControlSelectionName.mText.v = guest_str("L_KeyIcon_00");
    sMsgControlSecondaryName.mVtable = &sMsgControlStringVtable;
    sMsgControlSecondaryName.mText.v = guest_str("KeyIcon_00");
    sMsgControlArchives.mNames[0].mVtable = &sMsgControlStringVtable;
    sMsgControlArchives.mNames[0].mText.v = guest_str("MsgWindowMain_00");
    sMsgControlArchives.mNames[1].mVtable = &sMsgControlStringVtable;
    sMsgControlArchives.mNames[1].mText.v = guest_str("KeyIcon_00");
    sMsgControlLayoutName.mVtable = &sMsgControlStringVtable;
    sMsgControlLayoutName.mText.v = guest_str("MsgWindowMain_00");
    u32 counter;
    if (sMsgControlCounterInitialized) counter = sMsgControlCounter;
    else { sMsgControlCounterInitialized = 1; counter = 0; }
    sMsgControlBlankState.setup(s32(++counter), "::StateID_Wait", &sMsgControlStateVtable, 0x9B, &sMsgControlNullState);
    sMsgControlOutNowState.setup(s32(++counter), "::StateID_OutNow", &sMsgControlStateVtable, 0x22, &sMsgControlNullState);
    sMsgControlAutoCloseState.setup(s32(++counter), "::StateID_AutoCloseWait", &sMsgControlStateVtable, 0x55, &sMsgControlNullState);
    sMsgControlHandCloseState.setup(s32(++counter), "::StateID_HandCloseWait", &sMsgControlStateVtable, 0x5B, &sMsgControlNullState);
    sMsgControlNpcAnswerState.setup(s32(++counter), "::StateID_NpcAnswerWait", &sMsgControlStateVtable, 0x5E, &sMsgControlNullState);
    sMsgControlContinueState.setup(s32(++counter), "::StateID_Continue", &sMsgControlStateVtable, 0x64, &sMsgControlNullState);
    sMsgControlCloseState.setup(s32(++counter), "::StateID_Close", &sMsgControlStateVtable, 0x67, &sMsgControlNullState);
    sMsgControlDecideOutState.setup(s32(++counter), "::StateID_KeyIconDecideAnimEndWaitToOutNow", &sMsgControlStateVtable, 0x6D, &sMsgControlNullState);
    sMsgControlDecideOut2State.setup(s32(++counter), "::StateID_KeyIconDecideAnimEndWaitToOutNow2", &sMsgControlStateVtable, 0x70, &sMsgControlNullState);
    sMsgControlKeyOutState.setup(s32(++counter), "::StateID_KeyIconOutAnimEndWaitToOutNow", &sMsgControlStateVtable, 0x73, &sMsgControlNullState);
    sMsgControlCounter = ++counter;
    sMsgControlDecideCloseState.setup(s32(counter), "::StateID_KeyIconDecideAnimEndWaitToClose", &sMsgControlStateVtable, 0x76, &sMsgControlNullState);
}

/* WWHD glue
 data sMsgControlPointers 10497EF0
 data sMsgControlPointersRegistration 101F6588
 data sMsgControlFirstRegistration 101F6594
 data sMsgControlSecondRegistration 101F65A0
 data sMsgControlMinimum 10497E9C
 data sMsgControlMaximum 10497EA0
 data sMsgControlMinimumConstant 100FD154
 data sMsgControlMaximumConstant 100FD158
 data sMsgControlFirstFlag 10497EA4
 data sMsgControlSecondFlag 10497EA5
 data sMsgControlCounterInitialized 101FD8E4
 data sMsgControlCounter 101FDD50
 data sMsgControlSecondaryName 10497E94
 data sMsgControlStringVtable 100FCF14
 data sMsgControlStateVtable 100FCFD8
 data sMsgControlNullState 101FF32C
 data sMsgControlOutNowState 10497FB0
 data sMsgControlAutoCloseState 10497FE0
 data sMsgControlHandCloseState 10498010
 data sMsgControlNpcAnswerState 10498040
 data sMsgControlContinueState 10498070
 data sMsgControlDecideOutState 104980D0
 data sMsgControlDecideOut2State 10498100
 data sMsgControlKeyOutState 10498130
 data sMsgControlDecideCloseState 10498160
 str 100FD278 "G_SubWindow_00" in 026B0F78
 str 100FD288 "G_All_00" in 026B0F78
 str 100FD294 "G_LoopAll_00" in 026B0F78
 str 100FD22C "G_Position_00" in 026B0F78
 str 100FD2A4 "G_LoopSelNumber_00" in 026B0F78
 str 100FD164 "InAll" in 026B0F78
 str 100FD23C "LoopSelNumber" in 026B0F78
 str 100FD1F4 "InSubWindow" in 026B0F78
 str 100FD2B8 "OutSubWindow" in 026B0F78
 str 100FD15C "LoopAll" in 026B0F78
 str 100FD16C "OutAll" in 026B0F78
 str 100FD2C8 "Position" in 026B0F78
 str 100FD2D4 "L_KeyIcon_00" in 026B0F78
 str 100FD2E4 "KeyIcon_00" in 026B0F78
 str 100FD2F0 "MsgWindowMain_00" in 026B0F78
 str 100FD304 "::StateID_Wait" in 026B0F78
 str 100FD314 "::StateID_OutNow" in 026B0F78
 str 100FD174 "::StateID_AutoCloseWait" in 026B0F78
 str 100FD18C "::StateID_HandCloseWait" in 026B0F78
 str 100FD1A4 "::StateID_NpcAnswerWait" in 026B0F78
 str 100FD328 "::StateID_Continue" in 026B0F78
 str 100FD1BC "::StateID_Close" in 026B0F78
 str 100FD33C "::StateID_KeyIconDecideAnimEndWaitToOutNow" in 026B0F78
 str 100FD200 "::StateID_KeyIconDecideAnimEndWaitToOutNow2" in 026B0F78
 str 100FD1CC "::StateID_KeyIconOutAnimEndWaitToOutNow" in 026B0F78
 str 100FD24C "::StateID_KeyIconDecideAnimEndWaitToClose" in 026B0F78
 ext 028F026C void msgOnlyControlRegister(hdMsgControlRegistration_l*)
 ext 028ED6F8 void msgOnlyControlConstructFirstFlag(be<u8>*)
 ext 028EAB2C void msgOnlyControlConstructSecondFlag(be<u8>*)
 data sMsgControlZero 100FD0A4
 data sMsgControlOne 100FD020
 data sMsgControlTwo 100FD150
 data sMsgControlCore 101F4B5C
 ext 025F85C0 u32 msgOnlyControlSelect(void*,u32,u32,void*,u32)
 data sMsgControlBlankState 10497F80
 data sMsgControlCloseState 104980A0
 ext 02006478 hdPartsStateId_c* msgOnlyControlCurrentState(hdStateMachine*)
 ext 02006364 void msgOnlyControlUpdateState(hdStateMachine*)
 ext 025F6FCC void msgOnlyControlAdvanceText(HdMessageData*,s32,s32)
 ext 02704504 void msgOnlyControlPopulateText(HdMessagePane*,HdMessageData*,u32)
 data sMsgControlArchives 10497F60
 data sMsgControlLayoutName 10498190
 data sMsgControlRequest 104756C8
 str 100FD140 "L_KayIcon_00" in 026B01EC
 data sMsgControlSystem 1018C404
 data sMsgControlFiveGroups 10497F00
 data sMsgControlSevenAnimations 10497F28
 data sMsgControlAnimationSlots 100FCF74
 ext 0273B050 void* msgOnlyControlAllocate(u32,hdScreenHeap*,s32)
 ext 02004E04 void msgOnlyControlBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 data sMsgControlSelectionName 10497E8C
 data sMsgControlKeyType 10494FE4
 ext 02674164 hdParts_c* msgOnlyControlConstructKey(hdParts_c*)
 ext 026741D0 u32 hdKeyboardKeyIcon_c::create(u32,u32,u32)
 data sMsgControlService 101F4AE8
 ext 025F6B18 void msgOnlyControlOpenMessage(HdMessageData*,u32,hdUiSafeString*)
 ext 025F50E8 void msgOnlyControlResolveMessage(void*,HdMessageData*,PaneString_l*,PaneString_l*)
 ext 0273A2E8 hdMsgControlOpenAttribute_l* msgOnlyControlGetAttribute(void*,u32)
 ext 026F826C void hdPartsKeyIcon_c::hide()
 ext 026F8228 void hdPartsKeyIcon_c::requestIn()
 ext 026F8238 void hdPartsKeyIcon_c::requestDecide()
 ext 026F8260 void hdPartsKeyIcon_c::requestOut()
 ext 026F82CC u32 hdPartsKeyIcon_c::isWaiting()
 ext 026F83B4 u32 hdPartsKeyIcon_c::isHidden()
 ext 026F833C u8 hdPartsKeyIcon_c::isNotDeciding()
 ext 026F90D8 void hdParts_c::attachArchives(hdUiSafeString*,u32)
 ext 026F91F8 void hdParts_c::detachArchives()
 ext 026F92C0 u32 hdParts_c::createChild(u32,hdPartsDesc_c*)
*/

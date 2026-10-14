/**
 * hd_msg_choice_control.cpp
 * HD - the talk window with choices: loading, input routing, drawing on the TV or the GamePad,
 * the sounds and the start of a message
 *
 * WWHD (cking.rpx), HD-only code (no GameCube source), verified function by function against the
 * recompiled original (tools/verify, unit hd_msg_msg_window_main). Each function is marked with its WWHD
 * address; the glue block at the end lists this translation unit's data (tools/verify/autoglue.py). Layout,
 * pane, animation, sound label and state names are HD strings; all other names are ours.
 */

#include "d/hd_msg_choice_window.h"
#include "d/hd_msg_window_a.h"
#include "d/hd_input_ctrl.h"
using namespace gabi;
struct hdTalkControlCore_c {
    u8 mReserved[0x14]; PaneWork_l mWork;
    u8 mReserved930[0x1C]; be<u32> mMode;
    u8 mReserved950[0x10]; gptr<hdMsgChoiceWindow_c> mpWindow;
};
struct hdTalkControlCorePointer_c { gptr<hdTalkControlCore_c> mpCore; };
static hdTalkControlCorePointer_c sTalkControlCore;
static hdUiSafeString sTalkControlArchiveName;
static hdUiSafeString sTalkControlLayoutName;
static hdPartsStateId_c sTalkControlBlankState;
static hdPartsStateId_c sTalkControlCloseState;
static hdPartsStateId_c sTalkControlClosedState;
extern hdPartsStateId_c* talkControlCurrentState(hdStateMachine*);
extern void talkControlUpdateState(hdStateMachine*);
extern u32 talkControlUpdateCursor(hdTalkControlCore_c*, u32, u32, hdTextPane_c*, u32);
// The expected state's vtable is snapshotted before the current-key callback.
static bool talkControlIsState(hdMsgChoiceWindow_c* object, hdPartsStateId_c* expected) {
    hdPartsStateId_c* current = talkControlCurrentState(object->stateMachine());
    u32 currentGetter = current->mVtable->mGetKey.get();
    hdPartsStateIdVtbl_c* expectedVtable = expected->mVtable;
    u32 currentKey = call_ptr<u32>(currentGetter, current);
    u32 expectedKey = call_ptr<u32>(expectedVtable->mGetKey.get(), expected);
    return currentKey == expectedKey;
}

/* 026ADBD4 */
void hdMsgChoiceWindow_c::clearSounds() {
    mSecondChoiceSound = 0;
    mFirstChoiceSound = 0;
}

/* 026ADBE4 */
u32 hdMsgChoiceWindow_c::initialize(u32 first, u32 second, void* descriptor) {
    sTalkControlCore.mpCore->mpWindow = this;
    attachArchives(&sTalkControlArchiveName, 1);
    hdPartsResAccessor_c* accessor = mpResAccessor;
    if (!call_ptr<u32>(mVtable->mLoad.get(), this, &sTalkControlLayoutName, accessor, first, second, descriptor))
        return 0;
    findWindowPanes();
    findTextPanes();
    mpMessage = &sTalkControlCore.mpCore->mWork;
    hide();
    startState(&sTalkControlBlankState);
    clearSounds();
    return 1;
}

/* 026ADCE0 */
void hdMsgChoiceWindow_c::release() {
    sTalkControlCore.mpCore->mpWindow = nullptr;
    hdMessageGetPlay()->mAdvanceCount = 0;
    vDestroyLayout();
    detachArchives();
}

/* 026ADD44 */
void hdMsgChoiceWindow_c::updateCursor() {
    u32 active = mChoiceActive;
    if (active) {
        u32 closing = mClosing;
        if (!closing) {
            if (!talkControlUpdateCursor(sTalkControlCore.mpCore, 0, mSelection, mpChoiceRoot, mOnTv)) return;
            mChoiceActive = mClosing;
            mSelectedChoice = mSelection;
        } else {
            u32 selection = mSelection;
            if (mSelectedChoice == selection) return;
            if (!talkControlUpdateCursor(sTalkControlCore.mpCore, active, selection, mpChoiceRoot, mOnTv)) return;
            mSelectedChoice = mSelection;
        }
    } else {
        u32 closing = mClosing;
        if (!closing) return;
        if (!talkControlUpdateCursor(sTalkControlCore.mpCore, closing, mSelection, mpChoiceRoot, mOnTv)) return;
        mChoiceActive = mClosing;
        mSelectedChoice = mSelection;
    }
}

/* 026ADE10 */
void hdMsgChoiceWindow_c::execute() {
    hdTalkControlCore_c* core = sTalkControlCore.mpCore;
    u32 requestedClose = mChoiceChanged;
    mChoiceChanged = 0;
    if (core->mMode != 5) return;
    if (talkControlIsState(this, &sTalkControlBlankState)) return;
    if (!talkControlIsState(this, &sTalkControlCloseState) &&
        !talkControlIsState(this, &sTalkControlClosedState))
        hdMessageGetPlay()->mAdvanceCount = 0;
    if (requestedClose && !talkControlIsState(this, &sTalkControlCloseState) &&
        !talkControlIsState(this, &sTalkControlClosedState))
        changeState(&sTalkControlCloseState);
    hdInputManager* manager = hdInputGetManager();
    if (manager) {
        hdPointerController* controller = manager->mInput;
        if (controller) {
            mFlags = controller->mRepeat;
            mInputFlags = controller->mHold;
            u32 held = controller->mHold;
            u32 released = controller->mRelease;
            mInputMode = held | released;
        }
    }
    talkControlUpdateState(stateMachine());
    vCalcLayout();
    updateCursor();
}

/* 026AE044 */
void hdMsgChoiceWindow_c::drawTv(void* info) {
    if (mOnTv && !talkControlIsState(this, &sTalkControlBlankState)) vDrawLayout(info);
}

/* 026AE0EC */
void hdMsgChoiceWindow_c::drawGamePad(void* info) {
    if (!mOnTv && !talkControlIsState(this, &sTalkControlBlankState)) vDrawLayout(info);
}

/* 026AE410 */
void hdMsgChoiceWindow_c::setSounds(u32 secondChoice, u32 firstChoice) {
    mSecondChoiceSound = secondChoice;
    mFirstChoiceSound = firstChoice;
}

/* 026AE41C */
void hdMsgChoiceWindow_c::requestClose() { mChoiceChanged = 1; }
struct hdTalkMessageName_c { hdUiSafeString mString; be<u32> mCapacity; be<u8> mBuffer[32]; };
struct hdTalkMessageGroup_c { gptr<void> mpAttributes; be<u32> mCount; };
struct hdTalkMessageLookup_c { gptr<void> mpLookup; };
static hdTalkMessageLookup_c sTalkMessageLookup;
static hdPartsStateId_c sTalkControlOpenState;
static hdPartsStateId_c sTalkControlOpenNewState;
extern void talkControlFormatName(hdTalkMessageName_c*, const char*, u32);
extern hdTalkMessageGroup_c* talkControlFindMessage(void*, hdTalkMessageName_c*);
extern u32 talkControlGetMessageAttribute(void*, u32);
static hdUiStringTable sTalkMessageNameVtable;

/* 026AE194 */
u32 hdMsgChoiceWindow_c::startMessage(u32 index, u32 id, u32 choice, u32 allowCancel) {
    if (talkControlIsState(this, &sTalkControlOpenState)) return 0;
    hdTalkMessageName_c name;
    name.mBuffer[31] = 0;
    name.mString.mText = name.mBuffer;
    name.mBuffer[0] = 0;
    name.mCapacity = 32;
    name.mString.mVtable = &sTalkMessageNameVtable;
    talkControlFormatName(&name, "%05d", u16(id));
    hdTalkMessageGroup_c* group = talkControlFindMessage(sTalkMessageLookup.mpLookup, &name);
    if (!group || index >= group->mCount) return 0;
    if (!talkControlGetMessageAttribute(group->mpAttributes, index)) return 0;
    mMessageId = id;
    mOnTv = 1;
    mChoiceMode = choice == 1 ? 1 : 0;
    mCancelAllowed = allowCancel;
    u32 mode = mChoiceMode;
    mpPanes[mode < 4 ? mode : 0]->mShrink = 0;
    mode = mChoiceMode;
    PaneWork_l* work = mpMessage;
    hdTextPane_c* pane = mpPanes[mode < 4 ? mode : 0];
    reinterpret_cast<hdTalkChoiceWidth_c*>(work)->mWidthBits = reinterpret_cast<hdTalkPaneWidth_c*>(pane)->mWidthBits.get();
    clearSounds();
    changeState(&sTalkControlOpenState);
    return 1;
}

/* 026AE330 */
void hdMsgChoiceWindow_c::showCustomMessage(u32 choice, u32 useSound) {
    if (talkControlIsState(this, &sTalkControlOpenState)) return;
    mOnTv = useSound;
    mMessageId = 0xFFFFFFFF;
    mChoiceMode = choice == 1 ? 1 : 0;
    mpPanes[choice == 1 ? 1 : 0]->mShrink = 0;
    u32 mode = mChoiceMode;
    PaneWork_l* work = mpMessage;
    hdTextPane_c* pane = mpPanes[mode < 4 ? mode : 0];
    reinterpret_cast<hdTalkChoiceWidth_c*>(work)->mWidthBits = reinterpret_cast<hdTalkPaneWidth_c*>(pane)->mWidthBits.get();
    clearSounds();
    changeState(&sTalkControlOpenNewState);
}
struct hdTalkLayoutSystem_c { gptr<hdScreenUiSystem> mpSystem; };
struct hdTalkAnimationGroupNames_c { hdUiSafeString mNames[2]; };
struct hdTalkAnimationNames_c { hdUiSafeString mNames[3]; };
struct hdTalkAnimationSlots_c { be<u32> mSlots[3]; };
static hdTalkLayoutSystem_c sTalkLayoutSystem;
static hdTalkAnimationGroupNames_c sTalkAnimationGroupNames;
static hdTalkAnimationNames_c sTalkAnimationNames;
static hdTalkAnimationSlots_c sTalkAnimationSlots;
extern void* talkControlAllocateLayout(u32, hdScreenHeap*, s32);
extern void talkControlBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

/* 026AE428 */
u32 hdMsgChoiceWindow_c::loadLayout(void* name, void* resource, u32 first, u32 second, void* descriptor) {
    hdScreenHeap* heap = sTalkLayoutSystem.mpSystem->mHeap;
    hdPartsLayout_c* layout = static_cast<hdPartsLayout_c*>(talkControlAllocateLayout(sizeof(hdPartsLayout_c), heap, 4));
    if (layout) layout = hdPartsLayout_c::construct(layout);
    mpLayout = layout;
    if (!layout || !layout->vLoad(name, ea(resource), first, second, 3, 2, descriptor)) return 0;
    for (u32 animation = 0; animation < 3; ++animation) {
        hdAnimSet* animations = animSet();
        u32 slot = sTalkAnimationSlots.mSlots[animation];
        talkControlBindAnimation(animations, animation, &sTalkAnimationNames.mNames[animation],
                                 &sTalkAnimationGroupNames.mNames[slot]);
    }
    vCalcLayout();
    return 1;
}
struct hdTalkStaticPointers_c { gptr<void> mPointers[4]; };
struct hdTalkStaticRegistration_c { be<u32> mWords[3]; };
static hdTalkStaticPointers_c sTalkStaticPointers;
static hdTalkStaticRegistration_c sTalkPointerRegistration;
static hdTalkStaticRegistration_c sTalkFirstFlagRegistration;
static hdTalkStaticRegistration_c sTalkSecondFlagRegistration;
static f32 sTalkMinimumAngle;
static f32 sTalkMaximumAngle;
static f32 sTalkMinimumAngleConstant;
static f32 sTalkMaximumAngleConstant;
static u8 sTalkFirstFlag;
static u8 sTalkSecondFlag;
static u32 sTalkStateCounterInitialized;
static u32 sTalkStateCounter;
static hdPartsStateId_c sTalkControlSelectState;
static hdPartsStateId_c sTalkControlCloseWaitState;
extern void talkControlRegisterStatic(hdTalkStaticRegistration_c*);
extern void talkControlConstructFirstFlag(be<u8>*);
extern void talkControlConstructSecondFlag(be<u8>*);
static hdUiStringTable sTalkControlStringVtable;
static hdPartsStateIdVtbl_c sTalkControlStateVtable;
static hdStateId sTalkControlNullState;

/* 026AE554 */
static void initializeTalkChoiceStatics() {
    for (s32 index = 3; index >= 0; --index) sTalkStaticPointers.mPointers[index] = nullptr;
    talkControlRegisterStatic(&sTalkPointerRegistration);
    sTalkMinimumAngle = sTalkMinimumAngleConstant;
    sTalkMaximumAngle = sTalkMaximumAngleConstant;
    talkControlConstructFirstFlag(reinterpret_cast<be<u8>*>(&sTalkFirstFlag));
    talkControlRegisterStatic(&sTalkFirstFlagRegistration);
    talkControlConstructSecondFlag(reinterpret_cast<be<u8>*>(&sTalkSecondFlag));
    talkControlRegisterStatic(&sTalkSecondFlagRegistration);
    sTalkAnimationGroupNames.mNames[0].mVtable = &sTalkControlStringVtable;
    sTalkAnimationGroupNames.mNames[0].mText.v = guest_str("G_All_00");
    sTalkAnimationNames.mNames[0].mVtable = &sTalkControlStringVtable;
    sTalkAnimationNames.mNames[0].mText.v = guest_str("InAll");
    u32 initialized = sTalkStateCounterInitialized;
    sTalkAnimationGroupNames.mNames[1].mVtable = &sTalkControlStringVtable;
    sTalkAnimationNames.mNames[1].mVtable = &sTalkControlStringVtable;
    sTalkAnimationNames.mNames[1].mText.v = guest_str("ChangeText");
    sTalkAnimationNames.mNames[2].mVtable = &sTalkControlStringVtable;
    sTalkAnimationNames.mNames[2].mText.v = guest_str("OutAll");
    sTalkAnimationGroupNames.mNames[1].mText.v = guest_str("G_Msg_00");
    sTalkControlArchiveName.mVtable = &sTalkControlStringVtable;
    sTalkControlArchiveName.mText.v = guest_str("MsgOnly_00");
    sTalkControlLayoutName.mVtable = &sTalkControlStringVtable;
    sTalkControlLayoutName.mText.v = guest_str("MsgOnly_00");
    u32 counter;
    if (initialized) counter = sTalkStateCounter;
    else { sTalkStateCounterInitialized = 1; counter = 0; }
    auto* vtable = &sTalkControlStateVtable;
    auto* nullState = &sTalkControlNullState;
    sTalkControlOpenState.setup(s32(++counter), "::StateID_Open", vtable, 0x19, nullState);
    sTalkControlOpenNewState.setup(s32(++counter), "::StateID_OpenNewMsg", vtable, 0x1C, nullState);
    sTalkControlSelectState.setup(s32(++counter), "::StateID_Select", vtable, 0x1F, nullState);
    sTalkControlCloseWaitState.setup(s32(++counter), "::StateID_CloseWait", vtable, 0x22, nullState);
    sTalkControlCloseState.setup(s32(++counter), "::StateID_Close", vtable, 0x25, nullState);
    sTalkStateCounter = ++counter;
    sTalkControlClosedState.setup(s32(counter), "::StateID_Delete", vtable, 0x28, nullState);
}

/* WWHD glue
 data sTalkMessageNameVtable 100FCAD4
 data sTalkControlStringVtable 100FCA8C
 data sTalkControlStateVtable 100FCB70
 data sTalkControlNullState 101FF32C
 data sTalkStaticPointers 10497CE8
 data sTalkPointerRegistration 101F6510
 data sTalkFirstFlagRegistration 101F651C
 data sTalkSecondFlagRegistration 101F6528
 data sTalkMinimumAngle 10497CAC
 data sTalkMaximumAngle 10497CB0
 data sTalkMinimumAngleConstant 100FCCD4
 data sTalkMaximumAngleConstant 100FCCD8
 data sTalkFirstFlag 10497CB4
 data sTalkSecondFlag 10497CB5
 data sTalkStateCounterInitialized 101FD8E4
 data sTalkStateCounter 101FDD50
 data sTalkControlSelectState 10497DA0
 data sTalkControlCloseWaitState 10497DD0
 str 100FCD10 "G_All_00"
 str 100FCCDC "InAll"
 str 100FCD28 "ChangeText"
 str 100FCCE4 "OutAll"
 str 100FCD1C "G_Msg_00"
 str 100FCD34 "MsgOnly_00"
 str 100FCD40 "::StateID_Open"
 str 100FCD50 "::StateID_OpenNewMsg"
 str 100FCD68 "::StateID_Select"
 str 100FCCFC "::StateID_CloseWait"
 str 100FCCEC "::StateID_Close"
 str 100FCD7C "::StateID_Delete"
 ext 028F026C void talkControlRegisterStatic(hdTalkStaticRegistration_c*)
 ext 028ED6F8 void talkControlConstructFirstFlag(be<u8>*)
 ext 028EAB2C void talkControlConstructSecondFlag(be<u8>*)
 data sTalkLayoutSystem 1018C404
 data sTalkAnimationGroupNames 10497CF8
 data sTalkAnimationNames 10497D08
 data sTalkAnimationSlots 100FCB1C
 ext 0273B050 void* talkControlAllocateLayout(u32,hdScreenHeap*,s32)
 ext 02004E04 void talkControlBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 data sTalkMessageLookup 101F4AE8
 data sTalkControlOpenState 10497D40
 data sTalkControlOpenNewState 10497D70
 str 100FCCCC "%05d"
 ext 02759C28 void talkControlFormatName(hdTalkMessageName_c*,const char*,u32)
 ext 025F4FC0 hdTalkMessageGroup_c* talkControlFindMessage(void*,hdTalkMessageName_c*)
 ext 0273A574 u32 talkControlGetMessageAttribute(void*,u32)
 data sTalkControlCore 101F4B5C
 data sTalkControlArchiveName 10497CA4
 data sTalkControlLayoutName 10497E60
 data sTalkControlBlankState 1049DD60
 data sTalkControlCloseState 10497E00
 data sTalkControlClosedState 10497E30
 ext 02006478 hdPartsStateId_c* talkControlCurrentState(hdStateMachine*)
 ext 02006364 void talkControlUpdateState(hdStateMachine*)
 ext 025F85C0 u32 talkControlUpdateCursor(hdTalkControlCore_c*,u32,u32,hdTextPane_c*,u32)
*/

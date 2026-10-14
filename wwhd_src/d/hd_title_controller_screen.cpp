/* HD SelectController_00 screen; private binary-derived source. */
#include "d/hd_title_parts.h"

static void* sControllerScreenPromptManager;
static hdScreenUiSystem* sControllerScreenUiSystem;
struct hdTitleControllerAnimationGroups_l { be<u32> mIndices[2]; };
static hdTitleControllerAnimationGroups_l sControllerAnimationGroups;
static hdUiSafeString sControllerAnimationNames;
static hdUiSafeString sControllerGroupNames;
static hdUiSafeString sControllerChildNames;
struct hdTitleControllerDescriptor_l {
    u8 _padding[0x14];
    hdUiSafeString mName;
};
WWHD_OFFSET(hdTitleControllerDescriptor_l, mName, 0x14);
extern void titleScreenClearPrompt(void*, u32);
extern void titleFinalizeControllerScreen(hdParts_c*);
extern void titleUpdateControllerScreenState(hdStateMachine*);
extern void* titleControllerScreenAllocateLayout(u32, hdScreenHeap*, u32);
extern hdPartsLayout_c* titleControllerScreenConstructLayout(hdPartsLayout_c*);
extern void titleControllerScreenBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);
extern u32 titleControllerScreenCreateChild(hdParts_c*, u32, hdPartsDesc_c*);

/* 026D4D68 */
void hdTitleControllerScreen_c::connectControllerReceivers() {
    u32 count = mChildren.mNum;
    hdParts_c* first = *mChildren.slot(0);
    auto* selected = reinterpret_cast<hdButtonPart_c*>(
        (*mChildren.slot(count > 1 ? 1 : 0)).get());
    reinterpret_cast<hdButtonPart_c*>(first)->setReceiverButtons(0, 0, 0, 0, 0, 0, 0, selected->mpReceiver.v);

    count = mChildren.mNum;
    hdParts_c* second = *mChildren.slot(count > 1 ? 1 : 0);
    auto* primary = reinterpret_cast<hdButtonPart_c*>((*mChildren.slot(0)).get());
    reinterpret_cast<hdButtonPart_c*>(second)->setReceiverButtons(0, 0, 0, 0, 0, 0, primary->mpReceiver.v, 0);
}

struct hdTitleControllerSoundButtons_l { be<u32> mIndices[2]; };
static hdTitleControllerSoundButtons_l sControllerSoundButtons;
static hdUiSafeString sControllerSilentSound;
static hdUiStringTable sControllerSoundStringTable;

/* 026D4E28 */
void hdTitleControllerScreen_c::bindControllerSounds() {
    for (u32 index = 0; index < 2; ++index) {
        u32 buttonIndex = sControllerSoundButtons.mIndices[index];
        hdParts_c* button = *mChildren.safeSlot(buttonIndex);
        if (button != NULL) {
            gabi::Local<hdUiSafeString> sound;
            sound->mVtable = &sControllerSoundStringTable;
            sound->mText.v = gabi::guest_addr("SE_DRC_TOUCH_ON");
            reinterpret_cast<hdButtonPart_c*>(button)->setTouchSound(sound.get(), 0x830);
            reinterpret_cast<hdButtonPart_c*>(button)->setCursorSound(&sControllerSilentSound, 0x830);
            reinterpret_cast<hdBasicButton_c*>(button)->setSound(&sControllerSilentSound, 0x831);
        }
    }
}

struct hdTitleControllerConnectionStatus_l {
    u8 _padding[0x1C];
    be<u8> mPrimaryConnected;
};
WWHD_OFFSET(hdTitleControllerConnectionStatus_l, mPrimaryConnected, 0x1C);
static hdTitleControllerConnectionStatus_l* sControllerConnectionStatus;
static hdUiSafeString sControllerScreenArchives;
static hdUiSafeString sControllerScreenLayoutName;
static hdPartsStateId_c sControllerScreenInitialState;
extern void titleControllerScreenAttachArchives(hdParts_c*, hdUiSafeString*, u32);
extern void titleControllerScreenFindRoot(hdParts_c*);
extern void titleControllerScreenFindPane(hdParts_c*);
extern void titleControllerScreenConnectReceivers(hdParts_c*);
extern void titleControllerScreenBindSounds(hdParts_c*);
extern void titleControllerScreenSetAnimationFrame(hdAnimSet*, u32, u32);

/* 026D4EE0 */
u32 hdTitleControllerScreen_c::initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc) {
    titleControllerScreenAttachArchives(this, &sControllerScreenArchives, 2);
    if (vLoad(&sControllerScreenLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc) == 0)
        return 0;
    if (gabi::call_ptr<u32>(mVtable->mCreateChildren.get(), this) == 0) return 0;
    titleControllerScreenFindRoot(this);
    titleControllerScreenFindPane(this);
    titleControllerScreenConnectReceivers(this);
    u32 selected = u32(sControllerConnectionStatus->mPrimaryConnected) ^ 1;
    hdPartsLayout_c* layout = mpLayout;
    mSelectedController = selected;
    titleControllerScreenSetAnimationFrame(layout->animSet(), 0, 0);
    vCalcLayout();
    titleControllerScreenBindSounds(this);
    titleControllerScreenSetAnimationFrame(animSet(), 0, 0);
    startState(&sControllerScreenInitialState);
    return 1;
}

/* 026D5054 */
void hdTitleControllerScreen_c::finalizeParts() {
    titleScreenClearPrompt(sControllerScreenPromptManager, 0);
    vDeleteChildren();
    vDestroyLayout();
    titleFinalizeControllerScreen(this);
}

/* 026D50BC */
void hdTitleControllerScreen_c::updateState() {
    titleUpdateControllerScreenState(stateMachine());
}

/* 026D50C4 */
void hdTitleControllerScreen_c::draw(void* info) {
    vDrawLayout(info);
    vDrawChildren(info);
}

/* 026D5120 */
u32 hdTitleControllerScreen_c::isPrimarySelected() {
    return mSelectedController == 0;
}

/* 026D5130 */
u32 hdTitleControllerScreen_c::createNamedChild(hdPartsDesc_c* desc) {
    auto* name = &reinterpret_cast<hdTitleControllerDescriptor_l*>(desc)->mName;
    for (u32 index = 0; index < 2; ++index) {
        name->assureTermination();
        name->assureTermination();
        auto* expected = hdPartsView<hdUiSafeString>(gabi::ea(&sControllerChildNames) + index * sizeof(hdUiSafeString));
        be<u8>* originalText = name->mText;
        expected->assureTermination();
        bool matches = originalText == expected->mText.get();
        if (!matches) {
            u32 actualAddress = gabi::ea(name->mText.get());
            u32 expectedAddress = gabi::ea(expected->mText.get());
            for (u32 offset = 0; offset <= 0x40000; ++offset) {
                u8 actual = *hdPartsView<be<u8>>(actualAddress + offset);
                u8 wanted = *hdPartsView<be<u8>>(expectedAddress + offset);
                if (actual != wanted) break;
                if (actual == 0) {
                    matches = true;
                    break;
                }
            }
        }
        if (matches) return titleControllerScreenCreateChild(this, index, desc);
    }
    return 1;
}

/* 026D5238 */
u32 hdTitleControllerScreen_c::loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    auto* layout = static_cast<hdPartsLayout_c*>(titleControllerScreenAllocateLayout(sizeof(hdPartsLayout_c), sControllerScreenUiSystem->mHeap, 4));
    if (layout != NULL) layout = titleControllerScreenConstructLayout(layout);
    mpLayout = layout;
    if (layout == NULL || layout->vLoad(name, arg1, arg2, arg3, 2, 1, desc) == 0) return 0;
    for (u32 i = 0; i < 2; ++i) {
        hdPartsLayout_c* currentLayout = mpLayout;
        u32 group = sControllerAnimationGroups.mIndices[i];
        hdAnimSet* animations = currentLayout->animSet();
        auto* animationName = hdPartsView<hdUiSafeString>(gabi::ea(&sControllerAnimationNames) + i * sizeof(hdUiSafeString));
        auto* groupName = hdPartsView<hdUiSafeString>(gabi::ea(&sControllerGroupNames) + group * sizeof(hdUiSafeString));
        titleControllerScreenBindAnimation(animations, i, animationName, groupName);
    }
    vCalcLayout();
    return 1;
}

struct hdTitleControllerGlobalList_l { be<u32> mHead, mTail, mOwner, mCount; };
struct hdTitleControllerFloat_l { be<f32> mValue; };
static hdTitleControllerGlobalList_l sControllerGlobalList;
static u8 sControllerGlobalRegistration;
static hdTitleControllerFloat_l sControllerMinimumAngle;
static hdTitleControllerFloat_l sControllerMaximumAngle;
static hdTitleControllerFloat_l sControllerMinimumValue;
static hdTitleControllerFloat_l sControllerMaximumValue;
static u8 sControllerPrimaryFlag;
static u8 sControllerSecondaryFlag;
static u8 sControllerPrimaryRegistration;
static u8 sControllerSecondaryRegistration;
static hdUiStringTable sControllerGlobalStringTable;
static hdUiSafeString sControllerResourceNames;
static hdPartsStateCounter_c sControllerCounterGuard;
static hdPartsStateCounter_c sControllerStateCounter;
static hdPartsStateId_c sControllerIntroDescriptor;
static hdPartsStateId_c sControllerInDescriptor;
static hdPartsStateId_c sControllerOutDescriptor;
static hdPartsStateId_c sControllerPrimaryMissingDescriptor;
static hdPartsStateId_c sControllerSecondaryMissingDescriptor;
static hdPartsStateIdVtbl_c sControllerDirectStateTable;
static hdPartsStateIdVtbl_c sControllerVirtualStateTable;
static hdStateId sControllerNullState;
static hdStateId sControllerDisplayParent;
extern void titleControllerRegisterGlobals(void*);
extern void titleControllerConstructPrimaryFlag(void*);
extern void titleControllerConstructSecondaryFlag(void*);

enum hdTitleControllerStateDelegate {
    CONTROLLER_RESET_BUTTONS = 0x026D4260,
    CONTROLLER_CALCULATE_BUTTONS = 0x026D42F4,
    CONTROLLER_WAIT_EXIT = 0x026D57A4,
    CONTROLLER_WAIT_UNUSED = 0x026D57A8,
    CONTROLLER_SAVE_PRIMARY = 0x026D48A8,
    CONTROLLER_POLL_PRIMARY = 0x026D48F8,
    CONTROLLER_RESTORE_PRIMARY = 0x026D4988,
    CONTROLLER_PRIMARY_UNUSED = 0x026D57BC,
    CONTROLLER_SAVE_SECONDARY = 0x026D49D8,
    CONTROLLER_POLL_SECONDARY = 0x026D4A28,
    CONTROLLER_RESTORE_SECONDARY = 0x026D4AB8,
    CONTROLLER_SECONDARY_UNUSED = 0x026D57C4,
};

struct hdTitleControllerGroupNamesNames_l { hdUiSafeString mNames[1]; };
static hdTitleControllerGroupNamesNames_l sControllerGroupNamesInitializer;
struct hdTitleControllerAnimationNamesNames_l { hdUiSafeString mNames[2]; };
static hdTitleControllerAnimationNamesNames_l sControllerAnimationNamesInitializer;
struct hdTitleControllerChildNamesNames_l { hdUiSafeString mNames[2]; };
static hdTitleControllerChildNamesNames_l sControllerChildNamesInitializer;
struct hdTitleControllerScreenArchivesNames_l { hdUiSafeString mNames[2]; };
static hdTitleControllerScreenArchivesNames_l sControllerScreenArchivesInitializer;
struct hdTitleControllerResourceNamesNames_l { hdUiSafeString mNames[2]; };
static hdTitleControllerResourceNamesNames_l sControllerResourceNamesInitializer;
struct hdTitleControllerScreenLayoutNameNames_l { hdUiSafeString mNames[1]; };
static hdTitleControllerScreenLayoutNameNames_l sControllerScreenLayoutNameInitializer;

/* 026D5364 */
static void initializeTitleControllerGlobals() {
    sControllerGlobalList.mOwner = 0;
    sControllerGlobalList.mHead = 0;
    sControllerGlobalList.mCount = 0;
    sControllerGlobalList.mTail = 0;
    titleControllerRegisterGlobals(&sControllerGlobalRegistration);
    f32 maximum = sControllerMaximumValue.mValue;
    f32 minimum = sControllerMinimumValue.mValue;
    sControllerMaximumAngle.mValue = maximum;
    sControllerMinimumAngle.mValue = minimum;
    titleControllerConstructPrimaryFlag(&sControllerPrimaryFlag);
    titleControllerRegisterGlobals(&sControllerPrimaryRegistration);
    titleControllerConstructSecondaryFlag(&sControllerSecondaryFlag);
    titleControllerRegisterGlobals(&sControllerSecondaryRegistration);
    {
        auto* name = &sControllerGroupNamesInitializer.mNames[0];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("G_All_00");
    }
    {
        auto* name = &sControllerAnimationNamesInitializer.mNames[0];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("InAll");
    }
    {
        auto* name = &sControllerAnimationNamesInitializer.mNames[1];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("OutAll");
    }
    {
        auto* name = &sControllerChildNamesInitializer.mNames[0];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("L_Btn_00");
    }
    {
        auto* name = &sControllerChildNamesInitializer.mNames[1];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("L_Btn_01");
    }
    {
        auto* name = &sControllerScreenArchivesInitializer.mNames[0];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("SelectController_00");
    }
    {
        auto* name = &sControllerScreenArchivesInitializer.mNames[1];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("BtnController_00");
    }
    {
        auto* name = &sControllerResourceNamesInitializer.mNames[0];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("BtnController_00");
    }
    {
        auto* name = &sControllerResourceNamesInitializer.mNames[1];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("BtnController_00");
    }
    {
        auto* name = &sControllerScreenLayoutNameInitializer.mNames[0];
        name->mVtable = &sControllerGlobalStringTable;
        name->mText.v = gabi::guest_addr("SelectController_00");
    }
    u32 number = 0;
    if (sControllerCounterGuard.mValue != 0) number = sControllerStateCounter.mValue;
    else sControllerCounterGuard.mValue = 1;
    sControllerScreenInitialState.mNumber = ++number;
    sControllerScreenInitialState.mName.v = gabi::guest_addr("::StateID_Wait");
    sControllerScreenInitialState.mVtable = &sControllerDirectStateTable;
    sControllerScreenInitialState.mEnter.mThisDelta = 0;
    sControllerScreenInitialState.mEnter.mVirtualIndex = -1;
    sControllerScreenInitialState.mEnter.mFunction = CONTROLLER_RESET_BUTTONS;
    sControllerScreenInitialState.mExecute.mThisDelta = 0;
    sControllerScreenInitialState.mExecute.mVirtualIndex = -1;
    sControllerScreenInitialState.mExecute.mFunction = CONTROLLER_CALCULATE_BUTTONS;
    sControllerScreenInitialState.mExit.mThisDelta = 0;
    sControllerScreenInitialState.mExit.mVirtualIndex = -1;
    sControllerScreenInitialState.mExit.mFunction = CONTROLLER_WAIT_EXIT;
    sControllerScreenInitialState.mUnused.mThisDelta = 0;
    sControllerScreenInitialState.mUnused.mVirtualIndex = -1;
    sControllerScreenInitialState.mUnused.mFunction = CONTROLLER_WAIT_UNUSED;
    sControllerIntroDescriptor.setup(++number, "::StateID_Display", &sControllerVirtualStateTable, 7, &sControllerDisplayParent);
    sControllerInDescriptor.setup(++number, "::StateID_InAll", &sControllerVirtualStateTable, 25, &sControllerNullState);
    sControllerOutDescriptor.setup(++number, "::StateID_OutAll", &sControllerVirtualStateTable, 28, &sControllerNullState);
    sControllerPrimaryMissingDescriptor.mNumber = ++number;
    sControllerPrimaryMissingDescriptor.mName.v = gabi::guest_addr("::StateID_WaitConnectDRC");
    sControllerPrimaryMissingDescriptor.mVtable = &sControllerDirectStateTable;
    sControllerPrimaryMissingDescriptor.mEnter.mThisDelta = 0;
    sControllerPrimaryMissingDescriptor.mEnter.mVirtualIndex = -1;
    sControllerPrimaryMissingDescriptor.mEnter.mFunction = CONTROLLER_SAVE_PRIMARY;
    sControllerPrimaryMissingDescriptor.mExecute.mThisDelta = 0;
    sControllerPrimaryMissingDescriptor.mExecute.mVirtualIndex = -1;
    sControllerPrimaryMissingDescriptor.mExecute.mFunction = CONTROLLER_POLL_PRIMARY;
    sControllerPrimaryMissingDescriptor.mExit.mThisDelta = 0;
    sControllerPrimaryMissingDescriptor.mExit.mVirtualIndex = -1;
    sControllerPrimaryMissingDescriptor.mExit.mFunction = CONTROLLER_RESTORE_PRIMARY;
    sControllerPrimaryMissingDescriptor.mUnused.mThisDelta = 0;
    sControllerPrimaryMissingDescriptor.mUnused.mVirtualIndex = -1;
    sControllerPrimaryMissingDescriptor.mUnused.mFunction = CONTROLLER_PRIMARY_UNUSED;
    sControllerSecondaryMissingDescriptor.mNumber = ++number;
    sControllerSecondaryMissingDescriptor.mName.v = gabi::guest_addr("::StateID_WaitConnectProCon");
    sControllerSecondaryMissingDescriptor.mVtable = &sControllerDirectStateTable;
    sControllerSecondaryMissingDescriptor.mEnter.mThisDelta = 0;
    sControllerSecondaryMissingDescriptor.mEnter.mVirtualIndex = -1;
    sControllerSecondaryMissingDescriptor.mEnter.mFunction = CONTROLLER_SAVE_SECONDARY;
    sControllerSecondaryMissingDescriptor.mExecute.mThisDelta = 0;
    sControllerSecondaryMissingDescriptor.mExecute.mVirtualIndex = -1;
    sControllerSecondaryMissingDescriptor.mExecute.mFunction = CONTROLLER_POLL_SECONDARY;
    sControllerSecondaryMissingDescriptor.mExit.mThisDelta = 0;
    sControllerSecondaryMissingDescriptor.mExit.mVirtualIndex = -1;
    sControllerSecondaryMissingDescriptor.mExit.mFunction = CONTROLLER_RESTORE_SECONDARY;
    sControllerSecondaryMissingDescriptor.mUnused.mThisDelta = 0;
    sControllerSecondaryMissingDescriptor.mUnused.mVirtualIndex = -1;
    sControllerSecondaryMissingDescriptor.mUnused.mFunction = CONTROLLER_SECONDARY_UNUSED;
    sControllerStateCounter.mValue = number;
}

/* WWHD glue (tools/verify/autoglue.py): controller screen
 data sControllerScreenPromptManager 101F4FF0
 data sControllerScreenUiSystem 1018C404
 data sControllerAnimationGroups 10102B78
 data sControllerAnimationNames 1049A9FC
 data sControllerGroupNames 1049A9C8
 data sControllerChildNames 1049AA0C
 ext 02615154 void titleScreenClearPrompt(void*,u32)
 ext 026F91F8 void titleFinalizeControllerScreen(hdParts_c*)
 ext 02006364 void titleUpdateControllerScreenState(hdStateMachine*)
 ext 0273B050 void* titleControllerScreenAllocateLayout(u32,hdScreenHeap*,u32)
 ext 026FADB0 hdPartsLayout_c* titleControllerScreenConstructLayout(hdPartsLayout_c*)
 ext 02004E04 void titleControllerScreenBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 ext 026F92C0 u32 titleControllerScreenCreateChild(hdParts_c*,u32,hdPartsDesc_c*)
 ext 026F5C08 void hdButtonPart_c::setReceiverButtons(u32,u32,u32,u32,u32,u32,u32,u32)
 data sControllerSoundButtons 10102B80
 data sControllerSilentSound 104A0CD8
 data sControllerSoundStringTable 10102B94
 str 10102CE8
 ext 026F6524 void hdButtonPart_c::setTouchSound(hdUiSafeString*,u32)
 ext 026F6534 void hdButtonPart_c::setCursorSound(hdUiSafeString*,u32)
 ext 026F2540 void hdBasicButton_c::setSound(hdUiSafeString*,u32)
 data sControllerConnectionStatus 101F8378
 data sControllerScreenArchives 1049AA2C
 data sControllerScreenLayoutName 1049AAD4
 data sControllerScreenInitialState 1049AADC
 ext 026F90D8 void titleControllerScreenAttachArchives(hdParts_c*,hdUiSafeString*,u32)
 ext 026D4BA4 void titleControllerScreenFindRoot(hdParts_c*)
 ext 026D4C68 void titleControllerScreenFindPane(hdParts_c*)
 ext 026D4D68 void titleControllerScreenConnectReceivers(hdParts_c*)
 ext 026D4E28 void titleControllerScreenBindSounds(hdParts_c*)
 ext 02005674 void titleControllerScreenSetAnimationFrame(hdAnimSet*,u32,u32)
 data sControllerGlobalList 1049A9EC
 data sControllerGlobalRegistration 101F6A4C
 data sControllerMinimumAngle 1049A9D0
 data sControllerMaximumAngle 1049A9D4
 data sControllerMinimumValue 10102CF8
 data sControllerMaximumValue 10102CFC
 data sControllerPrimaryFlag 1049A9D8
 data sControllerSecondaryFlag 1049A9D9
 data sControllerPrimaryRegistration 101F6A58
 data sControllerSecondaryRegistration 101F6A64
 data sControllerGlobalStringTable 10102B94
 data sControllerResourceNames 1049AA1C
 data sControllerCounterGuard 101FD8E4
 data sControllerStateCounter 101FDD50
 data sControllerIntroDescriptor 1049AA44
 data sControllerInDescriptor 1049AA74
 data sControllerOutDescriptor 1049AAA4
 data sControllerPrimaryMissingDescriptor 1049AB08
 data sControllerSecondaryMissingDescriptor 1049AB34
 data sControllerDirectStateTable 10102BF4
 data sControllerVirtualStateTable 10102C3C
 data sControllerNullState 101FF32C
 str 10102D64 in 026D5364
 str 10102D00 in 026D5364
 str 10102D08 in 026D5364
 str 10102D70 in 026D5364
 str 10102D7C in 026D5364
 str 10102D20 in 026D5364
 str 10102D88 in 026D5364
 str 10102D9C in 026D5364
 str 10102D50 in 026D5364
 str 10102D10 in 026D5364
 str 10102DAC in 026D5364
 str 10102DC0 in 026D5364
 str 10102D34 in 026D5364
 ext 028F026C void titleControllerRegisterGlobals(void*)
 ext 028ED6F8 void titleControllerConstructPrimaryFlag(void*)
 ext 028EAB2C void titleControllerConstructSecondaryFlag(void*)
 data sControllerGroupNamesInitializer 1049A9C8
 data sControllerAnimationNamesInitializer 1049A9FC
 data sControllerChildNamesInitializer 1049AA0C
 data sControllerScreenArchivesInitializer 1049AA2C
 data sControllerResourceNamesInitializer 1049AA1C
 data sControllerScreenLayoutNameInitializer 1049AAD4
 data sControllerDisplayParent 1049DD90
*/

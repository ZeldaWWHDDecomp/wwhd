/* HD start-sequence background state transitions; private binary-derived source. */
#include "d/hd_title_parts.h"

static hdPartsStateId_c sBackgroundHeaderState;
static hdPartsStateId_c sBackgroundCaptionState;
static hdPartsStateId_c sBackgroundDisplayState;
static hdScreenUiSystem* sBackgroundUiSystem;
struct hdTitleBackgroundAnimationGroups_l { be<u32> mIndices[5]; };
static hdTitleBackgroundAnimationGroups_l sBackgroundAnimationGroups;
static hdUiSafeString sBackgroundAnimationNames;
static hdUiSafeString sBackgroundGroupNames;
extern void titleFinalizeScreen(hdParts_c*);
extern void titleUpdateStateMachine(hdStateMachine*);
extern void* titleBackgroundAllocateLayout(u32, hdScreenHeap*, u32);
extern hdPartsLayout_c* titleBackgroundConstructLayout(hdPartsLayout_c*);
extern void titleBackgroundBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

/* 026E54D4 */
void hdTitleStartBackground_c::finalizeParts() {
    vDestroyLayout();
    vDeleteChildren();
    vDestroyLayout();
    titleFinalizeScreen(this);
}

/* 026E553C */
void hdTitleStartBackground_c::updateState() {
    titleUpdateStateMachine(stateMachine());
}

/* 026E5544 */
void hdTitleStartBackground_c::showHeader() {
    auto* root = reinterpret_cast<hdTitleBackgroundPane_l*>(mpLayout->lyt()->mpRootPane.get());
    root->mFlags = (root->mFlags & 0xFE) + 1;
    changeState(&sBackgroundDisplayState);
}

/* 026E5570 */
void hdTitleStartBackground_c::requestHeaderState() {
    changeState(&sBackgroundHeaderState);
}

/* 026E5580 */
void hdTitleStartBackground_c::hideHeader() {
    auto* root = reinterpret_cast<hdTitleBackgroundPane_l*>(mpLayout->lyt()->mpRootPane.get());
    root->mFlags = root->mFlags & 0xFE;
}

/* 026E559C */
void hdTitleStartBackground_c::setCaptionIndex(u32 index) {
    if (index == u32(mCaptionIndex)) return;
    mCaptionIndex = index;
    changeState(&sBackgroundCaptionState);
}

/* 026E55C4 */
u32 hdTitleStartBackground_c::loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    auto* layout = static_cast<hdPartsLayout_c*>(titleBackgroundAllocateLayout(sizeof(hdPartsLayout_c), sBackgroundUiSystem->mHeap, 4));
    if (layout != NULL) layout = titleBackgroundConstructLayout(layout);
    mpLayout = layout;
    if (layout == NULL || layout->vLoad(name, arg1, arg2, arg3, 5, 3, desc) == 0) return 0;
    for (u32 i = 0; i < 5; ++i) {
        hdPartsLayout_c* currentLayout = mpLayout;
        u32 group = sBackgroundAnimationGroups.mIndices[i];
        hdAnimSet* animations = currentLayout->animSet();
        auto* animationName = hdPartsView<hdUiSafeString>(gabi::ea(&sBackgroundAnimationNames) + i * sizeof(hdUiSafeString));
        auto* groupName = hdPartsView<hdUiSafeString>(gabi::ea(&sBackgroundGroupNames) + group * sizeof(hdUiSafeString));
        titleBackgroundBindAnimation(animations, i, animationName, groupName);
    }
    vCalcLayout();
    return 1;
}

struct hdTitleBackgroundGlobals_l { be<u32> mHead, mTail, mOwner, mCount; };
struct hdTitleBackgroundFloat_l { be<f32> mValue; };
static hdTitleBackgroundGlobals_l sBackgroundGlobals;
static u8 sBackgroundGlobalsRegistration;
static hdTitleBackgroundFloat_l sBackgroundMinimumAngle;
static hdTitleBackgroundFloat_l sBackgroundMaximumAngle;
static hdTitleBackgroundFloat_l sBackgroundMinimumValue;
static hdTitleBackgroundFloat_l sBackgroundMaximumValue;
static u8 sBackgroundPrimaryFlag;
static u8 sBackgroundSecondaryFlag;
static u8 sBackgroundPrimaryRegistration;
static u8 sBackgroundSecondaryRegistration;
static hdUiStringTable sBackgroundGlobalStringTable;
static hdUiSafeString sBackgroundGlobalArchive;
static hdUiSafeString sBackgroundGlobalLayout;
static hdUiSafeString sBackgroundCaptionNames;
static hdPartsStateCounter_c sBackgroundCounterGuard;
static hdPartsStateCounter_c sBackgroundStateCounter;
static hdPartsStateId_c sBackgroundWaitState;
static hdPartsStateId_c sBackgroundEndState;
static hdPartsStateIdVtbl_c sBackgroundStateTable;
static hdPartsStateIdVtbl_c sBackgroundEndTable;
static hdStateId sBackgroundNullState;
extern void titleBackgroundRegisterGlobals(void*);
extern void titleBackgroundConstructPrimaryFlag(void*);
extern void titleBackgroundConstructSecondaryFlag(void*);

enum hdTitleBackgroundEndDelegate {
    BACKGROUND_END_ENTER = 0x026E5B80,
    BACKGROUND_END_EXECUTE = 0x026E5B84,
    BACKGROUND_END_EXIT = 0x026E5B88,
    BACKGROUND_END_UNUSED = 0x026E5B8C,
};

struct hdTitleBackgroundGroupNamesNames_l { hdUiSafeString mNames[3]; };
static hdTitleBackgroundGroupNamesNames_l sBackgroundGroupNamesInitializer;
struct hdTitleBackgroundAnimationNamesNames_l { hdUiSafeString mNames[5]; };
static hdTitleBackgroundAnimationNamesNames_l sBackgroundAnimationNamesInitializer;
struct hdTitleBackgroundCaptionNamesNames_l { hdUiSafeString mNames[13]; };
static hdTitleBackgroundCaptionNamesNames_l sBackgroundCaptionNamesInitializer;

/* 026E56F0 */
static void initializeTitleBackgroundGlobals() {
    sBackgroundGlobals.mCount = 0;
    sBackgroundGlobals.mOwner = 0;
    sBackgroundGlobals.mTail = 0;
    sBackgroundGlobals.mHead = 0;
    titleBackgroundRegisterGlobals(&sBackgroundGlobalsRegistration);
    f32 minimum = sBackgroundMinimumValue.mValue;
    f32 maximum = sBackgroundMaximumValue.mValue;
    sBackgroundMinimumAngle.mValue = minimum;
    sBackgroundMaximumAngle.mValue = maximum;
    titleBackgroundConstructPrimaryFlag(&sBackgroundPrimaryFlag);
    titleBackgroundRegisterGlobals(&sBackgroundPrimaryRegistration);
    titleBackgroundConstructSecondaryFlag(&sBackgroundSecondaryFlag);
    titleBackgroundRegisterGlobals(&sBackgroundSecondaryRegistration);
    {
        auto* name = &sBackgroundGroupNamesInitializer.mNames[0];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("G_All_00");
    }
    {
        auto* name = &sBackgroundGroupNamesInitializer.mNames[1];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("G_LoopAll_00");
    }
    {
        auto* name = &sBackgroundGroupNamesInitializer.mNames[2];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("G_Header_00");
    }
    {
        auto* name = &sBackgroundAnimationNamesInitializer.mNames[0];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("InAll");
    }
    {
        auto* name = &sBackgroundAnimationNamesInitializer.mNames[1];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("LoopAll");
    }
    {
        auto* name = &sBackgroundAnimationNamesInitializer.mNames[2];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("InHeader");
    }
    {
        auto* name = &sBackgroundAnimationNamesInitializer.mNames[3];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("OutHeader");
    }
    {
        auto* name = &sBackgroundAnimationNamesInitializer.mNames[4];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("OutAll");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[0];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_00_SelectCont");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[1];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_00_Save_00");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[2];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_00_Save_01");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[3];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_00_Save_02");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[4];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_00_Save_03");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[5];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_00_Save_04");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[6];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_00_Save_05");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[7];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_00_Save_06");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[8];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_00_Save_07");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[9];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_Second00");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[10];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_Second01");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[11];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_Second02");
    }
    {
        auto* name = &sBackgroundCaptionNamesInitializer.mNames[12];
        name->mVtable = &sBackgroundGlobalStringTable;
        name->mText.v = gabi::guest_addr("T_Header_Second03");
    }
    sBackgroundGlobalArchive.mVtable = &sBackgroundGlobalStringTable;
    sBackgroundGlobalArchive.mText.v = gabi::guest_addr("StartSequeneBG_00");
    sBackgroundGlobalLayout.mVtable = &sBackgroundGlobalStringTable;
    sBackgroundGlobalLayout.mText.v = gabi::guest_addr("StartSequeneBG_00");
    u32 number = 0;
    if (sBackgroundCounterGuard.mValue != 0) number = sBackgroundStateCounter.mValue;
    else sBackgroundCounterGuard.mValue = 1;
    sBackgroundDisplayState.setup(++number, "::StateID_InAll", &sBackgroundStateTable, 25, &sBackgroundNullState);
    sBackgroundCaptionState.setup(++number, "::StateID_ChangeText", &sBackgroundStateTable, 28, &sBackgroundNullState);
    sBackgroundWaitState.setup(++number, "::StateID_Wait", &sBackgroundStateTable, 31, &sBackgroundNullState);
    sBackgroundHeaderState.setup(++number, "::StateID_OutAll", &sBackgroundStateTable, 34, &sBackgroundNullState);
    sBackgroundStateCounter.mValue = ++number;
    sBackgroundEndState.mNumber = number;
    sBackgroundEndState.mName.v = gabi::guest_addr("::StateID_OutAllEnd");
    sBackgroundEndState.mVtable = &sBackgroundEndTable;
    sBackgroundEndState.mEnter.mThisDelta = 0;
    sBackgroundEndState.mEnter.mVirtualIndex = -1;
    sBackgroundEndState.mEnter.mFunction = BACKGROUND_END_ENTER;
    sBackgroundEndState.mExecute.mThisDelta = 0;
    sBackgroundEndState.mExecute.mVirtualIndex = -1;
    sBackgroundEndState.mExecute.mFunction = BACKGROUND_END_EXECUTE;
    sBackgroundEndState.mExit.mThisDelta = 0;
    sBackgroundEndState.mExit.mVirtualIndex = -1;
    sBackgroundEndState.mExit.mFunction = BACKGROUND_END_EXIT;
    sBackgroundEndState.mUnused.mThisDelta = 0;
    sBackgroundEndState.mUnused.mVirtualIndex = -1;
    sBackgroundEndState.mUnused.mFunction = BACKGROUND_END_UNUSED;
}

/* WWHD glue (tools/verify/autoglue.py): start-sequence state descriptors
 data sBackgroundHeaderState 1049C570
 data sBackgroundCaptionState 1049C510
 data sBackgroundDisplayState 1049C4E0
 data sBackgroundUiSystem 1018C404
 data sBackgroundAnimationGroups 101069C8
 data sBackgroundAnimationNames 1049C44C
 data sBackgroundGroupNames 1049C434
 ext 026F91F8 void titleFinalizeScreen(hdParts_c*)
 ext 02006364 void titleUpdateStateMachine(hdStateMachine*)
 ext 0273B050 void* titleBackgroundAllocateLayout(u32,hdScreenHeap*,u32)
 ext 026FADB0 hdPartsLayout_c* titleBackgroundConstructLayout(hdPartsLayout_c*)
 ext 02004E04 void titleBackgroundBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 data sBackgroundGlobals 1049C424
 data sBackgroundGlobalsRegistration 101F6ED4
 data sBackgroundMinimumAngle 1049C418
 data sBackgroundMaximumAngle 1049C41C
 data sBackgroundMinimumValue 10106AD8
 data sBackgroundMaximumValue 10106ADC
 data sBackgroundPrimaryFlag 1049C420
 data sBackgroundSecondaryFlag 1049C421
 data sBackgroundPrimaryRegistration 101F6EE0
 data sBackgroundSecondaryRegistration 101F6EEC
 data sBackgroundGlobalStringTable 101069B0
 data sBackgroundGlobalArchive 1049C410
 data sBackgroundGlobalLayout 1049C5A0
 data sBackgroundCaptionNames 1049C474
 data sBackgroundCounterGuard 101FD8E4
 data sBackgroundStateCounter 101FDD50
 data sBackgroundWaitState 1049C540
 data sBackgroundEndState 1049C5A8
 data sBackgroundStateTable 10106A6C
 data sBackgroundEndTable 10106A24
 data sBackgroundNullState 101FF32C
 str 10106C38 in 026E56F0
 str 10106C44 in 026E56F0
 str 10106B08 in 026E56F0
 str 10106AE8 in 026E56F0
 str 10106AE0 in 026E56F0
 str 10106C54 in 026E56F0
 str 10106BC8 in 026E56F0
 str 10106AF0 in 026E56F0
 str 10106C60 in 026E56F0
 str 10106B14 in 026E56F0
 str 10106B28 in 026E56F0
 str 10106B3C in 026E56F0
 str 10106B50 in 026E56F0
 str 10106B64 in 026E56F0
 str 10106B78 in 026E56F0
 str 10106B8C in 026E56F0
 str 10106BA0 in 026E56F0
 str 10106BE8 in 026E56F0
 str 10106BFC in 026E56F0
 str 10106C10 in 026E56F0
 str 10106C24 in 026E56F0
 str 10106BD4 in 026E56F0
 str 10106AF8 in 026E56F0
 str 10106C78 in 026E56F0
 str 10106C90 in 026E56F0
 str 10106CA0 in 026E56F0
 str 10106BB4 in 026E56F0
 ext 028F026C void titleBackgroundRegisterGlobals(void*)
 ext 028ED6F8 void titleBackgroundConstructPrimaryFlag(void*)
 ext 028EAB2C void titleBackgroundConstructSecondaryFlag(void*)
 data sBackgroundGroupNamesInitializer 1049C434
 data sBackgroundAnimationNamesInitializer 1049C44C
 data sBackgroundCaptionNamesInitializer 1049C474
*/

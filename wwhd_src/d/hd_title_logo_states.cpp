/* HD title logo state operations; private binary-derived source. */
#include "d/hd_title_parts.h"

static hdScreenUiSystem* sTitleLogoUiSystem;
struct hdTitleLogoAnimationGroups_l { be<u32> mIndices[4]; };
static hdTitleLogoAnimationGroups_l sTitleLogoAnimationGroups;
static hdUiSafeString sTitleLogoAnimationNames;
static hdUiSafeString sTitleLogoGroupNames;
extern void titleFinalizeLogoScreen(hdParts_c*);
extern void titleUpdateLogoStateMachine(hdStateMachine*);
extern void* titleLogoAllocateLayout(u32, hdScreenHeap*, u32);
extern hdPartsLayout_c* titleLogoConstructLayout(hdPartsLayout_c*);
extern void titleLogoBindAnimation(hdAnimSet*, u32, hdUiSafeString*, hdUiSafeString*);

static hdUiSafeString sTitleLogoArchiveName;
static hdUiSafeString sTitleLogoLayoutName;
static hdPartsStateId_c sTitleLogoInitialState;
extern void titleLogoAttachArchives(hdParts_c*, hdUiSafeString*, u32);
extern void titleLogoSetAnimationFrame(hdAnimSet*, u32, u32, f32);
extern void titleLogoFindTextPanes(hdParts_c*);
extern void titleLogoFindSpecialPanes(hdParts_c*);
extern void titleLogoFindLogoPanes(hdParts_c*);

/* 026EB750 */
u32 hdTitleLogo_c::initialize(u32 arg1, u32 arg2, hdPartsDesc_c* desc) {
    titleLogoAttachArchives(this, &sTitleLogoArchiveName, 1);
    if (vLoad(&sTitleLogoLayoutName, gabi::ea(mpResAccessor.get()), arg1, arg2, desc) == 0)
        return 0;
    titleLogoSetAnimationFrame(animSet(), 0, 1, 0.0f);
    vCalcLayout();
    titleLogoFindTextPanes(this);
    titleLogoFindSpecialPanes(this);
    titleLogoFindLogoPanes(this);
    auto* special = reinterpret_cast<hdTitleBackgroundPane_l*>(mSpecialPanes[2].get());
    special->mFlags = special->mFlags & 0xFE;
    auto* hiddenText = reinterpret_cast<hdTitleBackgroundPane_l*>(mTextPanes[1].get());
    hiddenText->mFlags = hiddenText->mFlags & 0xFE;
    auto* shownText = reinterpret_cast<hdTitleBackgroundPane_l*>(mTextPanes[2].get());
    shownText->mFlags = (shownText->mFlags & 0xFE) + 1;
    startState(&sTitleLogoInitialState);
    return 1;
}

struct hdTitleLogoGlobalList_l {
    be<u32> mHead, mTail, mOwner, mCount;
};
struct hdTitleLogoFloat_l { be<f32> mValue; };
struct hdTitleLogoFloatPair_l { be<f32> mX, mY; };
struct hdTitleLogoCounterGuard_l { be<u32> mInitialized; };
static hdTitleLogoGlobalList_l sLogoGlobals;
static u8 sLogoGlobalsRegistration;
static hdTitleLogoFloat_l sLogoMinimumAngle;
static hdTitleLogoFloat_l sLogoMaximumAngle;
static hdTitleLogoFloat_l sLogoMinimumAngleValue;
static hdTitleLogoFloat_l sLogoMaximumAngleValue;
static u8 sLogoPrimaryFlag;
static u8 sLogoSecondaryFlag;
static u8 sLogoPrimaryFlagRegistration;
static u8 sLogoSecondaryFlagRegistration;
static hdUiStringTable sLogoGlobalsStringTable;
static hdTitleLogoFloatPair_l sLogoTextPosition;
static hdTitleLogoFloatPair_l sLogoJapanesePosition;
static hdTitleLogoFloatPair_l sLogoEnglishPosition;
static hdPartsStateCounter_c sLogoStateNumberOffset;
static hdPartsStateCounter_c sLogoAnimationBaseNumber;
static hdTitleLogoCounterGuard_l sLogoStateCounterGuard;
static hdPartsStateCounter_c sLogoStateCounter;
static hdPartsStateId_c sLogoIntroStateDescriptor;
static hdPartsStateId_c sLogoLoopStateDescriptor;
static hdPartsStateIdVtbl_c sLogoStateDescriptorTable;
static hdStateId sLogoNullState;
extern void titleLogoRegisterGlobals(void*);
extern void titleLogoConstructPrimaryFlag(void*);
extern void titleLogoConstructSecondaryFlag(void*);

struct hdTitleLogoTitleLogoGroupNamesNames_l { hdUiSafeString mNames[3]; };
static hdTitleLogoTitleLogoGroupNamesNames_l sTitleLogoGroupNamesInitializer;
struct hdTitleLogoTitleLogoAnimationNamesNames_l { hdUiSafeString mNames[4]; };
static hdTitleLogoTitleLogoAnimationNamesNames_l sTitleLogoAnimationNamesInitializer;

/* 026EBA20 */
static void initializeTitleLogoGlobals() {
    sLogoGlobals.mCount = 0;
    sLogoGlobals.mOwner = 0;
    sLogoGlobals.mTail = 0;
    sLogoGlobals.mHead = 0;
    titleLogoRegisterGlobals(&sLogoGlobalsRegistration);
    f32 minimum = sLogoMinimumAngleValue.mValue;
    f32 maximum = sLogoMaximumAngleValue.mValue;
    sLogoMinimumAngle.mValue = minimum;
    sLogoMaximumAngle.mValue = maximum;
    titleLogoConstructPrimaryFlag(&sLogoPrimaryFlag);
    titleLogoRegisterGlobals(&sLogoPrimaryFlagRegistration);
    titleLogoConstructSecondaryFlag(&sLogoSecondaryFlag);
    titleLogoRegisterGlobals(&sLogoSecondaryFlagRegistration);
    {
        auto* name = &sTitleLogoGroupNamesInitializer.mNames[0];
        name->mVtable = &sLogoGlobalsStringTable;
        name->mText.v = gabi::guest_addr("G_LoopText_00");
    }
    {
        auto* name = &sTitleLogoGroupNamesInitializer.mNames[1];
        name->mVtable = &sLogoGlobalsStringTable;
        name->mText.v = gabi::guest_addr("G_All_00");
    }
    {
        auto* name = &sTitleLogoGroupNamesInitializer.mNames[2];
        name->mVtable = &sLogoGlobalsStringTable;
        name->mText.v = gabi::guest_addr("G_LoopTitleLogo_00");
    }
    {
        auto* name = &sTitleLogoAnimationNamesInitializer.mNames[0];
        name->mVtable = &sLogoGlobalsStringTable;
        name->mText.v = gabi::guest_addr("InAll");
    }
    {
        auto* name = &sTitleLogoAnimationNamesInitializer.mNames[1];
        name->mVtable = &sLogoGlobalsStringTable;
        name->mText.v = gabi::guest_addr("InShort");
    }
    {
        auto* name = &sTitleLogoAnimationNamesInitializer.mNames[2];
        name->mVtable = &sLogoGlobalsStringTable;
        name->mText.v = gabi::guest_addr("LoopText");
    }
    {
        auto* name = &sTitleLogoAnimationNamesInitializer.mNames[3];
        name->mVtable = &sLogoGlobalsStringTable;
        name->mText.v = gabi::guest_addr("LoopTitleLogo");
    }
    sLogoTextPosition.mX = 0.0f;
    sLogoTextPosition.mY = 0.0f;
    sTitleLogoLayoutName.mVtable = &sLogoGlobalsStringTable;
    sTitleLogoLayoutName.mText.v = gabi::guest_addr("Title_00");
    sTitleLogoArchiveName.mVtable = &sLogoGlobalsStringTable;
    sTitleLogoArchiveName.mText.v = gabi::guest_addr("Title_00");
    u32 animationBase = sLogoAnimationBaseNumber.mValue;
    sLogoEnglishPosition.mY = -15.0f;
    sLogoEnglishPosition.mX = -72.0f;
    sLogoStateNumberOffset.mValue = animationBase + 21;
    sLogoJapanesePosition.mY = -44.0f;
    sLogoJapanesePosition.mX = -28.0f;
    u32 number = 0;
    if (sLogoStateCounterGuard.mInitialized != 0) number = sLogoStateCounter.mValue;
    else sLogoStateCounterGuard.mInitialized = 1;
    sTitleLogoInitialState.setup(++number, "::StateID_Wait", &sLogoStateDescriptorTable, 25, &sLogoNullState);
    sLogoIntroStateDescriptor.setup(++number, "::StateID_InAll", &sLogoStateDescriptorTable, 28, &sLogoNullState);
    sLogoStateCounter.mValue = ++number;
    sLogoLoopStateDescriptor.setup(number, "::StateID_LoopText", &sLogoStateDescriptorTable, 31, &sLogoNullState);
}

/* 026EB8B4 */
void hdTitleLogo_c::finalizeParts() {
    vDestroyLayout();
    titleFinalizeLogoScreen(this);
}

/* 026EB8F4 */
void hdTitleLogo_c::updateState() {
    titleUpdateLogoStateMachine(stateMachine());
}

/* 026EB8FC */
u32 hdTitleLogo_c::loadLayout(void* name, u32 arg1, u32 arg2, u32 arg3, void* desc) {
    auto* layout = static_cast<hdPartsLayout_c*>(titleLogoAllocateLayout(sizeof(hdPartsLayout_c), sTitleLogoUiSystem->mHeap, 4));
    if (layout != NULL) layout = titleLogoConstructLayout(layout);
    mpLayout = layout;
    if (layout == NULL) return 0;
    layout->vLoad(name, arg1, arg2, arg3, 4, 3, desc);
    for (u32 i = 0; i < 4; ++i) {
        hdPartsLayout_c* currentLayout = mpLayout;
        u32 group = sTitleLogoAnimationGroups.mIndices[i];
        hdAnimSet* animations = currentLayout->animSet();
        auto* animationName = hdPartsView<hdUiSafeString>(gabi::ea(&sTitleLogoAnimationNames) + i * sizeof(hdUiSafeString));
        auto* groupName = hdPartsView<hdUiSafeString>(gabi::ea(&sTitleLogoGroupNames) + group * sizeof(hdUiSafeString));
        titleLogoBindAnimation(animations, i, animationName, groupName);
    }
    vCalcLayout();
    return 1;
}

/* WWHD glue (tools/verify/autoglue.py): title logo state operations
 data sTitleLogoUiSystem 1018C404
 data sTitleLogoAnimationGroups 1010808C
 data sTitleLogoAnimationNames 1049CF48
 data sTitleLogoGroupNames 1049CF30
 ext 026F91F8 void titleFinalizeLogoScreen(hdParts_c*)
 ext 02006364 void titleUpdateLogoStateMachine(hdStateMachine*)
 ext 0273B050 void* titleLogoAllocateLayout(u32,hdScreenHeap*,u32)
 ext 026FADB0 hdPartsLayout_c* titleLogoConstructLayout(hdPartsLayout_c*)
 ext 02004E04 void titleLogoBindAnimation(hdAnimSet*,u32,hdUiSafeString*,hdUiSafeString*)
 data sTitleLogoArchiveName 1049CE74
 data sTitleLogoLayoutName 1049D004
 data sTitleLogoInitialState 1049CF74
 ext 026F90D8 void titleLogoAttachArchives(hdParts_c*,hdUiSafeString*,u32)
 ext 0200552C void titleLogoSetAnimationFrame(hdAnimSet*,u32,u32,f32)
 ext 026EAFE8 void titleLogoFindTextPanes(hdParts_c*)
 ext 026EB250 void titleLogoFindSpecialPanes(hdParts_c*)
 ext 026EB4E8 void titleLogoFindLogoPanes(hdParts_c*)
 data sLogoGlobals 1049CF20
 data sLogoGlobalsRegistration 101F700C
 data sLogoMinimumAngle 1049CE7C
 data sLogoMaximumAngle 1049CE80
 data sLogoMinimumAngleValue 1010826C
 data sLogoMaximumAngleValue 10108270
 data sLogoPrimaryFlag 1049CE84
 data sLogoSecondaryFlag 1049CE85
 data sLogoPrimaryFlagRegistration 101F7018
 data sLogoSecondaryFlagRegistration 101F7024
 data sLogoGlobalsStringTable 10108074
 data sLogoTextPosition 1049D014
 data sLogoJapanesePosition 1049D00C
 data sLogoEnglishPosition 1049D01C
 data sLogoStateNumberOffset 1049D024
 data sLogoAnimationBaseNumber 101F7030
 data sLogoStateCounterGuard 101FD8E4
 data sLogoStateCounter 101FDD50
 data sLogoIntroStateDescriptor 1049CFA4
 data sLogoLoopStateDescriptor 1049CFD4
 data sLogoStateDescriptorTable 101080E4
 data sLogoNullState 101FF32C
 str 101082A4 in 026EBA20
 str 101082C4 in 026EBA20
 str 101082D0 in 026EBA20
 str 1010828C in 026EBA20
 str 10108284 in 026EBA20
 str 101082E4 in 026EBA20
 str 101082B4 in 026EBA20
 str 101082F0 in 026EBA20
 str 101082FC in 026EBA20
 str 10108294 in 026EBA20
 str 1010830C in 026EBA20
 ext 028F026C void titleLogoRegisterGlobals(void*)
 ext 028ED6F8 void titleLogoConstructPrimaryFlag(void*)
 ext 028EAB2C void titleLogoConstructSecondaryFlag(void*)
 data sTitleLogoGroupNamesInitializer 1049CF30
 data sTitleLogoAnimationNamesInitializer 1049CF48
*/
